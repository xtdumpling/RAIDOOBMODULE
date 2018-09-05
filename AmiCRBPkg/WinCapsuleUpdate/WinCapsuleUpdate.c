//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file WinCapsuleUpdate

    This file changes boot mode if processing windows Capsule update
*/

#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Ppi/Capsule.h>

#define AMI_PEI_AFTER_MRC_GUID \
 {0x64c96700, 0x6b4c, 0x480c, 0xa3, 0xe1, 0xb8, 0xbd, 0xe8, 0xf6, 0x2, 0xb2}
EFI_GUID gAmiPeiAfterMrcGuid            = AMI_PEI_AFTER_MRC_GUID;

EFI_STATUS
WinCapsuleUpdateChangeBootMode (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
    IN VOID                               *InvokePpi
);

static EFI_PEI_NOTIFY_DESCRIPTOR  mPeiNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiPeiAfterMrcGuid,
    WinCapsuleUpdateChangeBootMode
  }
};

/**

  PEI termination callback.

  @param PeiServices       -  General purpose services available to every PEIM.
  @param NotifyDescriptor  -  Not uesed.
  @param Ppi               -  Not uesed.

  @retval EFI_SUCCESS  -  If the interface could be successfully
                  @retval installed.

**/
EFI_STATUS
WinCapsuleUpdateChangeBootMode (
    IN EFI_PEI_SERVICES              **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
    IN VOID                          *InvokePpi
)
{
    EFI_STATUS                       Status = EFI_SUCCESS;
    EFI_BOOT_MODE                    BootMode;
    PEI_CAPSULE_PPI                  *CapsulePpi;
    
    DEBUG ((EFI_D_INFO, "WinCapsuleUpdateChangeBootMode - Start\n"));
    
    Status = PeiServicesGetBootMode (&BootMode);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, " Get Boot Mode is fail\n"));
      return Status;
    }

    DEBUG ((EFI_D_INFO, " BootMode = %X\n", BootMode));
    if (!EFI_ERROR(Status) && (BootMode == BOOT_ON_S3_RESUME)) {
      Status = PeiServicesLocatePpi (
                                   &gEfiPeiCapsulePpiGuid,
                                   0,
                                   NULL,
                                   (VOID **) &CapsulePpi
                                   );

      if (!EFI_ERROR (Status)) {
        Status = CapsulePpi->CheckCapsuleUpdate(PeiServices);
        if (!EFI_ERROR (Status)) {
          BootMode = BOOT_ON_FLASH_UPDATE;
          PeiServicesSetBootMode(BootMode);
          DEBUG ((EFI_D_INFO, " Change boot mode to BOOT_ON_FLASH_UPDATE\n"));
        }
        else
          DEBUG ((EFI_D_INFO, " No Capsule data found!\n"));
      }
      else {
        DEBUG ((EFI_D_INFO, " Cannot locate CapsulePpi!\n"));
      }
    }

    DEBUG ((EFI_D_INFO, "WinCapsuleUpdateChangeBootMode - End\n"));
    return Status;
}

/**

  Driver entry point                

  @param FfsHeader -  Not used.
  @param PeiServices - General purpose services available to every PEIM.

  @retval EFI_SUCCESS  NotifyPpi installation completed successfully.
  @retval All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
WinCapsuleUpdateEntryPoint (
    IN        EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES        **PeiServices 
    )
{
        EFI_STATUS          Status = EFI_SUCCESS;
        EFI_PEI_NOTIFY_DESCRIPTOR   *pNotifyList;

        pNotifyList = &mPeiNotifyList[0];
        Status = (*PeiServices)->NotifyPpi(PeiServices, pNotifyList);
        
        return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
