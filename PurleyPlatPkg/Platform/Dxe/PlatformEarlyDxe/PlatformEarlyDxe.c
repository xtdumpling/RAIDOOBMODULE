/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights 
reserved This software and associated documentation (if any) is 
furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by 
such license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PlatformEarlyDxe.c

**/

#include "PlatformEarlyDxe.h"
#include <Setup/IioUniversalData.h>
#include <Include/Protocol/SiliconRegAccess.h>

USRA_PROTOCOL     *mUsraProtocol = NULL;


IIO_UDS           *mIioUds;


/**

  This function will retrieve the DPR data from HOBs produced by MRC
  and will use it to program the DPR registers in IIO and in PCH

  @param VOID

  @retval VOID

**/
VOID
ProgramDPRregs (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_PLATFORM_TXT_DEVICE_MEMORY  *PlatformTxtMemory;
  EFI_HOB_GUID_TYPE               *GuidHob      = NULL;
  VOID                            *HobData      = NULL;
  EFI_HANDLE                      Handle        = NULL;
  volatile UINT64                 *Ptr64;
  UINT64                          Value64;
  UINT32                          Value32;
  UINTN                           PciAddress;
  UINT32                          Temp32;
  UINT8                           i;
  UINT8                           Socket;
  USRA_ADDRESS                    Address;

  //
  // Get Txt Device Memory HOB if it was published
  //
  GuidHob = GetFirstGuidHob (&gEfiPlatformTxtDeviceMemoryGuid);

  if (GuidHob != NULL) {

    HobData = GET_GUID_HOB_DATA (GuidHob);
    PlatformTxtMemory = (EFI_PLATFORM_TXT_DEVICE_MEMORY*)HobData;

    //
    // Loop thru all IIO stacks of all sockets that are present
    //
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if ((mIioUds->SystemStatus.socketPresentBitMap & (1<<Socket)) == 0) {
        continue;
      }
      for (i = 0; i < MAX_IIO_STACK; i++) {
        if ((mIioUds->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap & (1<<i)) == 0) {
          continue;
        }
        //
        // Read IIO LTDPR Register current value
        //
        USRA_CSR_OFFSET_ADDRESS(Address, Socket, i, LTDPR_IIO_VTD_REG, CsrBoxInst);
        mUsraProtocol->RegRead(&Address, &Value32);

        if ((PlatformTxtMemory->DMAProtectionMemoryRegionAddress != 0) && ((Value32 & 0x0FF5) == 0)) {
          //
          // Program IIO LTDPR Register if not programmed already
          //
          Temp32 = ((UINT32)(PlatformTxtMemory->DMAProtectionMemoryRegionSize) >> 16) | SA_DPR_LOCK | SA_DPR_PRM;
          Value32 = Value32 & (~(0x0FF5)) | Temp32;
          mUsraProtocol->RegWrite(&Address, &Value32);
          //
          // Save for S3 resume
          //
          PciAddress = mUsraProtocol->GetRegAddr(&Address);

          S3BootScriptSaveMemWrite (
             S3BootScriptWidthUint32,
             (UINTN)PciAddress,
             1,
             &Value32
             );

          DEBUG ((EFI_D_ERROR, "Socket%2d Stack%2d LTDPR_IIO_VTD Register[%08x] written with %08x,  ", Socket,i, PciAddress, Value32));
          //
          // Verify register value by reading it back
          //BUGBUG: need to double check DPR REG//ASSERT (((Value32 & 0xFF0) << 16) == LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize);
          //
          Value32 = *(UINT32*)PciAddress;
          DEBUG ((EFI_D_ERROR, "read back as %08x\n", Value32));
          if (((Value32 & 0xFF0) << 16) != PlatformTxtMemory->DMAProtectionMemoryRegionSize){
            DEBUG ((EFI_D_ERROR, "ProgramDPRregs: LTDPR_IIO_VTD_REG[0x%02x] = 0x%08x not set correctly!!! \n", PciAddress, Value32));
          }
        }
      }
    }

    //
    // Program DPR register in LT Public Space on PCH
    //
    Ptr64   = (UINT64 *) (UINTN) (LT_PUBLIC_SPACE_BASE_ADDRESS + LT_DMA_PROTECTED_RANGE_REG_OFF);
    Value64 = RShiftU64 (PlatformTxtMemory->DMAProtectionMemoryRegionSize, 16) | SA_DPR_LOCK | SA_DPR_PRM;
    //Add top of the DPR
    Value64 |= ((PlatformTxtMemory->DMAProtectionMemoryRegionAddress + PlatformTxtMemory->DMAProtectionMemoryRegionSize) & 0xFFF00000);
    *Ptr64  = Value64;
    Value64 = *Ptr64;
    //
    // Save for S3 resume if LT DPR Reg was programmed correctly and clear the protected region
    //
    if ((LShiftU64 ((Value64 & 0xFFE), 16)) == PlatformTxtMemory->DMAProtectionMemoryRegionSize) {
       DEBUG ((EFI_D_ERROR, "PCH TXT Public Space DPR reg (at FED3_0000 + 0x330) = %08x,\n", (UINT32) Value64));
       S3BootScriptSaveMemWrite (
           S3BootScriptWidthUint64,
           (UINT64) (UINTN) (Ptr64),
           2,
           &Value64
           );
       //
       // Clear out the DMA protected memory area for use.
       //
       ZeroMem (
         (VOID *)(UINTN)PlatformTxtMemory->DMAProtectionMemoryRegionAddress,
         (UINTN)PlatformTxtMemory->DMAProtectionMemoryRegionSize
       );
    }else {
      DEBUG ((EFI_D_ERROR, "ProgramDPRregs: PCH TXT Public Space DPR reg programming error!!! \n"));
    }


  } else {
    DEBUG((EFI_D_INFO, "ProgramDPRregs: EfiPlatformTxtDeviceMemoryGuid HOB not found.  DPR regs not programmed.\n"));
  }
  //
  // Install dummy protocol to signal DPR regs are programmed.
  // Note that SMM driver BiosGuardServices depends on this protocol, so we always install it here.
  //
  DEBUG((EFI_D_INFO, "ProgramDPRregs: Installing gEfiDprRegsProgrammedGuid Protocol.\n"));
  Status = gBS->InstallProtocolInterface (
        &Handle,
        &gEfiDprRegsProgrammedGuid,
        EFI_NATIVE_INTERFACE,
        NULL
        );
  ASSERT_EFI_ERROR (Status);

  return;
}

