//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
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
// $Header: /Alaska/BIN/Core/Include/PCI.h 13    8/17/11 12:21p Yakovlevs $
//
// $Revision: 13 $
//
// $Date: 8/17/11 12:21p $
//
//**********************************************************************
// Revision History
// ----------------
//$Log: /Alaska/BIN/Core/Include/PCI.h $
// 
// 13    8/17/11 12:21p Yakovlevs
// [TAG]  		EIP67406
// [Category]  	Bug Fix
// [Symptom]  	LEGACY_OPT_ROM_HEADER structure doesn't match to PCI
// specification
// [RootCause]  	Typo
// [Solution]  	Correct the typo
// [Files]  		Pci.h
// 
// 12    8/07/09 2:57p Rameshr
// Sd card definition added
// 
// 11    7/17/07 5:01p Yakovlevs
// Common PCI Header Offset Definitions added
// 
// 10    7/17/07 5:00p Yakovlevs
// 
// 9     3/15/07 3:10p Yakovlevs
// Changed PCI_PME register block Field NAMES; made all data pack(1) here
// 
// 8     3/15/07 3:09p Yakovlevs
// File Headr updated
//
//**********************************************************************

#ifndef _PCI22_H
#define _PCI22_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>

#define PCI_MAX_SEGMENT 	255

#define PCI_MAX_BUS     	255

#define PCI_MAX_DEVICE  	31
#define PCI_MAX_FUNC    	7

#define PCI_DEV_ROM_BAR		0x30
#define PCI_P2P_ROM_BAR 	0x38

#define MAX_PCI_BAR			6

#define P2P_BRG_MEM_GRA 	0xFFFFF //naturally, P2P Brg MMIO aligned to the 1M boundary
#define P2P_BRG_IO_GRA		0xFFF	//naturally, P2P Brg IO aligned to the 4k boundary

//Here comes some Common PCI Header Offset Definitions.
#define PCI_VID             0x0000        // Vendor ID Register
#define PCI_DID             0x0002        // Device ID Register
#define PCI_CMD             0x0004        // PCI Command Register
#define PCI_STS             0x0006        // PCI Status Register
#define PCI_RID             0x0008        // Revision ID Register
#define PCI_IFT             0x0009        // Interface Type
#define PCI_SCC             0x000A        // Sub Class Code Register
#define PCI_BCC             0x000B        // Base Class Code Register
#define PCI_CLS             0x000C        // Cache Line Size
#define PCI_PMLT            0x000D        // Primary Master Latency Timer
#define PCI_HDR             0x000E        // Header Type Register
#define PCI_BIST            0x000F        // Built in Self Test Register
#define PCI_BAR0            0x0010        // Base Address Register 0
#define PCI_BAR1            0x0014        // Base Address Register 1
#define PCI_BAR2            0x0018        // Base Address Register 2
#define PCI_PBUS            0x0018        // Primary Bus Number Register
#define PCI_SBUS            0x0019        // Secondary Bus Number Register
#define PCI_SUBUS           0x001A        // Subordinate Bus Number Register
#define PCI_SMLT            0x001B        // Secondary Master Latency Timer
#define PCI_BAR3            0x001C        // Base Address Register 3
#define PCI_IOBASE          0x001C        // I/O base Register
#define PCI_IOLIMIT         0x001D        // I/O Limit Register
#define PCI_SECSTATUS       0x001E        // Secondary Status Register
#define PCI_BAR4            0x0020        // Base Address Register 4
#define PCI_MEMBASE         0x0020        // Memory Base Register
#define PCI_MEMLIMIT        0x0022        // Memory Limit Register
#define PCI_BAR5            0x0024        // Base Address Register 5
#define PCI_PRE_MEMBASE     0x0024        // Prefetchable memory Base register
#define PCI_PRE_MEMLIMIT    0x0026        // Prefetchable memory Limit register
#define PCI_PRE_MEMBASE_U   0x0028        // Prefetchable memory base upper 32 bits
#define PCI_PRE_MEMLIMIT_U  0x002C        // Prefetchable memory limit upper 32 bits
#define PCI_SVID            0x002C        // Subsystem Vendor ID
#define PCI_SID             0x002E        // Subsystem ID
#define PCI_IOBASE_U        0x0030        // I/O base Upper Register
#define PCI_IOLIMIT_U       0x0032        // I/O Limit Upper Register
#define PCI_CAPP            0x0034        // Capabilities Pointer
#define PCI_EROM            0x0038        // Expansion ROM Base Address
#define PCI_INTLINE         0x003C        // Interrupt Line Register
#define PCI_INTPIN          0x003D        // Interrupt Pin Register
#define PCI_MAXGNT          0x003E        // Max Grant Register
#define PCI_BRIDGE_CNTL     0x003E        // Bridge Control Register
#define PCI_MAXLAT          0x003F        // Max Latency Register

