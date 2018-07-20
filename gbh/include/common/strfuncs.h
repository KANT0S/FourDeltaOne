// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: String-related functions.
//
// Initial author: NTAuthority
// Started: 2013-03-02
// ==========================================================

#ifndef _STRFUNCS_H
#define _STRFUNCS_H

#include <functional>
#include <ctype.h>

namespace std
{
	template<>
	struct hash<const char*>
	{
		inline size_t operator()(const char* val) const
		{
			unsigned long hash = 0;

			for ( ; *val; ++val)
			{
				hash = 5 * hash + *val;
			}

			return size_t(hash);
		};
	};
}

inline const char* strtolower(const char* str)
{
	char* string = _strdup(str);

	for (char* p = string; *p; ++p)
	{
		*p = tolower(*p);
	}

	return string;
}

#endif