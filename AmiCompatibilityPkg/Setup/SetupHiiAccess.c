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

/**
 * @file
 * This file contains supporting functions for the HiiAccess Protocol.
 */
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Library/PrintLib.h>

/// Internal structure for storing the length of a reserved name and the string of the reserved name
typedef struct _NAME_PATTERN
{
    UINTN Length;   ///< Length, in bytes, of the Tag 
    CHAR16 *Tag;    ///< Pointer to the unicode string of this name pattern instance
} NAME_PATTERN;

static const NAME_PATTERN GuidPattern   = { 10, L"GUID=" };
static const NAME_PATTERN NamePattern   = { 10, L"NAME=" };
static const NAME_PATTERN PathPattern   = { 10, L"PATH=" };
static const NAME_PATTERN AltCfgPattern = { 14, L"ALTCFG=" };
static const NAME_PATTERN OffsetPattern = { 14, L"OFFSET=" };
static const NAME_PATTERN WidthPattern  = { 12, L"WIDTH=" };
static const NAME_PATTERN ValuePattern  = { 12, L"VALUE=" };

/**
 * Verify the passed String contains the pattern contained by in the structure Pattern.
 *
 * @param String Pointer to the string buffer to check for the pattern
 * @param Pattern The pattern to check for in the buffer
 *
 * @return BOOLEAN
 * @retval TRUE The pattern is contained in the string buffer
 * @retval FALSE The pattern is not contained in the string buffer
 */
static BOOLEAN CheckPattern(CONST CHAR16 *String, CONST NAME_PATTERN *Pattern)
{
    if(CompareMem(String, Pattern->Tag, Pattern->Length))
        return FALSE;
    else
        return TRUE;
}

/**
 * Check if the passed CHAR16 is a hexadecimal value
 *
 * @param Char The character to verify if it is hex
 *
 * @return BOOLEAN
 * @retval TRUE The character is a valid hex digit
 * @retval FALSE The character is not a valid hex digit
 */
static BOOLEAN CheckIfHexDigit(CHAR16 Char)
{
    BOOLEAN Status = TRUE;

    if(Char < 0x30 || Char > 0x66)
        Status = FALSE;

    if(Char > 0x39 && Char < 0x41)
        Status = FALSE;

    if(Char > 0x46 && Char < 0x61)
        Status = FALSE;

    return Status;
}

/**
 * Convert the passed ASCII character to its hexadecimal integer
 *
 * @param Char The ASCII character to convert to an integer
 *
 * @return UINT8 The integer value of the ASCII character
 */
static UINT8 HexValue(CHAR8 Char)
{
    // characters 0...9
    if(Char >= 0x30 && Char <= 0x39)
        return Char - 0x30;

    // characters A...F
    if(Char >= 0x41 && Char <= 0x46)
        return Char - 0x37;

    // Characters a...f
    return Char - 0x57;
}

/**
 * Convert a length of the passed Unicode string buffer into a buffer of hexadecimal values
 *
 * @param String The string buffer
 * @param StringSize The number of string buffer characters to convert
 * @param Block The output buffer to store the converted characters
 */
static VOID StringToBlock(  IN CHAR16 *String,
                            IN UINTN StringSize,
                            OUT UINT8 *Block)
{
    UINT8 LowNibble, HiNibble;
    UINTN i, j;

    j = 0;

    for(i = 0; i < StringSize; i += 2)
    {
        // odd number need to fill with zeroes
        if(i == StringSize - 1) {
            HiNibble = 0;
            LowNibble = HexValue((UINT8)String[i]);
        } else {
            HiNibble = HexValue((UINT8)String[i]);
            LowNibble = HexValue((UINT8)String[i + 1]);
        }
        Block[j] = (HiNibble << 4) | LowNibble;
        j++;
    }
    return;
}

/**
 * Convert the string to a null terminated Unicode string buffer
 *
 * @param String The input string to convert
 * @param Size The number of characters to convert
 * @param Block The buffer to store the output
 */
static VOID StringToChar16( IN CHAR16 *String,
                            IN OUT UINTN  *Size,
                            OUT CHAR16 *Block)
{
    UINTN i = 0;
    UINTN j = 0;

    while( CheckIfHexDigit(String[i]) && CheckIfHexDigit(String[i + 1]) &&
           CheckIfHexDigit(String[i + 2]) && CheckIfHexDigit(String[i + 3]) &&
           j < *Size - 1 )
    {
        Block[j]  = HexValue((UINT8)String[i]) & 0x000f;
        Block[j] <<= 4;
        Block[j] |= HexValue((UINT8)String[i + 1]);
        Block[j] <<= 4;
        Block[j] |= HexValue((UINT8)String[i + 2]);
        Block[j] <<= 4;
        Block[j] |= HexValue((UINT8)String[i + 3]);

        i += 4;
        j++;
    }
    *Size = j;      //number of words processed
    Block[j] = 0;   //put null-terminator
}

