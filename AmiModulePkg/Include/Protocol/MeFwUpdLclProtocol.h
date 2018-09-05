#include <Protocol/AMIPostMgr.h>

//{ DCA334AB-56E3-4EDE-B9B3-8EAE2ACF5E78 }
#define ME_FW_UPD_LOCAL_PROTOCOL_GUID \
    { 0xDCA334AB, 0x56E3, 0x4EDE, 0xB9, 0xB3, 0x8E, 0xAE, 0x2A, 0xCF, 0x5E, 0x78 }
//{ 9F8B1DEF-B62B-45F3-8282-BFD7EA19801B }
#define ME_FW_CAPSULE_FIRMWARE_VOLUME_GUID \
    { 0x9F8B1DEF, 0xB62B, 0x45F3, 0x82, 0x82, 0xBF, 0xD7, 0xEA, 0x19, 0x80, 0x1B }
//{ 71C7671C-AC53-463F-BFEE-770FD76E7B4C }
#define ME_FW_UPD_VARIABLE_GUID \
    { 0x71C7671C, 0xAC53, 0x463F, 0xBF, 0xEE, 0x77, 0x0F, 0xD7, 0x6E, 0x7B, 0x4C }

#define MEFW_LAST_ATTEMPT_NAME  L"MeFwLastAttempt"

#pragma pack(push, 1)

typedef struct _ME_FW_UPD_VERSION {
  UINT32    MajorVersion : 16;
  UINT32    MinorVersion : 16;
  UINT32    HotfixVersion : 16;
  UINT32    BuildVersion : 16;
} ME_FW_UPD_VERSION;

typedef struct {
  UINT32    Version;
  UINT32    Status;
} FW_LAST_ATTEMPT_STATE;

#pragma pack(pop)

typedef enum {
  MeModeNormal,
  MeModeDebug,
  MeModeTempDisabled,
  MeModeSecOver,
  MeModeFailed
} ME_FW_STATE;

typedef enum {
  ME_FIRMWARE, 
  ISH_FIRMWARE, 
  PDT_DATA,
  MAX_FIRMWARE_TYPE = 0xff
} FIRMWARE_TYPE;

typedef struct _ME_FW_UPDATE_LOCAL_PROTOCOL  ME_FW_UPDATE_LOCAL_PROTOCOL;

typedef 
EFI_STATUS
(EFIAPI *ME_FW_UPD_LOCAL_FUNC) (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT8                       *FileBuffer,
    IN  UINTN                       FileLength,
    IN  UINT8                       FirmwareType
);

typedef 
EFI_STATUS
(EFIAPI *ME_FW_CHECK_MODE) (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  ME_FW_STATE                 MeFwState
);

typedef 
EFI_STATUS
(EFIAPI *ME_FW_CHECK_VERSION) (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  ME_FW_UPD_VERSION           *MeFwVerion
);

typedef 
EFI_STATUS
(EFIAPI *ME_MEI_HMRFPO_ENABLE) (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This
);

typedef 
EFI_STATUS
(EFIAPI *PLATFORM_RESET) (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  EFI_RESET_TYPE              ResetType
);

typedef 
EFI_STATUS
(EFIAPI *PLATFORM_UNLOCK) (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT32                      UnlockType OPTIONAL
);

typedef struct _ME_FW_UPDATE_LOCAL_PROTOCOL {
    EFI_PHYSICAL_ADDRESS        FwUpdBufferAddress;
    UINTN                       FwUpdBufferLength;
    ME_FW_UPD_LOCAL_FUNC        FwUpdLcl;
    ME_FW_CHECK_MODE            MeFwCheckMode;
    ME_FW_CHECK_VERSION         MeFwGetVersion;
    ME_MEI_HMRFPO_ENABLE        HmrfpoEnable;
    PLATFORM_RESET              PlatformReset;
    PLATFORM_UNLOCK             PlatformUnlock;
    AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr;
};