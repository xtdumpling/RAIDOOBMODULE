//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00   Enable Above 4G when system memory out of resource.
//    Bug Fix:
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Sep/12/2016  
//
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiPciAccessCspBaseLib.c
    Library Class for AMI CSP PCI Interface

	@note Requires Chipset Specific Porting for each project!
**/
//*************************************************************************

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Token.h>
#include <AmiLib.h>
#include <AcpiRes.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/AcpiTable.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
// APTIOV_SERVER_OVERRIDE_START : OOR Support
#include <Protocol/IioUds.h>
// APTIOV_SERVER_OVERRIDE_END : OOR Support .
#include <Library/BaseMemoryLib.h>
#if CRB_OUT_OF_RESOURCE_SUPPORT
// APTIOV_SERVER_OVERRIDE_START : Dynamic Resource Allocation.
#include "Guid/SocketMpLinkVariable.h"
#include "Guid/SocketPciResourceData.h"
#include <Guid/SetupVariable.h>
// APTIOV_SERVER_OVERRIDE_END : Dynamic Resource Allocation.
#endif

// APTIOV_SERVER_OVERRIDE_START : Added to support FPGA
#if FPGA_SUPPORT
#include <Library/FpgaConfigurationLib.h>
#endif
// APTIOV_SERVER_OVERRIDE_END : Added to support FPGA

#if SMCPKG_SUPPORT
#include <Setup.h>
#endif
// APTIOV_SERVER_OVERRIDE_START : Added to support MCFG ACPI Table update.
#include <Acpi/Mcfg.h>
#include <Library/AmiPlatformAcpiLib.h>
#include <Protocol/GlobalNvsArea.h>
// APTIOV_SERVER_OVERRIDE_END : Added to support MCFG ACPI Table update.

// APTIOV_SERVER_OVERRIDE_START : dynamic MMCFG base change support.
#ifdef DYNAMIC_MMCFG_BASE_FLAG 
VOID*
GetPciExpressBaseAddress (
  VOID
  );
#endif
// APTIOV_SERVER_OVERRIDE_END : dynamic MMCFG base change support.

// APTIOV_SERVER_OVERRIDE_START : Added to support FPGA
#if FPGA_SUPPORT
typedef struct _ROOTBRIDGEINDEXLOCATOR {
    UINT8   RootBridgeIndex;
    UINT8   SocketNumber;
    UINT8   StackNumber;
}ROOTBRIDGEINDEXLOCATOR;

ROOTBRIDGEINDEXLOCATOR  Rootbridgeindexloc[] = {
        
        {0,0,0}, {1,0,1}, {2,0,2}, {3,0,3}, {4,0,4}, {5,0,5},{6,0,4}, {7,0,5},
        {8,1,0}, {9,1,1}, {10,1,2}, {11,1,3}, {12,1,4}, {13,1,5},{14,1,4}, {15,1,5},
        {16,2,0}, {17,2,1}, {18,2,2}, {19,2,3}, {20,2,4}, {21,2,5},{22,2,4}, {23,2,5},
        {24,3,0}, {25,3,1}, {26,3,2}, {27,3,3}, {28,3,4}, {29,3,5},{30,3,4}, {31,3,5}
};
#endif
// APTIOV_SERVER_OVERRIDE_END : Added to support FPGA

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
#define PCIE_MAX_BUS ((PCIEX_LENGTH/0x100000)-1)

// GUID Definition(s)
extern EFI_GUID gAmiGlobalVariableGuid;

// Variable Declaration(s)
EFI_EVENT       			mAcpiEvent=NULL;
VOID            			*mAcpiReg=NULL;
CHAR16          			gMemoryCeilingVariable[] = L"MemCeil.";
//Keep MCFG table key and ACPI Table Protocol as global
//in case somebody will like to update MCFG table.
UINTN           			gMcfgTableKey=0;
EFI_ACPI_TABLE_PROTOCOL     *gAcpiTableProtocol=NULL;
// APTIOV_SERVER_OVERRIDE_START : Platform/Chipset Sapecifik interface provoding information about resource Split between Multiple ROOTs.
EFI_IIO_UDS_PROTOCOL  	    *gIioUds = NULL;
// APTIOV_SERVER_OVERRIDE_END : Platform/Chipset Sapecifik interface provoding information about resource Split between Multiple ROOTs.

BOOLEAN				gRbIndexSocketStackMapUpdated = FALSE;
UINTN					gMaxRbIndex = 0xFF;

// APTIOV_SERVER_OVERRIDE_START : OOR Support 
#if CRB_OUT_OF_RESOURCE_SUPPORT

#define SAD_LEG_IO_GRANTY_2KB   0x800         // 2KB
#define SAD_MMIOL_GRANTY_4MB    0x400000      // 4MB 

#define IOFAILURE 		        0x01
#define MMIOFAILURE 	        0x02
#define MMIO32_TYPES	        0x02  // MMIO32 MMIO32pf
#define IO_TYPES	            0x02  // Can be 16/32

#ifndef MAX_SAD_TARGETS
#define MAX_SAD_TARGETS         8
#endif

#define LAST_BOOT_FAILED_GUID \
{0x8163179a, 0xf31a, 0x4132, 0xae, 0xde, 0x89, 0xf3, 0x6a, 0xee, 0x43, 0xda}
#define AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA_GUID \
{ 0x3992e100, 0x8860, 0x11e0, 0x9d,0x78, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66}

EFI_GUID LastBootFailedIohGuid          = LAST_BOOT_FAILED_GUID;
EFI_GUID    gEfiAutoMmioIOVariableGuid = AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA_GUID;

// Function Declaration
VOID UpdateSocketPciRescVariable ();
VOID CalculateRequiredSocketRatio (UINT8 OutOfResrcFailureType);
VOID InitAndFillValidSocketIndex (UINT8 OutOfResrcFailureType);
VOID CheckValidSocketsAvail (BOOLEAN *NotPossibleFlag);
VOID SocketResourceAdjustment (UINT8    OutOfResrcFailureType,BOOLEAN    *NotPossibleFlag);

//
// OutOfResource Structure
//
typedef struct _IOHX_OUT_OF_RESOURCE_INFO {
    
    // ########################### SOCKET LEVEL #############################
    // MMIO/IO OutOFResourceFlag
	UINT8     MmmioOutofResrcFlag [MAX_SOCKET];
	UINT8     IoOutofResrcFlag [MAX_SOCKET];
	
	UINT8     MmmioOutofResrcFlag_Stack_Level [MAX_SOCKET][MAX_IIO_STACK];
	UINT8     IoOutofResrcFlag_Stack_Level [MAX_SOCKET][MAX_IIO_STACK];

    // MMio Stuff
	UINT64    MmioBase [MAX_SOCKET];
	UINT64	  MmioLimit[MAX_SOCKET];
	UINT64	  MmioRequested[MAX_SOCKET];

    //IO Stuff
	UINT64    IoBase[MAX_SOCKET];
	UINT64	  IoLimit[MAX_SOCKET];
	UINT64	  IoRequested[MAX_SOCKET][IO_TYPES];
	
	UINT64    Socket_IOLength[MAX_SOCKET];
	UINT64    Socket_MMIOLength[MAX_SOCKET];
	
	UINT16    Socket_Current_Boot_IORatio[MAX_SOCKET];
	UINT16    Socket_Current_Boot_MMIORatio[MAX_SOCKET];

    // ########################### STACK LEVEL #############################
    // MMio Stuff
	UINT64    MmioBase_Stack_Level [MAX_SOCKET] [MAX_IIO_STACK];
	UINT64    MmioLimit_Stack_Level[MAX_SOCKET][MAX_IIO_STACK];
	UINT64    MmioRequested_Stack_Level[MAX_SOCKET][MAX_IIO_STACK] [MMIO32_TYPES];
	UINT64    Mmio_Gra_Stack_Level [MAX_SOCKET][MAX_IIO_STACK] [MMIO32_TYPES];

    // IO Stuff
	UINT64    IoBase_Stack_Level[MAX_SOCKET] [MAX_IIO_STACK];
	UINT64    IoLimit_Stack_Level[MAX_SOCKET] [MAX_IIO_STACK];
	UINT64    IoRequested_Stack_Level[MAX_SOCKET][MAX_IIO_STACK][IO_TYPES];	
	UINT64    Io_Gra_Stack_Level [MAX_SOCKET][MAX_IIO_STACK] [IO_TYPES];

	UINT64    Stack_IOLength[MAX_SOCKET][MAX_IIO_STACK];
	UINT64    Stack_MMIOLength[MAX_SOCKET][MAX_IIO_STACK];

	UINT16    Stack_Current_Boot_IORatio[MAX_SOCKET] [MAX_IIO_STACK];
	UINT16    Stack_Current_Boot_MMIORatio[MAX_SOCKET][MAX_IIO_STACK];

    // ########################### COMMON #############################
	UINT64	  Failed_Stack;
	//UINT64    Failed_Socket;
    
} IOHX_OUT_OF_RESOURCE_INFO;

//
//	LAST_BOOT_FAILED_IOH Structure
//
typedef struct _LAST_BOOT_FAILED_IOH {
    UINT8     MmmioOutofResrcFlag [MAX_SOCKET][MAX_IIO_STACK];
    UINT8     IoOutofResrcFlag [MAX_SOCKET][MAX_IIO_STACK];
} LAST_BOOT_FAILED_IOH;

//
//	CURRENT_BOOT_MMIO_IO_RATIO Structure
//
typedef struct _CURRENT_BOOT_MMIO_IO_RATIO {
	
    // ########################### SOCKET LEVEL #############################	
    UINT16    MMIO [MAX_SOCKET];
    UINT16    IO [MAX_SOCKET];
	
    // ########################### STACK LEVEL #############################
    UINT16    MMIO_Stack_Level [MAX_SOCKET] [MAX_IIO_STACK];
    UINT16    IO_Stack_Level [MAX_SOCKET] [MAX_IIO_STACK];
    
} CURRENT_BOOT_MMIO_IO_RATIO;


//
// REQUIRED_MMIO_IO_RATIO Ratio Structure
//
typedef struct _REQUIRED_MMIO_IO_RATIO {

    // ########################### SOCKET LEVEL #############################
    UINT16    MMIO [MAX_SOCKET];
    UINT16    IO [MAX_SOCKET];
	
    // ########################### STACK LEVEL #############################
    UINT16    MMIO_Stack_Level [MAX_SOCKET] [MAX_IIO_STACK];
    UINT16    IO_Stack_Level [MAX_SOCKET] [MAX_IIO_STACK];
    
} REQUIRED_MMIO_IO_RATIO;

static UINT8  		  NoOfStackFailureRetries = NO_OF_STACK_FAIULRE_RETRIES;
UINT8       TempMmmioOutofResrcFlag [MAX_SOCKET][MAX_IIO_STACK];
UINT8       TempIoOutofResrcFlag [MAX_SOCKET][MAX_IIO_STACK];
CURRENT_BOOT_MMIO_IO_RATIO            CurrentMmioIoRatios;
REQUIRED_MMIO_IO_RATIO                RequiredMMIOIoRatio;
IOHX_OUT_OF_RESOURCE_INFO             IohOutofResInfo;
AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA    AutoAdjustMmioIoVariable;

UINT8           NumberOfIoH;
UINT8           NumberofStacksPerCpu ;
BOOLEAN         Stack_MmmioOutofResrcFlag                = FALSE;
BOOLEAN 	    Stack_IoOutofResrcFlag 	                 = FALSE;
BOOLEAN 	    Socket_MmmioOutofResrcFlag               = FALSE;
BOOLEAN 	    Socket_IoOutofResrcFlag                  = FALSE;
BOOLEAN         SocketLevelAdjustmentNeeded              = FALSE;
BOOLEAN			SocketLevelMMIoAdjustmentNeeded			 = FALSE;
BOOLEAN			SocketLevelIoAdjustmentNeeded			 = FALSE;
BOOLEAN         StackLevelAdjustmentNeeded               = FALSE;
BOOLEAN		    AfterStackAdjust_SocketMMIO_AdjustNeeded = FALSE;
BOOLEAN	        AfterStackAdjust_SocketIO_AdjustNeeded   = FALSE;

static BOOLEAN	IohOutofResInfoStructureUpdated          = FALSE;
#if OOR_NOT_POSSIBLE_HANDLING_POLICY
BOOLEAN	        IsNewMmCfgBaseSuccess= FALSE ;
#endif
UINT32          Attributes;

UINT8           ValidStacks [MAX_IIO_STACK];
UINT8           ValidIOH [MAX_SOCKET];
UINT8           IOHUsedDuringSocketMMIOAdjustment [MAX_SOCKET];
UINT8           IOHUsedDuringSocketIOAdjustment [MAX_SOCKET];
UINT8           NumIoH;

UINTN           Socket_IO_Failed_Socket [MAX_SOCKET];  
UINTN           Socket_MMIO_Failed_Socket [MAX_SOCKET];

UINTN           Stack_IO_Failed_Socket [MAX_SOCKET];   
UINTN           Stack_MMIO_Failed_Socket [MAX_SOCKET]; 

UINT8           Stack_Mmio_SocketNumber = 0;  // Stack Level
UINT8           Stack_Io_SocketNumber = 0;

UINT8           Socket_Mmio_SocketNumber = 0; // Socket Level
UINT8           Socket_Io_SocketNumber = 0;

BOOLEAN         Socket_Stack_MmioBaseAddressExceededeMaxLimit[MAX_SOCKET] [MAX_IIO_STACK] = {0};
BOOLEAN         Socket_Stack_IoBaseAddressExceededeMaxLimit[MAX_SOCKET] [MAX_IIO_STACK]   = {0};

UINT64          AlignedBaseAddrss;
UINT64          AlignedIoBaseAddrss;

BOOLEAN         FoundMmioRatioAtleaseOnce = FALSE;
BOOLEAN         FoundIoRatioAtleaseOnce = FALSE;

BOOLEAN         SocketAutoAdjustCalledOnce = FALSE;
BOOLEAN         StackAutoAdjustCalledOnce = FALSE;
BOOLEAN         Adjustmentneeded = FALSE;

UINT64          IoBaseExceededLimit = 0;

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   NbGetMaximumIohSockets
//
// Description: Get Max Socket Number from IIO
//
// Input:       None
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 NbGetMaximumIohSockets(VOID)
{
	return gIioUds->IioUdsPtr->SystemStatus.numCpus;
}
#endif
// APTIOV_SERVER_OVERRIDE_END : OOR Support

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   AlignFromGranularity
//
// Description: Converts C passed into Alignment format
//
// Input:       g - AlignFromGra Value to convert
//
// Output:      UINTN - Converted Alignment value
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN AlignFromGranularity(UINTN g){
    UINTN a=0;
//------------
    while(g&1){
        a++;
        g=g>>1;
    }
    return a;   
}

/**
    This function will create the MCFG ACPI table when ACPI
    support protocol is available.

    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID

    @note  Porting required.
**/

VOID CreateNbAcpiTables (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    EFI_STATUS                  Status = 0;
    UINT8                       OemId[6] = CONVERT_TO_STRING(T_ACPI_OEM_ID);
    UINT8                       OemTblId[8] = CONVERT_TO_STRING(T_ACPI_OEM_TBL_ID);

    // APTIOV_SERVER_OVERRIDE_START
    // MCFG_20                                               *Mcfg;
    UINT8                                                    NodeId;
    UINT8                                                    NodeCount;
    EFI_GLOBAL_NVS_AREA_PROTOCOL                             *AcpiNvsProtocol = NULL;
    BIOS_ACPI_PARAM                                          *mAcpiParameter = NULL;
    EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE  *Mcfg;

    if (gIioUds == NULL) 
    {
        Status = pBS->LocateProtocol( &gEfiIioUdsProtocolGuid,
                                      NULL,
                                      &gIioUds );
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return;
    }

    Status = pBS->LocateProtocol ( &gEfiGlobalNvsAreaProtocolGuid,
                                   NULL,
                                   &AcpiNvsProtocol );
    if (EFI_ERROR(Status))
    {
        DEBUG ((EFI_D_ERROR, "PciHbCsp: Locating gEfiGlobalNvsAreaProtocolGuid Status (MCFG) = %r \n", Status));
        return;
    }
    mAcpiParameter = AcpiNvsProtocol->Area;

    Mcfg = MallocZ(sizeof(EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE));
	#if 0
//---------------------------------------------
    // it must be only one instance of such protocol
    if (gAcpiTableProtocol==NULL){
        Status = pBS->LocateProtocol( &gEfiAcpiTableProtocolGuid, NULL, &gAcpiTableProtocol );
        if(EFI_ERROR(Status)) {
            DEBUG ((EFI_D_ERROR, "PciHbCsp: LocateProtocol(ACPITableProtocol)= %r\n", Status));
            return;
        }
    }

    Mcfg = MallocZ(sizeof(MCFG_20));
    #endif
    // APTIOV_SERVER_OVERRIDE_END
    ASSERT(Mcfg);
    if(!Mcfg) return;

    // Fill Table header;
	// APTIOV_SERVER_OVERRIDE_START
    Mcfg->Header.Header.Signature = MCFG_SIG;
    Mcfg->Header.Header.Length = sizeof(EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE);
    Mcfg->Header.Header.Revision = 1;
    Mcfg->Header.Header.Checksum = 0;
    MemCpy(&(Mcfg->Header.Header.OemId), OemId, 6);
    MemCpy(&(Mcfg->Header.Header.OemTableId), OemTblId, 8);
    Mcfg->Header.Header.OemRevision = ACPI_OEM_REV;
    Mcfg->Header.Header.CreatorId = 0x5446534d; //"MSFT" 4D 53 46 54
    Mcfg->Header.Header.CreatorRevision = 0x97;

    //
    // Dynamically allow multi-seg support
    //
    mAcpiParameter->PCIe_MultiSeg_Support = 0;
    for (NodeId = 0; NodeId < MAX_SOCKET; NodeId++) 
    {
        if ((UINT16)(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment) > 0) 
        {
            mAcpiParameter->PCIe_MultiSeg_Support = 1;
            break;
        }
    }

    //
    // Update MCFG table entries (segment number, base addr and start/end bus numbers)
    //
    if ( mAcpiParameter->PCIe_MultiSeg_Support == 0 ) 
    {
        //
        // Original code for single  PCIe segment start
        //
        Mcfg->Segment[0].BaseAddress = gIioUds->IioUdsPtr->PlatformData.PciExpressBase;
        Mcfg->Segment[0].EndBusNumber = (UINT8)RShiftU64 (gIioUds->IioUdsPtr->PlatformData.PciExpressSize, 20) - 1;

        //
        // Single segment with segment number as 0
        //
        Mcfg->Segment[0].PciSegmentGroupNumber = 0;
        NodeCount = 1;
    }
    //
    // PCIe Multi-Segment handling - Assume each CPU socket as a segment, and copy Segement info from IioUds HOB to MCFG table entries
    //
    else 
    {
        //
        // Segment count = 0
        //
        NodeCount = 0;

        for (NodeId = 0; NodeId < MAX_SOCKET; NodeId++) 
        {
            //
            // Skip a socket if it does not exist or does not contain valid bus range data
            //
            if ( (UINT8)(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketLastBus) ==
                 (UINT8)(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketFirstBus) ) 
            {
                continue; 
            }

            //
            // Copy PCIe Segement info from IioUds HOB to MCFG table entries
            //
            Mcfg->Segment[NodeCount].PciSegmentGroupNumber = (UINT16)(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment);

            Mcfg->Segment[NodeCount].BaseAddress = LShiftU64 (gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SegMmcfgBase.hi, 32) + 
                                                   (gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SegMmcfgBase.lo) ;

            Mcfg->Segment[NodeCount].StartBusNumber = (UINT8)(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketFirstBus);

            Mcfg->Segment[NodeCount].EndBusNumber = (UINT8)(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].SocketLastBus);

            //
            // Update segment number returned by AML  _SEG() .  It resides in mAcpiParameter region now.
            //
            mAcpiParameter->PcieSegNum[NodeId] = (UINT8)(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[NodeId].PcieSegment);

            //
            // Update count of valid segments
            //
            NodeCount++;
        }
    }

    //
    // Set MCFG table "Length" field based on the number of PCIe segments enumerated so far
    //
    Mcfg->Header.Header.Length = sizeof (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER) + 
                                 sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE) * NodeCount;

    //
    // Debug dump of MCFG table
    //
    PCI_TRACE(( TRACE_PCI, "ACPI MCFG table @ address 0x%x\n", Mcfg ));
    PCI_TRACE(( TRACE_PCI, "  Multi-Seg Support = %x\n", mAcpiParameter->PCIe_MultiSeg_Support));
    PCI_TRACE(( TRACE_PCI, "  Number of Segments (sockets): %2d\n", NodeCount ));
    PCI_TRACE(( TRACE_PCI, "  Table Length = 0x%x\n\n", Mcfg->Header.Header.Length ));
    for (NodeId = 0; NodeId < NodeCount; NodeId ++) 
    {
        PCI_TRACE(( TRACE_PCI, "   Segment[%2d].BaseAddress = %x\n",  NodeId, Mcfg->Segment[NodeId].BaseAddress));
        PCI_TRACE(( TRACE_PCI, "   Segment[%2d].PciSegmentGroupNumber = %x\n", NodeId, Mcfg->Segment[NodeId].PciSegmentGroupNumber));
        PCI_TRACE(( TRACE_PCI, "   Segment[%2d].StartBusNumber = %x\n", NodeId, Mcfg->Segment[NodeId].StartBusNumber));
        PCI_TRACE(( TRACE_PCI, "   Segment[%2d].EndBusNumber = %x\n\n", NodeId, Mcfg->Segment[NodeId].EndBusNumber));
    }

    // Add table
    Status = AcpiLibInstallAcpiTable( Mcfg, Mcfg->Header.Header.Length, &gMcfgTableKey );
   // Commenting code
	#if 0
    Mcfg->Header.Signature = MCFG_SIG;
    Mcfg->Header.Length = sizeof(MCFG_20);
    Mcfg->Header.Revision = 1;
    Mcfg->Header.Checksum = 0;
    MemCpy(&(Mcfg->Header.OemId[0]), OemId, 6);
    MemCpy(&(Mcfg->Header.OemTblId[0]), OemTblId, 8);
    Mcfg->Header.OemRev = ACPI_OEM_REV;
    Mcfg->Header.CreatorId = 0x5446534d;//"MSFT" 4D 53 46 54
    Mcfg->Header.CreatorRev = 0x97;

    // Fill MCFG Fields

    // Base address of 256/128/64MB extended config space
    Mcfg->BaseAddr = (UINTN)PcdGet64 (PcdPciExpressBaseAddress);
    // Segment # of PCI Bus
    Mcfg->PciSeg = 0;
    // Start bus number of PCI segment
    Mcfg->StartBus = 0;
    // End bus number of PCI segment
    Mcfg->EndBus = PCIE_MAX_BUS;

    // Add table
    Status = gAcpiTableProtocol->InstallAcpiTable( gAcpiTableProtocol, Mcfg, sizeof(MCFG_20), &gMcfgTableKey );
	#endif
	// APTIOV_SERVER_OVERRIDE_END
    PCI_TRACE((TRACE_PCI,"PciHbCsp: Installing AcpiTable (MCFG) = %r \n", Status));
    ASSERT_EFI_ERROR(Status);

    // Free memory used for table image
    pBS->FreePool(Mcfg);

    // Kill the Event
    pBS->CloseEvent(Event);

    return;
}

//----------------------------------------------------------------------------
// Following functions are HOST BRIDGE Infrastructure Overrides and Porting.
//----------------------------------------------------------------------------

/**
    This procedure will be invoked in PCI Host Bridge Protocol
    when NotifyPhase function is called with phase
    EfiPciHostBridgeBeginEnumeration.

    @param ResAllocProtocol Pointer to Host Bridge Resource
        Allocation Protocol.
    @param RbIoProtocolBuffer Pointer to Root Bridge I/O Protocol.
    @param RbCount Root Bridge counter.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbNotifyCspBeforeEnumeration (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}


/**
    This function will be invoked in PCI Host Bridge Driver
    before converting all Non Existant MMIO into PCI MMIO.
    In order to allow CSP code do aome none standard conversion.

    @param ImageHandle - this image Handle
    @param ControllerHandle - Controller(RB) Handle (Optional).

    @retval EFI_STATUS
            EFI_UNSUPPORTED - means use default MMIO convertion.
            EFI_SUCCESS - CSP code has been converted MMIO itself.
            ANYTHING else - ERROR.
            
    @note  Porting required if needed.
**/
EFI_STATUS HbCspConvertMemoryMapIo (
            IN EFI_HANDLE      ImageHandle,
            IN EFI_HANDLE      ControllerHandle)
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;

    // Any Additional Variables goes here
 //---------------------------------------   
    

    return Status;
}

EFI_STATUS HbCspConvertMemoryMapMmio (
            IN EFI_HANDLE      ImageHandle,
            IN EFI_HANDLE      ControllerHandle)
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;
	// APTIOV_SERVER_OVERRIDE_START
    EFI_PHYSICAL_ADDRESS        BaseAddress;
    DXE_SERVICES                *pDxeSerTbl = NULL;
    UINTN                       IioResourceMapEntry;
    UINTN                       IioStackResources;
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR GcdDescriptor;

    if (gIioUds == NULL) {
        Status = pBS->LocateProtocol( &gEfiIioUdsProtocolGuid,
                                      NULL,
                                      &gIioUds );
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return Status;
    }

    //
    // Get DXE Services Table Pointer
    //
    Status = LibGetDxeSvcTbl (&pDxeSerTbl);
    if(EFI_ERROR(Status)) return Status;

    //
    // Add PCIE base into Runtime memory so that it can be reported in E820 table
    //
    Status = pDxeSerTbl->AddMemorySpace (
                          EfiGcdMemoryTypeMemoryMappedIo,
                          gIioUds->IioUdsPtr->PlatformData.PciExpressBase,
                          gIioUds->IioUdsPtr->PlatformData.PciExpressSize,
                          EFI_MEMORY_RUNTIME | EFI_MEMORY_UC
                          );
    ASSERT_EFI_ERROR(Status);
    BaseAddress = gIioUds->IioUdsPtr->PlatformData.PciExpressBase;
    Status = pDxeSerTbl->AllocateMemorySpace (
                     EfiGcdAllocateAddress,
                     EfiGcdMemoryTypeMemoryMappedIo,
                     0,
                     (UINT64)gIioUds->IioUdsPtr->PlatformData.PciExpressSize,
                     &BaseAddress,
                     ImageHandle,
                     NULL
                     );
    ASSERT_EFI_ERROR(Status);
    Status = pDxeSerTbl->GetMemorySpaceDescriptor (BaseAddress, &GcdDescriptor);

     if (!EFI_ERROR(Status)) {
         Status = pDxeSerTbl->SetMemorySpaceAttributes (
                         BaseAddress,
                         GcdDescriptor.Length,
                         GcdDescriptor.Attributes | EFI_MEMORY_RUNTIME
                         );
         ASSERT_EFI_ERROR (Status);
     }

  //    for (IioResourceMapEntry = 0; IioResourceMapEntry < gIioUds->IioUdsPtr->PlatformData.numofIIO; IioResourceMapEntry++) {
     for (IioResourceMapEntry = 0; IioResourceMapEntry < MAX_SOCKET; IioResourceMapEntry++) {
         if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].Valid)
           continue; // No guarantee that the valid IIOs are sequential starting at 0!
         for (IioStackResources = 0; IioStackResources < MAX_IIO_STACK; IioStackResources++) {
             if(!(gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[IioResourceMapEntry].stackPresentBitmap & (1 << IioStackResources)))
                continue;

          if (gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem32Limit >
              gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem32Base) {
              //
              // Shouldn't the capabilities be UC?
              //
              Status = pDxeSerTbl->AddMemorySpace (
                            EfiGcdMemoryTypeMemoryMappedIo,
                            gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem32Base ,
                            (gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem32Limit -
                             gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem32Base + 1),
                            0
                            );
              ASSERT_EFI_ERROR (Status);
           }

           if ((gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem64Limit >
                gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem64Base) &&
                (gIioUds->IioUdsPtr->PlatformData.Pci64BitResourceAllocation)) {

                Status = pDxeSerTbl->AddMemorySpace (
                              EfiGcdMemoryTypeMemoryMappedIo,
                              gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem64Base ,
                              (gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem64Limit -
                               gIioUds->IioUdsPtr->PlatformData.IIO_resource[IioResourceMapEntry].StackRes[IioStackResources].PciResourceMem64Base + 1),
                              0
                              );
              ASSERT_EFI_ERROR (Status);
           }
         }
      }
	// APTIOV_SERVER_OVERRIDE_END
    return Status;
}


/**
    This procedure will be invoked in PCI Host Bridge Protocol
    when NotifyPhase function is called with phase
    EfiPciHostBridgeBeginBusAllocation.

    @param ResAllocProtocol Pointer to Host Bridge Resource
        Allocation Protocol.
    @param RbIoProtocolBuffer Pointer to Root Bridge I/O Protocol.
    @param RbCount Root Bridge counter.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbNotifyCspBeginBusAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                   **RbIoProtocolBuffer,
    IN UINTN                                             RbCount )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}


/**
    This procedure will be invoked in PCI Host Bridge Protocol
    when NotifyPhase function is called with phase
    EfiPciHostBridgeEndBusAllocation

    @param ResAllocProtocol Pointer to Host Bridge Resource
        Allocation Protocol.
    @param RbIoProtocolBuffer Pointer to Root Bridge I/O Protocol.
    @param RbCount Root Bridge counter.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbNotifyCspEndBusAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}

/**
    This procedure will be invoked in PCI Host Bridge Protocol
    when NotifyPhase function is called with phase
    EfiPciHostBridgeBeginResourceAllocation.

    @param ResAllocProtocol Pointer to Host Bridge Resource
        Allocation Protocol.
    @param RbIoProtocolBuffer Pointer to Root Bridge I/O Protocol.
    @param RbCount Root Bridge counter.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/

EFI_STATUS HbNotifyCspBeginResourceAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}


/**
    This procedure will be invoked in PCI Host Bridge Protocol
    when NotifyPhase function is called with phase
    EfiPciHostBridgeAllocateResources.

    @param ResAllocProtocol Pointer to Host Bridge Resource
        Allocation Protocol.
    @param RbIoProtocolBuffer Pointer to Root Bridge I/O Protocol.
    @param RbCount Root Bridge counter.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/

EFI_STATUS HbNotifyCspAllocateResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}

/**
    This procedure will be invoked in PCI Host Bridge Protocol
    when NotifyPhase function is called with phase
    EfiPciHostBridgeSetResources.

    @param ResAllocProtocol Pointer to Host Bridge Resource
        Allocation Protocol.
    @param RbIoProtocolBuffer Pointer to Root Bridge I/O Protocol.
    @param RbCount Root Bridge counter.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbNotifyCspSetResources (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}


/**
    This procedure will be invoked in PCI Host Bridge Protocol
    when NotifyPhase function is called with phase
    EfiPciHostBridgeEndResourceAllocation

    @param ResAllocProtocol Pointer to Host Bridge Resource
        Allocation Protocol.
    @param RbIoProtocolBuffer Pointer to Root Bridge I/O Protocol.
    @param RbCount Root Bridge counter.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbNotifyCspEndResourceAllocation (
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                  **RbIoProtocolBuffer,
    IN UINTN                                            RbCount )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}


/**
    This procedure will be invoked in PCI Host Bridge Protocol
    StartBusEnumeration function, it must prepare initial Bus
    ACPI Resource

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData Pointer to Root Bridge private structure data.
    @param RootBrgIndex Root Bridge index (0 Based).

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspStartBusEnumeration (
							IN PCI_HOST_BRG_DATA    *HostBrgData,
							IN PCI_ROOT_BRG_DATA    *RootBrgData,
							IN UINTN                RootBrgIndex )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    // Any Additional Variables goes here


    return Status;
}

/**
    This procedure will be invoked in PCI Host Bridge Protocol
    SubmitBusNumbers function.

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData Pointer to Root Bridge private structure data.
    @param RootBrgIndex Root Bridge index (0 Based).

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspSetBusNnumbers (
							IN PCI_HOST_BRG_DATA    *HostBrgData,
							IN PCI_ROOT_BRG_DATA    *RootBrgData,
							IN UINTN                RootBrgIndex )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}

/**
    This procedure will be invoked in PCI Host Bridge Protocol
    SubmitResources function.

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData Pointer to Root Bridge private structure data.
    @param RootBrgIndex Root Bridge index (0 Based).

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspSubmitResources (
								IN PCI_HOST_BRG_DATA    *HostBrgData,
								IN PCI_ROOT_BRG_DATA    *RootBrgData,
								IN UINTN                RootBrgIndex )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}

/** 
 * @internal
 * Attempts to set a variable. If attempt fails because the variable already exists with different attributes,
 * tries to delete the variable and to create it with the new attributes specified by Attributes. 
 *
 * @retval EFI_SUCCESS The variable has been successfully created.
 */