/**

  This function will program the legacy range and RCBA range into the
  General Protected Range registers in IIO.

  @param VOID

  @retval VOID

**/
VOID
ProgramGenProtRangeRegs( VOID )
{
  USRA_ADDRESS              Address;
  UINT32                    Data32;
  UINT8                     Socket;
  UINT8                     Stack;
  //
  // Loop thru all IIO stacks of all sockets that are present
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if ((mIioUds->SystemStatus.socketPresentBitMap & (1<<Socket)) == 0) {
      continue;
    }
    for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      if ((mIioUds->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap & (1<<Stack)) == 0) {
        continue;
      }
      //
      //  Legacy address ranges need to be protected from inbound memory reads to prevent CTOs.
      //  (HSD 3614978, 3876023, 3247123, 3876024, 3876008)
      //
      Data32 = 0x0000;   // Base address of PAM region [63:32]
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE2_BASE_N1_IIO_VTD_REG, CsrBoxInst);
      mUsraProtocol->RegWrite(&Address, &Data32);

      Data32 = 0xA0000;   // Base address of PAM region [31:16], [15:0] assumed zero
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE2_BASE_N0_IIO_VTD_REG, CsrBoxInst);
      mUsraProtocol->RegWrite(&Address, &Data32);

      Data32 = 0x0000;   // Limit address of PAM region [63:32]
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE2_LIMIT_N1_IIO_VTD_REG, CsrBoxInst);
      mUsraProtocol->RegWrite(&Address, &Data32);

      Data32 = 0xF0000;   // Limit address of PAM region [31:16], [15:0] assumed one
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE2_LIMIT_N0_IIO_VTD_REG, CsrBoxInst);
      mUsraProtocol->RegWrite(&Address, &Data32);

      //
      // Cover SPI MMIO region with GenProtRange register
      //
      Data32 = 0x0000;   // Base address  [63:32] of SPI region
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE1_BASE_N1_IIO_VTD_REG, CsrBoxInst);
// APTIOV_SERVER_OVERRIDE_RC_START : Restore SPI region for BIOSGuard.
      if (FeaturePcdGet (PcdUsraSupportS3))
        Address.Attribute.S3Enable = USRA_ENABLE;
// APTIOV_SERVER_OVERRIDE_RC_END : Restore SPI region for BIOSGuard.
      mUsraProtocol->RegWrite(&Address, &Data32);
      Data32 = PCH_SPI_BASE_ADDRESS & 0xFFFF0000;  // Base address of SPI MMIO region is 0xFE010000
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE1_BASE_N0_IIO_VTD_REG, CsrBoxInst);
// APTIOV_SERVER_OVERRIDE_RC_START : Restore SPI region for BIOSGuard.
      if (FeaturePcdGet (PcdUsraSupportS3))
        Address.Attribute.S3Enable = USRA_ENABLE;
// APTIOV_SERVER_OVERRIDE_RC_END : Restore SPI region for BIOSGuard.
      mUsraProtocol->RegWrite(&Address, &Data32);

      Data32 = 0x0000;   // Limit address of SPI region
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE1_LIMIT_N1_IIO_VTD_REG, CsrBoxInst);
// APTIOV_SERVER_OVERRIDE_RC_START : Restore SPI region for BIOSGuard.
      if (FeaturePcdGet (PcdUsraSupportS3))
        Address.Attribute.S3Enable = USRA_ENABLE;
// APTIOV_SERVER_OVERRIDE_RC_END : Restore SPI region for BIOSGuard.
      mUsraProtocol->RegWrite(&Address, &Data32);
      Data32 = (PCH_SPI_BASE_ADDRESS + 0x1000) & 0xFFFFF000;  // End of SPI Base region is 0xFE011000
      USRA_CSR_OFFSET_ADDRESS(Address, Socket, Stack, GENPROTRANGE1_LIMIT_N0_IIO_VTD_REG, CsrBoxInst);
