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

  ACPI Data structures for building ACPI Tables
  !!!!!!!!!!!!!!!!NO PORTING NEEDED!!!!!!!!!!!!!

 */


#ifndef  _ACPI11_H   //To Avoid this header get compiled twice
#define  _ACPI11_H


#include	<Efi.h>
#include    <Token.h>
#include <Acpi.h>

#pragma pack(1)

//======================================================
//  RSDT Pointer structure for IA32
//======================================================
typedef struct _RSDT_PTR32{
    UINT64      Signature; //should contain trailing space
    UINT8       Checksum;
    UINT8       OEMID[6];
    UINT8       reserved;  //should be 0  for ACPI 1.0
    UINT32      RsdtAddress;
} RSDT_PTR32,*PRSDT_PTR32;

//======================================================
//  RSDT Pointer structure for IA64
//======================================================
typedef struct _RSDT_PTR64{
    UINT64  Signature;      // RSDP_signature
    UINT8   Checksum;       // Sum of all bytes of this struct must equal 0
    UINT8   OEMID[6];       // UINTnique OEM ID string
    UINT8   Reserved;       // must be 0
    UINT64  RsdtAddress;    // Physical Address of RSDT
} RSDT_PTR64,*PRSDT_PTR64;

//======================================================
//  ACPI Tables header structure for IA64
//======================================================
typedef struct _ACPI_HDR64 {
    UINT32      Signature;
    UINT32      Length;
    UINT8       Revision; 
	UINT8		Checksum;
    UINT8       OemId[6];
	UINT8		OemTableId[8];
    UINT32      OemRevision;
	UINT32      CreatorId;
	UINT32		CreatorRevision;
    UINT32      Reserved;
} ACPI_HDR64,*PACPI_HDR64;

#define facp_flags FF_WBINVD | FF_SLP_BUTTON

//======================================================
//  RSDT Root System Description Table for IA64
//======================================================
typedef	struct _RSDT64 {
	ACPI_HDR64	Header;
	UINT64		Ptrs[ACPI_RSDT_TABLE_NUM];
} RSDT64, *PRSDT64;

//======================================================
//  FACP Fixed ACPI Description Table for IA32
//======================================================
typedef struct _FACP32 {
    ACPI_HDR  Header;              //0..35
    UINT32      FIRMWARE_CTRL;       //36  Phisical memory address (0-4G) of FACS
    UINT32      DSDT;                //40  Phisical memory address (0-4G) of DSDT
    UINT8       INT_MODEL;           //44  0 Dual PIC; 1 Multipy APIC; >1 reserved
    UINT8       Reserved1;           //45
    UINT16      SCI_INT;             //46  SCI int Pin should be Shareble/Level/Act Low
    UINT32      SMI_CMD;             //48  Port Addr of ACPI Command reg
    UINT8       ACPI_ENABLE_CMD;         //52  Value to write in SMI_CMD reg
    UINT8       ACPI_DISABLE_CMD;        //53  Value to write in SMI_CMD reg
    UINT8       S4BIOS_REQ;          //54  Value to write in SMI_CMD reg
    UINT8       Reserved2;           //55
    UINT32      PM1a_EVT_BLK;        //56
    UINT32      PM1b_EVT_BLK;        //60
    UINT32      PM1a_CNT_BLK;        //64
    UINT32      PM1b_CNT_BLK;        //68
    UINT32      PM2_CNT_BLK;         //72
    UINT32      PM_TMR_BLK;          //76
    UINT32      GPE0_BLK;            //80
    UINT32      GPE1_BLK;            //84
    UINT8       PM1_EVT_LEN;         //88
    UINT8       PM1_CNT_LEN;         //89
    UINT8       PM2_CNT_LEN;         //90
    UINT8       PM_TM_LEN;           //91
    UINT8       GPE0_BLK_LEN;        //92
    UINT8       GPE1_BLK_LEN;        //93
    UINT8       GPE1_BASE;           //94
    UINT8       Reserved3;           //95
    UINT16      P_LVL2_LAT;          //96
    UINT16      P_LVL3_LAT;          //98
    UINT16      FLUSH_SIZE;          //100
    UINT16      FLUSH_STRIDE;        //102
    UINT8       DUTY_OFFSET;         //104
    UINT8       DUTY_WIDTH;          //105
    UINT8       DAY_ALRM;            //106
    UINT8       MON_ALRM;            //107
    UINT8       CENTURY;             //108
    UINT8       Reserved4;           //109
    UINT16      Reserved5;           //110
    UINT32      FLAGS;               //112
}FACP32,*PFACP32;

//======================================================

