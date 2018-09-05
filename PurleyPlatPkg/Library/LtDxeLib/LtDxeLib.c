/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

  Copyright (c) 2015 - 2016 Intel Corporation. All rights 
  reserved This software and associated documentation (if any) 
  is furnished under a license and may only be used or copied in 
  accordance with the terms of the license. Except as permitted 
  by such license, no part of this software or documentation may 
  be reproduced, stored in a retrieval system, or transmitted in 
  any form or by any means without the express written consent 
  of Intel Corporation. 


    @file LtDxeLib.c

  This has platform-specific code for LT.  It contains hooks used by
  ServerCommon code that must be run during DXE for enabling/disabling/examining
  LT/TXT functions and status.

**/

#include <Uefi.h>
#include <Platform.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/SetupLib.h>
#include <Guid/SocketVariable.h>
#include <Guid/SocketProcessorCoreVariable.h>
#include <Protocol/IioSystem.h>
#include <Protocol/Spi.h>
//APTIOV_SERVER_OVERRIDE_START - Resolve build error
#include <Universal/GetSec/Dxe/TxtDxeLib.h>
//APTIOV_SERVER_OVERRIDE_END - Resolve build error
#include <Platform/Dxe/PlatformEarlyDxe/PlatformEarlyDxe.h>
#include <Guid/DebugMask.h>
#include <Register/PchRegsPmc.h>    // Lewisburg
#include <Library/PlatformLtDxeLib.h>
#include <PlatformInfo.h>
#include "RcRegs.h"

#ifdef PC_HOOK
#endif

// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load SINIT binary from FV_MAIN, to avoid separate FD AREA
#define AMI_LTSX_SINIT_GUID \
	{0x8bd816b2, 0x655f, 0x4c71, 0x90, 0x32, 0xc9, 0x0d, 0x72, 0xc0, 0x7c, 0x03}

EFI_GUID gAmiLtsxSinitGuid          = AMI_LTSX_SINIT_GUID;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load SINIT binary from FV_MAIN, to avoid separate FD AREA
// Local macros
//
#define LT_SINIT_BASE_REG_OFF       0x270
#define LT_SINIT_SIZE_REG_OFF       0x278
#define LT_HEAP_BASE_REG_OFF        0x300
#define LT_HEAP_SIZE_REG_OFF        0x308

// External variables
//
extern  EFI_RUNTIME_SERVICES        *gRT;

// External PROCs
//
UINT8
EFIAPI
IoRead8 (
  IN UINTN   Port
  );

UINT8
EFIAPI
IoWrite8 (
  IN UINTN   Port,
  IN UINT8   Value
  );

// Helper PROCs for ServerCommonPkg/Universal/GetSec
//
/**

  Returns FLASH_REGION_SINIT_ACM_BASE to non-platform code

  @param None

  @retval PCD for FLASH_REGION_SINIT_ACM_BASE

**/
UINTN
GetSinitAddress (
  VOID
  )
{
  return ((UINTN) FLASH_REGION_SINIT_ACM_BASE);
};

EFI_STATUS
GetBiosAcmErrorReset (
  UINT8 *BiosAcmErrorResetBit
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                NULL,
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  if (Status == EFI_SUCCESS) {
    *BiosAcmErrorResetBit = SocketData.SocketProcessorCoreConfiguration.BiosAcmErrorReset;
  }

  return (Status);
}

EFI_STATUS
GetAcmType (
  UINT8 *AcmTypeValue
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                NULL,
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  if (Status == EFI_SUCCESS) {
    *AcmTypeValue = SocketData.SocketProcessorCoreConfiguration.AcmType;
  }

  return (Status);
}

EFI_STATUS
SetAcmType (
  IN UINT8 AcmTypeValue
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;
  // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
  UINT32                    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
                &Attributes,
                // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  SocketData.SocketProcessorCoreConfiguration.AcmType = AcmTypeValue;

  Status = gRT->SetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
                Attributes,
                // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute
                SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  return (Status);
}

EFI_STATUS
GetLockChipset (
  UINT8 *LockChipsetBit
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                NULL,
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  if (Status == EFI_SUCCESS) {
    *LockChipsetBit = SocketData.SocketProcessorCoreConfiguration.LockChipset;
  }

  return (Status);
}

