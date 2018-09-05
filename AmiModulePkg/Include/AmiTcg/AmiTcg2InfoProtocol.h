#ifndef _AMI_TCG2_INFO_PROTOCOL_H_
#define _AMI_TCG2_INFO_PROTOCOL_H_

#include <Uefi.h>
#include <Token.h>

#pragma pack (1)

typedef struct
{
    UINTN  Size;                    //size of the structure including the size field
    UINT8  TpmDeviceType;           //0: No_TPM 1:TPM=TPM_1_2, 2:TPM_2_0, 3: TCM1_0
    UINT8  TpmFwUpdateInterface;    //0: None, 1:FirmwareUpdate, 2:BiosSetting, 3:OemMethod
    UINT32 TpmFwVersion1;     
    UINT32 TpmFwVersion2;           //if applicable
    UINT32 Tpm2Vendor;              //if available
    UINT16 TpmPCRBanks;             //BIT 1:SHA-1, BIT2:SHA256, BIT3:SHA-384, BIT4:SHA-512, BIT5: SM3
    UINT8  TpmState;                //0:Disabled, 1:Enabled (for TPM 2.0: if SH is disabled and EH is 
                                    //disabled: TPM is disabled, if SH and EH is enabled TPM is enabled)

} AMI_TCG2_TPM_INFO_STRUCT;

#pragma pack ()


struct _AMI_TCG2_INFO_PROTOCOL;

typedef struct _AMI_TCG2_INFO_PROTOCOL AMI_TCG2_INFO_PROTOCOL;


#define AMI_TCG2_INFO_PROTOCOL_GUID \
        {0x325a554, 0x5be, 0x466b, 0xbc, 0x8c, 0x70, 0xbe, 0x3c, 0x9d, 0xaf, 0xb1}


typedef 
EFI_STATUS
(EFIAPI *AMI_TCG2_GET_TPM_INFO) (
  IN CONST AMI_TCG2_INFO_PROTOCOL *This,
  OUT  AMI_TCG2_TPM_INFO_STRUCT	    **info
);


struct _AMI_TCG2_INFO_PROTOCOL {
  UINT32                   Version;
  AMI_TCG2_GET_TPM_INFO    getInfo;
};


#endif
