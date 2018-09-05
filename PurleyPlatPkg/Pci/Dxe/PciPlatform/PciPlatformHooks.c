/**
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
**/
/**

Copyright (c)  2004 - 2016 Intel Corporation. All rights 
reserved This software and associated documentation (if any) is 
furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by 
such license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PciPlatformHooks.c
    
    Registers onboard PCI ROMs with PCI.IO.
    
Revision History:

**/

#include <PiDxe.h>
#include <Protocol/IioUds.h> 
#include <PciPlatform.h>
#include <PciPlatformHooks.h>
#include <PciSupportLib.h>
#include <IoApic.h>
#include <Cpu/CpuBaseLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/IioSystem.h>
#ifdef PC_HOOK
#include <IndustryStandard/Acpi10.h>
#include <Library/ReportStatusCodeLib.h>
#include <EfiSmStatusCodes.h>
#include <Guid/DebugMask.h>
EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL   *PciResAlloc = NULL; 
#endif
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>

SYSTEM_CONFIGURATION  mSystemConfiguration;
EFI_IIO_UDS_PROTOCOL                    *mIioUds          = NULL;
EFI_IIO_SYSTEM_PROTOCOL       *IioSystemProtocol = NULL;
IIO_GLOBALS                   *IioGlobalData = NULL;

VOID
ChipsetCallback (
  IN  EFI_HANDLE                                    RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS   PciAddress,
  IN  EFI_PCI_ENUMERATION_PHASE                     Phase,
  EFI_PCI_CALLBACK_CONTEXT                          *Context
  )
{
  EFI_LIST_ENTRY    *NodeEntry;
  PCI_CALLBACK_DATA *PciCallbackData;

  //
  // Check if the node has been added
  //
  // DEBUG ((DEBUG_ERROR, "PCI Callback (%d,%d,%d)\n",PciAddress.Bus, PciAddress.Device, PciAddress.Function ));
  //
  Context->PciRootBridgeIo  = mPciPrivateData.PciRootBridgeIo;
  NodeEntry                 = GetFirstNode (&mPciPrivateData.PciCallbackList);
  while (!IsNull (&mPciPrivateData.PciCallbackList, NodeEntry)) {
    PciCallbackData = PCI_CALLBACK_DATA_FROM_LINK (NodeEntry);
    if (PciCallbackData->Phase & Phase) {
      (PciCallbackData->Function) (RootBridgeHandle, PciAddress, Phase, Context);
    }

    NodeEntry = GetNextNode (&mPciPrivateData.PciCallbackList, NodeEntry);
  }
}

