/**

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file CpuS3MsrLib.h

**/

#ifndef __CPU_S3_MSR_LIB__
#define __CPU_S3_MSR_LIB__


/**

  Restores required MSR settings on S3 Resume.  This is used for MSRs
  that must be set late in S3 resume such as lock bits or MSRs that cannot
  be handled by WriteRegisterTable restore method.

  @param PeiServices   - Pointer to PEI Services Table.

  @retval EFI_SUCCESS in all cases.

**/
EFI_STATUS
EFIAPI
S3RestoreCpuMsrs (
  IN      EFI_PEI_SERVICES          **PeiServices
  );

/**

  Performs any PEI CPU Initialization needed in PEI phase that
  requires multi-threaded execution. This is executed on normal
  boots and S3 resume.

  @param PeiServices   - Pointer to PEI Services Table.

  @retval EFI_SUCCESS in all cases.

**/
EFI_STATUS
EFIAPI
LatePeiCpuInit (
  IN      EFI_PEI_SERVICES          **PeiServices
  );

typedef union _MSR_REGISTER {
  UINT64  Qword;

  struct _DWORDS {
    UINT32  Low;
    UINT32  High;
  } Dwords;

  struct _BYTES {
    UINT8 FirstByte;
    UINT8 SecondByte;
    UINT8 ThirdByte;
    UINT8 FouthByte;
    UINT8 FifthByte;
    UINT8 SixthByte;
    UINT8 SeventhByte;
    UINT8 EighthByte;
  } Bytes;

} MSR_REGISTER;

#endif