/*++
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 1996 - 2016, Intel Corporation.

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


Module Name:

  CrystalRidge.c

Abstract:

  Implementation of NVMCTLR Access of Mailbox thru SADs

--*/
//
// Include files
//
#include <IndustryStandard/SmBios.h>
#include <SysHostChip.h>
#include <Protocol/PcatProtocol.h>

#include "CrystalRidge.h"
#include "RdRand.h"
#include "Nfit.h"
#include "Pcat.h"
#include "XlateFunctions/XlateFunctions.h"

// APTIOV_SERVER_OVERRIDE_RC_START
void *memset(void *dst, char value, size_t cnt);  //To resolve build error
// APTIOV_SERVER_OVERRIDE_RC_END
#pragma warning(disable : 4100)
#pragma optimize("", off)


//
// Globals
static EFI_CRYSTAL_RIDGE_PROTOCOL      mCrystalRidgeProtocol;
struct SystemMemoryMapHob       *mSystemMemoryMap;
CR_INFO                         mCrInfo;

EFI_SMM_BASE2_PROTOCOL          *mSmmBase = NULL;
EFI_SMM_SYSTEM_TABLE2           *mSmst = NULL;

UINT16                          mNumSMBiosHandles;
SMBIOS_HANDLE_INFO              mSMBiosHandleInfo[MAX_SOCKET * MAX_CH * MAX_AEP_CH];

EFI_MP_SERVICES_PROTOCOL        *mMpService;
EFI_CPU_CSR_ACCESS_PROTOCOL     *mCpuCsrAccess;
UINT64                          mWpqFlushHintAddress[MAX_SOCKET * MAX_IMC];

NGN_ACPI_SMM_INTERFACE              *mNgnAcpiSmmInterface;
EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL mNgnAcpiSmmInterfaceArea;

// Global boolean to tell whether to use BIOS Mailbox or OS Mailbox
BOOLEAN                         mUseOSMailbox = FALSE;

UINT8                           *mLargePayloadBuffer;

/**

Routine Description: Status Decode will print status message after issuing a NVMCTLR Command

  @param mbstatus    - 8 bit status code from status register
  @retval 0 - success
  @retval not 0 - EFI error value according to type status code returned

**/

EFI_STATUS
MbStatusDecode (
  MB_SMM_STATUS_FNV_SPA_MAPPED_0_STRUCT *pSmmStatusReg
)
{
  EFI_STATUS  Status;

  switch(pSmmStatusReg->Bits.stat) {
    case MB_CMD_SUCCESS:
      Status = EFI_SUCCESS;
      DEBUG((DEBUG_INFO, "NVMCTL Mailbox SUCCESS\n"));
      break;
    case MB_CMD_INVALID_PARAM:
      Status = EFI_INVALID_PARAMETER;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Invalid Command Parameter\n"));
      break;
    case MB_CMD_DATA_XFER_ERR:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Data Transfer Error\n"));
      break;
    case MB_CMD_INTERNAL_ERR:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Internal Device Error\n"));
      break;
    case MB_CMD_UNSUPPORTED_CMD:
      Status = EFI_UNSUPPORTED;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Unsupported Command\n"));
      break;
    case MB_CMD_DEVICE_BUSY:
      Status = EFI_NOT_READY;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Device Busy\n"));
      break;
    case MB_CMD_INCORRECT_NONCE:
      Status = EFI_ACCESS_DENIED;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Incorrect Passphrase/Security Nonce\n"));
      break;
    case MB_CMD_SECURITY_CHK_FAIL:
      Status = EFI_ACCESS_DENIED;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: FW Authentification Failed\n"));
      break;
    case MB_CMD_INVALID_SEC_STATE:
      Status = EFI_SECURITY_VIOLATION;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Invalid Security State\n"));
      break;
    case MB_CMD_SYS_TIME_NOT_SET:
      Status = EFI_NOT_STARTED;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: System Time Not Set\n"));
      break;
    case MB_CMD_DATA_NOT_SET:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Data Not Set\n"));
      break;
    case MB_CMD_ABORTED:
      Status = EFI_ABORTED;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Aborted\n"));
      break;
    case MB_CMD_NO_NEW_FW:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: No New FW to Execute\n"));
      break;
    case MB_CMD_REVISION_FAIL:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Revision Failure\n"));
      break;
    case MB_CMD_INJECT_NOT_ENABLED:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Injection Not Enabled\n"));
      break;
    case MB_CMD_CONFIG_LOCKED:
      Status = EFI_WRITE_PROTECTED;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Config Lockdown\n"));
      break;
    case MB_CMD_INVALID_ARGUMENT:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Invalid Alignment\n"));
      break;
    case MB_CMD_INCOMPATIBLE_DIMM:
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Incompatible DIMM Type\n"));
      break;
    case MB_CMD_TIMEOUT:
      Status = EFI_TIMEOUT;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Timeout Occured\n"));
      break;
    default:
      Status = EFI_NO_RESPONSE;
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Unknown status: %x\n", pSmmStatusReg->Bits.stat));
      break;
  }

  if (!pSmmStatusReg->Bits.comp) {
    Status = EFI_NOT_READY;
    DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Completion Bit Not Set\n"));
  }

  if (pSmmStatusReg->Bits.stat != MB_CMD_SUCCESS) {
    if (pSmmStatusReg->Bits.nonce_fail) {
      DEBUG((EFI_D_ERROR, "NVMCTL Mailbox Failure: Nonce Mismatch\n"));
    }
  }

  DEBUG((EFI_D_INFO, "NVMCTL MbStatusDecode - Status: %r\n", Status));

  return  Status;
}

/**

  Returns mailbox for given Dimm.

  @param NvmDimmPtr   - AEP Dimm structure, describing current Dimm
  @param UseOSMailbox - If TRUE, use the OS mailbox. BIOS mailbox otherwise.

  @return Pointer to CR Mailbox for given AEP Dimm

**/
CR_MAILBOX *
GetMailbox(
  AEP_DIMM  *NvmDimmPtr,
  BOOLEAN   UseOSMailbox
  )
{
  if (UseOSMailbox) {
    return (&NvmDimmPtr->OSMailboxSPA);
  } else {
    return (&NvmDimmPtr->MailboxSPA);
  }
}

/**

  Utility function to check whether given DIMM is using FNV or EKV controller.
  In the case DID is not programmed, assuming older gen.

  @param NvmDimmPtr - AEP Dimm structure, describing current Dimm

  @return           - TRUE if FNV or EKV or DeviceId == 0x0
**/
BOOLEAN
IsNvmCtrlFirstGen(
  AEP_DIMM *NvmDimmPtr
  )
{
  if (NvmDimmPtr->DeviceID == 0x0) {
    DEBUG((EFI_D_ERROR, "CRP: Warning: Device ID is 0x0! This could lead to potential problems!\n"));
  }

  if ((NvmDimmPtr->DeviceID == FNV) ||
      (NvmDimmPtr->DeviceID == EKV)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Routine Description: Returns dimm number for
    channel which has a Ngn Dimm.

  @param[in]  Socket      - Socket Number
  @param[in]  Ch          - DDR Channel ID
  @param[out] Dimm        - Dimm number of the channel

  @retval EFI_SUCCESS           Dimm found
  @retval EFI_INVALID_PARAMETER Dimm is NULL
  @retval EFI_NOT_FOUND         Dimm was not found

**/
EFI_STATUS
GetChannelDimm(
  UINT8     Socket,
  UINT8     Ch,
  UINT8     *Dimm
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  if (Dimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsNvmDimm(Socket, Ch, 0)) {
    *Dimm = 0;
  } else if (IsNvmDimm(Socket, Ch, 1)) {
    *Dimm = 1;
  } else {
    Status = EFI_NOT_FOUND;
  }

  return (Status);
}

/**

  Routine Description: This function computes the
  number of ways iMC has been interleaved for a particular SAD in
  both the Numa and Uma cases.

  @param[in] Socket      - socket number (0 based)
  @param[in] SadIndex    - SAD index

  @return iMC interleave Ways

**/
UINT8
GetImcWays (
  UINT8       Socket,
  UINT8       SadIndex
  )
{
  UINT8        ImcWays, i;

  ImcWays = 0;
  for (i = 0; i < MC_MAX_NODE; i++) {
    if (mSystemMemoryMap->Socket[Socket].SADintList[SadIndex][i]) {
      ImcWays++;
    }
  }
  return ImcWays;
}

/**

  Routine description: Gets the channel ways number for given SAD index and socket.

  @param[in] Socket   - socket number (0 based)
  @param[in] SadIndex - SAD index

  @return Channel ways

*/
UINT8
GetChWays(
    UINT8 Socket,
    UINT8 SadIndex
    )
{
  UINT8  ChWays, i, McIndex;
  ChWays = 0;
  // Determine which IMC channel bitmap to use
  McIndex = GetMcIndexFromBitmap (mSystemMemoryMap->Socket[Socket].SAD[SadIndex].imcInterBitmap);
  for (i = 0; i < MAX_MC_CH; i++) {
    if (((mSystemMemoryMap->Socket[Socket].SAD[SadIndex].FMchannelInterBitmap[McIndex] >> i) & 0x1) ==1) {
      ChWays++;
    }
  }
  return ChWays;
}

/**

  Routine Description: Find the MC index to use for calculating channel ways.

  @param[in] ImcInterBitmap  - bitmap of IMCs in the interleave.
                               ImcInterBitmap must be a non-zero value in input.
                               Routine assumes value has BIT0 and/or BIT1 set.

  @return IMC number to use for calculating channel ways

**/
UINT8
GetMcIndexFromBitmap (
  UINT8 ImcInterBitmap
  )
{
  UINT8 Imc;

  // if this entry is IMC0/IMC1 interleaved then use MC0 since number of channels have to be the same > 1 IMC way
  if ((ImcInterBitmap & BIT0) && (ImcInterBitmap & BIT1)) {
    Imc = 0;
  } else if (ImcInterBitmap & BIT1) { // if this entry for MC1 only
    Imc = 1;
  } else { // if this entry for MC0 only
    Imc = 0;
  }
  return Imc;
}

/**

Routine Description: GetMcNgnControlRegionReservedBase - This function returns reserved region for NGN dimm

  @param Socket - Socket Number
         Mc - Memory controller number
         Offset - DPA Offset
  @retval UINT64 Memory controller control region reserved address

**/
UINT64
GetMcNgnControlRegionReservedBase (
  UINT8  Socket,
  UINT8  Mc,
  UINT64 Offset
  )
{
  UINT8           i;
  UINT8           ch;
  UINT8           dimm;
  PAEP_DIMM       pDimm = NULL;
  UINT64          McNgnControlRegionReservedBase = 0;
  EFI_STATUS      Status;

  for( i = 0; i < MAX_SAD_RULES; i++) {
    //Break if we have reached the end of the SAD table
    if (mSystemMemoryMap->Socket[Socket].SAD[i].Enable == 0) {
      break;
    }

    // Continue if entry is a remote entry
    if (mSystemMemoryMap->Socket[Socket].SAD[i].local == 0) {
      continue;
    }

    //Find the first SAD entry for Control region in this IMC
    if ((mSystemMemoryMap->Socket[Socket].SAD[i].type == MEM_TYPE_CTRL) &&
        (mSystemMemoryMap->Socket[Socket].SADintList[i][Socket * MAX_IMC + Mc] == 1)) {
      //Find the lowest channel number that is being interleaved.
      if (mSystemMemoryMap->Socket[Socket].SAD[i].FMchannelInterBitmap[Mc] & BIT0) {
        ch = 0;
      } else if (mSystemMemoryMap->Socket[Socket].SAD[i].FMchannelInterBitmap[Mc] & BIT1) {
        ch = 1;
      } else {
        ch = 2;
      }

      if(Mc == 1 && ch < MAX_MC_CH) {
        ch += MAX_MC_CH;
      }

      //Find the dimm slot that has the NVMDIMM in ths channel.
      Status = GetChannelDimm(Socket, ch, &dimm);
      ASSERT_EFI_ERROR(Status);

      //Get the PAEP_DIMM struct for the corresponding dimm
      pDimm = GetDimm(Socket, ch, dimm);

      if(pDimm != NULL) {
        //Call the XlateDpaOffsetToSpa to find the SPA of offset in the DDRT_CTRL_DPA_START
        XlateDpaOffsetToSpa(pDimm, pDimm->CntrlBase, Offset, &McNgnControlRegionReservedBase);
      }
      break;
    }
  }
  return McNgnControlRegionReservedBase;
}

/**

Routine Description: ProgramFlushHintAddressForWpqFlush - This function programs Flush hint address in CPU and IMC

  @param VOID
  @retval EFI_STATUS

**/
VOID
ProgramFlushHintAddressForWpqFlush (
   VOID
   )
{
  UINT8                            Socket;
  UINT8                            Mc;
  PAD0_N0_M2MEM_MAIN_STRUCT        Pad0N0;
  PAD0_N1_M2MEM_MAIN_STRUCT        Pad0N1;
  UINT64                           McNgnControlRegionReservedBase;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      if (!(mSystemMemoryMap->Socket[Socket].imcEnabled[Mc])) {
        continue;
      }

      // The control region is always interleaved as 4KB socket/memory controller and 256B channel in the current implementation
      // So 4K addresses are mapped to each MC having NGN Dimm
      // Only program PAD0, no need to program PAD1 and PAD2 as 16 addresses are programmed in PAD0 iwth 4K interleave across MC for control region.
      McNgnControlRegionReservedBase = GetMcNgnControlRegionReservedBase(Socket, Mc, (FLUSH_HINT_ADDRESS_OFFSET + MC_FLUSH_HINT_ADDRESS_OFFSET));
      mWpqFlushHintAddress[Socket * MAX_IMC + Mc] = McNgnControlRegionReservedBase;
      mSystemMemoryMap->Socket[Socket].imc[Mc].WpqFlushHintAddress = McNgnControlRegionReservedBase;

      if (mWpqFlushHintAddress[Socket * MAX_IMC + Mc]) {
        DEBUG ((EFI_D_ERROR, "CRP: mWpqFlushHintAddress[0x%x] = 0x%lx\n", Socket * MAX_IMC + Mc, McNgnControlRegionReservedBase));
        Pad0N0.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, PAD0_N0_M2MEM_MAIN_REG);
        Pad0N1.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, PAD0_N1_M2MEM_MAIN_REG);
        Pad0N0.Bits.address = (UINT32)mWpqFlushHintAddress[Socket * MAX_IMC + Mc] >> 6;
        Pad0N0.Bits.enable = 1;
        Pad0N1.Bits.address = (UINT32)(mWpqFlushHintAddress[Socket * MAX_IMC + Mc] >> 32);
        Pad0N1.Bits.mask = 0xF;  // Mask 16 addresses
        mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, PAD0_N1_M2MEM_MAIN_REG, Pad0N1.Data);
        mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, PAD0_N0_M2MEM_MAIN_REG, Pad0N0.Data);
      }
    }
  }
}

/**

Routine Description: ProgramCrQos - This function programs MSR 0x6D based on CR_QOS_CONFIG setup option

  @param VOID
  @retval VOID

**/
VOID
ProgramCrQos(
   VOID
   )
{
  //
  // Program MSR 0x6D based on CR_QOS_CONFIG setup option
  AsmWriteMsr64 (0x6d, mSystemMemoryMap->MemSetup.crQosConfig);
}

/**

Routine Description: ProgramCrQosForAllCores - This function programs CrQos in all CPU cores

  @param VOID
  @retval VOID

**/
VOID
ProgramCrQosForAllCores (
   VOID
   )
{
  BOOLEAN                           Packages[MAX_SOCKET] = {FALSE};
  EFI_PROCESSOR_INFORMATION         ProcessorInfoBuffer;
  UINTN                             ProcessorNum;
  UINTN                             NumProcessors;
  UINTN                             NumEnabledProcessor;

  mMpService->GetNumberOfProcessors (
                mMpService,
                &NumProcessors,
                &NumEnabledProcessor
                );

  DEBUG ((EFI_D_INFO, "CRP: Programming QoS optimization to %x\n", mSystemMemoryMap->MemSetup.crQosConfig));
  for (ProcessorNum = 0; ProcessorNum < NumProcessors; ProcessorNum++) {
    mMpService->GetProcessorInfo (
                  mMpService,
                  ProcessorNum,
                  &ProcessorInfoBuffer
                  );
    if (Packages[ProcessorInfoBuffer.Location.Package] == FALSE) {
      if (ProcessorInfoBuffer.StatusFlag & PROCESSOR_AS_BSP_BIT) {
        ProgramCrQos ();
      } else {
        mMpService->StartupThisAP (mMpService,
                      (EFI_AP_PROCEDURE)ProgramCrQos,
                      ProcessorNum,
                      NULL,
                      0,
                      NULL,
                      NULL
                      );
      }
      Packages[ProcessorInfoBuffer.Location.Package] = TRUE;
    }
  }
}


/**

Routine Description: CreateDimmPresentBitmaps - This function
basically creates 2 64-bit fields that are part of the
AcpiSMMInterface. The first 64-bit field is for socket 0-3 and
the second 64-bit field is for socket 3-7. This field contains
bit that are set in the corresponding position when an
NVMDIMM is present in that position. Least significant 16 bits
(of the 1st field) represents socket0 and the most signifFicant
16-bit is for socket3 and the other sockets in between. For
example, if the system has 2 sockets and there is an NVMDIMM in
Dimm0 of all channels, then the bitmap would look like below:
0000000000000000000000000000000000000101010101010000010101010101b

  @param VOID         - None
  @retval VOID        - None

**/
VOID
CreateDimmPresentBitmaps(
   VOID
   )
{
  UINT64       DimmBitmap1 = 0;
  UINT64       DimmBitmap2 = 0;
  UINT8        Socket;
  UINT8        Ch;
  UINT8        Dimm;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Ch = 0; Ch < MAX_CH; Ch++) {
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (IsNvmDimmEnabled(Socket, Ch, Dimm)) {
          if (Socket < 4) {
            DimmBitmap1 |= ((UINT64)0x1 << ((Socket * 16) + (Ch * 2) + Dimm));
          } else {
            DimmBitmap2 |= ((UINT64)0x1 << ((Socket * 16) + (Ch * 2) + Dimm - 64));
          }
        }
      }
    }
  }
  mNgnAcpiSmmInterface->NvDimmConfigBitmap0 = DimmBitmap1;
  mNgnAcpiSmmInterface->NvDimmConfigBitmap1 = DimmBitmap2;
}

/**

Routine Description: GetDimmInfoFromDevHandle - This function
deconstructs Socket, Channel and Dimm from the Device Handle
that is passed in and returns them in their respective pointers
that are passed in.

  @param *Socket      - pointer to Socket that this function
         will return
  @param *Ch          - pointer to channel that this function
         will return
  @param *Dimm        - Pointer to Dimm that this function will
         return
  @param DevHandle    - 32-bit device handle.
  @retval None - void

**/
VOID
GetDimmInfoFromDevHandle(
   UINT8       *Socket,
   UINT8       *Ch,
   UINT8       *Dimm,
   UINT32      DevHandle
   )
{
  UINT8 Imc;

  // Bit 15:12 of DevHandle contains the socket
  *Socket = (UINT8)((DevHandle >> 12) & 0x0000000F);
  // Bit 11:8 contains the memory controller ID in Nfit DeviceHandle
  Imc = (UINT8)((DevHandle >> 8) & 0x0000000F);
  // Bit 7:4 contains the channel number
  *Ch = (UINT8)((DevHandle >> 4) & 0x0000000F);

  // Nfit table starts from ch 0 for each Imc, so if Imc is 1, need to calculate overall Ch number.
  if (Imc == 1 && *Ch < MAX_MC_CH) {
    *Ch += MAX_MC_CH;
  }

  *Dimm = (UINT8)(DevHandle & 0x0000000F);
}

/**

  Routine Description: Returns channel interleve for given socket, Imc, and Sad index.

  @param[in] Socket   - socket number
  @param[in] Imc      - memory controller number
  @param[in] SadInx   - SAD index

  @return Ways of channel interleave for socket/sad

**/
UINT8
GetChInterleave(
  UINT8 Socket,
  UINT8 Imc,
  UINT8 SadInx
  )
{
  UINT8             ChInterleave;
  struct SADTable   *SadPtr;

  if (Socket >= MAX_SOCKET || Imc >= MAX_IMC || SadInx >= SAD_RULES) {
    return 0;
  }

  SadPtr = &mSystemMemoryMap->Socket[Socket].SAD[SadInx];

  if (SadPtr->FMchannelInterBitmap[Imc] == 0x7) {
    ChInterleave = CH_3WAY;
  } else if ((SadPtr->FMchannelInterBitmap[Imc] == 0x3) ||
             (SadPtr->FMchannelInterBitmap[Imc] == 0x5) ||
             (SadPtr->FMchannelInterBitmap[Imc] == 0x6)) {
    ChInterleave = CH_2WAY;
  } else {
    ChInterleave = CH_1WAY;
  }

  return ChInterleave;
}

/**

  Routine Description: Returns interleave channel bitmap for given socket and SAD.

  @param[in] Socket   - socket number
  @param[in] SadInx   - SAD number

  @return Channel bitmap for given socket/SAD. Counting from BIT0 - channel 1 of first iMC,
          BIT1 - channel 2 of first iMC, etc.

**/
UINT32
GetChInterleaveBitmap(
  UINT8 Socket,
  UINT8 SadInx
  )
{
  UINT32            InterleaveBitmap = 0;
  struct SADTable   *SadPtr;
  UINT8             i;

  if (Socket >= MAX_SOCKET || SadInx >= SAD_RULES) {
    return 0;
  }

  SadPtr = &mSystemMemoryMap->Socket[Socket].SAD[SadInx];

  // check which iMC is involved in the interleaving set
  for (i = 0; i < MAX_IMC; i++) {
    if (mSystemMemoryMap->Socket[Socket].SADintList[SadInx][(Socket * MAX_IMC) + i])  {
      InterleaveBitmap |= SadPtr->FMchannelInterBitmap[i] << (MAX_MC_CH * i);
    }
  }

  return InterleaveBitmap;
}

/**

  Routine Description: This helper function returns first index of the PMEMInfo
  array found based on the SAD base address passed in.

  @param[in] SpaBaseAddr     - Spa Start Address of the SAD Rule

  @return Index of the array, -1 if no match found

**/
INT32
GetPmemIndex (
   UINT64      SpaBaseAddr
   )
{
  INT32            Index = -1;
  UINT32           i;

  for (i = 0; i < mCrInfo.NumPMEMRgns, i < ARRAY_SIZE (mCrInfo.PMEMInfo); i++) {
    if (mCrInfo.PMEMInfo[i].SADPMemBase == SpaBaseAddr) {
      Index = (INT32)i;
      return Index;
    }
  }
  return Index;
}

/**

  Routine Description: This helper function returns the total number of Dimms
  in the Interleave set of the SAD Rule for a PMEM Region.

  @param[in] SpaBaseAddr - Spa Start Address of the SAD Rule

  @return Total number of dimms in the Interleave set.

**/
UINT8
GetNumDimmsForPmemRgn (
  UINT64      SpaBaseAddr
  )
{
  UINT8            i = 0;
  UINT8            DimmCount = 0;
  PMEM_INFO        *Pmem = NULL;

  for (i = 0; i < mCrInfo.NumPMEMRgns; i++) {
    Pmem =  &mCrInfo.PMEMInfo[i];
    if (Pmem->SADPMemBase == SpaBaseAddr) {
      DimmCount++;
    }
  }
  return DimmCount;
}

/**

  Routine Description: This function checks if the given start address
  of a SAD rule belongs to a PMEM Region.

  @param[in] SpaBaseAddr - Spa Start Address of the SAD Rule

  @return TRUE if yes, else FALSE

**/
BOOLEAN
IsPmemRgn (
  UINT64 SpaBaseAddr
  )
{
  INT32 Index;

  Index = GetPmemIndex (SpaBaseAddr);

  if (Index >= 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Routine Description: This helper function returns index of the PMEMInfo array
  based on the SAD base address, Socket and Channel info passed in.

  @param[in] SpaBaseAddr  - Spa Start Address of the SAD Rule
  @param[in] Socket       - Socket Number
  @param[in] Ch           - Channel

  @return Index of the array, -1 if no match found

**/
INT32
GetPmemIndexBasedOnSocCh (
  UINT64      SpaBaseAddr,
  UINT8       Socket,
  UINT8       Ch
  )
{
  INT32            Index = -1;
  UINT32           i;

  for (i = 0; i < mCrInfo.NumPMEMRgns, i < ARRAY_SIZE (mCrInfo.PMEMInfo); i++) {
    if ((mCrInfo.PMEMInfo[i].SADPMemBase == SpaBaseAddr) &&
        (mCrInfo.PMEMInfo[i].Socket      == Socket) &&
        (mCrInfo.PMEMInfo[i].Channel     == Ch)) {
      Index = (INT32)i;
      return (Index);
    }
  }
  return (Index);
}

/**

  Routine Description: This helper function returns interleave ways for the
  given PMEM region.

  @param[in] SpaBaseAddr  - Spa Start Address of the SAD Rule

  @return Interleave ways for PMEM region or 0 if error

**/
UINT8
GetInterleaveWaysForPmemRgn (
  UINT64 SadBase
  )
{
  INT32 Index;

  Index = GetPmemIndex (SadBase);

  if (Index < 0) {
    return 0;
  }

  return mCrInfo.PMEMInfo[Index].iMCWays * mCrInfo.PMEMInfo[Index].iMCWays;
}

/**

  Routine Description: Gets the PMEM DPA base address for given NVDIMM using the
  SPA base address for the relevant SAD of type PMEM.

  @param[in] SadSpaBase  - Base Address of the SAD Rule found
  @param[in] Socket      - Socket Number
  @param[in] Ch          - Channel number
  @param[in] Dimm        - Dimm number

  @return DPA base address or 0 in case of failure

**/
UINT64
GetPmemDpaBase (
  UINT64  SadSpaBase,
  UINT8   Socket,
  UINT8   Ch,
  UINT8   Dimm
  )
{
  EFI_STATUS  Status;
  UINT8       i;
  UINT64      DpaBase = 0;
  UINT64      SadStart;
  UINT64      SadLimit;
  UINT32      DpaOffset = 0;

  if ((Socket >= MAX_SOCKET) ||
      (Ch     >= MAX_CH)     ||
      (Dimm   >= MAX_DIMM)) {
    return 0;
  }

  for (i = 0; i < MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++)
  {
    Status = GetSadLimits (SadSpaBase, Socket, &SadStart, &SadLimit);

    if (EFI_ERROR (Status)) {
      break;
    }

    SadLimit = SadLimit >> BITS_64MB_TO_BYTES;

    if ((UINT32)SadLimit == mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].perDPAMap[i].SPALimit) {
      DpaOffset = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].perDPAMap[i].perRegionDPAOffset;
      break;
    }
  }

  DpaBase = (UINT64)(mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].perRegionDPA + DpaOffset) << BITS_64MB_TO_BYTES;

  return DpaBase;
}

/**

  Routine Description: This function returns socket granularity for interleave
  (which is really iMC Granularity) as part of the setup options for the memory
  configuration.

  @param[in] SadPtr      - pointer to the SAD

  @return Socket Granularity

**/
UINT32
GetSocketGranularity (
  struct SADTable *SadPtr
  )
{
  UINT32   SktGran, SetupOptions;
  UINT8    GranEncoding = 2;

  SetupOptions = mSystemMemoryMap->MemSetup.options;

  if ((SadPtr->type & MEM_TYPE_PMEM) || (SadPtr->type & MEM_TYPE_PMEM_CACHE)) {
    switch (SadPtr->granularity) {
      case MEM_INT_GRAN_PMEM_NUMA_4KB_4KB:
        // Fall thru
      case MEM_INT_GRAN_PMEM_NUMA_4KB_256B:
        GranEncoding = 2;
        break;
    }
  } else if (SadPtr->type & MEM_TYPE_BLK_WINDOW) {
    // No Setup Option for Block Window or Control Region, just fixed size of 4k
    GranEncoding = 2;
  } else if (SadPtr->type & MEM_TYPE_CTRL) {
    // No Setup Option for Block Window or Control Region, just fixed size of 4k
    GranEncoding = 2;
  }

  switch (GranEncoding) {
  case 0:
    SktGran = 64;
    break;
  case 1:
    SktGran = 256;
    break;
  case 2:
    SktGran = 4096;
    break;
  case 3:
    SktGran = 0x40000000;
    break;
  default:
    SktGran = 4096;
    break;
  }
  return SktGran;
}

