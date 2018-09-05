//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module:

**/

#ifndef __PCIE_COMMON_INIT_LIB_H__
#define __PCIE_COMMON_INIT_LIB_H__

#include <Library/PciePlatformHookLib.h>
#include <Library/UsraAccessApi.h>


typedef enum _PCIE_LIB_FEATURE_TYPE {
  PCIE_LIB_FEATURE_INVALID       = 0,
  PCIE_LIB_FEATURE_SKIP          = 1,
  PCIE_LIB_FEATURE_VPP           = 2,
  PCIE_LIB_FEATURE_ASPM          = 3,
  PCIE_LIB_FEATURE_CTO           = 4,
  PCIE_LIB_FEATURE_MAXPAYLOAD    = 5,
  PCIE_LIB_FEATURE_COMMONCLOCK   = 6,
  PCIE_LIB_FEATURE_SLOTCONTROL   = 7,
  PCIE_LIB_FEATURE_EXTTAGFIELD   = 8,
  PCIE_LIB_FEATURE_ATOMICOP      = 9,
  PCIE_LIB_FEATURE_ARIFORWARD    = 10,
  PCIE_LIB_FEATURE_LTR           = 11,
  PCIE_LIB_FEATURE_MRRS          = 12,
  PCIE_LIB_FEATURE_RELAXORDER    = 13,
  PCIE_LIB_FEATURE_NOSNOOP       = 14,
  PCIE_LIB_FEATURE_EXTSYNC       = 15,
  // Add more feature here
  PCIE_LIB_FEATURE_MAX
} PCIE_LIB_FEATURE_TYPE;


typedef enum _PCIE_LIB_PORTTYPE
{
  PCIE_LIB_PCIE    = 1,
  PCIE_LIB_DMI     = 2,
  PCIE_LIB_PORT_TYPE_MAX
} PCIE_LIB_PORTTYPE;

typedef enum {
  PcieLibAspmDisabled = 0,
  PcieLibAspmL0s      = 1,
  PcieLibAspmL1       = 2,
  PcieLibAspmL0sL1    = 3,
  PcieLibAspmAutoConfig,
  PcieLibAspmMax
} PCIE_LIB_ASPM_POLICY;

typedef enum {
  PcieLibCompletionTO_Default  = 0,
  PcieLibCompletionTO_50_100us = 1,
  PcieLibCompletionTO_1_10ms   = 2,
  PcieLibCompletionTO_16_55ms  = 5,
  PcieLibCompletionTO_65_210ms = 6,
  PcieLibCompletionTO_260_900ms= 9,
  PcieLibCompletionTO_1_3P5s   = 10,
  PcieLibCompletionTO_4_13s    = 13,
  PcieLibCompletionTO_17_64s   = 14,
  PcieLibCompletionTO_Disabled
} PCIE_LIB_COMPLETION_TIMEOUT_POLICY;

typedef enum {
  PcieLibSlotFlagDisable   = 0,
  PcieLibSlotFlagEnable    = 1,
  PcieLibSlotFlagOn        = 2,
  PcieLibSlotFlagOff       = 3,
  PcieLibSlotFlagBlink     = 4,
  PcieLibSlotFlagDefault   = 5,
  PcieLibSlotMax
} PCIE_LIB_SLOT_POLICY;

typedef struct _PCIE_LIB_SLOT_CONTROL_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 SlotImplemented;
  PCIE_LIB_SLOT_POLICY  AttentionButton;
  PCIE_LIB_SLOT_POLICY  PowerFaultDetect;
  PCIE_LIB_SLOT_POLICY  MRLSensorChanged;
  PCIE_LIB_SLOT_POLICY  PresenceDetect;
  PCIE_LIB_SLOT_POLICY  CommandCompletedInt;
  PCIE_LIB_SLOT_POLICY  HotPlugInt;
  PCIE_LIB_SLOT_POLICY  AttentionIndicator;
  PCIE_LIB_SLOT_POLICY  PowerIndicator;
  PCIE_LIB_SLOT_POLICY  PowerController;
  PCIE_LIB_SLOT_POLICY  DataLinkLayerState;
} PCIE_LIB_SLOT_CONTROL_INF;

typedef struct _PCIE_LIB_SLOT_VPP_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8 Flag;
  UINT8 VppAddr;
  UINT8 IoExpander_PortNum;
} PCIE_LIB_SLOT_VPP_INF;

