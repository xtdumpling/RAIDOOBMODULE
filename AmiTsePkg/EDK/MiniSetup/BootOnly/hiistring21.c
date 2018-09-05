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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/BootOnly/hiistring21.c $
//
// $Author: Rajashakerg $
//
// $Revision: 17 $
//
// $Date: 5/28/12 7:27a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file hiistring21.c
    This file contains code to handle UEFI2.1 supported hii strings.

**/

#ifdef TSE_FOR_APTIO_4_50
#include "Token.h"  
#include "Efi.h"
#include "Protocol/DevicePath.h"
#else
#include "tokens.h"
#include "efi.h"
#endif

#ifdef TSE_FOR_APTIO_4_50
#ifndef EFI_PROTOCOL_DEFINITION
#define TSE_STRINGIZE(a) #a
#define EFI_PROTOCOL_DEFINITION(a) TSE_STRINGIZE(Protocol/a.h)
#endif
#endif

#include EFI_PROTOCOL_DEFINITION (HiiString)
#include EFI_PROTOCOL_DEFINITION (HiiDatabase)
#include EFI_PROTOCOL_DEFINITION (HiiFont)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)
#include EFI_PROTOCOL_DEFINITION (SimpleTextIn)
#include EFI_PROTOCOL_DEFINITION (SimpleTextInEx)
#include EFI_PROTOCOL_DEFINITION (AmiKeycode)
#include EFI_PROTOCOL_DEFINITION (FormBrowser2)

#include "mem.h"
#include "HiiLib.h"
#include "variable.h"
#include "TseDrvHealth.h"       //Has TSE related driver health structures
#include "boot.h"
#include <Protocol/AMIPostMgr.h>
#include <Protocol/EsaTseInterfaces.h>
#include "PwdLib.h"
#include "AmiLib.h"				//Added for using CONVERT_TO_STRING macro
#include "AmiDxeLib.h"
#include "Library/PcdLib.h"
#if TSE_SUPPORT_WRITE_CHAR_LAST_ROW_COLUMN
#include "Protocol/AmiTextOut.h"
#endif //TSE_SUPPORT_WRITE_CHAR_LAST_ROW_COLUMN

#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
#include "Protocol/MouseProtocol.h"
#include "Protocol/SoftKbdProtocol.h"
#endif

#pragma pack(push,1)
typedef struct{
    EFI_GUID SubtypeGuid;
	UINT32 NumberOfPackages;
} RESOURCE_SECTION_HEADER;
#pragma pack(pop)

UINT32      CheckBootOptionMatch (UINT16 BootOption);
HOT_KEYS    *gHotKeysDetails = NULL;
UINTN       gHotKeysCount = 0;
UINTN       gAllocateKeysCount = 0;
UINTN       gSkipEscCode = 0 ;
#define HOTKEY_ALLOC_UNIT   10
#define HOT_KEY_SHIFT           0x00000100          
#define HOT_KEY_CTRL            0x00000200
#define HOT_KEY_ALT             0x00000400
#define HOT_KEY_LOGO            0x00000800
#define HOT_KEY_MENU            0x00001000
#define HOT_KEY_SYSREQ          0x00002000
#define MODIFIER_DUAL_KEYS 			0x000000FF          //For handling any of the two dual(ctrl, alt, shift and logo) keys pressed
#define RIGHT_LEFT_SHIFT_PRESSED	0x00000003
#define RIGHT_LEFT_CONTROL_PRESSED	0x0000000C
#define RIGHT_LEFT_ALT_PRESSED		0x00000030
#define RIGHT_LEFT_LOGO_PRESSED		0x000000C0
BOOLEAN CheckAdvShiftState (UINT32 AmiKeyShiftState, UINT32 CondShiftState);
extern INTN  gHotKeyBootOption;
VOID    SetBootOptionSupportVariable (UINT32);
BOOLEAN MetaiMatch (VOID *,IN CHAR16 *, IN CHAR16 *);
EFI_STATUS InitUnicodeCollectionProtocol (VOID **);

#define RFC_3066_ENTRY_SIZE (42 + 1)
#define GLYPH_WIDTH 8
#define GLYPH_HEIGHT 19
#define NG_SIZE 	19
#define WG_SIZE		38

#define MINI_SETUP_RESOURCE_SECTION_GUID \
	{0x97e409e6, 0x4cc1, 0x11d9, 0x81, 0xf6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL EFI_UGA_PIXEL;

extern EFI_BOOT_SERVICES *gBS;
extern EFI_RUNTIME_SERVICES	*gRT;
extern /*EFI_HII_HANDLE*/VOID* gHiiHandle;

#if !TSE_APTIO_5_SUPPORT
EFI_GUID gEfiHiiFontProtocolGuid = EFI_HII_FONT_PROTOCOL_GUID;
#endif
// EFI_GUID gEfiHiiDatabaseProtocolGuid = EFI_HII_DATABASE_PROTOCOL_GUID;
// EFI_GUID gEfiHiiStringProtocolGuid = EFI_HII_STRING_PROTOCOL_GUID;

EFI_HII_STRING_PROTOCOL *gHiiString;
EFI_HII_DATABASE_PROTOCOL *gHiiDatabase;
EFI_HII_FONT_PROTOCOL *gHiiFont;

extern EFI_GUID gEfiGlobalVariableGuid;
CHAR8 *GlyphWidthCache = NULL;

CHAR8 *_GetSupportedLanguages(EFI_HII_HANDLE HiiHandle);
VOID _GetNextLanguage(CHAR8 **LangCode, CHAR8 *Lang);
EFI_STATUS UefiHiiWrapperGetString( EFI_HII_HANDLE handle, CHAR8 *Language, EFI_STRING_ID StringId, CHAR16 *buffer,UINTN *bufferSize );

extern BOOLEAN gBrowserCallbackEnabled;
BOOLEAN gEnableDrvNotification = FALSE; //TRUE if allow notification function to process action, FALSE to ignore the notification

extern VOID *EfiLibAllocatePool(IN UINTN AllocationSize);
extern EFI_STATUS HiiExtendedInitializeProtocol();
extern CHAR8* StrDup16to8(CHAR16 *String);
extern VOID *VarGetNvram( UINT32 variable, UINTN *size );
extern INTN EfiAsciiStrCmp(IN CHAR8 *String, IN CHAR8 *String2);
extern CHAR8 *StrDup8( CHAR8 *string );
extern EFI_STATUS ReadImageResource(EFI_HANDLE ImageHandle, EFI_GUID *pGuid,
					VOID **ppData, UINTN *pDataSize);
extern EFI_GUID gEfiShellFileGuid;
BOOLEAN AdvancedRepairSupported (VOID);
VOID DriverHealthSystemReset (VOID);
VOID ReInstallBgrt();
CHAR16 *DefaultPasswordFromToken(CHAR16 * PasswordString);
BOOLEAN   IsTseCachePlatformLang (VOID);
BOOLEAN IsSetupPrintEvalMessage();
// Typecast EFI_KEY_OPTION to make it compatible Aptio 4 and 5
/*
typedef struct {
  ///
  /// Specifies options about how the key will be processed.
  ///
  HOT_KEY_EFI_KEY_DATA  KeyData;
  ///
  /// The CRC-32 which should match the CRC-32 of the entire EFI_LOAD_OPTION to
  /// which BootOption refers. If the CRC-32s do not match this value, then this key
  /// option is ignored.
  ///
  UINT32             BootOptionCrc;
  ///
  /// The Boot#### option which will be invoked if this key is pressed and the boot option
  /// is active (LOAD_OPTION_ACTIVE is set).
  ///
  UINT16             BootOption;
  ///
  /// The key codes to compare against those returned by the
  /// EFI_SIMPLE_TEXT_INPUT and EFI_SIMPLE_TEXT_INPUT_EX protocols.
  /// The number of key codes (0-3) is specified by the EFI_KEY_CODE_COUNT field in KeyOptions.
  ///
  //EFI_INPUT_KEY      Keys[];
} AMI_EFI_KEY_OPTION;
*/

CHAR16  gPlatformLang [20];			
CHAR16  gCachePlatformLang[20];//To Save the Current PlatformLang for Cache purpose.
/**

    @param 

    @retval 

**/
CHAR8 *_GetSupportedLanguages (EFI_HII_HANDLE HiiHandle)
{
  EFI_STATUS  status;
  UINTN       BufferSize;
  CHAR8       *LanguageString;

  status = HiiInitializeProtocol ();

  //
  // Collect current supported Languages for given HII handle
  //
  BufferSize = 0x1000;
  LanguageString = (CHAR8*)EfiLibAllocatePool (BufferSize);
  status = gHiiString->GetLanguages (gHiiString, HiiHandle, LanguageString, &BufferSize);
  if(status == EFI_BUFFER_TOO_SMALL)
  {
    MemFreePointer((VOID**)&LanguageString);
    LanguageString = (CHAR8*)EfiLibAllocatePool (BufferSize);
    status = gHiiString->GetLanguages (gHiiString, HiiHandle, LanguageString, &BufferSize);
  }

  if(EFI_ERROR(status))
  {
    LanguageString = NULL;
  }

  return LanguageString;
}

VOID _GetNextLanguage(CHAR8 **LangCode, CHAR8 *Lang)
{
  UINTN  Index;
  CHAR8  *StringPtr;

  if (LangCode == NULL || *LangCode == NULL)
  {
    *Lang = 0;
    return;
  }

  Index = 0;
  StringPtr = *LangCode;
  while (StringPtr[Index] != 0 && StringPtr[Index] != ';')
  {
    Index++;
  }

  MemCopy(Lang, StringPtr, Index);
  Lang[Index] = 0;

  if (StringPtr[Index] == ';')
  {
    Index++;
  }
  *LangCode = StringPtr + Index;
}

/**

    @param 

    @retval EFI_STATUS
        EFI_SUCCESS
**/
EFI_STATUS Uefi21HiiInitializeProtocol(VOID)
{
	EFI_STATUS Status = EFI_SUCCESS;

	Status = gBS->LocateProtocol(&gEfiHiiFontProtocolGuid, NULL, (VOID**)&gHiiFont);
	if(EFI_ERROR(Status))
	{
		goto DONE;
	}
	Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&gHiiDatabase);
	if(EFI_ERROR(Status))
	{
		goto DONE;
	}
	Status = gBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL,(VOID**) &gHiiString);
	if(EFI_ERROR(Status))
	{
		goto DONE;
	}
	Status = HiiExtendedInitializeProtocol();

