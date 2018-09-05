//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/AmiTcgPlatform/AmiTcgPlatformDxe/AmiTcgPlatformDxe.h 11    1/20/12 9:20p Fredericko $
//
// $Revision: 11 $
//
// $Date: 1/20/12 9:20p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    AmiTcgPlatformDxe.h
//
// Description: Header file for AmiTcgPlatformDxe
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Token.h>
#include <AmiTcg/TcgPc.h>
//#include <AmiDxeLib.h>
#include <AmiTcg/TcgCommon12.h>
#include <AmiTcg/tcg.h>
#if SMBIOS_SUPPORT == 1
//  #include <Protocol\SmBios.h>
//  #include <Protocol\AmiSmBios.h>
#endif
#include <AmiTcg/TCGMisc.h>
#include <Protocol/TcgTcmService.h>
#include <Protocol/TcgService.h>
#include <Protocol/TpmDevice.h>
#include <Include/Setup.h>
#include "Protocol/CpuIo.h"
#include "Protocol/FirmwareVolume.h"
#include "Protocol/DevicePath.h"
#include <Library/IoLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmiTpmSupportTypeProtocol.h>
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
#include "Protocol/LegacyBios.h"
#endif

#include "Protocol/PciIo.h"
#include "Protocol/Variable.h"
#include "Protocol/Reset.h"
#include "Protocol/SimpleTextOut.h"
#include "Protocol/SimpleTextIn.h"
//#include "Protocol/UgaDraw.h"
//#include "Protocol/AMIPostMgr.h"
/*
#if EFI_SPECIFICATION_VERSION>0x20000
    #include "Include\UefiHii.h"
    #include "Protocol/HiiDatabase.h"
    #include "Protocol/HiiString.h"
#else
  #include "Protocol/HII.h"
#endif
*/
#include <Protocol/SmmBase.h>
#include <Protocol/SmmSwDispatch.h>
#include <Protocol/Runtime.h>
//#include <Include\Acpi.h>
//#include <Include\Acpi11.h>
//#include "AmiTcgPlatformDxeLib.h"
//#include <AcpiRes.h>




//--------------------------------------------------------------------------
//GUID Definitions
//--------------------------------------------------------------------------
#define TCG_DEACTIVED_ERROR  0x06

//---------------------------------------------------------------------------
//      Structure Definitions
//---------------------------------------------------------------------------
typedef struct
{
    UINTN          Signature;
    EFI_LIST_ENTRY AllEntries;            // All entries
    EFI_GUID       ProtocolID;            // ID of the protocol
    EFI_LIST_ENTRY Protocols;             // All protocol interfaces
    EFI_LIST_ENTRY Notify;                // Registerd notification handlers
} TCG_PROTOCOL_ENTRY;

typedef struct
{
    UINTN              Signature;
    TCG_PROTOCOL_ENTRY *Protocol;
    EFI_LIST_ENTRY     Link;                  // All notifications for this protocol
    EFI_EVENT          Event;                 // Event to notify
    EFI_LIST_ENTRY     *Position;              // Last position notified
} TCG_PROTOCOL_NOTIFY;

typedef struct
{
    UINT8 MajorVersion;
    UINT8 MinorVersion;
    UINT8 Reserve;
    UINT8 Flag;
} AMI_TCG_PROTOCOL_VERSION;




//**********************************************************************
//              Function Declarations
//**********************************************************************
EFI_STATUS
OnAcpiInstalled(IN EFI_EVENT ev,
                IN VOID  *ctx );



EFI_STATUS measureCpuMicroCode();


EFI_FORWARD_DECLARATION( AMI_TCG_PLATFORM_PROTOCOL );


typedef
EFI_STATUS
(EFIAPI * MEASURE_CPU_MICROCODE)(

);


typedef
EFI_STATUS
(EFIAPI * MEASURE_HANDOFF_TABLES)(

);


typedef
EFI_STATUS
(EFIAPI * MEASURE_PCI_OPROMS)(

);


