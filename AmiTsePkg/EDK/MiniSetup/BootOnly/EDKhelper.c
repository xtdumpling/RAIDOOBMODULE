//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/EDKhelper.c $
//
// $Author: Madhans $
//
// $Revision: 7 $
//
// $Date: 6/17/10 2:59p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file EDKhelper.c
    This file contains code from EDK library.

**/

/*++

Copyright (c) 2004 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  xxxxxx.c

Abstract:

  xxxxxxxxxxxx

--*/

#include "minisetup.h"

#if !TSE_USE_EDK_LIBRARY

extern EFI_SYSTEM_TABLE 	*pST;
extern EFI_BOOT_SERVICES 	*pBS;
extern EFI_RUNTIME_SERVICES 	*pRS;

#if !TSE_APTIO_5_SUPPORT
EFI_SYSTEM_TABLE 	*gST=NULL;
EFI_BOOT_SERVICES 	*gBS=NULL;
EFI_RUNTIME_SERVICES 	*gRT=NULL;
EFI_GUID  gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;
EFI_GUID gEfiEventReadyToBootGuid = EFI_EVENT_GROUP_READY_TO_BOOT;
#endif

#define EFI_SHELL_FILE_GUID  \
  { 0xc57ad6b7, 0x0515, 0x40a8, 0x9d, 0x21, 0x55, 0x16, 0x52, 0x85, 0x4e, 0x37 }
EFI_GUID gEfiShellFileGuid = EFI_SHELL_FILE_GUID;

/*
#define EFI_GLOBAL_VARIABLE_GUID \
  { \
    0x8BE4DF61, 0x93CA, 0x11d2, 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C \
  }
*/

#define EFI_PRINT_PROTOCOL_GUID  \
   { 0xdf2d868e, 0x32fc, 0x4cf0, 0x8e, 0x6b, 0xff, 0xd9, 0x5d, 0x13, 0x43, 0xd0 }

EFI_GUID  gEfiPrintProtocolGuid = EFI_PRINT_PROTOCOL_GUID;

#ifndef EFI_OEM_BADGING_PROTOCOL_GUID
#define EFI_OEM_BADGING_PROTOCOL_GUID \
  { 0x170e13c0, 0xbf1b, 0x4218, 0x87, 0x1d, 0x2a, 0xbd, 0xc6, 0xf8, 0x87, 0xbc }
#endif
//EFI_GUID  gEfiOEMBadgingProtocolGuid = EFI_OEM_BADGING_PROTOCOL_GUID;
extern EFI_GUID  gEfiOEMBadgingProtocolGuid;

/*
#define EFI_EVENT_GROUP_READY_TO_BOOT \
  { 0x7ce88fb3, 0x4bd7, 0x4679, { 0x87, 0xa8, 0xa8, 0xd8, 0xde, 0xe5, 0x0d, 0x2b } }
*/
VOID InitAmiLib(
	IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable
);

/**
    To avoid including EDK libs.

    @param ImageHandle and Systemtable

    @retval Status

**/
EFI_STATUS
EfiInitializeDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
	InitAmiLib(ImageHandle,SystemTable);
	gST = pST;
	gBS = pBS;
	gRT = pRS;
	return EFI_SUCCESS;
}

/**
    To avoid including EDK libs.

    @param size

    @retval pointer

**/
void* MallocZ(UINTN Size);
VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  )
{
	return MallocZ(AllocationSize);
}

/**
    To avoid including EDK libs.

    @param size

    @retval pointer

**/
void* Malloc(UINTN Size);
VOID *
EfiLibAllocatePool (
  IN  UINTN   AllocationSize
  )
{
	return Malloc(AllocationSize);
}

/**
    To avoid including EDK libs.

    @param Guid1 IN EFI_GUID *Guid2

    @retval BOOLEAN

**/
BOOLEAN
EfiCompareGuid (
  IN EFI_GUID *Guid1,
  IN EFI_GUID *Guid2
  )
{
	return (BOOLEAN)(0==MemCmp((UINT8*)Guid1,(UINT8*)Guid2,sizeof(EFI_GUID)));
}

/**
    To avoid including EDK libs.

    @param MemOne IN VOID *MemTwo,

    @retval INTN

**/
INTN
EfiCompareMem (
  IN VOID     *MemOne,
  IN VOID     *MemTwo,
  IN UINTN    Length
  )
{
	return MemCmp(MemOne,MemTwo,Length);
}

/**
    To avoid including EDK libs.

    @param string 

    @retval UINTN

**/
UINTN Wcslen(CHAR16 *string);
UINTN
EfiStrLen (
  IN CHAR16   *String
  )
{
	return Wcslen(String);
}

/**
    To avoid including EDK libs.

    @param string1 CHAR16* string2

    @retval CHAR16*

**/
CHAR16* Wcscpy(CHAR16 *string1, CHAR16* string2);
VOID
EfiStrCpy (
  IN CHAR16   *Destination,
  IN CHAR16   *Source
  )
{
	Wcscpy(Destination,Source);
}