DONE:
	return Status;
}

CHAR8 *
EFIAPI
GetBestLanguage (
  IN CONST CHAR8  *SupportedLanguages, 
  IN BOOLEAN      Iso639Language,
  ...
  );

UINT16 Uefi21HiiChangeStringLanguage(VOID* handle, UINT16 token, CHAR16 *lang, CHAR16 *string)
{
	EFI_STATUS		status = EFI_SUCCESS;
	UINT16			newToken = INVALID_TOKEN;
	CHAR8			*passedLang = (CHAR8 *)NULL;
	CHAR8			*Languages = (CHAR8 *)NULL;
	CHAR8			*LangStrings = (CHAR8 *)NULL;
	//CHAR8			Lang[RFC_3066_ENTRY_SIZE];
	CHAR8			*Lang = (CHAR8 *)NULL;

	status = HiiInitializeProtocol();

	if((EFI_ERROR(status)) || (string == NULL))	
	{
		return (UINT16)INVALID_TOKEN;
	}

	passedLang = StrDup16to8(lang);

	Languages = _GetSupportedLanguages(handle);
	if(Languages == NULL)
		return INVALID_TOKEN;

	//
	//Iterate through the languages supported by package list to see whether passed language is supported. If yes, add/change the string. 
	//
	LangStrings = Languages;
	if (*LangStrings != 0)
	{
		Lang = GetBestLanguage(LangStrings, FALSE, passedLang, NULL );
//		_GetNextLanguage (&LangStrings, Lang);
		if(token)
		{
			status = gHiiString->SetString(gHiiString, handle, token, Lang, string, NULL);
			newToken = token;
		}
		else
		{
			status = gHiiString->NewString(gHiiString, handle, &newToken, Lang, NULL, string, NULL);
		}

		if(EFI_ERROR(status))
		{
			newToken = INVALID_TOKEN;
		}
	}
	else
		newToken = INVALID_TOKEN;

	MemFreePointer ((VOID **)&passedLang);
    MemFreePointer ((VOID **)&Languages);
    MemFreePointer ((VOID **)&Lang);
	return newToken;
}


#define TSE_TO_STRING(a) #a
VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  );
EFI_STATUS HiiLibGetStringEx(
	IN EFI_HII_HANDLE HiiHandle, IN UINT16 StringId, IN CHAR8 *Language,
	IN OUT UINTN *StringSize, OUT EFI_STRING String
);
VOID EfiStrCpy (IN CHAR16   *Destination, IN CHAR16   *Source);
#define EfiCopyMem(_Destination, _Source, _Length)  gBS->CopyMem((_Destination), (_Source), (_Length))
#define EfiZeroMem(_Destination, _Length)  gBS->SetMem((_Destination), (_Length), 0)
  
/**
    Returns the string for incoming token.
    If lang is valid then return string for this lang
    else return string for PlatformLang variable
    else return english string - default

    @param Handle, string token and lang code

    @retval Sting buffer

**/
CHAR16 *Uefi21HiiGetStringLanguage(EFI_HII_HANDLE handle, UINT16 token, CHAR16 *lang)
{
	EFI_STATUS status;
	CHAR16 *buffer = NULL;
	UINTN bufferSize = 0,Size=0;
//  UINT8 *language = NULL;
 CHAR8 *language = NULL;

	status = HiiInitializeProtocol();
	if ( EFI_ERROR(status) )
	{
		return buffer;
	}

		/* This is a Quick Fix. We need to get the Default Languag here. */
	if(lang)
		language = StrDup16to8(lang);
	else
	{
		
		//language = VarGetNvramName( L"PlatformLang", &gEfiGlobalVariableGuid, NULL, &Size );
		if (*gPlatformLang)		//gPlatformLang will filled in mainsetuploop only if any call comes before it get from NVRAM
		{
			language = EfiLibAllocateZeroPool (20 * sizeof (CHAR16));
			if (NULL != language)
			{
				EfiStrCpy ((CHAR16 *)language, gPlatformLang);
			}
		}
		else
		{
			language = VarGetNvramName( L"PlatformLang", &gEfiGlobalVariableGuid, NULL, &Size );
			if (NULL != language)
			{
 	      	EfiZeroMem (gPlatformLang, sizeof (gPlatformLang));
		   	EfiCopyMem (gPlatformLang, language, Size);
			}
		}
				
	}
	if(NULL != language)
	{
		if ( handle == INVALID_HANDLE )
			handle = gHiiHandle;	  
		status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);  
	
	    if(status == EFI_INVALID_PARAMETER)
		{
			buffer = (CHAR16*)EfiLibAllocateZeroPool(sizeof(CHAR16));
			if(buffer == NULL)
			{
				return buffer;
			}
			status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);
		}
	}
    else 
    {
    	TRACE((-1,"\n[TSE] PlatformLang is not defined, falling back to DEFAULT_LANGUAGE_CODE, and then en-US \n"));
    	status = EFI_NOT_FOUND;
    }
	    	
	// String not found with current Lang or PlatfromLang is not defined
	if (( EFI_ERROR(status) ) && (bufferSize==0))
	{
		MemFreePointer( (VOID **)&language );
		language = StrDup8(CONVERT_TO_STRING(DEFAULT_LANGUAGE_CODE));
		status = UefiHiiWrapperGetString(handle, language,token ,buffer, &bufferSize );
		// String not found in Default Language
		if (( EFI_ERROR(status) )&&(bufferSize==0))
		{
			if(EfiAsciiStrCmp(language, "en-US")!=0)
			{
				MemFreePointer( (VOID **)&language );
				language = StrDup8("en-US");
				status = UefiHiiWrapperGetString(handle, language,token ,buffer, &bufferSize );
				// string not found in English
				if (( EFI_ERROR(status) )&&(bufferSize==0))
				{
					MemFreePointer( (VOID **)&language );
					return buffer;
				}
			}
			else
			{
				MemFreePointer( (VOID **)&language );
				return buffer;
			}
		}
		if (EFI_BUFFER_TOO_SMALL == status)
		{
		    buffer = (CHAR16*)EfiLibAllocateZeroPool(bufferSize);
		    if(buffer == NULL)
		    {
		      return buffer;
		    }
			status = UefiHiiWrapperGetString (handle, language, token, buffer, &bufferSize );
			if (!EFI_ERROR (status))
			{
				MemFreePointer( (VOID **)&language );
				return buffer;
			}
		}
	}
	buffer = (CHAR16*)EfiLibAllocatePool( bufferSize );
	if ( buffer == NULL )
		return buffer;
	
	status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);


	if ( EFI_ERROR(status) )
		MemFreePointer( (VOID **)&buffer );
	MemFreePointer( (VOID **)&language );

	return buffer;
}