static EFI_STATUS AmiPciAccessCspBaseLibSetVariableWithNewAttributes(
    IN CHAR16 *Name, IN EFI_GUID *Guid, IN UINT32 Attributes,
    IN UINTN DataSize, IN VOID *Data    
)
{
    EFI_STATUS Status;
    
    Status = pRS->SetVariable(Name, Guid, Attributes, DataSize, Data);
    if (!EFI_ERROR(Status) || Status != EFI_INVALID_PARAMETER) return Status;

    Status = pRS->SetVariable(Name, Guid, 0, 0, NULL); // Delete the variable
    if (EFI_ERROR(Status)) return Status;

    return pRS->SetVariable(Name, Guid, Attributes, DataSize, Data);
}

/**
    This procedure will be invoked during PCI bus enumeration,
    it determines the PCI memory base address below 4GB whether
    it is overlapping the main memory, if it is overlapped, then
    updates MemoryCeiling variable and reboot.

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData Pointer to Root Bridge private structure data.
    @param RootBrgIndex Root Bridge index (0 Based).

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspAdjustMemoryMmioOverlap (
									IN PCI_HOST_BRG_DATA    *HostBrgData,
									IN PCI_ROOT_BRG_DATA    *RootBrgData,
									IN UINTN                RootBrgIndex )
{

// APTIOV_SERVER_OVERRIDE_START 
// commented whole method: we are doing the Dynamic Resource Allocation differently
#if 0
    EFI_STATUS  Status;

    EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap;
    UINTN                           NumberOfDescriptors;
    ASLR_QWORD_ASD                  *Res;
    UINTN                           i;
    EFI_PHYSICAL_ADDRESS            Highest4GMem    = 0;
    EFI_PHYSICAL_ADDRESS            LowestMMIO      = 0xffffffff;
    EFI_PHYSICAL_ADDRESS            LowestAllocMMIO = 0xffffffff;
    UINTN                           MemoryCeiling = 0; // Init to 0
    UINTN                           NewMemoryCeiling = 0xffffffff;
    UINTN                           DataSize = sizeof(UINT32);
    DXE_SERVICES                    *DxeSvc;

//------------------------------------
    Status = LibGetDxeSvcTbl( &DxeSvc );
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;

    pRS->GetVariable ( gMemoryCeilingVariable,
                       &gAmiGlobalVariableGuid,
                       NULL,
                       &DataSize,
                       &MemoryCeiling );
    PCI_TRACE((TRACE_PCI, "\nMemoryCeiling : %8X \n", MemoryCeiling));

    // Memory sizing uses memory ceiling to set top of memory.


    Status = DxeSvc->GetMemorySpaceMap( &NumberOfDescriptors,
                                        &MemorySpaceMap );
    ASSERT_EFI_ERROR(Status);

    // Find the lowest MMIO and lowest allocated MMIO in GCD.
    for (i = 0; i < NumberOfDescriptors; ++i)
    {
        EFI_GCD_MEMORY_SPACE_DESCRIPTOR *Descr = &MemorySpaceMap[i];
        EFI_PHYSICAL_ADDRESS Base = Descr->BaseAddress;

        // Find highest system below 4GB memory.
        // Treat any non MMIO as system memory. Not all system memory is
        // reported as system memory, such as SMM.

        if ((Descr->GcdMemoryType != EfiGcdMemoryTypeMemoryMappedIo) && (Base < LowestMMIO))
        {
            EFI_PHYSICAL_ADDRESS EndMem = Base + Descr->Length - 1;
            if ((EndMem > Highest4GMem) && (EndMem <= 0xffffffff))
                Highest4GMem = EndMem;

        // Find Lowest mmio above system memory.
        }
        else if (Descr->GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo)
        {
            if (Base >= 0x100000)
            {
                if (Base < LowestMMIO)
                    LowestMMIO = Base;

                // If ImageHandle, then MMIO is allocated.
                if ((Base < LowestAllocMMIO) && Descr->ImageHandle)
                    LowestAllocMMIO = Base;
            }
        }
    }

    pBS->FreePool(MemorySpaceMap);

    PCI_TRACE((TRACE_PCI, "\nLowestMMIO : %8X \n", LowestMMIO));
    PCI_TRACE((TRACE_PCI, "\nLowestAllocMMIO : %8X \n", LowestAllocMMIO));

    if (Highest4GMem + 1 != LowestMMIO) {
        PCI_TRACE((TRACE_PCI, "PciHostCSHooks: "));
        PCI_TRACE((TRACE_PCI, "System Memory and MMIO are not consequitive.\n"));
        PCI_TRACE((TRACE_PCI, "Top of Below 4G Memory: %lX", Highest4GMem));
        PCI_TRACE((TRACE_PCI, "Bottom of MMIO: %X\n", LowestMMIO));
    }


    // Find any MMIO that could not be allocated due to small of MMIO region.
    for (i = 0; i < RootBrgData->ResCount; ++i)
    {
        EFI_PHYSICAL_ADDRESS NeededBottomMmio;

        Res = RootBrgData->RbRes[i];

        // Any unallocated MMIO will have Res->_MIN set to zero for the MMIO
        // type.
        if (Res->Type != ASLRV_SPC_TYPE_MEM || Res->_GRA != 32 || Res->_MIN)
            continue;

        // Determine new memory ceiling variable needed to allocate this
        // memory.
        NeededBottomMmio = LowestAllocMMIO - Res->_LEN;

        // Round down. If resource is not allocated, _MAX contains
        // granularity.
        NeededBottomMmio &= ~Res->_MAX;
        if (NeededBottomMmio < NewMemoryCeiling)
            NewMemoryCeiling = (UINTN) NeededBottomMmio;
    }

    // Check if a NewMemory Ceiling is needed.
    if (NewMemoryCeiling < 0xffffffff)
    {
        if (!MemoryCeiling || (MemoryCeiling != NewMemoryCeiling))
        {

            // Set memory ceiling variable.
            AmiPciAccessCspBaseLibSetVariableWithNewAttributes(
                gMemoryCeilingVariable,
                &gAmiGlobalVariableGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof (UINT32),
                &NewMemoryCeiling
            );

            PCI_TRACE((TRACE_PCI, "\nResetting System for NewMemoryCeiling : %8X\n", NewMemoryCeiling));

        // Reset only needed of type of physical memory overlaps with MMIO.

#if (NV_SIMULATION != 1)
            // Don't reset system in case of NVRAM simulation
            pRS->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
#endif
        // Control should not come here if NV_SIMULATION = 0.
            return EFI_SUCCESS;
        }
    }

    // Check to see if Ceiling needs to be increased. If too low,
    // then part of the memory be not be usuable.
    if (MemoryCeiling != LowestAllocMMIO)
    {
        // Set memory ceiling variable.
        AmiPciAccessCspBaseLibSetVariableWithNewAttributes(
            gMemoryCeilingVariable, 
            &gAmiGlobalVariableGuid, 
            EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
            sizeof (UINT32), 
            &LowestAllocMMIO
        );

        PCI_TRACE((TRACE_PCI, "\nResetting System for LowestAllocMMIO : %8X\n", LowestAllocMMIO));

#if (NV_SIMULATION != 1)
        // Don't reset system in case of NVRAM simulation
        pRS->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
#endif
        // Control should not come here if NV_SIMULATION = 0.
    }
#endif
// APTIOV_SERVER_OVERRIDE_END 
// commented whole method: we are doing the Dynamic Resource Allocation differently
    return EFI_SUCCESS;
}

/**
    This function will be invoked after Initialization of generic
    part of the Host and Root Bridges.
    All Handles for PCIHostBrg and PciRootBrg has been created
    and Protocol Intergaces installed.

    @param HostBrgData Pointer to Host Bridge private structure data.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspBasicChipsetInit (
    IN PCI_HOST_BRG_DATA    *HostBrg0, UINTN	HbCount)
{
    EFI_STATUS              Status;

    // Now for Chipset which has PCI Express support we have to build
    // MCFG Table to inform OS about PCIE Ext cfg space mapping
    // APTIOV_SERVER_OVERRIDE_START
	#if 0
    Status = RegisterProtocolCallback( &gEfiAcpiTableProtocolGuid,\
                                       CreateNbAcpiTables, \
                                       NULL, \
                                       &mAcpiEvent,\
                                       &mAcpiReg );
	#endif
	Status = RegisterProtocolCallback( &gEfiGlobalNvsAreaProtocolGuid, // Added to support MCFG ACPI Table update.
                                       CreateNbAcpiTables,
                                       NULL,
                                       &mAcpiEvent,
                                       &mAcpiReg );
    // APTIOV_SERVER_OVERRIDE_END
   	ASSERT_EFI_ERROR(Status);				

    // If this protocol has been installed we can use it rigth on the way
    pBS->SignalEvent( mAcpiEvent );
//-------------------------------------------------------
//Here add some more code for basic HB init IF NEEDED!
//-------------------------------------------------------




//-------------------------------------------------------
    return EFI_SUCCESS;
}

/**
    This function will be invoked when Pci Host Bridge driver runs  
    out of resources.

    @param HostBrgData Pointer to Host Bridge private structure data.

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspGetProposedResources (
    IN PCI_HOST_BRG_DATA                            *HostBrgData,
    IN PCI_ROOT_BRG_DATA                            *RootBrgData,
    IN UINTN                                        RootBrgNumber )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}

/**
    This function is called for all the PCI controllers that
    the PCI bus driver finds.
    It can be used to Preprogram the controller.

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData Pointer to Root Bridge private structure data.
    @param RootBrgNumber Root Bridge number (0 Based).
    @param PciAddress Address of the controller on the PCI bus.
    @param Phase The phase during enumeration

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspPreprocessController (
    IN PCI_HOST_BRG_DATA                            *HostBrgData,
    IN PCI_ROOT_BRG_DATA                            *RootBrgData,
    IN UINTN                                        RootBrgNumber,
    IN EFI_PCI_CONFIGURATION_ADDRESS                PciAddress,
    IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}
// APTIOV_SERVER_OVERRIDE_START
// RbIndex to Socket and Stack Index Mapping
/**
    This function locates IIOUDS structure and
    updates RB Index to Socket and Stack Index mapping.

    @param VOID

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS
InitRbIndexSocketStackMap (
    VOID )
{
	UINTN 				RbIndex = 0;
	UINTN 				SocketIndex = 0;
	UINTN 				StackIndex = 0;
	EFI_STATUS			Status;
// APTIOV_SERVER_OVERRIDE_START : Added to support FPGA
#if FPGA_SUPPORT
    FPGA_CONFIGURATION  FpgaConfiguration;
#endif
// APTIOV_SERVER_OVERRIDE_END : Added to support FPGA
	//
 	// Locate IIOUDS protocol if not located already
 	//
 	if ( gIioUds == NULL ) {
		Status = pBS->LocateProtocol(
						&gEfiIioUdsProtocolGuid,
						NULL,
						&gIioUds );
		ASSERT_EFI_ERROR (Status);
		if ( EFI_ERROR(Status) ) {
			return Status;
		}
 	}

    //
    // Loop through all IIO stacks of all sockets that are present
    //
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) {
          continue;
        }
        for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) {
            if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) {
                continue;
            }
				
            //
            // Increment RbIndex
            //
            RbIndex++;
        } // for loop for Stack
    } // for loop for Socket

// APTIOV_SERVER_OVERRIDE_START : Added to support FPGA
#if FPGA_SUPPORT
    Status = FpgaConfigurationGetValues (&FpgaConfiguration);
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
        if (FpgaConfiguration.FpgaSktActive & (1 << SocketIndex)) {
            RbIndex++;
        }
    }
#endif
// APTIOV_SERVER_OVERRIDE_END : Added to support FPGA

    //
    // Update MaxRbIndex value
    //
    gMaxRbIndex = RbIndex;
    PCI_TRACE((TRACE_PCI,"\ngMaxRbIndex: %x \n);", gMaxRbIndex )); 	
    
    return EFI_SUCCESS;
}

// OOR Support
#if CRB_OUT_OF_RESOURCE_SUPPORT

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   PrintBestRatioAfterSocketAdjustment
//
// Description: This function prints best socket levelRatio's
//
// Input:       OutOfResrcFailureType - MMIO/IO
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PrintBestRatioAfterSocketAdjustment (
    UINT8 OutOfResrcFailureType
)
{
	UINT8 SocketIndex;
	for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
	{
		if (OutOfResrcFailureType == MMIOFAILURE )
		{
			PCI_TRACE((TRACE_PCI, "\n MMIO:SocketIndex=%d Newly Calculated BestRatio=0x%x\n",SocketIndex,CurrentMmioIoRatios.MMIO [SocketIndex]));
			PCI_TRACE((TRACE_PCI, "\n MMIO:SocketIndex=%d CurrentBoot Ratio=0x%x\n",SocketIndex,AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex]));
		}
		if (OutOfResrcFailureType == IOFAILURE )
		{
			PCI_TRACE((TRACE_PCI, "\n IO:SocketIndex=%d Newly Calculated BestRatio=0x%x\n",SocketIndex,CurrentMmioIoRatios.IO [SocketIndex]));
			PCI_TRACE((TRACE_PCI, "\n IO:SocketIndex=%d CurrentBoot Ratio=0x%x\n",SocketIndex,AutoAdjustMmioIoVariable.IoRatio [SocketIndex]));
		}
	}
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   ReApplyNewBase
//
// Description: This function re-apply the newly calculated base (IohOutofResInfo Structure)
//
// Input:       OutOfResrcFailureType - MMIO/IO
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ReApplyNewBase(
    UINT8   OutOfResrcFailureType)
{

  UINT8           StackIndex;
  UINT8           SocketIndex;
  UINT64          LastStackMmioLimit = 0;
  UINT64          LastStackIoLimit = 0;
  
  for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
  {
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
      if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
        continue;
         
      if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
        continue;
      
      if (OutOfResrcFailureType == MMIOFAILURE)
      {
        if (SocketIndex == 0 && StackIndex == 0 )
        {
         IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex] = IohOutofResInfo.MmioBase_Stack_Level[SocketIndex][StackIndex] + AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex][StackIndex]* SAD_MMIOL_GRANTY_4MB;
          LastStackMmioLimit = IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex];
        }
        else
        {
            IohOutofResInfo.MmioBase_Stack_Level[SocketIndex][StackIndex] =  LastStackMmioLimit;
            IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex] = IohOutofResInfo.MmioBase_Stack_Level[SocketIndex][StackIndex] + AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex][StackIndex]* SAD_MMIOL_GRANTY_4MB;
            LastStackMmioLimit = IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex];
        }
        
        PCI_TRACE((TRACE_PCI, "\n ReApplyNewBase MMIO : Socket=%d Stack=%d  LastStackMmioLimit=%lx MmioBase_Stack_Level=%lx\n",SocketIndex,StackIndex,LastStackMmioLimit,IohOutofResInfo.MmioBase_Stack_Level[SocketIndex][StackIndex]));
        PCI_TRACE((TRACE_PCI, "\n ReApplyNewBase MMIO : Socket=%d Stack=%d  LastStackMmioLimit=%lx MmioLimit_Stack_Level=%lx\n",SocketIndex,StackIndex,LastStackMmioLimit,IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex]));
      }
      
      if (OutOfResrcFailureType == IOFAILURE)
      {
        
        if (SocketIndex == 0 && StackIndex == 0 )
        {
          IohOutofResInfo.IoLimit_Stack_Level[SocketIndex][StackIndex] = IohOutofResInfo.IoBase_Stack_Level[SocketIndex][StackIndex] + AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketIndex][StackIndex]* SAD_LEG_IO_GRANTY_2KB;
          LastStackIoLimit = IohOutofResInfo.IoLimit_Stack_Level[SocketIndex][StackIndex];
        }
        else
        {
            IohOutofResInfo.IoBase_Stack_Level[SocketIndex][StackIndex] =  LastStackIoLimit;
            IohOutofResInfo.IoLimit_Stack_Level[SocketIndex][StackIndex] = IohOutofResInfo.IoBase_Stack_Level[SocketIndex][StackIndex] + AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketIndex][StackIndex]* SAD_LEG_IO_GRANTY_2KB;
            LastStackIoLimit = IohOutofResInfo.IoLimit_Stack_Level[SocketIndex][StackIndex];
      
        }
                
        PCI_TRACE((TRACE_PCI, "\n ReApplyNewBase IO : Socket=%d Stack=%d  LastStackIoLimit=%lx IoBase_Stack_Level=%lx\n",SocketIndex,StackIndex,LastStackIoLimit,IohOutofResInfo.IoBase_Stack_Level[SocketIndex][StackIndex]));
        PCI_TRACE((TRACE_PCI, "\n ReApplyNewBase IO : Socket=%d Stack=%d  LastStackIoLimit=%lx IoLimit_Stack_Level=%lx\n",SocketIndex,StackIndex,LastStackIoLimit,IohOutofResInfo.IoLimit_Stack_Level[SocketIndex][StackIndex]));
              
      }
    }
    
   }
  
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CheckExtraChunks
//
// Description: This function checks whether  newly calculated base in sufficient or need to re-adjust
//
// Input:       OutOfResrcFailureType - MMIO/IO
//
// Output:      NotPossible (TRUE)  - Need to try OEM defined Table
//				NotPossible (FALSE) - Found a new base
// 				Adjustmentneeded (TRUE)  - New base is not the correct one. Need to find base again.
//				Adjustmentneeded (FALSE) - New base is the correct one. 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CheckExtraChunks(
    UINT8   OutOfResrcFailureType,
    BOOLEAN *NotPossibleFlag,
    BOOLEAN *Adjustmentneeded)
{
  UINT8   SocketIndex;
  UINT16  TotalRequiredRatio = 0 ;
  BOOLEAN ResourceNotEnough = FALSE;
  
 
  for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
  {
    if (OutOfResrcFailureType == MMIOFAILURE)
    {
      TotalRequiredRatio = TotalRequiredRatio + RequiredMMIOIoRatio.MMIO [SocketIndex];
      if (TotalRequiredRatio > (0xFC000000 - IohOutofResInfo.MmioBase_Stack_Level[0][0])/SAD_MMIOL_GRANTY_4MB)
      {
        PCI_TRACE((TRACE_PCI, "\n MMIO : No Resource available!!!!!. NotPossibleFlag=%d\n",*NotPossibleFlag));
        *NotPossibleFlag = TRUE;
        return;
      }
    }
    
    else
    {
      TotalRequiredRatio = TotalRequiredRatio + RequiredMMIOIoRatio.IO [SocketIndex];
      if (TotalRequiredRatio > (0x10000 - IohOutofResInfo.MmioBase_Stack_Level[0][0])/SAD_LEG_IO_GRANTY_2KB)
      {
         PCI_TRACE((TRACE_PCI, "\n IO : No Resource available!!!!!. NotPossibleFlag=%d\n",*NotPossibleFlag));
         *NotPossibleFlag = TRUE;
          return;
      }
    }
  }
  
  for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
  {
    if (OutOfResrcFailureType == MMIOFAILURE)
    {
      if (RequiredMMIOIoRatio.MMIO [SocketIndex] > CurrentMmioIoRatios.MMIO [SocketIndex])
      {
          IohOutofResInfo.MmmioOutofResrcFlag [SocketIndex] = 1; //  : Please comment
          ResourceNotEnough = TRUE;
          break; // Still resource not enough.
      }
    }
    
    if (OutOfResrcFailureType == IOFAILURE)
    {
      if (RequiredMMIOIoRatio.IO [SocketIndex] > CurrentMmioIoRatios.IO [SocketIndex])
      {
          IohOutofResInfo.IoOutofResrcFlag [SocketIndex] = 1;
          ResourceNotEnough = TRUE;
          break; // Still resource not enough.
      }
   }
  }
  
  if (SocketIndex == NumIoH && !ResourceNotEnough)
  {
    PCI_TRACE((TRACE_PCI, "\n Found Best Ratio !!!!!\n"));
    PCI_TRACE((TRACE_PCI, "\n CheckExtraChunks *NotPossibleFlag=%d\n",*NotPossibleFlag));
    *NotPossibleFlag = FALSE;
    *Adjustmentneeded = FALSE;
     return;
  }
  
  for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
  {
    if (ValidIOH[SocketIndex] != 0xFF)
    {
      if (OutOfResrcFailureType == MMIOFAILURE)
      {
        if (RequiredMMIOIoRatio.MMIO [SocketIndex] == CurrentMmioIoRatios.MMIO [SocketIndex])
          continue;
        else
          break;
      }
      else  
      {
        if (RequiredMMIOIoRatio.IO [SocketIndex] == CurrentMmioIoRatios.IO [SocketIndex])
          continue;
        else
          break;
      }
    }
  }
    
  if (SocketIndex == NumIoH)
     *NotPossibleFlag = TRUE;
  else
  {
      *NotPossibleFlag = FALSE;
      *Adjustmentneeded = TRUE;
  }
    PCI_TRACE((TRACE_PCI, "\n CheckExtraChunks *NotPossibleFlag=%d *Adjustmentneeded =%d\n",*NotPossibleFlag,*Adjustmentneeded));
                  
    
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   ReCompareSocketNewRatioWithCurrentBootRatio
//
// Description: This function re-compares Socket Current and New Ratio
//
// Input:       OutOfResrcFailureType - MMIO/IO
//
// Output:      TRUE - Need re-adjustment , FALSE no need to have re-adjustment. 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN ReCompareSocketNewRatioWithCurrentBootRatio(
    UINT8   OutOfResrcFailureType
    )
{
  UINT8   SocketIndex;
  UINT8   StackIndex;
  BOOLEAN NeedRatioAdjustment = FALSE;
  
  //
  // Comparing Newly got Ratio's with current boot Ratio,if same then there is no possible way
  //
    PCI_TRACE((TRACE_PCI, "\n Re-Comparing New Adjusted Ratio's with the Current Boot Ratio\n"));
    for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
    {
      if (OutOfResrcFailureType == MMIOFAILURE)
      {
        if (AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex] != CurrentMmioIoRatios.MMIO [SocketIndex])
        {
          NeedRatioAdjustment = TRUE;
          AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex] = CurrentMmioIoRatios.MMIO [SocketIndex];
          
          PCI_TRACE((TRACE_PCI, "\n MMIO : Ratio's changed for Socket=%d \n",SocketIndex));
          for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
          {
            if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
              continue;
            
              if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
              continue;
                                              
            if (AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [SocketIndex][StackIndex] != CurrentMmioIoRatios.MMIO_Stack_Level [SocketIndex] [StackIndex])
            {
              AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [SocketIndex] [StackIndex] = CurrentMmioIoRatios.MMIO_Stack_Level [SocketIndex] [StackIndex];
              DEBUG((EFI_D_ERROR, "\n MMIO : Ratio's changed for StackIndex=%d \n",StackIndex));
            }
          }
        }
      }
      
      if (OutOfResrcFailureType == IOFAILURE)
      {
        if (AutoAdjustMmioIoVariable.IoRatio [SocketIndex] != CurrentMmioIoRatios.IO [SocketIndex])
        {
          NeedRatioAdjustment = TRUE;
          AutoAdjustMmioIoVariable.IoRatio [SocketIndex] = CurrentMmioIoRatios.IO [SocketIndex];
              
          PCI_TRACE((TRACE_PCI, "\n IO : Ratio's changed for Socket=%d \n",SocketIndex));
          for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
          {
            if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
              continue;
                
              if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
              continue;
                                                  
            if (AutoAdjustMmioIoVariable.IoRatio_Stack_Level [SocketIndex][StackIndex] != CurrentMmioIoRatios.IO_Stack_Level [SocketIndex] [StackIndex])
            {
              AutoAdjustMmioIoVariable.IoRatio_Stack_Level [SocketIndex] [StackIndex] = CurrentMmioIoRatios.IO_Stack_Level [SocketIndex] [StackIndex];
              PCI_TRACE((TRACE_PCI, "\n IO : Ratio's changed for StackIndex=%d \n",StackIndex));
            }
          }
        }
      }
      
    }
    
    return  NeedRatioAdjustment;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CompareSocketNewRatioWithCurrentBootRatio
//
// Description: This function compares socket current boot Ratio and new Ratio
//               if no change, this is a not possible case.
//
// Input:       OutOfResrcFailureType
//
// Output:      NotPossibleFlag - TRUE/FALSE
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CompareSocketNewRatioWithCurrentBootRatio (
    UINT8   OutOfResrcFailureType,
    BOOLEAN *NotPossibleFlag
)
{
	UINT8   SocketIndex;
	UINT8   StackIndex;
	
	//
	// Comparing Newly got Ratio's with current boot Ratio,if same then there is no possible way
	//
	*NotPossibleFlag = TRUE;
	PCI_TRACE((TRACE_PCI, "\n Comparing New Adjusted Ratio's with the Current Boot Ratio\n"));
	for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
	{
		if (OutOfResrcFailureType == MMIOFAILURE)
		{
			if (AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex] != CurrentMmioIoRatios.MMIO [SocketIndex])
			{
				*NotPossibleFlag = FALSE;
				FoundMmioRatioAtleaseOnce = TRUE;
				AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex] = CurrentMmioIoRatios.MMIO [SocketIndex];
				
				PCI_TRACE((TRACE_PCI, "\n MMIO : Ratio's changed for Socket=%d \n",SocketIndex));
				for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
				{
					if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
                        continue;
					
    				if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
						continue;
															    					
					if (AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [SocketIndex][StackIndex] != CurrentMmioIoRatios.MMIO_Stack_Level [SocketIndex] [StackIndex])
					{
						AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [SocketIndex] [StackIndex] = CurrentMmioIoRatios.MMIO_Stack_Level [SocketIndex] [StackIndex];
						DEBUG ((EFI_D_INFO, "\n MMIO : Ratio's changed for StackIndex=%d \n",StackIndex));
					}
				}
			}
		}
		
		if (OutOfResrcFailureType == IOFAILURE)
		{
			if (AutoAdjustMmioIoVariable.IoRatio [SocketIndex] != CurrentMmioIoRatios.IO [SocketIndex])
			{
				*NotPossibleFlag = FALSE;
				FoundIoRatioAtleaseOnce = TRUE;
				AutoAdjustMmioIoVariable.IoRatio [SocketIndex] = CurrentMmioIoRatios.IO [SocketIndex];
						
		    PCI_TRACE((TRACE_PCI, "\n IO : Ratio's changed for Socket=%d \n",SocketIndex));
				for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
				{
					if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
		                continue;
							
		    		if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
						continue;
																	    					
					if (AutoAdjustMmioIoVariable.IoRatio_Stack_Level [SocketIndex][StackIndex] != CurrentMmioIoRatios.IO_Stack_Level [SocketIndex] [StackIndex])
					{
						AutoAdjustMmioIoVariable.IoRatio_Stack_Level [SocketIndex] [StackIndex] = CurrentMmioIoRatios.IO_Stack_Level [SocketIndex] [StackIndex];
						PCI_TRACE((TRACE_PCI, "\n IO : Ratio's changed for StackIndex=%d \n",StackIndex));
					}
				}
			}
		}
		
	}
	
	// re-check whether calculated ratio is sufficient or still needs adjustment
	// Ex :  4 socket machine : Socket 2 Stack 3 fails, then first ratio is calculation is based
	// on the current boot MMIObase of each socket. After adjustment, Base will be different for
	// affected Socket and calcuated Ratio may not be sufficient. Try until we find best ratio
	// We found one best Ratio. Need to confirm whether this Ratio is sufficient by calculating
	// ratio again.
	if (*NotPossibleFlag == FALSE)
	{
		if (OutOfResrcFailureType == MMIOFAILURE && FoundMmioRatioAtleaseOnce)
		{
      Adjustmentneeded = FALSE;
			PCI_TRACE((TRACE_PCI, "\n MMIO : Re-checking whether Current Calculated Ratio is sufficient\n"));
			// Apply the new base and calculate the Ratio again.
			do
			{
			  ReApplyNewBase(OutOfResrcFailureType);
			  CalculateRequiredSocketRatio(MMIOFAILURE);
			  CheckExtraChunks(OutOfResrcFailureType,NotPossibleFlag ,&Adjustmentneeded);
			  if(*NotPossibleFlag)
			  {
			      DEBUG((EFI_D_ERROR, "\n MMIO : NO VALID Socket's are present\n"));
			      DEBUG((EFI_D_ERROR, "\n System will try the OEM DEFINED TABLE\n"));
			      return;
			  }
			  
			  SocketResourceAdjustment(MMIOFAILURE,NotPossibleFlag);
			} while (ReCompareSocketNewRatioWithCurrentBootRatio(OutOfResrcFailureType) && Adjustmentneeded);
		}
		
		if (OutOfResrcFailureType == IOFAILURE && FoundIoRatioAtleaseOnce)
		{
      Adjustmentneeded = FALSE;
			PCI_TRACE((TRACE_PCI, "\n IO : Re-checking whether Current Calculated Ratio is sufficient\n"));
			// Apply the new base and calculate the Ratio again.
			do
			{
			  ReApplyNewBase(OutOfResrcFailureType);
			  CalculateRequiredSocketRatio(IOFAILURE);
			  CheckExtraChunks(OutOfResrcFailureType,NotPossibleFlag,&Adjustmentneeded);
			  if(*NotPossibleFlag)
			  {
			        DEBUG((EFI_D_ERROR, "\n IO : NO VALID Socket's are present\n"));
			        DEBUG((EFI_D_ERROR, "\n System will try the OEM DEFINED TABLE\n"));
			        return ;
			  }
			          
			  SocketResourceAdjustment(IOFAILURE,NotPossibleFlag);
			  
		
			} while (ReCompareSocketNewRatioWithCurrentBootRatio(OutOfResrcFailureType) && Adjustmentneeded);
		}
	}
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   AdjustStackAfterSocketRatioAdjustment
//
// Description: This get called after Socket Ratio Adjust is done
//
// Input:       UINT8  Idx
//              UINT8  OutOfResrcFailureType
//              UINT8  RemainChunk
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID AdjustStackAfterSocketRatioAdjustment (
    UINT8   Idx,
    UINT8   OutOfResrcFailureType,
    UINT8   RemainChunk )
{
    UINT8 StackIndex;
    UINT8 TempChunk = RemainChunk;
    UINT8 Index;
    // Update CurrentMmioIoRatios with RequiredMMIOIoRatio always
    for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) 
    {
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Idx].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
        
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            CurrentMmioIoRatios.MMIO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex]; 
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            CurrentMmioIoRatios.IO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex]; 
        }
    }

    if (RemainChunk)
    {
        while (RemainChunk)
        {
            for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) 
            {
                if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Idx].stackPresentBitmap) & (1 << StackIndex))) 
                    continue;
                
                if (OutOfResrcFailureType == MMIOFAILURE)
                {
                    if (Adjustmentneeded == FALSE && FoundMmioRatioAtleaseOnce)
                    {
                      if ((NumberofStacksPerCpu -1) != StackIndex) 
                      {
                        CurrentMmioIoRatios.MMIO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex]; 
                        continue;
                      }
                      else 
                      {
                        PCI_TRACE((TRACE_PCI, "\n MMIO : RemainChunks Added to Socket=%d , StackIndex=%d %d\n", Idx, StackIndex, RemainChunk ));
                        RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex] + RemainChunk;
                        CurrentMmioIoRatios.MMIO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex];
                        return;
                      }
                    }
                    
                    else
                    {
                  
                      PCI_TRACE((TRACE_PCI, "\n MMIO : RemainChunks Socket=%d , StackIndex=%d %d\n", Idx, StackIndex, RemainChunk ));
                      // PCI_TRACE((TRACE_PCI, "\n Required Ratio Before Applying MMIO Chunks Socket=%d , StackIndex=%d 0x%lx\n", Idx, StackIndex, RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex] ));
                      RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex] + 1;
                      CurrentMmioIoRatios.MMIO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex]; 
                      PCI_TRACE((TRACE_PCI, "\n Required Ratio after Applying MMIO Chunks Socket=%d , StackIndex=%d 0x%lx\n", Idx, StackIndex, RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][StackIndex] ));
                    }
                }
                if (OutOfResrcFailureType == IOFAILURE)
                {
                  if (Adjustmentneeded == FALSE && FoundIoRatioAtleaseOnce)
                  {
                     if ((NumberofStacksPerCpu -1) != StackIndex)
                     {
                       CurrentMmioIoRatios.IO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex]; 
                       continue;
                     }
                     else 
                     {
                       PCI_TRACE((TRACE_PCI, "\n IO : RemainChunks Added to Socket=%d , StackIndex=%d %d\n", Idx, StackIndex, RemainChunk ));
                       RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex] + RemainChunk;
                       CurrentMmioIoRatios.IO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex];
                       return;
                     }
                  }
                  
                  else
                  {
                    PCI_TRACE((TRACE_PCI, "\n IO : RemainChunks Socket=%d , StackIndex=%d %d\n", Idx, StackIndex, RemainChunk ));
                    // PCI_TRACE((TRACE_PCI, "\n Required Ratio Before Applying IO Chunks Socket=%d , StackIndex=%d 0x%lx\n", Idx, StackIndex, RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex] ));
                    RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex] + 1;
                    CurrentMmioIoRatios.IO_Stack_Level[Idx][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex]; 
                    PCI_TRACE((TRACE_PCI, "\n Required Ratio after Applying IO Chunks Socket=%d , StackIndex=%d 0x%lx\n", Idx, StackIndex, RequiredMMIOIoRatio.IO_Stack_Level[Idx][StackIndex] ));
                  }
                }
                
                //
                // If no Chunk remain we will break out of the Stack for Loop
                //
                if (RemainChunk)
                {
                    RemainChunk = RemainChunk - 1;
                    if (RemainChunk == 0) 
                    {
                      if (TempChunk < NumberofStacksPerCpu && (FoundMmioRatioAtleaseOnce || FoundIoRatioAtleaseOnce))
                      {
                        for (Index = NumberofStacksPerCpu ; Index > TempChunk ;)
                        {
                          PCI_TRACE((TRACE_PCI, "\n : RemainChunks less than number of stacks...Socket=%d , Index=%d %d\n", Idx, Index-1, TempChunk ));
                          Index -- ;
                          if (OutOfResrcFailureType == MMIOFAILURE)
                            CurrentMmioIoRatios.MMIO_Stack_Level[Idx][Index] = RequiredMMIOIoRatio.MMIO_Stack_Level[Idx][Index]; 
                          if (OutOfResrcFailureType == IOFAILURE)
                            CurrentMmioIoRatios.IO_Stack_Level[Idx][Index] = RequiredMMIOIoRatio.IO_Stack_Level[Idx][Index]; 
                            
                        }
                      }
                        break;
                    }
                }
                else
                    break;
            } // End of Stack
        } // End of while loop
    } // End of If loop
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:  	CompareSocketNewRatioWithCurrentBootRatio
//
// Description: This function re-compares Socket Current and New Ratio
//
// Input:       OutOfResrcFailureType - MMIO/IO
//
// Output:      None. 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID CompareStackNewRatioWithCurrentBootRatio (
    UINT8   OutOfResrcFailureType,
    UINTN   Socket
)
{
  UINT8    StackIndex;
  for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
  {
       if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid)
           continue;
       if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
           continue;
       
       if (OutOfResrcFailureType == MMIOFAILURE)
       {
           if (AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [Socket][StackIndex] != CurrentMmioIoRatios.MMIO_Stack_Level [Socket] [StackIndex])
           {
               AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [Socket] [StackIndex] = CurrentMmioIoRatios.MMIO_Stack_Level [Socket] [StackIndex];
               PCI_TRACE((TRACE_PCI, "\n CompareStackNewRatioWithCurrentBootRatio MMIO : Ratio's changed for Socket=%d Stack=%d\n",Socket,StackIndex));
           }
       }
       if (OutOfResrcFailureType == IOFAILURE)
       {
           if (AutoAdjustMmioIoVariable.IoRatio_Stack_Level [Socket][StackIndex] != CurrentMmioIoRatios.IO_Stack_Level [Socket] [StackIndex])
           {
               AutoAdjustMmioIoVariable.IoRatio_Stack_Level [Socket] [StackIndex] = CurrentMmioIoRatios.IO_Stack_Level [Socket] [StackIndex];
               PCI_TRACE((TRACE_PCI, "\n CompareStackNewRatioWithCurrentBootRatio IO : Ratio's changed for Socket=%d Stack=%d\n",Socket,StackIndex));
           }
       }
   }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   SocketResourceAdjustment
//
// Description: This function adjust the socket resource
//
// Input:       OutOfResrcFailureType
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SocketResourceAdjustment (
    UINT8    OutOfResrcFailureType,
    BOOLEAN    *NotPossibleFlag)
{
    //
    // Going to take the resource from the validIOH
    //
    UINT8   SocketIndex;
    UINT8   TempExtraChunks = 0;
    UINT8   Index1,Idx;
    UINT8   ExtraChunks;
    
    for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
    {
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            if (IohOutofResInfo.MmmioOutofResrcFlag [SocketIndex] != 1) // This IOH is not failed
                continue;
            
            //
            // ReApplyNewBase() might have already adjusted resources. If RR and CR are same, then don't do anything for this socket.
            // 
            if (RequiredMMIOIoRatio.MMIO [SocketIndex] <= CurrentMmioIoRatios.MMIO [SocketIndex] && FoundMmioRatioAtleaseOnce)
            {
              AdjustStackAfterSocketRatioAdjustment(SocketIndex,MMIOFAILURE,CurrentMmioIoRatios.MMIO[SocketIndex] - RequiredMMIOIoRatio.MMIO[SocketIndex]); // Affected CPU Stack
              CompareStackNewRatioWithCurrentBootRatio(OutOfResrcFailureType,SocketIndex);
              continue ; 
            }

            //
            // Going to use the VALIDIOH TABLE to help the affected IOH
            //
            Index1 = 0 ;
            TempExtraChunks =  0;
            
            while(ValidIOH [Index1] != 0xFF && Index1 < NumIoH) // ADD A CONIDTION VALID[ARRAY] NOT SHOULD NOT ARRAY BOUND
            {
                Idx = ValidIOH [Index1];
                if (CurrentMmioIoRatios.MMIO [Idx] < RequiredMMIOIoRatio.MMIO[Idx] )
                {
                  Index1++;
                  continue; 
                }
                ExtraChunks = (CurrentMmioIoRatios.MMIO [Idx] - RequiredMMIOIoRatio.MMIO[Idx]);
                
                if (RequiredMMIOIoRatio.MMIO [SocketIndex] < CurrentMmioIoRatios.MMIO [SocketIndex])
                {
                	if (SocketIndex != NumIoH-1) goto next_socket;
                    DEBUG ((EFI_D_INFO, "\n Socket MMIO : Currently Alloted Space (CPU) is more than Required Space\n"));
                    DEBUG ((EFI_D_INFO, "\n But Still couldn't get continous space\n"));
                    *NotPossibleFlag = TRUE;
                    return;
                }

                if ((RequiredMMIOIoRatio.MMIO [SocketIndex] > CurrentMmioIoRatios.MMIO [SocketIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    
                    //
                    // Check the residual resource from this CPU is sufficient
                    //
                    if ( (TempExtraChunks + CurrentMmioIoRatios.MMIO [SocketIndex]) < RequiredMMIOIoRatio.MMIO[SocketIndex])
                    {
                        // Socket level Adjustment
                        CurrentMmioIoRatios.MMIO[Idx] = CurrentMmioIoRatios.MMIO[Idx] - ExtraChunks;
                        CurrentMmioIoRatios.MMIO [SocketIndex] = CurrentMmioIoRatios.MMIO [SocketIndex] + ExtraChunks;
                        // Call to Adjust Stack
                        AdjustStackAfterSocketRatioAdjustment(Idx,MMIOFAILURE,CurrentMmioIoRatios.MMIO[Idx] - RequiredMMIOIoRatio.MMIO[Idx]);
                        IOHUsedDuringSocketMMIOAdjustment[Index1] = Idx;
                        
                        Index1++;
                        continue;
                    }
                    
                    CurrentMmioIoRatios.MMIO [Idx] = CurrentMmioIoRatios.MMIO[Idx] - (RequiredMMIOIoRatio.MMIO [SocketIndex] - CurrentMmioIoRatios.MMIO [SocketIndex]);
                    CurrentMmioIoRatios.MMIO [SocketIndex] = CurrentMmioIoRatios.MMIO [SocketIndex] + (RequiredMMIOIoRatio.MMIO [SocketIndex] - CurrentMmioIoRatios.MMIO [SocketIndex]);
                    if (CurrentMmioIoRatios.MMIO [SocketIndex] == RequiredMMIOIoRatio.MMIO[SocketIndex])
                    {
                        AdjustStackAfterSocketRatioAdjustment(Idx,MMIOFAILURE,CurrentMmioIoRatios.MMIO[Idx] - RequiredMMIOIoRatio.MMIO[Idx]);                         // Non-Affected CPU Stack
                        AdjustStackAfterSocketRatioAdjustment(SocketIndex,MMIOFAILURE,CurrentMmioIoRatios.MMIO[SocketIndex] - RequiredMMIOIoRatio.MMIO[SocketIndex]); // Affected CPU Stack
                        IOHUsedDuringSocketMMIOAdjustment[Index1] = Idx;
                        return;
                    }
                }
                
                //
                // if both Requested and Current Ratio's are same and if we have any valid stacks, apply the residual resource to the affectd stack.
                //                
                if ((RequiredMMIOIoRatio.MMIO [SocketIndex] == CurrentMmioIoRatios.MMIO [SocketIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    CurrentMmioIoRatios.MMIO[Idx] = CurrentMmioIoRatios.MMIO[Idx] - ExtraChunks;
                    CurrentMmioIoRatios.MMIO [SocketIndex] = CurrentMmioIoRatios.MMIO [SocketIndex] + ExtraChunks;
                    AdjustStackAfterSocketRatioAdjustment(Idx,MMIOFAILURE,CurrentMmioIoRatios.MMIO[Idx] - RequiredMMIOIoRatio.MMIO[Idx]);                         // Non-Affected CPU Stack
                    AdjustStackAfterSocketRatioAdjustment(SocketIndex,MMIOFAILURE,CurrentMmioIoRatios.MMIO[SocketIndex] - RequiredMMIOIoRatio.MMIO[SocketIndex]); // Affected CPU Stack
                    IOHUsedDuringSocketMMIOAdjustment[Index1] = Idx;
                    return;
                }
                Index1++;
            } // while(ValidIOH [Index1] != 0xFF)
            
            //
            // If Socket level resource is enough we can go ahead for stack level
            //
            if (CurrentMmioIoRatios.MMIO[SocketIndex] == RequiredMMIOIoRatio.MMIO[SocketIndex])
            {
                AdjustStackAfterSocketRatioAdjustment(Idx,MMIOFAILURE,CurrentMmioIoRatios.MMIO[Idx] - RequiredMMIOIoRatio.MMIO[Idx]);                         // Non-Affected CPU Stack
                AdjustStackAfterSocketRatioAdjustment(SocketIndex,MMIOFAILURE,CurrentMmioIoRatios.MMIO[SocketIndex] - RequiredMMIOIoRatio.MMIO[SocketIndex]); // Affected CPU Stack
                return;
            }
            if (CurrentMmioIoRatios.MMIO[SocketIndex] < RequiredMMIOIoRatio.MMIO[SocketIndex])
            {
                DEBUG ((EFI_D_INFO, "\n Socket MMIO : Not able to find space\n"));
                *NotPossibleFlag = TRUE;
                return;
            }
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            if (IohOutofResInfo.IoOutofResrcFlag [SocketIndex] != 1) // This IOH is not failed
                continue;
            
            //
            // ReApplyNewBase() might have already adjusted resources. If RR and CR are same, then don't do anything for this socket.
            // 
            if (RequiredMMIOIoRatio.IO [SocketIndex] <= CurrentMmioIoRatios.IO [SocketIndex] && FoundIoRatioAtleaseOnce)
            {
              AdjustStackAfterSocketRatioAdjustment(SocketIndex,IOFAILURE,CurrentMmioIoRatios.IO[SocketIndex] - RequiredMMIOIoRatio.IO[SocketIndex]); // Affected CPU Stack
              CompareStackNewRatioWithCurrentBootRatio(OutOfResrcFailureType,SocketIndex);
              continue ; 
            }
            //
            // Going to use the VALIDIOH TABLE to help the affected IOH
            //
            Index1 = 0 ;
            TempExtraChunks =  0;
            
            while(ValidIOH [Index1] != 0xFF && Index1 < NumIoH)
            {
                Idx = ValidIOH [Index1];
                if (CurrentMmioIoRatios.IO [Idx] < RequiredMMIOIoRatio.IO[Idx] )
                {
                  Index1++;
                  continue; 
                }
                ExtraChunks = (CurrentMmioIoRatios.IO [Idx] - RequiredMMIOIoRatio.IO[Idx]);
                if (RequiredMMIOIoRatio.IO [SocketIndex] < CurrentMmioIoRatios.IO [SocketIndex])
                {
                	if (SocketIndex != NumIoH-1) goto next_socket;
                	DEBUG ((EFI_D_INFO, "\n Socket IO : Currently Alloted Space (CPU) is more than Required Space\n"));
                	DEBUG ((EFI_D_INFO, "\n But Still couldn't get continous space\n"));
                	DEBUG ((EFI_D_INFO, "\n GOING to try MAX_SYSTEM_RESET_RETRYCOUNT_ON_RESOURCE_ERROR to fix the PCI RESOURCE GRANULARTIY issue\n"));
                    *NotPossibleFlag = TRUE;
                    return;
                }

                if ((RequiredMMIOIoRatio.IO [SocketIndex] > CurrentMmioIoRatios.IO [SocketIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    //
                    // Check the residual resource from this CPU is sufficient.
                    //
                    if ( (TempExtraChunks + CurrentMmioIoRatios.IO [SocketIndex]) < RequiredMMIOIoRatio.IO[SocketIndex])
                    {
                        // Socket level Adjustment.
                        CurrentMmioIoRatios.IO[Idx] = CurrentMmioIoRatios.IO[Idx] - ExtraChunks;
                        CurrentMmioIoRatios.IO [SocketIndex] = CurrentMmioIoRatios.IO [SocketIndex] + ExtraChunks;
                        // Call to Adjust Stack
                        AdjustStackAfterSocketRatioAdjustment(Idx,IOFAILURE,CurrentMmioIoRatios.IO[Idx] - RequiredMMIOIoRatio.IO[Idx]);
                        IOHUsedDuringSocketIOAdjustment[Index1] = Idx;
                        
                        Index1++;
                        continue;
                    }

                    CurrentMmioIoRatios.IO [Idx] = CurrentMmioIoRatios.IO[Idx] - (RequiredMMIOIoRatio.IO [SocketIndex] - CurrentMmioIoRatios.IO [SocketIndex]);
                    CurrentMmioIoRatios.IO [SocketIndex] = CurrentMmioIoRatios.IO [SocketIndex] + (RequiredMMIOIoRatio.IO [SocketIndex] - CurrentMmioIoRatios.IO [SocketIndex]);
                    if (CurrentMmioIoRatios.IO [SocketIndex] == RequiredMMIOIoRatio.IO[SocketIndex])
                    {
                        AdjustStackAfterSocketRatioAdjustment(Idx,IOFAILURE,CurrentMmioIoRatios.IO[Idx] - RequiredMMIOIoRatio.IO[Idx]);                         // Non-Affected CPU Stack
                        AdjustStackAfterSocketRatioAdjustment(SocketIndex,IOFAILURE,CurrentMmioIoRatios.IO[SocketIndex] - RequiredMMIOIoRatio.IO[SocketIndex]); // Affected CPU Stack
                        IOHUsedDuringSocketIOAdjustment[Index1] = Idx;
                        return;
                    }
                }
                
                //
                // if both Requested and Current Ratio's are same and if we have any valid stacks, apply the residual resource to the affectd stack.
                //
                if ((RequiredMMIOIoRatio.IO [SocketIndex] == CurrentMmioIoRatios.IO [SocketIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    CurrentMmioIoRatios.IO[Idx] = CurrentMmioIoRatios.IO[Idx] - ExtraChunks;
                    CurrentMmioIoRatios.IO [SocketIndex] = CurrentMmioIoRatios.IO [SocketIndex] + ExtraChunks;
                    AdjustStackAfterSocketRatioAdjustment(Idx,IOFAILURE,CurrentMmioIoRatios.IO[Idx] - RequiredMMIOIoRatio.IO[Idx]);                         // Non-Affected CPU Stack
                    AdjustStackAfterSocketRatioAdjustment(SocketIndex,IOFAILURE,CurrentMmioIoRatios.IO[SocketIndex] - RequiredMMIOIoRatio.IO[SocketIndex]); // Affected CPU Stack
                    IOHUsedDuringSocketIOAdjustment[Index1] = Idx;
                    return;
                }
                Index1++; // Try the next valid Socket
            } // while(ValidIOH [Index1] != 0xFF)
            
            //
            // If Socket level resource is enough we can go ahead for stack level
            //
            if (CurrentMmioIoRatios.IO[SocketIndex] == RequiredMMIOIoRatio.IO[SocketIndex])
            {
                AdjustStackAfterSocketRatioAdjustment(Idx,IOFAILURE,CurrentMmioIoRatios.IO[Idx] - RequiredMMIOIoRatio.IO[Idx]);                         // Non-Affected CPU Stack
                AdjustStackAfterSocketRatioAdjustment(SocketIndex,IOFAILURE,CurrentMmioIoRatios.IO[SocketIndex] - RequiredMMIOIoRatio.IO[SocketIndex]); // Affected CPU Stack
                return;
            }
            if (CurrentMmioIoRatios.IO[SocketIndex] < RequiredMMIOIoRatio.IO[SocketIndex])
            {
                DEBUG ((EFI_D_INFO, "\n Socket IO : Not able to find space\n"));
                *NotPossibleFlag = TRUE;
                return;
            }
        } // if (OutOfResrcFailureType == IOFAILURE)
next_socket : ; // do nothing
    } // End of Socket
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CheckValidSocketsAvail
//
// Description: This function checks for any valid socket available
//
// Input:       OutOfResrcFailureType
//
// Output:      NotPossibleFlag - TRUE/FALSE
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
CheckValidSocketsAvail (
    BOOLEAN *NotPossibleFlag
)
{
    UINT8   SocketIndex;
    for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
    {
        if (ValidIOH[SocketIndex] != 0xFF)
            break;
    }

    //
    // No Socket is valid : This is a not possible case
    //  Need to try OEM Defined Table
    if (SocketIndex == NumIoH)
        *NotPossibleFlag = TRUE;
    else
        *NotPossibleFlag = FALSE;
    
    return;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   PrintValidSocketInfo
//
// Description: This function prints the valid socket info
//
// Input:       None
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PrintValidSocketInfo ()
{
    UINT8 SocketIndex;
    for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
    {
        if (ValidIOH[SocketIndex] != 0xFF)
            PCI_TRACE((TRACE_PCI, "\n VALID CPU=%d\n",ValidIOH[SocketIndex]));
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   InitAndFillValidSocketIndex
//
// Description: This function finds Valid sockets that are available
//
// Input:       OutOfResrcFailureType - MMIO/IO
//
// Output:      ValidIOH - Array has valid socket Info
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID InitAndFillValidSocketIndex (
    UINT8 OutOfResrcFailureType
)
{
    UINT8 SocketIndex;
    UINT8 Index1;
    
    for (SocketIndex = 0, Index1 = 0; SocketIndex < NumIoH ; SocketIndex++)
    {
        ValidIOH[SocketIndex] = 0xFF;
        if (OutOfResrcFailureType == MMIOFAILURE)    
        {
            if (IohOutofResInfo.MmmioOutofResrcFlag [SocketIndex] != 1)
            {
                ValidIOH[Index1] = SocketIndex;
                Index1++;
            }
        }
        if (OutOfResrcFailureType == IOFAILURE)    
        {
            if (IohOutofResInfo.IoOutofResrcFlag [SocketIndex] != 1)
            {   
                ValidIOH[Index1] = SocketIndex;
                Index1++;
            }
        }
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CollectCurrentBootRatio
//
// Description: This function will get the Current Boot Ratio (MMIOL/IO) for stacks
//
// Input:       OutOfResrcFailureType - MMIO/IO
//              MMIO_Failed_Socket    - MMIO_Failed_Socket
//
// Output:      CurrentMmioIoRatios   - Current Boot Ratio's
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CollectCurrentBootRatio (
    UINT8   OutOfResrcFailureType,
    UINTN   Socket
)
{
    UINT8   StackIndex;
    
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
            continue;
        
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
        
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [Socket][StackIndex] ;
            PCI_TRACE((TRACE_PCI, "\n MMIO: Stack=%d CurrentBootMMIO chunk (4MB GRA)=0x%x\n", StackIndex, AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [Socket][StackIndex]));
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex] = AutoAdjustMmioIoVariable.IoRatio_Stack_Level [Socket][StackIndex] ;
            PCI_TRACE((TRACE_PCI, "\n IO: Stack=%d CurrentBootIO chunk (2KB GRA)=0x%x\n", StackIndex, AutoAdjustMmioIoVariable.IoRatio_Stack_Level [Socket][StackIndex]));
        }
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   FindFirstbitset
//
// Description: This function will find whether GRA is aligned with BaseAddress
//
// Input:       value - Base Address or GRA
//
// Output:      Returns the BIT SET Position in BaseAddress and GRA
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT64 FindFirstbitset (
    UINT64 value
)
{
    UINT64 Firstbitset = 0 ;
    while (value & 0xFFFFFFFFFFFFFFFF)
    {
        value = value >> 1 ;
        Firstbitset = Firstbitset + 1;
        if (value & 1) break;
    }
    return Firstbitset;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CalculateRequiredRatio
//
// Description: This function will find the Required Ratio (MMIOL/IO) for stacks
//
// Input:       OutOfResrcFailureType - MMIO/IO
//              Socket                - Stacks under this Socket
//              SocketLevelInfoNeeded - SocketLevel Ratio Needed or not.
//
// Output:      RequiredMMIOIoRatio   - Array filled with RequiredMMIOIoRatio
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CalculateRequiredRatio (
    UINT8   OutOfResrcFailureType,
    UINTN   Socket,
    BOOLEAN    SocketLevelInfoNeeded)
{
    UINT8           StackIndex;
    int             FailedIoIndex;
    int             FailedMMioIndex;
    UINT64          ExtraSpaceNeededCount = 0;
    UINT64          MaxGra ;
    UINT64          BaseAddress;
    UINT64          IoBaseAddress;
    UINT64          TempBaseAddressBeforeAlignemnt;
    UINT64          TempIoBaseAddressBeforeAlignemnt;
    UINT16          TotalRequiredRatio = 0 ;
    UINT16          TotalCurrentRatio = 0 ;
    
    UINT16          TempTotalRequiredRatio [MAX_SOCKET] = {0} ;
    UINT16          TempTotalCurrentRatio [MAX_SOCKET]= {0} ;
    
    UINT16			    TempTotalSocketCurrentRatio = 0;
    UINT16			    TempTotalSocketRequiredRatio = 0;

    UINT64          FistbitSetPoistioninMaxGra;
    UINT64          FistbitSetPoistioninTempBaseAddress;
    BOOLEAN         Stack0_MMIO_Updated_Once = FALSE;
    BOOLEAN         Stack0_IO_Updated_Once = FALSE;
    UINT16          TempRequiredMMIOIoRatio = 0;
    UINT16          TempRequiredIoRatio = 0;
    UINT16          PreviousRatio = 0;
    BOOLEAN         Stack_MmioBaseAddressExceededeMaxLimit[MAX_SOCKET] [MAX_IIO_STACK] = {0};
    BOOLEAN         Stack_IoBaseAddressExceededeMaxLimit[MAX_SOCKET] [MAX_IIO_STACK] = {0};
    
    static UINT64   NewBase = 0;
    static UINT64   NewIoBase = 0;

    
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
            continue;
        
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
        
        //
        //  MMIO Failure
        //
        
        /*  MMIO FLOW ->
         *  1.  MMIO can be of 2 types MMIO32 Prefectable and MMIO32 non-prefectable.
         *  2.  AMI PCI Driver will try to allocate the MMIO32P first from top-down and followed by MMIO32 NP.
         *  3.  As MMIO32NP will be at the bottom, it needs to be aligned with BaseAddress. 
         *  4.  If not aligned, we will be finding the Aligned BaseAddress by adding "SAD_MMIOL_GRANTY (0x400000)" to BaseAddress.
         *      until we get aligned Address.
         *  5.  If aligned, current baseAddress is enough to allocate and goto step 9. 
         *  6.  While adding "SAD_MMIOL_GRANTY(0x400000)", we will be checking whether new BaseAddress reaches 
         *      MAX_MMIOL Limit (0xFC000000). 
         *  7.  If new BaseAddress exceeds MAX_MMIOL Limit, we will try to lower the Current base of the Socket[x]stack[0]
         *      with how much value exceeded MMIOL LIMIT. If lowered new base is less than MMCFG base,
         *      this situation will be not possible case.
         *  8.  If new base is greater than MMCFG base,repeat steps from 4.   
         *  9.  Based on the new base, required ration will be calculated for all stacks.
         *  10. Extra Resources from the valid stacks will be added to the affected stack until it's required ratio 
         *      gets satisfied.
         */
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            MaxGra = 0;
            RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] = 0;
            for (FailedMMioIndex = MMIO32_TYPES -1 ; FailedMMioIndex >= 0; FailedMMioIndex--)
            {
                if (IohOutofResInfo.MmioRequested_Stack_Level[Socket][StackIndex][FailedMMioIndex] > 0 ) 
                {
                    PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_1 Stack=%d MMIO RequestedLength=0x%x MMIO32_TYPES=%d\n", StackIndex, IohOutofResInfo.MmioRequested_Stack_Level[Socket][StackIndex][FailedMMioIndex], FailedMMioIndex));
                    if (StackIndex == 0 )
                    {
                    

                            if (Stack0_MMIO_Updated_Once == FALSE)
                            {
                                if (IohOutofResInfo.MmioBase_Stack_Level[Socket][StackIndex] == 0xFFFFFFFF) continue; 
                                TempBaseAddressBeforeAlignemnt = IohOutofResInfo.MmioBase_Stack_Level[Socket][StackIndex];
                                BaseAddress = TempBaseAddressBeforeAlignemnt;
                            }
                            else
                            {
                                BaseAddress = TempBaseAddressBeforeAlignemnt;
                            }

                    }
                    else
                    {
                      if (Stack0_MMIO_Updated_Once == FALSE)
                      {
                            //
                            //  Example : If there is no MMIO Requested in socket STACK 0 and if there is any failure in STK 3
                            //  Without this condition check, TempIoBaseAddressBeforeAlignemnt will return wrong data.
                            //
                                                                       
                            TempBaseAddressBeforeAlignemnt = IohOutofResInfo.MmioBase_Stack_Level[Socket][0]; 
                            if (TempBaseAddressBeforeAlignemnt == 0xFFFFFFFF && (IohOutofResInfo.MmioBase_Stack_Level[Socket][StackIndex] == 0xFFFFFFFF) )
                              continue;
                            else
                            {
                              if (TempBaseAddressBeforeAlignemnt == 0xFFFFFFFF) 
                                TempBaseAddressBeforeAlignemnt = IohOutofResInfo.MmioBase_Stack_Level[Socket][StackIndex];
                              else
                                TempBaseAddressBeforeAlignemnt = IohOutofResInfo.MmioBase_Stack_Level[Socket][0];
                            }
                                                      
                            Stack0_MMIO_Updated_Once = TRUE;
                      }
                        BaseAddress = TempBaseAddressBeforeAlignemnt  ;
                    }
                    
                    MaxGra = IohOutofResInfo.Mmio_Gra_Stack_Level[Socket][StackIndex][FailedMMioIndex] + 1;
                    PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_3 BaseAddress =%lx TempBaseAddressBeforeAlignemnt=%lx MaxGra=%lx\n", BaseAddress, TempBaseAddressBeforeAlignemnt, MaxGra));
                    FistbitSetPoistioninMaxGra = FindFirstbitset (MaxGra);
                    FistbitSetPoistioninTempBaseAddress = FindFirstbitset (TempBaseAddressBeforeAlignemnt);
                    
                    PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_4 FistbitSetPoistioninMaxGra =%lx FistbitSetPoistioninTempBaseAddress=%lx \n", FistbitSetPoistioninMaxGra, FistbitSetPoistioninTempBaseAddress));
                    if (FistbitSetPoistioninTempBaseAddress >= FistbitSetPoistioninMaxGra)
                    {
                        PreviousRatio = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_5 PreviousRatio =%lx \n", PreviousRatio));
                        
                        RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] = (UINT16)((IohOutofResInfo.MmioRequested_Stack_Level[Socket][StackIndex][FailedMMioIndex]) / SAD_MMIOL_GRANTY_4MB);
                        ExtraSpaceNeededCount = (IohOutofResInfo.MmioRequested_Stack_Level[Socket][StackIndex][FailedMMioIndex])%SAD_MMIOL_GRANTY_4MB;
                        if (ExtraSpaceNeededCount > 0)
                        {
                            RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] + 1;
                            PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_6 RequiredMMIOIoRatio.MMIO_Stack_Level[%d][%d] =%lx \n",Socket,StackIndex,RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]));
                        }
                        
                        TempBaseAddressBeforeAlignemnt = TempBaseAddressBeforeAlignemnt + RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] * SAD_MMIOL_GRANTY_4MB;
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_7 TempBaseAddressBeforeAlignemnt=%lx \n",TempBaseAddressBeforeAlignemnt));        
                        
                        RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] = PreviousRatio + (UINT16) ((TempBaseAddressBeforeAlignemnt - BaseAddress)/SAD_MMIOL_GRANTY_4MB);
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_8 RequiredMMIOIoRatio.MMIO_Stack_Level[%d][%d] =%lx \n",Socket,StackIndex,RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]));
                        
                        if ( Stack_MmioBaseAddressExceededeMaxLimit[Socket][StackIndex])
                        {
                            RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] + (UINT16) ((TempBaseAddressBeforeAlignemnt - 0xFC000000) /SAD_MMIOL_GRANTY_4MB) ; // MMIOL MAX LIMIT
                            PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_9 Socket=%d Stack=%d crossed the MAX MMIOL LIMIT 0xFC000000 \n",Socket, StackIndex));
                        }
                        else
                        {
                            PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_9A Socket=%d Stack=%d NumberofStacksPerCpu=%d \n",Socket, StackIndex,NumberofStacksPerCpu));	
                            if ((FailedMMioIndex == 0) && (StackIndex == (NumberofStacksPerCpu -1)) && (TempBaseAddressBeforeAlignemnt < IohOutofResInfo.MmioLimit_Stack_Level[Socket][StackIndex]) && AfterStackAdjust_SocketMMIO_AdjustNeeded == FALSE && SocketLevelMMIoAdjustmentNeeded == FALSE) 
                            {
                                PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_9B Socket=%d Stack=%d NumberofStacksPerCpu=%d \n",Socket, StackIndex,NumberofStacksPerCpu));	
                                RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] =  RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] + (UINT16)((IohOutofResInfo.MmioLimit_Stack_Level[Socket][StackIndex] - TempBaseAddressBeforeAlignemnt)/SAD_MMIOL_GRANTY_4MB);
                            }
                        }
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_10 RequiredMMIOIoRatio =%lx \n",RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]));
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_11 BaseAddress =%lx TempBaseAddressBeforeAlignemnt=%lx \n",BaseAddress,TempBaseAddressBeforeAlignemnt));
                        if (StackIndex == 0)
                            Stack0_MMIO_Updated_Once = TRUE;
                    }
                    else
                    {
                        while (FistbitSetPoistioninTempBaseAddress < FistbitSetPoistioninMaxGra)
                        {
                            TempBaseAddressBeforeAlignemnt = TempBaseAddressBeforeAlignemnt + SAD_MMIOL_GRANTY_4MB;
                            FistbitSetPoistioninTempBaseAddress = FindFirstbitset (TempBaseAddressBeforeAlignemnt);
                            if (TempBaseAddressBeforeAlignemnt > 0xFC000000) // MMIOL MAX LIMIT
                            {
                                Stack_MmioBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE;
                                Socket_Stack_MmioBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE; // Global variable.
                                PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_12 Socket=%d Stack=%d crossed the MAX MMIOL LIMIT 0xFC000000 \n",Socket, StackIndex));    
                            }
                        }
                        
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_13 TempBaseAddressBeforeAlignemnt=%lx \n",TempBaseAddressBeforeAlignemnt));
                        PreviousRatio = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                        
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_14 PreviousRatio =%lx \n",PreviousRatio));
                        RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] =  (UINT16)((IohOutofResInfo.MmioRequested_Stack_Level[Socket][StackIndex][FailedMMioIndex]) / SAD_MMIOL_GRANTY_4MB);
                        ExtraSpaceNeededCount = (IohOutofResInfo.MmioRequested_Stack_Level[Socket][StackIndex][FailedMMioIndex])%SAD_MMIOL_GRANTY_4MB;
                        if (ExtraSpaceNeededCount > 0)
                        {
                            RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] + 1;
                            PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_15 RequiredMMIOIoRatio.MMIO_Stack_Level[%d][%d] =%lx \n",Socket,StackIndex,RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]));
                        }
                        
                        TempBaseAddressBeforeAlignemnt = TempBaseAddressBeforeAlignemnt + RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] * SAD_MMIOL_GRANTY_4MB;
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_16 TempBaseAddressBeforeAlignemnt=%lx \n",TempBaseAddressBeforeAlignemnt));
                        RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] = PreviousRatio + (UINT16)((TempBaseAddressBeforeAlignemnt - BaseAddress)/SAD_MMIOL_GRANTY_4MB);
                        
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_17 RequiredMMIOIoRatio =%lx \n",RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]));
                        PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_18 BaseAddress =%lx TempBaseAddressBeforeAlignemnt=%lx \n",BaseAddress,TempBaseAddressBeforeAlignemnt));

                        if (TempBaseAddressBeforeAlignemnt > 0xFC000000) // MMIOL MAX LIMIT
                        {
                            Stack_MmioBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE;
                            Socket_Stack_MmioBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE; // Global variable.
                            PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_19 Socket=%d Stack=%d crossed the MAX MMIOL LIMIT 0xFC000000 \n",Socket, StackIndex));
                        }

                        if (FailedMMioIndex == 0 && StackIndex == (NumberofStacksPerCpu -1)&& (TempBaseAddressBeforeAlignemnt < IohOutofResInfo.MmioLimit_Stack_Level[Socket][StackIndex]) && AfterStackAdjust_SocketMMIO_AdjustNeeded == FALSE  && SocketLevelMMIoAdjustmentNeeded == FALSE) 
                        {
                       		PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_19A Socket=%d Stack=%d NumberofStacksPerCpu=%d \n",Socket, StackIndex,NumberofStacksPerCpu));	
                            RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] =  RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] + (UINT16)((IohOutofResInfo.MmioLimit_Stack_Level[Socket][StackIndex] - TempBaseAddressBeforeAlignemnt)/SAD_MMIOL_GRANTY_4MB);
                            PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_19B RequiredMMIOIoRatio =%lx \n",RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]));
                        }
                        if (StackIndex == 0)
                            Stack0_MMIO_Updated_Once = TRUE;
                    }
                }
            }
            if(SocketLevelInfoNeeded)
            {
                PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_20 Socket=%d Stack=%d RequiredMMIOIoRatio.MMIO[Socket]=%lx \n",Socket, StackIndex,RequiredMMIOIoRatio.MMIO[Socket]));
                RequiredMMIOIoRatio.MMIO[Socket] = RequiredMMIOIoRatio.MMIO[Socket] + RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                PCI_TRACE((TRACE_PCI, "\n MMIO : STEP_21. Socket=%d Stack=%d RequiredMMIOIoRatio.MMIO[Socket]=%lx \n",Socket, StackIndex,RequiredMMIOIoRatio.MMIO[Socket]));
            }
        } // if (OutOfResrcFailureType == MMIOFAILURE)
        
        //
        // IOFAILURE
        //
        
        /*  IO FLOW ->
         *   1. There will be only one IO TYPE. 
         *   2  Same Logic will be applied for IO but IO_TYPES for loop will be executed only once.
         * 
         */
        if (OutOfResrcFailureType == IOFAILURE)
        {
            MaxGra = 0;
            RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] = 0;
            for (FailedIoIndex = IO_TYPES -1  ; FailedIoIndex >=0; FailedIoIndex--)
            {
                if (IohOutofResInfo.IoRequested_Stack_Level[Socket][StackIndex][FailedIoIndex] > 0 ) 
                {
                    PCI_TRACE((TRACE_PCI, "\n IO : STEP_1 Stack=%d IO RequestedLength=0x%x IO_TYPES=%d\n",StackIndex,IohOutofResInfo.IoRequested_Stack_Level[Socket][StackIndex][FailedIoIndex],FailedIoIndex));
                    if (StackIndex == 0 )
                    {
                           if (Stack0_IO_Updated_Once == FALSE)
                            {
                                if (IohOutofResInfo.IoBase_Stack_Level[Socket][StackIndex] == 0xFFFF)
                                {
                                  if (IoBaseExceededLimit == 0)
                                    TempIoBaseAddressBeforeAlignemnt = 0x10000; 
                                  else
                                    TempIoBaseAddressBeforeAlignemnt = IoBaseExceededLimit;
                                }
                                else
                                {
                                  TempIoBaseAddressBeforeAlignemnt = IohOutofResInfo.IoBase_Stack_Level[Socket][StackIndex];
                                }
                                IoBaseAddress = TempIoBaseAddressBeforeAlignemnt;
                            }
                            else
                            {
                                  IoBaseAddress = TempIoBaseAddressBeforeAlignemnt;
                            }

                    }
                    else
                    {
                        if (Stack0_IO_Updated_Once == FALSE)
                        {
                            // 
                            //  Example : In LR, CPU 3, STK 0 alone will have IO alloted.
                            //  And if there is no IO Requested in that socket and if there is any failure in STK 3
                            //  Without this condition check, TempIoBaseAddressBeforeAlignemnt will return wrong data.
                            //
                            TempIoBaseAddressBeforeAlignemnt = IohOutofResInfo.IoBase_Stack_Level[Socket][0]; 
                            if (TempIoBaseAddressBeforeAlignemnt == 0xFFFF && (IohOutofResInfo.IoBase_Stack_Level[Socket][StackIndex] == 0xFFFF) )
                            {
                              if (IoBaseExceededLimit == 0)
                                TempIoBaseAddressBeforeAlignemnt = 0x10000; 
                              else
                                TempIoBaseAddressBeforeAlignemnt = IoBaseExceededLimit;
                            }
                            else
                            {
                              if (TempIoBaseAddressBeforeAlignemnt == 0xFFFF) 
                                TempIoBaseAddressBeforeAlignemnt = IohOutofResInfo.IoBase_Stack_Level[Socket][StackIndex];
                              else
                                TempIoBaseAddressBeforeAlignemnt = IohOutofResInfo.IoBase_Stack_Level[Socket][0];
                            }
                            
                            Stack0_IO_Updated_Once = TRUE;
                        }
                        IoBaseAddress = TempIoBaseAddressBeforeAlignemnt;
                    }
                    
                    MaxGra = IohOutofResInfo.Io_Gra_Stack_Level[Socket][StackIndex][FailedIoIndex] + 1;
                    PCI_TRACE((TRACE_PCI, "\n IO : STEP_3 : IoBaseAddress =%lx TempIoBaseAddressBeforeAlignemnt=%lx MaxGra=%lx\n",IoBaseAddress,TempIoBaseAddressBeforeAlignemnt,MaxGra));
                    FistbitSetPoistioninMaxGra = FindFirstbitset (MaxGra);
                    FistbitSetPoistioninTempBaseAddress = FindFirstbitset (TempIoBaseAddressBeforeAlignemnt);
                    
                    PCI_TRACE((TRACE_PCI, "\n IO : STEP_4 : FistbitSetPoistioninMaxGra =%lx FistbitSetPoistioninTempBaseAddress=%lx \n",FistbitSetPoistioninMaxGra,FistbitSetPoistioninTempBaseAddress));
                    
                    if (FistbitSetPoistioninTempBaseAddress >= FistbitSetPoistioninMaxGra)
                    {
                        PreviousRatio = RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex];
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_5 : PreviousRatio =%lx \n",PreviousRatio));
                        RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] =  (UINT16)((IohOutofResInfo.IoRequested_Stack_Level[Socket][StackIndex][FailedIoIndex]) / SAD_LEG_IO_GRANTY_2KB);
                        ExtraSpaceNeededCount = (IohOutofResInfo.IoRequested_Stack_Level[Socket][StackIndex][FailedIoIndex])%SAD_LEG_IO_GRANTY_2KB;
                        if (ExtraSpaceNeededCount > 0)
                        {
                            RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] + 1;
                            PCI_TRACE((TRACE_PCI, "\n IO : STEP_6 RequiredMMIOIoRatio.IO_Stack_Level[%d][%d] =%lx \n",Socket,StackIndex,RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]));
                        }
                        
                        TempIoBaseAddressBeforeAlignemnt = TempIoBaseAddressBeforeAlignemnt + RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] * SAD_LEG_IO_GRANTY_2KB;
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_7 : TempIoBaseAddressBeforeAlignemnt=%lx \n",TempIoBaseAddressBeforeAlignemnt));
                        
                        if (TempIoBaseAddressBeforeAlignemnt > 0x10000)
                        {
                          IoBaseExceededLimit = TempIoBaseAddressBeforeAlignemnt;
                        }
                        
                        RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] = PreviousRatio + (UINT16) ((TempIoBaseAddressBeforeAlignemnt - IoBaseAddress)/SAD_LEG_IO_GRANTY_2KB);
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_8 RequiredMMIOIoRatio.IO_Stack_Level[%d][%d] =%lx \n",Socket,StackIndex,RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]));
                        
                        if (Stack_IoBaseAddressExceededeMaxLimit[Socket][StackIndex])
                        {
                            RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] + (UINT16) ((TempIoBaseAddressBeforeAlignemnt - 0x10000) /SAD_LEG_IO_GRANTY_2KB) ; // MMIOL MAX LIMIT
                            PCI_TRACE((TRACE_PCI, "\n IO : STEP_9 Socket=%d Stack=%d crossed the MAX IOL LIMIT 0x10000 \n",Socket, StackIndex));
                        }
                        else
                        {
                            PCI_TRACE((TRACE_PCI, "\n IO : STEP_9A Socket=%d Stack=%d NumberofStacksPerCpu=%d \n",Socket, StackIndex,NumberofStacksPerCpu));	
                            if ((StackIndex == (NumberofStacksPerCpu -1)) && (TempIoBaseAddressBeforeAlignemnt < IohOutofResInfo.IoLimit_Stack_Level[Socket][StackIndex]) && AfterStackAdjust_SocketIO_AdjustNeeded == FALSE && SocketLevelIoAdjustmentNeeded == FALSE)
                            {
                                PCI_TRACE((TRACE_ALWAYS, "\n IO : STEP_9B Socket=%d Stack=%d NumberofStacksPerCpu=%d \n",Socket, StackIndex,NumberofStacksPerCpu));	
                                RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] =  RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] + (UINT16)((IohOutofResInfo.IoLimit_Stack_Level[Socket][StackIndex] - TempIoBaseAddressBeforeAlignemnt)/SAD_LEG_IO_GRANTY_2KB);
                            }
                        }
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_10 :  RequiredMMIOIoRatio =%lx \n",RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]));
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_11 : IoBaseAddress =%lx TempIoBaseAddressBeforeAlignemnt=%lx \n",IoBaseAddress,TempIoBaseAddressBeforeAlignemnt));
                        if (StackIndex == 0)
                            Stack0_IO_Updated_Once = TRUE;
                    }
                    else
                    {
                        while (FistbitSetPoistioninTempBaseAddress < FistbitSetPoistioninMaxGra)
                        {
                            TempIoBaseAddressBeforeAlignemnt = TempIoBaseAddressBeforeAlignemnt + SAD_LEG_IO_GRANTY_2KB;
                            FistbitSetPoistioninTempBaseAddress = FindFirstbitset (TempIoBaseAddressBeforeAlignemnt);
                            if (TempIoBaseAddressBeforeAlignemnt > 0x10000) // IOL MAX LIMIT
                            {
                                Stack_IoBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE;
                                Socket_Stack_IoBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE; // Global variable.
                                PCI_TRACE((TRACE_PCI, "\n IO : STEP_12 Socket=%d Stack=%d crossed the MAX IOL LIMIT 0x10000 \n",Socket, StackIndex));
                            }
                        }
                        
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_13 : TempIoBaseAddressBeforeAlignemnt=%lx \n",TempIoBaseAddressBeforeAlignemnt));
                        
                        PreviousRatio = RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex];
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_14 PreviousRatio =%lx \n",PreviousRatio));
                        RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] =  (UINT16)((IohOutofResInfo.IoRequested_Stack_Level[Socket][StackIndex][FailedIoIndex]) / SAD_LEG_IO_GRANTY_2KB);
                        ExtraSpaceNeededCount = (IohOutofResInfo.IoRequested_Stack_Level[Socket][StackIndex][FailedIoIndex])%SAD_LEG_IO_GRANTY_2KB;
                        if (ExtraSpaceNeededCount > 0)
                        {
                            RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] + 1;
                            PCI_TRACE((TRACE_PCI, "\n IO : STEP_15 RequiredMMIOIoRatio.IO_Stack_Level[%d][%d] =%lx \n",Socket,StackIndex,RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]));
                        }
                        TempIoBaseAddressBeforeAlignemnt = TempIoBaseAddressBeforeAlignemnt + RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] * SAD_LEG_IO_GRANTY_2KB;
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_16 : TempIoBaseAddressBeforeAlignemnt=%lx \n",TempIoBaseAddressBeforeAlignemnt));
                        RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] = PreviousRatio + (UINT16)((TempIoBaseAddressBeforeAlignemnt - IoBaseAddress)/SAD_LEG_IO_GRANTY_2KB);
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_17 : RequiredMMIOIoRatio =%lx \n",RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]));
                        PCI_TRACE((TRACE_PCI, "\n IO : STEP_18 : IoBaseAddress =%lx TempIoBaseAddressBeforeAlignemnt=%lx \n",IoBaseAddress,TempIoBaseAddressBeforeAlignemnt));
                        if (TempIoBaseAddressBeforeAlignemnt > 0x10000) // IOL MAX LIMIT
                        {
                            Stack_IoBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE;
                            Socket_Stack_IoBaseAddressExceededeMaxLimit[Socket][StackIndex] = TRUE; // Global variable.

                            PCI_TRACE((TRACE_PCI, "\n IO : STEP_19 Socket=%d Stack=%d crossed the MAX IOL LIMIT 0x10000 \n",Socket, StackIndex));
                        }
                        if (FailedIoIndex == 0 && StackIndex == (NumberofStacksPerCpu -1)&& (TempIoBaseAddressBeforeAlignemnt < IohOutofResInfo.IoLimit_Stack_Level[Socket][StackIndex]) && AfterStackAdjust_SocketIO_AdjustNeeded == FALSE && SocketLevelIoAdjustmentNeeded == FALSE)
                        {
                       		PCI_TRACE((TRACE_PCI, "\n IO : STEP_19A Socket=%d Stack=%d NumberofStacksPerCpu=%d \n",Socket, StackIndex,NumberofStacksPerCpu));	
                            RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] =  RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] + (UINT16)((IohOutofResInfo.IoLimit_Stack_Level[Socket][StackIndex] - TempIoBaseAddressBeforeAlignemnt)/SAD_LEG_IO_GRANTY_2KB);
                            PCI_TRACE((TRACE_PCI, "\n IO : STEP_19B RequiredMMIOIoRatio =%lx \n",RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]));
                        }
                        if (StackIndex == 0)
                            Stack0_IO_Updated_Once = TRUE;
                    }
                }
            }
            
            if(SocketLevelInfoNeeded)
            {
                PCI_TRACE((TRACE_PCI, "\n IO : STEP_20 Socket=%d Stack=%d RequiredMMIOIoRatio.IO[Socket]=%lx \n",Socket, StackIndex,RequiredMMIOIoRatio.IO[Socket]));
                RequiredMMIOIoRatio.IO[Socket] = RequiredMMIOIoRatio.IO[Socket] + RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex];
                PCI_TRACE((TRACE_PCI, "\n IO : STEP_21. Socket=%d Stack=%d RequiredMMIOIoRatio.IO[Socket]=%lx \n",Socket, StackIndex,RequiredMMIOIoRatio.IO[Socket]));
            }
        } // if (OutOfResrcFailureType == IOFAILURE)
    } // End of Stack
    
       // PCI_TRACE((TRACE_PCI, "\n 15A. MMIO : Socket=%d RequiredMMIOIoRatio.MMIO[Socket] =%lx \n",Socket, RequiredMMIOIoRatio.MMIO[Socket]));
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CheckSocketAdjustmentNeeded
//
// Description: This function checks whether Socket Level Adjustment is needed or not
//
// Input:       OutOfResrcFailure  - MMIO/IO Failure
//              Socket             - Failed Socket
//              NotPossibleFlag    - NotPossibleFlag
//
// Output:      TRUE/FALSE
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN CheckSocketAdjustmentNeeded (
    UINT8   OutOfResrcFailureType,
    UINTN   Socket,
    BOOLEAN *NotPossibleFlag
)
{
    UINT16  TotalStackRatio = 0;
    UINT8   StackIndex;
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
            continue;
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
        
        //
        // MMIO_FAILURE
        //
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            TotalStackRatio = TotalStackRatio + RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
            if (TotalStackRatio > IohOutofResInfo.Socket_Current_Boot_MMIORatio [Socket])
            {
                IohOutofResInfo.MmmioOutofResrcFlag [Socket] = 1;
                *NotPossibleFlag = FALSE;
                return TRUE;
            }
        }
        //
        // IO_FAILURE
        //
        if (OutOfResrcFailureType == IOFAILURE)
        {
            TotalStackRatio = TotalStackRatio + RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex];
            // All Stacks are already empty.Try socket level
            if (TotalStackRatio == 0 && StackIndex == NumberofStacksPerCpu-1)
            {
                IohOutofResInfo.IoOutofResrcFlag [Socket] = 1;
                *NotPossibleFlag = FALSE;
                 return TRUE;
            }
            if (TotalStackRatio > IohOutofResInfo.Socket_Current_Boot_IORatio [Socket])
            {
                // SocketLevelIOAdjustmentNeeded = TRUE;
                IohOutofResInfo.IoOutofResrcFlag [Socket] = 1;
                *NotPossibleFlag = FALSE;
                return TRUE;
            }
        }
    } // End of Stack
    *NotPossibleFlag = FALSE;
    return FALSE;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   InitAndFillValidStacksIndex
