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

/** @file AmiUsbRtCcid.h
    AMI Usb CCID driver definitions

**/

#ifndef _EFI_CCID_RT_H
#define _EFI_CCID_RT_H

#include    <Token.h>
#include    "UsbDef.h"
#include    <AmiDxeLib.h>

//CCID APIs
#define    USB_CCID_SMARTCLASSDESCRIPTOR   0x000
#define    USB_CCID_ATR                    0x001
#define    USB_CCID_POWERUP_SLOT           0x002
#define    USB_CCID_POWERDOWN_SLOT         0x003
#define    USB_CCID_GET_SLOT_STATUS        0x004
#define    USB_CCID_XFRBLOCK               0x005
#define    USB_CCID_GET_PARAMETERS         0x006

//SMART CARD PROTOCOL APIs
#define    USB_SMART_CARD_READER_CONNECT       0x007
#define    USB_SMART_CARD_READER_DISCONNECT    0x008
#define    USB_SMART_CARD_READER_STATUS        0x009
#define    USB_SMART_CARD_READER_TRANSMIT      0x00A
#define    USB_SMART_CARD_READER_CONTROL       0x00B
#define    USB_SMART_CARD_READER_GET_ATTRIB    0x00C

typedef     struct  _ICC_DEVICE ICC_DEVICE;
typedef     struct  _SMARTCLASS_DESC SMARTCLASS_DESC;
typedef     struct  _ATR_DATA ATR_DATA;

#define    MAX_ATR_LENGTH                32
// ATR data dequence is T0, TA1, TB1, TC1, TD1, TA2, TB2, TC2, TD2, TA3... etcs

//    
//         equates for bVoltageSupport
//
#define    AUTO_VOLT                    0x00
#define    VOLT_5                       0x01
#define    VOLT_3                       0x02
#define    VOLT_18                      0x04


//    
//         equates for dwFeatures
//
#define    AUTO_PARAMETER_CONFIG          0x02
#define    AUTO_ACTIVATION_OF_ICC         0x04
#define    AUTO_ACTIVATION_VOLT_SELECTION 0x08
#define    AUTO_ICC_CLOCK_FREQ            0x10
#define    AUTO_BAUD_RATE_SELECTION       0x20
#define    AUTO_PPS_NEGOTIATION_CCID      0x40
#define    AUTO_PPS_NEGOTIATION_ACTIVE    0x80
#define    STOP_CLOCK_MODE                0x100
#define    NAD_OTHER_THAN_00              0x200
#define    AUTO_IFSD_EXCHANGE             0x400
#define    CHARACTER_LEVEL_EXCHANGE       0x00000
#define    TDPU_LEVEL_EXCHANGE            0x10000
#define    SHORT_ADPU_LEVEL_EXCHANGE      0x20000
#define    EXT_ADPU_LEVEL_EXCHANGE        0x40000

#define    PC_TO_RDR_ICCPOWERON           0x62
#define    PC_TO_RDR_ICCPOWEROFF          0x63
#define    PC_TO_RDR_GETSLOTSTATUS        0x65
#define    PC_TO_RDR_XFRBLOCK             0x6F
#define    PC_TO_RDR_GETPARAMETERS        0x6C
#define    PC_TO_RDR_RESETPARAMETERS      0x6D
#define    PC_TO_RDR_SETPARAMETERS        0x61
#define    PC_TO_RDR_ESCAPE               0x6B
#define    PC_TO_RDR_ICCCLOCK             0x6E
#define    PC_TO_RDR_T0APDU               0x6A
#define    PC_TO_RDR_SECURE               0x69
#define    PC_TO_RDR_MECHANICAL           0x71
#define    PC_TO_RDR_ABORT                0x72
#define    PC_TO_RDR_SETDATARATEANDCLOCK  0x73


