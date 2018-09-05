//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.11
//    Bug Fix:  Add WDT timer out value selection feature.
//    Reason:   
//    Auditor:  Jimmy Chiu (Refer Greenlow - Mark Chen)
//    Date:     May/05/2017
//
//  Rev. 1.10
//    Bug Fix:  Modify location of graphic mode progress-code.
//    Reason:   
//    Auditor:  YuLin Yang
//    Date:     Mar/17/2017
//
//  Rev. 1.09
//    Bug Fix:  Early Graphic Logo Support.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/14/2017
//
//  Rev. 1.08
//    Bug Fix:  Fix WDT no function.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Sep/10/2016
//
//  Rev. 1.07
//    Bug Fix:  Chenge 0x30 0x70 0x3a to ready to boot and later than full smbios.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/23/2016
//
//  Rev. 1.06
//    Bug Fix:  Fix BMC WEB can't show LAN MAC.
//    Reason:   First 0x30 0x20 in PEI but pcie device not ready in PEI, so add second in DXE.
//    Auditor:  Kasber Chen
//    Date:     Aug/02/2016
//
//  Rev. 1.05
//    Bug Fix:  Review BMC OEM command
//    Reason:   Rollback BMC OEM command, add SMC_IPMICmd30_20, SMC_IPMICmd30_99(default disabled)
//    Auditor:  Jimmy Chiu
//    Date:     Jul/28/2016
//
//  Rev. 1.04
//    Bug Fix:  Add SendBIOSVerBuildTimeDXE to sync BIOS version with SMBIOS Type 0.
//    Reason:   To avoid BIOS version not synchronize problem.
//    Auditor:  Jacker Yeh
//    Date:     Jul/19/2016
//
//  Rev. 1.03
//    Bug Fix:  Review BMC OEM command.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/11/2016
//
//  Rev. 1.02
//    Bug Fix:  Use AMIBCP to disable boot procedure messages displaying.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/21/2016
//
//  Rev. 1.01
//    Bug Fix: Implement SMC_SIMPLETEXTOUT_PROTOCOL.
//    Reason : Use the protocol instead of the previous lib.
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Jan/19/2015
//
//****************************************************************************
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

/** @file DxeIpmiBmcInitialize.c
    IPMI Transport Driver.

**/

//---------------------------------------------------------------------------

#include "IpmiBmc.h"
#include <Library/UefiLib.h>
#if IPMI_BMC_TIME_SYNC_TO_BIOS_TIME_SUPPORT
#include <Protocol/RealTimeClock.h>
#endif
#include <Include/ServerMgmtSetupVariable.h>
#include <Guid/EventGroup.h>
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
#include "IpmiInitHooks.h"
#endif
#if SMCPKG_SUPPORT
#include <AmiDxeLib.h>
#include <Setup.h>
#include "SuperMDxeDriver.h"
#include "SmcSimpleTextOutProtocol.h"
#include "SmcLib.h"
#endif

//---------------------------------------------------------------------------

//
// UTC offset range macros
//
#define MAX_UTC_OFFSET      1440
#define MIN_UTC_OFFSET      0

//
// Global Variables
//
EFI_IPMI_BMC_INSTANCE_DATA       *gIpmiInstance = NULL;
SERVER_MGMT_CONFIGURATION_DATA   gServerMgmtConfiguration;
UINT32                           gAttributes;
EFI_STATUS                       gSetupStatus = EFI_SUCCESS;
EFI_I2C_MASTER_PROTOCOL          *gMasterTransmit;
EFI_SMBUS_HC_PROTOCOL            *gEfiSmbusHcProtocol;

#if SMCPKG_SUPPORT
SMC_SIMPLETEXTOUT_PROTOCOL  *pmSmcSimpleTextOutProtocol = NULL;
SETUP_DATA	SetupData;
UINTN		VariableSize = sizeof(SETUP_DATA);
#endif

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
extern IPMI_OEM_DXE_INIT_HOOK IPMI_OEM_DXE_INIT_HOOK_LIST EndOfIpmiOemDxeInitHookListFunctions;
IPMI_OEM_DXE_INIT_HOOK* gIpmiOemDxeInitHookList[] = {IPMI_OEM_DXE_INIT_HOOK_LIST NULL};
#endif

VOID InitAmiLib(
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  );

EFI_STATUS_CODE_VALUE ErrorBitToStatusCodeMap [] = {
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_FIRMWARE_CORRUPTED,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_BB_CORRUPTED,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_BMC_FRU_CORRUPTED,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SDR_EMPTY,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_IPMB_NOT_RESPONDING,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_BMC_FRU_NOT_ACCESSIBLE,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SDR_REPOSITORY_NOT_ACCESSABLE,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SEL_NOT_ACCESSIBLE
                                                   };

#if IPMI_BMC_TIME_SYNC_TO_BIOS_TIME_SUPPORT
//
// Time Stamp Event - To Synchronize BIOS and BMC Time using Set SelTime command
//
#if USE_PLATFORM_EVENT_MESSAGE

IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST EventMessage = {
 (UINT8) EFI_GENERATOR_ID (BIOS_SOFTWARE_ID),      // GenID:BIOS
 IPMI_EVM_REVISION,      // IPMI_EVM_REVISION
 0x12,      // Sensor Type
 BIOS_SOFTWARE_ID,      // Sensor No
 IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE,      // Event DIR
 05,        // Sensor specific Offset for Time stamp Clock Synch Event.
 00,        // ED2
 0xFF       // ED3
};

#else

IPMI_SEL_EVENT_RECORD_DATA TimeStampEvtRecord = {
 0000,      // Record Id
 IPMI_SEL_SYSTEM_RECORD,      // Record Type
 00000000,  // Time stamp
 (UINT16) EFI_GENERATOR_ID (BIOS_SOFTWARE_ID),    // GenID:BIOS
 IPMI_EVM_REVISION,      // IPMI_EVM_REVISION
 0x12,      // Sensor Type
 BIOS_SOFTWARE_ID,      // Sensor No
 IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE,      // Event DIR
 05,        // Sensor specific Offset for Time stamp Clock Synch Event.
 00,        // ED2
 0xFF       // ED3
};
#endif
#endif

//---------------------------------------------------------------------------