/**
 * Convert the input string buffer into a DevicPath and return the pointer to the device path
 * and the size of the device path
 *
 * @param ConfigString The input string containing the device path
 * @param DevicePath The output device path pointer
 * @param DpLength The length of the device path
 *
 * @return EFI_STATUS
 * @retval EFI_OUT_OF_RESOURCES Memory could not be allocated for the device path
 * @retval EFI_SUCCESS The output parameters are valid
 */
static EFI_STATUS ExtractDevicePath(CHAR16 *ConfigString, UINT8 **DevicePath, UINTN *DpLength)
{
    EFI_STATUS Status;
    UINTN Length = 0;
    UINTN i = 5;        //set pointer to skip first four symbols "PATH="

    while(CheckIfHexDigit(ConfigString[i]))      //determine device path length in characters
    {
        Length++;
        i++;
    }

    Status = gBS->AllocatePool( EfiBootServicesData, (Length / 2), (VOID**)DevicePath);
    if (EFI_ERROR(Status))
        return Status;

    StringToBlock(&ConfigString[5], Length, *DevicePath);
    *DpLength = Length / 2;

    return EFI_SUCCESS;
}

/**
 * Validate the device path to ensure it is not malformed
 *
 * @param DevicePath Pointer to the device path
 * @param DpLength Length of the device path
 *
 * @return BOOLEAN
 * @retval TRUE The device path is not malformed
 * @retval FALSE The device path is malformed
 */
