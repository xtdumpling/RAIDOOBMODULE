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

/** @file ViewSelLog.c
    Implementation for View System Event log setup page

**/

//----------------------------------------------------------------------

#include "ViewSelLog.h"

//----------------------------------------------------------------------

//
// Global Variables
//
static EFI_GUID gErrorManagerGuid       = ERROR_MANAGER_GUID;

ERROR_MANAGER                           *gErrorManager = NULL;
SEL_SENSOR_DETAILS_TOKEN_LOOKUP_TABLE   *gSensorDetailsSavedToken = NULL;
UINTN                                   gSensorDetailsSavedTokenCount = 0;
SEL_RECORD_DESC_TOKEN_LOOKUP_TABLE      *gSelRecordDescSavedToken = NULL;
UINTN                                   gSelRecordDescSavedTokenCount = 0;
UINT16                                  gRecordIdToViewMoreLog = 0;

/**
    This function verifies the leap year

    @param Year - Year in YYYY format

    @return BOOLEAN
    @retval TRUE For leap year.
    @retval FALSE For Non-leap year.

**/

BOOLEAN
IsLeapYear (
  IN  UINT16   Year )
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
    This function returns Number of seconds in a year.

    @param Year - Year in YYYY format

    @return UINT32 Returns Number of seconds in a year.

**/

UINT32
SecInYear (
  IN  UINT16   Year )
{
    if ( IsLeapYear(Year) ) {
        return 366 * 24 * 60 * 60;
    } else {
        return 365 * 24 * 60 * 60;
    }
}

/**
    This function searches for the value in lookup table and returns string
    associated with the value.

    @param Value Value to be searched in Lookup table
    @param Table[] Lookup table
    @param TableSize  Size of the Lookup table

    @return EFI_STRING
    @retval RESERVED_SENSOR_NUMBER_STRING If the value is not found in lookup table
    @retval DescString If the value is found in lookup table

**/

EFI_STRING
GetDescStringFromValue (
  IN  UINT8                     Value,
  IN  VALUE_TO_DESC_STRING      Table[],
  IN  UINT8                     TableSize )
{
    UINT8   Counter = -1;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Inside %s\n", __FUNCTION__));

    while ( ++Counter < TableSize ) {
        //
        // Search for Desc string in table
        //
        if ( Table[Counter].Value == Value) {
            return Table[Counter].DescString;
        }
    }

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Exit %s\n", __FUNCTION__));

    return RESERVED_UNKNOWN_STRING;
}

/**
    This function converts Time stamp (in sec) into EFI Time format

    @param TimeStamp - Time stamp in Sec

    @return EFI_TIME

**/

