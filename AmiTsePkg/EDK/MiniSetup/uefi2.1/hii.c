//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/Hii.c $
//
// $Author: Rajashakerg $
//
// $Revision: 41 $
//
// $Date: 9/18/12 2:14a $
//
//*****************************************************************//
/** @file hii.c
    Contains Hii related Functions

**/
//*************************************************************************

//----------------------------------------------------------------------------
#include "minisetup.h"
#include "TseDrvHealth.h" 
#include "TseUefiHii.h"
#ifndef TSE_FOR_APTIO_4_50
#define EFI_HII_PACKAGE_FORMS                EFI_HII_PACKAGE_FORM
#include "tianohii.h"
#endif
//----------------------------------------------------------------------------
// MACRO DEFINITIONS
//----------------------------------------------------------------------------
#define MAX_DEV_PATH_STR_LEN    100	

//----------------------------------------------------------------------------
// EXTERN VARIABLES
//----------------------------------------------------------------------------
extern UINTN gLangCount;
extern UINTN gDynamicPageCount;
extern UINT32 gRefreshIdCount;
extern REFRESH_ID_INFO 	*gRefreshIdInfo;
extern DYNAMIC_PAGE_GROUP *gDynamicPageGroup ;
//----------------------------------------------------------------------------
// VARIABLE DECLARATIONS
//----------------------------------------------------------------------------
static SETUP_LINK 		*gSetupData = NULL;
static EFI_HII_HANDLE 	*gSetupHandles;
static EFI_HANDLE 		gNotifyHandle[3];
EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *gDevPathToTextProtocol = (EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *)NULL;
EFI_GUID 			gDevPathToTextGuid 							= EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
VOID 					*gFirstPageRef;
PAGE_ID_INFO 		*PageIdInfoPtr;
PAGE_ID_LIST 		*PageIdListPtr;
PAGE_INFO 			*PageInfoPtr;
PAGE_INFO 			*FirstPage;
PAGE_INFO 			*NewPageInfo;
PAGE_LIST 			*PageListPtr;
VARIABLE_LIST 		*VariableListPtr;
VARIABLE_INFO 		*VariableInfoPtr;
BOOLEAN 				gHiiInitialized = FALSE;
UINT32 				AllocatedFirstPageSize, FirstPageOffset;
UINT32 				ControlListSize, ControlListOffset;
UINT32 				PageIdListSize, PageIdListOffset;
UINT32 				PageIdInfoSize, PageIdInfoOffset;
UINT32 				PageListSize, PageListOffset;
UINT32 				PageInfoSize, PageInfoOffset;
UINT32 				VariableListSize, VariableListOffset;
UINT32 				VariableInfoSize, VariableInfoOffset;
UINTN 				TotalRootPages;
UINTN 				gSetupCount = 0;
EFI_GUID 			*gGuidDump=NULL;		// offline vfr pages hided
UINTN    			gGuidDumpCount=0;
UINTN               gDynamicPageCount=0;
UINTN               gDynamicPageGroupCount=0;
EFI_HANDLE *gNewHandleList;
UINTN gNewHandleCount;

//----------------------------------------------------------------------------
// FUNCCTION DECLARATIONS
//----------------------------------------------------------------------------
EFI_HII_HANDLE 		*_HiiGetHandles (UINT16 *NumberOfHiiHandles);
EFI_IFR_FORM_SET 		*_HiiGetFormSet (UINTN index);


EFI_STATUS 				_HiiLocateSetupHandles (VOID);
EFI_STATUS 				_MergePageIdListAndInfo (VOID);
EFI_STATUS 				_MergePageListAndInfo (VOID);
EFI_STATUS 				_MergeVariableListAndInfo (VOID);
EFI_STATUS 				_GetClassSubClassID (SETUP_LINK *SetupData);
EFI_STATUS 				GetAcknowledgementKey ();
EFI_STATUS 				_ReInitializeSetupdataPointers ();

UINT16 					_IsFormFound (EFI_IFR_FORM_SET *formSet, UINT16 formid);
UINT16 					_HiiGetLinkIndex (EFI_GUID *guid, UINT16 class, UINT16 subclass, UINT16 formid);
UINT16 					_HiiGetLinkIndexForXml(EFI_GUID *guid, UINT16 class, UINT16 subclass);
VOID 						UIUpdateCallback (VOID * Handle, UINT32 OldVariableCount);
EFI_STATUS _FxiupNotificationData(SETUP_LINK *Setup_Link);

EFI_STATUS 				UpdateIFRRefXControls (PAGE_INFO *PageInfo, CONTROL_INFO * CtrlInfo);
EFI_STATUS				GetPageIdFromGuid (EFI_GUID *FormGuid, UINT16 PageFormID, UINT16 *PageID);
VOID 						SetDriverHealthCount (VOID);//Driver health support
BOOLEAN  				IsDriverHealthSupported (VOID);
EFI_STATUS 				CreateEventforIFR (CONTROL_INFO *control_Info );
VOID                    SetDynamicPageGroupCount(VOID);
//EFI_GUID gDevPathToTextGuid = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
//EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *gDevPathToTextProtocol = (EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *)NULL;

EFI_IFR_REF *gRefData = NULL;
BOOLEAN isSubPageRootPage(PAGE_INFO *info);
UINTN 		GetSubPageAsRootPageCount(VOID);
VOID 				FixMergePagesExtraHook (VOID *gPages, VOID *tempPages);
VOID 				PageRemoveHook (UINTN PageIndex, void *pageInfo, 	void *PageHandle, UINT16	PageFormID);
VOID InitGlobalPointers( VOID );
EFI_STATUS	InitApplicationData(EFI_HANDLE ImageHandle);
VOID FixHiddenPageList(VOID);
VOID InitializeDynamicCount(VOID);
VOID IncrementDynamicPageCount(UINT16);
VOID DecrementDynamicPageCount(UINT16);

VOID AddNewHandleIntoList(EFI_HANDLE Handle);
////////////////////////////////////
//	Externing Functions
///////////////////////////////////
extern BOOLEAN   IsSupportDefaultForStringControl (VOID);
extern BOOLEAN ShowClassGuidFormsets (TSE_EFI_IFR_FORM_SET *FormSet );

/**
    Get HII form package by handle and form ID

    @param Handle 
    @param form 
    @param Length 

    @retval VOID *formPkBuffer - 
**/
VOID *HiiGetForm( EFI_HII_HANDLE Handle, UINT16 form, UINTN *Length )
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HII_PACKAGE_HEADER *pkgHdr;
	EFI_IFR_OP_HEADER *opHeader = NULL;

	VOID *hiiFormPackage=NULL;
	UINT8 *formPkBuffer = NULL;
	UINT8 *pkgOffset;
//	UINT8 *msgBoxSel = NULL;
	UINT8 end = 0;
	UINT32 offset = 0;
	UINTN len=10;
	SETUP_LINK DummySetupLink;

	if(Length == NULL)
		Length = &len;

	hiiFormPackage = EfiLibAllocateZeroPool(*Length);

	status = gHiiDatabase->ExportPackageLists(gHiiDatabase, Handle, Length, hiiFormPackage);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		MemFreePointer((void**)&hiiFormPackage);
		//
		// Allocate space for retrieval of IFR data
		//
		hiiFormPackage = EfiLibAllocateZeroPool(*Length);

		if (hiiFormPackage == NULL)
		{
			hiiFormPackage = NULL;
			goto DONE;
		}
		//
		// Get all the packages associated with this HiiHandle
		//
		status = gHiiDatabase->ExportPackageLists (gHiiDatabase, Handle, Length, hiiFormPackage);
		if(EFI_ERROR(status))
		{
			MemFreePointer((void**)&hiiFormPackage);
			hiiFormPackage = NULL;
			goto DONE;
		}
	}else if(EFI_ERROR(status))
	{
		hiiFormPackage = NULL;
		goto DONE;
	}

	offset = sizeof(EFI_HII_PACKAGE_LIST_HEADER);

	while(offset < ((EFI_HII_PACKAGE_LIST_HEADER*)hiiFormPackage)->PackageLength)
	{
		pkgOffset = ((UINT8*)hiiFormPackage) + offset;
		pkgHdr = (EFI_HII_PACKAGE_HEADER*)pkgOffset;
		switch(pkgHdr->Type)
		{
			case EFI_HII_PACKAGE_FORMS:
				formPkBuffer = (UINT8*)EfiLibAllocateZeroPool(pkgHdr->Length);
				if(formPkBuffer == NULL)
				{
					goto DONE;
				}
				MemCopy(formPkBuffer, pkgHdr, pkgHdr->Length);
				opHeader = (EFI_IFR_OP_HEADER*)((UINT8*)formPkBuffer + sizeof(EFI_HII_PACKAGE_HEADER));
				if(opHeader->OpCode != EFI_IFR_FORM_SET_OP)
				{
#ifdef STANDALONE_APPLICATION
					CHAR16 *Temp = L"ERROR! Form Package contains No FormSet. Skipping Package Processing.\x0a\x0dPress a Key to Continue :\x0a\x0d";
					gST->ConOut->OutputString(gST->ConOut,Temp);
					status = GetAcknowledgementKey();
					if(EFI_ERROR(status))
					{
						gST->ConOut->OutputString(gST->ConOut, L"GetAcknowledgementKey returned ERROR!");
					}
#endif
					MemFreePointer((void**)&formPkBuffer);
					formPkBuffer = NULL;
					*Length = 0;
					goto DONE;
				}
				// to check is the formset is EFI_SETUP_APPLICATION_SUBCLASS.
				MemSet(&DummySetupLink,sizeof(DummySetupLink),0);
				DummySetupLink.FormSet = formPkBuffer;
				if(_GetClassSubClassID(&DummySetupLink) == EFI_UNSUPPORTED)
				{
					MemFreePointer((void**)&formPkBuffer);
					formPkBuffer = NULL;
					*Length = 0;
					goto DONE;
				}
				end = 1;
				break;
			case EFI_HII_PACKAGE_END:
				end = 1;
				break;
			default:
				break;
		}
		if (end)
		{
			break;
		}
		offset += pkgHdr->Length;
	}
	MemFreePointer((void**)&hiiFormPackage);
	if(form && (NULL != formPkBuffer))
	{
	    UINT8 *tempForm = NULL;
		tempForm = GetFormByFormID((EFI_HII_PACKAGE_HEADER *)formPkBuffer, form, Length);
		MemFreePointer((void**)&formPkBuffer);
		formPkBuffer = tempForm;
	}

DONE:
	return formPkBuffer;
}

/**

    @param DefValue 

    @retval EFI_STATUS The status
**/

EFI_STATUS _GetClassSubClassID(SETUP_LINK *SetupData)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)NULL;
  BOOLEAN flag = TRUE;
  INTN inScope = 0;

  if(SetupData->FormSet == NULL)
  {
    return EFI_UNSUPPORTED;
  }

  SetupData->ClassID = 0;
  SetupData->SubClassID = 0;
  opHeader = (EFI_IFR_OP_HEADER*)(SetupData->FormSet + sizeof(EFI_HII_PACKAGE_HEADER));
  do
  {
    switch(opHeader->OpCode)
    {
    case EFI_IFR_GUID_OP:
      {
        UINT8 extOpcode = (UINT8)*((UINT8*)opHeader + sizeof(EFI_IFR_GUID));
        switch(extOpcode)
        {
        case EFI_IFR_EXTEND_OP_CLASS:
          {
            EFI_IFR_GUID_CLASS *guidClass = (EFI_IFR_GUID_CLASS*)opHeader;
            SetupData->ClassID = guidClass->Class;
          }
          break;
        case EFI_IFR_EXTEND_OP_SUBCLASS:
          {
            EFI_IFR_GUID_SUBCLASS *guidSubClass = (EFI_IFR_GUID_SUBCLASS*)opHeader;
            SetupData->SubClassID = guidSubClass->SubClass;
            flag = FALSE;
          }
          break;
        }
      }
	  break;
	case EFI_IFR_END_OP:
		if(inScope)
		{
		  inScope--;
		}
    default:
      break;
    }

	if(opHeader->Scope)
	{
	  inScope++;
	}
	opHeader = (EFI_IFR_OP_HEADER*)((UINT8*)opHeader + opHeader->Length);
  } while(flag && inScope > 0);

  return status;
}

/**

    @param 

    @retval EFI_STATUS
**/
EFI_STATUS _MergePageIdListAndInfo()
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN i;

	if((PageIdListOffset + PageIdInfoOffset) == 0)
	{
		gPageIdList = NULL;
		gPageIdInfo = NULL;
		return status; // NO Formset to show.
	}

	gPageIdList = (PAGE_ID_LIST*)EfiLibAllocateZeroPool(PageIdListOffset + PageIdInfoOffset);
	if(gPageIdList == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	MemCopy(gPageIdList,PageIdListPtr,PageIdListOffset);
	MemCopy(((UINT8 *)gPageIdList)+ PageIdListOffset,PageIdInfoPtr,PageIdInfoOffset);

	for(i=0; i<gPageIdList->PageIdCount; i++)
	{
		gPageIdList->PageIdList[i] += PageIdListOffset;
	}

	gPageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[0]);

	//Free Temp Memory
	MemFreePointer((void**)&PageIdInfoPtr);
	MemFreePointer((void**)&PageIdListPtr);

DONE:
	return status;
}
/**
    Fixes control information for special control in the varstore

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS SpecialControlsFixup()
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN i,j;
	VARIABLE_INFO *varInfo = NULL;

	for(i = 0; i < gPages->PageCount; i++)
	{
		PAGE_INFO *PageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[i]);
		if(!PageInfo){
			continue;
		}
		for ( j = 0; j < PageInfo->PageControls.ControlCount; j++ )
		{
			CONTROL_INFO *ctrlInfo = (CONTROL_INFO*)(((UINTN)gControlInfo) + PageInfo->PageControls.ControlList[j]);
			if(!ctrlInfo){
				continue;
			}

			UpdateIFRRefXControls(PageInfo, ctrlInfo); //Update the CONTROL_INFO with REFX information

			
		}
	}
	for ( i = 0; i < gVariables->VariableCount; i++)
	{
		varInfo = (VARIABLE_INFO *)((UINT8 *)gVariables + gVariables->VariableList[i]);
		if(!varInfo){
			continue;
		}
		for(j = 0; j < gGuidList->GuidCount; j++)
        {
			GUID_INFO *GuidInfo = (GUID_INFO *)((UINTN)gGuidList + gGuidList->GuidList[j]);
			if(GuidInfo && EfiCompareGuid(&varInfo->VariableGuid, &GuidInfo->GuidValue))
			{
				switch(GuidInfo->GuidKey)  
				{
					//case GUID_KEY_NO_COMMIT:
					//	varInfo->ExtendedAttibutes = AMI_SPECIAL_NO_COMMIT_VARIABLE;
					//	break;
					default:
						break;
				}
			}
		}
	}

	return status;
}
/**

    @param 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_OUT_OF_RESOURCES
**/
EFI_STATUS _MergePageListAndInfo()
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN i;

    SETUP_DEBUG_UEFI ( "\n[TSE] Entering _MergePageListAndInfo()\n" );

    
	gApplicationData = (UINT8*)EfiLibAllocateZeroPool(PageListOffset + FirstPageOffset + PageInfoOffset);
	if(gApplicationData == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	gPages = (PAGE_LIST *) gApplicationData;
	MemCopy(gApplicationData, PageListPtr, PageListOffset);
	MemCopy(gApplicationData + PageListOffset, FirstPage, FirstPageOffset);
	MemCopy(gApplicationData + PageListOffset + FirstPageOffset, PageInfoPtr, PageInfoOffset);

	//Fix Offsets
	i=0;
	gPages->PageList[i] = PageListOffset;
	i++;

	for(; i < gPages->PageCount; i++)
	{
		gPages->PageList[i] += (FirstPageOffset + PageListOffset);
	}

	//Free Temp Memory
	MemFreePointer((void**)&FirstPage);
	MemFreePointer((void**)&PageInfoPtr);
	MemFreePointer((void**)&PageListPtr);
    
	SpecialControlsFixup(); //Fixes control information for special control in the varstore
DONE:
    SETUP_DEBUG_UEFI ( "\n[TSE] Exitingg _MergePageListAndInfo(),  status = 0x%x \n" , status );

	return status;
}

