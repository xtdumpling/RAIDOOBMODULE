//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  A Library to support PlatformDataRegion access in PEI Phase.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Ppi/Spi.h>
#include <Library/HobLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiPdrAccessLib.h>
#include <Guid/SetupVariable.h>
#include <Library/IoLib.h>
#include <Include/Register/PchRegsSpi.h>
#include <Include/PchAccess.h>
#include <Library/MmPciBaseLib.h>

#define BLOCK_SIZE_64KB  				 0x10000
#define BLOCK_SIZE_32KB                  0x8000
#define PDR_REGION_START_OFFSET		     0x0
#define PDR_REGION_START_OFFSET2	     0x8000
#define UUID_OFFSET                      0
#define PCAT_RTC_ADDRESS_REGISTER        0x70
#define PCAT_RTC_DATA_REGISTER           0x71


/**
  Update PlatformDataRegion information, not only for UUID. This routine reads the PDR information - 64KB size to memory buffer,
  and backup the PDR data, update memory buffer and then write back to PDR.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Buffer           Data to update.
  @param  DestOffset       Region to update, the offset of PDR.
  @param  BufferSize       The size of data to update.
  
  @return EFI_STATUS

**/
EFI_STATUS
UpdatePlatDataArea (
  IN        CONST EFI_PEI_SERVICES          **PeiServices,
  IN        UINT8                           *Buffer,
  IN        UINT32                          DestOffset,
  IN        UINT32                          BufferSize
)
{
  EFI_STATUS            Status;
  EFI_SPI_PROTOCOL      *SpiPpi;
  UINT8                 *BackUpBlock1, *BackUpBlock2, *TargetAddress;
  UINT8                 *BackUpBlock1Start, *BackUpBlock2Start;
  UINT32                DescSize;
  UINT32                Index;
  UINTN                 SpiBaseAddress;
  UINT8                 BiosControl;

  SpiBaseAddress = MmPciBase(
    DEFAULT_PCI_BUS_NUMBER_PCH,
    PCI_DEVICE_NUMBER_PCH_SPI,
    PCI_FUNCTION_NUMBER_PCH_SPI
    );

  //
  // Locate the SPI PPI Interface
  //
  Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gPeiSpiPpiGuid,
                    0,
                    NULL,
                    &SpiPpi
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Ensure and Clear EISS before attempting SPI flash update; EISS can be modified only if it is not locked [Bios Control:Bit1-Lock Enable]
  // Platform data area is updated with new UUID. 
  //
  BiosControl = MmioRead8(SpiBaseAddress + R_PCH_SPI_BC);
  if (BiosControl & B_PCH_SPI_BC_EISS) {
    MmioAnd8(SpiBaseAddress + R_PCH_SPI_BC, (UINT8)~B_PCH_SPI_BC_EISS);
  }
  //
  // SPI Write operation requires a block erase since even for a single bit the erase operation will clear an
  // entire block so before we attempt a write we must find a way to back up the block that it will be erased.
  //
  DescSize = BLOCK_SIZE_32KB;
  Status   = (*PeiServices)->AllocatePool (
                             PeiServices,
                             DescSize,
                             &BackUpBlock1
                             );
  
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AllocatePool BackUpBlock1 Status:%r\n",Status));
    goto PlatDataAreaEnd;
  }
  
  Status   = (*PeiServices)->AllocatePool (
                             PeiServices,
                             DescSize,
                             &BackUpBlock2
                             );
  if (EFI_ERROR (Status)) {
    goto PlatDataAreaEnd;
  }
  
  BackUpBlock1Start = BackUpBlock1;
  BackUpBlock2Start = BackUpBlock2;
  //
  // Read the PDR region, copy to buffer
  //
  Status = SpiPpi->FlashRead (
                              SpiPpi,
                              FlashRegionPlatformData,
                              PDR_REGION_START_OFFSET,
                              DescSize,
                              (UINT8 *)BackUpBlock1
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FlashRead the first 32kb Status:%r\n",Status));
    goto PlatDataAreaEnd;
  }
  Status = SpiPpi->FlashRead (
                              SpiPpi,
                              FlashRegionPlatformData,
                              PDR_REGION_START_OFFSET2,
                              DescSize,
                              (UINT8 *)BackUpBlock2
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FlashRead the second 32kb Status:%r\n",Status));
    goto PlatDataAreaEnd;
  } 
  //
  // Adjust the BackupBlock pointer to the offset passed in, and copy the data in the buffer passed in
  // to the BackupBlock
  //
  if(DestOffset<DescSize)
    TargetAddress  = BackUpBlock1 + DestOffset;
  else
	TargetAddress  = BackUpBlock2 + (DestOffset - DescSize);

  for (Index = 0; Index < BufferSize; Index++, TargetAddress++) {
    *TargetAddress = Buffer[Index];
  }
  //
  //Erase the PDR
  //
  Status = SpiPpi->FlashErase (
                          SpiPpi,
                          FlashRegionPlatformData,
                          PDR_REGION_START_OFFSET,
                          BLOCK_SIZE_64KB
                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FlashErase Status:%r\n",Status));
    goto PlatDataAreaEnd;
  } 
  //
  // Write back the PDR with the new data added.
  //
  Status = SpiPpi->FlashWrite(
                          SpiPpi,
                          FlashRegionPlatformData,
                          PDR_REGION_START_OFFSET,
                          DescSize,
                          (UINT8 *)BackUpBlock1
                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "FlashWrite back the first 32 Status:%r\n",Status));
    goto PlatDataAreaEnd;
  }
	  
  Status = SpiPpi->FlashWrite(
                          SpiPpi,
                          FlashRegionPlatformData,
                          PDR_REGION_START_OFFSET2,
                          DescSize,
                          (UINT8 *)BackUpBlock2
                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UUID Update failed !\n"));
  }
	
