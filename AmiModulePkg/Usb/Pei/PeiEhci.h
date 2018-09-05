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

/** @file PeiEhci.h
    This file is the main header file for the EHCI PEI USB
    recovery module. It contains generic constant and type
    declarations/definitions.

**/

#ifndef _EHCIPEI_H
#define _EHCIPEI_H

#include <Ppi/UsbPeiPpi.h>
#include <Token.h>

//----------------------------------------------------------------------------
// EHCI_FIXED_DELAY_MS( EhciDevPtr, milliseconds )
//
// invokes a delay specified in milliseconds using the PEI_EHCI_DEV.StallPpi
// member interface pointer, which is initialized from a EFI_PEI_STALL_PPI
// interface.
#define EHCI_FIXED_DELAY_MS( EhciDevPtr, milliseconds ) \
    EhciDevPtr->StallPpi->Stall( EhciDevPtr->PeiServices, \
    EhciDevPtr->StallPpi, milliseconds * 1000 );

#define EHCI_FIXED_DELAY_US( EhciDevPtr, microseconds ) \
    EhciDevPtr->StallPpi->Stall( EhciDevPtr->PeiServices, \
    EhciDevPtr->StallPpi, microseconds);

#ifndef PEI_EHCI_RESET_TIMEOUT_MS
#define PEI_EHCI_RESET_TIMEOUT_MS       50
#endif

#ifndef PEI_EHCI_RELEASE_PORT_TIMEOUT_MS
#define PEI_EHCI_RELEASE_PORT_TIMEOUT_MS        20
#endif

#ifndef PEI_EHCI_RESET_PORT_TIMEOUT_MS
#define PEI_EHCI_RESET_PORT_TIMEOUT_MS        10
#endif

//----------------------------------------------------------------------------
// PEI_RECOVERY_USB_EHCI_DEV_FROM_THIS( a )
//
// uses the _CR macro to obtain a pointer to a PEI_EHCI_DEV structure from 
// a PEI_USB_HOST_CONTROLLER_PPI interface pointer
#define _CR( Record, TYPE, Field ) \
    ( (TYPE *) ( (CHAR8 *) (Record) - (CHAR8 *) &( ( (TYPE *) 0 )->Field ) ) )
#define PEI_RECOVERY_USB_EHCI_DEV_FROM_THIS( a ) \
    _CR( a, PEI_EHCI_DEV, UsbHostControllerPpi )


//----------------------------------------------------------------------------
// Constants
//
// Notes:
//      1)  These constants are not associated with a particular structure
//          and are used globally withing EHCI-specific code
//----------------------------------------------------------------------------



// Standard EHCI PCI configuration space registers
#define EHCI_BASE_ADDR_REG         0x10     // EHCI BAR register
#define EHCI_CMD_REGISTER          0x04     // EHCI command register
#define DEBUG_BASE                 0x5A     // DEBUG_BASE offset address 

// EHCI controller status definitions
#define EHCI_RESET_ERROR    BIT0
#define EHCI_VERCAPLENGTH   0   // Interface Version and Capability
#define EHCI_USBCMD_REG         0   // USB Command

// Bit definition for queue transfer descriptor token fields
//-------------------------------------------------------------------------
#define QTD_DATA_TOGGLE         0x80000000  // BIT 31
    #define QTD_SETUP_TOGGLE    0x00000000
    #define QTD_DATA0_TOGGLE    0x00000000
    #define QTD_DATA1_TOGGLE    0x80000000
    #define QTD_STATUS_TOGGLE   0x80000000
#define QTD_XFER_DATA_SIZE      0x7FFF0000  // BIT 30:16
#define QTD_IOC_BIT             0x00008000  // BIT 15
#define QTD_ERROR_COUNT         0x00000C00
    #define QTD_NO_ERRORS       0x00000000
    #define QTD_ONE_ERROR       0x00000400
    #define QTD_TWO_ERRORS      0x00000800
    #define QTD_THREE_ERRORS    0x00000C00
#define QTD_DIRECTION_PID       0x00000300
    #define QTD_OUT_TOKEN       0x00000000
    #define QTD_IN_TOKEN        0x00000100
    #define QTD_SETUP_TOKEN     0x00000200
