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
/** @file SBPEI.C

    This file contains code for Template Southbridge initialization
*/

// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiPeiLib.h>
#include <AmiCspLib.h>
#include <PchAccess.h>
#include <ppi\CpuIo.h>
#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
#include    <Ppi/AhciControllerRecovery.h>
#endif
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Register/PchRegsPmc.h>
#include <Library/MmPciBaseLib.h>


// {95E8152B-1B98-4f11-8A77-DB26583EBC42}
#define AMI_PEI_SBINIT_POLICY_PPI_GUID  \
 {0x95e8152b, 0x1b98, 0x4f11, 0x8a, 0x77, 0xdb, 0x26, 0x58, 0x3e, 0xbc, 0x42}

typedef struct _AMI_PEI_SBINIT_POLICY_PPI   AMI_PEI_SBINIT_POLICY_PPI;

struct _AMI_PEI_SBINIT_POLICY_PPI {
    UINTN         unFlag;
};

// Produced PPIs

// GUID Definitions

// Portable Constants

// Function Prototypes

// PPI interface definition
EFI_GUID gAmiPEISBInitPolicyGuid     = AMI_PEI_SBINIT_POLICY_PPI_GUID;
#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
EFI_GUID gPeiAhciControllerPpiGuid   = PEI_AHCI_CONTROLLER_2_PPI_GUID;
#endif

//----------------------------------------------------------------------------
// PPI Definition
//----------------------------------------------------------------------------

static AMI_PEI_SBINIT_POLICY_PPI mAmiPeiSbInitPolicyPpi =
{
    TRUE
};

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
#pragma pack(1)
typedef struct _AHCI_PEI_PRIVATE_DATA
{
	PEI_AHCI_CONTROLLER_2_PPI	AhciControllerPpi;
	UINT8				Segment;
	UINT8				Bus;
	UINT8				Device;
	UINT8				Function;	
} AHCI_PEI_PRIVATE_DATA;
#pragma pack()

EFI_STATUS
EFIAPI
GetSbSataLocation (
    IN  EFI_PEI_SERVICES             **PeiServices,
    IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
    IN OUT UINT8                     *Bus,
    IN OUT UINT8                     *Device,
    IN OUT UINT8                     *Function
);

EFI_STATUS
EFIAPI
RecInitSbSataController (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_AHCI_CONTROLLER_2_PPI      *This,
  IN UINTN                         AhciMmioBaseAddress
);
#endif

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT

static AHCI_PEI_PRIVATE_DATA gSataAhciControllerPpi =
{
		{
		    GetSbSataLocation,
		    RecInitSbSataController
		},
		0,
		DEFAULT_PCI_BUS_NUMBER_PCH,
		PCI_DEVICE_NUMBER_PCH_SATA,
		PCI_FUNCTION_NUMBER_PCH_SATA
};

static AHCI_PEI_PRIVATE_DATA gsSataAhciControllerPpi =
{
		{
		    GetSbSataLocation,
		    RecInitSbSataController
		},
		0,
		DEFAULT_PCI_BUS_NUMBER_PCH,
		PCI_DEVICE_NUMBER_PCH_SSATA,
		PCI_FUNCTION_NUMBER_PCH_SSATA
};
#endif

//----------------------------------------------------------------------------
// PPI that are installed
//----------------------------------------------------------------------------
static EFI_PEI_PPI_DESCRIPTOR gPpiList[] =
{

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT    
    { EFI_PEI_PPI_DESCRIPTOR_PPI, &gPeiAhciControllerPpiGuid, &gsSataAhciControllerPpi },
    { EFI_PEI_PPI_DESCRIPTOR_PPI, &gPeiAhciControllerPpiGuid, &gSataAhciControllerPpi },
#endif

    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gAmiPEISBInitPolicyGuid, &mAmiPeiSbInitPolicyPpi }

};

