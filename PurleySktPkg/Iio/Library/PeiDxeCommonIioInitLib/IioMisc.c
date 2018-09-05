/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2016   Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IioMisc.c

Abstract:

  Implement IIO Misc functions.

--*/
#include <SysFunc.h>
#include <IoAccess.h>
#include <IioMisc.h>



/**

  Delay time in 1 us units

  @param usDelay - number of us units to delay

  @retval VOID

**/
VOID
IioStall (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32       UsDelay
  )
{
#ifdef IA32
  FixedDelay((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, UsDelay);
#else
  UINT32  StartCount, StopCount, Delay;

  StartCount = IioIoRead32 (IioGlobalData, (UINT16)(IioGlobalData->IioVar.IioVData.PmBase + 0x8)) & 0xFFFFFF;

  while(1) {
    StopCount = IioIoRead32 (IioGlobalData, (UINT16)(IioGlobalData->IioVar.IioVData.PmBase + 0x8)) & 0xFFFFFF;
    if (StartCount > StopCount) {
      StartCount |= 0xFF000000;
    }
    Delay = (StopCount - StartCount) * 1000U / 3579U;
    if (Delay > UsDelay) {
      break;
    }
  }
#endif // IA32
}

/**

  Generates a warm reset

  @param IioGlobalData - Global data structure

  @retval None

--*/
VOID
IioWarmReset (
  IN IIO_GLOBALS *IioGlobalData
  )
{
  IioIoWrite8 (IioGlobalData, 0xcf9, 6);
  IioCpuDeadLoop();
}

/**

  Enters in a Infinite loop to stop BIOS flow

  @param None

  @retval None

--*/
VOID
IioCpuDeadLoop (
 )
{
  volatile UINT32  Index;

  for (Index = 0; Index != 0xFF; Index++ ) {
    if (Index >= 0xFE) {
      Index = 0;
    }
  }
}


/**

  Copy memory from source to destination parameters

  @param *dest              - Destination Address
  @param *src               - Source Address

  @retval None

--*/
VOID
IioCopyMem (
  IN      VOID *dest,
  IN OUT  VOID *src,
  IN UINT32 size
  )
{

#ifdef IA32
  MemCopy((UINT8 *)dest, (UINT8 *)src, size);
#else
  CopyMem((VOID*)dest, (VOID *)src, size);
#endif

}

/**

  Routine to assert
  
  @param None
  
  @retval None

--*/
VOID
IioAssert (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT32          Data
  )
{
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "\n Reading/Writing: %x ", Data);
  IioCpuDeadLoop(); 
}

#ifdef IA32
/**

  Specific print to serial output

  @param host      - Pointer to the system host (root) structure
  @param DbgLevel  - specific debug level
  @param Format    - string format

  @retval None

--*/
VOID
IioDebugPrint (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT32 DbgLevel,
  IN  char* Format,
  ...
  )
{
#ifdef SERIAL_DBG_MSG
  va_list  Marker;
  va_start (Marker, Format);
  rcVprintf ((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Format, Marker);
  va_end (Marker);
#endif
}
#endif // IA32
