//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.07
//    Bug Fix: Fix POST hang up when set MMCFG Base to 3G.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : May/17/2017
//
//  Rev. 1.06
//    Bug Fix: Fix Early Graphic Logo malfunction for DP platform.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Mar/15/2017
//
//  Rev. 1.05
//    Bug Fix: Early Graphic Logo Support.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Mar/14/2017
//
//  Rev. 1.04 
//    Bug Fix: Enhance SMC debug card function code.
//    Reason : None
//    Auditor: Jacker Yeh
//    Date   : Jan/21/2017
//
//  Rev. 1.03
//    Bug Fix: Add SMC Debug card support
//    Reason : 
//    Auditor: Jacker Yeh
//    Date   : Oct/19/2016
//
//  Rev. 1.02
//    Bug Fix: Turn on Early-Video in earlier PEI.
//    Reason : To sync with Grantley
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//
//  Rev. 1.01
//    Bug Fix: Implement SMC_SIMPLETEXTOUT_PROTOCOL.
//    Reason : Use the protocol instead of the previous lib.
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/04/2014
//
//****************************************************************************
//****************************************************************************

#include <Token.h>
#include <AmiLib.h>
#include <AmiCspLib.h>
#include <Pi/PiStatusCode.h>
#include <AmiStatusCodes.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Library\PciLib.h>
#include <Library\IoLib.h>
#include <PeiSimpleTextOutPpi.h>
#include <Ppi/ReadOnlyVariable2.h>


#define STATUS_CODE_TYPE(Type) ((Type)&EFI_STATUS_CODE_TYPE_MASK)

#define TextAttrib(FORE, BACK)  ((FORE<<4)|(BACK&0x0F))
#define GetHiToAscii(NUM)       ConvNumToAsciiCode((NUM>>4)&0x0F)
#define GetLoToAscii(NUM)       ConvNumToAsciiCode((NUM)&0x0F)

typedef struct
 {
     EFI_STATUS_CODE_VALUE Value;
     UINT8 Byte;
 } STATUS_CODE_TO_BYTE_MAP;


STATUS_CODE_TO_BYTE_MAP ProgressCheckpointMap[] =
{
//Regular boot
    { PEI_CORE_STARTED, 0x10 },
    { PEI_CAR_CPU_INIT, 0x11 },
    // reserved for CPU 0x12 - 0x14
    { PEI_CAR_NB_INIT, 0x15 },
    // reserved for NB 0x16 - 0x18
    { PEI_CAR_SB_INIT, 0x19 },
    // reserved for SB 0x1A - 0x1C
    // reserved for OEM use: 0x1D - 0x2A
    { PEI_MEMORY_SPD_READ, 0x2B },
    { PEI_MEMORY_PRESENCE_DETECT, 0x2C },
    { PEI_MEMORY_TIMING, 0x2D},
    { PEI_MEMORY_CONFIGURING, 0x2E },
    { PEI_MEMORY_INIT, 0x2F },
    // reserved for AML use: 0x30
    { PEI_MEMORY_INSTALLED, 0x31 },
    { PEI_CPU_INIT,  0x32 },
    { PEI_CPU_CACHE_INIT, 0x33 },
    { PEI_CPU_AP_INIT, 0x34 },
    { PEI_CPU_BSP_SELECT, 0x35 },
    { PEI_CPU_SMM_INIT, 0x36 },
    { PEI_MEM_NB_INIT, 0x37 },
    // reserved for NB 0x38 - 0x3A
    { PEI_MEM_SB_INIT, 0x3B },
    // reserved for SB 0x3C - 0x3E
    // reserved for OEM use: 0x3F - 0x4E
    { PEI_DXE_IPL_STARTED, 0x4F },
    { DXE_CORE_STARTED, 0x60 },
//Recovery
    { PEI_RECOVERY_AUTO, 0xF0 },
    { PEI_RECOVERY_USER, 0xF1 },
    { PEI_RECOVERY_STARTED, 0xF2 },
    { PEI_RECOVERY_CAPSULE_FOUND, 0xF3 },
    { PEI_RECOVERY_CAPSULE_LOADED, 0xF4 },
//S3
    { PEI_S3_STARTED, 0xE0 },
    { PEI_S3_BOOT_SCRIPT, 0xE1 },
    { PEI_S3_VIDEO_REPOST, 0xE2 },
    { PEI_S3_OS_WAKE, 0xE3 },
    {0,0}
};


