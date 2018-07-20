// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Legacy CI 'proxying' detection
//
// Initial author: NTAuthority
// Started: 2013-04-17
// ==========================================================

#include "aci3.h"
#include "lib/qlibc.h"
#include "detection.h"
#include "detections.h"
#include "sysutils.h"

typedef void (__cdecl * CI_SendStatusProxy_t)(int status);
extern CI_SendStatusProxy_t CI_SendStatusProxy;

extern "C" void ciSendStatus(int status);

namespace ci
{
	static Detection* g_LegacyCIDetectionPtr;

	class LegacyCIDetection : public Detection
	{
	public:
		LegacyCIDetection();

		virtual void initialize();

		virtual DetectionId getDetectionId() { return DetectionLegacyCI; }
		virtual void runDetection(BitBuffer* outBuffer);

		virtual int getTokenCount();
		virtual uint64_t getToken(int tokenNum);

		void setStatus(uint32_t status);

	private:
		uint32_t ciStatus;
	};

	LegacyCIDetection::LegacyCIDetection()
	{
		g_LegacyCIDetectionPtr = this;

		DetectionManager::registerDetection(&g_LegacyCIDetectionPtr);

		CI_SendStatusProxy = ciSendStatus;
	}

	void LegacyCIDetection::initialize()
	{
		ciStatus = 50001;
	}

	void LegacyCIDetection::runDetection(BitBuffer* outBuffer)
	{
		outBuffer->write(17, &ciStatus);
	}

	int LegacyCIDetection::getTokenCount()
	{
		return 0;
	}

	uint64_t LegacyCIDetection::getToken(int tokenNum)
	{
		return 0;
	}

	void LegacyCIDetection::setStatus(uint32_t status)
	{
		ciStatus = status;
	}

	LegacyCIDetection g_LegacyCIDetection;
}

#include "init.h"

extern ci_descriptor_t* legDesc;

extern "C" void ciSendStatus(int statusN)
{
	//ci::g_LegacyCIDetection.setStatus(status);
	char status[256];
	sprintf(status, "troll %i", statusN);

	legDesc->SendStatus(status);
}