/**

  Routine Description: This function returns channel granularity for interleave
  as part of the setup options for the memory configuration.

  @param[in] SadPtr      - pointer to the SAD

  @return Channel granularity

**/
UINT32
GetChannelGranularity (
  struct SADTable *SadPtr
  )
{
  UINT32   ChGran;
  UINT8    GranEncoding = 2;

  if (SadPtr->type & MEM_TYPE_PMEM) {
    switch (SadPtr->granularity)  {
    case 1:
      GranEncoding = 2;
      break;
    case 2:
      GranEncoding = 1;
      break;
    }
  } else if (SadPtr->type & MEM_TYPE_PMEM_CACHE) {
    switch (SadPtr->granularity)  {
    case 1:
      GranEncoding = 2;
      break;
    case 2:
      GranEncoding = 1;
      break;
    }
  } else if (SadPtr->type & MEM_TYPE_BLK_WINDOW) {
    // No setup options for Blk and Ctrl region, use 256 bytes for channel granularity
    GranEncoding = 1;
  } else if (SadPtr->type & MEM_TYPE_CTRL) {
    // No setup options for Blk and Ctrl region, use 256 bytes for channel granularity
    GranEncoding = 1;
  }
  switch (GranEncoding) {
  case 0:
    ChGran = 64;
    break;
  case 1:
    ChGran = 256;
    break;
  case 2:
    ChGran = 4096;
    break;
  default:
    ChGran = 256;
    break;
  }
  return ChGran;
}

/**

  Routine Description: Given a SAD base address, function returns channel
  granularity based on the SAD Rule.

  @param[in] SadPtr      - pointer to the SAD

  @return Channel granularity

**/
UINT32
GetChGranularityFromSadBaseAddr (
  UINT64 SadBaseAddr
  )
{
  UINT8           Soc, i;
  struct          SADTable *SadPtr;
  UINT64          SpaBase;
  UINT32          ChGran = 0;

  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      SadPtr = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);
      }
      if ((SpaBase == SadBaseAddr) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        ChGran = GetChannelGranularity (SadPtr);
        break;
      }
    }
  }
  return ChGran;
}

/**

  Routine Description: This function returns SAD Rule limits based on the
  address passed in.

  @param[in]  SpaAddress   - Spa address used to find containing SAD Rule
  @param[in]  Socket       - socket id, for which SAD limits are to be found
  @param[out] *SpaStart    - start address of SAD rule
  @param[out] *SpaEnd      - end address of SAD rule

  @retval EFI_SUCCESS           - limits found
  @retval EFI_INVALID_PARAMETER - SpaStart or SpaEnd are NULL
  @retval EFI_NOT_FOUND         - limits not found, output variables not changed

**/
EFI_STATUS
GetSadLimits (
  UINT64  SpaAddress,
  UINT8   Socket,
  UINT64  *SpaStart,
  UINT64  *SpaEnd
  )
{
  UINT8            i;
  struct SADTable  *Sad;
  UINT64           SadStart, SadEnd;

  if (SpaStart == NULL || SpaEnd == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < SAD_RULES; i++) {
    Sad = &mSystemMemoryMap->Socket[Socket].SAD[i];

    if (Sad->Enable == 0 || Sad->local == 0) {
      continue;
    }

    if (i == 0) {
      SadStart = 0;
    } else {
      SadStart = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);
    }
    SadEnd = ((UINT64)Sad->Limit << BITS_64MB_TO_BYTES);

    if (SadStart <= SpaAddress && SpaAddress < SadEnd) {
      *SpaStart = SadStart;
      *SpaEnd   = SadEnd;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**

  Routine Description: This function returns Socket and Channel Information
  of a dimm given its virtual socket and channel information that the alogrithm uses.
  This routine handles both Control Region and PMEM regions as well.

  @param[in]  SpaStartAddr - Spa Start Address of the SAD Rule
  @param[in]  IsPmem       - TRUE for PMEM region type, FALSE for Cntrl Region type
  @param[in]  ChWays       - Interleave channel ways
  @param[in]  VirtualSkt   - Virtual Socket number
  @param[in]  VirtualImc   - Virtual Imc number
  @param[in]  VirtualCh    - Virtual Channel number
  @param[out] *Socket      - This pointer will have socket number
  @param[out] *Channel     - This pointer will hold channel number

  @return Status      - Status

**/
EFI_STATUS
VirtualToPhysicalDimmInfo (
   UINT64      SpaStartAddr,
   BOOLEAN     IsPmem,
   UINT8       ChWays,
   UINT8       VirtualSkt,
   UINT8       VirtualImc,
   UINT8       VirtualCh,
   UINT8       *Socket,
   UINT8       *Channel
   )
{
  UINT8            i;
  UINT8            Limit;
  UINT8            Index;
  UINT64           BaseAddr;
  PMEM_INFO        *PmemPtr;
  AEP_DIMM         *NvmDimmPtr;

  if (VirtualSkt >= MAX_SOCKET ||
      VirtualImc >= MAX_IMC    ||
      ChWays     == 0          ||
      ChWays     >  CH_3WAY    ||
      VirtualCh  >= ChWays     ||
      Socket     == NULL       ||
      Channel    == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Limit = IsPmem ? mCrInfo.NumPMEMRgns : mCrInfo.NumAepDimms;
  Index = Limit;

  for (i = 0; i < Limit; i++) {
    BaseAddr = IsPmem ? mCrInfo.PMEMInfo[i].SADPMemBase : mCrInfo.AepDimms[i].SADSpaBase;

    //
    // when given SPA start address is equal to base address from structure,
    // we have our starting index
    //
    if (BaseAddr == SpaStartAddr) {
      //
      // Index number of pmem or aep structure is counted using this equation
      //   (channel, iMC and socket numbers are 0 based):
      //
      // starting index + channel number + channel ways * iMC number +
      //   channel ways * max iMCs per socket * socket number
      //
      Index = i + VirtualCh + ChWays * (VirtualImc + MAX_IMC * VirtualSkt);
      break;
    }
  }

  if (Index >= Limit) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsPmem) {
    PmemPtr    = &mCrInfo.PMEMInfo[Index];
    *Socket    = PmemPtr->Socket;
    *Channel   = PmemPtr->Channel;
  } else {
    NvmDimmPtr = &mCrInfo.AepDimms[Index];
    *Socket    = (UINT8)NvmDimmPtr->SocketId;
    *Channel   = NvmDimmPtr->Ch;
  }

  return EFI_SUCCESS;
}

/**

  Routine Description: This function is responsible for getting Socket, Channel and Dimm
  information based on the given System Physical Address when the Channel Interleave
  Ways of 2. That is, to figure out the Nvm Dimm that is mapped to the passed in Spa.
  First part of the code computes the logical Socket & Channel and all ifs and else ifs
  are to figure out the actual Socket and the Channel that is in the data structs.

  @param[in] Spa          - Spa for which the socket, channel & Dimm information.
  @param[in] SpaStartAddr - The Start Address of the SAD Interleave set.
  @param[in] ImcWays      - Number of iMCs participating in the interleave set of the SAD Rule
  @param[out] *Soc        - pointer to Socket that this function will return
  @param[out] *Channel    - pointer to channel that this function will return
  @param[out] *Dimm       - Pointer to Dimm that this function will return

  @retval Status      - Return Status

**/
EFI_STATUS
GetDimmInfoFromSpaFor3ChWays(
   UINT64      Spa,
   UINT64      SpaStartAddr,
   UINT8       ImcWays,
   UINT8       *Soc,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT64           AddrDiff;
  UINT32           NumLines = 0; // one based
  UINT32           LineSize;
  UINT32           ImcSize;
  UINT32           NumRotations;
  UINT32           NumBytes;
  UINT32           i;
  UINT8            Socket = 0;
  UINT8            Inx = 0;
  UINT8            Imc;
  UINT8            Ch;
  BOOLEAN          PMemRgn = FALSE;
  // These arrays below helps to determine the start channel when the rotation occurs.
  UINT8            TwoImcWayCh[6]    = { 0, 2, 1, 1, 0, 2 };
  UINT8            FourImcWayCh[12]  = { 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2 };
  UINT8            EightImcWayCh[24] = { 0, 2, 1, 1, 0, 2, 2, 1, 0, 0, 2, 1, 1, 0, 2, 2, 1, 0, 0, 2, 1, 1, 0, 2 };
  //UINT8            SixteeniMCWayCh[48] = { 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2, 1, 2, 0,
  //                                         2, 0, 1, 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2 };

  LineSize = GetChGranularityFromSadBaseAddr(SpaStartAddr);
  if (LineSize == 0) {
    DEBUG((EFI_D_ERROR, "CRP: GetDimmInfoFromSpaFor3ChWays - LineSize returned is 0, returning!\n"));
    return Status;
  }

  ImcSize = mCrInfo.SktGran;
  Imc = 0;

  //
  // How far off this Spa from the SAD Spa Start Address?
  AddrDiff = Spa - SpaStartAddr;

  //
  // How many rotations it is going to take get to this Spa from SAD Spa Start?
  NumRotations = (UINT32)(AddrDiff / ImcSize);

  NumLines = ((AddrDiff % ImcSize) / LineSize);

  // These are the number of bytes past the LineSize boundary
  NumBytes = (AddrDiff % LineSize);

  //
  // Does the SAD Start Address belong to PMEM Region?
  PMemRgn = IsPmemRgn(SpaStartAddr);

  //
  // Now with this we should have the Socket information
  switch (ImcWays) {
  case ONE_IMC_WAY:
    // ImcWays is 1 and 3ch ways
    NumLines = (UINT32)(AddrDiff / LineSize);
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_3WAY, 0, 0, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case TWO_IMC_WAY:
    // If ImcWays = 2, then it is only one socket, socket 0.
    if ((NumRotations % ImcWays)) {
      // If so it is in iMC1
      Imc = 1;
    }
    Inx = ((NumRotations  % (CH_3WAY * ImcWays)) / ImcWays);
    if (Imc == 1) {
      Inx += 3;
    }
    Ch = TwoImcWayCh[Inx];
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_3WAY, 0, Imc, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case FOUR_IMC_WAY:
    // 4 iMC Ways interleave,
    if (((NumRotations % ImcWays) == 2) || ((NumRotations % ImcWays) == 3)) {
      Socket = 1;
      Inx = 6;
      if ((NumRotations % ImcWays) == 3) {
        Imc = 1;
      }
    }
    else if (((NumRotations % ImcWays) == 0) || ((NumRotations % ImcWays) == 1)) {
      Socket = 0;
      Inx = 0;
      if ((NumRotations % ImcWays) == 1) {
        Imc = 1;
      }
    }
    Inx += ((NumRotations  % (CH_3WAY * ImcWays)) / ImcWays);
    if (Imc == 1) {
      Inx += 3;
    }
    Ch = FourImcWayCh[Inx];
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_3WAY, Socket, Imc, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case EIGHT_IMC_WAY:
    // 8 iMC Way interleave
    if (((NumRotations % ImcWays) == 6) || ((NumRotations % ImcWays) == 7)) {
      Inx = 18;
      Socket = 3;
      if ((NumRotations % ImcWays) == 7) {
        Imc = 1;
      }
    }
    else if (((NumRotations % ImcWays) == 4) || ((NumRotations % ImcWays) == 5)) {
      Socket = 2;
      Inx = 12;
      if ((NumRotations % ImcWays) == 5) {
        Imc = 1;
      }
    }
    else if (((NumRotations % ImcWays) == 2) || ((NumRotations % ImcWays) == 3)) {
      Socket = 1;
      Inx = 6;
      if ((NumRotations % ImcWays) == 3) {
        Imc = 1;
      }
    }
    else if (((NumRotations % ImcWays) == 0) || ((NumRotations % ImcWays) == 1)) {
      Socket = 0;
      Inx = 0;
      if ((NumRotations % ImcWays) == 1) {
        Imc = 1;
      }
    }
    Inx += ((NumRotations  % (CH_3WAY * ImcWays)) / ImcWays);
    if (Imc == 1) {
      Inx += 3;
    }
    Ch = EightImcWayCh[Inx];
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_3WAY, Socket, Imc, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  Routine Description: This function is responsible for getting Socket, Channel and Dimm
  information based on the given System Physical Address when the Channel Interleave
  Ways of 2. That is, to figure out the Nvm Dimm that is mapped to the passed in Spa.
  First part of the code computes the logical Socket & Channel and all ifs and else ifs
  are to figure out the actual Socket and the Channel that is in the data structs.

  @param[in] Spa          - Spa for which the socket, channel & Dimm information.
  @param[in] SpaStartAddr - The Start Address of the SAD Interleave set.
  @param[in] ImcWays      - Number of iMCs participating in the interleave set of the SAD Rule
  @param[out] *Soc        - pointer to Socket that this function will return
  @param[out] *Channel    - pointer to channel that this function will return
  @param[out] *Dimm       - Pointer to Dimm that this function will return

  @retval Status      - Return Status

**/
EFI_STATUS
GetDimmInfoFromSpaFor2ChWays(
   UINT64      Spa,
   UINT64      SpaStartAddr,
   UINT8       ImcWays,
   UINT8       *Soc,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT64           AddrDiff;
  UINT32           NumLines = 0; // one based
  UINT32           LineSize;
  UINT32           ImcSize;
  UINT32           NumRotations;
  UINT32           NumBytes;
  UINT32           i;
  UINT8            Socket = 0;
  UINT8            Imc;
  UINT8            Ch;
  BOOLEAN          PMemRgn = FALSE;

  LineSize = GetChGranularityFromSadBaseAddr(SpaStartAddr);
  if (LineSize == 0) {
    DEBUG((EFI_D_ERROR, "CRP: GetDimmInfoFromSpaFor2ChWays - LineSize returned is 0, returning!\n"));
    return Status;
  }

  ImcSize = mCrInfo.SktGran;
  Imc = 0;

  //
  // How far off this Spa from the SAD Spa Start Address?
  AddrDiff = Spa - SpaStartAddr;

  //
  // How many rotations it is going to take get to this Spa from SAD Spa Start?
  NumRotations = (UINT32)(AddrDiff / ImcSize);

  NumLines = ((AddrDiff % ImcSize) / LineSize);

  // These are the number of bytes past the LineSize boundary
  NumBytes = (AddrDiff % LineSize);

  //
  // Does the SAD Start Address belong to PMEM Region?
  PMemRgn = IsPmemRgn(SpaStartAddr);

  //
  // Now with this we should have the Socket information
  switch (ImcWays) {
  case ONE_IMC_WAY:
    NumLines = (UINT32)(AddrDiff / LineSize);
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_2WAY, 0, 0, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case TWO_IMC_WAY:
    if ((NumRotations % ImcWays)) {
      // If it so, then pertinent information is in the second 2 dimms of the set of 4 Dimm AEP_DIMM struct.
      Imc = 1;
    }
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_2WAY, 0, Imc, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case FOUR_IMC_WAY:
    // 4 Imc Ways interleave,
    if (((NumRotations % ImcWays) == 2) || ((NumRotations % ImcWays) == 3)) {
      Socket = 1;
      if ((NumRotations % ImcWays) == 3) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 0) || ((NumRotations % ImcWays) == 1)) {
      Socket = 0;
      if ((NumRotations % ImcWays) == 1) {
        Imc = 1;
      }
    }
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_2WAY, Socket, Imc, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case EIGHT_IMC_WAY:
    // 8 Imc Way interleave
    if (((NumRotations % ImcWays) == 6) || ((NumRotations % ImcWays) == 7)) {
      Socket = 3;
      if ((NumRotations % ImcWays) == 7) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 4) || ((NumRotations % ImcWays) == 5)) {
      Socket = 2;
      if ((NumRotations % ImcWays) == 5) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 2) || ((NumRotations % ImcWays) == 3)) {
      Socket = 1;
      if ((NumRotations % ImcWays) == 3) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 0) || ((NumRotations % ImcWays) == 1)) {
      Socket = 0;
      if ((NumRotations % ImcWays) == 1) {
        Imc = 1;
      }
    }
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1) {
        Ch = 0;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_2WAY, Socket, Imc, Ch, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  }
  //
  // If it gets here, return EFI_INVALID_PARAMETER Status.
  return EFI_INVALID_PARAMETER;
}

/**

  Routine Description: This function is responsible for getting Socket, Channel and Dimm
  information based on the given System Physical Address when the Channel Interleave
  Ways of 1. That is, to figure out the Nvm Dimm that is mapped to the passed in Spa.
  First part of the code computes the logical Socket & Channel and all ifs and else ifs
  are to figure out the actual Socket and the Channel that is in the data structs.

  @param[in] Spa          - Spa for which the socket, channel & Dimm information.
  @param[in] SpaStartAddr - The Start Address of the SAD Interleave set.
  @param[in] ImcWays      - Number of iMCs participating in the interleave set of the SAD Rule
  @param[out] *Soc        - pointer to Socket that this function will return
  @param[out] *Channel    - pointer to channel that this function will return
  @param[out] *Dimm       - Pointer to Dimm that this function will return

  @retval Status      - Return Status

**/
EFI_STATUS
GetDimmInfoFromSpaFor1ChWays(
   UINT64      Spa,
   UINT64      SpaStartAddr,
   UINT8       ImcWays,
   UINT8       *Soc,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT64           AddrDiff;
  UINT32           NumLines = 0; // one based
  UINT32           LineSize;
  UINT32           ImcSize;
  UINT32           NumRotations, NumBytes;
  UINT8            Socket = 0;
  UINT8            Imc;
  BOOLEAN          PMemRgn = FALSE;

  LineSize = GetChGranularityFromSadBaseAddr(SpaStartAddr);
  if (LineSize == 0) {
    DEBUG((EFI_D_ERROR, "CRP: GetDimmInfoFromSpaFor1ChWays - LineSize returned is 0, returning!\n"));
    return (Status);
  }

  ImcSize = mCrInfo.SktGran;
  Imc = 0;

  //
  // How far off this Spa from the SAD Spa Start Address?
  AddrDiff = Spa - SpaStartAddr;

  //
  // How many rotations it is going to take get to this Spa from SAD Spa Start?
  NumRotations = (UINT32)(AddrDiff / ImcSize);

  NumLines = ((AddrDiff % ImcSize) / LineSize);

  // These are the number of bytes past the LineSize boundary
  NumBytes = (AddrDiff % LineSize);

  //
  // Does the SAD Start Address belong to PMEM Region?
  PMemRgn = IsPmemRgn(SpaStartAddr);

  //
  // Now with this we should have the Socket information
  switch (ImcWays) {
  case ONE_IMC_WAY:
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_1WAY, 0, 0, 0, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case TWO_IMC_WAY:
    // 2 Imc Ways
    if ((NumRotations % ImcWays)) {
      // If so it is in Imc1
      Imc = 1;
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_1WAY, 0, Imc, 0, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case FOUR_IMC_WAY:
    // Four Imc Ways
    if (((NumRotations % ImcWays) == 2) || ((NumRotations % ImcWays) == 3)) {
      Socket = 1;
      if ((NumRotations % ImcWays) == 3) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 0) || ((NumRotations % ImcWays) == 1)) {
      Socket = 0;
      if ((NumRotations % ImcWays) == 1) {
        Imc = 1;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_1WAY, Socket, Imc, 0, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  case EIGHT_IMC_WAY:
    // 8 Imc Way interleave
    if (((NumRotations % ImcWays) == 6) || ((NumRotations % ImcWays) == 7)) {
      Socket = 3;
      if ((NumRotations % ImcWays) == 7) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 4) || ((NumRotations % ImcWays) == 5)) {
      Socket = 2;
      if ((NumRotations % ImcWays) == 5) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 2) || ((NumRotations % ImcWays) == 3)) {
      Socket = 1;
      if ((NumRotations % ImcWays) == 3) {
        Imc = 1;
      }
    } else if (((NumRotations % ImcWays) == 0) || ((NumRotations % ImcWays) == 1)) {
      Socket = 0;
      if ((NumRotations % ImcWays) == 1) {
        Imc = 1;
      }
    }
    // Need to get the actual mapped channel
    Status = VirtualToPhysicalDimmInfo(SpaStartAddr, PMemRgn, CH_1WAY, Socket, Imc, 0, Soc, Channel);
    if (!EFI_ERROR(Status)) {
      Status = GetChannelDimm(*Soc, *Channel, Dimm);
    }
    return Status;
    break;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  Routine Description: This function is responsible for getting Socket,
  Channel and Dimm information based on the given System Physical Address.
  That is, to figure out the Nvm Dimm that is mapped to the passed in Spa.

  @param[in]  Spa          - Spa
  @param[out] *SadSpaBase  - Spa base address
  @param[out] *Soc         - Socket that this function will return
  @param[out] *Channel     - Channel that this function will return
  @param[out] *Dimm        - Dimm that this function will return

  @return Return Status

**/
EFI_STATUS
GetDimmInfoFromSpa(
  UINT64      Spa,
  UINT64      *SadSpaBase,
  UINT8       *Soc,
  UINT8       *Channel,
  UINT8       *Dimm
  )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT8            Socket;
  UINT8            iMCWays, i;
  UINT64           SpaStart, SpaEnd;
  struct SADTable  *pSAD;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    //
    // Traverse thru all SAD entries to check for the Spa being in the range of one
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Socket].SAD[i];

      if (pSAD->Enable == 0 || pSAD->local == 0) {
        continue;
      }

      //
      // If the Sad type is for an NVMDIMM..
      if ((pSAD->type == MEM_TYPE_PMEM) ||
          (pSAD->type == MEM_TYPE_PMEM_CACHE) ||
          (pSAD->type == MEM_TYPE_BLK_WINDOW) ||
          (pSAD->type == MEM_TYPE_CTRL)) {

        if (i == 0) {
          SpaStart = 0;
        } else {
          SpaStart = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);
        }

        SpaEnd = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i].Limit << BITS_64MB_TO_BYTES);
        //
        // If the passed in SPA falls with in this SAD SPA Range
        if ((Spa >= SpaStart) && (Spa <= SpaEnd)) {
          //
          // Found the Sad rule with in which the passed in Spa has mapping, now next to figure out to which dimm
          // this Spa is mapped into.
          //
          // One of the return values is SadSpaBase address
          *SadSpaBase = SpaStart;
          iMCWays = GetImcWays(Socket, i);

          switch (GetChWays(Socket, i)) {
          case CH_1WAY:
            Status = GetDimmInfoFromSpaFor1ChWays(Spa, SpaStart, iMCWays, Soc, Channel, Dimm);
            break;
          case CH_2WAY:
            Status = GetDimmInfoFromSpaFor2ChWays(Spa, SpaStart, iMCWays, Soc, Channel, Dimm);
            break;
          case CH_3WAY:
            Status = GetDimmInfoFromSpaFor3ChWays(Spa, SpaStart, iMCWays, Soc, Channel, Dimm);
            break;
          }
        }
      }
    }
  }
  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CRP: GetDimmInfoFromSpa - Found Dimm Info From Spa (0x%llx), Base = 0x%llx, Soc = %d, Channel = %d, Dimm = %d\n",
      Spa, *SadSpaBase, *Soc, *Channel, *Dimm));
  } else {
    DEBUG((EFI_D_ERROR, "CRP: GetDimmInfoFromSpa - Spa (0x%llx) Failed\n", Spa));
  }
  return (Status);
}

//
// Note: The code from all translation routines that converts Dpa to Spa (and vice-versa),
// have been tested with another tool and all known issues have been addressed with these
// functions.
//

/**

  Routine Description: This function converts given Dpa
  to Spa and also populates the socket channel and dimm information with
  only system phy address as the input.
  CR protocol function.

  @param[in]  Spa        - System Physical address SPA to be translated
  @param[out] *Skt       - socket number of the given SPA
  @param[out] *Ch        - channel number of the given SPA
  @param[out] *Dimm      - dimm corresponding to the given SPA
  @param[out] *Dpa       - Device physical address

  @return EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
SpaToNvmDpa (
  UINT64       Spa,
  UINT8        *Skt,
  UINT8        *Ch,
  UINT8        *Dimm,
  UINT64       *Dpa
  )
{
  UINT64        SpaStart;
  EFI_STATUS    Status = EFI_NOT_FOUND;
  AEP_DIMM      *NvmDimm = NULL;

  if (!IsSystemWithAepDimm ()) {
    return Status;
  }

  Status = GetDimmInfoFromSpa (Spa, &SpaStart, Skt, Ch, Dimm);
  if (Status == EFI_SUCCESS) {
    NvmDimm = GetDimm (*Skt, *Ch, *Dimm);
  }

  if (NvmDimm == NULL) {
    Status = EFI_NOT_FOUND;
    return Status;
  }

  Status = XlateSpaToDpaOffset (NvmDimm, SpaStart, Spa, Dpa);
  return Status;
}

/**

Routine Description: GetRegIndex will return the index of the NVMDIMM
Mailbox Register that are #defined in the NVMCTLR Register header.

Arguments.

  @param pDimm       - Pointer to AEP_DIMM structure, this will
                     have the Dimm struct.
  @param RegSPA      - Register that is in MBSpa.
  @retval Index of the register if found, else -1.

**/
INT16
GetRegIndex(
   AEP_DIMM  *pDimm,
   UINT64    Reg
   )
{
  INT16      Index;

  switch (Reg) {
  case MB_SMM_CMD_FNV_DPA_MAPPED_0_REG:
    // Command Register
    Index = MB_REG_COMMAND;
    break;
  case MB_SMM_NONCE_0_FNV_DPA_MAPPED_0_REG:
    Index = MB_REG_NONCE0;
    break;
  case MB_SMM_NONCE_1_FNV_DPA_MAPPED_0_REG:
    Index = MB_REG_NONCE1;
    break;
  case MB_SMM_STATUS_FNV_DPA_MAPPED_0_REG:
    Index = MB_REG_STATUS;
    break;
  default:
    Index = -1;
    break;
  }
  return (Index);
}

/**

Routine Description: GetSPARegIndex will return the index of the
NVMDIMM Mailbox Register (in SPA) from the pDimm structure.

arguments.

  @param pDimm       - Pointer to AEP_DIMM structure, this will
                     have the Dimm struct.
  @param RegSPA      - Register that is in MBSpa.
  @retval Index of the register if found, else -1.

**/
INT16
GetSPARegIndex(
   AEP_DIMM  *pDimm,
   UINT64    RegSPA
   )
{
  CR_MAILBOX *MBSpa;

  MBSpa = &pDimm->MailboxSPA;

  if((UINT64)MBSpa->pCommand == RegSPA) {
    return (MB_REG_COMMAND);
  } else if ((UINT64)MBSpa->pNonce0 == RegSPA) {
    return (MB_REG_NONCE0);
  } else if ((UINT64)MBSpa->pNonce1 == RegSPA) {
    return (MB_REG_NONCE1);
  } else if ((UINT64)MBSpa->pStatus == RegSPA) {
    return (MB_REG_STATUS);
  }

  return (-1);
}

