//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
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
  SmiFlash SMM Driver Sub-Procedure File.
**/

//----------------------------------------------------------------------
// header includes
#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/SmiFlash.h>
#include <Setup.h>
#include <Protocol/FlashProtocol.h>
#include <SMIFlashELinks.h>
#include <ImageAuthentication.h>
#include <TimeStamp.h>
#if AMIUSB_SUPPORT == 1
#include <Protocol/AmiUsbController.h>
extern EFI_GUID gAmiUsbSmmProtocolGuid;
#endif  // #if AMIUSB_SUPPORT == 1

//----------------------------------------------------------------------
// component MACROs
#define DEFAULT_VARIABLE_NAME_SIZE     100

#define AMITSESETUP_GUID \
{ 0xc811fa38, 0x42c8, 0x4579, 0xa9, 0xbb, 0x60, 0xe9, 0x4e, 0xdd, 0xfb, 0x34 }
#define EFI_USB_PROTOCOL_GUID \
{ 0x2ad8e2d2, 0x2e91, 0x4cd1, 0x95, 0xf5, 0xe7, 0x8f, 0xe5, 0xeb, 0xe3, 0x16 }
#ifndef AMI_SECURE_BOOT_SETUP_VAR 
#define AMI_SECURE_BOOT_SETUP_VAR  L"SecureBootSetup"
#endif  // #ifndef AMI_SECURE_BOOT_SETUP_VAR 

#pragma pack(1)
typedef struct {
    CHAR16      *BootVarName;
    EFI_GUID    Guid;
    UINT32      Attrib;
    UINTN       Size;
    UINT8       *Data;
    VOID        *NextData;
} SAVED_VAR;

typedef struct {
    CHAR16      *Name;
    EFI_GUID    Guid;
    UINT8       IsAuthVars;
    VOID        *pVariable;
} NVAR_LIST;

// Replace WIN_CERTIFICATE_UEFI_GUID_1 with WIN_CERTIFICATE_EFI_PKCS1_15 for
// avoiding to include AmiCertificate.h of AmiCryptoPkg.
typedef struct {
    EFI_TIME                            TimeStamp;
    WIN_CERTIFICATE_EFI_PKCS1_15         AuthInfo;
} AMI_EFI_VARIABLE_AUTHENTICATION_2;

#pragma pack()

//----------------------------------------------------------------------
// Module defined global variables
extern FLASH_PROTOCOL *Flash;

#if !defined _OUTSIDE_SMM_
//*********** INSIDE SMM ***********************************************
#if defined EMUL6064_SUPPORT && (EMUL6064_SUPPORT == 1)
#include <Protocol/Emul6064Trap.h>
EFI_EMUL6064TRAP_PROTOCOL *gEmulationTrap = NULL;
#endif

#if AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER >= 10
API_FUNC gUsbRtKbcAccessControl = NULL;
#else
EFI_KBC_ACCESS_CONTROL gUsbRtKbcAccessControl = NULL;
#endif
#endif

EFI_FFS_FILE_STATE  NvramFFSState;
EFI_FFS_FILE_STATE  *NvramFFSStatePtr;
SAVED_VAR           *gRestoreVarList = NULL;

NVAR_LIST FixedSaveVarsList[] = {
    SMIFLASH_SAVE_VARS_LIST
    {NULL, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, FALSE, NULL}
};

