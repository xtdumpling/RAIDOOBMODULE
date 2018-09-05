//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file AcpiCore.h
    ACPI Core Header File. Main ACPI Driver Specific Definitions.

**/
//**********************************************************************
#ifndef __AcpiCore__H__
#define __AcpiCore__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/FirmwareVolume.h>
#include <Protocol/AmiSio.h>
#include <Protocol/DevicePath.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/AcpiTable.h>
#include <AmiDxeLib.h>
#include <AmiHobs.h>
#include <Token.h>
#include <Dxe.h>
#include <AcpiRes.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include "AcpiSdtPrivate.h"
#include <Setup.h>
#include <Acpi11.h>
#include <Acpi20.h>
#include <Efi.h>
#include <Acpi.h>
#pragma	pack(push, 1)

//**********************************************************************
// Protocol Data Definitions
//**********************************************************************
// ACPI Version bitmap definition:
//
// EFI_ACPI_TABLE_VERSION_1_0B - ACPI Version 1.0b (not supported curently)
// EFI_ACPI_TABLE_VERSION_2_0 - ACPI Version 2.0 
// EFI_ACPI_TABLE_VERSION_NONE - No ACPI Versions.  This might be used
//  to create memory-based operation regions or other information
//  that is not part of the ACPI "tree" but must still be found
//  in ACPI memory space and/or managed by the core ACPI driver.
//
//**********************************************************************
/*
#define EFI_ACPI_TABLE_VERSION 			UINT32

#define EFI_ACPI_TABLE_VERSION_NONE		0x00000001
#define EFI_ACPI_TABLE_VERSION_1_0B   	0x00000002
#define EFI_ACPI_TABLE_VERSION_2_0    	0x00000004
#define EFI_ACPI_TABLE_VERSION_3_0    	0x00000008
#define EFI_ACPI_TABLE_VERSION_X    	(EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0)
#define EFI_ACPI_TABLE_VERSION_ALL  	(EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_X)
*/
//==================================================================//
//GUID for DSDT storage type	FFS_FILEGUID                = 11D8AC35-FB8A-44d1-8D09-0B5606D321B9
#define EFI_ACPI_DSDT_V_1_1_GUID {0x9410046e,0x1d26,0x45bc,0xb6,0x58,0xde,0xca,0xb1,0x4d,0x75,0x65}

#define EFI_ACPI_DSDT_V_2_0_GUID {0x11D8AC35,0xFB8A,0x44d1,0x8D,0x09,0x0B,0x56,0x06,0xD3,0x21,0xB9}
//ACPI tables GUIDs must be published in System Configuration table
#define EFI_ACPI_11_TABLE_GUID   {0xeb9d2d30,0x2d88,0x11d3,0x9a,0x16,0x00,0x90,0x27,0x3f,0xc1,0x4d}
//#define EFI_ACPI_20_TABLE_GUID   {0x8868e871,0xe4f1,0x11d3,0xbc,0x22,0x00,0x80,0xc7,0x3c,0x88,0x81}

#define	ACPI_REV1       0x01
#define	ACPI_REV2       0x02
#define	ACPI_REV3       0x03
#define	ACPI_REV4       0x04

#define ACPI_TABLE_NOT_FOUND        (~0)
#define ACPI_TABLE_NOT_REMOVABLE    (~1)
//======================================================
//  Here Follows Definitions that are not Chipset specific
//======================================================

#define CREATOR_ID_MS 0x5446534D  //"TFSM""MSFT"(Microsoft)

#define	CREATOR_REV_MS 0x00010013

#define CREATOR_ID_AMI 0x20494D41  //" IMA""AMI "(AMI)

#define	CREATOR_REV	0x00000000


#define APIC_SIG 0x43495041  //"APIC"Multiple APIC Description Table

#define SPIC_SIG 0x43495053  //"SPIC"Multiple SAPIC Description Table

#define BOOT_SIG 0x544F4F42  //"BOOT"MS Simple Boot Spec

#define DBGP_SIG 0x50474244  //"DBGP" MS Bebug Port Spec

#define DSDT_SIG 0x54445344  //"DSDT" Differentiated System Description Table

#define ECDT_SIG 0x54444345	//"ECDT" Embedded Controller Boot Resources Table

