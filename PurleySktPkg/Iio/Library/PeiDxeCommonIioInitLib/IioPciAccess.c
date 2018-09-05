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

  IioPciAccess.c

Abstract:

  Implement IIO initialization protocol to provide IIO initialization functions.
  This interfaces will be called by PciHostBridge driver in the PCI bus enumeration.

--*/
#include <IioPlatformData.h>
#include <PciAccess.h>
#include <IioMisc.h>
#include <SysFunc.h>
#include <ChipApi.h>
#ifndef IA32
#include <Library/DebugLib.h>
#include <Library/S3BootScriptLib.h>
#endif // IA32

/*++

Routine Description:
  Update 32-bits PCIe address memory with new Data

Arguments:
  IioGlobalData      - Pointer to IIO_GLOBALS
  Address            - Address that needs to be updated
  Data               - Data value

Returns:
  None

--*/
VOID
IioPciExpressWrite32(
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset,
  IN UINT32            Data
  )
{
  USRA_ADDRESS                Address;
  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth32, IioIndex, \
         Bus, Device, Function, Offset);
  Address.Attribute.S3Enable = USRA_ENABLE;
  RegisterWrite(&Address, &Data);
#ifndef IA32
#endif //IA32
}

/*++

Routine Description:
  Update 16-bits PCIe address memory with new Data

Arguments:
  IioGlobalData      - Pointer to IIO_GLOBALS
  Address            - Address that needs to be updated
  Data               - Data value

Returns:
  None

--*/
VOID
IioPciExpressWrite16(
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset,
  IN UINT16            Data
  )
{
  USRA_ADDRESS                Address;
  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth16, IioIndex, \
            Bus, Device, Function, Offset);
  Address.Attribute.S3Enable = USRA_ENABLE;
  RegisterWrite(&Address, &Data);
#ifndef IA32
#endif //IA32
}

/*++

Routine Description:
  Update 8-bits PCIe address memory with new Data

Arguments:
  IioGlobalData      - Pointer to IIO_GLOBALS
  Address            - Address that needs to be updated
  Data               - Data value

Returns:
  None

--*/
VOID
IioPciExpressWrite8(
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset,
  IN UINT8             Data
  )
{
  USRA_ADDRESS                Address;

  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth8, IioIndex, \
            Bus, Device, Function, Offset);
  Address.Attribute.S3Enable = USRA_ENABLE;
  RegisterWrite(&Address, &Data);
#ifndef IA32
#endif //IA32
}

/*++

Routine Description:
  Reads 32-bits PCIe address memory

Arguments:
  IioGlobalData      - Pointer to IIO_GLOBALS
  Address            - Address that needs to be updated

Returns:
  Data               - Data value

--*/
UINT32
IioPciExpressRead32(
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset
  )
{
  UINT32 Data;
  USRA_ADDRESS                Address;
  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth32, IioIndex, \
            Bus, Device, Function, Offset);
  RegisterRead(&Address, &Data);
  return Data;
}

/*++

Routine Description:
  Reads 16-bits PCIe address memory

Arguments:
  IioGlobalData      - Pointer to IIO_GLOBALS
  Address            - Address that needs to be updated

Returns:
  Data               - Data value

--*/
UINT16
IioPciExpressRead16(
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset
  )
{
  UINT16 Data;
  USRA_ADDRESS                Address;
  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth16, IioIndex, \
            Bus, Device, Function, Offset);
  RegisterRead(&Address, &Data);
  return Data;
}

/*++

Routine Description:
  Reads 8-bits PCIe address memory

Arguments:
  IioGlobalData      - Pointer to IIO_GLOBALS
  Address            - Address that needs to be updated

Returns:
  Data               - Data value

--*/
UINT8
IioPciExpressRead8(
  IN IIO_GLOBALS       *IioGlobalData,
  IN UINT8             IioIndex,
  IN UINT16            Bus,
  IN UINT16            Device,
  IN UINT8             Function,
  IN UINT32            Offset
  )
{
  UINT8 Data;
  USRA_ADDRESS                Address;

  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth8, IioIndex, \
            Bus, Device, Function, Offset);
  RegisterRead(&Address, &Data);
  return Data;
}

