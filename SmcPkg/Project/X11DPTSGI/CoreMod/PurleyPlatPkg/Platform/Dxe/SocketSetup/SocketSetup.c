//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  Hide Processor 1 Information for UP Platform.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Jan/03/2017
//
//  Rev. 1.05
//    Bug Fix:  Fix that Processor Information doesn't show correctly.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Dec/22/2016
//
//  Rev. 1.04
//    Bug Fix:  Make memory information string shorter in setup menu.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/05/2016
//
//  Rev. 1.03
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//  Rev. 1.02
//    Bug Fixed:  Fixed CPU information show in wrong socket number under setup menu issue.
//    Reason:     
//    Auditor:    Jacker Yeh
//    Date:       Jul/12/2016
//
//  Rev. 1.01
//    Bug Fixed:  Support SMC Memory map-out function.
//    Reason:     
//    Auditor:    Ivern Yeh
//    Date:       Jul/07/2016
//
//    Rev. 1.00
//    Reason:	Add riser card name to IIO root port.
//    Auditor:	Kasber Chen
//    Date:	06/21/2016
//
//**************************************************************************//
//
// Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
// This software and associated documentation (if any) is furnished
// under a license and may only be used or copied in accordance
// with the terms of the license. Except as permitted by such
// license, no part of this software or documentation may be
// reproduced, stored in a retrieval system, or transmitted in any
// form or by any means without the express written consent of
// Intel Corporation.
//

/**
  @file ErrorManagerSetup.c

  ErrorManagerSetup driver to initialize Error Manager setup screen


**/
#include "SocketSetup.h"
#include <BackCompatible.h>
#include <IncludePrivate/Library/PchAlternateAccessModeLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Use GUID for TSE.
#include <Guid/HiiSetupEnter.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Use GUID for TSE.

#if SMCPKG_SUPPORT
#include <SspTokens.h>
#include <GpioConfig.h>
#include <Library\GpioLib.h>
#endif
//
// ToDo: SV_OTHER_CONFIGURATION and SV_PCH_CONFIGURATION should not really be a part of the socket package, and should be removed,
//       but a few of the Setup choices are dependent on it, so for now it's included. We can remove it
//       once the offending variables are moved to a socket structure.
//
SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;
SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;
//
// static Socket Structures
//
SOCKET_IIO_CONFIGURATION       mIioConfig;
SOCKET_COMMONRC_CONFIGURATION  mCommonRcConfig;
SOCKET_MP_LINK_CONFIGURATION   mCsiConfig;
SOCKET_MEMORY_CONFIGURATION    mMemoryConfig;

CPU_SETUP_INFO                mCpuSetupInfo[MAX_SOCKET];
EFI_IIO_UDS_PROTOCOL          *mIioUds;

#define SOCKET_SETUP_SIGNATURE SIGNATURE_32 ('S', 'V', 'O', 'S')

extern EFI_GUID gSocketPkgListGuid;
EFI_STRING_ID MemTypeStrIdTab [] = {
  (STRING_REF) STR_MEMORY_TRAINING_DISPLAY_FORM_HELP,
  (STRING_REF) STR_ENFORCE_POR_HELP,
};
#define MEMORY_TYPE_DDR_STR L"DDR4"

typedef struct {
  UINT16  MfgId;
  CHAR16  String[16];
} MEMORY_MODULE_MANUFACTURER_LIST;
UINT8 ChannelStr[MAX_CH] = {0x41,0x42,0x43,0x44,0x45,0x46}; // ASCII values for A, B, C, D, E, F (DDR Channel Silk screen)

MEMORY_MODULE_MANUFACTURER_LIST MemoryModuleManufacturerList[] =
{
#if 1	// SMCPKG_SUPPORT
  {0xF103, L"Innodisk"},
  {0x1603, L"Netlist"},
  {0x3400, L"Agiga"},
  {0x4001, L"Viking"},
#endif
  {0x2c00, L"Micron"},
  {0xce00, L"Samsung"},
  {0x4f01, L"Transcend"},
  {0x9801, L"Kinston"},
  {0xfe02, L"Elpida"},
  {0xad00, L"Hynix"},
  {0xc100, L"Infineon"},
  {0x9401, L"Smart"},
  {0x5705, L"Aeneon"},
  {0x5105, L"Qimonda"}
};

typedef struct _INTEL_SOCKET_SETUP_INSTANCE {
  UINTN                            Signature;
  EFI_HANDLE                       ImageHandle;
  EFI_HANDLE                       DeviceHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  VOID                            *StartOpCodeHandle;
  VOID                            *EndOpCodeHandle;

  EFI_HII_HANDLE                   HiiHandle;

} INTEL_SOCKET_SETUP_INSTANCE;

INTEL_SOCKET_SETUP_INSTANCE      mPrivateData;
EFI_HII_CONFIG_ROUTING_PROTOCOL  *mHiiConfigRouting = NULL;
EFI_HII_HANDLE                   mSocketStringsHiiHandle = NULL;

typedef struct {
  EFI_GUID     *Guid;
  UINTN        BufferSize;
  CHAR16       *VariableName;
} SOCKET_VARIABLE_INFO;

VOID    *SocketPtrs[] = {
    &(mIioConfig           ),
    &(mCommonRcConfig      ),
    &(mCsiConfig           ),
    &(mMemoryConfig        ),
    &(mSocketPowermanagementConfiguration),
    &(mSocketProcessorCoreConfiguration)
};

SOCKET_VARIABLE_INFO SocketVariableInfo [] = {
  {&gEfiSocketIioVariableGuid     ,sizeof (SOCKET_IIO_CONFIGURATION)     , (CHAR16 *)(SOCKET_IIO_CONFIGURATION_NAME     )},
  {&gEfiSocketCommonRcVariableGuid,sizeof (SOCKET_COMMONRC_CONFIGURATION), (CHAR16 *)(SOCKET_COMMONRC_CONFIGURATION_NAME)},
  {&gEfiSocketMpLinkVariableGuid  ,sizeof (SOCKET_MP_LINK_CONFIGURATION) , (CHAR16 *)(SOCKET_MP_LINK_CONFIGURATION_NAME )},
  {&gEfiSocketMemoryVariableGuid  ,sizeof (SOCKET_MEMORY_CONFIGURATION)  , (CHAR16 *)(SOCKET_MEMORY_CONFIGURATION_NAME  )},
  {&gEfiSocketPowermanagementVarGuid ,sizeof (SOCKET_POWERMANAGEMENT_CONFIGURATION) , (CHAR16 *)(SOCKET_POWERMANAGEMENT_CONFIGURATION_NAME)},
  {&gEfiSocketProcessorCoreVarGuid   ,sizeof (SOCKET_PROCESSORCORE_CONFIGURATION)   , (CHAR16 *)(SOCKET_PROCESSORCORE_CONFIGURATION_NAME)}
};
#define MaxSocketVarInfo sizeof(SocketVariableInfo) / sizeof(SocketVariableInfo[0])

typedef struct {
  VENDOR_DEVICE_PATH        Node;
  EFI_DEVICE_PATH_PROTOCOL  End;
} HII_VENDOR_DEVICE_PATH;

HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    SOCKET_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};
// APTIOV_SERVER_OVERRIDE_RC_START
//extern UINT8  SocketSetupFormsBin[];
// APTIOV_SERVER_OVERRIDE_RC_END
//SGI+
BOOLEAN  IsA1B1Node(
VOID
)
{

   BOOLEAN             A1B1Node = FALSE;
   UINT32             Value = 0;
//    DEBUG ((DEBUG_INFO, "PeiProjectSystemConfiguration.\n"));
   DEBUG((EFI_D_INFO, "Check IsA1B1Node ent .\n"));

   GpioGetInputValue(SGI_NODE_1_PIN, &Value);
   //Check GPP_C_12     1 : A1/B1 side  
    if (Value) {
        //Disable NMI2SMI_EN for prevent NMI trigger again before service.
        DEBUG((EFI_D_INFO, "IsA1B1Node .\n"));
        A1B1Node = TRUE;
    }  

    return A1B1Node;
}
//SGI-
/**

    GC_TODO: add routine description

    @param This     - GC_TODO: add arg description
    @param Request  - GC_TODO: add arg description
    @param Progress - GC_TODO: add arg description
    @param Results  - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
DefaultExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  UINTN                            Size;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Index=0;


  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;

  if (Request != NULL) {
    for (Index = 0; Index < MaxSocketVarInfo; Index ++) {
      if (HiiIsConfigHdrMatch (Request, SocketVariableInfo[Index].Guid, SocketVariableInfo[Index].VariableName)){
        break;
      }
    }

    if (Index == MaxSocketVarInfo)
      return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  Size             = 0;
  AllocatedRequest = FALSE;

  //
  // Get Buffer Storage data from EFI variable
  //
  BufferSize = SocketVariableInfo[Index].BufferSize;

  Status = gRT->GetVariable (
                  SocketVariableInfo[Index].VariableName,
                  SocketVariableInfo[Index].Guid,
                  NULL,
                  &BufferSize,
                  SocketPtrs[Index]
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ConfigRequest = Request;
  if (Request == NULL || (StrStr (Request, L"OFFSET") == NULL)) {

  //
    // Request is set to NULL, construct full request string.
    //

    //
    // First Set ConfigRequestHdr string.
    //
    ConfigRequestHdr = HiiConstructConfigHdr (SocketVariableInfo[Index].Guid, SocketVariableInfo[Index].VariableName,  mPrivateData.DeviceHandle);
    if (ConfigRequestHdr == NULL) {
      DEBUG ((EFI_D_ERROR, "\n ERROR!!! HiiConstructConfigHdr () - Returned NULL Pointer for ConfigRequestHdr\n"));
      ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
      return EFI_INVALID_PARAMETER;
    }

    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    if (ConfigRequest == NULL) {
      DEBUG ((EFI_D_ERROR, "\n ERROR!!! AllocateZeroPool () - Returned NULL Pointer for ConfigRequest\n"));
      ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
      return EFI_OUT_OF_RESOURCES;
    }

    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = mHiiConfigRouting->BlockToConfig (
                                mHiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) SocketPtrs[Index],
                                BufferSize,
                                Results,
                                Progress
                                );

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = (EFI_STRING)(Request + StrLen (Request));
  }

  return Status;
}
//

/**

    GC_TODO: add routine description

    @param This          - GC_TODO: add arg description
    @param Configuration - GC_TODO: add arg description
    @param Progress      - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
DefaultRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS          Status;
  UINTN               BufferSize;
  UINTN               Index;
  UINT32              Attributes;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  for (Index = 0; Index < MaxSocketVarInfo; Index ++) {
    if (HiiIsConfigHdrMatch (Configuration, SocketVariableInfo[Index].Guid, SocketVariableInfo[Index].VariableName)){
      break;
    }
  }


  if (Index == MaxSocketVarInfo)
    return EFI_NOT_FOUND;


  //
  // Get Buffer Storage data from EFI variable
  //
  BufferSize = SocketVariableInfo[Index].BufferSize;
  Status = gRT->GetVariable (
                  SocketVariableInfo[Index].VariableName,
                  SocketVariableInfo[Index].Guid,
                  &Attributes,
                  &BufferSize,
                  SocketPtrs[Index]
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = SocketVariableInfo[Index].BufferSize;
  Status = mHiiConfigRouting->ConfigToBlock (
                               mHiiConfigRouting,
                               Configuration,
                               (UINT8 *) SocketPtrs[Index],
                               &BufferSize,
                               Progress
                               );


  //
  // Store Buffer Storage back to EFI variable
  //

  Status = gRT->SetVariable (
                  SocketVariableInfo[Index].VariableName,
                  SocketVariableInfo[Index].Guid,
                  Attributes,
                  SocketVariableInfo[Index].BufferSize,
                  SocketPtrs[Index]
                  );

  return Status;
}


/**

    GC_TODO: add routine description

    @param This       - GC_TODO: add arg description
    @param Action     - GC_TODO: add arg description
    @param QuestionId - GC_TODO: add arg description
    @param Type       - GC_TODO: add arg description
    @param Value      - GC_TODO: add arg description
    @param OPTIONAL   - GC_TODO: add arg description

    @retval Status - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
CallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  )
{
   EFI_STATUS                     Status = EFI_SUCCESS;
   // APTIOV_SERVER_OVERRIDE_RC_START : Fix to F3: Load Defaults causing long delays in setup
#if 0
   UINT16                         Index;
   EFI_STRING                     ConfigRequestHdr;
   BOOLEAN                        ActionFlag;
#endif
    // APTIOV_SERVER_OVERRIDE_RC_END : Fix to F3: Load Defaults causing long delays in setup
   SOCKET_MEMORY_CONFIGURATION    SocketMemoryConfiguration;
   SOCKET_MP_LINK_CONFIGURATION   CsiConfig;

  switch(Action)
  {
  case EFI_BROWSER_ACTION_FORM_CLOSE:

     break;
  case EFI_BROWSER_ACTION_FORM_OPEN:
     break;
  case EFI_BROWSER_ACTION_CHANGING:

   break;
  case EFI_BROWSER_ACTION_CHANGED:
    switch (QuestionId) {
      case KEY_CPU_ONLINE:
          HiiGetBrowserData (
            &gEfiSocketMpLinkVariableGuid,
            SOCKET_MP_LINK_CONFIGURATION_NAME,
            sizeof(SOCKET_MP_LINK_CONFIGURATION),
            (UINT8*)&CsiConfig
            );
          CsiConfig.QpiCpuSktHotPlugEn = Value->u8;
          HiiSetBrowserData(
            &gEfiSocketMpLinkVariableGuid,
            SOCKET_MP_LINK_CONFIGURATION_NAME,
            sizeof(SOCKET_MP_LINK_CONFIGURATION),
            (UINT8*)&CsiConfig,
            NULL
            );

          HiiGetBrowserData (
            &gEfiSocketMemoryVariableGuid,
            SOCKET_MEMORY_CONFIGURATION_NAME,
            sizeof(SOCKET_MEMORY_CONFIGURATION),
            (UINT8*)&SocketMemoryConfiguration
            );
          SocketMemoryConfiguration.Srat = Value->u8;
          SocketMemoryConfiguration.SratCpuHotPlug= Value->u8;
          SocketMemoryConfiguration.SratMemoryHotPlug= Value->u8;
          HiiSetBrowserData(
            &gEfiSocketMemoryVariableGuid,
            SOCKET_MEMORY_CONFIGURATION_NAME,
            sizeof(SOCKET_MEMORY_CONFIGURATION),
            (UINT8*)&SocketMemoryConfiguration,
            NULL
            );
        break;
      default:
        break;
    }
    break;
  case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
  // APTIOV_SERVER_OVERRIDE_RC_START : Fix to F3: Load Defaults causing long delays in setup
#if 0
    for (Index = 0; Index < MaxSocketVarInfo; Index ++) {
      ConfigRequestHdr = HiiConstructConfigHdr (SocketVariableInfo[Index].Guid,  SocketVariableInfo[Index].VariableName,  mPrivateData.DeviceHandle);
      ASSERT (ConfigRequestHdr != NULL);
      ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_STANDARD);
      ASSERT (ActionFlag);
    }
#endif
    Status = EFI_UNSUPPORTED;
    // APTIOV_SERVER_OVERRIDE_RC_END : Fix to F3: Load Defaults causing long delays in setup
    
      break;

  case EFI_BROWSER_ACTION_RETRIEVE:
      break;

  default:
    //
    // Do nothing for other UEFI Action.
    //
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
BOOLEAN
IsCMOSBad(
     VOID
)
{
  UINT8           Nmi;
  volatile UINT32 Data32; 
  Nmi     = 0;
  Data32  = 0;
  //
  // Preserve NMI bit setting
  //
  PchAlternateAccessMode (TRUE);
  Nmi     = (IoRead8 (R_IOPORT_CMOS_STANDARD_INDEX) & 0x80);
  PchAlternateAccessMode (FALSE);

  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, R_IOPORT_CMOS_IDX_DIAGNOSTIC_STATUS | Nmi);
  if (IoRead8 (R_IOPORT_CMOS_STANDARD_DATA) & (BIT6 + BIT7)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
clearCmos0E()
{
  UINT8           Value;
  UINT8           Nmi;
  volatile UINT32 Data32; 
  Nmi     = 0;
  Data32  = 0;
  //
  // Preserve NMI bit setting
  //
  PchAlternateAccessMode (TRUE);
  Nmi     = (IoRead8 (R_IOPORT_CMOS_STANDARD_INDEX) & 0x80);
  PchAlternateAccessMode (FALSE);

  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, R_IOPORT_CMOS_IDX_DIAGNOSTIC_STATUS | Nmi);
  Value = IoRead8 (R_IOPORT_CMOS_STANDARD_DATA);
  IoWrite8 (R_IOPORT_CMOS_STANDARD_DATA, Value & 0x3F);
}

/**

  Acquire the string associated with the Index from SMBIOS structure and return it.
  The caller is responsible for freeing the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS if successful.
  @retval   EFI_NOT_FOUND     The function returns EFI_NOT_FOUND if unsuccessful.

**/
EFI_STATUS
SmbiosGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  )
{
  UINTN          StrSize;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart); // size includes null terminator
  // SMBIOS strings are NULL terminated, and end of all strings is indicated with NULL
  // loop until at end of all SMBIOS strings (found NULL terminator at string index past string's NULL), and Index != 0
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero
    return EFI_INVALID_PARAMETER;
  } else {
    AsciiStrToUnicodeStr (OptionalStrStart, *String);
  }

  return EFI_SUCCESS;
}

/**

  Find the SMBIOS structure with the specified Type and Handle.
  If Type is NULL then only Handle will be matched.

  @param    Handle            The Handle to search for.
  @param    Type              The SMBIOS Type to search for.
  @param    Record            The SMBIOS record if a match is found

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS if successful.
  @retval   EFI_NOT_FOUND     The function returns EFI_NOT_FOUND if unsuccessful.

**/
EFI_STATUS
SmbiosGetStructure (
  IN      UINT16  Handle,
  IN      EFI_SMBIOS_TYPE         Type,
  OUT     EFI_SMBIOS_TABLE_HEADER **Record
  )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  SMBIOS_HEADER                     *SmbiosHeader;
  EFI_STATUS                        Status;

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = 0;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &Type, Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    SmbiosHeader = (SMBIOS_HEADER *) *Record;
    if (SmbiosHeader->Handle == Handle) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