EFI_STATUS UefiHiiWrapperGetString( EFI_HII_HANDLE handle, CHAR8 *Language, EFI_STRING_ID StringId, CHAR16 *buffer,UINTN *bufferSize )
{
	if ( handle == INVALID_HANDLE )
		handle = gHiiHandle;

	return gHiiString->GetString(gHiiString, Language, handle, StringId, buffer, bufferSize, NULL);
}

#if TSE_USE_EDK_LIBRARY

#if APTIO_4_00 || SETUP_USE_GUIDED_SECTION

EFI_STATUS LoadStrings(
	EFI_HANDLE ImageHandle, VOID * *pHiiHandle
)
{
	static EFI_GUID guidResourceSection= MINI_SETUP_RESOURCE_SECTION_GUID;
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_LOADED_IMAGE_PROTOCOL *pImage;
	RESOURCE_SECTION_HEADER *pSection;
	UINTN SectionSize;
	EFI_HII_PACKAGE_LIST_HEADER *pPackages = NULL;
	EFI_HII_PACKAGE_HEADER *pPack;

	Status = HiiInitializeProtocol();
	if (EFI_ERROR(Status))
		goto DONE;

	Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, &pImage);
	if (EFI_ERROR(Status))
		goto DONE;

	Status = ReadImageResource(ImageHandle,&guidResourceSection,&pSection,&SectionSize);
	if (EFI_ERROR(Status))
		goto DONE;

	pPack = (EFI_HII_PACKAGE_HEADER*)(pSection+1);
	if (pPack->Type==EFI_HII_PACKAGE_STRINGS)
	{
		pPackages = EfiLibAllocateZeroPool(sizeof(EFI_HII_PACKAGE_LIST_HEADER) +
			pPack->Length);
		pPackages->PackageListGuid = ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)(pImage->FilePath))->NameGuid;
		pPackages->PackageLength = sizeof(EFI_HII_PACKAGE_LIST_HEADER) + pPack->Length;
		MemCopy(pPackages + 1, pPack, pPack->Length);
		Status = gHiiDatabase->NewPackageList(gHiiDatabase, pPackages, ImageHandle, (EFI_HII_HANDLE*)pHiiHandle);
	}
	else
	{
		Status = EFI_INVALID_PARAMETER;
	}
	MemFreePointer((VOID **)&pPackages);
	gBS->FreePool(pSection);

DONE:
	return Status;
}
#endif

#endif //#if TSE_USE_EDK_LIBRARY

UINTN Uefi21HiiGetGlyphWidth(VOID)
{
	return GLYPH_WIDTH;
}

UINTN Uefi21HiiGetGlyphHeight(VOID)
{
	return GLYPH_HEIGHT;
}

//----------------------------------------------------------------------------
// Patch to use UnicodeCollation until
// Aptio supports UnicodeCollation2 protocol
//----------------------------------------------------------------------------
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation)
#if UEFI_2_1_SUPPORT 
#ifndef TSE_FOR_APTIO_4_50
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation2)
#endif
#endif

EFI_STATUS GetUnicodeCollection2Protocol(VOID **Protocol)
{
	EFI_STATUS Status = EFI_SUCCESS;

	Status = gBS->LocateProtocol(
                &gEfiUnicodeCollation2ProtocolGuid,
                NULL,
                Protocol);
//----------------------------------------------------------------------------
// Patch to use UnicodeCollation until
// Aptio supports UnicodeCollation2 protocol
//----------------------------------------------------------------------------
	if(EFI_ERROR(Status))
	{
		Status = gBS->LocateProtocol(
                &gEfiUnicodeCollationProtocolGuid,
                NULL,
                Protocol);
	}

	return Status;
}
CHAR16 *TseSkipEscCode(CHAR16 *String);
UINTN EfiStrLen(IN CHAR16 *String);
/**
    function to get the printable lenght of the string

    @param string 

    @retval Size of string

**/
UINTN UefiHiiTestPrintLength ( IN CHAR16   *String )
{

	UINTN Size = 0;

	if(String == NULL)
	{
		goto DONE;
	}

#if !TSE_USE_GETGYLPH_PRINTLENGH
	{
		EFI_STATUS Status = EFI_SUCCESS;
		EFI_IMAGE_OUTPUT *Blt = NULL;
		UINTN BltX = 0;
		UINTN BltY = 0;
		UINT32 Flag = EFI_HII_OUT_FLAG_WRAP;

		// Skip Esc Characters from String
		String = (UINT16* )TseSkipEscCode(String);
	
		Status = gHiiFont->StringToImage(gHiiFont, Flag, String, NULL, &Blt, BltX, BltY, NULL, NULL, NULL);
		if(EFI_ERROR(Status))
		{
			if(String != NULL)
				gBS->FreePool( String );
			goto DONE;
		}
		if(Blt)
		{
			Size = Blt->Width * Blt->Height;
			Size = Size/GLYPH_WIDTH;
            Size = Size/(Blt->Height/EFI_GLYPH_HEIGHT); 
			MemFreePointer(&Blt->Image.Bitmap);
			MemFreePointer(&Blt);
		}

		if(Size==0)
			Size = (EfiStrLen(String)*NG_SIZE);

		if(String != NULL)
			gBS->FreePool( String );
	}
#else // Non Aptio case with EDK Nt32 emulation
	{
		EFI_STATUS Status = EFI_SUCCESS;
		UINTN StringLen = 0;
		UINT32 Index = 0;

  	    if (GlyphWidthCache == NULL) {
  		    // Allocate a 64k buffer to store the widths of each possible CHAR16 glyph
  		    GlyphWidthCache = EfiLibAllocateZeroPool(65536); 
  		    if (GlyphWidthCache == NULL)
  			    return 0;
  	    }
  	    
  	  // Skip Esc Characters from String
  	    if(gSkipEscCode)
  	        String = (UINT16* )TseSkipEscCode(String);
      		
  	    StringLen = EfiStrLen(String);
  	    while(Index < StringLen) //Get the glyph of each character
  	    {
  		    UINTN Baseline = 0;
  		    CHAR16 Character = String[Index];
      		
  		    if (*(GlyphWidthCache+Character) == 0) {
  			    EFI_IMAGE_OUTPUT *Blt = NULL;
      
				Status = gHiiFont->GetGlyph(gHiiFont, Character, NULL, &Blt, &Baseline);
				if(EFI_ERROR(Status) && Status != EFI_WARN_UNKNOWN_GLYPH)
				{
					goto DONE;
				}
  			    if(Blt == NULL)
  			    {
  				    Character = L' ';
  				    gHiiFont->GetGlyph(gHiiFont, Character, NULL, &Blt, &Baseline);
  			    }
  			    *(GlyphWidthCache+Character) = (CHAR8) Blt->Width;
  			    if(Blt)
  			    {
  				    MemFreePointer((VOID**)&Blt->Image.Bitmap);
  				    MemFreePointer((VOID**)&Blt);
  			    }			
  		    }
      
  		    Size += *(GlyphWidthCache+Character);
  		    Index++;
  	    }
  	    
  	    if(String != NULL && gSkipEscCode)
  	       gBS->FreePool( String );
  	    
  	    Size = ((Size/GLYPH_WIDTH)*NG_SIZE);
	}
#endif

DONE:

	return Size;
}

