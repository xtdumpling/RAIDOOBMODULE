/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
 /**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2017 Intel Corp.                             *
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
// *      This file contains KTI system topology discovery and route        *
// *      calculation code.                                                 *
// *                                                                        *
// **************************************************************************
**/

#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysHost.h"
#include "KtiLib.h"
#include "KtiMisc.h"
#include "SysFunc.h"

KTI_STATUS
PrepareDiscEngData (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA       *DiscEngData
  );

KTI_STATUS
LocateRings (
  struct sysHost      *host,
  KTI_DISC_DATA       *DiscEngData
  );

STATIC
KTI_STATUS
ConstructRingTree (
  struct sysHost             *host,
  KTI_DISC_DATA              *DiscEngData,
  TOPOLOGY_TREE_NODE         *RingTree,
  UINT8                      RootSocId
  );

STATIC
KTI_STATUS
CheckForRings (
  struct sysHost             *host,
  KTI_DISC_DATA              *DiscEngData,
  TOPOLOGY_TREE_NODE         *RingTree,
  UINT8                      RootSocId
  );

STATIC
KTI_STATUS
ParseRings (
  struct sysHost             *host,
  KTI_DISC_DATA              *DiscEngData
  );

STATIC
KTI_STATUS
AssignVn (
  struct sysHost          *host,
  KTI_DISC_DATA           *DiscEngData
  );

STATIC
KTI_STATUS
ComputeTopologyTree (
  struct sysHost             *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA            *DiscEngData
  );

STATIC
KTI_STATUS
ExploreThisNode (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8                      RootSocId,
  TOPOLOGY_TREE_NODE  *Parent,
  UINT8                      ParentIdx,
  UINT8                      Hop
  );

STATIC
KTI_STATUS
CheckAndAddNodeToTopologyTree (
  struct sysHost           *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8                    RootSocId,
  TOPOLOGY_TREE_NODE        *Parent,
  TOPOLOGY_TREE_NODE       *LeafNode,
  UINT8                      Hop
  );

STATIC
KTI_STATUS
TrackPathLoad (
  struct sysHost       *host,
  KTI_DISC_DATA        *DiscEngData,
  TOPOLOGY_TREE_NODE   *Tree,
  TOPOLOGY_TREE_NODE   *LeafNode,
  UINT8                      Adjust
  );

STATIC
UINT8
CalculatePathLoad (
  struct sysHost       *host,
  KTI_DISC_DATA        *DiscEngData,
  TOPOLOGY_TREE_NODE   *Tree,
  TOPOLOGY_TREE_NODE   *LeafNode
  );

STATIC
KTI_STATUS
DetectDeadlock (
  struct sysHost      *host,
  KTI_DISC_DATA       *DiscEngData
  );

STATIC
KTI_STATUS
CopyTopologyTree (
  struct sysHost      *host,
  KTI_SOCKET_DATA      *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA       *DiscEngData
  );

#if MAX_SOCKET > 4
STATIC
KTI_STATUS
ComputeTopologyTree8S (
  struct sysHost             *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA            *DiscEngData
  );

STATIC
KTI_STATUS
Map8SNodes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData
  );

STATIC
KTI_STATUS
Map8SRoutes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8                    (*OemRoute)[4][2]
  );

STATIC
KTI_STATUS
Map8SRoutes_VN1Disable (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8                    (*OemRoute)[4][3]
  );

STATIC
KTI_STATUS
ComputeTopologyTree6S (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData
  );

STATIC
KTI_STATUS
Map6SNodes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData
  );

STATIC
KTI_STATUS
Map6SRoutes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8         (*OemRoute)[3][3]
  );
#endif


STATIC
KTI_STATUS
CalculateRT (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal
  );

STATIC
KTI_STATUS
SetupNonBCRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE       *TopologyTree,
  TOPOLOGY_TREE_NODE       *Root,
  TOPOLOGY_TREE_NODE       *Interm,
  TOPOLOGY_TREE_NODE       *Dest
  );

STATIC
KTI_STATUS
SetupBCRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE       *Root,
  TOPOLOGY_TREE_NODE       *Interm,
  TOPOLOGY_TREE_NODE       *Dest,
  BOOLEAN                  PassThruFanout
  );

STATIC
KTI_STATUS
SetRootNodeRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  UINT8                    SocId,
  UINT8                    DestSocId,
  UINT8                    OutPort1,
  UINT8                    OutPort2,
  UINT8                    OutPort3,
  BOOLEAN                  DualLinkOrTripleLink,
  UINT8                    RootVn
  );

STATIC
KTI_STATUS
SetPassThruRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  UINT8                    SocId,
  UINT8                    DestSocId,
  UINT8                    InPort,
  UINT8                    OutPort,
  UINT8                    RootVn
  );

STATIC
KTI_STATUS
SetRootNodeFanoutRoute (
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE       *Parent,
  UINT8                    RootBcRti,
  UINT8                    OutPort
  );

STATIC
KTI_STATUS
SetPassThruFanoutRoute (
  struct sysHost          *host,
  KTI_SOCKET_DATA          *SocketData,
  TOPOLOGY_TREE_NODE       *Parent,
  UINT8                    RootBcRti,
  UINT8                    InPort,
  UINT8                    OutPort
  );

STATIC
VOID
OptimizeChaFanoutSnoop (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData
  );


/**

   Perform the KTI Topology Discovery of the system. It discovers the system
   topology from each socket's perspective and calculates the route. This
   function must be invoked only by SBSP after collecting LEP of each socket populated.
   This function doesn't program any H/W registers. Route Table of each socket is
   returned in SocketData. The caller is responsible for porgramming the Route Table into
   the appropriate registers upon successful exit from this function.

   @param host              - Pointer to the system host (root) structure
   @param SocketData        - Pointer to socket specific data
   @param KtiInternalGlobal - KTIRC internal variables.

   @retval 0     - Success
   @retval Other - Failure

**/
KTI_STATUS
KtiTopologyDiscovery (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  KTI_DISC_DATA           DiscEngData;

  // Prepare the discovery engine data structure
  PrepareDiscEngData (host, SocketData, KtiInternalGlobal, &DiscEngData);

  // Check and identify the rings present in the topology
  LocateRings (host, &DiscEngData);

  // Assign VN to each node that is part of a ring
  if (host->var.kti.OutVn1En == TRUE)  {
    AssignVn (host, &DiscEngData);
  }

  // Discover the topology and construct the topology tree.
  ComputeTopologyTree (host, KtiInternalGlobal, &DiscEngData);

  // Check to make sure the route is deadlock free
  DetectDeadlock (host, &DiscEngData);

  // Copy the topology tree info into host/SocketData
  CopyTopologyTree (host, SocketData, KtiInternalGlobal, &DiscEngData);

  // Sanity check discoverd topology
  if(SanityCheckDiscoveredTopology (host, SocketData, KtiInternalGlobal) != KTI_SUCCESS) {
    return KTI_FAILURE;
  }

  // Calculate the route setting
  CalculateRT (host, SocketData, KtiInternalGlobal);

  return KTI_SUCCESS;
}

/**

  Copies the socket data to disc engine structure and applies the KTI port mapping to make
  it Si agnostic.

  @param host        - Pointer to the system host (root) structure
  @param SocketData  - Pointer to socket specific data
  @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
KTI_STATUS
PrepareDiscEngData (
  struct sysHost      *host,
  KTI_SOCKET_DATA      *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA       *DiscEngData
  )
{
  UINT8               Idx1, Idx2;
  UINT8               TempPort;
  UINT8               PeerPort, PeerSocId;

  //
  // Copy the Si specific CPU socket data and convert that into Si agnostic data using the port mapping.
  //
  MemSetLocal ((VOID *) DiscEngData, 0x00, sizeof (KTI_DISC_DATA));
  MemCopy ((UINT8 *) DiscEngData->Cpu, (UINT8 *) SocketData->Cpu, sizeof (DiscEngData->Cpu));
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((DiscEngData->Cpu[Idx1].Valid == TRUE) && (DiscEngData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
      DiscEngData->TotCpus++;
      DiscEngData->CpuList = DiscEngData->CpuList | (1 << Idx1);
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        if ((DiscEngData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) && (DiscEngData->Cpu[Idx1].LinkData[Idx2].PeerSocType == SOCKET_TYPE_CPU)) {
          TempPort = (UINT8) DiscEngData->Cpu[Idx1].LinkData[Idx2].PeerPort;
          DiscEngData->Cpu[Idx1].LinkData[Idx2].PeerPort = TranslateToGenPort (host, TempPort);
        }
      }
    } else if ((DiscEngData->Cpu[Idx1].SocType == SOCKET_TYPE_FPGA) && !(SocketFpgasInitialized (host, KtiInternalGlobal)) ) {
      DiscEngData->Cpu[Idx1].Valid = FALSE;
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt; Idx2++) {
        if (DiscEngData->Cpu[Idx1].LinkData[Idx2].Valid == TRUE) {
          PeerPort = (UINT8)DiscEngData->Cpu[Idx1].LinkData[Idx2].PeerPort;
          PeerSocId = (UINT8)DiscEngData->Cpu[Idx1].LinkData[Idx2].PeerSocId;
          DiscEngData->Cpu[PeerSocId].LinkData[PeerPort].Valid = FALSE;
        }
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  Identifies all the rings present in a topology. A ring is defined as 4 sockets
  forming a circle. Each ring is assigned a unique number and the sockets
  are marked with the number of rings that they belong to and ring number that they
  belong to. This routine also identifies the pair of nodes that belong to more than
  one ring.

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to discovery engine data

   @retval 0     - Success
   @retval Other - Failure

**/
KTI_STATUS
LocateRings (
  struct sysHost      *host,
  KTI_DISC_DATA       *DiscEngData
  )
{
  TOPOLOGY_TREE_NODE  RingTree[MAX_RING_TREE_NODES];
  UINT8               Index;

  KtiCheckPoint (0xFF, 0xFF, 0xFF, STS_TOPOLOGY_DISCOVERY, MINOR_STS_DETECT_RING, host);
  KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n  Locating the Rings Present in the Topology\n"));

  //
  // Locate all the rings present in the system
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (DiscEngData->Cpu[Index].Valid == TRUE) {
      MemSetLocal ((VOID *) (RingTree), 0x00, sizeof (RingTree));
      ConstructRingTree (host, DiscEngData, RingTree, Index);
      CheckForRings (host, DiscEngData, RingTree, Index);
    }
  }

  //
  // Dump all the rings present in the system
  //
  if (DiscEngData->TotRings == 0) {
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  No Rings Found"));
  } else {
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  %u Ring(s) Found", DiscEngData->TotRings));
    for (Index = 0; Index < DiscEngData->TotRings; Index++) {
      KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n    Ring %u:   CPU%u -> CPU%u -> CPU%u -> CPU%u -> CPU%u",
                  Index, DiscEngData->Rings[Index][0], DiscEngData->Rings[Index][1],
                  DiscEngData->Rings[Index][2], DiscEngData->Rings[Index][3], DiscEngData->Rings[Index][4]));
    }
  }

  //
  // Parse the rings and identify the pair of nodes that belong to more than 1 ring
  //
  if (DiscEngData->TotRings > 0) {
    ParseRings (host, DiscEngData);
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n\n  Ring Pairs:"));
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if (DiscEngData->Cpu[Index].Valid == TRUE && DiscEngData->RingNodePairs[Index] != 0xFF) {
        KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n    %u  -  %u", Index, DiscEngData->RingNodePairs[Index]));
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  Construct a simple tree for each socket with nodes spanning upto 4 hops. Then parse
  the tree to look for any rings.

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data
   @param RingTree    - Pointer to full system KTI fabric tree
   @param RootSocId   - Root Socket Id

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
ConstructRingTree (
  struct sysHost             *host,
  KTI_DISC_DATA              *DiscEngData,
  TOPOLOGY_TREE_NODE         *RingTree,
  UINT8                      RootSocId
  )
{
  TOPOLOGY_TREE_NODE  Child, Parent, GrandParent, RootSocket;
  KTI_LINK_DATA       ParentLep;
  KTI_STATUS          Status;
  UINT8               Index;
  UINT8               KtiLink;

  GrandParent.RawNode        = 0;
  //
  //  Add this socket as Root of the topology tree
  //
  RootSocket.RawNode          = 0;
  RootSocket.Node.Valid       = TRUE;
  RootSocket.Node.SocId       = RootSocId;
  RootSocket.Node.SocType     = SOCKET_TYPE_CPU;
  RootSocket.Node.ParentPort  = 0xF;
  RootSocket.Node.Hop         = 0;
  RootSocket.Node.ParentIndex = 0x7F;
  if ((Status = AddNodeToRingTree (host, RingTree, &RootSocket)) != KTI_SUCCESS) {
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Adding CPU%u to Ring Tree of CPU%u failed - Data Structure Overflow.",
                    RootSocket.Node.SocId, RootSocket.Node.SocId));
    KTI_ASSERT (FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
  }

  //
  // Do a Breadth First Search (BSF) and construct the tree. To start with, the RingTree must
  // have the root socket already. The RingTree will grow as we discover new sockets in BSF manner.
  //
  for (Index = 0; ((RingTree[Index].Node.Valid != FALSE) && (RingTree[Index].Node.SocType != SOCKET_TYPE_FPGA)); Index++) {

    //
    // Stop constructing the tree when we reach hop level 4 (hop 0 being root) since the ring formation
    // invovles only 4 nodes
    //
    Parent = RingTree[Index];
    if (Parent.Node.Hop == 4) {
      break;
    }

    if (Index != 0) {
      GrandParent.RawNode = RingTree[Parent.Node.ParentIndex].RawNode;
    }

    Child.RawNode     = 0;
    Child.Node.Valid  = TRUE;
    Child.Node.Hop    = Parent.Node.Hop + 1;
    Child.Node.ParentIndex = Index;
    if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
      for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
        ParentLep = DiscEngData->Cpu[Parent.Node.SocId].LinkData[KtiLink];

        //
        // Skip this link if it is marked as "Not Valid" or "DualLink" or "TwoSkt3Link".
        //
        if (ParentLep.Valid != TRUE || ParentLep.DualLink == TRUE || ParentLep.TwoSkt3Link == TRUE) {
          continue;
        }

        if (Index != 0 && ParentLep.PeerSocId == GrandParent.Node.SocId) {
          continue; // Skip if the link if it is pointing towards root
        }

        //
        // Get the Child Socket Id, Type and port that connects it to the parent form LEP
        //
        Child.Node.SocId      = ParentLep.PeerSocId;
        Child.Node.SocType    = ParentLep.PeerSocType;
        Child.Node.ParentPort = ParentLep.PeerPort;

        // Add the node into the Ring Tree
        if ((Status = AddNodeToRingTree (host, RingTree, &Child)) != KTI_SUCCESS) {
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Adding CPU%u to Ring Tree of CPU%u failed - Data Structure Overflow.",
                     Child.Node.SocId, RootSocket.Node.SocId));
          KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
        }
      } // for(KtiLink...)
    } else {
      //
      // This is a fatal error
      //
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Unknown Socket Type found in RingTree of CPU%u", RootSocId));
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Topology not supported\n"));
      KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE);
    }
  } // for(Index...)

  return KTI_SUCCESS;
}

