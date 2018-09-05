//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PciExpress.h

Abstract:

  Macros to simplify and abstract the interface to PCIE configuration.

--*/

#ifndef _PCIEXPRESS_H_INCLUDED_
#define _PCIEXPRESS_H_INCLUDED_

typedef union {
  struct{    
    UINT16  CapabilityVersion:4;          // 3:0      
    UINT16  DevicePortType:4;             // 7:4
    UINT16  SlotImplemented:1;            // 8  
    UINT16  InteruptMessageNumber:5;      // 13:9
  } Bits;
  UINT16    Word;
} PCIE_CAP;

typedef union {
  struct{
    UINT32  MaxPayloadSupported:3;          // 2:0
    UINT32  PhantomFunctionsSupported:2;    // 4:3
    UINT32  ExtendedTagFieldSupported:1;    // 5
    UINT32  EndpointL0sAcceptableLatency:3; // 8:6
    UINT32  EndpointL1sAcceptableLatency:3; // 11:9
    UINT32  AttentionButtonPresent:1;       // 12  
    UINT32  AttentionIndicatorPresent:1;    // 13
    UINT32  PowerIndicatorPresent:1;        // 14
    UINT32  Reserved:3;                     // 17:15
    UINT32  CapturedSlotPowerLimitValue:8;  // 25:18
    UINT32  CapturedSlotPowerLimitScale:2;  // 27:26
    UINT32  Reserved2:4;                    // 31:28    
  } Bits;
  UINT32    Dword;
} PCIE_DEV_CAP;

typedef union {
  struct{
    UINT16  CorrectableErroReportingEnable:1;      // 0
    UINT16  NonFatalErroReportingEnable:1;         // 1
    UINT16  FatalErroReportingEnable:1;            // 2
    UINT16  UnsuppportedRequrestReportingEnable:1;  // 3
    UINT16  EnableRelaxedOrdering:1;                // 4
    UINT16  MaxPlayloadSize:3;                      // 7:5
    UINT16  ExtendedTagFieldEnable:1;               // 8
    UINT16  PhantomFunctionsEnable:1;               // 9    
    UINT16  AuxPowerPmEnable:1;                     // 10      
    UINT16  EnableSnoopNotRequired:1;               // 11    
    UINT16  MaxReadRequestSize:3;                   // 14:12
    UINT16  BridgeConfigurationRetryEnable:1;       // 15    
  } Bits;
  UINT16    Word;
} PCIE_DEV_CTRL;

typedef union {
  struct{
    UINT16  CorrectableErrorDetect:1;               // 0
    UINT16  NonFatalErrorDetect:1;                  // 1
    UINT16  FatalErrorDetect:1;                     // 2
    UINT16  UnsuppportedRequrestDetect:1;           // 3
    UINT16  AuxPowerDetect:1;                       // 4
    UINT16  TransactionsPending:1;                  // 5
    UINT16  Reserved:10;                            // 15:6    
  } Bits;
  UINT16    Word;
} PCIE_DEV_STS;

typedef union {
  struct{
    UINT32  MaxLinkSpeed:4;                        // 3:0
    UINT32  MaxLinkWidth:6;                        // 9:4
    UINT32  AspmSupport:2;                         // 11:10
    UINT32  L0sExitLatency:3;                      // 14:12
    UINT32  L1ExitLatency:3;                       // 17:15
    UINT32  ClockPowerManagement:1;                // 18  
    UINT32  SurpriseDownErroReporting:1;          // 19
    UINT32  DataLinkLayerLinkActiveReporting:1;    // 20  
    UINT32  LinkBandwidthNotificationCapability:1; // 21
    UINT32  ASPMOptionalityCompliance:1;           // 22
    UINT32  RsvdP:1;                               // 23
    UINT32  PortNumber:8;                          // 31:24
  } Bits;
  UINT32    Dword;
} PCIE_LNK_CAP;

typedef union {
  struct{
    UINT16  ActiveStatePowerManagementControl:2;     // 1:0
    UINT16  Reserved1:1;                             // 2
    UINT16  ReadCompletionBoundary:1;                // 3
    UINT16  LinkDisable:1;                           // 4
    UINT16  RetrainLink:1;                           // 5
    UINT16  CommonClockConfiguration:1;              // 6  
    UINT16  ExtendedSynch:1;                         // 7
    UINT16  EnableClockPowerManagement:1;            // 8  
    UINT16  HardwareAutonomousWidthDisable:1;        // 9
    UINT16  LinkBandwidthManagementInterruptEnable:1;// 10  
    UINT16  LinkAutonomousBandwidthInterruptEnable:1;// 11
    UINT16  Reserved2:4;                             // 15:12
  } Bits;
  UINT16    Word;
} PCIE_LNK_CTRL;