GetCpuInformation (
  VOID
  )
{
  UINT32                       ApicId, SocketId;
  UINT32                       RegEax, RegEbx, RegEcx, RegEdx;
  UINT32                       SocketIdShift, SocketIdMask, i;

  // Determine socket ID
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  ApicId = (RegEbx >> 24);
  SocketIdShift = 5;

  AsmCpuid(EFI_CPUID_SIGNATURE, &RegEax, NULL, NULL, NULL);
  if(RegEax >= 0xB) {
    //is X2 Capable Processor, get ApicId from leaf B
    AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 0, NULL, NULL, NULL, &RegEdx);
    ApicId = RegEdx;

    // How many bits to shift to get Socket ID?
    AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 1, &RegEax, NULL, NULL, NULL);
    SocketIdShift = RegEax & B_EFI_CPUID_CORE_TOPOLOGY_SHIFT_MASK;
  }

  SocketId = ApicId >> SocketIdShift;

  // SocketIdMask is SocketIdShift number of bits.
  SocketIdMask = 0;
  for(i = 0; i < SocketIdShift; i++) {
    SocketIdMask = (SocketIdMask << 1) | 1;
  }

  //
  // Get microcode revision from primary thread in the socket
  //
  if((ApicId & SocketIdMask) == 0x00) {
    mCpuSetupInfo[SocketId].Ucode = (UINT32) RShiftU64 (AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID), 32);
  }
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
UpdateCpuInformation (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 StrSize;
  EFI_STRING_ID                         TokenToUpdate;
  CHAR16                                *NewString;
  CHAR16                                *VersionString[MAX_SOCKET];
  UINTN                                 SocketIndex;
  UINTN                                 BspPackageNumber;
  UINT32                                BspCpuidSignature;
  UINT8                                 StrIndex;
  EFI_SMBIOS_HANDLE                     SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                   *Smbios;
  SMBIOS_TABLE_TYPE4                    *SmbiosType4Record;
  SMBIOS_TABLE_TYPE7                    *SmbiosType7Record;
  EFI_SMBIOS_TYPE                       SmbiosType;
  EFI_SMBIOS_TABLE_HEADER               *SmbiosRecord;
  EFI_SMBIOS_TABLE_HEADER               *SmbiosRecord2;
  UINTN                                 NaLen;
  UINT32                                SocketState;
  UINT32                                RegEax, RegEbx, RegEcx, RegEdx;
  CHAR16                                *NotAvailable;
  EFI_MP_SERVICES_PROTOCOL              *mMpService;

  StrSize      = NEWSTRING_SIZE;
  NewString    = AllocateZeroPool (StrSize);
  NotAvailable = L"   N/A   ";
  NaLen        = StrLen (NotAvailable) * 2;
  ZeroMem (mCpuSetupInfo, sizeof (CPU_SETUP_INFO) * MAX_SOCKET);
  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    VersionString[SocketIndex] = AllocateZeroPool (StrSize);
  }

  //
  // Get BSP CPU ID
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, &RegEbx, &RegEcx, &RegEdx);
  BspCpuidSignature = RegEax;

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    SmbiosType4Record = (SMBIOS_TABLE_TYPE4 *) SmbiosRecord;
    StrIndex = SmbiosType4Record->Socket;
    if (SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &NewString) == EFI_SUCCESS) {
      // string should be 'CPUxx'
      if (StrnCmp(NewString, L"CPU", 3) != 0) continue;

#if SMCPKG_SUPPORT
      SocketIndex = StrDecimalToUintn(NewString + StrLen(L"CPU")) -1;
      if(SocketIndex>8) SocketIndex = 0; // For UP's CPU Description is "CPU", Patch SocketIndex to 0
#else
      SocketIndex = StrDecimalToUintn(NewString + StrLen(L"CPU"));
#endif
      // if Status [6] is set then socket is populated
      if (SocketIndex >= 0 && SocketIndex < MAX_SOCKET && (SmbiosType4Record->Status & 0x40)) {
        mCpuSetupInfo[SocketIndex].Id = *(UINT32 *) &SmbiosType4Record->ProcessorId.Signature;
        mCpuSetupInfo[SocketIndex].Freq = SmbiosType4Record->CurrentSpeed;
        mCpuSetupInfo[SocketIndex].MaxRatio = (UINT32) mIioUds->IioUdsPtr->PlatformData.MaxCoreToBusRatio[SocketIndex];
        mCpuSetupInfo[SocketIndex].MinRatio = (UINT32) mIioUds->IioUdsPtr->PlatformData.MinCoreToBusRatio[SocketIndex];
        //Populate the Setup structure for use in overclockinginterface driver
        mSocketPowermanagementConfiguration.MaxEfficiencyRatio[SocketIndex] = (UINT32) mIioUds->IioUdsPtr->PlatformData.MinCoreToBusRatio[SocketIndex];
        mSocketPowermanagementConfiguration.MaxNonTurboRatio[SocketIndex] = (UINT32) mIioUds->IioUdsPtr->PlatformData.MaxCoreToBusRatio[SocketIndex];

        if (SmbiosGetStructure (SmbiosType4Record->L1CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
          SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
          if ((SmbiosType7Record->CacheConfiguration & 0x7) == 0) { // [2:0] is Cache Level; verify it matches
            // [15] indicates granularity; 0=1 KB, 1=64 KB
            // set cache size in bytes by setting the value to 1024*size
            mCpuSetupInfo[SocketIndex].L1Cache = (SmbiosType7Record->InstalledSize / SmbiosType4Record->CoreCount) & 0x7FFF;
            // if granularity is 64 KB, then multiply by 64
            if (SmbiosType7Record->InstalledSize & 0x8000) {
              mCpuSetupInfo[SocketIndex].L1Cache *= 64;
            }
          }
        }

        if (SmbiosGetStructure (SmbiosType4Record->L2CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
          SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
          if ((SmbiosType7Record->CacheConfiguration & 0x7) == 1) { // [2:0] is Cache Level; verify it matches
            // [15] indicates granularity; 0=1 KB, 1=64 KB
            // set cache size in bytes by setting the value to 1024*size
            mCpuSetupInfo[SocketIndex].L2Cache = (SmbiosType7Record->InstalledSize / SmbiosType4Record->CoreCount) & 0x7FFF;
            // if granularity is 64 KB, then multiply by 64
            if (SmbiosType7Record->InstalledSize & 0x8000) {
              mCpuSetupInfo[SocketIndex].L2Cache *= 64;
            }
          }
        }

        if (SmbiosGetStructure (SmbiosType4Record->L3CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
          SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
          if ((SmbiosType7Record->CacheConfiguration & 0x7) == 2) { // [2:0] is Cache Level; verify it matches
            // [15] indicates granularity; 0=1 KB, 1=64 KB
            // set cache size in bytes by setting the value to 1024*size
            mCpuSetupInfo[SocketIndex].L3Cache = SmbiosType7Record->InstalledSize & 0x7FFF;
            // if granularity is 64 KB, then multiply by 64
            if (SmbiosType7Record->InstalledSize & 0x8000) {
              mCpuSetupInfo[SocketIndex].L3Cache *= 64;
            }
          }
        }

        StrIndex = SmbiosType4Record->ProcessorVersion;
        if (SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &VersionString[SocketIndex]) == EFI_SUCCESS) {
          mCpuSetupInfo[SocketIndex].Version = VersionString[SocketIndex];
        }
      }
    }
  }

 Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &mMpService);
  if(!EFI_ERROR(Status)) {
    //
    // Get Microcode version from each socket
    //
    GetCpuInformation();
    mMpService->StartupAllAPs (
           mMpService,
           (EFI_AP_PROCEDURE)GetCpuInformation,
           FALSE,
           NULL,
           0,
           NULL,
           NULL
           );
  } else {
    //
    // If MP Protocol not found, fill in BSP info anyway.
    //
    GetCpuInformation();
  }

  BspPackageNumber  = 0;

  if (mCpuSetupInfo[0].Version) {
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET0_VALUE2;
    StrCpyS (NewString, NEWSTRING_SIZE/sizeof(CHAR16), mCpuSetupInfo[0].Version);
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString + 23, NULL);
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET0_VALUE;
    NewString[23] = L'\0';
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);
  }
#if !(SMCPKG_SUPPORT && NUMBER_CPU_SOCKETS == 1)
  if (mCpuSetupInfo[1].Version) {
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET1_VALUE2;
    StrCpyS (NewString, NEWSTRING_SIZE/sizeof(CHAR16), mCpuSetupInfo[1].Version);
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString + 23, NULL);
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET1_VALUE;
    NewString[23] = L'\0';
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);
  }
#endif
  if (mCpuSetupInfo[2].Version) {
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET2_VALUE2;
    StrCpyS (NewString, NEWSTRING_SIZE/sizeof(CHAR16), mCpuSetupInfo[2].Version);
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString + 23, NULL);
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET2_VALUE;
    NewString[23] = L'\0';
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);
  }

  if (mCpuSetupInfo[3].Version) {
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET3_VALUE2;
    StrCpyS (NewString, NEWSTRING_SIZE/sizeof(CHAR16), mCpuSetupInfo[3].Version);
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString + 23, NULL);
    TokenToUpdate = STR_PROCESSOR_VERSION_SOCKET3_VALUE;
    NewString[23] = L'\0';
    HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);
  }

