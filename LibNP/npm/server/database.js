var agentkeepalive = require('agentkeepalive');
var myagent = new agentkeepalive(
{
    maxSockets: 50,
    maxKeepAliveRequests: 0,
    maxKeepAliveTime: 30000
});

var nconf = require('nconf');

var db = require('nano')(
{
    "url": "http://" + nconf.get('couch:host') + ':' + nconf.get('couch:port') + "/",
    "request_defaults": { "agent": myagent }
});

var mysql = require('mysql');
var mysqlPool;

var xnp_files = null;
  
function start()
{
    xnp_files = db.use('xnp_files');
    
    mysqlPool = mysql.createPool(nconf.get('mysql'));
}

function get_xnp_files()
{
    return xnp_files;
}

function get_mysql(cb)
{
    return mysqlPool.getConnection(cb);
}
  
exports.start = start;
exports.xnp_files = get_xnp_files;
exports.mysql = get_mysql;