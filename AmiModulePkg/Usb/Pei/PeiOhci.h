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

/** @file PeiOhci.h
    This file is the main header file for the OHCI PEI USB
    recovery module. It contains generic constant and type
    declarations/definitions.

**/

#ifndef _OHCIPEI_H
#define _OHCIPEI_H

#include <PiPei.h>
#include <Ppi/UsbPeiPpi.h>



//----------------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// OHCI_FIXED_DELAY_MS( OhciDevPtr, milliseconds )
//
// invokes a delay specified in milliseconds using the PEI_OHCI_DEV.StallPpi
// member interface pointer, which is initialized from a EFI_PEI_STALL_PPI
// interface.
#define OHCI_FIXED_DELAY_MS( OhciDevPtr, milliseconds ) \
    OhciDevPtr->StallPpi->Stall( OhciDevPtr->PeiServices, \
    OhciDevPtr->StallPpi, milliseconds * 1000 );


//----------------------------------------------------------------------------
// MASK(high,low) 
//
// defines a bit mask value with bits set in the bit
// range specified from high down to low (zero-based)
//
// Notes:
//      1) this is expected to be used only in 32-bit compiled code
#ifndef MASK
#define MAX_VAL  0xffffffff     // maximum value supported by CPU
//#define MAX_BIT  31             // highest bit position in MAX_VAL
//#define MASK( high, low )  (\
//        ( MAX_VAL >> ( MAX_BIT - ( (high) - (low) ) ) ) \
//      << (low) \
//)
#endif


//----------------------------------------------------------------------------
// READ_BITS(src,high,low) 
//
// extracts a bit field shifted down to bit position 0 from src, where high 
// and low define the bit range (zero-based).
//
// Notes:
//      1) this is expected to be used only in 32-bit compiled code
#ifndef READ_BITS
#define READ_BITS( src, high, low )  (\
        ( (src) & MASK( (high), (low) ) ) \
        >> (low) \
)
#endif


//----------------------------------------------------------------------------
// WRITE_BITS( src, val, high, low )
//
// returns the modified src after writing a bit field with val, where the bit
// field's range is specified by high down to low.
//
// Notes:
//      1) the user must ensure that the val fits in the bit range
//      2) the bits in the range high:low are the only bits touched and
//         they are cleared and replaced with val
//      3) this is expected to be used only in 32-bit compiled code
#ifndef WRITE_BITS
#define WRITE_BITS( src, val, high, low )  (\
        ( (src) & ~MASK( (high), (low) ) ) \
        | ( (val) << (low) ) \
)
#endif


//----------------------------------------------------------------------------
// ABORT_ON_ERROR( call ) 
//
// simplifies the task of returning EFI_STATUS value after an unsuccessful 
// function execution.
#define ABORT_ON_ERROR( call )  {\
        EFI_STATUS status = call; \
        if ( EFI_ERROR( status ) ) {\
            return status;}}
        
            
//----------------------------------------------------------------------------
// OHCI_PCI_ADDRESS( ControllerIndex, Register )
//
// allows easy generation of a valid PCI address using an index into the 
// gOhciControllerPciTable and a register number
#define OHCI_PCI_ADDRESS( ControllerIndex, Register )  EFI_PEI_PCI_CFG_ADDRESS( \
    gOhciControllerPciTable[ControllerIndex].Bus, \
    gOhciControllerPciTable[ControllerIndex].Device, \
    gOhciControllerPciTable[ControllerIndex].Function, \
    Register )


//----------------------------------------------------------------------------
// EFI_SIGNATURE_32( A, B, C, D)
//
// defines a signature that can be used with the PEI_CR macro to return a 
// pointer to the data structure that contains the specified field and
// signature
//#define EFI_SIGNATURE_16(A,B)             ((A) | ((B)<<8))
//#define EFI_SIGNATURE_32(A,B,C,D) \
//   (EFI_SIGNATURE_16((A),(B)) | (EFI_SIGNATURE_16((C),(D)) << 16))


//----------------------------------------------------------------------------
// PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( a )
//
// uses the _CR macro to obtain a pointer to a PEI_OHCI_DEV structure from 
// a PEI_USB_HOST_CONTROLLER_PPI interface pointer
#define _CR( Record, TYPE, Field ) \
    ( (TYPE *) ( (CHAR8 *) (Record) - (CHAR8 *) &( ( (TYPE *) 0 )->Field ) ) )
