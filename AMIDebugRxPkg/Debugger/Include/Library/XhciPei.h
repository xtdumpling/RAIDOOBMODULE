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
// Name:        XhciPei.h
//
// Description:
//  This file is the main header file for the EHCI PEI USB recovery module. It
//  contains generic constant and type declarations/definitions.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#if (USB_3_DEBUG_SUPPORT == 1)

#ifndef _XHCIPEI_H
#define _XHCIPEI_H

#ifndef EFI_x64
#include <Pei.h>
#endif
#include "UsbHostController.h"
#include "PPI/Stall.h"

UINT32
EFIAPI
IoRead32 (
  IN      UINTN                     Port
  );
//----------------------------------------------------------------------------
// XHCI_PCI_ADDRESS( ControllerIndex, Register )
//
// allows easy generation of a valid PCI address using an index into the 
// gXhciControllerPciTable and a register number
#define XHCI_PCI_ADDRESS( ControllerIndex, Register )  PEI_PCI_CFG_ADDRESS( \
    gXhciControllerPciTable[ControllerIndex].Bus, \
    gXhciControllerPciTable[ControllerIndex].Device, \
    gXhciControllerPciTable[ControllerIndex].Function, \
    Register )

/*#define XHCI_FIXED_DELAY_MS( Usb3Hc, milliseconds ) \
    Usb3Hc->StallPpi->Stall( Usb3Hc->PeiServices, Usb3Hc->StallPpi, milliseconds * 1000 );

#define XHCI_FIXED_DELAY_15MCS( Usb3Hc, units ) \
    Usb3Hc->StallPpi->Stall( Usb3Hc->PeiServices, Usb3Hc->StallPpi, units * 15 );*/

//    CountTime(milliseconds * 1000, PM_BASE_ADDRESS);

#define XHCI_SWITCH2SS_DELAY_MS 5
#define MAX_CONTROL_DATA_SIZE   0x200

#define EP_DESC_FLAG_TYPE_BITS  0x03    //Bit 1-0: Indicate type of transfer on endpoint
#define EP_DESC_FLAG_TYPE_CONT  0x00    //Bit 1-0: 00 = Endpoint does control transfers
#define EP_DESC_FLAG_TYPE_ISOC  0x01    //Bit 1-0: 01 = Endpoint does isochronous transfers
#define EP_DESC_FLAG_TYPE_BULK  0x02    //Bit 1-0: 10 = Endpoint does bulk transfers
#define EP_DESC_FLAG_TYPE_INT   0x03    //Bit 1-0: 11 = Endpoint does interrupt transfers

#pragma pack(push,1)

typedef struct {
    UINT8 Bus;
    UINT8 Device;
    UINT8 Function;
} PCI_BUS_DEV_FUNCTION;

typedef struct {
    UINT8   Bus;
    UINT8   Device;
    UINT8   Function;
    UINT32  Register;
    UINT8   Size;
    UINT32  SetBits;
    UINT32  ClearBits;
} PCI_DEV_REGISTER_VALUE;

// XHCI_EP_CONTEXT.DW0.State definitions
#define XHCI_EP_STATE_DISABLED  0
#define XHCI_EP_STATE_RUNNING   1
#define XHCI_EP_STATE_HALTED    2
#define XHCI_EP_STATE_STOPPED   3
#define XHCI_EP_STATE_ERROR     4

// XHCI_EP_CONTEXT.DW1.EpType definitions
#define XHCI_EP_TYPE_NOTVALID   0
#define XHCI_EP_TYPE_ISO_OUT    1
#define XHCI_EP_TYPE_BLK_OUT    2
#define XHCI_EP_TYPE_INT_OUT    3
#define XHCI_EP_TYPE_CONTROL    4
#define XHCI_EP_TYPE_ISO_IN     5
#define XHCI_EP_TYPE_BLK_IN     6
#define XHCI_EP_TYPE_INT_IN     7

//---------------------------------------------------------
// Device context definition
//---------------------------------------------------------

// TRB completion codes Table 130
#define XHCI_TRB_INVALID 0
#define XHCI_TRB_SUCCESS 1
#define XHCI_TRB_DATABUF_ERROR 2
#define XHCI_TRB_BABBLE_ERROR 3
#define XHCI_TRB_TRANSACTION_ERROR  4
#define XHCI_TRB_TRB_ERROR 5
#define XHCI_TRB_STALL_ERROR 6
#define XHCI_TRB_RESOURCE_ERROR 7
#define XHCI_TRB_BANDWIDTH_ERROR 8
#define XHCI_TRB_OUTOFSLOTS_ERROR 9
#define XHCI_TRB_INVALIDSTREAMTYPE_ERROR 10
#define XHCI_TRB_SLOTNOTENABLED_ERROR 11
#define XHCI_TRB_ENDPOINTNOTENABLED_ERROR 12
#define XHCI_TRB_SHORTPACKET 13
#define XHCI_TRB_RINGUNDERRUN 14
#define XHCI_TRB_RINGOVERRUN 15
#define XHCI_TRB_VFRINGFULL_ERROR 16
#define XHCI_TRB_PARAMETER_ERROR 17
#define XHCI_TRB_BANDWIDTHOVERRUN_ERROR 18
#define XHCI_TRB_CONTEXTSTATE_ERROR 19
#define XHCI_TRB_NOPINGRESPONSE_ERROR 20
#define XHCI_TRB_EVENTRINGFULL_ERROR 21
#define XHCI_TRB_MISSEDSERVICE_ERROR 23
#define XHCI_TRB_CMDRINGSTOPPED 24
#define XHCI_TRB_COMMANDABORTED 25
#define XHCI_TRB_STOPPED 26
#define XHCI_TRB_STOPPEDLENGTHINVALID 27
#define XHCI_TRB_CONTROLABORT_ERROR 28
#define XHCI_TRB_ISOCHBUFOVERRUN 31
#define XHCI_TRB_EVENTLOST_ERROR 32
#define XHCI_TRB_UNDEFINED_ERROR 33
#define XHCI_TRB_INVALIDSTREAMID_ERROR 34
#define XHCI_TRB_SECONDARYBANDWIDTH_ERROR 35
#define XHCI_TRB_SPLITTRANSACTION_ERROR 36

