// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Global detection/token manager - generates data
//          packets from detections/tokens.
//
// Initial author: NTAuthority
// Started: 2013-02-14
// ==========================================================

#include "aci3.h"
#include "init.h"
#include "detections.h"

#include "lib/qlibc.h"

#define MAX_BUFFER_SIZE 3072

namespace ci
{
	qlist_t* DetectionManager::detections;
	bool DetectionManager::gameFrameRun;
	qdlobj_t DetectionManager::currentDetection;
	void* DetectionManager::threadHandle;

	void DetectionManager::initialize()
	{
		VMProtectBeginMutation(__FUNCTION__);

		// initialize all detections
		detections->lock(detections);

		while (detections->getnext(detections, &currentDetection, false))
		{
			Detection* detection = *(Detection**)currentDetection.data;

			detection->initialize();
		}

		detections->unlock(detections);

		// reset the current detection
		memset(&currentDetection, 0, sizeof(currentDetection));

		// create a thread
		ThreadUtils::createThread(DetectionManager::detectionThread, NULL, &threadHandle);

		gameFrameRun = true;

		VMProtectEnd();
	}

	void DetectionManager::registerDetection(Detection** detection)
	{
		if (!detections)
		{
			detections = qlist();
		}

		detections->addlast(detections, detection, sizeof(Detection*));
	}
	
	void DetectionManager::runDetectionFrame()
	{
		VMProtectBeginVirtualization(__FUNCTION__);

		// if the game frame hasn't run yet, we just exit - the first game frame creates a CI session for us
		if (!gameFrameRun)
		{
			return;
		}

		// create a new detection packet
		BitBuffer packetData;
		packetData.setUseDataTypes(false);
		packetData.writeUInt32(0xCAFEC0DE); // packet signature

		char sessionToken[32];
		main::getDescriptor()->GetSessionToken(sessionToken, sizeof(sessionToken));

		uint32_t sessionHash = qhashfnv1_32(sessionToken, 32);
		packetData.writeUInt32(sessionHash);

		// first, we state all our tokens (these, indeed, are sent in every packet for now)
		addTokensToBuffer(&packetData);

		// then, we run all the detections until we don't have space
		detections->lock(detections);

		while (detections->getnext(detections, &currentDetection, false))
		{
			Detection* detection = *(Detection**)currentDetection.data;

			packetData.writeType(PacketDetection);
			packetData.writeUInt32(detection->getDetectionId());
			
			detection->runDetection(&packetData);

			// housekeeping
			if (packetData.getLength() >= MAX_BUFFER_SIZE)
			{
				break; // note that currentDetection *should* be persisted along to the next entry in this function
			}

			if (currentDetection.next == NULL)
			{
				memset(&currentDetection, 0, sizeof(currentDetection));
				break;
			}
		}

		detections->unlock(detections);

		// and write an end marker
		packetData.writeType(PacketEnd);

		//__asm int 3
		// now, encrypt the packet
		static char packetOutput[8192];
		encryptPacket(packetData, packetOutput, sizeof(packetOutput));

		if (true)
		{
			// and send it, as it's already base64'd
			main::getDescriptor()->SendStatus(packetOutput);
		}

		VMProtectEnd();
	}

	void DetectionManager::addTokensToBuffer(BitBuffer* buffer)
	{
		VMProtectBeginVirtualization(__FUNCTION__);

		qdlobj_t obj;
		memset(&obj, 0, sizeof(obj));
		detections->lock(detections);

		while (detections->getnext(detections, &obj, false))
		{
			Detection* detection = *(Detection**)obj.data;

			int detectionID = detection->getDetectionId();
			int tokenCount = detection->getTokenCount();

			for (int i = 0; i < tokenCount; i++)
			{
				buffer->writeType(PacketToken);

				uint64_t tokenNum = detection->getToken(i);
				uint16_t tokenType = (detectionID ^ (tokenNum & 0xFFFF));
				
				buffer->write(16, &tokenType);
				buffer->write(64, &tokenNum);
			}
		}

		detections->unlock(detections);

		VMProtectEnd();
	}

	DWORD DetectionManager::detectionThread(LPVOID param)
	{
		main::antiDebug();

		while (true)
		{
			// run the current detection stuff
			runDetectionFrame();

			// wait for the next detection run
			LARGE_INTEGER waitTime;
			waitTime.QuadPart = -(10000 * 1000 * 30);

			NtWaitForSingleObject(threadHandle, FALSE, &waitTime);
		}

		return 0;
	}

	void DetectionManager::encryptPacket(BitBuffer& buffer, char* outPacket, int outPacketLen)
	{
		VMProtectBeginMutation(__FUNCTION__);

		static char bufBytes[8192];
		ZeroMemory(bufBytes, sizeof(bufBytes));
		CopyMemory(bufBytes, (char*)buffer.getBytes(), buffer.getLength());

		uint32_t len = buffer.getLength();

		main::getDescriptor()->ImportPacket(bufBytes, len, outPacket, outPacketLen);

		b64_ntop((uint8_t*)outPacket, len + (16 - (len % 16)) + 256, bufBytes, sizeof(bufBytes));
		outPacket[0] = 'r';
		outPacket[2] = 'l';
		outPacket[1] = 'o';
		outPacket[3] = 'l';
		outPacket[4] = ' ';
		strcpy(&outPacket[5], bufBytes);

		VMProtectEnd();
	}
}