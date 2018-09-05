//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
    AMI TextOut protocol definition.
    It provides extended (compared to SimpleTxtOut->OutpuString) line printing functionality.
 **/

#ifndef __AMI_TEXT_OUT_PROTOCOL_
#define __AMI_TEXT_OUT_PROTOCOL_

#ifdef __cplusplus
extern "C" {
#endif

// e1e4a857-c970-4075-a4da-e9c41b69adfc
#define AMI_TEXT_OUT_PROTOCOL_GUID \
    { 0xe1e4a857, 0xc970, 0x4075, { 0xa4, 0xda, 0xe9, 0xc4, 0x1b, 0x69, 0xad, 0xfc } }

typedef struct _AMI_TEXT_OUT_PROTOCOL AMI_TEXT_OUT_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMI_TEXT_OUT_PROTOCOL_PRINT_LINE) (
    IN AMI_TEXT_OUT_PROTOCOL *This,             //! pointer to AMI_TEXT_OUT_PROTOCOL instance
    IN CHAR16                *String,           //! pointer to the String to print
    IN UINT32                 Attribute,        //! attributes (colors to use), zero to use current SimpleTextOut attributes
    IN UINT32                 Row,              //! row, where to print the line, 0xffff for the last line in current resolution
    IN UINT32                 Alignment,        //! alignment: 0 - align left, 1 - align center, 2 - align right
    IN BOOLEAN                UpdateBackground  //! flag to update background with OemUpdateBltBuffer function before print
 );

struct _AMI_TEXT_OUT_PROTOCOL {
    AMI_TEXT_OUT_PROTOCOL_PRINT_LINE   PrintLine;
};

extern EFI_GUID gAmiTextOutProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
