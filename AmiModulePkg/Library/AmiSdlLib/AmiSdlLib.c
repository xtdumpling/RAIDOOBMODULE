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
//<AMI_FHDR_START>
//
// Name:  AmiSioDxeLib.C
//
// Description: Library Class for AMI SIO Driver.
//
//
//<AMI_FHDR_END>
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Protocol/AmiBoardInitPolicy.h>


//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
AMI_BOARD_INFO2_PROTOCOL	*gAmiBoardInfo2Protocol=NULL;
AMI_SDL_PCI_DATA			*gSdlPciData=NULL;
AMI_SDL_SIO_DATA			*gSdlSioData=NULL;



//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlInitBoardInfo()
{
	EFI_STATUS				Status=EFI_SUCCESS;
//-------------------------------------------------

	if(gAmiBoardInfo2Protocol==NULL){
		Status = pBS->LocateProtocol(&gAmiBoardInfo2ProtocolGuid, NULL,(VOID**)&gAmiBoardInfo2Protocol);
		if(EFI_ERROR(Status)){
			TRACE((TRACE_ALWAYS,"ERROR: Failed to locate AMI_BOARD_INFO2_PROTOCOL. Status=%r\n",Status));
			ASSERT_EFI_ERROR(Status);
		} else {
			gSdlPciData=gAmiBoardInfo2Protocol->PciBrdData;
			gSdlSioData=gAmiBoardInfo2Protocol->SioBrdData;
		}
	}

	return Status;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlPciGetHostBridges(AMI_SDL_PCI_DEV_INFO ***HostBridgesBuffer, UINTN *HostBridgesCount)
{
	EFI_STATUS				Status;
	UINTN					i, cnt;
	AMI_SDL_PCI_DEV_INFO	*dev;
	AMI_SDL_PCI_DEV_INFO	**buffer;
//-----------------------

	if(HostBridgesBuffer==NULL || HostBridgesCount==NULL) return EFI_INVALID_PARAMETER;

	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)) return Status;

	for (i=0, cnt=0;i<gSdlPciData->PciDevCount; i++){
		dev=&gSdlPciData->PciDevices[i];
		if(dev->PciDevFlags.Bits.HostBridgeType)
			cnt++;
	}

	if (cnt==0) return EFI_NOT_FOUND;

	buffer=Malloc(sizeof(AMI_SDL_PCI_DEV_INFO*)*cnt);
	if(buffer==NULL) return EFI_OUT_OF_RESOURCES;

	for (i=0,cnt=0;i<gSdlPciData->PciDevCount; i++){
		dev=&gSdlPciData->PciDevices[i];
		if(dev->PciDevFlags.Bits.HostBridgeType) {
			buffer[cnt]=dev;
			cnt++;
		}
	}

	*HostBridgesBuffer=buffer;
	*HostBridgesCount=cnt;

	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindRbBySegBus()