//
// Description: This function finds Valid stacks that are available in the socket
//
// Input:       OutOfResrcFailure  - MMIO/IO Failure
//              Socket             - Failed Socket
//
// Output:      ValidStacks        - Array has valid stack Info
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID InitAndFillValidStacksIndex (
    UINT8   OutOfResrcFailureType,
    UINTN   Socket
)
{
    UINT8  StackIndex;
    UINT8  Index1;
        
    for (StackIndex = 0, Index1 = 0; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
            continue;
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
            
        // 
        //  Init valid stack array with 0xFF. Later fill the valid stacks based on OutOfResourceFlag.
        //
        ValidStacks[StackIndex] = 0xFF;
        
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            if (IohOutofResInfo.MmmioOutofResrcFlag_Stack_Level [Socket][StackIndex] != 1)
            {
                ValidStacks[Index1] = StackIndex;
                Index1++;
            }
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            if (IohOutofResInfo.IoOutofResrcFlag_Stack_Level [Socket][StackIndex] != 1)
            {
                ValidStacks[Index1] = StackIndex;
                Index1++;
            }
        }
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CheckValidStacksAvail
//
// Description: This function checks for any valid stack available
//
// Input:       Socket
//
// Output:      NotPossibleFlag  - TRUE/FALSE
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CheckValidStacksAvail (
    BOOLEAN *NotPossibleFlag,
    UINTN   Socket
)
{
    UINT8   StackIndex;
    
    for (StackIndex = 0; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
            continue;
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
        
        if (ValidStacks[StackIndex] != 0xFF)
            break;
    }
    
    //
    // No Stack is valid : THis is a not possible case
    //  Need to try OEM Defined Table
    //
    if (StackIndex == MAX_IIO_STACK)
        *NotPossibleFlag = TRUE;
    else
        *NotPossibleFlag = FALSE;
    
    return;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   PrintValidStackInfo
//
// Description: This function prints the valid stack info
//
// Input:       Socket
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PrintValidStackInfo (
    UINTN Socket
)
{
    UINT8   StackIndex;
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
            continue;
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;

        if (ValidStacks[StackIndex] != 0xFF)
            PCI_TRACE((TRACE_PCI, "\n VALID Stack=%d\n",ValidStacks[StackIndex]));
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   ResourceAdjustment
//
// Description: This will Check the valid Stack Resourc for MMIO and IO
//
// Input:       UINT8    OutOfResrcFailureType
//              UINTN    Socket
//              BOOLEAN  *NotPossibleFlag
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ResourceAdjustment(
    UINT8    OutOfResrcFailureType,
    UINTN    Socket,
    BOOLEAN  *NotPossibleFlag
)
{
    UINT8   StackIndex, Index1, Idx;
    UINT8   ExtraChunks;
    UINT8   TempExtraChunks = 0;
    
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid) 
            continue;
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex)))
            continue;
        
        //
        // MMIO_FAILURE
        //
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            if (IohOutofResInfo.MmmioOutofResrcFlag_Stack_Level [Socket][StackIndex] != 1) // This stack is not failed
                continue;
            
            //
            // Going to use the ValidStacks TABLE to help the affected stack's
            //
            Index1 = 0 ;
            TempExtraChunks =  0;
            while(ValidStacks [Index1] != 0xFF && Index1 < NumberofStacksPerCpu) // ADD A CONIDTION VALID[ARRAY] NOT SHOULD NOT CROSS ARRAY BOUND
            {
                Idx = ValidStacks [Index1];
                if (CurrentMmioIoRatios.MMIO_Stack_Level[Socket] [Idx] < RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][Idx] )
                {
                    Index1++;
                    continue; 
                }
                ExtraChunks = (CurrentMmioIoRatios.MMIO_Stack_Level[Socket] [Idx] - RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][Idx]);
                if (RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] < CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex])
                {
                	if (StackIndex != NumberofStacksPerCpu -1 )
                	{
                		//CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                		goto next_stack;
                	}
                    PCI_TRACE((TRACE_PCI, "\n MMIO: Currently Alloted Space is more than Required Space\n"));
                    PCI_TRACE((TRACE_PCI, "\n But Still couldn't get continous space\n"));
                    PCI_TRACE((TRACE_PCI, "\n GOING to try MAX_SYSTEM_RESET_RETRYCOUNT_ON_RESOURCE_ERROR to fix the PCI RESOURCE GRANULARTIY issue\n"));
                    *NotPossibleFlag = TRUE;
                    return;
                }
                
                if ((RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] > CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    if ( (TempExtraChunks + CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]) < RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex])
                    {
                        CurrentMmioIoRatios.MMIO_Stack_Level[Socket][Idx] = CurrentMmioIoRatios.MMIO_Stack_Level[Socket][Idx] - ExtraChunks;
                        CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]  = CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] + ExtraChunks;
                        Index1++;
                        continue;
                    }
                    
                    CurrentMmioIoRatios.MMIO_Stack_Level[Socket][Idx] = CurrentMmioIoRatios.MMIO_Stack_Level[Socket][Idx] - (RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]  - CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]);
                    CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]  + (RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex]  - CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]);
                    
                    if (CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] == RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex])
                    {
                	if (StackIndex != NumberofStacksPerCpu -1 )
                	{
                		//CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                	        goto next_stack;
                	}
                        return;
                    }
                }
                
                //
                // if both Requested and Current Ratio's are same and if we have any valid stacks, apply the residual resource to the affectd stack.
                //
                if ((RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex] == CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    CurrentMmioIoRatios.MMIO_Stack_Level[Socket][Idx] = CurrentMmioIoRatios.MMIO_Stack_Level[Socket][Idx] - ExtraChunks;
                    CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]  + ExtraChunks;
                    
                    if (StackIndex != NumberofStacksPerCpu -1 )
                    {
                      		//CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                                goto next_stack;
                    }
                    return;
                }
                
                // PCI_TRACE((TRACE_PCI, "\n MMIO : Socket =%ld Stack=%d BestRatio=0x%x\n",Socket,StackIndex ,CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex]));
                Index1++;  // Try the next valid Stack
            }
        } // if (OutOfResrcFailureType == MMIOFAILURE)
        //
        // IO_FAILURE
        //
        if (OutOfResrcFailureType == IOFAILURE)
        {
            if (IohOutofResInfo.IoOutofResrcFlag_Stack_Level [Socket][StackIndex] != 1) // This stack is not failed
                continue;
            
            //
            // Going to use the ValidStacks TABLE to help the affected IOH
            //
            Index1 = 0 ;
            TempExtraChunks =  0;
            while( ValidStacks [Index1] != 0xFF)
            {
                Idx = ValidStacks [Index1];
                if (CurrentMmioIoRatios.IO_Stack_Level[Socket] [Idx] < RequiredMMIOIoRatio.IO_Stack_Level[Socket][Idx] )
                {
                  Index1++;
                  continue; 
                }
                ExtraChunks = (CurrentMmioIoRatios.IO_Stack_Level[Socket] [Idx] - RequiredMMIOIoRatio.IO_Stack_Level[Socket][Idx]);
                if (RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] < CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex])
                {
                	if (StackIndex != NumberofStacksPerCpu -1 ) 
                	{
                		//CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex];
                		goto next_stack; 
                	}
                    PCI_TRACE((TRACE_PCI, "\n IO: Currently Alloted Space is more than Required Space\n"));
                    PCI_TRACE((TRACE_PCI, "\n But Still couldn't get continous space\n"));
                    PCI_TRACE((TRACE_PCI, "\n GOING to try MAX_SYSTEM_RESET_RETRYCOUNT_ON_RESOURCE_ERROR to fix the PCI RESOURCE GRANULARTIY issue\n"));
                    *NotPossibleFlag = TRUE;
                    return;
                }

                if ((RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] > CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    if ( (TempExtraChunks + CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]) < RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex])
                    {
                        CurrentMmioIoRatios.IO_Stack_Level[Socket][Idx] = CurrentMmioIoRatios.IO_Stack_Level[Socket][Idx] - ExtraChunks;
                        CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]  = CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex] + ExtraChunks;
                        Index1++;
                        continue;
                    }
                    
                    CurrentMmioIoRatios.IO_Stack_Level[Socket][Idx] = CurrentMmioIoRatios.IO_Stack_Level[Socket][Idx] - (RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]  - CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]);
                    CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex] = CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]  + (RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex]  - CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]);
                    
                    if (CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]  == RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex])
                    {
                	if (StackIndex != NumberofStacksPerCpu -1 )
                	{
                		//CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                	        goto next_stack;
                	}
                        return;
                    }
                }
                
                //
                // if both Requested and Current Ratio's are same and if we have any valid stacks, apply the residual resource to the affectd stack.
                //
                if ((RequiredMMIOIoRatio.IO_Stack_Level[Socket][StackIndex] == CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]) && ExtraChunks >= 1)
                {
                    TempExtraChunks = ExtraChunks;
                    CurrentMmioIoRatios.IO_Stack_Level[Socket][Idx] = CurrentMmioIoRatios.IO_Stack_Level[Socket][Idx] - ExtraChunks;
                    CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex] = CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]  + ExtraChunks;
                    
                    if (StackIndex != NumberofStacksPerCpu -1 )
                    {
                	    //CurrentMmioIoRatios.MMIO_Stack_Level[Socket][StackIndex] = RequiredMMIOIoRatio.MMIO_Stack_Level[Socket][StackIndex];
                            goto next_stack;
                    }
                    return;
                }
                
                // PCI_TRACE((TRACE_PCI, "\n IO : Socket =%d Stack=%d BestRatio=0x%lx\n",Socket,StackIndex ,CurrentMmioIoRatios.IO_Stack_Level[Socket][StackIndex]));
                Index1++; // Try the next valid Stack
            }
        } // if (OutOfResrcFailureType == IOFAILURE)
