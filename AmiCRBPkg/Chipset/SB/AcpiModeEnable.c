//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//     Rev. 1.04
//       Bug Fix:       [Fixes] Fixed setup item "Install Windows 7 USB support' doesn't working.
//       Reason:        Correct the method to get setup variable during SMM mode
//       Auditor:       Jimmy Chiu
//       Date:          Jul/27/2017
//
//     Rev. 1.03        Enhance SMC debug card function code.
//       Bug Fix:
//       Reason:        None
//       Auditor:       Jacker Yeh
//       Date:          Jan/21/2017
//
//     Rev. 1.02        Improve debug card port 80 function.
//       Bug Fix:
//       Reason:        None
//       Auditor:       Jacker Yeh
//       Date:          Oct/27/2016
//
//     Rev. 1.01        Keep USB 60, 64 io trap SMI.
//       Bug Fix:
//       Reason:        for OS(e.g. win7)  without U3 driver can install OS with XHCI disabed.
//       Auditor:       Sunny Yang
//       Date:          Sep/07/2016
//
//     Rev. 1.00        Add always turbo mode function.
//       Bug Fix:
//       Reason:        Reference from Grantlley, control by TOKEN "MAX_PERFORMANCE_OPTION", default disabled.
//       Auditor:       Jimmy Chiu
//       Date:          May/16/2016
//
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name:  AcpiModeEnable.c
//
// Description: Provide functions to enable and disable ACPI mode
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <Token.h>
#include <AmiCspLib.h>
#include <Protocol\SmmSwDispatch2.h>
#include <Protocol\AcpiModeEnable.h>
#include <AcpiModeELinks.h>
#if defined REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB && REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <PchAccess.h>
#endif

#include <Library/PciLib.h>
#include <Include/Register/PchRegsLpc.h>
#include <Include/IchRegs.h>
#include <Include/SysRegs.h>
#include <Register/PchRegsPmc.h>

#if SMCPKG_SUPPORT
#include "Setup.h"
#endif
#if SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION
#include <Library/BaseLib.h>
#endif


#if SMCPKG_SUPPORT
// SMM Nvram Control Protocol Definitions
typedef EFI_STATUS (*SHOW_BOOT_TIME_VARIABLES)(BOOLEAN Show);
typedef struct{
    SHOW_BOOT_TIME_VARIABLES ShowBootTimeVariables;
} AMI_NVRAM_CONTROL_PROTOCOL;

AMI_NVRAM_CONTROL_PROTOCOL *gNvramControl = NULL;

UINT8 SMCI_Flag = 0;

AMI_NVRAM_CONTROL_PROTOCOL *LocateNvramControlSmmProtocol(VOID){
    static EFI_GUID gAmiNvramControlProtocolGuid = \
    { 0xf7ca7568, 0x5a09, 0x4d2c, { 0x8a, 0x9b, 0x75, 0x84, 0x68, 0x59, 0x2a, 0xe2 } };
    return GetSmstConfigurationTable(&gAmiNvramControlProtocolGuid);
}
#endif

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

ACPI_DISPATCH_LINK  *gAcpiEnDispatchHead = 0, *gAcpiEnDispatchTail = 0;
ACPI_DISPATCH_LINK  *gAcpiDisDispatchHead = 0, *gAcpiDisDispatchTail = 0;

UINT16 wPM1_SaveState;
UINT32 dGPE_SaveState;

// GUID Definition(s)
EFI_GUID gEfiAcpiEnDispatchProtocolGuid = EFI_ACPI_EN_DISPATCH_PROTOCOL_GUID;
EFI_GUID gEfiAcpiDisDispatchProtocolGuid = EFI_ACPI_DIS_DISPATCH_PROTOCOL_GUID;

extern INIT_FUNCTION ACPIMODE_ENABLE_LIST EndOfAcpiModeEnList;
INIT_FUNCTION* AcpiModeEnList[]= {ACPIMODE_ENABLE_LIST NULL};

extern INIT_FUNCTION ACPIMODE_DISABLE_LIST EndOfAcpiModeDisList;
INIT_FUNCTION* AcpiModeDisList[]= {ACPIMODE_ENABLE_LIST NULL};

#if defined REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB && REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB
EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL *gPeriodicTimerDispatch = NULL;
EFI_HANDLE  gSwSmiTimerHandle = NULL;
#endif

