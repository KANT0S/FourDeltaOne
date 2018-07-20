// simple STRcrypt without any library functions
#ifndef STRCRYPT_H
#define STRCRYPT_H
#include <windows.h>

#define STR_DECRYPT	0
#define STR_CRYPT	1

void			 s_srand(long unsigned int seedin);
unsigned long	 s_rand(void);
int				 s_ustrcpy(char *ustr, char *str, int maxlen);


void			 FUNCclear(BYTE *adr);
DWORD			 STRhash(char *str, int length=0, int casesensitive=0, int incr=1);
char			*STRcrypt(char *src,char *dst=0);
char			*STRdecrypt(char *src,char *dst=0);
void			 STRcryptInit(unsigned long seed=0);

#endif
