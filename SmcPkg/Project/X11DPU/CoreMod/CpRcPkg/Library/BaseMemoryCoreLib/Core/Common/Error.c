//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.0xx
//      Bug Fixed:  Fixed memory error LED can't turn on for early POST memory error.
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Aug/24/2017
//
//  Rev. 1.0xx
//      Bug Fixed:  Add memory error LED function
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jul/03/2017
//
//  Rev. 1.04
//      Bug Fixed:  fixed 2 times log when promotion is enabled.
//      Reason:     PlatformLogWarning should not be called when promtion is enabled
//                  beacuse the FatalErrorEx will record the log later.
//      Auditor:    Max Mu
//      Date:       Jun/30/2017
//
//  Rev. 1.03
//      Bug Fixed:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/05/2017
//
//  Rev. 1.02
//      Bug Fixed:  Fix coding error.
//      Reason:     
//      Auditor:    Jacker Yeh
//      Date:       Aug/17/2016
//
//  Rev. 1.01
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/7/2016
//
//  Rev. 1.00
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
//
//*****************************************************************************

/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
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
#ifdef _MSC_VER
#pragma warning(disable:4740)
#endif

#include "SysFunc.h"
#include "Token.h"//SMCPKG_SUPPORT

#include "CoreApi.h"

#if SMCPKG_SUPPORT
UINT8 EFIAPI IoWrite8 (IN UINTN Port,IN UINT8 Value);
UINT8 EFIAPI IoRead8 (IN UINTN Port);
#endif

//
// Local Prototypes
//
void
Exit (
  PSYSHOST host,
  UINT8 majorCode,
  UINT8 minorCode
  );

/**

  Log warning to the warning log

  @param host              - pointer to sysHost
  @param warningCode       - Major warning code
  @param minorWarningCode  - Minor warning code
  @param socket              - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

**/
void
OutputWarning (
  PSYSHOST host,
  UINT8    warningCode,
  UINT8    minorWarningCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
#if ENHANCED_WARNING_LOG
  //Create enhanced warning of type 1 and store in EWL structure
  EwlOutputType1 (host, warningCode, minorWarningCode, socket, ch, dimm, rank);
#else
  //
  // Log the warning in classic warning log
  //
  PlatformOutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);
  LogWarning (host, warningCode, minorWarningCode, (socket << 24) | (ch << 16) | (dimm << 8) | rank);
#endif
}

#if SMCPKG_SUPPORT		//SMCPKG_SUPPORT++
void
FatalErrorEx (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT32   logData
  ){
    host->var.mem.socket[host->var.common.socketId].logData = logData | 0x80000000; //override
    FatalError (host, majorCode, minorCode);
}

VOID WriteCMOS (
  IN UINT8  Offset,
  IN OUT UINT8 Value )
{
    IoWrite8(0x70, Offset);
    IoWrite8(0xEB, Offset);
    IoWrite8(0x71, Value);
}

UINT8 ReadCMOS (
  IN UINT8  Offset)
{
    UINT8 Value;
    IoWrite8(0x70, Offset);
    IoWrite8(0xEB, Offset);
    Value = IoRead8(0x71);
    return Value;
}	

VOID
RWMSSMBUSByte (
  IN UINT8 CMD, //1:Read  2:Write
  IN UINT8 SMBusAddr,
  IN UINT8 Offset,
  IN OUT UINT8 *Data
)
{
    UINT8 Timeout = 0;
//1. Clear Status bit
    while(IoRead8(SMBUS_BASE_ADDRESS) != 0){
        IoWrite8(SMBUS_BASE_ADDRESS, 0xFF);
        IoWrite8(0xED, 0xFF);
        IoWrite8(0xED, 0xFF);

        Timeout++;
        if (Timeout == 0xff)
            break;
    }
//2. Set slave address
	IoWrite8(SMBUS_BASE_ADDRESS + 0x04, (SMBusAddr << 1) + CMD);
	IoWrite8(0xED, 0xFF);
	IoWrite8(0xED, 0xFF);
		
//3. Set Read/Write offset
	IoWrite8(SMBUS_BASE_ADDRESS + 0x03, Offset);
	IoWrite8(0xED, 0xFF);
	IoWrite8(0xED, 0xFF);
		
//4. Set Data.
	IoWrite8(SMBUS_BASE_ADDRESS + 0x05, *Data);
	IoWrite8(0xED, 0xFF);
	IoWrite8(0xED, 0xFF);	
		
//5. Set transfer command.
	IoWrite8(SMBUS_BASE_ADDRESS + 0x02, 0x48);//Read Byte.
	IoWrite8(0xED, 0xFF);
	IoWrite8(0xED, 0xFF);
		
//6. Wait status bit
    while(IoRead8(SMBUS_BASE_ADDRESS) != 0x02){
        IoWrite8(0xED, 0xFF);
        IoWrite8(0xED, 0xFF);

        Timeout++;
        if (Timeout == 0xff){
            break;
        }
    }

//7. Read Data back.    
	*Data = IoRead8(SMBUS_BASE_ADDRESS + 0x05);

	IoWrite8(0xED, 0xFF);
	IoWrite8(0xED, 0xFF);
}

typedef struct{
    UINT8	DIMM_NUM[3];
}IMC_DIMM_NUM;

