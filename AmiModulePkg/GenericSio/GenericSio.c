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


//**********************************************************************
/** @file 
    Implementation of the AMI SIO Driver.

**/
//**********************************************************************

#include <Token.h>

//==================================================================================

#include <Pci.h>
#include <Protocol/AmiSio.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/PciIo.h>
#include <Protocol/S3SaveState.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/AmiSioCompatibility.h>
#include <Protocol/SmmCommunication.h>
#include <Dxe.h>
#include <AmiDxeLib.h>
#include <AmiGenericSio.h>
#include <AcpiRes.h>
#include <Protocol/AmiSioSmmHandoff.h>
#include <Library/AmiSioDxeLib.h>
#include <Library/AmiSdlLib.h>
#include <Library/BaseMemoryLib.h>


#if ACPI_SUPPORT
//include this if ACPI is Supported
#include <Acpi20.h>
#include <Protocol/AcpiSupport.h>

//
//Some variables we need if ACPI mode is SUPPORTED 
//
ACPI_HDR 	*gPDsdt=NULL;
EFI_EVENT 	mAcpiEvent=NULL;
VOID		*mAcpiReg;
EFI_GUID	gAcpiSupportGuid=EFI_ACPI_SUPPORT_GUID;

#endif

//#define _AND_ &
#define SIO_MAX_REG			0xFF

//0 = lpAptioV   1 = lpAptio4xAll other values are reserved
const UINT8	CompatibleSioDeviceOrder =
#ifdef SIO_PORT_DEVICE_ORDER 
	SIO_PORT_DEVICE_ORDER 	
#else 
	0	
#endif
	;
///==================================================================================
///Global Variables
///==================================================================================

BOOLEAN						mVarWrite=FALSE;
extern const BOOLEAN 		HideComPort;

///global SPIO structure
GSIO2 	 					*gSpio=NULL;
UINTN						gSpioCount=0;
AMI_BOARD_INIT_PROTOCOL		*gSioInitProtocol=NULL;
EFI_EVENT					mIsaEvent=NULL;
VOID						*mIsaReg;


EFI_SMM_COMMUNICATE_HEADER *gSmmCommunicationBuffer; 
//--------------------------------------------------------
EFI_GUID gSioDevStatusVarGuid 			= SIO_DEV_STATUS_VAR_GUID;

///DxeServices table Pointer to get access to the GCD Services
DXE_SERVICES		*gDxeSvcTbl=NULL;
///Driver Name
UINT16		*gDriverName=L"AMI Generic LPC Super I/O Driver";
///ISA IRQ Mask tells which interrupts we can use for SIO devices
UINT16		gAvailableIsaIrq=0;//=ISA_IRQ_MASK;
///Global variable to report which IRQs are being used in the SIO
UINT16      gUsedSioIrq=0;
///Global variable to report which IRQs are requested/reserved in the SIO
UINT16      gReservedIrq=0;
///ISA DMA Chanal Mask tells which DMA Chanals we can use for SIO devices
UINT8		gAvailableDmaChnl=0;//=ISA_DMA_MASK;//0xEF;

EFI_EVENT	gEvtReadyToBoot = NULL;

///==================================================================================
///Some Function Prototypes to place Entry point at the beginning of this file 
///==================================================================================
#ifdef EFI_DEBUG
EFI_STATUS EFIAPI DevicePathToStr(EFI_DEVICE_PATH_PROTOCOL *Path,CHAR8	**Str);
#endif
EFI_STATUS EFIAPI SioEnumerateAll(GSIO2 *Spio);

SIO_DEV2* EFIAPI NewDev(AMI_SDL_LOGICAL_DEV_INFO *SioDevInfo, GSIO2 *Owner, UINTN SdlIdx,  BOOLEAN CompatibleMode);
EFI_STATUS EFIAPI SioSetupData(SIO_DEV2 *Dev, BOOLEAN Get);

///==================================================================================
/// Function Prototypes for Driver Binding Protocol Interface
///==================================================================================
EFI_STATUS EFIAPI AmiSioSupported(IN EFI_DRIVER_BINDING_PROTOCOL	*This,
						   IN EFI_HANDLE                    Controller,
						   IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath);

EFI_STATUS EFIAPI AmiSioStart(IN EFI_DRIVER_BINDING_PROTOCOL		*This,
					   IN EFI_HANDLE						Controller,
					   IN EFI_DEVICE_PATH_PROTOCOL			*RemainingDevicePath );

EFI_STATUS EFIAPI AmiSioStop(IN EFI_DRIVER_BINDING_PROTOCOL			*This,
					   IN EFI_HANDLE						Controller,
					   IN UINTN								NumberOfChildren,
					   IN EFI_HANDLE						*ChildHandleBuffer);

EFI_STATUS EFIAPI AmiSioComponentNameGetControllerName(IN EFI_COMPONENT_NAME2_PROTOCOL *This,
						IN  EFI_HANDLE                   ControllerHandle,
						IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
						IN  CHAR8                        *Language,
						OUT CHAR16                       **ControllerName );

EFI_STATUS EFIAPI AmiSioComponentNameGetDriverName(IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
						IN  CHAR8                        *Language,
						OUT CHAR16                       **DriverName);


EFI_STATUS EFIAPI AmiSioRegister(IN AMI_SIO_PROTOCOL	*This,
						IN BOOLEAN				Write,
						IN BOOLEAN				ExitCfgMode,
						IN UINT8            	Register,
    					IN OUT UINT8        	*Value);


EFI_STATUS EFIAPI AmiSioCRS(IN AMI_SIO_PROTOCOL	*This,
				    IN BOOLEAN 				Set,
					IN OUT T_ITEM_LIST		**Resources);

EFI_STATUS EFIAPI AmiSioPRS(IN AMI_SIO_PROTOCOL	*This,
				    IN BOOLEAN 				Set,
					IN OUT T_ITEM_LIST		**Resources);

EFI_STATUS EFIAPI ReserveIrq(SIO_DEV2 *Dev, UINTN Index);

VOID EFIAPI CallbackReadyToBoot(
	IN EFI_EVENT	Event,
	IN VOID			*Context
);

///
///SIO Compatibility Porting Protocol
///
AMI_SIO_PORT_COMPATIBILITY_PROTOCOL	*gSioCompProtocol=NULL;



EFI_STATUS EFIAPI CreateSioDevStatusVar();
EFI_STATUS EFIAPI DisableDevInSioDevStatusVar(SIO_DEV2 *Dev);
VOID EFIAPI SaveSioRegs(GSIO2 *Sio, SIO_DEV2 *Dev, UINT8* InclRegList, UINTN InclRegCount, UINT8 *Buffer);
VOID EFIAPI DevSelect(SIO_DEV2 *Dev);

#if ACPI_SUPPORT
VOID EFIAPI GetDsdt(IN EFI_EVENT Event, IN VOID *Context);
#endif
VOID EFIAPI UpdateIsaMask(IN EFI_EVENT Event, IN VOID *Context);

///
/// Driver binding protocol instance for AmiSio Driver
///
EFI_DRIVER_BINDING_PROTOCOL gAmiSioDriverBinding = {
	AmiSioSupported,	//Supported
	AmiSioStart,		//PciBusDrvStart,
	AmiSioStop,			//PciBusDrvStop,
	0x10,				//Version,
	NULL,				//Image Handle,
	NULL				//DriverBindingHandle,
};

AMI_SIO_SMM_HANDOFF_PROTOCOL gAmiSioSmmHandoff ={0,NULL};

//==================================================================================
//Component Name Protocol Instance
#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID //old Core
#ifndef LANGUAGE_CODE_ENGLISH
#define LANGUAGE_CODE_ENGLISH "eng"
#endif
BOOLEAN LanguageCodesEqual(
    CONST CHAR8* LangCode1, CONST CHAR8* LangCode2
){
    return    LangCode1[0]==LangCode2[0] 
           && LangCode1[1]==LangCode2[1]
           && LangCode1[2]==LangCode2[2];
}
//static EFI_GUID gEfiComponentName2ProtocolGuid = EFI_COMPONENT_NAME_PROTOCOL_GUID;
#endif
EFI_COMPONENT_NAME2_PROTOCOL gAmiSioComponentName = {
  AmiSioComponentNameGetDriverName,
  AmiSioComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};

//==================================================================================
//Ami SIO Protocol instance
AMI_SIO_PROTOCOL gAmiSioProtocol = {
    AmiSioRegister,
	AmiSioCRS,
	AmiSioPRS,
};

EFI_STATUS EFIAPI EfiSioRegisterAccess(
  IN   CONST  EFI_SIO_PROTOCOL  *This,
  IN          BOOLEAN           Write,
  IN          BOOLEAN           ExitCfgMode,
  IN          UINT8             Register,
  IN OUT      UINT8             *Value);


EFI_STATUS EFIAPI EfiSioGetResourcces( 
  IN  CONST EFI_SIO_PROTOCOL            *This,
  OUT       ACPI_RESOURCE_HEADER_PTR    *ResourceList
);

EFI_STATUS EFIAPI EfiSioSetResources(
  IN CONST  EFI_SIO_PROTOCOL        *This,
  IN        ACPI_RESOURCE_HEADER_PTR ResourceList
);

EFI_STATUS EFIAPI EfiSioPossibleResources(
  IN  CONST EFI_SIO_PROTOCOL         *This,
  OUT       ACPI_RESOURCE_HEADER_PTR *ResourceCollection
);

EFI_STATUS EFIAPI EfiSioModify(
  IN CONST EFI_SIO_PROTOCOL         *This,
  IN CONST EFI_SIO_REGISTER_MODIFY  *Command,
  IN       UINTN                    NumberOfCommands
);


EFI_SIO_PROTOCOL gEfiSioProtocol = {
    EfiSioRegisterAccess,
    EfiSioGetResourcces,
    EfiSioSetResources,
    EfiSioPossibleResources,
    EfiSioModify,
};

/**
    This function will update gAvailableIsaIrq &gAvailableDmaChnl value

    @param Event Event handle
    @param Context Pointer to the optional Data Buffer
	 
    @retval VOID

**/ 
VOID EFIAPI UpdateIsaMask(IN EFI_EVENT Event, IN VOID *Context){
	EFI_STATUS		Status;
	UINTN			i;
//-------------------------

	SIO_TRACE((TRACE_SIO,"GSIO: Variable Write Available!\n"));	
	Status = AmiIsaIrqMask(&gAvailableIsaIrq, TRUE);
	if(Status==EFI_NOT_FOUND){
		gAvailableIsaIrq=ISA_IRQ_MASK;
		SIO_TRACE((TRACE_SIO,"GSIO: Get ISA_IRQ_MASK Status=%r Updating with DEFAULT %X\n",Status, gAvailableIsaIrq));
		Status = AmiIsaIrqMask(&gAvailableIsaIrq, FALSE);
	}
	if(EFI_ERROR(Status)){
		SIO_TRACE((TRACE_SIO,"GSIO: ERROR Updating ISA_IRQ_MASK Status=%r\n",Status));
		gAvailableIsaIrq=ISA_IRQ_MASK;
	}

	Status = AmiIsaDmaMask(&gAvailableDmaChnl, TRUE);
	if(Status==EFI_NOT_FOUND){
		gAvailableDmaChnl=ISA_DMA_MASK;
		SIO_TRACE((TRACE_SIO,"GSIO: Get ISA_DMA_MASK Status=%r Updating with DEFAULT %X\n",Status, gAvailableDmaChnl));
		Status=AmiIsaDmaMask(&gAvailableDmaChnl, FALSE);
	}

	if(EFI_ERROR(Status)){
		SIO_TRACE((TRACE_SIO,"GSIO: ERROR Updating ISA_DMA_MASK Status=%r\n",Status));
		gAvailableDmaChnl=ISA_DMA_MASK;
	}

	mVarWrite=TRUE;

	//ACPI Support was Installed Before VariableWrite Protocol
	//So we have collected SIOs _PRS info but did not reserve IRQs yet!
    SIO_TRACE((TRACE_SIO,"=================================================\n"));
#if ACPI_SUPPORT
	if(gPDsdt!=NULL){
		for(i=0;i<gSpioCount;i++){
			SIO_DEV2		*dev;
			UINTN			j;
		//-------------------------	
    		SIO_TRACE((TRACE_SIO,"GSIO: (ACPI Mode) Enumerate SIO Chip # %d >>>>> \n",i));
			for(j=0; j<gSpio[i].LdCount; j++){
				dev=gSpio[i].DeviceList[j];
		        if(dev->DeviceInfo->HasSetup) {
					Status=SioSetupData(dev,TRUE);
					SIO_TRACE((TRACE_SIO,"GSIO: Get SIO Setup Data. Status=%r\n",Status));
					ASSERT_EFI_ERROR(Status);
				}
				//Taking care of issue 1 in EIP72716 
		        if(!dev->DeviceInfo->Implemented) continue;
				//For Irq reservation it might be a disabled by setup device that shares it's resources
				//It appears to be a resource owner in such case it will have field Resource consumer filled.
				if(!dev->NvData.DevEnable) if(dev->ResConsumer==NULL) continue;
				Status=ReserveIrq(dev, j);				
				SIO_TRACE((TRACE_SIO,"GSIO: Reserve IRQ for SIO[%d].Device[%d] - Status=%r\n",i,j,Status));
			}
 			SIO_TRACE((TRACE_SIO,"GSIO: (ACPI Mode) EnumerateAll for SIO Chip # %d -> Status=%r\n\n",i, Status));
		}
	}
#else
	//Set the Possible Resources for each implemented device
	for(i=0;i<gSpioCount;i++){

		SIO_TRACE((TRACE_SIO,"GSIO: (NO ACPI Mode) Enumerate SIO Chip # %d >>>>> \n",i));
		Status=SioEnumerateAll(&gSpio[i]);	
		SIO_TRACE((TRACE_SIO,"GSIO: (NO ACPI Mode) EnumerateAll for SIO Chip # %d -> Status=%r\n\n",i, Status));
	}
	//install driver binding protocol here ....
	Status = pBS->InstallMultipleProtocolInterfaces(
					&gAmiSioDriverBinding.DriverBindingHandle,
					&gDriverBindingProtocolGuid,&gAmiSioDriverBinding,
					&gEfiComponentName2ProtocolGuid, &gAmiSioComponentName,
					NULL,NULL);

	ASSERT_EFI_ERROR(Status);				
#endif
	if(Event) {
		pBS->CloseEvent(Event);
		Event=NULL;	
	}
}

/**
    This is a routine to launch Enter Configuration Mode Routine

    @param Sio Pointer to the private SIO data structure
    @param Function Function Index in the init function list.
	@param S3Save indicatr to record coresponded this action Boot Script
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends

**/ 
EFI_STATUS EFIAPI LaunchCfgModeRoutine(GSIO2 *Sio, UINTN Function, BOOLEAN S3Save)
{
	AMI_BOARD_INIT_PARAMETER_BLOCK 	Parameters;
	EFI_STATUS						Status;
	VOID							*LpcPciIo=Sio->IsaBrgPciIo;
	AMI_BOARD_INIT_PROTOCOL			*InitProtocol=Sio->SioInitProtocol;

//------------------------
	if(S3Save) {
		BOOT_SCRIPT_S3_DISPATCH_MACRO(Sio->SaveState, (VOID*)(InitProtocol->Functions[Function]));
		return EFI_SUCCESS;
	}

	if(InitProtocol->FunctionCount < Function) {
		SIO_TRACE((TRACE_SIO,"GSIO: InitProtocol->FunctionCount(%d) < InitFunction (%d)!\n", InitProtocol->FunctionCount, Function));
		return EFI_INVALID_PARAMETER;
	}

	Parameters.Signature=AMI_SIO_PARAM_SIG;
	Parameters.InitStep=isSioNone;
	Parameters.Param1=NULL;
	Parameters.Param2=LpcPciIo;
	Parameters.Param3=NULL;
	Parameters.Param4=NULL;

	SIO_TRACE((TRACE_SIO,"GSIO: Calling InitProtocol->Function[%d](Enter/ExitCfgMode, LpcPciIo=%X)...",
			Function, LpcPciIo));
	Status=InitProtocol->Functions[Function](InitProtocol,&Function, &Parameters);
	SIO_TRACE((TRACE_SIO,"%r\n",Status));

	ASSERT_EFI_ERROR(Status);

	return Status;
}


/**
    This is a routine will convert SDL data abour registers needed to 
	save in Boot Script	to internal array used in SIO Private Data.

    @param RegCount number of registers included in following parameter
    @param SaveBits AMI SDL bitmask (256 bits represents 256 registers)
    @param SaveRegs Allocated array with register offsets - returned as Output

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends

**/
EFI_STATUS EFIAPI CountCreateIncludeReg(UINTN* RegCount,  UINT8* SaveBits, UINT8 **SaveRegs){
	UINTN	i,j, cnt;
	UINT8	tmp,b,offs, *buff=NULL;
//---------------------

	//Check parameters first;
	if(RegCount==NULL || (VOID*)SaveRegs==NULL || SaveBits==NULL) return EFI_INVALID_PARAMETER;

	//1.Count how much space we need to accommodate all script items...
	for (i=0, cnt=0; i<32; i++){
		tmp=SaveBits[i];
		for(j=0; j<8; j++){
			b=(1<<j);
			if(b&tmp) cnt++;
		}
	}

	if(cnt==0) return EFI_SUCCESS;

	buff=MallocZ(sizeof(UINT8)*cnt);
	ASSERT(buff);
	if(buff==NULL) return EFI_OUT_OF_RESOURCES;

    SIO_TRACE((TRACE_SIO,"SioInit: S3 Boot script will Monitor SIO registers at offsets:\n "));
	for (i=0, cnt=0, offs=0; i<32; i++){
		tmp=SaveBits[i];
		for(j=0; j<8; j++){
			b=(1<<j);
			if(b&tmp){
				buff[cnt]=(UINT8)offs;
                SIO_TRACE((TRACE_SIO," 0x%X;", offs));
				cnt++;
			}
			offs++;
		}
	}
    SIO_TRACE((TRACE_SIO,"\n"));

	*RegCount=cnt;
	*SaveRegs=buff;

	return EFI_SUCCESS;
}

