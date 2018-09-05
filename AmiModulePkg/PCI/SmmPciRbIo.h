//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


/** @file
  The implementation AMI SMM PCI Root Bridge Io Driver.

  SMM PCI Root Bridge I/O protocol is used by DXE_SMM Drivers to perform PCI Memory, PCI I/O, 
  and PCI Configuration cycles. 
**/

#ifndef _AMI_SMM_PCI_ROOT_BRIDGE_H_
#define _AMI_SMM_PCI_ROOT_BRIDGE_H_

// Include common header file for this module.
#include <SmmPi.h>

// The protocols, PPI and GUID defintions for this module
#include <Protocol/SmmPciRootBridgeIo.h>
#include <Protocol/SmmCpuIo2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/AmiPciSmmHandoff.h>

// The Library classes and other headers this module consumes
#include <Library/PciAccessCspLib.h>
#include <Library/AmiSdlLib.h>
#include <PciHostBridge.h>


#define AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE  0x42524D53494D4124   //$AMISMRB

///
/// AMI SMM PCI Root Bridge private structure. It holds all RB information and 
/// implementation of EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL for PCI root bridge.
/// 
typedef struct _AMI_SMM_PCI_RB_DATA{
    EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL SmmRbIo;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
    EFI_HANDLE                          Handle;
    UINT64                              Supports;
    UINT64                              Attributes;  
    UINTN                               ResCount;
//    ASLR_QWORD_ASD                      *RbRes;
    BOOLEAN                             BusesSet;
    BOOLEAN                             ResSubmited;
    BOOLEAN                             ResAsquired;
    BOOLEAN                             NotPresent;
    AMI_SDL_PCI_DEV_INFO                *RbSdlData;
    UINTN                               RbSdlIndex;
    UINTN                               Reserved1;
    UINTN                               HostIdx;
    UINTN                               RootIdx;
    UINT64                              Signature;
} AMI_SMM_PCI_RB_DATA;

static EFI_STATUS
EFIAPI
SmmRbIoPciRW (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     BOOLEAN                                Write,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

//
// Protocol Member Function Prototypes
//

EFI_STATUS
EFIAPI
SmmRbIoPollMem ( 
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
EFI_STATUS
EFIAPI
SmmRbIoPollIo ( 
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
EFI_STATUS
EFIAPI
SmmRbIoMemRead (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
SmmRbIoMemWrite (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
SmmRbIoIoRead (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
EFIAPI
SmmRbIoIoWrite (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
EFIAPI
SmmRbIoCopyMem (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 DestAddress,
  IN     UINT64                                 SrcAddress,
  IN     UINTN                                  Count
  );

EFI_STATUS
EFIAPI
SmmRbIoPciRead (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
SmmRbIoPciWrite (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
SmmRbIoMap (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  );

EFI_STATUS
EFIAPI
SmmRbIoUnmap (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  VOID                                 *Mapping
  );

EFI_STATUS
EFIAPI
SmmRbIoAllocateBuffer (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                    Type,
  IN  EFI_MEMORY_TYPE                      MemoryType,
  IN  UINTN                                Pages,
  OUT VOID                                 **HostAddress,
  IN  UINT64                               Attributes
  );

EFI_STATUS
EFIAPI
SmmRbIoFreeBuffer (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                                Pages,
  OUT VOID                                 *HostAddress
  );

EFI_STATUS
EFIAPI
SmmRbIoFlush (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
SmmRbIoGetAttributes (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                               *Supported,
  OUT UINT64                               *Attributes
  );

EFI_STATUS
EFIAPI
SmmRbIoSetAttributes (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                               Attributes,
  IN OUT UINT64                               *ResourceBase,
  IN OUT UINT64                               *ResourceLength 
  ); 

EFI_STATUS
EFIAPI
SmmRbIoConfiguration (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT    VOID                                 **Resources
  );

#endif // _AMI_SMM_PCI_ROOT_BRIDGE_H_

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
