//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PciHostBridge.c
    AMI's Native Root Bridge Protocol Implementation

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <PciHostBridge.h>
#include <Protocol/DevicePath.h>
#include <Protocol/AmiPciSmmHandoff.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/AmiPciSmmHandoff.h>
#include <Library/AmiPciBusLib.h>
#include <Library/AmiPciHotPlugLib.h>
#include <Library/AmiSdlLib.h>
#include <Library/PciAccessCspLib.h>
#include <Library/PcdLib.h>




///
/// Global Vars and Constants been used 
/// 
EFI_HANDLE                   gAmiPciSmmHandoffHandle=NULL; //just in case remembr it...
AMI_PCI_SMM_HANDOFF_PROTOCOL gAmiPciSmmHandoff ={0,NULL};


///Forward Declarations
EFI_STATUS EFIAPI HbResAllocNotifyPhase (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE        Phase
);

EFI_STATUS EFIAPI HbResAllocGetNextRootBridge (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN OUT EFI_HANDLE                                       *RootBridgeHandle
);

EFI_STATUS EFIAPI HbResAllocGetAllocAttributes (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    OUT UINT64                                              *Attributes
);

EFI_STATUS EFIAPI HbResAllocStartBusEnumeration (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    OUT VOID                                                **Configuration
);

EFI_STATUS EFIAPI HbResAllocSetBusNumbers (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    IN VOID                                                 *Configuration
);

EFI_STATUS EFIAPI HbResAllocSubmitResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    IN VOID                                                 *Configuration
);

EFI_STATUS EFIAPI HbResAllocGetProposedResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    OUT VOID                                                **Configuration
);

EFI_STATUS EFIAPI HbResAllocPreprocessController (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *This,
    IN EFI_HANDLE                                           RootBridgeHandle,
    IN EFI_PCI_CONFIGURATION_ADDRESS                        PciAddress,
    IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE         Phase
);

///Function Prototypes for PciRootBridgeIo
EFI_STATUS EFIAPI PollMem (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN  UINT64			Address,
	IN  UINT64			Mask,
	IN  UINT64			Value,
	IN  UINT64			Delay,
	OUT UINT64			*Result);

EFI_STATUS EFIAPI PollIo (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN  UINT64			Address,
	IN  UINT64			Mask,
	IN  UINT64			Value,
	IN  UINT64			Delay,
	OUT UINT64			*Result);

EFI_STATUS EFIAPI PciMemRead (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN     UINT64		Address,
	IN     UINTN		Count,
	IN OUT VOID			*Buffer);

EFI_STATUS EFIAPI PciMemWrite (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN     UINT64		Address,
	IN     UINTN		Count,
	IN OUT VOID			*Buffer);

EFI_STATUS EFIAPI PciIoRead (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN     UINT64		UserAddress,
	IN     UINTN		Count,
	IN OUT VOID			*UserBuffer);

EFI_STATUS EFIAPI PciIoWrite (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN     UINT64		UserAddress,
	IN     UINTN		Count,
	IN OUT VOID			*UserBuffer);

EFI_STATUS EFIAPI PciCopyMem (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN     UINT64		DestAddress,
	IN     UINT64		SrcAddress,
	IN     UINTN		Count);

EFI_STATUS EFIAPI PciRead (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
	IN     UINT64		Address,
	IN     UINTN		Count,
	IN OUT VOID			*Buffer);

EFI_STATUS EFIAPI PciWrite (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
	IN     UINT64		Address,
	IN     UINTN		Count,
	IN OUT VOID			*Buffer);

EFI_STATUS EFIAPI Map (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL            *This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
	IN     VOID					*HostAddress,
	IN OUT UINTN				*NumberOfBytes,
	OUT    EFI_PHYSICAL_ADDRESS	*DeviceAddress,
	OUT    VOID					**Mapping);

EFI_STATUS EFIAPI Unmap (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN  VOID			*Mapping);

EFI_STATUS EFIAPI AllocateBuffer (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN  EFI_ALLOCATE_TYPE	Type,
	IN  EFI_MEMORY_TYPE		MemoryType,
	IN  UINTN				Pages,
	OUT VOID				**HostAddress,
	IN  UINT64				Attributes);

EFI_STATUS EFIAPI FreeBuffer (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN  UINTN			Pages,
	OUT VOID			*HostAddress);

EFI_STATUS EFIAPI Flush (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This);

EFI_STATUS EFIAPI GetAttributes (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	OUT UINT64			*Supported,
	OUT UINT64			*Attributes);

EFI_STATUS EFIAPI SetAttributes (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN     UINT64		Attributes,
	IN OUT UINT64		*ResourceBase,
	IN OUT UINT64		*ResourceLength);

EFI_STATUS EFIAPI Configuration (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	OUT    VOID			**Resources);

EFI_DEVICE_IO_INTERFACE * ConstructDeviceIoProtocol(
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*RootBridgeIo,
	IN EFI_DEVICE_PATH_PROTOCOL *Path);

//**********************************************************************
//Variables

/**
    Array of PCI Host Bridge Data Sructures.
    Initial data point for PCI Subsystem.


**/
PCI_HOST_BRG_DATA		*gHostBrgPrivateData=NULL;
UINTN					gHostBrgCount=0;


/**
    Global Setup Variable to get the setup settings pointer.

    @note  PCI_SETUP_DATA

**/
PCI_COMMON_SETUP_DATA    *gPciCommonSetup;

UINT64		TmpBot[raMaxRes]={0,0,0,0};
UINT64		TmpTop[raMaxRes]={0,0,0,0};

//**********************************************************************
//Function Prototypes needed forward declaration
//**********************************************************************
EFI_STATUS EFIAPI ConvertMemoryMap(EFI_HANDLE	ImgHandle, EFI_HANDLE	CntrHandle OPTIONAL);
VOID EFIAPI RbReadyToBoot(IN EFI_EVENT Event,	IN VOID	*Context);

AMI_BOARD_INIT_PROTOCOL *gPciInitProtocol=NULL;


///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///PCI Root Bridge Driver Entry Point
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**
    This function is the entry point for PCI Root Bridge Driver. 
    Since PCI Root Bridge Driver follows EFI 1.1 driver model in it's entry 
    point it will initialize some global data and install
    EFI_DRIVER_BINDING_PROTOCOL. 

        
    @param ImageHandle Image handle.
    @param SystemTable Pointer to the EFI system table.

    @retval EFI_STATUS
        EFI_SUCCESS         When everything is going on fine!
        EFI_NOT_FOUND       When something required is not found!
        EFI_DEVICE_ERROR    When the device is not responding!

    @note  
  Entry Points are used to locate or install protocol interfaces and
 	notification events. 

