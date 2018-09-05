//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.19
//    Bug Fix : Correct a wrong status that make system cannot export HII.
//    Reason  : 
//    Auditor : Durant Lin
//    Date    : Jun/12/2018
//
//  Rev. 1.18
//    Bug Fix : N/A
//    Reason  : Modify HII export package lists method and DEBUG Message.
//    Auditor : Durant Lin
//    Date    : Jun/01/2018
//
//  Rev. 1.17
//    Bug Fix : N/A
//    Reason  : Isolate IPMI support for no IPMI module platform InBand. 
//    Auditor : Durant Lin
//    Date    : Feb/07/2018
//
//  Rev. 1.16
//    Bug Fix:  Update default value of "Boot Option #".
//    Reason:
//    Auditor:  Jason Hsu
//    Date:     Apr/06/2017
//
//  Rev. 1.15
//    Bug Fix:  Fix the default value of "IPMI LAN Selection" is incorrect in sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/16/2017
//
//  Rev. 1.14
//    Bug Fix:  Expose "BMC Network Configuration" for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/15/2017
//
//  Rev. 1.13
//    Bug Fix:  Update HideItem data for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/14/2017
//
//  Rev. 1.12
//    Bug Fix:  Modify setup template Json code in HII for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/28/2016
//
//  Rev. 1.11
//    Bug Fix:  Add setup template Json code in HII for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/23/2016
//
//  Rev. 1.10
//    Bug Fix:  Fix some errors for upload and download OOB files.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/20/2016
//
//  Rev. 1.09
//    Bug Fix:  Add HPK - "FixedBootOrder Group Form" for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/14/2016
//
//  Rev. 1.08
//    Bug Fix:  Filter some unused HII packages.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/06/2016
//
//  Rev. 1.07
//    Bug Fix:  Send all HII data for sum and fix checksum error.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/09/2016
//
//  Rev. 1.06
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix HII data checksum error.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/19/2016
//
//  Rev. 1.04
//    Bug Fix:  Calculate HII data checksum and fill to header.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/09/2016
//
//  Rev. 1.03
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.02
//    Bug Fix:  Compress and encipher HII data for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/25/2016
//
//  Rev. 1.01
//    Bug Fix:  Extend FileSize in InBand header.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/22/2016
//
//  Rev. 1.00
//    Bug Fix:  Add InBand HII data for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/21/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcInBandHii.c
//
// Description: In-Band HII feature.
//
//<AMI_FHDR_END>
//**********************************************************************
//#include "Token.h"

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "TianoCompress.h"
#include "CBlowfish.h"
#include "SmcLib.h"
#include "SmcInBand.h"
#include "SmcInBandSetupTemplate.h"
#include "SmcSetupModify.h"
#include "SmcInBandHii.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>

#define MAX_PACKAGELISTS_SIZE 0x180000


extern SMC_GUID_HANDLE GuidHandleMap[MAX_GUID_HANDLE_COUNT];

extern SMC_OOB_PLATFORM_POLICY *mSmcOobPlatformPolicyPtr;

#define MAX_HPK_LIST_SIZE 0x100000
#define MAX_HII_COMBINE_SIZE 0x200000

UINTN ItemValue = 0;