/**

  Identify the rings present in the system and store if it is not found already.

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data
   @param RingTree    - Pointer to full system KTI fabric tree
   @param RootSocId   - Root Socket Id

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
CheckForRings (
  struct sysHost             *host,
  KTI_DISC_DATA              *DiscEngData,
  TOPOLOGY_TREE_NODE         *RingTree,
  UINT8                      RootSocId
  )
{
  TOPOLOGY_TREE_NODE  RingNode;
  UINT8               RingSocs[CPUS_PER_RING + 1];  // +1 needed since Element-0 is repeated in last Element to show a full circle.
  UINT8               Idx1, Idx2, Idx3;
  BOOLEAN             ValidRing;

  // Identify the index of the last node in the ring tree
  KTI_ASSERT((RingTree[0].Node.SocId == RootSocId), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
  for (Idx1 = MAX_RING_TREE_NODES - 1; Idx1 != 0; Idx1--) {
    if (RingTree[Idx1].Node.Valid == TRUE) {
      break;
    }
  }

  //
  // The ring tree is a tree that spans 4 hops. By traversing from each leaf node towards the root node and checking
  // if the leaf node is same as root node (i.e the loop completes) and nodes don't repeat we can find if it is a ring or not.
  //
  for (Idx2 = Idx1; Idx2  != 0; Idx2--) {

    RingNode = RingTree[Idx2];

    //
    // We need to explore only the leaf nodes. If we explored all leaf nodes (i.e all nodes at hop level 4) break out.
    //
    if (RingNode.Node.Hop != 4) {
      break;
    }

    // If the leaf node is not same as root node, it can form a ring, so continue to neaxt leaf
    if (RingNode.Node.SocId  !=  RootSocId) {
      continue;
    }

    //
    // We found a leaf node which is same as root node. Traverse back to the root to find out the nodes of the (possible) ring
    //
    Idx3 = (UINT8)RingNode.Node.Hop;
    while (Idx3 != 0) {
      RingSocs[Idx3] = (UINT8)RingNode.Node.SocId;
      RingNode.Node = RingTree[RingNode.Node.ParentIndex].Node;
      Idx3--;
      KTI_ASSERT((Idx3 == RingNode.Node.Hop), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
    }
    RingSocs[Idx3] = RootSocId;

    //
    // Make sure it is a valid ring. For a ring to be valid, the nodes 0 to 3 should not repeat and nodes 0 & 2, nodes 1 & 3
    // should not have any direct link between them.
    //

    // Make sure the 4 nodes of the ring are unique
    if ((RingSocs[0] == RingSocs[1]) || (RingSocs[0] == RingSocs[2]) || (RingSocs[0] == RingSocs[3]) ||
        (RingSocs[1] == RingSocs[2]) || (RingSocs[1] == RingSocs[3]) || (RingSocs[2] == RingSocs[3])) {
      continue;
    }

    // Make sure the disagonally opposite nodes dosen't have connection between them
    ValidRing = TRUE;
    for (Idx3 = 0; Idx3  < host->var.common.KtiPortCnt; Idx3++) {
      if ((DiscEngData->Cpu[RingSocs[0]].LinkData[Idx3].PeerSocId == RingSocs[2]) ||
          (DiscEngData->Cpu[RingSocs[1]].LinkData[Idx3].PeerSocId == RingSocs[3])) {
        ValidRing = FALSE;
        break;
      }
    }
    if (ValidRing == FALSE) {
      continue;
    }

    //
    // Now that we found a valid ring, make sure this ring is not detected already as part of other root sockets
    //
    ValidRing = TRUE;
    for (Idx3 = 0; Idx3 < DiscEngData->TotRings; Idx3++) {
      if (CompareRing (host, RingSocs, DiscEngData->Rings[Idx3]) == TRUE) {
        ValidRing = FALSE;
        break;
      }
    }
    if (ValidRing == TRUE) {
      MemCopy ((UINT8 *) DiscEngData->Rings[DiscEngData->TotRings], (UINT8 *) RingSocs, sizeof (DiscEngData->Rings[DiscEngData->TotRings]));
      DiscEngData->TotRings++;
    }
  } // for(Idx...)

  return KTI_SUCCESS;
}

/**

  Parse the rings and identify the number of rings and the ring number that each socket belongs to.

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
ParseRings (
  struct sysHost             *host,
  KTI_DISC_DATA              *DiscEngData
  )
{
  UINT8               TmpSocId;
  UINT8               Idx1, Idx2, TmpIdx;

  //
  // Identify the number of rings each socket is part of and the ring number
  //
  for (Idx1 = 0; Idx1 < DiscEngData->TotRings; Idx1++) {
    for (Idx2 = 0; Idx2 < CPUS_PER_RING; Idx2++) {
      TmpSocId = DiscEngData->Rings[Idx1][Idx2];
      TmpIdx = DiscEngData->RingInfo[TmpSocId][0]; // Element 0 contains the number of rings this socket is already part of
      DiscEngData->RingInfo[TmpSocId][TmpIdx+1] = Idx1; // Element 1 to x contains the number of the ring which this socket is part
      DiscEngData->RingInfo[TmpSocId][0]++; // Increase the number of ring counts of which this socket is part of
    }
  }

  //
  // Identify the pair of sockets that is part of any 2 rings in the topology
  //
  MemSetLocal ((UINT8 *) DiscEngData->RingNodePairs, 0xFF, sizeof (DiscEngData->RingNodePairs));
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->RingInfo[Idx1][0] == 2) { // If this socket is part of 2 different rings
      for (Idx2 = 0; Idx2 < MAX_SOCKET; Idx2++) { // Identify the other socket that is part of the same 2 rings
        if (Idx2 != Idx1 && DiscEngData->RingInfo[Idx2][0] == 2) {
          if ((DiscEngData->RingInfo[Idx1][1] == DiscEngData->RingInfo[Idx2][1] &&
               DiscEngData->RingInfo[Idx1][2] == DiscEngData->RingInfo[Idx2][2]) ||
              (DiscEngData->RingInfo[Idx1][1] == DiscEngData->RingInfo[Idx2][2] &&
               DiscEngData->RingInfo[Idx1][2] == DiscEngData->RingInfo[Idx2][1])) {
            DiscEngData->RingNodePairs[Idx1] = Idx2;
            break;
          }
        }
      }
    }
  }

  return KTI_SUCCESS;
}



/**

  Assign VN to the nodes. It is assigned to only the nodes that are part of a ring. Two adjacent nodes of the ring
  are assigned VN0 while the other two nodes are assigned VN1. If a pair of nodes is part of more than one ring
  such as in 8SG topology, the VN will be assigned such that each pair of nodes in all rings are different.

   For example:
    +-------------------------+
     |                         |
    S0 ----- S1       S6 ----- S7
     |       | \     /|        |
     |       |   \ /  |        |
     |       |    \   |        |
     |       |  /   \ |        |
     |       |/      \|        |
    S2 ----- S3       S4 ----- S5
     |                         |
     +-------------------------+

   VN Assignment:
      S0  -  VN0
      S1  -  VN1
      S2  -  VN0
      S3  -  VN1
      S4  -  VN0
      S5  -  VN1
      S6  -  VN0
      S7  -  VN1

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
AssignVn (
  struct sysHost          *host,
  KTI_DISC_DATA           *DiscEngData
  )
{
  UINT8               TmpSoc0, TmpSoc1, TmpSoc2;
  UINT8               Idx1, Idx2, Idx3;
  UINT8               SocVnAssigned, VnAssigned, TmpVn;

  //
  // For each ring, identify the pair of nodes that is also part of another ring and assign same VN number to those pair of nodes
  //
  SocVnAssigned = 0; // Sockets for which VN is assignment
  VnAssigned = 0; // VN (i.e VN0/VN1) assigned to each socket
  for (Idx1 = 0; Idx1 < DiscEngData->TotRings; Idx1++) {
    for (Idx2 = 0; Idx2 < CPUS_PER_RING; Idx2++) {
      TmpSoc0 = DiscEngData->Rings[Idx1][Idx2];
      TmpSoc1 = DiscEngData->RingNodePairs[TmpSoc0];

      //
      // If the node is not assigned VN already and it has a pair with which it shares another ring, assign VN to the pair of sockets.
      // Make sure the VN assigned to the pair is different from the VN, if any, already assigned to other 2 nodes of the ring.
      // Since VN is assigned to pair of nodes, just checkig the VN assigned for just of the other 2 nodes is sufficient.
      //
      if (((SocVnAssigned & (1 << TmpSoc0)) == 0) && (TmpSoc1 != 0xFF)) {
        for (Idx3 = 0; Idx3 < CPUS_PER_RING; Idx3++) {
          TmpSoc2 = DiscEngData->Rings[Idx1][Idx3];
          if (TmpSoc2 != TmpSoc0 && TmpSoc2 != TmpSoc1) {
            if (SocVnAssigned & (1 << TmpSoc2)) {
              TmpVn = (((VnAssigned >> TmpSoc2) & 0x1) == VN0) ? VN1 : VN0;
            } else {
              TmpVn = VN0;
            }
            VnAssigned = VnAssigned | (TmpVn << TmpSoc0) | (TmpVn << TmpSoc1);
            SocVnAssigned = SocVnAssigned | (1 << TmpSoc0) | (1 << TmpSoc1);
            break;
          }
        }
      }
    }
  }

  //
  // For the nodes that are part of only one ring, pair it with immediate neighbor and assign the VN to both of them.
  //
  for (Idx1 = 0; Idx1 < DiscEngData->TotRings; Idx1++) {
    //
    // Check if any of the nodes of this ring is assigned a VN already
    //
    TmpVn = 0xFF;
    for (Idx2 = 0; Idx2 < CPUS_PER_RING; Idx2++) {
      TmpSoc0 = DiscEngData->Rings[Idx1][Idx2];
      if (SocVnAssigned & (1 << TmpSoc0)) {
        TmpVn = (((VnAssigned >> TmpSoc0) & 0x1) == VN0) ? VN1 : VN0;
        break;
      }
    }

    if (TmpVn == 0xFF) {
      //
      // If none of the nodes of this ring is assigned an VN, then this ring has nodes that are part of only this ring.
      // Assign VN0 to first two nodes of the ring and VN1 to the remaining two nodes of the ring.
      //
      for (Idx2 = 0; Idx2 < CPUS_PER_RING; Idx2++) {
        TmpSoc0 = DiscEngData->Rings[Idx1][Idx2];
        VnAssigned = VnAssigned | ((Idx2 /2) << TmpSoc0);
        SocVnAssigned = SocVnAssigned | (1 << TmpSoc0);
      }
    } else {
      //
      // This means either two nodes or all 4 nodes are already assigned VN. In case 1, assign VN to the
      // remaining 2 nodes. For case 2, nothing to do.
      //
      for (Idx2 = 0; Idx2 < CPUS_PER_RING; Idx2++) {
        TmpSoc0 = DiscEngData->Rings[Idx1][Idx2];
        if ((SocVnAssigned & (1 << TmpSoc0)) == 0) {
          VnAssigned = VnAssigned | (TmpVn << TmpSoc0);
          SocVnAssigned = SocVnAssigned | (1 << TmpSoc0);
        }
      }
    }
  }

  //
  // Verify that all nodes of a ring is assigned a VN; otherwise it is fatal condition.
  //
  for (Idx1 = 0; Idx1 < DiscEngData->TotRings; Idx1++) {
    for (Idx2 = 0; Idx2 < CPUS_PER_RING; Idx2++) {
      TmpSoc0 = DiscEngData->Rings[Idx1][Idx2];
      if ((SocVnAssigned & (1 << TmpSoc0)) == 0) {
        KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n  CPU%u VN Allocation Failed - Topology Not Supported!!! \n  System Halted", TmpSoc0));
        KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
      }
    }
  }

  // Copy the assignedVN into Disc Eng structure
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      DiscEngData->Cpu[Idx1].Vn = (VnAssigned >> Idx1) & 0x1;
    }
  }

  //
  // If there are no rings, all sockets would have been assigned VN0 which will cause deadlock in some
  // degraded topology scenarios.  Assign VN1 for half of the populated sockets.
  // (Non-degraded topologies will not have this problem since they will have rings and VN will be assigned correctly).
  //
  VnAssigned = VN1;
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
      if ((DiscEngData->Cpu[Idx1].Valid == TRUE) && ((SocVnAssigned & (1 << Idx1)) == 0)) {
        DiscEngData->Cpu[Idx1].Vn = VnAssigned;
        (VnAssigned == VN0) ? (VnAssigned = VN1) : (VnAssigned = VN0); // Toggle between VN0& VN1
      }
  }

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  VN Assignment:"));
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n    %u - VN%u", Idx1, DiscEngData->Cpu[Idx1].Vn));
    }
  }

  return KTI_SUCCESS;
}

/**

  Computes the KTI CPU topology found in the system

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
ComputeTopologyTree (
  struct sysHost             *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA            *DiscEngData
  )
{
  TOPOLOGY_TREE_NODE  Parent;
  UINT8               Idx1, Idx2, Hop, RootSocId, Ring4S;

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  Constructing Topology Tree"));
  KtiCheckPoint (0xFF,  SOCKET_TYPE_CPU, 0, STS_TOPOLOGY_DISCOVERY, MINOR_STS_CONSTRUCT_TOPOLOGY_TREE, host);

  //
  // Add each populated socket as Root Node in its topology tree
  //
  Parent.RawNode          = 0;
  Parent.Node.Valid       = TRUE;
  Parent.Node.SocType     = SOCKET_TYPE_CPU;
  Parent.Node.ParentPort  = 0xF;
  Parent.Node.Hop         = 0;
  Parent.Node.ParentIndex = 0x7F;
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((DiscEngData->Cpu[Idx1].Valid == TRUE) && (DiscEngData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
      Parent.Node.SocId = Idx1;
      if (AddNodeToTopologyTree (host, DiscEngData->CpuTree[Idx1], &Parent) != KTI_SUCCESS) {
        KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Adding CPU%u to Topology Discovery Tree of CPU%u failed - Data Structure Overflow.",
                          Parent.Node.SocId, Parent.Node.SocId));
        KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_55);
      }
    }
  }

  //
  // Do the topology discovery using Breadth First Search (BSF). To start with, the Topology Tree must
  // have the root socket already added. The TopologyTree will grow as we discover new sockets in BSF manner.
  // 4S ring topology is a special case; to avoid deadlock, we need to construct the tree for sockets in the
  // order they appear in the ring. For non-degraded 6S/8S configs, we will use the intel suggested
  // base line routing.
  //

#if MAX_SOCKET > 4
  if (DiscEngData->TotRings == 4) { // 8S
    ComputeTopologyTree8S (host, KtiInternalGlobal, DiscEngData);
  } else if (DiscEngData->TotRings == 2 && DiscEngData->TotCpus == 6) { // 6S
    ComputeTopologyTree6S (host, DiscEngData);
  } else {
    // Degraded 6S (6SG1) topology uses generic topology flow - Just as if 'DEGRADE_TOPOLOGY_AS_IS' was set.
#endif

    //
    // Identify if this is a 4S with ring topology
    //
    if (DiscEngData->TotRings == 1 && DiscEngData->CpuList == 0xF) {
      Ring4S = TRUE;
    } else {
      Ring4S = FALSE;
    }
    for (Hop = 0; Hop < MAX_SOCKET; Hop++) {
      for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
        if (Ring4S == TRUE) {
          if (Idx1 < 4) {
            RootSocId = DiscEngData->Rings[0][Idx1];
          } else {
            break;
          }
        } else {
          RootSocId = Idx1;
        }
        if ((DiscEngData->Cpu[RootSocId].Valid == TRUE) && (DiscEngData->Cpu[RootSocId].SocType == SOCKET_TYPE_CPU)) {
          for (Idx2 = 0; DiscEngData->CpuTree[RootSocId][Idx2].Node.Valid != FALSE; Idx2++) {
            Parent = DiscEngData->CpuTree[RootSocId][Idx2];
            if (Parent.Node.Hop < Hop) {
              continue;
            } else  if (Parent.Node.Hop > Hop) {
              break;
            }
            ExploreThisNode (host, DiscEngData, RootSocId, &Parent, Idx2, Hop);
          }
        }
      }
    }

#if MAX_SOCKET > 4
  }
#endif

  return KTI_SUCCESS;
}

/**

  Computes the KTI CPU topology found in the system

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data
   @param RootSocId   - Root Socket ID
   @param Parent      - KTI fabric tree structure
   @param ParentIdx   - parent ID
   @param Hop         - Hops in path

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
ExploreThisNode (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8                      RootSocId,
  TOPOLOGY_TREE_NODE  *Parent,
  UINT8                      ParentIdx,
  UINT8                      Hop
  )
{
  TOPOLOGY_TREE_NODE  Child;
  UINT8               KtiLink;
  KTI_LINK_DATA       ParentLep;

    Child.RawNode    = 0;
    Child.Node.Valid = TRUE;
    Child.Node.Hop = Parent->Node.Hop + 1; // One level down from parent
    Child.Node.ParentIndex = ParentIdx; // Remember who is the immediate parent of this child

    for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) { // Explore all the links of this parent
      ParentLep = DiscEngData->Cpu[Parent->Node.SocId].LinkData[KtiLink];

      //
      // Skip this link if it is marked as "Not Valid" or "DualLink" or "TwoSkt3Link".
      // A Link is marked as "Not Valid" (before entering this function) when we don't
      // want to explore it. Also, for SV validation purpose, we might want to mark a link as "Not Valid"
      // to simulate different topology. When there is a dual-link exists between any two CPU sockets,
      // one of the link is marked as "DualLink" at both the ends.
      // Exploring only one link of a dual-link is sufficient to identify the neighbor.
      //
      if (ParentLep.Valid != TRUE || ParentLep.DualLink == TRUE || ParentLep.TwoSkt3Link == TRUE) {
        continue;
      }

      //
      // Get the Child Socket Id, Type and port that connects it to the parent form LEP
      //
      Child.Node.SocId      = ParentLep.PeerSocId;
      Child.Node.SocType    = ParentLep.PeerSocType;
      Child.Node.ParentPort = ParentLep.PeerPort;

      //
      // Make sure that this socket is not already found in the tree between hop level 0 and parent's hop level.
      // The rational is that, if we find child socket already at that level, then we have a shorter path to this child.
      //
      if (ShorterPathExists (host, DiscEngData->CpuTree[RootSocId], &Child) == TRUE) {
        continue;
      }

      //
      // Finally, if we have multiple paths that are of equal hop length, then choose
      // the one that is used least number of times; if they are used equal number of times, then
      // choose the one that goes thru the minimum numberd socket.
      //
      if (CheckAndAddNodeToTopologyTree (host, DiscEngData, RootSocId, Parent, &Child, Hop) != KTI_SUCCESS) {
        KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Adding CPU%u to Topology Discovery Tree of CPU%u failed.",
                              Child.Node.SocId, RootSocId));
        KTI_ASSERT(FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_5B);
      }
    } // for(KtiLink...)

  return KTI_SUCCESS;
}

/**

  Adds current KTI node to KTI system fabric

  @param host        - Pointer to the system host (root) structure
  @param DiscEngData - Pointer to socket specific data
  @param RootSocId   - Root Socket ID
  @param Parent      - KTI fabric tree structure
  @param LeafNode    - KTI fabric tree structure
  @param Hop         - Hops in path

  @retval 0 - Successful
  @retval 1 - Failure

**/
STATIC
KTI_STATUS
CheckAndAddNodeToTopologyTree (
  struct sysHost           *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8                    RootSocId,
  TOPOLOGY_TREE_NODE        *Parent,
  TOPOLOGY_TREE_NODE       *LeafNode,
  UINT8                      Hop
  )
{
  UINT8                 Idx1;
  UINT8                 Path1Usage, Path2Usage;
  TOPOLOGY_TREE_NODE    *Tree;

  KTI_ASSERT ((DiscEngData != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_56);
  KTI_ASSERT ((Parent != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_57);
  KTI_ASSERT ((LeafNode != NULL), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_58);

  Tree = DiscEngData->CpuTree[RootSocId];

  for (Idx1 = 0; Idx1 < MAX_TREE_NODES; Idx1++) {
    //
    // If the LeafNode is not already found in the tree, add it to the tree
    //
    if (Tree[Idx1].Node.Valid == FALSE) {
      Tree[Idx1] = *LeafNode;
      if (Hop != 0) {
        TrackPathLoad (host, DiscEngData, Tree, LeafNode, 0);
      }
      return KTI_SUCCESS;
    }

    //
    // If Socket ID, Type & Hop level of the node to be added matches a node in the tree
    // (i.e the LeafNode is already in the tree with different path to the root), then choose
    // the path that is used minimum number of times.
    //
    if (Tree[Idx1].Node.SocId   == LeafNode->Node.SocId   &&
        Tree[Idx1].Node.SocType == LeafNode->Node.SocType &&
        Tree[Idx1].Node.Hop     == LeafNode->Node.Hop) {

      //
      // Find the immediate parent of the leaf nodes and check how many times the immediate
      // parent used the link that connects it to the leaf node
      //
      Path1Usage = CalculatePathLoad (host, DiscEngData, Tree, &Tree[Idx1]);

      Path2Usage = CalculatePathLoad (host, DiscEngData, Tree, Parent) + (UINT8)LeafNode->Node.Hop;

      //
      // Select the path that is used minimum number of times.
      //
      if (Path2Usage < Path1Usage) {
        //
        // Replace the existing node with the leaf node
        //
        TrackPathLoad (host, DiscEngData, Tree, &Tree[Idx1], 1);
        Tree[Idx1] = *LeafNode;
        TrackPathLoad (host, DiscEngData, Tree, LeafNode, 0);
        return KTI_SUCCESS;
      } else {
        //
        // Existing path is optimum. Leave the tree intact
        //
        return KTI_SUCCESS;
      }
    }
  }

  return (KTI_STATUS) KTI_FAILURE;
}

/**

  Checks and resolves potential dead locks due to overlapping routes

  @param host        - Pointer to the system host (root) structure
  @param DiscEngData - Pointer to socket specific data
  @param Tree        - KTI fabric tree structure
  @param LeafNode    - KTI fabric tree structure
  @param Adjust      - 1 = Remove, 0 = Add to linkload

  @retval 0 - Successful
  @retval 1 - Failure

**/
STATIC
KTI_STATUS
TrackPathLoad (
  struct sysHost       *host,
  KTI_DISC_DATA        *DiscEngData,
  TOPOLOGY_TREE_NODE   *Tree,
  TOPOLOGY_TREE_NODE   *LeafNode,
  UINT8                      Adjust
  )
{
  UINT8               Port0; // Parent Port that connects it to the child
  TOPOLOGY_TREE_NODE  Child, Parent;

  Child = *LeafNode;
  while (Child.Node.Hop != 0) {
    Parent = Tree[Child.Node.ParentIndex];
    Port0 = GetChildLocalPortFromLEP (host, DiscEngData->Cpu, &Parent, &Child);
    if ((Port0 == 0xFF) || (Port0 >= MAX_KTI_PORTS)) {
      KTI_ASSERT ((Port0 < MAX_KTI_PORTS), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_59);
      break;
    }
    if (Adjust == 0) {
      DiscEngData->LinkLoad[Parent.Node.SocId][Port0]++;
    } else {
      KTI_ASSERT ((DiscEngData->LinkLoad[Parent.Node.SocId][Port0] != 0), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_5C);
      DiscEngData->LinkLoad[Parent.Node.SocId][Port0]--;
    }
    Child = Parent;
  }

  return KTI_SUCCESS;
}

/**

  Checks and resolves potential dead locks due to overlapping routes

  @param host        - Pointer to the system host (root) structure
  @param DiscEngData - Pointer to socket specific data
  @param Tree        - KTI fabric tree structure
  @param LeafNode    - KTI fabric tree structure

**/
STATIC
UINT8
CalculatePathLoad (
  struct sysHost       *host,
  KTI_DISC_DATA        *DiscEngData,
  TOPOLOGY_TREE_NODE   *Tree,
  TOPOLOGY_TREE_NODE   *LeafNode
  )
{
  UINT8               Port0; // Parent Port that connects it to the child
  UINT8               Usage;
  TOPOLOGY_TREE_NODE  Child, Parent;

  Usage = 0;
  Child = *LeafNode;
  while (Child.Node.Hop != 0) {
    Parent = Tree[Child.Node.ParentIndex];
    Port0 = GetChildLocalPortFromLEP (host, DiscEngData->Cpu, &Parent, &Child);
    if ((Port0 == 0xFF) || (Port0 >= MAX_KTI_PORTS)) {
      KTI_ASSERT ((Port0 < MAX_KTI_PORTS), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_5A);
      break;
    }
    Usage = Usage + DiscEngData->LinkLoad[Parent.Node.SocId][Port0];
    Child = Parent;
  }

  return Usage;
}

/**

  Treat the topology tree as a Directed Graph and check if the Graph is Directed Acylic Graph (DAG)
  or not using "Topological Sort" algorithm. If the graph is acyclic it indicates the routes are deadlock free.
  The Directed Graph consists of finite set of vertices V and Edges E. In KTI terminology, source/sink of
  KTI packets represent Vertices and the link connecting them represent Edges. So V & E for all
  supported topology is finite set as defined below:

      V = {S0...S7}{P0...P2}{VN0/VN1}{TX/RX} = 8 * 3 * 2 * 2 = 96
      E = {S0...S7}{P0...P2}{VN0/VN1} = 8 * 3 * 2 = 48

  The topology tree is converted into an array consisting the list of adjacent nodes of each node. Since
  each node has 3 links and 2 VNs, a node can have maximum 6 outgoing edges; so it can have maximum
  of 6 nodes in its adjacency list.

  Example: Consider the 4S ring topology

       0   0
    S0 ----- S1
    1|       |1
     |       |
     |       |
     |       |
    1|       |1
    S3 ----- S2
       0   0

   The route/topology tree for each socket would be:
    "S0 P0 VN0 TX" -> "S1 P0 VN0 RX";
    "S0 P1 VN0 TX" -> "S3 P1 VN0 RX";
    "S0 P0 VN0 TX" -> "S1 P0 VN0 RX" -> "S1 P1 VN0 TX" -> "S2 P1 VN0 RX";

    "S1 P0 VN0 TX" -> "S0 P0 VN0 RX";
    "S1 P1 VN0 TX" -> "S2 P1 VN0 RX";
    "S1 P0 VN0 TX" -> "S0 P0 VN0 RX" -> "S0 P1 VN0 TX" -> "S3 P1 VN0 RX";

    "S2 P0 VN0 TX" -> "S3 P0 VN0 RX";
    "S2 P1 VN0 TX" -> "S1 P1 VN0 RX";
    "S2 P0 VN0 TX" -> "S3 P0 VN0 RX" -> "S3 P1 VN0 TX" -> "S0 P1 VN0 RX";

    "S3 P0 VN0 TX" -> "S2 P0 VN0 RX";
    "S3 P1 VN0 TX" -> "S0 P1 VN0 RX";
    "S3 P0 VN0 TX" -> "S2 P0 VN0 RX" -> "S2 P1 VN0 TX" -> "S1 P1 VN0 RX";

    The above topology tree can be represented as an array of adjacency list as shown below:
    Node           Adjacency List
    S0 P0 VN0 TX   S1 P0 VN0 RX
    S0 P0 VN0 RX   S0 P1 VN0 TX
    S0 P1 VN0 TX   S3 P1 VN0 RX
    S1 P0 VN0 TX   S0 P0 VN0 RX
    S1 P0 VN0 RX   S1 P1 VN0 TX
    S1 P1 VN0 TX   S2 P1 VN0 RX
    S2 P0 VN0 TX   S3 P0 VN0 RX
    S2 P0 VN0 RX   S2 P1 VN0 TX
    S2 P1 VN0 TX   S1 P1 VN0 RX
    S3 P0 VN0 TX   S2 P0 VN0 RX
    S3 P0 VN0 RX   S3 P1 VN0 TX
    S3 P1 VN0 TX   S0 P1 VN0 RX

    While this particular topology has only one adjacent node for each node, complex topologies such as 8SG will
    have more than one node in the adjacency list.

    By applying the topological sort on the above table, it can be detected if it is DAG or not.

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
DetectDeadlock (
  struct sysHost      *host,
  KTI_DISC_DATA       *DiscEngData
  )
{
  UINT8               Idx1, Idx2, Idx3;
  UINT8               GrandParentIdx, ParentIdx, ChildIdx;
  UINT8               Port1, Port2;
  UINT8               RootVn, Temp1;
  UINT8               Vertices[MAX_SOCKET * MAX_KTI_PORTS * 2 * 2];
  UINT8               AdjList[MAX_SOCKET * MAX_KTI_PORTS * 2 * 2][MAX_KTI_PORTS * 2];
  UINT8               Indegree[MAX_SOCKET * MAX_KTI_PORTS * 2 * 2];
  BOOLEAN             DeadlockFound;
  TOPOLOGY_TREE_NODE  *TopologyTree;
  TOPOLOGY_TREE_NODE  *Parent, *Child;

  MemSetLocal ((VOID *) Vertices, 0x0, sizeof (Vertices));
  MemSetLocal ((VOID *) AdjList, 0xFF, sizeof (AdjList));
  MemSetLocal ((VOID *) Indegree, 0x0, sizeof (Indegree));

  //
  // Construct the adjacency list. Note that the pass thru route is split into two separate independent nodes;
  // one containing the RX and the other containing the TX at the intermediate node. The list is contructed
  // by parsing the topology tree of each socket. In other words, the graph is nothing but collection of the
  // topology tree of each node.
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      //
      // VN assignment is at the soure (root node) and they don't change along the path. Identify the
      // root node's VN assignment.
      //
      TopologyTree = DiscEngData->CpuTree[Idx1];
      RootVn = DiscEngData->Cpu[TopologyTree[0].Node.SocId].Vn;

      //
      // Traverse the topology tree of this socket and mark the Vertices found in its tree.
      //
     for (Idx2 = 1; TopologyTree[Idx2].Node.Valid != FALSE; Idx2++) {
        //
        // Extract the Parent and Child nodes
        //
        Child = &TopologyTree[Idx2];
        Port2 = (UINT8)Child->Node.ParentPort; // Child's Port that connects it to the parent
        Port1 = (UINT8)DiscEngData->Cpu[Child->Node.SocId].LinkData[Port2].PeerPort; // Parent's Port that connects it to this child
        Parent = &TopologyTree[Child->Node.ParentIndex];

        //
        // Mark the corresponding vertices of the parent & child as valid.
        // Note that the parent is always at "TX" node and the child is always at "RX" node.
        //
        ParentIdx = (UINT8)(Parent->Node.SocId * MAX_KTI_PORTS * 2 * 2) + (Port1 * 2 * 2) + (RootVn * 2) + TX;
        ChildIdx = (UINT8)(Child->Node.SocId * MAX_KTI_PORTS * 2 * 2) + (Port2 * 2 * 2) + (RootVn * 2) + RX;
        Vertices[ParentIdx] = TRUE;
        Vertices[ChildIdx] = TRUE;

        //
        // Record the child as adjacent node in parent's adjacency list if it is not already found.
        //
        for (Idx3 = 0; Idx3 < host->var.common.KtiPortCnt * 2; Idx3++) {
          if (AdjList[ParentIdx][Idx3] == ChildIdx) {
            break;
          } else if (AdjList[ParentIdx][Idx3] == 0xFF) {
            AdjList[ParentIdx][Idx3] = ChildIdx;
            break;
          }
        }

        //
        // If the parent is not the root node, make sure that the pass thru route is split into separate nodes corresponding
        // to RX & TX and recorded in the adjacency list.
        //
        if (Parent->Node.Hop != 0) {
          Port1 = (UINT8)Parent->Node.ParentPort; // Port connecting the parent to grand parent
          GrandParentIdx = (UINT8)(Parent->Node.SocId * MAX_KTI_PORTS * 2 * 2) + (Port1 * 2 * 2) + (RootVn * 2) + RX;
          for (Idx3 = 0; Idx3 < host->var.common.KtiPortCnt * 2; Idx3++) {
            if (AdjList[GrandParentIdx][Idx3] == ParentIdx) {
              break;
            } else if (AdjList[GrandParentIdx][Idx3] == 0xFF) {
              AdjList[GrandParentIdx][Idx3] = ParentIdx;
              break;
            }
          }
        }

      }
    }
  }


  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n Adjacency Table"));
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n ----------------"));
  for (Idx1 = 0; Idx1 < MAX_SOCKET * MAX_KTI_PORTS * 2 * 2; Idx1++) {
    if (Vertices[Idx1] == TRUE) {
      Temp1 = Idx1 % (MAX_KTI_PORTS * 2 * 2);
      Port1 = Temp1 / (2 * 2);
      RootVn = (Temp1 % (2 * 2)) / 2;
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\nS%u P%u VN%u %s (%02u) :", Idx1 / (MAX_KTI_PORTS * 2 * 2), Port1, RootVn, (Idx1 & 0x1) ? "RX" : "TX", Idx1));
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt * 2; Idx2++) {
        if (AdjList[Idx1][Idx2] != 0xFF) {
          Temp1 = AdjList[Idx1][Idx2] % (MAX_KTI_PORTS * 2 * 2);
          Port2 = Temp1 / (2 * 2);
          RootVn = (Temp1 % (2 * 2)) / 2;
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "   S%u P%u VN%u %s (%02u)", AdjList[Idx1][Idx2] / (MAX_KTI_PORTS * 2 * 2), Port2, RootVn,
              (AdjList[Idx1][Idx2] & 0x1) ? "RX" : "TX", AdjList[Idx1][Idx2]));
        }
      }
    }
  }


  //
  // Do topological sort of the graph. A topological sort is an ordering of the vertices in a directed graph
  // such that if there is a path from node i to j, then node j appears after node i in the ordering.
  // If there is a cycle in the graph then such ordering is not possible and it indicates a deadlock in the route.
  // Note that we are not interested in doing the actual sorting. We just knock out the vertices with indegree
  // 0 and repeat the loop for all nodes. At the end if we find any node with non-zero indegree, it implies
  // there is a cycle in the graph. Also note that the list of nodes in the array is nothing but the nodes that
  // constitue the cycle in the graph.
  //

  //
  // Construct a table containing the indgree of each node.
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET * MAX_KTI_PORTS * 2 * 2; Idx1++) {
    if (Vertices[Idx1] == TRUE) {
      for (Idx3 = 0; Idx3 < host->var.common.KtiPortCnt * 2; Idx3++) {
        if (AdjList[Idx1][Idx3] == 0xFF) {
          break;
        } else {
          Indegree[AdjList[Idx1][Idx3]]++;
        }
      }
    }
  }


  //
  // Remove all nodes with indegree zero and all its outgoing edges by decreasing the indegree of the nodes
  // that are connected by the edges being removed.
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET * MAX_KTI_PORTS * 2 * 2; Idx1++) {
    for (Idx2 = 0; Idx2 < MAX_SOCKET * MAX_KTI_PORTS * 2 * 2; Idx2++) {
      if (Vertices[Idx2] == TRUE && Indegree[Idx2] == 0) {
        Vertices[Idx2] = FALSE;
        for (Idx3 = 0; Idx3 < host->var.common.KtiPortCnt * 2; Idx3++) {
          if (AdjList[Idx2][Idx3] == 0xFF) {
            break;
          } else {
            Indegree[AdjList[Idx2][Idx3]]--;
          }
        }
      }
    }
  }

  //
  // Check for cycle and print the nodes that form the cycle. If we find any valid node in the vertices it indicates a cycle.
  //
  DeadlockFound = FALSE;
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n Checking for Deadlock..."));
  for (Idx1 = 0; Idx1 < MAX_SOCKET * MAX_KTI_PORTS * 2 * 2; Idx1++) {
    if (Vertices[Idx1] == TRUE) {
      DeadlockFound = TRUE;
      for (Idx2 = 0; Idx2 < host->var.common.KtiPortCnt * 2; Idx2++) {
        if (AdjList[Idx1][Idx2] == 0xFF) {
          break;
        } else {
          Temp1 = Idx1 % (MAX_KTI_PORTS * 2 * 2);
          Port1 = Temp1 / (2 * 2);
          RootVn = (Temp1 % (2 * 2)) / 2;
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\nS%u P%u VN%u %s ->  ", Idx1 / (MAX_KTI_PORTS * 2 * 2), Port1, RootVn, (Idx1 & 0x1) ? "RX" : "TX"));
          Temp1 = AdjList[Idx1][Idx2] % (MAX_KTI_PORTS * 2 * 2);
          Port2 = Temp1 / (2 * 2);
          RootVn = (Temp1 % (2 * 2)) / 2;
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "S%u P%u VN%u %s", AdjList[Idx1][Idx2] / (MAX_KTI_PORTS * 2 * 2), Port2, RootVn, (AdjList[Idx1][Idx2] & 0x1) ? "RX" : "TX"));
        }
      }
    }
  }

  if (DeadlockFound == TRUE) {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n Dead Lock Found!!! \n Topology Not Supported. System Halted\n"));
    KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
  }

  return KTI_SUCCESS;
}

/**

   Copies the Si agnostic toplogy tree created by the discovery engine and apply the port mapping to make Si specific.
   Also copies the VN assigned, and the LEP Info and Topology Tree Info into host structure

   @param host              - Pointer to the system host (root) structure
   @param SocketData        - Pointer to socket specific data
   @param KtiInternalGlobal - KTIRC internal variables.
   @param DiscEngData       - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
CopyTopologyTree (
  struct sysHost      *host,
  KTI_SOCKET_DATA      *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA       *DiscEngData
  )
{
  UINT8               Idx1, Idx2;
  UINT8               TempPort;

  //
  // Copy the Si agnostic toplogy tree created by the discovery engine and apply the port mapping to make Si specific.
  //
  MemCopy ((UINT8 *) SocketData->CpuTree, (UINT8 *) DiscEngData->CpuTree, sizeof (SocketData->CpuTree));
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((DiscEngData->Cpu[Idx1].Valid == TRUE) && (DiscEngData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {
      for (Idx2 = 1; DiscEngData->CpuTree[Idx1][Idx2].Node.Valid == TRUE; Idx2++) { // Skip the root node since it doesn't have parent
        TempPort = (UINT8) DiscEngData->CpuTree[Idx1][Idx2].Node.ParentPort;
        SocketData->CpuTree[Idx1][Idx2].Node.ParentPort = TranslateToSiPort (host, TempPort);
      }
    }
  }

  // Copy the VN assigned
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (SocketData->Cpu[Idx1].Valid == TRUE) {
      SocketData->Cpu[Idx1].Vn = DiscEngData->Cpu[Idx1].Vn;
    }
  }

  // Copy the LEP Info and Topology Tree Info into host structure
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (SocketData->Cpu[Idx1].Valid == TRUE) {
      host->var.kti.CpuInfo[Idx1].Valid  = SocketData->Cpu[Idx1].Valid;
      host->var.kti.CpuInfo[Idx1].SocId  = SocketData->Cpu[Idx1].SocId;
      host->var.kti.CpuInfo[Idx1].SocType  = SocketData->Cpu[Idx1].SocType;
      MemCopy ((UINT8 *) host->var.kti.CpuInfo[Idx1].LepInfo, (UINT8 *) SocketData->Cpu[Idx1].LinkData, sizeof (host->var.kti.CpuInfo[Idx1].LepInfo));
      MemCopy ((UINT8 *) host->var.kti.CpuInfo[Idx1].TopologyInfo, (UINT8 *) SocketData->CpuTree[Idx1], sizeof (host->var.kti.CpuInfo[Idx1].TopologyInfo));
    }
  }

  TopologyInfoDump (host, SocketData->Cpu, SocketData->CpuTree, TRUE);

  return KTI_SUCCESS;
}

#if MAX_SOCKET > 4

/**

  Check this Child Node is in this route

   @param Child       - Pointer to child topology tree node
   @param DiscEngData - Pointer to socket specific data

   @retval TRUE  - yes
   @retval FALSE - no

**/
BOOLEAN
CheckChildNodeInThisRoute (
  UINT8               RootSocket,
  TOPOLOGY_TREE_NODE  *Parent,
  TOPOLOGY_TREE_NODE  *Child,
  UINT8               (*OemRoute)[4][2]
  )
{
   BOOLEAN       AddNode;
   UINT8         PathNum;

   //
   // If it is 1 Hop away node (i.e immediate neighbor), just add the node
   //
  AddNode = FALSE;
  if (Child->Node.Hop == 1) {
    AddNode = TRUE;
  } else {
    //
    // If it is 2 Hop away node, make sure the template routing indicates this route to be used
    //
    for (PathNum = 0; PathNum < 4; PathNum++) {
      if (OemRoute[RootSocket][PathNum][0] == Parent->Node.SocId && OemRoute[RootSocket][PathNum][1] == Child->Node.SocId) {
        AddNode = TRUE;
        break;
      }
    }
  }

  return AddNode;
}

/**

  Check this Child Node is in this route

   @param Child       - Pointer to child topology tree node
   @param DiscEngData - Pointer to socket specific data

   @retval TRUE  - yes
   @retval FALSE - no

**/
BOOLEAN
CheckChildNodeInThisRoute_VN1Disable (
  KTI_DISC_DATA       *DiscEngData,
  UINT8               RootSocket,
  TOPOLOGY_TREE_NODE  *Parent,
  TOPOLOGY_TREE_NODE  *Child,
  UINT8               (*OemRoute_VN1Disable)[4][3]
  )
{
  BOOLEAN       AddNode;
  UINT8         PathNum;
  TOPOLOGY_TREE_NODE  GrandParent;

  //
  // If it is 1 Hop away node (i.e immediate neighbor), just add the node
  //
  AddNode = FALSE;
  if (Child->Node.Hop == 1) {
    AddNode = TRUE;
  } else if (Child->Node.Hop == 2) {
    //
    // If it is 2 Hop away node, make sure the template routing indicates this route to be used
    //
    for (PathNum = 0; PathNum < 4; PathNum++) {
      if (OemRoute_VN1Disable[RootSocket][PathNum][0] == Parent->Node.SocId &&
          OemRoute_VN1Disable[RootSocket][PathNum][1] == Child->Node.SocId ) {
          AddNode = TRUE;
          break;
      }
    }
  } else {
    //
    // If it is 3 Hop away node, make sure the template routing indicates this route to be used
    //
    GrandParent = DiscEngData->CpuTree[RootSocket][Parent->Node.ParentIndex];
    for (PathNum = 0; PathNum < 4; PathNum++) {
      if (OemRoute_VN1Disable[RootSocket][PathNum][0] == GrandParent.Node.SocId &&
          OemRoute_VN1Disable[RootSocket][PathNum][1] == Parent->Node.SocId &&
          OemRoute_VN1Disable[RootSocket][PathNum][2] == Child->Node.SocId) {
        AddNode = TRUE;
        break;
      }
    }
  }

  return AddNode;
}


/**

  Computes the KTI CPU topology found in the system

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
ComputeTopologyTree8S (
  struct sysHost             *host,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  KTI_DISC_DATA            *DiscEngData
  )
{
  UINT8         OemRoute_VN1Disable[MAX_SOCKET][4][3];
  UINT8         OemRoute[MAX_SOCKET][4][2];
  UINT8         Idx1, Idx2, KtiLink, MaxHop;
  TOPOLOGY_TREE_NODE  Parent, Child;
  KTI_LINK_DATA       ParentLep;
  BOOLEAN       AddNode;

  MemSetLocal ((VOID *) OemRoute, 0xFF, sizeof (OemRoute));
  MemSetLocal ((VOID *) OemRoute_VN1Disable, 0xFF, sizeof (OemRoute_VN1Disable));
  Map8SNodes (host, DiscEngData);

  if( host->var.kti.OutVn1En == TRUE) {
    Map8SRoutes (host, DiscEngData, OemRoute);
    MaxHop = 2;
  } else {
    Map8SRoutes_VN1Disable(host, DiscEngData, OemRoute_VN1Disable);
    MaxHop = 3;
  }


  //
  // Construct the topology tree that uses the template routing
  //
  Child.RawNode    = 0;
  Child.Node.Valid = TRUE;
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      for (Idx2 = 0; DiscEngData->CpuTree[Idx1][Idx2].Node.Valid == TRUE && DiscEngData->CpuTree[Idx1][Idx2].Node.Hop < MaxHop; Idx2++) {

        Parent = DiscEngData->CpuTree[Idx1][Idx2];
        Child.Node.Hop = Parent.Node.Hop + 1; // One level down from parent
        Child.Node.ParentIndex = Idx2; // Remember who is the immediate parent of this child

        for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) { // Explore all the links of this parent

          ParentLep = DiscEngData->Cpu[Parent.Node.SocId].LinkData[KtiLink];
          if (ParentLep.Valid != TRUE || ParentLep.DualLink == TRUE || ParentLep.TwoSkt3Link == TRUE) {
            continue;
          }

          //
          // Get the Child Socket Id, Type and port that connects it to the parent form LEP
          //
          Child.Node.SocId      = ParentLep.PeerSocId;
          Child.Node.SocType    = ParentLep.PeerSocType;
          Child.Node.ParentPort = ParentLep.PeerPort;

          //
          // Make sure that this socket is not already found in the tree between hop level 0 and parent's hop level.
          // The rational is that, if we find child socket already at that level, then we have a shorter path to this child.
          //
          if (ShorterPathExists (host, DiscEngData->CpuTree[Idx1], &Child) == TRUE) {
            continue;
          }

          if( host->var.kti.OutVn1En == TRUE) {
             AddNode = CheckChildNodeInThisRoute(Idx1, &Parent, &Child, OemRoute);
          } else {
             AddNode = CheckChildNodeInThisRoute_VN1Disable(DiscEngData, Idx1, &Parent, &Child, OemRoute_VN1Disable);
          }

          if (AddNode == FALSE) {
            continue;
          }

          if (AddNodeToTopologyTree (host, DiscEngData->CpuTree[Idx1], &Child) != KTI_SUCCESS) {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Adding CPU%u to Topology Discovery Tree of CPU%u failed.",
                              Child.Node.SocId, Idx1));
            KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_5D);
          }
        } // for(KtiLink...)
      }
    }
  }

  return KTI_SUCCESS;
}

/**

  This routine establishes a mapping between intel's ref platform and this platform. Port number is not relevant.

  Ref Platform:

     +-------------------------+
     |                         |
    S0 ----- S1       S6 ----- S7
     |       | \     /|        |
     |       |   \ /  |        |
     |       |    \   |        |
     |       |  /   \ |        |
     |       |/      \|        |
    S2 ----- S3       S4 ----- S5
     |                         |
     +-------------------------+


  OEM Platform:

     +-------------------------+
     |                         |
    S1 ----- S2       S3 ----- S6
     |       | \     /|        |
     |       |   \ /  |        |
     |       |    \   |        |
     |       |  /   \ |        |
     |       |/      \|        |
    S5 ----- S7       S0 ----- S4
     |                         |
     +-------------------------+



  Due to isomorphic nature of the 8SG topology, the OEM Platform can be redrawn as:

     +-------------------------+
     |                         |
    S0 ----- S4       S1 ----- S2
     |       | \     /|        |
     |       |   \ /  |        |
     |       |    \   |        |
     |       |  /   \ |        |
     |       |/      \|        |
    S3 ----- S6       S5 ----- S7
     |                         |
     +-------------------------+


  So the socket mapping would be:

    Intel   OEM
      S0     S0
      S1     S4
      S2     S3
      S3     S6
      S4     S5
      S5     S7
      S6     S1
      S7     S2

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
Map8SNodes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData
  )
{
  UINT8               Idx1, KtiLink;
  UINT8               RingNum1, RingNum2, RingNum3;
  UINT8               RefSoc;
  KTI_LINK_DATA       ParentLep;
  UINT8               TempSoc1 =0 , TempSoc2=0;


  //
  // Begin with S0 of this platform, identify its pair and map it to S0, S2 of ref platform
  //
  RefSoc = 0;
  DiscEngData->SocMap[0] = RefSoc;
  DiscEngData->SocMap[2] = DiscEngData->RingNodePairs[RefSoc];

  // Identify S0's two rings present in this platform
  RingNum1 = DiscEngData->RingInfo[RefSoc][1];
  RingNum2 = DiscEngData->RingInfo[RefSoc][2];

  // Identify the other 2 nodes of S0's first ring
  for (Idx1 = 0; Idx1 < CPUS_PER_RING; Idx1++) {
    if (DiscEngData->Rings[RingNum1][Idx1] == RefSoc || DiscEngData->Rings[RingNum1][Idx1] == DiscEngData->RingNodePairs[RefSoc]) {
      continue;
    }
    TempSoc1 = DiscEngData->Rings[RingNum1][Idx1];
    break;
  }
  TempSoc2 = DiscEngData->RingNodePairs[TempSoc1];

  //
  // Make sure TempSoc2 is the diagonally opposite in S0's first ring; otherwise swap out TempSoc1 & TempSoc2
  //
  for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
    ParentLep = DiscEngData->Cpu[RefSoc].LinkData[KtiLink];
    if (ParentLep.Valid == TRUE) {
      if (ParentLep.PeerSocId == TempSoc2) {
        TempSoc2 = TempSoc1;
        TempSoc1 = (UINT8)ParentLep.PeerSocId;
        break;
      }
    }
  }

  // TempSoc1 and TempSoc2 can be now mapped to S1 and S3 in intel ref platform
  DiscEngData->SocMap[1] = TempSoc1;
  DiscEngData->SocMap[3] = TempSoc2;


  // Identify the other 2 nodes of S0's second ring
  for (Idx1 = 0; Idx1 < CPUS_PER_RING; Idx1++) {
    if (DiscEngData->Rings[RingNum2][Idx1] == RefSoc || DiscEngData->Rings[RingNum2][Idx1] == DiscEngData->RingNodePairs[RefSoc]) {
      continue;
    }
    TempSoc1 = DiscEngData->Rings[RingNum2][Idx1];
    break;
  }
  TempSoc2 = DiscEngData->RingNodePairs[TempSoc1];

  //
  // Make sure TempSoc2 is the diagonally opposite in S0's second ring; otherwise swap out TempSoc1 & TempSoc2
  //
  for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
    ParentLep = DiscEngData->Cpu[RefSoc].LinkData[KtiLink];
    if (ParentLep.Valid == TRUE) {
      if (ParentLep.PeerSocId == TempSoc2) {
        TempSoc2 = TempSoc1;
        TempSoc1 = (UINT8)ParentLep.PeerSocId;
        break;
      }
    }
  }

  // TempSoc1 and TempSoc2 can be now mapped to S7 and S5 in intel ref platform
  DiscEngData->SocMap[7] = TempSoc1;
  DiscEngData->SocMap[5] = TempSoc2;


  RefSoc = DiscEngData->SocMap[1];

  // Identify RefSoc's ring that doesn't belong to S0
  if (DiscEngData->RingInfo[RefSoc][1] == RingNum1 || DiscEngData->RingInfo[RefSoc][1] == RingNum2) {
    RingNum3 = DiscEngData->RingInfo[RefSoc][2];
  } else {
    RingNum3 = DiscEngData->RingInfo[RefSoc][1];
  }

  // Identify the other 2 nodes ofRefSoc's ring
  for (Idx1 = 0; Idx1 < CPUS_PER_RING; Idx1++) {
    if (DiscEngData->Rings[RingNum3][Idx1] == RefSoc || DiscEngData->Rings[RingNum3][Idx1] == DiscEngData->RingNodePairs[RefSoc]) {
      continue;
    }
    TempSoc1 = DiscEngData->Rings[RingNum3][Idx1];
    break;
  }
  TempSoc2 = DiscEngData->RingNodePairs[TempSoc1];

  //
  // Make sure TempSoc2 is the diagonally opposite in RefSoc's ring; otherwise swap out TempSoc1 & TempSoc2
  //
  for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
    ParentLep = DiscEngData->Cpu[RefSoc].LinkData[KtiLink];
    if (ParentLep.Valid == TRUE) {
      if (ParentLep.PeerSocId == TempSoc2) {
        TempSoc2 = TempSoc1;
        TempSoc1 = (UINT8)ParentLep.PeerSocId;
        break;
      }
    }
  }

  // TempSoc1 and TempSoc2 can be now mapped to S4 and S6 in intel ref platform
  DiscEngData->SocMap[4] = TempSoc1;
  DiscEngData->SocMap[6] = TempSoc2;

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n    Socket Mapping"));
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n      %u  -  %u ", Idx1, DiscEngData->SocMap[Idx1]));
  }

  return KTI_SUCCESS;
}

/**

  Uses the following static routing as template and produces the routing for the given platform
  using the socket mapping.

  S0 -> S2 -> S3;   S0 -> S7 -> S6;   S0 -> S1 -> S4;   S0 -> S2 -> S5
  S1 -> S3 -> S2;   S1 -> S3 -> S6;   S1 -> S4 -> S5;   S1 -> S0 -> S7
  S2 -> S0 -> S1;   S2 -> S3 -> S6;   S2 -> S5 -> S4;   S2 -> S0 -> S7
  S3 -> S1 -> S0;   S3 -> S1 -> S4;   S3 -> S2 -> S5;   S3 -> S6 -> S7
  S4 -> S1 -> S0;   S4 -> S5 -> S2;   S4 -> S6 -> S3;   S4 -> S6 -> S7
  S5 -> S7 -> S0;   S5 -> S2 -> S3;   S5 -> S4 -> S1;   S5 -> S7 -> S6
  S6 -> S7 -> S0;   S6 -> S3 -> S2;   S6 -> S4 -> S1;   S6 -> S4 -> S5
  S7 -> S5 -> S2;   S7 -> S6 -> S3;   S7 -> S0 -> S1;   S7 -> S5 -> S4

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data
   @param OemRoute    -

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
Map8SRoutes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8         (*OemRoute)[4][2]
  )
{
  UINT8         RefRoute[8][4][2] = {
                {{2, 3}, {7, 6}, {1, 4}, {2, 5}}, // S0
                {{3, 2}, {3, 6}, {4, 5}, {0, 7}}, // S1
                {{0, 1}, {3, 6}, {5, 4}, {0, 7}}, // S2
                {{1, 0}, {1, 4}, {2, 5}, {6, 7}}, // S3
                {{1, 0}, {5, 2}, {6, 3}, {6, 7}}, // S4
                {{7, 0}, {2, 3}, {4, 1}, {7, 6}}, // S5
                {{7, 0}, {3, 2}, {4, 1}, {4, 5}}, // S6
                {{5, 2}, {6, 3}, {0, 1}, {5, 4}}  // S7
             };
  UINT8         Idx1, Idx2, Idx3, OemSoc;

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    OemSoc = DiscEngData->SocMap[Idx1];
    for (Idx2 = 0; Idx2 < 4; Idx2++) {
      for (Idx3 = 0; Idx3 < 2; Idx3++) {
        OemRoute[OemSoc][Idx2][Idx3] = DiscEngData->SocMap[RefRoute[Idx1][Idx2][Idx3]];
      }
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n"));
    for (Idx2 = 0; Idx2 < 4; Idx2++) {
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "  S%u -> S%u -> S%u", Idx1, OemRoute[Idx1][Idx2][0], OemRoute[Idx1][Idx2][1]));
      KTI_ASSERT ((OemRoute[Idx1][Idx2][0] != 0xFF && OemRoute[Idx1][Idx2][1] != 0xFF), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_5F);
    }
  }

  return KTI_SUCCESS;
}

/**

  Uses the following static routing as template and produces the routing for the given platform
  using the socket mapping.

  S0 -> S7;  S0 -> S2 -> S5; S0 -> S1 -> S4 -> S6; S0 -> S2 -> S3
  S1 -> S0 -> S7; S1 -> S3 -> S2 -> S5; S1 -> S4 -> S6
  S2 -> S0 -> S7; S2 -> S5 -> S4; S2 -> S3 -> S6;  S2 -> S0 -> S1
  S3 -> S6 -> S7; S3 -> S2 -> S5; S3 -> S1 -> S4;  S3 -> S1 -> S0
  S4 -> S5 -> S7; S4 -> S6; S4 -> S1 -> S0; S4 -> S5 -> S2 -> S3
  S5 -> S7 -> S6; S5 -> S7 -> S0; S5 -> S4 -> S1; S5 -> S2 -> S3
  S6 -> S4 -> S5; S6 -> S3 -> S2; S6 -> S7 -> S0; S6 -> S4 -> S1
  S7 -> S5 -> S4; S7 -> S5 -> S2; S7 -> S0 -> S1; S7 -> S6 -> S3

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data
   @param OemRoute    -

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
Map8SRoutes_VN1Disable (
  struct sysHost           *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8                    (*OemRoute)[4][3]
  )
{
  UINT8     RefRoute[8][4][3] = {
                {{2, 3, 0xFF}, {2, 5, 0xFF}, {1, 4,    6}, {   7, 0xFF, 0xFF}}, // S0
                {{0, 7, 0xFF}, {4, 6, 0xFF}, {3, 2,    5}, {0xFF, 0xFF, 0xFF}}, // S1
                {{0, 1, 0xFF}, {3, 6, 0xFF}, {5, 4, 0xFF}, {   0,    7, 0xFF}}, // S2
                {{1, 0, 0xFF}, {1, 4, 0xFF}, {2, 5, 0xFF}, {   6,    7, 0xFF}}, // S3
                {{1, 0, 0xFF}, {5, 7, 0xFF}, {5, 2,    3}, {   6, 0xFF, 0xFF}}, // S4
                {{7, 0, 0xFF}, {2, 3, 0xFF}, {4, 1, 0xFF}, {   7,    6, 0xFF}}, // S5
                {{7, 0, 0xFF}, {3, 2, 0xFF}, {4, 1, 0xFF}, {   4,    5, 0xFF}}, // S6
                {{5, 2, 0xFF}, {6, 3, 0xFF}, {0, 1, 0xFF}, {   5,    4, 0xFF}}  // S7
             };

  UINT8         Idx1, Idx2, Idx3, OemSoc;

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    OemSoc = DiscEngData->SocMap[Idx1];
    for (Idx2 = 0; Idx2 < 4; Idx2++) {
      for (Idx3 = 0; Idx3 < 3; Idx3++) {
        if (RefRoute[Idx1][Idx2][Idx3] != 0xFF) {
          OemRoute[OemSoc][Idx2][Idx3] = DiscEngData->SocMap[RefRoute[Idx1][Idx2][Idx3]];
        }
      }
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n"));
      for (Idx2 = 0; Idx2 < 4; Idx2++) {
          if (OemRoute[Idx1][Idx2][0] != 0xFF) {
            KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "  S%u->S%u", Idx1, OemRoute[Idx1][Idx2][0]));
            if (OemRoute[Idx1][Idx2][1] != 0xFF) {
              KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "->S%u", OemRoute[Idx1][Idx2][1]));
            }
            if (OemRoute[Idx1][Idx2][2] != 0xFF) {
              KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "->S%u", OemRoute[Idx1][Idx2][2]));
            }
            KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "; "));
          }
      }
    }
  }


  return KTI_SUCCESS;
}


/**

  Computes the KTI CPU topology found in the system

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
ComputeTopologyTree6S (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData
  )
{
  UINT8         OemRoute[MAX_SOCKET][3][3];
  UINT8         Idx1, Idx2, Idx3, KtiLink;
  TOPOLOGY_TREE_NODE  GrandParent, Parent, Child;
  KTI_LINK_DATA       ParentLep;
  BOOLEAN       AddNode;

  MemSetLocal ((VOID *) OemRoute, 0xFF, sizeof (OemRoute));
  Map6SNodes (host, DiscEngData);
  Map6SRoutes (host, DiscEngData, OemRoute);

  //
  // Construct the topology tree that uses the template routing
  //
  Child.RawNode    = 0;
  Child.Node.Valid = TRUE;
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      for (Idx2 = 0;  DiscEngData->CpuTree[Idx1][Idx2].Node.Valid == TRUE && DiscEngData->CpuTree[Idx1][Idx2].Node.Hop < 3; Idx2++) {

        Parent = DiscEngData->CpuTree[Idx1][Idx2];
        Child.Node.Hop = Parent.Node.Hop + 1; // One level down from parent
        Child.Node.ParentIndex = Idx2; // Remember who is the immediate parent of this child

        for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) { // Explore all the links of this parent

          ParentLep = DiscEngData->Cpu[Parent.Node.SocId].LinkData[KtiLink];
          if (ParentLep.Valid != TRUE || ParentLep.DualLink == TRUE || ParentLep.TwoSkt3Link == TRUE) {
            continue;
          }

          //
          // Get the Child Socket Id, Type and port that connects it to the parent form LEP
          //
          Child.Node.SocId      = ParentLep.PeerSocId;
          Child.Node.SocType    = ParentLep.PeerSocType;
          Child.Node.ParentPort = ParentLep.PeerPort;

          //
          // Make sure that this socket is not already found in the tree between hop level 0 and parent's hop level.
          // The rational is that, if we find child socket already at that level, then we have a shorter path to this child.
          //
          if (ShorterPathExists (host, DiscEngData->CpuTree[Idx1], &Child) == TRUE) {
            continue;
          }

          //
          // If it is 1 Hop away node (i.e immediate neighbor), just add the node
          //
          AddNode = FALSE;
          if (Child.Node.Hop == 1) {
            AddNode = TRUE;
          } else if (Child.Node.Hop == 2) {
            //
            // If it is 2 Hop away node, make sure the template routing indicates this route to be used
            //
            for (Idx3 = 0; Idx3 < 3; Idx3++) {
              if (OemRoute[Idx1][Idx3][0] == Parent.Node.SocId &&
                  OemRoute[Idx1][Idx3][1] == Child.Node.SocId &&
                  OemRoute[Idx1][Idx3][2] == 0xFF) {
                AddNode = TRUE;
                break;
              }
            }
          } else {
            //
            // If it is 3 Hop away node, make sure the template routing indicates this route to be used
            //
            GrandParent = DiscEngData->CpuTree[Idx1][Parent.Node.ParentIndex];
            for (Idx3 = 0; Idx3 < 3; Idx3++) {
              if (OemRoute[Idx1][Idx3][0] == GrandParent.Node.SocId &&
                  OemRoute[Idx1][Idx3][1] == Parent.Node.SocId &&
                  OemRoute[Idx1][Idx3][2] == Child.Node.SocId) {
                AddNode = TRUE;
                break;
              }
            }
          }

          if (AddNode == FALSE) {
            continue;
          }

          if (AddNodeToTopologyTree (host, DiscEngData->CpuTree[Idx1], &Child) != KTI_SUCCESS) {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n  Adding CPU%u to Topology Discovery Tree of CPU%u failed.",
                              Child.Node.SocId, Idx1));
            KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_5E);
          }
        } // for(KtiLink...)
      }
    }
  }

  return KTI_SUCCESS;

}


/**

  This routine establishes a mapping between intel's ref platform and this platform. Port number is not relevant.

  Ref Platform:

    S0 ----- S1 ----- S4
     |       |        |
     |       |        |
     |       |        |
     |       |        |
     |       |        |
    S2 ----- S3 ----- S5


  OEM Platform:

    S5 ----- S1 ----- S3
     |       |        |
     |       |        |
     |       |        |
     |       |        |
     |       |        |
    S4 ----- S2 ----- S0



  The socket mapping would be:

    Intel   OEM
      S0     S5
      S1     S1
      S2     S4
      S3     S2
      S4     S3
      S5     S0

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data

   @retval 0     - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
Map6SNodes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData
  )
{
  UINT8               Idx1, KtiLink;
  UINT8               RingNum1, RingNum2;
  UINT8               RefSoc = 0;
  UINT8               TempSoc1, TempSoc2;
  KTI_LINK_DATA       ParentLep;

  //
  // Identify the two nodes that are part of both the rings and map them to S1 & S3 of the ref topology
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->RingNodePairs[Idx1] != 0xFF) {
      RefSoc = Idx1;
      DiscEngData->SocMap[1] = RefSoc;
      DiscEngData->SocMap[3] = DiscEngData->RingNodePairs[RefSoc];
      break;
    }
  }
  KTI_ASSERT ((Idx1 < MAX_SOCKET), ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);

  // Identify RefSoc's two rings present in this platform
  RingNum1 = DiscEngData->RingInfo[RefSoc][1];
  RingNum2 = DiscEngData->RingInfo[RefSoc][2];

  // Identify the other 2 nodes of RefSoc's first ring
  TempSoc1 = 0xFF;
  TempSoc2 = 0xFF;
  for (Idx1 = 0; Idx1 < CPUS_PER_RING; Idx1++) {
    if (DiscEngData->Rings[RingNum1][Idx1] == RefSoc || DiscEngData->Rings[RingNum1][Idx1] == DiscEngData->RingNodePairs[RefSoc]) {
      continue;
    }
    if (TempSoc1 == 0xFF) {
      TempSoc1 = DiscEngData->Rings[RingNum1][Idx1];
    } else {
      TempSoc2 = DiscEngData->Rings[RingNum1][Idx1];
    }
  }
  KTI_ASSERT ((TempSoc1 != 0xFF), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_60);
  KTI_ASSERT ((TempSoc2 != 0xFF), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_61);

  //
  // Make sure TempSoc2 is the diagonally opposite in RefSoc's first ring; otherwise swap out TempSoc1 & TempSoc2
  //
  for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
    ParentLep = DiscEngData->Cpu[RefSoc].LinkData[KtiLink];
    if (ParentLep.Valid == TRUE) {
      if (ParentLep.PeerSocId == TempSoc2) {
        TempSoc2 = TempSoc1;
        TempSoc1 = (UINT8)ParentLep.PeerSocId;
        break;
      }
    }
  }

  // TempSoc1 and TempSoc2 can be now mapped to S0 and S2 of intel ref platform
  DiscEngData->SocMap[0] = TempSoc1;
  DiscEngData->SocMap[2] = TempSoc2;


  // Identify the other 2 nodes of RefSoc's second ring
  TempSoc1 = 0xFF;
  TempSoc2 = 0xFF;
  for (Idx1 = 0; Idx1 < CPUS_PER_RING; Idx1++) {
    if (DiscEngData->Rings[RingNum2][Idx1] == RefSoc || DiscEngData->Rings[RingNum2][Idx1] == DiscEngData->RingNodePairs[RefSoc]) {
      continue;
    }
    if (TempSoc1 == 0xFF) {
      TempSoc1 = DiscEngData->Rings[RingNum2][Idx1];
    } else {
      TempSoc2 = DiscEngData->Rings[RingNum2][Idx1];
    }
  }
  KTI_ASSERT ((TempSoc1 != 0xFF), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_62);
  KTI_ASSERT ((TempSoc2 != 0xFF), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_63);

  //
  // Make sure TempSoc2 is the diagonally opposite in S0's second ring; otherwise swap out TempSoc1 & TempSoc2
  //
  for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {
    ParentLep = DiscEngData->Cpu[RefSoc].LinkData[KtiLink];
    if (ParentLep.Valid == TRUE) {
      if (ParentLep.PeerSocId == TempSoc2) {
        TempSoc2 = TempSoc1;
        TempSoc1 = (UINT8)ParentLep.PeerSocId;
        break;
      }
    }
  }

  // TempSoc1 and TempSoc2 can be now mapped to S4 and S5 of intel ref platform
  DiscEngData->SocMap[4] = TempSoc1;
  DiscEngData->SocMap[5] = TempSoc2;

  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n    Socket Mapping"));
  for (Idx1 = 0; Idx1 < 6; Idx1++) {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n      %u  -  %u ", Idx1, DiscEngData->SocMap[Idx1]));
  }

  return KTI_SUCCESS;
}

STATIC
KTI_STATUS
Map6SRoutes (
  struct sysHost             *host,
  KTI_DISC_DATA            *DiscEngData,
  UINT8         (*OemRoute)[3][3]
  )
/**

  Uses the following static routing as tamplate and produces the routing for the given platform
  using the socket mapping.

  S0->S2->S3;  S0->S1->S4;  S0->S2->S3->S5
  S1->S3->S2;  S1->S3->S5
  S2->S0->S1;  S2->S3->S5;  S2->S0->S1->S4
  S3->S1->S0;  S3->S1->S4
  S4->S5->S3;  S4->S1->S0;  S4->S5->S3->S2
  S5->S4->S1;  S5->S3->S2;  S5->S4->S1->S0

   @param host        - Pointer to the system host (root) structure
   @param DiscEngData - Pointer to socket specific data
   @param OemRoute    -

   @retval 0     - Success
   @retval Other - Failure

**/
{
  UINT8         RefRoute[6][3][3] = {
                {{2, 3, 0xFF}, {1, 4, 0xFF}, {2, 3, 5}}, // S0
                {{3, 2, 0xFF}, {3, 5, 0xFF}, {0xFF, 0xFF, 0xFF}}, // S1
                {{0, 1, 0xFF}, {3, 5, 0xFF}, {0, 1, 4}}, // S2
                {{1, 0, 0xFF}, {1, 4, 0xFF}, {0xFF, 0xFF, 0xFF}}, // S3
                {{5, 3, 0xFF}, {1, 0, 0xFF}, {5, 3, 2}}, // S4
                {{4, 1, 0xFF}, {3, 2, 0xFF}, {4, 1, 0}}  // S5
             };
  UINT8         Idx1, Idx2, Idx3, OemSoc;

  for (Idx1 = 0; Idx1 < 6; Idx1++) {
    OemSoc = DiscEngData->SocMap[Idx1];
    for (Idx2 = 0; Idx2 < 3; Idx2++) {
      for (Idx3 = 0; Idx3 < 3; Idx3++) {
        if (RefRoute[Idx1][Idx2][Idx3] != 0xFF) {
          OemRoute[OemSoc][Idx2][Idx3] = DiscEngData->SocMap[RefRoute[Idx1][Idx2][Idx3]];
        }
      }
    }
  }

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if (DiscEngData->Cpu[Idx1].Valid == TRUE) {
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n"));
      for (Idx2 = 0; Idx2 < 3; Idx2++) {
        if (OemRoute[Idx1][Idx2][0] != 0xFF) {
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "  S%u -> S%u -> S%u", Idx1, OemRoute[Idx1][Idx2][0], OemRoute[Idx1][Idx2][1]));
          if (OemRoute[Idx1][Idx2][2] != 0xFF) {
            KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, " -> S%u", OemRoute[Idx1][Idx2][2]));
          }
        }
      }
    }
  }

  return KTI_SUCCESS;
}

