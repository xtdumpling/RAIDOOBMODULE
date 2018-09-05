/*++

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IioDevHide.h

Abstract:

  Houses all function definition of IioDevFuncHideTable 

Revision History:

--*/
#ifndef IIO_DEVHIDE_H_
#define IIO_DEVHIDE_H_

#if MAX_SOCKET > 4
extern IIO_DEVFUNHIDE_TABLE IioDevFuncHideTable[8];
#else
extern IIO_DEVFUNHIDE_TABLE IioDevFuncHideTable[4];
#endif

#endif