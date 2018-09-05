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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/HiiNotificationHandler.c $
//
// $Author: Arunsb $
//
// $Revision: 11 $
//
// $Date: 5/26/12 5:20a $
//
//*****************************************************************//
/** @file HiiNotificationHandler.c
    Contains Hii related Functions

**/
//*************************************************************************

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "minisetup.h"
#include "TseUefiHii.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define BY_ENTRY         0
#define BY_HANDLE        1
#define BY_TYPE          2
#define BY_HANDLE_TYPE   3

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
typedef struct _NOTIFICATION_INFO NOTIFICATION_INFO;

struct _NOTIFICATION_INFO 
{
  EFI_HII_HANDLE Handle;
  EFI_HII_DATABASE_NOTIFY_TYPE NotifyType;
  UINT8 *PackData;
  UINTN DataLength;
  NOTIFICATION_INFO *prevNotifInfo;
  NOTIFICATION_INFO *nextNotifInfo;
};

typedef struct _NOTIFICATION_QUEUE
{
  UINTN NumOfNotification;
  NOTIFICATION_INFO *headNotifInfo;
  NOTIFICATION_INFO *tailNotifInfo;
} NOTIFICATION_QUEUE;

//---------------------------------------------------------------------------
// Externs
//---------------------------------------------------------------------------
extern EFI_STATUS FindHandleWithInSetupData(EFI_HII_HANDLE Handle);
extern EFI_BROWSER_ACTION gBrowserCallbackAction ;
extern BOOLEAN gBrowserCallbackEnabled;
//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------
BOOLEAN gIFRChangeNotify = FALSE;
EFI_HII_HANDLE gRemovedHandle=NULL;
EFI_HANDLE 		*gReconnectPageHandles =NULL;
extern BOOLEAN gEnableDrvNotification;
BOOLEAN gEnableProcessPack = FALSE; //TRUE if the process pack is allowed, FALSE if to be ignored
extern VOID    **gSfHandles;
extern UINTN   gSfHandleCount;

//static EFI_HANDLE gIFRHandle; Not used
//static UINTN gNotifyType; Not used
#ifndef TSE_FOR_APTIO_4_50
#define EFI_HII_PACKAGE_FORMS                EFI_HII_PACKAGE_FORM
#include "tianohii.h"
#endif

NOTIFICATION_QUEUE *gNotificationQueue = (NOTIFICATION_QUEUE *)NULL; //This is a pointer to the notifiaction queue
BOOLEAN gPackUpdatePending = FALSE;

//---------------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------------
EFI_STATUS AddNotificationToQueue(EFI_HII_HANDLE Handle, EFI_HII_DATABASE_NOTIFY_TYPE Type);
EFI_STATUS ProcessPackNotificationHook (VOID);
NOTIFICATION_INFO *GetNotificationFromCurrentSfHandles();
//---------------------------------------------------------------------------
// Function Implementation
//---------------------------------------------------------------------------

/**
    Notification Registering Function

    @param PackageNotifyFn 
    @param NotifyType 
    @param NotifyHandle 

    @retval EFI_STATUS
        EFI_SUCCESS
**/
EFI_STATUS RegisterFormNotification(EFI_HII_DATABASE_NOTIFY PackageNotifyFn,
                                    EFI_HII_DATABASE_NOTIFY_TYPE NotifyType,
                                    EFI_HANDLE *NotifyHandle)
{
  //
  // Register notification
  //
  return gHiiDatabase->RegisterPackageNotify (
                           gHiiDatabase,
                           EFI_HII_PACKAGE_FORMS,
                           NULL,
                           PackageNotifyFn,
                           NotifyType,
                           NotifyHandle
                           );
}

/**
    Notification UnRegistering Function

    @param NotifyHandle 

    @retval EFI_STATUS
        EFI_SUCCESS
**/
EFI_STATUS UnRegisterFormNotification(EFI_HANDLE NotifyHandle)
{
  //
  // UnRegister notification
  //
  return gHiiDatabase->UnregisterPackageNotify (gHiiDatabase, NotifyHandle);
}

