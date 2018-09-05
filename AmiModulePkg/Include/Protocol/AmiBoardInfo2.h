//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2005, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/BIN/Core/Modules/BoardInfo/Protocol/AmiBoardInfo2.h 2     3/01/10 5:06p Yakovlevs $
//
// $Revision: 2 $
//
// $Date: 3/01/10 5:06p $
//**********************************************************************


#ifndef AMI_BOARD_INFO2_H_
#define AMI_BOARD_INFO2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <PiDxe.h>

//Define AmiBoardInfo Protocol Interface GUID
#define AMI_BOARD_INFO2_PROTOCOL_GUID \
{ 0x4FC0733F, 0x6FD2, 0x491b, 0xA8, 0x90, 0x53, 0x74, 0x52, 0x1B, 0xF4, 0x8F }

extern EFI_GUID gAmiBoardInfo2ProtocolGuid;

#define AMI_PCI_DATA_SIG  0x4154414449435024 //"$PCIDATA"

//-------------------------------------------------
//Forward Declarations
//-------------------------------------------------
typedef struct _AMI_BOARD_INFO2_PROTOCOL AMI_BOARD_INFO2_PROTOCOL;

#pragma pack(push, 1)

typedef union _AMI_SDL_PCI_DEV_FLAGS{
    UINT64                      PCI_DEV_FLAGS;
    struct PCI_DEV_FLAG_BIT {
//byte 1..2 PCI Device type
        UINT8		 HostBridgeType      	: 1; //bit 0
        UINT8        RootBridgeType      	: 1; //bit 1
        UINT8        Pci2PciBridgeType   	: 1; //bit 2
        UINT8        Pci2CrdBridgeType   	: 1; //bit 3
        UINT8        PciDevice           	: 1; //bit 4
        UINT8        LpcBridge           	: 1; //bit 5
        UINT8        IsaBridge           	: 1; //bit 6
        UINT8        Virtual             	: 1; //bit 7
        UINT8		 ContainerType			: 1; //bit 8
        UINT8        Reserved1           	: 7; //bit 9..15
//byte 3..4 PCI Feature and Location type
        UINT8        OnBoard             	: 1; //bit 16
        UINT8        PciExpress          	: 1; //bit 17
        UINT8        PciX                	: 1; //bit 18
        UINT8        EmbededRom          	: 1; //bit 19
        UINT8        HotPlug             	: 1; //bit 20
        UINT8        FixedBus            	: 1; //bit 21
        UINT8        Ari                 	: 1; //bit 22
        UINT8		 FixedResources        	: 1; //bit 23
        UINT8		 NoAslHpCode           	: 1; //bit 24
        UINT8        ResourcePadding       	: 1; //bit 25
        UINT8        Reserved2           	: 6; //bit 26..31
//byte 5..6 Reference string properties and UI features
        UINT8        Unicode             	: 1; //bit 32
        UINT8        SmbiosPresent 			: 1; //bit 33
        UINT8        UefiDriverBin        	: 1; //bit 34
        UINT8        StdPciRom      		: 1; //bit 35
        UINT8        HasSetup           	: 1; //bit 36
        UINT8        Reserved3           	: 3; //BIT 37..39
        UINT8		 Reserved4; 			//bit 40..47
//byte 7..8
        UINT8        Reserved5[2];		// Reserved for future use
    } Bits;
} AMI_SDL_PCI_DEV_FLAGS;


typedef enum {
	sBarUnused=0,
	sBarIo16,		//1
	sBarIo32, 		//2 not supported in IA32, x64 architecture.
	sBarMmio32,		//3
	sBarMmio32pf,	//4
	sBarMmio64,		//5
	sBarMmio64Pf,	//6
	sBarRom,		//7 Must have offset field set to 0x30
	sBarMaxType		//8
} AMI_SDL_PCI_BAR_TYPE;


typedef enum {
	sResBus=0,
	sResIo16,		//1
	sResIo32,		//2 not supported in IA32, x64 architecture.
	sResMmio32,		//3
	sResMmio32p,	//4
	sResMmio64,		//5
	sResMmio64p,	//6
	sResMaxResType
} AMI_SDL_PCI_RES_TYPE;


typedef struct _AMI_SDL_PCI_FIX_BAR {
	AMI_SDL_PCI_BAR_TYPE	BarType;
	UINT32					BarOffset;
	UINT64					BarBase;
	UINT64					BarLength;
} AMI_SDL_PCI_FIX_BAR;



/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//Define Internal VeB Structures
//Use BYTE alignment since this stuff comes from ASM file.
//---------------------------------------------------------------------------

typedef struct _AMI_SDL_TBL_HEADER {
	UINT64		TblSignature;
	UINT32		TblLength;
	UINT8		TblChecksum;
	UINT8		Reserved[3];
} AMI_SDL_TBL_HEADER;


