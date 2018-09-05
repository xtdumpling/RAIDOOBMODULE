/**
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
*/

/** @file HddSmartCommon.h
    This file contains the Includes, Definitions, typedefs,
    Variable and External Declarations, Structure and
    function prototypes needed for the different module parts in the 
    HddSmart driver

**/

#ifndef _AMI_HDDSMART_COMMON_
#define _AMI_HDDSMART_COMMON_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
#include <Protocol/DiskInfo.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#include <Protocol/AmiHddSmartInit.h>
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
#include <Protocol/AmiIdeBus.h>
#else
#define MASTER_DRIVE		0
#define SLAVE_DRIVE		1
#define PRIMARY_CHANNEL		0
#define SECONDARY_CHANNEL	1
#endif

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
#include <Protocol/AmiAhciBus.h>
#else
typedef struct _AHCI_ATAPI_COMMAND{
    UINT8           Ahci_Atapi_Command[0x10];
} AHCI_ATAPI_COMMAND;

typedef struct{
    VOID                        *Buffer;
    UINT32                      ByteCount;
    UINT8                       Features;
    UINT8                       FeaturesExp;
    UINT16                      SectorCount;
    UINT8                       LBALow;
    UINT8                       LBALowExp;
    UINT8                       LBAMid;
    UINT8                       LBAMidExp;
    UINT8                       LBAHigh;
    UINT8                       LBAHighExp;
    UINT8                       Device;
    UINT8                       Command;
    UINT8                       Control;
    AHCI_ATAPI_COMMAND          AtapiCmd;
}COMMAND_STRUCTURE;
#endif

//---------------------------------------------------------------------------

typedef struct _AMI_SMART_INTERFACE{

    UINT8                             Revision;
    UINT32                            Signature;
    AMI_HDD_SMART_SELF_TEST           SmartSelfTest;
    AMI_HDD_SMART_SELF_TEST_STATUS    SmartSelfTestStatus;
    AMI_HDD_SMART_RETURN_STATUS       SmartReturnStatus;
    AMI_HDD_SEND_SMART_COMMAND        SendSmartCommand;
    AMI_HDD_SMART_READ_DATA           SmartReadData;
    AMI_HDD_SMART_WRITE_DATA          SmartWriteData;
    AMI_HDD_SMART_ABORT_SELF_TEST     SmartAbortSelfTest;
    AMI_HDD_SMART_GET_SMART_DATA      SmartGetSmartData;
    VOID                              *BusInterface;
    UINT8                             ShortPollingTime;
    UINT16                            ExtPollingTime;
    AMI_STORAGE_BUS_INTERFACE         BusInterfaceType;
} AMI_SMART_INTERFACE;

#pragma pack(1)
typedef struct _NVME_SMART_HEALTH_INFO {
    UINT8    CriticalWarning;
    UINT8    CompositeTemp[2];
    UINT8    AvailableSpare;
    UINT8    AvailSpareThreshold;
    UINT8    PercentUsed;
    UINT8    Reserved[26];
    UINT8    DataUnitsRead[16];
    UINT8    DataUnitsWritten[16];
    UINT8    HostReadCmds[16];
    UINT8    HostWriteCmds[16];
    UINT8    CtrlBusyTime[16];
    UINT8    PowerCycle[16];
    UINT8    PowerOnHours[16];
    UINT8    UnsafeShutdowns[16];
    UINT8    MediaDataIntegrityErrors[16];
    UINT8    NoofErrorInfoEntry[16];
    UINT8    WriteCompositeTempTime[4];
    UINT8    CriticalCompositeTempTime[4];
    UINT8    TempSensor1[2];
    UINT8    TempSensor2[2];
    UINT8    TempSensor3[2];
    UINT8    TempSensor4[2];
    UINT8    TempSensor5[2];
    UINT8    TempSensor6[2];
    UINT8    TempSensor7[2];
    UINT8    TempSensor8[2];
    UINT8    Reserved1[296];
} NVME_SMART_HEALTH_INFO;
#pragma pack()

#define NVME_SMART_HEALTH_IDENTIFY_MASK    0x1F

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

/**
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
*/

