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
  This file contains Reflash driver flash update code

**/
#include <Token.h>
#include <Ffs.h>
#include <AmiCspLib.h>
#include <AmiRomLayout.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/PcdLib.h>
#include <Library/AmiRomLayoutLib.h>
#include <Library/BaseMemoryLib.h>
#include "ReFlash.h"

#define AMI_ROM_LAYOUT_NVRAM_GUID \
        { 0xfa4974fc, 0xaf1d, 0x4e5d, { 0xbd, 0xc5, 0xda, 0xcd, 0x6d, 0x27, 0xba, 0xec } }

#define AMI_ROM_LAYOUT_FV_BB_GUID \
        { 0x61C0F511, 0xA691, 0x4F54, { 0x97, 0x4F, 0xB9, 0xA4, 0x21, 0x72, 0xCE, 0x53 } }

#ifndef FtRecovery_SUPPORT
#define FtRecovery_SUPPORT 0
BOOLEAN IsTopSwapOn (VOID) { return FALSE; }
VOID SetTopSwap (BOOLEAN On) {};
#endif

#ifndef BUILD_TIME_BACKUP
#define BUILD_TIME_BACKUP 0
#endif

extern EFI_STATUS RecoveryStatus;

FLASH_AREA_EX *BlocksToUpdate = NULL;

static EFI_GUID AmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
static EFI_GUID AmiRomLayoutNvramGuid = AMI_ROM_LAYOUT_NVRAM_GUID;
static EFI_GUID AmiRomLayoutBootBlockGuid = AMI_ROM_LAYOUT_FV_BB_GUID;
static AMI_POST_MANAGER_PROTOCOL *AmiPostMgr = NULL;
static EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl = NULL;
static BOOLEAN UserConfirmation = TRUE;


/**
  This function draws message box on screen and waits till user presses a key
  
  @param Caption    Pointer to a Message Box caption string 
  @param Message    Pointer to a Message Box message string 

**/
VOID DrawMessageBox(
    IN CHAR16 *Caption,
    IN CHAR16 *Message
)
{
    EFI_EVENT ev;
#if TSE_BUILD <= 0x1249
    UINTN i;
#endif
    
    if(!UserConfirmation)
        return;

    if(AmiPostMgr != NULL) {
        if(ConsoleControl != NULL)
            ConsoleControl->LockStdIn(ConsoleControl, L"");

        pST->ConIn->Reset(pST->ConIn, FALSE);    //discard all keystrokes happend during flash update
        
#if TSE_BUILD <= 0x1249
        AmiPostMgr->DisplayInfoBox(Caption, Message, 5, &ev);
        pBS->SetTimer(ev, TimerCancel, 0);
        pBS->WaitForEvent(1, &pST->ConIn->WaitForKey, &i);
        pBS->SignalEvent(ev);
#else
        AmiPostMgr->DisplayInfoBox(Caption, Message, 0, &ev);
#endif
    }
}

/**
  This function establishes pause in non-interactive mode
  to make error messages readable for humans
  
**/
VOID CountDown(
    VOID
)
{
    pBS->Stall(5000000);
}