#if SMCPKG_SUPPORT
BOOLEAN
SmcIPMIDXESetting()
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_GUID	gEfiSuperMDriverProtocolGuid = EFI_SUPERM_DRIVER_PROTOCOL_GUID;
    SUPERMDRIVER_PROTOCOL	*gSuperMProtocol;
    SUPERMBOARDINFO	gSuperMBoardInfo;   
    BOOLEAN	bIPMISupport = TRUE;
#if EarlyVideo_SUPPORT
    UINT8	n;
    CHAR8*	strBlank = " ";
    BOOLEAN BootProcedureMsgSupport = TRUE;
#if EarlyVideo_Mode
    UINT8   Attr;
#endif
#endif

    SERVER_IPMI_DEBUG((-1, "[SmcIPMIDXESetting] Start\n"));

// Get BMC present via SuperM protocol

    Status = gBS->LocateProtocol(&gEfiSuperMDriverProtocolGuid, NULL, &gSuperMProtocol);
    if(EFI_ERROR (Status))
        return EFI_SUCCESS;

    Status = gSuperMProtocol->GetSuperMBoardInfo(&gSuperMBoardInfo);
    if(EFI_ERROR (Status))
        return EFI_SUCCESS;

    if(gSuperMBoardInfo.BMC_Present == 0)
        return EFI_SUCCESS;

#if EarlyVideo_SUPPORT
    BootProcedureMsgSupport = SetupData.SmcBootProcedureMsg_Support;

    if (BootProcedureMsgSupport) {
#if EarlyVideo_Mode
        Attr = PcdGet8(PcdSmcColor);
        // Write string to VGA Screen
        for(n=0; n<50; n++)
            pmSmcSimpleTextOutProtocol->OutputString(n, SMC_MAX_ROW-2, Attr, strBlank);

        pmSmcSimpleTextOutProtocol->OutputString(00, SMC_MAX_ROW-2, Attr, "  DXE-IPMI Initialization..");
        pmSmcSimpleTextOutProtocol->OutputString(sizeof("  DXE-IPMI Initialization..")-1, SMC_MAX_ROW-2, Attr, ".");
#else
        // Write string to VGA Screen
        for(n=0; n<50; n++)
            pmSmcSimpleTextOutProtocol->OutputString(n, SMC_MAX_ROW-1, 0x07, strBlank);	

        pmSmcSimpleTextOutProtocol->OutputString(00, SMC_MAX_ROW-1, 0x0F, "  DXE-IPMI Initialization..");
        pmSmcSimpleTextOutProtocol->OutputString(sizeof("  DXE-IPMI Initialization..")-1, SMC_MAX_ROW-1, 0x8F, ".");
#endif
    }
#endif

    SERVER_IPMI_DEBUG((-1, "[SmcIPMIDXESetting] End\n"));	

    return bIPMISupport;
}

VOID
SmcSysInfo(
    EFI_EVENT	Event,
    VOID	*Context
)
{
    SMC_IPMICmd30_20(); //Sent LAN1 MAC at this time
    SMC_IPMICmd30_9E();
    SMC_IPMICmd30_AC();
    gBS->CloseEvent(Event);
}

VOID
SmcIPMIOEM(
    EFI_EVENT	Event,
    VOID	*Context
)
{
    SERVER_IPMI_DEBUG((-1, "[SmcIPMIOEM] Start\n"));
    SMC_IPMICmd30_97(SetupData.SmcWatchDogFn, SetupData.SmcWatchDogTime);
    SmcLibBmcPostCpl(TRUE);
    SMC_IPMICmd30_70_3A();
    gBS->CloseEvent(Event);
    SERVER_IPMI_DEBUG((-1, "[SmcIPMIOEM] End\n"));	
}

VOID
SmcIPMIReadyToBoot()
{
    EFI_EVENT	SmcSysInfoEvt, ReadyToBootEvent;
    EFI_STATUS	Status = EFI_SUCCESS;
    VOID	*SmcSysInfoReg, *Registration;
    
    Status = gBS->CreateEvent(
                        EVT_NOTIFY_SIGNAL,
                        TPL_NOTIFY,
                        SmcSysInfo,
                        NULL,
                        &SmcSysInfoEvt);

    if(!Status){
	Status = gBS->RegisterProtocolNotify(
                        &gBdsAllDriversConnectedProtocolGuid,
                        SmcSysInfoEvt,
                        &SmcSysInfoReg);
    }

    RegisterProtocolCallback(
                  &gAmiTseEventBeforeBootGuid,
                  SmcIPMIOEM,
                  NULL, 
                  &ReadyToBootEvent, 
                  &Registration);
    
    RegisterProtocolCallback(
                  &gAmiLegacyBootProtocolGuid,
                  SmcIPMIOEM,
                  NULL, 
                  &ReadyToBootEvent, 
                  &Registration);
}
#endif

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
/**
    Initiates the DXE hooks of OEM.

    @param BootServices     - Pointer the set of Boot services.
    @param MmioBaseAddress  - Pointer to the MMIO base address.
    @param BaseAddressRange - Pointer to the base address range.

    @return EFI_STATUS      - Return status of the Hook.
*/

EFI_STATUS
IpmiOemDxeInitHook (
  IN CONST EFI_BOOT_SERVICES            *BootServices,
  IN       UINTN                        *MmioBaseAddress,
  IN       UINTN                        *BaseAddressRange
)
{
    EFI_STATUS  Status = EFI_DEVICE_ERROR;
    UINTN       Index;
    for (Index = 0; gIpmiOemDxeInitHookList[Index]; Index++) {
        Status = gIpmiOemDxeInitHookList[Index] (BootServices, MmioBaseAddress, BaseAddressRange);
    }
    return Status;
}
#endif

/**
    Notification function for Virtual address change event.
    Convert all addresses inside IpmiTransportProtocol

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @return VOID

**/

VOID
EFIAPI
DxeIpmiIntializeVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    //
    // Change all addresses inside IpmiTransportProtocol stored locally
    //
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiInstance->IpmiTransport.SendIpmiCommand));
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiInstance->IpmiTransport.GetBmcStatus));
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiInstance->IpmiTransport.SendIpmiCommandEx));
    //
    // Change the address pointed by gIpmiInstance
    //
    gRT->ConvertPointer (
                0,
                (VOID **) &gIpmiInstance);

    return;
}

