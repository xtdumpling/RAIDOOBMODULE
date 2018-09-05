//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Fix the IO-APIC ID assign error issue.
//    Reason:   The IO-APIC ID assign error will cause the "IOAPIC not found!" error message in some OS.
//    Auditor:  Jacker Yeh (Refer from Grantley)
//    Date:     Oct/27/2016
//
//***************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file mptable.c
    MPS table builder functions implementation

**/
#define F0000_BIT       1//LegacyBios.h

#include <Token.h>

//------------------------ Include files ------------------------------

#include <AmiDxeLib.h>

#include <Protocol/LegacyBios.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Library/AmiSdlLib.h>
#include <Library/AmiAcpiCpuLib.h>


#include "AcpiCore.h"
#include "mptable.h"

//------------------------ Global variables ---------------------------

static EFI_GUID gMpsTableGuid = MPS_TABLE_GUID;
static EFI_GUID EfiLegacyBiosProtocolGuid = EFI_LEGACY_BIOS_PROTOCOL_GUID;
static EFI_EVENT MpsTableReadyToBootEvent = 0;
static MP_FLOATING_POINTER MpsTableFloatingPointer;
static VOID *MpsTablePointer = NULL;
static VOID *MpsTableCurrentPointer = NULL;
static VOID *MpsExTablePointer = NULL;
static VOID *MpsExTableCurrentPointer = NULL;
static UINT32  BaseTableLength = 0;
static UINT16  BaseTableEntryCount = 0;
static UINT16  ExtendedTableLength = 0;
static UINT8   OemId[8] = CONVERT_TO_STRING(T_ACPI_OEM_TBL_ID);   //{ 0,1,2,3,4,5,6,7 };
static UINT8   ProductId[12] = CONVERT_TO_STRING(T_ACPI_OEM_ID);   //{ 0,1,2,3,4,5,6,7,8,9,0xA, 0xB };

static BUS_INFO BusEntry[MP_TABLE_MAX_BUS_ENTRIES];
static UINT8    MaxBusId = 0;
static UINT8    CurrentBusEntry = 0;
static UINT8    IsaBusId = 0;
static UINT8    *BusTypeString[] = {"CBUS  ", "CBUSII", "EISA  ", "FUTURE", "INTERN", "ISA   ",
                                    "MBI   ", "MBII  ", "MCA   ", "MPI   ", "MPSA  ", "NUBUS ",
                                    "PCI   ", "PCMCIA", "TC    ", "VL    ", "VME   ", "XPRESS" };
static UINT8    BridgeBusTypes[] = { 12, 5, 2, 8, 12, 13, 11 }; //each number represents corresponding
                                                                //number in BusTypeString array, i.e.
                                                                //bridge subclass 0 - PCI host has number
                                                                //12 in array, subclass 1 - ISA bridge -
                                                                //number 5 etc.
static IO_APIC_INFO IoApicEntry[MP_TABLE_MAX_IO_APIC_ENTRIES];
static UINT8    CurrentIoApicEntry = 0;

extern ISO_PARAMETER_TABLE IsoTbl[];

EFI_STATUS MpsTableCreateStatus =  EFI_NO_RESPONSE;
EFI_STATUS MpsTableFloatPointerModifyStatus = EFI_NO_RESPONSE;
BOOLEAN MpsTableWasCreated = FALSE;

//------------------------ Functions ----------------------------------

/**
    MPS table builder entry point 

        
    @param ImageHandle MPS table builder image handle
    @param SystemTable pointer to system table
               
         
    @retval EFI_SUCCESS Initialization completed successfully
    @retval EFI_ERROR Initialization failed

**/

EFI_STATUS MpsTableBuilderInit(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;

	InitAmiLib(ImageHandle,SystemTable);

    Status = CreateReadyToBootEvent(TPL_CALLBACK,
                                    MpsTableReadyToBootNotify,
                                    NULL,
                                    &MpsTableReadyToBootEvent);
	return Status;
}

/**
    MPS table builder ReadyToBoot notification function 

        
    @param Event signalled event
    @param Context pointer to calling context
    
                    
    @retval VOID

**/

VOID MpsTableReadyToBootNotify(
	IN EFI_EVENT Event, 
    IN VOID *Context)
{
    EFI_STATUS Status;
    
    if(EFI_ERROR(MpsTableCreateStatus)) 
    {
        MpsTableCreateStatus = MpsTableCreate();
        if(EFI_ERROR(MpsTableCreateStatus))
            return;
    }
    if(EFI_ERROR(MpsTableFloatPointerModifyStatus)) 
        MpsTableFloatPointerModifyStatus = MpsTableFloatPointerModify();

    //Install a Configuration table  Entry
    Status=pBS->InstallConfigurationTable(&gMpsTableGuid,MpsTablePointer);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return;
    //After we did everything needed 
    pBS->CloseEvent(Event);

    return;
}

/**
    Fills MP_FLOATING_POINTER structure with actual data
    and places it into F000 segment 

        
    @param VOID
            
            
    @retval EFI_SUCCESS Pointer created and placed successfully
    @retval EFI_ERROR Some error occurs

	@note Modifies MpsTableFloatingPointer
**/