VOID CheckTcoNewCenturyBit (VOID)
{
    UINT16  TcoSts1;
    UINT16  TcoSts2;
    UINT8   Century;
    UINT8   Value;
    
    TcoSts1 = IoRead16(TCO_BASE_ADDRESS + R_PCH_TCO1_STS);
    
    if (TcoSts1 & B_PCH_TCO1_STS_NEWCENTURY) {
        TcoSts2 = IoRead16(TCO_BASE_ADDRESS + R_PCH_TCO2_STS);
        
        // B_PCH_TCO1_STS_NEWCENTURY is set
        // If RTC Power failure, skip increase the century
        if (!(MmioRead16 (
                  MmPciBase (
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_PMC,
                  PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B) & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS))
        {
        	IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
        	Century = IoRead8(CMOS_DATA_PORT); // Read register.
                    
        	Value = BcdToDecimal8(Century);
        	Value++;
        	Century = DecimalToBcd8(Value);
                    
        	IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
        	IoWrite8(CMOS_DATA_PORT, Century);
        }
        
        // clear B_PCH_TCO1_STS_NEWCENTURY bit
        IoWrite8(TCO_BASE_ADDRESS + R_PCH_TCO1_STS, B_PCH_TCO1_STS_NEWCENTURY);               
        if ((TcoSts1 == B_PCH_TCO1_STS_NEWCENTURY) && (TcoSts2 == 0)) {
            IoWrite32(ACPI_BASE_ADDRESS + R_PCH_SMI_STS, B_PCH_SMI_STS_TCO);
        }
    }    
}
    
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SBPEI_Init
//
// Description: This function is the entry point for this PEI. This function
//    initializes the chipset SB
//
// Input:  FfsHeader Pointer to the FFS file header
//         PeiServices Pointer to the PEI services table
//
// Output:  Return Status based on errors that occurred while waiting for
//          time to expire.
//
// Notes:  This function should initialize South Bridge for memory detection.
//    Install AMI_PEI_SBINIT_POLICY_PPI to indicate that SB Init PEIM
//    is installed
//    Following things can be done at this point:
//     - Enabling top of 4GB decode for full flash ROM
//     - Programming South Bridge ports to enable access to South
//      Bridge and other I/O bridge access
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
EFIAPI
SBPEI_Init(
    IN       EFI_PEI_FILE_HANDLE   FileHandle,
    IN CONST EFI_PEI_SERVICES          **PeiServices
)
{
    EFI_STATUS                  Status;
    EFI_PEI_PCI_CFG2_PPI         *PciCfg;
    EFI_PEI_CPU_IO_PPI          *CpuIo;


    PciCfg = (*PeiServices)->PciCfg;
    CpuIo = (*PeiServices)->CpuIo;

    PEI_PROGRESS_CODE(PeiServices, PEI_CAR_SB_INIT);

    // Check TCO1_STS NewCentury bit 
    CheckTcoNewCenturyBit ();
    
    // Install the SB Init Policy PPI
    Status = (*PeiServices)->InstallPpi(PeiServices, gPpiList);
    ASSERT_PEI_ERROR(PeiServices, Status);

    return EFI_SUCCESS;
}


#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: GetSbSataLocation
//
// Description: Gets the SB SATA Controller's PCI Location to program 
//              the ABAR Address.
//
// Input:  PeiServices           Specifies pointer to the PEI Services data structure.
//         This                  Indicates the PEI_AHCI_CONTROLLER_PPI instance. 
//         Bus                   Specifies Bus Number of the programmed PCH SATA Controller.
//         Device                Specifies Device Number of the programmed PCH SATA Controller.
//         Function              Specifies Function Number of the programmed PCH SATA Controller.
//
// Output:  Return Status based on errors that occurred
// 
//
// Notes: 
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetSbSataLocation (
    IN  EFI_PEI_SERVICES             **PeiServices,
    IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
    IN OUT UINT8                     *Bus,
    IN OUT UINT8                     *Device,
    IN OUT UINT8                     *Function
)
{
  AHCI_PEI_PRIVATE_DATA *PrivateData = (AHCI_PEI_PRIVATE_DATA *)This;
  
  *Bus      = PrivateData->Bus;
  *Device   = PrivateData->Device;
  *Function = PrivateData->Function;
  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
RecInitSbSataController (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_AHCI_CONTROLLER_2_PPI        *This,
  IN UINTN                         AhciMmioBaseAddress
)
{
  AHCI_PEI_PRIVATE_DATA 				*PrivateData = (AHCI_PEI_PRIVATE_DATA *)This;

  DEBUG((EFI_D_INFO, "\nSbPei.c : \n"__FUNCTION__));
  
  ///
  /// Force To AHCI Mode
  ///
  PciAnd8( PCI_LIB_ADDRESS(PrivateData->Bus, PrivateData->Device, PrivateData->Function, R_PCH_SATA_SATAGC + 2), (UINT8) ~(BIT0));

  ///
  /// Enable ABAR decode
  ///
  PciOr16( PCI_LIB_ADDRESS(PrivateData->Bus, PrivateData->Device, PrivateData->Function, 0x04), BIT02 | BIT01);

  // Enable all sata ports
  MmioOr32 (AhciMmioBaseAddress + R_PCH_SATA_AHCI_PI, (UINT32) ~(0));

  //
  // Two reads required (C-spec ch. 25 - 1.9.14.1)
  //
  MmioRead32 (AhciMmioBaseAddress + R_PCH_SATA_AHCI_PI);
  MmioRead32 (AhciMmioBaseAddress + R_PCH_SATA_AHCI_PI);

  return EFI_SUCCESS;
}

#endif //AhciRecovery_SUPPORT

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