/**
  This function sets flash update environment
  
  @param Interactive       TRUE if flash update is interactive, FALSE otherwise
  @param Win8StyleUpdate   TRUE if flash update was initiated by Windows 8

  @retval EFI_SUCCESS Environment set successfully
  @retval other       error occured during execution

**/
EFI_STATUS Prologue(
    IN BOOLEAN Interactive,
    IN BOOLEAN Win8StyleUpdate
)
{
    static BOOLEAN Executed = FALSE;
    static EFI_STATUS Status = EFI_SUCCESS;

    if(Executed)
        return Status;

    Executed = TRUE;
    
    if (RecoveryBuffer == NULL)     //rare case, where Autoflash variable is ignored
        return EFI_ABORTED;         //(when NVRAM is in StdDefaults mode)
    
    if(!Win8StyleUpdate)    //we do not report progress during Win8-style flash update
        Status = pBS->LocateProtocol(&AmiPostManagerProtocolGuid, NULL, &AmiPostMgr);

    Status = CreateFlashUpdateArea();
    if(EFI_ERROR(Status))
    	return Status;
    
    ApplyUserSelection(Interactive);

    if(Interactive) {
        Status = pBS->LocateProtocol(&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
        if(!EFI_ERROR(Status))
            ConsoleControl->LockStdIn(ConsoleControl, L"");
    } else {
        if(EFI_ERROR(RecoveryStatus)) { //we can't perform flash update, inform user and return error for resetting system
            CountDown();
            return EFI_ABORTED;
        }
        UserConfirmation = FALSE;
    }

    OemBeforeFlashCallback();
    Flash->DeviceWriteEnable();
    return Status;
}

/**
  This function finishes flash update process
  It's not supposed to return control to the caller
  
  @retval EFI_DEVICE_ERROR  Something went wrong

**/
EFI_STATUS Epilogue(VOID)
{
    Flash->DeviceWriteDisable();
    OemAfterFlashCallback();

    DrawMessageBox(L"Flash update", L"Flash update completed. Press any key to reset the system");
    pRS->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    return EFI_DEVICE_ERROR;
}

/**
  This function creates backups or programs flash regions
  
  @param BackUp       TRUE if respective flash regions should be backed up

  @retval EFI_SUCCESS Execution went successfully
  @retval other       error occured during execution

**/
EFI_STATUS FlashWalker(
    IN BOOLEAN BackUp
)
{
    EFI_STATUS Status;
    UINTN i;
    UINTN k;
    EFI_STRING Title;
    EFI_STRING Message;
    UINTN Size;
    UINTN Step;
    UINTN Threshold;
    UINTN Blocks;

    VOID *DisplayHandle;
    AMI_POST_MGR_KEY OutKey;

    if(AmiPostMgr != NULL) {
        Title = NULL;
        if(BackUp)
            GetHiiString(ReflashHiiHandle, STRING_TOKEN(STR_FLASH_PROGRESS_TITLE_BACKUP), &Size, &Title);
        else
            GetHiiString(ReflashHiiHandle, STRING_TOKEN(STR_FLASH_PROGRESS_TITLE_FLASH), &Size, &Title);

        AmiPostMgr->DisplayProgress(AMI_PROGRESS_BOX_INIT,
                 (Title == NULL) ? L"Flash Update Progress" : Title,
                 NULL,
                 NULL,
                 0,
                 &DisplayHandle,
                 &OutKey);
    }

    for(i = 0; BlocksToUpdate[i].Type != FvTypeMax; i++) {
        if(!BlocksToUpdate[i].Update)
            continue;   //block is not updated

        if(BackUp && BlocksToUpdate[i].BackUp == NULL)
            continue; //no backup function

        if(AmiPostMgr != NULL) {
        /* prepare display progress window */
            Message = NULL;
            if(BackUp)
                GetHiiString(ReflashHiiHandle, BlocksToUpdate[i].BackUpStringId, &Size, &Message);
            else
                GetHiiString(ReflashHiiHandle, BlocksToUpdate[i].ProgramStringId, &Size, &Message);
        }

    /* calculate number of blocks to flash */
        Blocks = BlocksToUpdate[i].Size / BlocksToUpdate[i].BlockSize;

    /* calculate progress steps */
        Step = 0;
        if(Blocks > 100) {
            Threshold = (Blocks / 100) + 1;
        } else {
            Threshold = 1;
        }

    /* now we're ready to do actual job */
        for(k = 0; k < Blocks; k++) {
            if(BackUp)
                Status = BlocksToUpdate[i].BackUp(&BlocksToUpdate[i], k);
            else
                Status = BlocksToUpdate[i].Program(&BlocksToUpdate[i], k);

            if(EFI_ERROR(Status))
                return Status;

            Step++;
            if(Step == Threshold && AmiPostMgr != NULL) {
            /* report progress to user */
                AmiPostMgr->DisplayProgress(AMI_PROGRESS_BOX_UPDATE,
                                 (Title == NULL) ? L"Flash Update Progress" : Title,
                                 (Message == NULL) ? L"Flash new data" : Message,
                                 NULL,
                                 (k * 100) / Blocks,
                                 &DisplayHandle,
                                 &OutKey);
                Step = 0;
            }
        }
        
    /* check if TopSwap should be triggered */
        if(FtRecovery_SUPPORT) {
            if(BlocksToUpdate[i].TopSwapTrigger) {
                if(BackUp)
                    SetTopSwap(TRUE);
                else
                    SetTopSwap(FALSE);
            }
        }
    }

    if(AmiPostMgr != NULL) {
    /* close progress window */
        AmiPostMgr->DisplayProgress(AMI_PROGRESS_BOX_CLOSE,
                 (Title == NULL) ? L"Flash Update Progress" : Title,
                 NULL,
                 NULL,
                 0,
                 &DisplayHandle,
                 &OutKey);
    }

    return EFI_SUCCESS;
}

/**
  This function modifies setup control values to enable flash process
  that was disabled previously
  
**/
VOID EnableFlash(VOID)
{
    AUTOFLASH FlashUpdateControl;
    UINTN Size = sizeof(FlashUpdateControl);
    static EFI_GUID guidRecovery = RECOVERY_FORM_SET_GUID;

    HiiLibGetBrowserData(&Size, &FlashUpdateControl, &guidRecovery, L"Setup");
    FlashUpdateControl.UserOverride = 0;
    HiiLibSetBrowserData(Size, &FlashUpdateControl, &guidRecovery, L"Setup");
}

/**
  This function is the setup browser callback to perform flash update
  
  @param This           Pointer to EFI_HII_CONFIG_ACCESS_PROTOCOL instance
  @param Action         Action that setup browser is prepared to take
  @param KeyValue       ID of setup control, that browser is currently processing
  @param Type           Value type of setup control, that browser is currently processing
  @param Value          Value of setup control, that browser is currently processing
  @param ActionRequest  Action that callback wants browser to perform

  @retval EFI_SUCCESS     Execution went successfully
  @retval EFI_UNSUPPORTED Browser action is not supported
  @retval other           error occured during execution

**/
EFI_STATUS EFIAPI FlashProgressEx(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
)
{
    EFI_STATUS Status;

    if (ActionRequest) 
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

    if(Action != EFI_BROWSER_ACTION_CHANGING)
        return EFI_UNSUPPORTED;


#if REFLASH_INTERACTIVE
    if(KeyValue == FLASH_START_KEY) {
        Status = Prologue(TRUE, FALSE);
        if(EFI_ERROR(RecoveryStatus) || EFI_ERROR(Status)) {//we check recovery status here just in case
                                                            //that Autoflash variable was ignored
        /* inform user, that flashing can't be performed */
            DrawMessageBox(L"ERROR!!!!", L"Flash update failed to initialize. Press any key to reset system");
            pRS->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
    /* we have to give control back to setup browser to refresh the screen */
        return EFI_SUCCESS;
    }

    if(KeyValue == FLASH_ENABLE_KEY) {
        EnableFlash();
        return EFI_SUCCESS;
    }
/* nothing to do here, wait for user response */
    if (KeyValue != FLASH_PROGRESS_KEY) {
        return EFI_SUCCESS;     
    }
#else
    Status = Prologue(FALSE, FALSE);
    if(EFI_ERROR(Status)) {
        pRS->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
#endif

    if(FtRecovery_SUPPORT) {
        Status = FtControlFlow();
    } else {
        Status = FlashWalker(FALSE);
    }
    
    if(EFI_ERROR(Status)) {
    /* something was wrong - inform user */
        DrawMessageBox(L"ERROR!!!!", L"Flash update failed. Press any key to reset system");
        pRS->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    }

    Status = Epilogue();
    return Status;
}

/**
  This function prepares FV_MAIN region to be backup region
  
**/
VOID SetBackUpArea(VOID)
{
    static BOOLEAN FirstRun = TRUE;
    static UINT32 FlashEraser = (FLASH_ERASE_POLARITY) ? 0 : 0xffffffff;
    EFI_FIRMWARE_VOLUME_HEADER *Fv;

    if(!FirstRun)
        return;

    FirstRun = FALSE;
    Fv = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)AMI_ROM_LAYOUT_FV_MAIN_ADDRESS;
    Flash->Write(&(Fv->Signature), sizeof(UINT32), &FlashEraser);
}

/**
  This function performs backup of certain flash area
  
  @param Block          Pointer to the flash area 
  @param BlockNumber    Block number within flash area

  @retval EFI_SUCCESS   Execution went successfully
  @retval other         error occured during execution

**/
EFI_STATUS AmiFlashBackUp(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
)
{
    UINT8 *Source;
    UINT8 *Destination;
    VOID *Tmp;
    EFI_STATUS Status;
    EFI_TPL OldTpl;

    Status = pBS->AllocatePool(EfiBootServicesData, Block->BlockSize, &Tmp);
    if(EFI_ERROR(Status))
        return Status;

    Source = Block->BlockAddress + BlockNumber * Block->BlockSize;
    Destination = Block->BackUpAddress + BlockNumber * Block->BlockSize;

    if(Block->Type == FvTypeBootBlock)
        SetBackUpArea();

    Status = Flash->Read(Source, Block->BlockSize, Tmp);
    if(EFI_ERROR(Status)) {
        pBS->FreePool(Tmp);
        return Status;
    }

//save to backup address
    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    Status = Flash->Update(Destination, Block->BlockSize, Tmp);
    pBS->RestoreTPL(OldTpl);
    
    pBS->FreePool(Tmp);
    return Status;
}

/**
  This function performs update of certain flash area
  
  @param Block          Pointer to the flash area 
  @param BlockNumber    Block number within flash area

  @retval EFI_SUCCESS   Execution went successfully
  @retval other         error occured during execution

**/
EFI_STATUS AmiFlashProgram(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
)
{
    UINT8 *ImageStart;
    UINT8 *Source;
    UINT8 *Destination;
    EFI_STATUS Status;
    EFI_TPL OldTpl;

    ImageStart = RecoveryBuffer + Block->RomFileOffset;
    Source = ImageStart + BlockNumber * Block->BlockSize;
    Destination = Block->BlockAddress + BlockNumber * Block->BlockSize;

    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    Status = Flash->Update(Destination, Block->BlockSize, Source);
    pBS->RestoreTPL(OldTpl);

    return Status;
}

/**
  This function performs update of TOP_SWAP flash area
  
  @param Block          Pointer to the flash area 
  @param BlockNumber    Block number within flash area

  @retval EFI_SUCCESS   Execution went successfully
  @retval other         error occured during execution

**/
EFI_STATUS AmiFlashProgramTs(
    IN FLASH_AREA_EX *Block,
    IN UINTN BlockNumber
)
{
    UINT8 *ImageStart;
    UINT8 *Source;
    UINT8 *Destination;
    EFI_STATUS Status;
    EFI_TPL OldTpl;
    
    ImageStart = RecoveryBuffer + Block->RomFileOffset;
    Source = ImageStart + BlockNumber * Block->BlockSize;
    Destination = Block->BlockAddress + BlockNumber * Block->BlockSize;

    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    Status = Flash->Erase(Destination, Block->BlockSize);
    if(!EFI_ERROR(Status)){
    	Status = Flash->Write(Destination, Block->BlockSize, Source);
    }
    pBS->RestoreTPL(OldTpl);

    return Status;
}

/**
  This function checks if given flash area is a TOP_SWAP flash area
  
  @param Area          Pointer to the flash area 

  @retval TRUE   Area is a TOP_SWAP flash area
  @retval FALSE  Area is not a TOP_SWAP flash area

**/
BOOLEAN IsTopSwapArea(
	IN AMI_ROM_AREA *Area
)
{
	return ((Area->Address - 1 + Area->Size) == 0xFFFFFFFF) ? TRUE : FALSE;
}

/**
  This function returns ID of the string that describes given flash region type
  
  @param Type    Type of the flash region

  @return   Corresponding String ID

**/
EFI_STRING_ID GetStringIdByType(FLASH_FV_TYPE Type)
{
	return (Type == FvTypeBootBlock) ? STRING_TOKEN(STR_FLASH_PROGRESS_MESSAGE_FLASH_BB) :
		   (Type == FvTypeMain) ? STRING_TOKEN(STR_FLASH_PROGRESS_MESSAGE_FLASH_MAIN) :
		   (Type == FvTypeNvRam) ? STRING_TOKEN(STR_FLASH_PROGRESS_MESSAGE_FLASH_NVRAM) : STRING_TOKEN(STR_FLASH_PROGRESS_CAPTION_FLASH);
}

/**
  This function returns address within flash to use as backup address
  
  @param Size    Size of the region, that needs to be backed up

  @return   Pointer to backup area

**/
UINT8 *GetRuntimeBackupAddress (
        UINT32 Size
        )
{
    /*
    AMI_ROM_AREA *Area;
    UINT8 *Result = NULL;
    
    for (Area = AmiGetFirstRomArea (); Area != NULL; Area = AmiGetNextRomArea (Area)) {
        if (Area->Type != 0 ||                                          //we need usable FV
            (Area->Attributes & AMI_ROM_AREA_VITAL) ||                   //it can't be boot block
            !(Area->Attributes & AMI_ROM_AREA_FV_ACCESS_ATTRIBUTES) || //it can't be backup or nvram
            Area->Size - Size < 0x10000)                                //it should be big enough
            continue;
            
            Result = (UINT8 *)(UINTN)Area->Address;
            Result += 0x10000;
    }
    return Result;
    */
    UINT8 *Result = (UINT8 *)(UINTN)AMI_ROM_LAYOUT_FV_MAIN_ADDRESS;
    Result += 0x10000;
    return Result;
}

/**
  This function adds flash area to the list of updatable areas
  
  @param Area    Pointer to the flash area
  @param Index   Index within flash update array
  @param Type    Flash area type

**/
VOID AddBlockToList(
	IN AMI_ROM_AREA *Area OPTIONAL,
	IN UINT32 Index,
	IN FLASH_FV_TYPE Type
)
{
	if(Type == FvTypeMax) {
		MemSet(&BlocksToUpdate[Index], sizeof(FLASH_AREA_EX), 0);
		BlocksToUpdate[Index].Type = FvTypeMax;
		return;
	}
	
	BlocksToUpdate[Index].BlockAddress = (UINT8 *)(UINTN)(Area->Address);
	BlocksToUpdate[Index].RomFileOffset = Area->Offset;
	BlocksToUpdate[Index].Size = Area->Size;
	BlocksToUpdate[Index].BlockSize = FLASH_BLOCK_SIZE;
	BlocksToUpdate[Index].Type = Type;
	BlocksToUpdate[Index].Update = TRUE;
	BlocksToUpdate[Index].TopSwapTrigger = FALSE;
	BlocksToUpdate[Index].Program = (IsTopSwapArea(Area) || Type == FvTypeBootBlockBackup) ? 
	                                 AmiFlashProgramTs : AmiFlashProgram;
	BlocksToUpdate[Index].ProgramStringId = GetStringIdByType(Type);
	if(Type != FvTypeBootBlock || BUILD_TIME_BACKUP) {		//no runtime backup or non-backup FV
		BlocksToUpdate[Index].BackUpAddress = NULL;
		BlocksToUpdate[Index].BackUpStringId = 0;
		BlocksToUpdate[Index].BackUp = NULL;
	} else {
		BlocksToUpdate[Index].BackUpAddress = (IsTopSwapArea(Area)) ? (UINT8 *)(UINTN)(Area->Address - Area->Size) : GetRuntimeBackupAddress (Area->Size);
		BlocksToUpdate[Index].BackUpStringId = STRING_TOKEN(STR_FLASH_PROGRESS_MESSAGE_BACKUP_BB);
		BlocksToUpdate[Index].BackUp = AmiFlashBackUp;
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CreateFlashUpdateArea
// 
// Description:  This function converts given ROM layout to FLASH_AREA_EX records array
//               
// Input:   
//  None
//
// Output:
//  EFI_SUCCESS - ROM layout converted successfully
//	EFI_ERROR - error occured during conversion
// 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS TempGetRomLayout (
    AMI_ROM_AREA **Layout,
    UINT32 *AreaSize,
    UINT32 *LayoutSize
    );

/**
  This function creates flash update array, based on current ROM layout
  
  @retval EFI_SUCCESS   Flash update array created successfully
  @retval other         error occured during execution

**/
EFI_STATUS CreateFlashUpdateArea(
    VOID
)
{
	EFI_STATUS Status;
	AMI_ROM_AREA *Area;
	AMI_ROM_AREA *Layout;
	UINT32 Count = 0;
	UINT32 Start = 0;
	UINT32 End;
	BOOLEAN FirstMain = TRUE;
	
//	UINT32 Version;
	UINT32 AreaSize;
	UINT32 LayoutSize;
	UINT32 AreaCount;
//	UINTN ImageSize;
	
	UINT32 i;
	
//	ImageSize = (UINTN) PcdGet32 (PcdRecoveryImageSize);
//	Status = AmiGetImageRomLayout (RecoveryBuffer, ImageSize, &Layout, &Version, &AreaSize, &LayoutSize);
	Status = TempGetRomLayout (&Layout, &AreaSize, &LayoutSize);
	if (EFI_ERROR (Status))
		return Status;
	
	//get number of firmware volumes in ROM layout
	//we count all of type AmiRomAreaTypeFv or signed RAW blocks
	AreaCount = LayoutSize / AreaSize;
	Area = Layout;
	for (i = 0; i < AreaCount; i++, Area++) {
	    if (Area->Type == 0 || Area->Attributes & AMI_ROM_AREA_SIGNED)
	        Count++;
	}
	
	Status = pBS->AllocatePool (EfiBootServicesData, (Count + 1) * sizeof(FLASH_AREA_EX), &BlocksToUpdate);	//add one more member as terminator
	if(EFI_ERROR(Status))
		return Status;
	
	//We need update areas to be in particular order:
	// 0: BootBlock0
	// 1: BootBlock1
	// ...
	// X: BootBlockX - last boot block area with TopSwap trigger set
	// X+1: AnyBlock0
	// ...
	// Count-2: AnyBlockY
	// Count-1: FirstMainBlock - first encountered MainBlock
	// Count: Terminator
	// Total size of array is Count+1, where Count is number of FVs in ROM layout
	// AnyBlock can be NvramBlock, BackupBlock or MainBlock
	// The reason that one of possible several main blocks should be flashed last
	// is to verify, that flash update was successful.
	// Otherwise, if any backup blocks were flashed last and failed, we may not know
	// about it right away, since they are not taking part in regular boot and not verified
	End = Count - 2;
    AddBlockToList(NULL, Count, FvTypeMax);     //add terminator member
	
    Area = Layout;
    for (i = 0; i < AreaCount; i++, Area++) {
		if (Area->Type != 0 && !(Area->Attributes & AMI_ROM_AREA_SIGNED))
			continue;
		
		if(Start > End)
			return EFI_DEVICE_ERROR;			//something wrong - indexes are overlapped
		
		if (Area->Attributes & AMI_ROM_AREA_FV_ACCESS_ATTRIBUTES) {
		    if (Area->Attributes & AMI_ROM_AREA_VITAL) {
		        AddBlockToList (Area, Start, FvTypeBootBlock);
		        Start++;
		    } else {    //FV has access attributes, but not VITAL - it is main block
		        if (FirstMain) {
		            AddBlockToList (Area, Count - 1, FvTypeMain);
		            FirstMain = FALSE;
		        } else {
		            AddBlockToList (Area, End, FvTypeMain);
		            End--;
		        }
		    }
		} else {    //no access attributes - either backup or custom or nvram
		    if (CompareGuid (&(Area->Guid), &AmiRomLayoutNvramGuid))
		        AddBlockToList(Area, End, FvTypeNvRam);
		    else if (CompareGuid (&(Area->Guid), &AmiRomLayoutBootBlockGuid))
		        AddBlockToList(Area, End, FvTypeBootBlockBackup);
		    else
		        AddBlockToList(Area, End, FvTypeCustom);
		    End--;
		}
	}
	
    i = 0;
	if (FtRecovery_SUPPORT == 1) {
		//set top swap trigger to last FV of type FvTypeBootBlock
		while(BlocksToUpdate[i].Type == FvTypeBootBlock)
			i++;
		BlocksToUpdate[i - 1].TopSwapTrigger = TRUE;
	}
	
	return EFI_SUCCESS;
}

/**
  This function establishes correct backup/update control flow for different scenarios
  
  @retval EFI_SUCCESS   Flash update executed successfully
  @retval other         error occured during execution

**/
EFI_STATUS FtControlFlow(
    VOID
)
{
    EFI_STATUS Status;
    
    if(IsTopSwapOn()) {                 //top swap is  ON already - reflash new image
        Status = FlashWalker(FALSE);
        return Status;
    }
    
    if(BUILD_TIME_BACKUP) {         //we have build-time backup image - no runtime backup needed
        SetTopSwap(TRUE);
        Status = FlashWalker(FALSE);
    } else {                        //create runtime backup image and flash new image
        Status = FlashWalker(TRUE);
        if(!EFI_ERROR(Status)) {
            Status = FlashWalker(FALSE);
        }
    }
    return Status;
}

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
