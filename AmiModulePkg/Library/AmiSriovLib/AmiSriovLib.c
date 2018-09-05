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
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
/** @file AmiSriovLib.c
        Library Class for AMI SR-IOV Functions. 


**/
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <AcpiRes.h>
#include <PciBus.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/AmiPciBusLib.h>
#include <Library/AmiPciExpressLib.h>


//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------


/**
    This function will collect information about SRIOV PCIE Device
    and initialize it based on information collected.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
//EFI_STATUS SriovProbeDevice(PCI_DEV_INFO *Device, UINT32 SriovCapBaseOffset, UINT8 *MaxBusFound){
EFI_STATUS SriovProbeDevice(PCI_DEV_INFO *Device, UINT8 *MaxBusFound){
    EFI_STATUS Status;
    PCIE_SRIOV_CNT_REG SrIoVControl;
    UINT32 PageSize, i=0, incr, maxbar;
    UINT16 InitialFv;
    UINT16 FirstVfOffset;
    UINT16 VfStride;
    UINT16 RoutingId;
    UINT8  VfMaxBus;
    UINT32 SriovCapBaseOffset;
    PCI_BAR *Bar;
	PCI_CFG_ADDR		addr;
//-------------------------------------
    SriovCapBaseOffset=Device->PciExpress->SriovData->SriovCapHdrOffs;
    addr.ADDR=Device->Address.ADDR;
    addr.Addr.ExtendedRegister = SriovCapBaseOffset;
    PCI_TRACE((TRACE_PCI,"PciBus: Found SRIOV Device @ B%X|D%X|F%X; ", addr.Addr.Bus, addr.Addr.Device, addr.Addr.Function));

//    Device->PciExpress->SriovData = MallocZ(sizeof(PCIE_SRIOV_DATA));
//    ASSERT(Device->PciExpress->SriovData!=NULL);

    //Disable VFs before any further manipulations
    addr.Addr.ExtendedRegister=SriovCapBaseOffset+PCIE_SRIOV_CNT_OFFSET;
    Status=PciCfg16(Device->RbIo,addr,FALSE,&SrIoVControl.SRIOV_CNT);
    ASSERT_EFI_ERROR(Status);
    SrIoVControl.VfEnable = 0;
    SrIoVControl.VfMse = 0;
    Status=PciCfg16(Device->RbIo,addr,TRUE,&SrIoVControl.SRIOV_CNT);
	ASSERT_EFI_ERROR(Status);

    //Initislize System Page Size
    //get supported page sizes
    addr.Addr.ExtendedRegister=SriovCapBaseOffset+PCIE_SRIOV_SUPPORTED_PAGE_SIZES_OFFSET;
    Status=PciCfg32(Device->RbIo,addr,FALSE,&PageSize);
    ASSERT_EFI_ERROR(Status);
    PCI_TRACE((TRACE_PCI,"PageSize: Sup=0x%X; ", PageSize));
    if(PageSize){
        //find the leftmost non-zero bit
	    for(i=1; (PageSize&i)==0 && i!=0; i<<=1) ;
        ASSERT(i!=0);
        //set page size
        addr.Addr.ExtendedRegister=SriovCapBaseOffset+PCIE_SRIOV_SYSTEM_PAGE_SIZE_OFFSET;
        Status=PciCfg32(Device->RbIo,addr,TRUE,&i);
	    ASSERT_EFI_ERROR(Status);
        PageSize = 1<<(i+11);
    }
    PCI_TRACE((TRACE_PCI,"Progr=0x%X(0x%X); ", i, PageSize));

    //Read InitialVF (number of initially associated VFs)
    //SRIOV Spec. 3.3.5. 
    //"For Devices operating in Single Root mode, this field is HwInit 
    //and must contain the same value as TotalVFs"
    addr.Addr.ExtendedRegister=SriovCapBaseOffset+PCIE_SRIOV_INITIAL_VF_OFFSET;
    Status=PciCfg16(Device->RbIo,addr,FALSE,&InitialFv);
    ASSERT_EFI_ERROR(Status);
    PCI_TRACE((TRACE_PCI,"InitialFv=%X; ", InitialFv));

    //++EIP 124526
    if(InitialFv==0){
       	//In SRIOV spec such situation means NO VF associated with PF
       	//No virtualization facilities for this function!
        PCI_TRACE((TRACE_PCI,"- ZERO VFs free SRIOV DATA...EFI_SUCCESS\n"));
    	pBS->FreePool(Device->PciExpress->SriovData);
    	Device->PciExpress->SriovData=NULL;

    	return EFI_SUCCESS;
    }
    //--EIP 124526
    
    //Calculate number of buses consumed by VF
    //Read FirstVfOffset
    addr.Addr.ExtendedRegister=SriovCapBaseOffset+PCIE_SRIOV_FIRST_VF_OFFSET;
    Status=PciCfg16(Device->RbIo,addr,FALSE,&FirstVfOffset);
    ASSERT_EFI_ERROR(Status);
    PCI_TRACE((TRACE_PCI,"FirstVfOffset=%X; ", FirstVfOffset));
    //Read VfStride
    addr.Addr.ExtendedRegister=SriovCapBaseOffset+PCIE_SRIOV_VF_STRIDE_OFFSET;
    Status=PciCfg16(Device->RbIo,addr,FALSE,&VfStride);
    ASSERT_EFI_ERROR(Status);
    PCI_TRACE((TRACE_PCI,"VfStride=%X; ", VfStride));
    
    //Calculate number of busses conusmed by all VF
    RoutingId = //Request ID of PF
    			(addr.Addr.Bus << 8 ) +
    			(addr.Addr.Device << 3 ) +
    			addr.Addr.Function +
    			FirstVfOffset + (InitialFv-1)*VfStride;
    
    VfMaxBus = (UINT8)(RoutingId>>8);
    PCI_TRACE((TRACE_PCI,"VfMaxBus=%X; ", VfMaxBus));
    //reserve additional buses consumed by VF
    
    //++EIP 124526
    if (VfMaxBus > *MaxBusFound ) {
    	*MaxBusFound=VfMaxBus;
    }
    //--EIP 124526
    
    //Read VF BARs
    addr.Addr.ExtendedRegister=SriovCapBaseOffset+PCIE_SRIOV_VF_BAR0_OFFSET;
    incr=0;
    Bar = &Device->PciExpress->SriovData->Bar[0];
    maxbar = PCI_MAX_BAR_NO;
	for(i=0; i<maxbar; i++){
        UINT32 old32, val32;
        UINT64 old64, val64;
        Status=PciCfg32(Device->RbIo,addr,FALSE,&val32);
        if(EFI_ERROR(Status)) return Status;
        ASSERT((val32 & 1)==0); //VF BAR must be MMIO
        switch (val32 & 0x0F) {
            case 0x0 :
                Bar[i].Type=tBarMmio32;
		        Bar[i].DiscoveredType=tBarMmio32;
			    incr=4;
                break;
            case 0x4 :
			    Bar[i].Type=tBarMmio64;
			    Bar[i].DiscoveredType=tBarMmio64;
				incr=8;
				maxbar--;
				break;
			case 0x8 :
			    if(CombineMemPfMem32){
			        Bar[i].Type=tBarMmio32;
			    } else {
			        Bar[i].Type=tBarMmio32pf;
			    }
			    Bar[i].DiscoveredType=tBarMmio32pf;
				incr=4;
				break;
			case 0xc :
				Bar[i].Type=tBarMmio64pf;
				Bar[i].DiscoveredType=tBarMmio64pf;
				incr=8;
				maxbar--;
				break;
			default : return EFI_UNSUPPORTED;
		} //switch
		Bar[i].ExtOffset=addr.Addr.ExtendedRegister;

        val32=(~0); val64=(~0);

		switch (Bar[i].Type){

		    case tBarMmio64pf	:
			case tBarMmio64		:
			    Status=PciCfg64(Device->RbIo,addr,FALSE,&old64);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg64(Device->RbIo,addr,TRUE,&val64);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg64(Device->RbIo,addr,FALSE,&val64);
				if(EFI_ERROR(Status)) return Status;
				val64&=(~0x0F); //Mask don't care bits
				if(val64){
				    Bar[i].Gran=(UINTN)(~val64);
					Bar[i].Length=Mul64((Bar[i].Gran+1),(UINT32)InitialFv);
				} else {
                    Bar[i].Type=tBarUnused;
                }
				break;

			case tBarMmio32pf	:
			case tBarMmio32		:
				Status=PciCfg32(Device->RbIo,addr,FALSE,&old32);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg32(Device->RbIo,addr,TRUE,&val32);
				if(EFI_ERROR(Status)) return Status;
				Status=PciCfg32(Device->RbIo,addr,FALSE,&val32);
				if(EFI_ERROR(Status)) return Status;
				val32&=(~0x0F);
				if(val32){
				    Bar[i].Gran=(~val32);
					Bar[i].Length=Mul64((Bar[i].Gran+1),(UINT32)InitialFv);
				} else{
                    Bar[i].Type=tBarUnused;
                }
				break;

			default : Bar[i].Type=tBarUnused;
		}//switch

		//Restore Original Value value
		if(Bar[i].Type!=tBarUnused){
		    if(Bar[i].Type==tBarMmio64pf || Bar[i].Type==tBarMmio64) Status=PciCfg64(Device->RbIo,addr,TRUE,&old64);
			else Status=PciCfg32(Device->RbIo,addr,TRUE,&old32);
			if(EFI_ERROR(Status)) return Status;
		}
        Bar[i].Owner = Device;
        addr.Addr.ExtendedRegister+=incr;
	} // Bar enumeration loop
    PCI_TRACE((TRACE_PCI,"\n"));
    return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
EFI_STATUS SriovAllocateSriovData(PCI_DEV_INFO *Device, UINT32 SriovCapOffset){
	EFI_STATUS		 Status;
//--------------------------------
    PCI_TRACE((TRACE_PCI,"PciE2: Found SRIOV Ext Cap Header @ offset 0x%X\n", SriovCapOffset));
    Status=PcieCollectDeviceExtCap(Device,(VOID**) &Device->PciExpress->SriovData, sizeof(PCIE_SRIOV_DATA),
    		SriovCapOffset, FALSE, 0); //32 bit cap register
    ASSERT_EFI_ERROR(Status);
    //if(EFI_ERROR(Status)) return Status;

    //Status=SriovProbeDevice(Device, SriovCapOffset, MaxBusFound);
    //PCI_TRACE((TRACE_PCI,"PciE2: SRIOVProbeDevice Status=%r\n", Status));
    //ASSERT_EFI_ERROR(Status);
    
    return Status;
}

/**

    @param 

    @retval 

**/
BOOLEAN SriovCheckSriovCompatible(PCI_DEV_INFO	*Device){
	if( PcieCheckPcieCompatible(Device) && Device->PciExpress->SriovData!=NULL)return TRUE;
	else return FALSE;
}


