//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file Ohci.h
    AMI USB OHCI driver header

**/

// Avoid including multiple instance of this file
#ifndef     __OHCI_H
#define     __OHCI_H

// Global equates for OHCI
//-------------------------------------------------------------------------
#define OHCI_FRAME_LIST_SIZE        32  // Number of DWORDs in interrupt list
#define MAX_OHCI_BULK_DATA_SIZE     (4 * 1024)  // 4K

// HCCA - Host Controller Commumications Area
//----------------------------------------------------------------------------
/**
    OHCI host controller communications area is used by the
    host controller driver(BIOS) to communicate with the
    OHCI based host controller.  This data area should be
    bus master capable.  Refer OHCI data sheet for more
    information.

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      INTERRUPTLIST   ARRAY   32 entries of periodic transfer pointer
      HCCAFRAMENUMBER WORD    Current frame number
      HCCAPAD1    WORD    Reserved
      HCCADONEHEAD    DWORD   Location where the done head pointers will be placed
      RES_HCCA    ARRAY   120 bytes of reserved data

**/

//OHCIHCCA      STRUC
//  INTERRUPTLIST       DD  OHCI_FRAME_LIST_SIZE DUP (?)
//  HCCAFRAMENUMBER     DW  ?
//  HCCAPAD1        DW  ?
//  HCCADONEHEAD        DD  ?
//  RES_HCCA        DB  120 DUP (?)
//OHCIHCCA      ENDS
#pragma pack(push, 1)

typedef struct {
    UINT32      aInterruptList[OHCI_FRAME_LIST_SIZE];
    UINT16      wHccaFrameNumber;
    UINT16      wHccaPad1;
    UINT32      dHccaDoneHead;
    UINT8       aResHcca[120];
} OHCI_HCCA_PTRS;

/**
    OHCI general transfer descriptor structure. This structure is
    used for bulk, interrupt and control transfers. This structure
    holds the information needed for the transfer like buffer
    size, address etc.  Refer to OHCI specification for more
    information.  The last sixteen bytes in the structure (after
    BufferEnd) is AMIBIOS internal data structure.

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      dControlStatus  DWORD       Control & status (OHCI_TD_CONTROL)
      fpCurrentBufferPointer DWORD    Current buffer pointer
      fpLinkPointer   DWORD       Pointer to the next GTD
      fpBufferEnd DWORD       Pointer to the end of the buffer
      dCSReloadValue  DWORD       Copy of control status during scheduling
      bCallBack   NEAR        Pointer to call back function
      bActiveFlag BYTE        Non-zero value indicates TD is active
      bDeviceAddr BYTE        USB device address
      aSetupData  ARRAY       8 byte setup data buffer

**/
/*
OHCI_TD     STRUC

    ControlStatus       DD  ?   ; Control and status fields
    CurrentBufferPointer    DD  ?   ; Current buffer pointer
    LinkPointer     DD  ?   ; Pointer to the next TD
    BufferEnd       DD  ?   ; End pointer of data buffer

    CSReloadValue       DD  ?   ; Reload value for control
    pCallback       PTRFUNCTDCALLBACK   ?   ; Routine to call on completion
    ActiveFlag      DB  ?   ; If nonzero, TD is active
    DeviceAddress       DB  ?   ; Device address
    SetupData       DB  8 dup (?) ; Used for setup packet

OHCI_TD     ENDS*/

typedef struct {
    UINT32      dControlStatus;         // Control and status fields
    UINT32      fpCurrentBufferPointer; // Current buffer pointer
    UINT32      fpLinkPointer;          // Pointer to the next TD
    UINT32      fpBufferEnd;            // End pointer of data buffer

    UINT32      dCSReloadValue;         // Control status reload value
    UINT8       bCallBackIndex;         // Index of the routine to call on completion
    UINT8       bActiveFlag;            // Routine to call on completion
    UINT8       bDeviceAddr;            // Device Address
    UINT8       bResvd;
    UINT8       aSetupData[8];          // Used for setup packet
} OHCI_TD;


/**
    OHCI Endpoint descriptor structure. This structure is needed
    for all the USB transaction. This structure had the
    information regarding the transfer.  Refer to OHCI specification
    for more information

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      dControl        DWORD   ED control fields (refer OHCI_ED_CONTROL)
      fpTDTailPtr DWORD   TD queue tail pointer
      fpTDHeadPointer DWORD   TD queue head pointer
      fpEDLinkPointer DWORD   Pointer to the next ED
      aReserved   ARRAY   16 bytes of reserved data

**/

