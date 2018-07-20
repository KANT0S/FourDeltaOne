// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Base game event manager.
//
// Initial author: NTAuthority
// Started: 2013-03-03
// ==========================================================

#include <gbh.h>
#include <common/events.h>
#include <queue>

class NullEvent : public Event
{
public:
	virtual void handle() {}

	virtual bool isNullEvent()
	{
		return true;
	}
};

class gbhEventManagerLocal : public gbhEventManager
{
private:
	std::queue<Event*> eventQueue;

	NullEvent timeEvent;

	Event* getEvent()
	{
		gameManager->getPlatform()->runEvents();

		if (!eventQueue.empty())
		{
			Event* event = eventQueue.front();
			eventQueue.pop();

			return event;
		}

		timeEvent.setTime(gameManager->getTime());

		return &timeEvent;
	}

public:
	virtual uint64_t handleEvents()
	{
		while (true)
		{
			Event* event = getEvent();

			if (event->isNullEvent())
			{
				return event->getTime();
			}

			event->handle();
			delete event;
		}
	}

	virtual void queueEvent(Event& event)
	{
		eventQueue.push(&event);
	}
};

static gbhEventManagerLocal eventManagerLocal;
gbhEventManager* eventManager = &eventManagerLocal;