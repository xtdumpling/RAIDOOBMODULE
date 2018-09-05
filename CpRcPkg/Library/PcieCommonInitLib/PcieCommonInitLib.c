//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 2013 - 2017, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module:

**/
#include <Uefi/UefiBaseType.h>
#include "Library/PcieCommonInitLib.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/TimerLib.h>
#include "PcieCommonHelper.h"
#include "PciExpress.h"

#define PCILIB_TO_COMMON_ADDRESS(Address) \
        ((UINT64) ((((UINTN) ((Address>>20) & 0xff)) << 24) + (((UINTN) ((Address>>15) & 0x1f)) << 16) + (((UINTN) ((Address>>12) & 0x07)) << 8) + ((UINTN) (Address & 0xfff ))))

/**
  Parse top level of PCI topology.

  Platform developer needs to fill out the information of PCIE_LIB_TOPOLOGY_INF 
  and its sub-content in HostBridgeList and call PcieInfoTreePaser 
  to initialize whole PCI tree.

  @param [in]    TopologyInf    PCI topology.

  @return status

**/

EFI_STATUS
PcieInfoTreePaser (  
  PCIE_LIB_TOPOLOGY_INF         *TopologyInf
  )
{ 
  EFI_STATUS Status;

  if (TopologyInf->Revision != PCIE_LIB_TOPOLOGY_REVISION_1) {
    return EFI_INVALID_PARAMETER;
  }

  PciePlatformHookEvent (PcieInitStart, NULL);
  
  Status = PcieHostBridgeInit(TopologyInf->HostBridgeList);

  PciePlatformHookEvent (PcieInitEnd, NULL);

  return Status;
}

/**
  Init Hostbridge by node information list.

  Platform developer needs to fill out the information of PCIE_LIB_HOST_BRIDGE_INF 
  and its sub-content like root port information and Bifurcation information,
  and call PcieHostBridgeInit to initialize the given host bridge.
  
  @param [in]    HostBridgeInf    Host bridge information.

  @return status

**/
EFI_STATUS
PcieHostBridgeInit (  
  PCIE_LIB_HOST_BRIDGE_INF      *HostBridgeInf
  )
{
  EFI_STATUS            Status;
  USRA_ADDRESS Address;
  
  Status = EFI_SUCCESS;
  if( HostBridgeInf == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (HostBridgeInf->Revision) {
    case PCIE_LIB_HOSTBRIDGE_REVISION_END:
      // reach the end of host bridge table
      return EFI_SUCCESS;
    case PCIE_LIB_HOSTBRIDGE_REVISION_1:
      // handle revision 1 content
      DEBUG ((EFI_D_INFO, "Handle hostbridge %c%c%c%c.\n",HostBridgeInf->Name[0],HostBridgeInf->Name[1],HostBridgeInf->Name[2],HostBridgeInf->Name[3]));

      Address.Pcie.Bus     = HostBridgeInf->TempBusMin;
      Address.Pcie.Seg = HostBridgeInf->Segment;
      PciePlatformHookEvent (BeforeBifurcation, &Address);

      //
      // Bifurcation
      //
      Status = PcieHostBridgeBifurcation (&HostBridgeInf->BifInf);
      if ( Status != EFI_SUCCESS) {
        DEBUG ((EFI_D_ERROR, "Bifurcation failed in hostbridge %c%c%c%c.\n",HostBridgeInf->Name[0],HostBridgeInf->Name[1],HostBridgeInf->Name[2],HostBridgeInf->Name[3]));
      }

      PciePlatformHookEvent (AfterBifurcation, &Address);

// APTIOV_SERVER_OVERRIDE_RC_START : Resolve issue with PCI scan
      Status = PcieRootPortInit(HostBridgeInf->RootPortList,HostBridgeInf->TempBusMin,HostBridgeInf->TempBusMax, FixedPcdGetBool(PcdCleanTempBusAssignment));
// APTIOV_SERVER_OVERRIDE_RC_END : Resolve issue with PCI scan
      if ( Status != EFI_SUCCESS) {
        DEBUG ((EFI_D_ERROR, "Root port init failed in hostbridge %%c%c%c%c.\n",HostBridgeInf->Name[0],HostBridgeInf->Name[1],HostBridgeInf->Name[2],HostBridgeInf->Name[3]));
        return EFI_INVALID_PARAMETER;
      }

      break;
    default:
      // Can't handle it, just skip it
      break;
  }
  // next node
  return PcieHostBridgeInit(HostBridgeInf + 1);
 
}

/**
  Bifurcation according to PCIE_LIB_BIF_INF.

  Platform developer needs to fill out the information of PCIE_LIB_BIF_INF 
  as this way, the lib knows how to do Bifurcation
   
  @param [in]    BifInf    Bifurcation information.

  @return status

**/

EFI_STATUS
PcieHostBridgeBifurcation (  
  PCIE_LIB_BIF_INF              *BifInf
  )
{

  if(BifInf->Valid == 0) {
    return EFI_INVALID_PARAMETER;
  }

  PcieProgramOpTable (BifInf->OpTable, BifInf->OpTableSize);

  MicroSecondDelay(BifInf->Delay);

  return EFI_SUCCESS;
}

/**
  Init root bridge by node information list.

  Platform developer needs to fill out the information of PCIE_LIB_ROOT_PORT_INF 
  and feature list according to platform policy or user seletion 
  and call PcieRootPortInit to initialize the root port.
   
  @param [in]    RootPortInf    Root port information.
  @param [in]    TempBusMin     Min bus number that could be assigned to the root port and its down steams.
  @param [in]    TempBusMax     Max bus number that could be assigned to the root port and its down steams.
  @param [in]    CleanFlag      TRUE to clean and restore the temp bus assigned, 
                                FALSE to keep assigned bus number unchanged

  @return status

**/

EFI_STATUS
PcieRootPortInit (  
  PCIE_LIB_ROOT_PORT_INF        *RootPortInf,
  UINT8                         TempBusMin,
  UINT8                         TempBusMax,
  BOOLEAN                       CleanFlag  
  )
{ 
  UINT32                              Index;
  USRA_ADDRESS                        Address;
  PCI_TYPE00                          Pci;
  UINT8                               BusUsed;
  
  if(RootPortInf->Valid == 0) {
    // End of the table
    return EFI_SUCCESS;
  }

  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth32, RootPortInf->Segment, RootPortInf->Bus, RootPortInf->Dev, RootPortInf->Fun, 0);

  BusUsed = 0;

  //
  // Do nothing if no root port present
  //
  if (IsPciDevicePresent(&Address, &Pci) && RootPortInf->Type == PCIE_LIB_PCIE) {
     
    PciePlatformHookEvent (BeforePortInit, &Address);
  
    // go through features list
    Index = 0;
    while (RootPortInf->FeatureList[Index].Common.Type != PCIE_LIB_FEATURE_MAX) {
      switch(RootPortInf->FeatureList[Index].Common.Type) {
        case PCIE_LIB_FEATURE_CTO:
           PcieLibSetCto (&Address, &RootPortInf->FeatureList[Index]);
           break;
        case PCIE_LIB_FEATURE_SLOTCONTROL:
           PcieLibSetSlotControl (&Address, &RootPortInf->FeatureList[Index]);
           break;
         // TODO: Add more cases here
           
        default:
           // can't handle this type, just skip
           break;
      }
      Index++;
    }
  
    PcieInitRootPortDownstreamDevices (&Address, TempBusMin, TempBusMax,&BusUsed,RootPortInf->FeatureList,CleanFlag,NULL);
  
    PciePlatformHookEvent (AfterPortInit, &Address);
  
  }
  
  // Next node
  if(CleanFlag) {
    return PcieRootPortInit( RootPortInf + 1,TempBusMin,TempBusMax, CleanFlag );
  } else {
    return PcieRootPortInit( RootPortInf + 1,TempBusMin + BusUsed,TempBusMax, CleanFlag );
  }
  
}