next_stack : ; // do nothing
    } // End of Stack
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   PrintBestRatioAfterAdjustment
//
// Description: This function prints best stack levelRatio's
//
// Input:       UINT8    OutOfResrcFailureType
//              UINTN    Socket
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PrintBestRatioAfterAdjustment (
    UINT8  OutOfResrcFailureType,
    UINTN  Socket
)
{
    UINT8    StackIndex;
    
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid)
            continue;
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
        
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            PCI_TRACE((TRACE_PCI, "\n MMIO:Stack=%d Newly Calculated BestRatio=0x%x\n",StackIndex,CurrentMmioIoRatios.MMIO_Stack_Level [Socket]  [StackIndex]));
            PCI_TRACE((TRACE_PCI, "\n MMIO:Stack=%d CurrentBoot Ratio=0x%x\n",StackIndex,AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [Socket][StackIndex]));
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            PCI_TRACE((TRACE_PCI, "\n IO:Stack=%d Newly Calculated BestRatio=0x%lx\n",StackIndex,CurrentMmioIoRatios.IO_Stack_Level [Socket]  [StackIndex]));
            PCI_TRACE((TRACE_PCI, "\n IO:Stack=%d CurrentBoot Ratio=0x%lx\n",StackIndex,AutoAdjustMmioIoVariable.IoRatio_Stack_Level [Socket][StackIndex]));
        }
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CompareNewRatioWithCurrentBootRatio
//
// Description: This function compares current boot Ratio and new Ratio
//               if no change, this is a not possible case
//
// Input:       OutOfResrcFailureType
//              Socket
//
// Output:      NotPossibleFlag - TRUE/FALSE
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CompareNewRatioWithCurrentBootRatio (
    UINT8   OutOfResrcFailureType,
    UINTN   Socket,
    BOOLEAN *NotPossibleFlag
)
{
    UINT8    StackIndex;
    
    PCI_TRACE((TRACE_PCI, "\n Comparing New Adjusted Ratio's with the Current Boot Ratio\n"));
    *NotPossibleFlag = TRUE;
    for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].Valid)
            continue;
        if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
        
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            if (AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [Socket][StackIndex] != CurrentMmioIoRatios.MMIO_Stack_Level [Socket] [StackIndex])
            {
                *NotPossibleFlag = FALSE;
                AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [Socket] [StackIndex] = CurrentMmioIoRatios.MMIO_Stack_Level [Socket] [StackIndex];
                PCI_TRACE((TRACE_PCI, "\n MMIO : Ratio's changed for Socket=%d Stack=%d\n",Socket,StackIndex));
            }
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            if (AutoAdjustMmioIoVariable.IoRatio_Stack_Level [Socket][StackIndex] != CurrentMmioIoRatios.IO_Stack_Level [Socket] [StackIndex])
            {
                *NotPossibleFlag = FALSE;
                AutoAdjustMmioIoVariable.IoRatio_Stack_Level [Socket] [StackIndex] = CurrentMmioIoRatios.IO_Stack_Level [Socket] [StackIndex];
                PCI_TRACE((TRACE_PCI, "\n IO : Ratio's changed for Socket=%d Stack=%d\n",Socket,StackIndex));
            }
        }
    }
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   CollectCurrentBootSocketRatio
//
// Description: This function collects the socket current boot Ratio
//
// Input:       OutOfResrcFailureType - MMIO/IO
//
// Output:      CurrentMmioIoRatios   - CurrentMmioIoRatios Array
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CollectCurrentBootSocketRatio (
    UINT8  OutOfResrcFailureType
)
{
    UINTN SocketIndex;
    
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET ; SocketIndex++) 
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
            continue;
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            CollectCurrentBootRatio(MMIOFAILURE,SocketIndex);
            CurrentMmioIoRatios.MMIO [SocketIndex]   = AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex];
            PCI_TRACE((TRACE_PCI, "\n MMIO: Socket=%d CurrentBootMMIO chunk =0x%lx\n",SocketIndex,CurrentMmioIoRatios.MMIO [SocketIndex]));
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            CollectCurrentBootRatio(IOFAILURE,SocketIndex);
            CurrentMmioIoRatios.IO [SocketIndex]   = AutoAdjustMmioIoVariable.IoRatio [SocketIndex];
            PCI_TRACE((TRACE_PCI, "\n IO: Socket=%d CurrentBootMMIO chunk =0x%lx\n",SocketIndex,CurrentMmioIoRatios.IO [SocketIndex]));
        }
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     CalculateRequiredSocketRatio
//
// Description:   This function collects the socket required boot Ratio
//
// Input:         OutOfResrcFailureType - MMIO/IO
//
// Output:        RequiredMMIOIoRatio   - RequiredMMIOIoRatio Array.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CalculateRequiredSocketRatio (
    UINT8 OutOfResrcFailureType)
{
    UINTN SocketIndex;
        
    for (SocketIndex = 0; SocketIndex < MAX_SOCKET ; SocketIndex++) 
    {
        if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
            continue;
        if (OutOfResrcFailureType == MMIOFAILURE)
        {
            RequiredMMIOIoRatio.MMIO[SocketIndex] = 0;
            CalculateRequiredRatio(MMIOFAILURE,SocketIndex,TRUE);
            PCI_TRACE((TRACE_PCI, "\n MMIO : Socket=%d Required MMIO=0x%x \n",SocketIndex,RequiredMMIOIoRatio.MMIO[SocketIndex]));
        }
        if (OutOfResrcFailureType == IOFAILURE)
        {
            RequiredMMIOIoRatio.IO[SocketIndex] = 0;
            CalculateRequiredRatio(IOFAILURE,SocketIndex,TRUE);
            PCI_TRACE((TRACE_PCI, "\n IO : Socket=%d Required IO=0x%x \n",SocketIndex,RequiredMMIOIoRatio.IO[SocketIndex]));
        }
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     SocketAutoAdjustIoRatio
//
// Description:   This function performs Auto Adjusted IO Ratio
//
// Input:         NumIoH                   - Number of IOH
//                AutoAdjustMmioIoVariable - Structure has Current MMIO and IO Ratio Information
//
// Output:        AutoAdjustMmioIoVariable - Newly Calculated Ratio's
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN SocketAutoAdjustIoRatio (
    UINT8                              NumIoH,
    AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA AutoAdjustMmioIoVariable
)
{
    BOOLEAN NotPossibleFlag = FALSE;
    
    //
    // Collecting the Current Boot MMIO Socket Ratio
    //
    CollectCurrentBootSocketRatio(IOFAILURE);
    
    //
    // Calculating the Required Ratio for Each IOh's
    //
    CalculateRequiredSocketRatio(IOFAILURE);
    
    

    //
    // Check which IOH has the problem and whether other IOH has some free space.
    //  Creating Dummy ValidIOH Strcuture;
    //  If FreeSpace is available in Socket,Fill Socket Index in the ValidIOH Table
    //
    InitAndFillValidSocketIndex(IOFAILURE);
    
    //
    // Sanity Check : ValidSockets present
    //  No Stack is valid : THis is a not possible case
    //  Need to try OEM Defined Table
    //
    CheckValidSocketsAvail(&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        DEBUG ((EFI_D_ERROR, "\n IO : NO VALID Socket's are present\n"));
        DEBUG ((EFI_D_ERROR, "\n System will try the OEM DEFINED TABLE\n"));
        return NotPossibleFlag;
    }
    
    //
    // Printing VALID Stacks
    //
    PrintValidSocketInfo();
    
    //
    // Going to take the resource from the validIOH
    //
    SocketResourceAdjustment(IOFAILURE,&NotPossibleFlag);
  
    
    //
    // Printing the New calculated Ratio
    //
    PrintBestRatioAfterSocketAdjustment(IOFAILURE);
    CompareSocketNewRatioWithCurrentBootRatio(IOFAILURE,&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        DEBUG((EFI_D_ERROR, "\n Socket IO : Ratio's are same before and after Adjustment. Not Possible case!!!!!!\n"));
    }

    return NotPossibleFlag;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     SocketAutoAdjustMMIoRatio
//
// Description:   This function performs Auto Adjusted MMIO Ratio
//
// Input:         NumIoH                   - Number of IOH
//                AutoAdjustMmioIoVariable - Structure has Current MMIO and IO Ratio Information
//
// Output:        AutoAdjustMmioIoVariable - Newly Calculated Ratio's
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN SocketAutoAdjustMMIoRatio(
    UINT8                              NumIoH,
    AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA AutoAdjustMmioIoVariable
)
{
    BOOLEAN  NotPossibleFlag = FALSE;
    
    //
    // Collecting the Current Boot MMIO Socket Ratio
    //
    CollectCurrentBootSocketRatio(MMIOFAILURE);
    
    //
    // Calculating the Required Ratio for Each IOh's
    //
    CalculateRequiredSocketRatio(MMIOFAILURE);
    
    
    
    //
    // Check which IOH has the problem and whether other IOH has some free space.
    //  Creating Dummy ValidIOH Strcuture;
    //  If FreeSpace is available in Socket,Fill Socket Index in the ValidIOH Table
    //
    InitAndFillValidSocketIndex(MMIOFAILURE);
    
    //
    // Sanity Check : ValidSockets present
    //  No Stack is valid : THis is a not possible case
    //  Need to try OEM Defined Table
    //
    CheckValidSocketsAvail(&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        DEBUG((EFI_D_ERROR, "\n MMIO : NO VALID Socket's are present\n"));
        DEBUG((EFI_D_ERROR, "\n System will try the OEM DEFINED TABLE\n"));
        return NotPossibleFlag;
    }
    
    //
    // Printing VALID Stacks
    //
    PrintValidSocketInfo();
    
    //
    // Going to take the resource from the validIOH
    //
    SocketResourceAdjustment(MMIOFAILURE,&NotPossibleFlag);
  
    
    //
    // Printing the New calculated Ratio
    //
    PrintBestRatioAfterSocketAdjustment(MMIOFAILURE);
    CompareSocketNewRatioWithCurrentBootRatio(MMIOFAILURE,&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        DEBUG((EFI_D_ERROR, "\n Socket MMIO : Ratio's are same before and after Adjustment. Not Possible case!!!!!!\n"));
    }
    
    return NotPossibleFlag;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     StackAutoAdjustIoRatio
//
// Description:   This function performs Auto Adjusted IO Ratio
//
// Input:         UINTN IO_Failed_Socket
//
// Output:        AutoAdjustMmioIoVariable - Newly Calculated Ratio's
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN StackAutoAdjustIoRatio (
    UINTN IO_Failed_Socket
)
{
    BOOLEAN  NotPossibleFlag = FALSE;
    BOOLEAN  SocketAdjustNeeded = FALSE;
    
    PCI_TRACE((TRACE_PCI, "\n IO_Failed_Socket=%d\n",IO_Failed_Socket ));
    
    //
    // Collecting the Current Boot IO Stack Ratio
    //
    CollectCurrentBootRatio(IOFAILURE,IO_Failed_Socket/*,FALSE*/);                                      
    
    //
    // Calculating the Required Ratio for Each Stack
    //
    
    CalculateRequiredRatio(IOFAILURE,IO_Failed_Socket,FALSE);
	

    
    //
    // Additonal check whether Sum of all stack's Required Ratio crossing the CPU Ratio.
    //  Check SocketLevel Adjustment Needed or not
    // 
    SocketAdjustNeeded = CheckSocketAdjustmentNeeded(IOFAILURE,IO_Failed_Socket,&NotPossibleFlag);
    if(SocketAdjustNeeded)
    {
        AfterStackAdjust_SocketIO_AdjustNeeded = TRUE;
        return NotPossibleFlag;
    }
    
    //
    // Check which IOH has the problem and whether other Stack has some free space.
    //  Creating Dummy ValidStack Strcuture;
    //  If FreeSpace is available in Stack,Fill Stack Index in the ValidStacks Table
    //
    InitAndFillValidStacksIndex(IOFAILURE,IO_Failed_Socket);
    
    //
    // Sanity Check : ValidStacks present
    //  No Stack is valid : THis is a not possible case
    //  Need to try OEM Defined Table
    // 
    CheckValidStacksAvail(&NotPossibleFlag,IO_Failed_Socket);
    if(NotPossibleFlag)
    {
        DEBUG((EFI_D_ERROR, "\n IO : NO VALID Stack's are present\n"));
        DEBUG((EFI_D_ERROR, "\n System will try the OEM DEFINED TABLE\n"));
        return NotPossibleFlag;
    }
    
    //
    // Printing VALID Stacks
    //
    PrintValidStackInfo(IO_Failed_Socket);

    //
    // Going to take the resource from the ValidStacks and applying to failed one's.
    //
    ResourceAdjustment(IOFAILURE,IO_Failed_Socket,&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        return NotPossibleFlag;
    }
    
    //
    // Printing the New calculated Ratio
    //
    PrintBestRatioAfterAdjustment(IOFAILURE,IO_Failed_Socket);
    
    //
    // Comparing Newly got Ratio's with current boot Ratio,if same then there is no possible way
    //
    CompareNewRatioWithCurrentBootRatio(IOFAILURE,IO_Failed_Socket,&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        DEBUG((EFI_D_ERROR, "\n IO : Ratio's are same before and after Adjustment. Not Possible case!!!!!!\n"));
    }
    return NotPossibleFlag;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     StackAutoAdjustMMIoRatio
//
// Description:   This function performs Auto Adjusted MMIO Ratio
//
// Input:         UINTN MMIO_Failed_Socket
//
// Output:        AutoAdjustMmioIoVariable - Newly Calculated Ratio's
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN StackAutoAdjustMMIoRatio(
    UINTN MMIO_Failed_Socket
)
{
    BOOLEAN  NotPossibleFlag = FALSE;
    BOOLEAN  SocketAdjustNeeded = FALSE;

    PCI_TRACE((TRACE_PCI, "\n MMIO_Failed_Socket=%ld\n",MMIO_Failed_Socket ));
    
    //
    // Collecting the Current Boot MMIO Stack Ratio
    //
    CollectCurrentBootRatio(MMIOFAILURE,MMIO_Failed_Socket);
    
    //
    // Calculating the Required Ratio for Each IOh's
    //
    CalculateRequiredRatio(MMIOFAILURE,MMIO_Failed_Socket,FALSE);
    
   
    
    //
    // Check SocketLevel Adjustment Needed or not
    //
    SocketAdjustNeeded = CheckSocketAdjustmentNeeded(MMIOFAILURE,MMIO_Failed_Socket,&NotPossibleFlag);
    if(SocketAdjustNeeded)
    {
        AfterStackAdjust_SocketMMIO_AdjustNeeded = TRUE;
        return NotPossibleFlag;
    }
    
    //
    // Check which IOH has the problem and whether other IOH has some free space.
    //  Creating Dummy ValidIOH Strcuture;
    //  If FreeSpace is available in IOH,Fill IOH Index in the ValidStacks Table
    // 
    InitAndFillValidStacksIndex(MMIOFAILURE,MMIO_Failed_Socket);
    
    //
    // Sanity Check : ValidStacks present
    //  No Stack is valid : THis is a not possible case
    //  Need to try OEM Defined Table
    //
    
    CheckValidStacksAvail(&NotPossibleFlag,MMIO_Failed_Socket);
    if(NotPossibleFlag)
    {
        DEBUG((EFI_D_ERROR, "\n MMIO : NO VALID Stack's are present\n"));
        DEBUG((EFI_D_ERROR, "\n System will try the OEM DEFINED TABLE\n"));
        return NotPossibleFlag;
    }
    
    //
    // Printing VALID Stacks
    //
    PrintValidStackInfo(MMIO_Failed_Socket);

    //
    // Going to take the resource from the ValidStacks and applying to failed one's.
    //
    ResourceAdjustment(MMIOFAILURE,MMIO_Failed_Socket,&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        return NotPossibleFlag;
    }
    
    //
    // Printing the New calculated Ratio
    //
    PrintBestRatioAfterAdjustment(MMIOFAILURE,MMIO_Failed_Socket);
    
    //
    // Comparing Newly got Ratio's with current boot Ratio,if same then there is no possible way
    //
    CompareNewRatioWithCurrentBootRatio(MMIOFAILURE,MMIO_Failed_Socket,&NotPossibleFlag);
    if(NotPossibleFlag)
    {
        DEBUG((EFI_D_ERROR, "\n MMIO : Ratio's are same before and after Adjustment. Not Possible case!!!!!!\n"));
    }
    return NotPossibleFlag;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     SocketAutoAdjustRatios
//
// Description:   This function performs AutoAdjusted MMIO or IO Ratio
//                 Will call SocketAutoAdjustMMIoRatio or SocketAutoAdjustIoRatio
//                 to calculate the best MMIO or IO Ratio to boot
//
// Input:         OutOfResrcFailure - MMIO/IO Failure
//
// Output:        None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN SocketAutoAdjustRatios (
    UINT8 OutOfResrcFailure
)
{
    BOOLEAN     Not_Possible_Flag = FALSE;
    UINT8       SocketIndex;
    UINT8       StackIndex;

    UINTN       VarSize = sizeof (AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA);
    
    //
    // Get the Number of Sockets
    //
    NumIoH = NbGetMaximumIohSockets();

    //
    // ##### Filling the AutoAdjustMmioIoVariable using IohOutofResInfo Structure
    // ##### Filling the AutoAdjustMmioIoVariable using IohOutofResInfo Structure
    // ##### Filling the AutoAdjustMmioIoVariable using IohOutofResInfo Structure
    // ##### This function will be called for both MMIO and IO
    // Filling once with Current Boot Ratio is enough
    // Othewise newRatio calculated (MMIO or IO) whichever gets called first will be overwritten
    //
    if (SocketAutoAdjustCalledOnce == FALSE && StackAutoAdjustCalledOnce == FALSE)
    {
        for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) 
        {
            if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
                continue;
            
            for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) 
            {
                if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
                {
                    continue;
                }
                AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [SocketIndex][StackIndex] = IohOutofResInfo.Stack_Current_Boot_MMIORatio[SocketIndex] [StackIndex];
                AutoAdjustMmioIoVariable.IoRatio_Stack_Level [SocketIndex][StackIndex]    = IohOutofResInfo.Stack_Current_Boot_IORatio[SocketIndex][StackIndex];
            } // for loop for Stack
            
            AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex] = IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex];
            AutoAdjustMmioIoVariable.IoRatio [SocketIndex]    = IohOutofResInfo.Socket_Current_Boot_IORatio[SocketIndex];
            
            SocketAutoAdjustCalledOnce = TRUE;
        } // for loop for Socket
    }
    
    //
    // Check IO Failure
    //
    if (OutOfResrcFailure == IOFAILURE)
    {
        Not_Possible_Flag = SocketAutoAdjustIoRatio(NumIoH,AutoAdjustMmioIoVariable);
    }
    //
    // Check MMIO Failure
    //
    if (OutOfResrcFailure == MMIOFAILURE)
    {
        Not_Possible_Flag = SocketAutoAdjustMMIoRatio(NumIoH,AutoAdjustMmioIoVariable);
    }
    
    return Not_Possible_Flag;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     StackAutoAdjustRatios
//
// Description:   This function performs AutoAdjusted MMIO or IO Ratio
//                 Will call StackAutoAdjustMMIoRatio or StackAutoAdjustIoRatio
//                 to calculate the best MMIO or IO Ratio to boot
//
// Input:         OutOfResrcFailure  - MMIO/IO Failure
//                MMIO_Failed_Socket - MMIO_Failed_Socket
//                IO_Failed_Socket   - IO_Failed_Socket
//
// Output:        TRUE/FALSE
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN StackAutoAdjustRatios (
    UINT8  OutOfResrcFailure,
    UINTN  MMIO_Failed_Socket,
    UINTN  IO_Failed_Socket
    )
{
    BOOLEAN    Not_Possible_Flag = FALSE;
    UINT8      StackIndex,SocketIndex;
    UINTN      VarSize = sizeof (AUTO_ADJUST_MMMIO_IO_VARIABLE_DATA);

    //
    // STEP 3 : START : STACK Level Adjustment 
    //
    
    //
    // Filling the AutoAdjustMmioIoVariable using IohOutofResInfo Structure.    
    // This function will be called for both MMIO and IO
    // Filling once with Current Boot Ratio is enough.
    // Otherwise newRatio calculated (MMIO or IO) whichever gets called first will be overwritten.    
    
    //
    // STEP 3A :  START : STACK Level Adjustment 
    //
    if (StackAutoAdjustCalledOnce == FALSE && SocketAutoAdjustCalledOnce == FALSE)
    {
        for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) 
        {
            if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
            {
                continue;
            }
            
            for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) 
            {
                if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
                {
                    continue;
                }
                
                AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [SocketIndex][StackIndex] = IohOutofResInfo.Stack_Current_Boot_MMIORatio[SocketIndex] [StackIndex];
                AutoAdjustMmioIoVariable.IoRatio_Stack_Level [SocketIndex][StackIndex]    = IohOutofResInfo.Stack_Current_Boot_IORatio[SocketIndex][StackIndex];
            } // for loop for Stack
            
            AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex] = IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex];
            AutoAdjustMmioIoVariable.IoRatio [SocketIndex]    = IohOutofResInfo.Socket_Current_Boot_IORatio[SocketIndex];
            
            StackAutoAdjustCalledOnce = TRUE;
      } // for loop for Socket
    }
    
    //
    // STEP 3B :  CHECK IO/MMIO FAILURE 
    //
    
    //
    // Check IO Failure
    //
    if (OutOfResrcFailure == IOFAILURE)
    {
        Not_Possible_Flag = StackAutoAdjustIoRatio(IO_Failed_Socket);
    }
    //
    // Check MMIO Failure
    //
    if (OutOfResrcFailure == MMIOFAILURE)
    {
        Not_Possible_Flag = StackAutoAdjustMMIoRatio(MMIO_Failed_Socket);
    }
    
    return Not_Possible_Flag;
}
#if OOR_NOT_POSSIBLE_HANDLING_POLICY


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   HaltMachine
//
// Description: This function halts the Machine with a BEEP and Checkpoint 
//              Display(0xD4).
//
// Input:       VOID
//
// Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID HaltMachine (
        VOID
        )
{
    DEBUG((EFI_D_ERROR,"\n+------------------------------------------------------+\n"));
    DEBUG((EFI_D_ERROR,"|         MmCfgBase cannot be lowered below 1GB        |\n"));
    DEBUG((EFI_D_ERROR,"+----------------- SYSTEM is HALTED -------------------+\n"));    
    ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
    EFI_DEADLOOP();

}
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   FillNewBaseAndLimits
//
// Description: This function Calculates the new MMIOL Base and Limits of 
//              all the Stacks of all the CPUs as per the new MMCFG Base and 
//              updates them in IohOutofResInfo Structure 
//
// Input:        UINT32  MmiolBaseInTest
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FillNewBaseAndLimits(
        UINT32  MmiolBaseInTest)
{

  UINT8           StackIndex;
  UINT8           SocketIndex;
  
  UINT16         TmpMmiolRatio, ExtraMmiol;
  UINT16         AvailMmiolChunk;
  UINT32         TmpMmiolSize;
  UINT8          TotalNumOfStacks=0;
  UINT64         MmiolBase;
  
  PCI_TRACE((TRACE_PCI,__FUNCTION__"\n: Updating the Structures IohOutofResInfo, CurrentMmioIoRatios and AutoAdjustMmioIoVariable as per new MmCfgBase:%x\n",MmiolBaseInTest)); 
  //
  // Get the Number of Sockets
  //
  NumIoH = NbGetMaximumIohSockets();
  TotalNumOfStacks = NumberofStacksPerCpu * NumIoH;

  MmiolBase = MmiolBaseInTest;
  // Convert the available MMIOL space into 4M chunck size
  AvailMmiolChunk  = (UINT16)((UINT32)((MMIOL_LIMIT - MmiolBase) + 1) >> 22); 

  TmpMmiolRatio = (AvailMmiolChunk / TotalNumOfStacks);
  ExtraMmiol = (AvailMmiolChunk % TotalNumOfStacks);

  PCI_TRACE((TRACE_PCI,"\nTotalNumOfStacks:%d, TmpMmiolRatio:%x, ExtraMmiol:%x",TotalNumOfStacks,TmpMmiolRatio,ExtraMmiol));

// Adding Extra MMIOL to C-Stack (Stack 0) of SBSP Socket (For Now Socket 0)(i.e., Socket:0 Stack:0).
  IohOutofResInfo.MmioBase_Stack_Level[0][0] =  MmiolBase;
  IohOutofResInfo.MmioLimit_Stack_Level[0][0] = MmiolBase + (((TmpMmiolRatio + ExtraMmiol) * SAD_MMIOL_GRANTY) - 1);
  IohOutofResInfo.Stack_MMIOLength[0][0] = IohOutofResInfo.MmioLimit_Stack_Level[0][0] - IohOutofResInfo.MmioBase_Stack_Level[0][0]; 
  
  IohOutofResInfo.Stack_Current_Boot_MMIORatio[0] [0]= TmpMmiolRatio + ExtraMmiol;
  IohOutofResInfo.Socket_Current_Boot_MMIORatio[0]= TmpMmiolRatio + ExtraMmiol;
  CurrentMmioIoRatios.MMIO_Stack_Level[0][0] = TmpMmiolRatio + ExtraMmiol;
  AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [0][0]= TmpMmiolRatio + ExtraMmiol;
  
  MmiolBase =   IohOutofResInfo.MmioLimit_Stack_Level[0][0] + 1;
  TmpMmiolSize = ((TmpMmiolRatio * SAD_MMIOL_GRANTY) - 1);
  
  PCI_TRACE((TRACE_PCI, "\n1MmioBase_Stack_Level=%lx MmioLimit_Stack_Level=%lx IohOutofResInfo.Stack_MMIOLength[0][0]:%x", \
          IohOutofResInfo.MmioBase_Stack_Level[0][0], IohOutofResInfo.MmioLimit_Stack_Level[0][0],IohOutofResInfo.Stack_MMIOLength[0][0]));

  PCI_TRACE((TRACE_PCI, "\nIohOutofResInfo.Stack_Current_Boot_MMIORatio[0][0]:%x ", \
          IohOutofResInfo.Stack_Current_Boot_MMIORatio[0][0]));
  PCI_TRACE((TRACE_PCI, "\nCurrentMmioIoRatios.MMIO_Stack_Level[0][0]:%x ", \
          CurrentMmioIoRatios.MMIO_Stack_Level[0][0]));
  PCI_TRACE((TRACE_PCI, "\nAutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [0] [0]:%x\n", \
          AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[0][0]));

  for (SocketIndex = 0 ; SocketIndex < NumIoH ; SocketIndex++)
  {
      if(SocketIndex) IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex]=0; // Skipping the Clearing of Socket 0 Values as they are updated above.
      for (StackIndex = 0 ; StackIndex < MAX_IIO_STACK ; StackIndex++)
      {
          if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
            continue;
             
          if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
            continue;
          
          if (SocketIndex == 0 && StackIndex == 0 )
                continue;// As Base and Limit Values for Socket:0 Stack:0 are updated above.

          IohOutofResInfo.MmioBase_Stack_Level[SocketIndex][StackIndex] =  MmiolBase;
          IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex] = MmiolBase + TmpMmiolSize;
          IohOutofResInfo.Stack_MMIOLength[SocketIndex][StackIndex] = TmpMmiolSize;
          IohOutofResInfo.Stack_Current_Boot_MMIORatio[SocketIndex] [StackIndex]= TmpMmiolRatio;
          MmiolBase = IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex] + 1 ;
    
          //Updating the Ratios as per new Base in CurrentBoot and AutoAdjust Variables
          CurrentMmioIoRatios.MMIO_Stack_Level[SocketIndex][StackIndex] = TmpMmiolRatio;
          AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [SocketIndex][StackIndex]= TmpMmiolRatio;
          IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex] = IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex] + IohOutofResInfo.Stack_Current_Boot_MMIORatio[SocketIndex] [StackIndex];
          
          PCI_TRACE((TRACE_PCI, "\n Socket=%d Stack=%d  MmioBase_Stack_Level=%lx MmioLimit_Stack_Level=%lx TmpMmiolRatio=%x", \
                  SocketIndex,StackIndex,IohOutofResInfo.MmioBase_Stack_Level[SocketIndex][StackIndex], IohOutofResInfo.MmioLimit_Stack_Level[SocketIndex][StackIndex],TmpMmiolRatio));
          PCI_TRACE((TRACE_PCI, "\nIohOutofResInfo.Stack_Current_Boot_MMIORatio[%d] [%d]:%x ", \
                  SocketIndex,StackIndex,IohOutofResInfo.Stack_Current_Boot_MMIORatio[SocketIndex] [StackIndex]));
          PCI_TRACE((TRACE_PCI, "\nCurrentMmioIoRatios.MMIO_Stack_Level[%d] [%d]:%x ", \
                  SocketIndex,StackIndex,CurrentMmioIoRatios.MMIO_Stack_Level[SocketIndex] [StackIndex]));
          PCI_TRACE((TRACE_PCI, "\nAutoAdjustMmioIoVariable.MmiolRatio_Stack_Level [%d] [%d]:%x ", \
                  SocketIndex,StackIndex,AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex] [StackIndex]));
      }
      CurrentMmioIoRatios.MMIO [SocketIndex] = IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex];
      AutoAdjustMmioIoVariable.MmiolRatio [SocketIndex]= IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex];
      PCI_TRACE((TRACE_PCI, "\n\nIohOutofResInfo.Socket_Current_Boot_MMIORatio[%d]:%x ", \
              SocketIndex,IohOutofResInfo.Socket_Current_Boot_MMIORatio[SocketIndex]));
      PCI_TRACE((TRACE_PCI, "\nCurrentMmioIoRatios.MMIO[%d]:%x ", \
              SocketIndex, CurrentMmioIoRatios.MMIO[SocketIndex]));
      PCI_TRACE((TRACE_PCI, "\nAutoAdjustMmioIoVariable.MmiolRatio [%d] :%x \n", \
              SocketIndex, AutoAdjustMmioIoVariable.MmiolRatio[SocketIndex]));
   }
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     CheckWithThisMMIOLBase
//
// Description:   This function 
//
// Input:         UINT32  -  MmCfgBaseInTest
//                
//
// Output:        
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
CheckWithThisMMIOLBase (
UINT32          MmiolBaseInTest){
 
//    UINT8   SocketIndex;
//    UINT8   StackIndex;
    BOOLEAN Flag = FALSE;
    UINT8   OutOfResrcFailureType = MMIOFAILURE;
    BOOLEAN NotPossibleFlag = FALSE;
    PCI_TRACE((TRACE_PCI,__FUNCTION__": MmiolBaseInTest: %x\n",MmiolBaseInTest)); 
    
    Adjustmentneeded = FALSE;
    FoundMmioRatioAtleaseOnce = FALSE;

    FillNewBaseAndLimits(MmiolBaseInTest);
    // Apply the new base and calculate the Ratio again.
    do
    {
        if (Flag)ReApplyNewBase(OutOfResrcFailureType);
        CalculateRequiredSocketRatio(MMIOFAILURE);
        CheckExtraChunks(OutOfResrcFailureType,&NotPossibleFlag ,&Adjustmentneeded);
        if(NotPossibleFlag)
        {
            DEBUG((EFI_D_ERROR, "\nFindingNewBase: MMIO : NO VALID Socket's are present"));
            DEBUG((EFI_D_ERROR,__FUNCTION__": MmiolBaseInTest: %x was able NOT ABLE to Meet the Resource Requirement\nFAIL !!!. \n",MmiolBaseInTest)); 
            DEBUG((EFI_D_ERROR, "\nWill check with Next MmCfgBase\n"));
            IsNewMmCfgBaseSuccess = FALSE;
            return ;
        }
    SocketResourceAdjustment(MMIOFAILURE,&NotPossibleFlag);
    Flag = TRUE;
    } while (ReCompareSocketNewRatioWithCurrentBootRatio(OutOfResrcFailureType) && Adjustmentneeded);
          
    PCI_TRACE((TRACE_PCI,__FUNCTION__": MmiolBaseInTest: %x was able to Meet the Resource Requirement\nSUCCESS !!!. \n",MmiolBaseInTest)); 
    IsNewMmCfgBaseSuccess = TRUE;
    return ;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     CrbLowerMmCfgBase
//
// Description:   This function tries to find out the MmCfgBase Value to reduce to,
//                in order to meet the resource request requirement and save it 
//                Socket Common RC Config Variable
//
// Input:         LastBootFailedIOH  -  LastBootFailedIOH Structure
//                
//                
//
// Output:        
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
CrbLowerMmCfgBase( 
    IN  LAST_BOOT_FAILED_IOH            *LastBootFailedIOH,
    IN  SOCKET_COMMONRC_CONFIGURATION   *SocketCommonRcConfigDxe 
    )
{
    EFI_STATUS                  Status;
    UINT8                       IndexSocket;
    UINT8                       IndexStack;
    UINT64                      CurrentMmCfgBase;
    UINT8                       NewMmCfgBase = 0; // Keep MmCfgBase to 1G by default
    UINTN                       SocketVariableSize;
    SocketVariableSize = sizeof(SOCKET_COMMONRC_CONFIGURATION);
    
    CurrentMmCfgBase = gIioUds->IioUdsPtr->PlatformData.PciExpressBase;

    PCI_TRACE((TRACE_PCI,__FUNCTION__": CurrentMmCfgBase:%x\n",CurrentMmCfgBase)); 

    switch(CurrentMmCfgBase){
        case 0xC0000000: 
            //Current MmCfgBase is @ 3G
            // Checking if Lowering MmCfgBase to 2.25G meets the requirement
            CheckWithThisMMIOLBase(0xA0000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 4;
                break;
            }
            
            // With MmCfgBase@ 2.25G, Available Space Could not meet Resource Demand.
            // So, Checking if Lowering MmCfgBase to 2G meets the requirement
            CheckWithThisMMIOLBase(0x90000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 3;
                break;
            }

            // With MmCfgBase@ 2G, Available Space Could not meet Resource Demand.
            // So, Checking if Lowering MmCfgBase to 1.75G meets the requirement
            CheckWithThisMMIOLBase(0x80000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 2;
                break;
            }

            // With MmCfgBase@ 1.75G, Available Space Could not meet Resource Demand.
            // So, Checking if Lowering MmCfgBase to 1G meets the requirement
            CheckWithThisMMIOLBase(0x50000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 0;
                break;
            }
            
            // No Possibility of Lowering MmCfgBase Below 1GB. So, Halting the Machine with BEEP and 
            // Checkpoint Display 0xD4.
            HaltMachine();
            break; 

        case 0x90000000: // Base is @ 2.25G
            // Checking if Lowering MmCfgBase to 2G meets the requirement
            CheckWithThisMMIOLBase(0x90000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 3;
                break;
            }

            // With MmCfgBase@ 2G, Available Space Could not meet Resource Demand.
            // So, Checking if Lowering MmCfgBase to 1.75G meets the requirement
            CheckWithThisMMIOLBase(0x80000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 2;
                break;
            }

            // With MmCfgBase@ 1.75G, Available Space Could not meet Resource Demand.
            // So, Checking if Lowering MmCfgBase to 1G meets the requirement
            CheckWithThisMMIOLBase(0x50000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 0;
                break;
            }
            
            // No Possibility of Lowering MmCfgBase Below 1GB. So, Halting the Machine with BEEP and 
            // Checkpoint Display 0xD4.
            HaltMachine();
            break; 

        case 0x80000000: // Base is @ 2G
            // Checking if Lowering MmCfgBase to 1.75G meets the requirement
            CheckWithThisMMIOLBase(0x80000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 2;
                break;
            }

            // With MmCfgBase@ 1.75G, Available Space Could not meet Resource Demand.
            // So, Checking if Lowering MmCfgBase to 1G meets the requirement
            CheckWithThisMMIOLBase(0x50000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 0;
                break;
            }
            
            // No Possibility of Lowering MmCfgBase Below 1GB. So, Halting the Machine with BEEP and 
            // Checkpoint Display 0xD4.
            HaltMachine();
            break; 
        
        case 0x70000000: // Base is @ 1.75G
            // Checking if Lowering MmCfgBase to 1G meets the requirement
            CheckWithThisMMIOLBase(0x50000000);
            if(IsNewMmCfgBaseSuccess){
                NewMmCfgBase = 0;
                break;
            }
            // No Possibility of Lowering MmCfgBase Below 1GB. So, Halting the Machine with BEEP and 
            // Checkpoint Display 0xD4.
            HaltMachine();
            break;
        
        case 0x40000000: // Base is @ 1G
            // No Possibility of Lowering MmCfgBase Below 1GB. So, Halting the Machine with BEEP and 
            // Checkpoint Display 0xD4.
            HaltMachine();
            break; 
    }
    if(IsNewMmCfgBaseSuccess){
        SocketCommonRcConfigDxe->MmcfgBase = NewMmCfgBase;
        PCI_TRACE((TRACE_PCI,__FUNCTION__"\nLastBootFailedIOH.NewMmCfgBase%d, SocketCommonRcConfigDxe->MmcfgBase:%d\n",NewMmCfgBase, SocketCommonRcConfigDxe->MmcfgBase));
        Status = pRS->SetVariable( SOCKET_COMMONRC_CONFIGURATION_NAME,
                                        &gEfiSocketCommonRcVariableGuid,
                                        Attributes,
                                        SocketVariableSize,
                                        SocketCommonRcConfigDxe
                                        );
        if(EFI_ERROR(Status)){
            DEBUG((EFI_D_ERROR,__FUNCTION__"SocketCommonRcConfig-SetVariable Failed. Status:%r\n",Status));
            DEBUG((EFI_D_ERROR, "\nSystem Halted!!!!!!!!!!!!\n"));
            EFI_DEADLOOP();  
        }
        PCI_TRACE((TRACE_PCI,"SocketCommonRcConfig Variable Status: %r\n",Status)); 
        
        // Updating the Default values in LastBootFailedIOH Structure.
        for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++){
            for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
                LastBootFailedIOH->IoOutofResrcFlag[IndexSocket][IndexStack] =  0xAA;
                LastBootFailedIOH->MmmioOutofResrcFlag[IndexSocket][IndexStack] = 0xAA;
            }
        }
    }
}
#endif
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     IoOutOfResourceConditionOccured
//
// Description:   This function fills IO related information
//                 (IO Requested,IO Limit etc for the Failed Stack/Socket)
//
// Input:         SocketIndex     - Affected CPU
//                StackIndex      - Affected Stack
//                LastBootFailedIOH  -  LastBootFailedIOH Structure
//                IohOutofResInfo - IohOutofResInfo Strucuture
//                IoRequestedLen  - IoRequested Length
//                Gra             - Granularity    
//                IoTypeIndex     - IoTypeIndex (16/32)
//
// Output:        IohOutofResInfo - Updated IohOutofResInfo Structure
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS IoOutOfResourceConditionOccured (
    IN  UINTN                     SocketIndex,
    IN  UINTN                     StackIndex,
    IN  LAST_BOOT_FAILED_IOH      *LastBootFailedIOH,
    IN  IOHX_OUT_OF_RESOURCE_INFO *IohOutofResInfo,
    IN  UINT64                    IoRequestedLen,
    IN  UINT64                    Gra,
    IN  UINT8                     IoTypeIndex
)
{
	EFI_STATUS	  GetSetStatus;
	UINTN         	  VariableSize = sizeof (LAST_BOOT_FAILED_IOH);
	PCI_TRACE((TRACE_PCI,"Inside IoOutOfResourceConditionOccured\n"));	

	GetSetStatus = pRS->GetVariable	(L"LastBootFailedIoh",
                                   &LastBootFailedIohGuid,
                                   NULL,
                                   &VariableSize,
                                   LastBootFailedIOH );

   if(EFI_ERROR(GetSetStatus))
   {
       DEBUG((EFI_D_ERROR, "PciRB: IO Allocation Failed: GetVariable : LastBootFailedIOH Structure Failed\n"));
	   return GetSetStatus;
   }


   // After trying OEM DEFINED TABLE, same Root Bridge may fail. Try OEM DEFINED Table for NoOfStackFailureRetries times.
   // If same Root Bridge Fails again then this is an absolute not possible case so halt the system
   if (TempIoOutofResrcFlag[SocketIndex][StackIndex] == 1)
   {
	   if (NoOfStackFailureRetries)
	   {
	       DEBUG ((EFI_D_ERROR, "IO : System is Trying the OEM DEFINED TABLE for this much time!%d\n",NoOfStackFailureRetries));
	       DEBUG ((EFI_D_ERROR, "By removing the device from Socket:%d Stack:%d!\n",SocketIndex, StackIndex));
		   NoOfStackFailureRetries--;
		   return EFI_NOT_FOUND ;
	   }
	   else
	   {
	       DEBUG ((EFI_D_ERROR, "#2Requested IO space is not possible!  System is Halted!\n"));
		   ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
		   EFI_DEADLOOP();
	   }
   }

   //
   // Checking LastBootFailedIOH Structure
   // In Current and LastBoot,if the same Root Bridge has failed, then we should Try OEM Defined Table
   // Taking the backup of the  LastBootFailedIOH ->IoOutofResrcFlag.
   
   if (LastBootFailedIOH->IoOutofResrcFlag[SocketIndex][StackIndex] == 1)
   {
	   TempIoOutofResrcFlag[SocketIndex][StackIndex] = LastBootFailedIOH->IoOutofResrcFlag[SocketIndex][StackIndex];
	   LastBootFailedIOH->IoOutofResrcFlag[SocketIndex][StackIndex] =  0;
	   GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
                                       &LastBootFailedIohGuid,
                                       EFI_VARIABLE_NON_VOLATILE |
                                       EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                       VariableSize,
                                       LastBootFailedIOH );

	   if(EFI_ERROR(GetSetStatus))
	   {
	       DEBUG((EFI_D_ERROR, "PciRB: IO Allocation Failed: SetVariable : LastBootFailedIOH structure Failed\n"));
		   return GetSetStatus;
	   }

	   DEBUG((EFI_D_ERROR, "PciRB: IO Allocation Failed: System Tried for the best Ratio.System won't even boot with the best ratio\n"));
	   DEBUG((EFI_D_ERROR, "System is going to execute the OEM Defined Table for normal boot\n"));
	   NoOfStackFailureRetries--;
	   return EFI_NOT_FOUND ;
   } 
   
    IohOutofResInfo->Io_Gra_Stack_Level[SocketIndex][StackIndex][IoTypeIndex] = Gra;
    PCI_TRACE((TRACE_PCI,"\n SocketIndex=%d StackIndex=%d IO_GRA=%lx:\n",SocketIndex,StackIndex,IohOutofResInfo->Io_Gra_Stack_Level[SocketIndex][StackIndex][IoTypeIndex] ));       
    
    if (SocketIndex == 0 && StackIndex == 0 && IoTypeIndex == 0)
    {
        IoRequestedLen = IoRequestedLen + 0x1000 ; // Intel RC code uses Stack base as 0x1000 for CPU 0 Stack 0.
    }
    IohOutofResInfo->IoRequested_Stack_Level[SocketIndex][StackIndex][IoTypeIndex] = IoRequestedLen;
    
    //
    // Check whether it is Stack level or Socket level OutOfResource.
    //
  
    //
    // Socket Level Adjustment Needed
    //
    if (IoRequestedLen > IohOutofResInfo->Socket_IOLength[SocketIndex])
    {
        IohOutofResInfo->IoOutofResrcFlag[SocketIndex] = 1;
        SocketLevelAdjustmentNeeded = TRUE;
        SocketLevelIoAdjustmentNeeded = TRUE; 
               
        PCI_TRACE((TRACE_PCI,"\n Socket Level Adjustment Needed:\n" ));     
        PCI_TRACE((TRACE_PCI,"\n Socket Affected=%d \n",SocketIndex ));     
        PCI_TRACE((TRACE_PCI,"\n IO Requested=:%lx\n",IoRequestedLen ));
        return EFI_SUCCESS;
    }
  
    //
    // Stack level Adjustment 
    //
        IohOutofResInfo->IoOutofResrcFlag_Stack_Level[SocketIndex][StackIndex] = 1;
        StackLevelAdjustmentNeeded = TRUE;
        
        PCI_TRACE((TRACE_PCI,"\n Stack Level Adjustment Needed:\n" ));     
        PCI_TRACE((TRACE_PCI,"\n Socket Affected=%d Stack Affected=%d \n",SocketIndex,StackIndex ));     
        PCI_TRACE((TRACE_PCI,"\n IO Requested:%lx\n",IoRequestedLen ));     
        return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     MmioOutOfResourceConditionOccured
