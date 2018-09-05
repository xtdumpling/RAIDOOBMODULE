//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//**********************************************************************
/** @file AmtLockKBD.h
    AMT Lock KeyBoard Includes.

**/
#ifndef _AMT_INT16_H
#define _AMT_INT16_H

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "Protocol\AmtWrapper\AmtWrapper.h"

#include EFI_PROTOCOL_CONSUMER (AmtWrapper)
#include EFI_PROTOCOL_CONSUMER (LegacyBios)

#define AMT_INT16_CSM_GUID \
    {0x6046e678, 0x24ef, 0x4005, 0xba, 0x39, 0xbd, 0xa1, 0x1f, 0x6d, 0x55, 0x5d}

EFI_GUID  	gEfiFirmwareVolumeProtocolGuid 	= EFI_FIRMWARE_VOLUME_PROTOCOL_GUID;
EFI_GUID  	gAmtInt16CsmGuid 				= AMT_INT16_CSM_GUID;
EFI_GUID 	gLegacyBiosProtocolGuid 		= EFI_LEGACY_BIOS_PROTOCOL_GUID;
EFI_GUID 	gEfiAmtWrapperProtocolGuid 		= EFI_AMT_WRAPPER_PROTOCOL_GUID;   
EFI_GUID  	gEfiEventReadyToBootGuid 		= EFI_EVENT_GROUP_READY_TO_BOOT;

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
