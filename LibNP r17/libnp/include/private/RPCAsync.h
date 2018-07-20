// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: asynchronous function result class, RPC-specific
//
// Initial author: NTAuthority
// Started: 2011-07-14
// ==========================================================

#pragma once
#include "NPAsync.h"

class NPRPCAsync : public NPAsync<INPRPCMessage>
{
private:
	void (__cdecl* _callback)(NPAsync<INPRPCMessage>*);
	INPRPCMessage* _result;
	void* _userData;

public:
	NPRPCAsync()
	{
		_callback = NULL;
		_result = NULL;
		_userData = NULL;
	}

	// implementations for base NPAsync
	virtual INPRPCMessage* Wait()
	{
		while (!HasCompleted())
		{
			Sleep(1);
		}

		return GetResult();
	}

	virtual bool HasCompleted()
	{
		return (_result != NULL);
	}

	virtual INPRPCMessage* GetResult()
	{
		return _result;
	}

	virtual void SetCallback(void (__cdecl* callback)(NPAsync<INPRPCMessage>*), void* userData)
	{
		_callback = callback;
		_userData = userData;
	}

	virtual void* GetUserData()
	{
		return _userData;
	}

	virtual void Free()
	{
		delete this;
	}

	// additional definitions
	// set the result
	void SetResult(INPRPCMessage* result)
	{
		_result = result;

		this->RunCallback();
	}

	// run the callback function (if completed)
	void RunCallback()
	{
		if (HasCompleted())
		{
			if (_callback)
			{
				_callback(this);
			}
		}
	}
};