STATUS_CODE_TO_BYTE_MAP ErrorCheckpointMap[] =
{
//Regular boot
    { PEI_MEMORY_INVALID_TYPE, 0x50 },
    { PEI_MEMORY_INVALID_SPEED, 0x50 },
    { PEI_MEMORY_SPD_FAIL, 0x51 },
    { PEI_MEMORY_INVALID_SIZE, 0x52 },
    { PEI_MEMORY_MISMATCH, 0x52 },
    { PEI_MEMORY_NOT_DETECTED, 0x53 },
    { PEI_MEMORY_NONE_USEFUL, 0x53 },
    { PEI_MEMORY_ERROR, 0x54 },
    { PEI_MEMORY_NOT_INSTALLED, 0x55 },
    { PEI_CPU_INVALID_TYPE, 0x56 },
    { PEI_CPU_INVALID_SPEED, 0x56 },
    { PEI_CPU_MISMATCH, 0x57 },
    { PEI_CPU_SELF_TEST_FAILED, 0x58 },
    { PEI_CPU_CACHE_ERROR, 0x58 },
    { PEI_CPU_MICROCODE_UPDATE_FAILED, 0x59 },
    { PEI_CPU_NO_MICROCODE, 0x59 },
    { PEI_CPU_INTERNAL_ERROR, 0x5A },
    { PEI_CPU_ERROR, 0x5A },
    { PEI_RESET_NOT_AVAILABLE,0x5B },
    //reserved for AMI use: 0x5C - 0x5F
//Recovery
    { PEI_RECOVERY_PPI_NOT_FOUND, 0xF8 },
    { PEI_RECOVERY_NO_CAPSULE, 0xF9 },
    { PEI_RECOVERY_INVALID_CAPSULE, 0xFA },
    //reserved for AMI use: 0xFB - 0xFF
//S3 Resume
    { PEI_MEMORY_S3_RESUME_FAILED, 0xE8 },
    { PEI_S3_RESUME_PPI_NOT_FOUND, 0xE9 },
    { PEI_S3_BOOT_SCRIPT_ERROR, 0xEA },
    { PEI_S3_OS_WAKE_ERROR, 0xEB },
    //reserved for AMI use: 0xEC - 0xEF
    {0,0}
};


STATUS_CODE_TO_BYTE_MAP* CheckPointStatusCodes[] =
{
    //#define EFI_PROGRESS_CODE 0x00000001
    ProgressCheckpointMap,
    //#define EFI_ERROR_CODE 0x00000002
    CHECKPOINT_ERROR_CODES_MAP
    //#define EFI_DEBUG_CODE 0x00000003
};

BOOLEAN
OpenVgaDecode()
{
    UINT32 VgaBar = PcdGet32(PcdSmcVgaBar);

    //
    // Enable I/O & Mem space on PCI Command register on both VGA Controller and PCI Bridge
    //
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x19), Aspeed_P2P_BUS);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x1a), Aspeed_VGA_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x18), Aspeed_P2P_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x19), Aspeed_VGA_BUS);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x1a), Aspeed_VGA_BUS);

    //Config Intel P2P
    PciWrite16(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x1c), 0xD0D0);