EFI_GUID SMIFlashSaveGuidsList[] = {
    SMIFLASH_SAVE_GUIDS_LIST
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

typedef EFI_STATUS (*SHOW_BOOT_TIME_VARIABLES)(BOOLEAN Show);
typedef struct{
    SHOW_BOOT_TIME_VARIABLES ShowBootTimeVariables;
} AMI_NVRAM_CONTROL_PROTOCOL;

AMI_NVRAM_CONTROL_PROTOCOL *NvramControl;
UINT8       gKbcPortData;
//----------------------------------------------------------------------
// externally defined variables

//----------------------------------------------------------------------
// Function definitions
/**
 * Procedure to locates the Nvram Control Protocol for enabling Boot Time Variable search.
 *
 * @retval AMI_NVRAM_CONTROL_PROTOCOL* Nvram Control Protocol pointer.
 * @retval NULL Nvram Control Protocol not located.
 */
AMI_NVRAM_CONTROL_PROTOCOL *LocateNvramControlSmmProtocol(VOID){
    static EFI_GUID gAmiNvramControlProtocolGuid = \
    { 0xf7ca7568, 0x5a09, 0x4d2c, { 0x8a, 0x9b, 0x75, 0x84, 0x68, 0x59, 0x2a, 0xe2 } };
    return GetSmstConfigurationTable(&gAmiNvramControlProtocolGuid);
}

/**
 * Procedure to update the authentication Header of Variable.
 *
 * @param pVar Pointer to the Variable.
 */
#if defined SecureBoot_SUPPORT && SecureBoot_SUPPORT == 1
#if defined SECURE_BOOT_MODULE_REVISION && SECURE_BOOT_MODULE_REVISION < 14
static VOID FillAuthHeader(
    UINT8*  pVar
)
{
    AMI_EFI_VARIABLE_AUTHENTICATION_2 *AuthHdr2;
    EFI_GUID        EfiCertTypePkcs7Guid = EFI_CERT_TYPE_PKCS7_GUID;
    EFI_TIME        EfiTime = { FOUR_DIGIT_YEAR_INT, \
                                TWO_DIGIT_MONTH_INT, \
                                TWO_DIGIT_DAY_INT, \
                                TWO_DIGIT_HOUR_INT, \
                                TWO_DIGIT_MINUTE_INT, \
                                TWO_DIGIT_SECOND_INT, \
                                0,0,0,0,0};

    AuthHdr2 = (AMI_EFI_VARIABLE_AUTHENTICATION_2*)pVar;
    MemCpy (&AuthHdr2->TimeStamp, &EfiTime, sizeof (EFI_TIME));
    AuthHdr2->AuthInfo.Hdr.dwLength = sizeof(WIN_CERTIFICATE_EFI_PKCS1_15);
    AuthHdr2->AuthInfo.Hdr.wRevision = 0x200;
    AuthHdr2->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
    AuthHdr2->AuthInfo.HashAlgorithm = EfiCertTypePkcs7Guid;
    return;
}  
#endif  // #if defined SECURE_BOOT_MODULE_REVISION && SECURE_BOOT_MODULE_REVISION < 14
#endif  // #if defined SecureBoot_SUPPORT && SecureBoot_SUPPORT == 1

/**
 * Procedure to check whether it's a Boot LoadOption variable.
 *
 * @param Name Variable Name Pointer.
 * @param Guid Variable GUID Pointer.
 * @param Data Variable Data Pointer.
 */
BOOLEAN
IsEfiBootVarsValid (
    IN CHAR16               *Name,
    IN EFI_GUID             *Guid,
    IN VOID                 *Data
)
{
    CHAR16              *p;
    EFI_GUID            BootVarGuid = EFI_GLOBAL_VARIABLE;
    
    // Check for EFI Boot Variable.
    if (Data == NULL) {
        // Validate Boot Variable Name "BootXXXX"
        if (MemCmp((VOID*)Name, L"Boot", StrLen(L"Boot"))) return FALSE;
        if (guidcmp(Guid, &BootVarGuid)) return FALSE;
        if (StrLen(Name) != StrLen(L"BootXXXX")) return FALSE;
        for (p = Name + 4; p < (Name + 8); p++) {
            if (((*p >= L'0') && (*p <= L'9')) || ((*p >= L'A') && (*p <= L'F')))
                return TRUE;
        }
    } else {
        // Skip Attrib, Length and Description.
        for(p = (CHAR16*)Data + 3; *p != 0 ; p++);
        // if the device path is HDD + File , assume this BOOTxxxx is for EFI OS.
        if ((*(++p) == 0x0104) && (*(p + (*(p + 1) / 2)) == 0x0404)) return TRUE;
    }        
    return FALSE;
}

/**
 * Procedure to preserve NVRAM Variables before re-flashing.
 *
 */
VOID PreserveVariables(VOID)
{
    UINTN               VarNameSize, VariableSize, ExtAuthHdrSize, Result;
    UINTN               MaxVarNameSize;
    CHAR16              *VarName = NULL, *OldVarName = NULL;
    UINT8               *VariableData, *pData, i, j;
    UINT32              VariableAttr;
    EFI_GUID            VarGuid;
    EFI_STATUS          Status;
    SAVED_VAR           *pLastVar = NULL;

    // Locate a big enough buffer for following process.
    VarNameSize = MaxVarNameSize = DEFAULT_VARIABLE_NAME_SIZE;
    Status = pSmst->SmmAllocatePool (
        EfiRuntimeServicesData,
        VarNameSize, &VarName
    );
    if(EFI_ERROR(Status)) return;

    // Set Show BootTime Variable flag for Searvhing Win8 Debug Variables.
    NvramControl = LocateNvramControlSmmProtocol();
    if (NvramControl) NvramControl->ShowBootTimeVariables(TRUE);

    // Get frist variable
    VarName[0] = 0;
    do{
        VarNameSize = MaxVarNameSize;
        Status = pRS->GetNextVariableName (&VarNameSize, VarName, &VarGuid);

        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            // Default Buffer is not enough, allocate new enough buffer.
            OldVarName = VarName;
            Status = pSmst->SmmAllocatePool (
                          EfiRuntimeServicesData,
                          VarNameSize, &VarName);

            if(EFI_ERROR(Status)) {
                // Disabled Searching Boot Time Variables
                if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
                return;
            }

            // Update last Variable Name to New Buffer for continuing search.    
            MemCpy(VarName, OldVarName, MaxVarNameSize);
                
            // Free old working buffer and update Max Buffer size.    
            pSmst->SmmFreePool(OldVarName);
            MaxVarNameSize = VarNameSize;
            continue;
        }
        if(!EFI_ERROR(Status))
        {
            // 1. Check By Fixed Save Variables list .
            for(i = 0, Result = -1, ExtAuthHdrSize = 0; \
                                FixedSaveVarsList[i].Name != NULL; i++) {
                                        
                Result = MemCmp((VOID*)FixedSaveVarsList[i].Name, \
                                                    (VOID*)VarName, VarNameSize);
                if (Result == 0) {
                    Result = guidcmp(&FixedSaveVarsList[i].Guid, &VarGuid);
                    if (Result != 0) continue;
                    if (FixedSaveVarsList[i].IsAuthVars == TRUE) 
                        ExtAuthHdrSize = sizeof(AMI_EFI_VARIABLE_AUTHENTICATION_2);
                    break;
                }    
            }
            
            // 2. Check By Save Guids list .
            if (Result != 0) {
                for(j = 0; SMIFlashSaveGuidsList[j].Data1 !=  0; j++) {
                    Result = guidcmp(&SMIFlashSaveGuidsList[j], &VarGuid);
                    if (Result == 0) break;
                }
            }
            if (Result != 0)
            {
#if PRESERVE_EFIBOOTORDER
                // Check the valid Boot Order Name "BootXXXX".
                if (!IsEfiBootVarsValid(VarName, &VarGuid, NULL))
#endif  //  #if PRESERVE_EFIBOOTORDER
                {
                    continue;
                }
            }

            VariableSize = 0;
            Status = pRS->GetVariable(VarName, &VarGuid,
                                      &VariableAttr, &VariableSize, NULL);

            if(Status != EFI_BUFFER_TOO_SMALL) continue;

            Status = pSmst->SmmAllocatePool (
                          EfiRuntimeServicesData,
                          VariableSize + ExtAuthHdrSize, &VariableData);

            if(EFI_ERROR(Status)) continue;

            Status = pRS->GetVariable(VarName, &VarGuid, \
                            &VariableAttr, &VariableSize, \
                            VariableData + ExtAuthHdrSize);

            if(EFI_ERROR(Status))
            {
                pSmst->SmmFreePool(VariableData);
                continue;
            }
            
//-#if PRESERVE_EFIBOOTORDER
//-            // check the BootXXXX contains "Windows Boot Manager" for EFI OS.
//-            if (IsEfiBootVars == TRUE) {
//-                if (!IsEfiBootVarsValid(NULL, NULL, (VOID*)VariableData)) {
//-                    pSmst->SmmFreePool (VariableData);
//-                    continue;
//-                }
//-                // Reset "IsEfiBootVars" for next variable check.
//-                IsEfiBootVars = FALSE;
//-            }    
//-#endif  // #if PRESERVE_EFIBOOTORDER

            // Store variable in Link list
            Status = pSmst->SmmAllocatePool (
                          EfiRuntimeServicesData,
                          sizeof(SAVED_VAR), &pData);

            if(EFI_ERROR(Status))
                continue;

            Status = pSmst->SmmAllocatePool (
                          EfiRuntimeServicesData,
                          VarNameSize, &OldVarName);

            if(EFI_ERROR(Status))
                continue;

            MemCpy(OldVarName, VarName, VarNameSize);

            MemCpy(&((SAVED_VAR*)pData)->Guid, &VarGuid, sizeof(EFI_GUID));

            ((SAVED_VAR*)pData)->BootVarName = OldVarName;
            ((SAVED_VAR*)pData)->Attrib = VariableAttr;
            ((SAVED_VAR*)pData)->Size = VariableSize + ExtAuthHdrSize;
            ((SAVED_VAR*)pData)->Data = VariableData;
            ((SAVED_VAR*)pData)->NextData = NULL;

            if (FixedSaveVarsList[i].Name != NULL) {
#if defined SecureBoot_SUPPORT && SecureBoot_SUPPORT == 1
#if defined SECURE_BOOT_MODULE_REVISION && SECURE_BOOT_MODULE_REVISION < 14
                // Saved variable pointer to FixedSavedVarsList table in order 
                // to restore Secure Boot Variables by order (dbx->db->kek->pk).
                if (FixedSaveVarsList[i].IsAuthVars == TRUE) 
                    FillAuthHeader (VariableData);
#endif  // #if defined SECURE_BOOT_MODULE_REVISION && SECURE_BOOT_MODULE_REVISION < 14
#endif  // #if defined SecureBoot_SUPPORT && SecureBoot_SUPPORT == 1
                FixedSaveVarsList[i].pVariable = (UINT8*)pData;
            }
            else    
            {
                // Init Variable List
                if (gRestoreVarList == NULL) {
                    gRestoreVarList = (SAVED_VAR*)pData;
                    pLastVar = NULL;
                }    
                if (pLastVar != NULL) pLastVar->NextData = (VOID*)pData;
                pLastVar = (SAVED_VAR*)pData;
            }
            // Reset ExtAuthHdrSize for next search..
            ExtAuthHdrSize = 0; 
        }
        //If any other error, break for avoiding dead loop
        if(EFI_ERROR(Status)&&(Status!=EFI_BUFFER_TOO_SMALL))
            break;

    }while(Status != EFI_NOT_FOUND);
    // Disabled Searching Boot Time Variables
    if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
    pSmst->SmmFreePool(VarName);
}

