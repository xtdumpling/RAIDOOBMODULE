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

	ACPI v2.0 Data structures
	!!!!!!!!!!!!!!!!!!NO PORTING NEEDED!!!!!!!!!!!!!!!!
*/


#ifndef  _ACPI20_H   //To Avoid this header get compiled twice
#define  _ACPI20_H

#include	<Efi.h>
#include	<Token.h>
#include <Acpi.h>

#pragma	pack(push, 1)




//======================================================
//  GAS_20 Generic Address Structure Values
//======================================================
#define		GAS_SYS_MEM		0x00	//System memory in IA32 Hi Dword must be 0
#define		GAS_SYS_IO		0x01	//System I/O in IA32 Hi Dword must be 0
#define		GAS_PCI_CFG		0x02	//PCI Configuration Space on PCI BUS 0 SEG 0
									//Hi-> W3=0 | W2=DevId | W1=FunNo | W0=Offs <-Lo
#define		GAS_EMB_CTR		0x03	//Embeded Controller
#define		GAS_SM_BUS		0x04	//SMBus
#define		GAS_F_F_HW		0x7F	//Functional Fixed Hardware 
									//all other field in GAS_20 must be 0

//======================================================
//  GAS_20 Generic Address structure
//======================================================
typedef struct _GAS_20 {
    UINT8			AddrSpcID;          //The address space where the data structure or register exists.
                                    //Defined values are above                                            
    UINT8			RegBitWidth;		//The size in bits of the given register. 
									//When addressing a data structure, this field must be zero.
    UINT8			RegBitOffs;			//The bit offset of the given register at the given address.
                                    //When addressing a data structure, this field must be zero.
    UINT8			Reserved1;          //must be 0.
    UINT64			Address;            //The 64-bit address of the data structure or register in 
									//the given address space (relative to the processor).
} GAS_20,*PGAS_20;




//  Bits Description in FACP.FLAGS updated for ACPI 2.0
//======================================================
#define         FF_RESET_REG_S  0x00000400
#define         FF_SEALED_CASE  0x00000800
#define         FF_HEADLESS     0x00001000
#define         FF_CPU_SW_SLP   0x00002000
#define         FF_PCI_EXP_WALK 0x00004000
#define         FF_USE_PLATFORM_CLOCK 									0x00008000
#define         FF_S4_RTC_STS_VALID 										0x00010000
#define         FF_REMOTE_POWER_ON_CAPABLE 							0x00020000
#define         FF_FORCE_APIC_CLUSTER_MODEL 						0x00040000
#define         FF_FORCE_APIC_PHYSICAL_DESTINATION_MODE 0x00080000

#define facp_flags2 FF_WBINVD | FF_SLP_BUTTON | FF_PROC_C1 | FF_RESET_REG_S | FF_RTC_S4

//======================================================
//  XSDT Extended System Description Table
//======================================================
typedef	struct _XSDT_20 {
	ACPI_HDR	Header;
	UINT64			Ptrs[ACPI_RSDT_TABLE_NUM];
} XSDT_20, *PXSDT_20;

//======================================================
//  Define Preffered PM Profile (PM_PPROF offset 45)
//======================================================
#define		PM_UNSPEC		0	//Uncpecified
#define		PM_DESKTOP		1	//Desktop
#define		PM_MOBILE		2	//
#define		PM_WORKST		3
#define		PM_ENT_SVR		4
#define		PM_SOHO_SVR		5
#define		PM_APPL_PC		6

//======================================================
//  Define Preffered IA-PC Boot Architacture Falg 
//  (IAPC_BOOT_ARCH offset 109)
//======================================================
#define		IA_LEGACY		0x01	
#define		IA_8042			0x02	