#define PEI_RECOVERY_USB_OHCI_DEV_FROM_THIS( a ) \
    _CR( a, PEI_OHCI_DEV, UsbHostControllerPpi )


//----------------------------------------------------------------------------
// GET_PORT_STATUS_REG_OFFSET( x )
//
//Macro to get the RhPortStatus register offset
#define GET_PORT_STATUS_REG_OFFSET( x )  ( (x) + 21 ) // 0x54 / 4 = 21



//----------------------------------------------------------------------------
// Constants
//
// Notes:
//      1)  These constants are not associated with a particular structure
//          and are used globally withing OHCI-specific code
//----------------------------------------------------------------------------



// Standard OHCI PCI configuration space registers
#define OHCI_BASE_ADDR_REG         0x10     // OHCI BAR register
#define OHCI_CMD_REGISTER          0x04     // OHCI command register

// Stantdard ED and TD list structure constants
#define OHCI_TERMINATE         0x00000000

// Bit definition for endpoint descriptor TD queue tail pointer
//-------------------------------------------------------------------------
#define ED_HALTED           0x00000001
#define ED_TOGGLE_CARRY     0x00000002


//----------------------------------------------------------------------------
// Structure types
//
// Notes:
//      1)  Constants that are associated with a particular structure
//          are defined locally within the structure to facilitate easier
//          maintenance.
//----------------------------------------------------------------------------



#pragma pack(push,1)

/**
    This structure type defines the bus, device and function number
    that compose a PCI address.

 Fields:  Type    Name    Description
  ----------------------------------
  UINT8   Bus         PCI Bus on which a particular OHCI controller resides
  UINT8   Device      PCI Device number for a particular OHCI controller
  UINT8   Function    PCI Function number for a particular OHCI controller

**/

typedef struct
{
    UINT8 Bus;
    UINT8 Device;
    UINT8 Function;
} PCI_BUS_DEV_FUNCTION;


/**
    This structure type defines the parameters to be used in a call
    to EFI_PEI_PCI_CFG2_PPI.Modify(), when programming the PCI 
    configuration space for a particular OHCI controller.

 Fields:  Type    Name    Description
  ----------------------------------
  EFI_PEI_PCI_CFG_PPI_WIDTH   Size        Size of the access
  UINT32                      Register    Register/offset within the PCI configuration space
  UINT32                      SetBits     Bits to set
  UINT32                      ClearBits   Bits to clear

    @note  
      See also 

**/

typedef struct
{
    EFI_PEI_PCI_CFG_PPI_WIDTH   Size;
    UINT32                      Register;
    UINT32                      SetBits;
    UINT32                      ClearBits;
} OHCI_PCI_PROGRAMMING_TABLE;