/**
 * Procedure to restore the preserved Variables after re-flashing.
 *
 */
VOID RestoreVariables (VOID)
{
    SAVED_VAR           *TempPoint;
    UINTN               Index = 0;

    if (NvramControl) NvramControl->ShowBootTimeVariables(TRUE);
    while ((FixedSaveVarsList[Index].Name != NULL) || (gRestoreVarList != NULL)) {
    
        // In order to restore Secure Boot Variables by order (dbx->db->kek->pk).
        if (FixedSaveVarsList[Index].Name != NULL) {
            if (FixedSaveVarsList[Index].pVariable == NULL) {Index++; continue;}
            TempPoint = (SAVED_VAR*)FixedSaveVarsList[Index].pVariable;
            FixedSaveVarsList[Index++].pVariable = NULL;
        } else if (gRestoreVarList != NULL) {
            TempPoint = gRestoreVarList;
            gRestoreVarList = TempPoint->NextData;
        } else return;          // This condition should not happen.

/*      **** System hangs if use GetVariable call, Mark out for now. ****

        // Allocate memory for read variable back before update.
        VariableSize = TempPoint->Size;
        Status = pSmst->SmmAllocatePool (EfiRuntimeServicesData,
                                              VariableSize, &VariableData);
        if (!EFI_ERROR(Status)) {
            // Read variable back before update.
            Status = pRS->GetVariable (  TempPoint->BootVarName,
                                         &TempPoint->Guid,
                                         NULL,
                                         &VariableSize,
                                         &VariableData );
            if (!EFI_ERROR(Status) && (VariableSize == sizeof(AMITSESETUP))) {
                // check for skip update if variable contents not changed.
                if (MemCmp(VariableData, TempPoint->Data, VariableSize)) 
                    Status = EFI_NOT_FOUND;
            }
            // Free read variable memory
            pSmst->SmmFreePool(VariableData);
        }
        if (Status == EFI_NOT_FOUND) 
        **** System hangs if use GetVariable call, Mark out for now. *****/
        {
#if PRESERVE_PASSWORDS
            // Restore SilentBoot to Default setting if AMITSE Setup variable.
            if (!MemCmp((VOID*)TempPoint->BootVarName, \
                            L"AMITSESetup", StrLen(L"AMITSESetup"))) {
                ((AMITSESETUP*)TempPoint->Data)->AMISilentBoot = DEFAULT_QUIET_BOOT;
            }
#endif  // #if PRESERVE_PASSWORDS

            pRS->SetVariable (TempPoint->BootVarName, &TempPoint->Guid, \
                        TempPoint->Attrib, TempPoint->Size, TempPoint->Data );
        }
        pSmst->SmmFreePool(TempPoint->BootVarName);
        pSmst->SmmFreePool(TempPoint->Data);
        pSmst->SmmFreePool(TempPoint);
    }    
    if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
}