// Command
#define PCI_VGA_PALETTE_SNOOP_DISABLED   0x20

#pragma pack(push, 1)

//Common part of PCI Devices
typedef struct {
    UINT16      VendorId;
    UINT16      DeviceId;
    UINT16      CmmandReg;
    UINT16      StatusReg;
    UINT8       RevisionID;
    UINT8       ClassCode[3];
    UINT8       CacheLineSize;
    UINT8       LatencyTmr;
    UINT8       HeaderType;
    UINT8       BIST;
} PCI_COMMON_REG;

//Standard PCI Device
typedef struct {
    UINT32      Bar[6];
    UINT32      CardBusCISPtr;
    UINT16      SubsystemVendorID;
    UINT16      SubsystemID;
    UINT32      ExpansionRomBar;
    UINT8       CapabilityPtr;
    UINT8       Reserved[7];
    UINT8       InterruptLine;
    UINT8       InterruptPin;
    UINT8       MinGnt;
    UINT8       MaxLat;
} PCI_DEVICE_REG;

typedef struct {
    PCI_COMMON_REG		Header;
    PCI_DEVICE_REG		Device;
} PCI_STD_DEVICE;

//PCI 2 PCI Bridge
typedef struct {
    UINT32      Bar[2];
    UINT8       PrimaryBus;
    UINT8       SecondaryBus;
    UINT8       SubordinateBus;
    UINT8       SecondaryLatencyTimer;
    UINT8       IoBase;
    UINT8       IoLimit;
    UINT16      SecondaryStatus;
    UINT16      MemoryBase;
    UINT16      MemoryLimit;
    UINT16      PfMemoryBase;
    UINT16      PfMemoryLimit;
    UINT32      PfBaseUpper32;
    UINT32      PfLimitUpper32;
    UINT16      IoBaseUpper16;
    UINT16      IoLimitUpper16;
    UINT8       CapabilityPtr;
    UINT8       Reserved[3];
    UINT32      ExpansionRomBAR;
    UINT8       InterruptLine;
    UINT8       InterruptPin;
    UINT16      BridgeControl;
} PCI_P2P_BRG_REG;

typedef struct {
    PCI_COMMON_REG		Header;
    PCI_P2P_BRG_REG     Bridge;
} PCI_P2P_BRIDGE;

//PCI 2 CardBUS BRIDGE
typedef struct {
  UINT32        CardBusSocketReg;          // Cardus Socket/ExCA Base
                                           //Address Register
  UINT8			CapabilityPtr;
  UINT8         Reserved;
  UINT16        SecondaryStatus;           // Secondary Status
  UINT8         PciBusNumber;              // PCI Bus Number
  UINT8         CardBusBusNumber;          // CardBus Bus Number
  UINT8         SubordinateBusNumber;      // Subordinate Bus Number
  UINT8         CardBusLatencyTimer;       // CardBus Latency Timer
  UINT32        MemoryBase0;               // Memory Base Register 0
  UINT32        MemoryLimit0;              // Memory Limit Register 0
  UINT32        MemoryBase1;
  UINT32        MemoryLimit1;
  UINT32        IoBase0;
  UINT32        IoLimit0;                  // I/O Base Register 0
  UINT32        IoBase1;                   // I/O Limit Register 0
  UINT32        IoLimit1;
  UINT8         InterruptLine;             // Interrupt Line
  UINT8         InterruptPin;              // Interrupt Pin
  UINT16        BridgeControl;             // Bridge Control
} PCI_P2C_BRG_REG;

typedef struct {
    PCI_COMMON_REG		Header;
    PCI_P2C_BRG_REG     Bridge;
} PCI_P2C_BRIDGE;


