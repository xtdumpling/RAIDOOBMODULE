//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.04
//    Bug Fix: 1.Fixed still show warning message on POST screen at first boot after changing VGA priority from Offboard to Onboard.
//    Reason : 
//    Auditor: Chen Lin
//    Date   : June/23/2017
//
//
//  Rev. 1.03
//    Bug Fix: Fix Intel P2P has yellow mark when booting with Offboard VGA
//             and EarlyVideo enabled.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Apr/05/2017
//
//  Rev. 1.02
//    Bug Fix: Early Graphic Logo Support.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Mar/14/2017
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

#include <token.h>
#include <Pi/PiStatusCode.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiStatusCodes.h>
#include "SmcSimpleTextOutProtocol.h"
#include "AstVbiosDxeLib.h"
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/GraphicsOutput.h>
#include <Guid\SetupVariable.h>


#ifndef BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID
#define BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID \
    {0xdbc9fd21, 0xfad8, 0x45b0, 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93}
#endif
static EFI_GUID gBdsAllDriversConnectedProtocolGuid = BDS_ALL_DRIVERS_CONNECTED_PROTOCOL_GUID;

#define MAX_ROW                25
#define MAX_COL                80
#define STATUS_CODE_TYPE(Type) ((Type)&EFI_STATUS_CODE_TYPE_MASK)

SMC_SIMPLETEXTOUT_PROTOCOL  *pmSmcSimpleTextOutProtocol = NULL;
EFI_RSC_HANDLER_PROTOCOL    *mRscHandlerProtocol = NULL;
UINT8 Attr = 0x0F; // Color Attribute



typedef struct
 {
     EFI_STATUS_CODE_VALUE Value;
     UINT8 Byte;
 } STATUS_CODE_TO_BYTE_MAP;


STATUS_CODE_TO_BYTE_MAP ProgressCheckpointMap[] =
{
//DXE Phase
    { DXE_CORE_STARTED, 0x60 },
    { DXE_NVRAM_INIT, 0x61 },
    { DXE_SBRUN_INIT, 0x62 },
    { DXE_CPU_INIT, 0x63 },
    //reserved for CPU 0x64 - 0x67
    { DXE_NB_HB_INIT, 0x68 },
    { DXE_NB_INIT, 0x69 },
    { DXE_NB_SMM_INIT, 0x6A },
    //reserved for NB 0x6B - 0x6F
    { DXE_SB_INIT, 0x70 },
    { DXE_SB_SMM_INIT, 0x71 },
    { DXE_SB_DEVICES_INIT, 0x72 },
    //reserved for SB 0x73 - 0x77
    { DXE_ACPI_INIT, 0x78 },
    { DXE_CSM_INIT, 0x79 },
    //reserved for AMI use: 0x7A - 0x7F
    //reserved for OEM use: 0x80 - 0x8F
    { DXE_BDS_STARTED, 0x90 },
    { DXE_BDS_CONNECT_DRIVERS, 0x91 },
    { DXE_PCI_BUS_BEGIN, 0x92 },
    { DXE_PCI_BUS_HPC_INIT, 0x93 },
    { DXE_PCI_BUS_ENUM, 0x94 },
    { DXE_PCI_BUS_REQUEST_RESOURCES, 0x95 },
    { DXE_PCI_BUS_ASSIGN_RESOURCES, 0x96 },
    { DXE_CON_OUT_CONNECT, 0x97 },
    { DXE_CON_IN_CONNECT, 0x98 },
    { DXE_SIO_INIT, 0x99 },
    { DXE_USB_BEGIN, 0x9A },
    { DXE_USB_RESET, 0x9B },
    { DXE_USB_DETECT, 0x9C },
    { DXE_USB_ENABLE, 0x9D },
    //reserved for AMI use: 0x9E - 0x9F
    { DXE_IDE_BEGIN, 0xA0 },
    { DXE_IDE_RESET, 0xA1 },
    { DXE_IDE_DETECT, 0xA2 },
    { DXE_IDE_ENABLE, 0xA3 },
    { DXE_SCSI_BEGIN, 0xA4 },
    { DXE_SCSI_RESET, 0xA5 },
    { DXE_SCSI_DETECT, 0xA6 },
    { DXE_SCSI_ENABLE, 0xA7 },
    { DXE_SETUP_VERIFYING_PASSWORD, 0xA8 },
    { DXE_SETUP_START, 0xA9 },
    //reserved for AML use: 0xAA
    { DXE_SETUP_INPUT_WAIT, 0xAB },
    //reserved for AML use: 0xAC
    { DXE_READY_TO_BOOT, 0xAD },
    { DXE_LEGACY_BOOT, 0xAE },
    { DXE_EXIT_BOOT_SERVICES, 0xAF },
    { RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN, 0xB0 },
    { RT_SET_VIRTUAL_ADDRESS_MAP_END, 0xB1 },
    { DXE_LEGACY_OPROM_INIT, 0xB2 },
    { DXE_RESET_SYSTEM, 0xB3 },
    { DXE_USB_HOTPLUG, 0xB4 },
    { DXE_PCI_BUS_HOTPLUG, 0xB5 },
    { DXE_NVRAM_CLEANUP, 0xB6 },
    { DXE_CONFIGURATION_RESET, 0xB7 },
    //reserved for AMI use: 0xB8 - 0xBF
    //reserved for OEM use: 0xC0 - 0xCF
    {0,0}
};


