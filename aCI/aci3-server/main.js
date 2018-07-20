// THIS MUST BE IN SYNC WITH DETECTIONS.CPP
var packetVersion = 6;

var log4js = require('log4js');

var logger = log4js.getLogger();
logger.info('ciSV starting...');

var fs = require('fs');
var ursa = require('ursa');
var crypto = require('crypto');
var BitStream = require('bit-buffer').BitStream;

var pkey = ursa.createPrivateKey(fs.readFileSync('cikey.pem'));

var redisq = require('redisq');
redisq.options({ "redis": {
    "host": "localhost",
    "port": 6379
}});

var inQueue = redisq.queue('cireq');
var outQueue = redisq.queue('ciresp');

process.on('uncaughtException', function(err) {
  logger.fatal(err);
});

var fnv = require('fnv').FNV;

var handler = require('./handler');

//app.post('/ci/:npid/:token?', function(req, res)
inQueue.process(function(task, done)
{
    var npID = task.npid;

    try
    {
        var token = task.token;
        var body = new Buffer(task.body, 'base64');

        var req = {
            rawBody: body,
            params:
            {
                npid: npID,
                token: token,
                lastCI: task.lastCI
            }
        };

        // decrypt the request
        var header = req.rawBody.slice(0, 256);
        var data = req.rawBody.slice(256);

        var keyData = pkey.decrypt(header);

        var key = keyData.slice(0, 32);
        var iv = keyData.slice(32, 48);

        var cipher = crypto.createDecipheriv('aes256', key, iv);
        cipher.setAutoPadding(false);
        var decData = cipher.update(data);

        var decData = Buffer.concat([decData, cipher.final()]);
        var stuff = new BitStream(decData);

        var signature = stuff.readBits(32);
        if (signature != 0xCAFEC0DE)
        {
            logger.info('Bad signature from ' + req.params.npid);

            outQueue.push({
                status: 0,
                authorized: false,
                message: 'Invalid CI packet received. Restart the game and try again',
                npid: npID
            });

            return done(null);
        }

        /*var version = stuff.readBits(32);
        logger.info(version);

        if (version != packetVersion)
        {
            logger.info('Bad version from ' + req.params.npid);

            outQueue.push({
                status: 0,
                authorized: (req.params.lastCI > 0) ? true : false,
                message: 'Outdated CI packet received. Restart the game and try again',
                npid: npID
            });

            return done(null);
        }*/

        var fn = new fnv();
        var tokenbuf = Buffer(req.params.token || '');
        fn.update(tokenbuf);

        var s = req.params.token || '';

        var actToken = fn.value();
        var sentToken = (stuff.readBits(32) & 0xFFFFFFFF);

        if (actToken != sentToken)
        {
            logger.info('Bad auth token from ' + req.params.npid + '(' + [s, actToken, sentToken, tokenbuf.length].join(', ') + ')');

            outQueue.push({
                status: 0,
                authorized: false,
                message: 'Invalid CI packet received',
                npid: npID
            });

            return done(null);
        }

        //req.params.lastCI = req.query.lastCI;

        handler.handlePacket(req.params, stuff, function(result)
        {
            result.npid = npID;

            outQueue.push(result);
        });
    }
    catch (e)
    {
        logger.fatal(e);

        outQueue.push({
            status: 0,
            authorized: true,
            message: 'An error occurred.',
            npid: npID
        });
    }

    return done(null);
}, 16);

//app.listen(60321);
