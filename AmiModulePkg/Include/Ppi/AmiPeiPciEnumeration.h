//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  @brief The file contains definition of the AMI PEI PCI Enumeration PPI.
**/

#ifndef __AMI_PEI_PCI_ENUMERATION_PPI__H__
#define __AMI_PEI_PCI_ENUMERATION_PPI__H__

#define AMI_PEI_PCI_ENUMERATION_PPI_GUID \
  {0xE7B5B715,0x1183, 0x4533, { 0xBE, 0x76, 0x56, 0xA6, 0xD7, 0xCE, 0xB0, 0x2E } }

///
/// The forward declaration for AMI_PEI_PCI_ENUMERATION_PPI
///
typedef struct _AMI_PEI_PCI_ENUMERATION_PPI AMI_PEI_PCI_ENUMERATION_PPI;

/**
  Structure definition that represents the PCI Device Location.
 */
typedef struct {  
    UINT8    SegmentNumber;
    UINT8    BusNumber;
    UINT8    DeviceNumber;
    UINT8    FunctionNumber;
} AMI_PEI_PCI_DEVICE_LOCATION;

/**
  Structure definition that represents the PCI Device Class Code.
 */
typedef struct
{
    UINT8    BaseClassCode;
    UINT8    SubClassCode;
    UINT8    ProgrammingInterface;
} AMI_PEI_PCI_CLASSCODE;

/** 
  Performs Partial Enumeration and initializes the devices based on the Root Bridge and Class code
  specified through the ELink's PEI_PCI_BRIDGE_LIST and PEI_PCI_CLASSCODE_LIST.
  
  @param[in]  This                  AMI_PEI_PCI_ENUMERATION_PPI instance. 
  @retval EFI_SUCCESS               PCI Bridge list provided through ELink "PEI_PCI_BRIDGE_LIST"
                                    is successfully Enumerated.
  @retval EFI_INVALID_PARAMETER     The input parameter is illegal.
  @retval EFI_OUT_OF_RESOURCES      There is insufficient memory to store the PCI Device Data.
 */
typedef
EFI_STATUS 
(EFIAPI *AMI_PEI_PCI_ENUMERATION_INIT_DEVICES) (
    IN  AMI_PEI_PCI_ENUMERATION_PPI    *This
);

/** 
  Based on the specified Class Code input and DevInstance Number,finds the PCI devices
  and returns the PCI device location. 
  
  @param[in]      This                AMI_PEI_PCI_ENUMERATION_PPI instance. 
  @param[in]      PciClassCode        Specifies the Class Code Structure of the PCI Device to fetch.
  @param[in]      DevInstance         Instance number to discover.
  @param[in, out] PciDevice           Pointer to reference the found PciDevice.
  
  @retval EFI_SUCCESS                 PciDevice data is found at the specified Input DevInstance. 
  @retval EFI_NOT_FOUND               No PciDevice is found at that instance.
 */
typedef
EFI_STATUS 
(EFIAPI *AMI_PEI_PCI_ENUMERATION_GET_NEXT_DEVICE_BY_CLASSCODE) (
    IN  AMI_PEI_PCI_ENUMERATION_PPI     *This,
    IN  AMI_PEI_PCI_CLASSCODE           PciClassCode,
    IN  UINT8                           DevInstance,
    OUT AMI_PEI_PCI_DEVICE_LOCATION     *PciDevice
);

/** 
  Resets the resources allocated to the PCI Devices,like Secondary Bus Number,
  Subordinate Bus Number, MEM/IO Base Address and Limit Register values.
  This is to avoid the conflict with DXE drivers. 
  
  @param[in]  This              Indicates the AMI_PEI_PCI_ENUMERATION_PPI instance. 
  @retval EFI_SUCCESS           PCI Bridge and Device Resources are cleared successfully.
  @retval EFI_INVALID_PARAMETER The input parameter is illegal.
 */
typedef EFI_STATUS 
(EFIAPI *AMI_PEI_PCI_ENUMERATION_RESET_DEVICE_RESOURCES) (
    IN  AMI_PEI_PCI_ENUMERATION_PPI     *This
);

/**
  PPI that can be used to perform Partial Enumeration and Resource Allocation
  for PCI Devices with the specified class codes.    
 */
struct _AMI_PEI_PCI_ENUMERATION_PPI {
    AMI_PEI_PCI_ENUMERATION_INIT_DEVICES                    InitDevices;
    AMI_PEI_PCI_ENUMERATION_GET_NEXT_DEVICE_BY_CLASSCODE    GetNextDeviceByClassCode;
    AMI_PEI_PCI_ENUMERATION_RESET_DEVICE_RESOURCES          ResetDeviceResources;
};

/// GUID Variable Definition
extern EFI_GUID gAmiPeiPciEnumerationPpiGuid;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