typedef struct _OHCI_ED {
    UINT32      dControl;               //  ED control fields
    UINT32      fpTailPointer;          // TD queue tail pointer
    UINT32      fpHeadPointer;          // TD queue head pointer
    UINT32      fpEDLinkPointer;    // Pointer to next ED
    UINT8       Interval;
    UINT8       aReserved[15];
} OHCI_ED;

#pragma pack(pop)

/*
typedef struct
{
    UINT32          dControlStatus;
    PHY_ADDR        pCurBufPtr;
    PHY_ADDR        pLinkPtr;
    PHY_ADDR        pBufEnd;

// AMI defined fields
    UINT32          dCSReload;  // Control status reload value
    UINT8           bCallBackIndex;
    UINT8           bActiveFlag;
    UINT8           bDevAddr;
    UINT8           bReserved;
    UINT8           aDataArea[8];
} OHCI_GTD, _FAR_ *FPOHCI_GTD;
*/
//typedef   struct
//{
//  OHCI_GTD _FAR_ *    fpSomeGTD;
//  OHCI_ED  _FAR_ *    fpSomeED;
//} OHCI_DESC_PTRS, _FAR_ *FPOHCI_DESC_PTRS;


/**
    Descriptors structure is used to hold the host controller
    instance specific general transfer descriptor and endpoint
    descriptor pointers.  The following structure defines such
    the descriptors for OHCI based host controller instances.

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      fpED1ms     OHCI_ED ED scheduled in 1ms timeframe
      fpED2ms     OHCI_ED ED scheduled in 2ms timeframe
      fpED8ms     OHCI_ED ED scheduled in 8ms timeframe
      fpED32ms    OHCI_ED ED scheduled in 32ms timeframe
      fpTD32ms    OHCI_TD TD scheduled in 32ms timeframe
      fpEDRepeat  OHCI_ED ED scheduled for keyboard repeat rate generator
      fpTDRepeat  OHCI_TD TD scheduled for keybord repeat rate generator
               TDRepeat   OHCI_TD TD associated with generation of repeat data
      fpEDControl OHCI_ED ED associated for control transfer
               fpTDControlSetup   OHCI_TD TD associated with control setup
               fpTDControlData    OHCI_TD TD associated with control data
               fpTDControlStatus OHCI_TD TD associated with control status
      fpEDInterrupt   OHCI_ED ED associated for interrupt transfer
               fpTDInterruptData OHCI_TD TD associated to transfer interrupt data
      fpEDBulk        OHCI_ED ED associated for bulk transfer
      fpTDBulkData    OHCI_TD TD associated to transfer bulk data

**/
/*
OHCIDescriptors     STRUC
    ED1ms           DW  ?
    ED2ms           DW  ?
    ED8ms           DW  ?
// Do not change the following order of ED32ms and TD32ms
    ED32ms          DW  ?
    TD32ms          DW  ?
// Do not change the following order of EDRepeat and TDRepeat
    EDRepeat        DW  ?
    TDRepeat        DW  ?
    EDControl       DW  ?
    TDControlSetup      DW  ?
    TDControlData       DW  ?
    TDControlStatus     DW  ?
    EDInterrupt     DW  ?
    TDInterruptData     DW  ?
    EDBulk          DW  ?
    TDBulkData      DW  ?
OHCIDescriptors     ENDS
*/

typedef struct {
    OHCI_ED     *PeriodicEd;
// Do not change the following order of EDRepeat and TDRepeat
    OHCI_ED     *fpEDRepeat;
    OHCI_TD     *fpTDRepeat;

    OHCI_ED     *fpEDControl;
    OHCI_TD     *fpTDControlSetup;
    OHCI_TD     *fpTDControlData;
    OHCI_TD     *fpTDControlStatus;

    OHCI_ED     *fpEDInterrupt;
    OHCI_TD     *fpTDInterruptData;

    OHCI_ED     *fpEDBulk;
    OHCI_TD     *fpTDBulkData;
} OHCI_DESC_PTRS;

#define USB_OHCI_DESCRIPTOR_SIZE_BLK    ((size of OHCIDescriptors + USB_MEM_BLK_SIZE - 1) >> USB_MEM_BLK_SIZE_SHIFT)