#define XHCI_TRB_EXECUTION_TIMEOUT_ERROR 255

//---------------------------------------------------------
// Transfer Descriptor Block (TRB) definitions, section 4.11
//---------------------------------------------------------
// TRB Template
typedef struct {
    UINT32  Param1;
    UINT32  Param2;

    UINT32  RsvdZ1          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ3      : 16;
} XHCI_TRB;

// Event TRB types, Section 6.4.2
typedef struct {
    UINT64  TrbPtr;

    UINT32  TransferLength  : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ1      : 1;
    UINT32  EventData   : 1;
    UINT32  RsvdZ2      : 7;
    UINT32  TrbType     : 6;
    UINT32  EndpointId  : 5;
    UINT32  RzvdZ3      : 3;
    UINT32  SlotId      : 8;
} XHCI_TRANSFER_EVT_TRB;

typedef struct {
    UINT64  CmdTrbPtr;

    UINT32  RsvdZ1          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 9;
    UINT32  TrbType     : 6;
    UINT32  VfId        : 8;
    UINT32  SlotId      : 8;
} XHCI_CMDCOMPLETE_EVT_TRB;

typedef struct {
    UINT32  RsvdZ1      : 24;
    UINT32  PortId      : 8;

    UINT32  RsvdZ2;

    UINT32  RsvdZ3          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ4      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ5      : 16;
} XHCI_PORTSTSCHG_EVT_TRB;

typedef struct {
    UINT32  RsvdZ1[2];

    UINT32  RsvdZ2          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ3      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ4      : 8;
    UINT32  SlotId      : 8;
} XHCI_BANDWIDTHRQ_EVT_TRB;

typedef struct {
    UINT32  DbReason    : 5;
    UINT32  RsvdZ1      : 27;

    UINT32  Rsvd2;

    UINT32  RsvdZ3          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ4      : 9;
    UINT32  TrbType     : 6;
    UINT32  VfId        : 8;
    UINT32  SlotId      : 8;
} XHCI_DORBELL_EVT_TRB;

typedef struct {
    UINT32  RsvdZ1[2];

    UINT32  RsvdZ2          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ3      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ4      : 16;
} XHCI_HC_EVT_TRB;

typedef struct {
    UINT8   RsvdZ1      : 4;
    UINT8   NtfType     : 4;

    UINT8   DevNtfData[7];

    UINT32  RsvdZ2          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ3      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ4      : 8;
    UINT32  SlotId      : 8;
} XHCI_DEVNOTIFY_EVT_TRB;

typedef struct {
    UINT32  RsvdZ[2];

    UINT32  RsvdZ2          : 24;
    UINT32  CompletionCode  : 8;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ3      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ4      : 16;
} XHCI_MFINDXWRAP_EVT_TRB;

typedef union {
    XHCI_TRANSFER_EVT_TRB       TransferEvt;
    XHCI_CMDCOMPLETE_EVT_TRB    CmdEvt;
    XHCI_PORTSTSCHG_EVT_TRB     PortStsChgEvt;
    XHCI_BANDWIDTHRQ_EVT_TRB    BandwidthRqEvt;
    XHCI_DORBELL_EVT_TRB        DoorbellEvt;
    XHCI_HC_EVT_TRB             HcEvt;
    XHCI_DEVNOTIFY_EVT_TRB      DevNotificationEvt;
    XHCI_MFINDXWRAP_EVT_TRB     MicroframeIndxWrapEvt;
} XHCI_EVENT_TRB;

// Command TRB types, Section 6.4.3
typedef struct {
    UINT32  RsvdZ1[3];

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ3      : 16;
} XHCI_COMMON_CMD_TRB;

typedef struct {
    UINT32  RsvdZ1[3];

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ3      : 8;
    UINT32  SlotId      : 8;
} XHCI_DISABLESLOT_CMD_TRB;

typedef struct {
    UINT64  InpCtxAddress;
    UINT32  RsvdZ1;
    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 8;
    UINT32  Bsr         : 1;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ3      : 8;
    UINT32  SlotId      : 8;
} XHCI_ADDRESSDEV_CMD_TRB;

typedef struct {
    UINT32  RsvdZ1[3];

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 8;
    UINT32  Tsp         : 1;
    UINT32  TrbType     : 6;
    UINT32  EndpointId  : 5;
    UINT32  RsvdZ3      : 3;
    UINT32  SlotId      : 8;
} XHCI_RESET_EP_CMD_TRB;

