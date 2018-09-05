//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /AptioV/BIN/AMIDebugRx/DbgHostStatusLib/CommonDebug.c 1     11/02/12 10:14a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:14a $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRx/DbgHostStatusLib/CommonDebug.c $
// 
//
//*********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          CommonDebug.C
//
// Description:   Common Debug definitions
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include "Efi.h"
#include "Pei.h"
#include "Include/Library/IoLib.h"
//#include "misc.h"
#ifndef	EFIx64
#include <Library/AMIpeidebug.h>
#else
#include <Library/AMIpeidebugX64.h>
#endif
#include <Library/AMIPeiGUIDs.h>

#include "Token.h"

#ifdef REDIRECTION_ONLY_MODE
#if REDIRECTION_ONLY_MODE
volatile UINTN gRedirectionOnlyEnabled = 1;
#else
volatile UINTN gRedirectionOnlyEnabled = 0;
#endif
#endif

#ifdef DBG_PERFORMANCE_RECORDS
UINTN gDbgPerformanceRecords = DBG_PERFORMANCE_RECORDS;
#else
UINTN gDbgPerformanceRecords = 0;
#endif

#ifdef DBG_WRITE_IO_80_SUPPORT
volatile UINTN gDbgWriteIO80Support = DBG_WRITE_IO_80_SUPPORT;
#else
volatile UINTN gDbgWriteIO80Support = 0;
#endif

#ifndef GENERIC_USB_CABLE_SUPPORT
#define GENERIC_USB_CABLE_SUPPORT 0
#endif

#ifdef USB_3_DEBUG_SUPPORT
UINT8 gDbgUSB3DebugSupport = USB_3_DEBUG_SUPPORT;
#else
UINT8 gDbgUSB3DebugSupport = 0;
#endif


#ifdef USB_3_DEBUG_DEBUGPORT_INIT_WAIT
UINT8 gDbgUSB3Wait = USB_3_DEBUG_DEBUGPORT_INIT_WAIT;
#else
UINT8 gDbgUSB3Wait = 0;
#endif

#ifdef DYNAMIC_MMCFG_BASE_SUPPORT
UINT8 gDynamicPCIExBase = DYNAMIC_MMCFG_BASE_SUPPORT;
#else
UINT8 gDynamicPCIExBase = 0;
#endif

#define CORE05_BUS_NUM                            0
#define CORE05_DEV_NUM                            5
#define CORE05_FUNC_NUM                           0
#define R_IIO_MMCFG_B0                         0x90              // MMCFG Address Range //AptioV server override: Dynamic mmcfg base address change

#define MmioAddress(BaseAddr, Register) \
    ( (UINTN)BaseAddr + (UINTN)(Register) )

#define Mmio32Ptr(BaseAddr, Register) \
    ( (volatile UINT32 *)MmioAddress(BaseAddr, Register) )

#define Mmio32(BaseAddr, Register) \
    *Mmio32Ptr(BaseAddr, Register)

// Load Fv Support
UINTN gFvMainBase = FV_MAIN_BASE;
UINTN gFvMainBlocks = FV_MAIN_BLOCKS;
UINTN gFvBBBlocks = FV_BB_BLOCKS;
UINTN gBlockSize = FLASH_BLOCK_SIZE;

volatile UINTN gGenericUsbSupportEnabled = GENERIC_USB_CABLE_SUPPORT;

EFI_GUID  mPeiDbgBasePpiGuid = EFI_PEI_DBG_BASEADDRESS_PPI_GUID;

EFI_GUID  mDxeDbgDataGuid = DXE_DBG_DATA_GUID;
INT8 CompareGuid(EFI_GUID *G1, EFI_GUID *G2);

UINTN DebugDataBaseAddress = 0;
UINTN SMMDebugDataBaseAddress = 0;
UINTN DxeDataBaseAddress = 0;

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetPciExBaseAddr
//
// Description:	Returns the PCIBase Address
//
// Input:		VOID
//
// Output:		UINTN 
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>

UINTN  GetPciExBaseAddr()
{
	UINT8 CoreBusNum=0,CoreDevNum=0,CoreFuncNum=0,IIOMMCfgB0=0;
	
	if(gDynamicPCIExBase){
#ifdef DBG_CORE05_BUS_NUM
	  CoreBusNum = DBG_CORE05_BUS_NUM;
#endif
#ifdef DBG_CORE05_DEV_NUM
	  CoreDevNum = DBG_CORE05_DEV_NUM;
#endif
#ifdef DBG_CORE05_FUNC_NUM
	  CoreFuncNum = DBG_CORE05_FUNC_NUM;
#endif
#ifdef DBG_R_IIO_MMCFG_B0
	  IIOMMCfgB0 = DBG_R_IIO_MMCFG_B0;
#endif
	  IoWrite32 (0xCF8, 0x80000000 | (CoreBusNum << 16) + (CoreDevNum << 11) + (CoreFuncNum << 8) + IIOMMCfgB0);
	  return (UINTN)(IoRead32 (0xCFC) & 0xFFFFFFF0);
	}
	else
	   return (UINTN) PcdGet64 (PcdPciExpressBaseAddress);
}

#define PCIE_CFG_ADDRESS(bus, dev, func, reg) \
    ((VOID*)(UINTN)(GetPciExBaseAddr() + ((UINT8)(bus) << 20) + ((UINT8)(dev) << 15) + ((UINT8)(func) << 12) + (reg)))

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetTsegBaseAddress 
//
// Description: Returns the base address of TSEG.
//
// Input:       None
//
// Output:      UINT32 - The Base Address of TSEG.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT32 GetTsegBaseAddress (VOID)
{
    // The TSEG DRAM memory base address register is 0xB8 in the MC device(B:0D:0F:0).
    return (Mmio32(PCIE_CFG_ADDRESS(0, 0, 0, 0), 0xB8) & 0xFFF00000);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetTargetVersion 
//
// Description: Returns the Build version of the target eModule.
//
// Input:       None
//
// Output:      UINT8.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 GetTargetVersion()
{
#ifdef AMI_DEBUG_BUILD
	return AMI_DEBUG_BUILD;
#else
	return 0;
#endif
}
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