/**

Routine Description: GetDpaFromRegIndex will return the NVMDIMM
Mailbox Register (in DPA) for the corresponding Index in NVMDIMM
Mailbox in SPA from the pDimm structure.

arguments.

  @param pDimm       - Pointer to AEP_DIMM structure, this will
                       have the Dimm struct.
  @param Index       - Index of the register in Spa.
  @param *RDpa       - pointer that will contain the Dpa
                       Register.
  @retval EFI_STATUS - Status of the conversion effort

**/
EFI_STATUS
GetDpaFromRegIndex(
   AEP_DIMM  *pDimm,
   INT16     Index,
   UINT64    *RDpa
   )
{
  CR_MAILBOX *MBDpa;
  EFI_STATUS Status = EFI_SUCCESS;

  MBDpa = &pDimm->MailboxDPA;

  if(Index == MB_REG_COMMAND) {
    *RDpa = (UINT64)MBDpa->pCommand;
  } else if(Index == MB_REG_NONCE0) {
    *RDpa = (UINT64)MBDpa->pNonce0;
  } else if(Index == MB_REG_NONCE1) {
    *RDpa = (UINT64)MBDpa->pNonce1;
  } else if(Index == MB_REG_STATUS) {
    *RDpa = (UINT64)MBDpa->pStatus;
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  return (Status);
}

/**

  Routine Description: Returns the Index number of the NVMDIMM for a given
    Socket and Channel.

  @param[in] Socket      - Socket Number
  @param[in] Ch          - DDR Channel ID in socket

  @return Index

**/
EFI_STATUS
GetDimmIndex(
  UINT8  Socket,
  UINT8  Ch,
  UINT8  *Index
  )
{
  EFI_STATUS    Status;
  AEP_DIMM      *NvmDimmPtr;
  UINT8         Dimm;
  UINT8         i;

  if (Index == NULL || Socket >= MAX_SOCKET || Ch >= MAX_CH) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetChannelDimm(Socket, Ch, &Dimm);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  for (i = 0; i < mCrInfo.NumAepDimms; i++) {
    NvmDimmPtr = &mCrInfo.AepDimms[i];
    if ((NvmDimmPtr->SocketId == Socket) && (NvmDimmPtr->Ch == Ch) && (NvmDimmPtr->Dimm == Dimm)) {
      *Index = i;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**

  Routine Description: Returns the pointer to the Dimm structure if found,
    based on the Index in mCrInfo.AepDimms array.

  @param[in] Index       - Index Number in mCrInfo.AepDimms array

  @return Pointer to AEP_DIMM structure or NULL if Index is out of bounds.

**/
AEP_DIMM *
GetDimmByIndex(
  UINT8  Index
  )
{
  AEP_DIMM *NvmDimmPtr = NULL;

  if (Index < mCrInfo.NumAepDimms) {
    NvmDimmPtr = &mCrInfo.AepDimms[Index];
  }

  return NvmDimmPtr;
}

/**

  Routine Description: GetDimm will return the pointer to the Dimm
    structure if found based on socket, channel and dimm arguments.

  @param[in] Socket      - Socket Number
  @param[in] Ch          - DDR Channel ID
  @param[in] Dimm        - DIMM number

  @return AEP_DIMM structure for given Dimm or NULL if not found

**/
AEP_DIMM *
GetDimm(
  UINT8     Socket,
  UINT8     Ch,
  UINT8     Dimm
  )
{
  AEP_DIMM      *NvmDimmPtr = NULL;
  UINT8         i;

  for (i = 0; i < mCrInfo.NumAepDimms; i++) {
    NvmDimmPtr = &mCrInfo.AepDimms[i];
    if ((NvmDimmPtr->SocketId == Socket) && (NvmDimmPtr->Ch == Ch) && (NvmDimmPtr->Dimm == Dimm)) {
      return NvmDimmPtr;
    }
  }
  return NULL;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrReadCfgMem(
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm,
   UINT32    reg,
   UINT32    *data
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  INT16           Index;
  UINT64          RDpa;

  pDimm = GetDimm(socket, ch, dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  //
  //
  Index = GetRegIndex(pDimm, (UINT64)reg);
  if (Index == -1) {
    Index = GetSPARegIndex(pDimm, (UINT64)reg);
  }

  if (Index != -1) {
    Status = GetDpaFromRegIndex(pDimm, Index, &RDpa);
  } else {
    Status = XlateSpaToDpaOffset(pDimm, pDimm->SADSpaBase, (UINT64)reg, &RDpa);
  }

  if (Status != EFI_SUCCESS) {
    return (EFI_INVALID_PARAMETER);
  }
  //
  // Now that we have Dpa for this register, go and read it.
  *data = *(volatile UINT32 *)(UINTN)(RDpa);
  return Status;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrWriteCfgMem(
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm,
   UINT32    reg,
   UINT32    data
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  INT16           Index;
  UINT64          RDpa;

  pDimm = GetDimm(socket, ch, dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  //
  //
  Index = GetRegIndex(pDimm, (UINT64)reg);
  if (Index == -1) {
    Index = GetSPARegIndex(pDimm, (UINT64)reg);
  }

  if (Index != -1) {
    Status = GetDpaFromRegIndex(pDimm, Index, &RDpa);
  } else {
    Status = XlateSpaToDpaOffset(pDimm, pDimm->SADSpaBase, (UINT64)reg, &RDpa);
  }

  if (Status != EFI_SUCCESS) {
    return (EFI_INVALID_PARAMETER);
  }
  //
  // Now that we have Dpa for this register, go and write the data to it.
  *(volatile UINT32 *)(UINTN)(RDpa) = data;
  return Status;
}


/**

Routine Description: Performs a durable Cache Line Write

  @param CsrAddress   - Mailbox CSR Address
  @param Data         - Data to be written
  @param DataSize     - Size of Data to be written

DurableCacheLineWrite( addr, data ) performs the following:

(1) SFENCE
(2) UC Write of cache line of write data using CSR addr
(3) SFENCE
(4) UC Read of same cache line as step b, using CSR addr (See Note* below)

Note*: IMC will block new requests to the same address as a pending request in the buffer.
       Read forces previous write data to be flushed from WPQ to SXPs Durability Domain

**/
VOID
DurableCacheLineWrite (
  UINTN   CsrAddress,
  UINT64  Data,
  UINT8   DataSize
  )
{
  volatile UINT64 TempData;

#ifdef __GNUC__
  asm("sfence");
#else
  _mm_sfence();
#endif
  switch (DataSize) {
    case (sizeof (UINT8)):
      *(UINT8 *) CsrAddress = (UINT8)Data;
    break;
    case (sizeof (UINT16)):
      *(UINT16 *) CsrAddress = (UINT16)Data;
    break;
    case (sizeof (UINT32)):
      *(UINT32 *) CsrAddress = (UINT32)Data;
    break;
    case (sizeof (UINT64)):
      *(UINT64 *) CsrAddress = Data;
    break;
    default:
    break;
  }

#ifdef __GNUC__
  asm("sfence");
#else
  _mm_sfence();
#endif
  TempData = *(UINT64 *) CsrAddress;
}

/**

Routine Description: Issues a WPQ Flush

  @param SocketId     - Socket number
  @param ImcId        - Memory Controller number
  @param SecondSfence - Indicates whether to issue second SFENCE

**/
VOID
IssueWpqFlush (
  UINT16  SocketId,
  UINT16  ImcId,
  BOOLEAN SecondSfence
  )
{
  UINT64  *FlushHintAddress;

  FlushHintAddress = (UINT64 *)mWpqFlushHintAddress[SocketId * MAX_IMC + ImcId];

  if (FlushHintAddress) {
    //
    // Execute SFENCE, WPQ Flush, SFENCE
#ifdef __GNUC__
    asm("sfence");
#else
    _mm_sfence();
#endif
    *FlushHintAddress = 0xffffffffffffffff;
    if (SecondSfence) {
#ifdef __GNUC__
      asm("sfence");
#else
      _mm_sfence();
#endif
    }
  }
}

/**

Routine Description: Writes data from the buffer to small input payload

  @param[in] *NvmDimm       - AEP dimm structure pointer
  @param[in] *Buffer        - buffer to write
  @param[in] BufferSize     - buffer size in bytes
  @param[in] UseOsMailbox   - if TRUE uses OS mailbox and BIOS mailbox otherwise.

  @retval EFI_SUCCESS           Operation finished successfully
  @retval EFI_INVALID_PARAMETER NvmDimm or Buffer are NULL, BufferSize is 0 or
                                greater than Payload size.

**/
EFI_STATUS
WriteToSmallPayload (
  AEP_DIMM    *NvmDimm,
  CONST VOID  *Buffer,
  UINTN       BufferSize,
  BOOLEAN     UseOsMailbox
  )
{
  CR_MAILBOX      *Mailbox;
  UINT8           PayloadIndex;
  UINT32          Payload[NUM_PAYLOAD_REG];
  UINT8           PayloadLen;

  if ((NvmDimm    == NULL) ||
      (Buffer     == NULL) ||
      (BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize > sizeof (Payload)) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (Payload, Buffer, BufferSize);

  Mailbox = GetMailbox (NvmDimm, UseOsMailbox);

  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush (NvmDimm->SocketId, NvmDimm->ImcId, TRUE);
  }

  //
  // We need to fit the Buffer into 32 bit payload registers, so
  // PayloadLen should be the ceiling of BufferSize divided by size of
  // single payload register
  //
  PayloadLen = (UINT8)((BufferSize / sizeof (Payload[0])) +
                      ((BufferSize % sizeof (Payload[0])) != 0));

  for (PayloadIndex = 0; PayloadIndex < PayloadLen; PayloadIndex++) {

    if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
      *(UINT32 *)(UINTN)Mailbox->pInPayload[PayloadIndex] = Payload[PayloadIndex];
    } else {
      DurableCacheLineWrite (
        (UINTN)Mailbox->pInPayload[PayloadIndex],
        Payload[PayloadIndex],
        sizeof (Payload[0])
        );
    }

  }

  return EFI_SUCCESS;
}

/**

Routine Description: Reads data from small output payload to the buffer

  @param[in]  *NvmDimm       - AEP dimm structure pointer
  @param[out] *Buffer        - buffer to read to
  @param[in]  BufferSize     - buffer size in bytes and data to read
  @param[in]  UseOsMailbox   - if TRUE uses OS mailbox and BIOS mailbox otherwise.

  @retval EFI_SUCCESS           Operation finished successfully
  @retval EFI_INVALID_PARAMETER NvmDimm or Buffer are NULL, BufferSize is 0 or
                                greater than Payload size.

**/
EFI_STATUS
ReadFromSmallPayload (
  AEP_DIMM  *NvmDimm,
  VOID      *Buffer,
  UINTN     BufferSize,
  BOOLEAN   UseOsMailbox
  )
{
  CR_MAILBOX      *Mailbox;
  UINT8           PayloadIndex;
  UINT32          Payload[NUM_PAYLOAD_REG];
  UINT8           PayloadLen;

  if ((NvmDimm    == NULL) ||
      (Buffer     == NULL) ||
      (BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize > sizeof (Payload)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // We need to fit the 32 bit payload registers into Buffer, so
  // PayloadLen should be the ceiling of BufferSize divided by size of
  // single payload register
  //
  PayloadLen = (UINT8)((BufferSize / sizeof (Payload[0])) +
                      ((BufferSize % sizeof (Payload[0])) != 0));

  Mailbox = GetMailbox (NvmDimm, UseOsMailbox);

  for (PayloadIndex = 0; PayloadIndex < PayloadLen; PayloadIndex++) {
    Payload[PayloadIndex] = *(UINT32 *)(UINTN)Mailbox->pOutPayload[PayloadIndex];
  }

  CopyMem (Buffer, Payload, BufferSize);

  return EFI_SUCCESS;
}

/**

Routine Description: SendFnvCommand sends a NVMCTLR command as
specified in the Opcode passed in and returns the status back.

  @param pDimm         - Pointer to the Dimm struct
  @param Opcode        - Opcode of the command to be sent
  @retval EFI_STATUS   - success if command sent

**/
EFI_STATUS
SendFnvCommand(
   PAEP_DIMM  pDimm,
   UINT16     Opcode
   )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  CR_MAILBOX                            *pMBox;
  UINT32                                TckCntr = DOORBELL_TIMEOUT;
  MB_SMM_CMD_FNV_SPA_MAPPED_0_STRUCT    *pSmmCmdReg;
  MB_SMM_CMD_FNV_SPA_MAPPED_0_STRUCT    SmmCmdReg;
  MB_SMM_STATUS_FNV_SPA_MAPPED_0_STRUCT *pSmmStatusReg;
  UINT8                                 i = 0;

  SmmCmdReg.Data = 0x0;

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  for (i = 0;  i < CR_OUT_PAYLOAD_NUM; i++) {
    if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
      ZeroMem ((UINT8 *) (UINTN)pMBox->pOutPayload[i], sizeof (UINT32));
    } else {
      DurableCacheLineWrite(pMBox->pOutPayload[i], 0x00, sizeof (UINT32));
    }
  }

  //
  // Set the Opcode for what ever command is desired.
  pSmmCmdReg = (MB_SMM_CMD_FNV_SPA_MAPPED_0_STRUCT *)pMBox->pCommand;

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    //
    // Per NVMCTLR Spec, need to toggle the Sequence Bit, everytime we send a command
    // so that F/W knows if the command is a retry or not.
    (pSmmCmdReg->Bits.sequence_bit) ? (pSmmCmdReg->Bits.sequence_bit = 0) : (pSmmCmdReg->Bits.sequence_bit = 1);

    pSmmCmdReg->Bits.opcode = Opcode;

    //
    // If the Security Nonce has already been set, copy the Nonce to the Nonce0 register (and only if we use BIOS Mailbox)
    if ((pDimm->NonceSet) && (!mUseOSMailbox)) {
      *(UINT64 *)(UINTN)pMBox->pNonce0 = pDimm->Nonce;
    }

    //
    // Set the doorbell to tell the controller that the command is ready to be processed.
    pSmmCmdReg->Bits.doorbell = 1;

    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, FALSE);
  } else {
    //
    // Per NVMCTLR Spec, need to toggle the Sequence Bit, everytime we send a command
    // so that F/W knows if the command is a retry or not.
    (pSmmCmdReg->Bits.sequence_bit) ? (SmmCmdReg.Bits.sequence_bit = 0) : (SmmCmdReg.Bits.sequence_bit = 1);

    SmmCmdReg.Bits.opcode = Opcode;

    DurableCacheLineWrite((UINTN)pSmmCmdReg, SmmCmdReg.Data, sizeof (UINT64));
    //
    // If the Security Nonce has already been set, copy the Nonce to the Nonce0 register (and only if we use BIOS Mailbox)
    if ((pDimm->NonceSet) && (!mUseOSMailbox)) {
      DurableCacheLineWrite((UINTN)pMBox->pNonce0, pDimm->Nonce, sizeof (UINT64));
    }

    //
    // Set the doorbell to tell the controller that the command is ready to be processed.
    SmmCmdReg.Bits.doorbell = 1;
    DEBUG((EFI_D_ERROR, "CRP: NVMCTL Command SmmCmdReg.Data =0x%lx\n", SmmCmdReg.Data));
    DurableCacheLineWrite((UINTN)pSmmCmdReg, SmmCmdReg.Data, sizeof (UINT64));
  }

  //
  // Poll every 10 us till the doorbell is clear and read the status register
  DEBUG((EFI_D_ERROR, "NVMCTL Waiting for doorbell to clear: "));
  while (pSmmCmdReg->Bits.doorbell && TckCntr) {
    MicroSecondDelay(10);
    if (TckCntr % 100 == 0) {
      DEBUG((EFI_D_ERROR, "%d ", TckCntr/100));
    }
    TckCntr--;
  }

  // Clear Nonce, in case it wasn't cleared by the controller
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    *(UINT64 *)(UINTN)pMBox->pNonce0 = 0;
  } else {
    DurableCacheLineWrite((UINTN)pMBox->pNonce0, 0, sizeof (UINT64));
  }

  // If doorbell is still set, return command failure status
  if (pSmmCmdReg->Bits.doorbell) {
    DEBUG((EFI_D_ERROR, "\nNVMCTL Mailbox Failure: Doorbell Still Set\n"));
    return EFI_DEVICE_ERROR;
  } else {
    DEBUG((EFI_D_ERROR, "- Doorbell cleared\n"));
  }

  //
  // Check the status register for the status of the command completion
  pSmmStatusReg = (MB_SMM_STATUS_FNV_SPA_MAPPED_0_STRUCT *)pMBox->pStatus;

  Status = MbStatusDecode (pSmmStatusReg);

  return Status;
}

/**
Routine Description: SendFnvIdentifyDevice sends Identify
command to the Fnv F/W.

  @param socket        - Socket Number
  @param ch            - DDR Channel ID
  @param dimm          - DIMM number
  @retval EFI_STATUS   - success if command sent

**/
EFI_STATUS
SendFnvIdentifyDevice(
   UINT8    Socket,
   UINT8    Ch,
   UINT8    Dimm
   )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  PAEP_DIMM    pDimm = NULL;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);
  }
  Status = SendFnvCommand(pDimm, IDENTIFY_DIMM);

  return Status;
}

/**

Routine Description: InitDIMMInfoFromHOB initializes DIMM
information from HOB.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval EFI_STATUS - success if command sent

**/
EFI_STATUS
InitDIMMInfoFromHOB(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pDimm->VendorID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].VendorID;
  pDimm->DeviceID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].DeviceID;
  pDimm->RevisionID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].RevisionID;
  pDimm->subSysVendorID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].subSysVendorID;
  pDimm->subSysDeviceID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].subSysDeviceID;
  pDimm->subSysRevisionID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].subSysRevisionID;
  pDimm->ManufLocation = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].manufacturingLocation;
  pDimm->ManufDate = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].manufacturingDate;
  pDimm->FormatInterfaceCode = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].InterfaceFormatCode;

  pDimm->SerialNum = (((UINT32)mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[3]) << 24  |
                    ((UINT32)mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[2]) << 16  |
                    ((UINT32)mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[1]) << 8 |
                     mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[0]);

  DEBUG((EFI_D_ERROR, "CRP: VendorID = 0x%X, DeviceID = 0x%X\n", pDimm->VendorID, pDimm->DeviceID));
  DEBUG((EFI_D_ERROR, "CRP: Dimm Serial Number = 0x%X\n", pDimm->SerialNum));
  DEBUG((EFI_D_ERROR, "CRP: FormatInterfaceCode = 0x%X\n", pDimm->FormatInterfaceCode));

  return (Status);
}

/**

Routine Description: InitAllDimmsFromHOB Initializes
all Aep Dimms installed in the system using the HOB,
basically to gather VendorID & DeviceID of all these dimms and
stores in the pDimm Struct.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval EFI_STATUS - success if command sent

**/
VOID
InitAllDimmsFromHOB(
   VOID
   )
{
  UINT8           i;
  PAEP_DIMM       pDimm;

  for (i = 0; i < mCrInfo.NumAepDimms; i++) {
    pDimm = &mCrInfo.AepDimms[i];
    InitDIMMInfoFromHOB((UINT8)pDimm->SocketId, pDimm->Ch, pDimm->Dimm);
  }
}

/**

Routine Description: SendSecurityNonceToAllDimms sends a
security nonce to all Aep Dimms installed in the system.

  @param ResetNonces      - Specifes whether to reset all Nonces

**/
VOID
SendSecurityNonceToAllDimms(
  BOOLEAN ResetNonces
  )
{
  UINT8           i;
  PAEP_DIMM       pDimm;

  for (i = 0; i < mCrInfo.NumAepDimms; i++) {
    pDimm = &mCrInfo.AepDimms[i];
    if (ResetNonces) {
      pDimm->NonceSet = FALSE;
    }

    SendFnvSecurityNonce((UINT8)pDimm->SocketId, pDimm->Ch, pDimm->Dimm);
  }
}

/**

Routine Description: SendFnvSecurityNonce sends NVMCTLR Security Nonce

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval EFI_STATUS - success if command sent

**/
EFI_STATUS
SendFnvSecurityNonce(
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  UINT64          Nonce;
  CR_MAILBOX      *pMBox;

  DEBUG ((DEBUG_INFO, "CR: setting Nonce for Soc:%x Ch:%x Slot:%x\n", socket, ch, dimm));

  pDimm = GetDimm(socket, ch, dimm);

  if (pDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // If the Nonce is already set for this dimm, just punt
  if (pDimm->NonceSet) {
    return Status;
  }

  pMBox = &pDimm->MailboxSPA;

  //Generate 64bit Security Nonce using RNG
  Status = RdRand64 (&Nonce, TRUE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    //
    // Actaully, the input payload is the Nonce generated in the low 8 bytes.
    *(UINT32 *)(UINTN)pMBox->pInPayload[0] = (UINT32) Nonce;
    *(UINT32 *)(UINTN)pMBox->pInPayload[1] = (UINT32) (Nonce >> 32);
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], (UINT32) Nonce, sizeof (UINT32));
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[1], (UINT32) (Nonce >> 32), sizeof (UINT32));
  }

  Status = SendFnvCommand(pDimm, SET_SECURITY_NONCE);
  if (Status) {
    DEBUG ((DEBUG_ERROR, "CR: Error occurred setting Nonce\n"));
  }

  if (Status == MB_CMD_SUCCESS) {
    // Save the Nonce for future sending of any commands and enable flag to tell that
    // Nonce has been set.
    pDimm->Nonce = Nonce;
    pDimm->NonceSet = TRUE;
  }

  //
  // Send Set Config lockdown for this DIMM
  if (mSystemMemoryMap->MemSetup.optionsNgn & LOCK_NGN_CSR) {
    DEBUG ((DEBUG_INFO, "CR: setting config lockdown for Soc:%x Ch:%x Slot:%x\n", socket, ch, dimm));
    Status = SendFnvSetConfigLockdown(socket, ch, dimm);
    if (Status) {
      DEBUG ((DEBUG_ERROR, "CR: Error occurred sending Set Config lockdown command\n"));
    }
  }

  return Status;
}

/**

  Routine description: This function restores nonces for all Nvm dimms. This is a
  handler for SMI interrupt.

  @param[in] DispatchHandle - not used
  @param[in] DispatchContext - not used
  @param[in,out] CommBuffer - not used
  @param[in,out] CommBufferSize - not used

  @retval EFI_SUCCESS - operation done
*/
EFI_STATUS
RestoreNonces(
  IN            EFI_HANDLE                  DispatchHandle,
  IN      CONST EFI_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT        VOID                        *CommBuffer,      OPTIONAL
  IN OUT        UINTN                       *CommBufferSize   OPTIONAL
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  DEBUG((EFI_D_ERROR, "CR: Restoring Nonces\n"));
  SendSecurityNonceToAllDimms(TRUE);

  return Status;
}

/**

Routine Description: SendFnvSetConfigLockdown sends NVMCTLR the Set Config Lockdown command

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval EFI_STATUS - success if command sent

**/
EFI_STATUS
SendFnvSetConfigLockdown(
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
  )
{
  PAEP_DIMM   pDimm = NULL;
  CR_MAILBOX  *pMBox;

  pDimm = GetDimm(socket, ch, dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = &pDimm->MailboxSPA;

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    *(UINT32 *)(UINTN)pMBox->pInPayload[0] = 0;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], 0, sizeof (UINT32));
  }

  return SendFnvCommand(pDimm, SET_CONFIG_LOCKDOWN);
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrGetLatestErrorLog(
   UINT8    Socket,
   UINT8    Ch,
   UINT8    Dimm,
   UINT8    LogLevel,
   UINT8    LogType,
   UINT8    Count,
   VOID*    LogBuffer
    )
{
  PAEP_DIMM        pDimm;
  UINT16           NumberOfEntry;
  UINT16           Offset;
  UINT8            RequestCount;

  pDimm = GetDimm(Socket, Ch, Dimm);
  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  //Send first command to get total log count
  SendFnvGetErrorLog(Socket, Ch, Dimm, LogLevel, LogType, RETRIEVE_LOG_DATA, 0, 0, LogBuffer);
  NumberOfEntry = *((UINT16*)LogBuffer);

  //Set reqeuest count and offset to get latest n logs. Note that offset 0 always points to oldest data.
  RequestCount = (NumberOfEntry > Count) ? Count : (UINT8)NumberOfEntry;
  Offset = (NumberOfEntry > RequestCount) ? (NumberOfEntry - RequestCount - 1) : 0;

  SendFnvGetErrorLog(Socket, Ch, Dimm, LogLevel, LogType, RETRIEVE_LOG_DATA, Offset, RequestCount, LogBuffer);
  return EFI_SUCCESS;
}

