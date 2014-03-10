var fsoApi;

if (!fsoApi)
{
	fsoApi = {};
}

(function()
{
	fsoApi.query = function(args)
	{
		native function queryFunc(args);
		return queryFunc(args);
	};
	
	fsoApi.registerCallback = function(name, callback)
	{
		native function registerCallback(name, callback);
		return registerCallback(name, callback);
	};
	
	fsoApi.unregisterCallback = function(id)
	{
		native function unregisterCallback(id);
		return unregisterCallback(id);
	};
})();