/**

    @param 

    @retval 

**/
EFI_STATUS	SriovDbAddVirtualBar(PCI_DEV_INFO *Device, DBE_DATABASE *Db, MRES_TYPE ResType){
    PCI_BAR 	*VfBar;
    UINTN		j;
    EFI_STATUS	Status=EFI_SUCCESS;
//---------------------------
    VfBar=&Device->PciExpress->SriovData->Bar[0];

    for( j=0; j<PCI_MAX_BAR_NO; j++){
	    //Use ResType+1 Since CountBars() takes diferent type of parameter -
	    //PCI_BAR_TYPE. It mapps to MRES_TYPE as MRES_TYPE+1
	    if ((VfBar[j].Type==ResType+1) && VfBar[j].Length){
		    Status=DbeInsert(Db,&VfBar[j]);
		    if(EFI_ERROR(Status)) return Status;
	    }
    }//bar loop

	return Status;
}


/**

    @param 

    @retval 

**/
BOOLEAN SriovCheckBarType(PCI_DEV_INFO *Device, PCI_BAR_TYPE BarType){
	UINTN			j;
//------------------
    for(j=0; j<PCI_MAX_BAR_NO; j++) {
       	if(Device->PciExpress->SriovData->Bar[j].Type==BarType) return TRUE;
    }
    return FALSE;
}