VOID
X11DPUMemErrLedHandle(
  PSYSHOST host,
  UINT8 majorCode,
  UINT8 minorCode,
  UINT32 logData
  )
{
  UINT8 tempData[4];
  UINT8 PCA9539_ADDR1 = 0x75;
  UINT8 PCA9539_ADDR2 = 0x76;
  UINT32 ErrDimm, ErrDimmMap;	
  UINT8 DimmNum, Socket, Channel;
  BOOLEAN DimmFailure = FALSE;
  IMC_DIMM_NUM IMC_DIMM = CPU1_DIMM_NUM;
  //DEBUG ((EFI_D_INFO, "X11DPUMemErrLedHandle start\n"));

  switch(majorCode){
      case 0xED: //Incorrect memory DIMM population, check Memory Population Rule.
      case 0xEF: //Memory initialization panic, apply AC Reset.
      case 0x07: //Memory DIMM incompatible with memory controllers.
      case 0x09: //Failed to honor Lockstep mode
      case 0x0C: //Failed to honor Mirror mode
      case 0x0E: //Failed to honor Interleave mode
      case 0x17: //Incorrect memory DIMM population.
	      DimmFailure = FALSE;
          break;
		  
      default:
	      DimmFailure = TRUE;
          break;
  }
  
  if(!DimmFailure) return;

  Socket = (UINT8)((logData >> 24) & 0xFF);
  Channel = (UINT8)((logData >> 16) & 0xFF);
  DimmNum = (UINT8)((logData >> 8) & 0xFF);

  ErrDimm = ErrDimmMap = 0x00000000;
  ErrDimm = Channel * (IMC_DIMM.DIMM_NUM[0]) + DimmNum;

  if(Socket == 0)
      ErrDimmMap = 0x00000001;
  else if(Socket == 1)
      ErrDimmMap = 0x00010000;

  ErrDimmMap = ErrDimmMap << ErrDimm;	

  WriteCMOS(CMOS_MEM_ERR_FLAG_0, 0xAA); //Set memory error flag
  WriteCMOS(CMOS_MEM_ERR_FLAG_1, 0x55); //Set memory error flag

  tempData[0] = ReadCMOS(CMOS_ERR_DIMM_LOC0); //Get memory error DIMM location 0
  tempData[1] = ReadCMOS(CMOS_ERR_DIMM_LOC1); //Get memory error DIMM location 1
  tempData[2] = ReadCMOS(CMOS_ERR_DIMM_LOC2); //Get memory error DIMM location 2
  tempData[3] = ReadCMOS(CMOS_ERR_DIMM_LOC3); //Get memory error DIMM location 3

  //Set error DIMM location of this time, keep previous one
  tempData[0] |= (UINT8)(ErrDimmMap & 0x0000003F); //CPU1 CH A&B&C
  tempData[1] |= (UINT8)((ErrDimmMap & 0x00000FC0) >> 6); //CPU1 CH D&E&F
  tempData[2] |= (UINT8)((ErrDimmMap & 0x003F0000) >> 16); //CPU2 CH A&B&C
  tempData[3] |= (UINT8)((ErrDimmMap & 0x0FC00000) >> 22); //CPU2 CH D&E&F
	
  WriteCMOS(CMOS_ERR_DIMM_LOC0, tempData[0]); //Set memory error DIMM location 0
  WriteCMOS(CMOS_ERR_DIMM_LOC1, tempData[1]); //Set memory error DIMM location 1
  WriteCMOS(CMOS_ERR_DIMM_LOC2, tempData[2]); //Set memory error DIMM location 2
  WriteCMOS(CMOS_ERR_DIMM_LOC3, tempData[3]); //Set memory error DIMM location 3	

  //Set error DIMM bitmap
  RWMSSMBUSByte(0, PCA9539_ADDR1, 0x02, &tempData[0]);
  RWMSSMBUSByte(0, PCA9539_ADDR1, 0x03, &tempData[1]);
  
  RWMSSMBUSByte(0, PCA9539_ADDR2, 0x02, &tempData[2]);
  RWMSSMBUSByte(0, PCA9539_ADDR2, 0x03, &tempData[3]);
  
  //DEBUG ((EFI_D_INFO, "X11DPUMemErrLedHandle end\n"));
  return;
}
#endif					//SMCPKG_SUPPORT++
/**

  Log a warning to the host structure legacy warning log

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log
  @param logData   - 32 bit error specific data to log

  @retval N/A

**/
void
LogWarning (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT32   logData
  )
{
#if ENHANCED_WARNING_LOG == 0
  UINT8   promoteWarning;
  UINT8   PostCode;
  UINT32  i;

  promoteWarning = 0;

  //
  // Append warningCode to the status log
  //
  i = host->var.common.status.index;
  if (i < MAX_LOG) {
    host->var.common.status.log[i].code = (UINT32) ((majorCode << 8) | minorCode);
    host->var.common.status.log[i].data = logData;
    host->var.common.status.index++;
  }

  //
  // Print warning info
  //
  rcPrintf ((
           host,
           "\nA warning has been logged! Warning Code = 0x%X, Minor Warning Code = 0x%X, Data = 0x%X\n",
           majorCode,
           minorCode,
           logData
           ));
  if (((logData >> 24) & 0xFF) != 0xFF) {
    rcPrintf ((host, "S%d", (logData >> 24) & 0xFF));
  }

  if (((logData >> 16) & 0xFF) != 0xFF) {
    rcPrintf ((host, " Ch%d", (logData >> 16) & 0xFF));
  }

  if (((logData >> 8) & 0xFF) != 0xFF) {
    rcPrintf ((host, " DIMM%d", (logData >> 8) & 0xFF));
  }

  if ((logData & 0xFF) != 0xFF) {
    rcPrintf ((host, " Rank%d", logData & 0xFF));
  }

  rcPrintf ((host, "\n\n"));

  //
  // Promote warning based on setup option
  //
  if (host->setup.common.options & PROMOTE_WARN_EN) {
    promoteWarning = 1;

    //
    // Search platform exception list
    //
    for (i = 0; i < host->var.common.promoteWarnLimit; i++) {
      if ((host->var.common.promoteWarnList[i] == (UINT16)((majorCode << 8) | minorCode)) ||
        (host->var.common.promoteWarnList[i] == (UINT16)((majorCode << 8) | WARN_MINOR_WILDCARD))
        ) {
        promoteWarning = 0;
        break;
      }
    }
  }//Promote Warning enabled

  if ((majorCode >= WARN_FPT_CORRECTABLE_ERROR) && (majorCode <= WARN_FPT_UNCORRECTABLE_ERROR)) {
    if (host->setup.common.options & PROMOTE_MRC_WARN_EN) {
      promoteWarning = 1;
    } else {
      promoteWarning = 0;
    }
  } //FPT Warning Code

  //
  // Call platform hook to handle warnings
  //
#if SMCPKG_SUPPORT
#ifdef SERIAL_DBG_MSG
    rcPrintf ((host, "PlatformLogWarning (host, majorCode=0x%x, minorCode=0x%x, logData=0x%x, promoteWarning=0x%x); \n\n", majorCode, minorCode, logData, promoteWarning));
#endif
    X11DPUMemErrLedHandle(host, majorCode, minorCode, logData);
  if (!promoteWarning)
#endif
    PlatformLogWarning (host, majorCode, minorCode, logData);

  //
  // Promote warning to fatal error
  //
  if (promoteWarning) {
    rcPrintf ((host, "Warning upgraded to Fatal Error!\n"));
    //Postcode to identify DIMM channel errors
    if (host->setup.common.DfxAltPostCode) {
      //PostCode will be in the form 0x7y, where y represent the DIMM channel where the error was detected
      PostCode = ERR_DIMM_CHANNEL | ((logData >> 16) & 0xFF);
      OutPort8 (host, host->setup.common.debugPort, PostCode);
    }
#if SMCPKG_SUPPORT	//SMCPKG_SUPPORT++
    FatalErrorEx (host, majorCode, minorCode, logData);
#else    
    FatalError (host, majorCode, minorCode);
#endif //  SMCPKG_SUPPORT++  
  }
#endif // ENHANCED_WARNING_LOG
}


