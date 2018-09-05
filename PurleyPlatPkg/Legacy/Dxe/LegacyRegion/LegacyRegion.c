/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file LegacyRegion.c

  This file contains functions needed for manipulation of
  PAM registers that control access to C0000-FFFFF range.
  The PAM registers are assumed to reside in the PCI config
  space of the device 0, function 0, bus 0.
  Given that, the only code needed to be modified here for
  purposes of porting to a new platform/chipset are the values
  in the staic initialization of the mPamData[] variable.

**/
#include "LegacyRegion.h"

#define PAM0HI 0
#define PAM1LO 1
#define PAM1HI 2
#define PAM2LO 3
#define PAM2HI 4
#define PAM3LO 5
#define PAM3HI 6
#define PAM4LO 7
#define PAM4HI 8
#define PAM5LO 9
#define PAM5HI 10
#define PAM6LO 11
#define PAM6HI 12

//
// Module Global:
//  Since this driver will only ever produce one instance of
//  the Private Data protocol you are not required to dynamically
//  allocate the PrivateData.
//
LEGACY_REGION_INSTANCE  mPrivateData;


//
// Read\Write CPU CSR protocol
// 
EFI_CPU_CSR_ACCESS_PROTOCOL     *mCpuCsrAccess;
EFI_IIO_UDS_PROTOCOL            *mIioUds;


STATIC PAM_DATA         mPamData[] = {
  {
    0xF0000,
    0x10000,
    PAM0HI
  },
  {
    0xC0000,
    0x04000,
    PAM1LO
  },
  {
    0xC4000,
    0x04000,
    PAM1HI
  },
  {
    0xC8000,
    0x04000,
    PAM2LO
  },
  {
    0xCC000,
    0x04000,
    PAM2HI
  },
  {
    0xD0000,
    0x04000,
    PAM3LO
  },
  {
    0xD4000,
    0x04000,
    PAM3HI
  },
  {
    0xD8000,
    0x04000,
    PAM4LO
  },
  {
    0xDC000,
    0x04000,
    PAM4HI
  },
  {
    0xE0000,
    0x04000,
    PAM5LO
  },
  {
    0xE4000,
    0x04000,
    PAM5HI
  },
  {
    0xE8000,
    0x04000,
    PAM6LO
  },
  {
    0xEC000,
    0x04000,
    PAM6HI
  }
};

/**

    Get Legacy Region Information

    @param This            - This protocol
    @param DescriptorCount - Descriptor type
    @param Descriptor      - Legacy region descriptor

    @retval EFI_UNSUPPORTED 

**/
EFI_STATUS
EFIAPI
LegacyRegionGetInfo (
  IN  EFI_LEGACY_REGION2_PROTOCOL   *This,
  OUT UINT32                        *DescriptorCount,
  OUT EFI_LEGACY_REGION_DESCRIPTOR  **Descriptor
  )
{
  return EFI_UNSUPPORTED;
}



/**

  Tests if range 1 contains range 2 in its entirety.

  @param Start1   -  start of containing range.
  @param Length1  -  size of containing range.
  @param Start2   -  start of contained range.
  @param Length2  -  size of contained range.

  @retval TRUE  -  Range 1 contains range 2, FALSE otherwise.

**/
BOOLEAN
LegacyRegionRangeContainsRange (
  IN  UINT32                              Start1,
  IN  UINT32                              Length1,
  IN  UINT32                              Start2,
  IN  UINT32                              Length2
  )
{
  if ((Start2 >= Start1) && ((Start2 + Length2) <= (Start1 + Length1))) {
    return TRUE;
  }

  return FALSE;
}

