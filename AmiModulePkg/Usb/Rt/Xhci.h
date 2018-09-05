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

/** @file Xhci.h
    XHCI equates and structure definitions

**/

#ifndef _XHCI_H_
#define _XHCI_H_

#include <Token.h>
#include "UsbKbd.h"

#pragma pack(push, 1)

// XHCI Device Context structures
//---------------------------------------------------------------------------

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
} HCSPARAMS1;

typedef struct {
    UINT32  Ist         : 4;            // Isochronous Scheduling Threshold
    UINT32  ErstMax     : 4;            // Event Ring Segment Table Max
    UINT32  Rsvd        : 13;
    UINT32  MaxScratchPadBufsHi   : 5;  // Max Scratchpad Buffers (Max Scratchpad Bufs Hi).
    UINT32  Spr         : 1;            // Scratchpad restore
    UINT32  MaxScratchPadBufsLo   : 5;  // Max Scratchpad Buffers (Max Scratchpad Bufs Lo).
} HCSPARAMS2;

typedef struct {
    UINT32  U1DevExitLatency    : 8;    // Worst case latency of U1->U0, mks
    UINT32  Rsvd                : 8;    
    UINT32  U2DevExitLatency    : 16;   // Worst case latency of U2->U0, mks
} HCSPARAMS3;

typedef struct {
    UINT32  Ac64        : 1;    // 64-bit Addressing Capability
    UINT32  Bnc         : 1;    // Bandwidth Negotiation Capability
    UINT32  Csz         : 1;    // Context data structures width (32 or 64 bit)
    UINT32  Ppc         : 1;    // Power Port Control
    UINT32  Pind        : 1;    // Port Indicators
    UINT32  Lhrc        : 1;    // Light HC Reset Capability
    UINT32  Ltc         : 1;    // Latency Tolerance Capability
    UINT32  Nss         : 1;    // No Secondary SID Support
    UINT32  Pae         : 1;    // Parse All Event Data
    UINT32  Spc         : 1;    // Stopped - Short Packet Capability
    UINT32  Sec         : 1;    // Stopped EDTLA Capability
    UINT32  Cfc         : 1;    // Contiguous Frame ID Capability
    UINT32  MaxPsaSize  : 4;    // Maximum Primary Stream Array Size
    UINT32  Xecp        : 16;   // xHCI Extended Capabilities Pointer
} HCCPARAMS1;

typedef struct {
    UINT32  U3c         : 1;    // U3 Entry Capability
    UINT32  Cmc         : 1;    // Configure Endpoint Command Max Exit Latency Too Large Capability
    UINT32  Fsc         : 1;    // Force Save Context Capability
    UINT32  Ctc         : 1;    // Compliance Transition Capability
    UINT32  Lec         : 1;    // Large ESIT Payload Capability
    UINT32  Cic         : 1;    // Configuration Information Capability
    UINT32  Rsvd        : 26;
} HCCPARAMS2;

typedef struct {
    UINT8           CapLength;      // 00
    UINT8           Rsvd;           // 01
    UINT16          HciVersion;     // 02
    HCSPARAMS1      HcsParams1;     // 04
    HCSPARAMS2      HcsParams2;     // 08
    HCSPARAMS3      HcsParams3;     // 0C
    HCCPARAMS1      HccParams1;      // 10
    UINT32          DbOff;          // 14
    UINT32          RtsOff;         // 18
    HCCPARAMS2      HccParams2;     // 1C
} XHCI_HC_CAP_REGS;

