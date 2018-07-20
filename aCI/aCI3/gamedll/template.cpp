// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Detection template.
//
// Initial author: NTAuthority
// Started: 2013-01-06
// ==========================================================

#include "aci3.h"
#include "lib/qlibc.h"
#include "detection.h"
#include "detections.h"
#include "sysutils.h"

namespace ci
{
	static Detection* g_TDetectionPtr;

	class TDetection : public Detection
	{
	public:
		TDetection();

		virtual void initialize();

		virtual DetectionId getDetectionId() { return DetectionMACToken; }
		virtual void runDetection(BitBuffer* outBuffer);

		virtual int getTokenCount();
		virtual uint64_t getToken(int tokenNum);
	};

	TDetection::TDetection()
	{
		g_TDetectionPtr = this;

		//DetectionManager::registerDetection(&g_TDetectionPtr);
	}

	void TDetection::initialize()
	{
		
	}

	void TDetection::runDetection(BitBuffer* outBuffer)
	{
		
	}

	int TDetection::getTokenCount()
	{
		return 0;
	}

	uint64_t TDetection::getToken(int tokenNum)
	{
		return 0;
	}

	TDetection g_TDetection;
}