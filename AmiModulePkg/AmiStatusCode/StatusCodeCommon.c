//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//    Rev.  1.02
//     Bug Fix:     Enhance SMC debug card function code.
//     Reason:      
//     Auditor:     Jacker Yeh
//     Date:        Jan/21/2017
//
//    Rev.  1.01
//     Bug Fix:     Remove runtime checkpoint debug card support for system cannot enter UEFI OS problem.
//     Reason:      This is workaround, still checking root cause.
//     Auditor:     Jacker Yeh
//     Date:        Oct/28/2016
//
//    Rev.  1.00
//     Bug Fix:     Add SMC Debug card support.
//     Reason:      Write checkpoint to memory address 0xFED40F80.
//     Auditor:     Jacker Yeh
//     Date:        Oct/19/2016
//
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


/** @file StatusCodeCommon.c

**/
//**********************************************************************
#include <AmiLib.h>
#include <Token.h>
//#include <StatusCodeELinks.h>
#include "StatusCodeInt.h"
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/AmiBeepLib.h>
#if SMCPKG_SUPPORT
#include <Library/IoLib.h>
#endif

#pragma pack(1)
typedef struct
{
    UINT32                        ErrorLevel;
    // 12 * sizeof (UINT64) Var Arg stack
    // ascii EFI_DEBUG () Format string
} EFI_DEBUG_INFO;
#pragma pack()

//----------------------------------------------------------------------------
// GUID Definitions
#define EFI_STATUS_CODE_DATA_TYPE_DEBUG_GUID \
          {0x9A4E9246, 0xD553, 0x11D5, 0x87, 0xE2, 0x00, 0x06, 0x29, 0x45, 0xC3, 0xb9}
#define EFI_STATUS_CODE_DATA_TYPE_ASSERT_GUID \
          {0xDA571595, 0x4D99, 0x487C, 0x82, 0x7C, 0x26, 0x22, 0x67, 0x7D, 0x33, 0x07}
static EFI_GUID gEfiStatusCodeDataTypeDebugGuid = EFI_STATUS_CODE_DATA_TYPE_DEBUG_GUID;
static EFI_GUID gEfiStatusCodeDataTypeAssertGuid = EFI_STATUS_CODE_DATA_TYPE_ASSERT_GUID;
EFI_GUID gDataTypeStringGuid = EFI_STATUS_CODE_DATA_TYPE_STRING_GUID;
EFI_GUID gSpecificDataGuid = EFI_STATUS_CODE_SPECIFIC_DATA_GUID;

extern UINT16 StringMaxSize;
#if DETAILED_ERROR_MESSAGES
BOOLEAN PrintDetailedErrorMessage(IN EFI_STATUS_CODE_VALUE Value, OUT UINT8 *String);
#endif
//**********************************************************************
// Status Code Library (functions used throughout the module)
//**********************************************************************
/**
    Match "Value" to Map->Value and return Map->Byte.

        
    @param Map Pointer to a table (array of structures) that maps status code value (EFI_STATUS_CODE_VALUE) to a byte (UINT8) domain specific value.
    @param Value status code to map

         
    @retval UINT8 domain specific mapping of the Value

**/
UINT8 FindByteCode(STATUS_CODE_TO_BYTE_MAP *Map, EFI_STATUS_CODE_VALUE Value)
{
    while (Map->Value!=0)
    {
        if (Map->Value==Value)
        {
            return Map->Byte;
        }

        Map++;
    }

    return 0;
}