//
// Capability registers offset
//
#define XHCI_CAPLENGTH_OFFSET               0x00 // Capability Register length
#define XHCI_HCIVERSION_OFFSET              0x02 // Interface Version Number
#define XHCI_HCSPARAMS1_OFFSET              0x04 // Structural Parameters 1
#define XHCI_HCSPARAMS2_OFFSET              0x08 // Structural Parameters 2
#define XHCI_HCSPARAMS3_OFFSET              0x0c // Structural Parameters 3
#define XHCI_HCCPARAMS1_OFFSET              0x10 // Capability Parameters 1
#define XHCI_DBOFF_OFFSET                   0x14 // Doorbell Offset
#define XHCI_RTSOFF_OFFSET                  0x18 // Runtime Register Space Offset
#define XHCI_HCCPARAMS2_OFFSET              0x1C // Capability Parameters 2

//-----------------------------------------------------------
// Host Controller Operational Registers
//-----------------------------------------------------------
typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
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

#define XHCI_CMD_RS         BIT0
#define XHCI_CMD_HCRST      BIT1
#define XHCI_CMD_INTE       BIT2
#define XHCI_CMD_HSEE       BIT3
#define XHCI_CMD_LHCRST     BIT4

typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
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

#define XHCI_STS_HALTED                 BIT0
#define XHCI_STS_HOSTSYSTEM_ERROR       BIT2
#define XHCI_STS_EVT_INTERRUPT          BIT3
#define XHCI_STS_PCD                    BIT4
#define XHCI_STS_SSS                    BIT8
#define XHCI_STS_RSS                    BIT9
#define XHCI_STS_SRE                    BIT10
#define XHCI_STS_CNR                    BIT11
#define XHCI_STS_HCE                    BIT12

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


// Port speed definitions as read from PortSpeed field of PORTSC
#define XHCI_DEVSPEED_UNDEFINED         0
#define XHCI_DEVSPEED_FULL              1
#define XHCI_DEVSPEED_LOW               2
#define XHCI_DEVSPEED_HIGH              3
#define XHCI_DEVSPEED_SUPER             4
#define XHCI_DEVSPEED_SUPER_PLUS        5

// Port link definitions
#define	XHCI_PORT_LINK_U0				0
#define	XHCI_PORT_LINK_U1				1
#define	XHCI_PORT_LINK_U2				2
#define	XHCI_PORT_LINK_U3				3
#define	XHCI_PORT_LINK_DISABLED			4
#define	XHCI_PORT_LINK_RXDETECT			5
#define	XHCI_PORT_LINK_INACTIVE			6
#define	XHCI_PORT_LINK_POLLING			7
#define	XHCI_PORT_LINK_RECOVERY			8
#define	XHCI_PORT_LINK_HOT_RESET		9
#define	XHCI_PORT_LINK_COMPLIANCE_MODE	10
#define	XHCI_PORT_LINK_TEST_MODE		11
#define	XHCI_PORT_LINK_RESUME			15

typedef struct {
    union {
        UINT32  AllBits;    // can be used for clearing status
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
            UINT32      Cas      	: 1;    // 24 Cold Attach Status     - RO
            UINT32      Wce         : 1;    // 25 Wake on Connect Enable      - RWS
            UINT32      Wde         : 1;    // 26 Wake on Disconnect Enable   - RWS
            UINT32      Woe         : 1;    // 27 Wake on Over-current Enable - RWS
            UINT32      RsvdZ2      : 2;    // 28..29
            UINT32      Dr          : 1;    // 30 Device Removable (0 removable) - RO
            UINT32      Wpr         : 1;    // 31 Warm Port Reset - RW1S/RsvdZ
        } Field;
    };
} XHCI_PORTSC;

#define XHCI_PCS_CCS    BIT0
#define XHCI_PCS_PED    BIT1
#define XHCI_PCS_OCA    BIT3
#define XHCI_PCS_PR     BIT4
#define XHCI_PCS_PP     BIT9
#define XHCI_PCS_LWS    BIT16
#define XHCI_PCS_CSC    BIT17
#define XHCI_PCS_WRC    BIT19
#define XHCI_PCS_PRC    BIT21
#define XHCI_PCS_WPR    BIT31

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

