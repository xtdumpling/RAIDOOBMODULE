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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/postmgmt.c $
//
// $Author: Arunsb $
//
// $Revision: 26 $
//
// $Date: 9/25/12 12:39p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file postmgmt.c
    file to handle the post management operations

**/

#include "minisetup.h"
#if !TSE_USE_EDK_LIBRARY
#include "Acpi.h" 
#include <Protocol/AcpiSupport.h>  
#include  "Protocol/AcpiTable.h" 
#else
#include EFI_PROTOCOL_DEFINITION (AcpiTable)
#endif

#pragma pack (1)
typedef struct					//Structure for forming BGRT Table
{
	UINT16  Version;
	UINT8   Status;
	UINT8   ImageType;
	UINT64  ImageAddress;
	UINT32  ImageOffsetX;
	UINT32  ImageOffsetY;
}BGRT_TABLE;
#pragma pack()
#if !TSE_USE_EDK_LIBRARY
#define EFI_SIGNATURE_16(A,B)     ((A) | (B<<8))
#define EFI_SIGNATURE_32(A,B,C,D) (EFI_SIGNATURE_16 (A,B) | (EFI_SIGNATURE_16 (C,D) << 16))
#endif
#define BGRT_SIGNATURE             EFI_SIGNATURE_32 ('B','G','R','T')
VOID    FormBGRTTable (VOID **, UINTN *, UINT8 *, INTN, INTN, BOOLEAN);
UINT8   CalculateChecksum (VOID *, UINT8);
#define CREATOR_ID_AMI 0x20494D41
#define	CREATOR_REV_MS 0x00010013
UINT8   *GetACPIOEMID (VOID);
UINT8   *GetACPIOEMTableID (VOID);
UINT32  GetACPIOEMRevision (VOID);
LOGO_TYPE GetLogoType(UINT8 *ImageData);
LOGO_TYPE GetExtendedLogoType(UINT8 *ImageData);
VOID    *gBgrtSafeBuffer = NULL;	
UINTN   gTableKey = 0; 
VOID 	InvalidateStatusInBgrtWrapper (VOID);
BOOLEAN IsOEMPOSTLogoSupport();
BOOLEAN 	IsMouseSupported(VOID);
BOOLEAN 	IsSoftKbdSupported(VOID);
UINT64  TSEMousePostTriggerTime(VOID);
extern void PrintGopAndConInDetails();
extern CHAR16 gPostStatusArray[12][100];
/**
    Function to draw OEMLogo to in post screen operations

    @param UINTN *, UINTN*, INTN, INTN

    @retval EFI_STATUS

**/
EFI_STATUS DrawOEMImage (UINTN *width, UINTN *height, INTN CoordinateX, INTN CoordinateY)
{
	EFI_GUID OEMLogoGuid = AMI_OEM_LOGO_GUID;
	UINT8 *ImageData = NULL;
	UINTN ImageSize = 0;
	EFI_STATUS Status = EFI_UNSUPPORTED;
	UINTN       LogoType = 0;

	Status =  GetGraphicsBitMapFromFV( &OEMLogoGuid, (void**)&ImageData, &ImageSize );
	if(EFI_SUCCESS == Status)
	{
		DrawImage( ImageData, ImageSize, CA_AttributeRightTop, CoordinateX, CoordinateY, FALSE, width, height );
		LogoType = GetExtendedLogoType (ImageData);
		if (GIF_Logo != LogoType)
		{
			MemFreePointer ((VOID **)&ImageData);
		}            
	}
	return Status;
}