// Definitions of PCI class bytes and manipulation macros.
#define PCI_CL_OLD						0x00
#define PCI_CL_OLD_SCL_VGA				0x01

#define PCI_CL_MASS_STOR				0x01
#define PCI_CL_MASS_STOR_SCL_SCSI		0x00
#define PCI_CL_MASS_STOR_SCL_IDE		0x01
#define PCI_CL_MASS_STOR_SCL_FDC		0x02
#define PCI_CL_MASS_STOR_SCL_IPI		0x03
#define PCI_CL_MASS_STOR_SCL_RAID		0x04
#define PCI_CL_MASS_STOR_SCL_OTHER		0x80

#define PCI_CL_NETWORK					0x02
#define PCI_CL_NETWORK_SCL_ETHERNET		0x00
#define PCI_CL_NETWORK_SCL_TOK_RING		0x01
#define PCI_CL_NETWORK_SCL_FDDI			0x02
#define PCI_CL_NETWORK_SCL_ATM			0x03
#define PCI_CL_NETWORK_SCL_ISDN			0x04
#define PCI_CL_NETWORK_SCL_OTHER		0x80

#define PCI_CL_DISPLAY					0x03
#define PCI_CL_DISPLAY_SCL_VGA          0x00
#define PCI_CL_DISPLAY_SCL_XGA          0x01
#define PCI_CL_DISPLAY_SCL_3D           0x02
#define PCI_CL_DISPLAY_SCL_OTHER        0x80

#define PCI_CL_MMEDIA					0x04
#define PCI_CL_MMEDIA_SCL_VIDEO			0x00
#define PCI_CL_MMEDIA_SCL_AUDIO			0x01
#define PCI_CL_MMEDIA_SCL_PHONE			0x02
#define PCI_CL_MMEDIA_SCL_OTHER			0x80

#define PCI_CL_MEMORY					0x05
#define PCI_CL_MEMORY_SCL_RAM			0x00
#define PCI_CL_MEMORY_SCL_FLASH			0x01
#define PCI_CL_MEMORY_SCL_OTHER			0x80

#define PCI_CL_BRIDGE					0x06
#define PCI_CL_BRIDGE_SCL_HOST          0x00
#define PCI_CL_BRIDGE_SCL_ISA           0x01
#define PCI_CL_BRIDGE_SCL_EISA          0x02
#define PCI_CL_BRIDGE_SCL_MCA           0x03
#define PCI_CL_BRIDGE_SCL_P2P           0x04
#define PCI_CL_BRIDGE_SCL_PCMCIA        0x05
#define PCI_CL_BRIDGE_SCL_NUBUS         0x06
#define PCI_CL_BRIDGE_SCL_CARDBUS       0x07
#define PCI_CL_BRIDGE_SCL_RACEWAY       0x08
#define PCI_CL_BRIDGE_SCL_OTHER			0x80

#define PCI_CL_COMM						0x07
#define PCI_CL_COMM_CSL_SERIAL			0x00
#define PCI_CL_COMM_CSL_PARALLEL		0x01
#define PCI_CL_COMM_CSL_MULTISERIAL		0x02
#define PCI_CL_COMM_CSL_MODEM			0x03
#define PCI_CL_COMM_CSL_OTHER			0x80

#define PCI_CL_SYSTEM_PERIPHERALS		0x08
#define PCI_CL_SYSTEM_PERIPHERALS_SCL_SD	0x05

#define PCI_CL_SER_BUS	                0x0C
#define PCI_CL_SER_BUS_SCL_FIREWIRE     0x00
#define PCI_CL_SER_BUS_SCL_ACCESS       0x01
#define PCI_CL_SER_BUS_SCL_SSA          0x02
#define PCI_CL_SER_BUS_SCL_USB          0x03
#define PCI_CL_SER_BUS_SCL_FIBCHAN		0x04
#define PCI_CL_SER_BUS_SCL_SMB          0x05

#define PCI_CL_I2O		                0x0E

//Some Class codes are not listed here.  Use 
// MdePkg/Include/IndustryStandard/Pci.h instead
#define HDR_TYPE_DEVICE		            0x00
#define HDR_TYPE_P2P_BRG				0x01
#define HDR_TYPE_P2C_BRG				0x02

#define HDR_TYPE_MULTIFUNC				0x80

