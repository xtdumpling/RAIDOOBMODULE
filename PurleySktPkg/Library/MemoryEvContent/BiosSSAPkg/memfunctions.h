#ifndef __SSA_BIOS_MEMORY_FUNCTIONS_H__
#define __SSA_BIOS_MEMORY_FUNCTIONS_H__

#ifdef SSA_FLAG
VOID* bssa_memset(VOID* s, INT32 c, UINT32 n);
VOID* bssa_memcpy(VOID* destination, const VOID* source, UINT32 size);
#endif //SSA_FLAG

#endif