**/
EFI_STATUS EFIAPI InitializePCIHostBridge(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS					Status;
	UINTN						i,j,rbcnt;
	PCI_HOST_BRG_DATA			*hb;
	PCI_ROOT_BRG_DATA			*rb;
    EFI_EVENT                   ReadyToBootEvent=NULL;
    AMI_SDL_PCI_DEV_INFO		**HbSdlData=NULL;
    AMI_SDL_PCI_DEV_INFO		**RbSdlData=NULL;
    AMI_SDL_PCI_DEV_INFO		*sdldata, *nextrb;
	ACPI_HID_DEVICE_PATH		*rbdp;
    ASLR_QWORD_ASD      		*busrd;

//-----------------------------------------
//DEBUG
//EFI_DEADLOOP();
//DEBUG
	//Init Lib globals...
	InitAmiLib(ImageHandle,SystemTable);
    PROGRESS_CODE(DXE_NB_HB_INIT);

	//Print PCI Bus Driver Version
    PCI_TRACE((TRACE_PCI,"\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
    PCI_TRACE((TRACE_PCI,"PciHostBrg: Initializing... AMI PCI Module Version %X.%d.%d\n", PCI_BUS_MAJOR_VER, PCI_BUS_MINOR_VER, PCI_BUS_REVISION));
    PCI_TRACE((TRACE_PCI,"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));

    Status=AmiSdlPciGetHostBridges(&HbSdlData,&gHostBrgCount);
    //It has to be at least 1(one) HostBridgeType record in SDL OUTPUT data!
    PCI_TRACE((TRACE_PCI,"\n HB: Got HostBridge Info %r; HostBrgSdlData @ 0x%X; gHostBrgCount=%d.\n",Status,HbSdlData,gHostBrgCount));
    ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

    //Reserve CSP MMIO and IO reported in AMI_SDL_PCI_DATA.CspResCount trough GCD services.
	//If we here gSdlPciData must be initialized get
    PCI_TRACE((TRACE_PCI," HB: Reserve CSP Resources( ImageHandle=0x%X)\n",ImageHandle));
    PCI_TRACE((TRACE_PCI,"---------------------------------------------------------------------\n"));
	Status=AmiSdlReserveCspResources(ImageHandle);
    PCI_TRACE((TRACE_PCI,"---------------------------------------------------------------------\n"));
    PCI_TRACE((TRACE_PCI," HB: Reserve CSP Resources Status=%r\n",Status));

	ASSERT_EFI_ERROR(Status);
	//FAILURE is operation above is not that critical
	//some driver that was launched before us reserved some...
	if(EFI_ERROR(Status)) Status=EFI_SUCCESS;

	//Zero-out Setup Data Buffer
	gPciCommonSetup=MallocZ(sizeof(PCI_COMMON_SETUP_DATA));

	//Get memory for Host Private data structure
	gHostBrgPrivateData=MallocZ(sizeof(PCI_HOST_BRG_DATA)*gHostBrgCount);
    PCI_TRACE((TRACE_PCI," HB: Allocate HB Private Data ...@ 0x%X; ", gHostBrgPrivateData));
	if(gHostBrgPrivateData==NULL) {
	    PCI_TRACE((TRACE_PCI," EFI_OUT_OF_RESOURCES\n"));
		ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
		return EFI_OUT_OF_RESOURCES;
	}
    PCI_TRACE((TRACE_PCI," EFI_SUCCES\n"));

    //Get Setup Data it has some Root Brg related features
#if (PCI_SETUP_USE_APTIO_4_STYLE == 0)	
    Status = AmiPciGetCommonSetupData(gPciCommonSetup);
#else
    Status = AmiPciGetSetupData(NULL, gPciCommonSetup, NULL);
#endif	
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

	///Update GCD resource map all non existent memory will become MMIO.
	Status=ConvertMemoryMap(ImageHandle,NULL);
	
	//Handle ERROR here...
    PCI_TRACE((TRACE_PCI," HB: Convert All Unallocated Memory to MMIO - %r;\n", Status));
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;

    if(gPciInitProtocol==NULL){
    	//Get AMI BoardINit Protocol to call initialization functions of PCI Devices
    	Status=pBS->LocateProtocol( &gAmiBoardPciInitProtocolGuid,	//*Protocol,
    								NULL,							//*Registration OPTIONAL,
    								(VOID**)&gPciInitProtocol);		//*Interface 

    	//It must be here otherwise it will NO INIT functions for all Pci Devices.
    	PCI_TRACE((TRACE_PCI,"HB: Locate AmiBoardPciInitProtocol. Status=%r.\n", Status));
    	//ASSERT_EFI_ERROR(Status);
    }
	
	//---------------------------------------------------------
	///Update a basic info of gHostBrgPrivateData  and Root Data
	for(i=0; i<gHostBrgCount; i++){
		hb=&gHostBrgPrivateData[i];
		sdldata=HbSdlData[i];
		
		Status=AmiSdlFindRecordIndex(sdldata,&hb->HbSdlIndex);
		PCI_TRACE((TRACE_PCI," HB: Getting HostBrg[%d] SDLRecordIndex=%d; Status=%r.\n",i,hb->HbSdlIndex,Status));
		ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return Status;
		
		//Update private data
		hb->ImageHandle=ImageHandle;
		hb->HbSdlData=sdldata;
		hb->PciInitProtocol=gPciInitProtocol;
		hb->Signature=AMI_HB_SIG;
		//If device type HostBridgeType Attributes field contains Allocation Attributes.
		hb->AllocAttrib=sdldata->Attributes;

		//Init Prtocol Functions
		hb->ResAllocProtocol.NotifyPhase=HbResAllocNotifyPhase;
		hb->ResAllocProtocol.GetNextRootBridge=HbResAllocGetNextRootBridge;
		hb->ResAllocProtocol.GetAllocAttributes=HbResAllocGetAllocAttributes;
	  	hb->ResAllocProtocol.StartBusEnumeration=HbResAllocStartBusEnumeration;
		hb->ResAllocProtocol.SetBusNumbers=HbResAllocSetBusNumbers;
		hb->ResAllocProtocol.SubmitResources=HbResAllocSubmitResources;
		hb->ResAllocProtocol.GetProposedResources=HbResAllocGetProposedResources;
		hb->ResAllocProtocol.PreprocessController=HbResAllocPreprocessController;
		hb->AllocPhase=-1;

		if( gPciCommonSetup->Above4gDecode == 0 ) hb->AllocAttrib &= (~(EFI_PCI_HOST_BRIDGE_MEM64_DECODE));

		//Call PCI Init Routine to do Chipset/Platform Specific Basic HW INIT....
		Status=AmiPciLibLaunchInitRoutine(hb, isHbBasicInit, itHost, hb, NULL, NULL, NULL);
		if(EFI_ERROR(Status)){
			if(Status==EFI_UNSUPPORTED){
				Status=EFI_SUCCESS;
			} else {
				ASSERT_EFI_ERROR(Status);
				return Status;
			}
		}

		//Install HB res Alloc Protocol on this HOST BRG
		Status=pBS->InstallMultipleProtocolInterfaces(
			&hb->HbHandle,		//it was NULL so it going to be a new handle
			&gEfiPciHostBridgeResourceAllocationProtocolGuid, &hb->ResAllocProtocol,//RbIo Protocol GUID - I/F pare
			NULL);									//terminator
	    PCI_TRACE((TRACE_PCI," HB [%d] Install Res Alloc Protocol: Status=%r; Setup4GDecode=%d; AllocAttributes=0x%X.\n",
	    		i, Status,gPciCommonSetup->Above4gDecode,hb->AllocAttrib));

		//Now Get THIS HOST Root Bridges..
		Status=AmiSdlPciGetChildsOfParentIndex(&RbSdlData, &rbcnt, hb->HbSdlIndex);
	    ASSERT_EFI_ERROR(Status);

	    //Arrange data it might come not ordered
	    Status=SortRbSdlData(&RbSdlData, &rbcnt);

	    //Print what we got there
	    PCI_TRACE((TRACE_PCI," HB [%d] Get RootBridges: Status=%r; RbCount=%d; RbSdlData @ 0x%X.\n",i, Status, rbcnt, RbSdlData));
	    ASSERT_EFI_ERROR(Status);
	    if(EFI_ERROR(Status)) return Status;

	    ///Allocate Memory for RB Private Data...
	    //hb->RootBridges=MallocZ(sizeof(PCI_ROOT_BRG_DATA)*hb->RootBridgeCount);
	    //if(hb->RootBridges==NULL){
	    //	ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
	    //	return EFI_OUT_OF_RESOURCES;
	    //}

	    //Now we get memory, do some RootBridge basic initialization..
	    for(j=0; j<rbcnt; j++){
	    	INT16 		busshift;
	    //-------------------------	
	    	//Allocate Memory for RB Private Data...
	    	rb=MallocZ(sizeof(PCI_ROOT_BRG_DATA));
		    if(rb==NULL){ //ASSERT if no memory
		    	ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
		    	return EFI_OUT_OF_RESOURCES;
		    }
		    
	    	Status=AppendItemLst((T_ITEM_LIST*)&hb->RbInitCnt, rb);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
	    	sdldata=RbSdlData[j];

	    	//Initialize Root Bridge Private Data...
	    	rb->Owner=hb;
            rb->ImageHandle=ImageHandle;
	    	rb->RbSdlData=sdldata;
	    	rb->PciInitProtocol=gPciInitProtocol;
	    	rb->Signature=AMI_RB_SIG;
			Status=AmiSdlFindRecordIndex(sdldata,&rb->RbSdlIndex);
			PCI_TRACE((TRACE_PCI,"   RB[%d] - SDLRecordIndex=%d; Status=%r.\n",j,rb->RbSdlIndex,Status));
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;

			
			//for RootBridgeType Attributes field corresponds to Supported Attributes of EFI_PCI_ROOT_BRIDGE_IO Attributes
			rb->Supports=sdldata->Attributes;
			rb->RbAslName=(CHAR8*)&sdldata->AslName[0]; 
			//Just in case if during porting this Attribute was not set.
			if(hb->AllocAttrib & EFI_PCI_HOST_BRIDGE_MEM64_DECODE) rb->Supports |= EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE;

			//Display what we got so far
			PCI_TRACE((TRACE_PCI,"   RB[%d] - Setup4GDecode=%d; AttributesSupported=0x%X; ASL Name='%a'.\n",
					j,gPciCommonSetup->Above4gDecode,rb->Supports, rb->RbAslName));

			
			//Check with CSP/BSP code if this Root Node active and present....
			Status=AmiPciLibLaunchInitRoutine(rb, isRbCheckPresence, itRoot, rb, &j, NULL, NULL);
			if(EFI_ERROR(Status)){
				if(Status==EFI_UNSUPPORTED){
					Status=EFI_SUCCESS;
				} else {
					ASSERT_EFI_ERROR(Status);
					return Status;
				}
			}
			//This node - not present, so just continue...

			if(rb->NotPresent){
				PCI_TRACE((TRACE_PCI,"   RB[%d] - NOT Present ! Skipping...\n",j));
				continue;
			}
			PCI_TRACE((TRACE_PCI,"   RB[%d] - Present ! Initializing...\n",j));
			
            //5. Fill out Protocol Interface for RB Protocol Member functions
            //it might be that 
            rb->RbIoProtocol.ParentHandle=hb->HbHandle;
            rb->RbIoProtocol.PollMem=PollMem;
            rb->RbIoProtocol.PollIo=PollIo;
            rb->RbIoProtocol.Mem.Read=PciMemRead;
            rb->RbIoProtocol.Mem.Write=PciMemWrite;
            rb->RbIoProtocol.Io.Read=PciIoRead;
            rb->RbIoProtocol.Io.Write=PciIoWrite;
            rb->RbIoProtocol.Pci.Read=PciRead;
            rb->RbIoProtocol.Pci.Write=PciWrite;
            rb->RbIoProtocol.CopyMem=PciCopyMem;
            rb->RbIoProtocol.Map=Map;
            rb->RbIoProtocol.Unmap=Unmap;
            rb->RbIoProtocol.AllocateBuffer=AllocateBuffer;
            rb->RbIoProtocol.FreeBuffer=FreeBuffer;
            rb->RbIoProtocol.Flush=Flush;
            rb->RbIoProtocol.GetAttributes=GetAttributes;
            rb->RbIoProtocol.SetAttributes=SetAttributes;
            rb->RbIoProtocol.Configuration=Configuration;
            rb->RbIoProtocol.SegmentNumber=(UINT32)sdldata->PciSegment;
			
			//Create Bus resource Descriptor for this Root Bridge...
	        //1.Get memory for Bus Resource Descriptor
	        busrd=MallocZ(sizeof(ASLR_QWORD_ASD));
	        //2.Fill Bus Resource descriptor fields
	        busrd->Hdr.HDR=0x8A;
	        busrd->Hdr.Length=0x2B;
	        busrd->Type=ASLRV_SPC_TYPE_BUS;
	        busrd->_GRA=1;
	        //3.Determine Bus ranges And range Length used by this RB...
	        //for RootBridgeType Bus Number mandatory and it signifies Base Bus Number.
	        busrd->_MIN=sdldata->Bus;
	        if(j<rbcnt-1) nextrb=RbSdlData[j+1];
	        else nextrb=NULL;
	        
	        if(nextrb==NULL) busrd->_MAX=0xFF;
	        else busrd->_MAX=nextrb->Bus-1;
	        busrd->_LEN = busrd->_MAX - busrd->_MIN + 1;

			PCI_TRACE((TRACE_PCI,"   RB[%d] - Initial Bus Range _MIN=0x%X; _LEN=0x%X; _MAX=0x%X\n",
							j, busrd->_MIN, busrd->_LEN, busrd->_MAX));

			busshift=(INT16)busrd->_MIN;

			
			//Check with CSP/BSP code if this Root Bus base need to be adjusted...
			Status=AmiPciLibLaunchInitRoutine(rb, isRbBusUpdate, itRoot, rb, busrd, NULL, NULL);
			if(EFI_ERROR(Status)){
				if(Status==EFI_UNSUPPORTED){
					Status=EFI_SUCCESS;
				} else {
					ASSERT_EFI_ERROR(Status);
					return Status;
				}
			}
			
			busshift=(INT16)busrd->_MIN - busshift;
	        
			PCI_TRACE((TRACE_PCI,"   RB[%d] - Updated Bus Range: _MIN=0x%X; _LEN=0x%X; _MAX=0x%X; BusShift=%d.\n",
							j, busrd->_MIN, busrd->_LEN, busrd->_MAX, busshift));


			//3.a Now calculate bus adjustment value for fixed buses if ROOT bus range changed.
			//this value should add to the actual fixed bus number value to compensate bus layout shift.
			Status=AmiPciBusShift(sdldata, &busshift, i, j, TRUE);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
			
			//4.Add newly created BUS resources to RB resource list
			Status=AppendItemLst((T_ITEM_LIST*)&rb->ResInitCnt,busrd);
			//Display what we got so far
			PCI_TRACE((TRACE_PCI,"   RB[%d] - ResDsc TBUS:_MIN=0x%X; _MAX=0x%X, _LEN=0x%X, _GRA=0x%X; AppendRD=%r.\n",
					j, busrd->_MIN, busrd->_MAX, busrd->_LEN,busrd->_GRA, Status));
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;

			//4. Create RB DevicePath Instance...
			rbdp=MallocZ(sizeof(ACPI_HID_DEVICE_PATH)+sizeof(EFI_DEVICE_PATH_PROTOCOL));
			ASSERT(rbdp);
			if(!rbdp)return EFI_OUT_OF_RESOURCES;

			rb->DevPath=&rbdp->Header;
			rbdp->Header.Type=ACPI_DEVICE_PATH;
			rbdp->Header.SubType=ACPI_DP;
			SET_NODE_LENGTH(&rbdp->Header,ACPI_DEVICE_PATH_LENGTH);
			rbdp->HID=EISA_PNP_ID(0x0A03);
			rbdp->UID=(UINT32)j;

			rbdp++;
			rbdp->Header.Type=END_DEVICE_PATH;
			rbdp->Header.SubType=END_ENTIRE_SUBTYPE;
			SET_NODE_LENGTH(&rbdp->Header,END_DEVICE_PATH_LENGTH);

			//6. And finally install instance of PCI_ROOT_BRIDGE_PROTOCOL
			Status=pBS->InstallMultipleProtocolInterfaces(
				&rb->RbHandle,							//it was NULL so it going to be a new handle
				&gEfiPciRootBridgeIoProtocolGuid, &rb->RbIoProtocol, 	//RbIo Protocol GUID - I/F pare
				&gEfiDevicePathProtocolGuid, rb->DevPath, 	//DevPath Protocol GUID - I/F pare
				NULL);									//terminator
			//Display what we done so far...
			PCI_TRACE((TRACE_PCI,"   RB[%d] - Installing RB_IO and DP Protocol Status = %r.\n", j, Status));

			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
			
			//Update HB/RB data for hotplug if any... 
			//Host and Root bridges already mapped and Root DP is valid at that point.
			//So find HP Slots that belongs to 
			if(gPciCommonSetup->HotPlug == 1 ){
				Status=HpcFindSlots(hb, rb);
				//Some Particular ROOT Bridge might not have any HP Bridges
				//EFI_NOT_FOUND is a normal condition here, just keep going
				if(Status==EFI_NOT_FOUND)continue;
				
				//Any other ERROR type is critical ... hung here.
				ASSERT_EFI_ERROR(Status);
				if(EFI_ERROR(Status)) return Status;
			}
	    } //Root bridge loop

	    //Here we must initialize and install PciHotPlugControllerInit Protocol
	    //Update HP Int Functions in AmiPciHpLib to work with
		//6. If System Supports Hot Plug and option enabled in Setup Initialize and install ROOT_HP_INIT_PROTOCOL
	    if( HpcCheckHpCompatibleHost(hb)){
        	//PCI_HOTPLUG_SETUP_DATA hpSetup;
        	//Status=AmiPciGetSetupData(NULL, NULL, &hpSetup);
        	Status = HpcInstallHpProtocol(hb);
        	ASSERT_EFI_ERROR(Status);
        	if(EFI_ERROR(Status)) return Status;
        }//if Setup allows 

	} //Host Bridge loop

	//Once all RB IO installed Install AmmiPciSmmHandoff Protocol to convey collected data
	//to SMM RbIo Driver...
	if(gAmiPciSmmHandoffHandle==NULL){
        gAmiPciSmmHandoff.HostCnt=gHostBrgCount;
        gAmiPciSmmHandoff.AmiHostData=&gHostBrgPrivateData[0];
        
        Status=pBS->InstallMultipleProtocolInterfaces(
            &gAmiPciSmmHandoffHandle,                          //it going to be a new handle
            &gAmiPciSmmHandoffProtocolGuid, &gAmiPciSmmHandoff,    //PCI SMM Handoff Protocol GUID - I/F pare
            NULL);
        //Report Status:
        PCI_TRACE((TRACE_PCI,"HB: Install SMM Handoff Protocol HostCnt=%d; Handle=0x%X; Status=%r.\n", 
                gAmiPciSmmHandoff.HostCnt, gAmiPciSmmHandoffHandle, Status));
	}    
    
	//Now set up a ready to boot event 
    Status = CreateReadyToBootEvent(TPL_CALLBACK,
                                    RbReadyToBoot,
                                    NULL,
                                    &ReadyToBootEvent);
    PCI_TRACE((TRACE_PCI,"HB: Create ReadyToBootEvent - returned %r\n", Status));
	ASSERT_EFI_ERROR(Status);
    
	//Call Chipset Hook aftert we have initialized Pci Host bridge.
	Status=HbCspBasicChipsetInit(&gHostBrgPrivateData[0],gHostBrgCount);
	ASSERT_EFI_ERROR(Status);


	PCI_TRACE((TRACE_PCI,"\n---------------------------------------------------------------------\n"));
    PCI_TRACE((TRACE_PCI,"PciHostBrg: END Initialization!!!\n"));
	PCI_TRACE((TRACE_PCI,"---------------------------------------------------------------------\n\n"));

	return Status;
}


//**********************************************************************
//HelperFunctions
//**********************************************************************
//#if AMI_HOTPLUG_INIT_SUPPORT == 1

//#endif  //AMI_HOTPLUG_INIT_SUPPORT == 1

EFI_STATUS EFIAPI FindRbBusData(PCI_ROOT_BRG_DATA *RootBrg, ASLR_QWORD_ASD **ResDsc){
    UINTN           i;
    ASLR_QWORD_ASD  *res;
//----------------------------

   //Init return walue with NOT_FOUND result
    *ResDsc=NULL;    

    for(i=0;i<RootBrg->ResCount;i++){

        res=RootBrg->RbRes[i];
        if(res->Type== ASLRV_SPC_TYPE_BUS){
            *ResDsc=res;
            return EFI_SUCCESS;
        }
    }//for k
    
    return EFI_NOT_FOUND;
}


EFI_STATUS EFIAPI ClaimMmioResources(PCI_ROOT_BRG_DATA *CurRootBrg, PCI_ROOT_BRG_DATA *NextRootBrg, ACPI_RES_TYPE ResType){
    EFI_STATUS                      Status;
    DXE_SERVICES                    *dxe=NULL;
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR mem;
    UINT64                          top, bot, tmp, ct, cb;    
    ACPI_RES_DATA                   *car, *nar;
//--------------------------------
	// Get GCD's Memory Space and IO Space Maps
	Status=LibGetDxeSvcTbl(&dxe);
    if(EFI_ERROR(Status)){
        PCI_TRACE((TRACE_PCI,"PciRB: Fail to Get DXE Services Table Pointer - %r\n", Status));
        ASSERT_EFI_ERROR(Status);
        return EFI_DEVICE_ERROR;
    }
    
    if(ResType==raMmio32)car=&CurRootBrg->AcpiRbRes[raMmio32];
    else car=&CurRootBrg->AcpiRbRes[raMmio64];

    if(car->Len==0) return EFI_NOT_FOUND;

	// Use MMIO resources as-is if AllocType is set to EfiGcdAllocateAddress.
	if(car->AllocType == EfiGcdAllocateAddress) return EFI_SUCCESS;

    if(NextRootBrg!=NULL) {
        if(ResType==raMmio32)nar=&NextRootBrg->AcpiRbRes[raMmio32];
        else nar=&NextRootBrg->AcpiRbRes[raMmio64];
    }else nar=NULL;


    if( nar==NULL ) {
        if(ResType==raMmio32){
            top=MAX_PCI_MMIO32;
            bot=MIN_PCI_MMIO32;
        } else {
            top=MAX_PCI_MMIO64;
            bot=MIN_PCI_MMIO64;
        }
    } else {
        if(car->AllocType<=EfiGcdAllocateAddress){
            top=nar->Min-1;
            bot=car->Min;
        } else {
            top=car->Max;
            if(ResType==raMmio32) bot=MIN_PCI_MMIO32;
            else bot=MIN_PCI_MMIO64;
        }
    }
	
	if( TmpTop[ResType] != 0 ) bot=TmpTop[ResType]+1;
//	if( TmpBot[ResType] != 0 ) bot=TmpBot[ResType];
	
    cb=car->Min;
    ct=car->Max;

    PCI_TRACE((TRACE_PCI,"  Ajusting ACPI MMIO MAP [0x%lX...[0x%lX...0x%lX]...0x%lX]\n",bot,cb,ct,top));
    
    PCI_TRACE((TRACE_PCI,"    Searching from "));

    if(car->AllocType<=EfiGcdAllocateAddress) {
        tmp=ct+1;
        PCI_TRACE((TRACE_PCI,"0x%lX ==> UP.....to 0x%lX\n", tmp, top));
    } else {
        tmp=cb-1;
        PCI_TRACE((TRACE_PCI,"0x%lX <== DOWN...to 0x%lX\n", tmp, bot));
    }
    
    //All IO map's EfiGcdIoTypeNonExistent IO type, must be converted to EfiGcdIoTypeIo    
    while(TRUE){
        Status=dxe->GetMemorySpaceDescriptor(tmp,&mem);
        
        //Check if we reach end of GCD Space map... 
        if(EFI_ERROR(Status)) break;
        
        //Check if we hit another RB 
        if(mem.DeviceHandle!=NULL && mem.DeviceHandle!=CurRootBrg->RbHandle) break;

        //Check if we hit system memory...
        if(mem.GcdMemoryType != EfiGcdMemoryTypeMemoryMappedIo) break;        

        PCI_TRACE((TRACE_PCI,"    MemDsc: BAS=0x%lX; LEN=0x%lX; TYP=%d; IH=0x%X; DX=0x%X; %r\n",
        mem.BaseAddress, mem.Length, mem.GcdMemoryType, mem.ImageHandle, mem.DeviceHandle, Status));

        if(car->AllocType<=EfiGcdAllocateAddress){
           ct=mem.BaseAddress+mem.Length-1;   
        } else {
            cb=mem.BaseAddress;
        }                

        if(car->AllocType<=EfiGcdAllocateAddress){
            //if we are searching bottom to top...
            if(ct>=top){
                ct=top;
                break;
            }
        } else {
            //if we are searching top to bottom...
            if(cb<=bot){
                cb=bot;
                break;
            }
        }
        
        if(car->AllocType<=EfiGcdAllocateAddress) tmp=ct+1;
        else tmp=cb-1;
    }

    car->Max=ct;
    car->Min=cb;
    car->Len=ct-cb+1;    

	TmpTop[ResType]=ct;
	TmpBot[ResType]=cb;

    return EFI_SUCCESS;
}


EFI_STATUS EFIAPI ClaimIoResources(PCI_ROOT_BRG_DATA *CurRootBrg, PCI_ROOT_BRG_DATA *NextRootBrg){
    EFI_STATUS                      Status;
    DXE_SERVICES                    *dxe=NULL;
    EFI_GCD_IO_SPACE_DESCRIPTOR     io;
    UINT64                          top, bot, tmp, ct, cb;    
    ACPI_RES_DATA                   *car, *nar;
//--------------------------------
	// Get GCD's Memory Space and IO Space Maps
	Status=LibGetDxeSvcTbl(&dxe);
    if(EFI_ERROR(Status)){
        PCI_TRACE((TRACE_PCI,"PciRB: Fail to Get DXE Services Table Pointer - %r\n", Status));
        ASSERT_EFI_ERROR(Status);
        return EFI_DEVICE_ERROR;
    }

    car=&CurRootBrg->AcpiRbRes[raIo];

    if(car->Len==0) return EFI_NOT_FOUND;

    if(NextRootBrg!=NULL) nar=&NextRootBrg->AcpiRbRes[raIo];
    else nar=NULL;

    if( nar==NULL ) {
        top=MAX_PCI_IO;
        bot=MIN_PCI_IO;
    } else {
        if(car->AllocType<=EfiGcdAllocateAddress){
            top=nar->Min-1;
            bot=car->Min;
        } else {
            top=car->Max;
            bot=MIN_PCI_IO;
        }
    }

	if( TmpTop[raIo] != 0 ) bot=TmpTop[raIo]+1;
	//if( TmpBot[raIo] != 0 ) bot=TmpBot[raIo];

    cb=car->Min;
    ct=car->Max;

    PCI_TRACE((TRACE_PCI,"  Ajusting ACPI IO MAP [0x%lX...[0x%lX...0x%lX]...0x%lX]\n",bot,cb,ct,top));
    
    PCI_TRACE((TRACE_PCI,"    Searching from "));

    if(car->AllocType<=EfiGcdAllocateAddress) {
        tmp=ct+1;
        PCI_TRACE((TRACE_PCI,"0x%lX ==> UP.....to 0x%lX\n", tmp, top));
    } else {
        tmp=cb-1;
        PCI_TRACE((TRACE_PCI,"0x%lX <== DOWN...to 0x%lX\n", tmp, bot));
    }
    
    //All IO map's EfiGcdIoTypeNonExistent IO type, must be converted to EfiGcdIoTypeIo    
    while(TRUE){
        Status=dxe->GetIoSpaceDescriptor(tmp,&io);

        //Check if we reach end of GCD Space map... 
        if(EFI_ERROR(Status)) break;
        
        //Check if we hit another RB 
        if(io.DeviceHandle!=NULL && io.DeviceHandle!=CurRootBrg->RbHandle) break;        

        PCI_TRACE((TRACE_PCI,"    IoDes: BAS=0x%lX; LEN=0x%lX; TYP=%d; IH=0x%X; DX=0x%X; %r\n",
        io.BaseAddress, io.Length, io.GcdIoType, io.ImageHandle, io.DeviceHandle, Status));

        if(car->AllocType<=EfiGcdAllocateAddress){
           ct=io.BaseAddress+io.Length-1;   
        } else {
            cb=io.BaseAddress;
        }                
        if(car->AllocType<=EfiGcdAllocateAddress){
            //if we are searching bottom to top...
            if(ct>=top){
                ct=top;
                break;
            }
        } else {
            //if we are searching top to bottom...
            if(cb<=bot){
                cb=bot;
                break;
            }
        }
        
        if(car->AllocType<=EfiGcdAllocateAddress) tmp=ct+1;
        else tmp=cb-1;
    }

    car->Max=ct;
    car->Min=cb;
    car->Len=ct-cb+1;    

	TmpTop[raIo]=ct;
	TmpBot[raIo]=cb;

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI AjustAcpiResource(UINTN HostIndex, UINTN RootIndex, ACPI_RES_TYPE ResType){
    EFI_STATUS          Status=EFI_SUCCESS;
    PCI_HOST_BRG_DATA   *hb, *nexthb;
    PCI_ROOT_BRG_DATA   *rb, *nextrb;
    ASLR_QWORD_ASD      *res, *nextres;
//--------------------

    //Get pointers to the main data structures we are going to work with.
    hb=&gHostBrgPrivateData[HostIndex];
    rb=hb->RootBridges[RootIndex];

    //Check if system has NEXT entity available...
    if(gHostBrgCount > HostIndex+1) nexthb=&gHostBrgPrivateData[HostIndex+1];
    else nexthb=NULL;

    if(hb->RootBridgeCount>RootIndex+1)nextrb=hb->RootBridges[RootIndex+1];
    else {
        if(nexthb!=NULL) nextrb=nexthb->RootBridges[0];
        else nextrb=NULL;
    }

    //Start parsing resources...
    switch(ResType){
        case raBus:
            //Trying to make bus ranges among the bridges be decoded consistently
            //even RB0 decodes buses 0..5 and RB1 decodes 0x80..0x82 for ACPI 
            //make it decode 0..0x7F and 80..0xFF correspondently.
            Status=FindRbBusData(rb, &res);
            //BUS resources must be there
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

            //fill ACPI Resource properties
            rb->AcpiRbRes[raBus].Min=res->_MIN;
            rb->AcpiRbRes[raBus].Gra=1;
            if (nextrb!=NULL) {
                Status=FindRbBusData(nextrb, &nextres);        
                //BUS resources must be there
                ASSERT_EFI_ERROR(Status);
                if(EFI_ERROR(Status)) return EFI_DEVICE_ERROR;
				//Check if NULL bus descriptor...
				if(nextres->_MIN==0 && nextres->_MAX==0 && nextres->_LEN==0)
	               rb->AcpiRbRes[raBus].Max=MAX_PCI_BUSES; 
				else
                   rb->AcpiRbRes[raBus].Max=nextres->_MIN-1;
           } else {
                rb->AcpiRbRes[raBus].Max=MAX_PCI_BUSES;
           }
           rb->AcpiRbRes[raBus].Len=rb->AcpiRbRes[raBus].Max-rb->AcpiRbRes[raBus].Min+1;

           PCI_TRACE((TRACE_PCI,"  Ajusting ACPI BUS MAP [0x%lX...[0x%lX...0x%lX]...0x%lX]\n",
           rb->AcpiRbRes[raBus].Min,res->_MIN,res->_MIN+res->_LEN-1,rb->AcpiRbRes[raBus].Max));
           PCI_TRACE((TRACE_PCI,"  ACPI BUS MAP Set to [0x%lX...0x%lX]\n",
                        rb->AcpiRbRes[raBus].Min,rb->AcpiRbRes[raBus].Max));
        break;

        case raIo:
        //With IO picture is much worse. we should scan and assume any unallocated IO to the 
        //corresponded RB keeping in mind boundary between RB0..RB1..RBn decoding ranges.
            //IO resources may not be there ISA res 0...0xFFF is taken care of automatically
            //If system don't have second root bridge and does not decode any IO, assume all IO 
            //from 0x1000...Fset  decoded by
            Status=ClaimIoResources(rb, nextrb);
            PCI_TRACE((TRACE_PCI,"  ACPI IO MAP Set to [0x%lX...0x%lX] %r\n",
                        rb->AcpiRbRes[raIo].Min,rb->AcpiRbRes[raIo].Max,Status));
        break;
        case raMmio32:
            Status=ClaimMmioResources(rb, nextrb, raMmio32);
            PCI_TRACE((TRACE_PCI,"  ACPI MEM32 MAP Set to [0x%lX...0x%lX] %r\n",
                        rb->AcpiRbRes[raMmio32].Min,rb->AcpiRbRes[raMmio32].Max, Status));
            
        break;
        case raMmio64:
            Status=ClaimMmioResources(rb, nextrb, raMmio64);
            PCI_TRACE((TRACE_PCI,"  ACPI MEM64 MAP Set to [0x%lX...0x%lX] %r\n",
                        rb->AcpiRbRes[raMmio64].Min,rb->AcpiRbRes[raMmio64].Max, Status));
        break;
        default: return EFI_DEVICE_ERROR; 
    }

    return EFI_SUCCESS;
}

/**
    This function will create ReadyToBoot Event to update each 
    Root Bridge ASL Object with corresponded resources decoded.

    @param Nothing

    @retval EFI_SUCCESS is OK
              

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
VOID EFIAPI RbReadyToBoot(IN EFI_EVENT Event,	IN VOID	*Context){
    EFI_STATUS                      Status;
    UINTN                           i,j,k;
    PCI_HOST_BRG_DATA               *hb;
    PCI_ROOT_BRG_DATA               *rb;
    ACPI_HDR                        *dsdt;
    EFI_PHYSICAL_ADDRESS            a;   
    BOOLEAN                         vga=FALSE;
//-----------------------------
    
//	EFI_DEADLOOP();

    //Get DSDT.. we have to update it.
    Status=LibGetDsdt(&a,EFI_ACPI_TABLE_VERSION_ALL);
   	if(EFI_ERROR(Status)){
        PCI_TRACE((TRACE_PCI,"PciRB: Fail to Get DSDT - returned %r\n", Status));
	    ASSERT_EFI_ERROR(Status);
        return;
    } else dsdt=(ACPI_HDR*)a;

   	if(!DontUpdateRbResAsl){
        //Collect currently programed resource requirements, we will need to ajust it
        //to clame all Unallocated IO/MMIO to report it to OS. 
        for(i=0; i<gHostBrgCount; i++){
            hb=&gHostBrgPrivateData[i];
            for(j=0; j<hb->RootBridgeCount; j++){
                rb=hb->RootBridges[j];            
    
                PCI_TRACE((TRACE_PCI,"PciRB: Host[ %d ].Root[ %d ] \n", i, j));
    
                for(k=raBus; k<raMaxRes; k++){
                    Status=AjustAcpiResource(i,j,k);
                }//for k
            }//for j
        }//for i
    
        //after collectinfg and ajusting all Rb decoding info info
        //Update place holders in each Rb Device Scope in DSDT.
        for(i=0; i<gHostBrgCount; i++){
            hb=&gHostBrgPrivateData[i];
            for(j=0; j<hb->RootBridgeCount; j++){
                rb=hb->RootBridges[j];            
                
                //Check if this is a Compatibility Root Bridge.                        
                PCI_TRACE((TRACE_PCI,"PciRB: Updating ASL Device Object '%a': ...\n", rb->RbAslName));
    
                if(rb->Supports & (PCI_ROOT_COMPATIBILITY_ATTRIBUTES)){
                    Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "CPRB", 1); 
                    PCI_TRACE((TRACE_PCI,"        =>'CPRB'= 1 -> %r;\n", Status));
                } else {
                    Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "CPRB", 0); 
                    PCI_TRACE((TRACE_PCI,"        =>'CPRB'= 0 -> %r;\n", Status));
                }
    
                if(rb->Attributes & (PCI_ROOT_VGA_ATTRIBUTES)){
                    Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "LVGA", 1);
                    PCI_TRACE((TRACE_PCI,"        =>'LVGA'= 1 -> %r;\n", Status));
                    vga=TRUE;
                } else {
                    Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "LVGA", 0); 
                    PCI_TRACE((TRACE_PCI,"        =>'LVGA'= 0 -> %r;\n", Status));
                }
    
                //Now Update place holders fit in collected system resource consumption.
    
                //0. Update device Status
                if( rb->NotPresent ) Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "STAV", 0x00);
                PCI_TRACE((TRACE_PCI,"        =>'SATV'=%d -> %r;\n", !rb->NotPresent, Status));
    
                //1.Update Bus Object
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "BRB", rb->AcpiRbRes[raBus].Min);
                PCI_TRACE((TRACE_PCI,"        =>'BRB_'=0x%lX -> %r; ",rb->AcpiRbRes[raBus].Min, Status));
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "BRL", rb->AcpiRbRes[raBus].Len);
                PCI_TRACE((TRACE_PCI," 'BRL_'=0x%lX -> %r;\n",rb->AcpiRbRes[raBus].Len, Status));
    
                //2.Update IO Object
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "IOB", rb->AcpiRbRes[raIo].Min);
                PCI_TRACE((TRACE_PCI,"        =>'IOB_'=0x%lX -> %r; ",rb->AcpiRbRes[raIo].Min, Status));
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "IOL", rb->AcpiRbRes[raIo].Len);
                PCI_TRACE((TRACE_PCI," 'IOL_'=0x%lX -> %r;\n",rb->AcpiRbRes[raIo].Len, Status));
    
                //3.Update MMIO32 Object
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MBB", rb->AcpiRbRes[raMmio32].Min);
                PCI_TRACE((TRACE_PCI,"        =>'MBB_'=0x%lX -> %r; ",rb->AcpiRbRes[raMmio32].Min, Status));
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MBL", rb->AcpiRbRes[raMmio32].Len);
                PCI_TRACE((TRACE_PCI," 'MBL_'=0x%lX -> %r;\n",rb->AcpiRbRes[raMmio32].Len, Status));
    
                //3.Update MMIO64 Object
                //3.1 Update LO part of _MIN
                a=rb->AcpiRbRes[raMmio64].Min & 0xFFFFFFFF;
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MABL", a);
                PCI_TRACE((TRACE_PCI,"        =>'MABL'=0x%lX -> %r; ", a, Status));
    
                //3.2 Update HI part of _MIN
                a=Shr64(rb->AcpiRbRes[raMmio64].Min, 32);
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MABH", a);
                PCI_TRACE((TRACE_PCI,"'MABH'=0x%lX -> %r;\n", a, Status));
    
                //3.3 Update LO part of MAX
                a=rb->AcpiRbRes[raMmio64].Max & 0xFFFFFFFF;
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MAML", a);
                PCI_TRACE((TRACE_PCI,"        =>'MAML'=0x%lX -> %r; ", a, Status));
    
                //3.4 Update HI part of _MAX
                a=Shr64(rb->AcpiRbRes[raMmio64].Max, 32);
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MAMH", a);
                PCI_TRACE((TRACE_PCI,"'MAMH'=0x%lX -> %r;\n", a, Status));
    
                //3.3 Update LO part of MAX
                a=rb->AcpiRbRes[raMmio64].Len & 0xFFFFFFFF;
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MALL", a);
                PCI_TRACE((TRACE_PCI,"        =>'MALL'=0x%lX -> %r; ", a, Status));
    
                //3.4 Update HI part of _MAX
                a=Shr64(rb->AcpiRbRes[raMmio64].Len, 32);
                Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "MALH", a);
                PCI_TRACE((TRACE_PCI,"'MALH'=0x%lX -> %r;\n", a, Status));
    
            }//j
        }//i    
    
        //if PCI_ROOT_VGA_ATTRIBUTES were not set by drivers we have to clame Legacy VGS IO and MEM Space.
        if(!vga){
            hb=&gHostBrgPrivateData[0];
            rb=hb->RootBridges[0];            
            PCI_TRACE((TRACE_PCI,"PciRB: Legacy VGA Attributes were not set!!!\n"));
            PCI_TRACE((TRACE_PCI,"    Setting 'LVGA'=1 of Device Object '%a' for Hb[0].Rb[0] -> ", rb->RbAslName));
            Status=UpdateAslNameOfDevice(dsdt, rb->RbAslName, "LVGA", 1);
            PCI_TRACE((TRACE_PCI,"%r;\n", Status));
        }
   	}
   	
   	
    //Update PCIe Base address Name Object it might gets changed through PCD Mechanism...
    //SDL token PEBS Initialization Must changed to accommodate 64 bit value.
   	if(!DontUpdatePebs){
   	    Status = UpdateAslNameObject( dsdt, (UINT8*)"PEBS", PcdGet64 (PcdPciExpressBaseAddress));
   	}
    
    //Now....Checksum 
   	if(!(DontUpdatePebs & DontUpdateRbResAsl)){
   	    //no need to checksum the table if were no updates...
   	    dsdt->Checksum = 0;
   	    dsdt->Checksum = ChsumTbl((UINT8*)dsdt, dsdt->Length);
   	}

	// Check for invalid or Empty Pci RootBridge Handle. 
	//If Rootbridge handle is invalid, then uninstall PCI Rootbridge IO protocol on this handle.
	for(i=0; i<gHostBrgCount; i++){
         hb=&gHostBrgPrivateData[i];
        for(j=0; j<hb->RootBridgeCount; j++){
            rb=hb->RootBridges[j];  
            PCI_TRACE((TRACE_PCI,"PciRootBrg: Host[ %d ].Root[ %d ] \n", i, j));
          // APTIOV_SERVER_OVERRIDE_START  
            if(!DontUpdateRbUidAsl){
                if (rb->NotPresent) {
                    // Update _UID for skipped RootBridges also to its equivalent Rb Index				
                    ((ACPI_HID_DEVICE_PATH*)(rb->DevPath))->UID = (UINT32)j;
                }			
                PCI_TRACE((TRACE_PCI,"PciRB: Updating Rb UID ASL Device Object '%a': ...", rb->RbAslName));
                // Update UID value set for each RootBridge device path
                Status = UpdateAslNameOfDevice(dsdt, rb->RbAslName, "_UID", ((ACPI_HID_DEVICE_PATH*)(rb->DevPath))->UID);
                PCI_TRACE((TRACE_PCI,"%r;\n", Status));
            }
			// APTIOV_SERVER_OVERRIDE_END
            
			if(rb->NotPresent && rb->RbHandle!=NULL) {
				PCI_TRACE((TRACE_PCI,"Hb.SetBusNumbers - Invalid RB Hndle Passed\n"));
				Status = pBS->UninstallMultipleProtocolInterfaces (
        			rb->RbHandle, 
					&gEfiPciRootBridgeIoProtocolGuid, &rb->RbIoProtocol,
					&gEfiDevicePathProtocolGuid, 	rb->DevPath,
					NULL
    			);
			
				PCI_TRACE((TRACE_PCI,"Uninstall RbIoProtocol Status %r; \n", Status));			
			    rb->RbHandle=NULL;
			}//if     
        }//for j
    }//for i
	
    pBS->CloseEvent(Event);
}

/**
    This function will adjust the final GCD resource map and
    convert all NonrExistant Memory resources into MMIO.

    @param ImgHandle	Image handle
        CntrHandle	Controller Handle

    @retval VOID

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS EFIAPI ConvertMemoryMap(EFI_HANDLE	ImgHandle, EFI_HANDLE	CntrHandle)
{
	EFI_STATUS						Status;
	EFI_GCD_MEMORY_SPACE_DESCRIPTOR	*pMap;
	EFI_GCD_IO_SPACE_DESCRIPTOR		*pIo;
	UINTN 							Size, i;
	DXE_SERVICES					*dxe;	
//----------------------------------------------------------------------

	// Convert All Nonexistent Memory space to MMIO
	// Once GCD and Memory manager gets initialized
	Status=LibGetDxeSvcTbl(&dxe);
	if(EFI_ERROR(Status)) return Status;

	//Call CSP function to convert MMIO memory map;
	Status=HbCspConvertMemoryMapMmio(ImgHandle,CntrHandle);
    //If Status SUCCESS - Mem Map converted
    if(EFI_ERROR(Status)){
        //if UNSUPPORTED do default conversion...
        if(Status==EFI_UNSUPPORTED){
            Status=dxe->GetMemorySpaceMap(&Size, &pMap);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) return Status;
        
            for(i=0; i<Size; i++)
            if (pMap[i].GcdMemoryType == EfiGcdMemoryTypeNonExistent ){
                Status=dxe->AddMemorySpace(EfiGcdMemoryTypeMemoryMappedIo,
                    pMap[i].BaseAddress,pMap[i].Length, GCD_COMMON_MMIO_CAPS);
                ASSERT_EFI_ERROR(Status);
                if(EFI_ERROR(Status)) return Status;
            }
            pBS->FreePool(pMap);
        } else {
            //ERROR status and not UNSUPPORTED!
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
    }
        
	//Convert all non-existant IO to existant IO
    //Call CSP function to convert IO memory map;
    Status=HbCspConvertMemoryMapIo(ImgHandle,CntrHandle);
    //If Status SUCCESS - Mem Map converted
    if(EFI_ERROR(Status)){
        //if UNSUPPORTED do default conversion...
        if(Status==EFI_UNSUPPORTED){
            Status=dxe->GetIoSpaceMap(&Size, &pIo);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) return Status;
            for(i=0; i<Size; i++)
            if (pIo[i].GcdIoType == EfiGcdIoTypeNonExistent){
                Status=dxe->AddIoSpace(EfiGcdIoTypeIo,pIo[i].BaseAddress,pIo[i].Length);
                ASSERT_EFI_ERROR(Status);
                if(EFI_ERROR(Status)) return Status;
            }
            pBS->FreePool(pIo);
        } else {
            //ERROR status and not UNSUPPORTED!
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
    }
	return Status;	
}

/**
    Will check if all root bridges belonging to the "Host" has submitted 
    it's resources

        
    @param Host Pointer to HOST Bridge private data
    @param RootBridgeHandle PCI Root Bridge to be configured
    @param Configuration Pointer to the pointer to the PCI bus resource descriptor
    @param PciBusAddress Address of the controller on the PCI bus
    @param Phase The Phase during resource allocation

    @retval BOOLEAN
        TRUE    all root bridges belonging to the "Host" has its resources submitted
        FALSE   opposite ...
**/
BOOLEAN EFIAPI IsResSubmited(PCI_HOST_BRG_DATA *Host){
	PCI_ROOT_BRG_DATA	*rb;
	UINTN				i;
//---------------------
	for(i=0; i<gHostBrgCount; i++){
		rb=Host->RootBridges[i];
		if(!rb->ResSubmited) return FALSE;
	}
	return TRUE;
}


/**
    Lokates 

        
    @param This Protocol instance
    @param RbHandle PCI Root Bridge Handle to get Private data for.
    @param Index Updated Index of the Found Root Bridge private data

    @retval PCI_ROOT_BRG_DATA 
        Not NULL if Root was found.
        NULL if Root was not found.
**/
PCI_ROOT_BRG_DATA *GetRbByHandle(EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*This,
								 EFI_HANDLE										 	RbHandle,
								 UINTN												*Index OPTIONAL)
{
	PCI_HOST_BRG_DATA	*hbdata=(PCI_HOST_BRG_DATA*)This;
	PCI_ROOT_BRG_DATA	*rbdata;
	UINTN				i;
//----------------------------------
	for(i=0;i<hbdata->RootBridgeCount;i++){
		rbdata=hbdata->RootBridges[i];
		if(rbdata->RbHandle==RbHandle){
			if(Index)(*Index)=i;
//			hbdata->CurrentRb=i;
			return rbdata;
		}
	}
	return NULL;
}


/**
    This function will generate Resource descriptors block based on information
    stored in RootBridge->RbRes array

        
    @param Host Pointer to HOST Bridge private data
    @param Resources Resource List in a form of ACPI QWORD Resource Descriptor.
    @param ResType Describes what type of resources we are getting.

        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_OUT_OF_RESOURCES If SubmitResources ( ) could not allocate resources
    @retval EFI_NOT_READY This phase cannot be entered at this time
    @retval EFI_DEVICE_ERROR SetResources failed due to HW error.
**/
EFI_STATUS EFIAPI GetResources(PCI_ROOT_BRG_DATA *RootBrg, ASLR_QWORD_ASD **Resources, ASLR_TYPE_ENUM ResType)
{
	UINTN				i,cnt=0;
	ASLR_QWORD_ASD		*qw;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	BOOLEAN				cpy = FALSE;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
//---------------------------------1
	//count descriptors first
	if(ResType==tResAll) cnt=RootBrg->ResCount;
	else { 
		for(i=0;i<RootBrg->ResCount;i++){
			if(ResType==tResBus){
				if(RootBrg->RbRes[i]->Type == ASLRV_SPC_TYPE_BUS)cnt++;
			} else {
				if(RootBrg->RbRes[i]->Type < ASLRV_SPC_TYPE_BUS) cnt++;
			}
		}
	}
	//if(!cnt) return EFI_NOT_FOUND;

	//Allocate memory for ASL Resource Descriptors
	qw=Malloc(sizeof(ASLR_QWORD_ASD)*cnt+sizeof(ASLR_EndTag));
	if(!qw) return EFI_OUT_OF_RESOURCES;

	*Resources=qw;
	
	//Copy Resource information
	for(i=0; i<RootBrg->ResCount; i++, cpy=FALSE){
		if(ResType==tResAll) cpy=TRUE;
		else {
			if(ResType==tResBus){
				if(RootBrg->RbRes[i]->Type == ASLRV_SPC_TYPE_BUS)cpy=TRUE;
			} else {
				if(RootBrg->RbRes[i]->Type < ASLRV_SPC_TYPE_BUS) cpy=TRUE;
			}
		}
		if(cpy){
			MemCpy(qw,RootBrg->RbRes[i],sizeof(ASLR_QWORD_ASD));
			qw++;
		}
	}
	((ASLR_EndTag*)qw)->Hdr.HDR=ASLV_END_TAG_HDR;
	((ASLR_EndTag*)qw)->Chsum=0;
	
	return EFI_SUCCESS;
}

/**
    This function will update RootBridge->RbRes array with Descriptors 
    prowided with *Resources parameter

        
    @param Host Pointer to HOST Bridge private data
    @param Resources Resource List in a form of ACPI QWORD Resource Descriptor.
    @param ResType Describes what type of resources we are getting.
    @param Replace if TRUE function will Remove all descriptors of "ResType" before adding.
        if FALSE function will just add "Resources"

        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_OUT_OF_RESOURCES If SubmitResources ( ) could not allocate resources
    @retval EFI_NOT_READY This phase cannot be entered at this time
    @retval EFI_DEVICE_ERROR SetResources failed due to HW error.
**/
EFI_STATUS EFIAPI SetResources(PCI_ROOT_BRG_DATA *RootBrg, ASLR_QWORD_ASD *Resources, 
			ASLR_TYPE_ENUM ResType, BOOLEAN	Replace)
{
	UINTN				i, len, cnt;
	ASLR_QWORD_ASD		*res;
//---------------------------------

	len=ValidateDescriptorBlock((ASLR_QWORD_ASD*)Resources, ResType, FALSE);
	if(!len) return EFI_INVALID_PARAMETER;

	cnt=(len-sizeof(ASLR_EndTag))/sizeof(ASLR_QWORD_ASD);
    //We have received Empty descriptor nothing to do just return.
    if((!cnt)&&(!Replace)) return EFI_SUCCESS;
    

	if(Replace){
		if(ResType==tResAll) ClearItemLst((T_ITEM_LIST*)&RootBrg->ResInitCnt, TRUE);
		else {
            i=RootBrg->ResCount;
            while(i){
                i--;
    		    if(ResType==tResBus){
			        if(RootBrg->RbRes[i]->Type == ASLRV_SPC_TYPE_BUS)
				        DeleteItemLst((T_ITEM_LIST*)&RootBrg->ResInitCnt, i,TRUE);
                } else {
					if(RootBrg->RbRes[i]->Type < ASLRV_SPC_TYPE_BUS)
					    DeleteItemLst((T_ITEM_LIST*)&RootBrg->ResInitCnt, i,TRUE);
			    }
            }	
		}
	}

	//Update Bus Resources in rbdata
	for(i=0; i<cnt; i++,Resources++) {
		res=Malloc(sizeof(ASLR_QWORD_ASD));
		if(!res) return EFI_OUT_OF_RESOURCES;

		MemCpy(res,Resources,sizeof(ASLR_QWORD_ASD));
		
		if(EFI_ERROR(AppendItemLst((T_ITEM_LIST*)&RootBrg->ResInitCnt,res))) return EFI_OUT_OF_RESOURCES;
	}
	return EFI_SUCCESS;
}

/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   IsPowerOfTwo()
//
// Description: 
//  Checks if value passed makes "POWER OF TWO"
//
// Input:
//  UINT64      Value       Value to check.
//
// Output: BOOLEAN 
//  TRUE or FALSE based on value passed.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN IsPowerOfTwo(UINT64 Value){
	UINTN 	i;
	UINT64 	t;
//--------------
	for(i=0; i<64; i++){
		t=Shl64(1,(UINT8)i);
		if(Value&t) {
			if(Value&(~t))return FALSE;
			else return TRUE;
		}
	}
	return FALSE;
}
*/

//(EIP45278)>
#if PCI_MMIO_RES_TOP_ALLIGN == 1
/**
    This function will try to Allocate from GCD set of resources requested by
    input Length and Alignment.

        
    @param Alignment Align with 2^Alignment
    @param Length Length to allocate
    @param BaseAddress Base address to allocate
    @param ImageHandle The image handle consume the allocated space.
    @param DeviceHandle The device handle consume the allocated space.

    @retval EFI_STATUS
        EFI_INVALID_PARAMETER   Invalid parameter.
        EFI_NOT_FOUND           No descriptor contains the desired space.
        EFI_SUCCESS             Memory space successfully allocated.

        EFI_PHYSICAL_ADDRESS    *BaseAddress    Base address to be allocated

**/
EFI_STATUS EFIAPI AllocateMemoryResource (
    IN      UINTN                   Alignment,
    IN      UINT64                  Length,
    IN OUT  EFI_PHYSICAL_ADDRESS    *BaseAddress,
    IN      EFI_HANDLE              ImageHandle,
    IN      EFI_HANDLE              DeviceHandle
)
{
    EFI_STATUS                      Status;
    DXE_SERVICES                    *dxe;
    EFI_PHYSICAL_ADDRESS            AlignmentMask;

    Status=LibGetDxeSvcTbl(&dxe);
    if(EFI_ERROR(Status)) return Status;

    AlignmentMask = (1 << Alignment) - 1;
    // Search an unused for fit the length
    while (((*BaseAddress + Length) & AlignmentMask)) {
        Status = dxe->AllocateMemorySpace (
                      EfiGcdAllocateMaxAddressSearchTopDown,
                      EfiGcdMemoryTypeMemoryMappedIo,
                      0,
                      Length,
                      BaseAddress,
                      ImageHandle,
                      DeviceHandle);
        if(EFI_ERROR(Status)) break;
        // Check if the space is under Alignment
        if (((*BaseAddress + Length) & AlignmentMask)) {
            Status = dxe->FreeMemorySpace (*BaseAddress, Length);
            if(EFI_ERROR(Status)) break;
            *BaseAddress = ((*BaseAddress + Length) & (~AlignmentMask)) - Length;
        }
    }
    return  Status;
}
#endif
//<(EIP45278)

/**
    Converts C passed into Alignment format 

        
    @param g AlignFromGra Value to convert

    @retval UINTN
        Converted Alignment value.
**/
UINTN AlignFromGra(UINTN g){
	UINTN a=0;
//------------
	while(g&1){
		a++;
		g=g>>1;
	}
	return a;	
}

/**
    This function will try to Allocate from GCD set of resources requested by 
    PCI BUS driver in "*Resources" buffer.

        
    @param RootBrg Pointer to ROOT Bridge private data

        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_OUT_OF_RESOURCES If could not allocate resources
    @retval EFI_DEVICE_ERROR failed due to HW error.
**/
EFI_STATUS EFIAPI AllocateResources(PCI_ROOT_BRG_DATA *RootBrg){
	EFI_STATUS 			Status;
    EFI_STATUS          ReturnStatus = 0;
	UINTN				i;
	ASLR_QWORD_ASD		*res;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    ACPI_RES_DATA       *ar = NULL;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
	UINT64				a=0;
	DXE_SERVICES		*dxe;	
//OUT_OF_RES HANDLING++ must be added in Custom Res allocate function.
#if BoardPciRes_SUPPORT
    AMI_OUT_OF_RES_VAR  OutOfResVar;
//--------------------------------------
    //Check if OUT_OF_RES_VAR was already created.
    Status=AmiPciOutOfRes(&OutOfResVar, TRUE);
    if(EFI_ERROR(Status)) pBS->SetMem(&OutOfResVar,sizeof(AMI_OUT_OF_RES_VAR),0);
    else OutOfResVar.Count++;
#endif
//OUT_OF_RES HANDLING-- must be added in Custom Res allocate function.

	Status=LibGetDxeSvcTbl(&dxe);
	if(EFI_ERROR(Status)) return Status;

 
	for(i=0; i<RootBrg->ResCount; i++){
		res=RootBrg->RbRes[i];
		if(res->Type==ASLRV_SPC_TYPE_BUS) continue;
		//Check if Alignment
		if(!IsPowerOfTwo(res->_MAX+1)) return EFI_INVALID_PARAMETER;
		switch(res->_GRA){
			case 16 : 	a=0xFFFF;//-res->_LEN;
				break;
			case 32	:	a=0xFFFFFFFF;//-res->_LEN;
				break;
			case 64	:	a=0xFFFFFFFFFFFFFFFF;//-res->_LEN+1;
				break;
			default :ASSERT(0); return EFI_INVALID_PARAMETER;
		}
        
        PCI_TRACE((TRACE_PCI,"PciRB: AllocateResources In -> _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX; _GRA=0x%lX\n",
        res->_MIN,res->_MAX,res->_LEN,res->_GRA));


		//Allocate IO
		if(res->Type==ASLRV_SPC_TYPE_IO){
            
            ar=&RootBrg->AcpiRbRes[raIo];
            ar->AddrUsed=a;
            ar->AllocType=EfiGcdAllocateMaxAddressSearchTopDown;

			Status=dxe->AllocateIoSpace(EfiGcdAllocateMaxAddressSearchTopDown,
							EfiGcdIoTypeIo,
							AlignFromGra((UINTN)res->_MAX),
							res->_LEN,
							&a,
							RootBrg->ImageHandle, RootBrg->RbHandle);
            
			if(EFI_ERROR(Status)) {
                ReturnStatus = Status;
                PCI_TRACE((TRACE_PCI,"PciRB: IO Allocation Failed: Length: %lX\n",res->_LEN));
//++OUT_OF_RES!! IO                
#if BoardPciRes_SUPPORT
                OutOfResVar.Resource=*res;
                Status=AmiPciOutOfRes(&OutOfResVar, FALSE);                
                return ReturnStatus;
//--OUT_OF_RES!! IO                
#else
                continue;
#endif
		    }
            
        }
		//Allocate MMIO
		else if( res->Type==ASLRV_SPC_TYPE_MEM){

            if(res->_GRA==32)ar=&RootBrg->AcpiRbRes[raMmio32];
            else ar=&RootBrg->AcpiRbRes[raMmio64];
            ar->AddrUsed=a;
            ar->AllocType=EfiGcdAllocateMaxAddressSearchTopDown;
//(EIP45278)>
#if PCI_MMIO_RES_TOP_ALLIGN == 1
            Status = AllocateMemoryResource(
                     AlignFromGra((UINTN)res->_MAX),
                     res->_LEN,
                     &a,
                     RootBrg->ImageHandle,
                     RootBrg->RbHandle);
#else
			Status=dxe->AllocateMemorySpace(EfiGcdAllocateMaxAddressSearchTopDown,
							EfiGcdMemoryTypeMemoryMappedIo,
							AlignFromGra((UINTN)res->_MAX),
							res->_LEN,
							&a,
							RootBrg->ImageHandle, RootBrg->RbHandle);
#endif
//<(EIP45278)

			if(EFI_ERROR(Status)) {
                ReturnStatus = Status;
                PCI_TRACE((TRACE_PCI,"PciRB: Memory Allocation Failed: Length: %lX\n",res->_LEN));
//++OUT_OF_RES!! MEM                
#if BoardPciRes_SUPPORT
                OutOfResVar.Resource=*res;
                Status=AmiPciOutOfRes(&OutOfResVar, FALSE);                
                return ReturnStatus;
#else
//--OUT_OF_RES!! MEM                
                continue;
#endif
            }

			//Set this region as WT cache if it is PREFETCHABLE 
			if(res->TFlags.MEM_FLAGS._MEM!=ASLRV_MEM_UC) 
			{
				//Status=gDxeSvcTbl->SetMemorySpaceAttributes(a,res->_LEN,EFI_MEMORY_WT|EFI_MEMORY_RUNTIME);
				Status=dxe->SetMemorySpaceAttributes(a,res->_LEN,EFI_MEMORY_WT);
				//if attempt to set WT attributes has filed, let's try UC
				if(EFI_ERROR(Status))
				{
					PCI_TRACE((TRACE_DXE_CHIPSET,"PciRB: Setting of WT attributes for prefetchable memory has failed(%r). UC is used.\n",Status));
					//Status=gDxeSvcTbl->SetMemorySpaceAttributes(a,res->_LEN,EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
					Status=dxe->SetMemorySpaceAttributes(a,res->_LEN,EFI_MEMORY_UC);
				}
			}
			else Status=dxe->SetMemorySpaceAttributes(a,res->_LEN,EFI_MEMORY_UC);
			//	Status=gDxeSvcTbl->SetMemorySpaceAttributes(a,res->_LEN,EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;

		}
		res->_MIN=a;


        PCI_TRACE((TRACE_PCI,"PciRB: AllocateResources Out-> _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX; _GRA=0x%lX\n",
        res->_MIN,res->_MAX,res->_LEN,res->_GRA));

// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
        if(ar != NULL) {
            //Do some calculation for ACPI _CRS update.        
            if (ar->Min==0) ar->Min=res->_MIN;
            else {
                if(ar->Min > res->_MIN)ar->Min=res->_MIN;
            }
//(EIP45278+)>
#if PCI_MMIO_RES_TOP_ALLIGN == 1
            ar->Gra = 0;
#else
            if (ar->Gra==0) ar->Gra=res->_MAX;
            else {
                if(ar->Gra<res->_MAX)ar->Gra=res->_MAX;
            }
#endif
    //<(EIP45278+)
            if(ar->Len==0){
                ar->Len=res->_LEN;
                if(ar->Len & ar->Gra) ar->Len=(ar->Len |ar->Gra)+1;
                ar->Max=ar->Min+ar->Len-1; 
            } else {
                UINT64  max;
            //--------
                max=res->_MIN + res->_LEN - 1;
                if(max>ar->Max)ar->Max=max; 
                ar->Len=ar->Max - ar->Min + 1;
                if(ar->Len & ar->Gra) ar->Len=(ar->Len |ar->Gra)+1;
                ar->Max=ar->Min+ar->Len-1; 
            }
            PCI_TRACE((TRACE_PCI,"PciRB: AcpiResources Min=0x%lX; Max=0x%lX; Len=0x%lX; Gra=0x%lX, AdrUsed=0x%lX\n\n",
            ar->Min,ar->Max,ar->Len,ar->Gra, ar->AddrUsed));
    
            //Don't need to set this to actual value it could be calculated using commented formula.
            //More important to preserve alignment requirements of the resource window.
            //res->_MAX=res->_MIN+res->_LEN-1;
        }
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
	}
	return ReturnStatus;	
}




//**********************************************************************
//Pci Host Bridge Resource Allocation Protocol function Implementation
//**********************************************************************

/**
    Enter a certain phase of the PCI enumeration process

        
    @param This Protocol instance
    @param Phase The phase during enumeration
    
        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_OUT_OF_RESOURCES If SubmitResources ( ) could not allocate resources
    @retval EFI_NOT_READY This phase cannot be entered at this time
    @retval EFI_DEVICE_ERROR SetResources failed due to HW error.
**/
EFI_STATUS EFIAPI HbResAllocNotifyPhase(IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL 		*This,
						  		 IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE            Phase )
{
	PCI_HOST_BRG_DATA	*hbdata=(PCI_HOST_BRG_DATA*)This;				
	EFI_STATUS			Status=0;	
	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL		**pp;
	UINTN				i;
//--------------------------------------------------------
	pp=Malloc(sizeof(VOID*)*hbdata->RootBridgeCount);
	ASSERT(pp);
	if(!pp) return EFI_INVALID_PARAMETER;
	
	for(i=0; i<hbdata->RootBridgeCount; i++) pp[i]=&hbdata->RootBridges[i]->RbIoProtocol;  

	switch(Phase){
  		case EfiPciHostBridgeBeginEnumeration:
  		    Status=HbNotifyCspBeforeEnumeration(&hbdata->ResAllocProtocol, &pp[0], hbdata->RootBridgeCount);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
			break;
  		case EfiPciHostBridgeBeginBusAllocation:
			Status=HbNotifyCspBeginBusAllocation(&hbdata->ResAllocProtocol, &pp[0],hbdata->RootBridgeCount);
			ASSERT_EFI_ERROR(Status);
			break;
  		case EfiPciHostBridgeEndBusAllocation:
			if(hbdata->AllocPhase+1!=Phase) return EFI_INVALID_PARAMETER;
			Status=HbNotifyCspEndBusAllocation(&hbdata->ResAllocProtocol, &pp[0],hbdata->RootBridgeCount);
			ASSERT_EFI_ERROR(Status);
			break;
  		case EfiPciHostBridgeBeginResourceAllocation:
		// APTIOV_SERVER_OVERRIDE_START
  		  if (PcdGetBool(AmiPcdPciOutOfResourcesStatus))
  		  {
  		    hbdata->AllocPhase = hbdata->AllocPhase - 1;
  		  }
        // APTIOV_SERVER_OVERRIDE_END
			if(hbdata->AllocPhase+1!=Phase) return EFI_INVALID_PARAMETER;
			Status=HbNotifyCspBeginResourceAllocation(&hbdata->ResAllocProtocol, &pp[0],hbdata->RootBridgeCount);
			ASSERT_EFI_ERROR(Status);
			break;
  		case EfiPciHostBridgeAllocateResources:
			if(hbdata->AllocPhase+1!=Phase) return EFI_INVALID_PARAMETER;
			if(!IsResSubmited(hbdata)) return EFI_NOT_READY;			
			Status=HbNotifyCspAllocateResources(&hbdata->ResAllocProtocol, &pp[0],hbdata->RootBridgeCount);
			ASSERT_EFI_ERROR(Status);
			break;
  		case EfiPciHostBridgeSetResources:
			if(hbdata->AllocPhase+1!=Phase) return EFI_INVALID_PARAMETER;
			if(!IsResSubmited(hbdata)) return EFI_NOT_READY;			
			Status=HbNotifyCspSetResources(&hbdata->ResAllocProtocol, &pp[0],hbdata->RootBridgeCount);
			ASSERT_EFI_ERROR(Status);
			break;
  		case EfiPciHostBridgeFreeResources:
			return EFI_UNSUPPORTED;
			break;
  		case EfiPciHostBridgeEndResourceAllocation:
			if(hbdata->AllocPhase+2!=Phase) return EFI_INVALID_PARAMETER;
			Status=HbNotifyCspEndResourceAllocation(&hbdata->ResAllocProtocol, &pp[0],hbdata->RootBridgeCount);
			ASSERT_EFI_ERROR(Status);
			break;
		default: return EFI_INVALID_PARAMETER;	
	}

	if(EFI_ERROR(Status)) return Status;
	hbdata->AllocPhase=Phase;
	return EFI_SUCCESS;
}

/**
    Return the device handle of the next PCI root bridge that is 
    associated with this Host Bridge.

        
    @param This Protocol instance
    @param RootBridgeHandle Returns the device handle 
        of the next PCI Root Bridge. On input, it holds the RootBridgeHandle 
        returned by the most recent call to GetNextRootBridge().
        The handle for the first PCI Root Bridge is returned if RootBridgeHandle 
        is NULL on input.
   
        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_INVALID_PARAMETER RootBridgeHandle is invalid
**/
EFI_STATUS EFIAPI HbResAllocGetNextRootBridge(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  	*This,
  		IN OUT EFI_HANDLE                                       *RootBridgeHandle)
{
	PCI_HOST_BRG_DATA	*hbdata=(PCI_HOST_BRG_DATA*)This;
	PCI_ROOT_BRG_DATA	*rbdata;
	UINTN				i;
//--------------------------------------------------
	//We can have not present RB in RB array.
	if(!This) return EFI_INVALID_PARAMETER;
	if(*RootBridgeHandle==NULL) {
		//we have to pick first ACTIVE RB. Keeping in mind -
		//the very first could be NOT ACTIVE as well
		for(i=0; i<hbdata->RootBridgeCount; i++){
			rbdata=hbdata->RootBridges[i];
			if(!rbdata->NotPresent){
				*RootBridgeHandle=rbdata->RbHandle;
				return EFI_SUCCESS;
			}
			//it can't be like that, but just in case... If we here 
			//that means - there are no NO ACTIVE RB AT ALL!!!
		}	
		ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
		return EFI_INVALID_PARAMETER;
	} else {
		rbdata=GetRbByHandle(This,*RootBridgeHandle,&i);
		if(!rbdata) return EFI_INVALID_PARAMETER;
		if(hbdata->RootBridgeCount-1>i){
			for(i=i+1; i<hbdata->RootBridgeCount; i++){
				rbdata=hbdata->RootBridges[i];
				if(!rbdata->NotPresent){
					*RootBridgeHandle=rbdata->RbHandle;
					return EFI_SUCCESS;
				}
			}	
		}
		//if we come here - we can't find active RB any more..
		return  EFI_NOT_FOUND;
	}		
}


/**
    Returns the attributes of a PCI Root Bridge.

        
    @param This Protocol instance
    @param RootBridgeHandle The device handle of the PCI Root Bridge 
        that the caller is interested in
    @param Attribute The pointer to attributes of the PCI Root Bridge                    
    
        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_INVALID_PARAMETER RootBridgeHandle is invalid
    @retval EFI_INVALID_PARAMETER Attributes is NULL
**/
EFI_STATUS EFIAPI HbResAllocGetAllocAttributes(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  	*This,
  		IN EFI_HANDLE	                                        RootBridgeHandle,
	  	OUT UINT64                                              *Attributes)
{
	PCI_ROOT_BRG_DATA	*rbdata;
//----------------------------------
	if(!This || !Attributes) return EFI_INVALID_PARAMETER;
	rbdata=GetRbByHandle(This,RootBridgeHandle,NULL);
	if(!rbdata)return EFI_INVALID_PARAMETER;
	else *Attributes=rbdata->Owner->AllocAttrib;
	return EFI_SUCCESS;
}

/**
    This is the request from the PCI enumerator to set up 
    the specified PCI Root Bridge for bus enumeration process. 

        
    @param This PROTOCOL instance
    @param RootBridgeHandle PCI Root Bridge to be configured
    @param Configuration Pointer to the pointer to the PCI bus resource descriptor
    
        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_INVALID_PARAMETER RootBridgeHandle is invalid
    @retval EFI_DEVICE_ERROR Request failed due to hardware error
    @retval EFI_OUT_OF_RESOURCES Request failed due to lack of resources
**/
EFI_STATUS EFIAPI HbResAllocStartBusEnumeration(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL		*This,
  		IN EFI_HANDLE                                      		RootBridgeHandle,
		OUT VOID                                                **Configuration)
{
	PCI_ROOT_BRG_DATA	*rbdata;
	UINTN				i;
	EFI_STATUS 			Status;
    ASLR_QWORD_ASD      *qw;
//-----------------------------
	if(!Configuration || !This) return EFI_INVALID_PARAMETER;


	rbdata=GetRbByHandle(This,RootBridgeHandle,&i);		
	if(!rbdata){
		PCI_TRACE((TRACE_PCI,"Hb.StartBusEnumeration - Invalid Rb Handle  Passed\n"));
		//ASSERT(rbdata);
		//
		// Invalid RB handle should return EFI_INVALID_PARAMETER as per the PI SPEC
		//
		return EFI_INVALID_PARAMETER;
	}

    //ThisCSP Function MIGHT Update rbdata->RbRes[] with bus ranges to scan
	//and program corresponded RB registers to decode this ranges.
    //If function overwrites BUS ranges this ROOT decodes IT MUST UPDATE  rbdata->RbRes[]!
    //If NOT it should not touch BUS resource descriptor.
	Status=HbCspStartBusEnumeration((PCI_HOST_BRG_DATA*)This, rbdata,i);	
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.StartBusEnumeration - HbCspUpdateBusesBefore returned ERROR!\n"));
		ASSERT_EFI_ERROR(Status);
		return Status;	
	}

	Status=GetResources(rbdata, (ASLR_QWORD_ASD**)Configuration, tResBus);
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.StartBusEnum - RB Resources Invalid!\n"));
		ASSERT_EFI_ERROR(Status);
		return Status; 
	}
    
    qw=(ASLR_QWORD_ASD*)*Configuration;

    PCI_TRACE((TRACE_PCI,"PciRb#%d: Enumerating Buses 0x%lX - 0x%lX \n", i, qw->_MIN, qw->_MIN+qw->_LEN-1));
	PCI_TRACE((TRACE_PCI,"=====================================================================\n\n"));


	((PCI_HOST_BRG_DATA*)This)->EnumStarted=TRUE;
	return EFI_SUCCESS;
}

/**
    This function programs the PCI Root Bridge hardware so that 
    it decodes the specified PCI bus range

       
    @param This Protocol instance
    @param RootBridgeHandle PCI Root Bridge to be configured
    @param Configuration Pointer to the pointer to the PCI bus resource descriptor
    
        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_INVALID_PARAMETER RootBridgeHandle is invalid
    @retval EFI_INVALID_PARAMETER Configuration is NULL
    @retval EFI_INVALID_PARAMETER Configuration does not point to a valid ACPI resource descriptor
    @retval EFI_INVALID_PARAMETER Configuration contains one or more memory or IO ACPI resource descriptor
    @retval EFI_INVALID_PARAMETER Address Range Minimum or Address Range Length fields in Configuration 
        are invalid for this Root Bridge.
    @retval EFI_INVALID_PARAMETER Configuration contains one or more invalid ACPI resource descriptor
    @retval EFI_DEVICE_ERROR Request failed due to hardware error
    @retval EFI_OUT_OF_RESOURCES Request failed due to lack of resources
**/
EFI_STATUS EFIAPI HbResAllocSetBusNumbers(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL		*This,
  		IN EFI_HANDLE                                           RootBridgeHandle,
  		IN VOID                                                 *Configuration)
{
	PCI_ROOT_BRG_DATA	*rbdata;
	UINTN				i;
	EFI_STATUS 			Status;
    ASLR_QWORD_ASD      *qw=(ASLR_QWORD_ASD*)Configuration;
//-----------------------------
	if(!Configuration || !This) return EFI_INVALID_PARAMETER;

	rbdata=GetRbByHandle(This,RootBridgeHandle,&i);		
	if(!rbdata) {
		PCI_TRACE((TRACE_PCI,"Hb.SetBusNumbers - Invalid RB Hndle Passed\n"));
		return EFI_INVALID_PARAMETER;
	}

	Status=SetResources(rbdata, Configuration, tResBus, TRUE);
	
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.SetBusNumbers - Invalid Resoutce Descriptor(s) Passed\n"));
		//ASSERT_EFI_ERROR(Status);
		//return Status;
		return EFI_INVALID_PARAMETER;
	}
	
	//Call CSP function to update RB Buses accordinaly submited information
	Status=HbCspSetBusNnumbers((PCI_HOST_BRG_DATA*)This, rbdata, i);
	
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.SetBusNumbers - HbCspUpdateBusesAfter returned ERROR!\n"));
		ASSERT_EFI_ERROR(Status);
		return Status;	
	}
		
    PCI_TRACE((TRACE_PCI,"PciRb#%d: Assigning Buses 0x%lX - 0x%lX \n", i, qw->_MIN, qw->_MIN+qw->_LEN-1));    

		
	rbdata->BusesSet=TRUE;
	return EFI_SUCCESS;
}