#define PCI_MAX_BAR_NO					6
#define PCI_MAX_CFG_OFFS				0x100


#define PCI_COMMAND_REGISTER_OFFSET        0x04
#define PCI_STATUS_REGISTER_OFFSET         0x06
#define PCI_BRIDGE_CONTROL_REGISTER_OFFSET 0x3E
#define PCI_BRIDGE_STATUS_REGISTER_OFFSET  0x1E

#define PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET      0x18
#define PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET    0x19
#define PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET  0x1a

typedef struct {
    UINT8   Register;
    UINT8   Function;
    UINT8   Device;
    UINT8   Bus;
    UINT8   Reserved[4];
} EFI_PCI_ADDR;

#ifndef _PCI22_H_
typedef struct {
    UINT32  Reg     : 8;
    UINT32  Func    : 3;
    UINT32  Dev     : 5;
    UINT32  Bus     : 8;
    UINT32  Reserved: 7;
    UINT32  Enable  : 1;
} PCI_CONFIG_ACCESS_CF8;
#endif //_PCI22_H_

#define PCI_REV_ID_OFFSET						0x08
#define PCI_PI_OFFSET							0x09

// Following are the PCI-CARDBUS bridge control bit     
#define PCI_STD_OPT_ROM_SIGNATURE				0xaa55
#define PCI_EFI_OPT_ROM_SIGNATURE				0x0EF1
#define PCI_CODE_TYPE_PCAT_IMAGE				0x00
#define PCI_CODE_TYPE_EFI_IMAGE					0x03
#define PCI_EFI_OPT_ROM_SIGNATURE_COMPRESSED    0x0001

// Following are the PCI-COMMAND REG control bit     
#define PCI_CMD_IO_SPACE                        0x0001
#define PCI_CMD_MEMORY_SPACE                    0x0002
#define PCI_CMD_BUS_MASTER                      0x0004
#define PCI_CMD_SPECIAL_CYCLE                   0x0008
#define PCI_CMD_MEMORY_WRITE_AND_INVALIDATE     0x0010
#define PCI_CMD_VGA_PALETTE_SNOOP               0x0020
#define PCI_CMD_PARITY_ERROR_RESPOND            0x0040
#define PCI_CMD_STEPPING_CONTROL                0x0080
#define PCI_CMD_SERR                            0x0100
#define PCI_CMD_FAST_BACK_TO_BACK               0x0200
#define PCI_CMD_INTERRUPT_DISABLE               0x0400

typedef union {
	UINT16 		CMD_REG_ALL;
	struct{
		UINT16	IO_SPACE 		: 1; //bit0
		UINT16	MEM_SPACE 		: 1; //bit1
		UINT16	BUS_MASTER		: 1; //bit2
		UINT16	SP_CYCLE		: 1; //bit3
		UINT16	MEM_WI_EN		: 1; //bit4
		UINT16	VGA_PALETTE		: 1; //bit5
		UINT16	PARITY_ERR		: 1; //bit6
		UINT16	STEP_CTR		: 1; //bit7
		UINT16	SERR_ENA		: 1; //bit8
		UINT16	FAST_B2B_ENA	: 1; //bit9
		UINT16	reserved		: 6; //bit10-15
	};
} PCI_CMD_REG_BITS;

// Following are the PCI-2-PCI  bridge control bit     
#define P2P_BRG_CONTROL_PARITY_ERROR_RESPONSE    0x0001
#define P2P_BRG_CONTROL_SERR                     0x0002
#define P2P_BRG_CONTROL_ISA                      0x0004
#define P2P_BRG_CONTROL_VGA                      0x0008
#define P2P_BRG_CONTROL_VGA_16                   0x0010
#define P2P_BRG_CONTROL_MASTER_ABORT             0x0020
#define P2P_BRG_CONTROL_RESET_SECONDARY_BUS      0x0040
#define P2P_BRG_CONTROL_FAST_BACK_TO_BACK        0x0080
#define P2P_BRG_CONTROL_PRIMARY_DISCARD_TIMER    0x0100
#define P2P_BRG_CONTROL_SECONDARY_DISCARD_TIMER  0x0200
#define P2P_BRG_CONTROL_TIMER_STATUS             0x0400
#define P2P_BRG_CONTROL_DISCARD_TIMER_SERR       0x0800
                                                       
