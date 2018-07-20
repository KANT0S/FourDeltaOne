// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: MAC address token provider/status service.
//
// Initial author: NTAuthority
// Started: 2013-01-06
// ==========================================================

#include "aci3.h"
#include "lib/qlibc.h"
#include "detection.h"
#include "detections.h"
#include "sysutils.h"
#include <ipexport.h>
#include "tdiinfo.h"
#include <tcpioctl.h>
#include <setupapi.h>

#define INITGUID
#include <devguid.h>
#include <devpkey.h>
#undef INITGUID

namespace ci
{
	Detection* g_macTokenDetectionPtr;

// =======================================================
// MAC address token source
// -------------------------------------------------------
// Means: Uses IOCTLs to \Device\Ip/Tcp (general NT),
// \Device\Nsi (Vista+) and calls the IPHLPAPI public API
// to get MAC addresses and check for mismatches (i.e.
// hooking attempts).
//
// In addition, this should check for any changed registry
// NetworkAddress values (using native APIs only), and
// ignore MACs found from such a source.
//
// As well, the PnP manager should be queried as to
// whether the NIC is a PCI/USB device - virtual devices
// commonly generate bad MACs.
//
// Known weaknesses:
//  - kernel-mode hooking of IOCTLs
//  - tracing back of IPHLPAPI calls (should be VM'd)
//  - hardware-based MAC changes (i.e. NIC firmware)
//  - SetupAPI/CFGMGR hooking
// =======================================================

#define MAX_MAC_TOKENS 10

	class MACTokenDetection : public Detection
	{
	public:
		MACTokenDetection();

		virtual void initialize();

		virtual DetectionId getDetectionId() { return DetectionMACToken; }
		virtual void runDetection(BitBuffer* outBuffer);

		virtual int getTokenCount();
		virtual uint64_t getToken(int tokenNum);

	private:
		struct MACData
		{
			int numTokens;

			char addresses[MAX_MAC_TOKENS][6];
			uint64_t tokens[MAX_MAC_TOKENS];
		};

		MACData dataIP;
		HANDLE hTCP;

		bool openTCPDevice(HANDLE* handle);
		void obtainUsingTCP(MACData* data);

		NTSTATUS performTCPQuery(TCP_REQUEST_QUERY_INFORMATION_EX& req, void* outBuffer, ULONG* outLength);

		bool isPhysicalDevice(const wchar_t* name, size_t nameLen);
		void addMACToken(MACData* data, const uchar* physAddr, size_t addrLen);

		uint32_t legacyHash(const uint8_t* addr);
	};

	MACTokenDetection::MACTokenDetection()
	{
		g_macTokenDetectionPtr = this;

		DetectionManager::registerDetection(&g_macTokenDetectionPtr);
	}

	void MACTokenDetection::initialize()
	{
		obtainUsingTCP(&dataIP);
	}