/**
    This function will initialize SIO internal data structures

        
    @param ImageHandle Image of this driver. 
    @param SystemTable UEFI System Table Pointer.

    @retval EFI_SUCCESS if initialization is DONE EFI_ERROR otherwise.
**/ 
AMI_SDL_SIO_CHIP_INFO* EFIAPI ProcessCompSioData(SPIO_LIST_ITEM* CmpChipEntry){
	AMI_SDL_SIO_CHIP_INFO 	*ret;
//------------------------------------
	//get mem
	ret=MallocZ(sizeof(AMI_SDL_SIO_CHIP_INFO)+(sizeof(SPIO_DEV_LST)*CmpChipEntry->DevCount));
	if(ret==NULL){
		ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
		return ret;
	}

	//Update fields from Aptio 4.x compatible data structure;
	//only supporting STD SIO devices in compatibility mode...
	ret->StdSioDevice=TRUE; 
	ret->IsaBusNo = CmpChipEntry->IsaBusNo;
	ret->IsaDevNo = CmpChipEntry->IsaDevNo;
	ret->IsaFuncNo = CmpChipEntry->IsaFuncNo;
	//Main controll registers...
	ret->SioIndex = CmpChipEntry->SioIndex;
	ret->SioData = CmpChipEntry->SioData;
	ret->DevSelReg = CmpChipEntry->DevSel;
	ret->ActivateReg = CmpChipEntry->Activate;
	ret->ActivateVal = CmpChipEntry->ActivVal;
	ret->DeactivateVal = CmpChipEntry->DeactVal;
	//Main Resource Registers...
	ret->Base1HiReg = CmpChipEntry->Base1Hi;
	ret->Base1LoReg = CmpChipEntry->Base1Lo;
	ret->Base2HiReg = CmpChipEntry->Base2Hi;
	ret->Base2LoReg = CmpChipEntry->Base2Lo;
	ret->Irq1Reg = CmpChipEntry->Irq1;
	ret->Irq2Reg = CmpChipEntry->Irq2;
	ret->Dma1Reg = CmpChipEntry->Dma1;
	ret->Dma2Reg = CmpChipEntry->Dma2;

	//Update Device Count...
	ret->LogicalDevCount = (UINT32)CmpChipEntry->DevCount;
	//Copy Logical Device Array.
	MemCpy(&ret->LogicalDev[0],CmpChipEntry->SioDevList, sizeof(SPIO_DEV_LST)*CmpChipEntry->DevCount);
	
	return ret;
}

VOID EFIAPI UpdateSioCfgScript(UINT8 *SdlBytreArray, SPIO_SCRIPT_LST *CompatibleScritpData){
	UINTN	i, bc;
	UINT8	**ci, b,*a;
//-------------------	
	SdlBytreArray[0]=(UINT8)CompatibleScritpData->OpType;
	SdlBytreArray[1]=(UINT8)CompatibleScritpData->InstrCount;
	
	//Now copy instructions at the byte position 
	ci=(UINT8**)CompatibleScritpData->Instruction;
	a=*ci;
	bc=CompatibleScritpData->InstrCount*3;
	
	for(i=0; i<bc; i++){
		b=a[i];
		SdlBytreArray[i+2]=b;
	}
}

/**
    This function will initialize SIO internal data structures

        
    @param ImageHandle Image of this driver. 
    @param SystemTable UEFI System Table Pointer.

    @retval EFI_SUCCESS if initialization is DONE EFI_ERROR otherwise.
**/ 
EFI_STATUS EFIAPI InitSioDataStructure(
IN EFI_HANDLE		ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable )
{
	EFI_STATUS					Status;
	AMI_SDL_SIO_CHIP_INFO		*SioChipSdlInfo=NULL;
	AMI_SDL_LOGICAL_DEV_INFO	*LdSdlInfo=NULL;
	GSIO2						*gspio=NULL;
	SIO_DEV2					*ro=NULL, *dev;//ro=resource owner
	UINTN						i, j, k, ccnt;
	BOOLEAN 					m4=FALSE, mV=FALSE, cm=FALSE; //Compatibility Mode
	UINT8						*p8;
//-----------------------------------------

	SIO_TRACE((TRACE_SIO,"GSIO: Starting SIO Initialization!\n"));
	Status = AmiSdlInitBoardInfo();
	SIO_TRACE((TRACE_SIO,"GSIO: Locate AmiBoardInfo2Protocol: Status=%r\n", Status));
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status))return Status;

	//Get Aptio 4.x Compatinility Protocol to call initialization functions of SIO LDs
	Status=pBS->LocateProtocol( &gAmiSioPortCompatibilityProtocolGuid,	//*Protocol,
								NULL,							//*Registration OPTIONAL,
								&gSioCompProtocol);				//*Interface

	//It must be here otherwise it will NO INIT functions for all Pci Devices.
	SIO_TRACE((TRACE_SIO,"GSIO: Locate AmiSioCompatibilityProtocol: Status=%r.\n", Status));

	//It could be the gSdlSioData == NULL in case if no Aptio V style SIO Devices Present...
	if(gSdlSioData!=NULL && gSdlSioData->SioCount){
		gSpioCount+=gSdlSioData->SioCount;
		SIO_TRACE((TRACE_SIO,"GSIO: Found %d SIO Chip(s)\n", gSpioCount));
		mV=TRUE;
	} else SIO_TRACE((TRACE_SIO,"GSIO: There are NO AptioV SIO Chip(s)in project!\n", gSpioCount));
	
	if(gSioCompProtocol!=NULL && gSioCompProtocol->SioPortSioCount){
		gSpioCount+=gSioCompProtocol->SioPortSioCount;
		SIO_TRACE((TRACE_SIO,"GSIO: Found %d Aptio4x SIO Chip(s) in Compatibility Data. Processing...\n", gSioCompProtocol->SioPortSioCount));
		m4=TRUE;
	} else SIO_TRACE((TRACE_SIO,"GSIO: There are NO Aptio4x Compatibility Data in project!\n", gSpioCount));
	
	
	if(gSpioCount==0) return EFI_UNSUPPORTED;
	
	//Get AMI BoardINit Protocol to call initialization functions of SIO LD
	Status=pBS->LocateProtocol( &gAmiBoardSioInitProtocolGuid,	//*Protocol,
								NULL,							//*Registration OPTIONAL,
								&gSioInitProtocol);				//*Interface

	//It must be here otherwise it will NO INIT functions for this SIO LDs.
	SIO_TRACE((TRACE_SIO,"GSIO: Locate AmiBoardSioInitProtocol. Status=%r.\n", Status));

	//-----------------------------------------------------------------------------
	//Start Initialization.....
	//-----------------------------------------------------------------------------
	//Get memory for SIO Data structure(s)
	gSpio=MallocZ(sizeof(GSIO2)*gSpioCount);
	ASSERT(gSpio);
	if(gSpio==NULL) return EFI_OUT_OF_RESOURCES;
	
	
	
	//Get Pointer at AMI_SIO_DEV_INFO it should follow (gSioSdlData+1)
	// 0 = lpAptioV   1 = lpAptio4x 
	if((m4 && !mV) || (m4 && (CompatibleSioDeviceOrder > 0)) ) cm=TRUE;
	else cm=FALSE;
	
	for(k=0;k<2; k++){
		
		if(cm){
			SioChipSdlInfo=ProcessCompSioData(gSioCompProtocol->SioPortSioList[0]);
			ccnt=gSioCompProtocol->SioPortSioCount;
		} else {
			SioChipSdlInfo=&gSdlSioData->SioDev[0];
			ccnt=gSdlSioData->SioCount;
		}
			
		for(i=0; i<ccnt; i++){
			gspio=&gSpio[i];
			gspio->SioInfoIdx=i;
			gspio->CompatibleData=cm;
			gspio->SpioSdlInfo=SioChipSdlInfo;
			gspio->SioInitProtocol=gSioInitProtocol;
	//		gspio->SioCompProtocol=gSioCompProtocol;
			//Init Fields of GSIO2 data structure from SDL data...
			//1.Update S3Save Global registers...
			gspio->LdCount=gspio->SpioSdlInfo->LogicalDevCount;
			SIO_TRACE((TRACE_SIO,"GSIO: SIO[%d] has [%d] Logical Devices.\n", i,gspio->LdCount));
	
			
			if(cm){
				gspio->GlobalInclRegCount=gSioCompProtocol->SioPortSioList[i]->GlobalInclRegCount;
				gspio->GlobalIncludeReg=gSioCompProtocol->SioPortSioList[i]->GlobalIncludeReg;
				gspio->LocalInclRegCount=gSioCompProtocol->SioPortSioList[i]->LocalInclRegCount;
				gspio->LocalIncludeReg=gSioCompProtocol->SioPortSioList[i]->LocalIncludeReg;
	
				//Update Enter exit Configuration Mode Script data...
				UpdateSioCfgScript(&SioChipSdlInfo->EnterCfgMode[0], gSioCompProtocol->SioPortSioList[i]->EnterCfgMode);
				UpdateSioCfgScript(&SioChipSdlInfo->ExitCfgMode[0], gSioCompProtocol->SioPortSioList[i]->ExitCfgMode);
	
			} else {
			    //Create Global Registers need to add to boot script.
				Status=CountCreateIncludeReg(&gspio->GlobalInclRegCount,
										 &gspio->SpioSdlInfo->GlobalRegMask[0],
										 &gspio->GlobalIncludeReg);
				ASSERT_EFI_ERROR(Status);
				if (EFI_ERROR(Status)) return Status;
	
		        //Local ones...
				Status=CountCreateIncludeReg(&gspio->LocalInclRegCount,
						&gspio->SpioSdlInfo->LocalRegMask[0],
						&gspio->LocalIncludeReg);
				ASSERT_EFI_ERROR(Status);
				if (EFI_ERROR(Status)) return Status;
	
			}
	
			//Update Enter exit Configuration Mode Script data...
			gspio->EnterCfgMode=(SIO_SCRIPT_LST2*)&SioChipSdlInfo->EnterCfgMode[0];
			gspio->ExitCfgMode=(SIO_SCRIPT_LST2*)&SioChipSdlInfo->ExitCfgMode[0];
			
			//Allocate buffer and Save Registers global registers content to compare what changed.
			//(PowerOn after PEI Init) state of SIO Global registers (reg 0.. 2F usually)
			gspio->GlobalCfgDump=Malloc(sizeof(UINT8)*gspio->GlobalInclRegCount);
			ASSERT(gspio->GlobalCfgDump);
			if(gspio->GlobalCfgDump==NULL) return EFI_OUT_OF_RESOURCES;
			SaveSioRegs(gspio, NULL, gspio->GlobalIncludeReg, gspio->GlobalInclRegCount,gspio->GlobalCfgDump);
	
			//Allocate memory for SIO LDs
			gspio->DeviceList=MallocZ(sizeof(VOID*)*gspio->LdCount);
			ASSERT(gspio->DeviceList);
			if(!gspio->DeviceList)return EFI_OUT_OF_RESOURCES;
	
			//Now init SIO LD Infrastructure...
			for(j=0, p8=(UINT8*)&(gspio->SpioSdlInfo->LogicalDev[0]); j<gspio->LdCount; j++){
	
				//There are difference in size between AMI_SDL_LOGICAL_DEV_INFO used in AptioV format
				//and SPIO_DEV_LST used in Apti 4.x to avoid data corruption use pointer UINT8* p8
				//adjustment in the bottom of this j loop instead just index reference....
				LdSdlInfo=(AMI_SDL_LOGICAL_DEV_INFO*)p8;
	
				dev=NewDev(LdSdlInfo,gspio, j, cm);
				
				SIO_TRACE((TRACE_SIO,"GSIO: Creating SIO_DEV2 structure for SIO[%d] LogicalDevice[%d] AslName[%s] @ 0x%X\n", i,j, LdSdlInfo->AslName, dev ));
				ASSERT(dev);
				if(!dev) return EFI_OUT_OF_RESOURCES;
				gspio->DeviceList[j]=dev;
	
				//should avoid Flags = SIO_NO_RES(0x80)
				//very first device in a list can't be a device with shared resources
				if(j==0 && dev->DeviceInfo->Flags&~SIO_NO_RES) return EFI_INVALID_PARAMETER;
	
				if(dev->DeviceInfo->Flags&~SIO_NO_RES && ro){
					dev->ResOwner=ro;
					ro->ResConsumer=dev;
				}
	
				//Advance to next Resource Owner.
				ro=dev;
	
				if(cm) p8+=(sizeof(SPIO_DEV_LST));
				else  p8+=(sizeof(AMI_SDL_LOGICAL_DEV_INFO));
			} //for j
			//In MultiSIO case set "SioChipSdlInfo" var to 
			//point at the next address after last Logical Device of SIO_CHIP[i]
			//this is a start Address of the next AMI_SDL_SIO_CHIP_INFO
			SioChipSdlInfo=(AMI_SDL_SIO_CHIP_INFO*)(LdSdlInfo+1);
		} //for i
		
		//Check the condition here 
		if(cm){
			//if we are in compatibilityMode (cm==TRUE) here we are done processing Aptio4.x style entries.
			cm=FALSE;
			if(!mV) break; //so if no AptioV style SIO found -just exit
		} else { 
			//if we are in AptioV Mode (cm==FALSE) here we are done processing Aptio4.x style entries.
			cm=TRUE;
			if(!m4) break; //so if no Aptio4.x style SIO found -just exit
		}
	} //for k
	
	SIO_TRACE((TRACE_SIO,"GSIO: SIO Initialization ENDs successfully!\n"));
	return EFI_SUCCESS;
}

/**
    This is AcpiSupport Protocol presence event handler, will find DSDT table

    @param Event AcpiSupportProtocol installed Event Handle
    @param Context Pointer to the optional Data buffer

    @retval VOID

**/ 
#if ACPI_SUPPORT
VOID EFIAPI GetDsdt(IN EFI_EVENT Event, IN VOID *Context){
	UINTN					i;
	EFI_STATUS				Status;
	EFI_PHYSICAL_ADDRESS	dsdtaddr=0;
//-----------------------------
	
//	EFI_DEADLOOP();
	
	Status=LibGetDsdt(&dsdtaddr,EFI_ACPI_TABLE_VERSION_ALL);
	if(EFI_ERROR(Status)) return;
	else gPDsdt=(ACPI_HDR*)dsdtaddr;

	if(!gPDsdt)	SIO_TRACE((TRACE_SIO,"GSIO: FAIL to locate DSDT Table -> %r\n", Status));
	else {
		for(i=0;i<gSpioCount;i++){
			Status=SioEnumerateAll(&gSpio[i]);	
			SIO_TRACE((TRACE_SIO,"GSIO: EnumerateAll (ACPI Mode) for SIO Chip # %d returned %r\n",i, Status));
		}
		
		gAmiSioSmmHandoff.SpioCount=gSpioCount;
		gAmiSioSmmHandoff.SpioData=&gSpio[0];
		
		//install driver binding protocol here ....
		Status = pBS->InstallMultipleProtocolInterfaces(
					&gAmiSioDriverBinding.DriverBindingHandle,
					&gEfiDriverBindingProtocolGuid,&gAmiSioDriverBinding,
					&gEfiComponentName2ProtocolGuid, &gAmiSioComponentName,
					&gAmiSioSmmHandoffProtocolGuid, &gAmiSioSmmHandoff,
					NULL,NULL);
		ASSERT_EFI_ERROR(Status);				
	}

	if(Event) pBS->CloseEvent(Event);
}
#endif	