/**
 * Procedure to re-enable the BIOS Write Enable bit before Write/Erase procedures 
 * if BIOS_LOCK_ENABLE is set. 
 *
 */
VOID 
BiosLockEnablePatchHook (
    IN UINT8            SwSmiNum,
    IN OUT UINT64       Buffer
)
{
 /*   if ((SwSmiNum != 0x22) && (SwSmiNum != 0x23))return; 
    ChipsetFlashDeviceWriteEnable ();*/
}    

#ifdef FAULT_TOLERANT_NVRAM_UPDATE
#if FAULT_TOLERANT_NVRAM_UPDATE
/**
 * Procedure to get the Main NVRAM state.
 *
 */
EFI_FFS_FILE_STATE*
GetNvramMainFFSStats(
    VOID
)
{
    EFI_FFS_FILE_HEADER *FfsFileHeader;
    EFI_FIRMWARE_VOLUME_HEADER *FwVolHeader;
    
    FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *)NVRAM_ADDRESS;
    
    if (FwVolHeader->FvLength == NVRAM_SIZE)
    {
        FfsFileHeader = (EFI_FFS_FILE_HEADER *)(
                            NVRAM_ADDRESS + FwVolHeader->HeaderLength
                        );
        return &FfsFileHeader->State;
    }else
        return NULL;
}