/**
  Set ASPM to the endpoint and up stream according to feature node.

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    ASPM feature node.

  @return status

**/
EFI_STATUS
PcieLibSetAspmPath (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  )
{
  INTN                    Index;
  INTN                    EntryNumber;
  UINT32                  LinkCap;
  UINT32                  DevCap;
  USRA_ADDRESS   TempPciAddress;
  UINT8                   AspmSupport;
  UINT8                   L0LinkLatency;
  UINT8                   L1LinkLatency;
  UINT8                   L0DeviceAcceptLatency;
  UINT8                   L1DeviceAcceptLatency;
  UINT16                  L0L1Support;
  PCIE_LIB_ASPM_POLICY    AsmpPolicy;
  UINT16                  AndMask;

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetAspmPath() Start.\n"));
  
  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);
  //
  // Fill the PCIE link and device capability along the path
  //
  EntryNumber = PcieStack->Top;
  TempPciAddress.Attribute.AccessWidth = UsraWidth32;
  for (Index = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie     = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CAPABILITY_OFFSET;
    RegisterRead(&TempPciAddress, &DevCap);
    TempPciAddress.Pcie.Offset = PcieStack->PcieCapInfo[Index].CapPcieAddress.Offset + PCIE_LINK_CAPABILITY_OFFSET; 
    RegisterRead(&TempPciAddress, &LinkCap);

    PcieStack->PcieCapInfo[Index].PcieLnkCap = (UINT16)((LinkCap >> 10) & 0xFF);
    PcieStack->PcieCapInfo[Index].PcieDevCap = (UINT16)((DevCap >> 6) & 0x3F);
  }

  //
  // Check support for L0s and L1
  //
  AspmSupport   = 0x03;
  L0LinkLatency = 0;
  L1LinkLatency = 0;
  for (Index  = 0; Index < EntryNumber; Index++) {
    AspmSupport  &= PcieStack->PcieCapInfo[Index].PcieLnkCap;    
    L0LinkLatency = (UINT8)MAX ( ((PcieStack->PcieCapInfo[Index].PcieLnkCap >> 2) & 0x07) , L0LinkLatency );  
    L1LinkLatency = (UINT8)MAX ( ((PcieStack->PcieCapInfo[Index].PcieLnkCap >> 5) & 0x07) , L1LinkLatency );  
  }
  
  L0L1Support = 0;
  if (AspmSupport & BIT0) {
    //
    // Support L0s
    //
    L0DeviceAcceptLatency = (UINT8)(PcieStack->PcieCapInfo[EntryNumber-1].PcieDevCap >> 0) & 0x07;
    if (L0DeviceAcceptLatency >= L0LinkLatency) {
      L0L1Support |= BIT0;
    }
  }

  if (AspmSupport & BIT1) {
    //
    // Support L1
    //
    L1DeviceAcceptLatency = (UINT8)(PcieStack->PcieCapInfo[EntryNumber-1].PcieDevCap >> 3) & 0x07;
    if (L1DeviceAcceptLatency >= L1LinkLatency) {
      L0L1Support |= BIT1;      
    }
  }

  AsmpPolicy = ((PCIE_LIB_ASPM_INF *)FeatureList)->Policy;

  if (AsmpPolicy != PcieLibAspmAutoConfig) {
    L0L1Support &= (UINT16)AsmpPolicy;
  }
  
  if (L0L1Support > 0) {  
    if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
    TempPciAddress.Attribute.AccessWidth = UsraWidth16;
    for (Index = 0; Index < EntryNumber; Index++) {
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;;
      TempPciAddress.Pcie.Offset += PCIE_LINK_CONTROL_OFFSET;
      AndMask = (UINT16)(~(BIT0 | BIT1));
      RegisterModify(&TempPciAddress, &AndMask, &L0L1Support);
    }
  }
  return EFI_SUCCESS;
}

