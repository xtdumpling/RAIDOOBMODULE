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

/** @file 
Ofbd.h

Header file for the Ofbd module

*/

#ifndef _EFI_OFBD_H_
#define _EFI_OFBD_H_

///
///	OFBD Handler Guid for ARM
///
/// {3346D44E-B699-46ce-9737-D26C5E78463E}
#define	OFBD_HANDLER_GUID	\
	{0x3346d44e, 0xb699, 0x46ce, 0x97, 0x37, 0xd2, 0x6c, 0x5e, 0x78, 0x46, 0x3e}

		
#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
//	On Flash Block Structure Definition
//----------------------------------------------------------------------------

#pragma pack(1)

/**
  AFRI_OFBD_BUFFER : OFBD Header structure for ARM
*/

typedef struct _AFRI_OFBD_BUFFER {
	UINT32			Command;
	UINT8			Data[1];	/// OFBD Data
} AFRI_OFBD_BUFFER;

/**
  OFBD_HDR : OFBD Header structure
*/

typedef struct 
{
    UINT32 OFBD_SIG;            ///< 'OFBD'
    UINT16 OFBD_VER;            ///< 0200
    UINT16 OFBD_Size;           ///< Ofbd buffer total size
    UINT32 OFBD_FS;             ///< On Flash Status 
    UINT16 OFBD_RS;             ///< Return Status
    UINT16 OFBD_HDR_SIZE;       ///< Header Size
    UINT16 OFBD_OS;             ///< OS environment 
}OFBD_HDR;

#pragma pack()

///
///	OFBD_FS : On Flash Status
///

#define     OFBD_FS_VC      BIT00
#define     OFBD_FS_BBLK    BIT01
#define     OFBD_FS_NVRAM   BIT02
#define     OFBD_FS_MAIN    BIT03
#define     OFBD_FS_NCB     BIT04
#define     OFBD_FS_ROMH    BIT05
#define     OFBD_FS_SFU     BIT06
#define     OFBD_FS_OA      BIT07
#define     OFBD_FS_AFU_MFG BIT08
#define     OFBD_FS_AC_BAT  BIT16
#define     OFBD_FS_GET_PAT BIT17
#define     OFBD_FS_SECURUD BIT22
#define     OFBD_FS_ERR_LOG BIT23
#define     OFBD_FS_OEM_CMD BIT24
#define     OFBD_FS_SETUP   BIT25
#define     OFBD_FS_MEUD    BIT26
#define     OFBD_FS_EC      BIT27
#define     OFBD_FS_ORI     BIT28
#define     OFBD_FS_PWD     BIT29
#define     OFBD_FS_CFG     BIT30
#define     OFBD_FS_BIOS_CFG_PRESERVE   BIT31

///
///	OFBD_RS : Return Status
///

#define OFBD_RS_SUPPORT         BIT00
#define OFBD_RS_PROTECT         BIT01
#define OFBD_RS_PROTECT_PART    BIT02
#define OFBD_RS_ERR_OUT         BIT03
#define OFBD_RS_DIS_OEMSTR      BIT04

///
///	OFBD_OS : OS environment 
///

#define OFBD_OS_DOS     BIT00
#define OFBD_OS_WIN     BIT01
#define OFBD_OS_LINUX   BIT02
#define OFBD_OS_FREEBSD BIT03
#define OFBD_OS_EFI     BIT04

#pragma pack(1)

/**
  OFBD_EXT_HDR : OFBD Extended Table Header Structure
*/

typedef struct
{
    UINT8 TypeCodeID;       ///< Type Code ID
    UINT8 NumTCStruct;      ///< Number of structure
    UINT16 TCStructSize;    ///< Number of structure size
}OFBD_EXT_HDR;

/**
  OFBD_END : OFBD End Structure
*/

typedef struct
{
    UINT32  OFBD_END;       ///< Ofbd buffer end point
}OFBD_END;

#pragma pack()

#define OFBD_EXT_TBL_END    0x0FFFF55AA

///
///	TypeCodeID : Type Code ID
///

