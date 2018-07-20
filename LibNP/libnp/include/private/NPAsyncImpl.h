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

	void (__cdecl* _timeoutCallback)(NPAsync<T>*);
	unsigned int _timeout;
	DWORD _start;

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
		return Wait(-1);
	}

	virtual T* Wait(unsigned int timeout)
	{
		DWORD start = GetTickCount();

		while (!HasCompleted())
		{
			RPC_RunFrame();

			Sleep(1);

			DWORD elapsed = GetTickCount() - start;

			if (elapsed > timeout)
			{
				Log_Print("Operation timed out after %i msec\n", timeout);
				return nullptr;
			}
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

	virtual void SetTimeoutCallback(void (__cdecl* callback)(NPAsync<T>*), unsigned int timeout)
	{
		_timeout = timeout;
		_start = GetTickCount();
		_timeoutCallback = callback;
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
	bool HandleTimeout()
	{
		if (!HasCompleted())
		{
			DWORD elapsed = GetTickCount() - _start;

			if (elapsed > _timeout)
			{
				if (_timeoutCallback)
				{
					_timeoutCallback(this);
				}

				_isReferencedByCB = false;
				return true;
			}
		}

		return false;
	}

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