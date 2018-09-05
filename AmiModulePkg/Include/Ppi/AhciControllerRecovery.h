//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AhciControllerRecovery.h
    Ahci Controller Init PPI definition

**/

#ifndef _PEI_AHCI_CONTROLLER_PPI_H
#define _PEI_AHCI_CONTROLLER_PPI_H
#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include    "Efi.h"
#include    "Pei.h"

//---------------------------------------------------------------------------

#define PEI_AHCI_CONTROLLER_PPI_GUID \
  {0xe0a4add8, 0xf07d, 0x4d7a, 0xbe, 0x6c, 0xac, 0x6, 0x6, 0x93, 0xf, 0x4f }

#define PEI_AHCI_CONTROLLER_2_PPI_GUID \
  {0xc5cbdbea, 0x5364, 0x4bca, 0x80, 0xa3,0xd0, 0xc0, 0x96, 0x97, 0x21, 0xcb}


typedef struct _PEI_AHCI_CONTROLLER_PPI PEI_AHCI_CONTROLLER_PPI;

typedef
EFI_STATUS (
EFIAPI *PEI_REC_INIT_SATA_CONTROLLER) (
    IN  EFI_PEI_SERVICES           **PeiServices,
    IN  PEI_AHCI_CONTROLLER_PPI    *This
);

typedef struct _PEI_AHCI_CONTROLLER_PPI {
    PEI_REC_INIT_SATA_CONTROLLER    RecInitSataController;
}PEI_AHCI_CONTROLLER_PPI;

typedef struct _PEI_AHCI_CONTROLLER_2_PPI  PEI_AHCI_CONTROLLER_2_PPI;

/** 
  Gets the SB SATA Controller's PCI Location to program the ABAR Address.
  
  @param[in]       PeiServices           Specifies pointer to the PEI Services data structure.
  @param[in]       This                  Indicates the PEI_AHCI_CONTROLLER_PPI instance. 
  @param[in, out]  Bus                   Specifies Bus Number of the programmed PCH SATA Controller.
  @param[in, out]  Device                Specifies Device Number of the programmed PCH SATA Controller.
  @param[in, out]  Function              Specifies Function Number of the programmed PCH SATA Controller.
  
  @retval EFI_SUCCESS                    The operation succeeded return EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          The input parameter is illegal.
    
**/
typedef
EFI_STATUS (
EFIAPI *PEI_GET_SB_SATA_LOCATION) (
    IN  EFI_PEI_SERVICES             **PeiServices,
    IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
    IN OUT UINT8                     *Bus,
    IN OUT UINT8                     *Device,
    IN OUT UINT8                     *Function
);
/** 
  Performs SB SATA Controller Initialization.
  
  @param[in]       PeiServices           Specifies pointer to the PEI Services data structure.
  @param[in]       This                  Indicates the PEI_AHCI_CONTROLLER_PPI instance. 
  @param[in]       AhciMmioBaseAddress   Mmio Base Address programmed in ABAR of the PCH SATA Controller.
  
  @retval EFI_SUCCESS                    The operation succeeded return EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          The input parameter is illegal.
    
**/
typedef
EFI_STATUS (
EFIAPI *PEI_REC_INIT_SB_SATA_CONTROLLER) (
    IN  EFI_PEI_SERVICES             **PeiServices,
    IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
    IN  UINTN                        AhciMmioBaseAddress
);

typedef struct _PEI_AHCI_CONTROLLER_2_PPI {
    PEI_GET_SB_SATA_LOCATION           GetSbSataLocation;
    PEI_REC_INIT_SB_SATA_CONTROLLER    RecInitSbSataController;
} PEI_AHCI_CONTROLLER_2_PPI;

#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