// APTIOV_SERVER_OVERRIDE_RC_START : Restore SPI region for BIOSGuard.
      if (FeaturePcdGet (PcdUsraSupportS3))
        Address.Attribute.S3Enable = USRA_ENABLE;
// APTIOV_SERVER_OVERRIDE_RC_END : Restore SPI region for BIOSGuard.
      mUsraProtocol->RegWrite(&Address, &Data32);
    }
  }

}


VOID
SetBiosInfoFlagWpe (
  VOID
  )
/*++

Routine Description:

  Sets the WPE bit of the BIOS Info Flags MSR to enable Anti-Flash wearout protection
  within BIOS Guard

Arguments:

  None

Returns:

  None

--*/
{
  AsmWriteMsr64 (MSR_BIOS_INFO_FLAGS, B_MSR_BIOS_INFO_FLAGS_WPE);

  return;
}

VOID
EFIAPI
EnableAntiFlashWearout (
      EFI_EVENT                 Event,
      VOID                      *Context
  )
/*++

Routine Description:

  Function to set the WPE bit of the BIOS Info Flags MSR to enable Anti-Flash wearout
  protection within BIOS Guard before booting to the OS

Arguments:

  IN EFI_EVENT         Event
  IN VOID              *Context

Returns:

  None

--*/
{
  EFI_STATUS                    Status;
  EFI_MP_SERVICES_PROTOCOL      *MpServices = NULL;
  SYSTEM_CONFIGURATION          SetupData;

  Status = GetSpecificConfigGuid (&gEfiSetupVariableGuid, (VOID *)&SetupData);
  ASSERT_EFI_ERROR(Status);
  //
  // First check if Anti-flash wearout feature is supported by platform and Setup variable is enabled
  //
  if (SetupData.AntiFlashWearoutSupported == TRUE && SetupData.EnableAntiFlashWearout) {
    Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpServices
                  );
    ASSERT_EFI_ERROR(Status);

    //
    // Set WPE on BSP, then all other APs
    //
    SetBiosInfoFlagWpe();

    MpServices->StartupAllAPs (
                    MpServices,
                    (EFI_AP_PROCEDURE) SetBiosInfoFlagWpe,
                    FALSE,
                    NULL,
                    0,
                    NULL,
                    NULL
                    );
  }

}

EFI_STATUS
EFIAPI
PlatformEarlyDxeEntry (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
/**

  This is the main entry point of the Platform Early DXE module.

  @param ImageHandle  -  Handle for the image of this driver.
  @param SystemTable  -  Pointer to the EFI System Table.

  @retval EFI_SUCCESS  -  Module launched successfully.

**/
{
  EFI_STATUS            Status = EFI_SUCCESS;
  EFI_EVENT             EndOfDxeEvent;
  UINT32                BspCpuidSignature;
  UINT32                RegEax, RegEbx, RegEcx, RegEdx;
  EFI_HOB_GUID_TYPE     *GuidHob;
  IIO_UDS               *UdsHobPtr;
  EFI_BOOT_MODE         BootMode;
  EFI_GUID              UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;

  //
  // Get the IIO_UDS data HOB
  //
  GuidHob    = GetFirstGuidHob (&UniversalDataGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  UdsHobPtr = GET_GUID_HOB_DATA(GuidHob); 
  //
  // Allocate Memory Pool buffer for IIO_UDS data
  //
  Status = gBS->AllocatePool ( EfiBootServicesData, sizeof (IIO_UDS), (VOID **) &mIioUds );
  ASSERT_EFI_ERROR (Status);
  //
  // Initialize the Memory Pool buffer with the data from the Hand-Off-Block
  //
  CopyMem(mIioUds, UdsHobPtr, sizeof(IIO_UDS));

  //
  // Locate USRA protocol  for common register access (this protocol is assumed to have been installed)
  //
  Status = gBS->LocateProtocol (&gUsraProtocolGuid, NULL, &mUsraProtocol);
  ASSERT_EFI_ERROR (Status);
  //
  // Get the boot mode that we are currently in
  //
  BootMode = GetBootModeHob();

  //
  // Program DPR registers with the range from Memory Init
  //
  ProgramDPRregs();

  //
  // Program the GenProtRange registers for BIOS Guard
  //
  ProgramGenProtRangeRegs();

  //
  // Get BSP CPU ID
  // Shift out the stepping
  //
  AsmCpuid (0x01, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  BspCpuidSignature = (RegEax >> 4) & 0x0000FFFF;
  if ( (BspCpuidSignature == CPU_FAMILY_SKX) && (BootMode != BOOT_ON_FLASH_UPDATE)) {
    //
    // Register event to set WPE bit in Bios Info Flags MSR to enable Anti Flash wearout
    //
    Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                EnableAntiFlashWearout,
                NULL,
                &gEfiEndOfDxeEventGroupGuid,
                &EndOfDxeEvent
                );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;

}