//
// Description:   This function fills MMIO related information
//                 (MMIO Requested,MMIO Limit etc for the Failed Stack/Socket)
//
// Input:         SocketIndex      - Affected CPU
//                StackIndex       - Affected Stack
//                LastBootFailedIOH  -  LastBootFailedIOH Structure
//                IohOutofResInfo  - IohOutofResInfo Strucuture
//                MmioRequestedLen - MmioRequestedLen Length
//                Gra              - Granularity    
//                MmioTypeIndex    - MmioTypeIndex (32/32p)
//
// Output:        IohOutofResInfo - Updated IohOutofResInfo Structure
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS MmioOutOfResourceConditionOccured (
    IN UINTN                     SocketIndex,
    IN UINTN                     StackIndex,
	IN  LAST_BOOT_FAILED_IOH 			*LastBootFailedIOH,
    IN IOHX_OUT_OF_RESOURCE_INFO *IohOutofResInfo,
    IN UINT64                    MmioRequestedLen,
    IN UINT64                    Gra,
    IN UINT8                     MmioTypeIndex
)
{
	EFI_STATUS	GetSetStatus;
	UINTN         VariableSize = sizeof (LAST_BOOT_FAILED_IOH);
    PCI_TRACE((TRACE_PCI, "Inside: MmioOutOfResourceConditionOccured\n"));	

	GetSetStatus = pRS->GetVariable	(L"LastBootFailedIoh",
                                   &LastBootFailedIohGuid,
                                   NULL,
                                   &VariableSize,
                                   LastBootFailedIOH);

	if(EFI_ERROR(GetSetStatus))
	{
	    DEBUG((EFI_D_ERROR, "PciRB: MMIO Allocation Failed: GetVariable : LastBootFailedIOH Structure Failed\n"));
		return GetSetStatus;
	}

   // After trying OEM DEFINED TABLE, same Root Bridge may fail. Try OEM DEFINED Table for NoOfStackFailureRetries times.
   // If same Root Bridge fails again then this is an absolute not possible case so halt the system
	if (TempMmmioOutofResrcFlag[SocketIndex][StackIndex] == 1)
	{
		if (NoOfStackFailureRetries)
		{
		    DEBUG ((EFI_D_ERROR, "MMIO : System is Trying the OEM DEFINED TABLE for this much time!%d\n",NoOfStackFailureRetries));
		    DEBUG ((EFI_D_ERROR, "By removing the device from Socket:%d Stack:%d!\n",SocketIndex, StackIndex));
			NoOfStackFailureRetries--;
			return EFI_NOT_FOUND ;
		}
		else
		{
		    DEBUG ((EFI_D_ERROR, "#4 Requested MMIO space is not possible!  System is Halted!\n"));
			ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
			EFI_DEADLOOP();
		}
	}

   //
   // Checking LastBootFailedIOH Structure
   // In Current and LastBoot,if the same Root Bridge has failed,then we should Try OEM Defined Table
   // Taking the backup of the  LastBootFailedIOH ->IoOutofResrcFlag.
   
	if ((LastBootFailedIOH->MmmioOutofResrcFlag[SocketIndex][StackIndex] == 1))
	{
		TempMmmioOutofResrcFlag[SocketIndex][StackIndex] = LastBootFailedIOH->MmmioOutofResrcFlag[SocketIndex][StackIndex];
		LastBootFailedIOH->MmmioOutofResrcFlag[SocketIndex][StackIndex] =  0;
		GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
                                       &LastBootFailedIohGuid,
                                       EFI_VARIABLE_NON_VOLATILE |
                                       EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                       VariableSize,
                                       LastBootFailedIOH );

		if(EFI_ERROR(GetSetStatus))
		{
		    DEBUG((EFI_D_ERROR, "PciRB: IO Allocation Failed: SetVariable : LastBootFailedIOH structure Failed\n"));
			return GetSetStatus;
		}
		DEBUG((EFI_D_ERROR, "PciRB: MMIO Allocation Failed: System Tried for the best Ratio.System won't even boot with the best ratio\n"));
		DEBUG((EFI_D_ERROR, "System is going to execute the OEM Defined Table for normal boot\n"));
		NoOfStackFailureRetries--;
		return EFI_NOT_FOUND ; 
	}
    IohOutofResInfo->Mmio_Gra_Stack_Level[SocketIndex][StackIndex][MmioTypeIndex] = Gra;
    PCI_TRACE((TRACE_PCI,"\n SocketIndex=%d StackIndex=%d MMIO_GRA=%lx\n",SocketIndex,StackIndex,IohOutofResInfo->Mmio_Gra_Stack_Level[SocketIndex][StackIndex][MmioTypeIndex] ));
    
    IohOutofResInfo->MmioRequested_Stack_Level[SocketIndex][StackIndex][MmioTypeIndex] = MmioRequestedLen;
    
    //
    // Check whether it is Stack level or Socket level OutOfResource.
    //
    
    //
    // Socket Level Adjustment Needed
    //    
    if (MmioRequestedLen > IohOutofResInfo->Socket_MMIOLength[SocketIndex])
    {
        IohOutofResInfo->MmmioOutofResrcFlag[SocketIndex] = 1;
        SocketLevelAdjustmentNeeded = TRUE;
        SocketLevelMMIoAdjustmentNeeded = TRUE; 
            
        PCI_TRACE((TRACE_PCI,"\n Socket Level Adjustment Needed:\n" ));
        PCI_TRACE((TRACE_PCI,"\n Socket Affected=%d \n",SocketIndex ));
        PCI_TRACE((TRACE_PCI,"\n MMIO Requested:%lx\n",MmioRequestedLen ));
        return EFI_SUCCESS;
    }
    
    //
    // Stack level Adjustment
    //
        IohOutofResInfo->MmmioOutofResrcFlag_Stack_Level[SocketIndex][StackIndex] = 1;
        StackLevelAdjustmentNeeded = TRUE;
        
        PCI_TRACE((TRACE_PCI,"\n Stack Level Adjustment Needed:\n" ));
        PCI_TRACE((TRACE_PCI,"\n Socket Affected=%d Stack Affected=%d \n",SocketIndex,StackIndex ));
        PCI_TRACE((TRACE_PCI,"\n MMIO Requested:%lx\n",MmioRequestedLen ));
        return EFI_SUCCESS;
    
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:     StackAdjustedDuringSocketAdjustment
//
// Description:   This function checks whether Stack is adjusted during Socket Adjustmentn
//
// Input:         SocketNumber      - Affected CPU
//                OutOfResrcFailure - MMIOL /IO
// Output:        TRUE/FALSE - TRUE - Stacks Adjusted During Socket Adjustment FALSE - Stacks not Adjusted
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN StackAdjustedDuringSocketAdjustment (
    UINTN SocketNumber,
    UINT8  OutOfResrcFailure)
{
    UINT8 Index = 0 ;
    UINT8 StackIndex = 0;
    
    if (SocketAutoAdjustCalledOnce == FALSE && StackAutoAdjustCalledOnce == FALSE)
          return FALSE;
    

    for (Index = 0; Index < MAX_SOCKET; Index++) 
    {
      if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[Index].Valid) 
      {
        continue;
      }
      
      if (OutOfResrcFailure == MMIOFAILURE )
      {
         if (IOHUsedDuringSocketMMIOAdjustment [Index] == SocketNumber)
           return TRUE;
      }
      
      if (OutOfResrcFailure == IOFAILURE )
      {
         if (IOHUsedDuringSocketIOAdjustment [Index] == SocketNumber)
           return TRUE;
      }
    }
    
    for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) 
    {
      if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketNumber].stackPresentBitmap) & (1 << StackIndex))) 
      {
            continue;
      }
        
      if (OutOfResrcFailure == MMIOFAILURE )
      {
        if (IohOutofResInfo.MmmioOutofResrcFlag_Stack_Level [SocketNumber][StackIndex])
        {
              CalculateRequiredRatio(MMIOFAILURE,SocketNumber,FALSE);
              if (RequiredMMIOIoRatio.MMIO_Stack_Level[SocketNumber][StackIndex] != AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketNumber][StackIndex]) 
                return FALSE;
              else
                return TRUE;
 
        }
      }
      
      if (OutOfResrcFailure == IOFAILURE )
      {
         if (IohOutofResInfo.IoOutofResrcFlag_Stack_Level [SocketNumber][StackIndex])
         {
             CalculateRequiredRatio(IOFAILURE,SocketNumber,FALSE);
             if (RequiredMMIOIoRatio.IO_Stack_Level[SocketNumber][StackIndex] != AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketNumber][StackIndex])
               return FALSE;
             else
               return TRUE;
         }
      }
    }
    return TRUE;

}
#endif
// APTIOV_SERVER_OVERRIDE_END
/**
    This function is invoked in PCI Host Bridge Driver when time
    to ask GCD for resources. You can overwrite a default
    algorithm used to allocate resources for the Root Bridge.

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData Pointer to Root Bridge private structure data.
    @param RootBrgIndex Root Bridge index (0 Based).

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspAllocateResources (
							IN PCI_HOST_BRG_DATA    *HostBrgData,
							IN PCI_ROOT_BRG_DATA    *RootBrgData,
							IN UINTN                RootBrgIndex )
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;
//###
//### //PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//### //PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//### //Any Additional Variables goes here
// APTIOV_SERVER_OVERRIDE_START
// Dynamic Resource Allocation
 	UINTN						i;
 	ASLR_QWORD_ASD				*res;
 	UINT64						a=0;
 	UINT64						AddressLimit=0;
 	DXE_SERVICES				*dxe;
	EFI_STATUS		    		RetStatus = EFI_SUCCESS;
#if SMCPKG_SUPPORT
	SETUP_DATA	AmiSetupData;
	UINT32		SetupAttr;
#endif
#if CRB_OUT_OF_RESOURCE_SUPPORT	
	UINTN                       VariableSize = sizeof (LAST_BOOT_FAILED_IOH);
	LAST_BOOT_FAILED_IOH        LastBootFailedIOH;
	EFI_STATUS                  GetSetStatus;
	UINT8                       IndexSocket;
	UINT8                       IndexStack;
	EFI_STATUS                  FreeIoStatus;
	UINT8                       MmioTypeIndex = 0;
	UINT8                       IoTypeIndex = 0;
	UINT8			    		OutOfResourceFailure = 0xFF;
	EFI_STATUS		    		MmioStatus;
	EFI_STATUS		    		IoStatus;
	UINTN                       SetupDataVarSize = sizeof(SYSTEM_CONFIGURATION);
	UINTN                       TempSocketIndex, TempStackIndex;
	UINT8						FailedStackIndex = 0, FailedSocketNumber = 0;
    UINTN                       SocketVariableSize;
    SOCKET_COMMONRC_CONFIGURATION           SocketCommonRcConfigDxe;
#endif
    static UINTN                       NumberOfValidRb = 0;
	UINTN                       SocketIndex, StackIndex;
// APTIOV_SERVER_OVERRIDE_START : Added to support FPGA
#if FPGA_SUPPORT
    UINTN                           Index;
#endif
// APTIOV_SERVER_OVERRIDE_END : Added to support FPGA    

 #if BoardPciRes_SUPPORT
     AMI_OUT_OF_RES_VAR  OutOfResVar;
 //--------------------------------------

     //Check if OUT_OF_RES_VAR was already created.
     Status=AmiPciOutOfRes(&OutOfResVar, TRUE);
     if(EFI_ERROR(Status)) pBS->SetMem(&OutOfResVar,sizeof(AMI_OUT_OF_RES_VAR),0);
     else OutOfResVar.Count++;
 #endif
 	// ported for four IOHs

  	//
    // Validate RootBrgIndex number
    //
#if FPGA_SUPPORT
    if (RootBrgIndex >= (sizeof(Rootbridgeindexloc) / sizeof(ROOTBRIDGEINDEXLOCATOR))) {
        return EFI_UNSUPPORTED;
    }
#else
 	if ( RootBrgIndex > (MAX_SOCKET * MAX_IIO_STACK - 1) ) {
 		return EFI_UNSUPPORTED;
 	}
#endif
// APTIOV_SERVER_OVERRIDE_END : Added to support FPGA
 	PCI_TRACE((TRACE_PCI," (HbCspAllocateResources called for Rb index: %d ", RootBrgIndex ));

	if ( gRbIndexSocketStackMapUpdated == FALSE ) {
  		Status = InitRbIndexSocketStackMap ();
		ASSERT_EFI_ERROR(Status);
 		if ( EFI_ERROR(Status) ) {
 			return Status;
 		}
  		gRbIndexSocketStackMapUpdated = TRUE;
		NumberOfValidRb = gMaxRbIndex;
  	}
	//
 	// Locate IIOUDS protocol if not located already
 	//
 	if ( gIioUds == NULL ) {
		Status = pBS->LocateProtocol(
						&gEfiIioUdsProtocolGuid,
						NULL,
						&gIioUds );
		ASSERT_EFI_ERROR (Status);
		if ( EFI_ERROR(Status) ) {
			return Status;
		}
 	}

 	Status=LibGetDxeSvcTbl(&dxe);
 	if(EFI_ERROR(Status)) return Status;
 	
	//
	// Get Socket and Stack Index from RootBrgIndex
	//
// APTIOV_SERVER_OVERRIDE_START : Added to support FPGA
#if FPGA_SUPPORT
 	for (Index=0; Index < (sizeof(Rootbridgeindexloc) / sizeof(ROOTBRIDGEINDEXLOCATOR)); Index++) {
 	    if (RootBrgIndex==Rootbridgeindexloc[Index].RootBridgeIndex) {
 	        SocketIndex = Rootbridgeindexloc[Index].SocketNumber;
 	        StackIndex = Rootbridgeindexloc[Index].StackNumber;
 	    }
 	}
#else
 	SocketIndex = RootBrgIndex/MAX_IIO_STACK; 	
 	StackIndex = RootBrgIndex%MAX_IIO_STACK; //within each IIO socket ID, the stack resources are indexed 0 to 5.
#endif
// APTIOV_SERVER_OVERRIDE_END : Added to support FPGA 	
 	PCI_TRACE((TRACE_PCI," Socket %d Stack %d );\n", SocketIndex, StackIndex )); 	

     //Fill out needed structures.
 	if(RootBrgData->AcpiRbRes[raIo].Min==0){ //do it only once
     	RootBrgData->AcpiRbRes[raIo].Min=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceIoBase;
     	RootBrgData->AcpiRbRes[raIo].Max=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceIoLimit;
     	RootBrgData->AcpiRbRes[raIo].Len=RootBrgData->AcpiRbRes[raIo].Max-RootBrgData->AcpiRbRes[raIo].Min+1;
     	RootBrgData->AcpiRbRes[raIo].AddrUsed=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceIoLimit+1;
     	RootBrgData->AcpiRbRes[raIo].AllocType=EfiGcdAllocateMaxAddressSearchTopDown;

      RootBrgData->AcpiRbRes[raMmio32].Min=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem32Base;
      RootBrgData->AcpiRbRes[raMmio32].Max=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem32Limit;
      RootBrgData->AcpiRbRes[raMmio32].Len=RootBrgData->AcpiRbRes[raMmio32].Max-RootBrgData->AcpiRbRes[raMmio32].Min+1;
      RootBrgData->AcpiRbRes[raMmio32].AddrUsed=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem32Limit+1;
      RootBrgData->AcpiRbRes[raMmio32].AllocType=EfiGcdAllocateMaxAddressSearchTopDown;

      RootBrgData->AcpiRbRes[raMmio64].Min=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem64Base;
      RootBrgData->AcpiRbRes[raMmio64].Max=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem64Limit;
      RootBrgData->AcpiRbRes[raMmio64].Len=RootBrgData->AcpiRbRes[raMmio64].Max-RootBrgData->AcpiRbRes[raMmio64].Min+1;
      RootBrgData->AcpiRbRes[raMmio64].AddrUsed=gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem64Limit+1;
      RootBrgData->AcpiRbRes[raMmio64].AllocType=EfiGcdAllocateMaxAddressSearchTopDown;
 	}
#if CRB_OUT_OF_RESOURCE_SUPPORT
	
 	//
	// CsiSocketResourceAutoAdjust is enabled in Setup.
	//
	if (1)// (SystemConfiguration.CsiSocketResourceAutoAdjust)
	{
		NumberOfIoH = NbGetMaximumIohSockets();
        NumberofStacksPerCpu = (UINT8)(gMaxRbIndex / NumberOfIoH); 

		// Creating LastBootFailedIoh Variable with Dummy Values. 

		GetSetStatus = pRS->GetVariable (L"LastBootFailedIoh",
		                       &LastBootFailedIohGuid,
		                       NULL,
		                       &VariableSize,
		                       &LastBootFailedIOH );

  		if(EFI_ERROR(GetSetStatus))
		{
    		DEBUG((EFI_D_ERROR, "PciRB: IO Allocation Failed: GetVariable : First Time Creating LastBootFailedIOH Structure\n"));
			//
			//  Creating the DUMMY LastBootFailedIOH Structure
			//
    		for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++)
    		{
    			for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
				LastBootFailedIOH.IoOutofResrcFlag[IndexSocket][IndexStack] =  0xAA;
				LastBootFailedIOH.MmmioOutofResrcFlag[IndexSocket][IndexStack] = 0xAA;
    	    		}

    		}

    		GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
									&LastBootFailedIohGuid,
									EFI_VARIABLE_NON_VOLATILE |
									EFI_VARIABLE_BOOTSERVICE_ACCESS,
									VariableSize,
									&LastBootFailedIOH );
    		if (EFI_ERROR(GetSetStatus)){
      			DEBUG((EFI_D_ERROR, "PciRB: SetVariable Failed: Creating LastBootFailedIOH Structure\n"));
      			return GetSetStatus;
    		}
		}
  		// Geting Socket Common RC Config Setup Variable to Know PCI Segment(Single or Multi-Segment)
  		// Environment currently configured on System
  	    SocketVariableSize = sizeof(SOCKET_COMMONRC_CONFIGURATION);
  	    
  		GetSetStatus = pRS->GetVariable( SOCKET_COMMONRC_CONFIGURATION_NAME,
  		                                &gEfiSocketCommonRcVariableGuid,
  		                                &Attributes,
  		                                &SocketVariableSize,
  		                                &SocketCommonRcConfigDxe
  		                                );
  		if(EFI_ERROR(Status)){
  		    DEBUG((EFI_D_ERROR, "\n HbCspAllocateResources(): GetVariable(SOCKET_COMMONRC_CONFIGURATION_NAME)Failed. Status:%r\n",GetSetStatus));
  		    return Status;
  		}
		// 
		// All Stack's and Socket Data will be filled in IohOutofResInfo Structure once.
		// 
		if (IohOutofResInfoStructureUpdated) goto IohOutofResStructureDone;
		
		for (TempSocketIndex = 0; TempSocketIndex < MAX_SOCKET; TempSocketIndex++) {
            Socket_IO_Failed_Socket[TempSocketIndex] =  0xFF;
            Socket_MMIO_Failed_Socket[TempSocketIndex]  =  0xFF;
            Stack_IO_Failed_Socket[TempSocketIndex]  =  0xFF;
            Stack_MMIO_Failed_Socket[TempSocketIndex]  =  0xFF;
    if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].Valid) {
    			continue;
		}
    
    for (TempStackIndex = 0; TempStackIndex < MAX_IIO_STACK; TempStackIndex++) {
    if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[TempSocketIndex].stackPresentBitmap) & (1 << TempStackIndex)))
    	continue;
    

    // ########################### STACK LEVEL #############################    
    //  As per RC CODE, adding Extra 0x1000 IO to Stack 0 CPU 0
    if (TempSocketIndex == 0 && TempStackIndex == 0 )
    {
    	IohOutofResInfo.IoBase_Stack_Level[TempSocketIndex] [TempStackIndex] =  0;
    }
    else
    {
    	IohOutofResInfo.IoBase_Stack_Level[TempSocketIndex] [TempStackIndex] =  gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].StackRes[TempStackIndex].PciResourceIoBase;
    }
    
    IohOutofResInfo.IoLimit_Stack_Level[TempSocketIndex] [TempStackIndex] = ( gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].StackRes[TempStackIndex].PciResourceIoLimit)+1;
    
    IohOutofResInfo.MmioBase_Stack_Level[TempSocketIndex] [TempStackIndex] =  gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].StackRes[TempStackIndex].PciResourceMem32Base;
    IohOutofResInfo.MmioLimit_Stack_Level[TempSocketIndex] [TempStackIndex] = ( gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].StackRes[TempStackIndex].PciResourceMem32Limit)+1;
    					
    // No resource allocated to this stack by RC.
    if (IohOutofResInfo.IoBase_Stack_Level[TempSocketIndex] [TempStackIndex] > IohOutofResInfo.IoLimit_Stack_Level[TempSocketIndex] [TempStackIndex])
    {
    	IohOutofResInfo.Stack_IOLength[TempSocketIndex] [TempStackIndex]  = 0;
    }
    
    else
    {
    	IohOutofResInfo.Stack_IOLength[TempSocketIndex] [TempStackIndex]  = (IohOutofResInfo.IoLimit_Stack_Level[TempSocketIndex] [TempStackIndex] - IohOutofResInfo.IoBase_Stack_Level[TempSocketIndex] [TempStackIndex] );
    }
    					
    if (IohOutofResInfo.MmioBase_Stack_Level[TempSocketIndex] [TempStackIndex] > IohOutofResInfo.MmioLimit_Stack_Level[TempSocketIndex] [TempStackIndex])
    {
    	IohOutofResInfo.Stack_MMIOLength[TempSocketIndex] [TempStackIndex]  = 0;
    }
    
    else
    {
    	IohOutofResInfo.Stack_MMIOLength[TempSocketIndex] [TempStackIndex]  = (IohOutofResInfo.MmioLimit_Stack_Level[TempSocketIndex] [TempStackIndex] - IohOutofResInfo.MmioBase_Stack_Level[TempSocketIndex] [TempStackIndex] );
    }

    IohOutofResInfo.Stack_Current_Boot_IORatio[TempSocketIndex] [TempStackIndex] =  (UINT16) (IohOutofResInfo.Stack_IOLength[TempSocketIndex] [TempStackIndex] / SAD_LEG_IO_GRANTY_2KB); // 2KB GRA
		IohOutofResInfo.Stack_Current_Boot_MMIORatio[TempSocketIndex] [TempStackIndex] = (UINT16) (IohOutofResInfo.Stack_MMIOLength[TempSocketIndex] [TempStackIndex] / SAD_MMIOL_GRANTY_4MB) ;// 4MB GRA
				        		
    IohOutofResInfo.Socket_Current_Boot_IORatio[TempSocketIndex] = IohOutofResInfo.Socket_Current_Boot_IORatio[TempSocketIndex] + IohOutofResInfo.Stack_Current_Boot_IORatio[TempSocketIndex] [TempStackIndex];
    IohOutofResInfo.Socket_Current_Boot_MMIORatio[TempSocketIndex] = IohOutofResInfo.Socket_Current_Boot_MMIORatio[TempSocketIndex] + IohOutofResInfo.Stack_Current_Boot_MMIORatio[TempSocketIndex] [TempStackIndex];
    					
    					
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo Structure information Stack Level:\n" )); 	
    PCI_TRACE((TRACE_PCI,"\n SocketIndex=%d StackIndex=%d \n",TempSocketIndex,TempStackIndex )); 

    PCI_TRACE((TRACE_PCI,"\n #################################################:\n" )); 	
    PCI_TRACE((TRACE_PCI,"\n Stack IO information:\n" )); 	
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.IoBase_Stack_Level=%lx \n",IohOutofResInfo.IoBase_Stack_Level[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.IoLimit_Stack_Level=%lx \n",IohOutofResInfo.IoLimit_Stack_Level[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Stack_IOLength=%lx \n",IohOutofResInfo.Stack_IOLength[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Stack_Current_Boot_IORatio=%lx \n",IohOutofResInfo.Stack_Current_Boot_IORatio[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n #################################################:\n" )); 	
    		  
    PCI_TRACE((TRACE_PCI,"\n #################################################:\n" ));
    PCI_TRACE((TRACE_PCI,"\n Stack MMIO information:\n" )); 
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.MmioBase_Stack_Level=%lx \n",IohOutofResInfo.MmioBase_Stack_Level[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.MmioLimit_Stack_Level=%lx \n",IohOutofResInfo.MmioLimit_Stack_Level[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Stack_MMIOLength=%lx \n",IohOutofResInfo.Stack_MMIOLength[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Stack_Current_Boot_MMIORatio=%lx \n",IohOutofResInfo.Stack_Current_Boot_MMIORatio[TempSocketIndex] [TempStackIndex] ));
    PCI_TRACE((TRACE_PCI,"\n #################################################:\n" )); 	
    		         	
    		           
    } // for loop for Stack
    	
    // ########################### SOCKET LEVEL #############################
    		
    IohOutofResInfo.IoBase[TempSocketIndex] = gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].PciResourceIoBase;
    IohOutofResInfo.IoLimit[TempSocketIndex] = (gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].PciResourceIoLimit)+1;
    IohOutofResInfo.MmioBase[TempSocketIndex] = gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].PciResourceMem32Base;
    IohOutofResInfo.MmioLimit[TempSocketIndex] = (gIioUds->IioUdsPtr->PlatformData.IIO_resource[TempSocketIndex].PciResourceMem32Limit)+1;
    		        
    IohOutofResInfo.Socket_IOLength[TempSocketIndex]  = (IohOutofResInfo.IoLimit[TempSocketIndex] - IohOutofResInfo.IoBase[TempSocketIndex] );
    IohOutofResInfo.Socket_MMIOLength[TempSocketIndex]  = (IohOutofResInfo.MmioLimit[TempSocketIndex] - IohOutofResInfo.MmioBase[TempSocketIndex] );
    		        
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo Structure information Socket Level:\n" )); 	
    PCI_TRACE((TRACE_PCI,"\n SocketIndex=%d\n",TempSocketIndex ));	
    
    PCI_TRACE((TRACE_PCI,"\n #################################################:\n" )); 	
    PCI_TRACE((TRACE_PCI,"\n Socket IO information:\n" )); 
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.IoBase=%lx \n",IohOutofResInfo.IoBase[TempSocketIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.IoLimit=%lx \n",IohOutofResInfo.IoLimit[TempSocketIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Socket_IOLength=%lx \n",IohOutofResInfo.Socket_IOLength[TempSocketIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Socket_Current_Boot_IORatio=%lx \n",IohOutofResInfo.Socket_Current_Boot_IORatio[TempSocketIndex] ));
    	
    PCI_TRACE((TRACE_PCI,"\n #################################################:\n" )); 	
    PCI_TRACE((TRACE_PCI,"\n Socket MMIO information:\n" )); 
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.MmioBase=%lx \n",IohOutofResInfo.MmioBase[TempSocketIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.MmioLimit=%lx \n",IohOutofResInfo.MmioLimit[TempSocketIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Socket_MMIOLength=%lx \n",IohOutofResInfo.Socket_MMIOLength[TempSocketIndex] ));
    PCI_TRACE((TRACE_PCI,"\n IohOutofResInfo.Socket_Current_Boot_MMIORatio=%lx \n",IohOutofResInfo.Socket_Current_Boot_MMIORatio[TempSocketIndex] ));
    PCI_TRACE((TRACE_PCI,"\n #################################################:\n" )); 	
    
    PCI_TRACE((TRACE_PCI,"\n Socket_IO_Failed_Socket[%d]=%d ",TempSocketIndex,Socket_IO_Failed_Socket[TempSocketIndex]));
    PCI_TRACE((TRACE_PCI,"\n Socket_MMIO_Failed_Socket[%d]=%d ",TempSocketIndex,Socket_MMIO_Failed_Socket[TempSocketIndex]));
    PCI_TRACE((TRACE_PCI,"\n Stack_IO_Failed_Socket[%d]=%d ",TempSocketIndex,Stack_IO_Failed_Socket[TempSocketIndex]));
    PCI_TRACE((TRACE_PCI,"\n Stack_MMIO_Failed_Socket[%d]=%d\n ",TempSocketIndex,Stack_MMIO_Failed_Socket[TempSocketIndex]));

    } // for loop for Socket
    	
		IohOutofResInfoStructureUpdated = TRUE;
	}

IohOutofResStructureDone :

#endif
 	for(i=0; i<RootBrgData->ResCount; i++){
 		res=RootBrgData->RbRes[i];
 		if(res->Type==ASLRV_SPC_TYPE_BUS) {
 			PCI_TRACE((TRACE_PCI, "PciRB: #%X BUS _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX, _GRA=0x%1X\n",
 						RootBrgIndex,res->_MIN,res->_MAX,res->_LEN, res->_GRA));
 			continue;
 		}

 		// update io and memory values from qpi rc output.
 		//Allocate IO
 		if(res->Type==ASLRV_SPC_TYPE_IO){

            PCI_TRACE((TRACE_PCI, "PciRB: #%X BUS _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX, _GRA=0x%1X\n",
 						RootBrgIndex,res->_MIN,res->_MAX,res->_LEN, res->_GRA));
         //
         //Set Resource starting address for IO based on which RB it is.
         // Update Root Bridge with UDS resource information
         //
 	    	RootBrgData->AcpiRbRes[raIo].Gra=res->_MAX;
 			a = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceIoBase;
     	    AddressLimit = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceIoLimit;
 			AddressLimit = AddressLimit + 1; // make power of 2
	
 			PCI_TRACE((TRACE_PCI,"PciRB: #%X Resources Type=%X; routed by QPI: Address base = %lX; AddressLimit=%lX; \n",
 			RootBrgIndex, res->Type, a, AddressLimit));

 			PCI_TRACE((TRACE_PCI, "PciRB: #%X IO ",RootBrgIndex));
             //Use prefered method to allocate resources for
 			Status=dxe->AllocateIoSpace(EfiGcdAllocateMaxAddressSearchTopDown,
 							EfiGcdIoTypeIo,
 							AlignFromGranularity((UINTN)res->_MAX),
 							res->_LEN,
 							&AddressLimit,
 							RootBrgData->ImageHandle,
 							RootBrgData->RbHandle);

 			if(EFI_ERROR(Status))
 			{
 					DEBUG((EFI_D_ERROR, "PciRB: IO Allocation Failed: _LEN=%lX; _GRA=l%X.\n",res->_LEN, res->_MAX));

 #if BoardPciRes_SUPPORT
           OutOfResVar.Resource=*res;
           AmiPciOutOfRes(&OutOfResVar, FALSE);
 #endif
#if CRB_OUT_OF_RESOURCE_SUPPORT           
 					if (1)// (SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
 					{
 						IoStatus = IoOutOfResourceConditionOccured(SocketIndex,StackIndex,&LastBootFailedIOH,&IohOutofResInfo,res->_LEN,res->_MAX,IoTypeIndex);
 						//
 						// System will try OEM DEFINED TABLE if Status is other than Success.
 						// If Success : Best Ratio is calculated
 						if (IoStatus != EFI_SUCCESS)
 						{
 							return IoStatus;
 						}
 					}
 					//
 					// Manual Option is selected: Don't try our AutoAdjust Algorithm.
 					//
 					else
 					{
 						return Status;
 					}
#else
 		 	return Status;
#endif //CRB_OUT_OF_RESOURCE_SUPPORT            		 	
 			}

       // If IO Space was allocated to space not reserved for current Root Bridge, free space and return error
       if((Status == EFI_SUCCESS) && (AddressLimit < gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceIoBase))
 			{
 					PCI_TRACE((TRACE_PCI,"PciRB: IO Allocation Failed! Could not fit in Root Bridge Allotted IO Resources: AllocatedIoBase=%x, RequiredIoBase=%x, _LEN=%lX; _GRA=l%X.\n",\
                         		AddressLimit,\
                         		gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceIoBase,\
                         		res->_LEN, res->_MAX));


           dxe->FreeIoSpace(AddressLimit, res->_LEN);
           Status = EFI_NOT_FOUND;
 #if BoardPciRes_SUPPORT
           OutOfResVar.Resource=*res;
           AmiPciOutOfRes(&OutOfResVar, FALSE);
 #endif

#if CRB_OUT_OF_RESOURCE_SUPPORT           
			if (1)// (SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
			{
				IoStatus = IoOutOfResourceConditionOccured(SocketIndex,StackIndex,&LastBootFailedIOH,&IohOutofResInfo,res->_LEN,res->_MAX,IoTypeIndex);
				//
				// System will try OEM DEFINED TABLE if Status is other than Success.
				// If Success : Best Ratio is calculated
				if (IoStatus != EFI_SUCCESS )
				{
						return IoStatus;
				}
			}
			//
			// Manual Option is selected: Don't try our AutoAdjust Algorithm.
			// 
			else
			{
				return Status;
			}
#else
 		 	return Status;
#endif //CRB_OUT_OF_RESOURCE_SUPPORT            		 	
		}
#if CRB_OUT_OF_RESOURCE_SUPPORT
		//
		// Filling the IohOutofResInfo Structure even for the SUCCESS case
		//
		if (Status == EFI_SUCCESS)
		{
			if (1) //(SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
			{
				IohOutofResInfo.IoRequested_Stack_Level[SocketIndex][StackIndex] [IoTypeIndex] = res->_LEN;
				IohOutofResInfo.Io_Gra_Stack_Level[SocketIndex][StackIndex][IoTypeIndex] = res->_MAX;
			}
		}

		if (1)//(SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
		{
		    IoTypeIndex = IoTypeIndex + 1;
		}
#endif		
	}
         //Allocate MMIO
 		else if( res->Type==ASLRV_SPC_TYPE_MEM){
             //First root or Second one????
             if(res->_GRA==32){
                 a = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem32Base;
                 AddressLimit = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem32Limit;
 								AddressLimit = AddressLimit + 1; // make power of 2
 								PCI_TRACE((TRACE_PCI, "PciRB: #%X MMIO32",RootBrgIndex));
 								RootBrgData->AcpiRbRes[raMmio32].Gra=res->_MAX;
           } else {
                 a = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem64Base;
                 AddressLimit = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem64Limit;
                 AddressLimit = AddressLimit + 1; // make power of 2
                 PCI_TRACE((TRACE_PCI, "PciRB: #%X MMIO64",RootBrgIndex));
 		         RootBrgData->AcpiRbRes[raMmio64].Gra=res->_MAX;
 			}

            PCI_TRACE((TRACE_PCI,"PciRB: #%X Resources Type=%X; routed by QPI: _GRA=%lX; Address base = %lX; AddressLimit=%lX; \n",
 			RootBrgIndex, res->Type, res->_GRA, a, AddressLimit));

 			Status=dxe->AllocateMemorySpace(EfiGcdAllocateMaxAddressSearchTopDown,
 							EfiGcdMemoryTypeMemoryMappedIo,
 							AlignFromGranularity((UINTN)res->_MAX),
 							res->_LEN,
 							&AddressLimit,
 							RootBrgData->ImageHandle,
 							RootBrgData->RbHandle);

 			if(EFI_ERROR(Status))
 			{
 					DEBUG((EFI_D_ERROR, "PciRootBrg: Memory Allocation Failed: Length: %lX\n",res->_LEN));
#if SMCPKG_SUPPORT
    					VariableSize = sizeof(SETUP_DATA);
    					GetSetStatus = pRS->GetVariable(
    							L"Setup",
    				                        &gEfiSetupVariableGuid,
    				                        &SetupAttr,
    				                        &VariableSize,
    				                        &AmiSetupData);

    				        if(!GetSetStatus && !AmiSetupData.Above4gDecode){
    				        	TRACE((-1, "SMC enable Above4gDecode and reset.\n"));
    				        	AmiSetupData.Above4gDecode = 1;
    				        	GetSetStatus = pRS->SetVariable(
    				        			L"Setup",
    				        			&gEfiSetupVariableGuid,
    				        			SetupAttr,
    				        			VariableSize,
    				        			&AmiSetupData);

    				        	if(!GetSetStatus)
    				        		pRS->ResetSystem(EfiResetCold, 0, 0, NULL);
    				        }
#endif	//#endif SMCPKG_SUPPORT
 #if BoardPciRes_SUPPORT
                 OutOfResVar.Resource=*res;
                 AmiPciOutOfRes(&OutOfResVar, FALSE);
 #endif

          if (res->_GRA !=32) return Status ; // CRB BIOS is not handling MMIOH OOR Failure
#if CRB_OUT_OF_RESOURCE_SUPPORT  
 					if (1) //(SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
 					{
 						MmioStatus = MmioOutOfResourceConditionOccured(SocketIndex,StackIndex,&LastBootFailedIOH,&IohOutofResInfo,res->_LEN,res->_MAX,MmioTypeIndex);
 						//
 						// System will try OEM DEFINED TABLE if Status is other than Success.
 						// If Success : Best Ratio is calculated
 						if (MmioStatus != EFI_SUCCESS)
 						{
						// Root Bridge Resource Allocation takes place in order of Io Resources followed by Mmio Resources. 
						// If Mmio Resource failure occurs, possibily there must be some Io Resources allocated to this Bridge. 
						// If so, below code frees those allocated Io Resources.
 						    if(RootBrgData->RbRes[1]->_MIN > 0){
 						       FreeIoStatus = dxe->FreeIoSpace(RootBrgData->RbRes[1]->_MIN, RootBrgData->RbRes[1]->_LEN);
 						    }
 							return MmioStatus;
 						}
 					}
 					//
 					// Manual Option is selected: Don't try our AutoAdjust Algorithm.
 					// 
 					else
 					{
 						return Status;
 					}
#else
 		 	return Status;
#endif //CRB_OUT_OF_RESOURCE_SUPPORT            		 	
 			}


       if((Status == EFI_SUCCESS )&& (AddressLimit < a))
       {
           PCI_TRACE((TRACE_PCI,"PciRB: MMIO Allocation Failed! Could not fit in Root Bridge Allotted MMIO Resources: AllocatedMmioBase=%x, RequiredMmioBase=%x, _LEN=%lX; _GRA=l%X.\n",\
       		                  AddressLimit,\
         		                a,\
           		              res->_LEN, res->_MAX));
           dxe->FreeMemorySpace(AddressLimit, res->_LEN);
           Status = EFI_NOT_FOUND;
 #if BoardPciRes_SUPPORT
                 OutOfResVar.Resource=*res;
                 AmiPciOutOfRes(&OutOfResVar, FALSE);
 #endif

          if (res->_GRA !=32) return Status ; // CRB BIOS is not handling MMIOH OOR Failure       	 
#if CRB_OUT_OF_RESOURCE_SUPPORT
          if (1)//(SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
          {
          	MmioStatus = MmioOutOfResourceConditionOccured(SocketIndex,StackIndex,&LastBootFailedIOH,&IohOutofResInfo,res->_LEN,res->_MAX,MmioTypeIndex);
          	//
          	// System will try OEM DEFINED TABLE if Status is other than Success.
          	// If Success : Best Ratio is calculated
          	if (MmioStatus != EFI_SUCCESS)
          	{
						// Root Bridge Resource Allocation takes place in order of Io Resources followed by Mmio Resources. 
						// If Mmio Resource failure occurs, possibily there must be some Io Resources allocated to this Bridge. 
						// If so, below code frees those allocated Io Resources.
                if(RootBrgData->RbRes[1]->_MIN > 0){
                    FreeIoStatus = dxe->FreeIoSpace(RootBrgData->RbRes[1]->_MIN, RootBrgData->RbRes[1]->_LEN);
                }
 							return MmioStatus;
          	}
          }
          //
          // Manual Option is selected: Don't try our AutoAdjust Algorithm.
          // 
          else
          {
          	return Status;
          }
#else
 		 	return Status;
#endif //CRB_OUT_OF_RESOURCE_SUPPORT            		 	
        }

	    	//
  			// Filling the IohOutofResInfo Structure even for the SUCCESS case
       	//
 			if (Status == EFI_SUCCESS)
 			{
#if CRB_OUT_OF_RESOURCE_SUPPORT 				
					if (1)//(SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
					{
						if(res->_GRA ==32){
							
							IohOutofResInfo.MmioRequested_Stack_Level[SocketIndex][StackIndex] [MmioTypeIndex] = res->_LEN;
							IohOutofResInfo.Mmio_Gra_Stack_Level[SocketIndex][StackIndex][MmioTypeIndex] = res->_MAX;
						}
					}
#endif					
// APTIOV_SERVER_OVERRIDE_START : PCI Resource is set as UC in Reference Code in MemoryCallback.c. Commented the code as there is no need to override the attributes here.
#if 0
 					//Set this region as WT cache if it is PREFETCHABLE
 					if(res->TFlags.MEM_FLAGS._MEM!=ASLRV_MEM_UC)
 					{
 					    PCI_TRACE((TRACE_PCI,"P"));
 							Status=dxe->SetMemorySpaceAttributes(AddressLimit,res->_LEN,EFI_MEMORY_WT);
 							//if attempt to set WT attributes has filed, let's try UC
 							if(EFI_ERROR(Status))
 							{
 									DEBUG((EFI_D_ERROR,"PciHostBridge: Setting of WT attributes for prefetchable memory has failed(%r). UC is used.\n",Status));
 									Status=dxe->SetMemorySpaceAttributes(AddressLimit,res->_LEN,EFI_MEMORY_UC);
 							}
 					}
 					else Status=dxe->SetMemorySpaceAttributes(AddressLimit,res->_LEN,EFI_MEMORY_UC);
 							ASSERT_EFI_ERROR(Status);
 					if(EFI_ERROR(Status)) return Status;
#endif
// APTIOV_SERVER_OVERRIDE_END : PCI Resource is set as UC in Reference Code in MemoryCallback.c. Commented the code as there is no need to override the attributes here.
 			}

#if CRB_OUT_OF_RESOURCE_SUPPORT      
		if (1)//(SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
		{
			MmioTypeIndex = MmioTypeIndex + 1;
		}
#endif		
	}



     	if (Status == EFI_SUCCESS)
 		    	res->_MIN = AddressLimit;

 //		DEBUG ((EFI_D_INFO, "PciRB: #%X Resources: Type=%X; TFlags=%X; GCD asssigned address _MIN=%lX; _MAX=%lX; _GRA=%lX; _LEN=%lX.\n",
 //        RootBrgIndex, res->Type, res->TFlags.TFLAGS,res->_MIN, res->_MAX, res->_GRA, res->_LEN ));
            PCI_TRACE((TRACE_PCI, ": _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX; _GRA=0x%lX\n",
 			res->_MIN,res->_MAX,res->_LEN, res->_GRA ));

 	    //No need to change this and lose Granularity Value... will preserve it.
 		//res->_MAX=res->_MIN+res->_LEN-1;
 	}


 //PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
 //PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//

#if CRB_OUT_OF_RESOURCE_SUPPORT
	//
	// CsiSocketResourceAutoAdjust is enabled in Setup.
	//
	if (1)// (SystemConfiguration.CsiSocketResourceAutoAdjust) // CsiSocketResourceAutoAdjust is enabled in Setup.
	{
//#if OOR_ERROR_DEBUG_MESSAGE		
        PCI_TRACE((TRACE_PCI,":: RootBrgIndex=%x NumberofRb=%x NumberOfIoH=%x\n",RootBrgIndex,NumberOfValidRb,NumberOfIoH));
//#endif		
		if (NumberOfIoH > 1)
		{

			  if (NumberOfValidRb > 1)
			  {
			    NumberOfValidRb = NumberOfValidRb -1 ;
			    return EFI_SUCCESS;// Status
			  }
		}
		
		if ((StackLevelAdjustmentNeeded == FALSE) && (SocketLevelAdjustmentNeeded == FALSE))
		{
		    PCI_TRACE((TRACE_PCI,"System booted Successfully without any OOR Condition\n"));
			return EFI_SUCCESS;
		}

	
	//
	// Check whether both IO and MMIO allocation phase are crossed for all Stack's and CPU's
	// We parsed all IOH and going to check whether OutOfResource Flag for IO/MMIO is set
	// And for which Socket or Stack ?
	
	PCI_TRACE((TRACE_PCI,"PciRB: OutOfResource Condition Occured: \n"));
	
	///AMI OOR Feature Note: Proceed with AMI Out of Resources (OOR) Handling Algorithm only in Single Segment Environment.
	///In Multi-Segment Scenario, System is Halted with 0xD4 Checkpoint Display.
	if( SocketCommonRcConfigDxe.MmcfgSize > 2 ){
		DEBUG((EFI_D_ERROR, "\n\n+-----------------------!!! SYSTEM ALERT !!!-----------------------------+\n"));
		DEBUG((EFI_D_ERROR, "|                      PCI Multi-Segment is Enabled                      |\n"));
		DEBUG((EFI_D_ERROR, "| System does not handle Out of Resources in Multi-Segment Environment   |\n"));
		DEBUG((EFI_D_ERROR, "+------------------------ SYSTEM is HALTED ------------------------------+\n"));
        ERROR_CODE(DXE_PCI_BUS_OUT_OF_RESOURCES,EFI_ERROR_MAJOR);
		EFI_DEADLOOP();
	}
	
	// ####################################################################################################################
	// ########################### FIND and FILL SOCKET/STACK LEVEL Failure INFORMATION START #############################
	// ####################################################################################################################
	
	//
	// STEP 1 : Loop through all IIO stacks of all sockets that are present and find the Failing Stack and Sockets.
	//
	
	
	Stack_Mmio_SocketNumber = 0 ;
	Stack_Io_SocketNumber = 0 ;
	
	Socket_Mmio_SocketNumber = 0 ;
	Socket_Io_SocketNumber = 0 ;
	
	for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) 
	{
		if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) 
		{
			continue;
		}
		
		IOHUsedDuringSocketMMIOAdjustment [SocketIndex] = 0xFF;
		IOHUsedDuringSocketIOAdjustment[SocketIndex] = 0xFF;
		for (StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) 
		{
			if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) 
			{
				continue;
			}

			if (IohOutofResInfo.MmmioOutofResrcFlag_Stack_Level [SocketIndex][StackIndex])
			{
				Stack_MMIO_Failed_Socket [Stack_Mmio_SocketNumber] = SocketIndex;
				Stack_Mmio_SocketNumber ++ ;
	            // If Stack Failure happens Enable Flag for Failure for respective Stack in LastBootFailedIOh 
	            LastBootFailedIOH.MmmioOutofResrcFlag[SocketIndex][StackIndex]=1;
				PCI_TRACE((TRACE_PCI,"\n STACKLEVEL : MMIO OUTOFRESOURCE OCCURED : SocketIndex=%d  StackIndex=%d\n", SocketIndex, StackIndex )); 
			}
			
			if (IohOutofResInfo.IoOutofResrcFlag_Stack_Level [SocketIndex][StackIndex])
			{
				Stack_IO_Failed_Socket [Stack_Io_SocketNumber] = SocketIndex;
				Stack_Io_SocketNumber++;
                // If Stack Failure happens Enable Flag for Failure for respective Stack in LastBootFailedIOh 
                LastBootFailedIOH.IoOutofResrcFlag[SocketIndex][StackIndex]=1;
				PCI_TRACE((TRACE_PCI,"\n STACKLEVEL : IO OUTOFRESOURCE OCCURED : SocketIndex=%d  StackIndex=%d\n", SocketIndex, StackIndex )); 
			}
		    	
		} // for loop for Stack
		  
		if (IohOutofResInfo.MmmioOutofResrcFlag [SocketIndex])
		{
			Socket_MMIO_Failed_Socket[Socket_Mmio_SocketNumber] = SocketIndex;
			// If Socket Failure happens Enable Flag for Failure on all Stacks of failing Socket in LastBootFailedIOh 
			for (FailedStackIndex = 0; FailedStackIndex < MAX_IIO_STACK; FailedStackIndex++) {
			    LastBootFailedIOH.MmmioOutofResrcFlag[SocketIndex][FailedStackIndex]=1;
			}
			
			Socket_Mmio_SocketNumber++;
			
			PCI_TRACE((TRACE_PCI,"\n SOCKET LEVEL : MMIO OUTOFRESOURCE OCCURED : SocketIndex=%d \n", SocketIndex )); 
		}
		
		if (IohOutofResInfo.IoOutofResrcFlag [SocketIndex])
		{
		    Socket_IO_Failed_Socket[Socket_Io_SocketNumber] = SocketIndex;
			// If Socket Failure happens Enable Flag for Failure on all Stacks of failing Socket in LastBootFailedIOh 
            for (FailedStackIndex = 0; FailedStackIndex < MAX_IIO_STACK; FailedStackIndex++) {
                LastBootFailedIOH.IoOutofResrcFlag[SocketIndex][FailedStackIndex]=1;
            }
		  	Socket_Io_SocketNumber++;
			  PCI_TRACE((TRACE_PCI,"\n SOCKET LEVEL : IO OUTOFRESOURCE OCCURED : SocketIndex: %x \n ", SocketIndex )); 
		}
		
	} // for loop for Socket
	
	// ##################################################################################################################
	// ########################### FIND and FILL SOCKET/STACK LEVEL Failure INFORMATION END #############################
	// ##################################################################################################################

	
	//
	// STEP 2 : Handle the Socket Failure First.
	//
	
	// ##################################################################################################################
	// ########################### SOCKET LEVEL Failure START ###########################################################
	// ##################################################################################################################

	if (SocketLevelAdjustmentNeeded == TRUE)
	{
		for (FailedSocketNumber = 0 ; FailedSocketNumber < NbGetMaximumIohSockets() ; FailedSocketNumber++)
		{
		
			if (Socket_MMIO_Failed_Socket[FailedSocketNumber] != 0xFF)
			{
			    PCI_TRACE((TRACE_PCI, "\n Socket Level MMIO OutOfResource Condition Occured !\n"));
			    OutOfResourceFailure = MMIOFAILURE;
				if (SocketAutoAdjustRatios(OutOfResourceFailure))
				{
					DEBUG ((EFI_D_INFO, "Socket Level MMIO OOR: Requested MMIO space is not possible!  System is trying OOR Not-Possible Handling Policy\n"));
					#if OOR_NOT_POSSIBLE_HANDLING_POLICY
				    PCI_TRACE((TRACE_PCI,"\n\nSystem Encountered Out Of Resources Not-Possible Condition\n\n")); 
					CrbLowerMmCfgBase(&LastBootFailedIOH, &SocketCommonRcConfigDxe);
					#endif
					// Filling the Failed IOH details in the LastBootFailedIOH Structure
    				for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++) {
		    			for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
							DEBUG((EFI_D_INFO, "LastBootFailedIoh Socket:%d Stack:%d MmmioOutofResrcFlag : 0x%x\n",IndexSocket,IndexStack,LastBootFailedIOH.MmmioOutofResrcFlag[IndexSocket][IndexStack] ));
    		    		}
		    		}

					GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
		                                   &LastBootFailedIohGuid,
		                                   EFI_VARIABLE_NON_VOLATILE |
		                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
		                                   VariableSize,
		                                   &LastBootFailedIOH );

					if(EFI_ERROR(GetSetStatus)) {
						DEBUG((EFI_D_ERROR, "Socket MMIO : SetVariable : LastBootFailedIOH structure Failed\n"));
						DEBUG ((EFI_D_ERROR, "\nSystem Halted!!!!!!!!!!!!\n"));
						EFI_DEADLOOP(); 
					}
					pRS->ResetSystem( EfiResetCold, 0, 0, NULL );
									
				}
			} // Socket_MmmioOutofResrcFlag End
						
			if (Socket_IO_Failed_Socket[FailedSocketNumber] != 0xFF)
			{
				OutOfResourceFailure = IOFAILURE;
				PCI_TRACE((TRACE_PCI, "\n Socket Level IO OutOfResource Condition Occured !\n"));
				if (SocketAutoAdjustRatios(OutOfResourceFailure))
				{
					DEBUG ((EFI_D_INFO, "Socket Level IO OOR: Requested IO space is not possible!  System is trying OOR Not-Possible Handling Policy\n"));
					// Filling the Failed IOH details in the LastBootFailedIOH Structure
					for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++){
    					for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
							DEBUG((EFI_D_INFO, "LastBootFailedIoh Socket:%d Stack:%d IoOutofResrcFlag : 0x%x\n",IndexSocket,IndexStack,LastBootFailedIOH.IoOutofResrcFlag[IndexSocket][IndexStack] ));
    	    			}
    				}
					GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
		                                   &LastBootFailedIohGuid,
		                                   EFI_VARIABLE_NON_VOLATILE |
		                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
		                                   VariableSize,
		                                   &LastBootFailedIOH );

					if(EFI_ERROR(GetSetStatus)) {
						DEBUG ((EFI_D_ERROR, "Socket IO : SetVariable : LastBootFailedIOH structure Failed\n"));
						DEBUG ((EFI_D_ERROR, "\nSystem Halted!!!!!!!!!!!!\n"));
						EFI_DEADLOOP(); 
					}
					pRS->ResetSystem( EfiResetCold, 0, 0, NULL );
								
				}
			}  //  Socket_IoOutofResrcFlag End
		}	// FailedSocketNumber  End
			
		
					
	} // SocketLevelAdjustmentNeeded End
	
	// ##################################################################################################################
	// ########################### SOCKET LEVEL Failure END #############################################################
	// ##################################################################################################################
	
	//
	// STEP 3 : Handle the STACK Failure. If it requires SOCKET LEVEL Handling, it will also be taken care.
	//
	
	// ##################################################################################################################
	// ########################### STACK LEVEL Failure START ############################################################
	// ##################################################################################################################
	
	if ((StackLevelAdjustmentNeeded == TRUE))
	{
		for (FailedSocketNumber = 0 ; FailedSocketNumber < NbGetMaximumIohSockets() ; FailedSocketNumber++)
		{
			if (Stack_MMIO_Failed_Socket [FailedSocketNumber] != 0xFF)
			{
			  
			  
			  if (StackAdjustedDuringSocketAdjustment (Stack_MMIO_Failed_Socket [FailedSocketNumber],MMIOFAILURE) == TRUE)
			  {
			    PCI_TRACE ((TRACE_PCI, "\n MMIO : SocketLevel code must have adjusted the Stack Failure !\n"));
			    PCI_TRACE ((TRACE_PCI, "\n MMIO::: Stack_MMIO_Failed_Socket [%d] IOHUsedDuringSocketMMIOAdjustment=%d \n",Stack_MMIO_Failed_Socket [FailedSocketNumber],IOHUsedDuringSocketMMIOAdjustment[FailedSocketNumber]));
			    goto IO_Check;
			  }
			  
			  PCI_TRACE ((TRACE_PCI, "\n Stack Level MMIO OutOfResource Condition Occured !\n"));
				OutOfResourceFailure = MMIOFAILURE;
				if (StackAutoAdjustRatios(OutOfResourceFailure,Stack_MMIO_Failed_Socket [FailedSocketNumber],Stack_IO_Failed_Socket [FailedSocketNumber]))
				{
					          DEBUG ((EFI_D_INFO, "Stack Level MMMIO OOR: Requested MMIO space is not possible!  System is trying OOR Not-Possible Handling Policy\n"));
                    #if OOR_NOT_POSSIBLE_HANDLING_POLICY
                    PCI_TRACE((TRACE_PCI,"\n\nSystem Encountered Out Of Resources Not-Possible Condition\n\n")); 
                    CrbLowerMmCfgBase(&LastBootFailedIOH,&SocketCommonRcConfigDxe);
                    #endif
					// Filling the Failed IOH details in the LastBootFailedIOH Structure
    				for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++){
		    			for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
							DEBUG((EFI_D_INFO, "LastBootFailedIoh Socket:%d Stack:%d MmmioOutofResrcFlag : 0x%x\n",IndexSocket,IndexStack,LastBootFailedIOH.MmmioOutofResrcFlag[IndexSocket][IndexStack] ));
    		    		}
    				}
			
					GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
		                                   &LastBootFailedIohGuid,
		                                   EFI_VARIABLE_NON_VOLATILE |
		                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
		                                   VariableSize,
		                                   &LastBootFailedIOH );

					if(EFI_ERROR(GetSetStatus)) {
						DEBUG((EFI_D_ERROR, "Stack MMIO : SetVariable : LastBootFailedIOH structure Failed\n"));
						DEBUG ((EFI_D_ERROR, "\nSystem Halted!!!!!!!!!!!!\n"));
						EFI_DEADLOOP(); 
					}
					pRS->ResetSystem( EfiResetCold, 0, 0, NULL );
									
				}
			
				// ########################### Sometimes while Adjusting Stack LEVEL with new Ratio's #############################
				// ########################### we may see Socket Failure #############################			
				// ########################### SOCKET LEVEL Failure #############################
				if (AfterStackAdjust_SocketMMIO_AdjustNeeded)
				{
					PCI_TRACE ((TRACE_PCI, "\n Stack Level MMIO Not possible. Will try for Socket Level Adjustment !\n"));
					if (SocketAutoAdjustRatios(OutOfResourceFailure))
					{
						DEBUG ((EFI_D_INFO, "AfterStackAdjust_SocketMMIO_AdjustNeeded: Requested MMIO space is not possible!  System is trying OOR Not-Possible Handling Policy\n"));
                        #if OOR_NOT_POSSIBLE_HANDLING_POLICY
	                    PCI_TRACE((TRACE_PCI,"\n\nSystem Encountered Out Of Resources Not-Possible Condition\n\n")); 
                        CrbLowerMmCfgBase(&LastBootFailedIOH,&SocketCommonRcConfigDxe);
                        #endif
						// Filling the Failed IOH details in the LastBootFailedIOH Structure
			    		for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++){
			    			for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
								DEBUG((EFI_D_INFO, "LastBootFailedIoh Socket:%d Stack:%d MmmioOutofResrcFlag : 0x%x\n",IndexSocket,IndexStack,LastBootFailedIOH.MmmioOutofResrcFlag[IndexSocket][IndexStack] ));
    			    		}
    					}
						GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
		                                   &LastBootFailedIohGuid,
		                                   EFI_VARIABLE_NON_VOLATILE |
		                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
		                                   VariableSize,
		                                   &LastBootFailedIOH );

						if(EFI_ERROR(GetSetStatus)) {
							DEBUG((EFI_D_ERROR, "Stack MMIO : SetVariable : LastBootFailedIOH structure Failed\n"));
							DEBUG ((EFI_D_ERROR, "\nSystem Halted!!!!!!!!!!!!\n"));
							EFI_DEADLOOP(); 
						}
						pRS->ResetSystem( EfiResetCold, 0, 0, NULL );
													
					}
					AfterStackAdjust_SocketMMIO_AdjustNeeded = FALSE;
				}
			} // Stack_MmmioOutofResrcFlag End
IO_Check:			
			if (Stack_IO_Failed_Socket [FailedSocketNumber] != 0xFF)
			{
			  if (StackAdjustedDuringSocketAdjustment (Stack_IO_Failed_Socket [FailedSocketNumber],IOFAILURE) == TRUE )
			  {
			      PCI_TRACE ((TRACE_PCI, "\n IO : SocketLevel code must have adjusted the Stack Failure !\n"));
			      PCI_TRACE ((TRACE_PCI, "\n IO::: Stack_IO_Failed_Socket [%d] IOHUsedDuringSocketIOAdjustment=%d \n",Stack_IO_Failed_Socket [FailedSocketNumber],IOHUsedDuringSocketIOAdjustment[FailedSocketNumber]));
			      continue;
			  }
				OutOfResourceFailure = IOFAILURE;
				PCI_TRACE ((TRACE_PCI, "\n Stack Level IO OutOfResource Condition Occured !\n"));
				if (StackAutoAdjustRatios(OutOfResourceFailure,Stack_MMIO_Failed_Socket [FailedSocketNumber],Stack_IO_Failed_Socket [FailedSocketNumber]))
				{
					DEBUG ((EFI_D_INFO, "Stack Level IO OOR: Requested IO space is not possible!  System is trying OOR Not-Possible Handling Policy\n"));
					// Filling the Failed IOH details in the LastBootFailedIOH Structure
		    		for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++){
		    			for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
							DEBUG((EFI_D_INFO, "LastBootFailedIoh Socket:%d Stack:%d IoOutofResrcFlag : 0x%x\n",IndexSocket,IndexStack,LastBootFailedIOH.IoOutofResrcFlag[IndexSocket][IndexStack] ));
    		    		}
    				}
					GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
		                                   &LastBootFailedIohGuid,
		                                   EFI_VARIABLE_NON_VOLATILE |
		                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
		                                   VariableSize,
		                                   &LastBootFailedIOH );
					if(EFI_ERROR(GetSetStatus)) {
						DEBUG((EFI_D_ERROR, "Stack IO : SetVariable : LastBootFailedIOH structure Failed\n"));
						DEBUG ((EFI_D_ERROR, "\nSystem Halted!!!!!!!!!!!!\n"));
						EFI_DEADLOOP(); 
					}
					pRS->ResetSystem( EfiResetCold, 0, 0, NULL );
					
				}
				
				// ########################### Sometimes while Adjusting Stack LEVEL with new Ratio's #############################
				// ########################### we may see Socket Failure #############################			
				// ########################### SOCKET LEVEL Failure #############################
				if (AfterStackAdjust_SocketIO_AdjustNeeded)
				{
					PCI_TRACE ((TRACE_PCI, "\n Stack Level IO Not possible. Will try for Socket Level Adjustment !\n"));
					if (SocketAutoAdjustRatios(OutOfResourceFailure))
					{
						DEBUG ((EFI_D_INFO, "AfterStackAdjust_SocketIO_AdjustNeeded: Requested IO space is not possible!  System is trying OOR Not-Possible Handling Policy\n"));
					// Filling the Failed IOH details in the LastBootFailedIOH Structure

    				for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++) {
		    			for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
							DEBUG((EFI_D_INFO, "LastBootFailedIoh Socket:%d Stack:%d IoOutofResrcFlag :0x%x\n",IndexSocket,IndexStack,LastBootFailedIOH.IoOutofResrcFlag[IndexSocket][IndexStack] ));
    		    		}
		    		}
			
					GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
		                                   &LastBootFailedIohGuid,
		                                   EFI_VARIABLE_NON_VOLATILE |
		                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
		                                   VariableSize,
		                                   &LastBootFailedIOH );
					if(EFI_ERROR(GetSetStatus)) {
						DEBUG((EFI_D_ERROR, "Stack IO : SetVariable : LastBootFailedIOH structure Failed\n"));
						DEBUG ((EFI_D_ERROR, "\nSystem Halted!!!!!!!!!!!!\n"));
						EFI_DEADLOOP(); 
					}
						pRS->ResetSystem( EfiResetCold, 0, 0, NULL );
																	
					}
					
					AfterStackAdjust_SocketIO_AdjustNeeded = FALSE;
				}
			} // Stack_IoOutofResrcFlag End
		} // FailedSocketNumber End 
			
	 } // StackLevelAdjustmentNeeded End
		
	
	// ##################################################################################################################
	// ########################### STACK LEVEL Failure END ##############################################################
	// ##################################################################################################################
	
	//
	// STEP 4 : Update the newly calculated Ratio in SOCKET_PCI_RESOURCE_CONFIGURATION_DATA_NAME which will be used in RC
	//          in Next boot.
		UpdateSocketPciRescVariable();
		DEBUG ((EFI_D_INFO, "\n Ratio's Adjusted. Clearing LastBootFailedIoh Variable\n"));
    	for (IndexSocket = 0 ; IndexSocket < MAX_SOCKET ; IndexSocket++) {
    		for (IndexStack = 0; IndexStack < MAX_IIO_STACK; IndexStack++) {
				LastBootFailedIOH.IoOutofResrcFlag[IndexSocket][IndexStack] =  0;
				LastBootFailedIOH.MmmioOutofResrcFlag [IndexSocket][IndexStack] = 0;
    	    }
    	}
		GetSetStatus = pRS->SetVariable (L"LastBootFailedIoh",
	                                   &LastBootFailedIohGuid,
	                                   EFI_VARIABLE_NON_VOLATILE |
	                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
	                                   VariableSize,
	                                   &LastBootFailedIOH );

		if(EFI_ERROR(GetSetStatus)) {
			DEBUG((EFI_D_ERROR, "IO Allocation Failed: SetVariable : LastBootFailedIOH structure Failed\n"));
			ASSERT_EFI_ERROR(GetSetStatus);
		}
		PCI_TRACE ((TRACE_PCI, "\n Ratio's Adjusted and System is going to Reset !\n"));
		pRS->ResetSystem( EfiResetCold, 0, 0, NULL );
	
	}
#endif
// APTIOV_SERVER_OVERRIDE_END
    return Status;
}
// APTIOV_SERVER_OVERRIDE_START
// ########################### Update MRC paramters with new Stack  or Socket Ratios #############################
// ########################### Update MRC paramters with new Stack  or Socket Ratios #############################
// ########################### Update MRC paramters with new Stack  or Socket Ratios #############################
#if CRB_OUT_OF_RESOURCE_SUPPORT
VOID UpdateSocketPciRescVariable (
)
{
	
	EFI_STATUS			Status = EFI_SUCCESS;
	UINTN 				  SocketIndex = 0;
	UINTN 				  StackIndex = 0 ;
	UINTN           TemporaryStackIndex = 0 ;
	UINTN           FirstValidStack;
	UINT32					PciSocketMmiolLimit;
	UINT32					PciSocketIoLimit;
	UINT32          SocketVariableAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
	UINT32					PciResourceIoLimit;
	UINT8						ValidMMIOStackIndex = 0,ValidIOStackIndex = 0;
	UINT32          LastValidPciSocketMmiolLimit;
	SOCKET_PCI_RESOURCE_CONFIGURATION_DATA  SocketPciResourceConfiguration;
	UINTN           		SocketVariableSize = sizeof(SOCKET_PCI_RESOURCE_CONFIGURATION_DATA);
	

		
	ASSERT_EFI_ERROR(Status);
	
	ZeroMem (SocketPciResourceConfiguration.PciSocketIoBase, (sizeof (UINT16)*MAX_SOCKET));
	ZeroMem (SocketPciResourceConfiguration.PciSocketIoLimit, (sizeof (UINT16)*MAX_SOCKET));
	ZeroMem (SocketPciResourceConfiguration.PciSocketMmiolBase, (sizeof (UINT32)*MAX_SOCKET));
	ZeroMem (SocketPciResourceConfiguration.PciSocketMmiolLimit, (sizeof (UINT32)*MAX_SOCKET));
	
	ZeroMem (SocketPciResourceConfiguration.PciSocketMmiohBase, (sizeof (UINT64)*MAX_SOCKET));
	ZeroMem (SocketPciResourceConfiguration.PciSocketMmiohLimit, (sizeof (UINT64)*MAX_SOCKET));
	ZeroMem (SocketPciResourceConfiguration.PciResourceIoBase, (sizeof (UINT16)*TOTAL_IIO_STACKS));
	ZeroMem (SocketPciResourceConfiguration.PciResourceIoLimit, (sizeof (UINT16)*TOTAL_IIO_STACKS));
	ZeroMem (SocketPciResourceConfiguration.PciResourceMem32Base, (sizeof (UINT32)*TOTAL_IIO_STACKS));
	ZeroMem (SocketPciResourceConfiguration.PciResourceMem32Limit, (sizeof (UINT32)*TOTAL_IIO_STACKS));

	ZeroMem (SocketPciResourceConfiguration.PciResourceMem64Base, (sizeof (UINT64)*TOTAL_IIO_STACKS));
  ZeroMem (SocketPciResourceConfiguration.PciResourceMem64Limit, (sizeof (UINT64)*TOTAL_IIO_STACKS));
		  
	for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
	if (!gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].Valid) {
		continue;
	}
		
	PciSocketMmiolLimit = 0;
	PciSocketIoLimit = 0;
	for (StackIndex = 0  ; StackIndex < MAX_IIO_STACK; StackIndex++) {
	if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << StackIndex))) {
			continue;
	}
	          
	PCI_TRACE((TRACE_PCI,"\n SocketIndex=%d  StackIndex=%d", SocketIndex, StackIndex + (SocketIndex*MAX_IIO_STACK) ));
	PCI_TRACE((TRACE_PCI,"\n After AutoAdjust: IORatio=%lx MMIORatio=%lx",AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketIndex][StackIndex]* SAD_LEG_IO_GRANTY_2KB,AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex][StackIndex]* SAD_MMIOL_GRANTY_4MB ));

	//
	// Stack 0 and Socket 0
	//
	if (StackIndex == 0 && SocketIndex == 0)
	{
		SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)] = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].StackRes[StackIndex].PciResourceMem32Base;
		SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)]+ (AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex][StackIndex]* SAD_MMIOL_GRANTY_4MB);
		SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] -1;
	    		
		SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)] = 0;
		SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)]+ (AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketIndex][StackIndex]* SAD_LEG_IO_GRANTY_2KB);
		SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)] -1;
    		
 		ValidIOStackIndex = (UINT8)(StackIndex + (SocketIndex*MAX_IIO_STACK));
 		ValidMMIOStackIndex = (UINT8)(StackIndex + (SocketIndex*MAX_IIO_STACK));
	}
	         
	else
	{
		//
		// IO : Check Any resource are taken from Stack or is it disabled during current boot
		//
	  if (((AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketIndex][StackIndex]) == 0 && (IohOutofResInfo.Stack_Current_Boot_IORatio[SocketIndex][StackIndex]) > 0 ) ||
	   	 (AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketIndex][StackIndex] == 0 && IohOutofResInfo.Stack_Current_Boot_IORatio[SocketIndex][StackIndex] == 0))
	  {
		  SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)] = 0xFFFF;
		  SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = 0;
	    			 	
	  }
	    		 
	  else
	  {
	  	// PciResourceIoLimit & PciResourceIoBase are UINT16.
	    // Last Stack IOL : FFFF + 1 which won't fit in UINT16.
	    			   	    			 	
		  SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)] = 1 + SocketPciResourceConfiguration.PciResourceIoLimit[ValidIOStackIndex];
	    PciResourceIoLimit = SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)] + (AutoAdjustMmioIoVariable.IoRatio_Stack_Level[SocketIndex][StackIndex]* SAD_LEG_IO_GRANTY_2KB);
   		PciResourceIoLimit = PciResourceIoLimit - 1;

   		SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = ((UINT16)PciResourceIoLimit) ;
	    ValidIOStackIndex = (UINT8)(StackIndex + (SocketIndex*MAX_IIO_STACK));
	   }
	  
	  //
	  // MMIO : Check Any resource are taken from Stack or is it disabled during current boot
	  //
	  if (((AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex][StackIndex]) == 0 && (IohOutofResInfo.Stack_Current_Boot_MMIORatio[SocketIndex][StackIndex]) > 0 ) ||
	     (AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex][StackIndex] == 0 && IohOutofResInfo.Stack_Current_Boot_MMIORatio[SocketIndex][StackIndex] == 0))
	   {
		  SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)] = 0xFFFFFFFF;
		  SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = 0;
	    		 	  
	   }
	    		 	    		 
	   else
	   {
		   SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)] = 1 + SocketPciResourceConfiguration.PciResourceMem32Limit[ValidMMIOStackIndex];
		   SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)] + (AutoAdjustMmioIoVariable.MmiolRatio_Stack_Level[SocketIndex][StackIndex]* SAD_MMIOL_GRANTY_4MB);
		   SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] = SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] -1 ;
	     ValidMMIOStackIndex = (UINT8)(StackIndex + (SocketIndex*MAX_IIO_STACK));
	    		 	    			 	 
	   }
	    			 
	}	
	
	//
	// No resource alloted.
	//
	if ((SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)]) < (SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)])) 
	{
		PciSocketMmiolLimit = PciSocketMmiolLimit + 1+ (SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] - SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)]);
	}
	    	
	if ((SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)]) < (SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)])) 
	{
		PciSocketIoLimit = PciSocketIoLimit + 1+ (SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)] - SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)]);
	}
	    	
	PCI_TRACE((TRACE_PCI,"\n After AutoAdjust : Socket=%d Stack=%d  PciResourceMem32Base=%lx PciResourceMem32Limit=%lx \n", SocketIndex,StackIndex + (SocketIndex*MAX_IIO_STACK),SocketPciResourceConfiguration.PciResourceMem32Base[StackIndex + (SocketIndex*MAX_IIO_STACK)], SocketPciResourceConfiguration.PciResourceMem32Limit[StackIndex + (SocketIndex*MAX_IIO_STACK)] ));
	PCI_TRACE((TRACE_PCI,"\n After AutoAdjust : Socket=%d Stack=%d PciResourceIoBase=%lx PciResourceIoLimit=%lx \n", SocketIndex,StackIndex + (SocketIndex*MAX_IIO_STACK),SocketPciResourceConfiguration.PciResourceIoBase[StackIndex + (SocketIndex*MAX_IIO_STACK)], SocketPciResourceConfiguration.PciResourceIoLimit[StackIndex + (SocketIndex*MAX_IIO_STACK)]));
  
	} // for loop for Stack
	    
	if (SocketIndex == 0)
	{
	//  Matching as like RC.SocketPciResourceConfiguration.PciSocketMmiolBase[SocketIndex] = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].PciResourceMem32Base;
		SocketPciResourceConfiguration.PciSocketMmiolBase[SocketIndex] = gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].PciResourceMem32Base;
		SocketPciResourceConfiguration.PciSocketMmiolLimit[SocketIndex] = SocketPciResourceConfiguration.PciSocketMmiolBase[SocketIndex] + PciSocketMmiolLimit -1;
			
		SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] = 0; //gIioUds->IioUdsPtr->PlatformData.IIO_resource[SocketIndex].PciResourceIoBase;
		SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex] = SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] + PciSocketIoLimit -1;
	  
		LastValidPciSocketMmiolLimit = SocketPciResourceConfiguration.PciSocketMmiolLimit[SocketIndex];
	}
	   	         
else
      {
            SocketPciResourceConfiguration.PciSocketMmiolBase[SocketIndex] = 1 + LastValidPciSocketMmiolLimit;
            SocketPciResourceConfiguration.PciSocketMmiolLimit[SocketIndex] = SocketPciResourceConfiguration.PciSocketMmiolBase[SocketIndex] + PciSocketMmiolLimit -1 ;
            LastValidPciSocketMmiolLimit = SocketPciResourceConfiguration.PciSocketMmiolLimit[SocketIndex];
            FirstValidStack = 0xFF ;
            for (TemporaryStackIndex = 0  ; TemporaryStackIndex < MAX_IIO_STACK; TemporaryStackIndex++) 
            {
              if (!((gIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SocketIndex].stackPresentBitmap) & (1 << TemporaryStackIndex))) {
                  continue;
              }
              
              if (SocketPciResourceConfiguration.PciResourceIoBase[TemporaryStackIndex + (SocketIndex*MAX_IIO_STACK)] == 0xFFFF &&
                  SocketPciResourceConfiguration.PciResourceIoLimit[TemporaryStackIndex + (SocketIndex*MAX_IIO_STACK)]== 0x0)
              {
                  if (TemporaryStackIndex == NumberofStacksPerCpu -1 && FirstValidStack == 0xFF) 
                  {
                       SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] = 0xFFFF;
                       SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex]= 0;
                  }
                  continue;
              }
              else
              {
                
                if (FirstValidStack == 0xFF) // Only one time update SocketIO base.
                {
                  FirstValidStack = TemporaryStackIndex;
                  SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] = SocketPciResourceConfiguration.PciResourceIoBase[FirstValidStack + (SocketIndex*MAX_IIO_STACK)];
                }
                SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex] = SocketPciResourceConfiguration.PciResourceIoLimit[TemporaryStackIndex + (SocketIndex*MAX_IIO_STACK)];
                
              }
              
              if (TemporaryStackIndex == NumberofStacksPerCpu -1 && 
                  SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] == 0 && 
                  SocketPciResourceConfiguration.PciResourceIoLimit[SocketIndex] == 0)
              {
                 SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] = 0xFFFF;
                 SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex]= 0;
              }
            }
/*            
            if (SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex-1] == 0xFFFF ||
                SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex-1] == 0xFFFF) // No IO Resource allotted
            {
                  SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] = 0xFFFF;
                  SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex] = 0 ;

            }
            else
            {
                  SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] = 1 + SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex-1];
                  SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex] = SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex] + PciSocketIoLimit -1 ;
            }
            */

  } 

	   	         
  PCI_TRACE((TRACE_PCI,"\n After AutoAdjust : Socket=%d PciSocketMmiolBase=%lx PciSocketMmiolLimit=%lx PciSocketMmiolLimit=%lx\n", SocketIndex,SocketPciResourceConfiguration.PciSocketMmiolBase[SocketIndex], SocketPciResourceConfiguration.PciSocketMmiolLimit[SocketIndex],PciSocketMmiolLimit ));
  PCI_TRACE((TRACE_PCI,"\n After AutoAdjust : Socket=%d PciSocketIoBase=%lx PciSocketIoLimit=%lx PciSocketIoLimit=%lx\n", SocketIndex,SocketPciResourceConfiguration.PciSocketIoBase[SocketIndex], SocketPciResourceConfiguration.PciSocketIoLimit[SocketIndex],PciSocketIoLimit ));
   
	} // for loop for Socket 

	Status = pRS->SetVariable (
															SOCKET_PCI_RESOURCE_CONFIGURATION_DATA_NAME,
	                            &gEfiSocketPciResourceDataGuid,
	                            SocketVariableAttr,
	                            SocketVariableSize,
	                            &SocketPciResourceConfiguration
	                            );
	
	if (EFI_ERROR(Status))
	{
		 DEBUG((EFI_D_ERROR, "\n AmiPciAccessCspBaseLib.c SOCKET_PCI_RESOURCE_CONFIGURATION_DATA_NAME : Setvariable failed\n" ));
		 EFI_DEADLOOP();
	}
	
	ASSERT_EFI_ERROR(Status);
	
}
#endif
// APTIOV_SERVER_OVERRIDE_END