typedef union {
  struct{
    UINT16  LinkSpeed:4;                        // 3:0
    UINT16  NegotiatedLinkWidth:6;              // 9:4
    UINT16  TrainingError:1;                    // 10
    UINT16  LinkTraining:1;                     // 11
    UINT16  SlotClockConfiguration:1;           // 12
    UINT16  DataLinkLayerLinkActive:1;          // 13
    UINT16  LinkBandwidthManagementStatus:1;    // 14
    UINT16  LinkAutonomousBandwidthStatus:1;    // 15
  } Bits;
  UINT16    Word;
} PCIE_LNK_STS;


typedef union {
  struct{
    UINT32  AttentionButtonImplented:1;   // 0
    UINT32  PowerControllerImplemented:1; // 1
    UINT32  MrlSensorImplemented:1;       // 2
    UINT32  AttentionIndicatorImplented:1;// 3
    UINT32  PowerIndicatorImplemented:1;  // 4
    UINT32  HotplugSuprise:1;             // 5  
    UINT32  HotplugCapable:1;             // 6
    UINT32  SlotPowerLimitValue:8;        // 14:7  
    UINT32  SlotPowerLimitScale:2;        // 16:15
    UINT32  Reserved:2;                   // 18:17
    UINT32  PhysicalSlotNum:13;           // 31:19    
  } Bits;
  UINT32    Dword;
} PCIE_SLOT_CAP;

typedef union {
  struct{
    UINT16  AttentionButtonEnable:1;            // 0
    UINT16  PowerFaultDetectedEnable:1;         // 1
    UINT16  MrlSensorChangedEnable:1;           // 2
    UINT16  PresenceDetectChangedEnable:1;      // 3
    UINT16  CommandCompletedInterruptEnable:1;  // 4
    UINT16  HotplugInterruptEnable:1;           // 5  
    UINT16  AttentionIndicatorControl:2;        // 7:6
    UINT16  PowerIndicatorControl:2;            // 9:8  
    UINT16  PowerControllerControl:1;           // 10    
    UINT16  ElectromechanicalInterlockControl:1;// 11    
    UINT16  DataLinkLayerStateChangedEnable:1;  // 12    
  } Bits;
  UINT16    Word;
} PCIE_SLOT_CTRL;

typedef union {
  struct{
    UINT16  AttentionButtonPressed:1;           // 0    
    UINT16  PowerFaultDetected:1;               // 1
    UINT16  MrlSensorChanged:1;                 // 2
    UINT16  PresenceDetectChanged: 1;           // 3
    UINT16  CommandCompleted:1;                 // 4    
    UINT16  MrlSensorState:1;                   // 5
    UINT16  PresenceDetectState: 1;             // 6
    UINT16  MechanicalLatchStatus: 1;           // 7
    UINT16  DataLinkStateChanged: 1;            // 8
  } Bits;
  UINT16    Word;
} PCIE_SLOT_STS;

typedef union {
  struct{
    UINT8  CtoRangeSupported:4;          // 3:0
    UINT8  CtoDiableSupported:1;         // 4
    UINT8  Reserved2:3;                  // 7:5    
  } Bits;
  UINT8    Byte;
} PCIE_DEV_CAP2;

typedef union {
  struct{
    UINT8  CtoValue:4;                   // 3:0
    UINT8  CtoDiable:1;                  // 4
    UINT8  Reserved2:3;                  // 7:5    
  } Bits;
  UINT8    Byte;
} PCIE_DEV_CTRL2;

#define PCIE_CAPABILITY_OFFSET                0x02
#define   V_PCIE_CAPABILITY_DPT_ROOT_PORT       0x40

