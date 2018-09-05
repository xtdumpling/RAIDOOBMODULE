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


/** @file CmosTypes.h
    Contains the constants, data types and declarations
    necessary to support the CMOS manager module.

**/

#ifndef _CMOS_TYPES_H
#define _CMOS_TYPES_H

#include <Efi.h>

#define ALL_BITS            0xff    // bits to write mask


// For determination of boot mode
#define MONOTONIC_VARIABLE  L"MonotonicCounter"

#pragma pack(push, 1) // force byte alignment on all structures

typedef enum _CMOS_DEFAULTS_STATUS_TYPE {
    UsingDefaults = 0,
    NotUsingDefaults = 1,
} CMOS_DEFAULTS_STATUS_TYPE;

// Used in read/write functions

typedef enum _CMOS_BIT_ACCESS_TYPE {
    SetType = 1,
    ClearType = 0,
} CMOS_BIT_ACCESS_TYPE;

typedef UINT16 CMOS_CHECKSUM;


/**
    This structure type is used to define a generic CMOS byte that,
    among other things, can be used to define an array of CMOS addresses
    and values to be read or written.

 Fields: Type    Name                      Description
 ----------------------------------------------------------------
 UINT16  Register CMOS register associated with this byte
 UINT8   Value    Byte value associated with the specified register

    @note
      None

**/

typedef struct _CMOS_BYTE
{
    UINT16  Register;       // CMOS Register
    UINT8   Value;          // CMOS Value
}CMOS_BYTE;


/**
    This enumerated type is used to define a bitmap that represents the
    configuration status of the CMOS manager.  If a bit is set then
    the condition is TRUE.  If a bit is clear then the condition it
    represents is FALSE.

  Fields:  _ Bit  Condition
  ----------------------------------------------------------------
  _ CMOS_VALID_MANAGER                The manager is initialized
  _ CMOS_INTERFACE_ALREADY_INSTALLED  The access interface has already been installed
  _ CMOS_INSTALL_FAILED               The installation of the access interface failed
  _ CMOS_VALID_INTERFACE              The access interface is valid, but not necessarily installed
  _ CMOS_FIRST_BOOT_DETECTED          Indicates first boot (after progamming the boot device) is detected
  _ CMOS_EXECUTING_IN_MEMORY          The CMOS manager is executing in permanent memory
  _ CMOS_BAD_CHECKSUM                 The most recently computed checksum does not equal the saved checksum
  _ CMOS_ADDRESS_IS_CHECKSUMMED       The most recently accessed address is included in the checksum
  _ CMOS_ERROR_LOADING_DEFAULTS       CMOS could not be updated from the default optimal data table
  _ CMOS_FORCE_NO_CHECKSUM            Checksum updating is disabled
  _ CMOS_LEGACY_STATUS_ENABLED        Update legacy status registers in CMOS
  _ CMOS_BAD_BATTERY                  Bad battery has been detected
  _ CMOS_OPTIMAL_DEFAULTS_ENABLED     Optimal Defaults buffer is in use
  _ CMOS_DEFAULTS_LOADED              Optimal Defaults buffer has been written to CMOS
  _ CMOS_RECOVER_ONLY_CHECKSUMMED     Use Optimal Defaults buffer only for checksummed locations upon incoherency detection
  _ CMOS_COLD_BOOT_DETECTED           Cold boot was detected
  _ CMOS_BSP_IS_EXECUTING             BSP is executing
  _ CMOS_RECOVER_IN_PEI               PEI incoherency recovery (bad checksum & good battery)

**/

typedef enum _CMOS_MANAGER_STATUS {
    CMOS_VALID_MANAGER                  = BIT0,  // manager is initialized
    CMOS_INTERFACE_ALREADY_INSTALLED    = BIT1,  // duplicate interface found
    CMOS_INSTALL_FAILED                 = BIT2,  //
    CMOS_VALID_INTERFACE                = BIT3,  // not necessarily installed
    CMOS_FIRST_BOOT_DETECTED            = BIT4,
    CMOS_EXECUTING_IN_MEMORY            = BIT6,
    CMOS_BAD_CHECKSUM                   = BIT7,
    CMOS_ADDRESS_IS_CHECKSUMMED         = BIT8,
    CMOS_ERROR_LOADING_DEFAULTS         = BIT9,
    CMOS_FORCE_NO_CHECKSUM              = BIT10, // internal flag
    CMOS_BAD_BATTERY                    = BIT12,
    CMOS_OPTIMAL_DEFAULTS_ENABLED       = BIT13, // recovery enabled
    CMOS_DEFAULTS_LOADED                = BIT14,
    CMOS_RECOVER_ONLY_CHECKSUMMED       = BIT15, // limit recovery scope
    CMOS_COLD_BOOT_DETECTED             = BIT16, // Cold boot was detected
    CMOS_BSP_IS_EXECUTING               = BIT17, // BSP is executing
    CMOS_RECOVER_IN_PEI                 = BIT18, // PEI incoherency recovery
    CMOS_IS_USABLE                      = BIT19, // CMOS can be used
                                                 // (battery may be bad)
} CMOS_MANAGER_STATUS;


