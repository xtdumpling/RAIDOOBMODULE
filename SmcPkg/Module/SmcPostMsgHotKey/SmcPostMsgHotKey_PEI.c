//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.07
//    Bug Fix: Add BMC IPv6 address display during POST
//    Reason : The rule for IPv6 show:
//             1) No display if IPv6 address read from BMC is failure or empty;
//             2) IPv6 Static Address will be shown if BMC report this address was enabled;
//             3) IPv6  Dynamic Address(SLAAC/DHCPv6) will be shown if no static IP address is enabled;
//             4) Continuous zero address bytes longer than 2 will be display as '::' instead.
//    Auditor: Max Mu
//    Date   : Jul/12/2017
//
//  Rev. 1.06
//    Bug Fix: 1.Fixed still show warning message on POST screen at first boot after changing VGA priority from Offboard to Onboard.
//    Reason : 
//    Auditor: Chen Lin
//    Date   : June/23/2017
//
//  Rev. 1.05
//    Bug Fix:  Modify location of graphic mode progress-code.
//    Reason:   
//    Auditor:  YuLin Yang
//    Date:     Mar/17/2017
//
//  Rev. 1.04
//    Bug Fix:  Early Graphic Logo Support.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Mar/14/2017
//
//  Rev. 1.03
//    Bug Fix:  Show change VGA connector information when output display change to offboard VGA.
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     Jun/23/2016
//
//  Rev. 1.02
//    Bug Fix:  Display BMC IP at early POST.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/22/2016
//
//  Rev. 1.01
//    Bug Fix:  Use AMIBCP to disable boot procedure messages displaying.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/21/2016
//
//     Rev. 1.00
//       Reason:	Rewrite SmcPostMsgHotKey
//       Auditor:       Leon Xu 
//       Date:          Dec/19/2014
//
//****************************************************************************
//****************************************************************************
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiPeiLib.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Ppi/Stall.h>
#include <AmiModulePkg/AmiStatusCode/StatusCodeMapPei.c>
#include <Library/DebugLib.h>
//#include <Library/PeiServicesTablePointerLib.h>
#include <Setup.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/SetupVariable.h>
#if EarlyVideo_SUPPORT == 1
#include <Guid/SocketMpLinkVariable.h>
#if IPMI_SUPPORT == 1
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Ppi/IPMITransportPpi.h>
#define GET_LAN_COMMAND_DATA_SIZE 0x04
static UINT8 HasPrintedBmcIp = 0;
#endif // #if IPMI_SUPPORT == 1
#endif  // #if EarlyVideo_SUPPORT == 1

#include "SmcPostPrintLib.h"

#if EarlyVideo_SUPPORT == 1
#include "PeiSimpleTextOutPpi.h"
static EFI_GUID  gEfiPeiSimpleTextOutPPIGuid = EFI_PEI_SIMPLETEXTOUT_PPI_GUID;
#endif 

typedef struct {
	EFI_STATUS_CODE_VALUE	Value;
	CHAR8*	str;
} SMCPOSTMSGSTRINGTABLE;

static SMCPOSTMSGSTRINGTABLE SmcPostMsgPEIStringTable[] = {
	{PEI_MEMORY_INIT,
		"  PEI--Intel MRC Execution.."},
	{PEI_CPU_INIT,
		"  PEI--CPU Initialization.."},
	{PEI_CPU_CACHE_INIT,
		"  PEI--CPU Cache Initialization.."},
	{PEI_CPU_AP_INIT,
		"  PEI--CPU AP Initialization.."},
	{PEI_CAR_SB_INIT,
		"  PEI--SB Initialization.."},
	{PEI_DXE_IPL_STARTED,
		"  PEI--DXE Phase Start.."},
	{0, 0}
};

#if EarlyVideo_SUPPORT == 1

#if IPMI_SUPPORT == 1

#define IP_ADDR_STR_MAX_CHAR_LEN     64      //for IPv6   'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF' - 128Bit address coded
//#define IP_ADDR_STR_MAX_CHAR_LEN     32    //for IPv4   'FF:FF:FF:FF' - 32Bit address coded