/**
  Set Extended Tag Field to the endpoint and up stream according to feature node.

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Extended Tag Field feature node.

  @return status

**/
EFI_STATUS
PcieLibSetExtendedTagField (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  )
{
  INTN                      Index;
  INTN                      EntryNumber;
  UINT32                    DevCap;
  USRA_ADDRESS              TempPciAddress;
  BOOLEAN                   EightBit_ExtendedTagField;
  PCIE_LIB_EXTTAGFIELD_INF  *ExtendedTagField;
  UINT16                    AndMask, OrMask;
  

  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetExtendedTagField() Start.\n"));
  
  //
  // Fill the PCIE link and device capability along the path
  //
  EntryNumber = PcieStack->Top;
  TempPciAddress.Attribute.AccessWidth = UsraWidth32;
  for (Index = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CAPABILITY_OFFSET;
    RegisterRead(&TempPciAddress, &DevCap);
    PcieStack->PcieCapInfo[Index].PcieDevCap = (UINT16)((DevCap >> 5) & 0x01);
  }

  ExtendedTagField = (PCIE_LIB_EXTTAGFIELD_INF *)FeatureList;

  //
  // determine whether root port to endpoint devices are capable of 8-bit tag field
  //
  EightBit_ExtendedTagField = TRUE;   //initially assume all the deivces support 8-bit tag
  for (Index  = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CAPABILITY_OFFSET;
    if( !PcieStack->PcieCapInfo[Index].PcieDevCap ){
      DEBUG((DEBUG_INFO, "  PCIe B:D:F[%x:%x:%x] - does not support 8-bit Tag field\n", TempPciAddress.Pcie.Bus, TempPciAddress.Pcie.Dev,TempPciAddress.Pcie.Func));
      EightBit_ExtendedTagField = FALSE;
    }
  }

  //
  // overwite with the selected platform policy for the root port (by user)
  // if 8-bit tag field is supported from the root to endpoint port
  //
  if (EightBit_ExtendedTagField == TRUE){
    if (ExtendedTagField->Flag != PCIE_LIB_EXTTAGFIELD_AUTO) {
      EightBit_ExtendedTagField = ExtendedTagField->Flag;
    }
  }
    
  if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
  TempPciAddress.Attribute.AccessWidth = UsraWidth16;
  for (Index = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL_OFFSET;
    AndMask = (UINT16)(~(BIT8));
    OrMask = (EightBit_ExtendedTagField << 8);
    RegisterModify(&TempPciAddress, &AndMask, &OrMask);
  }  
  return EFI_SUCCESS;

}


/**
  Set AtomicOp Routing to the endpoint and up stream according to feature node.

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    AtomicOp Routing Field feature node.

  @return status

**/
EFI_STATUS
PcieLibSetAtomicOpRouting (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  )
{
  INTN                      Index;
  INTN                      EntryNumber;
  UINT32                    DevCap2;
  USRA_ADDRESS              TempPciAddress;
  PCIE_LIB_ATOMICOP_INF     *AtomicOp;
  BOOLEAN                   AtomicOpSupport;
  UINT16                    AndMask, OrMask;
  
  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetAtomicOpRouting() Start.\n"));
  
  //
  // Fill the PCIE link and device capability along the path
  //
  EntryNumber = PcieStack->Top;
  TempPciAddress.Attribute.AccessWidth = UsraWidth32;
  for (Index = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CAPABILITY2_OFFSET;
    RegisterRead(&TempPciAddress, &DevCap2);
    PcieStack->PcieCapInfo[Index].PcieDevCap2 = (UINT16)((DevCap2 >> 6) & 0x01);
  }

  AtomicOp = (PCIE_LIB_ATOMICOP_INF *)FeatureList;
   
  if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
  TempPciAddress.Attribute.AccessWidth = UsraWidth16;
  for (Index = 0; Index < EntryNumber; Index++) {
    if (PcieStack->PcieCapInfo[Index].PcieDevCap2 != 0) {  //check for Atomicop Req capability
      if (AtomicOp->Flag == PCIE_LIB_ATOMICOP_ENABLE){
        AtomicOpSupport = 1;
      } else {
        AtomicOpSupport = 0;
      }
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
      TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL2_OFFSET;
      AndMask = (UINT16)(~(BIT6));
      OrMask = AtomicOpSupport << 6;
      RegisterModify(&TempPciAddress, &AndMask, &OrMask);
    }
    
    if ((Index != 0) && (PcieStack->PcieCapInfo[Index].PcieDevCap2 == 0)){ 
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index - 1].CapPcieAddress;
      TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL2_OFFSET;
      AndMask = (UINT16)(~(BIT7));
      OrMask = (1 << 7);
      RegisterModify(&TempPciAddress, &AndMask, &OrMask);
      }    
  }
  return EFI_SUCCESS;

}

/**
  Set LTR to the endpoint and up stream according to feature node.

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    LTR Routing Field feature node.

  @return status

**/
EFI_STATUS
PcieLibSetLtrPath (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  )
{
  INTN                      Index;
  INTN                      EntryNumber;
  UINT32                    DevCap2;
  USRA_ADDRESS              TempPciAddress;
  PCIE_LIB_LTR_INF          *LtrPolicy;
  UINT16                    AndMask = (UINT16)(~(BIT10));

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetLtrPath() Start.\n"));

  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);
  LtrPolicy = (PCIE_LIB_LTR_INF *)FeatureList;
  //
  // Fill the PCIE link and device capability along the path
  //
  EntryNumber = PcieStack->Top;
  TempPciAddress.Attribute.AccessWidth = UsraWidth32;
  if(LtrPolicy->Flag == PCIE_LIB_LTR_ENABLE) { //Enabled in setup
    for (Index = 0; Index < EntryNumber; Index++) {
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
      TempPciAddress.Pcie.Offset += PCIE_DEVICE_CAPABILITY2_OFFSET;
      RegisterRead(&TempPciAddress, &DevCap2);
      PcieStack->PcieCapInfo[Index].PcieDevCap2 = (UINT16)((DevCap2 >> 11) & 0x01);
      // Check if device supports the LTR mechanism
      if ((PcieStack->PcieCapInfo[Index].PcieDevCap2 & BIT11) == 0) {
        //
        // LTR is not supported in the current path therefore must exit since per PCIe Spec:
        // "It is not required that all Endpoints support LTR to permit
        // enabling LTR in those Endpoints that do support it".
        //
        DEBUG((DEBUG_INFO, "  PCIe B:D:F[%x:%x:%x] - does not support LTR\n", TempPciAddress.Pcie.Bus, TempPciAddress.Pcie.Dev,TempPciAddress.Pcie.Func));
        return EFI_SUCCESS;
      }
    }
    DevCap2 = 0x400;
  } else { // Disable
    DevCap2 = 0x00;
  }


  //
  //For every device in the heap, enable/disable LTR support based on the setup option.
  //
  if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
  TempPciAddress.Attribute.AccessWidth = UsraWidth16;
  for (Index = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL2_OFFSET;
    RegisterModify(&TempPciAddress, &AndMask, &DevCap2);
  } 

  return EFI_SUCCESS;

}