PlatDataAreaEnd:
  //
  // Restore EISS value to BC register
  //
  if (BiosControl & B_PCH_SPI_BC_EISS) {
    MmioWrite8(SpiBaseAddress + R_PCH_SPI_BC, BiosControl);
  }
  return Status;
}


/**
  Read PDR Information.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Pdr              A pointer to PLATFORM_PDR_TABLE, which contains PDR information.
  
  @return EFI_STATUS

**/
EFI_STATUS
PlatDataRegRead (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  OUT PLATFORM_PDR_TABLE         *Pdr
  )
{
  EFI_STATUS              Status;
  EFI_SPI_PROTOCOL        *SpiPpi;
  UINTN                   Size;

  //
  // Locate the SPI PPI Interface
  //
  Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gPeiSpiPpiGuid,
                    0,
                    NULL,
                    &SpiPpi
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Cannot Locate gPchSpiPpiGuid!\n"));
    return Status;
  }
  //
  // Read the PIT (Platform Info Table) from the SPI Flash Platform Data Region
  //
  Size = sizeof(PLATFORM_PDR_TABLE);

  Status = SpiPpi->FlashRead(
                          SpiPpi,
                          FlashRegionPlatformData,
                          0,
                          (UINT32) Size,
                          (UINT8 *) Pdr
                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "PIT from SPI PDR reports Platform ID invalidation. Status:%r!\n", Status));
	}
  return Status;
}

/**
  Read UUID from PDR and update to UuidInfoHob.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  UuidBuffer       A pointer to UUID info.
  
  @return EFI_STATUS

**/
EFI_STATUS
UuidGetByPdr (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *UuidBuffer
  )
{
  EFI_STATUS              Status;
  PLATFORM_PDR_TABLE      Pdr;

  Status = PlatDataRegRead(PeiServices, &Pdr);
  if (!EFI_ERROR (Status)) {
    CopyMem (UuidBuffer, Pdr.SystemUuid, sizeof (EFI_GUID));
  }

  return Status;
}