#if BMC_IPV6_POST_SCREEN_SUPPORT

//#define BMC_IPV6_ADDR_DEBUG


#define DEFAULT_ZERO_START     ((UINTN) ~0)


// PEI phase code excuting on ROM, actually there is no Global Variable to use.
// All Global Variables we write here is just working as Global Constant
// PEIM resident on flash, global variables are read-only and it is not 
// possible to save the pointer to a global variable to be shared inside the PEIM
// if want to use Global Variable, try PeiServices instead.(Global data in CAR - Cache As RAM, such as PPI structure)

//static UINT8 HasPrintedBmcIpv6 = 0;

//
//
//  Convert one EFI_IPv6_ADDRESS to Null-terminated ASIIC string.
//  Return the string length.
//

UINT32 SmcConvertBmcIp6ToStr (
   OUT  UINT8     *String,
   IN   UINT8     *Ip6Address/*16Byte IPv6 address*/
  )
{
    UINT16     Ip6Addr[8];
    UINTN      Index;
    UINTN      LongestZerosStart;
    UINTN      LongestZerosLength;
    UINTN      CurrentZerosStart;
    UINTN      CurrentZerosLength;
    UINT8      Buffer[sizeof"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"];
    UINT8      *Ptr;
    UINT32     i = 0;
    
    if (Ip6Address == NULL || String == NULL) {
        return 0;
    }

    //
    // Convert the UINT8 array to an UINT16 array for easy handling.
    // 
    for (i = 0; i < 8; i++) {
        Ip6Addr[i] = 0;
    }
    
    for (Index = 0; Index < 16; Index++) {
        Ip6Addr[Index / 2] |= (Ip6Address[Index] << ((1 - (Index % 2)) << 3));
    }

    //
    // Find the longest zeros and mark it.
    //
    CurrentZerosStart  = DEFAULT_ZERO_START;
    CurrentZerosLength = 0;
    LongestZerosStart  = DEFAULT_ZERO_START;
    LongestZerosLength = 0;
    for (Index = 0; Index < 8; Index++) {
        if (Ip6Addr[Index] == 0) {
            if (CurrentZerosStart == DEFAULT_ZERO_START) {
                CurrentZerosStart = Index;
                CurrentZerosLength = 1;
            } else {
               CurrentZerosLength++;
            }
        } else {
            if (CurrentZerosStart != DEFAULT_ZERO_START) {
                if (CurrentZerosLength > 2 && (LongestZerosStart == (DEFAULT_ZERO_START) || CurrentZerosLength > LongestZerosLength)) {
                    LongestZerosStart = CurrentZerosStart;
                    LongestZerosLength = CurrentZerosLength;
                }
                CurrentZerosStart  = DEFAULT_ZERO_START;
                CurrentZerosLength = 0;
           }
        }
    }

  if (CurrentZerosStart != DEFAULT_ZERO_START && CurrentZerosLength > 2) {
    if (LongestZerosStart == DEFAULT_ZERO_START || LongestZerosLength < CurrentZerosLength) {
      LongestZerosStart  = CurrentZerosStart;
      LongestZerosLength = CurrentZerosLength;
    }
  }

  if(LongestZerosLength >= 8) return 0;//all zero

  Ptr = Buffer;
  for (Index = 0; Index < 8; Index++) {
    if (LongestZerosStart != DEFAULT_ZERO_START && Index >= LongestZerosStart && Index < LongestZerosStart + LongestZerosLength) {
      if (Index == LongestZerosStart) {
        *Ptr++ = ':';
      }
      continue;
    }
    if (Index != 0) {
      *Ptr++ = ':';
    }
    Ptr += Sprintf(Ptr, "%x", Ip6Addr[Index]);
  }
  
  if (LongestZerosStart != DEFAULT_ZERO_START && LongestZerosStart + LongestZerosLength == 8) {
    *Ptr++ = ':';
  }
  *Ptr = '\0';

  Sprintf (String, "BMC IPv6: %s", Buffer);

  for (i = 0; i < IP_ADDR_STR_MAX_CHAR_LEN; i++) {
      if (String[i] == 0) break;
  }
  
  return i;
}