	void MACTokenDetection::obtainUsingTCP(MACData* data)
	{
		VMProtectBeginVirtualization("obtainUsingTCP");

		if (!openTCPDevice(&hTCP))
		{
			return;
		}

		// get all entities
		TDIEntityID* entityIDs = (TDIEntityID*)halloc(sizeof(TDIEntityID) * 64);
		TCP_REQUEST_QUERY_INFORMATION_EX req;
		memset(&req, 0, sizeof(req));

		req.ID.toi_entity.tei_entity = GENERIC_ENTITY;
		req.ID.toi_entity.tei_instance = 0;

		req.ID.toi_class = INFO_CLASS_GENERIC;
		req.ID.toi_type = INFO_TYPE_PROVIDER;
		req.ID.toi_id = ENTITY_LIST_ID;

		ULONG destSize = sizeof(TDIEntityID) * 64;

		NTSTATUS status;

		if (!NT_SUCCESS(performTCPQuery(req, entityIDs, &destSize)))
		{
			return;
		}
		
		// get interfaces
		for (int i = 0; i < (destSize / sizeof(TDIEntityID)); i++)
		{
			TDIEntityID id = entityIDs[i];
			
			req.ID.toi_class = INFO_CLASS_GENERIC;
			req.ID.toi_type = INFO_TYPE_PROVIDER;
			req.ID.toi_entity = id;
			req.ID.toi_id = ENTITY_TYPE_ID;

			if (id.tei_entity == IF_ENTITY)
			{
				ULONG entrySize = sizeof(IFEntry) + 256 + 1;
				IFEntry* entry = (IFEntry*)halloc(entrySize);

				req.ID.toi_class = INFO_CLASS_PROTOCOL;
				req.ID.toi_id = IF_MIB_STATS_ID;

				if (!NT_SUCCESS(performTCPQuery(req, entry, &entrySize)))
				{
					continue;
				}

				// check if it's a PCI/USB/VMBus device
				if (isPhysicalDevice((wchar_t*)entry->if_descr, entry->if_descrlen))
				{
					addMACToken(data, entry->if_physaddr, entry->if_physaddrlen);
				}

				hfree(entry);
			}
		}

		// and free it
		hfree(entityIDs);

		NtClose(hTCP);

		VMProtectEnd();
	}

	bool MACTokenDetection::isPhysicalDevice(const wchar_t* name, size_t nameLen)
	{
		VMProtectBeginUltra("IsPhysicalDevice");

		int index = 0;

		SP_DEVINFO_DATA data;
		memset(&data, 0, sizeof(data));
		data.cbSize = sizeof(data);

		HDEVINFO handle = SetupDiGetClassDevs(&GUID_DEVCLASS_NET, NULL, NULL, DIGCF_PRESENT);

		while (SetupDiEnumDeviceInfo(handle, index, &data))
		{
			DEVPROPTYPE propType;
			DWORD propSize;

			if (!SetupDiGetDeviceRegistryPropertyW(handle, &data, SPDRP_DEVICEDESC, NULL, NULL, 0, &propSize))
			{
				wchar_t* propName = (wchar_t*)halloc(propSize);
				
				if (SetupDiGetDeviceRegistryPropertyW(handle, &data, SPDRP_DEVICEDESC, NULL, (PBYTE)propName, propSize, NULL))
				{
					if (StringUtils::equals(propName, name, min(StringUtils::findPos(name, '#') - 1, nameLen / 2)))
					{
						char buffer[1024];
						SetupDiGetDeviceRegistryPropertyA(handle, &data, SPDRP_HARDWAREID, NULL, (PBYTE)buffer, sizeof(buffer), NULL);

						bool isPhysical = false;

						if (tolower(buffer[0]) == 'p' && tolower(buffer[1]) == 'c' && tolower(buffer[2]) == 'i')
						{
							isPhysical = true;
						}
						else if (tolower(buffer[0]) == 'u' && tolower(buffer[1]) == 's' && tolower(buffer[2]) == 'b')
						{
							isPhysical = true;
						}
						else if (tolower(buffer[0]) == 'v' && tolower(buffer[1]) == 'm' && tolower(buffer[2]) == 'b')
						{
							isPhysical = true;
						}
						// vms_mp; not vms_vsmp!
						else if (tolower(buffer[0]) == 'v' && tolower(buffer[1]) == 'm' && tolower(buffer[4]) == 'm') // HyperV driver; the driver mirrors the MAC of the physical NIC
						{
							isPhysical = true;
						}

						hfree(propName);
						SetupDiDestroyDeviceInfoList(handle);

						return isPhysical;
					}
				}

				hfree(propName);
			}

			index++;
		}

		SetupDiDestroyDeviceInfoList(handle);

		return false;

		VMProtectEnd();
	}