#define ETDT_SIG 0x54445445	//"ETDT" Event Timer Description Table

#define FACP_SIG 0x50434146  //"FACP" Fixed ACPI Description Table (FADT)

#define FACS_SIG 0x53434146  //"FACS" //Firmware ACPI Control Structure

#define PSDT_SIG 0x54445350  //"PSDT"Persistent System Description Table

#define RSDT_SIG 0x54445352  //"RSDT"Root System Description Table

#define SBST_SIG 0x54534253  //"SBST"Smart Battery Specification Table

#define SLIT_SIG 0x54494C53  //"SLIT"System Locality Information Table

#define SPCR_SIG 0x52435053  //"SPCR"Serial Port Concole Redirection Table

#define SRAT_SIG 0x54415253  //"SBST"Static Resource Affinity Table

#define SSDT_SIG 0x54445353  //"SSDT"Secondary System Description Table

#define SPMI_SIG 0x494D5053  //"SPMI"Server Platform Management Interface Table

#define XSDT_SIG 0x54445358  //"XSDT" eXtended System Description Table

#define RSDP_SIG 0x2052545020445352 //"RSD PTR "

#define FIDT_SIG 0x54444946  //"FIDT" $FID Table

#define THREE_CHAR_ARRAY(x) {(x)/10+'0',(x)%10+'0',0}

//======================================================
//  ACPI Description Headers Signatures
//======================================================
#define MCFG_SIG 0x4746434D  //PCI Express Congiguration Table

#define	HPET_SIG 0x54455048

//for testing purposes only
#define IPPT_SIG 0x54505049 // "IPPT" --  IA64 Platform Properties Table

//======================================================
//  Bits Description in APICXX.Type field. ACPI 2.0
//======================================================
#ifndef AT_LAPIC_H
#define			AT_LAPIC_H		0
#endif

#ifndef	AT_IOAPIC
#define			AT_IOAPIC		1
#endif

#ifndef	AT_ISO
#define			AT_ISO			2
#endif

#ifndef	AT_NMI
#define			AT_NMI			3
#endif

#ifndef AT_LAPIC_H_NMI
#define			AT_LAPIC_H_NMI	4
#endif

#ifndef AT_LAPIC_H_ADR
#define			AT_LAPIC_H_ADR	5
#endif

#ifndef	AT_IOSAPIC
#define			AT_IOSAPIC		6
#endif
//Updated according ACPI Spec 2.0 errata 1.3
#ifndef AT_LSAPIC
#define			AT_LSAPIC		7
#endif

#ifndef AT_PIS
#define			AT_PIS			8
#endif

#ifndef AT_LX2APIC
#define			AT_LX2APIC      9
#endif

#ifndef AT_LX2APIC_NMI
#define			AT_LX2APIC_NMI  0xa
#endif


#define		IOA_BASE_BOT		0xFEC00000
#define		IOA_IND_REG_OFFS	0x00
#define		IOA_DAT_REG_OFFS	0x10
#define		IOA_ID_IND		    0x00
#define		IOA_VER_IND		    0x01

/**
    Structure to store System resource Information
 
**/

typedef struct _ACPI_AML_RES {
	UINT64			Base; ///< Base address
	UINT64			Length;	///< Length of the structure
} ACPI_AML_RES;

typedef enum {
	pgBelow1M,
	pgBelow4G,
	pgAbove4G,
	pgMaxResType,
} AML_RES_TYPE;

//======================================================
//  Bits Description in LAPIC_XX.Flags field. ACPI 2.0
//======================================================
#define		FL_ENABLE		0x0001

/**
    Common Header for MADT table entries.
             
**/
typedef struct _MADT_ENTRY_HEADER{
	UINT8	Type; ///< Header type (APIC, LAPIC ...)
	UINT8	Length; ///< Header Length
}MADT_ENTRY_HEADER;

/**
     Processor Local APIC Structure for each CPU 
             
**/

typedef	struct _LAPIC_H32 {
    MADT_ENTRY_HEADER   Header;
	UINT8		        CpuId;///< The ProcessorId for which this processor is listed in the ACPI Processor declaration operator
	UINT8		        LapicId;///< The processor’s local APIC ID.
	UINT32	            Flags;///< Local APIC flags (See ACPI Spec)
} LAPIC_H32,*PLAPIC_H32, LAPIC_H_20,*PLAPIC_H_20;


