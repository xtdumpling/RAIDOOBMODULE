//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//
//  Rev. 1.16
//    Bug Fixed: Remove Ctrl home feature.
//    Reason   : 
//    Auditor  : Chen Lin
//    Date     : Jul/17/2017
//
//  Rev. 1.15
//   Bug Fixed:  Clear MCE at end of POST.
//   Reason:     
//   Auditor:    Donald Han
//   Date:       Jul/08/2017
//
//  Rev. 1.14
//    Bug Fix : N/A
//    Reason  : Hidden "TXT Support" in BIOS Setup menu dynamically when CPU 
//              doesn't support it.
//    Auditor : Joe Jhang
//    Date    : Jun/26/2017
//
//  Rev. 1.13
//   Bug Fixed:  Add 24NVMe patches
//   Reason:     1. Recover 10 of 12 hidden NVMes behind PLX
//				 2. Assign MMIO for each empty NVMe slot for hot insertion in runtime
//				 3. Mask PLX error reporting, otherwise system may hang
//   Auditor:    Donald Han
//   Date:       May/16/2017
// 
//  Rev. 1.12
//   Bug Fixed:  Fixed system may occur IERR when injecting PCIE error.
//   Reason:     Remove workaround.
//   Auditor:    Chen Lin
//   Date:       May/08/2017
// 
//  Rev. 1.11
//    Bug Fixed: Fix system will have general exception.
//    Reason   : BIOS will install lots of SimpleTextIn driver in the POST. And Yenhsin's solution will register ctrl+home function for every SimpleTextIn driver.
//					Somehow BIOS maybe uninstall or replace the old SimpleTextIn driver, that will cause the 256 array has invalid driver pointer and cause CPU #GP.
//    Auditor  : Jacker Yeh
//    Date     : Apr/17/2017
//
//  Rev. 1.10
//    Bug Fixed: Pressing Ctrl+Home to enter BIOS recovery mode.
//    Reason   : 
//    Auditor  : Yenhsin Cho
//    Date     : Apr/07/2017
//
//  Rev. 1.09
//    Bug Fixed:  Move CPLD code to SmcNVDIMM module.
//    Reason:     
//    Auditor:    Leon Xu
//    Date:       Mar/16/2017
//
//  Rev. 1.08
//    Bug Fixed:  Fixed system will hang up and no log when inject SERR/PERR with H0 CPU 
//    Reason:     
//    Auditor:    Chen Lin
//    Date:       Mar/13/2017
//
//  Rev. 1.07
//    Bug Fixed:  Support Throttle on Power Fail.
//    Reason:     
//    Auditor:    Leon Xu
//    Date:       Mar/08/2017
//
//  Rev. 1.06
//    Bug Fixed:  Add NVDIMM/CPLD function
//    Reason:     
//    Auditor:    Leon Xu
//    Date:       Feb/17/2017
//
//  Rev. 1.05
//    Bug Fix:  Fixed ProjectReadyToBootCallBack can't run correctly.
//    Reason:   
//    Auditor:  Jimmy Chiu
//    Date:     Sep/26/2016
//
//  Rev. 1.04
//   Bug Fixed:  Fixe Memory Map out feature could not work. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Sep/26/2016
//
//  Rev. 1.03
//    Bug Fix:  Improved use BIOS setup menu item to enable/disable Win7 USB Keyboard/Mouse support.   
//    Reason:   
//    Auditor:  Sunny Yang (Refer Greenlow)
//    Date:     Sep/08/2016
//
//  Rev. 1.02
//    Bug Fix:  Add SMC_EXT_NMI_GPIO and SMC_BMC_NMI_GPIO support
//    Reason:   For GPI_NMI and BMC_NMI
//    Auditor:  Jimmy Chiu (Refer Greenlow)
//    Date:     Jun/13/2016
//
//  Rev. 1.01
//    Bug Fix : Add a SMC feature - JPME2_Jumpless_SUPPORT
//    Reason  : It is one of SMC features.
//    Auditor : Hartmann Hsieh
//    Date    : Jun/08/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/19/2014
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include <AmiDxeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <SmcLib.h>
#include "ProjectDxeDriver.h"
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>
#include <SspTokens.h>
#if JPME2_Jumpless_SUPPORT
#include <SmcLibCommon.h>
#endif // #if JPME2_Jumpless_SUPPORT
#include "Library/GpioLib.h"
#include "Setup.h"
#include <AmiCompatibilityPkg/Include/AcpiRes.h>
#include <Protocol/GenericElogProtocol.h>
#include <Library/PcdLib.h>
#include <Library/SetupLib.h>
#include <PciBus.h>
#include <Library/MmPciBaseLib.h>
#include "ProjectHideSetupItem.h"