typedef struct {
    UINT64  TrPointer;

    UINT32  RsvdZ1      :16;
    UINT32  StreamId    :16;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 9;
    UINT32  TrbType     : 6;
    UINT32  EndpointId  : 5;
    UINT32  RsvdZ3      : 3;
    UINT32  SlotId      : 8;
} XHCI_SET_TRPTR_CMD_TRB;

typedef union {
    XHCI_COMMON_CMD_TRB         GenericCmdTrb;
    XHCI_COMMON_CMD_TRB         NoOpCmdTrb;
    XHCI_COMMON_CMD_TRB         EnableSlotCmdTrb;
    XHCI_DISABLESLOT_CMD_TRB    DisableSlotCmdTrb;
    XHCI_ADDRESSDEV_CMD_TRB     AddressDevCmdTrb;
    XHCI_SET_TRPTR_CMD_TRB      SetTrPtrCmdTrb;
} XHCI_CMD_TRB;

// Transfer TRB types, Section 6.4.1

typedef struct {
    UINT64  DataBuffer;

    UINT32  XferLength  : 17;
    UINT32  TdSize      : 5;
    UINT32  Interrupter : 10;

    UINT32  CycleBit    : 1;
    UINT32  EvalNext    : 1;
    UINT32  Isp         : 1;
    UINT32  NoSnoop     : 1;
    UINT32  Chain       : 1;
    UINT32  Ioc         : 1;
    UINT32  Idt         : 1;
    UINT32  RsvdZ1      : 2;
	UINT32	Bei			: 1;
    UINT32  TrbType     : 6;
    UINT32  Rsvd2       : 16;
} XHCI_NORMAL_XFR_TRB;

#define	XHCI_XFER_TYPE_NO_DATA		0
#define	XHCI_XFER_TYPE_DATA_OUT		2
#define	XHCI_XFER_TYPE_DATA_IN		3

typedef struct {
    UINT8   bmRequestType;
    UINT8   bRequest;
    UINT16  wValue;
    UINT16  wIndex;
    UINT16  wLength;

    UINT32  XferLength  : 17;
    UINT32  RsvdZ1      : 5;
    UINT32  Interrupter : 10;

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 4;
    UINT32  Ioc         : 1;
    UINT32  Idt         : 1;
    UINT32  RsvdZ3      : 3;
    UINT32  TrbType     : 6;
	UINT32	Trt			: 2;
    UINT32  RsvdZ4      : 14;
} XHCI_SETUP_XFR_TRB;

typedef struct {
    UINT64  DataBuffer;

    UINT32  XferLength  : 17;
    UINT32  TdLength    : 5;
    UINT32  Interrupter : 10;

    UINT32  CycleBit    : 1;
    UINT32  EvalNext    : 1;
    UINT32  Isp         : 1;
    UINT32  NoSnoop     : 1;
    UINT32  Chain       : 1;
    UINT32  Ioc         : 1;
    UINT32  Idt         : 1;
    UINT32  RsvdZ1      : 3;
    UINT32  TrbType     : 6;
    UINT32  Dir         : 1;
    UINT32  Rsvd2       : 15;
} XHCI_DATA_XFR_TRB;

typedef struct {
    UINT64  RsvdZ1;

    UINT32  RsvdZ2      : 22;
    UINT32  Interrupter : 10;

    UINT32  CycleBit    : 1;
    UINT32  EvalNext    : 1;
    UINT32  RsvdZ3      : 2;
    UINT32  Chain       : 1;
    UINT32  Ioc         : 1;
    UINT32  RsvdZ4      : 4;
    UINT32  TrbType     : 6;
    UINT32  Dir         : 1;
    UINT32  Rsvd2       : 15;
} XHCI_STATUS_XFR_TRB;

typedef struct {
    UINT64  DataBuffer;

    UINT32  XferLength  : 17;
    UINT32  TdLength    : 5;
    UINT32  Interrupter : 10;

    UINT32  CycleBit    : 1;
    UINT32  EvalNext    : 1;
    UINT32  Isp         : 1;
    UINT32  NoSnoop     : 1;
    UINT32  Chain       : 1;
    UINT32  Ioc         : 1;
    UINT32  Idt         : 1;
    UINT32  RsvdZ1      : 3;
    UINT32  TrbType     : 6;
    UINT32  Rsvd2       : 4;
    UINT32  FrameId     : 11;
    UINT32  Sia         : 1;
} XHCI_ISOCH_XFR_TRB;

typedef struct {
    UINT64  RsvdZ1;

    UINT32  RsvdZ2      : 22;
    UINT32  Interrupter : 10;

    UINT32  CycleBit    : 1;
    UINT32  EvalNext    : 1;
    UINT32  RsvdZ3      : 2;
    UINT32  Chain       : 1;
    UINT32  Ioc         : 1;
    UINT32  RsvdZ4      : 4;
    UINT32  TrbType     : 6;
    UINT32  Rsvd2       : 16;
} XHCI_NOOP_XFR_TRB;

typedef union {
    XHCI_NORMAL_XFR_TRB     NormalXfrTrb;
    XHCI_SETUP_XFR_TRB      SetupXfrTrb;
    XHCI_DATA_XFR_TRB       DataXfrTrb;
    XHCI_STATUS_XFR_TRB     StatusXfrTrb;
    XHCI_ISOCH_XFR_TRB      IsockXfrTrb;
    XHCI_NOOP_XFR_TRB       NoopXfrTrb;
} XHCI_XFR_TRB;


