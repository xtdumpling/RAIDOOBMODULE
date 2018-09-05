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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/CtrlCond.c $
//
// $Author: Premkumara $
//
// $Revision: 13 $
//
// $Date: 2/02/12 2:59a $
//
//*****************************************************************//
/** @file CtrlCond.c

**/
//*************************************************************************

//---------------------------------------------------------------------------
#include "minisetup.h"
#include "CtrlCond.h"
#include "TseUefiHii.h"
//---------------------------------------------------------------------------

extern VOID EvaluateReadWrite(UINT8 *buf,CONTROL_INFO *ControlInfo,EFI_HII_VALUE *);

UINTN _SkipExpression(UINT8 *ControlCondPtr);
extern AMI_POST_MANAGER_PROTOCOL 	*mPostMgr;

/**
    check control flags

    @param ControlAccess 

    @retval UINT8
**/
UINT8 CheckControlAccess(UINT32 ControlAccess)
{
  UINT8 Cond = COND_NONE;

  if(gPasswordType  == AMI_PASSWORD_USER)
  {
    switch(ControlAccess)
    {
    case CONTROL_ACCESS_ADMIN:  // suppress
      Cond = COND_SUPPRESS;
      break;
    case CONTROL_ACCESS_USER: // grayout (read-only)
      Cond = COND_GRAYOUT;
      break;
    default:
      break;
    }
  }

  return Cond;
}

/**

    @param FinalCond UINT8 Cond

    @retval UINT8
**/
UINT8 UpdateFinalCondition(UINT8 FinalCond, UINT8 Cond)
{
  switch(FinalCond)
  {
  case COND_NONE:
    return(Cond);
    break;

  case COND_NOSUBMIT:    
	return(Cond);
	break;
  case COND_SUPPRESS:
  case COND_HIDDEN:
    return(FinalCond);
    break;

  case COND_GRAYOUT:
    if(Cond != COND_NONE)
      return(Cond);
    break;

  case COND_INCONSISTENT:
  case COND_WARN_IF:
    return(Cond);
    break;

  }
  return(FinalCond);
}
extern EFI_STATUS _SetValueFromQuestionId(UINT16 QuestionId, UINT16 PageId, EFI_HII_VALUE *Value);
/**

    @param controlInfo 

    @retval UINT8
**/
UINT8 CheckControlCondition( CONTROL_INFO *controlInfo )
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *tempControlInfo = NULL;
  UINT32 CtrlAccess;
  UINT8 *CondPtr;
  UINT8 *CtrlPtr;
  UINT8 Cond = COND_NONE, FinalCond = COND_NONE;
  UINTN controlInfoLength = 0;
  UINTN offset = 0;
  UINTN	i = 0;
  UINTN ScopeCount = 0;
  EFI_IFR_OP_HEADER *opHeader= (EFI_IFR_OP_HEADER *) NULL;
  EFI_IFR_DEFAULT *Default = (EFI_IFR_DEFAULT *) NULL;

  UINT8 condStack[50];
  UINT8 opcodeStack[50];

  INTN stackIndex = 0;
  INTN inScope = 0;
  EFI_HII_VALUE Value;
  EFI_HII_VALUE Value1;

  BOOLEAN Inside = TRUE;

  CondPtr    = (UINT8 *)controlInfo->ControlConditionalPtr;
  CtrlAccess = controlInfo->ControlFlags.ControlAccess;

  CtrlPtr = (UINT8 *)controlInfo->ControlPtr;
  EfiZeroMem (&Value1, sizeof (EFI_HII_VALUE));
