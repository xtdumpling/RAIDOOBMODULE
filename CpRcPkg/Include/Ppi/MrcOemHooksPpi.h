//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

   Reference Code

   ESS - Enterprise Silicon Software

   INTEL CONFIDENTIAL

@copyright
  Copyright 2016 -  Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.

@file
  MrcOemHooksPpi.h

@brief
  This file defines a PPI for OEM Hooks called by the silicon MRC.
  With this PPI, we can implement OEM hooks in a separate PEIM from the
  main MRC, without changing functional implementation of MRC or OEM
  hooks implementations.

**/
#ifndef _MRC_OEM_HOOKS_PPI_H_
#define _MRC_OEM_HOOKS_PPI_H_

///
/// PPI revision information
/// This PPI will be extended in a backwards compatible manner over time
/// Added interfaces should be documented here with the revisions added
/// Revision 1:  Initial revision
///
#define MRC_OEM_HOOKS_PPI_REVISION  1

typedef struct _MRC_OEM_HOOKS_PPI MRC_OEM_HOOKS_PPI;
extern EFI_GUID gMrcOemHooksPpiGuid;

/**

  Initialize host structure with OEM specific setup data

  @param Host - pointer to SYSHOST structure

  @retval     - N/A

**/
typedef 
VOID 
(EFIAPI *OEM_INITIALIZE_PLATFORM_DATA) (
  PSYSHOST Host
  );

/**

  OemPreMemoryInit  - OEM hook pre memory init

  @param Host - pointer to SYSHOST structure 
  @retval     - N/A

**/

typedef 
UINT32
(EFIAPI *OEM_PRE_MEMORY_INIT) (
  PSYSHOST Host  
  );

/**

  RC wrapper code.
  Copy host structure to system memory buffer after RC when memory becomes available

  @param Host     - pointer to SYSHOST structure 
  @param RcStatus - return status  of memory init to be checked in Post memory init function
  @retval         - N/A

**/
typedef 
VOID 
(EFIAPI *OEM_POST_MEMORY_INIT) (
  PSYSHOST Host,
  UINT32   RcStatus  
  );

/**

  Proc wrapper code.

    @param Host - pointer to SYSHOST structure 

    @retval     - N/A

**/
typedef 
VOID
(EFIAPI *OEM_PRE_PROC_INIT) (
  PSYSHOST Host  
  );

/**

  Proc wrapper code.

    @param Host - pointer to SYSHOST structure 
    @param ProcStatus  - Return status from Proc RC code execution

    @retval     - N/A

**/
typedef 
VOID
(EFIAPI *OEM_POST_PROC_INIT) (
  PSYSHOST Host,    
  UINT32   ProcStatus  
  );

/**

  OEM hook for reacting to Board VS CPU conflict.
  Refcode will halt execution. OEMs might prefer to allow system to boot to
  video and display error code on screen.

  @param Host - pointer to SYSHOST structure
  @param ProcStatus  - Return status from Proc RC code execution

  @retval     - N/A

**/
typedef 
VOID
(EFIAPI *OEM_CHECK_FOR_BOARD_VS_CPU_CONFLICTS) (
  PSYSHOST Host    
  );

/**

  Executed by System BSP only.
  OEM hook before checking and triggering the proper type of reset.

  @param Host - pointer to SYSHOST structure  on stack

  @retval     - N/A

**/
typedef 
VOID 
(EFIAPI *OEM_CHECK_AND_HANDLE_RESET_REQUESTS) (
  PSYSHOST Host  
  );


/**

  MEBIOS hook before checking and triggering the proper type of reset.

  @param Host - pointer to SYSHOST structure

  @retval     - N/A

**/
typedef 
VOID 
(EFIAPI *ME_BIOS_CHECK_AND_HANDLE_RESET_REQUESTS) ( 
  PSYSHOST Host
  );

/**

  OEM hook to do any platform specifc init after Cpu init

  @param Host   - pointer to SYSHOST structure  on stack
  @param Socket - socket number

  @retval UINT8

**/
typedef 
VOID
(EFIAPI *OEM_POST_CPU_INIT) (
  PSYSHOST Host,  
  UINT8    Socket  
  );

/**

  Puts the host structure variables for POST

  @param Host - pointer to SYSHOST structure

  @retval None

**/
typedef 
VOID 
(EFIAPI *OEM_PUBLISH_DATA_FOR_POST) (
  PSYSHOST Host
  );

/**

  OEM hook to handle a Fatal Error

  @param Host        - pointer to SYSHOST structure  on stack
  @param MajorCode   - Major error code
  @param MinorCode   - Minor error code
  @param HaltOnError - Halt on Error input/output value

  @retval N/A

--*/
typedef 
VOID
(EFIAPI *OEM_PLATFORM_FATAL_ERROR) (
  PSYSHOST Host,  
   UINT8   MajorCode,  
   UINT8   MinorCode,  
   UINT8   *HaltOnError  
   );

/**

  Determine if VRs should be turned off on HEDT

  @param Host - pointer to SYSHOST structure

  @retval None

**/
typedef 
VOID 
(EFIAPI *OEM_TURN_OFF_VRS_FOR_HEDT) ( 
  PSYSHOST Host
  );