EFI_STATUS MpsTableFloatPointerModify(VOID)
{
    EFI_STATUS Status;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBiosProtocol;
    VOID *LegacyRegionAddress;

    MpsTableFloatingPointer.Signature = MP_FLOATING_POINTER_SIGNATURE;

//at this point we have address of MPS table
    MpsTableFloatingPointer.PhysicalAddresPointer = (UINT32) MpsTablePointer;
 
//length in 16-byte paragraphs  
    MpsTableFloatingPointer.Length = sizeof(MP_FLOATING_POINTER)/16;  

//here must be token  
    MpsTableFloatingPointer.VersionNumber = MP_TABLE_VERSION_1_4;

//force checksum to zero before calculate actual checksum
    MpsTableFloatingPointer.CheckSum = 0;

//here must be token
    MpsTableFloatingPointer.DefaultConfiguration = 0;

//here must be token
    MpsTableFloatingPointer.IMCRflag = 0;

    MpsTableFloatingPointer.Reserved[0] = 0;
    MpsTableFloatingPointer.Reserved[1] = 0;
    MpsTableFloatingPointer.Reserved[2] = 0;

//now we are ready to calculate checksum
    MpsTableFloatingPointer.CheckSum = ChsumTbl((UINT8 *)&MpsTableFloatingPointer,
                                                sizeof(MP_FLOATING_POINTER));

    Status = pBS->LocateProtocol(&EfiLegacyBiosProtocolGuid,
                                 NULL,
                                 &LegacyBiosProtocol);
    if(EFI_ERROR(Status))
        return Status;

    Status = LegacyBiosProtocol->GetLegacyRegion(LegacyBiosProtocol,
                                        (UINTN)sizeof(MP_FLOATING_POINTER),
                                        F0000_BIT,
                                        0x10,
                                        &LegacyRegionAddress);
    if(EFI_ERROR(Status))
        return Status;

    Status = LegacyBiosProtocol->CopyLegacyRegion(LegacyBiosProtocol,
                                        (UINTN) sizeof(MP_FLOATING_POINTER),
                                        LegacyRegionAddress,
                                        (VOID *) &MpsTableFloatingPointer);
    return Status;
}

/**
    Allocates space and creates MPS table 

        
    @param VOID
               
         
    @retval EFI_SUCCESS function executed successfully
    @retval EFI_ERROR some error occured

**/
EFI_STATUS MpsTableCreate(VOID)
{
    EFI_STATUS Status;
    UINT32 EntryCount;
    VOID *CpuMpTableEntries;
    UINT32 CpuMpTableSize;
    
    if (MpsTableWasCreated)
        return MpsTableAddHeader();    

    Status = pBS->AllocatePool(EfiBootServicesData,
                               MP_TBL_TMP_BUFFER_SIZE,
                               &MpsTablePointer);
    if(EFI_ERROR(Status))
        return Status;

    MpsTableCurrentPointer = MpsTablePointer;

//split buffer onto two parts for base and extended tables
    MpsExTablePointer = MpsTablePointer;
    MpsExTablePointer = (UINT8 *)MpsExTablePointer + ((MP_TBL_TMP_BUFFER_SIZE)/2);
    MpsExTableCurrentPointer = MpsExTablePointer;

//Leave placeholder for header
    MpsTableCurrentPointer=(UINT8 *)MpsTableCurrentPointer + sizeof(MP_CONFIGURATION_TABLE_HEADER);
    BaseTableLength += sizeof(MP_CONFIGURATION_TABLE_HEADER);

//add CPU Entry
    Status =  CreateCpuMpTableEntries(&CpuMpTableEntries, &EntryCount, &CpuMpTableSize);
    if (!EFI_ERROR(Status))  {
        MemCpy(MpsTableCurrentPointer, CpuMpTableEntries, CpuMpTableSize);
        BaseTableLength += sizeof(MP_TABLE_CPU_ENTRY) * EntryCount;
        BaseTableEntryCount += EntryCount;
        MpsTableCurrentPointer=(UINT8*)MpsTableCurrentPointer + sizeof(MP_TABLE_CPU_ENTRY) * EntryCount;
        pBS->FreePool(CpuMpTableEntries);
    }

//add Bus Entry
    EntryCount = MpsTableAddBusEntry();
    MpsTableCurrentPointer=(UINT8 *)MpsTableCurrentPointer + sizeof(MP_TABLE_BUS_ENTRY) * EntryCount;
    BaseTableLength += sizeof(MP_TABLE_BUS_ENTRY) * EntryCount;
    BaseTableEntryCount += EntryCount;

//add IO Apic Entry
    EntryCount = MpsTableAddIoApicEntry();
    MpsTableCurrentPointer =(UINT8 *)MpsTableCurrentPointer + sizeof(MP_TABLE_IO_APIC_ENTRY) * EntryCount;
    BaseTableLength += sizeof(MP_TABLE_IO_APIC_ENTRY) * EntryCount;
    BaseTableEntryCount += EntryCount;

//add Interrupt assignment Entry
    EntryCount = MpsTableAddIntAssignEntry();
    MpsTableCurrentPointer =(UINT8 *)MpsTableCurrentPointer + sizeof(MP_TABLE_INT_ASSIGN_ENTRY) * EntryCount;
    BaseTableLength += sizeof(MP_TABLE_INT_ASSIGN_ENTRY) * EntryCount;
    BaseTableEntryCount += EntryCount;

//add Local interrupt assignment Entry
    EntryCount = MpsTableAddLocalIntAssignEntry();
    MpsTableCurrentPointer =(UINT8 *)MpsTableCurrentPointer + sizeof(MP_TABLE_LOCAL_INT_ASSIGN_ENTRY) * EntryCount;
    BaseTableLength += sizeof(MP_TABLE_LOCAL_INT_ASSIGN_ENTRY) * EntryCount;
    BaseTableEntryCount += EntryCount;

    MpsTableAddExtendedTable();

    MpsTableWasCreated = TRUE;
    
    return MpsTableAddHeader();
}