//======================================================
//  FACP Fixed ACPI Description Table for IA64
//======================================================
typedef struct _FACP64{
    ACPI_HDR64  Header;              //0..39
    UINT64      FIRMWARE_CTRL;       //40  Phisical memory address of FACS
    UINT64      DSDT;                //48  Phisical memory address of DSDT
    UINT8       INT_MODEL;           //56  <2-reserved; Mult SAPIC; >2 reserved
    UINT8       ADDRESS_SPACE;       //57  Addr space bitmask for each reg block
    UINT16      SCI_INT;             //58  SCI int Pin should be Shareble/Level/Act Low
    UINT8       ACPI_ENABLE;         //60  Value to write in SMI_CMD reg
    UINT8       ACPI_DISABLE;        //61  Value to write in SMI_CMD reg
    UINT8       S4BIOS_REQ;          //62  Value to write in SMI_CMD reg
    UINT8       Reserved1;           //63
    UINT64      SMI_CMD;             //64  Port Addr of ACPI Command reg
    UINT64      PM1a_EVT_BLK;        //72
    UINT64      PM1b_EVT_BLK;        //80
    UINT64      PM1a_CNT_BLK;        //88
    UINT64      PM1b_CNT_BLK;        //96
    UINT64      PM2_CNT_BLK;         //104
    UINT64      PM_TMR_BLK;          //112
    UINT64      GPE0_BLK;            //120
    UINT64      GPE1_BLK;            //128
    UINT8       PM1_EVT_LEN;         //136
    UINT8       PM1_CNT_LEN;         //137
    UINT8       PM2_CNT_LEN;         //138
    UINT8       PM_TM_LEN;           //139
    UINT8       GPE0_BLK_LEN;        //140
    UINT8       GPE1_BLK_LEN;        //141
    UINT8       GPE1_BASE;           //142
    UINT8       Reserved2;           //143
    UINT16      P_LVL2_LAT;          //144
    UINT16      P_LVL3_LAT;          //146
    UINT8       DAY_ALRM;            //148
    UINT8       MON_ALRM;            //149
    UINT8       CENTURY;             //150
    UINT8       Reserved3;           //151
    UINT32      FLAGS;               //152
}FACP64,*PFACP64;


//======================================================
//  FACS Firmware ACPI Control Structure for IA32
//======================================================
typedef struct _FACS32 {
    UINT32      Signature;           //0
    UINT32      Length;              //4
    UINT32      HardwareSignature;   //8
    UINT32      FwWakingVector;      //12
    UINT32      GlobalLock;          //16
    UINT32      Flags;               //20
    UINT8       Reserved[40];        //24 Should be 0
} FACS32,*PFACS32;

//======================================================
//  FACS Firmware ACPI Control Structure for IA64
//======================================================
typedef struct _FACS64 {
    UINT32      Signature;	         //0
    UINT32      Length;              //4
    UINT32      HardwareSignature;   //8
    UINT32      Reserved1;           //12
    UINT64      FwWakingVector;      //16
    UINT64      GlobalLock;          //24
    UINT32      Flags;               //32
    UINT8       Reserved[28];        //24 Should be 0
} FACS64,*PFACS64;


//======================================================
//  SPIC Multiple SAPIC Description Table Header IA64
//======================================================
typedef	struct _SPIC64H {
	ACPI_HDR64	Header;				//0..39
	UINT64		IntBlock;			//40
} SPIC64H,*PSPIC64H;

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
//  LAPIC Structure per each CPU  IA64
//======================================================
typedef	struct _LSAPIC64 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT16	CpuId;
	UINT16	Flags;
	UINT8		LsapicId;
	UINT8		LsapicEid;
} LSAPIC64,*PLSAPIC64;

//======================================================
//  Bits Description in LAPICXX.Flags field. IA32&64
//======================================================
#ifndef FI_Enabled
#define	FI_Enabled		0x0001
#endif
//  IOAPIC Structure per each IOAPIC  IA64
//======================================================
typedef	struct _IOSAPIC64 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT16	Reserved;			//2
	UINT32	SysVectBase;		//4
	UINT64	IosapicAddress;		//8
} IOSAPIC64,*PIOSAPIC64;

//======================================================
//  ISO Interrupt Source Override Structure IA64
//======================================================
//  Should be present per each ISA Interrupt which
//  is not identity-mapped into SAPIC Interrupt space
//======================================================
typedef	struct _ISO64 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT8		Bus;				//2
	UINT8		Source;				//3
	UINT32	GlobalSysVect;		//4
	UINT16	Flags;				//8
	UINT8		Reserved[6];		//12
} ISO64,*PISO64;


//======================================================
//  NIS Non-Maskable Interrupt Source Structure IA32
//======================================================
typedef	struct _NIS32 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT16	Flags;				//2
	UINT32	GlobalSysVect;		//4
} NIS32,*PNIS32;


//======================================================
//  PIS Platform Interrupt Source Structure IA64
//======================================================
typedef	struct _PIS64 {
	UINT8		Type;				//0
	UINT8		Length;				//1
	UINT16	Flags;				//2
	UINT8    IntType;			//4
	UINT8		CpuId;				//5
	UINT8		CpuEid;				//6
	UINT8		IosapicVect;		//7
	UINT32	Reserved;			//8
	UINT32	GlobalSysVect;		//12
} PIS64,*PPIS64;


//======================================================
//  Define INT_TYPE values also can be used in MP 1.5
//  Bits Description in PIS64.IntType field. IA64
//======================================================
#define		IT_INT			0x0000 
#define		IT_NMI			0x0001
#define		IT_PMI			0x0002
#define		IT_EXT			0x0003
#define		IT_INIT			0x0004
#define		IT_CorrMem		0x0005
#define		IT_CorrPlt		0x0006

//======================================================
//  SBDT Smart Battery Description Table IA64
//======================================================
typedef struct _SBDT64 {
	ACPI_HDR64	Header;
	UINT32		WarningLvl;
	UINT32		LowLvl;
	UINT32		CriticalLvl;
} SBDT64,*PSBDT64;

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