/**
    Cleans internal RB data and free resources allocated trough GCD.
  
        
    @param RootBrg Pointer to RB Private data.

    @retval VOID
**/
VOID EFIAPI FreeUsedResources(PCI_ROOT_BRG_DATA	*RootBrg){
	ASLR_QWORD_ASD		*res;
    UINTN               i;
	DXE_SERVICES		*dxe;	
    EFI_STATUS          Status;
//------------------------------
	Status=LibGetDxeSvcTbl(&dxe);
    ASSERT_EFI_ERROR(Status);

	for(i=0; i<RootBrg->ResCount; i++){
		res=RootBrg->RbRes[i];
        
        //Clean only IO and MMIO resources buses was submitted in different call and must stay.
		if(res->Type < ASLRV_SPC_TYPE_BUS){
            if(res->_MIN != 0){
		        //Free IO
		        if(res->Type==ASLRV_SPC_TYPE_IO){
                    Status=dxe->FreeIoSpace(res->_MIN, res->_LEN);
                } else {
                    Status=dxe->FreeMemorySpace(res->_MIN, res->_LEN);
                }
                ASSERT_EFI_ERROR(Status);
            } //res_MIN
            //Don't free resource list, it will be reupdated with RPLACE option.
        }
    } //for

    //Almost done now clean AcpiRbRes[] data
    pBS->SetMem(&RootBrg->AcpiRbRes[0], sizeof(RootBrg->AcpiRbRes),0);
   
}


