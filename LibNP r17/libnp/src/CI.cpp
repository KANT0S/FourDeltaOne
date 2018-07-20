// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: aCI loader
//
// Initial author: NTAuthority
// Started: 2012-01-18
// ==========================================================

#include "StdInc.h"
#include <MemoryModule.h>
#include <sys/stat.h>

//#define TESTING_ACI

void CI_InitDLL();

#define LTM_DESC
#include <tomcrypt.h>

// 1 MB ought to do for now
// 2013-02-07: made it 4MB due to ExtDLL
#define MAX_PAYLOAD_SIZE 4 * 1024 * 1024

static struct  
{
	uint8_t payload[MAX_PAYLOAD_SIZE];
	uint8_t decryptedPayload[MAX_PAYLOAD_SIZE];

	uint8_t gamePayload[MAX_PAYLOAD_SIZE];
	uint8_t decryptedGamePayload[MAX_PAYLOAD_SIZE];

	HMEMORYMODULE ciDLL;
	bool initialized;
} g_ci;

static const uint8_t g_key[] = 
{
	0xA6,0xAD,0xAC,0x9C,0x46,0xF1,0x03,0xDD,0x5B,0x6A,0x2A,
	0x3E,0x13,0x33,0x3C,0xC1,0x29,0x9B,0x6D,0xCF,0x54,0xC0,
	0x43,0x1C,0xFA,0xDE,0xDA,0xBA,0xB4,0xF4,0xD8,0xAE,
};

static const uint8_t g_iv[] = 
{
	0x9D,0x6F,0x9D,0x00,0xFD,0xA5,0xAB,0x47,0xAF,0x80,0xAF,
	0xA3,0x6A,0xA3,0x57,0xCB,0xC6,0xF6,0xD4,0x47,0xC4,0x79,
	0x36,0xAD,0x6D,0xD6,0x50,0xBD,0x81,0xAB,0x0B,0x00,
};

static const uint8_t g_pub[] =
{
	0x30,0x4B,0x03,0x02,0x07,0x00,0x02,0x01,0x20,0x02,0x20,
	0x35,0xD7,0xFC,0x01,0x98,0x49,0x25,0xE0,0x42,0x9D,0x8B,
	0x94,0x53,0xAE,0xDC,0x87,0xAB,0x07,0x4F,0xCE,0x0E,0xA0,
	0xD2,0x65,0x94,0x24,0xE8,0x93,0x17,0x0F,0x01,0x6C,0x02,
	0x20,0x16,0xC2,0xE9,0xBB,0xDB,0x4B,0xD1,0xC0,0x52,0x3A,
	0xFA,0xE7,0x3F,0x7E,0x6F,0x0B,0x0E,0xCD,0xE6,0xAE,0x35,
	0xA4,0xCC,0x59,0xCC,0x0C,0x62,0x6D,0x65,0x91,0x70,0xA0,
};

struct ci_payload_header
{
	uint8_t key[32];
	uint8_t iv[32];
	uint8_t signature[128]; // 80 should be fine for a 256-bit key's ECDSA signature
};

// downloads the GameRT.dat (aCI2.dll) from the NP server
void CI_ObtainedRT(NPAsync<NPGetPublisherFileResult>* async);

void CI_Initialize(char version)
{
	if (g_ci.initialized)
	{
		return;
	}

	g_ci.initialized = true;

	char str[11];
	str[0] = '\xdb';
	str[1] = '\xfd';
	str[2] = '\xf1';
	str[3] = '\xf9';
	str[4] = '\xce';
	str[5] = '\xc8';
	str[6] = '\xb2';
	str[7] = '\xf8';
	str[8] = '\xfd';
	str[9] = '\xe8';

	for (int i = 0; i < 10; i++)
	{
		str[i] = str[i] ^ version;
	}

	str[10] = '\0';

	NPAsync<NPGetPublisherFileResult>* async = NP_GetPublisherFile(str, g_ci.payload, MAX_PAYLOAD_SIZE);
	async->SetCallback(CI_ObtainedRT, "emag");
}

void CI_Dummy(NPAsync<NPGetPublisherFileResult>* async)
{

}

void CI_InitializeGameDLL(const char* dllName)
{
	if (GetFileAttributesA(dllName) != INVALID_FILE_ATTRIBUTES)
	{
		LoadLibraryA(dllName);
		return;
	}

	NPAsync<NPGetPublisherFileResult>* async = NP_GetPublisherFile(dllName, g_ci.gamePayload, MAX_PAYLOAD_SIZE);
	//async->SetCallback(CI_ObtainedRT, "game");
	async->SetCallback(CI_Dummy, "game");
	async->Wait();

	CI_ObtainedRT(async);
}