/**
    Creates MPS table header, allocates space and copies entire
    table into RuntimeServicesData memory

        
    @param VOID
               
         
    @retval EFI_SUCCESS function executed successfully

	@note Modifies MpsTablePointer
	
**/
EFI_STATUS MpsTableAddHeader(VOID)
{
    EFI_STATUS Status;
    MP_CONFIGURATION_TABLE_HEADER *HeaderPointer;
    VOID *AllocationPointer;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBiosProtocol=NULL;

    HeaderPointer = (MP_CONFIGURATION_TABLE_HEADER *) MpsTablePointer;

    HeaderPointer->Signature = MP_CONFIGURATION_TABLE_SIGNATURE;
    HeaderPointer->BaseTableLength = BaseTableLength;
    HeaderPointer->VersionNumber = MP_TABLE_VERSION_1_4;
    HeaderPointer->CheckSum = 0;    //force checksum to zero before calculating

    MemCpy((VOID *)HeaderPointer->OemId, (VOID *)OemId, 8);
    MemCpy((VOID *)HeaderPointer->ProductId, (VOID *)ProductId, 12);

    HeaderPointer->OemTablePointer = 0;
    HeaderPointer->OemTableSize = 0;
    HeaderPointer->BaseTableEntryCount = BaseTableEntryCount;
    HeaderPointer->LocalApicAddress = LOCAL_APIC_BASE;                //sdl token
    HeaderPointer->ExtendedTableLength = ExtendedTableLength;
    HeaderPointer->ExtendedCheckSum = 0;
    HeaderPointer->Reserved = 0;

    HeaderPointer->ExtendedCheckSum = ChsumTbl(MpsExTablePointer, ExtendedTableLength);
    HeaderPointer->CheckSum = ChsumTbl(MpsTablePointer, BaseTableLength);

#if MP_TABLE_LEGACY_REGION_LOCATION == 0

//publish table in high memory
    Status = pBS->AllocatePool(EfiRuntimeServicesData,
                               BaseTableLength + ExtendedTableLength,
                               &AllocationPointer);

    if(EFI_ERROR(Status))  
        return Status; 
    MemCpy(AllocationPointer, //memory allocated - copy MPS table to new location
            MpsTablePointer, 
            (UINTN) (BaseTableLength + ExtendedTableLength));
    
#else

//publish table in F0000 segment
    Status = pBS->LocateProtocol(&EfiLegacyBiosProtocolGuid,
                                 NULL,
                                 &LegacyBiosProtocol);
    if(EFI_ERROR(Status) || (LegacyBiosProtocol == NULL))
        return Status;

    Status = LegacyBiosProtocol->GetLegacyRegion(LegacyBiosProtocol,
                                        (UINTN)(BaseTableLength + ExtendedTableLength),
                                        F0000_BIT,
                                        0x10,
                                        &AllocationPointer);
    if(EFI_ERROR(Status))
        return Status;

    Status = LegacyBiosProtocol->CopyLegacyRegion(LegacyBiosProtocol,
                                        (UINTN) (BaseTableLength + ExtendedTableLength),
                                        AllocationPointer,
                                        MpsTablePointer);
#endif

    pBS->FreePool(MpsTablePointer);
    MpsTablePointer = AllocationPointer;

    return Status;
}

/**
    Adds Bus entries to MPS table

        
    @param VOID
     
                   
    @retval Number of entries added
 
**/
UINT16 MpsTableAddBusEntry(VOID)
{
    MP_TABLE_BUS_ENTRY *EntryPointer;
    UINT16 EntryCount = 0;
    UINT16 i;

    EntryPointer = (MP_TABLE_BUS_ENTRY *)MpsTableCurrentPointer;

    QueryBusInfo();

    for(i = 0; i < CurrentBusEntry; i++)
    {
        EntryPointer->EntryType = MP_TABLE_BUS_ENTRY_TYPE;
        EntryPointer->BusId = BusEntry[i].BusId;

        MemCpy((VOID *)EntryPointer->BusTypeString, (VOID *)BusTypeString[(BusEntry[i].BusType)], 6);

        EntryPointer++;
        EntryCount++;
    }
    return EntryCount;
}