/**
    Function that handles IFR change notification

    @param PackageType 
    @param PackageGuid 
    @param Package 
    @param Handle 
    @param NotifyType 

    @retval EFI_STATUS
        EFI_SUCCESS
**/
EFI_STATUS
IFRChangeNotifyFn (
	IN UINT8                              PackageType,
	IN CONST EFI_GUID                     *PackageGuid,
	IN CONST EFI_HII_PACKAGE_HEADER       *Package,
	IN EFI_HII_HANDLE                     Handle,
	IN EFI_HII_DATABASE_NOTIFY_TYPE       NotifyType
	)
{
	EFI_STATUS status = EFI_NOT_FOUND;
	
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Entering IFRChangeNotifyFn() \n\n");
	SETUP_DEBUG_UEFI_NOTIFICATION ("\n\n[TSE] Details are Handle %x NotifyType %x\n\n",  Handle,  NotifyType);
	
	if (FALSE == gEnableDrvNotification)		
	{
		SETUP_DEBUG_UEFI_NOTIFICATION ("\n\n[TSE] Illegal Notification (Type: NotifyType 0x%x) received. \n\n", NotifyType);
	}
	
  	status = AddNotificationToQueue (Handle, NotifyType); //Adding notification to the queue
	if(status != EFI_SUCCESS) //Failed to add notification in the queue, return error
	{
		goto DONE;
	}
	gPackUpdatePending = TRUE;
	
	if(gEnableDrvNotification && !gEnableProcessPack)
	{
		SETUP_DEBUG_UEFI_NOTIFICATION ("\n\n[TSE] IFRChangeNotifyFn() calling ProcessPackNotificationHook() \n\n");
		gEnableProcessPack = TRUE;
		status = ProcessPackNotificationHook (); // Synchronized Notification Call happening because of Callback,ExtractConfig. routeconfig.
		gEnableProcessPack = FALSE;
	}
DONE:
  SETUP_DEBUG_UEFI_NOTIFICATION ("\n\n[TSE] Exiting IFRChangeNotifyFn() status: %x\n\n", status);
  return status;
}

/**
    Adds new notification to the queue and increments queue size

    @param Handle EFI_HII_DATABASE_NOTIFY_TYPE Type

    @retval EFI_STATUS status - EFI_ABORTED, By default
        EFI_OUT_OF_RESOURCES, if memory allocation fails
        EFI_SUCCESS, if successful
**/
EFI_STATUS AddNotificationToQueue (EFI_HII_HANDLE Handle, EFI_HII_DATABASE_NOTIFY_TYPE Type)
{
	EFI_STATUS status = EFI_ABORTED;
	NOTIFICATION_INFO *tempNotifInfo = (NOTIFICATION_INFO *)NULL;
	
	if (gNotificationQueue == NULL)
	{
		gNotificationQueue = EfiLibAllocateZeroPool (sizeof (NOTIFICATION_QUEUE));
		if (NULL == gNotificationQueue)
		{
			status = EFI_OUT_OF_RESOURCES; //Cannot allocate memory for the notification queue
			goto DONE;
		}
	}
	if (NULL == tempNotifInfo)
	{
		tempNotifInfo = EfiLibAllocateZeroPool(sizeof(NOTIFICATION_INFO));
		if (NULL == tempNotifInfo)
		{
			status = EFI_OUT_OF_RESOURCES; //Cannot allocate memory for the notification queue
			goto DONE;
		}
	}
	tempNotifInfo->Handle = Handle;
	tempNotifInfo->NotifyType = Type;
	if((Type == EFI_HII_DATABASE_NOTIFY_NEW_PACK) || (Type == EFI_HII_DATABASE_NOTIFY_ADD_PACK))
	{
		tempNotifInfo->PackData = (UINT8*)HiiGetForm(Handle, 0, &tempNotifInfo->DataLength);
	}
	if((gNotificationQueue->NumOfNotification == 0) || (gNotificationQueue->headNotifInfo == NULL))
	{
		gNotificationQueue->headNotifInfo = tempNotifInfo;
		gNotificationQueue->tailNotifInfo = tempNotifInfo;
	}
	else
	{
		tempNotifInfo->prevNotifInfo = gNotificationQueue->tailNotifInfo;
		gNotificationQueue->tailNotifInfo = tempNotifInfo;
		gNotificationQueue->tailNotifInfo->prevNotifInfo->nextNotifInfo = tempNotifInfo;
	}
	gNotificationQueue->NumOfNotification++;
	status = EFI_SUCCESS;
DONE:
	return status;
}