#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  TokenToUpdate = (STRING_REF) STR_PROCESSOR_SOCKET_VALUE;
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, L"Socket 0", NULL);
#endif

  SocketState = 0;
  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    SocketState += (mCpuSetupInfo[SocketIndex].Id ? 0 : 1 << SocketIndex);
  }

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_ID_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x%c",
    (UINTN)mCpuSetupInfo[0].Id,
    BspPackageNumber ? ' ' : '*'
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x%c |  %08x%c",
    (UINTN)mCpuSetupInfo[0].Id,
    BspPackageNumber ? ' ' : '*',
    (UINTN)mCpuSetupInfo[1].Id,
    BspPackageNumber ? '*' : ' '
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 2) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_ID_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x%c |  %08x%c",
    (UINTN)mCpuSetupInfo[2].Id,
    BspPackageNumber ? ' ' : '*',
    (UINTN)mCpuSetupInfo[3].Id,
    BspPackageNumber ? '*' : ' '
    );
  if (SocketState & 4) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 8) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_FREQ_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%1d.%03dGHz",
    (UINTN) (mCpuSetupInfo[0].Freq / 1000),
    (UINTN) (mCpuSetupInfo[0].Freq % 1000)
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%1d.%03dGHz  |  %1d.%03dGHz",
    (UINTN) (mCpuSetupInfo[0].Freq / 1000),
    (UINTN) (mCpuSetupInfo[0].Freq % 1000),
    (UINTN) (mCpuSetupInfo[1].Freq / 1000),
    (UINTN) (mCpuSetupInfo[1].Freq % 1000)
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 2) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_FREQ_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%1d.%03dGHz  |  %1d.%03dGHz",
    (UINTN) (mCpuSetupInfo[2].Freq / 1000),
    (UINTN) (mCpuSetupInfo[2].Freq % 1000),
    (UINTN) (mCpuSetupInfo[3].Freq / 1000),
    (UINTN) (mCpuSetupInfo[3].Freq % 1000)
    );
  if (SocketState & 4) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 8) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_MAX_RATIO_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
   // APTIOV_SERVER_OVERRIDE_RC_START
    L"     %02xH", 
   // APTIOV_SERVER_OVERRIDE_RC_END
    (UINTN) (mCpuSetupInfo[0].MaxRatio)
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
   // APTIOV_SERVER_OVERRIDE_RC_START
    L"     %02xH  |  %02xH", 
   // APTIOV_SERVER_OVERRIDE_RC_END
    (UINTN) (mCpuSetupInfo[0].MaxRatio),
    (UINTN) (mCpuSetupInfo[1].MaxRatio)
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 2) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_MAX_RATIO_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
   // APTIOV_SERVER_OVERRIDE_RC_START
    L"     %02xH  |  %02xH", 
   // APTIOV_SERVER_OVERRIDE_RC_END
    (UINTN) (mCpuSetupInfo[2].MaxRatio),
    (UINTN) (mCpuSetupInfo[3].MaxRatio)
    );
  if (SocketState & 4) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 8) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_MIN_RATIO_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
   // APTIOV_SERVER_OVERRIDE_RC_START
    L"     %02xH", 
   // APTIOV_SERVER_OVERRIDE_RC_END
    (UINTN) (mCpuSetupInfo[0].MinRatio)
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
   // APTIOV_SERVER_OVERRIDE_RC_START
    L"     %02xH  |  %02xH", 
   // APTIOV_SERVER_OVERRIDE_RC_END
    (UINTN) (mCpuSetupInfo[0].MinRatio),
    (UINTN) (mCpuSetupInfo[1].MinRatio)
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 2) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_MIN_RATIO_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
   // APTIOV_SERVER_OVERRIDE_RC_START
    L"     %02xH  |  %02xH", 
   // APTIOV_SERVER_OVERRIDE_RC_END
    (UINTN) (mCpuSetupInfo[2].MinRatio),
    (UINTN) (mCpuSetupInfo[3].MinRatio)
    );
  if (SocketState & 4) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 8) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_MICROCODE_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x",
    (UINTN)mCpuSetupInfo[0].Ucode
    );
  if (mCpuSetupInfo[0].Ucode == 0) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x  |  %08x",
    (UINTN)mCpuSetupInfo[0].Ucode,
    (UINTN)mCpuSetupInfo[1].Ucode
    );
  if (mCpuSetupInfo[0].Ucode == 0) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (mCpuSetupInfo[1].Ucode == 0) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_MICROCODE_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"%08x  |  %08x",
    (UINTN)mCpuSetupInfo[2].Ucode,
    (UINTN)mCpuSetupInfo[3].Ucode
    );
  if (mCpuSetupInfo[2].Ucode == 0) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (mCpuSetupInfo[3].Ucode == 0) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_L1_CACHE_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB",
    (UINTN)mCpuSetupInfo[0].L1Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB  |    %4dKB",
    (UINTN)mCpuSetupInfo[0].L1Cache,
    (UINTN)mCpuSetupInfo[1].L1Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 2) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_L1_CACHE_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB  |    %4dKB",
    (UINTN)mCpuSetupInfo[2].L1Cache,
    (UINTN)mCpuSetupInfo[3].L1Cache
    );
  if (SocketState & 4) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 8) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_L2_CACHE_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB",
    (UINTN)mCpuSetupInfo[0].L2Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB  |    %4dKB",
    (UINTN)mCpuSetupInfo[0].L2Cache,
    (UINTN)mCpuSetupInfo[1].L2Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 2) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_L2_CACHE_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L"  %4dKB  |    %4dKB",
    (UINTN)mCpuSetupInfo[2].L2Cache,
    (UINTN)mCpuSetupInfo[3].L2Cache
    );
  if (SocketState & 4) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 8) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR_L3_CACHE_VALUE;
#if SMCPKG_SUPPORT==1 && NUMBER_CPU_SOCKETS==1
  UnicodeSPrint (
    NewString,
    StrSize,
    L" %5dKB",
    (UINTN)mCpuSetupInfo[0].L3Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
#else
  UnicodeSPrint (
    NewString,
    StrSize,
    L" %5dKB  |   %5dKB",
    (UINTN)mCpuSetupInfo[0].L3Cache,
    (UINTN)mCpuSetupInfo[1].L3Cache
    );
  if (SocketState & 1) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 2) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
#endif
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  TokenToUpdate = (STRING_REF) STR_PROCESSOR2_3_L3_CACHE_VALUE;
  UnicodeSPrint (
    NewString,
    StrSize,
    L" %5dKB  |   %5dKB",
    (UINTN)mCpuSetupInfo[2].L3Cache,
    (UINTN)mCpuSetupInfo[3].L3Cache
    );
  if (SocketState & 4) {
    CopyMem (&NewString[0], NotAvailable, NaLen);
  }
  if (SocketState & 8) {
    CopyMem (&NewString[13], NotAvailable, NaLen);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Display Processor Revision in the Main page
  //
  TokenToUpdate = STR_CPU_BSP_REVISION_VALUE;
  switch (BspCpuidSignature) {
    case ((CPU_FAMILY_SKX << 4) + A0_CPU_STEP):
      UnicodeSPrint (NewString, StrSize, L"%5X - SKX A0", (UINTN)mCpuSetupInfo[BspPackageNumber].Id);
      break;
    case ((CPU_FAMILY_SKX << 4) + A2_CPU_STEP):
      UnicodeSPrint (NewString, StrSize, L"%5X - SKX A2", (UINTN)mCpuSetupInfo[BspPackageNumber].Id);
      break;
    case ((CPU_FAMILY_SKX << 4) + B0_CPU_STEP):
      UnicodeSPrint (NewString, StrSize, L"%5X - SKX B0", (UINTN)mCpuSetupInfo[BspPackageNumber].Id);
      break;
    case ((CPU_FAMILY_SKX << 4) + B1_CPU_STEP):
      UnicodeSPrint (NewString, StrSize, L"%5X - SKX B1", (UINTN)mCpuSetupInfo[BspPackageNumber].Id);
      break;
    case ((CPU_FAMILY_SKX << 4) + H0_CPU_STEP):
      UnicodeSPrint (NewString, StrSize, L"%5X - SKX H0", (UINTN)mCpuSetupInfo[BspPackageNumber].Id);
      break;
    default:
      UnicodeSPrint (NewString, StrSize, L"%5X - Unknown", (UINTN)mCpuSetupInfo[BspPackageNumber].Id);
  }
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Display Microcode Revision in the Main page
  //
  TokenToUpdate = STR_PROCESSOR_MICROCODE_VALUE;
  UnicodeSPrint (NewString, StrSize, L"%08X", mCpuSetupInfo[0].Ucode);
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  gBS->FreePool (NewString);
  for (SocketIndex = 0; SocketIndex < MAX_SOCKET; SocketIndex++) {
    gBS->FreePool (VersionString[SocketIndex]);
  }
  return;
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
UpdateMemType (
  VOID
)
{
  EFI_STATUS                              Status;
  EFI_SMBIOS_HANDLE                       SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                    *Smbios;
  EFI_SMBIOS_TYPE                         SmbiosType;
  EFI_STRING                              AdvancedString;
  EFI_STRING                              MemoryTypeString;
  SMBIOS_TABLE_TYPE17                    *SmbiosType17Record;
  CHAR16                                  MemoryTypeChar;
  UINTN                                   i;

  // Get memory type from SMBIOS
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  Status = Smbios->GetNext (
                    Smbios,
                    &SmbiosHandle,
                    &SmbiosType,
                    (EFI_SMBIOS_TABLE_HEADER **) (&SmbiosType17Record),
                    NULL);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Can not find SMBIOS information.\n"));
    goto Done;
  }

  //DEBUG((EFI_D_INFO, "SmbiosType17Record->MemoryType:0x%x\n", SmbiosType17Record->MemoryType));

  switch (SmbiosType17Record->MemoryType) {
    case MemoryTypeDdr3:
      MemoryTypeChar = L'3';
      break;
    default:
      MemoryTypeChar = L'4';
      break;
  }

  //DEBUG((EFI_D_INFO, "Memory type is DDR%c\n", MemoryTypeChar));

  // Get string from Advanced menu.
  for (i = 0; i < sizeof (MemTypeStrIdTab) / sizeof (EFI_STRING_ID); ++i) {
    AdvancedString = HiiGetString (mSocketStringsHiiHandle, MemTypeStrIdTab [i], NULL);
    if (NULL == AdvancedString) {
      DEBUG((EFI_D_ERROR, "Can not found memory type string.\n"));
      continue;
    }

    // Search the DDR# string
    MemoryTypeString = StrStr (AdvancedString, MEMORY_TYPE_DDR_STR);
    if (MemoryTypeString != NULL) {
      MemoryTypeString [3] = MemoryTypeChar;
      HiiSetString(mSocketStringsHiiHandle, MemTypeStrIdTab [i], AdvancedString, NULL);
      //DEBUG((EFI_D_INFO, "Updated memory type string:%s. \n", AdvancedString));
    }

    FreePool (AdvancedString);
    AdvancedString = NULL;
  }
Done:
  return;
}