/**
    Submits the I/O and memory resource requirements for the specified PCI Root Bridge
  
        
    @param This Protocol instance
    @param RootBridgeHandle PCI Root Bridge to be configured
    @param Configuration Pointer to the pointer to the PCI bus resource descriptor

        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_INVALID_PARAMETER RootBridgeHandle is invalid
    @retval EFI_INVALID_PARAMETER Configuration is NULL
    @retval EFI_INVALID_PARAMETER Configuration does not point to a valid ACPI resource descriptor
    @retval EFI_INVALID_PARAMETER Configuration includes a resource descriptor of unsupported type
**/
EFI_STATUS EFIAPI HbResAllocSubmitResources(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  		*This,
  		IN EFI_HANDLE                                               RootBridgeHandle,
	  	IN VOID                                                     *Configuration)
{
	PCI_ROOT_BRG_DATA	*rbdata;
	UINTN				i;
	EFI_STATUS 			Status,AllocStatus;
	UINT64          Attributes;
    ASLR_QWORD_ASD  *qw=(ASLR_QWORD_ASD*)Configuration;
//-----------------------------
	if(!Configuration || !This) return EFI_INVALID_PARAMETER;

	rbdata=GetRbByHandle(This,RootBridgeHandle,&i);		
	if(!rbdata) {
		PCI_TRACE((TRACE_PCI,"Hb.SubmitResources - Invalid RB Hndle Passed\n"));
		return EFI_INVALID_PARAMETER;
	}

	//Update Internal data structures with Resource Request
    Status=SetResources(rbdata, Configuration, tResIoMem, TRUE);
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.SubmitResources - Invalid Resoutce Descriptor(s) Passed\n"));
		//ASSERT_EFI_ERROR(Status);
		//return Status;
		return EFI_INVALID_PARAMETER;
	}
    Status = HbResAllocGetAllocAttributes(This,RootBridgeHandle,&Attributes);
    
	// Refered from RefPkg
    // this root bridge did not support separate windows for non-prefetchable
    // and prefetchable memory.
	//  
    if ( (Attributes == EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM) && (qw->TFlags.TFLAGS == 0x06) ) {
      return EFI_INVALID_PARAMETER;
    }

	//Call CSP function to update RB Resources Windows
	Status=HbCspSubmitResources((PCI_HOST_BRG_DATA*)This, rbdata,i);
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.SubmitResources - HbCspSubmitResources return an ERROR!\n"));
		ASSERT_EFI_ERROR(Status);
		return Status;	
	}
	rbdata->ResSubmited=TRUE; //set the FLAG

    //Check if we are able to accomodate resource request 
    //CSP Routine First...
    PCI_TRACE((TRACE_PCI,"PciRb: Calling CSP HbCspAllocateResources() - returned "));        
    AllocStatus=HbCspAllocateResources((PCI_HOST_BRG_DATA*)This, rbdata,i);
    PCI_TRACE((TRACE_PCI,"%r\n", AllocStatus));        
    
    if(EFI_ERROR(AllocStatus)){    
        if( AllocStatus==EFI_UNSUPPORTED ){
            PCI_TRACE((TRACE_PCI,"PciRb: Calling Generic AllocateResources() - returned "));
            AllocStatus=AllocateResources(rbdata);
            PCI_TRACE((TRACE_PCI,"%r\n", AllocStatus));
        }  
        //If custom function Fails or Generic on report it.
        if(EFI_ERROR(AllocStatus)){
            PCI_TRACE((TRACE_PCI,"Hb.SubmitResources - Fail to allocate resources AllocStatus = %r\n",AllocStatus));        
            return EFI_INVALID_PARAMETER;
        }
    } else {   
        //Custom function Returned Correct Status:
        //Check if Above 4G Memory was correctly updated by custom function 
        //and if it match with Above4g setup question?
        if((gPciCommonSetup->Above4gDecode==0) && (rbdata->AcpiRbRes[raMmio64].Len!=0)){
            rbdata->AcpiRbRes[raMmio64].Min=0;
            rbdata->AcpiRbRes[raMmio64].Max=0;
            rbdata->AcpiRbRes[raMmio64].Len=0;
            rbdata->AcpiRbRes[raMmio64].AddrUsed=0;
            rbdata->AcpiRbRes[raMmio64].AllocType=0;
            PCI_TRACE((TRACE_PCI,"PciRB:  PCI Above 4G Decode Setup Settings and Custom Alloc Function Data Mismatched\n"));
        }
    }
    
    //Check if we can use previoce MRC settings 
    //if(!mc || EFI_ERROR(Status)){ 
    Status = HbCspAdjustMemoryMmioOverlap((PCI_HOST_BRG_DATA*)This, rbdata,i );

	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.SubmitResources - AdjustMmioOverlap() returned %r\n",Status));
        //AdjustMmioOverlap() must return SUCCESS or Reset the system...
		ASSERT_EFI_ERROR(Status);
    }

    //Reflecting What TYPE and HOW MUCH of resouces is not enough...
    if(EFI_ERROR(AllocStatus)) {
        ((PCI_HOST_BRG_DATA*)This)->AllocPhase--;
        FreeUsedResources(rbdata);
        Status=AllocStatus;
    }

	if(EFI_ERROR(Status))return Status;

	rbdata->ResAsquired=TRUE; //set the FLAG
		
	return EFI_SUCCESS;
}