#define    RDR_TO_PC_DATABLOCK            0x80
#define    RDR_TO_PC_SLOTSTATUS           0x81
#define    RDR_TO_PC_PARAMETERS           0x82
#define    RDR_TO_PC_ESCAPE               0x83
#define    RDR_TO_PC_DATARATEANDCLOCK     0x84

#define RDR_TO_PC_NOTIFYSLOTCHANGE        0x50
#define RDR_TO_PC_HARDWAREERROR           0x51
//
// BIT definition for ConfiguredStatus
//
#define ICCPRESENT                        0x01
#define VOLTAGEAPPLIED                    0x02
#define BAUDRATEPROGRAMMED                0x04
#define ATRDATAPRESENT                    0x08
#define CARDREMOVED                       0x40
#define CONFIGFAILED                      0x80

#define IBLOCK      0x00
#define RBLOCK      0x80
#define SBLOCK      0xC0


// Codes for access mode
#define SCARD_AM_READER                   0x0001 // Exclusive access to reader
#define SCARD_AM_CARD                     0x0002 // Exclusive access to card

// Codes for card action
#define SCARD_CA_NORESET                  0x0000 // Don’t reset card
#define SCARD_CA_COLDRESET                0x0001 // Perform a cold reset
#define SCARD_CA_WARMRESET                0x0002 // Perform a warm reset
#define SCARD_CA_UNPOWER                  0x0003 // Power off the card
#define SCARD_CA_EJECT                    0x0004 // Eject the card

// Protocol types
#define SCARD_PROTOCOL_UNDEFINED          0x0000
#define SCARD_PROTOCOL_T0                 0x0001
#define SCARD_PROTOCOL_T1                 0x0002
#define SCARD_PROTOCOL_RAW                0x0004

// Smart Card Reader Status
#define SCARD_UNKNOWN                     0x0000
#define SCARD_ABSENT                      0x0001
#define SCARD_INACTIVE                    0x0002
#define SCARD_ACTIVE                      0x0003


//CCID Class specific request
#define CCID_CLASS_SPECIFIC_ABORT                   (0x01 << 8) | USB_REQ_TYPE_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE     
#define CCID_CLASS_SPECIFIC_GET_CLOCK_FREQUENCIES   (0x02 << 8) | USB_REQ_TYPE_INPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE    
#define CCID_CLASS_SPECIFIC_GET_DATA_RATES          (0x03 << 8) | USB_REQ_TYPE_INPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE     

typedef enum {
  T0_PROTOCOL,
  T1_PROTOCOL    
} TRANSMISSION_PROTOCOL;

//I-Block information
#define NSBIT       0x40
#define MBIT       0x20

// S-Block Information
#define     RESYNCH_REQUEST     0xC0
#define     IFS_REQUEST         0xC1
#define     ABORT_REQUEST       0xC2
#define     WTX_REQUEST         0xC3

#define     WTX_RESPONSE        0xE3
#define     ABORT_RESPONSE      0xE2
#define     IFS_RESPONSE        0xE1
#define     RESYNCH_RESPONSE    0xE0


#define     I_BLOCK_MATCH                       0x01
#define     I_BLOCK_RESEND                      0x02

#define     SEND_R_BLOCK_0                      0x03
#define     SEND_R_BLOCK_1                      0x04

#define     GET_DATA_T1_CHAR                    0x05

#define     T1_CHAR_CMD_PHASE                   0x06
#define     T1_CHAR_DATA_PHASE                  0x07
#define     RESEND_BLOCK                        0x08


    
#define     BLOCK_TRANSMISSION_TERMINATE        0xFF
#define     BLOCK_TRANSMISION_SUCCESS           0x00


//    Time extension Calculation (worst case scenario assumed)

//1ETU = F/D * 1/f =  2048/1*1/4Mhz = 0.000512sec

// Use Maximum value for BWI which is 9 and minimum Freq 4Mhz for calculation
//BWT = 11 ETU + (2BWI * 960 * 372/Clock Frequency) = 11ETU + ( 2* 9 * 960* 372/4000000) = 11+1.607040 = 12ETU = 0.00614sec = 6msec