/**

  This is the function for displaying memory topology in Setup.

  @param VOID.

  @retval VOID.

**/
VOID
DisplayMemTopology (
  VOID
  )
{
  struct      SystemMemoryMapHob *SystemMemoryMap;
  UINT8       Socket, Channel, Dimm;
  UINT8       MfdSize,MfdIndex,Cntr;
  UINT16      DimmSize;
  CHAR16      *DimmString1, *DimmString2;
 // APTIOV_SERVER_OVERRIDE_RC_START : Update MemoryTopology with extra DIMM details
  UINT32      Index = 0;  
 // APTIOV_SERVER_OVERRIDE_RC_END : Update MemoryTopology with extra DIMM details
  UINTN       StrSize;
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
  BOOLEAN     FailDimmExist = FALSE;
#endif
  VOID        *HobList;
  EFI_STATUS  Status;
  STRING_REF  TokenToUpdate;
  EFI_HOB_GUID_TYPE   *GuidHob;
  STRING_REF            MemTopology[] = {
       STRING_TOKEN(STR_MEM_STRING1), STRING_TOKEN(STR_MEM_STRING2),STRING_TOKEN(STR_MEM_STRING3),STRING_TOKEN(STR_MEM_STRING4),
       STRING_TOKEN(STR_MEM_STRING5), STRING_TOKEN(STR_MEM_STRING6),STRING_TOKEN(STR_MEM_STRING7),STRING_TOKEN(STR_MEM_STRING8),
       STRING_TOKEN(STR_MEM_STRING9), STRING_TOKEN(STR_MEM_STRING10),STRING_TOKEN(STR_MEM_STRING11),STRING_TOKEN(STR_MEM_STRING12),
       STRING_TOKEN(STR_MEM_STRING13), STRING_TOKEN(STR_MEM_STRING14),STRING_TOKEN(STR_MEM_STRING15),STRING_TOKEN(STR_MEM_STRING16),
       STRING_TOKEN(STR_MEM_STRING17), STRING_TOKEN(STR_MEM_STRING18),STRING_TOKEN(STR_MEM_STRING19),STRING_TOKEN(STR_MEM_STRING20),
       STRING_TOKEN(STR_MEM_STRING21), STRING_TOKEN(STR_MEM_STRING22),STRING_TOKEN(STR_MEM_STRING23),STRING_TOKEN(STR_MEM_STRING24),
       STRING_TOKEN(STR_MEM_STRING25), STRING_TOKEN(STR_MEM_STRING26),STRING_TOKEN(STR_MEM_STRING27),STRING_TOKEN(STR_MEM_STRING28),
       STRING_TOKEN(STR_MEM_STRING29), STRING_TOKEN(STR_MEM_STRING30),STRING_TOKEN(STR_MEM_STRING31),STRING_TOKEN(STR_MEM_STRING32),
       STRING_TOKEN(STR_MEM_STRING33), STRING_TOKEN(STR_MEM_STRING34),STRING_TOKEN(STR_MEM_STRING35),STRING_TOKEN(STR_MEM_STRING36),
       STRING_TOKEN(STR_MEM_STRING37), STRING_TOKEN(STR_MEM_STRING38),STRING_TOKEN(STR_MEM_STRING39),STRING_TOKEN(STR_MEM_STRING40),
       STRING_TOKEN(STR_MEM_STRING41), STRING_TOKEN(STR_MEM_STRING42),STRING_TOKEN(STR_MEM_STRING43),STRING_TOKEN(STR_MEM_STRING44),
       STRING_TOKEN(STR_MEM_STRING45), STRING_TOKEN(STR_MEM_STRING46),STRING_TOKEN(STR_MEM_STRING47),STRING_TOKEN(STR_MEM_STRING48)
   };
  //
  // Search for the Memory Map GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
  if (GuidHob != NULL){
     SystemMemoryMap = (struct SystemMemoryMapHob*) GET_GUID_HOB_DATA (GuidHob);
  } else {
    SystemMemoryMap = NULL;
    Status = EFI_DEVICE_ERROR;
    ASSERT_EFI_ERROR (Status);
    return;
  }

  StrSize   = 0x250;
  DimmString1 = AllocateZeroPool (StrSize);
  Cntr = 0;

  if (DimmString1 == NULL) {
    DEBUG ((EFI_D_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocateZeroPool() returned NULL pointer.\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return;
  }

  DimmString2 = AllocateZeroPool (StrSize);
  if (DimmString2 == NULL) {
    DEBUG ((EFI_D_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocateZeroPool() returned NULL pointer.\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    gBS->FreePool (DimmString1);
    return;
  }

  MfdSize = sizeof (MemoryModuleManufacturerList) / sizeof (MemoryModuleManufacturerList[0]);

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Channel = 0; Channel < MAX_CH; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {    
      // APTIOV_SERVER_OVERRIDE_RC_START : Update MemoryTopology with extra DIMM details
        if (/*SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].Enabled == 0
              ||*/ SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].Present == 0 ) {
     // APTIOV_SERVER_OVERRIDE_RC_END : Update MemoryTopology with extra DIMM details
            UnicodeSPrint (DimmString1,StrSize,L"");
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
          if((SystemMemoryMap->MemSetup.socket[Socket].ddrCh[Channel].dimmList[Dimm].Disable == 0x01) && Dimm < SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].MaxDimm) {
          //SGI+
            //UnicodeSPrint (DimmString1, StrSize, L"P%d-DIMM%c%d: Mapped Out", Socket, ChannelStr[Channel], Dimm + 1);
            if( IsA1B1Node() ) {    
              UnicodeSPrint (DimmString1, StrSize, L"A1-P%d-D%d: Mapped Out", Socket+1, Channel+1);
            } else {
              UnicodeSPrint (DimmString1, StrSize, L"A0-P%d-D%d: Mapped Out", Socket+1, Channel+1);  
            } 
         //SGI-   
            FailDimmExist = TRUE;
          }

#endif
        } else {
             // APTIOV_SERVER_OVERRIDE_RC_START : Update MemoryTopology with extra DIMM details
       //SGI+
            // UnicodeSPrint (DimmString1,StrSize,L"Socket%d.Ch%c.Dimm%d:  ",Socket, ChannelStr[Channel], Dimm);
               if( IsA1B1Node() ) {
                 UnicodeSPrint (DimmString1,StrSize,L"A1-P%d-D%d:  ",Socket+1,Channel+1);
               } 
                else {
                 UnicodeSPrint (DimmString1,StrSize,L"A0-P%d-D%d:  ",Socket+1,Channel+1);
               }
      //SGI-         
            // Support for additional details on DIMM based on following:
            // 1. Dimm Present and Enabled 2. DIMM present but Disabled. 3. Channel is not enabled but Dimm is present
            if ( SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].Enabled == 1 ) {
#if SMCPKG_SUPPORT == 0
                 UnicodeSPrint (DimmString2,StrSize,L"DIMM is Present. DIMM Status:Enabled ");
                 StrCat (DimmString1, DimmString2 );
#endif
            } else if ( SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].Enabled == 0 ){
#if SMCPKG_SUPPORT == 0
                 UnicodeSPrint (DimmString2,StrSize,L"DIMM is Present. DIMM Status:Disabled ");
                 StrCat (DimmString1, DimmString2 );
#endif
              // Warning and Error major and minor codes are logged in host->var.common.status
              for (Index = 0; Index < SystemMemoryMap->status.index; Index++) {
                if( (((SystemMemoryMap->status.log[Index].data >> 24) & 0xFF) == Socket) && 
                  (((SystemMemoryMap->status.log[Index].data >> 16) & 0xFF) == Channel) &&
                  (((SystemMemoryMap->status.log[Index].data >> 8) & 0xFF) == Dimm) ) {
                     UnicodeSPrint(DimmString2,StrSize,L"- Major Code = 0x%X Minor Code = 0x%X ",((SystemMemoryMap->status.log[Index].code >> 8) & 0xFF), (SystemMemoryMap->status.log[Index].code & 0xFF));
                     StrCat (DimmString1, DimmString2 );
                     break;
                }
              }
              if (Index == SystemMemoryMap->status.index )  {
                  // No Major/Minor code found – mark it as Invalid code.
                  UnicodeSPrint(DimmString2,StrSize,L"- Invalid Major/Minor code ");
                  StrCat (DimmString1, DimmString2 );  
              }
            }
           // APTIOV_SERVER_OVERRIDE_RC_END : Update MemoryTopology with extra DIMM details

          for( MfdIndex = 0; MfdIndex < MfdSize; MfdIndex++) {
            if(MemoryModuleManufacturerList[MfdIndex].MfgId == SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].SPDMMfgId )
              break;
          }
          if (MfdIndex < MfdSize ) {
              // APTIOV_SERVER_OVERRIDE_RC_START
#if SMCPKG_SUPPORT
              UnicodeSPrint (DimmString2,StrSize,L"%dMT/s %s ",
#else
              UnicodeSPrint (DimmString2,StrSize,L" DIMM Details: %dMT/s %s ",
#endif
                           SystemMemoryMap->memFreq,MemoryModuleManufacturerList[MfdIndex].String);
          } else {
#if SMCPKG_SUPPORT
          UnicodeSPrint (DimmString2,StrSize,L"%dMT/s UNKNOWN ",
#else
              UnicodeSPrint (DimmString2,StrSize,L" DIMM Details: %dMT/s UNKNOWN ",
#endif
                           SystemMemoryMap->memFreq);
              // APTIOV_SERVER_OVERRIDE_RC_END
          }
        // APTIOV_SERVER_OVERRIDE_RC_START : Update MemoryTopology with extra DIMM details
          StrCat (DimmString1, DimmString2 );
       // APTIOV_SERVER_OVERRIDE_RC_END : Update MemoryTopology with extra DIMM details
          DimmSize = SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].DimmSize >> 4;
          if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].NumRanks == 1) {
            if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].X4Present)
              UnicodeSPrint(DimmString2,StrSize, L"SRx4 %dGB", DimmSize);
            else
              UnicodeSPrint(DimmString2,StrSize, L"SRx8 %dGB", DimmSize);
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].NumRanks == 2) {
            if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].X4Present)
              UnicodeSPrint(DimmString2,StrSize, L"DRx4 %dGB", DimmSize);
            else
              UnicodeSPrint(DimmString2,StrSize, L"DRx8 %dGB", DimmSize);
          } else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].NumRanks == 4) {
            if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].X4Present)
              UnicodeSPrint(DimmString2,StrSize, L"QRx4 %dGB", DimmSize);
            else
              UnicodeSPrint(DimmString2,StrSize, L"QRx8 %dGB", DimmSize);
          }

          StrCatS(DimmString1, StrSize/sizeof(CHAR16), DimmString2);
          if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].keyByte == SPD_TYPE_DDR4) {
            switch (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].actKeyByte2) {
            case SPD_RDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" RDIMM");
              break;
            case SPD_UDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" UDIMM");
              break;
            case SPD_SODIMM:
              UnicodeSPrint(DimmString2,StrSize, L" SODIMM");
              break;
            //case SPD_MICRO_DIMM:
            //  UnicodeSPrint(DimmString2,StrSize, L" MICRO-DIMM");
            //  break;
            case SPD_MINI_RDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" MINI-RDIMM");
              break;
            case SPD_MINI_UDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" MINI-UDIMM");
              break;
            case SPD_MINI_CDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" MINI-CDIMM");
              break;
            case SPD_ECC_SO_UDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" ECC-SO-UDIMM");
              break;
            case SPD_ECC_SO_RDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" ECC-SO-RDIMM");
              break;
            case SPD_ECC_SO_CDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" ECC-SO-CDIMM");
                  break;
            case SPD_LRDIMM_DDR4:
              UnicodeSPrint(DimmString2,StrSize, L" LRDIMM-DDR4");
              break;
            case SPD_LRDIMM:
              UnicodeSPrint(DimmString2,StrSize, L" LRDIMM");
              break;
            case SPD_NVM_DIMM:
              UnicodeSPrint(DimmString2,StrSize, L" NVDIMM");
              break;
            case SPD_UDIMM_ECC:
              UnicodeSPrint(DimmString2,StrSize, L" ECC-UDIMM");
              break;
            default:
              UnicodeSPrint(DimmString2,StrSize, L"N/A");
              break;
            }
          }
          else if (SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].keyByte == SPD_TYPE_AEP) {
            UnicodeSPrint(DimmString2,StrSize, L" AEP-DIMM");
          }
          StrCatS(DimmString1, StrSize/sizeof(CHAR16), DimmString2);
        }
        TokenToUpdate = MemTopology[Cntr];
        HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, DimmString1, NULL);
       // APTIOV_SERVER_OVERRIDE_RC_START : Update MemoryTopology with extra DIMM details
        if (/*SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].Enabled
            &&*/ SystemMemoryMap->Socket[Socket].ChannelInfo[Channel].DimmInfo[Dimm].Present) {
            Cntr++;
        }
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
        if (FailDimmExist) {
            Cntr++;
            FailDimmExist = FALSE;
        }