/**
    fucntion to perform the post screen operations

    @param VOID

    @retval VOID

**/
BOOLEAN DrawPostLogoAt0Pos (VOID);
VOID InitPostScreen( VOID )
{
	UINTN height, width, OrgHeight = 0;
	UINT8 *ImageData = NULL;
	UINTN ImageSize = 0;
	UINTN CoordinateX = 0;
	UINTN CoordinateY = 0;
	UINT32 HorizontalResolution = 0;
	UINT32 VerticalResolution = 0;
	UINTN ConCols = 0, ConRows = 0;
#if !SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
	UINT32 ColorDepth;
	UINT32 RefreshRate;
#endif
	EFI_SIMPLE_TEXT_OUT_PROTOCOL *pConOut = gST->ConOut;
   UINTN       LogoType = 0;
	UINTN tempPostStatus = 0;
	SETUP_DEBUG_TSE("\n[TSE] InitPostScreen Entering :\n");
	if ( pConOut == NULL )
	{
		SETUP_DEBUG_TSE("\n[TSE] InitPostScreen Exiting since pConOut = NULL\n");
		return;
	}
	SETUP_DEBUG_TSE ("\n[TSE] gPostStatus = %s\n",gPostStatusArray[gPostStatus]);
	SETUP_DEBUG_TSE ("\n[TSE] At Entering :\n");
	PrintGopAndConInDetails();
	SETUP_DEBUG_TSE ("\n[TSE] gMaxCols = %d gMaxRows = %d \n",gMaxCols,gMaxRows);
	//If any SwitchToPostScreen happens during ProceedToBoot then taking backup of original gPostStatus before changing to IN_POST_SCREEN
	if (TSE_POST_STATUS_PROCEED_TO_BOOT == gPostStatus)
		tempPostStatus = gPostStatus;

	if ( gPostStatus <= TSE_POST_STATUS_IN_TSE ) 
		 gPostStatus = TSE_POST_STATUS_IN_POST_SCREEN;

#ifndef STANDALONE_APPLICATION
	if(gConsoleControl != NULL)
	{
		gConsoleControl->SetMode( gConsoleControl, EfiConsoleControlScreenText );
		SETUP_DEBUG_TSE("\n[TSE] Setting Text screen\n");
	}
#endif

#if !APTIO_4_00 || TSE_FULL_SCREEN_POST_SUPPORT
	SetDesiredTextMode ();
#else
	//  In Aptio use the Text mode as is it in post screen
	if (EFI_ERROR (gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &ConCols, &ConRows)))
	{
		gMaxRows = StyleGetStdMaxRows(); 
		gMaxCols = StyleGetStdMaxCols(); 
	}
	else if ((gMaxRows < ConRows) && (gMaxCols < ConCols))
	{
		SetDesiredTextMode ();
	}
#endif
	if(IsMouseSupported()&& IsSoftKbdSupported())//Initializing the mouse at post when mouse and softkbd present
		MouseInit();
	pConOut->EnableCursor( pConOut, FALSE );
	pConOut->SetAttribute( pConOut, EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );
	pConOut->ClearScreen( pConOut );

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
	if(gGOP)
	{
		HorizontalResolution = gGOP->Mode->Info->HorizontalResolution;
		VerticalResolution = gGOP->Mode->Info->VerticalResolution;
	}
#else
	if ( gUgaDraw != NULL )
	{
		gUgaDraw->GetMode (gUgaDraw, &HorizontalResolution, &VerticalResolution, &ColorDepth, &RefreshRate);
	}
#endif

	if ( gPostStatus > TSE_POST_STATUS_IN_TSE ) 
	{
		 InvalidateStatusInBgrtWrapper ();
		 SETUP_DEBUG_TSE("\n[TSE] Not drawing post screen/logo since currently not in or before Post screen\n");
		 return;
	}
	SETUP_DEBUG_TSE ("\n[TSE] Before Drawing Post Logo :\n");
	PrintGopAndConInDetails();
	SETUP_DEBUG_TSE ("\n[TSE] gMaxCols = %d gMaxRows = %d \n",gMaxCols,gMaxRows);
	height = 0;

#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
	if(gGOP )
#else
	if ( gUgaDraw )
