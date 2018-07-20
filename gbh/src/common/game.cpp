// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Common game manager context thingamabob.
//
// Initial author: NTAuthority
// Started: 2013-03-03
// ==========================================================

#include <gbh.h>
#include <common/events.h>
#include <renderer/renderer.h>

class gbhGameManagerLocal : public gbhGameManager
{
private:
	gbhPlatform* platform;

	ConVar* com_maxFPS;
	ConVar* timescale;

public:
	virtual void initialize(gbhPlatform* platform)
	{
		// set the platform
		this->platform = platform;

		platform->initialize();

		// write a 'hello' message
		print(CH_NONE, "GBH2 initializing...\n");

		// initialize the game
		stringList->initialize();
		conVarManager->initialize();

		// register common console variables
		com_maxFPS = conVarManager->registerVar("com_maxFPS", 0, "Maximum framerate for the game loop.", CONVAR_ARCHIVED);
		timescale = conVarManager->registerVar("timescale", 1.0f, "Scale time by this amount", CONVAR_CHEAT);

		// initialize further things
		renderer->initialize();
	}

	virtual void shutdown()
	{
		stringList->shutdownSystem(0);
		platform->shutdown();
	}

	virtual gbhPlatform* getPlatform()
	{
		return platform;
	}

private:
	uint64_t lastTime;
	float dT;

public:
	virtual void process()
	{
		// limit FPS and handle events
		uint32_t minMsec = (com_maxFPS->getValue()->integer > 0) ? (1000 / com_maxFPS->getValue()->integer) : 1;
		uint32_t msec = 0;
		uint64_t frameTime;

		do
		{
			frameTime = eventManager->handleEvents();

			msec = (uint32_t)(frameTime - lastTime);
		} while (msec < minMsec);

		// handle time scaling
		float scale = timescale->getValue()->number;
		msec = (uint32_t)(msec * scale);

		if (msec < 1)
		{
			msec = 1;
		}
		else if (msec > 5000)
		{
			msec = 5000;
		}

		if (msec > 500)
		{
			printf(CH_NONE, "Hitch warning: %i msec frame time\n", msec);
		}

		dT = msec / 1000.0f;

		lastTime = frameTime;

	}

	virtual uint64_t getTime()
	{
		return (platform->getTime());
	}

	virtual float getDT()
	{
		return dT;
	}

	virtual void printf(ConsoleChannel channel, const char* format, ...)
	{
		// format message
		va_list ap;
		char buffer[32768];

		va_start(ap, format);
		_vsnprintf(buffer, sizeof(buffer), format, ap);
		va_end(ap);

		buffer[sizeof(buffer) - 1] = '\0';

		// call print function
		print(channel, buffer);
	}

	virtual void print(ConsoleChannel channel, const char* buffer)
	{
		platform->print(buffer);
	}
};

static gbhGameManagerLocal gameManagerLocal;
gbhGameManager* gameManager = &gameManagerLocal;