#if SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION   //For always turbo feature
#define MSR_IA32_PERF_STS                     0x198
#define MSR_IA32_PERF_CTL                     0x199
#define P_STATE_TARGET_OFFSET                 8        
#define P_STATE_TARGET_MASK                   (0xFF << 8)

void SetFixedCpuFreq(UINT8 *PerfControlMsrRatioValue)
{
   UINT64 mPerfCtrlMSR;

   mPerfCtrlMSR = AsmReadMsr64(MSR_IA32_PERF_CTL);
   mPerfCtrlMSR &=~P_STATE_TARGET_MASK;
   mPerfCtrlMSR |= (   (*PerfControlMsrRatioValue) << P_STATE_TARGET_OFFSET);   
  
   AsmWriteMsr64(MSR_IA32_PERF_CTL,mPerfCtrlMSR );
}
#endif //SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION

//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   EnableAcpiMode
//
// Description: This function enable ACPI mode by clearing all SMI and
//              enabling SCI generation
//              This routine is also called on a S3 resume for special ACPI
//              programming.
//              Status should not be cleared on S3 resume. Enables are
//              already taken care of.
//
// Input:  DispatchHandle - Handle to the Dispatcher
//              DispatchContext - SW SMM dispatcher context
//
// Output:     None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS EnableAcpiMode(
	IN EFI_HANDLE  DispatchHandle,
	IN CONST VOID  *Context         OPTIONAL,
	IN OUT VOID    *CommBuffer      OPTIONAL,
	IN OUT UINTN   *CommBufferSize  OPTIONAL)
{
    ACPI_DISPATCH_LINK      *Link;
    UINTN i = 0;
#if SMCPKG_SUPPORT
    EFI_GUID	SetupGuid = SETUP_GUID;
    SETUP_DATA	SetupData;
    UINTN	VarSize = sizeof(SETUP_DATA);
    EFI_STATUS                Status;
    UINT8       SmcW7USB;

    gNvramControl = LocateNvramControlSmmProtocol();  
    if( gNvramControl ) gNvramControl->ShowBootTimeVariables( TRUE );   
    
    Status = pRS->GetVariable(
		    L"Setup",
		    &SetupGuid,
		    NULL,
		    &VarSize,
		    &SetupData);   
    
    SmcW7USB = SetupData.SmcW7USB;
    
    if( gNvramControl ) gNvramControl->ShowBootTimeVariables( FALSE );
    
    //DEBUG((-1, "SetupData.SmcW7USB : %x\n", SetupData.SmcW7USB));
#endif    

#if defined EMUL6064_SUPPORT && EMUL6064_SUPPORT == 1
#if SMCPKG_SUPPORT
   if(SmcW7USB == 0)  
#endif    
  if ( PciRead8( PCI_LIB_ADDRESS( LPC_BUS, LPC_DEVICE, LPC_FUNC, R_PCH_LPC_ULKMC)) )
	  PciWrite8(PCI_LIB_ADDRESS( LPC_BUS, LPC_DEVICE, LPC_FUNC, R_PCH_LPC_ULKMC ), 0);
#endif

    //Disable SMI Sources : SW SMI Timer
     IoAnd8(PM_BASE_ADDRESS + R_PCH_SMI_EN, ~(BIT06));
     
    // And enable SMI on write to SLP_EN when SLP_TYP is written
     IoOr8(PM_BASE_ADDRESS + R_PCH_SMI_EN, BIT04);     

    //Disable and Clear PM1 Sources except power button   
    wPM1_SaveState = IoRead16( PM_BASE_ADDRESS + R_PCH_ACPI_PM1_EN); //PM1_EN 
    IoOr16(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_EN, BIT08);
    IoWrite16(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, 0xffff);

    //Disable and Clear GPE0 Sources
    dGPE_SaveState = IoRead16(PM_BASE_ADDRESS + R_ACPI_GPE0_EN); //GPE0_EN
    IoWrite16(PM_BASE_ADDRESS + R_ACPI_GPE0_EN, 0);
    IoWrite32(PM_BASE_ADDRESS + R_ACPI_GPE0_STS + 0, 0);
    IoWrite32(PM_BASE_ADDRESS + R_ACPI_GPE0_STS + 4, 0);

    //Set day of month alarm invalid - ACPI 1.0 section 4.7.2.4
    IoWrite8(CMOS_ADDR_PORT, 0x0d |0x80);         //RTC_REGD
    IoWrite8(CMOS_DATA_PORT, 0);

    //Enable Sci
    IoOr8(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, BIT00);

    for (i = 0; AcpiModeEnList[i] ; i++)
    	 AcpiModeEnList[i](DispatchHandle, Context);

    for (Link = gAcpiEnDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }

#if SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION   //For always turbo feature
    {
	EFI_STATUS Status;
	UINT8 PerfControlMsrRatioValue;
	UINT8 CoreThreadCount;

	//Always turbo mode : MSR 1FCh(MSR_POWER_CTL) bit25=1 and MSR A01h(ENERGY_PREF_BIAS_CONFIG) bit6:3=0
	if( ((UINT8)((AsmReadMsr64(0xA01) >> 3) & 0x0F) == 0x03) && ((UINT8)((AsmReadMsr64(0x1FC) >> 25) & 0x01) == 0x01) ) { 
	    
            //MSR 1FCh MSR_POWER_CTL, BIT25:PWR_PERF_TUNING_CFG_MODE, 0:OS controls ENERGY_PREF_BIAS, 1:BIOS controls ENERGY_PREF_BIAS
            //MSR A01h ENERGY_PREF_BIAS_CONFIG, [6:3] 0x0:Maximum Power, 0x7:Balanced Performance, 0x8:Balanced Power, 0xF:Power
            //MSR 1ADh TURBO_RATIO_LIMIT_RATIOS, RATIO_0(7:0) ~ RATIO_7(63:56)
            //MSR 1AEh TURBO_RATIO_LIMIT_CORES, NUMCORE_0(7:0) ~ NUMCORE_7(63:56)
            CoreThreadCount = (AsmReadMsr64(0x35) >> 16) & 0xFF;
		
            //Set CPU requency as maximum turbo ratio
            Status = EFI_UNSUPPORTED;

            for(i = 7; i >= 0; i--) {
                if( CoreThreadCount >= (UINT8)((AsmReadMsr64 (0x1AE) >> (i*8)) & 0xFF) ) {
                   PerfControlMsrRatioValue = (UINT8)((AsmReadMsr64 (0x1AD) >> (i*8)) & 0xFF);
                    Status = EFI_SUCCESS;
                    break;
                }
            }

            if(!EFI_ERROR(Status)) {
                for (i = 0; i < pSmst->NumberOfCpus; ++i) {
                    pSmst->SmmStartupThisAp(SetFixedCpuFreq, i, &PerfControlMsrRatioValue);
                }
                SetFixedCpuFreq( &PerfControlMsrRatioValue );
            }
        }
    }	
#endif //SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION
    
    IoWrite8(0x80, SW_SMI_ACPI_ENABLE);
#if SMCPKG_SUPPORT
    MmioWrite8(SMC_DEBUG_ADDRESS,SW_SMI_ACPI_ENABLE);//SMC Debug card support
#endif
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   DisableAcpiMode
//
// Description: This function disables ACPI mode by enabling SMI generation
//
// Input:  DispatchHandle - Handle to the Dispatcher
//              DispatchContext - SW SMM dispatcher context
//
// Output:     None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS DisableAcpiMode(
		IN EFI_HANDLE  DispatchHandle,
		IN CONST VOID  *Context         OPTIONAL,
		IN OUT VOID    *CommBuffer      OPTIONAL,
		IN OUT UINTN   *CommBufferSize  OPTIONAL)
{
    ACPI_DISPATCH_LINK *Link;
    UINTN i = 0;

    //TODO: What about SW SMI Timer?
    //Disable SW SMI Timer
//#### byteValue = IoRead8(PM_BASE_ADDRESS + 0x30);  //SMI_EN (SMI Control and Enable register.)
//#### byteValue |= 0x40;         //SWSMI_TMR_EN = 1;
//#### IoWrite8(PM_BASE_ADDRESS + 0x30, byteValue);

    //Clear PM Sources and set Enables
    IoWrite16(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, 0xffff);
    IoWrite16(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_EN, wPM1_SaveState);

    //Clear GPE0 Sources and set Enables
    IoWrite32(PM_BASE_ADDRESS +R_ACPI_GPE0_STS + 0, 0xffffffff);  //GPE0_STS
    IoWrite32(PM_BASE_ADDRESS +R_ACPI_GPE0_STS + 4, 0xffffffff);  //GPE0_STS
    IoWrite32(PM_BASE_ADDRESS +R_ACPI_GPE0_EN, dGPE_SaveState); //GPE0_EN


    //Disable SCI
    IoAnd8(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, ~(BIT00));

    for (i = 0; AcpiModeDisList[i] != NULL; i++)
    	 AcpiModeDisList[i](DispatchHandle, Context);

    for (Link = gAcpiDisDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }

    IoWrite8(0x80, SW_SMI_ACPI_DISABLE);
#if SMCPKG_SUPPORT
    MmioWrite8(SMC_DEBUG_ADDRESS,SW_SMI_ACPI_DISABLE);//SMC Debug card support
#endif
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AddLink
//
// Description: Create and add link to specified list.
//
// Input:  Size -
//              Head -
//              Tail -
//
// Output:     VOID Pointer
//
// Modified:
//
// Referrals:   SmmAllocatePool
//
// Notes:       Here is the control flow of this function:
//              1. Allocate Link in Smm Pool.
//              2. Add Link to end.
//              3. Return Link address.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID * AddLink(
    IN UINT32       Size,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    VOID *Link;

    if (pSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, &Link) != EFI_SUCCESS) return 0;

    ((GENERIC_LINK*)Link)->Link = 0;
    if (!*Head)
    {
        *Head = *Tail = Link;
    }
    else
    {
        ((GENERIC_LINK*)*Tail)->Link = Link;
        *Tail = Link;
    }

    return Link;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RemoveLink