#if IPMI_BMC_TIME_SYNC_TO_BIOS_TIME_SUPPORT
/**
    This function verifies the leap year

    @param Year year in YYYY format

    @return BOOLEAN
    @retval TRUE - for Leap year
    @retval FALSE - for non Leap year

**/

BOOLEAN
IsLeapYear (
  IN UINT16   Year )
{
    if ( Year % 4 == 0 ) {
        if ( Year % 100 == 0 ) {
            if ( Year % 400 == 0 ) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            return TRUE;
        }
    } else {
        return FALSE;  
    }
}

/**
    This function calculates the total number of days passed
    till the day in a year

    @param Time Contains detailed information about date and time

    @return UINTN Returns number of days passed

**/

UINTN
CalculateNumOfDayPassedThisYear (
  IN  EFI_TIME  Time )
{
    UINTN   Index;
    UINTN   NumOfDays;
    INTN    DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    NumOfDays = 0;
    for (Index = 1; Index < Time.Month; Index++) {
        NumOfDays += DaysOfMonth[(Index - 1)];
        // 
        // Here we need not to consider about the Leap Year. This is already taken in to
        // account in the CountNumOfLeapYears ().
        //
        /*
        if ((IsLeapYear (Time.Year)) && (Index == 2)) {
            NumOfDays += 1;
        }
        */
    }

    NumOfDays += Time.Day;
    return NumOfDays;
}

/**
    This function counts number of leap years from 1970

    @param CurYear Current year 

    @return UINTN Returns number of leap years

**/

UINTN
CountNumOfLeapYears(
  IN UINT16           CurYear )
{
    UINT16      NumOfYear;
    UINT16      BaseYear;
    UINT16      Index;
    UINTN       Count = 0;

    BaseYear = 1970;
    NumOfYear = (UINT16)(CurYear - 1970);
    for (Index = 0; Index <= NumOfYear; Index++) {
        if (IsLeapYear ((UINT16)(BaseYear + Index))) {
            Count += 1;
        }
    }
    return Count;
}
#endif

#if BMC_TIMEZONE_SUPPORT

/**
    1. Validates TimeZone setup variable
    2. Converts time zone string to UTC offset format

    @param TimeZone TimeZone string entered in Setup 

    @retval INT16 UTC offset obtained from Time Zone string

**/

INT16
GetUtcOffsetFromTimeZone (
  OUT  CHAR16      TimeZone[] )
{
    INT16       UtcOffset = 0;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered... TimeZone: %S \n", __FUNCTION__, TimeZone));

    //
    // If setup variable is not found then return Unspecified Time zone
    // and also if 0x7FFF is entered in setup, return Unspecified Time zone
    //
    if ( EFI_ERROR (gSetupStatus) ) {
        return EFI_UNSPECIFIED_TIMEZONE;
    } else if ( ( TimeZone[0] == '0') && ( (TimeZone[1] == 'x') ||\
                (TimeZone[1] == 'X') ) && (TimeZone[2] == '7') && \
                ( (TimeZone[3] == 'f') || (TimeZone[3] == 'F') ) &&\
                ( (TimeZone[4] == 'f') || (TimeZone[4] == 'F') ) &&\
                ( (TimeZone[5] == 'f') || (TimeZone[5] == 'F') ) ) {
        return EFI_UNSPECIFIED_TIMEZONE;
    }

    //
    // Check for special characters
    //
    if ( (TimeZone[0] != '+') && (TimeZone[0] != '-') ) {
        goto ErrorExit;
    } else if ( TimeZone[3] != ':' ) {
        goto ErrorExit;
    }

    //
    // Validate TimeZone[5] range. Range: 0 - 9
    // Converting char to number - 1's position
    //
    if ( (TimeZone[5] >= '0') && (TimeZone[5] <= '9') ) {
        UtcOffset += (TimeZone[5] - 0x30);
        SERVER_IPMI_DEBUG ((EFI_D_INFO," \n After Converting char to number - 1's position UtcOffset: %d \n", UtcOffset));
    } else {
        goto ErrorExit;
    }

    //
    // Validate TimeZone[4] range. Range: 0 - 5
    // Converting char to number - 10's position
    //
    if ( (TimeZone[4] >= '0') && (TimeZone[4] <= '5') ) {
        UtcOffset += ( (TimeZone[4] - 0x30) * 10);
        SERVER_IPMI_DEBUG ((EFI_D_INFO," After Converting char to number - 10's position UtcOffset: %d \n", UtcOffset));
    } else {
        goto ErrorExit;
    }

    //
    // Validate TimeZone[2] range. Range: 0 - 9
    // Converting Hour to minute - 1's position
    //
    if ( (TimeZone[2] >= '0') && (TimeZone[2] <= '9') ) {
        UtcOffset += ( (TimeZone[2] - 0x30) * 60);
        SERVER_IPMI_DEBUG ((EFI_D_INFO," After Converting Hour to minute - 1's position UtcOffset: %d \n", UtcOffset));
    } else {
        goto ErrorExit;
    }

    //
    // Validate TimeZone[1] range. Range: 0 - 2
    // Converting Hour to minute - 10's position
    //
    if ( (TimeZone[1] >= '0') && (TimeZone[1] <= '2') ) {
        UtcOffset += ( (TimeZone[1] - 0x30) * 60 * 10);
        SERVER_IPMI_DEBUG ((EFI_D_INFO," After Converting Hour to minute - 10's position UtcOffset: %d \n", UtcOffset));
    } else {
        goto ErrorExit;
    }

    //
    // UtcOffset should be in the range 0 to 1440
    // check for the range.
    //
    if ( !( (UtcOffset <= MAX_UTC_OFFSET) && \
            (UtcOffset >= MIN_UTC_OFFSET) ) ) {
        goto ErrorExit;
    }
    //
    // Check for sign and form UtcOffset accordingly
    //
    if ( TimeZone[0] == '-' ) { 
        UtcOffset *= -1;
    }
    return UtcOffset;

ErrorExit:
    //
    // Return unspecified time zone.
    //
    return EFI_UNSPECIFIED_TIMEZONE;

}

/**
    Get SEL Time UTC Offset from BMC. Compare both BMC UTC Offset
    and UTC Offset entered in Setup, if they are different, BMC UTC Offset
    is updated to UTC Offset entered in Setup using Set SEL Time UTC Offset
    command

    @param gIpmiInstance Pointer to IPMI BMC instance Data

    @return EFI_STATUS

**/

