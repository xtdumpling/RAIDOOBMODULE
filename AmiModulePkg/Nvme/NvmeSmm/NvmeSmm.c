//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NvmeSmm.c
    Nvme SMM driver to handle the Nvme device access

**/

//---------------------------------------------------------------------------

#include "NvmeSmm.h"
#include <Library/BaseMemoryLib.h>
#include <Guid/SmmVariableCommon.h>
#include <Library/AmiBufferValidationLib.h>

//---------------------------------------------------------------------------

EFI_SMM_BASE2_PROTOCOL          *gSmmBase2;
EFI_SMM_SYSTEM_TABLE2           *pSmst2;
NVME_GLOBAL_DATA                *gNvmeData;
AMI_NVME_CONTROLLER_PROTOCOL    *gNvmeController[NVMEDEVS_MAX_ENTRIES] = {0};
BOOLEAN                         gFirstAPICall = FALSE;
AMI_NVME_PASS_THRU_PROTOCOL     gAmiSmmNvmePassThru;

/**
     NvmeMassApiTable - NVMe API Function Dispatch Table
        
**/


API_FUNC NvmeMassApiTable[] = {
    NvmeMassAPIGetDeviceInformation,    // Nvme Mass API Sub-Func 00h
    NvmeMassAPIGetDeviceGeometry,       // Nvme Mass API Sub-Func 01h
    NvmeMassAPIResetDevice,             // Nvme Mass API Sub-Func 02h
    NvmeMassAPIReadDevice,              // Nvme Mass API Sub-Func 03h
    NvmeMassAPIWriteDevice,             // Nvme Mass API Sub-Func 04h
    NvmeMassAPIPass,                    // Nvme Mass API Sub-Func 05h VerifyDevice
    NvmeMassAPIPass,                    // Nvme Mass API Sub-Func 06h FormatDevice
    NvmeMassAPINotSupported,            // Nvme Mass API Sub-Func 07h CommandPassThru
    NvmeMassAPINotSupported,            // Nvme BIOS API function 08h AssignDriveNumber
    NvmeMassAPINotSupported,            // Nvme BIOS API function 09h CheckDevStatus
    NvmeMassAPINotSupported,            // Nvme BIOS API function 0Ah GetDevStatus
    NvmeMassAPINotSupported             // Nvme BIOS API function 0Bh GetDeviceParameters
};

/**
    Get the Index# for the DeviceAddress

    @param  DeviceAddress,
    @param  **ActiveNameSpace

    @retval None

**/

VOID
GetDevEntry (
    UINT8                   DeviceAddress,
    ACTIVE_NAMESPACE_DATA   **ActiveNameSpace
)
{

    UINT8                           Count;
    LIST_ENTRY                      *LinkData;
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController;
    
    // Locate a free slot to copy the pointer
     for (Count = 0; Count < NVMEDEVS_MAX_ENTRIES; Count++ ){
         if (gNvmeController[Count]) {
             NvmeController = gNvmeController[Count];
             
             if(IsListEmpty(&NvmeController->ActiveNameSpaceList)) {
                 continue;
             } 
             
             for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; \
                     LinkData != &NvmeController->ActiveNameSpaceList; 
                     LinkData = LinkData->ForwardLink) {
                 
                 *ActiveNameSpace = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);
                 if ((*ActiveNameSpace)->Int13DeviceAddress == DeviceAddress) {
                     return;
                 }
             }
         }
     }
    
    *ActiveNameSpace = NULL;
    return ;

}

/**
    Return Device information

    @param  *NvmeURP

    @retval None

**/

VOID
NvmeMassAPIGetDeviceInformation (
    NVME_STRUC  *NvmeURP
)
{

    NvmeURP->bRetValue = NVME_NOT_SUPPORTED;
    return;
    
}

/**
    Return Device Geometry

    @param *NvmeURP

    @retval None

**/

