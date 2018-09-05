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


/** @file CmosAccess.c
    Contains the routines that constitute the CMOS access
    implementation.  This file contains source code that is used
    for both PEI and DXE phases.

**/

#include <Efi.h>
#include <CmosAccess.h>
#include <SspTokens.h>
#include <CmosManagerHooks.h>
#include "CmosManager.h"
#include "CmosManagerHob.h"
#include "CmosBoard.h"


//---------------------------------------------------------------------------
// Function(s) originally defined in CmosManager.c
//---------------------------------------------------------------------------
extern EFI_CMOS_MANAGER_INTERFACE   *GetCmosMangerInterface(
    IN EFI_CMOS_ACCESS_INTERFACE    *Cmos );


//---------------------------------------------------------------------------
// Function declarations for this file
//---------------------------------------------------------------------------
EFI_STATUS CalculateUpdatedCheckSum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN  UINT16                      CmosAddress,
    IN  UINT8                       OldCmosValue,
    IN  UINT8                       NewCmosValue,
    OUT UINT16                      *NewChecksum );

EFI_STATUS CalculateChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    OUT UINT16                      *ChecksumValue );

EFI_STATUS WriteChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    OUT UINT16                      ChecksumValue );


/**
    Value is shifted left by the bit field size specified in Token.

    @param Value Address of value to be encoded
    @param Token Token describing the bit field size

**/

VOID EncodeParameter(
    OUT UINT8       *Value,
    IN  CMOS_TOKEN  *Token )
{
    *Value <<= Token->Value.Field.BitOffset;
}


/**
    Value is shifted right and masked by the bit field's size and offset
    as specified in Token.

    @param Value Address of value to be decoded
    @param Token Token describing the bit field's size and offset

**/

VOID DecodeParameter(
    OUT UINT8        *Value,
    IN CMOS_TOKEN   *Token )
{
    *Value >>= Token->Value.Field.BitOffset;
    *Value &=  (0xff >> (8 - Token->Value.Field.Size));
}


/**
    For testing whether or not a Value overflows the field size
    as specified in Token.

    @param Value Value to be tested
    @param Token Token describing the bit field's size

**/

BOOLEAN BitFieldOverflow(
    IN UINT8        *Value,
    IN CMOS_TOKEN   *Token )
{
    if ( *Value & ~(0xff >> (8 - Token->Value.Field.Size)) )
        return TRUE;
    else
        return FALSE;
}


/**
    Creates a mask with bits set corresponding to the size and offset
    of the bit field as specified by Token.

    @param Mask  Mask to be created
    @param Token Token describing the bit field's size and offset

**/

VOID SetClobberBits(
    OUT UINT8       *Mask,
    IN CMOS_TOKEN   *Token )
{
    *Mask = 0xff >> (8 - Token->Value.Field.Size);
    *Mask <<= Token->Value.Field.BitOffset;
}


/**
    Returns index of CMOS token in token table or 0 if not found.

    @param Mgr       This is the CMOS Manager interface pointer.
    @param CmosToken Encoded CMOS token for which to locate an index into
                     the CMOS token table

    @retval UINT16 (Return Value)
        = Index of the CmosToken in the token table, or
        = 0, if the CmosToken was not found

**/

UINT16 CmosFindToken(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN  UINT16                      CmosToken )
{
    UINT16   i = 0;   // First valid token table index is 1

    while (++i < Mgr->TokenCount) {
        if (Mgr->TokenTable[i].Value.AllBits == CmosToken) {
            break;
        }
    }

    if (i < Mgr->TokenCount)
        return i;

    CMOS_TRACE_FULL(( Mgr,
                "CmosFindToken: Mgr->TokenCount = 0x%02X\n",
                Mgr->TokenCount));
    return 0;
}


/**
    Returns encoded token for the specified register.

    @param Mgr          This is the CMOS Manager interface pointer.
    @param CmosRegister CMOS register to for which to search

    @retval UINT16 (Return Value)
        = Token found in the token table, or
        = 0, if the Register was not found

    @note
      Caller must ensure the register has been reserved properly
      in SSP.

      This should be considered a risky function call and extra care
      should be taken to ensure the proper CmosRegister is specified.

      This function has the potential for problems, as it is possible that
      a specified CmosRegister has not been defined in SSP and/or may
      be used by another CmosToken.

      All verification of parameters must be done by the caller.

**/

UINT16 CmosGetTokenFromRegister(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  UINT16                      CmosRegister )
{
    UINT16                      i = 0;
    EFI_CMOS_MANAGER_INTERFACE  *Mgr = GetCmosMangerInterface(Cmos);

    // Mgr->TokenCount is the total number of entries in
    // Mgr->TokenTable where the first entry is zero.
    while (++i < Mgr->TokenCount) {
        if (Mgr->TokenTable[i].Value.Field.CmosAddress == CmosRegister){
            break;
        }
    }

    // Minimal error checking is to ensure it is an 8-bit CMOS Token
    if (i < Mgr->TokenCount ){
        if (Mgr->TokenTable[i].Value.Field.Size == 8)
            return Mgr->TokenTable[i].Value.AllBits;
    }

    // a valid token is greater than 0x1000
    return 0;
}


/**
    This function determines which structure in the global CMOS_PORT_MAP
    array corresponds to the provided CmosAddress and either calls the
    associated function or uses the associated IO ports to read/write the
    CMOS register value to/from the CmosParameterValue.

    @param Mgr                Pointer to the CMOS Manager internal interface
    @param AccessType         WriteType or ReadType
    @param CmosAddress        CMOS register to for which the access is to be made
    @param CmosParameterValue This is the value to write for write access or the
                              value that was read for read access.

    @retval EFI_STATUS (Return Value)
        EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS CmosBankReadWrite(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN  CMOS_ACCESS_TYPE            AccessType,
    IN  UINT16                      CmosAddress,
    IN OUT UINT8                    *CmosParameterValue )
{
    UINT8       i = 1;
    EFI_STATUS  Status = EFI_NOT_FOUND;

    // scan the CMOS_PORT_MAP array to determine how to access this address
    for (; i < Mgr->CmosBankCount; i++)
    {
      if (     (CmosAddress >= Mgr->CmosBank[i].Low)
            && (CmosAddress <= Mgr->CmosBank[i].High) )
      {
        // First, attempt to call the board-specific function if available
        if (Mgr->CmosBank[i].BoardReadWrite != NULL){
            Status = Mgr->CmosBank[i].BoardReadWrite( Mgr->Access.PeiServices,
                                              AccessType,
                                              CmosAddress,
                                              CmosParameterValue );
            break;
        }

        // Otherwise, attempt to use the provided index/data ports
        // if available (assuming 8 bit port access)
        else if ( (Mgr->CmosBank[i].Index > 0) && (Mgr->CmosBank[i].Data > 0) ){
            IoWrite8( Mgr->CmosBank[i].Index, (UINT8)CmosAddress );
            if (AccessType == ReadType){
                *CmosParameterValue = IoRead8( Mgr->CmosBank[i].Data );
            }
            else {
                IoWrite8( Mgr->CmosBank[i].Data, *CmosParameterValue );
            }
            Status = EFI_SUCCESS;
            break;
        }
      }
    }
    return Status;
}


/**
    This function provides support for performing the actual
    read or write from/to a physical CMOS location.


    @param Mgr                Manager interface pointer
    @param AccessType         Specifies whether to perform a read or a write
    @param CmosAddress        Actual CMOS address/offset
    @param BitsToWrite        Mask specifying the bits to be written (these
                              bits will be cleared first)
    @param CmosParameterValue CMOS value to be written or, on successful exit, will
                              contain the value that was read

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

**/