#define QTD_STATUS_FIELD        0x000000FF
    #define QTD_ACTIVE          0x00000080
    #define QTD_HALTED          0x00000040
    #define QTD_BUFFER_ERROR    0x00000020
    #define QTD_BABBLE          0x00000010
    #define QTD_XACT_ERROR      0x00000008
    #define QTD_MISSED_UFRAME   0x00000004
    #define QTD_SPLIT_XSTATE    0x00000002
      #define   QTD_START_SPLIT 0x00000000
      #define   QTD_COMPLETE_SPLIT  0x00000002
    #define QTD_SPLIT_ERROR     0x00000001
    #define QTD_PING_STATE      0x00000001
      #define   QTD_DO_OUT      0x00000000
      #define   QTD_DO_PING     0x00000001
//-------------------------------------------------------------------------

#define QH_I_BIT                0x00000080  // BIT 7
#define QH_ENDPOINT_SPEED       0x00003000  // BIT 13:12
    #define QH_FULL_SPEED       0x00000000
    #define QH_LOW_SPEED        0x00001000
    #define QH_HIGH_SPEED       0x00002000
#define QH_DATA_TOGGLE_CONTROL  0x00004000  // BIT 14
    #define QH_IGNORE_QTD_DT    0x00000000
    #define QH_USE_QTD_DT       0x00004000
#define QH_HEAD_OF_LIST         0x00008000  // BIT 15
#define QH_CONTROL_ENDPOINT     0x08000000  // BIT 27
#define QH_DATA_TOGGLE          0x80000000  // BIT 31

#define QH_MULT_SETTING         0xC0000000  // BIT 31:30
    #define QH_ONE_XFER         0x40000000
    #define QH_TWO_XFER         0x80000000
    #define QH_THREE_XFER       0xC0000000

#define EHCI_QUEUE_HEAD     2   // Queue head id


#define EHCI_TERMINATE      1

//----------------------------------------------------------------------------
// Structure types
//
// Notes:
//      1)  Constants that are associated with a particular structure
//          are defined locally within the structure to facilitate easier
//          maintenance.
//----------------------------------------------------------------------------

#pragma pack(push,1)


//
// Notes:
//  To ensure proper updating, follow these guidelines:
//
//                  1)  Write all bits using the AllBits union element
//                      (OR bit-constants if modifying multiple fields).
//
//                          * Writing individual bits may have adverse 
//                            effects.
//
//                  2)  Typically you will read individual bits using the  
//                      Field union element.
//
//                  3)  Writing a zero to any field in this register has
//                      no effect.
//

typedef struct _EHCI_HCS_PARAMS
{
    union {
        UINT32  AllBits;
            #define EHCI_HCSP_N_PORTS   (BIT0+BIT1+BIT2+BIT3)
            #define EHCI_HCSP_PPC       BIT4
            #define EHCI_HCSP_PRR       BIT7
            #define EHCI_HCSP_N_PCC     (BIT8+BIT9+BIT10+BIT11)
            #define EHCI_N_CC           (BIT12+BIT13+BIT14+BIT15)
            #define EHCI_P_INDICATOR    BIT16
            #define EHCI_DEBUG_N        (BIT20+BIT21+BIT22+BIT23)
        struct
        {
            UINT32  NumberOfPorts       : 4;    // [3:0]
            UINT32  PortPowerControl    : 1;    // [4]
            UINT32  Reserved1           : 2;    // [6:5]
            UINT32  PortRoutingRules    : 1;    // [7]
            UINT32  PortPerCompanion    : 4;    // [11:8]
            UINT32  NumberOfCompanions  : 4;    // [15:12]
            UINT32  PortIndicators      : 1;    // [16]
            UINT32  Reserved2           : 3;    // [19:17]
            UINT32  DebugPortNumber     : 4;    // [23:20]
            UINT32  Reserved3           : 8;    // [31:24]
        } Field;
    };
} EHCI_HCS_PARAMS;

typedef struct _EHCI_HCC_PARAMS
{
    union {
        UINT32  AllBits;
            #define EHCI_64BIT_CAP  0x01    // 64-bit addressing capability
            #define EHCI_PFLFLAG    0x02    // Programmable Frame List Flag
            #define EHCI_ASP_CAP    0x04    // Asynchronous Schedule  Park Capability
            #define EHCI_IST        0xF0    // Isochronous Scheduling Threshold
            #define EHCI_EECP       0xFF00  // EHCI Extended Capabilities Pointer
        struct
        {
            UINT32  AddressingCap64     : 1;    // [0]
            UINT32  FrameListFlag       : 1;    // [1]
            UINT32  AsyncShedParkCap    : 1;    // [2]
            UINT32  Reserved1           : 1;    // [3]
            UINT32  IsochShedThreshold  : 4;    // [7:4]
            UINT32  ExtCapPointer       : 8;    // [15:8]
            UINT32  Reserved2           : 16;   // [31:8]
        } Field;
    };
} EHCI_HCC_PARAMS;