/**
    This Function will get NonVolatile SioDevice Setup Var if it is not present 
	it will create it or set Volatile SioDevice Setup Var parameter Get is the 
	Action Selector

    @param Dev Pointer to the private LD Data
    @param Get Action Selector

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends

**/  
EFI_STATUS EFIAPI SioSetupData(SIO_DEV2 *Dev, BOOLEAN Get){
	EFI_STATUS	Status=0,s2=0;
	EFI_GUID	ssg=SIO_VARSTORE_GUID;
	EFI_STRING	vname=NULL;
	UINTN		vs;
//---------------------------------
	//vname=MallocZ(sizeof(UINT16)*14);
	//ASSERT(vname);
	//if(!vname)return EFI_OUT_OF_RESOURCES;

	if(Get){
		//Get Non Volatile Variable for this Device 
		//the variable name is spels as "PNP"+Dev->PnpId+'_'+Dev_Uid"
		//Swprintf(&vname[0],L"PNP%04X_%X_NV",Dev->DeviceInfo->PnpId,Dev->DeviceInfo->Uid);
		vname=GetSioLdVarName(Dev,FALSE);
		vs=sizeof(SIO_DEV_NV_DATA);
		Status = pRS->GetVariable(vname, &ssg, NULL, &vs, &Dev->NvData);
		if(vname!=NULL) pBS->FreePool(vname);
		
		//upon creation Dev->NvData Dev->VlData was initialized with '0'
		if(EFI_ERROR(Status)){
			if(Status!=EFI_NOT_FOUND) return Status;
			Dev->NvData.DevEnable=1; //Default is Enabled
		}
		//If SIO setup screens NOT using Generic SIO Setup Screens 
		//we need to get setup options from the global SETUP_DATA var.
		if(Dev->CompatibleMode) {
			Status=SioLibLaunchCompRoutine(Dev,isGetSetupData);
		} else {
			Status=SioLibLaunchInitRoutine(Dev,isGetSetupData);
		}
	} else {
		//First get number of LD Supported Modes;
		Status=SioLibLaunchInitRoutine(Dev,isGetModeData);
		
		//Set Volatile var
		vname=GetSioLdVarName(Dev,TRUE);
		vs=sizeof(SIO_DEV_V_DATA);
		s2 = pRS->SetVariable(vname, &ssg, 
				(EFI_VARIABLE_BOOTSERVICE_ACCESS), 
				vs, &Dev->VlData);
		if(vname!=NULL) pBS->FreePool(vname);
		vname=NULL;
		
		//Set NonVolatile var
		//Swprintf(&vname[0],L"PNP%04X_%X_NV",Dev->DeviceInfo->PnpId,Dev->DeviceInfo->Uid);
		vname=GetSioLdVarName(Dev,FALSE);
		vs=sizeof(SIO_DEV_NV_DATA);
		Status = pRS->SetVariable(vname, &ssg, 
				(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
				vs, &Dev->NvData);
		if(vname!=NULL) pBS->FreePool(vname);
	}
	return (s2|Status);
}

/**
    Allocates memory for SPIO Logical Device Instance and init basic fields

    @param SioDevInfo Pointer to the AMI SDL Porting Data Stucture describing LD
    @param Owner Pointer at SIO Private Data Structure where this LD belongs 
    @param SdlIdx Index in AMI SDL Data array of SioDevInfo
    @param CompatibleMode Tells if SIO is standard SIO (Logical Device concept)

    @retval Pointer at created SIO_DEV2 or NULL if unsuccessful
 
**/  
SIO_DEV2* EFIAPI NewDev( AMI_SDL_LOGICAL_DEV_INFO *SioDevInfo, GSIO2 *Owner, UINTN SdlIdx, BOOLEAN CompatibleMode){
	SIO_DEV2	*dev;
//---------------------------
	dev=MallocZ(sizeof(SIO_DEV2));
	if(!dev) return NULL;
	
	//Fill out SIO_DEV2 struct;
	dev->Owner=Owner;
	dev->DeviceInfo=SioDevInfo;
	dev->DevInfoIdx=SdlIdx;
    dev->EfiSioData.Owner=dev;
    dev->CompatibleMode=CompatibleMode;
    
	//dev->Type=SioDevInfo->Type;
	//dev->LDN=SioDevInfo->LDN;	
	dev->EisaId.UID=SioDevInfo->Uid;
	dev->EisaId.HID=PNP_HID(SioDevInfo->PnpId);
	
	//Get Device Setup Data Volatile and NonVolatile
	//Can't use variable services now because it is NOT_AVAILABLE_YET
	dev->VlData.DevImplemented=SioDevInfo->Implemented;
	dev->NvData.DevEnable=SioDevInfo->Implemented;
	//dev->Implemented=
	//dev->Flags=SioDevInfo->Flags;
	//dev->InitRoutine=SioDevInfo->InitRoutine;
	dev->LocalCfgDump=MallocZ(Owner->LocalInclRegCount);
	if(dev->LocalCfgDump==NULL){
		pBS->FreePool(dev);
		dev=NULL;
	} else {
		SaveSioRegs(Owner, dev, Owner->LocalIncludeReg, Owner->LocalInclRegCount, dev->LocalCfgDump);
	}
	return dev;
}

//==================================================================================
/**
    Routine to transit Sio in/from Config Mode.

    @param Sio Pointer to the SIO Private Data
    @param Enter Action Selector
	
    @retval VOID

**/  
VOID EFIAPI SioCfgMode(GSIO2 *Sio, BOOLEAN Enter)
{
	UINTN				i;
	SIO_SCRIPT_LST2		*sl;
//---------------------------------
	if(Enter)sl=Sio->EnterCfgMode;
	else sl=Sio->ExitCfgMode;

    if(sl==NULL) return;

	for (i=0; i<sl->InstrCount; i++){
		switch (sl->OpType){
			case cfNone: 
				break;
			case cfByteSeq:
			{
				SPIO_SCRIPT  *Instr = (SPIO_SCRIPT*) &sl->Instruction[0];
				SPIO_SCRIPT	*cmd = &Instr[i];
				UINT16		reg;
				UINT8		dat;	
			//------------------------
				if(cmd->IdxDat)reg=Sio->SpioSdlInfo->SioIndex;
				else reg=Sio->SpioSdlInfo->SioData;
				
				if (cmd->WrRd) IoWrite8(reg,cmd->Value);
				else {
					UINTN c=100000;
					dat=IoRead8(reg);
					while( c && dat!=cmd->Value ) {
						dat=IoRead8(reg);
						c--;	
					}
				}
				break;
			}
			case cfRoutine:
			{
				UINT32	*FunctionNo = (UINT32*)&sl->Instruction[0];
				LaunchCfgModeRoutine(Sio,*FunctionNo, FALSE);
				break;
			}

			default: return;
		}//switch
	}//for
	Sio->InCfgMode=Enter;
	return;
}

/**
    Save Sio Registers content in the "Buffer"

    @param Sio Pointer to the SIO Private Data
    @param Dev Pointer to the private LD Data
    @param InclRegsList Pointer to array with Register Offsets
    @param InclRegCount Number of registers in "InclRegsList" array
    @param Buffer on return this buffer will be filled with values read from registers specified in InclRegsList

    @retval VOID

**/ 
VOID EFIAPI SaveSioRegs(GSIO2 *Sio, SIO_DEV2 *Dev, UINT8 *InclRegsList, UINTN InclRegCount, UINT8 *Buffer){
	UINTN	i;	
	UINT8	r;
//------------------------
	if(!Sio->InCfgMode) SioCfgMode(Sio, TRUE);
	if(Dev)DevSelect(Dev);
	for(i=0; i<InclRegCount; i++){
		r=InclRegsList[i];
		IoWrite8(Sio->SpioSdlInfo->SioIndex,r);
		Buffer[i]=IoRead8(Sio->SpioSdlInfo->SioData);
	}
	SioCfgMode(Sio, FALSE);
}


/**
    This Routine Enumerates SIO Infrastructure. 

        
    @param Spio Pointer at Internal SIO Device Data structure

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI SioEnumerateAll(GSIO2 *Spio){
	UINTN			i;
	SIO_DEV2		*dev;	
	EFI_STATUS		Status=0;
//-------------------
	for(i=0; i<Spio->LdCount; i++){
		dev=Spio->DeviceList[i];
		SIO_TRACE((TRACE_SIO,"=================================================\n"));
        SIO_TRACE((TRACE_SIO,"+> SIO_LD[%d]; AslName[%s];Implemented=%d; FLAGS=%X;", i,dev->DeviceInfo->AslName,dev->DeviceInfo->Implemented, dev->DeviceInfo->Flags));
        //Taking care of issue 1 in EIP72716 
		if (!dev->DeviceInfo->Implemented){
			SIO_TRACE((TRACE_SIO,"<-\n"));
			continue;
		}	
		if(dev->DeviceInfo->HasSetup)Status=SioSetupData(dev,TRUE);
		//For Irq reservation it might be a disabled by setup device that shares it's resources
		//It appears to be a resource owner in such case it will have field Resource consumer filled.
		SIO_TRACE((TRACE_SIO," Enabled=%d; SHR_RES=%d. <-\n",dev->NvData.DevEnable, (dev->ResConsumer!=NULL)));
		if(!dev->NvData.DevEnable) if(dev->ResConsumer==NULL) continue;
//if we don't have any ACPI support we still have to prowide _PRS for SIO devices
//#ifndef rather DEBUG
#if (!ACPI_SUPPORT)		
		switch (dev->DeviceInfo->Type){
			case dsFDC: 	Status=SioDxeLibSetFdcPrs(dev); 
				break;
			case dsPS2CK:
			case dsPS2K: 	Status=SioDxeLibSetPs2kPrs(dev);
				break;
			case dsPS2CM:
			case dsPS2M: 	Status=SioDxeLibSetPs2mPrs(dev);
				break;
			case dsUART:	Status=SioDxeLibSetUartPrs(dev);
				break;		
			case dsLPT:		Status=SioDxeLibSetLptPrs(dev,FALSE);
				break;
			case dsGAME:	Status=SioDxeLibSetGamePrs(dev);		
				break;
            case dsMPU401:  Status=SetMpu401Prs(dev);
                break;      
//PORT THIS IF NEEDED +++
//If SIO supports following Logical device types:
//			case dsSB16:	//Status=SetSb16Prs(dev);
//				break;
//			case dsFmSynth: //fill this
//				break;
//			case dsCIR:
//				break;
//			case dsGPIO:	Status=SetGpioPrs(dev);
//				break;
//			case dsHwMon:	Status=SetHhmPrs(dev);
//				break;
//			case dsPME:		Status=SetPmePrs(dev);
//				break;
//			case dsACPI:
//				break;
// Add to SIO_DEV_TYPE devices which may reside in SIO
// Fill this switch with implementation for missed SIO_DEV_TYPE
//PORT THIS IF NEEDED ---
			default : Status=GetPrsFromTable(dev, i);
		}//switch
#else
		Status=SioDxeLibGetPrsFromAml(dev,NULL,i);
#endif
		//if ACPI Support was Installed after VariableWrite Protocol
		//we will reserve ISA IRQs now! 
		//if not we will do it when VariableWrite Event signalled.
		if(EFI_ERROR(Status)) {
			//check if device has a custom routine to get PRS settings.
			if(dev->CompatibleMode) {
				Status=SioLibLaunchCompRoutine(dev,isPrsSelect);
			} else {
				Status=SioLibLaunchInitRoutine(dev,isPrsSelect);
			}
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
//			Status=EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
		} 
		if(mVarWrite){
		    Status=ReserveIrq(dev, i);
		    SIO_TRACE((TRACE_SIO,"GSIO: Reserve IRQ for SIO.Device[%d] - Status=%r\n",i,Status));
		}
        SIO_TRACE((TRACE_SIO,"-------------------------------------------------\n\n"));
	}//for

	return Status;
}
	


///
/// Functions for Driver Binding Protocol Interface
///
EFI_STATUS EFIAPI AmiSioComponentNameGetControllerName(IN EFI_COMPONENT_NAME2_PROTOCOL *This,
						IN  EFI_HANDLE                   ControllerHandle,
						IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
						IN  CHAR8                        *Language,
						OUT CHAR16                       **ControllerName )
{
	return EFI_UNSUPPORTED;
}

EFI_STATUS EFIAPI AmiSioComponentNameGetDriverName(IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
						IN  CHAR8                        *Language,
						OUT CHAR16                       **DriverName)
{
	//Supports only English
	if(!Language || !DriverName) return EFI_INVALID_PARAMETER;

	if (!LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH)) 
        return EFI_UNSUPPORTED;
	else 
        *DriverName=gDriverName;
	
	return EFI_SUCCESS;
}


//==============================================================================

/**
    This is a routine will check if EFI_DEVICE_PATH_PROTOCOL is a valid instance
	for AMI_SIO_PROTOCOL or EFI_SUPER_IO_PROTOCOL.

    @param DevicePath Pointer to EFI_DEVICE_PATH_PROTOCOL instance
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_UNSUPPORTED if Device Path is not valid
**/ 

EFI_STATUS EFIAPI CheckSioDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
    // Check if the RemainingDevicePath is valid
	if (DevicePath->Type!=ACPI_DEVICE_PATH ||
			DevicePath->SubType!= ACPI_DP ||
			NODE_LENGTH(DevicePath) != ACPI_DEVICE_PATH_LENGTH ) 
	return EFI_UNSUPPORTED;
	else return EFI_SUCCESS;
}


//==================================================================================
//Functions for Driver Binding Protocol Interface
//==================================================================================
/**
    This is Supported Function of the EFI_DRIVER_BINDING_PROTOCOL
    of Generic SIO Driver.
	
	@note See EFI Specification for detail description
**/
EFI_STATUS EFIAPI AmiSioSupported(IN EFI_DRIVER_BINDING_PROTOCOL	*This,
						   IN EFI_HANDLE                    Controller,
						   IN EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath)
{
	EFI_PCI_IO_PROTOCOL		*PciIo;		
	EFI_STATUS				Status;
	EFI_DEVICE_PATH_PROTOCOL	*ParentDevicePath;
	UINT8					cc[4];
	UINTN					sn,bn,dn,fn,i;
	UINT32					sid, id;
	BOOLEAN					AlreadyStarted = FALSE, m;
	GSIO2					*spio;
	AMI_SDL_PCI_DEV_INFO	*lpcbrg;
//---------------------------------------------------------
	//Check if it is valid Device Path 
	if(RemainingDevicePath){
		Status=CheckSioDevicePath(RemainingDevicePath);
		if(EFI_ERROR(Status))return Status;
	}

	//Check if it is real hardware or some virtual driver faking hardware
	//the real hardware has to have DevPath Protocol on it's HAndle
	Status=pBS->OpenProtocol( Controller, &gEfiDevicePathProtocolGuid,
							(VOID **)&ParentDevicePath,	This->DriverBindingHandle,     
							Controller, EFI_OPEN_PROTOCOL_BY_DRIVER	);
	if ( EFI_ERROR(Status) && Status!= EFI_ALREADY_STARTED) return Status;
 
	pBS->CloseProtocol(Controller,&gEfiDevicePathProtocolGuid,This->DriverBindingHandle, Controller);

	//This is real hardware we are dealing with but it has to be PCI2LPC/ISA bridge
	//So check if it has PCI IO Protocol on its Handle
	Status=pBS->OpenProtocol( Controller,&gEfiPciIoProtocolGuid,(VOID **)&PciIo,
							This->DriverBindingHandle,Controller,EFI_OPEN_PROTOCOL_BY_DRIVER);

	if(Status==EFI_ALREADY_STARTED){
		AlreadyStarted = TRUE;
		Status=pBS->OpenProtocol(Controller,&gEfiPciIoProtocolGuid,(VOID **)&PciIo,
							This->DriverBindingHandle,Controller,EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	}
	
	if(EFI_ERROR(Status))return Status;

	//PciIo Protocol present. 
	//Read PCI Class Code Reg to find out which Device is that 
	Status=PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32,PCI_REV_ID_OFFSET,1,&cc);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;	
	//VendorID DevId
	Status=PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32,0,1,&id);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;	
	//Subsystem VendorID 
	Status=PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32,0x2c,1,&sid);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;	
	//Controller PCI Location 
	Status=PciIo->GetLocation(PciIo,&sn, &bn, &dn, &fn);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;	

	//try to select right SIO device bridge for this sio
	Status=EFI_UNSUPPORTED;
	for(i=0; i<gSpioCount; i++){
		spio=&gSpio[i];
		lpcbrg=&gSdlPciData->PciDevices[spio->SpioSdlInfo->LpcBridgeIndex];

		//SDL PCI Data Index of the Parent LPC device Bus# Dev# and Fun# are updated already.
		m=(lpcbrg->Bus==bn);
		if(!m) continue;

		m=(lpcbrg->Device==dn);
		if(!m) continue;

		m=(lpcbrg->Function==fn);
		if(!m) continue;

		//if we are here: The controller PCi properties matched the SPIO_LIST ITEM table
		spio->SupportedHandle=Controller;
		Status=EFI_SUCCESS;
	}			
	//Check it has to be LPC/ISA bridge who hosts SIO	
	//byte0=RevId;byte1=BaseClassCode;byte2=SubClassCode;byte3=ProgrInterface
	if( Status && ( cc[3]==PCI_CL_BRIDGE && cc[2]==PCI_CL_BRIDGE_SCL_ISA && cc[1]==0 )) Status=EFI_SUCCESS;
	
	if (!AlreadyStarted) pBS->CloseProtocol(Controller,&gEfiPciIoProtocolGuid,This->DriverBindingHandle,Controller);

	return Status;
}


/**
    This Function provides generic access to the SIO registers
	

    @param Dev Pointer to the private LD Data
    @param Write Action Selector
    @param Val Pointer to the data byte/buffer 

    @retval VOID
	
	@note This function assumes SIO is in Config Mode and LD has been selected
**/
VOID EFIAPI SioRegister(SIO_DEV2 *Dev, BOOLEAN Write, UINT8 Reg, UINT8 *Val){
	IoWrite8(Dev->Owner->SpioSdlInfo->SioIndex,Reg);
	if(Write)IoWrite8(Dev->Owner->SpioSdlInfo->SioData,*Val);
	else *Val=IoRead8(Dev->Owner->SpioSdlInfo->SioData);
}

/**
    This Function Selects SIO Logical device "Dev"
	
    @param Dev Pointer to the private LD Data

    @retval VOID

	@note This function assumes SIO is in Config Mode
**/
VOID EFIAPI DevSelect(SIO_DEV2 *Dev){
//----------------
    //in case of non Standard SIO w/o Device Select Register specified just return
	if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (Dev->Owner->SpioSdlInfo->DevSelReg==0)) return;
    
	//all other case 
    IoWrite8(Dev->Owner->SpioSdlInfo->SioIndex,Dev->Owner->SpioSdlInfo->DevSelReg);
    IoWrite8(Dev->Owner->SpioSdlInfo->SioData,Dev->DeviceInfo->Ldn);
    	
    
}


/**
    This function will probe SIO Logical Device "Dev" if it is responding 
    to IO transactions.

    @param Dev Pointer to the private LD Data

    @retval EFI_SUCCESS if everything good
    @retval EFI_NO_RESPONSE if not

	@note This function assumes SIO is in Config Mode
**/
EFI_STATUS EFIAPI CheckDevicePresent(SIO_DEV2 *Dev){
	UINT8		val;
//---------------
    //in case of non Standard SIO w/o Device Select Register specified just return SUCCESS
	if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (Dev->Owner->SpioSdlInfo->DevSelReg==0)) return EFI_SUCCESS;

	//following logic will work only for LD based SIOs
	DevSelect(Dev);
	val=IoRead8(Dev->Owner->SpioSdlInfo->SioData);
	//it must read Device's LDN
    if(Dev->DeviceInfo->Ldn!=val) return EFI_NO_RESPONSE;
    else return EFI_SUCCESS;
}

/**
    This Function Enables or Disables SIO Logical device "Dev"
	
    @param Dev Pointer to the private LD Data
    @param Enable Action Selector

    @retval VOID

	@note This function assumes SIO is in Config Mode
**/
VOID EFIAPI DevEnable(SIO_DEV2 *Dev, BOOLEAN Enable){
	UINT8		v = Enable ? Dev->Owner->SpioSdlInfo->ActivateVal : Dev->Owner->SpioSdlInfo->DeactivateVal;
//----------------
	DevSelect(Dev);
	
    //in case of non Standard SIO w/o Device Enable Register specified just return SUCCESS
	if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (Dev->Owner->SpioSdlInfo->ActivateReg==0)) return;

	IoWrite8(Dev->Owner->SpioSdlInfo->SioIndex,Dev->Owner->SpioSdlInfo->ActivateReg);
	IoWrite8(Dev->Owner->SpioSdlInfo->SioData,v);
}


