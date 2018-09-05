//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jun/07/2016
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

/** @file BmcSelfTestLog.c
    Functions to log BMC status to setup

**/

//-----------------------------------------------------------------------

#include "BmcSelfTestLog.h"

//-----------------------------------------------------------------------

//
// Global Variables
//
BMC_SELF_TEST_LOG_DATA          gBstLog;
SERVER_MGMT_CONFIGURATION_DATA  gServerMgmtConfiguration;

/**
    Convert EFI_STATUS CODE value to match the string index in the array 
    SetupStrings

    @param Value Value of Status code need to be converted.

    @return UINT8 String Array Index equivalent for EFI_STATUS_CODE_CALUE

**/
UINT8
ConvertStatusCode (
  IN  EFI_STATUS_CODE_VALUE Value )
{
    switch ( Value ) {

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_HARD_FAIL:
            return 2;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SOFT_FAIL:
            return 3;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_COMM_ERROR:
            return 4;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SEL_NOT_ACCESSIBLE:
            return 5;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SDR_REPOSITORY_NOT_ACCESSABLE:
            return 6;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_BMC_FRU_NOT_ACCESSIBLE:
            return 7;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_IPMB_NOT_RESPONDING:
            return 8;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SDR_EMPTY:
            return 9;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_BMC_FRU_CORRUPTED:
            return 10;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_BB_CORRUPTED:
            return 11;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_FIRMWARE_CORRUPTED:
            return 12;

        case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_FORCE_UPDATE_MODE:
            return 13;

        default :
            return 0;
    }
}
#if TSE_MULTILINE_CONTROLS == 1
/**
    This function updates Time to BMC Self Test Log

    @param Index Index of Log

    @retval VOID

**/
VOID
UpdateTime (
  IN UINT8 Index )
{

    EFI_TIME    Time;

    //
    // Getting the time and date using the runtime service GetTime
    //
    gRT->GetTime (&Time, NULL);
    //
    // Filling the gBstLog NvRam variable with date and time
    // of each particular log
    //
    gBstLog.date[Index].Year = Time.Year;
    gBstLog.date[Index].Month = Time.Month;
    gBstLog.date[Index].Day = Time.Day;
    gBstLog.time[Index].Hour = Time.Hour;
    gBstLog.time[Index].Minute = Time.Minute;
    gBstLog.time[Index].Second = Time.Second;

}
#endif
/**
    This function updates the BMC Self Test log in setup.

    @param HiiHandle Handle to HII database
    @param Class Indicates the setup class

    @retval VOID

**/
VOID
BmcSelfTestLogStrings (
  IN EFI_HII_HANDLE     HiiHandle,
  IN UINT16             Class )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_BMC_SELF_TEST_LOG_PROTOCOL  *BmcSelfTestProtocol = NULL;
    UINT8       Index;
    UINT8       LogIndex;
    UINTN       Size;
    UINT16      StringRef;
#if SMCPKG_SUPPORT
    UINT8       BMCFR;
