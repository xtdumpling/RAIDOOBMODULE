/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"
#ifndef MINIBIOS_BUILD
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#endif

#ifdef PC_HOOK

#include <Guid/PlatformInfo.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
EFI_GUID mPlatformSpdRawDataGuid = PLATFORM_SPD_RAW_DATA_GUID;

VOID
PublishDimmRawSpdData (
  PSYSHOST host,
  UINT8    Socket,
  UINT8    Channel,
  UINT8    Dimm
  )
{
  UINT16                      Index;
  UINT8                       *DataPtr;
  PLATFORM_SPD_RAW_DATA_HOB   *SpdInfo;

  SpdInfo = BuildGuidHob (&mPlatformSpdRawDataGuid, sizeof (*SpdInfo) + MAX_SPD_BYTE_DDR4);
  DEBUG ((EFI_D_INFO, "[MemorySpd]Build SPD data HOB for Socket %x, Channel %x, Dimm %x\n", Socket, Channel, Dimm));
  SpdInfo->Size = MAX_SPD_BYTE_DDR4;
  SpdInfo->Socket = Socket;
  SpdInfo->Channel = Channel;
  SpdInfo->Dimm = Dimm;

  DataPtr = (UINT8 *) (SpdInfo + 1);
  for (Index = 0; Index < MAX_SPD_BYTE_DDR4; Index++) {
    ReadSpd (host, Socket, Channel, Dimm, Index, DataPtr++);
  }
}

VOID
PublishRawSpdData (
  PSYSHOST host
  )
{
  UINT8     Socket;
  UINT8     Channel;
  UINT8     Dimm;
  struct channelNvram (*ChannelNvList)[MAX_CH];
  struct dimmNvram (*DimmNvList)[MAX_DIMM];

  DEBUG ((EFI_D_INFO, "[MemorySpd]Read SPD Data\n"));
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if ((host->nvram.mem.socket[Socket].enabled == 0) || (host->nvram.mem.socket[Socket].maxDimmPop == 0)) {
      continue;
    }
    DEBUG ((EFI_D_INFO, "[MemorySpd]Read SPD on Socket %x\n", Socket));
    ChannelNvList = &host->nvram.mem.socket[Socket].channelList;
    for (Channel = 0; Channel < MAX_CH; Channel++) {
      if ((*ChannelNvList)[Channel].enabled == 0) {
        continue;
      }
      DEBUG ((EFI_D_INFO, "[MemorySpd]Read SPD on Socket %x, Channel %x\n", Socket, Channel));

      DimmNvList = &host->nvram.mem.socket[Socket].channelList[Channel].dimmList;
      for (Dimm = 0; Dimm < (*ChannelNvList)[Channel].maxDimm; Dimm++) {
        if ((*DimmNvList)[Dimm].dimmPresent == 0) {
          continue;
        }
        DEBUG ((EFI_D_INFO, "[MemorySpd]Read SPD on Socket %x, Channel %x, Dimm %x\n", Socket, Channel, Dimm));
        PublishDimmRawSpdData (host, Socket, Channel, Dimm);
      }
    }
  }
}

#endif


//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif //_MSC_VER
// APTIOV_SERVER_OVERRIDE_RC_START
#include <AmiMrcHooksWrapperLib\AmiMrcHooksWrapperLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END

/**

  OEM Hook to Log warning to the warning log

  @param host              - pointer to sysHost
  @param warningCode       - Major warning code
  @param minorWarningCode  - Minor warning code
  @param socket              - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

**/
void
PlatformOutputWarning (
  PSYSHOST host,
  UINT8    warningCode,
  UINT8    minorWarningCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
  //
  // OEM Specific Log the warning
  //
  // APTIOV_SERVER_OVERRIDE_RC_START
  AmiPlatformOutputWarningWrapper(host, warningCode, minorWarningCode, socket, ch, dimm, rank );
  // APTIOV_SERVER_OVERRIDE_RC_END
}

/**

  Oem hook to Log Error to the warning log

  @param host              - pointer to sysHost
  @param ErrorCode         - Major warning code
  @param minorErrorCode    - Minor warning code
  @param socket            - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

**/
void
PlatformOutputError (
  PSYSHOST host,
  UINT8    ErrorCode,
  UINT8    minorErrorCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
  //
  // OEM Specific Output the error
  //
  // APTIOV_SERVER_OVERRIDE_RC_START
  AmiPlatformOutputErrorWrapper(host, ErrorCode, minorErrorCode,socket,ch,dimm,rank);
  // APTIOV_SERVER_OVERRIDE_RC_END
}

