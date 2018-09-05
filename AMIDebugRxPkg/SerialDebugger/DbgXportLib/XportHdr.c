//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
// $Header: /AptioV/BIN/SerialDebugger/DbgXportLib/XportHdr.c 3     2/20/15 10:25a Jekekumarg $
//
// $Revision: 3 $
//
// $Date: 2/20/15 10:25a $
//*****************************************************************
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/BIN/SerialDebugger/DbgXportLib/XportHdr.c $
// 
// 3     2/20/15 10:25a Jekekumarg
// [TAG]			EIP205866
// [Category]   	Improvement
// [Description]	New internal label (INT)5.004_SerialDebugger_3.01.0017
// for Aptio5 Serial Debugger Module
// [Files]			DbgXportLib.lib
// DbgXportX64Lib.lib
// XportHdr.c
// 
// 2     12/31/14 6:38p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0016 Label
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     9/21/13 4:28a Sudhirv
// Added for AMI Debug for UEFI 3.01.0006 Label.
// 
// 1     11/02/12 10:04a Sudhirv
// AMIDebugRx eModule for AMI Debug for UEFI 3.0
// 
// 5     10/25/12 5:11p Sudhirv
// Moved DbgIORead32 and Stall functions from Xp_Layer.c to XportHdr.c to
// be in sync with the binaries.
// 
// 4     10/09/12 12:07p Sudhirv
// [TAG]     EIP99636
// [Category] Enhancement
// [Severity] Normal
// [Symptom] Debugger 3.0: Make changes to enable AMIDebugRx\Debugger to
// boot normally if AMIDebugRx is not connected.
// 
// 3     8/08/12 3:58p Sudhirv
// Updated after Binaries prepared.
// 
// 2     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//*****************************************************************
//*****************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:		XportHdr.c
//
// Description:	File containing the globals for CAR base address & size.
//				Also the code which make use of this so that it can be
//				integrated as it is Debugger eModule of binary.
//
//<AMI_FHDR_END>
//**********************************************************************
#include "efi.h"
#include "Pei.h"

//#include "misc.h"
#ifndef	EFIx64
#include <Library\AMIPeiDebug.h>
#else
#include <Library\AMIPeiDebugX64.h>
#endif
#include <Library\AMIPeiGUIDs.h>

#include "token.h"
#include "timestamp.h"
#include <Library/IoLib.h>

#define CONVERT_TO_STRING(a) #a
#define STR(a) CONVERT_TO_STRING(a)

UINTN	AMI_PEIDEBUGGER_DS1_SIZE = 0x500;
UINT8 m_ExceptionPending = 0;

EFI_GUID  mPeiDebugDataGuidXp = PEI_DBGSUPPORT_DATA_GUID;
EFI_GUID  mPeiDbgBasePpiGuidXp = EFI_PEI_DBG_BASEADDRESS_PPI_GUID;
const char *sTargetProjectTag= STR(TARGET_PROJECT_TAG);

#ifdef PeiDebugger_SUPPORT
#if PeiDebugger_SUPPORT
volatile UINTN gPeiDebuggerEnabled = 1;
#else
volatile UINTN gPeiDebuggerEnabled = 0;
#endif
#endif

volatile UINTN USB_DEBUGGER_ENABLED = USB_DEBUG_TRANSPORT;
UINT8 HostMessage[64] = "AMIDebugRx Authentication Failed, Firmware update required...\n";
typedef struct {
	UINT16		Year;
	UINT8		Month;
	UINT8		Day;
	UINT8		Hour;
	UINT8		Minute;
	UINT8		Second;
} DateTime_T;

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	IoRead32
//
// Description:	Internal Helper function. Reads 32-bit value from IO port
//
// Input:		UINT16 
//
// Output:		UINT32 - port data
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
UINT32 DbgIoRead32(UINT16 Port)
{
#ifdef	EFIx64
	return IoRead32(Port);
#else
	_asm {
		mov dx, Port
		in eax, dx
	}
#endif
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	Stall
//
// Description:	Internal Helper function.
//
// Input:		UINTN  
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void Stall (UINTN Usec)
{
    UINTN   Counter = (Usec * 7)/2; // 3.58 count per microsec
    UINTN   i;
    UINT32  Data32;
    UINT32  PrevData;

    PrevData = DbgIoRead32(PM_BASE_ADDRESS + 8);
    for (i=0; i < Counter; ) {
       Data32 = DbgIoRead32(PM_BASE_ADDRESS + 8);
        if (Data32 < PrevData) {        // Reset if there is a overlap
            PrevData=Data32;
            continue;
        }
        i += (Data32 - PrevData);
		PrevData=Data32; // FIX need to find out the real diff betweek different count.
    }
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetBuildTime()
//
// Description:	Sends the Build Time Details
//
// Input:		void*
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
#define Str2No(A)	(A - '0')

#define TwoDigitStr2BCD(String) (Str2No(String[0])*0x10+Str2No(String[1]))

VOID GetBuildTime(void *Buffer)
{
	UINT8 *strYear = FOUR_DIGIT_YEAR;
	UINT8 *strMonth = TWO_DIGIT_MONTH;
	UINT8 *strDay = TWO_DIGIT_DAY;
	UINT8 *strHour = TWO_DIGIT_HOUR;
	UINT8 *strMin = TWO_DIGIT_MINUTE;
	UINT8 *strSec = TWO_DIGIT_SECOND;


	((DateTime_T*)Buffer)->Year=(Str2No(strYear[0])*0x1000+Str2No(strYear[1])*0x100+Str2No(strYear[2])*0x10+Str2No(strYear[3]));
	((DateTime_T*)Buffer)->Month=TwoDigitStr2BCD(strMonth);
	((DateTime_T*)Buffer)->Day=TwoDigitStr2BCD(strDay);
	((DateTime_T*)Buffer)->Hour=TwoDigitStr2BCD(strHour);
	((DateTime_T*)Buffer)->Minute=TwoDigitStr2BCD(strMin);
	((DateTime_T*)Buffer)->Second=TwoDigitStr2BCD(strSec);
	return;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetFlowCtrl
//
// Description:	Internal Helper function. returns Flow Ctrl pcd value
//
// Input:		void
//
// Output:		Boolean
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN GetFlowCtrl()
{
	return PcdGetBool (PcdSerialUseHardwareFlowControl);
}

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
