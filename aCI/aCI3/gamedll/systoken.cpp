// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Machine GUID token provider.
//
// Initial author: NTAuthority
// Started: 2013-04-16
// ==========================================================

#include "aci3.h"
#include "detection.h"
#include "detections.h"
#include "lib/qlibc.h"

namespace ci
{
	Detection* g_sysTokenDetectionPtr;

	class SysTokenDetection : public Detection
	{
	private:
		uint64_t token;
		char data[36];

	public:
		SysTokenDetection();

		virtual void initialize();

		virtual DetectionId getDetectionId() { return DetectionSysToken; }
		virtual void runDetection(BitBuffer* outBuffer);

		virtual int getTokenCount();
		virtual uint64_t getToken(int tokenNum);
	};

	SysTokenDetection::SysTokenDetection()
	{
		token = NULL;

		g_sysTokenDetectionPtr = this;

		DetectionManager::registerDetection(&g_sysTokenDetectionPtr);
	}

	void SysTokenDetection::initialize()
	{
		VMProtectBeginVirtualization(__FUNCTION__);

		HANDLE hKey;
		
		if (NT_SUCCESS(RegistryUtils::openKey(/*L"\\Registry\\Machine\\Software\\Microsoft\\Cryptography"*/
											  StringObfuscation::deobfuscateString(L"\x93\x9C\xA8\xAB\xA2\xB9\xBD\xBA\xBE\x9A\x88\xA5\xA0\xAA\xA8\xAE\xDA\xE2\xEE\xD3\xDD\xCE\xCE\xD9\xC5\xD3\xE9\xF9\xDA\xD1\xC3\xDF\xDC\xC1\xCB\xD8\xF7\xE9\xDB\xD1\xD7\xD2\xCA\xC3\xD1\xC3\xD1\xC8\xE6"),
											  KEY_READ | KEY_WOW64_64KEY, &hKey)))
		{
			PKEY_VALUE_PARTIAL_INFORMATION result;
			size_t resultLength;

			// MachineGuid
			if (NT_SUCCESS(RegistryUtils::queryKey(hKey, StringObfuscation::deobfuscateString(L"\x82\xAF\xAE\xA4\xA2\xA4\xAC\x8F\xB2\xAF\xA1"), &result, &resultLength)))
			{
				wchar_t wdata[37];
				memcpy(wdata, result->Data, min(result->DataLength, sizeof(wdata)));

				wdata[36] = '\0';

				for (int i = 0; i < 36; i++)
				{
					data[i] = (char)wdata[i];
				}

				// calculate a token
				token = qhashfnv1_64(data, 36);

				hfree(result);
			}

			NtClose(hKey);
		}

		VMProtectEnd();
	}

	void SysTokenDetection::runDetection(BitBuffer* outBuffer)
	{
		VMProtectBeginMutation(__FUNCTION__);

		// write raw addresses and raw tokens to match
		int numTokens = (token) ? 1 : 0;
		outBuffer->write(1, &numTokens);

		for (int i = 0; i < numTokens; i++)
		{
			outBuffer->write(36 * 8, data);
		}

		VMProtectEnd();
	}

	int SysTokenDetection::getTokenCount()
	{
		return (token) ? 1 : 0;
	}

	uint64_t SysTokenDetection::getToken(int tokenNum)
	{
		if (!token)
		{
			return 0;
		}

		return token;
	}

	SysTokenDetection g_sysTokenDetection;
}