//
// Description: Remove link from specified list.
//
// Input:  Handle - EFI Handle
//              Head -
//              Tail -
//
// Output:     BOOLEAN
//                  TRUE if link was removed. FALSE if link not in the list.
//
// Modified:
//
// Referrals:   SmmFreePool
//
// Notes:       Here is the control flow of this function:
//              1. Search link list for Link.
//              2. Remove link from list.
//              3. Free link.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN RemoveLink(
    IN EFI_HANDLE   Handle,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    GENERIC_LINK *PrevLink, *Link;

    PrevLink = *Head;

    // Is link first. Link address is the same as the Handle.
    if (((GENERIC_LINK*)*Head) == Handle)
    {
        if (PrevLink == *Tail) *Tail = 0; // If Tail = Head, then 0.
        *Head = PrevLink->Link;
        pSmst->SmmFreePool(PrevLink);
        return TRUE;
    }

    // Find Link.
    for (Link = PrevLink->Link; Link; PrevLink = Link, Link = Link->Link)
    {
        if (Link == Handle)     // Link address is the same as the Handle.
        {
            if (Link == *Tail) *Tail = PrevLink;
            PrevLink->Link = Link->Link;
            pSmst->SmmFreePool(Link);
            return TRUE;
        }
    }

    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   EfiAcpiEnRegister