//----------------------------------------------------------------------------
// Following functions are ROOT BRIDGE Infrastructure Overrides and Porting.
//----------------------------------------------------------------------------

/**
    Chipset Specific function to Map Internal Device address
    residing ABOVE 4G to the BELOW 4G address space for DMA.
    MUST BE IMPLEMENTED if CHIPSET supports address space
    decoding ABOVE 4G.

    @param RbData Root Bridge private structure data
    @param Operation Operation to provide Mapping for
    @param HostAddress HostAddress of the Device
    @param NumberOfBytes Number of Byte in Mapped Buffer.
    @param DeviceAddress Mapped Device Address.
    @param Mapping Mapping Info Structure this function must
        allocate and fill.

        EFI_STATUS
    @retval EFI_SUCCESS Successful.
    @retval EFI_UNSUPPORTED The Map function is not supported.
    @retval EFI_INVALID_PARAMETER One of the parameters has an
        invalid value.

    @note  Porting is required for chipsets that supports Decoding
              of the PCI Address Space ABOVE 4G.
**/
EFI_STATUS RbCspIoPciMap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
    IN EFI_PHYSICAL_ADDRESS                         HostAddress,
    IN OUT UINTN                                    *NumberOfBytes,
    OUT EFI_PHYSICAL_ADDRESS                        *DeviceAddress,
    OUT VOID                                        **Mapping )
{

    EFI_STATUS  Status = EFI_UNSUPPORTED;

    // Any Additional Variables goes here

    *Mapping = NULL;

    // For Chipsets which DOES support decoding of the PCI resources ABOVE 4G.
	// But CPU architecture DOES NOT not (like 32bit mode)
    // here must be something like that.

//###DEBUG
//###     PCI_ROOT_BRIDGE_MAPPING *mapping;
//###     //------------------------------
//###         // Common buffer operations can not be remapped because in such
//###         // operations the same buffer will be accessed by CPU and PCI hardware
//###         if ( (Operation == EfiPciOperationBusMasterCommonBuffer) || 
//###              (Operation == EfiPciOperationBusMasterCommonBuffer64) )
//###             return EFI_UNSUPPORTED;
//###
//###         mapping = Malloc(sizeof(PCI_ROOT_BRIDGE_MAPPING));
//###         if (mapping == NULL) return EFI_OUT_OF_RESOURCES;
//###
//###         mapping->Signature  = EFI_PCI_RB_MAPPING_SIGNATURE;
//###         mapping->Resrved    = 0;
//###         mapping->Operation  = Operation;
//###         mapping->NumPages   = EFI_SIZE_TO_PAGES(*NumberOfBytes);
//###         mapping->HostAddr   = HostAddress;
//###         mapping->DeviceAddr = 0x00000000ffffffff;
//###
//###         Status = pBS->AllocatePages( AllocateMaxAddress, 
//###                                      EfiBootServicesData, 
//###                                      mapping->NumPages, 
//###                                      &mapping->DeviceAddr );
//###         if (EFI_ERROR(Status)) {
//###             pBS->FreePool(mapping);
//###             return Status;
//###         }
//###
//###         *Mapping = (VOID*)mapping;
//###
//###         // Here must be a way to copy context of HostDevice buffer to the
//###         // Mapped one.
//###         // This code given as example only you might need to do some chipset
//###         // programming to access PCI Address Space Above 4G
//###
//###         if ( (Operation == EfiPciOperationBusMasterRead) || 
//###              (Operation == EfiPciOperationBusMasterRead64) )
//###             pBS->CopyMem( (VOID*)(UINTN)mapping->DeviceAddr, 
//###                           (VOID*)(UINTN)mapping->HostAddr, 
//###                           mapping->NumBytes );
//###
//###         *DeviceAddress = mapping->DeviceAddr;
//### DEBUG END

    return Status;
}