/**
    This function creates a string from the ASSERT data

        
    @param Data Pointer to the beginning of the ASSERT data
    @param String Pointer to the beginning of a byte String array

    @retval *String - Contains information from the ASSERT data

**/
EFI_STATUS ReportAssertString(IN EFI_STATUS_CODE_DATA *Data, OUT UINT8 *String)
{
    EFI_DEBUG_ASSERT_DATA *AssertData;
    CHAR8 *FileName;
    UINT32 LineNum;
    CHAR8 *Desc;

    // gather the data for the different parts of the Assert Info
	//The commented out line below is correct (based on PI spec), however, 
	//the EDKII (multiple instances of the DebugLib/ReportStatusCode libraries) does not properly
	//construct the data structure. The EDKII puts EFI_STATUS_CODE_DATA header twice.
	// The +1 below is indended to skit the dummy header (work around for the EDKII bug).
    //AssertData = (EFI_DEBUG_ASSERT_DATA *)Data;
	AssertData = (EFI_DEBUG_ASSERT_DATA *)(Data+1);

    FileName = (CHAR8 *) (AssertData + 1);

    // get line number
    LineNum = AssertData->LineNumber;

    // Get description
    // NOTE: According to the PI specification the length of the file name string
    // should be specified by the FileNameSize field in the data structure.
    // So, the description address should be calculated as Filename + AssertData->FileNameSize;
    // However, since EDKII debug libraries do not initialize FileNameSize, we can't use it and forced to use Strlen. 
    // If in future EDKII libraries will fill FileNameSize field - it is better to switch to above mentioned style.
    Desc = FileName + Strlen(FileName) + 1;

    Sprintf_s(String, StringMaxSize, "ASSERT in %s on %i: %s\n", FileName, LineNum, Desc);
    return EFI_SUCCESS;
}

/**
    This function creates a string for error types that have not
    been predefined by the spec.  This string contains the information in
    the Type and Value parameters.

        
    @param Type Contains the type and severity of the error
    @param Value Contains the Class, SubClass, and Operation that caused
        the error
    @param Data Data field that contains strings and values to be reported
    @param String Pointer to the beginning of a byte String array

    @retval EFI_STATUS

**/
EFI_STATUS ReportData(
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
    IN EFI_STATUS_CODE_DATA *Data, OUT UINT8 *String
)
{
    if (STATUS_CODE_TYPE(Type)!=EFI_ERROR_CODE) return EFI_UNSUPPORTED;
#if DETAILED_ERROR_MESSAGES
    if (!PrintDetailedErrorMessage(Value, String))
#endif
    Sprintf_s(
        String, StringMaxSize,
        "ERROR: Class:%X; Subclass:%X; Operation: %X\n",
        Value & EFI_STATUS_CODE_CLASS_MASK, 
        Value & EFI_STATUS_CODE_SUBCLASS_MASK, 
        Value & EFI_STATUS_CODE_OPERATION_MASK
    );
    return EFI_SUCCESS;
}

/**
    This function creates a string from the debug informatoin packaged into
    Intel-specific(not defined in the standard spec) EFI_DEBUG_INFO structure.

    @param Data Pointer to the beginning of the ASSERT data

    @retval OUT UINT8 *String - Contains information in the ASSERT data

**/
EFI_STATUS ReportDebugInfo(IN EFI_STATUS_CODE_DATA *Data, OUT UINT8 *String)
{
    BASE_LIST Marker;
    CHAR8 *Format;
    EFI_DEBUG_INFO *DebugInfo = (EFI_DEBUG_INFO *)(Data + 1);

    // The first 12 * UINTN bytes of the string are really an
    // argument stack to support varargs on the Format string.
    Marker = (BASE_LIST) (DebugInfo + 1);
    Format = (CHAR8 *)(((UINT64 *)Marker) + 12);

    AsciiBSPrint((CHAR8*)String, StringMaxSize, Format, Marker);
    return EFI_SUCCESS;
}

