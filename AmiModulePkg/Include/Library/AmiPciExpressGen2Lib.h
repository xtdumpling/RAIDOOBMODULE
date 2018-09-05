//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header:$
//
// $Revision:  $
//
// $Date: $
//*************************************************************************
/** @file AmiPciExpressGen2Lib.h
    Library Class for AMI Pci Express GEN2 Support Functions.

**/
//*************************************************************************
#ifndef _AMI_PCI_EXPRESS_GEN_2_LIB_H_
#define _AMI_PCI_EXPRESS_GEN_2_LIB_H_
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pci.h>
#include <PciE.h>
#include <PciE21.h>
#include <PciE30.h>
#include <PciBus.h>
#include <AmiLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

/**
    This function will collect information about PCIE GEN2 Device
    and initialize PCIE2_DATA structure based on information collected.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2GetGen2Info(PCI_DEV_INFO *Device);

/**
    This function will Enable ARI Forwarding in DownSream Port of
    the device passed if
    1.Device referenced is an ARI device;
    2.Parenting Bridge supports ARI Forwarding.
    3.ARI Firvarding Setup Question Set to "Enabled"

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_UNSUPPORTED         When Device or Parenting Bridge does not support ARI
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2EnableAri(PCI_DEV_INFO *Device, BOOLEAN EnableOption);


/**
    This function will Enable ARI Forwarding in DownSream Port of
    the device passed if
    1.Device referenced is an ARI device;
    2.Parenting Bridge supports ARI Forwarding.
    3.ARI Firvarding Setup Question Set to "Enabled"

        
    @param Device Pointer to PCI Device Private Data structure.
    @param MultiFunc Pointer to a Flag to modify if Device is MF Device
    @param AriEnabled Pointer to a Flag to modify if this function was able to ENABLE ARI.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2CheckAri(PCI_DEV_INFO *Device, BOOLEAN *MultiFunc, BOOLEAN *AriEnabled);

/**
    This function will Select values for Link Control2 register on
    both sides of the LINK based on Setup Settings and hardware capabilities.

        
    @param DnStreamPort Pointer to PCI Device Private Data of Downstream Port of the link.
    @param DnLnkCnt2 Pointer to the LNK_CNT2 Reg of the Downstream Port of the link.
    @param UpStreamPort Pointer to PCI Device Private Data of Upwnstream Port of the link.
    @param UpLnkCnt2 Pointer to the LNK_CNT2 Reg of the Downstream Port of the link.
    @param LinkHotResetRequired Flag to modify if Link will need HOT RESET after programming.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetLnkProperties(PCI_DEV_INFO           *DnStreamPort,
                                 PCIE_LNK_CNT2_REG      *DnLnkCnt2,
                                 PCI_DEV_INFO           *UpStreamPort,
                                 PCIE_LNK_CNT2_REG      *UpLnkCnt2,
                                 BOOLEAN                *LinkHotResetRequired,
                                 BOOLEAN                *LinkRetrainRequired
                                 );

/**

    @param 

         
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.

**/
BOOLEAN Pcie2CheckPcie2Compatible(PCI_DEV_INFO	*Device);


/**
    This function will select appropriate Completion Timeout range
    from supported by the device.

        
    @param Support Supported by Device Completion Timeout ranges.
    @param Short A Flag to Indicate wahat type of ranges to select Biggest or Smallest

    @retval UINT16
        Value to be programmed in DEV_CNT2 Register.

**/
UINT16 Pcie2SelectComplTimeOut(UINT32 Support, BOOLEAN Short);

/**
    This function will Select values for DEVICE CCONTROL2 register
    based on Setup Settings and hardware capabilities.

        
    @param DnStreamPort Pointer to PCI Device Private Data of Downstream Port of the link.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetDevProperties(	PCI_DEV_INFO 		*Device);


/**

    @param 

         
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.

**/
EFI_STATUS Pcie2AllocateInitPcie2Data(PCI_DEV_INFO *Device);

/**
    This function will Set CRS Sw visibility if device supports it.

        
    @param Device Pointer to PCI Device Private Data of Downstream Port of the link.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_ERROR        		When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetCrs(PCI_DEV_INFO *Device);

/**
    This function will Set CRS Software visibility trough RCRB Ext. Capability if device supports it.

        
    @param Device Pointer to PCI Device Private Data of Downstream Port of the link.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_ERROR        		When driver fails access PCI Bus.

**/
EFI_STATUS Pcie2SetCrsExt(PCI_DEV_INFO *Device);


/**

    @param 

         
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.

**/

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif //_AMI_PCI_EXPRESS_GEN_2_LIB_H_
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