/**

    @param 

    @retval EFI_STATUS
**/
EFI_STATUS _MergeVariableListAndInfo()
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN i;

	//Try to reallocate the existing Variable Cache.
	if (gVariables->VariableCount != VariableListPtr->VariableCount)		//Reallocating only when counts varies
	{
		if(gVariableList != NULL)
			gVariableList = MemReallocateZeroPool( gVariableList, sizeof(NVRAM_VARIABLE) * gVariables->VariableCount, VariableListPtr->VariableCount * sizeof(NVRAM_VARIABLE));
		
		if(gOptimalDefaults != NULL)
			gOptimalDefaults = MemReallocateZeroPool( gOptimalDefaults, sizeof(NVRAM_VARIABLE) * gVariables->VariableCount, VariableListPtr->VariableCount * sizeof(NVRAM_VARIABLE));
		
		if(gFailsafeDefaults != NULL)
			gFailsafeDefaults = MemReallocateZeroPool( gFailsafeDefaults, sizeof(NVRAM_VARIABLE) * gVariables->VariableCount, VariableListPtr->VariableCount * sizeof(NVRAM_VARIABLE));
	}

	gVariables = (VARIABLE_LIST*)EfiLibAllocateZeroPool(VariableListOffset + VariableInfoOffset);
	if(gVariables == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	MemCopy(gVariables,VariableListPtr,VariableListOffset);
	MemCopy(((UINT8 *)gVariables)+ VariableListOffset,VariableInfoPtr,VariableInfoOffset);

	for(i=0; i<gVariables->VariableCount; i++)
	{
		gVariables->VariableList[i] += VariableListOffset;
	}

	//Free Temp Memory
	MemFreePointer((void**)&VariableInfoPtr);
	MemFreePointer((void**)&VariableListPtr);

DONE:
	return status;
}

/**

    @param 

    @retval EFI_STATUS Return the Status
**/
VOID RTIfrUnRegFormNotificationWrapper (VOID);
EFI_STATUS HiiExit()
{
	UINTN i;

	//Remove gSetupHandles
	MemFreePointer((VOID**)&gSetupHandles);

	// Remove memory held by gSetupData
	for ( i = 0; i < gSetupCount; i++ )
    {
		//Free memory only if gSetupData[i].FormSetLength is non-zero.
      //  if(gSetupData[i].FormSetLength)
            MemFreePointer((void**)&(gSetupData[i].FormSet));
    }
	MemFreePointer((void**)&gSetupData);
	gSetupCount = 0;

	MemFreePointer((VOID**)&gGuidDump); 
	MemFreePointer((void**)&gControlInfo);
	ControlListSize = ControlListOffset = 0;
	
	if(gDynamicPageGroup!=NULL)
	    MemFreePointer((void**)&gDynamicPageGroup);
	
	if(gRefData!=NULL)
	    MemFreePointer((void**)&gRefData);

	MemFreePointer((void**)&gApplicationData);
	gPages = gToolPages;
	PageListPtr = NULL;
	PageInfoPtr = NULL;
	FirstPage = NULL;
	AllocatedFirstPageSize = FirstPageOffset = PageListSize = PageListOffset = PageInfoSize = PageInfoOffset = 0;

//#if SETUP_RUNTIME_IFR_PROCESSING
	RTIfrProcessExitWrapper();
//#endif

	gVariables = gToolVariables;
	gPageIdList = gToolPageIdList;
	gPageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[0]);

	RTIfrUnRegFormNotificationWrapper ();	
	return EFI_SUCCESS;
}

/**
    Unregisters the form notification

    @param VOID

    @retval VOID
**/
VOID UnRegFormNotification (VOID)
{
	UnRegisterFormNotification (gNotifyHandle[0]);
	UnRegisterFormNotification (gNotifyHandle[1]);
	UnRegisterFormNotification (gNotifyHandle[2]);
}