#define OFBD_EXT_TC_ROMHOLE     0x50
#define OFBD_EXT_TC_AFUDEFCFG   0x51
#define OFBD_EXT_TC_PWD         0x52
#define OFBD_EXT_TC_ORI         0x53
#define OFBD_EXT_TC_EC          0x54
#define OFBD_EXT_TC_MEUD        0x55
#define OFBD_EXT_TC_SETUP       0x56
#define OFBD_EXT_TC_OEM_CMD     0x57
#define OFBD_EXT_TC_CLN_ERR_LOG 0x58
#define OFBD_EXT_TC_SECURE_UD   0x59
#define OFBD_EXT_TC_OA_HANDLE   0x5A
#define OFBD_EXT_TC_AC_BAT      0x5B
#define OFBD_EXT_TC_GET_PATTERN 0x5C
#define OFBD_EXT_TC_SFU         0x70
#define OFBD_EXT_TC_AFU_MFG     0x71
#define OFBD_EXT_TC_BIOS_CFG_PRESERVE 0x80
//----------------------------------------------------------------------------
//	On Flash Block Type Code Structure Section
//----------------------------------------------------------------------------

#pragma pack(1)

/**
  OFBD_TC_50_RH_STRUCT : Type Code 50 - Oem Rom Hole Checking Structure (display message only)
*/

typedef struct
{
    UINT32      ddExtCmd;   ///< Check Afu flash command
    EFI_GUID    HoleGuid;   ///< Check RomHole Guid
}OFBD_TC_50_RH_STRUCT;

/**
  OFBD_TC_51_DC_STRUCT : Type Code 51 - Afu Default Command Configuration Structure
*/

typedef struct
{
    UINT32 ddExtCfg;        ///< Cancel Afu flash command
    UINT32 ddRetSts;        ///< Add Afu flash command
}OFBD_TC_51_DC_STRUCT;

typedef struct
{
    UINT32 ddIndexCfg;      ///< Check current is Ln or Kn command
    UINT16 ddExtCfg;        ///< Add Ln or Kn command
    UINT16 ddIgnCfg;        ///< Cancel Ln or Kn command
}OFBD_TC_51_DC_EXT_STRUCT;

/**
  OFBD_TC_52_PC_STRUCT : Type Code 52 - Oem Password Check Structure
*/

typedef struct
{
    UINT16 dwPwdLen;        ///< Password Length
    UINT8 dbGetCkSts;       ///< Get Check Status
    UINT8 dbRetSts;         ///< Return Status
}OFBD_TC_52_PC_STRUCT;

/**
  OFBD_TC_53_ORI_STRUCT : Type Code 53 - Oem Rom Id Check Structure
*/

typedef struct
{
    UINT8 dwGetCkSts;           ///< Get Check Status
    UINT8 dwRetSts;             ///< Return Status
    UINT32 ddStartAddrLow;      ///< The Low part of Check Address Buffer
    UINT32 ddStartAddrHigh;     ///< The High part of Check Address Buffer
    UINT32 ddSize;              ///< The Check Address Buffer Size
}OFBD_TC_53_ORI_STRUCT;

/**
  OFBD_TC_54_STD_EC_STRUCT : Type Code 54 - AMI Standard EC Flash Structure
*/

typedef struct
{
    UINT8 dbFlaSts;         ///< EC Set & Check Status
    UINT16 dwRetSts;        ///< EC Return Status
    UINT32 ddChkSum;        ///< EC Data CheckSum
    UINT32 ddSize;          ///< EC Data Size
}OFBD_TC_54_STD_EC_STRUCT;

/**
  OFBD_TC_55_MEUD_STRUCT : Type Code 55 - Intel ME Ignition Firmware Update Structure
*/

typedef struct 
{
    UINT8   bReturnStatus;          ///< Return status of Intel ME update
    UINT8   bSubFunction;           ///< Sub function for advanced operation
    UINT32  dMERuntimeBase;         ///< File offset of ME firmware
    UINT32  dMERuntimeLength;       ///< Size of ME firmware
    UINT32  dMEBiosRegionBase;      ///< File offset of BIOS region
    UINT32  dMEBiosRegionLength;    ///< Size of BIOS region
#if (OFBD_VERSION >= 0x0210)
    UINT32  ddFlashBufOffset;       ///< Flash Buffer Offset
    UINT32  ddBlockAddr;            ///< ME firmware each block address
    UINT32  ddBlockSize;            ///< Each block size
#endif
}OFBD_TC_55_MEUD_STRUCT;

/**
  OFBD_TC_56_SETUP_STRUCT : Type Code 56 - Oem Setup/Nvram Preserve Structure
*/

typedef struct
{
    UINT8 dbGetSts;             ///< Get Status
    UINT8 dbRetSts;             ///< Return Status
}OFBD_TC_56_SETUP_STRUCT;

/**
  OFBD_TC_57_OEM_CMD_STRUCT : Type Code 57 - Oem Command Structure
*/