/**

  Funtion to write CPU CSR with 32-bit data in IIO module.

  @param IIO_GLOBALS -  IioGlobalData structure with CPU CSR access routine pointers
  @param BoxInst     -  Instance in Box type to be accessed
  @param Csr         -  CPU CSR offset to be access
  @param Data        -  Input data to be written to CPU CSR

  @retval VOID

**/
VOID 
IioWriteCpuCsr32 (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr,
  IN  UINT32            Data
  )
{
#ifdef IA32
  WriteCpuPciCfgEx ((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, IioIndex, BoxInst, Csr, Data);
#else
  UINT64    RegAddr;
  UINT8     Size;
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  CpuCsrAccess->WriteCpuCsr(IioIndex, BoxInst, Csr, Data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress (IioIndex, BoxInst, Csr, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );
#endif // IA32
}

// TBD, the IioWriteCpuCsr16/IioWriteCpuCsr8 routine may not be needed since CSR should indicate size itself.
/**

  Function to write CPU CSR with 16-bit data in IIO module.

  @param IIO_GLOBALS -  IioGlobalData structure with CPU CSR access routine pointers
  @param BoxInst     -  Instance in Box type to be accessed
  @param Csr         -  CPU CSR offset to be access
  @param Data        -  Input data to be written to CPU CSR

  @retval VOID

**/
VOID 
IioWriteCpuCsr16 (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr,
  IN  UINT16            Data
  )
{
#ifdef IA32
  WriteCpuPciCfgEx ((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, IioIndex, BoxInst, Csr, Data);
#else

  UINT64    RegAddr;
  UINT8     Size;
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  CpuCsrAccess->WriteCpuCsr(IioIndex, BoxInst, Csr, Data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress (IioIndex, BoxInst, Csr, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint16, RegAddr, 1, &Data );
#endif // IA32
}

/**

  Function to write CPU CSR with 8-bit data in IIO module.

  @param IIO_GLOBALS -  IioGlobalData structure with CPU CSR access routine pointers
  @param BoxInst     -  Instance in Box type to be accessed
  @param Csr         -  CPU CSR offset to be access
  @param Data        -  Input data to be written to CPU CSR

  @retval VOID

**/
VOID 
IioWriteCpuCsr8 (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr,
  IN  UINT8             Data
  )
{
#ifdef IA32
  WriteCpuPciCfgEx ((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, IioIndex, BoxInst, Csr, Data);
#else
  UINT64    RegAddr;
  UINT8     Size;
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  CpuCsrAccess->WriteCpuCsr(IioIndex, BoxInst, Csr, Data);
  RegAddr = CpuCsrAccess->GetCpuCsrAddress (IioIndex, BoxInst, Csr, &Size);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint8, RegAddr, 1, &Data );
#endif //IA32
}

/**

  Funtion to write CPU CSR with 32-bit data in IIO module.

  @param IIO_GLOBALS -  IioGlobalData structure with CPU CSR access routine pointers
  @param BoxInst     -  Instance in Box type to be accessed
  @param Csr         -  CPU CSR offset to be access
  @param Data        -  Input data to be written to CPU CSR

  @retval VOID

**/
UINT32
IioReadCpuCsr32 (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr
  )
{
  UINT32            Data;
#ifdef IA32
  Data = ReadCpuPciCfgEx((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, IioIndex, BoxInst, Csr);
#else
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  Data = CpuCsrAccess->ReadCpuCsr(IioIndex, BoxInst, Csr);
#endif
 return Data;
}

/**

  Function to write CPU CSR with 16-bit data in IIO module.

  @param IIO_GLOBALS -  IioGlobalData structure with CPU CSR access routine pointers
  @param BoxInst     -  Instance in Box type to be accessed
  @param Csr         -  CPU CSR offset to be access
  @param Data        -  Input data to be written to CPU CSR

  @retval VOID

**/
UINT16
IioReadCpuCsr16 ( 
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr
  )
{
  UINT16 Data;
#ifdef IA32

  Data = (UINT16)ReadCpuPciCfgEx((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, IioIndex, BoxInst, Csr);
#else
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  Data = (UINT16)CpuCsrAccess->ReadCpuCsr(IioIndex, BoxInst, Csr);
#endif
  return Data;
}

/**

  Function to write CPU CSR with 8-bit data in IIO module.

  @param IIO_GLOBALS -  IioGlobalData structure with CPU CSR access routine pointers
  @param BoxInst     -  Instance in Box type to be accessed
  @param Csr         -  CPU CSR offset to be access
  @param Data        -  Input data to be written to CPU CSR

  @retval VOID

**/
UINT8
IioReadCpuCsr8 (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             BoxInst,
  IN  UINT32            Csr
  )
{
  UINT8 Data;
#ifdef IA32
  Data = (UINT8)ReadCpuPciCfgEx((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, IioIndex, BoxInst, Csr);
#else
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  Data = (UINT8)CpuCsrAccess->ReadCpuCsr(IioIndex, BoxInst, Csr);
#endif
 return Data;
}

UINT32
IioBios2PcodeMailBoxWrite(
  IIO_GLOBALS *IioGlobalData,
  UINT8 Iio,
  UINT32 MailBoxCommand,
  UINT32 MailboxData
){
#ifdef IA32
  MailboxData = WriteBios2PcuMailboxCommand((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, Iio, MailBoxCommand, MailboxData);
#else
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  MailboxData = CpuCsrAccess->Bios2PcodeMailBoxWrite(Iio, MailBoxCommand, MailboxData);
#endif
 return MailboxData;
}




/**

  Function to halt the system if the CheckPoint is enabled 

  @param IIO_GLOBALS   -  IioGlobalData structure with CPU CSR access routine pointers
  @param MajorCode     -  Major code ID of the BreakPoint
  @param MinorCode     -  Minor code ID of the BreakPoint
  @param Data          -  Input data to be written to CPU CSR

  @retval VOID

**/
VOID
IioBreakAtCheckPoint (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8    MajorCode,
  IN  UINT8    MinorCode,
  IN  UINT16   Data
  )
{
#ifdef IA32
  OutputCheckpoint((PSYSHOST)IioGlobalData->IioVar.IioOutData.PtrAddress.Address32bit.Value, MajorCode, MinorCode, Data);
#else
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;
  CpuCsrAccess->BreakAtCheckpoint (MajorCode, MinorCode, Data);
#endif
}

