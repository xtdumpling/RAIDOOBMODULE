//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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

//**********************************************************************
// $Header: /AptioV/BIN/AMIDebugRxPkg/Include/Protocol/DebugPort.h 1     11/02/12 10:15a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:15a $
//**********************************************************************
//**********************************************************************


#ifndef _DEBUG_PORT_H_
#define _DEBUG_PORT_H_

#include "Efi.h"
#include <Protocol/DevicePath.h>

//
// DebugPortIo protocol {EBA4E8D2-3858-41EC-A281-2647BA9660D0}
//

// Commented to avoid redefinition error with Core4.6.5.1_2
#ifndef EFI_DEBUGPORT_PROTOCOL_GUID
#define EFI_DEBUGPORT_PROTOCOL_GUID \
  { 0xEBA4E8D2, 0x3858, 0x41EC, 0xA2, 0x81, 0x26, 0x47, 0xBA, 0x96, 0x60, 0xD0 }
#endif

extern EFI_GUID  gDbgEfiDebugPortProtocolGuid;

//EFI_FORWARD_DECLARATION (EFI_DEBUGPORT_PROTOCOL);
typedef struct _EFI_DEBUGPORT_PROTOCOL EFI_DEBUGPORT_PROTOCOL;

//
// DebugPort member functions
//
typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_RESET) (
  IN EFI_DEBUGPORT_PROTOCOL               *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_WRITE) (
  IN EFI_DEBUGPORT_PROTOCOL               *This,
  IN UINT32                               Timeout,
  IN OUT UINTN                            *BufferSize,
  IN VOID                                 *Buffer
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_READ) (
  IN EFI_DEBUGPORT_PROTOCOL               *This,
  IN UINT32                               Timeout,
  IN OUT UINTN                            *BufferSize,
  OUT VOID                                *Buffer
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_POLL) (
  IN EFI_DEBUGPORT_PROTOCOL               *This
  );

//
// DebugPort protocol definition
//
typedef struct _EFI_DEBUGPORT_PROTOCOL {
  EFI_DEBUGPORT_RESET                     Reset;
  EFI_DEBUGPORT_WRITE                     Write;
  EFI_DEBUGPORT_READ                      Read;
  EFI_DEBUGPORT_POLL                      Poll;
} EFI_DEBUGPORT_PROTOCOL;

//
// DEBUGPORT variable definitions...
//
#define EFI_DEBUGPORT_VARIABLE_NAME L"DEBUGPORT"
#define EFI_DEBUGPORT_VARIABLE_GUID EFI_DEBUGPORT_PROTOCOL_GUID
// To avoid Compilation error with Core4.6.5.1_2
//extern EFI_GUID  gEfiDebugPortProtocolGuid;
#define gEfiDebugPortVariableGuid gDbgEfiDebugPortProtocolGuid

//
// DebugPort device path definitions...
//

#define DEVICE_PATH_MESSAGING_DEBUGPORT EFI_DEBUGPORT_PROTOCOL_GUID
// To avoid Compilation error with Core4.6.5.1_2
//extern EFI_GUID  gEfiDebugPortProtocolGuid;
#define gEfiDebugPortDevicePathGuid gDbgEfiDebugPortProtocolGuid

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL                Header;
  EFI_GUID                                Guid;
} DEBUGPORT_DEVICE_PATH;

#endif /* _DEBUG_PORT_H_ */

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