/**

    GC_TODO: add routine description

    @param StartBus - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
PciTreeTraverse (
  IN  UINT8                                        StartBus
  )
{
  UINT64  PciAddress;
  UINT8   Device;
  UINT8   Func;
  UINT8   SecondaryBus;
  BOOLEAN MultiFunc;

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    MultiFunc =  FALSE;
    for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
      if (IsPciDevicePresent (
            mPciPrivateData.PciRootBridgeIo,
            &mPciPrivateData.Context.PciHeader,
            StartBus,
            Device,
            Func
            )) {
        if ((Func == 0) && IS_PCI_MULTI_FUNC(&(mPciPrivateData.Context.PciHeader))) {
          MultiFunc = TRUE;
        }
        PciAddress = EFI_PCI_ADDRESS (StartBus, Device, Func, 0);
        ChipsetCallback (
          mPciPrivateData.RootBridgeHandle,
          *(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS *) &PciAddress,
          mPciPrivateData.PciEnumerationPhase,
          &(mPciPrivateData.Context)
          );
        if (IS_PCI_BRIDGE (&(mPciPrivateData.Context.PciHeader))) {
          PciAddress = EFI_PCI_ADDRESS (StartBus, Device, Func, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
          mPciPrivateData.PciRootBridgeIo->Pci.Read (
                                                mPciPrivateData.PciRootBridgeIo,
                                                EfiPciWidthUint8,
                                                *(UINT64 *) &PciAddress,
                                                1,
                                                &SecondaryBus
                                                );
          if ((SecondaryBus > 0) && (SecondaryBus < 0xFF)) {
            PciTreeTraverse (SecondaryBus);
          }
        }
      }

      if (MultiFunc == FALSE) {
        //
        // Skip sub functions, this is not a multi function device
        //
        Func = PCI_MAX_FUNC;
      }
    }
  }

  return EFI_SUCCESS;
}

/**

  Program Io Apic Id

  @param IoApicAddress and IoApicId

  @retval None

**/
VOID
ProgramIoApicId (
  IN UINT32                            IoApicAddress,
  IN UINT8                             IoApicId
  )
{

  UINT32  Data;

  mPciPrivateData.CpuIo->Mem.Read (
                              mPciPrivateData.CpuIo,
                              EfiCpuIoWidthUint32,
                              IoApicAddress + EFI_IO_APIC_INDEX_OFFSET,
                              1,
                              &Data
                              );

  //
  // IOAPIC is not there
  //
  if (Data == (UINT32) -1) {
    return ;
  }
  //
  // Set up IO APIC ID and enable FSB delivery
  // Use CPU IO protocol since the IO APIC ranges
  // are not included in PCI apertures
  //
  Data = EFI_IO_APIC_ID_REGISTER;
  mPciPrivateData.CpuIo->Mem.Write (
                              mPciPrivateData.CpuIo,
                              EfiCpuIoWidthUint32,
                              IoApicAddress + EFI_IO_APIC_INDEX_OFFSET,
                              1,
                              &Data
                              );

  Data = IoApicId << EFI_IO_APIC_ID_BITSHIFT;
  mPciPrivateData.CpuIo->Mem.Write (
                              mPciPrivateData.CpuIo,
                              EfiCpuIoWidthUint32,
                              IoApicAddress + EFI_IO_APIC_DATA_OFFSET,
                              1,
                              &Data
                              );

  Data = EFI_IO_APIC_BOOT_CONFIG_REGISTER;
  mPciPrivateData.CpuIo->Mem.Write (
                              mPciPrivateData.CpuIo,
                              EfiCpuIoWidthUint32,
                              IoApicAddress + EFI_IO_APIC_INDEX_OFFSET,
                              1,
                              &Data
                              );

  Data = EFI_IO_APIC_FSB_INT_DELIVERY;
  mPciPrivateData.CpuIo->Mem.Write (
                              mPciPrivateData.CpuIo,
                              EfiCpuIoWidthUint32,
                              IoApicAddress + EFI_IO_APIC_DATA_OFFSET,
                              1,
                              &Data
                              );
}