EFI_STATUS CmosPhysicalReadWrite(
    IN      EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN      CMOS_ACCESS_TYPE            AccessType,
    IN      UINT16                      CmosAddress,
    IN      UINT8                       BitsToWrite,
    IN OUT  UINT8                       *CmosParameterValue )
{
    UINT8       CmosRegisterValue;
    UINT8       NewValue;
    UINT16      NewChecksum = 0;

    // Always read the value
    CmosBankReadWrite( Mgr,
                       ReadType,
                       CmosAddress,
                       &CmosRegisterValue );

    // If read access, simply return the unmodified data
    if ( AccessType == ReadType ) {
        CMOS_TRACE_FULL(( Mgr,
            "    Physical Read: Register 0x%02X = 0x%02X\n",
            CmosAddress, CmosRegisterValue));
        *CmosParameterValue = CmosRegisterValue;
    }

    // If writing, then don't clobber unused bits
    else {
        NewValue = (CmosRegisterValue & ~BitsToWrite) | *CmosParameterValue;

        // get the new checksum before writing
        if (!Mgr->CheckStatus(Mgr, CMOS_FORCE_NO_CHECKSUM))
            CalculateUpdatedCheckSum( Mgr, CmosAddress,
                CmosRegisterValue, NewValue, &NewChecksum);

        // write the value
        CMOS_TRACE_FULL(( Mgr,
            "    Physical Write: Register 0x%02X = 0x%02X\n",
            CmosAddress, NewValue));
        CmosBankReadWrite( Mgr,
                           WriteType,
                           CmosAddress,
                           &NewValue );

        // write the checksum, if necessary
        // (this write will cause an infinite loop without a flag)
        if (!Mgr->CheckStatus(Mgr, CMOS_FORCE_NO_CHECKSUM)){
            if (Mgr->CheckStatus(Mgr, CMOS_ADDRESS_IS_CHECKSUMMED)){
                Mgr->SetStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);
                WriteChecksum( Mgr, NewChecksum );
                Mgr->ClearStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);
            }
        }
    }

    return EFI_SUCCESS;
}


/**
    This function returns TRUE if the CmosAddress is NOT included in the
    checksum and returns FALSE otherwise.

    @param Mgr         Manager interface pointer
    @param CmosAddress Actual CMOS address/offset

    @retval BOOLEAN (Return Value)
        = TRUE if CmosAddress is NOT included in the checksum
        = FALSE if CmosAddress IS included in the checksum

**/

BOOLEAN NotChecksummed(
    IN      EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN      UINT16                      CmosAddress )
{
    UINT16  NoChecksumIndex = 1;    // NoChecksumTable starts at 1

    while ( (NoChecksumIndex < Mgr->NoChecksumCount)
         && (CmosAddress > Mgr->NoChecksumTable[NoChecksumIndex].Index) )
    {
        ++NoChecksumIndex;
    }

    if ( CmosAddress == Mgr->NoChecksumTable[NoChecksumIndex].Index )
        return TRUE;
    else
        return FALSE;
}


/**
    This function provides support for performing the actual
    read or write from/to a CMOS buffer location (using the
    Optimal Defaults Table Buffer).

    @param Mgr                Manager interface pointer
    @param AccessType         Specifies whether to perform a read or a write
    @param CmosAddress        Actual CMOS address/offset
    @param BitsToWrite        Mask specifying the bits to be written (these
                              bits will be cleared first)
    @param CmosParameterValue CMOS value to be written or, on successful exit, will
                              contain the value that was read

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = or other valid EFI error code

    @note
      Control comes here because of one of two reasons:

      1) The checksum is bad

                  All writes/read are to/from the
                  Mgr->OptimalDefaultTable only for locations not found
                  in the Mgr->NoChecksumTable until DXE phase.

      2) The battery is bad

                  All writes/read are to/from the
                  Mgr->OptimalDefaultTable for all locations.

**/


EFI_STATUS CmosBufferReadWrite(
    IN      EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN      CMOS_ACCESS_TYPE            AccessType,
    IN      UINT16                      CmosAddress,
    IN      UINT8                       BitsToWrite,
    IN OUT  UINT8                       *CmosParameterValue )
{
    UINT8       NewValue;
    UINT16      NewChecksum = 0;
    UINT8       CmosRegisterValue;
    // First CMOS register @ buffer index 1
    UINT16      BufferIndex = CmosAddress - FIRST_CMOS_REGISTER + 1;

    if (CmosAddress < FIRST_CMOS_REGISTER || CmosAddress > LAST_CMOS_REGISTER){
        CMOS_TRACE((Mgr,
            "    CmosBufferReadWrite: Invalid CmosAddress (0x%X)\n",
            CmosAddress ));
        return EFI_INVALID_PARAMETER;
    }

    // get the default value from the table
    CmosRegisterValue = Mgr->OptimalDefaultTable[BufferIndex].Value;

    // If read access, simply return the unmodified data
    if ( AccessType == ReadType ) {
        *CmosParameterValue = CmosRegisterValue;
        CMOS_TRACE_FULL((Mgr,
            "    Buffer Read: 0x%X = OptimalDefaultTable[%d].Value \n",
            CmosRegisterValue, BufferIndex  ));
    }

    // If writing, mark the location as dirty and don't clobber unused bits
    else {
        NewValue = (CmosRegisterValue & ~BitsToWrite) | *CmosParameterValue;

        // get the new checksum before writing (this sets or clears
        // the CMOS_ADDRESS_IS_CHECKSUMMED Mgr->ManagerStatus flag)
        if (!Mgr->CheckStatus(Mgr, CMOS_FORCE_NO_CHECKSUM)) {
            CalculateUpdatedCheckSum( Mgr, CmosAddress,
                CmosRegisterValue, NewValue, &NewChecksum);
        }

        // Optimal default table writes take precedence
        Mgr->OptimalDefaultTable[BufferIndex].Value = NewValue;
        CMOS_TRACE_FULL((Mgr,
            "    Buffer Write: OptimalDefaultTable[%d].Value = 0x%X\n",
            BufferIndex, NewValue ));

        // write the checksum, if necessary
        // (this write will cause an infinite loop without a flag)
        if (!Mgr->CheckStatus(Mgr, CMOS_FORCE_NO_CHECKSUM)){
            if (Mgr->CheckStatus(Mgr, CMOS_ADDRESS_IS_CHECKSUMMED)){
                Mgr->SetStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);
                WriteChecksum( Mgr, NewChecksum );
                Mgr->ClearStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);
                CMOS_TRACE_FULL((Mgr,
                    "    NewChecksum = 0x%X\n",
                    NewChecksum ));
            }
        }
    }

    return EFI_SUCCESS;
}