UINT32 GetBmcIpv6 (
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT8* LanIpStr
)
{
    EFI_STATUS Status;
    PEI_IPMI_TRANSPORT_PPI *IpmiTransportPpi;
    UINT32 LanIpStrLen = 0;
    UINT8 LanCommandData[GET_LAN_COMMAND_DATA_SIZE];
    UINT32 LanCommandDataSize = GET_LAN_COMMAND_DATA_SIZE;
    UINT8 ResData[64];
    UINT32 ResDataSize = 64;
    UINT32 i;

    //
    //Locate IPMI Transport protocol to send commands to BMC.
    //
    Status = (*PeiServices)->LocatePpi (
        (CONST EFI_PEI_SERVICES**)PeiServices,
        &gEfiPeiIpmiTransportPpiGuid,
        0,
        NULL,
        (VOID **)&IpmiTransportPpi );

    if ( EFI_ERROR(Status) ) {
        return 0;
    }
    
    for (i = 0; i < 4; i++) {
        LanCommandData[i] = 0x0;
    }
    for (i = 0; i < 64; i++) {
        ResData[i] = 0x0;
    }

    //
    // Get the BMC IPv6 Static address
    //
    LanCommandData[0]  = BMC_LAN1_CHANNEL_NUMBER;
    LanCommandData[1]  = EfiIpv6StaticAddress;
    LanCommandDataSize = GET_LAN_COMMAND_DATA_SIZE;
    Status = IpmiTransportPpi->SendIpmiCommand (
        IpmiTransportPpi,
        IPMI_NETFN_TRANSPORT,
        BMC_LUN,
        IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
        LanCommandData,
        LanCommandDataSize,
        &(ResData[0]),
        (UINT8*)&ResDataSize );

    if (!EFI_ERROR (Status)) {
        
        #ifdef BMC_IPV6_ADDR_DEBUG
        PEI_TRACE((-1, PeiServices, "EfiIpv6StaticAddress Response : \n"));
        for (i = 0; i < 21; i++) {
            PEI_TRACE((-1, PeiServices, "ResData[%d] = %02x\n",i,ResData[i]));
        }
        #endif
        
        if(ResData[2] & BIT7)//Address source/type is enable?
        {
            LanIpStrLen = SmcConvertBmcIp6ToStr(LanIpStr, &(ResData[3]));
            if (LanIpStrLen > 0) {
                return LanIpStrLen;
            }
        }
    }
    PEI_TRACE((-1, PeiServices, "GetBmcIpv6 : %d\n",__LINE__));
    for (i = 0; i < 4; i++) {
        LanCommandData[i] = 0x0;
    }
    for (i = 0; i < 64; i++) {
        ResData[i] = 0x0;
    }

    //
    // Get the BMC IPv6 Dynamic address
    //
    LanCommandData[0]  = BMC_LAN1_CHANNEL_NUMBER;
    LanCommandData[1]  = EfiIpv6DhcpAddress;
    LanCommandDataSize = GET_LAN_COMMAND_DATA_SIZE;
    Status = IpmiTransportPpi->SendIpmiCommand (
        IpmiTransportPpi,
        IPMI_NETFN_TRANSPORT,
        BMC_LUN,
        IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
        LanCommandData,
        LanCommandDataSize,
        &(ResData[0]),
        (UINT8*)&ResDataSize );

    if (!EFI_ERROR (Status)) {
        
        #ifdef BMC_IPV6_ADDR_DEBUG
        PEI_TRACE((-1, PeiServices, "EfiIpv6DhcpAddress Response : \n"));
        for (i = 0; i < 21; i++) {
            PEI_TRACE((-1, PeiServices, "ResData[%d] = %02x\n",i,ResData[i]));
        }
        #endif
        
        LanIpStrLen = SmcConvertBmcIp6ToStr(LanIpStr, &(ResData[3]));
        if (LanIpStrLen > 0) {
            return LanIpStrLen;
        }
    }

    return 0;
}
#endif