EFI_STATUS
GetEVMode (
  UINT8 *EVModeBit
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                NULL,
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  if (Status == EFI_SUCCESS) {
    *EVModeBit = SocketData.SocketProcessorCoreConfiguration.EVMode;
  }

  return (Status);
}

EFI_STATUS
SetLockChipset (
  IN UINT8 LockChipsetValue
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;
  // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
  UINT32                    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
                &Attributes,
                // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  SocketData.SocketProcessorCoreConfiguration.LockChipset = LockChipsetValue;

  Status = gRT->SetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
                Attributes,
                // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute
                SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  return (Status);
}

EFI_STATUS
SetEVMode (
  IN UINT8 EVModeValue
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;
  // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
  UINT32                    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_RC_OVERRIDE_START : Remove NVRAM RT Attribute
                &Attributes,
                // APTIOV_SERVER_RC_OVERRIDE_END : Remove NVRAM RT Attribute
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  SocketData.SocketProcessorCoreConfiguration.EVMode = EVModeValue;

  Status = gRT->SetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_RC_OVERRIDE_START : Remove NVRAM RT Attribute
                Attributes,
                // APTIOV_SERVER_RC_OVERRIDE_END : Remove NVRAM RT Attribute
                SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  return (Status);
}

EFI_STATUS
GetProcessorLtsxEnable (
  UINT8 *ProcessorLtsxEnableBit
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                NULL,
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  if (Status == EFI_SUCCESS) {
    *ProcessorLtsxEnableBit = SocketData.SocketProcessorCoreConfiguration.ProcessorLtsxEnable;
  }

  return (Status);
}

EFI_STATUS
SetDisableLtsx (
  VOID
  )
{
  EFI_STATUS                Status;
  UINTN                     SysCfgSize;
  SOCKET_CONFIGURATION      SocketData;
  UINT8                     Data8 = 1;
  // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
  UINT32                    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute
  //
  // Write NV flag so that TXT errors can be handled in next boot.
  //
  gRT->SetVariable (
         L"AcmError",
         &gEfiGenericVariableGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof(UINT8),
         &Data8
         );

  SysCfgSize = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);
  Status = gRT->GetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
                &Attributes,
                // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute
                &SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );

  SocketData.SocketProcessorCoreConfiguration.ProcessorLtsxEnable = 0;
  SocketData.SocketProcessorCoreConfiguration.ProcessorSmxEnable = 0;
  //Keep VT and LockChipset unchanged to aovid disabling them unexpectedly.
  //SocketData.SocketProcessorCoreConfiguration.ProcessorVmxEnable = 0;
  //SocketData.SocketProcessorCoreConfiguration.LockChipset = 0;

  Status = gRT->SetVariable (
                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,
                &gEfiSocketProcessorCoreVarGuid,
                // APTIOV_SERVER_OVERRIDE_RC_START : Remove NVRAM RT Attribute
                Attributes,
                // APTIOV_SERVER_OVERRIDE_RC_END : Remove NVRAM RT Attribute
                SysCfgSize,
                &(SocketData.SocketProcessorCoreConfiguration)
                );
  
  PlatformHookForDisableLtsx ();
  
  return (Status);
}

VOID
SetDisableSoftwareSmi (
  VOID
  )
{
  UINT8 Buffer8 = IoRead8 (PCH_ACPI_BASE_ADDRESS + R_PCH_SMI_EN) & ~(B_PCH_SMI_EN_SWSMI_TMR);
  IoWrite8 (PCH_ACPI_BASE_ADDRESS + R_PCH_SMI_EN, Buffer8);
}

// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to load SINIT binary from FV_MAIN, to avoid separate FD AREA
//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   GetRawImage
//
// Description: Loads binary from RAW section of main firwmare volume
//
//
// Input:       IN     EFI_GUID   *NameGuid,
//              IN OUT VOID   **Buffer,
//              IN OUT UINTN  *Size
//
// Output:      EFI STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS GetRawImage(
    IN EFI_GUID  *NameGuid,
    IN OUT VOID  **Buffer,
    IN OUT UINTN *Size  )
{
    EFI_STATUS                   Status;
    EFI_HANDLE                   *HandleBuffer = 0;
    UINTN                        HandleCount   = 0;
    UINTN                        i;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
    UINT32                       AuthenticationStatus;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiFirmwareVolume2ProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
        );
    if ( EFI_ERROR( Status ) || HandleCount == 0 ) {
        return EFI_NOT_FOUND;
    }

    //
    // Find desired image in all Fvs
    //
    for ( i = 0; i < HandleCount; i++ ) {
        Status = gBS->HandleProtocol(
            HandleBuffer[i],
            &gEfiFirmwareVolume2ProtocolGuid,
            &Fv
            );

        if ( EFI_ERROR( Status )) {
            gBS->FreePool( HandleBuffer );
            return EFI_LOAD_ERROR;
        } // if EFI_ERROR

        //
        // Try a raw file
        //
        *Buffer = NULL;
        *Size   = 0;
        Status  = Fv->ReadSection(
            Fv,
            NameGuid,
            EFI_SECTION_RAW,
            0,
            Buffer,
            Size,
            &AuthenticationStatus
            );

        if ( !EFI_ERROR( Status )) {
            break;
        } // if EFI_ERROR
    } // for i
    gBS->FreePool( HandleBuffer );

    if ( i >= HandleCount ) {
        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
}
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to load SINIT binary from FV_MAIN, to avoid separate FD AREA

