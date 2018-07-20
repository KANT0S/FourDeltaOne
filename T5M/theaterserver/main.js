var g_base = 'http://77.21.194.119:8132';

var express = require('express');
var app = express();

var crypto = require('crypto');

function genHash(expires, path)
{
    console.log('generating hash ' + path + expires + 'cakemake');
    
    var hash = crypto.createHash('sha1');
    hash.update(path + expires + 'cakemake', 'ascii');
    return hash.digest('base64');
}

app.use(express.bodyParser());
app.use(express.methodOverride());
app.use(function(req, res, next)
{
    if (!req.query.e || !req.query.s)
    {
        res.send(401, '');
        return;
    }
    
    var sig = genHash(req.query.e, req.path);
    
    if (sig != req.query.s)
    {
        res.send(402, '');
        return;
    }
    
    if (Math.floor(new Date().getTime() / 1000) > req.query.e)
    {
        res.send(403, '');
        return;
    }
    
    next(null);
});

app.use(app.router);
app.use(express.errorHandler());

var Sequelize = require("sequelize");
var sequelize = new Sequelize('mysql://cs:9EuPA6DwDT8DWXzz@localhost/cs_fdo');

var File = sequelize.define('File',
{
    fileName: { type: Sequelize.STRING },
    storageType: { type: Sequelize.INTEGER },
    fileSize: { type: Sequelize.INTEGER.UNSIGNED },
    metaData: { type: Sequelize.BLOB },
    ownerID: { type: Sequelize.BIGINT },
    summarySize: { type: Sequelize.INTEGER.UNSIGNED },
});

var FileTag = sequelize.define('FileTag',
{
    tagID: { type: Sequelize.STRING(24) },
});

var SlotFile = sequelize.define('SlotFile',
{
    userID: { type: Sequelize.BIGINT },
    slotNumber: { type: Sequelize.INTEGER }
});

File.hasMany(FileTag);
FileTag.hasMany(File);

SlotFile.hasOne(File);
SlotFile.belongsTo(File);

sequelize.sync();

var redisq = require('redisq');
redisq.options({ "redis": {
    "host": "localhost",
    "port": 6379
}});

var inQueue = redisq.queue('threq');
var outQueue = redisq.queue('thresp');

var handlers = {};

function genURL(task, filename)
{
    var expires = Math.floor(new Date().getTime() / 1000) + 7200;
    var path =  '/' + task.title + '/' + task.npid + '/' + filename;
    var sig = genHash(expires, path);
    
    var obj = 
    {
        url: (g_base + path + '?e=' + expires + '&s=' + encodeURIComponent(sig)),
        serverNPID: parseInt(task.npid.substring(7), 16),
        serverID: parseInt(task.npid.substring(1, 2)),
        serverFilename: filename
    };
    
    return obj;
}

handlers.writeStreamed = function(task, cb)
{
    var body = task.body;
    /*var expires = Math.floor(new Date().getTime() / 1000) + 7200;
    var path =  '/' + task.title + '/' + task.npid + '/' + task.body.name;
    var sig = genHash(expires, path);
    
    var obj = 
    {
        url: (g_base + path + '?e=' + expires + '&s=' + encodeURIComponent(sig)),
        serverNPID: parseInt(task.npid.substring(7), 16),
        serverID: parseInt(task.npid.substring(1, 2)),
        serverFilename: task.body.name
    };*/
    
    cb(genURL(task, task.body.name));
};

handlers.confirmStreamed = function(task, cb)
{
    File.create({
        fileName: task.body.name,
        storageType: task.body.nptype,
        metaData: null,
        fileSize: task.body.size,
        ownerID: parseInt(task.body.npid, 16),
    }).success(function(file)
    {
        cb({ fileid: file.id });
    }).error(function(err)
    {
        console.log(err);
    });
};

var async = require('async');

function createTags(file, task, cb)
{
    var tagData = [];
    var hasTags = [];
            
    task.body.tags.forEach(function(tag)
    {
        if (hasTags.indexOf(tag[0] + tag[1]) != -1)
        {
            return;
        }
    
        hasTags.push(tag[0] + tag[1]);
    
        tagData.push(function(insCB)
        {
            FileTag.findOrCreate({
                tagID: tag[0] + ':' + tag[1]
            }).success(function(item)
            {
                insCB(null, item);
            }).error(function(err)
            {
                insCB(err);
            });
        });
    });
    
    async.parallel(tagData, function(err, results)
    {
        if (err)
        {
            console.log(err);
            return;
        }
    
        file.setFileTags(results).success(function()
        {
            cb();
        }).error(function(e) { console.log(e); });
    });    
}

handlers.writeStreamedSummary = function(task, cb)
{
    File.find(task.body.fileid).success(function(file)
    {
        file.metaData = new Buffer(task.body.meta, 'base64');
        file.summarySize = task.body.size;
        
        file.save().success(function()
        {
            createTags(file, task, function()
            {
                cb(genURL(task, file.fileName + '.summary'));
            });
        }).error(function(e) { console.log(e); });
    }).error(function(e) { console.log(e); });
};

handlers.confirmStreamedSummary = function(task, cb)
{
    /*File.find(task.body.fileid).success(function(file)
    {
        file.summarySize = task.body.size;
        
        file.save().success(function()
        {
            cb({});
        });
    });*/
    
    // no-op as dw doesn't pass file id
    cb({});
};

