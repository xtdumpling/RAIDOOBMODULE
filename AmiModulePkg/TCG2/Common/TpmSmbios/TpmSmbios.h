//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file TpmSmbios.h
    Header file for TpmSmbios module

**/

#ifndef __TPM_SMBIOS_H__
#define __TPM_SMBIOS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/TrEEProtocol.h>
#include <Protocol/TcgService.h>
#include <AmiTcg/TCGMisc.h>
#include <AmiTcg/AmiTcg2InfoProtocol.h>
#include <AmiTcg/TcgEFI12.h>
#include <AmiTcg/TcgCommon20.h>

///
/// Non-static SMBIOS table data to be filled later with a dynamically generated value
///
#define TO_BE_FILLED  0
#define TO_BE_FILLED_STRING  "  "        ///< Initial value should not be NULL


#define EFI_SMBIOS_TYPE_TPM_DEVICE		43

///
/// TPM Capabilities Vendor ID (CAP_ID)
///
#define AMD_CAP_ID  		0x414D4400     ///AMD vendor chip
#define ATMEL_CAP_ID  		0x41544D4C     ///Atmel vendor chip
#define BROADCOM_CAP_ID  	0x4252434D     ///Broadcom vendor chip
#define IBM_CAP_ID  		0x49424D00     ///IBM vendor chip
#define INFINEON_CAP_ID  	0x494D5800     ///Infineon vendor chip
#define INTEL_CAP_ID  		0x494E5443     ///Intel vendor chip
#define LENOVO_CAP_ID  		0x4C454E00     ///Lenovo vendor chip
#define NATIONAL_SEMI_CAP_ID  	0x4E534D20     ///National Semiconductor vendor chip
#define NATIONZ_CAP_ID  	0x4E545A00     ///Nationz vendor chip
#define NUVOTON_CAP_ID  	0x4E544300     ///Nuvoton Technology vendor chip
#define QUALCOMM_CAP_ID  	0x51434F4D     ///Qualcomm vendor chip
#define SMSC_CAP_ID  		0x534D534     ///SMSC vendor chip
#define ST_CAP_ID  		0x53544D20     ///ST Microelectronics vendor chip
#define SAMSUNG_CAP_ID 		0x534D534E     ///Samsung vendor chip
#define SINOSUN_CAP_ID 		0x534E5300     ///Sinosun vendor chip
#define TI_CAP_ID 		0x54584E00     ///Texas Instruments vendor chip
#define WINBOND_CAP_ID 		0x57454300     ///Winbond vendor chip

#define TPM_1_2_MAJOR_SPEC	0x1;
#define TPM_1_2_MINOR_SPEC	0x2;
#define TPM_2_0_MAJOR_SPEC	0x2;
#define TPM_2_0_MINOR_SPEC	0x0;

///
/// Smbios Table Entry Point Structure.
///
#pragma pack(1)

typedef struct {
    UINT32   ID;
    CHAR8    *String;
} TPM_VENDOR_STRING;

typedef struct {
	UINT64      Resrved1        :   2;  //bit 0..1 
	UINT64      Unsupport       :   1;  //bit 2
	UINT64      ConfigFirmware  :   1;  //bit 3
	UINT64      ConfigSoftware  :   1;  //bit 4
	UINT64      ConfigOem       :   1;  //bit 5
	UINT64      Resrved2        :   58; //bit 6:63
} TPM_SMBIOS_CHAR;

///
/// TPM Device (Type 43).
///
typedef struct {
  SMBIOS_STRUCTURE	  Hdr;
  UINT32              VendorId;
  UINT8               MajorSpecVer;   
  UINT8               MinorSpecVer;
  UINT32              FirmwareVer1;
  UINT32              FirmwareVer2;
  UINT8               Description;
  TPM_SMBIOS_CHAR     Characteristics;
  UINT32              OemDefined;  
} SMBIOS_TABLE_TYPE43;

///
/// TPM1.2 Version Info 
///
typedef struct {
    UINT16 Tag;
    UINT32 ParamSize;
    UINT32 RetCode;
    UINT32 respSize;
    UINT16 tag;
    UINT32 Version;
    UINT16 specLevel;
    UINT8  errataRev;
    UINT32 VendorId;
} TPM12_CapabilitiesVersionInfo;

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif // __TPM_SMBIOS_H__

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
