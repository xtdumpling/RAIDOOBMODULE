/** @file 
  This is Chipset Error Reporting library Private Interface 
  
Copyright (c)  1999 - 2016 Intel Corporation. All rights 
reserved This software and associated documentation (if any) is 
furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by 
such license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/
#ifndef _CHIPSET_ERR_REPORTING_PRIVATE_H_
#define _CHIPSET_ERR_REPORTING_PRIVATE_H_

//
// Includes
//
//#include <Cpu/CpuRegs.h>
#include "SysFunc.h"
#include "RcRegs.h"
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Library/PciExpressLib.h>
#include <Library/DebugLib.h>
#include <Iio/IioRegs.h>
#include <Library/mpsyncdatalib.h>
#include <IndustryStandard/Pci.h>
#include <Library/SmmServicesTableLib.h>
#include <mca_msr.h>


//
// Defines
//
#define   LNERR_MASK_DATA   0x3F0FFEF;
#define   LFERR_MASK_DATA   0x3F0FFEF;
#define   LCERR_MASK_DATA   0x7F0FFEF;
#define   LERRCTL_MASK_DATA 0x7F0FFEF;

#define R_RPPIOERRHF							0x08
#define R_RPPIOERRSTS							0x0C
#define R_RPPIOERRMSK							0x10

#define ONLY_REGISTER_OFFSET(x)  (x & 0xFFF)

#define PCIE_EXT_CAP_HEADER_RPPIOERR_CAPID          0x000b
#define PCIE_EXT_CAP_HEADER_RPPIOERR_VSECID         0x0007

#define IIO_PCIEADD_UNCORR_MSK      0x0000037a      // XPUNCEDMASK   XPUNCERRSEV   XPUNCERRMSK
#define IIO_PCIEAER_CORRERR_MSK     0x000031c1      // COREDMASK     CORERRMSK
#define IIO_PCIEAER_UNCORRERR_MSK   0x0037f030      // UNCERRMSK     UNCEDMASK     UNCERRSEV
#define IIO_GxERRST_PCIE_MASK   0x10ffe0
#define		IIO_GLBL_ERR_BITMASK			0x0000003F;		//<<SKXIIORAS-TODO: Update after Silicon DE provide

// Used to override the AER mask registers
// Set bits that need to be masked due to known issues
#define IIO_AER_CEM_OVERRIDE    0x00000000        
#define IIO_AER_UEM_OVERRIDE    0x00100000

#define   V_SYSMAP_NoInband                         000                         // No inband message
#define   V_SYSMAP_SMI_PMI                          001                         // Generate SMI/PMI
#define   V_SYSMAP_NMI                              002                         // Generate NMI
#define   V_SYSMAP_CPEI                             003                         // Generate CPEI
#define   V_SYSMAP_MCA                              004                         // Generate MCA

#define   V_ERRPINCTL_DIS                           00
#define   V_ERRPINCTL_DATA                          01
#define   V_ERRPINCTL_SEV                           02
#define   V_ERRPINCTL_RSVD                          03

#define IIO_CORRECTABLE 0x0
#define IIO_RECOVERABLE 0x1
#define IIO_FATAL       0x2


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
#define IIO_PCIEAER_UNCORRERR_MSK   0x0037f030      // UNCERRMSK     UNCEDMASK     UNCERRSEV
// APTIOV_SERVER_OVERRIDE_RC_START : To resolve build error.
#define PCI_EXPRESS_EXTENDED_CAPABILITY_ADVANCED_ERROR_REPORTING_ID   0x0001
// APTIOV_SERVER_OVERRIDE_RC_END : To resolve build error.
//
// Type Definitions
//
//

typedef enum  {
  LocalSingleStackRegs,
  LocalMultiStackRegs,
  IioErrStsGlobalRegs,
  IioErrStsGlblErrPinRegs
} IIO_ERRSTS_REG_TYPE;

typedef struct  {
  UINT32  RegDef;
//UINT8   RegWidth;
// Add other attributes if necessary
} IIO_ERRSTS_REG;

//
// Functions prototype declarations 
//

/**
  Clear IIO Error Status Registers which require write "1" to clear (RW1C).

  @param[in]  Arg1  Socket ID
  @param[in]  Arg2  Error Register Type Local, Global, etc.

  @retval  EFI_SUCCESS 
**/
EFI_STATUS 
ClearErrStsRegisters (
  IN UINT8    SktId,
  IN UINT8    BoxInst,
  IN UINT8    IIO_ERRSTS_REG_TYPE
);

/**
  This function will enable all the RP PIO errors.
  
  @param[in] Ioh        The IOH number.

  @retval    Status.
**/ 
EFI_STATUS
RppIoErrEnable(
  IN UINT8 RootBridge
);

/**
  This Function will enable all the Pcie errors, that need to be reported as LER.

  @param[in] Ioh        The IOH number.

  @retval    Status.
**/
EFI_STATUS
IIOLerEnable (
  IN      UINT8   Ioh
  );

/**
  This function returns whether the socket is in a viral state or not.

  @retval ViralState  True if in viral state, otherwise False

**/
BOOLEAN
SocketInViralState (
  IN    UINT8    Iio
  );

/**
  This function enables or disables poison forwarding and viral

  @retval None.

**/
VOID
ProgramViralPoison(
  ); 

/**
  This function enable forwarding of Poison indication with the data.
  
  @param[in] Ioh        The IOH number.

  @retval    Status.
**/
EFI_STATUS
EnablePoisonIoh (
  IN UINT8 ioh
  );

EFI_STATUS
IioEnableViral (
  IN UINT8                    IIO
  );

#endif //_CHIPSET_ERR_REPORTING_PRIVATE_H_
