var server = require('./../server').NPServer;
var int64 = require('node-int64');

var log4js = require('log4js');
var logger = log4js.getLogger('auth');

var codify = require('codify');

var attemptsPerIP = {};

setInterval(function()
{
    attemptsPerIP = {};
}, 1000 * 3600 * 2);

function generateKey()
{
    var buffer = require('crypto').randomBytes(5 * 4);
    var string = '';
    
    for (i = 0; i < 5; i++)
    {
        string += '-' + codify.toCode(buffer.readUInt32LE(i * 4) % 1679616, 4);
    }
    
    return string.substring(1);
}

module.exports = function(data, id)
{
    var reply =
    {
        result: 2,
        licenseKey: '',
        serverID: 0
    };
    
    var connection = this;
    
    var doReply = function()
    {   
        connection.reply('RPCAuthenticateRegisterServerResultMessage', id, reply);
    };
    
    // flood protection
    if (!(connection.ipAddr in attemptsPerIP))
    {
        attemptsPerIP[connection.ipAddr] = 0;
    }
    
    attemptsPerIP[connection.ipAddr]++;
    
    if (attemptsPerIP[connection.ipAddr] > 20) // way too much already, but startup for new users on migration
    {
        reply.result = 3;
        doReply();
    }
    
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
            var licenseKey = generateKey();
        
            connection.query('INSERT INTO licensekeys SET ?', { key: licenseKey }, function(err, rows)
            {
                if (err)
                {
                    logger.error('Error setting key: ' + err);
        
                    reply.result = 2; // service unavailable
                }
                else
                {
                    reply.result = 0;
                    reply.licenseKey = licenseKey;
                    reply.serverID = rows.insertId;
                    
                    logger.info('Registered new server key: ' + licenseKey);
                }
                
                doReply();
                connection.end();
            });
        }
    });
};