EFI_TIME
GetEfiTimeFromTimeStamp (
  IN  UINT32    TimeStamp )
{
    EFI_TIME        EfiTime = {0};
    UINT16          Year4 = 1970;
    UINT8           NumDays = 0;
    UINT16          DaysInYear = 0;
    UINT32          TimeInSec = TimeStamp;
    UINT8           Month = 0;
    UINT8           Day   = 0;
    UINT16          Year  = 0;
    UINT8           Second = 0;
    UINT8           Minute = 0;
    UINT8           Hour = 0;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Inside %s\n", __FUNCTION__ ));

    while ( TimeInSec >= SecInYear (Year4) ) {
        TimeInSec = TimeInSec - SecInYear (Year4);
        Year4++;
    }
    Year = Year4;
    Year4 = (Year4 / 10) * (10) + (Year4 % 10);

    DaysInYear = (TimeInSec / (24 * 60 * 60)) + 1;
    Month = 1;
    if ( DaysInYear > 31 ) { // see if past january
        DaysInYear -= 31;
        Month++;
        if ( IsLeapYear (Year4) ) {
          NumDays = 29;
        } else {
          NumDays = 28;
        }
        if ( DaysInYear > NumDays ) { // see if past feb
            DaysInYear -= NumDays;
            Month++;
            if ( DaysInYear > 31 ) { // see if past march
                DaysInYear -= 31;
                Month++;
                if ( DaysInYear > 30 ) { // see if past april
                    DaysInYear -= 30;
                    Month++;
                    if ( DaysInYear > 31 ) { // see if past may
                        DaysInYear -= 31;
                        Month++;
                        if ( DaysInYear > 30 ) { // see if past june
                            DaysInYear -= 30;
                            Month++;
                            if ( DaysInYear > 31 ) { // see if past july
                                DaysInYear -= 31;
                                Month++;
                                if ( DaysInYear > 31 ) { // see if past august
                                    DaysInYear -= 31;
                                    Month++;
                                    if ( DaysInYear > 30 ) { // see if past sept
                                        DaysInYear -= 30;
                                        Month++;
                                        if ( DaysInYear > 31 ) { // see if past oct
                                            DaysInYear -= 31;
                                            Month++;
                                            if ( DaysInYear > 30 ) { // see if past november
                                                DaysInYear -= 30;
                                                Month++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Day = DaysInYear % 32;
    Month = (Month / 10 )* (10) + (Month % 10);
    Day   =   (Day / 10) * (10) + (Day % 10);

    TimeInSec  = TimeInSec % (24 * 60 * 60);
    Hour      = TimeInSec / (60 * 60);
    Hour      = (Hour / 10) * (10) + (Hour % 10);

    TimeInSec = TimeInSec % (60 * 60);
    Minute    = TimeInSec / (60);
    Minute   = (Minute / 10) * (10) + (Minute % 10);

    TimeInSec = TimeInSec % (60);
    Second = TimeInSec;
    Second   = (Second / 10) * (10) + (Second % 10);

    //
    // Update EfiTime variable
    //
    EfiTime.Second = Second;
    EfiTime.Minute = Minute;
    EfiTime.Hour   = Hour;
    EfiTime.Day    = Day;
    EfiTime.Month  = Month;
    EfiTime.Year   = Year;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Exiting %s\n", __FUNCTION__));

    return EfiTime;
}

/**
    This function Builds SEL Sensor Details With Time Stamp

    @param HiiHandle - A handle that was previously registered in the
                       HII Database.
    @param SelRecordData - SEL Record Data

    @return STRING_REF

**/

STRING_REF
BuildSelSensorDetailsWithTimeStamp (
  IN  EFI_HII_HANDLE                HiiHandle,
  IN  IPMI_SEL_EVENT_RECORD_DATA    SelRecordData )
{
    EFI_TIME        EfiTime = {0};
    EFI_STRING      String;
    CHAR16          UnicodeSPrintBuffer[STRING_BUFFER_LENGTH] = {0};
    STRING_REF      StringRefToken = NULL_STRING_REF;
    UINTN           Index = 0;
    CHAR16          CharPtr[STRING_BUFFER_LENGTH];
    UINTN           BufferLength = STRING_BUFFER_LENGTH;
    EFI_STATUS      Status = EFI_SUCCESS;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Inside %s\n", __FUNCTION__));

    //
    // Validate the record type passed
    //
    if (!ValidateRecordType (SelRecordData.RecordType)) {
        return StringRefToken;
    } // End of if loop
    //
    // Check if identical description token has already been
    // created earlier, if so use it
    //
    if (SelRecordData.RecordType == IPMI_SEL_SYSTEM_RECORD) {
        for (Index = 0; Index < gSensorDetailsSavedTokenCount; Index++) {
            if ((SelRecordData.SensorType == gSensorDetailsSavedToken[Index].SensorType) && \
                (SelRecordData.TimeStamp == gSensorDetailsSavedToken[Index].TimeStamp)) {
                return gSensorDetailsSavedToken[Index].Token;
            } // End of if loop
        } // End of for loop
    } // End of if loop

    if (!(SelRecordData.RecordType >= EFI_SEL_OEM_NO_TIME_STAMP_RECORD_START)) {
        //
        // Get EfiTime from Time stamp
        //
        EfiTime = GetEfiTimeFromTimeStamp (SelRecordData.TimeStamp);

        //
        // Build Time Stamp Details
        //
        UnicodeSPrint (
            UnicodeSPrintBuffer,
            STRING_BUFFER_LENGTH,
            L"%02d/%02d/%02d  %02d:%02d:%02d",
            EfiTime.Month, EfiTime.Day, (EfiTime.Year % 100),
            EfiTime.Hour, EfiTime.Minute, EfiTime.Second);
    } else {
        //
        // Build Time Stamp Details
        // static characters are added to denote time stamp not available
        //
        UnicodeSPrint (
            UnicodeSPrintBuffer,
            STRING_BUFFER_LENGTH,
            L"--/--/--  --:--:--");
    } // End of if loop

    if (SelRecordData.RecordType != IPMI_SEL_SYSTEM_RECORD) {

        Status = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_OEM_RECORD_TYPE), CharPtr, &BufferLength, NULL);
        //
        // Build SEL Sensor Details for OEM specific SEL record
        //
        if (!EFI_ERROR(Status)) {
            UnicodeSPrint (
               UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
               STRING_BUFFER_LENGTH,
               CharPtr,
               SelRecordData.RecordType);
        }
        goto SENSOR_TIME_DETAIL_DONE;
    } // End of if loop
    //
    // Get the Description string for Sensor Type
    //
    if ((SelRecordData.SensorType >= RESERVED_SENSOR_TYPE_START) && \
         (SelRecordData.SensorType <= RESERVED_SENSOR_TYPE_END)) {
        String = RESERVED_SENSOR_TYPE_STRING;
    } else if ((SelRecordData.SensorType >= OEM_SPECIFIC_SENSOR_TYPE_START) && \
                  (SelRecordData.SensorType <= OEM_SPECIFIC_SENSOR_TYPE_END)) {
        String = GetDescStringFromValue (
                    SelRecordData.SensorType,
                    gSensorTypeTable,
                    SENSOR_TYPE_COUNT );
    } else {
        String = GetDescStringFromValue (
                    SelRecordData.SensorType,
                    gSensorTypeTable,
                    SENSOR_TYPE_COUNT );
    }

    //
    // Build SEL Sensor Details
    //
    UnicodeSPrint (
        UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
        STRING_BUFFER_LENGTH,
        L"   %s",
        String );

SENSOR_TIME_DETAIL_DONE:
    //
    // Add the string to HII string database and get the token value for it.
    // Return if value of the StringRefToken is NULL_STRING_REF 
    //
    StringRefToken = HiiSetString (
                        HiiHandle,
                        0,
                        UnicodeSPrintBuffer,
                        NULL) ;
    ASSERT (StringRefToken != NULL_STRING_REF);
    if (StringRefToken == NULL_STRING_REF) {
        return StringRefToken;
    }

    //
    // Save the token value for system SEL record so that it can be used for Next identical description
    // and Increment the saved token count
    //
    if (SelRecordData.RecordType == IPMI_SEL_SYSTEM_RECORD) {
        if ((gSensorDetailsSavedTokenCount + 1) < MAX_TOKENS_TO_SAVE) {
            gSensorDetailsSavedToken[gSensorDetailsSavedTokenCount].SensorType = SelRecordData.SensorType;
            gSensorDetailsSavedToken[gSensorDetailsSavedTokenCount].TimeStamp = SelRecordData.TimeStamp;
            gSensorDetailsSavedToken[gSensorDetailsSavedTokenCount].Token = StringRefToken;
            gSensorDetailsSavedTokenCount++;
        }
    }
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Exiting %s\n", __FUNCTION__));

    return StringRefToken;
}

/**
    This function Builds SEL Record Description

    @param HiiHandle - A handle that was previously registered in the
                             HII Database.
    @param SelRecordData - SEL Record Data

    @return STRING_REF

**/

STRING_REF
BuildSelRecordDescription (
  IN  EFI_HII_HANDLE               HiiHandle,
  IN  IPMI_SEL_EVENT_RECORD_DATA   SelRecordData )
{
    CHAR16          UnicodeSPrintBuffer[STRING_BUFFER_LENGTH] = {0};
    EFI_STRING      String;
    STRING_REF      StringRefToken = NULL_STRING_REF;
    UINTN           Index = 0;
    CHAR16          CharPtr[STRING_BUFFER_LENGTH];
    CHAR16          BmcLunString[STRING_BUFFER_LENGTH];
    CHAR16          ChannelString[STRING_BUFFER_LENGTH];
    EFI_SEL_OEM_TIMESTAMP_RECORD SelOemRecord;
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_STATUS      BmcLunStatus = EFI_SUCCESS;
    EFI_STATUS      ChannelStatus = EFI_SUCCESS;
    UINTN           BufferLength = STRING_BUFFER_LENGTH;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Inside %s\n", __FUNCTION__));

    //
    // Check if identical description token has already been
    // created earlier, if so use it
    //
    for (Index = 0; Index < gSelRecordDescSavedTokenCount; Index++) {
        if (CompareMem (&(gSelRecordDescSavedToken[Index].SelRecordData), \
                    &SelRecordData, sizeof(SelRecordData)) == 0) {
            return gSelRecordDescSavedToken[Index].Token;
        } // End of if loop
    } // End of for loop

    //
    // Update the buffer with Hex values in below format.
    // HEX:
    // FE 01 02...
    //
    Status = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_SEL_RECORDS_DATA), CharPtr, &BufferLength, NULL);
    if (!EFI_ERROR(Status)) {
        UnicodeSPrint (
            UnicodeSPrintBuffer,
            STRING_BUFFER_LENGTH,
            CharPtr );
    }
    //
    // Concatenate all HEX values.
    //
    for (Index = 0; Index < sizeof (SelRecordData); Index++) {
        //
        // Introduce new line based on MAX_HEX_VALUES_TO_PRINT_PER_LINE
        //
        if ((Index % MAX_HEX_VALUES_TO_PRINT_PER_LINE) == 0) {
            UnicodeSPrint (
                UnicodeSPrintBuffer + StrLen(UnicodeSPrintBuffer),
                STRING_BUFFER_LENGTH,
                L"\n");
        }
        //
        // Add Hex value to the buffer and include two spaces
        //
        UnicodeSPrint (
            UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
            STRING_BUFFER_LENGTH,
            L"%02X  ",
            *(UINT8*)((UINT8*)&SelRecordData + Index) );
    } //End of for loop

    if (SelRecordData.RecordType != IPMI_SEL_SYSTEM_RECORD) {
        if ((SelRecordData.RecordType >= EFI_SEL_OEM_TIME_STAMP_RECORD_START) &&
            (SelRecordData.RecordType <= EFI_SEL_OEM_TIME_STAMP_RECORD_END)) {
            //
            // Getting OEM SEL event
            //
            CopyMem (
                 &SelOemRecord,
                 &SelRecordData,
                 sizeof (SelOemRecord) );
            //
            // Add Manufacturer ID string
            //
            BufferLength = STRING_BUFFER_LENGTH;
            Status = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_MANUFACTURER_ID), CharPtr, &BufferLength, NULL);
            if (!EFI_ERROR(Status)) {
                UnicodeSPrint (
                     UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
                     STRING_BUFFER_LENGTH,
                     CharPtr,
                     SelOemRecord.ManufacturerId[0] + (SelOemRecord.ManufacturerId[1] << 8) + (SelOemRecord.ManufacturerId[2] << 16));
            }
        } // End of if loop
        goto SENSOR_DETAIL_BUILD_DONE;
    } // End of if loop
    //
    // Generator ID Information
    //
    if ((SelRecordData.GeneratorId & IPMI_SOFTWARE_ID) \
            == SYSTEM_SOFTWARE_ID_PRESENT) {
        UINT8   SoftwareId = SOFTWARE_ID_FROM_GENERATOR_ID(SelRecordData.GeneratorId);
        //
        // Software Type String
        //
        if ((SoftwareId >= BIOS_ID_RANGE_START) && \
                (SoftwareId <= BIOS_ID_RANGE_END)) {
            String = BIOS_SOFTWARE_TYPE_STRING;
        } else if ((SoftwareId >= SMI_HANDLER_ID_RANGE_START) && \
                    (SoftwareId <= SMI_HANDLER_ID_RANGE_END)) {
            String = SMI_HANDLER_SOFTWARE_TYPE_STRING;
        } else if ((SoftwareId >= SMS_ID_RANGE_START) && \
                    (SoftwareId <= SMS_ID_RANGE_END)) {
            String = SMS_SOFTWARE_TYPE_STRING;
        } else if ((SoftwareId >= OEM_ID_RANGE_START) && \
                    (SoftwareId <= OEM_ID_RANGE_END)) {
            String = OEM_SOFTWARE_TYPE_STRING;
        } else if ((SoftwareId >= REMOTE_CONSOLE_ID_RANGE_START) && \
                    (SoftwareId <= REMOTE_CONSOLE_ID_RANGE_END)) {
            String = REMOTE_CONSOLE_SOFTWARE_TYPE_STRING;
        } else if (SoftwareId == TERMINAL_CONSOLE_ID) {
            String = TERMINAL_CONSOLE_SOFTWARE_TYPE_STRING;
        } else {
            String = RESERVED_SOFTWARE_TYPE_STRING;
        }
        //
        // Add Channel number along with Generator string
        //
        BufferLength = STRING_BUFFER_LENGTH;
        Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_GENERATORID_CHANNELNO), CharPtr, &BufferLength, NULL);
        BufferLength = STRING_BUFFER_LENGTH;
        ChannelStatus = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_CHANNEL_NUMBER), ChannelString, &BufferLength, NULL);
        if ((!EFI_ERROR(Status)) && (!EFI_ERROR(ChannelStatus))) {
            UnicodeSPrint (
                UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
                STRING_BUFFER_LENGTH,
                CharPtr,
                String,
                ChannelString,
                CHANNEL_NUMBER_FROM_GENERATOR_ID(SelRecordData.GeneratorId) );
        }
    } else {
        //
        // Add Channel number and LUN along with Generator string
        //
        BufferLength = STRING_BUFFER_LENGTH;
        Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_BMCLUN_GENERATORID_CHANNELNO), CharPtr, &BufferLength, NULL);
        BufferLength = STRING_BUFFER_LENGTH;
        BmcLunStatus = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_BMC_LUN), BmcLunString, &BufferLength, NULL);
        BufferLength = STRING_BUFFER_LENGTH;
        ChannelStatus = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_CHANNEL_NUMBER), ChannelString, &BufferLength, NULL);
        if ((!EFI_ERROR(Status)) && (!EFI_ERROR(BmcLunStatus)) && (!EFI_ERROR(ChannelStatus)))
        UnicodeSPrint (
            UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
            STRING_BUFFER_LENGTH,
            CharPtr,
            BmcLunString,
            LUN_FROM_GENERATOR_ID (SelRecordData.GeneratorId),
            ChannelString,
            CHANNEL_NUMBER_FROM_GENERATOR_ID (SelRecordData.GeneratorId) );
    }

    //
    // Get the Description string for Sensor Number
    //
    if ((SelRecordData.SensorNumber >= RESERVED_SENSOR_NUM_START) && \
         (SelRecordData.SensorNumber <= RESERVED_SENSOR_NUM_END)) {
        String = RESERVED_SENSOR_NUMBER_STRING;
    } else if ((SelRecordData.SensorNumber >= CHASSIS_SPECIFIC_SENSOR_NUM_START) && \
                (SelRecordData.SensorNumber <= CHASSIS_SPECIFIC_SENSOR_NUM_END)) {
        String = CHASSIS_SPECIFIC_NUMBER_STRING;
    } else if ((SelRecordData.SensorNumber >= BOARD_SPECIFIC_SENSOR_NUM_START) && \
               (SelRecordData.SensorNumber <= BOARD_SPECIFIC_SENSOR_NUM_END)) {
        String = BOARD_SPECIFIC_NUMBER_STRING;
    } else if ((SelRecordData.SensorNumber >= OEM_SPECIFIC_SENSOR_NUM_START) && \
                  (SelRecordData.SensorNumber <= OEM_SPECIFIC_SENSOR_NUM_END)) {
        String = GetDescStringFromValue (
                    SelRecordData.SensorNumber,
                    gSensorNumTable,
                    SENSOR_NUM_COUNT );
    } else {
        String = GetDescStringFromValue (
                    SelRecordData.SensorNumber,
                    gSensorNumTable,
                    SENSOR_NUM_COUNT );
    }
    
    //
    // Add Record's Sensor Number and Description string to the buffer
    //
    BufferLength = STRING_BUFFER_LENGTH;
    Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_SENSOR_NUMBER), CharPtr, &BufferLength, NULL);
    if (!EFI_ERROR(Status)) {
        UnicodeSPrint (
            UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
            STRING_BUFFER_LENGTH,
            CharPtr,
            SelRecordData.SensorNumber,
            String );
    }
    //
    // Add Event Description to the buffer
    //
    BufferLength = STRING_BUFFER_LENGTH;
    Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_EVENT_DESCRIPTION), CharPtr, &BufferLength, NULL);
    if (!EFI_ERROR(Status)) {
        UnicodeSPrint (
            UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
            STRING_BUFFER_LENGTH,
            CharPtr );
    }

    //
    // Get Event Description for Sensor specific events based on Event Data
    //
    if (EFI_SEL_EVENT_TYPE(SelRecordData.EventDirType) == \
                                EFI_SEL_EVENT_TYPE_SENSOR_SPECIFIC) {
        UINT8                               SensorTypeIndex = 0;
        BOOLEAN                             SensorSpecOffsetStringUpdated = FALSE;
        BOOLEAN                             MatchOEMEvData2 = FALSE;
        BOOLEAN                             MatchOEMEvData3 = FALSE;
        SENSOR_OFFSET_STRING_TABLE          OffsetTable;
        SENSOR_TYPE_VALUE_TO_DESC_STRING    DescTable;

        //
        // Loop through gSensorTypeValueToDescStringTable to get event Description
        //
        for (SensorTypeIndex = 0; \
                  SensorTypeIndex < SENSOR_TYPE_VALUE_TO_DESC_STRING_TABLE_COUNT; \
                      SensorTypeIndex++) {
            DescTable = gSensorTypeValueToDescStringTable[SensorTypeIndex];
            //
            // Check for SelRecordData Sensor type match with Table entry.
            // If it is matched go further, else continue.
            //
            if ((DescTable.SensorType == SelRecordData.SensorType) == FALSE) {
                continue;
            }

            //
            // Loop through DescTable to get event Description
            //
            for (Index = 0; Index < DescTable.SizeOfTable; Index++) {

                OffsetTable = DescTable.SensorOffsetStringTable[Index];

                //
                // Check for SensorSpecOffsetStringUpdated Flag
                // If it is FALSE, try to match Sensor Offset
                //
                if (SensorSpecOffsetStringUpdated == FALSE) {
                    //
                    // Check and update Sensor specific offset string 
                    //
                    if (UpdateSensorSpecOffsetString (OffsetTable, SelRecordData)) {
                        UnicodeSPrint (
                            UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
                            STRING_BUFFER_LENGTH,
                            L"%s. ",
                            OffsetTable.DescString );
                        //
                        // Make SensorSpecOffsetStringUpdated flag as TRUE
                        //
                        SensorSpecOffsetStringUpdated = TRUE;
                        //
                        // If any one of MatchOEMEvData2 and MatchOEMEvData3 is TRUE
                        // then continue else break the loop.
                        //
                        MatchOEMEvData2 = IsOEMEvData2SelRecordDataValid (SelRecordData.OEMEvData1);
                        MatchOEMEvData3 = IsOEMEvData3SelRecordDataValid (SelRecordData.OEMEvData1);
                        if (MatchOEMEvData2 || MatchOEMEvData3) {
                            continue;
                        } else {
                            break;
                        }
                    } else { // else condition for UpdateSensorSpecOffsetString
                        continue;
                    }
                } else { // else condition for ( SensorSpecOffsetStringUpdated == FALSE )
                    //
                    // If MatchOEMEvData2 is TRUE then check for OEMEvData2 
                    // match in table entry. If matched, Update the string and 
                    // Continue the loop.
                    //
                    if (MatchOEMEvData2 && \
                            UpdateOEMEvData2String (OffsetTable, SelRecordData)) {
                        //
                        // If Or2 is not zero then print associated HEX value.
                        //
                        if (OffsetTable.Or2 != 0x00) {
                            UINT8   NoOfLeftShifts = 0;
                            UINT8   HexValueTobePrinted = 0;

                            //
                            // Get NoOfLeftShifts value from MASK
                            // Loop until BIT0 is 1
                            //
                            while (((OffsetTable.Mask2 >> NoOfLeftShifts) & BIT0) == 0x00){
                                NoOfLeftShifts++;
                            }
                            //
                            // Get the HEX value to be printed
                            //
                            HexValueTobePrinted = ( (SelRecordData.OEMEvData2 & \
                                                    OffsetTable.Mask2) >> NoOfLeftShifts);
                            //
                            // Update string along with HEX value.
                            //
                            UnicodeSPrint (
                                UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
                                STRING_BUFFER_LENGTH,
                                L"%s%02X. ",
                                OffsetTable.DescString, 
                                HexValueTobePrinted );
                        } else { // else condition for (OffsetTable.Or2 != 0x00)
                            //
                            // Update string alone.
                            //
                            UnicodeSPrint (
                                UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
                                STRING_BUFFER_LENGTH,
                                L"%s. ",
                                OffsetTable.DescString);
                        } // End of if-else loop: (OffsetTable.Or2 != 0x00)
                        //
                        // Continue the loop
                        //
                        continue;
                    } // End of if loop: UpdateOEMEvData2String
                    //
                    // If MatchOEMEvData3 is TRUE then check for OEMEvData3 
                    // match in table entry. If matched, Update the string and 
                    // Continue the loop.
                    //
                    if (MatchOEMEvData3 && \
                            UpdateOEMEvData3String (OffsetTable, SelRecordData)) {
                        //
                        // If Or3 is not zero then print associated HEX value.
                        //
                        if (OffsetTable.Or3 != 0x00) {
                            UINT8   NoOfLeftShifts = 0;
                            UINT8   HexValueTobePrinted = 0;

                            //
                            // Get NoOfLeftShifts value from MASK
                            // Loop until BIT0 is 1
                            //
                            while ( ((OffsetTable.Mask3 >> NoOfLeftShifts) & BIT0) == 0x00 ){
                                NoOfLeftShifts++;
                            }
                            //
                            // Get the HEX value to be printed
                            //
                            HexValueTobePrinted = ( (SelRecordData.OEMEvData3 & \
                                                    OffsetTable.Mask3) >> NoOfLeftShifts);
                            //
                            // Update string along with HEX value.
                            //
                            UnicodeSPrint (
                                UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
                                STRING_BUFFER_LENGTH,
                                L"%s%02X. ",
                                OffsetTable.DescString, 
                                HexValueTobePrinted );
                        } else { // else condition for (OffsetTable.Or3 != 0x00)
                            //
                            // Update string alone.
                            //
                            UnicodeSPrint (
                                UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
                                STRING_BUFFER_LENGTH,
                                L"%s. ",
                                OffsetTable.DescString);
                        } // End of if-else loop: (OffsetTable.Or3 != 0x00)
                        //
                        // Continue the loop
                        //
                        continue;
                    } // End of if loop: UpdateOEMEvData3String
                } // End of if-else loop: SensorSpecOffsetStringUpdated
            } // End of for loop: Size of DescTable
            //
            // Control comes here only when Sensor Desc string 
            // is updated. So break the loop.
            //
            break;
        } // End of for loop: SensorTypeIndex
    } // End of if loop

    //
    // Add Record Type to the buffer
    //
    BufferLength = STRING_BUFFER_LENGTH;
    Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_RECORD_TYPE), CharPtr, &BufferLength, NULL);
    if (!EFI_ERROR(Status)) {
        UnicodeSPrint (
            UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
            STRING_BUFFER_LENGTH,
            CharPtr,
            SelRecordData.RecordType );
    }

    //
    // Get the string for Event Dir and add it to the buffer
    //
    BufferLength = STRING_BUFFER_LENGTH;
    if (EFI_SEL_EVENT_DIR(SelRecordData.EventDirType) == EFI_SEL_EVENT_DIR_ASSERTION_EVENT) {
        Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_ASSERTION_EVENT), CharPtr, &BufferLength, NULL);
    } else {
        Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_DEASSERTION_EVENT), CharPtr, &BufferLength, NULL);
    }
    if (!EFI_ERROR(Status)) {
        UnicodeSPrint (
            UnicodeSPrintBuffer + StrLen (UnicodeSPrintBuffer),
            STRING_BUFFER_LENGTH,
            L"%s. ",
            CharPtr );
    }