// Other TRB types
typedef struct {
    UINT64  NextSegPtr;

    UINT32  RsvdZ1      : 22;
    UINT32  Interrupter : 10;

    UINT32  CycleBit    : 1;
    UINT32  ToggleCycle : 1;
    UINT32  RsvdZ2      : 2;
    UINT32  Chain       : 1;
    UINT32  Ioc         : 1;
    UINT32  RsvdZ3      : 4;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ4      : 16;
} XHCI_LINK_TRB;

typedef struct {
    XHCI_TRB    *Base;
    UINT32      Size;   // #of TRBs in the ring
    XHCI_TRB*   LastTrb;
    XHCI_TRB*   QueuePtr;
    UINT8       CycleBit;
    UINT8       Pad[27-3*sizeof(VOID*)];    // Make size 32 Bytes
} TRB_RING;

// The following definition fixes the size of ring
// segment to TRBS_PER_SEGMENT * sizeof(XHCI_TRB)
#define TRBS_PER_SEGMENT    64
#define RING_SIZE TRBS_PER_SEGMENT*sizeof(XHCI_TRB)

// Default timeouts
#define XHCI_CMD_COMPLETE_TIMEOUT_MS 500
#define XHCI_CTL_COMPLETE_TIMEOUT_MS 2000
#define XHCI_BULK_COMPLETE_TIMEOUT_MS 10000
#define XHCI_INT_COMPLETE_TIMEOUT_MS 1500

//#define XHCI_BOT_TD_MAXSIZE 512
#define XHCI_BOT_TD_MAXSIZE 0x10000
#define XHCI_BOT_MAX_XFR_SIZE XHCI_BOT_TD_MAXSIZE*8

typedef struct {
    UINT32  RouteString     : 20;
    UINT32  Speed           : 4;
    UINT32  RsvdZ1          : 1;
    UINT32  MultiTT         : 1;
    UINT32  Hub             : 1;
    UINT32  ContextEntries  : 5;

    UINT32  MaxExitLatency  : 16;
    UINT32  RootHubPort     : 8;
    UINT32  PortsNum        : 8;

    UINT32  TtHubSlotId     : 8;
    UINT32  TtPortNumber    : 8;
    UINT32  TThinkTime      : 2;
    UINT32  RsvdZ3          : 4;
    UINT32  Interrupter     : 10;

    UINT32  DevAddr         : 8;
    UINT32  RsvdZ4          : 19;
    UINT32  SlotState       : 5;

    UINT32  RsvdO[4];
} XHCI_SLOT_CONTEXT;

// XHCI_SLOT_CONTEXT.DW3.SlotState definitions
#define XHCI_SLOT_STATE_DISABLED    0
#define XHCI_SLOT_STATE_DEFAULT     1
#define XHCI_SLOT_STATE_ADDRESSED   2
#define XHCI_SLOT_STATE_CONFIGURED  3

// Endpoint types, Table 57
#define XHCI_EPTYPE_NOT_VALID   0
#define XHCI_EPTYPE_ISOCH_OUT   1
#define XHCI_EPTYPE_BULK_OUT    2
#define XHCI_EPTYPE_INT_OUT     3
#define XHCI_EPTYPE_CTL         4
#define XHCI_EPTYPE_ISOCH_IN    5
#define XHCI_EPTYPE_BULK_IN     6
#define XHCI_EPTYPE_INT_IN      7

typedef struct {
    UINT32  EpState         : 3;
    UINT32  RsvdZ1          : 5;
    UINT32  Mult            : 2;
    UINT32  MaxPStreams     : 5;
    UINT32  Lsa             : 1;
    UINT32  Interval        : 8;
    UINT32  RzvdZ2          : 8;

    UINT32  ForceEvent      : 1;
    UINT32  ErrorCount      : 2;
    UINT32  EpType          : 3;
    UINT32  RsvdZ           : 1;
    UINT32  Hid             : 1;
    UINT32  MaxBurstSize    : 8;
    UINT32  MaxPacketSize   : 16;

    UINT64  TrDequeuePtr;   // BIT0 of this field is DCS (Dequeue Cycle State)
    UINT16  AvgTrbLength;
    UINT16  MaxEsitPayload;
    UINT32  RsvdO[3];
} XHCI_EP_CONTEXT;

typedef struct {
    UINT32  DropContextFlags;
    UINT32  AddContextFlags;
    UINT32  RzvdZ[6];
} XHCI_INPUT_CONTROL_CONTEXT;

typedef struct {
    XHCI_SLOT_CONTEXT   Slot;
    XHCI_EP_CONTEXT     Ep[31];
} XHCI_DEVICE_CONTEXT;

#define	XHCI_DEVICE_CONTEXT_ENTRIES	32

typedef struct {
    XHCI_INPUT_CONTROL_CONTEXT  CtlCtx;
    XHCI_DEVICE_CONTEXT         DevCtx;
} XHCI_INPUT_CONTEXT;

#define	XHCI_INPUT_CONTEXT_ENTRIES	33

										//(EIP75547+)>