typedef struct _AMI_SDL_CHIPSET_RESOURCE {
	UINT64			ResBase;	//Base Address
	UINT64			ResLength;	//Resource Length in bytes
	EFI_GCD_MEMORY_TYPE	ResType;	//Same as in PI 1.2 Spec Vol 2.
	UINT64			Attributes; //if this field == -1 IO; !=-1 MMIO;
	UINT64			SdlFlags;	// Reserved for future use
} AMI_SDL_CHIPSET_RESOURCE;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		PCI_PIC_IRQ_DATA
//
// Description:	Structure used to describe PCI IRQ Router Register and available 
// IRQ MASK used by AMISDL output format.
//
// Fields:		
//  Name            Type        Description
//   ------------------------------------------------------------------
//  ChipsetReg      UINT8       Chipset register number 
//	IrqMask         UINT16      IRQ MASK for legacy Interrupts
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _AMI_SDL_PCI_PIC_IRQ_DATA  {
	UINT8				ChipsetReg;
	UINT16				IrqMask;
} AMI_SDL_PCI_PIC_IRQ_DATA;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		PCI_APIC_IRQ_DATA
//
// Description:	Structure used to describe (APIC) IRQ Routing DATA.
//
// Fields:		
//  Name            Type                Description
//   ------------------------------------------------------------------
//  IoApicItin      UINT8               IO/APIC INTIN pin 
//	IoApicId        UINT8               IO/APIC ID value
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _AMI_SDL_PCI_APIC_IRQ_DATA {
    UINT8   IoApicItin;
    UINT8   IoApicId;
} AMI_SDL_PCI_APIC_IRQ_DATA;

#define AMI_IO_APIC_FLAGS UINT32

#define AMI_IO_APIC_FLAG_PCIBUSAPIC 0x1
#define AMI_IO_APIC_FLAG_IOSAPIC 	0x2
#define AMI_IO_APIC_FLAG_XAPIC 		0x4


typedef union _AMI_IOAPIC_ADDRESS{
    UINT64          ADDRESS;
    struct {
        UINT32      Func    : 8;
        UINT32      Dev     : 8;  
        UINT32      Bus     : 8;
        UINT32      Seg     : 8;
        UINT32		ApicFlags;
    };
}AMI_IOAPIC_ADDRESS;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		AMI_APIC_INFO
//
// Description:	Structure used to describe IO/APIC Information
//
// Fields:		
//  Name            Type                Description
//   ------------------------------------------------------------------
//  BusBuild        UINT8               PCI Bus# Assigned at BIOS build time  
//	BusRun          UINT8               PCI Bus# Assigned at run time  
//  Bridge[]        PCI_DEV_FUN         Describes bridges chain 
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _AMI_SDL_IO_APIC_INFO {
//    AMI_IOAPIC_ADDRESS      ApicAddress;
	UINT64					ApicAddress;
    UINT32                  ApicIdBuild;
    UINT32                  ApicIdRun;
    UINT32					IntVectorBase;
    UINT32					Reserved;
} AMI_SDL_IO_APIC_INFO;

#define AMI_APIC_DATA_SIG 0x4154414449504124 //"$APIDATA"

typedef struct _AMI_SDL_APIC_DATA {
	AMI_SDL_TBL_HEADER		Hdr;
	UINT32					IoApicCount;
	AMI_SDL_IO_APIC_INFO	IoApicInfo[1];
}AMI_SDL_APIC_DATA;



#define AMI_SDL_PCI_INIT_ROUTINE UINT32

typedef struct _AMI_SDL_PCI_DEV_INFO {
	UINT32       				ParentIndex;
	UINT32						PciSegment;
	UINT8 						Bus;			//Optional
	UINT8           			Device;
	UINT8             			Function; 		//Optional
   	UINT8             			Reserved[5];	//for alignment
	UINT64						Attributes;
	UINT8						AslName[5];
   	AMI_SDL_PCI_PIC_IRQ_DATA    PicIrq[4];
   	AMI_SDL_PCI_APIC_IRQ_DATA   ApicIrq[4];
   	UINT32                      Slot; 			//Used in _SUN ASL method and SMBIOS
	UINT32						SsvId; 			//Device Subsystem Vendor ID
	AMI_SDL_PCI_DEV_FLAGS		PciDevFlags;
	UINT32                   	NameStringIndex;
	EFI_GUID                	RomFileGuid;
	EFI_GUID					RomSectionGuid;
	AMI_SDL_PCI_FIX_BAR			FixedResources[7]; //Including RomBAR
	UINT64              		ResourcePadding[7];
	AMI_SDL_PCI_INIT_ROUTINE 	InitRoutine;
   	UINT8                       SmbiosInfo[0x10];   // Reserved for future use
} AMI_SDL_PCI_DEV_INFO;