VOID FixupPage0ControlInfo( UINTN FormSet, UINTN ControlPtr, EFI_HII_HANDLE Handle)
{
	PAGE_INFO *pageInfo = (PAGE_INFO *)(((UINT8 *)gSetupPkg) + gToolPages->PageList[0]);
	CONTROL_INFO *controlInfo = NULL;
	UINT32 j;

	if(pageInfo->PageControls.ControlCount)
	{
		for ( j = 0; j < pageInfo->PageControls.ControlCount; j++ )
		{
			controlInfo = (CONTROL_INFO*)((UINTN)gToolControlInfo + pageInfo->PageControls.ControlList[j]);

#if TSE_APTIO_5_SUPPORT			
			controlInfo->ControlFlags.ControlVisible = 0 ; // Hide Page[0] controls for Legacy Style
#endif			
			if( (ControlPtr-FormSet) == (UINTN)controlInfo->ControlPtr )
			{
				controlInfo->ControlHandle = Handle;
				controlInfo->ControlPtr = (VOID*)ControlPtr;
			}else{
				if(controlInfo->ControlType == CONTROL_TYPE_NULL)
					controlInfo->ControlHandle = Handle;
			}
		}
	}

}
/**
    Registers the hii pack notification

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS RegFormNotification (VOID)
{
	EFI_STATUS status = EFI_SUCCESS;

	status = RegisterFormNotification(IFRChangeNotifyFn, EFI_HII_DATABASE_NOTIFY_NEW_PACK, &gNotifyHandle[0]);
	if (EFI_ERROR(status))
	{
		return status;
	}
	status = RegisterFormNotification(IFRChangeNotifyFn, EFI_HII_DATABASE_NOTIFY_REMOVE_PACK, &gNotifyHandle[1]);
	if (EFI_ERROR(status))
	{
		return status;
	}
	status = RegisterFormNotification(IFRChangeNotifyFn, EFI_HII_DATABASE_NOTIFY_ADD_PACK, &gNotifyHandle[2]);
	if (EFI_ERROR(status))
	{
		return status;
	}
	return EFI_SUCCESS;
}

/**

    @param 

    @retval EFI_STATUS Return the Status
**/
EFI_STATUS RTIfrRegFormNotificationWrapper (VOID);
extern EFI_GUID 	RefreshEventGroupId;
EFI_STATUS HiiFixupData()
{
	EFI_STATUS status = EFI_SUCCESS;

	PAGE_INFO	*pageInfo = NULL;
	EFI_IFR_FORM_SET *formSet;
	EFI_IFR_REF *ref = NULL;
	EFI_IFR_REF *href = NULL;
	UINTN RootPageCount = 0 ;
    //UINT16      *varString;
    //UINT16      StringToken = 0;
	UINT16      link;
	UINT32      i=0;
	UINT32      j=0;
	UINT8 	*tformSet = NULL;
	UINTN 	length = 0;

   SETUP_DEBUG_UEFI ( "\n[TSE] Entering HiiFixupData()\n" );
   if (PageListOffset || FirstPageOffset || PageInfoOffset)          // If handofftse invoked second time then offsets incrementing from current so exiting and reforming.
   {
      HiiExit();
   }
	status = RTIfrRegFormNotificationWrapper ();			
	if (EFI_ERROR (status))
		goto DONE;
	status = _HiiLocateSetupHandles();
	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	gSetupData = EfiLibAllocateZeroPool(sizeof(SETUP_LINK) * gSetupCount);
	if(gSetupData == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	for(i = 0; i < gSetupCount - 1; i++)
	{
		tformSet = (UINT8*)HiiGetForm(gSetupHandles[i], 0, &length);
		if (NULL != tformSet)
		{
			EFI_IFR_FORM_SET 			*formset = NULL;
			
			formset = ((EFI_IFR_FORM_SET*)((UINT8*)(tformSet)+sizeof(EFI_HII_PACKAGE_HEADER)));			
			//Skip to fill gSetupData for formset which has different Formset ClassGUID
			if ( !ShowClassGuidFormsets( (TSE_EFI_IFR_FORM_SET*)(formset) ) )
			{
				if (NULL != tformSet)
				{
					MemFreePointer((void**)&tformSet);
				}
				continue;
			}
			gSetupData[i].FormSet = HiiGetForm(gSetupHandles[i], 0, &(gSetupData[i].FormSetLength));
			if (NULL != tformSet)
			{
				MemFreePointer((void**)&tformSet);
			}
		}
		if(gSetupData[i].FormSet == NULL)
		{
			// make sure the handle is also zero
			gSetupData[i].Handle = 0 ;
			continue;
		}
		status = ValidateParseForm(gSetupData[i].FormSet);
		if (EFI_ERROR(status))
		{
			gSetupData[i].Handle = 0 ;
			MemFreePointer((void**)&(gSetupData[i].FormSet));
			continue;
		}
		gSetupData[i].Handle = gSetupHandles[i];
		formSet = (EFI_IFR_FORM_SET*)_HiiGetFormSet(i);

		_GetClassSubClassID(&gSetupData[i]);

#if TSE_DEBUG_MESSAGES
	    WritePackToFile(SETUPDATAFILENAME, 
				gSetupData[i].Handle, 
				NULL,
				gSetupData[i].FormSetLength) ;
#endif
	}

	RootPageCount = GetSubPageAsRootPageCount(); //Get the number of SubPages as root pages
	        
	if(RootPageCount)
	{
	    gRefData = EfiLibAllocateZeroPool( RootPageCount * sizeof(EFI_IFR_REF));
	    if(gRefData == NULL)
	    {
	        status = EFI_OUT_OF_RESOURCES;
	        goto DONE;
	    }
	    href = (EFI_IFR_REF *)gRefData ;
	} 

	// need to allocate one more dynamic IFR for the root page, this is NOT a 100% valid
	// formset as there is no form defined
	gSetupData[i].FormSet = EfiLibAllocateZeroPool(sizeof(EFI_HII_PACKAGE_HEADER) +
			sizeof(EFI_IFR_FORM_SET) + /*i*/ (gToolPages->PageCount+gSetupCount) * sizeof(EFI_IFR_REF)); // No. of root pages can be more then hii handle because of AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST_SUPPORT
	if(gSetupData[i].FormSet == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	formSet = (EFI_IFR_FORM_SET*)_HiiGetFormSet(i);
	MemCopy(&(formSet->Guid),&(gSetupPkg->PackageGuid),sizeof(EFI_GUID));

	TotalRootPages = 0;
	if ( gSetupData[i].FormSet != NULL )
	{
		ref = (EFI_IFR_REF *)(gSetupData[i].FormSet + sizeof(EFI_HII_PACKAGE_HEADER) + sizeof(EFI_IFR_FORM_SET));
		for ( j = 1; j <= gToolPages->PageCount; j++ )
		{
			pageInfo = (PAGE_INFO *)(((UINT8 *)gSetupPkg) + gToolPages->PageList[j]);
			if ( pageInfo->PageParentID != 0 ) //if ( pageInfo->PageFormID != 1 ) 
			{
				continue;
			}

			TotalRootPages++;

			link = _HiiGetLinkIndex(&(gToolPageIdInfo[pageInfo->PageIdIndex].PageGuid),
						gToolPageIdInfo[pageInfo->PageIdIndex].PageClass,
						gToolPageIdInfo[pageInfo->PageIdIndex].PageSubClass,
						pageInfo->PageFormID);

			if ( link == (gSetupCount - 1) )
			{
				continue;
			}

			formSet = (EFI_IFR_FORM_SET*)_HiiGetFormSet(link);
			//Suppress the warnings from static code analyzer
			if(NULL == formSet){
				continue;
			}
			ref->Header.OpCode = EFI_IFR_REF_OP;
			ref->Header.Length = sizeof(EFI_IFR_REF);
			ref->Question.Header.Prompt = formSet->FormSetTitle;
			ref->Question.Header.Help = formSet->Help;
			FixupPage0ControlInfo( (UINTN)gSetupData[i].FormSet, (UINTN)ref, gSetupData[link].Handle);
			ref++;
		}
		gSetupData[i].Handle = gSetupData[0].Handle;
	}
	
	gDynamicPageGroup = EfiLibAllocateZeroPool(sizeof(DYNAMIC_PAGE_GROUP) * DYNAMIC_PAGE_GROUPS_SIZE );
    if(gDynamicPageGroup == NULL)
    {
        status = EFI_OUT_OF_RESOURCES;
        goto DONE;
    }
	
//#if SETUP_RUNTIME_IFR_PROCESSING
	RTIfrProcessAddVarListAndPageIDListWrapper();
//#endif //SETUP_RUNTIME_IFR_PROCESSING
//offline vfr pages hided 
	i = 0;
    if(gGuidDump == NULL)
    {
        gGuidDumpCount = gToolPages->PageCount;
    	gGuidDump = EfiLibAllocateZeroPool (sizeof (EFI_GUID) * gGuidDumpCount);
        j = 0;
    }
    else
    {
        j = (UINT32) gGuidDumpCount;
		gGuidDump = MemReallocateZeroPool (gGuidDump, gGuidDumpCount * sizeof (EFI_GUID), (gGuidDumpCount+gToolPages->PageCount) * sizeof (EFI_GUID));
        gGuidDumpCount += gToolPages->PageCount;
    }

	if (NULL != gGuidDump)
	{
		for (i = 0; i < gToolPages->PageCount; i++)
		{
			PAGE_INFO	*ParsepageInfo = NULL;
			ParsepageInfo = (PAGE_INFO *)(((UINT8 *)gSetupPkg) + gToolPages->PageList [i]);

			EfiCopyMem (&gGuidDump [i+j], &gToolPageIdInfo [ParsepageInfo->PageIdIndex].PageGuid, sizeof (EFI_GUID));
		}	
	}
//offline vfr pages hided 
	// update the control (IFR) pointers and HII handles
	for ( i = 0; i< gToolPages->PageCount; i++ )
	{
//		PAGE_INFO *NewPageInfo;
		UINT32 PageSize;
		UINT32 ControlSize;
		CONTROL_INFO *NewControlInfo;
		UINTN NoOfControlSpace;

		pageInfo = (PAGE_INFO *)(((UINT8 *)gSetupPkg) + gToolPages->PageList[i]);

		if(i == 0)
		{
			link = (UINT16)(gSetupCount - 1);
		}
		else
		{
			link = _HiiGetLinkIndex( &(gToolPageIdInfo[pageInfo->PageIdIndex].PageGuid),
					gToolPageIdInfo[pageInfo->PageIdIndex].PageClass,
					gToolPageIdInfo[pageInfo->PageIdIndex].PageSubClass,
					pageInfo->PageFormID);
		}

		NoOfControlSpace = pageInfo->PageControls.ControlCount ? pageInfo->PageControls.ControlCount - 1 : pageInfo->PageControls.ControlCount;
        PageSize = (UINT32)(sizeof(PAGE_INFO) + sizeof(pageInfo->PageControls.ControlList) * NoOfControlSpace);
		NewPageInfo = EfiLibAllocateZeroPool(PageSize);
		if (NewPageInfo == NULL)
		{
			status = EFI_OUT_OF_RESOURCES;
			goto DONE;
		}
		MemCopy(NewPageInfo,pageInfo,PageSize);

		if ( link == (gSetupCount - 1) )
		{
			NewPageInfo->PageHandle = 0;// This page has been removed in runtime!!!
		}
		else
		{
//#if SETUP_RUNTIME_IFR_PROCESSING
			if(RTIfrProcessFormIfUpdatedWrapper(link) == TRUE)
				continue;
//#endif //SETUP_RUNTIME_IFR_PROCESSING

        	NewPageInfo->PageHandle = gSetupData[link].Handle;
		}
		
		if(NewPageInfo->PageFlags.PageRefreshID)
		{
			//Check refresh id opcode for page
			CheckRefreshIDForPage(NewPageInfo,link);
		}
		//Fixup and add the controls in this page to gContolInfo and update offsets in page info

		if(NewPageInfo->PageControls.ControlCount)
		{
		for ( j = 0; j < NewPageInfo->PageControls.ControlCount; j++ )
		{
			VARIABLE_INFO *varInfo = NULL;
			CONTROL_INFO *controlInfo = (CONTROL_INFO*)((UINTN)gToolControlInfo + NewPageInfo->PageControls.ControlList[j]);

			//Updated the contol size to copy the Default information for the control including padding which is included from Asm file.
			if(controlInfo->ControlDataWidth == 1) /* 1 byte padding */
				ControlSize = sizeof(CONTROL_INFO) + controlInfo->ControlDataWidth * 2 + controlInfo->DefaultStoreCount * (sizeof(UINT16) + controlInfo->ControlDataWidth * 2) ;
			else /* [controlInfo->ControlDataWidth - sizeof(UINT16)] byte padding */
				ControlSize = sizeof(CONTROL_INFO) + controlInfo->ControlDataWidth * 2 + controlInfo->DefaultStoreCount * (2 * controlInfo->ControlDataWidth) ;

			NewControlInfo = EfiLibAllocateZeroPool(ControlSize);
			if (NewControlInfo == NULL)
			{
				status = EFI_OUT_OF_RESOURCES;
				goto DONE;
			}
			MemCopy(NewControlInfo, controlInfo, ControlSize);

			if(NewPageInfo->PageHandle != 0)
			{
				NewControlInfo->ControlHandle = gSetupData[link].Handle;

				if ( NewControlInfo->ControlPtr != 0 )
				{
					NewControlInfo->ControlPtr = (VOID *)((UINTN)NewControlInfo->ControlPtr + (UINTN)gSetupData[link].FormSet);
					
					if ( NewControlInfo->ControlFlags.RefreshID )
					{
						UINT32	index=0;
						UINT8   *pOpcodeHeader=(UINT8 *)NewControlInfo->ControlPtr;
						//search for the EFI_IFR_REFRESH_ID_OP.if Found then only type cast and assign RefreshEventGroupId and create event for IFR.
						while(pOpcodeHeader)
						{
							if(EFI_IFR_REFRESH_ID_OP == ((EFI_IFR_OP_HEADER *)(pOpcodeHeader))->OpCode)
							{
								MemCopy (&RefreshEventGroupId, &(((AMI_EFI_IFR_REFRESH_ID *)(pOpcodeHeader))->RefreshEventGroupId), sizeof (EFI_GUID));
								CreateEventforIFR (NewControlInfo);
								break;
							}
							if((((EFI_IFR_OP_HEADER *)(pOpcodeHeader))->Scope))//if OpcodeHeader has scope then, Increment index
								index++;
							if(EFI_IFR_END_OP == ((EFI_IFR_OP_HEADER *)(pOpcodeHeader))->OpCode)//if EFI_IFR_END_OP found then,decrement index
								index--;
							if(!index)
								break;
							pOpcodeHeader+=((EFI_IFR_OP_HEADER *)(pOpcodeHeader))->Length;//point to next opcode header depending upon the Length
						} 
					}
					//Patch to set Hpk checkbox control as CONTROL_TYPE_CHECKBOX
					if(((EFI_IFR_OP_HEADER*)NewControlInfo->ControlPtr)->OpCode == EFI_IFR_CHECKBOX_OP)
					{
						NewControlInfo->ControlType = GetCheckBoxStyle(NewControlInfo);
					}

					if ( NewControlInfo->ControlConditionalPtr != 0 )
					{
						NewControlInfo->ControlConditionalPtr = (VOID *)((UINTN)NewControlInfo->ControlConditionalPtr + (UINTN)NewControlInfo->ControlPtr);
					}
				}
				else if ( NewControlInfo ->ControlConditionalPtr != 0 )
				{
					NewControlInfo ->ControlConditionalPtr = (VOID *)((UINTN)NewControlInfo ->ControlConditionalPtr + (UINTN)gSetupData[link].FormSet);
				}
				// Get The Variable Info referenced by Control Variable
                if (RTIfrUpdateVariableInfoWrapper (NewControlInfo->ControlVariable, gSetupData[link].Handle) == FALSE)
                {
                    varInfo = VarGetVariableInfoIndex (NewControlInfo->ControlVariable);
                    if(varInfo->VariableHandle == NULL)
                    {
                        varInfo->VariableHandle = gSetupData[link].Handle;
                    }
                    // Commented for suppressing build time parsing. Name value not supported by current vfr compiler. Uncomment it once vfr compiler supports it.
                    /*if (VARIABLE_ATTRIBUTE_NAMEVALUE == (varInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
                    {
                        if (0 == (EfiStrLen (varInfo->VariableName)))
                        {
                            StringToken = (UINT16)((UINT16)(varInfo->NameValueToken [1] << 8) + (UINT8)(varInfo->NameValueToken [0]));
                            varString = HiiGetString (varInfo->VariableHandle, StringToken);
                            EfiStrCpy (varInfo->VariableName, varString);
                        }
                    }*/
                 }
            }
			   if(controlInfo->PageIdIndex != NewPageInfo->PageIdIndex)
			   {
				   UINT16      Temp_link;
				   Temp_link = _HiiGetLinkIndexForXml( &(gToolPageIdInfo[controlInfo->PageIdIndex].PageGuid),
				   gToolPageIdInfo[controlInfo->PageIdIndex].PageClass,
				   gToolPageIdInfo[controlInfo->PageIdIndex].PageSubClass);
				   NewControlInfo->ControlHandle = gSetupData[Temp_link].Handle;
				   if(NewControlInfo->ControlPtr != 0)
				   {
					   NewControlInfo->ControlPtr = (VOID *)((UINTN)controlInfo->ControlPtr + (UINTN)gSetupData[Temp_link].FormSet);
						if ( NewControlInfo->ControlConditionalPtr != 0 )
						{
							NewControlInfo->ControlConditionalPtr = (VOID *)((UINTN)controlInfo->ControlConditionalPtr + (UINTN)NewControlInfo->ControlPtr);
						}
				   }
				   else if(NewControlInfo->ControlConditionalPtr != 0)
					   NewControlInfo->ControlConditionalPtr = (VOID *)((UINTN)controlInfo->ControlConditionalPtr +(UINTN)gSetupData[Temp_link].FormSet);
				   NewControlInfo->PageIdIndex = NewPageInfo->PageIdIndex;
			   }
   			NewPageInfo->PageControls.ControlList[j] = AddControlToList(NewControlInfo,ControlSize); //Update new offset in gControlInfo
   			MemFreePointer((void**)&NewControlInfo);
			}
		}
		else
		{
			// Add the Empty Control to control List even if NewPageInfo->PageControls.ControlCount = 0
			CONTROL_INFO *controlInfo = (CONTROL_INFO*)((UINTN)gToolControlInfo + 0);
			ControlSize = sizeof(CONTROL_INFO) + controlInfo->ControlDataWidth * 2;
			NewControlInfo = EfiLibAllocateZeroPool(ControlSize);
			MemCopy(NewControlInfo, controlInfo, ControlSize);
			AddControlToList(NewControlInfo,ControlSize); //Update new offset in gControlInfo
			MemFreePointer((void**)&NewControlInfo);
		}

		if(i==0)
		{
			status = CreatePage(&FirstPage, &AllocatedFirstPageSize, &FirstPageOffset, NewPageInfo, PageSize);
		}
		else
		{
			status = AddPageToList(NewPageInfo,PageSize);
			
			//Find the AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST, 
			//then add submenu controls to the main form that points to these pages.
            if(isSubPageRootPage(NewPageInfo) && RootPageCount)
            {
                UINT32 controlOffset = 0;
                CONTROL_INFO *control_Info;
                
                // add NULL control above submenu in main
                controlOffset = 0;
                CreatePage(&FirstPage, &AllocatedFirstPageSize, &FirstPageOffset,
                    &controlOffset, sizeof(UINT32));

                // add second null control for splitting the page in 2 halves
                if (TotalRootPages == 7)
                {
                    controlOffset = 0;
                    CreatePage(&FirstPage, &AllocatedFirstPageSize, &FirstPageOffset,
                        &controlOffset, sizeof(UINT32));
                }

                control_Info = (CONTROL_INFO*)EfiLibAllocateZeroPool(sizeof(CONTROL_INFO));
                if(control_Info == NULL)
                {
                    status = EFI_OUT_OF_RESOURCES;
                    goto DONE;
                }

                // add the submenu control to main form  that points to this form.
                control_Info->ControlHandle = NewPageInfo->PageHandle; 
                control_Info->ControlType = CONTROL_TYPE_SUBMENU;
                control_Info->ControlDestPageID = i ;
                control_Info->ControlFlags.ControlVisible = 1;
                
                href->Header.OpCode = EFI_IFR_REF_OP;
                href->Header.Length = sizeof(EFI_IFR_REF);
                href->Question.Header.Prompt = NewPageInfo->PageSubTitle ; 
                href->Question.Header.Help = 0 ; 
         
                control_Info->ControlPtr =  (VOID*)href ; 
                    
                controlOffset = AddControlToList(control_Info, sizeof(CONTROL_INFO));
                CreatePage(&FirstPage, &AllocatedFirstPageSize, &FirstPageOffset,
                    &controlOffset, sizeof(UINT32));
                
                MemFreePointer((VOID**)&control_Info);

                href++;
                TotalRootPages++;
            }
		}
		if(EFI_ERROR(status))
		{
			goto DONE;
		}
		MemFreePointer((void**)&NewPageInfo);
	}

//#if SETUP_RUNTIME_IFR_PROCESSING
	RTIfrProcessRunTimeFormsWrapper(&ref);
//#endif //SETUP_RUNTIME_IFR_PROCESSING

	status = _MergePageListAndInfo();
	if (IsDriverHealthSupported ())
	{
		SetDriverHealthCount ();
	} 
DONE:
    SETUP_DEBUG_UEFI ( "\n[TSE] Exiting HiiFixupData(),  status = 0x%x \n" , status );
	return status;
}

/**

    @param 

    @retval EFI_STATUS
**/
EFI_STATUS _HiiLocateSetupHandles()
{
    EFI_STATUS status;
	UINT16 i = 0;
	UINT16 count;
	EFI_HII_HANDLE	*handleBuffer;
	UINT8 *buffer;
	UINT16 found = 0;

	status = HiiInitializeProtocol();
	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	if(FormBrowserLocateSetupHandles(&handleBuffer,&count)!= EFI_SUCCESS)
	{
		handleBuffer = _HiiGetHandles( &count );
	}

	if ( handleBuffer == NULL )
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	gSetupHandles = (EFI_HII_HANDLE*)EfiLibAllocatePool((count + 1) * sizeof(EFI_HII_HANDLE));
	if ( gSetupHandles == NULL )
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	for ( i = found = 0; i < count; i++ )
	{
		buffer = HiiGetForm( handleBuffer[i], 0, NULL );
		if ( buffer == NULL )
		{
			continue;
		}

		gSetupHandles[found] = handleBuffer[i];
		found++;
		MemFreePointer( (void**)&buffer );
	}

	gSetupCount = found + 1;

	// at this point we have all the setup specific HII handles into our buffer
DONE:

	if(!FormBrowserHandleValid())
		MemFreePointer((VOID**)&handleBuffer);

	return status;
}

/**

    @param 

    @retval UINT16
**/
UINT16 _HiiGetLinkIndex(EFI_GUID *guid, UINT16 class, UINT16 subclass, UINT16 formid)
{
	UINT16	i;
	EFI_IFR_FORM_SET *formSet;
	EFI_HII_PACKAGE_HEADER *pack;

	for ( i = 0; i < gSetupCount - 1; i++ )
	{
		pack = (EFI_HII_PACKAGE_HEADER *)gSetupData[i].FormSet;
		if(pack == NULL)
		{
			continue;
		}
		if ( sizeof(EFI_HII_PACKAGE_HEADER) == pack->Length )
		{
			continue;
		}

		formSet = (EFI_IFR_FORM_SET *)((UINT8 *)pack + sizeof(EFI_HII_PACKAGE_HEADER));

		if(EfiCompareGuid( guid , (VOID *)&(formSet->Guid) ))
		{
			if( gSetupData[i].ClassID == class )
			{
				if( gSetupData[i].SubClassID == subclass )
				{
					if(_IsFormFound(formSet, formid))
					{
						break;
					}
				}
			}
		}
	}

	return i;
}

/**

    @param 

    @retval UINT16
**/
UINT16 _HiiGetLinkIndexForXml(EFI_GUID *guid, UINT16 class, UINT16 subclass)
{
	UINT16	i;
	EFI_IFR_FORM_SET *formSet;
	EFI_HII_PACKAGE_HEADER *pack;
	for ( i = 0; i < gSetupCount - 1; i++ )
	{
		pack = (EFI_HII_PACKAGE_HEADER *)gSetupData[i].FormSet;
		if(pack == NULL)
		{
			continue;
		}
		if ( sizeof(EFI_HII_PACKAGE_HEADER) == pack->Length )
		{
			continue;
		}
		formSet = (EFI_IFR_FORM_SET *)((UINT8 *)pack + sizeof(EFI_HII_PACKAGE_HEADER));
		if(EfiCompareGuid( guid , (VOID *)&(formSet->Guid) ))
		{
			if( gSetupData[i].ClassID == class )
			{
				if( gSetupData[i].SubClassID == subclass )
				{
						break;
				}
			}
		}
	}
	return i;
}
/**

    @param 

    @retval EFI_IFR_FORM_SET
**/
EFI_IFR_FORM_SET *_HiiGetFormSet(UINTN index)
{
		EFI_IFR_FORM_SET *formSet = NULL;

	if ( (index >= gSetupCount) || (gSetupData == NULL))
	{
		return formSet;
	}

	formSet = (EFI_IFR_FORM_SET*)(gSetupData[index].FormSet + sizeof(EFI_HII_PACKAGE_HEADER));

	return formSet;
}

/**
    gets all the HII handles

    @param No of handles

    @retval EFI_HII_HANDLE *
**/

EFI_HII_HANDLE *_HiiGetHandles(UINT16 *NumberOfHiiHandles)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HII_HANDLE *hiiHandleBuffer = NULL;

	UINTN handleBufferLength = 0;

//	gBS->AllocatePool(EfiBootServicesData, handleBufferLength,
//				(VOID **)&hiiHandleBuffer);
	hiiHandleBuffer = EfiLibAllocatePool(handleBufferLength);

	status = gHiiDatabase->ListPackageLists (gHiiDatabase, EFI_HII_PACKAGE_TYPE_ALL, NULL,
					&handleBufferLength, hiiHandleBuffer);
	if (status == EFI_BUFFER_TOO_SMALL)
	{
		//
		// Free the old pool
		//
		MemFreePointer ((VOID**)&hiiHandleBuffer);
		//
		// Allocate new pool with correct value
		//
//		gBS->AllocatePool (EfiBootServicesData, handleBufferLength,
//				(VOID **)&hiiHandleBuffer);
		hiiHandleBuffer = EfiLibAllocatePool(handleBufferLength);

		//
		// Get the Handles of the packages that were registered with Hii
		//
		gBS->SetMem(hiiHandleBuffer,handleBufferLength,0);
		status = gHiiDatabase->ListPackageLists (gHiiDatabase, EFI_HII_PACKAGE_TYPE_ALL,
					  NULL, &handleBufferLength, hiiHandleBuffer);

		if (EFI_ERROR (status))
		{
			MemFreePointer((VOID**)&hiiHandleBuffer);
			hiiHandleBuffer = NULL;
			goto DONE;
		}
	}else if(EFI_ERROR(status))
	{
		MemFreePointer((VOID**)&hiiHandleBuffer);
		hiiHandleBuffer = NULL;
		goto DONE;
	}

	*NumberOfHiiHandles = (UINT16)(handleBufferLength / sizeof(EFI_HII_HANDLE));

DONE:
	return hiiHandleBuffer;
}

/**

    @param 

    @retval UINT16 -
        1 Form Found
        0 Form Not Found
**/
UINT16 _IsFormFound(EFI_IFR_FORM_SET *formSet,UINT16 formid )
{
	EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER*)formSet;
	BOOLEAN flag = TRUE;
	UINT16 found = 0;
	UINTN offset = 0;
	UINTN ScopeCount = 0;

	while(flag)
	{
		switch(opHeader->OpCode)
		{
		case EFI_IFR_FORM_OP:
			{
				EFI_IFR_FORM *formOp = (EFI_IFR_FORM*)opHeader;
				if(formOp->FormId == formid)
				{
					found = 1;
					flag = FALSE;
				}
			}
			break;
        case EFI_IFR_FORM_MAP_OP: 
			{
				AMI_EFI_IFR_FORM_MAP *formMapOp = (AMI_EFI_IFR_FORM_MAP *)opHeader;
				if(formMapOp->FormId == formid)
				{
					found = 1;
					flag = FALSE;
				}
			}
			break;
		case EFI_IFR_END_OP:
			flag = (--ScopeCount)? TRUE:FALSE;
			break;
		default:
			break;
		}
		if(opHeader->Scope)
		{
			ScopeCount++;
		}
		offset += opHeader->Length;
		opHeader = (EFI_IFR_OP_HEADER*)((UINT8*)formSet + offset);
	}

	return found;
}


/**
    HiiFindHandle

    @param guid UINT16 *index

    @retval EFI_HII_HANDLE
**/

EFI_HII_HANDLE HiiFindHandle( EFI_GUID *guid, UINT16 *index )
{
	EFI_HII_HANDLE	*handleBuffer = NULL;
	EFI_HII_HANDLE	handle = INVALID_HANDLE;
	UINT16 found = 0;
	UINT16 i, count;
	UINTN len=10;
	EFI_HII_PACKAGE_LIST_HEADER *hiiPackageList;
	EFI_STATUS          status=EFI_SUCCESS;

	handleBuffer = _HiiGetHandles( &count );
	if ( handleBuffer == NULL )
		return handle;

	for ( i = found = 0; i < count; i++ )
	{
		hiiPackageList = EfiLibAllocateZeroPool(len);

		status = gHiiDatabase->ExportPackageLists(gHiiDatabase, handleBuffer[i], &len, hiiPackageList);
		if(status == EFI_BUFFER_TOO_SMALL)
		{
			MemFreePointer((VOID**)&hiiPackageList);
			//
			// Allocate space for retrieval of IFR data
			//
			hiiPackageList = EfiLibAllocateZeroPool(len);

			if (hiiPackageList == NULL)
			{
				hiiPackageList = NULL;
				goto DONE;
			}
			//
			// Get all the packages associated with this HiiHandle
			//
			status = gHiiDatabase->ExportPackageLists(gHiiDatabase, handleBuffer[i], &len, hiiPackageList);
			if(EFI_ERROR(status))
			{
				MemFreePointer((VOID**)&hiiPackageList);
				hiiPackageList = NULL;
				goto DONE;
			}
		}
		if ( EfiCompareGuid( guid, &hiiPackageList->PackageListGuid ) )
		{
			if ( *index == found )
			{
				handle = handleBuffer[i];
				(*index)++;
				break;
			}
			found++;
		}

		MemFreePointer((VOID**) &hiiPackageList );
	}

DONE:
	MemFreePointer( (VOID **)&handleBuffer );
	return handle;

}


