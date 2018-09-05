//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2016 Intel Corp.                             *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *      This file contains the KTI discovery specific information.        *
// *                                                                        *
// **************************************************************************
**/

#ifndef _KTI_DISCOVERY_H_
#define _KTI_DISCOVERY_H_
#ifdef _MSC_VER
#pragma warning (disable: 4127 4214 4100)     // disable C4127: constant conditional expression
#endif
#include "DataTypes.h"
#include "PlatformHost.h"
#include "KtiSi.h"

#define MAX_TREE_NODES       (MAX_SOCKET + 2) // 2 additional nodes since a node will appear more than once in the tree when it is being constructed
#define MAX_RING_TREE_NODES  46  // A CPU with 3 links supported will have 1 + 1*3 + 3*2 + 6*2 + 12*2 = 46 nodes maximum in ring tree
#define MAX_RINGS            6   // Maximum number of rings possible in systems with upto 8 sockets (HyperCube)
#define CPUS_PER_RING        4   // # of CPUs in a CPU ring
#define VN0                  0
#define VN1                  1
#define TX                   0
#define RX                   1

#pragma pack(1)

//
// Generic Data structure to describe Link Exchange Parameter (LEP) info
//
typedef struct {
#ifndef ASM_INC
  UINT32  Valid : 1;        // TRUE, if the link is valid (i.e trained successfully for low speed, no validation override that disables it)
  UINT32  PeerSocId : 3;    // Socket ID
  UINT32  PeerSocType : 2;  // Socket Type
  UINT32  PeerPort : 4;     // Port of the peer socket
  UINT32  DualLink : 1;     // TRUE, if there is a second link to the same neighbor
  UINT32  TwoSkt3Link : 1;  // TRUE, if there is a second and third link to the same neighbor
  UINT32  Rsvd1 : 20;
#else
  UINT32  RawData;
#endif // ASM_INC
} KTI_LINK_DATA;

//
// Generic Data structure to describe socket info
//
typedef struct {
#ifndef ASM_INC
  UINT8         Valid   : 1;
  UINT8         SocId   : 3;  // Socket ID
  UINT8         Vn      : 1;  // VN used by traffic originating at this socket
  UINT8         SocType : 2;  // Socket Type
  UINT8         Rsvd1   : 1;
#else
  UINT8         RawData;
#endif // ASM_INC
  KTI_LINK_DATA LinkData[MAX_KTI_PORTS];     // KTI Link data
} KTI_CPU_SOCKET_DATA;

//
// Generic Data structure to describe topology tree
//
typedef union {
#ifndef ASM_INC
  struct {
    UINT32  Valid : 1;
    UINT32  SocId : 3;        // Socket ID
    UINT32  SocType : 2;      // Socket Type
    UINT32  Rsvd1 : 2;
    UINT32  ParentPort : 4;   // Port that connects to the parent
    UINT32  Hop : 4;          // Depth of the node from root socket
    UINT32  Rsvd2 : 3;
    UINT32  ParentIndex : 7;  // Index of immediate parent
    UINT32  Rsvd3 : 6;
  } Node;
#endif // ASM_INC
  UINT32  RawNode;
} TOPOLOGY_TREE_NODE;


typedef struct {
  UINT32  IgrVnSel;         // Ingress VN SEL setting. Same setting is used for both ports of this M3KTI
}M3KTI_VN_SEL;

//
// Data structure to capture all the route setting
//
typedef struct {
  UINT32         ChaKtiRt;
  UINT32         M2PcieKtiRt;
  UINT32         M3KtiRt[MAX_M3KTI];
  M3KTI_VN_SEL   M3KtiVnSel[MAX_M3KTI];
  UINT32         ChaKtiFanOut;
#ifndef ASM_INC
  UINT32         M3KtiSnpFanOut[MAX_M3KTI][MAX_PORT_IN_M3KTI];
#else
  UINT32         M3KtiSnpFanOut[MAX_M3KTI * MAX_PORT_IN_M3KTI];
#endif
  UINT32         KtiAgentRt[MAX_KTI_PORTS];
} KTI_CPU_RT;

//
// Topology Tree info generated and consumed by Discovery engine.
// The Si specific data (currently only port number) is abstracted out using generic port number
// 0, 1, ... MAX_KTI_PORTS. If a Si family has port numbering that is contiguous, then this abtrasction
// has no effect.
//
typedef struct {
  UINT8               TotCpus;  // Total number of CPUs populated
  UINT32              CpuList;  // List of CPUs populated
  UINT8               TotRings;  // Total number of rings present in the topology
  UINT8               RingNodePairs[MAX_SOCKET];  // Socket pair that is common to any two rings
  UINT8               SocMap[MAX_SOCKET]; // Mapping of intel reference system socket id to this system's socket id
  KTI_CPU_SOCKET_DATA Cpu[MAX_SOCKET]; // Si agnostic socket data (Port numbers are generalized)
#ifndef ASM_INC
  UINT8               Rings[MAX_RINGS][CPUS_PER_RING + 1];  //All the rings present in the topology.
                                                            //Note: '+1' needed since Element-0 is repeated in last Element to show a full circle.
  UINT8               RingInfo[MAX_SOCKET][MAX_RINGS];  // Number of rings (Element 0) and the ring numbers (Element 1 onwards) this socket is a part of
  UINT8               LinkLoad[MAX_SOCKET][MAX_KTI_PORTS]; // Number of times this link is used; link number is Si agnostic
  TOPOLOGY_TREE_NODE  CpuTree[MAX_SOCKET][MAX_TREE_NODES];  // Si agnostic topology tree (Port numbers are generalized)
#else
  UINT8               Rings[MAX_RINGS * (CPUS_PER_RING + 1)];//All the rings present in the topology
                                                             //Note: '+1' needed since Element-0 is repeated in last Element to show a full circle.
  UINT8               RingInfo[MAX_SOCKET * MAX_RINGS];  // Number of rings (Element 0) and the ring numbers (Element 1 onwards) this socket is a part of
  UINT8               LinkLoad[MAX_SOCKET * MAX_KTI_PORTS]; // Number of times this link is used; link number is Si agnostic
  TOPOLOGY_TREE_NODE  CpuTree[MAX_SOCKET * MAX_TREE_NODES];  // Si agnostic topology tree (Port numbers are generalized)
#endif
} KTI_DISC_DATA;


//
// Topology Tree and Final Routing setting for each populated socket
//
typedef struct {
  KTI_CPU_SOCKET_DATA Cpu[MAX_SOCKET];
  TOPOLOGY_TREE_NODE  SbspMinPathTree[MAX_TREE_NODES];
#ifndef ASM_INC
  TOPOLOGY_TREE_NODE  CpuTree[MAX_SOCKET][MAX_TREE_NODES];
#else
  TOPOLOGY_TREE_NODE  CpuTree[MAX_SOCKET * MAX_TREE_NODES];
#endif
  KTI_CPU_RT          Route[MAX_SOCKET];
} KTI_SOCKET_DATA;

#pragma pack()

#endif // _KTI_DISCOVERY_H_
