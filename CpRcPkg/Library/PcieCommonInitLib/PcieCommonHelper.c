//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module:

**/

#include "PcieCommonHelper.h"

VOID
ResetPcieStack (
  PCIE_LIB_STACK  *PcieStack
)
{
  ASSERT (PcieStack != NULL);
  if(PcieStack == NULL){
    DEBUG ((EFI_D_ERROR, "\nERROR: ResetPcieStack () - EFI_INVALID_PARAMETER.\n"));
    return;
  }

  PcieStack->Top = 0;
}

VOID
PushPcieStack (
  PCIE_LIB_STACK            *PcieStack,
  IN USRA_PCIE_ADDR_TYPE    *PcieAddress
)
{
  ASSERT (PcieStack != NULL && PcieStack->Top != FixedPcdGet32(PcdMaxNestedLevel));
  if(PcieStack == NULL){
    DEBUG ((EFI_D_ERROR, "\nERROR: PushPcieStack () - EFI_INVALID_PARAMETER.\n"));
    return;
  }

  PcieStack->PcieCapInfo[PcieStack->Top++].CapPcieAddress = *PcieAddress;
}

VOID
PopPcieStack (
  PCIE_LIB_STACK  *PcieStack
)
{
  ASSERT (PcieStack != NULL);
  if(PcieStack == NULL){
    DEBUG ((EFI_D_ERROR, "\nERROR: PopPcieStack () - EFI_INVALID_PARAMETER.\n"));
    return;
  }

  if (PcieStack->Top > 0) {
    PcieStack->Top--;
  }
}

UINT8
PcieFindCapId (
  USRA_ADDRESS            *PcieAddress,
  UINT8                   CapId
  )
/*++
  
Routine Description:
  
  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability
    
Arguments:
  
  Bus                     Pci Bus Number
  Device                  Pci Device Number
  Function                Pci Function Number
  CapId                   CAPID to search for

Returns:
  
  0                       CAPID not found
  Other                   CAPID found, Offset of desired CAPID

--*/
{
  UINT8                 CapHeaderOffset;
  UINT8                 CapHeaderId = 0;
  UINT8                 Data8;
  USRA_ADDRESS TempPcieAddress;

  USRA_ADDRESS_COPY(&TempPcieAddress, PcieAddress);
  //
  // Read Reg 06 Status Register
  //
  TempPcieAddress.Attribute.AccessWidth = UsraWidth8;
  TempPcieAddress.Pcie.Offset = PCI_PRIMARY_STATUS_OFFSET;
  RegisterRead(&TempPcieAddress, &Data8);

  //
  // Check capabilities list enable
  //
  if ((Data8 & BIT4) == 0) {
    return 0;
  }

  //
  // Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
  //
  TempPcieAddress.Pcie.Offset = PCI_CAPBILITY_POINTER_OFFSET;
  RegisterRead(&TempPcieAddress, &CapHeaderOffset);

  //
  // Keep looking for a pointer ID that match the flag
  // Read the Capabilities Pointer
  //
  while (CapHeaderOffset != 0 && CapHeaderId != 0xFF) {
    TempPcieAddress.Pcie.Offset = CapHeaderOffset;
    RegisterRead(&TempPcieAddress, &CapHeaderId);

    if (CapHeaderId == CapId) {
      return CapHeaderOffset;
    }
    TempPcieAddress.Pcie.Offset = CapHeaderOffset + 1;
    RegisterRead(&TempPcieAddress, &CapHeaderOffset);
  }

  return CapHeaderOffset;
}

UINT16
PcieFindExtendedCapId (
  USRA_ADDRESS              *PcieAddress,
  UINT16                    CapId
  )
/*++
  
Routine Description:
  
  Search and return the offset of desired Pci Express Capability ID
  CAPID list:
    0x0001 = Advanced Error Reporting Capability
    0x0002 = Virtual Channel Capability
    0x0003 = Device Serial Number Capability
    0x0004 = Power Budgeting Capability
    
Arguments:
  
  Bus                     Pci Bus Number
  Device                  Pci Device Number
  Function                Pci Function Number
  CapId                   Extended CAPID to search for

Returns:
  
  0                       CAPID not found
  Other                   CAPID found, Offset of desired CAPID

--*/
{
  UINT16                CapHeaderOffset;
  UINT16                CapHeaderId;
  USRA_ADDRESS TempPcieAddress;

  USRA_ADDRESS_COPY(&TempPcieAddress, PcieAddress);
  //
  // Start to search at Offset 0x100
  // Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
  //
  CapHeaderId     = 0;
  CapHeaderOffset = 0x100;
  TempPcieAddress.Attribute.AccessWidth = UsraWidth16;
  while (CapHeaderOffset != 0 && CapHeaderId != 0xFFFF)
  {
    TempPcieAddress.Pcie.Offset = CapHeaderOffset;
    RegisterRead(&TempPcieAddress, &CapHeaderId);
    if (CapHeaderId == CapId) {
      return CapHeaderOffset;
    }
    //
    // Each capability must be DWORD aligned. 
    // The bottom two bits of all pointers are reserved and must be implemented as 00b 
    // although software must mask them to allow for future uses of these bits.
    //
    TempPcieAddress.Pcie.Offset = CapHeaderOffset + 2;
    RegisterRead(&TempPcieAddress, &CapHeaderOffset);
    CapHeaderOffset = (CapHeaderOffset>>4) & ((UINT16) ~(BIT0 | BIT1));
  }

  return 0;
}

BOOLEAN
IsPciDevicePresent (
  USRA_ADDRESS                        *PcieAddress,
  PCI_TYPE00                          *Pci
  )
{
  USRA_ADDRESS   TempPcieAddress;

  USRA_ADDRESS_COPY(&TempPcieAddress, PcieAddress);

// APTIOV_SERVER_OVERRIDE_RC_START : Resolve issue with PCI scan
  TempPcieAddress.Attribute.AccessWidth = UsraWidth32;
  TempPcieAddress.Attribute.AddrType     = AddrTypePCIEBLK;
  TempPcieAddress.PcieBlk.Count  = sizeof (PCI_TYPE01)/ sizeof(UINT32);
  TempPcieAddress.Pcie.Offset = 0;
//  PciReadBuffer((TempPcieAddress.Attribute.RawData32[0] & 0x0fffffff), sizeof (PCI_TYPE00), Pci);
  RegisterRead(&TempPcieAddress, Pci);
// APTIOV_SERVER_OVERRIDE_RC_END : Resolve issue with PCI scan

  if ( (Pci->Hdr).VendorId != 0xffff) 
  {
    return TRUE;
  }
  return FALSE;
}

VOID PcieProgramOpTable(
   PCI_LIB_OP_STRUCT  *OpTable,
   UINT32             OpTableSize
   )
{
  UINT32                    i;
  USRA_ADDRESS     Address;

  USRA_ZERO_ADDRESS_TYPE(Address, AddrTypePCIE);
  if (FeaturePcdGet (PcdUsraSupportS3)) Address.Attribute.S3Enable = USRA_ENABLE;

  for (i = 0; i < OpTableSize/sizeof(PCI_LIB_OP_STRUCT); i++)
  {
    Address.Pcie = OpTable[i].PcieAddr;
    RegisterModify(&Address, &OpTable[i].AndMask, &OpTable[i].OrMask);
  }  
}