#endif

//
// All the following routines are Si specific and needs to be ported based on Si capability
//

/**

  Calculate routing for socket

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC internal variables.

   @retval 0 - Success
   @retval Other - Failure

**/
STATIC
KTI_STATUS
CalculateRT (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  TOPOLOGY_TREE_NODE  *TopologyTree;
  TOPOLOGY_TREE_NODE  Child, Parent;
  UINT8               Idx1, Idx2;
  UINT8               KtiLink;
  KTI_LINK_DATA       ParentLep;

  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((SocketData->Cpu[Idx1].Valid == TRUE) && (SocketData->Cpu[Idx1].SocType == SOCKET_TYPE_CPU)) {

      TopologyTree = SocketData->CpuTree[Idx1];
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n Calculating Route for CPU%u ", TopologyTree[0].Node.SocId));
      KtiCheckPoint ((UINT8) (1 << TopologyTree[0].Node.SocId), (UINT8) TopologyTree[0].Node.SocType, 0, STS_TOPOLOGY_DISCOVERY, MINOR_STS_CALCULATE_ROUTE, host);

      for (Idx2 = 0; ((TopologyTree[Idx2].Node.Valid != FALSE) && (TopologyTree[Idx2].Node.SocType == SOCKET_TYPE_CPU)); Idx2++) {
        Parent            = TopologyTree[Idx2];
        Child.Node.Valid  = TRUE;
        Child.Node.Hop    = Parent.Node.Hop + 1; // One level down from parent
        if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
          for (KtiLink = 0; KtiLink < host->var.common.KtiPortCnt; KtiLink++) {

            ParentLep = SocketData->Cpu[Parent.Node.SocId].LinkData[KtiLink];

            //
            // Skip the Invalid/Duplicate Links/3 links
            //
            if (ParentLep.Valid != TRUE || ParentLep.DualLink == TRUE || ParentLep.TwoSkt3Link == TRUE ) {
              continue;
            }

            //
            // Get the Child's socket Id, Type and parent port form LEP
            //
            Child.Node.SocId      = ParentLep.PeerSocId;
            Child.Node.SocType    = ParentLep.PeerSocType;
            Child.Node.ParentPort = ParentLep.PeerPort;

            //
            // Check that this Child is found in the topology tree
            //
            if (NodeFoundInTopologyTree (host, TopologyTree, &Child) == TRUE) {
              //
              // Setup the route including snoop fanout route.
              //
              SetupNonBCRoute (host, SocketData, KtiInternalGlobal, TopologyTree, &TopologyTree[0], &Parent, &Child);
              if (KtiInternalGlobal->SnoopFanoutEn == TRUE) {
                if ((KtiInternalGlobal->Is4SRing == TRUE) && (TopologyTree[0].Node.SocId != Child.Node.SocId && Parent.Node.Hop == 1)) {
                  //
                  // Check if 4S ring and the socket is not Root socket and hop is 1 only (i.e interm socket)
                  //
                  SetupBCRoute(host, SocketData, KtiInternalGlobal, &TopologyTree[0], &Parent, &Child, TRUE);
                } else {
                  SetupBCRoute(host, SocketData, KtiInternalGlobal, &TopologyTree[0], &Parent, &Child, FALSE);
                }
              }
            }
          } // for(KtiLink...)
        } else {
          //
          // This is a fatal error
          //
          KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Unknown Socket Type found in CPU%u TopologyTree. Topology not supported.\n ", TopologyTree[0].Node.SocId));
          KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE);
        }
      }
    }
  }

  // Optmize the Cha from sending snoop to the requester in snoop fanout mode.
  if (KtiInternalGlobal->SnoopFanoutEn == TRUE) {
    OptimizeChaFanoutSnoop (host, SocketData);
  }

  RouteTableDump (host, SocketData, KtiInternalGlobal);

  return KTI_SUCCESS;
}