	void MACTokenDetection::addMACToken(MACData* data, const uchar* physAddr, size_t addrLen)
	{
		VMProtectBeginMutation("addMACToken");

		// don't use the token if this is a legacy hash that's blacklisted
		uint32_t legacyHash = this->legacyHash(physAddr);

		// list from http://localhost/aiw3/forums/viewtopic.php?f=30&t=52750
		if (legacyHash == 0x57B3821C || legacyHash == 0x6C79E634 || legacyHash == 0x6A9528FD || legacyHash == 0xE2642C56 || legacyHash == 0x75A01668 || legacyHash == 0x1A297F2B)
		{
			return;
		}
		
		// hash the token and add the address + token to the list
		uint64_t tokenHash = qhashfnv1_64(physAddr, addrLen);

		// look for the token first
		for (int i = 0; i < data->numTokens; i++)
		{
			if (data->tokens[i] == tokenHash)
			{
				return;
			}
		}

		int nextToken = data->numTokens;

		if (nextToken > (MAX_MAC_TOKENS - 1))
		{
			return;
		}

		// add the MAC
		memcpy(data->addresses[nextToken], physAddr, min(addrLen, 6));

		// hash the token
		data->tokens[nextToken] = tokenHash;

		data->numTokens++;

		VMProtectEnd();
	}
	
	NTSTATUS MACTokenDetection::performTCPQuery(TCP_REQUEST_QUERY_INFORMATION_EX& req, void* outBuffer, ULONG* outLength)
	{
		if (SystemUtils::isWOW64())
		{
			// for WOW64 we need to use the structure with 8-byte alignment on the Context field
			TCP_REQUEST_QUERY_INFORMATION_EX64 req64;
			memcpy(&req64.ID, &req.ID, sizeof(req.ID));
			memcpy(&req64.Context, req.Context, sizeof(req.Context));

			return FileUtils::performIOCTL(hTCP, IOCTL_TCP_QUERY_INFORMATION_EX, &req64, sizeof(req64), outBuffer, outLength);
		}

		// use the structure directly
		return FileUtils::performIOCTL(hTCP, IOCTL_TCP_QUERY_INFORMATION_EX, &req, sizeof(req), outBuffer, outLength);
	}

	bool MACTokenDetection::openTCPDevice(HANDLE* handle)
	{
		// \Device\Tcp
		if (FileUtils::openDevice(StringObfuscation::deobfuscateString(L"\x93\x8A\xA8\xBA\xA2\xA9\xAC\x94\x93\xA5\xB5"), handle) != STATUS_SUCCESS)
		{
			return false;
		}

		return true;
	}

	void MACTokenDetection::runDetection(BitBuffer* outBuffer)
	{
		// write raw addresses and raw tokens to match
		outBuffer->write(4, &dataIP.numTokens);
		
		for (int i = 0; i < dataIP.numTokens; i++)
		{
			outBuffer->write(6 * 8, dataIP.addresses[i]);
			outBuffer->write(64, &dataIP.tokens[i]);
		}
	}

	int MACTokenDetection::getTokenCount()
	{
		return dataIP.numTokens;
	}

	uint64_t MACTokenDetection::getToken(int tokenNum)
	{
		if (tokenNum > dataIP.numTokens)
		{
			return 0;
		}

		return dataIP.tokens[tokenNum];
	}

	uint32_t MACTokenDetection::legacyHash(const uint8_t* addr)
	{
		size_t len = 6;
		
		unsigned int value = 0,temp = 0;
		for(size_t i=0;i<len;i++)
		{
			temp = addr[i];
			temp += value;
			value = temp << 10;
			temp += value;
			value = temp >> 6;
			value ^= temp;
		}
		temp = value << 3;
		temp += value;
		unsigned int temp2 = temp >> 11;
		temp = temp2 ^ temp;
		temp2 = temp << 15;
		value = temp2 + temp;
		if(value < 2) value += 2;
		return value;
	}

	MACTokenDetection g_macTokenDetection;
}