/**

  OEM hook meant to be used by customers where they can use it to write their own jumper detection code
  and have it return FALSE when no jumper is present, thereby, blocking calling of the BSSA loader; else have it
  return TRUE if the concerned jumper is physically present. This check ensures that someone will have to be
  physically present by the platform to enable the BSSA Loader.

  @param Host - pointer to SYSHOST structure

  @retval TRUE - This function returns TRUE by default as OEMs are meant to implement their own jumper detection
                 code(DetectPhysicalPresence).

**/
typedef 
BOOLEAN
(EFIAPI *OEM_DETECT_PHYSICAL_PRESENCE_SSA) ( 
  PSYSHOST Host
  );

/**
  OEM hook before topology discovery in KTIRC.

  @param Host - pointer to SYSHOST structure

  @retval None

**/
typedef 
VOID
(EFIAPI *OEM_HOOK_PRE_TOPOLOGY_DISCOVERY) (
  PSYSHOST Host  
  );

/**
  OEM hook at the end of topology discovery in KTIRC to be used for error reporting.

  @param Host - pointer to SYSHOST structure
**/
typedef 
VOID
(EFIAPI *OEM_HOOK_POST_TOPOLOGY_DISCOVERY) (
  PSYSHOST Host  
  );

/**

  PlatformInitializeData - OEM hook to initialize Input structure data if required.

  @param Host  - Pointer to sysHost

  @retval N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_INITIALIZE_DATA) (
// APTIOV_SERVER_OVERRIDE_RC_START
  PSYSHOST Host,
  UINT8    socket
// APTIOV_SERVER_OVERRIDE_RC_END
  );

/**

  PlatformCheckpoint - OEM hook to provide major checkpoint output
  to the user interface. By default a byte is written to port 80h.

  @param Host  - pointer to SYSHOST structure 

  @retval N/A

**/
typedef 
VOID 
(EFIAPI *PLATFORM_CHECK_POINT) (
  PSYSHOST Host
  );

/**

  PlatformLogWarning - OEM hook to provide common warning output to the
  user interface

  @param Host      - pointer to SYSHOST structure 
  @param MajorCode - major error/warning code
  @param MinorCode - minor error/warning code
  @param LogData   - data log

**/
typedef 
VOID
(EFIAPI *PLATFORM_LOG_WARNING) (
  PSYSHOST Host, 
  UINT8    MajorCode, 
  UINT8    MinorCode, 
  UINT32   LogData
  );

/**

  PlatformFatalError - OEM hook to provide fatal error output to the
  user interface and to override the halt on error policy.

  @param Host - pointer to SYSHOST structure 
  @param MajorCode - major error/warning code
  @param MinorCode - minor error/warning code
  @param HaltOnError - pointer to variable that selects policy
  @retval            - N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_FATAL_ERROR) (
  PSYSHOST Host, 
  UINT8    MajorCode, 
  UINT8    MinorCode, 
  UINT8    *HaltOnError
  );

/**

  OEM hook to read data from a device on the SMBbus.

  @param Host - pointer to SYSHOST structure 
  @param Socket - socket number
  @param Device - SMB device structure
  @param ByteOffset - offset of data to read
  @param Data - Pointer to data to be returned

  @retval data - Pointer to data to be returned
  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadSmb

**/
  
typedef 
UINT8
(EFIAPI *PLATFORM_READ_SMB) (
  PSYSHOST         Host, 
  UINT8            Socket, 
  struct smbDevice Device, 
  UINT8            ByteOffset, 
  UINT8            *Data
  );

/**

  OEM hook to write data to a device on the SMBbus.

  @param Host - pointer to SYSHOST structure 
  @param Device - SMB device structure
  @param ByteOffset - offset of data to write
  @param Data - Pointer to data to write

  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and write was successful

**/
typedef 
UINT8
(EFIAPI *PLATFORM_WRITE_SMB) (
  PSYSHOST         Host, 
  struct smbDevice Device, 
  UINT8            ByteOffset, 
  UINT8            *Data
  );

/**

  OEM hook for one-time initialization of GPIO parameters such
  as decode, direction, and polarity init.

  @param Host - pointer to SYSHOST structure 

  @retval None

**/
typedef 
VOID
(EFIAPI *PLATFORM_INIT_GPIO) (
  PSYSHOST Host
  );

/**

  OEM hook to read a byte from a platform GPIO device that is
  not recognized by ReadGpio().

  @param Host      - pointer to SYSHOST structure 
  @param Address   - GPIO device address
  @param Data      - Pointer to data

  @retval 0         - Hook not implemented (default)
  @retval 1         - Hook implemented and read was successful
  @retval 2         - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadGpio

**/
typedef 
UINT32
(EFIAPI *PLATFORM_READ_GPIO) (
  PSYSHOST Host, 
  UINT16   Address, 
  UINT32   *Data
  );

/**

  OEM hook to write a byte to a platform GPIO device that is
  not recognized by WriteGpio().

  @param Host      - pointer to SYSHOST structure 
  @param Address   - GPIO device address
  @param Data      - Data to write

  @retval 0         - Hook not implemented (default)
  @retval 1         - Hook implemented and write was successful
  @retval 2         - Hook implemented and write failed / slave device not present
  @retval Return 1 or 2 if this routine should replace WriteGpio

**/
typedef 
UINT32
(EFIAPI *PLATFORM_WRITE_GPIO) (
  PSYSHOST Host, 
  UINT16   Address, 
  UINT32   Data
  );

