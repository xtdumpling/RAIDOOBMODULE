/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2016 - 2017 Intel Corporation.  All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaDxe.c

Abstract:

  This driver patches PCIe and ACPI for FPGA device

--*/

#include "FpgaDxe.h"
#include "FpgaHssi.h"

//
// Modular variables needed by this driver
//
IIO_GLOBALS                   *IioGlobalData;
EFI_IIO_UDS_PROTOCOL          *mIioUds;
BIOS_ACPI_PARAM               *GlobalNvsArea;
FPGA_CONFIGURATION            FpgaConfiguration;
EFI_MP_SERVICES_PROTOCOL      *mMpService;
UINTN                         FmeBar[MAX_SOCKET];


/**
  Check whether the bar is existed or not.

  @param[in]  FpgaId            The FPGA device register ID.
  @param[in]  Offset            The offset.
  @param[out] BarLengthValue    The bar length value returned.
  @param[out] OriginalBarValue  The original bar value returned.

  @retval EFI_NOT_FOUND         The bar doesn't exist.
  @retval EFI_SUCCESS           The bar exist.

**/
EFI_STATUS
FpgaBarExisted (
  IN UINT32          FpgaId,
  IN UINT32          Offset,
  OUT UINT32        *BarLengthValue,
  OUT UINT32        *OriginalBarValue
  )
{
  UINT8               Socket;
  UINT8               BusNum;
  UINT8               DevNum;
  UINT8               FunNum;
  UINT32              OriginalValue;
  UINT32              Value;

  Socket = EFI_PCI_SOCKET_OF_FPGAID (FpgaId);
  BusNum = EFI_PCI_BUS_OF_FPGAID (FpgaId);
  DevNum = EFI_PCI_DEV_OF_FPGAID (FpgaId);
  FunNum = EFI_PCI_FUN_OF_FPGAID (FpgaId);

  //
  // Preserve the original value
  //
  OriginalValue = IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset);

  IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset, 0xFFFFFFFF);
  Value = IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset);

  //
  // Write back the original value
  //
  IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset, OriginalValue);

  if (BarLengthValue != NULL) {
    *BarLengthValue = Value;
  }

  if (OriginalBarValue != NULL) {
    *OriginalBarValue = OriginalValue;
  }

  if (Value == 0) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Locate PciExpress capability register block per capability ID.

  @param FpgaId            The FPGA device register ID.
  @param CapId             The capability ID.
  @param Offset            A pointer to the offset returned.
  @param NextRegBlock      A pointer to the next block returned.

  @retval EFI_SUCCESS      Successfuly located capability register block.
  @retval EFI_UNSUPPORTED  Pci device does not support capability.
  @retval EFI_NOT_FOUND    Pci device support but can not find register block.

**/
EFI_STATUS
FpgaLocatePciExpressCapabilityRegBlock (
  IN     UINT32              FpgaId,
  IN     UINT16              CapId,
  IN OUT UINT32              *Offset,
     OUT UINT32              *NextRegBlock OPTIONAL
  )
{
  UINT8                Socket;
  UINT8                BusNum;
  UINT8                DevNum;
  UINT8                FunNum;
  UINT32               CapabilityPtr;
  UINT32               CapabilityEntry;
  UINT16               CapabilityID;

  Socket = EFI_PCI_SOCKET_OF_FPGAID (FpgaId);
  BusNum = EFI_PCI_BUS_OF_FPGAID (FpgaId);
  DevNum = EFI_PCI_DEV_OF_FPGAID (FpgaId);
  FunNum = EFI_PCI_FUN_OF_FPGAID (FpgaId);

  //
  // Assume the capability of FPGA device supports
  //
  if (*Offset != 0) {
    CapabilityPtr = *Offset;
  } else {
    CapabilityPtr = EFI_PCIE_CAPABILITY_BASE_OFFSET;
  }

  while (CapabilityPtr != 0) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFFC;
    CapabilityEntry = IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, CapabilityPtr);
    DEBUG ((DEBUG_ERROR, "CapabilityPtr: 0x%x, CapabilityEntry: %X\n", CapabilityPtr, CapabilityEntry));

    CapabilityID = (UINT16) CapabilityEntry;

    if (CapabilityID == CapId) {
      *Offset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (CapabilityEntry >> 20) & 0xFFF;
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (CapabilityEntry >> 20) & 0xFFF;
  }

  return EFI_NOT_FOUND;
}


/**
  Update FPGA resource in root port to enable the VTD resource decode.
  Vtd Bar is at PCI offset 0x180 of FPGA PCIE0 device, this Bar is not a standard Bar and is not scan during PCI scan.
  Need expand root port resouce to enable the VTD resouce decode.

  @param[in] PciIo        The PCI IO protocol instance.
  @param[in] BaseAddr     The base address of resource.

  @retval None
**/
VOID
UpdateFpgaBridgeResForVtd (
  IN EFI_PCI_IO_PROTOCOL     *PciIo,
  IN UINT32                  BaseAddr
  )
{
  UINT16                  TempData16;
  UINT32                  TempData32;

  if (!IioGlobalData->SetupData.VTdSupport) {
    return;
  }

  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x20, 1, &TempData32);

  if (TempData32 == 0xFFF0) {
    //
    // No resource , set the new resource for VTD
    //
    TempData32 = BaseAddr >> 16;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x22, 1, &TempData32);
    TempData32 = ((TempData32 >> 4) -1) << 4;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x20, 1, &TempData32);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x20, 1, &TempData32);
    DEBUG ((DEBUG_ERROR, "BaseLimit(0x20) for VTD = %lx\n", TempData32));
  } else {
    //
    // Have other resource , expand the  Memory_Base_Address which is 1M alinment in root port.
    //
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x20, 1, &TempData16);
    TempData16 = ((TempData16 >> 4) -1) << 4;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x20, 1, &TempData16);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x20, 1, &TempData32);
    DEBUG ((DEBUG_ERROR, "BaseLimit(0x20) for VTD = %lx\n", TempData32));
  }

}