//
// Return the string length.
//
UINT32 SmcConvertNumToIpAddress(
  OUT UINT8* Destination,
  IN UINT8* Source
)
{
    UINT32 i = 0;

    if (Source[0] == 0 && Source[1] == 0 && Source[2] == 0 && Source[3] == 0) return 0;

    Sprintf (Destination, "BMC IP:%d.%d.%d.%d", Source[0], Source[1], Source[2], Source[3]);
    for (i = 0; i < 32; i++) {
        if (Destination[i] == 0) break;
    }
    return i;
}

//
// Return :
//          0 - No BMC IP
//          > 0 - String Length
//
UINT32 GetBmcIp (
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT8* LanIpStr
)
{
    EFI_STATUS Status;
    PEI_IPMI_TRANSPORT_PPI *IpmiTransportPpi;
    UINT32 LanIpStrLen = 0;
    UINT8 LanCommandData[GET_LAN_COMMAND_DATA_SIZE];
    UINT32 LanCommandDataSize = GET_LAN_COMMAND_DATA_SIZE;
    UINT8 ResData[64];
    UINT32 ResDataSize = 64;

    //
    //Locate IPMI Transport protocol to send commands to BMC.
    //
    Status = (*PeiServices)->LocatePpi (
        (CONST EFI_PEI_SERVICES**)PeiServices,
        &gEfiPeiIpmiTransportPpiGuid,
        0,
        NULL,
        (VOID **)&IpmiTransportPpi );
    if ( EFI_ERROR(Status) ) {
        return 0;
    }

    //
    // Get station IP address of Channel[1]
    //
    LanCommandData[0]  = BMC_LAN1_CHANNEL_NUMBER;
    LanCommandData[1]  = IpmiLanIpAddress;
    LanCommandDataSize = GET_LAN_COMMAND_DATA_SIZE;

    Status = IpmiTransportPpi->SendIpmiCommand (
        IpmiTransportPpi,
        IPMI_NETFN_TRANSPORT,
        BMC_LUN,
        IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
        LanCommandData,
        LanCommandDataSize,
        &(ResData[0]),
        (UINT8*)&ResDataSize );
    if (!EFI_ERROR (Status)) {
        LanIpStrLen = SmcConvertNumToIpAddress (LanIpStr, &(ResData[1]));
        if (LanIpStrLen > 0) {
            return LanIpStrLen;
        }
    }
    return 0;
}

#endif // #if IPMI_SUPPORT == 1

#endif // #if EarlyVideo_SUPPORT == 1

STATIC UINT8 SmcPostMsgHotKeyPEIFindString(
	EFI_STATUS_CODE_VALUE Value, 
	CHAR8** str
)
{
	UINT32	i = 0;
	UINT8	iSize = 0;
	CHAR8*	pStr;
	
	while( SmcPostMsgPEIStringTable[i].Value != 0 ) {
		if ( SmcPostMsgPEIStringTable[i].Value == Value ) {
			pStr = *str = SmcPostMsgPEIStringTable[i].str;
			while( pStr[iSize] != 0 ) {
				iSize++;
			}
			return iSize;
		}
	
		i++;
	}

	return 0;
}

#ifdef EFI_DEBUG
VOID SmcDelay500ms(
	IN CONST	EFI_PEI_SERVICES		**PeiServices
	)
{
	EFI_STATUS	Status=EFI_SUCCESS;
	EFI_PEI_STALL_PPI *Stall = NULL;
	UINT8 iCount = 0;
	Status = (*PeiServices)->LocatePpi(PeiServices,&gEfiPeiStallPpiGuid,0,NULL,(VOID**)&Stall);
	if ( EFI_ERROR(Status) || Stall == NULL )
		return;

	for(iCount=0; iCount<1; iCount++) {
		IoWrite8(0x80, 0xfb-iCount);
		Stall->Stall(PeiServices, Stall, 300000);
	}
}
#endif