//---------------------------------------------------------
// Extended Capabilities
//---------------------------------------------------------
#define XHCI_EXT_CAP_USB_LEGACY                 1
#define XHCI_EXT_CAP_SUPPORTED_PROTOCOL         2
#define XHCI_EXT_CAP_POWERMANAGEMENT            3
#define XHCI_EXT_CAP_IO_VIRTUALIZATION          4
#define XHCI_EXT_CAP_USB_DEBUG_PORT             10

typedef struct {
    UINT32       CapId:8;		// Capability ID
    UINT32       NextCapPtr:8;	// Next xHCI Extended Capability Pointer
    UINT32       Cap:16;		// Capability Specific
} XHCI_EXT_CAP;
										//<(EIP75547+)
										
typedef struct {
    UINT64  ScratchpadBufArrayPtr;
    UINT64  DevCntxtAddr[255];
} XHCI_DCBAA;   // Total size is 256 64-bit entries, or 2K Bytes (section 6.1)
// XHCI PCI Configuration Registers
//---------------------------------------------------------------------------

// Serial Bus Release Number Register
#define XHCI_PCI_SBRN   0x60

// Frame Length Adjustment Register
#define XHCI_PCI_FLADJ  0x61

// Host Controller Capability Registers
//---------------------------------------------------------------------------

typedef struct {
    UINT32  MaxSlots    : 8;    // Number of Device Slots
    UINT32  MaxIntrs    : 11;   // Number of Interrupters
    UINT32  Rsvd        : 5;    // Reserved
    UINT32  MaxPorts    : 8;    // Number of ports
} HCPARAMS1;

typedef struct {
    UINT32  Ist         : 4;    // Isochronous Scheduling Threshold
    UINT32  ErstMax     : 4;    // Event Ring Segment Table Max
    UINT32  Rsvd        : 18;
    UINT32  Spr         : 1;    // Scratchpad restore
    UINT32  MaxScratchPadBufs   : 5;    // Number of scratchpad buffers
} HCPARAMS2;

typedef struct {
    UINT32  U1DevExitLatency    : 8;    // Worst case latency of U1->U0, mks
    UINT32  Rsvd                : 8;    
    UINT32  U2DevExitLatency    : 16;   // Worst case latency of U2->U0, mks
} HCPARAMS3;

typedef struct {
    UINT32  Ac64        : 1;    // 64-bit Addressing Capability
    UINT32  Bnc         : 1;    // Bandwidth Negotiation Capability
    UINT32  Csz         : 1;    // Context data structures width (32 or 64 bit)
    UINT32  Ppc         : 1;    // Power Port Control
    UINT32  Pind        : 1;    // Port Indicators
    UINT32  Lhrc        : 1;    // Light HC Reset Capability
    UINT32  Ltc         : 1;    // Latency Tolerance Capability
    UINT32  Nss         : 1;    // No Secondary SID Support
    UINT32  Rsvd        : 4;
    UINT32  MaxPsaSize  : 4;    // Maximum Primary Stream Array Size
    UINT32  Xecp        : 16;   // xHCI Extended Capabilities Pointer
} HCPARAMS;

typedef struct {
    UINT8       CapLength;  // 00
    UINT8       Rsvd;       // 01
    UINT16      HciVersion; // 02
    HCPARAMS1   HcParams1;  // 04
    HCPARAMS2   HcParams2;  // 08
    HCPARAMS3   HcParams3;  // 0C
    HCPARAMS    HcParams;   // 10
    UINT32      DbOff;      // 14
    UINT32      RtsOff;     // 18
} XHCI_HC_CAP_REGS;

//-----------------------------------------------------------
// Host Controller Operational Registers
//-----------------------------------------------------------
typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
            #define XHCI_CMD_RS     BIT0
            #define XHCI_CMD_HCRST  BIT1
            #define XHCI_CMD_INTE   BIT2
            #define XHCI_CMD_HSEE   BIT3
            #define XHCI_CMD_LHCRST BIT4
        struct {
            UINT32      RunStop : 1;
            UINT32      HcRst   : 1;    // HC Reset
            UINT32      Inte    : 1;    // Interrupter Enable
            UINT32      HsEe    : 1;    // Host System Error Enable
            UINT32      Rsvd    : 3;
            UINT32      LhcRst  : 1;    // Light Host Controller Reset
            UINT32      Css     : 1;    // Controller Save State
            UINT32      Crs     : 1;    // Controller Restore State
            UINT32      Ewe     : 1;    // Enable Wrap Event
            UINT32      Eu3S    : 1;    // Enable U3 MFINDEX Stop
            UINT32      Rsvd1   : 20;
        } Field;
    };
} XHCI_USBCMD;

typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
            #define XHCI_STS_HALTED             BIT0
            #define XHCI_STS_HOSTSYSTEM_ERROR   BIT2
            #define XHCI_STS_EVT_INTERRUPT      BIT3
            #define XHCI_STS_PCD                BIT4
        struct {
            UINT32      HcHalted    : 1;
            UINT32      Rsvd1       : 1;
            UINT32      Hse         : 1;    // Host System Error
            UINT32      Eint        : 1;    // Event Interrupt
            UINT32      Pcd         : 1;    // Port Change Detect
            UINT32      Rsvd2       : 3;
            UINT32      Sss         : 1;    // Save State Status
            UINT32      Rss         : 1;    // Restore State Status
            UINT32      Sre         : 1;    // Save/Restore Error
            UINT32      Cnr         : 1;    // Controller Not Ready
            UINT32      Hce         : 1;    // Host Controller Error
            UINT32      Rsvd3       : 19;
        } Field;
    };
} XHCI_USBSTS;

