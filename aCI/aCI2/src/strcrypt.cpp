// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: simple STRcrypt without any library functions
//          too keep it as small as possible
//
// Initial author  : JohnD
//
// Started: 2012-01-18
// ==========================================================
#include "stdinc.h"

// string crypt
unsigned char STRcrypt_key[130];
int			  STRcrypt_len=128;

// random generator not dependent on a library
#define constapmc 16807
unsigned long  seed31pmc = 0x5DEECE6D;   
unsigned long s_rand(void)
{
    unsigned long hi, lo;
    lo = constapmc * (seed31pmc & 0xFFFF);
    hi = constapmc * (seed31pmc >> 16);
    lo += (hi & 0x7FFF) << 16;
    lo += hi >> 15;                  
    if (lo > 0x7FFFFFFF) lo -= 0x7FFFFFFF;          
    return ( seed31pmc = (long)lo );        
}

void s_srand(long unsigned int seedin)
{
    if (seedin != 0) seed31pmc = seedin;
}

// very simple unicode 2 string copy
int s_ustrcpy(char *ustr, char *str, int maxlen)
{
	int len=0;
	while (len<maxlen && ustr[0]!=0 || ustr[1]!=0) {
		len++;
		*str++=*ustr++;
		ustr++;
	}
	*str='\0';
	return len;
}

// simple function to randomize code after it has been used
void    FUNCclear(BYTE *adr)
{
	int len=0;
	while ( (*(DWORD *)adr)!=(DWORD)0xcccccccc) {
		len++;
		*adr++=(BYTE)(s_rand()%0xff);
	}
	*(DWORD *)adr=s_rand();
	DEBUGprintf(("clear %08x %d\n",adr,len));
}


// standard unix ELF hash
// adapted to work caseinsensitive and ignore unicode
DWORD	STRhash(char *str, int length, int casesensitive, int incr)
{
	DWORD dwHash;
	// determine length of str if not given
	if( length==0 ) {
		while( str[length]!=0 || (incr>1 && str[length+1]!=0) )  length+=incr;
	}
	length/=incr;
	dwHash = 0;
	do
	{
		dwHash = _rotr( (DWORD)dwHash, 13 );
		// normalize to uppercase if we need to ignore the case
		if( !casesensitive && *((BYTE *)str) >= 'a' )
			dwHash += *((BYTE *)str) - 0x20;
		else
			dwHash += *((BYTE *)str);
		str+=incr;
	} while( --length );
	return dwHash;
}

char *STRint_crypt(int typ,char *src,char *dst)
{
    int i,seed,swcrypt;
	seed=(int)((unsigned long)(src)%STRcrypt_len);	// use address of string as seed
	swcrypt=((unsigned char)src[0]>=0x80)?1:0;		// determine if src is currently crypted or not
    if (dst==0) dst=src;
    for(i=0; src[i]!=0; i++) {
        if (seed>=STRcrypt_len) seed%=STRcrypt_len;	// wrap it
		if (typ!=swcrypt)							// it differs so we have to encrypt/decrypt 
	        dst[i]=src[i]^STRcrypt_key[seed++];
		else
			dst[i]=src[i];							// same as it currently is, so just copy
	}
    dst[i]='\0';
	return dst;
}  

char *STRcrypt(char *src,char *dst)
{
	return STRint_crypt(STR_CRYPT,src,dst);
}

char *STRdecrypt(char *src,char *dst)
{
	return STRint_crypt(STR_DECRYPT,src,dst);
}


void STRcryptInit(unsigned long seed)
{
    DEBUGprintf(("STRcryptInit\n"));

	if (seed==0) seed=(unsigned long)(STRcryptInit);		// use this function address as seed...
	s_srand(seed);	
	for (int i=0; i<STRcrypt_len; i++)  STRcrypt_key[i]=(s_rand()%0x7f)|0x80;
	return;
SCANMARK
}
