//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
 
*/

#ifndef  _ACPI_H   //To Avoid this header get compiled twice
#define  _ACPI_H


#include	<Efi.h>
#include    <Token.h>

//---------------------------------------------------------------------------
#pragma	pack(1)

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

//======================================================
//  ACPI Description Headers Signatures
//======================================================
#define MCFG_SIG 0x4746434D  //PCI Express Congiguration Table

#define	HPET_SIG 0x54455048

//for testing purposes only
#define IPPT_SIG 0x54505049 // "IPPT" --  IA64 Platform Properties Table

//======================================================
//  RSDT Pointer structure for IA32
//======================================================
typedef struct _RSDT_PTR_20{
    UINT64		    Signature;	//should contain trailing space
    UINT8			Checksum;   //Only First 20 bytes for future compatability 
    UINT8			OEMID[6];
    UINT8			Revision;	//Now Revision ID was reserved in ACPI 1.1 
							//should be 0  for ACPI 1.0 or 2 for ACPI 2.0
    UINT32		    RsdtAddr;	//32 bit RSDT Address for future compatability
	// Here follows additional part that couldn't be met in ACPI 1.1 
	UINT32			Length;		//Length of the table in bytes including header;
	UINT64			XsdtAddr;	
	UINT8			XtdChecksum;//Checksum of entire table
	UINT8			Reserved[3];
} RSDT_PTR_20,*PRSDT_PTR_20;

//======================================================
//  ACPI Tables header structure  for IA32
//======================================================
typedef struct _ACPI_HDR {
    UINT32      Signature;
    UINT32      Length;
    UINT8       Revision;
    UINT8       Checksum;
    UINT8       OemId[6]; 
    UINT8       OemTblId[8];
    UINT32      OemRev;
    UINT32      CreatorId;
    UINT32      CreatorRev;
} ACPI_HDR,*PACPI_HDR;

//======================================================
//  Bits Description in FACP.FLAGS field. IA32&IA64
//======================================================
#define         FF_WBINVD				0x00000001
#define         FF_WBINVD_FLUSH 0x00000002     
#define         FF_PROC_C1      0x00000004
#define         FF_LVL2_UP      0x00000008
#define         FF_PWR_BUTTON   0x00000010
#define         FF_SLP_BUTTON   0x00000020
#define         FF_FIX_RTC      0x00000040
#define         FF_RTC_S4       0x00000080
#define         FF_TMR_VAL_EXT  0x00000100
#define         FF_DCK_CAP      0x00000200

//======================================================
//  RSDT Root System Description Table for IA32
//======================================================
#ifndef ACPI_RSDT_TABLE_NUM
#define ACPI_RSDT_TABLE_NUM	0x10
#endif

typedef	struct _RSDT32 {
	ACPI_HDR	Header;
	UINT32		Ptrs[ACPI_RSDT_TABLE_NUM];
} RSDT32, *PRSDT32, RSDT_20, *PRSDT_20;

//======================================================
//  APIC Multiple APIC Description Table Header IA32
//======================================================
typedef	struct _APIC32H {
	ACPI_HDR	Header;				//0..35
	UINT32		LAPIC_Address;		//36
	UINT32		Flags;				//40
} APIC32H,*PAPIC32H,APIC_20H,*PAPIC_20H;

//======================================================
//  Bits Description in APIC32H.Flags field. IA32
//======================================================
#define	PCAT_COMPAT		0x00000001

//======================================================
//  Bits Description in APICXX.Type field. IA32&64
//======================================================
#define	CpuLapic		0
#define	IoApic			1
//For IA32 following is reserved
#define	IntSrcOvrd		2
#define	IntSrcPlat		3
//For IA64 >4 reserved


//======================================================
//  LAPIC Structure per each CPU  IA32
//======================================================
typedef	struct _LAPIC32 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT8		CpuId;
	UINT8		LapicId;
	UINT32	Flags;
} LAPIC32,*PLAPIC32, LAPIC_20,*PLAPIC_20;

//======================================================
//  IOAPIC Structure per each CPU  IA32
//======================================================
typedef	struct _IOAPIC32 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT8		IoapicId;			//2
	UINT8		Reserved;			//3
	UINT32	IoapicAddress;		//4
	UINT32	SysVectBase;		//8
} IOAPIC32,*PIOAPIC32, IOAPIC_20,*PIOAPIC_20;

//======================================================
//  ISO Interrupt Source Override Structure IA32
//======================================================
//  Should be present per each ISA Interrupt which
//  is not identity-mapped into SAPIC Interrupt space
//======================================================
typedef	struct _ISO32 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT8		Bus;				//2
	UINT8		Source;				//3
	UINT32	GlobalSysVect;		//4
	UINT16	Flags;				//8

} ISO32,*PISO32, ISO_20,*PISO_20;

//======================================================
//  SBDT Smart Battery Description Table IA32
//======================================================
typedef struct _SBDT32 {
	ACPI_HDR	Header;
	UINT32		WarningLvl;
	UINT32		LowLvl;
	UINT32		CriticalLvl;
} SBDT32,*PSBDT32, SBDT_20,*PSBDT_20;

//======================================================
//  Here Follows Definitions that are not Chipset specific
//======================================================

#define CREATOR_ID_MS 0x5446534D  //"TFSM""MSFT"(Microsoft)

#define	CREATOR_REV_MS 0x00010013

#define CREATOR_ID_AMI 0x20494D41  //" IMA""AMI "(AMI)

#define	CREATOR_REV	0x00000000

//==========================================================
// Define  APIC IO Interrupt Assignment INT_FLAGS
//==========================================================
#define IF_PO_HIG	0x1

#define IF_PO_LOW	0x3

#define IF_EL_EDG	0x4

#define IF_EL_LVL	0xC

#pragma	pack()

#endif 

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