/**
 * Procedure to check the NVRAM state.
 *
 */
VOID
CheckNVRAMArea(
    VOID
)
{
    NvramFFSState = *GetNvramMainFFSStats();
    NvramFFSStatePtr = GetNvramMainFFSStats();
}

/**
 * Procedure to Check Location of NVRAM region.
 *
 */
BOOLEAN
NVRAMChanged(
    VOID
)
{
    UINTN FVSignOffset = 0x28, NVRAMRegion = NVRAM_ADDRESS;
    UINT32 FVSig;

    // Check NVRAM_ADDRESS still NVRAM region
    // Can we find signature ??
    Flash->Read((UINT8*)(NVRAMRegion + FVSignOffset),sizeof(FVSig), &FVSig);
//    if(FVSig != FV_SIGNATURE)
//        return TRUE;

    // The first Data is NVAR ??
    // Simple verify.
    Flash->Read((UINT8*)(NVRAMRegion + NVRAM_HEADER_SIZE),sizeof(FVSig), &FVSig);
    if(FVSig != 'NVAR')
        return TRUE;

    return FALSE;
}


/**
 * Procedure to update Backup NVRAM area if NVRAM is working in Backup.
 *
 */
VOID
UpdateNVRAMArea(
    VOID
)
{
    UINT32              i = 0, Buffer32 = 0;
    UINT8*              Buffer;
    EFI_FFS_FILE_STATE  NewNVRamFFSState;
    EFI_STATUS          Status;
    UINTN               NumberOfPages;
    EFI_PHYSICAL_ADDRESS   Phy_Address;

    // Check NVRAM_ADDRESS FFS State
    // If Status changed.
    // Then, we got an NVRAM area update
    // and current NVRam working at NVRAM_BACKUP_ADDRESS!!
    // So move the NVRAM_MAIN to NVRAM_BACKUP and Restore NVRAM_MAIN State.
    Flash->Read((UINT8*)NvramFFSStatePtr, 
                sizeof(EFI_FFS_FILE_STATE), &NewNVRamFFSState);
    // Before update FFSState to new NVRAM, check ROMLAYOUT changed or not.
    if(NVRAMChanged())
        return;
    if((NvramFFSState == NewNVRamFFSState) || (NvramFFSState == 0))
        return;
    NumberOfPages = ((NVRAM_SIZE) >> 12);
    Status = pSmst->SmmAllocatePages(AllocateAnyPages, EfiRuntimeServicesData, NumberOfPages, &Phy_Address);
    Buffer = (UINT8*)Phy_Address;

    // NVRAM updated !!
    // Move NVRAM Main to NVRAM Backup
    Flash->Erase((UINT8*)NVRAM_BACKUP_ADDRESS, NVRAM_SIZE);
    // For AMD chipset workaround !!
    // AMD SPI can't read SPI ROM as buffer.
    if(!EFI_ERROR(Status))
    {
        Flash->Read(
              (UINT8*)(NVRAM_ADDRESS), NVRAM_SIZE, Buffer);

        Flash->Write(
              (UINT8*)(NVRAM_BACKUP_ADDRESS), NVRAM_SIZE, Buffer);

        pSmst->SmmFreePages(Phy_Address, NumberOfPages);
    }else
    {
        // If no memory , just do it 4 bytes one time.
        for(; i < NVRAM_SIZE ; i += sizeof(UINT32))
        {
            Flash->Read(
                  (UINT8*)(NVRAM_ADDRESS + i), sizeof(UINT32), &Buffer32);
            if(Buffer32 == 0xFFFFFFFF)
                continue;
            Flash->Write(
                  (UINT8*)(NVRAM_BACKUP_ADDRESS + i), sizeof(UINT32), &Buffer32);
        }
    }
    // Restore NVRAM Main FFS state!!
    Flash->Write((UINT8*)NvramFFSStatePtr,
                 sizeof(EFI_FFS_FILE_STATE),&NvramFFSState);
}
#endif
#endif


