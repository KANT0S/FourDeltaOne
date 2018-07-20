define(['jquery', 'pages'], function($, pages)
{
	var page =
	{
		load: function(cb)
		{
			pages.open('iw4_frontend');
			page.close();
		}
	};
	
	return page;
});