//======================================================
//  FADT Fixed ACPI Description Table 
//======================================================
typedef struct _FACP_20 {
   	ACPI_HDR Header;              //0..35
   	UINT32		FIRMWARE_CTRL;       //36  Phisical memory address (0-4G) of FACS
   	UINT32		DSDT;                //40  Phisical memory address (0-4G) of DSDT
   	UINT8		Reserved1;			 //44  (Was INT_MODEL 0 Dual PIC;1 Multipy APIC) 
									 //	   can be 0 or 1 for ACPI 1.0  
   	UINT8  		PM_PPROF;			 //45  Preffered PM Profile (was Reserved)
   	UINT16		SCI_INT;             //46  SCI int Pin should be Shareble/Level/Act Low

   	UINT32		SMI_CMD;             //48  Port Addr of ACPI Command reg
   	UINT8		ACPI_ENABLE_CMD;         //52  Value to write in SMI_CMD reg
	UINT8		ACPI_DISABLE_CMD;        //53  Value to write in SMI_CMD reg    
	UINT8		S4BIOS_REQ;          //54  Value to write in SMI_CMD reg
   	UINT8		PSTATE_CNT;          //55  Was Reserved2 now 
									 //	   Value that OSPM writes in SCI_CMD to assume 
									 //	   Processor Perfomance State control responsibility
   	UINT32		PM1a_EVT_BLK;        //56
   	UINT32		PM1b_EVT_BLK;        //60
   	UINT32		PM1a_CNT_BLK;        //64
   	UINT32		PM1b_CNT_BLK;        //68
   	UINT32		PM2_CNT_BLK;         //72
   	UINT32		PM_TMR_BLK;          //76
   	UINT32		GPE0_BLK;            //80
   	UINT32		GPE1_BLK;            //84    
	UINT8		PM1_EVT_LEN;         //88
   	UINT8		PM1_CNT_LEN;         //89
   	UINT8		PM2_CNT_LEN;         //90
   	UINT8		PM_TM_LEN;           //91
   	UINT8		GPE0_BLK_LEN;        //92
   	UINT8		GPE1_BLK_LEN;        //93
   	UINT8		GPE1_BASE;           //94
   	UINT8		CST_CNT;			 //95 Was Reserved3 
   	UINT16		P_LVL2_LAT;          //96
   	UINT16		P_LVL3_LAT;          //98
   	UINT16		FLUSH_SIZE;          //100
   	UINT16		FLUSH_STRIDE;        //102
   	UINT8		DUTY_OFFSET;         //104
   	UINT8		DUTY_WIDTH;          //105
   	UINT8		DAY_ALRM;            //106
   	UINT8		MON_ALRM;            //107
   	UINT8		CENTURY;             //108
   	UINT16		IAPC_BOOT_ARCH;      //109	
	UINT8		Reserved2;           //111    
   	UINT32		FLAGS;               //112
//That was 32 bit part of a FADT Here follows 64bit part
	GAS_20			RESET_REG;			 //116
	UINT8		RESET_VAL;			 //128
	UINT8		Reserved[3];		 //129
   	UINT64		X_FIRMWARE_CTRL;     //132  Phisical memory address (0-4G) of FACS
   	UINT64		X_DSDT;              //140  Phisical memory address (0-4G) of DSDT
   	GAS_20			X_PM1a_EVT_BLK;      //148
   	GAS_20			X_PM1b_EVT_BLK;      //160
   	GAS_20			X_PM1a_CNT_BLK;      //172
   	GAS_20			X_PM1b_CNT_BLK;      //184
   	GAS_20			X_PM2_CNT_BLK;       //196
   	GAS_20			X_PM_TMR_BLK;        //208
 	GAS_20			X_GPE0_BLK;          //220
   	GAS_20			X_GPE1_BLK;          //232    
}FACP_20,*PFACP_20;

//======================================================
//  FACS Firmware ACPI Control Structure for ACPI 2.0
//======================================================
typedef struct _FACS_20 {
    UINT32		Signature;           //0
    UINT32		Length;              //4
    UINT32		HardwareSignature;   //8
    UINT32		FwWakingVector;      //12
    UINT32		GlobalLock;          //16
    UINT32		Flags;               //20
//====== ACPI 2.0 ======================================
    //UINT8       Reserved[40];      //24 Should be 0
	UINT64		X_FwWakingVector;    //24
	UINT8		Version;			 //32 1-Version of this table;
	UINT8		Reserved[31];		 //33
} FACS_20,*PFACS_20;

//======================================================
//  Bits Description in FACS.GlobalLock field. ACPI 2.0
//======================================================
#define     GL_PENDING          0x01
#define     GL_OWNED			0x20

//======================================================
//  APIC Multiple APIC Description Table Header ACPI 2.0
//======================================================
struct _APIC_20H {
	ACPI_HDR	Header;				//0..35
	UINT32			LAPIC_Address;		//36
	UINT32			Flags;				//40
} ;

//======================================================
//  Bits Description in APICXX.Type field. ACPI 2.0
//======================================================
#ifndef AT_LAPIC
#define			AT_LAPIC		0
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

#ifndef AT_LAPIC_NMI
#define			AT_LAPIC_NMI	4
#endif

#ifndef AT_LAPIC_ADR
#define			AT_LAPIC_ADR	5
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


//======================================================
//  Bits Description in LAPICXX.Flags field. ACPI 2.0
//======================================================
#define		FL_ENABLE		0x0001