/**
    Adds IO Apic entries to MPS table

        
    @param VOID
     
                   
    @retval Number of entries added
 
**/
UINT16 MpsTableAddIoApicEntry(VOID)
{
    MP_TABLE_IO_APIC_ENTRY *EntryPointer;
    UINT16 EntryCount = 0;
	UINT64 SearchAddress=0;
    UINT8 i=0;


    EntryPointer = (MP_TABLE_IO_APIC_ENTRY *)MpsTableCurrentPointer;

#if FEC00000_APIC_AUTODETECT == 1  
//first we detect IO APICS mapped into default address range
	for(SearchAddress = IO_APIC_BASE_ADDRESS_BOTTOM;
	    SearchAddress < IO_APIC_BASE_ADDRESS_TOP;
	    SearchAddress += IO_APIC_SEARCH_STEP)

		if(*((UINT8 *)SearchAddress) != 0xFF)             //we have something
            CollectIoApicInfo(NULL, (UINT32)SearchAddress);

//second we arrange IO APIC in their ID ascending order (if there more than one)
#endif


//Check also SDL ceated Structures they might address very none standard IOAPICS.
//-------------------------------------------------------------------------------------------
#if USE_BOARD_INFO_APIC_DATA == 1
{
    AMI_APIC_INFO   *BrdApicInfo;
    BOOLEAN         Present;  
    UINTN           j; 
    //-----------------        
    for(i=0; i<(gAmiBoardInfo2Protocol->ApicInfoLength/sizeof(AMI_APIC_INFO)); i++){
        BrdApicInfo=&gAmiBoardInfo2Protocol->ApicInfoTable[i];
        //This is a special case it could be a duplicate entries in MadtTblEntries[]
        //if other options of IOAPIC/IOsAPIC detection was on. 
        //So check if IOAPIC/IOsAPIC entries with the same properties already present in DB.
        for(j=0,Present=FALSE; j < CurrentIoApicEntry; j++){
            if(BrdApicInfo->ApicAddress.ADDRESS == IoApicEntry[j].IoApicAddress){
                Present = TRUE;
                break;
            } 
        }
        //Entry with this address already present in IoApicEntry[] array
        if(Present) continue;
        
        //Looks like it is a new IOAPIC/IOSAPIC entry!
        //Check if something alive at this address
        CollectIoApicInfo(NULL, BrdApicInfo->ApicAddress.ADDRESS);
    } // for(...i<(gAmiBoardInfoProtocol->ApicInfoLength/sizeof(AMI_APIC_INFO))
}
#endif //AmiBoardInfo_SUPPORT != 0
//-------------------------------------------------------------------------------------------

    if(CurrentIoApicEntry > 1)
        ArrangeIoApicEntries();


//now we are ready to create MPS table IO APIC entries
    for(i = 0; i < CurrentIoApicEntry; i++)
    {
        EntryPointer->EntryType = MP_TABLE_IO_APIC_ENTRY_TYPE;
		EntryPointer->IoApicId = IoApicEntry[i].IoApicId;
		EntryPointer->IoApicVersion = IoApicEntry[i].IoApicVersion;
        EntryPointer->Flags = IoApicEntry[i].IoApicFlags;
        EntryPointer->IoApicAddress = IoApicEntry[i].IoApicAddress;

        EntryPointer++;
        EntryCount++;
    }
    return EntryCount;
}

/**
    Adds Interrupt assignment entries to MPS table

        
    @param VOID
     
                   
    @retval UINT16 Number of entries added
 
**/
UINT16 MpsTableAddIntAssignEntry(VOID)
{
    MP_TABLE_INT_ASSIGN_ENTRY *EntryPointer;
    UINT16 EntryCount = 0;
    UINT16 Dummy;
    PCI_IRQ_APIC_ROUTE *RouteTable;
    UINT8   i, j;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    EFI_STATUS Status;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.

    EntryPointer = (MP_TABLE_INT_ASSIGN_ENTRY *)MpsTableCurrentPointer;

//First assign ISA IRQ interrupts
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    Status = AmiIsaIrqMask(&Dummy, TRUE);
    ASSERT_EFI_ERROR(Status);
    
    if(EFI_ERROR(Status)) 
    	return EntryCount;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.

    for(i = 0; i < 16; i++)
    {
        if((Dummy & 1) && (i != 2))    //create entry only if IRQ is used
        {                                   //and it isn't IRQ2
            EntryPointer->EntryType = MP_TABLE_INT_ASSIGN_ENTRY_TYPE;
            EntryPointer->InterruptType = INT_TYPE_INT;
            EntryPointer->Flags = (UINT16)IsoTbl[i].Flags;
            EntryPointer->SourceBusId = IsaBusId;
            EntryPointer->SourceBusIrq.IsaBusIrq = i;
//define Apic ID and Itin depends of GLobal sys vector
            GetIoApicId(IsoTbl[i].ApicInt, 
                        &EntryPointer->DestIoApicId,
                        &EntryPointer->DestIoApicItin);

            EntryPointer++;
            EntryCount++;
        }
        Dummy >>= 1;
    }

//Second assign PCI IRQ Entries
    
    Dummy = (UINT16)gAmiBoardInfo2Protocol->ApicRoutLength/sizeof(PCI_IRQ_APIC_ROUTE);
    RouteTable = gAmiBoardInfo2Protocol->ApicRoutTable;
    
    for(i = 0; i < Dummy; i++)
    {

        for(j = 0; j < 4; j++)
        {
            if((RouteTable[i].Intn[j].IoApicItin != 0) || (RouteTable[i].Intn[j].IoApicId != 0))
            {
                EntryPointer->EntryType = MP_TABLE_INT_ASSIGN_ENTRY_TYPE;
                EntryPointer->InterruptType = INT_TYPE_INT;
                EntryPointer->Flags = 0;    //always conforms PCI bus specs
                EntryPointer->SourceBusId = RouteTable[i].PciBusNumber;
                EntryPointer->SourceBusIrq.PciBusIrq.PciIntSignal = j;
                EntryPointer->SourceBusIrq.PciBusIrq.PciDeviceNumber = RouteTable[i].DeviceNumber;
                EntryPointer->SourceBusIrq.PciBusIrq.Reserved = 0;
                EntryPointer->DestIoApicId = RouteTable[i].Intn[j].IoApicId;  
#if SMCPKG_SUPPORT
                EntryPointer->DestIoApicItin = RouteTable[i].Intn[j].IoApicItin % 24;
#else
                EntryPointer->DestIoApicItin = RouteTable[i].Intn[j].IoApicItin;
#endif
                EntryPointer++;
                EntryCount++;
            }
        }
    }    

    return EntryCount;
}

