//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/TcgSetup/TcgSetup.c 1     10/08/13 12:05p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:05p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/TcgSetup/TcgSetup.c $
//
// 1     10/08/13 12:05p Fredericko
// Initial Check-In for Tpm-Next module
//
// 1     7/10/13 5:57p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
// 2     3/29/11 1:24p Fredericko
//
// [TAG]        EIP 54642
// [Category] Improvement
// [Description] 1. Checkin Files related to TCG function override
// 2. Include TCM and TPM auto detection
// [Files] Affects all TCG files
//
//
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  TcgSetup.c
//
// Description:
//
//
//<AMI_FHDR_END>
//**********************************************************************
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/AMIPostMgr.h>
#include <AmiTcg/TCGMisc.h>
#include <Setup.h>
//#include <Library\DebugLib.h>
#include <Library/HiiLib.h>


/**
    This function is eLink'ed with the chain executed right before
    the Setup.

**/
extern EFI_GUID gTcgInternalflagGuid;


VOID InitTcgStrings(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
    UINTN          Size = sizeof(AMITCGSETUPINFOFLAGS);
    AMITCGSETUPINFOFLAGS    Info;
    EFI_STATUS     Status;
    UINT32         VariableAttributes=0;
    SETUP_DATA              SetupDataBuffer;
    UINTN                   SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                  SetupVariableAttributes;
    EFI_GUID                gSetupGuid = SETUP_GUID;

    CHAR16         *String[0x500];
    CHAR16         *Manufacturer[20];
    CHAR8          *StringBegin;
    UINTN          len=0;
    BOOLEAN        Comma=FALSE;
    UINTN          StingSize = 0x500;
    CHAR16         myPtr;
    UINT32         ManufacturerVal;
    UINT32         iter=0;

    Status = pRS->GetVariable (L"Setup",
                               &gSetupGuid,
                               &SetupVariableAttributes,
                               &SetupVariableSize,
                               &SetupDataBuffer);
    if(EFI_ERROR(Status))return;

    pBS->SetMem(String, 0x500, L'\0');
    pBS->SetMem(Manufacturer, 0x20, L'\0');

    if(SetupDataBuffer.Tpm20Device == 0 && SetupDataBuffer.TpmHrdW == 0)
    {
        //TPM 1.2 or TCM Devices
        if(SetupDataBuffer.TpmEnaDisable == 0)
        {

            HiiLibGetString(HiiHandle, STRING_TOKEN(STR_TPM_ENABLE), &StingSize, (EFI_STRING)String);
            InitString(HiiHandle, STRING_TOKEN(STR_ENABLED_PROMPT_VALUE),
                       L"%S", String);
        }
        else
        {
            HiiLibGetString(HiiHandle, STRING_TOKEN(STR_TPM_DISABLE), &StingSize, (EFI_STRING)String);
            InitString(HiiHandle, STRING_TOKEN(STR_ENABLED_PROMPT_VALUE),
                       L"%S", String);
        }

        pBS->SetMem(String, 0x500, L'\0');
        StingSize = 0x500;

        if(SetupDataBuffer.TpmActDeact == 1)
        {
            HiiLibGetString(HiiHandle, STRING_TOKEN(STR_TPMDEACTIVATED), &StingSize, (EFI_STRING)String);
            InitString(HiiHandle, STRING_TOKEN(STR_ACTIVATED_PROMPT_VALUE),
                       L"%S", String);
        }
        else
        {
            HiiLibGetString(HiiHandle, STRING_TOKEN(STR_TPMACTIVATED), &StingSize, (EFI_STRING)String);
            InitString(HiiHandle, STRING_TOKEN(STR_ACTIVATED_PROMPT_VALUE),
                       L"%S", String);
        }

        pBS->SetMem(String, 0x500, L'\0');
        StingSize = 0x500;
        if(SetupDataBuffer.TpmOwnedUnowned == 1)
        {
            HiiLibGetString(HiiHandle, STRING_TOKEN(STR_OWNED), &StingSize, (EFI_STRING)String);
            InitString(HiiHandle, STRING_TOKEN(STR_OWNED_PROMPT_VALUE),
                       L"%S", String);
        }
        else
        {
            HiiLibGetString(HiiHandle, STRING_TOKEN(STR_UNOWNED), &StingSize, (EFI_STRING)String);
            InitString(HiiHandle, STRING_TOKEN(STR_OWNED_PROMPT_VALUE),
                       L"%S", String);
        }
    }


    Status = pRS->GetVariable( L"PCRBitmap", \
                               &gTcgInternalflagGuid, \
                               &VariableAttributes, \
                               &Size, \
                               &Info );
    if(EFI_ERROR(Status))return;

    pBS->SetMem(String, 0x500, 0);

    StringBegin = (UINT8 *)&String[0];
    if(Info.SupportedPcrBitMap & 1)
    {
        len = Wcslen(L"SHA-1");
        pBS->CopyMem(StringBegin, L"SHA-1", len*2);
        StringBegin += len*2;
        Comma = TRUE;
        SetupDataBuffer.Sha1Supported=1;
    }

    if(Info.SupportedPcrBitMap & 2)
    {
        if(Comma)
        {
            len = Wcslen(L",SHA256");
            pBS->CopyMem(StringBegin, L",SHA256", len*2);
        }
        else
        {
            len = Wcslen(L"SHA256");
            pBS->CopyMem(StringBegin, L"SHA256", len*2);
        }
        StringBegin += len*2;
        Comma = TRUE;
        SetupDataBuffer.Sha256Supported=2;
    }

    if(Info.SupportedPcrBitMap & 4)
    {
        if(Comma)
        {
            len = Wcslen(L",SHA384");
            pBS->CopyMem(StringBegin, L",SHA384", len*2);
        }
        else
        {
            len = Wcslen(L"SHA384");
            pBS->CopyMem(StringBegin, L"SHA384", len*2);
        }
        StringBegin += len*2;
        Comma = TRUE;
        SetupDataBuffer.Sha384Supported=4;
    }

    if(Info.SupportedPcrBitMap & 0x8)
    {
        if(Comma)
        {
            len = Wcslen(L",SHA512");
            pBS->CopyMem(StringBegin,  L",SHA512", len*2);
        }
        else
        {
            len = Wcslen(L"SHA512");
            pBS->CopyMem(StringBegin,  L"SHA512", len*2);
        }
        StringBegin += len*2;
        Comma = TRUE;
        SetupDataBuffer.Sha512Supported=8;
    }

    if(Info.SupportedPcrBitMap & 0x10)
    {
        if(Comma)
        {
            len = Wcslen(L",SM3");
            pBS->CopyMem(StringBegin,  L",SM3", len*2);
        }
        else
        {
            len = Wcslen(L"SM3");
            pBS->CopyMem(StringBegin,  L"SM3", len*2);
        }
        StringBegin += len*2;
        SetupDataBuffer.SM3Supported=0x10;
    }

    Status = pRS->SetVariable (
                 L"Setup",
                 &gSetupGuid,
                 SetupVariableAttributes,
                 SetupVariableSize,
                 &SetupDataBuffer);

    InitString(HiiHandle, STRING_TOKEN(STR_TPM_SUPPORTED_PCR_BANKS_VALUE),
               L"%s", String);

    pBS->SetMem(String, 0x500, 0);

    StringBegin = (UINT8 *)&String[0];
    Comma = FALSE;

    if(Info.ActivePcrBitMap & 1)
    {
        len = Wcslen(L"SHA-1");
        pBS->CopyMem(StringBegin, L"SHA-1", len*2);
        StringBegin += len*2;
        Comma = TRUE;
    }

    if(Info.ActivePcrBitMap & 2)
    {
        if(Comma)
        {
            len = Wcslen(L",SHA256");
            pBS->CopyMem(StringBegin, L",SHA256", len*2);
        }
        else
        {
            len = Wcslen(L"SHA256");
            pBS->CopyMem(StringBegin, L"SHA256", len*2);
        }
        StringBegin += len*2;
        Comma = TRUE;
    }

    if(Info.ActivePcrBitMap & 4)
    {
        if(Comma)
        {
            len = Wcslen(L",SHA384");
            pBS->CopyMem(StringBegin, L",SHA384", len*2);
        }
        else
        {
            len = Wcslen(L"SHA384");
            pBS->CopyMem(StringBegin, L"SHA384", len*2);
        }
        StringBegin += len*2;
        Comma = TRUE;
    }

    if(Info.ActivePcrBitMap & 0x8)
    {
        if(Comma)
        {
            len = Wcslen(L",SHA512");
            pBS->CopyMem(StringBegin, L",SHA512", len*2);
        }
        else
        {
            len = Wcslen(L"SHA512");
            pBS->CopyMem(StringBegin, L"SHA512", len*2);
        }
        StringBegin += len*2;
        Comma = TRUE;
    }

    if(Info.ActivePcrBitMap & 0x10)
    {
        if(Comma)
        {
            len = Wcslen(L",SM3");
            pBS->CopyMem(StringBegin, L",SM3", len*2);
        }
        else
        {
            len = Wcslen(L"SM3");
            pBS->CopyMem(StringBegin, L"SM3", len*2);
        }
    }

    InitString(HiiHandle, STRING_TOKEN(STR_TPM_ACTIVE_PCR_BANKS_VALUE),
               L"%s", String);
    
    pBS->SetMem(String, 0x500, 0);
    StringBegin = (CHAR8 *)&String[0];
    
    pBS->CopyMem(StringBegin, L"  Firmware Version: ", Wcslen(L"  Firmware Version: ")*2);
    StringBegin += Wcslen(L"  Firmware Version: ")*2;
    
    Swprintf((CHAR16 *)StringBegin, L"%d.%d", Info.TpmFwVersion >> 16, Info.TpmFwVersion & 0xFFFF);
    InitString(HiiHandle, STRING_TOKEN(STR_TPM_FW_VERSION),L"%s", String);
    
    pBS->SetMem(String, 0x500, 0);
    StringBegin = (CHAR8 *)&String[0];
    
    pBS->CopyMem(StringBegin, L"  Vendor: ", Wcslen(L"  Vendor: ")*2);
    StringBegin += Wcslen(L"  Vendor: ")*2;
    ManufacturerVal = Info.TpmManufacturer;
    StringBegin+=6;
        
    while(iter < 4){
        myPtr = (UINT16) ManufacturerVal & 0xFF;
        pBS->CopyMem((CHAR16 *)StringBegin, &myPtr, sizeof(CHAR16)); 
        StringBegin-=2;
        iter+=1;
        ManufacturerVal = ManufacturerVal >> 8;
    } 
    
    InitString(HiiHandle, STRING_TOKEN(STR_TPM_MANUFACTURER),L"%s", String);

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