//KEY_INFO_FOR_STIEX_REG  mKeyInfoForStiExReg[] = {
//    {{{0x0005, 0x0000}, {0x80000008, 0x82}}, StiExNotifyFuncForCtrlHome},
//    {{{0x0005, 0x0000}, {0x80000004, 0x82}}, StiExNotifyFuncForCtrlHome}
//};

STIEX_INFO_FOR_UNREG  mStiExInfoForUnreg[256];  // 256 is arbitrary and for safety.
UINTN                 unCountOfStiExUnreg = 0;

EFI_EVENT  mEventForStiExInstalled = NULL;

//EFI_STATUS
//StiExNotifyFuncForCtrlHome(
//    EFI_KEY_DATA        *pKeyData
//)
//{
//    DEBUG((-1, "StiExNotifyFuncForCtrlHome enter.\n"));
//
//    IoWrite8(0x72, CMOS_CTRL_HOME_FLAG);
//    IoWrite8(0x73, 0x55);
//    gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
//  
//    DEBUG((-1, "StiExNotifyFuncForCtrlHome end.\n"));
//    return EFI_SUCCESS;
//}

//VOID
//NotifyFuncForStiExInstalled(
//    EFI_EVENT   Event,
//    VOID        *Context
//)
//{
//    EFI_STATUS  Status;
//    UINTN       HandleArrayCount, unIndex0, unIndex1;
//    EFI_HANDLE  *HandleArray;
//    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *pStiEx;
//    VOID        *pNotifyHandle;
//    
//    DEBUG((-1, "NotifyFuncForStiExInstalled enter.\n"));
//  
//    Status = gBS->LocateHandleBuffer(
//                    ByProtocol,
//                    &gEfiSimpleTextInputExProtocolGuid,
//                    NULL,
//                    &HandleArrayCount,
//                    &HandleArray);
//
//    if(EFI_ERROR(Status))       return;
//  
//    for(unIndex0 = 0; unIndex0 < HandleArrayCount; unIndex0++){
//        Status = gBS->HandleProtocol (
//                        HandleArray[unIndex0],
//                        &gEfiSimpleTextInputExProtocolGuid,
//                        (VOID**)&pStiEx);
//
//        if(EFI_ERROR(Status))   continue;
//
//        for(unIndex1 = 0; unIndex1 < unCountOfStiExUnreg; unIndex1++){
//            if (mStiExInfoForUnreg[unIndex1].pStiEx == pStiEx) break;
//        }
//
//        if(unIndex1 < unCountOfStiExUnreg)      continue;
//        for(unIndex1 = 0; unIndex1 < sizeof(mKeyInfoForStiExReg)/sizeof(KEY_INFO_FOR_STIEX_REG); unIndex1++){
//            Status = pStiEx->RegisterKeyNotify(
//                            pStiEx,
//                            &mKeyInfoForStiExReg[unIndex1].KeyData,
//                            mKeyInfoForStiExReg[unIndex1].NotifyFunc,
//                            &pNotifyHandle);
//
//            if(EFI_ERROR(Status))       continue;
//
//            if(unCountOfStiExUnreg >= sizeof(mStiExInfoForUnreg)/sizeof(STIEX_INFO_FOR_UNREG))
//                continue;
//
//            mStiExInfoForUnreg[unCountOfStiExUnreg].pStiEx        = pStiEx;
//            mStiExInfoForUnreg[unCountOfStiExUnreg].pNotifyHandle = pNotifyHandle;
//            unCountOfStiExUnreg ++;
//        }
//    }
//  
//    gBS->FreePool (HandleArray);
//    DEBUG((-1, "NotifyFuncForStiExInstalled end.\n"));
//}

