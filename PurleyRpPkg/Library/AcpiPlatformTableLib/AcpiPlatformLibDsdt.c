/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
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


  @file AcpiDsdtHook.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"


#pragma optimize("",off)

extern BIOS_ACPI_PARAM             *mAcpiParameter;
extern struct SystemMemoryMapHob   *mSystemMemoryMap;
extern EFI_IIO_UDS_PROTOCOL        *mIioUds;


extern SOCKET_MP_LINK_CONFIGURATION  mSocketMpLinkConfiguration;
extern SOCKET_IIO_CONFIGURATION     mSocketIioConfiguration;
extern SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;

extern BOOLEAN      mCpuOrderSorted;
extern const UINT32 *mApicIdMap;

EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL *NgnAcpiSmmInterface = NULL; 

AML_OFFSET_TABLE_ENTRY            *mAmlOffsetTablePointer = NULL;

//
// iASL generates an offset table .h file which contains offsets from the start of DSDT AML for the following items:
//   Name, OperationRegion, WORDBusNumber, DWORDMemory, QWORDMemory, WORDIO
// The last 4 items must populate the last field (DescriptorName) for iASL to put them in the offset table.
//
/*
typedef struct {
    char                  *Pathname;
    char                  *FixTag;
} BIOS_FIXUP_AML_OFFSET_TABLE_ENTRY;

//
// BIOS fixup table for Purley platform
//
BIOS_FIXUP_AML_OFFSET_TABLE_ENTRY BiosFixupAmlOffsetTable[] =
{
    //
    // Cast AML_RESOURCE_ADDRESS16 for WORDBusNumber operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Cast AML_RESOURCE_ADDRESS16 for WORDIO operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Cast AML_RESOURCE_ADDRESS32 for DWORDMemory operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Cast AML_RESOURCE_ADDRESS64 for QWORDMemory operator so AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // Name operator AML_OFFSET_TABLE_ENTRY.offset is at value to change.
    // OperationRegion AML_OFFSET_TABLE_ENTRY.operator offset is at value to change.
    //
    {"PSYS",                "FIX0"}, // OperationRegion() in Acpi\AcpiTables\Dsdt\CommonPlatform.asi
    {"_SB_.PCXX.FIX1",      "FIX1"}, // WORDBusNumber()   in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX2",      "FIX2"}, // WORDIO()          in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX3",      "FIX3"}, // DWORDMemory()     in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX4",      "FIX4"}, // QWORDMemory()     in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX5",      "FIX5"}, // DWORDMemory()     in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX6",      "FIX6"}, // WORDIO()          in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.FIX7",      "FIX7"}, // WORDIO()          in Acpi\AcpiTables\Dsdt\PC00.asi (for all root bridges)
    {"_SB_.PCXX.MCTL",      "FIX8"}, // OperationRegion() in Acpi\AcpiTables\Dsdt\PcieHp.asi (for all root ports)
    {"_SB_.CRCM",           "FIXA"}, // OperationRegion() in Acpi\AcpiTables\Dsdt\Ngn.asi
    {0, 0}                           // table terminator
};
*/


