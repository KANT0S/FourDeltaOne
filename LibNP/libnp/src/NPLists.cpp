// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: List data structure wrappers.
//
// Initial author: NTAuthority
// Started: 2013-04-15
// ==========================================================

#include "StdInc.h"
#include <unordered_map>

class NPKeyValueIteratorInternalImpl : public NPKeyValueIteratorInternal
{
private:
	std::unordered_map<std::string, std::string>::iterator iter;

	uint32_t refCount;

public:
	NPKeyValueIteratorInternalImpl(std::unordered_map<std::string, std::string>::iterator& i)
	{
		refCount = 0;
		iter = i;
	}

	virtual void AddReference()
	{
		InterlockedIncrement(&refCount);
	}

	virtual void Release()
	{
		if (InterlockedDecrement(&refCount) == 0)
		{
			delete this;
		}
	}

	virtual NPKeyValuePair Dereference()
	{
		NPKeyValuePair pair;
		pair.key = iter->first.c_str();
		pair.value = iter->second.c_str();

		return pair;
	}

	virtual void Increment()
	{
		iter++;
	}

	virtual bool Equals(NPKeyValueIteratorInternal* right)
	{
		return (iter == static_cast<NPKeyValueIteratorInternalImpl*>(right)->iter);
	}
};

class NPDictionaryImpl : public NPDictionaryInternal
{
private:
	std::unordered_map<std::string, std::string> map;

	uint32_t refCount;
public:
	NPDictionaryImpl()
	{
		refCount = 0;
	}

	~NPDictionaryImpl()
	{
		// nothing, so far (the map gets destroyed when we do).
	}

	virtual void AddReference()
	{
		InterlockedIncrement(&refCount);
	}

	virtual void Release()
	{
		if (InterlockedDecrement(&refCount) == 0)
		{
			delete this;
		}
	}

	virtual const char* Get(const char* key)
	{
		auto i = map.find(key);

		if (i != map.end())
		{
			return i->second.c_str();
		}

		return nullptr;
	}

	virtual void Set(const char* key, const char* value)
	{
		map[key] = value;
	}

	virtual bool Has(const char* key)
	{
		auto i = map.find(key);

		return (i != map.end());
	}

	virtual NPKeyValueIterator begin()
	{
		return NPKeyValueIterator(new NPKeyValueIteratorInternalImpl(map.begin()));
	}

	virtual NPKeyValueIterator end()
	{
		return NPKeyValueIterator(new NPKeyValueIteratorInternalImpl(map.end()));
	}
};

LIBNP_API NPDictionaryInternal* LIBNP_CALL NP_CreateDictionary()
{
	return new NPDictionaryImpl();
}