EFI_STATUS
SetupLtDeviceMemory (
  IN LT_DXE_LIB_CONTEXT *LtDxeCtx
  )
{
  EFI_STATUS                          Status;
  volatile UINT64                     *Ptr64;
  UINT64                              Value64;
  UINT32                              Value32;
  UINT32                              Temp32;
  EFI_PHYSICAL_ADDRESS                Addr;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridge;
  UINT64                              PciAddress;
  EFI_IIO_SYSTEM_PROTOCOL             *IioSystemProtocol;
  IIO_GLOBALS                         *IioGlobalData;
  UINT8                               Socket;
  UINT8                               Iio;
  UINT8                               Size = 4;
  EFI_PLATFORM_INFO                   *PlatformInfoHob;
  EFI_HOB_GUID_TYPE                   *GuidHob;

#if 1 //SINIT_IN_FLASH
// APTIOV_SERVER_OVERRIDE_RC_START
//  UINTN   SinitAddress;
  UINT8*  SinitAddress;
// APTIOV_SERVER_OVERRIDE_RC_END
  UINTN   SinitSize;
#endif
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;
  
  //
  // Locate Platform Info Hob
  //
  GuidHob = GetFirstGuidHob(&gEfiPlatformInfoGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfoHob = GET_GUID_HOB_DATA(GuidHob);
  if (PlatformInfoHob == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // Check whether we need to allocate memory
  //
  if ((LtDxeCtx->PlatformTxtDeviceMemory->SINITMemoryAddress == 0) ||
      (LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress == 0)) {
    //
    // We do not need support the case that SINIT/TXT heap unallocated, because they must be in DPR region.
    //
    return EFI_UNSUPPORTED;
  }
  
  Status = gBS->LocateProtocol (
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &PciRootBridge
                );
  ASSERT_EFI_ERROR (Status);

  //
  // Program LT Device Memory Chipset Registers and record them in
  // BootScript so they will be saved and restored on S3
  //

  //
  // NoDMA Registers
  // We do not need NoDMA support, which is deprecated by VTd
  //

  //
  // SINIT Registers
  //
  Ptr64       = (UINT64 *)(UINTN)(LT_PUBLIC_SPACE_BASE_ADDRESS + LT_SINIT_BASE_REG_OFF);
  Value64     = (UINT64)LtDxeCtx->PlatformTxtDeviceMemory->SINITMemoryAddress;
  *Ptr64      = Value64;
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32, 
    (UINT64)(UINTN)(Ptr64), 
    2, 
    &Value64
    );

  Ptr64       = (UINT64 *)(UINTN)(LT_PUBLIC_SPACE_BASE_ADDRESS + LT_SINIT_SIZE_REG_OFF);
  Value64     = (UINT64)LtDxeCtx->PlatformTxtDeviceMemory->SINITMemorySize;
  *Ptr64      = Value64;
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32, 
    (UINT64)(UINTN)(Ptr64), 
    2, 
    &Value64
    );

  ZeroMem (
    (VOID *)(UINTN)LtDxeCtx->PlatformTxtDeviceMemory->SINITMemoryAddress, 
    (UINTN)LtDxeCtx->PlatformTxtDeviceMemory->SINITMemorySize
    );

  //
  // TXTHEAP Registers
  //
  Ptr64       = (UINT64 *)(UINTN)(LT_PUBLIC_SPACE_BASE_ADDRESS + LT_HEAP_BASE_REG_OFF);
  Value64     = (UINT64)LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress;
  *Ptr64      = Value64;
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32, 
    (UINT64)(UINTN)(Ptr64), 
    2, 
    &Value64
    );

  Ptr64       = (UINT64 *)(UINTN)(LT_PUBLIC_SPACE_BASE_ADDRESS + LT_HEAP_SIZE_REG_OFF);
  Value64     = (UINT64)LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemorySize;
  *Ptr64      = Value64;
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32, 
    (UINT64)(UINTN)(Ptr64), 
    2, 
    &Value64
    );

  ZeroMem (
    (VOID *)(UINTN)LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress, 
    (UINTN)LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemorySize
    );   
  
  //
  // Program DPR registers if not yet programmed
  //
  Status = gBS->LocateProtocol (
                  &gEfiIioSystemProtocolGuid,
                  NULL,
                  &IioSystemProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  IioGlobalData = IioSystemProtocol->IioGlobalData;
  // Read current CSR LTDPR_IIO_VTD_REG from socket0, IIO stack 0
  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  Value32 = CpuCsrAccess->ReadCpuCsr(0, 0, LTDPR_IIO_VTD_REG);

  if (LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionAddress != 0 && ((Value32 & 0x0FFF) == 0)) {
    //
    // Loop thru all IIO stacks of all  sockets that are present
    //
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if (!IioGlobalData->IioVar.IioVData.SocketPresent[Socket]) {
          continue;
        }
        for (Iio = 0; Iio < MAX_IIO_STACK; Iio++) {
            if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[Socket] & (1<<Iio))) {
                continue;
            }
            // Read current CSR LTDPR_IIO_VTD_REG
            Value32 = CpuCsrAccess->ReadCpuCsr(Socket, Iio, LTDPR_IIO_VTD_REG);
            if ((LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize == 0) || ((Value32 & 0x0FF5) != 0)) {
                continue;
            }
            // Program the IIO CSR LTDPR_IIO_VTD_REG
            Temp32 = ((UINT32)(LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize) >> 16) | SA_DPR_LOCK | SA_DPR_PRM;
            Value32 = Value32 & (~(0x0FF5)) | Temp32;
            CpuCsrAccess->WriteCpuCsr(Socket, Iio, LTDPR_IIO_VTD_REG, Value32);
            // 
            // Verify register value by reading it back, and assert error if value is not right
            //BUGBUG: need to double check DPR REG//ASSERT (((Value32 & 0xFF0) << 16) == LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize);
            //
            PciAddress = CpuCsrAccess->GetCpuCsrAddress(Socket, Iio, LTDPR_IIO_VTD_REG, &Size);
            DEBUG ((EFI_D_ERROR, "Socket%2d Stack%2d IIO LTDPR Register[%08x] written with %08x,  ", Socket, Iio, PciAddress, Value32));
            Value32 = *(UINT32*)PciAddress;
            DEBUG ((EFI_D_ERROR, "read back as %08x\n", Value32));
            if (((Value32 & 0xFF0) << 16) == LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize){
                S3BootScriptSaveMemWrite (
                    S3BootScriptWidthUint32,
                    (UINTN)PciAddress,
                    1,
                    &Value32
                );
            } else {
                DEBUG ((EFI_D_ERROR, "TxtDxeLib IIO LTDPR CSR programming error!!! \n"));
            }
        }
    }

    //
    // Program DPR register in LT Public space on PCH
    //
    Ptr64   = (UINT64 *) (UINTN) (LT_PUBLIC_SPACE_BASE_ADDRESS + LT_DMA_PROTECTED_RANGE_REG_OFF);
    Value64 = RShiftU64 (LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize, 16) | SA_DPR_LOCK  | SA_DPR_PRM ;
    Value64 |= ((LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionAddress + LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize) & 0xFFF00000);
    *Ptr64 = Value64;
    //
    // Assert error if programmed value is different from requested. This
    // means error is requested size.
    //
    Value64 = *Ptr64;
    DEBUG ((EFI_D_ERROR, "PCH TXT Public Space DPR reg (at FED3_0000 + 0x330) = %08x,\n", (UINT32) Value64));
    ASSERT ((LShiftU64 ((Value64 & 0xFFE), 16)) == LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize);

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
      (VOID *)(UINTN)LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionAddress, 
      (UINTN)LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionSize
    );
  }

  //
  // Fill BiosOsDataRegion
  //
  Ptr64     = (UINT64 *)(UINTN)(LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress + 8);
  
  if (LtDxeCtx->PlatformTxtPolicyData->BiosOsDataRegionRevision >= 2) {
    *(UINT64 *)(UINTN)LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress = sizeof(EFI_TXT_BIOS_OS_DATA_REGION_V3)+ 8 ;
    
    ((EFI_TXT_BIOS_OS_DATA_REGION_V3 *)Ptr64)->Revision = LtDxeCtx->PlatformTxtPolicyData->BiosOsDataRegionRevision;

    //
    // Allocate Physical Memory for LCP
    //
    ((EFI_TXT_BIOS_OS_DATA_REGION_V2 *)Ptr64)->LcpPolicyDataBase = 0;
    ((EFI_TXT_BIOS_OS_DATA_REGION_V2 *)Ptr64)->LcpPolicyDataSize = 0;

    if ((LtDxeCtx->PlatformTxtPolicyData->LcpPolicyDataBase != 0) &&
        (LtDxeCtx->PlatformTxtPolicyData->LcpPolicyDataSize != 0)) {
     //LCP PD must be copied into a DMA protected region, here it will be copied to DPR region
      Addr = LtDxeCtx->PlatformTxtDeviceMemory->DMAProtectionMemoryRegionAddress + (UINT32)LShiftU64 (PlatformInfoHob->MemData.BiosGuardMemSize, 20);
      ZeroMem (
        (VOID *)(UINTN)Addr, 
        (UINTN)LtDxeCtx->PlatformTxtPolicyData->LcpPolicyDataSize
        );
      CopyMem (
        (VOID *)(UINTN)Addr,
        (VOID *)(UINTN)LtDxeCtx->PlatformTxtPolicyData->LcpPolicyDataBase,
        (UINTN)LtDxeCtx->PlatformTxtPolicyData->LcpPolicyDataSize
        );
      ((EFI_TXT_BIOS_OS_DATA_REGION_V3 *)Ptr64)->LcpPolicyDataBase = Addr;
      ((EFI_TXT_BIOS_OS_DATA_REGION_V3 *)Ptr64)->LcpPolicyDataSize = LtDxeCtx->PlatformTxtPolicyData->LcpPolicyDataSize;
    }
    
    ((EFI_TXT_BIOS_OS_DATA_REGION_V3 *)Ptr64)->NumberOfLogicalProcessors = (UINT32)LtDxeCtx->ApCount + 1;

    if (LtDxeCtx->PlatformTxtPolicyData->BiosOsDataRegionRevision == 3){
      ((EFI_TXT_BIOS_OS_DATA_REGION_V3 *)Ptr64)->FlagVariable = LtDxeCtx->PlatformTxtPolicyData->FlagVariable;
    }

  } 
  
  //
  // Set it to zero because SINIT_ACM is not loaded here.
  //
  ((EFI_TXT_BIOS_OS_DATA_REGION_V1 *)Ptr64)->BiosSinitSize = 0;

  //Below debug should = 0x2c, when version = 3
  DEBUG ((EFI_D_ERROR, "  Lt - BiosDataSize = 0x%08x\n", *(UINT64 *)(UINTN)LtDxeCtx->PlatformTxtDeviceMemory->TXTHeapMemoryAddress));
  DEBUG ((EFI_D_ERROR, "  Lt - StartupAcmAddress = 0x%08x\n", (UINT8 *)(UINTN)(LtDxeCtx->PlatformTxtPolicyData->StartupAcmAddress)));