#endif
	{
		if (!DrawPostLogoAt0Pos ())
		{
			CoordinateX = (HorizontalResolution - (gMaxCols * HiiGetGlyphWidth ())) / 2;
			CoordinateY = (VerticalResolution - (gMaxRows * HiiGetGlyphHeight ())) / 2;
		}
		SETUP_DEBUG_TSE ("\n[TSE] Post Logo Drawing at CoordinateX = %d CoordinateY = %d\n",CoordinateX,CoordinateY);
		//To display AMILogo on TopLeft corner of setup
	    if(EFI_SUCCESS == GetGraphicsBitMapFromFV( &gAMIEfiLogoGuid, (void**)&ImageData, &ImageSize ))
	    {
			
			MouseStop();
	        DrawImage(
	            ImageData,
	            ImageSize,
	            CA_AttributeLeftTop,
	            CoordinateX,
	            CoordinateY,
	            FALSE,
	            &width,
	            &height
	            );
			 OrgHeight = height; 
          LogoType = GetExtendedLogoType (ImageData);
          SETUP_DEBUG_TSE ("\n[TSE] LogoType = %d\n",LogoType);
          if (GIF_Logo != LogoType)
          {
              MemFreePointer ((VOID **)&ImageData);
          }
			InvalidateStatusInBgrtWrapper ();
	    }
		// To display OEM_POST_Logo on RightTop corner of setup 
		if (IsOEMPOSTLogoSupport())
		{
			if(EFI_SUCCESS == DrawOEMImage(&width, &height, CoordinateX, CoordinateY))
			{								
				InvalidateStatusInBgrtWrapper ();
			}
		}
		if (DrawPostLogoAt0Pos ())
		{
			if (height > (VerticalResolution - (gMaxRows * HiiGetGlyphHeight())) / 2)		//If height is lesser than empty space then make height as 0. Occurs for greater graphics resolution with lesser text resolution. 
			{
				height -= (VerticalResolution - (gMaxRows * HiiGetGlyphHeight())) / 2;
			}
		}
		height = (height / HiiGetGlyphHeight());
		if(OrgHeight % HiiGetGlyphHeight())			//Seeing remainder for original height
		    height ++;
	}

	//If SwitchToPostScreen is called during TSE_POST_STATUS_PROCEED_TO_BOOT poststatus then we need to save PROCEED_TO_BOOT to set it back after drawing postlogo to avoid drawing msgbox, message, other postinterfaces
	if (TSE_POST_STATUS_PROCEED_TO_BOOT == tempPostStatus)
		gPostStatus = TSE_POST_STATUS_PROCEED_TO_BOOT;

	DisplayActivePostMsgs(height);
    
    AfterInitPostScreenHook();
	MouseRefresh();
	SETUP_DEBUG_TSE("\n[TSE] InitPostScreen Exiting :\n");
}

/**
    fucntion to install the timer event

    @param VOID

    @retval VOID

**/
VOID InstallKeyHandlers( VOID )
{
	EFI_STATUS Status;
	if(gKeyTimer != NULL)
		return; // timer already created.
	Status = TimerCreateTimer( &gKeyTimer, CheckForKeyHook, NULL, TimerPeriodic, TIMER_HALF_SECOND, EFI_TPL_CALLBACK );
	// Signal it once immediately.
    if (!EFI_ERROR(Status))
		gBS->SignalEvent( gKeyTimer );
}

/**
    fucntion to install the timer event

    @param VOID

    @retval VOID

**/
VOID InstallClickHandlers( VOID )
{
	EFI_STATUS Status;
	UINT64 Trigger;
	if(gClickTimer != NULL)
		return; // timer already created.
	Trigger= TSEMousePostTriggerTime();
	//TPL callback always has less priority so there might chance that CheckForClickHook never called so increasing the priority level
	Status = TimerCreateTimer( &gClickTimer, CheckForClickHook, NULL, TimerPeriodic, Trigger, EFI_TPL_NOTIFY );
	// Signal it once immediately.
    if (!EFI_ERROR(Status))
		gBS->SignalEvent( gClickTimer );
}