/**
    Bit definition for OHCI_ED control field

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      FuncAddress BITS:06-00  USB device address
      EndpointNum BITS:10-07  Endpoint number
      Direction   BITS:12-11  Direction of data flow
      Speed       BITS:13     Endpoint speed
      Skip        BITS:14     If set ED will be skipped
      Format      BITS:15     Set for isochronous endpoint
      MaxPacketSize   BITS:26-16  Endpoint max packet size
      Reserved    BITS:31-27  Reserved bits

**/


// Bit definitions for endpoint descriptor control field
//-------------------------------------------------------------------------
#define ED_FUNCTION_ADDRESS 0x0000007f
#define ED_ENDPOINT         0x00000780
#define ED_DIRECTION        0x00001800
#define ED_OUT_PACKET       0x00000800
#define ED_IN_PACKET        0x00001000
#define ED_LOW_SPEED        0x00002000
#define ED_SKIP_TDQ         0x00004000
#define ED_FORMAT           0x00008000
#define ED_MAX_PACK_SIZE    0x07ff0000

//Bit definition for endpoint descriptor direction
//-------------------------------------------------------------------------
#define ED_DATA_OUT         0x01
#define ED_DATA_IN          0x02

// Bit definition for endpoint descriptor TD queue tail pointer
//-------------------------------------------------------------------------
#define ED_HALTED           0x00000001
#define ED_TOGGLE_CARRY     0x00000002

//Bit define for general pointer
//------------------------------------------------------------------------
#define OHCI_TERMINATE      0x00000000


/**
    Bit definition for OHCI_TD ControlStatus field

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      Reserved    BITS:17-00  Reserved bits
      BufferRounding  BITS:18     Buffer rounding (1-Allow small packets)
      DirectionPid    BITS:20-19  Direction & PID (SETUP/IN/OUT etc)
      DelayInt    BITS:23-21  Num. frames to wait before interrupting
      DataToggle  BITS:25-24  Data toggle
      ErrorCount  BITS:27-26  Error count
      ConditionCode   BITS:31-28  Completion condition code

**/

// Bit definition for general transfer descriptor control fields
//-------------------------------------------------------------------------
#define     GTD_BUFFER_ROUNDING 0x000040000
#define     GTD_DIRECTION_PID   0x000180000
#define     GTD_SETUP_PACKET    0x000000000
#define     GTD_OUT_PACKET      0x000080000
#define     GTD_IN_PACKET       0x000100000
#define     GTD_DELAY_INTERRUPT 0x000e00000
#define     GTD_IntD            0x000000000 // depend on device,now guest 0
#define     GTD_DATA_TOGGLE     0x003000000
#define     GTD_SETUP_TOGGLE    0x002000000 // same as MSB of data toggle
#define     GTD_DATA1_TOGGLE    0x003000000
#define     GTD_STATUS_TOGGLE   0x003000000
#define     GTD_ERROR_COUNT     0x00c000000
#define     GTD_NO_ERRORS       0x000000000
#define     GTD_ONE_ERROR       0x004000000
#define     GTD_TWO_ERRORS      0x008000000
#define     GTD_THREE_ERRORS    0x00C000000
#define     GTD_CONDITION_CODE  0x0f0000000
//------------------------------------------------------------------------

// Bit define for ConditionCode or CompletionCode use for each CallBack func
// transmission error
//-------------------------------------------------------------------------
#define     GTD_STATUS_FIELD        0x0f0000000 //include NOT_ACCESS
#define     GTD_NO_ERROR            0x00
#define     GTD_CRC_ERROR           0x01
#define     GTD_BIT_STUFF           0x02
#define     GTD_TOGGLE_MISMATCH     0x03
#define     GTD_STALL               0x04
#define     GTD_DEVICE_NOT_RESPOND  0x05
#define     GTD_PID_CHECK_ERROR     0x06
#define     GTD_UNEXPECTED_PID      0x07
#define     GTD_DATA_OVERRUN        0x08
#define     GTD_DATA_UNDERRUN       0x09
#define     GTD_BUFFER_OVERRUN      0x0c        // not used for GTD
#define     GTD_BUFFER_UNDERRUN     0x0d        // not used for GTD
#define     GTD_NOT_ACCESSED        0x0f

