// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: String list - safe allocation/freeing of strings.
//
// Initial author: NTAuthority
// Started: 2013-03-02
// ==========================================================

#ifndef _STRINGLIST_H
#define _STRINGLIST_H

class gbhStringList;

class StringRef
{
friend class gbhStringListLocal;

private:
	uint16_t stringRef;

	StringRef(uint16_t stringRef) { this->stringRef = stringRef; }

public:
	const char* toString();
	void free();

	void addRef();
	void removeRef();

	void addUser(int user);

	inline bool isValid() { return stringRef != 0; }
};

class gbhStringList
{
friend class StringRef;

public:
	virtual void initialize() = 0;
	
	virtual StringRef getString(const char* string) = 0;
	virtual StringRef getString(const char* string, int size) = 0;
	virtual StringRef getString(const char* string, int size, int system) = 0;
	
	virtual StringRef findString(const char* string) = 0;
	virtual StringRef findString(const char* string, int size) = 0;

protected:
	virtual void freeString(StringRef* string) = 0;

	virtual void addRefToString(StringRef* string) = 0;
	virtual void removeRefToString(StringRef* string) = 0;

	virtual void addUser(StringRef* string, int user) = 0;

	virtual const char* convertToString(StringRef* string) = 0;

public:
	// sets the field pointed to by [field] to [string], freeing any string that was there before
	virtual void setString(StringRef* field, StringRef string) = 0;

	virtual void shutdownSystem(int system) = 0;

	// strdup, but managed by us
	inline const char* duplicateWithRef(const char* string)
	{
		StringRef ref = getString(string);
		return convertToString(&ref);
	}

	inline void freeDuplicate(const char* string)
	{
		StringRef ref = findString(string);

		if (ref.isValid())
		{
			freeString(&ref);
		}
	}
};

extern gbhStringList* stringList;

inline void StringRef::addRef()
{
	stringList->addRefToString(this);
}

inline void StringRef::addUser(int user)
{
	stringList->addUser(this, user);
}

inline void StringRef::free()
{
	stringList->freeString(this);
}

inline void StringRef::removeRef()
{
	stringList->removeRefToString(this);
}

inline const char* StringRef::toString()
{
	return stringList->convertToString(this);
}

#endif