//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
/** @file AmiPciExpressGen2Lib.c
    Library Class for AMI Pci Express GEN2 Support Functions.


**/
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/AmiPciBusLib.h>
#include <Library/AmiPciExpressGen3Lib.h>
#include <Library/AmiSriovLib.h>


//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

/**
    Checks if PCIe GEN 2 features supported by the Device

        
    @param Device pointer to device internal data.
    @retval 
        BOOLEAN : TRUE of FALSE 
**/
BOOLEAN Pcie2CheckPcie2Compatible(PCI_DEV_INFO  *Device){

    if( Device->PciExpress!=NULL &&
        Device->PciExpress->Pcie2!=NULL &&
        Device->DevSetup.Pcie1Disable == 0 &&
        Device->DevSetup.Pcie2Disable == 0 )return TRUE;
    else return FALSE;
}

/**
    This function will collect information about PCIE GEN2 Device
    and initialize PCIE2_DATA structure based on information collected.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2GetGen2Info(PCI_DEV_INFO *Device){
    EFI_STATUS          Status;
	PCI_CFG_ADDR		addr;
//-----------------------------
	addr.ADDR=Device->Address.ADDR;
    //PCI_TRACE((TRACE_PCI," PciE2: Device @ [B%X|D%X|F%X] is PCIE GEN2 Device:\n",
    //        addr.Addr.Bus,addr.Addr.Device,addr.Addr.Function));

    //Get PCI Express Device CAP2 - 32 bit
    addr.Addr.Register=Device->PciExpress->PcieOffs+PCIE_DEV_CAP2_OFFSET;
    Status=PciCfg32(Device->RbIo, addr, FALSE,&Device->PciExpress->Pcie2->DeviceCap2.DEV_CAP2);
	ASSERT_EFI_ERROR(Status);

    PCI_TRACE((TRACE_PCI," DEV_CAP2-> [R2=%X|MEEP=%X|EETP=%X|EFF=%X|R1=%X|TPH=%X|LTR=%X|NRP=%X|C128=%X|AtC64=%X|AtC32=%X|AtR=%X|Ari=%d|CTD=%X|CmTO=%X]\n",
        Device->PciExpress->Pcie2->DeviceCap2.Reserved2,
        Device->PciExpress->Pcie2->DeviceCap2.MaxEndEndPrefix,
        Device->PciExpress->Pcie2->DeviceCap2.EndEndTlpPrefix,
        Device->PciExpress->Pcie2->DeviceCap2.ExtFmtField,
        Device->PciExpress->Pcie2->DeviceCap2.Reserved1,
        Device->PciExpress->Pcie2->DeviceCap2.TphCompleter,
        Device->PciExpress->Pcie2->DeviceCap2.LtrMechanism,
        Device->PciExpress->Pcie2->DeviceCap2.NoRoPrPrPassing,
        Device->PciExpress->Pcie2->DeviceCap2.Cas128Completer,
        Device->PciExpress->Pcie2->DeviceCap2.AtomicOpCompl64,
        Device->PciExpress->Pcie2->DeviceCap2.AtomicOpCompl32,
        Device->PciExpress->Pcie2->DeviceCap2.AtomicOpRouting,
        Device->PciExpress->Pcie2->DeviceCap2.AriForwarding,
        Device->PciExpress->Pcie2->DeviceCap2.ComplToutDisable,
        Device->PciExpress->Pcie2->DeviceCap2.ComplToutRanges));

    //Get PCI Express Link CAP2 - 32 bit
    addr.Addr.Register=Device->PciExpress->PcieOffs+PCIE_LNK_CAP2_OFFSET;
    Status=PciCfg32(Device->RbIo, addr, FALSE,&Device->PciExpress->Pcie2->LinkCap2.LNK_CAP2);
	ASSERT_EFI_ERROR(Status);

    if(Device->PciExpress->PcieCap.SlotImpl){
        //Get PCI Express Link SLOT2 - 32 bit
        addr.Addr.Register=Device->PciExpress->PcieOffs+PCIE_SLT_CAP2_OFFSET;
        Status=PciCfg32(Device->RbIo, addr, FALSE,&Device->PciExpress->Pcie2->SlotCap2.SLT_CAP2);
	    ASSERT_EFI_ERROR(Status);
    }

    Device->PciExpress->Pcie2->Owner=Device;

    return Status;
}


/**
    This function will Enable ARI Forwarding in DownSream Port of
    the device passed if
    1.Device referenced is an ARI device;
    2.Parenting Bridge supports ARI Forwarding.
    3.ARI Firvarding Setup Question Set to "Enabled"

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_UNSUPPORTED         When Device or Parenting Bridge does not support ARI
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2EnableAri(PCI_DEV_INFO *Device, BOOLEAN EnableOption){
    EFI_STATUS          Status=EFI_SUCCESS;
    PCI_DEV_INFO        *brg;
	PCI_CFG_ADDR		addr;
	PCIE_DEV_CNT2_REG	dcnt2;
//-----------------------------------
	
	//Just in case if ARI can't be supported...
	Status=SriovAriCapableHierarchy(Device, FALSE);
	
    //Setup Don't have it enabled
    if(EnableOption == FALSE) {
        PCI_TRACE((TRACE_PCI," -> Setup=%d", EnableOption));//gPciSetupData->AriFwd));
        return EFI_UNSUPPORTED;
    }

    if(Device->Type != tPciDevice) {
        PCI_TRACE((TRACE_PCI," -> not tPciDevice Type"));
        return EFI_UNSUPPORTED;
    }

    if(!Pcie2CheckPcie2Compatible(Device)) {
        PCI_TRACE((TRACE_PCI," -> not PCIe Gen2 Device"));
        return EFI_UNSUPPORTED;
    }

    //Device don't have ARI Ext Cap
    if(Device->PciExpress->AriData==NULL){
        PCI_TRACE((TRACE_PCI," -> non-ARI Device"));
        return EFI_UNSUPPORTED;
    }
    //If we are here - device is an ARI Device. Now check if Parenting Bridge can do it...
    brg=Device->ParentBrg;
    //Check if Parenting Brg is Gen 2 device
    if(!Pcie2CheckPcie2Compatible(brg)){
        PCI_TRACE((TRACE_PCI," -> ParentBrg non-GEN2"));
        return EFI_UNSUPPORTED;
    }

    //Check if bridge can do ARI Forwarding
    if(brg->PciExpress->Pcie2->DeviceCap2.AriForwarding==0){
        PCI_TRACE((TRACE_PCI," -> ParentBrg.Cap.AriFwd=0"));
        return EFI_UNSUPPORTED;
    }

    //Now check if this bridge has ARI Forwarding Enabled allready
    if(brg->PciExpress->Pcie2->AriEnabled==TRUE){
        //Set Device ARI Falg
        Device->PciExpress->Pcie2->AriEnabled=brg->PciExpress->Pcie2->AriEnabled;
        PCI_TRACE((TRACE_PCI," -> Parent Bridge AriEnabled=1"));
        return EFI_SUCCESS;
    }

    //At that Point we must enable ARI - all conditions are checked!.
    //for SRIOV capable devices we need to set ARI Capable bit in SRIOV_CNT_REG
    Status=SriovAriCapableHierarchy(Device, TRUE);

	addr.ADDR=brg->Address.ADDR;
    addr.Addr.Register=brg->PciExpress->PcieOffs+PCIE_DEV_CNT2_OFFSET;
    Status=PciCfg16(brg->RbIo, addr, FALSE,&dcnt2.DEV_CNT2);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;


    dcnt2.AriForwarding=TRUE;
    PCI_TRACE((TRACE_PCI,"  ARI -> Enabling ARI for Parent Bridge!"));
    Status=PciCfg16(Device->RbIo, addr, TRUE,&dcnt2.DEV_CNT2);
    ASSERT_EFI_ERROR(Status);


    if(EFI_ERROR(Status)) return Status;

    Device->PciExpress->Pcie2->AriEnabled=(BOOLEAN)dcnt2.AriForwarding;
    brg->PciExpress->Pcie2->AriEnabled=(BOOLEAN)dcnt2.AriForwarding;
    return Status;
}

/**
    This function will Enable ARI Forwarding in DownSream Port of
    the device passed if
    1.Device referenced is an ARI device;
    2.Parenting Bridge supports ARI Forwarding.
    3.ARI Firvarding Setup Question Set to "Enabled"

        
    @param Device Pointer to PCI Device Private Data structure.
    @param MultiFunc Pointer to a Flag to modify if Device is MF Device
    @param AriEnabled Pointer to a Flag to modify if this function was able to ENABLE ARI.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2CheckAri(PCI_DEV_INFO *Device, BOOLEAN *MultiFunc, BOOLEAN *AriEnabled){
    EFI_STATUS  Status=EFI_SUCCESS;
    //BOOLEAN 	EnableOption;
//--------------------
    PCI_TRACE((TRACE_PCI," PciE2: Checking ARI @ [B%X|D%X|F%X]:",
            Device->Address.Addr.Bus,Device->Address.Addr.Device,Device->Address.Addr.Function));

    *AriEnabled=FALSE;
    //EnableOption=Device->PciExpress->Pcie2->Pcie2Setup.AriFwd;
    if(Device->PciExpress->Pcie2->Pcie2Setup.AriFwd != PCI_SETUP_DONT_TOUCH){
        Status=Pcie2EnableAri(Device, Device->PciExpress->Pcie2->Pcie2Setup.AriFwd);
        PCI_TRACE((TRACE_PCI," Status=%r\n",Status));
    
        if((!EFI_ERROR(Status)) &&
           (Device->PciExpress->AriData!=NULL) &&
           (Device->PciExpress->Pcie2->AriEnabled==TRUE))
        {
            //*MultiFunc=TRUE;
            *AriEnabled=TRUE;
        } else {
            if(Status==EFI_UNSUPPORTED) Status=EFI_SUCCESS;
        }
    } 
    //PCI_TRACE((TRACE_PCI," AriEn=%d %r\n\n",*AriEnabled, Status));

    return Status;
}


/**
    This function will Select values for Link Control2 register on
    both sides of the LINK based on Setup Settings and hardware capabilities.

        
    @param DnStreamPort Pointer to PCI Device Private Data of Downstream Port of the link.
    @param DnLnkCnt2 Pointer to the LNK_CNT2 Reg of the Downstream Port of the link.
    @param UpStreamPort Pointer to PCI Device Private Data of Upwnstream Port of the link.
    @param UpLnkCnt2 Pointer to the LNK_CNT2 Reg of the Downstream Port of the link.
    @param LinkHotResetRequired Flag to modify if Link will need HOT RESET after programming.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetLnkProperties(PCI_DEV_INFO           *DnStreamPort,
                                 PCIE_LNK_CNT2_REG      *DnLnkCnt2,
                                 PCI_DEV_INFO           *UpStreamPort,
                                 PCIE_LNK_CNT2_REG      *UpLnkCnt2,
                                 BOOLEAN                *LinkHotResetRequired,
                                 BOOLEAN                *LinkRetrainRequired
                                 )
{
    EFI_STATUS          Status;
    PCIE_LNK_STA_REG    dnls, upls; //link status reg content of up and down side of the link
	PCI_CFG_ADDR		addr;
    UINT16              ls, maxls, curls, lsstore;//link speed variables
    UINT8				LnkSpeedOption;
    BOOLEAN				HwAutoSpeedOption;

//------------------------------

    PCI_TRACE((TRACE_PCI,"PciE2: Pcie2SetLnkProperties() ENTRY:\n"));

    //Get DownStream Port Properties
    addr.ADDR=DnStreamPort->Address.ADDR;

    PCI_TRACE((TRACE_PCI," DN STREAM PORT -> [B%X|D%X|F%X] <--> [B%X|D%X|F%X] <- UP STREAM PORT\n",
            DnStreamPort->Address.Addr.Bus,DnStreamPort->Address.Addr.Device, DnStreamPort->Address.Addr.Function,
            UpStreamPort->Address.Addr.Bus,UpStreamPort->Address.Addr.Device, UpStreamPort->Address.Addr.Function));

    addr.Addr.Register=DnStreamPort->PciExpress->PcieOffs+PCIE_LNK_CNT2_OFFSET;
    Status=PciCfg16(DnStreamPort->RbIo,addr,FALSE, &DnLnkCnt2->LNK_CNT2);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    PCI_TRACE((TRACE_PCI," LNK_CNT2-> [CmDe=%X|CmSOS=%X|ECm=%X|TrM=%X|SlDe=%X|AuSp=%X|EnCm=%X|TLS=%X]\n",
                DnLnkCnt2->ComplDeEmphasis,
                DnLnkCnt2->ComplianceSos,
                DnLnkCnt2->EnterModCompl,
                DnLnkCnt2->TrsMargin,
                DnLnkCnt2->SelDeEmphasis,
                DnLnkCnt2->HwAutoSpeedDis,
                DnLnkCnt2->EnterCompliance,
                DnLnkCnt2->TargetLnkSpeed));


    addr.Addr.Register=DnStreamPort->PciExpress->PcieOffs+PCIE_LNK_STA_OFFSET;
    Status=PciCfg16(DnStreamPort->RbIo,addr,FALSE, &dnls.LNK_STA);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    //Get UpStream Stream Port Properties
    addr.ADDR=UpStreamPort->Address.ADDR;

    //PCI_TRACE((TRACE_PCI," UP STREAM PORT @ [B%X|D%X|F%X]:\n",
    //        addr.Addr.Bus,addr.Addr.Device, addr.Addr.Function));

    addr.Addr.Register=UpStreamPort->PciExpress->PcieOffs+PCIE_LNK_CNT2_OFFSET;
    Status=PciCfg16(UpStreamPort->RbIo,addr,FALSE, &UpLnkCnt2->LNK_CNT2);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    PCI_TRACE((TRACE_PCI," LNK_CNT2-> [CmDe=%X|CmSOS=%X|ECm=%X|TrM=%X|SlDe=%X|AuSp=%X|EnCm=%X|TLS=%X]\n    ....... UPDATINNG .......\n",
                UpLnkCnt2->ComplDeEmphasis,
                UpLnkCnt2->ComplianceSos,
                UpLnkCnt2->EnterModCompl,
                UpLnkCnt2->TrsMargin,
                UpLnkCnt2->SelDeEmphasis,
                UpLnkCnt2->HwAutoSpeedDis,
                UpLnkCnt2->EnterCompliance,
                UpLnkCnt2->TargetLnkSpeed));

    //Get Lnk Status Content...
    addr.Addr.Register=UpStreamPort->PciExpress->PcieOffs+PCIE_LNK_STA_OFFSET;
    Status=PciCfg16(UpStreamPort->RbIo,addr,FALSE, &upls.LNK_STA);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    //Link Speed Settings...START
    if((DnStreamPort->PciExpress->Pcie2->Pcie2Setup.LnkSpeed!=PCI_SETUP_DONT_TOUCH) && 
            (UpStreamPort->PciExpress->Pcie2->Pcie2Setup.LnkSpeed!=PCI_SETUP_DONT_TOUCH)){
        //get Link Speed option
        if(DnStreamPort->PciExpress->Pcie2->Pcie2Setup.LnkSpeed<=UpStreamPort->PciExpress->Pcie2->Pcie2Setup.LnkSpeed)
            LnkSpeedOption=DnStreamPort->PciExpress->Pcie2->Pcie2Setup.LnkSpeed;
        else LnkSpeedOption=UpStreamPort->PciExpress->Pcie2->Pcie2Setup.LnkSpeed;
    
        //Check link Speed selected in Setup and calculate applicable Link speed...
        //get max possible link speed
        maxls=DnStreamPort->PciExpress->LinkCap.MaxLnkSpeed;
        if(maxls>UpStreamPort->PciExpress->LinkCap.MaxLnkSpeed) maxls=UpStreamPort->PciExpress->LinkCap.MaxLnkSpeed;
    
        //get the current speed of the link...
        curls=DnStreamPort->PciExpress->CurrLnkSpeed;
        //must be the same on both sides of the link
        ASSERT(curls==UpStreamPort->PciExpress->CurrLnkSpeed);
    
        if(maxls>=PCIE_LINK_SPEED_80G){
            if(!Pcie3CheckPcie3Compatible(DnStreamPort)|| !Pcie3CheckPcie3Compatible(UpStreamPort)) maxls=PCIE_LINK_SPEED_50G;
            else{
               if((maxls>PCIE_LINK_SPEED_80G) && (LnkSpeedOption==3))ls=PCIE_LINK_SPEED_80G; 
            }
        }
        
        //Check Link Speed setup options...
        //AUTO==PCI_SETUP_AUTO_VALUE(55 dec)
        ls=curls;
        if(LnkSpeedOption==PCI_SETUP_AUTO_VALUE)ls=maxls;
    
        //Force 5.0GT/s == 2
        if(LnkSpeedOption==2){
            //check if it is supported
            if(DnStreamPort->PciExpress->Pcie2->LinkCap2.SuppLnkSpeeds!=0){
                //if device is compatible with V3.0 spec it must report supported speeds
                if(DnStreamPort->PciExpress->Pcie2->LinkCap2.SuppLnkSpeeds & PCIE_LINK_SPEED_VECT_50G) ls=PCIE_LINK_SPEED_50G;
            } else {
                //for devices complaint to spec v 2.1 check maxls value
                if(maxls>=PCIE_LINK_SPEED_50G) ls=PCIE_LINK_SPEED_50G;
            }
        }
    
        //Force 2.5GT/s == 1
        if(LnkSpeedOption==1){
            //check if it is supported
            if(DnStreamPort->PciExpress->Pcie2->LinkCap2.SuppLnkSpeeds!=0){
                //if device is compatible with V3.0 spec it must report supported speeds
                if(DnStreamPort->PciExpress->Pcie2->LinkCap2.SuppLnkSpeeds & PCIE_LINK_SPEED_VECT_25G)ls=PCIE_LINK_SPEED_25G;
            } else {
                //for devices compliand to spec v 2.1 check maxls value
                if(maxls>=PCIE_LINK_SPEED_25G) ls=PCIE_LINK_SPEED_25G;
            }
        }
    
        PCI_TRACE((TRACE_PCI," LNK SPEED SETTINGS: MaxLS=%X; CurrentLS=%X; SelectedLS=%X; Setup=%d\n",
                maxls,curls,ls, LnkSpeedOption ));
    
        //Call porting hook to override link speed settings.
        lsstore=ls;
    
        //All conditions for launching init routine checked inside  AmiPciLibLaunchInitRoutine() itself
        //so the previous code was inefficient.
        //The only thing... values must be the same on both sides of the link.
        //DOWN STREAM  PORT will have priority here since it closer to onboard infrastructure...
        Status=AmiPciLibLaunchInitRoutine(DnStreamPort, isPcieSetLinkSpeed, itDevice, DnStreamPort, UpStreamPort, &ls, &maxls);
        //Handle returned values...
        if(EFI_ERROR(Status)){
            if(Status==EFI_UNSUPPORTED){
                //just in case if function returned UNSUPPORTED has modified Link Speed settings - Restore it...
                ls=lsstore;
                PCI_TRACE((TRACE_PCI,"\n DnStreamPort: Don't Overwrite LNK_SPEED = 0x%X: %r\n", ls, Status));
    
                //if function unsupported by DNSTRM_PORT, try UPSTREAM now...
                Status=AmiPciLibLaunchInitRoutine(UpStreamPort, isPcieSetLinkSpeed, itDevice, DnStreamPort, UpStreamPort, &ls, &maxls);
                //Handle returned values...
                if(EFI_ERROR(Status)){
                    if(Status==EFI_UNSUPPORTED){
                        //just in case if function returned UNSUPPORTED has modified Link Speed settings - Restore it...
                        ls=lsstore;
                        PCI_TRACE((TRACE_PCI,"\n UpStreamPort: Don't Overwrite LNK_SPEED = 0x%X: %r\n", ls, Status));
                    } else {
                        ASSERT_EFI_ERROR(Status);
                        return Status;
                    }
                } else {
                    //this is SUCCESS status branch..
                    PCI_TRACE((TRACE_PCI,"\n UpStreamPort: Overwrites LNK_SPEED=0x%X; Old LNK_SPEED=0x%X: %r\n", ls, lsstore, Status));
                }  
                Status=EFI_SUCCESS;
            } else {
                ASSERT_EFI_ERROR(Status);
                return Status;
            }
        } else {
            //this is SUCCESS status branch..
            PCI_TRACE((TRACE_PCI,"\n DnStreamPort: Overwrites LNK_SPEED=0x%X; Old LNK_SPEED=0x%X: %r\n", ls, lsstore, Status));
        }  
    
        //now check if curls different from selected ls
        if(ls!=curls){
    //        DnLnkCnt2->EnterCompliance=TRUE;
    //        UpLnkCnt2->EnterCompliance=TRUE;
    //        *LinkHotResetRequired=TRUE;
            *LinkRetrainRequired=TRUE;
        }
    
        //Set selected speed on both sides of the link
        DnLnkCnt2->TargetLnkSpeed=ls;
        UpLnkCnt2->TargetLnkSpeed=ls;
    } else {
        ls=PCI_SETUP_DONT_TOUCH;
    }
    //Link Speed Settings...END
    
    //get Link HwAutoSpeed option
    if((DnStreamPort->PciExpress->Pcie2->Pcie2Setup.HwAutoSpeed!=PCI_SETUP_DONT_TOUCH) && 
            (UpStreamPort->PciExpress->Pcie2->Pcie2Setup.HwAutoSpeed!=PCI_SETUP_DONT_TOUCH)){
        
        if(DnStreamPort->PciExpress->Pcie2->Pcie2Setup.HwAutoSpeed<=UpStreamPort->PciExpress->Pcie2->Pcie2Setup.HwAutoSpeed)
            HwAutoSpeedOption=DnStreamPort->PciExpress->Pcie2->Pcie2Setup.HwAutoSpeed;
        else HwAutoSpeedOption=UpStreamPort->PciExpress->Pcie2->Pcie2Setup.HwAutoSpeed;
    
        //Check;  HwAutoSpeedDis; HwAutoWidth settings
        DnLnkCnt2->HwAutoSpeedDis=HwAutoSpeedOption;
        UpLnkCnt2->HwAutoSpeedDis=HwAutoSpeedOption;
    }
    
    PCI_TRACE((TRACE_PCI," LNK_CNT2-> [CmDe=%X|CmSOS=%X|ECm=%X|TrM=%X|SlDe=%X|AuSp=%X|EnCm=%X|TLS=%X]\n",
                DnLnkCnt2->ComplDeEmphasis,
                DnLnkCnt2->ComplianceSos,
                DnLnkCnt2->EnterModCompl,
                DnLnkCnt2->TrsMargin,
                DnLnkCnt2->SelDeEmphasis,
                DnLnkCnt2->HwAutoSpeedDis,
                DnLnkCnt2->EnterCompliance,
                DnLnkCnt2->TargetLnkSpeed));

    PCI_TRACE((TRACE_PCI," LNK_CNT2-> [CmDe=%X|CmSOS=%X|ECm=%X|TrM=%X|SlDe=%X|AuSp=%X|EnCm=%X|TLS=%X]\n\n",
                UpLnkCnt2->ComplDeEmphasis,
                UpLnkCnt2->ComplianceSos,
                UpLnkCnt2->EnterModCompl,
                UpLnkCnt2->TrsMargin,
                UpLnkCnt2->SelDeEmphasis,
                UpLnkCnt2->HwAutoSpeedDis,
                UpLnkCnt2->EnterCompliance,
                UpLnkCnt2->TargetLnkSpeed));


//#if PCI_EXPRESS_GEN3_SUPPORT
	if(Pcie3CheckPcie3Compatible(DnStreamPort) && Pcie3CheckPcie3Compatible(UpStreamPort)){

    if((ls != PCI_SETUP_DONT_TOUCH) && (ls >= PCIE_LINK_SPEED_80G)) {
        Status=Pcie3EqualizeLink(DnStreamPort, LinkRetrainRequired, (UINT8)ls);
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) return Status;
        //Equalize link must be set for Upstream Port if it has closslink
        if(UpStreamPort->PciExpress->Pcie2->LinkCap2.CrossLnk){
            Status=Pcie3EqualizeLink(UpStreamPort, LinkRetrainRequired,(UINT8)ls);
            ASSERT_EFI_ERROR(Status);
        	}
		}
    }
//#endif

    return Status;
}

/**
    This function will select appropriate Completion Timeout range
    from supported by the device.

        
    @param Support Supported by Device Completion Timeout ranges.
    @param Short A Flag to Indicate wahat type of ranges to select Biggest or Smallest

    @retval UINT16
        Value to be programmed in DEV_CNT2 Register.

**/
UINT16 Pcie2SelectComplTimeOut(UINT32 Support, UINT8 Short){
    //Completion Timeout Programability is not supported by HW.
    if(Support==0) return 0;

    //Select default CTO according to PCI SIG ECN:
    //UEFI must set PCI device’s Completion Timeout (CTO) range  to 250ms-900ms if CTO range C is supported. 
    //Otherwise CTO must be disabled in that device.
    if(Short==0xFF){
        if(Support & PCIE_CAP2_RANGE_C) return 0x9;
        else return 0;
    }
    
    if(Short==TRUE){
        //It is strongly recommended that the Completion Timeout
        //mechanism not expire in less than 10 ms

        //Values available if Range A (50 mks to 10 ms) programmability range is supported:
        // 0001b 50 mks to 100 mks; 0010b; 1 ms to 10 ms
        if(Support & PCIE_CAP2_RANGE_A) return 0x2;

        //Values available if Range B (10 ms to 250 ms) programmability range is supported:
        // 0101b 16 ms to 55 ms; 0110b 65 ms to 210 ms
        if(Support & PCIE_CAP2_RANGE_B) return 0x5;

        //Values available if Range C (250 ms to 4 s) programmability range is supported:
        // 1001b 260 ms to 900 ms; 1010b 1 s to 3.5 s
        if(Support & PCIE_CAP2_RANGE_C) return 0x9;

        //Values available if the Range D (4 s to 64 s) programmability range is supported:
        // 1101b 4 s to 13 s; 1110b 17 s to 64 s
        if(Support & PCIE_CAP2_RANGE_D) return 0xD;

    } else {
        //Values available if the Range D (4 s to 64 s) programmability range is supported:
        // 1101b 4 s to 13 s; 1110b 17 s to 64 s
        if(Support & PCIE_CAP2_RANGE_D) return 0xE;

        //Values available if Range C (250 ms to 4 s) programmability range is supported:
        // 1001b 260 ms to 900 ms; 1010b 1 s to 3.5 s
        if(Support & PCIE_CAP2_RANGE_C) return 0xA;

        //Values available if Range B (10 ms to 250 ms) programmability range is supported:
        // 0101b 16 ms to 55 ms; 0110b 65 ms to 210 ms
        if(Support & PCIE_CAP2_RANGE_B) return 0x6;

        //Values available if Range A (50 mks to 10 ms) programmability range is supported:
        // 0001b 50 mks to 100 mks; 0010b; 1 ms to 10 ms
        if(Support & PCIE_CAP2_RANGE_A) return 0x2;
    }
    return 0;
}


