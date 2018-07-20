// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Windows NT platform initialization code.
//
// Initial author: NTAuthority
// Started: 2013-03-02
// ==========================================================

#include <gbh.h>
#include <common/platform.h>
#include <windows.h>

class gbhWin32Platform : public gbhPlatform
{
private:
	int timePeriod;

	uint32_t lastTime;
public:
	virtual void initialize()
	{
		// set up precision timer
		TIMECAPS caps;
		timeGetDevCaps(&caps, 8);
		timeBeginPeriod(caps.wPeriodMin);

		timePeriod = caps.wPeriodMin;

		lastTime = timeGetTime();
		curTime = 0;
	}

	virtual void shutdown()
	{
		// shut down precision timer
		timeEndPeriod(timePeriod);
	}

private:
	uint64_t curTime;

public:
	virtual uint64_t getTime()
	{
		uint32_t time = timeGetTime();

		// wraparound?
		uint32_t timeDiff;

		if (time < lastTime)
		{
			timeDiff = time + (UINT_MAX - lastTime);
		}
		else
		{
			timeDiff = time - lastTime;
		}

		lastTime = time;

		// add to our global timer
		curTime += timeDiff;

		return curTime;
	}

	virtual void print(const char* message)
	{
		printf(message);
	}

	virtual void runEvents()
	{

	}
};

static gbhWin32Platform win32Platform;

void main()
{
	gameManager->initialize(&win32Platform);

	while (true)
	{
		gameManager->process();
	}
}