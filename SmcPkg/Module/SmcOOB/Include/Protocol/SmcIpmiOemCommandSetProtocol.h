//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug fixed:
//    Reason   :
//    Auditor  :
//    Date     :
//
//****************************************************************************

#ifndef _SMC_IPMI_OEM_COMMAND_SET_PROTOCOL_H_
#define _SMC_IPMI_OEM_COMMAND_SET_PROTOCOL_H_
        
#define SMC_IPMI_OEM_COMMAND_SET_PROTOCOL_GUID \
  { \
    0x81F93099, 0xA7F, 0x4F1A, {0x94, 0xD4, 0xB3, 0xBE, 0x3F, 0xAC, 0xEA, 0x6A} \
  }

//
// Forward Declaration
//
typedef struct _SMC_IPMI_OEM_COMMAND_SET_PROTOCOL SMC_IPMI_OEM_COMMAND_SET_PROTOCOL;

//
// Revision 1: First Version
//
#define SMC_IPMI_OEM_COMMAND_SET_PROTOCOL_REVISION_1   1

typedef
EFI_STATUS
(EFIAPI *DOWNLOAD_OOB_DATA_START) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT32                            *FileSize,
  OUT UINT8                             *ResponseStatus
  );

typedef
EFI_STATUS
(EFIAPI *DOWNLOAD_OOB_DATA_READY) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT8                             *ResponseStatus
  );

typedef
EFI_STATUS
(EFIAPI *DOWNLOAD_OOB_DATA_DONE) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  IN  UINT8                             UpdateStatus
  );

typedef
EFI_STATUS
(EFIAPI *UPLOAD_OOB_DATA_START) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  IN  UINT32                            FileSize,
  OUT UINT8                             *ResponseStatus
  );

typedef
EFI_STATUS
(EFIAPI *UPLOAD_OOB_DATA_DONE) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             FileType,
  OUT UINT8                             *ResponseStatus
  );

typedef
EFI_STATUS
(EFIAPI *READ_DATA_TO_BIOS) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  OUT UINT8                             *Buffer,
  IN  UINT32                            BufferLength
  );

typedef
EFI_STATUS
(EFIAPI *PREPARE_DATA_TO_BMC) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             *Buffer,
  IN  UINT32                            BufferLength
  );

typedef
EFI_STATUS
(EFIAPI *GET_SET_OOB_IDENTIFY) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT32                            BoardId,
  IN  UINT32                            Date,
  IN  UINT16                            Time
  );

typedef
EFI_STATUS
(EFIAPI *SMBIOS_PRESERVATION) (
  IN  SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *This,
  IN  UINT8                             Operation,
  IN  UINT8                             FlashFlag,
  OUT UINT8                             *ResponseStatus
  );

typedef struct _OOB_IDENTIFY_SET {
  UINT8 BoardId[4];
  UINT8 Date[4];
  UINT8 Time[2];
};

//
// SMC IPMI OEM COMMAND SET PROTOCOL
//
typedef struct _SMC_IPMI_OEM_COMMAND_SET_PROTOCOL {
  UINT32                           PolicyRevision;
  UINT32                           PlatformId;
  DOWNLOAD_OOB_DATA_START          DownloadOobDataStart;
  DOWNLOAD_OOB_DATA_READY          DownloadOobDataReady;
  DOWNLOAD_OOB_DATA_DONE           DownloadOobDataDone;
  UPLOAD_OOB_DATA_START            UploadOobDataStart;
  UPLOAD_OOB_DATA_DONE             UploadOobDataDone;
  READ_DATA_TO_BIOS                ReadDataToBios;
  PREPARE_DATA_TO_BMC              PrepareDataToBmc;
  GET_SET_OOB_IDENTIFY             GetSetOobIdentify;
  SMBIOS_PRESERVATION              SmibiosPreservation;
};

extern EFI_GUID gSmcIpmiOemCommandSetProtocolGuid;

#endif