typedef struct _EHCI_HC_CAP_REG {
    UINT8   CapLength;
    UINT8   Reserved;
    UINT16  HciVersion;
    EHCI_HCS_PARAMS  HcsParams;
    EHCI_HCC_PARAMS  HccParams;
    UINT8   HcspPortRoute[15];
} EHCI_HC_CAP_REG;


typedef struct _EHCI_USBCMD
{
    union {
        UINT32  AllBits;
            #define EHCI_RUNSTOP    BIT0
            #define EHCI_HCRESET    BIT1
            #define EHCI_FRM1024    0       // Reset BIT2 and BIT3 before
            #define EHCI_FRM512     BIT2    // setting the new Frame List Size
            #define EHCI_FRM256     BIT3
            #define EHCI_PER_SCHED_ENABLE           BIT4
            #define EHCI_ASYNC_SCHED_ENABLE         BIT5
            #define EHCI_INT_ASYNC_ADVANCE_ENABLE   BIT6
            #define EHCI_INTTHRESHOLD   (8 << 16)
        struct
        {
            UINT32  RunStop                 : 1;    // [0]
            UINT32  HcReset                 : 1;    // [1]
            UINT32  FrameListSize           : 2;    // [3:2]
            UINT32  PeriodicSheduleEnable   : 1;    // [4]
            UINT32  AsyncSheduleEnable      : 1;    // [5]
            UINT32  IntAsyncAdvDorbell      : 1;    // [6]
            UINT32  LightHcReset            : 1;    // [7]
            UINT32  Reserved1               : 8;    // [16:8]
            UINT32  IntThresholdCtl         : 8;    // [23:16]
            UINT32  Reserved2               : 8;    // [31:24]
        } Field;
    };
} EHCI_USBCMD;

typedef struct _EHCI_USBSTS
{
    union {
        UINT32  AllBits;
            #define EHCI_USB_INTERRUPT          BIT0    // Interrupt
            #define EHCI_USB_ERROR_INTERRUPT    BIT1    // Error interrupt
            #define EHCI_PORT_CHANGE_DETECT     BIT2    // Port Change Detect
            #define EHCI_FRAME_LIST_ROLLOVER    BIT3    // Frame List Rollover
            #define EHCI_HOST_SYSTEM_ERROR      BIT4    // Host System Error
            #define EHCI_INT_ASYNC_ADVANCE      BIT5    // Interrupt on Async Advance
            #define EHCI_HCHALTED               BIT12   // HCHalted
            #define EHCI_RECLAIM                BIT13   // Reclamation
            #define EHCI_PER_SCHED_STATUS       BIT14   // Periodic Schedule Status
            #define EHCI_ASYNC_SCHED_STATUS     BIT15   // Asynchronous Schedule Status
        struct
        {
            UINT32  UsbInt                  : 1;    // [0]
            UINT32  UsbErrInt               : 1;    // [1]
            UINT32  PortChangeDetect        : 1;    // [2]
            UINT32  FrameListRollover       : 1;    // [3]
            UINT32  HostSystemError         : 1;    // [4]
            UINT32  IntOnAsyncAdvance       : 1;    // [5]
            UINT32  Reserved1               : 6;    // [11:6]
            UINT32  HCHalted                : 1;    // [12]
            UINT32  Reclamation             : 1;    // [13]
            UINT32  PeriodicSheduleStatus   : 1;    // [14]
            UINT32  AsyncSheduleStatus      : 1;    // [15]
            UINT32  Reserved2               : 16;   // [31:16]
        } Field;
    };
} EHCI_USBSTS;

typedef struct _EHCI_USBINTR
{
    union {
        UINT32  AllBits;
            #define EHCI_USBINT_EN      BIT0    // Interrupt Enable
            #define EHCI_USBERRINT_EN   BIT1    // Error Interrupt Enable
            #define EHCI_PCDINT_EN      BIT2    // Port Change Detect Interrupt Enable
            #define EHCI_FLRINT_EN      BIT3    // Frame List Rollover Interrupt Enable
            #define EHCI_HSEINT_EN      BIT4    // Host System Error Interrupt Enable
            #define EHCI_IAAINT_EN      BIT5    // Interrupt on Async Advance Enable
        struct
        {
            UINT32  UsbIntEnable            : 1;    // [0]
            UINT32  UsbErrIntEnable         : 1;    // [1]
            UINT32  PortChangeIntEnable     : 1;    // [2]
            UINT32  FrameListRolloverEnable : 1;    // [3]
            UINT32  HostSystemErrorEnable   : 1;    // [4]
            UINT32  IntOnAsyncAdvanceEnable : 1;    // [5]
            UINT32  Reserved                : 26;   // [31:6]
        } Field;
    };
} EHCI_USBINTR;