static BOOLEAN CheckDevicePath( IN UINT8 *DevicePath,
                                IN UINTN DpLength)
{
    EFI_DEVICE_PATH_PROTOCOL* TempDp;

    TempDp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;

    if (DevicePath == NULL)
        return FALSE;

    // Valid device path must have at least one node
    if (DpLength < 4)
        return FALSE;

    // Valid device path protocol cannot be longer than binary data
    if (*(UINT16*)TempDp->Length > DpLength)
        return FALSE;

    // Valide device path types
    if (TempDp->Type != HARDWARE_DEVICE_PATH    &&
        TempDp->Type != ACPI_DEVICE_PATH        &&
        TempDp->Type != MESSAGING_DEVICE_PATH   &&
        TempDp->Type != MEDIA_DEVICE_PATH       &&
        TempDp->Type != BBS_DEVICE_PATH         &&
        TempDp->Type != END_DEVICE_PATH_TYPE)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Using the passed ConfigString, locate the device path node and then find the
 * handle associated with the device path
 *
 * @param ConfigString The string buffer containing the device path node
 * @param Handle The handle of the device associated with the device path contained in the config string
 *
 * @return EFI_STATUS
 * @retval EFI_NOT_FOUND The device path in the config string did not have a handle associated with it
 * @retval EFI_SUCCESS The handle of the device path in the config string was returned
 */
static EFI_STATUS FindDeviceHandle( IN CHAR16 *ConfigString,
                                    OUT EFI_HANDLE *Handle)
{
    UINTN i = 0;
    UINTN DpLength;
    UINT8 *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL* TempDp;
    EFI_STATUS Status;

    //scan config string to find PATH pattern
    do
    {
        while((ConfigString[i] != L'&') && (ConfigString[i] != 0))
            i++;

        if(ConfigString[i] == 0)
            return EFI_NOT_FOUND;

        i++;            //shift pointer to next symbol
        if(CheckPattern(&ConfigString[i], &PathPattern))
            break;
    } while(1);

    //if we get here, we have pointer set on PATH pattern
    Status = ExtractDevicePath(&ConfigString[i], &DevicePath, &DpLength);
    if(EFI_ERROR(Status))
        return Status;

    if (CheckDevicePath(DevicePath, DpLength)) {
        TempDp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
        Status = gBS->LocateDevicePath (
                          &gEfiDevicePathProtocolGuid,
                          (EFI_DEVICE_PATH_PROTOCOL**)&TempDp,
                          Handle
                          );
    } else {
        Status = EFI_NOT_FOUND;
    }

    gBS->FreePool(DevicePath);

    return Status;
}

/**
 * Decode the request string and return a Results string that contains the requested information from the Request string
 *
 * @param This Pointer to the EFI_HII_CONFIG_ACCESS_PROTOCOL
 * @param Request Pointer to the string buffer that contains the request for information
 * @param Progress Pointer that contains the progress of parsing the Request string
 * @param Results Pointer to a newly created string that contains the requested information
 *
 * @return EFI_STATUS
 * @retval EFI_INVALID_PARAMETER A parameter was not valid, or an error occurred while processing the request
 * @retval EFI_SUCCESS The Results pointer contains the requested information
 *
 */
EFI_STATUS HiiAccessExtractConfig(  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
                                    IN CONST EFI_STRING Request,
                                    OUT EFI_STRING *Progress,
                                    OUT EFI_STRING *Results
)
{
    EFI_STATUS  Status;
    UINTN       Length;
    EFI_GUID    Guid;
    CHAR16      Name[80];
    CHAR16      *Pointer = Request;
    UINT8       *Var = NULL;
    CHAR16      *ErrorMarker;

    EFI_HII_CONFIG_ROUTING_PROTOCOL *ConfigRouting = NULL;
    if(Request == NULL) {
        *Progress = NULL;
        return EFI_INVALID_PARAMETER;
    }

    Status = gBS->LocateProtocol(&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID**)&ConfigRouting);
    if(EFI_ERROR(Status))
        return Status;

    // Check syntax for guid
    if(!CheckPattern(Request, &GuidPattern)) {
        *Progress = Request;
        return EFI_INVALID_PARAMETER;
    }

    // Extract guid
    ErrorMarker = Pointer;

    // Skip "GUID=" pattern
    Pointer += 5;

    // Length of guid in symbols
    Length = 32;
    StringToBlock(Pointer, Length, (UINT8 *)&Guid);
    Pointer += Length;
    if(*Pointer != L'&') {
        DEBUG((DEBUG_ERROR, "HII Extract Config: Guid extraction failed\n"));
        *Progress = ErrorMarker;
        return EFI_INVALID_PARAMETER;
    }

    // Check syntax for name
    if(!CheckPattern(Pointer + 1, &NamePattern)) {
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }

    // Extract name
    ErrorMarker = Pointer;

    // Skip '&NAME=' pattern
    Pointer += 6;
    Length = 80;
    StringToChar16(Pointer, &Length, Name);
    Pointer += (Length * 4);
    if(*Pointer != L'&') {
        DEBUG((DEBUG_ERROR, "HII Extract Config: Name extraction failed\n"));
        *Progress = ErrorMarker;
        return EFI_INVALID_PARAMETER;
    }

    //get variable by name and GUID from NVRAM
    Length = 0;
    Status = gRT->GetVariable(Name, &Guid, NULL, &Length, Var);
    if(Status == EFI_BUFFER_TOO_SMALL)
    {
        gBS->AllocatePool(EfiBootServicesData, Length, (VOID**)&Var);
        Status = gRT->GetVariable(Name, &Guid, NULL, &Length, Var);
    }
    if(EFI_ERROR(Status)) {
        *Progress = Request;
        return Status;
    }

    //find start of OFFSET/WIDTH/VALUE block

    //skip '&'
    Pointer++;

    //skip PATH
    while((*Pointer != L'&') && (*Pointer != 0))
            Pointer++;

    //OFFSET/WIDTH blocks are missed
    if(*Pointer == 0) {
        UINTN TempSize;
        CHAR16 *TempString;
        CHAR16 *TempString2;

        TempSize = (StrLen(L"&OFFSET=0&WIDTH=") + sizeof(Length) + 1) * sizeof(CHAR16);
        Status = gBS->AllocatePool(EfiBootServicesData, TempSize, (VOID**)&TempString);
        if(EFI_ERROR(Status))
            return EFI_OUT_OF_RESOURCES;
        gBS->SetMem(TempString, TempSize, 0);

        UnicodeSPrint (TempString, TempSize, L"&OFFSET=0&WIDTH=%x", Length);

        TempSize += StrSize(Request);
        Status = gBS->AllocatePool(EfiBootServicesData, TempSize, (VOID**)&TempString2);
        if(EFI_ERROR(Status))
            return EFI_OUT_OF_RESOURCES;
        gBS->SetMem(TempString2, TempSize, 0);

        StrCat(TempString2, Request);
        StrCat(TempString2, TempString);

        Status = ConfigRouting->BlockToConfig(ConfigRouting, TempString2, Var, Length, Results, Progress);
        *Progress = Request + StrLen(Request); //set Progress to end of ConfigHdr
        gBS->FreePool(TempString);
        gBS->FreePool(TempString2);
    } else {                                        //OFFSET/WIDTH block found, let's check if it's correct
        if(!CheckPattern(Pointer + 1, &OffsetPattern)) {
            *Progress = Pointer;
            return EFI_INVALID_PARAMETER;
        }
        Status = ConfigRouting->BlockToConfig(ConfigRouting, Request, Var, Length, Results, Progress);
    }

    gBS->FreePool(Var);

    return Status;
}