/**
    Removes the notification entry from the queue, and returns
    the pointer to the next notification node

    @param matchingNotifInfo 

    @retval VOID
**/
VOID RemoveFromQueueByEntry(NOTIFICATION_INFO *matchingNotifInfo)
{
	NOTIFICATION_INFO *tempNotifInfo = matchingNotifInfo;
	
	if ((NULL == tempNotifInfo) || (NULL == gNotificationQueue) || (0 == gNotificationQueue->NumOfNotification)){
		goto DONE;
	}
	if(gNotificationQueue->headNotifInfo == tempNotifInfo){
		if (gNotificationQueue->headNotifInfo == gNotificationQueue->tailNotifInfo){
			gNotificationQueue->tailNotifInfo = tempNotifInfo->nextNotifInfo;
		}
		gNotificationQueue->headNotifInfo = tempNotifInfo->nextNotifInfo;
	}else if(gNotificationQueue->tailNotifInfo == tempNotifInfo){
		gNotificationQueue->tailNotifInfo = tempNotifInfo->prevNotifInfo;
	}
	if(tempNotifInfo->prevNotifInfo){
		tempNotifInfo->prevNotifInfo->nextNotifInfo = tempNotifInfo->nextNotifInfo;
	}
	if(tempNotifInfo->nextNotifInfo){
		tempNotifInfo->nextNotifInfo->prevNotifInfo = tempNotifInfo->prevNotifInfo;
	}
	gNotificationQueue->NumOfNotification--; //Decrement the number of notification by 1
	MemFreePointer((VOID**)&tempNotifInfo); //Free the node
DONE:
	return;
}

/**
    Removes the all the node with same notification handle and
    type except the passed the entry from the queue

    @param Handle 
    @param Type 
    @param matchingNotifInfo 

    @retval EFI_STATUS status - EFI_ABORTED, By default ;
        EFI_INVALID_PARAMETER, if invalid gNotificationQueue
        EFI_SUCCESS, if successful
**/
EFI_STATUS RemoveNotifFromQueueByHandleType (UINTN RemoveOption, EFI_HII_HANDLE Handle, EFI_HII_DATABASE_NOTIFY_TYPE Type, NOTIFICATION_INFO *matchingNotifInfo)
{
	EFI_STATUS status = EFI_NOT_FOUND;
	NOTIFICATION_INFO *tempNotifInfo = (NOTIFICATION_INFO *)NULL;
	NOTIFICATION_INFO *tempPrevNotifInfo = (NOTIFICATION_INFO *)NULL;
	NOTIFICATION_INFO *tempNextNotifInfo = (NOTIFICATION_INFO *)NULL;
	NOTIFICATION_INFO *currNotifInfo = (NOTIFICATION_INFO *)NULL;
	EFI_HII_HANDLE NotifyHandle = (EFI_HII_HANDLE)NULL;
	EFI_HII_DATABASE_NOTIFY_TYPE NotifyType = 0;
	BOOLEAN found = FALSE;
	
	if (NULL == gNotificationQueue){
		status = EFI_INVALID_PARAMETER; //gNotificationQueue invalid
		goto DONE;
	}
	
	//Taking backup of prevNotifInfo before removing the node from queue
	if (NULL != matchingNotifInfo)
	{
		tempPrevNotifInfo = matchingNotifInfo->prevNotifInfo;
		tempNextNotifInfo = matchingNotifInfo->nextNotifInfo;
	}
		
	if((Handle != NULL) || (Type != 0)){
		NotifyHandle = Handle;
		NotifyType = Type;
	} else if (NULL != matchingNotifInfo){
		NotifyHandle = matchingNotifInfo->Handle;
		NotifyType = matchingNotifInfo->NotifyType;
	}
	tempNotifInfo = gNotificationQueue->headNotifInfo;
	while(tempNotifInfo != NULL)
	{
		currNotifInfo = tempNotifInfo->nextNotifInfo;
		if (BY_ENTRY == RemoveOption){
			if (tempNotifInfo == matchingNotifInfo){
				RemoveFromQueueByEntry(tempNotifInfo);
				status = EFI_SUCCESS;
				break;
			}
		}else if(tempNotifInfo != matchingNotifInfo){	//Match found
			if (BY_HANDLE == RemoveOption){
				//Don't remove node if any node has NEW Pack with same handle
				if (EFI_HII_DATABASE_NOTIFY_NEW_PACK != tempNotifInfo->NotifyType && tempNotifInfo->Handle == NotifyHandle){					
					RemoveFromQueueByEntry(tempNotifInfo);
					status = EFI_SUCCESS;
				}
			} else if(RemoveOption == BY_TYPE){
				if(tempNotifInfo->NotifyType == NotifyType){
					RemoveFromQueueByEntry(tempNotifInfo);
					status = EFI_SUCCESS;
				}
			} else if(RemoveOption == BY_HANDLE_TYPE){	
//If match found process the node and move to the next node
				if((tempNotifInfo->Handle == NotifyHandle) && (tempNotifInfo->NotifyType == NotifyType)){
					RemoveFromQueueByEntry(tempNotifInfo);
					status = EFI_SUCCESS;
				}
			}
		}
		tempNotifInfo = currNotifInfo;
	}

	//Reseting tempNotifInfo to HeadNode for node traversal
	tempNotifInfo = gNotificationQueue->headNotifInfo;

	//Traverse all nodes to check whether the removed node is present in the list or not
	if (NULL != matchingNotifInfo)
	{
		while (tempNotifInfo)
		{
			if (matchingNotifInfo == tempNotifInfo)
			{
				found = TRUE;
				break;
			}
			tempNotifInfo = tempNotifInfo->nextNotifInfo;
		}
		if (!found)
		{
			//Resetting prevNode and nextNode address not to skip any node if node is delete either by HeadNode or by TailNode Iteration
			matchingNotifInfo->prevNotifInfo = tempPrevNotifInfo;
			matchingNotifInfo->nextNotifInfo = tempNextNotifInfo;
		}
	}

DONE:
	return status;
}