VOID
PostEndCallbackForStiExUnreg(
    EFI_EVENT   Event,
    VOID        *Context
)
{
#if 1	// Rev. 1.11 +
    UINTN       i, j;
	EFI_STATUS  Status;
    UINTN       HandleArrayCount;
    EFI_HANDLE  *HandleArray;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *pStiEx;
#else
    UINTN       unIndex;
#endif
    
    DEBUG((-1, "PostEndCallbackForStiExUnreg enter.\n"));
  
    gBS->CloseEvent(Event);
    gBS->CloseEvent(mEventForStiExInstalled);


#if 1	// Rev. 1.11+
	Status = gBS->LocateHandleBuffer(
                    ByProtocol,
                    &gEfiSimpleTextInputExProtocolGuid,
                    NULL,
                    &HandleArrayCount,
                    &HandleArray);

    if(EFI_ERROR(Status)) {
		DEBUG((-1, "fail to get simple text input.\n"));
    }
	else {
		DEBUG((-1, "HandleArrayCount=%d.\n", HandleArrayCount));
		for(i = 0; i < HandleArrayCount; i++){
			Status = gBS->HandleProtocol (
						HandleArray[i],
						&gEfiSimpleTextInputExProtocolGuid,
						(VOID**)&pStiEx);

			DEBUG((-1, "[i=%d] pStiEx=0x%x\n", i, pStiEx));

        	if(EFI_ERROR(Status))
				continue;

			for(j = 0; j < unCountOfStiExUnreg; j++){
				if ( mStiExInfoForUnreg[j].pStiEx == pStiEx ) {
					DEBUG((-1, "[j=%d]unregister pStiEx=0x%x, pNotifyHandle=0x%x\n", 
							i, 
							mStiExInfoForUnreg[j].pStiEx, 
							mStiExInfoForUnreg[j].pNotifyHandle));
					mStiExInfoForUnreg[j].pStiEx->UnregisterKeyNotify(
                        mStiExInfoForUnreg[j].pStiEx,
                        mStiExInfoForUnreg[j].pNotifyHandle);
				}
			}
		}
	}
#else
    for(unIndex = 0; unIndex < unCountOfStiExUnreg; unIndex++){
        mStiExInfoForUnreg[unIndex].pStiEx->UnregisterKeyNotify(
                        mStiExInfoForUnreg[unIndex].pStiEx,
                        mStiExInfoForUnreg[unIndex].pNotifyHandle);
    }
    unCountOfStiExUnreg = 0;
#endif

    DEBUG((-1, "PostEndCallbackForStiExUnreg end.\n"));
}

