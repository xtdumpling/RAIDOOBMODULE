//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
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
/** @file AmiPciHotPlugLib.c
    Library Class for AMI Hot Plug Support Functions. 


**/
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <AcpiRes.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHotPlugInit.h>
#include <Protocol/DevicePath.h>
#include <Library/AmiPciBusLib.h>
#include <Library/AmiPciExpressLib.h>
#include <Library/AmiSdlLib.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
//ONLY ONE INSTANCE ALOUD IN THE SYSTEM!!! 
EFI_PCI_HOT_PLUG_INIT_PROTOCOL	*gRhpcInitProtocol=NULL;
BOOLEAN 						gRhpcNotFound=FALSE;

/**
    This function will check if "HostBrg" passed is the Bridge
    with hotplug support.

        
    @param HostBrg Pointer to PCI HB Private Data structure.

    @retval BOOLEAN
        TRUE    If "Device" is a Bridge with HPC on it.
        FALSE   Otherwice.

**/
BOOLEAN HpcCheckHpCompatibleHost(PCI_HOST_BRG_DATA *HostBrg){
	if(	HostBrg->HpcData!=NULL && HostBrg->HpcData->HpcLocCount > 0 ) return TRUE;
	return FALSE;
}

/**
    This function will check if "Device" passed is the Bridge
    Type Device with hotplug support.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    If "Device" is a Bridge with HPC on it.
        FALSE   Otherwice.

**/
BOOLEAN HpIsHpb(PCI_DEV_INFO *Device){

	//check if it is the right type of device to have Hotplug capabilities
	if(!((Device->Type==tPci2PciBrg)||(Device->Type==tPci2CrdBrg)||(Device->Type==tPciRootBrg))){
		PCI_TRACE((TRACE_PCI,"PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (Brg Type)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
		return FALSE;
	}

	//Check if it has Root Hpc or PciExpress capabilities
	if((Device->HotPlug==NULL) && !PcieCheckPcieCompatible(Device)){
		PCI_TRACE((TRACE_PCI,"PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (SHPC/PCIe)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
		return FALSE;
	}
	//If Device is PciExpress, check if SLOT CAPABILITIES Register supports Hotplug
	if(PcieCheckPcieCompatible(Device) && (!Device->PciExpress->SlotCap.HpCapable)){
		PCI_TRACE((TRACE_PCI,"PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (SltCap.HpCap)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
//DEBUG	start all PCIe is HP Capable for debugging. Uncomment 
		//PCI_TRACE((TRACE_PCI,"DEBUG: Device @ B%X|D%X|F%X become HP CAPABLE (SltCap.HpCap)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
		return FALSE; //DEBUG	all PCIe is HP Capable for debugging. comment this line!
//DEBUG	end.
	}

	PCI_TRACE((TRACE_PCI,"PciHP: Device @ B%X|D%X|F%X is HP CAPABLE returning TRUE\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
	return TRUE;
}

/**
    This function will update pointer to PCI_RHPC_INFO of
    Bridge Type device which creates a hot plug bus. Also if "Device"
    creates the 'home bus' for Root HPC it will initialize Root HPC and
    record the HPC state;

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS HpCheckRootHotplug(PCI_DEV_INFO *Device, UINT8 MaxBusFound){
	EFI_STATUS			Status=EFI_SUCCESS;
	EFI_DEVICE_PATH_PROTOCOL *dp=NULL;
	PCI_HPC_INFO		*rhpc;
	UINTN				i;
//---------------
	if(Device->Type==tPci2PciBrg || Device->Type==tPci2CrdBrg || Device->Type==tPciRootBrg){
		//we have identify and Init all the Root Hpc and HPB
		if(Device->HostData->InitRhpcCount==Device->HostData->RhpcCount) return EFI_SUCCESS;
        PROGRESS_CODE(DXE_PCI_BUS_HPC_INIT);
		//here we must check if 2 things:
		//	1.If "Device" Device Path mutches with one of Root Hpc Device pathes
		//	2.If "Device" Device Path mutches with one of Root Hpb Device pathes
		for(i=0; i<Device->HostData->RhpcCount; i++){
			rhpc=Device->HostData->RhpcList[i];
			//if this controller already has been initialized - keep going
			if(rhpc->Initialized && rhpc->BusFound)	continue;


			//check if it is a secondary interface of the bridge where RHPC is sitting
			if(!rhpc->BusFound){
				if(!DPCmp(Device->DevicePath, rhpc->HpcLocation->HpbDevicePath)){
					rhpc->HpbBridge=Device;
					rhpc->BusFound=TRUE;
					Device->HotPlug=rhpc;
					PCI_TRACE((TRACE_PCI,"PciHP: Found HP Bus Bridge @ B%X|D%X|F%X \n\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
				}
			}

			if(!rhpc->Initialized){
				//We will cut the last node of the HPC device path
				//resulting Device Path will correspond to the ParentBridge.DevicePath of the HPC
				dp=DPCut(rhpc->HpcLocation->HpcDevicePath);
				//if the HPC sits behind this bridge get the secondary I/F bus number
				if(!DPCmp(Device->DevicePath,dp)){
					EFI_DEVICE_PATH_PROTOCOL	*tdp;
				//-----------------------------------
					rhpc->HpcPciAddr.Addr.Bus=MaxBusFound;
					tdp=DPGetLastNode(rhpc->HpcLocation->HpcDevicePath);

					rhpc->HpcPciAddr.Addr.Device=((PCI_DEVICE_PATH*)tdp)->Device;
					rhpc->HpcPciAddr.Addr.Function=((PCI_DEVICE_PATH*)tdp)->Function;

					//we will not set up an event to signal when HPC initialization is ready
					//that will give us a possibility to enumerate PCI BUS in one pass
					Status=Device->HostData->HpInitProt->InitializeRootHpc(
							Device->HostData->HpInitProt, rhpc->HpcLocation->HpcDevicePath,
							rhpc->HpcPciAddr.ADDR, NULL, &rhpc->HpcState);

					ASSERT_EFI_ERROR(Status);
					rhpc->Owner=Device;
					rhpc->Initialized=TRUE;
					PCI_TRACE((TRACE_PCI,"PciBus: Found RHPC @ B%X|D%X|F%X \n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
				}
			}

			//if we come here we did some step of the initialization process
			//let's check if this controller has been completly initialized.
			//if so we did our job. there are no reasons to stay in this loop any longer
			if(rhpc->Initialized && rhpc->BusFound)	{
				Device->HostData->InitRhpcCount++;
				break;
			}

		}//for
	}
	if(dp)pBS->FreePool(dp);
	return Status;
}

/**
    This function will get and apply resource padding
    requirements for the PCI to PCI Bridge or Card Bus Bridge, if this
    Bridge is supports hot plug.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS HpGetHpbResPadding(PCI_DEV_INFO *Device){
	EFI_STATUS		Status=EFI_SUCCESS;
//-----------------------------
	if(HpIsHpb(Device)){
		PCI_BRG_EXT		*ext=(PCI_BRG_EXT*)(Device+1);
		ASLR_QWORD_ASD	*cnf=NULL, *res=NULL;
		PCI_BAR			*bar;
	//----------------------------
		Status=Device->HostData->HpInitProt->GetResourcePadding(Device->HostData->HpInitProt,
						Device->HotPlug->HpcLocation->HpcDevicePath, Device->HotPlug->HpcPciAddr.ADDR,
						&Device->HotPlug->HpcState,&cnf,&ext->PaddAttr);

		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;

		if(ext->PaddAttr==EfiPaddingPciRootBridge){
			pBS->FreePool(cnf);
			return Status;
		}
		
		PCI_TRACE((TRACE_PCI,"PciHp Padding: Processing QWORD RD for [B%X|D%X|F%X]:", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
		//Check if we got a valid descriptors in cfg buffer
		if(!ValidateDescriptorBlock(cnf, tResAll, FALSE)){
			PCI_TRACE((TRACE_PCI," !!!INVALID DESCRIPTOR!!!\n"));
			return EFI_INVALID_PARAMETER;
		}

		PCI_TRACE((TRACE_PCI,"\n"));

		res=cnf;
		while(res->Hdr.HDR!=ASLV_END_TAG_HDR) {

			PCI_TRACE((TRACE_PCI,"\tType=0x%lX; TFlags=0x%lX; _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX; _GRA=0x%lX;\n", 
					res->Type,res->TFlags,res->_MIN,res->_MAX,res->_LEN,res->_GRA));

			
			bar=NULL;
			switch(res->Type){
				case ASLRV_SPC_TYPE_BUS	:
					bar=&ext->Pad[rtBus];
				break;
				case ASLRV_SPC_TYPE_IO	:
						bar=&ext->Pad[rtIo16];
						bar->Type=tBarIo16;
						if(res->_GRA==32){
							bar=&ext->Pad[rtIo32];
							bar->Type=tBarIo32;
						}
				break;
				case ASLRV_SPC_TYPE_MEM	:
					if(res->_GRA==32){
						if(res->TFlags.MEM_FLAGS._MEM==ASLRV_MEM_CEPF)	{
							bar=&ext->Pad[rtMmio32p];
							bar->Type=tBarMmio32pf;
						} else {
							bar=&ext->Pad[rtMmio32];
							bar->Type=tBarMmio32;
						}
					} else {
						if(res->_GRA==64){
							if(res->TFlags.MEM_FLAGS._MEM==ASLRV_MEM_CEPF){
								bar=&ext->Pad[rtMmio64p];
								bar->Type=tBarMmio64pf;
							} else {
								bar=&ext->Pad[rtMmio64];
								bar->Type=tBarMmio64;
							}
						} else {
							Status=EFI_INVALID_PARAMETER; //no other options alloud
							goto ExitLbl;
						}
					}
				break;
				default: {
					Status=EFI_INVALID_PARAMETER; //no other options alloud
					goto ExitLbl;
				}
			} //switch

			bar->Length=res->_LEN;
			bar->Gran=res->_MAX;
			
			res++;
		} //while
ExitLbl:	
		if(cnf!=NULL)pBS->FreePool(cnf);
		PCI_TRACE((TRACE_PCI,"PciHp: Padding for [B%X|D%X|F%X] Updated...Status=%r\n", 
		Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function, Status));
		
	}

	//Now after padding info was collected call porting hook to update it if needed.
	Status=AmiPciLibLaunchInitRoutine(Device, isHpUpdatePadding, itDevice, Device, NULL, NULL, NULL);
	if(EFI_ERROR(Status)){
		if(Status==EFI_UNSUPPORTED){
			Status=EFI_SUCCESS;
		} else ASSERT_EFI_ERROR(Status);
	} else {
        PCI_TRACE((TRACE_PCI,"PciInit: Device @ [B%X|D%X|F%X], Padding Data was Updated...Status=%r \n",
        		Device->Address.Addr.Bus, Device->Address.Addr.Device, Device->Address.Addr.Function, Status));
	}
	
	return Status;
}


/**

    @param 

    @retval 

**/
BOOLEAN HpCheckHpCompatible(PCI_DEV_INFO *Device){
	if(Device->HotPlug!=NULL) return TRUE;
	else return FALSE;
}

/**

    @param 

    @retval 

**/
EFI_STATUS HpAllocateInitHpData(PCI_DEV_INFO *Device, UINT16 HpCapOffset){
	//we may process this controller as root HPC so check that.
	if(Device->HotPlug==NULL){
		Device->HotPlug=MallocZ(sizeof(PCI_HPC_INFO));
		ASSERT(Device->HotPlug);
		if(!Device->HotPlug) return EFI_OUT_OF_RESOURCES;

		//Save Current Offset of PCIE Std Compatibility Registers Block
		Device->HotPlug->HpcLocation->HpcDevicePath=Device->DevicePath;
		Device->HotPlug->HpcLocation->HpcDevicePath=Device->DevicePath;
		Device->HotPlug->HpcPciAddr.ADDR=Device->Address.ADDR;
		Device->HotPlug->HpbBridge=Device;
		Device->HotPlug->BusFound=TRUE;
		//For SHPC it must be initialized and enabled by hardware
		Device->HotPlug->HpcState=EFI_HPC_STATE_INITIALIZED|EFI_HPC_STATE_ENABLED;
		Device->HotPlug->HpcOffs=HpCapOffset;
		Device->HotPlug->Owner=Device;
	}

	return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
EFI_STATUS HpLocateProtocolSortRhpc(PCI_HOST_INFO	*PciHpcHost, PCI_DEV_INFO	*RootBridge){
	EFI_STATUS					Status;
	UINTN						ghpccnt=0, j;
	EFI_HPC_LOCATION			*ghpclst=NULL;
	EFI_DEVICE_PATH_PROTOCOL	*rbdp;
//------------------------------
	//if we were there and not found what was looking for just exit...
	if(gRhpcNotFound==TRUE) return EFI_SUCCESS;

	if(gRhpcInitProtocol==NULL){
		Status=pBS->LocateProtocol(&gEfiPciHotPlugInitProtocolGuid, NULL, &gRhpcInitProtocol);
		if(EFI_ERROR(Status)){
			if(Status == EFI_NOT_FOUND){
				gRhpcNotFound=TRUE;
				return EFI_SUCCESS;
			} else return Status;
		}
	}

	//If we here, we got gRhpcInterface
	//It MUST be only one INSTANCE in the system, even if system has multiple hosts!
	//now we must initialize internal data with of PCI_HOST_DATA information.
	if(PciHpcHost->HpInitProt==NULL) PciHpcHost->HpInitProt=gRhpcInitProtocol;
	rbdp=RootBridge->DevicePath;

	//Get Root HPC List
	Status=gRhpcInitProtocol->GetRootHpcList(gRhpcInitProtocol,&ghpccnt,&ghpclst);
	ASSERT_EFI_ERROR(Status);


	//sort out Root Hot Plug Controlers list items to where they belongs
    PCI_TRACE((TRACE_PCI,"PciBus: Get Location - HpcLocCount=%d; RbDp=[HID(%X)UID(%X)]\n",
        ghpccnt,((ACPI_HID_DEVICE_PATH*)rbdp)->HID,((ACPI_HID_DEVICE_PATH*)rbdp)->UID));
    PCI_TRACE((TRACE_PCI,"-------------------------------------------------\n"));

	for (j=0; j<ghpccnt; j++){
		if( !MemCmp(rbdp, ghpclst[j].HpcDevicePath, sizeof(ACPI_HID_DEVICE_PATH)) ){
			PCI_HPC_INFO	*rhpc;
		//-----------------------------
			rhpc=MallocZ(sizeof(PCI_HPC_INFO));
			ASSERT(rhpc);
			if(!rhpc) return EFI_OUT_OF_RESOURCES;
			//Init PCI_ROOT_HPC structure
			rhpc->Root=TRUE;
			rhpc->HpcLocation=&ghpclst[j];

            PCI_TRACE((TRACE_PCI,"HpcDP=[HID(%X)UID(%X)]; HpbDP[HID(%X)UID(%X)]\n",
            ((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpcDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpcDevicePath)->UID,
            ((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpbDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpbDevicePath)->UID));

			Status=AppendItemLst((T_ITEM_LIST*)&PciHpcHost->RhpcInitCnt,rhpc);
			ASSERT_EFI_ERROR(Status);
		}
	}
    PCI_TRACE((TRACE_PCI,"-------------------------------------------------\n"));

    return Status;
}



/**

    @param 

    @retval 

**/
EFI_STATUS HpApplyResPadding(PCI_BAR *PaddingBar, PCI_BAR *BridgeBar){

	if(PaddingBar->Length){
		PCI_TRACE((TRACE_PCI,"PciHp: Bridge BAR  BAR_TYPE=%X; Len=%lX; Gran=%lX;\n",
				BridgeBar->Type, BridgeBar->Length, BridgeBar->Gran ));
		
		if(PaddingBar->Length > BridgeBar->Length) BridgeBar->Length = PaddingBar->Length;
		if(BridgeBar->Gran < PaddingBar->Gran) BridgeBar->Gran=PaddingBar->Gran;
		BridgeBar->Type=PaddingBar->Type;
		
		PCI_TRACE((TRACE_PCI,"PciHp: Padding BAR BAR_TYPE=%X; Len=%lX; Gran=%lX;\n",
				PaddingBar->Type, PaddingBar->Length, PaddingBar->Gran ));
		
		return EFI_SUCCESS;
	}

	return EFI_UNSUPPORTED;
}


/**

    @param 

    @retval 

**/
EFI_STATUS HpApplyBusPadding(PCI_DEV_INFO *Brg, UINT8 OldMaxBus, UINT8 *MaxBusFound){
	PCI_BRG_EXT		*ext;
//-----------------------
	if(!HpCheckHpCompatible(Brg)) return EFI_SUCCESS;

	ext=(PCI_BRG_EXT*)(Brg+1);

	HpGetHpbResPadding(Brg);

	//Here we come with information about resources padding
	//Brg->Hotplug->Padd[rtBus].Length will have Bus numbers needed to pad for this bridge
	//mMaxBusFound will have number of buses actualy present behind this bridge
	//take care of buses now the rest of resources will be taken care of when
	//CalculateBridge resources will be called
	if(ext->PaddAttr==EfiPaddingPciBus){
		//EIP 19106 If 1 Bus set to padding in Setup it did not do the padding.
		if((ext->Pad[rtBus].Length!=0) && (ext->Pad[rtBus].Length > (*MaxBusFound)-ext->Res[rtBus].Base))
		{
			*MaxBusFound=OldMaxBus+(UINT8)ext->Pad[rtBus].Length;
		}
	}
	return EFI_SUCCESS;

}

/**

    @param 

    @retval 

**/
EFI_STATUS HpcUpdateLocationData(
								PCI_HOST_BRG_DATA 		*HostBrg, 
								PCI_ROOT_BRG_DATA 		*RootBrg, 
								AMI_SDL_PCI_DEV_INFO	*HpBrgSdlData,
								UINTN					HpBrgIndex,
								UINTN					HpSlotCount,
								UINTN					*HpSlotIdxArray)
{
	EFI_STATUS  				Status;
	AMI_SDL_PCI_DEV_INFO		*parent;
    PCI_DEVICE_PATH     		pcidp;
    EFI_DEVICE_PATH_PROTOCOL    *tmpdp;
    HPC_LOCATION_DATA           *locdata=NULL;
//-------------------------------------
	//If has not been allocated yet..
	if(HostBrg->HpcData==NULL){
		HostBrg->HpcData=MallocZ(sizeof(PCI_HPC_DATA));
		if(HostBrg->HpcData==NULL) {
			Status=EFI_OUT_OF_RESOURCES;
			ASSERT_EFI_ERROR(Status);
			return Status;
		}
	}
	
	//Initialize PCI Device Path variable we will not touch header...
	pcidp.Header.SubType=HW_PCI_DP;
	pcidp.Header.Type=HARDWARE_DEVICE_PATH;
    pcidp.Device=HpBrgSdlData->Device;
    pcidp.Function=HpBrgSdlData->Function;
	SET_NODE_LENGTH(&pcidp.Header,HW_PCI_DEVICE_PATH_LENGTH);
	
	//Get Parent of HP_BRG... 
	parent=&gSdlPciData->PciDevices[HpBrgSdlData->ParentIndex];
	
    //Generate the device path for the HPController\HPBus we have pci device path reserved there.
	//this will add END OF DP to the PCI_DP.
    tmpdp=DPAddNode(NULL, (EFI_DEVICE_PATH_PROTOCOL*)&pcidp);  
    if(tmpdp==NULL) {
        Status=EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    while (!parent->PciDevFlags.Bits.RootBridgeType){
        VOID    					*oldp=tmpdp;
        EFI_DEVICE_PATH_PROTOCOL    *ppcidp;
    //---------------------------------------
        //ppcidp=NULL;
        pcidp.Device=parent->Device;
        pcidp.Function=parent->Function;
        ppcidp=DPAddNode(NULL, (EFI_DEVICE_PATH_PROTOCOL*)&pcidp);
 
        if(ppcidp==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
        		
        tmpdp=DPAdd(ppcidp, tmpdp);
        pBS->FreePool(ppcidp);

        if(tmpdp==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
        
        if(oldp!=NULL)pBS->FreePool(oldp);
        
        parent=&gSdlPciData->PciDevices[parent->ParentIndex];
        
        //should not come here but who knows.. if wrong porting...
        if(parent->ParentIndex==0) break;
    }

    //Here DevicePath where points "tmpdp" Is Ready but missing root bridge DP...
    //Get some Memory ....
    locdata=MallocZ(sizeof(HPC_LOCATION_DATA));
    if(locdata==NULL) {
    	Status = EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    } else Status=EFI_SUCCESS;
                
    locdata->Owner=RootBrg;
    locdata->SdlHpBrgData=HpBrgSdlData;
    locdata->SdlHpBrgIndex=HpBrgIndex;
    
    //Now we have to generate PCI_DP down to parent Root Bridge of this HpBrgSdlData 
    //For SHPC and PCIExpress Hotplug HPC==HPB = tmpdp.
    //make a copy of generated DP into hpclocation buffer
    locdata->HpcLocation.HpcDevicePath=DPAdd(RootBrg->DevPath, tmpdp);            
    locdata->HpcLocation.HpbDevicePath=DPAdd(RootBrg->DevPath, tmpdp);            
    
    pBS->FreePool(tmpdp);

    if(locdata->HpcLocation.HpbDevicePath==NULL || locdata->HpcLocation.HpcDevicePath==NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    Status=AppendItemLst((T_ITEM_LIST*)&HostBrg->HpcData->HpclInitCnt, locdata);
    ASSERT_EFI_ERROR(Status);
    
    //Update get Padding Setup Data.
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)
    if(HpBrgSdlData->PciDevFlags.Bits.HasSetup){
    	//if Bridge has setup data just ignore what slots can have, it naturally coming from the bridge...
    	Status=AmiPciGetPciHpSetupData(&locdata->HpSetup.ArrayField, HpBrgSdlData, HpBrgIndex, FALSE);
    	if(EFI_ERROR(Status)){
    		if(Status==EFI_NOT_FOUND){
    	    	//Use SDL PCI DATA object for padding...
    	    	locdata->HpSetup.ArrayField.BusPadd=HpBrgSdlData->ResourcePadding[ptBus];
    	    	locdata->HpSetup.ArrayField.IoPadd=HpBrgSdlData->ResourcePadding[ptIo];
    	    	locdata->HpSetup.ArrayField.Io32Padd=HpBrgSdlData->ResourcePadding[ptIo32];
    	    	locdata->HpSetup.ArrayField.Mmio32Padd=HpBrgSdlData->ResourcePadding[ptMmio32];   
    	    	locdata->HpSetup.ArrayField.Mmio32PfPadd=HpBrgSdlData->ResourcePadding[ptMmio32pf];
    	    	locdata->HpSetup.ArrayField.Mmio64Padd=HpBrgSdlData->ResourcePadding[ptMmio64];
    	    	locdata->HpSetup.ArrayField.Mmio64PfPadd=HpBrgSdlData->ResourcePadding[ptMmio64pf];
    	    	Status=AmiPciGetPciHpSetupData(&locdata->HpSetup.ArrayField, HpBrgSdlData, HpBrgIndex, TRUE);
    		}  
    		if(EFI_ERROR(Status))return Status;
    	}
    } else {
    	//check how many slots come here with setup option set...
    	//if we have setup for each slot 
    	if(HpSlotCount){
    		UINTN					i, x;
    		PCI_HP_SETUP_DATA		hps;
    		AMI_SDL_PCI_DEV_INFO	*sdl;
    	//------------------------------	
    		//locdata->HpSetup at that moment initialized with all 0 
    		for(i=0; i<HpSlotCount; i++){
    			x=HpSlotIdxArray[i];
    			Status=AmiSdlFindIndexRecord(x, &sdl);
    			ASSERT_EFI_ERROR(Status);
    			Status=AmiPciGetPciHpSetupData(&hps, sdl, x, FALSE);
    	    	if(EFI_ERROR(Status)){
    	    		if(Status==EFI_NOT_FOUND){
    	    	    	//Use SDL PCI DATA object for Default padding... 
    	    	    	pBS->CopyMem(&hps,&HpBrgSdlData->ResourcePadding, sizeof(PCI_HP_SETUP_DATA));
    	    	    	Status=AmiPciGetPciHpSetupData(&hps, sdl, x, TRUE);
    	    		}  
    	    		if(EFI_ERROR(Status))return Status;
    	    	}
    	    	locdata->HpSetup.ArrayField.BusPadd+=hps.BusPadd;
    	    	locdata->HpSetup.ArrayField.IoPadd+=hps.IoPadd;
    	    	locdata->HpSetup.ArrayField.Io32Padd+=hps.Io32Padd;
    	    	locdata->HpSetup.ArrayField.Mmio32Padd+=hps.Mmio32Padd;   
    	    	locdata->HpSetup.ArrayField.Mmio32PfPadd+=hps.Mmio32PfPadd;
    	    	locdata->HpSetup.ArrayField.Mmio64Padd+=hps.Mmio64Padd;
    	    	locdata->HpSetup.ArrayField.Mmio64PfPadd+=hps.Mmio64PfPadd;
    		}
    		
    	} else {
			//Use SDL PCI DATA object for padding...
			locdata->HpSetup.ArrayField.BusPadd=HpBrgSdlData->ResourcePadding[ptBus];
			locdata->HpSetup.ArrayField.IoPadd=HpBrgSdlData->ResourcePadding[ptIo];
			locdata->HpSetup.ArrayField.Io32Padd=HpBrgSdlData->ResourcePadding[ptIo32];
			locdata->HpSetup.ArrayField.Mmio32Padd=HpBrgSdlData->ResourcePadding[ptMmio32];   
			locdata->HpSetup.ArrayField.Mmio32PfPadd=HpBrgSdlData->ResourcePadding[ptMmio32pf];
			locdata->HpSetup.ArrayField.Mmio64Padd=HpBrgSdlData->ResourcePadding[ptMmio64];
			locdata->HpSetup.ArrayField.Mmio64PfPadd=HpBrgSdlData->ResourcePadding[ptMmio64pf];
    	}
    }	
#else
	//Get global HP Padding Setup data of Aptio 4.x style setup
    Status=AmiPciGetSetupData(NULL,NULL,&locdata->HpSetup);
	ASSERT_EFI_ERROR(Status);
#endif    
    return Status;
    
}


/**
    This function find
    information and initialize an instance of PCI Root Hotplug Controller
    Initialization Protocol.

    @param HostBrg Pointer on Private Data
        structure for which PCI Hot Plug Init Protocol going to
        be initialized

    @retval EFI_SUCCESS is OK
        EFI_NOT_FOUND no Hotplug slots where found.

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS HpcFindSlots(PCI_HOST_BRG_DATA *HostBrg, PCI_ROOT_BRG_DATA *RootBrg)
{
	EFI_STATUS  			Status=EFI_NOT_FOUND;
    UINTN       			i,j, idx;
    AMI_SDL_PCI_DEV_INFO	*brgdata=NULL;
    AMI_SDL_PCI_DEV_INFO	*sdltmp=NULL;
    AMI_SDL_PCI_DEV_INFO	**sltdata;
    UINTN               	hpscnt, hpbcnt, sltcnt;
    UINTN					*sltidxbuff;
    BOOLEAN 				br;
	PCI_DEVICE_SETUP_DATA	devs;
//-------------------------

	
	PCI_TRACE((TRACE_PCI,"---------------------------------------------------------------\n"));
    PCI_TRACE((TRACE_PCI,"PciHp: Looking for HP Bridges of RB '%a'; SdlIndex 0x%X(%d)...\n",
    		RootBrg->RbAslName,RootBrg->RbSdlIndex,RootBrg->RbSdlIndex));

    //Just get Device defaults to initialize devs buffer...
    //it is usefull for PCI_SETUP_USE_APTIO_4_STYLE == 1          
    Status=AmiPciGetPciDevSetupData(&devs, NULL, 0, FALSE);
    ASSERT_EFI_ERROR(Status);

    for(i=0,hpbcnt=0; i<gSdlPciData->PciDevCount; i++){
    	sdltmp=&gSdlPciData->PciDevices[i];
    	sltdata=NULL;
    	
    	if( sdltmp->PciDevFlags.Bits.HotPlug && sdltmp->PciDevFlags.Bits.Pci2PciBridgeType){
    	    
    		//Check if It has Setup and if HP feature enabled Selectively for this particular bridge...
            //this setup options can changed by some Chipset or Platform code. 
            //if "HasSetup" flag set we will keep checking for Aptio 4.x setup style as well
            //this will create "PCI_DEV_idx_PCI" wariable but CSP/BRD code may use AmiPciGetPciVarName() function to modify it.
    		if(sdltmp->PciDevFlags.Bits.HasSetup){
    			Status=AmiPciGetPciDevSetupData(&devs, sdltmp, i, FALSE);
    			ASSERT_EFI_ERROR(Status);
    			//If HP Disabled on HPBrg level - just keep going and don't update location data...
    			if(devs.HpDisable) continue;
    		}

    		//This bridge produces HP Bus. Check if i has slots behind it...
    		if(AmiSdlCheckIfParent(i, RootBrg->RbSdlIndex)){
    			//So far... BRG with HP caps belongs to the right RB
    			//Now check if it has slot s behind it and  how many
    			//One slot is enough for the location entry in HostBrg->HpcData. 
    		    brgdata=sdltmp;
    		    
    			Status=AmiSdlPciGetChildsOfParentIndex(&sltdata,&hpscnt,i);
    			if(EFI_ERROR(Status)){
    				if(Status==EFI_NOT_FOUND){
    					PCI_TRACE((TRACE_PCI,"\t!!!WARNING!!! HP Capable Bridge has no Children SdlIndex=0x%X(%d)!!!\n", i,i));
    					continue;
    				} else return Status;
    			}
    			
    			sltidxbuff=MallocZ(sizeof(UINTN)*hpscnt);
    			if(sltidxbuff==NULL) return EFI_OUT_OF_RESOURCES;
    					
    			for(j=0,br=TRUE,sltcnt=0; j<hpscnt; j++){
    				//Try to check if there are any slots behind the bridge..
    				sdltmp=sltdata[j];
    				AmiSdlFindRecordIndex(sdltmp,&idx);
    				if(sdltmp->PciDevFlags.Bits.OnBoard){
    					//It has to be slot!
    					PCI_TRACE((TRACE_PCI,"\t!!!WARNING Found OnBoard Device @HP Bus, SdlIndex 0x%X(%d)!!!\n", idx, idx));
    					continue;
    				} else {
    					//Found a SLOT behind HP cap bridge!
    				    //this setup options can changed by some Chipset or Platform code. 
                        //if "HasSetup" flag set we will keep checking for Aptio 4.x setup style as well
    				    //this will create "PCI_DEV_idx_PCI" wariable but CSP/BRD code may use AmiPciGetPciVarName() function to modify it.
    		    		if(sdltmp->PciDevFlags.Bits.HasSetup){
    		    			Status=AmiPciGetPciDevSetupData(&devs, sdltmp, idx, FALSE);
    		    			ASSERT_EFI_ERROR(Status);
    		    		}
    					PCI_TRACE((TRACE_PCI,"\tFound Slot# 0x%X(%d), SdlIndex 0x%X(%d); HpDisable=%d; ",
    							sdltmp->Slot, sdltmp->Slot,idx,idx, devs.HpDisable));
    					
		    			//If HP Disabled on HPBrg level - just keep going and don't update location data...
		    			if(devs.HpDisable) {
		    				PCI_TRACE((TRACE_PCI,"Slot Skipped...\n"));
		    				continue;
		    			}
		    			
	    				PCI_TRACE((TRACE_PCI,"\n"));
    					
    	    			if(br){
    	    				//One HPC_LOCATION per bridge.
    	    				hpbcnt++;
    	    				br=FALSE;
    	    			}
    	    			//but if other slots has SETUP Item we need to create/update setup VARs Associated for the slot
    	    			//so keep going until the last children of a bridge in that list
    	    			//For PCIe HP it is 1 slot per bridge, but for Std HPC it could be a couple...
    	    			//If each SHPC slot has Setup Option with Resource Padding for each slot we have to add each Padding
    	    			if(sdltmp->PciDevFlags.Bits.HasSetup){
    	    				sltidxbuff[sltcnt]=idx;
    	    				sltcnt++;
    	    			}
    				}    				
    			} //For j...
				//Free memory allocated for the sdldata...
    			if(sltdata!=NULL) pBS->FreePool(sltdata);
    			//Now we have to generate PCI_DP down to this HP_BRG paren Root Bridge
				//If we found slots....
    			if(!br) {
    				PCI_TRACE((TRACE_PCI,"Updating HPData...Status="));
    				Status=HpcUpdateLocationData(HostBrg,RootBrg,brgdata, i, sltcnt, sltidxbuff);
    				PCI_TRACE((TRACE_PCI,"%r.\n", Status));
    	   			ASSERT_EFI_ERROR(Status);
    			}
    			
    		}//if AmiSdlCheckIfParent() 
    	}
    	if(EFI_ERROR(Status)&& Status!=EFI_NOT_FOUND )break;
    }
    
    //now get memory buffer to store HP
    PCI_TRACE((TRACE_PCI,"---------------------------------------------------------------\n"));
    PCI_TRACE((TRACE_PCI,"Total found %d P2P HP Bridges with HP Slots. Returning %r\n", hpbcnt, Status));

    return Status;

}


/**
    GetRootHpcList() Function of the PCI Root Hotplug Controller
    Initialization Protocol. See PI 1.1 Spec or details

**/
EFI_STATUS HpcInitGetRootHpcList(IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL      *This,
                                 OUT UINTN                              *HpcCount,
                                 OUT EFI_HPC_LOCATION                   **HpcList)
{
    EFI_STATUS          Status=EFI_SUCCESS;
    UINTN               i;
    EFI_HPC_LOCATION    *hpcl;
    EFI_HPC_LOCATION    *lp=NULL;
    PCI_HPC_DATA        *hpcdata=(PCI_HPC_DATA*)This;
//------------------------
    if(hpcdata==NULL) return EFI_INVALID_PARAMETER;

    PCI_TRACE((TRACE_PCI,"PciHb: Get Location - HpcLocCount=%d\n", hpcdata->HpcLocCount));
    PCI_TRACE((TRACE_PCI,"-------------------------------------------------\n"));

    //Allocate Buffer for HPC List
    hpcl=MallocZ(sizeof(EFI_HPC_LOCATION) * hpcdata->HpcLocCount);

    //Check conditions
    if(hpcl==NULL){
        Status=EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    //Fill data in RHPC List Data...
    for(i=0, lp=hpcl; i<hpcdata->HpcLocCount; i++, lp++){
        HPC_LOCATION_DATA   *locdata=hpcdata->HpcLocData[i];
    //----------------
        MemCpy(lp, &locdata->HpcLocation, sizeof(EFI_HPC_LOCATION));
        PCI_TRACE((TRACE_PCI,"  lp->HpCtrollerDP=[HID(%X)UID(%X)]; lp->HpBusDP[HID(%X)UID(%X)]\n",
            ((ACPI_HID_DEVICE_PATH*)lp->HpcDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)lp->HpcDevicePath)->UID,
            ((ACPI_HID_DEVICE_PATH*)lp->HpbDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)lp->HpbDevicePath)->UID));
    }
    PCI_TRACE((TRACE_PCI,"-------------------------------------------------\n"));

    *HpcCount=hpcdata->HpcLocCount;
    *HpcList=hpcl;

    return Status;
}


/**

    @param 

    @retval 

**/
EFI_STATUS FindHpcLocData(  IN EFI_DEVICE_PATH_PROTOCOL *HpcDevicePath,
                            IN PCI_HPC_DATA             *HpcData,
                            OUT HPC_LOCATION_DATA       **HpcLocData )
{
    UINTN               i;
    HPC_LOCATION_DATA   *locdata;
//------------------

    for(i=0; i< HpcData->HpcLocCount; i++){
        locdata=HpcData->HpcLocData[i];
        if(DPCmp(HpcDevicePath, locdata->HpcLocation.HpcDevicePath) == 0 ){
            *HpcLocData=locdata;
            return EFI_SUCCESS;
        }
    }

    PCI_TRACE((TRACE_PCI,"PciHp: Can't find matching HPC for DevicePath @ 0x%X \n",HpcDevicePath));

    return EFI_NOT_FOUND;

}


/**
    InitializeRootHpc() Function of the PCI Root Hotplug Controller
    Initialization Protocol. See PI 1.1 Spec or details

    @note  
  This function call will be used to overwrite default resource Padding settings.
**/
EFI_STATUS HpcInitInitializeRootHpc(IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL       *This,
                                    IN  EFI_DEVICE_PATH_PROTOCOL            *HpcDevicePath,
                                    IN  UINT64                              HpcPciAddress,
                                    IN  EFI_EVENT                         	Event, OPTIONAL
                                    OUT EFI_HPC_STATE                    	*HpcState)
{
    EFI_STATUS          Status;
    HPC_LOCATION_DATA   *locdata;
    PCI_HPC_DATA        *hpcdata=(PCI_HPC_DATA*)This;
//------------------------
    if(hpcdata==NULL) return EFI_INVALID_PARAMETER;

    //Find which exactly HPC PCI BUS Driver means..
    Status=FindHpcLocData(HpcDevicePath, hpcdata,&locdata);
    if(EFI_ERROR(Status)) return Status;

    *HpcState=(EFI_HPC_STATE_INITIALIZED|EFI_HPC_STATE_ENABLED);

    return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
VOID HpcFillDescriptor(ASLR_QWORD_ASD *Descriptor, HP_PADD_RES_TYPE PaddType, UINT64  Length){

//---------------------

    Descriptor->Hdr.HDR=0x8A;
    Descriptor->Hdr.Length=0x2B;

    Descriptor->_LEN=Length;
    if(PaddType==ptBus)Descriptor->_MAX=1;
    else{
        if(IsPowerOfTwo(Length))Descriptor->_MAX=Length-1;
        else Descriptor->_MAX=1;
    }

    Descriptor->GFlags._MIF=1;
    Descriptor->GFlags._MAF=1;
    Descriptor->TFlags.TFLAGS=0;

    switch(PaddType){
        case ptBus:
            Descriptor->Type=ASLRV_SPC_TYPE_BUS;
            break;
        case ptIo:
            Descriptor->Type=ASLRV_SPC_TYPE_IO;
            break;
        case ptMmio32:
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->_GRA=32;
            break;
        case ptMmio32pf:
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->TFlags.TFLAGS=0x06;
            Descriptor->_GRA=32;
            break;
        case ptMmio64:   //4
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->_GRA=64;
            break;
        case ptMmio64pf:  //5
            Descriptor->Type=ASLRV_SPC_TYPE_MEM;
            Descriptor->TFlags.TFLAGS=0x06;
            Descriptor->_GRA=64;
            break;
    }

}


/**

    @param 

    @retval 

**/
EFI_STATUS HpcGetResourcePadding(IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL *This,
                                 IN  EFI_DEVICE_PATH_PROTOCOL       *HpcDevicePath,
                                 IN  UINT64                         HpcPciAddress,
                                 OUT EFI_HPC_STATE                  *HpcState,
                                 OUT VOID                           **Padding,
                                 OUT EFI_HPC_PADDING_ATTRIBUTES     *Attributes)
{
    EFI_STATUS          Status;
    HPC_LOCATION_DATA   *locdata;
    UINTN               i, cnt;
    ASLR_QWORD_ASD      *pd, *qw;
    PCI_HPC_DATA        *hpcdata=(PCI_HPC_DATA*)This;
//------------------------
    if(hpcdata==NULL) return EFI_INVALID_PARAMETER;

    //Find which exactly HPC PCI BUS Driver means..
    Status=FindHpcLocData(HpcDevicePath, hpcdata,&locdata);
    if(EFI_ERROR(Status)) return Status;

    //Calculate how many ACPI_QW_DESC we need for Padding Info.
    for(i=0, cnt=0; i<ptMaxType; i++){
    	if(i==ptIo32) continue;
        if(locdata->HpSetup.ARRAY[i]!=0) cnt++;
    }

    //Get memory for it.
    pd=MallocZ(sizeof(ASLR_QWORD_ASD)*cnt+sizeof(ASLR_EndTag));
    if (pd==NULL) {
        Status=EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    //fill out QW Resopurce Descriptors for resource requirements.
    for(i=0, qw=pd; i<ptMaxType; i++){
    	if(i==ptIo32)continue;
        if(locdata->HpSetup.ARRAY[i]!=0){
            HpcFillDescriptor(qw, (HP_PADD_RES_TYPE)i, locdata->HpSetup.ARRAY[i]);
            qw++;
        }
    }

	((ASLR_EndTag*)qw)->Hdr.HDR=ASLV_END_TAG_HDR;
	((ASLR_EndTag*)qw)->Chsum=0;

    *Padding=pd;
    *HpcState=(EFI_HPC_STATE_INITIALIZED|EFI_HPC_STATE_ENABLED);
    *Attributes=EfiPaddingPciBus;

    return Status;
}


/**
    This function will collect all information HP related
    information and initialize an instance of PCI Root Hotplug Controller
    Initialization Protocol.

    @param HostBrg Pointer on Private Data
        structure for which PCI Hot Plug Init Protocol going to
        be initialized

    @retval EFI_SUCCESS is OK
        EFI_NOT_FOUND no Hotplug slots where found.

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS HpcInstallHpProtocol(PCI_HOST_BRG_DATA *HostBrg)
{
    EFI_STATUS          Status;
//---------------------------

    Status=AmiSdlInitBoardInfo();
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

	PCI_TRACE((TRACE_PCI,"PciHP: HpcInitProtocol()... "));

    if( !HpcCheckHpCompatibleHost(HostBrg)) { 
    	PCI_TRACE((TRACE_PCI,"System is NOT HP CAPABLE\n"));
    	return EFI_SUCCESS;
    }    
    
    //So far so good... Init Protocol Instance Functions
    HostBrg->HpcData->RootHotplugProtocol.GetRootHpcList=HpcInitGetRootHpcList;
    HostBrg->HpcData->RootHotplugProtocol.InitializeRootHpc=HpcInitInitializeRootHpc;
    HostBrg->HpcData->RootHotplugProtocol.GetResourcePadding=HpcGetResourcePadding;


    Status=pBS->InstallMultipleProtocolInterfaces(
        &HostBrg->HbHandle,		//it is NOT NULL now
        &gEfiPciHotPlugInitProtocolGuid, &HostBrg->HpcData->RootHotplugProtocol,
	    NULL);									//terminator

	PCI_TRACE((TRACE_PCI,"Status=%r\n", Status));

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

