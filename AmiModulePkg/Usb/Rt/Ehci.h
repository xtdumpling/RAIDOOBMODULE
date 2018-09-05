//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file Ehci.h
    AMI USB EHCI support header

**/

// Avoid including multiple instance of this file
#ifndef     __EHCI_H
#define     __EHCI_H

// Global equates for EHCI
//-------------------------------------------------------------------------
#define EHCI_FRAMELISTSIZE  1024    // Number of DWORDs in frame list

// Maximum data that can be transferred in a transaction
#define MAX_EHCI_DATA_SIZE  16 * 1024

// Mask for masking unwanted bits in the QH & qTD pointers
#define EHCI_POINTER_MASK   0xFFFFFFE0  // Mask bit 0-4

// Capability Registers
//-------------------------------------------------------------------------
// These registers specify the limits, restrictions and capabilities of the
// host controller implementation
//-------------------------------------------------------------------------
#define EHCI_VERCAPLENGTH   0   // Interface Version and Capability
                                // Register Length
#define EHCI_HCSPARAMS      4   // Structural Parameters
  // EHCI_HCSPARAMS bit definitions
    #define EHCI_N_PORTS    BIT0+BIT1+BIT2+BIT3 // Number of implemented
                                                // ports
    #define EHCI_PPC        BIT4                // Port Power Control
    #define EHCI_PRR        BIT7                // Port Routing Rules
    #define EHCI_N_PCC      BIT8+BIT9+BIT10+BIT11   // Number of Ports per
                                                    // Companion Controller
    #define EHCI_N_CC       BIT12+BIT13+BIT14+BIT15 // Number of Companion
                                                    // Controllers
    #define EHCI_P_INDICATOR    BIT16   // Port Indicators
    #define EHCI_DEBUG_N        BIT20+BIT21+BIT22+BIT23 // Debug Port Number
//---------------------------------------
#define EHCI_HCCPARAMS      8   // Capability Parameters
    // EHCI_HCCPARAMS bit definitions
    #define EHCI_64BIT_CAP  0x01    // 64-bit addressing capability
    #define EHCI_PFLFLAG    0x02    // Programmable Frame List Flag
    #define EHCI_ASP_CAP    0x04    // Asynchronous Schedule  Park Capability
    #define EHCI_IST        0xF0    // Isochronous Scheduling Threshold
    #define EHCI_EECP       0xFF00  // EHCI Extended Capabilities Pointer
//---------------------------------------
#define EHCI_HCSP_PORTROUTE 0x0C    // Companion Port Route Description

// Host Controller operational registers
//-------------------------------------------------------------------------
// The following equates specify the HC operational registers as offsets of
// the operational register base. Operational register base is calculated by
// adding the value in the first capabilities register (EHCI_VERCAPLENGTH) to
// the base address of EHCI HC register address space.
//-------------------------------------------------------------------------
#define EHCI_USBCMD     0   // USB Command
  // EHCI_USBCMD bit definitions
    #define EHCI_RUNSTOP    BIT0
    #define EHCI_HCRESET    BIT1
    #define EHCI_FRM1024    0       // Reset BIT2 and BIT3 before
    #define EHCI_FRM512     BIT2    // setting the new Frame List Size
    #define EHCI_FRM256     BIT3
    #define EHCI_PER_SCHED_ENABLE           BIT4
    #define EHCI_ASYNC_SCHED_ENABLE         BIT5
    #define EHCI_INT_ASYNC_ADVANCE_ENABLE   BIT6
    #define EHCI_INTTHRESHOLD   (8 << 16)
//---------------------------------------
#define EHCI_USBSTS     4   // USB Status
    // EHCI_USBSTS bit definitions
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
//---------------------------------------
#define EHCI_USBINTR        8   // USB Interrupt Enable
  // EHCI_USBINTR bit definitions
    #define EHCI_USBINT_EN      BIT0    // Interrupt Enable
    #define EHCI_USBERRINT_EN   BIT1    // Error Interrupt Enable
    #define EHCI_PCDINT_EN      BIT2    // Port Change Detect Interrupt Enable
    #define EHCI_FLRINT_EN      BIT3    // Frame List Rollover Interrupt Enable
    #define EHCI_HSEINT_EN      BIT4    // Host System Error Interrupt Enable
    #define EHCI_IAAINT_EN      BIT5    // Interrupt on Async Advance Enable