EFI_STATUS
SetTimeZone (
  IN  EFI_IPMI_BMC_INSTANCE_DATA    *gIpmiInstance )
{
    EFI_STATUS            Status;
    UINT8                 DataSize;
    INT16                 SetupUtcOffset = 0x00;
    INT16                 BmcUtcOffset = 0x00;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...\n", __FUNCTION__));
  
    //
    // Convert Setup TimeZone string to UTC offset in minutes
    //
    SetupUtcOffset = GetUtcOffsetFromTimeZone (gServerMgmtConfiguration.TimeZone);
    SERVER_IPMI_DEBUG ((EFI_D_INFO,"SetupUtcOffset: %x \n", SetupUtcOffset));
    
    if (!EFI_ERROR(gSetupStatus)) {
    //
    // Set default Time zone values - 0x7FFF
    //
    gServerMgmtConfiguration.TimeZone[0] ='0';
    gServerMgmtConfiguration.TimeZone[1] ='x';
    gServerMgmtConfiguration.TimeZone[2] ='7';
    gServerMgmtConfiguration.TimeZone[3] ='F';
    gServerMgmtConfiguration.TimeZone[4] ='F';
    gServerMgmtConfiguration.TimeZone[5] ='F';

    gRT->SetVariable(
        L"ServerSetup",
        &gEfiServerMgmtSetupVariableGuid,
        gAttributes,
        sizeof(SERVER_MGMT_CONFIGURATION_DATA),
        &gServerMgmtConfiguration );
    }
    //
    // Get SEL Time UTC Offset Command
    //
    DataSize = sizeof (BmcUtcOffset);
    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gIpmiInstance->IpmiTransport,
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                EFI_STORAGE_GET_SEL_TIME_UTC_OFFSET,
                NULL,
                0,
                (UINT8*) &(BmcUtcOffset),
                &DataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "EFI_STORAGE_GET_SEL_TIME_UTC_OFFSET status %r\n", Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO,"BmcUtcOffset: %x \n", BmcUtcOffset));
    //
    // If both Setup and BMC UTC offset are same then Return
    // else, Set Setup UTC offset to BMC
    //
    if ((SetupUtcOffset == BmcUtcOffset) || \
         (SetupUtcOffset == EFI_UNSPECIFIED_TIMEZONE)) {
        DEBUG ((EFI_D_INFO,"Set SEL Time UTC Offset Command is not send to BMC as SetupUtcOffset = BmcUtcOffset/0x7FF.\n"));
        return EFI_SUCCESS;
    }
    //
    // Set SEL Time UTC Offset Command
    // Whenever the Set SEL Time command is executed, UTC Offset must be
    // re-written after using the Set SEL Time command
    //
    DataSize = 0x00;
    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gIpmiInstance->IpmiTransport,
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                EFI_STORAGE_SET_SEL_TIME_UTC_OFFSET,
                (UINT8*) &(SetupUtcOffset),
                sizeof (SetupUtcOffset),
                NULL,
                &DataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "EFI_STORAGE_SET_SEL_TIME_UTC_OFFSET status %r\n", Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));

    return EFI_SUCCESS;
}
#endif //BMC_TIMEZONE_SUPPORT

#if IPMI_BMC_TIME_SYNC_TO_BIOS_TIME_SUPPORT
/**
    1. Compares BIOS and BMC time stamps. If they are different, BMC time
    stamp is updated to BIOS time using Set SEL Time command
    2. Logs Time stamp Clock Synch pair of Events and

    @param gIpmiInstance Pointer to IPMI BMC instance Data

    @return EFI_STATUS

**/
EFI_STATUS
SetTimeStamp (
  IN  EFI_IPMI_BMC_INSTANCE_DATA    *gIpmiInstance )
{
    EFI_STATUS            Status;
    UINT32                SelTime;
    UINT32                NumOfSeconds;
    UINT16                NumOfYears;
    UINTN                 NumOfLeapYears;
    UINTN                 NumOfDays;
    EFI_TIME              Time = {2012, 03, 06, 00, 00, 00, 00, 00, 00, 00, 00};
    UINT8                 DataSize;
    UINT32                CurrentBmcTime;
    UINT16                RecordId = 0;
    INT16                 CurrentBmcUtcOffset = 0x00;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...\n", __FUNCTION__));

    //
    // Get current UTC offset from BMC
    //
    DataSize = sizeof (CurrentBmcUtcOffset);
    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                       &gIpmiInstance->IpmiTransport,
                       IPMI_NETFN_STORAGE,
                       BMC_LUN,
                       EFI_STORAGE_GET_SEL_TIME_UTC_OFFSET,
                       NULL,
                       0,
                       (UINT8*)&CurrentBmcUtcOffset,
                       &DataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "EFI_STORAGE_GET_SEL_TIME_UTC_OFFSET status = %r  Value = %x\n", Status, CurrentBmcUtcOffset));

    if (EFI_ERROR (Status)) {
        CurrentBmcUtcOffset = EFI_UNSPECIFIED_TIMEZONE;
    }

    /* Commenting code as Time Zone issue faced in OS
    if ( CurrentBmcUtcOffset != EFI_UNSPECIFIED_TIMEZONE) {
        //
        // Read the BIOS time using RuntimeServices->GetTime function
        //
        Status = gRT->GetTime (&Time, NULL);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetTime status %r\n", Status));
        if (EFI_ERROR (Status)) {
            return Status;
        }
        //
        // BMC Time Zone calculated as Local time - UTC.
        // BIOS Time Zone calculated as Local time + UTC
        // To overcome the above difference in calculation -1 is used for BIOS time zone
        //
        if (CurrentBmcUtcOffset != (Time.TimeZone * -1)) {

            //
            // Read the BIOS Time Zone using RuntimeServices->GetTime function
            // Reading once again to get the latest time
            //
            Status = gRT->GetTime (&Time, NULL);
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetTime status %r\n", Status));
            if (EFI_ERROR (Status)) {
                return Status;
            }
            Time.TimeZone = CurrentBmcUtcOffset * -1;
            //
            // Set the BIOS time Zone using RuntimeServices->SetTime function
            //
            Status = gRT->SetTime (&Time);
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->SetTime status %r\n", Status));
            if (EFI_ERROR (Status)) {
                return Status;
            }
        }
    }*/
    //
    // Get BMC time using Get SEL Time command
    //
    DataSize = sizeof (SelTime);
    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gIpmiInstance->IpmiTransport,
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                IPMI_STORAGE_GET_SEL_TIME,
                NULL,
                0,
                (UINT8*)&SelTime,
                &DataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI_STORAGE_GET_SEL_TIME status %r\n", Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }

    gBS->CopyMem (&CurrentBmcTime, &SelTime, 4);

    //
    // Read the BIOS time using RuntimeServices->GetTime function
    //
    Status = gRT->GetTime (&Time, NULL);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetTime status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }
  
    NumOfYears = (UINT16)(Time.Year - 1970);
  
    NumOfLeapYears = CountNumOfLeapYears (Time.Year);
    //
    // If this is an actual leap year, we need to reduce the count by 1
    // if it's not past February.  This will allow the proper number of
    // days to be calculated later.
    //
    if (IsLeapYear (Time.Year) && (Time.Month < 3)) {
        ASSERT (NumOfLeapYears > 0);
        NumOfLeapYears--;
    }

    NumOfDays = CalculateNumOfDayPassedThisYear (Time);

    //
    // Add 365 days for all years. Add additional days for Leap Years.
    //
    NumOfDays += (NumOfLeapYears + (365 * (NumOfYears)) - 1);

    NumOfSeconds = (UINT32)(3600 * 24 * NumOfDays + (Time.Hour * 3600) + \
                    (60 * Time.Minute) + Time.Second);

    //
    // Because the precision of the BMC time is 1 second, there is chance that 
    // the difference between BMC time and BIOS time to vary by 1 second, also 
    // considering the lag between BMC time read and BIOS time read another 1 second
    // variation is allowed.
    //
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "CurrentBmcTime %x NumOfSeconds: %x \n", CurrentBmcTime, NumOfSeconds));
    if(CurrentBmcTime < NumOfSeconds - 2 || CurrentBmcTime > NumOfSeconds + 2) {
#if SMCPKG_SUPPORT == 0
        //
        // Log Time stamp Clock Synch First Event of pair.
        //
#if USE_PLATFORM_EVENT_MESSAGE
        DataSize = 0;
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gIpmiInstance->IpmiTransport,
                    IPMI_NETFN_SENSOR_EVENT,
                    BMC_LUN,
                    IPMI_SENSOR_PLATFORM_EVENT_MESSAGE,
                    (UINT8 *) &EventMessage,
                    sizeof (EventMessage),
                    NULL,
                    &DataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Status of Platform Event Message command= %r\n", Status));
#else
        DataSize = sizeof (RecordId);
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gIpmiInstance->IpmiTransport,
                    IPMI_NETFN_STORAGE,
                    BMC_LUN,
                    IPMI_STORAGE_ADD_SEL_ENTRY,
                    (UINT8 *) &TimeStampEvtRecord,
                    sizeof (TimeStampEvtRecord),
                    (UINT8*)&RecordId,
                    &DataSize );