typedef struct {
    UINT32      Rcs         : 1;    // Ring Cycle State
    UINT32      Cs          : 1;    // Command Stop
    UINT32      Ca          : 1;    // Command Abort
    UINT32      Crr         : 1;    // Command Ring Running
    UINT32      Rsvd        : 2;
    UINT32      CrPointer   : 26;   // Command Ring Pointer
} XHCI_CRCR;

#define XHCI_PORT_CONNECT   BIT0
#define XHCI_PORT_ENABLE    BIT1
#define XHCI_PORT_RESET     BIT4
#define XHCI_PORT_RESET_CHG BIT21

#define XHCI_PORTSC_OFFSET  0x400

// Port speed definitions as read from PortSpeed field of PORTSC
#define XHCI_DEVSPEED_UNDEFINED 0
#define XHCI_DEVSPEED_FULL      1
#define XHCI_DEVSPEED_LOW       2
#define XHCI_DEVSPEED_HIGH      3
#define XHCI_DEVSPEED_SUPER     4

typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
            #define XHCI_PCS_CCS    BIT0
            #define XHCI_PCS_PED    BIT1
            #define XHCI_PCS_OCA    BIT3
            #define XHCI_PCS_PR     BIT4
            #define XHCI_PCS_PP     BIT9
            #define XHCI_PCS_CSC    BIT17
            #define XHCI_PCS_PEC    BIT18
            #define XHCI_PCS_WRC    BIT19  //(EIP75547+)
            #define XHCI_PCS_OCC    BIT20
            #define XHCI_PCS_PRC    BIT21
            #define XHCI_PCS_WPR    BIT31  //(EIP75547+)
        struct {
            UINT32      Ccs         : 1;    // 0 Current Connect Status - RO
            UINT32      Ped         : 1;    // 1 Port Enabled/Disabled  - RW1CS
            UINT32      RsvdZ1      : 1;    // 2
            UINT32      Oca         : 1;    // 3 Over-current Active     - RO
            UINT32      Pr          : 1;    // 4 Port Reset              - RW1S
            UINT32      Pls         : 4;    // 5..8 Port Link State      - RWS
            UINT32      Pp          : 1;    // 9 Port Power              - RWS
            UINT32      PortSpeed   : 4;    // 10..13 Port Speed         - RO
            UINT32      Pic         : 2;    // 14..15 Port Indicator Ctl - RWS
            UINT32      Lws         : 1;    // 16 Port Link State Write Strobe - RW
            UINT32      Csc         : 1;    // 17 Connect Status Change  - RW1CS
            UINT32      Pec         : 1;    // 18 Port Enabled/Disabled Change - RW1CS
            UINT32      Wrc         : 1;    // 19 Warm Port Reset Change - RW1CS/RsvdZ
            UINT32      Occ         : 1;    // 20 Over-current Change    - RW1CS
            UINT32      Prc         : 1;    // 21 Port Reset Change      - RW1CS
            UINT32      Plc         : 1;    // 22 Port Link State Change - RW1CS
            UINT32      Cec         : 1;    // 23 Port Config Error Change  - RW1CS/RsvdZ
            UINT32      Cas      	: 1;    // 24 Cold Attach Status     - RO	//(EIP75547)
            UINT32      Wce         : 1;    // 25 Wake on Connect Enable      - RWS
            UINT32      Wde         : 1;    // 26 Wake on Disconnect Enable   - RWS
            UINT32      Woe         : 1;    // 27 Wake on Over-current Enable - RWS
            UINT32      RsvdZ2      : 2;    // 28..29	//(EIP75547)
            UINT32      Dr          : 1;    // 30 Device Removable (0 removable) - RO
            UINT32      Wpr         : 1;    // 31 Warm Port Reset - RW1S/RsvdZ
        } Field;
    };
} XHCI_PORTSC;

typedef struct {
    XHCI_USBCMD UsbCmd;     // 00
    XHCI_USBSTS UsbSts;     // 04
    UINT32      PageSize;   // 08
    UINT8       Rsvd1[8];   // 0C
    UINT32      DnCtrl;     // 14 Device Notification Control
    UINT64      Crcr;       // 18 Command Ring Control
    UINT8       Rsvd2[16];  // 20
    UINT64      DcbAap;     // 30 Device Context Base Address Array Pointer
    UINT32      Config;     // 38 Max Device Slots Enabled
} XHCI_HC_OP_REGS;

// Minimum inter-interrupt interval, in 250ns units. The value of 4000 makes 1ms interval.
#define XHCI_IMODI 4000

// Note: the following structure defines 32-bit and 64-bits fields 
// without detailing; this MMIO data must be accessed using Dword
// access for 32-bit fields and Qword access for 64-bit, Section 5.5.
typedef struct {
    UINT32  IMan;   // Interrupter Management
    UINT32  IMod;   // Interrupter Moderation
    UINT32  Erstz;  // Event Ring Segment Table Size
    UINT32  RsrvP;
    UINT64  Erstba; // Event Ring Segment Table Base Address
    UINT64  Erdp;   // Event Ring Dequeue Pointer
} XHCI_INTERRUPTER_REGS;