/**

  Log Error to the warning log

  @param host              - pointer to sysHost
  @param ErrorCode         - Major warning code
  @param minorErrorCode    - Minor warning code
  @param socket            - socket number
  @param ch                - channel number
  @param dimm              - dimm number
  @param rank              - rank number

**/
void
OutputError (
  PSYSHOST host,
  UINT8    ErrorCode,
  UINT8    minorErrorCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
  PlatformOutputError (host, ErrorCode, minorErrorCode, socket, ch, dimm, rank);
  //
  // Log the error
  //
  LogError (host, ErrorCode, minorErrorCode, (socket << 24) | (ch << 16) | (dimm << 8) | rank);
}

/**

  Log an Error to the host structure

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log
  @param Socket    - Socket error occurred on
  @param BistMask  - 32 bit Bist Mask results

  @retval N/A

**/

void
LogCpuError (
         PSYSHOST host,
         UINT8    majorCode,
         UINT8    minorCode,
         UINT8    Socket,
         UINT32   BistMask
         )
{
#if ENHANCED_WARNING_LOG == 0
  UINT32  i;
#endif
  UINT32  SktBistMask;

  //
  // Provide Socket & Bist failure information in one data entry for legacy and platform implementations.
  // BITS[31-28] - Socket Index of error
  // BITS[27-0]  - Bitmap of failed cores
  //
  SktBistMask = ((Socket & 0x0f) << 28) | BistMask;

  //
  // Append errorCode to the status log
  //
#if ENHANCED_WARNING_LOG
  EwlOutputType7 (host, majorCode, minorCode, Socket, BistMask);
#else
  i = host->var.common.status.index;
  if (i < MAX_LOG) {
    host->var.common.status.log[i].code = (UINT32) ((majorCode << 8) | minorCode);
    host->var.common.status.log[i].data = SktBistMask;
    host->var.common.status.index++;
  }
#endif

  //
  // Call platform hook to handle warnings
  //
  PlatformLogWarning (host, majorCode, minorCode, SktBistMask);


  rcPrintf ((
           host,
           "\nAn Error logged! Error Code = 0x%X, Minor Error Code = 0x%X, Socket = 0x%X, Data = 0x%X\n",
           majorCode,
           minorCode,
           Socket,
           BistMask
           ));

}

/**

  Log an Error to the host structure

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log
  @param logData   - 32 bit error specific data to log

  @retval N/A

**/
void
LogError (
         PSYSHOST host,
         UINT8    majorCode,
         UINT8    minorCode,
         UINT32   logData
         )
{
#if ENHANCED_WARNING_LOG == 0
  UINT32  i;
#endif

  //
  // Append errorCode to the status log
  //
#if ENHANCED_WARNING_LOG
  EwlOutputType1 (host, majorCode, minorCode, (UINT8) (logData >> 24), (UINT8) (logData >> 16), (UINT8) (logData >> 8), (UINT8) logData);
#else
  i = host->var.common.status.index;
  if (i < MAX_LOG) {
    host->var.common.status.log[i].code = (UINT32) ((majorCode << 8) | minorCode);
    host->var.common.status.log[i].data = logData;
    host->var.common.status.index++;
  }
#endif

  //
  // Call platform hook to handle warnings
  //
  PlatformLogWarning (host, majorCode, minorCode, logData);


  rcPrintf ((
           host,
           "\nAn Error logged! Error Code = 0x%X, Minor Error Code = 0x%X, Data = 0x%X\n",
           majorCode,
           minorCode,
           logData
           ));

  if (((logData >> 24) & 0xFF) != 0xFF) {
    rcPrintf ((host, "S%d", (logData >> 24) & 0xFF));
  }

  if (((logData >> 16) & 0xFF) != 0xFF) {
    rcPrintf ((host, " Ch%d", (logData >> 16) & 0xFF));
  }

  if (((logData >> 8) & 0xFF) != 0xFF) {
    rcPrintf ((host, " DIMM%d", (logData >> 8) & 0xFF));
  }

  if ((logData & 0xFF) != 0xFF) {
    rcPrintf ((host, " Rank%d", logData & 0xFF));
  }

  rcPrintf ((host, "\n\n"));

  //
  // Promote fatal error
  //
#if SMCPKG_SUPPORT			//SMCPKG_SUPPORT++
    //X11DPUMemErrLedHandle(host, majorCode, minorCode, logData);
    FatalErrorEx (host, majorCode, minorCode, logData);
#else 						//SMCPKG_SUPPORT++
  FatalError (host, majorCode, minorCode);
#endif 						//SMCPKG_SUPPORT++
}