/**
    This Function selects Possible Resource Settings List by number
	
    @param DfLst Pointer to the Possible Resource Settings Lists Collection 
    @param num Resource List Number in collection "DfLst" 

    @retval Pointer to the Possible Resource Settings List

**/
T_ITEM_LIST* EFIAPI GetNumResources(T_ITEM_LIST *DfLst, UINT8 *num ){
//	ASLRF_S_HDR		*hdr;
	EFI_ASL_DepFn	*df=NULL;//*dfnp,*dfwp;
//-------------------------------------
	
	if(*num<DfLst->ItemCount){
//		hdr=(ASLRF_S_HDR*)DfLst->Items[*num];
//		if(hdr->Length==1) {	
//			dfwp=(EFI_ASL_DepFn*)hdr;
			df=(EFI_ASL_DepFn*)DfLst->Items[*num];
			return &df->DepRes;
//		}
//		if(hdr->Length==0) {	
//			dfnp=(EFI_ASL_DepFnNoPri*)hdr;
//			return &dfnp->DepRes;
//		}
//		*num=(UINT8)DfLst->ItemCount;
//		return NULL;
	} else *num=(UINT8)DfLst->ItemCount;
	return NULL; 
}


//Selects Resources by Priotity settings.
//UINT8 *num number of descriptors in Dependency list;
/**
    This Function selects Possible Resource Settings List by Priotity settings
	
    @param DfLst Pointer to the Possible Resource Settings Lists Collection 
    @param cmp Resource List preference level 
    @param prf Resource List preference level 

    @retval Pointer to the Possible Resource Settings List

    @note See ACPI Spec DependentFn macro
**/
T_ITEM_LIST* EFIAPI GetPriResources(T_ITEM_LIST *DfLst, UINT8 cmp, UINT8 prf){
	UINTN				i;
	ASLRF_S_HDR			*hdr;
	EFI_ASL_DepFn		*df;		
//-------------------------------------
	for(i=0; i<DfLst->ItemCount; i++){
		df=DfLst->Items[i];
		hdr=(ASLRF_S_HDR*)df->DepFn;
		if(hdr->Length==1) {	
			if(((ASLR_StartDependentFn*)df->DepFn)->_PRI._CMP==cmp || 
				((ASLR_StartDependentFn*)df->DepFn)->_PRI._PRF==prf)
				return &df->DepRes;
		}
	} 
	return NULL; 
}


/**
    This Function, like GCD, will controll legacy DMA resource assignment

    @param ChNo legacy DMA channel number 0,1,2 or 3
    @param Flags Pointer to DMA flags
	
    @retval TRUE if everything good
    @retval FALSE if something wrong happends
	
	@note See ACPI Spec _DMA macro for details
**/
BOOLEAN EFIAPI AssignDma(UINT8 ChNo, ASLRF_DFLAGS *Flags){
	UINT16		dma;
	EFI_STATUS 	Status;
//------------------------	
  	Status=AmiIsaDmaMask(&gAvailableDmaChnl, TRUE);
	if(EFI_ERROR(Status)){
		SIO_TRACE((TRACE_SIO, "Unexpected Error while refreshing Var ISA_DMA_MASK. STATUS = %r\n", Status));
		return FALSE;
	}

	SIO_TRACE((TRACE_SIO, "Updating ISA_DMA_MASK = 0x%X", gAvailableIsaIrq));

	dma=(1<<ChNo);
	if((~gAvailableDmaChnl) & dma){
		gAvailableDmaChnl |= dma;
  		Status=AmiIsaDmaMask(&gAvailableDmaChnl,FALSE);
		if(EFI_ERROR(Status)){
			SIO_TRACE((TRACE_SIO, "Unexpected Error. STATUS = %r\n", Status));
			return FALSE;
		}
		SIO_TRACE((TRACE_SIO, " with 0x%X for Chnl %d\n", dma, ChNo ));
		return TRUE;
	} else return FALSE;	
}

/**
    This Function, like GCD, will controll legacy IRQ resource assignment

    @param IrqNo legacy IRQ number 0..15 
    @param Flags Pointer to IRQ flags
	
    @retval TRUE if everything good
    @retval FALSE if something wrong happends
	
	@note See ACPI Spec _IRQ macro for details
**/
BOOLEAN EFIAPI AssignIrq(UINT8 IrqNo, ASLRF_IFLAGS *Flags){
	UINT16		irq;
	EFI_STATUS 	Status;
//------------------------	

  	Status=AmiIsaIrqMask(&gAvailableIsaIrq, TRUE);
	if(EFI_ERROR(Status)){
		SIO_TRACE((TRACE_SIO, "Unexpected Error while refreshing Var ISA_IRQ_MASK. STATUS = %r\n", Status));
		return FALSE;
	}
	
	SIO_TRACE((TRACE_SIO, "Updating ISA_IRQ_MASK = 0x%X", gAvailableIsaIrq));

	irq=(1<<IrqNo);
	if((~gAvailableIsaIrq) & irq){
		gAvailableIsaIrq |= irq;
		gReservedIrq |= irq;
  		Status=AmiIsaIrqMask(&gAvailableIsaIrq,FALSE);
		if(EFI_ERROR(Status)){
			SIO_TRACE((TRACE_SIO, "Unexpected Error. STATUS = %r\n", Status));
			return FALSE;
		}
		SIO_TRACE((TRACE_SIO, " with 0x%X for IRQ# %d ISA_IRQ_MASK=%X gReservedIrq=%X\n", irq, IrqNo,gAvailableIsaIrq, gReservedIrq));
		return TRUE;
	} else return FALSE;	
}

/**
    This Function, will controll SIO IO resource assignment 
    and report assigned IO range to the GCD services.

    @param Base IO resource Base Address 
    @param Len IO resource Length
    @param Aln IO resource AddressAlignment requirements
	
    @retval TRUE if everything good
    @retval FALSE if something wrong happends
	
	@note See ACPI Spec _IRQ macro for details
**/
BOOLEAN EFIAPI AssignIo(UINT16 *Base, UINT8 Len, UINT8 Aln){
	EFI_PHYSICAL_ADDRESS	addr=*Base;
	EFI_STATUS				Status;
//------------------------------------
	
	if(!gDxeSvcTbl)	return FALSE;
	Status=gDxeSvcTbl->AllocateIoSpace(EfiGcdAllocateAddress,//IN EFI_GCD_ALLOCATE_TYPE AllocateType,
								EfiGcdIoTypeIo,//IN EFI_GCD_IO_TYPE GcdIoType,
								Aln,//IN UINTN Alignment,
								Len, //IN UINT64 Length,
								&addr,//IN OUT EFI_PHYSICAL_ADDRESS *BaseAddress,
								gAmiSioDriverBinding.ImageHandle,// IN EFI_HANDLE ImageHandle,
								NULL	//IN EFI_HANDLE DeviceHandle OPTIONAL
	);
	SIO_TRACE((TRACE_SIO,"AssignIo Status=%r,*Base=%x, addr=%x, Len=%x, Aln=%x\n",Status,*Base,addr,Len,Aln));
	if(EFI_ERROR(Status)||((*Base)!=(UINT16)addr))return FALSE;	
	*Base=(UINT16)addr;
	return TRUE;	
}


/**
    This Function, will assign DMA resources to the SIO LD 
    specified as "Dev" and update corresponded private data.

    @param Hdr Pointer to ASLRF_S_HDR of ACPI resource descriptor with resource information
    @param Dev Pointer to the private LD Data
    @param ResNo Indicates what resource number is that (0 - first or 1 - second)
    each device can have up to 2 resouses of the same type.
	
    @retval VOID* at the resource Descriptor Buffer or NULL if unsuccessful
**/
VOID* EFIAPI ApplyDmaRes(ASLRF_S_HDR* Hdr, SIO_DEV2* Dev,UINT8 ResNo ){
	UINT8 			dmac=0;
	UINT8			i;
	BOOLEAN			a=FALSE;
	ASLR_DMA		*dmar=(ASLR_DMA*)Hdr, *rd;
	EFI_STATUS      Status=EFI_SUCCESS;
//--------------------------
	
	rd=ASLM_DMA(1,dmar->Flags._TYP,dmar->Flags._BM, dmar->Flags._SIZ,0); 
    if(rd==NULL) return rd;

	rd->_DMA=0;

    SIO_TRACE((TRACE_SIO,"\nGSIO: ApplyIO StdSio=%d, DMA=%x,rd=%x; ",dmar->_DMA,rd));
    //Don't need to run all this if we got an empty descriptor
    if(!dmar->_DMA){
        SIO_TRACE((TRACE_SIO,"Empty Descriptor\n"));
        //Don't need to run all this if we got an empty descriptor
        return rd;
    }
	
	for(i=0; i<8; i++){
		dmac=dmar->_DMA&(1<<i);
		if(dmac){
			if((Dev->DeviceInfo->Flags&SIO_SHR_DMA2) && ResNo ) a=TRUE;
			else { 
				if((Dev->DeviceInfo->Flags&SIO_SHR_DMA1) && !ResNo) a=TRUE;
				else a=AssignDma(i, &dmar->Flags);
			}
			if(a){
				UINT8 r;
			//---------------------
				if(ResNo){
					r=Dev->Owner->SpioSdlInfo->Dma2Reg;
					Dev->VlData.DevDma2=i;
				} else {
					r=Dev->Owner->SpioSdlInfo->Dma1Reg;
					Dev->VlData.DevDma1=i;
				}
				
				if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (r==0))
					Status=SioLibLaunchResourceRoutine(Dev, srDmaReg1+ResNo);
				else SioRegister(Dev,TRUE,r,&i);
				
				SIO_TRACE((TRACE_SIO,"r=%x,i=%x, %r\n",r,i, Status));

				rd->_DMA=i;
				return rd;
			}  		
		}
	} //for 
	if(rd)pBS->FreePool(rd);
	SIO_TRACE((TRACE_SIO,"returning NULL!\n"));
	return NULL;
}



/**
    This Function, will assign or reserve IRQ resources to the SIO LD 
    specified as "Dev" and update corresponded private data.

    @param Hdr Pointer to ASLRF_S_HDR of ACPI resource descriptor with resource information
    @param Dev Pointer to the private LD Data
    @param ResNo Indicates what resource number is that (0 - first or 1 - second)
    each device can hev up to 2 resouses of the same type.
    @param IrqReserve Action Selector (if TRUE indicates IRQ reservation phase)
	
    @retval VOID* at the resource Descriptor Buffer or NULL if unsuccessful
**/
VOID* EFIAPI ApplyIrqRes(ASLRF_S_HDR* Hdr, SIO_DEV2* Dev, UINT8 ResNo, BOOLEAN IrqReserve){
	UINT16 			irqm=0;
	UINT8			i;
	BOOLEAN			a=FALSE;
	ASLR_IRQ		*irqr=(ASLR_IRQ*)Hdr, *rd=NULL;	
	EFI_STATUS      Status=EFI_SUCCESS;
//--------------------------
	
	SIO_TRACE((TRACE_SIO,"\nGSIO: Device's IRQ_MSK=%X; ",irqr->_INT));
	
	//Create Irq resource descriptors based on request 
	if(!IrqReserve){
		if(irqr->Hdr.Length==3) rd=ASLM_IRQ(1,irqr->Flags._LL,irqr->Flags._HE, irqr->Flags._SHR,0); 
		else rd=ASLM_IRQNoFlags(1,0); 
		if(rd==NULL) return rd;
		rd->_INT=0;
	}

	//Don't need to run all this if we got an empty descriptor
	if(!irqr->_INT) {
        SIO_TRACE((TRACE_SIO,"Empty Descriptor\n"));
        return rd; 
    }

	for(i=0; i<16; i++){
		irqm=irqr->_INT&(1<<i);
		if(irqm ){
            //if we hit IRQ that was used already by some other SIO device, just skip it.
            if((IrqReserve == FALSE) && ((irqm & gUsedSioIrq ) && (!(Dev->DeviceInfo->Flags & SIO_SHR_IRQ)) ) ) continue;
            
			if((Dev->DeviceInfo->Flags&SIO_SHR_IRQ2) && ResNo ) a=TRUE;
			else {
				if((Dev->DeviceInfo->Flags&SIO_SHR_IRQ1) && !ResNo) a=TRUE;
				else {
					if(!Dev->IrqReserved) a=AssignIrq(i, &irqr->Flags);
					else a=TRUE;
				}
			}
			if(a){
				UINT8 r;
			//---------------------
				if(IrqReserve) {
					Dev->IrqReserved=TRUE;
					SIO_TRACE((TRACE_SIO,"GSIO: ApplyIrq - IrqReserve=%d. Returning NULL!",IrqReserve));
					return NULL;
				}
				//Special Case for PS2C (PS2K and PS2M in the same LD)
				if(Dev->DeviceInfo->Type==dsPS2CM) ResNo=1;				
				if(ResNo){
					r=Dev->Owner->SpioSdlInfo->Irq2Reg;
					//If share IRQ, it should share the ResOwner's IRQ
					if(Dev->DeviceInfo->Flags&SIO_SHR_IRQ2){
						i=Dev->ResOwner->VlData.DevIrq2;
					}
					Dev->VlData.DevIrq2=i;
				} else {
					r=Dev->Owner->SpioSdlInfo->Irq1Reg;
					//If share IRQ, it should share the ResOwner's IRQ
					if(Dev->DeviceInfo->Flags&SIO_SHR_IRQ1){
						i=Dev->ResOwner->VlData.DevIrq1;
					}
					Dev->VlData.DevIrq1=i;
				}
                gUsedSioIrq |= irqm;
				SIO_TRACE((TRACE_SIO,"GSIO: ApplyIrq - dsPS2CM=%d; Register=0x%X; Value=0x%X; gUsedSioIrq=0x%X.",
				(Dev->DeviceInfo->Type==dsPS2CM), r, i, gUsedSioIrq));
				
				if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (r==0))
					Status=SioLibLaunchResourceRoutine(Dev, srIrqReg1+ResNo);
				else SioRegister(Dev,TRUE,r,&i);
				
				SIO_TRACE((TRACE_SIO,"r=%x,i=%x, %r\n",r,i, Status));
				
				rd->_INT=i;
				return rd;
			}
		}  		
	}
	if(rd)pBS->FreePool(rd);
	SIO_TRACE((TRACE_SIO,"returning NULL!\n"));
	return NULL;
}

/**
    This Function, will assign IO resources passed as FixedIO ACPI 
	Resource Descriptorto the SIO LD specified as "Dev" and will update 
	corresponded private data.

    @param Hdr Pointer to ASLRF_S_HDR of ACPI resource descriptor with resource information
    @param Dev Pointer to the private LD Data
    @param ResNo Indicates what resource number is that (0 - first or 1 - second)
    each device can have up to 2 resouses of the same type.
	
    @retval VOID* at the resource Descriptor Buffer or NULL if unsuccessful
**/

VOID* EFIAPI ApplyFixedIoRes(ASLRF_S_HDR* Hdr, SIO_DEV2* Dev, UINT8 ResNo){
	BOOLEAN			a=FALSE;
	ASLR_FixedIO	*iord=(ASLR_FixedIO*)Hdr, *rd;
	EFI_STATUS      Status=EFI_SUCCESS;
//--------------------------
	rd=ASLM_FixedIO(iord->_BAS, iord->_LEN);
    if(rd==NULL) return rd;

	SIO_TRACE((TRACE_SIO,"\nGSIO: ApplyFixedIO BAS=%x, LEN=%x, rd=%x; ",iord->_BAS,iord->_LEN,rd));
	//Don't need to run all this if we got an empty descriptor
	if(!iord->_LEN) {
        SIO_TRACE((TRACE_SIO,"Empty Descriptor\n"));
		return rd;
	}
	
	if((Dev->DeviceInfo->Flags&SIO_SHR_IO2) && ResNo ) a=TRUE;
	else {
		if((Dev->DeviceInfo->Flags&SIO_SHR_IO1) && !ResNo) a=TRUE;
		else a=AssignIo(&rd->_BAS, rd->_LEN, 0);
	}

	if(a){
		UINT8 	r,chk,*b;
	//---------------------
		b=(UINT8*)&iord->_BAS;

		if(ResNo){
			chk=Dev->Owner->SpioSdlInfo->Base2HiReg | Dev->Owner->SpioSdlInfo->Base2LoReg;
			r=Dev->Owner->SpioSdlInfo->Base2HiReg;
			Dev->VlData.DevBase2=iord->_BAS;
		} else {
			chk=Dev->Owner->SpioSdlInfo->Base1HiReg | Dev->Owner->SpioSdlInfo->Base1LoReg;
			r=Dev->Owner->SpioSdlInfo->Base1HiReg;
			Dev->VlData.DevBase1=iord->_BAS;
		}
		
		if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (chk==0)){
		    SIO_TRACE((TRACE_SIO,"chk=%x,",chk));
			Status=SioLibLaunchResourceRoutine(Dev, srBaseAddrReg1+ResNo);
		} else {
		    SIO_TRACE((TRACE_SIO,"r=%x,",r));
			SioRegister(Dev,TRUE,r,&b[1]);

			if(ResNo)r=Dev->Owner->SpioSdlInfo->Base2LoReg;
			else r=Dev->Owner->SpioSdlInfo->Base1LoReg;
		
			SioRegister(Dev,TRUE,r,&b[0]);
		}
			
	    SIO_TRACE((TRACE_SIO,"r=%x,b[0]=%x,b[1]=%x, %r\n",r,b[0],b[1], Status));

		return rd;
	}
	if(rd)pBS->FreePool(rd);	
	SIO_TRACE((TRACE_SIO,"returning NULL\n"));	
	return NULL;
}

/**
    This Function, will assign IO resources passed as _IO ACPI 
	Resource Descriptorto the SIO LD specified as "Dev" and will update 
	corresponded private data.

    @param Hdr Pointer to ASLRF_S_HDR of ACPI resource descriptor with resource information
    @param Dev Pointer to the private LD Data
    @param ResNo Indicates what resource number is that (0 - first or 1 - second)
    each device can have up to 2 resouses of the same type.
	
    @retval VOID* at the resource Descriptor Buffer or NULL if unsuccessful
**/

