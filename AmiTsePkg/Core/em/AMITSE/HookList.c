//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/HookList.c $
//
// $Author: Madhans $
//
// $Revision: 8 $
//
// $Date: 3/28/11 9:06p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file HookList.c
    AMITSE Hook definitions

**/

#ifdef TSE_FOR_APTIO_4_50
#include <Efi.h>
#include "Token.h"
#else
#include "Tiano.h"
#include "tokens.h"
#endif

#include "AMITSEElinks.h"

UINTN gHookBase = TSE_HOOKBASE_VALUE;

typedef VOID TSE_HOOK (VOID);
extern TSE_HOOK HOOK_LIST_FROM_ELINKS;

#define HOOK0	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE)
#define HOOK1	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+1)
#define HOOK2	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+2)
#define HOOK3	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+3)
#define HOOK4	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+4)
#define HOOK5	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+5)
#define HOOK6	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+6)
#define HOOK7	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+7)
#define HOOK8	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+8)
#define HOOK9	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+9)
#define HOOK10	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+10)
#define HOOK11	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+11)
#define HOOK12	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+12)
#define HOOK13	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+13)
#define HOOK14	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+14)
#define HOOK15	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+15)
#define HOOK16	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+16)
#define HOOK17	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+17)
#define HOOK18	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+18)
#define HOOK19	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+19)
#define HOOK20	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+20)
#define HOOK21	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+21)
#define HOOK22	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+22)
#define HOOK23	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+23)
#define HOOK24	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+24)
#define HOOK25	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+25)
#define HOOK26	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+26)
#define HOOK27	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+27)
#define HOOK28	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+28)
#define HOOK29	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+29)
#define HOOK30	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+30)
#define HOOK31	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+31)
#define HOOK32	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+32)
#define HOOK33	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+33)
#define HOOK34	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+34)
#define HOOK35	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+35)
#define HOOK36	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+36)
#define HOOK37	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+37)
#define HOOK38	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+38)
#define HOOK39	(TSE_HOOK *)((UINTN)TSE_HOOKBASE_VALUE+39)


TSE_HOOK * AMITSE_HookList [] = {HOOK_LIST_FROM_ELINKS, NULL};

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