typedef struct _HC_CONTROL
{
    union {
        UINT32 AllBits;
            // Set bits using the following
            #define CONTROL_LIST_ENABLE     BIT4
            #define BULK_LIST_ENABLE        BIT5
            #define USB_RESET               0 << 6
            #define USB_RESUME              1 << 6
            #define USB_OPERATIONAL         2 << 6
            #define USB_SUSPEND             3 << 6
            
        struct
        {
            UINT32 ControlBulkServiceRatio         : 2;     // [1:0] CBSR
            UINT32 PeriodicListEnable              : 1;     // [2] PLE
            UINT32 IsochronousEnable               : 1;     // [3] IE
            UINT32 ControlListEnable               : 1;     // [4] CLE
                // Read: TRUE or FALSE
                
            UINT32 BulkListEnable                  : 1;     // [5] BLE
                // Read: TRUE or FALSE
                
            UINT32 HostControllerFunctionalState   : 2;     // [7:6] HCFS
                // Write:
                #define SET_USB_RESET            0
                #define SET_USB_RESUME           1
                #define SET_USB_OPERATIONAL      2
                #define SET_USB_SUSPEND          3

            UINT32 InterruptRouting                : 1;     // [8] IR
            UINT32 RemoteWakeupConnected           : 1;     // [9] RWC
            UINT32 RemoteWakeupEnable              : 1;     // [10] RWE
            UINT32 Reserved                        : 21;    // [31:11]
        } Field;
    };
} HC_CONTROL;

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
typedef struct _HC_RH_PORT_STATUS
{
    union  {
        UINT32 AllBits; 
            #define SET_PORT_ENABLE                         BIT1
            #define SET_PORT_SUSPEND                        BIT2
            #define SET_PORT_RESET                          BIT4
            #define SET_PORT_POWER                          BIT8

            #define CLEAR_PORT_ENABLE                       BIT0
            #define CLEAR_SUSPEND_STATUS                    BIT3
            #define CLEAR_PORT_POWER                        BIT9
            #define CLEAR_CONNECT_STATUS_CHANGE             BIT16
            #define CLEAR_PORT_ENABLE_STATUS_CHANGE         BIT17
            #define CLEAR_PORT_SUSPEND_STATUS_CHANGE        BIT18
            #define CLEAR_PORT_OVERCURRENT_INDICATOR_CHANGE BIT19
            #define CLEAR_PORT_RESET_STATUS_CHANGE          BIT20 

        struct
        {
            UINT32 CurrentConnectStatus            : 1;         // [0] CCS
                #define RH_PORT_CLEAR_ENABLE 1
            UINT32 PortEnableStatus                : 1;         // [1] PES
                #define RH_PORT_SET_ENABLE 1
            UINT32 PortSuspendStatus               : 1;         // [2] PSS
                #define RH_PORT_SET_SUSPEND 1
            UINT32 PortOverCurrentIndicator        : 1;         // [3] POCI
                #define RH_PORT_CLEAR_SUSPEND 1
            UINT32 PortResetStatus                 : 1;         // [4] PRS
                #define RH_SET_PORT_RESET 1 // write to initiate reset
                #define RH_RESET_ACTIVE 1   // read set if reset active
            UINT32 Reserved1                       : 3;         // [7:5]
            UINT32 PortPowerStatus                 : 1;         // [8] PPS
                #define RH_PORT_POWER_ON 1

            UINT32 LowSpeedDeviceAttached          : 1;         // [9] LSDA
            UINT32 Reserved2                       : 6;         // [15:10]
            UINT32 ConnectStatusChange             : 1;         // [16] CSC
                #define RH_PORT_CLEAR_CONNECT_CHANGE 1
            UINT32 PortEnableStatusChange          : 1;         // [17] PESC
                #define RH_PORT_CLEAR_ENABLE_CHANGE 1
            UINT32 PortSuspendStatusChange         : 1;         // [18] PSSC
            UINT32 PortOverCurrentIndicatorChange  : 1;         // [19] OCIC
            UINT32 PortResetStatusChange           : 1;         // [20] PRSC
                #define RH_PORT_CLEAR_RESET_CHANGE 1
                #define PORT_RESET_NOT_COMPLETE 0
                #define PORT_RESET_COMPLETE 1
            UINT32 Reserved3                       : 11;        // [31:21]
        } Field;
    };
} HC_RH_PORT_STATUS;


typedef struct _HC_INTERRUPT_STATUS
{
    union  {
        UINT32 AllBits;
        struct
        {
            #define HCINT_STATUS_CLEAR  1     // write 1 to clear a status bit

            UINT32 SchedulingOverrun   : 1;     // [0] SO
            UINT32 WriteBackDoneHead   : 1;     // [1] WDH
                #define HCINT_WB_DONE   1

            UINT32 StartofFrame        : 1;     // [2] SF
            UINT32 ResumeDetected      : 1;     // [3] RD
            UINT32 UnrecoverableError  : 1;     // [4] UE
            UINT32 FrameNumberOverflow : 1;     // [5] FNO
            UINT32 RootHubStatusChange : 1;     // [6] RHSC
            UINT32 Reserved1           : 23;    // [29:7]
            UINT32 OwnershipChange     : 1;     // [30] OC
            UINT32 Reserved2           : 1;     // [31]
        } Field;
    };
} HC_INTERRUPT_STATUS;


