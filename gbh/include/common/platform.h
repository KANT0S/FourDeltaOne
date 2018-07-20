// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Base platform-specific interface.
//
// Initial author: NTAuthority
// Started: 2013-03-03
// ==========================================================

#include <stdint.h>

#ifndef _PLATFORM_H
#define _PLATFORM_H

class gbhPlatform
{
public:
	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual void runEvents() = 0;

	virtual void print(const char* message) = 0;

	virtual uint64_t getTime() = 0;
};

#endif