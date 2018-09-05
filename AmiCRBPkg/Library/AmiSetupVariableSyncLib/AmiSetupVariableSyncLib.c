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

/** @file AmiSetupVariableSyncLib.c

    This file contains ELINK to sync AMI and Intel above4G decode value.
*/

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Guid/SocketIioVariable.h>
#include <Include/PciSetup.h>
#include <Guid/SetupVariable.h>

// Produced Protocols

// Consumed Protocols

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
EFI_GUID    gSetupGuid = SETUP_GUID;
EFI_GUID    gPciSetupGuid = PCI_FORM_SET_GUID;
// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)
EFI_STATUS DxeGetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data
);


EFI_STATUS DxeSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);


/**
    Update Above4gDecode setup option value to Intel Pci64BitResourceAllocation when ever there is a change in Pci64BitResourceAllocation.


    @retval EFI_SUCCESS

**/

EFI_STATUS IntelPciResourceSetupUpdate (
  IN UINT8 *Pci64BitResourceAllocation
)
{
    UINTN                         VarSize;
    UINT32                        Attributes;
#if PCI_SETUP_USE_APTIO_4_STYLE
    SETUP_DATA                    SetupData;
#else
    PCI_COMMON_SETUP_DATA         PciSetupData;
#endif
    EFI_STATUS                    Status;

#if PCI_SETUP_USE_APTIO_4_STYLE

    VarSize = sizeof(SETUP_DATA);
    Status = DxeGetVariable (
                    L"Setup",
                    &gSetupGuid,
                    &Attributes,
                    &VarSize,
                    &SetupData
                    );
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
        return Status;
    }

    if (*Pci64BitResourceAllocation != SetupData.Above4gDecode) {
        //
        // Update Intel value to match AMI PCI setup value.
        //
        DEBUG((EFI_D_INFO, "Current: *Pci64BitResourceAllocation: %d\n", *Pci64BitResourceAllocation));
        *Pci64BitResourceAllocation = SetupData.Above4gDecode;
        DEBUG((EFI_D_INFO, "Updated: *Pci64BitResourceAllocation: %d\n", *Pci64BitResourceAllocation));
    }
#else

    VarSize = sizeof(PCI_COMMON_SETUP_DATA);
    Status = DxeGetVariable (
                    L"PCI_COMMON",
                    &gPciSetupGuid,
                    &Attributes,
                    &VarSize,
                    &PciSetupData
                    );
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
        return Status;
    }
    if (*Pci64BitResourceAllocation != PciSetupData.Above4gDecode) {
        //
        // Update Intel value to match AMI PCI setup value.
        //
        DEBUG((EFI_D_INFO, "Current: *Pci64BitResourceAllocation: %d\n", *Pci64BitResourceAllocation));
        *Pci64BitResourceAllocation = PciSetupData.Above4gDecode;
        DEBUG((EFI_D_INFO, "Updated: *Pci64BitResourceAllocation: %d\n", *Pci64BitResourceAllocation));
    }
#endif

    return EFI_SUCCESS;

}

/**
    Update Intel Pci64BitResourceAllocation based on Above4gDecode setup
    option value changes. 

    @retval EFI_SUCCESS

**/

EFI_STATUS AmiPciResourceSetupUpdate (
  IN UINT8 Above4gDecode
)
{
    UINTN                         VarSize;
    UINT32                        Attributes;
    SOCKET_IIO_CONFIGURATION      SocketIioConfiguration;
    EFI_STATUS                    Status;

    VarSize = sizeof(SOCKET_IIO_CONFIGURATION);
    Status = DxeGetVariable (
                              SOCKET_IIO_CONFIGURATION_NAME,
                              &gEfiSocketIioVariableGuid,
                              &Attributes,
                              &VarSize,
                              &SocketIioConfiguration
                              );
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
        return Status;
    }

    if (Above4gDecode != SocketIioConfiguration.Pci64BitResourceAllocation) {
        DEBUG((EFI_D_INFO, "Current: Pci64BitResourceAllocation: %d\n", SocketIioConfiguration.Pci64BitResourceAllocation));
        SocketIioConfiguration.Pci64BitResourceAllocation = Above4gDecode;
        DEBUG((EFI_D_INFO, "Updated: Pci64BitResourceAllocation: %d\n", SocketIioConfiguration.Pci64BitResourceAllocation));
        Status = DxeSetVariable (
                          SOCKET_IIO_CONFIGURATION_NAME,
                          &gEfiSocketIioVariableGuid,
                          Attributes,
                          sizeof (SOCKET_IIO_CONFIGURATION),
                          &SocketIioConfiguration
                          );
        ASSERT_EFI_ERROR (Status);
    }
    return EFI_SUCCESS;
}

/**
    Update Intel SRIOVEnable based on SriovSupport setup
    option value changes. 
    
    @Param *SRIOVEnable - Points to SRIOVEnable value of IntelRcSetup

    @retval EFI_SUCCESS

**/