/**

  Routine Description: Gets error logs from NVMCTLR. Uses small payload, so
  request count shouldn't be more than 3 for media logs and 7 for thermal logs.
  FIS 1.3 compatible.

  @param[in] Socket            - Socket Number
  @param[in] Ch                - DDR Channel ID
  @param[in] Dimm              - DIMM Number
  @param[in] LogLevel          - Log Level 0 - low priority 1 - high priority
  @param[in] LogType           - 0 - for media logs, 1 for thermal
  @param[in] LogInfo           - 0 - retrieve log data, 1 - retrieve log info
  @param[in] Sequence          - Reads log from specified sequence number
  @param[in] RequestCount      - Max number of log entries requested for this access
  @param[out] *LogData         - pointer to LogData structure.
                                 Caller must assure this data struct is big enough.

  @retval EFI_INVALID_PARAMETER  - one of parameters is not valid
  @retval EFI_OUT_OF_RESOURCES   - requested number of log entries does not fit in small payload
  @retval EFI_SUCCESS            - success
  @retval other                  - see return codes of SendFnvCommand

**/
EFI_STATUS
SendFnvGetErrorLog (
  UINT8     Socket,
  UINT8     Ch,
  UINT8     Dimm,
  UINT8     LogLevel,
  UINT8     LogType,
  UINT8     LogInfo,
  UINT16    Sequence,
  UINT16    RequestCount,
  VOID      *LogData
  )
{
  EFI_STATUS                        Status;
  AEP_DIMM                          *NvmDimm = NULL;
  UINT32                            EntrySize;
  UINT32                            LogSize;
  NVM_GET_ERROR_LOG_PARAMS_PAYLOAD  Data;

  if ((LogData  == NULL)             ||
      (LogLevel > HIGH_LEVEL_LOG)    ||
      (LogInfo  > RETRIEVE_LOG_INFO) ||
      (LogType  > THERMAL_LOG_REQ)) {
    return EFI_INVALID_PARAMETER;
  }

  EntrySize = (LogType == MEDIA_LOG_REQ) ? sizeof(MEDIA_ERR_LOG) : sizeof(THERMAL_ERR_LOG);

  if (LogInfo == RETRIEVE_LOG_INFO) {
    LogSize = sizeof (MEDIA_ERR_LOG_INFO);
  } else {
    LogSize = ERROR_LOG_HEAD_SIZE + EntrySize * RequestCount;
  }

  if (LogSize > CR_OUT_PAYLOAD_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }

  NvmDimm = GetDimm (Socket, Ch, Dimm);
  if (NvmDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Data, sizeof (Data));
  Data.LogLevel       = LogLevel;
  Data.LogType        = LogType;
  Data.LogInfo        = LogInfo;
  Data.LogPayload     = USE_SMALL_PAYLOAD;
  Data.SequenceNumber = Sequence;
  Data.RequestCount   = RequestCount;

  Status = WriteToSmallPayload (NvmDimm, &Data, sizeof(Data), mUseOSMailbox);
  ASSERT_EFI_ERROR (Status);

  Status = SendFnvCommand (NvmDimm, GET_ERROR_LOG);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = ReadFromSmallPayload (NvmDimm, LogData, LogSize, mUseOSMailbox);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**

  Routine Description: This function will send Log Page Get Long Operation
  Command to the NVMCTLR Controller and return Status of the command sent.

  @param[in] Socket       - Socket Number
  @param[in] Ch           - DDR Channel ID
  @param[in] Dimm         - DIMM number

  @return Status of command sent.

**/
EFI_STATUS
SendFnvGetLongOperationStatus(
  UINT8     Socket,
  UINT8     Ch,
  UINT8     Dimm
  )
{
  EFI_STATUS      Status   = EFI_SUCCESS;
  AEP_DIMM        *NvmDimm = NULL;

  NvmDimm = GetDimm (Socket, Ch, Dimm);

  if (NvmDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush (NvmDimm->SocketId, NvmDimm->ImcId, TRUE);
  }

  Status = SendFnvCommand (NvmDimm, GET_LONG_OPERATION);

  return Status;
}

/**

  Routine description: returns long operation status data for ARS operation.

  @param[in]  Socket    - socket id (0 based)
  @param[in]  Channel   - channel id in socket (0 based)
  @param[in]  Dimm      - dimm id in channel (0 based)
  @param[out] ArsStatus - status for long operation

  @return Status returned by firmware, EFI_INVALID_PARAMETER when dimm not found
          or ArsStatus is NULL.

**/
EFI_STATUS
GetLongOperationStatusDataForArs (
  UINT8         Socket,
  UINT8         Channel,
  UINT8         Dimm,
  ARS_OP_STATUS *ArsStatus
  )
{
  EFI_STATUS                                Status;
  AEP_DIMM                                  *NvmDimm = NULL;
  UINT16                                    PercentComplete;
  NVM_GET_LONG_OPERATION_STATUS_ARS_PAYLOAD Data;
  UINT8                                     i;

  if (ArsStatus == NULL) {
    DEBUG ((DEBUG_ERROR, "CRP ERROR: GetLongOperationStatusDataForArs - bad input parameter!\n"));
    return EFI_INVALID_PARAMETER;
  }

  NvmDimm = GetDimm (Socket, Channel, Dimm);
  if (NvmDimm == NULL) {
    DEBUG ((DEBUG_ERROR, "CRP ERROR: GetLongOperationStatusDataForArs - can't find the given Dimm!\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = SendFnvGetLongOperationStatus (Socket, Channel, Dimm);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "CRP ERROR: GetLongOperationStatusDataForArs - can't get LongOperationStatus!\n"));
    return Status;
  }

  ZeroMem (&Data, sizeof(Data));
  Status = ReadFromSmallPayload (NvmDimm, &Data, sizeof(Data), mUseOSMailbox);

  // Ensure it is ARS Command that the Long Op Status that it has returned
  if (Data.Command != SET_ADDRESS_RANGE_SCRUB) {
    DEBUG ((DEBUG_ERROR, "CRP ERROR: GetLongOperationStatusDataForArs - long operation status is not for SET_ADDRESS_RANGE_SCRUB command!\n"));
    return EFI_NOT_FOUND;
  }

  ZeroMem (ArsStatus, sizeof(*ArsStatus));

  // HiWord of DataPtr contains ARS % Complete in BCD format = XXX
  PercentComplete = Data.PercentComplete;
  ArsStatus->PercentComplete = ((PercentComplete >> 8) & 0xF) * 100 +
                               ((PercentComplete >> 4) & 0xF) * 10 +
                                (PercentComplete & 0xF);

  ArsStatus->EstimatedTime  = Data.Etc;
  ArsStatus->StatusCode     = Data.StatusCode;
  ArsStatus->ErrorsFound    = Data.ErrorsFound;

  if (ArsStatus->StatusCode == MB_CMD_DEVICE_BUSY) {
    ArsStatus->InProgress = TRUE;
  } else {
    ArsStatus->InProgress = FALSE;
  }

  if (Data.MaxDpaErrorAddressReached == 1) {
    ArsStatus->PrematurelyEnded = TRUE;
  } else {
    ArsStatus->PrematurelyEnded = FALSE;
  }

  for (i = 0; i < Data.ErrorsFound; i++) {
    ArsStatus->DpaErrorAddress[i] = Data.DpaErrorAddress[i];
  }

  return Status;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrSetHostAlert(
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm,
   UINT16    hostAlerts[MAX_HOST_TRANSACTIONS]
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;
  UINT8           i;

  pDimm = GetDimm(socket, ch, dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    for (i = 0; i < NUM_PAYLOAD_REG; i++) {
      *(UINT32 *)(UINTN)pMBox->pInPayload[i]= (((UINT32)hostAlerts[2 * i]) | ((UINT32)hostAlerts[(2 * i) + 1] << 16));
    }
  } else {
    for (i = 0; i < NUM_PAYLOAD_REG; i++) {
      DurableCacheLineWrite ((UINTN)pMBox->pInPayload[i], (((UINT32)hostAlerts[2 * i]) | ((UINT32)hostAlerts[(2 * i) + 1] << 16)), sizeof (UINT32));
    }

  }

  Status = SendFnvCommand(pDimm, SET_DDRT_ALERTS);

  return Status;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrGetHostAlert(
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm,
   UINT16    hostAlerts[NUM_HOST_TRANSACTIONS]
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;
  UINT8           i;
  UINT32          payloadData;

  pDimm = GetDimm(socket, ch, dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  Status = SendFnvCommand(pDimm, GET_DDRT_ALERTS);

  if (Status == MB_CMD_SUCCESS) {
    for (i = 0; i < NUM_HOST_TRANSACTIONS; i++) {
      payloadData = *(UINT32 *)(UINTN)(pMBox->pOutPayload[i / 2]);
      hostAlerts[i] = (UINT16) (payloadData >> ((i % 2) * 16));

    }
  }
  return Status;
}

/**

  Routine Description: this function will send
  Set Address Range Scrub command to NVMCTLR.
  CR protocol function.

  @param[in] Socket             - Socket Number
  @param[in] Ch                 - DDR Channel ID
  @param[in] Dimm               - DIMM number
  @param[in] DpaStartAddress    - Start address for scrubbing
  @param[in] DpaEndAddress      - End address for scrubbing
  @param[in] Enable             - Enable\Disable scrubbing
  @param[in] UseOsMailbox       - TRUE to use OS mailbox

  @return EFI_STATUS            - Status of the command sent

**/
EFI_STATUS
NvmCtlrSetAddressRangeScrub (
  UINT8     Socket,
  UINT8     Ch,
  UINT8     Dimm,
  UINT64    DpaStartAddress,
  UINT64    DpaEndAddress,
  BOOLEAN   Enable,
  BOOLEAN   UseOsMailbox
  )
{
  EFI_STATUS          Status;
  AEP_DIMM            *NvmDimm = NULL;
  NVM_SET_ARS_PAYLOAD Data;

  NvmDimm = GetDimm (Socket, Ch, Dimm);

  if (NvmDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DpaStartAddress &= ~(ARS_ALIGNMENT_SIZE - 1);
  DpaEndAddress   &= ~(ARS_ALIGNMENT_SIZE - 1);

  if (DpaStartAddress >= DpaEndAddress) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Data, sizeof (Data));
  Data.Enable           = Enable ? 0x1 : 0x0;
  Data.DpaStartAddress  = DpaStartAddress;
  Data.DpaEndAddress    = DpaEndAddress;

  Status = WriteToSmallPayload (NvmDimm, &Data, sizeof(Data), UseOsMailbox);
  ASSERT_EFI_ERROR (Status);

  Status = SendFnvCommand (NvmDimm, SET_ADDRESS_RANGE_SCRUB);

  return Status;
}

/**

  Routine Description: this function will send
  Get Address Range Scrub command to NVMCTLR.
  CR protocol function.

  @param[in]  Socket                 - Socket Number
  @param[in]  Ch                     - DDR Channel ID
  @param[in]  Dimm                   - DIMM number
  @param[out] *DpaStartAddress       - Start address for scrubbing
  @param[out] *DpaCurrentAddress     - Current address of scrub
  @param[out] *DpaEndAddress         - End address for scrubbing
  @param[out] *Enabled               - TRUE if scrub is enabled
  @param[in]  UseOsMailbox           - TRUE to use OS mailbox

  @return EFI_STATUS            - Status of the command sent

**/
EFI_STATUS
NvmCtlrGetAddressRangeScrub (
  UINT8     Socket,
  UINT8     Ch,
  UINT8     Dimm,
  UINT64    *DpaStartAddress,
  UINT64    *DpaEndAddress,
  UINT64    *DpaCurrentAddress,
  BOOLEAN   *Enabled,
  BOOLEAN   UseOsMailbox
  )
{
  EFI_STATUS          Status;
  AEP_DIMM            *NvmDimm = NULL;
  NVM_GET_ARS_PAYLOAD Data;

  NvmDimm = GetDimm (Socket, Ch, Dimm);

  if ((NvmDimm           == NULL) ||
      (DpaStartAddress   == NULL) ||
      (DpaCurrentAddress == NULL) ||
      (DpaEndAddress     == NULL) ||
      (Enabled           == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SendFnvCommand (NvmDimm, GET_ADDRESS_RANGE_SCRUB);

  if (!EFI_ERROR (Status)) {
    Status = ReadFromSmallPayload (NvmDimm, &Data, sizeof (Data), UseOsMailbox);
    ASSERT_EFI_ERROR (Status);

    *Enabled            = Data.Enabled != 0;
    *DpaStartAddress    = Data.DpaStartAddress;
    *DpaEndAddress      = Data.DpaEndAddress;
    *DpaCurrentAddress  = Data.DpaCurrentAddress;
  }

  return Status;
}

#ifdef FNV_COMMANDS_TESTING
/**

Routine Description: FnvGetDimmPartitionInfo will store the
partition info into the Buffer. It is assumed that the buffer
provided will be big enough to copy the data from the Payload
out buffer.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param *buffer     - Output buffer pointer
  @retval EFI_STATUS - success if command sent

**/
EFI_STATUS
FnvGetDimmPartitionInfo(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm,
   UINT8     *buffer
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;
  UINT32          *pData = NULL;
  UINT8           i;
  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);
  }

  Status = SendFnvCommand(pDimm, GET_DIMM_PARTITION);
  if (Status == MB_CMD_SUCCESS) {
    for(i=0; i<NUM_PAYLOAD_REG; i++) {
      pData = (UINT32 *)(UINTN)pMBox->pOutPayload[i];
      CopyMem (buffer, pData, sizeof (UINT32));
      //increment by 4 bytes
      buffer = buffer+4;
    }
  }

  return (Status);
}

/**

Routine Description: FnvSetDimmPartitionInfo will send the
partition info command to the NVMCTLR.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param TwoLM       - 2LM Size
  @param PM          - Persistent Mem size
  @retval EFI_STATUS - success if command sent

**/
EFI_STATUS
FnvSetDimmPartitionInfo(
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm,
   UINT32    TwoLM,
   UINT32    PM
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;

  pDimm = GetDimm(socket, ch, dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    *(UINT32 *)(UINTN)pMBox->pInPayload[0] = TwoLM;
    *(UINT32 *)(UINTN)pMBox->pInPayload[1] = PM;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], TwoLM, sizeof (UINT32));
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[1], PM, sizeof (UINT32));
  }

  // Send the command
  Status = SendFnvCommand(pDimm, SET_DIMM_PARTITION);

  return (Status);
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrEnableErrInjection(
    UINT8     Socket,
    UINT8     Ch,
    UINT8     Dimm
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    //Byte 0 is enable/Disable
    *(UINT32 *)(UINTN)pMBox->pInPayload[0] = (UINT32)0x1;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], 0x1, sizeof (UINT32));
  }
  //
  // Send the command
  Status = SendFnvCommand(pDimm, ENABLE_ERR_INJECTION);

  return (Status);
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrInjectPoisonError(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm,
   UINT64    DPA,
   UINT8     Enable,
   UINT32    *EinjCsr
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox; // How to populate this

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    *(UINT32 *)(UINTN)pMBox->pInPayload[0] = (UINT32)(DPA);
    *(UINT32 *)(UINTN)pMBox->pInPayload[1] = (UINT32)(DPA >> 32);
    *(UINT32 *)(UINTN)pMBox->pInPayload[2] = (UINT32)Enable;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], (UINT32)(DPA), sizeof (UINT32));
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[1], (UINT32)(DPA >> 32), sizeof (UINT32));
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[2], (UINT32)Enable, sizeof (UINT32));
  }
  //
  // Send the command
  Status = SendFnvCommand(pDimm, INJECT_POISON_ERROR);
  *EinjCsr = *(UINT32 *)pMBox->pOutPayload[0];
  return (Status);
}

/**

Routine Description: FnvInjectTempError injects a particular artificial temperature into the part

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @param temp        - temperature in Kelvin
  @retval EFI_STATUS - success if command sent

**/
EFI_STATUS
FnvInjectTempError(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm,
   UINT16    temp
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;
  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    *(UINT32 *)(UINTN)pMBox->pInPayload[0] = (UINT32)temp | BIT16;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], (UINT32)temp | BIT16, sizeof (UINT32));
  }
  //
  // Send the command
  Status = SendFnvCommand(pDimm, INJECT_TEMP_ERROR);

  return (Status);
}

#endif // FNV_COMMANDS_TESTING

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
UpdateNvmAcpiTables(
  UINT64 *CrystalRidgeTablePtr
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  EFI_HANDLE           Handle = NULL;
  UINTN                CrystalRidgeTableSize = sizeof(NVDIMM_FW_INTERFACE_TABLE);

  // Only if AEP DIMM is present
  if (!IsSystemWithAepDimm()) {
    return EFI_SUCCESS;
  }

  //
  // Call OEM-hook to create NFIT table in buffer.  If success, then skip code below
  //
  Status = OemCreateNfitTable(CrystalRidgeTablePtr, (UINT32)CrystalRidgeTableSize);
  if (!EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "Nfit: OemCreateNfitTable succeeded - BuildNfitTable skipped\n"));
    return EFI_SUCCESS;
  }

  //
  // Build the actual NFIT table with needed sub-tables
  //
  Status = BuildNfitTable(CrystalRidgeTablePtr, CrystalRidgeTableSize);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Nfit: BuildNfitTable failed, status = %r\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Call OEM-hook to update NFIT table in buffer
  //
  OemUpdateNfitTable(CrystalRidgeTablePtr);

  //
  // Update checksum because of possible changes in OemUpdateNfitTable
  //
  Status = UpdateNfitTableChecksum(CrystalRidgeTablePtr);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Nfit: UpdateNfitTableChecksum failed, status = %r\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Having created Nfit table, Install NfitBindingProtocol so that the
  // UEFI driver can bind to it instead of some virtual Pci Device.
  //
  if (mSystemMemoryMap->MemSetup.dfxMemSetup.dfxLoadDimmMgmtDriver == LOAD_NGN_DIMM_MGMT_DRIVER_EN) {
    DEBUG((EFI_D_ERROR, "Nfit: Load NGN DIMM MGMT driver - gNfitBindingProtocolGuid will be published\n"));
    Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gNfitBindingProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
    ASSERT_EFI_ERROR (Status);

    return Status;
  }
  return EFI_LOAD_ERROR;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
UpdateNvmAcpiPcatTable(
  UINT64 *PcatTablePtr
  )
{
  EFI_STATUS Status;
  EFI_ACPI_PCAT_PROTOCOL *Pcat;
  UINTN                  PcatTableSize = sizeof(NVDIMM_PLATFORM_CONFIG_ATTRIBUTE_TABLE);

  if ((NULL == PcatTablePtr)) {
    DEBUG ((DEBUG_ERROR, "[UpdateAcpiTables] ERROR - NULL pointer as a parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  // Access to ACPI PCAT supporting functions
  Status = gBS->LocateProtocol (&gAcpiPcatProtocolGuid, NULL, &Pcat);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[UpdateAcpiTables] ERROR - Cannot locate PCAT protocol, status %r\n", Status));
    return EFI_NOT_STARTED;
  }

  // Initialize ACPI PCAT only if not initialized yet
  if (!Pcat->IsInitialized()) {
    Status = Pcat->InitializeAcpiPcat (PcatTablePtr, PcatTableSize);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "[UpdateAcpiTables] ERROR - Couldn't initialize PCAT header, status %r\n", Status));
      return Status;
    }
  }

  return EFI_SUCCESS;

}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
BOOLEAN
IsNvmAddress (
  UINT64 Spa
)
{
  struct SADTable  *pSAD;
  UINT8            Socket, i;
  UINT64           SpaStart, SpaEnd;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    //
    // Traverse thru all SAD entries to check for the Spa being in the range of one
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Socket].SAD[i];
      //
      // If the Sad type is for an NVMDIMM..
      if (pSAD->type != MEM_TYPE_1LM) {
        if (pSAD->Enable == 0 || pSAD->local == 0) {
          continue;
        }

        if (i == 0) {
          SpaStart = 0;
        } else {
          SpaStart = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);
        }
        SpaEnd = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i].Limit << BITS_64MB_TO_BYTES);
        //
        // If the passed in SPA falls with in this SAD SPA Range
        if ((Spa >= SpaStart) && (Spa <= SpaEnd)) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**

  Routine Description: Find all channels that are marked as 2LM cached

  @param Socket         - index of socket
  @parma iMC            - index of memory controller
  @retval Bitmap of channels that are 2LM cached, e.g. bits b101 means channel 0,2

**/
static
UINT8
FindChannelsBitmapFor2LMType(
  UINT8 Socket,
  UINT8 iMC
   )
{
  struct SADTable          *pSADTable;
  UINT8                    BitmapOfCh = 0;
  UINT8                    SadInx;

  if (Socket >= MAX_SOCKET || iMC >= MAX_IMC) {
    DEBUG((EFI_D_ERROR, "CR: ERROR: %s - invalid parameters\n", __FUNCTION__));
    ASSERT(Socket < MAX_SOCKET);
    ASSERT(iMC < MAX_IMC);
    return 0;
  }

  for (SadInx = 0; SadInx < SAD_RULES; SadInx++) {
    pSADTable = &mSystemMemoryMap->Socket[Socket].SAD[SadInx];
    if ( pSADTable->Enable &&
        (pSADTable->imcInterBitmap & (1 << iMC)) &&
        (pSADTable->type == MEM_TYPE_2LM)
        ) {
      BitmapOfCh |= pSADTable->channelInterBitmap[iMC] << (MAX_MC_CH * iMC);
    }
  }

  return BitmapOfCh;
}

/**

  Routine Description: Finds all the Dimms taking part in CR interleave sets and
  updates SMBIOS entries for nonvolatile/cached info.
  Updates mSMBiosHandleInfo global array.

  @return UINT16       - Number of NVDIMM handles found.

**/
UINT16
GetAndUpdateAepDimmSMBiosType17Handles (
   VOID
   )
{
  EFI_STATUS              Status;
  UINT8                   PlatformMaxSocket;
  UINT8                   PlatformMaxChannelsPerSocket;
  UINT8                   PlatformMaxDimm;
  UINT8                   Socket;
  UINT8                   Ch;
  UINT8                   Dimm;
  UINT16                  i = 0;
  UINT8                   BitmapOfCachedChannels;
  EFI_SMBIOS_PROTOCOL     *SmbiosProtocol = NULL;
  EFI_SMBIOS_HANDLE       SmbiosHandle;
  EFI_SMBIOS_TYPE         SmbiosType;
  SMBIOS_TABLE_TYPE17     *pSmbiosRecord = NULL;
  UINT8                   Cached;

  Status = gBS->LocateProtocol(
    &gEfiSmbiosProtocolGuid,
    NULL,
    (VOID **)&SmbiosProtocol
    );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CR: ERROR - GetAndUpdateAepDimmSMBiosType17Handles: can't locate EfiSmbiosProtocol!\n"));
    ASSERT_EFI_ERROR (Status);
    return 0;
  }

  //
  // Need to grab those variables from PCD to be the same as in MemorySubClassEntryPoint()
  OemGetMemTopologyInfo (&PlatformMaxSocket, &PlatformMaxChannelsPerSocket, &PlatformMaxDimm);
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  for (Socket = 0; Socket < PlatformMaxSocket; Socket++) {
    for (Ch = 0; Ch < PlatformMaxChannelsPerSocket; Ch++) {
      BitmapOfCachedChannels = FindChannelsBitmapFor2LMType(Socket, Ch / MAX_MC_CH);
      for (Dimm = 0; Dimm < PlatformMaxDimm; Dimm++) {
        SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
        Status = SmbiosProtocol->GetNext (
                                    SmbiosProtocol,
                                    &SmbiosHandle,
                                    &SmbiosType,
                                    (EFI_SMBIOS_TABLE_HEADER**)&pSmbiosRecord,
                                    NULL
                                    );
        if (Status == EFI_NOT_FOUND) {
          break;
        }
        // This must be in DIMM loop instead of channel loop because
        // this retains order of enumeration using SmbiosProtocol->GetNext.
        // Algorithm for constructing SMBIOS table and order is in
        // MemorySubClassEntryPoint()
        if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].Enabled == 0 ||
            mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].Present == 0) {
          continue;
        }

        if (BitmapOfCachedChannels & (1 << Ch)) {
          Cached = 1;
        } else {
          Cached = 0;
        }

        // Found an NVMDIMM, now get it's SMBios Type17 Handle and save it.
        if (IsNvmDimm(Socket, Ch, Dimm)) {
          mSMBiosHandleInfo[i].Socket = Socket;
          mSMBiosHandleInfo[i].Channel = Ch;
          mSMBiosHandleInfo[i].Dimm = Dimm;
          mSMBiosHandleInfo[i].SMBiosType17Handle = pSmbiosRecord->Hdr.Handle;
          pSmbiosRecord->TypeDetail.Nonvolatile = 1;
          i++;
        } else {
          pSmbiosRecord->TypeDetail.Nonvolatile = 0;
          //If this is not volatile and involved in caching it must
          //be marked as that
          pSmbiosRecord->TypeDetail.CacheDram = Cached;
        }
      }
    }
  }

  // Check that we have enumerated all elements
  ASSERT(
    SmbiosProtocol->GetNext(
      SmbiosProtocol,
      &SmbiosHandle,
      &SmbiosType,
      (EFI_SMBIOS_TABLE_HEADER**)&pSmbiosRecord,
      NULL
      ) == EFI_NOT_FOUND
    );
  return i;
}

/*

  Routine Description: Does this system have at least one NVMDIMM

  @return TRUE if NVMDIMM present

**/
BOOLEAN
IsSystemWithAepDimm(
  VOID
  )
{
  if (mSystemMemoryMap->AepDimmPresent != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Routine Description: Is this Dimm the NVMDIMM.
    CR interface protocol function.

  @param[in] Socket      - Socket Number
  @param[in] Ch          - DDR Channel ID
  @param[in] Dimm        - DIMM number

  @return TRUE if NVMDIMM otherwise FALSE

**/
BOOLEAN
IsNvmDimm(
  UINT8 Socket,
  UINT8 Ch,
  UINT8 Dimm
   )
{
  if (Socket >= MAX_SOCKET || Ch >= MAX_CH || Dimm >= MAX_DIMM) {
    return FALSE;
  }

  if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].Present != 0 &&
      mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].aepDimmPresent != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Routine Description: Is the given NVMDIMM Enabled

  @param[in] Socket   - Socket ID
  @param[in] Ch       - Channel ID on given Socket
  @param[in] Dimm     - Dimm ID on given channel

  @return TRUE if NVMDIMM is Enabled, FALSE otherwise

**/
BOOLEAN
IsNvmDimmEnabled(
  UINT8 Socket,
  UINT8 Ch,
  UINT8 Dimm
  )
{
  if (IsNvmDimm(Socket, Ch, Dimm)) {
    return mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].Enabled;
  }

  return FALSE;
}

/**

Routine Description:IsRedundantSAD checks to see if this SAD has
been accounted for previously in the creation of the DimmInfo
struct for Control Region.

  @param SADTable    - Pointer to the SAD Table
  @retval BOOLEAN    - TRUE if redundant else FALSE

**/
BOOLEAN
IsRedundantCtrlRgnSAD(
   struct SADTable      *pSAD
   )
{
  STATIC SAD_CONTROL_REGION       SadControlRegion;
  UINT8                           i;
  BOOLEAN                         RetCode = FALSE;

  if (SadControlRegion.NumOfSADCntrlReg == 0) {
    SadControlRegion.Limits[0] = pSAD->Limit;
    SadControlRegion.NumOfSADCntrlReg++;
    RetCode = FALSE;
  } else {
    for (i = 0; i < SadControlRegion.NumOfSADCntrlReg; i++) {
      if (SadControlRegion.Limits[i] == pSAD->Limit) {
        RetCode = TRUE;
        break;
      }
    }
    if (i == SadControlRegion.NumOfSADCntrlReg) {
      SadControlRegion.Limits[i] = pSAD->Limit;
      SadControlRegion.NumOfSADCntrlReg++;
      RetCode = FALSE;
    }
  }
  return (RetCode);
}

/**

Routine Description:IsRedundantSAD checks to see if this SAD has
been accounted for previously in the creation of the PMEMInfo
structs for PMEM Region.

  @param SADTable    - Pointer to the SAD Table
  @retval BOOLEAN    - TRUE if redundant else FALSE

**/
BOOLEAN
IsRedundantPMemRgnSAD(
   struct SADTable      *pSAD
   )
{
  STATIC SAD_PMEM_REGION          SadPMemRegion;
  UINT8                           i;
  BOOLEAN                         RetCode = FALSE;


  if (SadPMemRegion.NumOfSADPMemReg == 0) {
    SadPMemRegion.Limits[0] = pSAD->Limit;
    SadPMemRegion.NumOfSADPMemReg++;
    RetCode = FALSE;
  } else {
    for (i = 0; i < SadPMemRegion.NumOfSADPMemReg; i++) {
      if (SadPMemRegion.Limits[i] == pSAD->Limit) {
        RetCode = TRUE;
        break;
      }
    }
    if (i == SadPMemRegion.NumOfSADPMemReg) {
      SadPMemRegion.Limits[i] = pSAD->Limit;
      SadPMemRegion.NumOfSADPMemReg++;
      RetCode = FALSE;
    }
  }
  return (RetCode);

}