/**
    RTIfrProcessExit

    @param VOID

    @retval VOID
**/
VOID RTIfrProcessExit(VOID)
{
	MemFreePointer((VOID**)&gVariables);
	VariableListPtr = NULL;
	VariableInfoPtr = NULL;
	VariableListOffset = VariableListSize = VariableInfoOffset = VariableInfoSize = 0;

	MemFreePointer((VOID**)&gPageIdList);
	PageIdListPtr = NULL;
	PageIdInfoPtr =  NULL;
	PageIdListOffset = PageIdListSize = PageIdInfoOffset = PageIdInfoSize = 0;
}


/**

    @param VOID

    @retval VOID
**/
VOID RTIfrProcessAddVarListAndPageIDList(VOID)
{
	UINT32	i;

    SETUP_DEBUG_UEFI ( "\n[TSE] Entering RTIfrProcessAddVarListAndPageIDList()\n" );

	//Create Variable List
	for(i=0; i < gToolVariables->VariableCount ; i++)
	{
		VARIABLE_INFO *VariableInfo;

		VariableInfo = (VARIABLE_INFO*)((UINT8*)gToolVariables + gToolVariables->VariableList[i]);
		AddVariableToList(VariableInfo);
	}

	//Create Page ID list
	for(i=0; i < gToolPageIdList->PageIdCount ; i++)
	{
		PAGE_ID_INFO *PageIdInfo;

		PageIdInfo = (PAGE_ID_INFO*)((UINT8*)gToolPageIdList + gToolPageIdList->PageIdList[i]);
		AddPageIdToList(PageIdInfo);
	}

    SETUP_DEBUG_UEFI ( "\n[TSE] Exiting RTIfrProcessAddVarListAndPageIDList()\n" );
}

/**

    @param 

    @retval BOOLEAN
**/
BOOLEAN RTIfrProcessFormIfUpdated(UINT16 link)
{
	UINT32 j;

    SETUP_DEBUG_UEFI ( "\n[TSE] Entering RTIfrProcessFormIfUpdated()\n" );

	//Fix to correctly check if FormSet length has changed and if so Parse it. This fixes the issue with duplicate ClassID and SubClassID.
	
	for(j=1; j < gToolPageIdList->PageIdCount; j++) // PAGE_ID_INFO[0] contains reference to the NULL page
	{
		//Check if the page has changed during run time
		EFI_HII_PACKAGE_HEADER *pack;
		PAGE_ID_INFO *pageIdList = (PAGE_ID_INFO *)((UINT8 *)gToolPageIdList + gToolPageIdList->PageIdList[j]);
		EFI_IFR_FORM_SET *formSet = (EFI_IFR_FORM_SET*)((UINT8 *)gSetupData[link].FormSet + sizeof(EFI_HII_PACKAGE_HEADER));

		// Compare if the FormSet triplet is already available
		if((pageIdList->PageClass == gSetupData[link].ClassID) &&
		   (pageIdList->PageSubClass == gSetupData[link].SubClassID) &&
		   (EfiCompareGuid(&pageIdList->PageGuid, (VOID *)&formSet->Guid)))
		{
			pack = (EFI_HII_PACKAGE_HEADER *)gSetupData[link].FormSet;

			if ( gHpkInfo[j-1].Length != pack->Length )
			{
   	 			break;
			}
		}
	}

	if(j < gToolPageIdList->PageIdCount)
	{
	        //This page changed during run time parse again
	        if(!gSetupData[link].Added)
		{
			ParseForm(&gSetupData[link]);
		}
		return TRUE;
	}


	gSetupData[link].Added = 1;

    SETUP_DEBUG_UEFI ( "\n[TSE] Exiting RTIfrProcessFormIfUpdated()\n" );

	return FALSE;
}


/**

    @param 

    @retval VOID
**/
VOID RTIfrProcessRunTimeForms(EFI_IFR_REF **ref)
{
	UINT32 i;
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_IFR_FORM_SET *formSet;
	EFI_GUID 	DynamicPageGuid = DYNAMIC_PAGE_COUNT_GUID;
	UINTN count = 0;

    SETUP_DEBUG_UEFI ( "\n[TSE] Entering RTIfrProcessRunTimeForms()\n" );

    InitializeDynamicCount();
    if(!IsGroupDynamicPages())   
        gFirstPageRef = (VOID*)(*ref);

	for(i = 0; i < gSetupCount - 1; i++)
	{
		if (NULL == gSetupData[i].Handle)//To avoid call parsing for NULL gSetupData
		{
			continue;
		}

		if(!(gSetupData[i].Added))
		{
			Status = ValidateParseForm(gSetupData[i].FormSet);
			if (Status != EFI_SUCCESS)
			{
				MemFreePointer( (VOID **)&(gSetupData[i].FormSet) );
				gSetupData[i].Handle = NULL;
				continue;
			}
			Status = ParseForm(&gSetupData[i]);
			if((gSetupData[i].Added))
			{
				formSet = _HiiGetFormSet( i );
				if(NULL == formSet){
					continue;
				}
				
                if(!IsGroupDynamicPages())   
                {
                    (*ref)->Header.OpCode = EFI_IFR_REF_OP;
    				(*ref)->Header.Length = sizeof(EFI_IFR_REF);
    				(*ref)->Question.Header.Prompt = formSet->FormSetTitle;
    				(*ref)->Question.Header.Help = formSet->Help;
    				(*ref)++;
    				gFirstPageRef = (VOID*)(*ref);
                }
                
			}
			if (EFI_SUCCESS == Status) //To avoid increment count incase parsing is aborted and avoid showing sub-link under advanced page
			    IncrementDynamicPageCount(gSetupData[i].ClassID);
		}
	}

	_MergePageIdListAndInfo();
	_MergeVariableListAndInfo();

    SETUP_DEBUG_UEFI ( "\n[TSE] Exiting RTIfrProcessRunTimeForms()\n" );
}

/**

    @param 

    @retval EFI_STATUS
**/
EFI_STATUS HiiLoadDefaults( VOID **varList, UINT32 Mask )
{
  // This function needs to be revisited once Aptio supports GetAltCfg
  EFI_STATUS status = EFI_SUCCESS;
  UINT32 index = 0;

  status = GetNvramVariableList((NVRAM_VARIABLE **)varList); // Performance Improving of variable data load and usage
    if(EFI_ERROR(status)){
        goto DONE;
    }

  // Get the Controls in each Page
  for ( index = 0; index < gPages->PageCount; index++ )
  {
    PAGE_INFO *pageInfo = (PAGE_INFO*)((UINT8 *)gPages + gPages->PageList[index]);
    UINTN control = 0;

    if(pageInfo->PageHandle == NULL)
    {
      continue;
    }
    // Get the Default values for each control and update DefaultStore
    for(control = 0; control < pageInfo->PageControls.ControlCount; control++)
    {
      CONTROL_INFO *controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

      if(controlInfo->ControlDataWidth > 0)
      {
        UINT32 offset = 0;
        UINTN width = 0;
        UINT8 *defaultValue = NULL;

		if( UefiIsEfiVariable(controlInfo->ControlVariable,VarGetVariableInfoIndex( controlInfo->ControlVariable ) ) )
			continue;

        offset = UefiGetQuestionOffset(controlInfo->ControlPtr);
        width = UefiGetWidth(controlInfo->ControlPtr);

        defaultValue = (Mask & AMI_FLAG_MANUFACTURING)? ((UINT8*)controlInfo + sizeof(CONTROL_INFO)) :
            ((UINT8*)controlInfo + sizeof(CONTROL_INFO) + controlInfo->ControlDataWidth);
        _VarGetSetValue( VAR_COMMAND_SET_VALUE, *varList, controlInfo->ControlVariable, offset, width, defaultValue );
      }
    }
  }

DONE:
	return status;
}

/**

    @param 

    @retval 
**/
UINTN HiiMyGetStringLength( /*EFI_HII_HANDLE*/VOID* handle, UINT16 token )
{
	CHAR16 *string;
	UINTN length = 0;

	string = HiiGetString( handle, token );
	if ( string == NULL )
		return length;

	length = TestPrintLength( string ) / (NG_SIZE);
	MemFreePointer( (VOID **)&string );

	return length;
}

/**
    returns Max length of the in the paragraph

    @param Handle and token

    @retval Length

**/

UINTN HiiMyGetMultiLineStringLength( /*EFI_HII_HANDLE*/VOID* handle, UINT16 token )
{
	CHAR16 *string;
	UINTN length = 0,MaxLength=0;
	UINTN i,j;

	string = HiiGetString( handle, token );
	if ( string == NULL )
		return length;

	i=j=0;
	while(string[i])
	{
		// is Newline
		if((string[i]==0x0d)&& (string[i+1]==0x0a))
		{
			string[i]=0x0;
			length = TestPrintLength( &string[j] ) / (NG_SIZE);
			if(length>MaxLength)
				MaxLength = length;
			i+=2;
			j=i;
		}
		else
			i++;
	}
	length = TestPrintLength( &string[j] ) / (NG_SIZE);
	MemFreePointer( (VOID **)&string );

	if(length>MaxLength)
		MaxLength = length;

	return MaxLength;
}

/**
    Waits for a Key Press

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS GetAcknowledgementKey()
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN Index;
    EFI_INPUT_KEY Key;

	//Flush Key Stroke
	while(gST->ConIn->ReadKeyStroke( gST->ConIn, &Key ) == EFI_SUCCESS)
		;

	status = gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &Index );
	if(EFI_ERROR(status))
	{
		goto DONE;
	}

	status = gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );

DONE:
	return status;
}

/**
        Procedure  : HiiChangeString

        Description  : to change a existing string

        Input    : VOID* handle, UINT16 token, CHAR16 *string

        Output   : Status

**/
UINT16 HiiChangeString( /*EFI_HII_HANDLE*/VOID* handle, UINT16 token, CHAR16 *string )
{
/*  return HiiChangeStringLanguage( handle, token, L"   ", string );*/
  UINTN i;
  UINT16 returntoken;
  UINT16 status = token;

  for ( i = 0; i < gLangCount; i++ )
    {
      returntoken = HiiChangeStringLanguage( handle, token, gLanguages[i].Unicode, string );
        if (( token != returntoken ) && ( INVALID_TOKEN != returntoken ))
		{
			//Fix to Use the HiiChangeString() to Add the string for morethen one langs.
			//with the same token.
   	        token = status = returntoken;
		}

    }
  return status;
}

/**
    GetFormSetTitleAndHelp

    @param Handle EFI_STRING_ID *Title, EFI_STRING_ID *Help

    @retval status
**/
EFI_STATUS GetFormSetTitleAndHelp(EFI_HII_HANDLE Handle, EFI_STRING_ID *Title, EFI_STRING_ID *Help)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_IFR_FORM_SET *formSet = NULL;
  UINT32 index = 0;
  UINT8 *pkgHdr = NULL;


  for(index = 0; index < gSetupCount; index++)
  {
    if(gSetupData[index].Handle == Handle)
    {
      pkgHdr = (UINT8*)gSetupData[index].FormSet;
      formSet = (EFI_IFR_FORM_SET*)(pkgHdr + sizeof(EFI_HII_PACKAGE_HEADER));
      *Title = formSet->FormSetTitle;
      *Help = formSet->Help;
      break;
    }
  }

  if(index == gSetupCount)
  {
    status = EFI_INVALID_PARAMETER;
  }

  return status;
}

/*
VOID HiiTestFormBrowser(EFI_HANDLE Handle )
{
		EFI_HII_HANDLE	hHandle[3];
		EFI_FORM_BROWSER2_PROTOCOL * pfb;

		_HiiLocateSetupHandles();
		hHandle[0] = gSetupHandles[1];
		hHandle[1] = gSetupHandles[3];
		hHandle[2] = gSetupHandles[4];

		gBS->HandleProtocol(Handle,&gEfiFormBrowser2ProtocolGuid,&pfb);
		pfb->SendForm(pfb,(EFI_HII_HANDLE *)&hHandle,3,NULL,0,NULL,NULL);
}
*/

/**
    Added a new IFR pack data in gSetupData

    @param Handle Formset handle
    @param PackData New pack data to add

    @retval EFI_STATUS status - EFI_OUT_OF_RESOURCES if memory allocation fails
        EFI_SUCCESS if successful
**/
EFI_STATUS HandleNewIFRPack(EFI_HANDLE Handle, UINT8 *PackData)
{
  EFI_STATUS 	status = EFI_SUCCESS;
  UINTN 		length = 0;
//  UINTN 		oldSetupCount = 0, size = 0; size unused
  UINTN 		oldSetupCount = 0;
  UINT8 		*formSet = PackData;
  UINT8 		*tformSet = PackData;
  UINT32 		OldVariableCount = gVariables->VariableCount;

  SETUP_DEBUG_UEFI_NOTIFICATION ( "\n\n[TSE] Entering HandleNewIFRPack()\n\n" );     

  if(gSetupCount == 0)
  {
    goto DONE;
  }

  // Check if Handle contains an IFR formset
  //formSet = (UINT8*)HiiGetForm(Handle, 0, &length);
  if(NULL == formSet)
  {
    formSet = (UINT8*)HiiGetForm(Handle, 0, &length);
	if (NULL == formSet)
	{
	    goto DONE;
 	}	
  }
  	// basic validation of a new formset no point to continue if it's bad
  	status = ValidateParseForm(formSet);
    if (EFI_SUCCESS != status)
  	{
    	MemFreePointer((void**)&formSet);
    	goto DONE;
  	}
	tformSet = (UINT8*)HiiGetForm(Handle, 0, &length);  
	//If the current handle has valid formset then check for valid formsetClassGUID or not
	if (NULL != tformSet)
	{
		EFI_IFR_FORM_SET *formset;
		CHAR16 *tFormsetTitle = NULL;

		formset = ((EFI_IFR_FORM_SET*)((UINT8*)(tformSet)+sizeof(EFI_HII_PACKAGE_HEADER)));
		if (!ShowClassGuidFormsets( (TSE_EFI_IFR_FORM_SET*)formset))
		{
			MemFreePointer((void**)&tformSet);
			//Don't add this formset and don't increment gDynamicPageCount bcz formset with different ClassGUID will be suppressed
			goto DONE;
		}
		MemFreePointer((void**)&tformSet);
	}
	else
	{
		//If the current handle has no formset info then skip parsing
		goto DONE;
	}
	
	oldSetupCount = gSetupCount++;

  // Reallocate gSetupHandles
  gSetupHandles = MemReallocateZeroPool(gSetupHandles, oldSetupCount * sizeof (EFI_HII_HANDLE), gSetupCount * sizeof (EFI_HII_HANDLE));
  if(gSetupHandles == NULL)
  {
    status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }

  gSetupHandles[oldSetupCount] = Handle;

  // Reallocate and set gSetupData
  gSetupData = MemReallocateZeroPool(gSetupData, oldSetupCount * sizeof(SETUP_LINK), gSetupCount * sizeof(SETUP_LINK));
  if(gSetupData == NULL)
  {
    status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }
  MemCopy(&gSetupData[gSetupCount -1], &gSetupData[oldSetupCount -1], sizeof(SETUP_LINK));
  gSetupData[gSetupCount -1].FormSet = formSet;
  gSetupData[gSetupCount -1].Handle = Handle;
  _GetClassSubClassID(&gSetupData[gSetupCount -1]);
  status = _FxiupNotificationData(&gSetupData[gSetupCount -1]);
   UIUpdateCallbackHook((VOID*)Handle, OldVariableCount); //  Modified UIUpdateCallback as module hook


   AddNewHandleIntoList(Handle);
DONE:
    SETUP_DEBUG_UEFI_NOTIFICATION ( "\n\n[TSE] Exiting HandleNewIFRPack:   status = 0x%x \n\n" , status );    
  return status;
}

