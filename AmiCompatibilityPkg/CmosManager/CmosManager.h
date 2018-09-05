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


/** @file CmosManager.h
    Contains the constants, data types and declarations
    necessary to support the CMOS manager interface.

**/


/**
    The page provides links to descriptions of the internal interfaces
    used by the CMOS Manager's subsystems.

 Fields:  Phase  Name  Description
 ------------------------------------------------------------------
 All  EFI_CMOS_MANAGER_INTERFACE      Provides general services

**/


#ifndef _CMOS_MANAGER_H
#define _CMOS_MANAGER_H

#include <Efi.h>
#include <CmosAccess.h>
#include "CmosManagerHob.h"
#include "CmosTypes.h"

typedef struct _EFI_CMOS_MANAGER_INTERFACE EFI_CMOS_MANAGER_INTERFACE;

typedef EFI_STATUS (*CMOS_MANAGER_DISPLAY_TOKEN_TABLE) (
    IN EFI_CMOS_MANAGER_INTERFACE       *Manager );

typedef EFI_CMOS_ACCESS_INTERFACE * (*CMOS_MANAGER_GET_ACCESS_INTERFACE) (
    IN EFI_CMOS_MANAGER_INTERFACE       *Manager );

typedef EFI_STATUS (*CMOS_MANAGER_LOCATE_ACCESS_INTERFACE) (
	IN  EFI_CMOS_MANAGER_INTERFACE   *Manager,
	OUT EFI_CMOS_ACCESS_INTERFACE    **AccessInterface );

typedef EFI_STATUS  (*CMOS_MANAGER_INSTALL_ACCESS_INTERFACE) (
    IN EFI_CMOS_MANAGER_INTERFACE       *Manager );

typedef EFI_STATUS (*CMOS_MANAGER_CALCULATE_CHECKSUM)(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    OUT UINT16                      *ChecksumValue );

typedef EFI_STATUS (*CMOS_MANAGER_READ_CHECKSUM)(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    OUT UINT16                      *ChecksumValue );

typedef EFI_STATUS (*CMOS_MANAGER_WRITE_CHECKSUM)(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    OUT UINT16                      ChecksumValue );

typedef EFI_STATUS (*CMOS_MANAGER_CONFIGURE) (
    IN EFI_CMOS_MANAGER_INTERFACE       *Manager,
    IN CMOS_CONFIGURATION_SETTING       Setting );

typedef VOID (*CMOS_MANAGER_SET_STATUS)(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap );

typedef VOID (*CMOS_MANAGER_CLEAR_STATUS)(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap );

typedef BOOLEAN (*CMOS_MANAGER_CHECK_STATUS)(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap );

typedef BOOLEAN (*CMOS_MANAGER_CHECK_ANY_STATUS)(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN CMOS_MANAGER_STATUS          BitMap );

typedef EFI_STATUS (*CMOS_MANAGER_LOAD_OPTIMAL_DEFAULTS)(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager );

typedef EFI_STATUS (*CMOS_MANAGER_SAVE_API_POINTER_TO_CMOS)(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN EFI_CMOS_ACCESS_INTERFACE    *ApiPointer  );

typedef EFI_STATUS (*CMOS_MANAGER_GET_API_POINTER_FROM_CMOS)(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    IN EFI_CMOS_ACCESS_INTERFACE    **ApiPointer  );

typedef EFI_STATUS (*CMOS_MANAGER_INITIALIZE_CALLOUT)(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager );

typedef EFI_STATUS (*CMOS_MANAGER_ALLOCATE_POOL)(
    IN  EFI_CMOS_MANAGER_INTERFACE  *Manager,
    IN  UINTN                       Size,
    OUT VOID                        **Buffer );