//  EfiZeroMem (&Value, sizeof (EFI_HII_VALUE));
  if(controlInfo->ControlFlags.ControlRWEvaluate)
  {
      do
      {
       opHeader = (EFI_IFR_OP_HEADER*)(CtrlPtr + i);
        switch(opHeader->OpCode)
        {
            case EFI_IFR_READ_OP:
                  EvaluateReadWrite((CtrlPtr + i + opHeader->Length),controlInfo,&Value1);
                  if(Value1.Type != EFI_IFR_TYPE_UNDEFINED){
                     _SetValueFromQuestionId(controlInfo->ControlKey,controlInfo->ControlPageID,&Value1);
                  }
                  break;
            case EFI_IFR_WRITE_OP:
                 EvaluateReadWrite((CtrlPtr + i + opHeader->Length),controlInfo,&Value1);
                break;
            case EFI_IFR_DEFAULT_OP:
                 Default =(EFI_IFR_DEFAULT*)opHeader;
                 if(((EFI_IFR_OP_HEADER*)(CtrlPtr + i + opHeader->Length))->OpCode == EFI_IFR_VALUE_OP){
                    i += opHeader->Length;
                    opHeader = (EFI_IFR_OP_HEADER*)(CtrlPtr + i);
                    EvaluateReadWrite((CtrlPtr + i + opHeader->Length),controlInfo,&Value1);
                 }
                break;
            case EFI_IFR_END_OP:
			    if(ScopeCount)
			    {
				    ScopeCount--;
			    }
			    break;
        }
        if(opHeader->Scope)
	    {
		    ScopeCount++;
        }
        i += opHeader->Length ;
      }while( ScopeCount);
  }
  if ( CondPtr == NULL )
  {  
    FinalCond = CheckControlAccess(CtrlAccess) ;
    if(FinalCond != COND_NONE)
        return FinalCond ;
    goto DONE ;
  }  

  MemSet(&Value, sizeof(EFI_HII_VALUE),0);
  MemSet(&condStack, 50, 0);
  MemSet(&opcodeStack, 50, 0);

  controlInfoLength = sizeof(CONTROL_INFO);
  tempControlInfo = (CONTROL_INFO*)EfiLibAllocateZeroPool(controlInfoLength);
  if (NULL == tempControlInfo) 
    return FinalCond;

  MemCopy(tempControlInfo, controlInfo, controlInfoLength);

  status = EvaluateExpression(tempControlInfo, &Value, &offset);
  if(EFI_ERROR(status))
  {
	offset = _SkipExpression(tempControlInfo->ControlConditionalPtr);
    FinalCond = COND_NONE;
	Value.Value.b = FALSE;
  }
  if(Value.Value.b)
  {
    if((((EFI_IFR_OP_HEADER*)CondPtr)->OpCode == EFI_IFR_SUPPRESS_IF_OP) ||
       (((EFI_IFR_OP_HEADER*)CondPtr)->OpCode == EFI_IFR_DISABLE_IF_OP))
    {
      FinalCond = COND_SUPPRESS;
      Inside = FALSE;
    }
    if(((EFI_IFR_OP_HEADER*)CondPtr)->OpCode == EFI_IFR_INCONSISTENT_IF_OP)
    {
      FinalCond = COND_INCONSISTENT;
      Inside = FALSE;
    }
		if(((EFI_IFR_OP_HEADER*)CondPtr)->OpCode == EFI_IFR_NO_SUBMIT_IF_OP)
		{
			FinalCond = COND_NOSUBMIT;
			Inside = FALSE;
		}
		if(((EFI_IFR_OP_HEADER*)CondPtr)->OpCode == TSE_EFI_IFR_WARNING_IF_OP)
		{
			FinalCond = COND_WARN_IF;
			Inside = FALSE;
		}
		
  }

  while( Inside )
  {
    switch(*(CondPtr))
    {
    case EFI_IFR_DISABLE_IF_OP:
    case EFI_IFR_SUPPRESS_IF_OP:
      Cond = COND_SUPPRESS;
      break;

    case EFI_IFR_GRAY_OUT_IF_OP:
      Cond = COND_GRAYOUT;
      break;

    case EFI_IFR_INCONSISTENT_IF_OP:
      Cond = COND_INCONSISTENT;
	  Inside = FALSE;
      break;
	case EFI_IFR_NO_SUBMIT_IF_OP:
	  Cond = COND_NOSUBMIT;
	  Inside = FALSE;
      break;
	case TSE_EFI_IFR_WARNING_IF_OP:
	  Cond = COND_WARN_IF;
	  Inside = FALSE;
	  break;
    case EFI_IFR_END_OP:
      if(stackIndex)
      {
        condStack[--stackIndex] = 0;
        opcodeStack[stackIndex] = 0;
      }
      offset = ((EFI_IFR_OP_HEADER*)CondPtr)->Length;
      break;
    case EFI_IFR_FORM_OP:
    case EFI_IFR_FORM_MAP_OP:
	offset += ((EFI_IFR_OP_HEADER*)CondPtr)->Length;
	break;
    default:
      if(CondPtr == (UINT8*)controlInfo->ControlPtr)
      {
        Inside = FALSE;
      }else if(((EFI_IFR_OP_HEADER*)CondPtr)->OpCode != EFI_IFR_END_OP)
      {
        //Skip this Control
        inScope = 0;
        do
        {
          if(((EFI_IFR_OP_HEADER*)((UINT8*)CondPtr + offset))->Scope)
            inScope++;
          offset += ((EFI_IFR_OP_HEADER*)((UINT8*)CondPtr + offset))->Length;
          if(((EFI_IFR_OP_HEADER*)((UINT8*)CondPtr + offset))->OpCode == EFI_IFR_END_OP)
            inScope--;
        }while(inScope > 0);
      }
      break;
    }
    if(((EFI_IFR_OP_HEADER*)CondPtr)->Scope)
    {
      if(Value.Value.b)
      {
        condStack[stackIndex] = Cond;
      }else
      {
        condStack[stackIndex] = COND_NONE;
      }
      opcodeStack[stackIndex++] = ((EFI_IFR_OP_HEADER*)CondPtr)->OpCode;
    }

    tempControlInfo->ControlConditionalPtr = (VOID *)((UINT8 *)tempControlInfo->ControlConditionalPtr + offset);
    CondPtr = (UINT8*)tempControlInfo->ControlConditionalPtr;
    offset = 0;
    Value.Value.b = 0;
    status = EvaluateExpression(tempControlInfo, &Value, &offset);
    if(EFI_ERROR(status))
    {
	  offset = _SkipExpression(tempControlInfo->ControlConditionalPtr);
      FinalCond = COND_NONE;
	  Value.Value.b = FALSE;
    }
  }

  while(stackIndex--)
  {
    Cond = condStack[stackIndex];
    FinalCond = UpdateFinalCondition(FinalCond,Cond);
  }

  // if CtrlAccess is CONTROL_ACCESS_DEFAULT then do not override VFR conditions
  if(CONTROL_ACCESS_DEFAULT != CtrlAccess)
  {
    FinalCond = CheckControlAccess(CtrlAccess);
  }

  if(tempControlInfo)
  {
    MemFreePointer((void**)&tempControlInfo);
  }