#define PCIE_DEVICE_CAPABILITY_OFFSET         0x04
#define   B_PCIE_DEVICE_CAPABILITY_MAX_PAYLOAD  (BIT5 | BIT6 | BIT7)
#define     V_PCIE_DEVICE_CAPABILITY_128          0
#define     V_PCIE_DEVICE_CAPABILITY_256          1
#define     V_PCIE_DEVICE_CAPABILITY_512          2
#define     V_PCIE_DEVICE_CAPABILITY_1024         3
#define     V_PCIE_DEVICE_CAPABILITY_2048         4
#define     V_PCIE_DEVICE_CAPABILITY_4096         5
#define PCIE_DEVICE_CONTROL_OFFSET            0x08
#define PCIE_DEVICE_STATUS_OFFSET             0x0A


#define PCIE_LINK_CAPABILITY_OFFSET           0x0C
#define   B_PCIE_LINK_CAPABILITY_PORT_TYPE      (BIT4 | BIT5 | BIT6 | BIT7)
#define     V_PCIE_LINK_CAPABILITY_PORT_TYPE_ENDPOINT             0
#define     V_PCIE_LINK_CAPABILITY_PORT_TYPE_LEGACY_ENDPOINT      1
#define     V_PCIE_LINK_CAPABILITY_PORT_TYPE_ROOT_PORT            4
#define     V_PCIE_LINK_CAPABILITY_PORT_TYPE_UP_PORT              5 
#define     V_PCIE_LINK_CAPABILITY_PORT_TYPE_DOWN_PORT            6
#define     V_PCIE_LINK_CAPABILITY_PORT_TYPE_PCIE_TO_PCI_BRIDGE   7
#define     V_PCIE_LINK_CAPABILITY_PORT_TYPE_PCI_TO_PCIE_BRIDGE   8

#define PCIE_LINK_CONTROL_OFFSET              0x10
#define   B_PCIE_LINK_CONTROL_RETRAIN_LINK      BIT5
#define   B_PCIE_LINK_CONTROL_COMMON_CLOCK      BIT6
#define PCIE_LINK_STATUS_OFFSET               0x12
#define   B_PCIE_LINK_STATUS_LINK_TRAINING      BIT11
#define   B_PCIE_LINK_STATUS_SLOT_CLOCK         BIT12

#define PCIE_DEVICE_CAPABILITY2_OFFSET        0x24
#define PCIE_DEVICE_CONTROL2_OFFSET           0x28
#define PCIE_CAPABILITY_LIST_OFFSET           0x00
#define PCIE_CAPABILITY_OFFSET                0x02
#define PCIE_DEVICE_CAPABILITY_OFFSET         0x04
#define PCIE_DEVICE_CONTROL_OFFSET            0x08
#define   B_PCIE_DEVICE_CONTROL_COR_ERR_EN      BIT0
#define   B_PCIE_DEVICE_CONTROL_NF_ERR_EN       BIT1
#define   B_PCIE_DEVICE_CONTROL_FAT_ERR_EN      BIT2
#define   B_PCIE_DEVICE_CONTROL_UNSUP_REQ_EN    BIT3
#define PCIE_DEVICE_STATUS_OFFSET             0x0A
#define   B_PCIE_DEVICE_STATUS_COR_ERR          BIT0
#define   B_PCIE_DEVICE_STATUS_NF_ERR           BIT1
#define   B_PCIE_DEVICE_STATUS_FAT_ERR          BIT2
#define   B_PCIE_DEVICE_STATUS_UNSUP_REQ        BIT3
#define PCIE_LINK_CAPABILITY_OFFSET           0x0C
#define PCIE_LINK_CONTROL_OFFSET              0x10
#define PCIE_LINK_STATUS_OFFSET               0x12
#define PCIE_SLOT_CAPABILITY_OFFSET           0x14
#define PCIE_SLOT_CONTROL_OFFSET              0x18
#define PCIE_SLOT_STATUS_OFFSET               0x1A
#define PCIE_ROOT_CONTROL_OFFSET              0x1C
#define   B_PCIE_ROOT_CONTROL_COR_ERR_EN        BIT0
#define   B_PCIE_ROOT_CONTROL_NF_ERR_EN         BIT1
#define   B_PCIE_ROOT_CONTROL_FAT_ERR_EN        BIT2
#define PCIE_ROOT_STATUS_OFFSET               0x20
#define PCIE_DEVICE_CAPABILITY2_OFFSET        0x24
#define PCIE_DEVICE_CONTROL2_OFFSET           0x28

//
//PCI Express Extended CAP_ID Assigned for advanced error reporting
//
#define V_PCIE_EXT_CAP_HEADER_AERC            0x0001

#endif