/**
    To avoid including EDK libs.

    @param string1 CHAR16 *string2

    @retval int

**/
int Wcscmp( CHAR16 *string1, CHAR16 *string2 );
INTN
EfiStrCmp (
  IN CHAR16   *String,
  IN CHAR16   *String2
  )
{
	return Wcscmp( String, String2 );
} 

int Strcmp( char *string1, char *string2 );
INTN
EfiAsciiStrCmp (
  IN CHAR8   *String,
  IN CHAR8   *String2
  )
{
	return Strcmp( String, String2 );
}

/**
    To avoid including EDK libs.

    @param pDp1 EFI_DEVICE_PATH_PROTOCOL *pDp2

    @retval VOID*

**/
VOID* DPAdd(EFI_DEVICE_PATH_PROTOCOL *pDp1, EFI_DEVICE_PATH_PROTOCOL *pDp2);

EFI_DEVICE_PATH_PROTOCOL *
EfiAppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  )
{
	return (EFI_DEVICE_PATH_PROTOCOL *)DPAdd(Src1,Src2);
}

/**
    To avoid including EDK libs.

    @param pDp1 EFI_DEVICE_PATH_PROTOCOL *pDp2

    @retval VOID*

**/
VOID* DPAddNode(EFI_DEVICE_PATH_PROTOCOL *pDp1, EFI_DEVICE_PATH_PROTOCOL *pDp2);

EFI_DEVICE_PATH_PROTOCOL        *
EfiAppendDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  )
{
	return (EFI_DEVICE_PATH_PROTOCOL *)DPAddNode(Src1, Src2);
}

/**
    To avoid including EDK libs.

    @param Handle 

    @retval EFI_DEVICE_PATH_PROTOCOL *

**/
EFI_DEVICE_PATH_PROTOCOL *
EfiDevicePathFromHandle (
  IN EFI_HANDLE  Handle
  )
/*++

Routine Description:

  Get the device path protocol interface installed on a specified handle.

Arguments:

  Handle  - a specified handle

Returns:

  The device path protocol interface installed on that handle.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  DevicePath = NULL;
  gBS->HandleProtocol (
        Handle,
        &gEfiDevicePathProtocolGuid,
        (VOID *) &DevicePath
        );
  return DevicePath;
}

VOID
EfiStrCat (
  IN CHAR16   *Destination,
  IN CHAR16   *Source
  )
/*++

Routine Description:
  Concatinate Source on the end of Destination

Arguments:
  Destination - String to added to the end of.
  Source      - String to concatinate.

Returns:
  NONE

--*/
{   
  EfiStrCpy (Destination + EfiStrLen (Destination), Source);
}

INTN
EfiStrnCmp (
  IN CHAR16   *String,
  IN CHAR16   *String2,
  IN UINTN    Length
  )
/*++

Routine Description:
  This function compares the Unicode string String to the Unicode
  string String2 for len characters.  If the first len characters
  of String is identical to the first len characters of String2,
  then 0 is returned.  If substring of String sorts lexicographically
  after String2, the function returns a number greater than 0. If
  substring of String sorts lexicographically before String2, the
  function returns a number less than 0.

Arguments:
  String  - Compare to String2
  String2 - Compare to String
  Length  - Number of Unicode characters to compare

Returns:
  0     - The substring of String and String2 is identical.
  > 0   - The substring of String sorts lexicographically after String2
  < 0   - The substring of String sorts lexicographically before String2

--*/
{
  while (*String && Length != 0) {
    if (*String != *String2) {
      break;
    }
    String  += 1;
    String2 += 1;
    Length  -= 1;
  }
  return Length > 0 ? *String - *String2 : 0;
}


UINTN
EfiStrSize (
  IN CHAR16   *String
  )
/*++

Routine Description:
  Return the number bytes in the Unicode String. This is not the same as
  the length of the string in characters. The string size includes the NULL

Arguments:
  String - String to process

Returns:
  Number of bytes in String

--*/
{
  return ((EfiStrLen (String) + 1) * sizeof (CHAR16));
}


#define AMI_SIZE_OF_FILEPATH_DEVICE_PATH STRUCT_OFFSET(FILEPATH_DEVICE_PATH,PathName)

/**
    To avoid including EDK libs.

    @param Device OPTIONAL, IN CHAR16 *FileName

    @retval EFI_DEVICE_PATH_PROTOCOL *

**/
EFI_DEVICE_PATH_PROTOCOL *
EfiFileDevicePath (
  IN EFI_HANDLE               Device  OPTIONAL,
  IN CHAR16                   *FileName
  )
