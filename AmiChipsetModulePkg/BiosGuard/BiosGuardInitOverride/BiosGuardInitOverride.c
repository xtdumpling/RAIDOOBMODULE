/** @file
  BIOS Guard EarlyPost initializations.

@copyright
  Copyright (c) 2011 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification
**/

#include "BiosGuardInitOverride.h"
#include "Library/BaseCryptLib.h"
#include "FlashMap.h"

EFI_GUID BiosGuardPubKeyGuid = BIOS_GUARD_PUB_KEY_FFS_FILE_RAW_GUID;

extern EFI_GUID gBiosGuardHobGuid;

//
// Private GUID for BIOS Guard initializes
//
extern EFI_GUID gBiosGuardModuleGuid;

/**
    Loads binary from RAW section of X firwmare volume

    @param PpSv 
    @param Buffer Returns a pointer to allocated memory. Caller
        must free it when done.

    @retval EFI_STATUS

**/
EFI_STATUS
LocateBiosGuardPubKey (
    IN CONST EFI_PEI_SERVICES       **PpSv,
    IN OUT VOID                     **Buffer
)
{
    EFI_STATUS                    Status;
    EFI_FIRMWARE_VOLUME_HEADER    *pFV;
    UINTN                         FvNum=0;
    EFI_FFS_FILE_HEADER           *ppFile=NULL;
    BOOLEAN                       Found = FALSE;

    Status = (*PpSv)->FfsFindNextVolume (PpSv, FvNum, &pFV);

    while ( TRUE ) {
        Status = (*PpSv)->FfsFindNextVolume( PpSv, FvNum, &pFV );
        if ( EFI_ERROR( Status )) {
            return Status;
        }

        ppFile = NULL;

        while ( TRUE ) {
            Status = (*PpSv)->FfsFindNextFile( PpSv,
                                               EFI_FV_FILETYPE_FREEFORM,
                                               pFV,
                                               &ppFile );

            if ( Status == EFI_NOT_FOUND ) break;            

            if ( CompareGuid( &ppFile->Name, &BiosGuardPubKeyGuid ) ) {             
                Found = TRUE;
                break;
            }
        }

        if ( Found ) {
            break;        
        } else {
            FvNum++;
        }
    }

     Status = (*PpSv)->FfsFindSectionData( PpSv,
                                          EFI_SECTION_RAW,
                                          ppFile,
                                          Buffer );

    if ( EFI_ERROR( Status ) ) {
        return EFI_NOT_FOUND;
    }

    return Status;
}

/**
  Get the BIOS Guard Module pointer.

  @param[in, out] ModulePtr - Input is a NULL pointer,
                              and output points BIOS Guard module address if found.

  @retval EFI_SUCCESS       - BIOS Guard Module found.
  @retval EFI_NOT_FOUND     - BIOS Guard Module not found.
**/
EFI_STATUS
UpdateBiosGuardModulePtr (
  IN OUT EFI_PHYSICAL_ADDRESS   *ModulePtr
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME_HEADER    *FvHeader;
  EFI_FFS_FILE_HEADER           *FfsFile;
  EFI_GUID                      *BiosGuardModuleGuidPtr;
  UINT32                        ModuleAddr;

  BiosGuardModuleGuidPtr      = &gBiosGuardModuleGuid;
  FfsFile                     = NULL;
  Status                      = EFI_SUCCESS;

  ///
  /// Locate Firmware Volume header
  ///
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) FV_BB_BASE;  

  while (TRUE) {
    ///
    /// Locate Firmware File System file within Firmware Volume
    ///
    Status = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_RAW, FvHeader, (VOID **)&FfsFile);

    if (EFI_ERROR(Status)) {
      return EFI_NOT_FOUND;
    }

    ///
    /// Validate that the found Firmware File System file is the BIOS Guard Module
    ///
    if (CompareGuid (&(FfsFile->Name), BiosGuardModuleGuidPtr)) {
      ModuleAddr = (UINT32)((UINT8 *) FfsFile + sizeof (EFI_FFS_FILE_HEADER));
      DEBUG(( DEBUG_ERROR, "BIOS Guard Module Location: %x\n", ModuleAddr));
      *ModulePtr = (EFI_PHYSICAL_ADDRESS) ModuleAddr;
      return Status;
    }
  }
}

