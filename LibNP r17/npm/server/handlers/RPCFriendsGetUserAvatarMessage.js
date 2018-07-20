var fs = require('fs');
var request = require('request');

var log4js = require('log4js');
var logger = log4js.getLogger('avatar');

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

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        replyWithError(2, connection, id, data.guid);
        return;
    }
    request( { encoding: null, uri: "http://fourdeltaone.net/api/avatar/" + data.guid + "/a"}, function(error, response, body)
    {
        if (error)
        {
            replyWithError(3, connection, id, data.guid);
        }
        else if(response.statusCode == 403)
        {
            replyWithError(1, connection, id, data.guid);
        }
        else
        {
            replyWithError(0, connection, id, data.guid, body);
        }
    });
};