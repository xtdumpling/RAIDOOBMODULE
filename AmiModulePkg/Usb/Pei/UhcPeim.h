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

/** @file UhcPeim.h
    This file belongs to "Framework".
    This file is modified by AMI to include copyright message,
    appropriate header and integration code.

**/

//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//

/*++

   Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.


   Module Name:

   UhcPeim.h

   Abstract:

   Header file for Usb Host Controller PEIM

   --*/
#ifndef _PEI_UHC_H
#define _PEI_UHC_H

//
// Driver Produced PPI Prototypes
//
#include <Ppi/UsbPeiPpi.h>

//
// Driver Consumed PPI Prototypes
//
#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>

#define bit( a )  (1 << (a))

#define USB_SLOW_SPEED_DEVICE  0x01
#define USB_FULL_SPEED_DEVICE  0x02

//
// One memory block uses 16 page
//
#define NORMAL_MEMORY_BLOCK_UNIT_IN_PAGES  16

#define   USBCMD                           0            // Command Register Offset 00-01h
#define   USBCMD_RS                        bit( 0 )     // Run/Stop
#define   USBCMD_HCRESET                   bit( 1 )     // Host reset
#define   USBCMD_GRESET                    bit( 2 )     // Global reset
#define   USBCMD_EGSM                      bit( 3 )     // Global Suspend Mode
#define   USBCMD_FGR                       bit( 4 )     // Force Global Resume
#define   USBCMD_SWDBG                     bit( 5 )     // SW Debug mode
#define   USBCMD_CF                        bit( 6 )     // Config Flag (sw only)
#define   USBCMD_MAXP                      bit( 7 )     // Max Packet (0 = 32, 1 = 64)

// Status register
#define   USBSTS         2              // Status Register Offset 02-03h
#define   USBSTS_USBINT  bit( 0 )       // Interrupt due to IOC
#define   USBSTS_ERROR   bit( 1 )       // Interrupt due to error
#define   USBSTS_RD      bit( 2 )       // Resume Detect
#define   USBSTS_HSE     bit( 3 )       // Host System Error
                                  // - basically PCI problems
#define   USBSTS_HCPE    bit( 4 ) // Host Controller Process Error
                                  // - the scripts were buggy
#define   USBSTS_HCH     bit( 5 ) // HC Halted

// Interrupt enable register
#define   USBINTR          4            // Interrupt Enable Register 04-05h
#define   USBINTR_TIMEOUT  bit( 0 )     // Timeout/CRC error enable
#define   USBINTR_RESUME   bit( 1 )     // Resume interrupt enable
#define   USBINTR_IOC      bit( 2 )     // Interrupt On Complete enable
#define   USBINTR_SP       bit( 3 )     // Short packet interrupt enable

// Frame Number Register Offset 06-08h
#define   USBFRNUM  6

// Frame List Base Address Register Offset 08-0Bh
#define   USBFLBASEADD  8

// Start of Frame Modify Register Offset 0Ch
#define   USBSOF  0x0c

// USB port status and control registers
#define   USBPORTSC1          0x10      //Port 1 offset 10-11h
#define   USBPORTSC2          0x12      //Port 2 offset 12-13h

#define   USBPORTSC_CCS       bit( 0 )  // Current Connect Status ("device present")
#define   USBPORTSC_CSC       bit( 1 )  // Connect Status Change
#define   USBPORTSC_PED       bit( 2 )  // Port Enable / Disable
#define   USBPORTSC_PEDC      bit( 3 )  // Port Enable / Disable Change
#define   USBPORTSC_LSL       bit( 4 )  // Line Status Low bit
#define   USBPORTSC_LSH       bit( 5 )  // Line Status High bit
#define   USBPORTSC_RD        bit( 6 )  // Resume Detect
#define   USBPORTSC_LSDA      bit( 8 )  // Low Speed Device Attached
#define   USBPORTSC_PR        bit( 9 )  // Port Reset
#define   USBPORTSC_SUSP      bit( 12 ) // Suspend

#define SETUP_PACKET_ID       0x2d
#define INPUT_PACKET_ID       0x69
#define OUTPUT_PACKET_ID      0xe1
#define ERROR_PACKET_ID       0x55

#define MEM_QH_TD_TYPE        0
#define MEM_DATA_BUFFER_TYPE  1

#define STALL_1_MILLI_SECOND  1000
//#define STALL_1_MILLI_SECOND  10

