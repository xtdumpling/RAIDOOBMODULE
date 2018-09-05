/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/**

Copyright (c)  2004 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PciPlatform.c
    
    Registers onboard PCI ROMs with PCI.IO.
    
Revision History:

**/

#include <PiDxe.h>
#include "PciPlatform.h"
#include <Library/PcdLib.h>
#include <Library/PlatformOpromPolicyLib.h>
#ifdef EFI_PCI_IOV_SUPPORT
#include "PciIovPlatformPolicy.h"
#endif

PCI_PLATFORM_PRIVATE_DATA mPciPrivateData;

BOOLEAN                   FirstCall = TRUE;
UINT8                     sSataRaidLoadEfiDriverOption;
UINT8                     SataRaidLoadEfiDriverOption;
UINT8                     BootNetworkOption;

/**

  Set the PciPolicy as EFI_RESERVE_ISA_IO_NO_ALIAS | EFI_RESERVE_VGA_IO_NO_ALIAS.

  @param This       -  The pointer to the Protocol itself.
  PciPolicy  -  the returned Policy.

  @retval EFI_UNSUPPORTED        -  Function not supported.
  @retval EFI_INVALID_PARAMETER  -  Invalid PciPolicy value.

**/
EFI_STATUS
EFIAPI
GetPlatformPolicy (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL         *This,
  OUT EFI_PCI_PLATFORM_POLICY           *PciPolicy
  )
{
  if (PciPolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_UNSUPPORTED;
}
// APTIOV_SERVER_OVERRIDE_RC_START
#if 0
/**

  Get an indicated image in raw sections.

  @param NameGuid  -  NameGuid of the image to get.
  @param Buffer    -  Buffer to store the image get.
  @param Size      -  size of the image get.

  @retval EFI_NOT_FOUND   -  Could not find the image.
  @retval EFI_LOAD_ERROR  -  Error occurred during image loading.
  @retval EFI_SUCCESS     -  Image has been successfully loaded.

**/
EFI_STATUS
GetRawImage(
   IN     EFI_GUID                       *NameGuid, 
   IN OUT VOID                           **Buffer, 
   IN OUT UINTN                          *Size
   ) { 
  EFI_STATUS                    Status; 
  EFI_HANDLE                    *HandleBuffer; 
  UINTN                         HandleCount; 
  UINTN                         Index; 
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv; 
  UINT32                        AuthenticationStatus; 
  
  Status = gBS->LocateHandleBuffer(
     ByProtocol, 
     &gEfiFirmwareVolume2ProtocolGuid, 
     NULL, 
     &HandleCount, 
     &HandleBuffer
     ); 
  if (EFI_ERROR(Status) || HandleCount == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol(
       HandleBuffer[Index], 
       &gEfiFirmwareVolume2ProtocolGuid, 
       &Fv
       ); 
    if (EFI_ERROR(Status)) {
      return EFI_LOAD_ERROR;
    }
    //
    // Try a raw file
    //
    *Buffer = NULL; 
    *Size   = 0; 
    Status = Fv->ReadSection(
         Fv, 
         NameGuid, 
         EFI_SECTION_RAW, 
         0, 
         Buffer, 
         Size, 
         &AuthenticationStatus
         );
    if (!EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "Read the OROM successfully!\n")); 
      break;
    }
  }

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
#endif
// APTIOV_SERVER_OVERRIDE_RC_END