/**

  Setup route for node

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC internal variables.
  @param TopologyTree      - Pointer to full system KTI fabric tree
  @param Root              - Pointer to root KTI node
  @param Interm            - Pointer to intermediate KTI node
  @param Dest              - Pointer to destination KTI node

  @retval 0 - Successful
  @retval 1 - Failure

**/
STATIC
KTI_STATUS
SetupNonBCRoute (
  struct sysHost             *host,
  KTI_SOCKET_DATA            *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE         *TopologyTree,
  TOPOLOGY_TREE_NODE         *Root,
  TOPOLOGY_TREE_NODE         *Interm,
  TOPOLOGY_TREE_NODE         *Dest
  )
{
  TOPOLOGY_TREE_NODE  Parent;
  TOPOLOGY_TREE_NODE  Child;
  TOPOLOGY_TREE_NODE  GrandParent;
  UINT8               cLocalPort1;
  UINT8               cLocalPort2;
  UINT8               cLocalPort3;
  UINT8               pLocalPort1;
  UINT8               RootVn;
  KTI_CPU_SOCKET_DATA     *Cpu;

  Cpu = SocketData->Cpu;
  RootVn = Cpu[Root->Node.SocId].Vn;
  //
  // Set the Forward Path from "Root" to "Dest". Backward path from "Dest" to "Root" will be
  // set when this routineis called with "Dest" as "Root" and "Root" as "Dest".
  //
  //
  // 1. Recursively set the Forward Path in:
  //     - Socket "Interm"
  //     - Sockets that exist between "Interm" & "Root"
  //     - Socket "Root"
  // (No need to set the Forward path in Socket "Dest" itself since the Local Routing rules in "Dest" will take care of it)
  //
  Parent  = *Interm;
  Child   = *Dest;
  while (1) {
    if (Parent.Node.SocId == Root->Node.SocId && Parent.Node.SocType == Root->Node.SocType) {
      if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
        //
        // In Root CPU socket, setup the route in Cha & M2PCIe.
        //
        if (DuplicateOrTripleLinkExists (host, SocketData->Cpu, &Parent, &Child)) {
          GetDualLinkOrTripleLinkFromLEP (host, SocketData->Cpu, &Parent, &Child, &cLocalPort1, &cLocalPort2, &cLocalPort3);
          if (cLocalPort1 == 0xFF || cLocalPort2 == 0xFF) {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Invalid Dual-Link between CPU%u and CPU%u. ", Parent.Node.SocId, Child.Node.SocId));
            KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
          }
          SetRootNodeRoute (host, SocketData, KtiInternalGlobal, (UINT8) Parent.Node.SocId, (UINT8) Dest->Node.SocId, cLocalPort1, cLocalPort2, cLocalPort3, TRUE, RootVn);
        } else {
          cLocalPort1 = GetChildLocalPortFromLEP (host, SocketData->Cpu, &Parent, &Child);
          if (cLocalPort1 == 0xFF) {
            KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Invalid Link between CPU%u and CPU%u. ", Parent.Node.SocId, Child.Node.SocId));
            KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
          }
          SetRootNodeRoute (host, SocketData, KtiInternalGlobal, (UINT8) Parent.Node.SocId, (UINT8) Dest->Node.SocId, cLocalPort1, cLocalPort1, cLocalPort1, FALSE, RootVn);
        }
      } else {
        KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Unknown Socket Type found in CPU%u TopologyTree. \n ", Root->Node.SocId));
        KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE);
      }
      break;
    } else if (Parent.Node.SocType == SOCKET_TYPE_CPU) {
      //
      // Setup the path in intermediate CPU socket found in between root & dest
      //
      GetParentSocket (host, TopologyTree, &Parent, &GrandParent);
      pLocalPort1 = (UINT8) Parent.Node.ParentPort;
      cLocalPort1 = GetChildLocalPortFromLEP (host, SocketData->Cpu, &Parent, &Child);
      if (cLocalPort1 == 0xFF) {
        KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Invalid Port connection between CPU%u and CPU%u. ", Parent.Node.SocId, Child.Node.SocId));
        KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
      }
      SetPassThruRoute (host, SocketData, (UINT8) Parent.Node.SocId, (UINT8) Dest->Node.SocId, pLocalPort1, cLocalPort1, RootVn);
    } else {
      KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n Unknown Intermediate Socket Type found in CPU%u Topology Tree. ", Root->Node.SocId));
      KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_SOCKET_TYPE);
    }

    //
    // Go up one step towards the root & repeat the loop
    //
    Child = Parent;
    GetParentSocket (host, TopologyTree, &Child, &Parent);
  }

  return KTI_SUCCESS;
}

