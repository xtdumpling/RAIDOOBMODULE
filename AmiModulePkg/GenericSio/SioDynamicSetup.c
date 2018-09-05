//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//****************************************************************************
// $Header: /Alaska/SOURCE/Modules/CSM/Generic/csmsetup.c 4     1/12/10 11:52a Olegi $
//
// $Revision: 4 $
//
// $Date: 1/12/10 11:52a $
//
//****************************************************************************

//**********************************************************************
/** @file SioDynamicSetup.c
    SIO Dynamic Setup Functions Implementation

**/
//**********************************************************************

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Token.h>
#include <Efi.h>

#include <SioDynamicSetupStrDefs.h>
#include <AmiDxeLib.h>
#include <AcpiRes.h>
#include <AmiGenericSio.h>
#include "SioDynamicSetupPrivate.h"

///
/// Protocols Used
///
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>
#include <Protocol/SuperIo.h>

///
/// Library used
///
#include <Library/HiiLib.h>
#include <Library/AmiSdlLib.h>
#include <Library/AmiHiiUpdateLib.h>
#include <Library/AmiSioDxeLib.h>
#include <Library/BaseLib.h>

///
/// Global Variable Definitions
///
SIO_LD_FORM_DATA    gLdConfigForm = {0};

SIO_IFR_INFO        gSioIfrInfo;

EFI_HII_HANDLE      gSioHiiHandle = NULL;

BOOLEAN             gDataSaved=FALSE;

EFI_STATUS SioBrowserCallback(
    CONST EFI_HII_CONFIG_ACCESS_PROTOCOL*, EFI_BROWSER_ACTION,EFI_QUESTION_ID,UINT8,EFI_IFR_TYPE_VALUE*, EFI_BROWSER_ACTION_REQUEST*
	);

EFI_STATUS SioRoutConfigCallback(
   IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
   IN CONST  EFI_STRING                      Configuration,
    OUT       EFI_STRING                      *Progress        
   );
    
EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = { NULL, SioRoutConfigCallback, SioBrowserCallback };
//EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = { NULL, NULL, SioBrowserCallback };
EFI_GUID SioFormsetGuid = SIO_FORM_SET_GUID;

CALLBACK_INFO SetupCallBack[] =
{
    // Last field in every structure will be filled by the Setup
    { &SioFormsetGuid, &CallBack, SIO_FORM_SET_CLASS, 0, 0},
};

//forward declaration.
EFI_STATUS UpdateLdConfigForm(IN EFI_QUESTION_ID KeyValue);


EFI_GUID gEfiIfrTianoGuid = EFI_IFR_TIANO_GUID;

//Flag that tells we have made PS2 Controller GotoOpcode already
//this for PS2KB and PS2MS residing in single LD we can't Disable one keeping Enabled another.
//So Even we have setup settings for both of them we should create only one Goto Opcode. 
BOOLEAN	gPs2Ctr=FALSE;

AMI_BOARD_INIT_PROTOCOL		*gSioInitProtocol=NULL;


///
/// Functions Implementation
///
/**
    This function generates SIO LD device title string and Help string
	
    @param LdGotoData Pointer to the private Goto Data Structure.

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR if something wrong happends

**/
EFI_STATUS EFIAPI CreateGotoString(LD_SETUP_GOTO_DATA *LdGotoData){
    EFI_STRING 			s=NULL, ldnamestr=NULL, configstr=NULL, statusstr=NULL;
//--------------------------------
    //
    // Create goto title string
    //
    s = MallocZ(200);
    if(s==NULL) return EFI_OUT_OF_RESOURCES;
    
    switch (LdGotoData->SdlInfo->Type){
		case dsFDC: 
			ldnamestr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_FLOPPY),NULL);
			break;
		case dsPS2K:
			ldnamestr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_PS2_KEYBOARD),NULL);
			break;
		case dsPS2M:
			ldnamestr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_PS2_MOUSE),NULL);
			break;
		case dsPS2CK:	
		case dsPS2CM:
			if(gPs2Ctr==FALSE){
				ldnamestr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_PS2_CONTROLLER),NULL);
				gPs2Ctr=TRUE;
			} else return EFI_NOT_FOUND;	
			break;
		case dsUART: 
			ldnamestr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_SERIAL_PORT),NULL);
			break;
		case dsLPT: 
			ldnamestr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_PARALLEL_PORT),NULL);
			break;
		case dsCIR:
			ldnamestr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_CIR_CONTROLLER),NULL);
			break;			
		default: return EFI_NOT_FOUND;	
    }
 
    if(ldnamestr==NULL) return EFI_NOT_FOUND;
    
    configstr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_CONFIGURATION),NULL);

    if(LdGotoData->SioLd!=NULL){
    	statusstr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_STATUS_ON),NULL);
    } else {
    	statusstr=HiiGetString(gSioHiiHandle,STRING_TOKEN(STR_STATUS_OFF),NULL);
    }

    if(LdGotoData->LdNumber > 0){
#if (SIO_SETUP_USE_UID_AS_NUMBERS == 1)
    	Swprintf(s, L"%s %s %d", statusstr, ldnamestr, LdGotoData->SdlInfo->Uid );
#else 
    	Swprintf(s, L"%s %s %d", statusstr, ldnamestr, LdGotoData->LdNumber);
#endif    	
    } else {
    	//Don't print UID for device with UID 0 - does not looks good. 
    	Swprintf(s, L"%s %s", statusstr, ldnamestr );
    }
    
    /// StringId==0 makes function return a new one, SupportedLanguages==NULL sets it for current language.
    LdGotoData->GotoStringId = HiiSetString(gSioHiiHandle, 0, s, NULL );

    LdGotoData->GotoHelpStringId=STRING_TOKEN(STR_SIO_GOTO_HELP);

    if(LdGotoData->LdNumber > 0){
#if (SIO_SETUP_USE_UID_AS_NUMBERS == 1)
    	Swprintf(s, L"%s %d %s", ldnamestr, LdGotoData->SdlInfo->Uid, configstr);
#else 
    	Swprintf(s, L"%s %d %s", ldnamestr, LdGotoData->LdNumber, configstr);
#endif    	
    } else {
    	//Don't print UID for device with UID 0 - does not looks good. 
    	Swprintf(s, L"%s %s", ldnamestr, configstr);
    }
    
    LdGotoData->LdFormTitleStringId=HiiSetString(gSioHiiHandle, LdGotoData->LdFormTitleStringId, s, NULL );

    //House keeping
    if(ldnamestr!=NULL) pBS->FreePool(ldnamestr);
    if(configstr!=NULL) pBS->FreePool(configstr);
    if(statusstr!=NULL) pBS->FreePool(statusstr);
    pBS->FreePool(s);

    return EFI_SUCCESS;
}

