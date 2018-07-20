// global variables defined for the purpose of, well, code callbacks
var CodeCallback_UITaskDone;
var CodeCallback_OpenPage;
var CodeCallback_CloseAllPages;
var CodeCallback_ReloadPage;
var CodeCallback_PageBroadcast;

define(['pages'], function(pages)
{
	var uiTaskHandlers = {};

	var callbacks =
	{
		init: function()
		{
			CodeCallback_UITaskDone = UITaskDone;
			CodeCallback_OpenPage = function(pageName)
			{
				pages.open(pageName);
			};
			
			CodeCallback_CloseAllPages = function()
			{
				pages.closeAll();
			};
			
			CodeCallback_ReloadPage = function(pageName)
			{
				less.refresh();
				pages.reload(pageName);
			};
			
			CodeCallback_PageBroadcast = function(messageType, messageData)
			{
				pages.broadcast(messageType, messageData);
			};
		},
		
		addTaskHandler: function(taskID, handler)
		{
			if (uiTaskHandlers[taskID] === undefined)
			{
				uiTaskHandlers[taskID] = [];
			}
			
			uiTaskHandlers[taskID].push(handler);
		}
	};

	function UITaskDone(taskID)
	{
		if (taskID in uiTaskHandlers)
		{
			uiTaskHandlers[taskID].forEach(function (handler)
			{
				handler();
			});
		}
	}

	return callbacks;
});