#endif
#endif
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Time stamp - First Event pair IPMI_STORAGE_ADD_SEL_ENTRY status %r\n", Status));

        //
        // Send SEL Time Command using Set SEL time Command
        //
        DataSize = 0;
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gIpmiInstance->IpmiTransport,
                    IPMI_NETFN_STORAGE,
                    BMC_LUN,
                    IPMI_STORAGE_SET_SEL_TIME,
                    (UINT8*) &NumOfSeconds,
                    4,
                    NULL,
                    &DataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "IPMI_STORAGE_SET_SEL_TIME status %r\n", Status));
#if SMCPKG_SUPPORT == 0
        //
        // Log Time stamp Clock Synch Second Event of pair.
        //

#if USE_PLATFORM_EVENT_MESSAGE
        EventMessage.OEMEvData2 |= 0x80; // Set Event second of pair bit
        DataSize = 0;
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gIpmiInstance->IpmiTransport,
                    IPMI_NETFN_SENSOR_EVENT,
                    BMC_LUN,
                    IPMI_SENSOR_PLATFORM_EVENT_MESSAGE,
                    (UINT8 *) &EventMessage,
                    sizeof (IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST),
                    NULL,
                    &DataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Status of Platform Event Message command= %r\n", Status));
#else
        TimeStampEvtRecord.OEMEvData2 |= 0x80; // Set Event second of pair bit
        DataSize = sizeof (RecordId);
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gIpmiInstance->IpmiTransport,
                    IPMI_NETFN_STORAGE,
                    BMC_LUN,
                    IPMI_STORAGE_ADD_SEL_ENTRY,
                    (UINT8 *) &TimeStampEvtRecord,
                    sizeof (TimeStampEvtRecord),
                    (UINT8*)&RecordId,
                    &DataSize );
#endif
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Time stamp - second Event pair IPMI_STORAGE_ADD_SEL_ENTRY status %r\n", Status));

        if (CurrentBmcUtcOffset != EFI_UNSPECIFIED_TIMEZONE) {
            //
            // Set current UTC offset  from BMC
            //
            DataSize = 0;
            Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                                &gIpmiInstance->IpmiTransport,
                                IPMI_NETFN_STORAGE,
                                BMC_LUN,
                                EFI_STORAGE_SET_SEL_TIME_UTC_OFFSET,
                                (UINT8*)&CurrentBmcUtcOffset,
                                sizeof (CurrentBmcUtcOffset),
                                NULL,
                                &DataSize );
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "EFI_STORAGE_SET_SEL_TIME_UTC_OFFSET status = %r  Value = %04x\n", Status,CurrentBmcUtcOffset));
        }
#endif
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));
    return Status;
}
#endif

/**
    Execute the Get Device ID command.

    @param ResponseData Buffer to copy the GetDeviceId response
    @param ResponseDataSize Response data size

    @return  EFI_STATUS

**/

EFI_STATUS
GetDeviceId (
 OUT  UINT8 *ResponseData,
 OUT  UINT8 *ResponseDataSize )
{
    EFI_STATUS          Status;

    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gIpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_DEVICE_ID,
                NULL,
                0,
                ResponseData,
                ResponseDataSize );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Device ID status %r\n", Status));

    return Status;
}