/**
    This is the internal interface used by subsystems within the CMOS
    manager module.

  Fields:  Type  Name  Description
      EFI_CMOS_ACCESS_INTERFACE               Access                      The external interface
      CMOS_MANAGER_INSTALL_ACCESS_INTERFACE   InstallAccessInterface      Installs the Access PPI or Protocol, depending on the phase (see InstallCmosAccessInterface)
      CMOS_MANAGER_GET_ACCESS_INTERFACE       GetAccessInterface          Used internally to get a pointer to the Access interface (see GetCmosAccessInterface)
      EFI_GUID                                AccessGuid                  The Access PPI (or Protocol) GUID
      EFI_PEI_PPI_DESCRIPTOR                  Ppi[4]                      Descriptors for the PEIMs installed by this module
      UINT8                                   FirstManagedRegister        First register in the managed space
      UINT8                                   LastManagedRegister         Last register in the managed space
      CMOS_TOKEN                              *TokenTable                 All defined tokens
      UINT16                                  TokenCount                  Entry count for TokenTable (#Tokens + 1)
      CMOS_REGISTER                           *OptimalDefaultTable        Default values for all managed CMOS addresses
      UINT16                                  OptimalDefaultCount         Entry count for OptimalDefaultTable
      CMOS_REGISTER                           *NoChecksumTable            CMOS address of locations not included in the checksum in the managed area
      UINT16                                  NoChecksumCount             Entry count for NoChecksumTable
      CMOS_MANAGER_CONFIGURE                  ConfigureManager            Provides means for specifying configurable settings for CMOS manager during PEI/DXE initialization (see CmosConfigureManager)
      CMOS_CHECKSUM                           Checksum                    Valid checksum after the call to NewCmosManagerInterface()
      CMOS_MANAGER_CALCULATE_CHECKSUM         CalculateChecksum           Forces calculation of the checksum over the entire managed region
      CMOS_MANAGER_READ_CHECKSUM              ReadChecksum                Reads checksum from the CMOS checksum location
      CMOS_MANAGER_WRITE_CHECKSUM             WriteChecksum               Writes the provided checksum to the CMOS checksum location
      CMOS_MANAGER_LOAD_OPTIMAL_DEFAULTS      LoadOptimalDefaults         Writes default values from the OptimalDefaultTable to all CMOS registers
      CMOS_MANAGER_STATUS                     ManagerStatus               Bitmap containing various Manager-related status values
      CMOS_MANAGER_SET_STATUS                 SetStatus                   Sets bits in the ManagerStatus value (see CmosManagerSetStatus)
      CMOS_MANAGER_CLEAR_STATUS               ClearStatus                 Clears bits in the ManagerStatus value (see CmosManagerClearStatus)
      CMOS_MANAGER_CHECK_STATUS               CheckStatus                 Returns true if all of the specified bits are "set" in the ManagerStatus value (see CmosManagerCheckStatus)
      CMOS_MANAGER_CHECK_ANY_STATUS           CheckAnyStatus              Returns true if any of the specified bits are "set" in the ManagerStatus value (see CmosManagerCheckAnyStatus)
      CMOS_MANAGER_HOB                        *ManagerHob                 Pointer to the HOB defined after permanent memory detection in PEI
      CMOS_MANAGER_SAVE_API_POINTER_TO_CMOS   SaveApiPointerToCmos        Pointer to the Access interface is maintained in a 4-byte CMOS location
      CMOS_MANAGER_GET_API_POINTER_FROM_CMOS  GetApiPointerFromCmos       Retrieve pointer to the Access interface from 4-byte CMOS location

**/

#pragma pack(push, 1)           // force byte alignment
typedef struct _EFI_CMOS_MANAGER_INTERFACE
{
    // Note: EFI_CMOS_ACCESS_INTERFACE must be first in this structure
    EFI_CMOS_ACCESS_INTERFACE               Access;                 // Access
    CMOS_MANAGER_INSTALL_ACCESS_INTERFACE   InstallAccessInterface;
    CMOS_MANAGER_GET_ACCESS_INTERFACE       GetAccessInterface;
    CMOS_MANAGER_LOCATE_ACCESS_INTERFACE    LocateAccessInterface;
    EFI_GUID                                AccessGuid;
    EFI_PEI_PPI_DESCRIPTOR                  Ppi[4];
    UINT16                                  FirstManagedRegister;
    UINT16                                  LastManagedRegister;
    CMOS_TOKEN                              *TokenTable;            // Tables
    UINT16                                  TokenCount;
    CMOS_REGISTER                           *OptimalDefaultTable;
    UINT16                                  OptimalDefaultCount;
    CMOS_REGISTER                           *NoChecksumTable;
    UINT16                                  NoChecksumCount;
    CMOS_REGISTER                           *UnmanagedTable;
    UINT16                                  UnmanagedTableCount;
    CMOS_MANAGER_CONFIGURE                  ConfigureManager;       // Manager
    CMOS_CHECKSUM                           Checksum;
    CMOS_MANAGER_CALCULATE_CHECKSUM         CalculateChecksum;
    CMOS_MANAGER_READ_CHECKSUM              ReadChecksum;
    CMOS_MANAGER_WRITE_CHECKSUM             WriteChecksum;
    CMOS_MANAGER_LOAD_OPTIMAL_DEFAULTS      LoadOptimalDefaults;
    CMOS_MANAGER_STATUS                     ManagerStatus;
    CMOS_MANAGER_SET_STATUS                 SetStatus;
    CMOS_MANAGER_CLEAR_STATUS               ClearStatus;
    CMOS_MANAGER_CHECK_STATUS               CheckStatus;
    CMOS_MANAGER_CHECK_ANY_STATUS           CheckAnyStatus;
    CMOS_MANAGER_HOB                        *ManagerHob;
    CMOS_MANAGER_SAVE_API_POINTER_TO_CMOS   SaveApiPointerToCmos;
    CMOS_MANAGER_GET_API_POINTER_FROM_CMOS  GetApiPointerFromCmos;
    CMOS_MANAGER_INITIALIZE_CALLOUT			InitailizeCallout;
    EFI_CMOS_IS_FIRST_BOOT        			*IsFirstBoot;
    EFI_CMOS_IS_COLD_BOOT         			*IsColdBoot;
    EFI_CMOS_IS_BSP               			*IsBsp;
    EFI_CMOS_IS_CMOS_USABLE       			*CmosIsUsable;
    EFI_CMOS_BATTERY_TEST                   *CmosBatteryIsGood;
    CMOS_PORT_MAP  							*CmosBank;
    UINT16         							CmosBankCount;
};
#pragma pack(pop)           // force byte alignment

EFI_CMOS_MANAGER_INTERFACE* NewCmosManagerInterface(
    EFI_PEI_SERVICES                **PeiServices );

VOID CmosTraceCallout(
    IN EFI_CMOS_MANAGER_INTERFACE   *Manager,
    CHAR8                           *Format,
    ... );

#endif  // #ifndef _CMOS_MANAGER_H


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