/**
  Processor Local x2APIC Structure per each CPU 
  
**/

typedef	struct _LX2APIC {
    MADT_ENTRY_HEADER   Header;
    UINT16              Reserved;
	UINT32              ApicId;///< The processor’s local x2APIC ID.
	UINT32	            Flags;///< Same as Local APIC flags (See ACPI Spec)
    UINT32              Uid;///< OSPM associates the X2APIC Structure with a processor object declared in the namespace using the Device statement, when the _UID child object of the processor device evaluates to a numeric value, by matching the numeric value with this field
} LX2APIC,*PLX2APIC;

/**
  I/O APIC Structure per each CPU 
  
**/

typedef	struct _IOAPIC_H32 {
    MADT_ENTRY_HEADER   Header;
	UINT8		        IoapicId;///< The I/O APIC’s ID.
	UINT8		        Reserved;
	UINT32	            IoapicAddress;///< The 32-bit physical address to access this I/O APIC. Each I/O APIC resides at a unique address.
	UINT32	            SysVectBase;///< The global system interrupt number where this I/O APIC’s interrupt inputs start. The number of interrupt inputs is determined by the I/O APIC’s Max Redir Entryregister.
} IOAPIC_H32,*PIOAPIC_H32, IOAPIC_H20,*PIOAPIC_H20;

/**
  Interrupt Source Override Structure 
  Should be present per each ISA Interrupt which
  is not identity-mapped into SAPIC Interrupt space
  
**/

typedef	struct _ISO_H32 {
    MADT_ENTRY_HEADER   Header;
	UINT8		        Bus;///< 0 Constant, meaning ISA
	UINT8		        Source;///<	Bus-relative interrupt source (IRQ)
	UINT32	            GlobalSysVect;///< The Global System Interrupt that this bus-relative interrupt source will signal.
	UINT16	            Flags;///< MPS INTI flags (See ACPI Spec)

} ISO_H20,*PISO_H20;

typedef	struct _LSAPIC_H20 {	
    MADT_ENTRY_HEADER   Header;		    
	UINT8			CpuId;				//2
	UINT8			LsapicId;			//3
	UINT8			LsapicEid;			//4
    UINT8           Reserved1;          //5
    UINT8           Reserved2;          //6
    UINT8           Reserved3;          //7
	UINT32			Flags;				//8 Same as MPS 1.4 
} LSAPIC_H20,*PLSAPIC_H20;

/**
  Processor Local SAPIC Structure 
  
**/  

typedef	struct _LSAPIC_H30 {	
    MADT_ENTRY_HEADER   Header;
	UINT8			CpuId;///< OSPM associates the Local SAPIC Structure with a processor object declared in the namespace using the Processor statement by matching the processor object’s ProcessorID value with this field.
	UINT8			LsapicId;///< The processor’s local SAPIC ID
	UINT8			LsapicEid;///< The processor’s local SAPIC EID
    UINT8           Reserved1;          
    UINT8           Reserved2;          
    UINT8           Reserved3;         
	UINT32			Flags;///< Local SAPIC flags (See ACPI Spec).
    UINT32          AcpiProcUidVal;///< OSPM associates the Local SAPIC Structure with a processor object declared in the namespace using the Devicestatement, when the _UID child object of the processor device evaluates to a numeric value, by matching the numeric value with this field.
    UINT8           AcpiProcUidString;///< OSPM associates the Local SAPIC Structure with a processor object declared in the namespace using the Devicestatement, when the _UID child object of the processor device evaluates to a string, by matching the string with this field. This value is stored as a null-terminated ASCII string.
    ///It can be be more than one AcpiProcUidString entries
} LSAPIC_H30,*PLSAPIC_H30;

/**
  I/O SAPIC Structure 
  
**/  
typedef	struct _IOSAPIC_H20 {	
    MADT_ENTRY_HEADER   Header;
	UINT8			IoapicId;///< I/O SAPIC ID
	UINT8			Reserved;			
	UINT32			SysVectBase;///< The global system interrupt number where this I/O SAPIC’s interrupt inputs start. The number of interrupt inputs is determined by the I/O SAPIC’s Max Redir Entryregister.
	UINT64			IosapicAddress;///<	The 64-bit physical address to access this I/O SAPIC. Each I/O SAPIC resides at a unique address. 
} IOSAPIC_H20,*PIOSAPIC_H20;