/**
 * Convert the Configuration string into a appropriate information and then store the data
 * contained in it to the correct medium
 *
 * @param This Pointer to the EFI_HII_CONFIG_ACCESS_PROTOCOL
 * @param Configuration Pointer to the Null terminated string that represents the configuration request
 * @param Progress A pointer to the progress of parsing the Configuration request
 *
 * @return EFI_STATUS
 * @retval EFI_INVALID_PARAMETER A parameter was not valid, or an error occurred while processing the request
 * @retval EFI_SUCCESS The Results pointer contains the requested information
 */
EFI_STATUS HiiAccessRouteConfig( IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
                                        IN CONST EFI_STRING Configuration,
                                        OUT EFI_STRING *Progress)
{
    EFI_STATUS  Status;
    UINTN       Length;
    UINTN       VarSize;
    EFI_GUID    Guid;
    CHAR16      Name[80];
    CHAR16      *Pointer = Configuration;
    UINT8       *Var = NULL;
    EFI_HII_CONFIG_ROUTING_PROTOCOL *ConfigRouting;
    EFI_HANDLE  Handle;
    UINT32      Attributes;

    if(Configuration == NULL)
    {
        *Progress = NULL;
        return EFI_INVALID_PARAMETER;
    }

    Status = gBS->LocateProtocol(&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID**)&ConfigRouting);
    if(EFI_ERROR(Status))
        return Status;

    //check syntax for guid
    if(!CheckPattern(Configuration, &GuidPattern))
    {
        *Progress = Configuration;
        return EFI_INVALID_PARAMETER;
    }

    //check proper device path
    Status = FindDeviceHandle(Configuration, &Handle);
    if(EFI_ERROR(Status))
        return EFI_NOT_FOUND;

    //extract guid
    Pointer += 5;       //skip "GUID=" pattern
    Length = 32;        //length of guid in bytes
    StringToBlock(Pointer, Length, (UINT8 *)&Guid);
    Pointer += Length;
    if(*Pointer != L'&')
    {
        DEBUG((DEBUG_ERROR, "HII Route Config: Guid extraction failed\n"));
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }

    //check syntax for name
    if(!CheckPattern(Pointer + 1, &NamePattern))
    {
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }

    //extract name
    Pointer += 6;       //skip '&NAME=' pattern
    Length = 80;
    StringToChar16(Pointer, &Length, Name);
    Pointer += (Length * 4);
    if(*Pointer != L'&')
    {
        DEBUG((DEBUG_ERROR, "HII Route Config: Name extraction failed\n"));
        *Progress = Pointer;
        return EFI_INVALID_PARAMETER;
    }

    //get variable by name and guid from nvram
    Length = 0;
    Status = gRT->GetVariable(Name, &Guid, &Attributes, &Length, Var);
    if(Status == EFI_BUFFER_TOO_SMALL)
    {
        gBS->AllocatePool(EfiBootServicesData, Length, (VOID**)&Var);
        Status = gRT->GetVariable(Name, &Guid, &Attributes, &Length, Var);
    }
    //requested variable not found
    if(EFI_ERROR(Status))
    {
        if(Var != NULL)
            gBS->FreePool(Var);

        Var = NULL;
        Length = 0;
        Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    }
    VarSize = Length;

    Status = ConfigRouting->ConfigToBlock(ConfigRouting, Configuration, Var, &Length, Progress);
    while(Status == EFI_DEVICE_ERROR || (Status == EFI_INVALID_PARAMETER && Var == NULL)) //block not large enough
    {
        if(Var != NULL)
            gBS->FreePool(Var);

        Status = gBS->AllocatePool(EfiBootServicesData, Length, (VOID**)&Var);
        if(EFI_ERROR(Status))
            return Status;

        VarSize = Length;
        Status = ConfigRouting->ConfigToBlock(ConfigRouting, Configuration, Var, &Length, Progress);
    }

    if(EFI_ERROR(Status))
        return Status;

    gRT->SetVariable(Name, &Guid, Attributes, VarSize, Var);
    gBS->FreePool(Var);

    return EFI_SUCCESS;
}
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