/**

  Halts the platform

  @param host      - Pointer to the system host (root) structure

  @retval N/A

**/
void
HaltOnError (
            PSYSHOST host,
            UINT8    majorCode,
            UINT8    minorCode
            )
{
  UINT8   haltOnError;


  //
  // Default halt on error based on setup option
  //
  if (host->setup.common.options & HALT_ON_ERROR_EN) {
    haltOnError = 1;
  } else {
    haltOnError = 0;
  }

  //
  // Loop forever
  //
  RcDeadLoop (haltOnError);

  //
  // Exit now!
  //
  Exit (host, majorCode, minorCode);
} // HaltOnError


/**

  Exits reference code

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log

  @retval N/A

**/
void
Exit (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode
  )
{

#if defined (IA32) || defined (RC_SIM)
  UINT32  exitFrame;
  UINT32  exitVector;

  exitFrame   = host->var.common.exitFrame;
  exitVector  = host->var.common.exitVector;

#ifdef __GNUC__
  asm
  (
    "xor %%eax, %%eax\n\t"
    "mov %0, %%ah\n\t"
    "mov %1, %%al\n\t"
    "mov %2, %%esp\n\t"
    "mov %3, %%edi\n\t"
    "jmp *%%edi"
    :
    : "m" (majorCode), "m" (minorCode), "m" (exitFrame), "m" (exitVector)
    :
  );
#else
  _asm
  {
    xor eax, eax
    mov ah, majorCode
    mov al, minorCode
    mov esp, exitFrame
    mov edi, exitVector
    jmp edi
  }
#endif
#endif // defined (IA32) || defined (RC_SIM)
}

/**

  Adds error/exception to the promoteWarningList in host structure if possible

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log

  @retval 0 - Successfully added to log
  @retval 1 - Log full

**/
UINT32
SetPromoteWarningException (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode
  )
{
  UINT32  status;
  UINT32  i;

  status = SUCCESS;
  //
  // Search warning list for unused entry
  //
  for (i = 0; i < MAX_PROMOTE_WARN_LIMIT; i++) {
    if (host->var.common.promoteWarnList[i] == 0) {
      host->var.common.promoteWarnList[i] = (UINT16) ((majorCode << 8) | minorCode);
      if (i == host->var.common.promoteWarnLimit) {
        host->var.common.promoteWarnLimit++;
      }
      break;
    }
  }

  if (i == MAX_PROMOTE_WARN_LIMIT) {
    status = FAILURE;
  }

  return status;
}

/**

  Removes a warning from the promoteWarningList in host structure if present

  @param host      - Pointer to the system host (root) structure
  @param majorCode - Major error code to log
  @param minorCode - Minor error code to log

  @retval 0 - Successfully cleared from log
  @retval 1 - Warning/Error not in log

**/
UINT32
ClearPromoteWarningException (
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode
  )
{
  UINT32  status;
  UINT32  i;

  status = SUCCESS;
  //
  // Search warning list for unused entry
  //
  for (i = 0; i < MAX_PROMOTE_WARN_LIMIT; i++) {
    if (host->var.common.promoteWarnList[i] == (UINT16) ((majorCode << 8) | minorCode)) {
      host->var.common.promoteWarnList[i] = 0;
      if (i == host->var.common.promoteWarnLimit) {
        host->var.common.promoteWarnLimit--;
      }
      break;
    }
  }

  if (i == MAX_PROMOTE_WARN_LIMIT) {
    status = FAILURE;
  }

  return status;
}

/**

  Used to add warnings to the promote warning exception list. Directs call to Chip layer.

  @param host  - pointer to sysHost

  @retval N/A

**/
UINT32
MemInitWarning (
  PSYSHOST host
  )
{
  return PlatformMemInitWarning(host);
}

void
MemCheckBoundary (
   PSYSHOST host,
   UINT32 boundary,
   UINT32 arrayBoundary
   )
/*++

Routine Description:

  Checks that boundary is less than arrayBoundary to avoid array out of bounds access

Arguments:

  boundary
  arrayBoundary

Returns:

  N/A

--*/
{
  if (boundary > arrayBoundary) {
#ifdef SERIAL_DBG_MSG
    MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\nERROR: array out of bounds\n"));
#endif
#if SMCPKG_SUPPORT
    OutputError (host, ERR_MRC_STRUCT, ERR_ARRAY_OUT_OF_BOUNDS, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
#else
    FatalError (host, ERR_MRC_STRUCT, ERR_ARRAY_OUT_OF_BOUNDS);
#endif
  }
} // MemCheckBoundary

void
MemCheckIndex (
  PSYSHOST host,
  UINT32 index,
  UINT32 arraySize
  )
/*++

Routine Description:

  Checks that index is less than arraySize to avoid array out of bounds access

Arguments:

  index
  arraySize

Returns:

  N/A


--*/
{
  if (index >= arraySize) {
#ifdef SERIAL_DBG_MSG
    MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "\nERROR: array out of bounds\n"));
#endif
#if SMCPKG_SUPPORT
    OutputError (host, ERR_MRC_STRUCT, ERR_ARRAY_OUT_OF_BOUNDS, 0xFF, 0xFF, 0xFF, 0xFF);
#else
    FatalError (host, ERR_MRC_STRUCT, ERR_ARRAY_OUT_OF_BOUNDS);
#endif
  }
} // MemCheckIndex

