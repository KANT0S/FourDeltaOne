// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: asynchronous function result class, internal
//          details.
//
// Initial author: NTAuthority
// Started: 2011-07-14
// ==========================================================

#pragma once
#include "NPAsync.h"

class NPAsyncCallback
{
public:
	virtual void Free() = 0;
	virtual bool RunCallback() = 0;
};

extern std::queue<NPAsyncCallback*> _asyncCallbacks;

template <class T>
class NPAsyncImpl : public NPAsync<T>, public NPAsyncCallback
{
private:
	void (__cdecl* _callback)(NPAsync<T>*);
	T* _result;
	void* _userData;
	bool _freeResult;
	bool _isReferencedByCB;

public:
	NPAsyncImpl()
	{
		_callback = NULL;
		_result = NULL;
		_userData = NULL;
		_freeResult = true;
		_isReferencedByCB = false;
	}

	// implementations for base NPAsync
	virtual T* Wait()
	{
		while (!HasCompleted())
		{
			RPC_RunFrame();
			Sleep(1);
		}

		return GetResult();
	}

	virtual bool HasCompleted()
	{
		return (_result != NULL);
	}

	virtual T* GetResult()
	{
		return _result;
	}

	virtual void SetCallback(void (__cdecl* callback)(NPAsync<T>*), void* userData)
	{
		_callback = callback;
		_userData = userData;
		_isReferencedByCB = true;
	}

	virtual void* GetUserData()
	{
		return _userData;
	}

	virtual void Free()
	{
		if (_freeResult)
		{
			delete _result;
		}

		if (!_isReferencedByCB)
		{
			delete this;
		}
	}

	// additional definitions
	// set the result
	void SetResult(T* result)
	{
		_result = result;

		if (_callback != NULL)
		{
			_asyncCallbacks.push(this);
		}
	}

	// do we free the result, or is it externally handled? (defaults to true)
	void SetFreeResult(bool freeResult)
	{
		_freeResult = freeResult;
	}

	// run the callback function (if completed)
	bool RunCallback()
	{
		if (HasCompleted())
		{
			if (_callback)
			{
				_callback(this);
			}

			_isReferencedByCB = false;
			return true;
		}

		return false;
	}
};

void Async_RunCallbacks();