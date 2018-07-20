// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Base game event manager.
//
// Initial author: NTAuthority
// Started: 2013-03-03
// ==========================================================

#ifndef _EVENTS_H
#define _EVENTS_H

class Event
{
protected:
	uint64_t eventTime;

public:
	virtual void handle() = 0;

	virtual bool isNullEvent() = 0;

	inline uint64_t getTime() { return eventTime; }
	inline void setTime(uint64_t time) { eventTime = time; }
};

class gbhEventManager
{
public:
	virtual uint64_t handleEvents() = 0;

	virtual void queueEvent(Event& event) = 0;
};

extern gbhEventManager* eventManager;

#endif