//
// Only below HPK and it's string HPK will be sent to sum.
//
SMC_HPK_DATA AllowedHpkTable[] =
{
    {"Platform Configuration",   {0xEC87D643,0xEBA4,0x4BB5,{0xA1,0xE5,0x3F,0x3E,0x36,0xB2,0x0D,0xA9}}},
    {"All Cpu Information",      {0xdbf338fe,0x93be,0x46d6,{0xad,0xf5,0xc1,0x7c,0x22,0x36,0xd8,0x38}}},
    {"Socket Configuration",     {0x516d5a04,0xc0d5,0x4657,{0xb9,0x08,0xe4,0xfb,0x1d,0x93,0x5e,0xf0}}},
    {"Event Logs",               {0x8BEB8C19,0x3FEC,0x4FAB,{0xA3,0x78,0xC9,0x03,0xE8,0x90,0xFC,0xAE}}},
    {"IPMI",                     {0x01239999,0xFC0E,0x4B6E,{0x9E,0x79,0xD5,0x4D,0x5D,0xB6,0xCD,0x20}}},
    {"Setup",                    {0x7B59104A,0xC00D,0x4158,{0x87,0xFF,0xF0,0x4D,0x63,0x96,0xA9,0x15}}},
    {"FixedBootOrder Group Form",{0xde8ab926,0xefda,0x4c23,{0xbb,0xc4,0x98,0xfd,0x29,0xaa,0x00,0x69}}},
	{"SMCLSIOOBSetup",			 {0xca113bd9,0x21cc,0xaa1b,{0x33,0x58,0xa9,0x73,0x78,0xdd,0x3c,0x5e}}},
    {"-----",                    {0xFFFFFFFF,0xFFFF,0xFFFF,{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}}
};

//HiddenItemMap HideItemForSum[] =
//{
//    {0, L"Setup", STR_DRIVER_ORDER_X}
//};

EFI_GUID NullGuid = {0xFFFFFFFF,0xFFFF,0xFFFF,{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};

EFI_HII_DATABASE_PROTOCOL*	GetHiiDataBaseProtocol(){
	
	static	EFI_HII_DATABASE_PROTOCOL*	sHiiDataBase = NULL;
	EFI_STATUS	Status = EFI_SUCCESS;

	if(!!sHiiDataBase) return sHiiDataBase;

    Status = pBS->LocateProtocol (
                 &gEfiHiiDatabaseProtocolGuid,
                 NULL,
                 &sHiiDataBase
             );

	return (EFI_ERROR(Status)) ? NULL : sHiiDataBase;
}

EFI_STATUS GetListPackageListsHandle(UINTN *ArgBufferLength, EFI_HII_HANDLE* *ArgHandleBuffer){

    static EFI_HII_HANDLE   TempHiiHandleBuffer = NULL;
    static EFI_HII_HANDLE 	*HiiHandleBuffer = NULL;
    static UINTN 			HandleBufferLength = 0;
	EFI_STATUS				Status;


	if(!!HandleBufferLength && !!HiiHandleBuffer)
		goto RETURN_ARG;

	if(!(!!GetHiiDataBaseProtocol())) return EFI_LOAD_ERROR;

	Status = GetHiiDataBaseProtocol()->ListPackageLists(
							GetHiiDataBaseProtocol(),
							EFI_HII_PACKAGE_TYPE_ALL,
							NULL,
							&HandleBufferLength,
							&TempHiiHandleBuffer
						);

	if(Status != EFI_BUFFER_TOO_SMALL) return EFI_LOAD_ERROR;

	pBS->AllocatePool(EfiBootServicesData,
					  HandleBufferLength + sizeof(EFI_HII_HANDLE),
					  &((UINT8*)HiiHandleBuffer));

	if(!(!!HiiHandleBuffer)) return EFI_LOAD_ERROR;

	MemSet((UINT8*)HiiHandleBuffer,(UINT32)(HandleBufferLength + sizeof(EFI_HII_HANDLE)),0x00);

	Status = GetHiiDataBaseProtocol()->ListPackageLists(
							GetHiiDataBaseProtocol(),
							EFI_HII_PACKAGE_TYPE_ALL,
							NULL,
							&HandleBufferLength,
							HiiHandleBuffer
						);

	if(EFI_ERROR(Status)) return EFI_LOAD_ERROR;

RETURN_ARG:
		*ArgBufferLength = HandleBufferLength;
		*ArgHandleBuffer = HiiHandleBuffer;
		return EFI_SUCCESS;
}

EFI_STATUS	GetExportPackageLists(EFI_HII_HANDLE HiiHandle,UINTN	BufferSize, VOID*	Buffer){
		
	UINTN	TempBufferSize = 0;
	EFI_STATUS	Status = EFI_SUCCESS;

	Status = GetHiiDataBaseProtocol()->ExportPackageLists (
				GetHiiDataBaseProtocol(), 
				HiiHandle,
				&TempBufferSize,
				Buffer
			 );
	if(Status != EFI_BUFFER_TOO_SMALL) return Status;
	if(BufferSize <  TempBufferSize) return EFI_BUFFER_TOO_SMALL;
	MemSet (Buffer, BufferSize, 0x0);

    Status = GetHiiDataBaseProtocol()->ExportPackageLists (
				GetHiiDataBaseProtocol(), 
				HiiHandle, 
				&TempBufferSize, 
				Buffer
			 );

	return Status;
}

BOOLEAN AllowedHpk (
    CHAR8* Title,
    EFI_GUID* Guid
)
{
    UINT8* pAllowedHpkTable = (UINT8*)AllowedHpkTable;
    SMC_HPK_DATA* HpkData = (SMC_HPK_DATA*)pAllowedHpkTable;
    UINT32 Count = 0;

    for (Count = 0; Count < sizeof (AllowedHpkTable) / sizeof (SMC_HPK_DATA); Count++) {
        HpkData = (SMC_HPK_DATA*)pAllowedHpkTable;

        if (
            Strcmp ("-----", HpkData->Title) == 0 ||
            MemCmp ((UINT8*)&NullGuid, (UINT8*)&(HpkData->Guid), sizeof (EFI_GUID)) == 0
        ) {
            break; // Table end, break.
        }

        if (
            Strcmp (Title, HpkData->Title) == 0 &&
            MemCmp ((UINT8*)Guid, (UINT8*)&(HpkData->Guid), sizeof (EFI_GUID)) == 0
        ) {
            return TRUE;
        }

        pAllowedHpkTable += sizeof (SMC_HPK_DATA);
    }

    return FALSE;
}

VOID InsertIfrSuppressIf(
    UINT8** pPointer,
    UINT32* Offset
)
{
    EFI_IFR_SUPPRESS_IF IfrSuppressIf;
    IfrSuppressIf.Header.OpCode = EFI_IFR_SUPPRESS_IF_OP;
    IfrSuppressIf.Header.Length = sizeof (EFI_IFR_SUPPRESS_IF);
    IfrSuppressIf.Header.Scope = 1;
    MemCpy (*pPointer, (UINT8*)(&IfrSuppressIf), sizeof (EFI_IFR_SUPPRESS_IF));
    *pPointer += sizeof (EFI_IFR_SUPPRESS_IF);
    *Offset += sizeof (EFI_IFR_SUPPRESS_IF);
}

VOID InsertIfrTrue(
    UINT8** pPointer,
    UINT32* Offset
)
{
    EFI_IFR_TRUE IfrTrue;

    IfrTrue.Header.OpCode = EFI_IFR_TRUE_OP;
    IfrTrue.Header.Length = sizeof(EFI_IFR_TRUE);
    IfrTrue.Header.Scope = 0;
    MemCpy(*pPointer, (UINT8*)(&IfrTrue), sizeof(EFI_IFR_TRUE));
    *pPointer += sizeof(EFI_IFR_TRUE);
    *Offset += sizeof(EFI_IFR_TRUE);
}

VOID InsertIfrEnd(
    UINT8** pPointer,
    UINT32* Offset
)
{
    EFI_IFR_END IfrEnd;

    IfrEnd.Header.OpCode = EFI_IFR_END_OP;
    IfrEnd.Header.Length = sizeof(EFI_IFR_END);
    IfrEnd.Header.Scope = 0;
    MemCpy(*pPointer, (UINT8*)(&IfrEnd), sizeof(EFI_IFR_END));
    *pPointer += sizeof(EFI_IFR_END);
    *Offset += sizeof(EFI_IFR_END);
}

//
// Search for the end - 'EFI_IFR_END_OP'.
//
EFI_IFR_OP_HEADER* SearchForOpNodeEnd (
    EFI_IFR_OP_HEADER* Op,
    UINT8** pPointer,
    UINT32* Offset
)
{
    EFI_IFR_OP_HEADER* pOp;

    pOp = Op;
    while (1) {

        MemCpy((*pPointer), (UINT8*)pOp, pOp->Length);
        (*pPointer) += pOp->Length;
        (*Offset) += pOp->Length;

        if (pOp->OpCode == EFI_IFR_END_OP) {
            break;
        }
        else if (pOp->Scope == 1) {
            pOp = SearchForOpNodeEnd ((EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length), pPointer, Offset);
        }

        pOp = (EFI_IFR_OP_HEADER*)((UINT8*)pOp + pOp->Length);
    }

    return pOp;
}

EFI_STATUS FindFormsetData (
    IN EFI_HII_HANDLE HiiHandle,
    IN EFI_HII_PACKAGE_LIST_HEADER* HiiPackageList,
    OUT CHAR16* StrBuffWide,
    IN OUT UINTN* StrBuffWideSize,
    OUT EFI_IFR_FORM_SET** ppIfrFormSet)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HII_PACKAGE_HEADER *IfrPackagePtr;
    EFI_IFR_OP_HEADER* pEFI_IFR_OP_HEADER;

    for (
        IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList+1);
        IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList+HiiPackageList->PackageLength);
        IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr+IfrPackagePtr->Length))
    {
        if (IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) {
            pEFI_IFR_OP_HEADER = (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + sizeof (EFI_HII_PACKAGE_HEADER));
            if (pEFI_IFR_OP_HEADER->OpCode == EFI_IFR_FORM_SET_OP) {
                MemSet ((UINT8*)StrBuffWide, (*StrBuffWideSize) * sizeof (CHAR16), 0);
                Status = HiiLibGetString (
                             HiiHandle,
                             ((EFI_IFR_FORM_SET*)pEFI_IFR_OP_HEADER)->FormSetTitle,
                             StrBuffWideSize,
                             StrBuffWide);
                if (!EFI_ERROR (Status)) {
                    *ppIfrFormSet = (EFI_IFR_FORM_SET*)pEFI_IFR_OP_HEADER;
                    return Status; // Success
                }
                else {
                    DEBUG((-1, "[SMC_OOB] :: Can't get HII string for 'FormSetTitle'. Status = %r\n", Status));
                    return Status;
                }
            }
        }
    }
    return EFI_NOT_FOUND;
}