/**

Routine Description:InitMailboxAddresses updates CR_MAILBOX
struct members with details of the register addresses for the
NVMDIMM's (found) Control Region.

  @param CrMailbox   - Pointer to the CR_MAILBOX struct
  @param pDimm       - Pointer to the Dimm struct
  @param pCntrlRgn   - Control Region Base (SPA) - since
                       from SAD, it needs to be converted to
                       bytes.
  @retval void       - None

**/
EFI_STATUS
InitMailboxAddresses(
   CR_MAILBOX      *pCrMailboxDPA,
   PAEP_DIMM       pDimm,
   UINT64          pCntrlRgn
   )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT64     CntrlBase;
  UINT64     RSpa;
  UINT32     PayloadOffset;
  UINT8      i;

  //
  // Control Region Base Addr
  //
  CntrlBase = pCntrlRgn;

  // Save the Control Region Base Addr for later use
  pDimm->CntrlBase = CntrlBase;
  DEBUG((EFI_D_ERROR, "CRP: Control Region Base = 0x%1lx\n", pDimm->CntrlBase));

  //
  // We need Dpa for each of these Mailbox addresses. However, what we have is Spa.
  // So it we need one routine to convert the Spa to Dpa.
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_COMMAND_OFFSET), &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: Command Reg SPA = 0x%1lx\n", RSpa));
  pCrMailboxDPA->pCommand = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_COMMAND_OFFSET);
  if (!EFI_ERROR(Status)) {
    pDimm->MailboxSPA.pCommand = RSpa;
  }
  ASSERT(Status == EFI_SUCCESS);

  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_NONCE0_OFFSET), &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: Nonce Reg SPA = 0x%1lx\n", RSpa));
  pCrMailboxDPA->pNonce0 = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_NONCE0_OFFSET);
  pDimm->MailboxSPA.pNonce0 = (UINT64)RSpa;

  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_NONCE1_OFFSET), &RSpa);
  pCrMailboxDPA->pNonce1 = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_NONCE1_OFFSET);
  pDimm->MailboxSPA.pNonce1 = (UINT64)RSpa;
  PayloadOffset = 0;
  for (i = 0; i < CR_IN_PAYLOAD_NUM; i++) {
    // If Odd register..
    if (i % 2) {
      pCrMailboxDPA->pInPayload[i] = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset + CR_IN_SINGLE_PAYLOAD_SIZE);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset + CR_IN_SINGLE_PAYLOAD_SIZE), &RSpa);
      PayloadOffset += CR_REG_ALIGNMENT_SIZE;
    } else {
      pCrMailboxDPA->pInPayload[i] = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset), &RSpa);
    }
    DEBUG((EFI_D_ERROR, "CRP: InPayload%d Reg SPA = 0x%1lx\n", i, RSpa));
    pDimm->MailboxSPA.pInPayload[i] = (UINT64)RSpa;
  }
  pCrMailboxDPA->pStatus = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_STATUS_OFFSET);
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_STATUS_OFFSET), &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: Status Reg SPA = 0x%1lx\n", RSpa));
  pDimm->MailboxSPA.pStatus = (UINT64)RSpa;
  PayloadOffset = 0;
  for (i = 0; i < CR_OUT_PAYLOAD_NUM; i++) {
    // If Odd register..
    if (i % 2) {
      pCrMailboxDPA->pOutPayload[i] = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset + CR_OUT_SINGLE_PAYLOAD_SIZE);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset + CR_OUT_SINGLE_PAYLOAD_SIZE), &RSpa);
      PayloadOffset += CR_REG_ALIGNMENT_SIZE;
    } else {
      pCrMailboxDPA->pOutPayload[i] = (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_BIOS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset), &RSpa);
    }
    DEBUG((EFI_D_ERROR, "CRP: OutPayload%d Reg SPA = 0x%1lx\n", i, RSpa));
    pDimm->MailboxSPA.pOutPayload[i] = (UINT64)RSpa;
  }

  pCrMailboxDPA->LargeInPayload = (UINT64)(CR_BIOS_MB_LARGE_INPAYLOAD);
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)CR_BIOS_MB_LARGE_INPAYLOAD, &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: Large InPayload Reg SPA = 0x%1lx\n", RSpa));
  pDimm->MailboxSPA.LargeInPayload = (UINT64)RSpa;

  pCrMailboxDPA->LargeOutPayload = (UINT64)CR_BIOS_MB_LARGE_OUTPAYLOAD;
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)CR_BIOS_MB_LARGE_OUTPAYLOAD, &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: Large OutPayload Reg SPA = 0x%1lx\n", RSpa));
  pDimm->MailboxSPA.LargeOutPayload = (UINT64)RSpa;

  //
  // The following code adds OS Mailbox Register addresses to the pDimm struct.
  //
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_COMMAND_OFFSET), &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: OS Command Reg SPA = 0x%1lx\n", RSpa));
  pDimm->OSMailboxDPA.pCommand = (UINT64)(CR_OS_MB_OFFSET + CR_MB_COMMAND_OFFSET);
  if (Status == EFI_SUCCESS) {
    pDimm->OSMailboxSPA.pCommand = RSpa;
  }
  ASSERT(Status == EFI_SUCCESS);

  // if (Status != EFI_SUCCESS) {
  //   return (Status);
  // }

  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_NONCE0_OFFSET), &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: OS Nonce Reg SPA = 0x%1lx\n", RSpa));
  pDimm->OSMailboxDPA.pNonce0 = (UINT64)(CR_OS_MB_OFFSET + CR_MB_NONCE0_OFFSET);
  pDimm->OSMailboxSPA.pNonce0 = (UINT64)RSpa;

  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_NONCE1_OFFSET), &RSpa);
  pDimm->OSMailboxDPA.pNonce1 = (UINT64)(CR_OS_MB_OFFSET + CR_MB_NONCE1_OFFSET);
  pDimm->OSMailboxSPA.pNonce1 = (UINT64)RSpa;
  PayloadOffset = 0;
  for (i = 0; i < CR_IN_PAYLOAD_NUM; i++) {
    // If Odd register..
    if (i % 2) {
      pDimm->OSMailboxDPA.pInPayload[i] = (UINT64)(CR_OS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset + CR_IN_SINGLE_PAYLOAD_SIZE);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset + CR_IN_SINGLE_PAYLOAD_SIZE), &RSpa);
      PayloadOffset += CR_REG_ALIGNMENT_SIZE;
    } else {
      pDimm->OSMailboxDPA.pInPayload[i] = (UINT64)(CR_OS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_IN_PAYLOAD0_OFFSET + PayloadOffset), &RSpa);
    }
    DEBUG((EFI_D_ERROR, "CRP: InPayload%d Reg SPA = 0x%1lx\n", i, RSpa));
    pDimm->OSMailboxSPA.pInPayload[i] = (UINT64)RSpa;
  }
  pDimm->OSMailboxDPA.pStatus = (UINT64)(CR_OS_MB_OFFSET + CR_MB_STATUS_OFFSET);
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_STATUS_OFFSET), &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: Status Reg SPA = 0x%1lx\n", RSpa));
  pDimm->OSMailboxSPA.pStatus = (UINT64)RSpa;
  PayloadOffset = 0;
  for (i = 0; i < CR_OUT_PAYLOAD_NUM; i++) {
    // If Odd register..
    if (i % 2) {
      pDimm->OSMailboxDPA.pOutPayload[i] = (UINT64)(CR_OS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset + CR_OUT_SINGLE_PAYLOAD_SIZE);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset + CR_OUT_SINGLE_PAYLOAD_SIZE), &RSpa);
      PayloadOffset += CR_REG_ALIGNMENT_SIZE;
    } else {
      pDimm->OSMailboxDPA.pOutPayload[i] = (UINT64)(CR_OS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset);
      Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)(CR_OS_MB_OFFSET + CR_MB_OUT_PAYLOAD0_OFFSET + PayloadOffset), &RSpa);
    }
    DEBUG((EFI_D_ERROR, "CRP: OS OutPayload%d Reg SPA = 0x%1lx\n", i, RSpa));
    pDimm->OSMailboxSPA.pOutPayload[i] = (UINT64)RSpa;
  }

  pDimm->OSMailboxDPA.LargeInPayload = (UINT64)(CR_OS_MB_LARGE_INPAYLOAD);
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)CR_OS_MB_LARGE_INPAYLOAD, &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: OS Large InPayload Reg SPA = 0x%1lx\n", RSpa));
  pDimm->OSMailboxSPA.LargeInPayload = (UINT64)RSpa;

  pDimm->OSMailboxDPA.LargeOutPayload = (UINT64)CR_OS_MB_LARGE_OUTPAYLOAD;
  Status = XlateDpaOffsetToSpa(pDimm, CntrlBase, (UINT64)CR_OS_MB_LARGE_OUTPAYLOAD, &RSpa);
  DEBUG((EFI_D_ERROR, "CRP: OS Large OutPayload Reg SPA = 0x%1lx\n", RSpa));
  pDimm->OSMailboxSPA.LargeOutPayload = (UINT64)RSpa;

  return (Status);
}

/**

Routine Description:PreInitDimmStruct pre initializes AEP_DIMM
struct that is being created with the values that are being
passed in of its members.

  @param SADSpaBase  - Spa Base Address of SAD Rule
  @param socket      - socket
  @param Ch          - Channel
  @param dimm        - Dimm number
  @param iMCWays     - iMCWays this dimm is interleaved across
                     iMCs
  @param ChWays      - Channel Ways this dimm is interleaved.
  @param IntDimmNum  - Dimm number in the interleave set
  @param FMchannelInterBitmap - pSAD->FMchannelInterBitmap of
                              the sad rule
  @retval VOID       - None

**/
VOID
PreInitDimmStruct(
   UINT64    SADSpaBase,
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm,
   UINT8     iMCWays,
   UINT8     ChWays,
   UINT8     IntDimmNum,
   UINT8     FMchannelInterBitmap
   )
{
  PAEP_DIMM       pDimm;

  pDimm = &mCrInfo.AepDimms[mCrInfo.NumAepDimms];
  pDimm->SADSpaBase = SADSpaBase;
  pDimm->Ch = ch;
  pDimm->Dimm = dimm;
  pDimm->SocketId = (UINT16)socket;
  if (ch < MAX_MC_CH) {
    pDimm->ImcId = 0; // iMC0
  } else {
    pDimm->ImcId = 1; // iMC1
  }
  pDimm->NonceSet = 0;
  pDimm->iMCWays = iMCWays;
  pDimm->ChWays = ChWays;
  pDimm->IntDimmNum = IntDimmNum;
  pDimm->FMchannelInterBitmap = FMchannelInterBitmap;
}

/**

Routine Description:GetNextChannel returns the next Channel
number based on what the currnet channel number is in
FMChannelInterBitmap.

  @param FMchannelInterBitmap - FMChannelInterbitmap in SAD
  @param CurrentCh   - Current Channel in FMChannelInterbitmap
  @retval NextCh     - Next Channel Number

**/
UINT8
GetNextChannel(
   UINT8     FMchannelInterBitmap,
   UINT8     CurrentCh
   )
{
  UINT8      NextCh = 0;

  switch (CurrentCh) {
  case 0:
    if (FMchannelInterBitmap & 0x02) {
      // Then next channel is 1
      NextCh = 1;
    } else if (FMchannelInterBitmap & 0x04) {
      NextCh = 2;
    }
    break;
  case 1:
    if (FMchannelInterBitmap & 0x04) {
      NextCh = 2;
    } else if (FMchannelInterBitmap & 0x01) {
      // Then next channel is 0
      NextCh = 0;
    }
    break;
  case 2:
    if (FMchannelInterBitmap & 0x01) {
      // Then next channel is 1
      NextCh = 0;
    } else if (FMchannelInterBitmap & 0x02) {
      NextCh = 1;
    }
    break;
  }
  return (NextCh);
}

/**

  Routine Description: initializes the PMEMInfo struct with the values
  provided as arguments into this function. Basically these values
  are used for the translation of Spa-To-Dpa conversion routines.

  @param[in] Socket      - Socket Number
  @param[in] Ch          - Channel number
  @param[in] Dimm        - Dimm number
  @param[in] SadSpaBase  - Base Address of the SAD Rule found
  @param[in] DpaBase     - Physical base address on the DIMM
  @param[in] ImcWays     - iMC Ways interleave
  @param[in] ChWays      - Channel Ways it is interleaved.
  @param[in] IntDimmNum  - Dimm Number in the SAD Interleave set

**/
VOID
InitPmemInfoStruct(
  UINT8           Socket,
  UINT8           Ch,
  UINT8           Dimm,
  UINT64          SadSpaBase,
  UINT64          DpaBase,
  UINT8           ImcWays,
  UINT8           ChWays,
  UINT8           IntDimmNum
  )
{
  PMEM_INFO       *PmemInfo;

  if (mCrInfo.NumPMEMRgns >= ARRAY_SIZE(mCrInfo.PMEMInfo)) {
    return;
  }

  PmemInfo = &mCrInfo.PMEMInfo[mCrInfo.NumPMEMRgns];

  PmemInfo->SADPMemBase  = SadSpaBase;
  PmemInfo->DPABase      = DpaBase;
  PmemInfo->Socket       = Socket;
  PmemInfo->Channel      = Ch;
  PmemInfo->Dimm         = Dimm;
  PmemInfo->ChWays       = ChWays;
  PmemInfo->iMCWays      = ImcWays;
  PmemInfo->IntDimmNum   = IntDimmNum;

  mCrInfo.NumPMEMRgns += 1;
}

/**

Routine Description:AddPMemInfoFor3ChWays adds pertinent
information regarding PMEM Region that has been found to the
AEP_DIMM Struct (for 3ch ways) and these fields are important to
the Spa-To-Dpa translation rountines ARS & RAS.
  @param iMCWays     - iMC Ways interleave
  @param SADSpaBase  - Base Address of the SAD Rule found
  @retval Status     - Status

**/
EFI_STATUS
AddPMemInfoFor3ChWays(
   UINT8          iMCWays,
   UINT64         SADSpaBase
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Soc, Ch, Dimm, i, k;
  UINT8           IntDimmNum = 0;
  struct          SADTable *pSAD;
  UINT64          SpaBase, DpaBase;
  PAEP_DIMM       pDimm = NULL;

  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    Ch = 0;
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          // iMC0 of this socket is involved in this interleave set,
          // Include these channels for the creation of Dimm Structs
          //
          for (k = 0; k < CH_3WAY; k++) {
            Status = GetChannelDimm(Soc, Ch, &Dimm);
            ASSERT_EFI_ERROR(Status);
            pDimm = GetDimm(Soc, Ch, Dimm);
            DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
            InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_3WAY, IntDimmNum);
            DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
            DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_3WAY));
            Ch++;
            DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
            IntDimmNum++;
          }
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          // If iMC1 of this socket is involved in this interleave set,
          // Include these channels for the creation of Dimm Structs
          Ch = MAX_MC_CH;
          for (k = 0; k < CH_3WAY; k++) {
            Status = GetChannelDimm(Soc, Ch, &Dimm);
            ASSERT_EFI_ERROR(Status);
            pDimm = GetDimm(Soc, Ch, Dimm);
            DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
            InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_3WAY, IntDimmNum);
            DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
            DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_3WAY));
            Ch++;
            DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
            IntDimmNum++;
          }
        }
      }
    }
  }
  return (Status);
}

/**

Routine Description:AddPMemInfoFor2ChWays adds pertinent
information regarding PMEM Region that has been found to the
AEP_DIMM Struct (for 2ch ways) and these fields are important to
the Spa-To-Dpa translation rountines ARS & RAS.
  @param iMCWays     - iMC Ways interleave
  @param SADSpaBase  - Base Address of the SAD Rule found
  @retval Status     - Status

**/
EFI_STATUS
AddPMemInfoFor2ChWays(
   UINT8          iMCWays,
   UINT64         SADSpaBase
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Soc, Ch = 0, Dimm, i;
  UINT8           IntDimmNum = 0;
  struct          SADTable *pSAD;
  UINT64          SpaBase, DpaBase;
  PAEP_DIMM       pDimm = NULL;

  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          // iMC0 is participating in 2 Channel Way Interleave set
          // Knowing it is 2 way interleaved, get the first channel
          if (pSAD->FMchannelInterBitmap[IMC0] & 0x01) {
            Ch = 0;
          } else if (pSAD->FMchannelInterBitmap[IMC0] & 0x02) {
            Ch = 1;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          pDimm = GetDimm(Soc, Ch, Dimm);
          DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
          InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_2WAY, IntDimmNum);
          DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_2WAY));
          DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
          IntDimmNum++;
          Ch = GetNextChannel(pSAD->FMchannelInterBitmap[IMC0], Ch);
          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          pDimm = GetDimm(Soc, Ch, Dimm);
          DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
          InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_2WAY, IntDimmNum);
          DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_2WAY));
          DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
          IntDimmNum++;
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          // iMC1 is participating in 2 Channel Way Interleave set
          if (pSAD->FMchannelInterBitmap[IMC1] & 0x01) {
            Ch = 3;
          } else if (pSAD->FMchannelInterBitmap[IMC1] & 0x02) {
            Ch = 4;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          pDimm = GetDimm(Soc, Ch, Dimm);
          DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
          InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_2WAY, IntDimmNum);
          DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_2WAY));
          DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
          IntDimmNum++;
          Ch = GetNextChannel(pSAD->FMchannelInterBitmap[IMC1], (Ch - MAX_MC_CH));
          Ch += MAX_MC_CH;
          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          pDimm = GetDimm(Soc, Ch, Dimm);
          DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
          InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_2WAY, IntDimmNum);
          DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_2WAY));
          DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
          IntDimmNum++;
        }
      }
    }
  }
  return (Status);
}

/**

Routine Description:AddPMemInfoFor1ChWays adds pertinent
information regarding PMEM Region that has been found to the
AEP_DIMM Struct (for 1ch ways) and these fields are important to
the Spa-To-Dpa translation rountines ARS & RAS.
  @param iMCWays     - iMC Ways interleave
  @param SADSpaBase  - Base Address of the SAD Rule found
  @retval Status     - Status

**/
EFI_STATUS
AddPMemInfoFor1ChWays(
   UINT8          iMCWays,
   UINT64         SADSpaBase
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Soc, Ch = 0, Dimm, i;
  UINT8           IntDimmNum = 0;
  struct          SADTable *pSAD;
  UINT64          SpaBase, DpaBase;
  PAEP_DIMM       pDimm = NULL;

  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      }
      else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          // iMC0 is participating in 1 Channel Way Interleave set
          // Knowing it is 1 way interleaved, get the channel
          if (pSAD->FMchannelInterBitmap[IMC0] & 0x01) {
            Ch = 0;
          } else if (pSAD->FMchannelInterBitmap[IMC0] & 0x02) {
            Ch = 1;
          } else if (pSAD->FMchannelInterBitmap[IMC0] & 0x04) {
            Ch = 2;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          pDimm = GetDimm(Soc, Ch, Dimm);
          DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
          InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_1WAY, IntDimmNum);
          DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_3WAY));
          DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
          IntDimmNum++;
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          if (pSAD->FMchannelInterBitmap[IMC1] & 0x01) {
            Ch = 3;
          } else if (pSAD->FMchannelInterBitmap[IMC1] & 0x02) {
            Ch = 4;
          } else if (pSAD->FMchannelInterBitmap[IMC1] & 0x04) {
            Ch = 5;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          pDimm = GetDimm(Soc, Ch, Dimm);
          DpaBase = GetPmemDpaBase(SADSpaBase, Soc, Ch, Dimm);
          InitPmemInfoStruct(Soc, Ch, Dimm, SADSpaBase, DpaBase, iMCWays, CH_1WAY, IntDimmNum);
          DEBUG((EFI_D_ERROR, "CRP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d, SPA = 0x%llx, DPA = 0x%llx\n", Soc, Ch, Dimm, SADSpaBase, DpaBase));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_3WAY));
          DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
          IntDimmNum++;
        }
      }
    }
  }
  return (Status);
}

/**

Routine Description:BuildDimmsFor1ChWays builds Dimm info data
structs for each NVMDIMM's (found) in the Control Region when
the associated SAD Rule is 3 way channel interleaved.

  @param iMCWays     - iMC Ways involved in the SAD Rule
  @param SADSpaBase  - Base Address associated with the SAD Rule
  @retval Status     - Status

  Notes: The ImcIncrements array below holds, the offset values
  of the Spa of the Channel of the iMC. Starting from 0, for
  iMC0-Ch0, the next one would be iMC0-Ch1, iMC0-Ch2 of socket0,
  then followed by iMC1-Ch0, iMC1-Ch1, iMC1-Ch2 of Socket0, then
  followed by Socket1 iMCs and Channels till Socket3 iMCs and
  Channels. By adding what is in this array to the Starting base
  address of the SAD rule, one can get the translated SPA Base
  Address of the Dimm in that particular Channel of the Socket.

**/
EFI_STATUS
BuildDimmsFor3ChWays(
   UINT8          iMCWays,
   UINT64         SADSpaBase
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Soc, Ch, Dimm, i, j, k;
  UINT8           IntDimmNum = 0;
  struct          SADTable *pSAD;
  UINT64          SpaBase, DimmSpaBase;
  UINT32          ImcIncrements[48] = { // MAX_SOCKET * MAX_MC_CH * MAX_IMC (for 8 sockets)
    0, CTRL_RGN_GRANULARITY, (2 * CTRL_RGN_GRANULARITY), (18 * CTRL_RGN_GRANULARITY), (16 * CTRL_RGN_GRANULARITY), (17 * CTRL_RGN_GRANULARITY),
    (33 * CTRL_RGN_GRANULARITY), (34 * CTRL_RGN_GRANULARITY), (32 * CTRL_RGN_GRANULARITY), (48 * CTRL_RGN_GRANULARITY), (49 * CTRL_RGN_GRANULARITY), (50 * CTRL_RGN_GRANULARITY),
    (66 * CTRL_RGN_GRANULARITY), (64 * CTRL_RGN_GRANULARITY), (65 * CTRL_RGN_GRANULARITY), (81 * CTRL_RGN_GRANULARITY), (82 * CTRL_RGN_GRANULARITY), (80 * CTRL_RGN_GRANULARITY),
    (96 * CTRL_RGN_GRANULARITY), (97 * CTRL_RGN_GRANULARITY), (98 * CTRL_RGN_GRANULARITY), (114 * CTRL_RGN_GRANULARITY), (112 * CTRL_RGN_GRANULARITY), (113 * CTRL_RGN_GRANULARITY),
    (129 * CTRL_RGN_GRANULARITY), (130 * CTRL_RGN_GRANULARITY), (128 * CTRL_RGN_GRANULARITY), (144 * CTRL_RGN_GRANULARITY), (145 * CTRL_RGN_GRANULARITY), (146 * CTRL_RGN_GRANULARITY),
    (162 * CTRL_RGN_GRANULARITY), (160 * CTRL_RGN_GRANULARITY), (161 * CTRL_RGN_GRANULARITY), (177 * CTRL_RGN_GRANULARITY), (178 * CTRL_RGN_GRANULARITY), (176 * CTRL_RGN_GRANULARITY),
    (192 * CTRL_RGN_GRANULARITY), (193 * CTRL_RGN_GRANULARITY), (194 * CTRL_RGN_GRANULARITY), (210 * CTRL_RGN_GRANULARITY), (208 * CTRL_RGN_GRANULARITY), (209 * CTRL_RGN_GRANULARITY),
    (225 * CTRL_RGN_GRANULARITY), (226 * CTRL_RGN_GRANULARITY), (224 * CTRL_RGN_GRANULARITY), (240 * CTRL_RGN_GRANULARITY), (241 * CTRL_RGN_GRANULARITY), (242 * CTRL_RGN_GRANULARITY)
                                      };
  j = 0;
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          // iMC0 of this socket is involved in this interleave set,
          // Include these channels for the creation of Dimm Structs

          // Determine starting channel number
          Ch = (SpaBase/CTRL_RGN_GRANULARITY) % CH_3WAY;

          for (k = 0; k < CH_3WAY; k++) {
            Status = GetChannelDimm(Soc, Ch, &Dimm);
            ASSERT_EFI_ERROR(Status);
            DEBUG((EFI_D_ERROR, "CRP: Found An AepDimm on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
            DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_3WAY));
            PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_3WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC0]);
            DimmSpaBase = SADSpaBase + ImcIncrements[j];
            Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
            ASSERT_EFI_ERROR(Status);
            mCrInfo.NumAepDimms++;
            j++;
            Ch = (Ch == CH_3WAY - 1) ? 0 : Ch + 1;
            DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
            IntDimmNum++;
          }
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          // If iMC1 of this socket is involved in this interleave set,
          // Include these channels for the creation of Dimm Structs

          // Determine starting channel number
          Ch = ((SpaBase/CTRL_RGN_GRANULARITY) % CH_3WAY) + MAX_MC_CH;

          for (k = 0; k < CH_3WAY; k++) {
            Status = GetChannelDimm(Soc, Ch, &Dimm);
            ASSERT_EFI_ERROR(Status);
            DEBUG((EFI_D_ERROR, "CRP: Found An AepDimm on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
            DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_3WAY));
            PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_3WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC1]);
            DimmSpaBase = SADSpaBase + ImcIncrements[j];
            Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
            ASSERT_EFI_ERROR(Status);
            mCrInfo.NumAepDimms++;
            j++;
            Ch = (Ch == MAX_MC_CH + CH_3WAY - 1) ? MAX_MC_CH : Ch + 1;
            DEBUG((EFI_D_ERROR, "CRP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
            IntDimmNum++;
          }
        }
      }
    }
  }
  ASSERT(mCrInfo.NumAepDimms <= MAX_SYS_DIMM);
  DEBUG((EFI_D_ERROR, "CRP: Number of AepDimms found so far = %d\n", mCrInfo.NumAepDimms));
  return (Status);
}

/**

Routine Description:BuildDimmsFor1ChWays builds Dimm info data
structs for each NVMDIMM's (found) in the Control Region when
the associated SAD Rule is 2 way channel interleaved.

  @param iMCWays     - iMC Ways involved in the SAD Rule
  @param SADSpaBase  - Base Address associated with the SAD Rule
  @retval Status     - Status

**/
EFI_STATUS
BuildDimmsFor2ChWays(
   UINT8          iMCWays,
   UINT64         SADSpaBase
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Soc, Ch = 0, Dimm, i;
  UINT8           IntDimmNum = 0;
  struct          SADTable *pSAD;
  UINT32          ChGran = mCrInfo.ChGran;
  UINT32          ImcIncrement;
  UINT64          SpaBase, DimmSpaBase;

  ImcIncrement = 0;
  DimmSpaBase = SADSpaBase;
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }

      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          // iMC0 is participating in 2 Channel Way Interleave set
          // Knowing it is 2 way interleaved, get the first channel
          if (pSAD->FMchannelInterBitmap[IMC0] & 0x01) {
            Ch = 0;
          } else if (pSAD->FMchannelInterBitmap[IMC0] & 0x02) {
            Ch = 1;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          DEBUG((EFI_D_ERROR, "CRP: Found An AepDimm on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_2WAY));
          PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_2WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC0]);
          DimmSpaBase += ImcIncrement;
          Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
          ASSERT_EFI_ERROR(Status);
          IntDimmNum++;
          mCrInfo.NumAepDimms++;
          Ch = GetNextChannel(pSAD->FMchannelInterBitmap[IMC0], Ch);
          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_2WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC0]);
          DimmSpaBase += ChGran;
          Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
          ASSERT_EFI_ERROR(Status);
          IntDimmNum++;
          mCrInfo.NumAepDimms++;
          ImcIncrement = mCrInfo.SktGran;
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          // iMC1 is participating in 2 Channel Way Interleave set
          if (pSAD->FMchannelInterBitmap[IMC1] & 0x01) {
            Ch = 3;
          } else if (pSAD->FMchannelInterBitmap[IMC1] & 0x02) {
            Ch = 4;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          DEBUG((EFI_D_ERROR, "CRP: Found An AepDimm on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_2WAY));
          PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_2WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC1]);
          DimmSpaBase += ImcIncrement;
          Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
          ASSERT_EFI_ERROR(Status);
          IntDimmNum++;
          mCrInfo.NumAepDimms++;
          Ch = GetNextChannel(pSAD->FMchannelInterBitmap[IMC1], (Ch - MAX_MC_CH));
          Ch += MAX_MC_CH;
          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_2WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC1]);
          DimmSpaBase += ChGran;
          Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
          ASSERT_EFI_ERROR(Status);
          IntDimmNum++;
          mCrInfo.NumAepDimms++;
          ImcIncrement = mCrInfo.SktGran;
        }
      }
    }
  }
  ASSERT(mCrInfo.NumAepDimms <= MAX_SYS_DIMM);
  DEBUG((EFI_D_ERROR, "CRP: Number of AepDimms found so far = %d\n", mCrInfo.NumAepDimms));
  return (Status);
}

/**

Routine Description:BuildDimmsFor1ChWays builds Dimm info data
structs for each NVMDIMM's (found) in the Control Region when
the associated SAD Rule is 1 way channel interleaved.

  @param iMCWays     - iMC Ways involved in the SAD Rule
  @param SADSpaBase  - Base Address associated with the SAD Rule
  @retval Status     - Status

**/
EFI_STATUS
BuildDimmsFor1ChWays(
   UINT8          iMCWays,
   UINT64         SADSpaBase
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Soc, Ch = 0, Dimm, i;
  UINT8           IntDimmNum = 0;
  struct          SADTable *pSAD;
  UINT32          ImcIncrement;
  UINT64          SpaBase, DimmSpaBase;

  ImcIncrement = 0;
  DimmSpaBase = SADSpaBase;
  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      } else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << 26);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][Soc * MAX_IMC]) {
          // iMC0 is participating in 1 Channel Way Interleave set
          // Knowing it is 1 way interleaved, get the channel
          if (pSAD->FMchannelInterBitmap[IMC0] & 0x01) {
            Ch = 0;
          } else if (pSAD->FMchannelInterBitmap[IMC0] & 0x02) {
            Ch = 1;
          } else if (pSAD->FMchannelInterBitmap[IMC0] & 0x04) {
            Ch = 2;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          DEBUG((EFI_D_ERROR, "CRP: Found An AepDimm on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_1WAY));
          PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_1WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC0]);
          DimmSpaBase += ImcIncrement;
          Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
          ASSERT_EFI_ERROR(Status);
          IntDimmNum++;
          mCrInfo.NumAepDimms++;
          ImcIncrement = mCrInfo.SktGran;
        }
        if (mSystemMemoryMap->Socket[Soc].SADintList[i][(Soc * MAX_IMC) + 1]) {
          if (pSAD->FMchannelInterBitmap[IMC1] & 0x01) {
            Ch = 3;
          } else if (pSAD->FMchannelInterBitmap[IMC1] & 0x02) {
            Ch = 4;
          } else if (pSAD->FMchannelInterBitmap[IMC1] & 0x04){
            Ch = 5;
          }

          Status = GetChannelDimm(Soc, Ch, &Dimm);
          ASSERT_EFI_ERROR(Status);
          DEBUG((EFI_D_ERROR, "CRP: Found An AepDimm on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
          DEBUG((EFI_D_ERROR, "CRP: iMCWays = %d, ChWays = %d\n", iMCWays, CH_1WAY));
          PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, CH_1WAY, IntDimmNum, pSAD->FMchannelInterBitmap[IMC1]);
          DimmSpaBase += ImcIncrement;
          Status = InitMailboxAddresses(&mCrInfo.AepDimms[mCrInfo.NumAepDimms].MailboxDPA, &mCrInfo.AepDimms[mCrInfo.NumAepDimms], DimmSpaBase);
          ASSERT_EFI_ERROR(Status);
          IntDimmNum++;
          mCrInfo.NumAepDimms++;
          ImcIncrement = mCrInfo.SktGran;
        }
      }
    }
  }
  ASSERT(mCrInfo.NumAepDimms <= MAX_SYS_DIMM);
  DEBUG((EFI_D_ERROR, "CRP: Number of AepDimms found so far = %d\n", mCrInfo.NumAepDimms));
  return (Status);
}