/**
    function to get the Blt for the message with the given attributes

    @param Message EFI_UGA_PIXEL Foreground, EFI_UGA_PIXEL Background, 
    @param Width OUT EFI_UGA_PIXEL **BltBuffer

    @retval Size of string

**/
EFI_STATUS HiiString2BltBuffer(	CHAR16 *Message,
								EFI_UGA_PIXEL Foreground, 
								EFI_UGA_PIXEL Background, 
								OUT	UINTN *Width,
								OUT EFI_UGA_PIXEL **BltBuffer,
								OUT UINTN *BltGlyphWidth)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_IMAGE_OUTPUT *Blt = NULL;
  EFI_FONT_DISPLAY_INFO FontDisplayInfo={0};
  UINTN BltX = 0;
  UINTN BltY = 0;
  UINT32 Flag = EFI_HII_OUT_FLAG_WRAP;
  EFI_HII_ROW_INFO *RowInfo = NULL;

  // Get the Font Display Info for use in StringToImage
  FontDisplayInfo.ForegroundColor = Foreground;
  FontDisplayInfo.BackgroundColor = Background;

  // Get the String To Image Blt Buffer
  Status = gHiiFont->StringToImage (gHiiFont, Flag, Message, &FontDisplayInfo, &Blt, BltX, BltY, &RowInfo, NULL, NULL);	//Get the width from row info
  if(EFI_ERROR(Status))
  {
	  Blt = (EFI_IMAGE_OUTPUT *)NULL;											//Avoiding freeing the memory in DONE
	  RowInfo = (EFI_HII_ROW_INFO *)NULL;
    goto DONE;
  }
  if (Blt->Image.Bitmap)
	  MemFreePointer ((VOID **)&Blt->Image.Bitmap);

  if(Blt)
    MemFreePointer ((VOID **)&Blt);
  
  Blt = EfiLibAllocateZeroPool (sizeof (EFI_IMAGE_OUTPUT));
  if (NULL == Blt)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }  
  Blt->Image.Bitmap = EfiLibAllocateZeroPool (RowInfo->LineWidth  * RowInfo->LineHeight * sizeof  (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (NULL == Blt->Image.Bitmap)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }  
  Blt->Width = (UINT16)RowInfo->LineWidth;
  Blt->Height = (UINT16)RowInfo->LineHeight;
  Status = gHiiFont->StringToImage (gHiiFont, Flag, Message, &FontDisplayInfo, &Blt, BltX, BltY, NULL, NULL, NULL);
  if(EFI_ERROR(Status))
  {
    goto DONE;
  }
  *BltBuffer = Blt->Image.Bitmap;
  *BltGlyphWidth = *Width = RowInfo->LineWidth;

  // Free Blt. Only Blt is freed. The Bitmap memory is not freed.
DONE:
	if(Blt)
		MemFreePointer((VOID **)&Blt);
	if(RowInfo)
		MemFreePointer((VOID **)&RowInfo);
  return Status;
}

/**
    Clears the boot key gloabl details

    @param VOID

    @retval VOID

**/
VOID RefreshBootKeysDetails (VOID)
{
    gHotKeysCount = 0;
    gAllocateKeysCount = 0;
    MemFreePointer ((VOID **)&gHotKeysDetails);
}

/**
    Function to form the boot option hot keys

    @param CHAR16 * = Hot key name to retrieve the hot key details

    @retval VOID

**/
#ifdef EFI_KEY_OPTION_REVISION_MASK
#define KEY_DATA_INPUT_KEY_COUNT(KeyData) ((KeyData) >> 30)
#define KEY_DATA_VALUE(KeyData) (KeyData)
#else
#define KEY_DATA_INPUT_KEY_COUNT(KeyData) (KeyData).Options.InputKeyCount
#define KEY_DATA_VALUE(KeyData) (KeyData).PackedValue
#endif
VOID FormHotBootKeys (CHAR16 *VarName)
{
    UINT8           Keycount = 0;
    EFI_INPUT_KEY   *InputKey;
//    HOT_KEYS        *ParseKey = NULL; Not used
    UINTN		    size=0;
    UINT32          CRC32 = 0;
    EFI_KEY_OPTION  *HotKeyBuff = NULL;

    
    if (gHotKeysCount >= gAllocateKeysCount)
    {
        gHotKeysDetails = MemReallocateZeroPool (gHotKeysDetails, gHotKeysCount * sizeof (HOT_KEYS), ((gAllocateKeysCount + HOTKEY_ALLOC_UNIT) * sizeof (HOT_KEYS)) );
        if (NULL == gHotKeysDetails)
        {
            return;
        }
        gAllocateKeysCount = gAllocateKeysCount + HOTKEY_ALLOC_UNIT;
    }
    HotKeyBuff = VarGetNvramName (VarName, &gEfiGlobalVariableGuid, NULL, &size);
    if (NULL == HotKeyBuff)
    {
        return;
    }
    Keycount = KEY_DATA_INPUT_KEY_COUNT(HotKeyBuff->KeyData);
    CRC32 = CheckBootOptionMatch (HotKeyBuff->BootOption);
    if ((!CRC32) || (CRC32 != HotKeyBuff->BootOptionCrc))             //For failure case CRC32 will return 0
    {
        return;
    }
    gHotKeysDetails [gHotKeysCount].KeyData.KeyState.KeyShiftState = SHIFT_STATE_VALID;         //Making valid shift state
    if (KEY_DATA_VALUE(HotKeyBuff->KeyData) & HOT_KEY_SHIFT)              //Checking for shift(special) keys, if anything found in data then we have to give the value for both the shifts/ctrls etc.,
    {
        gHotKeysDetails [gHotKeysCount].KeyData.KeyState.KeyShiftState |= RIGHT_LEFT_SHIFT_PRESSED;
    }
    if (KEY_DATA_VALUE(HotKeyBuff->KeyData) & HOT_KEY_CTRL)
    {
        gHotKeysDetails [gHotKeysCount].KeyData.KeyState.KeyShiftState |= RIGHT_LEFT_CONTROL_PRESSED;
    }
    if (KEY_DATA_VALUE(HotKeyBuff->KeyData) & HOT_KEY_ALT)
    {
        gHotKeysDetails [gHotKeysCount].KeyData.KeyState.KeyShiftState |= RIGHT_LEFT_ALT_PRESSED;
    }
    if (KEY_DATA_VALUE(HotKeyBuff->KeyData) & HOT_KEY_LOGO)
    {
        gHotKeysDetails [gHotKeysCount].KeyData.KeyState.KeyShiftState |= RIGHT_LEFT_LOGO_PRESSED;
    }
    if (KEY_DATA_VALUE(HotKeyBuff->KeyData) & HOT_KEY_MENU)
    {
        gHotKeysDetails [gHotKeysCount].KeyData.KeyState.KeyShiftState |= MENU_KEY_PRESSED;
    }
    if (KEY_DATA_VALUE(HotKeyBuff->KeyData) & HOT_KEY_SYSREQ)
    {
        gHotKeysDetails [gHotKeysCount].KeyData.KeyState.KeyShiftState |= SYS_REQ_PRESSED;
    }
    gHotKeysDetails [gHotKeysCount].BootOption = HotKeyBuff->BootOption;
    gHotKeysDetails [gHotKeysCount].BootOptionCrc = HotKeyBuff->BootOptionCrc;

    while (Keycount)
    {                                                       //Since the values are ORED the values may read at reverse also
        InputKey = (EFI_INPUT_KEY *)((UINT8 *)HotKeyBuff + (sizeof (EFI_KEY_OPTION) + ((Keycount-1) * sizeof (EFI_INPUT_KEY))));
        gHotKeysDetails [gHotKeysCount].KeyData.Key.ScanCode |= InputKey->ScanCode;
        gHotKeysDetails [gHotKeysCount].KeyData.Key.UnicodeChar |= InputKey->UnicodeChar;
        Keycount --;
    }
    gHotKeysCount ++;
}

/**
    Function to free the extra memories allocated

    @param VOID

    @retval VOID

**/
VOID FreeExtraKeyMemories (VOID)
{
    if (gHotKeysCount < gAllocateKeysCount)
    {
        gHotKeysDetails = MemReallocateZeroPool (gHotKeysDetails, gAllocateKeysCount * sizeof (HOT_KEYS), gHotKeysCount * sizeof (HOT_KEYS));
    }
}

