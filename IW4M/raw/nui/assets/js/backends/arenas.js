define(['require', 'pages'], function(require, pages)
{
	var arenaBE = 
	{
		data: {}
	};

	pages.registerCallback('arenas', function(data)
	{
		arenaBE.data = data;
	});
	
	return arenaBE;
});