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
    AFRI Variable Name, Guid, and data buffer definition.
**/
#ifndef __AMI_FRI_H__
#define __AMI_FRI_H__

#define AMI_FRI_VARIABLE_NAME L"AFRI"

#define AMI_FRI_VARIABLE_GUID \
    { 0x280bcc15, 0x2cd7, 0x467c, { 0x96, 0x47, 0x1b, 0x30, 0x30, 0x7d, 0x7e, 0xd5 } }

/**
   AMI Firmware Runtime Interface Data Header.
**/
typedef struct _AMI_FRI_DATA_HEADER{
    UINT16  HeaderLength; ///< Header Length. Should be 8.
    UINT16  Revision;     ///< Should be 0. This field indicates the revision of the AMI_FRI_DATA and AMI_FRI_DATA_HEADER.
    UINT16  UintnSize;    ///< Native integer size. Must be equal to sizeof(UINTN). On mismatch AFRI_INVALID_UINTN_SIZE is returned
    UINT16  ReturnValue;  ///< AFRI call return status value.
} AMI_FRI_DATA_HEADER;

/**
   AFRI Return Values
*/
#define AFRI_ERROR_BIT               0x8000
#define AFRI_SUCCESS                 0                    ///< AFRI call has completed without errors
#define AFRI_INVALID_HEADER_REVISION (AFRI_ERROR_BIT | 1) ///< Invalid Header Revision
#define AFRI_NOT_SUPPORTED           (AFRI_ERROR_BIT | 2) ///< AFRI interface is not supported
#define AFRI_INVALID_UINTN_SIZE      (AFRI_ERROR_BIT | 3) ///< UINTN size mismatch
#define AFRI_NO_RESPONSE             (AFRI_ERROR_BIT | 4) ///< No response from AFRI handler
#define AFRI_HANDLER_NOT_FOUND       (AFRI_ERROR_BIT | 5) ///< Handler is not found
#define AFRI_HANDLER_ERROR           (AFRI_ERROR_BIT | 6) ///< Handler returned an error
#define AFRI_OUT_OF_RESOURCES        (AFRI_ERROR_BIT | 7) ///< Response size is larger than the maximum supported response size

/**
  AMI SMM Communication Header.
**/
typedef struct _AMI_SMM_COMMUNICATE_HEADER{
    EFI_GUID  HeaderGuid;
    UINTN     MessageLength; /// Describes the size of Data (in bytes)and does not include the size of the header.
    // UINT8  Data[MessageLength];
}AMI_SMM_COMMUNICATE_HEADER;

/**
  AMI Firmware Runtime Interface Data.
**/
typedef struct _AMI_FRI_DATA{
    AMI_FRI_DATA_HEADER        DataHeader;
    AMI_SMM_COMMUNICATE_HEADER CommHeader;
}AMI_FRI_DATA;

extern EFI_GUID gAmiFriVariableGuid;

#endif

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