/**
    This function returns the proposed resource settings for the specified 
    PCI Root Bridge

        
    @param This Protocol instance
    @param RootBridgeHandle PCI Root Bridge to be configured
    @param Configuration Pointer to the pointer to the PCI bus resource descriptor

        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_INVALID_PARAMETER RootBridgeHandle is invalid
    @retval EFI_DEVICE_ERROR Request failed due to hardware error
    @retval EFI_OUT_OF_RESOURCES Request failed due to lack of resources
**/
EFI_STATUS EFIAPI HbResAllocGetProposedResources(
  		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL			*This,
		IN EFI_HANDLE                                              	RootBridgeHandle,
		OUT VOID                                                    **Configuration)
{

	PCI_ROOT_BRG_DATA	*rbdata;
	UINTN				i;
	EFI_STATUS 			Status;
//-----------------------------
	if(!Configuration || !This) return EFI_INVALID_PARAMETER;

	rbdata=GetRbByHandle(This,RootBridgeHandle,&i);		
	if(!rbdata) {
		PCI_TRACE((TRACE_PCI,"Hb.GetProposedResources - Invalid RB Handle Passed!\n"));
		return EFI_INVALID_PARAMETER;
	}

	//Call CSP function to update RB Resources Windows
	Status=HbCspGetProposedResources((PCI_HOST_BRG_DATA*)This, rbdata,i);
	
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.GetProposedResources - HbCspGetProposedResources returned %r\n", Status));
		ASSERT_EFI_ERROR(Status);
		return Status;	
	}
	
	Status=GetResources(rbdata, (ASLR_QWORD_ASD**)Configuration, tResAll);
	if(EFI_ERROR(Status)){
		PCI_TRACE((TRACE_PCI,"Hb.GetProposedResources - RB Resources Invalid! %r\n", Status));
		ASSERT_EFI_ERROR(Status);
	}

	return Status;	
		
}


