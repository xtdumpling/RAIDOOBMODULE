//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Add a function that limit the primary VGA's resources under
//              4G.(Refer to Grantley)
//    Reason:   To prevent form Display have no output when enabled Above 4G
//              and set VGA Priority to offboard.
//    Auditor:  Isaac Hsu
//    Date:     Dec/14/2016
//
//*****************************************************************************
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
/** @file AmiPciBusLib.c
    Library Class for AMI PCI Bus And AMI Root Bridge common routines.


**/
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <AcpiRes.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <PciSetup.h>
#include <Token.h>
#include <Setup.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Library/AmiSdlLib.h>
//-------------------------------------------------------------------------
/// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
EFI_GUID		gPciSetupGuid	= PCI_FORM_SET_GUID;

BOOLEAN         gNoSdlDataMode  = FALSE;

//----------------------------------------------------------------------
///Define Variables == TOKENS to be able to use binary
// See token help for details
//----------------------------------------------------------------------

/**
    Variable to replace NB_DMI_L0_EXIT_LATENCY token.

    @note  const UINT16

**/
const UINT16 NbDmiL0ExitLatency =
#ifdef NB_DMI_L0_EXIT_LATENCY
    NB_DMI_L0_EXIT_LATENCY
#else
    0
#endif
    ;

/**
    Variable to replace NB_DMI_L1_EXIT_LATENCY token.

    @note  const UINT16

**/
const UINT16 NbDmiL1ExitLatency =
#ifdef NB_DMI_L1_EXIT_LATENCY
    NB_DMI_L1_EXIT_LATENCY
#else
    0
#endif
    ;

/**
    Variable to replace SB_DMI_L0_EXIT_LATENCY token.

    @note  const UINT16

**/
const UINT16 SbDmiL0ExitLatency =
#ifdef SB_DMI_L0_EXIT_LATENCY
    SB_DMI_L0_EXIT_LATENCY
#else
    0
#endif
    ;

/**
    Variable to replace SB_DMI_L1_EXIT_LATENCY token.

    @note  const UINT16

**/
const UINT16 SbDmiL1ExitLatency =
#ifdef SB_DMI_L1_EXIT_LATENCY
    SB_DMI_L1_EXIT_LATENCY
#else
    0
#endif
    ;

/**
    Variable to replace NB_DMI_ASPM_SUPPORT token.

    @note  const UINT16

**/
const UINT16 NbDmiAspmSupport =
#ifdef NB_DMI_ASPM_SUPPORT
    NB_DMI_ASPM_SUPPORT
#else
    0
#endif
    ;

/**
    Variable to replace SB_DMI_ASPM_SUPPORT token.

    @note  const UINT16

**/
const UINT16 SbDmiAspmSupport =
#ifdef SB_DMI_ASPM_SUPPORT
    SB_DMI_ASPM_SUPPORT
#else
    0
#endif
    ;

/**
    Variable to replace SB_INTERNAL_DELAY token.

    @note  const UINT16

**/
const UINT16 SbInternalDelay =
#ifdef SB_INTERNAL_DELAY
    SB_INTERNAL_DELAY
#else
    0
#endif
    ;

const BOOLEAN FixedBusAssign=
#ifdef PCI_FIXED_BUS_ASSIGNMENT
	PCI_FIXED_BUS_ASSIGNMENT
#else
	0
#endif
;

const BOOLEAN DecodeFullBusRanges=
#ifdef PCI_DECODE_FULL_BUS_RANGES
	PCI_DECODE_FULL_BUS_RANGES
#else
	0
#endif
;

const UINT8	PciRserveUncoreBuses =
#ifdef PCI_UNCORE_RESERVED_BUSES_PER_ROOT
	PCI_UNCORE_RESERVED_BUSES_PER_ROOT
#else
	0
#endif
;

const BOOLEAN HotPlugSupport=
#ifdef HOTPLUG_SUPPORT
	HOTPLUG_SUPPORT
#else
	0
#endif
;

const BOOLEAN CombineMemPfMem32=
#ifdef PCI_AMI_COMBINE_MEM_PMEM32
        PCI_AMI_COMBINE_MEM_PMEM32
#else
    0
#endif
;

const BOOLEAN CombineMem64PfMem=
#ifdef PCI_AMI_COMBINE_MEM64_PMEM
        PCI_AMI_COMBINE_MEM64_PMEM
#else
    0
#endif
;

const BOOLEAN IgnoreOptionRom=
#ifdef PCI_BUS_IGNORE_OPTION_ROM
        PCI_BUS_IGNORE_OPTION_ROM
#else
    0
#endif
;

const BOOLEAN Io32support=
#ifdef PCI_IO32_SUPPORT
        PCI_IO32_SUPPORT
#else
    0
#endif
;

const UINT32 CrdBusPaddingMmioLen=
#ifdef PCI_CARD_BUS_BRG_MMIO_PADDING_LEN
        PCI_CARD_BUS_BRG_MMIO_PADDING_LEN
#else
        0x2000000
#endif        
;

const UINT32 CrdBusPaddingMmioGra=
#ifdef PCI_CARD_BUS_BRG_MMIO_PADDING_ALN
        PCI_CARD_BUS_BRG_MMIO_PADDING_ALN
#else 
        0x1FFFFFF
#endif
;

const UINT32 CrdBusPaddingIoLen=
#ifdef PCI_CARD_BUS_BRG_IO_PADDING_LEN
        PCI_CARD_BUS_BRG_IO_PADDING_LEN
#else
        0x1000
#endif        
;

const UINT32 CrdBusPaddingGra=
#ifdef PCI_CARD_BUS_BRG_IO_PADDING_ALN
        PCI_CARD_BUS_BRG_IO_PADDING_ALN
#else 
        0xFFF
#endif
;


/**
    Variable to replace PCI_PORT_LAUNCH_INIT_ROUTINE_PRIORITY token.

    @note  const UINT16

**/
const UINT32 LaunchInitRoutinePriority =
#ifdef PCI_PORT_LAUNCH_INIT_ROUTINE_PRIORITY
		PCI_PORT_LAUNCH_INIT_ROUTINE_PRIORITY
#else
    0
#endif
    ;
	
/**
    Variable to replace HOTPLUG_APPLY_PADDING_ANYWAY token.

    @note  const UINT16

**/
const BOOLEAN ApplyPaddingAnyway=
#ifdef HOTPLUG_APPLY_PADDING_ANYWAY
		HOTPLUG_APPLY_PADDING_ANYWAY
#else
	0
#endif
;

const BOOLEAN UsePciIoForOptRom=
#ifdef PCI_BUS_USE_PCI_PCIIO_FOR_OPTION_ROM
        PCI_BUS_USE_PCI_PCIIO_FOR_OPTION_ROM
#else 
        0
#endif
;
        

const BOOLEAN PcieCrsSupport=
#ifdef PCIE_CRS_SUPPORT
		PCIE_CRS_SUPPORT
#else 
        0
#endif
;

const BOOLEAN PciRbCombineMemPmemReset=
#ifdef PCI_RB_COMBINE_MEM_PMEM_RESET
        PCI_RB_COMBINE_MEM_PMEM_RESET
#else 
        0
#endif
;

const BOOLEAN DeviceClassPolicyOverride =
#ifdef DEVICE_CLASS_POLICY_OVERRIDE
    DEVICE_CLASS_POLICY_OVERRIDE
#else
    0
#endif
    ;

const UINT8 PolicyOverrideMode =
#ifdef POLICY_OVERRIDE_MODE
    POLICY_OVERRIDE_MODE
#else
    0
#endif
    ;

const BOOLEAN LaunchCommonRoutineFirst =
#ifdef LAUNCH_COMMON_INIT_ROUTINE_FIRST
        LAUNCH_COMMON_INIT_ROUTINE_FIRST
#else
    0
#endif
    ;

const BOOLEAN DontUpdateRbResAsl =
#ifdef DONT_UPDATE_RB_RES_ASL
        DONT_UPDATE_RB_RES_ASL
#else
    0
#endif
    ;

const BOOLEAN DontUpdatePebs =
#ifdef DONT_UPDATE_PEBS_IN_AML
        DONT_UPDATE_PEBS_IN_AML
#else
    0
#endif
    ;

const BOOLEAN PciWriteVidDid=
#ifdef PCI_WRITE_VID_DID 
        PCI_WRITE_VID_DID
#else 
    1
#endif
    ;
// APTIOV_SERVER_OVERRIDE_START
const BOOLEAN DontUpdateRbUidAsl=
#ifdef DONT_UPDATE_RB_UID_IN_ASL 
        DONT_UPDATE_RB_UID_IN_ASL
#else 
    0
#endif
    ;
// APTIOV_SERVER_OVERRIDE_END
/**
    Reads Option Rom file. If it indicated  .

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_END_OF_MEDIA        When Search reaches last container.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

**/
EFI_STATUS PciBusReadNextEmbeddedRom(	PCI_DEV_INFO *Device, UINTN OpRomNumber, //IN
									AMI_SDL_PCI_DEV_INFO **ThisRomSdlData, VOID **RomFile, UINTN *RomSize ) //OUT
{
	EFI_STATUS				Status;
	AMI_SDL_PCI_DEV_INFO	*SdlData;
	EFI_GUID				NullGuid;
	EFI_GUID				*RomSectionGuid;
//------------------------
	
	pBS->SetMem(&NullGuid, sizeof(NullGuid),0);
	
	//If device does not have SDL data associated with it or search reache the and,
	if((Device->AmiSdlPciDevData==NULL) || (Device->SdlDevCount < OpRomNumber)) return EFI_INVALID_PARAMETER; 

	//Init Output data as if we did not found anything...
	*ThisRomSdlData=NULL;
	*RomFile=NULL;
	*RomSize=0;
	
	//Now read the ROM
	SdlData=NULL;
	Status=AmiSdlFindIndexRecord(Device->SdlIdxArray[OpRomNumber],&SdlData);
	//If following TRUE SdlData->RomFileGuid and SdlData->RomSectionGuid has to be filled on
	if(SdlData->PciDevFlags.Bits.EmbededRom){
		RomSectionGuid=&SdlData->RomSectionGuid;
		if(!MemCmp(&NullGuid,RomSectionGuid, sizeof(EFI_GUID))) RomSectionGuid=NULL;
		
		Status=AmiSdlReadFfsSdlData((UINT8**)RomFile, RomSize,&SdlData->RomFileGuid, RomSectionGuid);
		ASSERT_EFI_ERROR(Status);
		*ThisRomSdlData=SdlData;
		return Status;
	}
	return EFI_NOT_FOUND;
}