/**
    Adds Local interrupt assignment entries to MPS table

        
    @param VOID
     
                   
    @retval Number of entries added
 
**/
UINT16 MpsTableAddLocalIntAssignEntry(VOID)
{
    MP_TABLE_LOCAL_INT_ASSIGN_ENTRY *EntryPointer;
    UINT16 EntryCount = 0;

    EntryPointer = (MP_TABLE_LOCAL_INT_ASSIGN_ENTRY *)MpsTableCurrentPointer;

    EntryPointer->EntryType = MP_TABLE_LOCAL_INT_ASSIGN_ENTRY_TYPE;
    EntryPointer->InterruptType = INT_TYPE_EXT_INT;
    EntryPointer->InterruptSignal.Polarity = POLARITY_CONFORM_SPEC;
    EntryPointer->InterruptSignal.TriggerMode = TRIGGER_MODE_CONFORM_SPEC;
    EntryPointer->InterruptSignal.Reserved = 0;
    EntryPointer->SourceBusId = 0;
    EntryPointer->SourceBusIrq = 0;
    EntryPointer->DestLocalApicId = 0xFF;
    EntryPointer->DestLocalApicItin = 0;

    EntryPointer++;
    EntryCount++;

    EntryPointer->EntryType = MP_TABLE_LOCAL_INT_ASSIGN_ENTRY_TYPE;
    EntryPointer->InterruptType = INT_TYPE_NMI;
    EntryPointer->InterruptSignal.Polarity = POLARITY_CONFORM_SPEC;
    EntryPointer->InterruptSignal.TriggerMode = TRIGGER_MODE_CONFORM_SPEC;
    EntryPointer->InterruptSignal.Reserved = 0;
    EntryPointer->SourceBusId = 0;
    EntryPointer->SourceBusIrq = 0;
    EntryPointer->DestLocalApicId = 0xFF;
    EntryPointer->DestLocalApicItin = 1;

    EntryPointer++;
    EntryCount++;

    return EntryCount;
}

/**
    Adds Extended table entries to base table

        
    @param VOID
     
                   
    @retval VOID
 
**/
VOID MpsTableAddExtendedTable(VOID)
{
    MemCpy(MpsTableCurrentPointer, MpsExTablePointer, ExtendedTableLength);
    MpsExTablePointer = MpsTableCurrentPointer;
    return;
}

/**
    Creates Extended MPS table system address space map entryies

        
    @param pPciRootBridgeIoProtocol 
    @param BusId PCI bus number
    @param BusAttributes PCI bus attributes
     
                   
    @retval VOID

	@note  Modifies MpsExTableCurrentPointer, ExtendedTableLength
	
**/
VOID MpsExTableAddSysAddressMapEntry(
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *pPciRootBridgeIoProtocol,
    IN UINT8 BusId,
    IN UINT8 BusAttributes)
{
    MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY *EntryPointer;
	ACPI_ADDRESS_SPACE_DESC *Resources;
    BOOLEAN  VgaMemoryAdded = FALSE;

    EntryPointer = (MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY *)MpsExTableCurrentPointer;

    pPciRootBridgeIoProtocol->Configuration(pPciRootBridgeIoProtocol, &Resources);

//add entries to ISA decode
    if(BusAttributes & 0x3)                     //support ISA I/O space
    {
        EntryPointer->EntryType = MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY_TYPE;
        EntryPointer->EntryLength = sizeof(MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY);
        EntryPointer->BusId = BusId;
        EntryPointer->AddressType = 0;    //IO descriptor
        EntryPointer->AddressBase = 0;
        EntryPointer->AddressLength = 0x1000;
        EntryPointer++;
        ExtendedTableLength += sizeof(MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY);
    }

    while(Resources->Signature == 0x8A)
    {
        if(Resources->ResourceType != 2  && Resources->AddressLength)
        {
            if(Resources->ResourceType == 1)    //IO descriptor
                EntryPointer->AddressType = 0;
            else if(Resources->TypeSpecificFlags & 6)   //bits 1 and 2 set
                EntryPointer->AddressType = 2;          //memory prefetch address
            else
                EntryPointer->AddressType = 1;          //memory descriptor

//          VGA region should be at the beginning of Memory range
            if ((EntryPointer->AddressType == 1) && (BusAttributes & 0x1C) && (!(VgaMemoryAdded))) {
                EntryPointer->EntryType = MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY_TYPE;
                EntryPointer->EntryLength = sizeof(MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY);
                EntryPointer->BusId = BusId;
                EntryPointer->AddressType = 1;    //memory descriptor
                EntryPointer->AddressBase = 0xA0000;
                EntryPointer->AddressLength = 0x20000;
                EntryPointer++;
                ExtendedTableLength += sizeof(MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY);
                VgaMemoryAdded = TRUE;
                EntryPointer->AddressType = 1;          //memory descriptor
            }

            EntryPointer->EntryType = MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY_TYPE;
            EntryPointer->EntryLength = sizeof(MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY);
            EntryPointer->BusId = BusId;
            EntryPointer->AddressBase = Resources->AddressRangeMin;
            EntryPointer->AddressLength = Resources->AddressLength;

            EntryPointer++;
            ExtendedTableLength += sizeof(MP_EX_TABLE_SYS_ADDRESS_MAP_ENTRY);
        }
        Resources++;
    }

    MpsExTableCurrentPointer = (VOID *)EntryPointer;

    return;
}

