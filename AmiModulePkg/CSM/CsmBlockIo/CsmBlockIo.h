//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CsmBlockIo.h
    CSM BlockIO driver header file.

**/

#ifndef _CsmBlockIo_
#define _CsmBlockIo_

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <Token.h>
#include <Dxe.h>
#include <Pci.h>
#include <AmiDxeLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>
#include <Protocol/BlockIo.h>

#include "CsmEdd.h"


// Global Variables
#if EFI_DEBUG
extern EFI_COMPONENT_NAME_PROTOCOL gBiosBlockIoComponentName;
#endif


// Define the I2O class code

#define PCI_BASE_CLASS_INTELLIGENT      0x0e
#define PCI_SUB_CLASS_INTELLIGENT       0x00

// SD class/subclass defined in PCI.H in 4.6.3.7
#ifndef PCI_CL_SYSTEM_PERIPHERALS
#define PCI_CL_SYSTEM_PERIPHERALS		0x08
#endif
#ifndef PCI_CL_SYSTEM_PERIPHERALS_SCL_SD
#define PCI_CL_SYSTEM_PERIPHERALS_SCL_SD	0x05
#endif


// Number of pages needed for our buffer under 1MB

#define BLOCK_IO_BUFFER_PAGE_SIZE (((sizeof (EDD_DEVICE_ADDRESS_PACKET) + sizeof (CSM_LEGACY_DRIVE) + MAX_EDD11_XFER) / EFI_PAGE_SIZE) + 1)


// PROTOTYPES

// Driver Binding Protocol functions

EFI_STATUS
CsmBlockIoSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

EFI_STATUS
CsmBlockIoStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

EFI_STATUS
CsmBlockIoStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  );
  
// Block I/O functions

EFI_STATUS 
CsmBlockIoFlushBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL           *This
    );

EFI_STATUS 
CsmBlockIoReset (
    IN  EFI_BLOCK_IO_PROTOCOL           *This,
    IN  BOOLEAN                ExtendedVerification
    );

// These prototypes aren't actually used -- they are the generic for the specific
// functions below
EFI_STATUS
CsmBlockIoReadBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL          *This,
    IN  UINT32                MediaId,
    IN  EFI_LBA               LBA,
    IN  UINTN                 BufferSize,
    OUT VOID                  *Buffer
    );

EFI_STATUS
CsmBlockIoWriteBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL           *This,
    IN  UINT32                 MediaId,
    IN  EFI_LBA                LBA,
    IN  UINTN                  BufferSize,
    OUT VOID                   *Buffer
    );

// Specific read/write function prototypes
EFI_STATUS
Edd30CsmReadBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL          *This, 
    IN  UINT32                MediaId, 
    IN  EFI_LBA               LBA, 
    IN  UINTN                 BufferSize, 
    OUT VOID                  *Buffer
    );

EFI_STATUS
Edd30CsmWriteBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL          *This,
    IN  UINT32                MediaId,
    IN  EFI_LBA               LBA,
    IN  UINTN                 BufferSize,
    OUT VOID                  *Buffer
    );

EFI_STATUS
Edd11CsmReadBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL          *This,
    IN  UINT32                MediaId,
    IN  EFI_LBA               LBA,
    IN  UINTN                 BufferSize,
    OUT VOID                  *Buffer
    );

EFI_STATUS
Edd11CsmWriteBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL          *This,
    IN  UINT32                MediaId,
    IN  EFI_LBA               LBA,
    IN  UINTN                 BufferSize,
    OUT VOID                  *Buffer
    );

EFI_STATUS
LegacyCsmReadBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL          *This,
    IN  UINT32                MediaId,
    IN  EFI_LBA               LBA,
    IN  UINTN                 BufferSize,
    OUT VOID                  *Buffer
    );

EFI_STATUS
LegacyCsmWriteBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL          *This,
    IN  UINT32                MediaId,
    IN  EFI_LBA               LBA,
    IN  UINTN                 BufferSize,
    OUT VOID                  *Buffer
    );

// Local support function prototypes
BOOLEAN
InitBlockIo (
    IN  CSM_BLOCK_IO_DEV        *Dev
    );

// Local function prototypes
VOID
BuildDevicePath (
    IN  EFI_DEVICE_PATH_PROTOCOL  *BaseDevicePath,
    IN  CSM_LEGACY_DRIVE          *Drive,
    IN  EFI_DEVICE_PATH_PROTOCOL  **DevPath
    );

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
AppendDevicePathNode (
    IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevPath,
    IN CONST EFI_DEVICE_PATH_PROTOCOL  *Node
    );

UINTN AlignAddress (
    UINTN                       Address
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
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