/**
    Fills gPciBusDb Array in ascending  order.

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

**/
EFI_STATUS  AmiPciAddBusDbEntry(AMI_SDL_PCI_DEV_INFO *SdlData, T_ITEM_LIST *BusDb)
{
    EFI_STATUS          	Status;
    UINTN               	i;
//--------------------------
    if(BusDb->ItemCount==0) {
         Status=AppendItemLst(BusDb, SdlData);
    } else {
        for(i=0; i<BusDb->ItemCount; i++){
			AMI_SDL_PCI_DEV_INFO	*sdldat=(AMI_SDL_PCI_DEV_INFO*)BusDb->Items[i];
		//------------------------------	
            if(sdldat->Bus > SdlData->Bus){
                return InsertItemLst(BusDb, SdlData, i);
            }
        }//for
        // if we here and didn't returned yet - BusHdr->BusBuild is the biggest one.
        Status = AppendItemLst(BusDb, SdlData);
    }

    return Status;
}

/**
    Fills gFixedResBarList Array in Descending  order.

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

**/
EFI_STATUS  AmiPciAddBarDbEntry(PCI_BAR *Bar, T_ITEM_LIST *BarDb, BOOLEAN Fixed)
{
    EFI_STATUS          	Status;
    UINTN               	i;
//--------------------------
    if(BarDb->ItemCount==0) {
         Status=AppendItemLst(BarDb, Bar);
    } else {
        for(i=0; i<BarDb->ItemCount; i++){
			PCI_BAR	*bar;
		//------------------------------	
			bar=(PCI_BAR*)BarDb->Items[i];
            if(Fixed){
                if( bar->Base > Bar->Base )return InsertItemLst(BarDb, Bar, i);
            } else {
                if( Bar->Length > bar->Length )return InsertItemLst(BarDb, Bar, i);
            }
        }//for
        // if we here and didn't returned yet - BAR is the biggest/smallest one.
        Status = AppendItemLst(BarDb, Bar);
    }

    return Status;
}

/**
    This function will parse PCI SDL Database looking for HostBridge
    entries  and populate gRootBusDb
    Structure.

    @param Nothing

    @retval EFI_SUCCESS is OK


    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS SortRbSdlData(AMI_SDL_PCI_DEV_INFO ***RbSdlData, UINTN *SdlDataCount)
{
	AMI_SDL_PCI_DEV_INFO	*tmp;
	UINTN					i;
	T_ITEM_LIST             rblst={0,0,NULL};
	AMI_SDL_PCI_DEV_INFO    **prbarray=*RbSdlData;
	EFI_STATUS				Status=EFI_SUCCESS;
//--------------------------------
	//Fill tmprbdb with Root Bridges SdlData checking PciDevFlags if it is a real RB
	for(i=0; i<*SdlDataCount; i++){
		tmp=prbarray[i];
		if(tmp->PciDevFlags.Bits.RootBridgeType){
			//AddBusDbEntry function will sort entries based on Bus number
			Status=AmiPciAddBusDbEntry(tmp, &rblst);
			ASSERT_EFI_ERROR(Status);
			if(EFI_ERROR(Status)) return Status;
		}
	}

	//now rblst has Root bridges sorted in ascending order.
	//if among HB childs we found something else than RB update root count...
	if(rblst.ItemCount!=*SdlDataCount) *SdlDataCount=rblst.ItemCount;

	//Now copy sorted entries inside passed address
	//free memory used.
	pBS->FreePool(prbarray);

	prbarray=(AMI_SDL_PCI_DEV_INFO**)rblst.Items;
	*RbSdlData=prbarray;


    return Status;
}

/**
    Fills gPciBusDb Array in ascending  order.

        
    @param Ext Pointer to PCI Bridge Extension Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

**/
EFI_STATUS  AmiPciAddDevDbEntry(AMI_SDL_PCI_DEV_INFO *SdlData, T_ITEM_LIST *BusDb)
{
    EFI_STATUS          	Status;
//--------------------------
    if(BusDb->ItemCount==0) {
         Status=AppendItemLst(BusDb, SdlData);
    } else {
		UINTN   i;
		UINT32	thisdevf=(SdlData->Device<<8)|SdlData->Function;
	//-----------------	
        for(i=0; i<BusDb->ItemCount; i++){
		    UINT32					devf;
			AMI_SDL_PCI_DEV_INFO	*sdldat = (AMI_SDL_PCI_DEV_INFO*)BusDb->Items[i];
		//---------------------------------------
        	devf=(sdldat->Device<<8)|sdldat->Function;
        	if(devf > thisdevf){
                return InsertItemLst(BusDb, SdlData, i);
            }
        }//for
        // if we here and didn't returned yet - BusHdr->BusBuild is the biggest one.
        Status = AppendItemLst(BusDb, SdlData);
    }

    return Status;
}


/**
    Will do PCI Configuration Space Access 8 bit width

        
    @param RbIo Pointer to PciRootBridgeIO Protocol.
    @param addr PCI_CFG_ADDR filled by caller
    @param wr TRUE = Write FALSE = Read
    @param buff Pointer Data Buffer.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

 Referals:PCI_CFG_ADDR

**/
EFI_STATUS PciCfg8(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT8 *buff){
	if(wr)return RbIo->Pci.Write(RbIo, EfiPciWidthUint8, addr.ADDR, 1, (VOID*)buff);
	else return RbIo->Pci.Read(RbIo, EfiPciWidthUint8, addr.ADDR, 1, (VOID*)buff);
}
/**
    Will do PCI Configuration Space Access 16 bit width

        
    @param RbIo Pointer to PciRootBridgeIO Protocol.
    @param addr PCI_CFG_ADDR filled by caller
    @param wr TRUE = Write FALSE = Read
    @param buff Pointer Data Buffer.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

 Referals:PCI_CFG_ADDR

**/
EFI_STATUS PciCfg16(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT16 *buff){
	EFI_STATUS	Status;
//------------
	if(wr)Status=RbIo->Pci.Write(RbIo, EfiPciWidthUint16, addr.ADDR, 1, (VOID*)buff);
	else Status=RbIo->Pci.Read(RbIo, EfiPciWidthUint16, addr.ADDR, 1, (VOID*)buff);
	//it might be a Width issue on Pci Root bridge level
	if(Status==EFI_INVALID_PARAMETER) {
		UINT8	*b=(UINT8*)buff;
	//--------------------
		Status=PciCfg8(RbIo, addr, wr, b);
		if(EFI_ERROR(Status)) return Status;

        //Check if Extended register used then Addr.Register is ignored.
        if( addr.Addr.ExtendedRegister != 0) addr.Addr.ExtendedRegister += 1;
        else addr.Addr.Register += 1;

		b++;
		Status=PciCfg8(RbIo, addr, wr, b);
	}
	return Status;
}


/**
    Will do PCI Configuration Space Access 32 bit width

        
    @param RbIo Pointer to PciRootBridgeIO Protocol.
    @param addr PCI_CFG_ADDR filled by caller
    @param wr TRUE = Write FALSE = Read
    @param buff Pointer Data Buffer.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

 Referals:PCI_CFG_ADDR

**/
EFI_STATUS PciCfg32(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT32 *buff){
	EFI_STATUS	Status;
//------------
	if(wr)Status=RbIo->Pci.Write(RbIo, EfiPciWidthUint32, addr.ADDR, 1, (VOID*)buff);
	else Status=RbIo->Pci.Read(RbIo, EfiPciWidthUint32, addr.ADDR, 1, (VOID*)buff);
	//it might be a Width issue on Pci Root bridge level
	if(Status==EFI_INVALID_PARAMETER) {
		UINT16	*b=(UINT16*)buff;
	//--------------------
		Status=PciCfg16(RbIo, addr, wr, b);
		if(EFI_ERROR(Status)) return Status;

        //Check if Extended register used then Addr.Register is ignored.
        if( addr.Addr.ExtendedRegister != 0) addr.Addr.ExtendedRegister += 2;
        else addr.Addr.Register += 2;

		b++;
		Status=PciCfg16(RbIo, addr, wr, b);
	}
	return Status;
}


/**
    Will do PCI Configuration Space Access 64 bit width

        
    @param RbIo Pointer to PciRootBridgeIO Protocol.
    @param addr PCI_CFG_ADDR filled by caller
    @param wr TRUE = Write FALSE = Read
    @param buff Pointer Data Buffer.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_NOT_FOUND           When Device not present in the system.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

 Referals:PCI_CFG_ADDR

**/
EFI_STATUS PciCfg64(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *RbIo, PCI_CFG_ADDR addr, BOOLEAN wr, UINT64 *buff){
	EFI_STATUS	Status;
//------------
	if(wr)Status=RbIo->Pci.Write(RbIo, EfiPciWidthUint64, addr.ADDR, 1, (VOID*)buff);
	else Status=RbIo->Pci.Read(RbIo, EfiPciWidthUint64, addr.ADDR, 1, (VOID*)buff);
	//it might be a Width issue on Pci Root bridge level
	if((Status==EFI_INVALID_PARAMETER) && (buff!=NULL)) {
		UINT32	*b=(UINT32*)buff;
	//--------------------
		Status=PciCfg32(RbIo, addr, wr, b);
		if(EFI_ERROR(Status)) return Status;

        //Check if Extended register used then Addr.Register is ignored.
        if( addr.Addr.ExtendedRegister != 0) addr.Addr.ExtendedRegister += 4;
        else addr.Addr.Register += 4;

		b++;
		Status=PciCfg32(RbIo, addr, wr, b);
	}
	return Status;
}