/**

  Used to add warnings to the promote warning exception list

  @param Host  - pointer to SYSHOST structure

  @retval      - N/A

**/
typedef 
UINT32
(EFIAPI *PLATFORM_MEM_INIT_WARNING) (
  PSYSHOST Host
  );

/**

  Used for memory related warnings

  @param Host      - pointer to SYSHOST structure
  @param MajorCode - Major warning code
  @param MinorCode - Minor warning code
  @param LogData   - Data log

  @retval          - N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_MEM_LOG_WARNING) (
  PSYSHOST Host, 
  UINT8    MajorCode, 
  UINT8    MinorCode, 
  UINT32   LogData
  );

/**

  Platform hook to initialize the GPIO controller

  @param Host - pointer to SYSHOST structure

  @retval     - N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_MEM_INIT_GPIO) (
  PSYSHOST Host
  );

/**

  Platform hook to select the SMBus segment

  @param Host - pointer to SYSHOST structure
  @param Segment  - Desired SMBus segment

  @retval     - N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_MEM_SELECT_SMB_SEG) (
  PSYSHOST Host, 
  UINT8    Segment
  );

/**

  OEM hook to read the current DIMM Vref setting from a given socket.
  If multiple Vref circuits are present per socket, this function assumes that
  they are set to the same value.

  @param Host - pointer to SYSHOST structure 
  @param Socket - processor number
  @param Vref - pointer to UINT8 where the vref value is stored

  @retval SUCCESS if the Vref circuit is present and can be read
  @retval FAILURE if the Vref circuit is not present or cannot be read

**/
typedef 
UINT32 
(EFIAPI *PLATFORM_MEM_READ_DIMM_VREF) (
  PSYSHOST Host, 
  UINT8    Socket, 
  UINT8    *Vref
  );

/**

  OEM hook to write the DIMM Vref setting to a given socket.
  If multiple Vref circuits are present per socket, this function assumes that
  they are set to the same value.

  @param Host - pointer to SYSHOST structure struct
  @param Socket - processor number
  @param Vref - vref value to write

  @retval SUCCESS if the Vref circuit is present and can be written
  @retval FAILURE if the Vref circuit is not present or cannot be written

**/
typedef 
UINT32
(EFIAPI *PLATFORM_MEM_WRITE_DIMM_VREF) (
  PSYSHOST Host, 
  UINT8    Socket, 
  UINT8    Vref
  );

/**

  OEM hook to change the socket Vddq

  @param Host  - pointer to SYSHOST structure
  @param Socket  - processor number

  @retval SUCCESS if the Vddq change was made
  @retval FAILURE if the Vddq change was not made

**/
typedef 
UINT8
(EFIAPI *PLATFORM_SET_VDD) (
  PSYSHOST Host, 
  UINT8    Socket
  );

/**

  OEM hook to release any platform clamps affecting CKE
  and/or DDR Reset

  @param Host      - pointer to SYSHOST structure 
  @param Socket    - processor number

  @retval None

**/
typedef 
VOID
(EFIAPI *PLATFORM_RELEASE_ADR_CLAMPS) (
  PSYSHOST Host, 
  UINT8    Socket
  );

/**

  OEM hook to allow a secondary ADR dection method other than
  PCH_PM_STS

  @param Host      - pointer to SYSHOST structure 

  @retval 0         - Hook not implemented or no ADR recovery
  @retval 1         - Hook implemented and platform should recover from ADR

**/
typedef 
UINT32
(EFIAPI *PLATFORM_DETECT_ADR) (
  PSYSHOST Host
  );

/**

  OEM hook to set error LEDs.

  @param Host   - pointer to SYSHOST structure
  @param Socket - Socket number

  @retval SUCCESS    - if able to access and set LEDs.
  @retval FAILURE    - if unable to access LEDs.

**/
typedef 
UINT32
(EFIAPI *PLATFORM_SET_ERROR_LEDS) (
  PSYSHOST Host, 
  UINT8    Socket
  );

/**

  This routine can be used to update CCUR

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_UPDATE_PLATFORM_CONFIG) (
  PSYSHOST Host
  );

/**

  OEM hook immediately before initialization of memory throttling early in the RC

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_INIT_THROTTLING_EARLY) (
  PSYSHOST Host
  );

/**

  OEM hook immediately before DIMM detection

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_DETECT_DIMM_CONFIG) (
  PSYSHOST Host
  );

/**

  OEM hook to initialize DDR clocks

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_INIT_DDR_CLOCKS) (
  PSYSHOST Host
  );

/**

  OEM hook to set the DDR frequency

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_SET_DDR_FREQ) (
  PSYSHOST Host
  );

/**

  OEM hook to configure XMP settings

  @param Host - pointer to SYSHOST structure

  @retval  UINT32

**/
typedef 
UINT32
(EFIAPI *OEM_CONFIGURE_XMP) (
  PSYSHOST Host
  );