/**
    This function is called for all the PCI controllers that the PCI 
    bus driver finds. Can be used to Preprogram the controller.

           
        
    @param This Protocol instance
    @param RootBridgeHandle PCI Root Bridge to be configured
    @param PciBusAddress Address of the controller on the PCI bus
    @param Phase The Phase during resource allocation
    
        EFI_STATUS
    @retval EFI_SUCCESS Success
    @retval EFI_INVALID_PARAMETER RootBridgeHandle is invalid
    @retval EFI_DEVICE_ERROR Device pre-initialization failed due to hardware error.
**/
EFI_STATUS EFIAPI HbResAllocPreprocessController(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  		*This,
		IN EFI_HANDLE                                               RootBridgeHandle,
  		IN EFI_PCI_CONFIGURATION_ADDRESS              				PciAddress,
		IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE             Phase)
{
//	EFI_STATUS	Status=0;
	UINTN				i;
	PCI_ROOT_BRG_DATA	*rb;
//-----------------------------------		
	rb=GetRbByHandle(This,RootBridgeHandle,&i);
	//ASSERT(rb)
	if(!rb) return EFI_INVALID_PARAMETER;
	//Call CSP function...
	return HbCspPreprocessController((PCI_HOST_BRG_DATA*)This,rb,i,PciAddress,Phase);
}


