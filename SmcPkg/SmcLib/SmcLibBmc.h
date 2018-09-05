//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.15
//    Bug Fix:  Update necessary code with newest SmcLibBmc.c
//    Reason:   
//    Auditor:  Durant Lin
//    Date:     Dec/21/2017
//
//  Rev. 1.14
//    Bug Fix : N/A
//    Reason  : Implemented BIOS-based TPM provisioning finished flag for SUM.
//    Auditor : Joe Jhang
//    Date    : May/24/2017
//
//  Rev. 1.13
//    Bug Fix:  Add WDT timer out value selection feature.
//    Reason:   
//    Auditor:  Jimmy Chiu (Refer Greenlow - Mark Chen)
//    Date:     May/05/2017
//
//  Rev. 1.12
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/06/2017
//
//  Rev. 1.11
//    Bug Fix:  Fix some errors for upload and download OOB files.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/20/2016
//
//  Rev. 1.10
//    Bug Fix:  Rewrite OOB download file code.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/19/2016
//
//  Rev. 1.09
//    Bug Fix:  Skip SmcOutBand and SmcAssetInfo when disabling JPG1.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/12/2016
//
//  Rev. 1.08
//   Bug Fix:   add CPLD version support
//   Reason:	Get CPLD version from BMC
//    Auditor:  Sunny Yang
//    Date:     Sep/01/2016
//
//  Rev. 1.07
//    Bug Fix:  Fix some errors in SmcOutBand module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/10/2016
//
//  Rev. 1.06
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.05
//    Bug Fix:  Review BMC OEM command
//    Reason:   Add SMC_IPMICmd30_99
//    Auditor:  Jimmy Chiu
//    Date:     Jul/28/2016
//
//  Rev. 1.04
//    Bug Fix:  Add IPMI video ram data transfer library.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.02
//    Bug Fix:  Add SendBIOSVerBuildTimeDXE to sync BIOS version with SMBIOS Type 0.
//    Reason:   To avoid BIOS version not synchronize problem.
//    Auditor:  Jacker Yeh
//    Date:     Jul/19/2016
//
//  Rev. 1.01
//    Bug Fix:  Review BMC OEM command.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/11/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************
//****************************************************************************

#ifndef __SMC_LIB_BMC__H__
#define __SMC_LIB_BMC__H__
#ifdef __cplusplus
extern "C" {
#endif

#define OOB_FILE_TYPE_HII 0
#define OOB_FILE_TYPE_BIN 1
#define OOB_FILE_TYPE_DMI 2
#define OOB_FILE_TYPE_FULL_SMBIOS 3
#define OOB_FILE_TYPE_VPD 4
#define OOB_FILE_TYPE_ASSET 5
#define OOB_FILE_TYPE_RESERVED 6
#define OOB_FILE_TYPE_BIOS_FEATURE_FLAG 7
#define OOB_FILE_TYPE_BIOS_STORAGE 8

#define	OOB_GET	0;
#define	OOB_SET	1;

#define OOB_IPMI_COMMAND_RETRY 20
#define OOB_IPMI_FILE_RETRY 3
#define SMC_IPMI_RETRY  5

//BIOS Storage flash parameter
#define Preserve_SMBIOS BIT0
#define Preserve_OA BIT1

#pragma pack(1)

typedef struct {
	UINT8	SubCMD;
	UINT8	FileType;
	UINT8	FileLength[4];
}DataToBMCReadyNotify_REQ;

typedef struct {
	UINT8	SubCMD;
	UINT8	FileType;
}DataToBMCDone_REQ;

typedef struct {
	UINT8	ChecksumStatus;
}DataToBMCDone_RSP;

typedef struct {
	UINT8	SubCMD;
	UINT8	FileType;
}DataToBIOSReadyNotify_REQ;

typedef struct {
	UINT8	Status;
	UINT8	FileLength[4];
}DataToBIOSReadyNotify_RSP;

typedef struct {
	UINT8	SubCMD;
	UINT8	FileType;
}DataToBIOSWait_REQ;

typedef struct {
	UINT8	Status;
}DataToBIOSWait_RSP;

typedef struct {
	UINT8	SubCMD;
	UINT8	FileType;
	UINT8	DownloadStatus;
}DataToBIOSDone_REQ;

typedef struct{
    UINT8	SVID[4];
    UINT8	Date[4];
    UINT8	Time[2];
}SMC_BOARD_INFO;

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: BIOS_OOB_Identify_Set
//
// Description: Structure for BIOS OOB Identify Set.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// SubCMD                   UINT8                   0x12
// GetSet                   UINT8                   0 : Get, 1 : Set
// BoardInfo                Board_Info              Board information
// 
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
	UINT8	SubCMD;
	UINT8	GetSet;
	SMC_BOARD_INFO	BoardInfo;
}BIOS_OOB_Identify_Set;

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: BIOS_GET_OOB_FILE_STATUS_REQ
//
// Description: Structure for BIOS GET OOB DATA STATUS REQ.
//
//    SubCMD - 
//    0x09 : Bin file
//    0x19 : Smbios
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// SubCMD                   UINT8                   Sub command
// 
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
	UINT8	SubCMD;
    UINT8   FileType;
}BIOS_GET_OOB_FILE_STATUS_REQ;