/**
    Cleans the IFR notification queue

    @param VOID

    @retval EFI_STATUS status - EFI_SUCCESS, if successful
**/
EFI_STATUS CleanNotificationQueue()
{
	EFI_STATUS status = EFI_ABORTED;
	
	if(IsSetupFormBrowserNestedSendFormSupport() && gSfHandles && gSfHandleCount)/*Clean the notifications corresponds to handles of current SendForm, gNotificationQueue may contains pending notifications of main Setup*/
	{
		UINTN i;
		NOTIFICATION_INFO *tempNotifInfo = (NOTIFICATION_INFO *)NULL,*currNotifInfo = (NOTIFICATION_INFO *)NULL;

		for (i = 0; i < gSfHandleCount ; i++)
		{
			tempNotifInfo = gNotificationQueue->headNotifInfo;
			while(tempNotifInfo != NULL)
			{
				currNotifInfo = tempNotifInfo->nextNotifInfo;
				if (tempNotifInfo->Handle == gSfHandles[i]){					
					RemoveFromQueueByEntry(tempNotifInfo);
				}
				tempNotifInfo = currNotifInfo;
			}
		}
		status = EFI_SUCCESS;
		goto DONE;
	}
	do //Loop to clean up the notification queue
	{
		if((NULL == gNotificationQueue) || (NULL == gNotificationQueue->headNotifInfo))
		{
			status = EFI_SUCCESS; //The is no notification to process
			goto DONE;
		}
		RemoveFromQueueByEntry(gNotificationQueue->headNotifInfo); //Removes this node
	}while (1);
	if (gNotificationQueue)
	{
		MemFreePointer ((VOID**)&gNotificationQueue);
		gNotificationQueue = (NOTIFICATION_QUEUE *)NULL; //Reset the Notification Queue
	}
DONE:
	return status;
}