//
// Operational registers offset
//
#define XHCI_USBCMD_OFFSET                  0x0000 // USB Command Register Offset
#define XHCI_USBSTS_OFFSET                  0x0004 // USB Status Register Offset
#define XHCI_PAGESIZE_OFFSET                0x0008 // USB Page Size Register Offset
#define XHCI_DNCTRL_OFFSET                  0x0014 // Device Notification Control Register Offset
#define XHCI_CRCR_OFFSET                    0x0018 // Command Ring Control Register Offset
#define XHCI_DCBAAP_OFFSET                  0x0030 // Device Context Base Address Array Pointer Register Offset
#define XHCI_CONFIG_OFFSET                  0x0038 // Configure Register Offset
#define XHCI_PORTSC_OFFSET                  0x0400 // Port Status and Control Register Offset


#define CRCR_RING_CYCLE_STATE	BIT0
#define	CRCR_COMMAND_STOP		BIT1
#define	CRCR_COMMAND_ABORT		BIT2
#define CRCR_COMMAND_RUNNING	BIT3

// 6.5
typedef struct {
    UINT64      RsBase;
    UINT16      RsSize;
    UINT16      Rsvd1;
    UINT32      Rsvd2;
} XHCI_ER_SEGMENT_ENTRY;

// Interrupt Moderation Interval (5.5.2.2)
// Minimum inter-interrupt interval, in 250ns units. The value of 4000 makes 1ms interval.
#define XHCI_IMODI 4000
#define XHCI_KEYREPEAT_IMODI        REPEAT_INTERVAL * 4000
#define XHCI_KEYREPEAT_IMODC        REPEAT_INTERVAL * 4000

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

//---------------------------------------------------------
// Slot context definitions, Section 6.2.2
//---------------------------------------------------------
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


//---------------------------------------------------------
// Endpoint Context context definitions, Section 6.2.3
//---------------------------------------------------------

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
    UINT32  EpState             : 3;
    UINT32  RsvdZ1              : 5;
    UINT32  Mult                : 2;
    UINT32  MaxPStreams         : 5;
    UINT32  Lsa                 : 1;
    UINT32  Interval            : 8;
    UINT32  MaxEsitPayloadHi    : 8;
    UINT32  RzvdZ2              : 1;
    UINT32  ErrorCount          : 2;
    UINT32  EpType              : 3;
    UINT32  RsvdZ               : 1;
    UINT32  Hid                 : 1;
    UINT32  MaxBurstSize        : 8;
    UINT32  MaxPacketSize       : 16;

    UINT64  TrDequeuePtr;   // BIT0 of this field is DCS (Dequeue Cycle State)
    UINT16  AvgTrbLength;
    UINT16  MaxEsitPayloadLo;
    UINT32  RsvdO[3];
} XHCI_EP_CONTEXT;

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
typedef struct {
    XHCI_SLOT_CONTEXT   Slot;
    XHCI_EP_CONTEXT     Ep[31];
} XHCI_DEVICE_CONTEXT;

#define	XHCI_DEVICE_CONTEXT_ENTRIES	32

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
    UINT64  InpCtxAddress;
    UINT32  RsvdZ1;
    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 8;
    UINT32  Dc          : 1;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ3      : 8;
    UINT32  SlotId      : 8;
} XHCI_CONFIGURE_EP_CMD_TRB;

typedef struct {
    UINT64  InpCtxAddress;
    UINT32  RsvdZ1;
    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 9;
    UINT32  TrbType     : 6;
    UINT32  RsvdZ3      : 8;
    UINT32  SlotId      : 8;
} XHCI_EVALUATE_CONTEXT_CMD_TRB;

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
										//(EIP54300+)>
