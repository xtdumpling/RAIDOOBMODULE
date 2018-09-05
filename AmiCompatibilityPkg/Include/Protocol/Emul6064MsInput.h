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

/** @file Emul6064MsInput.h
    Protocol used for 6064 USB mouse emulation

**/

#ifndef _EMUL6064MSINPUT_PROTOCOL_H_
#define _EMUL6064MSINPUT_PROTOCOL_H_

EFI_FORWARD_DECLARATION (EFI_EMUL6064MSINPUT_PROTOCOL);

#define EFI_EMUL6064MSINPUT_PROTOCOL_GUID \
  { 0x7578b307, 0xb25b, 0x44f9, 0x89, 0x2e, 0x20, 0x9b, 0x0e, 0x39, 0x93, 0xc6 }

#define PS2MSFLAGS_LBUTTON			0x01
#define PS2MSFLAGS_RBUTTON			0x02
#define PS2MSFLAGS_XSIGN			0x10
#define PS2MSFLAGS_YSIGN			0x20
#define PS2MSFLAGS_XO				0x40
#define PS2MSFLAGS_YO				0x80

typedef struct {
	UINT8 flags;
	UINT8 x;
	UINT8 y;
} PS2MouseData;


typedef
EFI_STATUS
(EFIAPI *EFI_EMUL6064MSINPUT_PROTOCOL_SEND) (
  IN EFI_EMUL6064MSINPUT_PROTOCOL           * This,
  IN PS2MouseData*					 data
  );

typedef struct _EFI_EMUL6064MSINPUT_PROTOCOL {
	EFI_EMUL6064MSINPUT_PROTOCOL_SEND Send;
};

extern EFI_GUID gEmul6064MsInputProtocolGuid;

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