typedef struct _HC_RH_DESCRIPTORA
{
    union  {
        UINT32 AllBits;
            #define POWERED_SAME                        (0 << 8)
            #define POWERED_INDIVIDUALLY                (1 << 8)
            #define POWER_SWITCHED                      (0 << 9)
            #define ALWAYS_POWERED                      (1 << 9)
            #define OVERCURRENT_REPORTED_COLLECTIVELY   (0 << 11)
            #define OVERCURRENT_REPORTED_PER_PORT       (1 << 11)
            #define OVERCURRENT_COLLECTIVE_PROTECTION   (0 << 12) 
            #define NO_OVERCURRENT_PROTECTION           (1 << 12)   
            
        struct
        {
            UINT32 NumberDownstreamPorts       : 8;     // [7:0] NDP
            UINT32 PowerSwitchingMode          : 1;     // [8] PSM
                #define PSM_COLLECTIVE  0   // all ports powered same
                #define PSM_PER_PORT    1   // ports powered individually

            UINT32 NoPowerSwitching            : 1;     // [9] NPS
                #define NPS_SWITCHED      0 // ports are individually 
                                            // power switched
                #define NPS_NOT_SWITCHED  1 // ports are always on when 
                                            // HC is powered

            UINT32 DeviceType                  : 1; // [10] DT (always 0)
            UINT32 OverCurrentProtectionMode   : 1; // [11] OCPM
                #define OCPM_COLLECTIVE  0  // reported collectively
                #define OCPM_PER_PORT    1  // reported per-port

            UINT32 NoOverCurrentProtection     : 1;     // [12] NOCP
                #define NOCP_PROTECTED      0   // reported collectively
                #define NOCP_NOT_PROTECTED  1   // no support

            UINT32 Reservered                  : 11;    // [23:13]
            UINT32 PowerOnToPowerGoodTime      : 8;     // [31:24] POTPGT
                                                        // (in units of 2ms)
        } Field;
    };
} HC_RH_DESCRIPTORA;


typedef struct _HC_RH_DESCRIPTORB
{
    union  {
        UINT32 AllBits;
            #define ALL_REMOVEABLE              (0x0000 << 0)
            #define ALL_PER_PORT_POWER_SWITCHED (0xfffe << 15)
        struct
        {
            UINT32 DeviceRemovable       : 16;  // [15:0] DR
                                                // Each bit represents a port
                                                // 0 = removable
                                                // 1 = not removable
            UINT32 PortPowerControlMask  : 16;  // [31:16] PPCM
                                                // Each bit represents a port
                                                // 0 = controlled by global
                                                //     power switch
                                                // 1 = per-port power control
        } Field;
    };
} HC_RH_DESCRIPTORB;

typedef struct _HC_RH_STATUS
{
    union  {
        UINT32 AllBits;
        struct
        {
            UINT32  LocalPowerStatus            : 1;    // [0] LPS
            UINT32  OverCurrentIndicator        : 1;    // [1] OCI
            UINT32  Reserved1                   : 13;   // [14:2]
            UINT32  DeviceRemoteWakeupEnable    : 1;    // [15] DRWE
            UINT32  LocalPowerStatusChange      : 1;    // [16] LPSC
                // Write 1 to turn on power to all ports or to ports
                // specified by PortPowerControlMask, depending on
                // PowerSwitchingMode.
                #define SET_GLOBAL_POWER 1
            
            UINT32  OverCurrentIndicatorChange  : 1;    // [17] OCIC
            UINT32  Reserved2                   : 13;   // [30:18]
            UINT32  ClearRemoteWakeupEnable     : 1;    // [31] CRWE
        } Field;
    };
} HC_RH_STATUS;

typedef struct _HC_FM_INTERVAL
{
    union  {
        UINT32 AllBits;
        struct
        {
            UINT32  FrameInterval       : 14;   // [13:0] FI
                #define FM_INTERVAL_DEFAULT  0x2edf
                
            UINT32  Reserved            : 2;    // [15:14] 
            UINT32  FsLargestDataPacket : 15;   // [30:16] FSMPS
                // (FrameInterval-210) * 6/7 ==> 0x2778
                // (See OHCI specification)
                #define FS_LARGEST_PACKET_DEFAULT  0x2778
                
            UINT32  FrameIntervalToggle : 1;    // [31] FIT
        } Field;
    };
} HC_FM_INTERVAL;

typedef struct _HC_HCCA
{
        #define OHCI_FRAME_LIST_SIZE       32
    UINT32      HccaInterrruptTable[OHCI_FRAME_LIST_SIZE];  // Offset 0x00
    UINT16      HccaFrameNumber;                            // Offset 0x80
    UINT16      HccaPad1;                                   // Offset 0x82
    UINT32      HccaDoneHead;                               // Offset 0x84
    UINT8       HccaReserved[120];                          // Offset 0x88
} HC_HCCA;