/*++

  Perform the restoration of anti-flash wear out settings in case of S3 resume.

  @param struct sysHost  *host  - sysHost struct with system configuration information

  @retval None

--*/
VOID
RestoreAntiFlashWearout(
    IN struct sysHost             *host
)
{
  UINT64              MsrValue;
  UINT64_STRUCT       MsrReg;
  UINT8               SocketId;

  //
  // Set the WPE bit to enable Anti Flash Wearout on each CPU
  //
  MsrValue = AsmReadMsr64 (MSR_BIOS_INFO_FLAGS);
  MsrValue |= B_MSR_BIOS_INFO_FLAGS_WPE;
  MsrReg.hi = 0;
  MsrReg.lo = (UINT32)MsrValue;
  
  for(SocketId = 0; SocketId < MAX_SOCKET; SocketId++) {
    if ((host->var.common.socketPresentBitMap & (1 << SocketId)) != 0)
      WriteMsrPipe(host, SocketId, MSR_BIOS_INFO_FLAGS, MsrReg);
  }
}

/**

  Initial settings for BIOS Guard Config structure.  Taken from Sample Code.

  @param BiosGuardConfig           - Pointer to the BIOS Guard config structure.

  @retval None

**/
VOID
InitializeBiosGuardConfigData(
  IN BIOSGUARD_CONFIG *BiosGuardConfig
  )
{
  UINT8 PlatIdStr[] = "PURLEY";

  ///
  /// If CpuConfig->BiosGuard is set to ENABLE '1' then
  ///   PlatformData->SmmBwp (found in PchPolicyInitPei.c file) has to be set to ENABLE '1'
  /// This is a BIOS Guard Security requirement that needs to be addressed
  /// If CpuConfig->BiosGuard is set to DISABLE '0' then
  ///   PlatformData->SmmBwp (found in PchPolicyInitPei.c file) value don't care, it can be
  ///   set to either ENABLE '1' or DISABLE '0' based on customer implementation
  ///
  ZeroMem (&BiosGuardConfig->Bgpdt, sizeof (BGPDT));
  BiosGuardConfig->Bgpdt.BgpdtMajVer      = BGPDT_MAJOR_VERSION;
  BiosGuardConfig->Bgpdt.BgpdtMinVer      = BGPDT_MINOR_VERSION;
  CopyMem (&BiosGuardConfig->Bgpdt.PlatId[0], &PlatIdStr[0], sizeof (PlatIdStr));
  BiosGuardConfig->Bgpdt.BgModSvn         = BIOSGUARD_SVN;
  BiosGuardConfig->Bgpdt.BiosSvn          = 0x00640000;
  BiosGuardConfig->Bgpdt.LastSfam         = MIN_SFAM_COUNT - 1;
  BiosGuardConfig->Bgpdt.SfamData[0].FirstByte = 0x00580000;
  BiosGuardConfig->Bgpdt.SfamData[0].LastByte  = 0x0058FFFF;
  BiosGuardConfig->Bgpdt.BgpdtSize             = (sizeof (BGPDT) - sizeof (BiosGuardConfig->Bgpdt.SfamData) + ((BiosGuardConfig->Bgpdt.LastSfam + 1) * sizeof (SFAM_DATA)));
  BiosGuardConfig->BgpdtHash[0]                = 0xae7295370672663c;
  BiosGuardConfig->BgpdtHash[1]                = 0x220375c996d23a36;
  BiosGuardConfig->BgpdtHash[2]                = 0x73aaea0f2afded9d;
  BiosGuardConfig->BgpdtHash[3]                = 0x707193b768a0829e;
  ZeroMem (&BiosGuardConfig->BgupHeader, sizeof (BGUP_HEADER));
  BiosGuardConfig->BgupHeader.Version          = BGUP_HDR_VERSION;
  CopyMem (&BiosGuardConfig->BgupHeader.PlatId[0], &PlatIdStr[0], sizeof (PlatIdStr));
  BiosGuardConfig->BgupHeader.PkgAttributes    = 0;
  BiosGuardConfig->BgupHeader.PslMajorVer      = PSL_MAJOR_VERSION;
  BiosGuardConfig->BgupHeader.PslMinorVer      = PSL_MINOR_VERSION;
  BiosGuardConfig->BgupHeader.BiosSvn          = BiosGuardConfig->Bgpdt.BiosSvn;
  BiosGuardConfig->BgupHeader.EcSvn            = 0;
  BiosGuardConfig->BgupHeader.VendorSpecific   = 0x808655AA;
  ZeroMem (&BiosGuardConfig->BiosGuardLog, sizeof (BIOSGUARD_LOG));
  BiosGuardConfig->BiosGuardLog.Version        = BIOSGUARD_LOG_VERSION;
  BiosGuardConfig->BiosGuardMemSize       = (UINT8)RShiftU64(FLASH_SIZE + SIZE_1MB, 20);

  return;
}