typedef struct _PCIE_LIB_CTO_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  PCIE_LIB_COMPLETION_TIMEOUT_POLICY Policy;
} PCIE_LIB_CTO_INF;

typedef struct _PCIE_LIB_RELAXORDER_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
} PCIE_LIB_RELAXORDER_INF;

typedef struct _PCIE_LIB_EXTSYNC_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
} PCIE_LIB_EXTSYNC_INF;

typedef struct _PCIE_LIB_NOSNOOP_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
} PCIE_LIB_NOSNOOP_INF;

#define PCIE_LIB_MAXPAYLOAD_AUTO        0
#define PCIE_LIB_MAXPAYLOAD_OVERRIDE    1

typedef struct _PCIE_LIB_MAXPAYLOAD_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
  UINT16                MaxPayload;
} PCIE_LIB_MAXPAYLOAD_INF;

typedef struct _PCIE_LIB_ASPM_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  PCIE_LIB_ASPM_POLICY  Policy;
  UINT8 Reserved[2];
} PCIE_LIB_ASPM_INF;

typedef struct _PCIE_LIB_COMMONCLOCK_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
} PCIE_LIB_COMMONCLOCK_INF;

#define PCIE_LIB_EXTTAGFIELD_AUTO       2
#define PCIE_LIB_EXTTAGFIELD_5_BIT      0
#define PCIE_LIB_EXTTAGFIELD_8_BIT      1

typedef struct _PCIE_LIB_EXTTAGFIELD_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
} PCIE_LIB_EXTTAGFIELD_INF;


#define PCIE_LIB_ATOMICOP_AUTO        2
#define PCIE_LIB_ATOMICOP_ENABLE      1
#define PCIE_LIB_ATOMICOP_DISABLE     0

typedef struct _PCIE_LIB_ATOMICOP_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
} PCIE_LIB_ATOMICOP_INF;


#define PCIE_LIB_LTR_AUTO        2
#define PCIE_LIB_LTR_ENABLE      1
#define PCIE_LIB_LTR_DISABLE     0

typedef struct _PCIE_LIB_LTR_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
} PCIE_LIB_LTR_INF;

#define PCIE_LIB_MRRS_AUTO        0
#define PCIE_LIB_MRRS_OVERRIDE    1

typedef struct _PCIE_LIB_MRRS_INF {
  PCIE_LIB_FEATURE_TYPE Type;
  UINT8                 Flag;
  UINT16                MaxReadReqSize;
} PCIE_LIB_MRRS_INF;

typedef union _PCIE_LIB_ROOTPORT_FEATURE_NODE {
  struct {
    PCIE_LIB_FEATURE_TYPE     Type;
  } Common;  
  PCIE_LIB_SLOT_VPP_INF          SlotVpp;
  PCIE_LIB_ASPM_INF              Aspm;
  PCIE_LIB_CTO_INF               Cto;
  PCIE_LIB_MAXPAYLOAD_INF        MaxPayload;
  PCIE_LIB_COMMONCLOCK_INF       CommonClock;
  PCIE_LIB_SLOT_CONTROL_INF      SlotConfig;
  PCIE_LIB_EXTTAGFIELD_INF       ExtendedTagField;
  PCIE_LIB_ATOMICOP_INF          AtomicOp;
  PCIE_LIB_LTR_INF               Ltr;
  PCIE_LIB_MRRS_INF              Mrrs;
  PCIE_LIB_RELAXORDER_INF        RelaxedOrder;
  PCIE_LIB_EXTSYNC_INF           ExtendedSync;
  PCIE_LIB_NOSNOOP_INF           NoSnoop;
  //
  // Add more feature here
  //
}PCIE_LIB_ROOTPORT_FEATURE_NODE;

typedef struct _PCIE_LIB_ROOT_PORT_INF {
  UINT8                            Valid;
  PCIE_LIB_PORTTYPE                Type;
  UINT8                            Name[8];             // Port Name
  UINT8                            Segment;             // Segment
  UINT8                            Bus;                 // Bus
  UINT8                            Dev;                 // Device
  UINT8                            Fun;                 // Function
  UINT8                            Attribute;           // Attribute
  UINT8                            PortNum;             // Port Number Associated With This Slot
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList;        // Point to feature list table
}PCIE_LIB_ROOT_PORT_INF;

typedef struct {
  USRA_PCIE_ADDR_TYPE   PcieAddr;
  UINT32                AndMask;
  UINT32                OrMask;
} PCI_LIB_OP_STRUCT;