#ifdef EFI_PCI_IOV_SUPPORT
/**

  Initialize the Pci Iov Platform Data.

  @param ImageHandle  -  Handle to the image.
  @param SystemTable  -  Handle to System Table.

  @retval EFI_STATUS  -  Status of the function calling.

**/
EFI_STATUS
EFIAPI
PciPlatformInitPciIovData (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_PCI_IOV_PLATFORM_POLICY           PciIovPolicy;
  UINT32                                SystemPageSize;
  EFI_PCI_IOV_PLATFORM_PROTOCOL         *gPciIovPlatformProtocol;

  Status = gBS->LocateProtocol (
             &gEfiPciIovPlatformProtocolGuid,
             NULL,
             &gPciIovPlatformProtocol
             );
  if (!EFI_ERROR (Status)) {
    Status = gPciIovPlatformProtocol->GetSystemLowestPageSize (
                                           gPciIovPlatformProtocol,
                                           &SystemPageSize
                                           );
    if (!EFI_ERROR (Status)) {
      Status = PcdSet32S (PcdSrIovSystemPageSize, (1 << SystemPageSize));
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    } else {
      return Status;
    }
    Status = gPciIovPlatformProtocol->GetPlatformPolicy (
                                           gPciIovPlatformProtocol,
                                           &PciIovPolicy
                                           );
    if (!EFI_ERROR (Status)) {
      if (PciIovPolicy & EFI_PCI_IOV_POLICY_ARI) {
        Status = PcdSetBoolS (PcdAriSupport, TRUE);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) return Status;
      } else {
        Status = PcdSetBoolS (PcdAriSupport, FALSE);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) return Status;
      }
      if (PciIovPolicy & EFI_PCI_IOV_POLICY_SRIOV) {
        Status = PcdSetBoolS (PcdSrIovSupport, TRUE);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) return Status;
      } else {
        Status = PcdSetBoolS (PcdSrIovSupport, FALSE);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) return Status;
      }
      if (PciIovPolicy & EFI_PCI_IOV_POLICY_MRIOV) {
        Status = PcdSetBoolS (PcdMrIovSupport, TRUE);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) return Status;
      } else {
        Status = PcdSetBoolS (PcdMrIovSupport, FALSE);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) return Status;
      }
    } else {
      return Status;
    }
    DEBUG ((
          EFI_D_INFO,
          " Initialized SR-IOV Platform Data: PCIIovPolicy = 0x%x; SystemPageSize = 0x%x;\n",
          PciIovPolicy, SystemPageSize
          ));
  } else {
    DEBUG ((
          EFI_D_INFO,
          " Using default values for SystemPageSize;\n"
          ));
  }
  return Status;
}
#endif

/**

  Platform Pci Express init.

  @param HostBridgeInstance  -  Pointer to Host Bridge private data
                                does not support 64 bit memory addresses.

  @retval EFI_SUCCESS  -  Success.
  
**/
EFI_STATUS
PciPlatformEarlyInit (
  VOID
  )
{
  EFI_STATUS                      Status;
  //
  // Locate the IIO Protocol Interface
  //
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid,NULL,&mIioUds);
  ASSERT_EFI_ERROR (Status);
  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  ASSERT_EFI_ERROR (Status); 

  IioGlobalData = IioSystemProtocol->IioGlobalData;

#ifdef EFI_PCI_IOV_SUPPORT
  Status = PciPlatformInitPciIovData();	// Update IOV PCD values
#endif
  return EFI_SUCCESS;
}

/**

  Attempts to set the XHCI controller's PCI CMD.MSE and CMD.BME bits to enable OS kernel debugging over XHCI.

**/
VOID
AttemptToSetXhciMse (
  )

{
  UINT32  XhciBar;
  UINT16  Command;

  //
  // Step 1. Make sure the XHCI BAR is initialized.
  //         Check if lower 32 bits of 64-bit BAR are configured.
  //
  XhciBar = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI) + R_PCH_XHCI_MEM_BASE) & ~(0xF);
  if (XhciBar == 0xFFFFFFF0) {
    return;
  }
  if ((XhciBar & 0xFFFF0000) == 0) {
    //
    // If lower 32 bits are not configured, check upper 32 bits.
    //
    XhciBar = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI) + R_PCH_XHCI_MEM_BASE + 4);
    if (XhciBar == 0) {
      return;
    }
  }

  //
  // Step 2. If XHCI's MSE (Memory Space Enable) or BME (Bus Master Enable) bits are cleared, set them.
  //
  Command = MmioRead16 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI) + PCI_COMMAND_OFFSET);
  if ((Command & (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)) != (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)) {
    MmioOr16 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI) + PCI_COMMAND_OFFSET, (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER));
  }
}