/**
  Set max payload to the endpoint and up stream according to feature node.

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Max payload feature node.

  @return status

**/
EFI_STATUS
PcieLibSetMaxPayloadPath (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  )
{
  INTN                      Index;
  INTN                      EntryNumber;
  UINT32                    DevCap;
  USRA_ADDRESS              TempPciAddress;
  UINT16                    MaxPayload;
  PCIE_LIB_MAXPAYLOAD_INF   *PlatformPayload;
  UINT16                    AndMask, OrMask;

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetMaxPayloadPath() Start.\n"));

  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);

  PlatformPayload = (PCIE_LIB_MAXPAYLOAD_INF *)FeatureList;

  //
  // Fill the PCIE link and device capability along the path
  //
  EntryNumber = PcieStack->Top;
  TempPciAddress.Attribute.AccessWidth = UsraWidth32;
  for (Index = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CAPABILITY_OFFSET;
    RegisterRead(&TempPciAddress, &DevCap);
    PcieStack->PcieCapInfo[Index].PcieDevCap =  (UINT16)(DevCap & 0x07);
  }

  //
  // Compute final MaxPayload
  //
  MaxPayload = 0x07;
  for (Index  = 0; Index < EntryNumber; Index++) {
    MaxPayload = MIN ( PcieStack->PcieCapInfo[Index].PcieDevCap, MaxPayload );
    if (PlatformPayload->Flag == PCIE_LIB_MAXPAYLOAD_OVERRIDE) {
      MaxPayload = MIN ( PlatformPayload->MaxPayload, MaxPayload );
    }
  }
  
  if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
  TempPciAddress.Attribute.AccessWidth = UsraWidth16;
  for (Index = 0; Index < EntryNumber; Index++) {
    TempPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL_OFFSET;
    AndMask = (UINT16)(~(BIT5 | BIT6 | BIT7));
    OrMask = MaxPayload << 5;
    RegisterModify(&TempPciAddress, &AndMask, &OrMask);
  }
  return EFI_SUCCESS;
}

/**
  Set max read request size to the endpoint and up stream 
  according to feature node. 

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Max payload feature node.

  @return status

**/
EFI_STATUS
PcieLibSetMaxReadRequestSize (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
)
{
  INTN                      EntryNumber;
  USRA_ADDRESS              TempPciAddress;
  PCIE_LIB_MRRS_INF        *MrrsPolicy;
  UINT16                    AndMask;
  PCIE_DEV_CTRL             OrMask = { 0 };

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetMaxReadRequestSize() Start.\n"));

  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);

  MrrsPolicy = (PCIE_LIB_MRRS_INF *)FeatureList;

  EntryNumber = PcieStack->Top;

  if (MrrsPolicy->Flag == PCIE_LIB_MRRS_OVERRIDE) {
      if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
      OrMask.Bits.MaxReadRequestSize = 7;
      TempPciAddress.Attribute.AccessWidth = UsraWidth16;
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[EntryNumber - 1].CapPcieAddress;
      TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL_OFFSET;
      AndMask = (UINT16)(~(OrMask.Word));
      OrMask.Bits.MaxReadRequestSize = MrrsPolicy->MaxReadReqSize & 7;
      RegisterModify(&TempPciAddress, &AndMask, &OrMask.Word);
  }

  return EFI_SUCCESS;
}

/**
  Pcie standard feature called Enable Relaxed Ordering(ENRO).

  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Max payload feature node.

  @return status

**/
EFI_STATUS
PcieLibSetRelaxedOrdering (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
)
{
  INTN                      EntryNumber;
  USRA_ADDRESS              TempPciAddress;
  PCIE_LIB_RELAXORDER_INF  *RelaxedOrder;
  UINT16                    AndMask;
  PCIE_DEV_CTRL             OrMask = { 0 };

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  DEBUG ((EFI_D_INFO, "PcieLibSetRelaxedOrdering() Start.\n"));

  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);

  RelaxedOrder = (PCIE_LIB_RELAXORDER_INF *)FeatureList;

  EntryNumber = PcieStack->Top;

  if (RelaxedOrder->Type == PCIE_LIB_FEATURE_RELAXORDER) {
      if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
      OrMask.Bits.EnableRelaxedOrdering = 1;
      TempPciAddress.Attribute.AccessWidth = UsraWidth16;
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[EntryNumber - 1].CapPcieAddress;
      TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL_OFFSET;
      AndMask = (UINT16)(~OrMask.Word);
      OrMask.Bits.EnableRelaxedOrdering = RelaxedOrder->Flag & BIT0;
      RegisterModify(&TempPciAddress, &AndMask, &OrMask.Word);
  }

  return EFI_SUCCESS;
}

/**
  Pcie standard feature called Enable No Snoop (ENOSNP)

  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Max payload feature node.

  @return status

**/
EFI_STATUS
PcieLibSetNoSnoop (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
)
{
  INTN                      EntryNumber;
  USRA_ADDRESS              TempPciAddress;
  PCIE_LIB_NOSNOOP_INF      *NoSnoop;
  UINT16                    AndMask;
  PCIE_DEV_CTRL             OrMask = { 0 };

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetNoSnoop() Start.\n"));

  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);

  NoSnoop = (PCIE_LIB_NOSNOOP_INF *)FeatureList;

  EntryNumber = PcieStack->Top;

  if (NoSnoop->Type == PCIE_LIB_FEATURE_NOSNOOP) {
      if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
      OrMask.Bits.EnableSnoopNotRequired = 1;
      TempPciAddress.Attribute.AccessWidth = UsraWidth16;
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[EntryNumber - 1].CapPcieAddress;
      TempPciAddress.Pcie.Offset += PCIE_DEVICE_CONTROL_OFFSET;
      AndMask = (UINT16)(~OrMask.Word);
      OrMask.Bits.EnableSnoopNotRequired = NoSnoop->Flag & BIT0;
      RegisterModify(&TempPciAddress, &AndMask, &OrMask.Word);
  }

  return EFI_SUCCESS;
}

/**
  Pcie standard feature called Extended Synch (ES)

  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress    PCI address of the end point.
  @param [in]    FeatureList    Max payload feature node.

  @return status

**/
EFI_STATUS
PcieLibSetExtendedSynch (
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
)
{
  INTN                      EntryNumber;
  USRA_ADDRESS              TempPciAddress;
  PCIE_LIB_EXTSYNC_INF      *ExtendedSynch;
  UINT16                    AndMask;
  PCIE_LNK_CTRL             OrMask = { 0 };

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieLibSetExtendedSynch() Start.\n"));

  USRA_ZERO_ADDRESS_TYPE(TempPciAddress, AddrTypePCIE);

  ExtendedSynch = (PCIE_LIB_EXTSYNC_INF *)FeatureList;

  EntryNumber = PcieStack->Top;

  if (ExtendedSynch->Type == PCIE_LIB_FEATURE_EXTSYNC) {
      if (FeaturePcdGet (PcdUsraSupportS3)) TempPciAddress.Attribute.S3Enable = USRA_ENABLE;
      OrMask.Bits.ExtendedSynch = 1;
      TempPciAddress.Attribute.AccessWidth = UsraWidth16;
      TempPciAddress.Pcie = PcieStack->PcieCapInfo[EntryNumber - 1].CapPcieAddress;
      TempPciAddress.Pcie.Offset += PCIE_LINK_CONTROL_OFFSET;
      AndMask = (UINT16)(~OrMask.Word);
      OrMask.Bits.ExtendedSynch = ExtendedSynch->Flag & BIT0;
      RegisterModify(&TempPciAddress, &AndMask, &OrMask.Word);
  }

  return EFI_SUCCESS;
}

