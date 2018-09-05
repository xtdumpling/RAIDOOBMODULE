//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.14
//      Bug Fixed:  Fix OPROM control no function when x8 AOC card in x16 lane reverse slot.
//      Reason:     Can't get bus number
//      Auditor:    Kasber Chen
//      Date:       Mar/08/2017
//
//  Rev. 1.13
//      Bug Fixed:  Fix onboard NVMe port OPROM control fail.
//      Reason: 
//      Auditor:    Kasber Chen
//      Date:       Jan/17/2017
//
//  Rev. 1.12
//      Bug Fixed:  Support Legacy NVMe and controlled by item.
//      Reason: 
//      Auditor:    Jimmy Chiu (Refer Grantley - Kasber Chen)
//      Date:       Jan/12/2017
//
//  Rev. 1.11
//      Bug Fixed:  Correct Purley IIO rule.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Nov/24/2016
//
//
//  Rev. 1.10
//      Bug Fixed:  Fix NVMe OPROM items no function.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Nov/21/2016
//
//  Rev. 1.09
//      Bug Fixed:  Fix OPROM item initial fail.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Nov/14/2016
//
//  Rev. 1.08
//      Bug Fixed:  Fixed UEFI RAID driver not been load problem.
//      Reason:     SmcUefiOpRomCtrl is called by CSM_CHECK_UEFI_OPROM_POLICY which is more early than SmcOpRomCtrl_Entry
//      Auditor:    Jacker Yeh
//      Date:       Nov/12/2016
//
//  Rev. 1.07
//      Bug Fixed:  Support 12 NVMe
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/29/2016
//
//  Rev. 1.06
//      Bug Fixed:  Support extern ROM of lan chip. ex: X722, X710
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/14/2016
//
//  Rev. 1.05
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//  Rev. 1.04
//    Bug Fix:  Fix 10G GBE can't control by item.
//    Reason:   There is a bridge in device.
//    Auditor:  Kasber Chen
//    Date:     Aug/01/2016
//
//  Rev. 1.03
//    Bug Fix:  Enable SATA RAID OPROM control.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/22/2016
//
//  Rev. 1.02
//    Bug Fix:  Support 6 NVMe.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/14/2016
//
//  Rev. 1.01
//    Bug Fix:  Update module and support EFI driver control when CSM disabled.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Apr/01/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/09/2014
//
//****************************************************************************
//****************************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MmPciBaseLib.h>
#include <Protocol/PciIo.h>
#include <PciBus.h>
#include "token.h"
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/AmiCsmOpromPolicy.h>
#include <Protocol/AmiSmbios.h>
#include "Setup.h"
#include <Include/Guid/PchRcVariable.h>
#include "SmcLib.h"
#include "SmcCspLib.h"
#include "SmcOpRomCtrl.h"
#if ROCKET_RAID_WORKAROUND
#include <PCI.h>
#endif

extern	EFI_GUID	gOpromStartEndProtocolGuid;

VOID	*SmcOpRomReg;
SETUP_DATA	SetupData;
EFI_GUID	SetupGuid = SETUP_GUID;
PCH_RC_CONFIGURATION      PchRcConfiguration;

UINT8	SmcOnBoardLenEnable_Array[4];
UINT16	OnBoardLenBDF_Array[4];
UINTN	LanPcdNumTbl[] = {
		PcdToken(PcdSmcOBLan1BDF), PcdToken(PcdSmcOBLan2BDF), PcdToken(PcdSmcOBLan3BDF),
		PcdToken(PcdSmcOBLan4BDF)};

