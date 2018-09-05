//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c) 2007 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file LaunchBiosAcm.h

  This file contains definitions required to use the LT BIOS
  Authenticated Code Module Library.

**/

#ifndef _LT_BIOS_ACM_LIB_H_
#define _LT_BIOS_ACM_LIB_H_

//
// LtSetupAndLaunchBiosAcm is platform specific code that can be used
// to setup and launch any of the LT BIOS Authenticated Code Module
// functions.
//
extern
VOID
LaunchBiosAcm (
  IN      UINTN                  AcmBase,
  IN      UINTN                  EsiValue
  );

//
// AsmGetSecParamters will return the parameter
//
VOID
AsmGetSecParameters (
  IN UINT32       Index,
  OUT UINT32      *EaxValue OPTIONAL,
  OUT UINT32      *EbxValue OPTIONAL,
  OUT UINT32      *EcxValue OPTIONAL
  );

//
// The following are values that are placed in the esi register when
// calling the BIOS ACM.  These constants are used as parameters to the
// LtSetupandLaunchBiosAcm function.
//
#define     LT_LAUNCH_SCLEAN        0x00
#define     LT_RESET_EST_BIT        0x01
#define     LT_CLEAR_TPM_AUX_INDEX  0x02
#define     LT_LAUNCH_SCHECK        0x04
#define     LT_CLEAR_SECRETS        0x05            // Used only in LTSX  (envoked by system bios when secrets are found in memory)
#define     LT_LOCK_CONFIG          0x06            // Used only in LTSX  (envoked by system BIOS after memory and LT configuration is done)
#define     LT_MEASURE_STARTUP_BIOS 0xFF            // Used only in LTSX  (envoked by microcode before reset vector)

#endif