/**
    Checks if PCI_DEV_INFO data passed belongs to Function 0 of
    Multy-Functional device.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    Device is Function 0 of Mulifunctional device.
        FALSE   Device is not Function 0 of Mulifunctional device.

**/
BOOLEAN IsFunc0OfMfDev(PCI_DEV_INFO *Device ){
    //If (Func0==NULL && FuncCount==0) function is a single function device, following fields are not used and reserved;
    //If (Func0!=NULL && FuncCount==0) function is one of the Func1..Func7 of multifunc device Func0 points on DevFunc0;
    //If (Func0!=NULL && (FuncCount!=0 || FuncInitCnt!=0)) function is Func0 of multifunc device DevFunc holds pointers at all other Func1..7 found
    //If (Func0==NULL && FuncCount!=0) Illehgal combination - reserved!
    if((Device->Func0!=NULL) && ((Device->FuncInitCnt!=0)||(Device->FuncCount!=0))) return TRUE;
    return FALSE;
}

/**
    Checks if PCI_DEV_INFO data passed belongs to other than Func0 of
    Multy-Functional device.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    Device is Function 0 of Mulifunctional device.
        FALSE   Device is not Function 0 of Mulifunctional device.

**/
BOOLEAN IsNotFunc0OfMfDev(PCI_DEV_INFO *Device ){
    //If (Func0==NULL && FuncCount==0) function is a single function device, following fields are not used and reserved;
    //If (Func0!=NULL && FuncCount==0) function is one of the Func1..Func7 of multifunc device Func0 points on DevFunc0;
    //If (Func0!=NULL && (FuncCount!=0 || FuncInitCnt!=0)) function is Func0 of multifunc device DevFunc holds pointers at all other Func1..7 found
    //If (Func0==NULL && FuncCount!=0) Illehgal combination - reserved!
    if((Device->Func0!=NULL) && (Device->FuncInitCnt==0)) return TRUE;
    return FALSE;
}

/**
    Checks if PCI_DEV_INFO is Function 0 of PCI device.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    Device is Function 0 of Mulifunctional device.
        FALSE   Device is not Function 0 of Mulifunctional device.

**/
BOOLEAN IsFunc0(PCI_DEV_INFO *Device ){
    //If (Func0==NULL && FuncCount==0) function is a single function device, following fields are not used and reserved;
    //If (Func0!=NULL && FuncCount==0) function is one of the Func1..Func7 of multyfunc device Func0 points on DevFunc0;
    //If (Func0!=NULL && (FuncCount!=0 || FuncInitCnt!=0)) function is Func0 of multyfunc device DevFunc holds pointers at all other Func1..7 found
    //If (Func0==NULL && FuncCount!=0) Illehgal combination - reserved!
    if(IsFunc0OfMfDev(Device)) return TRUE;
    if((Device->Func0==NULL) && (Device->FuncInitCnt==0) && (Device->FuncCount==0)) return TRUE;
    return FALSE;
}

/**
    Creates a copy of T_ITEM_LST structure.

        
    @param Lst Pointer to the structure to copy.
    @param NewLstPtr Double Pointer to the copied data (Memory allocation is done by this function).

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.

**/
EFI_STATUS CpyItemLst(T_ITEM_LIST *Lst, T_ITEM_LIST **NewLstPtr)
{
    T_ITEM_LIST     *NewLst;
//--------------------------
	if(*NewLstPtr == NULL) *NewLstPtr = MallocZ(sizeof(T_ITEM_LIST));

	if (*NewLstPtr==NULL) return EFI_OUT_OF_RESOURCES;

	NewLst = *NewLstPtr;
	NewLst->InitialCount = Lst->InitialCount;
	if (Lst->InitialCount == 0) return EFI_SUCCESS;

	NewLst->Items = MallocZ( Lst->InitialCount * sizeof(VOID*) );
	if (!NewLst->Items) return EFI_OUT_OF_RESOURCES;

	pBS->CopyMem((VOID*)NewLst->Items,(VOID*)Lst->Items,sizeof(VOID*)*Lst->ItemCount);

	NewLst->ItemCount = Lst->ItemCount;

    return EFI_SUCCESS;
}

/**
    Programm SubordinateBusNumber Register of PCI Bridge.

        
    @param Brg Pointer to PCI Bridge Private Data structure.
    @param SubBusNo Number to programm.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_BRG_INFO.

**/
EFI_STATUS SetSubBus(PCI_DEV_INFO *Brg, UINT8 SubBusNo)
{
	PCI_CFG_ADDR	addr;
//---------------------------------------
	addr.ADDR=Brg->Address.ADDR;
	addr.Addr.Register=PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET; //Sub Bus No reg

	return PciCfg8(Brg->RbIo, addr,TRUE,&SubBusNo);
}


/**
    Maps Bridge's Primary Secondary Subordinate Bus Numbers
    according information stored in PCI_DEV_INFO and PCI_BRG_EXT structures
    of the PCI Bridge.

        
    @param Brg Pointer to PCI Bridge Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.
        EFI_INVALID_PARAMETER   When some of the parameters are invalid.

 Referals: PCI_DEV_INFO; PCI_BRG_INFO; PCI_BRG_EXT.

    @note  
  BaseBus         =   PCI_DEV_INFO->Address.Addr.Bus;
  SecondaryBus    =   PCI_BRG_EXT->Res[rtBus].Base;
  SubordinateBus  =   PCI_BRG_EXT->Res[rtBus].Base + PCI_BRG_EXT->Res[rtBus].Length-1;

**/
EFI_STATUS MapBridgeBuses(PCI_DEV_INFO *Brg)
{
	EFI_STATUS		Status;
	PCI_CFG_ADDR	addr;
	UINT8			bus;
	PCI_BRG_EXT		*ext=(PCI_BRG_EXT*)(Brg+1);
//--------------------------------
	//Get Bridge Initial Address
	addr.ADDR=Brg->Address.ADDR;
	//Primary bus;
	addr.Addr.Register=PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET;    //Primary BusNo
	bus=Brg->Address.Addr.Bus;
	Status=PciCfg8(Brg->RbIo,addr,TRUE,&bus);
	if(EFI_ERROR(Status))return Status;

	//SecondaryBus Register
	addr.Addr.Register=PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET; //Secondary BusNo

	bus=(UINT8)ext->Res[rtBus].Base;
	Status=PciCfg8(Brg->RbIo,addr,TRUE,&bus);
	if(EFI_ERROR(Status))return Status;

	//Now Programm SubordinateBusNo reg
	bus=(UINT8)(ext->Res[rtBus].Base+ext->Res[rtBus].Length-1);
	return SetSubBus(Brg, bus);
}


/**
    This function will Reprogram Primary Secondary and Subordinate
    bus numbers for the downsteram bridges after SEC BUS reset signal assertion.

        
    @param DnPort Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS RestoreBridgeBuses(PCI_DEV_INFO *DnPort){
    UINTN           i;
    PCI_BRG_EXT     *ext=(PCI_BRG_EXT*)(DnPort+1);
    PCI_DEV_INFO    *dev;
    EFI_STATUS      Status=EFI_SUCCESS;
//---------------------
    for(i=0; i< ext->ChildCount; i++){
        dev=ext->ChildList[i];
        if(dev->Type==tPci2PciBrg){
            Status=MapBridgeBuses(dev);
            if(EFI_ERROR(Status)) return Status;
            //call recoursively to cover all hierarchy
            Status=RestoreBridgeBuses(dev);
            if(EFI_ERROR(Status)) return Status;
        }
    }
    return Status;
}

/**
    Checks if value passed makes "POWER OF TWO"

        
    @param Value Value to check.

    @retval BOOLEAN
        TRUE or FALSE based on value passed.
**/
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

/**
    Clears/zeros contents of PCI_BAR structure.

        
    @param Bar Pointer to PCI_BAR structure to clear.

    @retval Nothing

**/
VOID ClearBar(PCI_BAR *Bar)
{
	Bar->Type=tBarUnused;
	Bar->Gran=0;
	Bar->Length=0;
	Bar->Base=0;
}

UINT8 FindRootBus(PCI_DEV_INFO *Device){
	PCI_DEV_INFO *parent=Device->ParentBrg;
//-----------------------
	if (parent==NULL) return 0xFF;

	while(parent->Type!=tPciRootBrg){
		parent=parent->ParentBrg;
	}
	return parent->Address.Addr.Bus;
}

/**
    Porting function which collects all PCI Bus Driver specific
    SETUP configuration data and returns it to the PCI Bus Driver.

        
    @param PciSetupData Pointer to the Pci Bus specific setup data buffer.

    @retval Nothing
**/

//PCI_DEV_01

/**
    This function generate Variable Name associated with each SIO Logical Device.
    "Volatile" or None "Volatile"

**/
EFI_STRING AmiPciGetPciVarName(AMI_SDL_PCI_DEV_INFO *SdlInfo, UINTN Idx, PCI_SETUP_TYPE VarType){
	CHAR16 s[40];
	EFI_STRING	ret=NULL;
//------------------	
	if(SdlInfo==NULL) return ret;
	
	switch (VarType){
		case dtDevice:
			Swprintf(s, PCI_DEV_DEV_VAR_NAME_FORMAT, Idx);
		break;	
		case dtPcie1:
			Swprintf(s, PCI_DEV_PE1_VAR_NAME_FORMAT, Idx);
		break;	
		case dtPcie2:
			Swprintf(s, PCI_DEV_PE2_VAR_NAME_FORMAT, Idx);
		break;	
		case dtHp:
			Swprintf(s, PCI_DEV_HP_VAR_NAME_FORMAT, Idx);
		break;
		default: return NULL;
	}
	
	ret=MallocZ(Wcslen(s)*sizeof(CHAR16)+sizeof(CHAR16));
	Wcscpy(ret, s);
	return ret;
}