/**

    Setup broadcast routing.

    @param host              - pointer to the system host root structure
    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Root              - Pointer to root KTI node
    @param Interm            - Pointer to intermediate KTI node
    @param Dest              - Pointer to destination KTI node
    @param PassThruFanout    - Pass-through fanout on/off

    @retval KTI_SUCCESS - on successful completion

**/
STATIC
KTI_STATUS
SetupBCRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE       *Root,
  TOPOLOGY_TREE_NODE       *Interm,
  TOPOLOGY_TREE_NODE       *Dest,
  BOOLEAN                  PassThruFanout
  )
{
  UINT8                      cLocalPort1, cLocalPort2, cLocalPort3, pLocalPort1, pLocalPort2;

  cLocalPort1 = cLocalPort2 = pLocalPort1 = pLocalPort2 = 0xFF;

  //
  //  Set the broadcast path to "Dest" in "Interm" socket
  //
  if (Interm->Node.SocId == Root->Node.SocId && Interm->Node.SocType == Root->Node.SocType) {
    if (DuplicateOrTripleLinkExists(host, SocketData->Cpu, Interm, Dest)) {
      //
      // For dual-link or Triple links configs, we select the first link.
      //
      GetDualLinkOrTripleLinkFromLEP(host, SocketData->Cpu, Interm, Dest, &cLocalPort1, &cLocalPort2, &cLocalPort3);
    } else {
      cLocalPort1 = GetChildLocalPortFromLEP(host, SocketData->Cpu, Interm, Dest);
      pLocalPort1 = (UINT8)Dest->Node.ParentPort;
      if (cLocalPort1 == 0xFF || pLocalPort1 == 0xFF) {
        KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Invalid Port connection between CPU%u and CPU%u. ", Interm->Node.SocId, Dest->Node.SocId));
        KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
      }
    }
    SetRootNodeFanoutRoute (SocketData, KtiInternalGlobal, Interm, (UINT8)Root->Node.SocId, cLocalPort1);
  } else if (Interm->Node.SocType == SOCKET_TYPE_CPU) {
    if((KtiInternalGlobal->Is4SRing != TRUE) || ((KtiInternalGlobal->Is4SRing == TRUE) && (PassThruFanout == TRUE))) {  // check if (not 4S ring) or (4S ring with passthru)

      //
      // Setup the path in intermediate CPU socket found in between root & dest
      //
      cLocalPort1 = GetChildLocalPortFromLEP(host, SocketData->Cpu, Interm, Dest);
      pLocalPort1 = (UINT8)Interm->Node.ParentPort;
      if (cLocalPort1 == 0xFF || pLocalPort1 == 0xFF) {
        KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Invalid Port connection between CPU %u and CPU %u.", Interm->Node.SocId, Dest->Node.SocId));
        KTI_ASSERT(FALSE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
      }
      SetPassThruFanoutRoute (host, SocketData, Interm, (UINT8)Root->Node.SocId, pLocalPort1, cLocalPort1);
    }
  } else {
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Unknown Intermediate Socket Type found in CPU %u Topology Tree.", Root->Node.SocId));
    KtiDebugPrintFatal((host, KTI_DEBUG_ERROR, "\n Topology not supported\n"));
    KTI_ASSERT(TRUE, ERR_UNSUPPORTED_TOPOLOGY, MINOR_ERR_INVALID_TOPOLOGY);
  }

  return KTI_SUCCESS;
}

/**

  Set Root Node Route and its VN

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC internal variables.
  @param SocId             - Root socket ID
  @param DestSocId         - endpoint processor socket ID
  @param OutPort1          - First KTI port of node
  @param OutPort2          - Second KTI port of node; set to same port as OutPort1 for non dual-link topologies
  @param DualLink          - TRUE - Dual link exists
                             FALSE - No Dual link
  @param RootVn            - Root socket's VN

  @retval 0 - Successful
  @retval 1 - Failure

**/
STATIC
KTI_STATUS
SetRootNodeRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                   SocId,
  UINT8                   DestSocId,
  UINT8                   OutPort1,
  UINT8                   OutPort2,
  UINT8                   OutPort3,
  BOOLEAN                 DualLinkOrTripleLink,
  UINT8                   RootVn
  )
{
  UINT8    PortFieldWidth;
  UINT8    ChaPortFieldWidth;
  M3KTI_VN_SEL *M3KtiVnSel;

  if ((OutPort1 >= host->var.common.KtiPortCnt) || (OutPort2 >= host->var.common.KtiPortCnt)) {
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "Invalid value of KTI port of node"));
    return KTI_FAILURE;
  }

  //
  //  Set the RT in Cha & M2PCIe. M3KTI & KTIAgent RT needs to set for pass-thru route only.
  //  For all non-dual link topologies, the route setting is straight forward. Cha and M2PCIe
  //  route tables are configured to use the only link that was calculated in discovery process.
  //
  //  If dual link exists between root node and immediate neighbor, need to balance load on them.
  //  Dual link can exist in 2S config only. The load balancing needs to be done for CHA/M2MEM only.
  //  It is not needed for M2PCIe since it is set to operate in a mode where it sends
  //  all the responses back on the same port on which the request was received which will balance
  //  the load on the links (R2PGNCTRL.qpi_rt_sel_ndr_drs=0). However M2PCIe requests always use
  //  route table and they can select only one link based on route table so the M2PCIe requests are not
  //  balanced out. (Only Ubox initiated requests; IIO initiated requests such as DMA, P2P are serviced
  //  by Cha based on address hashing. Cha will use balanced routing).
  //
  //  Cha is configured to balance the load on the links based on P6 ^ P12.  It is no longer based on Cha ID.
  //  CBO_COH_CONFIG.enablexorbasedktilink. When this field is set, the Cha selects the link as follows:
  //
  //       Link = (PA6 ^ PA12)
  //
  //  For configs with port0<->port1, port1<->port0 mapping, socket 0 Chas are configured as above, but socket 1
  //  Chas will invert the port selection by setting this bit:
  //  CBO_COH_CONFIG.swapktilinkintlv = 1
  //
  //  This interleaving config described above will ensure that the responses always come back on
  //  the same port for Cha initiated requests/snoops. This esnures all flavours of 2S 2KTI configs
  //  benifit from D2C.
  //
  //  If TwoSkt3Link case, SAD_TARGET_CHABC_SAD1_REG.enablemodulo3basedktilink need to be set to
  //  Use Mod3(PA) value to decide the KTI link to use.
  //
  //  Note: We calculate M2PCIe & Cha route as well for simplicity to match other configs.
  //

  PortFieldWidth = 2; // Two bit field to encode link 0, 1 & 2
  ChaPortFieldWidth = 2; // Four bit field to encode link 0, 1 & 2

  // M2PCIe Route
  SocketData->Route[SocId].M2PcieKtiRt |= (OutPort1 << (PortFieldWidth * DestSocId));

  //
  // Calculate the VN setting for the root node. We use a scheme where the VN selection is done only
  // at the root (i.e local) node. At intermediate nodes the VN is not switched; messages are passed on the same
  // VN on which they arrived. The VN selection for the local traffic is obtained thru VN Lookup table (LUT)
  // stored in M3KIGRVNSEL register. Each entry in the table is 1 bit wide that selects VN0 (0) or VN1 (1).
  // The table is 8-entry deep whose index is calculated as follows:
  //
  //        {DNID[2:0]}
  //
  // Entries are used to select the VN for the traffic that originates in this (root) socket.
  // Since a root socket uses same VN for all DNIDs that it talks to, all 8 entries
  // will have same setting. For the same reason, all the ports will have same setting as well.
  // Note that even though a port may not be used to talk to all other sockets or not all the sockets are
  // populated, we calculate the VN setting assuming a port is used to talk to all possible sockets to
  // avoid variations in setting. Also since the h/w does the route lookup separate from VN lookup
  // and route table is set only for the valid sockets, this is ok.
  //
  if (PORT_TO_M3KTI(OutPort1) >= MAX_M3KTI) {
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\nERR_SW_CHECK: 0x%X!!!!  Invalid value. Outport1: 0x%X is greater or equal to MAX_M3KTI: 0x%X",
                         MINOR_ERR_SW_CHECK_50, OutPort1, MAX_M3KTI));
    KTI_ASSERT (FALSE, ERR_SW_CHECK, MINOR_ERR_SW_CHECK_50);
  }

  M3KtiVnSel =  &(SocketData->Route[SocId].M3KtiVnSel[PORT_TO_M3KTI(OutPort1)]);
  (RootVn == VN0) ? (M3KtiVnSel->IgrVnSel = 0x00000000) : (M3KtiVnSel->IgrVnSel = 0x000000FF);

  // Cha Route
  if (DualLinkOrTripleLink) {
    if (host->var.kti.SysConfig == SYS_CONFIG_2S3L ) {    // check if is 2S3Link
      //
      //In the 2S-3link mode, the module three will be used as the *index* into the CHA’s KTI route table.
      //So you can specify which link is connected to which on each socket by programming one socket to map 0->link0, 1->link1, and 2->link2
      //then program the other socket to map 0,1,2 to whichever links link0/link1/link2 are connected to.
      //
      if (SocId != 0){
         OutPort1 = (UINT8) SocketData->Cpu[0].LinkData[0].PeerPort;       // get the socket 0 link 0 peer port
         OutPort2 = (UINT8) SocketData->Cpu[0].LinkData[1].PeerPort;       // get the socket 0 link 1 peer port
         OutPort3 = (UINT8) SocketData->Cpu[0].LinkData[2].PeerPort;       // get the socket 0 link 2 peer port
      }
      SocketData->Route[SocId].ChaKtiRt = (OutPort1 | (OutPort2 << ChaPortFieldWidth) | (OutPort3 << (ChaPortFieldWidth * 2)));
    } else {
      //
      // In case of 2SEP, dual link, the indexing into CHA and M2Mem routing tables is done by even/odd address
      // This indexing mode is set off by CBO_COH_CONFIG.enablexorbasedktilink = 1 for CHAs and kti2slinkintlvena = 1 for m2MEm
      // [3:2] = 0x1 - route even-addressed entries to port 1
      // [1:0] = 0x0 - route odd-addressed entries to port 0
      //
      SocketData->Route[SocId].ChaKtiRt = 0x4;
    }
  } else {
    SocketData->Route[SocId].ChaKtiRt |= (OutPort1 << (ChaPortFieldWidth * DestSocId));
  }

  return KTI_SUCCESS;
}