#endif
    UINT16       SetupStrings[14] = {
                                     STRING_TOKEN(STR_BMCSELFTESTLOG_PROTOCOL_NOT_FOUND),
                                     STRING_TOKEN(STR_BMCSELFTESTLOG_EMPTY),
                                     STRING_TOKEN(STR_BMC_HARD_FAIL),
                                     STRING_TOKEN(STR_BMC_SOFT_FAIL),
                                     STRING_TOKEN(STR_BMC_COMMUNICATION_ERROR),
                                     STRING_TOKEN(STR_SEL_DEVICE_INACCESSIBLE),
                                     STRING_TOKEN(STR_SDR_REPOSITORY_INACCESSIBLE),
                                     STRING_TOKEN(STR_BMC_FRU_DEVICE_INACCESSIBLE),
                                     STRING_TOKEN(STR_IPMB_SIGNAL_LINES_UNRESPONSIVE),
                                     STRING_TOKEN(STR_SDR_REPOSITORY_EMPTY),
                                     STRING_TOKEN(STR_BMC_FRU_AREA_CORRUPTED),
                                     STRING_TOKEN(STR_BOOTBLOCK_FIRMWARE_CORRUPTED),
                                     STRING_TOKEN(STR_CONTROLLER_OPERATIONAL_FIRMWARE_CORRUPTED),
                                     STRING_TOKEN(STR_FORCE_UPDATE_MODE)
                                     };
    CHAR16       CharPtr[STRING_BUFFER_LENGTH];
    EFI_STATUS   CharPtrStatus = EFI_SUCCESS;
    UINTN        BufferLength = STRING_BUFFER_LENGTH;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a: Class ID:  %x\n", __FUNCTION__, Class));
    //
    // Continue only for Server Mgmt setup page
    //
    if (Class != SERVER_MGMT_CLASS_ID) {
        return;
    }

    //
    // Get ServerMgmt Setup variable
    //
    Size = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gRT->GetVariable (
                    L"ServerSetup",
                    &gEfiServerMgmtSetupVariableGuid,
                    NULL,
                    &Size,
                    &gServerMgmtConfiguration );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetVariable ServerSetup status %r \n", Status));

    if (EFI_ERROR (Status)) {
        //
        // Handle error case with default values
        //
        gServerMgmtConfiguration.BstErase = ERASE_ON_EVERY_RESET;
        gServerMgmtConfiguration.BstLogFull = CLEAR_LOG;
    }

    //
    // Get BstLogVar NVRAM variable
    //
    Size = sizeof (BMC_SELF_TEST_LOG_DATA);
    Status = gRT->GetVariable (
                    L"BstLogVar",
                    &gEfiServerMgmtSetupVariableGuid,
                    NULL,
                    &Size,
                    &gBstLog );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetVariable BstLogVar status %r \n", Status));

    if (EFI_ERROR (Status)) {
        //
        // Handle error case with default values
        //
        gServerMgmtConfiguration.BstErase = ERASE_ON_EVERY_RESET;
    }

    //
    // Clear gBstLog NvRam variable with condition check
    //
    if ((gServerMgmtConfiguration.BstErase == ERASE_ON_EVERY_RESET) ||
         ( (gBstLog.logSize == BST_LOG_SIZE) &&
           (gServerMgmtConfiguration.BstLogFull == CLEAR_LOG))) {
        ZeroMem (&gBstLog, sizeof (BMC_SELF_TEST_LOG_DATA));
    }

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBstLog.logSize:  %x \n", gBstLog.logSize));
    LogIndex = gBstLog.logSize;

    //
    // Locate the BMC self test protocol
    //
    Status = gBS->LocateProtocol (
                    &gEfiBmcSelfTestLogProtocolGuid,
                    NULL,
                    (VOID **)&BmcSelfTestProtocol );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiBmcSelfTestLogProtocolGuid: Status:  %r\n", Status));

    // 
    // Displaying the string PASSED or FAILED depending upon the Protocol locate status and Error count
    //
    if ((!EFI_ERROR (Status)) && (BmcSelfTestProtocol->BmcSelfTestErrorCount == 0)) {
#if SMCPKG_SUPPORT
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_SMC_LOCATE_STATUS_PASSED), CharPtr, &BufferLength, NULL);
#else
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_PROTOCOL_LOCATE_STATUS_PASSED), CharPtr, &BufferLength, NULL);
#endif
    } else {
#if SMCPKG_SUPPORT
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_SMC_LOCATE_STATUS_FAILED), CharPtr, &BufferLength, NULL);
#else
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_PROTOCOL_LOCATE_STATUS_FAILED), CharPtr, &BufferLength, NULL);
#endif
    }
    if(!EFI_ERROR (CharPtrStatus)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_IPMI_BMC_SELF_TEST_STATUS_VAL),
            CharPtr );
    }
    // 
    // Displaying BMC details in setup. If GetDeviceId command fails, "Unknown"
    // will be printed for each parameter
    //
    if ((!EFI_ERROR (Status)) && BmcSelfTestProtocol->IsGetDeviceIdCommandPassed) {

        //
        // BMC Device ID
        //
        InitString (
                HiiHandle,
                STRING_TOKEN(STR_IPMI_BMC_DEVICE_ID_VAL),
                L"%d",
                BmcSelfTestProtocol->GetDeviceIdResponse.DeviceId);

        //
        // BMC Device Revision
        //
        InitString (
                HiiHandle,
                STRING_TOKEN(STR_IPMI_BMC_DEVICE_REVISION_VAL),
                L"%x",
                BmcSelfTestProtocol->GetDeviceIdResponse.DeviceRevision);

        //
        // BMC Firmware Revision
        //
#if SMCPKG_SUPPORT
        BMCFR = BmcSelfTestProtocol->GetDeviceIdResponse.MinorFirmwareRev;
        if(BMCFR < 10){
            InitString(HiiHandle, STRING_TOKEN(STR_IPMI_BMC_FIRMWARE_REVISION_VAL),
                L"%x.0%x", 
                BmcSelfTestProtocol->GetDeviceIdResponse.MajorFirmwareRev, BmcSelfTestProtocol->GetDeviceIdResponse.MinorFirmwareRev);
        } else {
#endif
        InitString (
                HiiHandle,
                STRING_TOKEN(STR_IPMI_BMC_FIRMWARE_REVISION_VAL),
                L"%x.%x",
                BmcSelfTestProtocol->GetDeviceIdResponse.MajorFirmwareRev, BmcSelfTestProtocol->GetDeviceIdResponse.MinorFirmwareRev);
#if SMCPKG_SUPPORT
        }
#endif
        //
        // IPMI Version
        //
        InitString (
                HiiHandle,
                STRING_TOKEN(STR_IPMI_VERSION_VAL),
                L"%x.%x",
                ((BmcSelfTestProtocol->GetDeviceIdResponse.SpecificationVersion)& 0x0F), ((BmcSelfTestProtocol->GetDeviceIdResponse.SpecificationVersion) >> 0x04));

    }

    //
    // Logging Error
    //
    if (EFI_ERROR (Status)) {

        //
        //Log Protocol not found error
        //
        if (LogIndex < BST_LOG_SIZE) {
            gBstLog.log[LogIndex] = 0; // Index of String Indicating protocol error
#if TSE_MULTILINE_CONTROLS == 1
            UpdateTime (LogIndex);
#endif
            SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBstLog.log[%x]:  %x \n", LogIndex, gBstLog.log[LogIndex]));
            LogIndex = LogIndex + 1;
        }
    } else {
        //
        // Fill the gBstLog NVRAM variable with BMC Self Test Status codes
        //
        if (BmcSelfTestProtocol->BmcSelfTestErrorCount != 0 && LogIndex < BST_LOG_SIZE) {
            for(Index=0; Index < BmcSelfTestProtocol->BmcSelfTestErrorCount; Index++, LogIndex++) {
                if (LogIndex == BST_LOG_SIZE) {
                    break;
                }
                gBstLog.log[LogIndex] = ConvertStatusCode (BmcSelfTestProtocol->BmcSelfTestError[Index]);
                SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBstLog.log[%x]:  %x \n", LogIndex, gBstLog.log[LogIndex]));

#if TSE_MULTILINE_CONTROLS == 1
                UpdateTime (LogIndex);
#endif
            }
        }
    }
    gBstLog.logSize = LogIndex;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gBstLog.logSize:  %x \n", gBstLog.logSize));
    //
    // Set BstLogVar Nvram variable
    //
    Status = gRT->SetVariable (
                L"BstLogVar",
                &gEfiServerMgmtSetupVariableGuid,
                VAR_FLAG_ATTRIBUTES,
                sizeof(BMC_SELF_TEST_LOG_DATA),
                &gBstLog );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->SetVariable BstLogVar status %r \n", Status));

    //
    // Add/log BmcSelfTest status codes in setup
    // and also update Log area usage in setup
    //
    BufferLength = STRING_BUFFER_LENGTH;
    CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_LOG_AREA_USAGE), CharPtr, &BufferLength, NULL);
    if (!EFI_ERROR(CharPtrStatus)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_LOG_CAPACITY),
            CharPtr,
            gBstLog.logSize );
    }

    //
    // Check whether log is empty or not and update the strings
    //
    StringRef = STRING_TOKEN(STR_IPMI_SELF_TEST_LOG_1);
    if (!gBstLog.logSize) {
        BufferLength = STRING_BUFFER_LENGTH;
        CharPtrStatus = IpmiHiiGetString (HiiHandle, SetupStrings[1], CharPtr, &BufferLength, NULL);
        if (!EFI_ERROR(CharPtrStatus)) {
            InitString (
                HiiHandle,
                StringRef,
                CharPtr );
        }

    } else {
        //
        // Displaying the Date, Time and Status code Heading
        //
#if TSE_MULTILINE_CONTROLS == 1
        BufferLength = STRING_BUFFER_LENGTH;
        CharPtrStatus = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_DATE_TIME_STATUSCODE), CharPtr, &BufferLength, NULL);
        if (!EFI_ERROR(CharPtrStatus)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_DATE_TIME_STATUSCODE_HEADING),
                CharPtr );
        }
#endif
        for(Index = 0; Index < gBstLog.logSize ; Index++, StringRef++) {
            //
            // Displaying the Date and Time if the token MULTILINE_CONTROLS is set
            //
            BufferLength = STRING_BUFFER_LENGTH;
            CharPtrStatus = IpmiHiiGetString ( HiiHandle, SetupStrings[gBstLog.log[Index]], CharPtr, &BufferLength, NULL);
#if TSE_MULTILINE_CONTROLS == 1
            if (!EFI_ERROR(CharPtrStatus)) {
                InitString (
                        HiiHandle,
                        StringRef,
                        L"%02d/%02d/%02d  %02d:%02d:%02d  %s\n",
                        gBstLog.date[Index].Month,
                        gBstLog.date[Index].Day,
                        gBstLog.date[Index].Year,
                        gBstLog.time[Index].Hour,
                        gBstLog.time[Index].Minute,
                        gBstLog.time[Index].Second,
                        CharPtr );
            }
#else
            if (!EFI_ERROR(CharPtrStatus)) {
                InitString (
                        HiiHandle,
                        StringRef,
                        CharPtr );
            }
#endif
        } //For loop
    } //gBstLog.logSize Check for empty log
    return;
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