#if 1 //SINIT_IN_FLASH
    //
    // copy SINIT-ACM to LT.SINIT.MEMORY.BASE(LT offset 0x270)
    //

   // APTIOV_SERVER_OVERRIDE_RC_START
   // SinitAddress = GetSinitAddress();
   // Changes done to load SINIT binary from FV_MAIN, to avoid separate FD AREA
      Status = GetRawImage( &gAmiLtsxSinitGuid, &SinitAddress, &SinitSize );

      if ( EFI_ERROR( Status )) {
	  DEBUG((EFI_D_ERROR, "Failed to load SINIT ACM BINARY: Status=%r\n", Status));
          return Status;
      }
    // APTIOV_SERVER_OVERRIDE_RC_END
#ifdef PC_HOOK
#endif

      DEBUG ((EFI_D_ERROR, " Copy SINIT from the address in Flash:0x%08x\n",SinitAddress));
  SinitSize  = (UINT64)LtDxeCtx->PlatformTxtDeviceMemory->SINITMemorySize;
      DEBUG ((EFI_D_ERROR, " Copy SINIT from the address in Flash:0x%08x, Size:0x%x\n", SinitAddress,SinitSize));
    DEBUG ((EFI_D_ERROR, " to Txt Protected Memory address:0x%08x, Size:0x%x\n", LtDxeCtx->PlatformTxtDeviceMemory->SINITMemoryAddress,SinitSize));
      CopyMem (
        (VOID *)(UINTN)LtDxeCtx->PlatformTxtDeviceMemory->SINITMemoryAddress, 
        (VOID *)SinitAddress,
        (UINTN)SinitSize
        );
    // update the SINIT size in BiosOsData region (in the TXT HEAP)
    ((EFI_TXT_BIOS_OS_DATA_REGION_V3 *)Ptr64)->BiosSinitSize = (UINT32) SinitSize;