EFI_STATUS AmiPciGetPciHpSetupData( PCI_HP_SETUP_DATA 		*PciHp,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,		//OPTIONAL if == 0 
									BOOLEAN					Set)
{		
    EFI_STATUS          	Status=EFI_SUCCESS;
    UINTN               	vsz;
    EFI_STRING				vname=NULL;
//-------------------------------------------------------------
	if(PciHp != NULL){
		//Get PCI Express GEN 1 Device setup data variable
		vsz=sizeof(PCI_HP_SETUP_DATA);
		vname=AmiPciGetPciVarName(DevSdlInfo, DevIdx, dtHp);

		if(vname!=NULL){
			if(Set) {
				Status=pRS->SetVariable(vname, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, PciHp);
			} else 	Status=pRS->GetVariable(vname,&gPciSetupGuid, NULL, &vsz, PciHp);
		}
		pBS->FreePool(vname);
	} else Status=EFI_INVALID_PARAMETER; //Pcie2	
	return Status;
}

EFI_STATUS AmiPciGetPcie2SetupData( PCIE2_SETUP_DATA 		*Pcie2,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,		//OPTIONAL if == 0 
									BOOLEAN					Set)
{		
    EFI_STATUS          	Status=EFI_SUCCESS;
    UINTN               	vsz;
    EFI_STRING				vname=NULL;
//-------------------------------------------------------------

	if(Pcie2!=NULL){
		//Get PCI Express GEN 1 Device setup data variable
		vsz=sizeof(PCIE2_SETUP_DATA);
		vname=AmiPciGetPciVarName(DevSdlInfo, DevIdx, dtPcie2);

		if(vname!=NULL){
			if(Set) {
				Status=pRS->SetVariable(vname, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, Pcie2);
				ASSERT_EFI_ERROR(Status);
				pBS->FreePool(vname);
				return Status;
			} else 	Status=pRS->GetVariable(vname,&gPciSetupGuid, NULL, &vsz, Pcie2);
		}
			
		//Fill Data buffer with Default values...
		if(vname==NULL || EFI_ERROR(Status) ){
			//Gen2 Link Settings
			//UINT8   LnkSpeed;               //[Auto]\ 5.0 GHz \ 2.5 GHz
			Pcie2->LnkSpeed=PCI_SETUP_AUTO_VALUE;
			//UINT8   DeEmphasis;             //[Disable]\ Enable
			//UINT8   ClockPm;                //[Disable]\ Enable
			//UINT8   ComplSos;               //[Disable]\ Enable

			//UINT8   HwAutoWidth;            //[Enable]\ Disable
			//PciSetupData->HwAutoWidth=0;
			//UINT8   HwAutoSpeed;            //[Enable]\ Disable
			//PciSetupData->HwAutoSpeed=0;
			if(Status==EFI_NOT_FOUND) {
				Status=pRS->SetVariable(vname, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, Pcie2);
				ASSERT_EFI_ERROR(Status);
			} else {
				ASSERT_EFI_ERROR(Status);
			}
		}
		if(vname!=NULL)pBS->FreePool(vname);
	} else Status=EFI_INVALID_PARAMETER; //Pcie2	
	return Status;
}


EFI_STATUS AmiPciGetPcie1SetupData( PCIE1_SETUP_DATA 		*Pcie1,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,		//OPTIONAL if == 0 
									BOOLEAN					Set)
{		
    EFI_STATUS          	Status=EFI_SUCCESS;
    UINTN               	vsz;
    EFI_STRING				vname=NULL;
//-------------------------------------------------------------

	if(Pcie1!=NULL){
		//Get PCI Express GEN 1 Device setup data variable
		vsz=sizeof(PCIE1_SETUP_DATA);
		vname=AmiPciGetPciVarName(DevSdlInfo, DevIdx, dtPcie1);
		if(vname!=NULL){
			if(Set) {
				Status=pRS->SetVariable(vname, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, Pcie1);
				ASSERT_EFI_ERROR(Status);
				pBS->FreePool(vname);
				return Status;
			} else 	Status=pRS->GetVariable(vname,&gPciSetupGuid, NULL, &vsz, Pcie1);
		}
			
		//Fill Data buffer with Default values...
		if(vname==NULL || EFI_ERROR(Status) ){
			//PCI Express Device Settings: [] - default
			//UINT8	RelaxedOrdering; 		// Disable\[Enable]
		    Pcie1->RelaxedOrdering=1;
		    //UINT8	ExtTagField; 			//[Disable]\ Enable
			//UINT8	NoSnoop;				// Disable \[Enable]
			Pcie1->NoSnoop=1;

			//UINT8	MaxPayload;				//[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
			Pcie1->MaxPayload=PCI_SETUP_AUTO_VALUE;
			//UINT8 MaxReadRequest;			//[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
			Pcie1->MaxReadRequest=PCI_SETUP_AUTO_VALUE;
			//PCI Express Link settings: [] - default
			//UINT8 AspmMode; 				//[Disable]\ Auto \ Forse L0
			//UINT8 ExtendedSynch;			//[Disable]\ Enable

			//Fill in Default Values... (buffer was initialized with ZEROs)
			//UINT8   LnkTrRetry;             //Disable\ 2 \ 3 \[5]
			Pcie1->LnkTrRetry=5;
			//UINT16  LnkTrTimout;            //<1.[2]..100> (Microseconds uS)
			Pcie1->LnkTrTimeout=0x10;

			//Gen2 Device Settings
			//UINT8   ComplTimeOut;           //[Disable]\ Default \ 50 - 100 us \ 1ms - 10ms \ 16...
			//UINT8   AriFwd;                 //[Disable]\ Enable
			//UINT8   AtomOpReq;              //[Disable]\ Enable
			//UINT8   AtomOpEgressBlk;        //[Disable]\ Enable
			//UINT8   IDOReq;                 //[Disable]\ Enable
			//UINT8   IDOCompl;               //[Disable]\ Enable
			//UINT8   LtrReport;              //[Disable]\ Enable
			//UINT8   E2ETlpPrBlk;            //[Disable]\ Enable
			if(Status==EFI_NOT_FOUND) {
				Status=pRS->SetVariable(vname, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, Pcie1);
				ASSERT_EFI_ERROR(Status);
			} else {
				ASSERT_EFI_ERROR(Status);
			}
		}
		
		if(vname!=NULL)pBS->FreePool(vname);
	} else Status=EFI_INVALID_PARAMETER; //Pcie1	
	return Status;
}


EFI_STATUS AmiPciGetPciDevSetupData(PCI_DEVICE_SETUP_DATA 	*PciDev,
									AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,//OPTIONAL if == NULL get defaults...
									UINTN					DevIdx,
									BOOLEAN 				Set)		

{		
    EFI_STATUS          	Status=EFI_SUCCESS;
    UINTN               	vsz;
    EFI_STRING				vname=NULL;
//-------------------------------------------------------------
	if(PciDev!=NULL){
		//Get Device Conventional PCI  setup data variable
		vsz=sizeof(PCI_DEVICE_SETUP_DATA);
		vname=AmiPciGetPciVarName(DevSdlInfo, DevIdx, dtDevice);

		if(vname!=NULL){
			if(Set) {
				Status=pRS->SetVariable(vname, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, PciDev);
				ASSERT_EFI_ERROR(Status);
				pBS->FreePool(vname);
				return Status;
			} else 	Status=pRS->GetVariable(vname, &gPciSetupGuid, NULL, &vsz, PciDev);
		}
		
		//Fill Data buffer with Default values...
		if(vname==NULL || EFI_ERROR(Status)){
		
			//Zero out PCI_SETUP_DATA buffer
			pBS->SetMem(PciDev, sizeof(PCI_DEVICE_SETUP_DATA), 0);
		
			//General PCI Settings: [] - default
			//UINT8 PciLatency;				//[32]\ 64 \ 96 \ 128 \ 160 \ 192 \ 224 \ 248
			PciDev->PciLatency=32;
			//UINT8 OpRomPos                // Legacy \[EFI Compatible]
			//PciSetupData->OpRomPost=1;
			//UINT8 PciXLatency;			// 32 \[64]\ 96 \ 128 \ 160 \ 192 \ 224 \ 248
			PciDev->PciXLatency=64;
			//UINT8   VgaPallete;             //[Disable]\ Enable
			//UINT8   PerrEnable;             //[Disable]\ Enable
			//UINT8   SerrEnable;             //[Disable]\ Enable
			//----Device feture Disable Items----------------------
		    //UINT8   Decode4gDisable;        //[Disable]\ Enable
		    //UINT8   Pcie1Disable;           //[Disable]\ Enable
		    //UINT8   Pcie2Disable;           //[Disable]\ Enable
		    //UINT8   HpDisable;              //[Disable]\ Enable

			if(Status==EFI_NOT_FOUND) {
				Status=pRS->SetVariable(vname, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, PciDev);
				ASSERT_EFI_ERROR(Status);
			} else {
				ASSERT_EFI_ERROR(Status);
			}
		}
		if(vname!=NULL)pBS->FreePool(vname);
	}else Status=EFI_INVALID_PARAMETER; //PciCommon	
	return Status;
}
    
