/**

Copyright (c) 2005-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file RegAccess.h

  This file handles register accesses.

**/

#include "RegAccess.h"
#include "MmrcProjectDefinitions.h"
#include "MemFunc.h"
#include "SysFunc.h"

#if !defined SIM && !defined JTAG
//
// Memory Mapped IO
//
/**

    Read 32 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be read

    @retval *addr - pointer to the address

**/
UINT32
Mmio32Read (
  IN        UINT32      RegisterAddress
)
{
  VOLATILE UINT32 *addr = (UINT32 *) RegisterAddress;
  return *addr;
}

/**

    Read 16 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be read

    @retval *addr - pointer to the address

**/
UINT16
Mmio16Read (
  IN        UINT32      RegisterAddress
)
{
  VOLATILE UINT16 *addr = (UINT16 *) RegisterAddress;
  return *addr;
}

/**

    Read 8 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be read

    @retval *addr - pointer to the address

**/
UINT8
Mmio8Read (
  IN        UINT32      RegisterAddress
)
{
  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  return *addr;
}

/**

    Write 32 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be written
    @param Value           - Data to be written

    @retval None

**/
VOID
Mmio32Write (
  IN        UINT32      RegisterAddress,
  IN        UINT32      Value
)
{

#ifdef IA32
#endif //IA32

  VOLATILE UINT32 *addr = (UINT32 *) RegisterAddress;
  *addr = Value;

#ifdef IA32
#endif //IA32

}

/**

    Write 16 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be written
    @param Value           - Data to be written

    @retval None

**/
VOID
Mmio16Write (
  IN        UINT32      RegisterAddress,
  IN        UINT16      Value
)
{

#ifdef IA32
#endif //IA32

  VOLATILE UINT16 *addr = (UINT16 *) RegisterAddress;
  *addr = Value;

#ifdef IA32
#endif //IA32

}

/**

    Write 8 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be written
    @param Value           - Data to be written

    @retval None

**/
VOID
Mmio8Write (
  IN        UINT32      RegisterAddress,
  IN        UINT8       Value
)
{
#ifdef IA32
#endif //IA32

  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  *addr = Value;

#ifdef IA32
#endif //IA32

}
#elif defined JTAG
UINT32
  MySimIoRead (UINT8 Size, UINT32 Port) {
  return 1;
}

VOID
MySimIoWrite(UINT8 Size, UINT32 Port, UINT32 Data) {
}

VOID
Mmio32Write (
  IN       UINT32       Address,
  IN       UINT32       Value
  )
/**

    Write 32 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be written
    @param Value           - Data to be written

    @retval None

**/
{
  return;
}
/**

    Write 32 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be read

    @retval -1

**/
UINT32
Mmio32Read (
  IN        UINT32      RegisterAddress
)
{
  //VOLATILE UINT32 *addr = (UINT32 *) RegisterAddress;
  //return *addr;
  return -1;
  //kirk
}
/**

    Write 32 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be read

    @retval *addr - pointer to the address

**/
UINT8
Mmio8Read (
  IN        UINT32      RegisterAddress
)
{
  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  return *addr;
}
/**

    Write 8 bits of data through memory mapped IO

    @param RegisterAddress - Address location from where the data should be written

    @retval -1

**/
VOID
Mmio8Write (
  IN        UINT32      RegisterAddress,
  IN        UINT8       Value
)
{
  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  *addr = Value;
}
#endif