/**
  
  Init pci device registers after the device resources have been allocated, so
  that devices behind a bus could be accessed.
    
  @param HostBridgeInstance  -  PCI_HOST_BRIDGE_INSTANCE.

  @retval EFI_SUCCESS  -  Function has completed successfully.

**/
EFI_STATUS
PciPlatformPostInit (
  VOID
  )
{
  //
  // Program all the IOAPIC in system
  //
  UINT8 Socket, Stack, IoApicId;
  Stack = 0;
  IoApicId = 0;
  ProgramIoApicId (mIioUds->IioUdsPtr->PlatformData.IIO_resource[0].StackRes[0].IoApicBase, PCH_IOAPIC_ID);
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (!(mIioUds->IioUdsPtr->SystemStatus.socketPresentBitMap & (1 << Socket)))
      continue;

    for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      if (!(mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[Socket].stackPresentBitmap & (1 << Stack)))
        continue;
      switch (Socket) {
      case 0:
        switch(Stack) {
        case 0: 
          IoApicId = PC00_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC01_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC02_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC03_IOAPIC_ID;
          break;
#if MAX_SOCKET <= 4
        case 4: 
          IoApicId = PC04_IOAPIC_ID;
          break;
        case 5: 
          IoApicId = PC05_IOAPIC_ID;
          break;
#endif
        default:
          break;
        }
      break;
      case 1:
        switch(Stack) {
#if MAX_SOCKET <= 4
        case 0: 
          IoApicId = PC06_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC07_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC08_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC09_IOAPIC_ID;
          break;
        case 4: 
          IoApicId = PC10_IOAPIC_ID;
          break;
        case 5: 
          IoApicId = PC11_IOAPIC_ID;
          break;
#else
        case 0: 
          IoApicId = PC04_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC05_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC06_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC07_IOAPIC_ID;
          break;
#endif
        default:
          break;
        }
      break;
      case 2:
        switch(Stack) {
#if MAX_SOCKET <= 4
        case 0: 
          IoApicId = PC12_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC13_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC14_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC15_IOAPIC_ID;
          break;
        case 4: 
          IoApicId = PC16_IOAPIC_ID;
          break;
        case 5: 
          IoApicId = PC17_IOAPIC_ID;
          break;
#else
        case 0: 
          IoApicId = PC08_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC09_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC10_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC11_IOAPIC_ID;
          break;
#endif
        default:
          break;
        }
      break;
      case 3:
        switch(Stack) {
#if MAX_SOCKET <= 4
        case 0: 
          IoApicId = PC18_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC19_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC20_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC21_IOAPIC_ID;
          break;
        case 4: 
          IoApicId = PC22_IOAPIC_ID;
          break;
        case 5: 
          IoApicId = PC23_IOAPIC_ID;
          break;
#else
        case 0: 
          IoApicId = PC12_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC13_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC14_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC15_IOAPIC_ID;
          break;
#endif
        default:
          break;
        }
#if MAX_SOCKET > 4
      case 4:
        switch(Stack) {
        case 0: 
          IoApicId = PC16_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC17_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC18_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC19_IOAPIC_ID;
          break;
        default:
          break;
        }
      case 5:
        switch(Stack) {
        case 0: 
          IoApicId = PC20_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC21_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC22_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC23_IOAPIC_ID;
          break;
        default:
          break;
        }
      case 6:
        switch(Stack) {
        case 0: 
          IoApicId = PC24_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC25_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC26_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC27_IOAPIC_ID;
          break;
        default:
          break;
        }
      case 7:
        switch(Stack) {
        case 0: 
          IoApicId = PC28_IOAPIC_ID;
          break;
        case 1: 
          IoApicId = PC29_IOAPIC_ID;
          break;
        case 2: 
          IoApicId = PC30_IOAPIC_ID;
          break;
        case 3: 
          IoApicId = PC31_IOAPIC_ID;
        default:
          break;
        }
#endif
      break;
      default:
        break;
      }
      if ((Socket == 0) && (Stack == 0)) {
        ProgramIoApicId ((mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].IoApicBase + 0x1000), IoApicId);
      } else {
        ProgramIoApicId (mIioUds->IioUdsPtr->PlatformData.IIO_resource[Socket].StackRes[Stack].IoApicBase, IoApicId);
      }
    }
  }

  AttemptToSetXhciMse ();

  return EFI_SUCCESS;
}