UINT64 GetTypeValue (UINT8 Type, EFI_IFR_TYPE_VALUE* TypeValue)
{
    UINT64 Value = 0;
    switch (Type) {
    case EFI_IFR_TYPE_NUM_SIZE_8:
        Value = TypeValue->u8;
        break;
    case EFI_IFR_TYPE_NUM_SIZE_16:
        Value = TypeValue->u16;
        break;
    case EFI_IFR_TYPE_NUM_SIZE_32:
        Value = TypeValue->u32;
        break;
    case EFI_IFR_TYPE_NUM_SIZE_64:
        Value = TypeValue->u64;
        break;
    default:
        Value = TypeValue->u8;
        break;
    }
    return Value;
}

VOID PreUpdateOpData (
    CHAR8* FormSetTitle,
    CHAR8* ItemPrompt
)
{
//    EFI_STATUS Status = EFI_SUCCESS;
//    SERVER_MGMT_CONFIGURATION_DATA ServerConfiguration;
//    EFI_GUID ServerConfigurationGuid = SERVER_MGMT_CONFIGURATION_GUID;
//    UINTN VarSize;
//    UINT32 Attributes;
    if (Strcmp (FormSetTitle, "IPMI") == 0) {
        if (Strcmp (ItemPrompt, "IPMI LAN Selection") == 0) {
//            VarSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
//            Status = gRT->GetVariable (
//                         L"ServerSetup",
//                         &ServerConfigurationGuid,
//                         &Attributes,
//                         &VarSize,
//                         &ServerConfiguration);
//            if (!EFI_ERROR (Status))
//                ItemValue = ServerConfiguration.IPMILANSelection; // Save current value of "IPMI LAN Selection".
//            else
//                ItemValue = 0xffffffff;
          ItemValue = mSmcOobPlatformPolicyPtr->OobRefVariables.IpmiLanSelection;
        }
    }
}

