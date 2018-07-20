var pageZoom;
var topAdjust = 0;

requirejs(['jquery', 'callbacks', 'expressions', 'pages'], function($, callbacks, expressions, pages)
{
	// even though the DOM contains little, it still contains <body>
	function runFrame()
	{
		requestAnimationFrame(runFrame);
	
		expressions.runFrame();
		pages.runFrame();
	}
	
	function adjustZoom()
	{
		var screenWidth = document.documentElement.clientWidth;
		var screenHeight = document.documentElement.clientHeight;
		var dataHeight = 768;
		var oldHeight = 0;
		
		if (screenWidth / screenHeight == 5 / 4)
		{
			oldHeight = screenHeight;
			screenHeight = (screenWidth / (4 / 3));
			
			topAdjust = ((oldHeight - screenHeight) / 2);
		}
		
		$('body').css('zoom', ((screenHeight / dataHeight) * 100) + '%');
		pageZoom = (screenHeight / dataHeight);
	}
	
	$(function()
	{
		adjustZoom();
	
		callbacks.init();
		expressions.init();
		pages.init();
		
		pages.closeAll();
		//pages.open('connect');
		
		requestAnimationFrame(runFrame);
	});

	/*function runFrame(taskID)
	{
		requestAnimationFrame(runFrame);
		
		$('.loadbar div').css('width', (nui.getLoadProgress() * 100) + '%');
		//$('.main-title').html(nui.runExpression('dvarstring(version)'));
		//$('.main-title').html(nui.getLoadProgress());
	}
	
	CodeCallback_UITaskDone = runFrame;

	$(function()
	{
		nui.runUITask('uiscript', 'loadarenas');
		//requestAnimationFrame(runFrame);
	});*/
});