VOID
NvmeMassAPIGetDeviceGeometry (
    NVME_STRUC  *NvmeURP
)
{
    NvmeURP->bRetValue = NVME_NOT_SUPPORTED;
    return;
}

/**
    Reset device

    @param  *NvmeURP

    @retval None

**/

VOID
NvmeMassAPIResetDevice (
    NVME_STRUC  *NvmeURP
)
{
    NvmeURP->bRetValue = NVME_NOT_SUPPORTED;
    return;
}

/**
    Read data from the device

    @param *NvmeURP

    @retval None

**/

VOID
NvmeMassAPIReadDevice (
    NVME_STRUC  *NvmeURP
)
{
    
    ACTIVE_NAMESPACE_DATA               *ActiveNameSpace = NULL;
    EFI_STATUS                          Status;
    EFI_LBA                             Lba;
    UINT16                              NumBlks;
    UINT16                              BlksPerTransfer;
    VOID                                *Buffer = NULL;
    VOID                                *ReadBuffer;
    UINTN                               BufferSize;
    BOOLEAN                             UnalignedTransfer = FALSE;
    AMI_NVME_CONTROLLER_PROTOCOL        *NvmeController;
    
    GetDevEntry(NvmeURP->ApiData.Read.DeviceAddress, &ActiveNameSpace);
    
    if(ActiveNameSpace == NULL) {
        NvmeURP->bRetValue = NVME_PARAMETER_FAILED;
        return;
    }
    
    // Validate if input Buffer address is an non-SMRAM region to avoid SMRAM data 
    // corruption through SMI handlers.
    // NOTE: As DMA transfer isn't supported inside SMM, Buffer validation is not needed for 
    // NVMe. But below validation code is added to avoid repeated Security False Threat reports.

    Status = AmiValidateMemoryBuffer( (VOID*)NvmeURP->ApiData.Read.BufferAddress, 
                                       NvmeURP->ApiData.Read.NumBlks * 
                                       ActiveNameSpace->NvmeBlockIO.Media->BlockSize );
    if (EFI_ERROR(Status)) {
        NvmeURP->bRetValue = NVME_PARAMETER_FAILED;
        return;
    }
    
    NvmeController = ActiveNameSpace->NvmeController;

    Lba=NvmeURP->ApiData.Read.LBA;
    NumBlks=NvmeURP->ApiData.Read.NumBlks;
   
    Buffer= (VOID *)(UINTN)NvmeURP->ApiData.Read.BufferAddress;

    BlksPerTransfer =  NumBlks;
    ReadBuffer = Buffer;
    
    //If Buffer isn't aligned use internal buffer
    if ((UINTN)NvmeURP->ApiData.Read.BufferAddress & ((1 << ActiveNameSpace->NvmeBlockIO.Media->IoAlign)-1)) {
        BlksPerTransfer = 1;
        ReadBuffer = NvmeController->LegacyNvmeBuffer;
        UnalignedTransfer = TRUE;
    }

    BufferSize = BlksPerTransfer * ActiveNameSpace->NvmeBlockIO.Media->BlockSize;
    
    for (  ;  NumBlks; NumBlks -= BlksPerTransfer){
        Status = NvmeReadWriteBlocks (ActiveNameSpace, ActiveNameSpace->NvmeBlockIO.Media->MediaId, Lba, BufferSize, ReadBuffer,NULL,NVME_READ);
        if (EFI_ERROR(Status)) {
            break;
        }
        if  (UnalignedTransfer) {
            CopyMem (Buffer, ReadBuffer, BufferSize); 
        }
        Buffer = (VOID *)((UINTN)Buffer + BufferSize);
        Lba += BlksPerTransfer;

    }
   
    if (EFI_ERROR(Status)) {
        NvmeURP->bRetValue = NVME_READ_ERR;
    } else {
        NvmeURP->bRetValue = NVME_SUCCESS;
    }
    
    return;
    
}

/**
    Write data to the device

    @param  *NvmeURP

    @retval None

**/

