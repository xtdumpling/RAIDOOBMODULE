/** @file
  AMT Policy for AMT PEIMs

@copyright
 Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _AMT_POLICY_PPI_H_
#define _AMT_POLICY_PPI_H_

//
// Include other definitions
//
///
/// Intel AMT Policy PPI GUID
/// This PPI provides an interface to get the current Intel AMT SKU information
///
//
// Extern the GUID for PPI users.
//
extern EFI_GUID gAmtPolicyPpiGuid;

/**
  ME AMT Policy PPI \n
  All ME AMT Policy ppi change history listed here \n

  -<b> Revision 1</b>:
   - Initial version.\n
  -<b> Revision 2</b>
  - Added AmtSolEnabled \n

**/
#define AMT_POLICY_PPI_REVISION  2

#pragma pack (push,1)
///
/// Intel AMT Policy PPI
/// The Intel AMT Policy PPI returns the Intel ME feature set in PEI phase
///
typedef struct {
  //
  //  DWORD 0,
  //
  UINT8   Revision;               ///< Policy structure revision number
  UINT8   AmtEnabled : 1;         ///< <b> 0- Disable <b>, 1- Enable Intel AMT features
  /**
  ME WatchDog timer feature
  <b> 0: Disabled <b>
  1: Enabled ME WDT if corresponding timer value is not zero.
  See WatchDogTimerBios.
  **/
  UINT8   WatchDog : 1;
  UINT8   AsfEnabled : 1;         ///< 0- Disable , <b> 1- Enable <b> Asf features enable/disable
  UINT8   ManageabilityMode : 1;  ///< Manageability Mode set by Mebx
  UINT8   FWProgress: 1;          ///< 0- Disable , <b> 1- Enable <b> Progress Event option
  UINT8   AmtSolEnabled :1;       ///< Serial Over Lan enable/disable state by Mebx  
  UINT8   WatchDogOs    : 1;
  UINT8   RsvdBits0     : 1;      ///< Reserved bits, align to multiple 4.
  UINT8   Rsvd0[2];               ///< Reserved bytes, align to multiple 4.
  //
  // DWORD 1,
  //
  UINT16  WatchDogTimerOs;        ///< Not available in PEI phase.
  /**
  <b> 0 - Disable <b> BIOS WDT won't be started even if WatchDog is enabled.
  Non zero value - The BIOS WDT is set according to the value and started if WatchDog is enabled.
  **/
  UINT16  WatchDogTimerBios;
} AMT_POLICY_PPI;
#pragma pack (pop)

#endif