typedef struct _PCIE_LIB_BIF_INF {
  UINT8                     Valid;
  PCI_LIB_OP_STRUCT         *OpTable;               //OP table to perform Bifurcation
  UINT32                    OpTableSize;
  UINT8                     Delay;
  UINT8                     Reserved[4];
}PCIE_LIB_BIF_INF;

typedef struct _PCIE_LIB_HOST_BRIDGE_INF {
  UINT8                            Revision;
  UINT8                            Segment;             // Segment
  UINT8                            TempBusMin;          // Min Bus
  UINT8                            TempBusMax;          // Max Bus
  UINT8                            Name[8];             // Name
  UINT8                            Attribute;           // Attribute
  PCIE_LIB_BIF_INF                 BifInf;
  PCIE_LIB_ROOT_PORT_INF           *RootPortList;        
  UINT8                            Reserved[8];
}PCIE_LIB_HOST_BRIDGE_INF;

#define PCIE_LIB_HOSTBRIDGE_REVISION_1    1
#define PCIE_LIB_HOSTBRIDGE_REVISION_END  0

typedef struct _PCIE_LIB_TOPOLOGY_INF {
  UINT8                            Revision;
  PCIE_LIB_HOST_BRIDGE_INF         *HostBridgeList;        
  UINT8                            Reserved[8];
}PCIE_LIB_TOPOLOGY_INF;
#define PCIE_LIB_TOPOLOGY_REVISION_1      1
#define PCIE_ROOT_PORT_INDEX              0
#define PCIE_ROOT_PORT_COMMON_CLCOK_ENABLE  1
#define PCIE_ROOT_PORT_COMMON_CLCOK_DISABLE 0


typedef struct {
  USRA_PCIE_ADDR_TYPE     CapPcieAddress;
  UINT16                  PcieLnkCap;
  UINT16                  PcieDevCap;
  UINT16                  PcieDevCap2;
  UINT16                  Reserved;
} PCIE_CAP_INFO;

typedef struct {
  INTN            Top;
  PCIE_CAP_INFO   PcieCapInfo[FixedPcdGet32(PcdMaxNestedLevel)];
} PCIE_LIB_STACK;



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
  );

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
  );

/**
  Init root bridge by node information list.

  Platform developer needs to fill out the information of PCIE_LIB_ROOT_PORT_INF 
  and feature list according to platform policy or user selection 
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
  );

/**
  Bifurcation according to PCIE_LIB_BIF_INF.

  Platform developer needs to fill out the information of PCIE_LIB_BIF_INF 
  as this way, the lib knows how to do Bifurcation
   
  @param [in]    BifInf    Bifurcation information.

  @return status

**/
EFI_STATUS
PcieHostBridgeBifurcation (  
  PCIE_LIB_BIF_INF             *BifInf
  );

/**
  Set ASPM to the endpoint and up stream according to feature node.

  Only call this function on the end point.
   
  @param [in]    PcieStack      Whole PCIE stream stack to be performed on.
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    ASPM feature node.

  @return status

**/
EFI_STATUS
PcieLibSetAspmPath(
  PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                     *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE   *FeatureList
  );

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
  );

/**
  Set slot control features on root port.

  Only call this function on the root port.
   
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Slot control feature node.

  @return status

**/
EFI_STATUS
PcieLibSetSlotControl(
  USRA_ADDRESS                      *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE    *FeatureList
  );


/**
  Set complete timeout features on root port.

  Only call this function on the root port.
   
  @param [in]    PcieAddress     PCI address of the end point.
  @param [in]    FeatureList    Complete timeout feature node.

  @return status

**/
EFI_STATUS
PcieLibSetCto(
  USRA_ADDRESS                      *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE    *FeatureList
  );

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
   PCIE_LIB_STACK                   *PcieStack,
  USRA_ADDRESS                      *PcieAddress,
  PCIE_LIB_ROOTPORT_FEATURE_NODE    *FeatureList
  );

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
BOOLEAN
PcieInitRootPortDownstreamDevices (
  USRA_ADDRESS                   *PcieAddress,
  UINT8                          TempBusNumberMin,
  UINT8                          TempBusNumberMax,
  UINT8                          *TempBusNumberUsed,
  PCIE_LIB_ROOTPORT_FEATURE_NODE *FeatureList,
  BOOLEAN                        CleanFlag,
  PCIE_LIB_STACK                 *PcieStack
 );

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
  );

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
  );

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
  );

#endif