/**
    Returns associated with passed SioLd Volatile or Non Volatile varstoreid.

    @param SioIdx Index of processed SIO device
    @param LdIdx  Indes of processed LD 
    @param NoneVolatile Indicates variable is Volatile or not 
    
    @retval EFI_VARSTORE_ID 
    
**/
EFI_VARSTORE_ID EFIAPI GetVarStoreID(UINTN SioIdx, UINTN LdIdx, BOOLEAN NoneVolatile){
    SIO_VAR_ID  varid;
//-----------------------   
    varid.VidField.SioIndex=(UINT8)SioIdx;
    varid.VidField.LdIndex=(UINT8)LdIdx;
    varid.VidField.Always1=1;
    varid.VidField.IsNoneVolatile=NoneVolatile;
    varid.VidField.Reserved=0;
    return (EFI_VARSTORE_ID)(varid.VAR_ID);
}


VOID EFIAPI GetSetupNvData(EFI_QUESTION_ID KeyValue){
    UINTN  i=(UINTN)(KeyValue & (~SIO_GOTO_ID_BASE));
    LD_SETUP_GOTO_DATA      *gotodata;
    EFI_GUID    ssg = SIO_VARSTORE_GUID;
    UINTN       vsz;
    CHAR16      s[40];
    EFI_STATUS  Status = EFI_SUCCESS;
//--------------------------------    
    
    gotodata=gSioIfrInfo.LdSetupData[i];   
        
    pBS->SetMem(&s[0], sizeof(s),0);

    Swprintf(s, LD_NV_VAR_NAME_FORMAT, gotodata->SioIdx,  gotodata->LdIdx);
    
    gotodata->NvVarName=MallocZ(Wcslen(s)*sizeof(CHAR16)+sizeof(CHAR16));
    Wcscpy(gotodata->NvVarName, s); 
    vsz=sizeof(SIO_DEV_NV_DATA);

    Status = pRS->GetVariable(gotodata->NvVarName, &ssg, NULL, &vsz, &gotodata->SetupNvData);

    
    SIO_TRACE((TRACE_SIO,"SioSetup: Get NV_VAR('%S' = [%X, %X, %X]);\n", gotodata->NvVarName,
            gotodata->SetupNvData.DevEnable, gotodata->SetupNvData.DevPrsId, gotodata->SetupNvData.DevMode));
}



/**
    This will create SIO LDs Goto items information

    @param SioIfrInfo Pointer to the SIO IFR private data structure
	
    @retval VOID

**/
VOID EFIAPI CreateLdGotoItems(SIO_IFR_INFO *SioIfrInfo)
{
    UINTN 			i;
    EFI_STATUS 		Status;
//----------------------------------------------

    for (i=0; i<SioIfrInfo->LdCount; i++) {
    	LD_SETUP_GOTO_DATA	*ldgoto;
    //------------------------------
    	ldgoto=SioIfrInfo->LdSetupData[i];

        Status = CreateGotoString(ldgoto);
        if (EFI_ERROR(Status)){
        	if (Status==EFI_NOT_FOUND) continue;

        	ASSERT_EFI_ERROR(Status);
        	return;
        }
      
        //fill QuestionId fields
        ldgoto->GotoQid=(EFI_QUESTION_ID)(SIO_GOTO_ID_BASE+(UINT16)i);
        ldgoto->EnableQid=(EFI_QUESTION_ID)(SIO_LD_ENABLE_QID_BASE+(UINT16)i);
        ldgoto->ResSelQid=(EFI_QUESTION_ID)(SIO_LD_RESSEL_QID_BASE+(UINT16)i);
        if(ldgoto->ModeItemCount>0){
            ldgoto->ModeQid=(EFI_QUESTION_ID)(SIO_LD_MODE_QID_BASE+(UINT16)i);
        }
        
        //VarStoreId fields
        ldgoto->NvVarStoreId=GetVarStoreID(ldgoto->SioIdx, ldgoto->LdIdx,TRUE);
        ldgoto->VlVarStoreId=GetVarStoreID(ldgoto->SioIdx, ldgoto->LdIdx,FALSE);
        
        //VarStore Data will be updated in EFI_BROWSER_ACTION_RETRIEVE to support "Discard Changes" feature. 
        
        // Add goto control
        HiiCreateGotoOpCode (
            SioIfrInfo->StartOpCodeHandle,
            SIO_LD_FORM_ID,
            ldgoto->GotoStringId,
            ldgoto->GotoHelpStringId,
            EFI_IFR_FLAG_CALLBACK,
            ldgoto->GotoQid
        );
    }

    HiiUpdateForm (
      gSioHiiHandle,
      &SioFormsetGuid,
      SIO_MAIN_FORM_ID,
      SioIfrInfo->StartOpCodeHandle,
      SioIfrInfo->EndOpCodeHandle
    );
/*    
    //Publish dynamic pages....
    for(i=0;i<SioIfrInfo->LdCount; i++){
        
        if( (((SioIfrInfo->LdSetupData[i])->GotoQid)&SIO_GOTO_ID_BASE)==0) continue;
        
        Status = UpdateLdConfigForm((SioIfrInfo->LdSetupData[i])->GotoQid);
        ASSERT_EFI_ERROR(Status);
        
    }
*/    
}