DONE:

  if( (FinalCond == COND_NONE) && 
      (controlInfo->ControlFlags.ControlReadOnly == 1) && 
      IsReadOnlyGrayout() )
    {
        FinalCond = COND_GRAYOUT ;
    } 
	
	FinalCond = OEMCheckControlCondition(FinalCond,controlInfo);// provide the Support to override the control Condition checking() generically. And provide customization to supress the controls with Security GUIDed opcode.
  return FinalCond;
}

/**
    checks Inconsistence

    @param PAGE_INFO

    @retval BOOLEAN
**/
BOOLEAN CheckInconsistence( PAGE_INFO *pPageInfo )
{
	CONTROL_INFO *pControlInfo;
	UINTN i;
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_EVENT Event = NULL;
	CHAR16 *Title = NULL,*Message = NULL;

	Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
	if((Status == EFI_SUCCESS) && (mPostMgr != NULL))
	{
		//Find out if there is inconsistent value in any of the controls
		for(i=0; i < pPageInfo->PageControls.ControlCount; i++)
		{
			pControlInfo = (CONTROL_INFO*)((UINT8 *)(gControlInfo) + pPageInfo->PageControls.ControlList[i]);
			//Check if there is a CONTROL_TYPE_MSGBOX in this page
			if(pControlInfo->ControlType == CONTROL_TYPE_MSGBOX)
			{
			//check for warning condition
				if((CheckControlCondition(pControlInfo) == COND_WARN_IF) && (pControlInfo->ControlFlags.ControlEvaluateDefault == 0))
				{
					pControlInfo->ControlFlags.ControlEvaluateDefault = 1;
	          		Title = HiiGetString( 	gHiiHandle, STRING_TOKEN(STR_WARNING));
	          		Message = HiiGetString( pControlInfo->ControlHandle, ((TSE_EFI_IFR_WARNING_IF*)pControlInfo->ControlConditionalPtr)->Warning);
					Status = mPostMgr->DisplayInfoBox(
							Title,
							Message,
							((TSE_EFI_IFR_WARNING_IF*)pControlInfo->ControlConditionalPtr)->TimeOut, 
							&Event
							);
	          		MemFreePointer( (VOID **)&Title );
	          		MemFreePointer( (VOID **)&Message );
				}
				else if( (CheckControlCondition(pControlInfo) != COND_WARN_IF) && (pControlInfo->ControlFlags.ControlEvaluateDefault == 1))
					pControlInfo->ControlFlags.ControlEvaluateDefault = 0;
				if(CheckControlCondition(pControlInfo) == COND_INCONSISTENT)
				{
	          		Title = HiiGetString( gHiiHandle, STRING_TOKEN(STR_INCONSISTENT_MSG_TITLE));
	          		Message = HiiGetString( pControlInfo->ControlHandle, ((EFI_IFR_INCONSISTENT_IF*)pControlInfo->ControlConditionalPtr)->Error);
	          		mPostMgr->DisplayMsgBox( 
	          				Title, 
	          				Message , 
	          				MSGBOX_TYPE_OK,
	          				NULL
	          			);
	          		MemFreePointer( (VOID **)&Title );
	          		MemFreePointer( (VOID **)&Message );
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/**
    checks NoSubmitIf

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN CheckNoSubmitIf( VOID)
{
	UINT32 index = 0;
    EFI_STATUS Status = EFI_SUCCESS;
	// Get the Forms
	for ( index = 0; index < gPages->PageCount; index++ )
	{
		PAGE_INFO *pageInfo = (PAGE_INFO*)((UINT8 *)gPages + gPages->PageList[index]);
		UINTN control = 0;

		if(pageInfo->PageHandle == NULL)
		{
			continue;
		}	
			// Get the Controls in each Page
		for(control = 0; control < pageInfo->PageControls.ControlCount; control++)
		{
			CONTROL_INFO *pControlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);			
		
			if(pControlInfo->ControlType == NO_SUBMIT_IF)			
			{
                if(CheckControlCondition(pControlInfo) == COND_NOSUBMIT){
                    //return DrawMessageBox(pControlInfo, STRING_TOKEN(STR_NOSUBMITIF_MSG_TITLE), ((EFI_IFR_NO_SUBMIT_IF*)pControlInfo->ControlConditionalPtr)->Error);							
                    Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
                    if((Status == EFI_SUCCESS) && (mPostMgr != NULL))
                    {
                   		CHAR16 *Title = NULL,*Message = NULL;
                   		Title = HiiGetString( gHiiHandle, STRING_TOKEN(STR_NOSUBMITIF_MSG_TITLE));
                   		Message = HiiGetString( pControlInfo->ControlHandle, ((EFI_IFR_NO_SUBMIT_IF*)pControlInfo->ControlConditionalPtr)->Error);
                   		mPostMgr->DisplayMsgBox( 
                   				Title, 
                   				Message , 
                   				MSGBOX_TYPE_OK,
                   				NULL
                   			);
                   		MemFreePointer( (VOID **)&Title );
                   		MemFreePointer( (VOID **)&Message );                   	
                        return TRUE;
                    }
                }
			}
		}		
	}
	return FALSE;
}

/**

    @param ParentCtrlInfo UINT8* CtrlCondPtr

    @retval UINT8
**/
UINT8 CheckOneOfOptionSuppressIf(CONTROL_INFO *ParentCtrlInfo, UINT8* CtrlCondPtr, UINT8* CtrlPtr)
{
  CONTROL_INFO *newCtrlInfo = NULL;
  UINT8 condition = COND_NONE;

  newCtrlInfo = (CONTROL_INFO*)EfiLibAllocateZeroPool(sizeof(CONTROL_INFO));
  if(newCtrlInfo == NULL)
  {
    goto DONE;
  }

  newCtrlInfo->ControlHandle = ParentCtrlInfo->ControlHandle;
  newCtrlInfo->ControlPageID = ParentCtrlInfo->ControlPageID;
  newCtrlInfo->ControlKey = ParentCtrlInfo->ControlKey;
  newCtrlInfo->ControlConditionalPtr = (VOID *)CtrlCondPtr;
  newCtrlInfo->ControlPtr = (VOID *)CtrlPtr;
  condition = CheckControlCondition(newCtrlInfo);
  MemFreePointer((void**)&newCtrlInfo);

DONE:

  return condition;
}

/**

    @param CtrlInfo 

    @retval UINT64
**/
UINT16 EvaluateControlDefault(CONTROL_INFO *CtrlInfo, UINT64 *Defaults)
{
  CONTROL_INFO *newCtrlInfo = NULL;
  EFI_IFR_OP_HEADER *header = (EFI_IFR_OP_HEADER*)CtrlInfo->ControlPtr;
  EFI_HII_VALUE value;
  UINT16 size = 0;
  UINT32 scope = 0;
  UINTN i = 0;
  UINTN offset = 0;
  EFI_STATUS Status = EFI_SUCCESS;

  newCtrlInfo = (CONTROL_INFO*)EfiLibAllocateZeroPool(sizeof(CONTROL_INFO));
  if(newCtrlInfo == NULL)
  {
    goto DONE;
  }

  newCtrlInfo->ControlHandle = CtrlInfo->ControlHandle;
  newCtrlInfo->ControlPageID = CtrlInfo->ControlPageID;
  newCtrlInfo->ControlKey = CtrlInfo->ControlKey;

  while(header->OpCode != EFI_IFR_DEFAULT_OP)
  {
    i += header->Length;
    header = (EFI_IFR_OP_HEADER*)((UINT8*)header + i);
  }

  newCtrlInfo->ControlConditionalPtr = (VOID *)((UINT8 *)header + header->Length);
  // Set End of Default Scope as ControlPtr
  do
  {
    switch(header->OpCode)
    {
    case EFI_IFR_END_OP:
//      scope = scope? --scope : 0;
	    if ( scope)
	    {
		    --scope;
	    }
	    else
	    {
		    scope = 0;
	    }
 
      break;
    default:
      break;
    }
//    scope = header->Scope? ++scope : scope;
      if ( header->Scope)  ++scope ;
    header = (EFI_IFR_OP_HEADER*)((UINT8*)header + header->Length);
  }while(scope > 0);

  newCtrlInfo->ControlPtr = (VOID *)header;
  Status = EvaluateExpression(newCtrlInfo, &value, &offset);
  MemFreePointer((void**)&newCtrlInfo);
  if (EFI_ERROR (Status))
  {
	  goto DONE;
  }
  
  size = (UINT16)GetControlDataLength(CtrlInfo);
  
  //For String controls size will be length of string, so that should not be set to default value
  if(CONTROL_TYPE_POPUP_STRING == CtrlInfo->ControlType || CONTROL_TYPE_PASSWORD == CtrlInfo->ControlType)
	  MemCpy(Defaults, &(value.Value), sizeof(UINT16));
  else
	  MemCpy(Defaults, &(value.Value), size);
  
DONE:
  return size;
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
