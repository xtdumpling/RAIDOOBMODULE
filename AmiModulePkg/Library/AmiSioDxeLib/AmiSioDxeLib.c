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
#include <AmiLib.h>
#include <AmiCspLib.h>
#include <AmiGenericSio.h>
#include <GenericSio.h>
#include <AcpiRes.h>
#include <Protocol/AmiSio.h>
#include <Protocol/PciIo.h>
#include <Protocol/S3SmmSaveState.h>
#include <Library/AmiSioDxeLib.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
ACPI_HDR 	*gDsdtPtr=NULL;

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: GetSioLdVarName
//
// Description:
//This function generate Variable Name associated with each SIO Logical Device.
//"Volatile" or None "Volatile"
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STRING EFIAPI GetSioLdVarName(SIO_DEV2* SioLd, BOOLEAN Volatile){
	CHAR16 s[40];
	EFI_STRING	ret;
//------------------	
	if(Volatile){
#if (SIO_SETUP_USE_APTIO_4_STYLE==1)
		Swprintf(s, LD_VV_VAR_NAME_FORMAT, SioLd->DeviceInfo->PnpId, SioLd->DeviceInfo->Uid);
#else
		Swprintf(s, LD_VV_VAR_NAME_FORMAT, SioLd->Owner->SioInfoIdx, SioLd->DevInfoIdx);
#endif		
	} else {
#if (SIO_SETUP_USE_APTIO_4_STYLE==1)
		Swprintf(s, LD_NV_VAR_NAME_FORMAT, SioLd->DeviceInfo->PnpId, SioLd->DeviceInfo->Uid);
#else
		Swprintf(s, LD_NV_VAR_NAME_FORMAT, SioLd->Owner->SioInfoIdx, SioLd->DevInfoIdx);
#endif
	}
	
	ret=MallocZ(Wcslen(s)*sizeof(CHAR16)+sizeof(CHAR16));
	Wcscpy(ret, s);
	return ret;
}