/**
  Update FPGA device resource.

  @param[in] FpgaId       The FPGA device register ID.
  @param[in] BaseAddr     The base address of resource.

  @retval None
**/
VOID
UpdateFpgaDeviceRes (
  IN     UINT32              FpgaId,
  IN OUT UINT32              *BaseAddr
  )
{
  EFI_STATUS                    Status;
  UINT32                        Address32;
  UINT8                         Socket;
  UINT8                         BusNum;
  UINT8                         DevNum;
  UINT8                         FunNum;
  UINT16                        OldCommand;
  UINT16                        Command;
  UINT8                         Offset;
  UINT8                         BarIndex;
  UINT32                        Value;
  UINT32                        OriginalValue;
  UINT32                        Mask;
  UINT64                        Length;
  UINT64                        Alignment;

  Address32 = *BaseAddr;
  Socket = EFI_PCI_SOCKET_OF_FPGAID (FpgaId);
  BusNum = EFI_PCI_BUS_OF_FPGAID (FpgaId);
  DevNum = EFI_PCI_DEV_OF_FPGAID (FpgaId);
  FunNum = EFI_PCI_FUN_OF_FPGAID (FpgaId);

  OldCommand = IioPciExpressRead16 (IioGlobalData, Socket, BusNum, DevNum, FunNum, PCI_COMMAND_OFFSET);
  Command = (UINT16) (OldCommand & ~(EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_IO_SPACE));
  IioPciExpressWrite16 (IioGlobalData, Socket, BusNum, DevNum, FunNum, PCI_COMMAND_OFFSET, Command);

  //
  // Start to parse the bars
  //
  for (Offset = 0x10, BarIndex = 0; Offset <= 0x24 && BarIndex < PCI_MAX_BAR; BarIndex++) {
    Status = FpgaBarExisted (
               FpgaId,
               Offset,
               &Value,
               &OriginalValue
               );
    DEBUG ((DEBUG_ERROR, "OriginalValue = 0x%X, Value = 0x%X\n", OriginalValue, Value));
    if (EFI_ERROR (Status) || ((Value & 0x01) != 0x00)) {
      //
      // The bar doesn't exist or Device I/Os, skip it
      //
      Offset += 4;
      continue;
    }

    Mask = 0xfffffff0;
    switch (Value & 0x07) {
      //
      // memory space; anywhere in 32 bit address space (PciBarTypeMem32 / PciBarTypePMem32)
      //
      case 0x00:
        Length = (~(Value & Mask)) + 1;
        DEBUG ((DEBUG_ERROR, "Mem32 Length = 0x%X, Offset = 0x%X, Address32 = 0x%X.\n", Length, Offset, Address32));

        if (Length < (SIZE_4KB)) {
          Alignment = (SIZE_4KB - 1);
        } else {
          Alignment = Length - 1;
        }
        if ((Address32 & Alignment) != 0x00) {
          Address32 = (UINT32) (Address32 + Alignment + 1 - (Address32 & Alignment));
        }
        DEBUG ((DEBUG_ERROR, "Alignment = 0x%X, Address32 = 0x%X.\n", Alignment, Address32));

        IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset, Address32);
        IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset);
        DEBUG ((DEBUG_ERROR, "New Mem32 Data = 0x%X.\n\n", IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset)));
        Address32 += (UINT32) Length;
        Offset += 4;
        break;

      //
      // memory space; anywhere in 64 bit address space (PciBarTypeMem64 / PciBarTypePMem64)
      //
      case 0x04:
        Length = Value & Mask;
        //
        // Increment the offset to point to next DWORD
        //
        Offset += 4;

        Status = FpgaBarExisted (
                   FpgaId,
                   Offset,
                   &Value,
                   &OriginalValue
                   );
        if (EFI_ERROR (Status)) {
          //
          // the high 32 bit does not claim any BAR, we need to re-check the low 32 bit BAR again
          //
          if (Length == 0) {
            //
            // some device implement MMIO bar with 0 length, need to treat it as no-bar
            //
            Offset += 4;
            continue;
          }
        }

        //
        // Fix the length to support some spefic 64 bit BAR
        //
        if (Value == 0) {
          DEBUG ((DEBUG_ERROR, "BAR probing for upper 32bit of MEM64 BAR returns 0, change to 0xFFFFFFFF.\n"));
          Value = (UINT32) -1;
        } else {
          Value |= ((UINT32) (-1) << HighBitSet32 (Value));
        }

        //
        // Calculate the size of 64bit bar
        //
        Length    = Length | LShiftU64 ((UINT64) Value, 32);
        Length    = (~(Length)) + 1;
        DEBUG ((DEBUG_ERROR, "Mem64 Length = 0x%X, Offset = 0x%X, Address32 = 0x%X.\n", Length, Offset, Address32));
        if (Length < (SIZE_4KB)) {
          Alignment = (SIZE_4KB - 1);
        } else {
          Alignment = Length - 1;
        }
        if ((Address32 & Alignment) != 0x00) {
          Address32 = (UINT32) (Address32 + Alignment + 1 - (Address32 & Alignment));
        }
        DEBUG ((DEBUG_ERROR, "Alignment = 0x%X, Address32 = 0x%X.\n", Alignment, Address32));

        IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset - 4, Address32);
        IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset, 0x0);
        DEBUG ((DEBUG_ERROR, "New Mme64 Data = 0x%X.\n\n", IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset - 4)));
        Address32 += (UINT32) Length;
        Offset += 4;
        break;

      //
      // reserved
      //
      default:
        Offset += 4;
        break;
    }
  }
  //
  // Enable command register
  //
  IioPciExpressWrite16 (IioGlobalData, Socket, BusNum, DevNum, FunNum, PCI_COMMAND_OFFSET, (OldCommand |EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));
  *BaseAddr = Address32;
}

/**
  Parse PCI IOV VF bar information and update them into PCI device instance.

  @param FpgaId       The FPGA device register ID.
  @param BaseAddr     The base address of resource.

  @return None

**/
VOID
UpdateFpgaVfBar (
  IN     UINT32              FpgaId,
  IN OUT UINT32              *BaseAddr
  )
{
  EFI_STATUS                    Status;
  UINT32                        Address32;
  UINT8                         Socket;
  UINT8                         BusNum;
  UINT8                         DevNum;
  UINT8                         FunNum;
  UINT32                        Offset;
  UINT8                         BarIndex;
  UINT32                        Value;
  UINT32                        OriginalValue;
  UINT32                        Mask;
  UINT64                        Length;
  UINT64                        Alignment;
  UINT32                        SrIovCapabilityOffset;
  UINT16                        InitialVFs;
  UINT32                        SystemPageSize;

  Address32 = *BaseAddr;
  Socket = EFI_PCI_SOCKET_OF_FPGAID (FpgaId);
  BusNum = EFI_PCI_BUS_OF_FPGAID (FpgaId);
  DevNum = EFI_PCI_DEV_OF_FPGAID (FpgaId);
  FunNum = EFI_PCI_FUN_OF_FPGAID (FpgaId);
  DEBUG ((DEBUG_ERROR, "Socket: %X, BusNum: %X, DevNum: %X, FunNum: %X.\n", Socket, BusNum, DevNum, FunNum));

  SrIovCapabilityOffset = 0;
  Status = FpgaLocatePciExpressCapabilityRegBlock (
             FpgaId,
             EFI_PCIE_CAPABILITY_ID_SRIOV,
             &SrIovCapabilityOffset,
             NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Can't locate capability register block.\n"));
    return;
  }
  //
  // Ensure it is called properly
  //
  ASSERT (SrIovCapabilityOffset != 0);
  if (SrIovCapabilityOffset == 0) {
    return;
  }

  OriginalValue  = 0;
  Value          = 0;
  InitialVFs     = IioPciExpressRead16 (IioGlobalData, Socket, BusNum, DevNum, FunNum, SrIovCapabilityOffset + EFI_PCIE_CAPABILITY_ID_SRIOV_INITIALVFS);
  SystemPageSize = IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, SrIovCapabilityOffset + EFI_PCIE_CAPABILITY_ID_SRIOV_SYSTEM_PAGE_SIZE);
  DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] InitialVFs = 0x%X, SystemPageSize = 0x%X\n", InitialVFs, SystemPageSize));

  for (Offset = SrIovCapabilityOffset + EFI_PCIE_CAPABILITY_ID_SRIOV_BAR0, BarIndex = 0;
       Offset <= SrIovCapabilityOffset + EFI_PCIE_CAPABILITY_ID_SRIOV_BAR5;
       BarIndex++) {
    ASSERT (BarIndex < PCI_MAX_BAR);
    Status = FpgaBarExisted (
               FpgaId,
               Offset,
               &Value,
               &OriginalValue
               );
    DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] OriginalValue = 0x%X, Value = 0x%X\n", OriginalValue, Value));
    if (EFI_ERROR (Status) || ((Value & 0x01) != 0)) {
      //
      // The bar doesn't exist; Device I/Os, impossible
      //
      Offset += 4;
      continue;
    } else {
      Mask  = 0xfffffff0;

      switch (Value & 0x07) {
        //
        // memory space; anywhere in 32 bit address space (PciBarTypeMem32 / PciBarTypePMem32)
        //
        case 0x00:
          Length    = (~(Value & Mask)) + 1;
          Alignment = Length - 1;
          DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] Mem32 Length = 0x%X, Offset = 0x%X, Address32 = 0x%X\n", Length, Offset, Address32));

          //
          // Adjust Length
          //
          Length = MultU64x32 (Length, InitialVFs);
          //
          // Adjust Alignment
          //
          if (Alignment < SystemPageSize - 1) {
            Alignment = SystemPageSize - 1;
          }

          if ((Address32 & Alignment) != 0x00) {
            Address32 = (UINT32) (Address32 + Alignment + 1 - (Address32 & Alignment));
          }
          DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] Length = 0x%X, Alignment = 0x%X, Address32 = 0x%X\n", Length, Alignment, Address32));

          IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset, Address32);
          IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset);
          DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] New Mem32 Data = 0x%X.\n\n", IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset)));
          Address32 += (UINT32) Length;
          Offset += 4;
          break;

        //
        // memory space; anywhere in 64 bit address space (PciBarTypeMem64 / PciBarTypePMem64)
        //
        case 0x04:
          //
          // According to PCI 2.2,if the bar indicates a memory 64 decoding, next bar
          // is regarded as an extension for the first bar. As a result
          // the sizing will be conducted on combined 64 bit value
          // Here just store the masked first 32bit value for future size
          // calculation
          //
          Length    = Value & Mask;
          Alignment = Length - 1;

          if (Alignment < SystemPageSize - 1) {
            Alignment = SystemPageSize - 1;
          }

          //
          // Increment the offset to point to next DWORD
          //
          Offset += 4;

          Status = FpgaBarExisted (
                     FpgaId,
                     Offset,
                     &Value,
                     &OriginalValue
                     );

          if (EFI_ERROR (Status)) {
            if (Length == 0) {
              Offset += 4;
              continue;
            }
          }

          //
          // Fix the length to support some spefic 64 bit BAR
          //
          Value |= ((UINT32) -1 << HighBitSet32 (Value));

          //
          // Calculate the size of 64bit bar
          //
          Length    = Length | LShiftU64 ((UINT64) Value, 32);
          Length    = (~(Length)) + 1;
          Alignment = Length - 1;
          DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] Mem64 Length = 0x%X, Offset = 0x%X, Address32 = 0x%X.\n", Length, Offset, Address32));

          //
          // Adjust Length
          //
          Length = MultU64x32 (Length, InitialVFs);
          //
          // Adjust Alignment
          //
          if (Alignment < SystemPageSize - 1) {
            Alignment = SystemPageSize - 1;
          }
          if ((Address32 & Alignment) != 0x00) {
            Address32 = (UINT32) (Address32 + Alignment + 1 - (Address32 & Alignment));
          }
          DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] Length = 0x%X, Alignment = 0x%X, Address32 = 0x%X.\n", Length, Alignment, Address32));

          IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset - 4, Address32);
          IioPciExpressWrite32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset, 0x0);
          DEBUG ((DEBUG_ERROR, "[UpdateFpgaVfBar] New Mme64 Data = 0x%X.\n\n", IioPciExpressRead32 (IioGlobalData, Socket, BusNum, DevNum, FunNum, Offset - 4)));
          Address32 += (UINT32) Length;
          Offset += 4;
          break;

        //
        // reserved
        //
        default:
          Offset += 4;
          break;
      }
    }
  }
  *BaseAddr = Address32;
}