/**
    Removes a IFR pack data from gSetupData

    @param Handle Formset handle

    @retval EFI_STATUS status - EFI_OUT_OF_RESOURCES if memory allocation fails
        EFI_SUCCESS if successful
**/
VOID StopRefreshOpEvent (CONTROL_INFO *controlInfo);
EFI_STATUS HandleRemoveIFRPack(EFI_HANDLE Handle)
{
  EFI_STATUS status = EFI_SUCCESS;
//  UINTN ii = 0, size = 0; unused size
  UINTN ii = 0;
  UINTN count = gDynamicPageCount;
  EFI_GUID 	DynamicPageGuid = DYNAMIC_PAGE_COUNT_GUID;
  EFI_GUID 	RemovePackGuid;
  BOOLEAN 	RemovePackGuidFilled = FALSE;
  UINTN 		jIndex = 0;
  PAGE_ID_INFO *pageIdInfo = (PAGE_ID_INFO*)NULL;
  BOOLEAN IsDriverHealthFormset = 0;
  EFI_GUID  DriverHealthHiiGuid = EFI_HII_DRIVER_HEALTH_FORMSET_GUID;
  EFI_IFR_FORM_SET *formSet;
  UINT16 fsClass = 0xFFFF ;
  BOOLEAN IsRootPageHidden = TRUE;
    
  SETUP_DEBUG_UEFI_NOTIFICATION ( "\n\n[TSE] Entering HandleRemoveIFRPack()\n\n" );     

  if(gSetupCount == 0)
  {
    goto DONE;
  }

  for(ii = 0; ii < gPages->PageCount; ii++)
  {
    UINTN control = 0;
    PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[ii]);

    if(pageInfo->PageHandle != Handle)
    {
      continue;
    }
    if (FALSE == RemovePackGuidFilled)			//get the current formset guid
    {
		 pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[pageInfo->PageIdIndex]);
		 if(pageIdInfo)
		 {
			 MemCopy (&RemovePackGuid, &pageIdInfo->PageGuid, sizeof (EFI_GUID));
		 }
		 RemovePackGuidFilled = TRUE;
	 }
    for(control= 0; (control < pageInfo->PageControls.ControlCount); control++)
    {
      CONTROL_INFO *controlInfo = NULL;

      controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);
      if (controlInfo->ControlFlags.RefreshID)
      {
	      StopRefreshOpEvent (controlInfo);
      }
      controlInfo->ControlHandle = (VOID *)NULL;
    }
    if(pageInfo->PageFlags.PageRefreshID)
    {  
    	StopRefreshOpEventforPage (pageInfo);
    }
    if(pageInfo->PageFlags.PageDynamic)
    	IsRootPageHidden = FALSE;
    
    PageRemoveHook (ii, (VOID *)pageInfo, pageInfo->PageHandle, pageInfo->PageFormID);
    pageInfo->PageHandle = (VOID *)(UINTN)0xFFFF;
  }

  if (RemovePackGuidFilled)
  {
	for (jIndex = 0; jIndex < gGuidDumpCount; jIndex ++)				//consider it as dynamic page only when
	{																					//formset is not present offline setupdata data
		if (EfiCompareGuid (&gGuidDump [jIndex], &RemovePackGuid))
		{
			break;
		}
	}
  }
  else
  {
	  jIndex = gGuidDumpCount;
  }

	//If Handle present in gSetupData, then free allocated memory of Formset.
	for(ii = 0; ii < gSetupCount; ii++)
	{
		if(gSetupData[ii].Handle != Handle)
		{
			continue;
		}
		formSet = (EFI_IFR_FORM_SET*)((UINT8 *)gSetupData[ii].FormSet + sizeof(EFI_HII_PACKAGE_HEADER));
		if(EfiCompareGuid (&(formSet->Guid), &DriverHealthHiiGuid))
			IsDriverHealthFormset = 1;
		else
			IsDriverHealthFormset = 0;

		fsClass = gSetupData[ii].ClassID ;
		
		gSetupData[ii].ClassID = 0;
		gSetupData[ii].FormSetLength = 0;
		gSetupData[ii].Added = 0;
		gSetupData[ii].SubClassID = 0;
		MemFreePointer((VOID**)&gSetupData[ii].FormSet);
		
		if(!IsDriverHealthFormset && !IsRootPageHidden)
		{
			if(jIndex == gGuidDumpCount) //Decrement the gDynamicPageCount if current handle/formset is present in gSetupData else dont decrement
			{
				DecrementDynamicPageCount(fsClass);
			}
		}
		break;
	}
DONE:
    SETUP_DEBUG_UEFI_NOTIFICATION ( "\n\n[TSE] Exiting HandleRemoveIFRPack:   status = 0x%x \n\n" , status );  
  return status;
}

/**
    Adds a IFR pack data from gSetupData

    @param Handle Formset handle
    @param PackData Update pack data to add

    @retval EFI_STATUS status - EFI_OUT_OF_RESOURCES if memory allocation fails
        EFI_SUCCESS if successful
**/
EFI_STATUS HandleAddIFRPack(EFI_HANDLE Handle, UINT8 *PackData)
{
  EFI_STATUS status = EFI_SUCCESS;
  UINTN 	length = 0;
	UINT8 		*tformSet = NULL;
//  UINTN 	ii = 0, size = 0 ;
  UINTN 	ii = 0 ;
  UINT32 	OldVariableCount = gVariables->VariableCount;
  UINT8 	*formSetData = PackData;   


  SETUP_DEBUG_UEFI_NOTIFICATION ( "\n\n[TSE] Entering HandleAddIFRPack()\n" );     

  if(gSetupCount == 0)
  {
    goto DONE;
  }

	if(NULL == formSetData)
	{
		formSetData = (UINT8*)HiiGetForm(Handle, 0, &length);
		if(NULL == formSetData)
		{
			goto DONE;
		}
	}

	//do sanity check on the upcoming form
	status = ValidateParseForm(formSetData);
	if (EFI_SUCCESS != status)
	{
    	MemFreePointer((void**)&formSetData);
		goto DONE;
	}	
	
	tformSet = (UINT8*)HiiGetForm(Handle, 0, &length);
	if (NULL != tformSet)
	{
		UINT32 	ii = 0;
		EFI_IFR_FORM_SET 	*formset;
		CHAR16 	*tFormsetTitle = NULL;

		formset = ((EFI_IFR_FORM_SET*)((UINT8*)(tformSet)+sizeof(EFI_HII_PACKAGE_HEADER)));
		if ( !ShowClassGuidFormsets( ((TSE_EFI_IFR_FORM_SET*)formset) ) )
		{
			MemFreePointer((void**)&tformSet);
			//Don't change gDynamicPageCount and don't add this formset bcz formset with different ClassGUID will be suppress during parsing
			goto DONE;
		}

		MemFreePointer((void**)&tformSet);
	}
	else
	{
		//Don't change gDynamicPageCount bcz passing Handle has no valid formset
		goto DONE;
	}

	//Find matching handle from gSetupData
	for(ii = 0; ii < gSetupCount; ii++)
	{
		if(gSetupHandles[ii] == Handle)
		{
			break;
		}
	}


	// If no handle matches in gSetupData then treat as new IFR to parse.
	if( ii == gSetupCount )
	{
		status = HandleNewIFRPack(Handle, formSetData);
		goto DONE;
	}

  //Handle is present already in gSetupData so update FormSet info for the same handle
	if (NULL != formSetData)
		gSetupData[ii].FormSet = formSetData;
	
	_GetClassSubClassID(&gSetupData[ii]);
	status = _FxiupNotificationData(&gSetupData[ii]);
	UIUpdateCallbackHook((VOID*)Handle, OldVariableCount); // Modified UIUpdateCallback as module hook
	
DONE:
  SETUP_DEBUG_UEFI_NOTIFICATION ( "\n[TSE] Exiting HandleAddIFRPack:   status = 0x%x \n\n" , status );      
  return status;
}


/**

    @param 

    @retval 
**/