//
// Description: Finds Root Bridge SDL record by PCI Segment and Bus Number
//
// Input: UINT32                Segment     PCI Segment Number
//
// Input: UINT8                 Bus         PCI Bus Number
//
// Output: AMI_SDL_PCI_DEV_INFO *Record     Pointer at AMI_SDL_PCI_DEV_INFO structure 
//                                          of a RootBridge record matching with search criteria.
//
// Output: UINTN                *Index      Pointer to Index that will be updated. 
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindRbBySegBus(IN UINT32 Segment, IN UINT8 Bus, OUT AMI_SDL_PCI_DEV_INFO **Record, OUT UINTN *Index){
    UINTN                   i;
    AMI_SDL_PCI_DEV_INFO    *dev;
    EFI_STATUS              Status;
//-----------------
    if(Record==NULL || Index==NULL) return EFI_INVALID_PARAMETER;

    Status=AmiSdlInitBoardInfo();
    if(EFI_ERROR(Status)) return Status;

    for(i=0; i<gSdlPciData->PciDevCount; i++){
        dev=&gSdlPciData->PciDevices[i];
        if(dev->PciDevFlags.Bits.RootBridgeType) {
            if((dev->PciSegment == Segment) && (dev->Bus == Bus )){
                *Record=dev;
                *Index=i;
                return EFI_SUCCESS;
            }
        }
    }
    return EFI_NOT_FOUND;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindRbByAslName()
//
// Description: Finds Root Bridge SDL record by PCI Segment and Bus Number
//
// Input: UINT8*                AslName     Root Bridge ASL Name (4 Char string)
//
// Output: AMI_SDL_PCI_DEV_INFO *Record     Pointer at AMI_SDL_PCI_DEV_INFO structure 
//                                          of a RootBridge record matching with search criteria.
//
// Output: UINTN                *Index      Pointer to Index that will be updated. 
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindRbByAslName(IN UINT8* AslName, OUT AMI_SDL_PCI_DEV_INFO **Record, OUT UINTN *Index){
    UINTN                   i;
    AMI_SDL_PCI_DEV_INFO    *dev;
    EFI_STATUS              Status;
//-----------------
    if(Record==NULL || Index==NULL) return EFI_INVALID_PARAMETER;

    Status=AmiSdlInitBoardInfo();
    if(EFI_ERROR(Status)) return Status;

    for(i=0; i<gSdlPciData->PciDevCount; i++){
        dev=&gSdlPciData->PciDevices[i];
        if(dev->PciDevFlags.Bits.RootBridgeType) {
            if(!MemCmp(&dev->AslName,AslName,sizeof(dev->AslName))){
                *Record=dev;
                *Index=i;
                return EFI_SUCCESS;
            }
        }
    }
    return EFI_NOT_FOUND;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindIndexRecord()
//
// Description: Find Index of given Record 
//
// Input: 	UINTN					Index		Record Index to find. 
//
// Output:	AMI_SDL_PCI_DEV_INFO	**Record 	Double Pointer at AMI_SDL_PCI_DEV_INFO  
//												updated with address of a structure 
//												to be found.
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindIndexRecord(UINTN	Index, OUT AMI_SDL_PCI_DEV_INFO **Record)
{
	EFI_STATUS				Status;
//-----------------------------------	
	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)) return Status;

	//Check Parameters first...
	if(Record==NULL || Index>=gSdlPciData->PciDevCount) return EFI_INVALID_PARAMETER;

	//get the record by Index and update Output parameter...
	*Record=&gSdlPciData->PciDevices[Index];
	
	//must be EFI_SUCCESS if we here.
	return Status;
	
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlFindRecordIndex()
//
// Description: Find Index of given Record 
//
// Input: AMI_SDL_PCI_DEV_INFO 	*Record 	Pointer at AMI_SDL_PCI_DEV_INFO structure 
//											which Index needed to be found.
//
// Output: UINTN				*Index		Pointer to Index that will be updated. 
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlFindRecordIndex(IN AMI_SDL_PCI_DEV_INFO *Record, OUT UINTN	*Index)
{
	UINTN					i;
	AMI_SDL_PCI_DEV_INFO	*dev;
	EFI_STATUS				Status;
//-----------------
	if(Record==NULL || Index==NULL) return EFI_INVALID_PARAMETER;

	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)) return Status;

	for(i=0; i<gSdlPciData->PciDevCount; i++){
		dev=&gSdlPciData->PciDevices[i];
		if(dev==Record) {
			*Index=i;
			return EFI_SUCCESS;
		}
	}
	return EFI_NOT_FOUND;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS AmiSdlPciGetChildsOfParentIndex(AMI_SDL_PCI_DEV_INFO ***ChildrenSdlDataBuffer, UINTN *ChildCount, UINTN ParentIndex)
{
	EFI_STATUS				Status;
	UINTN					i, cnt;
	AMI_SDL_PCI_DEV_INFO	*dev;
	AMI_SDL_PCI_DEV_INFO	**buffer;
//-----------------------

	if(ChildrenSdlDataBuffer==NULL || ChildCount==NULL || ParentIndex>=gSdlPciData->PciDevCount) return EFI_INVALID_PARAMETER;

	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)) return Status;

	for (i=0, cnt=0; i<gSdlPciData->PciDevCount; i++){
		dev=&gSdlPciData->PciDevices[i];
		//host not suppose to have Parent...
		if(dev->ParentIndex==ParentIndex && (!dev->PciDevFlags.Bits.HostBridgeType))
			cnt++;
	}

	if (cnt==0) return EFI_NOT_FOUND;

	buffer=Malloc(sizeof(AMI_SDL_PCI_DEV_INFO*)*cnt);
	if(buffer==NULL) return EFI_OUT_OF_RESOURCES;

	for (i=0,cnt=0; i<gSdlPciData->PciDevCount; i++){
		dev=&gSdlPciData->PciDevices[i];
		//host not suppose to have Parent...
		if(dev->ParentIndex==ParentIndex && (!dev->PciDevFlags.Bits.HostBridgeType)) {
			buffer[cnt]=dev;
			cnt++;
		}
	}

	*ChildrenSdlDataBuffer=buffer;
	*ChildCount=cnt;

	return Status;
}