/**

Routine Description:UpdateDimmInfo updates Dimm info data struct
of the NVMDIMM's (found) Control Region and other info about
the Dimm.If there is more than one dimm in this sad rule, then
it is interleaved and need to account for each one, so creates a
separate dimminfo struct for each one.

  @param MemType       - Memory Type
  @retval VOID         - None

**/
VOID
UpdateDimmInfo(
   UINT8          MemType
   )
{
  struct SADTable *pSAD;
  UINT64          SpaBase;
  UINT64          Size;
  UINT8           i, ChWays, iMCWays, McIndex;
  UINT8           socket;
  STATIC BOOLEAN  FirstTime = TRUE;


  for (socket = 0; socket < MAX_SOCKET; socket++) {
    // Traverse thru all SAD entries to build the Nfit.
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[socket].SAD[i];

      if (pSAD->Enable == 0 || pSAD->local == 0) {
        continue;
      }

      if (MemType == MEM_TYPE_CTRL && (pSAD->type & MEM_TYPE_CTRL) && IsRedundantCtrlRgnSAD(pSAD)) {
        continue;
      } else if (MemType == MEM_TYPE_PMEM && (pSAD->type & MEM_TYPE_PMEM) && IsRedundantPMemRgnSAD(pSAD)) {
        continue;
      }

      if (FirstTime && (MemType == MEM_TYPE_CTRL) && (pSAD->type & MEM_TYPE_CTRL)) {
        mCrInfo.SktGran = GetSocketGranularity(pSAD);
        mCrInfo.ChGran  = GetChannelGranularity(pSAD);
        FirstTime       = FALSE;
      }

      // If the type is Control Region and if this SAD hasn't been previously accounted for
      if ((pSAD->type & MEM_TYPE_CTRL) || (pSAD->type & MEM_TYPE_PMEM)) {
        // Get iMC Interleave ways for this Cntrl Region SAD
        iMCWays = GetImcWays(socket, i);
        // Determine which IMC channel bitmap to use
        McIndex = GetMcIndexFromBitmap (pSAD->imcInterBitmap);

        // Get the Channel ways for this SAD
        ChWays = GetChInterleave(socket, McIndex, i);

        if (i == 0) {
          SpaBase = 0;
          Size = ((UINT64)mSystemMemoryMap->Socket[socket].SAD[i].Limit << 26);
        } else {
          SpaBase = ((UINT64)mSystemMemoryMap->Socket[socket].SAD[i - 1].Limit << 26);
          Size = ((UINT64)(mSystemMemoryMap->Socket[socket].SAD[i].Limit - mSystemMemoryMap->Socket[socket].SAD[i - 1].Limit) << 26);
        }
        DEBUG((EFI_D_ERROR, "CRP: UpdateDimmInfo - SAD Range Base = 0x%1lx   Size = 0x%1lx\n", SpaBase, Size));

        switch (ChWays) {
        case CH_3WAY:
          if ((MemType == MEM_TYPE_CTRL) && (pSAD->type & MEM_TYPE_CTRL)) {
            BuildDimmsFor3ChWays(iMCWays, SpaBase);
          } else if ((MemType == MEM_TYPE_PMEM) && (pSAD->type & MEM_TYPE_PMEM)){
            AddPMemInfoFor3ChWays(iMCWays, SpaBase);
          }
          break;
        case CH_2WAY:
          if ((MemType == MEM_TYPE_CTRL) && (pSAD->type & MEM_TYPE_CTRL)) {
            BuildDimmsFor2ChWays(iMCWays, SpaBase);
          } else if ((MemType == MEM_TYPE_PMEM) && (pSAD->type & MEM_TYPE_PMEM)) {
            AddPMemInfoFor2ChWays(iMCWays, SpaBase);
          }
          break;
        case CH_1WAY:
          if ((MemType == MEM_TYPE_CTRL) && (pSAD->type & MEM_TYPE_CTRL)) {
            BuildDimmsFor1ChWays(iMCWays, SpaBase);
          } else if ((MemType == MEM_TYPE_PMEM) && (pSAD->type & MEM_TYPE_PMEM)) {
            AddPMemInfoFor1ChWays(iMCWays, SpaBase);
          }
          break;
        }
      }
    }
  }
}

/**

Routine Description: SetMemAttributes updates the Attributes for each CTRL
region and Block window GCD Memory Map entry.

  @param VOID       - None
  @retval VOID      - None

**/
VOID
SetMemAttributes(
   VOID
   )
{
  struct SADTable                 *pSAD;
  UINT64                          SpaBase;
  UINT8                           i;
  UINT8                           socket;
  EFI_STATUS                      Status;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR Descriptor;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (i = 1; i < SAD_RULES; i++) { // SAD[0] will never be a CTRL Region or Block window
      pSAD = &mSystemMemoryMap->Socket[socket].SAD[i];

      if (pSAD->Enable == 0 || pSAD->local == 0) {
	    continue;
	  }

      if ((pSAD->type & MEM_TYPE_CTRL) || (pSAD->type & MEM_TYPE_BLK_WINDOW)) {
        // Get the Base of the SAD
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[socket].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);

        // Get the associated descriptor
        gDS->GetMemorySpaceDescriptor (SpaBase, &Descriptor);

        // Set Runtime flag
        Descriptor.Attributes |= EFI_MEMORY_RUNTIME;

        // For Block Window, set WB flag, clear the UC flag
        if (pSAD->type & MEM_TYPE_BLK_WINDOW) {
          Descriptor.Attributes = (Descriptor.Attributes & ~EFI_MEMORY_UC) | EFI_MEMORY_WB;
        }

        DEBUG((EFI_D_ERROR, "CRP: Setting Runtime flag for memory range %lx - %lx, Attributes = %lx\n",
                             Descriptor.BaseAddress, Descriptor.BaseAddress + Descriptor.Length, Descriptor.Attributes));

        Status = gDS->SetMemorySpaceAttributes (
          Descriptor.BaseAddress,
          Descriptor.Length,
          Descriptor.Attributes
          );
        ASSERT_EFI_ERROR (Status);
      }
    }
  }
}

/**

  Routine descritption: Gets registry offset for DDRT FNV EVENT CSR based
  on given Dimm number in channel and priority.

  @param[in] Dimm     - DIMM slot index within DDR channel
  @param[in] Priority - 0 for low priority event, 1 for high

  @return CSR offset

**/
UINT32
GetMcCsrFnvEventRegOffset (
  UINT8 Dimm,         // with in channel
  UINT8 Priority
  )
{
  UINT32 RegOffset;

  if (Dimm == 0) {
    RegOffset = (Priority == LOW_PRIORITY_EVENT) ? DDRT_FNV0_EVENT0_MC_2LM_REG : DDRT_FNV0_EVENT1_MC_2LM_REG;
  } else {
    RegOffset = (Priority == LOW_PRIORITY_EVENT) ? DDRT_FNV1_EVENT0_MC_2LM_REG : DDRT_FNV1_EVENT1_MC_2LM_REG;
  }

  return RegOffset;
}

/**

  Routine descritption: Clears error status of a specific NGN DIMM

  @param[in] Socket   - Socket index at system level
  @param[in] Ch       - Channel index at socket level
  @param[in] Dimm     - DIMM slot index within DDR channel
  @param[in] Priority - 0 for low priority event, 1 for high

  @retval EFI_SUCCESS           - error is cleared
  @retval EFI_INVALID_PARAMETER - given dimm is not NGN DIMM

**/
EFI_STATUS
FnvErrorLogClearError (
   UINT8       Socket,
   UINT8       Ch,
   UINT8       Dimm,
   UINT8       Priority
   )
{
  UINT32                          RegOffset;
  DDRT_FNV0_EVENT0_MC_2LM_STRUCT  FnvEvent;

  if (!IsNvmDimm (Socket, Ch, Dimm) ||
      ((Priority != LOW_PRIORITY_EVENT) && (Priority != HIGH_PRIORITY_EVENT))
     ) {
    return EFI_INVALID_PARAMETER;
  }

  RegOffset = GetMcCsrFnvEventRegOffset (Dimm, Priority);

  FnvEvent.Data = mCpuCsrAccess->ReadCpuCsr (Socket, Ch, RegOffset);

  FnvEvent.Bits.log = 0;
  FnvEvent.Bits.id  = 0;

  mCpuCsrAccess->WriteCpuCsr (Socket, Ch, RegOffset, FnvEvent.Data);

  return EFI_SUCCESS;
}

/**

  Routine Description: Programs Error Signaling for NVMCTLR Errors.
  Note that DDRT_FNV_INTR is used to select error signaling, not DDRT_FNV0/1_EVENT.
  CR protocol function.

  @param[in] Socket      - Socket index at system level
  @param[in] Ch          - Channel index at socket level
  @param[in] Dimm        - DIMM slot index within DDR channel
  @param[in] Priority    - 0:Low Priotity, 1:High Priority.
                           There are two buckets of errors. Each bucket can be
                           programmed to a particular signal scheme.
  @param[in] Signal      - BIT0 - SMI, BIT1 - ERROR0#, BIT2 - MSMI#
                           (all mutually exclusive)

  @retval EFI_SUCCESS           - signal programmed successfully
  @retval EFI_INVALID_PARAMETER - given DIMM is not NGN DIMM

**/
EFI_STATUS
NvmCtlrErrorLogProgramSignal (
  UINT8 Socket,
  UINT8 Ch,
  UINT8 Dimm,
  UINT8 Priority,
  UINT8 Signal
)
{
  DDRT_FNV_INTR_CTL_MCDDC_DP_STRUCT   FnvIntrCtrl;

  if (!IsNvmDimm (Socket, Ch, Dimm) ||
      ((Priority != LOW_PRIORITY_EVENT) && (Priority != HIGH_PRIORITY_EVENT))
     ) {
    return EFI_INVALID_PARAMETER;
  }

  if (Signal == 0) {
    return EFI_SUCCESS;
  }

  FnvIntrCtrl.Data = mCpuCsrAccess->ReadCpuCsr (Socket, Ch, DDRT_FNV_INTR_CTL_MCDDC_DP_REG);

  if (Priority == LOW_PRIORITY_EVENT) {
    if (Dimm == 0) {
      FnvIntrCtrl.Bits.slot0_lo_smi_en  = (Signal & SMI)    ? 1 : 0;
      FnvIntrCtrl.Bits.slot0_lo_err0_en = (Signal & ERROR0) ? 1 : 0;
    } else {
      FnvIntrCtrl.Bits.slot1_lo_smi_en  = (Signal & SMI)    ? 1 : 0;
      FnvIntrCtrl.Bits.slot1_lo_err0_en = (Signal & ERROR0) ? 1 : 0;
    }
  } else {
    if (Dimm == 0) {
      FnvIntrCtrl.Bits.slot0_hi_smi_en  = (Signal & SMI)    ? 1 : 0;
      FnvIntrCtrl.Bits.slot0_hi_err0_en = (Signal & ERROR0) ? 1 : 0;
    } else {
      FnvIntrCtrl.Bits.slot1_hi_smi_en  = (Signal & SMI)    ? 1 : 0;
      FnvIntrCtrl.Bits.slot1_hi_err0_en = (Signal & ERROR0) ? 1 : 0;
    }
  }

  mCpuCsrAccess->WriteCpuCsr (Socket, Ch, DDRT_FNV_INTR_CTL_MCDDC_DP_REG, FnvIntrCtrl.Data);

  return EFI_SUCCESS;
}

/**

  Routine Description: Detects the NGN DIMM error and returns 24-bit log.
  CR protocol function.

  @param[in] Socket         - Socket index at system level
  @param[in] Ch             - Channel index at socket level
  @param[in] Dimm           - DIMM slot index within DDR channel
  @param[in] Priority       - 0 for low priority event, 1 for high
  @param[in] ClearOnRead    - when set, clears the error when a NGN DIMM
                              is found and reported
  @param[out] *Log          - 24-bit log outcome

  @retval EFI_SUCCESS             - A NGN DIMM with error was found, pLog updated
  @retval EFI_INVALID_PARAMETER   - Given DIMM is not NGN DIMM or Log is NULL
  @retval EFI_NOT_FOUND           - No error found on NGN DIMM, Log = 0

**/
EFI_STATUS
NvmCtlrErrorLogDetectError (
  UINT8   Socket,
  UINT8   Ch,           // at socket level
  UINT8   Dimm,         // with in channel
  UINT8   Priority,
  BOOLEAN ClearOnRead,
  UINT32  *Log
  )
{
  UINT32                          RegOffset;
  DDRT_FNV0_EVENT0_MC_2LM_STRUCT  FnvEvent;

  if (!IsNvmDimm (Socket, Ch, Dimm) ||
      (Log == NULL) ||
      ((Priority != LOW_PRIORITY_EVENT) && (Priority != HIGH_PRIORITY_EVENT))
     ) {
    return EFI_INVALID_PARAMETER;
  }

  *Log = 0;

  RegOffset = GetMcCsrFnvEventRegOffset (Dimm, Priority);
  FnvEvent.Data = mCpuCsrAccess->ReadCpuCsr (Socket, Ch, RegOffset);

  if (!FnvEvent.Bits.valid) {
    return EFI_NOT_FOUND;
  }

  *Log = FnvEvent.Bits.log; // update the 24-bit log in to [23:0] of *Log


  if (ClearOnRead) {
    FnvErrorLogClearError (Socket, Ch, Dimm, Priority);
  }

  return EFI_SUCCESS;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrSetCfgDataPolicy (
  IN    UINT8     Socket,
  IN    UINT8     Ch,
  IN    UINT8     Dimm,
  IN    UINT32    Data
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;


  pDimm = GetDimm (Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush (pDimm->SocketId, pDimm->ImcId, TRUE);

    *(UINT32 *)(UINTN)pMBox->pInPayload[0] = Data;
  } else {
    DurableCacheLineWrite ((UINTN)pMBox->pInPayload[0], Data, sizeof (UINT32));
  }

  Status = SendFnvCommand (pDimm, SET_CONFIG_DATA_POLICY);

  return Status;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
NvmCtlrGetCfgDataPolicy(
  IN      UINT8     Socket,
  IN      UINT8     Ch,
  IN      UINT8     Dimm,
      OUT UINT32    *Data
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;

  pDimm = GetDimm (Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }
  // Fill in the command input parameters
  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);
  }

  Status = SendFnvCommand(pDimm, GET_CONFIG_DATA_POLICY);
  *Data =  *(UINT32 *)(UINTN)pMBox->pOutPayload[0];

  return Status;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
UINT32
GetNumNvmDimms(
  VOID
  )
{
  UINT8       Socket;
  UINT8       Ch;
  UINT8       Dimm;
  UINT32      NumNgnDimms = 0;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Ch = 0; Ch < MAX_CH; Ch++) {
      if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].Enabled == 0) {
        continue;
      }
      // Get the dimmNvList to see if any of them are Aep dimms
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (IsNvmDimm(Socket, Ch, Dimm)) {
          NumNgnDimms += 1;
        }
      }
    }
  }
  return (NumNgnDimms);
}

/**

  SMARTThreshhold translates to Alarm Threshold Return Data
  Command per NVMCTLR FIS. So, this function Sends get
  AlarmThresholdReturnData Command to the NVMCTLR f/w and return 8
  bytes of the data from the command payload.

  @param Socket - Socket index at system level
  @param Ch   - Channel index at socket level
  @param Dimm - DIMM slot index within DDR channel

  @return <EFI error code> - very unlikely unless a IO failed
  @return EFI_SUCCESS - error is cleared

**/
EFI_STATUS
GetSMARTThreshold(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);
  }

  Status = SendFnvCommand(pDimm, GET_ALARM_THRESHOLDS);

  return (Status);
}

/**

  This function Sends get SMARTHealth Command to the NVMCTLR

  @param Socket - Socket index at system level
  @param Ch   - Channel index at socket level
  @param Dimm - DIMM slot index within DDR channel

  @return <EFI error code> - very unlikely unless a IO write failed
  @return EFI_SUCCESS - error is cleared

**/
EFI_STATUS
GetSmartHealth(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);
  }

  Status = SendFnvCommand(pDimm, GET_SMART_HEALTH);

  return (Status);
}

/**

  This function returns the data from the SMARTHealth Command

  @param Socket - Socket index at system level
  @param Ch   - Channel index at socket level
  @param Dimm - DIMM slot index within DDR channel
  @param *SmartBuffer - pointer to a 128 byte buffer

  @return <EFI error code> - very unlikely unless a IO write failed
  @return EFI_SUCCESS - error is cleared

**/
EFI_STATUS
GetSmartHealthData(
  UINT8 Socket,
  UINT8 Ch,
  UINT8 Dimm,
  UINT8 *SmartBuffer
  )
{
  UINT8       i;
  PAEP_DIMM   pDimm = NULL;
  EFI_STATUS  Status = EFI_SUCCESS;
  CR_MAILBOX  *pMBox;
  BOOLEAN     UseOSMailboxCurrent = mUseOSMailbox;

  mUseOSMailbox = TRUE;
  Status = GetSmartHealth(Socket, Ch, Dimm);
  mUseOSMailbox = UseOSMailboxCurrent;

  if (EFI_ERROR(Status)) {
    return EFI_INVALID_PARAMETER;
  }

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  for (i = 0;  i < CR_OUT_PAYLOAD_NUM; i++) {
    CopyMem (&SmartBuffer[i * 4], (UINT8 *) (UINTN)pMBox->pOutPayload[i], sizeof (UINT32));
  }
  return EFI_SUCCESS;
}

/**

  This function returns Platform Config Data Area size.
  Currently this is not implemented in NVMCTLR F/W and hence this
  call is not implemented. However, we will need to revisit this
  when the F/W start to support this call.

  @param Socket          - Socket index at system level
  @param Ch              - Channel index at socket level
  @param Dimm            - DIMM slot index within DDR channel
  @param PartitionId     - Partition ID for which the PCD Size
                           needed.
  @param UseOsMailbox    - if TRUE, use the OS mailbox. BIOS mailbox otherwise
  @param *Size           - Pointer to return the size data.

  @return Status - Status of the operation

**/
EFI_STATUS
GetPlatformConfigDataAreaSize(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm,
   UINT8     PartitionId,
   BOOLEAN   UseOsMailbox,
   UINT32    *Size
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  AEP_DIMM        *NvmDimmPtr = NULL;
  CR_MAILBOX      *MailboxPtr;
  UINT32          PayloadType;
  UINT32          RetrieveOption;
  BOOLEAN         UseOsMailboxCurrent;

  NvmDimmPtr = GetDimm(Socket, Ch, Dimm);
  if (NvmDimmPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MailboxPtr = GetMailbox(NvmDimmPtr, UseOsMailbox);

  PayloadType    = SMALL_PAYLOAD;
  RetrieveOption = FNV_SIZE;

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(NvmDimmPtr->SocketId, NvmDimmPtr->ImcId, TRUE);

    *(UINT32 *)(UINTN)MailboxPtr->pInPayload[0] = (UINT32)PartitionId | (PayloadType << 8) | (RetrieveOption << 9);
  } else {
    DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[0], (UINT32)PartitionId | (PayloadType << 8) | (RetrieveOption << 9), sizeof (UINT32));
  }

  //
  // need to set global UseOSMailbox variable for SendFnvCommand
  UseOsMailboxCurrent = mUseOSMailbox;
  mUseOSMailbox       = UseOsMailbox;

  if (GetMediaStatus(NvmDimmPtr) == MEDIA_READY) {
    Status = SendFnvCommand(NvmDimmPtr, GET_PLATFORM_CONFIG);
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  mUseOSMailbox = UseOsMailboxCurrent;

  if (EFI_ERROR(Status)) {
    return Status;
  }

  *Size = *(UINT32 *)(UINTN)MailboxPtr->pOutPayload[0];

  return Status;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
GetPlatformConfigDataAreaData(
  UINT8     Socket,
  UINT8     Ch,
  UINT8     Dimm,
  UINT8     *OutputBuffer,
  UINT32    *OutputSize,
  UINT32    Offset,
  UINT8     PartitionId,
  BOOLEAN   UseLargePayload,
  BOOLEAN   UseOsMailbox
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  AEP_DIMM        *NvmDimmPtr = NULL;
  CR_MAILBOX      *MailboxPtr;
  UINT16          *InputPtr;
  UINT8           *BufferPtr;
  UINT8           *OutBufferPtr;
  UINT8           i;
  UINT32          PayloadType;
  UINT32          RetrieveOption;
  UINT32          Size;
  BOOLEAN         UseOsMailboxCurrent;

  NvmDimmPtr = GetDimm(Socket, Ch, Dimm);

  if (NvmDimmPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //Find the size of the partition to be read.
  Status = GetPlatformConfigDataAreaSize(Socket, Ch, Dimm, PartitionId, UseOsMailbox, &Size);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  MailboxPtr = GetMailbox(NvmDimmPtr, UseOsMailbox);

  (UseLargePayload == USE_LARGE_PAYLOAD) ? (PayloadType = LARGE_PAYLOAD) : (PayloadType = SMALL_PAYLOAD);
  RetrieveOption = FNV_DATA;

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(NvmDimmPtr->SocketId, NvmDimmPtr->ImcId, TRUE);

    if (UseLargePayload == USE_SMALL_PAYLOAD) {
      InputPtr  = (UINT16 *)(UINTN)MailboxPtr->pInPayload[0];
      *InputPtr = (UINT16)(PartitionId | (PayloadType << 8) | (RetrieveOption << 9));
      InputPtr++;
      *InputPtr = (UINT16)(Offset & 0x0000FFFF);
      InputPtr  = (UINT16 *)(UINTN)MailboxPtr->pInPayload[1];
      *InputPtr = (UINT16)((Offset >> 16) & 0x0000FFFF);
    } else {
      //Fill input payload registers
      *(UINT32 *)(UINTN)MailboxPtr->pInPayload[0] = (UINT32)PartitionId | (PayloadType << 8) | (RetrieveOption << 9);
    }
  } else {
    if (UseLargePayload == USE_SMALL_PAYLOAD) {
      DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[0], (UINT32)PartitionId | (PayloadType << 8) | (RetrieveOption << 9) | (Offset << 16), sizeof (UINT32));
      DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[1], (Offset >> 16), sizeof (UINT32));
    } else {
      DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[0], (UINT32)PartitionId | (PayloadType << 8) | (RetrieveOption << 9), sizeof (UINT32));
    }
  }

  //
  // need to set global UseOSMailbox variable for SendFnvCommand
  UseOsMailboxCurrent = mUseOSMailbox;
  mUseOSMailbox = UseOsMailbox;

  if (GetMediaStatus(NvmDimmPtr) == MEDIA_READY) {
    Status = SendFnvCommand(NvmDimmPtr, GET_PLATFORM_CONFIG);
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  mUseOSMailbox = UseOsMailboxCurrent;

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // If small payload is used, then copy the 128 bytes from the Small OutPayload to the buffer
  if (UseLargePayload == USE_SMALL_PAYLOAD) {
    OutBufferPtr = OutputBuffer;
    for (i = 0;  i < CR_OUT_PAYLOAD_NUM; i++) {
      BufferPtr = (UINT8 *)(UINTN)MailboxPtr->pOutPayload[i];
      CopyMem (OutBufferPtr, BufferPtr, sizeof (UINT32));
      OutBufferPtr += sizeof(UINT32);
    }
  } else {
    DEBUG((EFI_D_ERROR, "CRP: DSM VS GetPlatformConfigDataAreaData\n"));

    //
    // if Offset > 0, we don't want the whole PCD
    if (Offset > Size) {
      Offset = Size;
    }
    Size = Size - Offset;

    if (*OutputSize < Size) {
      Size = *OutputSize;
    } else {
      *OutputSize = Size;
    }

    Status = CopyFromLargePayloadToBuffer(
      Socket,
      Ch,
      Dimm,
      Offset,
      OutputBuffer,
      0,
      Size,
      LARGE_PAYLOAD_OUTPUT
      );
    ASSERT_EFI_ERROR(Status);

    //
    // CR workaround:
    // perform 2nd read only for FNV & EKV (1st gen).
    if (IsNvmCtrlFirstGen(NvmDimmPtr)) {
      Status = CopyFromLargePayloadToBuffer(
        Socket,
        Ch,
        Dimm,
        Offset,
        OutputBuffer,
        0,
        Size,
        LARGE_PAYLOAD_OUTPUT
        );
    }
  }

  return Status;
}

/* CR interface function, see documentation in Protocol/CrystalRidge.h */
EFI_STATUS
SetPlatformConfigDataAreaData(
  UINT8     Socket,
  UINT8     Ch,
  UINT8     Dimm,
  UINT8     *InputBuffer,
  UINT32    InputSize,
  UINT32    Offset,
  UINT8     PartitionId,
  BOOLEAN   UseLargePayload,
  BOOLEAN   UseOsMailbox
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  PAEP_DIMM   NvmDimmPtr = NULL;
  CR_MAILBOX  *MailboxPtr;
  UINT16      *InputPtr;
  UINT8       *BufferPtr;
  UINT8       *InBufferPtr;
  UINT8       i;
  UINT32      Size = 0;
  BOOLEAN     UseOsMailboxCurrent;

  if ((UINT64) Offset + (UINT64) InputSize > CR_BIOS_LARGE_PAYLOAD_SIZE) {
    return EFI_INVALID_PARAMETER;
  }
  NvmDimmPtr = GetDimm(Socket, Ch, Dimm);

  if (NvmDimmPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // for large payload we need to get full PCD contents first
  if (UseLargePayload == USE_LARGE_PAYLOAD) {
    //Find the size of the partition to be read.
    Status = GetPlatformConfigDataAreaSize(Socket, Ch, Dimm, PartitionId, UseOsMailbox, &Size);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = GetPlatformConfigDataAreaData(Socket, Ch, Dimm, mLargePayloadBuffer, &Size, 0, PartitionId, USE_LARGE_PAYLOAD, UseOsMailbox);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  MailboxPtr = GetMailbox(NvmDimmPtr, UseOsMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(NvmDimmPtr->SocketId, NvmDimmPtr->ImcId, TRUE);

    // Copy the input parameters to the Input Payload.
    InputPtr = (UINT16 *)(UINTN)MailboxPtr->pInPayload[0];

    if (UseLargePayload == USE_SMALL_PAYLOAD) {
      // Byte0 is Partition ID
      *InputPtr = (PartitionId | ((UINT16)SMALL_PAYLOAD << 8));
      // If we need to use the small payload, bytes 2 to 5 should contain the offset
      InputPtr++;
      *InputPtr = (UINT16)(Offset & 0x0000FFFF);
      InputPtr = (UINT16 *)(UINTN)MailboxPtr->pInPayload[1];
      *InputPtr = (UINT16)((Offset >> 16) & 0x0000FFFF);
      // pInPayload[16] thru pInPayload[31] will contain the Input Platform Config Data
      InBufferPtr = InputBuffer;
      for (i = 16; i < CR_IN_PAYLOAD_NUM; i++) {
        BufferPtr = (UINT8 *)(UINTN)MailboxPtr->pInPayload[i];
        CopyMem (BufferPtr, InBufferPtr, sizeof (UINT32));
        InBufferPtr += sizeof(UINT32);
      }
    } else {
      *InputPtr = (PartitionId | ((UINT16)LARGE_PAYLOAD << 8));
    }
  } else {
    if (UseLargePayload == USE_SMALL_PAYLOAD) {
      DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[0], (UINT32)PartitionId | (SMALL_PAYLOAD << 8) | (Offset << 16), sizeof (UINT32));
      DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[1], (Offset >> 16), sizeof (UINT32));
      // pInPayload[16] thru pInPayload[31] will contain the Input Platform Config Data
      InBufferPtr = InputBuffer;
      for (i = 16; i < CR_IN_PAYLOAD_NUM; i++) {
        DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[i], *(UINT32 *)InBufferPtr, sizeof (UINT32));
        InBufferPtr += sizeof(UINT32);
      }
    } else {
      DurableCacheLineWrite ((UINTN)MailboxPtr->pInPayload[0], (UINT32)PartitionId | (LARGE_PAYLOAD << 8), sizeof (UINT32));
    }
  }

  if (UseLargePayload == USE_LARGE_PAYLOAD) {
    //
    // Update section passed from OS
    CopyMem (mLargePayloadBuffer + Offset, InputBuffer, InputSize);

    if (mSystemMemoryMap->cpuStepping <= U0_REV_SKX) {
#ifdef __GNUC__
      asm("sfence");
#else
      _mm_sfence();
#endif
    }

    // Copy data from the large payload buffer to the Large Input Payload
    Status = CopyFromBufferToLargePayload(mLargePayloadBuffer, 0, Socket, Ch, Dimm, 0, Size, LARGE_PAYLOAD_INPUT);
    ASSERT_EFI_ERROR(Status);

    if (mSystemMemoryMap->cpuStepping <= U0_REV_SKX) {
#ifdef __GNUC__
      asm("sfence");
#else
      _mm_sfence();
#endif
      Status = CopyFromLargePayloadToBuffer(Socket, Ch, Dimm, 0, mLargePayloadBuffer, 0, Size, LARGE_PAYLOAD_OUTPUT);
    }
  }

  //
  // need to set global UseOSMailbox variable for SendFnvCommand
  UseOsMailboxCurrent = mUseOSMailbox;
  mUseOSMailbox        = UseOsMailbox;

  //
  // Send the command and return Status.
  if (GetMediaStatus(NvmDimmPtr) == MEDIA_READY) {
    Status = SendFnvCommand(NvmDimmPtr, SET_PLATFORM_CONFIG);
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  mUseOSMailbox = UseOsMailboxCurrent;

  return Status;
}

