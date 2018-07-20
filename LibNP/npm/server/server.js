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

var redis = require('redis');

function NPServer()
{
    EventEmitter.call(this);

    this.database = require('./database');
    this.database.start();
    
    this.webapi = require('./webapi');
    this.webapi.start();
    
    var self = this;
    
    this.clientsByID = {};
    this.anonymousNPID = 0;
    
    this.clients = {};
    
    this.sessions = {};
    this.sessionID = 0;
    
    this.redis = redis.createClient();
    this.redis.select(1);
    
    this.server = net.createServer(function(c)
    {
        var conn = new ClientConnection(self);
        
        self.clients[conn.connID] = conn;
        
        conn.on('message', function(cn, message)
        {
            self.handleMessage(cn, message);
        });
        
        conn.on('closed', function()
        {
            //delete self.clientsByID[conn.npID];
            NPServer.deleteConnection(conn);
        });
        
        conn.bindTo(c);
    });
	/*setInterval(function()
	{
		var buf = new Buffer(4);
		buf.writeUInt32LE(0xFACEFEED, 0);
		Object.keys(NPServer.instance.clientsByID).forEach(function (key)
		{
			NPServer.instance.clientsByID[key].c.write(buf);
		});
	}, 1000 * 120); // every 2 minutes*/
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

    this.server.listen(3036);
    
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
    
    var data = null;
    
    if (message.messageBuffer !== undefined)
    {
        data = parsers[type](message.messageBuffer);
        
        if (!data)
        {
            logger.warn('failed to parse message of type ' + type);
            return;
        }
    }
    else
    {
        data = message.messageData;
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

NPServer.getClientsByID = function(npid)
{
    if (!(npid in NPServer.instance.clientsByID))
    {
        return [];
    }
    
    if (NPServer.instance.clientsByID[npid].length == 0)
    {
        return [];
    }

    var clientIDs = NPServer.instance.clientsByID[npid];
    var clientConns = [];
    
    clientIDs.forEach(function(client)
    {
        clientConns.push(NPServer.instance.clients[client]);
    });
    
    return clientConns;
};

NPServer.deleteConnection = function(conn)
{
    var id = conn.connID;
    
    if (conn.npID)
    {
        if (conn.npID in NPServer.instance.clientsByID)
        {
            var clients = NPServer.instance.clientsByID[conn.npID];
        
            var idx = clients.indexOf(id);
            
            if (idx != -1)
            {
                clients.splice(idx, 1);
                
                if (clients.length == 0)
                {
                    delete NPServer.instance.clientsByID[conn.npID];
                }
            }
        }
    }
    
    delete NPServer.instance.clients[id];
};

NPServer.bindConnection = function(conn, npid)
{
    if (!(npid in NPServer.instance.clientsByID))
    {
        NPServer.instance.clientsByID[npid] = [];
    }
    
    NPServer.instance.clientsByID[npid].push(conn.connID);
};