/*
EFI_STATUS EFIAPI AmiSioDxeLibConstructor (IN EFI_HANDLE  ImageHandle, IN EFI_SYSTEM_TABLE  *SystemTable)
{
	EFI_STATUS				Status;
	EFI_PHYSICAL_ADDRESS	Addr;
//-------------------------------------------------
	InitAmiLib(ImageHandle, SystemTable);

	//Status=LibGetDsdt(&Addr,EFI_ACPI_TABLE_VERSION_ALL);

	//if(!EFI_ERROR(Status)) gDsdtPtr=(ACPI_HDR*)Addr;

	return EFI_SUCCESS;
}
*/

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: GetPrsFromTable
//
// Description:
//This function will get _PRS from mSpioDeviceList[] table
//"Index" is the entry # in mSpioDeviceList[]
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI GetPrsFromTable(SIO_DEV2* Dev, UINTN Index){
	AMI_SDL_LOGICAL_DEV_INFO	*de=Dev->DeviceInfo;//device entry
								//IO1 IO2  IRQ1 IRQ2 DMA1 DMA2
	VOID*						*rl[6]=	{NULL,NULL,NULL,NULL,NULL,NULL}; //resource list
	UINT16						i=0;
	T_ITEM_LIST					*dl;
//-----------------------------------
    SIO_TRACE((TRACE_SIO,"Dev[%d]: PRS Tbl=> \n", Index));

    if (Dev->DeviceInfo->Flags==SIO_NO_RES){
        SIO_TRACE((TRACE_SIO," Device NOT USING Resources FLAGS=0x%X\n", Dev->DeviceInfo->Flags));
        return EFI_SUCCESS;
    }

	i = de->ResLen[0] | de->ResLen[1] | de->IrqMask[0] | de->IrqMask[1] | de->DmaMask[0] | de->DmaMask[1];
	if(!i){
		SIO_TRACE((TRACE_SIO,"Entry is Empty! Check 'YourSioName_DevLst' table.\n"));
		return EFI_NOT_FOUND;
	}

	//Resource base and length 1
	rl[0]=ASLM_FixedIO(de->ResBase[0],de->ResLen[0]);
	if(!rl[0]) return EFI_OUT_OF_RESOURCES;
	SIO_TRACE((TRACE_SIO,"IO1.B=%X; IO1.L=%X; ",de->ResBase[0],de->ResLen[0]));

	//Resource base and length 2
	rl[1]=ASLM_FixedIO(de->ResBase[1],de->ResLen[1]);
	if(!rl[1]) return EFI_OUT_OF_RESOURCES;
	SIO_TRACE((TRACE_SIO,"IO2.B=%X IO2.L=%X; ",de->ResBase[1],de->ResLen[1]));

	//IRQ settings 1
	rl[2]=ASLM_IRQNoFlags(1,0);
	if(!rl[2]) return EFI_OUT_OF_RESOURCES;
	((ASLR_IRQNoFlags*)rl[2])->_INT=de->IrqMask[0];
	SIO_TRACE((TRACE_SIO,"IRQ1=%X; ",de->IrqMask[0]));

	//IRQ settings 2
	rl[3]=ASLM_IRQNoFlags(1,0);
	if(!rl[3]) return EFI_OUT_OF_RESOURCES;
	((ASLR_IRQNoFlags*)rl[3])->_INT=de->IrqMask[1];
	SIO_TRACE((TRACE_SIO,"IRQ2=%X; ",de->IrqMask[1]));

	//DMA settings 1
	rl[4]=ASLM_DMA(1,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 0);
	if(!rl[4]) return EFI_OUT_OF_RESOURCES;
	((ASLR_DMA*)rl[4])->_DMA=de->DmaMask[0];
	SIO_TRACE((TRACE_SIO,"DMA1=%X; ",de->DmaMask[0]));

	//DMA settings 2
	rl[5]=ASLM_DMA(1,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 0);
	if(!rl[5]) return EFI_OUT_OF_RESOURCES;
	((ASLR_DMA*)rl[5])->_DMA=de->DmaMask[1];
	SIO_TRACE((TRACE_SIO,"DMA2=%X;",de->DmaMask[1]));

    SIO_TRACE((TRACE_SIO,"\n"));

	//Now Create _PRS Object for this device with best priorities
	dl=ASLM_StartDependentFn(6,0,0,rl[0],rl[1],rl[2],rl[3],rl[4],rl[5]);
	if(!dl) return EFI_OUT_OF_RESOURCES;

	SIO_TRACE((TRACE_SIO,"DepFn=%X,ItemCount=%d; ",(EFI_ASL_DepFn*)dl,((EFI_ASL_DepFn*)dl)->DepRes.ItemCount));
    for(i=0;i<6;i++) {
    	SIO_TRACE((TRACE_SIO,"Item[%d]=%X  ",i,((EFI_ASL_DepFn*)dl)->DepRes.Items[i]));
    }
	SIO_TRACE((TRACE_SIO,"\n <= Dev[%d]: PRS Tbl End \n",Index));

	return AppendItemLst(&Dev->PRS, dl);
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure: Set***Prs
//
// Description:
//These functions will collect _PRS from legacy concept,it is not support ACPI
//
// Input:
//
// Output:
//
// Notes:
//------------------------------------------------------------------------- 
// <AMI_PHDR_END> 
//Create Possible Resource Settings Buffer for FDC
EFI_STATUS EFIAPI SioDxeLibSetFdcPrs(SIO_DEV2 *Dev)
{
	EFI_STATUS			Status;
	T_ITEM_LIST 		*dl;
//-----------------------------------
//	StartDependentFn(0, 0) {
//			IO(Decode16, 0x3F0, 0x3F0, 1, 8)
//			IRQNoFlags() {6}
//			DMA(Compatibility, NotBusMaster, Transfer8) {2}
//		} //Same as
//						sdc_cnt,pComp,pPerf
	dl=ASLM_StartDependentFn(4,    0,   0,
			ASLM_FixedIO(0x3F0, 6), //0x3F6 port reserved for Legacy IDE
			ASLM_FixedIO(0x3F7, 1),
			ASLM_IRQNoFlags(1,6),
			ASLM_DMA(1,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 2)
			);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

//	StartDependentFnNoPri() {
//			IO(Decode16, 0x3F0, 0x3F0, 1, 6)
//			IRQNoFlags() {3,4,5,6,7,10,11,12}
//			DMA(Compatibility, NotBusMaster, Transfer8) {0,1,2,3}
//		} same as
	dl=ASLM_StartDependentFnNoPri(4,
		ASLM_FixedIO(0x3F0,6), //0x3F6 port reserved for Legacy IDE
		ASLM_FixedIO(0x3F7,1),
		ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12),
		ASLM_DMA(4,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8,0,1,2,3)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;

	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

	dl=ASLM_StartDependentFnNoPri( 3,
			ASLM_FixedIO(0x370, 6), //0x376 port reserved for Legacy IDE
			ASLM_FixedIO(0x377, 1),
			ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12),
			ASLM_DMA(4,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8,0,1,2,3)
			);
	if(!dl) return EFI_OUT_OF_RESOURCES;

	Status=AppendItemLst(&Dev->PRS, dl);
	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure: Set***Prs
//
// Description:
//These functions will collect _PRS from legacy concept,it is not support ACPI
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
//Create Possible Resource Settings Buffer for PS2 Keyboard
EFI_STATUS EFIAPI SioDxeLibSetPs2kPrs(SIO_DEV2 *Dev)
{
	EFI_STATUS			Status=0;
	T_ITEM_LIST 		*dl;
//-----------------------------------
	if(Dev->DeviceInfo->Type==dsPS2CM){
		//Just in case overwrite porting settings
		//Dev->DeviceInfo->Flags=SIO_SHR_ALL;
		dl=ASLM_StartDependentFnNoPri( 4,
			ASLM_FixedIO(0x60, 1),
			ASLM_FixedIO(0x64, 1),
			ASLM_IRQNoFlags(1, 1),
			ASLM_IRQNoFlags(1, 12) );
	} else {
		Dev->DeviceInfo->Flags=SIO_SHR_IO;
		dl=ASLM_StartDependentFnNoPri( 3,
			ASLM_FixedIO(0x60, 1),
			ASLM_FixedIO(0x64, 1),
			ASLM_IRQNoFlags(1, 1) );
	}
	if(!dl) return EFI_OUT_OF_RESOURCES;

	Status=AppendItemLst(&Dev->PRS, dl);
	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure: Set***Prs
//
// Description:
//These functions will collect _PRS from legacy concept,it is not support ACPI
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
//Create Possible Resource Settings Buffer for PS2 mouse
EFI_STATUS EFIAPI SioDxeLibSetPs2mPrs(SIO_DEV2 *Dev)
{
	EFI_STATUS			Status=0;
	T_ITEM_LIST 		*dl;
//-----------------------------------
	if(Dev->DeviceInfo->Type==dsPS2CM){
		//Just in case overwrite porting settings
		//Dev->DeviceInfo->Flags=SIO_SHR_ALL;
		dl=ASLM_StartDependentFnNoPri( 4,
			ASLM_FixedIO(0x60, 1),
			ASLM_FixedIO(0x64, 1),
			ASLM_IRQNoFlags(1, 1),
			ASLM_IRQNoFlags(1, 12) );
	} else {
		Dev->DeviceInfo->Flags=SIO_SHR_IO;
		dl=ASLM_StartDependentFnNoPri( 3,
			ASLM_FixedIO(0x60, 1),
			ASLM_FixedIO(0x64, 1),
			ASLM_IRQNoFlags(1, 12) );
	}
	if(!dl) return EFI_OUT_OF_RESOURCES;

	Status=AppendItemLst(&Dev->PRS, dl);
	return Status;
}

EFI_STATUS EFIAPI SioDxeLibSetUartPrs(SIO_DEV2 *Dev){
	EFI_STATUS			Status=0;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	T_ITEM_LIST			*dl = NULL;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
//-----------------------------------
	switch(Dev->EisaId.UID){
		case 0 : //priority for Com 1;
			dl=ASLM_StartDependentFn(2, 0,0,
				ASLM_FixedIO(0x3F8, 8),
				ASLM_IRQNoFlags(1,4)
			);
			break;
		case 1 : //priority for Com 2;
			dl=ASLM_StartDependentFn(2, 0,0,
				ASLM_FixedIO(0x2F8, 8),
				ASLM_IRQNoFlags(1,3)
			);
			break;
		case 2 : //priority for Com 3;
			dl=ASLM_StartDependentFn(2, 0,0,
				ASLM_FixedIO(0x3E8, 8),
				ASLM_IRQNoFlags(1,11)
			);
			break;

		case 3 : //priority for Com 4;
			dl=ASLM_StartDependentFn(2, 0,0,
				ASLM_FixedIO(0x2E8, 8),
				ASLM_IRQNoFlags(1,10)
			);
			break;
	}
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

	//the rest of resource combinations will go with No Priority
	dl=ASLM_StartDependentFnNoPri(2,
		ASLM_FixedIO(0x3F8, 8),
		ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

	dl=ASLM_StartDependentFnNoPri(2,
		ASLM_FixedIO(0x2F8, 8),
		ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

	dl=ASLM_StartDependentFnNoPri(2,
		ASLM_FixedIO(0x3E8, 8),
		ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

	dl=ASLM_StartDependentFnNoPri(2,
		ASLM_FixedIO(0x2E8, 8),
		ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);

	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure: Set***Prs
//
// Description:
//These functions will collect _PRS from legacy concept,it is not support ACPI
//
// Input:
//
// Output:
//
// Notes:
//------------------------------------------------------------------------- 
// <AMI_PHDR_END> 
EFI_STATUS EFIAPI SioDxeLibSetLptPrs(SIO_DEV2 *Dev, BOOLEAN UseDma){
	EFI_STATUS			Status=0;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
	T_ITEM_LIST			*dl = NULL;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
//-----------------------------------

	switch(Dev->EisaId.UID){
		case 0 : //priority for LPT 1;
			if(UseDma){
				dl=ASLM_StartDependentFn(4, 0,0,
					ASLM_FixedIO(0x378, 8),
					ASLM_FixedIO(0x778, 8),
					ASLM_IRQNoFlags(1,7),
					ASLM_DMA(1,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 3)
				);
			} else {
				dl=ASLM_StartDependentFn(2, 0,0,
					ASLM_FixedIO(0x378, 8),
					ASLM_IRQNoFlags(1,7)
				);
			}
			break;
		case 1 : //priority for LPT 2;
			if(UseDma){
				dl=ASLM_StartDependentFn(4, 0,0,
					ASLM_FixedIO(0x278, 8),
					ASLM_FixedIO(0x678, 8),
					ASLM_IRQNoFlags(1,5),
					ASLM_DMA(1,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 1)
				);
			} else {
				dl=ASLM_StartDependentFn(2, 0,0,
					ASLM_FixedIO(0x278, 8),
					ASLM_IRQNoFlags(1,5)
				);
			}
			break;
	}
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

	//the rest of resource combinations will go with No Priority
	if(UseDma){
		//IRQ
		dl=ASLM_StartDependentFnNoPri(4,
			ASLM_FixedIO(0x378, 8),
			ASLM_FixedIO(0x778, 8),
			ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12),
			ASLM_DMA(2,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 1, 3)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(4,
			ASLM_FixedIO(0x278, 8),
			ASLM_FixedIO(0x678, 8),
			ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12),
			ASLM_DMA(2,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 1, 3)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(4,
			ASLM_FixedIO(0x3BC, 4),
			ASLM_FixedIO(0x7BC, 4),
			ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12),
			ASLM_DMA(2,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 1, 3)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;
		//No IRQ
		dl=ASLM_StartDependentFnNoPri(3,
			ASLM_FixedIO(0x378, 8),
			ASLM_FixedIO(0x778, 8),
			ASLM_DMA(2,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 1, 3)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(3,
			ASLM_FixedIO(0x278, 8),
			ASLM_FixedIO(0x678, 8),
			ASLM_DMA(2,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 1, 3)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(3,
			ASLM_FixedIO(0x3BC, 4),
			ASLM_FixedIO(0x7BC, 4),
			ASLM_DMA(2,ASLV_Compatibility, ASLV_NotBusMaster, ASLV_Transfer8, 1, 3)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);

	} else { //No DMA MODE
		//IRQ
		dl=ASLM_StartDependentFnNoPri(2,
			ASLM_FixedIO(0x378, 8),
			ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(2,
			ASLM_FixedIO(0x278, 8),
			ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(2,
			ASLM_FixedIO(0x3BC, 4),
			ASLM_IRQNoFlags(7, 3,4,5,6,10,11,12)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;
		//No IRQ
		dl=ASLM_StartDependentFnNoPri(1,
			ASLM_FixedIO(0x378, 8)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(1,
			ASLM_FixedIO(0x278, 8)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
		if(EFI_ERROR(Status)) return Status;

		dl=ASLM_StartDependentFnNoPri(1,
			ASLM_FixedIO(0x3BC, 4)
		);
		if(!dl) return EFI_OUT_OF_RESOURCES;
		Status=AppendItemLst(&Dev->PRS, dl);
	}
	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure: Set***Prs
//
// Description:
//These functions will collect _PRS from legacy concept,it is not support ACPI
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SioDxeLibSetGamePrs(SIO_DEV2 *Dev){
	EFI_STATUS			Status=0;
	T_ITEM_LIST 		*dl;
//-----------------------------------

	dl=ASLM_StartDependentFnNoPri(1,
		ASLM_FixedIO(0x200, 8)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);

	return Status;
}

EFI_STATUS EFIAPI SioDxeLibSetMpu401Prs(SIO_DEV2 *Dev){
	EFI_STATUS		Status=0;
	T_ITEM_LIST 	*dl;
//-----------------------------------


	//the rest of resource combinations will go with No Priority
	dl=ASLM_StartDependentFnNoPri(2,
		ASLM_FixedIO(0x300, 2),
		ASLM_IRQNoFlags(5, 5,7,9,10,11)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	if(EFI_ERROR(Status)) return Status;

	dl=ASLM_StartDependentFnNoPri(2,
		ASLM_FixedIO(0x330, 2),
		ASLM_IRQNoFlags(5, 5,7,9,10,11)
	);
	if(!dl) return EFI_OUT_OF_RESOURCES;
	Status=AppendItemLst(&Dev->PRS, dl);
	return Status;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: SioDxeLibGetPrsFromAml
//
// Description:
//These function will collect _PRS from Aml code which support ACPI
//"Index" is the entry # in mSpioDeviceList[]
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SioDxeLibGetPrsFromAml(SIO_DEV2* Dev, UINT8 *PrsName ,UINTN Index){
	EFI_STATUS					Status=0;
	AMI_SDL_LOGICAL_DEV_INFO 	*de=Dev->DeviceInfo;
	ASL_OBJ_INFO				obj={0}, prs={0};
	UINT8						bc, *p;
	UINTN						bl;
	ASLRF_S_HDR					*rd;
	EFI_ASL_DepFn				*pdf=NULL;
//---------------------------
	if(de->AslName[0]==0) return GetPrsFromTable(Dev, Index);

	if(gDsdtPtr==NULL){
		EFI_PHYSICAL_ADDRESS	addr;
		Status=LibGetDsdt(&addr,EFI_ACPI_TABLE_VERSION_ALL);
		if(!EFI_ERROR(Status)) gDsdtPtr=(ACPI_HDR*)addr;
		else {
			ASSERT_EFI_ERROR(Status);
			return Status;
		}
	}

	Status=GetAslObj((UINT8*)gDsdtPtr+sizeof(ACPI_HDR),gDsdtPtr->Length-sizeof(ACPI_HDR)-1,&de->AslName[0],otDevice,&obj);
	if(EFI_ERROR(Status)) {
		SIO_TRACE((TRACE_SIO,"SIO[%d]: Aml=> Failed to Locate ASL Object %s in DSDT at %08X\n",Index,&de->AslName[0], gDsdtPtr));
		return GetPrsFromTable(Dev, Index);
	}

	//it must be _PRS name Object there
	if(!PrsName) Status = GetAslObj(obj.DataStart,obj.Length,"_PRS",otName,&prs);
	else Status = GetAslObj(obj.DataStart,obj.Length,PrsName,otName,&prs);
	if(EFI_ERROR(Status)) {
		SIO_TRACE((TRACE_SIO,"SIO[%d]: Aml=> Failed to Locate _PRS Object in %s Object Scope at %08X\n",Index, &de->AslName[0], obj.Object));
		return GetPrsFromTable(Dev, Index);
	}

	p=prs.DataStart;
	if(*p==AML_OP_BUFFER){
		UINT8	*prsb;
	//---------------------
		GetPackageLen((AML_PACKAGE_LBYTE*)(p+1),&bc);
		//_PRS Name must be a "Buffer"
		//DefBuffer := BufferOp PkgLength BufferSize ByteList
		//BufferSize := TermArg=>Integer
		//DataObject := (in our case ComputationalData) | DefPackage | DefVarPackage
		p+=bc+2;
		switch(*p){
			case AML_PR_BYTE:
				bl=*(p+1);
				prsb=p+2;
				break;
			case AML_PR_WORD:
				bl=*((UINT16*)(p+1));
				prsb=p+3;
				break;
			case AML_PR_DWORD:
				bl=*((UINT32*)(p+1));
				prsb=p+5;
				break;
			default: return EFI_INVALID_PARAMETER;
		}
		//Now Copy _PRS Buffer into new location and fill SIO.PRS T_ITEM_LIST
		p=Malloc(bl);
		if(!p) return EFI_OUT_OF_RESOURCES;
		MemCpy(p,prsb,bl);
		//Parse AML _PRS Buffer
		rd=(ASLRF_S_HDR*)p;
		prsb=p;
		while(rd->Name!=ASLV_SR_EndTag){
		//------------------------------
			if(rd->Name==ASLV_RT_StartDependentFn){
				//Add this item to device PRS list
				pdf=MallocZ(sizeof(EFI_ASL_DepFn));
				if(!pdf) return EFI_OUT_OF_RESOURCES;
				pdf->DepFn=rd;
				Status=AppendItemLst(&Dev->PRS, (VOID*)pdf);
				if(EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
			} else {
				Status=AppendItemLst(&pdf->DepRes,rd);
				if(EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
			}
			//advance to the next descriptor in _PRS ResourceTemplate
			p+=sizeof(ASLRF_S_HDR)+rd->Length; //rd->Length => Byte length excluding length of the header
			//It could be only IO, FixedIO, IRQ and DMA types of resources so it is small resource descriptor type
			if((prsb+bl)<p){ //check if we still inside _PRS buffer or went out..
				SIO_TRACE((TRACE_SIO,"SIO[%d]: Aml=> Failed to Locate EndDepFn in _PRS buffer of %s Object at %08X\n",Index ,&de->AslName[0],prs.Object));
				return GetPrsFromTable(Dev, Index);
			}
			rd=(ASLRF_S_HDR*)p;
			if(rd->Name==ASLV_RT_EndDependentFn) break;
		} //while
		if(!Dev->PRS.ItemCount){
			SIO_TRACE((TRACE_SIO,"SIO[%d]: Aml=> Failed to Locate StartDepFn in _PRS buffer of %s Object at %08X\n",Index,&de->AslName[0],prs.Object));
			return GetPrsFromTable(Dev, Index);
		}
		SIO_TRACE((TRACE_SIO,"SIO[%d]: Aml=> Collected %d DepFunc Items of %s Object \n",Index,Dev->PRS.ItemCount,&de->AslName[0]));
	} else {
		SIO_TRACE((TRACE_SIO,"SIO[%d]: Aml=> _PRS Object of %s Object is not a Buffer Type at %08X\n",Index,&de->AslName[0],prs.Object));
		return GetPrsFromTable(Dev, Index);
	}
	return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: LaunchCompRoutine
//
// Description:
//  This is a routine to launch Init Function from Compatibility Module.
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SioLibLaunchCompRoutine(SIO_DEV2 *Device, SIO_INIT_STEP InitStep)
{
	SPIO_DEV_LST	*CompDevInfo;
	EFI_STATUS		Status=EFI_SUCCESS;
//------------------------
	
	CompDevInfo=(SPIO_DEV_LST*)Device->DeviceInfo;
	
	if(CompDevInfo->InitRoutine!=NULL){
		SIO_TRACE((TRACE_SIO,"GSIO: Compatible InitRoutine(is = %d) ... ",InitStep)); 
		Status=CompDevInfo->InitRoutine(&Device->AmiSio,Device->Owner->IsaBrgPciIo,InitStep);
		SIO_TRACE((TRACE_SIO,"Status=%r.\n", Status));
		ASSERT_EFI_ERROR(Status);
	}
	
	return Status;
}


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: SioLibLaunchInitRoutine
//
// Description:
//  This is a routine to launch Init Function
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SioLibLaunchInitRoutine(SIO_DEV2 *Device, SIO_INIT_STEP InitStep)
{
	AMI_BOARD_INIT_PARAMETER_BLOCK 	Parameters;
	EFI_STATUS						Status;
	VOID							*AmiSio=Device;
	VOID							*LpcPciIo=Device->Owner->IsaBrgPciIo;
	AMI_BOARD_INIT_PROTOCOL			*InitProtocol=Device->Owner->SioInitProtocol;
	UINTN							InitFunction=Device->DeviceInfo->InitRoutine;
//------------------------
	
	if(Device->DeviceInfo->InitRoutine==0) return EFI_SUCCESS;
		
	if(InitProtocol==NULL){
		SIO_TRACE((TRACE_SIO,"GSIO: InitProtocol Not Present! \n"));
		return EFI_SUCCESS;
	}
	
	if(InitProtocol->FunctionCount < InitFunction) {
		SIO_TRACE((TRACE_SIO,"GSIO: InitProtocol->FunctionCount(%d) < InitFunction (%d)!\n", InitProtocol->FunctionCount, InitFunction));
		return EFI_INVALID_PARAMETER;
	}

	Parameters.Signature=AMI_SIO_PARAM_SIG;
	Parameters.InitStep=InitStep;
	Parameters.Param1=AmiSio;
	Parameters.Param2=LpcPciIo;
	Parameters.Param3=NULL;
	Parameters.Param4=NULL;

	SIO_TRACE((TRACE_SIO,"\nGSIO: Calling InitProtocol->Function[%d](InitStep=%d, AmiSio=%X, LpcPciIo=%X)...",
			InitFunction, InitStep, AmiSio, LpcPciIo));
	Status=InitProtocol->Functions[InitFunction](InitProtocol,&InitFunction, &Parameters);
	SIO_TRACE((TRACE_SIO,"%r\n",Status));

	ASSERT_EFI_ERROR(Status);

	return Status;
}

EFI_STATUS EFIAPI SioLibLaunchResourceRoutine(SIO_DEV2 *Device, SIO_RESOURCE_REG_TYPE RegType)
{
    AMI_BOARD_INIT_PARAMETER_BLOCK  Parameters;
    EFI_STATUS                      Status;
    VOID                            *AmiSio=Device;
    VOID                            *LpcPciIo=Device->Owner->IsaBrgPciIo;
    AMI_BOARD_INIT_PROTOCOL         *InitProtocol=Device->Owner->SioInitProtocol;
    UINTN                           InitFunction=Device->DeviceInfo->InitRoutine;
//--------------------------------------------    
    if(Device->DeviceInfo->InitRoutine==0) return EFI_SUCCESS;
        
    if(InitProtocol==NULL){
        SIO_TRACE((TRACE_SIO,"GSIO: InitProtocol Not Present! \n"));
        return EFI_SUCCESS;
    }
    
    if(InitProtocol->FunctionCount < InitFunction) {
        SIO_TRACE((TRACE_SIO,"GSIO: InitProtocol->FunctionCount(%d) < InitFunction (%d)!\n", InitProtocol->FunctionCount, InitFunction));
        return EFI_INVALID_PARAMETER;
    }

    Parameters.Signature=AMI_SIO_PARAM_SIG;
    Parameters.InitStep=isSetResources;
    Parameters.Param1=AmiSio;
    Parameters.Param2=LpcPciIo;
    Parameters.Param3=&RegType;
    Parameters.Param4=NULL;

    SIO_TRACE((TRACE_SIO,"\nGSIO: Calling InitProtocol->isSetResources(InitFunc=%d, AmiSio=%X, LpcPciIo=%X)...",
            InitFunction, AmiSio, LpcPciIo));
    Status=InitProtocol->Functions[InitFunction](InitProtocol,&InitFunction, &Parameters);
    SIO_TRACE((TRACE_SIO,"%r\n",Status));

    ASSERT_EFI_ERROR(Status);

    return Status;

}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: ProgramIsaRegisterTable
//
// Description:
//  This function will Program the SIO config space.
//
// Input:
//  IN  UINT16  Index - Index port
//  IN  UINT16  Data - Data port
//  IN  UINT16  *Table - initial table
//  IN  UINT8   Count - table data count
// Output:
//  NONE
//
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
void EFIAPI ProgramIsaRegisterTable(
    IN  UINT16  Index,
    IN  UINT16  Data,
    IN  SIO_DEVICE_INIT_DATA *Table,
    IN  UINT8   Count
)
{
    UINT8   i;
    UINT8   Value8;

    for(i=0;i<Count;i++) {
        //Some registers may not allow to read, and also we can write some registers without any read operations.
        if(Table[i].AndData8 == 0x00) Value8=Table[i].OrData8;
        else Value8 = (IoRead8(Data)& Table[i].AndData8)| Table[i].OrData8;

        //Isa register Offset
        IoWrite8(Index, (UINT8)(Table[i].Reg16));
		
        //write Isa register data
        IoWrite8(Data, Value8);
    }

    return;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: ProgramRtRegisterTable
//
// Description:
//  This function will program the runtime register.
//
// Input:
//  IN  UINT16  Base - Runtime register IO base
//  IN  SIO_DEVICE_INIT_DATA  *Table - initial table
//  IN  UINT8   Count - table data count
// Output:
//  NONE
//
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
void EFIAPI ProgramRtRegisterTable(
    IN  UINT16  Base,
    IN  SIO_DEVICE_INIT_DATA  *Table,
    IN  UINT8   Count
)
{
    UINT8   i;
    UINT8   Value8;

    for(i=0;i<Count;i++) {
	    //Some registers may not allow to read, and also we can write some registers without any read operations.
        if(Table[i].AndData8 == 0x00) Value8 = Table[i].OrData8;
        else Value8 = (IoRead8(Base+Table[i].Reg16) & Table[i].AndData8)| Table[i].OrData8;
        IoWrite8(Base+Table[i].Reg16, Value8 );
    }
}

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:     GetValueWithIO
//
// Description:
//  Get the register value form HWM space register.
//
// Input:
//	UINT16  IndexP   -> LPC index port
//	UINT16  DataP    -> LPC data port
//  UINT8   BankReg  -> Optional. If 0x00, skip programing bank number.
//  UINT8   BankNo   -> Bank number.
//  UINT8   LpcReg   -> Register who content the wanted value
//  UINTN   *Value   -> Register value
//
// Output:
//  UINTN   *Value
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
void EFIAPI GetValueWithIOx (
	IN UINT16 IndexP,
	IN UINT16 DataP,
	IN UINT8  BankReg,
    IN UINT8  BankNo,
    IN UINT8  LpcReg,
    OUT UINTN *Value
)
{
	//1. Program bank
	if(BankReg != 0x00){
		IoWrite8(IndexP, BankReg);
		IoWrite8(DataP, BankNo);
	}
	//2. Access Lpc register
    //Read the data from register
    IoWrite8(IndexP, LpcReg);
    *Value = IoRead8(DataP);

    return;
}

VOID EFIAPI SioLib_BootScriptIO(UINT16 Reg16, UINT8 Data8, EFI_S3_SAVE_STATE_PROTOCOL *SaveState)
{
    IoWrite8(Reg16,Data8);
    //BOOT_SCRIPT_S3_IO_WRITE_MACRO(SaveState,
    //                            EfiBootScriptWidthUint8,
    //                            Reg16,
    //                            1,
    //                            &Data8);
    SaveState->Write(SaveState,0x00,EfiBootScriptWidthUint8,(UINT64)Reg16,(UINTN)1,&Data8);
    
	return;
}

VOID EFIAPI SioLib_BootscriptLdnSel(
		UINT16 idxReg,
		UINT16 dataReg,
		UINT8  LdnSel,
		UINT8  Ldn,
		EFI_S3_SAVE_STATE_PROTOCOL *SaveState)
{
	SioLib_BootScriptIO(idxReg, LdnSel, SaveState);
	SioLib_BootScriptIO(dataReg, Ldn, SaveState);
	return;
}

VOID EFIAPI SioLib_BootScriptSioS3SaveTable(
		UINT16 idxReg,
		UINT16 dataReg,
		SIO_DEVICE_INIT_DATA  *Table,
		UINT8 Count,
		EFI_S3_SAVE_STATE_PROTOCOL *SaveState)
{
    UINT8   i;
    UINT8   Value8, Reg8;

    for(i=0;i<Count;i++) {
      Reg8 = (UINT8) (Table[i].Reg16 &0xFF);
      IoWrite8(idxReg,Reg8);
      //BOOT_SCRIPT_S3_IO_WRITE_MACRO(SaveState,
      //                            EfiBootScriptWidthUint8,
      //                            idxReg,
      //                            1,
      //                            &Reg8);
      SaveState->Write(SaveState,0x00,EfiBootScriptWidthUint8,(UINT64)idxReg,(UINTN)1,&Reg8);
      
      Value8 = IoRead8(dataReg);
      //BOOT_SCRIPT_S3_IO_WRITE_MACRO(SaveState,
      //                            EfiBootScriptWidthUint8,
      //                            dataReg,
      //                            1,
      //                            &Value8);
      SaveState->Write(SaveState,0x00,EfiBootScriptWidthUint8,(UINT64)dataReg,(UINTN)1,&Value8);
    }

    return;
}


VOID EFIAPI SioLib_BootScriptRTS3SaveTable(UINT16 Base, SIO_DEVICE_INIT_DATA  *Table, UINT8 Count, EFI_S3_SAVE_STATE_PROTOCOL *SaveState)
{
    UINT8   i;
    UINT8   Value8;

    for(i=0;i<Count;i++) {

        Value8=Table[i].OrData8;
        IoWrite8(Table[i].Reg16,Value8);
        //BOOT_SCRIPT_S3_IO_WRITE_MACRO(SaveState,
        //                            EfiBootScriptWidthUint8,
        //                            Table[i].Reg16,
        //                            1,
        //                            &Value8);
        
        SaveState->Write(SaveState,0x00,EfiBootScriptWidthUint8,(UINT64)Table[i].Reg16,(UINTN)1,&Value8);
    }

    return;
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

