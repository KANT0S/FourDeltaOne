define(['socket.io'], function(io)
{
	var NPConnection = function(url)
	{
		this.socket = io.connect(url);
		
		this.dispatchHandlers = {};
		this.asyncHandlers = {};
		
		this.maxID = 0;
		
		var self = this;
		
		this.socket.on('message', function(data)
		{
			console.log('Dispatching RPC message with ID ' + data.id + ' and type ' + data.t);
			
			// global dispatch handlers
			if (data.t in self.dispatchHandlers)
			{
				var handlers = [];
				
				if (self.dispatchHandlers[data.t].isArray)
				{
					handlers = self.dispatchHandlers[data.t];
				}
				else
				{
					handlers.push(self.dispatchHandlers[data.t]);
				}
				
				handlers.forEach(function(handler)
				{
					try
					{
						handler(data.d);
					}
					catch (e)
					{
						console.log(e);
					}
				});
			}
			
			// async handlers
			if (data.id in self.asyncHandlers)
			{
				try
				{
					self.asyncHandlers[data.id](data.d);
				}
				catch (e)
				{
					console.log(e);
				}
				
				delete self.asyncHandlers[data.id];
			}
			
			// :D
			// :DD
			// :DDD
			// :DDDD
			// -- dot
		});
	};
	
	NPConnection.prototype.call = function(type, data)
	{
		this.maxID++;
		
		var d =
		{
			id: this.maxID,
			t: type,
			d: data
		};
		
		console.log('Sending RPC message of type ' + d.t + ' with ID ' + d.id);
		
		this.socket.emit('message', d);
		
		var self = this;
		
		var asyncObj =
		{
			done: function(cb)
			{
				self.asyncHandlers[d.id] = function(m)
				{
					if (asyncObj.timeoutID)
					{
						clearTimeout(asyncObj.timeoutID);
					}
					
					cb(m);
				};
				
				return asyncObj;
			},
			timeout: function(timeout, cb)
			{
				asyncObj.timeoutID = setTimeout(function()
				{
					console.log('Operation for ID ' + d.id + ' timed out.');
				
					delete self.asyncHandlers[d.id];
				
					cb();
				}, timeout);
				
				return asyncObj;
			}
		};
		
		return asyncObj;
	};
	
	var np =
	{
		NPConnection: NPConnection
	};
	
	return np;
});