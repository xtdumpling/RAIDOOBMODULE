#ifdef SSA_FLAG
#include "ssabios.h"

//#pragma function(memset)
//void *memset(void *s, int c, UINT32 n)
//{
//	unsigned char* p=s;
//	while(n--)
//		*p++ = (unsigned char)c;
//	return s;
//}

VOID* bssa_memset(VOID* s, INT32 c, UINT32 n)
{
	//return memset(s, c, n);
  unsigned char* p=s;
  while(n--)
    *p++ = (unsigned char)c;
  return s;
}
#endif //SSA_FLAG