/**
    Finds duplicate notification on the same handle and remove

    @param VOID

    @retval EFI_STATUS status - EFI_ABORTED, By default ;
        EFI_INVALID_PARAMETER, if invalid gNotificationQueue
        EFI_SUCCESS, if successful
**/
EFI_STATUS RemoveDuplicateIFRNotification()
{
	EFI_STATUS 		status = EFI_ABORTED;
//	EFI_HII_HANDLE Handle = (EFI_HII_HANDLE)NULL; Not used
	NOTIFICATION_INFO *tempNotifInfo = (NOTIFICATION_INFO *)NULL;
//	EFI_HII_DATABASE_NOTIFY_TYPE NotifyType = 0;
    BOOLEAN packFound = FALSE;	
	
	if((gNotificationQueue == NULL) || (gNotificationQueue->headNotifInfo == NULL))
	{
		status = EFI_SUCCESS; //The is no notification to process
		goto DONE;
	}
	SETUP_DEBUG_UEFI_NOTIFICATION ("\n\n[TSE] Before RemoveDuplicateIFRNotification(), NumOfNotification left: %d\n\n", gNotificationQueue->NumOfNotification);
	
	tempNotifInfo = gNotificationQueue->tailNotifInfo;
//	Handle = tempNotifInfo->Handle;  Not used
//	NotifyType = tempNotifInfo->NotifyType; Not used
	
	while(tempNotifInfo != NULL) //Loop to removed duplicate notifications
	{
    	if((EFI_HII_DATABASE_NOTIFY_REMOVE_PACK == tempNotifInfo->NotifyType) || 
           (EFI_HII_DATABASE_NOTIFY_ADD_PACK == tempNotifInfo->NotifyType)){
			RemoveNotifFromQueueByHandleType (BY_HANDLE_TYPE, 0, 0, tempNotifInfo); //If the handle belongs to the formset, clean up notification queue
		}
		if(tempNotifInfo != NULL){  //If the current node is valid
			tempNotifInfo = tempNotifInfo->prevNotifInfo; //Move current notification node pointer to previous notification node
		}
	}
	tempNotifInfo = gNotificationQueue->tailNotifInfo;
	while(tempNotifInfo != NULL) //Loop to removed duplicate notifications
	{
		status = FindHandleWithInSetupData(tempNotifInfo->Handle);
		if (EFI_SUCCESS == status){
			packFound = TRUE;
		}
	
		if(tempNotifInfo->NotifyType == EFI_HII_DATABASE_NOTIFY_REMOVE_PACK){
			if(packFound){
				BOOLEAN newPackOnThisHandle = FALSE;
				NOTIFICATION_INFO *thisNotifInfo = tempNotifInfo->prevNotifInfo;
				while(thisNotifInfo) //Loop to find if there is a new pack notification in the queue with this handle
				{
					if((thisNotifInfo->Handle == tempNotifInfo->Handle) && (EFI_HII_DATABASE_NOTIFY_NEW_PACK == thisNotifInfo->NotifyType)){
						newPackOnThisHandle = TRUE;
						break;
					}
					thisNotifInfo = thisNotifInfo->prevNotifInfo;
				}
				if(newPackOnThisHandle){	//If a new pack notification was found, no need to process any notification on this handle
					RemoveNotifFromQueueByHandleType(BY_HANDLE, tempNotifInfo->Handle, 0, NULL);
					tempNotifInfo = gNotificationQueue->tailNotifInfo;
					continue;
				} else{		//Removed everything except for the remove pack notification on the handle
					RemoveNotifFromQueueByHandleType(BY_HANDLE, tempNotifInfo->Handle, 0, tempNotifInfo);
				}
			} else if(!IsSetupFormBrowserNestedSendFormSupport() || !FormBrowserHandleValid()){/* Notification of Root TSE/Setup may be present. So don'thandle this case inside SendForm.*/
				RemoveNotifFromQueueByHandleType(BY_ENTRY, NULL, 0, tempNotifInfo); //clean up notification queue, except for this handle
			}
		} else if(tempNotifInfo->NotifyType == EFI_HII_DATABASE_NOTIFY_ADD_PACK) {
			if(packFound){
				if((gIFRChangeNotify == TRUE) && (gRemovedHandle != NULL)){
					RemoveNotifFromQueueByHandleType(BY_HANDLE, gRemovedHandle, 0, tempNotifInfo); //If the previous remove pack is already processed, clean up notification queue with this handle
				} else{
					NOTIFICATION_INFO *thisNotifInfo = tempNotifInfo->prevNotifInfo;
					while (thisNotifInfo) //Loop to find if there is a remove pack notification in the queue with this handle
					{
						if((thisNotifInfo->Handle == tempNotifInfo->Handle) && (thisNotifInfo->NotifyType == EFI_HII_DATABASE_NOTIFY_REMOVE_PACK)){
							tempNotifInfo = thisNotifInfo; //Skip the first remove pack encountered on this handle
							break;
						}
						thisNotifInfo = thisNotifInfo->prevNotifInfo;
					}
				}
			} else if(!IsSetupFormBrowserNestedSendFormSupport() || !FormBrowserHandleValid()){/* Notification of Root TSE/Setup may be present. So don't handle this case inside SendForm.*/
				RemoveNotifFromQueueByHandleType(BY_ENTRY, NULL, 0, tempNotifInfo); //clean up notification queue, except for this handle
			}
		}
		if (NULL != tempNotifInfo){		//If the current node is valid
			tempNotifInfo = tempNotifInfo->prevNotifInfo; //Move current notification node pointer to previous notification node
		}
		packFound = FALSE;
	}
	SETUP_DEBUG_UEFI_NOTIFICATION ("\n\n[TSE] After RemoveDuplicateIFRNotification(), NumOfNotification left: %d\n\n", gNotificationQueue->NumOfNotification);
DONE:
	return status;
}