/**
    Created the BGRT table with the input params

    @param VOID * = Buffer to store the ACPI table
        UINTN * = To store the size of the ACPI + BGRT table
        UINT8 * = Address of the image to be to stored
        INTN = X coordinate of the image 
        INTN = Y coordinate of the image
        BOOLEAN = Indicates whether to set the BGRT status field or not.

    @retval VOID

**/
VOID FormBGRTTable (VOID **AcpiTableBuffer, UINTN *AcpiTableBufferSize, UINT8 *ImageAddress, INTN ImageXOffset, INTN ImageYOffset, BOOLEAN GifImagePresence)
{
    BGRT_TABLE  BgrtTable = {0};
    const UINT8 *OemId  = GetACPIOEMID ();
    const UINT8 *OemTblId = GetACPIOEMTableID ();
    UINT8       iIndex = 0;
    UINT8       Checksum = 0;
    UINT8       AcpiHdrSize = 0;          
#if TSE_USE_EDK_LIBRARY
    EFI_ACPI_DESCRIPTION_HEADER AcpiHdr = {0};
    AcpiHdrSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
#else
    ACPI_HDR    AcpiHdr = {0};
    AcpiHdrSize = sizeof (ACPI_HDR);
#endif
    AcpiHdr.Signature = BGRT_SIGNATURE;
    AcpiHdr.Length = AcpiHdrSize + sizeof(BGRT_TABLE);
    AcpiHdr.Revision = 1;
    AcpiHdr.Checksum = 0;
    AcpiHdr.CreatorId = CREATOR_ID_AMI; //"AMI"
    for (iIndex = 0; ((iIndex < 6) && (OemId [iIndex])); iIndex ++)
    {
        AcpiHdr.OemId [iIndex] = OemId [iIndex];
    }
#if TSE_USE_EDK_LIBRARY
    AcpiHdr.OemRevision = GetACPIOEMRevision ();
    AcpiHdr.CreatorRevision = CREATOR_REV_MS;
    for (iIndex = 0; ((iIndex < 8) && (OemTblId [iIndex])); iIndex ++)
    {
        ((UINT8 *)(&AcpiHdr.OemTableId)) [iIndex] = OemTblId [iIndex];
    }
#else
    AcpiHdr.OemRev = GetACPIOEMRevision ();
    AcpiHdr.CreatorRev = CREATOR_REV_MS;    
    for (iIndex = 0; ((iIndex < 8) && (OemTblId [iIndex])); iIndex ++)
    {
        AcpiHdr.OemTblId [iIndex] = OemTblId [iIndex];
    }
#endif
	BgrtTable.Version = 1;
	if (!GifImagePresence)			//If gif image presented then status field should be 0.
	{
		BgrtTable.Status = 1;
	}
    BgrtTable.ImageType = 0;					//0 Represents BMP image
	BgrtTable.ImageAddress = (UINTN)ImageAddress;
    BgrtTable.ImageOffsetX = (UINT32)ImageXOffset;
    BgrtTable.ImageOffsetY = (UINT32)ImageYOffset;

    Checksum = CalculateChecksum (&AcpiHdr, AcpiHdrSize);
    Checksum += CalculateChecksum (&BgrtTable, (UINT8)sizeof (BGRT_TABLE));
    AcpiHdr.Checksum = Checksum;

    *AcpiTableBuffer = EfiLibAllocatePool (AcpiHdrSize + sizeof (BGRT_TABLE));
    if (NULL == *AcpiTableBuffer)
    {
        return;
    }
    EfiCopyMem (*AcpiTableBuffer, &AcpiHdr, AcpiHdrSize);
    EfiCopyMem (((UINT8 *)(*AcpiTableBuffer)) + AcpiHdrSize, &BgrtTable, sizeof (BGRT_TABLE));    
    *AcpiTableBufferSize = AcpiHdrSize + sizeof(BGRT_TABLE);     
}	

