#define LTM_DESC
#include <tomcrypt.h>
#include <stdio.h>
#include <sys/stat.h>

void GenerateKeyPair()
{
	prng_state prng;
	ecc_key key;

	// init ltc
	register_prng(&fortuna_desc);

	ltc_mp = ltm_desc;

	// initialize the PRNG
	int index = find_prng("fortuna");
	rng_make_prng(512, index, &prng, NULL);

	// generate keypair for ECC
	ecc_make_key(&prng, index, 32, &key);

	unsigned char publicKey[128];
	unsigned char privateKey[128];

	unsigned long publicKeySize = sizeof(publicKey);
	unsigned long privateKeySize = sizeof(privateKey);

	ecc_export(publicKey, &publicKeySize, PK_PUBLIC, &key);
	ecc_export(privateKey, &privateKeySize, PK_PRIVATE, &key);

	// generate a global key/iv
	unsigned char globalKey[32];
	unsigned char globalIV[32];

	fortuna_read(globalKey, 32, &prng);
	fortuna_read(globalIV, 32, &prng);

	// save the public key
	FILE* pubF = fopen("pub.bin", "wb");
	fwrite(publicKey, 1, publicKeySize, pubF);
	fclose(pubF);

	// save the private key
	FILE* privF = fopen("priv.bin", "wb");
	fwrite(privateKey, 1, privateKeySize, privF);
	fclose(privF);

	// save the root key
	FILE* keyF = fopen("erk.bin", "wb");
	fwrite(globalKey, 1, 32, keyF);
	fclose(keyF);

	// save the root iv
	FILE* ivF = fopen("riv.bin", "wb");
	fwrite(globalIV, 1, 32, ivF);
	fclose(ivF);
}

void EncryptFile(const char* filename, const char* outName)
{
	// open the file and read the size
	struct stat s;
	FILE* f = fopen(filename, "rb");
	fstat(f->_file, &s);

	unsigned long size = s.st_size;

	// allocate a buffer to hold the file
	unsigned char* originalData = (unsigned char*)malloc(size);
	unsigned char* encryptedData = (unsigned char*)malloc(size);

	// read in the file and close it
	fread(originalData, 1, size, f);
	fclose(f);

	// initialize libtomcrypt
	register_prng(&fortuna_desc);
	register_cipher(&aes_desc);
	register_cipher(&aes_enc_desc);

	ltc_mp = ltm_desc;

	// initialize the PRNG
	prng_state prng;
	int index = find_prng("fortuna"); // I've no crypto experience at all, but this PRNG seemed nice as it uses sha256; why is there no 'recommended PRNG'?
	rng_make_prng(512, index, &prng, NULL);

	// generate a key pair for this file
	unsigned char key[32];
	unsigned char iv[32];
	memset(key, 0, sizeof(key));
	memset(iv, 0, sizeof(iv));

	fortuna_read(key, 16, &prng);
	fortuna_read(iv, 16, &prng);

	// encrypt the file
	symmetric_CTR ctr;
	int aes = find_cipher("aes");

	ctr_start(aes, iv, key, 16, 0, CTR_COUNTER_LITTLE_ENDIAN, &ctr);
	ctr_encrypt(originalData, encryptedData, size, &ctr);
	ctr_done(&ctr);

	// generate a hash over the encrypted data
	hash_state hash;
	unsigned char fileHash[32];
	sha256_init(&hash);
	sha256_process(&hash, encryptedData, size);
	sha256_done(&hash, fileHash);

	// sign the encrypted data
	unsigned char privateKey[128];

	f = fopen("priv.bin", "rb");
	fread(privateKey, 1, 128, f);
	fclose(f);

	ecc_key ecckey;
	ecc_import(privateKey, sizeof(privateKey), &ecckey);

	unsigned char eccSignature[128];
	memset(eccSignature, 0, sizeof(eccSignature));

	unsigned long eccSignatureLength = sizeof(eccSignature);

	ecc_sign_hash(fileHash, sizeof(fileHash), eccSignature, &eccSignatureLength, &prng, index, &ecckey);

	// read the root key/iv
	unsigned char rootKey[32];
	unsigned char rootIV[32];

	f = fopen("erk.bin", "rb");
	fread(rootKey, 1, 32, f);
	fclose(f);

	f = fopen("riv.bin", "rb");
	fread(rootIV, 1, 32, f);
	fclose(f);

	// encrypt the header with the root key/iv
	unsigned char originalHeader[32 + 32 + 128];
	memcpy(originalHeader, key, sizeof(key));
	memcpy(&originalHeader[32], iv, sizeof(iv));
	memcpy(&originalHeader[64], eccSignature, sizeof(eccSignature));

	unsigned char encryptedHeader[32 + 32 + 128];

	symmetric_CBC cbc;
	cbc_start(aes, rootIV, rootKey, 32, 0, &cbc);
	cbc_encrypt(originalHeader, encryptedHeader, sizeof(originalHeader), &cbc);
	cbc_done(&cbc);

	f = fopen(outName, "wb");
	fwrite(encryptedHeader, 1, sizeof(encryptedHeader), f);
	fwrite(encryptedData, 1, size, f);
	fclose(f);

	printf("\n");
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		printf("usage: cigen <genkey|encrypt [filename] [out]>\n");
		return 1;
	}

	if (!stricmp(argv[1], "genkey"))
	{
		GenerateKeyPair();
	}
	else if (!stricmp(argv[1], "encrypt"))
	{
		if (argc == 2)
		{
			printf("false.\n");
			return 1;
		}

		EncryptFile(argv[2], argv[3]);
	}

	return 0;
}