/**
    This function will Select values for DEVICE CCONTROL2 register
    based on Setup Settings and hardware capabilities.

        
    @param DnStreamPort Pointer to PCI Device Private Data of Downstream Port of the link.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetDevProperties(	PCI_DEV_INFO 		*Device){
    EFI_STATUS          Status;
	PCI_CFG_ADDR		addr;
    PCIE_DEV_CNT2_REG   dcnt2;
    PCIE2_SETUP_DATA	*Pcie2Setup;
//------------------------------

    if (!Pcie2CheckPcie2Compatible(Device)) return EFI_SUCCESS;

    //Get Device Properties
    addr.ADDR=Device->Address.ADDR;
    Pcie2Setup=&Device->PciExpress->Pcie2->Pcie2Setup;

    PCI_TRACE((TRACE_PCI,"PciE2: Pcie2SetDevProperties() Device @ [B%X|D%X|F%X] ENTRY:\n",
            addr.Addr.Bus,addr.Addr.Device, addr.Addr.Function));

    addr.Addr.Register=Device->PciExpress->PcieOffs+PCIE_DEV_CNT2_OFFSET;
    Status=PciCfg16(Device->RbIo,addr, FALSE, &dcnt2.DEV_CNT2);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    PCI_TRACE((TRACE_PCI," DEV_CNT2-> [EEPrB=%X|R=%X|LTR=%X|IDOCm=%X|IDORq=%X|AtEgB=%X|AtRq=%X|ARI=%X|CToD=%X|CTo=%X]\n    ....... UPDATINNG .......\n",
                dcnt2.EndEndTlpBlk,
                dcnt2.Reserved,
                dcnt2.LtrEn,
                dcnt2.IdoComplEn,
                dcnt2.IdoRequestEn,
                dcnt2.AtomicOpEgresBlk,
                dcnt2.AtomicOpRequer,
                dcnt2.AriForwarding,
                dcnt2.ComplToutDisable,
                dcnt2.ComplToutRanges));

    //Now check setup settings and apply it. But do't touch ARI Forwarding it was updated earlier.
    if(Pcie2Setup->E2ETlpPrBlk != PCI_SETUP_DONT_TOUCH) dcnt2.EndEndTlpBlk=Pcie2Setup->E2ETlpPrBlk;
    if(Pcie2Setup->LtrReport   != PCI_SETUP_DONT_TOUCH) dcnt2.LtrEn=Pcie2Setup->LtrReport;
    if(Pcie2Setup->IDOCompl    != PCI_SETUP_DONT_TOUCH) dcnt2.IdoComplEn=Pcie2Setup->IDOCompl;
    if(Pcie2Setup->IDOReq      != PCI_SETUP_DONT_TOUCH) dcnt2.IdoRequestEn=Pcie2Setup->IDOReq;
    if(Pcie2Setup->AtomOpEgressBlk!= PCI_SETUP_DONT_TOUCH) dcnt2.AtomicOpEgresBlk=Pcie2Setup->AtomOpEgressBlk;
    if(Pcie2Setup->AtomOpReq   != PCI_SETUP_DONT_TOUCH) dcnt2.AtomicOpRequer=Pcie2Setup->AtomOpReq;

    if(Pcie2Setup->ComplTimeOut != PCI_SETUP_DONT_TOUCH){

        //Now check what completion Timeout ranges to select?
        dcnt2.ComplToutDisable=FALSE;
        dcnt2.ComplToutRanges=0; //set to default

        //In Setup 0=Disable;0x55=SHORT; 0xAA=LONG; 0xFF=DEFAULT
        if(Pcie2Setup->ComplTimeOut == 0) dcnt2.ComplToutDisable=TRUE;
        else {
            if(Pcie2Setup->ComplTimeOut == 0xFF)  
                dcnt2.ComplToutRanges=Pcie2SelectComplTimeOut(Device->PciExpress->Pcie2->DeviceCap2.ComplToutRanges, 0xFF);
            else{
                //use SHORT timeout srom supported ranges option.
                if(Pcie2Setup->ComplTimeOut == 0x55)
                    dcnt2.ComplToutRanges=Pcie2SelectComplTimeOut(Device->PciExpress->Pcie2->DeviceCap2.ComplToutRanges, TRUE);
                //use LONG timeout srom supported ranges option.
                else
                    dcnt2.ComplToutRanges=Pcie2SelectComplTimeOut(Device->PciExpress->Pcie2->DeviceCap2.ComplToutRanges, FALSE);
            }
        }
    }

    Status=PciCfg16(Device->RbIo,addr, TRUE, &dcnt2.DEV_CNT2);

    PCI_TRACE((TRACE_PCI," DEV_CNT2-> [EEPrB=%X|R=%X|LTR=%X|IDOCm=%X|IDORq=%X|AtEgB=%X|AtRq=%X|ARI=%X|CToD=%X|CTo=%X]\n\n",
                dcnt2.EndEndTlpBlk,
                dcnt2.Reserved,
                dcnt2.LtrEn,
                dcnt2.IdoComplEn,
                dcnt2.IdoRequestEn,
                dcnt2.AtomicOpEgresBlk,
                dcnt2.AtomicOpRequer,
                dcnt2.AriForwarding,
                dcnt2.ComplToutDisable,
                dcnt2.ComplToutRanges));
//    PCI_TRACE((TRACE_PCI,"Status = %r.\n",Status));

    ASSERT_EFI_ERROR(Status);
    return Status;
}


/**

    @param 

    @retval 

**/
EFI_STATUS Pcie2AllocateInitPcie2Data(PCI_DEV_INFO *Device){

	if(Device->PciExpress->PcieCap.CapVersion>=PCIE_CAP_VER2){
		Device->PciExpress->Pcie2=MallocZ(sizeof(PCIE2_DATA));
		if(Device->PciExpress->Pcie2==NULL){
			ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
			return EFI_OUT_OF_RESOURCES;
		} else return Pcie2GetGen2Info(Device);//Function Will update GEN 3 Properties as well...
	}
	return EFI_SUCCESS;
}