EFI_STATUS AmiPciGetCommonSetupData(PCI_COMMON_SETUP_DATA 	*PciCommon)
{		
    EFI_STATUS          	Status=EFI_SUCCESS;
    UINTN               	vsz;
//-------------------------------------------------------------
    //Get Setup Data
	if(PciCommon!=NULL){
		//Get global setup variable
		vsz=sizeof(PCI_COMMON_SETUP_DATA);
		Status=pRS->GetVariable(PCI_COMMON_VAR_NAME, &gPciSetupGuid, NULL, &vsz, PciCommon);
		if(EFI_ERROR(Status)){
			//Get Common Settings first
			pBS->SetMem(PciCommon, sizeof(PCI_COMMON_SETUP_DATA), 0);
			//PciCommon->S3ResumeVideoRepost=0;	//[Disable]\ Enable
			//PciCommon->S3PciExpressScripts=0; //[Disable]\ Enable
			PciCommon->HotPlug=1;             // Disable \[Enable]
			//Will use SDL token to initialize default value
			PciCommon->Above4gDecode=ABOVE_4G_PCI_DECODE;  
			PciCommon->SriovSupport=SRIOV_SUPPORT;        
			
			if(Status==EFI_NOT_FOUND) {
				Status=pRS->SetVariable(PCI_COMMON_VAR_NAME, &gPciSetupGuid, 
								(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
								vsz, PciCommon);
				ASSERT_EFI_ERROR(Status);
			} else {
				ASSERT_EFI_ERROR(Status);
			}
		}
	} else Status=EFI_INVALID_PARAMETER; //PciCommon	
	return Status;
}

/**
    Porting function which collects all PCI Bus Driver specific
    SETUP configuration data and returns it to the PCI Bus Driver.

        
    @param PciSetupData Pointer to the Pci Bus specific setup data buffer.

    @retval Nothing
**/
#if (PCI_SETUP_USE_APTIO_4_STYLE == 1)
EFI_STATUS AmiPciGetSetupData(	PCI_SETUP_DATA 			*PciSetupData, 
								PCI_COMMON_SETUP_DATA 	*PciCommon,
								PCI_HOTPLUG_SETUP_DATA 	*HpSetup)
{
    EFI_GUID 			SetupGuid	= SETUP_GUID;
    EFI_STATUS          Status;
    UINTN               sz=sizeof(SETUP_DATA);
    SETUP_DATA          *SetupData=NULL;
//-------------------------------------

    if( PciSetupData==NULL && PciCommon==NULL && HpSetup==NULL) return EFI_INVALID_PARAMETER;
    //Get Setup Data
    SetupData=MallocZ(sizeof(SETUP_DATA));
    if (SetupData==NULL) return EFI_OUT_OF_RESOURCES;

    //Get global setup variable
    Status=GetEfiVariable(L"Setup",&SetupGuid, NULL, &sz, &SetupData);
	if(EFI_ERROR(Status)){
		
		if(PciCommon!=NULL){
			//Get Common Settings first
			pBS->SetMem(PciCommon, sizeof(PCI_COMMON_SETUP_DATA), 0);
			//PciCommon->S3ResumeVideoRepost=0;	//[Disable]\ Enable
			//PciCommon->S3PciExpressScripts=0; //[Disable]\ Enable
			PciCommon->HotPlug=1;             // Disable \[Enable]
            //Will use SDL token to initialize default value
			PciCommon->Above4gDecode=ABOVE_4G_PCI_DECODE;
			PciCommon->SriovSupport=SRIOV_SUPPORT;       
		}
		
		if(PciSetupData!=NULL){
		    //Zero out PCI_SETUP_DATA buffer
			pBS->SetMem(PciSetupData, sizeof(PCI_SETUP_DATA), 0);
			

			//General PCI Settings: [] - default
			//UINT8 PciLatency;				//[32]\ 64 \ 96 \ 128 \ 160 \ 192 \ 224 \ 248
			PciSetupData->PciDevSettings.PciLatency=32;
			//UINT8 OpRomPos                // Legacy \[EFI Compatible]
			//PciSetupData->OpRomPost=1;
	//#if PCI_X_SUPPORT
			//UINT8 PciXLatency;			// 32 \[64]\ 96 \ 128 \ 160 \ 192 \ 224 \ 248
			PciSetupData->PciDevSettings.PciXLatency=64;
	//#endif
			//UINT8   VgaPallete;             //[Disable]\ Enable
			//UINT8   PerrEnable;             //[Disable]\ Enable
			//UINT8   SerrEnable;             //[Disable]\ Enable

			//PCI Express Device Settings: [] - default
			//UINT8	RelaxedOrdering; 		// Disable\[Enable]
			PciSetupData->Pcie1Settings.RelaxedOrdering=1;
			//UINT8	ExtTagField; 			//[Disable]\ Enable
			//UINT8	NoSnoop;				// Disable \[Enable]
			PciSetupData->Pcie1Settings.NoSnoop=1;

			//UINT8	MaxPayload;				//[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
			PciSetupData->Pcie1Settings.MaxPayload=PCI_SETUP_AUTO_VALUE;
			//UINT8 MaxReadRequest;			//[Auto]\ 128 \ 256 \ 512 \ 1024 \ 2048 \ 4096 (in bytes)
			PciSetupData->Pcie1Settings.MaxReadRequest=PCI_SETUP_AUTO_VALUE;
			//PCI Express Link settings: [] - default
			//UINT8 AspmMode; 				//[Disable]\ Auto \ Forse L0
			//UINT8 ExtendedSynch;			//[Disable]\ Enable

			//Fill in Default Values... (buffer was initialized with ZEROs)
			//UINT8   LnkTrRetry;             //Disable\ 2 \ 3 \[5]
			PciSetupData->Pcie1Settings.LnkTrRetry=5;
			//UINT16  LnkTrTimout;            //<1.[2]..100> (Microseconds uS)
			PciSetupData->Pcie1Settings.LnkTrTimeout=0x10;

			//Gen2 Device Settings
			//UINT8   ComplTimeOut;           //[Disable]\ Default \ 50 - 100 us \ 1ms - 10ms \ 16...
			//UINT8   AriFwd;                 //[Disable]\ Enable
			//UINT8   AtomOpReq;              //[Disable]\ Enable
			//UINT8   AtomOpEgressBlk;        //[Disable]\ Enable
			//UINT8   IDOReq;                 //[Disable]\ Enable
			//UINT8   IDOCompl;               //[Disable]\ Enable
			//UINT8   LtrReport;              //[Disable]\ Enable
			//UINT8   E2ETlpPrBlk;            //[Disable]\ Enable

			//Gen2 Link Settings
			//UINT8   LnkSpeed;               //[Auto]\ 5.0 GHz \ 2.5 GHz
			PciSetupData->Pcie2Settings.LnkSpeed=55;
			//UINT8   DeEmphasis;             //[Disable]\ Enable
			//UINT8   ClockPm;                //[Disable]\ Enable
			//UINT8   ComplSos;               //[Disable]\ Enable

			//UINT8   HwAutoWidth;            //[Enable]\ Disable
			//PciSetupData->HwAutoWidth=0;
			//UINT8   HwAutoSpeed;            //[Enable]\ Disable
			//PciSetupData->HwAutoSpeed=0;
		}

		if(HpSetup != NULL){
//#if AMI_HOTPLUG_INIT_SUPPORT
			//UINT8   BusPadd;                // Disable \[1]\ 2 \ 3 \ 4 \ 5
			HpSetup->ArrayField.BusPadd=1;         //    0       1    2     3    4
			//UINT8   IoPadd;                 // Disable \[ 4K]\ 8K \ 16K \ 32K
			HpSetup->ArrayField.IoPadd=0x1000;     //    0       1    2     3    4     5     6     7
			HpSetup->ArrayField.Io32Padd=0;		  //Not used in IAPC systems	
			//UINT8   Mmio32Padd;             // Disable \  1M \ 4M \  8M \[16M]\ 32M \ 64M \128M
			HpSetup->ArrayField.Mmio32Padd=0x1000000;
			//UINT8   Mmio32PfPadd;           // Disable \  1M \ 4M \  8M \[16M]\ 32M \ 64M \128M
			HpSetup->ArrayField.Mmio32PfPadd=0x1000000;
			HpSetup->ArrayField.Mmio64Padd=0;      //[Disable]\  1M \ 4M \  8M \ 16M \ 32M \ 64M \ 128M \ 256M \ 512M \ 1G
			HpSetup->ArrayField.Mmio64PfPadd=0;    //[Disable]\  1M \ 4M \  8M \ 16M \ 32M \ 64M \ 128M \ 256M \ 512M \ 1G

//#endif
		}

	} else {
		//General PCI Settings: [] - default
		if (PciCommon!=NULL){
			PciCommon->S3ResumeVideoRepost=SetupData->S3ResumeVideoRepost;
			PciCommon->S3PciExpressScripts=SetupData->S3PciExpressScripts;
			PciCommon->HotPlug=SetupData->HotPlugEnable;
			PciCommon->Above4gDecode=SetupData->Above4gDecode;
			PciCommon->SriovSupport = SetupData->SriovSupport;
		}

		if(PciSetupData!=NULL){
			PciSetupData->PciDevSettings.PciLatency = SetupData->PciLatency;
//#if PCI_X_SUPPORT
			PciSetupData->PciDevSettings.PciXLatency = SetupData->PciXLatency;
//#endif
			//General2 PCI Settings: [] - default
			PciSetupData->PciDevSettings.VgaPallete = SetupData->VgaPallete;
			PciSetupData->PciDevSettings.PerrEnable = SetupData->PerrEnable;
			PciSetupData->PciDevSettings.SerrEnable = SetupData->SerrEnable;

//#if PCI_EXPRESS_SUPPORT
			//PCI Express Device Settings: [] - default
			PciSetupData->Pcie1Settings.RelaxedOrdering = SetupData->RelaxedOrdering;
			PciSetupData->Pcie1Settings.ExtTagField = SetupData->ExtTagField;
			PciSetupData->Pcie1Settings.NoSnoop = SetupData->NoSnoop;
			PciSetupData->Pcie1Settings.MaxPayload = SetupData->MaxPayload;
			PciSetupData->Pcie1Settings.MaxReadRequest = SetupData->MaxReadRequest;
			//PCI Express Link settings: [] - default
			PciSetupData->Pcie1Settings.AspmMode = SetupData->AspmMode;
			PciSetupData->Pcie1Settings.ExtendedSynch = SetupData->ExtendedSynch;

			//Fill in Default Values... (buffer was initialized with ZEROs)
			//UINT8   LnkTrRetry;             //[Disable]\ 2 \ 3 \ 5
			PciSetupData->Pcie1Settings.LnkTrRetry=SetupData->LnkTrRetry;
			PciSetupData->Pcie1Settings.LnkTrTimeout=SetupData->LnkTrTimeout;
			PciSetupData->Pcie1Settings.LnkDisable=SetupData->LnkDisable; //[Keep ON == 0] / Disable ==1
			//UINT8   ClockPm;               //[Disable]\ Enable
			PciSetupData->Pcie1Settings.ClockPm=SetupData->ClockPm;
			//UINT8 S3PciExpressScripts       //[Disable]\ Enable
			//PciSetupData->S3PciExpressScripts = SetupData->S3PciExpressScripts;

	//#endif

	//#if PCI_EXPRESS_GEN2_SUPPORT
			//Gen2 Device Settings
			//UINT8   ComplTimeOut;           //[Disable]\ Default \ 50 - 100 us \ 1ms - 10ms \ 16...
			PciSetupData->Pcie2Settings.ComplTimeOut=SetupData->ComplTimeOut;
			//UINT8   AriFwd;                 //[Disable]\ Enable
			PciSetupData->Pcie2Settings.AriFwd=SetupData->AriFwd;
			//UINT8   AtomOpReq;              //[Disable]\ Enable
			PciSetupData->Pcie2Settings.AtomOpReq=SetupData->AtomOpReq;
			//UINT8   AtomOpEgressBlk;        //[Disable]\ Enable
			PciSetupData->Pcie2Settings.AtomOpEgressBlk=SetupData->AtomOpEgressBlk;
			//UINT8   IDOReq;                 //[Disable]\ Enable
			PciSetupData->Pcie2Settings.IDOReq=SetupData->IDOReq;
			//UINT8   IDOCompl;               //[Disable]\ Enable
			PciSetupData->Pcie2Settings.IDOCompl=SetupData->IDOCompl;
			//UINT8   LtrReport;              //[Disable]\ Enable
			PciSetupData->Pcie2Settings.LtrReport=SetupData->LtrReport;
			//UINT8   E2ETlpPrBlk;            //[Disable]\ Enable
			PciSetupData->Pcie2Settings.E2ETlpPrBlk=SetupData->E2ETlpPrBlk;

			//Gen2 Link Settings
			//UINT8   LnkSpeed;               //[Auto]\ 5.0 GHz \ 2.5 GHz
			PciSetupData->Pcie2Settings.LnkSpeed=SetupData->LnkSpeed;
			//UINT8   ComplSos;               //[Disable]\ Enable
			PciSetupData->Pcie2Settings.ComplSos=SetupData->ComplSos;
			//UINT8   HwAutoWidth;            //[Enable]\ Disable
			PciSetupData->Pcie2Settings.HwAutoWidth=SetupData->HwAutoWidth;
			//UINT8   HwAutoSpeed;            //[Enable]\ Disable
			PciSetupData->Pcie2Settings.HwAutoSpeed=SetupData->HwAutoSpeed;
#if SMCPKG_SUPPORT
            PciSetupData->SmcPriorVgaBus=SetupData->SmcPriorVgaBus;
#endif
//#endif
		}
		if(HpSetup != NULL){
//#if AMI_HOTPLUG_INIT_SUPPORT
			HpSetup->ArrayField.BusPadd=(UINT64)SetupData->BusPadd;
			HpSetup->ArrayField.IoPadd=(UINT64)SetupData->IoPadd*0x400; //Stored in units of KB
			HpSetup->ArrayField.Mmio32Padd=(UINT64)SetupData->Mmio32Padd*0x100000;
			HpSetup->ArrayField.Mmio32PfPadd=(UINT64)SetupData->Mmio32PfPadd*0x100000;
			HpSetup->ArrayField.Mmio64Padd=(UINT64)SetupData->Mmio64Padd*0x100000;
			HpSetup->ArrayField.Mmio64PfPadd=(UINT64)SetupData->Mmio64PfPadd*0x100000;
//#endif
		}
    }
    pBS->FreePool(SetupData);
    return EFI_SUCCESS;
}
#else

EFI_STATUS AmiPciGetSetupData(	PCI_COMMON_SETUP_DATA 	*PciCommon,
								PCI_DEVICE_SETUP_DATA	*PciDev, 
								PCIE1_SETUP_DATA		*Pcie1,
								PCIE2_SETUP_DATA		*Pcie2,
								AMI_SDL_PCI_DEV_INFO    *DevSdlInfo,  	//OPTIONAL if ==NULL get defaults...
								UINTN					DevIdx)			//OPTIONAL 
{
    EFI_STATUS          	Status=EFI_SUCCESS;
//-------------------------------------

    if( PciCommon==NULL && PciDev==NULL && Pcie1==NULL && Pcie2==NULL) return EFI_INVALID_PARAMETER;
    
    //Get Setup Data
	if(PciCommon!=NULL) Status=AmiPciGetCommonSetupData(PciCommon);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;
	
	
	if(PciDev!=NULL)Status=AmiPciGetPciDevSetupData(PciDev,DevSdlInfo,DevIdx,FALSE);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;
	
	if(Pcie1!=NULL)Status=AmiPciGetPcie1SetupData(Pcie1,DevSdlInfo,DevIdx,FALSE);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status)) return Status;
	
	if(Pcie2!=NULL)Status=AmiPciGetPcie2SetupData(Pcie2,DevSdlInfo,DevIdx,FALSE);
	ASSERT_EFI_ERROR(Status);

	return Status;
}
#endif