typedef struct _AMI_SDL_PCI_DATA {
	AMI_SDL_TBL_HEADER		Hdr;
	UINT64             		PciGlobalFlags; // Reserved for future use
	UINT32					PciDevCount;
	UINT32					CspResCount;
//PCI Device Table
	AMI_SDL_PCI_DEV_INFO	PciDevices[1]; // There are PciDevCount elements in the array
//Chipset Specific resources table
//  AMI_SDL_CHIPSET_RESOURCE CspResources[CspResCount];
} AMI_SDL_PCI_DATA;


///////////////////////////////////////////////
//SIO related Device Information.
#define AMI_SIO_DATA_SIG 0x415441444F495324 //"$SIODATA"

#define AMI_SDL_SIO_INIT_ROUTINE UINT32

typedef struct _AMI_SDL_LOGICAL_DEV_INFO {
	UINT32	        			Type;
	UINT8			        	Ldn;
	UINT8			        	Uid;
	UINT16			      		PnpId;
	BOOLEAN     	        	Implemented;
	BOOLEAN			      		HasSetup;
	UINT8			        	Flags;
	UINT8			        	AslName[5];
	UINT16		            	ResBase[2];
	UINT8			        	ResLen[2];
	UINT16			      		IrqMask[2];
	UINT8			        	DmaMask[2];
	AMI_SDL_SIO_INIT_ROUTINE 	InitRoutine;
} AMI_SDL_LOGICAL_DEV_INFO;

//==================================================================
//Definitions of SIO Shared Resource Flags if this flags set
//and will share following type of resources with PREVIOUSE device
#define SIO_SHR_NONE	0x00
#define SIO_SHR_IO1		0x01 //device shares resources programmed in SIO_1_BASE_REG
#define SIO_SHR_IO2		0x02 //device shares resources programmed in SIO_2_BASE_REG
#define SIO_SHR_IO		(SIO_SHR_IO1 | SIO_SHR_IO2) //device shares resources programmed in both SIO_BASE_REG
#define SIO_SHR_IRQ1	0x04
#define SIO_SHR_IRQ2	0x08
#define SIO_SHR_IRQ		(SIO_SHR_IRQ1 | SIO_SHR_IRQ2)
#define SIO_SHR_DMA1	0x10
#define SIO_SHR_DMA2	0x20
#define SIO_SHR_DMA		(SIO_SHR_DMA1 | SIO_SHR_DMA2)
#define SIO_SHR_ALL		(SIO_SHR_IO | SIO_SHR_IRQ | SIO_SHR_DMA)
#define SIO_NO_RES		0x80 //this bit will be set if device not using any resources but must be enabled
//all other values reserved


typedef struct _AMI_SDL_SIO_CHIP_INFO {
	UINT32					LpcBridgeIndex;
    UINT8                   StdSioDevice; //LD or not LD based SIO. 1/0 
	UINT8					IsaBusNo;
	UINT8					IsaDevNo;
	UINT8					IsaFuncNo;
	UINT16					SioIndex;
	UINT16			     	SioData;
	// Dev Select and Activate
	UINT8			        DevSelReg;
	UINT8			        ActivateReg;
	UINT8			        ActivateVal;
	UINT8			        DeactivateVal;
	//Generic registers location
	UINT8			        Base1HiReg;
	UINT8			        Base1LoReg;
	UINT8			        Base2HiReg;
	UINT8			        Base2LoReg;
	UINT8			        Irq1Reg;
	UINT8			        Irq2Reg;
	UINT8			        Dma1Reg;
	UINT8			        Dma2Reg;
   	UINT8                   GlobalRegMask[32];
   	UINT8                   LocalRegMask[32];
	UINT8                   EnterCfgMode[0x10]; // Reserved for future use
	UINT8                   ExitCfgMode[0x10]; // Reserved for future use
   	UINT8                   SmbiosInfo[0x10];  // Reserved for future use
   	UINT32			        LogicalDevCount;
 	AMI_SDL_LOGICAL_DEV_INFO LogicalDev[1]; // There are LogicalDevCount elements in the array
} AMI_SDL_SIO_CHIP_INFO;


