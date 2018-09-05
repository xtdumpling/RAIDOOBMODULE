/** @file
  Initializes the PCH HD Audio Controller and Codec.

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
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

@par Specification Reference:
**/
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>
#include <Library/MmPciBaseLib.h>

#include <Ppi/PchHdaVerbTable.h>
#include <Ppi/PchPolicy.h>
// For SA iDisplay Audio link configuration
#include <SaRegs.h>

///
/// HD-Audio codec verb table prototope
///
typedef struct  {
  PCH_HDA_VERB_TABLE_HEADER  Header;  ///< HDA codec verb table header.
  UINT32 Data[1];                     ///< Verb table data (variable size),
                                      ///< length must be specified in Header.DataDwords.
} PCH_HDA_VERB_TABLE_PPI;

#define HDA_MAX_LOOP_TIME            200
#define HDA_WAIT_PERIOD              5

#define HDA_MAX_SDI_NUMBER_SKL_PCH   3
#define HDA_MAX_SDI_MASK_SKL_PCH     ((1 << HDA_MAX_SDI_NUMBER_SKL_PCH) - 1)
#define HDA_SDI_0_HDALINK            0
#define HDA_SDI_1_HDALINK            1
#define HDA_SDI_2_IDISPLINK          2

/**
  Polling the Status bit

  @param[in] StatusReg            The register address to read the status
  @param[in] PollingBitMap        The bit mapping for polling
  @param[in] PollingData          The Data for polling

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_TIMEOUT             Polling the bit map time out
**/
EFI_STATUS
StatusPolling (
  IN      UINT32          StatusReg,
  IN      UINT32          PollingBitMap,
  IN      UINT32          PollingData
  )
{
  UINT32  LoopTime;

  for (LoopTime = 0; LoopTime < HDA_MAX_LOOP_TIME; LoopTime++) {
    if ((MmioRead32 (StatusReg) & PollingBitMap) == PollingData) {
      break;
    } else {
      MicroSecondDelay (HDA_WAIT_PERIOD);
    }
  }

  if (LoopTime >= HDA_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  Send the command to the codec via the Immediate Command mechanism is written
  to the IC register

  @param[in] HdaBar                     Base address of Intel HD Audio memory mapped configuration registers
  @param[in, out] CodecCommandData      The Codec Command to be sent to the codec
  @param[in] ReadBack                   Whether to get the response received from the codec

  @retval EFI_DEVICE_ERROR              Device status error, operation failed
  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
SendCodecCommand (
  IN      UINT32          HdaBar,
  IN OUT  UINT32          *CodecCommandData,
  IN      BOOLEAN         ReadBack
  )
{
  EFI_STATUS  Status;

  Status = StatusPolling (HdaBar + R_PCH_HDABA_ICS, (UINT16) B_PCH_HDABA_ICS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ICB bit is not zero before SendCodecCommand! \n"));
    return EFI_DEVICE_ERROR;
  }

  MmioWrite32 (HdaBar + R_PCH_HDABA_IC, *CodecCommandData);
  MmioOr16 ((UINTN) (HdaBar + R_PCH_HDABA_ICS), (UINT16) ((B_PCH_HDABA_ICS_IRV | B_PCH_HDABA_ICS_ICB)));

  Status = StatusPolling (HdaBar + R_PCH_HDABA_ICS, (UINT16) B_PCH_HDABA_ICS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    MmioAnd16 ((UINTN) (HdaBar + R_PCH_HDABA_ICS), (UINT16)~(B_PCH_HDABA_ICS_ICB));
    return Status;
  }

  if (ReadBack == TRUE) {
    if ((MmioRead16 (HdaBar + R_PCH_HDABA_ICS) & B_PCH_HDABA_ICS_IRV) != 0) {
      *CodecCommandData = MmioRead32 (HdaBar + R_PCH_HDABA_IR);
    } else {
      DEBUG ((DEBUG_ERROR, "SendCodecCommand: ReadBack fail! \n"));
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

/**
  Checks if connected codec supports statically switchable BCLK clock frequency. 

  @param[in] HdaBar                     Base address of Intel HD Audio memory mapped configuration registers
  @param[in] CodecSDINo                 SDI number to which codec is connected
  @param[out] BclkFrequencySupportMask  Bit mask of supported frequencies

  @retval EFI_NO_RESPONSE               Verb response has not been received from codec
  @retval EFI_UNSUPPORTED               Codec does not support frequency switching
  @retval EFI_SUCCESS                   Codec supports frequency switching, the function completed successfully
**/
EFI_STATUS
GetCodecFrequencySwitchingSupport (
  IN      UINT32                             HdaBar,
  IN      UINT8                              CodecSDINo,
  OUT     UINT32                             *BclkFrequencySupportMask
  )
{
  UINT32          BclkFreqSupport;
  EFI_STATUS      Status;

  *BclkFrequencySupportMask = 0;

  ///
  /// Read Statically Switchable BCLK Frequency capabilities from the attached codec (VerbId = F00h, ParameterId = 16h)
  ///  
  BclkFreqSupport = 0x000F0016 | (CodecSDINo << 28);
  Status   = SendCodecCommand (HdaBar, &BclkFreqSupport, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error: Reading the Codec BCLK Frequency Switching support fail!\n"));
    return EFI_NO_RESPONSE;

  }
  DEBUG ((DEBUG_INFO, "SDI:%d Verb F00h ParamtererId = 16h response = 0x%08x\n", CodecSDINo, BclkFreqSupport));

  ///
  /// If the codec returns value of all 0s, it implies that it does not support static frequency switching.
  /// HDAudio-Link (SDIN 0/1) : BIT2 (24MHz) must be always set if supported
  /// iDisp-Link:  (SDIN 2)   : BIT4 (96MHz) must be always set if supported
  ///
  if (BclkFreqSupport == 0 ||
      ((CodecSDINo == HDA_SDI_0_HDALINK || CodecSDINo == HDA_SDI_1_HDALINK) && ((BclkFreqSupport & BIT2) == 0)) ||
      ((CodecSDINo == HDA_SDI_2_IDISPLINK) && ((BclkFreqSupport & BIT4) == 0)))
  {
    DEBUG ((DEBUG_INFO, "HDA: Codec Static Frequency Switching not supported!\n"));
    return EFI_UNSUPPORTED;
  }

  *BclkFrequencySupportMask = (BclkFreqSupport & 0x1F); // 1Fh = BITS[4:0] - 96/48/24/12/6 MHz
  DEBUG ((DEBUG_INFO, "HDA: Codec Static Frequency Switching supported - FrequencySupportMask = 0x%08x\n", *BclkFrequencySupportMask));
  return EFI_SUCCESS;
}

/**
  Get codec's current BCLK clock frequency.

  @param[in] HdaBar                     Base address of Intel HD Audio memory mapped configuration registers
  @param[in] CodecSDINo                 SDI number to which codec is connected
  @param[out] CurrentBclkFrequency      Current BCLK frequency of codec

  @retval EFI_NO_RESPONSE               Verb response has not been received from codec
  @retval EFI_NOT_FOUND                 Could not match current codec frequency with any supported frequency
  @retval EFI_SUCCESS                   Supported frequency found, the function completed successfully
**/
EFI_STATUS
GetCodecCurrentBclkFrequency (
  IN      UINT32                              HdaBar,
  IN      UINT8                               CodecSDINo,
  OUT     PCH_HDAUDIO_LINK_FREQUENCY          *CurrentBclkFrequency
  )
{
  UINT32          BclkFreq;
  UINT32          FreqIndex;
  EFI_STATUS      Status;

  *CurrentBclkFrequency = PchHdaLinkFreqInvalid;
  ///
  /// Read BCLK Freq from the attached codec
  ///
  BclkFreq = 0x000F3700 | (CodecSDINo << 28);
  Status   = SendCodecCommand (HdaBar, &BclkFreq, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error: Reading the Codec current BCLK Frequency fail!\n"));
    return EFI_NO_RESPONSE;
  }
  DEBUG ((DEBUG_INFO, "SDI:%d Verb F37h (GET_CCF) response = 0x%08x\n", CodecSDINo, BclkFreq));
  
  for (FreqIndex = 0, Status = EFI_NOT_FOUND; FreqIndex < PchHdaLinkFreqInvalid; FreqIndex++) {
    if ((BclkFreq >> FreqIndex) & BIT0) {
      *CurrentBclkFrequency = FreqIndex;
      DEBUG ((DEBUG_INFO, "SDI:%d Codec BCLK frequency = %d\n", CodecSDINo, *CurrentBclkFrequency)); 
      Status = EFI_SUCCESS;
      break;
    }
  }

  DEBUG ((DEBUG_INFO, "GetCodecCurrentBclkFrequency() Status = %r\n", Status));
  return Status;
}

/**
  Sets link clock frequency.
  Controller must be out of reset to set Output/Input payload registers.
  Link must be turned off before setting Link Control register, then turned on again.

  @param[in] HdaPciBase                 PCI Configuration Space Base Address
  @param[in] HdaBar                     Base address of Intel HD Audio memory mapped configuration registers
  @param[in] LinkTypeIndex              Link index: 0 = HDA link, 1 = iDisp link
  @param[in] RequestedBclkFrequency     Clock frequency to be set

  @retval EFI_NOT_READY                 Link reset cannot be asserted
  @retval EFI_DEVICE_ERROR              iDisplay link frequency switching failed
  @retval EFI_INVALID_PARAMETER         Incorrect link index given
  @retval EFI_SUCCESS                   Requested frequency set, the function completed successfully
**/
EFI_STATUS
SetCodecBclkFrequency (
  IN      UINTN                               HdaPciBase,
  IN      UINT32                              HdaBar,
  IN      UINT8                               LinkTypeIndex,
  IN      PCH_HDAUDIO_LINK_FREQUENCY          RequestedBclkFrequency,
  IN      PCH_HDAUDIO_IDISP_TMODE             RequestedIDispTmode
  )
{
  UINT16      OutputPayloadWords;
  UINT16      InputPayloadWords;
  UINT32      Data32;
  EFI_STATUS  Status;

  // Assert link RESET# before frenquency switching
  MmioAnd32 ((UINTN)(HdaBar + R_PCH_HDABA_LCTLX(LinkTypeIndex)), (UINT32)~(B_PCH_HDABA_LCTLX_SPA));
  Status = StatusPolling (HdaBar + R_PCH_HDABA_LCTLX(LinkTypeIndex), (UINT32) B_PCH_HDABA_LCTLX_CPA, (UINT32) 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HDA: Turn off the link (SPA = 0) failed! CPA polling Status = %r\n", Status));
    return EFI_NOT_READY;
  }

  if (LinkTypeIndex == V_PCH_HDA_HDALINK_INDEX) {
     DEBUG ((DEBUG_INFO, "HDA: HDA-Link Static BCLK Frequency Switch ID = %d - ", RequestedBclkFrequency));

     // For HDA-Link change OUTPAY and INPAY
    switch (RequestedBclkFrequency) {
      case PchHdaLinkFreq24MHz:
        DEBUG ((DEBUG_INFO, "24MHz\n"));
        // Output: 24MHz -> 1000 bits/frame - 40 bits (command & control) = 960 bits -> 60 words of payload
        // Input : 24MHz -> 500 bits/frame - 36 bits (response) = 464 bits -> 29 words of payload (limit to 28)
        OutputPayloadWords = 0x3C; // 60 words of payload
        InputPayloadWords  = V_PCH_HDABA_INPAY_DEFAULT; // 28 words of payload
        break;
      case PchHdaLinkFreq12MHz:
        DEBUG ((DEBUG_INFO, "12MHz\n"));
        // Output: 12MHz -> 500 bits/frame - 40 bits (command & control) = 460 bits -> 28 words of payload
        // Input : 12MHz -> 250 bits/frame - 36 bits (response) = 214 bits -> 13 words of payload
        OutputPayloadWords = 0x1C; // 28 words of payload
        InputPayloadWords  = 0x0D; // 13 words of payload
        break;
      case PchHdaLinkFreq6MHz:
        DEBUG ((DEBUG_INFO, "6MHz\n"));
        // Output: 6MHz -> 250 bits/frame - 40 bits (command & control) = 210 bits -> 13 words of payload
        // Input : 6MHz -> 125 bits/frame - 36 bits (response) = 89 bits -> 5 words of payload
        OutputPayloadWords = 0x0D; // 13 words of payload
        InputPayloadWords  = 0x05; // 5 words of payload
        break;
      default:
        DEBUG ((DEBUG_INFO, "Default 24MHz\n"));
        // Default for 24MHz
        OutputPayloadWords = 0x3C; // 60 words of payload
        InputPayloadWords  = V_PCH_HDABA_INPAY_DEFAULT; // 28 words of payload
        break;
    }

    // Controller must be out of reset (CRSTB = 1) to set the following registers
    MmioWrite16 ((UINTN) (HdaBar + R_PCH_HDABA_OUTPAY), OutputPayloadWords);
    MmioWrite16 ((UINTN) (HdaBar + R_PCH_HDABA_INPAY), InputPayloadWords);

    Data32 = MmioRead16 ((UINTN) (HdaBar + R_PCH_HDABA_OUTPAY));
    DEBUG ((DEBUG_INFO, "HDA: OUTPAY (Output Payload Capability) = 0x%04x\n", Data32));
    Data32 = MmioRead16 ((UINTN) (HdaBar + R_PCH_HDABA_INPAY));
    DEBUG ((DEBUG_INFO, "HDA: INPAY (Input Payload Capability) = 0x%04x\n", Data32));

  } else {
    DEBUG ((DEBUG_ERROR, "HDA: Incorrect Link Type!\n"));
    return EFI_INVALID_PARAMETER;
  }

  MmioAndThenOr32 ((UINTN)(HdaBar + R_PCH_HDABA_LCTLX(LinkTypeIndex)),
                      (UINT32)~(BIT3 | BIT2 | BIT1 | BIT0),
                      (UINT32)(RequestedBclkFrequency << N_PCH_HDABA_LCTLX_SCF));

  Data32 = MmioRead32 ((UINTN)(HdaBar + R_PCH_HDABA_LCTLX(LinkTypeIndex)));
  DEBUG ((DEBUG_INFO, "HDA: LCTLx (Link %d Control) = 0x%08x\n", LinkTypeIndex, Data32));

  // De-assert link RESET# after frenquency switching
  MmioOr32 ((UINTN)(HdaBar + R_PCH_HDABA_LCTLX(LinkTypeIndex)), (UINT32) B_PCH_HDABA_LCTLX_SPA);
  Status = StatusPolling (HdaBar + R_PCH_HDABA_LCTLX(LinkTypeIndex), (UINT32) B_PCH_HDABA_LCTLX_CPA, (UINT32) B_PCH_HDABA_LCTLX_CPA);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HDA: Turn on the link (SPA = 1) failed! CPA polling Status = %r\n", Status));
  }

  return EFI_SUCCESS;
}

/**
  Configures static link frequency for HD-Audio link and iDisplay link.

  @param[in] HdaConfig                  HD-A Configuration
  @param[in] HdaPciBase                 PCI Configuration Space Base Address
  @param[in] HdaBar                     Base address of Intel HD Audio memory mapped configuration registers
  @param[in] CodecSDINo                 SDI number to which codec is connected

  @retval EFI_NOT_STARTED               iDisplay link frequency switch not supported (iGfx disabled)
  @retval EFI_INVALID_PARAMETER         Incorrect SDI number given
  @retval EFI_UNSUPPORTED               Codec does not support frequency switching
  @retval EFI_NOT_FOUND                 Codec current frequency does not match with any supported frequency
  @retval EFI_DEVICE_ERROR              Codec supports requested frequency, but the switching failed (default frequency set)
  @retval EFI_SUCCESS                   Requested frequency successfully set (or correct frequency is already configured)
**/
EFI_STATUS
ConfigureLinkFrequency (
  IN      PCH_HDAUDIO_CONFIG                  *HdaConfig,
  IN      UINTN                               HdaPciBase,
  IN      UINT32                              HdaBar,
  IN      UINT8                               CodecSDINo
  )
{
  PCH_HDAUDIO_LINK_FREQUENCY RequestedFreq;
  PCH_HDAUDIO_LINK_FREQUENCY CurrentFreq;
  UINT32                     FreqSupportMask;
  UINT8                      LinkTypeIndex;
  EFI_STATUS                 Status;

  DEBUG ((DEBUG_INFO, "ConfigureLinkFrequency() Start [SDI:%d]\n", CodecSDINo));

  switch (CodecSDINo) {
    case HDA_SDI_0_HDALINK:
    case HDA_SDI_1_HDALINK:
      // HDAudio link frequency change
      DEBUG ((DEBUG_INFO, "HDA: HDAudio-Link frequency configuration\n"));
      LinkTypeIndex = V_PCH_HDA_HDALINK_INDEX;
      RequestedFreq = HdaConfig->HdAudioLinkFrequency;
      // Program INPAY register with value 1Ch instead of the default value of 1Dh (controller must be out of reset, CRSTB = 1)
      MmioWrite16 ((UINTN) (HdaBar + R_PCH_HDABA_INPAY), V_PCH_HDABA_INPAY_DEFAULT);
      break;
    case HDA_SDI_2_IDISPLINK:
      DEBUG ((DEBUG_ERROR, "HDA: Invalid SDI number %d\n", CodecSDINo));
	  ASSERT (FALSE);
    default:
      DEBUG ((DEBUG_ERROR, "HDA: Invalid SDI number for frequency configuration!\n"));
      return EFI_INVALID_PARAMETER;      
  }

  Status = GetCodecFrequencySwitchingSupport (HdaBar, CodecSDINo, &FreqSupportMask);
  if (EFI_ERROR (Status)){
    return Status;
  }

  Status = GetCodecCurrentBclkFrequency (HdaBar, CodecSDINo, &CurrentFreq);
  if (EFI_ERROR (Status)){
    return Status;
  }

  if (CurrentFreq == RequestedFreq) {
    DEBUG ((DEBUG_INFO, "HDA: Requested BCLK clock frequency matches current frequency (ID = %d) - Exit\n", CurrentFreq));
    return EFI_SUCCESS;
  }

  if ((FreqSupportMask >> RequestedFreq) & BIT0) {
    DEBUG ((DEBUG_INFO, "HDA: Requested Frequency ID = %d - Supported!\n", RequestedFreq));
    Status = SetCodecBclkFrequency (HdaPciBase, HdaBar, LinkTypeIndex, RequestedFreq, HdaConfig->IDispLinkTmode);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;      
    }

    // Delay needed after switching frequency (link reset), before sending verb to codec.
    MicroSecondDelay (HdaConfig->ResetWaitTimer);

    Status = GetCodecCurrentBclkFrequency (HdaBar, CodecSDINo, &CurrentFreq);
    if ((Status == EFI_NOT_FOUND) || (CurrentFreq != RequestedFreq)) {
      DEBUG ((DEBUG_ERROR, "HDA: Frequency switching failed - return to default!\n"));
      RequestedFreq = (LinkTypeIndex == V_PCH_HDA_IDISPLINK_INDEX) ? PchHdaLinkFreq96MHz : PchHdaLinkFreq24MHz;
      SetCodecBclkFrequency (HdaPciBase, HdaBar, LinkTypeIndex, RequestedFreq, PchHdaIDispMode2T);
      return EFI_DEVICE_ERROR;
    }   
  }

  DEBUG ((DEBUG_INFO, "ConfigureLinkFrequency() End\n"));
  return EFI_SUCCESS;
}

/**
  Locates HD Audio codec verb table.

  @param[in] VendorDeviceId   Codec vendor ID  and device ID
  @param[in] RevisionId       Codec revision ID
  @param[in] SdiNo            SDI channel number

  @return PCH_HDA_VERB_TABLE_PPI pointer
  @retval NULL                Table not found.
**/
PCH_HDA_VERB_TABLE_PPI*
LocateVerbTable (
  IN      UINT32                  VendorDeviceId,
  IN      UINT8                   RevisionId,
  IN      UINT8                   SdiNo
  )
{
  PCH_HDA_VERB_TABLE_PPI  *VerbTable;
  EFI_STATUS                 Status;
  UINTN                      Num;

  ASSERT (VendorDeviceId != 0);

  VerbTable = NULL;
  Status = EFI_SUCCESS;

  for (Num = 0; Status == EFI_SUCCESS; ++Num) {
    VerbTable = NULL;
    Status = PeiServicesLocatePpi (
               &gPchHdaVerbTablePpiGuid,
               Num,
               NULL,
               (VOID **)&VerbTable
               );
    if ((Status == EFI_SUCCESS) &&
        (VerbTable != NULL) &&
        ((((UINT32) (VerbTable->Header.VendorId) << 16) | VerbTable->Header.DeviceId) == VendorDeviceId) &&
        ((VerbTable->Header.RevisionId == 0xFF) || (VerbTable->Header.RevisionId == RevisionId)) &&
        ((VerbTable->Header.SdiNo == 0xFF) || VerbTable->Header.SdiNo == SdiNo))
    {
      break;
    }
  }
  return VerbTable;
}

/**
  Initialize the Intel High Definition Audio Codec(s) present in the system.
  For each codec, a predefined codec verb table should be programmed.
  The list contains 32-bit verbs to be sent to the corresponding codec.
  If it is not programmed, the codec uses the default verb table, which may or may not
  correspond to the platform jack information.

  @param[in] HdaConfig            HD-A Configuration
  @param[in] HdaPciBase           PCI Configuration Space Base Address
  @param[in] HdaBar               Memory Space Base Address

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Provided VerbTableData is null
**/
EFI_STATUS
DetectAndInitializeCodec (
  IN      PCH_HDAUDIO_CONFIG                  *HdaConfig,
  IN      UINTN                               HdaPciBase,
  IN      UINT32                              HdaBar
  )
{
  EFI_STATUS                    Status;
  UINT32                        Index;
  UINT32                        VendorDeviceId;
  UINT32                        RevisionId;
  UINT16                        Data16;
  UINT16                        BitMask;
  UINT16                        BitValue;
  UINT32                        CodecCmdData;
  UINT32                        LoopTime;
  PCH_HDA_VERB_TABLE_PPI        *VerbTable;
  UINT8                         ByteReg;
  UINT8                         AzaliaSDINo;

  DEBUG ((DEBUG_INFO, "DetectAndInitializeCodec() Start\n"));
  
  ///
  /// PCH BIOS Spec Section 9.1.3 Codec Initialization Programming Sequence
  /// System BIOS should also ensure that the Controller Reset# bit of Global Control register
  /// in memory-mapped space (HDBAR+08h[0]) is set to 1 and read back as 1.
  /// Deassert the HDA controller RESET# to start up the link
  ///
  MmioOr32 ((UINTN) (HdaBar + R_PCH_HDABA_GCTL), (UINT32) B_PCH_HDABA_GCTL_CRST);

  BitMask = (UINT16) B_PCH_HDABA_GCTL_CRST;
  BitValue = (UINT16) B_PCH_HDABA_GCTL_CRST;
  Status = StatusPolling (HdaBar + R_PCH_HDABA_GCTL, BitMask, BitValue);
  ///
  /// PCH BIOS Spec Section 9.1.3 Codec Initialization Programming Sequence
  /// Read GCAP and write the same value back to the register once after Controller Reset# bit is set
  ///
  Data16  = MmioRead16 (HdaBar + R_PCH_HDABA_GCAP);
  MmioWrite16 (HdaBar + R_PCH_HDABA_GCAP, Data16);

  ///
  /// Clear the "State Change Status Register" STATESTS bits for
  /// each of the "SDIN Stat Change Status Flag"
  ///
  Data16 = HDA_MAX_SDI_MASK_SKL_PCH;
  MmioOr8 ((UINTN) (HdaBar + R_PCH_HDABA_WAKESTS), (UINT8) (Data16));

  ///
  /// Turn off the link and poll RESET# bit until it reads back as 0 to get hardware reset report
  ///
  MmioAnd32 ((UINTN) (HdaBar + R_PCH_HDABA_GCTL), (UINT32)~(B_PCH_HDABA_GCTL_CRST));

  BitMask = (UINT16) B_PCH_HDABA_GCTL_CRST;
  BitValue = 0;
  Status = StatusPolling (HdaBar + R_PCH_HDABA_GCTL, BitMask, BitValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Reset High Definition Audio (CRST = 0) Time Out - 1!\n"));
    goto ExitInitCodec;
  }

  ///
  /// Turn on the link and poll RESET# bit until it reads back as 1
  ///
  MmioOr32 ((UINTN) (HdaBar + R_PCH_HDABA_GCTL), (UINT32) B_PCH_HDABA_GCTL_CRST);

  ///
  /// For some combo card that will need this delay because each codec has different latency to come out from RESET.
  /// This delay can make sure all codecs be recognized by BIOS after RESET sequence.
  /// Additional delay might be required to allow codec coming out of reset prior to subsequent operations,
  /// please contact your codec vendor for detail. When clearing this bit and setting it afterward,
  /// BIOS must ensure that minimum link timing requirements (minimum RESET# assertion time, etc.) are met..
  ///
  MicroSecondDelay (HdaConfig->ResetWaitTimer);

  BitMask = (UINT16) B_PCH_HDABA_GCTL_CRST;
  BitValue = (UINT16) B_PCH_HDABA_GCTL_CRST;
  Status = StatusPolling (HdaBar + R_PCH_HDABA_GCTL, BitMask, BitValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UnReset High Definition Audio (CRST = 1) Time Out - 2!\n"));
    goto ExitInitCodec;
  }

  ///
  /// Read the "State Change Status Register" STATESTS bits twice to find out if any SDIN
  /// is connected to a codec.
  ///
  Data16 = HDA_MAX_SDI_MASK_SKL_PCH;

  for (LoopTime = 0, ByteReg = 0, AzaliaSDINo = 0; LoopTime < HDA_MAX_LOOP_TIME; LoopTime++) {
    ByteReg = (UINT8)(MmioRead8 (HdaBar + R_PCH_HDABA_WAKESTS) & Data16);
    if (ByteReg != 0 && (ByteReg == AzaliaSDINo)) {
      break;
    } else {
      AzaliaSDINo = ByteReg;
    }
    MicroSecondDelay (HDA_WAIT_PERIOD);
  }
  ///
  /// BIT3(1000) -- SDI3
  /// BIT2(0100) -- SDI2
  /// BIT1(0010) -- SDI1
  /// BIT0(0001) -- SDI0
  ///
  if (ByteReg == 0) {
    ///
    /// No codec detected - turn off the link
    ///
    DEBUG ((DEBUG_ERROR, "HD-Audio Codec not detected (SDIN not connected to a codec)\n"));
    MmioAnd32 ((UINTN) (HdaBar + R_PCH_HDABA_GCTL), (UINT32)~(B_PCH_HDABA_GCTL_CRST));
    Status = EFI_DEVICE_ERROR;
    goto ExitInitCodec;
  }

  ///
  /// PME Enable for Audio controller, this bit is in the resume well
  ///
  if (HdaConfig->Pme == TRUE) {
    MmioOr32 ((UINTN) (HdaPciBase + R_PCH_HDA_PCS), (UINT32) B_PCH_HDA_PCS_PMEE);
  }

  Data16 = HDA_MAX_SDI_NUMBER_SKL_PCH;
  for (AzaliaSDINo = 0; AzaliaSDINo < Data16; AzaliaSDINo++, ByteReg >>= 1) {
    if ((ByteReg & 0x1) == 0) {
      ///
      /// SDIx has no HD-Audio device
      ///
      DEBUG ((DEBUG_ERROR, "SDI%d has no HD-Audio device.\n", AzaliaSDINo));
      continue;
    }

    ///
    /// PME Enable for each existing codec, these bits are in the resume well
    ///
    if (HdaConfig->Pme != FALSE) {
      MmioOr16 (
        (UINTN) (HdaBar + R_PCH_HDABA_WAKEEN),
        (UINT16) ((B_PCH_HDABA_WAKEEN_SDI_0 << AzaliaSDINo))
        );
    }

    ///
    /// Verb:  31~28   27  26~20                   19~0
    ///         CAd    1    NID   Verb Command and data
    ///       0/1/2
    ///
    /// Read the Vendor ID/Device ID pair from the attached codec (ParameterId = 0x00)
    ///
    VendorDeviceId  = 0x000F0000 | (AzaliaSDINo << 28);
    Status          = SendCodecCommand (HdaBar, &VendorDeviceId, TRUE);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Error: Reading the Codec Vendor ID/Device ID fail!\n"));
      goto ExitInitCodec;
    }
    ///
    /// Read the Revision ID from the attached codec (ParameterId = 0x02)
    ///
    RevisionId  = 0x000F0002 | (AzaliaSDINo << 28);
    Status      = SendCodecCommand (HdaBar, &RevisionId, TRUE);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Error: Reading the Codec Revision ID fail!\n"));
      goto ExitInitCodec;
    }

    RevisionId = (RevisionId >> 8) & 0xFF;

    DEBUG ((DEBUG_INFO, "SDI:%d Detected HD-Audio Codec 0x%08X rev 0x%02X\n", AzaliaSDINo, VendorDeviceId, RevisionId));

    ///
    /// Link static frequency switching
    ///
    Status = ConfigureLinkFrequency (HdaConfig, HdaPciBase, HdaBar, AzaliaSDINo);
    DEBUG ((DEBUG_INFO, "ConfigureLinkFrequency() Exit, Status = %r\n", Status));

    ///
    /// Locate Verb Table and initialize detected codec
    ///
    VerbTable = LocateVerbTable (VendorDeviceId, (UINT8) RevisionId, AzaliaSDINo);

    if (VerbTable == NULL) {
      DEBUG ((DEBUG_ERROR, "Error: No matching HD-Audio codec verb table found for codec (0x%08X).\n", VendorDeviceId));
      continue;
    }

    DEBUG ((DEBUG_INFO, 
        "Found verb table for vendor 0x%04X devId 0x%04X "
        "rev 0x%02X (SDI:%X, size: %d dwords)\n", 
        VerbTable->Header.VendorId,
        VerbTable->Header.DeviceId,
        VerbTable->Header.RevisionId,
        VerbTable->Header.SdiNo,
        VerbTable->Header.DataDwords));

    ///
    /// Send the entire list of verbs in the matching verb table one by one to the codec
    ///
    DEBUG ((DEBUG_VERBOSE, "HDA: Sending verbs to codec:\n"));
    for (Index = 0; Index < VerbTable->Header.DataDwords; ++Index) {
      CodecCmdData  = VerbTable->Data[Index];
      ASSERT ((CodecCmdData >> 28) == 0);
      ///
      /// Program CAd Field per the SDI number got during codec detection
      ///
      CodecCmdData  |= (UINT32) (AzaliaSDINo << 28);
      Status = SendCodecCommand (HdaBar, &CodecCmdData, FALSE);
      if (EFI_ERROR (Status)) {
        ///
        /// Skip the Azalia verb table loading when find the verb table content is not
        /// properly matched with the HDA hardware, though IDs match.
        ///
        DEBUG ((DEBUG_ERROR,"Error loading verb table for Azalia Codec of 0x%08X", VendorDeviceId));
        break;
      }

      DEBUG ((DEBUG_VERBOSE, "[%03d] 0x%08X\n", Index, CodecCmdData));
    }
  }

  Status = EFI_SUCCESS;

ExitInitCodec:
  DEBUG ((DEBUG_INFO, "DetectAndInitializeCodec() Exit, Status = %r\n", Status));
  return Status;
}

/**
  Disables Intel High Definition Audio Subsystem
  (inclusive of Intel Smart Sound Technology)

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   PCR transaction error
**/
EFI_STATUS
DisableHdAudio (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT32                        PchPwrmBase;

  DEBUG ((DEBUG_INFO, "DisableHdAudio() Start\n"));

  ///
  /// Set PWRMBASE + 628h [23] = 1
  ///
  PchPwrmBaseGet (&PchPwrmBase);
  MmioOr32 (
    (UINTN)(PchPwrmBase + R_PCH_PWRM_NST_PG_FDIS_1),
    (UINT32)B_PCH_PWRM_NST_PG_FDIS_1_ADSP_FDIS_PMC
    );

  ///
  /// Set PCR[HDA] + 530h [5,4,3,2,0] = 0b,1b,0b,1b,1b
  /// (Power Gating Enabled, BIOS Configuration Lock Down, Clock Gating Enabled, ADSP Disabled, HDA Disabled)
  ///
  Status = PchPcrAndThenOr32 (PID_DSP,
                              R_PCH_PCR_HDA_FNCFG,
                              (UINT32)~(B_PCH_PCR_HDA_FNCFG_CGD),
                              (UINT32)(B_PCH_PCR_HDA_FNCFG_BCLD | B_PCH_PCR_HDA_FNCFG_ADSPD | B_PCH_PCR_HDA_FNCFG_HDASD));
  if(EFI_ERROR(Status)) {
    return Status;
  }
  Status = PchPcrAndThenOr32 (PID_PSF3,
                              R_PCH_PSF3_T0_SHDW_AUD_RS1_D24_F0_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
                              (UINT32) ~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_IOEN,
                              (UINT32) B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
                             );
  if(EFI_ERROR(Status)) {
    return Status;
  }
  ///
  /// Set PCR[PSF3] + PSF_3_AGNT_T0_SHDW_PCIEN_AUD_RS0_D31_F3[8] to 1
  ///
  Status = PchPcrAndThenOr32 (PID_PSF3,
                              R_PCH_PCR_PSF3_T0_SHDW_AUD_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
                              (UINT32)~0,
                              (UINT32)B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
  
  DEBUG ((DEBUG_INFO, "DisableHdAudio() End - Status = %r\n", Status));
  return Status;
}

/**
  Configures Audio PLL settings.
  Workaround for SKL PCH:
  The Audio PLL configuration setting is not at the optimal value by default.
  BIOS is required to program with the expected setting before controller is out of reset.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   PCR transaction error
**/
EFI_STATUS
ConfigureAudioPll (
  VOID
  )
{
  EFI_STATUS                    Status;
  DEBUG ((DEBUG_INFO, "ConfigureAudioPll() Start\n"));
  
  ///
  /// Set APLLP0.KPKISC, PCR[HDA] + 610h [31:30] = 11b
  /// Set APLLP0.KIS,    PCR[HDA] + 610h [29:28] = 11b
  /// Set APLLP0.KPS,    PCR[HDA] + 610h [27:26] = 11b
  /// Set APLLP0.PLLWC,  PCR[HDA] + 610h [22:16] = 1Eh
  ///
  Status = PchPcrAndThenOr32 (PID_DSP, R_PCH_PCR_HDA_APLLP0, (UINT32)~(0xFC7F0000), (UINT32)V_PCH_PCR_HDA_APLLP0);

  ///
  /// Set APLLP1.DCOTC, PCR[HDA] + 614h [13:8] = 3Fh
  ///
  Status = PchPcrAndThenOr32 (PID_DSP, R_PCH_PCR_HDA_APLLP1, (UINT32)~(0x00003F00), (UINT32)V_PCH_PCR_HDA_APLLP1);

  ///
  /// Set APLLP2.KPCE, PCR[HDA] + 618h [11:8] = 1h
  /// Set APLLP2.KICE, PCR[HDA] + 618h [4:0] = 1Dh
  ///
  Status = PchPcrAndThenOr32 (PID_DSP, R_PCH_PCR_HDA_APLLP2, (UINT32)~(0x00000F1F), (UINT32)V_PCH_PCR_HDA_APLLP2);

  DEBUG ((DEBUG_INFO, "ConfigureAudioPll() End - Status = %r\n", Status));
  return Status;
}

/**
  Initialize the Intel High Definition Audio Codec(s) present in the system.
  For each codec, a predefined codec verb table should be programmed.
  The list contains 32-bit verbs to be sent to the corresponding codec.
  If it is not programmed, the codec uses the default verb table, which may or may not
  correspond to the platform jack information.

  @param[in] HdaConfig            HD-A Configuration
  @param[in] HdaPciBase           PCI Configuration Space Base Address
  @param[in] HdaBar               Memory Space Base Address

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Provided VerbTableData is null
**/
EFI_STATUS
ConfigureHdAudio (
  IN      PCH_HDAUDIO_CONFIG                  *HdaConfig,
  IN      UINTN                               HdaPciBase,
  IN      UINT32                              HdaBar
  )
{
  EFI_STATUS                    Status;
  UINT32                        Data32AndFncfg;
  UINT32                        Data32OrFncfg;
  PCH_HDAUDIO_LINK_FREQUENCY    HdaLinkFrequency;

  DEBUG ((DEBUG_INFO, "ConfigureHdAudio() Start\n"));
  Data32AndFncfg = (UINT32)~0;
  Data32OrFncfg  = 0;
  HdaLinkFrequency = PchHdaLinkFreqInvalid;

  MmioWrite32 (HdaPciBase + R_PCH_HDA_HDALBA, HdaBar);
  MmioWrite32 (HdaPciBase + R_PCH_HDA_HDAUBA, 0);
  MmioOr16 ((UINTN) (HdaPciBase + PCI_COMMAND_OFFSET), (UINT16) EFI_PCI_COMMAND_MEMORY_SPACE);

  DEBUG ((DEBUG_INFO, "HdaPciBase = 0x%X, HdaBar = 0x%X\n", HdaPciBase, HdaBar));

  if (HdaConfig->DspEnable == TRUE) {

    if (HdaConfig->DspUaaCompliance == FALSE) {
      DEBUG ((DEBUG_INFO, "HDA: Audio DSP Enabled, SST mode - set Sub Class Code to 0x%02x\n", V_PCH_HDA_SCC_ADSP));
      //
      // Set Sub Class Code to 01 for Audio DSP enabled (SST only support)
      //
      MmioWrite8 ((UINTN) (HdaPciBase + R_PCH_HDA_SCC), (UINT8)V_PCH_HDA_SCC_ADSP);
    } else {
      DEBUG ((DEBUG_INFO, "HDA: Audio DSP Enabled, UAA mode - set Programming Interface to 0x%02x\n", V_PCH_HDA_PI_ADSP_UAA));
      //
      // Set Programming Interface to 01 for Audio DSP enabled (HDA Inbox and SST support)
      //
      MmioWrite8 ((UINTN) (HdaPciBase + R_PCH_HDA_PI), (UINT8)V_PCH_HDA_PI_ADSP_UAA);
    }

    //
    // Enable Audio DSP for operation:
    // GPROCEN = 0, ADSPxBA (BAR2) is mapped to the Intel HD Audio memory mapped configuration registers (Default),
    // GPROCEN = 1, ADSPxBA (BAR2) is mapped to the actual Audio DSP memory mapped configuration registers.
    // REMARK: Probably this bit shall be set by the new (SKL PCH) HDA Driver, not BIOS
    //
    DEBUG ((DEBUG_INFO, "HDA: Set PPCTL.GPROCEN = 1 (Global Processing Enable - to enable ADSP for operation)\n"));
    MmioOr32 ((UINTN) (HdaBar + R_PCH_HDABA_PPCTL), (UINT32)B_PCH_HDABA_PPCTL_GPROCEN);
  } else {
    DEBUG ((DEBUG_INFO, "HDA: Audio DSP Disabled\n"));
    ///
    /// Set PCR[HDA] + 530h [2] = 1b.
    ///
    Data32OrFncfg |= B_PCH_PCR_HDA_FNCFG_ADSPD;
  }
  
  //
  // 1. PCI power management capability structure PC.PMES reporting PME support for D0, but HW does not generate PME in D0.
  // Program PC.PMES[15:11] = 18h before locking FNCFG.BCLD.
  //
  MmioAndThenOr16 ((UINTN) (HdaPciBase + R_PCH_HDA_PC), (UINT16)~(0xF800), (UINT16)(V_PCH_HDA_PC_PMES << N_PCH_HDA_PC_PMES));
  //
  // 2. The Audio PLL configuration setting is not at the optimal value by default
  // Program with the expected settings before controller is out of reset
  //
  Status = ConfigureAudioPll ();

  ///
  /// Disable Slow Ring oscillator clock gating
  /// Set CGCTL.SROTCGE to 0b
  ///
  MmioAnd32 ((UINTN) (HdaPciBase + R_PCH_HDA_CGCTL), (UINT32)~B_PCH_HDA_CGCTL_SROTCGE);

  ///
  /// Set EM1/SEM1 register (HDABAR + 1000h / HDA PCI Config Space + C0h):
  /// 1) Set LFLCS BIT[24] = 1b - use XTAL oscillator clock source for low frequencies link clock
  /// 2) Set FIFORDYSEL BIT[10:9] = 11b
  ///
  MmioAndThenOr32 ((UINTN) (HdaPciBase + R_PCH_HDA_SEM1),
                      (UINT32)~(B_PCH_HDA_SEM1_LFLCS | B_PCH_HDA_SEM1_FIFORDYSEL),
                      (UINT32)(B_PCH_HDA_SEM1_LFLCS | B_PCH_HDA_SEM1_FIFORDYSEL));

  ///
  /// Set Buffer Size Minimum Threshold to 128B
  /// Set EM2/SEM2.BSMT to 01b
  ///
  MmioAndThenOr32 ((UINTN) (HdaPciBase + R_PCH_HDA_SEM2),
                      (UINT32)~B_PCH_HDA_SEM2_BSMT,
                      (UINT32)(V_PCH_HDA_SEM2_BSMT << N_PCH_HDA_SEM2_BSMT));

  ///
  /// Set Input Stream L1 Exit Threshold 2 to 1/8 of FIFO size
  /// Set EM3L/SEM3L.ISL1EXT2 to 10b
  ///
  MmioAndThenOr32 ((UINTN) (HdaPciBase + R_PCH_HDA_SEM3L),
                      (UINT32)~B_PCH_HDA_SEM3L_ISL1EXT2,
                      (UINT32)(V_PCH_HDA_SEM3L_ISL1EXT2 << N_PCH_HDA_SEM3L_ISL1EXT2));
  ///
  /// Set Output Stream L1 Exit Threshold 2 to 1/4 of FIFO size    
  /// Set EM4L/SEM4L.OSL1EXT2 to 11b
  ///
  MmioAndThenOr32 ((UINTN) (HdaPciBase + R_PCH_HDA_SEM4L),
                      (UINT32)~B_PCH_HDA_SEM4L_OSL1EXT2,
                      (UINT32)(V_PCH_HDA_SEM4L_OSL1EXT2 << N_PCH_HDA_SEM4L_OSL1EXT2));

  //
  // IoBufferOwnership policy not supported for SKL PCH-H, perform codec init over HDA-Link
  //
  Status = DetectAndInitializeCodec (HdaConfig, HdaPciBase, HdaBar);

  if (HdaConfig->IoBufferVoltage == PchHdaIoBuf18V) {
    //
    // Set I/O Buffers voltage mode to 1.8V
    //
    DEBUG ((DEBUG_INFO, "HDA: I/O buffers operate in 1.8V mode drive strenght attribute\n"));
    PchPcrAndThenOr32 (PID_DSP, R_PCH_PCR_HDA_IOBCTL, (UINT32)~0, (UINT32)B_PCH_PCR_HDA_IOBCTL_VSEL);
  }

  if (HdaConfig->VcType == PchHdaVc1) {
    ///
    /// Enable Virtual Channel VC1
    /// Permit non snoop on VC0, PCI Config Space + C4h[24] = 1b.
    /// Enable VC1 traffic, PCR[HDA] + E4h[1] = 1b
    ///
    DEBUG ((DEBUG_INFO, "HDA: Isonchronous Support on VC1\n"));
    MmioOr32 ((UINTN) (HdaPciBase + R_PCH_HDA_SEM2), (UINT32)B_PCH_HDA_SEM2_VC0PSNR);
    PchPcrAndThenOr32 (PID_DSP, R_PCH_PCR_HDA_TTCCFG, (UINT32)~0, (UINT32)B_PCH_PCR_HDA_TTCCFG_HCDT);
  }

  ///
  /// Set HDABAR + 1048h[6:0] = 29h
  ///
  MmioAndThenOr8 ((UINTN) (HdaBar + R_PCH_HDABA_LTRC), (UINT8)~(0x7F), (UINT8)V_PCH_HDABA_LTRC_GB);

  ///
  /// Set HDABAR + 104Bh[5,3] = 1b and clear bit 2,1,0
  ///
  MmioAndThenOr8 ((UINTN) (HdaBar + R_PCH_HDABA_PCE), (UINT8)~(BIT2 | BIT1 | BIT0), (UINT8)(BIT5 | BIT3));

  ///
  /// Power Gating and Clock Gating enable
  /// Clear "PGD", PCR[DSP] + 530h[5] = 0b
  /// Clear "CGD", PCR[DSP] + 530h[3] = 0b
  ///

  ///
  /// LBG Si WA s4930535 - Do not enable Power Gating for HDA to avoid buggy spurious gating
  /// and ungating transitions 
  ///
  Data32AndFncfg &= ~(B_PCH_PCR_HDA_FNCFG_CGD);
  DEBUG ((DEBUG_INFO, "HDA: Clock Gating enabled\n"));


    ///
    /// Set PCR[HDA] + 530h [4] = 1b.
    ///
    DEBUG ((DEBUG_INFO, "HDA: Configuration Lockdown\n"));
    Data32OrFncfg |= B_PCH_PCR_HDA_FNCFG_BCLD;

  ///
  /// Silicon WA
  /// 5330784: [LBG Si Request] aDSP bug requires BIOS to write cfg register to disable aDSP as opposed to fuse disabled
  ///
  Data32OrFncfg |= B_PCH_PCR_HDA_FNCFG_ADSPD;
  ///
  /// Si VAL Request to enable PCI Express mode for HDA - s5331290
  /// The bit HDASPCID is set by default, we have to clear it
  ///
  Data32AndFncfg &= ~(B_PCH_PCR_HDA_FNCFG_HDASPCID);

  //
  // Write PCR[DSP] FNCFG register only once, because BCLD bit is RWO
  //
  PchPcrAndThenOr32 (PID_DSP, R_PCH_PCR_HDA_FNCFG, Data32AndFncfg, Data32OrFncfg);

  GetCodecCurrentBclkFrequency (HdaBar, HDA_SDI_0_HDALINK, &HdaLinkFrequency);
  if (HdaLinkFrequency == PchHdaLinkFreq6MHz) {
    DEBUG ((DEBUG_INFO, "HDA: 6MHz link frequency - assert RESET# and put controller to D3\n"));
    MmioAnd32 ((UINTN) (HdaBar + R_PCH_HDABA_GCTL), (UINT32)~(B_PCH_HDABA_GCTL_CRST));
    Status = StatusPolling (HdaBar + R_PCH_HDABA_GCTL, B_PCH_HDABA_GCTL_CRST, 0);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Reset High Definition Audio (CRST = 0) Time Out - 3!\n"));
    }
    ///
    /// Put controller to D3 - set PCS.PS = 11b
    ///
    MmioOr32 ((UINTN) (HdaPciBase + R_PCH_HDA_PCS), B_PCH_HDA_PCS_PS);
  }

  ///
  /// Clear HDBAR and disable memory map access
  ///
  MmioAnd16 ((UINTN) (HdaPciBase + PCI_COMMAND_OFFSET), (UINT16) (~EFI_PCI_COMMAND_MEMORY_SPACE));
  MmioWrite32 (HdaPciBase + R_PCH_HDA_HDALBA, 0);
  MmioWrite32 (HdaPciBase + R_PCH_HDA_HDAUBA, 0);

  DEBUG ((DEBUG_INFO, "ConfigureHdAudio() End - Status = %r\n", Status));

  return Status;
}

/**
  Initialize the Intel High Definition Audio Controller
    
  @param[in] PchPolicy            Policy
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI 
                                  devices to be used to initialize MMIO 
                                  registers.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_FOUND           HDA PCI device not found - fuse disabled
  @retval EFI_INVALID_PARAMETER   Provided VerbTableData is null
**/
EFI_STATUS
PchHdAudioInit (
  IN PCH_POLICY_PPI      *PchPolicy
  )
{
  EFI_STATUS                    Status;
  PCH_HDAUDIO_CONFIG            *HdaConfig;
  UINTN                         HdaPciBase;
  UINT32                        HdaBar;

  HdaConfig = &PchPolicy->HdAudioConfig;

  HdaPciBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_HDA,
                    PCI_FUNCTION_NUMBER_PCH_HDA
                    );

  if (MmioRead16 (HdaPciBase + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    DEBUG ((DEBUG_WARN, "HDA: PCI Device not found!\n"));
    return EFI_NOT_FOUND;
  }

  ///
  /// System BIOS should ensure that the High Definition Audio HDBAR HDA PCI offset 10-17h contains a valid address value
  /// and is enabled by setting HDA PCI offset 04h[1].
  ///
  HdaBar = PchPolicy->TempMemBaseAddr;

  // Initialize HD-Audio
  if (HdaConfig->Enable != FALSE) {
    Status = ConfigureHdAudio (HdaConfig, HdaPciBase, HdaBar);
    if (EFI_ERROR (Status) && HdaConfig->Enable == PCH_HDAUDIO_AUTO) {
      DEBUG ((DEBUG_ERROR, "HDA: Codec detection/initialization error in Auto mode - Disable!\n"));
      Status = DisableHdAudio ();
    }
  } else {
    Status = DisableHdAudio ();
  }

  return Status;
}