EFI_STATUS LaunchPortingRoutine(PCI_DEV_INFO *Device, PCI_INIT_STEP InitStep, 
								VOID *Param1, VOID *Param2, VOID *Param3, VOID *Param4)
{
	EFI_STATUS	Status=EFI_UNSUPPORTED;
	EFI_STATUS	Status2=EFI_UNSUPPORTED;
//-----------------------
	if(Device->PciPortProtocol==NULL) return Status;

	switch (InitStep) {
		//(OEM_PCI_DEVICE_ATTRIBUTE_FUNCTION)(VOID *PciIoProtocol, UINT64 *Attr, UINT64 Capab, BOOLEAN Set);
		case isPciSetAttributes:
			Status=Device->PciPortProtocol->PciPortOemAttributes(
					Param1, 				//VOID *PciIoProtocol,		
					(UINT64*)Param2,		//UINT64 *Attr,
					*((UINT64*)Param3), 		//UINT64 Capab,
					*((BOOLEAN*)Param4)
					);
		break;
		
		//(OEM_PCI_DEVICE_GET_OPT_ROM_FUNCTION)(VOID *PciIoProtocol, VOID **OptRom, UINT64 *OptRomSize);
		case isPciGetOptionRom:
			Status=Device->PciPortProtocol->PciPortOemGetOptRom(
					Param1, 		//VOID *PciIoProtocol,		
					(VOID**)Param2,		//VOID **OptRom,
					(UINT64*)Param3 //UINT64 *OptRomSize
					);
		break;
		
		//(OEM_PCI_DEVICE_OUT_OF_RESOURCES_FUNCTION)(VOID *PciIoProtocol, UINTN Count, UINTN ResType);		
		case isPciOutOfResourcesCheck:
			Status=Device->PciPortProtocol->PciPortOutOfResourcesRemoveDevice(
					Param1, 			//VOID *PciIoProtocol,		
					*((UINTN*)Param2),	//UINTN Count,
					*((UINTN*)Param3) 	//UINTN ResType
					);
		break;
		
		//(OEM_PCI_PROGRAM_DEVICE_FUNCTION)(VOID *PciIoProtocol);
		case isPciSkipDevice:
			Status=Device->PciPortProtocol->PciPortSkipThisDevice(
					Param1 				//VOID *PciIoProtocol,		
					);
		break;
		
		//(OEM_PCI_PROGRAM_DEVICE_FUNCTION)(VOID *PciIoProtocol);
        case isPciProgramDevice:
            Status=Device->PciPortProtocol->PciPortOemProgDevice(
                    Param1              //VOID *PciIoProtocol,      
                    );
        break;

        //(OEM_PCI_PROGRAM_DEVICE_FUNCTION)(VOID *PciIoProtocol);
        //case isPciQueryDevice:
        //    Status=Device->PciPortProtocol->PciPortQueryPcieDevice(
        //            Param1              //VOID *PciIoProtocol,      
        //            );
        //break;
		
		//(OEM_PCI_PROGRAM_DEVICE_FUNCTION)(VOID *PciIoProtocol);
		case isPciGetSetupConfig:
//			EFI_DEADLOOP();		
			Status=Device->PciPortProtocol->PciPortCheckBadPcieDevice(
					Param1 				//VOID *PciIoProtocol,		
					);
			//In aptio V we don't have PciPortCheckBadPcieDevice 
			//so we will map it to the setup settings disabling PCI express initialization.
			if (!EFI_ERROR(Status)) Device->DevSetup.Pcie1Disable=TRUE;
		break;

		//(OEM_PCI_DEVICE_SET_ASPM_FUNCTION_PTR)(VOID *PciIoProtocol, VOID *AspmMode);
		case isPcieSetAspm:
			//In Aptio V set ASPM routine passes both Upstream and Downstream Ports,
			//Aptio4 must call porting hook twice 1 for UP and 1 for Down Stream 
			
			//this one for DN Stream Port 
			Status=Device->PciPortProtocol->PciPortOemSetAspm(
					Param1, 				//VOID *PciIoProtocol,		
					Param3					//VOID *AspmMode
					);
			
			//this one for UP Stream Port 
			Status2=Device->PciPortProtocol->PciPortOemSetAspm(
					Param2, 				//VOID *PciIoProtocol,		
					Param3					//VOID *AspmMode
					);
		break;

		//(OEM_PCI_DEVICE_SET_LNK_SPEED_FUNCTION_PTR)(VOID *PciIoProtocol, UINT8 *LnkSpeed, UINT8 SuppSpeeds);
		case isPcieSetLinkSpeed:
			//In aptio V set Lnk Speed routine passes both Upstream and Downstream Ports,
			//Aptio4 must call porting hook twice 1 for UP and 1 for Down Stream 
			
			//this one for DN Stream Port 
			Status=Device->PciPortProtocol->PciPortOemSetLnkSpeed(
					Param1, 				//VOID *PciIoProtocol,		
					(UINT8*)Param3,			//UINT8 *LnkSpeed
					*((UINT8*)Param4)		//UINT8 SuppSpeeds (max link speed)
					);
			
			//this one for UP Stream Port 
			Status2=Device->PciPortProtocol->PciPortOemSetLnkSpeed(
					Param2, 				//VOID *PciIoProtocol,		
					(UINT8*)Param3,			//UINT8 *LnkSpeed
					*((UINT8*)Param4)		//UINT8 SuppSpeeds (max link speed)
					);
			
		break;
		
		default: 
			break;
	} //switch...
	
	//UNSUPPORTED is a NORMAL condition meaning NO PORTING NEEDED FOR THIS DEVICE!
	if(EFI_ERROR(Status)&& Status!=EFI_UNSUPPORTED){
		ASSERT_EFI_ERROR(Status);
		return Status;
	}
	
	if(EFI_ERROR(Status2)&& Status2!=EFI_UNSUPPORTED){
		ASSERT_EFI_ERROR(Status2);
		return Status2;
	}
	
	//In this case possible status combination would be 
	//SUCCESS & UNSUPPORTED = SUCCESS or 
	//UNSUPPORTED & UNSUPPORTED = UNSUPPORTED.
	return (Status & Status2);
}