//#pragma pack(1)

#pragma pack(push, 16)
typedef struct
{
    UINT32 FrameListPtrTerminate : 1;
    UINT32 FrameListPtrQSelect   : 1;
    UINT32 FrameListRsvd         : 2;
    UINT32 FrameListPtr          : 28;
} FRAMELIST_ENTRY;

typedef struct
{
    UINT32 QHHorizontalTerminate : 1;
    UINT32 QHHorizontalQSelect   : 1;
    UINT32 QHHorizontalRsvd      : 2;
    UINT32 QHHorizontalPtr       : 28;
    UINT32 QHVerticalTerminate   : 1;
    UINT32 QHVerticalQSelect     : 1;
    UINT32 QHVerticalRsvd        : 2;
    UINT32 QHVerticalPtr         : 28;
} QUEUE_HEAD;

typedef struct
{
    QUEUE_HEAD QH;
    UINT32     reserved1;
    UINT32     reserved2;
    VOID *ptrNext;
    VOID *ptrDown;
    VOID *reserved3;
    UINT32     reserved4;
} QH_STRUCT;

typedef struct
{
    UINT32 TDLinkPtrTerminate   : 1;
    UINT32 TDLinkPtrQSelect     : 1;
    UINT32 TDLinkPtrDepthSelect : 1;
    UINT32 TDLinkPtrRsvd        : 1;
    UINT32 TDLinkPtr            : 28;
    UINT32 TDStatusActualLength : 11;
    UINT32 TDStatusRsvd         : 5;
    UINT32 TDStatus             : 8;
    UINT32 TDStatusIOC          : 1;
    UINT32 TDStatusIOS          : 1;
    UINT32 TDStatusLS           : 1;
    UINT32 TDStatusErr          : 2;
    UINT32 TDStatusSPD          : 1;
    UINT32 TDStatusRsvd2        : 2;
    UINT32 TDTokenPID           : 8;
    UINT32 TDTokenDevAddr       : 7;
    UINT32 TDTokenEndPt         : 4;
    UINT32 TDTokenDataToggle    : 1;
    UINT32 TDTokenRsvd          : 1;
    UINT32 TDTokenMaxLen        : 11;
    UINT32 TDBufferPtr;
} TD;

typedef struct
{
    TD     TDData;
    UINT8  *pTDBuffer;
    VOID   *ptrNextTD;
    VOID   *ptrNextQH;
    UINT16 TDBufferLength;
    UINT16 reserved;
} TD_STRUCT;

//#pragma pack()
#pragma pack(pop)

typedef struct _MEMORY_MANAGE_HEADER
{
    UINT8 *BitArrayPtr;
    UINTN BitArraySizeInBytes;
    UINT8 *MemoryBlockPtr;
    UINTN MemoryBlockSizeInBytes;
    struct _MEMORY_MANAGE_HEADER *Next;
} MEMORY_MANAGE_HEADER;

#define USB_UHC_DEV_SIGNATURE  EFI_SIGNATURE_32( 'p', 'u', 'h', 'c' )
typedef struct
{
    UINTN Signature;

    EFI_PEI_SERVICES **PeiServices;
    PEI_USB_HOST_CONTROLLER_PPI UsbHostControllerPpi;
    EFI_PEI_PPI_DESCRIPTOR PpiDescriptor;
    EFI_PEI_CPU_IO_PPI   *CpuIoPpi;
    EFI_PEI_STALL_PPI    *StallPpi;

    UINT32 UsbHostControllerBaseAddress;
    FRAMELIST_ENTRY  *FrameListEntry;
    //
    // Header1 used for QH,TD memory blocks management
    //
    MEMORY_MANAGE_HEADER *Header1;
    //
    // Header2 used for Data transfer memory blocks management
    //
    MEMORY_MANAGE_HEADER *Header2;
	UINT16  PortResetStatusChangeMap;
    QH_STRUCT   *IntQH;
    TD_STRUCT   *IntTD;    
	UINT8		PollingData[8];
    UINT8       ControllerIndex;
} USB_UHC_DEV;

#define PEI_RECOVERY_USB_UHC_DEV_FROM_UHCI_THIS( a ) \
    PEI_CR( a, USB_UHC_DEV, UsbHostControllerPpi, USB_UHC_DEV_SIGNATURE )