#define Max_Slot_Number	11
#define Max_NVMe_Number	12
UINT8	SmcSlotBifurcation_Array[30];
UINT8   SmcSlotOpromEnable_Array[30];
UINT16  SlotNumberBDF_Array[30];
UINTN	SlotPcdNumTbl[] = {
		PcdToken(PcdSmcSLOT1BDF), PcdToken(PcdSmcSLOT2BDF), PcdToken(PcdSmcSLOT3BDF),
		PcdToken(PcdSmcSLOT4BDF), PcdToken(PcdSmcSLOT5BDF), PcdToken(PcdSmcSLOT6BDF),
		PcdToken(PcdSmcSLOT7BDF), PcdToken(PcdSmcSLOT8BDF), PcdToken(PcdSmcSLOT9BDF),
		PcdToken(PcdSmcSLOT10BDF), PcdToken(PcdSmcSLOT11BDF), PcdToken(PcdSmcOBNVMe1BDF),
		PcdToken(PcdSmcOBNVMe2BDF), PcdToken(PcdSmcOBNVMe3BDF),	PcdToken(PcdSmcOBNVMe4BDF),
		PcdToken(PcdSmcOBNVMe5BDF), PcdToken(PcdSmcOBNVMe6BDF),	PcdToken(PcdSmcOBNVMe7BDF),
		PcdToken(PcdSmcOBNVMe8BDF), PcdToken(PcdSmcOBNVMe9BDF),	PcdToken(PcdSmcOBNVMe10BDF),
		PcdToken(PcdSmcOBNVMe11BDF), PcdToken(PcdSmcOBNVMe12BDF), PcdToken(PcdSmcOBSASBDF)};

UINT8	PciDevNumList[] = {PCI_DEV_LIST};

BOOLEAN	initialed = FALSE;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SmcCsmDisCheckUefiOpRomPolicy
//
// Description: SuperMicro UefiOpRomPolicy hook when CSM is disabled.
//
// Input:       AMI_OPROM_POLICY_PROTOCOL  *This      - Protocol instance
//              UINT8                      PciClass   - PCI device class code
//
// Output:      EFI_STATUS
//              EFI_SUCCESS     - The EfiOpRom image should be dispatched.
//              EFI_NOT_FOUND   - The EfiOpRom image should be dispatched.
//              EFI_UNSUPPORTED - Skip the EfiOpRom.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SmcCheckUefiOpRomPolicy(
    IN  AMI_OPROM_POLICY_PROTOCOL       *This,
    IN  UINT8                           PciClass
)
{
    return EFI_SUCCESS;
}

EFI_STATUS
SmcCheckUefiOpRomPolicyByPciHandle(
    IN  AMI_CSM_OPROM_POLICY_PROTOCOL   *This,
    IN  EFI_HANDLE                      PciHandle
)
{
    EFI_STATUS Status;

    Status = SmcUefiOpRomCtrl(PciHandle);
    if(Status == EFI_UNSUPPORTED)
        return  EFI_UNSUPPORTED;

    return      EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   DummyProcessOpRom
//
// Description: An empty function
//
// Input:       None      
//
// Output:      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID Dummy1ProcessOpRom(
    IN  OPTIONAL        CSM_PLATFORM_POLICY_DATA        *OpRomStartEndProtocol
){}

VOID Dummy2ProcessOpRom(
    IN  OPTIONAL        AMI_CSM_PLATFORM_POLICY_DATA    *OpRomStartEndProtocol
){}

AMI_OPROM_POLICY_PROTOCOL       SmcOpRomPolicyProtocol = {
    SmcCheckUefiOpRomPolicy,
    Dummy1ProcessOpRom
};

AMI_CSM_OPROM_POLICY_PROTOCOL   SmcOpRomPolicyByPciHandleProtocol = {
    SmcCheckUefiOpRomPolicyByPciHandle,
    Dummy2ProcessOpRom
};