/**

  Used to add warnings to the promote warning exception list

  @param host  - pointer to sysHost

  @retval N/A

**/
UINT32
PlatformMemInitWarning (
  PSYSHOST host
  )
{
  //
  //PC_ADD0x17_TO_WARNINGLIST_START
  //
#ifdef PC_HOOK
  SetPromoteWarningException (host, WARN_DIMM_POP_RUL, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_DIMM_COMPAT, 0);
#endif
  //
  //PC_ADD0x17_TO_WARNINGLIST_END
  //
  SetPromoteWarningException (host, WARN_USER_DIMM_DISABLE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_MEM_LIMIT, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_CLTT_DISABLE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_CLTT_DIMM_UNKNOWN, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_CLOSED_PAGE_OVERRIDE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_DIMM_VREF_NOT_PRESENT, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_LOCKSTEP_DISABLE, WARN_LOCKSTEP_DISABLE_MINOR_RAS_MODE);
  SetPromoteWarningException (host, WARN_MIRROR_DISABLE, WARN_MIRROR_DISABLE_MINOR_RAS_DISABLED);
  SetPromoteWarningException (host, WARN_DIMM_COMPAT, WARN_CHANNEL_MIX_ECC_NONECC);
  SetPromoteWarningException (host, WARN_PPR_FAILED, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_SPARE_DISABLE, 0);
  SetPromoteWarningException (host, WARN_MIRROR_DISABLE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_PMIRROR_DISABLE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_ADDDC_DISABLE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_SDDC_DISABLE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_REGISTER_OVERFLOW, 0);
  SetPromoteWarningException (host, WARN_DIMM_COMPAT, WARN_DIMM_SKU_MISMATCH);
  SetPromoteWarningException (host, WARN_INTERLEAVE_FAILURE, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_NVMCTRL_MEDIA_STATUS, WARN_MINOR_WILDCARD);
  SetPromoteWarningException (host, WARN_DIMM_COMPAT, WARN_DIMM_SPEED_NOT_SUP);

  return SUCCESS;
}

/**

  Used for memory related warnings

  @param host      - pointer to sysHost
  @param majorCode - Major warning code
  @param minorCode - Minor warning code
  @param logData   - Data log

  @retval N/A

**/
void
PlatformMemLogWarning (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT32   logData
  )
{
#ifdef SERIAL_DBG_MSG
  //
  // Output memory warning info
  //
  if (checkMsgLevel(host, SDBG_MINMAX)) {
    if (((logData >> 24) & 0xFF) != 0xFF) {
      rcPrintf ((host, "Socket = %d", (logData >> 24) & 0xFF));
    }

    if (((logData >> 16) & 0xFF) != 0xFF) {
      rcPrintf ((host, " Channel = %d", (logData >> 16) & 0xFF));
    }

    if (((logData >> 8) & 0xFF) != 0xFF) {
      rcPrintf ((host, " DIMM = %d", (logData >> 8) & 0xFF));
    }

    if ((logData & 0xFF) != 0xFF) {
      rcPrintf ((host, " Rank = %d", logData & 0xFF));
    }

    rcPrintf ((host, "\n\n"));
  }
#endif

  OutputWarning (host, majorCode, minorCode, ((logData >> 24) & 0xFF), ((logData >> 16) & 0xFF), ((logData >> 8) & 0xFF), (logData & 0xFF));
}

/**

  Platform hook to initialize the GPIO controller

  @param host  - pointer to sysHost

  @retval N/A

**/
void
PlatformMemInitGpio (
  PSYSHOST host
  )
{
}

/**

  Platform hook to select the SMBus segment

  @param host  - pointer to sysHost
  @param seg   - Desired SMBus segment

  @retval N/A

**/
void
PlatformMemSelectSmbSeg (
  PSYSHOST host,
  UINT8    seg
  )
{
}

/**

  PlatformMemReadDimmVref: OEM hook to read the current DIMM Vref setting from a given socket.
  If multiple Vref circuits are present per socket, this function assumes that
  they are set to the same value.

  @param host - Pointer to sysHost struct
  @param socket - processor number
  @param vref - pointer to UINT8 where the vref value is stored

  @retval SUCCESS if the Vref circuit is present and can be read
  @retval FAILURE if the Vref circuit is not present or cannot be read

**/
UINT32
PlatformMemReadDimmVref (
  PSYSHOST host,
  UINT8    socket,
  UINT8    *vref
  )
{
  UINT32  status = FAILURE;
  UINT8   ch;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((host->setup.mem.socket[socket].ddrCh[ch].vrefDcp.compId != NO_DEV) &&
        (host->setup.mem.socket[socket].ddrCh[ch].vrefDcp.compId != NOT_INITIALIZED)){
      status = ReadSmb (host, socket, host->setup.mem.socket[socket].ddrCh[ch].vrefDcp , 0, vref );
      if (status == SUCCESS) break;
    }
  }

  return status;
}

