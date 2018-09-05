//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/print.c $
//
// $Author: Blaines $
//
// $Revision: 6 $
//
// $Date: 11/14/11 2:58p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file print.c
    file contains code to support the print functionalities

**/

/*++
  VSPrint, Print, SPrint format specification has the follwoing form

  %[flags][width]type

  flags:
    '-' - Left justify
    '+' - Prefix a sign
    ' ' - Prefix a blank
    ',' - Place commas in numberss
    '0' - Prefix for width with zeros
    'l' - UINT64
    'L' - UINT64

  width:
    '*' - Get width from a UINTN argumnet from the argument list
    Decimal number that represents width of print

  type:
    'X' - argument is a UINTN hex number, prefix '0'
    'x' - argument is a hex number
    'd' - argument is a decimal number
    'a' - argument is an ascii string 
    'S','s' - argument is an Unicode string
    'g' - argument is a pointer to an EFI_GUID
    't' - argument is a pointer to an EFI_TIME structure
    'c' - argument is an ascii character
    'r' - argument is EFI_STATUS
    '%' - Print a %

--*/
#include "minisetup.h"
#include "setupdbg.h"

#if !TSE_USE_EDK_LIBRARY

typedef struct _EFI_PRINT_PROTOCOL EFI_PRINT_PROTOCOL;

typedef UINTN (EFIAPI *EFI_VSPRINT)(
	OUT CHAR16 *StartOfBuffer, IN UINTN BufferSize,
	IN CONST CHAR16 *FormatString, IN va_list Marker
);
extern EFI_GUID gEfiPrintProtocolGuid;
struct _EFI_PRINT_PROTOCOL{
	EFI_VSPRINT VSPrint;
};

#else

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "TianoCommon.h"
#include "EfiCommonLib.h"
#include "EfiPrintLib.h"

#endif

#if TSE_DEBUG_MESSAGES
extern BOOLEAN SetupDebugKeyHandler(UINT16 ItemIndex, UINT16 Unicode, CHAR16	*Value) ;
#endif
//typedef CHAR16  CHAR_W;
#if TSE_APTIO_5_SUPPORT
#define AMITSE_VSPrint  Swprintf_s_va_list
#else
#define AMITSE_VSPrint  VSPrint
#endif

#if !TSE_APTIO_5_SUPPORT 
/**
    VSPrint function to process format and place the results in Buffer. Since a 
    VA_LIST is used this rountine allows the nesting of Vararg routines. Thus 
    this is the main print working routine

    @param StartOfBuffer Unicode buffer to print the results of the parsing of Format into.
    @param BufferSize Maximum number of characters to put into buffer. Zero means no limit.
    @param FormatString Unicode format string see file header for more details.
    @param Marker Vararg list consumed by processing Format.

    @retval Number of characters printed.

**/
UINTN
VSPrint (
  OUT CHAR16        *StartOfBuffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *FormatString,
  IN  VA_LIST       Marker
  )
{
  EFI_STATUS          Status;
  EFI_PRINT_PROTOCOL  *PrintProtocol;

  Status = gBS->LocateProtocol (
                  &gEfiPrintProtocolGuid,
                  NULL,
                (void**)  &PrintProtocol
                  );
  if (EFI_ERROR (Status)) {
    return 0;
  } else {
    return PrintProtocol->VSPrint (
                            StartOfBuffer,
                            BufferSize,
                            FormatString,
                            Marker
                            );
  }
}


#endif
/**
    SPrint function to process format and place the results in Buffer

    @param Buffer Wide char buffer to print the results of the parsing of Format into.
    @param BufferSize Maximum number of characters to put into buffer. Zero means no limit.
    @param Format Format string see file header for more details.
        ...    - Vararg list consumed by processing Format.

    @retval Number of characters printed.

**/
UINTN
SPrint (
  OUT CHAR16        *Buffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *Format,
  ...
  )
{
  UINTN   Return;
  VA_LIST Marker;

  VA_START (Marker, Format);
//  Return = AMITSE_VSPrint(Buffer, BufferSize, Format, Marker);
  Return = AMITSE_VSPrint(Buffer, BufferSize, (CHAR16*)Format, Marker);
  VA_END (Marker);

  return Return;
}

#define MAX_DEBUG_STRING_SIZE 1000

CHAR16  tmpBuffer[MAX_DEBUG_STRING_SIZE];