/**
  Set slot control features on root port.

  Only call this function on the root port.
   
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Slot control feature node.

  @return status

**/
EFI_STATUS
PcieLibSetSlotControl(
  USRA_ADDRESS                     *Address,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  )
{
  UINT8                     CapPtr;
  PCIE_SLOT_CTRL            SlotControl;
  PCIE_CAP                  PcieCap;
  PCIE_LIB_SLOT_CONTROL_INF *SlotInf;

  DEBUG ((EFI_D_INFO, "PcieLibSetSlotControl() Start.\n"));

  SlotInf = (PCIE_LIB_SLOT_CONTROL_INF *)FeatureList;

  CapPtr = PcieFindCapId (Address, EFI_PCI_CAPABILITY_ID_PCIEXP);

  Address->Pcie.Offset = CapPtr + PCIE_CAPABILITY_OFFSET;
  if (FeaturePcdGet (PcdUsraSupportS3)) Address->Attribute.S3Enable = USRA_ENABLE;
  Address->Attribute.AccessWidth = UsraWidth16;
  RegisterRead(Address, &PcieCap.Word);
  
  if (!SlotInf->SlotImplemented) {
    return EFI_SUCCESS;
  } else {
    PcieCap.Bits.SlotImplemented = 1;
    RegisterWrite(Address, &PcieCap.Word);
  }
  
  Address->Pcie.Offset = CapPtr + PCIE_SLOT_CONTROL_OFFSET;
  RegisterRead(Address, &SlotControl.Word);

  if (SlotInf->AttentionButton == PcieLibSlotFlagEnable) {
    SlotControl.Bits.AttentionButtonEnable = 1;
  } else if (SlotInf->AttentionButton == PcieLibSlotFlagDisable ) {
    SlotControl.Bits.AttentionButtonEnable = 0;
  }

  if (SlotInf->PowerFaultDetect == PcieLibSlotFlagEnable) {
    SlotControl.Bits.PowerFaultDetectedEnable = 1;
  } else if (SlotInf->PowerFaultDetect == PcieLibSlotFlagDisable ) {
    SlotControl.Bits.PowerFaultDetectedEnable = 0;
  }

  if (SlotInf->MRLSensorChanged == PcieLibSlotFlagEnable) {
    SlotControl.Bits.MrlSensorChangedEnable = 1;
  } else if (SlotInf->MRLSensorChanged == PcieLibSlotFlagDisable ) {
    SlotControl.Bits.MrlSensorChangedEnable = 0;
  }
  
  if (SlotInf->PresenceDetect == PcieLibSlotFlagEnable) {
    SlotControl.Bits.PresenceDetectChangedEnable = 1;
  } else if (SlotInf->PresenceDetect == PcieLibSlotFlagDisable ) {
    SlotControl.Bits.PresenceDetectChangedEnable = 0;
  }

  if (SlotInf->CommandCompletedInt == PcieLibSlotFlagEnable) {
    SlotControl.Bits.CommandCompletedInterruptEnable = 1;
  } else if (SlotInf->CommandCompletedInt == PcieLibSlotFlagDisable ) {
    SlotControl.Bits.CommandCompletedInterruptEnable = 0;
  }
  if (SlotInf->HotPlugInt == PcieLibSlotFlagEnable) {
    SlotControl.Bits.HotplugInterruptEnable = 1;
  } else if (SlotInf->HotPlugInt == PcieLibSlotFlagDisable ) {
    SlotControl.Bits.HotplugInterruptEnable = 0;
  }

  switch (SlotInf->AttentionIndicator) {
    case PcieLibSlotFlagOn:
      SlotControl.Bits.AttentionIndicatorControl = 1;
      break;
    case PcieLibSlotFlagOff:
      SlotControl.Bits.AttentionIndicatorControl = 2;
      break;
    case PcieLibSlotFlagBlink:
      SlotControl.Bits.AttentionIndicatorControl = 3;
      break;
  }

  switch (SlotInf->PowerIndicator) {
    case PcieLibSlotFlagOn:
      SlotControl.Bits.PowerIndicatorControl = 1;
      break;
    case PcieLibSlotFlagOff:
      SlotControl.Bits.PowerIndicatorControl = 2;
      break;
    case PcieLibSlotFlagBlink:
      SlotControl.Bits.PowerIndicatorControl = 3;
      break;
  }

  if (SlotInf->PowerController == PcieLibSlotFlagOn) {
    SlotControl.Bits.PowerControllerControl = 0;
  } else if (SlotInf->PowerController == PcieLibSlotFlagOff ) {
    SlotControl.Bits.PowerControllerControl = 1;
  }
  
  if (SlotInf->DataLinkLayerState == PcieLibSlotFlagEnable) {
    SlotControl.Bits.DataLinkLayerStateChangedEnable = 1;
  } else if (SlotInf->DataLinkLayerState == PcieLibSlotFlagDisable ) {
    SlotControl.Bits.DataLinkLayerStateChangedEnable = 0;
  }

  RegisterWrite(Address, &SlotControl.Word);


  
  return EFI_SUCCESS;
}