/**

    @param 

    @retval 

**/
VOID SriovConvertResources(	PCI_DEV_INFO *Device, PCI_BAR_TYPE NarrowType, PCI_BAR_TYPE WideType,
							RES_CONV_TYPE ConvType, BOOLEAN NeedToConvert, BOOLEAN CombineMemPmem)
{
	UINTN	j;
//-----------------
	for(j=0; j<PCI_MAX_BAR_NO; j++){
	   if(NeedToConvert){
		   switch(ConvType){
		   	   case rcNone:
		   		   if(Device->PciExpress->SriovData->Bar[j].Type==WideType){
		   			   ClearBar(&Device->PciExpress->SriovData->Bar[j]);
                   }
		   		   break;
		   	   case rcOneOf:
		   	   case rcNarrow:
				   if(Device->PciExpress->SriovData->Bar[j].Type==WideType){
					   Device->PciExpress->SriovData->Bar[j].Type=NarrowType;
                   }
				   break;
		   	   case rcBoth:    
		   		   break;
		   }//switch
	   } //if cnv
	   
	   if(CombineMemPmem){
		   if( Device->PciExpress->SriovData->Bar[j].Type==tBarMmio32pf )
			   Device->PciExpress->SriovData->Bar[j].Type=tBarMmio32;
		   if( Device->PciExpress->SriovData->Bar[j].Type==tBarMmio64pf )
			   Device->PciExpress->SriovData->Bar[j].Type=tBarMmio64;
	   }
	}//for j
}

/**
    Set/Reset ARI Capable Hierarchy bit in SR-IOV Control register 

    @param 

    @retval 

**/
EFI_STATUS  SriovAriCapableHierarchy(PCI_DEV_INFO *Device, BOOLEAN Set){
    PCIE_SRIOV_CNT_REG  SrIoVControl;
    EFI_STATUS          Status;
    PCI_CFG_ADDR        addr;
//---------------------------------------
    if(!SriovCheckSriovCompatible(Device)) return EFI_NOT_FOUND;
    
    //Get SR-IOV CNT Reg
    addr.ADDR=Device->Address.ADDR;
    addr.Addr.ExtendedRegister = Device->PciExpress->SriovData->SriovCapHdrOffs+PCIE_SRIOV_CNT_OFFSET;
    
    Status=PciCfg16(Device->RbIo,addr,FALSE,&SrIoVControl.SRIOV_CNT);
    ASSERT_EFI_ERROR(Status);
    SrIoVControl.AreCap=Set;
    Status=PciCfg16(Device->RbIo,addr,TRUE,&SrIoVControl.SRIOV_CNT);
    ASSERT_EFI_ERROR(Status);

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