/**
    This function will Set CRS Sw visibility if device supports it.

        
    @param Device Pointer to PCI Device Private Data of Downstream Port of the link.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_ERROR        		When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetCrs(PCI_DEV_INFO *Device){
    EFI_STATUS          Status;
	PCI_CFG_ADDR		addr;
//-----------------------------
	if(!PcieCrsSupport) return EFI_SUCCESS;
	
	if(Device->PciExpress->PcieCap.PortType==PCIE_TYPE_ROOTPORT && Device->PciExpress->RootCap.CrsSwVisib){
		PCIE_ROOT_CNT_REG	rootc;
	//-----------------------------
		addr.ADDR=Device->Address.ADDR;
		addr.Addr.Register=Device->PciExpress->PcieOffs+PCIE_ROOT_CNT_OFFSET;

		//Get context of ROOT_CNT_REG...
		Status=PciCfg16(Device->RbIo,addr,FALSE,&rootc.ROOT_CNT);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
		
		rootc.CrsSwVisib=1;
			
		//Write it back..
		Status=PciCfg16(Device->RbIo,addr,TRUE,&rootc.ROOT_CNT);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
	}
	
	//In case of PCIe 2 PCI/PCI-X Bridge bit 15 must be set,
	//to forward CSR from Secondary Interface
	if(Device->PciExpress->PcieCap.PortType==PCIE_TYPE_PCIE_PCI){
		PCIE_DEV_CNT_REG devc;
	//--------------------------
		addr.ADDR=Device->Address.ADDR;
		addr.Addr.Register=Device->PciExpress->PcieOffs+PCIE_DEV_CNT_OFFSET;

		//Get context of PCIE_DEV_CNT_REG...
		Status=PciCfg16(Device->RbIo,addr,FALSE,&devc.DEV_CNT);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
		
		//set Bridge Configuration Retry Enable bit
		devc.FnRstBrgRtry=1;
			
		//Write it back..
		Status=PciCfg16(Device->RbIo,addr,TRUE,&devc.DEV_CNT);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
	}
	
	return EFI_SUCCESS;
}

/**
    This function will Set CRS Software visibility trough RCRB Ext. Capability if device supports it.

        
    @param Device Pointer to PCI Device Private Data of Downstream Port of the link.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_ERROR        		When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetCrsExt(PCI_DEV_INFO *Device){
    EFI_STATUS          Status;
	PCI_CFG_ADDR		addr;
//-----------------------------
	if(!PcieCrsSupport) return EFI_SUCCESS;

	if(Device->PciExpress->RcrbData->RcrbCap.CrsSwVisib){
		PCIE_RCRB_CNT_REG rcrbc;
	//-----------------------------	
		addr.ADDR=Device->Address.ADDR;
		addr.Addr.ExtendedRegister=Device->PciExpress->RcrbData->RcrbCapHdrOffs+PCIE_RCRB_CNT_OFFSET;
	
		//Get context of ROOT_CNT_REG...
		Status=PciCfg32(Device->RbIo,addr,FALSE,&rcrbc.RCRB_CNT);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
		
		rcrbc.CrsSwVisib=1;
			
		//Write it back..
		Status=PciCfg32(Device->RbIo,addr,TRUE,&rcrbc.RCRB_CNT);
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
	}
	
	return EFI_SUCCESS;
}

/**

    @param 

    @retval 

**/
/*
EFI_STATUS EFIAPI AmiPciExpressLibConstructor(IN EFI_HANDLE  ImageHandle, IN EFI_SYSTEM_TABLE  *SystemTable)
{
	EFI_STATUS				Status=EFI_SUCCESS;
//-------------------------------------------------
	InitAmiLib(ImageHandle, SystemTable);

	return Status;;
}
*/




//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

