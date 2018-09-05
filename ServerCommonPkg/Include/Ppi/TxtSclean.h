//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  TxtSclean.h

Abstract:

--*/

#ifndef _PEI_TXT_SCLEAN_PPI_H_
#define _PEI_TXT_SCLEAN_PPI_H_

typedef struct _PEI_TXT_SCLEAN_PPI PEI_TXT_SCLEAN_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_TXT_SCLEAN_RUN) (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN PEI_TXT_SCLEAN_PPI             * This
  );

typedef struct _PEI_TXT_SCLEAN_PPI {
  PEI_TXT_SCLEAN_RUN  RunSclean;
} PEI_TXT_SCLEAN_PPI;

extern EFI_GUID gPeiTxtScleanPpiGuid;

#endif