/**

  The PlatformPrepController() function can be used to notify the platform driver so that 
  it can perform platform-specific actions. No specific actions are required. 
  Several notification points are defined at this time. More synchronization points may be 
  added as required in the future. The PCI bus driver calls the platform driver twice for 
  every PCI controller-once before the PCI Host Bridge Resource Allocation Protocol driver 
  is notified, and once after the PCI Host Bridge Resource Allocation Protocol driver has 
  been notified. 
  This member function may not perform any error checking on the input parameters. It also 
  does not return any error codes. If this member function detects any error condition, it 
  needs to handle those errors on its own because there is no way to surface any errors to 
  the caller.  
    
  @param This          -  Pointer to the EFI_PCI_PLATFORM_PROTOCOL instance.  
  @param HostBridge    -  The associated PCI host bridge handle. 
  @param RootBridge    -  The associated PCI root bridge handle.
  @param PciAddress    -  The address of the PCI device on the PCI bus. 
  @param Phase         -  The phase of the PCI controller enumeration. 
  @param ChipsetPhase  -  Defines the execution phase of the PCI chipset driver. 
    
  @retval EFI_SUCCESS     -  The function completed successfully.
  @retval EFI_UNSUPPORTED -  Not supported.
  
**/
EFI_STATUS
EFIAPI
PlatformPrepController (
  IN  EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_HANDLE                                     RootBridge,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE   Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
{
  if (mPciPrivateData.RootBridgeHandle == NULL) {
    mPciPrivateData.RootBridgeHandle = RootBridge;
  }

  return EFI_SUCCESS;
}

/**

  Perform initialization by the phase indicated.

  @param This          -  Pointer to the EFI_PCI_PLATFORM_PROTOCOL instance.
  @param HostBridge    -  The associated PCI host bridge handle.
  @param Phase         -  The phase of the PCI controller enumeration.
  @param ChipsetPhase  -  Defines the execution phase of the PCI chipset driver.

  @retval EFI_SUCCESS  -  Must return with success.

**/
EFI_STATUS
EFIAPI
PhaseNotify (
  IN EFI_PCI_PLATFORM_PROTOCOL                       *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE  Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
{
  EFI_STATUS  Status;
  UINT8       i;
  UINT8       Stack;
#ifdef PC_HOOK
  EFI_HANDLE                            RootBridgeHandle;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *ACPIAddressDesc;
  UINT8                                 *Temp;
  VOID                                  *AcpiConfig;
  UINT64                                ResStatus;   
  BOOLEAN                               Mmio32OutOfResources = FALSE;
  BOOLEAN                               Mmio64OutOfResources = FALSE;
  BOOLEAN                               mReportStatusCode    = FALSE;
#endif

  if (ChipsetPhase == ChipsetEntry) {
    return EFI_SUCCESS;
  }
  //
  // If for multiple host bridges, need special consideration
  //
  switch (Phase) {

  case EfiPciHostBridgeBeginEnumeration:
    //
    // Pre-initialization before PCI bus enumeration
    // No bus number and no PCI resource
    //
    Status = gBS->LocateProtocol (
                    &gEfiPciRootBridgeIoProtocolGuid,
                    NULL,
                    &(mPciPrivateData.PciRootBridgeIo)
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->LocateProtocol (
                    &gEfiCpuIoProtocolGuid,
                    NULL,
                    &(mPciPrivateData.CpuIo)
                    );
    ASSERT_EFI_ERROR (Status);
    mPciPrivateData.Context.CpuIo = mPciPrivateData.CpuIo;

    DEBUG ((DEBUG_ERROR, "PCI Platform Pre-Initialization (Before bus scanning)\n"));
    PciPlatformEarlyInit ();
    break;

  case EfiPciHostBridgeEndBusAllocation:
    //
    // There are two rounds PCI bus scanning
    // First round will initilize the PCI hotplug device
    // Second round will be the final one
    //
    if (mPciPrivateData.BusAssignedTime == 0) {
      mPciPrivateData.PciEnumerationPhase = EfiPciEnumerationDeviceScanning;
      for (i = 0 ; i < MaxIIO ; i++) {
          if (mIioUds->IioUdsPtr->PlatformData.IIO_resource[i].Valid) {
              for(Stack = 0; Stack < MAX_IIO_STACK; Stack ++) {
                  PciTreeTraverse (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[i].StackBus[Stack]);
              }
          }
      }
      mPciPrivateData.BusAssignedTime++;
      DEBUG ((DEBUG_ERROR, "PCI Platform bus assigned\n"));
      /*//s3427069: Sunrise Ridge fails PCIE enumeration with IDT switch card - start
      //wait 1 sec to make sure HP capable slot power stablized
      gBS->Stall (1000000); /* 1 seconds */
      //s3427069 - end*/

    }
    break;

  case EfiPciHostBridgeBeginResourceAllocation:
    //
    // PCI bus number has been assigned, but resource is still empty
    //
    DEBUG ((DEBUG_ERROR, "PCI Platform Mid-Initialization (After bus number assignment)\n"));
    mPciPrivateData.PciEnumerationPhase = EfiPciEnumerationBusNumberAssigned;
    for (i = 0 ; i < MaxIIO ; i++) {
        if (mIioUds->IioUdsPtr->PlatformData.IIO_resource[i].Valid) {
            for(Stack = 0; Stack < MAX_IIO_STACK; Stack ++) {
                PciTreeTraverse (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[i].StackBus[Stack]);
            }
        }
    }
    //PciPlatformMidInit ();
    break;

  case EfiPciHostBridgeEndResourceAllocation:
    //
    // Resource enumeration is done.
    // Both bus number and resource have been assigned
    // Do any post initialization.
    //
    DEBUG ((DEBUG_ERROR, "PCI Platform Post-Initialization (After resource alloction)\n"));
    mPciPrivateData.PciEnumerationPhase = EfiPciEnumerationResourceAssigned;
    for (i = 0 ; i < MaxIIO ; i++) {
        if (mIioUds->IioUdsPtr->PlatformData.IIO_resource[i].Valid) {
            for(Stack = 0; Stack < MAX_IIO_STACK; Stack ++) {
                PciTreeTraverse (mIioUds->IioUdsPtr->PlatformData.CpuQpiInfo[i].StackBus[Stack]);
            }
        }
    }
    PciPlatformPostInit ();
    break;
// APTIOV_SERVER_OVERRIDE_RC_START
// AMI PCI bus driver handles out of resource condition
#if 0
#ifdef PC_HOOK
  case EfiPciHostBridgeAllocateResources:
    //
    // Resources are allocated per previously submitted requests for all the PCI Root
    // Bridges. These resource settings are returned on the next call to
    // GetProposedResources().
    //   
    DEBUG ((DEBUG_ERROR, "PCI Platform resources allocated\n"));

    Status = gBS->HandleProtocol (
                    HostBridge,
                    &gEfiPciHostBridgeResourceAllocationProtocolGuid,
                    (VOID **) &PciResAlloc
                    );
    ASSERT_EFI_ERROR (Status);
    RootBridgeHandle  = 0;
    while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {
      //
      // Get acpi resource node for all the resource types
      //
      AcpiConfig = NULL;

      Status = PciResAlloc->GetProposedResources (
                              PciResAlloc,
                              RootBridgeHandle,
                              &AcpiConfig
                              );

      if (EFI_ERROR (Status)) {
        return Status;
      }
   
      Temp = (UINT8 *) AcpiConfig;
      while (*Temp == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
        ACPIAddressDesc = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp;
        ResStatus = ACPIAddressDesc->AddrTranslationOffset;
        //
        // For MMIO resource
        //
        if (ACPIAddressDesc->ResType == 0) {

          //
          // For 32 bit MMIO out of resources case
          //
          if ((ACPIAddressDesc->AddrSpaceGranularity == 32) && (ACPIAddressDesc->SpecificFlag == 0) && (ResStatus != EFI_RESOURCE_SATISFIED)) {
            DEBUG ((DEBUG_ERROR, "Pci 32 bit Mmio allocated failed, Length = %lx\n", ACPIAddressDesc->AddrLen));
            if (!Mmio32OutOfResources) {
              Mmio32OutOfResources = TRUE;
            }
          }
          //
          // For 64 bit MMIO out of resources case
          //

          if ((ACPIAddressDesc->AddrSpaceGranularity == 64) && (ACPIAddressDesc->SpecificFlag == 0) && (ResStatus != EFI_RESOURCE_SATISFIED)) {  
            DEBUG ((DEBUG_ERROR, "Pci 64 bit Mmio allocated failed, Length = %lx\n", ACPIAddressDesc->AddrLen));
            if (!Mmio64OutOfResources) { 

              Mmio64OutOfResources = TRUE;
            }
          }
        }
        Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      }
      FreePool (AcpiConfig);
    }

      if (!mReportStatusCode) { 
        if(Mmio32OutOfResources || Mmio64OutOfResources) {
          REPORT_STATUS_CODE(
            EFI_ERROR_CODE|EFI_ERROR_MAJOR,
            EFI_IO_BUS_PCI | EFI_IOB_PCI_EC_OUT_OF_RESOURCES
          );
          mReportStatusCode = TRUE;
      } 
    }
    break;
#endif
#endif
// added our pcibus.c is calling with this phase.
  case EfiPciHostBridgeBeginBusAllocation :
  case EfiPciHostBridgeAllocateResources :
  case EfiPciHostBridgeSetResources :
	  return EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_RC_END

  default:
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}


/*VOID
PcieClearError(
  VOID
  )
{
  UINT8  i;
  UINT8  Bus;
  UINT8  Dev; 
  UINT32  Data;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  *PciAddressPtr;
  UINT64  PciAddress;

  PciAddressPtr = (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  *)&PciAddress;

  for (i = 0 ; i < MaxIIO ; i++) {
    if (mIioUds->IioUdsPtr->PlatformData.IIO_resource[i].Valid) {
      Bus = mIioUds->IioUdsPtr->PlatformData.IIO_resource[i].BusBase;
      for(Dev = 0; Dev < NUMBER_PORTS_PER_SOCKET; Dev++) { 
        PciAddress = EFI_PCI_ADDRESS (Bus, Dev, 0, R_PCIE_PORT_PXPLNKSTS);       
        mPciPrivateData.PciRootBridgeIo->Pci.Read (
                                             mPciPrivateData.PciRootBridgeIo,
                                             EfiPciWidthUint16,
                                             PciAddress,
                                             1,
                                             &Data
                                             );
        Data |= BIT15;
        mPciPrivateData.PciRootBridgeIo->Pci.Write (
                                             mPciPrivateData.PciRootBridgeIo,
                                             EfiPciWidthUint16,
                                             PciAddress,
                                             1,
                                             &Data
                                             );

        PciAddressPtr->Register         = 0;
        PciAddressPtr->Function         = 0;
        PciAddressPtr->Device           = Dev;
        PciAddressPtr->Bus              = Bus;
        PciAddressPtr->ExtendedRegister = R_PCIE_PORT_XPCORERRSTS;
        mPciPrivateData.PciRootBridgeIo->Pci.Read (
                                             mPciPrivateData.PciRootBridgeIo,
                                             EfiPciWidthUint16,
                                             PciAddress,
                                             1,
                                             &Data
                                             );

        Data |= BIT0;

        mPciPrivateData.PciRootBridgeIo->Pci.Write (
                                             mPciPrivateData.PciRootBridgeIo,
                                             EfiPciWidthUint16,
                                             PciAddress,
                                             1,
                                             &Data
                                             );
      }
    }
  }
}*/