/**
  Update FPGA resource.

  @param[in] PciIo        The PCI IO protocol instance.
  @param[in] Socket       The number of CPU socket.

  @retval None

**/
VOID
UpdateFpgaRes (
  IN EFI_PCI_IO_PROTOCOL     *PciIo,
  IN UINT8                   Socket
  )
{
  EFI_STATUS                        Status;
  UINTN                             Segment;
  UINTN                             BusNum;
  UINTN                             DeviceNum;
  UINTN                             FunctionNum;

  UINT8                             SecondaryBusNumber;

  UINT32                            FpgaPpbPciResMem32Base;
  UINT32                            FpgaPpbPciResMem32Limit;
  UINT32                            TempFpgaPpbPciResMem32Base;

  UINT32                            MemoryBase;
  UINT32                            MemoryLimit;
  UINT32                            MemoryBaseLimit;
  UINT32                            PrefetchableMemoryBase32;
  UINT32                            PrefetchableMemoryLimit32;
  UINT32                            PrefetchableMemoryBaseLimit32;
  UINT32                            PrefetchableMemoryBaseUpper32;
  UINT32                            PrefetchableMemoryLimitUpper32;

  UINT32                            Memory32Length;
  UINT32                            PrefetchableMemory32length;

  UINT16                            OldCommand;
  UINT16                            Command;

  UINT64                            Address;
  UINT32                            Address32;
  UINT32                            TempData32;

  Address                    = 0;
  Memory32Length             = 0;
  PrefetchableMemory32length = 0;
  TempFpgaPpbPciResMem32Base = 0;

  Status = PciIo->GetLocation (
                    PciIo,
                    &Segment,
                    &BusNum,
                    &DeviceNum,
                    &FunctionNum
                    );
  SecondaryBusNumber = IioPciExpressRead8 (IioGlobalData, Socket, (UINT8) BusNum, 0, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  DEBUG ((DEBUG_ERROR, "SecondaryBusNumber = %lx\n", SecondaryBusNumber));

  FpgaPpbPciResMem32Base = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[IIO_PSTACK3].PciResourceMem32Limit - FPGA_PREALLOCATE_MEM_SIZE + 1;
  FpgaPpbPciResMem32Limit = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[IIO_PSTACK3].PciResourceMem32Limit;

  DEBUG ((DEBUG_ERROR, "FpgaPpbPciResMem32Base = %lx\n", FpgaPpbPciResMem32Base));
  DEBUG ((DEBUG_ERROR, "FpgaPpbPciResMem32Limit = %lx\n", FpgaPpbPciResMem32Limit));

  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x20, 1, &MemoryBaseLimit);
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x24, 1, &PrefetchableMemoryBaseLimit32);
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x28, 1, &PrefetchableMemoryBaseUpper32);
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x2C, 1, &PrefetchableMemoryLimitUpper32);
  DEBUG ((DEBUG_ERROR, "MemoryBaseLimit = %lx\n", MemoryBaseLimit));
  DEBUG ((DEBUG_ERROR, "PrefetchableMemoryBaseLimit32 = %lx\n", PrefetchableMemoryBaseLimit32));
  DEBUG ((DEBUG_ERROR, "PrefetchableMemoryBaseUpper32 = %lx\n", PrefetchableMemoryBaseUpper32));
  DEBUG ((DEBUG_ERROR, "PrefetchableMemoryLimitUpper32 = %lx\n", PrefetchableMemoryLimitUpper32));

  MemoryBase = ((MemoryBaseLimit & 0x0000FFF0) >> 4 ) << 20;
  MemoryLimit = (MemoryBaseLimit & 0xFFF00000) | 0xFFFFF;
  PrefetchableMemoryBase32 = ((PrefetchableMemoryBaseLimit32 & 0x0000FFF0) >> 4 ) << 20;
  PrefetchableMemoryLimit32 = (PrefetchableMemoryBaseLimit32 & 0xFFF00000) | 0xFFFFF;

  if (MemoryLimit > MemoryBase) {
    Memory32Length = MemoryLimit - MemoryBase + 1;
  }
  if (PrefetchableMemoryLimit32 > PrefetchableMemoryBase32) {
    PrefetchableMemory32length = PrefetchableMemoryLimit32 - PrefetchableMemoryBase32 + 1;
  }
  DEBUG ((DEBUG_ERROR, "Memory32Length = %lx\n", Memory32Length));
  DEBUG ((DEBUG_ERROR, "PrefetchableMemory32length = %lx\n", PrefetchableMemory32length));

  //
  // Disable Command Register
  //
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_COMMAND_OFFSET, 1, &OldCommand);
  Command = (UINT16) (OldCommand & ~(EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_IO_SPACE));
  PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, PCI_COMMAND_OFFSET, 1, &Command);

  Address = (UINT64) FpgaPpbPciResMem32Base;
  //
  // Programming new PPB Memory Base (0x20) and Memory Limit (0x22) registers
  //
  if (Memory32Length != 0) {
    Address32 = ((UINT32) (Address)) >> 16;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x20, 1, &Address32);
    Address = (UINT64) (Address + Memory32Length - 1);
    Address32 = ((UINT32) (Address)) >> 16;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x22, 1, &Address32);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x20, 1, &TempData32);
    Address += 1;
    DEBUG ((DEBUG_ERROR, "New BaseLimit(0x20) = %lx\n", TempData32));
  }

  //
  // Programming new PPB Prefetchable Memory Base (0x24) and Prefetchable Memory Limit (0x28) registers
  //
  if (PrefetchableMemory32length != 0) {
    Address32 = ((UINT32) (Address)) >> 16;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x24, 1, &Address32);

    Address = (UINT32) (Address + PrefetchableMemory32length - 1);
    Address32 = ((UINT32) (Address)) >> 16;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x26, 1, &Address32);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x24, 1, &TempData32);
    DEBUG ((DEBUG_ERROR, "New PreBaseLimit(0x24) = %lx\n", TempData32));

    Address32 = 0;
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint32, 0x28, 1, &Address32);
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint32, 0x2C, 1, &Address32);
  }

  //
  // Enable VTD resource decode in root port.
  //
  UpdateFpgaBridgeResForVtd (PciIo, FpgaPpbPciResMem32Base);

  //
  // Set new PCI resource to FPGA device
  //
  TempFpgaPpbPciResMem32Base = FpgaPpbPciResMem32Base;
  UpdateFpgaDeviceRes (EFI_PCI_FPGAID (Socket, SecondaryBusNumber, 0, 0), &TempFpgaPpbPciResMem32Base);

  //
  // Update FPGA VF BAR address
  //
  UpdateFpgaVfBar (EFI_PCI_FPGAID (Socket, SecondaryBusNumber, 0, 0), &TempFpgaPpbPciResMem32Base);

  //
  // Enable Command Register
  //
  PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, PCI_COMMAND_OFFSET, 1, &OldCommand);

  //
  // Update FPGA ACPI NVS data
  //
  GlobalNvsArea->VFPBMemBase[Socket]  = FpgaPpbPciResMem32Base;
  GlobalNvsArea->VFPBMemLimit[Socket] = FpgaPpbPciResMem32Base + Memory32Length + PrefetchableMemory32length - 1;
}