VOID UpdateOpData (
    CHAR8* FormSetTitle,
    CHAR8* ItemPrompt,
    EFI_IFR_OP_HEADER* pIfrOpHeader
)
{
    UINT8    Index;
    CHAR8    Prompt[MAX_TITLE_SIZE];
    CHAR16   buffer[MAX_TITLE_SIZE];

    if (Strcmp (FormSetTitle, "IPMI") == 0) {
        if (Strcmp (ItemPrompt, "IPMI LAN Selection") == 0 && ItemValue != 0xffffffff) {
            EFI_IFR_ONE_OF_OPTION* pOneOfOption = (EFI_IFR_ONE_OF_OPTION*)pIfrOpHeader;
            //
            // Update default value with current value.
            // Because that the value of "IPMI LAN Selection" is read from BMC.
            // It maybe changed dynamically after loading default.
            // So we must sync HII default with it.
            //
            if (GetTypeValue (pOneOfOption->Type, &(pOneOfOption->Value)) == ItemValue)
                pOneOfOption->Flags = 0x0010; // Standard Default
            else
                pOneOfOption->Flags = 0;
        }
    }
    
    if (Strcmp (FormSetTitle, "FixedBootOrder Group Form") == 0) {
      for (Index = 1; Index < 16; Index++) {
        Sprintf (Prompt, "Boot Option #%d", Index);
        AsciiStrToUnicodeStr (Prompt, buffer);
        if (Strcmp (ItemPrompt, Prompt) == 0) {
            EFI_IFR_ONE_OF_OPTION* pOneOfOption = (EFI_IFR_ONE_OF_OPTION*)pIfrOpHeader;
            if (GetTypeValue (pOneOfOption->Type, &(pOneOfOption->Value)) == (Index - 1))
                pOneOfOption->Flags = 0x0010; // Standard Default}}
            else
                pOneOfOption->Flags = 0;
        }
      }
    }
}

VOID EnumerateHpkList (UINT8* DataBuffer, UINT32* DataSize)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandleBufferLength;
    EFI_HII_HANDLE *HiiHandleBuffer = NULL;
    UINTN Index1;
    EFI_HII_PACKAGE_LIST_HEADER *HiiPackageList = NULL;
    EFI_HII_PACKAGE_HEADER *IfrPackagePtr;
    UINT8* pHpkData = NULL;
    EFI_IFR_FORM_SET* pIfrFormSet = NULL;
    CHAR8 FormSetTitle[MAX_TITLE_SIZE];
    UINTN StrBuffWideSize = MAX_TITLE_SIZE;
    CHAR16 FormSetTitleWide[MAX_TITLE_SIZE];
    UINT8* pDataBuffer = DataBuffer;
    UINT8* CurrentHpkList = NULL;
    UINT32 CurrentHpkListSize = 0;
    UINT8* HiiTmpBuff = NULL;
    SMC_SETUP_MODIFY_PROTOCOL* SmcSetupModifyProtocol = NULL;

    DEBUG((-1, "[SMC_OOB] :: EnumerateHpkList entry.\n"));

    //
    // Clear GuidHandleMap.
    //
    MemSet ((UINT8*)&(GuidHandleMap[0]), sizeof (SMC_GUID_HANDLE) * MAX_GUID_HANDLE_COUNT, 0);

	if(!(!!GetHiiDataBaseProtocol())){
		DEBUG((-1,"[SMC_OOB] :: Locate protocol - gEfiHiiDatabaseProtocolGuid failed.\n"));
		goto ErrorFreeMem;
	}

    Status = pBS->LocateProtocol (&gSetupModifyProtocolGuid , NULL, &SmcSetupModifyProtocol);
    if (EFI_ERROR (Status)) {
        SmcSetupModifyProtocol = NULL;
    }

