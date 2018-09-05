/** @file
  Error structure for RAS reporting

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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
#ifndef _PCH_INT_ERRORS_H_
#define _PCH_INT_ERRORS_H_

//
// PSF Internall Errors cumulated status bits
// This is according to HSD request 4929460
//
typedef union {
  struct {
    UINT32 Psf1_CPLError :1; //  Error Handler has received ad dropped an erroneous CPL transaction for PSFx channel/port
    UINT32 Psf1_NPError  :1; //  Error Handler has received an erroneous NP transaction for PSFx channel/port
    UINT32 Psf1_PError   :1; //  Error Handler has received and dropped an erroneous P transaction for PSFx channel/port
    UINT32 Psf2_CPLError :1; 
    UINT32 Psf2_NPError  :1;
    UINT32 Psf2_PError   :1;
    UINT32 Psf3_CPLError :1;
    UINT32 Psf3_NPError  :1;
    UINT32 Psf3_PError   :1;
    UINT32 Psf4_CPLError :1;
    UINT32 Psf4_NPError  :1;
    UINT32 Psf4_PError   :1;
    UINT32 Psf5_CPLError :1;
    UINT32 Psf5_NPError  :1;
    UINT32 Psf5_PError   :1;
    UINT32 Psf6_CPLError :1;
    UINT32 Psf6_NPError  :1;
    UINT32 Psf6_PError   :1;
    UINT32 Psf7_CPLError :1;
    UINT32 Psf7_NPError  :1;
    UINT32 Psf7_PError   :1;
    UINT32 Psf8_CPLError :1;
    UINT32 Psf8_NPError  :1;
    UINT32 Psf8_PError   :1;
    UINT32 Psf9_CPLError :1;
    UINT32 Psf9_NPError  :1;
    UINT32 Psf9_PError   :1;
    UINT32 Psf10_CPLError:1;
    UINT32 Psf10_NPError :1;
    UINT32 Psf10_PError  :1;  
    UINT32 Rsvd          :2;      
  } Bits;
  UINT32    Data32;
} PCH_FABRIC_ERRORS;

#endif // _PCH_INT_ERRORS_H_