/**

  Return a PCI ROM image for the onboard device represented by PciHandle.

  @param This       -  Protocol instance pointer.
  PciHandle  -  PCI device to return the ROM image for.
  RomImage   -  PCI Rom Image for onboard device.
  RomSize    -  Size of RomImage in bytes.

  @retval EFI_SUCCESS    -  RomImage is valid.
  @retval EFI_NOT_FOUND  -  No RomImage.

**/
EFI_STATUS
EFIAPI
GetPciRom (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL   *This,
  IN  EFI_HANDLE                        PciHandle,
  OUT VOID                              **RomImage,
  OUT UINTN                             *RomSize
  )
{
// APTIOV_SERVER_OVERRIDE_RC_START : Remove OpROM processing code
#if 0
  EFI_STATUS                        Status;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  UINTN                             Segment;
  UINTN                             Bus;
  UINTN                             Device;
  UINTN                             Function;
  UINT16                            VendorId;
  UINT16                            DeviceId;
  UINT16                            DeviceClass;
  UINTN                             TableIndex;
  UINTN                             RomImageNumber;
  VOID                              *OpRomBase;
  UINTN                             OpRomSize;
  EFI_PCI_ROM_HEADER                RomHeader;
  PCI_DATA_STRUCTURE                *Pcir;
  OPROM_LOAD_POLICY                 OpromPolicy;
  BOOLEAN                           SlotOptionRomDisabled;
  SYSTEM_BOARD_PROTOCOL             *pSystemBoard;      

  OpRomBase = NULL;
  OpRomSize = 0;
  Status = gBS->LocateProtocol (&gEfiDxeSystemBoardProtocolGuid, NULL, (VOID**)&pSystemBoard); 
  ASSERT_EFI_ERROR (Status);                                                                   

  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (FirstCall == TRUE) {
    Status = GetOptionData (&gEfiSetupVariableGuid, OFFSET_OF(SYSTEM_CONFIGURATION, BootNetwork), &BootNetworkOption, sizeof(BootNetworkOption));
    if (EFI_ERROR (Status))
      BootNetworkOption = 0;
    Status = GetOptionData (&gEfiPchRcVariableGuid, OFFSET_OF(PCH_RC_CONFIGURATION, SataRaidLoadEfiDriver), &SataRaidLoadEfiDriverOption, sizeof(SataRaidLoadEfiDriverOption)); 
    if (EFI_ERROR (Status))
      SataRaidLoadEfiDriverOption = 0;
    Status = GetOptionData (&gEfiPchRcVariableGuid, OFFSET_OF(PCH_RC_CONFIGURATION, sSataRaidLoadEfiDriver), &sSataRaidLoadEfiDriverOption, sizeof(sSataRaidLoadEfiDriverOption));
    if (EFI_ERROR (Status))
      sSataRaidLoadEfiDriverOption = 0;

    FirstCall = FALSE;
  }

  PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_CLASSCODE_OFFSET + 1, 1, &DeviceClass);

  //
  // Run PXE ROM only if Boot network is enabled
  //
  if ((BootNetworkOption == 0) &&
      (DeviceClass == ((PCI_CLASS_NETWORK << 8) | PCI_CLASS_NETWORK_ETHERNET))
      ) {
    return EFI_NOT_FOUND;
  }

  //
  // Run each PCI-E slot ROM only if PCI-E Slot Oprom is enabled.
  //  
  if ( Bus != 0 ) {
    SlotOptionRomDisabled = pSystemBoard->SystemPcieRootPortCheck(Bus, PcdGet32(PcdOemSkuPcieSlotOpromBitMap));

    if (SlotOptionRomDisabled) {
      return EFI_NOT_FOUND;
    }
  }

  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &VendorId);
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_DEVICE_ID_OFFSET, 1, &DeviceId);

  //DEBUG ((EFI_D_INFO, "GetPciRom - VenID:DevID: %04x:%04x\n", (UINTN)VendorId, (UINTN)DeviceId));

  //
  // Fix MS-HD5770 video adapter can not work:
  // This device is not a OPROM 3.0 and does not have device id list as well.
  // It only have 1 device id in OPROM.
  // Device Id in OpROM is not same with the value in PCI configuration space
  // it will cause VBIOS fails to start
  //
  if ((VendorId == 0x1002) && (DeviceId == 0x68BE)) {
    DEBUG ((EFI_D_ERROR, "MS-HD5770 video adapter\n"));
    RomHeader.Raw = PciIo->RomImage;
    if (RomHeader.Raw != NULL) {
      Pcir = (PCI_DATA_STRUCTURE *)(RomHeader.Raw + RomHeader.Generic->PcirOffset);
      if ((Pcir->VendorId == 0x1002) && (Pcir->DeviceId == 0x68B8)) {
        //
        // Assign same device id in PCI configuration space
        //
        Pcir->DeviceId = DeviceId;
      }
    } else {
      DEBUG ((EFI_D_ERROR, "MS-HD5770 video adapter detected but PciIo->RomImage == NULL!\n"));
    }
  }

  //Check if user disables the option rom loading for this device.
  if (!PlatformOpromLoadDevicePolicy(PciIo)) {
    return EFI_NOT_FOUND;
  }

   // If setup value requested EFI, we don't load the RAID OROM. 
  if ((VendorId == V_PCH_SATA_VENDOR_ID) && 
      ((IS_PCH_LBG_SATA_RAID_DEVICE_ID(DeviceId)) && 
       (SataRaidLoadEfiDriverOption == 1 )) ||
      ((IS_PCH_LBG_SSATA_RAID_DEVICE_ID(DeviceId)) && 
       (sSataRaidLoadEfiDriverOption == 1 ))
      ) 
    { 
      return EFI_NOT_FOUND;
    }
  //
  // Loop through table of video option rom descriptions
  //
  RomImageNumber = 0;
  for (TableIndex = 0; pSystemBoard->SystemBoardInfo->PciOptionRomTable[TableIndex].VendorId != 0xffff; TableIndex++) {
    //
    // See if the PCI device specified by PciHandle matches at device in mPciOptionRomTable
    //
    if (VendorId != pSystemBoard->SystemBoardInfo->PciOptionRomTable[TableIndex].VendorId ||
        DeviceId != pSystemBoard->SystemBoardInfo->PciOptionRomTable[TableIndex].DeviceId ||
        Device != pSystemBoard->SystemBoardInfo->PciOptionRomTable[TableIndex].Device ||
        Function != pSystemBoard->SystemBoardInfo->PciOptionRomTable[TableIndex].Function
        ) {
      continue;
    }

    //Check if user wants to exclusively run this option rom for the device.    
    OpromPolicy = PlatformOpromLoadTypePolicy(PciHandle, TableIndex);
    if(OpromPolicy == DONT_LOAD) {
      continue;
    }

    Status = GetRawImage (
              &pSystemBoard->SystemBoardInfo->PciOptionRomTable[TableIndex].FileName,
              &OpRomBase,
              &OpRomSize
              );

    if (EFI_ERROR (Status)) {
      continue;
    } else {
      RomImageNumber++;
      if (RomImageNumber == PcdGet8(PcdMaxOptionRomNumber) || OpromPolicy == EXCLUSIVE_LOAD) {
        break;
      }
    }
  }

  if (RomImageNumber == 0) {

    return EFI_NOT_FOUND;

  } else {

    *RomImage = OpRomBase;
    *RomSize  = OpRomSize;

    return EFI_SUCCESS;
  }