typedef struct _AMI_SDL_SIO_DATA {
	AMI_SDL_TBL_HEADER          Header; //Signature ="$SIODATA"
    UINT64                      GlobalSioFalsgs; // Reserved for future use
    UINT32                      SioCount;
    AMI_SDL_SIO_CHIP_INFO       SioDev[1]; // There are SioCount elements in the array
} AMI_SDL_SIO_DATA;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		PCI_DEV_FUN
//
// Description:	UNION of UINT8 and a Structure used to describe AMISDL format
// for PCI Device Function Package. Packed PCI DEV/FUNCTION number.
//
// Fields:
//  Name            Type        Description
//   ------------------------------------------------------------------
//  DEV_FUN         UINT8       Device / Function all together
//	Fun             UINT8       PCI Device's Function#
//	Dev             UINT8       PCI Device's Device#
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef  union _PCI_DEV_FUN {
	UINT8				DEV_FUN;
	struct {
		UINT8			Fun	: 3;
		UINT8			Dev	: 5;
	};
} PCI_DEV_FUN;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		PCI_PIC_IRQ_DATA
//
// Description:	Structure used to describe PCI IRQ Router Register and available
// IRQ MASK used by AMISDL output format.
//
// Fields:
//  Name            Type        Description
//   ------------------------------------------------------------------
//  ChipsetReg      UINT8       Chipset register number
//	IrqMask         UINT16      IRQ MASK for legacy Interrupts
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _PCI_PIC_IRQ_DATA {
	UINT8				ChipsetReg;
	UINT16				IrqMask;
} PCI_PIC_IRQ_DATA;

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		PCI_IRQ_PIC_ROUTE
//
// Description:	Structure used to describe Legacy (PIC) IRQ Routing Information.
// AUTOGENERATED DATA.
//
// Fields:
//  Name            Type                Description
//   ------------------------------------------------------------------
//  PciBusNumber    UINT8               PCI Bus #
//	DevFun          PCI_DEV_FUN         PCI Device # \ Function #
//  PciIrq[4]       PCI_PIC_IRQ_DATA    Register \ IRQ MASK
//  SlotNum         UINT8               Pci Slot # or 0 if embeded device
//  Reserved        UINT8
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _PCI_IRQ_PIC_ROUTE {
	UINT8				PciBusNumber;
	PCI_DEV_FUN			DevFun;
	PCI_PIC_IRQ_DATA	PciIrq[4];
	UINT8				SlotNum;
	UINT8				Reserved;
} PCI_IRQ_PIC_ROUTE;


//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		PCI_APIC_IRQ_DATA
//
// Description:	Structure used to describe (APIC) IRQ Routing DATA.
//
// Fields:
//  Name            Type                Description
//   ------------------------------------------------------------------
//  IoApicItin      UINT8               IO/APIC INTIN pin
//	IoApicId        UINT8               IO/APIC ID value
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _PCI_APIC_IRQ_DATA {
    UINT8   IoApicItin;
    UINT8   IoApicId;
} PCI_APIC_IRQ_DATA;


//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:		PCI_IRQ_APIC_ROUTE
//
// Description:	Structure used to describe Extended (IO/APIC) IRQ Routing
// Information. AUTOGENERATED DATA.
//
// Fields:
//  Name            Type                Description
//   ------------------------------------------------------------------
//  PciBusNumber    UINT8               PCI Bus #
//	DeviceNumber    UINT8               PCI Device #
//  Intn[4]         PCI_APIC_IRQ_DATA   APIC IRQ Routing Info.
//  Reserved        UINT8
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _PCI_IRQ_APIC_ROUTE {
    UINT8   			PciBusNumber;
    UINT8   			DeviceNumber;
    PCI_APIC_IRQ_DATA 	Intn[4];
    UINT8   			Reserved;
} PCI_IRQ_APIC_ROUTE;

#pragma pack(pop)

//----------------------------------------------------------------------------
//Definition of AMI Board Info Protocol.
//All PCI related VeB configuration output files 
//generated based on PCI Portibg using VeB Wizards
typedef struct _AMI_BOARD_INFO2_PROTOCOL {
    UINT32                  BoardInfoVer;
    BOOLEAN                 DataValid;      //Signifies if Board Specific IRQ data was updated. 
    UINT8                   Reserved[7];
    AMI_SDL_PCI_DATA      	*PciBrdData;
    AMI_SDL_SIO_DATA        *SioBrdData;
    AMI_SDL_APIC_DATA       *ApicBrdData;
    VOID                    *BrdAcpiInfo;
    UINTN                  	BrdAcpiLength;
    VOID					*BrdAcpiIrqInfo; //Optional
    VOID					*BrdSetupInfo;
    VOID                    *BoardOemInfo;
    PCI_IRQ_PIC_ROUTE       *PicRoutTable;  //Will be filled by PciBus Driver
    UINTN                   PicRoutLength;	//Will be filled by PciBus Driver
    PCI_IRQ_APIC_ROUTE      *ApicRoutTable;	//Will be filled by PciBus Driver
    UINTN                   ApicRoutLength;	//Will be filled by PciBus Driver
    BOOLEAN					IrqDataValid;
} AMI_BOARD_INFO2_PROTOCOL;



/****** DO NOT WRITE BELOW THIS LINE *******/

#ifdef __cplusplus
}
#endif
#endif	//AMI_BOARD_INFO2_H_


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2005, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