/**

  OEM hook to check VDD

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_CHECK_VDD) (
  PSYSHOST Host
  );

/**

  OEM hook to perform early configuration

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_EARLY_CONFIG) (
  PSYSHOST Host
  );

/**

  OEM hook to perform late configuration

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_LATE_CONFIG) (
  PSYSHOST Host
  );

/**

  OEM hook to initialize memory throttling

  @param Host - pointer to SYSHOST structure

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *OEM_INIT_THROTTLING) (
  PSYSHOST Host
  );

/**

  PlatformCheckPORCompat - OEM hook to restrict supported configurations if required.

  @param Host - pointer to SYSHOST structure

  @retval     - N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_CHECK_POR_COMPAT) (
  PSYSHOST Host
  );

/**

  OEM hook for initializing Tcrit to 105 temp offset & the config register
  which is inside of initialization of memory throttling

  @param Host - pointer to SYSHOST structure
  @param Socket            - socket number
  @param Device            - SMB Device
  @param ByteOffset        - byte Offset
  @param Data              - data
  @retval                  - N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_HOOK_MST) (
  PSYSHOST         Host, 
  UINT8            Socket, 
  struct smbDevice Device, 
  UINT8            ByteOffset, 
  UINT8            *Data
  );

/**

  OEM hook to perform DRAM memory testing

  @param Host - pointer to SYSHOST structure
  @param Socket  - Socket number
  @param ChannelEnableMap - Map of enabled channels

  @retval SUCCESS

**/
typedef 
UINT32
(EFIAPI *PLATFORM_DRAM_MEMORY_TEST) (
  PSYSHOST Host, 
  UINT8    Socket, 
  UINT8    ChannelEnableMap
  );

/**

  OEM hook for overriding the DDR4 ODT Value table

  @param Host                    - pointer to SYSHOST structure
  @param Ddr4OdtValueStructPtr   - Pointer to Intel DDR4 ODT Value array
  @param Ddr4OdtValueStructSize  - Pointer to size of Intel DDR4 ODT Value array in number of bytes

  @retval                        - N/A


**/
typedef 
VOID
(EFIAPI *OEM_LOOKUP_DDR4_ODT_TABLE) (
  PSYSHOST                    Host, 
  struct ddr4OdtValueStruct   **Ddr4OdtValueStructPtr, 
  UINT16                      *Ddr4OdtValueStructSize
  );

/**

Routine Description:

  OEM hook for overriding the memory POR frequency table

  @param Host            - pointer to SYSHOST structure
  @param FreqTablePtr    - Pointer to Intel POR memory frequency table
  @param FreqTableLength - Pointer to number of DimmPOREntryStruct entries in POR Frequency Table

  @retval                - N/A

**/
typedef 
VOID
(EFIAPI *OEM_LOOKUP_FREQ_TABLE) (
  PSYSHOST                    Host, 
  struct DimmPOREntryStruct   **FreqTablePtr, 
  UINT16                      *FreqTableLength
  );

/**

  memory link reset
  
  @param Host - pointer to SYSHOST structure

  @retval     - N/A

**/
typedef 
VOID
(EFIAPI *PLATFORM_EX_MEMORY_LINK_RESET) (
  PSYSHOST Host
  );

/**

  VR SVID's for MC on SKX Platform

  @param Host                - pointer to SYSHOST structure
  @param Socket              - Socket number
  @param McId                - Memory Controller Id

  @retval 0 - VR SVID for Memory Controller ID 0
  @retval 2 - VR SVID for Memory Controller ID 1

**/
typedef 
UINT8
(EFIAPI *PLATFORM_VRS_SVID) (
  PSYSHOST  Host, 
  UINT8     Socket, 
  UINT8     McId
  );

/**

  Programs input parameters for the RC

  @param Host - pointer to SYSHOST structure

  @retval     - N/A

**/
typedef 
VOID
(EFIAPI *GET_MEM_SETUP_OPTIONS) (
  PSYSHOST Host
  );

/**

  OemSendCompressedPacket - OEM hook to handle RC serial log compressed packet

  @param Host         - Pointer to sysHost
  @param PacketPtr    - Pointer to compressed packet
  @param PacketBytes  - Number of bytes in packet

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

typedef 
UINT32
(EFIAPI *OEM_SEND_COMPRESSED_PACKET) (  
  PSYSHOST  Host,  
  UINT8     *PacketPtr,  
  UINT32    PacketBytes
  );

/**

  Locates the platform EPARAMS based on socket/link

  @param Host               - pointer to SYSHOST structure
  @param SocketID           - SocId we are looking up
  @param Link               - Link# on socket
  @param Freq               - Link frequency
  @param LinkSpeedParameter - return Ptr

  @retval KTI_SUCCESS if entry found
  @retval KTI_FAILURE if no entry found

**/
typedef 
INT32
(EFIAPI *OEM_KTI_GET_EPARAMS) (
  PSYSHOST Host, 
  UINT8    SocketID, 
  UINT8    Link, 
  UINT8    Freq, 
  VOID     *LinkSpeedParameter
  );