/**
    Creates Extended MPS table bus hierarchy entryies

        
    @param VOID
     
                   
    @retval VOID

	@note Modifies MpsExTableCurrentPointer, ExtendedTableLength
	
**/
VOID MpsExTableAddBusHierarchyEntry(VOID)
{
    MP_EX_TABLE_BUS_HIERARCHY_ENTRY *EntryPointer;
    UINT8 i;

    EntryPointer = (MP_EX_TABLE_BUS_HIERARCHY_ENTRY *)MpsExTableCurrentPointer;

    for(i = 0; i < CurrentBusEntry; i++)
    {
        if(BusEntry[i].ParentBusId != 0xFF && BusEntry[i].BusType != 12)    //skip root bridge and P2P bridge
        {
            EntryPointer->EntryType = MP_EX_TABLE_BUS_HIERARCHY_ENTRY_TYPE;
            EntryPointer->EntryLength = sizeof(MP_EX_TABLE_BUS_HIERARCHY_ENTRY);
            EntryPointer->BusId = BusEntry[i].BusId;
            EntryPointer->BusInfo = BusEntry[i].Flags;
            EntryPointer->ParentBusId = BusEntry[i].ParentBusId;
            EntryPointer->Reserved[0] = 0;
            EntryPointer->Reserved[1] = 0;
            EntryPointer->Reserved[2] = 0;

            EntryPointer++;
            ExtendedTableLength += sizeof(MP_EX_TABLE_BUS_HIERARCHY_ENTRY);
        }
    }
    MpsExTableCurrentPointer = (VOID *)EntryPointer;        
}

/**
    Creates Extended MPS table compatibility bus address space
    modifier entryies

        
    @param VOID
     
                   
    @retval VOID
	
	@note Modifies MpsExTableCurrentPointer, ExtendedTableLength
	
**/
VOID MpsExTableAddCompatBusAddressModifierEntry(VOID)
{
    MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY *EntryPointer;
    UINT8 i;

    EntryPointer = (MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY *)MpsExTableCurrentPointer;

    for(i = 0; i < CurrentBusEntry; i++)
    {
        if(BusEntry[i].ParentBusId == 0xFF)
        {
        //add/subtract ISA predefined range
            EntryPointer->EntryType = MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY_TYPE;
            EntryPointer->EntryLength = sizeof(MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY);
            EntryPointer->BusId = BusEntry[i].BusId;
            EntryPointer->RangeList = 0;    //isa range list
            EntryPointer->AddressModifier = (BusEntry[i].Flags & 0x3) ? 0 : 1;

            EntryPointer++;
            ExtendedTableLength += sizeof(MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY);

        //add/subtract VGA predefined range
            EntryPointer->EntryType = MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY_TYPE;
            EntryPointer->EntryLength = sizeof(MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY);
            EntryPointer->BusId = BusEntry[i].BusId;
            EntryPointer->RangeList = 1;    //vga range list
            EntryPointer->AddressModifier = (BusEntry[i].Flags & 0x1C) ? 0 : 1;

            EntryPointer++;
            ExtendedTableLength += sizeof(MP_EX_TABLE_COMPAT_BUS_ADDRESS_MODIFIER_ENTRY);
        }
    }
    MpsExTableCurrentPointer = (VOID *)EntryPointer;
}