//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: BIOS_GET_OOB_FILE_STATUS_RSP
//
// Description: Structure for BIOS GET OOB DATA STATUS RSP.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// FileStatus               UINT8                   The status of file existing
// "FileSize[4]"            UINT8                   File size
// 
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
	UINT8	FileStatus;
	UINT8	FileSize[4];
}BIOS_GET_OOB_FILE_STATUS_RSP;

typedef struct {
        UINT8   SubCMD;
        UINT8   GetSet;
        UINT8   FlashFlag;
}BIOS_SMBIOS_ERASE_REQ;

typedef struct {
        UINT8   SubCMD;
        UINT8   GetSet;
        UINT8   StatusFlag;
        UINT8   Reserved1; //<<< Platform ID
        UINT8   Reserved2; //<<< TPM version
        UINT8   Reserved3;
        UINT8   Reserved4;
        UINT8   Reserved5;
        UINT8   Reserved6;
        UINT8   Reserved7;
}BIOS_TPM_CAPABILITIES_REQ;

#pragma pack()

//
// SmcCspLibBmc Prototype
//
EFI_STATUS
SmcLibLocateIpmiProtocol (
    IN VOID
);

UINT8
SmcLibBmcIsPresent (
    IN VOID
);

EFI_STATUS
SmcLibBmcPostCpl (
    IN  BOOLEAN BmcPostCpl
);

UINT8* GetIPMIVideoBar(VOID);

EFI_STATUS DataToBMCPrepareData (
    IN UINT8* Buffer,
    IN UINT32 BufferLength,
    OUT UINT8* VramCheckSum
);

EFI_STATUS DataToBMCReadyNotify(
    IN	UINT8	FileType,
    IN	UINT32	FileLength,
    OUT UINT8   *ResponseStatus
);

EFI_STATUS DataToBMCDone(
    IN	UINT8	FileType,
    OUT UINT8 *ResponseStatus
);

EFI_STATUS DataToBIOSReadData (
    IN OUT UINT8* Buffer,
    IN UINT32 BufferLength
);

EFI_STATUS DataToBIOSReadyNotify(
    IN UINT8 FileType,
    OUT UINT8* RspStatus,
    OUT UINT32 *FileLength
);

EFI_STATUS DataToBIOSWait (
    IN UINT8 FileType,
    OUT UINT8* RspStatus
);

EFI_STATUS DataToBIOSDone(
    IN	UINT8	FileType,
    IN	UINT8	DownloadStatus
);

EFI_STATUS
SMC_IPMICmd30_68_1B (
  IN     UINT8   GetSet,
  IN OUT BIOS_TPM_CAPABILITIES_REQ   *CommandData
  );

EFI_STATUS
SMC_IPMICmd30_A0_12 (
  IN  UINT32                            BoardId,
  IN  UINT32                            Date,
  IN  UINT16                            Time
  );

EFI_STATUS SMC_IPMICmd30_70_70(VOID);

EFI_STATUS
SMC_IPMICmd30_A0_15(
    IN  UINT8   GetSet,
    IN OUT      UINT8   *FlashFlag
);

EFI_STATUS
GetPowerSupplyStatus(
    IN OUT	UINT8	PSU_DATA[],
    IN	UINT8	PSU_DATA_SIZE
);

EFI_STATUS
SMC_IPMICmd30_A1(
    IN  UINT8  *BMCCMDData,
    IN  UINT8  *pFRUBlockData
);

EFI_STATUS
SMC_IPMICmd30_70_DF(
    IN	UINT8	FunStatus
);

EFI_STATUS
SMC_IPMICmd30_70_3A(void);

EFI_STATUS
SMC_IPMICmd30_97(
    IN	UINT8	SmcWatchDogMode,
    IN  UINT8   SmcWatchDogTime
);

EFI_STATUS
SMC_IPMICmd30_98(void);

EFI_STATUS
SMC_IPMICmd30_20(void);

EFI_STATUS
SMC_IPMICmd30_9E(void);

EFI_STATUS
SMC_IPMICmd30_99(void);

BOOLEAN
CheckBoardNameFromFruData(void);

EFI_STATUS
SMC_IPMICmd30_AC(void);

EFI_STATUS
SendIPMIGetCPLDVersion(void);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************