/**
    Function to form the hot keys from NVRAM

    @param VOID

    @retval VOID

**/
VOID FindandSetHotKeys (VOID)
{
#ifndef STANDALONE_APPLICATION
    CHAR16		KeyPattern [] = L"Key[0-9a-f][0-9a-f][0-9a-f][0-9a-f]";
    UINTN		VarNameSize = 80;
    CHAR16		*VarName = NULL;
    EFI_GUID	VarGuid;
    UINTN		OldVarNameSize = 0;
    EFI_STATUS  Status = EFI_SUCCESS;
    VOID *		UnicodeInterface;
    
    OldVarNameSize = VarNameSize;
    VarName = EfiLibAllocateZeroPool(VarNameSize);
    if (NULL == VarName)
    {
        return;
    }
    RefreshBootKeysDetails ();
    SetBootOptionSupportVariable (EFI_BOOT_OPTION_SUPPORT_KEY|EFI_BOOT_OPTION_SUPPORT_APP);            //Indicates boot manager supports hot key boot
    Status = InitUnicodeCollectionProtocol (&UnicodeInterface);
    do
    { 
        Status = gRT->GetNextVariableName (&VarNameSize,
                    VarName,
                    &VarGuid);
        if (EFI_BUFFER_TOO_SMALL == Status)
        {
            //Allocate correct size
            VarName = MemReallocateZeroPool (
                        VarName,
                        OldVarNameSize,
                        VarNameSize);
            OldVarNameSize = VarNameSize;
            continue;
        }
        if (VarNameSize == ((EfiStrLen(L"keyXXXX")+1)*2))
        {
            if (MetaiMatch (UnicodeInterface, VarName, KeyPattern))
            {
                FormHotBootKeys (VarName);
            }
        }
        VarNameSize = OldVarNameSize;
    }while (EFI_NOT_FOUND != Status);
    FreeExtraKeyMemories ();
    MemFreePointer ((VOID **)&VarName);
#endif
}

/**
    Checks for the key pressed by user matches the boot key options

    @param AMI_EFI_KEY_DATA = Input key details pressed by user
        UINT16 * = Boot index to return; Memory should be allocated by user              

    @retval BOOLEAN TRUE = If boot option found for user pressed key
        FALSE = If boot index not found for user pressed key

**/
BOOLEAN CheckforHotKey (AMI_EFI_KEY_DATA InputKey)
{
    UINTN		Count = 0;
//    UINT32      CRC32 = 0;

    while (Count < gHotKeysCount)
    {
        if ( (gHotKeysDetails [Count].KeyData.Key.UnicodeChar == InputKey.Key.UnicodeChar)
				&&  (gHotKeysDetails [Count].KeyData.Key.ScanCode == InputKey.Key.ScanCode)
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
				&& (CheckAdvShiftState (InputKey.KeyState.KeyShiftState, gHotKeysDetails [Count].KeyData.KeyState.KeyShiftState))
#endif
        )
        {
            gHotKeyBootOption = (INTN)gHotKeysDetails->BootOption;
            return TRUE;
        }
        Count ++;
    }
    return FALSE;
}

/**
    Finds all agent handle which opens the input handle upto the last agent for the controller

    @param EFI_HANDLE = Handle to be examined
        EFI_HANDLE = Handle to be matched

    @retval BOOLEAN => If handle matches return TRUE else FALSE

**/
BOOLEAN FindtheMatchUptoEndNode (EFI_HANDLE Handle, EFI_HANDLE DPHandle)
{
    EFI_GUID    **ppGuid = NULL;
    UINTN		Count = 0;
    UINTN		iIndex = 0;
    UINTN		jIndex = 0;
//    VOID        *Interface = NULL; Not used
    UINTN       EntryCount = 0;
    EFI_STATUS  Status = EFI_SUCCESS;
    BOOLEAN     RetStatus = FALSE;    
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *EntryBuffer = NULL;
    
    Status = gBS->ProtocolsPerHandle (Handle, &ppGuid, &Count);
    if (EFI_ERROR (Status) )
    {
        return FALSE;
    }
    for (iIndex = 0; (iIndex < Count && !RetStatus); iIndex ++)
    {
        Status = gBS->OpenProtocolInformation (Handle, ppGuid [iIndex], &EntryBuffer, &EntryCount);
        if (EFI_ERROR (Status) )
        {
            continue;
        }
        for (jIndex = 0; (jIndex < EntryCount && !RetStatus); jIndex ++)
        {
            if (!(EntryBuffer [jIndex].Attributes & (EFI_OPEN_PROTOCOL_BY_DRIVER|EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)))
            {
                continue;
            }
            if ((EntryBuffer [jIndex].AgentHandle == DPHandle) || (EntryBuffer [jIndex].ControllerHandle == DPHandle) )
            {
                RetStatus = TRUE;
                break;
            }
            // It is possible for OpenProtocolInformation to return the handle it was called for
            // in the AgentHandle. We should go on to the next handle in that case and not make
            // FindtheMatchUptoEndNode()
     		if ( EntryBuffer [jIndex].AgentHandle == Handle )
            {
	        	continue;
            }
            RetStatus = FindtheMatchUptoEndNode (EntryBuffer [jIndex].AgentHandle, DPHandle);
        }
        MemFreePointer ((VOID **)&EntryBuffer);
    }
    MemFreePointer ((VOID **)&ppGuid);
    return RetStatus;
}           

/**
    Checks whether the input handle has the driver health support

    @param EFI_HANDLE = Handle to be examined

    @retval BOOLEAN => TRUE if input handle has the driver health support else FALSE

**/ 
BOOLEAN MatchDPHandleWithDrvHlthHndls (EFI_HANDLE DPHandle)
{
	EFI_STATUS  Status;
	UINTN       NumHandles = 0;
	UINTN		Count = 0;
    UINTN       EntryCount = 0;
	EFI_HANDLE  *DriverHealthHandles = NULL;
	UINTN       iIndex = 0, jIndex = 0, kIndex = 0;
    EFI_GUID    **ppGuid = NULL;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *EntryBuffer = NULL;
    EFI_GUID EfiDriverHealthProtocolGuid = EFI_DRIVER_HEALTH_PROTOCOL_GUID;
    TRACE ((-1,"\n[TSE]Entering MatchDPHandleWithDrvHlthHndls()"));
	Status = gBS->LocateHandleBuffer (
				ByProtocol,
				&EfiDriverHealthProtocolGuid,
				NULL,
				&NumHandles,
				&DriverHealthHandles
				);
	TRACE ((-1,"\n[TSE]MatchDPHandleWithDrvHlthHndls():gBS->LocateHandleBuffer Status:%r   No of handles:%X",Status,NumHandles));
	if (EFI_ERROR (Status))
	{
		return FALSE;
	}
	for (iIndex = 0; iIndex < NumHandles; iIndex ++)
	{
        Status = gBS->ProtocolsPerHandle (DriverHealthHandles [iIndex], &ppGuid, &Count);
        if (EFI_ERROR (Status) )
        {
            return FALSE;
        }
        for (jIndex = 0; jIndex < Count; jIndex++)
    	{
            Status = gBS->OpenProtocolInformation (DriverHealthHandles [iIndex], ppGuid [jIndex], &EntryBuffer, &EntryCount);
            if (EFI_ERROR (Status) )
            {
                continue;
            }
            for (kIndex = 0; kIndex < EntryCount; kIndex ++)
            {
                if (!(EntryBuffer [kIndex].Attributes & (EFI_OPEN_PROTOCOL_BY_DRIVER|EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)) )
                {
                    continue;
                }
                if ((EntryBuffer [kIndex].AgentHandle == DPHandle) || (EntryBuffer [kIndex].ControllerHandle == DPHandle))
                {
                    MemFreePointer ((VOID **)&DriverHealthHandles);
                    return TRUE;
                }
                if (TRUE == FindtheMatchUptoEndNode (EntryBuffer [kIndex].AgentHandle, DPHandle) ) //ControllerHandle is NULL here
                {
                    MemFreePointer ((VOID **)&DriverHealthHandles);   
                    return TRUE;
                }
            }
            MemFreePointer ((VOID **)&EntryBuffer);
        }
        MemFreePointer ((VOID **)&ppGuid);
    }
	 MemFreePointer ((VOID **)&DriverHealthHandles);
	 TRACE((-1,"\n[TSE]Exiting MatchDPHandleWithDrvHlthHndls()\n"));
    return FALSE;
}

