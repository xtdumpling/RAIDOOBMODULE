/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IIoErrorHandler.h

Abstract:
  This file contains all definitions related to IIO Error Handler

------------------------------------------------------------------*/
#include <Protocol/PlatformRasPolicyProtocol.h>
	
#ifndef _IIO_ERROR_HANDLER_H
#define _IIO_ERROR_HANDLER_H

#define R_MIERRSV                                 0x90                          // Misc Error Severity Register
#define R_PCIERRSV                                0x94                          // PCI-E Error Severity Map Register
#define   V_PCIERRSV0                               00                          // Error Severity 0
#define   V_PCIERRSV1                               01                          // Error Severity 1
#define   V_PCIERRSV2                               02                          // Error Severity 2
#define R_SYSMAP                                  0x9C                          // System Error Event Map Register
#define   V_SYSMAP_NoInband                         000                         // No inband message
#define   V_SYSMAP_SMI_PMI                          001                         // Generate SMI/PMI
#define   V_SYSMAP_NMI                              002                         // Generate NMI
#define   V_SYSMAP_CPEI                             003                         // Generate CPEI
#define   V_SYSMAP_MCA                              004                         // Generate MCA
#define R_ERRPINCTL                               0xA4
#define   V_ERRPINCTL_DIS                           00
#define   V_ERRPINCTL_DATA                          01
#define   V_ERRPINCTL_SEV                           02
#define   V_ERRPINCTL_RSVD                          03

//DMI settings
#define   R_DMI_DEVICE_CONTROL_OFFSET               0x60
#define   R_DMI_DEVICE_STATUS_OFFSET                0x62
#define   V_DMI_AER_CAPID                           0xB
#define   V_DMI_AER_VSEC_CAPID                      0x4

#define R_RPPIOERRHF							0x08
#define R_RPPIOERRSTS							0x0C
#define R_RPPIOERRMSK							0x10

#define ONLY_REGISTER_OFFSET(x)  (x & 0xFFF)

#define PCIE_EXT_CAP_HEADER_RPPIOERR_CAPID          0x000b
#define PCIE_EXT_CAP_HEADER_RPPIOERR_VSECID         0x0007

#define IIO_CORRECTABLE 0x0
#define IIO_RECOVERABLE 0x1
#define IIO_FATAL       0x2

#define   V_SCI     01
#define   MAX_NUM_ERR (MAX_SOCKET * NUMBER_PORTS_PER_SOCKET)

// Bitmap mask
#define IIO_CORE_ERROR_MSK          0x00003ff1      //IIOERRCTL
#define IIO_CORE_ERROR_SEVMSK       0x00003f03      //IIOERRSV
#define IIO_COH_INFACE_MSK          0x0000647e      //IRPP0ERRCTL     IRPP1ERRCTL  for BDX and HSX
#define IIO_COH_INFACE_IVTMSK       0x0000646e      //IRPP0ERRCTL     IRPP1ERRCTL  for IVT
#define IIO_COH_SEV_MSK             0x0c3003fc      // IRPPERRSV
#define IIO_MISC_ERROR_MSK          0x0000001f      // MIERRCTL
#define IIO_MISC_ERROR_SEVMSK       0x000003ff      // MIERRSV
#define IIO_VTD_MSK                 0x800001ff      // VTUNCERRMSK   VTUNCERRSEV    
#define IIO_ITC_MSK                 0x0fffffff      // ITCERRSEV
#define IIO_OTC_MSK                 0x0fffffff      // OTCERRSEV
#define IIO_DMA_MSK                 0x00073fff      // CHANERRMSK    CHANERRSEV   
#define IIO_PCIEADD_UNCORR_MSK      0x0000037a      // XPUNCEDMASK   XPUNCERRSEV   XPUNCERRMSK
#define IIO_PCIEAER_CORRERR_MSK     0x000031c1      // COREDMASK     CORERRMSK
#define IIO_PCIEAER_UNCORRERR_MSK   0x003ff030      // UNCERRMSK     UNCEDMASK     UNCERRSEV

// Used to override the AER mask registers
// Set bits that need to be masked due to known issues
#define IIO_AER_CEM_OVERRIDE    0x00000000        
#define IIO_AER_UEM_OVERRIDE    0x00100000


#define   LNERR_MASK_DATA   0x3F0FFEF;
#define   LFERR_MASK_DATA   0x3F0FFEF;
#define   LCERR_MASK_DATA   0x7F0FFEF;
#define   LERRCTL_MASK_DATA 0x7F0FFEF;

#define   IIO_XPCORERR_MASK_DATA    0x1
#define   IIO_XPUNCERRD_MASK_DATA   0x7f
#define   IIO_AERCORERR_MASK_DATA   0x31C1
#define   IIO_LERRCTL_MASK_DATA     0x90FFE0 //Mask out DMI and all PCIe error sources.
  
#define   IIO_STACK_RAS_DEV        0x5
#define   IIO_STACK_RAS_FUNC       0x2
extern IIO_UDS                         *mIioUds;

typedef struct {
  UINT8   UrMaskSet;
  UINT8   BusNum;
  UINT8   Device;
  UINT8   Function;
  UINT8   NmiSciFlag;
  UINT32  HdrLog1;
} SRIOV_UR_ERR_DEV_INFO;

////////////////////////////////////// COMMON ROUTINES /////////////////////////////////////////


////////////////////////////////////// ERROR HANDLING ROUTINES /////////////////////////////////////////


////////////////////////////////////// INITIALIZATION ROUTINES /////////////////////////////////////////

  
  
VOID
InitErrDevInfo (
  VOID
  );
  
  VOID
InitIOHInfo(
  VOID
  );


EFI_STATUS
IioDmiErrorEnable (
  VOID
  );

#endif


