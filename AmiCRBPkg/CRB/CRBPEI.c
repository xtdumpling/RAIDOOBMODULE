//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file CRBPEI.c

    This file contains code for Chipset Reference Board Template initialization in the PEI stage
*/

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Pei.h>
#include <Token.h>
// Dynamic mmcfg base address Support change
#ifdef DYNAMIC_MMCFG_BASE_FLAG 
#include "IioRegs.h"
#endif  

#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Hob.h>
#include <Setup.h>
#include <AmiCspLib.h>
#include <AmiCrbInc.h>//Build directory
#include "UncoreCommonIncludes.h"
#include <AmiPcieSegBusLib/AmiPcieSegBusLib.h>

#include <CrbPeiInitElink.h>
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x00010000)
#include <PPI\smbus2.h>
#else
#include <PPI\smbus.h>
#endif
#ifndef DISABLE_CRB_CHANGE_BOOT_MODE_ON_END_OF_PEI
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/PchPolicy.h>
#endif
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x00010000)
EFI_GUID gEfiPeiSmbus2PpiGuid = EFI_PEI_SMBUS2_PPI_GUID;
#else
EFI_GUID gPeiSmBusPpiGuid = EFI_PEI_SMBUS_PPI_GUID;
#endif

// PPI Definition(s)

// Function Definition(s)
EFI_STATUS InitCK505ClockGen(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *NullPpi
);

EFI_STATUS AmiUpdateSegBusPcd(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                      *NullPpi
);

EFI_STATUS CrbInitOnEndOfPei (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
    IN VOID                               *Ppi
);
// PPI that are installed

// PPI that are notified
static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = 
{
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiSiliconRcHobsReadyPpi,
    AmiUpdateSegBusPcd
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiEndOfPeiSignalPpiGuid,
    CrbInitOnEndOfPei
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x00010000)
    &gEfiPeiSmbus2PpiGuid,
#else
    &gPeiSmBusPpiGuid,
#endif
    InitCK505ClockGen
  }
};

// External Declaration(s)

//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   CRBPEI_Init
//
// Description: This function is the entry point for this PEI. This function
//              initializes the chipset CRB
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns:     Return Status based on errors that occurred while waiting for
//              time to expire.
//
// Notes:       This function could initialize CRB for anything.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS EFIAPI CRBPEI_Init (
    IN       EFI_PEI_FILE_HANDLE   FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices )
{
    EFI_STATUS Status = EFI_SUCCESS;
#if PI_SPECIFICATION_VERSION < 0x00010000
    EFI_PEI_PCI_CFG_PPI *PciCfg;   
#else
    EFI_PEI_PCI_CFG2_PPI *PciCfg;
#endif
    EFI_PEI_CPU_IO_PPI          *CpuIo;

// Dynamic mmcfg base address Support change
    #ifdef DYNAMIC_MMCFG_BASE_FLAG 
    UINT32                SecpciExbase;   

    // Read MMCFG_BASE Register (@ B:D:F;Offset :: 0:5:0;0x90) and Update PcdPciExpressBaseAddress with MMCFG_BASE Value
    IoWrite32 (0xCF8, 0x80000000 | (CORE05_BUS_NUM << 16) + (CORE05_DEV_NUM << 11) + (CORE05_FUNC_NUM << 8) + R_IIO_MMCFG_B0);
    SecpciExbase = IoRead32 (0xCFC) & 0xFFFFFFF0; 
    DEBUG((EFI_D_INFO, "SecpciExbase:%x\n", SecpciExbase));
    PcdSet64 (PcdPciExpressBaseAddress, (UINT64) SecpciExbase);
    DEBUG((EFI_D_INFO, "PcdPciExpressBaseAddress:%x\n", PcdGet64 (PcdPciExpressBaseAddress)));    
    #endif  


    // Get pointer to the PCI config PPI
    PciCfg = (*PeiServices)->PciCfg;
    CpuIo = (*PeiServices)->CpuIo;

    // Set the CRB Notify PPI
    Status = (*PeiServices)->NotifyPpi(PeiServices, mNotifyList);
    ASSERT_PEI_ERROR (PeiServices, Status);

    return EFI_SUCCESS;
}


EFI_STATUS InitCK505ClockGen(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *NullPpi
)
{
    EFI_STATUS		Status;
    EFI_PEI_SMBUS2_PPI	*SmBus;
    EFI_SMBUS_DEVICE_ADDRESS	Address;
    UINTN		Len = 12;
    UINT8		CK505TBL[] = { 0x95, 0x83, 0xff, 0xff, 0xff, 0x00, 0x00, 0x95, 0x13, 0x65, 0x7d, 0x56 };

    DEBUG((EFI_D_INFO, "Program CK505 start.\n"));
    return EFI_SUCCESS;
    Address.SmbusDeviceAddress = 0x69;

	Status = (*PeiServices)->LocatePpi( PeiServices,
				&gEfiPeiSmbus2PpiGuid,
				0,
				NULL,
				&SmBus );
	ASSERT_PEI_ERROR( PeiServices, Status);

	SmBus->Execute( SmBus,
			Address,
			0,
			EfiSmbusWriteBlock,
			0,
			&Len,
			CK505TBL
			);

    DEBUG((EFI_D_INFO, "Program CK505 end.\n"));


	return EFI_SUCCESS;
}

EFI_STATUS AmiUpdateSegBusPcd(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                      *NullPpi
)
{
  EFI_STATUS              Status;
  PCIE_SEG_BUS_TABLE      *PcieSegBusTable;
      
  //Get the PcdPcieSegBusTablePtr
  PcieSegBusTable = (PCIE_SEG_BUS_TABLE *) PcdGetPtr (PcdPcieSegBusTablePtr);
      
  if (!PcieSegBusTable->ValidFlag) {
    Status = AmiUpdatePcieSegmentBusTable (PcieSegBusTable);
    if (EFI_ERROR(Status))
      return Status;  
  }
    
  DEBUG((EFI_D_INFO, __FUNCTION__"\n"));
  return EFI_SUCCESS;
}

#ifndef DISABLE_CRB_CHANGE_BOOT_MODE_ON_END_OF_PEI
EFI_STATUS CrbChangeBootModeOnEndOfPei ( VOID )
{
  EFI_BOOT_MODE         BootMode;
  PCH_POLICY_PPI        *PchPolicyHob;
  EFI_PEI_HOB_POINTERS  Hob;

  BootMode = GetBootModeHob ();
  //
  // Get PCH Policy HOB.
  //
  Hob.Guid = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (Hob.Guid != NULL);
  if (Hob.Guid == NULL) {
    return EFI_NOT_FOUND;
  }
  PchPolicyHob = GET_GUID_HOB_DATA(Hob.Guid);
  
  if (BootMode == BOOT_ON_FLASH_UPDATE || BootMode == BOOT_IN_RECOVERY_MODE) {
    PchPolicyHob->LockDownConfig.BiosLock = 0;
  }
  return EFI_SUCCESS;
}
#endif


EFI_STATUS CrbInitOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
)
{
  EFI_STATUS            Status;

#ifndef DISABLE_CRB_CHANGE_BOOT_MODE_ON_END_OF_PEI
  Status = CrbChangeBootModeOnEndOfPei();
#endif

  return Status;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
