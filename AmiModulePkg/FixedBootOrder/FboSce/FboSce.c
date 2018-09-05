//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/**  @file FboSce.c

    Support Sce Tool With FixedBootOrder module.
**/

#include <AmiDxeLib.h>
#include <BootOptions.h>
#include <../FixedBootOrder.h>
#include <FboSce.h>

//
//  EXTERN GUID
//
#if !defined(ALWAYS_PUBLISH_HII_RESOURCES)||(0==ALWAYS_PUBLISH_HII_RESOURCES)
extern GUID gAmiTseSetupEnterGuid;
#endif

//
//  EXTERN VARIABLE
//
extern FBODevMap* pFBOLegacyDevMap;
extern FBODevMap* pFBOUefiDevMap;
extern EFI_GUID   gFixedBootOrderGuid;
extern DLIST*     BootOptionList;
//
//  EXTERN FUNCTION
//
extern UINT16 SearchDevMapByType(FBODevMap* pFBODevMap, UINT16 DevType);
extern UINT32 GetUefiBootOptionTag(BOOT_OPTION *Option);

/**
    Build default value of Legacy/Uefi SceOrder variable.
**/
VOID FboBuildDefaultSceOrder()
{
    EFI_STATUS  Status;
    UINTN       Size;
    UEFI_DEVICE_ORDER* UefiOrder = NULL;

#if CSM_SUPPORT
    LEGACY_DEVICE_ORDER* LegacyOrder = NULL;

    Status = GetEfiVariable(L"OriFboLegacyDevOrder", &gFixedBootOrderGuid, NULL, &Size, (VOID*)&LegacyOrder);
    if(!EFI_ERROR(Status))
    {
        UINT16 i;
        FBO_SCE_ORDER* SceOrder = MallocZ(Size);
        if (SceOrder) 
        {
            LEGACY_DEVICE_ORDER* OrderWalker;
            FBO_SCE_ORDER*       SceWalker = SceOrder;

            for (OrderWalker = LegacyOrder 
                    ; (UINT8*)OrderWalker < (UINT8*)LegacyOrder + Size
                    ; OrderWalker = (LEGACY_DEVICE_ORDER*)((UINT8*)OrderWalker + OrderWalker->Length + sizeof(OrderWalker->Type)))
            {
                for(i = 0; i < DEVORDER_COUNT(OrderWalker); i++)
                {
                    if(OrderWalker->Device[i] & FBO_LEGACY_DISABLED_MASK)
                        SceWalker->Index[i] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                    else
                        SceWalker->Index[i] = i;

                }
                SceWalker->Header.Type = OrderWalker->Type;
                SceWalker->Header.Length = sizeof(FBO_SCE_HEADER) + i * sizeof(SceWalker->Index);
                SceWalker = (FBO_SCE_ORDER*)&SceWalker->Index[i];

            }
            pRS->SetVariable(L"FboLegacySceOrder", &gFixedBootOrderGuid, BOOT_NV_ATTRIBUTES, (UINTN)SceWalker - (UINTN)SceOrder, SceOrder);
            pBS->FreePool(SceOrder);
        }
        pBS->FreePool((VOID*)LegacyOrder);
    }
#endif

    Status = GetEfiVariable(L"OriUefiDevOrder", &gFixedBootOrderGuid, NULL, &Size, (VOID*)&UefiOrder);
    if(!EFI_ERROR(Status))
    {
        UINT16 i;
        FBO_SCE_ORDER* SceOrder = MallocZ(Size);
        if (SceOrder) 
        {
            UEFI_DEVICE_ORDER* OrderWalker;
            FBO_SCE_ORDER*     SceWalker = SceOrder;
            for (OrderWalker = UefiOrder 
                    ; (UINT8*)OrderWalker < (UINT8*)UefiOrder + Size
                    ; OrderWalker = (UEFI_DEVICE_ORDER*)((UINT8*)OrderWalker + OrderWalker->Length + sizeof(OrderWalker->Type)))
            {
                for(i = 0; i < DEVORDER_COUNT(OrderWalker); i++)
                {
                    if(OrderWalker->Device[i] & FBO_UEFI_DISABLED_MASK)
                        SceWalker->Index[i] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                    else
                        SceWalker->Index[i] = i;

                }
                SceWalker->Header.Type = OrderWalker->Type;
                SceWalker->Header.Length = sizeof(FBO_SCE_HEADER) + i * sizeof(SceWalker->Index);
                SceWalker = (FBO_SCE_ORDER*)&SceWalker->Index[i];

            }
            pRS->SetVariable(L"FboUefiSceOrder", &gFixedBootOrderGuid, BOOT_NV_ATTRIBUTES, (UINTN)SceWalker - (UINTN)SceOrder, (VOID*)SceOrder);
            pBS->FreePool(SceOrder);
        }
        pBS->FreePool((VOID*)UefiOrder);
    }
}