/**

    Update the DSDT table

    @param *TableHeader   - The table to be set

    @retval EFI_SUCCESS - DSDT updated
    @retval EFI_INVALID_PARAMETER - DSDT not updated

**/
EFI_STATUS
PatchDsdtTable (
  IN OUT   EFI_ACPI_COMMON_HEADER   *Table
  )
{
  PCIE_PORT_INFO PCIEPortDefaults[] = {
    // DMI/PCIE 0
    { PCIE_PORT_0_DEV, IIO_CSTACK },
    //IOU0
    { PCIE_PORT_1A_DEV, IIO_PSTACK0 },
    { PCIE_PORT_1B_DEV, IIO_PSTACK0 },
    { PCIE_PORT_1C_DEV, IIO_PSTACK0 },
    { PCIE_PORT_1D_DEV, IIO_PSTACK0 },
    //IOU1
    { PCIE_PORT_2A_DEV, IIO_PSTACK1 },
    { PCIE_PORT_2B_DEV, IIO_PSTACK1 },
    { PCIE_PORT_2C_DEV, IIO_PSTACK1 },
    { PCIE_PORT_2D_DEV, IIO_PSTACK1 },
    //IOU2
    { PCIE_PORT_3A_DEV, IIO_PSTACK2 },
    { PCIE_PORT_3B_DEV, IIO_PSTACK2 },
    { PCIE_PORT_3C_DEV, IIO_PSTACK2 },
    { PCIE_PORT_3D_DEV, IIO_PSTACK2 },
    //MCP0 and MCP1
    { PCIE_PORT_4_DEV, IIO_PSTACK3 },
    { PCIE_PORT_5_DEV, IIO_PSTACK4 }
  };
  EFI_STATUS Status;
  UINT8   *DsdtPointer;
  UINT32  *Signature;
  UINT32  Fixes, NodeIndex;
  UINT8   Counter;
  UINT16  i;  // DSDT_PLATEXRP_OffsetTable LUT entries extends beyond 256!
  UINT64  MemoryBaseLimit = 0;
  UINT64  PciHGPEAddr = 0;
  UINT64  BusDevFunc = 0;
  UINT64  PcieHpBus = 0;
  UINT64  PcieHpDev = 0;
  UINT64  PcieHpFunc= 0;
  UINT8   PortCount = 0;
  UINT8   StackNumBus = 0;
  UINT8   StackNumIo = 0;
  UINT8   StackNumMem32 = 0;
  UINT8   StackNumMem64 = 0;
// APTIOV_SERVER_OVERRIDE_RC_START : Fix to resolve the yellow bang in windows device manager when offboard VGA device is connected on non-legacy sockets.
  UINT8   StackNumVgaIo0 = 0; // Start looking for Stack 1
  UINT8   StackNumVgaIo1 = 0; // Start looking for Stack 1
// APTIOV_SERVER_OVERRIDE_RC_END : Fix to resolve the yellow bang in windows device manager when offboard VGA device is connected on non-legacy sockets.
  UINT8   StackNumVgaMmioL = 0;
  UINT8   SktFPGA = 0;
  UINT8   SktFpgaKti = 0;
  UINT8   Stack = 0;
  UINT8   CurrSkt = 0, CurrStack = 0;
  UINT64  IioBusIndex = 0;
  UINT8   BusBase = 0, BusLimit = 0;
  UINT16  IoBase  = 0, IoLimit  = 0;
  UINT32  MemBase32 = 0, MemLimit32 = 0;
  UINT64  MemBase64 = 0, MemLimit64 = 0;
  AML_RESOURCE_ADDRESS16 *AmlResourceAddress16Pointer;
  AML_RESOURCE_ADDRESS32 *AmlResourceAddress32Pointer;
  AML_RESOURCE_ADDRESS64 *AmlResourceAddress64Pointer;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINT32 CpuCount = 0, CpuSkt = 0, CpuIndex = 0;
  EFI_ACPI_NAMEPACK_DWORD     *NamePtr;
  UINT8   *CurrPtr;
  FPGA_CONFIGURATION  FpgaConfiguration;

  Status = EFI_SUCCESS;
  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *)Table;

  if (mAmlOffsetTablePointer == NULL) return EFI_INVALID_PARAMETER;

  mAcpiParameter->MemoryBoardBitMask = 0;

  for(Counter = 0; Counter < mSystemMemoryMap->numberEntries; Counter++) {
    NodeIndex = mSystemMemoryMap->Element[Counter].NodeId;
    if((mAcpiParameter->MemoryBoardBitMask) & (1 << NodeIndex)){
      MemoryBaseLimit = mAcpiParameter->MemoryBoardRange[NodeIndex] + LShiftU64(mSystemMemoryMap->Element[Counter].ElementSize, MEM_ADDR_SHFT_VAL);
      mAcpiParameter->MemoryBoardRange[NodeIndex] = MemoryBaseLimit;
    } else {
      mAcpiParameter->MemoryBoardBitMask |= 1 << NodeIndex;
      MemoryBaseLimit = LShiftU64(mSystemMemoryMap->Element[Counter].BaseAddress, 30);
      mAcpiParameter->MemoryBoardBase[NodeIndex] = MemoryBaseLimit;
      MemoryBaseLimit = LShiftU64((mSystemMemoryMap->Element[Counter].BaseAddress + mSystemMemoryMap->Element[Counter].ElementSize), MEM_ADDR_SHFT_VAL);
      mAcpiParameter->MemoryBoardRange[NodeIndex] = MemoryBaseLimit;
    }
  }

  //
  // Mark all spare memory controllers as 1 in MemSpareMask bitmap.
  //
  mAcpiParameter->MemSpareMask = ~mAcpiParameter->MemoryBoardBitMask;

  mAcpiParameter->IioPresentBitMask = 0;
  mAcpiParameter->SocketBitMask = 0;

  Status = FpgaConfigurationGetValues (&FpgaConfiguration);
  ASSERT_EFI_ERROR (Status);

  mAcpiParameter->FpgaKtiPresent = mIioUds->IioUdsPtr->SystemStatus.FpgaPresentBitMap;

  for (Counter = 0; Counter < MAX_SOCKET; Counter++) {   
    //
    // Update the Bus number for FPGA KTI link
    //     
    if (mIioUds->IioUdsPtr->SystemStatus.FpgaPresentBitMap & (1 << Counter)){
      mAcpiParameter->FpgaKtiBase[Counter] =  mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Counter].SocketFirstBus;      
      DEBUG ((EFI_D_INFO, "SktFpga %x,  Base Bus = %X\n ", Counter , mAcpiParameter->FpgaKtiBase[Counter]));
    }  
    

    if (!mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Counter].Valid) continue;
    mAcpiParameter->SocketBitMask |= 1 << Counter;
    mAcpiParameter->IioPresentBitMask |= LShiftU64(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Counter].stackPresentBitmap, (Counter * 8));
    for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      mAcpiParameter->BusBase[Counter * MAX_IIO_STACK + Stack] = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Counter].StackRes[Stack].BusBase;
    }
    //
    // Update the Bus number for FPGA PCIE RP0
    //
    if (FpgaConfiguration.FpgaSktActive & (1 << Counter)) {
      mAcpiParameter->FpgaBusBase[Counter] = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Counter].StackRes[IIO_PSTACK3].BusLimit - FPGA_PREAllOCATE_BUS_NUM + 0x01;
      mAcpiParameter->FpgaBusLimit[Counter] = mIioUds->IioUdsPtr->PlatformData.IIO_resource[Counter].StackRes[IIO_PSTACK3].BusLimit;
      DEBUG ((EFI_D_INFO, "FPGA BUS Socket[%X] = %X\n ", Counter, mAcpiParameter->FpgaBusBase[Counter]));
    }
    
       
  }

  //
  // Initialize TsegSize - 1MB aligned.
  //
  PciHGPEAddr           = mIioUds->IioUdsPtr->PlatformData.PciExpressBase + 0x188;
  BusDevFunc            = 0x00;
  PcieHpBus             = 0;
  PcieHpDev             = 0;
  PcieHpFunc            = 0;

  Fixes = 0;
  //
  // Loop through the AML looking for values that we must fix up.
  //
  for (i = 0; mAmlOffsetTablePointer[i].Pathname != 0; i++) {
    //
    // Point to offset in DSDT for current item in AmlOffsetTable.
    //
    DsdtPointer = (UINT8 *) (TableHeader) + mAmlOffsetTablePointer[i].Offset;

    if (mAmlOffsetTablePointer[i].Opcode == AML_DWORD_PREFIX) {
      //
      // If Opcode is 0x0C, then operator is Name() or OperationRegion().
      // (TableHeader + AmlOffsetTable.Offset) is at offset for value to change.
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT(DsdtPointer[-1] == mAmlOffsetTablePointer[i].Opcode);
      //
      // AmlOffsetTable.Value has FIX tag, so check that to decide what to modify.
      //
      Signature = (UINT32 *) (&mAmlOffsetTablePointer[i].Value);
      switch (*Signature) {
        //
        // DRHx, ATSx, RHSx are not in ASI files, so why is the code here?
        // Due to iASL compiler change and DSDT patch design change, if these items need support
        // then the ASI files will need to conform to the format requires for iASL to add the items
        // to the offset table, and grt.bat will need to filter them out when iASL is executed.
        //
        case 0x31485244 :   // "DRH1" - PC06
          mAcpiParameter->DrhdAddr[0] = (UINTN) DsdtPointer;
          break;

        case 0x31535441 :   // "ATS1" - PC06
          mAcpiParameter->AtsrAddr[0] = (UINTN)((UINT64 *) DsdtPointer + 4);
          break;

        case 0x31534852 :   // "RHS1" - PC06
          mAcpiParameter->RhsaAddr[0] = (UINTN)((UINT64 *) DsdtPointer + 8);
          break;

        case 0x32485244 :   // "DRH2" - PC12
          mAcpiParameter->DrhdAddr[1] = (UINTN) DsdtPointer;
          break;

        case 0x32535441 :   // "ATS2" - PC12
          mAcpiParameter->AtsrAddr[1] = (UINTN)((UINT64 *) DsdtPointer + 4);
          break;

        case 0x32534852 :   // "RHS2" - PC12
          mAcpiParameter->RhsaAddr[1] = (UINTN)((UINT64 *) DsdtPointer + 8);
          break;

        case 0x33485244 :   // "DRH3" - PC18
          mAcpiParameter->DrhdAddr[1] = (UINTN) DsdtPointer;
          break;

        case 0x33535441 :   // "ATS3" - PC18
          mAcpiParameter->AtsrAddr[1] = (UINTN)((UINT64 *) DsdtPointer + 4);
          break;

        case 0x33534852 :   // "RHS3" - PC18
          mAcpiParameter->RhsaAddr[1] = (UINTN)((UINT64 *) DsdtPointer + 8);
          break;

        //
        // "FIX0" OperationRegion() in Acpi\AcpiTables\Dsdt\CommonPlatform.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '0')):
          * (UINT32 *) DsdtPointer = (UINT32) (UINTN) mAcpiParameter;
          Fixes++;
          break;
        //
        // "FIXA" OperationRegion() in Acpi\AcpiTables\Dsdt\Ngn.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', 'A')):
          Status = gBS->LocateProtocol(&gEfiNgnAcpiSmmInterfaceProtocolGuid, NULL, &NgnAcpiSmmInterface);
          ASSERT_EFI_ERROR(Status);
          *(UINT32 *) DsdtPointer = (UINT32) (UINTN) (UINT64)NgnAcpiSmmInterface->Area;
          Fixes++;
          break;
        //
        // "FIX8" OperationRegion() in Acpi\AcpiTables\Dsdt\PcieHp.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '8')):
          // APTIOV_SERVER_OVERRIDE_RC_START : IioBusIndex is not calculated.
          IioBusIndex = PortCount / PORTS_PER_SOCKET;
          // APTIOV_SERVER_OVERRIDE_RC_END : IioBusIndex is not calculated.
          Stack = PCIEPortDefaults[PortCount % PORTS_PER_SOCKET].Stack;
          PcieHpBus = mIioUds->IioUdsPtr->PlatformData.IIO_resource[IioBusIndex].StackRes[Stack].BusBase;
          PcieHpDev = PCIEPortDefaults[PortCount % PORTS_PER_SOCKET].Device;
          PcieHpFunc = PCIE_PORT_ALL_FUNC;

          //DEBUG((DEBUG_ERROR,"IioBus = %x, hpDev = %x, HpFunc= %x\n",IioBusIndex, PcieHpDev,PcieHpFunc));
          PciHGPEAddr &= ~(0xFFFF000);    // clear bus device func numbers
          BusDevFunc = (PcieHpBus << 8) | (PcieHpDev << 3) | PcieHpFunc;
          * (UINT32 *) DsdtPointer = (UINT32) (UINTN) (PciHGPEAddr + (BusDevFunc << 12));
          //DEBUG((DEBUG_ERROR,", BusDevFunc= %x, PortCount = %x\n",BusDevFunc, PortCount));

          PortCount++;
          Fixes++;
          break;

        default:
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_INDEX_OP) {
      //
      // If Opcode is 0x88, then operator is WORDBusNumber() or WORDIO().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS16 to change values.
      //
      AmlResourceAddress16Pointer = (AML_RESOURCE_ADDRESS16 *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT(AmlResourceAddress16Pointer->DescriptorType == mAmlOffsetTablePointer[i].Opcode);

      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX1" BUS resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '1')):
          CurrSkt = StackNumBus / MAX_IIO_STACK;
          CurrStack = StackNumBus % MAX_IIO_STACK;
          BusBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].BusBase;
          BusLimit = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].BusLimit;
   
          if (FpgaConfiguration.FpgaSktActive & (1 << CurrSkt)) {
            if (CurrStack == IIO_PSTACK3) {
              BusLimit = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].BusLimit - FPGA_PREAllOCATE_BUS_NUM;
              DEBUG ((EFI_D_INFO, "Skt[%X] Stack[%X] BusLimit = 0x%X\n ", CurrSkt, CurrStack, BusLimit));
            }
          }

          AmlResourceAddress16Pointer->Granularity = 0;
          if (BusLimit > BusBase) {
            AmlResourceAddress16Pointer->Minimum = (UINT16) BusBase;
            AmlResourceAddress16Pointer->Maximum = (UINT16) BusLimit;
            AmlResourceAddress16Pointer->AddressLength = (UINT16) (BusLimit - BusBase + 1);
          }
          //DEBUG((DEBUG_ERROR,", FIX1 BusBase = 0x%x, BusLimit = 0x%x\n",BusBase, BusLimit));
          StackNumBus++;
          Fixes++;
          break;

        //
        // "FIXB" BUS resource for FpgaKtiXX in Acpi\AcpiTables\Dsdt\FpgaKtiXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', 'B')):
          BusBase = 0;
          BusLimit = 0;          
          DEBUG ((EFI_D_INFO, "Patch BUS resource for FPGA KTI\n "));

          if  (mIioUds->IioUdsPtr->SystemStatus.FpgaPresentBitMap & (1 << SktFpgaKti)){
              BusBase = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SktFpgaKti].SocketFirstBus; 
              BusLimit = mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[SktFpgaKti].SocketLastBus;                   
              DEBUG ((EFI_D_INFO, "SktFpga [%X]:  BusBase[0x%X] BusLimit 0x%X\n ", SktFpgaKti, BusBase, BusLimit));
          }
   
          AmlResourceAddress16Pointer->Granularity = 0;
          if (BusLimit > BusBase) {
            AmlResourceAddress16Pointer->Minimum = (UINT16) BusBase;
            AmlResourceAddress16Pointer->Maximum = (UINT16) BusLimit;
            AmlResourceAddress16Pointer->AddressLength = (UINT16) (BusLimit - BusBase + 1);
          }
          
          SktFpgaKti++;
          Fixes++;
          break;

        //
        // "FIX2" IO resource for for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '2')):
          AmlResourceAddress16Pointer->Granularity = 0;
          CurrSkt = StackNumIo / MAX_IIO_STACK;
          CurrStack = StackNumIo % MAX_IIO_STACK;
          IoBase = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].PciResourceIoBase;
          IoLimit = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].PciResourceIoLimit;
          if (IoLimit > IoBase) {
            AmlResourceAddress16Pointer->Minimum = (UINT16) IoBase;
            AmlResourceAddress16Pointer->Maximum = (UINT16) IoLimit;
            AmlResourceAddress16Pointer->AddressLength = (UINT16) (IoLimit - IoBase + 1);
          }
          //DEBUG((DEBUG_ERROR,", FIX2 IoBase = 0x%x, IoLimit = 0x%x\n",IoBase, IoLimit));
          StackNumIo++;
          Fixes++;
          break;

        //
        // "FIX6" IO resource for PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '6')):
          AmlResourceAddress16Pointer->Granularity = 0;
          CurrSkt = StackNumVgaIo0 / MAX_IIO_STACK;
          CurrStack = StackNumVgaIo0 % MAX_IIO_STACK;
          if ((mSocketMpLinkConfiguration.LegacyVgaSoc == CurrSkt) &&
              (mSocketMpLinkConfiguration.LegacyVgaStack == CurrStack)){
              AmlResourceAddress16Pointer->Minimum = (UINT16) 0x03b0;
              AmlResourceAddress16Pointer->Maximum = (UINT16) 0x03bb;
              AmlResourceAddress16Pointer->AddressLength = (UINT16) 0x000C;
          }
          StackNumVgaIo0++;
          Fixes++;
         break;

        //
        // "FIX7" IO resource for PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '7')):
          AmlResourceAddress16Pointer->Granularity = 0;
          CurrSkt = StackNumVgaIo1 / MAX_IIO_STACK;
          CurrStack = StackNumVgaIo1 % MAX_IIO_STACK;
          if ((mSocketMpLinkConfiguration.LegacyVgaSoc == CurrSkt) &&
              (mSocketMpLinkConfiguration.LegacyVgaStack == CurrStack)) {
        	  AmlResourceAddress16Pointer->Minimum = (UINT16) 0x03c0;
              AmlResourceAddress16Pointer->Maximum = (UINT16) 0x03df;
              AmlResourceAddress16Pointer->AddressLength = (UINT16) 0x0020;
          }
          StackNumVgaIo1++;
          Fixes++;
          break;

        default:
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_SIZE_OF_OP) {
      //
      // If Opcode is 0x87, then operator is DWORDMemory().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS32 to change values.
      //
      AmlResourceAddress32Pointer = (AML_RESOURCE_ADDRESS32 *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT(AmlResourceAddress32Pointer->DescriptorType == mAmlOffsetTablePointer[i].Opcode);
      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX3" PCI32 resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '3')):
          AmlResourceAddress32Pointer->Granularity = 0;
          CurrSkt = StackNumMem32 / MAX_IIO_STACK;
          CurrStack = StackNumMem32 % MAX_IIO_STACK;
          MemBase32 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].PciResourceMem32Base;
          MemLimit32 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].PciResourceMem32Limit;

          if (FpgaConfiguration.FpgaSktActive & (1 << CurrSkt)) {
            if (CurrStack == IIO_PSTACK3) {
              //Carve out FPGA RPa resources from the original BIOS defined ACPI root bridge
              MemLimit32 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].PciResourceMem32Limit - FPGA_PREALLOCATE_MEM_SIZE - FPGA_PREALLOCATE_VTD_SIZE;
              DEBUG ((EFI_D_INFO, "Skt[%X] Stack[%X] MemLimit32 = 0x%X\n ", CurrSkt, CurrStack, MemLimit32));
            }
          }
 
          if (MemLimit32 > MemBase32) {
            AmlResourceAddress32Pointer->Minimum = (UINT32) MemBase32;
            AmlResourceAddress32Pointer->Maximum = (UINT32) MemLimit32;
            AmlResourceAddress32Pointer->AddressLength = (UINT32) (MemLimit32 - MemBase32 + 1);
          }
          //DEBUG((DEBUG_ERROR,", FIX3 MemBase32 = 0x%08x, MemLimit32 = 0x%08x\n",MemBase32, MemLimit32));
          StackNumMem32++;
          Fixes++;
          break;

        //
        // "FIX5" IO resource for PCXX in Acpi\AcpiTables\Dsdt\PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '5')):
          AmlResourceAddress32Pointer->Granularity = 0;
          CurrSkt = StackNumVgaMmioL / MAX_IIO_STACK;
          CurrStack = StackNumVgaMmioL % MAX_IIO_STACK;
          if ((mSocketMpLinkConfiguration.LegacyVgaSoc == CurrSkt) &&
              (mSocketMpLinkConfiguration.LegacyVgaStack == CurrStack)) {
              AmlResourceAddress32Pointer->Minimum = 0x000a0000;
              AmlResourceAddress32Pointer->Maximum = 0x000bffff;
              AmlResourceAddress32Pointer->AddressLength = 0x00020000;
          }
          StackNumVgaMmioL++;
          Fixes++;
          break;

        //
        // "FIXZ" IO resource for FpgaBusXX in Acpi\AcpiTables\Dsdt\FpgaBusXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', 'Z')):
          AmlResourceAddress32Pointer->Granularity = 0;
          if (FpgaConfiguration.FpgaSktActive & (1 << SktFPGA)) {
              AmlResourceAddress32Pointer->Minimum = mIioUds->IioUdsPtr->PlatformData.IIO_resource[SktFPGA].StackRes[IIO_PSTACK3].VtdBarAddress;
              AmlResourceAddress32Pointer->Maximum = mIioUds->IioUdsPtr->PlatformData.IIO_resource[SktFPGA].StackRes[IIO_PSTACK3].VtdBarAddress + 0x3fff;
              AmlResourceAddress32Pointer->AddressLength = 0x4000;
              DEBUG ((EFI_D_INFO, "Skt[%X] FPGA ACPI VTD BASE = 0x%X\n ", SktFPGA, AmlResourceAddress32Pointer->Minimum));
          }
          SktFPGA++;
          Fixes++;
          break;

        default:
          break;
      }
    } else if (mAmlOffsetTablePointer[i].Opcode == AML_CREATE_DWORD_FIELD_OP) {
      //
      // If Opcode is 0x8A, then operator is QWORDMemory().
      // (TableHeader + AmlOffsetTable.Offset) must be cast to AML_RESOURCE_ADDRESS64 to change values.
      //
      AmlResourceAddress64Pointer = (AML_RESOURCE_ADDRESS64 *) (DsdtPointer);
      //
      // The assert below confirms that AML structure matches the offsets table.
      // If not then patching the AML would just corrupt it and result in OS failure.
      // If you encounter this assert something went wrong in *.offset.h files
      // generation. Remove the files and rebuild.
      //
      ASSERT(AmlResourceAddress64Pointer->DescriptorType == mAmlOffsetTablePointer[i].Opcode);
      //
      // Last 4 chars of AmlOffsetTable.Pathname has FIX tag.
      //
      Signature = (UINT32 *) (mAmlOffsetTablePointer[i].Pathname + AsciiStrLen(mAmlOffsetTablePointer[i].Pathname) - 4);
      switch (*Signature) {
        //
        // "FIX4" PCI64 resource for PCXX in Acpi\AcpiTables\Dsdt\SysBus.asi and PCXX.asi
        //
        case (SIGNATURE_32 ('F', 'I', 'X', '4')):
          if (mSocketIioConfiguration.Pci64BitResourceAllocation) {
            AmlResourceAddress64Pointer->Granularity = 0;
            CurrSkt = StackNumMem64 / MAX_IIO_STACK;
            CurrStack = StackNumMem64 % MAX_IIO_STACK;
            MemBase64 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].PciResourceMem64Base;
            MemLimit64 = mIioUds->IioUdsPtr->PlatformData.IIO_resource[CurrSkt].StackRes[CurrStack].PciResourceMem64Limit;
            if (MemLimit64 > MemBase64) {
              AmlResourceAddress64Pointer->Minimum = (UINT64) MemBase64;
              AmlResourceAddress64Pointer->Maximum = (UINT64) MemLimit64;
              AmlResourceAddress64Pointer->AddressLength = (UINT64) (MemLimit64 - MemBase64 + 1);
            }
            //DEBUG((DEBUG_ERROR,", FIX4 MemBase64 = 0x%x, MemLimit64 = 0x%x\n",MemBase64, MemLimit64));
            StackNumMem64++;
            Fixes++;
          }
          break;
        default:
          break;
      }
    }
  }

  CurrPtr = (UINT8 *) TableHeader;
  for (DsdtPointer = CurrPtr; DsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length); DsdtPointer++) {

    //fix CpuMemHp.asi "%" value = 0x25, force no same ASL code string as it...
    if ((DsdtPointer[0] == 'C') && (DsdtPointer[1] == 'P')  && (DsdtPointer[4] == 0x25)) {
      if (mCpuOrderSorted) {
        CpuCount = CpuCount + 1;
        //DEBUG((EFI_D_ERROR, ":: Skt[%x]  CP%c%c  %02x%04x\n", CpuSkt, DsdtPointer[2],DsdtPointer[3], DsdtPointer[4], (UINT16)DsdtPointer[5]));

        CpuSkt = DsdtPointer[6];
        CpuIndex = DsdtPointer[5];

        DsdtPointer[4] = (UINT8) (CpuSkt * MAX_CORE * MAX_THREAD + CpuIndex);

        //Update IO Address
        *(UINT16 *)(DsdtPointer+5) = (UINT16)(PM_BASE_ADDRESS + 0x10);

        //DEBUG((EFI_D_ERROR, "::: --- Update: Skt[%x]  CP%c%c  %02x %04x\n", CpuSkt, DsdtPointer[2],DsdtPointer[3], DsdtPointer[4], (UINT16)DsdtPointer[5]));
      }
    }

    //
    // Find APTB name
    //
    if((DsdtPointer[0] == 'A') && (DsdtPointer[1] == 'P') && (DsdtPointer[2] == 'T') && (DsdtPointer[3] == 'B')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        //DEBUG(( EFI_D_ERROR, "...Found 'APTB', but it is not NAME_OP, continue...\n "));
        continue;
      }

      Counter = DsdtPointer[7];
      ASSERT (Counter == (UINT32) (MAX_THREAD * MAX_CORE));
      //DEBUG(( EFI_D_ERROR, "\n::ACPI::  Found 'APTB'...Counter = DsdtPointer[7] = %x\n\n", Counter));
      for (i = 0; i < Counter; i++) {
        //DEBUG(( EFI_D_ERROR, "Before override, DsdtPointer[%x] = %x,   ", i, DsdtPointer[i+8] ));
        DsdtPointer[i+8] = (UINT8)mApicIdMap[i];
        //DEBUG(( EFI_D_ERROR, "Then override value = %x \n", DsdtPointer[i+8] ));
      }
    }