//
// Description: Register a Link on ACPI enable SMI.
//
// Input:  This -
//              Function -
//              Context -
//
//
// Output:     Handle -
//              EFI_STATUS
//
// Modified:    gAcpiEnDispatchHead, gAcpiEnDispatchTail
//
// Referrals:   AddLink
//
// Notes:       Here is the control flow of this function:
//              1. Verify if Context if valid. If invalid,
//                 return EFI_INVALID_PARAMETER.
//              2. Allocate structure and add to link list.
//              3. Fill link.
//              4. Enable Smi Source.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS EfiAcpiEnRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      &gAcpiEnDispatchHead, \
                      &gAcpiEnDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   EfiAcpiEnUnregister
//
// Description: Unregister a Link on ACPI enable SMI.
//
// Input:  This -
//              Handle -
//
// Output:     EFI_STATUS
//
// Modified:    gAcpiEnDispatchHead, gAcpiEnDispatchTail
//
// Referrals:   RemoveLink
//
// Notes:       Here is the control flow of this function:
//              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
//              2. Disable SMI Source if no other handlers using source.
//              3. Return EFI_SUCCESS.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS EfiAcpiEnUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, &gAcpiEnDispatchHead, &gAcpiEnDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   EfiAcpiDisRegister
//
// Description: Register a Link on ACPI disable SMI.
//
// Input:  This -
//              Function -
//              *Context -
//
//
// Output:     Handle - EFI Handle
//              EFI_STATUS
//
// Modified:    gAcpiDisDispatchHead, gAcpiDisDispatchTail
//
// Referrals:   AddLink
//
// Notes:       Here is the control flow of this function:
//              1. Verify if Context if valid. If invalid,
//                 return EFI_INVALID_PARAMETER.
//              2. Allocate structure and add to link list.
//              3. Fill link.
//              4. Enable Smi Source.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS EfiAcpiDisRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      &gAcpiDisDispatchHead, \
                      &gAcpiDisDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   EfiAcpiDisUnregister