VOID
IntelPciResourceSriovUpdate (
  IN UINT8 *SRIOVEnable
)
{
    UINTN                         VarSize;
    UINT32                        Attributes;
#if PCI_SETUP_USE_APTIO_4_STYLE
    SETUP_DATA                    SetupData;
#else
    PCI_COMMON_SETUP_DATA         PciSetupData;
#endif
    EFI_STATUS                    Status;

#if PCI_SETUP_USE_APTIO_4_STYLE

    VarSize = sizeof(SETUP_DATA);
    Status = DxeGetVariable (
                    L"Setup",
                    &gSetupGuid,
                    &Attributes,
                    &VarSize,
                    &SetupData
                    );
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
        return;
    }

    if (*SRIOVEnable != SetupData.SriovSupport) {
        //
        // Update Intel value to match AMI PCI setup value.
        //
        *SRIOVEnable = SetupData.SriovSupport;
        DEBUG((EFI_D_INFO, "Updated: *SRIOVEnable %d\n", *SRIOVEnable));

    }
#else

    VarSize = sizeof(PCI_COMMON_SETUP_DATA);
    Status = DxeGetVariable (
                    L"PCI_COMMON",
                    &gPciSetupGuid,
                    &Attributes,
                    &VarSize,
                    &PciSetupData
                    );
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
        return;
    }
    if (*SRIOVEnable != PciSetupData.SriovSupport) {
        //
        // Update Intel value to match AMI PCI setup value.
        //
        *SRIOVEnable = PciSetupData.SriovSupport;
    }
#endif

    return;

}

/**
    Update Intel SRIOVEnable based on SriovSupport setup
    option value changes. 
    
    @Param SriovSupport - Value of SriovSupport in AMI PCI setup

    @retval EFI_SUCCESS

**/

VOID 
AmiPciResourceSriovUpdate (
    IN UINT8 SriovSupport
) {
    EFI_STATUS                    Status;
    UINTN                    VarSize;
    UINT32                   Attributes;
    SYSTEM_CONFIGURATION     SystemConfiguration;

    VarSize = sizeof(SYSTEM_CONFIGURATION);
    Status = DxeGetVariable (  
                    PLATFORM_SETUP_VARIABLE_NAME,
                    &gEfiSetupVariableGuid,
                    &Attributes,
                    &VarSize,
                    &SystemConfiguration
                    );
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
        return;
    }

    if (SriovSupport != SystemConfiguration.SRIOVEnable) {
        SystemConfiguration.SRIOVEnable = SriovSupport;
        Status = DxeSetVariable (
                          PLATFORM_SETUP_VARIABLE_NAME,
                          &gEfiSetupVariableGuid,
                          Attributes,
                          sizeof (SYSTEM_CONFIGURATION),
                          &SystemConfiguration
                          );
        ASSERT_EFI_ERROR (Status);
    }
    return;
}


/**
    Call back for set variable hook to sync Pci64BitResourceAllocation with AMI Above4gDecode.


    @retval EFI_SUCCESS

**/
EFI_STATUS
SetNvramVariableSyncHook ( 
  IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
  IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
  )
{

    //
    // Check to synchronize change in AMI PCI setup option.
    //
#if PCI_SETUP_USE_APTIO_4_STYLE
    if ((!guidcmp(VendorGuid, &gSetupGuid)) && (!StrCmp (VariableName, L"Setup"))) {
        AmiPciResourceSetupUpdate (((SETUP_DATA*)Data)->Above4gDecode);
        AmiPciResourceSriovUpdate (((SETUP_DATA*)Data)->SriovSupport);
    }
#else
    if ((!guidcmp(VendorGuid, &gPciSetupGuid)) && (!StrCmp (VariableName, L"PCI_COMMON"))) {
        AmiPciResourceSetupUpdate(((PCI_COMMON_SETUP_DATA*)Data)->Above4gDecode);
        AmiPciResourceSriovUpdate (((PCI_COMMON_SETUP_DATA*)Data)->SriovSupport);
    }
#endif
    
    //
    // Check to synchronize change in Intel PCI setup option.
    //
    if ((!guidcmp(VendorGuid, &gEfiSocketIioVariableGuid)) && (!StrCmp (VariableName, SOCKET_IIO_CONFIGURATION_NAME))) {
       IntelPciResourceSetupUpdate(&((SOCKET_IIO_CONFIGURATION*)Data)->Pci64BitResourceAllocation );
    }
    
    if ((!guidcmp( VendorGuid, &gEfiSetupVariableGuid)) && (!StrCmp( VariableName, PLATFORM_SETUP_VARIABLE_NAME))) {
        IntelPciResourceSriovUpdate(&((SYSTEM_CONFIGURATION *)Data)->SRIOVEnable);
    }

    return EFI_UNSUPPORTED;
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