/**
    Enables timer event to poll PCI IO and MEMORY.

           
        
    @param Delay Delay value in us
    
    @retval EFI_EVENT if Success
        NULL if unable to set timer event. 
**/
EFI_EVENT EFIAPI SetTmr(UINT64 Delay)
{
	EFI_STATUS	Status;
	EFI_EVENT 	evt;
//--------------------------------------------
	Status=pBS->CreateEvent(EVT_TIMER,TPL_NOTIFY,NULL,NULL,&evt);
	if (EFI_ERROR(Status)) return NULL;


	Status = pBS->SetTimer(evt, TimerRelative, Delay);
	if (EFI_ERROR(Status)){
		pBS->CloseEvent(evt);
		evt=NULL;
	}
	return evt;
}

/**
    Disables and Clear timer event to poll PCI IO and MEMORY.

           
        
    @param TmrEvt Timer event to close.
    
    @retval Nothing
**/
VOID EFIAPI KillTmr(EFI_EVENT TmrEvt)
{
	pBS->CloseEvent(TmrEvt);
}


/**
    Poll Memory for a value or until times out.

    @param IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*		- This
        IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	- Memory Width
        IN  UINT64		- Memory Address
        IN  UINT64		- Bit Mask
        IN  UINT64		- Value for exit
        IN  UINT64		- Timout
        OUT UINT64 *	- Contents of memory

        EFI_STATUS
    @retval EFI_SUCCESS Found value
    @retval EFI_TIMEOUT Did not find value.
        EFI_INVALID_PARAMETER

    @note  None

**/
EFI_STATUS EFIAPI PollMem (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN  UINT64		Address,
	IN  UINT64		Mask,
	IN  UINT64		Value,
	IN  UINT64		Delay,
	OUT UINT64		*Result)
{
	EFI_STATUS	Status;
	EFI_EVENT	Event;

	if (Result==NULL ||((UINTN)Width>=(UINTN)EfiPciWidthFifoUint8)) return EFI_INVALID_PARAMETER;

	Status = PciMemRead(This, Width, Address, 1, Result);	//Read memory at least once.
	if (EFI_ERROR(Status)) return Status;

	if ((*Result & Mask)==Value) return EFI_SUCCESS;			//If correct value, exit.
	if (Delay == 0)	return EFI_TIMEOUT;

	Event=SetTmr(Delay);
	if (!Event) return EFI_NOT_AVAILABLE_YET;

	while(pBS->CheckEvent(Event)==EFI_NOT_READY){
		Status = PciMemRead(This, Width, Address, 1, Result);
		if (EFI_ERROR(Status)) break;
		if ((*Result & Mask)==Value){
			Status = EFI_SUCCESS;
			break;
		} else Status=EFI_TIMEOUT;	//correct value?
	}
	KillTmr(Event);		//Correct value not read, in time alloted.
	return Status;
}

/**
    Poll IO for a value or until times out.

    @param IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*		- This
        IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	- Memory Width
        IN  UINT64		- Memory Address
        IN  UINT64		- Bit Mask
        IN  UINT64		- Value for exit
        IN  UINT64		- Timout
        OUT UINT64 *	- Contents of IO

        EFI_STATUS
    @retval EFI_SUCCESS Found value
    @retval EFI_TIMEOUT Did not find value.
        EFI_INVALID_PARAMETER

    @note  None

**/
EFI_STATUS EFIAPI PollIo (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN  UINT64		Address,
	IN  UINT64		Mask,
	IN  UINT64		Value,
	IN  UINT64		Delay,
	OUT UINT64		*Result)
{
	EFI_STATUS	Status;
	EFI_EVENT	Event;

	if (Result==NULL) return EFI_INVALID_PARAMETER;
	if ((UINTN)Width >= (UINTN)EfiPciWidthFifoUint8) return EFI_INVALID_PARAMETER;


	Status = PciIoRead(This, Width, Address, 1, Result);	//Read memory at least once.
	if (EFI_ERROR(Status)) return Status;

	if ((*Result & Mask)==Value) return EFI_SUCCESS;			//If correct value, exit.
	if (Delay == 0)	return EFI_TIMEOUT;

	Event=SetTmr(Delay);
	if (!Event) return EFI_NOT_AVAILABLE_YET;

	while(pBS->CheckEvent(Event)==EFI_NOT_READY){
		Status = PciIoRead(This, Width, Address, 1, Result);
		if (EFI_ERROR(Status)) break;
		if ((*Result & Mask)==Value){
			Status = EFI_SUCCESS;
			break;
		} else Status=EFI_TIMEOUT;	//correct value?
	}
	KillTmr(Event);		//Correct value not read, in time alloted.
	return Status;
}

/**
    Read memory IO into buffer.

    @param IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*		- This
        IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	- Memory Width
        IN UINT64		- Memory Address
        IN UINTN		- Number of width reads.
        IN OUT VOID *	- Buffer where memory is read into.

        EFI_STATUS
    @retval EFI_SUCCESS Successful read.
        EFI_INVALID_PARAMETER

    @note  None

**/
EFI_STATUS EFIAPI PciMemRead (
IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
IN     UINT64		Address,
IN     UINTN		Count,
IN OUT VOID			*Buffer)
{
	UINTN	IncrementType		= Width & ~3;
	UINT8	IncrementValue		= 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
	UINT8	IncrementBuffer		= 0;
	UINT8	IncrementAddress	= 0;

	if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
	if (IncrementType > 16 || IncrementValue > 8) return EFI_INVALID_PARAMETER;

    //if FIFO Address falls out of IO Range
    if(sizeof(UINTN)==4){
    	if (Address  > 0xffffffff) return EFI_INVALID_PARAMETER;	
    }
    
	switch (IncrementType)
	{
	case 0:	//EfiPciWidthUintxx
		IncrementAddress = IncrementBuffer = IncrementValue;
		break;
	case 4: //EfiPciWidthFifoUintxx
		IncrementBuffer = IncrementValue;
		break;
	default: //EfiPciWidthFillUintxx
		IncrementAddress = IncrementValue;
	}

    //Exclude FIFO operation from address increase check
    if(IncrementAddress) {
        if(sizeof(UINTN)==8){
    	    if (( 0xffffffffffffffff - Count * IncrementValue) <= Address)
    			return EFI_INVALID_PARAMETER;						
        } else {
    	    if ((Address + Count * IncrementValue) > 0xffffffff)
    			return EFI_INVALID_PARAMETER;						
        }
    }

	while(Count--)
	{
		switch(IncrementValue)
		{
		case 1:		//byte
			*(UINT8*) Buffer = *(UINT8*)Address;
			break;
		case 2:		//word
			*(UINT16*) Buffer = *(UINT16*)Address;
			break;
		case 4: 	//dword
			*(UINT32*) Buffer = *(UINT32*)Address;
			break;		
		case 8:	//dword
			*(UINT64*) Buffer = *(UINT64*)Address;
			break;
		default: return EFI_INVALID_PARAMETER;
		}
//		(UINT8*) Buffer += IncrementBuffer;
		Buffer = (UINT8*)Buffer + IncrementBuffer; 
		Address += IncrementAddress;
	}

	return EFI_SUCCESS;
}

/**
    Write memory IO from buffer.

    @param IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*		- This
        IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	- Memory Width
        IN UINT64		- Memory Address
        IN UINTN		- Number of width writes.
        IN OUT VOID *	- Buffer where memory is written from.

        EFI_STATUS
    @retval EFI_SUCCESS Successful write.
        EFI_INVALID_PARAMETER

    @note  None

**/
EFI_STATUS EFIAPI PciMemWrite (
IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
IN     UINT64		Address,
IN     UINTN		Count,
IN OUT VOID			*Buffer)
{
	UINTN	IncrementType		= Width & ~3;
	UINT8	IncrementValue		= 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
	UINT8	IncrementBuffer		= 0;
	UINT8	IncrementAddress	= 0;

	if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
	if (IncrementType > 16 || IncrementValue > 8) return EFI_INVALID_PARAMETER;

    //if FIFO Address falls out of IO Range
    if(sizeof(UINTN)==4){
    	if (Address  > 0xffffffff) return EFI_INVALID_PARAMETER;	
    }
    
	switch (IncrementType)
	{
	case 0:	//EfiPciWidthUintxx
		IncrementAddress = IncrementBuffer = IncrementValue;
		break;
	case 4: //EfiPciWidthFifoUintxx
		IncrementBuffer = IncrementValue;
		break;
	default: //EfiPciWidthFillUintxx
		IncrementAddress = IncrementValue;
	}

    //Exclude FIFO operation from buffer increase check
    if(IncrementAddress) {
        if(sizeof(UINTN)==8){
    	    if (( 0xffffffffffffffff - Count * IncrementValue) <= Address)
    			return EFI_INVALID_PARAMETER;						
        } else {
    	    if ((Address + Count * IncrementValue) > 0xffffffff)
    			return EFI_INVALID_PARAMETER;						
        }
    }
    

	while(Count--)
	{
		switch(IncrementValue)
		{
		case 1:		//byte
			*(UINT8*) Address = *(UINT8*)Buffer;
			break;
		case 2:		//word
			*(UINT16*) Address = *(UINT16*)Buffer;
			break;
		case 4:	//dword
			*(UINT32*) Address = *(UINT32*)Buffer;
			break;
		case 8:	//dword
			*(UINT64*) Address = *(UINT64*)Buffer;
			break;
		default: return EFI_INVALID_PARAMETER;
		}
		//(UINT8*) Buffer += IncrementBuffer;
		Buffer = (UINT8*)Buffer + IncrementBuffer; 
		Address += IncrementAddress;
	}

	return EFI_SUCCESS;
}

/**
    Read IO into buffer.

    @param 
        IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*		- This
        IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	- Io Width
        IN UINT64		- Io Address
        IN UINTN		- Number of width reads.
        IN OUT VOID *	- Buffer where Io is read into.

        EFI_STATUS
    @retval EFI_SUCCESS Successful read.
        EFI_INVALID_PARAMETER

    @note  None

**/
EFI_STATUS EFIAPI PciIoRead (
IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
IN     UINT64		Address,
IN     UINTN		Count,
IN OUT VOID			*Buffer)
{
	UINTN	IncrementType		= Width & ~3;
	UINT8	IncrementValue		= 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
	UINT8	IncrementBuffer		= 0;
	UINT8	IncrementAddress	= 0;

	if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
	if (IncrementType > 16 || IncrementValue > 8) return EFI_INVALID_PARAMETER;

    //if FIFO Address falls out of IO Range
	if (Address  > 0xffff)return EFI_INVALID_PARAMETER;	

	switch (IncrementType)
	{
	case 0:	//EfiPciWidthUintxx
		IncrementAddress = IncrementBuffer = IncrementValue;
		break;
	case 4: //EfiPciWidthFifoUintxx
		IncrementBuffer = IncrementValue;
		break;
	default: //EfiPciWidthFillUintxx
		IncrementAddress = IncrementValue;
	}

    //Exclude FIFO operation from buffer increase check
    if(IncrementAddress) {
        if(Io32support){
            if ((Address + Count * IncrementValue) > 0xffffffff)
                return EFI_INVALID_PARAMETER;   
        } else {
            //IO must be within range of the bridge.
            if ((Address + Count * IncrementValue) > 0xffff)
                return EFI_INVALID_PARAMETER;	
        }
    }

	while(Count--)
	{
		switch(IncrementValue)
		{
		case 1:		//byte
			*(UINT8*) Buffer = IoRead8((UINT16)Address);
			break;
		case 2:		//word
			*(UINT16*) Buffer = IoRead16((UINT16)Address);
			break;
		case 4:	//dword
			*(UINT32*) Buffer = IoRead32((UINT16)Address);
			break;
		case 8:
			*(UINT32*) Buffer = IoRead32((UINT16)Address);
			*((UINT32*)((UINT32*)Buffer+1)) = IoRead32((UINT16)(Address+4));
			break;
		default: return EFI_INVALID_PARAMETER;
		}
//		(UINT8*) Buffer += IncrementBuffer;
		Buffer = (UINT8*)Buffer + IncrementBuffer; 
		Address += IncrementAddress;
	}

	return EFI_SUCCESS;
}

/**
    Write IO from buffer.

        
    @param This Protocol instance
    @param Width Pci Width
    @param Address IO Address
    @param Count Number of width writes.
    @param Buffer where Pci registers are written from.

        EFI_STATUS
    @retval EFI_SUCCESS Successful write.
        EFI_INVALID_PARAMETER

**/
EFI_STATUS EFIAPI PciIoWrite (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
	IN     UINT64		Address,
	IN     UINTN		Count,
	IN OUT VOID			*Buffer)
{
	UINTN	IncrementType		= Width & ~3;
	UINT8	IncrementValue		= 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
	UINT8	IncrementBuffer		= 0;
	UINT8	IncrementAddress	= 0;

	if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
	if (IncrementType > 16 || IncrementValue > 4) return EFI_INVALID_PARAMETER;

    //if FIFO Address falls out of IO Range
	if (Address  > 0xffff)return EFI_INVALID_PARAMETER;	

	switch (IncrementType)
	{
	case 0:	//EfiPciWidthUintxx
		IncrementAddress = IncrementBuffer = IncrementValue;
		break;
	case 4: //EfiPciWidthFifoUintxx
		IncrementBuffer = IncrementValue;
		break;
	default: //EfiPciWidthFillUintxx
		IncrementAddress = IncrementValue;
	}

    //Exclude FIFO operation from buffer increase check
    if(IncrementAddress) {
        if(Io32support){
            if ((Address + Count * IncrementValue) > 0xffffffff)
                return EFI_INVALID_PARAMETER;   
        } else {
            //IO must be within range of the bridge.
            if ((Address + Count * IncrementValue) > 0xffff)
                return EFI_INVALID_PARAMETER;   
        }
    }

	while(Count--)
	{
		switch(IncrementValue)
		{
		case 1:		//byte
			 IoWrite8((UINT16)Address,*(UINT8*) Buffer);
			break;
		case 2:		//word
			 IoWrite16((UINT16)Address,*(UINT16*) Buffer);
			break;
		default:	//dword
			 IoWrite32((UINT16)Address,*(UINT32*) Buffer);
			break;
		}
		//(UINT8*) Buffer += IncrementBuffer;
		Buffer = (UINT8*)Buffer + IncrementBuffer; 
		Address += IncrementAddress;
	}

	return EFI_SUCCESS;
}

