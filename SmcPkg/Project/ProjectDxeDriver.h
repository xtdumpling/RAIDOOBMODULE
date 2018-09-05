//****************************************************************************
//**                                                                        **
//**           Copyright(c) 1993-2017 Super Micro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fixed: Pressing Ctrl+Home to enter BIOS recovery mode.
//    Reason   : 
//    Auditor  : Yenhsin Cho
//    Date     : Apr/07/2017
//
//****************************************************************************
#include <Protocol/SmbiosElogSupport.h>

#pragma pack(push,1)

typedef struct {
    UINT32      Correctable:1;
    UINT32      UnCorrectable:1;
    UINT32      DimmNum:2;              // DIMM 0-3
    UINT32      Socket:2;               // Socket 0-3
    UINT32      MemoryController:1;     // MemoryController 0-1
    UINT32      Channel:2;              // Channel 0-3
    UINT32      Reserved:23;
}NB_ECC_ERR;

typedef struct {
    UINT8       Bus;
    UINT8       Dev;
    UINT8       Fun;
    UINT16      DeviceStatus;
    UINT32      Correctable:1;
    UINT32      NonFatal:1;
    UINT32      Fatal:1;
    UINT32      ParityErr:1;  // PERR
    UINT32      SystemErr:1;  // SERR
    UINT32      Reserved:27;
} NB_PCIE_ERR;

typedef union {
    NB_ECC_ERR  EccMsg;
    NB_PCIE_ERR PcieMsg;
} ERROR_MSG;

typedef enum {
    NO_ERROR = 0,
    ECC_ERROR,
    NB_PCIE_ERROR,
    SB_PCIE_ERROR,
    SB_PCI_ERROR,
    ERROR_MAX
} HARDWARE_ID;

typedef struct _NB_ERROR_INFO {
    EFI_GPNV_RUNTIME_ERROR      Header;     // standard header ...error type
    HARDWARE_ID                 HardwareId; // identify the hardware
    ERROR_MSG                   Msg;        // hardware-specific error information
} ERROR_INFO;

typedef struct _KEY_INFO_FOR_STIEX_REG {
    EFI_KEY_DATA             KeyData;
    EFI_KEY_NOTIFY_FUNCTION  NotifyFunc;
}KEY_INFO_FOR_STIEX_REG;

typedef struct _STIEX_INFO_FOR_UNREG {
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *pStiEx;
  VOID                               *pNotifyHandle;
}STIEX_INFO_FOR_UNREG;

#pragma pack(pop)

EFI_STATUS
StiExNotifyFuncForCtrlHome (
  EFI_KEY_DATA  *pKeyData
  );

