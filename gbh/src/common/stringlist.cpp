// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: String list - safe allocation/freeing of strings.
//
// Initial author: NTAuthority
// Started: 2013-03-02
// ==========================================================

#include <gbh.h>
#include <sparsehash/dense_hash_map>

#define MAX_SCRIPT_STRINGS 65000

class gbhStringListLocal : public gbhStringList
{
private:
	struct ScriptStringData;

	struct ScriptStringManagementData
	{
		int32_t users; // bitmask
		uint32_t references; // reference count
		ScriptStringData* data; // backreference to the data we know about
	};

	struct ScriptStringData
	{
		ScriptStringManagementData* data;
		char* string;
		int length;
	};

	uint32_t firstFree;

	// FIXME: cross-platform support!
	CRITICAL_SECTION critSec;
	ScriptStringData strings[MAX_SCRIPT_STRINGS];
	google::dense_hash_map<std::string, uint16_t> hashMap;
public:
	virtual void initialize() 
	{
		hashMap.set_empty_key("**DUMMYDUMMY**");
		hashMap.set_deleted_key("**DUMMY**");

		firstFree = 1;

		InitializeCriticalSection(&critSec);
	}

	virtual StringRef getString(const char* string) 
	{
		return getString(string, strlen(string) + 1);
	}

	virtual StringRef getString(const char* string, int size) 
	{
		return getString(string, 0, size);
	}

	virtual StringRef getString(const char* string, int system, int size)
	{
		EnterCriticalSection(&critSec);

		// try to find the string first
		std::string str(string, size);
		google::dense_hash_map<std::string, uint16_t>::const_iterator iter = hashMap.find(str);

		if (iter != hashMap.end())
		{
			addRefToString(&StringRef(iter->second));

			LeaveCriticalSection(&critSec);
			return iter->second;
		}

		// add the string to the list
		ScriptStringData* data = &strings[firstFree];
		uint16_t stringID = firstFree;

		data->data = new ScriptStringManagementData;
		data->string = (char*)malloc(size);
		memcpy(data->string, string, size);

		data->length = size;

		data->data->data = data;
		data->data->references = 1;
		data->data->users = system;

		// find the next free string
		for (int i = firstFree; i < MAX_SCRIPT_STRINGS; i++)
		{
			if (!strings[i].string)
			{
				firstFree = i;
				break;
			}
		}

		hashMap[str] = stringID;

		LeaveCriticalSection(&critSec);

		return StringRef(stringID);
	}

	virtual StringRef findString(const char* string)
	{
		return findString(string, strlen(string) + 1);
	}

	virtual StringRef findString(const char* string, int size) 
	{
		std::string str(string, size);

		google::dense_hash_map<std::string, uint16_t>::const_iterator iter = hashMap.find(str);

		if (iter != hashMap.end())
		{
			return StringRef(iter->second);
		}

		return 0;
	}

	virtual void freeString(StringRef* stringRef) 
	{
		uint16_t string = stringRef->stringRef;

		EnterCriticalSection(&critSec);

		ScriptStringData* data = &strings[string];
		delete data->data;

		std::string str(data->string, data->length);

		hashMap.erase(str);

		free((void*)data->string);

		if (string < firstFree)
		{
			firstFree = string;
		}

		data->data = NULL;
		data->string = NULL;

		LeaveCriticalSection(&critSec);
	}

	virtual void addRefToString(StringRef* string) 
	{
		InterlockedIncrement(&strings[string->stringRef].data->references);
	}

	virtual void removeRefToString(StringRef* string) 
	{
		if (!InterlockedDecrement(&strings[string->stringRef].data->references))
		{
			freeString(string);
		}

	}

	virtual void addUser(StringRef* string, int user) 
	{
		ScriptStringManagementData* data = strings[string->stringRef].data;

		if (data)
		{
			if (!(data->users & user))
			{
				addRefToString(string);
			}

			_InterlockedOr((long*)&data->users, user);
		}

	}

	virtual const char* convertToString(StringRef* string)
	{
		return strings[string->stringRef].string;
	}

	virtual void setString( StringRef* field, StringRef string ) 
	{
		if (string.stringRef)
		{
			addRefToString(&string);
		}

		if (field->stringRef)
		{
			removeRefToString(field);
		}

		*field = string;
	}

	virtual void shutdownSystem( int system ) 
	{
		EnterCriticalSection(&critSec);

		for (int i = 0; i < MAX_SCRIPT_STRINGS; i++)
		{
			if (strings[i].data)
			{
				ScriptStringManagementData* data = strings[i].data;

				if (data->users & system)
				{
					if (data->users == system)
					{
						//SL_FreeString(i);
					}
					else
					{
						removeRefToString(&StringRef(i));
					}
				}
			}
		}

		LeaveCriticalSection(&critSec);
	}
};

static gbhStringListLocal stringListLocal;
gbhStringList* stringList = &stringListLocal;