//======================================================
//  NMI Structure ACPI 2.0
//======================================================
typedef	struct _NMI_20 {
	UINT8			Type;				//0
	UINT8			Length;				//1
	UINT16			Flags;				//2 same as MPS INTI flags 1.4
	UINT32			GlobalSysVect;		//4
} NMI_20,*PNMI_20;

//======================================================
//  Local APIC NMI Structure ACPI 2.0
//======================================================
typedef	struct _LNMI_20 {
	UINT8			Type;				//0
	UINT8			Length;				//1
	UINT8			CPU_ID;				//2
	UINT16			Flags;				//3 same as MPS INTI flags 1.4
	UINT8			LapicIntin;			//5
} LNMI_20,*PLNMI_20;

//======================================================
//  Local APIC Address Override Structure ACPI 2.0
// =========== Optional ================================
//======================================================
typedef	struct _LAPIC_ADDR_20 {
	UINT8			Type;				//0
	UINT8			Length;				//1
	UINT16			Reserved;			//2
	UINT64			LAPIC_Address;		//4 
} LAPIC_ADDR_20,*PLAPIC_ADDR_20;

//======================================================
//  IOSAPIC Structure  per each IOSAPIC ACPI 2.0
// =========== Optional ================================
//======================================================
typedef	struct _IOSAPIC_20 {	
	UINT8			Type;				//0
	UINT8			Length;				//1
	UINT8			IoapicId;			//2
	UINT8			Reserved;			//3
	UINT32			SysVectBase;		//8
	UINT64			IosapicAddress;		//4
} IOSAPIC_20,*PIOSAPIC_20;

//======================================================
//  LSAPIC Structure  per each CPU ACPI 2.0
// =========== Optional ================================
//======================================================
typedef	struct _LSAPIC_20 {	
	UINT8			Type;				//0
	UINT8			Length;				//1
	UINT8			CpuId;				//2
	UINT8			LsapicId;			//3
	UINT8			LsapicEid;			//4
// PO_DBG_TAS_0321
// Updates per ACPI errata spec.
    UINT8           Reserved1;          //5
    UINT8           Reserved2;          //6
    UINT8           Reserved3;          //7
// PO_DBG_TAS_0321
	UINT32			Flags;				//8 Same as MPS 1.4 
} LSAPIC_20,*PLSAPIC_20;


//======================================================
//  Platform Source Interrupt Override Structure  
//  per each INTIN
//======================================================
typedef	struct _PIS_20 {	
	UINT8			Type;				//0
	UINT8			Length;				//1
	UINT16			Flags;				//2 Same as MPS 1.4 
	UINT8			IntType;			//4
	UINT8			CpuId;				//5
	UINT8			CpuEid;				//6 
	UINT8			IoSapicVect;		//7
	UINT32			GlobalSysVect;		//8
	UINT32			Reserved;			//12
} PIS_20,*PPIS_20;

//======================================================
//  Define INT_TYPE values 
//======================================================
#define		PIT_PMI			0x0001
#define		PIT_NMI			0x0002
#define		PIT_ERR			0x0003

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//======================================================
//Stuff following below is ACPI 2.0 new tables 
//======================================================
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//======================================================
//  ECDT Embeded Controller Boot Resource Table ACPI 2.0
//======================================================
typedef struct _ECDT_20 {
	ACPI_HDR	Header;
	GAS_20				EC_CONTROL;
	GAS_20				EC_DATA;
	UINT32			UID;
	UINT8			GPE_BIT;
	UINT8			EC_ID[128]; //string that specify EC (\_SB.PCI0.ISA.EC)
} ECDT_20,*PECDT_20;


//======================================================
//  SRAT Static Resource Affinity Table 
//======================================================
typedef struct _SRAT_H_20 {
	ACPI_HDR		Header;
	UINT32			TBL_REV; //this table revision=1
	UINT8			Reserved1;
} SRAT_H_20,*PSRAT_H_20;

//======================================================
//  IPPT Table --  IA64 Platform Properties Table
//======================================================
typedef struct _IPPT_20 {
    ACPI_HDR	   	Header;
    UINT32          Flags;
    UINT32          Reserved[3];
} IPPT_20, *PIPPT_20;

#define IPPT_DIS_WRITE_COMBINING    BIT0
#define IPPT_ENA_CROSS_PART_IPI     BIT1
#define IPPT_DIS_PTCG_TB_FLUSH      BIT2
#define IPPT_DIS_UC_MAIN_MEMORY     BIT3