/**

  Get MMIO high base and size

  @param Host - pointer to SYSHOST structure
  @param MmiohSize - Region Size
  @param MmiohBase - Region Base

  @retval TRUE - get MMIO OK
  @retval FALSE - no MMIO is provided

**/
typedef 
BOOLEAN
(EFIAPI *OEM_KTI_GET_MMIOH) (
  PSYSHOST Host, 
  UINT16   *MmiohSize, 
  UINT16   *MmiohBase
  );

/**

  Get MMIO Low base and size

  @param Host - pointer to SYSHOST structure
  @param MmiolSize - Region Size
  @param MmiolBase - Region Base

  @retval TRUE - get MMIO OK
  @retval FALSE - no MMIO is provided

**/
typedef 
BOOLEAN 
(EFIAPI *OEM_KTI_GET_MMIOL) (
  PSYSHOST Host, 
  UINT8    *MmiolSize, 
  UINT32   *MmiolBase
  );

/**

  Check if CPU part was swapped

  @param Host - pointer to SYSHOST structure

  @retval TRUE - CPU has been swapped
  @retval FALSE - CPU has not been swapped

**/
typedef 
BOOLEAN
(EFIAPI *OEM_CHECK_CPU_PARTS_CHANGE_SWAP) (
  PSYSHOST Host
  );

/**

    Get adapted EQ settings

    @param Host            - pointer to SYSHOST structure
    @param Speed           - Link Speed
    @param AdaptationTable - Adaptation Table

    @retval TRUE
    @retval FALSE

**/
typedef 
BOOLEAN
(EFIAPI *OEM_GET_ADAPTED_EQ_SETTINGS) ( 
  PSYSHOST Host, 
  UINT8    Speed, 
  VOID     *AdaptationTable
  );

/**

    Wait for PBSP to check in

    @param Host     - pointer to SYSHOST structure
    @param WaitTime - time in milliseconds

    @retval         - N/A

**/
typedef 
VOID
(EFIAPI *OEM_WAIT_TIME_FOR_PSBP) ( 
  PSYSHOST  Host, 
  UINT32    *WaitTime 
  );

/**

  Read KTI Nvram from variable.

    @param Host     - pointer to SYSHOST structure

    @retval         - TRUE: read KTI NVRAM OK; FALSE: KTI NVRAM VAR not found

**/
typedef 
BOOLEAN
(EFIAPI *OEM_READ_KTI_NVRAM) (
  PSYSHOST Host
  );


/**

  Update PrevBoot prevBootNGNDimmCfg.

    @param Host               - pointer to SYSHOST structure
    @param prevBootNGNDimmCfg - Configuration details

    @retval         - 0:SUCCESS

**/
typedef
UINT32 
(EFIAPI *UPDATE_PREV_BOOT_NGN_DIMM_CFG) (
  PSYSHOST                    Host, 
  struct prevBootNGNDimmCfg   *PrevBootNGNDimmCfg
  );

/**

  Oem reset 

    @param Host       - pointer to SYSHOST structure
    @param ResetType  - reset type

    @retval         - N/A

**/
typedef
VOID
(EFIAPI *OEM_ISSUE_RESET) (
  PSYSHOST Host,
  UINT8    ResetType
  );

/**

  Oem Init Serial Debug

    @param Host       - pointer to SYSHOST structure
    @param KtiStatus  - Return status from RC code execution

    @retval         - N/A

**/
typedef 
VOID
(EFIAPI *OEM_INIT_SERIAL_DEBUG) (
  PSYSHOST Host
  );

/**

  OEM Hook to Log warning to the warning log

  @param Host              - pointer to SYSHOST structure
  @param WarningCode       - Major warning code
  @param MinorWarningCode  - Minor warning code
  @param Socket            - socket number
  @param Channel           - channel number
  @param Dimm              - dimm number
  @param Rank              - rank number

  @retval                  - N/A

**/
typedef
VOID
(EFIAPI *PLATFORM_OUTPUT_WARNING) (
  PSYSHOST Host,
  UINT8    WarningCode,
  UINT8    MinorWarningCode,
  UINT8    Socket,
  UINT8    Channel,
  UINT8    Dimm,
  UINT8    Rank
  );

/**

  Oem hook to Log Error to the warning log

  @param Host              - pointer to SYSHOST structure
  @param ErrorCode         - Major warning code
  @param MinorErrorCode    - Minor warning code
  @param Socket            - socket number
  @param Channel           - channel number
  @param Dimm              - dimm number
  @param Rank              - rank number

  @retval                  - N/A
  
**/
typedef
VOID
(EFIAPI *PLATFORM_OUTPUT_ERROR) (
  PSYSHOST Host,
  UINT8    ErrorCode,
  UINT8    MinorErrorCode,
  UINT8    Socket,
  UINT8    Ch,
  UINT8    Dimm,
  UINT8    Rank
  );

/**

  Platform function for initializing the enhanced warning log.
  It is called from core prior any calls to PlatformEwlLogEntry.
  This function will initialize the enhanced warning log public
  and private data structures.

  @param host        - Pointer to the system host (root) structure

  @retval            - SUCCESS

**/
typedef
UINT32
(EFIAPI *PLATFORM_EWL_INIT) (
  PSYSHOST Host
  );

