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

  IioIoAccess.c

Abstract:

  Implement IO access for IioEarLyInitLib

--*/
#include <SysFunc.h>
#include <IoAccess.h>
#ifndef IA32
#include <Library/IoLib.h>
#endif //IA32

/**

  Writes 32-bits IO address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param Data               - New Data

  @retval None

--*/
VOID
IioIoWrite32 (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT16        Address,
  IN  UINT32        Data
  )
{
#ifdef IA32
  OutPort32((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Address, Data);
#else
  IoWrite32(Address, Data);
#endif //IA32
}

/**

  Writes 16-bits IO address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param Data               - New Data

  @retval None

--*/
VOID
IioIoWrite16 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT16      Address,
  IN  UINT16      Data
  )
{
#ifdef IA32
  OutPort16((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Address, Data);
#else
  IoWrite16(Address, Data);
#endif
}

/**

  Writes 8-bits IO address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated
  @param Data               - New Data

  @retval None

--*/
VOID
IioIoWrite8 (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT16       Address,
  IN  UINT8        Data
  )
{
#ifdef IA32
  OutPort8((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Address, Data);
#else
  IoWrite8(Address, Data);
#endif //IA32
}

/**

  Read 32-bits IO address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated


  @retval Data               - Data Read

--*/
UINT32
IioIoRead32 (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT16        Address
  )
{
  UINT32 Data;

#ifdef IA32
  Data = InPort32((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Address);
#else
  Data = IoRead32(Address);
#endif //IA32

  return Data;
}

/**

  Reads 16-bits IO address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated


  @retval Data               - Data Read

--*/
UINT16
IioIoRead16 (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT16         Address
  )
{
  UINT16 Data;

#ifdef IA32
  Data = InPort16((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Address);
#else
  Data = IoRead16(Address);
#endif //IA32

  return Data;
}

/**

  Reads 8-bits IO address memory

  @param IioGlobalData      - Pointer to IIO_GLOBALS
  @param Address            - Address that needs to be updated


  @retval Data               - Data Read

--*/
UINT8
IioIoRead8 (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT16        Address
  )
{
  UINT8 Data;

#ifdef IA32
  Data = InPort8((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Address);
#else
  Data = IoRead8(Address);
#endif //IA32

  return Data;
}