typedef struct _HC_COMMAND_STATUS
{
    union  {
        UINT32 AllBits;
            // Typically write to this entire register
            #define SET_CONTROLLER_RESET        (1 << 0)
            #define SET_CONTROL_LIST_FILLED     (1 << 1)
            #define SET_BULK_LIST_FILLED        (1 << 2)
            
        struct
        {
            UINT32 HostControllerReset      : 1;  // [0] HCR
            UINT32 ControlListFilled        : 1;  // [1] CLF
            UINT32 BulkListFilled           : 1;  // [2] BLF
            UINT32 OwnershipChangeRequest   : 1;  // [3] OCR
            UINT32 Reserved1                : 12; // [15:4] 
            UINT32 SchedulingOverrunCount   : 2;  // [17:16] SOC
            UINT32 Reserved2                : 14; // [31:18] 
        } Field;
    };
} HC_COMMAND_STATUS;

typedef struct _OHCI_HC_REGISTERS_
{
    UINT32 HcRevision;                          //  [31:0]        (Offset 00)
    HC_CONTROL HcControl;                       //  [63:32]       (Offset 04)
    HC_COMMAND_STATUS HcCommandStatus;          //  [95:64]       (Offset 08)
    HC_INTERRUPT_STATUS HcInterruptStatus;      //  [127:96]      (Offset 0C)
    UINT32 HcInterruptEnable;                   //  [159:128]     (Offset 10)
    UINT32 HcInterruptDisable;                  //  [191:160]     (Offset 14)
    HC_HCCA *HcHcca;                            //  [223:192]     (Offset 18)
    UINT32 HcPeriodCurrentEd;                   //  [255:224]     (Offset 1C)
    UINT32 HcControlHeadEd;                     //  [287:256]     (Offset 20)
    UINT32 HcControlCurrentEd;                  //  [319:288]     (Offset 24)
    UINT32 HcBulkHeadEd;                        //  [351:320]     (Offset 28)
    UINT32 HcBulkCurrentEd;                     //  [383:352]     (Offset 2C)
    UINT32 HcDoneHead;                          //  [415:384]     (Offset 30)
    HC_FM_INTERVAL HcFmInterval;                //  [447:416]     (Offset 34)
    UINT32 HcFmRemaining;                       //  [479:448]     (Offset 38)
    HC_FM_INTERVAL HcFmNumber;                  //  [511:480]     (Offset 3C)
    UINT32 HcPeriodicStart;                     //  [543:512]     (Offset 40)
    UINT32 HcLsThreshold;                       //  [575:544]     (Offset 44)
    HC_RH_DESCRIPTORA HcRhDescriptorA;          //  [607:576]     (Offset 48)
    HC_RH_DESCRIPTORB HcRhDescriptorB;          //  [639:608]     (Offset 4C)
    HC_RH_STATUS HcRhStatus;                    //  [671:640]     (Offset 50)
    HC_RH_PORT_STATUS HcRhPortStatus[3];        //  [767:672]     (Offset 54)
    UINT32 Reserved[40];                        //  [2047:768]    (Offset 60)
    UINT32 HceControl;                          //  [2079:2048]   (Offset 100)
    UINT32 HceInput;                            //  [2111:2080]   (Offset 104)
    UINT32 HceOutput;                           //  [2143:2112]   (Offset 108)
    UINT32 HceStatus;                           //  [2175:2144]   (Offset 10C)
} OHCI_HC_REGISTERS;


typedef struct _OHCI_TD_
{
    UINT32 dControlStatus;              // Control and status fields
        #define GTD_BUFFER_ROUNDING     (1 << 18) // [18] BufferRounding
        #define GTD_SETUP_PACKET        (0 << 19) // [20:19] Direction/PID
        #define GTD_OUT_PACKET          (1 << 19)
        #define GTD_IN_PACKET           (2 << 19)
        #define GTD_INTD                (0 << 21) // [23:21] DelayInterrupt
                                                  // depends on device
        #define GTD_DATA_TOGGLE         (3 << 24) // [25:24] DataToggle
        #define GTD_SETUP_TOGGLE        (2 << 24) // (see MSB of data toggle)
        #define GTD_DATA1_TOGGLE        (3 << 24)
        #define GTD_NO_ERRORS           (0 << 26) // [27:26] ErrorCount 

    UINT8 *fpCurrentBufferPointer;      // Current buffer pointer
    UINT8 *fpLinkPointer;               // Pointer to the next TD
    UINT8 *fpBufferEnd;                 // End pointer of data buffer
        #define MAX_CONTROL_DATA_SIZE  0x200
        
    UINT32 dCSReloadValue;              // Control status reload value
    UINT8 bCallBackIndex;               // Index of the routine to
                                      // call
                                      // on completion
    UINT8 bActiveFlag;                // Routine to call on
                                      // completion
    UINT8 bDeviceAddr;                // Device Address
    UINT8 bResvd;
    UINT8 aSetupData[8];              // Used for setup packet
} OHCI_TD, *POHCI_TD;

