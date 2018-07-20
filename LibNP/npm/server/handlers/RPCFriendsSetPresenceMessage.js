var server = require('./../server').NPServer;

module.exports = function(data, id)
{
    var connection = this;
    
    //server.emit('client_presence', data.presence);
    connection.presenceData = {};
    
    data.presence.forEach(function(a)
    {
        connection.presenceData[a.key] = a.value;
    });
    
    server.instance.emit('client_presence', connection);
};