VOID *GetFormByFormID(EFI_HII_PACKAGE_HEADER *IFRData, UINT16 FormID, UINTN *Length)
{
  EFI_IFR_OP_HEADER *opHeader = NULL;
  UINTN formLength = 0;
  UINTN offset = 0;
  UINT8 *form = NULL;
  UINT8 *formBegin = NULL;
  BOOLEAN flag = TRUE;

  offset = sizeof(EFI_HII_PACKAGE_HEADER);

  ResetScopeStack();
	do
	{
    EFI_IFR_FORM *formOp = NULL;
    AMI_EFI_IFR_FORM_MAP *formMapOp = NULL;
		UINT8 scope = 0;

    opHeader = (EFI_IFR_OP_HEADER*)((UINT8*)IFRData + offset);
		switch(opHeader->OpCode)
		{
		case EFI_IFR_FORM_OP:
			formOp = (EFI_IFR_FORM*)opHeader;
			if(formOp->FormId == FormID)
			{
				formBegin = (UINT8*)opHeader;
			}
			break;
        case EFI_IFR_FORM_MAP_OP: 
			formMapOp = (AMI_EFI_IFR_FORM_MAP *)opHeader;
			if(formMapOp->FormId == FormID)
			{
				formBegin = (UINT8*)opHeader;
			}
			break;
		case EFI_IFR_END_OP:
      PopScope(&scope);
			switch(scope)
			{
			case EFI_IFR_FORM_SET_OP:
        flag = FALSE;
				break;
			case EFI_IFR_FORM_OP:
            case EFI_IFR_FORM_MAP_OP: 
				if(formBegin)
				{
					flag = FALSE;
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		if(opHeader->Scope)
		{
			PushScope(opHeader->OpCode);
		}
		if(formBegin)
		{
			formLength += opHeader->Length;
		}
		offset += opHeader->Length;
	}while(flag);

	if(formLength)
	{
		form = EfiLibAllocateZeroPool(formLength);
		if (form == NULL)
		{
      goto DONE;
		}
		MemCopy(form, formBegin, formLength);
		*Length = formLength;
	}

DONE:
  ResetScopeStack();
  return form;
}

/**

    @param 

    @retval 
**/
VOID *GetNextForm(EFI_HII_PACKAGE_HEADER *Package, UINT8 **IFRDataPtr, UINTN *Length)
{
  UINT8 *form = NULL;
  UINT8 *formBegin = NULL;
  UINTN offset = 0;
  UINTN formLen = 0;
  UINTN formScope = 0;
  BOOLEAN endForm = FALSE;

  static UINTN scopeCount = 0;

  if(Package == NULL)
  {
    goto DONE;
  }
  if(*IFRDataPtr == NULL)
  {
    *IFRDataPtr = (UINT8 *)Package + sizeof(EFI_HII_PACKAGE_HEADER);
    scopeCount = 0;
  }

  do
  {
    EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)(*IFRDataPtr + offset);
    switch(opHeader->OpCode)
    {
    case EFI_IFR_FORM_OP:
    case EFI_IFR_FORM_MAP_OP:
      formBegin = (UINT8 *)opHeader;
      break;
    case EFI_IFR_END_OP:
      scopeCount -= scopeCount? 1 : 0;
      formScope -= formScope? 1 : 0;
      endForm = formScope? FALSE : TRUE;
      break;
    default:
      break;
    }
    if(opHeader->Scope)
    {
      scopeCount++;
      formScope += formBegin? 1 : 0;
    }
    formLen += formBegin? opHeader->Length : 0;
    offset+=opHeader->Length;
  }while(endForm == FALSE);

  if(formBegin)
  {
    form = EfiLibAllocateZeroPool(formLen);
    if(form == NULL)
    {
      *Length = 0;
      goto DONE;
    }
    MemCopy(form, formBegin, formLen);
    *Length = formLen;
    formBegin += formLen;
    *IFRDataPtr += offset;
  }

DONE:
  return form;
}

/**

    @param 

    @retval 
**/
EFI_STATUS FixSetupData()
{
  EFI_STATUS status = EFI_SUCCESS;
  UINTN ii = 0;

  SETUP_DEBUG_UEFI ( "\n[TSE] Entering FixSetupData()\n" );    

  if(gIFRChangeNotify)
  {
    if(gRemovedHandle!=NULL)
    {
      UIUpdateCallbackHook((VOID*)gRemovedHandle, gVariables->VariableCount); // Modified UIUpdateCallback as module hook
      gRemovedHandle = NULL;
      gIFRChangeNotify = FALSE;
    }
  }
//Minisetup crashes after controller reconnection during Hii callback
  if((gApplicationData == NULL) || (gSetupCount == 0))
  {
    status = EFI_UNSUPPORTED;
    goto DONE;
  }

  for(ii = 0; ii < gPages->PageCount; ii++)
  {
    PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[ii]);

    if(pageInfo->PageHandle != (VOID *)(UINTN)0xFFFF)
    {
      continue;
    }
    pageInfo->PageHandle = NULL;
  }

  SETUP_DEBUG_UEFI ( "\n[TSE] Exiting FixSetupData(),  status = 0x%x \n" , status );

DONE:  
  return status;
}

/**

    @param 

    @retval 
**/
EFI_STATUS _FxiupNotificationData(SETUP_LINK *Setup_Link)
{
	EFI_STATUS Status = EFI_SUCCESS;
  PAGE_ID_LIST  *tempPageIdList = gPageIdList;
  PAGE_LIST     *tempPages = gPages;
  VARIABLE_LIST *tempVariables = gVariables;

  SETUP_DEBUG_UEFI ( "\n[TSE] Entering _FxiupNotificationData()\n" );
  //ReInitialize Setupdata Pointers
  _ReInitializeSetupdataPointers();
  //Parse Form
  Status = ParseForm(Setup_Link);
  if(Status == EFI_SUCCESS)
      IncrementDynamicPageCount(Setup_Link->ClassID);
  // Merge ListPtr and InfoPtr
	_MergePageIdListAndInfo();
	_MergeVariableListAndInfo();
	_MergePageListAndInfo();
  if(Status == EFI_SUCCESS)
	FixHiddenPageList();
	FixMergePagesExtraHook ((VOID *)gPages, (VOID *)tempPages );
	
  // Free memory used by temp Ptrs
  MemFreePointer((VOID**)&tempPageIdList);
  MemFreePointer((VOID**)&tempPages);
  MemFreePointer((VOID**)&tempVariables);
  
  SETUP_DEBUG_UEFI ( "\n[TSE] Exiting _FxiupNotificationData()\n" );
  return Status;
}

/**

    @param 

    @retval 
**/
EFI_STATUS _ReInitializeSetupdataPointers()
{
  EFI_STATUS status = EFI_SUCCESS;
  UINT32 i = 0;

  // Reinitialize PageIdListPtr and PageIdInfoPtr
  PageIdListPtr = (PAGE_ID_LIST *)EfiLibAllocateZeroPool(PageIdListSize);
	if(PageIdListPtr == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
  MemCopy(PageIdListPtr, gPageIdList, PageIdListOffset);

	//Fix Offsets
	for(i=0; i < PageIdListPtr->PageIdCount; i++)
	{
		PageIdListPtr->PageIdList[i] -= PageIdListOffset;
	}

  PageIdInfoPtr = (PAGE_ID_INFO *)EfiLibAllocateZeroPool(PageIdInfoSize);
	if(PageIdInfoPtr == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
  MemCopy(PageIdInfoPtr, (UINT8 *)gPageIdList + PageIdListOffset, PageIdInfoOffset);

  // Reinitialize PageListPtr, FirstPage and PageInfoPtr
  PageListPtr = (PAGE_LIST *)EfiLibAllocateZeroPool(PageListSize);
	if(PageIdListPtr == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
  MemCopy(PageListPtr, gPages, PageListOffset);
	//Fix Offsets
	for(i = 0; i < PageListPtr->PageCount; i++)
	{
    PageListPtr->PageList[i] -= (i == 0)? PageListOffset : (FirstPageOffset + PageListOffset);
	}

  FirstPage = (PAGE_INFO *)EfiLibAllocateZeroPool(AllocatedFirstPageSize);
	if(FirstPage == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
  MemCopy(FirstPage, (UINT8 *)gPages + PageListOffset, FirstPageOffset);

  PageInfoPtr = (PAGE_INFO *)EfiLibAllocateZeroPool(PageInfoSize);
	if(PageInfoPtr == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
  MemCopy(PageInfoPtr, (UINT8 *)gPages + PageListOffset + FirstPageOffset, PageInfoOffset);

  // Reinitialize VariableInfoPtr and VariableListPtr
  VariableListPtr = (VARIABLE_LIST *)EfiLibAllocateZeroPool(VariableListSize);
	if(VariableListPtr == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
  MemCopy(VariableListPtr, gVariables, VariableListOffset);

	//Fix Offsets
	for(i=0; i < VariableListPtr->VariableCount; i++)
	{
		VariableListPtr->VariableList[i] -= VariableListOffset;
	}

  VariableInfoPtr = (VARIABLE_INFO *)EfiLibAllocateZeroPool(VariableInfoSize);
	if(VariableInfoPtr == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}
  MemCopy(VariableInfoPtr, (UINT8 *)gVariables + VariableListOffset, VariableInfoOffset);

DONE:
  return status;
}

/**
    Wrapper function to include RTIfrUpdateVariableInfo
    based on SDL token.

    @param ControlVariable

    @retval VOID

**/
VOID RTIfrUpdateVariableInfo( UINT32 ControlVariable, EFI_HANDLE Handle )
{
	VARIABLE_INFO  *varInfo = NULL;
//    UINT16		*varString;
//    UINT16        StringToken = 0;


   if (ControlVariable < VariableListPtr->VariableCount)
   {
      varInfo = (VARIABLE_INFO*)((UINT8*)VariableInfoPtr + VariableListPtr->VariableList[ControlVariable]);
      if(varInfo->VariableHandle == NULL)
      {
   	   varInfo->VariableHandle = Handle;
      }
      // Commented for suppressing build time parsing. Name value not supported by current vfr compiler. Uncomment it once vfr compiler supports it.
      /*if (VARIABLE_ATTRIBUTE_NAMEVALUE == (varInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
      {
         if (0 == (EfiStrLen (varInfo->VariableName)))
         {
            StringToken = (UINT16)((UINT16)(varInfo->NameValueToken [1] << 8) + (UINT8)(varInfo->NameValueToken [0]));
            varString = HiiGetString (varInfo->VariableHandle, StringToken);
            EfiStrCpy (varInfo->VariableName, varString);
         }
      }*/
   }else
   {
      // ERROR. This case should not Occur.If it Does then DOOM!!!
   }
}

/**
    Function to Encode the password key

    @param PwKey 

    @retval VOID

**/
VOID HiiGetEfiKey(CHAR16 *PwKey)
{
// Need to fix this for UEFI 2.1
/*
  EFI_HII_KEYBOARD_LAYOUT *Key
    EFI_KEY_DESCRIPTOR *pEfiKD = NULL;	//, *pRetEfiKD = NULL;
    UINT16  Count = 0,i;
    EFI_STATUS Status;

    if(EFI_SUCCESS != HiiInitializeProtocol())
        return;

    //Get Count (No Of Key board Desc)
    Status= gHiiDatabase->GetKeyboardLayout(gHiiDatabase, NULL, &Count, pEfiKD);

    if(EFI_BUFFER_TOO_SMALL != Status)
        return;

    if(Count)
    {
        //Allocate space for Count number of EFI_KEY_DESCRIPTOR
        pEfiKD = EfiLibAllocateZeroPool(sizeof(EFI_KEY_DESCRIPTOR)*Count);

        Status= gHiiDatabase->GetKeyboardLayout(gHiiDatabase, &Count, pEfiKD);

        if(EFI_SUCCESS == Status)
        {
            for(i=0;i<Count;i++)
            {
                if((pEfiKD[i].Unicode == *PwKey) ||
                    (pEfiKD[i].ShiftedUnicode == *PwKey) ||
                    (pEfiKD[i].AltGrUnicode == *PwKey) ||
                    (pEfiKD[i].ShiftedAltGrUnicode == *PwKey)
                    )
                {
//                  pRetEfiKD = EfiLibAllocateZeroPool(sizeof(EFI_KEY_DESCRIPTOR));
//                  MemCopy(pRetEfiKD, pEfiKD+i,sizeof(EFI_KEY_DESCRIPTOR));
                    *PwKey = (CHAR16)(pEfiKD+i)->Key;
                    break;
                }
            }
        }
        MemFreePointer((VOID **)&pEfiKD);
    }
*/
}
/**
    Function to update defaults depending on the Default Id on Selecting the Reset Button

    @param ControlData UINT16 DefaultId

    @retval EFI_STATUS

**/
EFI_STATUS UefiupdateResetButtonDefault(CONTROL_INFO ControlData, UINT16 DefaultId)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT32 control, offset = 0;
	UINTN index=0, size = 0, defaultoffset = 0 /* for defaultId + default value + padding*/,defaultvalueoffset = 0 /* for deafultId + padding*/ ;
	UINT8 *Defaultaddress = 0;

   //Getting the current page info	
	PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[ControlData.ControlPageID]);
	
	if ((NULL == pageInfo) || (0 == pageInfo->PageHandle))
			return EFI_UNSUPPORTED;

	for ( control= 0; control < pageInfo->PageControls.ControlCount; control++ )
	{
		CONTROL_INFO *controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);
		index=0;
		defaultoffset=0;
		
		//checking for the Default count of the control
		while(index  <  controlInfo->DefaultStoreCount)
		{
			Defaultaddress = (UINT8 *)controlInfo + sizeof(CONTROL_INFO)+(controlInfo->ControlDataWidth * 2) + defaultoffset;

			if(controlInfo->ControlDataWidth == 1) // 1 byte padding
			{
				defaultoffset += sizeof(UINT16) + controlInfo->ControlDataWidth * 2;
				defaultvalueoffset = sizeof(UINT16);
			}
			else // controlInfo->ControlDataWidth - sizeof(UINT16) byte padding
			{
				defaultoffset += controlInfo->ControlDataWidth * 2;
				defaultvalueoffset = controlInfo->ControlDataWidth;
			}
				//checking for the default Id of the Resetbutton
				if(DefaultId == (UINT16)*Defaultaddress)
				{
						offset = UefiGetQuestionOffset(controlInfo->ControlPtr);
						size = UefiGetWidth(controlInfo->ControlPtr);
						status = VarSetValue(controlInfo->ControlVariable, offset, size,Defaultaddress + defaultvalueoffset );
				}				
				index++;
		}
		//Checking for the ControlEvaluateDefault Flag and the Default Id equal tp Manufacturing Id 
		 if(controlInfo->ControlFlags.ControlEvaluateDefault && IsDefaultConditionalExpression())
		 {
			 UINT64 Defaults = 0 ;
			 UINT16 size = 0;
			 offset = UefiGetQuestionOffset(controlInfo->ControlPtr);
			 
			 //Evaluating the Control Default
				size = EvaluateControlDefault(controlInfo,&Defaults);
				status = VarSetValue(controlInfo->ControlVariable, offset, size, &Defaults );
				// Setiing the Evaluated value to the gOptimalDefaults
				if(DefaultId == EFI_HII_DEFAULT_CLASS_MANUFACTURING)
				_VarGetSetValue( VAR_COMMAND_SET_VALUE, gOptimalDefaults, controlInfo->ControlVariable, offset, size, &Defaults );
		 }
	}

	return status;

}
/**
    Get the device path for the the given handle

    @param HiiHandle Input handle
    @param DevicePath Corresponding device path
        if successful.

    @retval EFI_STATUS Status - EFI_SUCCESS, if successful, else EFI_ERROR
**/
EFI_STATUS GetDevPathFromHandle(EFI_HII_HANDLE HiiHandle, EFI_DEVICE_PATH_PROTOCOL **DevicePath)
{
  EFI_STATUS Status = EFI_INVALID_PARAMETER;
  EFI_HANDLE DriverHandle = (EFI_HANDLE)NULL;
  EFI_DEVICE_PATH_PROTOCOL *TempDevPath = (EFI_DEVICE_PATH_PROTOCOL *)NULL;

  *DevicePath = TempDevPath;
  
  if(HiiHandle == NULL || HiiHandle == (VOID*)0xffff)
	  return EFI_INVALID_PARAMETER;

  Status = gHiiDatabase->GetPackageListHandle (gHiiDatabase, HiiHandle, &DriverHandle);
  if(EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (DriverHandle, &gEfiDevicePathProtocolGuid, (VOID**)&TempDevPath);
  if(Status == EFI_SUCCESS){
    *DevicePath = TempDevPath;
  }

  return Status;
}

/**
    Gets the string presentation of the given DevicePath

    @param DevPath Given device path
    @param DevPathStr Return pointer to the string presentation

    @retval EFI_STATUS status - EFI_SUCCESS, if successful, else EFI_ERROR
**/
EFI_STATUS GetStringFromDevPath(EFI_DEVICE_PATH_PROTOCOL *DevPath, CHAR16 **DevPathStr)
{
  EFI_STATUS status = EFI_ABORTED;
  
  if(gDevPathToTextProtocol == NULL)
  {
    status = gBS->LocateProtocol(&gDevPathToTextGuid, NULL, (VOID**)&gDevPathToTextProtocol);
  }
  if(gDevPathToTextProtocol != NULL)
  {
    *DevPathStr = gDevPathToTextProtocol->ConvertDevicePathToText(DevPath, FALSE, FALSE);
    status = EFI_SUCCESS;
  }
  return status;
}


/**
    Updates the parent pageID for the EFI_IFR_REFX
    control

    @param CtrlInfo Control Info in the library
    @param PageInfo Page Info in the library

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS FixIFRRefXControl(PAGE_INFO *PageInfo, CONTROL_INFO * CtrlInfo)
{
  EFI_STATUS status = EFI_NOT_FOUND;
  UINT32 		i=0;
  PAGE_INFO 	*pageInfo;
  
  for (i = 0; i < gPages->PageCount; i++)
  {
	pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[i]);
	if (pageInfo->PageID == CtrlInfo->ControlDestPageID)
	{
		pageInfo->PageParentID = PageInfo->PageID ; //CtrlInfo->ControlPageID;
		status = EFI_SUCCESS;
		break;
	}
  }
    
  return status;
}
/**
    Updates the reference link information in for the EFI_IFR_REFX
    controls

    @param CtrlInfo Control Info in the library
    @param PgHasRefXCtrl Return TRUE if REF4 control found

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS UpdateIFRRefXControls(PAGE_INFO *PageInfo, CONTROL_INFO * CtrlInfo)
{
    EFI_STATUS status = EFI_SUCCESS;
    EFI_DEVICE_PATH_PROTOCOL *tempDevPath = (EFI_DEVICE_PATH_PROTOCOL *)NULL;
    BOOLEAN IsQuestionIdValid = FALSE;
    BOOLEAN IsFormSetIdValid = FALSE;
    BOOLEAN IsDevicePathFound = FALSE;
    EFI_GUID formSetGuid = {0}, zeroFormSetGuid = {0};
    UINT16 ControlDestPageID = 0xFFFF;
    UINT16 ControlDestQuestionID = 0xFFFF;
    UINTN count = 0;
    UINTN control = 0;
    PAGE_INFO *pageInfo = (PAGE_INFO*)NULL;
    PAGE_ID_INFO *pageIdInfo = (PAGE_ID_INFO*)NULL;
  
    CHAR16 *devPathStr = (CHAR16 *)NULL, *tempDevPathStr = (CHAR16 *)NULL;

    if((CtrlInfo == NULL) || (CtrlInfo->ControlPtr == NULL))
    {
        status = EFI_INVALID_PARAMETER;
        goto DONE;
    }

    if(CtrlInfo->ControlHandle == NULL)
    {
        status = EFI_INVALID_PARAMETER;
        goto DONE;
    }   

    if(((EFI_IFR_OP_HEADER*)CtrlInfo->ControlPtr)->OpCode != EFI_IFR_REF_OP)
    {
        status = EFI_INVALID_PARAMETER;
        goto DONE;
    }

    if(((EFI_IFR_OP_HEADER *)CtrlInfo->ControlPtr)->Length == sizeof(AMI_EFI_IFR_REF5)) //If AMI_EFI_IFR_REF5
    {
        if(CtrlInfo->ControlDataLength == sizeof(AMI_EFI_HII_REF)) //if default value is of size AMI_EFI_HII_REF, retrieve its value
        {        
            AMI_EFI_HII_REF *Ref = NULL ;
            Ref = (AMI_EFI_HII_REF*)((UINT8*)CtrlInfo + sizeof(CONTROL_INFO) + CtrlInfo->ControlDataLength); //take the default value
            if(Ref) 
            {
                //fetch the destination questionId
                ControlDestQuestionID = Ref->QuestionId; //Set the destination question ID to set focus
                if(Ref->DevicePath) //if device path is provided
                {
                    devPathStr = HiiGetString(CtrlInfo->ControlHandle, Ref->DevicePath); //get the text representation of device path
                    if(devPathStr == NULL) //If the device path string is invalid, remain the same
                    {
                        CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page
                        CtrlInfo->DestQuestionID = CtrlInfo->QuestionId; //set the current control's question Id as the destination question Id.
                        goto DONE;
                    } 
                    else 
                    { //If the device path string is not NULL
                        CHAR16 *zeroDevPathStr = (CHAR16 *)EfiLibAllocateZeroPool(MAX_DEV_PATH_STR_LEN * sizeof(CHAR16));
                        if(zeroDevPathStr)
                        {
                            //MemSet(&zeroDevPathStr[0], MAX_DEV_PATH_STR_LEN * sizeof(CHAR16), L'0');
                            MemSet(&zeroDevPathStr[0], MAX_DEV_PATH_STR_LEN * sizeof(CHAR16), 0);
                            if(EfiStrCmp(devPathStr, zeroDevPathStr) == 0) //If the DevicePath string contains all zeros, remain the same page.
                            {
                                CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page
                                CtrlInfo->DestQuestionID = CtrlInfo->QuestionId; //set the current control's question Id as the destination question Id.
                                goto DONE;
                            }
                        }
                    }
                    //Device Path is not zero. check whether the provided device path is found.
                    for(count = 0; count < gPages->PageCount; count++)
                    {
                        PAGE_INFO *tempPageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[count]);
                        if(tempPageInfo == NULL)
                        {
                            continue;
                        }
                        status = GetDevPathFromHandle(tempPageInfo->PageHandle, &tempDevPath);
                        if((status == EFI_SUCCESS) && (tempDevPath != NULL))
                        {
                            GetStringFromDevPath(tempDevPath, &tempDevPathStr);
                            if(EfiStrCmp(devPathStr, tempDevPathStr) == 0) //if the provided device path is found
                            {
                                CtrlInfo->ControlDestPageID = tempPageInfo->PageID; //set that page ID
                                ControlDestPageID = tempPageInfo->PageID;
                                IsDevicePathFound = TRUE;
                                break;
                            }
                        }
                    }
                    if(!IsDevicePathFound) //if provided device path is not found, remain the same
                    {
                        CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page;
                        CtrlInfo->DestQuestionID = CtrlInfo->QuestionId; //set the current control's page ID itself as destination page Id.
                        goto DONE;
                    }
                }
                else //if device path is not provided
                {
                    //get formset GUID
                    MemCopy(&formSetGuid, &(Ref->FormSetGuid), sizeof (EFI_GUID));
		            if(EfiCompareGuid(&formSetGuid, &zeroFormSetGuid) == FALSE) //If it is not all zeroes
		            {
			            GetPageIdFromGuid(&formSetGuid, Ref->FormId, &ControlDestPageID);
			            if(ControlDestPageID != 0xFFFF) //If the destination page ID is valid
			            {
			                CtrlInfo->ControlDestPageID = ControlDestPageID; //Assign it to CONTROL_INFO->ControlDestPageID
			                IsFormSetIdValid = TRUE;
			            }
		            }
                    if(!IsFormSetIdValid) //if formset Id is invalid, remain the same
                    {
                        CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page;
                        CtrlInfo->DestQuestionID = CtrlInfo->QuestionId; //set the current control's page ID itself as destination page Id.
                        goto DONE;
                    }
                }
                //at this point either through device path or formset ID the destination page ID will be obtained.
                //check whether the provided Dest. Question ID exist in that destination page
                pageInfo = (PAGE_INFO*)((UINT8 *)gPages + gPages->PageList[ControlDestPageID]);                
                if(pageInfo->PageHandle == NULL || pageInfo->PageHandle == (VOID*)0xffff)
                {
                    CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page;
                    CtrlInfo->DestQuestionID = CtrlInfo->QuestionId; //set the current control's page ID itself as destination page Id.
                    goto DONE;
                }
                for(control = 0; control < pageInfo->PageControls.ControlCount; control++) //check if the destination Question ID is exists in any of
                { //the availble controls of the page.
                    CONTROL_INFO *controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);
                    if(ControlDestQuestionID == controlInfo->ControlKey)
                    {
                        IsQuestionIdValid = TRUE;
                        CtrlInfo->ControlDestPageID = ControlDestPageID; //Assign it to CONTROL_INFO->ControlDestPageID
                        CtrlInfo->DestQuestionID = ControlDestQuestionID;
                        break;
                    }                    
                }
                if(!IsQuestionIdValid) //if the provided Question ID is invalid, remain the same
                {
                    CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page;
                    CtrlInfo->DestQuestionID = CtrlInfo->QuestionId; //set the current control's page ID itself as destination page Id.
                }
            }
        }
    }

    else if(((EFI_IFR_OP_HEADER *)CtrlInfo->ControlPtr)->Length >= sizeof(EFI_IFR_REF2)) //If EFI_IFR_REFX
    {
	    CtrlInfo->DestQuestionID = ((EFI_IFR_REF2 *)CtrlInfo->ControlPtr)->QuestionId; //Set the destination question ID to set focus

	    if(((EFI_IFR_OP_HEADER *)CtrlInfo->ControlPtr)->Length >= sizeof(EFI_IFR_REF3)) //If EFI_IFR_REF3/EFI_IFR_REF4
	    {

		    MemCopy(&formSetGuid, &((EFI_IFR_REF3 *)CtrlInfo->ControlPtr)->FormSetId, sizeof (EFI_GUID));

		    if(EfiCompareGuid(&formSetGuid, &zeroFormSetGuid) == FALSE) //If it is not all zeroes
		    {
			    GetPageIdFromGuid(&formSetGuid, ((EFI_IFR_REF3 *)CtrlInfo->ControlPtr)->FormId, &ControlDestPageID);
			    if(ControlDestPageID != 0xFFFF) //If the destination page ID is valid
			    {
			     CtrlInfo->ControlDestPageID = ControlDestPageID; //Assign it to CONTROL_INFO->ControlDestPageID
			     IsFormSetIdValid = TRUE;
			     FixIFRRefXControl(PageInfo, CtrlInfo);
			    }
		    }

		    if(((EFI_IFR_OP_HEADER *)CtrlInfo->ControlPtr)->Length >= sizeof(EFI_IFR_REF4)) //If EFI_IFR_REF4
		    {
    			

				devPathStr = HiiGetString(CtrlInfo->ControlHandle, ((EFI_IFR_REF4 *)CtrlInfo->ControlPtr)->DevicePath);
				if(devPathStr == NULL) //If the device path string is invalid
				{
					CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page
					goto DONE;
				} 
				else 
				{ //If the device path string is valid
					CHAR16 *zeroDevPathStr = (CHAR16 *)EfiLibAllocateZeroPool(MAX_DEV_PATH_STR_LEN * sizeof(CHAR16));
					if(zeroDevPathStr)
					{
						MemSet(&zeroDevPathStr[0], MAX_DEV_PATH_STR_LEN * sizeof(CHAR16), L'0');
						if(EfiStrCmp(devPathStr, zeroDevPathStr) == 0) //If the DevicePath string contains all zeros
						{
							CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page
							goto DONE;
						}
					}
				}

			    for(count = 0; count < gPages->PageCount; count++)
			    {
				    PAGE_INFO *tempPageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[count]);
				    if(tempPageInfo == NULL)
				    {
					    continue;
				    }
				    status = GetDevPathFromHandle(tempPageInfo->PageHandle, &tempDevPath);
				    if((status == EFI_SUCCESS) && (tempDevPath != NULL))
				    {
					    GetStringFromDevPath(tempDevPath, &tempDevPathStr);

						if(EfiStrCmp(devPathStr, tempDevPathStr) == 0)
						{
							 MemCopy(&formSetGuid, &((EFI_IFR_REF4 *)CtrlInfo->ControlPtr)->FormSetId, sizeof (EFI_GUID));

							 if(EfiCompareGuid(&formSetGuid, &zeroFormSetGuid) == FALSE) //If it is not all zeroes
							 {
								GetPageIdFromGuid(&formSetGuid, ((EFI_IFR_REF4 *)CtrlInfo->ControlPtr)->FormId, &ControlDestPageID);
								if(ControlDestPageID != 0xFFFF) //If the destination page ID is valid
								{
								 CtrlInfo->ControlDestPageID = ControlDestPageID; //Assign it to CONTROL_INFO->ControlDestPageID
								 IsFormSetIdValid = TRUE;
								 FixIFRRefXControl(PageInfo, CtrlInfo);
								}
							 }
							
							IsDevicePathFound = TRUE;
							break;
						}
					    
					    
					  
				    }
			    }
                if(!IsDevicePathFound || !IsFormSetIdValid) //the provided device path/formset guid/ formid is not found, hence remain in the same page.
                {
                    CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page
                    goto DONE;
                }
		    } 
		    else
		    { //If the Device path is not present
			    if(IsFormSetIdValid == FALSE) //If the FormSetId is invalid, checking whether a page with same form id present in Current formset. If yes, linking to that page otherwise linking to current page.
			    {
				    PAGE_ID_INFO *pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[PageInfo->PageIdIndex]);
				    if(pageIdInfo)
				    {
					    ControlDestPageID = 0xFFFF;
					    GetPageIdFromGuid(&(pageIdInfo->PageGuid), ((EFI_IFR_REF3 *)CtrlInfo->ControlPtr)->FormId, &ControlDestPageID);
					    if(ControlDestPageID != 0xFFFF) //If the destination page ID is valid, assign it to CONTROL_INFO->ControlDestPageID
					    {
					      CtrlInfo->ControlDestPageID = ControlDestPageID;
					    }
					    else
					    	CtrlInfo->ControlDestPageID = PageInfo->PageID; //The link refers to the current page
				    }
			    }
		    }
	    }
    }

DONE:
	if(devPathStr)
		MemFreePointer((VOID **)&devPathStr);
  return status;
}

/**
    Finds the page ID with matching Guid from the page list

    @param FormGuid 
    @param PageID 

    @retval EFI_STATUS status - EFI_SUCCESS, if successful,
        EFI_INVALID_PARAMETER, if invalid values found
**/
EFI_STATUS GetPageIdFromGuid(EFI_GUID *FormGuid, UINT16 PageFormID, UINT16 *PageID)
{
  EFI_STATUS status = EFI_INVALID_PARAMETER;
  UINT32 pgIndex = 0;
  PAGE_INFO *pgInfo = (PAGE_INFO *)NULL;
  PAGE_ID_INFO *pageIdInfo = (PAGE_ID_INFO*)NULL;

  if(gPages && gApplicationData && gPageIdList)
  {
    for(pgIndex = 0; pgIndex < gPages->PageCount; pgIndex++)
    {
      pgInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[pgIndex]);
      if ((NULL == pgInfo) || (NULL == pgInfo->PageHandle) || ((VOID*)0xffff == pgInfo->PageHandle)/*To avoid linking to invalid page*/)		//if relash module formsetguid passed then it will return the pageid
          continue;															//so this should not happen when the resource not published so added the condition
      if(pgInfo->PageFormID == PageFormID)
      {
        pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[pgInfo->PageIdIndex]);
        if(pageIdInfo && (EfiCompareGuid(FormGuid, &(pageIdInfo->PageGuid)) == TRUE))//Compare Guid
        {
          *PageID  = pgInfo->PageID;
          status = EFI_SUCCESS;
		  goto DONE;
        }
      }
    }
  }

DONE:
  return status;
}


/**
    Checks if the given handle is avaiable in the scope of the
    current formset.

    @param Handle 

    @retval EFI_STATUS Status - EFI_NOT_FOUND, if not found, else EFI_SUCCESS
**/
EFI_STATUS FindHandleWithInSetupData (EFI_HII_HANDLE Handle)
{
  EFI_STATUS 	Status = EFI_NOT_FOUND;
  UINTN 			i = 0;

  if (gSetupData) //Check if the notification is within out formset's scope
  {
	//All new pack updated at last of gSetupData so comparing gSetupCount - 1 is not good because on removing same pack 
	//its not able to find the existing processed pack handle hence crashed. So removed -1.
    for (i = 0; i < gSetupCount; i++)
    {
      if (gSetupData[i].Handle == Handle)
      {
        Status = EFI_SUCCESS;
        break;
      }
    }
  }
  return Status;
}

/**
    Returns the variable ID for read write operation

    @param UINT16, CONTROL_INFO *, UINT16 *

    @retval EFI_STATUS
**/
EFI_STATUS GetRWVariableID (UINT16 RWVarStoreId, CONTROL_INFO *ControlInfo, UINT16 *VarStoreId)
{
   VARIABLE_INFO 	*varInfo = NULL;
   UINT32         		index = 0;

   for (index = 0; index < gVariables->VariableCount; index++)
   {
      varInfo = (VARIABLE_INFO *)VarGetVariableInfoIndex(index);
      if(NULL == varInfo)
      {
         continue;
      }
      if ((ControlInfo->ControlHandle == varInfo->VariableHandle) && (varInfo->VariableID == RWVarStoreId))
      {
            *VarStoreId = index;
            return EFI_SUCCESS;
      }
   }
   return EFI_NOT_FOUND;
}

/**
    Stops the refresh event for control

    @param CONTROL_INFO *

    @retval VOID
**/
VOID StopRefreshOpEvent (CONTROL_INFO *controlInfo)
{
	CONTROL_INFO 	*RefreshEventcontrolInfo = (CONTROL_INFO *)NULL;
	UINTN 				index;
	EFI_STATUS 		Status = EFI_UNSUPPORTED;
	
	if (gRefreshIdInfo)
	{
		for(index = 0; index < gRefreshIdCount; index++) 
		{
			if(gRefreshIdInfo[index].pEvent) //Close the event
			{
				RefreshEventcontrolInfo = (CONTROL_INFO *)gRefreshIdInfo[index].pNotifyContext;
				if (
					(controlInfo->ControlHandle == RefreshEventcontrolInfo->ControlHandle) && 
					(controlInfo->ControlConditionalPtr == RefreshEventcontrolInfo->ControlConditionalPtr) &&
					(controlInfo->ControlPtr == RefreshEventcontrolInfo->ControlPtr)
					)
				{
					Status = gBS->CloseEvent (gRefreshIdInfo[index].pEvent);
					if(Status == EFI_SUCCESS)
					{
						gRefreshIdInfo[index].pEvent = NULL;
					}								//Dont free the pNotifyContext might be using in formcallback, so freeing here will crash in formcallback. Free in MiniSetupUIExit
					break;
				}
			}
		}
	}
}


/**
    Stops the refresh event for page

    @param PAGE_INFO *

    @retval VOID
**/
VOID StopRefreshOpEventforPage (PAGE_INFO *pageInfo)
{
	PAGE_INFO 	*RefreshEventpageInfo = (PAGE_INFO *)NULL;
	UINTN 				index;
	EFI_STATUS 		Status = EFI_UNSUPPORTED;
	
	if (gRefreshIdInfo)
	{
		for(index = 0; index < gRefreshIdCount; index++) 
		{
			if(gRefreshIdInfo[index].pEvent) //Close the event
			{
				RefreshEventpageInfo = (PAGE_INFO *)gRefreshIdInfo[index].pNotifyContext;
				if (
					(pageInfo->PageHandle == RefreshEventpageInfo->PageHandle) && 
					(pageInfo->PageID == RefreshEventpageInfo->PageID) &&
					(pageInfo->PageIdIndex == RefreshEventpageInfo->PageIdIndex)
					)
				{
					Status = gBS->CloseEvent (gRefreshIdInfo[index].pEvent);
					if(Status == EFI_SUCCESS)
					{
						gRefreshIdInfo[index].pEvent = NULL;
					}								//Dont free the pNotifyContext might be using in formcallback, so freeing here will crash in formcallback. Free in MiniSetupUIExit
					break;
				}
			}
		}
	}
}

/**
    Function to get default stringID for String Type control from controlPtr and
    fetch respective string from Hii database and set as default to NVRAM, gOptimalDefaults, gFailSafeDefaults buffer
    @param OptimalDefs NVRAM_VARIABLE *FailSafeDefs

    @retval EFI_STATUS

**/
EFI_STATUS GetSetDefaultsFromControlPtr(NVRAM_VARIABLE *optimalDefaultlist, NVRAM_VARIABLE *failSafeDefaultlist)
{
	EFI_STATUS status = EFI_UNSUPPORTED;
	BOOLEAN isDefaultStringSupported = IsSupportDefaultForStringControl();
	UINT32	page = 0, control = 0;
	VOID *defaults4FirstBootVar = NULL;
	BOOLEAN FirstEvalExprFixup = FALSE;
	UINTN varSize = 0;

	if(FormBrowserHandleValid())
		FirstEvalExprFixup = 1;
	else
	{
		defaults4FirstBootVar = (VOID *)VarGetNvramName (L"EvaluateDefaults4FirstBoot", &gEvaluateDefaults4FirstBootGuid, NULL, &varSize);
		if (NULL == defaults4FirstBootVar)
		{
			VarSetNvramName( L"EvaluateDefaults4FirstBoot", &gEvaluateDefaults4FirstBootGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, &varSize, sizeof (varSize) );
			FirstEvalExprFixup = TRUE;
		}
		else
		{
			MemFreePointer((VOID**)&defaults4FirstBootVar);		
		}
	}
		
	for ( page = 0; page < gPages->PageCount; page++ )
	{
		PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[page]);

		if(pageInfo->PageHandle ==0)
			continue;

		for ( control= 0; control < pageInfo->PageControls.ControlCount; control++ )
		{
			CONTROL_INFO *controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);
			if ( NULL == controlInfo->ControlPtr )
				continue;
				
			if ( ( controlInfo->ControlVariable == VARIABLE_ID_LANGUAGE ) || ( controlInfo->ControlVariable == VARIABLE_ID_BOOT_ORDER ) || (controlInfo->ControlVariable == VARIABLE_ID_BBS_ORDER) )
				continue;
			
			if( controlInfo->ControlDataWidth > 0 && 0 == controlInfo->ControlFlags.ControlEvaluateDefault)
			{
				//To update Optimal and FailSafe defaults with STRING default based on StringID from controlPtr
				if ((controlInfo->ControlType == CONTROL_TYPE_POPUP_STRING) && isDefaultStringSupported && (controlInfo->ControlDataWidth > 0) )
				{
					if ( (optimalDefaultlist) || (failSafeDefaultlist))
					{
						CHAR16 *failDefStr = NULL, *optimalDefStr = NULL;
						UINT8 *failValue, *optValue;
						UINT32 ControlVarOffset, size = 0;
	                    UINTN Size = 0;

						//Getting stringID if any present in controlPtr for failsafe default
						failValue = (UINT8 *)controlInfo + sizeof(CONTROL_INFO);
					
						//Getting stringID if any present in controlPtr for optimal default
						optValue = failValue + controlInfo->ControlDataWidth;
						//size = controlInfo->ControlDataWidth;

						size = UefiGetWidth (controlInfo->ControlPtr) * sizeof(CHAR16); 	
						ControlVarOffset = UefiGetQuestionOffset(controlInfo->ControlPtr);
					
						//Getting string based on StrigID from controlPtr and updating failsafeDefaults
						if (NULL != failSafeDefaultlist)
						{
							if (*(UINT16*)(failValue))
							{
								failDefStr = (VOID*)HiiGetString( controlInfo->ControlHandle, *(UINT16*)(failValue));
								if (NULL == failDefStr)
									continue;
								
		                        if (!EfiStrLen(failDefStr)) 
		                        {
		                            Size = sizeof(CHAR16);                          //Since empty string is setting as default we can allocate 2 bytes
		                            MemFreePointer( (VOID **)&failDefStr);
		                            failDefStr = (CHAR16 *)EfiLibAllocateZeroPool (sizeof(CHAR16));
		                        }
		                        else
		                            Size  = EfiStrLen(failDefStr)*sizeof(CHAR16);

								//Updating Manufacturing/FailSafe Defaults buffer.
								_VarGetSetValue( VAR_COMMAND_SET_VALUE, gFailsafeDefaults, controlInfo->ControlVariable, ControlVarOffset, Size, failDefStr );								
								if (failDefStr)
									MemFreePointer( (VOID **)&failDefStr );
							}
						}

						//Getting string based on StrigID from controlPtr and updating optimalDefaults
						if (NULL != optimalDefaultlist)
						{
							if (*(UINT16*)(optValue))
							{
							    UINTN Size = 0;
							    
								optimalDefStr = (VOID*)HiiGetString( controlInfo->ControlHandle, *(UINT16*)(optValue));
								if (NULL == optimalDefStr)
									continue;
								
		                        if (!EfiStrLen(optimalDefStr)) 
		                        {
		                            Size = sizeof(CHAR16);                          //Since empty string is setting as default we can allocate 2 bytes
		                            MemFreePointer( (VOID **)&optimalDefStr);
		                            optimalDefStr = (CHAR16 *)EfiLibAllocateZeroPool (sizeof(CHAR16));
		                        }
		                        else
		                            Size  = EfiStrLen(optimalDefStr)*sizeof(CHAR16);

								//Updating Optimal Defaults buffer.
								status = _VarGetSetValue( VAR_COMMAND_SET_VALUE, gOptimalDefaults, controlInfo->ControlVariable, ControlVarOffset, Size, optimalDefStr );
								if (optimalDefStr)
									MemFreePointer( (VOID **)&optimalDefStr );
							}
						}
					}//End of optimal/Failsafe condition check
				}//End of ControlType condition check
			}
			else if (IsDefaultConditionalExpression() && (controlInfo->ControlFlags.ControlEvaluateDefault) && (TRUE == FirstEvalExprFixup))
			{	//If default is set with conditional expression then evaluate the expression
				UINT64 defaults = 0 ;
				VOID *defaultStr = NULL;
				UINTN size = 0;
				UINT32 controlVarOffset = 0;
				UINT8* tempNvBuffer = 0;
				UINTN tempNvBufferSize = 0;

				controlVarOffset = UefiGetQuestionOffset(controlInfo->ControlPtr);

				//Evaluating the Control Default
				size = EvaluateControlDefault(controlInfo,&defaults);
				if (0 == size)
				{
					continue;
				}
				defaultStr = (VOID*)&defaults;

				if (CONTROL_TYPE_POPUP_STRING == controlInfo->ControlType && TRUE == isDefaultStringSupported)
				{//For String controls Defaults will be string token, so need to get the string for this.
					defaultStr = (VOID*)HiiGetString(controlInfo->ControlHandle, (UINT16)defaults);
					if (NULL == defaultStr)
						continue;
					size = (EfiStrLen(defaultStr) + 1)*sizeof(CHAR16);
				}
					
				//Updating gVariablesList buffer.
				VarSetValue(controlInfo->ControlVariable, controlVarOffset, size, defaultStr );
				
				//Updating Manufacturing/FailSafe Defaults buffer.
				//_VarGetSetValue( VAR_COMMAND_SET_VALUE, gFailsafeDefaults, controlInfo->ControlVariable, controlVarOffset, size, &defaults );

				//Updating Optimal Defaults buffer.
				_VarGetSetValue(VAR_COMMAND_SET_VALUE, gOptimalDefaults, controlInfo->ControlVariable, controlVarOffset, size, defaultStr);
				
				//Update the value in NvRAM immediately for LoadPrevious to work properly
				tempNvBuffer = (UINT8*)VarGetNvram(controlInfo->ControlVariable, &tempNvBufferSize);
				if(tempNvBuffer)
				{
					MemCopy(tempNvBuffer+controlVarOffset, defaultStr, size);
					VarSetNvram(controlInfo->ControlVariable, (void*)(tempNvBuffer), tempNvBufferSize);
					MemFreePointer((VOID **)&tempNvBuffer);
				}

				//Free the HiiString allocated string memory
				if(defaultStr && CONTROL_TYPE_POPUP_STRING == controlInfo->ControlType && TRUE == isDefaultStringSupported)
					MemFreePointer( (VOID **)&defaultStr );
			}
		}//End of control iteration
	}//End of Page iteration
	return status;
}

/**
    Function to check whether the page has EFI_IFR_REFRESH_ID_OP opcode and 
	create event for this page.
    
    @param 	PAGE_INFO *PageInfo
			UINT16 Link

    @retval VOID

**/

VOID CheckRefreshIDForPage(PAGE_INFO *PageInfo,UINT16 Link)
{
	UINT32	i =0,index = 0;
	UINT8* ifrData = NULL;
	EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)NULL;
	EFI_HII_PACKAGE_HEADER *pkgHdr= (EFI_HII_PACKAGE_HEADER*)NULL;
	PAGE_ID_INFO *PageIdInfo = (PAGE_ID_INFO *)NULL;
	
	pkgHdr=(EFI_HII_PACKAGE_HEADER *)gSetupData[Link].FormSet;
				
	if(pkgHdr->Type == EFI_HII_PACKAGE_FORMS)
	{
		ifrData = ((UINT8 *)pkgHdr) + sizeof(EFI_HII_PACKAGE_HEADER);
		
		//search for the EFI_IFR_REFRESH_ID_OP.if Found then only type cast and assign RefreshEventGroupId and create event for IFR.
		while(i < pkgHdr->Length)
		{
			opHeader = (EFI_IFR_OP_HEADER*)(ifrData + i);
		
			PageIdInfo = (PAGE_ID_INFO*)((UINT8*)gToolPageIdList + gToolPageIdList->PageIdList[PageInfo->PageIdIndex]);

			if((opHeader->OpCode == EFI_IFR_FORM_SET_OP) && EfiCompareGuid(&PageIdInfo->PageGuid,&((EFI_IFR_FORM_SET *)opHeader)->Guid)) // to get current formset
			{
				while(i < pkgHdr->Length)
				{
					opHeader = (EFI_IFR_OP_HEADER*)(ifrData + i);
					
					if(opHeader->OpCode == EFI_IFR_FORM_OP)	
					{
						if(((EFI_IFR_FORM *)opHeader)->FormId == PageInfo->PageFormID) // to get current form header
						{
							while(1)//Check for EFI_IFR_REFRESH_ID_OP until endform reaches,if available then create event
							{
								opHeader = (EFI_IFR_OP_HEADER*)(ifrData + i);
								if((EFI_IFR_REFRESH_ID_OP == ((EFI_IFR_OP_HEADER *)(opHeader))->OpCode) && (index == 1))
								{
									CreateEventForPageRefresh (PageInfo,((AMI_EFI_IFR_REFRESH_ID *)(opHeader))->RefreshEventGroupId);
									return;
								}
								if((((EFI_IFR_OP_HEADER *)(opHeader))->Scope))//if OpcodeHeader has scope then, Increment index
									index++;
								if(EFI_IFR_END_OP == ((EFI_IFR_OP_HEADER *)(opHeader))->OpCode)//if EFI_IFR_END_OP found then,decrement index
									index--;
								i+=((EFI_IFR_OP_HEADER *)(opHeader))->Length;//point to next opcode header depending upon the Length
								if(!index)
									break;
							}
							return; 
						}
					}
					i+=((EFI_IFR_OP_HEADER *)(opHeader))->Length;//point to next opcode header depending upon the Length
				}
			}	
		i+=((EFI_IFR_OP_HEADER *)(opHeader))->Length;//point to next opcode header depending upon the Length	
		}
	}
}
/**
    This is the notification function to Evaluates the expression for setting defaults at first time
**/
VOID DefaultsEvaluateExpression (EFI_EVENT Event, VOID *Context)
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	void 		*defaults4FirstBootVar = NULL;
	UINTN 		varSize = 0;
	
	defaults4FirstBootVar = (VOID *)VarGetNvramName (L"EvaluateDefaults4FirstBoot", &gEvaluateDefaults4FirstBootGuid, NULL, &varSize);
	if (NULL == defaults4FirstBootVar)
	{
	 	if(gApplicationData == NULL )
	 	{
			InitAmiLib(gImageHandle,gST);
	   		Status = InitApplicationData(gImageHandle); // To initialize gApplication
	     	if(EFI_ERROR(Status)) return;
			InitGlobalPointers();  		// To initialize global buffers
	 	}
	    Status = HiiFixupData();
	    if(EFI_ERROR(Status)) return;
	    Status = VarBuildDefaults();
	    if(EFI_ERROR(Status)) return;
	}
	else
	{
		MemFreePointer((VOID**)&defaults4FirstBootVar);
	}
	if(Event != NULL)
	{
		gBS->CloseEvent(Event);
	}
}

