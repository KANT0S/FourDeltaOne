var server = require('./../server').NPServer;
var int64 = require('node-int64');

var log4js = require('log4js');
var logger = log4js.getLogger('friends');

server.instance.on('file_write', function(npid, filename, data)
{
    if (filename == 'iw4.stat')
    {
        var prestige = data.readInt32LE(2068);
        var experience = data.readInt32LE(2060);
    
        server.instance.database.mysql(function(err, connection)
        {
            if (err)
            {
                logger.error(err);
                return;
            }
            
            var uid = parseInt(npid.substring(7), 16);
            
            connection.query('SELECT user_id FROM iw4_profiles WHERE user_id = ?',
                [ uid ],
                function(err, result)
                {
                    if (err)
                    {
                        logger.error(err);
                        connection.end();
                        return;
                    }
                    
                    if (result.length > 0)
                    {
                        connection.query('UPDATE iw4_profiles SET ? WHERE user_id = ?',
                            [
                                { prestige: prestige, experience: experience },
                                uid
                            ],
                            function(err, result)
                            {
                                if (err)
                                {
                                    logger.error(err);
                                }
                                
                                connection.end();
                            });
                    }
                    else
                    {
                        connection.query('INSERT INTO iw4_profiles SET ?',
                            { prestige: prestige, experience: experience, user_id: uid },
                            function(err, result)
                            {
                                if (err)
                                {
                                    logger.error(err);
                                }
                                
                                connection.end();
                            });
                    }
                });
        });
    }
});

module.exports = function(data, id)
{
    var cconnection = this;

    server.instance.database.mysql(function(err, connection)
    {
        if (err)
        {
            logger.error(err);
            return;
        }
        
        var ids = [];
        
        data.npids.forEach(function(a)
        {
            ids.push(parseInt(a.substring(7), 16));
        });
        
        connection.query('SELECT * FROM iw4_profiles WHERE user_id IN (?)',
            [ ids ],
            function(err, result)
            {
                if (err)
                {
                    logger.error(err);
                    connection.end();
                    return;
                }
                
                var results = [];
                
                result.forEach(function(row)
                {
                    var buffer = new Buffer(8);
                    buffer.writeUInt32LE(parseInt(row.experience), 0);
                    buffer.writeUInt32LE(parseInt(row.prestige), 4);
                
                    var r =
                    {
                        npid: (new int64(0x1100001, row.user_id).toOctetString()).substring(1),
                        profile: buffer
                    };
                    
                    results.push(r);
                });
                
                cconnection.reply('RPCFriendsGetProfileDataResultMessage', id, {
                    results: results
                });
            });
   });
};