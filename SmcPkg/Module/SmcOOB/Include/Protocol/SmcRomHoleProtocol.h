//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug fixed:
//    Reason   : Add an interface to call platform specific SmmRomHoleLib
//    Auditor  : Durant Lin
//    Date     : Feb/07/2018
//
//  Rev. 1.00
//    Bug fixed:
//    Reason   :
//    Auditor  :
//    Date     :
//
//****************************************************************************

#ifndef _SMC_ROM_HOLE_PROTOCOL_H_
#define _SMC_ROM_HOLE_PROTOCOL_H_
        
#define SMC_ROM_HOLE_PROTOCOL_GUID \
  { \
    0x2938C3C9, 0x4F9B, 0x43B2, {0xA4, 0x29, 0x7E, 0x63, 0x15, 0xAD, 0x6A, 0x9D} \
  }

//
// Forward Declaration
//
typedef struct _SMC_ROM_HOLE_PROTOCOL SMC_ROM_HOLE_PROTOCOL;
typedef struct _SMC_ROM_HOLE_PROTOCOL SMC_SMM_ROM_HOLE_PROTOCOL;
//
// Revision 1: First Version
//
#define SMC_ROM_HOLE_PROTOCOL_REVISION_1   1

typedef
EFI_STATUS
(EFIAPI *SMC_ROMHOLE_WRITEREGION) (
  IN  UINT8                             Operation,
  IN  UINT8                             *pBuffer
  );

typedef
EFI_STATUS
(EFIAPI *SMC_ROMHOLE_READREGION) (
  IN  UINT8                             Operation,
  OUT UINT8                             *pBuffer
  );

typedef
UINT32
(EFIAPI *SMC_ROMHOLE_SIZE) (
  IN  UINT8                             Operation
  );

typedef
UINT32
(EFIAPI *SMC_ROMHOLE_OFFSET) (
  IN  UINT8                             Operation
  );

//
// SMC ROM HOLE PROTOCOL
//
typedef struct _SMC_ROM_HOLE_PROTOCOL {
  UINT32                           ProtocolRevision;
  UINT32                           PlatformId;
  SMC_ROMHOLE_WRITEREGION          SmcRomHoleWriteRegion;
  SMC_ROMHOLE_READREGION           SmcRomHoleReadRegion;
  SMC_ROMHOLE_SIZE                 SmcRomHoleSize;
  SMC_ROMHOLE_OFFSET               SmcRomHoleOffset;
};

extern EFI_GUID gSmcRomHoleProtocolGuid;
extern EFI_GUID gSmcSmmRomHoleProtocolGuid;

#endif