/**

    @param 

    @retval 

**/
VOID SetupDebugPrint(IN CONST CHAR8  *Format, ...)
{
    //UINTN   Return = 0; Set not used
    VA_LIST Marker;
    CHAR16 *c16Format = (CHAR16 *)NULL;
    CHAR8 *trace = NULL;

    c16Format = StrDup8to16((CHAR8  *)Format);

    VA_START (Marker, Format);
//    Return = AMITSE_VSPrint(tmpBuffer, MAX_DEBUG_STRING_SIZE, (CONST CHAR16 *)c16Format, Marker);
    AMITSE_VSPrint(tmpBuffer, MAX_DEBUG_STRING_SIZE, ( CHAR16 *)c16Format, Marker);
    VA_END (Marker);

    //For TRACE (Serial Output)
#if SERIAL_DEBUG
//    TRACE(-1, Format, Marker);

  trace = StrDup16to8(tmpBuffer);
  TRACE ((-1, trace));
  MemFreePointer( (VOID **)&trace );

#else //Else of SERIAL_DEBUG

#if SCREEN_PRINT
    Print(tmpBuffer);
#else //Else of SCREEN_PRINT
    if(gST->ConOut){
        gST->ConOut->OutputString( gST->ConOut, tmpBuffer );
    }
#endif //End of SCREEN_PRINT
#endif //End of SERIAL_DEBUG
    MemFreePointer( (VOID **)&c16Format );
}

/**

    @param 

    @retval 
**/
//extern VOID HandleDebugDumpKey( VOID *app, VOID *hotkey, VOID *cookie );
VOID SetupShowDebugScreenWrapper (VOID *app, VOID *hotkey, VOID *cookie )
{

#if TSE_DEBUG_MESSAGES    
    CHAR16  HiiParseing[2]          = L"x" ;
    CHAR16  HiiFunction[2]          = L"x" ;
    CHAR16  HiiNotification[2]      = L"x"; 
    CHAR16  HiiCallback[2]          = L"x" ;
    CHAR16  PrintIfrForm[27]        = L"press any key" ;
    CHAR16  PrintVariableCache[27]  = L"press any key" ; 

    TEXT_INPUT_TEMPLATE SetupDebugItems[6] = 
    {
        {STRING_TOKEN(STR_DBG_PRINT_HIIPARSING), 1, TSE_FILTER_KEY_CUSTOM, FALSE, HiiParseing},
        {STRING_TOKEN(STR_DBG_PRINT_HIIFUNCTION), 1, TSE_FILTER_KEY_CUSTOM, FALSE, HiiFunction},
        {STRING_TOKEN(STR_DBG_PRINT_HIICALLBACK), 1, TSE_FILTER_KEY_CUSTOM, FALSE, HiiCallback},
        {STRING_TOKEN(STR_DBG_PRINT_HIINOTIFICATION) , 1, TSE_FILTER_KEY_CUSTOM, FALSE, HiiNotification},
        {STRING_TOKEN(STR_DBG_PRINT_IFRFORM), 13, TSE_FILTER_KEY_CUSTOM, FALSE, PrintIfrForm},
        {STRING_TOKEN(STR_DBG_PRINT_VARIABLE_CACHE), 13, TSE_FILTER_KEY_CUSTOM, FALSE, PrintVariableCache}
    };
    
    UINT16 Title = HiiAddString( gHiiHandle, L"TSE Debug Print") ;

    if((gDbgPrint & PRINT_UEFI_PARSE)!= PRINT_UEFI_PARSE)
        EfiStrCpy( HiiParseing, L" ") ;

    if((gDbgPrint & PRINT_UEFI)!= PRINT_UEFI)
        EfiStrCpy( HiiFunction, L" ") ;

    if((gDbgPrint & PRINT_UEFI_NOTIFICATION)!= PRINT_UEFI_NOTIFICATION)
        EfiStrCpy( HiiNotification, L" ") ;

    if((gDbgPrint & PRINT_UEFI_CALLBACK)!= PRINT_UEFI_CALLBACK)
        EfiStrCpy( HiiCallback, L" ") ;      

    

    PostManagerDisplayTextBox(
			gHiiHandle,
            Title,
            SetupDebugItems, 
            6,
            SetupDebugKeyHandler
			) ;
#endif    
}

#if TSE_DEBUG_MESSAGES

/**

    @param 

    @retval 
**/
VOID SetupToggleDebugFeature(UINT16 FeatureBit, CHAR16 *Value)
{
    
    //EFI_STATUS Status = EFI_UNSUPPORTED;
    EFI_GUID    guidDbgPrint = TSE_DEBUG_MESSAGES_GUID;  
    
    if((gDbgPrint & FeatureBit)== FeatureBit)
    {
        gDbgPrint &= ~((UINT16)FeatureBit) ;    
        EfiStrCpy( Value, L" ") ;
    }
    else
    {
        gDbgPrint |= (UINT16)FeatureBit ;
        EfiStrCpy( Value, L"x");
    }

    pRS->SetVariable(L"TseDebugPrint", 
        &guidDbgPrint,
        EFI_VARIABLE_NON_VOLATILE |
        EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(gDbgPrint),
        &gDbgPrint);  

  
}
#endif //End of TSE_DEBUG_MESSAGES


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
