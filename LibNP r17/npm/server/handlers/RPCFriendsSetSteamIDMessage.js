module.exports = function(data, id)
{
    // don't do anything; we don't care about legacy Steam stuff for now
    var connection = this;
    
    setTimeout(function()
    {
        connection.reply('RPCAuthenticateExternalStatusMessage', 0, {
            status: 0
        });
    }, 900);
};