/**
    Notification function to build the FboSce default. 

    @param  Event    Event that triggered this Event
    @param  Context  Context for this callback
**/
VOID FboSceNotificationFunction(EFI_EVENT Event, VOID* Context)
{
    if(Event)
    {
        pBS->CloseEvent(Event);
    }
    FboBuildDefaultSceOrder();
}

/**
    BDS elink to Build FboSce default.
**/
VOID FboSceBuildDefault()
{
#if !defined(ALWAYS_PUBLISH_HII_RESOURCES)||(0==ALWAYS_PUBLISH_HII_RESOURCES)
    VOID *Registration;
    EFI_EVENT FboSceEvent;
    RegisterProtocolCallback(&gAmiTseSetupEnterGuid, FboSceNotificationFunction, NULL, &FboSceEvent, &Registration);
#else
    FboSceNotificationFunction(NULL, NULL);
#endif
}

/**
    Examine whether FixedBoot indexes are valid. 

    @param  Index   Index to check from
    @param  Count   number of indexes to check

    @retval  EFI_INVALID_PARAMETER   one of parameters is invalid
    @retval  EFI_UNSUPPORT           invalid index
    @retval  EFI_SUCCESS             valid indexes.
**/
EFI_STATUS ExamineFixedBootIndex(UINT8* Index, UINTN Count)
{
    UINTN  i;
    UINT8  IndexBucket[FIXED_BOOT_ORDER_SUBMENU_MAX_NUM] = {0};
    UINT8  Idx;

    if(!Index || !Count)
        return EFI_INVALID_PARAMETER;

    for(i = 0; i < Count; i++)
    {
        Idx = Index[i];

        if(Idx == FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
            continue;

        //
        //  INVALID Value
        //
        if(Idx >= Count)
            return EFI_UNSUPPORTED;


        //
        // Duplicate value
        //
        if(IndexBucket[Idx]++)
            return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}


/**
    Sync Fbo variables from Sce.

    @param  SceVar   contains info to sync with Fbo.
**/
VOID FboSyncVarFromSce(FBO_SCE_VAR *SceVar)
{
    EFI_STATUS  Status;
    UINTN       Size;
    FBODevMap*  DevMap = SceVar->IsUefi ? pFBOUefiDevMap : pFBOLegacyDevMap;

    if(SceVar->IsUefi)
    {
        UEFI_DEVICE_ORDER* UefiOrder = NULL;
        UEFI_DEVICE_ORDER* NewUefiOrder;
        Status = GetEfiVariable(L"OriUefiDevOrder", &gFixedBootOrderGuid, NULL, &Size, (VOID**)&UefiOrder);
        if(!EFI_ERROR(Status))
        {
            UINT16  i, j;
            UINT16  Count;
            FIXED_BOOT_SETUP* FixedBoot = SceVar->FixedBoot;

            NewUefiOrder = MallocZ(Size);
            if(NewUefiOrder)
            {
                UINTN  Offset;
                UINT8  Index;
                UINT8* ItemIndex;
                DLINK* Link;
                BOOT_OPTION* Option;
                BOOLEAN DisableFlag[FIXED_BOOT_ORDER_SUBMENU_MAX_NUM];
                UEFI_DEVICE_ORDER* OrderWalker, *NewOrderWalker;

                NewOrderWalker = NewUefiOrder;

                for(OrderWalker = UefiOrder
                        ; (UINT8*)OrderWalker < (UINT8*)UefiOrder + Size
                        ; OrderWalker = (UEFI_DEVICE_ORDER*)((UINT8*)OrderWalker + sizeof(OrderWalker->Type) + OrderWalker->Length)
                            , NewOrderWalker = (UEFI_DEVICE_ORDER*)((UINT8*)NewOrderWalker + sizeof(NewOrderWalker->Type) + NewOrderWalker->Length))
                {

                    Offset = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM * SearchDevMapByType(DevMap, OrderWalker->Type);
                    ItemIndex = (UINT8*)&FixedBoot->UefiDevice[Offset];
                    Count =  DEVORDER_COUNT(OrderWalker);

                    if(EFI_ERROR( ExamineFixedBootIndex(ItemIndex, Count)))
                    {
                        MemCpy((VOID*)NewOrderWalker, (VOID*)OrderWalker, sizeof(OrderWalker->Type) + OrderWalker->Length);
                        TRACE((FBO_TRACE_LEVEL, "ExamineFixedBootIndex failed\n"));
                        continue;
                    }

                    NewOrderWalker->Type = OrderWalker->Type;
                    NewOrderWalker->Length = OrderWalker->Length;

                    MemSet((VOID*)DisableFlag, Count, TRUE);
                    //
                    //  Enabled First
                    //
                    for(i = 0, j = 0; i < Count; i++)
                    {
                        Index = ItemIndex[i];
                        if(Index < FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
                        {
                            if(Index < Count)
                            {
                                NewOrderWalker->Device[j++] = OrderWalker->Device[Index] & FBO_UEFI_ORDER_MASK;
                                DisableFlag[Index] = FALSE;
                            }
                        }
                    }

                    //
                    //  Then Disabled
                    //
                    for(i = 0; i < Count; i++)
                    {
                        if(DisableFlag[i])
                            NewOrderWalker->Device[j++] = OrderWalker->Device[i] | FBO_UEFI_DISABLED_MASK;
                    }
                }
                pRS->SetVariable(L"UefiDevOrder", &gFixedBootOrderGuid, BOOT_NV_ATTRIBUTES, Size, NewUefiOrder);

                //
                //  Set attributes
                //
                for(OrderWalker = NewUefiOrder 
                        ; (UINT8*)OrderWalker < (UINT8*)NewUefiOrder + Size
                        ; OrderWalker = (UEFI_DEVICE_ORDER*)((UINT8*)OrderWalker + sizeof(OrderWalker->Type) + OrderWalker->Length))
                {
                    for(i = 0; i < DEVORDER_COUNT(OrderWalker) ; i++)
                    {
                        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
                        {
                            if(IsLegacyBootOption(Option))
                                continue;

                            if(Option->BootOptionNumber == (OrderWalker->Device[i] & FBO_UEFI_ORDER_MASK))
                            {
                                if(GetUefiBootOptionTag(Option) == OrderWalker->Type)
                                {
                                    if(OrderWalker->Device[i] & FBO_UEFI_DISABLED_MASK)
                                        Option->Attributes &= ~LOAD_OPTION_ACTIVE;
                                    else
                                        Option->Attributes |= LOAD_OPTION_ACTIVE;
                                }
                            }


                        }
                    }
                }

#ifdef EFI_DEBUG
                {
                    UINT8 *data = (UINT8*)NewUefiOrder;
                    TRACE((FBO_TRACE_LEVEL, "New UefiDevOrder order : "));
                    for(i = 0; i < Size; i++)
                        TRACE((FBO_TRACE_LEVEL, "%02X ", data[i]));
                    TRACE((FBO_TRACE_LEVEL, "\n"));
                }
#endif
                pBS->FreePool((VOID*)NewUefiOrder);
            }
            pBS->FreePool((VOID*)UefiOrder);
        }
    }
#if CSM_SUPPORT
    else //  IsLegacy
    {
        LEGACY_DEVICE_ORDER*  LegacyOrder = NULL;

        Status = GetEfiVariable(L"FboLegacyDevOrder", &gFixedBootOrderGuid, NULL, &Size, (VOID**)&LegacyOrder);
        if(!EFI_ERROR(Status))
        {
            BOOT_OPTION** Options = MallocZ(sizeof(BOOT_OPTION*) * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM);

            if(Options)
            {
                UINT16       i;
                DLINK*       Link;
                BOOT_OPTION* Option;
                UINT16       Count;
                UINTN        Offset;

                UINT32       Priority;
                UINTN        OptionCount;
                UINT8        Index;

                UINT8*               ItemIndex;
                BBS_BBS_DEVICE_PATH* BbsDp;
                LEGACY_DEVICE_ORDER* OrderWalker;
            
                for(OrderWalker = LegacyOrder
                        ; (UINT8*)OrderWalker < (UINT8*)LegacyOrder + Size
                        ; OrderWalker = (LEGACY_DEVICE_ORDER*)((UINT8*)OrderWalker + sizeof(OrderWalker->Type) + OrderWalker->Length))
                {

                    Count =  DEVORDER_COUNT(OrderWalker);
                    Offset = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM * SearchDevMapByType(DevMap, OrderWalker->Type);
                    ItemIndex = (UINT8*)&(SceVar->FixedBoot->LegacyDevice[Offset]);

                    if(EFI_ERROR( ExamineFixedBootIndex(ItemIndex, Count)))
                    {
                        TRACE((FBO_TRACE_LEVEL, "ExamineFixedBootIndex failed\n"));
                        continue;
                    }

                    OptionCount = 0;
                    //
                    //  Collect BootOptions by Type
                    //
                    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
                    {
                        if(!IsLegacyBootOption(Option) || Option->GroupHeader)
                            continue;

                        if(Count >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
                            break;

                        BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
                        if (BbsDp->DeviceType != OrderWalker->Type)
                        {
                            //
                            //  accelerate the check by the same tag would be grouped
                            //
                            if(OptionCount)
                                break;

                            continue;
                        }

                        Options[OptionCount++] = Option;
                    }

                    //
                    //  avoiding special case
                    //
                    if(Count != OptionCount) 
                        break;

                    Priority = Options[0]->Priority;
                    //
                    //  Enabled  First
                    //
                    for(i = 0; i < Count; i++)
                    {
                        Index = ItemIndex[i];
                        //
                        //  avoiding invalid value
                        //
                        if(Index < FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
                        {
                            if(Index < Count)
                            {
                                Options[Index]->Attributes |= LOAD_OPTION_ACTIVE;
                                Options[Index]->Priority = Priority;
                                Options[Index] = NULL;
                                Priority = GetNextBootOptionPriority(Priority);
                            }
                        }
                    }

                    //
                    //  Then Disabled
                    //
                    for(i = 0; i < Count; i++)
                    {
                        if(Options[i])
                        {
                            Options[i]->Attributes &= ~LOAD_OPTION_ACTIVE;
                            Options[i]->Priority = Priority;
                            Priority = GetNextBootOptionPriority(Priority);
                        }

                    }
                }
                pBS->FreePool((VOID*)Options);
            }

            pBS->FreePool((VOID*)LegacyOrder);
        }
    }
#endif
    DUMP_BOOT_OPTION_LIST(BootOptionList, "SCE Processing");
}

/**
    Sync Sce changes with Fbo.

    @param  SceVar   contains info to sync with Fbo.
**/
VOID FboSyncWithSce(FBO_SCE_VAR *SceVar)
{
    if(!SceVar) return;

    if(SceVar->FixedBoot 
            && SceVar->FixedBootSize)
    {

        if(SceVar->SceOrder
                && SceVar->SceOrderSize >= sizeof(FBO_SCE_ORDER))
        {
            UINTN       i;
            UINT16      Len = 0;
            UINTN       Offset;
            UINT8*      Index = SceVar->IsUefi ? SceVar->FixedBoot->UefiDevice : SceVar->FixedBoot->LegacyDevice;
            FBODevMap*  DevMap = SceVar->IsUefi ? pFBOUefiDevMap : pFBOLegacyDevMap;
            FBO_SCE_ORDER* SceWalker = SceVar->SceOrder;

            for(; Len < SceVar->SceOrderSize; )
            {
                Offset = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM * SearchDevMapByType(DevMap, SceWalker->Header.Type);

                for(i = 0; i < SCEORDER_COUNT(SceWalker); i++)
                {
#ifdef EFI_DEBUG
                    TRACE((FBO_TRACE_LEVEL, "FboSync SceWalker->Index[%X](%X) : Index[%X](%X)\n", i, SceWalker->Index[i], Offset+i, Index[Offset+i]));
#endif
                    if(SceWalker->Index[i] != Index[Offset+i])
                    {
                        TRACE((FBO_TRACE_LEVEL, "Fbo SCE Running!!!\n"));
                        FboSyncVarFromSce(SceVar);
                        return;
                    }
                }
                Len += SceWalker->Header.Length;
                SceWalker = (FBO_SCE_ORDER*)&SceWalker->Index[i];
            }
        }
    }
}

/**
    Elink to check Sce for Fbo.
**/
VOID FboSceCheck(VOID)
{
    EFI_STATUS        Status;
    FBO_SCE_VAR       SceVar = {NULL, sizeof(FIXED_BOOT_SETUP), NULL, 0, 0};
    FIXED_BOOT_SETUP  FixedBoot;
    EFI_GUID FboGroupFormFormSetGuid = FBO_GROUP_FORM_FORM_SET_GUID ;
    
    Status = pRS->GetVariable( L"FixedBoot", &FboGroupFormFormSetGuid, NULL, &SceVar.FixedBootSize, (VOID*)&FixedBoot);
    if(!EFI_ERROR(Status))
    {
        SceVar.FixedBoot = &FixedBoot;

#if CSM_SUPPORT
        Status = GetEfiVariable( L"FboLegacySceOrder", &gFixedBootOrderGuid, NULL, &SceVar.SceOrderSize, (VOID**)&SceVar.SceOrder);
        if(!EFI_ERROR(Status))
        {
            FboSyncWithSce(&SceVar);
            pBS->FreePool((VOID*)SceVar.SceOrder);
            SceVar.SceOrder = NULL;
#if !defined(ALWAYS_PUBLISH_HII_RESOURCES)||(0==ALWAYS_PUBLISH_HII_RESOURCES)
            pRS->SetVariable(L"FboLegacySceOrder", &gFixedBootOrderGuid, 0, 0, NULL);
#endif
        }
#endif

        Status = GetEfiVariable( L"FboUefiSceOrder", &gFixedBootOrderGuid, NULL, &SceVar.SceOrderSize, (VOID**)&SceVar.SceOrder);
        if(!EFI_ERROR(Status))
        {
            SceVar.IsUefi = 1;
            FboSyncWithSce(&SceVar);
            pBS->FreePool((VOID*)SceVar.SceOrder);
            SceVar.SceOrder = NULL;
#if !defined(ALWAYS_PUBLISH_HII_RESOURCES)||(0==ALWAYS_PUBLISH_HII_RESOURCES)
            pRS->SetVariable(L"FboUefiSceOrder", &gFixedBootOrderGuid, 0, 0, NULL);
#endif
        }
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