//WWT = 960 * WI * F / Clock Frequency
// Character Level
//The "waiting time" (see 7.2) shall be: WT =WI x960x Fi/f = 255 * 960 * 2048/4000000 = 125ETU = 0.064sec = 64msec

#define        INITWAITTIME            64    // 64msec worst case assuming T0

#pragma pack(push, 1)

struct    _ATR_DATA {
    UINT8        TS;                 
    UINT8        T0;                 
    BOOLEAN      TA1Present;             
    UINT8        TA1;
    BOOLEAN      TB1Present;
    UINT8        TB1;
    BOOLEAN      TC1Present;
    UINT8        TC1;
    BOOLEAN      TD1Present;
    UINT8        TD1;

    BOOLEAN      TA2Present;
    UINT8        TA2;
    BOOLEAN      TB2Present;
    UINT8        TB2;
    BOOLEAN      TC2Present;
    UINT8        TC2;
    BOOLEAN      TD2Present;
    UINT8        TD2;

    BOOLEAN      TD15Present;
    UINT8        TD15;
    BOOLEAN      TA15Present;
    UINT8        TA15;

    UINT8        NumberofHystoricalBytes;    

};

#define ICC_DEVICE_SIG       SIGNATURE_32('I','C','C','D') 

struct _ICC_DEVICE {

    UINT32          Signature;
    EFI_HANDLE      ChildHandle;
    EFI_HANDLE      SCardChildHandle;
    UINT8           Slot;                   // slot number
    UINT8           SlotConnectStatus;
    UINT8           RawATRData[32];
    ATR_DATA        AtrData;                // Processed ATR data
    UINT8           TransmissionProtocolSupported; // BitMap
    UINT8           NumofTransmissionProtocolSupported; // Count 1 based
    UINT8           Current;
    UINT8           bStatus;            // Holds the status of the last cmd
    UINT8           bError;             // Holds the Slot error value of the last cmd
    UINT8           ConfiguredStatus;   // See the bit defenitions above ICCPresent etc

    UINT16          etu;                // 1etu = F/D * 1/f  (Elementary time unit) This will hold the timing in Micro sec
    UINT16          GlobalFi;           // Clock Rate Conversion Integer
    UINT8           GlobalDi;           // Baude rate Adjustment
    UINT8           GlobalFmax;         // Max Frequency
    UINT8           TransmissionProtocol; // Higher nibble what is supported, Lower nibble what is selected

    UINT8           ExtraGuardTime;     // N = TC1 
    UINT8           BWI;                // From TB of T=1
    UINT8           CWI;                // From TB of T=1
    UINT8           BWT;                // Max. Block width time
    UINT8           IFSC;               // Default Information Filed Size (IFSC)
    UINT8           IFSD;               //
    UINT8           NAD;                 

    UINT8           EpilogueFields;     // First TC for T=1

    BOOLEAN        SpecificMode;        // BIT7 set in TA2

    UINT8           ClassABC;           // Updated from T15->TA
    UINT8           StopClockSupport;   // Updated from T15->TA 00: Not supported, 1 : State L, 2 : State H, 3 : No preference
    
    UINT8           bClockStatus;       // PlaceHolder for Clockstatus from the last RDR_to_PC_SlotStatus cmd
    UINT8           bChainParameter;    // PlaceHolder for ChainParameter from the last RDR_to_PC_DataBlock

    // This structure should be same as RDR_TO_PC_PARAMETERS_T1_STRUC. Don't add or remove the next 8 bytes
    UINT8           bProtocolNum;        // PlaceHolder for bProtocolNum from RDR_to_PC_Parameters
    UINT8           bmFindIndex;
    UINT8           bmTCCKST;
    UINT8           bGuardTime;
    UINT8           bWaitingInteger;
    UINT8           bClockStop;
    UINT8           bIFSC;
    UINT8           nNadValue;
    //End