typedef struct {
    UINT32                  MfIndex;
    UINT32                  Reserved[7];
    XHCI_INTERRUPTER_REGS   IntRegs[1024];
} XHCI_HC_RT_REGS;

typedef enum {
    XhciTNormal = 1,
    XhciTSetupStage, // 2
    XhciTDataStage,  // 3
    XhciTStatusStage,// 4
    XhciTIsoch,      // 5
    XhciTLink,       // 6
    XhciTEventData,  // 7
    XhciTNoOp,       // 8
    XhciTEnableSlotCmd,  // 9
    XhciTDisableSlotCmd, // 10
    XhciTAddressDeviceCmd,   // 11
    XhciTConfigureEndpointCmd,   //12
    XhciTEvaluateContextCmd,     //13
    XhciTResetEndpointCmd,       //14
    XhciTStopEndpointCmd,        // 15
    XhciTSetTRDequeuePointerCmd, //16
    XhciTResetDeviceCmd, // 17
    XhciTForceEventCmd,  // 18
    XhciTNegotiateBandwidthCmd,  // 19
    XhciTSetLatencyToleranceCmd, // 20
    XhciTGetPortBandwidthCmd,    // 21
    XhciTForceHeaderCmd, // 22
    XhciTNoOpCmd,        // 23
// 24..31 reserved
    XhciTTransferEvt = 32,
    XhciTCmdCompleteEvt, // 33
    XhciTPortStatusChgEvt,   // 34
    XhciTBandwidthRequestEvt,    // 35
    XhciTDoorbellEvt,    // 36
    XhciTHostControllerEvt,  // 37
    XhciTDevNotificationEvt, // 38
    XhciTMfIndexWrapEvt  // 39
} TRB_TYPE;

typedef struct {
    UINT64      RsBase;
    UINT16      RsSize;
    UINT16      Rsvd1;
    UINT32      Rsvd2;
} XHCI_ER_SEGMENT_ENTRY;

#define CRCR_RING_CYCLE_STATE BIT0

#define TRBS_PER_SEGMENT    64
#define RING_SIZE TRBS_PER_SEGMENT*sizeof(XHCI_TRB)

typedef struct {
    UINT8   SlotId;
    UINT8   DevAddr;
} PEI_XHCI_SLOTADDR_MAP;

// Note that MaxSlots affects the size of USB3_CONTROLLER structure as
// well as its data boundaries and alignment. Default value is 32; if a
// different value needs to be assigned, make sure that boundaries and
// alignment meets the requirements; do the padding as needed.
// See XHCI specification Chapter 6 for the details.

#define PEI_XHCI_MAX_SLOTS  32

										//(EIP75547+)>
typedef struct {
	struct {
		UINT32		CapId:8;
		UINT32		NextCapPtr:8;
		UINT32		MinorRev:8;
		UINT32		MajorRev:8;
	};

	UINT32		NameString;

	struct {
		UINT32		PortOffset:8;
		UINT32		PortCount:8;
		UINT32		L1c:1;		// L1 Capability
		UINT32		Hso:1;		// High-speed Only
		UINT32		Ihi:1;		// Integrated Hub Implemented
		UINT32		RsvdZ:13;
	};
} XHCI_EXT_PROTOCOL;
										//<(EIP75547+)

typedef struct _USB3_CONTROLLER {
    UINTN               Dcbaa[PEI_XHCI_MAX_SLOTS];      // 32*4 = 128
    UINT8               XfrTrb[RING_SIZE*PEI_XHCI_MAX_SLOTS*32];
    UINT8               CommandRing[RING_SIZE];
    UINT8               EventRing[RING_SIZE];

    UINT8               DevCtx[PEI_XHCI_MAX_SLOTS * 0x800];
    UINT8               InpCtx[0x840];
    UINT8               ScratchpadBufArray[0x100];
    XHCI_ER_SEGMENT_ENTRY   Erst;

    UINT16              Vid;
    UINT16              Did;
    BOOLEAN             Access64;
    UINT16              HciVersion;
    UINT8               MaxPorts;
    UINT32              PageSize4K;
	UINT8				ContextSize;
    UINT16              MaxIntrs;
    UINT32              DbOffset;
    UINT8               MaxSlots;
	VOID				*InputContext;
    TRB_RING            CmdRing;
    TRB_RING            EvtRing;
    TRB_RING            XfrRing[PEI_XHCI_MAX_SLOTS*32]; // 32 endpoints per device
    TRB_RING            *XfrRings;
    UINTN               XfrTrbs;
	VOID				*DeviceContext;
    XHCI_DCBAA          *DcbaaPtr;
    XHCI_HC_CAP_REGS    *CapRegs;
    XHCI_HC_OP_REGS     *OpRegs;
    XHCI_HC_RT_REGS     *RtRegs;
    XHCI_EXT_PROTOCOL   *Usb2Protocol;	//(EIP75547+)
    XHCI_EXT_PROTOCOL   *Usb3Protocol;	//(EIP75547+)

    PEI_XHCI_SLOTADDR_MAP DeviceMap[PEI_XHCI_MAX_SLOTS];

    EFI_PEI_SERVICES    **PeiServices;
    PEI_USB_HOST_CONTROLLER_PPI UsbHostControllerPpi;
    EFI_PEI_PPI_DESCRIPTOR      PpiDescriptor;
    EFI_PEI_CPU_IO_PPI  *CpuIoPpi;
    EFI_PEI_STALL_PPI   *StallPpi;
    EFI_PEI_PCI_CFG2_PPI *PciCfgPpi;

} USB3_CONTROLLER;