EFI_STATUS
EFIAPI
UhcControlTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT8                       MaximumPacketLength,
    IN UINT16                      TransactionTranslator    OPTIONAL,
    IN EFI_USB_DEVICE_REQUEST      *Request,
    IN EFI_USB_DATA_DIRECTION      TransferDirection,
    IN OUT VOID *Data              OPTIONAL,
    IN OUT UINTN *DataLength       OPTIONAL,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
);

EFI_STATUS
EFIAPI
UhcBulkTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       EndPointAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT16                      MaximumPacketLength,
    IN UINT16                      TransactionTranslator    OPTIONAL,
    IN OUT VOID                    *Data,
    IN OUT UINTN                   *DataLength,
    IN OUT UINT8                   *DataToggle,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
);

EFI_STATUS
EFIAPI
UhcSyncInterruptTransfer(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI  *This,
    IN     UINT8                    DeviceAddress,
    IN     UINT8                    EndPointAddress,
    IN     UINT8                    DeviceSpeed,
    IN     UINT8                    MaximumPacketLength,
    IN UINT16                       TransactionTranslator,
    IN OUT VOID                     *Data,
    IN OUT UINTN                    *DataLength,
    IN OUT UINT8                    *DataToggle,
    IN     UINTN                    TimeOut,
    OUT    UINT32                   *TransferResult
);

EFI_STATUS
EFIAPI
UhciHcActivatePolling(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN VOID                        *UsbDev
);

EFI_STATUS
EFIAPI
UhcGetRootHubPortNumber(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
);

EFI_STATUS
EFIAPI
UhcGetRootHubPortStatus(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS        *PortStatus
);

EFI_STATUS
EFIAPI
UhcSetRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS
EFIAPI
UhcClearRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS
EFIAPI
UhciHcReset(
  	IN EFI_PEI_SERVICES            **PeiServices,
  	IN PEI_USB_HOST_CONTROLLER_PPI *This,
  	IN UINT16                      Attributes
);


EFI_STATUS
InitializeUsbHC (
    USB_UHC_DEV *UhcDev );

EFI_STATUS
CreateFrameList (
    USB_UHC_DEV *UhcDev );

VOID
InitFrameList (
    USB_UHC_DEV *UhcDev );

UINT16
USBReadPortW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port );


UINT32
USBReadPortDW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port );

VOID
USBWritePortW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port,
    IN UINT16      Data );

VOID
USBWritePortDW (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      Port,
    IN UINT32      Data );


VOID
ClearStatusReg (
    IN USB_UHC_DEV *UhcDev,
    IN UINT32      STSAddr );


UINT32
GetFrameListBaseAddrFromRegister (
    USB_UHC_DEV *UhcDev,
    UINT32      FLBAAddr );

BOOLEAN
IsHostSysErr (
    USB_UHC_DEV *UhcDev,
    UINT32      StatusRegAddr );

BOOLEAN
IsHCProcessErr (
    USB_UHC_DEV *UhcDev,
    UINT32      StatusRegAddr );

BOOLEAN
IsHCHalted (
    USB_UHC_DEV *UhcDev,
    UINT32      StatusRegAddr );


UINT16
GetCurrentFrameNumber (
    USB_UHC_DEV *UhcDev,
    UINT32      FRNUMAddr );

VOID
SetFrameListBaseAddress (
    USB_UHC_DEV *UhcDev,
    UINT32      FLBASEADDRReg,
    UINT32      Addr );


EFI_STATUS
AllocateQHStruct (
    USB_UHC_DEV *UhcDev,
    QH_STRUCT   **ppQHStruct );

VOID
InitQH (
    IN QH_STRUCT *ptrQH );


EFI_STATUS
CreateQH (
    USB_UHC_DEV *UhcDev,
    QH_STRUCT   **pptrQH );

VOID
SetQHHorizontalLinkPtr (
    IN QH_STRUCT *ptrQH,
    IN VOID      *ptrNext );

VOID *
GetQHHorizontalLinkPtr (
    IN QH_STRUCT *ptrQH );

VOID
SetQHHorizontalQHorTDSelect (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bQH );


VOID
SetQHHorizontalValidorInvalid (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bValid );

VOID
SetQHVerticalLinkPtr (
    IN QH_STRUCT *ptrQH,
    IN VOID      *ptrNext );

VOID *
GetQHVerticalLinkPtr (
    IN QH_STRUCT *ptrQH );