/**

  Platform hook that is called to report a new entry is available for reporting to
  the Enhanced Warning Log.

  This function will add it to the log or return an error if there is insufficient
  space remaining for the entry.

  @param  Host        - Pointer to the system host (root) structure
  @param  EwlEntry    - Pointer to new EWL entry buffer

  @retval             - SUCCESS if entry is added; FAILURE if entry is not added

**/
typedef
MRC_STATUS
(EFIAPI *PLATFORM_EWL_LOG_ENTRY) (
  PSYSHOST          Host,
  EWL_ENTRY_HEADER  *EwlEntry     
  );


/**

  Get the board type bitmask.
    Bits[3:0]   - Socket0
    Bits[7:4]   - Socket1
    Bits[11:8]  - Socket2
    Bits[15:12] - Socket3
    Bits[19:16] - Socket4
    Bits[23:20] - Socket5
    Bits[27:24] - Socket6
    Bits[31:28] - Socket7

  Within each Socket-specific field, bits mean:
    Bit0 = CPU_TYPE_STD support; always 1 on Socket0
    Bit1 = CPU_TYPE_F support
    Bit2 = CPU_TYPE_P support
    Bit3 = reserved

  @param Host - pointer to syshost

  @retval board type bitmask

**/
typedef
UINT32
(EFIAPI *OEM_GET_BOARD_TYPE_BITMASK) (
  PSYSHOST Host
  );

/**

   Callout to allow OEM to adjust the resource map.
   Used to allow adjustment of IO ranges, bus numbers, mmioh and mmiol resource assignments
   Calling code assumes the data structure is returned in a good format.
   SBSP resources must be assigned first, with rest of sockets assigned in ascending order of NID
   Resources must be assigned in ascending consecutive order with no gaps.
   Notes: This does not allow for changing the mmcfg size of mmioh granularity
          However you can allocate multiple consecutive blocks of mmioh to increase size in a particular stack
          and adjust the mmiohbase


   @param Host         - Pointer to the system host (root) structure
   @param CpuResources - Ptr to array of Cpu Resources
   @param SystemParams - structure w/ system parameters needed

**/
typedef
VOID
(EFIAPI *OEM_GET_RESOURCE_MAP_UPDATE) (
  PSYSHOST          Host,
  KTI_CPU_RESOURCE  CpuResources[MAX_SOCKET],
  KTI_OEMCALLOUT    SystemParams
  );

/**
  RAS hook for validating Current config (CCUR)

  @param Host  - pointer to SYSHOST structure on stack

**/
typedef
UINT32
(EFIAPI *VALIDATE_CURRENT_CONFIG_OEM_HOOK) (
  PSYSHOST Host
  ); 

/**

  Platform function to get platform info

  @param IioGlobalData  - Pointer to the IIO global Data structure

  @retval               - N/A

**/ 
typedef
VOID
(EFIAPI *OEM_GET_IIO_PLATFORM_INFO) (
  IIO_GLOBALS    *IioGlobalData
  );

/**

  Platform function to get IIO uplink port info

  @param IioGlobalData  - Pointer to the IIO global Data structure
  @param IioIndex       - Index of IIO
  @param PortIndex      - pointer to port index
  @param Bus            - pointer to bus number
  @param Device         - pointer to device index
  @param Function       - pointer to function index

  @retval               - N/A

**/ 
typedef
VOID
(EFIAPI *OEM_IIO_UPLINK_PORT_DETAILS) (
  IIO_GLOBALS   *IioGlobalData,
  UINT8         IioIndex,
  UINT8         *PortIndex,
  UINT8         *Bus,
  UINT8         *Device,
  UINT8         *Function
  );
  
/**

  Platform function to do IIO WA at early stage

  @param IioGlobalData  - Pointer to the IIO global Data structure
  @param IioIndex       - Index of IIO
  @param Phase          - phase to hook

  @retval               - N/A

**/ 
typedef
VOID
(EFIAPI *OEM_IIO_EARLY_WORK_AROUND) (
  IIO_GLOBALS                 *IioGlobalData,
  UINT8                       IioIndex,
  IIO_INIT_ENUMERATION        Phase
  );

/**

  Platform function to override Iio Rx recipe settings

  @param IioGlobalData  - Pointer to the IIO global Data structure
  @param IioIndex       - Index of IIO

  @retval               - N/A

**/    
typedef 
VOID
(EFIAPI *OEM_OVERRIDE_IIO_RX_RECIPE_SETTINGS) (
  IIO_GLOBALS                *IioGlobalData,
  UINT8                      IioIndex
  );


typedef
VOID
(EFIAPI *OEM_DEFAULT_IIO_PORT_BIFURATION_INFO) (
    IIO_GLOBALS  *IioGlobalData
  );

/**

  Platform function to initialize IIO Port Bifurcation

  @param IioGlobalData  - Pointer to the IIO global Data structure

  @retval  N/A

**/
typedef
VOID
(EFIAPI *OEM_INIT_IIO_PORT_BIFURCATION_INFO) (
  IIO_GLOBALS  *IioGlobalData
  ); 

/**

  Platform function to pre-initialize Dmi device

  @param PciAddress     - Pci address
  @param IioGlobalData  - Pointer to the IIO global Data structure
  @param IioIndex       - Index of IIO to pre-initialize
  @param PortIndex      - Index of IIO port

  @retval  N/A

**/
typedef
VOID
(EFIAPI *OEM_DMI_PRE_INIT) (
  IN IIO_GLOBALS    *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  );