/**
  Update virtual FPGA bus resource.

  @param[in] FpgaBus      The bus number of FPGA root port.
  @param[in] Socket       The number of CPU socket.

  @retval EFI_SUCCESS     The function is executed successfully.
  @retval other           Some error occurs when executing this entry point.
**/
EFI_STATUS
UpdateVFPBResources (
  IN UINTN                FpgaBus,
// APTIOV_SERVER_OVERRIDE_RC_START: Use Segment Number instead of Socket Number to confirm the FPGA device.
  IN UINT8                Socket,
  IN UINT8                FpgaSegment
// APTIOV_SERVER_OVERRIDE_RC_END: Use Segment Number instead of Socket Number to confirm the FPGA device.
  )
{
  EFI_STATUS              Status;
  UINTN                   HandleCount;
  EFI_HANDLE              *Handles;
  EFI_PCI_IO_PROTOCOL     *PciIo;
  UINTN                   Index;
  UINTN                   Segment;
  UINTN                   BusNum;
  UINTN                   DeviceNum;
  UINTN                   FunctionNum;
  UINTN                   FpgaBusNum;

  FpgaBusNum = FpgaBus;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = PciIo->GetLocation (
                      PciIo,
                      &Segment,
                      &BusNum,
                      &DeviceNum,
                      &FunctionNum
                      );
// APTIOV_SERVER_OVERRIDE_RC_START: Use Segment Number instead of Socket Number to confirm the FPGA device.
      if (FpgaSegment== Segment &&
// APTIOV_SERVER_OVERRIDE_RC_END: Use Segment Number instead of Socket Number to confirm the FPGA device.
        FpgaBusNum == BusNum &&
        0          == DeviceNum &&
        0          == FunctionNum) {
      DEBUG ((DEBUG_ERROR, "Update Device on B%x:D%x:F%x\n", BusNum, DeviceNum, FunctionNum));
      UpdateFpgaRes (PciIo, Socket);
      break;
    }
  }
  gBS->FreePool (Handles);

  if (Index == HandleCount) {
    DEBUG ((DEBUG_ERROR, "Cannot find virtual FPGA bus.\n"));
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}


/**
  Set FPGA thermal configuration.

  @param[in] VOID

  @retval None.
**/
VOID
EFIAPI
FpgaThermalConfig (
  VOID
  )
{
  UINT8                            Socket;
  TMP_THRESHOLD_N0_FPGA_FME_STRUCT TmpThreshold_N0;
  TMP_THRESHOLD_N1_FPGA_FME_STRUCT TmpThreshold_N1;

  //
  // Loop thru all FPGA of all sockets that are present
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << Socket))) {
      continue;
    }

    //
    // Set the Thermal Threshold for Fpga
    //
    TmpThreshold_N0.Data = MmioRead32 (FmeBar[Socket] + TMP_THRESHOLD_N0_FPGA_FME_REG);
    TmpThreshold_N1.Data = MmioRead32 (FmeBar[Socket] + TMP_THRESHOLD_N1_FPGA_FME_REG);

    TmpThreshold_N0.Bits.tempthreshold1     = 0;
    TmpThreshold_N0.Bits.tempthreshold1enab = 0;
    TmpThreshold_N0.Bits.tempthreshold2     = 0;
    TmpThreshold_N0.Bits.tempthreshold2enab = 0;
    TmpThreshold_N1.Bits.thresholdpolicy    = 0;

    if ((FpgaConfiguration.FpgaSktThermalTH1[Socket] != 0) && (FpgaConfiguration.FpgaSktThermalTH1[Socket] != 0xff)) {
      TmpThreshold_N0.Bits.tempthreshold1     = FpgaConfiguration.FpgaSktThermalTH1[Socket];
      TmpThreshold_N0.Bits.tempthreshold1enab = 1;
      TmpThreshold_N1.Bits.thresholdpolicy    = V_THRESHOLD_1_POLICY_50_P;
    }

    if ((FpgaConfiguration.FpgaSktThermalTH2[Socket] != 0) && (FpgaConfiguration.FpgaSktThermalTH2[Socket] != 0xff)) {
      TmpThreshold_N0.Bits.tempthreshold2     = FpgaConfiguration.FpgaSktThermalTH2[Socket];
      TmpThreshold_N0.Bits.tempthreshold2enab = 1;
    }

    DEBUG ((DEBUG_ERROR, "TmpThreshold_N0.Data = %X, TmpThreshold_N1.Data = %X\n", TmpThreshold_N0.Data, TmpThreshold_N1.Data));
    MmioWrite32 (FmeBar[Socket] + TMP_THRESHOLD_N0_FPGA_FME_REG, TmpThreshold_N0.Data);
    MmioWrite32 (FmeBar[Socket] + TMP_THRESHOLD_N1_FPGA_FME_REG, TmpThreshold_N1.Data);
  }
}