//---------------------------------------
#define EHCI_FRINDEX            0x0C    // USB Frame Index
#define EHCI_CTRLDSSEGMENT      0x10    // 4G Segment Selector
#define EHCI_PERIODICLISTBASE   0x14    // Frame List Base Address
#define EHCI_ASYNCLISTADDR      0x18    // Next Asynchronous List Address
#define EHCI_CONFIGFLAG         0x40    // Configured Flag Register

#define EHCI_PORTSC             0x44    // Port Status/Control
  // EHCI_PORTSC bit definitions
    #define EHCI_CURRENTCONNECTSTATUS   BIT0
    #define EHCI_CONNECTSTATUSCHANGE    BIT1
    #define EHCI_PORTENABLE             BIT2
    #define EHCI_PORTENABLESTATUSCHANGE BIT3
    #define EHCI_OVERCURRENTACTIVE      BIT4
    #define EHCI_OVERCURRENTCAHGE       BIT5
    #define EHCI_FORCEPORTRESUME        BIT6
    #define EHCI_SUSPEND                BIT7
    #define EHCI_PORTRESET              BIT8
	#define EHCI_LINE_STATUS            (BIT10 | BIT11)
    #define EHCI_DMINUSBIT              BIT10
    #define EHCI_DPLUSBIT               BIT11
    #define EHCI_PORTPOWER              BIT12
    #define EHCI_PORTOWNER              BIT13
    #define EHCI_WKCNNT_E               BIT20   // Wake On Connect Enable
    #define EHCI_WKDSCNNT_E             BIT21   // Wake On Disconnect Enable
    #define EHCI_WKOC_E                 BIT22   // Wake On Over-current Enable

//EHCI Extended Capability registers
//------------------------------------------------
#define EHCI_LEGACY_REG     0x00
#define EHCI_LEGACY_CTRL_STS_REG    0x04

//USB Legacy Suport Extended Capibility
//------------------------------------------------
#define EHCI_CAP_ID         0x000000FF
#define EHCI_NEXT_EECP      0x0000FF00
#define EHCI_HC_BIOS        0x00010000
#define EHCI_HC_OS          0x01000000

//USB Legacy Suport Control/Status
//------------------------------------------------
#define EHCI_SMI                        0x001
#define EHCI_ERROR_SMI                  0x002
#define EHCI_PORT_CHANGE_SMI            0x004
#define EHCI_FRAME_LIST_ROLL_OVER_SMI   0x008
#define EHCI_HOST_SYSTEM_ERROR_SMI      0x010
#define EHCI_ASYNC_ADVANCE_SMI          0x020
#define EHCI_OWNERSHIP_CHANGE_SMI       0x2000
#define EHCI_SMI_PCI_COMMAND            0x4000
#define EHCI_SMI_ON_BAR                 0x8000
#define EHCI_SMI_STS                    0x0001
#define EHCI_ERROR_SMI_STS              0x0002
#define EHCI_PORT_CHANGE_STS            0x0004
#define EHCI_FRAME_LIST_ROLL_OVER_SMI_STS 0x0008
#define EHCI_SMI_HOST_SYSTEM_ERROR      0x0010
#define EHCI_ASYNC_ADVANCE_SMI_STS      0x0020
#define EHCI_OWNERSHIP_CHANGE_SMI_STS   0x2000
#define EHCI_SMI_PCI_COMMAND_STS        0x4000
#define EHCI_SMI_ON_BAR_STS             0x8000

#define EHCI_PCI_TRAP_SMI       EHCI_OWNERSHIP_CHANGE_SMI
#define EHCI_ALL_SMI            0x0E03F
#define EHCI_ALL_LEGACY_SMI     0x0003F

//-------------------------------------------------------------------------
// Descriptor structure used to store TD and ED addresses
//-------------------------------------------------------------------------
//
// Bit definitions for a generic pointer
//
#define EHCI_TERMINATE      1

#pragma pack(push, 1)