/**

  Final customizations to the BIOS Guard Platform structure are made here.
  Keys or any other specific changes should be done here.

  @param **PeiServices          - Indirect reference to the PEI Services Table.
  @param *BiosGuardConfig       - Pointer to the BIOS Guard config structure.
  @param FlashWearOutProtection - Value from Setup if FlashWearOut is enabled.

  @retval None

**/
VOID
CustomizeBiosGuardConfigData(
  IN EFI_PEI_SERVICES     **PeiServices,
  IN BIOSGUARD_CONFIG     *BiosGuardConfig,
  IN UINT8                FlashWearOutProtection
  )
{
  EFI_STATUS        Status;
  UINT32            FlashBase;
  UINT8             NumSpiComponents;
  UINT32            TotalFlashSize;
  VOID              *Sha256Context;
  VOID              *Data;
  UINTN             DataSize;
  UINT8             HashValue[SHA256_DIGEST_SIZE];
  UINT8             *Sha256PubKeyDigest;
  UINT8             BiosGuardSfamCount;
  UINT8             AmiSfamOemCount =0;
  UINTN             PchSpiBase;
  SFAM_DATA         AmiSfamOemData[] = { AMI_BIOS_GUARD_SFAM_OEM_1, 
                                         AMI_BIOS_GUARD_SFAM_OEM_2, 
                                         AMI_BIOS_GUARD_SFAM_OEM_3, 
                                         AMI_BIOS_GUARD_SFAM_OEM_4 };
  
  Status  = LocateBiosGuardPubKey(PeiServices ,&Sha256PubKeyDigest);
  ASSERT_EFI_ERROR (Status);
  //
  // If it fails it might be in non BGUPC mode
  //
  if ( !EFI_ERROR( Status ) ) {
      BiosGuardConfig->BgupHeader.PkgAttributes = 0;
      CopyMem (&BiosGuardConfig->Bgpdt.PkeySlot0, Sha256PubKeyDigest, 32);
  }
  
  PchSpiBase = MmioRead32 (MmPciBase (
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                            PCI_DEVICE_NUMBER_PCH_SPI,
                            PCI_FUNCTION_NUMBER_PCH_SPI)
                            + R_PCH_SPI_BAR0) & ~B_PCH_SPI_BAR0_MASK;
    ASSERT (PchSpiBase != 0);
  //
  // Select to Flash Map 0 Register to get the number of flash Component
  //
  MmioAndThenOr32 (
    (PchSpiBase + R_PCH_SPI_FDOC),
    (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
    (UINT32) (V_PCH_SPI_FDOC_FDSS_FSDM | R_PCH_SPI_FDBAR_FLASH_MAP0)
  );
  /**
    Copy Zero based Number Of Components
    Valid Bit Settings:
      - 00 : 1 Component
      - 01 : 2 Components
      - All other settings : Reserved
  **/
  NumSpiComponents = (UINT8) ((MmioRead16 (PchSpiBase + R_PCH_SPI_FDOD) & B_PCH_SPI_FDBAR_NC) >> N_PCH_SPI_FDBAR_NC);
  //
  // Select to Flash Components Register to get Components Density
  //
  MmioAndThenOr32 (
    (PchSpiBase + R_PCH_SPI_FDOC),
    (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
    (UINT32) (V_PCH_SPI_FDOC_FDSS_COMP | R_PCH_SPI_FCBA_FLCOMP)
  );
  ///
  /// Calculate TotalFlashSize from Descriptor information
  ///
  FlashBase = (UINT8) MmioRead32 (PchSpiBase + R_PCH_SPI_FDOD);
  TotalFlashSize = (SPI_SIZE_BASE_512KB << ((UINT8) (FlashBase & B_PCH_SPI_FLCOMP_COMP0_MASK)));
  if (NumSpiComponents == 1) {
    TotalFlashSize += (SPI_SIZE_BASE_512KB << ((UINT8) ((FlashBase & B_PCH_SPI_FLCOMP_COMP1_MASK) >> 4)));
  }

  FlashBase = TotalFlashSize - FLASH_SIZE;
  
  BiosGuardConfig->TotalFlashSize = TotalFlashSize;

  //
  // We only protect the ME, FV_MAIN, FV_DATA and FV_BB and protect regions must to be signed.
  // SfamData[0] : BIOS Guard protects the ME Region from 0 to BIOS Starting address.
  //
  BiosGuardSfamCount = 0;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = 0;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = FlashBase - 1;

  //
  // SfamData[1] : BIOS Guard protects the FV_MAIN Region from FV_MAIN to FV_MAIN + FV_MainLength.
  //
  BiosGuardSfamCount++;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = FlashBase + FV_MainStartingAddress;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = FlashBase + FV_MainStartingAddress + FV_MainLength - 1;
 
#if defined IntelLtsxFit_SUPPORT && IntelLtsxFit_SUPPORT
  //
  // SfamData[2] : BIOS Guard protects the FV_FIT Region from FV_FIT to FV_FIT + FV_FITLength.
  //
  BiosGuardSfamCount++;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = FlashBase + FV_FITStartingAddress;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = FlashBase + FV_FITStartingAddress + FV_FITLength - 1;
#endif

#if defined INTEL_TXT_SUPPORT && INTEL_TXT_SUPPORT
  //
  // SfamData[3] : BIOS Guard protects the FV_ACM Region from FV_ACM to FV_ACM + FV_ACMLength.
  //
  BiosGuardSfamCount++;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = FlashBase + FV_ACMStartingAddress;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = FlashBase + FV_ACMStartingAddress + FV_ACMLength - 1;
#endif
    
  //
  // SfamData[4] : BIOS Guard protects the FV_BB Region from FV_BB to FV_BB + FV_BBLength.
  //
  BiosGuardSfamCount++;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = FlashBase + FV_BBStartingAddress;
  BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = FlashBase + FV_BBStartingAddress + (FV_BBLength - 1);        
  
  for ( AmiSfamOemCount = 0; AmiSfamOemCount < 4; AmiSfamOemCount++  ) {
      if ( AmiSfamOemData[AmiSfamOemCount].LastByte != 0 ) {
          //
          // SfamData[5~8] : BIOS Guard protects the OEM Region from OEM to OEM + OEMLength.
          //
          BiosGuardSfamCount++;
          BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte = FlashBase + AmiSfamOemData[AmiSfamOemCount].FirstByte;
          BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].LastByte  = FlashBase + AmiSfamOemData[AmiSfamOemCount].FirstByte + AmiSfamOemData[AmiSfamOemCount].LastByte -1;
      }
  }
  
  //
  // LastSfam : Number of SfamData
  //
  BiosGuardConfig->Bgpdt.LastSfam = BiosGuardSfamCount;
  if ( BiosGuardConfig->Bgpdt.LastSfam > (MAX_SFAM_COUNT - 1) ) {
      BiosGuardConfig->Bgpdt.LastSfam = MAX_SFAM_COUNT - 1;
  }

  for ( BiosGuardSfamCount = 0; BiosGuardSfamCount <= BiosGuardConfig->Bgpdt.LastSfam; BiosGuardSfamCount++ ) {
      DEBUG ((EFI_D_INFO, "[BiosGuardInitLibOverride.c] BiosGuardConfig->Bgpdt.SfamData[%d].FirstByte: %X \n", BiosGuardSfamCount, BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].FirstByte));
      DEBUG ((EFI_D_INFO, "[BiosGuardInitLibOverride.c] BiosGuardConfig->Bgpdt.SfamData[%d].LastByte: %X \n", BiosGuardSfamCount, BiosGuardConfig->Bgpdt.SfamData[BiosGuardSfamCount].LastByte));
  }
  
  Status = UpdateBiosGuardModulePtr(&BiosGuardConfig->BiosGuardModulePtr);
  ASSERT_EFI_ERROR (Status);
  
  //
  // Defined values in the token
  //
  BiosGuardConfig->Bgpdt.BiosSvn = BIOS_SVN;
  BiosGuardConfig->BgupHeader.BiosSvn = BiosGuardConfig->Bgpdt.BiosSvn;

  BiosGuardConfig->BiosGuardLog.LastPage       = MAX_BIOSGUARD_LOG_PAGE - 1;
  BiosGuardConfig->BiosGuardLog.LoggingOptions = BIOSGUARD_LOG_OPT_DEBUG | BIOSGUARD_LOG_OPT_FLASH_ERROR | BIOSGUARD_LOG_OPT_FLASH_ERASE | BIOSGUARD_LOG_OPT_FLASH_WRITE | BIOSGUARD_LOG_OPT_BRANCH_TRACE | BIOSGUARD_LOG_OPT_STEP_TRACE;

  if (FlashWearOutProtection) {
    BiosGuardConfig->Bgpdt.PlatAttr |= FLASH_WEAR_OUT_PROTECTION;
  }
  BiosGuardConfig->Bgpdt.PlatAttr |= FTU_ENABLE;
  BiosGuardConfig->Bgpdt.BgpdtSize = (sizeof (BGPDT) - sizeof (BiosGuardConfig->Bgpdt.SfamData) + ((BiosGuardConfig->Bgpdt.LastSfam + 1) * sizeof (SFAM_DATA)));
  DataSize = Sha256GetContextSize ();
  Status   = (*PeiServices)->AllocatePool (PeiServices, DataSize, &Sha256Context);
  ASSERT_EFI_ERROR (Status);

  Status  = LocateBiosGuardPubKey(PeiServices ,&Sha256PubKeyDigest);
  ASSERT_EFI_ERROR (Status);
  //
  // If it fails it might be in non BGUPC mode
  //
  if ( !EFI_ERROR( Status ) ) {
      BiosGuardConfig->BgupHeader.PkgAttributes = 0;
      CopyMem (&BiosGuardConfig->Bgpdt.PkeySlot0, Sha256PubKeyDigest, 32);
  }

  DataSize = BiosGuardConfig->Bgpdt.BgpdtSize;
  Data     = (VOID *) &BiosGuardConfig->Bgpdt;
  Sha256Init   (Sha256Context);
  Sha256Update (Sha256Context, Data, DataSize);
  Sha256Final  (Sha256Context, HashValue);
  CopyMem (&BiosGuardConfig->BgpdtHash[0], &HashValue[0], 8);
  CopyMem (&BiosGuardConfig->BgpdtHash[1], &HashValue[8], 8);
  CopyMem (&BiosGuardConfig->BgpdtHash[2], &HashValue[16], 8);
  CopyMem (&BiosGuardConfig->BgpdtHash[3], &HashValue[24], 8);

  return;
}