//VOID
//StiExRegisterKeyNotifyFunc(VOID)
//{
//    EFI_STATUS  Status;
//    VOID        *pRegistration;
//    EFI_EVENT   EventForTemp = NULL;
//    
//    DEBUG((-1, "StiExRegisterKeyNotifyFunc enter.\n"));
//  
//    NotifyFuncForStiExInstalled(NULL, NULL);
//  
//    Status = gBS->CreateEvent(
//                    EVT_NOTIFY_SIGNAL,
//                    TPL_CALLBACK,
//                    NotifyFuncForStiExInstalled,
//                    NULL,
//                    &mEventForStiExInstalled);
//
//    if(EFI_ERROR(Status))       return;
//  
//    Status = gBS->RegisterProtocolNotify(
//                    &gEfiSimpleTextInputExProtocolGuid,
//                    mEventForStiExInstalled,
//                    &pRegistration);
//
//    if(EFI_ERROR(Status))       return;
//  
//    Status = CreateReadyToBootEvent(
//                    TPL_CALLBACK,
//                    PostEndCallbackForStiExUnreg,
//                    NULL,
//                    &EventForTemp);
//
//    if(EFI_ERROR(Status))       return;
//    DEBUG((-1, "StiExRegisterKeyNotifyFunc end.\n"));
//}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   ProjectReadyToBootCallBack
//
// Description: Run at ready to boot.
//
// Input:       IN EFI_EVENT   Event
//              IN VOID        *Context
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
ProjectReadyToBootCallBack(
    IN  EFI_EVENT       Event,
    IN  VOID            *Context
)
{
    ACPI_HDR    *dsdt;
    EFI_PHYSICAL_ADDRESS        a;
    EFI_GUID    SetupGuid = SETUP_GUID;
    SETUP_DATA  SetupData;
    UINTN       VarSize = sizeof(SETUP_DATA);
    EFI_STATUS  Status;
    
#if JPME2_Jumpless_SUPPORT
    UINT8 reg = 0;
#endif // #if JPME2_Jumpless_SUPPORT

#if CLEAR_POST_MCE
    UINT8	MceNo, index;
#endif //CLEAR_POST_MCE    
    
    Status = gRT->GetVariable(
                    L"Setup",
                    &SetupGuid,
                    NULL,
                    &VarSize,
                    &SetupData);   

    DEBUG((-1, "GetVariable from  Get DSDT - returned %r\n", Status));

    if(SetupData.SmcW7USB == 1){
        //Get DSDT.. we have to update it.
        Status=LibGetDsdt(&a,EFI_ACPI_TABLE_VERSION_ALL);
        if(EFI_ERROR(Status)){
            DEBUG((-1, "PciRB: Fail to Get DSDT - returned %r\n", Status));
            ASSERT_EFI_ERROR(Status);
        } else {
            dsdt=(ACPI_HDR*)a;
            Status = UpdateAslNameObject( dsdt, (UINT8*)"W7KF", 1);
            DEBUG((-1, "W7KF/UpdateAslNameObject- returned %r\n", Status));	    
            dsdt->Checksum = 0;
            dsdt->Checksum = ChsumTbl((UINT8*)dsdt, dsdt->Length);
        }
    }
    gBS->CloseEvent(Event);

#if JPME2_Jumpless_SUPPORT
    reg = GetCmosTokensValue (Q_MEUD);
    if(reg == 0x5a){
        reg = GetCmosTokensValue (Q_MEUD_Reboot_CHK);
        reg = reg | 0x10;
        SetCmosTokensValue(Q_MEUD_Reboot_CHK, reg);
    }
#endif // #if JPME2_Jumpless_SUPPORT
    
#if CLEAR_POST_MCE
    MceNo = (UINT8) AsmReadMsr64 (0x179);
    for (index = 0; index < MceNo; index++) AsmWriteMsr64(0x401+index*4, 0); // Clear all MCE which occurs during POST
#endif     
}

//<SMC_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : ClearMapOut
// Description : Clear the cmos and variable of map out.
//
// Parameters  : None
//
// Returns     : None
//
//----------------------------------------------------------------------------
//<SMC_PHDR_START>
VOID
Check_MemMapOut(
  VOID 
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       MemMapOutDIMM = 0;
    UINT32      AttributesRead, MapOutRTFlag = 0;
    UINTN       VarSize = sizeof(UINT32);

    DEBUG((-1, "ClearMapOut ent\n"));
    //Clear MemMapout variable if no valid DIMM location
    Status = gRT->GetVariable(
                    L"MemMapout",
                    &gSmcMemMapOutGuid,
                    &AttributesRead,
                    &VarSize,
                    &MapOutRTFlag);

    MemMapOutDIMM = GetCmosTokensValue (MEM_MAPOUT);    			 
    if(EFI_ERROR(Status) || ((MemMapOutDIMM & 0x80) == 00)){
    // Set default value 
        DEBUG((-1, "Zero MemMapOut var\n"));
        MapOutRTFlag = 0; 
        VarSize = sizeof(UINT32);
        Status = gRT->SetVariable(
                        L"MemMapout",
                        &gSmcMemMapOutGuid,
                        EFI_VARIABLE_NON_VOLATILE |
                        EFI_VARIABLE_BOOTSERVICE_ACCESS |
                        EFI_VARIABLE_RUNTIME_ACCESS,
                        VarSize,
                        &MapOutRTFlag);
    }
}