VOID SmcOpRomInitBDF(void)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;
    UINT32	TempBDF;
    UINT8	*Structure, i, j;
    UINT16	StructureSize;
    UINTN       VarSize;
    EFI_HANDLE  Handle = NULL;
    
    if(initialed)
	return;

    DEBUG((-1, "SmcOpRomInitBDF entry.\n"));
    initialed = TRUE;
    
    gBS->SetMem(SmcOnBoardLenEnable_Array, sizeof(SmcOnBoardLenEnable_Array), 1);       //default Legacy
    gBS->SetMem(SmcSlotOpromEnable_Array, sizeof(SmcSlotOpromEnable_Array), 1); //default Legacy
    gBS->SetMem(SmcSlotBifurcation_Array, sizeof(SmcSlotBifurcation_Array), 1);
        
    VarSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable(
                    L"Setup",
                    &SetupGuid,
                    NULL,
                    &VarSize,
                    &SetupData);

    if(Status == EFI_SUCCESS){
        for(i = 0; i < sizeof(SmcOnBoardLenEnable_Array); i++)
            SmcOnBoardLenEnable_Array[i] = SetupData.SmcLanOpRom[i];

        for(i = 0; i < sizeof(SmcSlotOpromEnable_Array); i++){
            if(i < Max_Slot_Number)
                SmcSlotOpromEnable_Array[i] = SetupData.SmcPciSlotOpRom[i];
            else if(i < (Max_Slot_Number + Max_NVMe_Number))
                SmcSlotOpromEnable_Array[i] = SetupData.SmcNVMeOpRom[i - Max_Slot_Number];
            else
                SmcSlotOpromEnable_Array[i] = SetupData.SmcSASOpRom;
        }

        if(SetupData.OnboardLANOpRomType)
            gBS->SetMem(SmcOnBoardLenEnable_Array, sizeof(SmcOnBoardLenEnable_Array), 0xff);

        if(!SetupData.CsmSupport){      //CSM disable
            Status = gBS->InstallMultipleProtocolInterfaces(
                            &Handle,
                            &gAmiOpromPolicyProtocolGuid, 
                            &SmcOpRomPolicyProtocol,
                            &gAmiCsmOpromPolicyProtocolGuid, 
                            &SmcOpRomPolicyByPciHandleProtocol,
                            NULL);
        }
    }

    VarSize = sizeof(PCH_RC_CONFIGURATION);
    Status = gRT->GetVariable(
                    L"PchRcConfiguration",
                    &gEfiPchRcVariableGuid,
                    NULL,
                    &VarSize,
                    &PchRcConfiguration);

    for(i = 0; i < sizeof(LanPcdNumTbl)/sizeof(UINTN); i++)
	OnBoardLenBDF_Array[i] = SmcBDFTransfer(LibPcdGet32(LanPcdNumTbl[i]));

    for(i = 0; i < sizeof(SlotPcdNumTbl)/sizeof(UINTN); i++)
	SlotNumberBDF_Array[i] = SmcBDFTransfer(LibPcdGet32(SlotPcdNumTbl[i]));

    Status = gBS->LocateProtocol(
		    &gAmiSmbiosProtocolGuid,
		    NULL,
		    &SmbiosProtocol);

    for(i = 1; !EFI_ERROR(Status); i++){
	Status = SmbiosProtocol->SmbiosReadStrucByType(
				9,  // Type 9
				i,  // Instance
				&Structure,
				&StructureSize);

	if(!EFI_ERROR(Status)){
	    for(j = 0; (j < sizeof(SlotNumberBDF_Array)/sizeof(UINT16)); j++){
		if((SlotNumberBDF_Array[j] == 0x8888) || !SlotNumberBDF_Array[j])	continue;
		TempBDF = (UINT32)(((SMBIOS_SYSTEM_SLOTS_INFO*)Structure)->SegGroupNumber) << 16;
		TempBDF |= (UINT32)(((SMBIOS_SYSTEM_SLOTS_INFO*)Structure)->BusNumber) << 8;
		TempBDF |= ((SMBIOS_SYSTEM_SLOTS_INFO*)Structure)->DevFuncNumber;
		if(SlotNumberBDF_Array[j] == SmcBDFTransfer(TempBDF)){
		    if(((SMBIOS_SYSTEM_SLOTS_INFO*)Structure)->SlotDataBusWidth == 0x0B)	//X8
			SmcSlotBifurcation_Array[j] = 2;
		    if (((SMBIOS_SYSTEM_SLOTS_INFO*)Structure)->SlotDataBusWidth == 0x0D)	//X16
			SmcSlotBifurcation_Array[j] = 4;
		    break;
		}
	    }
	    gBS->FreePool(Structure);
	}
    }
    DEBUG((-1, "SmcOpRomInitBDF end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SmcOpRomCtrlCallback
//
// Description: sync IPMI After G3 setting with BIOS variable
//
// Input:       IN EFI_EVENT Event - Callback event
//              IN VOID *Context - pointer to calling context
//
// Output:      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SmcOpRomCtrlCallback(
    IN EFI_EVENT        Event,
    IN VOID             *Context
)
{
    EFI_STATUS  Status;
    EFI_HANDLE  Handle;
    UINTN       Size = sizeof(EFI_HANDLE);
    CSM_PLATFORM_POLICY_DATA    *OpRomStartEndProtocol;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINT8       TempDev, TempFun, TempBusMin, TempBusMax, i, j;
    UINTN       PciSeg, PciBus, PciDev, PciFun;
    UINT16      TempBDF;
    UINT32      DevType;
#if Multi_VGA == 1    
    UINT32      VGA_ID;
#endif

    DEBUG((-1, "SmcOpRomCtrlCallback entry.\n"));
    
    SmcOpRomInitBDF();

    Status = gBS->LocateHandle(ByRegisterNotify, NULL, SmcOpRomReg, &Size, &Handle);
    if(EFI_ERROR(Status))       return;
    
    Status = gBS->HandleProtocol(Handle, &gOpromStartEndProtocolGuid, &OpRomStartEndProtocol);
    if(EFI_ERROR(Status))       return;

    if(OpRomStartEndProtocol == NULL)   return;

    PciIo = OpRomStartEndProtocol->PciIo;
    
    Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
    DEBUG((-1,"Legacy ROM Device in Bus:%02x Dev:%02x Fun:%02x\n", PciBus, PciDev, PciFun));
#if Multi_VGA == 1

    VGA_ID = *(UINT32*)MmPciAddress(0, (UINT8)PciBus, (UINT8)PciDev, (UINT8)PciFun, 0);
    if((VGA_ID == ONBOARD_VGA_DID)||(VGA_ID == ONBOARD_IGD_DID)){
        if(SetupData.OnboardVideoOpRom == 0x01){
            OpRomStartEndProtocol->ExecuteThisRom = TRUE;
        } else {
            OpRomStartEndProtocol->ExecuteThisRom = FALSE;
        }
        return;
    }  
#else  
    // Check Onboard Video Legacy ROM enable/disable
    if(*(UINT32*)MmPciAddress(0, (UINT8)PciBus, (UINT8)PciDev, (UINT8)PciFun, 0) == ONBOARD_VGA_DID){
        if(SetupData.OnboardVideoOpRom == 0x01)
            OpRomStartEndProtocol->ExecuteThisRom = TRUE;
        else
            OpRomStartEndProtocol->ExecuteThisRom = FALSE;
        return;
    }
#endif     

    // Check SATA and sSATA Legacy ROM enable/disable
    if(PciBus == 0x0 && PciDev == 0x11 && PciFun == 0x05){
        DEBUG((-1,"sSATA RAID = %x\n",PchRcConfiguration.sSataRaidLoadEfiDriver));
       	if(PchRcConfiguration.sSataRaidLoadEfiDriver == 0x02)
       	    OpRomStartEndProtocol->ExecuteThisRom = TRUE;
       	else
       	    OpRomStartEndProtocol->ExecuteThisRom = FALSE;
       	return;
    } 
    if(PciBus == 0x0 && PciDev == 0x17 && PciFun == 0x00){
        DEBUG((-1,"SATA RAID = %x\n",PchRcConfiguration.SataRaidLoadEfiDriver));
       	if(PchRcConfiguration.SataRaidLoadEfiDriver == 0x02)
       	    OpRomStartEndProtocol->ExecuteThisRom = TRUE;
       	else
       	    OpRomStartEndProtocol->ExecuteThisRom = FALSE;
       	return;
    }

    // Check onboard LAN Legacy ROM enable/disable
    for(i = 0 ;i < (sizeof(OnBoardLenBDF_Array)/sizeof(UINT16)) ;i++){
        TempBDF = OnBoardLenBDF_Array[i];
        if((!TempBDF) || (TempBDF == 0x8888))
            continue;	// If setting BDF = 0x0000 will not do any setting
        else if(TempBDF != 0xFFFF){	// Not GBE LAN
            TempBusMin = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x19);
            TempBusMax = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x1A);
            TempFun = i;
//    	    if(i < 2)
                TempFun %= PcdGet8(PcdSmcMultiFuncLanChip1);
//    	    else
//    		TempFun %= PcdGet8(PcdSmcMultiFuncLanChip2);
            if((TempBusMin <= PciBus) && (TempBusMax >= PciBus) && (PciFun == TempFun)){
                if((SmcOnBoardLenEnable_Array[i] == 1) || (SmcOnBoardLenEnable_Array[i] == 2)
                        && (SmcOnBoardLenEnable_Array[i] != 0xff))
                    OpRomStartEndProtocol->ExecuteThisRom = TRUE;
                else
                    OpRomStartEndProtocol->ExecuteThisRom = FALSE;
                return;
            }
        }else if((PciBus == 00) && (PciDev == 25) && (PciFun == 6)){
            if((SmcOnBoardLenEnable_Array[i] == 1) || (SmcOnBoardLenEnable_Array[i] == 2)
                    && (SmcOnBoardLenEnable_Array[i] != 0xff))
                OpRomStartEndProtocol->ExecuteThisRom = TRUE;
            else
                OpRomStartEndProtocol->ExecuteThisRom = FALSE;
            return;
        }
     }

    // Check Slot Legacy ROM enable/disable
    for(i = 0; i < (sizeof(SlotNumberBDF_Array)/sizeof(UINT16)); i++){
        TempBDF = SlotNumberBDF_Array[i];
        if(TempBDF && (TempBDF != 0x8888)){
            TempBusMin = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x19);
            TempBusMax = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x1A);
            DevType = (*(UINT32*)MmPciAddress(0, (UINT8)PciBus, 0, 0, 0x08)) & 0xffffff00;
            for(j = 0; j < SmcSlotBifurcation_Array[i]; j++){
                if(TempBusMin == 0xff)
                    TempBusMin = *(UINT8*)MmPciAddress(0, TempBDF >> 8, ((TempBDF >> 3) & 0x1F) + j, TempBDF & 0x07, 0x19);
                if(*(UINT8*)MmPciAddress(0, TempBDF >> 8, ((TempBDF >> 3) & 0x1F) + j, TempBDF & 0x07, 0x1A) != 0xff)
                    TempBusMax = *(UINT8*)MmPciAddress(0, TempBDF >> 8, ((TempBDF >> 3) & 0x1F) + j, TempBDF & 0x07, 0x1A);
            }
            DEBUG((-1,"TempBusMax: %02x TempBusMin: %02x Enable: %02x\n", TempBusMax, TempBusMin, SmcSlotOpromEnable_Array[i]));
            if(i < (sizeof(PciDevNumList)/sizeof(UINT8)))
                TempDev = PciDevNumList[i];	// it is a pci device
            else
                TempDev = 0xff;			// it is a pcie device
            if((TempBusMin <= PciBus) && (TempBusMax >= PciBus) && ((TempDev == PciDev) || (TempDev == 0xff))){
                if((SmcSlotOpromEnable_Array[i] == 1) || (SmcSlotOpromEnable_Array[i] == 3)){
                    if(SetupData.NvmeFWSource && (DevType == 0x01080200))
                        OpRomStartEndProtocol->ExecuteThisRom = FALSE;
                    else
                        OpRomStartEndProtocol->ExecuteThisRom = TRUE;
                } else
                    OpRomStartEndProtocol->ExecuteThisRom = FALSE;
                return;
            }
        }
    }
    return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SmcUefiOpRomCtrl