EFI_STATUS
SmcPostMsgHotKeyPEIReportWorker(
    IN	CONST EFI_PEI_SERVICES		**PeiServices,
    IN	EFI_STATUS_CODE_TYPE		CodeType,
    IN	EFI_STATUS_CODE_VALUE		Value,
    IN	UINT32				Instance,
    IN	CONST EFI_GUID			*CallerId,
    IN	CONST EFI_STATUS_CODE_DATA	*Data OPTIONAL
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    SETUP_DATA	SetupData;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI	*ReadOnlyVariable = NULL;
    UINTN	VariableSize = sizeof(SETUP_DATA);
    EFI_GUID	mSetupGuid = SETUP_GUID;
#if EarlyVideo_SUPPORT == 1
    EFI_PEI_SIMPLETEXTOUT_PPI	*vSimpleTextOutPpi = NULL;
    SOCKET_MP_LINK_CONFIGURATION	SocketMpLinkConfiguration;
    SYSTEM_CONFIGURATION    SystemConfiguration;
    UINT32	VarAttrib;			
    UINT8	dot, n;
    CHAR8*	str;
    CHAR8*	strBlank = " ";
#if EarlyVideo_Mode
    UINT8   Attr = 0x0F;
#endif

#if IPMI_SUPPORT == 1
    UINT8   LanIpStr[IP_ADDR_STR_MAX_CHAR_LEN], EmptyStr[IP_ADDR_STR_MAX_CHAR_LEN];
    UINT32  LanIpStrLen = 0, EmptyStrLen = 0;
#endif // #if IPMI_SUPPORT == 1
#endif  // #if EarlyVideo_SUPPORT == 1

    Status = (*PeiServices)->LocatePpi(
		PeiServices,
		&gEfiPeiReadOnlyVariable2PpiGuid,
		0,
		NULL,
		&ReadOnlyVariable);

    if(!EFI_ERROR(Status)){
	Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
			L"Setup",
			&mSetupGuid,
			NULL,
			&VariableSize,
			&SetupData);

	if(!EFI_ERROR(Status))
	    if(!SetupData.SmcBootProcedureMsg_Support)	return EFI_SUCCESS;
    }
#if EarlyVideo_SUPPORT == 1
    VariableSize = sizeof (SOCKET_MP_LINK_CONFIGURATION);

    Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
			SOCKET_MP_LINK_CONFIGURATION_NAME,
			&gEfiSocketMpLinkVariableGuid,
			&VarAttrib,
			&VariableSize,
			&SocketMpLinkConfiguration);
    
    VariableSize = sizeof(SYSTEM_CONFIGURATION);
    Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
			L"IntelSetup",
			&gEfiSetupVariableGuid,
			NULL,
			&VariableSize,
			&SystemConfiguration);
    
    dot = n = 0;
    str = NULL;

    Status = (*PeiServices)->LocatePpi(
                PeiServices,
                &gEfiPeiSimpleTextOutPPIGuid,
                0,
                NULL,
                &vSimpleTextOutPpi);

    if(EFI_ERROR(Status) || NULL == vSimpleTextOutPpi)
	return EFI_SUCCESS;

