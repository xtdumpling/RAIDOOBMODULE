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

/** @file PeiSelPpi.h
    Pei Sel Definations.

**/

#ifndef _PEI_SEL_PPI_H_
#define _PEI_SEL_PPI_H_

#define PEI_SEL_PPI_GUID \
  { \
    0xe50b6e5d, 0x359b, 0x4be0, { 0xa0, 0xb0, 0x7, 0x15, 0xa3, 0x1, 0xa6, 0x6 } \
  }

typedef struct _PEI_SEL_STATUS_CODE_PPI PEI_SEL_STATUS_CODE_PPI;

//
//   Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_PEI_SEL_REPORT_STATUS_CODE) (
  IN CONST          EFI_PEI_SERVICES         **PeiServices,
  IN                PEI_SEL_STATUS_CODE_PPI  * This,
  IN                EFI_STATUS_CODE_TYPE     CodeType,
  IN                EFI_STATUS_CODE_VALUE    Value,
  IN                UINT32                   Instance,
  IN CONST          EFI_GUID                 *CallerId,
  IN CONST OPTIONAL EFI_STATUS_CODE_DATA     *Data
  );

/**
  Pei Sel PROTOCOL
*/
typedef struct _PEI_SEL_STATUS_CODE_PPI {
  UINT8                             LogSelStatusCodes;  ///< Setup Option
  PEI_IPMI_TRANSPORT_PPI            *IpmiTransportPpi;  ///< Pointer to Ipmi Transport PPI
  EFI_PEI_SEL_REPORT_STATUS_CODE    EfiSelReportStatusCode; ///< Report Status code function
} PEI_SEL_STATUS_CODE_PPI;

extern EFI_GUID gEfiPeiSelPpiGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
