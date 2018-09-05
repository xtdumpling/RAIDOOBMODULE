/**

Copyright (c) 1996 - 2017, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file CallBacks.c

   This file contains callbacks of events that SPS is interested in.
   
**/
#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>

#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/S3BootScriptLib.h>

#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Guid/GlobalVariable.h>

#include <Guid/HobList.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/SpsInfoHobGuid.h>

#include <Protocol/MpService.h>
#include <Protocol/ExitPmAuth.h>
#include "Protocol/PchReset.h"

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include "Library/DxeMeLib.h"

#include <Protocol/CpuIo.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/PciIo.h>

#include <Sps.h>
#include <PchAccess.h>
#include <Library/PchPlatformLib.h>

#include <SpsMisc.h>
#include <HeciRegs.h>
#include <MeAccess.h>

#include "SpsDxe.h"


/******************************************************************************
 * Variables
 */

/******************************************************************************
 * Functions
 */
/**
  SPS callback for EndOfDxe event.
  Performs actions required by SPS ME-BIOS Spec before Option ROMs are loaded.
  
  @param [in] Event    - the event, not used
  @param [in] pContext - pointer to SPS firmware description
  
  @retval Void.
**/
VOID SpsEndOfDxeCallback(
  IN EFI_EVENT      Event,
  IN VOID          *pContext)
{
  EFI_STATUS        Status;
  UINT32            Data32[2];
  UINT8             Data8;
  UINT32            RspLen;
  HECI_PROTOCOL    *pHeci;
  SPS_DXE_CONTEXT  *pSpsContext = (SPS_DXE_CONTEXT*)pContext;

  union
  {
    MCTP_SET_BUS_OWNER_REQ     MctpSetBusOwnerReq;
    MCTP_SET_BUS_OWNER_RSP     MctpSetBusOwnerRsp;
  } HeciMsg;

  gBS->CloseEvent (Event);

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR (Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (%r)\n", Status));
  }
  else
  {
    //
    // Send MCPT Bus Owner Proxy configuration if configured.
    //
    if (pSpsContext->pSpsPolicy->SpsConfig.SpsMctpBusOwner != 0 &&
        pSpsContext->pSpsHob->FeatureSet.Bits.MctpProxy)
    {
      DEBUG((DEBUG_INFO, "[SPS] Sending MCTP_SET_BUS_OWNER with address 0x%X\n",
             pSpsContext->pSpsPolicy->SpsConfig.SpsMctpBusOwner));
      HeciMsg.MctpSetBusOwnerReq.Command = MCPT_CMD_SET_BUS_OWNER_REQ;
      HeciMsg.MctpSetBusOwnerReq.Reserved0[0] =
      HeciMsg.MctpSetBusOwnerReq.Reserved0[1] =
      HeciMsg.MctpSetBusOwnerReq.Reserved0[2] = 0;
      HeciMsg.MctpSetBusOwnerReq.PCIeAddress = pSpsContext->pSpsPolicy->SpsConfig.SpsMctpBusOwner;
      HeciMsg.MctpSetBusOwnerReq.Location = 0;
      HeciMsg.MctpSetBusOwnerReq.Reserved1 = 0;
      RspLen = sizeof(HeciMsg.MctpSetBusOwnerRsp);
      Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.MctpSetBusOwnerReq,
                               sizeof(HeciMsg.MctpSetBusOwnerReq), &RspLen,
                               SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MCTP);
      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send MCTP_SET_BUS_OWNER (%r)\n", Status));
      }
      else if (HeciMsg.MctpSetBusOwnerRsp.Command != MCPT_CMD_SET_BUS_OWNER_RSP)
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Wrong response received (Command: 0x%x)\n", HeciMsg.MctpSetBusOwnerRsp.Command));
      }
      else if (HeciMsg.MctpSetBusOwnerRsp.Result != MCPT_CMD_SET_BUS_OWNER_RSP_SUCCESS)
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: MCTP_SET_BUS_OWNER failure (cause: %d)\n",
               HeciMsg.MctpSetBusOwnerRsp.Result));
      }
    }
  } // if (HECI protocol located)
  //
  // Update S3 resume script
  // S3BootScript library is using specific form of PCI address, let's define macro for it.
  //