#if EarlyVideo_Mode
    Attr = PcdGet8(PcdSmcColor);
    if(CodeType == EFI_PROGRESS_CODE){
	dot = SmcPostMsgHotKeyPEIFindString(Value, &str);
	if(0 != dot){
	    // Clean the line 24
	    for(n=0; n<50; n++)
	        vSimpleTextOutPpi->OutputString (n, SMC_MAX_ROW-2, Attr, strBlank);

	    if((SocketMpLinkConfiguration.LegacyVgaSoc) == 0 && (SocketMpLinkConfiguration.LegacyVgaStack == 0)){
	        // Show the string
	        vSimpleTextOutPpi->OutputString(00, SMC_MAX_ROW-2, Attr, str);
	        // Show the blink dot
	        vSimpleTextOutPpi->OutputString(dot, SMC_MAX_ROW-2, Attr, ".");
	    } else if ( SystemConfiguration.VideoSelect == 2 ) {
            vSimpleTextOutPpi->OutputString (00, SMC_MAX_ROW-2, Attr, "  iKVM doesn't support add-on VGA device.");
            vSimpleTextOutPpi->OutputString (00, SMC_MAX_ROW-1, Attr, "  Please change the D-SUB connector to Add-on VGA device...");
	    }
	}
#else
    if(CodeType == EFI_PROGRESS_CODE){
	dot = SmcPostMsgHotKeyPEIFindString(Value, &str);
	if(0 != dot){
	    // Clean the line 24
	    for(n=0; n<50; n++)
	        vSimpleTextOutPpi->OutputString (n, SMC_MAX_ROW-1, 0x07, strBlank);

	    if((SocketMpLinkConfiguration.LegacyVgaSoc) == 0 && (SocketMpLinkConfiguration.LegacyVgaStack == 0)){
	        // Show the string
	        vSimpleTextOutPpi->OutputString(00, SMC_MAX_ROW-1, 0x0F, str);
	        // Show the blink dot
	        vSimpleTextOutPpi->OutputString(dot, SMC_MAX_ROW-1, 0x8F, ".");
	    } else {
            vSimpleTextOutPpi->OutputString (00, SMC_MAX_ROW-2, 0x0F, "  iKVM doesn't support add-on VGA device.");
            vSimpleTextOutPpi->OutputString (00, SMC_MAX_ROW-1, 0x0F, "  Please change the D-SUB connector to Add-on VGA device...");
	    }
	}
#endif
#if IPMI_SUPPORT == 1
	if(HasPrintedBmcIp == 0){
	    LanIpStrLen = GetBmcIp(PeiServices, &(LanIpStr[0]));
	    if(LanIpStrLen > 0){ // If BMC IP exists.
		//
		// Clear the line
                //
		for(EmptyStrLen = 0; EmptyStrLen < 24; EmptyStrLen++)
		    EmptyStr[EmptyStrLen] = 32;

		EmptyStr[EmptyStrLen] = 0;
#if EarlyVideo_Mode
		vSimpleTextOutPpi->OutputString(SMC_MAX_COL - EmptyStrLen - 1, SMC_MAX_ROW-3, Attr, EmptyStr);
#else
		vSimpleTextOutPpi->OutputString(SMC_MAX_COL - EmptyStrLen - 1, SMC_MAX_ROW-3, 0x0F, EmptyStr);
#endif

                //
                // Print BMC IP
                //
#if EarlyVideo_Mode
		vSimpleTextOutPpi->OutputString (SMC_MAX_COL - LanIpStrLen - 1, SMC_MAX_ROW-3, Attr, LanIpStr);
#else
		vSimpleTextOutPpi->OutputString (SMC_MAX_COL - LanIpStrLen - 1, SMC_MAX_ROW-3, 0x0F, LanIpStr);
#endif
		HasPrintedBmcIp = 1;
	    }
	}
        #if BMC_IPV6_POST_SCREEN_SUPPORT
        //if(HasPrintedBmcIpv6 == 0)
        {
            LanIpStrLen = GetBmcIpv6(PeiServices, &(LanIpStr[0]));
            if(LanIpStrLen > 0){ // If BMC IPv6 exists.
                //
                // Clear the line
                //
                for(EmptyStrLen = 0; EmptyStrLen < IP_ADDR_STR_MAX_CHAR_LEN; EmptyStrLen++)
                    EmptyStr[EmptyStrLen] = ' ';
                EmptyStr[EmptyStrLen] = 0;
                
            #if EarlyVideo_Mode
                vSimpleTextOutPpi->OutputString(SMC_MAX_COL - EmptyStrLen - 1, SMC_MAX_ROW-2, Attr, EmptyStr);
            #else
                vSimpleTextOutPpi->OutputString(SMC_MAX_COL - EmptyStrLen - 1, SMC_MAX_ROW-2, 0x0F, EmptyStr);
            #endif

                //
                // Print BMC IPv6
                //
            #if EarlyVideo_Mode
                vSimpleTextOutPpi->OutputString (SMC_MAX_COL - LanIpStrLen - 1, SMC_MAX_ROW-2, Attr, LanIpStr);
            #else
                vSimpleTextOutPpi->OutputString (SMC_MAX_COL - LanIpStrLen - 1, SMC_MAX_ROW-2, 0x0F, LanIpStr);
            #endif
                //HasPrintedBmcIpv6 = 1;
            }
        }
        #endif
    #endif // #if IPMI_SUPPORT == 1
    }
#endif // #if EarlyVideo_SUPPORT == 1
    return Status;
}

