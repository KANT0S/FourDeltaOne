var server = require('./../server').NPServer;

var log4js = require('log4js');
var logger = log4js.getLogger('auth');

var int64 = require('node-int64');

/*var Sequelize = require('sequelize');
var sequelize = new Sequelize('mysql://aiw3:NRvFafRdHVcUzBRR@localhost/aiw3_aci',{logging: false});

var Ban = sequelize.define('Ban', {
    userID: Sequelize.INTEGER,
    
    tokenType: Sequelize.INTEGER,
    tokenTypePub: Sequelize.INTEGER,
    
    tokenValue: Sequelize.STRING,
    
    reason: Sequelize.STRING,
    
    expires: Sequelize.DATE
});

var Connection = sequelize.define('Connection', {
    userID: Sequelize.INTEGER,
    
    authorized: Sequelize.BOOLEAN,
    status: Sequelize.INTEGER,
    
    // yes i know it's bad to do this
    connectionLog: Sequelize.TEXT
});

sequelize.sync();

var cronJob = require('cron').CronJob;

var job = new cronJob('00 * * * * *', function(){
    
  }, function () {}, 
  true);*/

function readTicket(ticket)
{
    var data =
    {
        version: ticket.readUInt32LE(0)
    };
    
    if (data.version == 1)
    {
        data.clientID = new int64(ticket.readUInt32LE(8), ticket.readUInt32LE(4)).toOctetString().substring(1),
        data.serverID = new int64(ticket.readUInt32LE(16), ticket.readUInt32LE(12)).toOctetString().substring(1),
        
        data.time = ticket.readUInt32LE(20);
    }
    
    return data;
}

module.exports = function(data, id)
{
    var ipNum = data.clientIP;
    var ip = ((ipNum >> 24) & 0xFF) + '.' + ((ipNum >> 16) & 0xFF) + '.' +
             ((ipNum >> 8) & 0xFF) + '.' + (ipNum & 0xFF);
             
    var ticket = readTicket(data.ticket);
    
    var npid = data.npid;
    
    if (npid == '0')
    {
        npid = ticket.clientID;
    }
    
    var valid = false;
    var groupID = 0;
    
    if (ticket.version == 1)
    {
        if (this.npID == ticket.serverID)
        {
            if (npid == ticket.clientID)
            {
                server.getClientsByID(ticket.clientID).forEach(function(remoteConn)
                {
                    if (!remoteConn.native)
                    {
                        return;
                    }

                    remoteConn.serverID = ticket.serverID;
                
                    //var remoteConn = server.instance.clientsByID[ticket.clientID];
                    
                    if (remoteConn.unclean)
                    {
                        logger.info('This guy is dirty.');
                    
                        valid = false;
                    }
                    else
                    {
                        if ((new Date().getTime() - remoteConn.lastCI) > 30000)
                        {
                            logger.info('No CI heartbeat lately. (lastCI: ' + remoteConn.lastCI + ')');
                            //return;
                        }
                        
                        remoteConn.serverID = this.npID;
						
                        logger.debug('set ' + remoteConn.npID + ' server ID to ' + this.npID);
                        
                        if (1==1 || ipNum == 0 || remoteConn.ip == ip || ip.indexOf('192.168') == 0 || ip.indexOf('172.1') == 0 || ip.indexOf('172.2') == 0 || ip.indexOf('10.') == 0)
                        {
                            server.instance.emit('client_joined', remoteConn);
                        
                            valid = true;
                            
                            groupID = remoteConn.group;
                        }
                        else
                        {
                            logger.info('IP did not match.');
                        }
                    }
                });
            }
            else
            {
                logger.info('Client NPID did not match.');
            }
        }
        else
        {
            logger.info('Server NPID did not match.');
        }
    }
    else
    {
        logger.info('Ticket version did not match.');
    }
    
    this.reply('RPCAuthenticateValidateTicketResultMessage', id, {
        result: (valid) ? 0 : 1,
        groupID: groupID,
        npid: npid
    });
};