SENSOR_DETAIL_BUILD_DONE:
    //
    // Add the string to HII string database and get the token value for it.
    // Return if value of the StringRefToken is NULL_STRING_REF 
    //
    StringRefToken = HiiSetString (
                        HiiHandle,
                        0,
                        UnicodeSPrintBuffer,
                        NULL);
    ASSERT (StringRefToken != NULL_STRING_REF);
    if (StringRefToken == NULL_STRING_REF) {
        return StringRefToken;
    }

    //
    // Save the token value so that it can be used for Next identical description
    // and Increment the saved token count
    //
    if ((gSelRecordDescSavedTokenCount + 1) < MAX_TOKENS_TO_SAVE) {
        CopyMem (
            &(gSelRecordDescSavedToken[gSelRecordDescSavedTokenCount].SelRecordData),
            &SelRecordData,
            sizeof(SelRecordData) );
        gSelRecordDescSavedToken[gSelRecordDescSavedTokenCount].Token = StringRefToken;
        gSelRecordDescSavedTokenCount++;
    }

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Exiting %s\n", __FUNCTION__));

    return StringRefToken;
}

/**
    This function,
    1. Builds SEL Sensor Details With Time Stamp
    2. Builds SEL Record Description
    3. Inserts an error into the form set by updating Current opcode
    4. Increments the ErrorCount

    @param  HiiHandle - A handle that was previously registered in the
                              HII Database.
    @param  SelRecordData - SEL Record Data

    @return EFI_STATUS
    @retval EFI_OUT_OF_RESOURCES Cannot add any more errors
    @retval EFI_SUCCESS Successful completion of the function

**/

