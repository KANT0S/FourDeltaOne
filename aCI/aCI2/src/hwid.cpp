#include "stdinc.h"
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

char* macAddress;

char* getMAC()
{
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	char *mac_addr = (char*)malloc(17);
	
	AdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL) 
	{
		return "nope";
	}
	
	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		AdapterInfo = (IP_ADAPTER_INFO *) malloc(dwBufLen);
		if (AdapterInfo == NULL)
		{
			return "nope";
		}
	}
	
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) 
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		
		sprintf(mac_addr, "%02X%02X%02X%02X%02X%02X",pAdapterInfo->Address[0], pAdapterInfo->Address[1],pAdapterInfo->Address[2], pAdapterInfo->Address[3],pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
		free(AdapterInfo);
		DEBUGprintf(("MAC Address resolved to %s\n", mac_addr));
		return mac_addr;     
	}

	return "nope";
}
