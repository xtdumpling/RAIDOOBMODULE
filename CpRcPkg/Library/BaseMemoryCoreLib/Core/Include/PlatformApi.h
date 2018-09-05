//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
//
//*****************************************************************************

//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Memory Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

@copyright
  Copyright 2006 - 2015 Intel Corporation All Rights Reserved.

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
   PlatformApi.h

@brief
   This file contains declarations for the MRC Core to Platform API

 ************************************************************************/
#ifndef  _PLATFORM_API_H_
#define  _PLATFORM_API_H_

#include "DataTypes.h"
#include "SysHost.h"
#include "Token.h" // SMCPKG_SUPPORT++
/**

  OEM Hook to Log warning to the warning log

  @param host              - Pointer to sysHost, the system host (root) structure
  @param warningCode       - Major warning code
  @param minorWarningCode  - Minor warning code
  @param socket            - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

  @retval None

**/
void
PlatformOutputWarning(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    warningCode,         // Major warning code
    UINT8    minorWarningCode,    // Minor warning code
    UINT8    socket,              // socket number
    UINT8    ch,                  // channel number
    UINT8    dimm,                // dimm number
    UINT8    rank                 // rank number
    );

/**

  Oem hook to Log Error to the warning log

  @param host           - Pointer to sysHost, the system host (root) structure
  @param ErrorCode      - Major error code
  @param minorErrorCode - Minor error code
  @param socket         - socket number
  @param ch             - channel number
  @param dimm           - dimm number
  @param rank           - rank number

  @retval None

**/
void
PlatformOutputError(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    ErrorCode,           // Major error code
    UINT8    minorErrorCode,      // Minor error code
    UINT8    socket,              // socket number
    UINT8    ch,                  // channel number
    UINT8    dimm,                // dimm number
    UINT8    rank                 // rank number
    );

/**

  OEM hook to handle a Fatal Error

  @param host        - Pointer to the system host (root) structure
  @param majorCode   - Major error code
  @param minorCode   - Minor error code
  @param haltOnError - Halt on Error input/output value

  @retval None

**/

#if SMCPKG_SUPPORT		// SMCPKG_SUPPORT++
void
OemIpmiWarningReport(
    PSYSHOST host
    );
#endif				// SMCPKG_SUPPORT++

void
OemPlatformFatalError(
    PSYSHOST host,                // Pointer to the system host (root) structure
    UINT8    majorCode,           // Major error code
    UINT8    minorCode,           // Minor error code
    UINT8    *haltOnError         // Halt on Error input/output value
    );

/**

  OEM hook pre memory init

  @param host - Pointer to the system host (root) structure

  @retval SUCCESS

**/
UINT32
OemPreMemoryInit(
    PSYSHOST host                 // Pointer to the system host (root) structure
    );

/**

  Copy host structure to system memory buffer after MRC when memory becomes available

  @param host       - Pointer to sysHost, the system host (root) structure
  @param MrcStatus  - Return status from MRC code execution

  @retval None

**/
VOID
OemPostMemoryInit(
    struct sysHost   *host,       // Pointer to sysHost, the system host (root) structure
    UINT32           MrcStatus    // Return status from MRC code execution
    );

/**

  Wrapper code to perform platform specific pre processor init

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
OemPreProcInit(
    struct sysHost   *host        // Pointer to sysHost, the system host (root) structure
    );

/**

  Wrapper code to perform platform specific post processor init

  @param host        - Pointer to sysHost, the system host (root) structure
  @param ProcStatus  - Return status

  @retval None

**/
VOID
OemPostProcInit(
    struct sysHost   *host,       // Pointer to sysHost, the system host (root) structure
    UINT32           ProcStatus   // Return status
    );

/**

  This function returns the pointer to the tag in System Configuration data structure

  @param host            - Pointer to sysHost, the system host (root) structure
  @param VariableTagPtr  - Pointer to the pointer to the System Configuration VariableTagPtr

  @retval None

**/
VOID
OemGetPlatformVariableTagPtr(
    struct sysHost  *host,             // Pointer to sysHost, the system host (root) structure
    VOID            **VariableTagPtr   // Pointer to the pointer to the System Configuration VariableTagPtr
    );

/**

  OEM hook to do any platform specifc init

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - memory controller number ( 0 based)

  @retval None

**/
VOID
OemPostCpuInit(
    struct sysHost *host,         // Pointer to sysHost, the system host (root) structure
    UINT8          socket         // memory controller number ( 0 based)
    );

