// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Base abstract 'detection' class.
//
// Initial author: NTAuthority
// Started: 2013-01-06
// ==========================================================

#pragma once
#include "bitbuffer.h"

namespace ci
{
	enum DetectionId
	{
		DetectionMACToken,
		DetectionSysToken,
		DetectionLegacyCI,

	};

	class Detection
	{
	public:
		virtual void initialize() = 0;

		virtual DetectionId getDetectionId() = 0;
		virtual void runDetection(BitBuffer* outBuffer) = 0;

		virtual int getTokenCount() = 0;
		virtual uint64_t getToken(int tokenNum) = 0;
	};
}