/**
    Decides which version of xxxReadWrite to call based upon the
    current Access Routing Logic.


    @param Mgr           Manager interface pointer
    @param AccessType    Specifies whether to perform a read or a write
    @param CmosAddress   Actual CMOS address/offset
    @param BitsToWrite   Mask specifying the bits to be written (these
                         bits will be cleared first)
    @param CmosParameter CMOS value to be written or, on successful exit, will
                         contain the value that was read

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS CmosReadWrite(
    IN      EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN      CMOS_ACCESS_TYPE            AccessType,
    IN      UINT16                      CmosAddress,
    IN      UINT8                       BitsToWrite,
    IN OUT  UINT8                       *CmosParameter )
{
    BOOLEAN UseBufferAccess = FALSE;

    // If the checksum is bad or the battery is bad, accesses will be
    // routed to the optimal default table by default.
    if ( Mgr->CheckStatus( Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED )){
        UseBufferAccess = TRUE;

        // Force physical access if these conditions are applicable:
        //
        //      1. the CMOS is usable,
        //      2. recovery is limited to only checksummed locations
        //      3. the current location is not checksummed
        if (    Mgr->CheckStatus( Mgr, CMOS_IS_USABLE )
            &&  Mgr->CheckStatus( Mgr, CMOS_RECOVER_ONLY_CHECKSUMMED )
            &&  NotChecksummed( Mgr, CmosAddress ) )
        {
           UseBufferAccess = FALSE;
        }
    }

    if (UseBufferAccess)
        return CmosBufferReadWrite( Mgr,
                                    AccessType,
                                    CmosAddress,
                                    BitsToWrite,
                                    CmosParameter);
    else
        return CmosPhysicalReadWrite(   Mgr,
                                        AccessType,
                                        CmosAddress,
                                        BitsToWrite,
                                        CmosParameter);
}


/**
    This is the main worker function which verifies and either
    reads/writes a value from/to the CMOS location as specified by the
    encoded token.


    @param Mgr             This is the CMOS Manager interface pointer.
    @param AccessType      Specifies whether to perform a read or a write
    @param TokenTableIndex Index into the token table
    @param CmosParameter   CMOS value to be written or, on successful exit, will
                           contain the value that was read

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = EFI_INVALID_PARAMETER, if the CmosParameter does
        not comply with the expected size
        = or other valid EFI error code

    @note
                  1) Gets the CMOS token from the global table
                  2) If writing, the value is error-checked for size and
                      up-shifted to the correct bit position. Also, the
                      BitsToWrite mask is used to specify
                      which bits to clear in the destination before
                      writing.
                  3) This token is decoded to get the CMOS index,
                  4) The read/write is performed
                  5) If reading, the value is down-shifted and masked to
                      return the expected value.

**/

EFI_STATUS CmosReadWriteEncodedToken(
    IN      EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN      CMOS_ACCESS_TYPE            AccessType,
    IN      UINT16                      TokenTableIndex,
    IN OUT  UINT8                       *CmosParameter )
{
    CMOS_TOKEN  *CmosToken = &Mgr->TokenTable[TokenTableIndex];
    UINT8       BitsToWrite = 0;

    // If writing, encode the CmosParameter
    if (AccessType == WriteType) {
        // Check to make sure the data is the correct size for the bit field
        if ( BitFieldOverflow(CmosParameter, CmosToken) ){
        	CMOS_TRACE_FULL((Mgr, "CmosReadWriteEncodedToken: BitFieldOverflow\n" ));
            return EFI_INVALID_PARAMETER;
        }

        // Shift up to the correct offset within the byte
        EncodeParameter(CmosParameter, CmosToken);

        // Set the bits in BitsToWrite mask that are
        // consumed by this token
        SetClobberBits(&BitsToWrite, CmosToken);
    }

    // Read (or write) the CmosParameter to actual (or CMOS buffer) location
    CmosReadWrite(Mgr,
                AccessType,
                (UINT16)CmosToken->Value.Field.CmosAddress,  // byte address
                BitsToWrite,
                CmosParameter);

    // If reading, shift down to bit-position zero and mask off upper bits
    if (AccessType == ReadType)
        DecodeParameter(CmosParameter, CmosToken);

    return EFI_SUCCESS;
}


/**
    Reads to CmosValue from the CMOS location specified by CmosToken.

    @param Cmos      This is the access interface pointer.
    @param CmosToken This is the encoded CMOS location.
    @param CmosValue On success, this will contain the value at
                     the specified CMOS location.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = EFI_NOT_FOUND, if the token is not in the token table
        = or other valid EFI error code

**/

EFI_STATUS CmosRead(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  UINT16                      CmosToken,
    OUT UINT8                       *CmosValue )
{
    EFI_STATUS                      Status = EFI_NOT_FOUND;
    UINT16                          TokenTableIndex = 0;
    EFI_CMOS_MANAGER_INTERFACE      *Mgr = GetCmosMangerInterface(Cmos);

    // if CmosToken <= 0xe, then always read the register.  These registers
    // will be readable regardless of the state of the battery.
    if (CmosToken <= 0xe){
        Status = CmosBankReadWrite( Mgr, ReadType, CmosToken, CmosValue );
    }

    // Otherwise, if CmosToken < MIN_TOKEN_VALUE and the battery is good,
    // then directly  read the register. (For a bad battery, return
    // EFI_DEVICE_ERROR.)
    else if (CmosToken < MIN_TOKEN_VALUE) {
        if ( !Mgr->CheckStatus( Mgr, CMOS_IS_USABLE ) ) {
            Status = EFI_DEVICE_ERROR;
        }
        else {
            Status = CmosBankReadWrite( Mgr, ReadType, CmosToken, CmosValue );
        }
    }

    // Otherwise, after validating the CmosToken, decode it to read the
    // associated register bit(s).
    else {
        TokenTableIndex = CmosFindToken(Mgr, CmosToken);
        if (TokenTableIndex > 0) {
            Status = CmosReadWriteEncodedToken( Mgr, ReadType,
                                                TokenTableIndex, CmosValue);
            if (EFI_ERROR(Status)){
            	CMOS_TRACE_FULL((Mgr, "CmosRead: CmosReadWriteEncodedToken failed\n" ));
            }
        }
        else {
            Status = EFI_INVALID_PARAMETER;
        	CMOS_TRACE_FULL((Mgr, "CmosRead: CmosFindToken failed\n" ));
        }
    }

    return Status;
}