//
// Description: SmcUefiOpRomCtrl runtime hook
//
// Input:       UINT8       PciClass    PCI Class Code Of the Device.
//
// Output:      EFI_STATUS
//              EFI_SUCCESS - The UefiOpRom image should be dispatched.
//              EFI_UNSUPPORTED - Skip the UefiOpRom
//              EFI_NOT_FOUND - Check if skip the UefiOpRom by AmiOpromPolicy
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SmcUefiOpRomCtrl(
    IN	EFI_HANDLE      PciHandle
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINT8       TempDev, TempFun, TempBusMin, TempBusMax, i, j;
    UINTN       PciSeg, PciBus, PciDev, PciFun;
    UINT16      TempBDF;
    UINT32      DevType;
#if Multi_VGA == 1    
    UINT32      VGA_ID;
#endif
    
    DEBUG((-1,"SmcUefiOpRomCtrl entry.\n"));

    SmcOpRomInitBDF();
    
    Status = gBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
    if(EFI_ERROR(Status))		// If get protocol fail, Agree run UEFI driver.
        return EFI_SUCCESS;
    
    Status = PciIo->GetLocation(PciIo, &PciSeg, &PciBus, &PciDev, &PciFun);
    DEBUG((-1,"UEFI Device in Bus:%02x Dev:%02x Fun:%02x\n", PciBus, PciDev, PciFun));

#if Multi_VGA == 1
    VGA_ID = *(UINT32*)MmPciAddress(0, (UINT8)PciBus, (UINT8)PciDev, (UINT8)PciFun, 0);
    if((VGA_ID == ONBOARD_VGA_DID)||(VGA_ID == ONBOARD_IGD_DID)){
        DEBUG((-1,"SetupData.OnboardVideoOpRom %x\n", SetupData.OnboardVideoOpRom));
        if(SetupData.OnboardVideoOpRom == 0x02)
            return EFI_SUCCESS;
        else
            return EFI_UNSUPPORTED;
    }    
#else        
    // Check Onboard Video EFI driver enable/disable
    if(*(UINT32*)MmPciAddress(0, (UINT8)PciBus, (UINT8)PciDev, (UINT8)PciFun, 0) == ONBOARD_VGA_DID){
        DEBUG((-1,"SetupData.OnboardVideoOpRom %x\n", SetupData.OnboardVideoOpRom));
        if(SetupData.OnboardVideoOpRom == 0x02)
            return EFI_SUCCESS;
        else
            return EFI_UNSUPPORTED;
    }
#endif    

    // Check SATA and sSATA EFI driver enable/disable
    if(PciBus == 0x0 && PciDev == 0x11 && PciFun == 0x05){
        if(!PchRcConfiguration.sSataInterfaceMode)
            return EFI_UNSUPPORTED;

        if(PchRcConfiguration.sSataRaidLoadEfiDriver == 0x01)
            return EFI_SUCCESS;
        else
            return EFI_UNSUPPORTED;
    }
    	
    if(PciBus == 0x0 && PciDev == 0x17 && PciFun == 0x00){
        if(!PchRcConfiguration.SataInterfaceMode)
            return EFI_UNSUPPORTED;

        if(PchRcConfiguration.SataRaidLoadEfiDriver == 0x01)
            return EFI_SUCCESS;
        else
            return EFI_UNSUPPORTED;
    }

    // Check onboard LAN UEFI enable/disable
    for(i = 0; i < (sizeof(OnBoardLenBDF_Array)/sizeof(UINT16)); i++){
        TempBDF = OnBoardLenBDF_Array[i];
        if((!TempBDF) || (TempBDF == 0x8888))
            continue;	// If setting BDF = 0x0000 will not do any setting
        else if(TempBDF != 0xFFFF){	// Not GBE LAN
            TempBusMin = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x19);
            TempBusMax = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x1A);
            TempFun = i;
//          if(i < 2)
                TempFun %= PcdGet8(PcdSmcMultiFuncLanChip1);
//    	    else
//              TempFun %= PcdGet8(PcdSmcMultiFuncLanChip2);
            if((TempBusMin <= PciBus) && (TempBusMax >= PciBus) && (PciFun == TempFun)){
                if((SmcOnBoardLenEnable_Array[i] == 3) || (SmcOnBoardLenEnable_Array[i] == 0xff))
                    return EFI_SUCCESS;
                else
                    return EFI_UNSUPPORTED;
            }
        }else if((PciBus == 00) && (PciDev == 25) && (PciFun == 6)){
            if((SmcOnBoardLenEnable_Array[i] == 3) || (SmcOnBoardLenEnable_Array[i] == 0xff))
                return EFI_SUCCESS;
            else
                return EFI_UNSUPPORTED;
        }
    }

    // Check Slot UEFI ROM Execute
    for(i = 0; i < (sizeof(SlotNumberBDF_Array)/sizeof(UINT16)); i++){	   
        TempBDF = SlotNumberBDF_Array[i];
        if(TempBDF && (TempBDF != 0x8888)){
            TempBusMin = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x19);
            TempBusMax = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x1A);
            DevType = (*(UINT32*)MmPciAddress(0, (UINT8)PciBus, 0, 0, 0x08)) & 0xffffff00;
            for(j = 0; j < SmcSlotBifurcation_Array[i]; j++){
                if(TempBusMin == 0xff)
                    TempBusMin = *(UINT8*)MmPciAddress(0, TempBDF >> 8, ((TempBDF >> 3) & 0x1F) + j, TempBDF & 0x07, 0x19);
                if(*(UINT8*)MmPciAddress(0, TempBDF >> 8, ((TempBDF >> 3) & 0x1F) + j, TempBDF & 0x07, 0x1A) != 0xff)
                    TempBusMax = *(UINT8*)MmPciAddress(0, TempBDF >> 8, ((TempBDF >> 3) & 0x1F) + j, TempBDF & 0x07, 0x1A);
            }
            DEBUG((-1,"TempBusMax: %02x TempBusMin: %02x Enable: %02x\n", TempBusMax, TempBusMin, SmcSlotOpromEnable_Array[i]));
            if(i < (sizeof(PciDevNumList)/sizeof(UINT8)))
                TempDev = PciDevNumList[i];
            else
                TempDev = 0xff;
            if((TempBusMin <= PciBus) && (TempBusMax >= PciBus) && ((TempDev == PciDev) || (TempDev == 0xff))){
                if(SmcSlotOpromEnable_Array[i] >= 2){
                    if(SetupData.NvmeFWSource && (DevType == 0x01080200))
                        return EFI_UNSUPPORTED;
                    else
                        return EFI_SUCCESS;
                } else
                    return EFI_UNSUPPORTED;
            }
        }
    }
    return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SmcOpRomCtrl_entry