/**
  Program LLPR (Last Level Protection Registers) for FPGA

  @param[in] VOID

  @retval EFI_SUCCESS   Program FPGA LLPR completed successfully.
  @retval Others        Internal error when program FPGA LLPR

**/
EFI_STATUS
EFIAPI
FpgaLLPRProgram (
  VOID
  )
{
  EFI_STATUS                             Status;
  UINT8                                  Idx;
  UINT32                                 FpgaPresentBitMap;
  UINTN                                  ProcessorNumber;
  EFI_PROCESSOR_INFORMATION              ProcessorInfoBuffer;
  UINT8                                  SbspId;
  UINT64                                 SmrrBase;
  UINT64                                 SmrrMask;
  UINT64                                 Smrr2Base;
  UINT64                                 Smrr2Mask;
  MESEG_BASE_N0_CHABC_SAD_STRUCT         MeSegLowBaseAddr;
  MESEG_BASE_N1_CHABC_SAD_STRUCT         MeSegHighBaseAddr;
  MESEG_LIMIT_N0_CHABC_SAD_STRUCT        MeSegLowLimit;
  MESEG_LIMIT_N1_CHABC_SAD_STRUCT        MeSegHighLimit;
  LLPR_MESEG_BASE_FPGA_CCI_STRUCT        LlprMeSegBase;
  LLPR_MESEG_LIMIT_FPGA_CCI_STRUCT       LlprMeSegLimit;
  GENPROTRANGE2_BASE_N0_FPGA_FME_STRUCT  GenProtRange2_Base_N0;
  GENPROTRANGE2_BASE_N1_FPGA_FME_STRUCT  GenProtRange2_Base_N1;
  GENPROTRANGE2_LIMIT_N0_FPGA_FME_STRUCT GenProtRange2_Limit_N0;
  GENPROTRANGE2_LIMIT_N1_FPGA_FME_STRUCT GenProtRange2_Limit_N1;

  DEBUG ((DEBUG_ERROR, "FpgaLLPRProgram() ...\n"));

  Idx = 0;
  FpgaPresentBitMap = mIioUds->IioUdsPtr->SystemStatus.FpgaPresentBitMap;
  if (FpgaPresentBitMap == 0) {
    return EFI_SUCCESS;
  }
  DEBUG ((DEBUG_ERROR, "FpgaPresentBitMap = %X\n", FpgaPresentBitMap));

  Status = mMpService->WhoAmI (
                         mMpService,
                         &ProcessorNumber
                         );
  ASSERT_EFI_ERROR (Status);

  Status = mMpService->GetProcessorInfo (
                         mMpService,
                         ProcessorNumber,
                         &ProcessorInfoBuffer
                         );
  ASSERT_EFI_ERROR (Status);
  SbspId = (UINT8) ProcessorInfoBuffer.Location.Package;
  DEBUG ((DEBUG_ERROR, "SBSP ID = %X\n", SbspId));

  //
  // Read SMRR and SMRR2 range data from MSR registers
  //
  SmrrBase = AsmReadMsr64 (EFI_SMRR_PHYS_BASE);
  SmrrMask = AsmReadMsr64 (EFI_SMRR_PHYS_MASK);

  Smrr2Base = AsmReadMsr64 (EFI_MSR_SMRR2_PHYS_BASE);
  Smrr2Mask = AsmReadMsr64 (EFI_MSR_SMRR2_PHYS_MASK);
  DEBUG ((DEBUG_ERROR, "SmrrBase = %X, SmrrMask = %X\n", SmrrBase, SmrrMask));
  DEBUG ((DEBUG_ERROR, "Smrr2Base = %X, Smrr2Mask = %X\n", Smrr2Base, Smrr2Mask));

  //
  // Read MESEG Base/Limit address
  //
  MeSegLowBaseAddr.Data  = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, MESEG_BASE_N0_CHABC_SAD_REG);
  MeSegHighBaseAddr.Data = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, MESEG_BASE_N1_CHABC_SAD_REG);
  MeSegLowLimit.Data     = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, MESEG_LIMIT_N0_CHABC_SAD_REG);
  MeSegHighLimit.Data    = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, MESEG_LIMIT_N1_CHABC_SAD_REG);
  DEBUG ((DEBUG_ERROR, "MeSegLowBaseAddr.Data = %X, MeSegHighBaseAddr.Data = %X\n", MeSegLowBaseAddr.Data, MeSegHighBaseAddr.Data));
  DEBUG ((DEBUG_ERROR, "MeSegLowLimit.Data = %X, MeSegHighLimit.Data = %X\n", MeSegLowLimit.Data, MeSegHighLimit.Data));

  //
  // Read Generic Protected Memory Range 2 Base/Limit address
  //
  GenProtRange2_Base_N0.Data  = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, GENPROTRANGE2_BASE_N0_IIO_VTD_REG);
  GenProtRange2_Base_N1.Data  = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, GENPROTRANGE2_BASE_N1_IIO_VTD_REG);
  GenProtRange2_Limit_N0.Data = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, GENPROTRANGE2_LIMIT_N0_IIO_VTD_REG);
  GenProtRange2_Limit_N1.Data = IioReadCpuCsr32 (IioGlobalData, SbspId, 0, GENPROTRANGE2_LIMIT_N1_IIO_VTD_REG);
  GenProtRange2_Limit_N0.Bits.enable = 1;
  DEBUG ((DEBUG_ERROR, "GenProtRange2_Base_N0 = %X, GenProtRange2_Base_N1 = %X\n", GenProtRange2_Base_N0, GenProtRange2_Base_N1));
  DEBUG ((DEBUG_ERROR, "GenProtRange2_Limit_N0 = %X, GenProtRange2_Limit_N1 = %X\n", GenProtRange2_Limit_N0, GenProtRange2_Limit_N1));

  while (FpgaPresentBitMap) {
    if (FpgaPresentBitMap & BIT0) {
      //
      // Program SMRR and SMRR2 range registers in FPGA
      //
      if (SmrrMask & EFI_MSR_SMRR_PHYS_MASK_VALID) {
        IioWriteCpuCsr32 (IioGlobalData, Idx, 0, LLPR_SMRR_BASE_FPGA_CCI_REG, (UINT32) (SmrrBase & EFI_MSR_SMRR_MASK));
        IioWriteCpuCsr32 (IioGlobalData, Idx, 0, LLPR_SMRR_MASK_FPGA_CCI_REG, (UINT32) SmrrMask);
      }

      if (Smrr2Mask & EFI_MSR_SMRR_PHYS_MASK_VALID) {
        IioWriteCpuCsr32 (IioGlobalData, Idx, 0, LLPR_SMRR2_BASE_FPGA_CCI_REG, (UINT32) (Smrr2Base & EFI_MSR_SMRR_MASK));
        IioWriteCpuCsr32 (IioGlobalData, Idx, 0, LLPR_SMRR2_MASK_FPGA_CCI_REG, (UINT32) Smrr2Mask);
      }

      //
      // Program MESEG Base/Limit address in FPGA
      //
      if (MeSegLowLimit.Bits.en) {
        LlprMeSegBase.Data = 0;
        LlprMeSegBase.Bits.mebase = (MeSegHighBaseAddr.Bits.mebase << 13) | (MeSegLowBaseAddr.Bits.mebase);
        DEBUG ((DEBUG_ERROR, "LlprMeSegBase.Data = %X\n", LlprMeSegBase.Data));
        IioWriteCpuCsr32 (IioGlobalData, Idx, 0, LLPR_MESEG_BASE_FPGA_CCI_REG, LlprMeSegBase.Data);

        LlprMeSegLimit.Data = 0;
        LlprMeSegLimit.Bits.melimit = (MeSegHighLimit.Bits.melimit << 13) | (MeSegLowLimit.Bits.melimit);
        LlprMeSegLimit.Bits.enable = 1;
        DEBUG ((DEBUG_ERROR, "LlprMeSegLimit.Data = %X\n", LlprMeSegLimit.Data));
        IioWriteCpuCsr32 (IioGlobalData, Idx, 0, LLPR_MESEG_LIMIT_FPGA_CCI_REG, LlprMeSegBase.Data);
      }
    }
    FpgaPresentBitMap >>= 1;
    Idx++;
  }

  //
  // Program PAM general protection range to protect from inbound DMA access
  // Expectation is this region is used to protect PAM region access from 0xa_0000 to 0xf_0000
  //
  for (Idx = 0; Idx < MAX_SOCKET; Idx++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << Idx))) {
      continue;
    }

    //
    // Protected range base address
    //
    MmioWrite32 (FmeBar[Idx] + GENPROTRANGE2_BASE_N0_FPGA_FME_REG, GenProtRange2_Base_N0.Data);
    MmioWrite32 (FmeBar[Idx] + GENPROTRANGE2_BASE_N1_FPGA_FME_REG, GenProtRange2_Base_N1.Data);

    //
    // Protected range limit address
    //
    MmioWrite32 (FmeBar[Idx] + GENPROTRANGE2_LIMIT_N0_FPGA_FME_REG, GenProtRange2_Limit_N0.Data);
    MmioWrite32 (FmeBar[Idx] + GENPROTRANGE2_LIMIT_N1_FPGA_FME_REG, GenProtRange2_Limit_N1.Data);
  }

  return EFI_SUCCESS;
}