/**
    Reads Bytes from the specified CMOS register(s).


    @param Mgr   Manger interface pointer.
    @param Count Count of CMOS_BYTEs to read
    @param Bytes An array of CMOS_BYTE structures

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS ReadCmosBytes(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN  UINT16                      Count,
    OUT CMOS_BYTE                   *CmosBytes )
{
    UINT16                          i = 0;

    for (; i < Count; i++){
        CmosReadWrite(Mgr, ReadType, CmosBytes[i].Register, 0,
                                         &CmosBytes[i].Value);
    }

    return EFI_SUCCESS;
}


/**
    Initializes a CMOS_STATUS_BYTES structure and reads the associated
    CMOS registers.

    @param Cmos        Manger interface pointer.
    @param StatusBytes Address of a valid, uninitialized, CMOS_STATUS_BYTES
                       structure.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS ReadCmosStatusBytes(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    OUT CMOS_STATUS_BYTES           *StatusBytes )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_CMOS_MANAGER_INTERFACE  *Mgr = GetCmosMangerInterface(Cmos);
    UINT8                       CmosValue;

    if (StatusBytes == NULL)
        return Status = EFI_INVALID_PARAMETER;

    StatusBytes->DiagnosticStatusRegister = CMOS_DIAGNOSTIC_STATUS_REGISTER;
    StatusBytes->BatteryStatusRegister = CMOS_RTC_STATUS_REGISTER;

    CmosPhysicalReadWrite(  Mgr,
                            ReadType,
                            StatusBytes->BatteryStatusRegister,
                            0,
                            &StatusBytes->Battery.AllBits );

    CmosPhysicalReadWrite(  Mgr,
                            ReadType,
                            StatusBytes->DiagnosticStatusRegister,
                            0,
                            &StatusBytes->Diagnostic.AllBits );

    if ( Mgr->CheckStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED) )
        StatusBytes->ConfigurationStatus.IsVirtualized = TRUE;
    else
        StatusBytes->ConfigurationStatus.IsVirtualized = FALSE;

    // If the CMOS hardware is usable, update from cmos-based
    // status bits, which are more reliable.  Otherwise, use the
    // the memory-based status.
    //
    // Note, some information could be lost on reset in DXE
    // when using the memory-based status.
    //
    // Also, register 0xe is cleared in DXE, if the CMOS
    // is usable and the checksum has been recalculated.

    StatusBytes->ConfigurationStatus.BadBattery = FALSE;
    StatusBytes->ConfigurationStatus.DefaultsLoaded = FALSE;
    StatusBytes->ConfigurationStatus.IsFirstBoot = FALSE;
    StatusBytes->ConfigurationStatus.BadChecksum = FALSE;

    if ( Mgr->CheckStatus(Mgr, CMOS_IS_USABLE) ) {
        StatusBytes->ConfigurationStatus.NotUsable = FALSE;

        Cmos->Read( Cmos, CMOS_MGR_BATTERY_BAD, &CmosValue );
        StatusBytes->ConfigurationStatus.BadBattery |= CmosValue;

        Cmos->Read( Cmos, CMOS_MGR_DEFAULTS_LOADED, &CmosValue );
        StatusBytes->ConfigurationStatus.DefaultsLoaded |= CmosValue;

        Cmos->Read( Cmos, CMOS_MGR_FIRST_BOOT_DETECTED, &CmosValue );
        StatusBytes->ConfigurationStatus.IsFirstBoot |= CmosValue;

        Cmos->Read( Cmos, CMOS_MGR_CHECKSUM_BAD,&CmosValue );
        StatusBytes->ConfigurationStatus.BadChecksum |= CmosValue;
    }
    else {
        StatusBytes->ConfigurationStatus.NotUsable = TRUE;
        if ( Mgr->CheckStatus(Mgr, CMOS_BAD_BATTERY) )
            StatusBytes->ConfigurationStatus.BadBattery = TRUE;
        if ( Mgr->CheckStatus(Mgr, CMOS_DEFAULTS_LOADED) )
            StatusBytes->ConfigurationStatus.DefaultsLoaded = TRUE;
        if ( Mgr->CheckStatus(Mgr, CMOS_FIRST_BOOT_DETECTED) )
            StatusBytes->ConfigurationStatus.IsFirstBoot = TRUE;
        if ( Mgr->CheckStatus(Mgr, CMOS_BAD_CHECKSUM) )
            StatusBytes->ConfigurationStatus.BadChecksum = TRUE;
    }

    return Status;
}


/**
    Reads the checksum from the CMOS register(s).


    @param Mgr           Manger interface pointer.
    @param ChecksumValue The returned checksum value

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS ReadChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    OUT UINT16                      *ChecksumValue )
{

    Mgr->Access.Read( &Mgr->Access, CMOS_CHECKSUM_HIGH,
        (UINT8*)ChecksumValue );
    *ChecksumValue <<= 8;
    Mgr->Access.Read( &Mgr->Access, CMOS_CHECKSUM_LOW,
        (UINT8*)ChecksumValue );

    return EFI_SUCCESS;
}


/**
    Write the checksum to the CMOS register(s).

    @param Mgr           Manger interface pointer.
    @param ChecksumValue Value to write to the checksum location.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS WriteChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN  UINT16                      ChecksumValue )
{
    // update the HOB if available
    if (Mgr->ManagerHob != NULL)
        Mgr->ManagerHob->Checksum = ChecksumValue;

    CMOS_TRACE_FULL((Mgr, "WriteChecksum: Value = 0x%X\n",
        ChecksumValue ));

    // update the CMOS checksum
    Mgr->SetStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);
    Mgr->Access.Write( &Mgr->Access, CMOS_CHECKSUM_LOW,
        (UINT8)ChecksumValue );
    ChecksumValue >>= 8;
    Mgr->Access.Write( &Mgr->Access, CMOS_CHECKSUM_HIGH,
        (UINT8)ChecksumValue );
    Mgr->ClearStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);

    return EFI_SUCCESS;
}


/**
    This function is used to update the CMOS battery status.  It is up
    to the user of the Manager interface to determine whether or ot
    the platform supports this feature.  (If the platform does not
    support this feature, the battery status is indeterminate.)


    @param Mgr Pointer to the Manager's interface

    @retval EFI_STATUS (Return value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS UpdateBatteryStatus (
    IN EFI_CMOS_MANAGER_INTERFACE   *Mgr )
{
    EFI_STATUS          Status = EFI_SUCCESS;
    BOOLEAN             IsGood = FALSE;
    EFI_CMOS_BATTERY_TEST  *CmosBatteryIsGood = Mgr->CmosBatteryIsGood;

    CMOS_TRACE_FULL(( Mgr, "\n\nUpdateBatteryStatus Entry\n"));

    // Update the battery status bit in the Manager's CMOS_MANAGER_STATUS
    // field and update the legacy CMOS status register if legacy register
    // updates are enabled.
    while (*CmosBatteryIsGood != NULL){
      IsGood = (*CmosBatteryIsGood)(Mgr->Access.PeiServices);
      ++CmosBatteryIsGood;
    }

    if ( IsGood ) {
        Mgr->ClearStatus(Mgr, CMOS_BAD_BATTERY);
        CMOS_TRACE_FULL(( Mgr, "    ...Battery is good.\n" ));
    }
    else {
        Mgr->SetStatus(Mgr, CMOS_BAD_BATTERY);
        CMOS_TRACE_FULL(( Mgr, "    ...Battery is BAD!!\n" ));
    }

    return Status;
}


/**
    Determines whether or not the legacy status bits in
    CMOS_DIAGNOSTIC_STATUS_REGISTER can be cleared to a non-failure
    status.


    @param Mgr Manger interface pointer


    @retval TRUE bits can be cleared
    @retval FALSE bits can be cleared

    @note

      EFI_CMOS_ACCESS_INTERFACE.PeiServices == NULL in DXE phase.

**/