/**
    This is EHCI queue TD data structure used to perform
    non-isochronous transaction in EHCI based HC

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      dNextqTDPtr DWORD   Pointer to next qTD
      dAltNextqTDPtr  DWORD   Pointer to alternate next qTD
      dToken      DWORD   Token double word
      dBufferPtr0 DWORD   Buffer pointer page 0
      dBufferPtr1 DWORD   Buffer pointer page 1
      dBufferPtr2 DWORD   Buffer pointer page 2
      dBufferPtr3 DWORD   Buffer pointer page 3
      dBufferPtr4 DWORD   Buffer pointer page 4

**/

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
// For 64bit data structure
    UINT32      dReserved[8];
#endif
} EHCI_QTD;


/**
    This is EHCI queue head data structure used to perform
    non-isochronous transaction in EHCI based HC

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      dLinkPointer    DWORD   Pointer to the next queue head
      dEndPntCharac   DWORD   Endpoint characteristics settings
      dEndPntCap  DWORD   Endpoint capability settings
      dCurqTDPtr  DWORD   Pointer to current qTD
      dNextqTDPtr DWORD   Pointer to next qTD
      dAltNextqTDPtr  DWORD   Pointer to alternate next qTD
      dToken      DWORD   Token double word
      dBufferPtr0 DWORD   Buffer pointer page 0
      dBufferPtr1 DWORD   Buffer pointer page 1
      dBufferPtr2 DWORD   Buffer pointer page 2
      dBufferPtr3 DWORD   Buffer pointer page 3
      dBufferPtr4 DWORD   Buffer pointer page 4

**/

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
// For 64bit data structure
    UINT32      dReserved[8];
#endif
	UINT8		bCallBackIndex;
    EHCI_QTD    *fpFirstqTD;
    UINT8       bActive;
    UINT8       bErrorStatus;
    UINT8       *fpDevInfoPtr;
    UINT8       aDataBuffer[8];
    UINT32		dTokenReload;
    UINT16      Interval;
    UINT8       Pad[31-2*sizeof(void*)];    // sizeof(EHCI_QH)should be divisible by 32
} EHCI_QH;                          // because of 32 bit pointers; the size of
                                    // the structure has to be aligned on a 32-Byte boundary
/*
 * 
 * Isochronous transfer related definitions
 * 
 */
 
// EHCI iTD max data size, EHCI Specification 1.0, section 3.3.3
//-------------------------------------------------------------------------
#define EHCI_ISOC_MAX_MULT 3
#define EHCI_ISOC_MAX_PACKET_SIZE 1024
#define EHCI_ISOC_MAX_REC_DATA_SIZE EHCI_ISOC_MAX_PACKET_SIZE*EHCI_ISOC_MAX_MULT

#define EHCI_ISOC_MAX_TRANSACTION_RECORDS 8
#define EHCI_ISOC_MAX_ITD_DATA_SIZE EHCI_FRAMELISTSIZE*EHCI_ISOC_MAX_REC_DATA_SIZE*EHCI_ISOC_MAX_TRANSACTION_RECORDS
#define BUFFER_SIZE_PAGES EFI_SIZE_TO_PAGES(EHCI_ISOC_MAX_ITD_DATA_SIZE)
#define EHCI_ITD_PAGE_MAX_OFFSET (0x1000-EHCI_ISOC_MAX_REC_DATA_SIZE)

// Isochronous transfer descriptor
//-------------------------------------------------------------------------
typedef struct _EHCI_ITD {
    UINT32  NextLinkPointer;
    UINT32  ControlStatus[8];
    UINT32  BufferPointer[7];
#if EHCI_64BIT_DATA_STRUCTURE    
    UINT32  ExtBufferPointer[7];
    UINT32  Padding[9];
#endif
} EHCI_ITD;

#pragma pack(pop)

#define EHCI_QUEUE_HEAD     2   // Queue head id

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

//-------------------------------------------------------------------------
// Descriptor structure used to store qTD and QH addresses
//-------------------------------------------------------------------------
typedef struct {
    EHCI_QH     *PeriodicQh;                
	EHCI_QH     *fpQHRepeat;
	EHCI_QTD    *fpqTDRepeat;
} EHCI_DESC_PTRS;

#endif      // __EHCI_H

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
