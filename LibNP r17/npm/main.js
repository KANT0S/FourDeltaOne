// ==========================================================
// Terminal project
// 
// Component: xnp
// Sub-component: layer1
//
// Initial author: NTAuthority
// Started: 2013-01-14
// ==========================================================

var log4js = require('log4js');

var defaultConfig =
{
    mysql: {
        host: "localhost",
        port: 3306,
        user: "xnp",
        password: "xnp",
        database: "xnp"
    },
    forumDB: "fourdetlaone",
    couch: {
        host: "localhost",
        port: 5984
    }
};

var nconf = require('nconf');
nconf.defaults(defaultConfig).argv().file({file: 'config.json'});

var server = new (require('./server/server').NPServer)();
var authapi = require('./server/authapi');
var playerlog = require('./server/playerlog');
var miscserver = require('./server/miscserver');

var logger = log4js.getLogger();
logger.info('Layer1 starting...');

/*
nconf.save(function (err)
{
    logger.info('Configuration stored (result: ' + err + ')');
});
*/

authapi.start();
server.start();
playerlog.start();
miscserver.start();

process.on('uncaughtException', function(err) {
  logger.fatal(err);
});