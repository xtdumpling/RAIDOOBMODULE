//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        UsbBotPeim.h
//
// Description: 
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>



#ifndef _DBG_XHCI_H
#define _DBG_XHCI_H

#include "Efi.h"
#include "Pei.h"
#include <Library/BaseMemoryLib.h>
#include <Token.h>
#include "XhciPei.h"

#define PEI_FAT_MAX_USB_IO_PPI  127

//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.BaseClass
//---------------------------------------------------------------------------
#define BASE_CLASS_HID           0x03
#define BASE_CLASS_MASS_STORAGE  0x08
#define BASE_CLASS_HUB           0x09
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.Protocol
//---------------------------------------------------------------------------
#define PROTOCOL_KEYBOARD  0x01         // Keyboard device protocol
#define PROTOCOL_MOUSE     0x02         // Mouse device protocol?

// Mass storage related protocol equates
#define PROTOCOL_CBI         0x00       // Mass Storage Control/Bulk/Interrupt
                                        // with command completion interrupt
#define PROTOCOL_CBI_NO_INT  0x01       // MASS STORAGE Control/Bulk/Interrupt
                                        // with NO command completion interrupt
#define PROTOCOL_BOT         0x50       // Mass Storage Bulk-Only Transport
#define PROTOCOL_VENDOR      0xff       // Vendor specific mass protocol
//---------------------------------------------------------------------------



#define USBFLOPPY   1
#define USBFLOPPY2  2   // for those that use ReadCapacity(0x25)
                        // command to retrieve media capacity
#define USBCDROM    3
//
// Bot device structure
//


#define ONE_SECOND_DELAY  1000000 // 1 second = 1000000 microseconds

#define TRBS_PER_SEGMENT    64
#define RING_SIZE TRBS_PER_SEGMENT*sizeof(XHCI_TRB)

#define XHCI_BOT_TD_MAXSIZE 0x10000
#define XHCI_BOT_MAX_XFR_SIZE XHCI_BOT_TD_MAXSIZE*8


#if (USB_3_DEBUG_SUPPORT == 1)
EFI_STATUS 
XhciPeiUsbEntryPoint (
	IN EFI_PEI_FILE_HANDLE     FileHandle,
	IN CONST EFI_PEI_SERVICES  **PeiServices );
#endif

EFI_STATUS
XhciInitInEpRing (
    IN OUT TRB_RING *Ring,
    IN UINTN    RingBase,
    IN UINT32   RingSize,
    IN BOOLEAN  PlaceLinkTrb);

EFI_STATUS
XhciDbCapBulkOut(
    UINT8  *DataBuffer,
    UINT32  *Length,
    UINT16	TimeoutMs);

#pragma pack(push, 1)

typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
            #define     XHCI_DCCTRL_LSE BIT1
            #define     XHCI_DCCTRL_DCE BIT31
        struct {
            UINT32      Dcr             : 1;    // 0 Dbc Run - RO
            UINT32      Lse             : 1;    // 1 Link Status Event Enable - RW
            UINT32      Hot             : 1;    // 2 Halt OUT TR - RW1S
            UINT32      Hit             : 1;    // 3 Halt IN TR - RW1S
            UINT32      Drc             : 1;    // 4 Dbc Run Change - RW1C
            UINT32      RsvdZ           : 11;   // 5..15 
            UINT32      DbMaxBurstSize  : 8;    // 16..23 Debug Max Burst Size - RO
            UINT32      Address         : 7;    // 24..30 Device Address - RO
            UINT32      Dce             : 1;    // 31 Debug Capability Enable - RW
        } Field;
    };
} XHCI_DB_CONTROL;

typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
            #define XHCI_PCS_CCS    BIT0
            #define XHCI_PCS_PED    BIT1
            #define XHCI_PCS_PR     BIT4
            #define XHCI_PCS_CSC    BIT17
            #define XHCI_PCS_PRC    BIT21
			#define XHCI_PCS_CEC    BIT23
        struct {
            UINT32      Ccs         : 1;    // 0 Current Connect Status - RO
            UINT32      Ped         : 1;    // 1 Port Enabled/Disabled  - RW1CS
            UINT32      RsvdZ1      : 2;    // 2..3
            UINT32      Pr          : 1;    // 4 Port Reset              - RW1S
            UINT32      Pls         : 4;    // 5..8 Port Link State      - RWS
            UINT32      RsvdZ2      : 1;    // 9
            UINT32      PortSpeed   : 4;    // 10..13 Port Speed         - RO
            UINT32      RsvdZ3      : 3;    // 14..16
            UINT32      Csc         : 1;    // 17 Connect Status Change  - RW1CS
            UINT32      RsvdZ4      : 3;    // 18..20
            UINT32      Prc         : 1;    // 21 Port Reset Change      - RW1CS
            UINT32      Plc         : 1;    // 22 Port Link State Change - RW1CS
            UINT32      Cec         : 1;    // 23 Port Config Error Change  - RW1CS/RsvdZ
            UINT32      RsvdZ5      : 8;    // 24..31
        } Field;
    };
} XHCI_DB_PORTSC;