//-------------------------------------------------------------------------
// Equates for Host Controller Operational Register
// reg for control and status
//-------------------------------------------------------------------------
#define     OHCI_REVISION_REG       0x00
#define     OHCI_CONTROL_REG        0x04
#define     OHCI_COMMAND_STATUS     0x08
#define     OHCI_INTERRUPT_STATUS   0x0c
#define     OHCI_INTERRUPT_ENABLE   0x10
#define     OHCI_INTERRUPT_DISABLE  0x14
// reg for memory pointer
#define     OHCI_HCCA_REG           0x18
#define     OHCI_PERIOD_CURRENT_ED  0x1c
#define     OHCI_CONTROL_HEAD_ED    0x20
#define     OHCI_CONTROL_CURRENT_ED 0x24
#define     OHCI_BULK_HEAD_ED       0x28
#define     OHCI_BULK_CURRENT_ED    0x2c
#define     OHCI_DONE_HEAD          0x30
// reg for frame counter
#define     OHCI_FRAME_INTERVAL     0x34
#define     OHCI_FRAME_REMAINING    0x38
#define     OHCI_FRAME_NUMBER       0x3c
#define     OHCI_PERIODIC_START     0x40
#define     OHCI_LS_THRESHOLD       0x44
// reg for root hub
#define     OHCI_RH_DESCRIPTOR_A    0x48
#define     OHCI_RH_DESCRIPTOR_B    0x4c
#define     OHCI_RH_STATUS          0x50
#define     OHCI_RH_PORT1_STATUS    0x54
#define     OHCI_RH_PORT2_STATUS    0x58

// OHCI emulation register equates
#define     OHCI_HCE_CONTROL        0x100
#define     OHCI_HCE_INPUT          0x104
#define     OHCI_HCE_OUTPUT         0x108
#define     OHCI_HCE_STATUS         0x10C

// Bit definitions for emulation registers
#define     HCE_CNTRL_EMULATION_ENABLE      BIT0
#define     HCE_CNTRL_EMULATION_INTERRUPT   BIT1
#define     HCE_CNTRL_CHARACTER_PENDING     BIT2
#define     HCE_CNTRL_IRQ_ENABLE            BIT3
#define     HCE_CNTRL_EXT_IRQ_ENABLE        BIT4
#define     HCE_CNTRL_GA20_SEQ              BIT5
#define     HCE_CNTRL_IRQ1_ACTIVE           BIT6
#define     HCE_CNTRL_IRQ12_ACTIVE          BIT7
#define     HCE_CNTRL_A20_STATE             BIT8


#define     HCE_STS_OUTPUTFULL      BIT0
#define     HCE_STS_INPUTFULL       BIT1
#define     HCE_STS_FLAG            BIT2
#define     HCE_STS_CMDDATA         BIT3
#define     HCE_STS_INHIBIT_SWITCH  BIT4
#define     HCE_STS_AUXOUTPUTFULL   BIT5
#define     HCE_STS_TIMEOUT         BIT6
#define     HCE_STS_PARITY          BIT7
//-------------------------------------------------------------------------
// Bit define for HC control register
//-------------------------------------------------------------------------
#define     CONTROL_BULK_RATE       0x0003
#define     PERIODIC_LIST_ENABLE    0x0004
#define     ISOCHRONOUS_ENABLE      0x0008
#define     CONTROL_LIST_ENABLE     0x0010
#define     BULK_LIST_ENABLE        0x0020
#define     HC_FUNCTION_STATE       0x00c0
#define     USBRESET                0x0000
#define     USBRESUME               0x0040
#define     USBOPERATIONAL          0x0080
#define     USBSUSPEND              0x00c0
#define     INTERRUPT_ROUTING       0x0100
#define     REMOTE_WAKEUP_CONNECT   0x0200
#define     REMOTE_WAKEUP_ENABLE    0x0400
//-------------------------------------------------------------------------

// Bit define for HC command status register
//-------------------------------------------------------------------------
#define     HC_RESET                    0x00001
#define     CONTROL_LIST_FILLED         0x00002
#define     BULK_LIST_FILLED            0x00004
#define     OWNERSHIP_CHANGE_REQUEST    0x00008
#define     SCHEDULING_OVERRUN_COUNT    0x30000

// Bit define for HC interrupt status register
//-------------------------------------------------------------------------
#define     SCHEDULING_OVERRUN      0x00000001
#define     WRITEBACK_DONEHEAD      0x00000002
#define     START_OF_FRAME          0x00000004
#define     RESUME_DETECTED         0x00000008
#define     UNCOVERABLE_ERR         0x00000010
#define     FRAMENUMBER_OVERFLOW    0x00000020
#define     RH_STATUS_CHANGE        0x00000040
#define     OWNERSHIP_CHANGE        0x40000000

