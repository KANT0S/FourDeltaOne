// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: string utility functions
//
// Initial author: NTAuthority
// Started: 2013-01-06
// ==========================================================

#include "aci3.h"

namespace ci
{
	bool StringUtils::equals(const char* a, const char* b, bool ignoreCase)
	{
		for (; *a != 0, *b != 0; a++, b++)
		{
			if (ignoreCase)
			{
				if (tolower(*a) != tolower(*b))
				{
					return false;
				}
			}
			else
			{
				if (*a != *b)
				{
					return false;
				}
			}
		}

		if ((*a == 0 && *b != 0) || (*a != 0 && *b == 0))
		{
			return false;
		}

		return true;
	}

	bool StringUtils::equals(const wchar_t* a, const wchar_t* b, size_t length)
	{
		if (length == 0)
		{
			return true;
		}

		do
		{
			//if (tolower(*a) != tolower(*b++))
			if (*a != *b++)
			{
				return false;
			}

			if (*a++ == 0)
			{
				break;
			}
		} while (--length != 0);

		return true;
	}

	size_t StringUtils::findPos(const wchar_t* str, wchar_t ch)
	{
		const wchar_t* p = str;

		while (*p != L'\0')
		{
			if (*p == ch)
			{
				return p - str;
			}

			p++;
		}

		return (length(str) - 1);
	}

}