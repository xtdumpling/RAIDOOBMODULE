//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:
//
// $Revision:
//
// $Date:
//**********************************************************************
// Revision History
// ----------------
// $Log:
// 
//
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  UpdateSmbiosTable.h
//
// Description:	Contents present here is used by UpdateSmbiosTable.c
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __SMBIOSUPDATE__H__
#define __SMBIOSUPDATE__H__
#ifdef __cplusplus
extern "C" {
#endif

// Header Files
#include <Token.h>
#include <AmiDxeLib.h>
#include <Pci.h>
#include <Uefi/UefiSpec.h>
#include <IndustryStandard/Smbios.h>
#include <Protocol/Smbios.h>
#include <Protocol/IioSystem.h>
#include <Library/PciExpressLib.h>
#include <Library/MemoryAllocationLib.h>
#include <AmiPcieSegBusLib/AmiPcieSegBusLib.h>

//Macro's
#if PLATFORM_TYPE == 0  //Neoncity

#define SLOT1_SOCKET_NO   0
#define SLOT1_STACK_NO    1
#define SLOT1_DEV_FUN_NO  0x10

#define SLOT2_SOCKET_NO   0
#define SLOT2_STACK_NO    3
#define SLOT2_DEV_FUN_NO  0x00

#define SLOT3_SOCKET_NO   0
#define SLOT3_STACK_NO    0
#define SLOT3_DEV_FUN_NO  0xE0

#define SLOT4_SOCKET_NO   1
#define SLOT4_STACK_NO    1
#define SLOT4_DEV_FUN_NO  0x00

#define SLOT5_SOCKET_NO   1
#define SLOT5_STACK_NO    3
#define SLOT5_DEV_FUN_NO  0x00

#define SLOT6_SOCKET_NO   1
#define SLOT6_STACK_NO    0
#define SLOT6_DEV_FUN_NO  0x00

#define SLOT7_SOCKET_NO   1
#define SLOT7_STACK_NO    3
#define SLOT7_DEV_FUN_NO  0x10

#define SLOT8_SOCKET_NO   1
#define SLOT8_STACK_NO    2
#define SLOT8_DEV_FUN_NO  0x00

#else                   //Lightningridge

#define SLOT1_SOCKET_NO   3
#define SLOT1_STACK_NO    3
#define SLOT1_DEV_FUN_NO  0x00

#define SLOT2_SOCKET_NO   0
#define SLOT2_STACK_NO    3
#define SLOT2_DEV_FUN_NO  0x00

#define SLOT3_SOCKET_NO   3
#define SLOT3_STACK_NO    2
#define SLOT3_DEV_FUN_NO  0x00

#define SLOT4_SOCKET_NO   1
#define SLOT4_STACK_NO    1
#define SLOT4_DEV_FUN_NO  0x00

#define SLOT5_SOCKET_NO   2
#define SLOT5_STACK_NO    3
#define SLOT5_DEV_FUN_NO  0x00

#define SLOT6_SOCKET_NO   1
#define SLOT6_STACK_NO    2
#define SLOT6_DEV_FUN_NO  0x00

#define SLOT7_SOCKET_NO   2
#define SLOT7_STACK_NO    1
#define SLOT7_DEV_FUN_NO  0x00

#endif

//Structure declarations
/**
    System Slot Socket, Stack, Type and Data bus width Information
**/
typedef struct {
    UINT8                     Socket;
    UINT8                     Stack;
    UINT8                     SlotType;
    UINT8                     SlotDataBusWidth;
    UINT8                     DevFun;
} SLOT_INFO; 

//Function Declarations
VOID
EFIAPI
UpdateSmbiosTables (
  IN  EFI_EVENT Event,
  IN  VOID      *Context
);

EFI_STATUS
EFIAPI
UpdateSmbiosType9Table (
);

EFI_STATUS
AddStructureForAdditionalSlots (
    UINT8 AddSlotCount
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