VOID* EFIAPI ApplyIoRes(ASLRF_S_HDR* Hdr, SIO_DEV2* Dev, UINT8 ResNo){
	BOOLEAN			a=FALSE;
	ASLR_IO			*iord=(ASLR_IO*)Hdr,*rd=NULL;
	UINT8			aln=0;
	EFI_STATUS      Status=EFI_SUCCESS;
//--------------------------
	rd=ASLM_IO(iord->_DEC,iord->_MIN,iord->_MAX,iord->_ALN,iord->_LEN);
    if(rd==NULL) return rd;

    SIO_TRACE((TRACE_SIO,"\nGSIO: ApplyIO StdSio=%d, MIN=%x, LEN=%x, rd=%x; ",iord->_MIN,iord->_LEN,rd));
    //Don't need to run all this if we got an empty descriptor
    if(!iord->_LEN) {
        SIO_TRACE((TRACE_SIO,"Empty Descriptor\n"));
        //Don't need to run all this if we got an empty descriptor
        return rd;
    }

	if((Dev->DeviceInfo->Flags&SIO_SHR_IO2) && ResNo ) a=TRUE;
	else {
		if((Dev->DeviceInfo->Flags&SIO_SHR_IO1) && !ResNo) a=TRUE;
		else {
			if((rd->_ALN) != 0)aln=rd->_ALN-1;
			a=AssignIo(&rd->_MIN, rd->_LEN,aln);
		}
	}

	if(a){
		UINT8 r,chk,*b;
	//---------------------
		b=(UINT8*)&iord->_MIN;

		//Special Case for FDC (second resource request 0x3F7 don't have a corresponded register)
		if((Dev->DeviceInfo->Type==dsFDC) && (ResNo==1)) return rd;				

		if(ResNo){
			chk=Dev->Owner->SpioSdlInfo->Base2HiReg | Dev->Owner->SpioSdlInfo->Base2LoReg;
			r=Dev->Owner->SpioSdlInfo->Base2HiReg;
			Dev->VlData.DevBase2=iord->_MIN;
		} else {
			chk=Dev->Owner->SpioSdlInfo->Base1HiReg | Dev->Owner->SpioSdlInfo->Base1LoReg;
			r=Dev->Owner->SpioSdlInfo->Base1HiReg;
			Dev->VlData.DevBase1=iord->_MIN;
		}
		
		if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (chk==0)){
		    SIO_TRACE((TRACE_SIO,"chk=%x,",chk));
			Status=SioLibLaunchResourceRoutine(Dev, srBaseAddrReg1+ResNo);
		} else {
		    SIO_TRACE((TRACE_SIO,"r=%x,",r));
			SioRegister(Dev,TRUE,r,&b[1]);

			if(ResNo)r=Dev->Owner->SpioSdlInfo->Base2LoReg;
			else r=Dev->Owner->SpioSdlInfo->Base1LoReg;

			SioRegister(Dev,TRUE,r,&b[0]);
		}
		
	    SIO_TRACE((TRACE_SIO,"r=%x,b[0]=%x,b[1]=%x, %r\n",r,b[0],b[1], Status));

		return rd;
	} 
	if(rd)pBS->FreePool(rd);
	SIO_TRACE((TRACE_SIO,"returning NULL\n"));	
	return NULL;
}

/**
    This Function, will free SIO LD specified as "Dev" IRQ resources.

    @param dev Pointer to the private LD Data
    @param dsc Pointer to ACPI IRQ resource descriptor with resource information
	
    @retval VOID
**/
VOID EFIAPI FreeIrq(SIO_DEV2 *dev, ASLR_IRQ *dsc)
{
	UINT8	v=0;
//-------------------	
	if ((gAvailableIsaIrq & dsc->_INT) == 0) {
		gAvailableIsaIrq |= dsc->_INT;
		
		dev->VlData.DevIrq1=0;
		dev->VlData.DevIrq2=0;
		
		if(dev->Owner->SpioSdlInfo->StdSioDevice){
		    SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Irq1Reg, &v);
		    SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Irq2Reg, &v);
        } else {
		    SioLibLaunchResourceRoutine(dev, srIrqReg1);
		    SioLibLaunchResourceRoutine(dev, srIrqReg2);
		}
	}
}

/**
    This Function, will free SIO LD specified as "Dev" DMA resources.

    @param dev Pointer to the private LD Data
    @param dsc Pointer to ACPI DMA resource descriptor with resource information
	
    @retval VOID
**/
VOID EFIAPI FreeDma(SIO_DEV2 *dev, ASLR_DMA *dsc)
{
	UINT8	v=0;
//-------------------	
	if ((gAvailableDmaChnl & dsc->_DMA) == 0) {
		gAvailableDmaChnl |= dsc->_DMA;

		dev->VlData.DevDma1=0;
		dev->VlData.DevDma2=0;
		
        if(dev->Owner->SpioSdlInfo->StdSioDevice){
            SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Dma1Reg, &v);
		    SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Dma2Reg, &v);
        } else {
		    SioLibLaunchResourceRoutine(dev, srDmaReg1);
		    SioLibLaunchResourceRoutine(dev, srDmaReg2);
		}
	}
}

/**
    This Function, will free SIO LD specified as "Dev" IO resources.

    @param dev Pointer to the private LD Data
    @param dsc Pointer to ACPI IO resource descriptor with resource information
	
    @retval VOID
**/
VOID EFIAPI FreeIo(SIO_DEV2 *dev, ASLRF_S_HDR	*dsc){
	UINT8	v=0;
//-------------------	
	if(dsc->Name==ASLV_RT_FixedIO){
		gDxeSvcTbl->FreeIoSpace( ((ASLR_FixedIO*)dsc)->_BAS, ((ASLR_FixedIO*)dsc)->_LEN);
	} else {
		gDxeSvcTbl->FreeIoSpace( ((ASLR_IO*)dsc)->_MIN, ((ASLR_IO*)dsc)->_LEN);
	}
	dev->VlData.DevBase1=0;
	dev->VlData.DevBase2=0;

	if(dev->Owner->SpioSdlInfo->StdSioDevice){
    	SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Base1HiReg, &v);
	    SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Base1LoReg, &v);
	    SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Base2HiReg, &v);
	    SioRegister(dev, TRUE, dev->Owner->SpioSdlInfo->Base2LoReg, &v);
	} else {
	    SioLibLaunchResourceRoutine(dev, srBaseAddrReg1);
	    SioLibLaunchResourceRoutine(dev, srBaseAddrReg2);
	}
}

/**
    This Function, will free ALL SIO LD specified as "Dev" resources if some
    of resources in tamplete failed to allocate

    @param dev Pointer to the private LD Data
    @param Res Pointer to ACPI IRQ resource descriptors list
	
    @retval VOID
**/

//Free GCD resources if Tesource Tamplete Allocation failed
VOID EFIAPI FreeResources(SIO_DEV2 *dev, T_ITEM_LIST *Res){
	UINTN 				i;
	ASLRF_S_HDR			*hdr;
	UINTN				bas,irq,dma;
//---------------------
	for(i=0, bas=0,irq=0,dma=0; i<Res->ItemCount; i++){
		hdr=(ASLRF_S_HDR*)Res->Items[i];

		switch(hdr->Name){

			case ASLV_RT_IO:
			case ASLV_RT_FixedIO: 
				bas++;
				if( (!(dev->DeviceInfo->Flags & SIO_SHR_IO1)) && (bas==1))FreeIo(dev,hdr);
				if( (!(dev->DeviceInfo->Flags & SIO_SHR_IO2)) && (bas==2))FreeIo(dev,hdr);
				break;
			case ASLV_RT_IRQ: 
				irq++;
				if( (!(dev->DeviceInfo->Flags & SIO_SHR_IRQ1)) && (irq==1))FreeIrq(dev, (ASLR_IRQ*)hdr);
				if( (!(dev->DeviceInfo->Flags & SIO_SHR_IRQ2)) && (irq==2))FreeIrq(dev, (ASLR_IRQ*)hdr);
				break;

			case ASLV_RT_DMA:
				dma++;
				if( (!(dev->DeviceInfo->Flags & SIO_SHR_DMA1)) && (dma==1))FreeDma(dev, (ASLR_DMA*)hdr);
				if( (!(dev->DeviceInfo->Flags & SIO_SHR_DMA2)) && (dma==2))FreeDma(dev, (ASLR_DMA*)hdr);
				break;
		} 
	}	
	dev->Assigned=FALSE;
}