//
// Description: Unregister a Link on ACPI Disable SMI.
//
// Input:  This -
//              Handle - EFI Handle
//
// Output:     EFI_STATUS
//
// Modified:    gAcpiDisDispatchHead, gAcpiDisDispatchTail
//
// Referrals:   RemoveLink
//
// Notes:       Here is the control flow of this function:
//              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
//              2. Disable SMI Source if no other handlers using source.
//              3. Return EFI_SUCCESS.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS EfiAcpiDisUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, &gAcpiDisDispatchHead, &gAcpiDisDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiEnDispatchProtocol = \
        {EfiAcpiEnRegister, EfiAcpiEnUnregister};

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiDisDispatchProtocol = \
        {EfiAcpiDisRegister, EfiAcpiDisUnregister};

//---------------------------------------------------------------------------
//
// Name:        CrbDummySwSmiTimerHandler
//
// Description:
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
CrbDummySwSmiTimerHandler (
	EFI_HANDLE  DispatchHandle,
	CONST VOID  *Context,
	VOID    	*CommBuffer,
	UINTN   	*CommBufferSize
)
{
    return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AcpiModeEnableInit
//
// Description: installs appropriate ACPI enable/disable Dispatch Protocol.
//
// Input:  ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:     EFI_STATUS
//
// Modified:
//
// Referrals:   InitAmiLib, InitSmmHandler
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AcpiModeEnableInit(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable)
{
    EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch2;
    EFI_SMM_SW_REGISTER_CONTEXT AcpiEnableContext = {SW_SMI_ACPI_ENABLE};
    EFI_SMM_SW_REGISTER_CONTEXT AcpiDisableContext = {SW_SMI_ACPI_DISABLE};
#if defined REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB && REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB
    EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT  TimerContext;
    BOOLEAN       SwSmiTmrEnabled = FALSE;
    UINT8         SmiEnBits = 0;
#endif
    
    EFI_STATUS  Status;
    EFI_HANDLE  Handle;
    EFI_HANDLE  DummyHandle = NULL;

    InitAmiSmmLib(ImageHandle, SystemTable);

    Status = pSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch2);

    if (EFI_ERROR(Status)) return Status;

    Status = SwDispatch2->Register(SwDispatch2,
                                  EnableAcpiMode, 
                                  &AcpiEnableContext, 
                                  &Handle);
    if (EFI_ERROR(Status)) return Status;

    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiEnDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiEnDispatchProtocol
                      );
    if (EFI_ERROR(Status)) return Status;
    Status = SwDispatch2->Register(SwDispatch2,
                                  DisableAcpiMode, 
                                  &AcpiDisableContext, 
                                  &Handle);

    if (EFI_ERROR(Status)) return Status;

    DummyHandle = NULL;
    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiDisDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiDisDispatchProtocol
                      );
    if (EFI_ERROR(Status)) return Status;

  //Aptiov workstation
#if defined REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB && REGISTER_DUMMY_SWSMI_TIMER_FOR_CRB
    SmiEnBits = IoRead8(PM_BASE_ADDRESS + R_PCH_SMI_EN);
    if (SmiEnBits & B_PCH_SMI_EN_SWSMI_TMR)
      SwSmiTmrEnabled = TRUE;
	
    Status = pSmst->SmmLocateProtocol (
	   	    &gEfiSmmPeriodicTimerDispatch2ProtocolGuid, 
		    NULL, 
		    &gPeriodicTimerDispatch);

    if (!EFI_ERROR(Status)) {
		TimerContext.Period = 160000;	//16ms 
		TimerContext.SmiTickInterval = 160000; 

		Status = gPeriodicTimerDispatch->Register (
						gPeriodicTimerDispatch, 
						CrbDummySwSmiTimerHandler,
						&TimerContext,
						&gSwSmiTimerHandle);
    }
    if (!SwSmiTmrEnabled) {
      IoWrite8(PM_BASE_ADDRESS + R_PCH_SMI_EN, (SmiEnBits &= ~B_PCH_SMI_EN_SWSMI_TMR));
    }

#endif
   //Aptiov workstation
   
    return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