/**
    get the location of the register

    @param BoxType  - Boxtype provided in the assignment.
    @param Channel  - Current Channel being examined.
    @param Instance - Channel under test
    @param Command  - Command
    @param PortId   - Port ID for the instance of the boxtype.
    @param Offset   - Base offset for the instance of the boxtype.
    @param Bus      - PCI Bus.
    @param Device   - PCI Device.
    @param Func     - PCI Function.
    @param Mode     - Read / Write

    @retval BoxTypeNotInSimulation 
    @retval InvalidInstance        - Invalid Channel
    @retval Error

**/
UINT32
GetRegisterAccessInfo (
  IN        UINT8       BoxType,
  IN        UINT8       Channel,
  IN        UINT8       Instance,
  IN  OUT   UINT32      *Command,
  IN  OUT   UINT8       *PortId,
  IN  OUT   UINT32      *Offset,
  IN  OUT   UINT8       *Bus,
  IN  OUT   UINT8       *Device,
  IN  OUT   UINT8       *Func,
  IN        UINT32      Mode
)
{
  PSYSHOST ThisMrcData;
  UINT32 Error;
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar;

  Error = NoError;
  ThisMrcData = GetMrcHostStructureAddress();
  CpuCsrAccessVar = &ThisMrcData->var.CpuCsrAccessVarHost;

#if SIM
  if (!InstancePortMapGenerated[BoxType].SimulationSupported) {
    return BoxTypeNotInSimulation;
  }
#endif
  //
  // Provided into this function is the instance number within the channel, getRegAccess requires the total instance index.
  // This translates the channel to the instance number in the INSTANCE_PORT_OFFSET structure.
  //
  if (ChannelToInstanceMapGenerated[BoxType][Channel] == 0xFF) {
    return InvalidInstance;
  } else {
    Instance += ChannelToInstanceMapGenerated[BoxType][Channel];
  }
  //
  // If the Type is a PCI access, get the B/D/F.
  //
  if (InstancePortMapGenerated[BoxType].AccessMethod == ePCI) {
    *Bus    = CpuCsrAccessVar->StackBus[ThisMrcData->var.mem.currentSocket][(InstancePortOffsetGenerated[InstancePortMapGenerated[BoxType].InstanceStartIndex + Instance].Port >> 8) & 0xff];
    *Device = (InstancePortOffsetGenerated[InstancePortMapGenerated[BoxType].InstanceStartIndex + Instance].Port >> 3) & 0x1f;
    *Func   = InstancePortOffsetGenerated[InstancePortMapGenerated[BoxType].InstanceStartIndex + Instance].Port & 0x07;
    *Offset = 0;
  } else if (InstancePortMapGenerated[BoxType].AccessMethod == eSB) {
    //
    // Look at the type, which is either a sideband port or an address offset for this box type.
    // If it's a port, assign it here. If it's an address offset, do the appropriate math to
    // target the Instance passed in.
    //
    *PortId = (UINT8) InstancePortOffsetGenerated[InstancePortMapGenerated[BoxType].InstanceStartIndex + Instance].Port;
    *Offset = InstancePortOffsetGenerated[InstancePortMapGenerated[BoxType].InstanceStartIndex + Instance].Offset;
  } else if (InstancePortMapGenerated[BoxType].AccessMethod == eBAR) {
    //
    // For BAR accesses, compile a full 32 bit offset from Port (upper 16 bits) and Offset (lower 16 bits).
    // We reuse the definition of Port for the upper 16 bits to save on code space in InstancePortOffsetGenerated.
    //
    *Offset = (InstancePortOffsetGenerated[InstancePortMapGenerated[BoxType].InstanceStartIndex + Instance].Port << 16) + InstancePortOffsetGenerated[InstancePortMapGenerated[BoxType].InstanceStartIndex + Instance].Offset;
  }
  //
  // Get the sideband command to read/write this particular box type.
  // If the command is not read or write, do not attempt to change it.
  //
  if (Mode == ModeRead) {
  *Command = InstancePortMapGenerated[BoxType].ReadOperation  << 24;
  } else if (Mode == ModeWrite) {
  *Command = InstancePortMapGenerated[BoxType].WriteOperation << 24;
  } else {
    *Command = *Command << 24;
  }
  //
  // If we've requested a register access to an instance that is
  // not valid (i.e. CH3 DDRIO register on a 2CH platform), flag
  // an error.
  //
/*  if (Instance >= InstancePortMapGenerated[BoxType].TotalInstances) {
    Error = InvalidInstance;
  }*/
  //
  // Handle all errors.
  //
  if (Error != NoError) {
    if (Mode == ModeRead) {
      MmrcDebugPrint ((SDBG_MIN, "RegRead ERROR: "));
    } else if (Mode == ModeWrite) {
      MmrcDebugPrint ((SDBG_MIN, "RegWrite ERROR: "));
    }
  }
  switch (Error) {
  case NoError:
    break;
  case InvalidInstance:
    MmrcDebugPrint ((SDBG_MIN, "Invalid Instance 0x%x for Box 0x%d (offset = 0x%x)\n", Instance, BoxType, *Offset));
    break;
  default:
    break;
  }
  return Error;
}