#pragma pack(pop)

//----------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------
EFI_STATUS
XhciHcControlTransfer (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT8                       DeviceAddress,
    IN  UINT8                       DeviceSpeed,
    IN  UINT8                       MaximumPacketLength,
    IN  UINT16                      TransactionTranslator    OPTIONAL,
    IN  EFI_USB_DEVICE_REQUEST      *Request,
    IN  EFI_USB_DATA_DIRECTION      TransferDirection,
    IN  OUT VOID *Data              OPTIONAL,
    IN  OUT UINTN *DataLength       OPTIONAL,
    IN  UINTN                       TimeOut,
    OUT UINT32                      *TransferResult
);

EFI_STATUS
XhciHcBulkTransfer (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT8                       DeviceAddress,
    IN  UINT8                       EndPointAddress,
    IN  UINT8                       DeviceSpeed,
    IN  UINT16                      MaximumPacketLength,
    IN  UINT16                      TransactionTranslator    OPTIONAL,
    IN  OUT VOID                    *Data,
    IN  OUT UINTN                   *DataLength,
    IN  OUT UINT8                   *DataToggle,
    IN  UINTN                       TimeOut,
    OUT UINT32                      *TransferResult
);

EFI_STATUS
XhciHcGetRootHubPortNumber (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
);

EFI_STATUS
XhciHcGetRootHubPortStatus (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS         *PortStatus
);

EFI_STATUS
XhciHcSetRootHubPortFeature (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT8                       PortNumber,
    IN  EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS
XhciHcClearRootHubPortFeature (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT8                       PortNumber,
    IN  EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS 
XhciHcReset (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT16                      Attributes
);

EFI_STATUS
XhciInitHC (
    EFI_PEI_SERVICES            **PeiServices,
    struct _USB3_CONTROLLER     *Usb3Hc,
    UINT8                       ControllerIndex
);

EFI_STATUS
XhciHcPreConfigureDevice( 
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT8                       Port,
    IN  UINT8                       Speed,
    IN  UINT16                      TransactionTranslator
);

EFI_STATUS
XhciEnableEndpoints (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This,
    IN  UINT8                       *Desc
);

VOID
XhciMmio64Write (
    USB3_CONTROLLER *Usb3Hc,
    UINTN           Address,
    UINT64          Data
);

EFI_STATUS
XhciInitRing (
    TRB_RING*,
    UINTN,
    UINT32,
    BOOLEAN
);

EFI_STATUS
XhciExecuteCommand (
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    TRB_TYPE            Cmd,
    VOID                *Params
);

TRB_RING*
XhciInitXfrRing (
    USB3_CONTROLLER*    Usb3Hc,
    UINT8               Slot,
    UINT8               Ep
);

EFI_STATUS
UpdateDeviceMap (
    PEI_XHCI_SLOTADDR_MAP   *DeviceMap,
    UINT8                   SlotId,
    UINT8                   DevAddr
);

EFI_STATUS
GetSlotId (
    PEI_XHCI_SLOTADDR_MAP   *DeviceMap,
    UINT8                   *SlotId,
    UINT8                   DevAddr
);

EFI_STATUS
XhciInitDeviceData (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_USB_HOST_CONTROLLER_PPI *This
//    HC_STRUC    *HcStruc,
//    DEV_INFO    *DevInfo,
//    UINT8       PortStatus,
//    UINT8       **DeviceData
);

TRB_RING*
XhciGetXfrRing (
    USB3_CONTROLLER*    Usb3Hc,
    UINT8               Slot,
    UINT8               Ep
);

UINT32*
XhciGetTheDoorbell (
    USB3_CONTROLLER *Usb3Hc,
    UINT8           SlotId
);

XHCI_TRB*
    XhciAdvanceEnqueuePtr (
    TRB_RING    *Ring
);

EFI_STATUS
XhciWaitForEvent (
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    XHCI_TRB            *TrbToCheck,
    TRB_TYPE            EventType,
    UINT8               *CompletionCode,
    UINT16              TimeOutMs,
    VOID                *Data
);

UINT8*
XhciGetDeviceContext (
    USB3_CONTROLLER *Usb3Hc,
    UINT8           SlotId
);

UINT8*
XhciGetContextEntry (
    USB3_CONTROLLER *Usb3Hc,
    VOID            *Context,
    UINT8           Index
);

                                //(EIP75547+)>
VOID
XhciResetUsb2Port (
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc
);

EFI_STATUS
XhciExtCapParser (
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc
);
                                //<(EIP75547+)

/*
#define _CR( Record, TYPE, Field ) \
    ( (TYPE *) ( (CHAR8 *) (Record) - (CHAR8 *) &( ( (TYPE *) 0 )->Field ) ) )
#define PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS( a ) \
    _CR( a, USB3_CONTROLLER, UsbHostControllerPpi )
*/

#endif  // _XHCIPEI_H

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
