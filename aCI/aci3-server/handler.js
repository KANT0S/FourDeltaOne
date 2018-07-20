var log4js = require('log4js');

var logger = log4js.getLogger();

var fillZeroes = "00000000000000000000";  // max number of zero fill ever asked for in global

function zeroFill(number, width)
{
    var input = number + "";  // make sure it's a string
    return(fillZeroes.slice(0, width - input.length) + input);
}

var Sequelize = require('sequelize');
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

var detections =
{
    // MAC token
    0: function(state, packet)
    {
        var numMACs = packet.readBits(4);
        
        state.result.macs = [];
        
        for (i = 0; i < numMACs; i++)
        {
            state.result.macs.push(
            {
                adr: [ packet.readBits(8), packet.readBits(8), packet.readBits(8), packet.readBits(8), packet.readBits(8), packet.readBits(8) ],
                token: [ packet.readBits(32).toString(16), packet.readBits(32).toString(16) ]
            });
        }
    },
    
    // machine GUID
    1: function(state, packet)
    {
        var hasToken = packet.readBits(1);
        
        if (!hasToken)
        {
            return;
        }
    
        var str = '';
        
        for (i = 0; i < 36; i++)
        {
            str += String.fromCharCode(packet.readBits(8));
        }
        
        state.result.machineID = str;
    },
    
    // legacy CI
    2: function(state, packet)
    {
        var status = packet.readBits(17);
        
        if (status != 50001)
        {
            state.result.authorized = false;
            state.result.message = 'Cheat detected (' + status + ')';
            state.result.status = status;
        }
    },
    
    // mutants
    3: function (state, packet)
    {
        // window texts
        var windowTexts = [
            packet.readBits(32),
            packet.readBits(32),
            packet.readBits(32),
            packet.readBits(32)
        ];
        
        var status = packet.readBits(32);
        var statusCafe = packet.readBits(32);
        
        if (status != 0)
        {
            logger.debug('status ' + status + ' cafe ' + statusCafe);
        }
        
        if (status == 31003 || status == 31004)
        {
            if (statusCafe != (status ^ 0xCAFE))
            {
                logger.info('received invalid mutant checksum from ' + state.npid);
                return;
            }
            
            if (status == 31003)
            {
                logger.debug('31003 window texts for ' + state.npid + ' are ' + windowTexts.join(', '));
            }
            
            state.result.authorized = false;
            state.result.message = 'Cheat detected (' + status + '/' + windowTexts[0] + ')';
            state.result.status = status;
        }
    },
};

function handleToken(state, packet)
{
    var tokenType = packet.readBits(16);
    var tokenNumLow = packet.readBits(32);
    var tokenNumHigh = packet.readBits(32);
    
    var tokenTypeRaw = (tokenNumLow & 0xFFFF) ^ tokenType;
    
    state.result.tokens.push([tokenTypeRaw, zeroFill(tokenNumLow.toString(16), 8), zeroFill(tokenNumHigh.toString(16), 8)]);
    
    // todo: check if banned
}

function handleDetection(state, packet)
{
    var detection = packet.readBits(32);
    
    if (detection in detections)
    {
        detections[detection](state, packet);
    }
}

var util = require('util');

exports.handlePacket = function(params, packet, doneCB)
{
    var type;
    var state =
    {
        npid: params.npid,
        session: params.token || '',
        result:
        {
            authorized: true,
            tokens: [],
            message: '',
            status: 0,
            macs: [],
            machineID: 'none'
        }
    };

    do
    {
        type = packet.readBits(5);
        
        switch (type)
        {
            case 0:
                handleToken(state, packet);
            break;
            case 1:
                handleDetection(state, packet);
            break;
        }
    } while (type != 2);
    
    // check for tokens existing
    if (!state.result.tokens.length)
    {
        state.result.authorized = false;
        state.result.message = 'No tokens sent.';
    }
    
    var tokens = [];
        
    state.result.tokens.forEach(function(a)
    {
        tokens.push(a[0] + ':' + a[1] + a[2]);
    });

    if ((params.lastCI !== undefined && params.lastCI == 0) || state.result.status > 0)
    {
        Ban.findAll({ where: { tokenValue: tokens/*, expires: { lt: new Date() }*/ } }).success(function(bans)
        {
            if (bans.length)
            {
                state.result.authorized = false;
                state.result.status = 41001;
                state.result.message = 'A ban has been issued on one or more of your tokens (' + zeroFill(bans[0].tokenTypePub.toString(16), 4) +
                                       ':' + bans[0].tokenValue.substring(2) + '). Stated reason: ' + bans[0].reason;
            }
            else
            {
                if (state.result.status > 0)
                {
                    var expiry = new Date();
                    expiry.setDate(expiry.getDate() + 30);
                
                    state.result.tokens.forEach(function(a)
                    {
                        var lowerPart = parseInt(a[2], 16);
                    
                        Ban.create({
                            userID: parseInt(state.npid.substring(7), 16),
                            
                            tokenType: a[0],
                            tokenValue: a[0] + ':' + a[1] + a[2],
                            tokenTypePub: (lowerPart & 0xFFFF) ^ a[0],
                            
                            reason: state.result.message,
                            
                            expires: expiry
                        });
                    });
                }
            }

            //console.log(util.inspect(state, { depth: null, colors: true }));
           
            if (params.lastCI !== undefined && params.lastCI == 0)
            {
                var macs = [];
                
                state.result.macs.forEach(function(a)
                {
                    macs.push(a.adr[0].toString(16) + '-' + a.adr[1].toString(16) + '-' + a.adr[2].toString(16) + '-' +
                              a.adr[3].toString(16) + '-' + a.adr[4].toString(16) + '-' + a.adr[5].toString(16));
                });
                
                var guid = state.result.machineID;
                
                var logMsg = state.npid + ':CIin (t: ' + tokens.join(', ') + ', m: ' + macs + ', g: ' + guid + ')';
                
                //console.log(logMsg);
                
                Connection.create({
                    userID: parseInt(state.npid.substring(7), 16),
                    authorized: state.result.authorized,
                    status: state.result.status,
                    
                    connectionLog: logMsg
                });
            }
           
            doneCB(state.result);
       });
   }
   else
   {
       doneCB(state.result);
   }
};