BOOLEAN CanClearLegacyStatusBits (
    IN      EFI_CMOS_MANAGER_INTERFACE  *Mgr )
{
    BOOLEAN  IsDxePhase = (Mgr->Access.PeiServices == NULL) ? TRUE : FALSE;

    if ( IsDxePhase  ){
        return TRUE;
    }
    else {
        return FALSE;
    }
}


/**
    Synchronizes legacy CMOS status registers with status changes.

    @param Mgr        Manger interface pointer
    @param BitMap     The status bits to be modified
    @param AccessType Specifies whether to set or clear the bits

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

    @note
      The BitMap contains all the bits to be modified in the legacy
      status registers.

      If CMOS_LEGACY_STATUS_ENABLED is off, or is being turned off, this
      function will not modify CMOS registers.

      Failure status bits in CMOS_DIAGNOSTIC_STATUS_REGISTER can only be
      cleared if this is the first boot or if executing in the DXE phase.

**/

EFI_STATUS SynchronizeLegacyStatusRegisters(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN CMOS_MANAGER_STATUS          BitMap,
    IN CMOS_BIT_ACCESS_TYPE         AccessType )
{
    DIAGNOSTIC_STATUS_BYTE  StatusByte;

    CMOS_TRACE_FULL((Mgr,
        "\n\nSynchronizeLegacyStatusRegisters: Entry\n" ));

    // If the checksum status is being modified,
    // then update the ChecksumIsBad bit
    if ( (BitMap & CMOS_BAD_CHECKSUM) != 0 )
    {
        // Read the legacy diagnostic status register
        CmosBankReadWrite(  Mgr, ReadType, CMOS_DIAGNOSTIC_STATUS_REGISTER,
                            &StatusByte.AllBits );

        if (AccessType == SetType){
            CMOS_TRACE_FULL((Mgr,
                "  StatusByte.Field.ChecksumIsBad = 1\n" ));
            StatusByte.Field.ChecksumIsBad = 1;
        }
        else if ( CanClearLegacyStatusBits(Mgr) ) {
            CMOS_TRACE_FULL((Mgr,
                "  StatusByte.Field.ChecksumIsBad = 0\n" ));
            StatusByte.Field.ChecksumIsBad = 0;
        }
        else {
            CMOS_TRACE_FULL((Mgr,
                "  StatusByte.Field.ChecksumIsBad = No Change\n" ));
        }

        // write back the changes
        CmosBankReadWrite(  Mgr, WriteType, CMOS_DIAGNOSTIC_STATUS_REGISTER,
                            &StatusByte.AllBits );
    }
    else {
        CMOS_TRACE_FULL((Mgr,
            "  StatusByte.Field.ChecksumIsBad = No Change\n" ));
    }

    // If the battery status is being modified,
    // then update the RtcPowerIsBad bit
    if (  (BitMap & CMOS_BAD_BATTERY) != 0 )
    {
        // Read the legacy diagnostic status register
        CmosBankReadWrite(  Mgr, ReadType, CMOS_DIAGNOSTIC_STATUS_REGISTER,
                            &StatusByte.AllBits );

        if (AccessType == SetType){
            CMOS_TRACE_FULL((Mgr,
                "  StatusByte.Field.RtcPowerIsBad = 1\n" ));
            StatusByte.Field.RtcPowerIsBad = 1;
        }
        else if ( CanClearLegacyStatusBits(Mgr) ) {
            StatusByte.Field.RtcPowerIsBad = 0;
            CMOS_TRACE_FULL((Mgr,
                "  StatusByte.Field.RtcPowerIsBad = 0\n" ));
        }

        // Write the legacy diagnostic status register
        CmosBankReadWrite(  Mgr, WriteType, CMOS_DIAGNOSTIC_STATUS_REGISTER,
                            &StatusByte.AllBits );
    }
    else {
        CMOS_TRACE_FULL((Mgr,
            "  StatusByte.Field.RtcPowerIsBad = No Change\n" ));
    }

    CMOS_TRACE_FULL((Mgr,
        "SynchronizeLegacyStatusRegisters: Exit\n\n" ));

    return EFI_SUCCESS;
}


/**
    Computes a new "updated" CMOS checksum based on the change of value
    at a single address.

    @param Mgr          The Manger interface pointer.
    @param CmosAddress  CMOS location to be changed
    @param OldCmosValue Current value at the CMOS location
    @param NewCmosValue New value to be written to the CMOS location
    @param NewChecksum  Updated checksum

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or a valid EFI error code

    @note
      After this call, Mgr->CheckStatus(Mgr,
      CMOS_ADDRESS_IS_CHECKSUMMED) returns TRUE if the address "is"
      included in the checksum.

**/

EFI_STATUS CalculateUpdatedCheckSum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    IN  UINT16                      CmosAddress,
    IN  UINT8                       OldCmosValue,
    IN  UINT8                       NewCmosValue,
    OUT UINT16                      *NewChecksum )
{
    UINT16  UnmanagedIndex = 1;  // UnmanagedTable starts at 1
    UINT16  NoChecksumIndex = 1; // NoChecksumTable starts at 1

    // Check if the address is included in the NoChecksumTable
    // or the UnmanagedTable
    while ( NoChecksumIndex < Mgr->NoChecksumCount )
    {
        if ( CmosAddress <= Mgr->NoChecksumTable[NoChecksumIndex].Index )
            break;
        ++NoChecksumIndex;
    }

    while ( UnmanagedIndex < Mgr->UnmanagedTableCount )
    {
        if ( CmosAddress <= Mgr->UnmanagedTable[UnmanagedIndex].Index )
            break;
        ++UnmanagedIndex;
    }

    // Set ManagerStatus and compute NewChecksum if necessary
    if (    CmosAddress == Mgr->NoChecksumTable[NoChecksumIndex].Index
         || CmosAddress == Mgr->UnmanagedTable[UnmanagedIndex].Index     )
    {
#if (FULL_CMOS_MANAGER_DEBUG)
        if ( CmosAddress == Mgr->NoChecksumTable[NoChecksumIndex].Index ) {
            CMOS_TRACE((Mgr, "    ->Not Checksummed (0x%X)\n", CmosAddress ));
        }
        else {
            CMOS_TRACE((Mgr, "    ->Not Managed (0x%X)\n", CmosAddress ));
        }
#endif
        Mgr->ClearStatus(Mgr, CMOS_ADDRESS_IS_CHECKSUMMED);
    }
    else {
        ReadChecksum( Mgr, NewChecksum );
        *NewChecksum = *NewChecksum - OldCmosValue + NewCmosValue;
        Mgr->SetStatus(Mgr, CMOS_ADDRESS_IS_CHECKSUMMED);
    }

    return EFI_SUCCESS;
}