//#if 0
// ** Removing to make space
/**

  Prints all the BIOS Guard related settings to the screen.

  @param BiosGuardConfig           - Pointer to the BIOS Guard config structure.

  @retval None

**/
VOID
PrintBiosGuardInfo(
  IN BIOSGUARD_CONFIG          *BiosGuardConfig
  )
{
#if BIOS_GUARD_DEBUG_MODE
  UINT8 Index;
  //
  // BIOSGUARD_CONFIG - Print values
  //
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : Version : 0x%X\n", BiosGuardConfig->BgupHeader.Version));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : PlatId[16] :\n"));
  for (Index = 0; Index < 16; Index++) {
    if (Index == 15) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", BiosGuardConfig->Bgpdt.PlatId[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", BiosGuardConfig->Bgpdt.PlatId[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : PkgAttributes : 0x%X\n", BiosGuardConfig->BgupHeader.PkgAttributes));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : PslMajorVer : 0x%X\n", BiosGuardConfig->BgupHeader.PslMajorVer));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : PslMinorVer : 0x%X\n", BiosGuardConfig->BgupHeader.PslMinorVer));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : ScriptSectionSize : 0x%X\n", BiosGuardConfig->BgupHeader.ScriptSectionSize));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : DataSectionSize : 0x%X\n", BiosGuardConfig->BgupHeader.DataSectionSize));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : BiosSvn : 0x%X\n", BiosGuardConfig->BgupHeader.BiosSvn));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : EcSvn : 0x%X\n", BiosGuardConfig->BgupHeader.EcSvn));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGUP_HEADER : VendorSpecific : 0x%X\n", BiosGuardConfig->BgupHeader.VendorSpecific));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : BgpdtSize : 0x%X\n", BiosGuardConfig->Bgpdt.BgpdtSize));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : BgpdtMajVer : 0x%X\n", BiosGuardConfig->Bgpdt.BgpdtMajVer));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : BgpdtMinVer : 0x%X\n", BiosGuardConfig->Bgpdt.BgpdtMinVer));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : PlatId[16] :\n"));
  for (Index = 0; Index < 16; Index++) {
    if (Index == 15) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", BiosGuardConfig->Bgpdt.PlatId[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", BiosGuardConfig->Bgpdt.PlatId[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : PkeySlot0[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15) || (Index == 31)) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", BiosGuardConfig->Bgpdt.PkeySlot0[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", BiosGuardConfig->Bgpdt.PkeySlot0[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : PkeySlot1[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15) || (Index == 31)) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", BiosGuardConfig->Bgpdt.PkeySlot1[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", BiosGuardConfig->Bgpdt.PkeySlot1[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : PkeySlot2[32] :\n"));
  for (Index = 0; Index < 32; Index++) {
    if ((Index == 15)|| (Index == 31)) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", BiosGuardConfig->Bgpdt.PkeySlot2[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", BiosGuardConfig->Bgpdt.PkeySlot2[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : BiosGuardModSvn : 0x%X\n", BiosGuardConfig->Bgpdt.BgModSvn));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : BiosSvn : 0x%X\n", BiosGuardConfig->Bgpdt.BiosSvn));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : ExecLim : 0x%X\n", BiosGuardConfig->Bgpdt.ExecLim));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : PlatAttr : 0x%X\n", BiosGuardConfig->Bgpdt.PlatAttr));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : EcCmd : 0x%X\n", BiosGuardConfig->Bgpdt.EcCmd));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : EcData : 0x%X\n", BiosGuardConfig->Bgpdt.EcData));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : EcCmdGetSvn : 0x%X\n", BiosGuardConfig->Bgpdt.EcCmdGetSvn));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : EcCmdOpen : 0x%X\n", BiosGuardConfig->Bgpdt.EcCmdOpen));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : EcCmdClose : 0x%X\n", BiosGuardConfig->Bgpdt.EcCmdClose));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : EcCmdPortTest : 0x%X\n", BiosGuardConfig->Bgpdt.EcCmdPortTest));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : LastSfam : 0x%X\n", BiosGuardConfig->Bgpdt.LastSfam));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BGPDT : SfamData[64] - first %2d entries:\n", BiosGuardConfig->Bgpdt.LastSfam));
  //
  // Change the array size according to MAX_SFAM_COUNT
  //
  //for (Index = 0; Index < 64; Index++) {
  for (Index = 0; Index <= BiosGuardConfig->Bgpdt.LastSfam; Index++) {
    if ((Index == 15) || (Index == 31) || (Index == 47) || (Index == 63) ) {
      DEBUG ((EFI_D_INFO, " 0x%X\n", BiosGuardConfig->Bgpdt.SfamData[Index]));
    } else {
      DEBUG ((EFI_D_INFO, " 0x%X ,", BiosGuardConfig->Bgpdt.SfamData[Index]));
    }
  }
  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BgpdtHash[4] : 0x%lX , 0x%lX , 0x%lX , 0x%lX \n",   BiosGuardConfig->BgpdtHash[0], \
          BiosGuardConfig->BgpdtHash[1], \
          BiosGuardConfig->BgpdtHash[2], \
          BiosGuardConfig->BgpdtHash[3]));

  DEBUG ((EFI_D_INFO, " \n"));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BiosGuardMemSize : 0x%X\n", BiosGuardConfig->BiosGuardMemSize));
  DEBUG ((EFI_D_INFO, " BiosGuard:: EcCmdDiscovery : 0x%X\n", BiosGuardConfig->EcCmdDiscovery));
  DEBUG ((EFI_D_INFO, " BiosGuard:: EcCmdProvisionEav : 0x%X\n", BiosGuardConfig->EcCmdProvisionEav));
  DEBUG ((EFI_D_INFO, " BiosGuard:: EcCmdLock : 0x%X\n", BiosGuardConfig->EcCmdLock));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BiosGuardLOG:: Version : 0x%X\n", BiosGuardConfig->BiosGuardLog.Version));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BiosGuardLOG:: LastPage : 0x%X\n", BiosGuardConfig->BiosGuardLog.LastPage));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BiosGuardLOG:: LoggingOptions : 0x%X\n", BiosGuardConfig->BiosGuardLog.LoggingOptions));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BiosGuardLOG:: BiosGuardModSvn : 0x%X\n", BiosGuardConfig->BiosGuardLog.BgModSvn));
  DEBUG ((EFI_D_INFO, " BiosGuard:: BiosGuardLOG:: NumOfEntriesInLog : 0x%X\n", BiosGuardConfig->BiosGuardLog.NumOfEntriesInLog));
