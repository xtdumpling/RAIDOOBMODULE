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
	Definitions introduced in ACPI 5.0 
*/
#ifndef __ACPI50__H__
#define __ACPI50__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Acpi30.h>
#pragma	pack(push, 1)
#define FPDT_SIG 0x54445046  //"FPDT" Firmware Performance Data Table
#define EFI_FPDT_VARIABLE   L"FPDT_Volatile"

typedef	struct _EFI_FPDT_STRUCTURE {
    EFI_PHYSICAL_ADDRESS    S3Pointer;
    EFI_PHYSICAL_ADDRESS    BasBootPointer;
    UINT64                  NanoFreq;
} EFI_FPDT_STRUCTURE;

typedef	struct _FPDT_PERF_RECORD {	
	UINT16			        PerfRecType;		//0
	UINT8			        Length;				//2
	UINT8			        Revision;			//3
	UINT32                  Reserved;           //4
    EFI_PHYSICAL_ADDRESS    Pointer;            //8
} FPDT_PERF_RECORD,*PFPDT_PERF_RECORD;

typedef struct _FPDT_50 {
	ACPI_HDR		    Header;
	FPDT_PERF_RECORD    BasS3Rec; 
    FPDT_PERF_RECORD    BasBootRec; 		
} FPDT_50,*PFPDT_50;

typedef	struct _PERF_TAB_HEADER {	
	UINT32			        Signature;		    //0
	UINT32			        Length;			    //4        
} PERF_TAB_HEADER,*PPERF_TAB_HEADER;

typedef	struct _PERF_REC_HEADER {	
	UINT16			        PerfRecType;		//0
	UINT8			        RecLength;			//2
    UINT8                   Revision;           //3
} PERF_REC_HEADER,*PPERF_REC_HEADER;

typedef struct _BASIC_S3_RESUME_PERF_REC {
	PERF_REC_HEADER			Header;             //0
	UINT32			        ResumeCount;		//4
	UINT64			        FullResume;			//8
    UINT64                  AverageResume;      //16
} BASIC_S3_RESUME_PERF_REC,*PBASIC_S3_RESUME_PERF_REC;

typedef struct _BASIC_S3_SUSPEND_PERF_REC {
	PERF_REC_HEADER			Header;             //0
	UINT64			        SuspendStart;		//4
	UINT64			        SuspendEnd;			//12
} BASIC_S3_SUSPEND_PERF_REC,*PBASIC_S3_SUSPEND_PERF_REC;

typedef struct _BASIC_BOOT_PERF_REC {
	PERF_REC_HEADER			Header;                     //0
	UINT32			        Reserved;		            //4
	UINT64			        ResetEnd;			        //8
    UINT64                  OsLoaderLoadImageStart;     //16
    UINT64                  OsLoaderStartImageStart;    //24
    UINT64                  ExitBootServicesEntry;      //32
    UINT64                  ExitBootServicesExit;       //40
} BASIC_BOOT_PERF_REC,*PBASIC_BOOT_PERF_REC;

//======================================================
//  FADT Fixed ACPI Description Table
//======================================================
typedef struct _FACP_50 {
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
   	GAS_30	    X_PM1a_EVT_BLK;      //148
   	GAS_30		X_PM1b_EVT_BLK;      //160
   	GAS_30		X_PM1a_CNT_BLK;      //172
   	GAS_30		X_PM1b_CNT_BLK;      //184
   	GAS_30		X_PM2_CNT_BLK;       //196
   	GAS_30		X_PM_TMR_BLK;        //208
 	GAS_30		X_GPE0_BLK;          //220
   	GAS_30		X_GPE1_BLK;          //232
    GAS_30		SLEEP_CONTROL_REG;   //244
    GAS_30		SLEEP_STATUS_REG;    //256

}FACP_50,*PFACP_50;

#pragma pack(pop)
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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