typedef struct 
{
    UINT16  dwFlaSts;           ///< 0: Before Flash, 1: After Flash, 2: Before End
    UINT16  dwRetSts;           ///< 0: Failure, 1: Successfully
    UINT32  ddCMD;              ///< Bit 0~15: OEM CMD
    UINT32  ddReserve;          ///< Reserve
} OFBD_TC_57_OEM_CMD_STRUCT;

/**
  OFBD_TC_58_CEL_STRUCT : Type Code 58 - Clean Event Log Structure
*/

typedef struct 
{
    UINT16  dwFunSts;           ///< 0: Do nothing 1: Clean Event Log
    UINT16  dwRetSts;           ///< 0: Failure, 1: Successfully
} OFBD_TC_58_CEL_STRUCT;

/**
  OFBD_TC_59_SECURE_UD : Type Code 59 - Oem Secure Update Structure
*/

typedef struct 
{
    UINT64      BufAddr;        ///< Buffer address
    UINT32      BlockAddr;      ///< 0 starts at 0xfff0_0000
    UINT32      BlockSize;      ///< Block Size
    UINT8       ErrorCode;      ///< Error Code returns
    UINT8       bSubFunction;   ///< Sub functions
} OFBD_TC_59_SECURE_UD;

/**
  OFBD_TC_5A_OA_HANDLE_STRUCT : Type Code 5A - Oem Oa Data Handle Structure
*/

typedef struct 
{
    UINT8   dbOASts;        ///< Bit 0: Notification BIOS, utility is now ready to update the OA data.								- OFBD_TC_OA_UPDATE_NOTIFY
                            ///< Bit 1~7: Reserved
    UINT8   dbErrorID;      ///< 0~255 : Filled by O.E.M.
    UINT16  dwRetSts;       ///< Bit 0: OA data is invalid, tell the utility stop the flash procedure                               - OFBD_RS_OA_DATA_IS_INVALID
                            ///< Bit 1: OA data has be modified, tell the utility use the new data to update                        - OFBD_RS_OA_DATA_IS_MODIFIED
                            ///< Bit 2: BIOS has updated the OA, so tell the utility doesn't to update                              - OFBD_RS_OA_UPDATE_SKIPPED
                            ///< Bit 3: BIOS doesn't allow the OA update, tell the utility stop the flash procedure                 - OFBD_RS_OA_UPDATE_DECLINED
                            ///< Bit 4~14: Reserved
                            ///< Bit 15: Use dbErrorID field for utility return OEM specific error code, when this Bit is set to 1. - OFBD_RS_OA_USE_OEM_ERROR_ID
    UINT32  ddOABlockAddr;  ///< OA Block Address of BIOS ROM (For NCB mode to use)
    UINT64  ddOADataBuffer; ///< OA Data Buffer
    UINT32  dwOADataSize;   ///< OA Data Buffer Size
} OFBD_TC_5A_OA_HANDLE_STRUCT;

/**
  OFBD_TC_5B_AC_BATTERY_STRUCT : Type Code 5B - AC/Battery Checking Structure
*/

typedef struct
{
    UINT8   dbGetSts;               ///< Get Status
    UINT8   dbRetSts;               ///< Return Status
} OFBD_TC_5B_AC_BATTERY_STRUCT;

/**
  OFBD_TC_5C_GET_PATTERN_STRUCT : Type Code 5C - Oem Get Pattern of Rom File Structure
*/

typedef struct
{
    UINT8   FunSts;                 ///< Function Status
    UINT8   RetSts;                 ///< Return Status
    UINT16  SignatureDataSize;      ///< The specific Signature data size range that you want the AFU to capture.
                                    ///< # 1. If the BIOS does not fill size range, that AFU default will get 1K data buffer size
                                    ///< # 2. Maximum size is 32 KB
                                    ///< # 3. BIOS can get data buffer pointer from OFBD_EXT_TBL_END
    UINT8   PatternSignature[64];   ///< The specific Signature value that you want the AFU to search entire Rom File
} OFBD_TC_5C_GET_PATTERN_STRUCT;

/**
  OFBD_TC_70_SFU_STRUCT : Type Code 70 - Secure Flash Update Structure
*/

typedef struct
{
    UINT8   Command;                ///< Secure Flash Command
    UINT8   Status;                 ///< Return Status
} OFBD_TC_70_SFU_STRUCT;

#pragma pack()

//=============================
// Type Code Specific Status   
//=============================

///
/// Type Code 50 Return Status 
///

#define OFBD_TC_GRS_PROTECT     BIT00

///
/// Type Code 50 Hole Description 
///

#define OFBD_TC_RH_HD_ROMHOLE   BIT00
#define OFBD_TC_RH_HD_NCB       BIT01

///
/// Type Code 51 Return Status  
///

