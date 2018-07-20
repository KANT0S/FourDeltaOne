// This file has been automatically generated.
var schema = new (require('protobuf').Schema)((require('fs').readFileSync(__dirname + '/proto.desc')));
var server = require('./server').NPServer;

var parsers =
{
<?php
foreach ($messages as $message)
{
?>
	<?php echo $message['type'] ?>: function(data)
	{
		return schema.<?php echo $message['message'] ?>.parse(data);
	},

<?php
}
?>
};

var serializers =
{
<?php
foreach ($messages as $message)
{
?>
	<?php echo $message['type'] ?>: function(data)
	{
		return schema.<?php echo $message['message'] ?>.serialize(data);
	},

<?php
}
?>
};

var ids =
{
<?php
foreach ($messages as $message)
{
?>
	<?php echo $message['name'] ?>: <?php echo $message['type'] ?>,
<?php
}
?>
};

<?php
foreach ($messages as $message)
{
?>
server.handlers[<?php echo $message['type'] ?>] = function(id, data)
{
	require('./handlers/<?php echo $message['name'] ?>').call(this, data, id);
};

<?php
}
?>

exports.parsers = parsers;
exports.serializers = serializers;
exports.ids = ids;
exports.schema = schema;