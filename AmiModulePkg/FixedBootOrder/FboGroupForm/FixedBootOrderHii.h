//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file FixedBootOrderHii.h

    Header file of FixedBootOrder module
*/

#ifndef _FIXED_BOOT_ORDER_HII_H
#define _FIXED_BOOT_ORDER_HII_H
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)
#if EFI_SPECIFICATION_VERSION>0x20000

typedef struct {
  //
  // Buffer size allocated for Data.
  //
  UINT32                BufferSize;

  //
  // Offset in Data to append the newly created opcode binary.
  // It will be adjusted automatically in Create***OpCode(), and should be
  // initialized to 0 before invocation of a serial of Create***OpCode()
  //
  UINT32                Offset;

  //
  // The destination buffer for created op-codes
  //
  UINT8                 *Data;
} EFI_HII_UPDATE_DATA;
#endif

#define EFI_IFR_EXTEND_OP_LABEL       0x0
#define EFI_IFR_EXTEND_OP_BANNER      0x1
#define EFI_IFR_EXTEND_OP_TIMEOUT     0x2
#define EFI_IFR_EXTEND_OP_CLASS       0x3
#define EFI_IFR_EXTEND_OP_SUBCLASS    0x4

#pragma pack()

EFI_STATUS
IfrLibUpdateForm (
  IN EFI_HII_HANDLE            Handle,
  IN EFI_GUID                  *FormSetGuid, OPTIONAL
  IN EFI_FORM_ID               FormId,
  IN UINT16                    Label,
  IN BOOLEAN                   Insert,
  IN EFI_HII_UPDATE_DATA       *Data
  );


EFI_STATUS CreateHiiOneOfItem(UINT8 **IfrBufferPtr, UINT16 VarStoreId, UINT16 VarOffset, UINT16 CallBackKey, STRING_REF StrTokenId, STRING_REF StrTokenHelpId, UINT8 Flags);
EFI_STATUS CreateHiiOneOfOptionItem(UINT8 **IfrBufferPtr, UINT8 Type, UINT16 Index, STRING_REF StrTokenId);
EFI_STATUS CreateHiiRefItem(UINT8 **IfrBufferPtr, UINT16 FromID, STRING_REF StrTokenId1, STRING_REF StrTokenId2);
EFI_STATUS CreateHiiIfrFormItem(UINT8 **IfrBufferPtr, UINT16 FromID, STRING_REF StrTokenId);
EFI_STATUS CreateHiiIfrEndItem(UINT8 **IfrBufferPtr);
EFI_STATUS CreateHiiSubtitleItem(UINT8 **IfrBufferPtr, STRING_REF StrTokenId, STRING_REF HelpStrTokenId, UINT8 Flags );

UINT16 FIndVariableID(CHAR8 *VarName );

void InitHiiData(EFI_HANDLE HiiHandle);
void FreeHiiData();

#ifdef __cplusplus
}
#endif


#endif  //#ifndef _FIXED_BOOT_ORDER_HII_H

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