typedef struct _ISO_PARAMETER_TABLE{
	UINT8	PicIrq;
	UINT8	Flags; 
	UINT16	ApicInt;
}ISO_PARAMETER_TABLE;

/**
	Non-Maskable Interrupt Source Structure

**/  

typedef	struct _NMI_H20 {
    MADT_ENTRY_HEADER   Header;
	UINT16			Flags;///< Same as MPS INTI flags.
	UINT32			GlobalSysVect;///< The Global System Interrupt that this NMI will signal.
} NMI_H20,*PNMI_H20;

/**
	 Local APIC NMI Structure 

**/  

typedef	struct _LNMI_H20 {
    MADT_ENTRY_HEADER   Header;
	UINT8			CPU_ID;///< Processor ID corresponding to the ID listed in the processor object. A value of 0xFF signifies that this applies to all processors in the machine.
	UINT16			Flags;///< MPS INTI flags.
	UINT8			LapicIntin;///< Local APIC interrupt input LINTn to which NMI is connected.
} LNMI_H20,*PLNMI_H20;

/**
	Local x2APIC NMI Structure
**/ 

typedef	struct _LX2APIC_NMI {
    MADT_ENTRY_HEADER   Header;
	UINT16			Flags;///< Same as MPS INTI flags.
    UINT32          Uid;///< UID corresponding to the ID listed in the processor Device object. A value of 0xFFFFFFFF signifies that this applies to all processors in the machine.
	UINT8			LapicIntin;///< Local x2APIC interrupt input LINTn to which NMI is connected.
    UINT8           Reserved[3]; 
} LX2APIC_NMI,*PLX2APIC_NMI;


/**
    Structure for storing collected entries for MADT table before 
    actually building it.  It is in the form of T_ITEM_LIST.
 
**/
typedef	struct _MADT_ENTRIES {
    UINTN               MadtEntInitCount;
    UINTN               MadtEntCount;
    MADT_ENTRY_HEADER   **MadtEntries;

} MADT_ENTRIES;

//======================================================
//  Bits Description in FACS.GlobalLock field. ACPI 2.0
//======================================================
#define     GL_PENDING          0x01
#define     GL_OWNED			0x20

/**
    Structure for storing pointers for ACPI tables and it's Version
    Member of ACPI_DB
 
**/
typedef	struct _ACPI_TBL_ITEM {
    ACPI_HDR                *BtHeaderPtr;
    EFI_ACPI_TABLE_VERSION  AcpiTblVer;

} ACPI_TBL_ITEM;

/**
    Structure for storing ACPI tables and protocols during Boottime.
    ACPI tables are stored in the form of T_ITEM_LIST.

**/
typedef	struct _ACPI_DB {
    UINTN                       AcpiInitCount; ///< Initial number of entries (used for allocate memory).
    UINTN                       AcpiEntCount; ///< Real number of entries .
    ACPI_TBL_ITEM               **AcpiEntries; ///< Pointer to Table item.
    UINT32                      AcpiLength; ///< Total length of all ACPI entries.
    EFI_ACPI_TABLE_PROTOCOL     AcpiTableProtocol;
    EFI_ACPI_SUPPORT_PROTOCOL	AcpiSupportProtocol;
	EFI_HANDLE					AcpiSupportHandle;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
//#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x00010014)
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    EFI_ACPI_SDT_PROTOCOL       AcpiSdtProtocol;
    UINTN                       NotifyInitCount;
    UINTN                       NotifyFnCount;
    EFI_ACPI_NOTIFICATION_FN    *AcpiNotifyFn;

//    UINTN                       SdtHndInitCount;
//    UINTN                       SdtHandleCount;
//    EFI_ACPI_NOTIFICATION_FN    *AcpiNotifyFn;
    EFI_AML_NODE_LIST           *AmlRootNode;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
//#endif
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
} ACPI_DB;

//Function Prototypes
UINTN FindAcpiTblByHandle (UINTN *Handle);



#pragma pack(pop)

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