typedef struct _OHCI_TD_DESCRIPTOR OHCI_TD_DESCRIPTOR;
typedef struct _OHCI_TD_DESCRIPTOR
{
    union {
        UINT32 AllBits;
        struct
        {
            UINT32 Reserved        : 18;        // [17:0]
            UINT32 BufferRounding  : 1;         // [18] R
            UINT32 DirectionPid    : 2;         // [20:19] DP
            UINT32 DelayInterrupt  : 3;         // [23:21] DI
            UINT32 DataToggle      : 2;         // [25:24] T
            UINT32 ErrorCount      : 2;         // [27:26] EC
            UINT32 ConditionCode   : 4;         // [31:28] CC
                #define     GTD_NO_ERROR                    0x00
                #define     GTD_CRC_ERROR                   0x01
                #define     GTD_BIT_STUFF                   0x02
                #define     GTD_TOGGLE_MISMATCH             0x03
                #define     GTD_STALL                       0x04
                #define     GTD_DEVICE_NOT_RESPOND          0x05
                #define     GTD_PID_CHECK_ERROR             0x06
                #define     GTD_UNEXPECTED_PID              0x07
                #define     GTD_DATA_OVERRUN                0x08
                #define     GTD_DATA_UNDERRUN               0x09
                #define     GTD_BUFFER_OVERRUN              0x0C
                #define     GTD_BUFFER_UNDERRUN             0x0D 
                #define     GTD_NOT_ACCESSED                0x0F
        } Field;
    } Controlfield;
    UINT32 CurrentBufferPointer;
    UINT32 NextTd;
    UINT32 BufferEnd;
} OHCI_TD_DESCRIPTOR;


typedef struct _OHCI_ED_
{
    UINT32 dControl;                        // ED control fields
        #define ED_OUT_PACKET   0x00000800
        #define ED_IN_PACKET    0x00001000
        #define ED_SKIP_TDQ     BIT14
    POHCI_TD fpTailPointer;                 // TD queue tail pointer
    POHCI_TD fpHeadPointer;                 // TD queue head pointer
    struct _OHCI_ED_ *fpEDLinkPointer;      // Pointer to next ED
    UINT8 aReserved[16];
} OHCI_ED, *POHCI_ED;

typedef struct _OHCI_EP_DESCRIPTOR OHCI_EP_DESCRIPTOR;
typedef struct _OHCI_EP_DESCRIPTOR
{
    union {
        UINT32 AllBits;
        struct
        {
            UINT32 FunctionAddress     : 7;     // [6:0] FA
            UINT32 EndpointNumber      : 4;     // [10:7] EN
            UINT32 Direction           : 2;     // [12:11] D
            UINT32 Speed               : 1;     // [13] S
            UINT32 Skip                : 1;     // [14] K
            UINT32 Format              : 1;     // [15] F
            UINT32 MaximumPacketSize   : 11;    // [26:16] MPS
            UINT32 Reserved            : 5;     // [31:27] Reserved
        } Field;
    } Controlfield;
    UINT32 TailPointer;
    union {
        UINT32 AllBits;
        struct
        {
            UINT32 Halted          : 1;         // [0] H
            UINT32 ToggleCarry     : 1;         // [1] C
            UINT32 Reserved        : 29;        // [31:2] Reserved
        } Field;
    } HeadPointer;
    OHCI_EP_DESCRIPTOR *NextEndpointDescriptor;
} OHCI_EP_DESCRIPTOR;


