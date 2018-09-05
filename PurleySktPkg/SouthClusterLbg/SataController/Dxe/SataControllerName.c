/** @file
  This portion is to register the Sata Controller Driver name:
  "SATA Controller Init Driver"

@copyright
 Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include "SataController.h"

///
/// Forward reference declaration
///
EFI_STATUS
EFIAPI
SataControllerGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL   *This,
  IN  CHAR8                         *Language,
  OUT CHAR16                        **DriverName
  );

EFI_STATUS
EFIAPI
SataControllerGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL   *This,
  IN  EFI_HANDLE                    ControllerHandle,
  IN  EFI_HANDLE                    ChildHandle        OPTIONAL,
  IN  CHAR8                         *Language,
  OUT CHAR16                        **ControllerName
  );

///
/// EFI Component Name Protocol
/// This portion declares a global variable of EFI_COMPONENT_NAME_PROTOCOL type.
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL     mSataControllerName = {
  SataControllerGetDriverName,
  SataControllerGetControllerName,
  "eng"
};

///
/// EFI Component Name Protocol
/// This portion declares a global variable of EFI_COMPONENT_NAME2_PROTOCOL type.
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL     mSataControllerName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) SataControllerGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) SataControllerGetControllerName,
  "en"
};

///
///  Define the Driver's unicode name string
///
static EFI_UNICODE_STRING_TABLE mSataControllerDriverNameTable[] = {
  {
    "eng;en",
    L"Intel(R) PCH Serial ATA Controller Initialization Driver"
  },
  {
    NULL,
    NULL
  }
};

static EFI_UNICODE_STRING_TABLE mSataControllerControllerNameTable[] = {
  {
    "eng;en",
    L"Intel(R) PCH Serial ATA Controller"
  },
  {
    NULL,
    NULL
  }
};

/**
  Retrieves a Unicode string that is the user readable name of the EFI Driver.

  @param[in] This                 A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in] Language             A pointer to a three character ISO 639-2 language identifier.
                                  This is the language of the driver name that that the caller
                                  is requesting, and it must match one of the languages specified
                                  in SupportedLanguages.  The number of languages supported by a
                                  driver is up to the driver writer.
  @param[out] DriverName          A pointer to the Unicode string to return.  This Unicode string
                                  is the name of the driver specified by This in the language
                                  specified by Language.

  @retval EFI_SUCCESS             The Unicode string for the Driver specified by This
                                  and the language specified by Language was returned
                                  in DriverName.
  @retval EFI_INVALID_PARAMETER   Language is NULL.
  @retval EFI_INVALID_PARAMETER   DriverName is NULL.
  @exception EFI_UNSUPPORTED      The driver specified by This does not support the
                                  language specified by Language.
**/
EFI_STATUS
EFIAPI
SataControllerGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL   *This,
  IN  CHAR8                         *Language,
  OUT CHAR16                        **DriverName
  )
{
  return LookupUnicodeString2 (
          Language,
          This->SupportedLanguages,
          mSataControllerDriverNameTable,
          DriverName,
          (BOOLEAN)(This == &mSataControllerName)
          );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by an EFI Driver.

  @param[in] This                 A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in] ControllerHandle     The handle of a controller that the driver specified by
                                  This is managing.  This handle specifies the controller
                                  whose name is to be returned.
  @param[in] ChildHandle          The handle of the child controller to retrieve the name
                                  of.  This is an optional parameter that may be NULL.  It
                                  will be NULL for device drivers.  It will also be NULL
                                  for a bus drivers that wish to retrieve the name of the
                                  bus controller.  It will not be NULL for a bus driver
                                  that wishes to retrieve the name of a child controller.
  @param[in] Language             A pointer to a three character ISO 639-2 language
                                  identifier.  This is the language of the controller name
                                  that that the caller is requesting, and it must match one
                                  of the languages specified in SupportedLanguages.  The
                                  number of languages supported by a driver is up to the
                                  driver writer.
  @param[out] ControllerName      A pointer to the Unicode string to return.  This Unicode
                                  string is the name of the controller specified by
                                  ControllerHandle and ChildHandle in the language
                                  specified by Language from the point of view of the
                                  driver specified by This.

  @retval EFI_SUCCESS             The Unicode string for the user readable name in the
                                  language specified by Language for the driver
                                  specified by This was returned in DriverName.
  @retval EFI_INVALID_PARAMETER   ControllerHandle is not a valid EFI_HANDLE.
                                  ChildHandle is not NULL and it is not a valid EFI_HANDLE.
  @retval EFI_INVALID_PARAMETER   Language is NULL.
                                  ControllerName is NULL.
  @exception EFI_UNSUPPORTED      The driver specified by This is not currently
                                  managing the controller specified by
                                  ControllerHandle and ChildHandle.
                                  The driver specified by This does not support the
                                  language specified by Language.
**/
EFI_STATUS
EFIAPI
SataControllerGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL   *This,
  IN  EFI_HANDLE                    ControllerHandle,
  IN  EFI_HANDLE                    ChildHandle        OPTIONAL,
  IN  CHAR8                         *Language,
  OUT CHAR16                        **ControllerName
  )
{
  EFI_STATUS                       Status;

  //
  // This is a device driver, so ChildHandle must be NULL.
  //
  if (ChildHandle != NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Make sure this driver is currently managing ControllHandle
  //
  Status = EfiTestManagedDevice (
             ControllerHandle,
             mSataControllerDriverBinding.DriverBindingHandle,
             &gEfiPciIoProtocolGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return LookupUnicodeString2 (
          Language,
          This->SupportedLanguages,
          mSataControllerControllerNameTable,
          ControllerName,
          (BOOLEAN)(This == &mSataControllerName)
          );
}