typedef struct {
    UINT32  RsvdZ1[3];

    UINT32  CycleBit    : 1;
    UINT32  RsvdZ2      : 9;
    UINT32  TrbType     : 6;
    UINT32  EndpointId  : 5;
    UINT32  RsvdZ3      : 2;
    UINT32  Suspend     : 1;
    UINT32  SlotId      : 8;
} XHCI_STOP_EP_CMD_TRB;
										//<(EIP54300+)
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
    UINT32  Tbc         : 2;
    UINT32  Bei         : 1;
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
#define EVT_TRBS_PER_SEGMENT    1024
#define ISOC_TRBS_PER_SEGMENT    8192
#define RING_SIZE TRBS_PER_SEGMENT*sizeof(XHCI_TRB)
#define XHCI_TRB_MAX_XFER_SIZE 0x10000

// Default timeouts
#ifndef XHCI_CMD_COMPLETE_TIMEOUT_MS
#define XHCI_CMD_COMPLETE_TIMEOUT_MS 50
#endif
#ifndef XHCI_ADDR_CMD_COMPLETE_TIMEOUT_MS
#define XHCI_ADDR_CMD_COMPLETE_TIMEOUT_MS 2000
#endif
#ifndef XHCI_CTL_COMPLETE_TIMEOUT_MS
#define XHCI_CTL_COMPLETE_TIMEOUT_MS 2000
#endif
#ifndef XHCI_INT_COMPLETE_TIMEOUT_MS
#define XHCI_INT_COMPLETE_TIMEOUT_MS 1500
#endif

#ifndef XHCI_RESET_DELAY_MS
#define XHCI_RESET_DELAY_MS         1
#endif

#ifndef XHCI_HALT_TIMEOUT_MS
#define XHCI_HALT_TIMEOUT_MS        32
#endif

#ifndef XHCI_RESET_PORT_DELAY_MS
#define XHCI_RESET_PORT_DELAY_MS    10
#endif

#ifndef XHCI_RESET_EP_DELAY_MS
#define XHCI_RESET_EP_DELAY_MS      10
#endif

//#define XHCI_BOT_TD_MAXSIZE 512
#define XHCI_BOT_TD_MAXSIZE 0x10000
#define XHCI_BOT_MAX_XFR_SIZE XHCI_BOT_TD_MAXSIZE*8

#ifndef XHCI_SWITCH2SS_DELAY_MS
#define XHCI_SWITCH2SS_DELAY_MS 5
#endif

#ifndef XHCI_MAX_PENDING_INTERRUPT_TRANSFER
#define XHCI_MAX_PENDING_INTERRUPT_TRANSFER 16
#endif

//---------------------------------------------------------
// Input context definition
//---------------------------------------------------------
typedef struct {
    UINT32  DropContextFlags;
    UINT32  AddContextFlags;
    UINT32  RzvdZ[6];
} XHCI_INPUT_CONTROL_CONTEXT;

typedef struct {
    XHCI_INPUT_CONTROL_CONTEXT  CtlCtx;
    XHCI_DEVICE_CONTEXT         DevCtx;
} XHCI_INPUT_CONTEXT;

#define	XHCI_INPUT_CONTEXT_ENTRIES	33

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

#define XHCI_LEGACY_CTRL_STS_REG        4

#define XHCI_BIOS_OWNED_SEMAPHORE		BIT16
#define XHCI_OS_OWNED_SEMAPHORE			BIT24

typedef struct {
	UINT32		CapId:8;
	UINT32		NextCapPtr:8;
	UINT32		HcBiosOwned:1;
	UINT32		RsvdP1:7;
	UINT32		HcOsOwned:1;
	UINT32		RsvdP2:7;
} XHCI_LEGSUP;

#define XHCI_SMI_ENABLE                         BIT0
#define XHCI_SMI_HOST_ERROR_ENABLE              BIT4
#define XHCI_SMI_OWNERSHIP_CHANGE_ENABLE		BIT13
#define XHCI_SMI_PCI_CMD_ENABLE                 BIT14
#define XHCI_SMI_PCI_BAR_ENABLE                 BIT15
#define XHCI_SMI_EVENT_INT                      BIT16
#define XHCI_SMI_HOST_ERROR                     BIT20
#define XHCI_SMI_OWNERSHIP_CHANGE               BIT29
#define XHCI_SMI_PCI_CMD                        BIT30
#define XHCI_SMI_PCI_BAR                        BIT31

