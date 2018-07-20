// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Global detection/token manager header file.
//
// Initial author: NTAuthority
// Started: 2013-02-14
// ==========================================================

#pragma once

#include "lib/qlibc.h"
#include "detection.h"

namespace ci
{
	enum PacketDataType
	{
		PacketToken,
		PacketDetection,
		PacketEnd
	};

	class DetectionManager
	{
	private:
		static qlist_t* detections;
		static qdlobj_t currentDetection;
		static bool gameFrameRun;

		static HANDLE threadHandle;
		static DWORD detectionThread(LPVOID param);

		static void runDetectionFrame();
		static void addTokensToBuffer(BitBuffer* buffer);

		static void encryptPacket(BitBuffer& buffer, char* outPacket, int outPacketLen);

	public:
		static void initialize();

		static void registerDetection(Detection** detection);
	};
}