/*++

Routine Description:

  This function allocates a device path for a file and appends it to an existiong
  device path.

Arguments:
  Device     - A pointer to a device handle.

  FileName   - A pointer to a Null-terminated Unicodestring.

Returns:
  A device path contain the file name.

--*/
{
  UINTN                     Size;
  FILEPATH_DEVICE_PATH      *FilePath;
  EFI_DEVICE_PATH_PROTOCOL  *Eop;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  for (Size = 0; FileName[Size] != 0; Size++)
    ;
  Size        = (Size + 1) * 2;

  FilePath    = EfiLibAllocateZeroPool (Size + AMI_SIZE_OF_FILEPATH_DEVICE_PATH + sizeof (EFI_DEVICE_PATH_PROTOCOL));

  DevicePath  = NULL;

  if (FilePath != NULL) {

    //
    // Build a file path
    //
    FilePath->Header.Type     = MEDIA_DEVICE_PATH;
    FilePath->Header.SubType  = MEDIA_FILEPATH_DP;
    SetDevicePathNodeLength (&FilePath->Header, Size + AMI_SIZE_OF_FILEPATH_DEVICE_PATH);
    EfiCopyMem (FilePath->PathName, FileName, Size);
    Eop = NextDevicePathNode (&FilePath->Header);
    SetDevicePathEndNode (Eop);

    //
    // Append file path to device's device path
    //

    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) FilePath;
    if (Device != NULL) {
      DevicePath = EfiAppendDevicePath (
                    EfiDevicePathFromHandle (Device),
                    DevicePath
                    );

      gBS->FreePool (FilePath);
    }
  }

  return DevicePath;
}

/**
    To avoid including EDK libs.

    @param FvDevicePathNode 
    @param NameGuid 

    @retval VOID EFIAPI

**/
VOID
EFIAPI
TseEfiInitializeFwVolDevicepathNode (
  IN  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH     *FvDevicePathNode,
  IN EFI_GUID                               *NameGuid
  )
/*++

Routine Description:

  Initialize a Firmware Volume (FV) Media Device Path node.
  
  Tiano extended the EFI 1.10 device path nodes. Tiano does not own this enum
  so as we move to UEFI 2.0 support we must use a mechanism that conforms with
  the UEFI 2.0 specification to define the FV device path. An UEFI GUIDed 
  device path is defined for PIWG extensions of device path. If the code 
  is compiled to conform with the UEFI 2.0 specification use the new device path
  else use the old form for backwards compatability.

Arguments:

  FvDevicePathNode  - Pointer to a FV device path node to initialize
  NameGuid          - FV file name to use in FvDevicePathNode

Returns:

  None

--*/
{
//*** AMI PORTING BEGIN ***//
//NEW PIWG Specific Device Path defined here is not in compliance with PI DXE CIS 1.0
//Let's disable it
//See also corresponding change in TianoSpecDevicePath.h
#if 1
//*** AMI PORTING END *****//
  //
  // Use old Device Path that conflicts with UEFI
  //
  FvDevicePathNode->Header.Type     = MEDIA_DEVICE_PATH;
  FvDevicePathNode->Header.SubType  = MEDIA_FV_FILEPATH_DP;
  SetDevicePathNodeLength (&FvDevicePathNode->Header, sizeof (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH));
  
#else
  //
  // Use the new Device path that does not conflict with the UEFI
  //
  FvDevicePathNode->Piwg.Header.Type     = MEDIA_DEVICE_PATH;
  FvDevicePathNode->Piwg.Header.SubType  = MEDIA_VENDOR_DP;
  SetDevicePathNodeLength (&FvDevicePathNode->Piwg.Header, sizeof (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH));

  //
  // Add the GUID for generic PIWG device paths
  //
  EfiCopyMem (&FvDevicePathNode->Piwg.PiwgSpecificDevicePath, &gEfiFrameworkDevicePathGuid, sizeof(EFI_GUID));

  //
  // Add in the FW Vol File Path PIWG defined inforation
  //
  FvDevicePathNode->Piwg.Type = PIWG_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH_TYPE;

#endif
  EfiCopyMem (&((AMITSE_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)FvDevicePathNode)->FvFileName, NameGuid, sizeof(EFI_GUID));
}

/**
    To avoid including EDK libs.

    @param Name 

    @retval STATUS

**/
EFI_STATUS
EfiLibNamedEventSignal (
  IN EFI_GUID            *Name
  )
/*++

Routine Description:
  Signals a named event. All registered listeners will run.
  The listeners should register using EfiLibNamedEventListen() function.

  NOTE: For now, the named listening/signalling is implemented
  on a protocol interface being installed and uninstalled.
  In the future, this maybe implemented based on a dedicated mechanism.

Arguments:
  Name - Name to perform the signaling on. The name is a GUID.

Returns:
  EFI_SUCCESS if successfull.

--*/
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  Name,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->UninstallProtocolInterface (
                  Handle,
                  Name,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
    To avoid including EDK libs.

    @param Type IN EFI_STATUS_CODE_VALUE Value,
    @param Instance IN EFI_GUID *CallerId OPTIONAL,
    @param Data OPTIONAL

    @retval STATUS

**/
EFI_STATUS LibReportStatusCode(
	IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
	IN UINT32 Instance, IN EFI_GUID *CallerId OPTIONAL,
	IN EFI_STATUS_CODE_DATA *Data OPTIONAL
);

EFI_STATUS
EfiLibReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     Type,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA     *Data     OPTIONAL  
  )
{
	return LibReportStatusCode(Type,Value,Instance,CallerId,Data);
}