/**

  Sets up route-through route on a socket

  @param host       - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param SocId      - Pass-thru socket ID
  @param DestSocId  - endpoint processor socket ID
  @param InPort     - pass-thru port towards the root
  @param OutPort    - pass-thru port towards the destination
  @param RootVn     - Root socket's VN

  @retval 0 - Successful
  @retval 1 - Failure

**/
STATIC
KTI_STATUS
SetPassThruRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA         *SocketData,
  UINT8                   SocId,
  UINT8                   DestSocId,
  UINT8                   InPort,
  UINT8                   OutPort,
  UINT8                   RootVn
  )
{
  UINT8  M3KtiNum;
  UINT32 EncodedRoute = 0;

  // set route table in M3KTI
  M3KtiNum = PORT_TO_M3KTI(InPort);
  EncodedRoute = SocketData->Route[SocId].M3KtiRt[M3KtiNum];
  ConvertM3KTIRoutingTable(InPort, OutPort, DestSocId, &EncodedRoute);
  SocketData->Route[SocId].M3KtiRt[M3KtiNum] = EncodedRoute;

  // set route table in KTI Agent
  EncodedRoute = SocketData->Route[SocId].KtiAgentRt[InPort];
  EncodedRoute &= (~(3 << (DestSocId * 2)));
  EncodedRoute |= (OutPort << (DestSocId * 2));
  SocketData->Route[SocId].KtiAgentRt[InPort] = EncodedRoute;

  return KTI_SUCCESS;
}