/**

    @param variable UINT16 PageClass

    @retval VOID
**/
VOID DecrementDynamicPageCount(UINT16 PageClass)
{
    
    UINTN count = gDynamicPageCount;
    EFI_GUID    DynamicPageGuid = DYNAMIC_PAGE_COUNT_GUID;
    
    SETUP_DEBUG_UEFI("\n[TSE] Inside DecrementDynamicPageCount : PageClass = %d\n",PageClass);
    if(count) //Decrement the gDynamicPageCount if that page is already added as dynamic
    {
        count-- ;
        gDynamicPageCount = count ;   
        VarSetNvramName( L"DynamicPageCount", &DynamicPageGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &count, sizeof(count) );
        VarUpdateVariable(VARIABLE_ID_DYNAMIC_PAGE_COUNT);
    }
    
    if (IsGroupDynamicPagesByClass()){
        UINT16 index = 0xFFFF ;

        switch(PageClass)
        {
        case EFI_NON_DEVICE_CLASS : index = 0 ; break ;
        case EFI_DISK_DEVICE_CLASS : index = 1 ; break ;
        case EFI_VIDEO_DEVICE_CLASS : index = 2 ; break ;
        case EFI_NETWORK_DEVICE_CLASS : index = 3 ; break ;
        case EFI_INPUT_DEVICE_CLASS : index = 4 ; break ;
        case EFI_ON_BOARD_DEVICE_CLASS : index = 5 ; break ;
        case EFI_OTHER_DEVICE_CLASS : index = 6 ;  break ;
        default:
            break;
        }
        if((index != 0xFFFF) && (gDynamicPageGroup [index].Count > 0)){    
            gDynamicPageGroup [index].Count-- ; 
            SetDynamicPageGroupCount() ;
            SETUP_DEBUG_UEFI("Decremented gDynamicPageGroup[%d].Count = %d \n",index,gDynamicPageGroup [index].Count);
        }    
    }
    SETUP_DEBUG_UEFI("Decremented values : gDynamicPageCount = %d, gDynamicPageGroupCount = %d\n",gDynamicPageCount,gDynamicPageGroupCount);
    SETUP_DEBUG_UEFI("\n[TSE] Exiting DecrementDynamicPageCount :\n"); 
}   
 
