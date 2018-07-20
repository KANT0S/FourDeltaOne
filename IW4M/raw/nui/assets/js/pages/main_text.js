define(['jquery', 'callbacks', 'pages'], function($, callbacks, pages)
{
	var page =
	{
		load: function(cb)
		{
			pages.open('iw4_frontend');
		
			callbacks.addTaskHandler(nui.runUITask('uiscript', 'loadarenas'), function()
			{
				nui.runUITask('execNow', '"set g_gametype menu"');
				nui.runUITask('execNow', '"set ui_mapname ui_viewer_mp"');
				//nui.runUITask('exec', '"wait; wait; devmap ui_viewer_mp"');
		
				cb();
			});
		}
	};
	
	return page;
});