#endif
        ZeroMem(DimmString1,StrSize);
        ZeroMem(DimmString2,StrSize);
      // APTIOV_SERVER_OVERRIDE_RC_END : Update MemoryTopology with extra DIMM details
      } // dimm loop
    } // Channel loop
  } // Socket loop

  gBS->FreePool (DimmString1);
  gBS->FreePool (DimmString2);
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
UpdateMemoryInformation (
  VOID
  )
{
  UpdateMemType();
  DisplayMemTopology();
}

/**

  This function configures KTI settings in Setup.

  @param none

  @retval none

**/
VOID
UpdateKtiStatusLines (
  VOID
  )
{
  CHAR16                *NewString;
  STRING_REF            TokenToUpdate;
  UINTN                 StrSize;

  StrSize   = 0x250;
  NewString = AllocateZeroPool (StrSize);

  if (NewString == NULL) {
    DEBUG ((EFI_D_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocateZeroPool() returned NULL pointer.\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return;
  }

  //
  // Number of CPU
  //
  TokenToUpdate = (STRING_REF) STR_KTI_NUMBER_OF_CPU_VAL;
  UnicodeSPrint (NewString,StrSize,L"%d",(UINTN)mIioUds->IioUdsPtr->SystemStatus.numCpus);
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Number of IOH
  //
  TokenToUpdate = (STRING_REF) STR_KTI_NUMBER_OF_IIO_VAL;
  UnicodeSPrint (NewString,StrSize,L"%d",(UINTN)mIioUds->IioUdsPtr->PlatformData.numofIIO);
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Current MP Link Frequency
  //
  switch (mIioUds->IioUdsPtr->SystemStatus.CurrentCsiLinkFrequency) {
  case SPEED_REC_96GT:
    StrCpyS (NewString, StrSize/sizeof(CHAR16), L"9.6 GT/s");
    break;

  case SPEED_REC_104GT:
    StrCpyS (NewString, StrSize/sizeof(CHAR16), L"10.4 GT/s");
    break;
  default:
    StrCpyS (NewString, StrSize/sizeof(CHAR16), L"Unknown or 1S configuration");
    break;
  }

  //
  // If Link Speed is set to slow and multiple CPUs are present, then by definition the link frequency should be 50 MT/s
  //
  if((!mIioUds->IioUdsPtr->SystemStatus.CurrentCsiLinkSpeed) && (mIioUds->IioUdsPtr->SystemStatus.numCpus > 1)){
    StrCpyS (NewString, StrSize/sizeof(CHAR16), L"50 MT/s");
  }

  TokenToUpdate = (STRING_REF) STR_KTI_LINK_FREQ_STATUS;
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Current MP Link Speed
  //
  if (mIioUds->IioUdsPtr->SystemStatus.CurrentCsiLinkSpeed)
      StrCpyS (NewString, StrSize/sizeof(CHAR16), L"Fast");
  else
      StrCpyS (NewString, StrSize/sizeof(CHAR16), L"Slow or 1S Configuration");

  TokenToUpdate = (STRING_REF) STR_KTI_LINK_SPEED_STATUS;
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);
  //
  // Global MMIO Low Base / Limit
  //
  TokenToUpdate = (STRING_REF) STR_KTI_GMMIOL_BASE_LIMIT_VAL;
  UnicodeSPrint (NewString,StrSize,L"%08x / %08x", (UINTN)mIioUds->IioUdsPtr->PlatformData.PlatGlobalMmiolBase, (UINTN)mIioUds->IioUdsPtr->PlatformData.PlatGlobalMmiolLimit);
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Global MMIO High Base / Limit
  //
  TokenToUpdate = (STRING_REF) STR_KTI_GMMIOH_BASE_LIMIT_VAL;
  UnicodeSPrint (NewString,StrSize,L"%016x / %016x", mIioUds->IioUdsPtr->PlatformData.PlatGlobalMmiohBase, mIioUds->IioUdsPtr->PlatformData.PlatGlobalMmiohLimit);
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Pci-e Configuration Base
  //
  TokenToUpdate = (STRING_REF) STR_KTI_PCI_E_CFG_BASE_VAL;
  UnicodeSPrint (NewString,StrSize,L"%08x / %08x", (UINTN)mIioUds->IioUdsPtr->PlatformData.PciExpressBase, (UINTN)mIioUds->IioUdsPtr->PlatformData.PciExpressSize);
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);


  gBS->FreePool (NewString);

  return ;
}