EFI_STATUS AmiSdlPciCheckFixedBus(AMI_SDL_PCI_DEV_INFO ***BridgesBuffer, UINTN *Count){
	EFI_STATUS				Status;
	UINTN					i, cnt;
	AMI_SDL_PCI_DEV_INFO	*dev;
	AMI_SDL_PCI_DEV_INFO	**buffer;
//-----------------------

	if(BridgesBuffer==NULL || Count==NULL ) return EFI_INVALID_PARAMETER;

	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)) return Status;

	for (i=0, cnt=0; i<gSdlPciData->PciDevCount; i++){
		dev=&gSdlPciData->PciDevices[i];
		if(!dev->PciDevFlags.Bits.Virtual){
			if (dev->PciDevFlags.Bits.Pci2PciBridgeType || dev->PciDevFlags.Bits.Pci2CrdBridgeType){
				if(dev->PciDevFlags.Bits.FixedBus) cnt++;
			}
		}
	}

	if (cnt==0) return EFI_NOT_FOUND;

	buffer=Malloc(sizeof(AMI_SDL_PCI_DEV_INFO*)*cnt);
	if(buffer==NULL) return EFI_OUT_OF_RESOURCES;

	for (i=0,cnt=0; i<gSdlPciData->PciDevCount; i++){
		dev=&gSdlPciData->PciDevices[i];
		//host not suppose to have Parent...
		if(!dev->PciDevFlags.Bits.Virtual){
			if (dev->PciDevFlags.Bits.Pci2PciBridgeType || dev->PciDevFlags.Bits.Pci2CrdBridgeType){
				if(dev->PciDevFlags.Bits.FixedBus){
					buffer[cnt]=dev;
					cnt++;
				}
			}
		}
	}

	*BridgesBuffer=buffer;
	*Count=cnt;

	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AmiSdlCheckIfParent()