//    for(Index1 = 0; Index1 < sizeof (HideItemForSum) / sizeof (HiddenItemMap); Index1++) {
//        UnicodeStrToAsciiStr (&(HideItemForSum[Index1].FormsetTitle[0]), &(FormSetTitle[0]));
//        SmcSetupModifyProtocol->Add2HideList(FormSetTitle, HideItemForSum[Index1].StringId);
//    }

    //
    // Retrieve the size required for the buffer of all HII handles.
    //
    HandleBufferLength = 0;
	if(EFI_ERROR(GetListPackageListsHandle(&HandleBufferLength,&HiiHandleBuffer))){
		DEBUG((-1,"[SMC_OOB] :: GetListPackageListHandle Error!\n"));
		goto ErrorFreeMem;
	}
    
	Status = pBS->AllocatePool(EfiBootServicesData, MAX_PACKAGELISTS_SIZE, &HiiTmpBuff);
    if (EFI_ERROR (Status)) {
        DEBUG ((-1, "[SMC_OOB] :: Can't allocate memory for 'HiiTmpBuff'. Status = %r\n", Status));
        goto ErrorFreeMem;
    }	


    HiiPackageList = NULL;
    Status = pBS->AllocatePool(EfiBootServicesData, MAX_HPK_LIST_SIZE, &((UINT8*)HiiPackageList));
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "[SMC_OOB] :: Can't allocate memory for 'HiiPackageList'. Status = %r\n", Status));
        goto ErrorFreeMem;
    }

    for (Index1 = 0; ; Index1++) {

        if (HiiHandleBuffer[Index1] == NULL || Index1 >= (HandleBufferLength/sizeof(EFI_HANDLE))) {
            DEBUG((-1, "[SMC_OOB] :: Break from getting HPK.\n"));
            break;
        }

		Status = GetExportPackageLists(HiiHandleBuffer[Index1],MAX_HPK_LIST_SIZE,HiiPackageList);

        if (Status == EFI_BUFFER_TOO_SMALL) {
            DEBUG((-1, "[SMC_OOB] :: ERROR : Hpk List Size is too big. Please extend the value - MAX_HPK_LIST_SIZE.\n"));
            continue;
        }
        else if (EFI_ERROR (Status)) {
            DEBUG((-1, "[SMC_OOB] :: ERROR : Can't export package lists. Continue. Status = %r\n", Status));
            continue;
        }

        //
        // Search for formset data in "HiiPackageList".
        //
        StrBuffWideSize = MAX_TITLE_SIZE;
        Status = FindFormsetData (
                     HiiHandleBuffer[Index1],
                     HiiPackageList,
                     FormSetTitleWide,
                     &StrBuffWideSize,
                     &pIfrFormSet);
        if (EFI_ERROR (Status)) {
            DEBUG((-1, "[SMC_OOB] :: ERROR : Can't find the formset title of package lists. Continue. Status = %r\n", Status));
            DEBUG((-1, "\n"));
            continue;
        }

        UnicodeStrToAsciiStr (&(FormSetTitleWide[0]), &(FormSetTitle[0]));
        if (!AllowedHpk (FormSetTitle, &(pIfrFormSet->Guid))) {
            DEBUG((-1, "[SMC_OOB] :: EnumerateHpkList : Skip HPK list - %s, %g\n", FormSetTitleWide, &(pIfrFormSet->Guid)));
            DEBUG((-1, "\n"));
            continue;
        }

        DEBUG((-1, "[SMC_OOB] :: EnumerateHpkList : Combine HPK list - %s, %g\n", FormSetTitleWide, &(pIfrFormSet->Guid)));

        AddGuidHandle (&(pIfrFormSet->Guid), HiiHandleBuffer[Index1]);

        CurrentHpkList = pDataBuffer; // Record the HPK list pointer.
        CurrentHpkListSize = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
        MemCpy (pDataBuffer, (UINT8*)HiiPackageList, sizeof (EFI_HII_PACKAGE_LIST_HEADER));
        pDataBuffer += sizeof (EFI_HII_PACKAGE_LIST_HEADER);
        (*DataSize) += sizeof (EFI_HII_PACKAGE_LIST_HEADER);

        for (
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)(HiiPackageList+1);
            IfrPackagePtr < (EFI_HII_PACKAGE_HEADER*)((UINT8*)HiiPackageList+HiiPackageList->PackageLength);
            IfrPackagePtr = (EFI_HII_PACKAGE_HEADER*)((UINT8*)IfrPackagePtr+IfrPackagePtr->Length))
        {
            EFI_IFR_OP_HEADER* pIfrOpHeader = NULL;
            UINT8* pHiiTmpBuff = NULL;
            UINT32 TotalSize = 0;
            BOOLEAN NeedToUpdateHpk = 0;
            CHAR8 ItemPrompt[MAX_TITLE_SIZE];
            CHAR16 ItemPromptWide[MAX_TITLE_SIZE];

#if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1
            if (//IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS && Strcmp (FormSetTitle, "Socket Configuration") == 0 ||
                IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS && Strcmp (FormSetTitle, "IPMI") == 0
            ) {
                //DisplayHiiPackageDebugMessage (HiiHandleBuffer[Index1], IfrPackagePtr);
            }
#endif // #if SMC_OOB_DEBUG_MESSAGE_SUPPORT == 1

            if (SmcSetupModifyProtocol != NULL && IfrPackagePtr->Type == EFI_HII_PACKAGE_FORMS) {
                MemSet ((UINT8*)HiiTmpBuff, MAX_PACKAGELISTS_SIZE, 0);
                pHiiTmpBuff = HiiTmpBuff;
                TotalSize = 0;

                //
                // Attach HPK header.
                //
                MemCpy(pHiiTmpBuff, (UINT8*)IfrPackagePtr, sizeof(EFI_HII_PACKAGE_HEADER));
                pHiiTmpBuff += sizeof(EFI_HII_PACKAGE_HEADER);
                TotalSize += sizeof(EFI_HII_PACKAGE_HEADER);

                for (
                    pIfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + sizeof(EFI_HII_PACKAGE_HEADER));
                    pIfrOpHeader < (EFI_IFR_OP_HEADER*)((UINT8*)IfrPackagePtr + IfrPackagePtr->Length);
                    pIfrOpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)pIfrOpHeader + pIfrOpHeader->Length))
                {   // FOR - Loop all OP
                    BOOLEAN FoundHideItem = 0;

                    switch (pIfrOpHeader->OpCode) {
                    case EFI_IFR_ONE_OF_OP :
                    {
                        EFI_IFR_ONE_OF* pOneOf = (EFI_IFR_ONE_OF*)pIfrOpHeader;
                        if(SmcSetupModifyProtocol->SearchForHideListByString(FormSetTitleWide, pOneOf->Question.Header.Prompt)) {
                            DEBUG((-1, "[SMC_OOB] ::     HideItem - %s, 0x%04x\n", FormSetTitleWide, pOneOf->Question.Header.Prompt));
                            FoundHideItem = 1;
                        }
                        StrBuffWideSize = MAX_TITLE_SIZE;
                        Status = HiiLibGetString (
                                     HiiHandleBuffer[Index1],
                                     pOneOf->Question.Header.Prompt,
                                     &StrBuffWideSize,
                                     ItemPromptWide);
                        UnicodeStrToAsciiStr (ItemPromptWide, ItemPrompt);
                        PreUpdateOpData (FormSetTitle, ItemPrompt);
                        break;
                    }
                    case EFI_IFR_ONE_OF_OPTION_OP :
                    {
                        EFI_IFR_ONE_OF_OPTION* pOneOfOption = (EFI_IFR_ONE_OF_OPTION*)pIfrOpHeader;
                        UpdateOpData (FormSetTitle, ItemPrompt, pIfrOpHeader);
                        break;
                    }
                    case EFI_IFR_CHECKBOX_OP :
                    {
                        EFI_IFR_CHECKBOX* pCheckBox = (EFI_IFR_CHECKBOX*)pIfrOpHeader;
                        if(SmcSetupModifyProtocol->SearchForHideListByString(FormSetTitleWide, pCheckBox->Question.Header.Prompt)) {
                            DEBUG((-1, "[SMC_OOB] ::     HideItem - %s, 0x%04x\n", FormSetTitleWide, pCheckBox->Question.Header.Prompt));
                            FoundHideItem = 1;
                        }
                        break;
                    }
                    case EFI_IFR_NUMERIC_OP :
                    {
                        EFI_IFR_NUMERIC* pNumeric = (EFI_IFR_NUMERIC*)pIfrOpHeader;
                        if(SmcSetupModifyProtocol->SearchForHideListByString(FormSetTitleWide, pNumeric->Question.Header.Prompt)) {
                            DEBUG((-1, "[SMC_OOB] ::     HideItem - %s, 0x%04x\n", FormSetTitleWide, pNumeric->Question.Header.Prompt));
                            FoundHideItem = 1;
                        }
                        break;
                    }
                    case EFI_IFR_REF_OP :
                    {
                        EFI_IFR_REF* pRef = (EFI_IFR_REF*)pIfrOpHeader;
                        if(SmcSetupModifyProtocol->SearchForHideListByString(FormSetTitleWide, pRef->Question.Header.Prompt)) {
                            DEBUG((-1, "[SMC_OOB] ::     HideItem - %s, 0x%04x\n", FormSetTitleWide, pRef->Question.Header.Prompt));
                            FoundHideItem = 1;
                        }
                        break;
                    }
                    case EFI_IFR_SUBTITLE_OP:
                    {
                        EFI_IFR_SUBTITLE* pSubTitle = (EFI_IFR_SUBTITLE*)pIfrOpHeader;
                        if(SmcSetupModifyProtocol->SearchForHideListByString(FormSetTitleWide, pSubTitle->Statement.Prompt)) {
                            DEBUG((-1, "[SMC_OOB] ::     HideItem - %s, 0x%04x\n", FormSetTitleWide, pSubTitle->Statement.Prompt));
                            FoundHideItem = 1;
                        }
                    }
                    }

                    //
                    // Found hide item, insert suppressif true OP.
                    //
                    if(FoundHideItem) {

                        InsertIfrSuppressIf(&pHiiTmpBuff, &TotalSize);
                        InsertIfrTrue(&pHiiTmpBuff, &TotalSize);

                        MemCpy(pHiiTmpBuff, (UINT8*)pIfrOpHeader, pIfrOpHeader->Length);
                        pHiiTmpBuff += pIfrOpHeader->Length;
                        TotalSize += pIfrOpHeader->Length;

                        if (pIfrOpHeader->Scope) {
                            pIfrOpHeader = SearchForOpNodeEnd (
                                               (EFI_IFR_OP_HEADER*)((UINT8*)pIfrOpHeader + pIfrOpHeader->Length),
                                               &pHiiTmpBuff,
                                               &TotalSize);
                        }

                        InsertIfrEnd(&pHiiTmpBuff, &TotalSize);

                        NeedToUpdateHpk = 1;
                    }
                    else {
                        MemCpy (pHiiTmpBuff, (UINT8*)pIfrOpHeader, pIfrOpHeader->Length);
                        pHiiTmpBuff += pIfrOpHeader->Length;
                        TotalSize += pIfrOpHeader->Length;
                    }
                } // FOR - Loop all OPs
            } // if (SmcSetupModifyProtocol != NULL) {

            if (DataBuffer != NULL) {
                //
                // Attach this HPK to buffer.
                //
                if (NeedToUpdateHpk) {
                    ((EFI_HII_PACKAGE_HEADER*)HiiTmpBuff)->Length = TotalSize; // Update the new HPK length.
                    MemCpy (pDataBuffer, HiiTmpBuff, TotalSize);
                    pDataBuffer += TotalSize;
                }
                else {
                    MemCpy (pDataBuffer, (UINT8*)IfrPackagePtr, IfrPackagePtr->Length);
                    pDataBuffer += IfrPackagePtr->Length;
                }
            }

            //
            // If "DataBuffer" is NULL, only calculate DataSize.
            //
            if (NeedToUpdateHpk) {
                (*DataSize) += (UINT32)TotalSize;
                CurrentHpkListSize += (UINT32)TotalSize;
            }
            else {
                (*DataSize) += (UINT32)IfrPackagePtr->Length;
                CurrentHpkListSize += (UINT32)IfrPackagePtr->Length;
            }
            DEBUG((-1, "[SMC_OOB] :: Original HPK size = 0x%x\n", IfrPackagePtr->Length));
            DEBUG((-1, "[SMC_OOB] :: New HPK size = 0x%x\n", TotalSize));
            DEBUG((-1, "[SMC_OOB] :: (*DataSize) = 0x%x\n", (*DataSize)));
        } // FOR - Loop all HPKs
        ((EFI_HII_PACKAGE_LIST_HEADER*)CurrentHpkList)->PackageLength = CurrentHpkListSize; // Update the new HPK list length.
        DEBUG((-1, "[SMC_OOB] :: Original HPK list size = 0x%x\n", ((EFI_HII_PACKAGE_LIST_HEADER*)HiiPackageList)->PackageLength));
        DEBUG((-1, "[SMC_OOB] :: New HPK list size = 0x%x\n", ((EFI_HII_PACKAGE_LIST_HEADER*)CurrentHpkList)->PackageLength));
        DEBUG((-1, "\n"));
    } // FOR - Loop all HPK list