/**
    Retreive system info about buses and bus ierarchy and fills
    BUS_INFO structure. Also creates MPS table extended entries

        
    @param VOID
     
                   
    @retval VOID

	@note Modifies BusEntry, MaxBusId, CurrentBusEntry, IsaBusId
	
**/
VOID QueryBusInfo(VOID)
{
	EFI_STATUS Status;
	EFI_HANDLE *pHandleBuffer;
	UINTN	NumberOfHandles;
    UINTN  i;

	EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *pPciRootBridgeIoProtocol;
	static EFI_GUID PciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
	ACPI_ADDRESS_SPACE_DESC *pAddressDescriptor;

	EFI_PCI_IO_PROTOCOL *pPciIoProtocol;
	static EFI_GUID PciIoProtocolGuid = EFI_PCI_IO_PROTOCOL_GUID;
    UINT8   PciData[4];
    UINTN   BusNumber;
    UINTN   dummy[3];
    UINT64  SupportedAttributes;

//First detect and retreive bus Id for PCI host (root) buses
	Status = pBS->LocateHandleBuffer(ByProtocol, &PciRootBridgeIoProtocolGuid,
					 NULL, &NumberOfHandles, &pHandleBuffer);
	if(EFI_ERROR(Status))
		return;

	for(i = 0; i < NumberOfHandles; i++)
    {
	    Status = pBS->HandleProtocol(pHandleBuffer[i], &PciRootBridgeIoProtocolGuid,
					                (VOID**)&pPciRootBridgeIoProtocol);
		ASSERT_EFI_ERROR(Status);
	    Status = pPciRootBridgeIoProtocol->Configuration(pPciRootBridgeIoProtocol,
							        &pAddressDescriptor);
		ASSERT_EFI_ERROR(Status);
        Status = pPciRootBridgeIoProtocol->GetAttributes(pPciRootBridgeIoProtocol,
                                    &SupportedAttributes,
                                    NULL);
		ASSERT_EFI_ERROR(Status);
//find bus range decoding - the min will be host PCI bus Id
        while((pAddressDescriptor->Signature == 0x8A) && \
              (pAddressDescriptor->ResourceType != 2))
                pAddressDescriptor++;

        BusEntry[CurrentBusEntry].BusId = (UINT8)pAddressDescriptor->AddressRangeMin;
        BusEntry[CurrentBusEntry].ParentBusId = 0xFF; //host bus has no parent
        BusEntry[CurrentBusEntry].BusType = BridgeBusTypes[0];
        BusEntry[CurrentBusEntry].Flags = (UINT8)SupportedAttributes;    //for host we save ISA/VGA support values

        MaxBusId = (MaxBusId > BusEntry[CurrentBusEntry].BusId) ? \
                    MaxBusId : BusEntry[CurrentBusEntry].BusId;

        MpsExTableAddSysAddressMapEntry(pPciRootBridgeIoProtocol, 
                                        BusEntry[CurrentBusEntry].BusId,
                                        BusEntry[CurrentBusEntry].Flags);
        CurrentBusEntry++;
    }
    pBS->FreePool(pHandleBuffer);

//Second detect all other buses via PCI bridges
	Status = pBS->LocateHandleBuffer(ByProtocol, &PciIoProtocolGuid,
					 NULL, &NumberOfHandles, &pHandleBuffer);
	if(EFI_ERROR(Status))
		return;

	for(i = 0; i < NumberOfHandles; i++)
    {
	    Status = pBS->HandleProtocol(pHandleBuffer[i], &PciIoProtocolGuid,
					                (VOID**)&pPciIoProtocol);
		if(EFI_ERROR(Status))
			continue;

    //read class code information at 0x8 offset in PCI header
		Status = pPciIoProtocol->Pci.Read(pPciIoProtocol, EfiPciIoWidthUint32,
					 	                  0x8, 1, (VOID*)PciData);
		if(EFI_ERROR(Status)) //problem
			continue;
#if PCI_BUS_APIC_AUTODETECT == 1
//if IO APIC device collect info, we will need it later
        if((PciData[3] == 0x8) && (PciData[2] == 0) && (PciData[1] >= 0x10))
        {
            UINT64  Attr=0, OldAttr=0;
        //----------------------
            //1. make sure it is Enabled and Decoding it's resources
            Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationGet, Attr, &OldAttr);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) continue;
                                
            Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationSupported, 0, &Attr);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) continue;

            Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationSet, Attr&(EFI_PCI_DEVICE_ENABLE), NULL);
            ASSERT_EFI_ERROR(Status);
  		    if(EFI_ERROR(Status)) continue;

            CollectIoApicInfo(pPciIoProtocol, 0);

#if PCI_BUS_APIC_LEAVE_ENABLE != 0
            //Restore attributes of the device
            Status=pPciIoProtocol->Attributes(pPciIoProtocol,EfiPciIoAttributeOperationSet, OldAttr, NULL);
            ASSERT_EFI_ERROR(Status);
            if(EFI_ERROR(Status)) continue;
#endif//PCI_BUS_APIC_LEAVE_ENABLE

            continue;
        }
#endif//PCI_BUS_APIC_AUTODETECT

        if(PciData[3] != 0x6)   //not bridge device
            continue;

        if((PciData[2] == 0) || (PciData[2] > 6))     //PCI host bridge we already know, so skip it
            continue;                                 //or unknown bridge - skip it also

        BusEntry[CurrentBusEntry].BusType = BridgeBusTypes[(PciData[2])];

        if(PciData[2] == 4)     // PCI to PCI bridge
        {
            BusEntry[CurrentBusEntry].Flags = PciData[1];
        //read primary and secondary bus numbers
		    Status = pPciIoProtocol->Pci.Read(pPciIoProtocol, EfiPciIoWidthUint32,
					 	                      0x18, 1, (VOID*)PciData);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
		    ASSERT_EFI_ERROR(Status);
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.

            BusEntry[CurrentBusEntry].BusId = PciData[1]; //bus Id prodused by this bridge
            BusEntry[CurrentBusEntry].ParentBusId = PciData[0];
            MaxBusId = (MaxBusId > BusEntry[CurrentBusEntry].BusId) ? \
                        MaxBusId : BusEntry[CurrentBusEntry].BusId;
        }
        else        //any other bridge
        {
            BusEntry[CurrentBusEntry].Flags = 1;  //all other buses has this feature
            BusEntry[CurrentBusEntry].BusId = 0xFF;     //we will assign Id after enumerating PCI buses
        //get bridge location to determine parent bus Id        
		    Status = pPciIoProtocol->GetLocation(pPciIoProtocol, &dummy[0], &BusNumber,
                                                 &dummy[1], &dummy[2]);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
		    ASSERT_EFI_ERROR(Status);
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.

            BusEntry[CurrentBusEntry].ParentBusId = (UINT8)BusNumber;
        }
        CurrentBusEntry++;
    }
    pBS->FreePool(pHandleBuffer);

