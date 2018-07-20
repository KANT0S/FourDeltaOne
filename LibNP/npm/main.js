// ==========================================================
// Terminal project
// 
// Component: xnp
// Sub-component: layer1
//
// Initial author: NTAuthority
// Started: 2013-01-14
// ==========================================================

require('nodetime').profile({
    accountKey: 'c07e3f63708af48143b0059a4f4a37da552758f4', 
    appName: 'Node.js Application'
  });

var log4js = require('log4js');

log4js.loadAppender('file');
log4js.addAppender(log4js.appenders.file('data/layer1.log'));

var defaultConfig =
{
    mysql: {
        host: "178.33.234.202",
        port: 3306,
        user: "fourdeltaone",
        password: "CF4BUB23R8gwRJ54j4szesWu",
        database: "xnp"
    },
    forumDB: "phpbb3_4d1",
    couch: {
        host: "localhost",
        port: 5984
    }
};

var fe = require('redisq/frontend');
fe.listen(6389);

var nconf = require('nconf');
nconf.use('file', { file: 'config.json' });
nconf.load();

nconf.defaults(defaultConfig);

var server = new (require('./server/server').NPServer)();
var authapi = require('./server/authapi');
var playerlog = require('./server/playerlog');
var miscserver = require('./server/miscserver');

var logger = log4js.getLogger();
logger.info('Layer1 starting...');

nconf.save(function (err)
{
    logger.info('Configuration stored (result: ' + err + ')');
});

authapi.start();
server.start();
playerlog.start();
miscserver.start();

process.on('uncaughtException', function(err) {
  logger.fatal(err);
});
