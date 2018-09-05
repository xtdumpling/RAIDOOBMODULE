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


/** @file CmosManagerPeiLib.c
    Contains the routines to used CMOS Manager's services
    during the PEI phase.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <CmosAccess.h>


/**
    This function creates the locates the CMOS Access interface.

    @param Cmos This is the returned interface.

    @retval EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS CmosLocateAccessInterface(
    OUT  EFI_CMOS_ACCESS_INTERFACE    **Cmos)
{
    CONST EFI_PEI_SERVICES      **PeiServices = NULL;
    EFI_GUID                    Guid = EFI_PEI_CMOS_ACCESS_GUID;

    if  (Cmos == NULL){
        return EFI_INVALID_PARAMETER;
    }

    PeiServices = GetPeiServicesTablePointer();
    if (PeiServices == NULL){
        return EFI_UNSUPPORTED;
    }

    return (*PeiServices)->LocatePpi(PeiServices, &Guid, 0, NULL, Cmos);
}


/**
    This function reads a byte from CMOS whose location is specified by
    the encoded value in CmosToken.

    @param CmosToken This is the encoded CMOS Token value.

    @retval UINT8 (Return value)
        = This is the byte that was read from CMOS.

    @note
      No error checking is performed. (If the returned value is 0xFF, it
      is possible that an error occurred.)

      This is essentially a thin wrapper for CmosReadEx, where minimal
      parameters are used.

**/

UINT8 CmosReadByte(
    IN  UINT16  CmosToken )
{
    return CmosReadByteEx(CmosToken, NULL, NULL);
}


/**
    This function reads a byte from CMOS whose location is specified by
    the encoded value in CmosToken. This version provides full-featured
    support for code optimization and error checking.

    @param CmosToken    This is the encoded CMOS Token value.
    @param InterfacePtr This is the address of the interface structure pointer
                        or NULL. The interface structure pointer, if specified,
                        will be initialized if its value is NULL.

                        If a non-NULL address is specified and the address to
                        which it points is NULL, the interface will be located
                        and the pointer will be updated.

                        If a non-NULL address is specified and the address to
                        which it points in on-NULL, the specified interface
                        will be used to access CMOS.

    @param StatusOut    This is the address of an EFI_STATUS variable or
                        NULL.  If this pointer is non-NULL, its value will be
                        updated with a valid UEFI status code on exit from this
                        function.

    @retval UINT8 (Return value)
        = This is the byte that was read from CMOS.

    @note
      Full error checking is performed.

      The implementation makes a single call to EFI_PEI_SERVICES.LocatePpi
      to update InterfacePtr, if specified.  On subsequent calls,
      within the same scope, the InterfacePtr is directly used. (This
      is a more efficient implementation for early PEI phase execution.)

      In the DXE phase, a global variable is initialized on the first
      call, such that InterfacePtr does not need to be specified. However,
      it will be used if specified.

**/

UINT8 CmosReadByteEx(
    IN      UINT16                       CmosToken,
    IN OUT  EFI_CMOS_ACCESS_INTERFACE    **InterfacePtr OPTIONAL,
    OUT     EFI_STATUS                   *StatusOut     OPTIONAL )
{
    CONST EFI_PEI_SERVICES      **PeiServices = NULL;
    EFI_CMOS_ACCESS_INTERFACE   *Cmos = NULL;
    EFI_STATUS					StatusLocal;
    EFI_STATUS                  *StatusPtr;
    UINT8                       CmosByte = 0xFF;

    if (StatusOut != NULL)
        StatusPtr = StatusOut;
    else
        StatusPtr = &StatusLocal;

    // If InterfacePtr is a valid pointer to an
    // EFI_CMOS_ACCESS_INTERFACE structure, use it and return.
    if (InterfacePtr != NULL && *InterfacePtr != NULL){
        Cmos = (EFI_CMOS_ACCESS_INTERFACE*)*InterfacePtr;
        *StatusPtr = Cmos->Read(Cmos, CmosToken, &CmosByte);
        return CmosByte;
    }

    // Locate the PPI
    *StatusPtr = CmosLocateAccessInterface(&Cmos);
    if (EFI_ERROR(*StatusPtr)){
        return CmosByte;
    }

    // If InterfacePtr is a valid pointer, update it;
    if (InterfacePtr != NULL){
        *InterfacePtr = (VOID*)Cmos;
    }

    // Read from the CMOS location.
    *StatusPtr = Cmos->Read(Cmos, CmosToken, &CmosByte);
    return CmosByte;
}


/**
    This function writes a byte to CMOS whose location is specified by
    the encoded value in CmosToken.

    @param CmosToken This is the encoded CMOS Token value.
    @param CmosByte  This is the byte value to be written.

    @retval EFI_STATUS (Return Value)
        - this is a valid EFI status code.

    @note
      This is essentially a thin wrapper for CmosWriteEx, where the
      CMOS Access interface is not specified.

**/

EFI_STATUS CmosWriteByte(
    IN      UINT16                       CmosToken,
    IN      UINT8                        CmosByte)
{
    return CmosWriteByteEx(CmosToken, CmosByte, NULL);
}


/**
    This function writes a byte to CMOS whose location is specified by
    the encoded value in CmosToken. This version provides full-featured
    support for code optimization.

    @param CmosToken    This is the encoded CMOS Token value.
    @param CmosByte     This is the byte value to be written.
    @param InterfacePtr This is the address of the interface structure pointer
                        or NULL. The interface structure pointer, if specified,
                        will be initialized if its value is NULL.

                        If a non-NULL address is specified and the address to
                        which it points is NULL, the interface will be located
                        and the pointer will be updated.

                        If a non-NULL address is specified and the address to
                        which it points in on-NULL, the specified interface
                        will be used to access CMOS.

    @retval EFI_STATUS (Return Value)
        - this is a valid EFI status code.

    @note
      The implementation makes a single call to EFI_PEI_SERVICES.LocatePpi
      to update InterfacePtr, if specified.  On subsequent calls,
      within the same scope, the InterfacePtr is directly used. (This
      is a more efficient implementation for early PEI phase execution.)

      In the DXE phase, a global variable is initialized on the first
      call, such that InterfacePtr does not need to be specified. However,
      it will be used if specified.

**/

EFI_STATUS CmosWriteByteEx(
    IN      UINT16                       CmosToken,
    IN      UINT8                        CmosByte,
    IN OUT  EFI_CMOS_ACCESS_INTERFACE    **InterfacePtr OPTIONAL)
{
    CONST EFI_PEI_SERVICES      **PeiServices = NULL;
    EFI_CMOS_ACCESS_INTERFACE   *Cmos = NULL;
    EFI_STATUS					Status;

    // If InterfacePtr is a valid pointer to an
    // EFI_CMOS_ACCESS_INTERFACE structure, use it and return.
    if (InterfacePtr != NULL && *InterfacePtr != NULL){
        Cmos = (EFI_CMOS_ACCESS_INTERFACE*)*InterfacePtr;
        return Cmos->Write(Cmos, CmosToken, CmosByte);
    }

    // Locate the PPI
    Status = CmosLocateAccessInterface(&Cmos);
    if (EFI_ERROR(Status)){
        return Status;
    }

    // If InterfacePtr is a valid pointer, update it;
    if (InterfacePtr != NULL){
        *InterfacePtr = (VOID*)Cmos;
    }

    // Write to the CMOS location.
    return Cmos->Write(Cmos, CmosToken, CmosByte);
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