EFI_STATUS
AddSelRecordToFormSet (
  IN  EFI_HII_HANDLE                HiiHandle,
  IN  IPMI_SEL_EVENT_RECORD_DATA    SelRecordData )
{
    ERROR_LIST          *CurrentOpCode = &gErrorManager->ErrorList[gErrorManager->ErrorCount];
    STRING_REF          SensorDetailsToken;
    STRING_REF          SelRecordDescriptionToken;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Inside %s\n", __FUNCTION__));

    //
    // Build SEL Sensor Details With Time Stamp
    //
    SensorDetailsToken = BuildSelSensorDetailsWithTimeStamp (
                            HiiHandle,
                            SelRecordData );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: SensorDetailsToken: %X \n", SensorDetailsToken));
    ASSERT (SensorDetailsToken != NULL_STRING_REF);
    if (SensorDetailsToken == NULL_STRING_REF) {
        return EFI_OUT_OF_RESOURCES;
    }
    //
    // Build SEL Record Description
    //
    SelRecordDescriptionToken = BuildSelRecordDescription (
                                    HiiHandle,
                                    SelRecordData );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: SelRecordDescriptionToken: %X \n", SelRecordDescriptionToken));
    ASSERT (SelRecordDescriptionToken != NULL_STRING_REF);
    if (SelRecordDescriptionToken == NULL_STRING_REF) {
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Insert an error into the form set by updating Current opcode
    //
    CurrentOpCode->Handle = (VOID*) HiiHandle;
    CurrentOpCode->Summary = SensorDetailsToken;
    CurrentOpCode->DetailInfo = SelRecordDescriptionToken;
    //
    // Increment the ErrorCount
    //
    gErrorManager->ErrorCount++;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Exiting %s\n", __FUNCTION__));

    return EFI_SUCCESS;
}

/**
    This function,
    1. Gets HII Browser Data
    2. Updates the ViewSelLogFlag value in ServerMgmtConfiguration
    3. Sets HII Browser Data with updated ServerMgmtConfiguration

    @param ViewSelLogFlag - ViewSelLogFlag variable

    @retval VOID

    @note  Check VIEW_SEL_LOG_FLAG enum for details about the Flag

**/

VOID
SetViewSelLogFlag (
  IN  VIEW_SEL_LOG_FLAG   ViewSelLogFlag )
{
    SERVER_MGMT_CONFIGURATION_DATA  ServerMgmtConfiguration;
    UINTN                           Size = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
    EFI_STATUS                      Status;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Inside %s\n", __FUNCTION__));

    //
    // Get HII Browser Data
    //
    Status = HiiLibGetBrowserData (
                &Size,
                &ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"ViewSelLog: Status of HiiLibGetBrowserData(): %r\n",Status));
    ASSERT_EFI_ERROR (Status);
    if ( EFI_ERROR (Status) ) {
        return;
    }
    //
    // Update the ViewSelLogFlag value in ServerMgmtConfiguration
    //
    ServerMgmtConfiguration.ViewSelLogFlag = ViewSelLogFlag;
    //
    // Set HII Browser Data with updated ServerMgmtConfiguration
    //
    Status = HiiLibSetBrowserData (
                Size,
                &ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"ViewSelLog: Status of HiiLibSetBrowserData(): %r\n",Status));
    ASSERT_EFI_ERROR (Status);
    if ( EFI_ERROR (Status) ) {
        return;
    }

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Exiting %s\n", __FUNCTION__));
}

