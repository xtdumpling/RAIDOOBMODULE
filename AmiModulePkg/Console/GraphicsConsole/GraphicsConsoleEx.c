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
    This file contains an extension to the SimpleTextOutput Protocol 
    This extension is the AMI_TEXT_OUT_PROTOCOL.
  
**/
#include "GraphicsConsole.h"

extern const EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorArray[];

EFI_STATUS GetColorFromAttribute(
    IN UINT32 Attribute, 
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Foreground,
    OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Background
    );

EFI_STATUS AmiPrintLine (
        IN AMI_TEXT_OUT_PROTOCOL *This,
        IN CHAR16                *String,
        IN UINT32                 Attribute,
        IN UINT32                 Row,
        IN UINT32                 Alignment,
        IN BOOLEAN                UpdateBackground
        );

AMI_TEXT_OUT_PROTOCOL AmiTextOutProtocol = { 
        AmiPrintLine
};



VOID AlignLine (
        IN UINT32  Alignment
        )
{

}

/**
This function outputs provided string on the corresponding row
based on input parameters - color, alignment and row number

  @param This - pointer to the instance of the protocol
  @param String - pointer  to the string to display
  @param Attribute - color of string 
  @param Row - number of row to put string on
  @param Alignment - type of string alignment
  @param UpdateBackground - flag to indicate whether
         background should be updated

  @retval EFI_SUCCESS - string displayed successfully
  @retval other - error occured during execution
**/
EFI_STATUS AmiPrintLine (
        IN AMI_TEXT_OUT_PROTOCOL *This,
        IN CHAR16                *String,
        IN UINT32                 Attribute,
        IN UINT32                 Row,
        IN UINT32                 Alignment,
        IN BOOLEAN                UpdateBackground
        )
{
    EFI_STATUS Status;
    GC_DATA    *GcData;
    EFI_FONT_DISPLAY_INFO FontInfo;
    EFI_IMAGE_OUTPUT      *Image = NULL;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Result;
    UINT32 p1, p2, space;
    UINT32 ScreenWidth;
    UINT32 i, j, k, l;
    
    GcData = OUTTER(This, AmiTextOut, GC_DATA);
    
    //retreive colors
    if(Attribute == 0)
        Attribute = (GcData->SimpleTextOut.Mode)->Attribute;
    
    GetColorFromAttribute( Attribute,
                           &FontInfo.ForegroundColor,
                           &FontInfo.BackgroundColor);
    //use system font
    FontInfo.FontInfoMask =   EFI_FONT_INFO_SYS_FONT |
                              EFI_FONT_INFO_SYS_SIZE |
                              EFI_FONT_INFO_SYS_STYLE;
    //get bitmap image of string
    Status = GcData->HiiFont->StringToImage(GcData->HiiFont,
                                    0,
                                    String,
                                    &FontInfo,
                                    &Image,
                                    0, 0, NULL, NULL, NULL);
    ScreenWidth = NARROW_GLYPH_WIDTH * GcData->MaxColumns;
    if(EFI_ERROR(Status) || 
       Image->Width > ScreenWidth ||
       Image->Height != EFI_GLYPH_HEIGHT)
        return EFI_INVALID_PARAMETER;

    //apply alignment
    space = ScreenWidth - Image->Width;
    if(Alignment == 0) {    //align left
        p1 = 0;
    } else if(Alignment == 1) { //align center
        p1 = space / 2;
    } else if(Alignment == 2) { //align right
        p1 = space;
    } else {
        return EFI_INVALID_PARAMETER;
    }
    
    p2 = p1 + Image->Width;
    
    Status = pBS->AllocatePool(EfiBootServicesData, 
                               ScreenWidth * EFI_GLYPH_HEIGHT * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL),
                               &Result);
    if(EFI_ERROR(Status))
        return Status;
    
    for(i = 0, j = 0, l = 0; i < EFI_GLYPH_HEIGHT; i++, j += ScreenWidth, l += Image->Width) {
        for(k = 0; k < p1; k++)
            Result[j + k] = FontInfo.BackgroundColor;
        for(k = p1; k < p2; k++)
            Result[j + k] = Image->Image.Bitmap[k - p1 + l];
        for(k = p2; k < ScreenWidth; k++)
            Result[j + k] = FontInfo.BackgroundColor;
    }
    
    pBS->FreePool(Image->Image.Bitmap);
    pBS->FreePool(Image);
    
    // update background if needed
    if(UpdateBackground)
        GcData->OemUpdateBltBuffer(GcData, ScreenWidth, EFI_GLYPH_HEIGHT, Result);
    
    if(Row >= GcData->MaxRows)
        Row = GcData->MaxRows - 1;
    
    // put everything on screen
    GcData->GraphicsOutput->Blt(
                    GcData->GraphicsOutput,
                    Result,
                    EfiBltBufferToVideo,
                    0,
                    0,                   
                    GcData->DeltaX,
                    GcData->DeltaY + Row * EFI_GLYPH_HEIGHT,
                    ScreenWidth,
                    EFI_GLYPH_HEIGHT,
                    ScreenWidth * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    pBS->FreePool(Result);
    
    return EFI_SUCCESS;
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