    // From GetDataRateAndClockFrequency
    UINT32          dwClockFrequency;
    UINT32          dwDataRate;

    UINT32          WTwaittime;          // (in ETU units) Based on T0 BWT/WWT time will be updated once ATR data is available 
    UINT32          WaitTime;            // Final Wait time used in msec

    // Information stored for TDPU transmission
    UINT8           NaSInterface;        // This holds the last N(S) value transmitted.
    UINT8           NaSCard;             // This holds the last N(S) received from card
    UINT8           RBlockCounter;
    UINT8           bIFSD;              // Max. size of the information field of blocks that can be received by the interface device.
    BOOLEAN         Chaining;           // Indicates M bit is set or not

    BOOLEAN         T1CharCmdDataPhase; // TRUE for Cmd Phase/False for Data Phase

    DLINK           ICCDeviceLink;       // Linked to ICCDeviceList
    
};

struct _SMARTCLASS_DESC{
    UINT8           bDescLength;
    UINT8           bDescType;
    UINT16          bcdCCID;
    UINT8           bMaxSlotIndex;
    UINT8           bVoltageSupport;
    UINT32          dwProtocols;
    UINT32          dwDefaultClock;
    UINT32          dwMaximumClock;
    UINT8           bNumClockSupported;
    UINT32          dwDataRate;
    UINT32          dwMaxDataRate;
    UINT8           bNumDataRatesSupported;
    UINT32          dwMaxIFSD;
    UINT32          dwSynchProtocols;
    UINT32          dwMechanical;
    UINT32          dwFeatures;
    UINT32          dwMaxCCIDMessageLength;
    UINT8           bClassGetResponse;
    UINT8           bClassEnvelope;
    UINT16          wLcdLayout;
    UINT8           bPINSupport;
    UINT8           bMaxCCIDBusySlots;
};

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bPowerSlot;
    UINT16          abRFU;
} PC_TO_RDR_ICCPOWERON_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           abRFU[3];
} PC_TO_RDR_ICCPOWEROFF_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           abRFU[3];
} PC_TO_RDR_GETSLOT_STATUS_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bBWI;
    UINT16          wLevelParameter;
} PC_TO_RDR_XFRBLOCK_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           abRFU[3];
} PC_TO_RDR_GETPARAMETERS_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           abRFU[3];
} PC_TO_RDR_RESETPARAMETERS_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bProtocolNum;
    UINT8           abRFU[2];
    UINT8           bmFindDindex;
    UINT8           bmTCCKST0;
    UINT8           bGuardTimeT0;
    UINT8           bWaitingIntergerT0;
    UINT8           bClockStop;
} PC_TO_RDR_SETPARAMETERS_T0_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bProtocolNum;
    UINT8           abRFU[2];
    UINT8           bmFindDindex;
    UINT8           bmTCCKST1;
    UINT8           bGuardTimeT1;
    UINT8           bWaitingIntergersT1;
    UINT8           bClockStop;
    UINT8           bIFSC;
    UINT8           bNadValue;
} PC_TO_RDR_SETPARAMETERS_T1_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           abRFU[3];
} PC_TO_RDR_ESCAPE_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bClockCommand;
    UINT8           abRFU[2];
} PC_TO_RDR_ICCCLOCK_STRUC;

typedef struct {
    UINT8            bMessageType;
    UINT32            dwLength;
    UINT8            bSlot;
    UINT8            bSeq;
    UINT8            bmChanges;
    UINT8            bClassGetResponse;
    UINT8            bClassEnvelope;
} PC_TO_RDR_T0APDU_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bBWI;
    UINT16          wLevelParameter;
} PC_TO_RDR_SECURE_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bFunction;
    UINT8           abRFU[2];
} PC_TO_RDR_MECHANICAL_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           abRFU[3];
} PC_TO_RDR_ABORT_STRUC;


typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           abRFU[3];
    UINT32          dwCloclFrequency;
    UINT32          dwDataRate;
} PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;        // Size of abData
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bStatus;
    UINT8           bError;    
    UINT8           Data;            // Usage depends on diffeent Message type
} RDR_HEADER;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;        // Size of abData
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bStatus;
    UINT8           bError;
    UINT8           bChainParameter;
} RDR_TO_PC_DATABLOCK_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;        
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bStatus;
    UINT8           bError;
    UINT8           bClockStatus;
} RDR_TO_PC_SLOTSTATUS_STRUC;

typedef struct {
    UINT8           bmFindDindex;
    UINT8           bmTCCKST0;
    UINT8           bGuardTimeT0;
    UINT8           bWaitingIntergerT0;
    UINT8           bClockStop;
} PROTOCOL_DATA_T0;

typedef struct {
    UINT8           bmFindDindex;
    UINT8           bmTCCKST1;
    UINT8           bGuardTimeT1;
    UINT8           bWaitingIntergersT1;
    UINT8           bClockStop;
    UINT8           bIFSC;
    UINT8           bNadValue;
} PROTOCOL_DATA_T1;

typedef struct {
    RDR_HEADER          Header;
    PROTOCOL_DATA_T0    ProtocolData;
} RDR_TO_PC_PARAMETERS_T0_STRUC;

typedef struct {
    RDR_HEADER          Header;
    PROTOCOL_DATA_T1    ProtocolData;
} RDR_TO_PC_PARAMETERS_T1_STRUC;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;        // Size of abData
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bStatus;
    UINT8           bError;
    UINT8           bRFU;
} RDR_TO_PC_ESCAPE_STATUS;

typedef struct {
    UINT8           bMessageType;
    UINT32          dwLength;        // Should be 8
    UINT8           bSlot;
    UINT8           bSeq;
    UINT8           bStatus;
    UINT8           bError;
    UINT8           bRFU;
    UINT32          dwClockFrequency;
    UINT32          dwDataRate;
} RDR_TO_PC_DATARATEANDCLOCKFREQUENCY_STRUC;

#pragma pack(pop)

VOID
USBCCIDInitialize(
    VOID
);

UINT8
USBCCIDCheckForDevice (
    DEV_INFO    *fpDevInfo,
    UINT8       bBaseClass,
    UINT8       bSubClass,
    UINT8       bProtocol
);

UINT8
USBCCIDDisconnectDevice (
    DEV_INFO    *fpDevInfo
);

DEV_INFO*
USBCCIDConfigureDevice (
    HC_STRUC        *fpHCStruc,
    DEV_INFO        *fpDevInfo,
    UINT8           *fpDesc,
    UINT16          wStart,
    UINT16          wEnd
);

UINT32
USBCCIDIssueBulkTransfer (
    DEV_INFO*   fpDevInfo, 
    UINT8       bXferDir,
    UINT8*      fpCmdBuffer, 
    UINT32      dSize
);

UINT32
USBCCIDIssueControlTransfer(
    DEV_INFO*   fpDevInfo,     
    UINT16      wRequest,
    UINT16      wIndex,
    UINT16      wValue,
    UINT8       *fpBuffer,
    UINT16      wLength
);

EFI_STATUS
PCToRDRXfrBlock (
    IN DEV_INFO             *fpDevInfo,
    IN ICC_DEVICE           *fpICCDevice,
    IN UINT32               CmdLength,
    IN UINT8                *CmdBuffer,
    IN UINT8                BlockWaitingTime,
    IN UINT16               LevelParameter    

);

EFI_STATUS
ConstructBlockFrame(
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice,
    IN UINT8            Nad,
    IN UINT8            PCB,
    IN UINT32           InfLength,
    IN UINT8            *InfBuffer,
    OUT UINT8           *wLevelParameter,
    OUT UINT32          *BlockFrameLength,
    OUT UINT8           **BlockFrame
);

