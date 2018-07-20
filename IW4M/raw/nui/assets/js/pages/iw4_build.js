define(['jquery', 'callbacks', 'pages/iw4_layout'], function($, callbacks, layout)
{
	var page =
	{
		isFullscreen: true,
		customRender: true,
	
		loaded: function()
		{
			layout.loaded();
		},
	
		update: function()
		{
			layout.update();
		}
	};
	
	return page;
});