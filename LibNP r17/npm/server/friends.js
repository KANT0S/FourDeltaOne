var server = require('./server').NPServer;
var int64 = require('node-int64');

var log4js = require('log4js');
var logger = log4js.getLogger('friends');

var nconf = require('nconf');

function loadRoster(npID, callback)
{
    server.instance.database.mysql(function(err, connection)
    {
        if (err)
        {
            logger.error('Error getting DB: ' + err);
        }
        else
        {
            connection.query(
                'SELECT f.friend_id, u.username FROM friends f, ' + nconf.get('forumDB') + '.phpbb_users u WHERE f.user_id = ? AND f.friend = 1 AND u.user_id = f.friend_id',
                [ parseInt(npID.substring(7), 16) ],
                function(err, rows)
                {
                    if (err)
                    {
                        logger.error('Error getting roster: ' + err);
                    }
                    else
                    {
                        var roster = [];
                        
                        rows.forEach(function(a)
                        {
                            roster.push({ npid: (new int64(0x1100001, a.friend_id).toOctetString()).substring(1), name: a.username });
                        });
                        
                        callback(roster);
                    }
                    
                    connection.end();
                }
            );
        }
    });
}

function sendPresenceMessage(from, to, why)
{
    var presenceState = 1; // 'online'
    
    if (why == 'offline')
    {
        presenceState = 0;
    }
    else if (from.currentServer)
    {
        presenceState = 2;
    }
    
    var presenceData = [];
    
    for (key in from.presenceData)
    {
        presenceData.push({ key: key, value: from.presenceData[key] });
    }

    to.reply('RPCFriendsPresenceMessage', 0,
    {
        friend: from.npID,
        presenceState: presenceState,
        currentServer: from.serverID,
        presence: presenceData
    });
}

function updateRoster(connection, argument)
{
    if (!connection.roster)
    {
        return;
    }

    connection.roster.forEach(function(a)
    {
        if (a.npid in server.instance.clientsByID)
        {
            var client = server.instance.clientsByID[a.npid];
            
            sendPresenceMessage(connection, client, argument);
        }
    });
}

exports.start = function()
{
    server.instance.on('authenticated', function(connection)
    {
        if (connection.isServer)
        {
            return;
        }
    
        loadRoster(connection.npID, function(roster)
        {
            connection.roster = roster;
            
            connection.reply('RPCFriendsRosterMessage', 0,
            {
                friends: connection.roster
            });
            
            roster.forEach(function(a)
            {
                if (a.npid in server.instance.clientsByID)
                {
                    var client = server.instance.clientsByID[a.npid];
                    
                    sendPresenceMessage(connection, client);
                    sendPresenceMessage(client, connection);
                }
            });
        });
    });
    
    server.instance.on('client_presence', function(connection)
    {
        updateRoster(connection);
    });
    
    server.instance.on('client_joined', function(connection)
    {
        updateRoster(connection);
    });
    
    server.instance.on('client_left', function(connection)
    {
        if (connection.serverID)
        {
            connection.serverID = null;
            updateRoster(connection);
        }
    });
    
    server.instance.on('client_closed', function(connection)
    {
        if (connection.isServer)
        {
            return;
        }
        
        updateRoster(connection, 'offline');
    });
};