/**
    This function converts the Type, Value, and Data into a string
    that can be easily output to any console: serial, screen, data storage, etc.

        
    @param Type Contains the type and severity of the error
    @param Value Contains the Class, SubClass, and Operation that caused
        the error
    @param Data Data field that contains strings and values to be reported
    @param String Pointer to the beginning of a byte String array

    @retval EFI_STATUS
        *String - points to the string that was created

**/
EFI_STATUS CreateString(
    IN  EFI_STATUS_CODE_TYPE    Type,
    IN  EFI_STATUS_CODE_VALUE   Value,
    IN  EFI_STATUS_CODE_DATA    *Data,
    OUT UINT8                   *String
)
{
    EFI_STATUS_CODE_STRING_DATA     *StrData;
    CHAR8                           *StrAscii;
    CHAR16                          *StrUnicode;
    UINT16                          Count = 0;

    if (Data != NULL)
    {
        // Check to see if the optional Data parameter is a standard string or
        //  if it is a more complex data structure
        if (!guidcmp(&(Data->Type), &gDataTypeStringGuid))
        {
            // set pointers to different parts of the data structure for
            //  easier access to the data it contains
            StrData = ( EFI_STATUS_CODE_STRING_DATA *)Data;

            // Set up pointer to the beginning of both  parts of the union
            StrAscii = StrData->String.Ascii;
            StrUnicode = StrData->String.Unicode;

            // check for end of string or max length of output string
            while ( ( ((StrData->StringType == EfiStringAscii)
                       && (*StrAscii != '\0'))
                      || ((StrData->StringType == EfiStringUnicode)
                          && (*StrUnicode != '\0')) )
                    && (Count < (StringMaxSize -1))
                  )
            {
                if (StrData->StringType == EfiStringAscii)
                {
                    String[Count++] = *(StrAscii++);
                }

                else if (StrData->StringType == EfiStringUnicode)
                {
                    String[Count] = (UINT8)(*(StrUnicode++));
                    // advance to the next character in the string
                    Count+=2;
                }

                else
                {
                    return EFI_UNSUPPORTED;
                }
            }

            String[Count] = '\0';
            return EFI_SUCCESS;
        }

        else if ( !guidcmp (&Data->Type, &gEfiStatusCodeDataTypeDebugGuid) )
        {
            ReportDebugInfo(Data, String);
        }

        else if (!guidcmp(&(Data->Type), &gSpecificDataGuid))
        {
            // This checks for an assert statement.  Make a string of the data
            //  field
            if ( ((Type & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE)
                    && ((Type & EFI_STATUS_CODE_SEVERITY_MASK) == EFI_ERROR_UNRECOVERED)
                    && ((Value & EFI_STATUS_CODE_OPERATION_MASK)== EFI_SW_EC_ILLEGAL_SOFTWARE_STATE)
                    && (Data != NULL)
               )
            {
                ReportAssertString(Data, String);
            }

            else
            {
                ReportData(Type, Value, Data, String);
            }
        }

        else if ( !guidcmp (&Data->Type, &gEfiStatusCodeDataTypeAssertGuid) )
        {
            //this is Intel specific GUID
            //we have to use Data+1 because of Intel specific bug
            ReportAssertString(Data+1, String);
        }

        else
        {
            ReportData(Type, Value, Data, String);
        }
    }

    else   // if (Data != NULL)
    {
        // If Data is NULL just report required data
        ReportData(Type, Value, NULL, String);
    }

    return EFI_SUCCESS;
}

/**
    Error Code Action.
    Halts the execution.

        
    @param PeiServices not used
    @param Value Value of the error code that triggered the action.

         
    @retval VOID

**/
VOID Halt(
    IN VOID *PeiServices, IN EFI_STATUS_CODE_VALUE Value
)
{
    EFI_DEADLOOP();
}

/**
    Error Code Action.
    Attempts to perform a system reset. If reset fails, halts the execution (calls Halt function).

        
    @param PeiServices not used
    @param Value Value of the error code that triggered the action.

         
    @retval VOID

**/
VOID ResetOrHalt(
    IN VOID *PeiServices, IN EFI_STATUS_CODE_VALUE Value
)
{
    ResetOrResume(PeiServices,Value);
    Halt(PeiServices, Value);
}


extern INIT_FUNCTION* InitPartsList[];

extern STRING_FUNCTION* StringList[];

extern SIMPLE_FUNCTION* SimpleList[];

extern MISC_FUNCTION* MiscList[];

extern CHECKPOINT_FUNCTION* CheckpointList[];

typedef STATUS_CODE_TO_BYTE_MAP STATUS_CODE_TO_BYTE_MAP_ARRAY[];

extern STATUS_CODE_TO_BYTE_MAP_ARRAY
CHECKPOINT_PROGRESS_CODES_MAP, CHECKPOINT_ERROR_CODES_MAP,
BEEP_PROGRESS_CODES_MAP, BEEP_ERROR_CODES_MAP
;

STATUS_CODE_TO_BYTE_MAP* CheckPointStatusCodes[] =
{
    //#define EFI_PROGRESS_CODE 0x00000001
    CHECKPOINT_PROGRESS_CODES_MAP,
    //#define EFI_ERROR_CODE 0x00000002
    CHECKPOINT_ERROR_CODES_MAP
    //#define EFI_DEBUG_CODE 0x00000003
};

STATUS_CODE_TO_BYTE_MAP* BeepStatusCodes[] =
{
    //#define EFI_PROGRESS_CODE 0x00000001
    BEEP_PROGRESS_CODES_MAP,
    //#define EFI_ERROR_CODE 0x00000002
    BEEP_ERROR_CODES_MAP
    //#define EFI_DEBUG_CODE 0x00000003
};
BOOLEAN StatusRuntime = FALSE;

#define ERROR_ACTION(Value,Action) Action

extern ERROR_CODE_ACTION RT_ERROR_CODE_ACTIONS EndOfActionList;

#undef ERROR_ACTION
#define ERROR_ACTION(Value,Action) {Value,&Action}

ERROR_CODE_ACTION_MAP RtErrorCodeActions[] =
{
	RT_ERROR_CODE_ACTIONS
    {0,0}
};


extern ERROR_CODE_ACTION_MAP ErrorCodeActions[];


//--------------------Serial Status Begin----------------------------

//----------------------------------------------------------------------------
// Module specific type definitions


//----------------------------------------------------------------------------
// Function Prototypes
EFI_STATUS SerialData(
    IN  EFI_STATUS_CODE_TYPE    Type,
    IN  EFI_STATUS_CODE_VALUE   Value,
    IN  EFI_STATUS_CODE_DATA    *Data,
    OUT UINT8                   *String
);

/**
    Writes the given string to the serial port byte by byte
    using the function SendByte.

        
    @param PeiServices pointer to the PEI Boot Services table
    @param String String to be sent over the serial port

    @retval EFI_SUCCESS

**/
EFI_STATUS SerialOutput(
    IN VOID **PeiServices,
    IN CHAR8 *String
)
{

    UINT8       *Buffer;
    UINTN       Count = 0;


    if (String == NULL) return EFI_SUCCESS;
    // first make data useable
    Buffer = (UINT8*)String;

    // Lets count the size of the string
    while (*(Buffer + Count))
    {
    	    if (*(Buffer + Count) =='\r' && *(Buffer + Count + 1) =='\n') Count+=2;
    	    else if (*(Buffer + Count) =='\n') 	//we need to add \r before \n so if we found \n send all that before it 
            {
        	    if (Count)  
        		    SerialPortWrite(Buffer, Count);	// replace "\n" with "\r\n" and send 2 symbols
        	    SerialPortWrite((UINT8*)"\r\n", 2);
        	    Buffer =  Buffer + Count + 1; 	// init Buffer with nex position after \n
        	    Count = 0;	// and reset Counter
            }
            else 
	    	    Count++;
    }
    if (Count)  
    	SerialPortWrite(Buffer, Count); 

    return EFI_SUCCESS;
}

/**
    The function programs the stop bits, data bits, and baud rate
    of the com port and outputs the debug message "Status Code Available."
    @note  This function does not assumes Super I/O has already been initialized.

        
    @param FfsHeader 
    @param PeiServices pointer to the PEI Boot Services table
    @retval 
        EFI_SUCCESS

**/
EFI_STATUS SerialStatusInit(
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN VOID **PeiServices
)
{
   SerialPortInitialize();
#ifdef EFI_DEBUG
   if (StringMaxSize < 550)
	   SerialOutput(PeiServices, "Status Code Available\n");
   else
	   SerialOutput(PeiServices, "DXE Status Code Available\n");
#endif
    return EFI_SUCCESS;
}

VOID SerialCheckpoint(VOID **PeiServices, UINT8 Checkpoint)
{
    char s[20];
    Sprintf_s(s, sizeof(s), "Checkpoint %X\n",Checkpoint);
    SerialOutput(PeiServices, s);
}
//--------------------Serial Status End-------------------------------
/**
    calls initialization routines of status code subcomponents
    registered under StatusCodeInitialize eLink.

        
    @param ImageHandler 
    @param ServicesTable 

    @retval VOID

**/
VOID InitStatusCodeParts(IN VOID *ImageHandler, IN VOID *ServicesTable)
{
    UINTN i;
    
    for (i=0; InitPartsList[i]; i++) InitPartsList[i](ImageHandler,ServicesTable);
}

/**
    Calls the string satus code handlers registered under ReportStringStatus eLink.

        
    @param FfsHeader 
    @param String 

    @retval VOID

**/
VOID StringStatusReport(IN VOID *PeiServices, IN CHAR8 *String)
{
    UINTN i;
    
    for (i=0; StringList[i]; i++) StringList[i](PeiServices,String);
}

/**
    Calls simple status code handlers registered under ReportSimpleStatus eLink

        
    @param PeiServices pointer to Pei Services
    @param Type Contains the type and severity of the error
    @param Value Contains the Class, SubClass, and Operation that caused
        the error

    @retval VOID

**/
VOID SimpleStatusReport(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value
)
{
    UINTN i;

    for (i=0; SimpleList[i]; i++) SimpleList[i](PeiServices,Type,Value);
}

/**
    Calls Misc. status code handlers
    (handlers that consume all available status code data)
    registered under ReportMiscStatus eLink

        
    @param PeiServices pointer to Pei Services
    @param Type Contains the type and severity of the error
    @param Value Contains the Class, SubClass, and Operation that caused
        the error
    @param Instance Instance
    @param CallerId OPTIONAL - The GUID of the caller function
    @param Data OPTIONAL - the extended data field that contains additional info

    @retval VOID

**/
VOID MiscStatusReport(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
    IN UINT32 Instance, IN CONST EFI_GUID *CallerId OPTIONAL,
    IN EFI_STATUS_CODE_DATA *Data OPTIONAL
)
{
    UINTN i;
    
    for (i=0; MiscList[i]; i++) MiscList[i](PeiServices,Type,Value,Instance,CallerId,Data);
}

/**
    Calls checkpoint status code handlers registered under ReportCheckpoint eLink.

        
    @param PeiServices pointer to Pei Services
    @param Checkpoint checkpoint

    @retval VOID

**/
VOID CheckpointStatusReport(IN VOID *PeiServices, IN UINT8 Checkpoint)
{
    UINTN i;
    
    for (i=0; CheckpointList[i]; i++) CheckpointList[i](PeiServices,Checkpoint);
}

/**
    Scans list of error actions defined by the PeiErrorCodeActions/DxeErrorCodeActions lists.
    If action corresponding to the passed in Value is found, performs the action.

        
    @param PeiServices pointer to Pei Services
    @param Type Contains the type and severity of the error
    @param Value Contains the Class, SubClass, and Operation that caused
        the error

    @retval VOID

**/
VOID PerformErrorCodeAction(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value
)
{
    ERROR_CODE_ACTION_MAP *Map = ErrorCodeActions;
    
    if (StatusRuntime) Map = RtErrorCodeActions;
    
    while (Map->Value!=0)
    {
        if (Map->Value==Value)
        {
            if (Map->Action!=NULL)
            {
                Map->Action(PeiServices,Value);
                return;
            }
        }
        
        Map++;
    }
}

/**
    Top level status code reporting routine exposed by the status code protocol/PPI.
    Calls the various types of status code handlers
    (SimpleStatusReport, StringStatusReport, MiscStatusReport, PerformErrorCodeAction)
    Generates string from the status code data to pass to StringStatusReport function.

        
    @param PeiServices pointer to the PEI Boot Services table
    @param Type the type and severity of the error that occurred
    @param Value the Class, subclass and Operation that caused the error
    @param Instance 
    @param CallerId OPTIONAL - The GUID of the caller function
    @param Data OPTIONAL - the extended data field that contains additional info

    @retval EFI_STATUS

**/
EFI_STATUS AmiReportStatusCode (
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
    IN UINT32 Instance, IN  EFI_GUID *CallerId OPTIONAL,
    IN EFI_STATUS_CODE_DATA *Data OPTIONAL, UINT8 *String
)
{

    SimpleStatusReport(PeiServices,Type,Value);
//#if STRING_STATUS_SUPPORT
    String[0] = '\0';
    CreateString(Type, Value, Data, String);
    
    // step through the useable data and display information as needed
    // Serial data
    
    if (String[0] != '\0') StringStatusReport(PeiServices,String);

//#endif
    MiscStatusReport(PeiServices,Type,Value,Instance,CallerId,Data);
    
    if (STATUS_CODE_TYPE(Type)==EFI_ERROR_CODE)
    PerformErrorCodeAction(PeiServices,Type,Value);
    return EFI_SUCCESS;
}

//**********************************************************************
// Status Code Transports
/**
    Outputs checkpoint value to port 0x80
    To support different checkpoint displaying hardware do not port this function.
    Use ReportCheckpoint eLink based interface instead.

        
    @param PeiServices pointer to the PEI Boot Services table,
    @param Checkpoint checkpoint to display

    @retval 
        EFI_STATUS

**/
VOID Port80Checkpoint(IN VOID *PeiServices, IN UINT8 Checkpoint)
{
    checkpoint(Checkpoint);
#if SMCPKG_SUPPORT
    if(((Checkpoint != 0xB0) && (Checkpoint != 0xB1))){//Workaround for system cannot enter EFI OS issue. RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN, RT_SET_VIRTUAL_ADDRESS_MAP_END
    	MmioWrite8(SMC_DEBUG_ADDRESS,Checkpoint);	
    	}
#endif
}

/**
    Looks up the checkpoint for the given status code Type
    and Value and calls CheckpointStatusReport function to invoke all checkpoint handlers.

        
    @param PeiServices pointer to the PEI Boot Services table
    @param Type the type and severity of the error that occurred
    @param Value the Class, subclass and Operation that caused the error

    @retval 
        EFI_STATUS

**/
EFI_STATUS CheckPointStatus(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value
)
{
    UINT8 n;
    UINT32 CodeTypeIndex = STATUS_CODE_TYPE(Type) - 1;
    
    if (CodeTypeIndex >= sizeof(CheckPointStatusCodes)/sizeof(STATUS_CODE_TO_BYTE_MAP*) )
        return EFI_SUCCESS;
        
    n=FindByteCode(CheckPointStatusCodes[CodeTypeIndex],Value);
    
    if (n>0) CheckpointStatusReport(PeiServices,n);
    
    return EFI_SUCCESS;
}


/**
    Simple Status Code handler that generates beep codes for a certain types of
    progress and error codes defined by the BEEP_PROGRESS_CODES_MAP and BEEP_ERROR_CODES_MAP mapping tables.

        
    @param PeiServices - pointer to the PEI Boot Services table
    @param Type the type and severity of the error that occurred
    @param Value the Class, subclass and Operation that caused the error

    @retval 
        EFI_STATUS

**/
EFI_STATUS BeepStatus(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value
)
{
    UINT32 CodeTypeIndex = STATUS_CODE_TYPE(Type) - 1;
    UINT8 i,n;
    
    if (CodeTypeIndex >= sizeof(BeepStatusCodes)/sizeof(STATUS_CODE_TO_BYTE_MAP*) ) return EFI_SUCCESS;
    
    n = FindByteCode(BeepStatusCodes[CodeTypeIndex],Value);
    
    if (n>0)
    {
        for (i=0; i<n; i++)
        {
            AmiBeep(29366*2,400000);
            MicroSecondDelay(100000);
        }
    }
    
    return EFI_SUCCESS;
}

/**
    This function plays Value quantity of beeps

        
    @param PeiServices pointer to the PEI Boot Services table
    @param Type the type and severity of the error that occurred
    @param Value the Class, subclass and Operation that caused the error
    @param Instance 
    @param CallerId OPTIONAL - The GUID of the caller function
    @param Data OPTIONAL - the extended data field that contains additional info

    @retval EFI_STATUS

**/
EFI_STATUS BeepService(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
    IN UINT32 Instance, IN EFI_GUID *CallerId OPTIONAL,
    IN EFI_STATUS_CODE_DATA *Data OPTIONAL
)
{
    UINT8 n;
    
    if ((Value&0xFF00)!=AMI_STATUS_CODE_BEEP_CLASS) return EFI_SUCCESS;
    
    n = (UINT8)Value;
    
    if (n)
    {
        UINT8 i;
        
        for (i=0; i<n; i++) { AmiBeep(26163*8,40000); MicroSecondDelay(100000);}
    }
    
    return EFI_SUCCESS;
}

// ******************** AmiDebugService implementation ****************
VOID EFIAPI AmiDebigServiceDebugPrintHelper (
    IN VOID *PeiServices, OUT UINT8 *String, IN UINTN ErrorLevel, IN CONST CHAR8 *Format, VA_LIST VaListMarker
)
{
    if (Format==NULL) return;
    AsciiVSPrint ((CHAR8*)String, StringMaxSize, Format, VaListMarker);
    StringStatusReport(PeiServices,String);
}

VOID EFIAPI AmiDebigServiceDebugAssertHelper (
    IN VOID *PeiServices, OUT UINT8 *String, IN CONST CHAR8  *FileName, IN UINTN LineNumber, IN CONST CHAR8 *Description
)
{
    AsciiSPrint ((CHAR8*)String, StringMaxSize, "ASSERT %a(%d): %a\n", FileName, LineNumber, Description);
    StringStatusReport(PeiServices,String);
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