ErrorFreeMem:

    if (HiiHandleBuffer != NULL) pBS->FreePool((UINT8*)HiiHandleBuffer);
    if (HiiPackageList != NULL) pBS->FreePool((UINT8*)HiiPackageList);
    if (HiiTmpBuff != NULL) pBS->FreePool((UINT8*)HiiTmpBuff);

    DEBUG((-1, "[SMC_OOB] :: EnumerateHpkList end.\n"));
}

VOID CombineHiiData (UINT8* BuffAddress, UINT32* BuffSize)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8* HiiDataBuffer = NULL;
    UINT32 HiiDataSize = 0;
    UINT8* OriDataBuffer = NULL;
    UINT32 OriDataSize = 0;
    UINT8* DstBuffer = NULL;
    UINT32 DstSize = 0;
    INBAND_HII_FILE_HEADER* pInBandHiiFileHeader = NULL;
    UINT32 ii = 0;
    UINT32 Rem = 0;
    UINT8 Checksum;

    DEBUG((-1, "[SMC_OOB] :: CombineHiiData entry.\n"));

    MemSet ((UINT8*)&(GuidHandleMap[0]), sizeof (SMC_GUID_HANDLE) * MAX_GUID_HANDLE_COUNT, 0);

    *BuffSize = 0;

    Status = pBS->AllocatePool(EfiBootServicesData, MAX_HII_COMBINE_SIZE, &HiiDataBuffer);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : Allocating memory for HiiDataBuffer is failed.\n"));
        goto ErrorFreeMem;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, MAX_HII_COMBINE_SIZE, &OriDataBuffer);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : Allocating memory for OriDataBuffer is failed.\n"));
        goto ErrorFreeMem;
    }

    //
    // Get the HII data.
    //
    EnumerateHpkList (HiiDataBuffer, &HiiDataSize);

    DEBUG((-1, "[SMC_OOB] :: CombineHiiData : EnumerateHpkList - HiiDataSize = 0x%x\n", HiiDataSize));

    //
    // Generate Json code of setup template and attach it behind HII data.
    // The Json code would be included into a HPK, it's type is '0xF0'.
    //
    OriDataSize = 0;
    GenFormLayout (HiiDataBuffer, HiiDataSize, OriDataBuffer, &OriDataSize);

    DEBUG((-1, "[SMC_OOB] :: CombineHiiData : GenFormLayout - OriDataSize = 0x%x\n", OriDataSize));

    //DEBUG((-1, "[SMC_OOB] :: CombineHiiData : 06 OriDataSize = 0x%x\n", OriDataSize));
    DEBUG((-1, "[SMC_OOB] :: CombineHiiData : MAX_HII_COMBINE_SIZE = 0x%x\n", MAX_HII_COMBINE_SIZE));
    if (OriDataSize > MAX_HII_COMBINE_SIZE) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : Buffer is too small. Please extend the value - MAX_HII_COMBINE_SIZE.\n"));
        goto ErrorFreeMem;
    }

    Status = pBS->AllocatePool(EfiBootServicesData, OriDataSize, &DstBuffer);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : Allocating memory for HII compress buffer is failed.\n"));
        goto ErrorFreeMem;
    }

    //
    // Compress the HII data.
    //
    DstSize = OriDataSize;
    TianoCompress (OriDataBuffer, OriDataSize, DstBuffer, &DstSize);
    *BuffSize = DstSize;

    DEBUG((-1, "[SMC_OOB] :: CombineHiiData : 07 DstSize = 0x%x\n", DstSize));

    //
    // The input data of CBlowFishEncipherCharArray must be divisible by 8.
    //
    Rem = DstSize % 8;
    if (Rem == 0) {
    }
    else {
        for (ii = 0; ii < (8 - Rem); ii++) {
            DstBuffer[DstSize + ii] = '\n';
        }
        DstSize = DstSize + (8 - Rem);
        *BuffSize = DstSize;
    }

    DEBUG((-1, "[SMC_OOB] :: CombineHiiData : 08 DstSize = 0x%x\n", DstSize));

    //
    // Encipher the HII Data.
    //
    CBlowFishInit ("SMCIKEY");
    if (CBlowFishEncipherCharArray (DstBuffer, DstSize, DstSize) == 0) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : Can't encipher OOB DAT data.\n"));
        goto ErrorFreeMem;
    }

    *BuffSize = DstSize + sizeof (INBAND_HII_FILE_HEADER);

    //
    // Calculate data checksum exclude header
    //
    Checksum = 0;
    for(ii = 0; ii < DstSize; ii++)
        Checksum += DstBuffer[ii];

    DEBUG((-1, "[SMC_OOB] :: CombineHiiData : 11 Checksum = 0x%x\n", Checksum));

    //
    // Fill data into header.
    //
    pInBandHiiFileHeader = (INBAND_HII_FILE_HEADER*)BuffAddress;
    MemCpy (&(pInBandHiiFileHeader->Signature[0]), "_HII", 4);
    pInBandHiiFileHeader->Version = mSmcOobPlatformPolicyPtr->OobConfigurations.HiiVersion;
    pInBandHiiFileHeader->FileSize = (UINT32)DstSize;
    pInBandHiiFileHeader->Checksum = Checksum;

    //
    // Copy HII data to buffer.
    //
    MemCpy (BuffAddress + sizeof (INBAND_HII_FILE_HEADER), DstBuffer, DstSize);

ErrorFreeMem:
    if (HiiDataBuffer != NULL) pBS->FreePool((UINT8*)HiiDataBuffer);
    if (DstBuffer != NULL) pBS->FreePool((UINT8*)DstBuffer);
    if (OriDataBuffer != NULL) pBS->FreePool((UINT8*)OriDataBuffer);

    DEBUG((-1, "[SMC_OOB] :: CombineHiiData end.\n"));
}