/**
    Adds the image data to the ACPI Table.

    @param UINT8 * = Address of the image to be to stored
        INTN = X coordinate of the image 
        INTN = Y coordinate of the image
        BOOLEAN = Indicates whether to set the BGRT status field or not

    @retval VOID

**/
VOID AddImageDetailToACPI (UINT8 *ImageData, INTN CoordinateX, INTN CoordinateY, BOOLEAN GifImagePresence)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_GUID    AcpiTableProtocolGuid = EFI_ACPI_TABLE_PROTOCOL_GUID;
    VOID        *AcpiTableBuffer = NULL;
    UINTN       AcpiTableBufferSize = 0;
	EFI_ACPI_TABLE_PROTOCOL *AcpiTableInstance = NULL;
    
	Status = gBS->LocateProtocol (&AcpiTableProtocolGuid, NULL,(void**)&AcpiTableInstance);
	if (EFI_SUCCESS != Status)
	{
		return;
	}
	FormBGRTTable (&AcpiTableBuffer, &AcpiTableBufferSize, ImageData, CoordinateX, CoordinateY, GifImagePresence);
    if (NULL != AcpiTableBuffer)
    {
        Status = AcpiTableInstance->InstallAcpiTable (AcpiTableInstance, AcpiTableBuffer, AcpiTableBufferSize, &gTableKey);
		if (EFI_SUCCESS == Status)
		{
			gBgrtSafeBuffer = AcpiTableBuffer;
		}
    }
}

/**
    Calculates the checksum for the given structure

    @param VOID => Table structure
        UINT8 => Table Size

    @retval UINT8 => Checksum for the table

**/
UINT8 CalculateChecksum (VOID *AcpiTable, UINT8 TableSize)
{
    UINT8   *TablePtr = (UINT8 *)AcpiTable;
    UINT8   iIndex = 0;
    UINT8   Checksum = 0;

    for (iIndex= 0; iIndex < TableSize; iIndex ++)
    {
        Checksum += TablePtr [iIndex];        
    }    
    return (0 - Checksum);
}

/**
    Checks whether the BGRT data supports the specified logo

    @param UINT8 * => Data for the image

    @retval BOOLEAN => TRUE if BGRT supports otherwise FALSE

**/
BOOLEAN BGRTSupportedLogoType (UINT8 *ImageData)
{
    UINTN LogoType;
    LogoType = GetLogoType (ImageData);
    if (BMP_Logo== LogoType)
    {
        BMP_IMAGE_HEADER *BmpHeader;
        BmpHeader = (BMP_IMAGE_HEADER *)ImageData;
        if (24 == BmpHeader->BitPerPixel) 		//As per spec. BMP image should be 24 bitmap
        {    
            return TRUE;
        }
    }
    return FALSE;     
}

/**
    Disables the BGRT status field if any changes happened in screen other than the image display

    @param VOID

    @retval VOID

**/
VOID InvalidateStatusInBgrt (VOID)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_GUID    AcpiTableProtocolGuid = EFI_ACPI_TABLE_PROTOCOL_GUID;
	BGRT_TABLE  *BgrtTable;
	EFI_ACPI_TABLE_PROTOCOL *AcpiTableInstance = NULL;
	UINTN	TableSize = 0;
#if TSE_USE_EDK_LIBRARY
    UINT8 	AcpiHdrSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
	EFI_ACPI_DESCRIPTION_HEADER *AcpiHdr;
#else
    UINT8 AcpiHdrSize = sizeof (ACPI_HDR);
	ACPI_HDR *AcpiHdr;
#endif
	TRACE((-1,"In InvalidateStatusInBgrt"));
	if (NULL == gBgrtSafeBuffer)		
	{
		return;
	}
	BgrtTable = (BGRT_TABLE  *)((UINT8 *)gBgrtSafeBuffer + AcpiHdrSize);

#if TSE_USE_EDK_LIBRARY
	AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER *)gBgrtSafeBuffer;
#else
	AcpiHdr = (ACPI_HDR *)gBgrtSafeBuffer;