/**
 * Procedure to Disable Power Button while flashing.
 *
 */
VOID DisablePowerButton(VOID)
{
    // Disable PWR Button SMI
    IoWrite16(PM_BASE_ADDRESS + 0x02, IoRead16(PM_BASE_ADDRESS + 0x02) & 0xFEDF);
}


/**
 * Procedure to Enable Power Button after flashing.
 *
 */
VOID EnablePowerButton (VOID)
{
    //Clear All PM  Statuses
    IoWrite16(PM_BASE_ADDRESS,IoRead16(PM_BASE_ADDRESS));
    // Re-ensable PWR Button SMI
    IoWrite16(PM_BASE_ADDRESS + 0x02, BIT05 + BIT08);
}


/**
 * Procedure to Wait till KBC I/P buffer is free..
 *
 * @retval EFI_SUCCESS KBC I/P buffer is free.
 * @retval EFI_TIMEOUT Wait for KBC I/P buffer free is Timeout.
 * @retval EFI_NO_RESPONSE KBC is not supproted.
 *
 */
static EFI_STATUS KbcIbFree(VOID)
{
    UINT32      i;
#if defined(KBC_SUPPORT)
#if (KBC_SUPPORT == 0)
    return EFI_NO_RESPONSE;
#endif
#else
    if( (IoRead8( 0x64 ) == 0xFF) )
        return EFI_NO_RESPONSE;
#endif
    for (i = 0; i < 0x100000; i++) {
        if (!(IoRead8(0x64) & BIT01)) return EFI_SUCCESS;
    }
    return EFI_TIMEOUT;
}
 
/**
 * Procedure to Disable USB Keyboard while flashing.
 *
 */