/**

  Tests if two ranges have a common section i.e., they intersect.

  @param Start1   -  start of first range.
  @param Length1  -  size of first range.
  @param Start2   -  start of second range.
  @param Length2  -  size of second range.

  @retval TRUE  -  If ranges intersect, FALSE otherwise.

**/
BOOLEAN
LegacyRegionRangeIntersectsRange (
  IN  UINT32                              Start1,
  IN  UINT32                              Length1,
  IN  UINT32                              Start2,
  IN  UINT32                              Length2
  )
{
  if (((Start1 >= Start2) && (Start1 < (Start2 + Length2))) ||
      (((Start1 + Length1) > Start2) && ((Start1 + Length1) <= (Start2 + Length2))) ||
      ((Start2 >= Start1) && (Start2 < (Start1 + Length1))) ||
      (((Start2 + Length2) > Start1) && ((Start2 + Length2) <= (Start1 + Length1)))
      ) {
    return TRUE;
  }

  return FALSE;
}


/**

  Modify PAM registers for region specified.
  This function is written in a chipset inspecific way.
  All it assumes is that the region control registers are
  in the PCI config space on bus 0. device 0, function 0.
  Specific bytes and bits covering the specific regions are
  defined in a global variable mPamData.
  Porting of this function should be limited to only
  updating entries in the mPamData.

  @param Start        -  Start of the region to control access to.
  @param Length       -  Lenght  of the region to control access to.
  @param EnableRead   -  Controls if Read asscess should be enabled/disabled/untouched.
  @param EnableWrite  -  Controls if Write asscess should be enabled/disabled/untouched.
  @param Granularity  -  Granularity of region in bytes.

  @retval EFI_SUCCESS  -  Legacy BIOS protocol installed.

**/
EFI_STATUS
LegacyRegionAccessControl (
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  IN  BOOLEAN3                            EnableRead,
  IN  BOOLEAN3                            EnableWrite,
  OUT UINT32                              *Granularity OPTIONAL
  )
{
  UINT8                           Idx;
  UINT8                           Socket;
  UINT32                          PamData;
  BOOLEAN                         Intersects;
  UINT32                          TempGranularity;
  BOOLEAN                         Contains;
  PAM0123_CHABC_SAD_STRUCT        Pam0123;
  PAM456_CHABC_SAD_STRUCT         Pam456;

  TempGranularity = 0;
  //
  // Iterate regions controlled by individual PAM registers.
  //
  for (Idx = 0; Idx < PAM_DATA_NO; Idx++) {
    //
    // Test if the ranges intersect and/or contain each other.
    //
    Intersects = LegacyRegionRangeIntersectsRange (
                  Start,
                  Length,
                  mPamData[Idx].Start,
                  mPamData[Idx].Length
                  );

    //
    // Non-intersection means non-relevance.
    //
    if (!Intersects) {
      continue;
    }
    //
    // Save the max length of the region for the granularity
    //
    if (mPamData[Idx].Length > TempGranularity) {
      TempGranularity = mPamData[Idx].Length;
    }

    Contains = LegacyRegionRangeContainsRange (
                Start,
                Length,
                mPamData[Idx].Start,
                mPamData[Idx].Length
                );

    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if ((mPrivateData.SocketPresentBitMap >> Socket) & 1)
        break;
    }
    Pam0123.Data = mCpuCsrAccess->ReadCpuCsr(Socket, 0, PAM0123_CHABC_SAD_REG);
    Pam456.Data = mCpuCsrAccess->ReadCpuCsr(Socket, 0, PAM456_CHABC_SAD_REG);
    //
    // Enable/Disable Read/Write region attributes.
    // The enable will occur if the requested region
    // intesects with the region controlled by this PAM.
    // The disable will occur only if the requested region
    // is contained within the region controlled by this PAM.
    //
    PamData = 0;
    switch(mPamData[Idx].PamOffset) {
      case PAM0HI:
        PamData = Pam0123.Bits.pam0_hienable;
        break;
      case PAM1LO:
        PamData = Pam0123.Bits.pam1_loenable;
        break;
      case PAM1HI:
        PamData = Pam0123.Bits.pam1_hienable;
        break;
      case PAM2LO:
        PamData = Pam0123.Bits.pam2_loenable;
        break;
      case PAM2HI:
        PamData = Pam0123.Bits.pam2_hienable;
        break;
      case PAM3LO:
        PamData = Pam0123.Bits.pam3_loenable;
        break;
      case PAM3HI:
        PamData = Pam0123.Bits.pam3_hienable;
        break;
      case PAM4LO:
        PamData = Pam456.Bits.pam4_loenable;
        break;
      case PAM4HI:
        PamData = Pam456.Bits.pam4_hienable;
        break;
      case PAM5LO:
        PamData = Pam456.Bits.pam5_loenable;
        break;
      case PAM5HI:
        PamData = Pam456.Bits.pam5_hienable;
        break;
      case PAM6LO:
        PamData = Pam456.Bits.pam6_loenable;
        break;
      case PAM6HI:
        PamData = Pam456.Bits.pam6_hienable;
        break;
      default:
        break;  
    }
    if (EnableRead == Bool3False) {
      if (Contains) {
        PamData &= ~BIT0;
      }
    } else if (EnableRead == Bool3True) {
      PamData |= BIT0;
    }
    if (EnableWrite == Bool3False) {
      if (Contains) {
        PamData &= ~BIT1;
      }
    } else if (EnableWrite == Bool3True) {
      PamData |= BIT1;
    }

    switch(mPamData[Idx].PamOffset) {
      case PAM0HI:
        Pam0123.Bits.pam0_hienable = PamData;
        break;
      case PAM1LO:
        Pam0123.Bits.pam1_loenable = PamData;
        break;
      case PAM1HI:
        Pam0123.Bits.pam1_hienable = PamData;
        break;
      case PAM2LO:
        Pam0123.Bits.pam2_loenable = PamData;
        break;
      case PAM2HI:
        Pam0123.Bits.pam2_hienable = PamData;
        break;
      case PAM3LO:
        Pam0123.Bits.pam3_loenable = PamData;
        break;
      case PAM3HI:
        Pam0123.Bits.pam3_hienable = PamData;
        break;
      case PAM4LO:
        Pam456.Bits.pam4_loenable = PamData;
        break;
      case PAM4HI:
        Pam456.Bits.pam4_hienable = PamData;
        break;
      case PAM5LO:
        Pam456.Bits.pam5_loenable = PamData;
        break;
      case PAM5HI:
        Pam456.Bits.pam5_hienable = PamData;
        break;
      case PAM6LO:
        Pam456.Bits.pam6_loenable = PamData;
        break;
      case PAM6HI:
        Pam456.Bits.pam6_hienable = PamData;
        break;
      default:
        break;  
    }
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if ((mPrivateData.SocketPresentBitMap >> Socket) & 1) {
        mCpuCsrAccess->WriteCpuCsr(Socket, 0, PAM0123_CHABC_SAD_REG, Pam0123.Data);
        mCpuCsrAccess->WriteCpuCsr(Socket, 0, PAM456_CHABC_SAD_REG, Pam456.Data);
      }
    }
  }

  if (Granularity != NULL) {
    *Granularity = TempGranularity;
  }

  return EFI_SUCCESS;
}

