/** @file
  PCH Smbus Driver

@copyright
 Copyright (c) 2004 - 2015 Intel Corporation. All rights
 reserved This software and associated documentation (if any) is
 furnished under a license and may only be used or copied in
 accordance with the terms of the license. Except as permitted
 by the license, no part of this software or documentation may
 be reproduced, stored in a retrieval system, or transmitted in
 any form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include "PchSmbus.h"

SMBUS_INSTANCE  *mSmbusContext;
extern EFI_GUID gEfiSmbusArpMapGuid;

/**
  Execute an SMBUS operation

  @param[in] This                 The protocol instance
  @param[in] SlaveAddress         The address of the SMBUS slave device
  @param[in] Command              The SMBUS command
  @param[in] Operation            Which SMBus protocol will be issued
  @param[in] PecCheck             If Packet Error Code Checking is to be used
  @param[in, out] Length          Length of data
  @param[in, out] Buffer          Data buffer

  @retval EFI_SUCCESS             The SMBUS operation is successful
  @retval Other Values            Something error occurred
**/
EFI_STATUS
EFIAPI
SmbusExecute (
  IN      CONST EFI_SMBUS_HC_PROTOCOL   *This, 
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND      Command,
  IN      EFI_SMBUS_OPERATION           Operation,
  IN      BOOLEAN                       PecCheck,
  IN OUT  UINTN                         *Length,
  IN OUT  VOID                          *Buffer
  )
{
  DEBUG ((DEBUG_INFO, "SmbusExecute() Start, SmbusDeviceAddress=%x, Command=%x, Operation=%x\n", (SlaveAddress.SmbusDeviceAddress << 1), Command, Operation));
  InitializeSmbusRegisters ();

  return SmbusExec (
          SlaveAddress,
          Command,
          Operation,
          PecCheck,
          Length,
          Buffer
          );
}

/**
  <b>SMBus DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The SMBus module is a DXE driver that provides a standard way to execute an SMBus command.

  - @pre
    - @link _PCH_POLICY PCH_POLICY_HOB @endlink
      - This module uses the parameters in SmbusConfig for platform reserved (non ARP capable) addresses.

  - @result
    The SMBus driver produces EFI_SMBUS_HC_PROTOCOL which is documented in the SMBus Host Controller 
    Protocol Specification.

  @param[in] ImageHandle          ImageHandle of this module
  @param[in] SystemTable          EFI System Table

  @retval EFI_SUCCESS             Driver initializes successfully
  @retval Other values            Some error occurred
**/
EFI_STATUS
EFIAPI
InitializePchSmbus (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  )
{
  PCH_POLICY_HOB           *PchPolicyHob;
  EFI_STATUS                        Status;
  UINTN                             DataSize;
  VOID                              *Data;
  EFI_PEI_HOB_POINTERS              HobList;

  DEBUG ((DEBUG_INFO, "InitializePchSmbus() Start\n"));

  HobList.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobList.Guid != NULL);
  if (HobList.Guid == NULL) {
      return EFI_NOT_FOUND;
  }
  PchPolicyHob  = GET_GUID_HOB_DATA(HobList.Guid); 


  mSmbusContext = AllocateZeroPool (sizeof (SMBUS_INSTANCE));
  if (mSmbusContext == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  mSmbusContext->Signature                  = PCH_SMBUS_PRIVATE_DATA_SIGNATURE;
  mSmbusContext->IoDone                     = IoDone;
  mSmbusContext->SmbusIoRead                = SmbusIoRead;
  mSmbusContext->SmbusIoWrite               = SmbusIoWrite;
  mSmbusContext->SmbusController.Execute    = SmbusExecute;
  mSmbusContext->SmbusController.ArpDevice  = SmbusArpDevice;
  mSmbusContext->SmbusController.GetArpMap  = SmbusGetArpMap;
  mSmbusContext->SmbusController.Notify     = SmbusNotify;
  mSmbusContext->PlatformNumRsvd            = PchPolicyHob->SmbusConfig.NumRsvdSmbusAddresses;
  mSmbusContext->PlatformRsvdAddr           = PchPolicyHob->SmbusConfig.RsvdSmbusAddressTable;
  ASSERT (mSmbusContext->PlatformNumRsvd <= PCH_MAX_SMBUS_RESERVED_ADDRESS);

  ///
  /// See if PEI already ARPed any devices, and if so, update our device map.
  ///
  /// Get Hob list
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  ASSERT_EFI_ERROR (Status);

  HobList.Raw = GetNextGuidHob (&gEfiSmbusArpMapGuid, HobList.Raw);
  ///
  /// If we found the right hob, store the information. Otherwise, continue.
  ///
  if (HobList.Raw != NULL) {
    Data      = (VOID *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
    DataSize  = HobList.Header->HobLength - sizeof (EFI_HOB_GUID_TYPE);
    CopyMem (mSmbusContext->DeviceMap, Data, DataSize);
    mSmbusContext->DeviceMapEntries = (UINT8) (DataSize / sizeof (EFI_SMBUS_DEVICE_MAP));
  }
  ///
  /// Initialize the NotifyFunctionList
  ///
  InitializeListHead (&mSmbusContext->NotifyFunctionList);

  ///
  /// Install the SMBUS interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmbusContext->Handle,
                  &gEfiSmbusHcProtocolGuid,
                  &mSmbusContext->SmbusController,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "InitializePchSmbus() End\n"));

  return EFI_SUCCESS;
}
