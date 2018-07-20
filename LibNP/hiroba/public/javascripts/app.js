requirejs(['jquery', 'libnp'], function($, np)
{
	var conn = new np.NPConnection('http://192.168.178.83:3035');
	conn.call(1001,
	{
		licenseKey: 'EONP-MQOJ-B5OA-R226-BUOA'
	}).done(function(data)
	{
		$('p').html('<pre>' + JSON.stringify(data, false, ' ') + '</pre>');
	}).timeout(5000, function()
	{
		$('p').html('Operation timed out.');
	});
});