// APTIOV_SERVER_OVERRIDE_RC_START : AMI ACPI driver updates sleep states object based on setup controls from ACPI configuration.
#if 0

    //
    // Fix up _S3
    //
    if ((DsdtPointer[0] == '_') && (DsdtPointer[1] == 'S') && (DsdtPointer[2] == '3')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        //DEBUG(( EFI_D_ERROR, "...Found '_S3', but it is not NAME_OP, continue...\n "));
        continue;
      }

      if (!mSocketPowermanagementConfiguration.AcpiS3Enable) {
        //
        // S3 disabled
        //
        DsdtPointer[0] = 'D';
      }
    }

    //
    // Fix up _S4
    //
    if ((DsdtPointer[0] == '_') && (DsdtPointer[1] == 'S') && (DsdtPointer[2] == '4')) {
      NamePtr = ACPI_NAME_COMMAND_FROM_NAMEPACK_STR (DsdtPointer);
      if (NamePtr->StartByte != AML_NAME_OP) {
        //DEBUG(( EFI_D_ERROR, "...Found '_S4', but it is not NAME_OP, continue...\n "));
        continue;
      }

      if (!mSocketPowermanagementConfiguration.AcpiS4Enable) {
        //
        // S4 disabled
        //
        DsdtPointer[0] = 'D';
      }
    }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : AMI ACPI driver updates sleep states object based on setup controls from ACPI configuration.
  }

  //return Status;
  return EFI_SUCCESS;

}