VOID EwlPrintMemoryLocation (
  PSYSHOST                  host,
  EWL_ENTRY_MEMORY_LOCATION memLoc
){
#ifdef SERIAL_DBG_MSG
  if (memLoc.Socket != 0xFF) {
    rcPrintf ((host, "\nSocket %d", memLoc.Socket));
  }
  if (memLoc.Channel != 0xFF) {
    rcPrintf ((host, "\nChannel %d", memLoc.Channel));
  }
  if (memLoc.Dimm != 0xFF) {
    rcPrintf ((host, "\nDimm %d", memLoc.Dimm));
  }
  if (memLoc.Rank != 0xFF) {
    rcPrintf ((host, "\nRank %d", memLoc.Rank));
  }
#endif //SERIAL_DBG_MSG
}

VOID
EwlPrintWarning (
  PSYSHOST         host,
  EWL_ENTRY_HEADER *WarningHeader
  )
{
#ifdef SERIAL_DBG_MSG
  //Get type from Enhanced Warning Header
  EWL_TYPE type = WarningHeader->Type;
  EWL_ENTRY_MEMORY_LOCATION memLoc;
  CHAR groupStrBuf[MAX_STRING_LENGTH];
  CHAR levelStrBuf[MAX_STRING_LENGTH];
  CHAR signalStrBuf[MAX_STRING_LENGTH];
  EWL_ENTRY_TYPE1* type1;
  EWL_ENTRY_TYPE2* type2;
  EWL_ENTRY_TYPE3* type3;
  EWL_ENTRY_TYPE4* type4;
  EWL_ENTRY_TYPE5* type5;
  EWL_ENTRY_TYPE7* type7;
  EWL_ENTRY_TYPE10* type10;

  rcPrintf ((host, "\nEnhanced warning of type %d logged:", type));
  
  //Print warning codes and current checkpoint (Context)
  EwlPrintBasicInfo (host, WarningHeader);

  //Print type specific details
  switch (type) {
    case EwlType0:
      rcPrintf ((host, "\nEwl spec does not define type 0 warning"));
      break;

    case EwlType1:
      //For type 1 print MemoryLocation
      type1 = (EWL_ENTRY_TYPE1 *)WarningHeader;
      memLoc= type1->MemoryLocation;
      EwlPrintMemoryLocation (host, memLoc);
      break;

    case EwlType2:
      //Print Type 2 warning details
      type2 = (EWL_ENTRY_TYPE2 *)WarningHeader;
      memLoc = type2->MemoryLocation;

      EwlPrintMemoryLocation (host, memLoc);
      if (type2->Strobe != 0xFF) {
        rcPrintf ((host, "\nStrobe %d", type2->Strobe));
      }
      if (type2->Bit != 0xFF) {
        rcPrintf ((host, "\nBit %d", type2->Bit));
      }
      if (type2->Level != GsmLtDelim) {
        rcPrintf ((host, "\nLevel "));
        CHIP_FUNC_CALL(host, GetLevelStr (type2->Level, levelStrBuf));
        rcPrintf ((host, levelStrBuf));
      }
      if (type2->Group != GsmGtDelim) {
        rcPrintf ((host, "\nGroup "));
        CHIP_FUNC_CALL(host, GetGroupStr (type2->Group, groupStrBuf));
        rcPrintf ((host, groupStrBuf));
      }
      if (type2->EyeSize != 0xFF) {
        rcPrintf ((host, "\nEyesize %d", type2->EyeSize));
      }
      break;

    case EwlType3:
      //Print Type 3 warning details
      type3 = (EWL_ENTRY_TYPE3 *)WarningHeader;
      memLoc = type3->MemoryLocation;
      
      EwlPrintMemoryLocation (host, memLoc);
      if (type3->Level != GsmLtDelim) {
        rcPrintf ((host, "\nLevel "));
         CHIP_FUNC_CALL(host, GetLevelStr (type3->Level, levelStrBuf));
        rcPrintf ((host, levelStrBuf));
      }
      if (type3->Group != GsmGtDelim) {
        rcPrintf ((host, "\nGroup "));
        CHIP_FUNC_CALL(host, GetGroupStr (type3->Group, groupStrBuf));
        rcPrintf ((host, groupStrBuf));
      }
      if (type3->Signal != gsmCsnDelim) {
        rcPrintf ((host, "\nSignal: "));
        CHIP_FUNC_CALL(host, GetSignalStr (type3->Signal, signalStrBuf));
        RC_ASSERT ((StrLenLocal (signalStrBuf) < MAX_STRING_LENGTH), 0, 0);
        rcPrintf((host, signalStrBuf));
      }
      if (type3->EyeSize != 0xFF) {
        rcPrintf ((host, "\nEyesize %d", type3->EyeSize));
      }
      break;

    case EwlType4:
      //Print Type 4 warning details
      type4 = (EWL_ENTRY_TYPE4 *)WarningHeader;
      memLoc = type4->MemoryLocation;

      EwlPrintMemoryLocation (host, memLoc);

      if (type4->MemtestType != AdvMtDelim) {
        rcPrintf((host, "\nAdvanced memtest of type %d", type4->MemtestType));
      }
      rcPrintf ((host,"\nDat0S:   %08x", type4->AdvMemtestErrorInfo.Dat0S));
      rcPrintf ((host,"\nDat1S:   %08x", type4->AdvMemtestErrorInfo.Dat1S));
      rcPrintf ((host,"\nDat2S:   %08x", type4->AdvMemtestErrorInfo.Dat2S));
      rcPrintf ((host,"\nDat3S:   %08x", type4->AdvMemtestErrorInfo.Dat3S));
      rcPrintf ((host,"\nEccS:    %08x", type4->AdvMemtestErrorInfo.EccS));
      rcPrintf ((host,"\nChunk:   %08x", type4->AdvMemtestErrorInfo.Chunk));
      rcPrintf ((host,"\nColumn:  %08x", type4->AdvMemtestErrorInfo.Column));
      rcPrintf ((host,"\nRow:     %08x", type4->AdvMemtestErrorInfo.Row));
      rcPrintf ((host,"\nBank:    %08x", type4->AdvMemtestErrorInfo.Bank));
      rcPrintf ((host,"\nRank:    %08x", type4->AdvMemtestErrorInfo.Rank));
      rcPrintf ((host,"\nSubrank: %08x", type4->AdvMemtestErrorInfo.Subrank));
      rcPrintf ((host,"\nCount:   %d", type4->Count));
      break;

    case EwlType5:
      //Print Type 5 warning details
      type5 = (EWL_ENTRY_TYPE5 *)WarningHeader;
      memLoc = type5->MemoryLocation;

      EwlPrintMemoryLocation (host, memLoc);
      if (type5->SubRank != 0xFF) {
        rcPrintf ((host, "\nSubRank %d", type5->SubRank));
      }

      if (type5->BankAddress != 0xFF) {
        rcPrintf ((host, "\nBank Address %d", type5->BankAddress));
      }
      rcPrintf ((host, "\nDq bytes 8 - 0 : 0x%02X %02X %02X %02X %02X %02X %02X %02X %02X", 
                 type5->DqBytes[8], 
                 type5->DqBytes[7], 
                 type5->DqBytes[6], 
                 type5->DqBytes[5], 
                 type5->DqBytes[4], 
                 type5->DqBytes[3], 
                 type5->DqBytes[2], 
                 type5->DqBytes[1], 
                 type5->DqBytes[0]));
      break;

    case EwlType7:
      //Print Type 7 warning details
      type7 = (EWL_ENTRY_TYPE7 *)WarningHeader;
      rcPrintf ((host, "\nSocket: %X", type7->Socket));
      rcPrintf ((host, "\nCore: %d", type7->Core));
      break;

    case EwlType10:
      //Print type 10 warning details
      type10 = (EWL_ENTRY_TYPE10 *)WarningHeader;
      memLoc = type10->MemoryLocation;
      
      EwlPrintMemoryLocation (host, memLoc);
      rcPrintf ((host, "\nSwizzle Pattern: 0x%X", type10->SwizzlePattern));
      rcPrintf ((host, "\nSwizzled Lanes: 0x%X", type10->SwizzledDqLanes));
      rcPrintf ((host, "\nLanes Per Strobe: %d", type10->LanesPerStrobe));
      rcPrintf ((host, "\nStrobe: %d", type10->Strobe));
      break;
    
    default:
      //expand this print function as warning types get implemented
      //We shouldn't reach this default case
      rcPrintf ((host, "\nPrint function has not been implemented for this type of warning"));
      break;
  }
  rcPrintf ((host, "\n"));
#endif

  
}