/**
    Chipset Specific function to Unmap previousely Mapped
    buffer for DMA.
    MUST BE IMPLEMENTED if CHIPSET supports address space
    decoding ABOVE 4G.

    @param RbData Root Bridge private structure data
    @param Mapping Mapping Info Structure this function must free.

        EFI_STATUS
    @retval EFI_SUCCESS Successful.
    @retval EFI_UNSUPPORTED The Unmap function is not supported.
    @retval EFI_INVALID_PARAMETER One of the parameters has an
        invalid value.

    @note  Porting required if needed.
**/
EFI_STATUS RbCspIoPciUnmap (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    OUT PCI_ROOT_BRIDGE_MAPPING                     *Mapping )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here


    if (Mapping != NULL) return EFI_INVALID_PARAMETER;
    // for all other conditions we would return EFI_UNSUPPORTED.
    Status = EFI_UNSUPPORTED;

    // for Chipsets which DOES support decoding of the PCI resources ABOVE 4G.
	// But CPU architecture DOES NOT not (like 32bit mode)
    // And provides corresponded mapping for the host address
    // here must be something like that.
//####  DEBUG
//####      if (Mapping->Signature != EFI_PCI_RB_MAPPING_SIGNATURE)
//####          Status = EFI_INVALID_PARAMERTER;
//####
//####      if (!EFI_ERROR(Status)) {
//####
//####          if ( (Mapping->Operation == EfiPciOperationBusMasterWrite) || 
//####               (Mapping->Operation == EfiPciOperationBusMasterWrite64) )
//####
//####          // Here must be a way to copy context of the Unmapped buffer to
//####          // HostDevice.
//####          // This code given as example only you might need to do some
//####          // chipset programming to access PCI Address Space Above 4G
//####          pBS->CopyMem( (VOID*)(UINTN)Mapping->HostAddr, 
//####                        (VOID*)(UINTN)Mapping->DeviceAddr, 
//####                        Mapping->NumBytes );
//####
//####          pBS->FreePages(Mapping->DeviceAddr, Mapping->NumPages);
//####          pBS->FreePool(Mapping);
//####      }
//####  DEBUG

    return Status;
}