#endif
	if(BgrtTable->Status != 0)
	{
		BgrtTable->Status = 0;
		AcpiHdr->Checksum ++;			//Since changing the status bit from 1 to 0 adding checksum by 1.
	}
	Status = gBS->LocateProtocol (&AcpiTableProtocolGuid, NULL,(void**) &AcpiTableInstance);
	if (EFI_SUCCESS != Status)
	{
		return;
	}
	Status = AcpiTableInstance->UninstallAcpiTable (AcpiTableInstance, gTableKey);
	if (EFI_SUCCESS != Status)
	{
		return;
	}
	gTableKey = 0;		//If not set to 0 then ACPI table is not created again
	TableSize = AcpiHdrSize + sizeof(BGRT_TABLE);
	Status = AcpiTableInstance->InstallAcpiTable (AcpiTableInstance, gBgrtSafeBuffer, TableSize, &gTableKey);
}
/**
    Reinstall the BGRT status field if BGRT is uinstalled when booting legacy boot options

    @param VOID

    @retval VOID

**/
VOID ReInstallBgrt(VOID)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_GUID    AcpiTableProtocolGuid = EFI_ACPI_TABLE_PROTOCOL_GUID;
	EFI_ACPI_TABLE_PROTOCOL *AcpiTableInstance = NULL;
	UINTN	TableSize = 0;
	BGRT_TABLE  *BgrtTable;
#if TSE_USE_EDK_LIBRARY
    UINT8 	AcpiHdrSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
    EFI_ACPI_DESCRIPTION_HEADER *AcpiHdr;
#else
    UINT8 AcpiHdrSize = sizeof (ACPI_HDR);
    ACPI_HDR *AcpiHdr;
#endif
    
	TRACE((-1,"In ReInstall Bgrt"));
	
	if (NULL == gBgrtSafeBuffer)		
	{
		return;
	}
	BgrtTable = (BGRT_TABLE  *)((UINT8 *)gBgrtSafeBuffer + AcpiHdrSize);
	
#if TSE_USE_EDK_LIBRARY
	AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER *)gBgrtSafeBuffer;
#else
	AcpiHdr = (ACPI_HDR *)gBgrtSafeBuffer;
#endif
	
	if(BgrtTable->Status != 0)
	{
		BgrtTable->Status = 0;
		AcpiHdr->Checksum ++;			//Since changing the status bit from 1 to 0 adding checksum by 1.
	}
	Status = gBS->LocateProtocol (&AcpiTableProtocolGuid, NULL,(void**) &AcpiTableInstance);
	if (EFI_SUCCESS != Status)
	{
		return;
	}

	gTableKey = 0;		//If not set to 0 then ACPI table is not created again
	TableSize = AcpiHdrSize + sizeof(BGRT_TABLE);
	Status = AcpiTableInstance->InstallAcpiTable (AcpiTableInstance, gBgrtSafeBuffer, TableSize, &gTableKey);
}
/**
    UninstallsBGRT table, Called for Legacy Boot

    @param VOID

    @retval VOID

**/
VOID UninstallBGRT (VOID)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_GUID    AcpiTableProtocolGuid = EFI_ACPI_TABLE_PROTOCOL_GUID;
	BGRT_TABLE  *BgrtTable;
	EFI_ACPI_TABLE_PROTOCOL *AcpiTableInstance = NULL;
#if TSE_USE_EDK_LIBRARY
    UINT8 	AcpiHdrSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
#else
    UINT8 AcpiHdrSize = sizeof (ACPI_HDR);
#endif
	TRACE((-1,"In UninstallBGRT"));

	if (NULL == gBgrtSafeBuffer)
	{
		return;
	}
	BgrtTable = (BGRT_TABLE  *)((UINT8 *)gBgrtSafeBuffer + AcpiHdrSize);

	Status = gBS->LocateProtocol (&AcpiTableProtocolGuid, NULL,(void**) &AcpiTableInstance);
	if (EFI_SUCCESS != Status)
	{
		return;
	}
	Status = AcpiTableInstance->UninstallAcpiTable (AcpiTableInstance, gTableKey);
	if (EFI_SUCCESS != Status)
	{
		return;
	}
   // MemFreePointer((VOID**)&BgrtTable->ImageAddress);
   // MemFreePointer((VOID**)&gBgrtSafeBuffer);

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 2015, American Megatrends, Inc.              **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*********************************************************************