UINT8   
HandleReceivedBlock (
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice,
    IN UINT32           OriginalBlockFrameLength,
    IN UINT8            *OriginalBlockFrame,
    IN UINT32           SentBlockFrameLength,
    IN UINT8            *SentBlockFrame,
    IN UINT8            *ReceivedBlockFrame
);

EFI_STATUS
TxRxT1TDPUChar (
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice,
    IN UINT32           CmdLength,
    IN UINT8            *CmdBuffer,
    IN UINT8            ISBlock,
    OUT UINT32          *ResponseLength,
    OUT UINT8           *ResponseBuffer
);

EFI_STATUS
TxRxT1Adpu (
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice,
    IN UINT32           CmdLength,
    IN UINT8            *CmdBuffer,
    OUT UINT32          *ResponseLength,
    OUT UINT8           *ResponseBuffer
);

EFI_STATUS
PCToRDRGetSlotStatus(
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice
);

EFI_STATUS
RDRToPCSlotStatus(
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice
);

EFI_STATUS
PCToRDRGetParameters(
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice
);

EFI_STATUS
RDRToPCParameters(
    IN DEV_INFO           *fpDevInfo,
    IN ICC_DEVICE        *fpICCDevice
);

EFI_STATUS
RDRToPCDataBlock(
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice,
    IN OUT UINT32       *dwLength,
    OUT UINT8           *Buffer

);

EFI_STATUS
ICCInsertEvent(
    DEV_INFO    *fpDevInfo,
    UINT8       Slot
);

EFI_STATUS
ICCRemovalEvent(
    IN DEV_INFO    *fpDevInfo,
    IN UINT8        Slot
);

VOID
ICC_SmiQueuePut(
    IN VOID    *d
);

VOID 
PrintPCParameters(
    IN UINT8   *Data
);

TRANSMISSION_PROTOCOL GetDefaultProtocol (
    IN ICC_DEVICE        *fpICCDevice
);

VOID
CalculateLRCChecksum (
    IN UINT8       *BlockFrame, 
    IN UINT32      BlockFrameLength
);

EFI_STATUS
PCtoRDRIccPowerOff(
    IN DEV_INFO         *fpDevInfo,
    IN ICC_DEVICE       *fpICCDevice
);

VOID
PrintDescriptorInformation (
    SMARTCLASS_DESC *fpCCIDDesc
);

VOID
USBCCIDAPISmartClassDescriptorSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBCCIDAPIAtrSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBCCIDAPIPowerupSlotSMM (
    IN OUT URP_STRUC *fpURP

);

VOID
USBCCIDAPIPowerDownSlotSMM (
    IN OUT URP_STRUC *fpURP

);

VOID
USBCCIDAPIGetSlotStatusSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBCCIDAPIXfrBlockSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBCCIDAPIGetParametersSMM (
    IN OUT URP_STRUC *fpURP

);

EFI_STATUS
ConfigureCCID(
    DEV_INFO          *fpDevInfo,
    ICC_DEVICE        *fpICCDevice
);

VOID
USBSCardReaderAPIConnectSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBSCardReaderAPIDisConnectSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBSCardReaderAPIStatusSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBSCardReaderAPITransmitSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBSCardReaderAPIControlSMM (
    IN OUT URP_STRUC *fpURP
);

VOID
USBSCardReaderAPIGetAttribSMM (
    IN OUT URP_STRUC *fpURP
);

ICC_DEVICE*
GetICCDevice(
    DEV_INFO        *fpDevInfo, 
    UINT8            Slot
);

VOID
PrintTimingInfo(
    ICC_DEVICE    *fpICCDevice
);

VOID
PrintATRData(
    UINT8            *ATRData
);

UINT8
FindNumberOfTs(
    UINT8    Data
);

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
