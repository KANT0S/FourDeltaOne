var http = require('http');
var request = require('request');

var log4js = require('log4js');
var logger = log4js.getLogger('ci');

var server = require('./../server').NPServer;

var redisq = require('redisq');
redisq.options({ "redis": {
    "host": "localhost",
    "port": 6379
}});

var thReqQ = redisq.queue('threq');
var thRespQ = redisq.queue('thresp');
var rtReqQ = redisq.queue('rtcgreq');

function handleTheater(connection, id, str)
{
    var data = JSON.parse(str.substring(8));
    
    logger.debug('sending theater query for ' + connection.connID + ', ' + id);
    
    thReqQ.push({
        npid: connection.npID,
        conn: [ connection.connID, id ],
        title: connection.presenceData.currentGame,
        body: data
    });
}

function handleRTCG(connection, id, str)
{
    var data = JSON.parse(str.substring(5));
    
    logger.debug('sending RTCG query for ' + connection.connID + ', ' + id);
    
    rtReqQ.push({
        npid: connection.npID,
        conn: [ connection.connID, id ],
        title: connection.presenceData.currentGame,
        body: data
    });
    
    if (!connection.rtcgHandlerSet)
    {
        connection.on('closed', function()
        {
            rtReqQ.push({
                npid: connection.npID,
                conn: [ connection.connID, 0 ],
                title: connection.presenceData.currentGame,
                body: {
                    type: 'disconnect'
                }
            });
        });
        
        connection.rtcgHandlerSet = true;
    }
}
thRespQ.process(function(task, done)
{
    try
    {
        logger.debug('sending theater response for ' + task.conn[0] + ', ' + task.conn[1]);
    
        if (task.conn[0] in server.instance.clients)
        {
            server.instance.clients[task.conn[0]].reply('RPCStorageSendRandomStringMessage', task.conn[1], {
                randomString: JSON.stringify(task.body)
            });
        }
    }
    catch (e)
    {
        logger.fatal(e);
    }
    
    done(null);
}, 16);

var reqQueue = redisq.queue('cireq');
var respQueue = redisq.queue('ciresp');

function handleCI3(connection, str)
{
    var data = str.split(' ')[1];
    var token = (connection.sessionToken) ? (connection.sessionToken) : '';
    
    //logger.debug('got a ci request...');
    
    //request.post('http://localhost:60321/ci/' + connection.npID + token + '?lastCI=' + connection.lastCI, function(error, response, body)
    reqQueue.push({
        npid: connection.npID,
        token: token,
        lastCI: connection.lastCI,
        body: data
    });
}

respQueue.process(function(task, done)
{
    /*if (error)
    {
        connection.lastCI = new Date().getTime();
        
        return;
    }*/
    
    var body = task;

    /*if (!(body.npid in server.instance.clientsByID))
    {
        logger.info('no such npid: ' + body.npid);

        done(null);
        return;
    }*/

    try
    {
        server.getClientsByID(body.npid).forEach(function(connection)
        {
            if (!connection.native)
            {
                return;
            }
            
            if (connection.unclean)
            {
                done(null);
                return;
            }

            if (body.authorized)
            {
                connection.lastCI = new Date().getTime();
                done(null);
                return;
            }
            
            logger.info(connection.npID + ' auth failed: ' + body.message);
            
            if (body.status > 0)
            {                
                connection.unclean = true;
            }
            
            logger.debug('server id: ' + connection.serverID);

            connection.reply('RPCAuthenticateKickUserMessage', 0, {
                npid: connection.npID,
                reason: 1,
                reasonString: '^5aCI3^7:' + body.message
            });

            if (connection.serverID)
            {
                logger.debug('there is one...');
                
                //if (connection.serverID in server.instance.clientsByID)
                server.getClientsByID(connection.serverID).forEach(function(serverConn)
                {
                    logger.debug('... and it\'s a client, even');
                        
                    var serverConn = server.instance.clientsByID[connection.serverID];
                    
                    serverConn.reply('RPCAuthenticateKickUserMessage', 0, {
                        npid: connection.npID,
                        reason: 1,
                        reasonString: '^5aCI3^7:' + body.message
                    });
                }
                );
            }
        });
    }
    catch (e)
    {
        logger.fatal(e);
    }
    
    done(null);
}, 16);

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
    
    http.get('http://aiw3.net/log.php?s=' + connection.sessionToken + '&r=' + reason, function(res)
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
    
    var type = str.split(/\s/)[0];
    
    //logger.debug('got a random ' + type);
    
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
        case 'roll':
            handleCI3(this, data.randomString);
        break;
        case 'theater':
            handleTheater(this, id, data.randomString);
        break;
        case 'rtcg':
            handleRTCG(this, id, data.randomString);
        break;
    }
};