VOID DisableUSBKBD(VOID)
{
    EFI_STATUS Status;
#if AMIUSB_SUPPORT == 1
#if USB_DRIVER_MAJOR_VER >= 10
    AMI_USB_SMM_PROTOCOL	*UsbSmmProtocol = NULL;
    URP_STRUC               UrpStruc;
    Status = pSmst->SmmLocateProtocol(&gAmiUsbSmmProtocolGuid, NULL, &UsbSmmProtocol);
    if (EFI_ERROR(Status)) return;
    // USBAPI_KbcAccessControl is the 17th API_FUNC in UsbApiTable.    
    UrpStruc.ApiData.KbcControlCode = 1;    
    gUsbRtKbcAccessControl = (API_FUNC)UsbSmmProtocol->UsbApiTable[17];
    if (gUsbRtKbcAccessControl != NULL) gUsbRtKbcAccessControl(&UrpStruc);
#else
    UINTN    VariableSize = sizeof(UINTN);
    EFI_GUID AmiGlobalVariableGuid = AMI_GLOBAL_VARIABLE_GUID;
    Status = pRS->GetVariable (L"USB_POINT", \
                &AmiGlobalVariableGuid, NULL, &VariableSize, &gUsbRtKbcAccessControl);
    if (EFI_ERROR(Status)) return;
    // Directly invoke th UsbRtKbcAccessContol avoiding to call outside SMM.
    if (gUsbRtKbcAccessControl != NULL) gUsbRtKbcAccessControl(1);
#endif
#endif

    // Disable KBC.
#if defined EMUL6064_SUPPORT && (EMUL6064_SUPPORT == 1)
    Status = pSmst->SmmLocateProtocol(&gEmul6064TrapProtocolGuid, NULL, &gEmulationTrap);
    if (!EFI_ERROR(Status)) {
#if defined KBCEMULATION_DRIVER_VERSION && (KBCEMULATION_DRIVER_VERSION >= 4)
        if (!gEmulationTrap->GetTrapStatus(gEmulationTrap)) gEmulationTrap = NULL;
#endif
        if (gEmulationTrap) gEmulationTrap->TrapDisable(gEmulationTrap);
    } else gEmulationTrap = NULL;    
#endif
    Status = KbcIbFree();
    if (!EFI_ERROR(Status)) IoWrite8(0x64, 0xad);
    if (!((gKbcPortData = IoRead8(0x21)) & BIT01)) IoWrite8(0x21, gKbcPortData | BIT01);
}


/**
 * Procedure to Re-enable USB Keyboard after flashing.
 *
 */
VOID EnableUSBKBD (VOID)
{
    // Enable KBC.
    if (!EFI_ERROR(KbcIbFree())) IoWrite8(0x64, 0xae);
    if (!(gKbcPortData & BIT01)) IoWrite8(0x21, gKbcPortData & ~BIT01);
#if defined EMUL6064_SUPPORT && (EMUL6064_SUPPORT == 1)
    if (gEmulationTrap) gEmulationTrap->TrapEnable(gEmulationTrap);
#endif	
#if AMIUSB_SUPPORT == 1
{
#if USB_DRIVER_MAJOR_VER >= 10
    URP_STRUC               UrpStruc;
    UrpStruc.ApiData.KbcControlCode = 0;    
    if (gUsbRtKbcAccessControl != NULL) gUsbRtKbcAccessControl(&UrpStruc);
#else
    if (gUsbRtKbcAccessControl != NULL) gUsbRtKbcAccessControl(0);
#endif
}    
#endif
}

#if AFU_BUFFER_IN_SHADOW
#include <AmiCspLib.h>
/**
 * Procedure to transfer data via Shadow RAM between BIOS and Flash Tool.
 *
 * @param Data Software SMI function of SmiFlash.
 * @param pCommBuff Buffer provided from Flash Tool.
 *
 */
VOID UpdateShadowBuffer(
  IN     UINT8  Data, 
  IN OUT UINT64 pCommBuff
)
{
    UINT8           FixLen;
    static UINT8*   Buffer = 0;
    UINT32          LowBufferAddress, HighBufferAddress;

    if(Data == SMIFLASH_ENABLE_FLASH) {
        OemRuntimeShadowRamWrite(TRUE);
        return ;
    }    
    if((UINTN)Buffer == 0)
    {
        Buffer = (UINT8*)0xF0000;
        for(;*(UINT32*)Buffer != 0x46534124 && 
                    ((UINTN)Buffer < 0x100000);(UINTN)Buffer++);
    }
    
    if((UINTN)Buffer == 0xFFFFF)
        return;
    // Skip Sig
    FixLen = *(Buffer + 4);
    
    if(*(UINT32*)(Buffer + FixLen) == 0 || 
       *(UINT32*)(Buffer + FixLen) == 0xFFFFFFFF)
        return;
    
    LowBufferAddress = *(UINT32*)(Buffer + FixLen);
    HighBufferAddress = 0;
    pCommBuff           = HighBufferAddress;
    pCommBuff           = Shl64(pCommBuff, 32);
    pCommBuff           += LowBufferAddress;
    
}