/**

  Oem Init Serial Debug

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
VOID
OemInitSerialDebug(
    PSYSHOST host                 // Pointer to sysHost, the system host (root) structure
    );

/**

  OEM hook to provide major checkpoint output tothe user interface.
  By default a byte is written to port 80h.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
PlatformCheckpoint(
    PSYSHOST host                 // Pointer to sysHost, the system host (root) structure
    );

/**

  OEM hook to provide common warning output to the user interface

  @param host           - pointer to sysHost structure
  @param majorCode      - major error/warning code
  @param minorCode      - minor error/warning code
  @param logData        - data log

  @retval None

**/
void
PlatformLogWarning(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    majorCode,           // major error/warning code
    UINT8    minorCode,           // minor error/warning code
    UINT32   logData
    );

/**

  OEM hook to provide fatal error output to the user interface and to override
  the halt on error policy.


  @param host          - Pointer to sysHost, the system host (root) structure structure
  @param majorCode     - major error/warning code
  @param minorCode     - minor error/warning code
  @param haltOnError   - pointer to variable that selects policy

  @retval None

**/
void
PlatformFatalError(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure structure
    UINT8    majorCode,           // major error/warning code
    UINT8    minorCode,           // minor error/warning code
    UINT8    *haltOnError         // pointer to variable that selects policy
    );

/**

  OEM hook to read data from a device on the SMBbus.

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - socket number
  @param dev         - SMB device structure
  @param byteOffset  - offset of data to read
  @param data        - Pointer to data to be returned

  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadSmb

**/
UINT8
PlatformReadSmb(
    PSYSHOST         host,        // Pointer to sysHost, the system host (root) structure
    UINT8            socket,      // socket number
    struct smbDevice dev,         // SMB device structure
    UINT8            byteOffset,  // offset of data to read
    UINT8            *data        // Pointer to data to be returned
    );

/**



  @param host        - Pointer to sysHost, the system host (root) structure
  @param dev         - SMB device structure
  @param byteOffset  - offset of data to write
  @param data        - Pointer to data to write

  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace WriteSmb

**/
UINT8
PlatformWriteSmb(
    PSYSHOST         host,        // Pointer to sysHost, the system host (root) structure
    struct smbDevice dev,         // SMB device structure
    UINT8            byteOffset,  // offset of data to write
    UINT8            *data        // Pointer to data to write
    );

/**

  OEM hook for one-time initialization of GPIO parameters such as decode,
  direction, and polarity init.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
PlatformInitGpio(
    PSYSHOST host                 // Pointer to sysHost, the system host (root) structure
    );

/**

  OEM hook to read a byte from a platform GPIO device that is
  not recognized by ReadGpio().

  @param host     - Pointer to sysHost, the system host (root) structure
  @param address  - GPIO device address
  @param data     - Pointer to data

  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace ReadGpio

**/
UINT32
PlatformReadGpio(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT16   address,             // GPIO device address
    UINT32   *data                // Pointer to data
    );

/**

  OEM hook to write a byte to a platform GPIO device that is
  not recognized by WriteGpio().

  @param host     - Pointer to sysHost, the system host (root) structure
  @param address  - GPIO device address
  @param data     - Data to write

  @retval 0 - Hook not implemented (default)
  @retval 1 - Hook implemented and read was successful
  @retval 2 - Hook implemented and read failed / slave device not present
  @retval Return 1 or 2 if this routine should replace WriteGpio

**/
UINT32
PlatformWriteGpio(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT16   address,             // GPIO device address
    UINT32   data                 // Data to write
    );

/**

  Used to add warnings to the promote warning exception list

  @param host - Pointer to sysHost, the system host (root) structure

  @retval SUCCESS

**/
UINT32
PlatformMemInitWarning(
    PSYSHOST host                 // Pointer to sysHost, the system host (root) structure
    );
UINT32
PlatformEwlInit (
    PSYSHOST host
    );

MRC_STATUS
PlatformEwlLogEntry (
    PSYSHOST          host,         // Pointer to the system host (root) structure
    EWL_ENTRY_HEADER  *EwlEntry     // Pointer to the EWL entry buffer
    );


/**

  Used for memory related warnings

  @param host       - Pointer to sysHost, the system host (root) structure
  @param majorCode  - Major warning code
  @param minorCode  - Minor warning code
  @param logData    - Data log

  @retval None

**/
void
PlatformMemLogWarning(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    majorCode,           // Major warning code
    UINT8    minorCode,           // Minor warning code
    UINT32   logData              // Data log
    );

/**

  OEM hook to read the current DIMM Vref setting from a given socket.
  If multiple Vref circuits are present per socket, this function assumes that
  they are set to the same value.

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - processor number
  @param vref    - pointer to UINT8 where the vref value is stored

  @retval SUCCESS if the Vref circuit is present and can be read
  @retval FAILURE if the Vref circuit is not present or cannot be read

**/
UINT32
PlatformMemReadDimmVref(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    socket,              // processor number
    UINT8    *vref                // pointer to UINT8 where the vref value is stored
    );