/**
    This is a routine to launch Init Function

    @param 

    @retval 

**/
EFI_STATUS AmiPciLibLaunchInitRoutine(VOID *RoutineOwner, PCI_INIT_STEP InitStep, PCI_INIT_TYPE InitType,
		VOID *Param1, VOID *Param2, VOID *Param3, VOID *Param4)
{
	AMI_BOARD_INIT_PARAMETER_BLOCK 	Parameters;
	EFI_STATUS						nStatus=EFI_UNSUPPORTED; //'n'ew Sdl Routines Status
    EFI_STATUS                      pStatus=EFI_UNSUPPORTED; //'p'orting Aptio Compatibility routine status
    EFI_STATUS                      cStatus=EFI_UNSUPPORTED; //'c'ommon rouinr status
    EFI_STATUS                      mStatus=EFI_UNSUPPORTED; //'m'ultipe new SDL Routines delivered through container object.
    EFI_STATUS                      Status=EFI_UNSUPPORTED;  //yet one more for multipe SDL Routines from container object.
    AMI_BOARD_INIT_PROTOCOL			*InitProtocol=NULL; //RoutineOwner->PciInitProtocol;
	UINTN							InitFunction;       //=RoutineOwner->AmiSdlPciDevData->InitRoutine;
	AMI_SDL_PCI_DEV_INFO			*AmiSdlPciDevData=NULL;
	UINTN							*idx=NULL;
	UINTN							cnt, di, i;
//------------------------
	switch(InitType){
		case itHost:
			InitProtocol=((PCI_HOST_BRG_DATA*)RoutineOwner)->PciInitProtocol;
			AmiSdlPciDevData=((PCI_HOST_BRG_DATA*)RoutineOwner)->HbSdlData;
			di=((PCI_HOST_BRG_DATA*)RoutineOwner)->HbSdlIndex;
			cnt=1; //host can not have a container object....
		break;
			
		case itRoot:
			InitProtocol=((PCI_ROOT_BRG_DATA*)RoutineOwner)->PciInitProtocol;
			AmiSdlPciDevData=((PCI_ROOT_BRG_DATA*)RoutineOwner)->RbSdlData;
			di=((PCI_ROOT_BRG_DATA*)RoutineOwner)->RbSdlIndex;
			cnt=1; //root can not have a container object....
		break;
		
		case itDevice:
			InitProtocol=((PCI_DEV_INFO*)RoutineOwner)->PciInitProtocol;
			AmiSdlPciDevData=((PCI_DEV_INFO*)RoutineOwner)->AmiSdlPciDevData;
			di=((PCI_DEV_INFO*)RoutineOwner)->SdlDevIndex;
			cnt=((PCI_DEV_INFO*)RoutineOwner)->SdlDevCount;
			if(cnt>1) idx=((PCI_DEV_INFO*)RoutineOwner)->SdlIdxArray;
		break;
		
		default: return EFI_INVALID_PARAMETER;
	}
	
	if(InitProtocol==NULL) return EFI_UNSUPPORTED;
	
	if ((LaunchInitRoutinePriority==lpAptio4x) && (InitType==itDevice) && (((PCI_DEV_INFO*)RoutineOwner)->PciPortProtocol!=NULL)){
		PCI_TRACE((TRACE_PCI,"\nPciPort: PciCompatibility(Prior=%d; InitStep=%d; SdlIndex=%d(0x%X);",
				LaunchInitRoutinePriority,InitStep, di, di));
		pStatus=LaunchPortingRoutine((PCI_DEV_INFO*)RoutineOwner, InitStep, Param1,Param2,Param3,Param4);
		PCI_TRACE((TRACE_PCI,")=%r\n",pStatus));
		if(EFI_ERROR(pStatus)){
			if(pStatus!=EFI_UNSUPPORTED) 
				ASSERT_EFI_ERROR(pStatus);
		}
	}
	
    //For Common Init Routine dispatch, Device might not have SDL data.
    if(LaunchCommonRoutineFirst){
        InitFunction=0;
        
        //Init parameter block each time - the init routine might corrupt them...
        Parameters.Signature=AMI_PCI_PARAM_SIG;
        Parameters.InitStep=InitStep;
        Parameters.Param1=Param1;
        Parameters.Param2=Param2;
        Parameters.Param3=Param3;
        Parameters.Param4=Param4;

        PCI_TRACE((TRACE_PCI,"\nPciInit: PciInitProtocol->Function[CmnFn](InitStep=%d; SdlIndex=%d(0x%X); SdlCnt=%d; PciDevice",
                InitStep, di, di, cnt));
        cStatus=InitProtocol->Functions[InitFunction](InitProtocol,&InitFunction, &Parameters);
        PCI_TRACE((TRACE_PCI,")=%r\n",cStatus));
        
        if(EFI_ERROR(nStatus)){
            if(cStatus!=EFI_UNSUPPORTED) ASSERT_EFI_ERROR(cStatus);
        }
    }
    
	//First Check if device has SDL data and InitRoutine...
	if(AmiSdlPciDevData!= NULL && AmiSdlPciDevData->InitRoutine!=0){
		InitFunction=AmiSdlPciDevData->InitRoutine;
		if(InitProtocol->FunctionCount < InitFunction) {
			PCI_TRACE((TRACE_PCI,"\nPciInit: InitProtocol->FunctionCount[%d] < InitFunction (%d)! EFI_INVALID_PARAMETER\n", InitProtocol->FunctionCount, InitFunction));
			return EFI_INVALID_PARAMETER;
		}
			
        //Init parameter block each time - the init routine might corrupt them...
        Parameters.Signature=AMI_PCI_PARAM_SIG;
        Parameters.InitStep=InitStep;
        Parameters.Param1=Param1;
        Parameters.Param2=Param2;
        Parameters.Param3=Param3;
        Parameters.Param4=Param4;

		PCI_TRACE((TRACE_PCI,"\nPciInit: PciInitProtocol->Function[%d](InitStep=%d; SdlIndex=%d(0x%X); SdlCnt=%d; PciDevice",
				InitFunction, InitStep, di, di, cnt));
		nStatus=InitProtocol->Functions[InitFunction](InitProtocol,&InitFunction, &Parameters);
		PCI_TRACE((TRACE_PCI,")=%r\n",nStatus));
	} 

	if(idx!=NULL){
		for(i=0; i<cnt; i++){
			if(idx[i]==di) continue; //Routine for the Main device was launched already.
			
			//if we have more than one init routine delivered through a Container object...
			Status=AmiSdlFindIndexRecord(idx[i],&AmiSdlPciDevData);
			ASSERT_EFI_ERROR(Status);
			
			if(AmiSdlPciDevData!= NULL && AmiSdlPciDevData->InitRoutine!=0){ 
				InitFunction=AmiSdlPciDevData->InitRoutine;
				if(InitProtocol->FunctionCount < InitFunction) {
					PCI_TRACE((TRACE_PCI,"\nPciInit: InitProtocol->FunctionCount[%d] < InitFunction (%d)!=EFI_INVALID_PARAMETER\n", 
							InitProtocol->FunctionCount, InitFunction));
					return EFI_INVALID_PARAMETER;
				}
			
				Parameters.Signature=AMI_PCI_PARAM_SIG;
				Parameters.InitStep=InitStep;
				Parameters.Param1=Param1;
				Parameters.Param2=Param2;
				Parameters.Param3=Param3;
				Parameters.Param4=Param4;
			
				PCI_TRACE((TRACE_PCI,"\nPciInit: PciInitProtocol->Function[%d](InitStep=%d; SdlIndex=%d(0x%X); Container[%d]",
						InitFunction, InitStep, idx[i], idx[i], i));
				Status=InitProtocol->Functions[InitFunction](InitProtocol,&InitFunction, &Parameters);
				PCI_TRACE((TRACE_PCI,")=%r\n\n",nStatus));
				if(Status!=EFI_UNSUPPORTED) mStatus=Status;
			} //else nStatus=EFI_UNSUPPORTED;

			if(EFI_ERROR(mStatus)){
	            if(mStatus!=EFI_UNSUPPORTED) ASSERT_EFI_ERROR(mStatus);
	        }
		}	
	}
		
    if(!LaunchCommonRoutineFirst){
        InitFunction=0;
        
        Parameters.Signature=AMI_PCI_PARAM_SIG;
        Parameters.InitStep=InitStep;
        Parameters.Param1=Param1;
        Parameters.Param2=Param2;
        Parameters.Param3=Param3;
        Parameters.Param4=Param4;

        PCI_TRACE((TRACE_PCI,"\nPciInit: PciInitProtocol->Function[CmnFn](InitStep=%d; SdlIndex=%d(0x%X); SdlCnt=%d; PciDevice)",
                InitStep, di, di, cnt));
        cStatus=InitProtocol->Functions[InitFunction](InitProtocol,&InitFunction, &Parameters);
        PCI_TRACE((TRACE_PCI,")=%r\n",cStatus));

        if(EFI_ERROR(cStatus)){
            if(cStatus!=EFI_UNSUPPORTED) ASSERT_EFI_ERROR(cStatus);
        }
    }

	if ((LaunchInitRoutinePriority==lpAptioV) && (InitType==itDevice) && (((PCI_DEV_INFO*)RoutineOwner)->PciPortProtocol!=NULL)){
		PCI_TRACE((TRACE_PCI,"\nPciPort: PciCompatibility(Prior=%d; InitStep=%d; SdlIndex=%d(0x%X);",
				LaunchInitRoutinePriority, InitStep, di, di));
		pStatus=LaunchPortingRoutine((PCI_DEV_INFO*)RoutineOwner, InitStep, Param1,Param2,Param3,Param4);
		PCI_TRACE((TRACE_PCI,")=%r\n",pStatus));
		if(EFI_ERROR(pStatus)){
			if(pStatus!=EFI_UNSUPPORTED) ASSERT_EFI_ERROR(pStatus);
		}
	}
	
	//One of routines changed something ....
	if((pStatus==EFI_SUCCESS)||(nStatus==EFI_SUCCESS)||(cStatus==EFI_SUCCESS)||(mStatus==EFI_SUCCESS)) return EFI_SUCCESS;
	
	//Nothing has changed....all set to UNSUPPORTED
    if((pStatus & nStatus & cStatus & mStatus) == EFI_UNSUPPORTED ) return EFI_UNSUPPORTED;
    
    //In case of ERROR! which ever comes first
    if(EFI_ERROR(nStatus) && (nStatus!=EFI_UNSUPPORTED)) return nStatus;
    if(EFI_ERROR(pStatus) && (pStatus!=EFI_UNSUPPORTED)) return pStatus;
    if(EFI_ERROR(cStatus) && (cStatus!=EFI_UNSUPPORTED)) return cStatus;
       
    return mStatus;     
}

