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

/** @file HddSecDynamicSetup.h
    HDD security setup configuration variable related header
**/

#ifndef _HDD_SECURITY_DYNAMIC_SETUP_H_
#define _HDD_SECURITY_DYNAMIC_SETUP_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>

// {B0DAFBEB-C686-427f-83C8-F4839FE944B4}
#define HDD_SECURITY_CONFIGURATION_VARIABLE_GUID \
    { 0xb0dafbeb, 0xc686, 0x427f, 0x83, 0xc8, 0xf4, 0x83, 0x9f, 0xe9, 0x44, 0xb4}

#define HDD_SECURITY_DYNAMIC_VARIABLE_ID 0xDDDD

#define HDD_SECURITY_MAX_HDD_COUNT 32
#define HDD_SECURITY_MAX_PASSWORD_LENGTH 32
#define HDD_SECURITY_TOTAL_PASSWORD_LENGTH HDD_SECURITY_MAX_HDD_COUNT + HDD_SECURITY_MAX_PASSWORD_LENGTH

#pragma pack(push, 1)
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
typedef struct _HDD_SECURITY_CONFIGURATION {
    UINT8      Count;
    UINT8      Supported[HDD_SECURITY_MAX_HDD_COUNT];
    UINT8      Enabled[HDD_SECURITY_MAX_HDD_COUNT];
    UINT8      Locked[HDD_SECURITY_MAX_HDD_COUNT];
    UINT8      Frozen[HDD_SECURITY_MAX_HDD_COUNT];
    UINT8      UserPasswordStatus[HDD_SECURITY_MAX_HDD_COUNT];
    UINT8      MasterPasswordStatus[HDD_SECURITY_MAX_HDD_COUNT];
    UINT8      ShowMaster[HDD_SECURITY_MAX_HDD_COUNT];
    UINT8      HddPassword[36];
}HDD_SECURITY_CONFIGURATION;
#else
typedef struct _HDD_SECURITY_CONFIGURATION {
    UINT8      Count;
    UINT8      Supported[1];
    UINT8      Enabled[1];
    UINT8      Locked[1];
    UINT8      Frozen[1];
    UINT8      UserPasswordStatus[1];
    UINT8      MasterPasswordStatus[1];
    UINT8      ShowMaster[1];
    UINT8      HddPassword[36];
}HDD_SECURITY_CONFIGURATION;
#endif
#pragma pack(pop)

#ifndef VFRCOMPILE
/**
  Internal function reserves the enough buffer for current opcode.
  When the buffer is not enough, Opcode buffer will be extended.

  @param[in]  OpCodeHandle   Handle to the buffer of opcodes.
  @param[in]  Size           Size of current opcode.

  @return Pointer to the current opcode.
**/
UINT8 *
EFIAPI
InternalHiiGrowOpCodeHandle (
  IN VOID   *OpCodeHandle,
  IN UINTN  Size
  );

/**
  Internal function creates opcode based on the template opcode.

  @param[in]  OpCodeHandle    Handle to the buffer of opcodes.
  @param[in]  OpCodeTemplate  Pointer to the template buffer of opcode.
  @param[in]  OpCode          OpCode IFR value.
  @param[in]  OpCodeSize      Size of opcode.
  @param[in]  ExtensionSize   Size of extended opcode.
  @param[in]  Scope           Scope bit of opcode.

  @return Pointer to the current opcode with opcode data.
**/
UINT8 *
EFIAPI
InternalHiiCreateOpCodeExtended (
  IN VOID   *OpCodeHandle,
  IN VOID   *OpCodeTemplate,
  IN UINT8  OpCode,
  IN UINTN  OpCodeSize,
  IN UINTN  ExtensionSize,
  IN UINT8  Scope
  );

/**
  Internal function gets the current position of opcode buffer.

  @param[in]  OpCodeHandle   Handle to the buffer of opcodes.

  @return Current position of opcode buffer.
**/
UINTN
EFIAPI
InternalHiiOpCodeHandlePosition (
  IN VOID  *OpCodeHandle
  );

/**
  Append opcodes from one OpCode Handle to another OpCode handle.

  If OpCodeHandle is NULL, then ASSERT().
  If RawOpCodeHandle is NULL, then ASSERT();

  @param[in]  OpCodeHandle     Handle to the buffer of opcodes.
  @param[in]  RawOpCodeHandle  Handle to the buffer of opcodes.

  @retval NULL   There is not enough space left in Buffer to add the opcode.
  @retval Other  A pointer to the appended opcodes.

**/
UINT8 *
EFIAPI
InternalHiiAppendOpCodes (
  IN VOID  *OpCodeHandle,
  IN VOID  *RawOpCodeHandle
  );

/**
  Internal function gets the start pointer of opcode buffer.

  @param[in]  OpCodeHandle   Handle to the buffer of opcodes.

  @return Pointer to the opcode buffer base.
**/
UINT8 *
EFIAPI
InternalHiiOpCodeHandleBuffer (
  IN VOID  *OpCodeHandle
  );

/**
    function to update the setup configuration page after HDD password update

    @param VOID

    @retval VOID

**/
VOID
HddSecurityRefreshSetupData (
        UINTN               HddIndex
);


/**
    This Event notifies String initialization for HddSecurity for  can be processed
    
    @param  Event                 Event whose notification function is being invoked.
    @param  Context               The pointer to the notification function's context,
                                  which is implementation-dependent.

    @retval VOID

**/

VOID 
EFIAPI
AmiTseHddNotifyInitString(
  IN EFI_EVENT Event, 
  IN VOID *Context
);
#endif

#ifdef __cplusplus
}
#endif
#endif

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
