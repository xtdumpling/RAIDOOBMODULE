/** @file*

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef PCH_HSIO_PTSSTABLES_H_
#define PCH_HSIO_PTSSTABLES_H_

#include <Platform.h>
#include <BoardTypes.h>
#include <Ppi/PchHsioPtssTable.h>
#include <PchAccess.h>

///
/// SATA PTSS Topology Types
///
typedef enum {
  PchSataTopoUnknown = 0x00,
  PchSataTopoIsata,
  PchSataTopoDirectConnect,
  PchSataTopoFlex,
  PchSataTopoM2
} PCH_SATA_TOPOLOGY;

///
/// PCIe PTSS Topology Types
///
typedef enum {
  PchPcieTopoUnknown = 0x00,
  PchPcieTopox1,
  PchPcieTopox4,
  PchPcieTopoSataE,
  PchPcieTopoM2  
} PCH_PCIE_TOPOLOGY;

///
/// DMI PTSS Topology Types
///
typedef enum {
  PchDmiTopoUnknown = 0x00,
} PCH_DMI_TOPOLOGY;

typedef struct {
  PCH_SBI_PTSS_HSIO_TABLE   PtssTable;
  UINT16                    Topology;
  UINT16                    BoardId;
} HSIO_PTSS_TABLES;

#endif // PCH_HSIO_PTSSTABLES_H_