#define OFBD_TC_CFG_P           BIT00
#define OFBD_TC_CFG_B           BIT01
#define OFBD_TC_CFG_N           BIT02
#define OFBD_TC_CFG_K           BIT03
#define OFBD_TC_CFG_E           BIT04
#define OFBD_TC_CFG_Q           BIT05
#define OFBD_TC_CFG_REBOOT      BIT06
#define OFBD_TC_CFG_X           BIT07
#define OFBD_TC_CFG_S           BIT08
#define OFBD_TC_CFG_ECUF        BIT09
#define OFBD_TC_CFG_SHUTDOWN        BIT10
#define OFBD_TC_CFG_SETUPRESERVE    BIT11
#define OFBD_TC_CFG_ROMHOLE         BIT12
#define OFBD_TC_CFG_NCB             BIT13
#define OFBD_TC_CFG_CLNEVNLOG       BIT14
#define OFBD_TC_CFG_R           BIT15
#define OFBD_TC_CFG_O           BIT16
#define OFBD_TC_CFG_GAN         BIT17
#define OFBD_TC_CFG_KN          BIT18
#define OFBD_TC_CFG_L           BIT19
#define OFBD_TC_CFG_LN          BIT20
#define OFBD_TC_CFG_A           BIT21
#define OFBD_TC_CFG_OAD         BIT22
#define OFBD_TC_CFG_CAPSULE     BIT23
#define OFBD_TC_CFG_RECOVERY    BIT24
#define OFBD_TC_CFG_CAF         BIT25
     
///
/// Type Code 52 Get Check Status 
///

#define OFBD_TC_OPC_GET         BIT00
#define OFBD_TC_OPC_CHECK       BIT01
 
///
/// Type Code 52 Return Status 
///

#define OEM_RS_PC_REQ               BIT00
#define OEM_RS_PC_CK_OK             BIT01
#define OEM_RS_PC_TIMES_EXCEEDED    BIT02

///
/// Type Code 53 Get & Check Status
///

#define OFBD_TC_ORI_GET         BIT00
#define OFBD_TC_ORI_CHECK       BIT01

///
/// Type Code 53 Return Status
///

#define OFBD_TC_ORI_REQ         BIT00
#define OFBD_TC_ORI_ROM         BIT01
#define OFBD_TC_ORI_MOD         BIT02
#define OFBD_TC_ORI_GUID        BIT03
#define OFBD_TC_ORI_CK_OK       BIT04

///
/// Type Code 54 Set Status		
///

#define OFBD_TC_STD_EC_START    BIT00
#define OFBD_TC_STD_EC_FLASH    BIT01
#define OFBD_TC_STD_EC_END      BIT02
#define OFBD_TC_ECX_SIZE        BIT03
#define OFBD_TC_ECX_ERASE       BIT04
#define OFBD_TC_ECX_VERIFY      BIT05
#define OFBD_TC_ECX_RETRY       BIT06

///
/// Type Code 54 Return Status		
///

#define OFBD_RS_EC_OK       BIT00
#define OFBD_RS_EC_CHKSUM   BIT01
#define OFBD_RS_EC_ENTRY    BIT02
#define OFBD_RS_EC_ERASE    BIT03
#define OFBD_RS_EC_INIT     BIT04
#define OFBD_RS_EC_START    BIT05
#define OFBD_RS_EC_WRITE    BIT06
#define OFBD_RS_EC_EXIT     BIT07
#define OFBD_RS_EC_CHIPID   BIT08

///
/// Type Code 55 SubFunction  
///

#define OFBD_TC_MEUD_GETINFO        0
#define OFBD_TC_MEUD_START          BIT00
 
///
/// Type Code 55 Return Status 
///

#define OFBD_TC_MEUD_OK             BIT00
#define OFBD_TC_MEUD_ERROR          BIT01

///
/// Type Code 56 Get Check Status 
///

#define OFBD_TC_SETUP_SUPPORT       BIT00
#define OFBD_TC_SETUP_STORE         BIT01
#define OFBD_TC_SETUP_RESTORE       BIT02
#define OFBD_TC_DMI_STORE           BIT03
#define OFBD_TC_DMI_RESTORE         BIT04
 
///
/// Type Code 56 Return Status 
///

#define OFBD_TC_SETUP_OK            BIT00
#define OFBD_TC_SETUP_NOT_SUPPORT   BIT01
#define OFBD_TC_SETUP_STORE_ERROR   BIT02
#define OFBD_TC_SETUP_RESTORE_ERROR BIT03
#define OFBD_TC_DMI_STORE_ERROR     BIT04
#define OFBD_TC_DMI_RESTORE_ERROR   BIT05

