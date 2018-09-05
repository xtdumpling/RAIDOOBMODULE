#ifdef SSA_FLAG
#include "ssabios.h"

VOID* bssa_memcpy(VOID* destination, const VOID* source, UINT32 size)
{
	UINT32 i;
	CHAR8* src;
	CHAR8* dest;

	src = (CHAR8*)source;
	dest = (CHAR8*)destination;
	for(i = 0; i < size; ++i)
	{
		dest[i] = src[i];
	}
	return destination;
}
#endif //SSA_FLAG