// Bit define for HC interrupt enable register
//-------------------------------------------------------------------------
#define     SCHEDULING_OVERRUN_ENABLE   0x00000001
#define     WRITEBACK_DONEHEAD_ENABLE   0x00000002
#define     START_OF_FRAME_ENABLE       0x00000004
#define     RESUME_DETECTED_ENABLE      0x00000008
#define     UNCOVERABLE_ERR_ENABLE      0x00000010
#define     FRAMENUMBER_OVERFLOW_ENABLE 0x00000020
#define     RH_STATUS_CHANGE_ENABLE     0x00000040
#define     OWNERSHIP_CHANGE_ENABLE     0x40000000
#define     MASTER_INTERRUPT_ENABLE     0x80000000

// Bit define for HC interrupt disable register
//-------------------------------------------------------------------------
#define     SCHEDULING_OVERRUN_DISABLE  0x00000001
#define     WRITEBACK_DONEHEAD_DISABLE  0x00000002
#define     START_OF_FRAME_DISABLE      0x00000004
#define     RESUME_DETECTED_DISABLE     0x00000008
#define     UNCOVERABLE_ERR_DISABLE     0x00000010
#define     FRAMENUMBER_OVERFLOW_DISABLE    0x00000020
#define     RH_STATUS_CHANGE_DISABLE    0x00000040
#define     OWNERSHIP_CHANGE_DISABLE    0x40000000
#define     MASTER_INTERRUPT_DISABLE    0x80000000

// Bit define for HC frame interval register
//-------------------------------------------------------------------------
#define     FRAME_INTERVAL              0x00003fff
#define     FS_LARGEST_DATA_PACKET      0x4fff0000
#define     FRAME_INTERVAL_TOGGLE       0x80000000

// Bit define for HC frame remaining register
//-------------------------------------------------------------------------
#define     FRAME_REMAINING             0x00003fff
#define     FRAME_REMAINING_TOGGLE      0x80000000

// Bit define for HC root hub descriptor A register
//-------------------------------------------------------------------------
#define     RH_PORT_NUMBER          0x000000ff
#define     POWER_SWITCH_MODE       0x00000100
#define     NO_POWER_SWITCH         0x00000200
#define     DEVICE_TYPE             0x00000400
#define     OVERCURRENT_PROTECT     0x00000800
#define     NO_OVERCURRENT_PROTECT  0x00001000
#define     POWERON2POWERGOOD_TIME  0x0ff000000


// Bit define for HC root hub descriptor B register
//-------------------------------------------------------------------------
#define     DEVICE_REMOVABLE        0x0000ffff
#define     PORT_POWER_MASK         0x0ffff0000

// Bit define for HC root hub status register
//-------------------------------------------------------------------------
#define     LOCAL_POWER_STATUS      0x00000001
#define     CLEAR_GLOBAL_POWER      0x00000001
#define     OVERCURRENT             0x00000002
#define     DEVICE_REMOTE_WAKEUP    0x00008000
#define     SET_REMOTE_WAKEUP       0x00008000
#define     LOCAL_POWER_CHANGE      0x00010000
#define     SET_GLOBAL_POWER        0x00010000
#define     OVERCURRENT_CHANGE      0x00020000
#define     CLEAR_REMOTE_WAKEUP     0x80000000

// Bit define for HC root hub port1,2 status register
//-------------------------------------------------------------------------
#define     CURRENT_CONNECT_STATUS      0x00000001
#define     CLEAR_PORT_ENABLE           0x00000001
#define     PORT_ENABLE_STATUS          0x00000002
#define     SET_PORT_ENABLE             0x00000002
#define     PORT_SUSPEND_STATUS         0x00000004
#define     SET_PORT_SUSPEND            0x00000004
#define     PORT_OVERCURRENT            0x00000008
#define     CLEAR_PORT_SUSPEND          0x00000004
#define     PORT_RESET_STATUS           0x00000010
#define     SET_PORT_RESET              0x00000010
#define     PORT_POWER_STATUS           0x00000100
#define     SET_PORT_POWER              0x00000100
#define     LOW_SPEED_DEVICE_ATTACHED   0x00000200
#define     CLEAR_PORT_POWER            0x00000200
#define     CONNECT_STATUS_CHANGE       0x00010000
#define     PORT_ENABLE_STATUS_CHANGE   0x00020000
#define     PORT_SUSPEND_STATUS_CHANGE  0x00040000
#define     PORT_OVERCURRENT_CHANGE     0x00080000
#define     PORT_RESET_STATUS_CHANGE    0x00100000

#endif      // __OHCI_H

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