#define S3BOOTSCRIPT_PCIADDR(Bus, Dev, Fun, Reg) \
          (UINT64)(((Bus) & 0xFF) << 24 | ((Dev) & 0x1F) << 16 | ((Fun) & 0x07) << 8 | ((Reg) & 0xFF ))
  //
  // If HECI-1 (D22 F0) is SMI or SCI mode there is no regular OS driver and
  // we must save it's configuration.
  //
  Data8 = HeciPciRead8(R_HIDM);
  if ((Data8 & ~HECI_INTERRUPUT_LOCK) != HECI_INTERRUPUT_GENERATE_LEGACY_MSI)
  {
    DEBUG((DEBUG_INFO, "[SPS] Saving HECI-1 configuration to S3 boot script\n"));
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_HIDM),
                                1, &Data8);
    Data32[0] = HeciPciRead32(R_HECIMBAR);
    Data32[1] = HeciPciRead32(R_HECIMBAR + 4);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint32,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_HECIMBAR),
                                2, &Data32[0]);
    Data8 = HeciPciRead8(R_IRQ);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_IRQ),
                                1, &Data8);
    Data8 = HeciPciRead8(R_COMMAND);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_COMMAND),
                                1, &Data8);
  }
  //
  // If HECI-2 (D22 F1) is SMI or SCI mode there is no regular OS driver and
  // we must save it's configuration.
  //
  Data8 = Heci2PciRead8(R_HIDM);
  if ((Data8 & ~HECI_INTERRUPUT_LOCK) != HECI_INTERRUPUT_GENERATE_LEGACY_MSI)
  {
    DEBUG((DEBUG_INFO, "[SPS] Saving HECI-2 configuration to S3 boot script\n"));
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_HIDM),
                                1, &Data8);
    Data32[0] = Heci2PciRead32(R_HECIMBAR);
    Data32[1] = Heci2PciRead32(R_HECIMBAR + 4);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint32,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_HECIMBAR),
                                2, &Data32[0]);
    Data8 = Heci2PciRead8(R_IRQ);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_IRQ),
                                1, &Data8);
    Data8 = Heci2PciRead8(R_COMMAND);
    if (!(Data8 & B_MSE) && (Data32[0] != 0 || Data32[1] != 0))
    {
      Data8 = B_BME | B_MSE;
      Heci2PciWrite8(R_COMMAND, Data8);
    }
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_COMMAND),
                                1, &Data8);
  }
  return;
} // SpsEndOfDxeCallback()


/**
    SPS callback for ready-to-boot event.
    Performs actions required by SPS ME-BIOS Spec before OS is loaded.
    
    @param Event    - the event, not used
    @param pContext - pointer to SPS firmware description
    
    @retval Void.
**/
VOID SpsReadyToBootCallback(
  IN EFI_EVENT      Event,
  IN VOID          *pContext)
{
  EFI_STATUS        Status;
  UINT32            RspLen;
  HECI_PROTOCOL    *pHeci;
  union
  {
    MKHI_MSG_END_OF_POST_REQ       EopReq;
    MKHI_MSG_END_OF_POST_RSP       EopRsp;
  } HeciMsg;
  
  PCH_RESET_PROTOCOL  *PchResetProtocol;
  UINTN               DataSize;
  VOID                *ResetData;

  gBS->CloseEvent(Event);

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR (Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (%r)\n", Status));
  }
  else
  {
#if ME_TESTMENU_FLAG
    //
    // Send END_OF_POST notification if not disabled in ME Test Menu options
    //
    if (MeEndOfPostEnabled())
#endif
    {
      DEBUG((DEBUG_INFO, "[SPS] Sending END_OF_POST to ME\n"));
      
      HeciMsg.EopReq.Mkhi.Data = 0x00000CFF;
      RspLen = sizeof(HeciMsg.EopRsp);
      Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.EopReq,
                               sizeof(HeciMsg.EopReq), &RspLen,
                               SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send END_OF_POST (%r)\n", Status));
        MeReportError(MSG_EOP_ERROR);
      }
      else if (HeciMsg.EopRsp.Mkhi.Data != 0x00008CFF)
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid END_OF_POST response (MKHI: 0x%X)\n",
                            HeciMsg.EopRsp.Mkhi.Data));
      }
      else if (RspLen == sizeof(HeciMsg.EopRsp) && // if response contains Action and
               HeciMsg.EopRsp.Action == 1)         // global reset was requested
      {
        DEBUG((DEBUG_WARN, "[SPS] WARNING: END_OF_POST response requests Global Reset!!!\n"));

        Status = gBS->LocateProtocol (&gPchResetProtocolGuid, NULL, &PchResetProtocol);

        if (!EFI_ERROR (Status)) {
          ResetData = PchResetProtocol->GetResetData (&gPchGlobalResetGuid, &DataSize);
          PchResetProtocol->Reset (PchResetProtocol, GlobalReset, DataSize, ResetData);
        }
      }
      // Set EndOfPostDone unconditionally
      SetEndOfPostDone();
    } // if (SystemCfgData.MeEndOfPostEnabled)
  } // if (HECI protocol located)

#if ME_TESTMENU_FLAG
  //
  // Upon user menu request set the CF9 reset promotion to global reset promotion
  //
  if (((SPS_DXE_CONTEXT*)pContext)->pSpsPolicy->SpsConfig.MeGrPromotionEnabled)
  {
    DEBUG((DEBUG_INFO, "[SPS] Promoting CF9 reset to global reset\n"));
    MmioOr32(MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC,
                               PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ETR3, (UINT32)B_PCH_PMC_ETR3_CF9GR);
  }
#endif

#if ME_TESTMENU_FLAG
  //
  // Lock global reset in ETR3 register if not disabled in ME Test Menu options
  //
  if (((SPS_DXE_CONTEXT*)pContext)->pSpsPolicy->SpsConfig.MeGrLockEnabled)
#endif
  {
    DEBUG((DEBUG_INFO, "[SPS] Disabling Global Reset capability\n"));
    MmioOr32(MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC,
                               PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ETR3, (UINT32)B_PCH_PMC_ETR3_CF9LOCK);
  }
  return;
} // SpsReadyToBootCallback()


