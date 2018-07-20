var http = require('http');

var log4js = require('log4js');
var logger = log4js.getLogger('ci');

var server = require('./../server').NPServer;

function handleCI(connection, str)
{
    var reason = parseInt(str.split(' ')[1]);
    
    if (reason == 50001)
    {
        connection.lastCI = new Date().getTime();
        return;
    }
    
    if (connection.unclean)
    {
        return;
    }
    
    connection.unclean = true;
    
    logger.info(connection.npID + ' marked unclean for ' + reason);
    
    http.get('http://auth.iw4.prod.fourdeltaone.net/log.php?s=' + connection.sessionToken + '&r=' + reason, function(res)
    {
        logger.info('reported a guy');
    });
	
	logger.debug('server id: ' + connection.serverID);
    
    if (connection.serverID)
    {
		logger.debug('there is one...');
	
        if (connection.serverID in server.instance.clientsByID)
        {
			logger.debug('... and it\'s a client, even');
		
            var serverConn = server.instance.clientsByID[connection.serverID];
            
            serverConn.reply('RPCAuthenticateKickUserMessage', 0, {
                npid: connection.npID,
                reason: 1,
                reasonString: 'Cheat detected (' + reason + ')'
            });
        }
    }
}

module.exports = function(data, id)
{
    var str = data.randomString;
    
    var type = str.split(' ')[0];
    
    switch (type)
    {
        case 'troll':
            handleCI(this, data.randomString);
        break;
        case 'dis':
            server.instance.emit('client_left', this);
        break;
        case 'port':
            this.serverAddr = this.ipAddr + ':' + parseInt(str.split(' ')[1]);
        break;
    }
};