#if IPMI_BMC_TIME_SYNC_TO_BIOS_TIME_SUPPORT
/**
    Notification function for Real Time Clock Arch protocol.
    Set time stamp to BMC, Validates time zone setup values and sets default
    time zone values in setup variable and also sets UTC offset in BMC.

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @return VOID

**/

VOID
EFIAPI
RealTimeClockArchProtocolNotification (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context )
{
    EFI_STATUS      Status;
    VOID            *RealTimeClockArchProtocol = NULL;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entered...\n", __FUNCTION__));

    Status = gBS->LocateProtocol (
                &gEfiRealTimeClockArchProtocolGuid,
                NULL,
                (VOID **)&RealTimeClockArchProtocol);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBS->LocateProtocol gEfiRealTimeClockArchProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }

    //
    // Checks for Time Stamp
    //
    SetTimeStamp (gIpmiInstance);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));

    return;
}
#endif

/**
    Execute the Get System Interface Capability command and update Ipmi Instance.

    @param  VOID

    @retval  VOID

**/
VOID
GetSystemInterfaceCapability (
  IN  VOID
)
{
    EFI_STATUS                                     Status;
    EFI_GET_SYSTEM_INTERFACE_CAPABILITY_COMMAND    GetSystemInterfaceCapabilityCmd;
    EFI_IPMI_SSIF_INTERFACE_CAPABILITY             GetSsifInterfaceCapability;
    UINT8                                          DataSize = sizeof (GetSsifInterfaceCapability);

    GetSystemInterfaceCapabilityCmd.SystemInterfaceType = 0x0; // SSIF
    GetSystemInterfaceCapabilityCmd.Reserved = 0x0;

    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gIpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_SYSTEM_INTERFACE_CAPABILITIES,
                (UINT8*) &GetSystemInterfaceCapabilityCmd,
                sizeof (GetSystemInterfaceCapabilityCmd),
                (UINT8*) &GetSsifInterfaceCapability,
                &DataSize);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Status %r\n", __FUNCTION__, Status));

    if (!EFI_ERROR (Status)) {
        gIpmiInstance->Interface.SSIF.RwSupport = GetSsifInterfaceCapability.TransactionSupport;
        gIpmiInstance->Interface.SSIF.PecSupport = GetSsifInterfaceCapability.PecSupport;
    }

}

/**
    Execute the Set Global Enable command to enable receive message queue interrupt.

    @return VOID

**/
VOID
SetGlobalEnable ()
{
    EFI_STATUS                      Status;
    EFI_BMC_GLOBAL_ENABLES          BmcGlobalEnables;
    UINT32                          ResponseDataSize = sizeof (BmcGlobalEnables);

    //
    // Get Global Enable Information.
    //
    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand(
                &gIpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_BMC_GLOBAL_ENABLES,
                NULL,
                0,
                (UINT8 *) (&BmcGlobalEnables),
                (UINT8 *) &ResponseDataSize
                );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get BMC Global Enable status %r\n", Status));

    if (EFI_ERROR(Status)) {
        return;
    }

    //
    // Set Smb alert pin based on ReceiveMsgQueueInterrupt bit
    //

    gIpmiInstance->Interface.SSIF.SmbAlertSupport = BmcGlobalEnables.ReceiveMsgQueueInterrupt;
}