/**

  This routine handles Vendor Specific commands from the OS
  layer, specifically for BIOS Emulated Commands. This is
  implemented as per new Intel NVM DIMM FW Interface Table. This
  routine handles exclusively BIOS Emulated Commands only.

  @param Socket - Socket index at system level
  @param Ch   - Channel index at socket level
  @param Dimm - DIMM slot index within DDR channel

  @return Status

**/
EFI_STATUS
HandleBIOSEmulatedCommands(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  PAEP_DIMM   pDimm = NULL;
  UINT16      SubOpCode;
  UINT32      *pDWord;
  UINT32      NumBytes;
  UINT32      MBOffset;
  UINT64      Spa;
  UINT64      *pOutBuf;
  UINT64      BootStatus;

  SubOpCode = (UINT16)mNgnAcpiSmmInterface->InputOpCode >> 8;

  pDimm = GetDimm(Socket, Ch, Dimm);
  if (pDimm == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    mNgnAcpiSmmInterface->OutputLength = 0;
    return (EFI_INVALID_PARAMETER);
  }

  switch (SubOpCode) {
  case GET_LP_MB_INFO:
    DEBUG((EFI_D_ERROR, "CRP: Vendor Specific; GET LargePayload MB info, SubOpCode=0x%x\n", SubOpCode));
    // Get Large Payload Mailbox Information
    pDWord = (UINT32 *)&mNgnAcpiSmmInterface->OutputBuffer[0];
    // First dword is the size of the large input payload mailbox in bytes
    *pDWord = CR_IN_MB_SIZE;
    // Second dword is the size of the large output payload mailbox in bytes
    pDWord++;
    *pDWord = CR_OUT_MB_SIZE;
    // Third dword is the NgnAcpi Buffer Size
    pDWord++;
    *pDWord = INPUT_BUF_SIZE;
    // Output Length should be 3 DWORDs
    mNgnAcpiSmmInterface->OutputLength = 12;
    mNgnAcpiSmmInterface->Status = DSM_STS_SUCCESS;
    break;
  case WRITE_LP_INPUT_MB:
    DEBUG((EFI_D_ERROR, "CRP: Vendor Specific; WRITE LargePayload Input MB, SubOpCode=0x%x\n", SubOpCode));
    // Write Large Payload Input Mailbox
    // allows the caller to transfer the data stored in the input buffer to the large payload region input mailbox.
    pDWord = (UINT32 *)&mNgnAcpiSmmInterface->InputBuffer[0];
    // First dword contains the number of bytes to write to the Large Input Payload Mailbox
    NumBytes = *pDWord;
    pDWord++;
    // Next dword contains the Large Payload Input Mailbox Offset
    MBOffset = *pDWord;
    pDWord++;
    if (NumBytes > MAX_INPUT_BUF_SIZE - ((UINT8 *)pDWord - mNgnAcpiSmmInterface->InputBuffer)) {
      mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
                DSM_STS_VEN_SPECIFIC_ERROR,
                DSM_STS_EXT_INVALID_BYTES_TO_XFER
              );
      break;
    }
    // Since we're writing LP InputPayload, Output Length needs to be just 0;
    mNgnAcpiSmmInterface->OutputLength = 0;
    if (MBOffset > CR_BIOS_LARGE_PAYLOAD_SIZE) {
      mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
                DSM_STS_VEN_SPECIFIC_ERROR,
                DSM_STS_EXT_INVALID_MB_OFFSET
              );
      break;
    }
    // If the transfer crosses Large MB InPayload to OutPayload
    if ((MBOffset + NumBytes) > CR_BIOS_LARGE_PAYLOAD_SIZE) {
      mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
                DSM_STS_VEN_SPECIFIC_ERROR,
                DSM_STS_EXT_INVALID_BYTES_TO_XFER
              );
      break;
    }

    // pDWord now points to the buffer that needs to be written to the Large Input MB at the specified offset.
    Status = CopyFromBufferToLargePayload((UINT8 *)pDWord, 0, Socket, Ch, Dimm, MBOffset, NumBytes, LARGE_PAYLOAD_INPUT);
    ASSERT(Status == EFI_SUCCESS);

    if (mSystemMemoryMap->cpuStepping <= U0_REV_SKX) {
#ifdef __GNUC__
      asm("sfence");
#else
      _mm_sfence();
#endif
      Status = CopyFromLargePayloadToBuffer(Socket, Ch, Dimm, 0, (UINT8 *)pDWord, MBOffset, NumBytes, LARGE_PAYLOAD_INPUT);
      ASSERT(Status == EFI_SUCCESS);
    }

    if (Status == EFI_SUCCESS) {
      mNgnAcpiSmmInterface->Status = DSM_STS_SUCCESS;
    } else {
      mNgnAcpiSmmInterface->Status = DSM_STS_UNKNOWN_FAIL;
    }
    break;
  case READ_LP_OUTPUT_MB:
    DEBUG((EFI_D_ERROR, "CRP: Vendor Specific; READ LargePayload Output MB, SubOpCode=0x%x\n", SubOpCode));
    // Read Large Payload Output Mailbox allows the caller to transfer the data stored
    // in the large payload region output mailbox to a buffer.
    pDWord = (UINT32 *)&mNgnAcpiSmmInterface->InputBuffer[0];
    // First dword contains the number of bytes to write to the Large Input Payload Mailbox
    NumBytes = *pDWord;
    pDWord++;
    // Next dword contains the Large Payload Input Mailbox Offset
    MBOffset = *pDWord;
    if (NumBytes > MAX_OUTPUT_BUF_SIZE) {
      mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
                DSM_STS_VEN_SPECIFIC_ERROR,
                DSM_STS_EXT_ERROR_BUFFER_OVERRUN
              );
      mNgnAcpiSmmInterface->OutputLength = 0;
      break;
    }
    if (MBOffset > CR_BIOS_LARGE_PAYLOAD_SIZE) {
      mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
                DSM_STS_VEN_SPECIFIC_ERROR,
                DSM_STS_EXT_INVALID_MB_OFFSET
              );
      mNgnAcpiSmmInterface->OutputLength = 0;
      break;
    }
    // If the transfer crosses Large MB InPayload to OutPayload
    if ((MBOffset + NumBytes) > CR_BIOS_LARGE_PAYLOAD_SIZE) {
      mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
                DSM_STS_VEN_SPECIFIC_ERROR,
                DSM_STS_EXT_INVALID_BYTES_TO_XFER
              );
      mNgnAcpiSmmInterface->OutputLength = 0;
      break;
    }
    Status = CopyFromLargePayloadToBuffer(Socket, Ch, Dimm, MBOffset, &mNgnAcpiSmmInterface->OutputBuffer[0], 0, NumBytes, LARGE_PAYLOAD_OUTPUT);

    //
    // CR workaround:
    // perform 2nd read only for FNV & EKV (1st gen)
    if (IsNvmCtrlFirstGen(pDimm)) {
      Status = CopyFromLargePayloadToBuffer(Socket, Ch, Dimm, MBOffset, &mNgnAcpiSmmInterface->OutputBuffer[0], 0, NumBytes, LARGE_PAYLOAD_OUTPUT);
    }

    if (Status == EFI_SUCCESS) {
      mNgnAcpiSmmInterface->Status = DSM_STS_SUCCESS;
      mNgnAcpiSmmInterface->OutputLength = NumBytes;
    } else {
      mNgnAcpiSmmInterface->Status = DSM_STS_UNKNOWN_FAIL;
      mNgnAcpiSmmInterface->OutputLength = 0;
    }
    break;
  case GET_BOOT_STATUS:
    DEBUG((EFI_D_ERROR, "CRP: Vendor Specific; Get Boot Status, SubOpCode=0x%x\n", SubOpCode));
    // Get Boot Status Reg which is located at DPA offset of 0x20000.
    // Pass in address of Spa (needs a pointer to Spa)
    Status = XlateDpaOffsetToSpa(pDimm, pDimm->CntrlBase, (UINT64)CR_MB_BOOT_STATUS_OFFSET, &Spa);
    if (Status == EFI_SUCCESS) {
      pOutBuf = (UINT64 *)(UINTN)&mNgnAcpiSmmInterface->OutputBuffer[0];
      BootStatus = *(volatile UINT64 *)Spa; // Spa is the pointer to the register SPA
      *pOutBuf = BootStatus;
      mNgnAcpiSmmInterface->Status = DSM_STS_SUCCESS;
      mNgnAcpiSmmInterface->OutputLength = sizeof(UINT64);
    } else {
      mNgnAcpiSmmInterface->Status = DSM_STS_NOT_SUPPORTED;
      mNgnAcpiSmmInterface->OutputLength = 0;
    }
    break;
  default:
    DEBUG((EFI_D_ERROR, "CRP: Vendor Specific; Status Not supported, SubOpCode=0x%x\n", SubOpCode));
    mNgnAcpiSmmInterface->Status = DSM_STS_NOT_SUPPORTED;
    mNgnAcpiSmmInterface->OutputLength = 0;
    break;
  }
  return (Status);
}

/**

  This routine handles Vendor Specific commands from the OS
  layer. This is implemented as per new Intel NVM DIMM FW
  Interface Table. Basically this means, we need to handle
  commands to NVMCTLR Controller as well as BIOS Emulated Commands
  as well.

  @param Socket - Socket index at system level
  @param Ch   - Channel index at socket level
  @param Dimm - DIMM slot index within DDR channel

  @return Status

**/
EFI_STATUS
VendorSpecificCommand(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  EFI_STATUS                                Status = EFI_SUCCESS;
  PAEP_DIMM                                 pDimm = NULL;
  UINT16                                    OpCode;
  UINT8                                     *pData;
  UINT8                                     *pBuf;
  UINT8                                     i;
  UINT32                                    DataSizeCopied;
  CR_MAILBOX                                *pMBox;
  MB_SMM_STATUS_FNV_SPA_MAPPED_0_STRUCT     *pSmmStatusReg;
  UINT64                                    Spa;

  OpCode = (UINT16)mNgnAcpiSmmInterface->InputOpCode;

  DEBUG((EFI_D_ERROR, "CRP: Vendor Specific NVMCTL Command!\n"));
  pDimm = GetDimm(Socket, Ch, Dimm);
  if (pDimm == NULL) {
    Status = EFI_INVALID_PARAMETER;
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    mNgnAcpiSmmInterface->OutputLength = 0;
    return (Status);
  }
  // print the BOOT STATUS reg
  Status = XlateDpaOffsetToSpa(pDimm, pDimm->CntrlBase, (UINT64)CR_MB_BOOT_STATUS_OFFSET, &Spa);
  if (Status == EFI_SUCCESS) {
      DEBUG((DEBUG_INFO, "CRP: Vendor Specific NVMCTL Command; BootStatus Reg=0x%x\n", *(volatile UINT64 *)Spa));
  }

  //
  // If BIOS Emulated Command, then handle it in another routine
  if ((OpCode & 0xff) == BIOS_EMULATED_COMMAND) {
    Status = HandleBIOSEmulatedCommands(Socket, Ch, Dimm);
    return (Status);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);

    //
    // If there is any Input Payload for the command, copy it into the InPayload of the pDimm.
    if (mNgnAcpiSmmInterface->InputLength) {
      pBuf = &mNgnAcpiSmmInterface->InputBuffer[0];
      DataSizeCopied = 0;
      for (i = 0; i < CR_IN_PAYLOAD_NUM; i++) {
        pData = (UINT8 *)(UINTN)pMBox->pInPayload[i];
        CopyMem (pData, pBuf, sizeof (UINT32));
        pBuf += sizeof(UINT32);
        DataSizeCopied += sizeof(UINT32);
        if (DataSizeCopied >= mNgnAcpiSmmInterface->InputLength) {
          break;
        }
      }
    }
  } else {
    //
    // If there is any Input Payload for the command, copy it into the InPayload of the pDimm.
    if (mNgnAcpiSmmInterface->InputLength) {
      pBuf = &mNgnAcpiSmmInterface->InputBuffer[0];
      DataSizeCopied = 0;
      for (i = 0; i < CR_IN_PAYLOAD_NUM; i++) {
        DurableCacheLineWrite ((UINTN)pMBox->pInPayload[i], *(UINT32 *)pBuf, sizeof (UINT32));
        pBuf += sizeof(UINT32);
        DataSizeCopied += sizeof(UINT32);
        if (DataSizeCopied >= mNgnAcpiSmmInterface->InputLength) {
          break;
        }
      }
    }
  }
  //
  // Having copied the Input Payload, send the command
  Status = SendFnvCommand(pDimm, OpCode);

  if (Status == EFI_SUCCESS) {
    pBuf = &mNgnAcpiSmmInterface->OutputBuffer[0];
    for (i = 0; i < CR_OUT_PAYLOAD_NUM; i++) {
      pData = (UINT8 *)(UINTN)pMBox->pOutPayload[i];
      CopyMem (pBuf, pData, sizeof (UINT32));
      pBuf += sizeof(UINT32);
    }
    mNgnAcpiSmmInterface->OutputLength = CR_OUT_PAYLOAD_SIZE;
    mNgnAcpiSmmInterface->Status = DSM_STS_SUCCESS;
  } else {
    mNgnAcpiSmmInterface->OutputLength = 0;

    //
    // if error, we want to return FW Mbox error code as extended status
    pSmmStatusReg = (MB_SMM_STATUS_FNV_SPA_MAPPED_0_STRUCT *)pMBox->pStatus;

    mNgnAcpiSmmInterface->Status = (UINT32)CREATE_DSM_EXT_STATUS(
      DSM_STS_VEN_SPECIFIC_ERROR,
      (UINT32)pSmmStatusReg->Bits.stat
      );
  }
  return Status;
}

/**
  Routine Description: Check, whether given memory region can be scrubbed.

  @param[in] SpaAddress address of region
  @param[in] Length length of region

  @retval EFI_SUCCESS - success, region can be scrubbed
  @retval EFI_INVALID_PARAMETER - region can't be scrubbed

 */
EFI_STATUS
CheckArsRegion(
  UINT64  SpaAddress,
  UINT64  Length
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT64          SadBaseAddr;
  UINT64          SadStart;
  UINT64          Limit;
  UINT8           Skt;
  UINT8           Ch;
  UINT8           Dimm;

  //
  // Given the Spa, lets get the Socket, Ch & Dimm that this Spa is mapped into.
  DEBUG ((EFI_D_ERROR, "[CR debug]: ARS: ARSStart - SpaStart = 0x%1lx, Length = 0x%1lx\n", SpaAddress, Length));
  Status = GetDimmInfoFromSpa (SpaAddress, &SadBaseAddr, &Skt, &Ch, &Dimm);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((EFI_D_ERROR, "[CR debug]: ARS: ARSStart - SadBaseAddr = 0x%llx\n", SadBaseAddr));
  DEBUG ((EFI_D_ERROR, "[CR debug]: ARS: ARSStart - ARSStart is in Skt = %d, Ch = %d, Dimm = %d\n", Skt, Ch, Dimm));

  //
  // The first check is to make sure the entire ARS Range provided is in PMEM Region(s).
  while (SadBaseAddr < (SpaAddress + Length)) {
    // Check and if the range is in the PMEM Region, if it isn't return error status
    if (!IsPmemRgn (SadBaseAddr)) {
      DEBUG ((EFI_D_ERROR, "[CR debug]: ARS: SadBaseAddr = %llx - Not a PMEM region, returning Invalid Param\n", SadBaseAddr));
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    Status = GetSadLimits (SadBaseAddr, Skt, &SadStart, &Limit);
    if (EFI_ERROR (Status)) {
      Status = EFI_INVALID_PARAMETER;
    }

    SadBaseAddr += Limit;
  }

  return Status;
}

/**

  Function returns correct DSM status code for NVDIMM device based on
  EFI status code.

  @param DsmType - type of DSM (root/device): ACPI_DSM_DEVICE or ACPI_DSM_ROOT
  @param Status  - EFI status code returned from functions interacting with FW


  @return DSM status code

**/
UINT32
GetDsmStatusCode(
   ACPI_DSM_TYPE  DsmType,
   EFI_STATUS     Status
   )
{
  UINT32 DSMStatus;

  switch(Status) {
    case EFI_SUCCESS:
      if (DsmType == ACPI_DSM_DEVICE){
        DSMStatus = DSM_STS_SUCCESS;
      } else {
        DSMStatus = DSM_STS_ROOT_SUCCESS;
      }
      break;
    case EFI_UNSUPPORTED:
      if (DsmType == ACPI_DSM_DEVICE){
        DSMStatus = DSM_STS_NOT_SUPPORTED;
      } else {
        DSMStatus = DSM_STS_ROOT_NOT_SUPPORTED;
      }
       break;
    case EFI_INVALID_PARAMETER:
      if (DsmType == ACPI_DSM_DEVICE){
        DSMStatus = DSM_STS_INVALID_PARAMS;
      } else {
        DSMStatus = DSM_STS_ROOT_INVALID_PARAMS;
      }
      break;
    case EFI_DEVICE_ERROR:
      if (DsmType == ACPI_DSM_DEVICE){
        DSMStatus = DSM_STS_HW_ERROR;
      } else {
        DSMStatus = DSM_STS_ROOT_HW_ERROR;
      }
      break;
    case EFI_TIMEOUT:
    case EFI_NOT_READY:
      if (DsmType == ACPI_DSM_DEVICE){
        DSMStatus = DSM_STS_RETRY_SUGGESTED;
      } else {
        DSMStatus = DSM_STS_ROOT_RETRY_SUGGESTED;
      }
      break;
    default:
      if (DsmType == ACPI_DSM_DEVICE){
        DSMStatus = DSM_STS_UNKNOWN_FAIL;
      } else {
        DSMStatus = DSM_STS_ROOT_UNKNOWN_FAIL;
      }
      break;
  }
  return DSMStatus;
}

/**

  This routine is gets Vendor Specific Command Effect Log
  from the NVMCTLR Controller f/w.

  @param Socket - Socket index at system level
  @param Ch   - Channel index at socket level
  @param Dimm - DIMM slot index within DDR channel

  @return Status

**/
EFI_STATUS
GetVendorSpecificCommandEffectLog(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  PAEP_DIMM       pDimm = NULL;
  EFI_STATUS      Status = EFI_SUCCESS;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  // CR workaround: For Ax/Bx/H0/M0/U0 step use DurableCacheLineWrite instead of WpqFlush.
  // WPQ Flush causes system hang for these steppings
  if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
    IssueWpqFlush(pDimm->SocketId, pDimm->ImcId, TRUE);
  }
  Status = SendFnvCommand(pDimm, GET_COMMAND_EFFECT_LOG);

  return (Status);
}

/**

  This routine copies Command Effects Log Data to the SMM Output buffer.
  A part of the data comes from small payload region and some from
  the large payload region.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @param Socket - Socket index at system level
  @param Ch   - Channel index at socket level
  @param Dimm - DIMM slot index within DDR channel

  @return Status

**/
EFI_STATUS
CopyEffectLogDataToSmmBuffer(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;
  UINT32          *pData, LogCount;
  UINT32          *pOutBuf;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  //
  // First DWord of the small payload region has the number of log entries.
  pData = (UINT32 *)(UINTN)pMBox->pOutPayload[0];
  LogCount = *pData;
  pOutBuf = (UINT32 *)&mNgnAcpiSmmInterface->OutputBuffer[0];
  *pOutBuf = LogCount;
  pOutBuf++;
  if (LogCount * sizeof (UINT64) > MAX_OUTPUT_BUF_SIZE - (UINT32)((UINT8 *) pOutBuf - mNgnAcpiSmmInterface->OutputBuffer)) {
    return EFI_DEVICE_ERROR;
  }
  CopyFromLargePayloadToBuffer(Socket, Ch, Dimm, 0, (UINT8 *)pOutBuf, 0, (UINT32)(LogCount * sizeof(UINT64)), LARGE_PAYLOAD_OUTPUT);

  //
  // CR workaround:
  // perform 2nd read only for FNV & EKV (1st gen)
  if (IsNvmCtrlFirstGen(pDimm)) {
    CopyFromLargePayloadToBuffer(Socket, Ch, Dimm, 0, (UINT8 *)pOutBuf, 0, (UINT32)(LogCount * sizeof(UINT64)), LARGE_PAYLOAD_OUTPUT);
  }
  //
  // Include LogCount into output length.
  mNgnAcpiSmmInterface->OutputLength = (sizeof(UINT64) * LogCount) + sizeof(UINT32);

  return EFI_SUCCESS;
}

/**

   DSM method. Function returns adress range scrub capabilities
   (is there an option to run ARS) for given memory region.
   Functions changes mNgnAcpiSmmInterface as a side effect.

   @retval EFI_SUCCESS - command successful
   @retval EFI_INVALID_PARAM - ARS range out of proper boundaries

 */
EFI_STATUS
DsmQueryArsCapabilities(
  VOID
  )
{
  EFI_STATUS                Status;
  ARS_DSM_ROOT_CAPABILITY   *pArsRootCapabilities;
  UINT64                    arsAddress, arsLength;

  DEBUG((EFI_D_ERROR, "[CR debug]: %s\n", __FUNCTION__));

  pArsRootCapabilities = (ARS_DSM_ROOT_CAPABILITY *) mNgnAcpiSmmInterface->OutputBuffer;

  arsAddress = (mNgnAcpiSmmInterface->ArsStartAddress / ARS_ALIGNMENT_SIZE) * ARS_ALIGNMENT_SIZE;;
  arsLength =  mNgnAcpiSmmInterface->ArsLength;

  pArsRootCapabilities->MaxArsQueryBuffersSize =
      sizeof(UINT32) + //Status + Status extended
      sizeof(ARS_DSM_ROOT_QUERY_HEADER) +
      MAX_SYS_DIMM * MAX_ERRORS_PER_DIMM * sizeof(ARS_ERROR_RECORD);

  //Uncorrected range currently not supported
  pArsRootCapabilities->ClearUncorrectedUnitSize = DSM_CLEAR_UNCORRECTABLE_UNIT_SIZE_NOT_SUPPORTED;
  pArsRootCapabilities->Reserved = 0;

  mNgnAcpiSmmInterface->OutputLength = sizeof(*pArsRootCapabilities);

  Status = CheckArsRegion(arsAddress, arsLength);
  if (EFI_ERROR(Status)) {
    Status = EFI_INVALID_PARAMETER;
    mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_ROOT, Status);
    return Status;
  }

  //
  // Currently we only support scrubbing the PMEM region
  mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
                                        GetDsmStatusCode(ACPI_DSM_ROOT, Status),
                                        PMEM_SCRUB_SUPPORTED);

  DEBUG((EFI_D_ERROR, "[CR debug]: %s end\n", __FUNCTION__ ));

  return Status;
}

/**

  DSM method. This function handles Start Address Range Scrub command.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @retval EFI_SUCCESS - command successful
  @retval EFI_INVALID_PARAM - ARS range out of proper boundaries or unsupported
                              memory type given

**/
EFI_STATUS
DsmStartArs(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT32          *pArsTime;
  UINT64          arsAddress, arsLength;

  DEBUG((EFI_D_ERROR, "[CR debug]: %s\n", __FUNCTION__ ));

  pArsTime = (UINT32*) &mNgnAcpiSmmInterface->OutputBuffer[0];
  *pArsTime = 0;

  mNgnAcpiSmmInterface->OutputLength = sizeof(*pArsTime);

  if (mNgnAcpiSmmInterface->ArsType & VOLATILE_SCRUB_SUPPORTED ||
      !(mNgnAcpiSmmInterface->ArsType & PMEM_SCRUB_SUPPORTED)) {
    Status = EFI_INVALID_PARAMETER;
    mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_ROOT, Status);
    return Status;
  }

  arsAddress = (mNgnAcpiSmmInterface->ArsStartAddress / ARS_ALIGNMENT_SIZE) * ARS_ALIGNMENT_SIZE;;
  arsLength  = mNgnAcpiSmmInterface->ArsLength;

  Status = CheckArsRegion(arsAddress, arsLength);
  if (EFI_ERROR(Status)) {
    Status = EFI_INVALID_PARAMETER;
    mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_ROOT, Status);
    return (Status);
  }

  *pArsTime = ARS_DEFAULT_TIME;

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_ROOT, Status);

  DEBUG((EFI_D_ERROR, "[CR debug]: %s end \n", __FUNCTION__ ));
  return (Status);

}

/**

  DSM method. This functions gathers status from the previous Start
  ARS commands for dimms that were participating in the Start
  Address Range Scrub command from the OS and returns Status as
  specified in the DSM Spec.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return Status

**/
EFI_STATUS
DsmQueryArsStatus(
   VOID
   )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  ARS_DSM_ROOT_QUERY_HEADER   *pArsDsmHeader;

  DEBUG((EFI_D_ERROR, "[CR debug]: %s\n", __FUNCTION__ ));

  pArsDsmHeader = (ARS_DSM_ROOT_QUERY_HEADER *) &mNgnAcpiSmmInterface->OutputBuffer[0];
  pArsDsmHeader->NumErrorRecords = 0;
  pArsDsmHeader->StartSpa        = 0;
  pArsDsmHeader->Length          = 0;
  pArsDsmHeader->RestartSpa      = 0;
  pArsDsmHeader->RestartLength   = 0;
  pArsDsmHeader->Type  = 0;
  pArsDsmHeader->Flags = 0;
  pArsDsmHeader->NumErrorRecords = 0;
  pArsDsmHeader->OutputLength = sizeof(*pArsDsmHeader);

  mNgnAcpiSmmInterface->OutputLength = pArsDsmHeader->OutputLength;

  mNgnAcpiSmmInterface->Status = CREATE_DSM_EXT_STATUS(
     GetDsmStatusCode(ACPI_DSM_ROOT, Status),
     DSM_STS_EXT_ROOT_ARS_NOT_PERFORMED);

  DEBUG((EFI_D_ERROR, "[CR debug]: %s end\n", __FUNCTION__ ));

  return (Status);
}

/**

   DSM method. Function initializes Clear Uncorrectable Error procedure.
   Not supported in current HW. Returns EFI_INVALID_PARAMETER.
   Functions changes mNgnAcpiSmmInterface as a side effect.

   @retval EFI_INVALID_PARAM - currently not supported

 */
EFI_STATUS
DsmClearUncorrectableError(
  VOID
  )
{
  EFI_STATUS                  Status = EFI_INVALID_PARAMETER;
  ARS_DSM_CLEAR_UNCORR_ERROR  *pArsDsmHeader;

  DEBUG((EFI_D_ERROR, "[CR debug]: %s\n", __FUNCTION__ ));

  pArsDsmHeader = (ARS_DSM_CLEAR_UNCORR_ERROR *) mNgnAcpiSmmInterface->OutputBuffer;

  pArsDsmHeader->Reserved = 0;
  pArsDsmHeader->ErrorRangeLength = 0;  // currently not supported

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_ROOT, Status);
  mNgnAcpiSmmInterface->OutputLength = sizeof(*pArsDsmHeader);

  DEBUG((EFI_D_ERROR, "[CR debug]: %s end \n", __FUNCTION__ ));
  return Status;
}

