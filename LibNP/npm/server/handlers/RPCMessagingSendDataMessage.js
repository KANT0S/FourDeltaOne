var server = require('./../server').NPServer;

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        return;
    }
    
    var remoteID = data.npid;
    
    server.getClientsByID(remoteID).forEach(function(remoteConn)
    {
        remoteConn.reply('RPCMessagingSendDataMessage', 0, {
            npid: connection.npID,
            data: data.data
        });
    });
};