typedef struct _EHCI_PORTSC
{
    union {
        UINT32  AllBits;
            #define EHCI_CURRENTCONNECTSTATUS   BIT0
            #define EHCI_CONNECTSTATUSCHANGE    BIT1
            #define EHCI_PORTENABLE             BIT2
            #define EHCI_PORTENABLESTATUSCHANGE BIT3
            #define EHCI_OVERCURRENTACTIVE      BIT4
            #define EHCI_OVERCURRENTCAHGE       BIT5
            #define EHCI_FORCEPORTRESUME        BIT6
            #define EHCI_SUSPEND                BIT7
            #define EHCI_PORTRESET              BIT8
            #define EHCI_DMINUSBIT              BIT10
            #define EHCI_DPLUSBIT               BIT11
            #define EHCI_PORTPOWER              BIT12
            #define EHCI_PORTOWNER              BIT13
            #define EHCI_WKCNNT_E               BIT20   // Wake On Connect Enable
            #define EHCI_WKDSCNNT_E             BIT21   // Wake On Disconnect Enable
            #define EHCI_WKOC_E                 BIT22   // Wake On Over-current Enable
        struct
        {
            UINT32  CurrentConnectStatus    : 1;    // [0]
            UINT32  ConnectStatusChange     : 1;    // [1]
            UINT32  PortEnable              : 1;    // [2]
            UINT32  PortEnableChange        : 1;    // [3]
            UINT32  OvercurrentActive       : 1;    // [4]
            UINT32  OvercurrentChange       : 1;    // [5]
            UINT32  ForcePortResume         : 1;    // [6]
            UINT32  Suspend                 : 1;    // [7]
            UINT32  PortReset               : 1;    // [8]
            UINT32  Reserved1               : 1;    // [9]
            UINT32  LineStatus              : 2;    // [11:10]
            UINT32  PortPower               : 1;    // [12]
            UINT32  PortOwner               : 1;    // [13]
            UINT32  PortIndicatorControl    : 2;    // [15:14]
            UINT32  PortTestControl         : 4;    // [19:16]
            UINT32  WakeOnConnectEnable     : 1;    // [20]
            UINT32  WakeOnDisconnectEnable  : 1;    // [21]
            UINT32  WakeOnOvercurrentEnable : 1;    // [22]
            UINT32  Reserved2               : 9;    // [31:23]
        } Field;
    };
} EHCI_PORTSC;



typedef struct _EHCI_HC_OPER_REG {
    EHCI_USBCMD     UsbCmd;
    volatile EHCI_USBSTS     UsbSts;
    EHCI_USBINTR    UsbIntr;
    UINT32          FrIndex;
    UINT32          CtrlDsSegment;
    UINT32          PeriodicListBase;
    UINT32          AsyncListAddr;
    UINT8           Reserved[36];
    UINT32          ConfigFlag;
    EHCI_PORTSC     PortSC[16];
} EHCI_HC_OPER_REG;


typedef struct {
    UINT32      dNextqTDPtr;
    UINT32      dAltNextqTDPtr;
    UINT32      dToken;
    UINT32      dBufferPtr0;
    UINT32      dBufferPtr1;
    UINT32      dBufferPtr2;
    UINT32      dBufferPtr3;
    UINT32      dBufferPtr4;
#if EHCI_64BIT_DATA_STRUCTURE
    UINT32      dReserved[8];
#endif
} EHCI_QTD;


typedef struct {
    UINT32      dLinkPointer;
    UINT32      dEndPntCharac;
    UINT32      dEndPntCap;
    UINT32      dCurqTDPtr;
    UINT32      dNextqTDPtr;
    UINT32      dAltNextqTDPtr;
    UINT32      dToken;
    UINT32      dBufferPtr0;
    UINT32      dBufferPtr1;
    UINT32      dBufferPtr2;
    UINT32      dBufferPtr3;
    UINT32      dBufferPtr4;
#if EHCI_64BIT_DATA_STRUCTURE
    UINT32      dReserved[8];
#endif
    VOID        *fpCallBackFunc;
    EHCI_QTD    *fpFirstqTD;
    UINT8       bActive;
    UINT8       bErrorStatus;
    UINT8       *fpDevInfoPtr;
    UINT8       aDataBuffer[8];
    UINT8       Pad[38-3*sizeof(VOID*)];    // sizeof(EHCI_QH)should be divisible by 32
} EHCI_QH;                          // because of 32 bin pointers; the size of
                                    // the structure has to be 32 bytes aligned