VOID
SetQHVerticalQHorTDSelect (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bQH );

BOOLEAN
IsQHHorizontalQHSelect (
    IN QH_STRUCT *ptrQH );

VOID
SetQHVerticalValidorInvalid (
    IN QH_STRUCT *ptrQH,
    IN BOOLEAN   bValid );


BOOLEAN
GetQHVerticalValidorInvalid (
    IN QH_STRUCT *ptrQH );

BOOLEAN
GetQHHorizontalValidorInvalid (
    IN QH_STRUCT *ptrQH );


EFI_STATUS
AllocateTDStruct (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   **ppTDStruct );

VOID
InitTD (
    IN TD_STRUCT *ptrTD );

EFI_STATUS
CreateTD (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   **pptrTD );

EFI_STATUS
GenSetupStageTD (
    USB_UHC_DEV *UhcDev,
    UINT8       DevAddr,
    UINT8       Endpoint,
    UINT8       DeviceSpeed,
    UINT8       *pDevReq,
    UINT8       RequestLen,
    TD_STRUCT   **ppTD );

EFI_STATUS
GenDataTD (
    USB_UHC_DEV *UhcDev,
    UINT8       DevAddr,
    UINT8       Endpoint,
    UINT8       *pData,
    UINT16      Len,
    UINT8       PktID,
    UINT8       Toggle,
    UINT8       DeviceSpeed,
    TD_STRUCT   **ppTD );

EFI_STATUS
CreateStatusTD (
    USB_UHC_DEV *UhcDev,
    UINT8       DevAddr,
    UINT8       Endpoint,
    UINT8       PktID,
    UINT8       DeviceSpeed,
    TD_STRUCT   **ppTD );


VOID
SetTDLinkPtrValidorInvalid (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bValid );

VOID
SetTDLinkPtrQHorTDSelect (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bQH );

VOID
SetTDLinkPtrDepthorBreadth (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bDepth );

VOID
SetTDLinkPtr (
    IN TD_STRUCT *ptrTDStruct,
    IN VOID      *ptrNext );

