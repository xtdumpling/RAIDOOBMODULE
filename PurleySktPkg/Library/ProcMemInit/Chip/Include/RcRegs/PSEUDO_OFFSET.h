//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      Reference Code                                                    *
// *                                                                        *
// *      Copyright (c) 2007-2016, Intel Corporation.                       *
// *                                                                        *
// *      This software and associated documentation (if any) is furnished  *
// *      under a license and may only be used or copied in accordance      *
// *      with the terms of the license. Except as permitted by such        *
// *      license, no part of this software or documentation may be         *
// *      reproduced, stored in a retrieval system, or transmitted in any   *
// *      form or by any means without the express written consent of       *
// *      Intel Corporation.                                                *
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
// *      This file contains Silicon register definitions.                  *
// *                                                                        *
// **************************************************************************
//
/* Date Stamp: 9/13/2016 */

#ifndef PSEUDO_OFFSET_h
#define PSEUDO_OFFSET_h


/* Columns of each table:                                                    */
/*        column 0: SKX_A0                                                   */
/*        column 1: SKX                                                      */



UINT16 MC_2LM_Offset[][2] = {
  {(0x07012A7C & 0x7FFF), (0x07014A7C & 0x7FFF)}
};

UINT16 KTI_LLPMON_Offset[][2] = {
  {(0x09004100 & 0x7FFF), (0x09004108 & 0x7FFF)},
  {(0x09004104 & 0x7FFF), (0x0900410C & 0x7FFF)},
  {(0x09004108 & 0x7FFF), (0x09004110 & 0x7FFF)},
  {(0x0900410C & 0x7FFF), (0x09004114 & 0x7FFF)},
  {(0x09004110 & 0x7FFF), (0x09004118 & 0x7FFF)},
  {(0x09004114 & 0x7FFF), (0x0900411C & 0x7FFF)}
};

#endif /* PSEUDO_OFFSET_h */