/**
  Configure FPGA PCIe0 device:
  1. Set it to Root Complex integrated Endpoint (RCiEP).
  2. Set the bit to lock VT-d BAR register.

  @param[in] VOID

  @retval EFI_SUCCESS   Set FPGA PCIe device configuration successfully.
  @retval Others        Internal error when set FPGA PCIe device configuration.

**/
EFI_STATUS
EFIAPI
FpgaPcieDeviceConfig (
  VOID
  )
{
  UINT8                                     SocketId;
  UINT32                                    PcodeMailboxStatus;
  BIOS_MAILBOX_INTERFACE_SMB_ACCESS_STRUCT  SmbAccessMailBoxInterface;
  BIOS_MAILBOX_DATA_SMB_ACCESS_STRUCT       SmbAccessMailBoxData;

  DEBUG ((DEBUG_ERROR, "FpgaPcieDeviceConfig() ...\n"));

  //
  // Loop thru all FPGA of all sockets that are present
  //
  for (SocketId = 0; SocketId < MAX_SOCKET; SocketId++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << SocketId))) {
      continue;
    }

    //
    // Step1: BIOS must request the semaphore prior to performing any CR reads or writes.
    //
    SmbAccessMailBoxInterface.Data = 0;
    SmbAccessMailBoxInterface.Bits.command = MAILBOX_BIOS_CMD_SMB_ACCESS;
    SmbAccessMailBoxInterface.Bits.semaphore = MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_REQUESET;
    SmbAccessMailBoxData.Data = 0;
    PcodeMailboxStatus = IioBios2PcodeMailBoxWrite (IioGlobalData, SocketId, SmbAccessMailBoxInterface.Data, SmbAccessMailBoxData.Data);
    if (PcodeMailboxStatus != MAILBOX_BIOS_ERROR_CODE_NO_ERROR) {
      DEBUG ((DEBUG_ERROR, "Can't grant the semaphore, status = %X.\n", PcodeMailboxStatus));
      return EFI_DEVICE_ERROR;
    }

    //
    // Step2: Set DATA register
    //
    SmbAccessMailBoxInterface.Data = 0;
    SmbAccessMailBoxInterface.Bits.command = MAILBOX_BIOS_CMD_SMB_ACCESS;
    SmbAccessMailBoxInterface.Bits.read_write = MAILBOX_BIOS_SMB_ACCESS_WRITE;
    SmbAccessMailBoxInterface.Bits.cr_index = MAILBOX_SMB2_DATA_INDEX;
    SmbAccessMailBoxInterface.Bits.semaphore = MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_NONE;
    SmbAccessMailBoxData.Data = 0;
    // [0]-PCIe 0 RCiEP: set the bit to configure PCIe0 as RCiEP; [1]-PCIe 0 VTd BAR lock: set the bit to lock VT-d BAR register
    SmbAccessMailBoxData.Bits.smb_wdata = (UINT16) (BIT1 | BIT0);
    PcodeMailboxStatus = IioBios2PcodeMailBoxWrite (IioGlobalData, SocketId, SmbAccessMailBoxInterface.Data, SmbAccessMailBoxData.Data);
    if (PcodeMailboxStatus != MAILBOX_BIOS_ERROR_CODE_NO_ERROR) {
      DEBUG ((DEBUG_ERROR, "Can't set DATA register, status = %X.\n", PcodeMailboxStatus));
    }

    //
    // Step3: Clear CMD_EXT register
    //
    SmbAccessMailBoxInterface.Data = 0;
    SmbAccessMailBoxInterface.Bits.command = MAILBOX_BIOS_CMD_SMB_ACCESS;
    SmbAccessMailBoxInterface.Bits.read_write = MAILBOX_BIOS_SMB_ACCESS_WRITE;
    SmbAccessMailBoxInterface.Bits.cr_index = MAILBOX_SMB2_CMD_EXT_INDEX;
    SmbAccessMailBoxInterface.Bits.semaphore = MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_NONE;
    SmbAccessMailBoxData.Data = 0;
    PcodeMailboxStatus = IioBios2PcodeMailBoxWrite (IioGlobalData, SocketId, SmbAccessMailBoxInterface.Data, SmbAccessMailBoxData.Data);
    if (PcodeMailboxStatus != MAILBOX_BIOS_ERROR_CODE_NO_ERROR) {
      DEBUG ((DEBUG_ERROR, "Can't clear CMD_EXT register, status = %X.\n", PcodeMailboxStatus));
    }

    //
    // Step4: Set CMD register to execute write 16-bit register at address YY
    //
    SmbAccessMailBoxInterface.Data = 0;
    SmbAccessMailBoxInterface.Bits.command = MAILBOX_BIOS_CMD_SMB_ACCESS;
    SmbAccessMailBoxInterface.Bits.read_write = MAILBOX_BIOS_SMB_ACCESS_WRITE;
    SmbAccessMailBoxInterface.Bits.cr_index = MAILBOX_SMB2_CMD_INDEX;
    SmbAccessMailBoxInterface.Bits.semaphore = MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_NONE;
    SmbAccessMailBoxData.Data = FPGA_SMB_ACCESS_PCIE_CONFIG_CMD;
    PcodeMailboxStatus = IioBios2PcodeMailBoxWrite (IioGlobalData, SocketId, SmbAccessMailBoxInterface.Data, SmbAccessMailBoxData.Data);
    if (PcodeMailboxStatus != MAILBOX_BIOS_ERROR_CODE_NO_ERROR) {
      DEBUG ((DEBUG_ERROR, "Can't set CMD register to execute write 16-bit register at address YY, status = %X.\n", PcodeMailboxStatus));
    }

    //
    // Step5: BIOS release SMB2 semaphore after execution
    //
    SmbAccessMailBoxInterface.Data = 0;
    SmbAccessMailBoxInterface.Bits.command = MAILBOX_BIOS_CMD_SMB_ACCESS;
    SmbAccessMailBoxInterface.Bits.semaphore = MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_RELEASE;
    SmbAccessMailBoxData.Data = 0;
    PcodeMailboxStatus = IioBios2PcodeMailBoxWrite (IioGlobalData, SocketId, SmbAccessMailBoxInterface.Data, SmbAccessMailBoxData.Data);
    if (PcodeMailboxStatus != MAILBOX_BIOS_ERROR_CODE_NO_ERROR) {
      DEBUG ((DEBUG_ERROR, "Can't release SMB2 semaphore, status = %X.\n", PcodeMailboxStatus));
      return EFI_DEVICE_ERROR;
    }
  }
  return EFI_SUCCESS;
}