/**

  PlatformWriteDimmVref: OEM hook to write the DIMM Vref setting to a given socket.
  If multiple Vref circuits are present per socket, this function assumes that
  they are set to the same value.

  @param host - Pointer to sysHost struct
  @param socket - processor number
  @param vref - vref value to write

  @retval SUCCESS if the Vref circuit is present and can be written
  @retval FAILURE if the Vref circuit is not present or cannot be written

**/
UINT32
PlatformMemWriteDimmVref (
  PSYSHOST host,
  UINT8    socket,
  UINT8    vref
  )
{
  UINT32  status;
  UINT8   ch;

  status  = FAILURE;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((host->setup.mem.socket[socket].ddrCh[ch].vrefDcp.compId != NO_DEV) &&
        (host->setup.mem.socket[socket].ddrCh[ch].vrefDcp.compId != NOT_INITIALIZED)){
      status = WriteSmb (host, socket, host->setup.mem.socket[socket].ddrCh[ch].vrefDcp , 0, &vref );
    }
  }

  return status;
}

/**

  PlatformInitializeData - OEM hook to initialize Input structure data if required.

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
PlatformInitializeData (
  // APTIOV_SERVER_OVERRIDE_RC_START
  PSYSHOST host,                  
  UINT8 socket                  
  // APTIOV_SERVER_OVERRIDE_RC_END
  )
{
    // APTIOV_SERVER_OVERRIDE_RC_START
    AmiPlatformInitializeDataWrapper(host,socket);
    // APTIOV_SERVER_OVERRIDE_RC_END
}

/**

  PlatformSetVdd - OEM hook to change the socket Vddq

  @param host  - Pointer to sysHost
  @param socket  - processor number

  @retval SUCCESS if the Vddq change was made
  @retval FAILURE if the Vddq change was not made

**/
UINT8
PlatformSetVdd (
  PSYSHOST host,
  UINT8    socket
  )
{
  UINT8 status = FAILURE;

  return status;
}

/**

  OEM hook to release any platform clamps affecting CKE
  and/or DDR Reset

  @param host      - pointer to sysHost structure

**/
void
PlatformReleaseADRClamps (
  PSYSHOST host,
  UINT8 socket
  )
{
} // PlatformReleaseADRClamps

/**

  OEM hook to allow a secondary ADR dection method other than
  PCH_PM_STS

  @param host      - pointer to sysHost structure

  @retval 0         - Hook not implemented or no ADR recovery
  @retval 1         - Hook implemented and platform should recover from ADR

**/
UINT32
PlatformDetectADR (
  PSYSHOST host
  )
{
  return 0;
}

/**

  PlatformCheckPORCompat - OEM hook to restrict supported configurations if required.

  @param host  - Pointer to sysHost

  @retval N/A

**/
void
PlatformCheckPORCompat (
  PSYSHOST host
  )
{
#ifdef PC_HOOK
  PublishRawSpdData (host);
#endif
}

/**

  OEM hook to set error LEDs.

  @param host      - pointer to sysHost structure
  @param socket    - socket node

  @retval SUCCESS    - if able to access and set LEDs.
  @retval FAILURE    - if unable to access LEDs.
**/
UINT32
PlatformSetErrorLEDs (
  PSYSHOST host,
  UINT8    socket
  )
{
  // TBD: Add actual functionality for platform.
  return SUCCESS;
}

/**

  OEM hook immediately before initialization of memory throttling early in the MRC

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemInitThrottlingEarly (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook immediately before DIMM detection

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemDetectDIMMConfig (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook immediately before DIMM detection

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemInitDdrClocks (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook immediately before DIMM detection

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemSetDDRFreq (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook immediately before DIMM detection

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemConfigureXMP (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook immediately before DIMM detection

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemCheckVdd (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook immediately before DIMM detection

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemEarlyConfig (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook immediately before DIMM detection

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemLateConfig (
  PSYSHOST host
  )
{
  return SUCCESS;
}

UINT32
OemUpdatePlatformConfig(
  struct sysHost            *host
/**
  this routine can be used to update CCUR
  @param host  - pointer to sysHost structure on stack
**/
  )
{
  UINT32 status = SUCCESS;
  return status;
}

UINT32
ValidateCurrentConfigOemHook(
  PSYSHOST host
/**
  RAS hook for validating Current config (CCUR)
  @param host  - pointer to sysHost structure on stack
**/
  )
{
    UINT32 status = SUCCESS;
    return status;
}

/**

  OEM hook immediately before initialization of memory throttling

  @param host  - Point to sysHost

  @retval UINT32

**/
UINT32
OemInitThrottling (
  PSYSHOST host
  )
{
  return SUCCESS;
}

