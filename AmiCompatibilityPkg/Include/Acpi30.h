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
#ifndef  _ACPI30H   //To Avoid this header get compiled twice
#define  _ACPI30H

#include	<Efi.h>
#include	<Token.h>
#include <Acpi20.h>

#pragma	pack(1)

typedef struct _GAS_30 {
    UINT8			AddrSpcID;          //The address space where the data structure or register exists.
                                    //Defined values are above                                            
    UINT8			RegBitWidth;		//The size in bits of the given register. 
									//When addressing a data structure, this field must be zero.
    UINT8			RegBitOffs;			//The bit offset of the given register at the given address.
                                    //When addressing a data structure, this field must be zero.
    UINT8			AccessSize;          
    UINT64			Address;            //The 64-bit address of the data structure or register in 
									//the given address space (relative to the processor).
} GAS_30,*PGAS_30;


//======================================================
//  FADT Fixed ACPI Description Table 
//======================================================
typedef struct _FACP_30 {
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
	GAS_30			RESET_REG;			 //116
	UINT8		RESET_VAL;			 //128
	UINT8		Reserved[3];		 //129
   	UINT64		X_FIRMWARE_CTRL;     //132  Phisical memory address (0-4G) of FACS
   	UINT64		X_DSDT;              //140  Phisical memory address (0-4G) of DSDT
   	GAS_30			X_PM1a_EVT_BLK;      //148
   	GAS_30			X_PM1b_EVT_BLK;      //160
   	GAS_30			X_PM1a_CNT_BLK;      //172
   	GAS_30			X_PM1b_CNT_BLK;      //184
   	GAS_30			X_PM2_CNT_BLK;       //196
   	GAS_30			X_PM_TMR_BLK;        //208
 	GAS_30			X_GPE0_BLK;          //220
   	GAS_30			X_GPE1_BLK;          //232    
}FACP_30,*PFACP_30;


//======================================================
//  SPCR Serial Port Console Redirection Table 
//======================================================
typedef struct _SPCR_30 {
   ACPI_HDR Header;              //0..35
   UINT8		InterfaceType;          //36 0/1 16550/16450 interface type
   UINT8		Reserved[3];            //37..39 Reserved  
   GAS_30			BaseAddress;			//40..51 Address of the redirection serial port
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
  
}SPCR_30,*PSPCR_30;

typedef struct _HPET_30 {
   	ACPI_HDR 	Header;     	//0..35
	HPET_CAP_BLK 	EvtTmrBlockId;	
	GAS_30				BaseAddress;	
	UINT8			HpetNumber;
	UINT16			MinTickPeriod;
	UINT8			PageProtOem;
} HPET_30;

#pragma pack()

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