/**
 * Procedure to clear the used Shadow RAM.
 *
 * @param Data Software SMI function of SmiFlash.
 * @param pCommBuff Buffer provided from Flash Tool.
 *
 */
VOID ClearShadowBuffer (
  IN     UINT8  Data, 
  IN OUT UINT64 pCommBuff
)
{
    UINT8           FixLen;
    static UINT8*   Buffer = 0;

    if(Data == SMIFLASH_DISABLE_FLASH) {
        OemRuntimeShadowRamWrite(FALSE);
        return ;
    }
    if((UINTN)Buffer == 0)
    {
        Buffer = (UINT8*)0xF0000;
        for(;*(UINT32*)Buffer != 0x46534124 && 
                    ((UINTN)Buffer < 0x100000);(UINTN)Buffer++);
        
    }
    if((UINTN)Buffer == 0xFFFFF)
        return;

    // Skip Sig
    FixLen = *(Buffer + 4);
    // Clear Buffer address
    *(UINT32*)(Buffer + FixLen) = 0;
    
}
#endif
#else // #if defined _OUTSIDE_SMM_
//*********** OUTSIDE SMM ***********************************************
#if defined RECOVERY_PRESERVE_VARS_IN_SMM && RECOVERY_PRESERVE_VARS_IN_SMM == 1
/**
 * Procedure to generate the SW SMI Call.
 *
 */
static FUNC_BLOCK FuncBlock = {0,0,0,0};
static UINT8 mSMIFlashSmi[] = {
    0x50,                   // push eax
    0x53,                   // push ebx
    0x51,                   // push ecx
    0x52,                   // push edx
    0xBA, 0, 0, 0, 0,       // mov edx, SW_SMI_IO_ADDRESS
    0xB8, 0x00, 0x00, 0x00, 0x00,       // mov eax, 0   //SW SMI value
    0xBB, 0x00, 0x00, 0x00, 0x00,       // mov ebx, 0   //LowBufferAddress
    0xB9, 0x00, 0x00, 0x00, 0x00,       // mov ecx, 0   //HighBufferAddress
    0xEE,                   // out dx, al
    0x5A,                   // pop edx
    0x59,                   // pop ecx
    0x5B,                   // pop ebx
    0x58,                   // pop eax
    0xC3,                   // ret
};//mSMIFlashSmi[]


/**
 * Procedure to preserve variable in SMM before BIOS recovery.
 *
 */
VOID ReflashBeforeUpdateHook (VOID)
{
    void    (*IssueSWSMI)(void);
    *(UINT8*)&mSMIFlashSmi[10] = SMIFLASH_ENABLE_FLASH; //EAX
    *(UINT32*)&mSMIFlashSmi[5] = SW_SMI_IO_ADDRESS;     //EDX
    *(UINT32*)&mSMIFlashSmi[15] = (UINT32)&FuncBlock;   //EBX
    IssueSWSMI = (void*)mSMIFlashSmi;
    IssueSWSMI();
#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT
    // For following DeviceWriteEnable call of reflash.c.
    Flash->DeviceWriteDisable(); 
#endif
}

/**
 * Procedure to restores variables from SMM after BIOS recovery.
 *
 */
VOID ReflashAfterUpdateHook (VOID)
{
    void    (*IssueSWSMI)(void);

#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT
    // For following DeviceWriteDisable call of smiflash func#24.
    Flash->DeviceWriteEnable();
#endif
    *(UINT8*)&mSMIFlashSmi[10] = SMIFLASH_DISABLE_FLASH;//EAX
    *(UINT32*)&mSMIFlashSmi[5] = SW_SMI_IO_ADDRESS;     //EDX
    *(UINT32*)&mSMIFlashSmi[15] = (UINT32)&FuncBlock;   //EBX
    IssueSWSMI = (void*)mSMIFlashSmi;
    IssueSWSMI();
}
#endif // if defined RECOVERY_PRESERVE_IN_SMM && RECOVERY_PRESERVE_IN_SMM == 1
#endif // if defined _OUTSIDE_SMM_                                            
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