#endif

  return;
}
//#endif

/**

  Perform the platform spefific BIOS Guard initializations.

  @param **PeiServices                 - Indirect reference to the PEI Services Table.
  @param *TotalSystemConfigurationPtr  - Pointer to the Setup structure
  @param *host                         - sysHost struct with system configuration information

  @retval None

**/
VOID
BiosGuardInit (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN SYSTEM_CONFIGURATION       *TotalSystemConfigurationPtr,
  IN struct sysHost             *host
  )
{
  EFI_STATUS          Status;
  UINT64              MsrValue;
  BOOLEAN             ResetRequired;
  BIOSGUARD_HOB       *BiosGuardHobPtr;
  UINT8               SocketId;
  BIOSGUARD_CONFIG    BiosGuardConfig;
  BIOSGUARD_CONFIG    *BiosGuardConfigPtr;
  EFI_PLATFORM_INFO   *PlatformInfoHob;
  EFI_HOB_GUID_TYPE   *GuidHob;
  EFI_PEI_RESET_PPI   *ResetPpi;
  UINT64_STRUCT       MsrReg;
  EFI_BOOT_MODE       BootMode=0;
  SYSTEM_BOARD_PPI    *SystemBoard;

  DEBUG ((EFI_D_INFO, "BiosGuardInit\n"));

  ZeroMem (&BiosGuardConfig, sizeof (BIOSGUARD_CONFIG));
  BiosGuardConfigPtr = &BiosGuardConfig;
  //
  // Locate Platform Info Hob
  //
  GuidHob          = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return;
  }
  PlatformInfoHob  = GET_GUID_HOB_DATA(GuidHob); 

  //
  // Write WPE bit on S3 Resume in BIOS Info Flags MSR to renable Anti-Flash wearout protection
  //
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  if ((BootMode == BOOT_ON_S3_RESUME) && (TotalSystemConfigurationPtr->EnableAntiFlashWearout) &&
        (TotalSystemConfigurationPtr->AntiFlashWearoutSupported == TRUE)) {
    RestoreAntiFlashWearout(host);
  }

  //
  // Get SystemBoard PPI
  //
  (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &SystemBoard
                  );

  ResetRequired = FALSE;
  MsrValue = AsmReadMsr64 (EFI_PLATFORM_INFORMATION);
  if (!(MsrValue & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL)) {
    DEBUG ((EFI_D_INFO, "BIOS Guard Feature is not supported\n"));
    return;
  }

  //
  // Need to make sure that only supported Platforms can enable BIOS Guard
  //
  if ((SystemBoard->FeaturesBasedOnPlatform() & BIOSGUARD_SUPPORT_ENABLED) == 0) {
    TotalSystemConfigurationPtr->BiosGuardEnabled = FALSE;
    DEBUG ((EFI_D_INFO, "Platform ID check failed. BiosGuardInit skipped.\n"));
  }

  if (TotalSystemConfigurationPtr->BiosGuardEnabled) {
    DEBUG ((EFI_D_INFO, "BIOS Guard Module is Enabled\n"));

    InitializeBiosGuardConfigData(BiosGuardConfigPtr);
    CustomizeBiosGuardConfigData(PeiServices, BiosGuardConfigPtr, TotalSystemConfigurationPtr->EnableAntiFlashWearout);
    PrintBiosGuardInfo(BiosGuardConfigPtr);

    //
    // Save BIOS Guard Mem Size in the Platform Info Hob
    //
    PlatformInfoHob->MemData.BiosGuardMemSize  = BiosGuardConfigPtr->BiosGuardMemSize;

    ///
    /// Read BIOS Guard Control Register
    ///
    MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL);
    if (MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_LK) {
      if (!(MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_EN)) {
        ///
        /// Reset required as the BIOS Guard CTRL MSR is locked and needs to be toggled
        ///
        ResetRequired = TRUE;
      }
    } else {
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_0, BiosGuardConfigPtr->BgpdtHash[0]);
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_1, BiosGuardConfigPtr->BgpdtHash[1]);
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_2, BiosGuardConfigPtr->BgpdtHash[2]);
      AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_HASH_3, BiosGuardConfigPtr->BgpdtHash[3]);
      MsrValue |= (UINT64) (B_MSR_PLAT_FRMW_PROT_CTRL_LK | B_MSR_PLAT_FRMW_PROT_CTRL_EN);

      //
      // Check to see if we need to set the Dual socket bit
      //
      if (host->var.common.numCpus > 1)
        MsrValue |= B_MSR_PLAT_FRMW_PROT_CTRL_S1;

      MsrReg.hi = 0;
      MsrReg.lo = (UINT32)MsrValue;

      for(SocketId = 0; SocketId < MAX_SOCKET; SocketId++) {
        if ((host->var.common.socketPresentBitMap & (1 << SocketId)) != 0)
          WriteMsrPipe(host, SocketId, MSR_PLAT_FRMW_PROT_CTRL, MsrReg);
      }
    }
    //
    // Create BIOS Guard HOB
    //
    if (!ResetRequired) {
      Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof (BIOSGUARD_HOB), (VOID **) &BiosGuardHobPtr);
      BiosGuardHobPtr->EfiHobGuidType.Name = gBiosGuardHobGuid;
      CopyMem (&BiosGuardHobPtr->Bgpdt, &BiosGuardConfigPtr->Bgpdt, BiosGuardConfigPtr->Bgpdt.BgpdtSize);
      CopyMem (&BiosGuardHobPtr->BgupHeader, &BiosGuardConfigPtr->BgupHeader, sizeof (BGUP_HEADER));
      CopyMem (&BiosGuardHobPtr->BiosGuardLog, &BiosGuardConfigPtr->BiosGuardLog, sizeof (BIOSGUARD_LOG));
      BiosGuardHobPtr->TotalFlashSize = (UINT16) RShiftU64(BiosGuardConfigPtr->TotalFlashSize, 10);
      BiosGuardHobPtr->BiosSize = (UINT16) RShiftU64(FLASH_SIZE, 10);
      BiosGuardHobPtr->BiosGuardModulePtr = BiosGuardConfigPtr->BiosGuardModulePtr;
    }
  } else {
    DEBUG ((EFI_D_INFO, "BIOS Guard Module is Disabled\n"));
    MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL);
    if (MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_LK) {
      if (MsrValue & B_MSR_PLAT_FRMW_PROT_CTRL_EN) {
        ///
        /// Reset required as the BIOS Guard CTRL MSR is locked and needs to be toggled
        ///
        ResetRequired = TRUE;
      }
    } else {
      MsrValue &= (UINT64) ~B_MSR_PLAT_FRMW_PROT_CTRL_EN;
      MsrValue |= B_MSR_PLAT_FRMW_PROT_CTRL_LK;

      MsrReg.hi = 0;
      MsrReg.lo = (UINT32)MsrValue;

      for(SocketId = 0; SocketId < MAX_SOCKET; SocketId++) {
        if ((host->var.common.socketPresentBitMap & (1 << SocketId)) != 0)
          WriteMsrPipe(host, SocketId, MSR_PLAT_FRMW_PROT_CTRL, MsrReg);
      }
    }
  }

  if (ResetRequired) {

    Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gEfiPeiResetPpiGuid,
                    0,
                    NULL,
                    &ResetPpi
                    );

    ///
    /// Perform Cold Reset
    ///
    DEBUG ((EFI_D_INFO, "Reset Required. Performing Cold Reset to unlock BIOS Guard CONTROL MSR\n"));
    ResetPpi->ResetSystem (PeiServices);
  }

  return;
}
