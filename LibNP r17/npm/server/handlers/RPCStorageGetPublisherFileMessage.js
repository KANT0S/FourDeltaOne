var fs = require('fs');

function replyWithError(result, connection, id, fileName, data)
{
    var reply =
    {
        result: result,
        fileName: fileName,
        fileData: data || new Buffer(1)
    };

    connection.reply('RPCStoragePublisherFileMessage', id, reply);
}

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        replyWithError(2, connection, id, data.fileName);
        return;
    }
    
    var fsFile = "data/pub/" + data.fileName.replace("\\", "/").replace('/', '');
    
    fs.exists(fsFile, function(exists)
    {
        if (!exists)
        {
            replyWithError(1, connection, id, data.fileName);
            return;
        }
    
        fs.readFile(fsFile, function(err, content)
        {
            if (err)
            {
                replyWithError(3, connection, id, data.fileName);
                return;
            }
            
            replyWithError(0, connection, id, data.fileName, content);
        });    
    });
};