VOID
NvmeMassAPIWriteDevice (
    NVME_STRUC  *NvmeURP
)
{
    
    ACTIVE_NAMESPACE_DATA               *ActiveNameSpace = NULL;
    EFI_STATUS                          Status;
    EFI_LBA                             Lba;
    UINT16                              NumBlks;
    UINT16                              BlksPerTransfer;
    VOID                                *Buffer = NULL;
    VOID                                *ReadBuffer;
    UINTN                               BufferSize;
    BOOLEAN                             UnalignedTransfer = FALSE;
    AMI_NVME_CONTROLLER_PROTOCOL        *NvmeController;
    
    GetDevEntry(NvmeURP->ApiData.Read.DeviceAddress, &ActiveNameSpace);
    
    if(ActiveNameSpace == NULL) {
        NvmeURP->bRetValue = NVME_PARAMETER_FAILED;
        return;
    }
    
    // Validate if input Buffer address is an non-SMRAM region to avoid SMRAM data 
    // corruption through SMI handlers.
    // NOTE: As DMA transfer isn't supported inside SMM, Buffer validation is not needed for 
    // NVMe. But below validation code is added to avoid repeated Security False Threat reports.

    Status = AmiValidateMemoryBuffer( (VOID*)NvmeURP->ApiData.Read.BufferAddress, 
                                      NvmeURP->ApiData.Read.NumBlks * 
                                      ActiveNameSpace->NvmeBlockIO.Media->BlockSize );

    if (EFI_ERROR(Status)) {
        NvmeURP->bRetValue = NVME_PARAMETER_FAILED;
        return;
    }

    NvmeController = ActiveNameSpace->NvmeController;

    Lba=NvmeURP->ApiData.Read.LBA;
    NumBlks=NvmeURP->ApiData.Read.NumBlks;
   
    Buffer= (VOID *) (UINTN) NvmeURP->ApiData.Read.BufferAddress;

    BlksPerTransfer =  NumBlks;
    ReadBuffer = Buffer;
    
    //If Buffer isn't aligned use internal buffer
    if ((UINTN)NvmeURP->ApiData.Read.BufferAddress & ((1 << ActiveNameSpace->NvmeBlockIO.Media->IoAlign)-1)) {
        BlksPerTransfer = 1;
        ReadBuffer = NvmeController->LegacyNvmeBuffer;
        UnalignedTransfer = TRUE;
    }

    BufferSize = BlksPerTransfer * ActiveNameSpace->NvmeBlockIO.Media->BlockSize;
    
    for (  ;  NumBlks; NumBlks -= BlksPerTransfer){
        
        if  (UnalignedTransfer) {
            CopyMem (ReadBuffer, Buffer, BufferSize); 
        }
        
        Status = NvmeReadWriteBlocks (ActiveNameSpace, ActiveNameSpace->NvmeBlockIO.Media->MediaId, Lba, BufferSize, ReadBuffer,NULL, NVME_WRITE);
        if (EFI_ERROR(Status)) {
            break;
        }

        Buffer = (VOID *)((UINTN)Buffer + BufferSize);
        Lba += BlksPerTransfer;

    }
   
    if (EFI_ERROR(Status)) {
        NvmeURP->bRetValue = NVME_WRITE_ERR;
    }
    else {
        NvmeURP->bRetValue = NVME_SUCCESS;
    }
    
    return;
    
}



/**
    Dummy handler to return NVME_SUCCESS

        
    @param *NvmeURP
    @retval None

**/

VOID
NvmeMassAPIPass(
    NVME_STRUC *NvmeURP
)
{

    NvmeURP->bRetValue = NVME_SUCCESS;
    return;

}


/**
    Dummy handler to return NVME_NOT_SUPPORTED

    @param *NvmeURP

    @retval None

**/

VOID
NvmeMassAPINotSupported (
    NVME_STRUC  *NvmeURP
)
{

    NvmeURP->bRetValue = NVME_NOT_SUPPORTED;
    return;

}