/**

  This function configures KTI settings in Setup.

  @param none

  @retval none

**/
VOID
UpdateKtiResourceAllocationLines (
  VOID
  )
{
  return ;
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
UpdateMpLinkInformation (
  VOID
  )
{
  UpdateKtiStatusLines();
  UpdateKtiResourceAllocationLines();
  return;
}

/**

  This function configures PCIE settings in Setup.

  @param none

  @retval none

**/
VOID
ConfigPciePort (
  )
{
  UINT8                                 PcieLinkStatus;
  UINT8                                 PcieLinkMax;
  UINT8                                 PcieLinkSpeed;
  UINTN                                 Index;
  CHAR16                                *NewString;
  CHAR16                                *NewString1;
  CHAR16                                *NewMaxString;
  CHAR16                                *NewMaxString1;
  CHAR16                                *NewSpeedString;
  CHAR16                                *NewSpeedString1;
  UINTN                                 StrSize;
  EFI_STRING_ID                         TokenToUpdate;
  EFI_STRING_ID                         TokenToUpdateMax;
  EFI_STRING_ID                         TokenToUpdateSpeed;
  EFI_IIO_SYSTEM_PROTOCOL               *mIioSystem;
  EFI_STATUS                            Status = EFI_SUCCESS;


  StrSize   = 0x50;

  PcieLinkStatus = 0;
  TokenToUpdate = (STRING_REF) STR_LINK_STATUS;
  NewString   = AllocateZeroPool (StrSize);
  NewString1  = AllocateZeroPool (StrSize);
  PcieLinkMax = 0;
  TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS;
  NewMaxString   = AllocateZeroPool (StrSize);
  NewMaxString1  = AllocateZeroPool (StrSize);
  TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS;
  NewSpeedString   = AllocateZeroPool (StrSize);
  NewSpeedString1  = AllocateZeroPool (StrSize);
  PcieLinkSpeed = 0;

  if (NewString == NULL || NewString1 == NULL || NewMaxString == NULL || 
      NewMaxString1 == NULL || NewSpeedString == NULL || NewSpeedString1 == NULL) {
    DEBUG ((EFI_D_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocateZeroPool() returned NULL pointer.\n")); 
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES); 
    goto Done;
  }

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &mIioSystem);
  //
  // Get the PCI-E Link Status
  //
  for (Index = 0; Index < (mIioUds->IioUdsPtr->PlatformData.numofIIO*NUMBER_PORTS_PER_SOCKET); Index++) {
      if (Status == EFI_SUCCESS) {
        PcieLinkStatus = mIioSystem->IioGlobalData->IioVar.IioOutData.LinkedPXPMap[Index];
        PcieLinkMax = mIioSystem->IioGlobalData->IioVar.IioOutData.MaxPXPMap[Index];
        PcieLinkSpeed = mIioSystem->IioGlobalData->IioVar.IioOutData.SpeedPXPMap[Index];
      }
      //DEBUG((EFI_D_ERROR, "the Index and PcieLinkStatus are %x, %x\n", Index,PcieLinkStatus));
      if (PcieLinkStatus != 0) {
        UnicodeValueToString (
                          NewString,
                          LEFT_JUSTIFY,
                          PcieLinkStatus,
                          2
                          );
        StrCpyS (NewString1, StrSize/sizeof(CHAR16), L"Linked as x");
        StrCatS (NewString1, StrSize/sizeof(CHAR16), NewString);
        } else {
        StrCpyS (NewString1, StrSize/sizeof(CHAR16), L"Link Did Not Train");
      }
      if (PcieLinkMax != 0) {
        UnicodeValueToString (
                          NewMaxString,
                          LEFT_JUSTIFY,
                          PcieLinkMax,
                          2
                          );
        StrCpyS (NewMaxString1, StrSize/sizeof(CHAR16), L"Max Width x");
        StrCatS (NewMaxString1, StrSize/sizeof(CHAR16), NewMaxString);
        } else {
        StrCpyS (NewMaxString1, StrSize/sizeof(CHAR16), L"ERROR: Not Available");
      }

      if (PcieLinkStatus != 0) {
        StrCpyS (NewSpeedString1, StrSize/sizeof(CHAR16), L"Gen ");
        UnicodeValueToString (
                          NewSpeedString,
                          LEFT_JUSTIFY,
                          PcieLinkSpeed,
                          2
                          );
        StrCatS(NewSpeedString1, StrSize/sizeof(CHAR16), NewSpeedString);

        switch(PcieLinkSpeed) {
        // Values for link speed come from PCI-E spec. Add new cases for future generations
        case 1:
          StrCatS (NewSpeedString1, StrSize/sizeof(CHAR16), L" (2.5 GT/s)");
          break;
        case 2:
          StrCatS (NewSpeedString1, StrSize/sizeof(CHAR16), L" (5.0 GT/s)");
          break;
        case 3:
          StrCatS (NewSpeedString1, StrSize/sizeof(CHAR16), L" (8.0 GT/s)");
          break;
        default:
          StrCatS (NewSpeedString1, StrSize/sizeof(CHAR16), L" (Undefined)");
          break;
        }
      } else {
        StrCpyS (NewSpeedString1, StrSize/sizeof(CHAR16), L"Link Did Not Train");
      }

      if (Index == 0) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT0;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT0;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT0;
      } else if (Index == 1) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT1;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT1;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT1;
      } else if (Index == 2) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT2;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT2;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT2;
      } else if (Index == 3) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT3;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT3;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT3;
      } else if (Index == 4) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT4;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT4;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT4;
      } else if (Index == 5) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT5;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT5;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT5;
      } else if (Index == 6) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT6;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT6;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT6;
      } else if (Index == 7) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT7;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT7;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT7;
      } else if (Index == 8) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT8;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT8;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT8;
      } else if (Index == 9) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT9;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT9;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT9;
      } else if (Index == 10) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT10;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT10;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT10;
      } else if (Index == 11) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT11;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT11;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT11;
      } else if (Index == 12) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT12;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT12;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT12;
      } else if (Index == 13) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT13;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT13;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT13;
      } else if (Index == 14) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT14;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT14;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT14;
      } else if (Index == 15) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT15;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT15;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT15;
      } else if (Index == 16) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT16;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT16;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT16;
      } else if (Index == 17) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT17;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT17;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT17;
      } else if (Index == 18) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT18;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT18;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT0;
      } else if (Index == 19) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT19;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT19;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT19;
      } else if (Index == 20) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO0PORT20;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO0PORT20;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO0PORT20;
      } else if (Index == 21) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT0;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT0;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT0;
      } else if (Index == 22) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT1;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT1;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT1;
      } else if (Index == 23) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT2;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT2;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT2;
      } else if (Index == 24) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT3;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT3;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT3;
      } else if (Index == 25) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT4;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT4;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT4;
      } else if (Index == 26) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT5;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT5;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT5;
      } else if (Index == 27) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT6;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT6;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT6;
      } else if (Index == 28) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT7;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT7;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT7;
      } else if (Index == 29) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT8;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT8;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT8;
      } else if (Index == 30) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT9;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT9;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT9;
      } else if (Index == 31) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT10;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT10;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT10;
      } else if (Index == 32) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT11;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT11;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT11;
      } else if (Index == 33) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT12;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT12;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT12;
      } else if (Index == 34) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT13;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT13;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT13;
      } else if (Index == 35) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT14;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT14;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT14;
      } else if (Index == 36) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT15;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT15;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT15;
      } else if (Index == 37) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT16;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT16;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT16;
      } else if (Index == 38) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT17;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT17;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT17;
      } else if (Index == 39) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT18;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT18;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT18;
      } else if (Index == 40) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT19;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT19;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT19;
      } else if (Index == 41) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO1PORT20;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO1PORT20;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO1PORT20;
      } else if (Index == 42) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT0;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT0;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT0;
      } else if (Index == 43) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT1;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT1;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT1;
      } else if (Index == 44) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT2;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT2;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT2;
      } else if (Index == 45) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT3;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT3;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT3;
      } else if (Index == 46) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT4;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT4;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT4;
      } else if (Index == 47) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT5;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT5;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT5;
      } else if (Index == 48) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT6;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT6;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT6;
      } else if (Index == 49) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT7;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT7;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT7;
      } else if (Index == 50) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT8;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT8;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT8;
      } else if (Index == 51) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT9;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT9;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT9;
      } else if (Index == 52) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT10;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT10;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT10;
      } else if (Index == 53) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT11;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT11;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT11;
      } else if (Index == 54) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT12;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT12;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT12;
      } else if (Index == 55) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT13;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT13;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT13;
      } else if (Index == 56) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT14;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT14;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT14;
      } else if (Index == 57) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT15;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT15;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT15;
      } else if (Index == 58) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT16;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT16;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT16;
      } else if (Index == 59) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT17;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT17;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT17;
      } else if (Index == 60) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT18;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT18;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT18;
      } else if (Index == 61) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT19;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT19;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT19;
      } else if (Index == 62) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO2PORT20;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO2PORT20;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO2PORT20;
      } else if (Index == 63) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT0;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT0;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT0;
      } else if (Index == 64) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT1;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT1;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT1;
      } else if (Index == 65) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT2;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT2;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT2;
      } else if (Index == 66) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT3;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT3;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT3;
      } else if (Index == 67) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT4;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT4;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT4;
      } else if (Index == 68) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT5;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT5;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT5;
      } else if (Index == 69) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT6;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT6;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT6;
      } else if (Index == 70) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT7;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT7;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT7;
      } else if (Index == 71) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT8;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT8;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT8;
      } else if (Index == 72) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT9;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT9;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT9;
      } else if (Index == 73) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT10;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT10;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT10;
      } else if (Index == 74) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT11;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT11;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT11;
      } else if (Index == 75) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT12;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT12;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT12;
      } else if (Index == 76) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT13;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT13;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT13;
      } else if (Index == 77) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT14;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT14;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT14;
      } else if (Index == 78) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT15;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT15;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT15;
      } else if (Index == 79) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT16;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT16;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT16;
      } else if (Index == 80) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT17;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT17;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT17;
      } else if (Index == 81) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT18;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT18;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT18;
      } else if (Index == 82) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT19;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT19;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT19;
      } else if (Index == 83) {
        TokenToUpdate    = (STRING_REF) STR_LINK_STATUS_IIO3PORT20;
        TokenToUpdateMax = (STRING_REF) STR_MAXLINK_STATUS_IIO3PORT20;
        TokenToUpdateSpeed = (STRING_REF) STR_PCIESPEED_STATUS_IIO3PORT20;
      }

      HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString1, NULL);
      HiiSetString(mSocketStringsHiiHandle, TokenToUpdateMax, NewMaxString1, NULL);
      HiiSetString(mSocketStringsHiiHandle, TokenToUpdateSpeed, NewSpeedString1, NULL);
    }

Done:
  if (NewString != NULL) {
    gBS->FreePool (NewString);
  }

  if (NewString1 != NULL) {
    gBS->FreePool (NewString1);
  }

  if (NewMaxString != NULL) {
    gBS->FreePool (NewMaxString);
  }

  if (NewMaxString1 != NULL) {
    gBS->FreePool (NewMaxString1);
  }

  if (NewSpeedString != NULL) {
    gBS->FreePool (NewSpeedString);
  }

  if (NewSpeedString1 != NULL) {
    gBS->FreePool (NewSpeedString1);
  }

  return;
}