///
/// Type Code 57 Flash Status
///

#define OFBD_TC_OEM_CMD_BF     1
#define OFBD_TC_OEM_CMD_AF     2
#define OFBD_TC_OEM_CMD_BE     3
#define OFBD_TC_OEM_CMD_SF     4
#define OFBD_TC_OEM_CMD_BUF    5
#define OFBD_TC_OEM_CMD_BUF_SF 6
#define OFBD_TC_OEM_CMD_BUF_AF 7
#define OFBD_TC_OEM_CMD_BUF_BE 8

///
/// Type Code 57 Return Status
///

#define OFBD_TC_OEM_CMD_OK              BIT00
#define OFBD_TC_OEM_CMD_NOT_SUPPORT     BIT01
#define OFBD_TC_OEM_CMD_ERROR           BIT02

///
/// Type Code 58 Func Status 
///

#define OFBD_TC_CEL_FUNC_REQ            BIT00

///
/// Type Code 58 Return Status 
///

#define OFBD_TC_CEL_OK                  BIT00
#define OFBD_TC_CEL_ERROR               BIT01
#define OFBD_TC_CEL_BIOS_NOT_SUPPORT    BIT02

///
/// Type Code 59 SubFunction  
///

#define OFBD_TC_SECUREUD_LOAD2SMM       BIT00
#define OFBD_TC_SECUREUD_VERIFY_SMM     BIT01
#define OFBD_TC_SECUREUD_DONE           BIT02

///
/// Type Code 5A Func Status 
///

#define OFBD_TC_OA_UPDATE_NOTIFY        BIT00

///
/// Type Code 5A Return Status 
///

#define OFBD_RS_OA_DATA_IS_INVALID      BIT00
#define OFBD_RS_OA_DATA_IS_MODIFIED     BIT01
#define OFBD_RS_OA_UPDATE_SKIPPED       BIT02
#define OFBD_RS_OA_UPDATE_DECLINED      BIT03
#define OFBD_RS_OA_USE_OEM_ERROR_ID     BIT15

///
/// Type Code 5B Func Status 
///

#define OFBD_TC_AC_BAT_INFO_REQ         BIT00

///
/// Type Code 5B Return Status 
///

#define OFBD_RS_AC_BAT_OK               BIT00
#define OFBD_RS_AC_BAT_ERR              BIT01

///
/// Type Code 5C Func Status 
///

#define OFBD_TC_GET_PAT_REQ             BIT00
#define OFBD_TC_GET_PAT_CHK             BIT01
#define OFBD_TC_GET_PAT_NOT_FOUND       BIT02
#define OFBD_TC_GET_PAT_LENGTH_CHECK  	BIT07

///
/// Type Code 5C Return Status 
///

#define OFBD_RS_GET_PAT_OK              BIT00
#define OFBD_RS_GET_PAT_ERROR           BIT01

///
/// Type Code 70 Get & Set Status     
///

#define OFBD_TC_SFU_GET_FLASH_INFO              BIT00
#define OFBD_TC_SFU_LOAD_FIRMWARE_IMAGE         BIT01
#define OFBD_TC_SFU_GET_FLASH_UPDATE_POLICY     BIT02
#define OFBD_TC_SFU_SET_FLASH_UPDATE_METHOD     BIT03
#define OFBD_TC_SFU_RUNTIME_FLASH               BIT04

///
/// Type Code 70 Return Status   
///

#define OFBD_TC_SFU_OK                                  BIT00
#define OFBD_TC_SFU_NOT_SUPPORTED                       BIT01
#define OFBD_TC_SFU_FW_AUTH_FAILED                      BIT02
#define OFBD_TC_SFU_INVALID_FW_CAPSULE_HDR              BIT03
#define OFBD_TC_SFU_SIGNING_KEYS_DONT_MATCH             BIT04
#define OFBD_TC_SFU_UNSUPPORTED_CERTIFICATE_HDR_FORMAT  BIT05

///
/// Type Code 71 Return Status   
///

#define OFBD_TC_AFU_MFG_OK          BIT00
#define OFBD_TC_AFU_MFG_ERROR       BIT01

///
/// This is Ofbd In Smm Function Prototype Define
///

typedef VOID (OFBD_INIT_SMM_FUNC) (VOID);

///
/// This is Ofbd Init Parts Function Prototype Define
///

typedef VOID (OFBD_INIT_PARTS_FUNC) (IN VOID *pOFBDBuffer, IN OUT UINT8 *pOFBDDataHandled);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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