/**
  Set complete timeout features on root port.

  Only call this function on the root port.
   
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Complete timeout feature node.

  @return status

**/
EFI_STATUS
PcieLibSetCto(
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  )
{
  UINT8                               CapPtr;
  PCIE_DEV_CAP2                       DevCap2;
  PCIE_DEV_CTRL2                      DevCtl2;
  PCIE_LIB_COMPLETION_TIMEOUT_POLICY  CtoPolicy;


  DEBUG ((EFI_D_INFO, "PcieLibSetCto() on B:D:F[%x:%x:%x] Start.\n",PcieAddress->Pcie.Bus,PcieAddress->Pcie.Dev,PcieAddress->Pcie.Func));
  CapPtr = PcieFindCapId (PcieAddress, EFI_PCI_CAPABILITY_ID_PCIEXP);

  PcieAddress->Attribute.AccessWidth = UsraWidth8;

  PcieAddress->Pcie.Offset = CapPtr + PCIE_DEVICE_CAPABILITY2_OFFSET;
  RegisterRead(PcieAddress, &DevCap2.Byte);

  PcieAddress->Pcie.Offset = CapPtr + PCIE_DEVICE_CONTROL2_OFFSET;

  RegisterRead(PcieAddress, &DevCtl2.Byte);

  CtoPolicy = ((PCIE_LIB_CTO_INF *)FeatureList)->Policy;

  switch (CtoPolicy) {
    case PcieLibCompletionTO_Disabled:
        if (DevCap2.Bits.CtoDiableSupported) {
          DevCtl2.Bits.CtoDiable = 1;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_Default:
        DevCtl2.Bits.CtoDiable = 0;
        DevCtl2.Bits.CtoValue = 0;
        break;
    case PcieLibCompletionTO_50_100us:
        // Check if range A supported
        if (DevCap2.Bits.CtoRangeSupported & BIT0) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 1;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_1_10ms:
        // Check if range A supported
        if (DevCap2.Bits.CtoRangeSupported & BIT0) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 2;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_16_55ms:
        // Check if range B supported
        if (DevCap2.Bits.CtoRangeSupported & BIT1) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 5;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_65_210ms:
        // Check if range B supported
        if (DevCap2.Bits.CtoRangeSupported & BIT1) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 6;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_260_900ms:
        // Check if range C supported
        if (DevCap2.Bits.CtoRangeSupported & BIT2) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 9;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_1_3P5s:
        // Check if range C supported
        if (DevCap2.Bits.CtoRangeSupported & BIT2) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 10;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_4_13s:
        // Check if range D supported
        if (DevCap2.Bits.CtoRangeSupported & BIT3) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 13;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    case PcieLibCompletionTO_17_64s:
        // Check if range D supported
        if (DevCap2.Bits.CtoRangeSupported & BIT3) {
          DevCtl2.Bits.CtoDiable = 0;
          DevCtl2.Bits.CtoValue = 14;
        } else {
          return EFI_UNSUPPORTED;
        }
        break;
    default:
        return EFI_UNSUPPORTED;
  }

  if (FeaturePcdGet (PcdUsraSupportS3)) PcieAddress->Attribute.S3Enable = USRA_ENABLE;
  RegisterWrite(PcieAddress, &DevCtl2.Byte);
  
  return EFI_SUCCESS;
}

/**
  Set common clock to the endpoint and up stream according to feature node.

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Common clock feature node.

  @return status

**/
EFI_STATUS
PcieSetCommonClock (
  PCIE_LIB_STACK                    *PcieStack,
  USRA_ADDRESS                      *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE    *FeatureList
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    ParentClkConfig;
  UINT16                    ChildClkConfig;
  UINT16                    RegData16; 
  UINTN                     EntryNumber;
  UINT16                    Var16;
  UINT16                    Data16;
  UINTN                     ScriptAddress;
  USRA_ADDRESS              ParentPciAddress, ChildPciAddress;
  UINT16                    OrMask = B_PCIE_LINK_CONTROL_COMMON_CLOCK;

  ASSERT ((PcieStack != NULL) && (PcieStack->Top > 0) && (!(PcieStack->Top & 1)));
  if (PcieStack == NULL) {
    return RETURN_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "PcieSetCommonClock() on B:D:F[%x:%x:%x] Start.\n",PcieAddress->Pcie.Bus, PcieAddress->Pcie.Dev,PcieAddress->Pcie.Func));
  EntryNumber = PcieStack->Top;

  USRA_ZERO_ADDRESS_TYPE(ParentPciAddress, AddrTypePCIE);
  USRA_ZERO_ADDRESS_TYPE(ChildPciAddress, AddrTypePCIE);

  ParentPciAddress.Attribute.AccessWidth = UsraWidth16;
  ChildPciAddress.Attribute.AccessWidth = UsraWidth16;

  for (Index = 0; Index < EntryNumber-1; Index++){
    //
    // get the Parent and Child device info 
    //
    ParentPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    ParentPciAddress.Pcie.Offset += PCIE_LINK_STATUS_OFFSET;
    RegisterRead(&ParentPciAddress, &ParentClkConfig);

    ChildPciAddress.Pcie  = PcieStack->PcieCapInfo[Index+1].CapPcieAddress;
    ChildPciAddress.Pcie.Offset += PCIE_LINK_STATUS_OFFSET;
    RegisterRead(&ChildPciAddress, &ChildClkConfig);
    
    if (FeaturePcdGet (PcdUsraSupportS3)) {
      ParentPciAddress.Attribute.S3Enable = USRA_ENABLE;
      ChildPciAddress.Attribute.S3Enable = USRA_ENABLE;
    }

    //
    //check the Slot Clock Configuration Bit in Parent  and child device,if yes, set common clock bit 
    //
    if(((ParentClkConfig & B_PCIE_LINK_STATUS_SLOT_CLOCK)!= 0) && ((ChildClkConfig & B_PCIE_LINK_STATUS_SLOT_CLOCK)!= 0)){      
      ParentPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
      ParentPciAddress.Pcie.Offset += PCIE_LINK_CONTROL_OFFSET;
      ChildPciAddress.Pcie  = PcieStack->PcieCapInfo[Index+1].CapPcieAddress;
      ChildPciAddress.Pcie.Offset += PCIE_LINK_CONTROL_OFFSET;
      OrMask = B_PCIE_LINK_CONTROL_COMMON_CLOCK;
      RegisterModify(&ParentPciAddress, NULL, &OrMask);
      RegisterModify(&ChildPciAddress, NULL, &OrMask);
    }else{
        Status = EFI_UNSUPPORTED;
      return Status;
    } 
  
    //  
    // If common clock supported, retrain,otherwise, return halt in loop.
    //
    OrMask = B_PCIE_LINK_CONTROL_RETRAIN_LINK;
    RegisterModify(&ParentPciAddress, NULL, &OrMask);
    
    //
    // Polling link training status to 0
    //
    ParentPciAddress.Pcie = PcieStack->PcieCapInfo[Index].CapPcieAddress;
    ParentPciAddress.Pcie.Offset += PCIE_LINK_STATUS_OFFSET;
    do{
        RegisterRead(&ParentPciAddress, &RegData16);
        RegData16 &= B_PCIE_LINK_STATUS_LINK_TRAINING;
    }while (RegData16 != 0);
  
    //
    // S3 boot script polling link training status
    //
    Data16 = 0;
    ScriptAddress = PCILIB_TO_COMMON_ADDRESS (ParentPciAddress.Attribute.RawData32[0]); 
    Var16  = B_PCIE_LINK_STATUS_LINK_TRAINING;
    S3BootScriptSavePciPoll(
      S3BootScriptWidthUint16, 
      ScriptAddress,  // Need to update to support Segment
      &Data16, 
      &Var16, 
      10000
    );
    
  }
  Status = EFI_SUCCESS;
 
  return Status;
}