/**

  OEM hook to write the DIMM Vref setting to a given socket.
  If multiple Vref circuits are present per socket, this function assumes that
  they are set to the same value.

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - processor number
  @param vref    - pointer to UINT8 where the vref value is stored

  @retval SUCCESS if the Vref circuit is present and can be written
  @retval FAILURE if the Vref circuit is not present or cannot be written

**/
UINT32
PlatformMemWriteDimmVref(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    socket,              // processor number
    UINT8    vref                 // pointer to UINT8 where the vref value is stored
    );

/**

  PlatformSetVdd - OEM hook to change the socket Vddq

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - processor number

  @retval SUCCESS if the Vddq change was made
  @retval FAILURE if the Vddq change was not made

**/
UINT8
PlatformSetVdd(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    socket               // processor number
    );

/**

  OEM hook to release any platform clamps affecting CKE
  and/or DDR Reset

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - processor number

  @retval None

**/
void
PlatformReleaseADRClamps(
    PSYSHOST host,                // Pointer to sysHost, the system host (root) structure
    UINT8    socket               // processor number
    );

/**

  OEM hook to allow a secondary ADR dection method other than PCH_PM_STS

  @param host - Pointer to sysHost, the system host (root) structure

  @retval 0  - Hook not implemented or no ADR recovery
  @retval 1  - Hook implemented and platform should recover from ADR

**/
UINT32
PlatformDetectADR(
    PSYSHOST host                 // Pointer to sysHost, the system host (root) structure
    );

/**

  OEM hook to restrict supported configurations if required.

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
PlatformCheckPORCompat(
    PSYSHOST host                 // Pointer to sysHost, the system host (root) structure
    );

/**

  OEM hook for initializing Tcrit to 105 temp offset & the config register
  which is inside of initialization of memory throttling

  @param host        - Pointer to sysHost, the system host (root) structure
  @param socket      - socket number
  @param dev         - SMB Device
  @param byteOffset  - byte Offset
  @param data        - data

  @retval None

**/
void
PlatformHookMst(
    PSYSHOST  host,               // Pointer to sysHost, the system host (root) structure
    UINT8     socket,             // socket number
    struct    smbDevice dev,      // SMB Device
    UINT8     byteOffset,         // byte Offset
    UINT8     *data               // data
    );

/**

  OEM hook for overriding the DDR4 ODT Value table

  @param host                    - Pointer to sysHost, the system host (root) structure
  @param ddr4OdtValueStructPtr   - Pointer to Intel DDR4 ODT Value array
  @param ddr4OdtValueStructSize  - Pointer to size of Intel DDR4 ODT Value array in number of bytes

  @retval None

**/
void
OemLookupDdr4OdtTable(
    PSYSHOST host,                                         // Pointer to sysHost, the system host (root) structure
    struct ddr4OdtValueStruct **ddr4OdtValueStructPtr,     // Pointer to Intel DDR4 ODT Value array
    UINT16 *ddr4OdtValueStructSize                         // Pointer to size of Intel DDR4 ODT Value array in number of bytes
    );

/**

  OEM hook for overriding the memory POR frequency table

  @param host             - Pointer to sysHost, the system host (root) structure
  @param freqTablePtr     - Pointer to Intel POR memory frequency table
  @param freqTableLength  - Pointer to number of DimmPOREntryStruct entries in POR Frequency Table

  @retval None

**/
void
OemLookupFreqTable(
    PSYSHOST host,                               // Pointer to sysHost, the system host (root) structure
    struct DimmPOREntryStruct **freqTablePtr,    // Pointer to Intel POR memory frequency table
    UINT16 *freqTableLength                      // Pointer to number of DimmPOREntryStruct entries in POR Frequency Table
    );

/**

  Platform hooks to log compressed serial messages per thread

  @param host - Pointer to sysHost, the system host (root) structure
  @param packetPtr - Pointer to compressed data packet
  @param packetBytes - Number of bytes in the packet

  @retval SUCCESS - packet successfully handled
          FAILURE - packet not handled

**/
UINT32
OemSendCompressedPacket (
  PSYSHOST host,                                  // Pointer to sysHost
  UINT8    *packetPtr,                            // Pointer to packet
  UINT32   packetBytes                            // Number of bytes in packet
  );

/**

  Platform specific input setup

  @param host - Pointer to sysHost, the system host (root) structure

  @retval None

**/
void
GetSetupOptionsEx(
    PSYSHOST host                 // Pointer to sysHost, the system host (root) structure
    );

#endif // _PLATFORM_API_H_