/**

  This function configures the IIO settings.

  @param none

  @retval none

**/
VOID
ConfigIio (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       VarSize;
  UINT32      IioBitMap;
  UINT8       Socket;
  struct SystemMemoryMapHob     *mSystemMemoryMap;
  EFI_HOB_GUID_TYPE             *GuidHob;
  SOCKET_IIO_CONFIGURATION      mSocketIioConfiguration;
  SOCKET_MEMORY_CONFIGURATION   mSocketMemoryConfiguration;
  SOCKET_COMMONRC_CONFIGURATION mSocketCommonRcConfiguration;
  UINT32                        IioAttributes;
  UINT32                        CrcAttributes;
  UINT32                        SktMemAttributes;
  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
  if ( GuidHob != NULL)
    mSystemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA (GuidHob);
  else {
    mSystemMemoryMap = NULL;
    Status = EFI_DEVICE_ERROR;
    ASSERT_EFI_ERROR (Status);
    return;
  }

  VarSize = sizeof(SOCKET_IIO_CONFIGURATION);

  Status = gRT->GetVariable(
                            SOCKET_IIO_CONFIGURATION_NAME,
                            &gEfiSocketIioVariableGuid,
                            &IioAttributes,
                            &VarSize,
                            &mSocketIioConfiguration
                            );
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof(SOCKET_COMMONRC_CONFIGURATION);

  Status = gRT->GetVariable(
                            SOCKET_COMMONRC_CONFIGURATION_NAME,
                            &gEfiSocketCommonRcVariableGuid,
                            &CrcAttributes,
                            &VarSize,
                            &mSocketCommonRcConfiguration
                            );
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof(SOCKET_MEMORY_CONFIGURATION);

  Status = gRT->GetVariable(
                            SOCKET_MEMORY_CONFIGURATION_NAME,
                            &gEfiSocketMemoryVariableGuid,
                            &SktMemAttributes,
                            &VarSize,
                            &mSocketMemoryConfiguration
                            );
  ASSERT_EFI_ERROR (Status);

  IioBitMap =  mIioUds->IioUdsPtr->SystemStatus.socketPresentBitMap;  // storing value of IIO presence indicator byte in temporary variable
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if((IioBitMap & (1 << Socket)) == 0) {    
      mSocketIioConfiguration.IioPresent[Socket] = 0;
    } else {
      mSocketIioConfiguration.IioPresent[Socket] = 1;
    }
  }

  mSocketMemoryConfiguration.XMPProfilesSup = mSystemMemoryMap->XMPProfilesSup;
  mSocketCommonRcConfiguration.CpuStepping   = mIioUds->IioUdsPtr->SystemStatus.MinimumCpuStepping;
  mSocketCommonRcConfiguration.SystemRasType = mIioUds->IioUdsPtr->SystemStatus.SystemRasType;

  Status = gRT->SetVariable(
                SOCKET_IIO_CONFIGURATION_NAME,
                &gEfiSocketIioVariableGuid,
                IioAttributes,
                sizeof (SOCKET_IIO_CONFIGURATION),
                &mSocketIioConfiguration
                );
  ASSERT_EFI_ERROR (Status);

  Status = gRT->SetVariable(
                SOCKET_MEMORY_CONFIGURATION_NAME,
                &gEfiSocketMemoryVariableGuid,
              // APTIOV_SERVER_OVERRIDE_RC_START : Change in Attribute variable
                SktMemAttributes,
             // APTIOV_SERVER_OVERRIDE_RC_END : Change in Attribute variable
                sizeof (SOCKET_MEMORY_CONFIGURATION),
                &mSocketMemoryConfiguration
                );
  ASSERT_EFI_ERROR (Status);

  Status = gRT->SetVariable(
                SOCKET_COMMONRC_CONFIGURATION_NAME,
                &gEfiSocketCommonRcVariableGuid,
              // APTIOV_SERVER_OVERRIDE_RC_START : Change in Attribute variable
                CrcAttributes,
              // APTIOV_SERVER_OVERRIDE_RC_END : Change in Attribute variable
                sizeof (SOCKET_COMMONRC_CONFIGURATION),
                &mSocketCommonRcConfiguration
                );
  ASSERT_EFI_ERROR (Status);
  return ;
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
UpdateIioPcieInformation (
  VOID
  )
{
  ConfigIio();
  ConfigPciePort();
}

/**

    Update Core Disable Bitmap in help 

    @param None

    @retval None
    @retval None

**/
VOID
UpdateCoreDisableHelp (
  VOID
)
{
  EFI_STRING                              NewString;
  EFI_STRING                              CoreBitMaskString;
  EFI_STRING_ID                           TokenToUpdate;

  NewString = AllocateZeroPool (NEWSTRING_SIZE);
  CoreBitMaskString = AllocateZeroPool (NEWSTRING_SIZE);

  if((NewString == NULL) || (CoreBitMaskString == NULL)){
    if(NewString != NULL){
      gBS->FreePool(NewString);
    }
    if(CoreBitMaskString != NULL){
      gBS->FreePool (CoreBitMaskString);
    }
    return;
  }

  TokenToUpdate = STR_STR_CORE_DISABLE_BITMAP_HELP;
  StrCpyS (NewString, NEWSTRING_SIZE/sizeof(CHAR16), L"0: Enable all cores. ");
  UnicodeValueToString (CoreBitMaskString, RADIX_HEX, MAX_CORE_BITMASK, 0);
  StrCatS (NewString, NEWSTRING_SIZE/sizeof(CHAR16), CoreBitMaskString);
  StrCatS (NewString, NEWSTRING_SIZE/sizeof(CHAR16), L": Disable all cores");
  HiiSetString(mSocketStringsHiiHandle, TokenToUpdate, NewString, NULL);

  gBS->FreePool (NewString);
  gBS->FreePool (CoreBitMaskString);
  return;
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
EFIAPI
UpdateSocketDataEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS Status;
  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);
  UpdateCpuInformation();
  UpdateMemoryInformation();
  UpdateIioPcieInformation();
  UpdateMpLinkInformation();
  UpdateCoreDisableHelp();
  return;
}

/**

    GC_TODO: add routine description

    @param None

    @retval TRUE  - GC_TODO: add retval description
    @retval FALSE - GC_TODO: add retval description

**/
VOID
UpdateSocketData (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_EVENT             DummyEvent;
  VOID                  *mSocketDummyRegistration;

  Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    UpdateSocketDataEvent,
                    NULL,
                    &DummyEvent
                    );
  ASSERT_EFI_ERROR (Status);
  //
  // Register protocol notifications on this event.
  //
  Status = gBS->RegisterProtocolNotify (
              // APTIOV_SERVER_OVERRIDE_RC_START : Use GUID for TSE.
                    &gEfiSetupEnterGuid,
              // APTIOV_SERVER_OVERRIDE_RC_END : Use GUID for TSE.
                    DummyEvent,
                    &mSocketDummyRegistration
                    );
  ASSERT_EFI_ERROR (Status);
  return;
}

/**

  Entry point of the SOCKET Embedded controller screen setup driver.

  @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully

**/
EFI_STATUS
EFIAPI
InstallSocketSetupScreen (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                           Status;
  UINTN                                Index;
  EFI_IFR_GUID_LABEL                   * StartLabel;
  EFI_IFR_GUID_LABEL                   * EndLabel;
  BOOLEAN                              ActionFlag;
  EFI_STRING                           ConfigRequestHdr;
  //
  //  APTIOV_SERVER_OVERRIDE_RC_START : Getting resources for Socket Configuration
  //
  EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList = NULL;
  EFI_HII_DATABASE_PROTOCOL *HiiDatabase = NULL;
  //
  //  Get HiiPackageList header resource from handle.
  //
  Status = gBS->HandleProtocol(ImageHandle, &gEfiHiiPackageListProtocolGuid, (VOID**)&HiiPackageList);
  ASSERT_EFI_ERROR (Status);
  //
  //  APTIOV_SERVER_OVERRIDE_RC_END : Getting resources for Socket Configuration
  //

  mPrivateData.Signature = SOCKET_SETUP_SIGNATURE;
  mPrivateData.ImageHandle = ImageHandle;


  mPrivateData.DeviceHandle = NULL;

  mPrivateData.ConfigAccess.ExtractConfig = DefaultExtractConfig;
  mPrivateData.ConfigAccess.RouteConfig   = DefaultRouteConfig;
  mPrivateData.ConfigAccess.Callback      = CallbackRoutine;

  Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
                  &mPrivateData.DeviceHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mPrivateData.ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);


  Status = SystemTable->BootServices->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID**)&mHiiConfigRouting);
  ASSERT_EFI_ERROR (Status);

  //
  //  APTIOV_SERVER_OVERRIDE_RC_START : Getting resources for Socket Configuration
  //
  #if 0
  mPrivateData.HiiHandle = HiiAddPackages (
                       &gSocketPkgListGuid,
                       mPrivateData.DeviceHandle,
                       SocketSetupStrings,
                       SocketSetupFormsBin,
                       NULL
                       );
  #endif
  //
  //  Locate HiiDataBase protocol
  //
  Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&HiiDatabase);
  ASSERT_EFI_ERROR (Status);

  //
  // Register SocketSetup package list with the HII Database
  //
  Status = HiiDatabase->NewPackageList (
                     HiiDatabase, 
                     HiiPackageList, 
                     mPrivateData.DeviceHandle, 
                     &mPrivateData.HiiHandle
                     );
  ASSERT_EFI_ERROR (Status);
  //
  //  APTIOV_SERVER_OVERRIDE_RC_END : Getting resources for Socket Configuration
  //

   ASSERT(mPrivateData.HiiHandle != NULL);


  //
  // Initialize the container for dynamic opcodes
  //
  mPrivateData.StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (mPrivateData.StartOpCodeHandle == NULL) {
    DEBUG ((EFI_D_ERROR, "\n ERROR!!! HiiAllocateOpCodeHandle () - Returned NULL Pointer for mPrivateData.StartOpCodeHandle\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return EFI_OUT_OF_RESOURCES;
  }

  mPrivateData.EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (mPrivateData.EndOpCodeHandle == NULL) {
    DEBUG ((EFI_D_ERROR, "\n ERROR!!! HiiAllocateOpCodeHandle () - Returned NULL Pointer for mPrivateData.EndOpCodeHandle\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create Hii Extend Label OpCode as the Top opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mPrivateData.StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = VFR_FORMLABLE_SOCKET_TOP;

  //
  // Create Hii Extend Label OpCode as the Bottom opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mPrivateData.EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = VFR_FORMLABLE_SOCKET_BOTTOM;

  if (IsCMOSBad()) {
    for (Index = 0; Index < MaxSocketVarInfo; Index ++) {
      ConfigRequestHdr = HiiConstructConfigHdr (SocketVariableInfo[Index].Guid,  SocketVariableInfo[Index].VariableName, mPrivateData.DeviceHandle);
      ASSERT (ConfigRequestHdr != NULL);
      ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_STANDARD);
      ASSERT (ActionFlag);

    }
  }

  mSocketStringsHiiHandle = mPrivateData.HiiHandle;
  UpdateSocketData();

  return EFI_SUCCESS;
}