/**

  OEM hook for initializing Tcrit to 105 temp offset & the config register
  which is inside of initialization of memory throttling

  @param host              - pointer to sysHost
  @param socket            - socket number
  @param smbDevice dev     - SMB Device
  @param byteOffset        - byte Offset
  @param data              - data

**/
void
PlatformHookMst (
  PSYSHOST host,
  UINT8    socket,
  struct   smbDevice dev,
  UINT8    byteOffset,
  UINT8    *data
  )
{
  OemPlatformHookMst(host, socket, dev, byteOffset, (UINT8 *)&data);

  return;
}

/**

  OEM hook for overriding the DDR4 ODT Value table

  @param ddr4OdtValueStructPtr   - Pointer to Intel DDR4 ODT Value array
  @param ddr4OdtValueStructSize  - Pointer to size of Intel DDR4 ODT Value array in number of bytes


  @retval ddr4OdtValueStructPtr   - Pointer to DDR4 ODT Value array to use
  @retval ddr4OdtValueStructSize  - Pointer to size of DDR4 ODT Value array in number of bytes to use

**/
void
OemLookupDdr4OdtTable (
  PSYSHOST host,
  struct ddr4OdtValueStruct **ddr4OdtValueStructPtr,
  UINT16 *ddr4OdtValueStructSize
  )
{
/*
  *ddr4OdtValueStructPtr = &ddr4OdtValueTableOem[0];
  *ddr4OdtValueStructSize = sizeof(ddr4OdtValueTableOem);
*/
}

void
OemLookupFreqTable (
  PSYSHOST host,
  struct DimmPOREntryStruct **freqTablePtr,
  UINT16 *freqTableLength
  )
/*++

Routine Description:

  OEM hook for overriding the memory POR frequency table

Arguments:

  freqTablePtr    - Pointer to Intel POR memory frequency table
  freqTableLength - Pointer to number of DimmPOREntryStruct entries in POR Frequency Table


Returns:

  freqTablePtr    - Pointer to POR memory frequency table to use
  freqTableLength - Pointer to number of DimmPOREntryStruct entries in POR Frequency Table to use

--*/
{

/*
  *freqTablePtr = &OemfreqTable[0];
  *freqTableLength = sizeof(freqTableOem) / sizeof(DimmPOREntryStruct);
*/
// APTIOV_SERVER_OVERRIDE_RC_START
    AmiOemLookupFreqTableWrapper(host, freqTablePtr, freqTableLength);
// APTIOV_SERVER_OVERRIDE_RC_END
}
/**

  DRAM Memory test

  @param host                - Pointer to sysHost
  @param socket              - Socket number
  @param chEnMap             - ddr channels to test

  @retval 0 - hook not implemented - execute default memory test
  @retval 1 - hook implemented and successful - skip default memory test

**/
UINT32
PlatformDramMemoryTest (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     chEnMap
  )
{
  return 0;
}

#ifdef MEM_NVDIMM_EN
UINT32
PlatformArmNVDIMMs (
  PSYSHOST host
  )
{
  return 0;
} // PlatformArmNVDIMMs
#endif  //MEM_NVDIMM_EN

/**

  VR SVID's for MC on SKX Platform

  @param host                - Pointer to sysHost
  @param socket              - Socket number
  @param mcId                - Memory Controller Id

  @retval 0 - VR SVID for Memory Controller ID 0
  @retval 2 - VR SVID for Memory Controller ID 1

**/

UINT8
PlatformVRsSVID (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     mcId
  )
{
  if(mcId == 1)
    return 0x12;
  else
    return 0x10;

}


/**

  OemAfterAddressMapConfigured - OEM hook after address map is configured.

  @param host  - Pointer to sysHost

  @retval N/A

**/
VOID
OemAfterAddressMapConfigured (
  PSYSHOST host
  )
{

}


/**

  OemSendCompressedPacket - OEM hook to handle MRC serial log compressed packet

  @param host         - Pointer to sysHost
  @param packetPtr    - Pointer to compressed packet
  @param packetBytes  - Number of bytes in packet

  @retval SUCCESS - packet successfully handled
          FAILURE - packet not handled

  Note:
    Thread packet format is:
     - 1 byte packet type/thread number
          - bit 7: 1 = compressed binary format. 0 = not compressed
          - bit 6-0: = thread id, if binary compressed format
     - 2 byte packet size (max 4KB)
     - packet payload (size - 4)
          - compressed packets use EFI/EDK2 compressed format
     - 1 byte checksum

**/
UINT32
OemSendCompressedPacket (
  PSYSHOST host,
  UINT8    *packetPtr,
  UINT32   packetBytes
  )
{
  return FAILURE;
}