#if EarlyVideo_Mode
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x20), (UINT32)((VgaBar+0x1000000)+(VgaBar>>16)));
#else
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x20), 0xFA00F800);
#endif
    PciWrite32(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x24), 0x0001FFF1);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x0c), 0x10);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3c), 0x05);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x3e), 0x18);
    PciWrite8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x04), 0x07);

    //Config Aspeed P2P
    PciWrite16(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x1c), 0xD1D1);
#if EarlyVideo_Mode
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x20), (UINT32)((VgaBar+0x1000000)+(VgaBar>>16)));
#else
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x20), 0xFA00F800);
#endif
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x24), 0x0001FFF1);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x0c), 0x10);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3c), 0x07);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x3e), 0x18);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_P2P_BUS, 0, 0, 0x04), 0x07);

    //Config Aspeed VGA
#if EarlyVideo_Mode
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x10), VgaBar);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x14), (VgaBar+0x1000000));
#else
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x10), 0xF8000000);
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x14), 0xFA000000);
#endif
    PciWrite32(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x18), 0x0000D001);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x3C), 0x07);
    PciWrite8(PCI_LIB_ADDRESS(Aspeed_VGA_BUS, 0, 0, 0x04), 0x07);

    return TRUE;
}

VOID
Port80VideoCheckpoint(
    IN	EFI_PEI_SERVICES        **PeiServices,
    IN	UINT8	Checkpoint
)
{
#if EarlyVideo_Mode
    EFI_STATUS Status;
    UINT16 Did;
    EFI_GUID gEfiPeiSimpleTextOutPPIGuid = EFI_PEI_SIMPLETEXTOUT_PPI_GUID;
    EFI_PEI_SIMPLETEXTOUT_PPI   *pSimpleTextOutPpi = NULL;
    UINT8   Char;
    UINT8 Attr = PcdGet8(PcdSmcColor);


    Did = PciRead16( ((UINT32)Aspeed_VGA_BUS << 20) | ((UINT32)0 << 15) | ((UINT32)0 << 12) + 0x00);
    if(Did != 0x1A03)
        OpenVgaDecode();

    Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiSimpleTextOutPPIGuid, 0, NULL, &pSimpleTextOutPpi);
    if(EFI_ERROR(Status))
        return;

    Char = ((Checkpoint & 0xF0) >> 4) | 0x30;
    if(Char > 0x39)
        Char += 7;
    pSimpleTextOutPpi->WriteChar(SMC_MAX_COL-5, SMC_MAX_ROW-1, Attr, Char);

    Char = (Checkpoint & 0x0F) | 0x30;
    if(Char > 0x39)
        Char +=7;
    pSimpleTextOutPpi->WriteChar(SMC_MAX_COL-4, SMC_MAX_ROW-1, Attr, Char);
#else
    UINT16	*VideoBuff;
    UINT8	Char;

    if(*((UINT16*)(UINTN)0xB8000) == 0xFFFF)
	OpenVgaDecode();     //Init once and go

    VideoBuff = (UINT16*)(UINTN)(0xB8000 + ((SMC_MAX_ROW * SMC_MAX_COL) << 1));
    Char = ((Checkpoint & 0xF0) >> 4) | 0x30;
    if(Char > 0x39)
	 Char += 7;
    *(VideoBuff - 2) = 0x0F00 | Char;

    Char = (Checkpoint & 0x0F) | 0x30;
    if(Char > 0x39)
	 Char +=7;
    *(VideoBuff - 1) = 0x0F00 | Char;
#endif
    MmioWrite8(SMC_DEBUG_ADDRESS, Checkpoint);//SMC Debug card support
    IoWrite8(0x80, Checkpoint);//To avoid POST code read from BMC not sync with RC code problem.
//	CloseVgaDecode();
}