/**
    Setup and initialize the BMC for the DXE phase. In order to 
    verify the BMC is functioning as expected, the BMC Self test is performed.
    The results are then checked and any errors are reported to the error manager.
    Errors are collected throughout this routine and reported just prior to
    installing the driver.  If there are more errors than BST_LOG_SIZE, then they
    will be ignored.

    @param ImageHandle Handle of this driver image 
    @param SystemTable Table containing standard EFI services

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializeIpmiPhysicalLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{

    EFI_STATUS                            Status = EFI_SUCCESS;
    EFI_STATUS                            StatusOfGetDeviceId = EFI_DEVICE_ERROR;
    EFI_HANDLE                            NewHandle = NULL;
    UINT8                                 DataSize;
    UINT8                                 Index;
    EFI_STATUS_CODE_VALUE                 StatusCodeValue[BST_LOG_SIZE];
    UINT8                                 ErrorCount = 0;
    EFI_BMC_SELF_TEST_LOG_PROTOCOL        *BmcSelfTestProtocol = NULL;
    UINTN                                 Size;
    EFI_EVENT                             EfiVirtualAddressChangeNotifyEvent = NULL;
    IPMI_GET_DEVICE_ID_RESPONSE           GetDeviceIdResponse;
    IPMI_SELF_TEST_RESULT_RESPONSE        BstStatus;
    UINT8                                 BitSelect = 0;
    UINT8                                 ErrorBitVal = 0;
    UINT8                                 PublishSPMI = TRUE;

    InitAmiLib (ImageHandle,SystemTable);

#if SMCPKG_SUPPORT
    Status = gBS->LocateProtocol(
		    &gSmcSimpleTextOutProtocolGuid,
                    NULL,
                    (VOID**)&pmSmcSimpleTextOutProtocol);
    
    Status = gRT->GetVariable(
		    L"Setup", 
		    &gEfiSetupVariableGuid,
		    NULL,
		    &VariableSize,
		    &SetupData);

#endif

    Size = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    gSetupStatus = gRT->GetVariable (
                    L"ServerSetup",
                    &gEfiServerMgmtSetupVariableGuid,
                    &gAttributes,
                    &Size,
                    &gServerMgmtConfiguration );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetVariable status %r ServerMgmtConfiguration.BmcSupport: %x \n", gSetupStatus, gServerMgmtConfiguration.BmcSupport));
    if (!EFI_ERROR(gSetupStatus) && !gServerMgmtConfiguration.BmcSupport) {
        return EFI_UNSUPPORTED;
    }

    //
    // Allocate memory for IPMI Instance
    //
    gIpmiInstance = (EFI_IPMI_BMC_INSTANCE_DATA*)AllocateRuntimeZeroPool (sizeof (EFI_IPMI_BMC_INSTANCE_DATA));
    if (gIpmiInstance == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for gIpmiInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

#if SMCPKG_SUPPORT
    if(!SmcIPMIDXESetting()){
	return EFI_SUCCESS;
    }
#endif

    if (IPMI_DEFAULT_HOOK_SUPPORT) {
        //
        // Initialize IPMI Instance Data
        //
        if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE) { // KCS Interface
            gIpmiInstance->Interface.KCS.DataPort = IPMI_KCS_DATA_PORT;          // KCS Data Port
            gIpmiInstance->Interface.KCS.CommandPort = IPMI_KCS_COMMAND_PORT;    // KCS Command Port
        } else if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE){ // BT_Interface
            gIpmiInstance->Interface.BT.CtrlPort = IPMI_BT_CTRL_PORT;              // BT Control Port
            gIpmiInstance->Interface.BT.ComBuffer = IPMI_BT_BUFFER_PORT;           // BT Buffer Port
            gIpmiInstance->Interface.BT.IntMaskPort = IPMI_BT_INTERRUPT_MASK_PORT; // BT IntMask Port
        }
        gIpmiInstance->MmioBaseAddress = 0;
        gIpmiInstance->AccessType = IPMI_IO_ACCESS;
        gIpmiInstance->BaseAddressRange = 0;
    } else {
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
        gIpmiInstance->AccessType = IPMI_MMIO_ACCESS;
        Status = IpmiOemDxeInitHook (gBS, &gIpmiInstance->MmioBaseAddress, &gIpmiInstance->BaseAddressRange);
        if (EFI_ERROR (Status)) {
            //
            // IpmiOemDxeInitHook has failed, so free the allocated memory and return Status.
            //
            FreePool (gIpmiInstance);
            SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IpmiOemDxeInitHook is failed and Status is %r.\n", Status));
            return Status;
        }
#endif
    }
    if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE) { // KCS Interface
        gIpmiInstance->Interface.KCS.KcsRetryCounter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
        gIpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmiSendCommand;   // KCS SendCommand
        gIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmiSendCommandEx; // KCS SendCommand Extension
    } else if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE){ // BT_Interface
        gIpmiInstance->Interface.BT.BtDelay = BT_DELAY;                // BT Delay
        gIpmiInstance->IpmiTransport.SendIpmiCommand = EfiBtIpmiSendCommand;   // BT SendCommand
        gIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiBtIpmiSendCommandEx; // BT SendCommand Extension
    } else if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) { // SSIF_Interface
        Status = gBS->LocateProtocol(
                    &gEfiSmbusHcProtocolGuid, 
                    NULL, 
                    (VOID **)&gEfiSmbusHcProtocol
                    );
        if (EFI_ERROR(Status)) {
            FreePool (gIpmiInstance);
            return Status;
        }

        gIpmiInstance->IpmiTransport.SendIpmiCommand = EfiSsifIpmiSendCommand; // SSIF SendCommand
        gIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiSsifIpmiSendCommandEx; // SSIF SendCommand Extension
        gIpmiInstance->Interface.SSIF.SsifRetryCounter = IPMI_SSIF_COMMAND_REQUEST_RETRY_COUNTER; // SSIF retry counter
        gIpmiInstance->Interface.SSIF.PecSupport = FALSE; // SSIF PEC support
        gIpmiInstance->Interface.SSIF.RwSupport = 0x0; // SSIF multi-part reads/writes support
        gIpmiInstance->Interface.SSIF.SmbAlertSupport = FALSE; // SMB alert pin support
    } else {
        Status = gBS->LocateProtocol (
                        &gEfiI2cMasterProtocolGuid,
                        NULL,
                       (VOID **)&gMasterTransmit
                       );
        if (EFI_ERROR(Status)) {
            FreePool (gIpmiInstance);
            return Status;
        }
        gIpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmbIpmiSendCommand; // IPMB SendCommand
        gIpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmbIpmiSendCommandEx; // IPMB SendCommand Extension
        PublishSPMI = FALSE; // SPMI table does not provide any information about IPMB.
    }

    gIpmiInstance->Signature = SM_IPMI_BMC_SIGNATURE;
    gIpmiInstance->SlaveAddress = IPMI_BMC_SLAVE_ADDRESS;
    gIpmiInstance->BmcStatus = EFI_BMC_OK;
    gIpmiInstance->IpmiTransport.GetBmcStatus = EfiIpmiBmcStatus;

    //
    // Check SSIF interface capability
    //
    if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) {
        GetSystemInterfaceCapability();
        SetGlobalEnable();
    }

    //
    // Get the BMC SELF TEST Results.
    //

    for (Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++) {

        DataSize = sizeof (BstStatus);
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gIpmiInstance->IpmiTransport,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                    (UINT8*)&BstStatus,
                    &DataSize);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Self test results status %r\n", Status));
        if (!EFI_ERROR(Status)) {
            break;
        }
    }

    DEBUG ((EFI_D_INFO, "Self test result Status: %r\n", Status));
    //
    // If Status indicates a Device error, then the BMC is not responding, so send an error.
    //
    if (EFI_ERROR (Status)) {
        if (ErrorCount < BST_LOG_SIZE) {
            StatusCodeValue[ErrorCount++] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_COMM_ERROR;
        }
        gIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
    } else {

        //
        // Check the self test results. Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch (BstStatus.Result) {
            case IPMI_APP_SELFTEST_NO_ERROR: // 0x55
            case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case IPMI_APP_SELFTEST_RESERVED: // 0xFF
                gIpmiInstance->BmcStatus = EFI_BMC_OK;
                break;

            case IPMI_APP_SELFTEST_ERROR: // 0x57
                gIpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
                //
                // Check the Error bit set and report the error.
                //
                ErrorBitVal = BstStatus.Param;
                while ((BitSelect < 8) && ErrorBitVal) { // Loop until all bits are checked. Initial value of BitSelect is Zero.
                    if (ErrorBitVal & BIT0) { // check if selected bit is set.
                        if (ErrorCount < BST_LOG_SIZE) {
                            StatusCodeValue[ErrorCount++] = ErrorBitToStatusCodeMap[BitSelect]; // Store the status code mapped for this error bit.
                        } else {
                            break;
                        }
                    }
                    BitSelect = BitSelect + 1; // Select the next bit to be checked. Initial value of BitSelect is Zero.
                    ErrorBitVal = ErrorBitVal >> 1; // Remove tested bit from the value.
                }
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                gIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
                break;
        } // switch case

        if (gIpmiInstance->BmcStatus == EFI_BMC_HARDFAIL) {
            if ( ErrorCount < BST_LOG_SIZE ) {
                StatusCodeValue[ErrorCount++] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_HARD_FAIL;
            }
        } else if (gIpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL) {
            if (ErrorCount < BST_LOG_SIZE) {
                StatusCodeValue[ErrorCount++] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SOFT_FAIL;
            }
        }

        //
        // The Device ID information is only retrieved when the self test has passed or generated a SOFTFAIL.
        // The BMC Status may be overwritten to show the BMC is in Force Update mode.
        // But the BMC Status will always be preserved if a hard fail has occurred since this code won't execute.
        //
        if ((gIpmiInstance->BmcStatus == EFI_BMC_OK) || (gIpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL)) {
#if IPMI_BMC_TIME_SYNC_TO_BIOS_TIME_SUPPORT
#if SMCPKG_SUPPORT == 0
// SMC PORTING disable The timestamp sync log
            VOID  *NotifyRegistration;

            //
            // Create Notification event for Real time clock arch protocol GUID
            //
            EfiCreateProtocolNotifyEvent (
                &gEfiRealTimeClockArchProtocolGuid,
                TPL_CALLBACK,
                RealTimeClockArchProtocolNotification,
                NULL,
                &NotifyRegistration
                );
#endif
#endif
            //
            // Send Get Device ID command
            //
            DataSize = sizeof (GetDeviceIdResponse);
            StatusOfGetDeviceId = GetDeviceId ((UINT8*)&GetDeviceIdResponse, &DataSize);
            if(!EFI_ERROR (StatusOfGetDeviceId)) {

                //
                // If the controller is in Update Mode and the maximum number of errors
                // has not been exceeded, then save the error code to the StatusCode
                // array and increment the counter. Set the BMC Status to indicate
                // the BMC is in force update mode.
                //
                if (GetDeviceIdResponse.UpdateMode != 0) {
                    gIpmiInstance->BmcStatus = EFI_BMC_UPDATE_IN_PROGRESS;
                    if (ErrorCount < BST_LOG_SIZE) {
                        StatusCodeValue[ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR\
                                | EFI_CU_FP_EC_FORCE_UPDATE_MODE;
                        ErrorCount++;
                    }
                }
            }
        }
    } // else case


    SERVER_IPMI_DEBUG ((EFI_D_INFO, "BMC status %x\n", gIpmiInstance->BmcStatus));

    for (Index = 0; Index < ErrorCount; Index++) {
       ReportStatusCode (
          EFI_ERROR_CODE | EFI_ERROR_MINOR,
          StatusCodeValue[Index]
        );
    }

    //
    // Install BmcSelfTestProtocolLog protocol.
    // It is used in BMC Self Test Log Setup page to display BMC self test results.
    // Allocate memory for BMC Self-test log Instance
    //
    BmcSelfTestProtocol = AllocateZeroPool (sizeof (EFI_BMC_SELF_TEST_LOG_PROTOCOL));
    if (BmcSelfTestProtocol == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for BmcSelfTestProtocol! \n"));
        ASSERT (BmcSelfTestProtocol != NULL);
    } else {

        //
        // GetDeviceId command succeed, Copy the BMC details into self test protocol to display it in setup
        //
        if (!EFI_ERROR (StatusOfGetDeviceId)) {
            BmcSelfTestProtocol->IsGetDeviceIdCommandPassed = TRUE;
            CopyMem (&BmcSelfTestProtocol->GetDeviceIdResponse, &GetDeviceIdResponse, sizeof (GetDeviceIdResponse));
        } else {
            BmcSelfTestProtocol->IsGetDeviceIdCommandPassed = FALSE;
        }

        //
        // Copy the Status code structure into BMC Self test protocol
        //
        CopyMem (&BmcSelfTestProtocol->BmcSelfTestError[0], &StatusCodeValue[0], ErrorCount * sizeof (EFI_STATUS_CODE_VALUE));
        BmcSelfTestProtocol->BmcSelfTestErrorCount = ErrorCount;

        Status = gBS->InstallProtocolInterface (
                    &NewHandle,
                    &gEfiBmcSelfTestLogProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    BmcSelfTestProtocol);

        SERVER_IPMI_DEBUG ((EFI_D_INFO, "BMC self test protocol Install status %r\n", Status));
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR (Status)) {

            //
            // Error in installing the protocol. So free the allocated memory.
            //
            FreePool (BmcSelfTestProtocol);
        }
    }
    //
    // Now install the Protocol if the BMC is not in a Hard Fail State
    //
    if (gIpmiInstance->BmcStatus != EFI_BMC_HARDFAIL) {

        if (PublishSPMI) { 
            //
            // Add SPMI support
            //
            SpmiSupport ();
        }
        //
        // Add IPMI SMBIOS types support
        //
        IpmiSmbiosTypesSupport ();

#if BMC_TIMEZONE_SUPPORT
        //
        // Set Time zone to BMC
        //
        SetTimeZone (gIpmiInstance);
#endif //BMC_TIMEZONE_SUPPORT

        //
        // CMOS Clear support
        //
#if PeiIpmiCmosClear_SUPPORT
        IpmiCmosClear ();
#endif
        //
        // Install the protocol
        //
        NewHandle = NULL;
        Status = gBS->InstallProtocolInterface (
                    &NewHandle,
                    &gEfiDxeIpmiTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gIpmiInstance->IpmiTransport);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiDxeIpmiTransportProtocolGuid protocol  status %r\n", Status));
        if (EFI_ERROR (Status)) {

            //
            // Error in installing the protocol. So free the allocated memory.
            //
            FreePool (gIpmiInstance);
            return Status;
        }
#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        //
        // Same Interface used for both SMM and DXE phases so Setup for Proxy the DXE calls to SMM
        //
        InitializeIpmiPhysicalLayerHook ();

#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        //
        // Create event for the Virtual Address Change Event
        //
        Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    DxeIpmiIntializeVirtualAddressChangeEvent,
                    NULL,
                    (CONST EFI_GUID *)&gEfiEventVirtualAddressChangeGuid,
                    &EfiVirtualAddressChangeNotifyEvent );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Create event for the Virtual Address Change Event  status %r\n", Status));

#if SMCPKG_SUPPORT
        SmcIPMIReadyToBoot();
#endif

        return EFI_SUCCESS;

    } else {

        //
        // SELF test has failed, so free the memory and return EFI_UNSUPPORTED to unload driver from memory.
        //
        FreePool (gIpmiInstance);
        return EFI_UNSUPPORTED;

    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
