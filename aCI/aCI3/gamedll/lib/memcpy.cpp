#include "aci3.h"

void * __cdecl memcpy (
        void * dst,
        const void * src,
        size_t count
        )
{
        void * ret = dst;

        /*
         * copy from lower addresses to higher addresses
         */
        while (count--) {
                *(char *)dst = *(char *)src;
                dst = (char *)dst + 1;
                src = (char *)src + 1;
        }

        return(ret);
}

void * __cdecl memset (
	void *dst,
	int val,
	size_t count
	)
{
	void *start = dst;

#if defined (_M_X64)

	{


		__declspec(dllimport)


			void RtlFillMemory( void *, size_t count, char );

		RtlFillMemory( dst, count, (char)val );

	}

#else  /* defined (_M_X64) */
	while (count--) {
		*(char *)dst = (char)val;
		dst = (char *)dst + 1;
	}
#endif  /* defined (_M_X64) */

	return(start);
}