/**

  Reads registers from an specified Unit

  @param Boxtype:          Unit to select
  @param Instance:         Channel under test
  @param Offset:           Offset of register to read.
  @param PciReg:           If this is

  @retval Value read

**/
UINTX
MemRegRead (
  IN        UINT8           BoxType,
  IN        UINT8           Channel,
  IN        UINT8           Instance,
  IN        REGISTER_ACCESS Register
)
{
  UINT32    Command;
  UINT8     PortId;
  UINT32    Data;
  UINT32    BaseOffset;
  UINT8     Bus;
  UINT8     Device;
  UINT8     Func;
  UINT32   *MemOffset;
  PSYSHOST  ThisMrcData;
  UINT8     OffsetShift;
  UINT8     ShiftLeftAmount;
  UINT8     ShiftRightAmount;
  UINTX     TotalValue;
  REGISTER_ACCESS TempRegister;

  OffsetShift       = 0;
  ShiftLeftAmount   = 0;
  ShiftRightAmount  = 0;
  TotalValue        = 0;
  ThisMrcData       = GetMrcHostStructureAddress();

  if (GetRegisterAccessInfo (BoxType, Channel, Instance, &Command, &PortId, &BaseOffset, &Bus, &Device, &Func, ModeRead) == NoError) {
    //
    // If we are dealing with a PCI or MEM type register, the offset could
    // not be DWORD aligned. In this case, we may need to do multiple reads to
    // piece together the entire register since reads to PCI or MEM should be
    // DWORD aligned.
    //
    if (InstancePortMapGenerated[BoxType].AccessMethod == ePCI || InstancePortMapGenerated[BoxType].AccessMethod == eBAR) {
      OffsetShift       = (Register.Offset % 4);
      ShiftLeftAmount   = OffsetShift * 8;
      ShiftRightAmount  = 32 - ShiftLeftAmount;
      //
      // Align the offset to the next DWORD down (i.e. offset 0x33 will be offset 0x30 after this).
      //
      Register.Offset  -= OffsetShift;
      //
      // If the offset needed to be shifted (ie. it's not DWORD aligned) or the
      // mask is more than 32 bits, we need to recurse to read another DWORD.
      //
      if (OffsetShift != 0 || Register.Mask > 0xFFFFFFFF) {
        //
        // Create a temporary register mask in case we need to recurse with a new mask.
        //
        TempRegister.Mask     = Register.Mask >> ShiftRightAmount;
        TempRegister.Offset   = Register.Offset + 4;
        if (TempRegister.Mask != 0) {
          TotalValue = MemRegRead (BoxType, Channel, Instance, TempRegister);
        }
      }
      //
      // We've completed recursion, building up all upper DWORDs of the register. Now we
      // need to read the final DWORD. Since we've subtracted OffsetShift from the original
      // offset, we need to align the mask with the new DWORD aligned offset.
      //
      Register.Mask <<= ShiftLeftAmount;
    }
    //
    // If the action is to be taking place and the transaction is a PCI, then just do the PCI Access.
    //
    if (InstancePortMapGenerated[BoxType].AccessMethod == ePCI) {
      //
      // PCI access
      //
#if defined SIM || defined SIM_BUILD || defined KTI_SW_SIMULATION || defined HEADLESS_MRC
      Data = CpuSimRegAccess (ThisMrcData, ThisMrcData->var.mem.currentSocket,
        (UINT32)(ThisMrcData->var.common.mmCfgBase +
        (UINT32)(Bus << 20) +
        (UINT32)(Device << 15) +
        (UINT32)(Func << 12) +
                 Register.Offset + BaseOffset), 4, 0, READ_ACCESS);
#else
#ifdef SEGMENT_ACCESS
          Data = PCI_CFG_32B_READ (ThisMrcData->var.common.mmCfgBaseL[ThisMrcData->var.mem.currentSocket], Bus, Device, Func, Register.Offset + BaseOffset);
#else
          Data = PCI_CFG_32B_READ (ThisMrcData->var.common.mmCfgBase, Bus, Device, Func, Register.Offset + BaseOffset);
#endif // segment_access
#endif
        MmrcDebugPrint ((SDBG_REG_ACCESS, "PCI read %02X/%02X/%01X, Offset=0x%X, Mask=0x%08X, Value=0x%08X\n",
           Bus, Device, Func, Register.Offset + BaseOffset, (UINT32) Register.Mask, (UINT32) Data));
    } else if (InstancePortMapGenerated[BoxType].AccessMethod == eBAR) {
      //
      // Memory BAR access
      //
      MemOffset = (UINT32 *)(UINT32)(ThisMrcData->nvram.mem.socket[ThisMrcData->var.mem.currentSocket].DynamicVars[Channel][InstancePortMapGenerated[BoxType].AccessBase] + Register.Offset + BaseOffset);
      Data = (UINT32)*MemOffset;
      MmrcDebugPrint ((SDBG_REG_ACCESS, "MEM read to offset=0x%08X; data=0x%08X\n", MemOffset, (UINT32) Data));
    } else {
      //
      // Sideband access.
      //
      //
      // If the read and write opcodes are the same, it means reads are not supported.
      //
      if (InstancePortMapGenerated[BoxType].ReadOperation == InstancePortMapGenerated[BoxType].WriteOperation) {
        return 0;
      }

#if SIM
      Data = MySimMmio32ReadFast (PortId, Register.Offset + BaseOffset, 0xF);
#elif JTAG
      Data = isb32Read(PortId, Register.Offset + BaseOffset);
#else
      Mmio32Write (ThisMrcData->var.common.mmCfgBase + 0xD8, (Register.Offset + BaseOffset) & 0xFFFFFF00);
      Mmio32Write (ThisMrcData->var.common.mmCfgBase + 0xD0, (Command | (PortId << 16) | (((Register.Offset & 0x000000FF) << 8) + 0xF0)) );
      Data = Mmio32Read (ThisMrcData->var.common.mmCfgBase + 0xD4);
#endif
      MmrcDebugPrint ((SDBG_REG_ACCESS, "SB read to port=0x%02X; opcode=0x%02X; offset=0x%08X; data=0x%08X\n",
        PortId, InstancePortMapGenerated[BoxType].ReadOperation, Register.Offset + BaseOffset, (UINT32) Data));
    }
    TotalValue = (TotalValue << ShiftRightAmount) | ((Data & (UINT32)Register.Mask) >> ShiftLeftAmount);
    return TotalValue;
  }
#if USE_64_BIT_VARIABLES
  return 0xFFFFFFFFFFFFFFFF;
#else
  return 0xFFFFFFFF;
#endif
}