VOID
EFIAPI
EventNotifySignalAllNullEvent (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  //
  // This null event is a size efficent way to enusre that 
  // EFI_EVENT_NOTIFY_SIGNAL_ALL is error checked correctly.
  // EFI_EVENT_NOTIFY_SIGNAL_ALL is now mapped into 
  // CreateEventEx() and this function is used to make the
  // old error checking in CreateEvent() for Tiano extensions
  // function.
  //
  return;
}

/**
    To avoid including EDK libs.

    @param NotifyTpl 
    @param NotifyFunction 
    @param NotifyContext 
    @param ReadyToBootEvent 

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
TseEfiCreateEventReadyToBoot (
  IN EFI_TPL                      NotifyTpl,
  IN EFI_EVENT_NOTIFY             NotifyFunction,
  IN VOID                         *NotifyContext,
  OUT EFI_EVENT                   *ReadyToBootEvent
  )
/*++

Routine Description:
  Create a Read to Boot Event.  
  
  Tiano extended the CreateEvent Type enum to add a ready to boot event type. 
  This was bad as Tiano did not own the enum. In UEFI 2.0 CreateEventEx was
  added and now it's possible to not voilate the UEFI specification and use 
  the ready to boot event class defined in UEFI 2.0. This library supports
  the R8.5/EFI 1.10 form and R8.6/UEFI 2.0 form and allows common code to 
  work both ways.

Arguments:
  @param LegacyBootEvent  Returns the EFI event returned from gBS->CreateEvent(Ex)

Return:
  EFI_SUCCESS   - Event was created.
  Other         - Event was not created.

--*/
{
  EFI_STATUS        Status;
  UINT32            EventType;
  EFI_EVENT_NOTIFY  WorkerNotifyFunction;

#if (EFI_SPECIFICATION_VERSION < 0x00020000)

  if (NotifyFunction == NULL) {
    EventType = EFI_EVENT_SIGNAL_READY_TO_BOOT | EFI_EVENT_NOTIFY_SIGNAL_ALL;
  } else {
    EventType = EFI_EVENT_SIGNAL_READY_TO_BOOT;
  }
  WorkerNotifyFunction = NotifyFunction;

  //
  // prior to UEFI 2.0 use Tiano extension to EFI
  //
  Status = gBS->CreateEvent (
                  EventType,
                  NotifyTpl,
                  WorkerNotifyFunction,
                  NotifyContext,
                  ReadyToBootEvent
                  );
#else

  EventType = EFI_EVENT_NOTIFY_SIGNAL;
  if (NotifyFunction == NULL) {
    //
    // CreatEventEx will check NotifyFunction is NULL or not
    //
    WorkerNotifyFunction = EventNotifySignalAllNullEvent;
  } else {
    WorkerNotifyFunction = NotifyFunction;
  }

  //
  // For UEFI 2.0 and the future use an Event Group
  //
  Status = gBS->CreateEventEx (
                  EventType,
                  NotifyTpl,
                  WorkerNotifyFunction,
                  NotifyContext,
                  &gEfiEventReadyToBootGuid,
                  ReadyToBootEvent
                  );
#endif
  return Status;
}


/**
    To avoid including EDK libs.

    @param 

    @retval 

**/
BOOLEAN
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  )
/*++

  Routine Description:
    Determines if a Unicode character is a hexadecimal digit.
    The test is case insensitive.

  Arguments:
    Digit - Pointer to byte that receives the value of the hex character.
    Char  - Unicode character to test.

  Returns:
    TRUE  - If the character is a hexadecimal digit.
    FALSE - Otherwise.

--*/
{
  if ((Char >= L'0') && (Char <= L'9')) {
    *Digit = (UINT8) (Char - L'0');
    return TRUE;
  }

  if ((Char >= L'A') && (Char <= L'F')) {
    *Digit = (UINT8) (Char - L'A' + 0x0A);
    return TRUE;
  }

  if ((Char >= L'a') && (Char <= L'f')) {
    *Digit = (UINT8) (Char - L'a' + 0x0A);
    return TRUE;
  }

  return FALSE;
}

/**
    To avoid including EDK libs.

    @param 

    @retval 

**/
EFI_STATUS
HexStringToBuf (
  IN OUT UINT8                     *Buf,   
  IN OUT UINTN                    *Len,
  IN     CHAR16                    *Str,
  OUT    UINTN                     *ConvertedStrLen  OPTIONAL
  )
