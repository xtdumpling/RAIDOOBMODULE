//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**           5555 Oakbrook Pkwy, Norcross, Georgia 30093       **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
#ifdef TSE_FOR_APTIO_4_50

#include "Token.h"  
#include <Efi.h>
#include <Protocol/SimpleTextIn.h>

#else //#ifdef TSE_FOR_APTIO_4_50

#include "minisetup.h"

#endif //#ifdef TSE_FOR_APTIO_4_50

#include "commonoem.h"		
#include "HotclickBin.h"
#include "HotClickElinks.h"

// Build time file generated from AMITSE_OEM_HEADER_LIST elink.
#include "AMITSEOem.h"		

#ifdef TSE_FOR_APTIO_4_50
#include "AMITSEStrTokens.h"
#else
#include STRING_DEFINES_FILE
#endif

HOTCLICK_TEMPLATE gHotClickInfo[] = { HOTCLICK_LIST 0 };                //Added 0 for having null entry if HOTCLICK_FULLSCREEN_SUPPORT = 0
UINT32 gHotClickCount = sizeof(gHotClickInfo) / sizeof(HOTCLICK_TEMPLATE);


//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**           5555 Oakbrook Pkwy, Norcross, Georgia 30093       **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