/**
    This enumerated type is used to define a bitmap that represents the
    requested configuration settings for the CMOS manager.  If a bit is
    set then the associated feature or setting is requested.

  Fields:  _ Bit  Requests
  ----------------------------------------------------------------

**/

typedef enum _CMOS_CONFIGURATION_SETTING {
    CMOS_OPTIMAL_DEFAULTS_ON            = BIT2,  // enable buffer usage
    CMOS_OPTIMAL_DEFAULTS_OFF           = BIT3,  // disable buffer usage
} CMOS_CONFIGURATION_SETTING;


// These are the PPI that may or may not be installed, depending on
// configuration settings
typedef enum _CMOS_PPI_TYPE {
    CMOS_DATA_HOB_INSTALLED_PPI_TYPE        = 0, // HOB is installed
    CMOS_ACCESS_PPI_TYPE                    = 1, // Access interface
    CMOS_CREATE_HOB_NOTIFY_PPI_TYPE         = 2, // In-memory callback
} CMOS_PPI_TYPE;

/**
    This structure type is used to define CMOS tokens, which are encoded
    values that define the usage of bits at a particular physical CMOS
    location.

 Fields: Type    Name                      Description
 ----------------------------------------------------------------
 UINT64  Name      A constant string pointer providing the name associated with the token (available only in debug mode)
 union   Value     Provides access to the encoded information in the token (see Notes below)

    @note
 The Value union provides access to all the bits of the token or to
 a specific token data field:

 Value.AllBits            All bits of the encoded token data
 Value.Field              Provides access to a specific token field
 Value.Field.BitOffset    Offset within the CMOS byte
 Value.Field.CmosAddress  The actual CMOS location
 Value.Field.Size         The number of bits consumed by the token

**/

typedef struct _CMOS_TOKEN
{
    union
    {
        UINT16  AllBits;
        struct  {
            UINT16  BitOffset   :3;  // [2:0] Bit offset (in the CMOS byte)
            UINT16  CmosAddress :9;  // [11:3] CMOS register address (0-511)
            UINT16  Size        :4;  // [15:12] Size (in bits)
        } Field;
    } Value;
}CMOS_TOKEN;

// Gets number of elements in an array of type CMOS_TOKEN
#define CMOS_TOKEN_COUNT(Buffer) ( sizeof(Buffer) / sizeof(CMOS_TOKEN) )


/**
    This union type is used to define un-encoded register
    values/numbers.

  Fields:  Type  Name Description
  ----------------------------------------------------------------
  UINT16  Value  The un-encoded value of a register
  UINT8   Index  The un-encoded register address

**/

typedef union _CMOS_REGISTER
{
    UINT8  Value;
    UINT16 Index;
}CMOS_REGISTER;

// Gets number of elements in an array of type CMOS_REGISTER
#define CMOS_REGISTER_COUNT(Buffer) ( sizeof(Buffer) / sizeof(CMOS_REGISTER) )

typedef EFI_STATUS (*EFI_CMOS_BOARD_READ_WRITE) (
    IN  EFI_PEI_SERVICES      **PeiServices,  // NULL in DXE phase
    IN  CMOS_ACCESS_TYPE      AccessType,
    IN  UINT16                CmosRegister,
    IN  UINT8                 *CmosParameterValue
);

typedef struct _CMOS_PORT_MAP
{
  UINT16                      Index;            // Index port
  UINT16                      Data;             // Data Port
  UINT16                      Low;              // First address in this range
  UINT16                      High;             // Last address in this range
  EFI_CMOS_BOARD_READ_WRITE   BoardReadWrite;

} CMOS_PORT_MAP;

typedef BOOLEAN (*EFI_CMOS_BATTERY_TEST) (
    IN  EFI_PEI_SERVICES      **PeiServices );

typedef BOOLEAN (*EFI_CMOS_IS_FIRST_BOOT) (
    IN  EFI_PEI_SERVICES        **PeiServices );

typedef BOOLEAN (*EFI_CMOS_IS_BSP) (
    IN  EFI_PEI_SERVICES        **PeiServices );

typedef BOOLEAN (*EFI_CMOS_IS_COLD_BOOT) (
    IN  EFI_PEI_SERVICES        **PeiServices );

typedef BOOLEAN (*EFI_CMOS_IS_CMOS_USABLE) (
    IN  EFI_PEI_SERVICES        **PeiServices );

#pragma pack(pop)

//----------------------------------------------------------------------------
// Debugging Macros
//----------------------------------------------------------------------------



#if ( defined(EFI_DEBUG) )

  #define CMOS_TRACE(args) CmosTraceCallout args;

  #if (FULL_CMOS_MANAGER_DEBUG)
     #define CMOS_TRACE_FULL(args) CmosTraceCallout args;
  #else
     #define CMOS_TRACE_FULL(args)
  #endif

#else
  #define CMOS_TRACE(args)
  #define CMOS_TRACE_FULL(args)

#endif

#endif  // #ifndef _CMOS_TYPES_H


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