/*++

  Routine Description:
    Converts Unicode string to binary buffer.
    The conversion may be partial.
    The first character in the string that is not hex digit stops the conversion.
    At a minimum, any blob of data could be represented as a hex string.

  Arguments:
    Buf    - Pointer to buffer that receives the data.
    Len    - Length in bytes of the buffer to hold converted data.
                If routine return with EFI_SUCCESS, containing length of converted data.
                If routine return with EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
    Str    - String to be converted from.
    ConvertedStrLen - Length of the Hex String consumed.

  Returns:
    EFI_SUCCESS: Routine Success.
    EFI_BUFFER_TOO_SMALL: The buffer is too small to hold converted data.
    EFI_

--*/
{
  UINTN       HexCnt;
  UINTN       Idx;
  UINTN       BufferLength;
  UINT8       Digit;
  UINT8       Byte;

  //
  // Find out how many hex characters the string has.
  //
  for (Idx = 0, HexCnt = 0; IsHexDigit (&Digit, Str[Idx]); Idx++, HexCnt++);

  if (HexCnt == 0) {
    *Len = 0;
    return EFI_SUCCESS;
  }
  //
  // Two Unicode characters make up 1 buffer byte. Round up.
  //
  BufferLength = (HexCnt + 1) / 2; 

  //
  // Test if  buffer is passed enough.
  //
  if (BufferLength > (*Len)) {
    *Len = BufferLength;
    return EFI_BUFFER_TOO_SMALL;
  }

  *Len = BufferLength;

  for (Idx = 0; Idx < HexCnt; Idx++) {

    IsHexDigit (&Digit, Str[HexCnt - 1 - Idx]);

    //
    // For odd charaters, write the lower nibble for each buffer byte,
    // and for even characters, the upper nibble.
    //
    if ((Idx & 1) == 0) {
      Byte = Digit;
    } else {
      Byte = Buf[Idx / 2];
      Byte &= 0x0F;
      Byte |= Digit << 4;
    }

    Buf[Idx / 2] = Byte;
  }

  if (ConvertedStrLen != NULL) {
    *ConvertedStrLen = HexCnt;
  }

  return EFI_SUCCESS;
}

/**
    Converts name value string to buffer

        : 	Buf    - Pointer to buffer that receives the data.
    @param Len Length in bytes of the buffer to hold converted data.
        If routine return with EFI_SUCCESS, containing length of converted data.
        If routine return with EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
    @param Str String to be converted from.
    @param ConvertedStrLen Length of the Hex String consumed.

    @retval EFI_SUCCESS: Routine Success.
			EFI_BUFFER_TOO_SMALL: The buffer is too small to hold converted data.

**/
UINT8 HexToNibbleChar (IN UINT8 Nibble);
EFI_STATUS NameValueHexStringToBuf (
  IN OUT UINT8                     *Buf,   
  IN OUT UINTN                    *Len,
  IN     CHAR16                    *Str,
  OUT    UINTN                     *ConvertedStrLen  OPTIONAL
  )
{
  UINTN       HexCnt;
  UINTN       Idx, ByteIndex = 0;
  UINTN       BufferLength;
  UINT8       Digit;
  UINT8       Byte;
  UINT64	  Ext8byte = 0;
  UINT32	  Ext4byte = 0;
  UINT16      Word;

  //
  // Find out how many hex characters the string has.
  //
  for (Idx = 0, HexCnt = 0; IsHexDigit (&Digit, Str[Idx]); Idx++, HexCnt++);

  if (HexCnt == 0) {
    *Len = 0;
    return EFI_SUCCESS;
  }
  //
  // Two Unicode characters make up 1 buffer byte. Round up.
  //
  BufferLength = (HexCnt + 1) / 2; 

  //
  // Test if  buffer is passed enough.
  //
  if (BufferLength > (*Len)) {
    *Len = BufferLength;
    return EFI_BUFFER_TOO_SMALL;
  }

  *Len = BufferLength;

  for (Idx = 0; Idx < HexCnt; Idx += 4)
  {
	  //00 31 00 36 00 30 00 00
	  Ext4byte = *((UINT32 *)&Str[Idx+2]);			//00 31 00 36

	  Word = ((UINT16)(Ext4byte & 0x0000FFFF));		//00 36
	  Byte = ((UINT8)(Word & 0x00FF));				//36
	  Buf [ByteIndex] = HexToNibbleChar (Byte);				//6
	  Buf [ByteIndex] <<= 0x4;						//60
	  
	  Word = ((UINT16)(Ext4byte >> 16));			//00 31
	  Byte = ((UINT8)(Word & 0x00FF));				//31
	  Buf [ByteIndex] |= HexToNibbleChar (Byte);				//1
	  
	  ByteIndex ++;

	  Ext4byte = *((UINT32 *)&Str[Idx]);	//00 30 00 30
	  Word = ((UINT16)(Ext4byte & 0x0000FFFF));	//00 30
	  Byte = ((UINT8)(Word & 0x00FF));				//30
	  Buf [ByteIndex] = HexToNibbleChar (Byte);			//0
	  Buf [ByteIndex] <<= 0x4;						//60

	  Word = ((UINT16)(Ext4byte >> 16));	//00 30
	  Byte = ((UINT8)(Word & 0x00FF));			//30
	  Buf [ByteIndex] |= HexToNibbleChar (Byte);			//0
	  
	  ByteIndex ++;
  }

  if (ConvertedStrLen != NULL) {
    *ConvertedStrLen = HexCnt;
  }

  return EFI_SUCCESS;
}

/**
    To avoid including EDK libs.

    @param 

    @retval 

**/
UINT64 Mul64(
    IN UINT64   Value64,
    IN UINTN	Value32
    );