/**
    Calculate the checksum over the entire range of managed CMOS
    (standard and/or extended CMOS).

    @param Mgr           The Manger interface pointer.
    @param ChecksumValue The calculated checksum.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS CalculateChecksum(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr,
    OUT UINT16                      *ChecksumValue )
{
    //-----------------------------------------------------------------------
    // Algorithm Notes:
    //
    //  The NoChecksumTable is in numeric order and contains all CMOS
    //  registers starting at FIRST_CMOS_REGISTER and ending with
    //  LAST_CMOS_REGISTER unless the register has been explicitly specified
    //  with Checksum = YES in the NvramField declaration.
    //
    //  ...the NoChecksumTable need traversed only once.
    //-----------------------------------------------------------------------

    UINT16  CurRegister;
    UINT8   CurValue;
    UINT16  UnmanagedIndex = 1;  // UnmanagedTable starts at 1
    UINT16  NoChecksumIndex = 1; // NoChecksumTable starts at 1
    UINT16  CurrentChecksum;

    *ChecksumValue = 0; // ...just in case
    CMOS_TRACE_FULL((Mgr, "\n\nCalculateChecksum: Entry\n" ));

    // Calculate it
    for ( CurRegister = Mgr->FirstManagedRegister;
          CurRegister <= Mgr->LastManagedRegister;   // inclusive
          ++CurRegister )
    {
        //---------------------------------------------
        // Skip the location if the location is not managed
        //
        // Note: the UnmanagedTable is in numeric order,
        //       so it only needs traversed one time.
        //---------------------------------------------
        while ( (UnmanagedIndex < Mgr->UnmanagedTableCount)
                && (CurRegister
                    > Mgr->UnmanagedTable[UnmanagedIndex].Index) )
        {
            ++UnmanagedIndex;
        }
        if ( CurRegister == Mgr->UnmanagedTable[UnmanagedIndex].Index ){
            CMOS_TRACE_FULL((Mgr,
                "    -->0x%X (un-managed)\n", CurRegister));
            continue;
        }

        while ( (NoChecksumIndex < Mgr->NoChecksumCount)
                && (CurRegister
                        > Mgr->NoChecksumTable[NoChecksumIndex].Index) )
        {
            ++NoChecksumIndex;
        }

        if ( (CurRegister ==
                Mgr->NoChecksumTable[NoChecksumIndex].Index ) )
        {
            CMOS_TRACE_FULL((Mgr,
                "    -->0x%X (not checksummed)\n", CurRegister));
        }

        if ( (CurRegister !=
                Mgr->NoChecksumTable[NoChecksumIndex].Index ) )
        {
            CmosReadWrite(Mgr, ReadType, CurRegister, 0,
                &CurValue);
            *ChecksumValue += CurValue; // update the output parameter
        }
    }

    CMOS_TRACE_FULL((Mgr,
        "\n         --------------------------------\n"));
    CMOS_TRACE_FULL((Mgr,
        "         Computed *ChecksumValue = 0x%04X\n\n",
        *ChecksumValue ));

    // Compare with physical CMOS checksum value and set/clear status bit
    ReadChecksum( Mgr, &CurrentChecksum);

    CMOS_TRACE_FULL((Mgr,
        "\n         -----------------------\n"));
    CMOS_TRACE_FULL((Mgr,
        "         Saved Checksum = 0x%04X\n\n",
        CurrentChecksum ));

    if (*ChecksumValue != CurrentChecksum) {
        Mgr->SetStatus(Mgr, CMOS_BAD_CHECKSUM);
        CMOS_TRACE((Mgr, "    ...CMOS_BAD_CHECKSUM\n"));
    }
    else {
        Mgr->ClearStatus(Mgr, CMOS_BAD_CHECKSUM);
        CMOS_TRACE_FULL((Mgr, "    ...Checksum is OK\n" ));
    }

    CMOS_TRACE_FULL((Mgr, "CalculateChecksum: Done\n" ));

    return EFI_SUCCESS;
}


/**
    Writes from CmosValue to CMOS location encoded into CmosToken.


    @param Cmos      This is the access interface pointer.
    @param CmosToken This is the encoded CMOS location.
    @param CmosValue On success, this value will be written to
                     the specified CMOS location.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS if successful
        = EFI_NOT_FOUND, if the token is not in the token table
        = or other valid EFI error code

**/

EFI_STATUS CmosWrite(
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  UINT16                      CmosToken,
    IN  UINT8                       CmosValue )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    UINT16                          TokenTableIndex = 0;
    EFI_CMOS_MANAGER_INTERFACE      *Mgr = GetCmosMangerInterface(Cmos);

    // if CmosToken <= 0xe, then always write the register.  These registers
    // will be writable regardless of the state of the battery.
    if (CmosToken <= 0xe){
        Status = CmosBankReadWrite( Mgr, WriteType, CmosToken, &CmosValue );
    }

    // Otherwise, if CmosToken is less than MIN_TOKEN_VALUE and the battery
    // is good, then an additional test is made to disallow writing of actual
    // registers if they are within the managed region.  Otherwise, the
    // actual physical register is written. (For a bad battery, return
    // EFI_DEVICE_ERROR when trying to write the physical register.)
    else if ( CmosToken < MIN_TOKEN_VALUE ){
        if ( !Mgr->CheckStatus( Mgr, CMOS_IS_USABLE ) ) {
            Status = EFI_DEVICE_ERROR;
        }
        else if ( (CmosToken >= FIRST_MANAGED_CMOS_ADDRESS)
             && (CmosToken < MAX_MANAGED_CMOS_ADDRESS) )
        {
        	CMOS_TRACE_FULL((Mgr, "CmosWrite: CMOS Token out of range\n" ));
            Status = EFI_INVALID_PARAMETER;
        }
        else
            Status = CmosBankReadWrite( Mgr, WriteType,
                                        CmosToken, &CmosValue );
    }

    // Otherwise, after validating the CmosToken, decode it to write the
    // associated register bit(s).
    else if ((TokenTableIndex = CmosFindToken( Mgr, CmosToken )) > 0) {
        Status = CmosReadWriteEncodedToken( Mgr, WriteType, TokenTableIndex,
                                            &CmosValue );
    }
    else {
    	CMOS_TRACE_FULL((Mgr, "CmosWrite: CmosFindToken failed\n" ));
        Status = EFI_INVALID_PARAMETER;
    }

    return Status;
}


/**
    Initialize all physical CMOS registers with the default values
    specified in NvramField's Default value.

    @param Mgr The Manger interface pointer.

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

    @note
      LoadOptimalDefaults will be called for one of two reasons:

      1) On the first boot after flashing the ROM, the default
         values will be written to all CMOS registers, regardless
         of whether or not they are listed in the NoChecksumTable.

      2) If a bad checksum is reported in PEI, the
         OptimalDefaultTable will be used as a read/write buffer
         until DXE, where (by default) the CMOS_BAD_CHECKSUM status bit will
         signal to write the buffer to physical CMOS.

      * The optimal defaults buffer may be modified from the original
        version if CMOS_OPTIMAL_DEFAULTS_ENABLED status bit is set.

      * If the CMOS_BAD_CHECKSUM status is set, then only those values
        included in the checksum are written to physical CMOS.

**/