/**

    Setup snoop fanout routing for root node

    @param SocketData        - pointer to the socket data structure
    @param KtiInternalGlobal - pointer to the KTI RC internal global structure
    @param Parent            - pointer to KTI parent node
    @param RootBcRti         -
    @param OutPort           - port from which snoop to be sent

    @retval KTI_SUCCESS - on successful completion

**/
STATIC
KTI_STATUS
SetRootNodeFanoutRoute (
  KTI_SOCKET_DATA          *SocketData,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  TOPOLOGY_TREE_NODE       *Parent,
  UINT8                    RootBcRti,
  UINT8                    OutPort
  )
{
  UINT8   Idx1;
  UINT32  EncodingBit = 0;

  //
  // Request coming from all populated sockets including the local socket should trigger a snoop to OutPort
  //
  for (Idx1 = 0; Idx1 < MAX_SOCKET; Idx1++) {
    if ((KtiInternalGlobal->CpuList & (1 << Idx1))) {
      EncodingBit |= ((1 << OutPort) << (4 * Idx1)) ; // program send2ktiport for this RHNID in CHA
    }
  }
  SocketData->Route[Parent->Node.SocId].ChaKtiFanOut |= EncodingBit;

  return KTI_SUCCESS;
}


/**

    Setup snoop fanout routing for a pass-through node.

    @param host       - pointer to the system host root structure
    @param SocketData - pointer to the socket data structure
    @param Parent     - pointer to KTI parent node
    @param RootBcRti  -
    @param InPort     - port receiving snoop
    @param OutPort    - port to send out snoop

    @retval KTI_SUCCESS - on successful completion

**/
STATIC
KTI_STATUS
SetPassThruFanoutRoute (
  struct sysHost           *host,
  KTI_SOCKET_DATA          *SocketData,
  TOPOLOGY_TREE_NODE       *Parent,
  UINT8                    RootBcRti,
  UINT8                    InPort,
  UINT8                    OutPort
  )
{
  UINT8   M3KtiNum;

  M3KtiNum = PORT_TO_M3KTI(InPort);            // determine M3KTI agent based on InPort
  SocketData->Route[Parent->Node.SocId].M3KtiSnpFanOut[M3KtiNum][InPort & 0x1] |= ((1 << OutPort) << (3 * RootBcRti));

  return KTI_SUCCESS;
}