VOID *
GetTDLinkPtr (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDLinkPtrQHOrTD (
    IN TD_STRUCT *ptrTDStruct );

VOID
EnableorDisableTDShortPacket (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bEnable );

VOID
SetTDControlErrorCounter (
    IN TD_STRUCT *ptrTDStruct,
    IN UINT8     nMaxErrors );


VOID
SetTDLoworFullSpeedDevice (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bLowSpeedDevice );

VOID
SetTDControlIsochronousorNot (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bIsochronous );

VOID
SetorClearTDControlIOC (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bSet );

VOID
SetTDStatusActiveorInactive (
    IN TD_STRUCT *ptrTDStruct,
    IN BOOLEAN   bActive );

UINT16
SetTDTokenMaxLength (
    IN TD_STRUCT *ptrTDStruct,
    IN UINT16    nMaxLen );

VOID
SetTDTokenDataToggle1 (
    IN TD_STRUCT *ptrTDStruct );

VOID
SetTDTokenDataToggle0 (
    IN TD_STRUCT *ptrTDStruct );

UINT8
GetTDTokenDataToggle (
    IN TD_STRUCT *ptrTDStruct );

VOID
SetTDTokenEndPoint (
    IN TD_STRUCT *ptrTDStruct,
    IN UINTN     nEndPoint );

VOID
SetTDTokenDeviceAddress (
    IN TD_STRUCT *ptrTDStruct,
    IN UINTN     nDevAddr );

VOID
SetTDTokenPacketID (
    IN TD_STRUCT *ptrTDStruct,
    IN UINT8     nPID );

VOID
SetTDDataBuffer (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDStatusActive (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDStatusStalled (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDStatusBufferError (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDStatusBabbleError (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDStatusNAKReceived (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDStatusCRCTimeOutError (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
IsTDStatusBitStuffError (
    IN TD_STRUCT *ptrTDStruct );

UINT16
GetTDStatusActualLength (
    IN TD_STRUCT *ptrTDStruct );

UINT16
GetTDTokenMaxLength (
    IN TD_STRUCT *ptrTDStruct );

UINT8
GetTDTokenEndPoint (
    IN TD_STRUCT *ptrTDStruct );

UINT8
GetTDTokenDeviceAddress (
    IN TD_STRUCT *ptrTDStruct );

UINT8
GetTDTokenPacketID (
    IN TD_STRUCT *ptrTDStruct );

UINT8 *
GetTDDataBuffer (
    IN TD_STRUCT *ptrTDStruct );

BOOLEAN
GetTDLinkPtrValidorInvalid (
    IN TD_STRUCT *ptrTDStruct );

UINTN
CountTDsNumber (
    IN TD_STRUCT *ptrFirstTD );

VOID
LinkTDToQH (
    IN QH_STRUCT *ptrQH,
    IN TD_STRUCT *ptrTD );

VOID
LinkTDToTD (
    IN TD_STRUCT *ptrPreTD,
    IN TD_STRUCT *ptrTD );


//
// Transfer Schedule related Helper Functions
//
VOID
SetorClearCurFrameListTerminate (
    IN FRAMELIST_ENTRY *pCurEntry,
    IN BOOLEAN         bSet );

VOID
SetCurFrameListQHorTD (
    IN FRAMELIST_ENTRY *pCurEntry,
    IN BOOLEAN         bQH );

BOOLEAN
IsCurFrameListQHorTD (
    IN FRAMELIST_ENTRY *pCurEntry );

BOOLEAN
GetCurFrameListTerminate (
    IN FRAMELIST_ENTRY *pCurEntry );

VOID
SetCurFrameListPointer (
    IN FRAMELIST_ENTRY *pCurEntry,
    IN UINT8           *ptr );

VOID *
GetCurFrameListPointer (
    IN FRAMELIST_ENTRY *pCurEntry );

VOID
LinkQHToFrameList (
    IN FRAMELIST_ENTRY *pEntry,
    IN UINT16          FrameListIndex,
    IN QH_STRUCT       *ptrQH );

EFI_STATUS
ExecuteControlTransfer (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   *ptrTD,
    UINT32      wIndex,
    UINTN       *ActualLen,
    UINTN       TimeOut,
    UINT32      *TransferResult );

EFI_STATUS
ExecBulkTransfer (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   *ptrTD,
    UINT32      wIndex,
    UINTN       *ActualLen,
    UINT8       *DataToggle,
    UINTN       TimeOut,
    UINT32      *TransferResult );

VOID
DeleteSingleQH (
    USB_UHC_DEV *UhcDev,
    QH_STRUCT   *ptrQH,
    UINT16      FrameListIndex,
    BOOLEAN     SearchOther );

VOID
DeleteQueuedTDs (
    USB_UHC_DEV *UhcDev,
    TD_STRUCT   *ptrFirstTD );


BOOLEAN
CheckTDsResults (
    IN TD_STRUCT *ptrTD,
    OUT UINT32   *Result,
    OUT UINTN    *ErrTDPos,
    OUT UINTN    *ActualTransferSize );

VOID
ClearTDStatus (
    IN TD_STRUCT *ptrTD );

VOID
SelfLinkBulkTransferQH (
    IN QH_STRUCT *ptrQH );

EFI_STATUS
CreateMemoryBlock (
    USB_UHC_DEV          *UhcDev,
    MEMORY_MANAGE_HEADER **MemoryHeader,
    UINTN                MemoryBlockSizeInPages );

EFI_STATUS
InitializeMemoryManagement (
    USB_UHC_DEV *UhcDev );

EFI_STATUS
UhcAllocatePool (
    USB_UHC_DEV *UhcDev,
    UINT8       Type,
    UINT8       **Pool,
    UINTN       AllocSize );

EFI_STATUS
AllocMemInMemoryBlock (
    MEMORY_MANAGE_HEADER *MemoryHeader,
    VOID                 **Pool,
    UINTN                NumberOfMemoryUnit );


VOID
UhcFreePool (
    USB_UHC_DEV *UhcDev,
    UINT8       Type,
    UINT8       *Pool,
    UINTN       AllocSize );

VOID
InsertMemoryHeaderToList (
    MEMORY_MANAGE_HEADER *MemoryHeader,
    MEMORY_MANAGE_HEADER *NewMemoryHeader );

BOOLEAN
IsMemoryBlockEmptied (
    MEMORY_MANAGE_HEADER *MemoryHeaderPtr );

VOID
DelinkMemoryBlock (
    MEMORY_MANAGE_HEADER *FirstMemoryHeader,
    MEMORY_MANAGE_HEADER *FreeMemoryHeader );

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