EFI_STATUS LoadOptimalDefaults(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Mgr )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT16      CurRegister;
    UINT8       CurValue;
    UINT16      NewChecksum = 0;
    UINT16      UnmanagedIndex = 1;     // UnmanagedTable starts at 1
    UINT16      DefaultIndex = 1;       // OptimalDefaultTable starts at 1
    UINT16      NoChecksumIndex = 1;    // NoChecksumTable starts at 1
    CMOS_MANAGER_STATUS  SavedStatus = Mgr->ManagerStatus &
                                    (CMOS_OPTIMAL_DEFAULTS_ENABLED );

    CMOS_TRACE_FULL((Mgr, "LoadOptimalDefaults Entry\n"));

    CMOS_TRACE_FULL((Mgr,
        "LoadOptimalDefaults (1) update global settings & statuses\n"));

    // Calculate it
    //-----------------------------------------------------------------------
    // Configure to ensure reads/writes are to physical registers.
    //-----------------------------------------------------------------------
    CMOS_TRACE_FULL((Mgr,"    force writing to physical registers\n"));
    Mgr->ClearStatus(Mgr, CMOS_OPTIMAL_DEFAULTS_ENABLED );

    // temporarily disallow checksum updating
    CMOS_TRACE_FULL((Mgr,"    disable checksum updating\n"));
    Mgr->SetStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);

    //---------------------------------------------
    // For the first boot, ignore the bad checksum
    // so that defaults are loaded for all
    // registers.
    //---------------------------------------------
    if ( Mgr->CheckStatus( Mgr, CMOS_FIRST_BOOT_DETECTED ) ){
        CMOS_TRACE_FULL((Mgr,
            "    CMOS_FIRST_BOOT_DETECTED ...clear bad checksum status\n"));
        Mgr->ClearStatus(Mgr, CMOS_BAD_CHECKSUM);
    }

    //-----------------------------------------------------------------------
    // Write the physical registers:
    //
    // There "should" be exactly one default entry for each register in the
    // managed region. (This is not assumed to be true.)
    //-----------------------------------------------------------------------
    CMOS_TRACE_FULL((Mgr,
        "LoadOptimalDefaults (2) begin loading defaults\n"));
    CMOS_TRACE_FULL((Mgr, "  Mgr->FirstManagedRegister=0x%X\n",
        Mgr->FirstManagedRegister));
    CMOS_TRACE_FULL((Mgr, "  Mgr->LastManagedRegister=0x%X\n",
        Mgr->LastManagedRegister));
    CMOS_TRACE_FULL((Mgr, "  Mgr->OptimalDefaultCount=0x%X\n",
        Mgr->OptimalDefaultCount));
    CMOS_TRACE_FULL((Mgr, "  Mgr->NoChecksumCount=0x%X\n",
        Mgr->NoChecksumCount));

    for ( CurRegister = Mgr->FirstManagedRegister;
          CurRegister <= Mgr->LastManagedRegister;   // inclusive
          ++CurRegister, ++DefaultIndex )
    {
        CMOS_TRACE_FULL((Mgr,
                "    CurRegister=0x%X, DefaultIndex=0x%X\n",
                CurRegister, DefaultIndex));

        // The normal exit condition is when the last default
        // register is loaded.
        if ( DefaultIndex >= Mgr->OptimalDefaultCount){
            CMOS_TRACE_FULL((Mgr,
                "    normal loop exit\n",
                CurRegister));
            break;
        }

        //---------------------------------------------
        // Skip the location if the location is not managed
        //
        // Note: the UnmanagedTable is in numeric order,
        //       so it only needs traversed one time.
        //---------------------------------------------
        while ( (UnmanagedIndex < Mgr->UnmanagedTableCount)
                && (CurRegister
                    > Mgr->UnmanagedTable[UnmanagedIndex].Index) )
        {
            CMOS_TRACE_FULL((Mgr, "    UnmanagedIndex=0x%X\n", UnmanagedIndex));
            ++UnmanagedIndex;
        }
        if ( CurRegister == Mgr->UnmanagedTable[UnmanagedIndex].Index ){

            // Ensure DefaultIndex is not incremented at the top of the
            // loop, as un-managed locations do not have a default value.
            CMOS_TRACE_FULL((Mgr, "    -->Skip 0x%X (un-managed)\n",
                CurRegister));
            --DefaultIndex;
            continue;
        }

        //---------------------------------------------
        // Skip location if:
        //
        //    1. there is a bad checksum,
        //    2. recovery is enabled only for checksummed locations
        //
        // (Note, the NoChecksumTable is in numeric order)
        //---------------------------------------------
        if ( Mgr->CheckStatus( Mgr,
                CMOS_BAD_CHECKSUM + CMOS_RECOVER_ONLY_CHECKSUMMED ) )
        {
            CMOS_TRACE_FULL((Mgr,
                "    NoChecksumIndex=0x%X\n",
                NoChecksumIndex));
            while ( (NoChecksumIndex < Mgr->NoChecksumCount)
                && (CurRegister
                    > Mgr->NoChecksumTable[NoChecksumIndex].Index) )
            {
                CMOS_TRACE_FULL((Mgr, "    NoChecksumIndex\n",
                    NoChecksumIndex));
                ++NoChecksumIndex;
            }
            if ( (CurRegister
                    == Mgr->NoChecksumTable[NoChecksumIndex].Index ) ){
                CMOS_TRACE_FULL((Mgr,
                    "    -->Skip 0x%X (not checksummed)\n",
                    CurRegister));
                continue;
            }
        }

        // Update the physical register
        CurValue = Mgr->OptimalDefaultTable[DefaultIndex].Value;
        Status = CmosReadWrite(Mgr, WriteType, CurRegister, ALL_BITS,
                                &CurValue);

        if (EFI_ERROR(Status)){
            CMOS_TRACE((Mgr,
                "    Error: unable to write to register: 0x%X\n",
                CurRegister ));
             Mgr->SetStatus(Mgr, CMOS_ERROR_LOADING_DEFAULTS);
             break;
        }
    }

    CMOS_TRACE_FULL((Mgr, "    done loading CMOS default values\n"));

    CMOS_TRACE_FULL((Mgr,
        "LoadOptimalDefaults (3) calculate & update checksum\n"));

    // if no error, calculate checksum and reset the
    // CMOS_BAD_CHECKSUM status bit.
    if (   EFI_ERROR(Status)
        || Mgr->CheckStatus(Mgr, CMOS_ERROR_LOADING_DEFAULTS))
    {
        CMOS_TRACE((Mgr,
            "    Error: Could not load Optimal Defaults\n"));
        Status = EFI_UNSUPPORTED;
    }
    else
    {
        CMOS_TRACE_FULL((Mgr,
            "    updating the checksum and clearing status\n"));
        CalculateChecksum(Mgr, &NewChecksum);
        WriteChecksum(Mgr, NewChecksum);
        Mgr->ClearStatus(Mgr, CMOS_BAD_CHECKSUM);
    }

    //-----------------------------------------------------------------------
    // Restore previous access routing settings.
    //-----------------------------------------------------------------------
   CMOS_TRACE_FULL((Mgr,
        "LoadOptimalDefaults (4) update global settings & statuses\n"));

    Mgr->ManagerStatus = Mgr->ManagerStatus | SavedStatus;

    // Allow checksums to be computed now
    Mgr->ClearStatus(Mgr, CMOS_FORCE_NO_CHECKSUM);

    // Set status bit to indicate that defaults have been loaded
    Mgr->SetStatus(Mgr, CMOS_DEFAULTS_LOADED);

    CMOS_TRACE_FULL((Mgr, "LoadOptimalDefaults Exit (%r)\n", Status));

    return Status;
}