// Following are the PCI-2-CARDBUS bridge control bit     
#define P2C_BRG_CONTROL_IREQINT_ENABLE           0x0080
#define P2C_BRG_CONTROL_RANGE0_MEMORY_TYPE       0x0100
#define P2C_BRG_CONTROL_RANGE1_MEMORY_TYPE       0x0200
#define P2C_BRG_CONTROL_WRITE_POSTING_ENABLE     0x0400
                                                       
// Following are the PCI status control bit             
#define PCI_STS_CAPABILITY                       0x0010
#define PCI_STS_66MZ_CAPABLE                     0x0020
#define PCI_STS_FAST_BACK_TO_BACK				 0x0080
#define PCI_STS_MASTER_DATA_PARITY_ERROR         0x0100
 

#define EFI_PCI_CAPABILITY_PTR                          0x34
#define EFI_PCI_CARDBUS_BRIDGE_CAPABILITY_PTR           0x14

//Some signature definitions
#define PCI_OPT_ROM_SIG		0xAA55			
#define PCI_OPT_ROM_EFISIG	0x0EF1
#define PCI_PCIR_SIG		0x52494350 		//"PCIR"

//EFI subsystem definitions
#define EFI_IMAGE_BS_DRIVER	11
#define EFI_IMAGE_RT_DRIVER 12

typedef struct {
    UINT16          Signature;              // 0xaa55
    UINT8           Reserved[0x16];
    UINT16          PcirOffset;
} PCI_STD_OPT_ROM_HEADER;

typedef struct {
    UINT16          Signature;              // 0xaa55
    UINT16          InitializationSize;
    UINT32          EfiSignature;           // 0x0EF1
    UINT16          EfiSubsystem;
    UINT16          EfiMachineType;
    UINT16          CompressionType;
    UINT8           Reserved[8];
    UINT16          EfiImageOffset;
    UINT16          PcirOffset;
} PCI_EFI_OPT_ROM_HEADER;

typedef struct {
    UINT16          Signature;              // 0xaa55
    UINT8           Size512;
    UINT8           Reserved[0x15];
    UINT16          PcirOffset;
} LEGACY_OPT_ROM_HEADER;

#ifndef _PCI22_H_
typedef union {
	UINT8                           *Raw;
	PCI_STD_OPT_ROM_HEADER          *Generic;
	PCI_EFI_OPT_ROM_HEADER			*Efi;
	LEGACY_OPT_ROM_HEADER			*PcAt;
}EFI_PCI_ROM_HEADER;


typedef struct {
    UINT32          Signature;              // "PCIR"
    UINT16          VendorId;
    UINT16          DeviceId;
    UINT16          Reserved0;
    UINT16          Length;
    UINT8           Revision;
    UINT8           ClassCode[3];
    UINT16          ImageLength;
    UINT16          CodeRevision;
    UINT8           CodeType;
    UINT8           Indicator;
    UINT16          Reserved1;
} PCI_DATA_STRUCTURE;
#endif //_PCI22_H_


// PCI Capability List IDs and records                                                        
#define PCI_CAP_ID_PMI			0x01
#define PCI_CAP_ID_AGP			0x02
#define PCI_CAP_ID_VPD			0x03
#define PCI_CAP_ID_SLOTID		0x04
#define PCI_CAP_ID_MSI			0x05
#define PCI_CAP_ID_HOTSWAP		0x06
#define PCI_CAP_ID_PCIX			0x07
#define PCI_CAP_ID_PCIEXP		0x10
#define PCI_CAP_ID_HOTPLUG		0x0C		

#ifndef _PCI22_H_
typedef union {
	UINT16			CAP_HDR;
    struct {
		UINT8		CapabilityID;
   		UINT8		NextItemPtr;
	};
} EFI_PCI_CAPABILITY_HDR;
#endif //_PCI22_H_

// Capability EFI_PCI_CAPABILITY_ID_PMI
//Derfinitions for Power Management Interface
typedef union _PCI_PM_CAP_REG {
	UINT16		PMI_CAP_REG;
	struct {
		UINT16	Version 	: 	3; 	//16,17,18
		UINT16  PmeClock	: 	1; 	//19
		UINT16	Reserved	:	1;	//20
		UINT16  DevSpecInit	:	1;	//21
		UINT16	AuxCurrent	:	3;	//22,23,24 
		UINT16	D1Support	:	1;	//25
		UINT16	D2Support	:	1;	//26
		UINT16	PmeSupport	:	5;	//27,28,29,30,31
	};
} PCI_PM_CAP_REG;