/**
    Repairs the driver for the input device path protocol

    @param EFI_DEVICE_PATH_PROTOCOL * = Device path for the handle to which repair operation to be done

    @retval VOID

**/
EFI_STATUS CheckForDeviceNeedRepair (EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
 //   UINTN       EntryCount = 0; Not used
//    UINTN       iIndex = 0;
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_HANDLE  DevHandle = NULL;
    EFI_HANDLE  FormHiiHandle = NULL;            
    EFI_DEVICE_PATH_PROTOCOL    *TempDevicePath = DevicePath;
	EFI_DRIVER_HEALTH_PROTOCOL	*DrvHealthInstance = NULL;
	EFI_DRIVER_HEALTH_STATUS	HealthStatus = EfiDriverHealthStatusHealthy;
    EFI_FORM_BROWSER2_PROTOCOL  *FormBrowserInterface = NULL;
    EFI_GUID    FormBrowserGuid = EFI_FORM_BROWSER2_PROTOCOL_GUID;
    EFI_GUID    DrvHealthFormsetGuid = EFI_HII_DRIVER_HEALTH_FORMSET_GUID;
    EFI_GUID    EfiDriverHealthProtocolGuid = EFI_DRIVER_HEALTH_PROTOCOL_GUID;
    TRACE((-1,"\n[TSE] Entering CheckForDeviceNeedRepair()"));
    Status = gBS->LocateDevicePath (
                &gEfiDevicePathProtocolGuid,
                &TempDevicePath,
                &DevHandle
                );
    TRACE((-1,"\n[TSE] CheckForDeviceNeedRepair() : LocateDevicePath Status :%r",Status));
	if (EFI_ERROR (Status))
	{
        return Status;
    }
    if (FALSE == MatchDPHandleWithDrvHlthHndls (DevHandle))
    {
   	 TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : MatchDPHandleWithDrvHlthHndls is FALSE For Handle:%X",DevHandle));
        return EFI_UNSUPPORTED;
    }
    Status = gBS->HandleProtocol (DevHandle, &EfiDriverHealthProtocolGuid, (VOID**)&DrvHealthInstance);
	if (EFI_ERROR (Status))
	{
		return Status;
	}
	gEnableDrvNotification = TRUE;      
	gBrowserCallbackEnabled = TRUE;   
	Status = DrvHealthInstance->GetHealthStatus (DrvHealthInstance, DevHandle, NULL, &HealthStatus, NULL, &FormHiiHandle);
	gEnableDrvNotification = FALSE;     
    gBrowserCallbackEnabled = FALSE; 
    TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : DrvHealthInstance->GetHealthStatus1 HealthStatus:%r Status:%r FormHiiHandle:[%X] ControllerHandle:[%X] ChildHandle:[0]",HealthStatus,Status,FormHiiHandle,DevHandle));
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    Status = gBS->LocateProtocol (&FormBrowserGuid, NULL, (VOID**)&FormBrowserInterface);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
DriverRepair:    
    if (EfiDriverHealthStatusRepairRequired == HealthStatus)
    {
   	 TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : HealthStatus :%r",HealthStatus));
        Status = DrvHealthInstance->Repair (
            DrvHealthInstance,
            DevHandle,
            NULL,
            NULL
            );
        if (EFI_ERROR (Status))
        {
      	  TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : DrvHealthInstance->Repair Status:%r ControllerHandle:[%X] ChildHandle:[0]",Status,DevHandle));
            return EFI_UNSUPPORTED;
        }
    }
    if (AdvancedRepairSupported ())
    {
        gEnableDrvNotification = TRUE;      
        gBrowserCallbackEnabled = TRUE;     
        TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : AdvancedRepairSupported() is TRUE "));
        Status = DrvHealthInstance->GetHealthStatus (DrvHealthInstance, DevHandle, NULL, &HealthStatus, NULL, &FormHiiHandle);
        
        gEnableDrvNotification = FALSE;      
        gBrowserCallbackEnabled = FALSE;    
        TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : DrvHealthInstance->GetHealthStatus()2  HealthStatus:%r Status:%r FormHiiHandle:[%X] ControllerHandle:[%X] ChildHandle:[0]",HealthStatus,Status,FormHiiHandle,DevHandle));
        if (!EFI_ERROR (Status))
        {
            if (EfiDriverHealthStatusConfigurationRequired == HealthStatus)
            {
            	TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : HealthStatus: %r",HealthStatus));
                Status = FormBrowserInterface->SendForm (
                            FormBrowserInterface,
                            &FormHiiHandle,
                            1,
                            &DrvHealthFormsetGuid,
                            0,
                            NULL,
                            NULL
                            );
                if (EFI_ERROR (Status))
                {
               	 TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : FormBrowserInterface->SendForm Status:%r For Form handle:%X Guid:%g",Status,FormHiiHandle,DrvHealthFormsetGuid));
                    return Status;
                }
                gEnableDrvNotification = TRUE;       
                gBrowserCallbackEnabled = TRUE;     
                
                Status = DrvHealthInstance->GetHealthStatus (DrvHealthInstance, DevHandle, NULL, &HealthStatus, NULL, &FormHiiHandle);
                TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() : DrvHealthInstance->GetHealthStatus3 HealthStatus:%r Status:%r FormHiiHandle:[%X] ControllerHandle:[%X] ChildHandle:[0]",HealthStatus,Status,FormHiiHandle,DevHandle));
                gEnableDrvNotification = FALSE;     
                gBrowserCallbackEnabled = FALSE;    
                
                if (EFI_ERROR (Status))
                {
                    return Status;
                }
            }
            if ((EfiDriverHealthStatusRepairRequired == HealthStatus) || (EfiDriverHealthStatusConfigurationRequired == HealthStatus))
            {
            	TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() :HealthStatus :%r",HealthStatus));
                goto DriverRepair;
            }
            else if (EfiDriverHealthStatusRebootRequired == HealthStatus)
            {
            	TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() :HealthStatus :%r ",HealthStatus));
                DriverHealthSystemReset ();
            }
            else if (EfiDriverHealthStatusReconnectRequired == HealthStatus)
            {
            	TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() :HealthStatus :%r ",HealthStatus));
                Status = gBS->DisconnectController (DevHandle, NULL, NULL);
                gBS->ConnectController (DevHandle, NULL, NULL, TRUE);
            }
        }
    }
    gEnableDrvNotification = TRUE;      
    gBrowserCallbackEnabled = TRUE;     
    
    Status = DrvHealthInstance->GetHealthStatus (DrvHealthInstance, DevHandle, NULL, &HealthStatus, NULL, &FormHiiHandle);
    TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() :DrvHealthInstance->GetHealthStatus4 HealthStatus:%r Status:%r FormHiiHandle:[%X] ControllerHandle:[%X] ChildHandle:[0]",HealthStatus,Status,FormHiiHandle,DevHandle));
    gEnableDrvNotification = FALSE;     
    gBrowserCallbackEnabled = FALSE;    
    
    if ((!(EFI_ERROR (Status))) && (EfiDriverHealthStatusHealthy == HealthStatus) )
    {
   	 TRACE ((-1,"\n[TSE] CheckForDeviceNeedRepair() :HealthStatus:%r and Status :EFI_SUCCESS",HealthStatus));
        return EFI_SUCCESS;
    }
    TRACE ((-1,"\n[TSE] Exiting CheckForDeviceNeedRepair()\n"));
    return EFI_UNSUPPORTED;
}

/**
    Function to return the PlatformLang

    @param Size 

    @retval PlatformLang String

**/
CHAR16 *ReadPlatformLang( UINTN *Size )
{
	VOID *buffer = NULL;
	if(IsTseCachePlatformLang())
 	{
 		if(*gCachePlatformLang )
 		{
 			*Size=(EfiStrLen(gCachePlatformLang)*sizeof(CHAR16));
 			buffer = EfiLibAllocateZeroPool(*Size+sizeof(CHAR16));
 			EfiCopyMem (buffer, gCachePlatformLang, *Size);
 		}
 		return buffer;
 	}	
	else
	return VarGetNvramName (L"PlatformLang", &gEfiGlobalVariableGuid, NULL, Size);
}

