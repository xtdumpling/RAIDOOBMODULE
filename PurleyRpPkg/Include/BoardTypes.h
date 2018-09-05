//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file BoardTypes.h

  Platform specific information

**/

#ifndef TypePlatformDefault
//
// Platform types - if not already defined
//
typedef enum {
  TypeNeonCityEPRP = 0x00,
  TypeNeonCityEPECB,
  TypeOpalCitySTHI,
  TypePurleyLBGEPDVP,
  TypeWolfPass,
  TypeBuchananPass,
  TypeCrescentCity,
  TypeHedtEV,
  TypeHedtCRB,
  TypeLightningRidgeEXRP,
  TypeLightningRidgeEXECB1, 
  TypeLightningRidgeEXECB2,
  TypeLightningRidgeEXECB3,
  TypeLightningRidgeEXECB4, 
  TypeLightningRidgeEX8S1N,
  TypeLightningRidgeEX8S2N,
  TypeBarkPeak,
  TypeKyanite,
  TypeSawtoothPass,  // We need to keep the value of TypeSawtoothPass unchanged
  TypeNeonCityFPGA,
  TypeOpalCityFPGA
} EFI_RP_TYPE;


#define TypePlatformUnknown       0xFF
#define TypePlatformMin           TypeNeonCityEPRP    
#define TypePlatformMax           TypeOpalCityFPGA
#define TypePlatformDefault       TypeNeonCityEPRP
#define TypePlatformOpalCityPPV   6
#define TypePlatformOpalCityCPV   7 

#endif

