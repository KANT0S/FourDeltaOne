var fs = require('fs');
var request = require('request');

var log4js = require('log4js');
var logger = log4js.getLogger('avatar');

var agentkeepalive = require('agentkeepalive');
var myagent = new agentkeepalive(
{
    maxSockets: 5000,
    maxKeepAliveRequests: 100,
    maxKeepAliveTime: 30000
});

var server = require('./../server').NPServer;
var int64 = require('node-int64');

function replyWithError(result, connection, id, guid, data)
{
    var reply =
    {
        result: result,
        guid: guid,
        fileData: data || new Buffer(1)
    };
    connection.reply('RPCFriendsGetUserAvatarResultMessage', id, reply);
}

var pool = { agent: myagent };

var defaulta = require('fs').readFileSync('data/noavatar.png');

var avatarCache = {}

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        replyWithError(2, connection, id, data.guid);
        return;
    }

    //console.log('doing avatar ' + data.guid);

    /*var npID = (new int64(0x1100001, data.guid).toOctetString()).substring(1);

    var fileID = '0' + npID + '_avatar.png';

    server.instance.database.xnp_files().get(fileID, {}, function(err, body)
    {
        if (err)
        {
            if (err.status_code == 404)
            {
                replyWithError(0, connection, id, data.guid, defaulta);
                return;
            }
        }

        replyWithError(0, connection, id, data.guid, new Buffer(body.fileData, 'base64'));
    });*/

    if (data.guid in avatarCache) {
        replyWithError(0, connection, id, data.guid, avatarCache[data.guid]);
        return;
    }

    //return;
    request( { pool: pool, encoding: null, uri: "https://aiw3.net/api/avatar.php?u=" + data.guid}, function(error, response, body)
    {
		//logger.info(state.npID + ' requesting avatar for user ' + data.guid);

        if (error)
        {
            avatarCache[data.guid] = defaulta;
            replyWithError(3, connection, id, data.guid, defaulta);
        }
        else if(response.statusCode == 403)
        {
            avatarCache[data.guid] = defaulta;
            replyWithError(1, connection, id, data.guid, defaulta);
        }
        else
        {
            avatarCache[data.guid] = body;
            replyWithError(0, connection, id, data.guid, body);
        }
    });
};
