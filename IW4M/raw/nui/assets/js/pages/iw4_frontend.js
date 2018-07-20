define(['jquery', 'callbacks', 'pages/iw4_layout', 'pages'], function($, callbacks, layout, pages)
{
	var page =
	{
		isFullscreen: true,
		customRender: true,
		
		finalOut: null,
	
		loaded: function(out)
		{
			$('#page-placeholder').append('<div class="page" id="page-iw4_frontend">' + out + '</div>');
			$('#page-iw4_frontend').hide();
			
			page.loadPages(['iw4_home', 'iw4_play', 'iw4_build']);
			
			$('.menu li.active').css('font-size', '130%');
		
			$('a[rel=fpage]').click(function()
			{
				var newPage = $(this).attr('href').substring(1);
				var self = this;
				
				page.scrollPage(newPage, function()
				{
					$('.menu li.active').animate({ fontSize: '100%' }, { queue: false, duration: 100 }).removeClass('active');
					$(self).parent().animate({ fontSize: '130%' }, { queue: false, duration: 100 }).toggleClass('active');
				});
			});
		
			layout.loaded();
		},
		
		loadPages: function(list)
		{
			page.loadPage(list, 0);
		},
		
		loadPage: function(list, i)
		{
			var pageName = list[i];
		
			pages.open(pageName, function(out)
			{
				$('#frontend-content').append('<div class="fpage" id="page-' + pageName + '">' + out + '</div>');
				
				i++;
				
				if (i < list.length)
				{
					page.loadPage(list, i);
				}
				else
				{
					page.scrollPage('iw4_play');
					
					$('#page-iw4_frontend').show();
				}
			});
		},
		
		scrollPage: function(pageName, callback)
		{
			// TODO: make aspect ratio independent
			var pageIndex = -1;
			
			$('#frontend-content .fpage').each(function(i, p)
			{
				if ($(p).attr('id') == ('page-' + pageName))
				{
					pageIndex = i;
					return;
				}
			});
			
			if (pageIndex == -1)
			{
				return;
			}
			
			$('.fpage').removeClass('active');
			$('#page-' + pageName).addClass('active');
			
			var areaWidth = $('body').hasClass('debug') ? 1032 : 1024;
			var aspectRatio = Math.ceil(($(document).width() / $(document).height()) * 768);
			var marginWidth = ((1366 - 1024) / 2) * 0.25;
			var pageLeft = (pageIndex * (areaWidth + marginWidth));
			var ofs = 200 + pageLeft - ((aspectRatio - (1024 + marginWidth)) / 2);
			//var pageLeft = $('#page-' + pageName).position().left;
			//var ofs = (pageLeft + $('#frontend-content').scrollLeft()) - (((1366 - 1024) / 2) * 0.7);
			$('#frontend-content').animate({ scrollLeft: ofs }, { queue: false, duration: 160 });
			
			$('#frontend-content .safearea > div > div').animate({ zoom: '90%' }, { queue: false, duration: 160 });
			$('#frontend-content .fpage.active .safearea > div > div').animate({ zoom: '100%' }, { queue: false, duration: 160 });
			//$('#frontend-content').scrollLeft(ofs);
		
			if (callback !== undefined)
			{
				callback();
			}
		},
	
		update: function()
		{
			layout.update();
		}
	};
	
	return page;
});