//
// Description: SmcOpRomCtrl runtime callback entry point
//
// Input:       IN EFI_HANDLE ImageHandle - Image handle
//              IN EFI_SYSTEM_TABLE *SystemTable - pointer to system table
//
// Output:      EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SmcOpRomCtrl_Entry(
    IN EFI_HANDLE	ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS	Status;
    EFI_EVENT	Event;
    
    DEBUG((-1, "SmcOpRomCtrl_Entry.\n"));

    Status = gBS->CreateEvent(
			EFI_EVENT_NOTIFY_SIGNAL,
			TPL_CALLBACK,
			SmcOpRomCtrlCallback,
			NULL,
		    	&Event);

    if(!EFI_ERROR(Status)){
	Status = gBS->RegisterProtocolNotify(
			&gOpromStartEndProtocolGuid,
			Event,
			&SmcOpRomReg);
    }

    DEBUG((-1, "SmcOpRomCtrl_End.\n"));
    return EFI_SUCCESS;
}

#if ROCKET_RAID_WORKAROUND
struct{
    UINT16	VendorId;
    UINT16	DeviceId;
}AddonCardList[] = {Workaround_Addon_Card_List};

VOID
SmcRocketRaidWorkaround(
    VOID
)
{
    EFI_STATUS                Status;
    EFI_PCI_IO_PROTOCOL       *PciIo;
    EFI_HANDLE                *HandleBuffer;
    UINT16                    VendorId, DeviceId;
    UINTN                     HandleCount, HandleIndex, ListIndex;


    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL, &HandleCount, &HandleBuffer);
    if(EFI_ERROR (Status)){
        return;
    }
    for(HandleIndex = 0; HandleIndex<HandleCount; HandleIndex++){
        Status = gBS->HandleProtocol(HandleBuffer[HandleIndex], &gEfiPciIoProtocolGuid,&PciIo);     	
        if (EFI_ERROR(Status)){
            continue;
        }

        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, PCI_VID, 1, &VendorId);
        if(EFI_ERROR(Status)){
            continue;
        }
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, PCI_DID, 1, &DeviceId);
        if(EFI_ERROR(Status)){
            continue;
        }
        for(ListIndex = 0; (AddonCardList[ListIndex].VendorId != 0x00 && AddonCardList[ListIndex].DeviceId != 0x00); ListIndex++){
            if((DeviceId == AddonCardList[ListIndex].DeviceId && VendorId == AddonCardList[ListIndex].VendorId) ||
                (AddonCardList[ListIndex].DeviceId == 0xFFFF && VendorId == AddonCardList[ListIndex].VendorId)){
                gBS->ConnectController(HandleBuffer[HandleIndex], NULL, NULL, TRUE);
                break;
            }
        }
    }
    gBS->FreePool(HandleBuffer);
}
#endif