VOID
ProjectGPNVCallBack(
    IN  EFI_EVENT       Event,
    IN  VOID            *Context
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_SM_ELOG_PROTOCOL        *gGenericElogProtocol = NULL;
    UINT64      RecordId;
    ERROR_INFO  ErrorInfo;  
    BOOLEAN     EnableElog, ElogStatus;

    Status = gBS->LocateProtocol(
                    &gEfiGenericElogProtocolGuid,
                    NULL,
                    &gGenericElogProtocol);

    if(EFI_ERROR(Status))       return; 

    DEBUG((-1, "GPNVCallBack ent\n"));
    // DEBUG((EFI_D_INFO, "smcdbg\n")); 
    ErrorInfo.Header.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;
    ErrorInfo.Header.EventLogType = EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR;
    ErrorInfo.Msg.PcieMsg.ParityErr = 1; 
    ErrorInfo.HardwareId = NB_PCIE_ERROR; 
    ErrorInfo.Msg.PcieMsg.Bus = GetCmosTokensValue(PERR_WA_H);
    ErrorInfo.Msg.PcieMsg.Dev = (GetCmosTokensValue(PERR_WA_L) >> 3) & 0x1F;
    ErrorInfo.Msg.PcieMsg.Fun = GetCmosTokensValue(PERR_WA_L) & 0x7;
    DEBUG((-1, "PERR Bus=0x%x,Dev=0x%x,Fun=0x%x ent\n",ErrorInfo.Msg.PcieMsg.Bus,ErrorInfo.Msg.PcieMsg.Dev,ErrorInfo.Msg.PcieMsg.Fun));
    EnableElog = TRUE;
    Status = gGenericElogProtocol->ActivateEventLog(
                    gGenericElogProtocol,
                    EfiElogSmSMBIOS,
                    &EnableElog,
                    &ElogStatus);

    DEBUG((-1, "gGenElog Active Status=%r \n",Status));               

    Status = gGenericElogProtocol->SetEventLogData(
                    gGenericElogProtocol,
                    (UINT8*)&ErrorInfo,
                    EfiElogSmSMBIOS,
                    FALSE,
                    sizeof (ERROR_INFO),
                    &RecordId);  

    DEBUG((-1, "gGenElog Status=%r \n",Status)); 

    SetCmosTokensValue(PERR_WA_H, 0);
    SetCmosTokensValue(PERR_WA_L, 0);        
    gBS->CloseEvent(Event);
}

//<SMC_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : ProjectDxeDriverInit
//
// Description : Setting the power status functio
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<SMC_PHDR_START>
EFI_STATUS
ProjectDxeDriverInit(
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_EVENT   ProjectReadyToBootEvent;
//    VOID        *GPNVReg;
    EFI_EVENT   ProjectHideItemEvent;
    VOID        *ProjectHideItemReg;

    InitAmiLib(ImageHandle, SystemTable);

    DEBUG((-1, "Project Driver Init\n"));
    
//    StiExRegisterKeyNotifyFunc();

    Status = CreateReadyToBootEvent(
                    TPL_CALLBACK,
                    ProjectReadyToBootCallBack,
                    NULL,
                    &ProjectReadyToBootEvent);

//    if(GetCmosTokensValue(PERR_WA_H) != 0){
//        DEBUG((-1, "PERR WA for GPNV\n"));  
//        Status = gBS->CreateEvent(
//                        EVT_NOTIFY_SIGNAL,
//                        TPL_CALLBACK,
//                        ProjectGPNVCallBack,
//                        NULL,
//                        &GPNVEvent);
//      
//        DEBUG((-1, "L286 Status=%r\n",Status));  
//        if(!Status){
//            Status = gBS->RegisterProtocolNotify(
//                            &gBdsAllDriversConnectedProtocolGuid,
//                            GPNVEvent,
//                            &GPNVReg);
//
//            DEBUG((-1, "L292 Status=%r\n",Status));                             
//        }
//    } 
#if MemoryMapOut_SUPPORT
    Check_MemMapOut(); 
#endif

// NMI button setting
#if defined SMC_EXT_NMI_GPIO && SMC_EXT_NMI_GPIO != 0xFF
    Status = GpioSetPadGpiNmiEnConfig(SMC_EXT_NMI_GPIO, 1);
#endif
// BMC NMI setting
#if defined SMC_BMC_NMI_GPIO && SMC_BMC_NMI_GPIO != 0xFF
    Status = GpioSetPadGpiNmiEnConfig(SMC_BMC_NMI_GPIO, 1);
#endif

    Status = gBS->CreateEvent(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ProjectHideSetupItem,
                    NULL,
                    &ProjectHideItemEvent
                    );
   if (!EFI_ERROR(Status)){
      Status = gBS->RegisterProtocolNotify(
                      &gSetupModifyProtocolGuid,
                      ProjectHideItemEvent,
                      &ProjectHideItemReg
                      );
   }

    DEBUG((-1, "Exit Project Driver Init\n"));
    return Status;
}