/**

  Reads registers from an specified Unit

  @param Boxtype:          Unit to select
  @param Instance:         Channel under test
  @param Offset:           Offset of register to write.
  @param Data:             Data to be written
  @param Be:               Byte enables

  @retval None

**/
VOID
MemRegWrite (
  IN        UINT8           BoxType,
  IN        UINT8           Channel,
  IN        UINT8           Instance,
  IN        REGISTER_ACCESS Register,
  IN        UINTX           Data,
  IN        UINT8           Be
)
{
  UINT32          Command;
  UINT8           PortId;
  UINT32          BaseOffset;
  UINT8           Bus;
  UINT8           Device;
  UINT8           Func;
  UINT32         *MemOffset;
  PSYSHOST        ThisMrcData;
  UINT8           OffsetShift;
  UINT8           ShiftLeftAmount;
  UINT8           ShiftRightAmount;
  UINTX           TotalValue;
  REGISTER_ACCESS TempRegister;
  UINTX           TempData;
  UINT8           Temp;

  OffsetShift       = 0;
  ShiftLeftAmount   = 0;
  ShiftRightAmount  = 0;
  TotalValue        = 0;
  ThisMrcData       = GetMrcHostStructureAddress();

  if (GetRegisterAccessInfo (BoxType, Channel, Instance, &Command, &PortId, &BaseOffset, &Bus, &Device, &Func, ModeWrite) == NoError) {
    //
    // If we are dealing with a PCI or MEM type register, the offset could
    // not be DWORD aligned. In this case, we may need to do multiple writes to
    // piece together the entire register since reads to PCI or MEM should be
    // DWORD aligned.
    //
    if (InstancePortMapGenerated[BoxType].AccessMethod == ePCI || InstancePortMapGenerated[BoxType].AccessMethod == eBAR) {
      OffsetShift       = (Register.Offset % 4);
      ShiftLeftAmount   = OffsetShift * 8;
      ShiftRightAmount  = 32 - ShiftLeftAmount;
      //
      // Align the offset to the next DWORD down (i.e. offset 0x33 will be offset 0x30 after this).
      //
      Register.Offset  -= OffsetShift;
      //
      // If the offset needed to be shifted (ie. it's not DWORD aligned) or the
      // mask is more than 32 bits, we need to recurse to read another DWORD.
      //
      if (OffsetShift != 0 || Register.Mask > 0xFFFFFFFF) {
        //
        // Write the next 32 bits (0xFFFFFFFF), but leave any bits above that
        // left over so if we need to recurse again, we can.
        //
        TempRegister.Mask     = Register.Mask >> ShiftRightAmount;
        TempRegister.Offset   = Register.Offset + 4;
        TempRegister.ShiftBit = Register.ShiftBit;
        TempData              = Data >> ShiftRightAmount;
        if (TempRegister.Mask != 0) {
          MemRegWrite (BoxType, Channel, Instance, TempRegister, TempData, 0xF);
        }
      }
      //
      // We've completed recursion, writing all upper DWORDs of the register. Now we
      // need to write the final DWORD. Since we've subtracted OffsetShift from the original
      // offset, we need to align the mask with the new DWORD aligned offset.
      //
      Register.Mask <<= ShiftLeftAmount;
      //
      // If the register is DWORD aligned and we're writing all 32 bits, skip the read.
      // Else, need to read/modify/write register.
      //
      if (OffsetShift != 0 || (UINT32) Register.Mask != 0xFFFFFFFF) {
        //
        // Since we're doing a 32 bit read/modify/write, only use the lower 32 bits of
        // the mask.
        //
        TempRegister.Mask = (UINT32) ~Register.Mask;
        TempRegister.Offset = Register.Offset;
        TotalValue = MemRegRead (BoxType, Channel, Instance, TempRegister);
      }
      //
      // Now that we have the fields read from the register and masked, OR in the new data.
      //
      TotalValue |= (Data << Register.ShiftBit);
      Data        = TotalValue << ShiftLeftAmount;
    }
    //
    // If the action is to be taking place and the transaction is a PCI, then just do the PCI Access.
    //
    if (InstancePortMapGenerated[BoxType].AccessMethod == ePCI) {
      MmrcDebugPrint ((SDBG_REG_ACCESS, "PCI write to %02X/%02X/%01X, Offset=0x%X, Mask=0x%08X, Data=0x%08X\n",
        Bus, Device, Func, Register.Offset + BaseOffset, (UINT32) Register.Mask, (UINT32) Data));
      //
      // PCI access
      //
#if defined SIM || defined SIM_BUILD || defined KTI_SW_SIMULATION || defined HEADLESS_MRC
      CpuSimRegAccess (ThisMrcData, ThisMrcData->var.mem.currentSocket,
        (UINT32)(ThisMrcData->var.common.mmCfgBase +
        (UINT32)(Bus << 20) +
        (UINT32)(Device << 15) +
        (UINT32)(Func << 12) +
                 Register.Offset + BaseOffset), 4, (UINT32)Data, WRITE_ACCESS);
#else
#ifdef SEGMENT_ACCESS     
      PCI_CFG_32B_WRITE (ThisMrcData->var.common.mmCfgBaseL[ThisMrcData->var.mem.currentSocket], Bus, Device, Func, Register.Offset + BaseOffset, (UINT32)Data);
#else      
      PCI_CFG_32B_WRITE (ThisMrcData->var.common.mmCfgBase, Bus, Device, Func, Register.Offset + BaseOffset, (UINT32)Data);
#endif
#endif
    } else if (InstancePortMapGenerated[BoxType].AccessMethod == eBAR) {
      //
      // Memory BAR access
      //
      MemOffset = (UINT32 *)(UINT32)(ThisMrcData->nvram.mem.socket[ThisMrcData->var.mem.currentSocket].DynamicVars[Channel][InstancePortMapGenerated[BoxType].AccessBase] + Register.Offset + BaseOffset);
      MmrcDebugPrint ((SDBG_REG_ACCESS, "MEM write to offset=0x%08X; data=0x%08X\n", MemOffset, (UINT32) Data));
      *MemOffset = (UINT32)Data;
    } else {
      //
      // Sideband access.
      //
      //
      // If we're writing all bits for each byte enable, skip the read.
      // Else, need to read/modify/write register.
      //
#if USE_64_BIT_VARIABLES
      if (ByteEnableToUintX (Be, 0xFFFFFFFFFFFFFFFF, &Temp) != Register.Mask) {
#else
      if (ByteEnableToUintX (Be, 0xFFFFFFFF, &Temp) != Register.Mask) {
#endif
        TempRegister.Mask   = (UINT32) ~Register.Mask;
        TempRegister.Offset = Register.Offset;
        TotalValue          = MemRegRead (BoxType, Channel, Instance, TempRegister);
        Data                = TotalValue | (Data << Register.ShiftBit);
      }
      MmrcDebugPrint ((SDBG_REG_ACCESS, "SB write to port=0x%02X; opcode=0x%02X; offset=0x%08X; data=0x%08X; be=0x%01X\n",
        PortId, InstancePortMapGenerated[BoxType].WriteOperation, Register.Offset + BaseOffset, (UINT32) Data, Be));
#if SIM
      MySimMmio32WriteFast (PortId, Command, Register.Offset + BaseOffset, (UINT32)Data, Be);
#elif defined JTAG
      isb32Write (PortId, Register.Offset + BaseOffset, Command, (UINT32) Data, Be);
#else
      Mmio32Write (ThisMrcData->var.common.mmCfgBase + 0xD8, (Register.Offset + BaseOffset) & 0xFFFFFF00);
      Mmio32Write (ThisMrcData->var.common.mmCfgBase + 0xD4, (UINT32)Data);
      Mmio32Write (ThisMrcData->var.common.mmCfgBase + 0xD0, (Command | (PortId << 16) | (((Register.Offset & 0x000000FF) << 8) + (Be << 4))) );
#endif
    }
  }
}

/**

    read the entire field specified

    @param BoxType  - Boxtype provided in the assignment.
    @param Channel  - Current Channel being examined.
    @param Instance - Channel under test
    @param Register - Field that specifies the location of the register including offset and bits.

    @retval Value 

**/
UINTX
MemFieldRead (
              IN        UINT8           BoxType,
              IN        UINT8           Channel,
              IN        UINT8           Instance,
              IN        REGISTER_ACCESS Register
              )
{
  UINTX Value;
  //
  // Read the entire register specified.
  //
  Value = MemRegRead (BoxType, Channel, Instance, Register);
  //
  // Shift it over so the field starting at ShiftBit is returned.
  //
  Value >>= Register.ShiftBit;
  //
  // Return the Value.
  //
  return Value;
}

/**

    Write the entire field specified

    @param BoxType  - Boxtype provided in the assignment.
    @param Channel  - Current Channel being examined.
    @param Instance - Channel under test
    @param Register - Field that specifies the location of the register including offset and bits.
    @param Value    - Value to be written
    @param Be       - Byte enable

    @retval None

**/
VOID
MemFieldWrite (
               IN        UINT8           BoxType,
               IN        UINT8           Channel,
               IN        UINT8           Instance,
               IN        REGISTER_ACCESS Register,
               IN        UINTX           Value,
               IN        UINT8           Be
               )
{
  UINTX TempValue;    // Value passed in is the Value that goes to the Bits, this contains the entire register Value.

  if (Register.Offset == 0xffff) {
    return;
  }
  //
  // Read the 32-bit register.
  //
  TempValue = MemRegRead (BoxType, Channel, Instance, Register);
  //
  // Shift the inputted Value to the correct start location
  //
  TempValue |= (TempValue << Register.ShiftBit);
  //
  // Write the register back.
  //
  MemRegWrite (BoxType, Channel, Instance, Register, TempValue, Be);

  return;
}