VOID
EwlPrintBasicInfo (
  PSYSHOST          host,
  EWL_ENTRY_HEADER  *Header
  )
{
  //
  // Treat warning as type 1, collect basic information and print to serial log
  //
#ifdef SERIAL_DBG_MSG
  EWL_ENTRY_TYPE1 *basicWarning;

  basicWarning = (EWL_ENTRY_TYPE1 *)Header;

  rcPrintf ((host, "\nMajor Warning Code = 0x%02X, Minor Warning Code = 0x%02X,", basicWarning->Context.MajorWarningCode, basicWarning->Context.MinorWarningCode));
  rcPrintf ((host, "\nMajor Checkpoint: 0x%02X", basicWarning->Context.MajorCheckpoint));
  rcPrintf ((host, "\nMinor Checkpoint: 0x%02X", basicWarning->Context.MinorCheckpoint));
#endif // SERIAL_DBG_MSG
}

#if ENHANCED_WARNING_LOG
UINT32
CountWarningsInLog (
  PSYSHOST host
  )
{
  UINT8                 *Buffer         = &host->var.common.ewlPrivateData.status.Buffer[0];
  UINT32                numEntries      = 0;
  EWL_ENTRY_HEADER      *currentWarning = (EWL_ENTRY_HEADER*) &Buffer[0];
  VOID                  *freeSpace      = &Buffer[host->var.common.ewlPrivateData.status.Header.FreeOffset];

  while ((VOID*) currentWarning < freeSpace) {
    currentWarning = (EWL_ENTRY_HEADER*) ((UINT8*)currentWarning + currentWarning->Size);
    numEntries++;
  }

  return numEntries;
}
#endif

//
// Constructs a type 1 warning according to EWL spec
//
void
EwlOutputType1 (
  PSYSHOST host,
  UINT8    warningCode,
  UINT8    minorWarningCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  )
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE1   legacyWarning;
  UINT8 socketId = host->var.common.socketId;

  PlatformOutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);

  //
  // A LogWarning call results in a legacy type 1 warning being added to the enhanced warning log
  //

  legacyWarning.Header.Size               = sizeof(EWL_ENTRY_TYPE1);
  legacyWarning.Header.Type               = EwlType1;
  legacyWarning.Header.Severity           = EwlSeverityWarning;
  legacyWarning.Context.MajorCheckpoint   = ((host->var.mem.lastCheckpoint[socketId]) >> 24) & 0xFF; // shift to get Major Checkpoint
  legacyWarning.Context.MinorCheckpoint   = ((host->var.mem.lastCheckpoint[socketId]) >> 16) & 0xFF; // shift to get Minor
  legacyWarning.Context.MajorWarningCode  = warningCode;
  legacyWarning.Context.MinorWarningCode  = minorWarningCode;
  legacyWarning.MemoryLocation.Socket     = socket;
  legacyWarning.MemoryLocation.Channel    = ch;
  legacyWarning.MemoryLocation.Dimm       = dimm;
  legacyWarning.MemoryLocation.Rank       = rank;

  //
  // Output warning details to the Serial Log
  //
  EwlPrintWarning (host, &(legacyWarning.Header));
  
  //
  // Log the warning
  //
  PlatformEwlLogEntry (host, &legacyWarning.Header);