/**
    Processes the IFR notification queue

    @param VOID

    @retval EFI_STATUS status - EFI_ABORTED, By default
        EFI_SUCCESS, if successful
        EFI_ERROR, otherwise
**/
EFI_STATUS ProcessPackNotification (VOID)
{
	EFI_STATUS 		status = EFI_ABORTED;
	EFI_HII_HANDLE 	Handle = (EFI_HII_HANDLE)NULL;
	EFI_HII_DATABASE_NOTIFY_TYPE NotifyType = 0;
	UINT8 			*PackData = (UINT8 *)NULL;
	NOTIFICATION_INFO * SfCurrNotifInfo = (NOTIFICATION_INFO *)NULL;
	
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Entering ProcessPackNotification () \n\n");
	RemoveDuplicateIFRNotification();

	if((gNotificationQueue == NULL) || (gNotificationQueue->headNotifInfo == NULL))
	{
		status = EFI_NOT_FOUND; //There is no notification to process
		goto DONE;
	}

	do
	{
		if ((NULL == gNotificationQueue) || (NULL == gNotificationQueue->headNotifInfo))
		{
			status = EFI_SUCCESS; //All the notifications are processed
			goto DONE;
		}
		else if (IsSetupFormBrowserNestedSendFormSupport() && gSfHandles && gSfHandleCount)
		{
			SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] Pack notification received inside SendForm \n");
			SfCurrNotifInfo = GetNotificationFromCurrentSfHandles();				

			if(SfCurrNotifInfo)
			{
				Handle = SfCurrNotifInfo->Handle;
				NotifyType = SfCurrNotifInfo->NotifyType;
				PackData = SfCurrNotifInfo->PackData;
			}
			else
			{
				SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] Skipping ProcessPackNotification since no notification pending related to Current Context...\n");	
				status = EFI_SUCCESS; //All the notifications related to current SendForm Handles are processed
				goto DONE;
			}
		}
		else
		{
			Handle = gNotificationQueue->headNotifInfo->Handle;
			NotifyType = gNotificationQueue->headNotifInfo->NotifyType;
			PackData = gNotificationQueue->headNotifInfo->PackData;
		}
		SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] gNotificationQueue->NumOfNotification left: %d \n\n", gNotificationQueue->NumOfNotification);	
	
		switch(NotifyType)
		{
			case EFI_HII_DATABASE_NOTIFY_REMOVE_PACK:
				SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] NotifyType: EFI_HII_DATABASE_NOTIFY_REMOVE_PACK \n\n[TSE] Removing IFR Pack \n");
				if(gIFRChangeNotify)
				{
					if(gRemovedHandle!=NULL)
					{
						FixSetupData(); //Will fix the UI and Setup data.
					}
				}
				status = HandleRemoveIFRPack(Handle);
				gIFRChangeNotify = TRUE;
				gRemovedHandle = Handle;
				break;
			case EFI_HII_DATABASE_NOTIFY_NEW_PACK:
				SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] NotifyType: EFI_HII_DATABASE_NOTIFY_NEW_PACK \n\n[TSE] New IFR Pack \n");
				if(gIFRChangeNotify)
				{
					if(gRemovedHandle!=NULL)
					{
						FixSetupData(); //Will fix the UI and Setup data.
					}
				}
				status = HandleNewIFRPack (Handle, PackData);
				break;
			case EFI_HII_DATABASE_NOTIFY_ADD_PACK:
				SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] NotifyType: EFI_HII_DATABASE_NOTIFY_ADD_PACK \n\n[TSE] Add IFR Pack \n");
				status = HandleAddIFRPack(Handle, PackData);
				gIFRChangeNotify = FALSE;
				FixSetupData();
				break;
			default:
				break;
		}

#if TSE_DEBUG_MESSAGES
	    ProcessPackToFile(NotifyType, Handle) ;
