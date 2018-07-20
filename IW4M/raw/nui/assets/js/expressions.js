define(['jquery'], function($)
{
	var expressions = 
	{
		runFrame: function()
		{
			$('.expression').each(function()
			{
				$(this).html(nui.runExpression($(this).attr('data-exp')));
			});
			
			$('.dvar').each(function()
			{
				$(this).html(nui.getDvar($(this).attr('data-dvar')));
			});
		},
		
		init: function()
		{
			
		}
	};
	
	return expressions;
});