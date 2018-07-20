// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Common game manager context thingamabob.
//
// Initial author: NTAuthority
// Started: 2013-03-03
// ==========================================================

#include <stdint.h>
#include <common/platform.h>

#ifndef _GAME_H
#define _GAME_H

// TODO: move to a separate header
enum ConsoleChannel
{
	CH_NONE,
	CH_ERROR,
	CH_OGRE,

};

class gbhGameManager
{
public:
	virtual void initialize(gbhPlatform* platform) = 0;
	virtual void shutdown() = 0;
	virtual void process() = 0;

	virtual void print(ConsoleChannel channel, const char* format) = 0;
	virtual void printf(ConsoleChannel channel, const char* format, ...) = 0;

	virtual gbhPlatform* getPlatform() = 0;

	virtual uint64_t getTime() = 0;
	virtual float getDT() = 0;
};

extern gbhGameManager* gameManager;
#endif