/**
    This will update LDs current configuration strings ID information

    @param GotoData Pointer to the Corresponded LD goto data structure

    @retval VOID

**/

VOID EFIAPI UpdateCurrentResourcesStrId(LD_SETUP_GOTO_DATA *GotoData){
	EFI_STRING 	s=NULL, configs=NULL;
	CHAR16		strbuf[61];
	CHAR16	   	*io1s=&strbuf[0];
	CHAR16	   	*io2s=&strbuf[10];
	CHAR16		*irq1s=&strbuf[20];
	CHAR16		*irq2s=&strbuf[30];
	CHAR16		*dma1s=&strbuf[40]; 
	CHAR16		*dma2s=&strbuf[50]; 
//----------------------	
	s=MallocZ(160);
	if (s==NULL) ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
	
	configs=HiiGetString (gSioHiiHandle, STRING_TOKEN(STR_CURRENT_SETTINGS),NULL);
	
	if(GotoData->SioLd !=NULL) {
		pBS->SetMem(&strbuf[0],sizeof(strbuf),0);
			
		if(GotoData->SioLd->VlData.DevBase1!=0) Swprintf(io1s, L"IO=%Xh; " ,GotoData->SioLd->VlData.DevBase1);
		if(GotoData->SioLd->VlData.DevBase2!=0) Swprintf(io2s, L"IO=%Xh; " ,GotoData->SioLd->VlData.DevBase2);
		
		if(GotoData->SioLd->VlData.DevIrq1!=0) Swprintf(irq1s, L"IRQ=%d; " ,GotoData->SioLd->VlData.DevIrq1);
		if(GotoData->SioLd->VlData.DevIrq2!=0) Swprintf(irq2s, L"IRQ=%d; " ,GotoData->SioLd->VlData.DevIrq2);
		
		if(GotoData->SioLd->VlData.DevDma1!=0) Swprintf(io1s, L"DMA=%d; "  ,GotoData->SioLd->VlData.DevDma1);
		if(GotoData->SioLd->VlData.DevDma2!=0) Swprintf(io2s, L"DMA=%d; "  ,GotoData->SioLd->VlData.DevDma2);
		
		Swprintf(s, L"%s     %s%s%s%s%s%s", configs,io1s,io2s,irq1s,irq2s,dma1s,dma2s);
	} else {
		EFI_STRING	resets=HiiGetString (gSioHiiHandle, STRING_TOKEN(STR_RESET_REQUIRED),NULL);
	//-----------------------	
		Swprintf(s, L"%s     %s",configs,resets );
		if(resets!=NULL) pBS->FreePool(resets);
		
	}
	
	HiiSetString(gSioHiiHandle, STRING_TOKEN(STR_CURRENT_SETTINGS_TEXT2), s, NULL);

	
	if(configs!=NULL)pBS->FreePool(configs);
	pBS->FreePool(s);

	return;
}

/**
    This will update LDs current configuration strings ID information

    @param KeyValue Key Value of the Question ID to get defaults for.

    @retval VOID

**/

VOID EFIAPI EFIAPI LoadLdDefaults(EFI_QUESTION_ID KeyValue){
    UINTN  i=(UINTN)(KeyValue & (~SIO_GOTO_ID_BASE));
    LD_SETUP_GOTO_DATA      *gotodata;
 //-------------------------------   
    //Using KeyValue - locate Goto Form Data...
    gotodata=gSioIfrInfo.LdSetupData[i];   

    SIO_TRACE((TRACE_SIO,"SioSetup: Loading Defaults for NV_SIO%X_LD%X;\n", gotodata->SioIdx, gotodata->LdIdx));
    
    gotodata->SetupNvData.DevEnable = 1;
    gotodata->SetupNvData.DevPrsId = 0;
    gotodata->SetupNvData.DevMode = 0;
}

