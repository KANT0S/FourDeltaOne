#include "StdInc.h"
#include "dw.h"
#include <tomcrypt.h>

static bool cryptoInited;

static void dw_init_crypto()
{
	if (cryptoInited)
	{
		return;
	}

	register_cipher(&des3_desc);

	cryptoInited = true;
}

void dw_calculate_iv(unsigned int seed, BYTE* iv)
{
	hash_state hash;
	tiger_init(&hash);
	tiger_process(&hash, (unsigned char*)&seed, sizeof(seed));
	tiger_done(&hash, iv);
}

static BYTE globalKey[24];

void dw_get_global_key(BYTE* key)
{
	memcpy(key, globalKey, sizeof(globalKey));
}

void dw_set_global_key(const BYTE* key)
{
	memcpy(globalKey, key, sizeof(globalKey));
}

void dw_decrypt_data(const char* ctext, BYTE* iv, BYTE* key, char* ptext, int len)
{
	dw_init_crypto();

	symmetric_CBC cbc;
	int des3 = find_cipher("3des");

	cbc_start(des3, iv, key, 24, 0, &cbc);
	cbc_decrypt((const BYTE*)ctext, (BYTE*)ptext, len, &cbc);
	cbc_done(&cbc);
}

void dw_encrypt_data(const char* ptext, BYTE* iv, BYTE* key, char* ctext, int len)
{
	dw_init_crypto();

	symmetric_CBC cbc;
	int des3 = find_cipher("3des");

	cbc_start(des3, iv, key, 24, 0, &cbc);
	cbc_encrypt((const BYTE*)ptext, (BYTE*)ctext, len, &cbc);
	cbc_done(&cbc);
}