#else
  OutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);
#endif // ENHANCED_WARNING_LOG
}

//
// Constructs type 2 warning according to EWL spec and calls PlatformEwlLogEntry
//
void
EwlOutputType2 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  UINT8        strobe,
  UINT8        bit,
  GSM_GT       group,
  GSM_LT       level,
  UINT8        eyeSize
  )
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE2   type2;
  UINT8 socketId = host->var.common.socketId;

  PlatformOutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);

  type2.Header.Size               = sizeof(EWL_ENTRY_TYPE2);
  type2.Header.Type               = EwlType2;
  type2.Header.Severity           = severity;

  type2.Context.MajorCheckpoint   = ((host->var.mem.lastCheckpoint[socketId]) >> 24) & 0xFF; //shift to get Major Checkpoint
  type2.Context.MinorCheckpoint   = ((host->var.mem.lastCheckpoint[socketId]) >> 16) & 0xFF; //shift to get Minor
  type2.Context.MajorWarningCode  = warningCode;
  type2.Context.MinorWarningCode  = minorWarningCode;

  type2.MemoryLocation.Socket     = socket;
  type2.MemoryLocation.Channel    = ch;
  type2.MemoryLocation.Dimm       = dimm;
  type2.MemoryLocation.Rank       = rank;

  type2.Level                     = level;
  type2.Group                     = group;
  type2.Strobe                    = strobe;
  type2.Bit                       = bit;
  type2.EyeSize                   = eyeSize;

  //
  // Output warning details to the Serial Log
  //
  EwlPrintWarning (host, &type2.Header);

  //
  // Log the warning
  //
  PlatformEwlLogEntry (host, &type2.Header);

#else
  OutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);
#endif // ENHANCED_WARNING_LOG
}

//
// Constructs type 3 warning according to EWL spec and calls PlatformEwlLogEntry
//
void
EwlOutputType3 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  GSM_GT       group,
  GSM_LT       level,
  GSM_CSN      signal,
  UINT8        eyeSize
  )
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE3   type3;
  UINT8 socketId = host->var.common.socketId;
#endif

  MemDebugPrint ((
    host,
    SDBG_MAX,
    socket, ch,
    NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "Eye width is too small: %d\n", eyeSize
    ));

#if ENHANCED_WARNING_LOG
  PlatformOutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);//OEM specific

  type3.Header.Size               = sizeof(EWL_ENTRY_TYPE3);
  type3.Header.Type               = EwlType3;
  type3.Header.Severity           = severity;

  type3.Context.MajorCheckpoint   = ((host->var.mem.lastCheckpoint[socketId]) >> 24) & 0xFF;//shift to get Major Checkpoint
  type3.Context.MinorCheckpoint   = ((host->var.mem.lastCheckpoint[socketId]) >> 16) & 0xFF;//shift to get Minor
  type3.Context.MajorWarningCode  = warningCode;
  type3.Context.MinorWarningCode  = minorWarningCode;

  type3.MemoryLocation.Socket     = socket;
  type3.MemoryLocation.Channel    = ch;
  type3.MemoryLocation.Dimm       = dimm;
  type3.MemoryLocation.Rank       = rank;

  type3.Level                     = level;
  type3.Group                     = group;
  type3.Signal                    = signal;
  type3.EyeSize                   = eyeSize;

  //
  // Output warning details to the Serial Log
  //
  EwlPrintWarning (host, &type3.Header);

  //
  // Log the warning
  //
  PlatformEwlLogEntry (host, &type3.Header);

#else
  OutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);
#endif // ENHANCED_WARNING_LOG
}

//
// Constructs type 4 warning according to EWL spec, and calls PlatformEwlLogEntry
//
void
EwlOutputType4 (
  PSYSHOST          host,
  UINT8             warningCode,
  UINT8             minorWarningCode,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             rank,
  EWL_SEVERITY      severity,
  ADV_MT_TYPE       memtestType,
  EWL_ADV_MT_STATUS advMemtestErrorInfo,
  UINT32            count
  )
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE4 type4;
  UINT8 socketId = host->var.common.socketId;
  
  PlatformOutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);//OEM specific

  type4.Header.Size              = sizeof(EWL_ENTRY_TYPE4);
  type4.Header.Type              = EwlType4;
  type4.Header.Severity          = severity;

  type4.Context.MajorCheckpoint  = ((host->var.mem.lastCheckpoint[socketId]) >> 24) & 0xFF;//shift to get Major Checkpoint
  type4.Context.MinorCheckpoint  = ((host->var.mem.lastCheckpoint[socketId]) >> 16) & 0xFF;//shift to get Major Checkpoint
  type4.Context.MajorWarningCode = warningCode;
  type4.Context.MinorWarningCode = minorWarningCode;

  type4.MemoryLocation.Socket    = socket;
  type4.MemoryLocation.Channel   = ch;
  type4.MemoryLocation.Dimm      = dimm;
  type4.MemoryLocation.Rank      = rank;

  type4.MemtestType              = memtestType;
  type4.AdvMemtestErrorInfo      = advMemtestErrorInfo;
  type4.Count                    = count;

  //
  // Output warning details to the Serial Log
  //
  EwlPrintWarning (host, &type4.Header);

  //
  // Log the warning
  //
  PlatformEwlLogEntry (host, &type4.Header);