/**

  Platform function to initialize Dmi device

  @param PciAddress     - Pci address
  @param IioGlobalData  - Pointer to the IIO global Data structure
  @param IioIndex       - Index of IIO to initialize
  @param PortIndex      - Index of IIO port

  @retval  N/A

**/
typedef
VOID
(EFIAPI *OEM_DMI_DEVICE_INIT) (
  IN IIO_GLOBALS    *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  );

/**

  Platform function to initialize Dmi Resource

  @param PciAddress     - Pci address
  @param IioGlobalData  - Pointer to the IIO global Data structure
  @param IioIndex       - Index of IIO to initialize
  @param PortIndex      - Index of IIO port

  @retval  N/A

**/

typedef
VOID
(EFIAPI *OEM_DMI_RESOURCE_ASSIGNED_INIT) (
  IN IIO_GLOBALS    *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  );

/**

  Initialize the Global Data Structure with the Default Values 

  @param IioGlobalData - Pointer to IioGlobalData

  @retval NONE

**/
typedef
VOID
(EFIAPI *OEM_SET_IIO_DEFAULT_VALUES) (
  IIO_GLOBALS  *IioGlobalData
  );

/**

  Initialize the Global Data Structure with the Setup Values
  read from NVRAM

  @param IioGlobalData - Pointer to IioGlobalData

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
typedef
VOID
(EFIAPI *OEM_SET_PLATFORM_DATA_VALUES) (
  IIO_GLOBALS  *IioGlobalData
  ); 

/**

  Publish the HOB for HFI UEFI driver usage

  @param IioGlobalData - Pointer to IioGlobalData

  @retval VOID        All other error conditions encountered result in an ASSERT

**/
typedef
VOID
(EFIAPI *OEM_PUBLISH_OPA_SOCKET_MAP_HOB) (
  IIO_GLOBALS  *IioGlobalData
  );

/**

  OEM hook after address map is configured.

  @param Host  - Pointer to sysHost

  @retval N/A

**/
typedef
VOID
(EFIAPI *OEM_AFTER_ADDRESS_MAP_CONFIGURED) (
  PSYSHOST Host
  );
    

struct _MRC_OEM_HOOKS_PPI {
  /**
    This member specifies the revision of the MRC_OEM_HOOKS_PPI. This field is used to
    indicate backwards compatible changes to the INTERFACE. Platform code that produces
    this INTERFACE must fill with the correct revision value for MRC code
    to correctly interpret the content of the INTERFACE fields.
  **/
  UINT32                                  Revision;