typedef struct _EHCI_DESC_PTRS_
{
    EHCI_QH     *fpQH1ms;
    EHCI_QH     *fpQH8ms;
    EHCI_QH     *fpQH32ms;
    EHCI_QH     *fpQHControl;
    EHCI_QTD    *fpqTDControlSetup;
    EHCI_QTD    *fpqTDControlData;
    EHCI_QTD    *fpqTDControlStatus;
    EHCI_QH     *fpQHInterrupt;
    EHCI_QTD    *fpqTDInterruptData;
    EHCI_QH     *fpQHBulk;
    EHCI_QTD    *fpqTDBulkData;

} EHCI_DESC_PTRS;


typedef struct _PEI_EHCI_DEV
{
    UINTN                           Signature;
        #define PEI_EHCI_DEV_SIGNATURE  EFI_SIGNATURE_32( 'p', 'e', 'h', 'c' )
    EFI_PEI_SERVICES                **PeiServices;
    PEI_USB_HOST_CONTROLLER_PPI     UsbHostControllerPpi;
    EFI_PEI_PPI_DESCRIPTOR          PpiDescriptor;
    EFI_PEI_CPU_IO_PPI              *CpuIoPpi;
    EFI_PEI_STALL_PPI               *StallPpi;
    EFI_PEI_PCI_CFG2_PPI            *PciCfgPpi;
    UINT32                          UsbHostControllerBaseAddress;
    EHCI_HC_OPER_REG                *EhciHcOpReg;
    UINT8                           bNumPorts;
    UINT32                          HcStatus;
    EHCI_DESC_PTRS                  stEHCIDescPtrs;
	UINT16							PortResetStatusChangeMap;
    UINT16                          AsyncListSize;
    UINT32                          *FrameList;
    UINT8                           ControllerIndex;
    BOOLEAN                         FreeResource;
} PEI_EHCI_DEV;

#pragma pack(pop)

#define PEI_MAX_EHCI_DATA_SIZE  20 * 1024

//----------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------



EFI_STATUS InitializeUsbHC (
    PEI_EHCI_DEV *UhcDev );

VOID EhciInitHC (
    EFI_PEI_SERVICES **PeiServices,
    PEI_EHCI_DEV     *pEhcDev );

VOID EhciReset (
    EFI_PEI_SERVICES **PeiServices,
    PEI_EHCI_DEV     *pEhcDev );

VOID EhciHcEnableRootHub (
    PEI_EHCI_DEV *EhciDevPtr,
    UINT8        PortNumber );

VOID EHCIWaitForAsyncTransferComplete (
    EFI_PEI_SERVICES    **PeiServices,
    PEI_EHCI_DEV        *EhciDevPtr,
    EHCI_QH             *QHead,
    UINT32              TimeOut);

EFI_STATUS
EFIAPI
EhciHcControlTransfer(
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
EhciHcBulkTransfer(
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
EhciHcSyncInterruptTransfer(
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
EhciHcGetRootHubPortNumber(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
);

EFI_STATUS
EFIAPI
EhciHcGetRootHubPortStatus(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS        *PortStatus
);

EFI_STATUS
EFIAPI
EhciHcSetRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS
EFIAPI
EhciHcClearRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS
EFIAPI
EhciHcReset(
	IN EFI_PEI_SERVICES            **PeiServices,
  	IN PEI_USB_HOST_CONTROLLER_PPI *This,
  	IN UINT16                      Attributes
);

VOID EhciInitializePeriodicSchedule (
    EFI_PEI_SERVICES **PeiServices,
    PEI_EHCI_DEV     *EhciDevPtr);

VOID
EHCIAddPeriodicQH(
    EHCI_QH *QH,
    UINT32  *FrameListPtr);

EFI_STATUS
EFIAPI
EhciHcActivatePolling(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN VOID                        *UsbDev);

VOID
EHCISetQTDBufferPointers(
    EHCI_QTD    *fpQtd,
    UINT8       *fpBuf,
    UINT32      dSize);

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