EFI_STATUS EFIAPI UpdateLdVarBuffer(IN EFI_QUESTION_ID KeyValue, UINT8 Type, EFI_IFR_TYPE_VALUE *Value )
{
    UINTN  i=(UINTN)(KeyValue & (~SIO_LD_MODE_QID_BASE));
    LD_SETUP_GOTO_DATA  *gotodata;
 //-------------------------------   
    //Using KeyValue - locate Goto Form Data...
    gotodata=gSioIfrInfo.LdSetupData[i];   

    if( (KeyValue & SIO_LD_MODE_QID_BASE) == SIO_LD_ENABLE_QID_BASE){
        if(Type!=EFI_IFR_TYPE_BOOLEAN) return EFI_INVALID_PARAMETER;
        if(gotodata->SetupNvData.DevEnable != Value->b){
            gotodata->SetupNvData.DevEnable=Value->b;
            gDataSaved=FALSE;
        }
        return EFI_SUCCESS;
    }
    
    if( (KeyValue & SIO_LD_MODE_QID_BASE) == SIO_LD_RESSEL_QID_BASE){
        if(Type!=EFI_IFR_TYPE_NUM_SIZE_8) return EFI_INVALID_PARAMETER;
        if(gotodata->SetupNvData.DevPrsId!=Value->u8){
            gotodata->SetupNvData.DevPrsId=Value->u8;
            gDataSaved=FALSE;
        }
        return EFI_SUCCESS;
    }
    
    if( (KeyValue & SIO_LD_MODE_QID_BASE) == SIO_LD_MODE_QID_BASE){
        if(Type!=EFI_IFR_TYPE_NUM_SIZE_8) return EFI_INVALID_PARAMETER;
        if(gotodata->SetupNvData.DevMode!=Value->u8){
            gotodata->SetupNvData.DevMode=Value->u8;
            gDataSaved=FALSE;
        }
        return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}

/**
    Updates Logical Device Configuration Form with data applicable for 
    selected Logical Device device

    @param KeyValue A Unique Setup Question Key Value

    @retval EFI_STRING if everything good
    @retval EFI_ERROR if something wrong happends

**/

EFI_STATUS EFIAPI UpdateLdConfigForm(
    IN EFI_QUESTION_ID KeyValue
)
{
    UINTN  i=(UINTN)(KeyValue & (~SIO_GOTO_ID_BASE));
    LD_SETUP_GOTO_DATA	*gotodata;
    EFI_STRING			s=NULL;
    UINTN               vs=sizeof(SIO_DEV_NV_DATA);
    EFI_GUID            ssg = SIO_VARSTORE_GUID;
//--------------------------------------------
    //Using KeyValue - locate Goto Form Data...
    gotodata=gSioIfrInfo.LdSetupData[i];   

    //Update Browser Data with Local Cache...
    HiiLibSetBrowserData(vs, &gotodata->SetupNvData, &ssg, gotodata->NvVarName);
//EFI_DEADLOOP();    
    // Create new OpCode Handle
    if (gLdConfigForm.StartOpCodeHandle != NULL)
    {
        HiiFreeOpCodeHandle (gLdConfigForm.StartOpCodeHandle);
        HiiFreeOpCodeHandle (gLdConfigForm.EndOpCodeHandle);
        HiiFreeOpCodeHandle (gLdConfigForm.PrsOptionsOpCodeHandle);
        HiiFreeOpCodeHandle (gLdConfigForm.ModeOptionsOpCodeHandle);
    }
    
    //Update Caption String...
    s=HiiGetString(gSioHiiHandle, gotodata->LdFormTitleStringId, NULL);
    HiiSetString(gSioHiiHandle, STRING_TOKEN(STR_SIO_LD_FORM_TITLE),s, NULL);
    if(s!=NULL)pBS->FreePool(s);
    
    gLdConfigForm.StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    gLdConfigForm.EndOpCodeHandle = HiiAllocateOpCodeHandle ();
    gLdConfigForm.PrsOptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
    gLdConfigForm.ModeOptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
    
    // Create Hii Extended Label OpCode as the start and end opcode
    gLdConfigForm.StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
    		gLdConfigForm.StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    gLdConfigForm.StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    gLdConfigForm.EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
    		gLdConfigForm.EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    gLdConfigForm.EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    gLdConfigForm.StartLabel->Number = SIO_LD_LABEL_START;
    gLdConfigForm.EndLabel->Number = SIO_LD_LABEL_END;

    //1. Create Enable/Disable CheckBox item
    HiiCreateCheckBoxOpCode (
        gLdConfigForm.StartOpCodeHandle,
        gotodata->EnableQid,
        gotodata->NvVarStoreId,
        EFI_FIELD_OFFSET(SIO_DEV_NV_DATA, DevEnable),
        STRING_TOKEN(STR_LD_ENABLE_PROMPT),
        STRING_TOKEN(STR_LD_ENABLE_HELP),
        (EFI_IFR_FLAG_RESET_REQUIRED | EFI_IFR_FLAG_CALLBACK),
        1,
        NULL
    );

    UpdateCurrentResourcesStrId(gotodata);
    
    //SEPARATOR
    HiiCreateSubTitleOpCode(gLdConfigForm.StartOpCodeHandle,
    		STRING_TOKEN(STR_EMPTY),
    		STRING_TOKEN(STR_EMPTY),
            0,0);
    
    //Now create suppress if OpCode and Condition if TRUE Suppress; FALSE Gray out. 
    AmiHiiCreateSuppresGrayIdVal(gLdConfigForm.StartOpCodeHandle, gotodata->EnableQid, 0,TRUE);	

    //Device Settings: 
    HiiCreateSubTitleOpCode(gLdConfigForm.StartOpCodeHandle,
            STRING_TOKEN(STR_LD_CONFIG_SUB),
            STRING_TOKEN(STR_EMPTY),
            0,0);


    //Current Settings Sub....
    HiiCreateSubTitleOpCode(gLdConfigForm.StartOpCodeHandle,
    		STRING_TOKEN(STR_CURRENT_SETTINGS_TEXT2),
    		STRING_TOKEN(STR_EMPTY),
            0,0);
    
	
    //SEPARATOR
    HiiCreateSubTitleOpCode(gLdConfigForm.StartOpCodeHandle,
    		STRING_TOKEN(STR_EMPTY),
    		STRING_TOKEN(STR_EMPTY),
            0,0);

   	//AUTO option 
   	HiiCreateOneOfOptionOpCode (
			gLdConfigForm.PrsOptionsOpCodeHandle,
			STRING_TOKEN(STR_AUTO_RES),
			(EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG),
            EFI_IFR_NUMERIC_SIZE_1,
            0
            );
    	
    //Will Display possible resource settings based on _PRS object...
    if(gotodata->SioLd!=NULL){

    	for(i=0; i<gotodata->PrsItemCount; i++){
    		//Ad a set of One of items
    		if(gotodata->PrsStrId[i]!=0){
    			// OneOf of 
    			HiiCreateOneOfOptionOpCode (
    					gLdConfigForm.PrsOptionsOpCodeHandle,
    					gotodata->PrsStrId[i],
    					0,
    		            EFI_IFR_NUMERIC_SIZE_1,
    		            i+1
    		            );

    		}
    	}//for
    }

    //Now create OneOf OpCode...
    HiiCreateOneOfOpCode (
    	gLdConfigForm.StartOpCodeHandle,			//*OpCodeHandle
    	gotodata->ResSelQid,						//QuestionId
        gotodata->NvVarStoreId,						//VarStore ID
        EFI_FIELD_OFFSET(SIO_DEV_NV_DATA, DevPrsId),//Offset
        STRING_TOKEN(STR_POSSIBLE_SETTINGS),		//Prompt
        STRING_TOKEN(STR_POSSIBLE_SETTINGS_HELP),	//Help 
        (EFI_IFR_FLAG_RESET_REQUIRED | EFI_IFR_FLAG_CALLBACK),//QuestionFlags
        EFI_IFR_NUMERIC_SIZE_1,						//OneOfFlags
    	gLdConfigForm.PrsOptionsOpCodeHandle,		//OptionsOpCodeHandle
    	NULL             							//*DefaultsOpCodeHandle  OPTIONAL
    );
    	
    //If Logical Device Has Modes .... 
    if((gotodata->SioLd!=NULL) && (gotodata->ModeItemCount>0)){ 
    		
   		//Here put Mode Selector
       	for(i=0; i<gotodata->ModeItemCount; i++){
			UINT8	flags;
       		//Ad a set of One of items
			if(i==0) flags=(EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
			else flags=0;
       		if(gotodata->ModeStrId[i]!=0){
       			// OneOf of 
       			HiiCreateOneOfOptionOpCode (
       					gLdConfigForm.ModeOptionsOpCodeHandle,
       					gotodata->ModeStrId[i],
       					flags,
       		            EFI_IFR_NUMERIC_SIZE_1,
       		            i
       		            );

       		}
       	}//for
    		
       	//Now create OneOf OpCode...
       	HiiCreateOneOfOpCode (
       		gLdConfigForm.StartOpCodeHandle,			//*OpCodeHandle
            gotodata->ModeQid,                        //QuestionId
            gotodata->NvVarStoreId,                     //VarStore ID
            EFI_FIELD_OFFSET(SIO_DEV_NV_DATA, DevMode),//Offset
            STRING_TOKEN(STR_MODE_SETTINGS),			//Prompt
            gotodata->ModeHelpStrId,					//Help 
            (EFI_IFR_FLAG_RESET_REQUIRED | EFI_IFR_FLAG_CALLBACK),//QuestionFlags
            EFI_IFR_NUMERIC_SIZE_1,						//OneOfFlags
            gLdConfigForm.ModeOptionsOpCodeHandle,		//OptionsOpCodeHandle
       		NULL             							//*DefaultsOpCodeHandle  OPTIONAL
        	);
    }

    //close scope
    AmiHiiTerminateScope(gLdConfigForm.StartOpCodeHandle);
    
    //We are done!!
    HiiUpdateForm (
      gSioHiiHandle,
      &SioFormsetGuid,
      SIO_LD_FORM_ID,
      gLdConfigForm.StartOpCodeHandle,
      gLdConfigForm.EndOpCodeHandle
    );

    return EFI_SUCCESS;
}

/**
    This will update LDs current Resource usage information

    @param Resources Pointer to the Possible Resources list of this LD

    @retval resource string, NULL terminated 

**/

EFI_STRING EFIAPI GenerateResourceString(EFI_ASL_DepFn *Resources){
	CHAR16			s[80];
	EFI_STRING		sp=&s[0], retstr=NULL;
	UINTN			i,j;
	ASLRF_S_HDR		*rh;
	
//--------------------------	
	pBS->SetMem(&s[0], sizeof(s),0);

	for(i=0; i<Resources->DepRes.ItemCount; i++ ) {
		rh=(ASLRF_S_HDR*)Resources->DepRes.Items[i];
		
		//IN sio THERE WILL BE ONLY SMALL RESOURCES TYPES
		//and omly following types IO fixedIO IRQ; DMA that's it...
		switch(rh->Name){
			case ASLV_RT_IRQ: {
				ASLR_IRQ	*irqrd=(ASLR_IRQ*)rh;
			//-----------------------	
				Swprintf(sp, L"IRQ=");
				sp=&s[Wcslen(s)];
				
				for(j=0;j<16;j++){
					if(irqrd->_INT & (1<<j)){
						Swprintf(sp, L"%d,",j);	
						sp=&s[Wcslen(s)];
					}
				}
				//Remove last comma and replace it with semicolon
				sp--;
				Swprintf(sp, L"; ");	
				sp=&s[Wcslen(s)];
				break;
			}
		
			case ASLV_RT_DMA: {
				ASLR_DMA	*dmard=(ASLR_DMA*)rh;
			//-----------------------	
				Swprintf(sp, L"DMA=");
				sp=&s[Wcslen(s)];
				
				for(j=0;j<8;j++){
					if(dmard->_DMA & (1<<j)){
						Swprintf(sp, L"%d,",j);	
						sp=&s[Wcslen(s)];
					}
				}
				//Remove last comma and replace it with semicolon
				sp--;
				Swprintf(sp, L"; ");	
				sp=&s[Wcslen(s)];
				break;
			}
			case ASLV_RT_IO: {
				ASLR_IO		*iord=(ASLR_IO*)rh;
			//-------------------	
				Swprintf(sp, L"IO=%Xh; ", iord->_MIN);	
				sp=&s[Wcslen(s)];
				break;
			}
			case ASLV_RT_FixedIO: {
				ASLR_FixedIO *iord=(ASLR_FixedIO*)rh;
			//-------------------	
				Swprintf(sp, L"IO=%Xh; ", iord->_BAS);	
				sp=&s[Wcslen(s)];
				break;
			}
			default: ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
		} //Switch
	}//for
	
	if(Wcslen(s)){ 
		retstr=MallocZ((Wcslen(s)+1)*sizeof(CHAR16));
		Wcscpy(retstr,s);
	}
	
	return retstr;
}

/**
    This will update LDs current configuration strings information

    @param GotoData Pointer to the Corresponded LD goto data structure

    @retval VOID

**/
VOID EFIAPI UpdateConfigurationStrings(LD_SETUP_GOTO_DATA *GotoData){
	EFI_ASL_DepFn	*resbuffer;
	UINTN			i;
	SIO_DEV2		*ld;
	EFI_STRING		s;
//------------------------------	
	ld=GotoData->SioLd;
	
	GotoData->PrsItemCount=ld->PRS.ItemCount;
	GotoData->PrsStrId=MallocZ(sizeof(EFI_STRING_ID)*ld->PRS.ItemCount);
	
	for(i=0; i<ld->PRS.ItemCount; i++){
		//get possible resources of that LD
		resbuffer=(EFI_ASL_DepFn*)ld->PRS.Items[i];
		
		//Get the string with resource shown as IO=xxx; Dma=XXX; Irq=XXX;
		s=GenerateResourceString(resbuffer);
		if(s==NULL) continue;
		
	    // StringId==0 makes function return a new one, SupportedLanguages==NULL sets it for current language.
		GotoData->PrsStrId[i] = HiiSetString(gSioHiiHandle, 0, s, NULL );
		pBS->FreePool(s);
	}
	
	if(ld->DevModeCnt>0){
		GotoData->ModeItemCount=ld->DevModeCnt;
		GotoData->ModeStrId=MallocZ(sizeof(EFI_STRING_ID)*ld->DevModeCnt);
		for(i=0; i<GotoData->ModeItemCount; i++){
			s=ld->DevModeStr[i];
			GotoData->ModeStrId[i]=HiiSetString(gSioHiiHandle, 0, s, NULL );
		}
		//Get the help string...
		s=ld->DevModeStr[GotoData->ModeItemCount];
		GotoData->ModeHelpStrId=HiiSetString(gSioHiiHandle, 0, s, NULL );
	}

}

/**
    Prepares the IFR labels within the SIO page for the controls insertion. It
    also gets the list of AmiSio handles.

    @param SioIfrInfo Pointer to the Private data structure of the Setup Page

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR if something wrong happends

    @note  
  This function does not free the memory allocated earlier for SIO_IFR_INFO;
  caller is responsible for freeing any memory before executing this function

**/

EFI_STATUS EFIAPI InitSioIfrData (SIO_IFR_INFO *SioIfrInfo)
{
    EFI_STATUS  				Status;
    UINTN						LdHandleCount, i, j;
    EFI_HANDLE					*LdHandles=NULL;
    AMI_SIO_PROTOCOL			*AmiSioProtocol;
//-------------------------
    // Create new OpCode Handle
    SioIfrInfo->StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    SioIfrInfo->EndOpCodeHandle = HiiAllocateOpCodeHandle ();

    // Create Hii Extend Label OpCode as the start opcode
    SioIfrInfo->StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            SioIfrInfo->StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    SioIfrInfo->StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    SioIfrInfo->EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
    		SioIfrInfo->EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    SioIfrInfo->EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    SioIfrInfo->StartLabel->Number = SIO_GOTO_LABEL_START;
    SioIfrInfo->EndLabel->Number = SIO_GOTO_LABEL_END;

    // Update LD information
    Status = pBS->LocateHandleBuffer (ByProtocol,
        &gEfiAmiSioProtocolGuid, NULL, &LdHandleCount, &LdHandles);

//EFI_DEADLOOP();
    
    //Now check devices with Handles this ones are Enabled and Active
    for (j=0; j<LdHandleCount; j++){
    	SIO_DEV2	*spio;
    //---------------------------	
    	Status=pBS->HandleProtocol(LdHandles[j], &gEfiAmiSioProtocolGuid, &AmiSioProtocol);
    	ASSERT_EFI_ERROR(Status);
    	
    	//Presence of Our Proprietary Interface constitutes following type casting...
    	spio=(SIO_DEV2*)(AmiSioProtocol);
    	
    	for(i=0; i<SioIfrInfo->LdCount; i++){
    		AMI_SDL_LOGICAL_DEV_INFO	*ld;
    	//------------------------
    		ld=(SioIfrInfo->LdSetupData[i])->SdlInfo;
    		
    		if(ld==spio->DeviceInfo){
    			(SioIfrInfo->LdSetupData[i])->SioLd=spio;
				UpdateConfigurationStrings(SioIfrInfo->LdSetupData[i]);
    		}
   			
    	}
    }
   
    if(LdHandles!=NULL) pBS->FreePool(LdHandles);
	return Status;
}



EFI_STATUS EFIAPI SioRoutConfigCallback(
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN CONST  EFI_STRING                      Configuration,
  OUT       EFI_STRING                      *Progress)
{
    UINTN   i;
    LD_SETUP_GOTO_DATA  *gotodata;
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_GUID    ssg = SIO_VARSTORE_GUID;
    SIO_DEV_NV_DATA     tmpvar;
    UINTN       vs;
//---------------------------------
    if(Configuration==NULL) return EFI_INVALID_PARAMETER;
    if(!HiiIsConfigHdrMatch(Configuration,&ssg,NULL)){
        *Progress=Configuration;
        return EFI_NOT_FOUND;
    }
    
    SIO_TRACE((TRACE_SIO,"SioSetup: SioRoutConfigCallback(Configuration=%S);\n", Configuration));
    *Progress=(EFI_STRING)(Configuration+StrLen(Configuration));
    
    if(!gDataSaved){
        
//EFI_DEADLOOP();        
        
        for(i=0;i<gSioIfrInfo.LdCount; i++){
            gotodata=gSioIfrInfo.LdSetupData[i];
            if( (gotodata->GotoQid & SIO_GOTO_ID_BASE) == 0) continue;
            
            //Update all None Volatile Variables, if this function called reset will be required...
            vs=sizeof(SIO_DEV_NV_DATA);
            
            Status = pRS->GetVariable(gotodata->NvVarName, &ssg, NULL, &vs, &tmpvar);
            ASSERT_EFI_ERROR(Status);
            
            if(MemCmp(&gotodata->SetupNvData, &tmpvar, sizeof(tmpvar)) !=0 ){ 
            
                SIO_TRACE((TRACE_SIO,"SioSetup: Saving VAR %S; [ %02X, %02X, %02X ] \n", gotodata->NvVarName, 
                        gotodata->SetupNvData.DevEnable, gotodata->SetupNvData.DevPrsId, gotodata->SetupNvData.DevMode));
                
                Status = pRS->SetVariable(gotodata->NvVarName, &ssg, 
                        (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS), 
                        vs, &gotodata->SetupNvData);
                ASSERT_EFI_ERROR(Status);
                if(EFI_ERROR(Status)) return Status;
            }            
        }
        gDataSaved=TRUE;
    }
    
    return Status;
}


/**
    This function is called by Setup browser to perform flash update
               
           
    @param This pointer to the instance of ConfigAccess protocol
    @param Action action, that setup browser is performing
    @param KeyValue value of currently processed setup control
    @param Type value type of setup control data
    @param Value pointer to setup control data
    @param ActionRequest pointer where to store requested action

         
    @retval EFI_SUCCESS flash updated successfully
    @retval EFI_UNSUPPORTED browser action is not supported
 
**/

EFI_STATUS EFIAPI SioBrowserCallback(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
)
{
    EFI_STATUS Status;
//---------------------------
	SIO_TRACE((TRACE_SIO,"SioSetup: BrowserCallback(Action %x, KeyValue %x, Type %x).\n", Action, KeyValue, Type));

    if (ActionRequest) 
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

    if(Action == EFI_BROWSER_ACTION_RETRIEVE){
        if((KeyValue&SIO_GOTO_ID_BASE)==SIO_GOTO_ID_BASE) {
            GetSetupNvData(KeyValue);
            return EFI_SUCCESS;
        }
    }
    
    if(Action == EFI_BROWSER_ACTION_CHANGING){
        if((KeyValue&SIO_GOTO_ID_BASE)==SIO_GOTO_ID_BASE) {
            Status = UpdateLdConfigForm(KeyValue);
            ASSERT_EFI_ERROR(Status);
            return Status;
        }

        if((KeyValue&SIO_LD_MODE_QID_BASE)!= 0) {
            Status = UpdateLdVarBuffer(KeyValue, Type, Value);
            ASSERT_EFI_ERROR(Status);
            return Status;
        }

    }    
    
  
    if( (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ||
        Action == EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING || 
        Action == EFI_BROWSER_ACTION_DEFAULT_SAFE)){
        if((KeyValue&SIO_GOTO_ID_BASE)==SIO_GOTO_ID_BASE) {
            LoadLdDefaults(KeyValue);
            gDataSaved=FALSE;
            return EFI_SUCCESS;
        }
    }
    
    return EFI_UNSUPPORTED;
}


/**

    This will sort having setup devices items in Following order:
    1. First go slots in Ascending order than on board devices...

    @param Event Browswer Callback Event 
    @param Context Pointer to the optional Data buffer

    @retval VOID
	
**/

VOID EFIAPI InitSioIfr(IN EFI_EVENT Event, IN VOID *Context)
{
	EFI_STATUS			Status;
//------------------------------------
    //SIO_TRACE((TRACE_SIO, "SioSetup: InitSioIfr(Event 0x%X, Context @0x%X) = ",Event, Context));

    Status=InitSioIfrData (&gSioIfrInfo);

    //SIO_TRACE((TRACE_SIO, "%r.\n",Status));
    
    //if(EFI_ERROR(Status)) return;
    
    CreateLdGotoItems(&gSioIfrInfo);
}

/**
    This function Updates Driver version string with actual number.

**/

VOID EFIAPI InitDriverVersionString()
{
    CHAR16 	s[80];
	CHAR16		d=0;
	EFI_STRING	pd;
//-----------------------------	
	pd=HiiGetString(gSioHiiHandle, STRING_TOKEN(STR_SIO_DRIVER_VER_PROMPT), NULL);
	if(pd==NULL)pd=&d;
    Swprintf(s, L"%s%02X.%02d.%02d",pd, AMI_SIO_MAJOR_VERSION, AMI_SIO_MINOR_VERSION, AMI_SIO_REVISION);
    HiiSetString(gSioHiiHandle, STRING_TOKEN(STR_SIO_DRIVER_VER), s, NULL);   
    if((pd!=(&d)) && (pd!=NULL))pBS->FreePool(pd);
}


/**
    This will sort having setup devices items in Following order:
    1. First go slots in Ascending order than on board devices...

    @param ImageHandle ImageHandle of the loaded driver
    @param SystemTable Pointer to the System Table

    @retval EFI_SUCCESS

**/
VOID EFIAPI SortData(SIO_IFR_INFO* IfrData){
#if defined(SIO_SETUP_LD_TYPE_ORDER)
	SIO_DEV_TYPE	ldorder[]={SIO_SETUP_LD_TYPE_ORDER};
#else //						0  		1	  2		3		4		5		6	  7	 
	SIO_DEV_TYPE	ldorder[]={dsLPT,dsUART,dsCIR,dsPS2CK,dsPS2CM,dsPS2K,dsPS2M,dsFDC};
#endif
	T_ITEM_LIST					*ldtype=NULL;
	T_ITEM_LIST					*ilptr=NULL;
	EFI_STATUS					Status;
	UINTN						i, j;
	LD_SETUP_GOTO_DATA			*gt;
	AMI_SDL_LOGICAL_DEV_INFO	*sd;
//	SIO_DEV_TYPE				ct=dsNone; 
	LD_SETUP_GOTO_DATA			*tmpgt;
	AMI_SDL_LOGICAL_DEV_INFO	*tmpsd;
	BOOLEAN						ap;
//------------------------------------	
//DEBUG	
//EFI_DEADLOOP();	
//DEBUG	

	ldtype=MallocZ(sizeof(T_ITEM_LIST)*(sizeof(ldorder)/sizeof(SIO_DEV_TYPE)));
	if(ldtype==NULL){
		ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
		return;
	}

	while(IfrData->LdCount){
		gt=IfrData->LdSetupData[0];
		sd=gt->SdlInfo;
		//Remove item from unsorted array;
		DeleteItemLst((T_ITEM_LIST*)&IfrData->LdInitCnt, 0, FALSE);
		for(j=0; j<sizeof(ldorder)/sizeof(SIO_DEV_TYPE); j++){
			if(ldorder[j]==sd->Type){
				ilptr=&ldtype[j]; 
				break;
			}
		}	
	    
	    // Now sort data
	    for(i=0, ap=TRUE; i<ilptr->ItemCount; i++){
        	tmpgt=(LD_SETUP_GOTO_DATA*)ilptr->Items[i];
            tmpsd=tmpgt->SdlInfo;
            if(tmpsd->Uid > sd->Uid){
                Status=InsertItemLst(ilptr, gt, i);
                ap=FALSE;
                break;
            }
        }//for...
	    
        // if apPEND Flag is set Ld device == the biggest one yet.
        if(ap) {
        	Status = AppendItemLst(ilptr, gt);
        }

        ASSERT_EFI_ERROR(Status);
		if(EFI_ERROR(Status)) return;
	} //keep going until unsorted list empty.
	
	//now fill same list we just emptied IfrData->LdSetupData[] with sorted data.
	//With Sorted Slot Data....
	for(j=0; j<sizeof(ldorder)/sizeof(SIO_DEV_TYPE); j++) {
		ilptr=&ldtype[j];
		for(i=0;i<ilptr->ItemCount; i++) {
			Status=AppendItemLst((T_ITEM_LIST*)&IfrData->LdInitCnt, ilptr->Items[i]);
			ASSERT_EFI_ERROR(Status);
			if(ilptr->ItemCount>1)((LD_SETUP_GOTO_DATA*)ilptr->Items[i])->LdNumber=i+1;	
		}
	}
	
	//Do some housekeeping - free memory used...
	for(j=0;j<sizeof(ldorder)/sizeof(SIO_DEV_TYPE); j++) {
		ilptr=&ldtype[j];
		ClearItemLst(ilptr,FALSE);
	}
	pBS->FreePool(ldtype);
}

/**
    This is the standard EFI driver entry point called for CSM Policy module
    initlaization

    @param ImageHandle ImageHandle of the loaded driver
    @param SystemTable Pointer to the System Table

    @retval EFI_SUCCESS

**/

EFI_STATUS EFIAPI SioDynamicSetupEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_STATUS 					Status=EFI_SUCCESS;
	UINTN						i,j;
	AMI_SDL_SIO_CHIP_INFO 		*sio;
//---------------------	
    InitAmiLib(ImageHandle, SystemTable);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    Status = AmiSdlInitBoardInfo();
    SIO_TRACE((TRACE_SIO,"SioSetup: Locate AmiBoardInfo2Protocol: Status=%r\n", Status));
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    pBS->SetMem(&gSioIfrInfo, sizeof(SIO_IFR_INFO), 0);
    
    //check if SIO Devices has setup Pages... If no pages - No Setup Resources will be loaded.
    sio=&gSdlSioData->SioDev[0];
    		
    for(j=0; j<gSdlSioData->SioCount; j++){
//APTIOV_SERVER_OVERRIDE_START
		AMI_SDL_LOGICAL_DEV_INFO	*ld = NULL;
//APTIOV_SERVER_OVERRIDE_END
    //--------------------------------
    	for(i=0; i<sio->LogicalDevCount; i++){
    		ld=&sio->LogicalDev[i];
    		//check if Ld implemented on thr board and Has Setup screen...
        	if(ld->Implemented && ld->HasSetup){
        	    LD_SETUP_GOTO_DATA	*ldgoto;
        	//--------------------------------
        	    ldgoto=MallocZ(sizeof(LD_SETUP_GOTO_DATA));
        		if(ldgoto==NULL) return EFI_OUT_OF_RESOURCES;
        		ldgoto->SdlInfo=ld;   
        		ldgoto->SioIdx=j;
        		ldgoto->LdIdx=i;
        		
        		Status=AppendItemLst((T_ITEM_LIST*)&gSioIfrInfo.LdInitCnt, ldgoto);
        		ASSERT_EFI_ERROR(Status);
        		if(EFI_ERROR(Status)) return Status;
        	}
    	}
    	//Advance to the next SIO CHIP in Multiple Sio case.
    	sio=(AMI_SDL_SIO_CHIP_INFO*)(ld+1);
    }
	
	SortData(&gSioIfrInfo);
    //Tell how much devices with setup we have found...
	SIO_TRACE((TRACE_SIO,"SioSetup: Found %d SIO LDs With Setup Pages... %r.\n",gSioIfrInfo.LdCount, Status));
    
    //If no SIO LD devices with Setup .... just exit..
    if(gSioIfrInfo.LdCount==0) return EFI_SUCCESS;
    
    //Load setup page and update strings
    LoadResources(ImageHandle, sizeof(SetupCallBack) / sizeof(CALLBACK_INFO), SetupCallBack, NULL);
    gSioHiiHandle = SetupCallBack[0].HiiHandle;

    InitDriverVersionString();

    // Register callback on TSE event to update IFR data
    {
        VOID *SetupRegistration;
        static EFI_GUID SetupEnterGuid = AMITSE_SETUP_ENTER_GUID;
        static EFI_EVENT SetupEnterEvent;

        RegisterProtocolCallback(
            &SetupEnterGuid, InitSioIfr,
            NULL, &SetupEnterEvent, &SetupRegistration
        );
    }
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
