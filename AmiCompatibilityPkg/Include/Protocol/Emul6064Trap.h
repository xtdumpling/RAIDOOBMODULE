//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file Emul6064Trap.h
    Protocol used for 6064 port trapping

**/


#ifndef _EMUL6064TRAP_PROTOCOL_H_
#define _EMUL6064TRAP_PROTOCOL_H_

#define EFI_EMUL6064TRAP_PROTOCOL_GUID \
    { 0x6ea0f71c, 0x614a, 0x437e, 0x8f, 0x49, 0x24, 0x3a, 0xd4, 0xe8, 0x32, 0x68 }

#define EFI_NONSMMEMUL6064TRAP_PROTOCOL_GUID \
    { 0x68b81e51, 0x2583, 0x4582, 0x95, 0xdb, 0xc5, 0x72, 0x32, 0x36, 0xc4, 0xf1 }

#define IRQ_SUPPORTED       0x0001		

typedef struct _EFI_EMUL6064TRAP_PROTOCOL EFI_EMUL6064TRAP_PROTOCOL;

typedef
BOOLEAN
(EFIAPI *EFI_EMUL6064TRAP_PROTOCOL_ENABLE) (
  IN EFI_EMUL6064TRAP_PROTOCOL           * This
  );

typedef
BOOLEAN
(EFIAPI *EFI_EMUL6064TRAP_PROTOCOL_DISABLE) (
  IN EFI_EMUL6064TRAP_PROTOCOL           * This
  );

typedef
UINT32
(EFIAPI *EFI_EMUL6064TRAP_PROTOCOL_FEATURE) (
  IN EFI_EMUL6064TRAP_PROTOCOL           * This
  );

typedef
BOOLEAN
(EFIAPI *EFI_EMUL6064TRAP_PROTOCOL_GETTRAPSTATUS) (
  IN EFI_EMUL6064TRAP_PROTOCOL           * This
  );

typedef struct _EFI_EMUL6064TRAP_PROTOCOL {
    EFI_EMUL6064TRAP_PROTOCOL_ENABLE TrapEnable;
    EFI_EMUL6064TRAP_PROTOCOL_DISABLE TrapDisable;
    EFI_EMUL6064TRAP_PROTOCOL_FEATURE FeatureSupported;
    EFI_EMUL6064TRAP_PROTOCOL_GETTRAPSTATUS GetTrapStatus;  
};

extern EFI_GUID gEmul6064TrapProtocolGuid;
extern EFI_GUID gNonSmmEmul6064TrapProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
