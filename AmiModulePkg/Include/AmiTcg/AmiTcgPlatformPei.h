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
// $Header: /Alaska/SOURCE/Modules/TCG/AmiTcgPlatform/AmiTcgPlatformPei/AmiTcgPlatformPei.h 6     12/30/11 4:40p Fredericko $
//
// $Revision: 6 $
//
// $Date: 12/30/11 4:40p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:	AmiTcgPlatformPei.h
//
// Description:	Header file for AmiTcgPlatformPei
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Include/Setup.h>
#include <AmiTcg/TcgPc.h>
#include <Ppi/TcgTcmService.h>
#include <Ppi/TcgService.h>
#include <Ppi/TpmDevice.h>
#include <PiPei.h>

#define EFI_MAX_BIT       0x80000000

#define TCG_LOG_MAX_TABLE_SIZE      0x100
#define _CR( Record, TYPE,\
       Field )((TYPE*) ((CHAR8*) (Record) - (CHAR8*) &(((TYPE*) 0)->Field)))


#pragma pack (1)
typedef struct
{
    EFI_PHYSICAL_ADDRESS PostCodeAddress;
    #if x64_TCG
    UINT64 PostCodeLength;
    #else
    UINTN PostCodeLength;
    #endif
} EFI_TCG_EV_POST_CODE;

typedef struct
{
    EFI_TCG_PCR_EVENT_HEADER Header;
    EFI_TCG_EV_POST_CODE     Event;
} PEI_EFI_POST_CODE;

typedef struct
{
    EFI_TCM_PCR_EVENT_HEADER Header;
    EFI_TCG_EV_POST_CODE     Event;
} TCM_PEI_EFI_POST_CODE;
#pragma pack ()

typedef struct _TCG_PEI_MEMORY_CALLBACK
{
    EFI_PEI_NOTIFY_DESCRIPTOR NotifyDesc;
    EFI_PEI_FILE_HANDLE       *FfsHeader;
} TCG_PEI_MEMORY_CALLBACK;


EFI_FORWARD_DECLARATION( AMI_TCG_PLATFORM_PPI );


typedef
EFI_STATUS
(EFIAPI * SET_TPM_PHYSICAL_PRESENCE)(
    IN EFI_PEI_SERVICES **PeiServices 
);


typedef
EFI_STATUS
(EFIAPI * MEMORY_PRESENT_ENTRY)(
    IN EFI_PEI_SERVICES **PeiServices 
);


typedef
EFI_STATUS
(EFIAPI * MEMORY_ABSENT_ENTRY)(
    IN EFI_PEI_SERVICES    **PeiServices 
);


typedef
EFI_STATUS
(EFIAPI * READ_RESET_MOR)(
    IN EFI_PEI_SERVICES    **PeiServices 
);

typedef
EFI_STATUS 
(EFIAPI * VERIFYTCGVARIABLES)(
    IN EFI_PEI_SERVICES    **PeiServices 
);



TPM_Capabilities_PermanentFlag TCGPEI_GETCAP(
    IN EFI_PEI_SERVICES **PeiServices );

void
EFIAPI xTcgPeiEntry(
    IN PEI_TCG_PPI         *tcg,
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN EFI_PEI_SERVICES    **ps );


//-----------------------------------------------------------
//HELPER FUNCTIONS
//-----------------------------------------------------------
VOID OverwriteSystemMemory(
IN EFI_PEI_SERVICES    **PeiServices
);


EFI_STATUS MeasureCRTMVersion(
    IN EFI_PEI_SERVICES    **PeiServices 
);


EFI_STATUS
 MeasureTcmCRTMVersion(
    IN EFI_PEI_SERVICES **PeiServices 
);

EFI_STATUS TcgPeiTpmStartup(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_BOOT_MODE    BootMode 
);

EFI_STATUS SendStartupandSelftest(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_BOOT_MODE    BootMode
);

TCM_Capabilities_PermanentFlag TCMPEI_GETCAP(
IN EFI_PEI_SERVICES **PeiServices );


EFI_STATUS MeasureDxeCoreFwVol(
    IN PEI_TCG_PPI                * tcg,
    IN EFI_PEI_SERVICES           **ps,
    IN EFI_FIRMWARE_VOLUME_HEADER *fwv 
);

BOOLEAN Lock_TPMPhysicalPresence(
    IN EFI_PEI_SERVICES **PeiServices 
);


EFI_STATUS ContinueTPMSelfTest(
     IN EFI_PEI_SERVICES **ps 
);


EFI_STATUS LocateTcgPpi(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_TPM_PPI      **gTpmDevicePpi,
    IN PEI_TCG_PPI      **gTcgPpi
);

EFI_STATUS LocateTcmPpi(
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_TPM_PPI      **gTpmDevicePpi,
    IN PEI_TCM_PPI      **gTcmPpi
);


EFI_STATUS TcgPeiBuildHobGuid(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_GUID         *Guid,
    IN UINTN            DataLength,
    OUT VOID            **Hob );


typedef
EFI_STATUS
(EFIAPI * AMI_TCG_PEI_FUNCTION_OVERRIDE)(
    IN EFI_PEI_SERVICES **PeiServices 
);



EFI_STATUS FindDxeCoreFile(
    IN EFI_PEI_SERVICES            **ps,
    OUT EFI_FIRMWARE_VOLUME_HEADER **fwMain );

typedef struct _AMI_TCG_PLATFORM_PPI_AFTER_MEM
{
    SET_TPM_PHYSICAL_PRESENCE   SetPhysicalPresence;
    MEMORY_PRESENT_ENTRY        MemoryPresentFunctioOverride;
    VERIFYTCGVARIABLES          VerifyTcgVariables;
} AMI_TCG_PLATFORM_PPI_AFTER_MEM;


typedef struct _AMI_TCG_PLATFORM_PPI_BEFORE_MEM
{
    MEMORY_ABSENT_ENTRY         MemoryAbsentFunctionOverride;
} AMI_TCG_PLATFORM_PPI_BEFORE_MEM;


typedef struct _AMI_TCG_PEI_FUNCTION_OVERRIDE_PPI
{
    AMI_TCG_PEI_FUNCTION_OVERRIDE   Function;
} AMI_TCG_PEI_FUNCTION_OVERRIDE_PPI;

typedef EFI_STATUS (MEASURE_CRTM_VERSION_PEI_FUNC_PTR)(
    IN EFI_PEI_SERVICES **PeiServices
);

typedef EFI_STATUS (MEASURE_CORE_DXE_FW_VOL_PEI_FUNC_PTR)(
    IN PEI_TCG_PPI                * tcg,
    IN EFI_PEI_SERVICES           **ps,
    IN EFI_FIRMWARE_VOLUME_HEADER *fwv
);

extern MEASURE_CRTM_VERSION_PEI_FUNC_PTR        *MeasureCRTMVersionFuncPtr;
extern MEASURE_CORE_DXE_FW_VOL_PEI_FUNC_PTR     *MeasureDxeCoreFwVolFuncPtr;


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