/**
  FPGA VTD initialization on specific socket

  @param[in] None

  @retval EFI_SUCCESS      - retuen EFI success
          EFI_UNSUPPORTED  - return Not Supported

**/
EFI_STATUS
EFIAPI
FpgaLateVtdInitialization (
  VOID
  )
{
  IIO_PTR_ADDRESS            VtdMmioCap;
  IIO_PTR_ADDRESS            VtdMmioExtCap;
  UINTN                      VtdMemoryMap;
  UINT32                     VtdBase;

  UINT8                      FpgaStackBusNum;
  UINT8                      FpgaStackSecondaryBusNumber;
  UINT8                      Socket;

  if (!IioGlobalData->SetupData.VTdSupport) {
    return EFI_UNSUPPORTED;
  }

  //
  // Loop thru all FPGA of all sockets that are present
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << Socket))) {
      continue;
    }

    //
    // Initialize VT-d / IOMMU in FPGA
    //
    VtdBase = IioGlobalData->IioVar.IioVData.VtdBarAddress[Socket][IIO_PSTACK3];
    if (VtdBase) {
      DEBUG ((DEBUG_ERROR, "Skt[%X] FPGA VTD BASE = 0x%X\n", Socket, VtdBase));

      //
      // Get the RCiEP0 devcei 0xBCC0 Bus Number
      //
      FpgaStackBusNum           = IioGlobalData->IioVar.IioVData.SocketStackBus[Socket][IIO_PSTACK3];
      FpgaStackSecondaryBusNumber   =  IioPciExpressRead8 (IioGlobalData, Socket, FpgaStackBusNum, 0, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);

      //
      // Program the VTD BAR for FPGA
      //
      IioPciExpressWrite32 (IioGlobalData, Socket, FpgaStackSecondaryBusNumber, 0, 0, R_FPGA_VTD_BAR, (VtdBase | B_IIO_VT_BAR_EN));
      DEBUG ((DEBUG_ERROR, "Skt[%X] FPGA VTD BASE Programmed to 0x%X\n", Socket, IioPciExpressRead32 (IioGlobalData, Socket, FpgaStackSecondaryBusNumber, 0, 0, R_FPGA_VTD_BAR)));

      //
      // Non Isoc engine Capabilities and Ext capabilities
      //
      VtdMemoryMap      = (UINTN) VtdBase;
      VtdMmioCap.Address32bit.Value    = MmioRead32 (VtdMemoryMap + R_VTD_CAP_LOW);

      //
      // BIT23=0 for other than Isoch VT-d MMMIO.
      //
      VtdMmioCap.Address32bit.Value &= ~(UINT64) BIT23;

      //
      // ZLR support enable BIT22 = 1
      //
      VtdMmioCap.Address32bit.Value |= BIT22;

      MmioWrite32 (VtdMemoryMap + R_VTD_CAP_LOW, VtdMmioCap.Address32bit.Value);

      //
      // 4927064:Provide BIOS Setup enable/disable of capabilites bit to enable or disable posted interrupts
      // VTBar offset 0x08, vtd0_cap, bit 59: when set allows Posted Interrupts to be set up.
      //
      VtdMmioCap.Address32bit.ValueHigh = MmioRead32 (VtdMemoryMap + R_VTD_CAP_HIGH);
      VtdMmioCap.Address32bit.ValueHigh &= ~(UINT64) (BIT27); // BIT27 is the same as BIT59, since we are reading High DWORD of 64bit address
      if (IioGlobalData->SetupData.PostedInterrupt) {
      //  VtdMmioCap.Address32bit.ValueHigh   |= BIT27;
      }

      // comment the following because the register is Read-Only
      // Bit 59 is RW_O
      MmioWrite32 (VtdMemoryMap + R_VTD_CAP_HIGH, VtdMmioCap.Address32bit.ValueHigh);

      VtdMmioExtCap.Address32bit.Value = MmioRead32 (VtdMemoryMap + R_VTD_EXT_CAP_LOW);

      //
      // Set Coherency for Non-Isoch Vtd
      //
      VtdMmioExtCap.Address32bit.Value &= ~(BIT0);
      if (IioGlobalData->SetupData.CoherencySupport) {
        VtdMmioExtCap.Address32bit.Value   |= BIT0;
      }

      VtdMmioExtCap.Address32bit.Value   &= ~(BIT2);
      if (IioGlobalData->SetupData.ATS) {
       // VtdMmioExtCap.Address32bit.Value   |= BIT2;
      }

      VtdMmioExtCap.Address32bit.Value   &= ~(BIT6);
      if (IioGlobalData->SetupData.PassThroughDma) {
        VtdMmioExtCap.Address32bit.Value   |= BIT6;
      }

      VtdMmioExtCap.Address32bit.Value   &= ~(BIT3);
      if (IioGlobalData->SetupData.InterruptRemap) {
     //   VtdMmioExtCap.Address32bit.Value   |= BIT3;
      }

      MmioWrite32 (VtdMemoryMap + R_VTD_EXT_CAP_LOW, VtdMmioExtCap.Address32bit.Value);

      VtdMmioExtCap.Address32bit.ValueHigh = MmioRead32 (VtdMemoryMap + R_VTD_EXT_CAP_HIGH);

      DEBUG ((DEBUG_ERROR, "FPGA Non-Iso Engine CapReg     :  0x%X%X\n", VtdMmioCap.Address32bit.ValueHigh, VtdMmioCap.Address32bit.Value));
      DEBUG ((DEBUG_ERROR, "FPGA Non-Iso Engine ExtCapReg  :  0x%X%X\n", VtdMmioExtCap.Address32bit.ValueHigh, VtdMmioExtCap.Address32bit.Value));
    }
  }

  return EFI_SUCCESS;
}

/**
  Display BBS Version Information

  @param[in] VOID

  @retval None.

**/
VOID
EFIAPI
DumpBbsVersion (
  VOID
  )
{
  UINT8                            SocketId;
  BITSTREAM_ID_N1_FPGA_FME_STRUCT  BitstreamId;

  DEBUG ((DEBUG_ERROR, "DumpBbsVersion () ...\n"));

  //
  // Loop thru all FPGA of all sockets that are present
  //
  for (SocketId = 0; SocketId < MAX_SOCKET; SocketId++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << SocketId))) {
      continue;
    }

  BitstreamId.Data = MmioRead32 (FmeBar[SocketId] + BITSTREAM_ID_N1_FPGA_FME_REG);
  DEBUG ((DEBUG_ERROR, "Socket[%X] Blue BitStream Version: %X.%X.%X.%X\n",
    SocketId,
    BitstreamId.Bits.vermajor,
    BitstreamId.Bits.verminor,
    BitstreamId.Bits.verpatch,
    BitstreamId.Bits.verdebug));
  }
}