/**

  DSM method. Function gets Smart and health information from FW and
  sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI status returned from FW interaction command or EFI_INVALID_PARAMETER
          if Dimm is not found.

**/
EFI_STATUS
DsmSmartAndHealthInfo(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;
  UINT8           *pBuf, *pOutBuf;
  UINT32          i;

  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    mNgnAcpiSmmInterface->OutputLength = 0;
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  Status = GetSmartHealth(Socket, Ch, Dimm);

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);

  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CRP: DSM VS SmartAndHealthInfo\n"));
    pOutBuf = &mNgnAcpiSmmInterface->OutputBuffer[0];
    for (i = 0;  i < CR_OUT_PAYLOAD_NUM; i++) {
      pBuf = (UINT8 *)(UINTN)pMBox->pOutPayload[i];
      CopyMem (pOutBuf, pBuf, sizeof (UINT32));
      pOutBuf += sizeof(UINT32);
    }
    mNgnAcpiSmmInterface->OutputLength = SMART_HEALTH_OUTPUT_SIZE;
  }
  else {
    mNgnAcpiSmmInterface->OutputLength = 0;
  }

  return Status;
}

/**

  DSM method. Function gets Smart related threshold info from FW and
  sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI status returned from FW interaction command or EFI_INVALID_PARAMETER
          if Dimm is not found.

**/
EFI_STATUS
DsmGetSmartThreshold(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;

  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    mNgnAcpiSmmInterface->OutputLength = 0;
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  Status = GetSMARTThreshold(Socket, Ch, Dimm);

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);

  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CRP: DSM VS GetSmartThreshold\n"));
    CopyMem ((UINT8 *)&mNgnAcpiSmmInterface->OutputBuffer[0],
             (UINT8 *) (UINTN)pMBox->pOutPayload[0],
             sizeof (UINT32)
             );
    CopyMem ((UINT8 *)(&mNgnAcpiSmmInterface->OutputBuffer[0] + sizeof (UINT32)),
             (UINT8 *)(UINTN)pMBox->pOutPayload[1],
             sizeof (UINT32)
             );
    mNgnAcpiSmmInterface->OutputLength = SMART_THRESHOLD_OUTPUT_SIZE;
  } else {
    mNgnAcpiSmmInterface->OutputLength = 0;
  }

  return Status;
}

/**

  DSM method. Function gets block mode NVDIMM flags from FW and
  sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI status returned from FW interaction command or EFI_INVALID_PARAMETER
          if Dimm is not found.

**/
EFI_STATUS
DsmGetBlockNVDIMMFlags(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;
  PAEP_DIMM       pDimm = NULL;
  CR_MAILBOX      *pMBox;
  UINT8           *pBuf, *pOutBuf;

  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    mNgnAcpiSmmInterface->OutputLength = 0;
    return (EFI_INVALID_PARAMETER);
  }

  pMBox = GetMailbox(pDimm, mUseOSMailbox);

  Status = SendFnvIdentifyDevice(Socket, Ch, Dimm);

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);

  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CRP: DSM VS GetBlockNVDIMMFlags\n"));
    pBuf = (UINT8 *)(UINTN)pMBox->pOutPayload[3];
    // pBuf now points to byte 12, make it point to byte 14
    pBuf += 2;
    pOutBuf = &mNgnAcpiSmmInterface->OutputBuffer[0];
    *pOutBuf = *pBuf;
    mNgnAcpiSmmInterface->OutputLength = sizeof(UINT32);
  } else {
    mNgnAcpiSmmInterface->OutputLength = 0;
  }

  return Status;
}

/**

  DSM method. Function gets namespace label size from FW and
  sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI status returned from FW interaction command or EFI_INVALID_PARAMETER
          if Dimm is not found.

**/
EFI_STATUS
DsmGetNamespaceLabelSize(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;
  UINT32          *pBuf;
  UINT32          Size;

  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  if (GetDimm(Socket, Ch, Dimm) == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    mNgnAcpiSmmInterface->OutputLength = 0;
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPlatformConfigDataAreaSize(
      Socket,
      Ch,
      Dimm,
      OS_PARTITION_OTHER,
      USE_OS_MAILBOX,
      &Size
    );

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);

  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CRP: DSM VS GetNamespaceLabelSize\n"));
    pBuf = (UINT32 *)&mNgnAcpiSmmInterface->OutputBuffer[0];
    *pBuf = Size;
    pBuf++;
    // Next Dword is the Optimal Buffer Size to be used for Set/Get Platform config data
    // which should be our buffer size
    *pBuf = OUTPUT_BUF_SIZE;
    mNgnAcpiSmmInterface->OutputLength = 2 * sizeof(UINT32);
  } else {
    mNgnAcpiSmmInterface->OutputLength = 0;
  }

  return Status;
}

/**

  DSM method. Function gets namespace label data from FW and
  sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI status returned from FW interaction command or EFI_INVALID_PARAMETER
          if Dimm is not found.

**/
EFI_STATUS
DsmGetNamespaceLabelData(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;
  UINT32          size;
  AEP_DIMM        *pDimm;

  mNgnAcpiSmmInterface->OutputLength = 0;

  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    return EFI_INVALID_PARAMETER;
  }

  if (mNgnAcpiSmmInterface->InputLength > MAX_OUTPUT_BUF_SIZE) {
    mNgnAcpiSmmInterface->Status = DSM_STS_INVALID_PARAMS;
    return EFI_INVALID_PARAMETER;
  }

  size =  mNgnAcpiSmmInterface->InputLength;

  Status = GetPlatformConfigDataAreaData(
      Socket,
      Ch,
      Dimm,
      &mNgnAcpiSmmInterface->OutputBuffer[0],
      &size,
      mNgnAcpiSmmInterface->InputOffset,
      OS_PARTITION_OTHER,
      USE_LARGE_PAYLOAD,
      USE_OS_MAILBOX
    );

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);

  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CRP: DSM VS GetNamespaceLabelData\n"));
    mNgnAcpiSmmInterface->OutputLength = size;
  }

  return Status;
}

/**

  DSM method. Function sets namespace label data, with command to FW and
  sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI status returned from FW interaction command or EFI_INVALID_PARAMETER
          if Dimm is not found.

**/
EFI_STATUS
DsmSetNamespaceLabelData(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;

  mNgnAcpiSmmInterface->OutputLength = 0;
  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  if (GetDimm(Socket, Ch, Dimm) == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    return EFI_INVALID_PARAMETER;
  }

  if (mNgnAcpiSmmInterface->InputLength > MAX_INPUT_BUF_SIZE) {
    mNgnAcpiSmmInterface->Status = DSM_STS_INVALID_PARAMS;
    return EFI_INVALID_PARAMETER;
  }

  DEBUG((EFI_D_ERROR, "CRP: DSM VS SetNamespaceLabelData\n"));
  Status = SetPlatformConfigDataAreaData(
    Socket,
    Ch,
    Dimm,
    &mNgnAcpiSmmInterface->InputBuffer[0],
    mNgnAcpiSmmInterface->InputLength,
    mNgnAcpiSmmInterface->InputOffset,
    OS_PARTITION_OTHER,
    USE_LARGE_PAYLOAD,
    USE_OS_MAILBOX
    );

  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);

  return Status;
}

/**

  DSM method. Function returns the maximum data size of output buffer for retrieving
  the Vendor-Specific Command Effect Log and sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI_SUCCESS

**/
EFI_STATUS
DsmGetVendorSpecificCommandEffectLogSize(
  VOID
  )
{
  UINT32 *pBuf;

  pBuf = (UINT32 *)&mNgnAcpiSmmInterface->OutputBuffer[0];
  *pBuf = ACPI_SMM_BUF_SIZE;
  DEBUG((EFI_D_ERROR, "CRP: DSM GetVendorSpecificCommandEffectLogSize\n"));
  mNgnAcpiSmmInterface->Status = DSM_STS_SUCCESS;
  mNgnAcpiSmmInterface->OutputLength = sizeof(UINT32);

  return EFI_SUCCESS;
}

/**

  DSM method. Function returns the Command Effect Log for all of
  the Vendor-Specific Commands and sets ACPI structures accordingly.
  Functions changes mNgnAcpiSmmInterface as a side effect.

  @return EFI status returned from GetVendorSpecificCommandEffectLog or
          EFI_INVALID_PARAMETER if Dimm is not found.

**/
EFI_STATUS
DsmGetVendorSpecificCommandEffectLog(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;

  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  mNgnAcpiSmmInterface->OutputLength = 0;

  if (GetDimm(Socket, Ch, Dimm) == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    return EFI_INVALID_PARAMETER;
  }

  Status = GetVendorSpecificCommandEffectLog(Socket, Ch, Dimm);

  if (EFI_ERROR(Status)) {
    mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);
    return Status;
  }

  DEBUG((EFI_D_ERROR, "CRP: DSM GetVendorSpecificCommandEffectLog\n"));
  Status = CopyEffectLogDataToSmmBuffer(Socket, Ch, Dimm);
  mNgnAcpiSmmInterface->Status = GetDsmStatusCode(ACPI_DSM_DEVICE, Status);
  return Status;

}

/**

  DSM method. Function provides access to the vendor specific commands
  and sets ACPI structures accordingly. Functions changes mNgnAcpiSmmInterface
  as a side effect.

  @return EFI status returned from VendorSpecificCommand or
          EFI_INVALID_PARAMETER if Dimm is not found.

**/
EFI_STATUS
DsmVendorSpecificCommand(
  VOID
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Socket = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0;

  GetDimmInfoFromDevHandle(&Socket, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

  if (GetDimm(Socket, Ch, Dimm) == NULL) {
    mNgnAcpiSmmInterface->Status = DSM_STS_NO_MEM_DEVICE;
    mNgnAcpiSmmInterface->OutputLength = 0;
    return EFI_INVALID_PARAMETER;
  }

  Status = VendorSpecificCommand(Socket, Ch, Dimm);

  return Status;
}

/**

  This routine handles Address Range Scrub related DSM commands.

  @return Status

**/
EFI_STATUS
HandleARSCommandsFromOS(
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  DEBUG((EFI_D_ERROR, "[CR debug]: Handle ARS - Function %d\n", mNgnAcpiSmmInterface->FunctionIndex ));

  switch (mNgnAcpiSmmInterface->FunctionIndex) {
    case DSM_ROOT_QUERY_ARS_CAPABILITES:
      Status = DsmQueryArsCapabilities();
      break;
    case DSM_ROOT_START_ARS:
      Status = DsmStartArs();
      break;
    case DSM_ROOT_QUERY_ARS_STATUS:
      Status = DsmQueryArsStatus();
      break;
    case DSM_ROOT_CLEAR_UNCORRECTABLE_ERROR:
      Status = DsmClearUncorrectableError();
      break;
    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }
  return Status;
}

/**

  SMM callback for all DSM methods.

  @param DispatchHandle - not used
  @param DispatchContext - not used
  @param CommBuffer - not used
  @param CommBufferSize - not used

  @return EFI status returned by DSM methods or
          EFI_INVALID_PARAMETER if DSM command function index is out of bounds.

**/
EFI_STATUS
ServeNgnCommandsFromOS(
   IN       EFI_HANDLE                   DispatchHandle,
   IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
   IN OUT   VOID                         *CommBuffer,     OPTIONAL
   IN OUT   UINTN                        *CommBufferSize  OPTIONAL
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;

  //
  // The new encoding is that the device handle of _DSM commands for the root
  // (ARS Commands) will be 0xFFFFFFFF. Current spec has 3 ARS related functions
  if ((mNgnAcpiSmmInterface->FunctionIndex > 0) &&
      (mNgnAcpiSmmInterface->FunctionIndex < MAX_ROOT_DEVICE_FCNS) &&
      (mNgnAcpiSmmInterface->DeviceHandle == ROOT_DEVICE_ACPI_HANDLE)
    ) {
    // ARS commands to use BIOS MB only.
    mUseOSMailbox = FALSE;

    Status = HandleARSCommandsFromOS();
    return (Status);
  }

  //
  // OS Mailbox should be used
  mUseOSMailbox = TRUE;

  switch (mNgnAcpiSmmInterface->FunctionIndex) {
  case DSM_GET_SMART_INFO:
    Status = DsmSmartAndHealthInfo();
    break;
  case DSM_GET_SMART_THRESHOLD:
    Status = DsmGetSmartThreshold();
    break;
  case DSM_GET_BLOCK_FLAGS:
    Status = DsmGetBlockNVDIMMFlags();
    break;
  case DSM_GET_LABEL_SIZE:
    Status = DsmGetNamespaceLabelSize();
    break;
  case DSM_GET_LABEL_DATA:
    Status = DsmGetNamespaceLabelData();
    break;
  case DSM_SET_LABEL_DATA:
    Status = DsmSetNamespaceLabelData();
    break;
  case DSM_GET_VENDOR_LOG_SIZE:
    Status = DsmGetVendorSpecificCommandEffectLogSize();
    break;
  case DSM_GET_VENDOR_LOG:
    Status = DsmGetVendorSpecificCommandEffectLog();
    break;
  case DSM_VENDOR_COMMAND:
    Status = DsmVendorSpecificCommand();
    break;
  default:
    Status = EFI_INVALID_PARAMETER;
    break;
  }

  mUseOSMailbox = FALSE;

  return Status;
}

/**

  Routine Description:This routine reads the  current NGN dimm data from the HOB and writes it as PrevBootNGNDimmCfg variable to NVRAM.
  This variable will be used in the next boot by MRC code.


  @retval NULL

**/
VOID
PrevBootNGNDimmCfg (
  UINT8 action )
{
  EFI_STATUS                Status;
  struct prevBootNGNDimmCfg prevBootNGNDimmCfg;
  UINT8                     Socket;
  UINT8                     Channel;
  UINT8                     Dimm;
  UINT8                     i;

  if (action == ADD_VARIABLE)  {
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      for (Channel = 0; Channel < MAX_CH; Channel++) {
        if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].Enabled == 0) {
          continue;
        }

        for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
          if (!IsNvmDimm(Socket, Channel, Dimm)) {
            continue;
          }

          //Update the prevBootNGNDimmCfg structure based on the data in the host struct
          for (i = 0; i < NGN_MAX_MANUFACTURER_STRLEN; i++) {
            prevBootNGNDimmCfg.socket[Socket].channelList[Channel].dimmList[Dimm].manufacturer[i] = mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].manufacturer[i];
          }
          for (i = 0; i < NGN_MAX_SERIALNUMBER_STRLEN; i++) {
            prevBootNGNDimmCfg.socket[Socket].channelList[Channel].dimmList[Dimm].serialNumber[i] = mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].serialNumber[i];
          }
          for (i = 0; i < NGN_MAX_MODELNUMBER_STRLEN; i++) {
            prevBootNGNDimmCfg.socket[Socket].channelList[Channel].dimmList[Dimm].modelNumber[i] = mSystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].modelNumber[i];
          }
        }
      }
    }

    //Update the variable in NVRAM
    Status = gRT->SetVariable (
      PREV_BOOT_NGN_DIMM_CONFIG_VARIABLE_NAME,
      &gEfiPrevBootNGNDimmCfgVariableGuid,
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
      sizeof(struct prevBootNGNDimmCfg),
      &prevBootNGNDimmCfg
      );
    ASSERT_EFI_ERROR (Status);

  } else {

    //Update the variable in NVRAM
    Status = gRT->SetVariable (
      PREV_BOOT_NGN_DIMM_CONFIG_VARIABLE_NAME,
      &gEfiPrevBootNGNDimmCfgVariableGuid,
      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
      0,
      NULL
      );

    switch (Status) {
      case EFI_SUCCESS: // variable was successfully removed
        break;
      case EFI_NOT_FOUND: // variable was not set before
        break;
      default:
        ASSERT_EFI_ERROR(Status);
    }
  }
}

/**

  Routine Description: Get/Retrieve the Media Status via Boot status register value.

  @param pNVMDimm - Pointer to Dimm structure

  @retval MEDIA_READY                 - media is ready
  @retval WARN_NVMCTRL_MEDIA_NOTREADY - media not ready
  @retval WARN_NVMCTRL_MEDIA_INERROR  - media is in error state
  @retval WARN_NVMCTRL_MEDIA_RESERVED - media status is reserved
  @retval MEDIA_UNKNOWN               - can't get media status

**/
UINT8
GetMediaStatus(
  AEP_DIMM *NvmDimmPtr
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       MediaStatus;
  UINT64      Bsr;

  Status = XlateDpaOffsetToSpa(NvmDimmPtr, NvmDimmPtr->CntrlBase, (UINT64)CR_MB_BOOT_STATUS_OFFSET, &Bsr);
  if (Status == EFI_SUCCESS) {

    MediaStatus = (*(volatile UINT64 *)Bsr >> 16) & 0x3;
    if (MediaStatus == MEDIA_READY) {
      return MediaStatus;
    } else if (MediaStatus == MEDIA_NOTREADY) {
      DEBUG((EFI_D_ERROR, "CRP: Problem with Media Access - Media NOT ready \n"));
      return WARN_NVMCTRL_MEDIA_NOTREADY;
    } else if (MediaStatus == MEDIA_INERROR) {
      DEBUG((EFI_D_ERROR, "CRP: Problem with Media Access - Media in Error \n"));
      return WARN_NVMCTRL_MEDIA_INERROR;
    } else {
      DEBUG((EFI_D_ERROR, "CRP: Problem with Media Access - Reserved! \n"));
      return WARN_NVMCTRL_MEDIA_RESERVED;
    }
  } else {
    return MEDIA_UNKNOWN;
  }
}

/**

Routine Description:CrystalRidgeStart gathers Control Region
Data from the SADs and initializes the internal data structs for
later enablement of the read/write of the NVMCTLR registers thru the
SAD interface. This function needs to be called during init
before the ReadFnvCfgMem & WriteFnvCfgMem can be called.

  @retval UINT32 - status

**/
EFI_STATUS
EFIAPI
CrystalRidgeStart (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  VOID                                *HobList;
  EFI_HOB_GUID_TYPE                   *GuidHob;
  EFI_STATUS                          Status = EFI_SUCCESS;
  EFI_HANDLE                          Handle = NULL;
  BOOLEAN                             InSmm = FALSE;
  EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL *NgnAcpiSmmInterfaceAreaPtr;
  EFI_PHYSICAL_ADDRESS                NgnAcpiSmmInterface;
  struct SystemMemoryMapHob           *TempSystemMemoryMap;

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, &mSmmBase);
  if (mSmmBase == NULL) {
    InSmm = FALSE;
  } else {
    mSmmBase->InSmm (mSmmBase, &InSmm);
    mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  }

  if (InSmm) {
    Status = mSmst->SmmLocateProtocol (
      &gEfiCpuCsrAccessGuid,
      NULL,
      &mCpuCsrAccess
      );
  } else {
    Status = gBS->LocateProtocol (
      &gEfiCpuCsrAccessGuid,
      NULL,
      &mCpuCsrAccess
      );
  }
  ASSERT_EFI_ERROR (Status);

  ZeroMem (&mCrystalRidgeProtocol, sizeof (EFI_CRYSTAL_RIDGE_PROTOCOL));
  ZeroMem (&mCrInfo, sizeof (CR_INFO));

  // NVM host support functions
  mCrystalRidgeProtocol.IsNvmDimm                       = IsNvmDimm;
  mCrystalRidgeProtocol.GetNumNvmDimms                  = GetNumNvmDimms;
  mCrystalRidgeProtocol.IsNvmAddress                    = IsNvmAddress;
  mCrystalRidgeProtocol.SpaToNvmDpa                     = SpaToNvmDpa;

  // Support for ACPI tables
  mCrystalRidgeProtocol.UpdateNvmAcpiTables             = UpdateNvmAcpiTables;
  mCrystalRidgeProtocol.UpdateNvmAcpiPcatTable          = UpdateNvmAcpiPcatTable;

  // NVM controller interface
  // Support for NvmController's config mem access
  mCrystalRidgeProtocol.NvmCtlrReadCfgMem               = NvmCtlrReadCfgMem;
  mCrystalRidgeProtocol.NvmCtlrWriteCfgMem              = NvmCtlrWriteCfgMem;

  // Support for Error Logging
  mCrystalRidgeProtocol.NvmCtlrSetHostAlert             = NvmCtlrSetHostAlert;
  mCrystalRidgeProtocol.NvmCtlrGetHostAlert             = NvmCtlrGetHostAlert;
  mCrystalRidgeProtocol.NvmCtlrSetAddressRangeScrub     = NvmCtlrSetAddressRangeScrub;
  mCrystalRidgeProtocol.NvmCtlrGetAddressRangeScrub     = NvmCtlrGetAddressRangeScrub;
  mCrystalRidgeProtocol.NvmCtlrErrorLogProgramSignal    = NvmCtlrErrorLogProgramSignal;
  mCrystalRidgeProtocol.NvmCtlrErrorLogDetectError      = NvmCtlrErrorLogDetectError;
  mCrystalRidgeProtocol.NvmCtlrGetLatestErrorLog        = NvmCtlrGetLatestErrorLog;

  // Support for Error Injection
  mCrystalRidgeProtocol.NvmCtlrEnableErrInjection       = NvmCtlrEnableErrInjection;
  mCrystalRidgeProtocol.NvmCtlrInjectPoisonError        = NvmCtlrInjectPoisonError;

  // Support for configuring Viral conditions
  mCrystalRidgeProtocol.NvmCtlrSetCfgDataPolicy         = NvmCtlrSetCfgDataPolicy;
  mCrystalRidgeProtocol.NvmCtlrGetCfgDataPolicy         = NvmCtlrGetCfgDataPolicy;

  // Support for PCD set/get
  // Used by external BNT tool - BIOS testing
  mCrystalRidgeProtocol.NvmCtlrGetPcd                   = GetPlatformConfigDataAreaData;
  mCrystalRidgeProtocol.NvmCtlrSetPcd                   = SetPlatformConfigDataAreaData;

  if (!InSmm) {
    //
    // Allocate Buffer for NgnAcpiSmmInterface
    NgnAcpiSmmInterface = 0xffffffff;
    Status = gBS->AllocatePages(
       AllocateMaxAddress,
       EfiACPIMemoryNVS,
       EFI_SIZE_TO_PAGES(sizeof(NGN_ACPI_SMM_INTERFACE)),
       &NgnAcpiSmmInterface
       );
    ASSERT_EFI_ERROR(Status);
    mNgnAcpiSmmInterface = (NGN_ACPI_SMM_INTERFACE *)NgnAcpiSmmInterface;
    ZeroMem(mNgnAcpiSmmInterface, sizeof(NGN_ACPI_SMM_INTERFACE));
    Handle = NULL;
    //
    // Now install this protocol
    mNgnAcpiSmmInterfaceArea.Area = mNgnAcpiSmmInterface;
    gBS->InstallProtocolInterface(
       &Handle,
       &gEfiNgnAcpiSmmInterfaceProtocolGuid,
       EFI_NATIVE_INTERFACE,
       &mNgnAcpiSmmInterfaceArea
       );
    ASSERT_EFI_ERROR(Status);

    //Allocate memory for SystemMemoryHob
    Status = gBS->AllocatePool (EfiBootServicesData, sizeof (struct SystemMemoryMapHob), (VOID **) &mSystemMemoryMap);

    // Allocate buffer for transferring data from Large Payload buffers
    Status = gBS->AllocatePool (EfiBootServicesData, sizeof (UINT8) * CR_BIOS_LARGE_PAYLOAD_SIZE, &mLargePayloadBuffer);
    ASSERT_EFI_ERROR(Status);
  } else {
    Status = gBS->LocateProtocol(&gEfiNgnAcpiSmmInterfaceProtocolGuid, NULL, &NgnAcpiSmmInterfaceAreaPtr);
    mNgnAcpiSmmInterface = NgnAcpiSmmInterfaceAreaPtr->Area;
    DEBUG((EFI_D_ERROR, "CRP: Found NgnAcpiSmmInterface = 0x%1lx\n", mNgnAcpiSmmInterface));
    ASSERT(mNgnAcpiSmmInterface);

    //Allocate memory for SystemMemoryHob
    Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (struct SystemMemoryMapHob), (VOID **) &mSystemMemoryMap);
    // Allocate buffer for transferring data from Large Payload buffers
    Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (UINT8) * CR_BIOS_LARGE_PAYLOAD_SIZE, &mLargePayloadBuffer);
    ASSERT_EFI_ERROR(Status);
  }

  //
  // Update HOB variable for PCI resource information
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable(&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR(Status);

  //
  // Search for the Memory Map GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob    = GetFirstGuidHob(&gEfiMemoryMapGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  TempSystemMemoryMap = GET_GUID_HOB_DATA(GuidHob);
  CopyMem(mSystemMemoryMap, TempSystemMemoryMap, sizeof(struct SystemMemoryMapHob));

  mCrInfo.NumAepDimms = 0;
  //
  // Get the SMBios Type17 Handles for all the Aep Dimms
  mNumSMBiosHandles = GetAndUpdateAepDimmSMBiosType17Handles();

  //
  // Get and update the Dimm_Info data structs for each and every
  // Aep (Ngn) Dimms installed in the system for later use.
  UpdateDimmInfo(MEM_TYPE_CTRL);

  //
  // Having created the Dimm Structs based on the Control Region, will
  // need SAD Rule base address and Interleave Dimm Num for PMEM and Block regions
  // later for Spa to Dpa translation as these are need by both RAS & DSM.
  UpdateDimmInfo(MEM_TYPE_PMEM);

  //
  // Create NVMDIMM Present Bitmaps based on ALL Aep Dimms installed in all sockets
  // for use by AcpiSmmInterface.
  CreateDimmPresentBitmaps();

  if (mNgnAcpiSmmInterface->NvDimmConfigBitmap0 || mNgnAcpiSmmInterface->NvDimmConfigBitmap1) {
    mNgnAcpiSmmInterface->IsAepDimmPresent = 1;
  }

  if (!InSmm) {
    Status = gBS->LocateProtocol(&gEfiMpServiceProtocolGuid,   NULL, &mMpService);
    ASSERT_EFI_ERROR(Status);

    // CR workaround: WPQ Flush causes system hang on Ax/Bx/H0/M0/U0-step
    // Do not initialize WPQ Flush for Ax/Bx/H0/M0/U0-step
    if (mSystemMemoryMap->cpuStepping > U0_REV_SKX) {
      ProgramFlushHintAddressForWpqFlush();
    }

    if ((mSystemMemoryMap->cpuStepping >= B0_REV_SKX)
      && (mSystemMemoryMap->MemSetup.crQosConfig != 0)
      && (mSystemMemoryMap->MemSetup.crQosConfig <= 5)) {
      ProgramCrQosForAllCores();
    }
  }

  //
  // get some details of these dimms using HOB.
  InitAllDimmsFromHOB();

  if (!InSmm) {
    SetMemAttributes();
    if (mSystemMemoryMap->MemSetup.dfxMemSetup.dfxDimmManagement == DIMM_MGMT_CR_MGMT_DRIVER) {
      // Read the current NGN dimm congig data from HOB and Save it as PrevBootNGNDimmCfg Variable in the NVRAM
      PrevBootNGNDimmCfg(ADD_VARIABLE);

      DEBUG((EFI_D_ERROR, "CRP: UpdatePCD\n"));
      UpdatePCD();
    } else {
      // Remove the PrevBootNGNCfg Variable from NVRAM, so that it is easier to swtich between boot flows
      PrevBootNGNDimmCfg(RMV_VARIABLE);
    }
  }

  if (InSmm) {
    Status = mSmst->SmmInstallProtocolInterface (
             &Handle,
             &gEfiCrystalRidgeSmmGuid,
             EFI_NATIVE_INTERFACE,
             &mCrystalRidgeProtocol
             );

    SendSecurityNonceToAllDimms(FALSE);
  } else {
    Status = gBS->InstallProtocolInterface (
             &Handle,
             &gEfiCrystalRidgeGuid,
             EFI_NATIVE_INTERFACE,
             &mCrystalRidgeProtocol
             );

    Status = InstallAcpiPcatProtocol (ImageHandle, SystemTable);
  }

  ASSERT_EFI_ERROR (Status);
  if (InSmm) {
    EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
    EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch  = 0;
    EFI_HANDLE                        SwHandle;
    Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    ASSERT_EFI_ERROR (Status);
    SwContext.SwSmiInputValue = 0xFC;
    Status = SwDispatch->Register (SwDispatch, ServeNgnCommandsFromOS, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);
    SwContext.SwSmiInputValue = 0xFB;
    Status = SwDispatch->Register (SwDispatch, RestoreNonces, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