/**
    Initialize NVMe Controller in S3 boot path

    @param   NvmeController,

    @retval  EFI_SUCCESS NvmeController is initialized successfully

**/

EFI_STATUS
EFIAPI
NvmeInitController (
    IN  AMI_NVME_CONTROLLER_PROTOCOL  *NvmeController 
  )
{
    UINT32                  Delay;
    UINT32                  ProgramCC = 0;
    NVME_DXE_SMM_DATA_AREA  *NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;

    if(NvmeController->NvmeBarOffset == 0 ) {
        return EFI_NOT_FOUND;
    }

    if(CONTROLLER_REG32(NvmeController, Offset_Version) == 0xFFFFFFF) {
        ASSERT(FALSE);
        return EFI_NOT_FOUND;
    }

    // Initialize Nvme controller only once in S3Resume 
    if( NvmeController->NvmeInitControllerFlag ) {
        if( CONTROLLER_REG32(NvmeController, Offset_Aqa) ==\
                     ((NvmeController->AdminCompletionQueueSize - 1) << 16) + (NvmeController->AdminSubmissionQueueSize - 1) ) {
            return EFI_SUCCESS;
        }
        NvmeController->NvmeInitControllerFlag = FALSE;
    }

    NvmeDxeSmmDataArea->AdminSubmissionQueueHeadPtr = 0;
    NvmeDxeSmmDataArea->AdminSubmissionQueueTailPtr = 0;
    NvmeDxeSmmDataArea->AdminCompletionQueueHeadPtr = 0;
    NvmeDxeSmmDataArea->AdminCompletionQueueTailPtr = 0;
    NvmeDxeSmmDataArea->CommandIdentifierAdmin = 0;
    NvmeController->DoorBellStride = 0;
    NvmeDxeSmmDataArea->AdminPhaseTag = FALSE;
    NvmeDxeSmmDataArea->Queue1PhaseTag = FALSE;

    ZeroMem((VOID *)NvmeController->AdminSubmissionQueue, NvmeController->AdminSubmissionQueueSize);
    ZeroMem((VOID *)NvmeController->AdminCompletionQueue, NvmeController->AdminCompletionQueueSize);

    Delay = NvmeController->TimeOut * 500;

    // Check if the controller is still in shutdown process occurring state
    do {
        if ((CONTROLLER_REG32(NvmeController, Offset_CSTS) & 0xC) != 4) {
            break;
        }
        gBS->Stall(1000); // 1msec delay
        
    } while (--Delay);

    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }

    // CC.EN bit needs to be disabled before set ASQ,ACQ values
    Delay = NvmeController->TimeOut * 500;
    if (CONTROLLER_REG32(NvmeController, Offset_CC) & 0x1) {
        //  Disable Enable bit
        CONTROLLER_REG32_AND (NvmeController, Offset_CC, ~0x01);
        do {
            if (!(CONTROLLER_REG32(NvmeController, Offset_CSTS) & 0x1)) {
                break;
            }
            gBS->Stall(1000); // 1msec delay
        }while (--Delay);
    }

    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }

    // Initialize Controller configuration register. 
    // Select Round Robin and NVM Command Set (both values are zero)
    // Max Page Size
    NvmeController->MemoryPageSize = NvmeController->MemoryPageSizeMin;
    ProgramCC |= (UINT32) LShiftU64((UINTN)RShiftU64(NvmeController->MemoryPageSize, 13), 7);
    
    // Initialize with default value. Later it can be modified
    ProgramCC |= (6 << 16);     // I/O Submission Queue Entry Size
    ProgramCC |= (4 << 20);     // I/O Completion Queue Entry Size
    
    CONTROLLER_WRITE_REG32( NvmeController, Offset_CC, ProgramCC );

    // Program Admin Queue Size and Base Address
    CONTROLLER_WRITE_REG32( NvmeController, Offset_Aqa, ((NvmeController->AdminCompletionQueueSize - 1) << 16) + 
                               (NvmeController->AdminSubmissionQueueSize - 1) );

    CONTROLLER_WRITE_REG32( NvmeController, Offset_Asq, 
                               (UINT32) NvmeController->AdminSubmissionQueue );

    CONTROLLER_WRITE_REG32( NvmeController, Offset_Asq + 4,  
                             (UINT32) RShiftU64(NvmeController->AdminSubmissionQueue, 32) );
    
    CONTROLLER_WRITE_REG32( NvmeController, Offset_Acq, 
                               (UINT32)NvmeController->AdminCompletionQueue );
    
    CONTROLLER_WRITE_REG32( NvmeController, Offset_Acq + 4, 
                               (UINT32) RShiftU64(NvmeController->AdminCompletionQueue, 32) );

    // Enable Controller
    CONTROLLER_REG32_OR( NvmeController, Offset_CC, 0x1 );

    // Wait for the controller to get ready
    // Check if the controller is already running. If yes stop it.
    Delay = NvmeController->TimeOut * 500;
    do {
        if ((CONTROLLER_REG32(NvmeController, Offset_CSTS) & 0x1)) {
            break;
        }
        
        MicroSecondDelay (1000);
    } while (--Delay);
    
    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }

    NvmeController->NvmeInitControllerFlag = TRUE;
    return EFI_SUCCESS;
}

