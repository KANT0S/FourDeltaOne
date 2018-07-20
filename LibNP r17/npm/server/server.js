// ==========================================================
// Terminal project
// 
// Component: xnp
// Sub-component: layer1
// Purpose: NP protocol server
//
// Initial author: NTAuthority
// Started: 2013-01-14
// ==========================================================

var log4js = require('log4js');
var logger = log4js.getLogger('server');

var net = require('net');
var ClientConnection = require('./clientConnection');

var EventEmitter = require('events').EventEmitter;
var sys = require('sys');

function NPServer()
{
    EventEmitter.call(this);

    this.database = require('./database');
    this.database.start();
    
    var self = this;
    
    this.clientsByID = {};
    this.anonymousNPID = 0;
    
    this.server = net.createServer(function(c)
    {
        var conn = new ClientConnection(self);
        
        conn.on('message', function(cn, message)
        {
            self.handleMessage(cn, message);
        });
        
        conn.on('closed', function()
        {
            delete self.clientsByID[conn.npID];
        });
        
        conn.bindTo(c);
    });
	setInterval(function()
	{
		var buf = new Buffer(4);
		buf.writeUInt32LE(0xFACEFEED, 0);
		Object.keys(NPServer.instance.clientsByID).forEach(function (key)
		{
			NPServer.instance.clientsByID[key].c.write(buf);
		});
	}, 1000 * 120); // every 2 minutes
}

sys.inherits(NPServer, EventEmitter);

exports.NPServer = NPServer;
NPServer.handlers = {};

var MessageDefinition = require('./messageDefinition');
var parsers = MessageDefinition.parsers;

NPServer.schema = MessageDefinition.schema;
NPServer.ids = MessageDefinition.ids;
NPServer.serializers = MessageDefinition.serializers;

var friends = require('./friends');

NPServer.prototype.start = function()
{
    NPServer.instance = this;
    
    this.ids = NPServer.ids;
    this.serializers = NPServer.serializers;

    this.server.listen(3025);
    
    friends.start();
};

NPServer.prototype.handleMessage = function(connection, message)
{
    var type = message.messageType;
    var id = message.messageID;
    
    if (!(type in parsers))
    {
        logger.warn('no parser for message type ' + type);
        return;
    }
    
    var data = parsers[type](message.messageBuffer);
    
    if (!data)
    {
        logger.warn('failed to parse message of type ' + type);
        return;
    }

    if (!(type in NPServer.handlers))
    {
        logger.warn('no handler for message type ' + type);
        return;
    }
    
    try
    {
        NPServer.handlers[type].call(connection, id, data);
    }
    catch (e)
    {
        logger.error(e);
    }
};