STATUS_CODE_TO_BYTE_MAP ErrorCheckpointMap[] =
{
//DXE error
    { DXE_CPU_ERROR, 0xD0 },
    { DXE_NB_ERROR, 0xD1 },
    { DXE_SB_ERROR, 0xD2 },
    { DXE_ARCH_PROTOCOL_NOT_AVAILABLE, 0xD3 },
    { DXE_PCI_BUS_OUT_OF_RESOURCES, 0xD4 },
    { DXE_LEGACY_OPROM_NO_SPACE, 0xD5 },
    { DXE_NO_CON_OUT, 0xD6 },
    { DXE_NO_CON_IN, 0xD7 },
    { DXE_INVALID_PASSWORD, 0xD8 },
    { DXE_BOOT_OPTION_LOAD_ERROR, 0xD9 },
    { DXE_BOOT_OPTION_FAILED, 0xDA },
    { DXE_FLASH_UPDATE_FAILED, 0xDB },
    { DXE_RESET_NOT_AVAILABLE, 0xDC },
    //reserved for AMI use: 0xDE - 0xDF
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

VOID Port80VideoCheckpoint(IN UINT8 Checkpoint)
{
#if EarlyVideo_Mode == 0
    UINT16                         *VideoBuff;
#endif
    UINT8                          Char1, Char2;
    EFI_GUID                       mAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    EFI_GUID                       gEfiGraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    AMI_POST_MANAGER_PROTOCOL      *PostManager = NULL;
    EFI_GRAPHICS_OUTPUT_PROTOCOL   *pGOP = NULL;
    UINT8                          Data8;
    EFI_STATUS                     Status, Status2;


    Char1 = ((Checkpoint & 0xF0) >> 4) | 0x30;
    if(Char1 > 0x39)
        Char1 += 7;

    Char2 = (Checkpoint & 0x0F) | 0x30;
    if(Char2 > 0x39)
        Char2 +=7;

    Status = pBS->LocateProtocol(&mAmiPostManagerProtocolGuid, NULL, &PostManager);
    Status2 = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, &pGOP);
    if( EFI_ERROR(Status) || EFI_ERROR(Status2) ) {
        // Determine if PCI Resources assigned by PciBus Driver
        Data8 = PciRead8(PCI_LIB_ADDRESS(0, Root_Port_Dev, Root_Port_Fun, 0x19));
        if(Data8!=Aspeed_P2P_BUS)
            return;

        // Open VGA Decode
        pmSmcSimpleTextOutProtocol->OpenVgaDecode();

        // Print 80 code
#if EarlyVideo_Mode
        pmSmcSimpleTextOutProtocol->WriteChar(SMC_MAX_COL-5, SMC_MAX_ROW-1, Attr, Char1);
        pmSmcSimpleTextOutProtocol->WriteChar(SMC_MAX_COL-4, SMC_MAX_ROW-1, Attr, Char2);
#else
        VideoBuff = (UINT16*)(UINTN)(0xB8000 + (((MAX_ROW)*MAX_COL)<<1));
        *(VideoBuff  - 2) = 0x0F00 |Char1;
        *(VideoBuff  - 1) = 0x0F00 |Char2;
#endif

        // Close VGA Decode
        pmSmcSimpleTextOutProtocol->CloseVgaDecode();
    }
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
UINT8 FindByteCode(STATUS_CODE_TO_BYTE_MAP *Map, EFI_STATUS_CODE_VALUE Value)
{
    while (Map->Value!=0)
    {
        if (Map->Value==Value)
        {
            return Map->Byte;
        }

        Map++;
    }

    return 0;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: DxeEarlyVideoStatusReportWorker
//
// Description:
//
//
// Input:
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
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
DxeEarlyVideoStatusReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN EFI_GUID                 	    *CallerId,
  IN EFI_STATUS_CODE_DATA     	    *Data OPTIONAL )
{
    UINT8 n;
    UINT32 CodeTypeIndex = STATUS_CODE_TYPE(CodeType) - 1;
    EFI_STATUS          Status=EFI_SUCCESS;


    if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    	//
    	// Check if status code to equivalent Sel entry found in table,If not found return.
    	//
    	n = FindByteCode(CheckPointStatusCodes[CodeTypeIndex],Value);
		if ( n != 0x00)
		{
		    Port80VideoCheckpoint(n);
		}
    }

    return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: EarlyVideoOffBoardCallback
//
// Description:
//       This callback function is the entry point for EarlyVideo OffBoard.
//
//  Input:
//       IN EFI_EVENT Event  -  signalled event.
//       IN VOID *Context    -  calling context.
//
//  Output: VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID EarlyVideoOffBoardCallback (
  IN EFI_EVENT         Event,
  IN VOID             *Context
  )
{
  EFI_STATUS		    Status = EFI_SUCCESS;
  SYSTEM_CONFIGURATION	SystemConfiguration;
  UINTN			        VariableSize = sizeof(SYSTEM_CONFIGURATION);
  UINT32		        VariableAttr;


  DEBUG((-1, "## EarlyVideoOffBoardCallback - Entry\n"));
  
  Status = gRT->GetVariable( L"IntelSetup",
                             &gEfiSetupVariableGuid,
                             &VariableAttr,
                             &VariableSize,
                             &SystemConfiguration);

  //if (CheckTheVideoSelect() == 2)
  if (!EFI_ERROR(Status) && (SystemConfiguration.VideoSelect == 2))
  {
     TraceRootPortAndAddonVGABus ();
  }
  gBS->CloseEvent(Event);
  DEBUG((-1, "## EarlyVideoOffBoardCallback - End\n"));
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: UnregisterReportHandlers
//
// Description:
//       This callback function is to unregister for
//       DxeEarlyVideoStatusReportWorker.
//
//  Input:
//       IN EFI_EVENT Event  -  signalled event.
//       IN VOID *Context    -  calling context.
//
//  Output: VOID
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
UnregisterReportHandlers (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    if(mRscHandlerProtocol)
        mRscHandlerProtocol->Unregister(DxeEarlyVideoStatusReportWorker);
}

//---------------------------------------------------------------------------
//
// Procedure: EarlyVideoDxeInit
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
EarlyVideoDxeInit  (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable )
{
    EFI_HANDLE                   NewHandle = NULL;
    EFI_STATUS                   Status = EFI_SUCCESS;
    EFI_EVENT                    Event     = NULL;
    VOID                         *NotifyReg = NULL;
    EFI_EVENT                    ExitBootServicesEvent = NULL;


    InitAmiLib (ImageHandle, SystemTable);
	
    Status = pBS->LocateProtocol(
                    &gSmcSimpleTextOutProtocolGuid,
                    NULL,
                    (VOID **) &pmSmcSimpleTextOutProtocol
                    );
    ASSERT_EFI_ERROR(Status);
    
    Status = gBS->LocateProtocol (&gEfiRscHandlerProtocolGuid,
                                  NULL,
                                  (VOID **) &mRscHandlerProtocol);
    ASSERT_EFI_ERROR (Status);
    if (!EFI_ERROR (Status)) {
        Status = mRscHandlerProtocol->Register(DxeEarlyVideoStatusReportWorker, TPL_HIGH_LEVEL);
        ASSERT_EFI_ERROR(Status);

        Status = RegisterProtocolCallback (
        		&gBdsAllDriversConnectedProtocolGuid,
        		EarlyVideoOffBoardCallback,
        		NULL,
        		&Event,
        		&NotifyReg);
        DEBUG((-1, "## RegisterProtocolCallback - EarlyVideoOffBoardCallback:[%r]\n", Status));
        ASSERT_EFI_ERROR(Status);

        Status = gBS->CreateEventEx(
                EVT_NOTIFY_SIGNAL,
                TPL_NOTIFY,
                UnregisterReportHandlers,
                NULL,
                &gEfiEventExitBootServicesGuid,
                &ExitBootServicesEvent);
    }

#if EarlyVideo_Mode
    // Get Color Attribute
    Attr = PcdGet8(PcdSmcColor);
#endif
    
    return EFI_SUCCESS;
}