BOOLEAN PciBusCheckIfPresent(PCI_DEV_INFO *Dev, AMI_SDL_PCI_DEV_INFO *DevFnData){
	PCI_CFG_ADDR 	addr;
	UINT32 			val=0;
	EFI_STATUS		Status;
//-------------------------------
	// Always report virtual devices as present
	if (DevFnData->PciDevFlags.Bits.Virtual) return TRUE;

	addr.ADDR=Dev->Address.ADDR;
	addr.Addr.Device=DevFnData->Device;
	addr.Addr.Function=DevFnData->Function;
	addr.Addr.ExtendedRegister=0;
	addr.Addr.Register=0; //will read vid did register

	// For root bridges, get bus from SDL
	if (DevFnData->PciDevFlags.Bits.RootBridgeType)	addr.Addr.Bus=DevFnData->Bus;

	Status=PciCfg32(Dev->RbIo, addr, FALSE, &val);
	ASSERT_EFI_ERROR(Status);
	
    if(val==0xFFFFFFFF || val==0 ) return FALSE;
    else  return TRUE;
}

//PciBus BusShift variable format....
#define PCI_BUS_SHIFT_VAR_NAME_FORMAT 	L"H[%d]R[%d](%a)BSH"

EFI_STATUS AmiPciBusShift(			AMI_SDL_PCI_DEV_INFO 	*DevSdlInfo,
						  	  	  	INT16					*ShiftValue,	
						  	  	  	UINTN					HostIdx,
									UINTN					RootIdx,
						  	  	  	BOOLEAN					Set)
{
    EFI_STATUS          	Status=EFI_SUCCESS;
    UINTN               	vsz;
    CHAR16 					vname[40];
//------------------------------------------------------
	//Get Device Conventional PCI  setup data variable
	vsz=sizeof(INT16);
	pBS->SetMem(&vname[0], sizeof(vname), 0);
	Swprintf(&vname[0], PCI_BUS_SHIFT_VAR_NAME_FORMAT, HostIdx, RootIdx, DevSdlInfo->AslName);

	if(Set) {
		Status=pRS->SetVariable(vname, &gPciSetupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, vsz, ShiftValue);
	} else {
		Status=pRS->GetVariable(vname, &gPciSetupGuid, NULL, &vsz, ShiftValue);
	}

	return Status;
}

/**
    Check if System Uses Fixed resource allocation. 

    @param VOID

    @retval EFI_SUCCESS 	if Fixed Resource Allocation used in the system
        EFI_NOT_FOUND   if NOT

**/
EFI_STATUS AmiPciCheckFixedResources(){
	EFI_STATUS				Status;
	UINTN					i;
	AMI_SDL_PCI_DEV_INFO	*sdldev;
//-----------------------------------	
	Status=AmiSdlInitBoardInfo();
	if(EFI_ERROR(Status)) return Status;

	for(i=0; i<gSdlPciData->PciDevCount; i++){
		sdldev=&gSdlPciData->PciDevices[i];
		if(sdldev->PciDevFlags.Bits.FixedResources) {
			return EFI_SUCCESS;
		}
	}
	
	return EFI_NOT_FOUND;
}

/**
Tests if region1(s-e) belongs to the region2(S-E)
    @retval 
        EFI_ACCESS_DENIED if Belongs=FALSE and region 1 belong to region 2
        EFI_ACCESS_DENIED if Belongs=TRUE and region 1 don't belong to region 2
        EFI_SUCCESS in all other cases.

**/
EFI_STATUS AmiPciTestRegions(UINT64 s, UINT64 e, UINT64 S, UINT64 E, BOOLEAN Belongs)
{
	BOOLEAN		Condition;
	EFI_STATUS  Status;
//------------------------
	//this condition is TRUE if s--e region falling into S--E region 
	Condition=((s >= S) && (e <= E)); 
	if(Belongs) Status=Condition ? EFI_SUCCESS		 : EFI_ACCESS_DENIED;
	else		Status=Condition ? EFI_ACCESS_DENIED : EFI_SUCCESS;
	return Status; 
}


EFI_STATUS  AmiPciUpdateBarFixedRes(PCI_BAR *FixedBar){
    UINTN               i;
    PCI_DEV_INFO        *dev;
    AMI_SDL_PCI_FIX_BAR *fb;
    BOOLEAN             found=FALSE;
    PCI_BAR             savebar=*FixedBar;
//------------------------
       
    dev=FixedBar->Owner;
    if(dev->AmiSdlPciDevData==NULL) return EFI_NOT_FOUND;
    
    //There are 7 SDL BARs (including the ROM BAR)
    for(i=0; i<PCI_MAX_BAR_NO+1; i++){
        fb=&dev->AmiSdlPciDevData->FixedResources[i];
        if(fb->BarType==0) continue;
        //We found match...
        if(FixedBar->Offset==fb->BarOffset){
            found=TRUE;
            break;
        }
    }

    //Now check and verify and Update BAR properties between Fixed and Discovered one.
    if(found){
        //Fixed res flag SET in FindSdlEntry routine.
        PCI_TRACE((TRACE_PCI,"Fixed BAR Found: Offs=0x%X; Base=0x%lX; ", 
                FixedBar->Offset, fb->BarBase));
        FixedBar->Base=fb->BarBase;
        FixedBar->Fixed=TRUE;

        if(!IsPowerOfTwo(fb->BarLength)){
            PCI_TRACE((TRACE_PCI,"\n  !!!Fixed BAR Length is NOT ^2 !!! SDL 0x%lX != 0x%lX H/W; ",  
                    fb->BarLength, FixedBar->Length));
        }

        //Check if BAR Length Corresponds to discovered one
        if(FixedBar->Length != fb->BarLength){
            PCI_TRACE((TRACE_PCI,"\n  !!!Fixed BAR Length mismatch!!! SDL 0x%lX != 0x%lX H/W; ",  
                    fb->BarLength, FixedBar->Length));
            if(fb->BarLength > FixedBar->Length)FixedBar->Length=fb->BarLength;
            PCI_TRACE((TRACE_PCI,"(0x%lX used) ",FixedBar->Length)); 
            //update BAR Granularity value to match new length 
            FixedBar->Gran=FixedBar->Length-1;
        } else PCI_TRACE((TRACE_PCI,"Len=0x%lX; ",FixedBar->Length)); 
        
        //Check if BAR Length Corresponds to discovered one
        if(FixedBar->Type != (PCI_BAR_TYPE)(fb->BarType+1)){
            PCI_TRACE((TRACE_PCI,"\n  !!!Fixed BAR  Type  mismatch!!! SDL 0x%X != 0x%X H/W;(0x%X used)\n",  
                    fb->BarType+1, FixedBar->Type, FixedBar->Type));
        } else PCI_TRACE((TRACE_PCI,"Type=0x%X.\n",FixedBar->Type)); 
        
        //Now Check if Proposed Address Correctly Alligned...
        if((FixedBar->Base & (~FixedBar->Gran)) != FixedBar->Base){
            PCI_TRACE((TRACE_PCI,"\n  !!!Proposed Address is NOT ALIGNED!!!: Base=0x%lX; Align=0x%lX!!!\n  !!!Restoring BAR to Discovered State!!!\n", 
                    FixedBar->Base, FixedBar->Gran ));
            ASSERT_EFI_ERROR(EFI_NOT_FOUND);
            
            //if it is not allign try to save situation by resetting fixed resources request and use AUTOMATED one.
            *FixedBar=savebar;
            return EFI_NOT_FOUND;
        }
        
    } else {
        PCI_TRACE((TRACE_PCI,"\n  !!!Filed to locate Fixed BAR in SDL Data: Offset=0x%X!!!\n", FixedBar->Offset));
        return EFI_NOT_FOUND;
    }
    return EFI_SUCCESS;
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