//Power Mamagement Control/Status Reg
typedef union _PCI_PM_CTRSTS_REG {
	UINT16		PMI_CTRSTS_REG;
	struct {
		UINT16	PowerState 	: 	2; 	//00,01
		UINT16	Reserved	:	6;	//02,03,04,05,06,07
		UINT16  PmeEnable	:	1;	//08
		UINT16	DataSelect	:	4;	//09,10,11,12 
		UINT16	DataScale	:	2;	//13,14
		UINT16	PmeStatus	:	1;	//15
	};
} PCI_PM_CTRSTS_REG;

#ifndef _PCI22_H_
typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    PCI_PM_CAP_REG         	PmCapReg;
    PCI_PM_CTRSTS_REG		PmStaCtlReg;
    UINT8                   BridgeExtention;
    UINT8                   Data;
} EFI_PCI_CAPABILITY_PMI;

// Capability EFI_PCI_CAPABILITY_ID_AGP
typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    UINT8                   Rev;
    UINT8                   Reserved;
    UINT32                  Status;
    UINT32                  Command;
} EFI_PCI_CAPABILITY_AGP;


// Capability EFI_PCI_CAPABILITY_ID_VPD
//Vital Poroduct Data
typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    UINT16                  AddrReg;
    UINT32                  DataReg;
} EFI_PCI_CAPABILITY_VPD;


// Capability EFI_PCI_CAPABILITY_ID_SLOTID
typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    UINT8                   ExpnsSlotReg;
    UINT8                   ChassisNo;
} EFI_PCI_CAPABILITY_SLOTID;


// Capability EFI_PCI_CAPABILITY_ID_MSI
typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    UINT16                  MsgCtrlReg;
    UINT32                  MsgAddrReg;
    UINT16                  MsgDataReg;
} EFI_PCI_CAPABILITY_MSI32;

typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    UINT16                  MsgCtrlReg;
    UINT32                  MsgAddrRegLsdw;
    UINT32                  MsgAddrRegMsdw;
    UINT16                  MsgDataReg;
} EFI_PCI_CAPABILITY_MSI64;


// Capability EFI_PCI_CAPABILITY_ID_HOTPLUG
typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    // not finished - fields need to go here
} EFI_PCI_CAPABILITY_HOTPLUG;
#endif //_PCI22_H_

#ifndef _PCI23_H_
// Capability EFI_PCI_CAPABILITY_ID_PCIX
typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    UINT16                  CommandReg;
    UINT32                  StatusReg;
} EFI_PCI_CAPABILITY_PCIX;

typedef struct {
    EFI_PCI_CAPABILITY_HDR  Hdr;
    UINT16                  SecStatusReg;
    UINT32                  StatusReg;
    UINT32                  SplitTransCtrlRegUp;
    UINT32                  SplitTransCtrlRegDn;
} EFI_PCI_CAPABILITY_PCIX_BRDG;
#endif //_PCI23_H_

#pragma pack (pop)

#ifndef _PCI22_H_
#define DEVICE_ID_NOCARE        0xFFFF

#define PCI_ACPI_UNUSED         0
#define PCI_BAR_NOCHANGE        0
#define PCI_BAR_OLD_ALIGN       0xFFFFFFFFFFFFFFFF 
#define PCI_BAR_EVEN_ALIGN      0xFFFFFFFFFFFFFFFE       
#define PCI_BAR_SQUAD_ALIGN     0xFFFFFFFFFFFFFFFD  
#define PCI_BAR_DQUAD_ALIGN     0xFFFFFFFFFFFFFFFC


#define PCI_BAR_IDX0            0x00
#define PCI_BAR_IDX1            0x01
#define PCI_BAR_IDX2            0x02
#define PCI_BAR_IDX3            0x03
#define PCI_BAR_IDX4            0x04
#define PCI_BAR_IDX5            0x05
#define PCI_BAR_ALL             0xFF
#endif //_PCI22_H_

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif
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