#if EarlyVideo_SUPPORT == 1
EFI_STATUS
EFIAPI SmcPostMsgHotKey_PEI_Callback(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *Ppi)
{

	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_PEI_RSC_HANDLER_PPI* RscHandlerPpi;
	
	PEI_TRACE((-1, PeiServices, "SmcPostMsgHotKey_PEI_Callback\n"));
	Status = (*PeiServices)->LocatePpi (
		PeiServices,
		&gEfiPeiRscHandlerPpiGuid,
		0,
		NULL,
		(VOID **) &RscHandlerPpi );
	if (!EFI_ERROR (Status)) {
		PEI_TRACE((-1, PeiServices, "install worker1\n"));
		Status = RscHandlerPpi->Register (SmcPostMsgHotKeyPEIReportWorker);
	}

	return EFI_SUCCESS;
}
#endif

EFI_STATUS
SmcPostMsgHotKey_PEI_Init(
    IN	EFI_FFS_FILE_HEADER	*FfsHeader,
    IN	EFI_PEI_SERVICES	**PeiServices
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    SETUP_DATA	SetupData;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI	*ReadOnlyVariable = NULL;
    UINTN	VariableSize = sizeof(SETUP_DATA);
    EFI_GUID	mSetupGuid = SETUP_GUID;
    EFI_GUID	mSmcPostPrintHobGuid = EFI_SMCPOSTPRINT_HOB_GUID;
    SMCPOSTPRINT_HOB_INFO	*pSmcPostPrintHob;	
#if EarlyVideo_SUPPORT == 1
    EFI_PEI_NOTIFY_DESCRIPTOR* pCallback;

    PEI_TRACE((-1, PeiServices, "SmcPostMsgHotKey_PEI_Init Start\n"));	

    Status = (**PeiServices).AllocatePool(
		PeiServices,
		sizeof(EFI_PEI_NOTIFY_DESCRIPTOR),
		&pCallback);

    ASSERT_PEI_ERROR (PeiServices, Status);

    pCallback->Flags = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    pCallback->Guid   = &gEfiPeiSimpleTextOutPPIGuid;
    pCallback->Notify = SmcPostMsgHotKey_PEI_Callback;

    Status = (*PeiServices)->NotifyPpi(PeiServices, pCallback);
    ASSERT_PEI_ERROR (PeiServices, Status);

    PEI_TRACE((-1, PeiServices, "SmcPostMsgHotKey_PEI_Init End\n"));
#endif

    Status = (*PeiServices)->LocatePpi(
		PeiServices,
		&gEfiPeiReadOnlyVariable2PpiGuid,
		0,
		NULL,
		&ReadOnlyVariable);

    if(!EFI_ERROR(Status)){
	Status = ReadOnlyVariable->GetVariable(
			ReadOnlyVariable,
			L"Setup",
			&mSetupGuid,
			NULL,
			&VariableSize,
			&SetupData);

	if(!EFI_ERROR(Status)){
	    Status = (**PeiServices).CreateHob (
				PeiServices,
				EFI_HOB_TYPE_GUID_EXTENSION,
				sizeof(SMCPOSTPRINT_HOB_INFO),
				&pSmcPostPrintHob);
	    pSmcPostPrintHob->EfiHobGuidType.Name = mSmcPostPrintHobGuid;
	    pSmcPostPrintHob->SmcBootProcedureMsg = SetupData.SmcBootProcedureMsg_Support;
	}
    }

    return Status;
}