//
// Description: Find Index of given Record 
//
// Input: UINTN 	DeviceIndex Index in DB of Child to check
//		  UINTN		ParentIndex Index in DB of Parent.									
//
// Output:  	TRUE if Device Index appears in the brunch produces by Parent 
//				FALSE otherwise
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
BOOLEAN AmiSdlCheckIfParent(UINTN DeviceIndex, UINTN ParentIndex){
	EFI_STATUS				Status;
	UINTN					i;
	AMI_SDL_PCI_DEV_INFO	*dev;
//-------------------------------	
	Status=AmiSdlInitBoardInfo();
	if( EFI_ERROR(Status)||(DeviceIndex == ParentIndex) ) return FALSE;
	
	//Assuming DevIndex and parent index newer can be equal..
	i=DeviceIndex;		
	while(i){
		dev=&gSdlPciData->PciDevices[i];
		i=dev->ParentIndex;
		if(i==ParentIndex)return TRUE;
	}
	
	return FALSE;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
#ifndef CACHEABILITY_ATTRIBUTES
#define CACHEABILITY_ATTRIBUTES \
    (EFI_MEMORY_UC|EFI_MEMORY_WC|EFI_MEMORY_WT|EFI_MEMORY_WB|EFI_MEMORY_WP)
#endif

EFI_STATUS AmiSdlReserveCspResources(EFI_HANDLE ImgHandle){
	EFI_STATUS					Status;
	UINTN						i;
	AMI_SDL_CHIPSET_RESOURCE	*Res, *ResTable;
	DXE_SERVICES				*DxeSvc=NULL;
	UINT64				top=0, bot=-1, len=0;
	UINTN				uccnt=0, memcnt=0;
//	UINTN				ResCount=gSdlPciData->CspResCount;
	EFI_GCD_MEMORY_SPACE_DESCRIPTOR	mdsc;
	EFI_GCD_IO_SPACE_DESCRIPTOR 	idsc;
//---------------------------------
	//Get Dxe Services Table
	Status=LibGetDxeSvcTbl(&DxeSvc);
	if(EFI_ERROR(Status)){
		TRACE((TRACE_ALWAYS, "Dxe Services Table could not be found! Status = %r",Status));
		ASSERT_EFI_ERROR(Status);
		return Status;
	}

	//Get pointer to the AMI_SDL_CHIPSET_RESOURCE;
	ResTable=(AMI_SDL_CHIPSET_RESOURCE*)(&gSdlPciData->PciDevices[gSdlPciData->PciDevCount]);


	//Add this Space as MMIO to the GCD
	for(i=0; i<gSdlPciData->CspResCount; i++){
		Res=&ResTable[i];
		//if all non existet space, Memory and IO has been converted to MMIO
		//we don't want to hang on ERROR Status here
		if(Res->Attributes==-1){
			Status=DxeSvc->AddIoSpace(Res->ResType,Res->ResBase,Res->ResLength);
			TRACE((TRACE_ALWAYS, "GCD: AddI/O"));
		} else {
			//Some attempt to optimize region caching
			//we will try to group some regions based on their cache requirements.
			memcnt++;
			if(Res->Attributes & EFI_MEMORY_UC) uccnt++;
			if((Res->ResBase+Res->ResLength) > top ) top=Res->ResBase+Res->ResLength;
			if(Res->ResBase < bot) bot=Res->ResBase;
			Status=DxeSvc->AddMemorySpace(Res->ResType,Res->ResBase,Res->ResLength,GCD_COMMON_MMIO_CAPS);
			TRACE((TRACE_ALWAYS, "GCD: AddMem"));
		}
		TRACE((TRACE_ALWAYS, "Space   B=%lX, L=%X, i=%d, S=%r\n",Res->ResBase,Res->ResLength,i,Status));

		//Allocate the Space
		if(Res->Attributes==-1){
			//Check Gcd IO we are trying to Allocate
			Status=DxeSvc->GetIoSpaceDescriptor(Res->ResBase, &idsc);
			if(idsc.ImageHandle != NULL) continue;
			//If Space is not allocated but marked as different type of Space - use the one we've got
			if(idsc.GcdIoType!=(EFI_GCD_IO_TYPE) Res->ResType) Res->ResType=idsc.GcdIoType;
			Status=DxeSvc->AllocateIoSpace(EfiGcdAllocateAddress,
				   Res->ResType,0, Res->ResLength, &Res->ResBase, ImgHandle, NULL);
			TRACE((TRACE_ALWAYS, "GCD: AllocI/O"));
		} else {
			//Check Gcd Memory we are trying to Allocate
			Status=DxeSvc->GetMemorySpaceDescriptor(Res->ResBase, &mdsc);
			//the resource has been allocated earlier by MRC or trough HOB
			if(mdsc.ImageHandle != NULL) continue;
			//If Space is not allocated but marked as different type of Space - use the one we got
			else if(mdsc.GcdMemoryType!=Res->ResType /*&&
					mdsc.BaseAddress<=Res->ResBase &&
					(mdsc.BaseAddress+mdsc.Length) >= (Res->ResBase+Res->ResLength)*/
				) Res->ResType=mdsc.GcdMemoryType;

			Status=DxeSvc->AllocateMemorySpace(EfiGcdAllocateAddress,
				   Res->ResType, 0, Res->ResLength, &Res->ResBase, ImgHandle, NULL);
			TRACE((TRACE_ALWAYS, "GCD: AllocMem"));
		}
		TRACE((TRACE_ALWAYS, "Space B=%lX, L=%X, i=%d, S=%r\n",Res->ResBase,Res->ResLength,i,Status));
		if(EFI_ERROR(Status)){
			ASSERT_EFI_ERROR(Status);
			return Status;
		}
	} //for loop for allocation

	//this is the case when entire region suppose to be uncached
	if(memcnt && (memcnt==uccnt)){
        EFI_GCD_MEMORY_SPACE_DESCRIPTOR md;
        UINT64                          newb, newt, newl;
    //----------------------
		//Adjust Caching Attribute Address to fit PAGES
		len=top-bot;
		if(bot & EFI_PAGE_MASK){
			bot&=(~EFI_PAGE_MASK);
			len = (len&(~EFI_PAGE_MASK))+EFI_PAGE_SIZE;
		}
		if(len & EFI_PAGE_MASK) len = (len&(~EFI_PAGE_MASK))+EFI_PAGE_SIZE;

        //Now we must apply Caching attributes but must be wery careful
        //not to clean RT attributes if it was set by earlier call
		TRACE((TRACE_ALWAYS, "GCD: SpaceAttr (UC ALL) B=%lX; L=%lX; \n",bot,top-bot));

        newb=bot;

        for(;;){
            Status=DxeSvc->GetMemorySpaceDescriptor(newb,&md);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) break;

            newt=md.BaseAddress+md.Length;
            if(newt>=top) newt=top;
            newl=newt-newb;

            if(md.Attributes & EFI_MEMORY_UC){
            	TRACE((TRACE_ALWAYS, "GCD:(UC ALL) skipping"));
            } else {
                md.Attributes|=EFI_MEMORY_UC;
        		Status=DxeSvc->SetMemorySpaceAttributes(newb,newl, md.Attributes);
        		TRACE((TRACE_ALWAYS, "GCD:(UC ALL) setting "));
            }
            TRACE((TRACE_ALWAYS, " B=%lX, L=%lX, A=%lX; S=%r\n",newb,newl,md.Attributes, Status));

            if(newt>=top) break;
            newb=newt;
        }

	}
    //set cacheability attributes
    for(i=0; i<gSdlPciData->CspResCount; i++){
        //Skip I/O
		if(ResTable[i].Attributes==-1) continue;
		//Set Attributes For this Region.
		Status=DxeSvc->SetMemorySpaceAttributes(ResTable[i].ResBase,ResTable[i].ResLength,ResTable[i].Attributes);
		TRACE((TRACE_ALWAYS, "GCD: SpaceAttr A=%lX B=%lX, L=%X, i=%X, S=%r\n",
            ResTable[i].Attributes, ResTable[i].ResBase, ResTable[i].ResLength,i,Status));
        if (EFI_ERROR(Status)){
            //Attempt to set attributes failed;
            //Let's set non-cacheability attributes
            UINT64 attr=ResTable[i].Attributes & ~CACHEABILITY_ATTRIBUTES;
            //If all descriptors are uncacheable add US attribute
            if(memcnt==uccnt) attr|=EFI_MEMORY_UC;
            Status=DxeSvc->SetMemorySpaceAttributes(ResTable[i].ResBase,ResTable[i].ResLength,  attr);
        }
	}

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AmiSdlReadFfsSdlData()
//
// Description: This function locate resource binary by GUID.
//
// Input:
//  pResourceGuid        - pointer to recource GUID
//  pResourceSectionGuid - pointer to recource section GUID
//  Address              - pointer to returning address of the resource
//
// Output:  EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiSdlReadFfsSdlData(
	OUT UINT8 **SdlAddress, UINTN *SdlDataSize,
	IN EFI_GUID *FileGuid, IN EFI_GUID *SectionGuid OPTIONAL
)
{
    EFI_STATUS Status;
    EFI_HANDLE *HandleBuffer = NULL;
    UINTN  NumberOfHandles;
    UINT32 Authentication;
    UINTN  i,j;
    VOID  *AmiData = NULL;
    UINTN  DataSize;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVolumeProtocol = NULL;
	EFI_SECTION_TYPE SectionType;
//-----------------------------------------------------

    // Locate the Firmware volume protocol
    Status = pBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiFirmwareVolume2ProtocolGuid,
        NULL,
        &NumberOfHandles,
        &HandleBuffer
    );
    if (EFI_ERROR(Status))
        return EFI_NOT_FOUND;
    
	SectionType = (SectionGuid==NULL)
				 ? EFI_SECTION_RAW : EFI_SECTION_FREEFORM_SUBTYPE_GUID;

    // Find and read raw data SDL INFO
    for (i = 0; i < NumberOfHandles; i++) {
        Status = pBS->HandleProtocol(HandleBuffer[i], &gEfiFirmwareVolume2ProtocolGuid,(VOID**) &FwVolumeProtocol);
        if (EFI_ERROR(Status)) continue;
        
        for(j=0; ;j++){        	
            AmiData=NULL;
            //Read Section From FFS file
            Status = FwVolumeProtocol->ReadSection(
                    FwVolumeProtocol,
                    FileGuid,
                    SectionType,
                    j,
                    &AmiData,
                    &DataSize,
                    &Authentication
            );
            if (EFI_ERROR(Status) && SectionGuid==NULL){
                Status = FwVolumeProtocol->ReadSection(
                        FwVolumeProtocol,
                        FileGuid,
                        EFI_SECTION_PE32,
                        j,
                        &AmiData,
                        &DataSize,
                        &Authentication
                );
            }
            
        	//No file found advance to the next FV... 
        	if (EFI_ERROR(Status))  break;

			if (SectionType==EFI_SECTION_FREEFORM_SUBTYPE_GUID){
            	EFI_GUID	*secguid;
           	//----------------------------
            	//Compare Section GUID, to find correct one..
            	secguid=(EFI_GUID*)AmiData;
        			
       			if ( guidcmp(secguid,SectionGuid)!=0 ){
       				//Free section read, it was not the one we need...
       				if(AmiData!=NULL) pBS->FreePool(AmiData);
       				continue; 
       			}
        		
       			//Update 
				DataSize -= sizeof(EFI_GUID);
				// ReadSection returns pointer to a section GUID, which caller of this function does not want to see.
				// We could've just returned (EFI_GUID*)AmiData + 1 to the caller,
				// but this pointed can't be used to free the pool (can't be passed to FreePool).
				// Copy section data into a new buffer
				AmiData = Malloc(DataSize);
				if (AmiData == NULL) Status = EFI_OUT_OF_RESOURCES;
				else MemCpy(AmiData,secguid+1,DataSize);
				pBS->FreePool(secguid);
			}
				
        	if (HandleBuffer!=NULL) pBS->FreePool(HandleBuffer);
        	if (!EFI_ERROR(Status)){
                *SdlAddress = (UINT8*)AmiData;
                *SdlDataSize=DataSize;
        	}
        	return Status;
        }//for [j]
    }//for [i]

    if(HandleBuffer!=NULL)pBS->FreePool(HandleBuffer);
    return EFI_NOT_FOUND;
}



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