/**
    Chipset Specific function to do PCI RB Attributes releated
    programming.

    @param RbData Pointer to Root Bridge private structure.
    @param Attributes The Root Bridge attributes to be programming.
    @param ResourceBase Pointer to the resource base. (OPTIONAL)
    @param ResourceLength Pointer to the resource Length. (OPTIONAL)

        EFI_STATUS
    @retval EFI_SUCCESS Successful.
    @retval EFI_INVALID_PARAMETER One of the parameters has an
        invalid value.
**/
EFI_STATUS RbCspIoPciAttributes (
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN UINT64                                       Attributes,
    IN OUT UINT64                                   *ResourceBase OPTIONAL,
    IN OUT UINT64                                   *ResourceLength OPTIONAL )
{


    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}

/**
    Read Pci Registers into buffer.
    Csp Function which actualy access PCI Config Space.
    Chipsets that capable of having PCI EXPRESS Ext Cfg Space
    transactions.
    Must Implement this access routine here.

    @param RbData Root Bridge private structure.
    @param Width PCI Width.
    @param Address PCI Address.
    @param Count Number of width reads/writes.
    @param Buffer Buffer where read/written.
    @param Write Set if write.

        EFI_STATUS
    @retval EFI_SUCCESS Successful read.
    @retval EFI_INVALID_PARAMETER One of the parameters has an
**/
EFI_STATUS RootBridgeIoPciRW (
    IN VOID                                     *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
    IN UINT64                                   Address,
    IN UINTN                                    Count,
    IN OUT VOID                                 *Buffer,
    IN BOOLEAN                                  Write )
{
    BOOLEAN ValidCfg = TRUE;
    UINT8   IncrementValue = 1 << (Width & 3); // 1st 2 bits currently define
                                               // width.
                                               // Other bits define type.
    //PCI Express Base Address will be added inside the library call.
    //PciEBase address uses only lower 28 bit of the PciAddress. 
    //We will use UPPER 32 bits as a segment # for the call.
    UINT64  PciAddress = (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus << 20 ) + \
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device << 15 ) + \
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 12) + \
        LShiftU64(((EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL*)RbData)->SegmentNumber,32);
//-------------------------------------------

    PciAddress += \
        ((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->ExtendedRegister ? \
            ((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->ExtendedRegister : \
            ((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register;

    // To read 64bit values, reduce Increment Value (by half) and
    // double the count value (by twice)
    if (IncrementValue > 4) {
        IncrementValue >>= 1;
        Count <<= 1;
    }

    if (Width >= EfiPciWidthMaximum || IncrementValue > 4)
        return EFI_INVALID_PARAMETER;

    while (Count--) {
		//Check if Access address falls into PCIExpress Config Address range
        //Range check and and Number of Buses checked on PCI Bus Driver level.
        if (PciAddress & 0xF0000000) ValidCfg = FALSE;
        if (Write) {
            switch(IncrementValue) {
                case 1:
                    if (ValidCfg) //*(UINT8*)PciAddress = *(UINT8*)Buffer;
                    	PciWrite8(PciAddress, *(UINT8*)Buffer);
                    break;
                case 2:
                    if (ValidCfg) //*(UINT16*)PciAddress = *(UINT16*)Buffer;
                    	PciWrite16(PciAddress, *(UINT16*)Buffer);
                    break;
                default:
                    if (ValidCfg) //*(UINT32*)PciAddress = *(UINT32*)Buffer;
                    	PciWrite32(PciAddress, *(UINT32*)Buffer);
                    break;
            }
        } else {
            switch(IncrementValue) {
                case 1:
                    //*(UINT8*)Buffer = (ValidCfg) ? *(UINT8*)PciAddress : -1;
                	*(UINT8*)Buffer = (ValidCfg) ? PciRead8(PciAddress) : -1;
                    break;
                case 2:
                    //*(UINT16*)Buffer = (ValidCfg) ? *(UINT16*)PciAddress : -1;
                	*(UINT16*)Buffer = (ValidCfg) ? PciRead16(PciAddress) : -1;
                    break;
                default:
                    //*(UINT32*)Buffer = (ValidCfg) ? *(UINT32*)PciAddress : -1;
                	*(UINT32*)Buffer = (ValidCfg) ? PciRead32(PciAddress) : -1;
                    break;
            }
        }

        if (Width <= EfiPciWidthFifoUint64) {
             Buffer = ((UINT8 *)Buffer + IncrementValue);
            // Buffer is increased for only EfiPciWidthUintxx and
            // EfiPciWidthFifoUintxx
        }

        // Only increment the PCI address if Width is not a FIFO.
        if ((Width & 4) == 0) {
            PciAddress += IncrementValue;
        }
    }

    return EFI_SUCCESS;
}



//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

