// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Public renderer interface.
//
// Initial author: NTAuthority
// Started: 2013-03-03
// ==========================================================

#ifndef _RENDERER_H
#define _RENDERER_H

class gbhRenderer
{
public:
	// initializes the renderer and sets up a target device
	virtual void initialize() = 0;

	// shuts down the rendering subsystem
	virtual void shutdown() = 0;
};

extern gbhRenderer* renderer;

#endif