// Third enumerate all other buses
    for(i = 0; i < CurrentBusEntry; i++)
    {
        if(BusEntry[i].BusId == 0xFF)
        {
            BusEntry[i].BusId = MaxBusId + 1;
            MaxBusId++;
            if(BusEntry[i].BusType == BUS_TYPE_ISA) //save ISA bus Id for interrupt assign procedure
                IsaBusId = MaxBusId;
        }
    }
    MpsExTableAddBusHierarchyEntry();
    MpsExTableAddCompatBusAddressModifierEntry();
}

/**
    Retreive system info about IO APIC 

        
    @param pPciIoProtocol 
    @param BaseAddress base address of IO APIC
     
                   
    @retval VOID

	@note Modifies IoApicEntry, CurrentIoApicEntry
**/
VOID CollectIoApicInfo(
    IN EFI_PCI_IO_PROTOCOL *pPciIoProtocol OPTIONAL,
    IN UINT32 BaseAddress OPTIONAL)
{
    UINT32 IoApicAddress;
    EFI_STATUS Status;
	UINT32 volatile *IoApicMemory32;
	UINT8  volatile *IoApicMemory8;
	UINT32 data;

    if(BaseAddress == (UINT32)NULL)     //IO APIC visible as PCI device
    {
	    Status = pPciIoProtocol->Pci.Read(pPciIoProtocol, EfiPciIoWidthUint32,
			    		 	              0x10, 1, (VOID*)&IoApicAddress);
	    if(EFI_ERROR(Status) || (IoApicAddress == 0)) //problem or mapped to default address range
		    return;
    }
    else                        // IO APIC mapped to default range
        IoApicAddress = BaseAddress;
    
	IoApicMemory32 = (UINT32 *)IoApicAddress;
	IoApicMemory8 = (UINT8 *)IoApicAddress;

	IoApicMemory8[IO_APIC_INDEX_REG] = IO_APIC_MAP_ID_REG;
	data = IoApicMemory32[IO_APIC_DATA_REG];

    IoApicEntry[CurrentIoApicEntry].IoApicId = (UINT8)((data & IO_APIC_ID_READ_MASK) >> 24);

	IoApicMemory8[IO_APIC_INDEX_REG] = IO_APIC_MAP_VERSION_REG;
	data = IoApicMemory32[IO_APIC_DATA_REG];

    IoApicEntry[CurrentIoApicEntry].IoApicVersion = (UINT8)(data & 0xFF);
    IoApicEntry[CurrentIoApicEntry].MaxRedirectionEntries = (UINT8)((data & 0x00FF0000) >> 16);
    IoApicEntry[CurrentIoApicEntry].IoApicFlags = 1;
    IoApicEntry[CurrentIoApicEntry].IoApicAddress = IoApicAddress;

    CurrentIoApicEntry++;
}

/**
    Arranges IO APIC Entries in their ID ascending order

        
    @param VOID
     
                   
    @retval VOID

	@note Modifies IoApicEntry, CurrentIoApicEntry
	
**/
VOID ArrangeIoApicEntries(VOID)
{
    UINT8 i;
    BOOLEAN Swap = TRUE;
    IO_APIC_INFO Temp;

    while(Swap)
    {
        Swap = FALSE;
        for(i = 0; i < CurrentIoApicEntry - 1; i++)
        {
            if(IoApicEntry[i].IoApicId > IoApicEntry[i+1].IoApicId)
            {
                Swap = TRUE;
                MemCpy(&Temp, &IoApicEntry[i], sizeof(IO_APIC_INFO));
                MemCpy(&IoApicEntry[i], &IoApicEntry[i+1], sizeof(IO_APIC_INFO));
                MemCpy(&IoApicEntry[i+1], &Temp, sizeof(IO_APIC_INFO));
            }
        }
    }
}

/**
    Returns IO APIC ID and Pin number assosiated with given SysVectorNumber

        
    @param SysVectorNumber system interrupt vector number
    @param IoApicId Id of IO APIC which handles that vector
    @param IoApicItin IO APIC Pin number associated with given vector number
     
                   
    @retval VOID

	@note Modifies    IoApicEntry, CurrentIoApicEntry
	
**/
VOID GetIoApicId(
    IN UINT16   SysVectorNumber,
    OUT UINT8 *IoApicId,
    OUT UINT8 *IoApicItin)
{
    UINT8 i;

    for(i = 0; i < CurrentIoApicEntry; i++)
    {
        if(SysVectorNumber < IoApicEntry[i].MaxRedirectionEntries) //this is Apic we need
        {
            *IoApicId = IoApicEntry[i].IoApicId;
            *IoApicItin = (UINT8)SysVectorNumber;
            return;
        }
        SysVectorNumber -= IoApicEntry[i].MaxRedirectionEntries;
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