/**
    Copy PCI memory to PCI memory in bridge.

        
    @param This Protocol instance
    @param Width Pci Width
    @param DestAddress Pci Address
    @param SrcAddress Pci Address
    @param Count Number of width writes.
    @param Buffer where Pci registers are written from.

        EFI_STATUS
    @retval EFI_SUCCESS Successful copy.
        EFI_INVALID_PARAMETER

**/
EFI_STATUS EFIAPI PciCopyMem (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN  UINT64									DestAddress,
	IN  UINT64									SrcAddress,
	IN  UINTN									Count)
{
	INTN	IncrementValue;

	if ((SrcAddress == 0) || (DestAddress==0)) return EFI_INVALID_PARAMETER;
	if	((UINTN)Width > (UINTN)EfiPciWidthUint64) return EFI_INVALID_PARAMETER;

	IncrementValue = (INTN)1<<(Width&3);

    if(sizeof(UINTN)==8){
	    if (( 0xffffffffffffffff - Count * IncrementValue) <= SrcAddress)
			return EFI_INVALID_PARAMETER;						
    } else {
	    if ((SrcAddress + Count * IncrementValue) > 0xffffffff)
			return EFI_INVALID_PARAMETER;						
    }

    if(sizeof(UINTN)==8){
	    if (( 0xffffffffffffffff - Count * IncrementValue) <= DestAddress)
			return EFI_INVALID_PARAMETER;						
    } else {
	    if ((DestAddress + Count * IncrementValue) > 0xffffffff)
			return EFI_INVALID_PARAMETER;						
    }

	if	(SrcAddress==DestAddress) return EFI_SUCCESS;			//Nothing to do.


	if ( (DestAddress > SrcAddress) && (SrcAddress + Count * IncrementValue) ) {
		SrcAddress += (Count-1) * IncrementValue;		//End of source
		DestAddress += (Count-1) * IncrementValue;		//End of destination
		IncrementValue = -IncrementValue;				//Count backwards.
	}	//If Destination addres and after and overlapps Source Address, Copy backwards.

	switch(IncrementValue)
	{
	case 1:
    case -1:
		while (Count--)
		{
			*(UINT8*) DestAddress = *(UINT8*)SrcAddress;
			SrcAddress += IncrementValue;
			DestAddress += IncrementValue;
		}
		break;
	case 2:
    case -2:
		while (Count--)
		{
			*(UINT16*) DestAddress = *(UINT16*)SrcAddress;
			SrcAddress += IncrementValue;
			DestAddress += IncrementValue;
		}
		break;
	default:
		while (Count--)
		{
			*(UINT32*) DestAddress = *(UINT32*)SrcAddress;
			SrcAddress += IncrementValue;
			DestAddress += IncrementValue;
		}
		break;
	}
	return EFI_SUCCESS;
}

/**
    Read Pci registers into buffer.

        
    @param This Protocol instance
    @param Width Pci Width
    @param Address Pci Address
    @param Count Number of width writes.
    @param Buffer where Pci registers are written from.

        EFI_STATUS
    @retval EFI_SUCCESS Successful read.
        EFI_INVALID_PARAMETER

**/
EFI_STATUS EFIAPI PciRead (
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
	IN UINT64		Address,
	IN UINTN		Count,
	IN OUT VOID		*Buffer)
{
	PCI_ROOT_BRG_DATA	*Private = (PCI_ROOT_BRG_DATA*)This;

	if (Buffer == NULL ||((UINTN)Width>=(UINTN)EfiPciWidthMaximum) ) return EFI_INVALID_PARAMETER;

	return RootBridgeIoPciRW (Private, Width, Address, Count, Buffer, FALSE);
}

/**
    Write Pci registers from buffer.

        
    @param This Protocol instance
    @param Width Pci Width
    @param Address Pci Address
    @param Count Number of width writes.
    @param Buffer where Pci registers are written from.

        EFI_STATUS
    @retval EFI_SUCCESS Successful write.
        EFI_INVALID_PARAMETER

**/
EFI_STATUS EFIAPI PciWrite (
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
	IN UINT64		Address,
	IN UINTN		Count,
	IN OUT VOID		*Buffer)
{
	PCI_ROOT_BRG_DATA	*Private = (PCI_ROOT_BRG_DATA*)This;

	if (Buffer == NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;

	Private = (PCI_ROOT_BRG_DATA *) This;

	return RootBridgeIoPciRW (Private, Width, Address, Count, Buffer, TRUE);
}

/**
    Provide addresses required to access system memory from a DMA
    bus master.

        
    @param This 
    @param Operation 
    @param HostAddress Host address
    @param NumberOfBytes Number of bytes
    @param DeviceAddress Device Address
    @param Mapping 

        EFI_STATUS
    @retval EFI_SUCCESS Successful map

    @note  None

**/
EFI_STATUS EFIAPI Map (
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
	IN VOID                         *HostAddress,
	IN OUT UINTN                    *NumberOfBytes,
	OUT EFI_PHYSICAL_ADDRESS        *DeviceAddress,
	OUT VOID                        **Mapping)
{
//    EFI_STATUS					Status=EFI_SUCCESS;
//	PCI_ROOT_BRIDGE_MAPPING		*mapping;
	EFI_PHYSICAL_ADDRESS		addr;
//-------------------------------------------------

	if ( !HostAddress || !NumberOfBytes || !DeviceAddress || !Mapping )
		return EFI_INVALID_PARAMETER;

	if ((UINT32)Operation >= (UINT32)EfiPciOperationMaximum ) return EFI_INVALID_PARAMETER;

	*Mapping=NULL;

	addr=(EFI_PHYSICAL_ADDRESS) (UINTN) HostAddress;

	if ((addr + *NumberOfBytes) > 0xffffffff ){
		return RbCspIoPciMap((PCI_ROOT_BRG_DATA*)This,Operation, addr, NumberOfBytes, DeviceAddress, Mapping);
	} else *DeviceAddress = addr;

	return EFI_SUCCESS;
}

/**
    Remove mapping

         
    @param This Protocol Instance
    @param Mapping Pointer to Mapping formation to Unmap

        EFI_STATUS
    @retval EFI_SUCCESS Successful unmapping.
        EFI_INVALID_PARAMETER

**/
EFI_STATUS EFIAPI Unmap (
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN VOID		*Mapping)

{
//	EFI_STATUS	Status=EFI_SUCCESS;
	PCI_ROOT_BRIDGE_MAPPING	*mapping=(PCI_ROOT_BRIDGE_MAPPING*)Mapping;
//-------

	if (mapping!=NULL) return RbCspIoPciUnmap((PCI_ROOT_BRG_DATA*)This,mapping);
	
	return EFI_SUCCESS;
}

/**
    Allocate buffer for PCI use.

        
    @param This Protocol Instance
    @param Type Alloocation Type (ignored and uses AllocateMaxAddress)
    @param MemoryType Memory Type (ex. EfiBootServicesData)
    @param Pages Number of pages to allocate.
    @param HostAddress Host Address
    @param Attributes Allocation Attributes.

        EFI_STATUS
    @retval EFI_SUCCESS Successful allocation.
    @retval EFI_UNSUPPORTED Attribute not supported.
        EFI_INVALID_PARAMETER

**/
#define BUFF_ATTR 	(EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE |\
					EFI_PCI_ATTRIBUTE_MEMORY_CACHED |\
					EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE)
//---------------------------------------------------------------
EFI_STATUS EFIAPI AllocateBuffer (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN  EFI_ALLOCATE_TYPE	Type,
	IN  EFI_MEMORY_TYPE		MemoryType,
	IN  UINTN				Pages,
	OUT VOID				**HostAddress,
	IN  UINT64				Attributes)

{
	//PCI_ROOT_BRIDGE_STRUCT	*Private = (PCI_ROOT_BRIDGE_STRUCT	*)This;

	EFI_STATUS				Status;
	EFI_PHYSICAL_ADDRESS	addr;

	if (HostAddress == NULL) return EFI_INVALID_PARAMETER;

	// The only valid memory types are EfiBootServicesData and EfiRuntimeServicesData
	if (MemoryType != EfiBootServicesData && MemoryType != EfiRuntimeServicesData)
		return EFI_INVALID_PARAMETER;

	//Check if legal Attributes for this function is passed
	//Check if any other Attributes set except supported
	if( Attributes & ~BUFF_ATTR ) return EFI_UNSUPPORTED;


	//865 chipset does not support any of Attributes above so as spec. says ignore it
	//and Allocate buffer
	// Limit allocations to memory below 4GB
	addr = 0xffffffff;

	Status = pBS->AllocatePages (AllocateMaxAddress, MemoryType, Pages, &addr);
	if (EFI_ERROR(Status)) return Status;

	*HostAddress = (VOID *)(UINTN)addr;
	return EFI_SUCCESS;
}

/**
    Frees buffer

    @param This Protocol Instance
    @param Pages Number of pages to free.
    @param HostAddress Host Address

        EFI_STATUS
    @retval EFI_SUCCESS Successful free.
        EFI_INVALID_PARAMETER

**/
EFI_STATUS EFIAPI FreeBuffer (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN  UINTN		Pages,
	IN  VOID		*HostAddress)

{
	return pBS->FreePages((EFI_PHYSICAL_ADDRESS) HostAddress, Pages);
}

/**
    Flush buffer used by PCI DMA Transfere.

        
    @param This Protocol Instance

        EFI_STATUS
    @retval EFI_SUCCESS Successful flushing

**/
EFI_STATUS EFIAPI Flush (IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *This)
{
	return EFI_SUCCESS;		//Doesn't need buffer to be flushed.
}

/**
    Get Supported and Current Root Bridge Attributes.

        
    @param This Protocol Instance
    @param Supported Supported Attributes
    @param Attributes Current Attributes

        EFI_STATUS
    @retval EFI_SUCCESS Successful reading attributes.
    @retval EFI_INVALID_PARAMETER If both for attributes are invalid.

    @note  None

**/
EFI_STATUS EFIAPI GetAttributes (
	IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	OUT UINT64	*Supported,
	OUT UINT64	*Attributes)
{
	PCI_ROOT_BRG_DATA	*Private = (PCI_ROOT_BRG_DATA*) This;

	if (Attributes == NULL && Supported == NULL) return EFI_INVALID_PARAMETER;

	if (Supported) *Supported  = Private->Supports;
	if (Attributes) *Attributes = Private->Attributes;

	return EFI_SUCCESS;
}

/**
    Get Supported and Current Root Bridge Attributes.

        
    @param This Protocol Instance
    @param Attributes 
    @param ResourceBase Resource Base (optional, unused)
    @param ResourceLength Resource Length (optional, unused)

    @retval EFI_STATUS
        EFI_SUCCESS     Successful setting attributes.
        EFI_UNSUPPORTED Unsupported attributes

    @note  None

**/

#define PCI_ATTRIBUTE_VALID_FOR_RESOURCE_BUFFER (EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE | EFI_PCI_ATTRIBUTE_MEMORY_CACHED | EFI_PCI_ATTRIBUTE_MEMORY_DISABLE)

EFI_STATUS EFIAPI SetAttributes (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
	IN     UINT64	Attributes,
	IN OUT UINT64	*ResourceBase,
	IN OUT UINT64	*ResourceLength)
{
	EFI_STATUS			Status = EFI_SUCCESS;
	PCI_ROOT_BRG_DATA	*Private  = (PCI_ROOT_BRG_DATA *) This;
//------------------------------------
	
//    PCI_TRACE((TRACE_PCI, "\nPciRb: SetAttributes('%a'; Attr=0x%lX; Capab=0x%lX; ResBs=0x%lX; ResLn=0x%lX); Status=", 
//    Private->RbAslName, Attributes, Private->Supports, ResourceBase, ResourceLength));

    //First Check if RB supports passed Attributes
	if((Attributes & Private->Supports)!=Attributes) Status = EFI_UNSUPPORTED;

	//Then check oprional parameters if eligable
	if ( !EFI_ERROR(Status) && (Attributes & PCI_ATTRIBUTE_VALID_FOR_RESOURCE_BUFFER) ){
	    if(ResourceBase==NULL || ResourceLength==NULL) Status = EFI_INVALID_PARAMETER;
	}

	if(EFI_ERROR(Status)) {
//        PCI_TRACE((TRACE_PCI, "%r.\n", Status));
        return Status;    
    }
    
	//Note: Only some attributes can be both enabled and disabled.
	Status=RbCspIoPciAttributes(Private, Attributes, ResourceBase, ResourceLength);
	
//	PCI_TRACE((TRACE_PCI, "%r.\n", Status));

	if(EFI_ERROR(Status)) return Status;	

	//Update the attributes property
	//Private->Attributes&=Attributes; //reset all bits that has to be reseted
	Private->Attributes |= Attributes; //set bits that has to be seted

	return Status;
}
//--------------------------------------------------

/**
    Returns, using ACPI 2.0 Descriptors, resources allocated.

    @param This 
    @param Resources 

    @retval EFI_STATUS
        EFI_SUCCESS	

**/
EFI_STATUS EFIAPI Configuration (
	IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	*This,
	OUT    VOID								**Resources)
{
	PCI_ROOT_BRG_DATA	*rb=(PCI_ROOT_BRG_DATA*)This;
	EFI_STATUS			Status=0;
//---------------------
	if(rb->ResAsquired)	
		Status=GetResources((PCI_ROOT_BRG_DATA*)This, (ASLR_QWORD_ASD**)Resources, tResAll);
	else Status=EFI_NOT_AVAILABLE_YET;
	return Status;
}


/**
    Provides Read and Write access to the PCI Configuration Space.

         
    @param This Protocol Instance
    @param Width Data Width.
    @param Address PCI Config Address
    @param Mask Mask for the bits to be preserved
    @param Value Data to be written

        EFI_STATUS
    @retval EFI_SUCCESS OK
    @retval EFI_DEVICE_ERROR HW ERROR
**/
VOID EFIAPI PciRw (
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL			*This,
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN UINT64	Address,
	IN UINT32	Mask,
	IN UINT32	Value
	)
{
	EFI_STATUS Status;
	UINT32	Data;

	Status = This->Pci.Read (This,Width,Address,1,&Data);
	ASSERT_EFI_ERROR (Status);

	Data = (Data & Mask) | Value;
	Status = This->Pci.Write (This,Width,Address,1,&Data);
	ASSERT_EFI_ERROR (Status);
} 




//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