/**

  When the system is in Snoop Fanout mode, the Cha is required to send send one snoop per link. But in some topologies
  it is possible the original requester is the only socket present on any of its link. In such cases, the Cha can be optimized
  to no send the snoop over the link to reduce the uncore ring traffic.

  @param host       - Pointer to the system host (root) structure
  @param SocketData - socket specific data structure

  @retval None

**/
STATIC
VOID
OptimizeChaFanoutSnoop (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData
  )
{
  UINT8               Index1, Index2, PeerSocId, PeerPort, M3KtiNum;
  UINT32              Data32;

  //
  // For each valid sockets, find out if the pass-through traffic is needed on the peer port of peer socket,
  // If there is no pass-through fanout traffic, then Cha can be optimized to no send the snoop over the link to reduce the uncore ring traffic for RHNID = peer socket.
  //
  //
  for (Index1 = 0; Index1 < MAX_SOCKET; Index1++) {
    if ((SocketData->Cpu[Index1].Valid == TRUE) && (SocketData->Cpu[Index1].SocType == SOCKET_TYPE_CPU)) {

      //
      // find out if the pass-through traffic is needed on the peer port of peer socket,
      //
      //
      for (Index2 = 0; Index2 < host->var.common.KtiPortCnt; Index2++) {
        if (SocketData->Cpu[Index1].LinkData[Index2].Valid == TRUE ) {

          //
          //Read M3KTI Fanout value from Peer Port of Peer Socket
          //If is 0, there is no pass-through fanout needed on Peer Port of Peer Socket
          //
          PeerSocId = (UINT8) SocketData->Cpu[Index1].LinkData[Index2].PeerSocId;
          PeerPort =  (UINT8) SocketData->Cpu[Index1].LinkData[Index2].PeerPort;
          M3KtiNum = PORT_TO_M3KTI(PeerPort);
          Data32 = SocketData->Route[PeerSocId].M3KtiSnpFanOut[M3KtiNum][PeerPort & 0x01];

          if (Data32 == 0){
          //
          // Optimize the CHA to not send snoop to the link that peer socket id is equal to RHNID
          //
          Data32 = SocketData->Route[Index1].ChaKtiFanOut;
          Data32 = Data32 & (~((1 << Index2) << (4 * PeerSocId)));
          SocketData->Route[Index1].ChaKtiFanOut = Data32;
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n CPU%1u CHA Snoop fanout optimized for Link%1u target CPU%1u",
                                 Index1, Index2, PeerSocId));
          }
        }
      }
    }
  }

  return;
}