#endif
  return EFI_NOT_FOUND;   // dummy return
// APTIOV_SERVER_OVERRIDE_RC_END : Remove OpROM processing code
}

/**

  GC_TODO: Add function description

  @param This      - GC_TODO: add argument description
  @param Function  - GC_TODO: add argument description
  @param Phase     - GC_TODO: add argument description

  @retval EFI_INVALID_PARAMETER - GC_TODO: Add description for return value
  @retval EFI_INVALID_PARAMETER - GC_TODO: Add description for return value
  @retval EFI_UNSUPPORTED - GC_TODO: Add description for return value
  @retval EFI_SUCCESS - GC_TODO: Add description for return value

**/
EFI_STATUS
EFIAPI
RegisterPciCallback (
  IN  EFI_PCI_CALLBACK_PROTOCOL               *This,
  IN  EFI_PCI_CALLBACK_FUNC                   Function,
  IN  EFI_PCI_ENUMERATION_PHASE               Phase
  )
{
  LIST_ENTRY     *NodeEntry;
  PCI_CALLBACK_DATA *PciCallbackData;

  if (Function == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ( (Phase &  (EfiPciEnumerationDeviceScanning | EfiPciEnumerationBusNumberAssigned \
              | EfiPciEnumerationResourceAssigned)) == 0) {
    return EFI_INVALID_PARAMETER; 
  }
  //
  // Check if the node has been added
  //
  NodeEntry = GetFirstNode (&mPciPrivateData.PciCallbackList);
  while (!IsNull (&mPciPrivateData.PciCallbackList, NodeEntry)) {
    PciCallbackData = PCI_CALLBACK_DATA_FROM_LINK (NodeEntry);
    if (PciCallbackData->Function == Function) {
      return EFI_UNSUPPORTED;
    }

    NodeEntry = GetNextNode (&mPciPrivateData.PciCallbackList, NodeEntry);
  }

  PciCallbackData = NULL;
  PciCallbackData = AllocateZeroPool (sizeof (PCI_CALLBACK_DATA));
  ASSERT (PciCallbackData != NULL);

  if(PciCallbackData != NULL){
    PciCallbackData->Signature  = PCI_CALLBACK_DATA_SIGNATURE;
    PciCallbackData->Function   = Function;
    PciCallbackData->Phase      = Phase;
    InsertTailList (&mPciPrivateData.PciCallbackList, &PciCallbackData->Link);
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}


/**

  Main Entry point of the Pci Platform Driver.
  
  @param ImageHandle  -  Handle to the image.
  @param SystemTable  -  Handle to System Table.
  
  @retval EFI_STATUS  -  Status of the function calling.

**/
EFI_STATUS
EFIAPI
PciPlatformDriverEntry (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HOB_GUID_TYPE             *GuidHob;


  GuidHob    = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  mPciPrivateData.PlatformInfo = GET_GUID_HOB_DATA(GuidHob); 

  //EDK2_TODO Check if clearing mPciPrivateData.PlatformInfo (got above) is intended.
  ZeroMem (&mPciPrivateData, sizeof (mPciPrivateData));
  InitializeListHead (&mPciPrivateData.PciCallbackList);

  mPciPrivateData.PciPlatform.PlatformNotify          = PhaseNotify; 
  mPciPrivateData.PciPlatform.PlatformPrepController  = PlatformPrepController;
  mPciPrivateData.PciPlatform.GetPlatformPolicy       = GetPlatformPolicy;
  mPciPrivateData.PciPlatform.GetPciRom               = GetPciRom;
  mPciPrivateData.PciCallback.RegisterPciCallback     = RegisterPciCallback;
#ifdef EFI_PCI_IOV_SUPPORT
  mPciPrivateData.PciIovPlatform.GetSystemLowestPageSize = GetSystemLowestPageSize;
  mPciPrivateData.PciIovPlatform.GetPlatformPolicy       = GetIovPlatformPolicy;
#endif

  //
  // Install on a new handle					
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPciPrivateData.PciPlatformHandle,
                  &gEfiPciPlatformProtocolGuid,
                  &mPciPrivateData.PciPlatform,
                  &gEfiPciCallbackProtocolGuid,
                  &mPciPrivateData.PciCallback,
#ifdef EFI_PCI_IOV_SUPPORT
                  &gEfiPciIovPlatformProtocolGuid,
                  &mPciPrivateData.PciIovPlatform,
#endif
                  NULL
                  );

  return Status;
}