typedef struct {
    UINT8                   CapId;
    UINT8                   NextCapPtr;
    UINT8                   DcerstMax;
    UINT8                   Resv1;
    UINT8                   Resv2;
    UINT8                   DbTarget;
    UINT16                  Resv3;
    UINT16                  EventRingSegmentTableSize;
    UINT16                  Resv4;
    UINT32                  Resv5;
    UINT64                  EventRingSegmentBassAddress;
    UINT64                  DequeuePointer;
    XHCI_DB_CONTROL         Dcctrl;
    UINT32                  Dcst;
    XHCI_DB_PORTSC          DcPortSc;
    UINT32                  Resv6;
    UINT64                  Dccp;
    UINT8                   DbcProtocol;
    UINT8                   Resv7;
    UINT16                  VendorId;
    UINT16                  ProductId;
    UINT16                  DeviceRevision;
} XHCI_DB_CAP_REGS;

typedef struct {
    UINT64  Str0DecAddr;
    UINT64  ManufacturerStrDecAddr;
    UINT64  ProductStrDecAddr;
    UINT64  SerNumStrDecAddr;
    UINT32  Str0Length              :8;
    UINT32  ManufacturerStrLength   :8;
    UINT32  ProductStrLength        :8;
    UINT32  SerNumStrLength         :8;
    UINT32  RsvdO[7];
} XHCI_DB_CAP_INFO_CONTEXT;

typedef struct {
    UINT32  EpState         : 3;
    UINT32  RsvdZ1          : 5;
    UINT32  Mult            : 2;
    UINT32  MaxPStreams     : 5;
    UINT32  Lsa             : 1;
    UINT32  Interval        : 8;
    UINT32  RzvdZ2          : 8;
    UINT32  RzvdZ3          : 1;
    UINT32  ErrorCount      : 2;
    UINT32  EpType          : 3;
    UINT32  RsvdZ           : 1;
    UINT32  Hid             : 1;
    UINT32  MaxBurstSize    : 8;
    UINT32  MaxPacketSize   : 16;
    UINT64  TrDequeuePtr; 
    UINT16  AvgTrbLength;
    UINT16  MaxEsitPayload;
    UINT32  RsvdO[11];
} XHCI_DB_CAP_EP_CONTEXT;


typedef struct {
    UINT8                           OutBuffer[XHCI_BOT_MAX_XFR_SIZE];
    UINT8                           InBuffer[XHCI_BOT_MAX_XFR_SIZE];
    XHCI_DB_CAP_INFO_CONTEXT        DbcInfoContext;
    XHCI_DB_CAP_EP_CONTEXT          OutEpContext;
    XHCI_DB_CAP_EP_CONTEXT          InEpContext;
    UINT8                           EventRing[RING_SIZE];
    UINT8                           OutXfrRing[RING_SIZE];
    UINT8                           InXfrRing[RING_SIZE];
    XHCI_ER_SEGMENT_ENTRY           Erst;
    TRB_RING                        EvtRing;
    TRB_RING                        OutEpRing;
    TRB_RING                        InEpRing;
    UINT16                          Vid;
    UINT16                          Did;
    XHCI_HC_CAP_REGS                *CapRegs;
    XHCI_HC_OP_REGS                 *OpRegs;
    XHCI_DB_CAP_REGS                *DbCapRegs;
    UINT8                           String0[0x80];
    UINT8                           ManufacturerStr[0x80];
    UINT8                           ProductStr[0x80];
    UINT8                           SerNumStr[0x80];
    UINT8                           DoorbellInOut;
    UINT16                          BusDevFuncNum;
} XHCI_DB_CAP_DATA;

#pragma pack(pop)

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