/**
    Loads ESA FV and inits the ESA_INTERFACES_FOR_TSE protocol
               
    @param VOID

    @retval VOID

**/
ESA_INTERFACES_FOR_TSE	*gEsaInterfaceForTSE = (ESA_INTERFACES_FOR_TSE *)NULL;
EFI_STATUS ESAInitFvHook(VOID);
extern UINT16 gDbgPrint ;
VOID UpdateGoPUgaDraw( VOID );
extern BOOLEAN gLaunchOtherSetup;
extern VOID    *gBgrtSafeBuffer;
extern UINTN gTableKey;
EFI_STATUS InitEsaTseInterfaces (void)
{
#if ESA_BINARY_SUPPORT
	EFI_STATUS Status = EFI_SUCCESS;
	extern EFI_GRAPHICS_OUTPUT_PROTOCOL	*gGOP;
	extern UINTN 					gPostStatus;
	extern BOOLEAN					gQuietBoot;
	extern EFI_SYSTEM_TABLE 	*gST;

	if (NULL == gEsaInterfaceForTSE)
	{
		ESAInitFvHook ();
		Status = gBS->LocateProtocol(&gEsaInterfacesForTseProtocolGuid, NULL,(void**) &gEsaInterfaceForTSE);
	}
	if (EFI_ERROR (Status))
	{
		gEsaInterfaceForTSE = (ESA_INTERFACES_FOR_TSE	*)NULL;
	}
	else
	{
#if TSE_BUILD_AS_APPLICATION
		UpdateGoPUgaDraw ();
#endif	
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (TSEHANDLE, 1, &gHiiHandle, 0);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (LANGDATA,1, &gLanguages, &gLangCount);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (GOP, 1, &gGOP, 0);
		if(!gLaunchOtherSetup){
			Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (PostStatus, 1, 0, &gPostStatus);
		}
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (QuietBoot, 1, 0, (UINTN*)&gQuietBoot);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (TSEDEBUG, 1, 0, (UINTN*)&gDbgPrint);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (BOOTDISABLEDOPTION, 1, 0, (UINTN*)&DISABLED_BOOT_OPTION);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (DRIVERDISABLEDOPTION, 1, 0, (UINTN*)&DISABLED_DRIVER_OPTION);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (BgrtSafeBuffer, 1, 0, (UINTN*)&gBgrtSafeBuffer);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (TableKey, 1, 0, (UINTN*)&gTableKey);
		
		gEsaInterfaceForTSE->InitGraphicsLibEntry (gImageHandle, gST);
	}
	return Status;
#else
	return EFI_UNSUPPORTED;
#endif	
}

/**
    Gets default password throug sdl tokens
               
    @param PasswordType 

    @retval CHAR16 * Password 

**/
#define EMPTY_DATA ""
CHAR16 *GetDefaultPasswordFromTokens (UINT32 PasswordType)
{
#if TSE_DEFAULT_SETUP_PASSWORD_SUPPORT
	if (AMI_PASSWORD_ADMIN == PasswordType)
	{
		return  DefaultPasswordFromToken(CONVERT_TO_WSTRING(TSE_ADMIN_PASSWORD));
	}
	if (AMI_PASSWORD_USER == PasswordType)
	{
		return DefaultPasswordFromToken(CONVERT_TO_WSTRING(TSE_USER_PASSWORD));
	}
#endif	
	return NULL;
}
	
/**
    Returns token RT_ACCESS_SUPPORT_IN_HPKTOOL value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   TseRtAccessSupport (VOID)
{
#ifdef RT_ACCESS_SUPPORT_IN_HPKTOOL
	return RT_ACCESS_SUPPORT_IN_HPKTOOL;
#else
	return 0;
#endif
}

/**
    Returns token TSE_BEST_TEXT_GOP_MODE value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsTseBestTextGOPModeSupported (VOID)
{
#ifdef TSE_BEST_TEXT_GOP_MODE
	return TSE_BEST_TEXT_GOP_MODE;
#else
	return FALSE;
#endif
}

/**
    Returns TSE_SUPPORT_DEFAULT_FOR_STRING_CONTROL token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsSupportDefaultForStringControl (VOID)
{
#if TSE_SUPPORT_DEFAULT_FOR_STRING_CONTROL
	return TSE_SUPPORT_DEFAULT_FOR_STRING_CONTROL;
#else
	return FALSE;
#endif
}

/**
    Wrapper function to call ReInstallBGRT

    @param VOID

    @retval VOID

**/
VOID ReInstallBgrtWrapper (
    )
{
#if CONTRIB_BGRT_TABLE_TO_ACPI
	ReInstallBgrt();
#endif
}
/**
    Returns token value TSE_SUPPORT_WRITE_CHAR_LAST_ROW_COLUMN

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsWriteCharLastRowColSupport()
{
#if TSE_SUPPORT_WRITE_CHAR_LAST_ROW_COLUMN
    return 1;
#else
    return 0;
#endif
}
/**
    function to write char to the last Row Col

    @param String 
    @param Attribute 
    @param Row 
    @param Alignment 
    @param UpdateBackground 

    @retval EFI_STATUS

**/
EFI_STATUS WriteCharLastRowCol (CHAR16 *String, UINT32 Attribute, UINT32 Row, UINT32 Alignment, BOOLEAN UpdateBackground)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    
#if TSE_SUPPORT_WRITE_CHAR_LAST_ROW_COLUMN    
    AMI_TEXT_OUT_PROTOCOL *pr = (AMI_TEXT_OUT_PROTOCOL *)NULL;
    EFI_GUID  AmiTextOutProtocolGuid = AMI_TEXT_OUT_PROTOCOL_GUID;
        
    Status = gBS->LocateProtocol(&AmiTextOutProtocolGuid, NULL, &pr);
    if(EFI_ERROR(Status))
        return Status;

    Status = pr->PrintLine(pr, String, Attribute, Row, Alignment, UpdateBackground);    //will print on last line
#endif
    return Status;
}

/**
    Returns token value  TSEMousePostTriggerTime

    @param VOID

    @retval UINT64

**/
UINT64 TSEMousePostTriggerTime(VOID)
{
	return TSE_MOUSE_POST_TRIGGER_TIME;
}

/**
    Function to return whether MousePointer is on softkbd or not

    @param VOID

    @retval TRUE/FALSE

**/
BOOLEAN TSEIsMouseOnSoftkbd(VOID);
BOOLEAN IsMouseOnSoftkbd(VOID)
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	return TSEIsMouseOnSoftkbd();	
#endif
	return FALSE;	
}

/**
    Returns the value of token which decides whether to skip orphan variable or not

    @param VOID

    @retval UINT32

**/
UINT32 SkipOrphanBootOrderVar()
{
#if TSE_SKIP_ORPHAN_BOOT_ORDER_VARIABLE
	return TSE_SKIP_ORPHAN_BOOT_ORDER_VARIABLE;
#else
	return 0;
#endif
}

/**
    Returns token TSE_ISOLATE_SETUP_DATA value for checking Isolating SetupData
	 Feature is enabled or not

    @param VOID

    @retval BOOLEAN

**/

BOOLEAN   IsIsolateSetupDataSupported (VOID)
{
#if TSE_ISOLATE_SETUP_DATA
	return TSE_ISOLATE_SETUP_DATA;
#else
	return 0;
#endif
}

/**
    Returns the password String of token 

    @param  PwdString

    @retval CHAR16 *

**/
CHAR16 *DefaultPasswordFromToken(CHAR16 * PwdString)
{
	CHAR16 *DefPwd=NULL;
	DefPwd= (CHAR16 *)EfiLibAllocateZeroPool(StrLen(PwdString)*sizeof(CHAR16));
	if(DefPwd == NULL)
		return NULL;
	StrnCpy(DefPwd,PwdString+1,StrLen(PwdString)-2);
	if((StrCmp(DefPwd,(CHAR16 *)L"EMPTY_DATA")==0) || (StrLen(DefPwd)==0)) 
	{
	     MemFreePointer ((VOID **) &DefPwd);
		 return CONVERT_TO_WSTRING(EMPTY_DATA);
	}
	return DefPwd;
}


/**
    Returns token TSE_WAIT_FOR_KEY_EVENT_BEFORE_READ value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsWaitForKeyEventSupport (VOID)
{
#if TSE_WAIT_FOR_KEY_EVENT_BEFORE_READ
	return TSE_WAIT_FOR_KEY_EVENT_BEFORE_READ;
#else
	return FALSE;
#endif
}

/**
    Returns System ResetType from  AmiTsePcdResetType value

    @param VOID

    @retval UINT8

**/
UINT8 TseGetResetType()
{
 return PcdGet8(AmiTsePcdResetType);
}

/**
    Returns GUID of the  PCD shell file

    @param VOID

    @retval EFI_GUID

**/
EFI_GUID TSEGetPCDptr()
{
	EFI_GUID *gPEfiShellFileGuid  = (EFI_GUID *)PcdGetPtr(PcdShellFile);
	return *gPEfiShellFileGuid;
}
/**
	Returns POPUP_MENU_ENTER_SETUP token value

	@param		VOID

	@retval		BOOLEAN

**/
BOOLEAN IsPopupMenuEnterSetupEnabled(VOID)
{
#if POPUP_MENU_ENTER_SETUP
	return TRUE;
#else
	return FALSE;
#endif	
}