/**

    @param variable UINT16 PageClass

    @retval VOID
**/
VOID IncrementDynamicPageCount(UINT16 PageClass)
{
    
    UINTN count = gDynamicPageCount;
    EFI_GUID    DynamicPageGuid = DYNAMIC_PAGE_COUNT_GUID;
    
    SETUP_DEBUG_UEFI("\n[TSE] Inside IncrementDynamicPageCount : PageClass = %d\n",PageClass);
    count++ ;
    gDynamicPageCount = count ;   
    VarSetNvramName( L"DynamicPageCount", &DynamicPageGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &count, sizeof(count) );
    VarUpdateVariable(VARIABLE_ID_DYNAMIC_PAGE_COUNT);
    
    if (IsGroupDynamicPagesByClass()){
        UINT16 index = 0xFFFF ;
        
        switch(PageClass)
        {
        case EFI_NON_DEVICE_CLASS : index = 0 ; break ;
        case EFI_DISK_DEVICE_CLASS : index = 1 ; break ;
        case EFI_VIDEO_DEVICE_CLASS : index = 2 ; break ;
        case EFI_NETWORK_DEVICE_CLASS : index = 3 ; break ;
        case EFI_INPUT_DEVICE_CLASS : index = 4 ; break ;
        case EFI_ON_BOARD_DEVICE_CLASS : index = 5 ; break ;
        case EFI_OTHER_DEVICE_CLASS : index = 6 ;  break ;
        default:
            break;
        }
        if(index != 0xFFFF){    
            gDynamicPageGroup [index].Count++ ; 
            SetDynamicPageGroupCount() ;
            SETUP_DEBUG_UEFI("Incremented gDynamicPageGroup[%d].Count = %d \n",index,gDynamicPageGroup [index].Count);
        }    
    }
    SETUP_DEBUG_UEFI("Incremented values : gDynamicPageCount = %d, gDynamicPageGroupCount = %d\n",gDynamicPageCount,gDynamicPageGroupCount);
    SETUP_DEBUG_UEFI("\n[TSE] Exiting IncrementDynamicPageCount :\n");
}

/**

    @param VOID
    @retval VOID
**/
VOID InitializeDynamicCount(VOID)
{
    int iIndex = 0,count = 0;
    EFI_GUID DynamicPageGuid = DYNAMIC_PAGE_COUNT_GUID;
    
    gDynamicPageCount = count;
    VarSetNvramName( L"DynamicPageCount", &DynamicPageGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &count, sizeof(count) );
    VarUpdateVariable(VARIABLE_ID_DYNAMIC_PAGE_COUNT);
    
    for (iIndex = 0; iIndex < DYNAMIC_PAGE_GROUPS_SIZE ; iIndex ++)
    {
        gDynamicPageGroup[iIndex].Count = 0;
    }
    SetDynamicPageGroupCount() ;
}

/**
    Function to save Setup handle and its related global buffers into Global buffer stack
    
    @param 	TSE_SETUP_GLOBAL_DATA *Data

    @retval VOID

**/
VOID SaveSetupData(TSE_SETUP_GLOBAL_DATA *Data)
{
	Data->SetupData.SetupCount = gSetupCount;
	gSetupCount = 0;
	
	Data->SetupData.SetupData = (VOID*)gSetupData;
	gSetupData = (SETUP_LINK*)NULL;
	
	Data->SetupData.SetupHandles = gSetupHandles;
	gSetupHandles = (EFI_HII_HANDLE*)NULL;
}

/**
    Function to restore Setup handle and its related global buffers from Global buffer stack
    
    @param 	TSE_SETUP_GLOBAL_DATA *Data

    @retval VOID

**/
VOID RestoreSetupData(TSE_SETUP_GLOBAL_DATA *Data)
{
	gSetupCount = Data->SetupData.SetupCount ;
	
	gSetupData = (SETUP_LINK*)Data->SetupData.SetupData;
	
	gSetupHandles = Data->SetupData.SetupHandles;
}

/**
    Function to check whether the SendForm page has EFI_IFR_REFRESH_ID_OP opcode and 
	create event for this page.
    
    @param 	PAGE_INFO *PageInfo
			UINT16 Link

    @retval VOID

**/

VOID CheckRefreshIDForSFPage(PAGE_INFO *PageInfo,UINT16 Link)
{
	UINT32	i =0,index = 0;
	UINT8* ifrData = NULL;
	EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)NULL;
	EFI_HII_PACKAGE_HEADER *pkgHdr= (EFI_HII_PACKAGE_HEADER*)NULL;
	PAGE_ID_INFO *PageIdInfo = (PAGE_ID_INFO *)NULL;
	
	pkgHdr=(EFI_HII_PACKAGE_HEADER *)gSetupData[Link].FormSet;
				
	if(pkgHdr->Type == EFI_HII_PACKAGE_FORMS)
	{
		ifrData = ((UINT8 *)pkgHdr) + sizeof(EFI_HII_PACKAGE_HEADER);
		
		//search for the EFI_IFR_REFRESH_ID_OP.if Found then only type cast and assign RefreshEventGroupId and create event for IFR.
		while(i < pkgHdr->Length)
		{
			opHeader = (EFI_IFR_OP_HEADER*)(ifrData + i);
		
			PageIdInfo = (PAGE_ID_INFO*)((UINT8*)gPageIdList + gPageIdList->PageIdList[PageInfo->PageIdIndex]);

			if((opHeader->OpCode == EFI_IFR_FORM_SET_OP) && EfiCompareGuid(&PageIdInfo->PageGuid,&((EFI_IFR_FORM_SET *)opHeader)->Guid)) // to get current formset
			{
				while(i < pkgHdr->Length)
				{
					opHeader = (EFI_IFR_OP_HEADER*)(ifrData + i);
					
					if(opHeader->OpCode == EFI_IFR_FORM_OP)	
					{
						if(((EFI_IFR_FORM *)opHeader)->FormId == PageInfo->PageFormID) // to get current form header
						{
							while(1)//Check for EFI_IFR_REFRESH_ID_OP until endform reaches,if available then create event
							{
								opHeader = (EFI_IFR_OP_HEADER*)(ifrData + i);
								if((EFI_IFR_REFRESH_ID_OP == ((EFI_IFR_OP_HEADER *)(opHeader))->OpCode) && (index == 1))
								{
									CreateEventForPageRefresh (PageInfo,((AMI_EFI_IFR_REFRESH_ID *)(opHeader))->RefreshEventGroupId);
									return;
								}
								if((((EFI_IFR_OP_HEADER *)(opHeader))->Scope))//if OpcodeHeader has scope then, Increment index
									index++;
								if(EFI_IFR_END_OP == ((EFI_IFR_OP_HEADER *)(opHeader))->OpCode)//if EFI_IFR_END_OP found then,decrement index
									index--;
								i+=((EFI_IFR_OP_HEADER *)(opHeader))->Length;//point to next opcode header depending upon the Length
								if(!index)
									break;
							}
							return; 
						}
					}
					i+=((EFI_IFR_OP_HEADER *)(opHeader))->Length;//point to next opcode header depending upon the Length
				}
			}	
		i+=((EFI_IFR_OP_HEADER *)(opHeader))->Length;//point to next opcode header depending upon the Length	
		}
	}
}

VOID AddNewHandleIntoList(EFI_HANDLE Handle)
{
	if(gNewHandleCount == 0) 
		gNewHandleList = EfiLibAllocateZeroPool(sizeof(EFI_HANDLE));
	else
		gNewHandleList = MemReallocateZeroPool(gNewHandleList,gNewHandleCount * sizeof(EFI_HANDLE), (gNewHandleCount + 1) * sizeof(EFI_HANDLE));
	
	gNewHandleList[gNewHandleCount++] = Handle;
	
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
