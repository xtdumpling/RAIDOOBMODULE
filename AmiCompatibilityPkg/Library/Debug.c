//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/SOURCE/Core/Library/Debug.c 10    7/30/09 4:39p Vyacheslava $
//
// $Revision: 10 $
//
// $Date: 7/30/09 4:39p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: Debug.c
//
// Description:
//  Contains generic debug library functions.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiLib.h>
#include <StatusCodes.h>

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Debug_Functions
//
// Description:
//  Debug related functions defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiPeiLib PEI_TRACE         Macro which creates a serial debug message in PEI.
// AmiDxeLib TRACE             Macro which creates a serial debug message in DXE.
// AmiDxeLib PROGRESS_CODE     Macro to report boot progress information in DXE.
// AmiPeiLib PEI_PROGRESS_CODE Macro report boot progress information in PEI.
// AmiDxeLib ERROR_CODE        Macro to report error condition in DXE.
// AmiPeiLib PEI_ERROR_CODE    Macro to report error condition in PEI.
// AmiLib    checkpoint        Send a checkpoint to port 0x80.  Use PROGRESS_CODE or PEI_PROGRESS_CODE if possible.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions.  Append .h to the given name.
//   
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: PROGRESS_CODE
//
// Description:	
//  PROGRESS_CODE(Code) is a macro which reports progress code
//    using UEFI ReportStatusCode API.
//
// Input:
//  Code - progress code value. 
//    Standard values are defined in AmiStatusCodes.h
//
// Output:
//  VOID.
//
//<AMI_PHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: PEI_PROGRESS_CODE
//
// Description:	
//  PEI_PROGRESS_CODE(PeiServices, Code) is a macro which reports progress code
//    using (*PeiServices)->ReportStatusCode
//
// Input:
//  PeiServices - Double pointer to PEI Services Table.    
//  Code - progress code value. 
//    Standard values are defined in AmiStatusCodes.h
// 
// Output:
//  VOID.
//
//<AMI_PHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: PEI_TRACE
//
// Description:	
//  PEI_TRACE(Arguments) is a macro which creates a serial debug message in
// PEI if the DEBUG_MODE SDL token is enabled by invoking the PeiTrace
// function with the provided arguments.  If DEBUG_MODE is off, nothing is
// generated.  See notes for an example usage.
//
// Input:
//  Arguments
// A bracket enclosed list including the following input parameters for
// PeiTrace (in order from left to right).
//
//  IN UINTN Level
// The error level of the debug message.
//
//  IN EFI_PEI_SERVICES **ppPS
// Double pointer to PEI Services Table.
//
//  IN CHAR8 *sFormat
// Format string for the debug message to print.
//
//  IN ...
// Additional parameters utilized by the format string.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//  PeiTrace
//  Sprintf
// 
// Notes:
//  See Sprintf for a detailed description of the format string parameters.
// Example usage:
//
// PEI_TRACE((TRACE_ALWAYS, PeiServices, "This is a test message.  Status
// is %r.\n", Status));
//          
//<AMI_PHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: TRACE
//
// Description:
//  TRACE(Arguments) is a macro which creates a serial debug message in DXE
// if the DEBUG_MODE SDL token is enabled by invoking the Trace function with
// the provided arguments.  If DEBUG_MODE is off, nothing is generated.  See
// notes for an example usage.
//
// Input:
//  Arguments
// A bracket enclosed list including the following input parameters for
// Trace (in order from left to right).
//
//  IN UINTN Level
// The error level of the debug message. 
//
//  IN CHAR8 *sFormat
// Format string for the debug message to print.
//
//  IN ...
// Argument list for the format string.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//  Trace
//  Sprintf
// 
// Notes:
//  See Sprintf for a detailed description of the format string parameters.
// Example usage:
//
// TRACE((TRACE_ALWAYS, "This is a test message.  Status is %r.\n", Status));
//          
//<AMI_PHDR_END>
//*************************************************************************


/**
  This function parses the input string for lowercase s and capital G and replaces them with 
  a and g respectively.  The Debug functions treat s as S and they don't recognize G
  
  @param String String to modify the format specifiers
  
  @return VOID
 */
VOID ProcessFormatStringtoEdk2(CHAR8 *String)
{
    CHAR8 *StrPtr;
    
    StrPtr = String;

    while (*StrPtr != 0)
    {
        // Check for a atring modifier first.  if no '%' sign then move to the next character
        if (*StrPtr != '%')
        {
            StrPtr++;
            continue;
        }
        
        StrPtr++;
        // Percent character found.  Check for next character
        if (*StrPtr == 's')
        {
            *StrPtr = 'a';
        }
        else if (*StrPtr == 'G')
        {
            *StrPtr = 'g';
        }
        StrPtr++;
    } // end of while (StrPtr != '%')
}



//*************************************************************************
//
// Name: PrepareStatusCodeString
//
// Description: Produces a Status Code ASCII string in the output
// buffer.  The ASCII string is produced by parsing the format string 
// specified by format.  Arguments are pulled from the variable argument
// list specified by ArgList based on the contents of the format string.
// This is internal function used by TRACE and PEI_TRACE
//
// Input:
//  pBuffer - Pointer to a EFI_STATUS_CODE_STRING_DATA buffer. User is 
//  responsible for allocating the necessary memory resources!
//  
//  Size - Size of the buffer in bytes.
//
//  sFormat - Pointer to a null-terminated format ASCII string.
//
//  ArgList - Marker for the variable argument list.
//
// Output: VOID
//
// Referrals: Sprintf_va_list
//
//          
//*************************************************************************
VOID PrepareStatusCodeString( VOID* pBuffer, UINTN Size, CHAR8 *sFormat, VA_LIST ArgList )
{
    static CHAR8 OverflowMessageStr[] = "... DEBUG BUFFER OVERFLOW!!!\n";
    UINTN n;

    static EFI_STATUS_CODE_STRING_DATA Header = 
    {
        {
            sizeof(EFI_STATUS_CODE_DATA),
            sizeof(EFI_STATUS_CODE_STRING_DATA) - sizeof(EFI_STATUS_CODE_DATA),
            EFI_STATUS_CODE_DATA_TYPE_STRING_GUID
        }, EfiStringAscii, NULL
    };
    EFI_STATUS_CODE_STRING_DATA *pData = pBuffer;

    //Prepare the data strucutre
    *pData = Header;
    pData->String.Ascii = (CHAR8*)pData + sizeof(EFI_STATUS_CODE_STRING_DATA);

    //Prepare the string
    n = Sprintf_s_va_list( 
        (CHAR8*)(pData + 1), 
        Size - sizeof(EFI_STATUS_CODE_STRING_DATA), 
        sFormat, 
        ArgList 
    );
    if ( n < (Size - sizeof(EFI_STATUS_CODE_STRING_DATA) - 1) || 
         Size < (sizeof(OverflowMessageStr) + sizeof(EFI_STATUS_CODE_STRING_DATA)) )
        return;

    // Buffer Overflow!
    // Cut of the previous debug message plus error message
    MemCpy(
        (CHAR8*)pBuffer + Size - sizeof(OverflowMessageStr),
        OverflowMessageStr, 
        sizeof(OverflowMessageStr)
    );
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