/**
    Returns  BOOT_TO_IMAGE_CODE_TYPE_SUPPORT token value

    @param VOID

    @retval UINT8

**/

UINT8  IsImageCodeTypeSupport (VOID)
{
#ifdef BOOT_TO_IMAGE_CODE_TYPE_SUPPORT
	return BOOT_TO_IMAGE_CODE_TYPE_SUPPORT;
#endif
	return 0;					//Defaultly enabled.
}


/**
    Returns  TSE_WATCHDOG_TIMER token value

    @param VOID

    @retval UINT8

**/
BOOLEAN IsTseWatchDogTimer()
{
#if TSE_WATCHDOG_TIMER
	return TSE_WATCHDOG_TIMER;
#else
	return 0;
#endif  
}

/**
	Retrurns TEXT_EXCESS_SUPPORT token value
	
	@param	VOID
	
	@retval	UINT8
**/
UINT8  IsTSETextExcessSupport (VOID)
{
#ifdef TEXT_EXCESS_SUPPORT
    return TEXT_EXCESS_SUPPORT;
#endif
    return 0;					//Defaultly disabled.
}

/**
	Returns DEFAULT_CONDITIONAL_EXPRESSION_SUPPORT token value

	@param		VOID

	@retval		BOOLEAN

**/
BOOLEAN IsDefaultConditionalExpression(VOID)
{
#if DEFAULT_CONDITIONAL_EXPRESSION_SUPPORT
	return TRUE;
#else 
	return FALSE;				//Defaultly disabled.
#endif
}

/**
	Returns TSE_DISPLAY_FORMSET_PARSING_ERROR token value

	@param		VOID

	@retval		BOOLEAN

**/

BOOLEAN IsTSEDisplayFormsetParsingError(VOID)
{
#if TSE_DISPLAY_FORMSET_PARSING_ERROR
	return TRUE;
#else 
	return FALSE;				//Default -> disabled.
#endif
}

/**
    Returns token GROUP_DYNAMIC_PAGES_BY_CLASSCODE value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsGroupDynamicPagesByClass()
{
#if GROUP_DYNAMIC_PAGES_BY_CLASSCODE
    return 1;
#else
    return 0;
#endif  
}


/**
	Returns TSE_CACHE_PLATFORM_LANG token value

	@param		VOID

	@retval		BOOLEAN

**/
BOOLEAN   IsTseCachePlatformLang (VOID)
{
#if TSE_CACHE_PLATFORM_LANG
	return 1;
#else
	return 0;
#endif
}

/**
    Returns token GROUP_DYNAMIC_PAGES_BY_CLASSCODE value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsSetupPrintEvalMessage()
{
#if SETUP_PRINT_EVAL_MSG
    return SETUP_PRINT_EVAL_MSG;
#else
    return 0;
#endif  
}

/**
 			BootFlowExitHook is called from BootFlowManageExit()
   
  @retval 	EFI_NOT_SUPPORTED to continue normal BootFlowManageExit() flow
  			EFI_NOT_STARTED - return from BootFlowManageExit with EFI_NOT_STARTED 
  				for TSE_POST_STATUS_NO_BOOT_OPTION_FOUND - TSE will display error and call BootFlowManageExit in a loop until return value changes
				for TSE_POST_STATUS_ALL_BOOT_OPTIONS_FAILED TSE will call CheckForKeyHook() to enter Setup
  			EFI_SUCCESS - return from BootFlowManageExit with EFI_SUCCESS
**/
EFI_STATUS BootFlowExitHook(VOID)
{
	return EFI_UNSUPPORTED;
}

/**
    Returns TSE_RECOVERY_SUPPORT token value

    @param VOID

    @retval BOOLEAN

**/
BOOLEAN   IsRecoverySupported(VOID)
{
#if TSE_RECOVERY_SUPPORT
    return TSE_RECOVERY_SUPPORT;
#else
    return 0;
#endif
}

/**
    Returns TSE_BREAK_INTERACTIVE_BBS_POPUP token value
	
    @param VOID

    @retval BOOLEAN

**/
#if !OVERRIDE_TSEBreakInteractiveBbsPopup
BOOLEAN   TSEBreakInteractiveBbsPopup (void)
{
#if TSE_BREAK_INTERACTIVE_BBS_POPUP
    return TSE_BREAK_INTERACTIVE_BBS_POPUP;
#else
    return 0;
#endif
}
#endif


/**
    Returns TSE_BOOT_TO_DISABLED_BOOT_OPTIONS token value
	
    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsBootToDisabledBootOption(void)
{
#if TSE_BOOT_TO_DISABLED_BOOT_OPTIONS
	return 1;
#else
	return 0;
#endif
}

/**
	Returns TSE_GOP_NOTIFICATION_SUPPORT token value

	@param		VOID

	@retval		BOOLEAN

**/

BOOLEAN IsTSEGopNotificationSupport(VOID)
{
#if TSE_GOP_NOTIFICATION_SUPPORT
	return TRUE; // Default enabled 
#else 
	return FALSE;		
#endif
}

/**
    Returns SETUP_HIDE_DISABLE_BOOT_OPTIONS token value
	
    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsSetupHideDisableBootOptions(void)
{
#if SETUP_HIDE_DISABLE_BOOT_OPTIONS
	return 1;
#else
	return 0;
#endif
}

/**
    Returns TSE_DEVICE_PATH_TO_TEXT_PROTOCOL_SUPPORT token value
	
    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsTseDevicePathToTextProtocolSupport(void)
{
#if TSE_DEVICE_PATH_TO_TEXT_PROTOCOL_SUPPORT
	return 1;
#else
	return 0;
#endif
}

/**
    Returns TSE_UEFI_26_FEATURES_SUPPORT token value
	
    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsTseUefi26FeaturesSupport(void)
{
#if TSE_UEFI_26_FEATURES_SUPPORT
	return 1;
#else
	return 0;
#endif
}

/**
    Returns TSE_EFI_BROWSER_ACTION_FORM_OPEN_ON_PAGE_NAVIGATION_SUPPORT token value
	
    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsTseCallFormOpenOnPgChangeSupport(void)
{
#if TSE_EFI_BROWSER_ACTION_FORM_OPEN_ON_PAGE_NAVIGATION_SUPPORT
	return 1;
#else
	return 0;
#endif
}


/**
    Returns SETUP_FORM_BROWSER_NESTED_SEND_FORM_SUPPORT token value
	
    @param VOID

    @retval BOOLEAN

**/
BOOLEAN IsSetupFormBrowserNestedSendFormSupport(void)
{
#if SETUP_FORM_BROWSER_NESTED_SEND_FORM_SUPPORT
	return 1;
#else
	return 0;
#endif
}

/**
    Updates original TSEMouse protocol inot Softkbd
	
    @param VOID

    @retval VOID

**/
VOID RestoreTseMouseInSoftkbd()
{
#if (MINISETUP_MOUSE_SUPPORT && AMITSE_SOFTKBD_SUPPORT)
	SOFT_KBD_PROTOCOL *TSESoftKbd_n = NULL;
	DXE_MOUSE_PROTOCOL *TSEMouse = NULL;
	EFI_GUID SoftKbdGuid = EFI_SOFT_KBD_PROTOCOL_GUID;
	EFI_GUID MouseDriverGuid = EFI_MOUSE_DRIVER_PROTOCOL_GUID;
	EFI_STATUS RetVal = EFI_UNSUPPORTED;
	RetVal = gBS->LocateProtocol(&MouseDriverGuid, NULL, (VOID**) &TSEMouse);
	if (!EFI_ERROR(RetVal))
	{
		RetVal = gBS->LocateProtocol(&SoftKbdGuid, NULL, &TSESoftKbd_n);
	   	if (!EFI_ERROR (RetVal)) {
	   		TSESoftKbd_n->UpdateMouseInterface (TSESoftKbd_n, TSEMouse); // Restoring original TSEMouse protocol to Softkbd module, if it is overwritten by FakeTSEMouse protocol inside Setup.
	   	}
	}
#endif	
}


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
