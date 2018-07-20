var server = require('./../server').NPServer;
var int64 = require('node-int64');

var log4js = require('log4js');
var logger = log4js.getLogger('auth');

module.exports = function(data, id)
{
    var reply =
    {
        result: 1,
        npid: '0',
        sessionToken: new Buffer(16)
    };
    
    var connection = this;
    
    var doReply = function()
    {   
        if (reply.result == 0)
        {
            server.instance.clientsByID[reply.npid] = connection;
                
            connection.npID = reply.npid;
            connection.group = 0;
            connection.authenticated = true;
            connection.sessionToken = '';
            connection.isServer = true;
            
            server.instance.emit('authenticated', connection);
                
            connection.on('closed', function()
            {
                server.instance.emit('client_closed', connection);
            });
        }
        
        connection.reply('RPCAuthenticateResultMessage', id, reply);
    };

    if (data.licenseKey != "")
    {
        server.instance.database.mysql(function(err, connection)
        {
            if (err)
            {
                logger.error('Error getting DB: ' + err);
            
                reply.result = 2; // service unavailable
                doReply();
            }
            else
            {
                connection.query('SELECT id FROM licensekeys WHERE `key` = ?', [ data.licenseKey ], function(err, rows)
                {
                    if (err)
                    {
                        logger.error('Error getting key: ' + err);
            
                        reply.result = 2; // service unavailable
                        doReply();
                    }
                    else
                    {
                        if (rows.length == 0)
                        {
                            reply.result = 1;
                        }
                        else
                        {
                            reply.result = 0;
                            reply.npid = (new int64(0x1200001, rows[0].id).toOctetString()).substring(1);
                        }
                        
                        doReply();
                    }
                    
                    connection.end();                    
                });
            }
        });
    }
    else
    {
        reply.result = 0;
        reply.npid = (new int64(0x1300001, ++server.instance.anonymousNPID).toOctetString()).substring(1);
        
        if (server.instance.anonymousNPID >= 0x70000000)
        {
            server.instance.anonymousNPID = 0;
        }
        
        doReply();
    }
};