define(['jquery', 'callbacks'], function($, callbacks)
{
	var page =
	{
		load: function(cb)
		{
			cb();
		},
		
		update: function()
		{
			var loadProgress = nui.getLoadProgress();
		
			$('.loadbar div').css('width', (loadProgress * 100) + '%');
			
			if (nui.getDvar('ui_mapname') == 'ui_viewer_mp')
			{
				$('.mapname').html('IW4M');
				$('.gametypename').html('&nbsp;');
				
				if (loadProgress <= 0.99)
				{
					$('.info').html('');
				}
			}
		},
		
		unload: function()
		{
			nui.runUITask('close', 'connect');
		}
	};
	
	return page;
});