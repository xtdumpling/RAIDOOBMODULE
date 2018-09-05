/**

  Copyright (c)  2014 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  @file PtuLoader.c

  SPS Node Manager may require Power-Thermal Utility application to be started
  in host partition to measure the system power consumption. This application
  is provided as PCI Option ROM for MROM device in PCH. But not all PCH
  implement MROM. On systems where MROM is not implemented PTU is invoked
  by this module.

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/Spi.h>
#include <IndustryStandard/Pci22.h>

#include <MeAccess.h>
#include <MeHeci3.h>
#include <Sps.h>
#include <Protocol/MeSpsPolicyProtocol.h>


/**
 * PTU Loader entry point.
 *
 * param[in] ImageHandle    Handle of driver image
 * param[in] pSysTable      Pointer to the system table
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
PtuLoaderEntryPoint (
  IN     EFI_HANDLE         ImageHandle,
  IN     EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS                                  Status;
  UINT32                                      ImageSize;
  EFI_SPI_PROTOCOL                           *pSpiProtocol;
  EFI_PCI_EXPANSION_ROM_HEADER                PtuRomHeader;
  MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH     EfiOpRomImageNode;
  EFI_DEVICE_PATH_PROTOCOL                   *pPciOptionRomImageDevicePath;
  PCI_DATA_STRUCTURE                          Pcir;
  VOID                                       *pPtuImage = NULL;
  UINT16                                      ImageOffset;
  UINT32                                      InitializationSize;
  SPS_MEFS1                                   Mefs1;
  SPS_NMFS                                    Nmfs;
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
  SPS_POLICY_PROTOCOL                        *pSpsPolicy = NULL;
  
  gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &pSpsPolicy);
  if (pSpsPolicy != NULL && pSpsPolicy->SpsConfig.NmPtuLoadOverride)
  {
    DEBUG((EFI_D_INFO, "[SPS] PTU execution forced in configuration\n"));
  }
  else
#endif
  {
    Mefs1.UInt32 = SpsHeciPciReadMefs1();
    Nmfs.UInt32 = SpsHeciPciReadNmfs();
    if (Mefs1.UInt32 == 0xFFFFFFFF || Mefs1.Bits.OperatingMode != MEFS1_OPMODE_SPS ||
        Nmfs.UInt32 == 0xFFFFFFFF || !Nmfs.Bits.NmEnabled || !Nmfs.Bits.DirectPtuExecution)
    {
      DEBUG((EFI_D_INFO, "[SPS] PTU execution not required (MEFS1: 0x%08X, NMFS: 0x%08X)\n",
                         Mefs1.UInt32, Nmfs.UInt32));
      return EFI_SUCCESS;
    }
  }
  Status = gBS->LocateProtocol(&gEfiSpiProtocolGuid, NULL, (VOID**)&pSpiProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: LocateProtocol gPchSpiProtocolGuid failed (%r)\n", Status));
    return Status;
  }
  
  Status = pSpiProtocol->FlashRead(pSpiProtocol, FlashRegionDer, 0,
                                   sizeof(EFI_PCI_EXPANSION_ROM_HEADER), (UINT8*)&PtuRomHeader);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Read PCI expansion ROM header from SPI flash failed (%r)\n", Status));
    return Status;
  }
  
  if (PtuRomHeader.Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE)
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: PCI expansion ROM signature not found\n"));
    return EFI_NOT_FOUND;
  }
  
  Status = pSpiProtocol->FlashRead(pSpiProtocol, FlashRegionDer, PtuRomHeader.PcirOffset,
                                   sizeof(PCI_DATA_STRUCTURE), (UINT8 *) &Pcir);
  
  if (EFI_ERROR(Status) || Pcir.Signature != PCI_DATA_STRUCTURE_SIGNATURE)
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: PCI data signature not found in DER region\n"));
    return EFI_NOT_FOUND;
  }
  
  EfiOpRomImageNode.Header.Type     = MEDIA_DEVICE_PATH;
  EfiOpRomImageNode.Header.SubType  = MEDIA_RELATIVE_OFFSET_RANGE_DP;
  SetDevicePathNodeLength (&EfiOpRomImageNode.Header, sizeof (EfiOpRomImageNode));
  
  EfiOpRomImageNode.StartingOffset  = 0x0;
  EfiOpRomImageNode.EndingOffset    = (UINT32)(Pcir.ImageLength * 512) - 1;
  
  pPciOptionRomImageDevicePath = AppendDevicePathNode(NULL, NULL);
  pPciOptionRomImageDevicePath = AppendDevicePathNode(pPciOptionRomImageDevicePath, &EfiOpRomImageNode.Header);
  if (pPciOptionRomImageDevicePath == NULL)
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Unable to create PCI OpROM image device path\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  
  ImageOffset        = PtuRomHeader.EfiImageHeaderOffset;
  InitializationSize = PtuRomHeader.InitializationSize * 512;
  ImageSize          = InitializationSize - ImageOffset;
  
  Status = gBS->AllocatePool(EfiBootServicesData, ImageSize, &pPtuImage);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: AllocatePool for PTU image failed (%r)\n", Status));
    goto ErrExit0;
  }
  
  Status = pSpiProtocol->FlashRead(pSpiProtocol, FlashRegionDer, ImageOffset, ImageSize, (UINT8*)pPtuImage);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Read PTU image from SPI filed (%r)\n", Status));
    goto ErrExit1;
  }
  
  ImageHandle = NULL;
  Status = gBS->LoadImage(FALSE, gImageHandle, pPciOptionRomImageDevicePath, pPtuImage,
                          ImageSize, &ImageHandle);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: PTU image load failed (%r)\n", Status));
    goto ErrExit1;
  }
  
  Status = gBS->StartImage (ImageHandle, NULL, NULL);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: PTU image start failed (%r)\n", Status));
    goto ErrExit1;
  }
  
  return EFI_SUCCESS;
  
 ErrExit1:
  gBS->FreePool(pPtuImage);
 ErrExit0:
  gBS->FreePool(pPciOptionRomImageDevicePath);
  return Status;
} // PtuLoaderEntryPoint()