/**
  Ready to Boot Event notification handler to configure FPGA device.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context

**/
VOID
FpgaOnReadyToBoot (
  EFI_EVENT                 Event,
  VOID                      *Context
  )
{
  EFI_STATUS                    Status;
  UINT8                         Socket;
  UINT8                         BusNumber;
  UINT8                         SecondaryBusNumber;
  UINT8                         SubordinateBusNumber;
  UINT8                         FpgaBusNum;
  UINT8                         SwSmiInputValue;
// APTIOV_SERVER_OVERRIDE_RC_START: Use Segment Number instead of Socket Number to confirm the FPGA device.
  UINT8                         FpgaSegNum;
// APTIOV_SERVER_OVERRIDE_RC_END: Use Segment Number instead of Socket Number to confirm the FPGA device.

  DEBUG ((DEBUG_ERROR, "FpgaOnReadyToBoot()...\n"));

  //
  // Closed the event to avoid call twice
  //
  gBS->CloseEvent (Event);

  DEBUG ((DEBUG_ERROR, "FpgaSktActive = 0x%X.\n", FpgaConfiguration.FpgaSktActive));

  //
  // Checkpoint to indicate FPGA callback function is about to execute.
  //
  IioBreakAtCheckPoint (IioGlobalData, STS_FPGA_DXE_CALLBACK, 0, 0xFF);

  ZeroMem (&FmeBar[0], sizeof(FmeBar));

  //
  // Loop thru all FPGA of all sockets that are present
  //
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!(FpgaConfiguration.FpgaSktActive & (1 << Socket))) {
      continue;
    }

    BusNumber = IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[Socket][IIO_PSTACK3];  // Stack Limit Bus Number
    DEBUG ((DEBUG_ERROR, "MCP Limit BusNumber = %x\n", BusNumber));

    FpgaBusNum           = IioGlobalData->IioVar.IioVData.SocketStackBus[Socket][IIO_PSTACK3];
    SecondaryBusNumber   = IioPciExpressRead8 (IioGlobalData, Socket, FpgaBusNum, 0, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    SubordinateBusNumber = IioPciExpressRead8 (IioGlobalData, Socket, FpgaBusNum, 0, 0, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
// APTIOV_SERVER_OVERRIDE_RC_START: Use Segment Number instead of Socket Number to confirm the FPGA device.
	FpgaSegNum           = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].segmentSocket;
    DEBUG ((DEBUG_ERROR, "FPGA Root Port SegmentNumber = %x, BusNumber = %x, Secondary = 0x%X, Subordinate = 0x%X\n", FpgaSegNum, FpgaBusNum, SecondaryBusNumber, SubordinateBusNumber));

    Status = UpdateVFPBResources (FpgaBusNum, Socket, FpgaSegNum);
// APTIOV_SERVER_OVERRIDE_RC_END: Use Segment Number instead of Socket Number to confirm the FPGA device.
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Set PCIe RCiEP 0 Bus number (0x8086:0xBCC0)
    //
    SecondaryBusNumber   = BusNumber - FPGA_PREAllOCATE_BUS_NUM + 0x01;
    SubordinateBusNumber = BusNumber - FPGA_PREAllOCATE_BUS_NUM + 0x01;

    IioPciExpressWrite8 (IioGlobalData, Socket, FpgaBusNum, 0, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET, SecondaryBusNumber);
    IioPciExpressWrite8 (IioGlobalData, Socket, FpgaBusNum, 0, 0, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET, SubordinateBusNumber);

    //
    // Store FME BAR for each socket for later use
    //
    FmeBar[Socket] = (UINTN) IioPciExpressRead32 (IioGlobalData, Socket, SecondaryBusNumber, 0, 0, R_FPGA_FME_BAR) & 0xFFFFFFF0;
    DEBUG ((DEBUG_ERROR, "FmeBar[%X] = %X\n", Socket, FmeBar[Socket]));

    //
    // Set PCIe 1 Bus number (0x8086:0xBCBE)
    //
    SecondaryBusNumber   = BusNumber;
    SubordinateBusNumber = BusNumber;

    IioPciExpressWrite8 (IioGlobalData, Socket, FpgaBusNum, 2, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET, SecondaryBusNumber);
    IioPciExpressWrite8 (IioGlobalData, Socket, FpgaBusNum, 2, 0, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET, SubordinateBusNumber);
  }

  //
  // Display BBS version information
  //
  DumpBbsVersion ();

  //
  // Set the Thermal Threshold Value
  //
  FpgaThermalConfig ();

  //
  // FPGA VTD initialization
  //
  FpgaLateVtdInitialization ();

  //
  // Program LLPR (Last Level Protection Registers)
  //
  FpgaLLPRProgram ();

  //
  // Configure FPGA PCIe0 device (RCiEP / VT-d BAR Lock)
  //
  Status = FpgaPcieDeviceConfig ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Configure FPGA PCIe0 device failed.\n"));
  }

  //
  // Program FPGA HSSI config data.
  //
  Status = ProgramHssiConfigData ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Program FPGA HSSI config data status = %r.\n", Status));
  }

  //
  // Checkpoint to indicate FPGA Root Port is about to be hidden.
  //
  IioBreakAtCheckPoint (IioGlobalData, STS_FPGA_HIDE_PCIE_ROOT_PORT, 0, 0xFF);

  //
  // Enable software SMI to trigger hidding FPGA root port
  //
  DEBUG ((DEBUG_ERROR, "Trigger software SMI for FPGA.\n"));
  SwSmiInputValue = PcdGet8 (PcdFpgaSwSmiInputValue);
  IoWrite8 (0xB2, SwSmiInputValue);

  DEBUG ((DEBUG_ERROR, "FpgaOnReadyToBoot() End...\n"));
}


/**
  Standard EFI driver point. This driver registers the notify function of EndOfDxe.

  @param ImageHandle         -  A handle for the image that is initializing this driver.
  @param SystemTable         -  A pointer to the EFI system table.

  @retval EFI_SUCCESS        -  Driver initialized successfully.
  @retval Others             -  Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
FpgaDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_IIO_SYSTEM_PROTOCOL        *IioSystemProtocol;
  EFI_GLOBAL_NVS_AREA_PROTOCOL   *GlobalNvsAreaProtocol;
  VOID                           *Registration;
  UINT8                          Index;

  //
  // BIOS detect FPGA SKU
  //
  Status = FpgaConfigurationGetValues (&FpgaConfiguration);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_ERROR, "FpgaSktActive = 0x%X.\n", FpgaConfiguration.FpgaSktActive));
  if (FpgaConfiguration.FpgaSktActive == 0) {
    DEBUG ((DEBUG_ERROR, "FpgaDxeEntryPoint() no FPGA activated.\n"));
    return EFI_SUCCESS;
  }

  //
  // Locate Global NVS protocol
  //
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  GlobalNvsArea = GlobalNvsAreaProtocol->Area;
  ASSERT (GlobalNvsArea);

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    GlobalNvsArea->FpgaPresent[Index] = (FpgaConfiguration.FpgaSktActive & (1 << Index)) ? 1: 0;
    DEBUG ((DEBUG_ERROR, "Socket[%x] FPGA active state = %x\n", Index, GlobalNvsArea->FpgaPresent[Index]));
  }

  //
  // Locate IIO System Protocol
  //
  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  ASSERT_EFI_ERROR (Status);
  IioGlobalData = IioSystemProtocol->IioGlobalData;

  //
  // Locate the IioUds Protocol Interface
  //
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate MP Services Protocol
  //
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &mMpService);
  ASSERT_EFI_ERROR (Status);

  //
  // Checkpoint to indicate FPGA callback is about to create.
  //
  IioBreakAtCheckPoint (IioGlobalData, STS_FPGA_DXE_ENTRY, 0, 0xFF);

  //
  // Create an EndOfDxe call back event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  FpgaOnReadyToBoot,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &Registration
                  );
  DEBUG ((DEBUG_ERROR, "Register Event to Update for FPGA device.\n"));

  return Status;
}