/**
  RP require UUID value cannot be full 00's or FF's for PXE PDK tests.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Buffer           A pointer to UUID.
  
  @return EFI_STATUS

**/
EFI_STATUS
RpUuidRefresh (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN OUT UINT8                  *SystemUuid
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_SPI_PROTOCOL        *SpiPpi;
  UINTN                   Size;
  UINT8                   *pPIT;
  UINT8                   i;
  BOOLEAN                 UuidValid;

  //
  // Locate the SPI PPI Interface
  //
  Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gPeiSpiPpiGuid,
                    0,
                    NULL,
                    &SpiPpi
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  pPIT = (UINT8 *)SystemUuid;
  UuidValid = FALSE;
  for (i = 0; i < 16; i++) {
    if ( pPIT[i] != 0xff && pPIT[i] != 0x00 ) {      
       UuidValid = TRUE;
       break;
    }
  }

  if (!UuidValid) {
    //
    // let's fill the first six bytes of our UUID with internal GbE controller MAC address
    //
	Status = SpiPpi->FlashRead (
                          SpiPpi,
                          FlashRegionGbE,
                          0,
                          6,
                          SystemUuid
                        );
	if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "GetPDRUUID: ERROR: Can't get GbE MAC address\n"));
      //
      // if error, fill with generic data
      //
      for (i = 0; i < 6; i++) {
    	  pPIT[i] = 0xa5;
      }
	}
	//
    // fill another six bytes with current time
    //
    pPIT[6] = RtcReadDec(RTC_YEAR);
    pPIT[7] = RtcReadDec(RTC_DAY);
    pPIT[8] = RtcReadDec(RTC_MONTH);
    pPIT[9] = RtcReadDec(RTC_HOURS);
    pPIT[10] = RtcReadDec(RTC_MINUTES);
    pPIT[11] = RtcReadDec(RTC_SECONDS);
    
    //
    // fill the rest with some generic filler
    //
    for (i = 12; i < 16; i++) {
      pPIT[i] = 0xa5;
    }
    
	Size = sizeof (EFI_GUID);
    Status = UpdatePlatDataArea (
      PeiServices,
      SystemUuid,
      UUID_OFFSET,
      Size
      );
	}
  return Status;
}

/**
  Get variable MfgSystemUuid, which was programmed by tool in MFG Mode.

  @param  PeiServices      Describes the list of possible PEI Services.
  @param  Buffer           A pointer to variable MfgSystemUuid.
  
  @return EFI_STATUS

**/
EFI_STATUS
GetMfgSystemUuidVar (
  IN  EFI_PEI_SERVICES           **PeiServices,
  OUT UINT8                      *Buffer
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *PeiVariable;
  UINTN                            SizeUUID;

  //
  // Locate the Setup configuration value.
  //
  Status = (*PeiServices)->LocatePpi (
                            PeiServices,
                            &gEfiPeiReadOnlyVariable2PpiGuid,
                            0,
                            NULL,
                            &PeiVariable
                            );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Check if MFG UUID variable is present
  //
  SizeUUID = sizeof (EFI_GUID);
  Status = PeiVariable->GetVariable (
                          PeiVariable,
                          L"MfgSystemUuid",
                          &gEfiSetupVariableGuid,
                          NULL,
                          &SizeUUID,
                          Buffer
                          );

  return Status;
}

/**
  Check for variable containing the Mfg UUID, if exists, update PDR with new UUID, if not found, get UUID from PDR, and then sync to PlatformInfoHob.

  @param  PeiServices          Describes the list of possible PEI Services.
  @param  Buffer               A pointer to UUID INFO.
  
  @return Status

**/
EFI_STATUS
MfgUuidGet (
  IN  EFI_PEI_SERVICES           **PeiServices,
  OUT UINT8                      *Buffer
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINT32                          SizeUUID;
  PLATFORM_PDR_TABLE              Pdr;

  Status = GetMfgSystemUuidVar((EFI_PEI_SERVICES**)PeiServices, Buffer);
  if(!EFI_ERROR (Status)){
    //
    // Update the PDR in flash with the UUID.
    //
    SizeUUID = sizeof (EFI_GUID);
    Status = UpdatePlatDataArea (
      PeiServices,
      Buffer,
      UUID_OFFSET,
      SizeUUID
      );
	}else{
      //
	  // Failed to Get Variable MfgSystemUuid, get UUID from PDR
	  //
      Status = PlatDataRegRead(PeiServices, &Pdr);
	  if (!EFI_ERROR (Status)) {
	    CopyMem (Buffer, Pdr.SystemUuid, sizeof (EFI_GUID));
      }
  }

  return Status;
}

/**

  Read RTC data register and return contents after converting it to decimal.
  Needed here because it looks like other methods of getting time (ex. gRT->GetTime) tend to fail.

  @param Address RTC Register address to read
  @retval Value of data register contents converted to Decimal

**/
UINT8
RtcReadDec (
  IN RTC_ADDRESS Address
  )
{
  UINT8 High;
  UINT8 Low;
  UINT8 Result;
  
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (IoRead8 (PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  Result = IoRead8 (PCAT_RTC_DATA_REGISTER);
  
  High = Result >> 4;
  Low = Result - (High << 4);

  return (UINT8) (Low + (High * 10));
}

