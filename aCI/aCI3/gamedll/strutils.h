// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: string utility functions
//
// Initial author: NTAuthority
// Started: 2013-01-06
// ==========================================================

#pragma once

#define tolower(a) (((a) >= 'A' && (a) <= 'Z') ? ((a) + 32) : (a))

namespace ci
{
	class StringUtils
	{
	private:
		StringUtils();
	public:
		static bool equals(const char* a, const char* b, bool ignoreCase);

		static bool equals(const wchar_t* a, const wchar_t* b, size_t length);

		static size_t findPos(const wchar_t* str, wchar_t ch);

		static inline size_t length(const wchar_t* str)
		{
			size_t length = 0;

			while (*str)
			{
				length++;
				str++;
			}

			return length;
		}
	};
}