/**
    This Function, will assign ALL types of resources passed as "ResLst"
	(ACPI Resource Descriptortors List) of the SIO LD specified as "Dev" and will update 
	corresponded private data.

    @param Dev Pointer to the private LD Data
    @param ResLst Pointer to a List of resource descriptors 
    @param IrqReserve Action Selector (if TRUE indicates Resource Reservation Phase)
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/

EFI_STATUS EFIAPI ApplyResources(SIO_DEV2 *Dev, T_ITEM_LIST *ResLst, BOOLEAN IrqReserve){
	UINTN			i;
	ASLRF_S_HDR		*hdr;
	EFI_STATUS		Status = IrqReserve ? EFI_UNSUPPORTED : EFI_SUCCESS;
	VOID			*rd;
	UINT8			irqc=0, basc=0, dmac=0;
//--------------------------------		
    SIO_TRACE((TRACE_SIO,"GSIO: ApplyResources "));
	//if device shares all resources we'll copy Resource Owner's CRS
	if(Dev->Assigned){
        SIO_TRACE((TRACE_SIO,"- Assigned Already\n"));
        return EFI_SUCCESS;
	}

    if(Dev->DeviceInfo->Flags & SIO_NO_RES){
        SIO_TRACE((TRACE_SIO,"- NO RSESOURCES USED\n"));
        return EFI_SUCCESS;
	}  

	if( (Dev->DeviceInfo->Flags & SIO_SHR_ALL )==SIO_SHR_ALL && 
			Dev->ResOwner->DeviceInfo->Ldn==Dev->DeviceInfo->Ldn &&
			(!IrqReserve))
	{
        SIO_TRACE((TRACE_SIO,"- Same LDN Share ALL Resources\n"));
		Dev->CRS.ItemCount=Dev->ResOwner->CRS.ItemCount;
		Dev->CRS.Items=Dev->ResOwner->CRS.Items;
		Dev->Assigned=TRUE;
		return EFI_SUCCESS;
	}
	
	for(i=0; i<ResLst->ItemCount; i++){
		rd=NULL;
		hdr=(ASLRF_S_HDR*)ResLst->Items[i];
		if (hdr->Type==ASLV_SMALL_RES){ //SIO mast have only small resources types
	//-------------------------
			switch(hdr->Name){
				case ASLV_RT_IRQ:
                    SIO_TRACE((TRACE_SIO,"- IRQ_Reg#%d; IrqReserve=%d\n",irqc+1, IrqReserve));

					rd=ApplyIrqRes(hdr,Dev,irqc,IrqReserve);
					irqc++;					
					break;

				case ASLV_RT_DMA:
					if(!IrqReserve){
                        SIO_TRACE((TRACE_SIO,"- DMA_Reg#%d >>",dmac+1));
						rd=ApplyDmaRes(hdr, Dev, dmac);
						dmac++;
					} else continue; 
					break;

				case ASLV_RT_IO:
					if(!IrqReserve){
                        SIO_TRACE((TRACE_SIO," IO_Reg#%d >>",basc+1));
						rd=ApplyIoRes(hdr, Dev, basc);
						basc++;
					} else continue; 
					break;

				case ASLV_RT_FixedIO:
					if(!IrqReserve){
                        SIO_TRACE((TRACE_SIO,"Fixed IO_Reg#%d >>",basc+1));
						rd=ApplyFixedIoRes(hdr, Dev, basc);
						basc++;
					} else continue; 
					break;
			}
		
			if(IrqReserve) continue;	
        
		    SIO_TRACE((TRACE_SIO,"rd=%x \n",rd));
			if(rd) Status=AppendItemLst(&Dev->CRS, rd);
			else Status=EFI_UNSUPPORTED;
		} else Status=EFI_UNSUPPORTED; //the SIO - doesn't supports LARGE RESOURCES  
		if (EFI_ERROR(Status)) break;
	}
    SIO_TRACE((TRACE_SIO,"\n"));

	//If Irq Reservation we no need to add any res descriptors yet
	if(IrqReserve && (Status==EFI_UNSUPPORTED)){
        SIO_TRACE((TRACE_SIO,"GSIO: ApplyResources(IrqReserve=%d) Status=EFI_SUCCESS\n",IrqReserve));
	    return EFI_SUCCESS; 
	}
	//clear resource tamplete if we fail to assign one of the members of ResLst
	if (EFI_ERROR(Status)){
		FreeResources(Dev, &Dev->CRS);
		ClearItemLst(&Dev->CRS, TRUE);
		Dev->Assigned=FALSE;
	} else Dev->Assigned=TRUE;
    SIO_TRACE((TRACE_SIO,"GSIO: ApplyResources(IrqReserve=%d) Dev->CRS count=%d; Status=%r\n",IrqReserve, Dev->CRS.ItemCount, Status));
	return Status;
}

/**
    This Function, will assign shared resources if SIO LD specified as "Dev" has one 
	and will update corresponded private data.

    @param Dev Pointer to the private LD Data
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/
EFI_STATUS EFIAPI AssignSharedResources(SIO_DEV2 *Dev){
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	EFI_STATUS		Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
	UINTN			i, bas,irq,dma;
	T_ITEM_LIST		*crs=&Dev->ResOwner->CRS;
	ASLRF_S_HDR		*hdr;
	BOOLEAN			app;
//----------------------------	
	for(i=0,bas=0,irq=0,dma=0,app=FALSE; i<crs->ItemCount; i++,app=FALSE){
		hdr=crs->Items[i];
		switch(hdr->Name){
			//IO resource type
			case ASLV_RT_FixedIO:
			case ASLV_RT_IO:
				bas++;
				if( (Dev->DeviceInfo->Flags&SIO_SHR_IO1)&& (bas==1)) app=TRUE;
				if( (Dev->DeviceInfo->Flags&SIO_SHR_IO2)&& (bas==2)) app=TRUE;
			break;
			//IRQ resource
			case ASLV_RT_IRQ :
				irq++;
				if( (Dev->DeviceInfo->Flags&SIO_SHR_IRQ1)&& (irq==1)) app=TRUE;
				if( (Dev->DeviceInfo->Flags&SIO_SHR_IRQ2)&& (irq==2)) app=TRUE;
				break;
			//DMA Resource
			case ASLV_RT_DMA:
				dma++;
				if( (Dev->DeviceInfo->Flags&SIO_SHR_DMA1)&& (dma==1)) app=TRUE;
				if( (Dev->DeviceInfo->Flags&SIO_SHR_DMA2)&& (dma==2)) app=TRUE;
				break;
		} //switch
		if(app){
			Status=AppendItemLst(&Dev->CRS,(VOID*)hdr);			
			if(EFI_ERROR(Status)) return Status;
		}
	}
	return Status;	
}

/**
    This Function, will assign resources to SIO LD specified as "Dev"  
	and will update corresponded private data.

    @param Dev Pointer to the private LD Data
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends

    @note Function Assumes SIO is in Config Mode and LD has been selected
**/
EFI_STATUS EFIAPI AssignResources(SIO_DEV2 *Dev){
	T_ITEM_LIST		*dl, *rl;
	UINT8			p=0,c=0, n=0;
	EFI_STATUS		Status;
//------------------------------------
	SIO_TRACE((TRACE_SIO,"GSIO: AssignResources "));
	if(gDxeSvcTbl==NULL) { 
        SIO_TRACE((TRACE_SIO,"- Dxe Sevice Table NOT_FOUND, returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
	}	

	if(Dev->Assigned) {
        SIO_TRACE((TRACE_SIO,"- Device has Resourcs Assigned, returning EFI_SUCCESS\n"));
        return EFI_SUCCESS;
	}

	//Take care about shared resources if any
	if(Dev->DeviceInfo->Flags & SIO_SHR_ALL){
        SIO_TRACE((TRACE_SIO,"- SHARED Flags=0x%X, Status=",Dev->DeviceInfo->Flags));
		Status=AssignSharedResources(Dev);
		if(Dev->DeviceInfo->Flags==SIO_SHR_ALL) {
			Dev->Assigned=TRUE;
			Status=EFI_SUCCESS;
	    	SIO_TRACE((TRACE_SIO,"%r\n",Status));
			return Status;
		}
        SIO_TRACE((TRACE_SIO,"%r\n",Status));
		ASSERT_EFI_ERROR(Status);
	}
	
	dl=&Dev->PRS;
	
	if(Dev->NvData.DevPrsId){
		n=Dev->NvData.DevPrsId-1;
		if(n<dl->ItemCount){
			rl=GetNumResources(dl,&n);
			if(rl){
				Status=ApplyResources(Dev,rl,FALSE);
                SIO_TRACE((TRACE_SIO,"- By PrsId(%x),get DepFn(%X) Status=%r\n",Dev->NvData.DevPrsId,rl,Status));
				if(!EFI_ERROR(Status))	return Status;
			}
		}
        SIO_TRACE((TRACE_SIO,"GSIO: AssignResources Fail to reserve By PrsId,%d DepFn in Count %d is %X \n",n,dl->ItemCount,rl));
		//if we can't apply selected resources reset it to "auto" again
		Dev->NvData.DevPrsId=0;
	}
	//use Auto Resource settings
	//try to get Prioritized resources
	while( c < 3 || p < 3 ){
		rl=GetPriResources(dl,c,p);
		if(rl) {
			Status=ApplyResources(Dev,rl,FALSE);
			if(!Status) return EFI_SUCCESS;
		}
		c++; p++; 
	}
	//Fail to get Dependent Function with Priority 
	//try by number
	n=0;
	while( n < dl->ItemCount){
		rl=GetNumResources(dl,&n);
		if(rl) {
			Status=ApplyResources(Dev,rl,FALSE);
            SIO_TRACE((TRACE_SIO,"- By PRS Num(n=%d),get DepFn(%X) Status=%r\n",n,rl,Status));
			if(!Status) return EFI_SUCCESS;
		}
		n++;
	}
	SIO_TRACE((TRACE_SIO,"\n"));	
	return EFI_NOT_FOUND;
}

/**
    This is a routine Implements Driver Binding Start function for 
	single SIO Logical Device

    @param CtrlHandle Handle of the SIO Controller
    @param Dev Pointer to the private LD Data to start
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/
EFI_STATUS EFIAPI StartSioChildDevice(EFI_HANDLE CtrlHandle, SIO_DEV2 *Dev){
	EFI_STATUS			Status=0;
	EFI_PCI_IO_PROTOCOL	*PciIo;		
//----------------------------------
	if(Dev->Started) return EFI_ALREADY_STARTED;
	//Now Install All protocols
	Status=pBS->InstallMultipleProtocolInterfaces(
				&Dev->Handle,
				&gEfiAmiSioProtocolGuid,  &Dev->AmiSio,
//#if ((defined PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x00010014))

		        &gEfiSioProtocolGuid, &Dev->EfiSioData.EfiSio, 
//#endif
				&gEfiDevicePathProtocolGuid, Dev->DevicePath, //DevPath  GUID - I/F pare
				NULL, NULL );

	SIO_TRACE((TRACE_SIO,"InstallProt()=%r.", Status));

    ASSERT_EFI_ERROR(Status);	
    if(EFI_ERROR(Status))return Status;

	Dev->Started=TRUE;

	//Open Protocol BY_CHILD_CONTROLLER to notify core we may produce CHILDS
	Status=pBS->OpenProtocol( CtrlHandle, &gEfiPciIoProtocolGuid, &PciIo,
					gAmiSioDriverBinding.DriverBindingHandle, Dev->Handle,
					EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);

	SIO_TRACE((TRACE_SIO,"Open(PciIo)=%r\n", Status));
    ASSERT_EFI_ERROR(Status);	
	
	return Status;	
}

/**
    This is a routine initiate IRQ Reservation Phase

    @param Dev Pointer to the private LD Data to start
    @param Index "Dev" index in SIO LD List

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/

EFI_STATUS EFIAPI ReserveIrq(SIO_DEV2 *Dev, UINTN Index){
	EFI_STATUS		Status;
	T_ITEM_LIST		*dl, *rl;
	UINT8			p=0,c=0, n=0;
//------------------------------------
	if(gDxeSvcTbl==NULL) return EFI_UNSUPPORTED;	

	if(Dev->IrqReserved) return EFI_SUCCESS;
	
    if(Dev->DeviceInfo->Flags & SIO_NO_RES){
        SIO_TRACE((TRACE_SIO,"- NO RSESOURCES USED\n"));
        return EFI_SUCCESS;
	}  


	//Take care about shared resources if any
	if((Dev->DeviceInfo->Flags & SIO_SHR_IRQ) == SIO_SHR_IRQ) return EFI_SUCCESS;
	
	dl=&Dev->PRS;
	
	if(Dev->NvData.DevPrsId){
		n=Dev->NvData.DevPrsId-1; //Very first Option is AUTO Select
		if(n<dl->ItemCount){
			rl=GetNumResources(dl,&n);
			if(rl){
				Status=ApplyResources(Dev,rl,TRUE); //Reserve Interrupts
				if(!EFI_ERROR(Status))	return Status;
			}
		}
		//if we can't apply selected resources reset it to "auto" again
		Dev->NvData.DevPrsId=0;
	}
	//use Auto Resource settings
	//try to get Prioritized resources
	while( c < 3 || p < 3 ){
		rl=GetPriResources(dl,c,p);
		if(rl) {
			Status=ApplyResources(Dev,rl,TRUE);
            SIO_TRACE((TRACE_SIO,"GSIO: ReserveIrq by PRI, Status=%r\n",Status));
			if(!Status) return EFI_SUCCESS;
		}
		c++; p++; 
	}
	//Fail to get Dependent Function with Priority 
	//try by number
	while( n < dl->ItemCount){
		rl=GetNumResources(dl,&n);
		if(rl) {
			Status=ApplyResources(Dev,rl,TRUE);
	        SIO_TRACE((TRACE_SIO,"GSIO: ReserveIrq PRS[%d], Status==%r\n",n,Status));
			if(!Status) return EFI_SUCCESS;
		}
		n++;
	}
	
	return EFI_NOT_FOUND;
}


/**
    This is a routine will initialize SIO Logical device.

    @param Dev Pointer to the private LD Data to start
    @param Index "Dev" index in SIO LD List

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/

EFI_STATUS EFIAPI InitSioDevice(SIO_DEV2 *Dev){
	EFI_STATUS			Status=0;
	BOOLEAN			    Dis_Flag=TRUE;
//-------------------------

	if (Dev->Started||Dev->Initialized) {
		SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() Device was Started=%d Initialized=%d - returning EFI_SUCCESS\n", 
		Dev->Started, Dev->Initialized));
        return EFI_SUCCESS;	
    }

	if(!Dev->Owner->InCfgMode) SioCfgMode(Dev->Owner, TRUE);	

	//See if there actual SIO there or just empty space
	Status=CheckDevicePresent(Dev);
	SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - CheckDevicePresent() Status=%r",Status));
	if(Status==EFI_NO_RESPONSE) Dev->DeviceInfo->Implemented=FALSE;
	SIO_TRACE((TRACE_SIO," Implemented=%d \n",Dev->DeviceInfo->Implemented));

	if(!EFI_ERROR(Status)) {
        if(Dev->DeviceInfo->Type == dsPS2CM) Dis_Flag=FALSE; //if ps2 mouse &ps2 keyboard in same logical device,not disable it
        if(Dis_Flag){ 
	       SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() Disabling Device\n"));
           DevEnable(Dev,FALSE);//disable all devices before programing
        }
      }
	
	if(Dev->DeviceInfo->Implemented && Dev->NvData.DevEnable){
		//if we are Initializing device with shared resources
		//make sure resource owner has it's resources Allocated! 
		if((Dev->DeviceInfo->Flags & SIO_SHR_ALL) && Dev->ResOwner){
          //we don't care about the irq share case
          if(!(Dev->DeviceInfo->Flags & SIO_SHR_IRQ)) {
			DevSelect(Dev->ResOwner);
            SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() AssignResources(Dev->ResOwner) -"));
			Status=AssignResources(Dev->ResOwner);
	        SIO_TRACE((TRACE_SIO," Status=%r; FLAGS=0x%X\n",Status, Dev->DeviceInfo->Flags));
			if(EFI_ERROR(Status)) return Status;

          }//if
		}//if
		
		DevSelect(Dev);
		if(Dev->DeviceInfo->Flags & SIO_NO_RES) {
	        SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Device has NO Resourcs\n"));
            Dev->Assigned=TRUE; 
        }else{ 
            SIO_TRACE((TRACE_SIO,"SIO InitSioDevice() AssignResources(Dev) -"));
            Status=AssignResources(Dev); //Assigned Flag is changed inside AssignResources
	        SIO_TRACE((TRACE_SIO," Status=%r\n",Status));
        }

		if(!EFI_ERROR(Status)){
			pBS->CopyMem(&Dev->AmiSio,&gAmiSioProtocol, sizeof(AMI_SIO_PROTOCOL));

//#if ((defined PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x00010014))
			pBS->CopyMem(&Dev->EfiSioData.EfiSio, &gEfiSioProtocol, sizeof(EFI_SIO_PROTOCOL));
//#endif
		
			if(!Dev->DevicePath){
				ACPI_HID_DEVICE_PATH	siodp;
			//--------------------------
				siodp.Header.Type=ACPI_DEVICE_PATH;
				siodp.Header.SubType=ACPI_DP;
				SET_NODE_LENGTH(&siodp.Header,ACPI_DEVICE_PATH_LENGTH);
				siodp.HID=Dev->EisaId.HID;
				siodp.UID=Dev->EisaId.UID;
				Dev->DevicePath = DPAddNode(Dev->Owner->CntrDevPath,&siodp.Header);
	            SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Set DevicePath=%X\n",Dev->DevicePath));
			}
			//Call Init BeforeActivate
			//if device is not implemented it might be needed to do some initialization 
			// even for disabled devices if so here we are 
            //SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Device InitRoutine(isBeforeActivate)"));
			if(Dev->CompatibleMode) {
				Status=SioLibLaunchCompRoutine(Dev,isBeforeActivate);
			} else {
				Status=SioLibLaunchInitRoutine(Dev,isBeforeActivate);
			}
			//SIO_TRACE((TRACE_SIO," Status=%r\n"));
			if(EFI_ERROR(Status))return Status;

			//In config routine we might exit config mode
			if(!Dev->Owner->InCfgMode) SioCfgMode(Dev->Owner, TRUE);	
	        SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Enabling Device\n"));	
			DevEnable(Dev,TRUE);

			//Call Init AfterActivate
        	//SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Calling InitRoutine(isAfterActivate)"));
        	if(Dev->CompatibleMode){
        		Status=SioLibLaunchCompRoutine(Dev,isAfterActivate); 		
        	} else {
				Status=SioLibLaunchInitRoutine(Dev,isAfterActivate);
			}
 			//SIO_TRACE((TRACE_SIO," Status=%r\n", Status));
			if(EFI_ERROR(Status)) return Status;
		} else {
		//Fail to assign device resource - so disable the device
	        SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Fail to Assign Resources, Disable Device!\n"));
			Dev->Started=TRUE;
			Dev->Assigned=TRUE;
			Dev->VlData.DevImplemented=FALSE;
		}
	} else { //if device was not Implemented on the board or was disabled by setup
		//set Flags in not implemented devices to avoid creating handles on them
	    SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Not Implemented or Disabled by Setup, Disable Device\n"));
		Dev->Started=TRUE;
		Dev->Assigned=TRUE;
	} 

	Dev->Initialized=TRUE;
		
	if(Dev->Owner->InCfgMode) SioCfgMode(Dev->Owner, FALSE);	

	if(Dev->DeviceInfo->HasSetup) Status=SioSetupData(Dev, FALSE);
 	SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Set Device VlData(DevImplemented=%x,DevBase1=%x,DevBase2=%x,DevIrq1=%x,DevIrq2=%x,DevDma1=%x,DevDma2=%x);\n",Dev->VlData.DevImplemented,Dev->VlData.DevBase1,Dev->VlData.DevBase2,Dev->VlData.DevIrq1,Dev->VlData.DevIrq2,Dev->VlData.DevDma1,Dev->VlData.DevDma2));
	SIO_TRACE((TRACE_SIO,"GSIO: InitSioDevice() - Set Device NvData(DevEnable=%x,DevPrsId=%x,DevMode=%x)\n",Dev->NvData.DevEnable,Dev->NvData.DevPrsId,Dev->NvData.DevMode));
	return Status;
}

/**
     Start Function of the EFI_DRIVER_BINDING_PROTOCOL of the Generic SIO Driver
	 
	 @note See EFI Specification for detail description
**/
EFI_STATUS EFIAPI AmiSioStart(IN EFI_DRIVER_BINDING_PROTOCOL		*This,
					   IN EFI_HANDLE						Controller,
					   IN EFI_DEVICE_PATH_PROTOCOL			*RemainingDevicePath )
{
	EFI_PCI_IO_PROTOCOL		*PciIo;		
	EFI_STATUS			Status=EFI_SUCCESS,StatusRdp=EFI_SUCCESS;
	INTN				i;
	UINTN				j,k;
	SIO_DEV2			*dev;
	ACPI_HID_DEVICE_PATH	*dp=NULL;
	GSIO2				*spio;
    BOOLEAN             AllStarted=TRUE;
    BOOLEAN             GcdIndex=TRUE;
    BOOLEAN             Init_Flag=FALSE;
//#ifdef EFI_DEBUG
//    CHAR8               *DpString=NULL;
//#endif
//--------------------------------
#if ACPI_SUPPORT
	if(!gPDsdt) return EFI_DEVICE_ERROR;
#endif
    PROGRESS_CODE(DXE_SIO_INIT);
	SIO_TRACE((TRACE_SIO,"GSIO: AmiSioStart() - gSpio=%x,gSpioCount=%x\n",gSpio,gSpioCount));
	for(j=0; j<gSpioCount; j++){
		spio=&gSpio[j];	
	    SIO_TRACE((TRACE_SIO,"\nSIO: AmiSioStart() - SIO[%d] SupportController=%X, This Controller=%X\n",j,spio->SupportedHandle,Controller));	
		if(spio->SupportedHandle!=Controller) continue;
		if(!spio->CntrDevPath){
			Status=pBS->OpenProtocol( Controller, &gEfiDevicePathProtocolGuid,
								(VOID **)&dp,This->DriverBindingHandle,     
								Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL	);
			if(EFI_ERROR(Status) && Status!= EFI_ALREADY_STARTED) return Status;
			spio->CntrDevPath=DPCopy((EFI_DEVICE_PATH_PROTOCOL*)dp);
			pBS->CloseProtocol(Controller,&gEfiDevicePathProtocolGuid,This->DriverBindingHandle, Controller);
			spio->CntrHandle=Controller;
			//To Create a chain of childs
			Status=pBS->OpenProtocol( Controller,&gEfiPciIoProtocolGuid,(VOID **)&PciIo,
								  This->DriverBindingHandle,Controller,EFI_OPEN_PROTOCOL_BY_DRIVER);
			//if there are 2 or more SIO on the same LPC Bridge it should return something like that
			if(EFI_ERROR(Status)) {
				if(Status!=EFI_ALREADY_STARTED)	return Status;	
				//Try to scan back and find opend PciIoProtocol for the previouse SIO
	            SIO_TRACE((TRACE_SIO,"GSIO: AmiSioStart() - The BindingDriver has been opened PciIo protocol for SIO chip=%X\n"));	
				for (i=j-1; i>=0; i--)if(gSpio[i].CntrHandle==Controller) spio->IsaBrgPciIo=gSpio[i].IsaBrgPciIo;
				if(!spio->IsaBrgPciIo) return EFI_DEVICE_ERROR;
			} else spio->IsaBrgPciIo=PciIo;
			//Reserve SIO Index/Data port in GCD
	        SIO_TRACE((TRACE_SIO,"GSIO: AmiSioStart() - SIO[%d] =>Assign cfg Index=%X\n",j,spio->SpioSdlInfo->SioIndex));
        //it is not first sio index and it is different with former index.

         if(j) {
			for (i=j-1; i>=0; i--)  
              if( (gSpio[i].SpioSdlInfo)->SioIndex==spio->SpioSdlInfo->SioIndex ) {
                  GcdIndex=FALSE;
                  break;
              }
          }
			//Reserve SIO Index/Data port in GCD when it is first io index or the index is different with other
          if(GcdIndex) {
			if(!AssignIo(&spio->SpioSdlInfo->SioIndex, 1, 0)) continue;
			if(!AssignIo(&spio->SpioSdlInfo->SioData, 1, 0)) continue;
          }
		} //if(!spio->CntrDevPath)

		dp=(ACPI_HID_DEVICE_PATH*)RemainingDevicePath;
		
		SIO_TRACE((TRACE_SIO,"\nGSIO: Starting ... RDP= %X\n",(UINTN)RemainingDevicePath));
	
    for(k=0;k<2;k++){
		//Locate Right SIO Device
		for (i=0; (UINTN)i<spio->LdCount; i++){
            Status=EFI_SUCCESS;
			dev=spio->DeviceList[i];
			SIO_TRACE((TRACE_SIO,"\n=========================================================================\n"));
            if((k==0)&&(dev->DeviceInfo->Type==dsUART)&&(dev->NvData.DevPrsId!=0)){
			   SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] InitSio Starting: first time=>\n",j,i));
               Init_Flag=TRUE;
            }
            if((k==1)&&((dev->DeviceInfo->Type!=dsUART)||(dev->NvData.DevPrsId==0))){
			   SIO_TRACE((TRACE_SIO,"\nSIO[%d]Dev[%d] InitSio Starting: second time=>\n",j,i));
               Init_Flag=TRUE;
            }
            if(Init_Flag) {
              	Status=InitSioDevice(dev);
                Init_Flag=FALSE;
            }
            else continue;

			SIO_TRACE((TRACE_SIO,"InitSioDevice()=%r;\n ", Status));
			if(EFI_ERROR(Status)){
				if (Status == EFI_NO_RESPONSE) {
			    SIO_TRACE((TRACE_SIO,"Device check EFI_NO_RESPONSE,Continue next device...\n",j,i));
                continue;
               } else {
            	   SIO_TRACE((TRACE_SIO,"=>ERROR,Stop %r AmiSioStart...<=\n",Status,j,i));
               	   return Status;
               }
			}

			if(RemainingDevicePath==NULL){
				SIO_TRACE((TRACE_SIO,"Start SIO[%d]Dev[%d] ChildDevice:",j,i));
				Status=StartSioChildDevice(Controller,dev);
				SIO_TRACE((TRACE_SIO,"StartSioDev()=%r; Hnd=%X;\n", Status, dev->Handle));
			
				if(EFI_ERROR(Status)){
                    if(Status!=EFI_ALREADY_STARTED){
					    return Status; 	
				    } else Status=EFI_SUCCESS;
                } else AllStarted=FALSE;
				SIO_TRACE((TRACE_SIO,"AllStarted=%X;\n", AllStarted));
			} else {
				SIO_TRACE((TRACE_SIO,"RemainingDevicePath is not Null,HID(%x)_UID(%x)\n",dp->HID,dp->UID));
                StatusRdp=EFI_NOT_FOUND;
			    if(dp->HID==dev->EisaId.HID && dp->UID==dev->EisaId.UID){
				    StatusRdp=StartSioChildDevice(Controller,dev);
				    SIO_TRACE((TRACE_SIO,"RDP; StartSioDev()=%r; Hnd=%X;\n", StatusRdp, dev->Handle));
				    //just keep going initializing all other devices.
				    //break; //return Status;
			    }
            }
		    SIO_TRACE((TRACE_SIO,"\n"));
		}//for i
      }//for k
	}// for j
	//if(AllStarted && RemainingDevicePath==NULL) CreateSioDevStatusVar();  
    CreateSioDevStatusVar();  
    
    if(RemainingDevicePath!=NULL) {
		SIO_TRACE((TRACE_SIO,"\nSIO: AmiSioStart() - returning RdpStatus = %r\n", StatusRdp));
    	return StatusRdp;
    }

    if(AllStarted) Status=EFI_ALREADY_STARTED;
    else  Status=EFI_SUCCESS;
	SIO_TRACE((TRACE_SIO,"\nSIO: AmiSioStart() - AllStarted=%d, returning %r\n", AllStarted, Status));
	return Status;

}

/**
    This is a routine Stops SIO LD specified as "Dev"

    @param Dev Pointer to the private LD Data
 	@param Controller SIO Controller handle
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 

EFI_STATUS EFIAPI StopSioDevice(SIO_DEV2 *Dev,EFI_HANDLE Controller)
{
	EFI_STATUS 				Status=EFI_SUCCESS;
//--------------------------------------------------------------------

    if(Dev->DeviceInfo->Implemented==FALSE || Dev->VlData.DevImplemented==FALSE){
        SIO_TRACE((TRACE_SIO,"GSIO: StopSioDevice - Device is NOT Implemented, returning EFI_SUCCESS\n"));
        return Status;
    }

    if(Dev->Started==FALSE) return Status;    

	Status=pBS->CloseProtocol(Controller,&gEfiPciIoProtocolGuid,
			gAmiSioDriverBinding.DriverBindingHandle, Dev->Handle);


	SIO_TRACE((TRACE_SIO,"GSIO: Closing(PciIo)=%r; ", Status));
    ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status))return Status;	


	Status=pBS->UninstallMultipleProtocolInterfaces(
					Dev->Handle,
					&gEfiAmiSioProtocolGuid,&Dev->AmiSio,
//#if ((defined PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x00010014))
			        &gEfiSioProtocolGuid, &Dev->EfiSioData.EfiSio, 
//#endif
					&gEfiDevicePathProtocolGuid,Dev->DevicePath,
					NULL, NULL );
	SIO_TRACE((TRACE_SIO,"Uninstalling(AmiSio;"));
//#if ((defined PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x00010014))
	SIO_TRACE((TRACE_SIO," EfiSio;")); 
//#endif
    SIO_TRACE((TRACE_SIO," DevPath)=%r;\n", Status));
    ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) 	return Status;

	//don't need to free and clear the device path buffer of the device 
	//- it not going to change any way. Just care not to create it over again 
	//when Start function will be called
	Dev->Handle=NULL;
	Dev->Started=FALSE;
	DevEnable(Dev, FALSE); 

	Status = DisableDevInSioDevStatusVar(Dev);

	if(EFI_ERROR(Status))SIO_TRACE((TRACE_SIO,"GSIO: Stop=> %r - Update SioDevStatusVar.\n", Status));
    ASSERT_EFI_ERROR(Status);

	return Status;
}	


/**
     Stop Function of the EFI_DRIVER_BINDING_PROTOCOL of the Generic SIO Driver
	 
	 @note See EFI Specification for detail description
**/

EFI_STATUS EFIAPI AmiSioStop(IN EFI_DRIVER_BINDING_PROTOCOL			*This,
					   IN EFI_HANDLE						Controller,
					   IN UINTN								NumberOfChildren,
					   IN EFI_HANDLE						*ChildHandleBuffer)
{
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	EFI_STATUS	Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
	UINTN		i,j,k,cnt=0;	
	SIO_DEV2	*dev;
	GSIO2		*spio;
//------------------------
	SIO_TRACE((TRACE_SIO,"\nGenericSIOStop: NumberOfChildren=0x%X, Controller=0x%X\n", NumberOfChildren, Controller));

	for(k=0; k<gSpioCount; k++){
		spio=&gSpio[k];
		if(spio->CntrHandle!=Controller) continue;

		if (!NumberOfChildren) {
			for(i=0; i<spio->LdCount; i++){
				SIO_TRACE((TRACE_SIO,"SIO[%d] Stopping: ", i));
                dev=spio->DeviceList[i];
                if(dev->Started) Status=StopSioDevice(spio->DeviceList[i], Controller);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
                else Status=EFI_SUCCESS; //Already stopped
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
				SIO_TRACE((TRACE_SIO,"%r.\n", Status));
				if(EFI_ERROR(Status)) return Status;
 			}
            //If everything OK we should close PciIo Protocol that we opened BY_DRIVER
   			Status=pBS->CloseProtocol(Controller,&gEfiPciIoProtocolGuid,
					  This->DriverBindingHandle,Controller);
		} else {
			for(j=0; j<NumberOfChildren; j++){

				for(i=0; i<spio->LdCount; i++){
					dev=spio->DeviceList[i];

					if(dev->Handle==ChildHandleBuffer[j]){		
						SIO_TRACE((TRACE_SIO,"SIO[%d] Stopping: ", i));
						Status=StopSioDevice(dev,Controller);
						SIO_TRACE((TRACE_SIO,"%r.\n", Status));
						if(EFI_ERROR(Status)) return Status;
						cnt++;
						break;
					}
				} //for i
			} //for j				
		} //else
        spio->CntrDevPath=NULL;
 	}//for k	
	
	if(NumberOfChildren && (!cnt)) Status=EFI_NOT_FOUND;
    ASSERT_EFI_ERROR(Status);

    return Status;
}

///
/// AmiSIO Protocol Functions Implementation
///


EFI_STATUS EFIAPI AmiSioRegister(IN AMI_SIO_PROTOCOL		*This,
						IN BOOLEAN				Write,
						IN BOOLEAN				ExitCfgMode,					
						IN UINT8            	Register,
    					IN OUT UINT8        	*Value)
{
	SIO_DEV2	*dev=(SIO_DEV2*)This;	
//-----------------------------------	
	if(!This || !Value || Register>SIO_MAX_REG ) return EFI_INVALID_PARAMETER;
	
	if(!dev->Owner->InCfgMode) SioCfgMode(dev->Owner, TRUE);	
	
	DevSelect(dev);
	SioRegister(dev, Write, Register, Value);

	if(ExitCfgMode) {
		if(dev->Owner->InCfgMode) SioCfgMode(dev->Owner, FALSE);	
	} 
	return EFI_SUCCESS;

}

EFI_STATUS EFIAPI AmiSioCRS(IN AMI_SIO_PROTOCOL	*This,
				    IN BOOLEAN 				Set,
					IN OUT T_ITEM_LIST		**Resources)
{
	EFI_STATUS	Status;
	SIO_DEV2		*dev=(SIO_DEV2*)This;	
//-----------------------------------	
	if (!This || !Resources) return EFI_INVALID_PARAMETER;
	
	if (Set) {
		FreeResources(dev, &dev->CRS);
		ClearItemLst(&dev->CRS, TRUE);
		Status = ApplyResources(dev, *Resources, FALSE);
		ASSERT_EFI_ERROR(Status);
	} else {
		Status=CopyItemLst(&dev->CRS, Resources); 
		ASSERT_EFI_ERROR(Status);
	}

	return Status;
}

/**
    This is a routine provides generic access to SIO LDs registers.
	It is standard function from EFI_SIO_PROTOCOL 

    @param This Pointer to the EFI_SIO_PROTOCOL instance
    @param Write Action Selector
    @param ExitCfgMode Action Selector 2 if TRUE will exit SIO Config Mode after access operation
	@param Register SIO LD Register to access
	@param Value Pointer to the Data value/buffer to write/read
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 

EFI_STATUS EFIAPI EfiSioRegisterAccess(
  IN   CONST  EFI_SIO_PROTOCOL  *This,
  IN          BOOLEAN           Write,
  IN          BOOLEAN           ExitCfgMode,
  IN          UINT8             Register,
  IN OUT      UINT8             *Value)
{
    EFI_SIO_DATA    *EfiSioData=(EFI_SIO_DATA*)This;
//---------------------
	if(This==NULL) return EFI_INVALID_PARAMETER;
    
    return AmiSioRegister(&EfiSioData->Owner->AmiSio,Write,ExitCfgMode,Register,Value);     
    
}

/**
    This is a helper function to convert AMI_SIO resource list to EFI_SIO format.

    @param AmiResLst Pointer to the AMI_SIO resource list
    @param EfiResLst Pointer to the EFI_SIO resource list
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI EfiSioGetRes(T_ITEM_LIST *AmiResLst, ACPI_RESOURCE_HEADER_PTR  *EfiResLst){
    UINTN                       i, sz=0;
    ASLRF_S_HDR                 *pRes;
    VOID                        *EfiSioRes=NULL;       
    UINT8                       *p8;
//----------------
    for(i=0; i<AmiResLst->ItemCount; i++){
        pRes=(ASLRF_S_HDR*)AmiResLst->Items[i];
        if(pRes->Type==ASLV_SMALL_RES) sz+=((UINTN)pRes->Length+sizeof(ASLRF_S_HDR));
        else sz+=(((ASLRF_L_HDR*)pRes)->Length+sizeof(ASLRF_L_HDR));
    }

    EfiSioRes=Malloc(sz+sizeof(ASLR_EndTag));
    if(EfiSioRes==NULL) return EFI_OUT_OF_RESOURCES;

    for(i=0,p8=EfiSioRes; i<AmiResLst->ItemCount; i++){
        pRes=(ASLRF_S_HDR*)AmiResLst->Items[i];
        if(pRes->Type==ASLV_SMALL_RES) sz=((UINTN)pRes->Length+sizeof(ASLRF_S_HDR));
        else sz=((UINTN)((ASLRF_L_HDR*)pRes)->Length+sizeof(ASLRF_L_HDR));
        
        MemCpy(p8,(VOID*)pRes, sz);        
        p8+=sz;        
    }
    
    ((ASLR_EndTag*)p8)->Hdr.HDR=ASLV_END_TAG_HDR;
	((ASLR_EndTag*)p8)->Chsum=0;
    
    EfiResLst->SmallHeader=EfiSioRes;
    
    return EFI_SUCCESS;
}


/**
    This is a routine provides Resource consumption information of the SIO LDs
	It is standard function from EFI_SIO_PROTOCOL 

    @param This Pointer to the EFI_SIO_PROTOCOL instance
    @param ResourceList Pointer to the EFI_SIO resource list
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI EfiSioGetResourcces( 
  IN  CONST EFI_SIO_PROTOCOL            *This,
  OUT       ACPI_RESOURCE_HEADER_PTR    *ResourceList)
{
    EFI_STATUS      Status;
    EFI_SIO_DATA    *EfiSioData=(EFI_SIO_DATA*)This;
    T_ITEM_LIST		*ResLst=NULL; //if NULL new Buffer will be allocated...
//---------------------
	if(This==NULL || ResourceList==NULL) return EFI_INVALID_PARAMETER;

    //Get resources in AMI SIO format...
	Status=AmiSioCRS(&EfiSioData->Owner->AmiSio, FALSE, &ResLst);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    //Convert it to EFI_SUPER_IO format.
    Status=EfiSioGetRes(ResLst, ResourceList);
    ASSERT_EFI_ERROR(Status);
    return Status;
}

/**
    This is a helper function to convert AMI_SIO resource list to EFI_SIO format.

    @param AmiResLst Pointer to the AMI_SIO resource list
    @param EfiResLst Pointer to the EFI_SIO resource list
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI EfiSioSetRes(T_ITEM_LIST *AmiResLst, ACPI_RESOURCE_HEADER_PTR  EfiResLst){
    UINTN       sz=0;
    UINT8       *p8;
    EFI_STATUS  Status;
//----------------
    p8=(UINT8*)EfiResLst.SmallHeader;
        
    //Convert resources to AMI_SIO format.
    while( ((ASLR_EndTag*)p8)->Hdr.HDR!=ASLV_END_TAG_HDR ){
        if( ((ASLRF_S_HDR*)p8)->Type==ASLV_SMALL_RES )sz=(UINTN)(((ASLRF_S_HDR*)p8)->Length+sizeof(ASLRF_S_HDR));
        else sz=(UINTN)(((ASLRF_L_HDR*)p8)->Length+sizeof(ASLRF_L_HDR));
        
        Status=AppendItemLst(AmiResLst, p8);
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) return Status;
        
        p8+=sz;
    }
    return Status;
}

/**
    This is a routine assigns resources to the SIO LD
	It is standard function from EFI_SIO_PROTOCOL 

    @param This Pointer to the EFI_SIO_PROTOCOL instance
    @param ResourceList Pointer to the EFI_SIO resource list
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 

EFI_STATUS EFIAPI EfiSioSetResources(
  IN CONST  EFI_SIO_PROTOCOL        *This,
  IN        ACPI_RESOURCE_HEADER_PTR ResourceList)
{
    EFI_STATUS      Status;
    EFI_SIO_DATA    *EfiSioData=(EFI_SIO_DATA*)This;
    T_ITEM_LIST		*ResLst=NULL; //if NULL new Buffer will be allocated...
//---------------------
	if(This==NULL || ResourceList.SmallHeader==NULL) return EFI_INVALID_PARAMETER;

    //Get Bubber for T_ITEM_LIST object...
    ResLst=(T_ITEM_LIST*)MallocZ(sizeof(T_ITEM_LIST));
    if(ResLst==NULL) return EFI_OUT_OF_RESOURCES;

    //Convert resources to AMI_SIO format.
    Status=EfiSioSetRes(ResLst, ResourceList);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;
    
	//Submit converted resources.
    return AmiSioCRS(&EfiSioData->Owner->AmiSio, TRUE, &ResLst);
}


/**
    This is a helper function to convert AMI_SIO possible resource list to EFI_SIO format.

    @param AmiResLst Pointer to the AMI_SIO resource list
    @param EfiResLst Pointer to the EFI_SIO resource list
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI EfiSioPrs(T_ITEM_LIST *AmiPrsLst, ACPI_RESOURCE_HEADER_PTR  *EfiResLst){
    UINTN                       i,j, sz=0;
    ASLRF_S_HDR                 *pRes;
    EFI_ASL_DepFn               *pPrs;
    VOID                        *EfiSioPrs=NULL;       
    UINT8                       *p8;
//----------------
    for(i=0; i<AmiPrsLst->ItemCount; i++){
        pPrs=(EFI_ASL_DepFn*)AmiPrsLst->Items[i];
        pRes=pPrs->DepFn;
        sz+=pRes->Length+sizeof(ASLRF_S_HDR);
        
        for(j=0; j<pPrs->DepRes.ItemCount; j++){
            pRes=(ASLRF_S_HDR*)pPrs->DepRes.Items[j];
            if(pRes->Type==ASLV_SMALL_RES) sz+=(UINTN)(pRes->Length+sizeof(ASLRF_S_HDR));
            else sz+=((ASLRF_L_HDR*)pRes)->Length+sizeof(ASLRF_L_HDR);
        }
    }

    //Allocate Buffer including EndDepFn object + Resource EndTag
    EfiSioPrs=Malloc(sz+sizeof(ASLR_EndTag)+sizeof(ASLV_RT_EndDependentFn));
    if(EfiSioPrs==NULL) return EFI_OUT_OF_RESOURCES;

    //Dump everything in buffer.
    for(i=0,p8=EfiSioPrs; i<AmiPrsLst->ItemCount; i++){
        pPrs=(EFI_ASL_DepFn*)AmiPrsLst->Items[i];
        //Copy StartDepFn Object...
        pRes=pPrs->DepFn;
        sz=pRes->Length+sizeof(ASLRF_S_HDR);
        MemCpy(p8,(VOID*)pRes, sz);        
        p8+=sz;

        for(j=0; j<pPrs->DepRes.ItemCount; j++){
            pRes=(ASLRF_S_HDR*)pPrs->DepRes.Items[j];
        
            if(pRes->Type==ASLV_SMALL_RES) sz=(UINTN)(pRes->Length+sizeof(ASLRF_S_HDR));
            else sz=(UINTN)(((ASLRF_L_HDR*)pRes)->Length+sizeof(ASLRF_L_HDR));
        
            MemCpy(p8,(VOID*)pRes, sz);        
            p8+=sz;        
        }
    }
    
    ((ASLRF_S_HDR*)p8)->Name=ASLV_RT_EndDependentFn;
    ((ASLRF_S_HDR*)p8)->Type=ASLV_SMALL_RES;
    ((ASLRF_S_HDR*)p8)->Length=sizeof(ASLV_RT_EndDependentFn);
    p8+=sizeof(ASLRF_S_HDR);

    ((ASLR_EndTag*)p8)->Hdr.HDR=ASLV_END_TAG_HDR;
	((ASLR_EndTag*)p8)->Chsum=0;
    
    EfiResLst->SmallHeader=EfiSioPrs;
    
    return EFI_SUCCESS;
}

/**
    This is a routine provides possible resource settings for the SIO LD
	It is standard function from EFI_SIO_PROTOCOL 

    @param This Pointer to the EFI_SIO_PROTOCOL instance
    @param ResourceCollection Pointer to the Resource collection buffer
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 

EFI_STATUS EFIAPI EfiSioPossibleResources(
  IN  CONST EFI_SIO_PROTOCOL         *This,
  OUT       ACPI_RESOURCE_HEADER_PTR *ResourceCollection)
{
    EFI_STATUS      Status;
    EFI_SIO_DATA    *EfiSioData=(EFI_SIO_DATA*)This;
    T_ITEM_LIST		*ResLst=NULL; //if NULL new Buffer will be allocated...
//----------------------------------
	if(This==NULL || ResourceCollection==NULL) return EFI_INVALID_PARAMETER;

    //Get PRS in AMI SIO format...
	Status=AmiSioPRS(&EfiSioData->Owner->AmiSio, FALSE, &ResLst);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    //Convert it to EFI_SUPER_IO format.
    Status=EfiSioPrs(ResLst, ResourceCollection);
    ASSERT_EFI_ERROR(Status);

    return Status;
}

/**
    This is a routine provides read modify write operation for the SIO LD register
    or registers group.
	It is standard function from EFI_SIO_PROTOCOL 

    @param This Pointer to the EFI_SIO_PROTOCOL instance
    @param Command Pointer to the Register/Data/Mask information array
    @param NumberOfCommands Number of elements in "Command" array.
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI EfiSioModify(
  IN CONST EFI_SIO_PROTOCOL         *This,
  IN CONST EFI_SIO_REGISTER_MODIFY  *Command,
  IN       UINTN                    NumberOfCommands)
{
    EFI_STATUS      Status;
    EFI_SIO_DATA    *EfiSioData=(EFI_SIO_DATA*)This;
    UINTN           i;
    UINT8           val;
    BOOLEAN         ExitCfg;
//----------------------------------
	if(This==NULL || Command==NULL) return EFI_INVALID_PARAMETER;
    
    if(NumberOfCommands==0) return EFI_SUCCESS;
    
    for(i=0; i<NumberOfCommands; i++){
        if(i<NumberOfCommands-1)ExitCfg=FALSE;
        else ExitCfg=TRUE;
        
        //Read Register
        Status=AmiSioRegister(&EfiSioData->Owner->AmiSio, FALSE, ExitCfg,
                              Command->Register, &val);
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) return Status;

        val&=Command->AndMask;
        val|=Command->OrMask;

        Status=AmiSioRegister(&EfiSioData->Owner->AmiSio, TRUE, ExitCfg,
                              Command->Register, &val);
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) return Status;

    }    
    return Status;
}

/**
    This is a routine provides possible resource settings for the SIO LD
	It is standard function from AMI_SIO_PROTOCOL 

    @param This Pointer to the AMI_SIO_PROTOCOL instance
    @param Set Action Selector
    @param Resources Pointer to the Resource collection buffer
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 

EFI_STATUS EFIAPI AmiSioPRS(IN AMI_SIO_PROTOCOL	*This,
				    IN BOOLEAN 				Set,
					IN OUT T_ITEM_LIST		**Resources)
{
	SIO_DEV2		*dev=(SIO_DEV2*)This;	
//---------------------------------------
	if(Set) return EFI_UNSUPPORTED;
	CopyItemLst(&dev->PRS, Resources); 
//	*Resources=&dev->PRS;	
	return EFI_SUCCESS;
}


/**
    Routine to record SIO transition to/from Config Mode into S3 Boot Script.

    @param Sio Pointer to the private SIO data structure
    @param Get Action Selector
    @param SaveState Pointer to EFI_S3_SAVE_STATE_PROTOCOL instance
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/
/*
VOID BootScriptSioCfgMode(GSIO2 *Sio, BOOLEAN Enter, EFI_S3_SAVE_STATE_PROTOCOL *SaveState)
{
	UINTN				i;
	SIO_SCRIPT_LST2		*sl;

//---------------------------------
	if(Enter)sl=Sio->EnterCfgMode;
	else sl=Sio->ExitCfgMode;

    if(sl==NULL) return;

	for (i=0; i<sl->InstrCount; i++){
		switch (sl->OpType){
			case cfNone: 
				break;
			case cfByteSeq:
			{
				SPIO_SCRIPT  *Instr = (SPIO_SCRIPT*) &sl->Instruction[0];
				SPIO_SCRIPT	*cmd = &Instr[i];
				UINT16		reg;
			//------------------------
				if(cmd->IdxDat)reg=Sio->SpioSdlInfo->SioIndex;
				else reg=Sio->SpioSdlInfo->SioData;
				
				if (cmd->WrRd) {
					BOOT_SCRIPT_S3_IO_WRITE_MACRO(SaveState,
						EfiBootScriptWidthUint8,
						reg, 1, &cmd->Value
					);
				} else {
					//This waits until SIO provides a specific value.
					//This unsupported by boot script Io read/writes.

					//<markw>I know of no SIO that needs this.
					//If needed, SIO S3 resume must be implemented differently
					// than the normal boot script.
					SIO_TRACE((TRACE_SIO,"GSIO: Reading Index/Data SIO registers not supported entering/exit in for S3 resume.\n"));
					ASSERT_EFI_ERROR(EFI_UNSUPPORTED);
				}
				break;
			}
			case cfRoutine:
			{
				//Implementing a generic routine like this is difficult using boot scripts in this
				//SIO implementation.
				//Use cfByteSeq.
				//SIO_TRACE((TRACE_SIO,"GSIO: cfRoutine in SIO not supported for S3 resume. Use cfByteSeq for enter/exit config space in IOx.c.\n"));
				//ASSERT_EFI_ERROR(EFI_UNSUPPORTED);
				UINT32	*FunctionNo = (UINT32*)&sl->Instruction[0];
				LaunchCfgModeRoutine(Sio,*FunctionNo, TRUE);
			}
				break;
			default: return;
		}//switch
	}//for
	Sio->InCfgMode=Enter;
	return;
}
*/

/**
    Routine to record SIO LD selection into S3 Boot Script.

    @param Dev Pointer to the private SIO LD data structure
    @param SaveState Pointer to EFI_S3_SAVE_STATE_PROTOCOL instance
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/
/*
VOID BootScriptSioDevSelect(SIO_DEV2 *Dev, EFI_S3_SAVE_STATE_PROTOCOL *SaveState){

    //in case of non Standard SIO w/o Device Select Register specified just return
	if((!Dev->Owner->SpioSdlInfo->StdSioDevice) && (Dev->Owner->SpioSdlInfo->DevSelReg==0)) return;
    
	BOOT_SCRIPT_S3_IO_WRITE_MACRO(SaveState,
			EfiBootScriptWidthUint8,
			Dev->Owner->SpioSdlInfo->SioIndex, 1, &Dev->Owner->SpioSdlInfo->DevSelReg);

	BOOT_SCRIPT_S3_IO_WRITE_MACRO(SaveState,
			EfiBootScriptWidthUint8,
			Dev->Owner->SpioSdlInfo->SioData, 1, &Dev->DeviceInfo->Ldn);
}
*/
/**
    This is SIO Ready to boot event handler

    @param Event AcpiSupportProtocol installed Event Handle
    @param Context Pointer to the optional Data buffer
	
    @retval VOID
**/ 
VOID EFIAPI CallbackReadyToBoot(IN EFI_EVENT Event,	IN VOID	*Context)
{
    EFI_GUID                        SmiReadyToBootCommunicateGuid=SMI_READY_TO_BOOT_COMMUNICATE_GUID;
    EFI_SMM_COMMUNICATION_PROTOCOL *SmmCommProtocol = NULL;
//    EFI_SMM_COMMUNICATE_HEADER      CommHdr;
    EFI_STATUS                      Status;
    UINTN                           CommSz=sizeof(EFI_SMM_COMMUNICATE_HEADER);
//----------------------------------
	SIO_TRACE((TRACE_SIO, "GenericSio: -> [[ CallbackReadyToBoot() Start... ]]\n"));

	
	Status = pBS->LocateProtocol( &gEfiSmmCommunicationProtocolGuid,
            NULL,
            &SmmCommProtocol );
	ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return;
    
    //Prepare Communication Header
    CopyGuid(&(gSmmCommunicationBuffer->HeaderGuid),&SmiReadyToBootCommunicateGuid);
    gSmmCommunicationBuffer->MessageLength=0;
    gSmmCommunicationBuffer->Data[0]=0;
	
	Status=SmmCommProtocol->Communicate(SmmCommProtocol,gSmmCommunicationBuffer, &CommSz);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return;
	
	pBS->CloseEvent(Event);

}

/**
    This routine will update SIO_STATUS_VAR according to the SOP LDs statuses.

    @param VOID
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI CreateSioDevStatusVar()  
{
  UINTN SioDevStatusVarSize = sizeof(SIO_DEV_STATUS);
	UINT32 SioDevStatusVarAttributes = 0;
	SIO_DEV_STATUS 	SioDevStatusVar;
	UINTN 			i,j;
	BOOLEAN 		Implemented; 
	UINT32			UID=0; 
	EFI_STATUS 		Status;
	GSIO2			*spio;
//--------------------------------------
	Status = pRS->GetVariable(SIO_DEV_STATUS_VAR_NAME, &gSioDevStatusVarGuid, 
														&SioDevStatusVarAttributes, 
														&SioDevStatusVarSize, &SioDevStatusVar.DEV_STATUS);
	if (EFI_ERROR(Status)) {
		SioDevStatusVar.DEV_STATUS = 0;
		SioDevStatusVarAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
	}	

	for (i = 0; i < gSpioCount; i++) {
		spio=&gSpio[i];
		for(j=0;j<spio->LdCount;j++){
			Implemented = (spio->DeviceList[j]->DeviceInfo->Implemented && spio->DeviceList[j]->NvData.DevEnable);
			UID = spio->DeviceList[j]->DeviceInfo->Uid;
			switch (spio->DeviceList[j]->DeviceInfo->Type){
				case dsFDC:		SioDevStatusVar.Fdd = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] FDC is Implemented=%d\n",i,j,Implemented));
					break;
				case dsPS2CK:
				case dsPS2K:	SioDevStatusVar.Key60_64 = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] PS2K is Implemented=%d\n",i,j,Implemented));
					break;
				case dsPS2CM:
				case dsPS2M: 	SioDevStatusVar.Ps2Mouse = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] PS2M is Implemented=%d\n",i,j,Implemented));
					break;
				case dsUART:	
					if (UID == 0) SioDevStatusVar.SerialA = Implemented;
					else 
						if (UID == 1) SioDevStatusVar.SerialB = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] UART(UID=%x) is Implemented=%d\n",i,j,UID,Implemented));
					break;
				case dsLPT:		SioDevStatusVar.Lpt = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] LPT is Implemented=%d\n",i,j,Implemented));
					break;
				case dsGAME:	
					if (UID == 0)SioDevStatusVar.Game1 = Implemented;
					else 
						if (UID == 1)SioDevStatusVar.Game2 = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] GAME(UID=%x) is Implemented=%d\n",i,j,UID,Implemented));
					break;
				case dsSB16:	SioDevStatusVar.Sb16 = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] SB16 is Implemented=%d\n",i,j,Implemented));
					break;
				case dsFmSynth: SioDevStatusVar.FmSynth = Implemented;
	                SIO_TRACE((TRACE_SIO,"SIO[%d]Dev[%d] FmSynth is Implemented=%d\n",i,j,Implemented));
					break;
				case dsMPU401:	
				case dsCIR:	
				case dsGPIO:	
				case dsHwMon:	
				case dsPME:	
				case dsACPI:	
					break;
			}//switch
		} //for j
	}//for i
	Status = pRS->SetVariable(SIO_DEV_STATUS_VAR_NAME, &gSioDevStatusVarGuid, 
														SioDevStatusVarAttributes,
														SioDevStatusVarSize, &SioDevStatusVar);
	SIO_TRACE((TRACE_SIO,"GSIO: DevStatusVar=%X Status=%r\n",SioDevStatusVar.DEV_STATUS,Status));
	return Status; 
}

/**
    This routine will disable SIO LD referenced as "Dev" in SIO_STATUS_VAR

    @param Dev Pointer to the private LD Data
	
    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends
**/ 
EFI_STATUS EFIAPI DisableDevInSioDevStatusVar(SIO_DEV2 *Dev)
{
	UINTN SioDevStatusVarSize = sizeof(SIO_DEV_STATUS);
	UINT32 SioDevStatusVarAttributes = 0;
	SIO_DEV_STATUS SioDevStatusVar; 
	EFI_STATUS Status; 
	SIO_DEV_TYPE Type = Dev->DeviceInfo->Type;
	SIO_DEV_TYPE UID = Dev->EisaId.UID;
//----------------------
	Status = pRS->GetVariable(SIO_DEV_STATUS_VAR_NAME, &gSioDevStatusVarGuid, 
														&SioDevStatusVarAttributes, 
														&SioDevStatusVarSize, &SioDevStatusVar.DEV_STATUS);
	if (!EFI_ERROR(Status)) {	
		switch (Type) {
			case dsFDC:	SioDevStatusVar.Fdd = 0;
									break;
			case dsPS2CK:
			case dsPS2K:	SioDevStatusVar.Key60_64 = 0;
										break;
			case dsPS2CM:
			case dsPS2M: 	SioDevStatusVar.Ps2Mouse = 0;
										break;
			case dsUART:	if (UID == 0) { 	
											SioDevStatusVar.SerialA = 0;
										} else if (UID == 1) {
											SioDevStatusVar.SerialB = 0;
										}
										break;
			case dsLPT:		SioDevStatusVar.Lpt = 0;
										break;
			case dsGAME:	if (UID == 0) { 	
											SioDevStatusVar.Game1 = 0;
										} else if (UID == 1) {
											SioDevStatusVar.Game2 = 0;
										}			
										break;
			case dsSB16:	SioDevStatusVar.Sb16 = 0;
										break;
			case dsFmSynth: SioDevStatusVar.FmSynth = 0;
											break;
			case dsMPU401:	case dsCIR:	case dsGPIO:	
			case dsHwMon:	case dsPME:	case dsACPI:	break;
	
		} //switch

		Status = pRS->SetVariable(SIO_DEV_STATUS_VAR_NAME, &gSioDevStatusVarGuid, 
															SioDevStatusVarAttributes,
															SioDevStatusVarSize, &SioDevStatusVar);
	} 
	SIO_TRACE((TRACE_SIO,"GSIO: DevStatusVar=%X\n",SioDevStatusVar.DEV_STATUS));
	return Status; 
}

/**
    This function is a Standard UEFI Driver Entry point.

        
    @param ImageHandle Image of this driver. 
    @param SystemTable UEFI System Table Pointer.

    @retval EFI_SUCCESS if initialization is DONE EFI_ERROR otherwise.
**/ 
EFI_STATUS EFIAPI GenericSioEntryPoint(
    IN EFI_HANDLE		ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable )
{
  	EFI_STATUS		Status=EFI_SUCCESS;
//--------------------------------------------------------------------
	InitAmiLib(ImageHandle, SystemTable);

	gDxeSvcTbl=(DXE_SERVICES*)GetEfiConfigurationTable(SystemTable,&gEfiDxeServicesTableGuid);

	//Fill ImgHandle Field in Driver Binding Protocol Structure
	gAmiSioDriverBinding.ImageHandle=ImageHandle;
	gAmiSioDriverBinding.DriverBindingHandle=NULL;

	//Here we try to find a Variable for IRQ and DMA mask 
	Status = AmiIsaIrqMask(&gAvailableIsaIrq, TRUE);
	//if Variable does not exists we create one
	if(EFI_ERROR(Status)){
		if(Status==EFI_NOT_FOUND) {
			gAvailableIsaIrq=ISA_IRQ_MASK;
		  	Status=AmiIsaIrqMask(&gAvailableIsaIrq, FALSE);
			if(EFI_ERROR(Status))SIO_TRACE((TRACE_SIO, "GSIO: Unexpected Error while creating Var ISA_IRQ_MASK. STATUS = %r\n", Status));
			else mVarWrite=TRUE;
		} else SIO_TRACE((TRACE_SIO, "GSIO: Unexpected Error while getting var ISA_IRQ_MASK. STATUS = %r\n", Status));
	} else mVarWrite=TRUE;

	Status = AmiIsaDmaMask(&gAvailableDmaChnl, TRUE);
	//if Variable does not exists we create one
	if(EFI_ERROR(Status)){
		if(Status==EFI_NOT_FOUND) {
			gAvailableDmaChnl=ISA_DMA_MASK;
		  	Status=AmiIsaDmaMask(&gAvailableDmaChnl, FALSE);
			if(EFI_ERROR(Status))SIO_TRACE((TRACE_SIO, "GSIO: Unexpected Error while creating Var ISA_DMA_MASK. STATUS = %r\n", Status));
		} else SIO_TRACE((TRACE_SIO, "GSIO: Unexpected Error while getting var ISA_DMA_MASK. STATUS = %r\n", Status));
	}

	//It might not be SIO in the project...
	Status=InitSioDataStructure(ImageHandle, SystemTable);
	if(EFI_ERROR(Status)){
		//If UNSUPPORTED - no SDL SIO data was found... possibly Legacy free system.
		if(Status!=EFI_UNSUPPORTED)	ASSERT_EFI_ERROR(Status);
		SIO_TRACE((TRACE_SIO,"GSIO: System Don't have any SIO Devices Ported...Exiting!\n"));
		return EFI_SUCCESS;
	}
	
#if ACPI_SUPPORT
	GetDsdt(NULL,NULL);						
	//if no ACPISupport Protocol Available yet - install Notify Event
	if(!gPDsdt) {
        Status = pBS->CreateEventEx (
                          EVT_NOTIFY_SIGNAL,
                          TPL_CALLBACK,
                          GetDsdt,
                          NULL,
                          &gEfiAcpiTableGuid,
                          &mAcpiEvent
                          );

		SIO_TRACE((TRACE_SIO,"GSIO: Locate ACPISupport FAILURE. Installing Callback (%r)\n", Status));
	} else SIO_TRACE((TRACE_SIO,"GSIO: Locate ACPISupport SUCCESS \n"));
	// If ACPI_SUPPORT is disabled, code for SIO port enumaration and Installing the protocol is not available
	// Adding the same here based on condition check.

#else
	{
		UINTN					i;
		//Set the Possible Resources for each implemented device
		for(i=0;i<gSpioCount;i++){

			SIO_TRACE((TRACE_SIO,"GSIO: (NO ACPI Mode) Enumerate SIO Chip # %d >>>>> \n",i));
			Status=SioEnumerateAll(&gSpio[i]);
			SIO_TRACE((TRACE_SIO,"GSIO: (NO ACPI Mode) EnumerateAll for SIO Chip # %d -> Status=%r\n\n",i, Status));
		}
	//install driver binding protocol here ....
		Status = pBS->InstallMultipleProtocolInterfaces(
					&gAmiSioDriverBinding.DriverBindingHandle,
					&gEfiDriverBindingProtocolGuid,&gAmiSioDriverBinding,
					&gEfiComponentName2ProtocolGuid, &gAmiSioComponentName,
					NULL,NULL);

	}
#endif	
	//Set the Callback for Variable Write
	if(!mVarWrite){
		Status=RegisterProtocolCallback(&gEfiVariableWriteArchProtocolGuid,UpdateIsaMask,NULL,&mIsaEvent,&mIsaReg);
		SIO_TRACE((TRACE_SIO,"GSIO: Variable Write FAILURE. Installing Callback (%r)\n", Status));	
	}


	//Create event for boot script
	Status = CreateReadyToBootEvent(
		TPL_NOTIFY,
		CallbackReadyToBoot,
		NULL,
		&gEvtReadyToBoot
	);
	ASSERT_EFI_ERROR(Status);


	//Here we can set up notification events if needed
	pBS->AllocatePool(EfiRuntimeServicesData, sizeof(EFI_SMM_COMMUNICATE_HEADER), &gSmmCommunicationBuffer);



	//------------------------------------
	return Status;

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
                                                                                                                    