#endif

  return EFI_SUCCESS ;
}

//
// The 8-bit data at offset 0xF in the ACM binary that gives ACM Type information. 
// It can be one of three values:
// 0x80 = debug signed ACM
// 0x40 = NPW production signed ACM
// 0x00 = PW production signed ACM
//
// APTIOV_SERVER_OVERRIDE_RC_START : Moved later to get ACM type to get BiosAcmAddress from PlatformTxtPolicyData
EFI_STATUS
FindAcmBinaryType(
  IN      LT_DXE_LIB_CONTEXT      *LtDxeCtx
  )
{
#if 0
  EFI_SPI_PROTOCOL      *SpiProtocol;
  UINT8                 *MemSourcePtr;
  UINT32                ACMAddress;
  UINTN                 Size;
  EFI_STATUS            Status;
  UINT8                 AcmVersion;

  Status = gBS->LocateProtocol (
                  &gEfiSpiProtocolGuid,
                  NULL,
                  &SpiProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MemSourcePtr = NULL;
  Status = gBS->AllocatePool( EfiBootServicesData,
                              ACM_READ_SIZE,
                              &MemSourcePtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (MemSourcePtr, ACM_READ_SIZE);

  ACMAddress = (UINT32)GetSinitAddress() & ACM_BASE_ADDRESS_MASK;
  //
  // Read 8-bit data at offset 0xF in the ACM binary that gives  ACM Type information 
  //
  Size = ACM_READ_SIZE;

  Status = SpiProtocol->FlashRead (
                          SpiProtocol,
                          FlashRegionPlatformData,
                          0,
                          (UINT32) Size,
                          (UINT8 *)MemSourcePtr
                          );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error during PDR read\n"));
  }
#endif

  UINT8                 AcmVersion;
  UINT8                 *MemSourcePtr;
  MemSourcePtr = (UINT8*)LtDxeCtx->PlatformTxtPolicyData->BiosAcmAddress; //(UINT8*)FV_ACM_BASE;
// APTIOV_SERVER_OVERRIDE_RC_END : Moved later to get ACM type to get BiosAcmAddress from PlatformTxtPolicyData
  AcmVersion = *(MemSourcePtr + ACM_TYPE_OFFSET);
  SetAcmType(AcmVersion);
// APTIOV_SERVER_OVERRIDE_RC_START
//  gBS->FreePool (MemSourcePtr);
// APTIOV_SERVER_OVERRIDE_RC_END
 
  return EFI_SUCCESS;
}
