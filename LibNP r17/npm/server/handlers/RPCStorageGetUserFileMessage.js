var server = require('./../server').NPServer;

var log4js = require('log4js');
var logger = log4js.getLogger('storage');

var crc = require("buffer-crc32");

function replyWithError(result, state, body)
{
    var reply =
    {
        result: result,
        fileName: state.fileName,
        fileData: (body) ? new Buffer(body.fileData, 'base64') : new Buffer(1),
        npid: state.npID
    };
    
    logger.info(state.npID + ' requesting ' + state.fileName + '; result: ' + result + (body ? (', size: ' + reply.fileData.length) : ''));

    state.connection.reply('RPCStorageUserFileMessage', state.id, reply);
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

    if (state.npID.substring(0, 2) != '11')
    {
        return;
    }
    
    if (!connection.authenticated)
    {
        replyWithError(2, state);
        return;
    }
    
    if (connection.npID != state.npID)
    {
        replyWithError(2, state);
        return;
    }
    
    var fileID = '0' + state.npID + '_' + state.fileName;
    
    server.instance.database.xnp_files().get(fileID, {}, function(err, body)
    {
        if (err)
        {
            if (err.status_code == 404)
            {
                replyWithError(1, state);
            }
            else
            {
                logger.warn(err.error);
            }
            
            return;
        }
        
		// staff members get insta-11 prestige, everyone else gets denied
		if (state.fileName == "iw4.stat")
		{
			var data = new Buffer(body.fileData, 'base64');
			var prestige = data.readInt32LE(2068);
			
			var group = server.instance.clientsByID[connection.npID].group;
			
			if (group == 4 || group == 5 ||  group == 8 || group == 9 || group == 10 ||  group == 11) data.writeInt32LE(11, 2068);
			else if (prestige >= 11) data.writeInt32LE(10, 2068);
			
			var checksum = crc(data.slice(4));
			checksum.copy(data, 0, 0, 4);
			
			body.fileData = data.toString('base64');
		}
		
        replyWithError(0, state, body);
    });
};