/**
  Enable/Disable decoding of the given region.

  @param This    -  EFI_LEGACY_REGION2_PROTOCOL
  @param Start   -  Starting address of region.
  @param Length  -  Length of region in bytes.
  @param On      -  0 = Disable decoding, 1 = Enable decoding.

  @retval EFI_SUCCESS  -  Decoding change affected.
  @retval Other        -  Decoding unmodified.

**/
EFI_STATUS
EFIAPI
LegacyRegionDecode (
  IN  EFI_LEGACY_REGION2_PROTOCOL          *This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                             *Granularity,
  IN  BOOLEAN                             *On
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**

  Make the indicated region read from ROM /write to ROM.

  @param This         -  EFI_LEGACY_REGION2_PROTOCOL.
  @param Start        -  Starting address of region.
  @param Length       -  Length of region in bytes.
  @param Granularity  -  Granularity of region in bytes.

  @retval EFI_SUCCESS  -  Region locked or made R/O.

**/
EFI_STATUS
EFIAPI
LegacyRegionLock (
  IN  EFI_LEGACY_REGION2_PROTOCOL          * This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity OPTIONAL
  )
{
    if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Disable write, do not touch read.
  //
  if ((mIioUds->IioUdsPtr->SystemStatus.cpuType == CPU_SKX) &&
    (mIioUds->IioUdsPtr->SystemStatus.MinimumCpuStepping < B0_REV_SKX)) {
  //SKX A0 Stepping WA
  return LegacyRegionAccessControl (Start, Length, Bool3DontKnow, Bool3True, Granularity);
  //END OF WA
  } else {
  return LegacyRegionAccessControl (Start, Length, Bool3DontKnow, Bool3False, Granularity);
  }
}

/**

  Make the indicated region write protected.
  In addition to LegacyRegionLock(), this function assures
  that the address is not aliased to other addressable space.

  @param This         -  Add argument and description to function.
  @param Start        -  Starting address of region.
  @param Length       -  Length of region in bytes.
  @param Granularity  -  Granularity of region in bytes.

  @retval EFI_SUCCESS  -  Region locked or made R/O.

**/
EFI_STATUS
EFIAPI
LegacyRegionBootLock (
  IN EFI_LEGACY_REGION2_PROTOCOL           * This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity OPTIONAL
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Disable write, do not touch read.
  //
  if ((mIioUds->IioUdsPtr->SystemStatus.cpuType == CPU_SKX) &&
    (mIioUds->IioUdsPtr->SystemStatus.MinimumCpuStepping < B0_REV_SKX)) {
  //SKX A0 Stepping WA
  return LegacyRegionAccessControl (Start, Length, Bool3DontKnow, Bool3True, Granularity);
  //END OF WA
  } else {
  return LegacyRegionAccessControl(Start, Length, Bool3DontKnow, Bool3False, Granularity);
  }
}

/**

  Make the indicated region read from RAM /write to RAM.

  @param This         -  Pointer to Efi legacy region protocol interface instance.
  @param Start        -  Starting address of region.
  @param Length       -  Length of region in bytes.
  @param Granularity  -  Granularity of region in bytes.

  @retval EFI_SUCCESS  -  Region unlocked or made R/W.

**/
EFI_STATUS
EFIAPI
LegacyRegionUnlock (
  IN  EFI_LEGACY_REGION2_PROTOCOL          * This,
  IN  UINT32                              Start,
  IN  UINT32                              Length,
  OUT UINT32                              *Granularity OPTIONAL
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Enable write, enable read.
  //
  return LegacyRegionAccessControl (Start, Length, Bool3True, Bool3True, Granularity);
}

/**

  Install Driver to produce Legacy Region protocol.

  @param ImageHandle  -  The image handle.
  @param SystemTable  -  The system table.

  @retval EFI_SUCCESS  -  Legacy Region protocol installed.
  @retval Other        -  No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
LegacyRegionInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                  Status;

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);

  mPrivateData.SocketPresentBitMap = mIioUds->IioUdsPtr->SystemStatus.socketPresentBitMap;
  mPrivateData.Signature              = LEGACY_REGION_INSTANCE_SIGNATURE;
  mPrivateData.Handle                 = NULL;
  mPrivateData.ImageHandle            = ImageHandle;
  mPrivateData.LegacyRegion.Decode    = LegacyRegionDecode;
  mPrivateData.LegacyRegion.UnLock    = LegacyRegionUnlock;
  mPrivateData.LegacyRegion.Lock      = LegacyRegionLock;
  mPrivateData.LegacyRegion.BootLock  = LegacyRegionBootLock;
  mPrivateData.LegacyRegion.GetInfo   = LegacyRegionGetInfo;

  //
  // Install the protocol on a new handle.
  //
  Status = gBS->InstallProtocolInterface (
                &mPrivateData.Handle,
                &gEfiLegacyRegion2ProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mPrivateData.LegacyRegion
                );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