handlers.searchByTags = function(task, cb)
{
    var tags = [];
    
    task.body.tags.forEach(function(tag)
    {
        tags.push(tag[0] + ':' + tag[1]);
    });

    FileTag.findAll({ where: { tagID: tags },
                      include: [ File ],
                      offset: task.body.start,
                      limit: task.body.count },
                    { raw: true }).success(function(tags)
    {
        var fileIDs = [];
        
        tags.forEach(function(tag)
        {
            //tag.Files.forEach(function(file)
            //{
                if (fileIDs.indexOf(tag.Files.id) == -1)
                {
                    fileIDs.push(tag.Files.id);
                }
            //});
        });
        
        cb(fileIDs);
    }).error(function(e) { console.log(e); });
};

var fillZeroes = "00000000000000000000";  // max number of zero fill ever asked for in global

function zeroFill(number, width)
{
    var input = number + "";  // make sure it's a string
    return(fillZeroes.slice(0, width - input.length) + input);
}

handlers.getStreamedSummaries = function(task, cb)
{
    File.findAll({ where: { id: task.body.ids }/*,
                   include: [ FileTag ]*/ }).success(function(files)
    {
        var retval = [];
        
        files.forEach(function(file)
        {
            var tags = [];
            
            /*if (file.tags)
            {
                file.tags.forEach(function(tag)
                {
                    tags.push(tag.split(':'));
                });
            }*/
        
            task.npid = '1' + file.storageType + '00001' + zeroFill(file.ownerID.toString(16), 8);
        
            retval.push({
                id: file.id,
                meta: file.metaData.toString('base64'),
                npid: task.npid,
                time: Math.floor(file.createdAt.getTime() / 1000),
                updateTime: Math.floor(file.updatedAt.getTime() / 1000),
                fileSize: file.fileSize,
                fileName: file.fileName,
                summarySize: file.summarySize,
                tags: tags
            });
        });
        
        cb(retval);
    }).error(function(e) { console.log(e); });   
};

handlers.getStreamedSummary = function(task, cb)
{
    File.find(task.body.fileid).success(function(file)
    {
        task.npid = '1' + file.storageType + '00001' + zeroFill(file.ownerID.toString(16), 8);
        
        cb({
            url: genURL(task, file.fileName + '.summary').url
        });
    });
};

handlers.getStreamedFile = function(task, cb)
{
    File.find(task.body.fileid).success(function(file)
    {
        task.npid = '1' + file.storageType + '00001' + zeroFill(file.ownerID.toString(16), 8);
        
        cb({
            url: genURL(task, file.fileName).url,
            id: file.id,
            meta: file.metaData.toString('base64'),
            npid: task.npid,
            fileSize: file.fileSize,
            fileName: file.fileName,
            summarySize: file.summarySize,
            time: Math.floor(file.createdAt.getTime() / 1000),
            updateTime: Math.floor(file.updatedAt.getTime() / 1000)
        });
    }).error(function(e) { console.log(e); });
};

handlers.write = function(task, cb)
{
    var body = task.body;
    
    cb(genURL(task, task.body.name));
};

handlers.confirm = function(task, cb)
{
    var own = parseInt(task.npid.substring(7), 16);
    
    File.create({
        fileName: task.body.name,
        storageType: task.npid.substring(1, 2),
        metaData: new Buffer(task.body.meta, 'base64'),
        fileSize: task.body.size,
        ownerID: own,
    }).success(function(file)
    {
        //cb({ fileid: file.id });
        
        SlotFile.find({ where: { userID: own, slotNumber: task.body.slot } }).success(function(slotFile)
        {
            if (!slotFile)
            {
                slotFile = SlotFile.build({
                    userID: own,
                    slotNumber: task.body.slot
                });
            }
            
            slotFile.save().success(function()
            {
                slotFile.setFile(file).success(function()
                {
                    createTags(file, task, function()
                    {
                        cb({ fileid: file.id });
                    });
                }).error(function(err) { console.log(err); });
            }).error(function(err) { console.log(err); });
        }).error(function(err) { console.log(err); });
    }).error(function(err)
    {
        console.log(err);
    });
};

inQueue.process(function(task, done)
{
    var conn = task.conn;
    var body = task.body;
    
    console.log(body.type);
    
    try
    {
        handlers[body.type](task, function(result)
        {        
            outQueue.push({
                conn: conn,
                body: result
            });
        });
    }
    catch (e)
    {
        console.log(e);
    }
    
    return done(null);
}, 16);
    
var fs = require('fs');
var mkdirp = require('mkdirp');

function getPath(npid, filename, cb)
{
    mkdirp('data/' + npid.substring(11, 13) + '/' + npid.substring(13, 15) + '/' + npid, function()
    {
        cb('data/' + npid.substring(11, 13) + '/' + npid.substring(13, 15) + '/' + npid + '/a_' + filename);
    });
}

function doStuff(id, req, res)
{
    getPath((req.params.npid) ? req.params.npid : '0000000000000000', req.params.filename, function(path)
    {
        var ws = fs.createWriteStream(path);
    
        req.on('data', function(buf)
        {
            ws.write(buf);
        });
        
        req.on('end', function()
        {
            ws.end();
            res.send(201, '');
        });
    });
}

app.put('/:title/:npid/:filename', function(req, res)
{
    doStuff(0, req, res);
});

app.get('/:title/:npid/:filename', function(req, res)
{
    getPath((req.params.npid) ? req.params.npid : '0000000000000000', req.params.filename, function(path)
    {
        res.sendfile(path.substring(5), { root: 'data' });
    });
});

app.put('/banana0/:filename', function(req, res)
{
    doStuff(1, req, res);
});

app.put('/banana1/:filename', function(req, res)
{
    doStuff(2, req, res);
});

app.put('/banana2/:filename', function(req, res)
{
    doStuff(3, req, res);
});

app.listen(8132);
