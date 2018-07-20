// ==========================================================
// GBH2 project
// 
// Component: common
// Purpose: Common include header file.
//
// Initial author: NTAuthority
// Started: 2013-03-02
// ==========================================================

#include <gbh.h>
#include <renderer/renderer.h>

class gbhRendererLocal : public gbhRenderer
{
public:
	void initialize()
	{
		gameManager->print(CH_NONE, "Hello!\n");
	}

	void shutdown()
	{
		gameManager->print(CH_NONE, "Bye!\n");
	}
};

static gbhRendererLocal rendererLocal;
gbhRenderer* renderer = &rendererLocal;


extern "C"
{ 
#ifdef WIN32
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
#else
	unsigned int NvOptimusEnablement = 0x00000001;
#endif
}

