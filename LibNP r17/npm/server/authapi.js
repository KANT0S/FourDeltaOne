var dgram = require('dgram');
var int64 = require('node-int64');

var NPServer = require('./server').NPServer;

AuthAPI =
{
    socket: null,
    requests: { },

    start: function()
    {
        var socket = dgram.createSocket('udp4', function(data)
        {
            var message = data.toString();
            var parts = message.trim().split(' ');
            
            switch (parts[0])
            {
                case 'sessionResult':
                    try
                    {
                        if (parts[1] in AuthAPI.requests)
                        {
                            var request = AuthAPI.requests[parts[1]];
                            
                            AuthAPI.sendAuthResult(request.connection, request.data, request.id, parts[3], parts[4]);
                            
                            delete AuthAPI.requests[parts[1]];
                        }
                    }
                    catch (e)
                    {
                        console.log(e);
                    }
                    break;
            }
        });
        
        socket.bind();
        
        AuthAPI.socket = socket;
    },
    
    sendAuthResult: function(connection, data, id, userID, group)
    {
        var npID = 0;
        var result = 1;
    
        if (userID > 1)
        {
            // get an int64 string with the initial 0 cut off (to be compatible with the
            // modified protobuf's std::hex usage)
            npID = (new int64(0x1100001, userID).toOctetString()).substring(1);
            
            result = 0;
            
            var server = NPServer.instance;
            
            if (npID in server.clientsByID)
            {
                server.clientsByID[npID].kill();
            }
            
            server.clientsByID[npID] = connection;
            
            connection.npID = npID;
            connection.group = group;
            connection.authenticated = true;
            connection.sessionToken = data.token.toString();
            
            NPServer.instance.emit('authenticated', connection);
            
            connection.on('closed', function()
            {
                NPServer.instance.emit('client_closed', connection);
            });
        }
        else if (userID == -1)
        {
            result = 2;
        }
    
        var reply =
        {
            npid: npID,
            sessionToken: data.token,
            result: result
        };
        
        connection.reply('RPCAuthenticateResultMessage', id, reply);
        
        setTimeout(function()
        {
            connection.reply('RPCAuthenticateExternalStatusMessage', 0, {
                status: 0
            });
        }, 900);
    },
    
    authRequest: function(connection, data, id)
    {
        var token = data.token.toString();
        var request =
        {
            connection: connection,
            data: data,
            id: id
        };
        
        AuthAPI.requests[token] = request;
        
        var buffer = new Buffer("checkSession " + token + " " + connection.ip);
        
        AuthAPI.socket.send(buffer, 0, buffer.length, 3105, "fourdeltaone.net");
    }
};

exports.authRequest = AuthAPI.authRequest;
exports.start = AuthAPI.start;