  OEM_INITIALIZE_PLATFORM_DATA            OemInitializePlatformData;                   
  OEM_PRE_MEMORY_INIT                     OemPreMemoryInit;                            
  OEM_POST_MEMORY_INIT                    OemPostMemoryInit;                           
  OEM_PRE_PROC_INIT                       OemPreProcInit;                              
  OEM_POST_PROC_INIT                      OemPostProcInit;                             
  OEM_CHECK_FOR_BOARD_VS_CPU_CONFLICTS    OemCheckForBoardVsCpuConflicts;              
  OEM_CHECK_AND_HANDLE_RESET_REQUESTS     OemCheckAndHandleResetRequests;              
  ME_BIOS_CHECK_AND_HANDLE_RESET_REQUESTS MEBIOSCheckAndHandleResetRequests;                                                               
  OEM_POST_CPU_INIT                       OemPostCpuInit;                              
  OEM_PUBLISH_DATA_FOR_POST               OemPublishDataForPost;                       
  OEM_PLATFORM_FATAL_ERROR                OemPlatformFatalError; 
  OEM_TURN_OFF_VRS_FOR_HEDT               OemTurnOffVrsForHedt;                        
  OEM_DETECT_PHYSICAL_PRESENCE_SSA        OemDetectPhysicalPresenceSSA;
  OEM_HOOK_PRE_TOPOLOGY_DISCOVERY         OemHookPreTopologyDiscovery;                 
  OEM_HOOK_POST_TOPOLOGY_DISCOVERY        OemHookPostTopologyDiscovery;  
  PLATFORM_INITIALIZE_DATA                PlatformInitializeData;
  PLATFORM_MEM_INIT_GPIO                  PlatformMemInitGpio; 
  PLATFORM_INIT_GPIO                      PlatformInitGpio;
  PLATFORM_READ_GPIO                      PlatformReadGpio;
  PLATFORM_WRITE_GPIO                     PlatformWriteGpio;
  PLATFORM_MEM_SELECT_SMB_SEG             PlatformMemSelectSmbSeg;                     
  PLATFORM_MEM_READ_DIMM_VREF             PlatformMemReadDimmVref;                     
  PLATFORM_MEM_WRITE_DIMM_VREF            PlatformMemWriteDimmVref;                    
  PLATFORM_SET_VDD                        PlatformSetVdd;                              
  PLATFORM_RELEASE_ADR_CLAMPS             PlatformReleaseADRClamps;                    
  PLATFORM_DETECT_ADR                     PlatformDetectADR;                           
  PLATFORM_SET_ERROR_LEDS                 PlatformSetErrorLEDs;                        
  OEM_UPDATE_PLATFORM_CONFIG              OemUpdatePlatformConfig;                     
  OEM_INIT_THROTTLING_EARLY               OemInitThrottlingEarly;                      
  OEM_DETECT_DIMM_CONFIG                  OemDetectDIMMConfig;                         
  OEM_INIT_DDR_CLOCKS                     OemInitDdrClocks;                            
  OEM_SET_DDR_FREQ                        OemSetDDRFreq;                               
  OEM_CONFIGURE_XMP                       OemConfigureXMP;                             
  OEM_CHECK_VDD                           OemCheckVdd;                                 
  OEM_EARLY_CONFIG                        OemEarlyConfig;                              
  OEM_LATE_CONFIG                         OemLateConfig;                               
  OEM_INIT_THROTTLING                     OemInitThrottling;                           
  PLATFORM_CHECK_POR_COMPAT               PlatformCheckPORCompat;                      
  PLATFORM_HOOK_MST                       PlatformHookMst;                             
  PLATFORM_DRAM_MEMORY_TEST               PlatformDramMemoryTest;                      
  OEM_LOOKUP_DDR4_ODT_TABLE               OemLookupDdr4OdtTable;                       
  OEM_LOOKUP_FREQ_TABLE                   OemLookupFreqTable;                          
  PLATFORM_EX_MEMORY_LINK_RESET           PlatformExMemoryLinkReset;                   
  PLATFORM_VRS_SVID                       PlatformVRsSVID;                             
  GET_MEM_SETUP_OPTIONS                   GetMemSetupOptions;  
  OEM_SEND_COMPRESSED_PACKET              OemSendCompressedPacket;
  OEM_KTI_GET_EPARAMS                     OemKtiGetEParams;                            
  OEM_KTI_GET_MMIOH                       OemKtiGetMmioh;                              
  OEM_KTI_GET_MMIOL                       OemKtiGetMmiol;                              
  OEM_CHECK_CPU_PARTS_CHANGE_SWAP         OemCheckCpuPartsChangeSwap;                  
  OEM_GET_ADAPTED_EQ_SETTINGS             OemGetAdaptedEqSettings;                     
  OEM_WAIT_TIME_FOR_PSBP                  OemWaitTimeForPSBP;        
  OEM_READ_KTI_NVRAM                      OemReadKtiNvram;
  UPDATE_PREV_BOOT_NGN_DIMM_CFG           UpdatePrevBootNGNDimmCfg;
  OEM_ISSUE_RESET                         OemIssueReset;
  OEM_INIT_SERIAL_DEBUG                   OemInitSerialDebug;
  PLATFORM_CHECK_POINT                    PlatformCheckpoint;
  PLATFORM_OUTPUT_WARNING                 PlatformOutputWarning;
  PLATFORM_OUTPUT_ERROR                   PlatformOutputError;
  PLATFORM_LOG_WARNING                    PlatformLogWarning;
  PLATFORM_FATAL_ERROR                    PlatformFatalError; 
  PLATFORM_EWL_INIT                       PlatformEwlInit;
  PLATFORM_MEM_INIT_WARNING               PlatformMemInitWarning;
  PLATFORM_EWL_LOG_ENTRY                  PlatformEwlLogEntry;
  OEM_GET_BOARD_TYPE_BITMASK              OemGetBoardTypeBitmask;
  OEM_GET_RESOURCE_MAP_UPDATE             OemGetResourceMapUpdate;
  VALIDATE_CURRENT_CONFIG_OEM_HOOK        ValidateCurrentConfigOemHook;
  PLATFORM_READ_SMB                       PlatformReadSmb;
  PLATFORM_WRITE_SMB                      PlatformWriteSmb;
  OEM_GET_IIO_PLATFORM_INFO               OemGetIioPlatformInfo;
  OEM_IIO_UPLINK_PORT_DETAILS             OemIioUplinkPortDetails;
  OEM_IIO_EARLY_WORK_AROUND               OemIioEarlyWorkAround;
  OEM_OVERRIDE_IIO_RX_RECIPE_SETTINGS     OemOverrideIioRxRecipeSettings;
  OEM_DEFAULT_IIO_PORT_BIFURATION_INFO    OemDefaultIioPortBifurationInfo;
  OEM_INIT_IIO_PORT_BIFURCATION_INFO      OemInitIioPortBifurcationInfo;  
  OEM_DMI_PRE_INIT                        OemDmiPreInit;
  OEM_DMI_DEVICE_INIT                     OemDmiDeviceInit;
  OEM_DMI_RESOURCE_ASSIGNED_INIT          OemDmiResourceAssignedInit;
  OEM_SET_IIO_DEFAULT_VALUES              OemSetIioDefaultValues;
  OEM_SET_PLATFORM_DATA_VALUES            OemSetPlatformDataValues;
  OEM_PUBLISH_OPA_SOCKET_MAP_HOB          OemPublishOpaSocketMapHob;
  OEM_AFTER_ADDRESS_MAP_CONFIGURED        OemAfterAddressMapConfigured;
};

#endif  // _MRC_OEM_HOOKS_PPI_H_