define(['jquery'], function($)
{
	var page =
	{
		loaded: function()
		{
			// update menu widths
			var menuItems = $('.page-header ul.menu li');
			var menuWidth = $('.page-header ul.menu').width();
			var itemWidth = (menuWidth / menuItems.length) - 1;
			
			menuItems.width(itemWidth);
			
			// home grid: add 'rollover' things
			$('.home-grid .box').append('<div class="ovah"></div>');
			
			// mouseover sounds for links
			$('a').mouseenter(function()
			{
				nui.runUITask('play', 'mouse_over');
			});
			
			$('.layout-page').css('padding-top', topAdjust + 'px');
		},
	
		update: function()
		{
			
		},
	};
	
	return page;
});