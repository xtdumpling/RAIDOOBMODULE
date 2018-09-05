//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  AMI Sio Protocol Definitions.
*/

#ifndef __SIO_PROTOCOL_H__
#define __SIO_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

// 51E9B4F9-555D-476c-8BB5-BD18D9A68878
#define EFI_AMI_SIO_PROTOCOL_GUID \
    {0x51e9b4f9,0x555d,0x476c,0x8b, 0xb5, 0xbd, 0x18, 0xd9, 0xa6, 0x88, 0x78}

GUID_VARIABLE_DECLARATION(gEfiAmiSioProtocolGuid,EFI_AMI_SIO_PROTOCOL_GUID);

#ifndef GUID_VARIABLE_DEFINITION
#include <AmiDxeLib.h>

typedef struct _AMI_SIO_PROTOCOL AMI_SIO_PROTOCOL;

typedef struct _EFI_EISAID {
  UINT32                HID;
  UINT32                UID;
} EFI_EISAID;

//This list migt be extended if some devices 
//not mention here present in SIO 
typedef enum {
	dsNone=0,
	dsFDC,
	dsPS2K,	//PS2 Keyboard Controller if KBC on separate from MS Logical Device
	dsPS2M,	//PS2 Mouse Controller if MS on separate from KBC Logical Device
	dsPS2CK,//PS2 Keyboard Controller where KBC and MS on same Logical Device
	dsPS2CM,//PS2 Mouse Controller where KBC and MS on same Logical Device
	dsUART,
	dsLPT,
	dsGAME,
	dsSB16,
	dsMPU401,
	dsFmSynth,
	dsCIR,
	dsGPIO,
	dsHwMon,
	dsPME,
	dsACPI,
} SIO_DEV_TYPE;

typedef EFI_STATUS (EFIAPI *SIO_REGISTER) (
    IN AMI_SIO_PROTOCOL		*This,
	IN BOOLEAN				Write,
	IN BOOLEAN				ExitCfgMode,
    IN UINT8            	Register,
    IN OUT UINT8        	*Value
);

typedef EFI_STATUS (EFIAPI *SIO_RESOURCES)(
    IN AMI_SIO_PROTOCOL 	*This,
    IN BOOLEAN 				Set,
	IN OUT T_ITEM_LIST		**Resources
);

//**********************************************************************
//<AMI_SHDR_START>
//
// Name:        AMI_SIO_PROTOCOL
//
// Description: Functions to access the SIO.
//
// Fields:     Name        Type        Description
//        ------------------------------------------------------------
//
//<AMI_SHDR_END>
//**********************************************************************
struct _AMI_SIO_PROTOCOL {
    SIO_REGISTER       		Access;
	SIO_RESOURCES			CurrentRes;
	SIO_RESOURCES			PossibleRes;
}; // AMI_SIO_PROTOCOL;

/****** DO NOT WRITE BELOW THIS LINE *******/
#endif // #ifndef GUID_VARIABLE_DEFINITION
#ifdef __cplusplus
}
#endif

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