#endif
	    if ( IsSetupFormBrowserNestedSendFormSupport() && gSfHandles && gSfHandleCount&&SfCurrNotifInfo)
	    {
	    	RemoveFromQueueByEntry(SfCurrNotifInfo); //Remove the processed notification node from the queue
	    	SfCurrNotifInfo = NULL;
	    }
	    else	
	    	RemoveFromQueueByEntry(gNotificationQueue->headNotifInfo); //Remove the processed notification node from the queue
		SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] NumOfNotification left: %d, headNotifInfo: 0x%x \n\n", gNotificationQueue->NumOfNotification, gNotificationQueue->headNotifInfo);
	}while(1);

DONE:
	gPackUpdatePending = FALSE;
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] Exiting ProcessPackNotificationHook, status: %x \n\n", status);
	return status;
}
/**
    Function to collect the Page Handles for Reconnect Required 

    @param VOID

    @retval VOID
**/
VOID CollectReconnectHandles()
{
   	 EFI_HANDLE PageHandle;
   	 UINTN Count=0;
   	 if(gReconnectPageHandles == NULL)
   	 {
   		 gReconnectPageHandles = (EFI_HANDLE *)EfiLibAllocateZeroPool((gPages->PageCount)*(sizeof(UINT32)));
   		 if(gReconnectPageHandles == NULL)
   			 return ;
   	 }
   	 PageHandle= ((PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[gApp->CurrentPage]))->PageHandle;
   	 if(PageHandle)
   	 {
   		 for(Count=0;gReconnectPageHandles[Count];Count++)
   		 {
   			 if(PageHandle == gReconnectPageHandles[Count])
   			 {
   				 return ;  			 
   			 }
   		 }
   		 gReconnectPageHandles[Count] = PageHandle;
   	 }
}

/**
    Function to Do DisconnectController/ConnectController for Page Handles on Reconnect Required 

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS DoReConnectPageHandles(void)
{
	EFI_STATUS  Status =EFI_UNSUPPORTED;
	EFI_HANDLE DriverHandle;
	UINTN       HandleCount=0;
	//
	// Disconnect  and Connect  the pageinfo handles
	//
	if(!gReconnectPageHandles)
	{
		return EFI_SUCCESS;
	}
	for( HandleCount = 0;gReconnectPageHandles[HandleCount]; HandleCount++ )
	{

		Status=gHiiDatabase->GetPackageListHandle(gHiiDatabase,gReconnectPageHandles[HandleCount], &DriverHandle);
		if(!EFI_ERROR (Status))
		{
			Status= gBS->DisconnectController (DriverHandle, NULL, NULL);
			if(EFI_ERROR (Status))
			{
				continue;
			}
			Status=gBS->ConnectController (DriverHandle, NULL, NULL, TRUE);
		}
	}
	//process pack function
	if((gEnableDrvNotification) && !(gEnableProcessPack))
	{
		gEnableProcessPack = TRUE;
		ProcessPackNotificationHook ();
		gEnableProcessPack = FALSE;
	}
	return EFI_SUCCESS;
}

/**
    Function to get the notification info of current SendForm handles 

    @param VOID

    @retval NOTIFICATION_INFO*
**/
NOTIFICATION_INFO *GetNotificationFromCurrentSfHandles()
{
	UINTN i;
	NOTIFICATION_INFO *CurrNotifInfo = NULL;
	
	if ((NULL == gNotificationQueue) || (NULL == gNotificationQueue->headNotifInfo))
		goto EXIT;
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] Entering GetNotificationFromCurrentSfHandles() \n");
	CurrNotifInfo = gNotificationQueue->headNotifInfo;
	
	if(!IsSetupFormBrowserNestedSendFormSupport())
		goto EXIT;
	
	while(CurrNotifInfo)
	{
		for(i = 0;i < gSfHandleCount ; i++)
		{
			if((EFI_HII_HANDLE)gSfHandles[i] == CurrNotifInfo->Handle)
				goto EXIT;
		}
		CurrNotifInfo = CurrNotifInfo->nextNotifInfo;
	}
	
EXIT:
SETUP_DEBUG_UEFI_NOTIFICATION( "\n[TSE] Exiting GetNotificationFromCurrentSfHandles() with CurrNotifInfo = 0x%x\n",CurrNotifInfo);
return CurrNotifInfo;
	
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