/**
    Initialize NVMe SMM data area

    @param   DispatchHandle,
    @param   *Context         OPTIONAL,
    @param   *CommBuffer      OPTIONAL,
    @param   *CommBufferSize  OPTIONAL

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeInitSmmData (
    IN  EFI_HANDLE  DispatchHandle,
    IN  CONST   VOID    *Context        OPTIONAL,
    IN  OUT     VOID    *CommBuffer     OPTIONAL,
    IN  OUT     UINTN   *CommBufferSize OPTIONAL
  )
{

    EFI_STATUS                    Status = EFI_SUCCESS;
    ACTIVE_NAMESPACE_DATA         *OrgActiveNameSpace;
    ACTIVE_NAMESPACE_DATA         *ActiveNameSpace;
    AMI_NVME_CONTROLLER_PROTOCOL  *NvmeController;
    AMI_NVME_CONTROLLER_PROTOCOL  *OrgNvmeController;
    UINT8                         Count;
    LIST_ENTRY                    *LinkData;
    EFI_BLOCK_IO_MEDIA            *Media = NULL;
    EFI_HANDLE                    AmiSmmNvmeControllerHandle=NULL;
    
    // After the first API call is invoked, don't initialize SMM data area. This is an additional
    // Security check so that data won't get corrupted.
    if (gFirstAPICall) {
        return EFI_SUCCESS;
    }
    
    // If input is invalid, stop processing this SMI
    if (CommBuffer == NULL || CommBufferSize == NULL) {
        return EFI_SUCCESS;
    }

    OrgNvmeController = (AMI_NVME_CONTROLLER_PROTOCOL  *)(*(UINTN *)CommBuffer);
            
    // Locate a free slot to copy the pointer
     for (Count = 0; Count < NVMEDEVS_MAX_ENTRIES; Count++ ){
         if (!gNvmeController[Count]) {
             break;
         }
     }
     
     if (Count == NVMEDEVS_MAX_ENTRIES) {
         return EFI_OUT_OF_RESOURCES;
     }
     
    Status = pSmst2->SmmAllocatePool ( 
                                EfiRuntimeServicesData, 
                                sizeof (AMI_NVME_CONTROLLER_PROTOCOL),
                                (VOID**)&NvmeController);
    ASSERT_EFI_ERROR(Status);
    
    // Copy input NvmeController passed in OrgNvmeController into SMM
    CopyMem ((VOID *)NvmeController, OrgNvmeController, sizeof (AMI_NVME_CONTROLLER_PROTOCOL));
    
    
    // Copy IDENTIFY_CONTROLLER_DATA
    Status = pSmst2->SmmAllocatePool ( 
                                EfiRuntimeServicesData, 
                                sizeof (IDENTIFY_CONTROLLER_DATA),
                                (VOID**)&(NvmeController->IdentifyControllerData));
    ASSERT_EFI_ERROR(Status);
    
    CopyMem (NvmeController->IdentifyControllerData, OrgNvmeController->IdentifyControllerData, sizeof(IDENTIFY_CONTROLLER_DATA));
    
    gNvmeController[Count] = NvmeController;
 
    
    // Initialize some of the pointers to NULL which aren't applicable during runtime
    NvmeController->PciIO = NULL;
    NvmeController->NvmeInSmm = TRUE;

    //Initilize the Controller Init API for SMM.
    NvmeController->NvmeInitController = NvmeInitController;

    // Flag to denote NvmeInitController API is called or not
    NvmeController->NvmeInitControllerFlag = FALSE;

    InitializeListHead (&NvmeController->ActiveNameSpaceList);
    
    pSmst2->SmmAllocatePool ( EfiRuntimeServicesData, 
                              sizeof (NVME_COMMAND_WRAPPER),
                              (VOID**)&(NvmeController->NvmeCmdWrapper));
    
    ZeroMem (NvmeController->NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER));
    
    // use original NVMe buffer for this as original address value is used.
    // Update the NvmeController pointer inside ActiveNameSpace
    for (LinkData = OrgNvmeController->ActiveNameSpaceList.ForwardLink; 
            LinkData != &OrgNvmeController->ActiveNameSpaceList; 
            LinkData = LinkData->ForwardLink) {
    
        OrgActiveNameSpace = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);
        
        pSmst2->SmmAllocatePool ( EfiRuntimeServicesData, 
                                  sizeof (ACTIVE_NAMESPACE_DATA),
                                  (VOID**)&(ActiveNameSpace));
        
        
        CopyMem (ActiveNameSpace, OrgActiveNameSpace, sizeof(ACTIVE_NAMESPACE_DATA));
        
        ActiveNameSpace->NvmeController = NvmeController;
        ActiveNameSpace->EfiDevicePath = NULL;

        pSmst2->SmmAllocatePool ( EfiRuntimeServicesData, 
                                  sizeof (EFI_BLOCK_IO_MEDIA),
                                  (VOID**)&(Media) );
        if( EFI_ERROR(Status) ) {
            ASSERT_EFI_ERROR(Status);
            return Status;
        }

        CopyMem ( Media, OrgActiveNameSpace->NvmeBlockIO.Media, sizeof(EFI_BLOCK_IO_MEDIA) );
        ActiveNameSpace->NvmeBlockIO.Media = Media;

        InsertTailList (&NvmeController->ActiveNameSpaceList, &ActiveNameSpace->Link);
        
    }

    Status = pSmst2->SmmInstallProtocolInterface(
                        &AmiSmmNvmeControllerHandle,
                        &gAmiSmmNvmeControllerProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        NvmeController
                        );

    return Status;
    
}

/**
    Handle SWSMI generated from NVMe CSM16 module 

    @param  DispatchHandle,
    @param  *DispatchContext OPTIONAL,
    @param  *CommBuffer OPTIONAL,
    @param  *CommBufferSize OPTIONAL
  
    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeSWSMIHandler (
    IN  EFI_HANDLE      DispatchHandle,
    IN  CONST   VOID    *DispatchContext OPTIONAL,
    IN  OUT     VOID    *CommBuffer OPTIONAL,
    IN  OUT     UINTN   *CommBufferSize OPTIONAL
)
{
    EFI_STATUS  Status =EFI_SUCCESS;
    NVME_STRUC  *NvmeURP=NULL;
    UINT8       bFuncIndex;
    UINT8       bNumberOfFunctions;
    UINT16      EbdaSeg;

    gFirstAPICall = TRUE;
    
    // Get the fpURP pointer from EBDA
        
    EbdaSeg = *((UINT16*)0x40E);
    NvmeURP = *(NVME_STRUC**)(UINTN)(((UINT32)EbdaSeg << 4) + NVME_DATA_EBDA_OFFSET);
    NvmeURP = (NVME_STRUC*)((UINTN)NvmeURP & 0xFFFFFFFF);
    

    // Validate if URP address is an non-SMRAM region to avoid SMRAM data 
    // corruption through SMI handlers
    Status = AmiValidateMemoryBuffer((VOID*)NvmeURP, sizeof(NVME_STRUC));
    if (EFI_ERROR(Status)) {
    	// return without modifying the NvmeUrp buffer as it's not belongs to the normal Memory Region.
        return Status;
    }
    
    if  (NvmeURP->bFuncNumber != NVME_API_MASS_DEVICE_REQUEST) {
        NvmeURP->bRetValue = NVME_PARAMETER_FAILED;
        return Status;    
    }
    
    bFuncIndex = NvmeURP->bSubFunc;
    bNumberOfFunctions = sizeof NvmeMassApiTable / sizeof (API_FUNC *);

    //
    // Make sure function number is valid; if function number is not zero
    // check for valid extended SDIO API function
    //
    if (bFuncIndex >= bNumberOfFunctions ) {
        NvmeURP->bRetValue = NVME_PARAMETER_FAILED;
        return Status;
    }

    //
    // Call the appropriate function
    //

    NvmeMassApiTable[bFuncIndex](NvmeURP);

    return Status;
}

/**
    Loads NVMe SMM module into SMM and registers SMI handler

    @param ImageHandle,
    @param *SystemTable

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeSmmDriverEntryPoint (
    IN  EFI_HANDLE       ImageHandle,
    IN  EFI_SYSTEM_TABLE *SystemTable
    )
{
    
    EFI_STATUS                      Status;
    EFI_HANDLE                      SwHandle = NULL;
    EFI_SMM_SW_DISPATCH2_PROTOCOL   *pSwDispatch = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
    EFI_HANDLE                      DispatchHandle;
    EFI_HANDLE                      AmiSmmNvmePassThruHandle=NULL;

    
    Status = gBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &gSmmBase2);
    if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
    }

    Status = gSmmBase2->GetSmstLocation (gSmmBase2, &pSmst2);
    if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
    }

    Status = pSmst2->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid,
                                       NULL,
                                       &pSwDispatch);
    if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
    }

    SwContext.SwSmiInputValue = NVME_SWSMI;
    Status = pSwDispatch->Register (pSwDispatch,
                                    NvmeSWSMIHandler,
                                    &SwContext,
                                    &SwHandle);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    //
    //Allocate Memory for NVMe global Data.
    //
    Status = pSmst2->SmmAllocatePool(EfiRuntimeServicesData,sizeof(NVME_GLOBAL_DATA), &gNvmeData);
    ASSERT_EFI_ERROR(Status);
    //
    //  Clear the Buffer
    //
    ZeroMem((VOID*)gNvmeData, sizeof(NVME_GLOBAL_DATA));
    
    
    //
    // Register Nvme handler to transfer data from DXE driver to SMM
    //
    Status = pSmst2->SmiHandlerRegister (
                      NvmeInitSmmData,
                      &gAmiSmmNvmeCommunicationGuid,
                      &DispatchHandle
                      );
    
    ASSERT_EFI_ERROR (Status);

    gAmiSmmNvmePassThru.ExecuteNvmeCmd = ExecuteNvmeCmd;

    Status = pSmst2->SmmInstallProtocolInterface(
                    &AmiSmmNvmePassThruHandle,
                    &gAmiSmmNvmePassThruProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gAmiSmmNvmePassThru
                    );

    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