/**
    Stops the time on RTC clock.


    @param Mgr The Manger interface pointer.

    @retval VOID

    @note  Here is the control flow of this function:
              1. Stop the time by setting bit 7 on RTC register 0xb.

**/

VOID StopTime(
    IN EFI_CMOS_MANAGER_INTERFACE  *Mgr )
{
    UINT8  Value;

    CmosBankReadWrite( Mgr, ReadType, 0xb, &Value );
    Value |= 0x80;
    CmosBankReadWrite( Mgr, WriteType, 0xb, &Value );
}


/**
    Start the time on RTC clock. This is used when changing the
    Date and Time

    @param Mgr The Manger interface pointer.

    @retval VOID

    @note  Here is the control flow of this function:
              1. Start the time by clearing bit 7 on RTC register 0xb.

**/

VOID StartTime(
    IN EFI_CMOS_MANAGER_INTERFACE  *Mgr )
{
    UINT8  Value;

    CmosBankReadWrite( Mgr, ReadType, 0xb, &Value );
    Value &= 0x7f;
    CmosBankReadWrite( Mgr, WriteType, 0xb, &Value );
}


/**
    Converts a Decimal value to a BCD value.

    @param Dec Decimal value

    @retval UINT8 (return value) - BCD value

    @note
      Only for 2 digit decimal.

**/

UINT8 DecToBCD(IN UINT8 Dec)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}


/**
    Converts a BCD value to a Decimal value.

    @param BCD BCD value

    @retval UINT8 decimal value

    @note
      Only for 2 digit BCD.

**/

UINT8 BCDToDec(IN UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;

    return SecondDigit * 10  + FirstDigit;
}


/**
    Read the RTC value at the given Index.

    @param Mgr   The Manger interface pointer.
    @param Index

    @retval UINT8 (return value) --
        = RTC Value read from the provided Index

    @note
      Here is the control flow of this function:
               1. Read port 0x70 (RTC Index Register) to get bit 7.
                  Bit 7 is the NMI bit-it should not be changed.
               2. Output 0x70 with the Index and NMI bit setting.
               3. Read 0x71 for Data. Getting Dec when appropriate.
               4. Return the Data.

**/

static UINT8 CmosMgrReadRtcIndex(
    IN EFI_CMOS_MANAGER_INTERFACE   *Mgr,
    IN UINT8                        Index )
{
    UINT8   Value;

    CmosBankReadWrite( Mgr, ReadType, Index, &Value );
    Value = BCDToDec(Value);

    return Value;
}


/**
    Write the RTC value at the given Index.


    @param Mgr   The Manger interface pointer.
    @param Index Index to write
    @param Value Value to write

    @retval VOID

    @note  Here is the control flow of this function:
              1. Read port 0x70 (RTC Index Register) to get bit 7.
                 Bit 7 is the NMI bit-it should not be changed.
              2. Output 0x70 with the Index. Switch to BCD when needed.
              3. Write the data to 0x71.

**/

static void CmosMgrWriteRtcIndex(
    IN EFI_CMOS_MANAGER_INTERFACE   *Mgr,
    IN UINT8                        Index,
    IN UINT8                        Value )
{
    Value = DecToBCD(Value);
    CmosBankReadWrite( Mgr, WriteType, Index, &Value );
}


/**
    This routine simply writes the CMOS RTC registers from information
    provided by the EFI_TIME structure, assuming the information is
    correct.


    @param Mgr  The Manger interface pointer.
    @param Time EFI Time structure

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

    @note
      Data is assumed to be valid.  Only a simple check for NULL pointer
      is done.

**/

EFI_STATUS CmosSetDateTime (
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    IN  EFI_TIME                    *Time )
{
    EFI_CMOS_MANAGER_INTERFACE      *Mgr = GetCmosMangerInterface(Cmos);

    if (Time == NULL || Mgr == NULL)
        return EFI_INVALID_PARAMETER;

    StopTime(Mgr);

    CmosMgrWriteRtcIndex( Mgr, ACPI_CENTURY_CMOS, Time->Year / 100 );
    CmosMgrWriteRtcIndex( Mgr, 9, Time->Year % 100 );
    CmosMgrWriteRtcIndex( Mgr, 8, Time->Month );
    CmosMgrWriteRtcIndex( Mgr, 7, Time->Day );
    CmosMgrWriteRtcIndex( Mgr, 4, Time->Hour );
    CmosMgrWriteRtcIndex( Mgr, 2, Time->Minute );
    CmosMgrWriteRtcIndex( Mgr, 0, Time->Second );

    StartTime(Mgr);

    return  EFI_SUCCESS;
}


/**
    Return the current date and time

    @param Mgr  The Manager interface pointer.
    @param Time EFI Time structure

    @retval EFI_STATUS (Return Value)
        = EFI_SUCCESS or valid EFI error code

**/

EFI_STATUS CmosGetDateTime (
    IN  EFI_CMOS_ACCESS_INTERFACE   *Cmos,
    OUT EFI_TIME                    *Time )
{
    EFI_CMOS_MANAGER_INTERFACE      *Mgr = GetCmosMangerInterface(Cmos);

    if (Time == NULL || Cmos == NULL)
        return EFI_INVALID_PARAMETER;

    //If RTC Year only 1 digit, EFI spec says years range is 1998 - 2097
    Time->Year      = CmosMgrReadRtcIndex(Mgr, ACPI_CENTURY_CMOS) * 100
                        + CmosMgrReadRtcIndex(Mgr, 9);
    Time->Month     = CmosMgrReadRtcIndex(Mgr, 8);
    Time->Day       = CmosMgrReadRtcIndex(Mgr, 7);
    Time->Hour      = CmosMgrReadRtcIndex(Mgr, 4);
    Time->Minute    = CmosMgrReadRtcIndex(Mgr, 2);
    Time->Second    = CmosMgrReadRtcIndex(Mgr, 0);
    Time->Nanosecond= 0;

    return  EFI_SUCCESS;
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