typedef
EFI_STATUS
(EFIAPI * PROCESS_TCG_SETUP)(

);



typedef
EFI_STATUS
(EFIAPI * PROCESS_TCG_PPI_REQUEST)(

);



typedef
EFI_STATUS
(EFIAPI * MEASURE_VARIABLES)(

);


typedef
EFI_STATUS
(EFIAPI * TCG_READY_TO_BOOT)(

);

typedef
EFI_STATUS
(EFIAPI * GET_PROTOCOL_VERSION)(
    AMI_TCG_PROTOCOL_VERSION *
);

typedef
EFI_STATUS
(EFIAPI * GET_)(
    AMI_TCG_PROTOCOL_VERSION *
);


typedef
VOID
(EFIAPI * RESETOSTCGVAR)(
);


typedef
EFI_STATUS
(EFIAPI * AMI_TCG_DXE_FUNCTION_OVERRIDE)(
);



typedef struct _AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL
{
    AMI_TCG_DXE_FUNCTION_OVERRIDE   Function;
} AMI_TCG_DXE_FUNCTION_OVERRIDE_PROTOCOL;



typedef EFI_STATUS (MEASURE_PCI_OPTION_ROM_FUNC_PTR)(

);

typedef EFI_STATUS (MEASURE_HANDOFF_TABLES_FUNC_PTR)(

);


typedef EFI_STATUS (MEASURE_CPU_MICROCODE_FUNC_PTR)(

);


typedef EFI_STATUS (MEASURE_BOOT_VAR_FUNC_PTR)(

);

typedef EFI_STATUS (MEASURE_PLATFORM_CONFIG_FUNC_PTR)(

);

#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables!= 0))
typedef EFI_STATUS (MEASURE_SECURE_BOOT_FUNC_PTR)(

);
#endif


typedef EFI_STATUS (MEASURES_TCG_BOOT_SEPARATORS_DXE_FUNC_PTR)(

);


typedef EFI_STATUS (MEASURE_WAKE_EVENT_DXE_FUNC_PTR)(

);


extern MEASURE_PCI_OPTION_ROM_FUNC_PTR  *MeasurePCIOpromsFuncPtr;
extern MEASURE_HANDOFF_TABLES_FUNC_PTR  *MeasureHandoffTablesFuncPtr;
extern MEASURE_CPU_MICROCODE_FUNC_PTR   *MeasureCpuMicroCodeFuncPtr;
extern MEASURE_BOOT_VAR_FUNC_PTR        *MeasureAllBootVariablesFuncPtr;
#if (defined(TCGMeasureSecureBootVariables) && (TCGMeasureSecureBootVariables != 0))
extern MEASURE_SECURE_BOOT_FUNC_PTR     *MeasureSecurebootVariablesFuncPtr;
#endif
extern MEASURES_TCG_BOOT_SEPARATORS_DXE_FUNC_PTR      *MeasureSeparatorsFuncPtr;
extern MEASURE_WAKE_EVENT_DXE_FUNC_PTR                *MeasureWakeEventFuncPtr;
extern MEASURE_PLATFORM_CONFIG_FUNC_PTR *MeasureConfigurationInfoFuncPtr;

typedef struct _AMI_TCG_PLATFORM_PROTOCOL
{
    MEASURE_CPU_MICROCODE       MeasureCpuMicroCode;
    MEASURE_PCI_OPROMS          MeasurePCIOproms;
    PROCESS_TCG_SETUP           ProcessTcgSetup;
    PROCESS_TCG_PPI_REQUEST     ProcessTcgPpiRequest;
    TCG_READY_TO_BOOT           SetTcgReadyToBoot;
    GET_PROTOCOL_VERSION        GetProtocolVersion;
    RESETOSTCGVAR               ResetOSTcgVar;
} AMI_TCG_PLATFORM_PROTOCOL;


extern EFI_GUID gAmiTcgPlatformProtocolguid;
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