/**
    This function,
    1. Reads the SEL records from BMC
    2. Add SEL record to Form Set
    3. Updates the ErrorManager variable to show the Error logs in Setup
    4. Updates the Setup String tokens

    @param HiiHandle - A handle that was previously registered in the
                               HII Database.
    @param Class - Formset Class of the Form Callback Protocol passed in
    @param SubClass - Formset sub Class of the Form Callback Protocol passed in
    @param Key - Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS
    @retval EFI_UNSUPPORTED If the wrong Key value is passed in
    @retval EFI_SUCCESS Successful completion of the function

**/

EFI_STATUS
ViewSelLogCallbackFunction (
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  UINT16            Class,
  IN  UINT16            SubClass,
  IN  UINT16            Key )
{
    EFI_STATUS                  Status;
    static BOOLEAN              IsFirstCall = TRUE;
    UINTN                       MaxSelPerForm;
    IPMI_GET_SEL_INFO_RESPONSE  SelInfo;
    UINT8                       SelInfoSize = sizeof (SelInfo);
    UINT8                       ViewSelLogFlag = PrintColumnTitleString;
    EFI_GET_SEL_ENTRY           SelEntry = {0};
    EFI_GET_SEL_RESPONSE        SelResponseData = {0};
    UINT8                       SelResponseSize;
    CALLBACK_PARAMETERS         *CallbackParameters = GetCallbackParameters ();
    UINT8                       CompletionCode;
    UINT32                      Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINTN                       Size = 0;
    CHAR16                      CharPtr[STRING_BUFFER_LENGTH];
    UINTN                       BufferLength = STRING_BUFFER_LENGTH;
    AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr = NULL;
    CHAR16                      MsgBoxTitle[STRING_BUFFER_LENGTH];
    CHAR16                      Message[STRING_BUFFER_LENGTH];
    UINT8                       MsgBoxSel;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Inside %s\n", __FUNCTION__));

    //
    // Check for the key and Return if Key value does not match
    //
    if ((Key != VIEW_SEL_LOG_FORM_KEY) && \
         (Key != VIEW_MORE_SEL_LOG_KEY)) {
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"ViewSelLog: Callback function is called with \
                Wrong Key Value. Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) {
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"ViewSelLog: CallbackParameters->Action != \
                EFI_BROWSER_ACTION_CHANGING... so return EFI_SUCCESS\n"));
        return  EFI_SUCCESS;
    }

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: Key: %x \n", Key));

    //
    // Locate Ami Post Manager Protocol and if Status is not an error display the Message box else continue without Message box.
    //
    Status = gBS->LocateProtocol (
                      &gAmiPostManagerProtocolGuid,
                      NULL,
                      (VOID **) &AmiPostMgr
                  );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: LocateProtocol for gAmiPostManagerProtocolGuid Status: %r.\n", Status));

    if (!EFI_ERROR (Status)) {
        //
        // Get the Message box strings and display Message box if returned status is not an error.
        //
        Status = IpmiHiiGetString (
                     HiiHandle,
                     STRING_TOKEN(STR_VIEW_SEL_LOG_TITLE),
                     MsgBoxTitle,
                     &BufferLength,
                     NULL
                 );
        if (!EFI_ERROR (Status)) {
            Status = IpmiHiiGetString (
                         HiiHandle,
                         STRING_TOKEN(STR_VIEW_SEL_LOG_PROMPT),
                         Message,
                         &BufferLength,
                         NULL
                     );
            if (!EFI_ERROR (Status)) {
                Status = AmiPostMgr->DisplayMsgBox (
                                         MsgBoxTitle,
                                         Message,
                                         MSGBOX_TYPE_YESNO,
                                         &MsgBoxSel
                                     );
                SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: DisplayMsgBox Status: %r.\n", Status));
                SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: MsgBoxSel value: %x.\n", MsgBoxSel));
                //
                // If Status is an error or user selects 'no' in message box then return EFI_ABORTED.
                //
                if (MsgBoxSel == MSGBOX_NO) {
                    return EFI_ABORTED;
                }
            }
        }
    }

    //
    // Send GET_SEL_INFO to get the SEL count.
    //
    Status = SendIpmiCommand (
                IPMI_NETFN_STORAGE,
                BMC_LUN,
                IPMI_STORAGE_GET_SEL_INFO,
                NULL,
                0,
                (UINT8 *) &SelInfo,
                (UINT8 *) &SelInfoSize,
                &CompletionCode );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: IPMI_STORAGE_GET_SEL_INFO Status: %r\n", Status));

    //
    // If error is returned from SendIpmiCommand then 
    // Set PrintNoProtocolFoundError flag and goto DONE.
    //
    if (EFI_ERROR (Status)) {
        ViewSelLogFlag = PrintNoProtocolFoundError;
        goto DONE;
    } 

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: SelInfo.NoOfEntries: %x \n", SelInfo.NoOfEntries));

    //
    // Update No Of Log Entries in STR_SYSTEM_ELOG_COUNT_STRING
    //
    BufferLength = STRING_BUFFER_LENGTH;
    Status = IpmiHiiGetString ( HiiHandle, STRING_TOKEN(STR_LOG_ENTRY_IN_SEL), CharPtr, &BufferLength, NULL);
    if(!EFI_ERROR(Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN (STR_SYSTEM_ELOG_COUNT_STRING),
            CharPtr,
            SelInfo.NoOfEntries );
    }

    //
    // If No Of Log Entries is zero then set PrintNoRecordsFoundString flag
    // and goto DONE.
    //
    if (SelInfo.NoOfEntries == 0x00) {
        ViewSelLogFlag = PrintNoRecordsFoundString;
        goto DONE;
    }

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: IsFirstCall: %x\n", IsFirstCall));

    //
    // For first call, 
    // 1. Allocate Memory for gSensorDetailsSavedToken and gSelRecordDescSavedToken
    // 2. Make IsFirstCall as FALSE.
    //
    if (IsFirstCall == TRUE) {

        gSensorDetailsSavedToken = AllocateZeroPool ( 
                                    sizeof(SEL_SENSOR_DETAILS_TOKEN_LOOKUP_TABLE) * \
                                    MAX_TOKENS_TO_SAVE );
        if (gSensorDetailsSavedToken == NULL) {
            SERVER_IPMI_TRACE((TRACE_ALWAYS, "Failed to allocate memory for gSensorDetailsSavedToken! \n"));
            return EFI_OUT_OF_RESOURCES;
        }

        gSelRecordDescSavedToken = AllocateZeroPool ( 
                                    sizeof(SEL_RECORD_DESC_TOKEN_LOOKUP_TABLE) * \
                                    MAX_TOKENS_TO_SAVE );
        if (gSelRecordDescSavedToken == NULL) {
            SERVER_IPMI_TRACE((TRACE_ALWAYS, "Failed to allocate memory for gSelRecordDescSavedToken! \n"));
            return EFI_OUT_OF_RESOURCES;
        }

        IsFirstCall = FALSE;
    }

    //
    // Allocate Memory for gErrorManager
    //
    gErrorManager = AllocateZeroPool (MAX_ERROR_MANAGER_SIZE);
    if (gErrorManager == NULL) {
        SERVER_IPMI_TRACE((TRACE_ALWAYS, "Failed to allocate memory for gErrorManager! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Calculate MaxSelPerForm value from SEL_MAX_ERROR_MANAGER_SIZE value.
    //
    MaxSelPerForm = ((SEL_MAX_ERROR_MANAGER_SIZE - sizeof(ERROR_MANAGER)) / sizeof(ERROR_LIST));
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: MaxSelPerForm: %x\n", MaxSelPerForm));

    //
    // If the Key value is VIEW_MORE_SEL_LOG_KEY then
    // get NextSelRecordId value from gRecordIdToViewMoreLog.
    //
    if (Key == VIEW_MORE_SEL_LOG_KEY) {
        SelResponseData.NextSelRecordId = gRecordIdToViewMoreLog;
    }
    //
    // Read Complete SEL record
    //
    SelEntry.BytesToRead = EFI_COMPLETE_SEL_RECORD;

    //
    // Loop to read the SEL records from BMC
    //
    while (SelResponseData.NextSelRecordId != 0xFFFF) {
        //
        // Fill Record Id to get the SEL record
        //
        SelEntry.SelRecID[0] = (UINT8)SelResponseData.NextSelRecordId;
        SelEntry.SelRecID[1] = (UINT8)(SelResponseData.NextSelRecordId >> 8);
        SelResponseSize = sizeof (EFI_GET_SEL_RESPONSE);

        SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: RecordId of the event to be read: 0x%02X%02X \n", SelEntry.SelRecID[1], SelEntry.SelRecID[0] ));

        //
        // Send Get SEL Entry command
        //
        Status = SendIpmiCommand (
                    IPMI_NETFN_STORAGE,
                    BMC_LUN,
                    EFI_STORAGE_GET_SEL_ENTRY,
                    (UINT8 *) &SelEntry,
                    sizeof (SelEntry),
                    (UINT8 *) (&SelResponseData),
                    (UINT8 *) &SelResponseSize,
                    &CompletionCode );

        SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: EFI_STORAGE_GET_SEL_ENTRY Status: %r Next RecordId: %x \n", Status, SelResponseData.NextSelRecordId ));
        //
        // For Error case, goto DONE.
        //
        if (EFI_ERROR (Status)) {
            goto DONE;
        }
        //
        // Add SEL record to Form Set
        //
        Status = AddSelRecordToFormSet (
                    HiiHandle,
                    SelResponseData.RecordData );
        ASSERT_EFI_ERROR (Status);
        //
        // For Error case, goto DONE.
        //
        if (EFI_ERROR (Status)) {
            goto DONE;
        }
        //
        // If ErrorCount reaches MaxSelPerForm then
        // 1. Update gRecordIdToViewMoreLog with NextSelRecordId
        // 2. Set PrintViewMoreSelLogString flag and
        // 3. goto DONE
        //
        if (MaxSelPerForm <= gErrorManager->ErrorCount) {
            gRecordIdToViewMoreLog = SelResponseData.NextSelRecordId;
            ViewSelLogFlag = PrintViewMoreSelLogString;
            goto DONE;
        }
    } // while loop

DONE:
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: ViewSelLogFlag: %x \n", ViewSelLogFlag));
    //
    // Update ViewSelLogFlag value in Browser data
    //
    SetViewSelLogFlag (ViewSelLogFlag);

    //
    //Get ErrorManager variable attribute
    //
    Status = gRT->GetVariable (
                L"ErrorManager",
                &gErrorManagerGuid,
                &Attribute,
                &Size,
                gErrorManager);
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: gRT->GetVariable Status: %r \n", Status));
    //
    //  If there are no SEL records or if protocol is not installed
    //
    if ((ViewSelLogFlag == PrintNoProtocolFoundError) || (ViewSelLogFlag == PrintNoRecordsFoundString)) {
        //
        // Clear the ErrorManager variable
        //
        Status = gRT->SetVariable (
                    L"ErrorManager",
                    &gErrorManagerGuid,
                    Attribute,
                    0,
                    NULL);
    } else {
        //
        // Set the ErrorManager variable to show the Error logs in Setup
        //
        Status = gRT->SetVariable (
                    L"ErrorManager",
                    &gErrorManagerGuid,
                    Attribute,
                    ( sizeof(ERROR_MANAGER) + (gErrorManager->ErrorCount) * sizeof(ERROR_LIST) ),
                    gErrorManager );
    }
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: gRT->SetVariable Status: %r \n", Status));
    //
    // Free the memory allocated for Error Manager
    //
    gBS->FreePool(gErrorManager);

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "ViewSelLog: ViewSelLogCallbackFunction() Exit\n"));

    return EFI_SUCCESS;
}

/**
    This function validate the SEL record type passed.

    @param  RecordType - Record Type to validate.

    @return BOOLEAN
    @retval TRUE Valid Record Type
    @retval FALSE Invalid Record Type

**/

BOOLEAN ValidateRecordType (
  IN UINT8 RecordType)
{
    BOOLEAN IsValid = FALSE;

    if (RecordType >= EFI_SEL_OEM_TIME_STAMP_RECORD_START) {
        IsValid = TRUE;
    } else if (RecordType == IPMI_SEL_SYSTEM_RECORD) {
        IsValid = TRUE;
    }
    return IsValid;
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