void CI_ObtainedRT(NPAsync<NPGetPublisherFileResult>* async)
{
	NPGetPublisherFileResult* result = async->GetResult();
	bool isGame = (((const char*)async->GetUserData())[0] == 'g');

	if (result->result != GetFileResultOK)
	{
		return;
	}
	/*NPGetPublisherFileResult* result = new NPGetPublisherFileResult;
	
	struct stat s;
	FILE* f = fopen("libnp.bin", "rb");
	fstat(f->_file, &s);

	unsigned long size = s.st_size;

	// allocate a buffer to hold the file
	unsigned char* originalData = (unsigned char*)malloc(size);

	// read in the file and close it
	fread(originalData, 1, size, f);
	fclose(f);

	result->buffer = originalData;
	result->fileSize = size;*/
	
	if (result->fileSize >= MAX_PAYLOAD_SIZE)
	{
		// CI module doesn't fit, exit the process here
		ExitProcess(0x8000C0DE);
		__asm int 3
		return;
	}

	// initialize libtomcrypt
	register_cipher(&aes_desc);

	ltc_mp = ltm_desc;

	char name[4];
	name[0] = 'a'; name[1] = 'e'; name[2] = 's'; name[3] = '\0';

	int aes = find_cipher(name);

	// first, decrypt the header
	ci_payload_header header;

	symmetric_CBC cbc;
	cbc_start(aes, g_iv, g_key, 32, 0, &cbc);
	cbc_decrypt(result->buffer, (unsigned char*)&header, sizeof(header), &cbc);
	cbc_done(&cbc);

	// hash the encrypted data
	uint8_t* payloadData = &result->buffer[sizeof(ci_payload_header)];
	uint32_t payloadSize = result->fileSize - sizeof(ci_payload_header);

	hash_state hash;
	unsigned char fileHash[32];
	sha256_init(&hash);
	sha256_process(&hash, payloadData, payloadSize);
	sha256_done(&hash, fileHash);

	// validate the data signature
	ecc_key ecckey;
	int sigResult = 0;
	ecc_import(g_pub, sizeof(g_pub), &ecckey);
	ecc_verify_hash(header.signature, sizeof(header.signature), fileHash, sizeof(fileHash), &sigResult, &ecckey);

	if (sigResult == 0)
	{
		// crash here
		__asm
		{
			push 1
			retn
		}
	}

	// decrypt the payload
	symmetric_CTR ctr;

	ctr_start(aes, header.iv, header.key, 16, 0, CTR_COUNTER_LITTLE_ENDIAN, &ctr);
	ctr_decrypt(payloadData, (isGame) ? g_ci.decryptedGamePayload : g_ci.decryptedPayload, payloadSize, &ctr);
	ctr_done(&ctr);

#ifdef TESTING_ACI
	FILE* fp = fopen("aCI2.dll", "rb");
	if (fp)
	{
		memset((void *)g_ci.decryptedPayload, 0, payloadSize);  // clear original
		fseek(fp,0,SEEK_END);
		payloadSize = ftell(fp);
		rewind(fp);
		fread((void*)g_ci.decryptedPayload, 1, payloadSize, fp);
		fclose(fp);
	}
#endif

	// load the aCI2.dll (why am I calling it '2' anyways? it's more weird than 'aCI 1.2'...)
	if (!isGame)
	{
		g_ci.ciDLL = MemoryLoadLibrary(g_ci.decryptedPayload);
		CI_InitDLL();
	}
	else
	{
		MemoryLoadLibrary(g_ci.decryptedGamePayload);
	}
}

void CI_Shutdown()
{

}

static void* ci_functions[] = 
{
	NP_SendRandomString,
	NP_GetNPID
};

void CI_GetFunctionPointers(void*** pointers, int* num)
{
	*pointers = ci_functions;
	*num = sizeof(ci_functions) / sizeof(void*);
}

void CI_SendStatus(const char* status)
{
	RPCStorageSendRandomStringMessage* request = new RPCStorageSendRandomStringMessage();
	request->GetBuffer()->set_randomstring(status);

	RPC_SendMessage(request);

	request->Free();
}

typedef struct
{
	void (__cdecl * GetFunctionPointers)(void*** pointers, int* num);
	void (__cdecl * SendStatus)(const char* status);
} ci_descriptor_t;

static ci_descriptor_t ci_descriptor =
{
	CI_GetFunctionPointers,
	CI_SendStatus
};

void CI_InitDLL()
{
	void(*a)(void*) = (void(*)(void*))MemoryGetProcAddress(g_ci.ciDLL, "a");

	if (a)
	{
		a(&ci_descriptor);
	}
}