typedef struct _OHCI_DESC_PTRS_
{
    POHCI_ED pstED1ms;
    POHCI_ED pstED2ms;
    POHCI_ED pstED8ms;
    // Do not change the following order of ED32ms and TD32ms
    POHCI_ED pstED32ms;
    POHCI_TD pstTD32ms;
    // Do not change the following order of EDRepeat and TDRepeat
    POHCI_ED pstEDRepeat;
    POHCI_TD pstTDRepeat;

    POHCI_ED pstEDControl;
    POHCI_TD pstTDControlSetup;
    POHCI_TD pstTDControlData;
    POHCI_TD pstTDControlStatus;

    POHCI_ED pstEDInterrupt;
    POHCI_TD pstTDInterruptData;

    POHCI_ED pstEDBulk;
    POHCI_TD pstTDBulkData;

} OHCI_DESC_PTRS, *POHCI_DESC_PTRS;

typedef struct _USB_HC_INFO_
{
    UINT32 dStateFlag;
    UINT8 bHCNum;
    UINT8 bHCType;
    UINTN *HCCommArea;
    UINTN BaseAddr;
    UINT8 bPortNum;
    UINT16 wBusDevFuncNum;
    //  FPIRQ_INFO
    UINT16 wAsyncListSize;
    UINT8 bOpRegOffset;
    UINT32 dMaxBulkDataSize;
    UINT8 bHCFlag;
    //  UINT8   bExtCapPtr; //EHCI
    //  UINT8   bRegOff;
    VOID *usb_bus_data;
    //  EFI_HANDLE  Controller;
    //  EFI_DEVICE_PATH_PROTOCOL    *pHCdp;

} USB_HC_INFO, *PUSB_HC_INFO;


typedef struct _PEI_OHCI_DEV
{
    UINTN                           Signature;
        #define PEI_OHCI_DEV_SIGNATURE  EFI_SIGNATURE_32( 'p', 'u', 'h', 'c' )
    EFI_PEI_SERVICES                **PeiServices;
    PEI_USB_HOST_CONTROLLER_PPI     UsbHostControllerPpi;
    EFI_PEI_PPI_DESCRIPTOR          PpiDescriptor;
    EFI_PEI_CPU_IO_PPI              *CpuIoPpi;
    EFI_PEI_STALL_PPI               *StallPpi;
    EFI_PEI_PCI_CFG2_PPI            *PciCfgPpi;
    UINT32                          UsbHostControllerBaseAddress;

    UINT8                           bNumPorts;
    OHCI_DESC_PTRS                  stOHCIDescPtrs;
    UINT16                          wAsyncListSize;
    UINT32                          dMaxBulkDataSize;
        #define MAX_OHCI_BULK_DATA_SIZE    4096
        
    UINT32                          *pFrameList; // HC Communication Area base
                                                 // & HccaInterruptTable base
    UINT16                          ControllerIndex;
} PEI_OHCI_DEV, *PPEI_OHCI_DEV;

#pragma pack(pop)



//----------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------



EFI_STATUS InitializeUsbHC (
    PEI_OHCI_DEV *UhcDev );

VOID OhciInitHC (
    EFI_PEI_SERVICES **PeiServices,
    PEI_OHCI_DEV     *pUhcDev,
    UINT8            Index );

VOID OhciReset (
    EFI_PEI_SERVICES **PeiServices,
    PEI_OHCI_DEV     *pUhcDev );

VOID OhciHcEnableRootHub (
    PEI_OHCI_DEV *OhciDevPtr,
    UINT8        PortNumber );

EFI_STATUS OhciHcWaitForTransferComplete (
	EFI_PEI_SERVICES **PeiServices,
    PEI_OHCI_DEV *OhciDevPtr,
    POHCI_TD     Td );

EFI_STATUS
EFIAPI
OhciHcControlTransfer(
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
OhciHcBulkTransfer(
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
OhciHcSyncInterruptTransfer(
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
OhciHcGetRootHubPortNumber(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
);

EFI_STATUS
EFIAPI
OhciHcGetRootHubPortStatus(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS        *PortStatus
);

EFI_STATUS
EFIAPI
OhciHcSetRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS
EFIAPI
OhciHcClearRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
);

EFI_STATUS
EFIAPI
OhciHcReset(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
	IN UINT16                      Attributes
);
  

EFI_STATUS
EFIAPI
OhciHcActivatePolling(
    IN EFI_PEI_SERVICES **PeiServices,
    IN VOID *UsbDev
);

VOID OhciScheduleED (
    PEI_OHCI_DEV* OhciDev,
    POHCI_ED    pX,
    POHCI_ED    pY );


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