//======================================================
//  SPCR Data Definitions 
//======================================================
//Interface Type Definitions
#define SER_IF_16550        0
#define SER_IF_16450        1
//interrupt Type Bits...
#define SER_IT_8259         BIT0
#define SER_IT_APIC         BIT1
#define SER_IT_SAPIC        BIT2
//Baud Rate Definitions
#define SER_BR_9600         3
#define SER_BR_19200        4
#define SER_BR_57600        6
#define SER_BR_115200       7
//Flow Controll Definitions
#define SER_FC_DCD          BIT0
#define SER_FC_RTS_CTS      BIT1
#define SER_FC_XON_XOF      BIT2
//Terminal Protocol Type Definitions
#define SER_TT_VT100        0
#define SER_TT_VT100P       1
#define SER_TT_VT_UTF8      2   
#define SER_TT_ANCI         3


//======================================================
//  SPCR Serial Port Console Redirection Table 
//======================================================
typedef struct _SPCR_20 {
   ACPI_HDR Header;              //0..35
   UINT8		InterfaceType;          //36 0/1 16550/16450 interface type
   UINT8		Reserved[3];            //37..39 Reserved  
   GAS_20			BaseAddress;			//40..51 Address of the redirection serial port
   UINT8       	InterruptType;          //52 System Interrupt modes supported
                                    //  bit[0] = dual 8259
                                    //  bit[1] = I/O APIC
                                    //  bit[2] = I/O SAPIC
                                    //  bit[3..7] = Reserved, must be 0
   UINT8       Irq;                    //53 PIC mode IRQ for this serial port
   UINT32      GlobalSystemInt;        //54..57 (S)APIC mode IRQ for this serial port
   UINT8       BaudRate;               //58 Baud Rate of this serial port
                                    //  3 = 9600
                                    //  4 = 19200
                                    //  6 = 57600
                                    //  7 = 115200
   UINT8       Parity;                 //59 0 = No Parity
   UINT8       StopBits;               //60 1 = 1 Stop Bit
   UINT8       FlowControl;            //61 Flow Control
                                    //  bit[0] = DCD Required
                                    //  bit[1] = RTS/CTS
                                    //  bit[2] = XON/XOFF
                                    //  bit[3..7] = Reserved, must be 0
   UINT8       TerminalType;           //62 Terminal Protocol
                                    //  0 = VT100
                                    //  1 = VT100+
                                    //  2 = VT-UTF8
                                    //  3 = ANSI
   UINT8       Reserved1;              //63 Must be 0
   UINT16      PciDeviceId;            //64 Must be 0xFFFF if not a PCI Device
   UINT16      PciVendorId;            //66 Must be 0xFFFF if not a PCI Device
   UINT8       PciBusNumber;           //68 Must be 0 if not a PCI Device
   UINT8       PciDeviceNumber;        //69 Must be 0 if not a PCI Device
   UINT8       PciFunctionNumber;      //70 Must be 0 if not a PCI Device
   UINT32      PciFlags;               //71 Must be 0 if not a PCI Device
   UINT8       PciSegment;             //75 For systems with less than 255 PCI Busses, this must be 0
   UINT32      Reserved4;              //76 Must be 0
  
}SPCR_20,*PSPCR_20;

//-------------------------------------------------------
//Additional MCFG TAble

typedef struct _MCFG_20 {
   	ACPI_HDR 	Header;     //0..35
	UINT64			Reserved;	//Bits 1-31 are reserved in ACPI 2.0
	UINT64			BaseAddr;	//Base address of 256MB extended config space
	UINT16			PciSeg; 	//Segment # of PCI Bus
	UINT8			StartBus; 	//Start bus number of PCI segment
	UINT8			EndBus; 	//End bus number of PCI segment
	UINT32			Reserved1; 	//Reserved DWORD
} MCFG_20;




//-------------------------------------------------------
//Additional HPET Table and data structures forit
typedef union _HPET_CAP_BLK {
	UINT32		TMR_BLK_ID;
	struct	{
		UINT32	HwRevId		:	8;	//0..7
		UINT32	BlkTmrNum	:	5; 	//8..12 Number of Timers In this Block 
		UINT32	Cntr64Bit	:	1; 	//13
		UINT32	Reserved	:	1;	//14
		UINT32	LegacyRout	:	1;	//15
		UINT32	VendorID	: 	16; //16..32
	};
} HPET_CAP_BLK;

typedef struct _HPET_20 {
   	ACPI_HDR 	Header;     	//0..35
	HPET_CAP_BLK 	EvtTmrBlockId;	
	GAS_20				BaseAddress;	
	UINT8			HpetNumber;
	UINT16			MinTickPeriod;
	UINT8			PageProtOem;
} HPET_20;

#pragma pack(pop)

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