/**
  Initializes the root port and its down stream devices.

  @param [in]    PcieStack        Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress       PCI address of the root port or bridge/switch.
  @param [in]    FeatureList      feature node of the root port.
  @param [in]    TempBusNumberMin Minimal temp bus number that can be assigned to the root port (as secondary
                                  bus number) and its down stream switches
  @param [in]    TempBusNumberMax Maximal temp bus number that can be assigned to the root port (as subordinate
                                  bus number) and its down stream switches
  @param [in]    CleanFlag        TRUE to clean and restore the temp bus assigned, 
                                  FALSE to keep assigned bus number unchanged

  @return status

**/
#pragma optimize("",off)
BOOLEAN
PcieInitRootPortDownstreamDevices (
  USRA_ADDRESS                   *PcieAddress,
  UINT8                          TempBusNumberMin,
  UINT8                          TempBusNumberMax,
  UINT8                          *TempBusNumberUsed,
  PCIE_LIB_ROOTPORT_FEATURE_NODE *FeatureList,
  BOOLEAN                        CleanFlag,
  PCIE_LIB_STACK                 *PcieStack
  )
{
  PCI_TYPE00                          Pci;
  PCI_TYPE01                          PciBridge;
  USRA_ADDRESS                        EndPointPciAddress;
  UINT8                               Data8;
  UINT8                               BusNumUsed, BusNumEndpointUsed, DevIndex,FuncIndex, EndPointFunctions;
  UINT8                               ChildCapPtr, ParentCapPtr;
  PCIE_CAP                            ParentCap;
  PCIE_CAP                            ChildCap;
  BOOLEAN                             IsEndPoint;
  BOOLEAN                             IsSwitch;
  UINT32                              FeatureIndex;
  UINT32                              AndMask, OrMask;
// APTIOV_SERVER_OVERRIDE_RC_START : Resolve issue with PCI scan
  USRA_ADDRESS                        TempPcieAddress;

  USRA_ADDRESS_COPY(&TempPcieAddress, PcieAddress);
// APTIOV_SERVER_OVERRIDE_RC_END : Resolve issue with PCI scan

  if (TempBusNumberMin >= TempBusNumberMax){
    DEBUG ((EFI_D_ERROR, "Root Port initialization could not be completed, insufficient number of Buses: TempBusNumberMax = 0x%x  TempBusNumberMin = 0x%x \n", TempBusNumberMax, TempBusNumberMin));
    return FALSE;
  }

  if (TempBusNumberUsed != NULL) {
    *TempBusNumberUsed = 0;
  }

  if (!IsPciDevicePresent(PcieAddress, &Pci)) {
    return FALSE;
  }

  if (!IS_PCI_BRIDGE(&Pci)) {
    return FALSE;
  }   

  ParentCapPtr = PcieFindCapId(PcieAddress, EFI_PCI_CAPABILITY_ID_PCIEXP);

// APTIOV_SERVER_OVERRIDE_RC_START : Resolve issue with PCI scan
//  PciReadBuffer((PcieAddress->Attribute.RawData32[0] & 0x0fffffff), sizeof (PCI_TYPE01), &PciBridge);
  TempPcieAddress.Attribute.AccessWidth = UsraWidth32;
  TempPcieAddress.Attribute.AddrType    = AddrTypePCIEBLK;
  TempPcieAddress.PcieBlk.Offset = 0;
  TempPcieAddress.PcieBlk.Count  = sizeof (PCI_TYPE01)/ sizeof(UINT32);
  RegisterRead(&TempPcieAddress, &PciBridge);
// APTIOV_SERVER_OVERRIDE_RC_END : Resolve issue with PCI scan

  PcieAddress->Pcie.Offset = PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET;
  PcieAddress->Attribute.AccessWidth = UsraWidth32;
  AndMask = 0xFF0000FF;
  OrMask = (TempBusNumberMin << 8) | (TempBusNumberMax << 16);
  if (FeaturePcdGet (PcdUsraSupportS3)) PcieAddress->Attribute.S3Enable = USRA_ENABLE;
  RegisterModify(PcieAddress, &AndMask, &OrMask);

  DEBUG ((EFI_D_INFO, "Temp bus assigned to B:D:F[%x:%x:%x], Min:Max[%x:%x].\n",
                PcieAddress->Pcie.Bus,PcieAddress->Pcie.Dev,PcieAddress->Pcie.Func,TempBusNumberMin,TempBusNumberMax));
  BusNumUsed  = 1;
  PcieAddress->Pcie.Offset = ParentCapPtr + PCIE_CAPABILITY_OFFSET;
  PcieAddress->Attribute.AccessWidth = UsraWidth16;
  RegisterRead(PcieAddress, &ParentCap.Word);

  if(ParentCap.Bits.DevicePortType == V_PCIE_LINK_CAPABILITY_PORT_TYPE_ROOT_PORT) {
    //
    // reset stack if doing root port
    // 
    PcieStack = AllocateZeroPool(sizeof(PCIE_LIB_STACK));
    ASSERT (PcieStack != NULL);
    if (PcieStack == NULL) {
      return FALSE;
    }
    ResetPcieStack (PcieStack);
    PcieAddress->Pcie.Offset = ParentCapPtr;
    PushPcieStack (PcieStack, &PcieAddress->Pcie);
  }


  //
  // Now go to endpoint
  // 
  for (DevIndex = 0; DevIndex <= PCI_MAX_DEVICE; DevIndex++) {
    //
    // Check if EndPoint device is Multi-Function Device
    //
    USRA_PCIE_SEG_ADDRESS(EndPointPciAddress, UsraWidth8, PcieAddress->Pcie.Seg, TempBusNumberMin, DevIndex, 0, PCI_HEADER_TYPE_OFFSET);
    RegisterRead(&EndPointPciAddress, &Data8);
    if (Data8 & HEADER_TYPE_MULTI_FUNCTION) {
      //
      // If multi-function Device, check function 0-7
      //
      EndPointFunctions = PCI_MAX_FUNC;
    } else {
      //
      // Otherwise, check function 0 only
      //
      EndPointFunctions = 0;
    }

    for (FuncIndex = 0; FuncIndex <= EndPointFunctions; FuncIndex++) {
      USRA_PCIE_SEG_ADDRESS(EndPointPciAddress, UsraWidth16, PcieAddress->Pcie.Seg, TempBusNumberMin, DevIndex, FuncIndex, 0);
      if (IsPciDevicePresent(&EndPointPciAddress, &Pci)) {
        ChildCapPtr = PcieFindCapId(&EndPointPciAddress, EFI_PCI_CAPABILITY_ID_PCIEXP);
        DEBUG ((EFI_D_INFO, "   ChildCapPtr=%x on B:D:F[%x:%x:%x]\n",ChildCapPtr,
                                 EndPointPciAddress.Pcie.Bus,EndPointPciAddress.Pcie.Dev,EndPointPciAddress.Pcie.Func));
        EndPointPciAddress.Attribute.AccessWidth = UsraWidth16;
        EndPointPciAddress.Pcie.Offset = ChildCapPtr + PCIE_CAPABILITY_OFFSET;
        RegisterRead(&EndPointPciAddress, &ChildCap.Word);

        IsEndPoint = FALSE;
        IsSwitch   = FALSE;
        if ( ChildCap.Bits.DevicePortType == V_PCIE_LINK_CAPABILITY_PORT_TYPE_ENDPOINT || \
             ChildCap.Bits.DevicePortType == V_PCIE_LINK_CAPABILITY_PORT_TYPE_LEGACY_ENDPOINT ) {
          IsEndPoint = TRUE;
        } 
        
        if ( (ChildCap.Bits.DevicePortType == V_PCIE_LINK_CAPABILITY_PORT_TYPE_UP_PORT) || \
             (ChildCap.Bits.DevicePortType == V_PCIE_LINK_CAPABILITY_PORT_TYPE_DOWN_PORT)) {  
          if (IS_PCI_BRIDGE(&Pci)) {
            IsSwitch = TRUE;
          }
        }
        if (IsSwitch || IsEndPoint) {
          EndPointPciAddress.Pcie.Offset = ChildCapPtr;
          PushPcieStack (PcieStack, &EndPointPciAddress.Pcie);
          if (IsSwitch) {
            if(PcieInitRootPortDownstreamDevices(&EndPointPciAddress, TempBusNumberMin + BusNumUsed, TempBusNumberMax,
                                                  &BusNumEndpointUsed,FeatureList,CleanFlag,PcieStack)) {
               BusNumUsed += BusNumEndpointUsed;
            }
          } else {
           // go through features list
           FeatureIndex = 0;
           if(FeatureList != NULL) {
            while (FeatureList[FeatureIndex].Common.Type != PCIE_LIB_FEATURE_MAX) {
              switch(FeatureList[FeatureIndex].Common.Type) {
                case PCIE_LIB_FEATURE_ASPM:
                   PcieLibSetAspmPath (PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                   break;
                case PCIE_LIB_FEATURE_MAXPAYLOAD:
                   PcieLibSetMaxPayloadPath (PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                   break; 
                case PCIE_LIB_FEATURE_COMMONCLOCK:
                   PcieSetCommonClock(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                   break;
                case PCIE_LIB_FEATURE_EXTTAGFIELD:
                   PcieLibSetExtendedTagField(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                   break;
                case PCIE_LIB_FEATURE_ATOMICOP:
                   PcieLibSetAtomicOpRouting(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                   break;
                case PCIE_LIB_FEATURE_LTR:
                   PcieLibSetLtrPath(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                   break;
                case PCIE_LIB_FEATURE_MRRS:
                  PcieLibSetMaxReadRequestSize(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                  break;
                 // TODO: Add more cases here
                case PCIE_LIB_FEATURE_RELAXORDER:
                  PcieLibSetRelaxedOrdering(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                  break;
                case PCIE_LIB_FEATURE_NOSNOOP:
                  PcieLibSetNoSnoop(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                  break;
                case PCIE_LIB_FEATURE_EXTSYNC:
                  PcieLibSetExtendedSynch(PcieStack, &EndPointPciAddress, &FeatureList[FeatureIndex]);
                  break;
                default:
                   // can't handle this type, just skip
                   break;
               }
               FeatureIndex++;
              }
            }
          }
          PopPcieStack (PcieStack);
        }

        if ((TempBusNumberMin + BusNumUsed - 1) >= TempBusNumberMax) {
          DEBUG ((EFI_D_ERROR, "Root Port initialization could not be completed, insufficient number of Buses: TempBusNumberMax = 0x%x  TempBusNumberMin = 0x%x \n", TempBusNumberMax, TempBusNumberMin));
          return FALSE;  
        }
      }
    }  
  }

  if( !CleanFlag ){
    //
    // Fix the SubordinateBus
    //
    PcieAddress->Attribute.AccessWidth = UsraWidth32;
    OrMask = (TempBusNumberMin << 8) | ((TempBusNumberMin + BusNumUsed - 1) << 16);
    PcieAddress->Pcie.Offset = PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET;

    RegisterModify(PcieAddress, &AndMask, &OrMask);

    DEBUG ((EFI_D_INFO, "  Fix the temp subordinateBus assigned to B:D:F[%x:%x:%x], Min:Max[%x:%x].\n",
                PcieAddress->Pcie.Bus,PcieAddress->Pcie.Dev,PcieAddress->Pcie.Func,TempBusNumberMin,TempBusNumberMin + BusNumUsed - 1));
    
    if (TempBusNumberUsed != NULL) {
      *TempBusNumberUsed = BusNumUsed;
    }
  } else {
    //
    // Clean up the bus number
    //
    PcieAddress->Pcie.Offset = PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET;
    PcieAddress->Attribute.AccessWidth = UsraWidth32;
    OrMask = (PciBridge.Bridge.SecondaryBus << 8) | (PciBridge.Bridge.SubordinateBus << 16);
    RegisterModify(PcieAddress, &AndMask, &OrMask);
    DEBUG ((EFI_D_INFO, "  Clean temp bus to B:D:F[%x:%x:%x].\n",PcieAddress->Pcie.Bus,PcieAddress->Pcie.Dev,PcieAddress->Pcie.Func));
    if (TempBusNumberUsed != NULL) {
      *TempBusNumberUsed = 0;
    }
  }

  if(ParentCap.Bits.DevicePortType == V_PCIE_LINK_CAPABILITY_PORT_TYPE_ROOT_PORT) {
    FreePool(PcieStack);
  }
  return TRUE;
}

#pragma optimize("",on)

