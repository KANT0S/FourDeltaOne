var io = require('socket.io');
var express = require('express');
var server = require('./server').NPServer;

var app = express();

app.use('/assets', express.static(__dirname + '/../assets'));

app.get('/users', function(req, res)
{
        var players = Object.keys(server.instance.clientsByID).length;
        res.send("" + players);
});

var nserver = require('http').createServer(app)
  , io = io.listen(nserver, { log: false });
  
var ClientConnection = require('./clientConnection');

  
exports.start = function()
{
    io.sockets.on('connection', function(socket)
    {
        var conn = new ClientConnection(server.instance);
        
        server.instance.clients[conn.connID] = conn;
        
        conn.on('message', function(cn, message)
        {
            server.instance.handleMessage(cn, message);
        });
        
        conn.on('closed', function()
        {
            //delete self.clientsByID[conn.npID];
            server.deleteConnection(conn);
        });
        
        conn.bindToWeb(socket);
    });

    nserver.listen(3035);
};