#if !TSE_APTIO_5_SUPPORT
UINT64
MultU64x32 (
  IN  UINT64  Multiplicand,
  IN  UINTN   Multiplier
  )
{
	return Mul64(Multiplicand,Multiplier);
}
#endif
UINTN DPLength(EFI_DEVICE_PATH_PROTOCOL *pDp);

UINTN
EfiDevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
	return DPLength(DevicePath);
}

/**
    Converts the low nibble of a byte  to hex unicode character.

    @param Nibble lower nibble of a byte.

    @retval Hex unicode character.

**/
CHAR16
NibbleToHexChar (
  IN UINT8      Nibble
  )
{
  Nibble &= 0x0F;
  if (Nibble <= 0x9) {
    return (CHAR16)(Nibble + L'0');
  }

  return (CHAR16)(Nibble - 0xA + L'A');
}


/**
    Converts the hex value to character.

    @param Nibble lower nibble of a byte.

    @retval Hex unicode character.

**/
UINT8
HexToNibbleChar (
  IN UINT8      Nibble
  )
{
  if (Nibble - L'0' <= 0x9) {
    return (CHAR16)(Nibble - L'0');
  }

  return (CHAR16)(Nibble + 0xA - L'A');
}

/**
    Converts binary buffer to Unicode string.
    At a minimum, any blob of data could be represented as a hex string.

    @param Str Pointer to the string.
    @param HexStringBufferLength Length in bytes of buffer to hold the hex string. Includes tailing '\0' character.
        If routine return with EFI_SUCCESS, containing length of hex string buffer.
        If routine return with EFI_BUFFER_TOO_SMALL, containg length of hex string buffer desired.
    @param Buf Buffer to be converted from.
    @param Len Length in bytes of the buffer to be converted.

    @retval EFI_SUCCESS: Routine success.
				    EFI_BUFFER_TOO_SMALL: The hex string buffer is too small.

**/
EFI_STATUS
BufToHexString (
  IN OUT CHAR16                    *Str,
  IN OUT UINTN                     *HexStringBufferLength,
  IN     UINT8                     *Buf,
  IN     UINTN                      Len
  )
{
  UINTN       Idx;
  UINT8       Byte;
  UINTN       StrLen;

  //
  // Make sure string is either passed or allocate enough.
  // It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
  // Plus the Unicode termination character.
  //
  StrLen = Len * 2;
  if (StrLen > ((*HexStringBufferLength) - 1)) {
    *HexStringBufferLength = StrLen + 1;
    return EFI_BUFFER_TOO_SMALL;
  }

  *HexStringBufferLength = StrLen + 1;
  //
  // Ends the string.
  //
  Str[StrLen] = L'\0'; 

  for (Idx = 0; Idx < Len; Idx++) {

    Byte = Buf[Idx];
    Str[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
    Str[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
  }

  return EFI_SUCCESS;
}


/**
    Converts binary buffer to Unicode string.
    At a minimum, any blob of data could be represented as a hex string.
    Used only for name value implementaion

    @param Str Pointer to the string.
    @param HexStringBufferLength Length in bytes of buffer to hold the hex string. Includes tailing '\0' character.
        If routine return with EFI_SUCCESS, containing length of hex string buffer.
        If routine return with EFI_BUFFER_TOO_SMALL, containg length of hex string buffer desired.
    @param Buf Buffer to be converted from.
    @param Len Length in bytes of the buffer to be converted.

    @retval EFI_SUCCESS: Routine success.
				    EFI_BUFFER_TOO_SMALL: The hex string buffer is too small.

**/
EFI_STATUS
NameValueBufToHexString (
  IN OUT CHAR16          **Str,
  IN OUT UINTN             *HexStringBufferLength,
  IN     UINT8                     *Buf,
  IN     UINTN                    Len
  )
{
  UINT8       Byte;

  //
  // Make sure string is either passed or allocate enough.
  // It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
  // Plus the Unicode termination character.
  //

  if (Len <= sizeof (UINT64))			//For upto 8 bytes we have to take care of endian allignment
  {
	  UINTN       Idx;
	  UINTN       StrLen;
	  CHAR16 	*tempStr = (CHAR16 *)NULL;

	  //
	  // Make sure string is either passed or allocate enough.
	  // It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
	  // Plus the Unicode termination character.
	  //
	  StrLen = Len * 2;
	  *Str = EfiLibAllocateZeroPool ((StrLen + 1) * sizeof (UINT16) );
	  if (NULL == *Str)
	  {
		  return EFI_OUT_OF_RESOURCES;
	  }
	  *HexStringBufferLength = (StrLen + 1) * sizeof (UINT16);			//UefiVarSetNvram needs full size
	  //
	  // Ends the string.
	  //
	  tempStr = *Str;
	  tempStr[StrLen] = L'\0'; 

	  for (Idx = 0; Idx < Len; Idx++) 
	  {
	    Byte = Buf[Idx];
	    tempStr[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
	    tempStr[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
	  }
  }
  else
  {
	  CHAR16 *tempBuf = (CHAR16 *)NULL, *tempStr = (CHAR16 *)NULL;
	  UINT32 iIndex = 0, StringHexLength = 0;
	  CHAR16 Word;

	  tempBuf = EfiLibAllocateZeroPool (Len + sizeof (CHAR16));		//If full string is given then NULL wont be there so crashing so added one NULL
	  if (NULL == tempBuf)											//character at end	
	  {
		  return EFI_OUT_OF_RESOURCES;
	  }
	  MemCpy (tempBuf, Buf, Len);
	  //Finding length to allocate
	  while (tempBuf [iIndex])
	  {
		  StringHexLength ++;
		  iIndex ++;
	  }
	  iIndex = 0;
	 *Str = EfiLibAllocateZeroPool ((StringHexLength  *  sizeof (CHAR16) * 4) + sizeof (CHAR16));
	 if (NULL == *Str )
  	 {
		 MemFreePointer ((VOID **)&tempBuf);
		 return EFI_OUT_OF_RESOURCES;
	 }
	 tempStr = *Str;
	  while (tempBuf [iIndex])
	  {
		  Word = tempBuf [iIndex];
		  Byte =  ((UINT8)(Word >> 8));
		  tempStr [iIndex * 4] = NibbleToHexChar ((UINT8)(Byte >> 4));
		  tempStr [iIndex * 4 + 1] = NibbleToHexChar (Byte);
		  Byte = Word & 0x00FF;
		  tempStr [iIndex * 4 + 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
		  tempStr [iIndex * 4 + 3] = NibbleToHexChar (Byte);
		  iIndex ++;
	  }
	  *HexStringBufferLength =  (StringHexLength  *  sizeof (CHAR16) * 4) + sizeof (CHAR16);
	  tempStr [StringHexLength * 4] = L'\0';
	  MemFreePointer ((VOID **)&tempBuf);
  }
  return EFI_SUCCESS;
}

/**
    Converts the unicode character of the string from uppercase to lowercase.

    @param Str String to be converted

    @retval VOID

**/
VOID
ToLower (
  IN OUT CHAR16    *Str
  )
{
  CHAR16      *Ptr;
  
  for (Ptr = Str; *Ptr != L'\0'; Ptr++) {
    if (*Ptr >= L'A' && *Ptr <= L'Z') {
      *Ptr = (CHAR16) (*Ptr - L'A' + L'a');
    }
  }
}

/**
    Swap bytes in the buffer.

    @param Buffer Binary buffer.
    @param BufferSize Size of the buffer in bytes.

    @retval VOID

**/
VOID
SwapBuffer (
  IN OUT UINT8     *Buffer,
  IN UINTN         BufferSize
  )
{
  UINTN  Index;
  UINT8  Temp;
  UINTN  SwapCount;

  SwapCount = BufferSize / 2;
  for (Index = 0; Index < SwapCount; Index++) {
    Temp = Buffer[Index];
    Buffer[Index] = Buffer[BufferSize - 1 - Index];
    Buffer[BufferSize - 1 - Index] = Temp;
  }
}

/**
    Convert Unicode string to binary representation Config string, e.g.
    "ABCD" => "0041004200430044". Config string appears in <ConfigHdr> (i.e.
    "&NAME=<string>"), or Name/Value pair in <ConfigBody> (i.e. "label=<string>").

    @param ConfigString Binary representation of Unicode String, <string> := (<HexCh>4)+
    @param StrBufferLen On input: Length in bytes of buffer to hold the Unicode string.
        Includes tailing '\0' character.
        On output:
        If return EFI_SUCCESS, containing length of Unicode string buffer.
        If return EFI_BUFFER_TOO_SMALL, containg length of string buffer desired.
    @param UnicodeString Original Unicode string.

    @retval EFI_SUCCESS Routine success.
    @retval EFI_BUFFER_TOO_SMALL The string buffer is too small.

**/
EFI_STATUS
UnicodeToConfigString (
  IN OUT CHAR16                *ConfigString,
  IN OUT UINTN                 *StrBufferLen,
  IN CHAR16                    *UnicodeString
  )
{
  UINTN       Index;
  UINTN       Len;
  UINTN       BufferSize;
  CHAR16      *String;

  Len = EfiStrLen (UnicodeString);
  BufferSize = (Len * 4 + 1) * sizeof (CHAR16);

  if (*StrBufferLen < BufferSize) {
    *StrBufferLen = BufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StrBufferLen = BufferSize;
  String        = ConfigString;

  for (Index = 0; Index < Len; Index++) {
    BufToHexString (ConfigString, &BufferSize, (UINT8 *) UnicodeString, 2);

    ConfigString += 4;
    UnicodeString += 1;
  }

  //
  // Add tailing '\0' character
  //
  *ConfigString = L'\0';

  //
  // Convert the uppercase to lowercase since <HexAf> is defined in lowercase format.
  //
  ToLower (String);  
  return EFI_SUCCESS;
}

/**
    Allocate BootServicesData pool and use a buffer provided by 
    caller to fill it.

    @param AllocationSize The size to allocate
    @param Buffer Buffer that will be filled into the buffer allocated

    @retval Pointer of the buffer allocated.

**/
VOID *
EfiLibAllocateCopyPool (
  IN  UINTN   AllocationSize,
  IN  VOID    *Buffer
  )
{
  VOID  *Memory;

  Memory = NULL;
  gBS->AllocatePool (EfiBootServicesData, AllocationSize, &Memory);
  if (Memory != NULL) {
    gBS->CopyMem (Memory, Buffer, AllocationSize);
  }

  return Memory;
}

/**
    Converts binary buffer to Unicode string in reversed byte order from BufToHexString().

    @param Str String for output
    @param Buffer Binary buffer.
    @param BufferSize Size of the buffer in bytes.

    @retval EFI_SUCCESS The function completed successfully.

**/
EFI_STATUS
BufferToHexString (
  IN OUT CHAR16    *Str,
  IN UINT8         *Buffer,
  IN UINTN         BufferSize
  )
{
  EFI_STATUS  Status;
  UINT8       *NewBuffer;
  UINTN       StrBufferLen;

  NewBuffer = EfiLibAllocateCopyPool (BufferSize, Buffer);
  SwapBuffer (NewBuffer, BufferSize);

  StrBufferLen = BufferSize * 2 + 1;
  Status = BufToHexString (Str, &StrBufferLen, NewBuffer, BufferSize);

  gBS->FreePool (NewBuffer);
  //
  // Convert the uppercase to lowercase since <HexAf> is defined in lowercase format.
  //
  ToLower (Str);
  
  return Status;
}

/**
    Find a substring.

    @param String Null-terminated string to search.
    @param StrCharSet Null-terminated string to search for.

    @retval The address of the first occurrence of the matching substring if successful, 
        or NULL otherwise.

**/
CHAR16*
 EfiStrStr (
   IN  CHAR16  *String,
   IN  CHAR16  *StrCharSet
   )
 {
   CHAR16 *Src;
   CHAR16 *Sub;
   
   Src = String;
   Sub = StrCharSet;
   
   while ((*String != L'\0') && (*StrCharSet != L'\0')) {
     if (*String++ != *StrCharSet++) {
       String = ++Src;
       StrCharSet = Sub;
     }
   }
   if (*StrCharSet == L'\0') {
     return Src;
   } else {
     return NULL;
   }
 }

UINT64 Shr64(
	IN UINT64 Value,
	IN UINT8 Shift
	);

UINT64 Shl64(
	IN UINT64 Value,
	IN UINT8 Shift
	);
#if !TSE_APTIO_5_SUPPORT
UINT64
LShiftU64 (
  IN UINT64   Operand,
  IN UINTN    Count
  )
{
 UINT8 Shift;

  while(Count)
  {
	Shift = (UINT8)((Count>256)?256:Count);
  	Operand = Shl64(Operand,Shift);
	Count -= Shift;
  }

  return Operand;
}

UINT64
RShiftU64 (
  IN UINT64   Operand,
  IN UINTN    Count
  )
{
 UINT8 Shift;

  while(Count)
  {
	Shift = (UINT8)((Count>256)?256:Count);
  	Operand = Shr64(Operand,Shift);
	Count -= Shift;
  }

  return Operand;
}
#endif
UINT64 Div64 (
	IN UINT64	Dividend,
	IN UINTN	Divisor,	//Can only be 31 bits.
  	OUT UINTN	*Remainder OPTIONAL
  	);

UINT64
AmiTseDivU64x32 (
  IN  UINT64  Dividend,
  IN  UINTN   Divisor,
  OUT UINTN   *Remainder OPTIONAL
  )
{
	return Div64(Dividend,Divisor,Remainder);
}

VOID
EfiDebugAssert (
    IN CHAR8    *FileName,
    IN INTN     LineNumber,
    IN CHAR8    *Description
    )
{
}

VOID
EfiDebugPrint (
IN  UINTN ErrorLevel,
IN  CHAR8 *Format,
...
)
{
}

#define EFI_FORM_BROWSER_PROTOCOL_GUID \
  { \
    0xe5a1333e, 0xe1b4, 0x4d55, 0xce, 0xeb, 0x35, 0xc3, 0xef, 0x13, 0x34, 0x43 \
  }
EFI_GUID  gEfiFormBrowserProtocolGuid = EFI_FORM_BROWSER_PROTOCOL_GUID;

#else //!TSE_USE_EDK_LIBRARY

// To support building when TSE_USE_EDK_LIBRARY = 1 
#ifdef TSE_FOR_APTIO_4_50
const UINTN FlashEmpty = (UINTN)(-FLASH_ERASE_POLARITY);
const UINT32 FlashEmptyNext = (FLASH_ERASE_POLARITY ? 0xffffff : 0);

VOID MemCpy(VOID* pDestination, VOID* pSource, UINTN Count)
{
	gBS->CopyMem (pDestination, pSource, Count);
}
#endif

#endif //#if !TSE_USE_EDK_LIBRARY

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