#endif
}

//
// Constructs type 5 warning according to EWL spec and calls PlatformEwlLogEntry
//
void
EwlOutputType5 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  UINT8        subRank,
  UINT8        bankAddress,
  UINT8*       dqBytes
  )
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE5   type5;
  UINT8 socketId = host->var.common.socketId;

  PlatformOutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);//OEM specific

  type5.Header.Size                 = sizeof(EWL_ENTRY_TYPE5);
  type5.Header.Type                 = EwlType5;
  type5.Header.Severity             = severity;

  type5.Context.MajorCheckpoint     = ((host->var.mem.lastCheckpoint[socketId]) >> 24) & 0xFF;//shift to get Major Checkpoint;
  type5.Context.MinorCheckpoint     = ((host->var.mem.lastCheckpoint[socketId]) >> 16) & 0xFF;//shift to get Minor
  type5.Context.MajorWarningCode    = warningCode;
  type5.Context.MinorWarningCode    = minorWarningCode;

  type5.MemoryLocation.Socket       = socket;
  type5.MemoryLocation.Channel      = ch;
  type5.MemoryLocation.Dimm         = dimm;
  type5.MemoryLocation.Rank         = rank;

  type5.SubRank                     = subRank;
  type5.BankAddress                 = bankAddress;

  MemCopy (type5.DqBytes, dqBytes, 9); // 9 bytes = 72 dq bits
  
  //
  // Output warning details to the Serial Log
  //
  EwlPrintWarning (host, &type5.Header);
 
  //
  // Log the warning
  //
  PlatformEwlLogEntry (host, &type5.Header);

#else
  OutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);
#endif// ENHANCED_WARNING_LOG
}

//
// Constructs type 7 warning according to EWL spec and calls PlatformEwlLogEntry
//
void
EwlOutputType7 (
  PSYSHOST     host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        Socket,
  UINT32       BistMask
)
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE7   type7;
  UINT8             CpuIndex;
  UINT8             socketId = host->var.common.socketId;

  PlatformOutputWarning (host, warningCode, minorWarningCode, Socket, 0xFF, 0xFF, 0xFF);//OEM specific

  type7.Header.Size                 = sizeof(EWL_ENTRY_TYPE7);
  type7.Header.Type                 = EwlType7;
  type7.Header.Severity             = EwlSeverityWarning;

  type7.Context.MajorCheckpoint     = ((host->var.mem.lastCheckpoint[socketId]) >> 24) & 0xFF;//shift to get Major Checkpoint;
  type7.Context.MinorCheckpoint     = ((host->var.mem.lastCheckpoint[socketId]) >> 16) & 0xFF;//shift to get Minor
  type7.Context.MajorWarningCode    = warningCode;
  type7.Context.MinorWarningCode    = minorWarningCode;

  type7.Socket       = Socket;

  //
  // Find the BIST failing cores - each core failure will produce log entry
  //
  for(CpuIndex = 0; CpuIndex < MAX_CORE; CpuIndex++){
    if ((BistMask >> CpuIndex) & 1){
      type7.Core = CpuIndex;
      //
      // Output warning details to the Serial Log
      //
      EwlPrintWarning (host, &type7.Header);

      //
      // Log the warning
      //
      PlatformEwlLogEntry (host, &type7.Header);
    }
  }
#endif// ENHANCED_WARNING_LOG
}

//
// Constructs type 10 warning according to EWL spec and calls PlatformEwlLogEntry
//
void
EwlOutputType10 (
  PSYSHOST host,
  UINT8        warningCode,
  UINT8        minorWarningCode,
  UINT8        socket,
  UINT8        ch,
  UINT8        dimm,
  UINT8        rank,
  EWL_SEVERITY severity,
  UINT8        swizzlePattern,
  UINT8        swizzledDqLanes,
  UINT8        lanesPerStrobe,
  UINT8        strobe
  )
{
#if ENHANCED_WARNING_LOG
  EWL_ENTRY_TYPE10  type10;
  UINT8 socketId = host->var.common.socketId;

  PlatformOutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);

  type10.Header.Size                = sizeof(EWL_ENTRY_TYPE10);
  type10.Header.Type                = EwlType10;
  type10.Header.Severity            = severity;

  type10.Context.MajorCheckpoint    = ((host->var.mem.lastCheckpoint[socketId]) >> 24) & 0x000000FF; // shift to get Major Checkpoint;
  type10.Context.MinorCheckpoint    = ((host->var.mem.lastCheckpoint[socketId]) >> 16) & 0x000000FF; // shift to get Minor
  type10.Context.MajorWarningCode   = warningCode;
  type10.Context.MinorWarningCode   = minorWarningCode;

  type10.MemoryLocation.Socket      = socket;
  type10.MemoryLocation.Channel     = ch;
  type10.MemoryLocation.Dimm        = dimm;
  type10.MemoryLocation.Rank        = rank;

  type10.SwizzlePattern             = swizzlePattern;
  type10.SwizzledDqLanes            = swizzledDqLanes;
  type10.LanesPerStrobe             = lanesPerStrobe;
  type10.Strobe                     = strobe;

  //
  // Output warning details to the Serial Log
  //
  EwlPrintWarning (host, &type10.Header);

  //
  // Log the warning
  //
  PlatformEwlLogEntry (host, &type10.Header);

#else
  OutputWarning (host, warningCode, minorWarningCode, socket, ch, dimm, rank);
#endif// ENHANCED_WARNING_LOG
}