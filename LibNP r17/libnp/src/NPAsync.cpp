// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: asynchronous function result handling
//
// Initial author: NTAuthority
// Started: 2011-07-14
// ==========================================================

#include "StdInc.h"

std::queue<NPAsyncCallback*> _asyncCallbacks;

void Async_RunCallbacks()
{
	RPC_RunFrame();

	while (!_asyncCallbacks.empty())
	{
		NPAsyncCallback* callback = _asyncCallbacks.front();

		if (callback->RunCallback())
		{
			callback->Free();
		}

		_asyncCallbacks.pop();
	}
}