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

/** @file Emul6064KbdInput.h
    Protocol used for USB 6064 keyboard emulation

**/


#ifndef _EMUL6064KBDINPUT_PROTOCOL_H_
#define _EMUL6064KBDINPUT_PROTOCOL_H_

EFI_FORWARD_DECLARATION (EFI_EMUL6064KBDINPUT_PROTOCOL);

#define EFI_EMUL6064KBDINPUT_PROTOCOL_GUID \
  { 0x62ceef5a, 0x1d7c, 0x4943, 0x9b, 0x3a, 0x95, 0xe2, 0x49, 0x4c, 0x89, 0x90 }

typedef enum {
	KBC_KBDTRANS_AT = 0,	 //NOT TRANSLATED (XLAT cleared in CCB)
	KBC_KBDTRANS_PCXT = 1,	 //TRANSLATED (XLAT set in CCB)
} KBC_KBDTRANSLATION;

typedef
EFI_STATUS
(EFIAPI *EFI_EMUL6064KBDINPUT_PROTOCOL_SEND) (
  IN EFI_EMUL6064KBDINPUT_PROTOCOL           * This,
  IN UINT8*	data,
  IN UINT32 count
  );

typedef
EFI_STATUS
(EFIAPI *EFI_EMUL6064KBDINPUT_PROTOCOL_GETTRANS) (
  IN EFI_EMUL6064KBDINPUT_PROTOCOL           * This,
  OUT KBC_KBDTRANSLATION*	outTrans
  );

typedef
EFI_STATUS
(EFIAPI *EFI_EMUL6064KBDINPUT_PROTOCOL_UPDATELEDSTATE) (
  IN EFI_EMUL6064KBDINPUT_PROTOCOL           * This,
  IN UINT8	data
  );


typedef struct _EFI_EMUL6064KBDINPUT_PROTOCOL {
	EFI_EMUL6064KBDINPUT_PROTOCOL_SEND Send ;
	EFI_EMUL6064KBDINPUT_PROTOCOL_GETTRANS GetTranslation;
	EFI_EMUL6064KBDINPUT_PROTOCOL_UPDATELEDSTATE UpdateLEDState;
};

extern EFI_GUID gEmul6064KbdInputProtocolGuid;

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