//**********************************************************************
// Status Code Library (functions used throughout the module)
//**********************************************************************
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FindByteCode
//
// Description: Match "Value" to Map->Value and return Map->Byte.
//
// Input:
//      STATUS_CODE_TO_BYTE_MAP *Map  - Pointer to a table (array of structures) that maps status code value (EFI_STATUS_CODE_VALUE) to a byte (UINT8) domain specific value.
//      EFI_STATUS_CODE_VALUE Value  - status code to map
//
// Output:
//      UINT8 - domain specific mapping of the Value
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8
FindByteCode(
    IN	STATUS_CODE_TO_BYTE_MAP	*Map,
    IN	EFI_STATUS_CODE_VALUE	Value
)
{
    while(Map->Value != 0){
        if(Map->Value == Value)
            return Map->Byte;
        Map++;
    }

    return 0;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: PeiSameDiskStatusCodeReportWorker
//
// Description:
//
//
// Input:
//	EFI_PEI_SERVICES	 **PeiServices	- Pointer to the PEI Core data Structure
//	EFI_STATUS_CODE_VALUE	Value - EFI status code Value
//	EFI_STATUS_CODE_TYPE	CodeType - EFI status code type
//  UINT32  Instance - The enumeration of a hardware or software entity within
//						the system. A system may contain multiple entities that
//						match a class/subclass pairing. The instance differentiates
//						between them. An instance of 0 indicates that instance
//						information is unavailable, not meaningful, or not relevant.
//						Valid instance numbers start with 1.
// EFI_GUID	*CallerId - This optional parameter may be used to identify the caller.
//						This parameter allows the status code driver to apply different
//						rules to different callers.
//  EFI_STATUS_CODE_DATA *Data - This optional parameter may be used to pass additional data.
//
// Output:
//  EFI_STATUS
//		EFI_SUCCESS      Status code reported to BMC successfully.
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
PeiEarlyVideoStatusReportWorker(
    IN	CONST EFI_PEI_SERVICES        **PeiServices,
    IN	EFI_STATUS_CODE_TYPE           CodeType,
    IN	EFI_STATUS_CODE_VALUE          Value,
    IN	UINT32                         Instance,
    IN	CONST EFI_GUID                 *CallerId,
    IN	CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
)
{
    UINT8	n;
    UINT32	CodeTypeIndex = STATUS_CODE_TYPE(CodeType) - 1;
    EFI_STATUS	Status = EFI_SUCCESS;

    if((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE){
	//
    	// Check if status code to equivalent Sel entry found in table,If not found return.
    	//
    	n = FindByteCode(CheckPointStatusCodes[CodeTypeIndex],Value);
    	if(n != 0x00)
    	    Port80VideoCheckpoint(PeiServices, n);
    }
    else if((UINT8)Value)
	Port80VideoCheckpoint(PeiServices, (UINT8)Value);

    return Status;
}

//---------------------------------------------------------------------------
//
// Procedure: InitializeEarlyVideoStatus
//
// Description:
//	 This function is the entry point for this PEI.
//	 This installs the SEL status code PPI
//
// Input:
//   EFI_PEI_FILE_HANDLE FileHandle	- Pointer to image file handle.
//   EFI_PEI_SERVICES	 **PeiServices	- Pointer to the PEI Core data Structure
//
// Output:
//  EFI_STATUS
//    EFI_SUCCESS - Successful driver initialization
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
InitializeEarlyVideoStatus(
    IN	CONST EFI_PEI_SERVICES	**PeiServices
)
{

    EFI_STATUS	Status;
    EFI_PEI_RSC_HANDLER_PPI	*RscHandlerPpi;
    //
    // Install call-back to update IpmiTransportPpi pointer in PeiSelPpi after MRC in PEI phase.
    //
    Status = (*PeiServices)->LocatePpi(
		    PeiServices,
		    &gEfiPeiRscHandlerPpiGuid,
		    0,
		    NULL,
		    (VOID**)&RscHandlerPpi);

    if(!EFI_ERROR(Status))
	Status = RscHandlerPpi->Register(PeiEarlyVideoStatusReportWorker);

    return EFI_SUCCESS;
}
