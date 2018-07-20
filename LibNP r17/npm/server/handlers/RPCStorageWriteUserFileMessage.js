var server = require('./../server').NPServer;

var log4js = require('log4js');
var logger = log4js.getLogger('storage');

function replyWithError(result, state)
{
    var reply =
    {
        result: result,
        fileName: state.fileName,
        npid: state.npID
    };
    
    logger.info(state.npID + ' saving ' + state.fileName + '; result: ' + result);

    state.connection.reply('RPCStorageWriteUserFileResultMessage', state.id, reply);
}

module.exports = function(data, id)
{
    var connection = this;
    
    var state =
    {
        fileName: data.fileName,
        npID: data.npid,
        connection: connection,
        id: id
    };
    
    if (!connection.authenticated)
    {
        replyWithError(1, state);
        return;
    }
    
    if (connection.npID != state.npID)
    {
        replyWithError(1, state);
        return;
    }
    
    var fileID = '0' + state.npID + '_' + state.fileName;
    
    server.instance.database.xnp_files().get(fileID, {}, function(err, body)
    {
        if (err)
        {
            if (err.status_code != 404)
            {
                logger.warn(err.error);
                replyWithError(2, state);
                
                return;
            }
            
            body = {};
        }
        
        body.fileData = data.fileData.toString('base64');
        
        server.instance.database.xnp_files().insert(body, fileID, function(err)
        {
            if (err)
            {
                logger.warn(err.error);
            }
            
            server.instance.emit('file_write', state.npID, state.fileName, data.fileData);
        
            replyWithError((!err) ? 0 : 2, state);
        });
    });
};