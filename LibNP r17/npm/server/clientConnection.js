// ==========================================================
// Terminal project
// 
// Component: xnp
// Sub-component: layer1
// Purpose: Client connection handler
//
// Initial author: NTAuthority
// Started: 2013-01-14
// ==========================================================

var log4js = require('log4js');
var logger = log4js.getLogger('ClientConnection');
var hexy = require('hexy');
var sys = require('sys');

var EventEmitter = require('events').EventEmitter;

function ClientConnection(server)
{
    EventEmitter.call(this);
    
    this.npServer = server;
    
    this.messageHeader = { };
    this.messageState = null;
    
    this.isServer = false;
    
    this.lastCI = 0;
    
    this.presenceData = {};
}

sys.inherits(ClientConnection, EventEmitter);

ClientConnection.prototype.bindTo = function(c)
{
    var self = this;

    this.c = c;
    this.ip = c.remoteAddress;
    this.ipAddr = this.ip.split(':')[0];
    
    c.on('data', function(buf)
    {
        if (buf.length > 0)
        {
            var read = buf.length;
            var origin = 0;
            
            while (read > 0)
            {
                if (!self.messageState)
                {
                    if (buf.length < (origin + 16))
                    {
                        return;
                    }

                    self.messageHeader = 
                    {
                        signature: buf.readUInt32LE(origin + 0),
                        length: buf.readUInt32LE(origin + 4),
                        type: buf.readUInt32LE(origin + 8),
                        id: buf.readUInt32LE(origin + 12)
                    };
                    
                    if (self.messageHeader.signature != 0xDEADC0DE)
                    {
                        logger.debug('Signature sent: ' + self.messageHeader.signature);
                        logger.error('Signature doesn\'t match (from ' + c.remoteAddress + ')');
                        
                        return;
                    }
                    
                    self.messageState = 
                    {
                        totalBytes: self.messageHeader.length,
                        readBytes: 0,
                        
                        messageBuffer: new Buffer(self.messageHeader.length),
                        messageType: self.messageHeader.type,
                        messageID: self.messageHeader.id
                    };
                    
                    read -= 16;
                    origin += 16;
                }
                
                var copyLen = Math.min(read, (self.messageState.totalBytes - self.messageState.readBytes));
                
                buf.copy(self.messageState.messageBuffer, self.messageState.readBytes, origin, origin + copyLen);
                self.messageState.readBytes += copyLen;
                origin += copyLen;
                read -= copyLen;
                
                if (self.messageState.readBytes >= self.messageState.totalBytes)
                {
                    self.emit('message', self, self.messageState);
                    
                    self.messageState = null;
                }
            }
        }
    });
    
    c.on('close', function()
    {
        self.emit('closed', self);
        self.closed = true;
    });
	
	c.on('timeout', function()
	{
		c.destroy();
		self.emit('closed', self);
                self.closed = true;
	});
	c.setTimeout(1000 * 60 * 5) // after 5 minutes (heartbeat is 2 minutes)
}

ClientConnection.prototype.reply = function(type, id, reply)
{
    try
    {
        if (!(type in this.npServer.ids))
        {
            throw "no such type";
        }

        var typeID = this.npServer.ids[type];
        var serializer = this.npServer.serializers[typeID];
    
        var data = serializer(reply);
    
        var header = new Buffer(16);
        header.writeUInt32LE(0xDEADC0DE, 0);
        header.writeUInt32LE(data.length, 4);
        header.writeUInt32LE(typeID, 8);
        header.writeUInt32LE(id, 12);
    
        this.c.write(Buffer.concat([header, data], header.length + data.length));
    }
    catch (e)
    {
        logger.error(e);
    }
};

ClientConnection.prototype.kill = function()
{
    if (this.closed) return;

    this.reply('RPCCloseAppMessage', 0, {
        reason: 'Another client connected with the same ID.'
    });
};

module.exports = ClientConnection;