typedef union {
	UINT32		AllBits;
	struct {
	    UINT32      UsbSmiEnable:1;
	    UINT32      RsvdP1:3;
	    UINT32      UsbHostErrorSmiEnable:1;
	    UINT32      RsvdP2:8;
	    UINT32      UsbOwnershipChangeSmiEnable:1;
	    UINT32      UsbPciCmdSmiEnable:1;
	    UINT32      UsbPciBarSmiEnable:1;
	    UINT32      UsbEventInterruptSmi:1;
	    UINT32      RsvdP3:3;
	    UINT32      UsbHostErrorSmi:1;
	    UINT32      RsvdP4:8;
	    UINT32      UsbOwnershipChangeSmi:1;
	    UINT32      UsbPciCmdSmi:1;
	    UINT32      UsbPciBarSmi:1;
	};
} XHCI_LEGCTLSTS;

typedef struct {
	XHCI_LEGSUP		LegSup;
	XHCI_LEGCTLSTS	LegCtlSts;
} XHCI_EXT_LEG_CAP;

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

#define XHCI_DB_CAP_DCCTRL_REG      0x20

#define XHCI_DB_CAP_DCE         BIT31

#pragma pack(pop)

										//(EIP60460)>
#define XHCI_FL100X_VID 0x1b73
#define XHCI_FL1000_DID 0x1000
#define XHCI_FL1009_DID 0x1009
										//<(EIP60460)
										//(EIP58979+)>
#define	XHCI_TUSB73X0_VID	0x104C
#define	XHCI_TUSB73X0_DID	0x8241
										//<(EIP58979+)
										//(EIP60327+)>
#define XHCI_VL800_VID	0x1106
#define XHCI_VL800_DID	0x3432
										//<(EIP60327+)
#define XHCI_EJ168A_VID	0x1B6F
#define XHCI_EJ168A_DID	0x7023

#define XHCI_AMD_SB900_VID	0x1022
#define XHCI_AMD_SB900_DID	0x7812

#define XHCI_NEC_VID 0x1033
#define XHCI_NEC_200_DID 0x0194

#define XHCI_INTEL_VID  0x8086

typedef struct _XHCI_PENDING_POLLING{
    XHCI_NORMAL_XFR_TRB         *Trb;
    UINT16                       TransferredLength;
}XHCI_PENDING_INTERRUPT_TRANSFER;

typedef struct _USB3_HOST_CONTROLLER {
    EFI_HANDLE              Controller;
    XHCI_HC_CAP_REGS        CapRegs;
    XHCI_HC_OP_REGS         *OpRegs;
    XHCI_HC_RT_REGS         *RtRegs;
	UINT32                  UsbLegSupOffSet;
	XHCI_EXT_PROTOCOL		Usb2Protocol;
	XHCI_EXT_PROTOCOL		Usb3Protocol;
    XHCI_EXT_PROTOCOL		Usb31Protocol;
    UINT32                  DbCapOffset;
    UINT16                  Vid;
    UINT16                  Did;
    EFI_USB_HC_STATE        HcState;
    UINT32                  PageSize4K;
    UINT8                   SBRN;
	UINT8					ContextSize;
    XHCI_DCBAA              *DcbaaPtr;
    TRB_RING                CmdRing;
    TRB_RING                EvtRing;
    TRB_RING                *XfrRings;
    UINTN                   XfrTrbs;
    VOID					*DeviceContext;
    VOID					*InputContext;
    UINT64                  *ScratchBufEntry;
    XHCI_PENDING_INTERRUPT_TRANSFER PendingInterruptTransfer[XHCI_MAX_PENDING_INTERRUPT_TRANSFER];
} USB3_HOST_CONTROLLER;

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
