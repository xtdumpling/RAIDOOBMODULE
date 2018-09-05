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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/Expression.c $
//
// $Author: Premkumara $
//
// $Revision: 13 $
//
// $Date: 12/01/11 1:35a $
//
//*****************************************************************//
/** @file Expression.c
    Expression evaluation.

**/
//*************************************************************************

//---------------------------------------------------------------------------
#include "minisetup.h"
#include "TseUefiHii.h"
//#include <Protocol/RegularExpressionProtocol.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// In Aptio UnicodeCollation2 protocol is supported in the same file as
// UnicodeCollation Protocol depending on EFI_SPECIFICATION_VERSION
//
#ifndef TSE_FOR_APTIO_4_50
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation2)
#else
#if TSE_USE_EDK_LIBRARY
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation2)
#else
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation)
#endif
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define EXPRESSION_STACK_SIZE_INCREMENT 100
#define NewString(a, b) HiiAddString(b, a)
#define GetToken(a,b) HiiGetString(b, a)
//---------------------------------------------------------------------------

//
// Global stack used to evaluate boolean expresions
//
EFI_HII_VALUE *mOpCodeScopeStack = NULL;
EFI_HII_VALUE *mOpCodeScopeStackEnd = NULL;
EFI_HII_VALUE *mOpCodeScopeStackPointer = NULL;

EFI_HII_VALUE *mExpressionEvaluationStack = NULL;
EFI_HII_VALUE *mExpressionEvaluationStackEnd = NULL;
EFI_HII_VALUE *mExpressionEvaluationStackPointer = NULL;

CHAR16 *gEmptyString = L"";
VOID *gInconsistCondPtr = NULL;
//
// Unicode collation protocol interface
//
EFI_UNICODE_COLLATION2_PROTOCOL *mUnicodeCollation = NULL;

extern EFI_STATUS GetStringFromDevPath(EFI_DEVICE_PATH_PROTOCOL *DevPath, CHAR16 **DevPathStr);
extern EFI_STATUS GetDevPathFromHandle(EFI_HII_HANDLE HiiHandle, EFI_DEVICE_PATH_PROTOCOL **DevicePath);
extern BOOLEAN EFIAPI CompareGuid (IN CONST GUID  *Guid1,IN CONST GUID  *Guid2);
EFI_GUID gEfiRegularExpressionProtocolGuid = EFI_REGULAR_EXPRESSION_PROTOCOL_GUID;
/**
    Grow size of the stack

        old stack; On output: new stack
    @param StackPtr On input: old stack pointer; On output: new stack pointer
    @param StackPtr On input: old stack end; On output: new stack end

    @retval EFI_SUCCESS Grow stack success.
    @retval EFI_OUT_OF_RESOURCES No enough memory for stack space.
**/

STATIC
EFI_STATUS
GrowStack (
  IN OUT EFI_HII_VALUE  **Stack,
  IN OUT EFI_HII_VALUE  **StackPtr,
  IN OUT EFI_HII_VALUE  **StackEnd
  )
{
  UINTN           Size;
  EFI_HII_VALUE  *NewStack;

  Size = EXPRESSION_STACK_SIZE_INCREMENT;
  if (*StackPtr != NULL) {
    Size = Size + (*StackEnd - *Stack);
  }

  NewStack = EfiLibAllocatePool (Size * sizeof (EFI_HII_VALUE));
  if (NewStack == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (*StackPtr != NULL) {
    //
    // Copy from Old Stack to the New Stack
    //
    EfiCopyMem (
      NewStack,
      *Stack,
      (*StackEnd - *Stack) * sizeof (EFI_HII_VALUE)
      );

    //
    // Free The Old Stack
    //
    gBS->FreePool (*Stack);
  }

  //
  // Make the Stack pointer point to the old data in the new stack
  //
  *StackPtr = NewStack + (*StackPtr - *Stack);
  *Stack    = NewStack;
  *StackEnd = NewStack + Size;

  return EFI_SUCCESS;
}

/**
    Push an element onto the Boolean Stack

        old stack; On output: new stack
    @param StackPtr On input: old stack pointer; On output: new stack pointer
    @param StackPtr On input: old stack end; On output: new stack end
    @param Data Data to push.

    @retval EFI_SUCCESS Push stack success.
**/
EFI_STATUS
PushStack (
  IN OUT EFI_HII_VALUE       **Stack,
  IN OUT EFI_HII_VALUE       **StackPtr,
  IN OUT EFI_HII_VALUE       **StackEnd,
  IN EFI_HII_VALUE           *Data
  )
{
  EFI_STATUS  Status;

  //
  // Check for a stack overflow condition
  //
  if (*StackPtr >= *StackEnd) {
    //
    // Grow the stack
    //
    Status = GrowStack (Stack, StackPtr, StackEnd);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Push the item onto the stack
  //
  EfiCopyMem (*StackPtr, Data, sizeof (EFI_HII_VALUE));
  *StackPtr = *StackPtr + 1;

  return EFI_SUCCESS;
}

/**
    Pop an element from the stack.

        old stack; On output: new stack
    @param StackPtr On input: old stack pointer; On output: new stack pointer
    @param StackPtr On input: old stack end; On output: new stack end
    @param Data Data to pop.

    @retval EFI_SUCCESS The value was popped onto the stack.
    @retval EFI_ACCESS_DENIED The pop operation underflowed the stack
**/
EFI_STATUS
PopStack (
  IN OUT EFI_HII_VALUE       **Stack,
  IN OUT EFI_HII_VALUE       **StackPtr,
  IN OUT EFI_HII_VALUE       **StackEnd,
  OUT EFI_HII_VALUE          *Data
  )
{
  //
  // Check for a stack underflow condition
  //
  if (*StackPtr == *Stack) {
    return EFI_ACCESS_DENIED;
  }

  //
  // Pop the item off the stack
  //
  *StackPtr = *StackPtr - 1;
  EfiCopyMem (Data, *StackPtr, sizeof (EFI_HII_VALUE));
  return EFI_SUCCESS;
}

/**
    Reset stack pointer to begin of the stack.

    @param None.

    @retval None.
**/
VOID
ResetScopeStack (
  VOID
  )
{
  mOpCodeScopeStackPointer = mOpCodeScopeStack;
}


/**
    Saves the current stack

    @param OUT EFI_HII_VALUE  ** -> Memory allocated by the function user should free it
    @param IN OUT UINTN *

    @retval EFI_STATUS
**/
EFI_STATUS SaveStack (EFI_HII_VALUE  **StackPtr, UINTN *Size)
{
	if (mOpCodeScopeStack == mOpCodeScopeStackPointer)
	{
		return EFI_NOT_FOUND;
	}
	*Size = (UINT8 *)mOpCodeScopeStackPointer - (UINT8 *)mOpCodeScopeStack;
	
	*StackPtr = EfiLibAllocateZeroPool (*Size);
	if (NULL == *StackPtr)
	{
		return EFI_OUT_OF_RESOURCES;
	}
	
	MemCopy (*StackPtr, (UINT8 *)mOpCodeScopeStack, (UINT8 *)mOpCodeScopeStackPointer - (UINT8 *)mOpCodeScopeStack);
    return EFI_SUCCESS;
}

/**
    Restores the incoming stack

    @param IN EFI_HII_VALUE  * -> Stack to restore
    @param IN OUT UINTN

    @retval EFI_STATUS
**/
EFI_STATUS RestoreStack (EFI_HII_VALUE  *StackPtr, UINTN Size)
{
	MemCopy (mOpCodeScopeStack, (UINT8 *)StackPtr, Size);
	mOpCodeScopeStackPointer = (EFI_HII_VALUE  *)((UINT8 *)mOpCodeScopeStackPointer + Size);
	return EFI_SUCCESS;
}

/**
    Push an Operand onto the Stack

    @param Operand to push.

    @retval EFI_SUCCESS The value was pushed onto the stack.
    @retval EFI_OUT_OF_RESOURCES There is not enough system memory to grow the stack.

**/
EFI_STATUS
PushScope (
  IN UINT8   Operand
  )
{
  EFI_HII_VALUE  Data;

  Data.Type = EFI_IFR_TYPE_NUM_SIZE_8;
  Data.Value.u8 = Operand;

  return PushStack (
           &mOpCodeScopeStack,
           &mOpCodeScopeStackPointer,
           &mOpCodeScopeStackEnd,
           &Data
           );
}

/**
    Pop an Operand from the Stack

    @param Operand to pop.

    @retval EFI_SUCCESS The value was pushed onto the stack.
    @retval EFI_OUT_OF_RESOURCES There is not enough system memory to
        grow the stack.

**/
EFI_STATUS
PopScope (
  OUT UINT8     *Operand
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Data;

  Status = PopStack (
             &mOpCodeScopeStack,
             &mOpCodeScopeStackPointer,
             &mOpCodeScopeStackEnd,
             &Data
             );

  *Operand = Data.Value.u8;

  return Status;
}

/**
    Reset stack pointer to begin of the stack.

    @param None.

    @retval VOID

**/
VOID
ResetExpressionStack (
  VOID
  )
{
  mExpressionEvaluationStackPointer = mExpressionEvaluationStack;
}

/**
    Push an Expression value onto the Stack

    @param Value Expression value to push.

    @retval EFI_SUCCESS The value was pushed onto the stack.
    @retval EFI_OUT_OF_RESOURCES There is not enough system memory to grow the stack.

**/
EFI_STATUS
PushExpression (
  IN EFI_HII_VALUE  *Value
  )
{
  return PushStack (
           &mExpressionEvaluationStack,
           &mExpressionEvaluationStackPointer,
           &mExpressionEvaluationStackEnd,
           Value
           );
}

/**
    Pop an Expression value from the Stack

    @param Value Expression value to pop.

    @retval EFI_SUCCESS The value was popped onto the stack.
    @retval EFI_OUT_OF_RESOURCES The pop operation underflowed the stack

**/
EFI_STATUS
PopExpression (
  OUT EFI_HII_VALUE  *Value
  )
{
  return PopStack (
           &mExpressionEvaluationStack,
           &mExpressionEvaluationStackPointer,
           &mExpressionEvaluationStackEnd,
           Value
           );
}


/**
    Locate the Unicode Collation Protocol interface for later use.

    @param VOID

    @retval EFI_SUCCESS Protocol interface initialize success.
    @retval Other Protocol interface initialize failed.

**/

EFI_STATUS
InitializeUnicodeCollationProtocol (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mUnicodeCollation != NULL) {
    return EFI_SUCCESS;
  }

  //
  // BUGBUG: Proper impelmentation is to locate all Unicode Collation Protocol
  // instances first and then select one which support English language.
  // Current implementation just pick the first instance.
  //
  Status = gBS->LocateProtocol (
                  &gEfiUnicodeCollation2ProtocolGuid,
                  NULL,
                  (void**)&mUnicodeCollation
                  );
  return Status;
}

/**
    function to conver the lower case string to upper

    @param 

    @retval VOID

**/
VOID
IfrStrToUpper (
  CHAR16                   *String
  )
{
  while (*String != 0) {
    if ((*String >= 'a') && (*String <= 'z')) {
      *String = (*String) & ((UINT16) ~0x20);
    }
    String++;
  }
}

/**
    Evaluate opcode EFI_IFR_TO_STRING.

    @param FormSet Formset which contains this opcode.
    @param Format String format in EFI_IFR_TO_STRING.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrToString (
  IN EFI_HII_HANDLE		   *HiiHandle,
  IN UINT8                 Format,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String;
  CHAR16         *PrintFormat;
  CHAR16         Buffer[CHARACTER_NUMBER_FOR_VALUE];
  UINTN          BufferSize;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (Value.Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
  case EFI_IFR_TYPE_NUM_SIZE_16:
  case EFI_IFR_TYPE_NUM_SIZE_32:
  case EFI_IFR_TYPE_NUM_SIZE_64:
    BufferSize = CHARACTER_NUMBER_FOR_VALUE * sizeof (CHAR16);
    switch (Format) {
    case EFI_IFR_STRING_UNSIGNED_DEC:
    case EFI_IFR_STRING_SIGNED_DEC:
      PrintFormat = L"%ld";
      break;

    case EFI_IFR_STRING_LOWERCASE_HEX:
      PrintFormat = L"%lx";
      break;

    case EFI_IFR_STRING_UPPERCASE_HEX:
      PrintFormat = L"%lX";
      break;

    default:
      return EFI_UNSUPPORTED;
    }
    SPrint (Buffer, BufferSize, PrintFormat, Value.Value.u64);
    String = Buffer;
    break;

  case EFI_IFR_TYPE_STRING:
    EfiCopyMem (Result, &Value, sizeof (EFI_HII_VALUE));
    return EFI_SUCCESS;

  case EFI_IFR_TYPE_BOOLEAN:
    String = (Value.Value.b) ? L"True" : L"False";
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  Result->Type = EFI_IFR_TYPE_STRING;
  Result->Value.string = NewString (String, HiiHandle);
  return EFI_SUCCESS;
}

/**
    Evaluate opcode EFI_IFR_TO_UINT.

    @param FormSet Formset which contains this opcode.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrToUint (
  IN EFI_HII_HANDLE		   *HiiHandle,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String;
  CHAR16         *StringPtr;
  UINTN          BufferSize;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Value.Type >= EFI_IFR_TYPE_OTHER) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_SUCCESS;
  if (Value.Type == EFI_IFR_TYPE_STRING) {
    String = GetToken (Value.Value.string, HiiHandle);
    if (String == NULL) {
      return EFI_NOT_FOUND;
    }

    IfrStrToUpper (String);
    StringPtr = EfiStrStr (String, L"0X");
    if (StringPtr != NULL) {
      //
      // Hex string
      //
      BufferSize = sizeof (UINT64);
      Status = HexStringToBuf ((UINT8 *) &Result->Value.u64, &BufferSize, StringPtr + 2, NULL);
    } else {
      //
      // BUGBUG: Need handle decimal string
      //
    }
    gBS->FreePool (String);
  } else {
    EfiCopyMem (Result, &Value, sizeof (EFI_HII_VALUE));
  }

  Result->Type = EFI_IFR_TYPE_NUM_SIZE_64;
  return Status;
}

/**
    Evaluate opcode EFI_IFR_TO_UINT.

    @param FormSet Formset which contains this opcode.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrCatenate (
  IN EFI_HII_HANDLE		   *HiiHandle,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String[2];
  UINTN          Index;
  CHAR16         *StringPtr;

  //
  // String[0] - The second string
  // String[1] - The first string
  //
  String[0] = NULL;
  String[1] = NULL;
  StringPtr = NULL;
  Status = EFI_SUCCESS;

  for (Index = 0; Index < 2; Index++) {
    Status = PopExpression (&Value);
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (Value.Type != EFI_IFR_TYPE_STRING) {
      Status = EFI_UNSUPPORTED;
      goto Done;
    }

    String[Index] = GetToken (Value.Value.string, HiiHandle);
    if (String[Index]== NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  StringPtr= EfiLibAllocatePool (EfiStrSize (String[1]) + EfiStrSize (String[0]));
  ASSERT (StringPtr != NULL);
  EfiStrCpy (StringPtr, String[1]);
  EfiStrCat (StringPtr, String[0]);

  Result->Type = EFI_IFR_TYPE_STRING;
  Result->Value.string = NewString (StringPtr, HiiHandle);

Done:
  EfiLibSafeFreePool (String[0]);
  EfiLibSafeFreePool (String[1]);
  EfiLibSafeFreePool (StringPtr);

  return Status;
}

/**
    Evaluate opcode EFI_IFR_MATCH.

    @param FormSet Formset which contains this opcode.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrMatch (
  IN EFI_HII_HANDLE      *HiiHandle,
  OUT  EFI_HII_VALUE	 *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String[2];
  UINTN          Index;

  //
  // String[0] - The string to search
  // String[1] - pattern
  //
  String[0] = NULL;
  String[1] = NULL;
  Status = EFI_SUCCESS;
  for (Index = 0; Index < 2; Index++) {
    Status = PopExpression (&Value);
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (Value.Type != EFI_IFR_TYPE_STRING) {
      Status = EFI_UNSUPPORTED;
      goto Done;
    }

    String[Index] = GetToken (Value.Value.string, HiiHandle);
    if (String[Index]== NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  Result->Type = EFI_IFR_TYPE_BOOLEAN;
  Result->Value.b = mUnicodeCollation->MetaiMatch (mUnicodeCollation, String[0], String[1]);

Done:
  EfiLibSafeFreePool (String[0]);
  EfiLibSafeFreePool (String[1]);

  return Status;
}


/**
  Evaluate opcode EFI_IFR_MATCH2.

  @param  FormSet                Formset which contains this opcode.
  @param  SyntaxType             Syntax type for match2.
  @param  Result                 Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
EFI_STATUS
IfrMatch2 (
  IN CONTROL_INFO      *ControlInfo,
  IN EFI_GUID              *SyntaxType,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS                       Status;
  EFI_HII_VALUE                    Value[2];
  CHAR16                           *String[2];
  UINTN                            Index;
  UINTN                            GuidIndex;
  EFI_HANDLE                       *HandleBuffer;
  UINTN                            BufferSize;
  EFI_REGULAR_EXPRESSION_PROTOCOL  *RegularExpressionProtocol;
  UINTN                            RegExSyntaxTypeListSize;
  EFI_REGEX_SYNTAX_TYPE            *RegExSyntaxTypeList;
  UINTN                            CapturesCount;
  BOOLEAN guidflag = FALSE;
  //
  // String[0] - The string to search
  // String[1] - pattern
  //
 
  String[0] = NULL;
  String[1] = NULL;
  HandleBuffer = NULL;
  RegExSyntaxTypeList = NULL;
  Status = EFI_SUCCESS;
  
  EfiZeroMem(Value, sizeof (Value));
  
  //pop expression value from the stack
  Status = PopExpression (&Value[0]);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = PopExpression (&Value[1]);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
//check value type is string or not
  for (Index = 0; Index < 2; Index++) {
    if (Value[Index].Type != EFI_IFR_TYPE_STRING) {
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      Status = EFI_SUCCESS;
      goto Done;
    }
    
    //getting string from Hii database
    String[Index] = GetToken (Value[Index].Value.string, ControlInfo->ControlHandle);
    if (String [Index] == NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  BufferSize    = 0;
  HandleBuffer  = NULL;
  
  //locate RegularExpressionProtocol handle
  Status = gBS->LocateHandle(
                      ByProtocol,
                      &gEfiRegularExpressionProtocolGuid,
                      NULL,
                      &BufferSize,
                      HandleBuffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    HandleBuffer = EfiLibAllocateZeroPool(BufferSize);
    if (HandleBuffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    Status = gBS->LocateHandle(
                        ByProtocol,
                        &gEfiRegularExpressionProtocolGuid,
                        NULL,
                        &BufferSize,
                        HandleBuffer);

  }
//if value is not string type and it is undefined type
  if (EFI_ERROR (Status)) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    Status = EFI_SUCCESS;
    goto Done;
  }

  ASSERT (HandleBuffer != NULL);
  for ( Index = 0; Index < BufferSize / sizeof(EFI_HANDLE); Index ++) {
	  //search for Handle Regular Expression Protoco 
    Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiRegularExpressionProtocolGuid,
                  (VOID**)&RegularExpressionProtocol
                 );
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    RegExSyntaxTypeListSize = 0;
    RegExSyntaxTypeList = NULL;
    //looking for a SyntaxType that is listed in the set of supported regular expression syntax types  returned by RegExSyntaxTypeList.
    Status = RegularExpressionProtocol->GetInfo (
                                          RegularExpressionProtocol, 
                                          &RegExSyntaxTypeListSize, 
                                          RegExSyntaxTypeList
                                          );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      RegExSyntaxTypeList = EfiLibAllocateZeroPool(RegExSyntaxTypeListSize);
      if (RegExSyntaxTypeList == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      Status = RegularExpressionProtocol->GetInfo (
                                            RegularExpressionProtocol, 
                                            &RegExSyntaxTypeListSize, 
                                            RegExSyntaxTypeList
                                            );
    } else if (EFI_ERROR (Status)) {
      goto Done;
    }

    for (GuidIndex = 0; GuidIndex < RegExSyntaxTypeListSize / sizeof(EFI_GUID); GuidIndex++) {
	    guidflag = CompareGuid (&RegExSyntaxTypeList[GuidIndex], SyntaxType);
      if (guidflag) {
        //
        // Find the match type, return the value.
        //
        Result->Type = EFI_IFR_TYPE_BOOLEAN;
        Status = RegularExpressionProtocol->MatchString (
                                              RegularExpressionProtocol, 
                                              String[0],
                                              String[1],
                                              SyntaxType,
                                              &Result->Value.b,
                                              NULL,
                                              &CapturesCount
                                              );
        goto Done;
      }
    }

    if (RegExSyntaxTypeList != NULL) {
	    EfiLibSafeFreePool (RegExSyntaxTypeList);
    }
  }

  //
  // Type specified by SyntaxType is not supported 
  // in any of the EFI_REGULAR_EXPRESSION_PROTOCOL instances.
  //
  Result->Type = EFI_IFR_TYPE_UNDEFINED;
  Status = EFI_SUCCESS;
//for freeing memory if data is NULL
Done:
  if (String[0] != NULL) {
	  EfiLibSafeFreePool (String[0]);
  }
  if (String[1] != NULL) {
	  EfiLibSafeFreePool (String[1]);
  }
  if (RegExSyntaxTypeList != NULL) {
	  EfiLibSafeFreePool (RegExSyntaxTypeList);
  }
  if (HandleBuffer != NULL) {
	  EfiLibSafeFreePool (HandleBuffer);
  }
  return Status;
}


/**
    Evaluate opcode EFI_IFR_FIND.

    @param FormSet Formset which contains this opcode.
    @param Format Case sensitive or insensitive.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrFind (
  IN EFI_HII_HANDLE		   *HiiHandle,
  IN UINT8                 Format,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String[2];
  UINTN          Base;
  CHAR16         *StringPtr;
  UINTN          Index;

  if (Format > EFI_IFR_FF_CASE_INSENSITIVE) {
    return EFI_UNSUPPORTED;
  }

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (Value.Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    return EFI_UNSUPPORTED;
  }
  Base = (UINTN) Value.Value.u64;

  //
  // String[0] - sub-string
  // String[1] - The string to search
  //
  String[0] = NULL;
  String[1] = NULL;
  for (Index = 0; Index < 2; Index++) {
    Status = PopExpression (&Value);
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (Value.Type != EFI_IFR_TYPE_STRING) {
      Status = EFI_UNSUPPORTED;
      goto Done;
    }

    String[Index] = GetToken (Value.Value.string, HiiHandle);
    if (String[Index]== NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }

    if (Format == EFI_IFR_FF_CASE_INSENSITIVE) {
      //
      // Case insensitive, convert both string to upper case
      //
      IfrStrToUpper (String[Index]);
    }
  }

  Result->Type = EFI_IFR_TYPE_NUM_SIZE_64;
  if (Base >= EfiStrLen (String[1])) {
    Result->Value.u64 = 0xFFFFFFFFFFFFFFFF;
  } else {
    StringPtr = EfiStrStr (String[1] + Base, String[0]);
    Result->Value.u64 = (StringPtr == NULL) ? 0xFFFFFFFFFFFFFFFF : (StringPtr - String[1]);
  }

Done:
  EfiLibSafeFreePool (String[0]);
  EfiLibSafeFreePool (String[1]);

  return Status;
}

/**
    Evaluate opcode EFI_IFR_MID.

    @param FormSet Formset which contains this opcode.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrMid (
  IN EFI_HII_HANDLE		   *HiiHandle,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String;
  UINTN          Base;
  UINTN          Length;
  CHAR16         *SubString;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (Value.Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    return EFI_UNSUPPORTED;
  }
  Length = (UINTN) Value.Value.u64;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (Value.Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    return EFI_UNSUPPORTED;
  }
  Base = (UINTN) Value.Value.u64;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (Value.Type != EFI_IFR_TYPE_STRING) {
    return EFI_UNSUPPORTED;
  }
  String = GetToken (Value.Value.string, HiiHandle);
  if (String == NULL) {
    return EFI_NOT_FOUND;
  }

  if (Length == 0 || Base >= EfiStrLen (String)) {
    SubString = gEmptyString;
  } else {
    SubString = String + Base;
    if ((Base + Length) < EfiStrLen (String)) {
      SubString[Length] = L'\0';
    }
  }

  Result->Type = EFI_IFR_TYPE_STRING;
  Result->Value.string = NewString (SubString, HiiHandle);

  gBS->FreePool (String);

  return Status;
}

/**
    Evaluate opcode EFI_IFR_TOKEN.

    @param FormSet Formset which contains this opcode.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrToken (
  IN EFI_HII_HANDLE		   *HiiHandle,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String[2];
  UINTN          Count;
  CHAR16         *Delimiter;
  CHAR16         *SubString;
  CHAR16         *StringPtr;
  UINTN          Index;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (Value.Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    return EFI_UNSUPPORTED;
  }
  Count = (UINTN) Value.Value.u64;

  //
  // String[0] - Delimiter
  // String[1] - The string to search
  //
  String[0] = NULL;
  String[1] = NULL;
  for (Index = 0; Index < 2; Index++) {
    Status = PopExpression (&Value);
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (Value.Type != EFI_IFR_TYPE_STRING) {
      Status = EFI_UNSUPPORTED;
      goto Done;
    }

    String[Index] = GetToken (Value.Value.string, HiiHandle);
    if (String[Index]== NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  Delimiter = String[0];
  SubString = String[1];
  while (Count > 0) {
    SubString = EfiStrStr (SubString, Delimiter);
    if (SubString != NULL) {
      //
      // Skip over the delimiter
      //
      SubString = SubString + EfiStrLen (Delimiter);
    } else {
      break;
    }
    Count--;
  }

  if (SubString == NULL) {
    //
    // nth delimited sub-string not found, push an empty string
    //
    SubString = gEmptyString;
  } else {
    //
    // Put a NULL terminator for nth delimited sub-string
    //
    StringPtr = EfiStrStr (SubString, Delimiter);
    if (StringPtr != NULL) {
      *StringPtr = L'\0';
    }
  }

  Result->Type = EFI_IFR_TYPE_STRING;
  Result->Value.string = NewString (SubString, HiiHandle);

Done:
  EfiLibSafeFreePool (String[0]);
  EfiLibSafeFreePool (String[1]);

  return Status;
}

/**
    Evaluate opcode EFI_IFR_SPAN.

    @param FormSet Formset which contains this opcode.
    @param Flags FIRST_MATCHING or FIRST_NON_MATCHING.
    @param Result Evaluation result for this opcode.

    @retval EFI_SUCCESS Opcode evaluation success.
    @retval Other Opcode evaluation failed.
**/
EFI_STATUS
IfrSpan (
  IN EFI_HII_HANDLE		   *HiiHandle,
  IN UINT8                 Flags,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String[2];
  CHAR16         *Charset;
  UINTN          Base;
  UINTN          Index;
  CHAR16         *StringPtr;
  BOOLEAN        Found;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (Value.Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    return EFI_UNSUPPORTED;
  }
  Base = (UINTN) Value.Value.u64;

  //
  // String[0] - Charset
  // String[1] - The string to search
  //
  String[0] = NULL;
  String[1] = NULL;
  for (Index = 0; Index < 2; Index++) {
    Status = PopExpression (&Value);
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (Value.Type != EFI_IFR_TYPE_STRING) {
      Status = EFI_UNSUPPORTED;
      goto Done;
    }

    String[Index] = GetToken (Value.Value.string, HiiHandle);
    if (String[Index]== NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  if (Base >= EfiStrLen (String[1])) {
    Status = EFI_UNSUPPORTED;
    goto Done;
  }

  Found = FALSE;
  StringPtr = String[1] + Base;
  Charset = String[0];
  while (*StringPtr != 0 && !Found) {
    Index = 0;
    while (Charset[Index] != 0) {
      if (*StringPtr >= Charset[Index] && *StringPtr <= Charset[Index + 1]) {
        if (Flags == EFI_IFR_FLAGS_FIRST_MATCHING) {
          Found = TRUE;
          break;
        }
      } else {
        if (Flags == EFI_IFR_FLAGS_FIRST_NON_MATCHING) {
          Found = TRUE;
          break;
        }
      }
      //
      // Skip characters pair representing low-end of a range and high-end of a range
      //
      Index += 2;
    }

    if (!Found) {
      StringPtr++;
    }
  }

  Result->Type = EFI_IFR_TYPE_NUM_SIZE_64;
  Result->Value.u64 = StringPtr - String[1];

Done:
  EfiLibSafeFreePool (String[0]);
  EfiLibSafeFreePool (String[1]);

  return Status;
}


/**
    Zero extend integer/boolean/date/time to UINT64 for comparing.

    @param Value HII Value to be converted.

    @retval None.
**/
VOID
ExtendValueToU64 (
  IN  EFI_HII_VALUE   *Value
  )
{
  UINT64  Temp;

  Temp = 0;
  switch (Value->Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    Temp = Value->Value.u8;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    Temp = Value->Value.u16;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    Temp = Value->Value.u32;
    break;

  case EFI_IFR_TYPE_BOOLEAN:
    Temp = Value->Value.b;
    break;

  case EFI_IFR_TYPE_TIME:
    Temp = Value->Value.u32 & 0xffffff;
    break;

  case EFI_IFR_TYPE_DATE:
    Temp = Value->Value.u32;
    break;

  default:
    return;
  }

  Value->Value.u64 = Temp;
}

/**
    Compare two Hii Value

    @param Value1 Expression value to compare on left-hand
    @param Value2 Expression value to compare on right-hand
    @param HiiHandle Only required for string compare

    @retval EFI_INVALID_PARAMETER Could not perform comparation on two values
    @retval 0 Two operators equeal
        < 0                    - Value1 is greater than Value2
        > 0                    - Value1 is less than Value2
**/
INTN
CompareHiiValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  )
{
  INTN    Result;
  INT64   Temp64;
  CHAR16  *Str1;
  CHAR16  *Str2;

  if (Value1->Type >= EFI_IFR_TYPE_OTHER || Value2->Type >= EFI_IFR_TYPE_OTHER ) {
    return EFI_INVALID_PARAMETER;
  }

  if (Value1->Type == EFI_IFR_TYPE_STRING || Value2->Type == EFI_IFR_TYPE_STRING ) {
    if (Value1->Type != Value2->Type) {
      //
      // Both Operator should be type of String
      //
      return EFI_INVALID_PARAMETER;
    }

    if (Value1->Value.string == 0 || Value2->Value.string == 0) {
      //
      // StringId 0 is reserved
      //
      return EFI_INVALID_PARAMETER;
    }

    if (Value1->Value.string == Value2->Value.string) {
      return 0;
    }

    Str1 = GetToken (Value1->Value.string, HiiHandle);
    if (Str1 == NULL) {
      //
      // String not found
      //
      return EFI_INVALID_PARAMETER;
    }

    Str2 = GetToken (Value2->Value.string, HiiHandle);
    if (Str2 == NULL) {
      gBS->FreePool (Str1);
      return EFI_INVALID_PARAMETER;
    }

    Result = EfiStrCmp (Str1, Str2);

    gBS->FreePool (Str1);
    gBS->FreePool (Str2);

    return Result;
  }

  //
  // Take remain types(integer, boolean, date/time) as integer
  //
  Temp64 = (INT64) (Value1->Value.u64 - Value2->Value.u64);
  if (Temp64 > 0) {
    Result = 1;
  } else if (Temp64 < 0) {
    Result = -1;
  } else {
    Result = 0;
  }

  return Result;
}

/**

    @param IFRData CONTROL_INFO **RuleControl

    @retval EFI_STATUS
**/
EFI_STATUS _GetRuleControlfromId(UINT16 RuleId, UINT16 PageId, CONTROL_INFO **RuleControl)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CONTROL_INFO *controlInfo = NULL;
  BOOLEAN found = FALSE;
  UINTN control = 0;

  PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[PageId]);
  if(pageInfo->PageHandle ==0)
  {
    Status = EFI_NOT_FOUND;
    goto DONE;
  }

  for(control= 0; (control < pageInfo->PageControls.ControlCount) && (found == FALSE); control++)
  {
    controlInfo = (CONTROL_INFO*)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

//if control type rule only make success because key can be common for other too
    if ((RuleId == controlInfo->ControlKey) && (CONTROL_TYPE_RULE == controlInfo->ControlType))
    {
      found = TRUE;
    }
  }

  if(found)
  {
    *RuleControl = controlInfo;
  }

DONE:
  return Status;
}

/**
    Checks the condition has EFI_IFR_THIS_OP

    @param UINT8  *

    @retval TRUE if EFI_IFR_THIS_OP presents else FALSE
**/
BOOLEAN checkforpushthispresence (UINT8  *ControlConditionalPtr)
{
	EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER*)ControlConditionalPtr;
	UINTN 		inScope = 0;
	UINTN 		i = 0;
	
	if (opHeader->Scope)
	{
		do
		{
			switch (opHeader->OpCode)
			{
			case EFI_IFR_THIS_OP:
				return TRUE;
			case EFI_IFR_END_OP:
				inScope--;
				break;
			default:
				if(opHeader->Scope)
				{
					inScope++;
				}
			}
			i += opHeader->Length;
			opHeader = (EFI_IFR_OP_HEADER*)(ControlConditionalPtr + i);
		}while(inScope);
	}
	return FALSE;
}

/**
    Gets the value for inconsistent/submitif from its parent control

    @param UINT16, UINT16, EFI_HII_VALUE **, PAGE_INFO *, VOID *

    @retval EFI_SUCCESS if value retrieved successfully else some other EFI Status
**/
EFI_STATUS _GetValueFromQuestionId (UINT16 QuestionId, UINT16 PageId, EFI_HII_VALUE **Value);
EFI_STATUS GetValueFromParentControl (UINT16 QuestionId, UINT16 PageId, EFI_HII_VALUE **Value, PAGE_INFO *pageInfo, VOID *CtrlCondPtr)
{
	UINT32		Count = 1;
	UINTN			control = 0;
	EFI_STATUS 	Status = EFI_UNSUPPORTED;
	EFI_IFR_OP_HEADER *OpHeader;
	EFI_IFR_QUESTION_HEADER *questionHdr = NULL;
	CHAR16 		*questionValue;
	EFI_STRING_ID  StrToken = 0;
	CONTROL_INFO *controlInfo;
	  
	for (control= 0; control < pageInfo->PageControls.ControlCount; control++)
	{
		controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);			//If two submit if present then questionid may be same so comparing conditionalptr also
		if ((QuestionId == controlInfo->ControlKey) && ((CONTROL_TYPE_MSGBOX == controlInfo->ControlType) || (NO_SUBMIT_IF == controlInfo->ControlType)) && (CtrlCondPtr == controlInfo->ControlConditionalPtr))
		{
			if (checkforpushthispresence (controlInfo->ControlConditionalPtr))
			{
			    if (control < Count)
			        break;
			    
				do					//Find the parent control. Inconsistent/submit only present inside question
				{
					controlInfo = (CONTROL_INFO*)((UINT8 *)(gControlInfo) + pageInfo->PageControls.ControlList [control-Count]);
					Count ++;
				}while ((CONTROL_TYPE_MSGBOX == controlInfo->ControlType) || (NO_SUBMIT_IF == controlInfo->ControlType));		//find parent suppress all the inconsistent/submit present in the control
				OpHeader=(EFI_IFR_OP_HEADER *)controlInfo->ControlPtr;
				  if (EFI_IFR_STRING_OP != OpHeader->OpCode)
				  {
					  Status = _GetValueFromQuestionId (controlInfo->ControlKey, PageId, Value);
				  }
				  else
				  {
					UINTN dataWidth = UefiGetWidth (controlInfo->ControlPtr);
					questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8 *)controlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));
					questionValue = EfiLibAllocateZeroPool (dataWidth + sizeof (CHAR16));		//sizeof (CHAR16) is for NULL if maximum characters used.
					 if (NULL == questionValue)
					 {
						 return EFI_OUT_OF_RESOURCES;
					 }
					
					// Updated the dataWidth in comparison to the maximum supported
					Status = VarGetValue (controlInfo->ControlVariable, questionHdr->VarStoreInfo.VarOffset, dataWidth, (VOID *)questionValue);
					if (!EFI_ERROR(Status))
					{
						if (*questionValue != 0)
						{
							StrToken = HiiAddString (controlInfo->ControlHandle, questionValue);
							(*Value)->Value.string = StrToken;
							(*Value)->Type = EFI_IFR_TYPE_STRING;
						}
						else
						{
							Status = EFI_NOT_STARTED;			//If null string then dont compare
						}
					}
				  }
			}
			break;
		}
	}
	return Status;
}

/**

    @param QuestionId UINT16 PageId, UINT64 *Value

    @retval EFI_STATUS
**/
EFI_STATUS _GetValueFromQuestionId(UINT16 QuestionId, UINT16 PageId, EFI_HII_VALUE **Value)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_IFR_QUESTION_HEADER *questionHdr = NULL;
  BOOLEAN found = FALSE;
  UINT32 varId = 0;
  UINTN control = 0;
  UINT64 questionValue = 0;

  CONTROL_INFO *controlInfo = NULL;

  PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[PageId]);

  if(pageInfo->PageHandle ==0)
  {
    Status = EFI_NOT_FOUND;
    goto DONE;
  }

  for ( control= 0; (control < pageInfo->PageControls.ControlCount) && (found == FALSE); control++ )
  {
    controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

    if(QuestionId == controlInfo->ControlKey)
    {
	    if ((CONTROL_TYPE_MSGBOX == controlInfo->ControlType) || (NO_SUBMIT_IF == controlInfo->ControlType) || gInconsistCondPtr)
	    {
		    if (gInconsistCondPtr == controlInfo->ControlConditionalPtr)		//All inconsistentif will have 0 as question id so adding more check
		    {
			  varId = controlInfo->ControlVariable;
			  found = TRUE;			    
		    }
	    }
	    else
	    {
		  varId = controlInfo->ControlVariable;
		  found = TRUE;
	    }
    }
  }

  if(!found)
  {
    // Control Definition not in this Page. Look within the formset
    EFI_HII_HANDLE pgHandle = pageInfo->PageHandle;
    UINT32 page = 0;

    for(;page < gPages->PageCount && (found == FALSE); page++)
    {
/*
      if(page == PageId)
      {
        continue;
      }
*/
      pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[page]);
      if(pageInfo->PageHandle != pgHandle)
      {
        continue;
      }

      for(control = 0; control < pageInfo->PageControls.ControlCount && (found == FALSE); control++)
      {
        controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

        if(QuestionId == controlInfo->ControlKey)
        {
          varId = controlInfo->ControlVariable;
          found = TRUE;
        }
      }
    }
  }

	if(found)
	{
		if (NULL != controlInfo->ControlPtr)      //For inconsistent and nosubmitif controlptr will be NULL
		{
			UINTN dataWidth = UefiGetWidth(controlInfo->ControlPtr);
			questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8 *)controlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));
			
			// Updated the dataWidth in comparision to the maximum supported
			Status = VarGetValue(varId, questionHdr->VarStoreInfo.VarOffset, (( dataWidth > sizeof(UINT64))? sizeof(UINT64) : dataWidth ), &questionValue);
			if(EFI_ERROR(Status))
			{
				goto DONE;
			}
			(*Value)->Value.u64 = questionValue;
		}
		else if ((CONTROL_TYPE_MSGBOX == controlInfo->ControlType) || (NO_SUBMIT_IF == controlInfo->ControlType))		//Check for inconistent control
		{
			Status = GetValueFromParentControl (QuestionId, PageId, Value, pageInfo, controlInfo->ControlConditionalPtr);
		}
	}
DONE:
  return Status;
}
/**

    @param QuestionId UINT16 PageId, UINT64 *Value

    @retval EFI_STATUS
**/
EFI_STATUS _SetValueFromQuestionId(UINT16 QuestionId, UINT16 PageId, EFI_HII_VALUE *Value)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_IFR_QUESTION_HEADER *questionHdr = NULL;
  BOOLEAN found = FALSE;
  UINT32 varId = 0;
  UINTN control = 0;
  UINT64 questionValue = Value->Value.u64;

  CONTROL_INFO *controlInfo = NULL;

  PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[PageId]);

  if(pageInfo->PageHandle ==0)
  {
    Status = EFI_NOT_FOUND;
    goto DONE;
  }

  for ( control= 0; (control < pageInfo->PageControls.ControlCount) && (found == FALSE); control++ )
  {
    controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

    if(QuestionId == controlInfo->ControlKey)
    {
      varId = controlInfo->ControlVariable;
      found = TRUE;
    }
  }

  if(!found)
  {
    // Control Definition not in this Page. Look within the formset
    EFI_HII_HANDLE pgHandle = pageInfo->PageHandle;
    UINT32 page = 0;

    for(;page < gPages->PageCount && (found == FALSE); page++)
    {

      pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[page]);
      if(pageInfo->PageHandle != pgHandle)
      {
        continue;
      }

      for(control = 0; control < pageInfo->PageControls.ControlCount && (found == FALSE); control++)
      {
        controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

        if(QuestionId == controlInfo->ControlKey)
        {
          varId = controlInfo->ControlVariable;
          found = TRUE;
        }
      }
    }
  }

  if(found)
  {
    UINTN dataWidth = UefiGetWidth(controlInfo->ControlPtr);
    questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8 *)controlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));

	// Updated the dataWidth in comparision to the maximum supported
	Status = VarSetValue(varId, questionHdr->VarStoreInfo.VarOffset, (( dataWidth > sizeof(UINT64))? sizeof(UINT64) : dataWidth ), &questionValue);
    
  }

DONE:
  return Status;
}

/**

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN IsScopeStackEmpty()
{
  UINT8 opCode = 0;
  EFI_STATUS status = EFI_SUCCESS;

  status = PopScope(&opCode);
  if(status == EFI_ACCESS_DENIED)
  {
    return TRUE;
  }

  status = PushScope(opCode);

  return FALSE;
}

/**
    Finds the page ID with matching Guid from the page list

    @param FormSetGuid 
    @param PageID 

    @retval EFI_STATUS status - EFI_SUCCESS, if successful,
        EFI_INVALID_PARAMETER, if invalid values found
**/
EFI_STATUS GetpageIdFromGuid(EFI_GUID *FormGuid, UINT16 *PageID)
{
  EFI_STATUS status = EFI_INVALID_PARAMETER;
  UINT32 pgIndex = 0; 
  PAGE_INFO *pgInfo = (PAGE_INFO *)NULL;
  PAGE_ID_INFO *pageIdInfo = (PAGE_ID_INFO*)NULL;
  
 
  if(gPages && gApplicationData && gPageIdList)
  {
    for(pgIndex = 0; pgIndex < gPages->PageCount; pgIndex++)
    {
      pgInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[pgIndex]);
      if(pgInfo){
        pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[pgInfo->PageIdIndex]);
        if(pageIdInfo && (EfiCompareGuid(FormGuid, &(pageIdInfo->PageGuid)) == TRUE))//Compare Guid
        {
          *PageID  = pgInfo->PageID;
          status = EFI_SUCCESS;
		  goto DONE;
        }
      }
     }
   }

DONE:
  return status;
}

/**
    Evaluate the result of a HII expression

    @param ControlInfo 
    @param ExpressionResult 
    @param Offset 

    @retval EFI_SUCCESS The expression evaluated successfuly
    @retval EFI_NOT_FOUND The Question which referenced by a
        QuestionId could not be found.
    @retval EFI_OUT_OF_RESOURCES There is not enough system memory to
        grow the stack.
    @retval EFI_ACCESS_DENIED The pop operation underflowed the stack
    @retval EFI_INVALID_PARAMETER Syntax error with the Expression
**/
EFI_STATUS
EvaluateExpression (
  IN CONTROL_INFO *ControlInfo,
  IN OUT EFI_HII_VALUE   *ExpressionResult,
  OUT UINTN *Offset
  )
{
  EFI_IFR_OP_HEADER		  *ifrData = NULL;
  CONTROL_INFO			  *ruleControl = NULL;
  EFI_STATUS              Status;
  UINT16                  Index;
  EFI_HII_VALUE           Data1;
  EFI_HII_VALUE           Data2;
  EFI_HII_VALUE           Data3;
  EFI_HII_VALUE			  *questionValue = NULL;
  EFI_HII_VALUE			  *questionValue2 = NULL;
  EFI_HII_VALUE           *Value = NULL;
  EFI_QUESTION_ID		      questionId = 0;
  INTN                    Result;
  CHAR16                  *StrPtr = NULL;
  UINTN        i = 0;
  UINT8        opCode = 0;
  BOOLEAN      IsDevicePathFound = FALSE ,GUIDFound = FALSE;
  EFI_GUID     formSetGuid = {0}, zeroFormSetGuid = {0};
  UINTN        count = 0,tempcount = 0;
  CHAR16       *devPathStr = (CHAR16 *)NULL, *tempDevPathStr = (CHAR16 *)NULL;
  EFI_DEVICE_PATH_PROTOCOL *tempDevPath = (EFI_DEVICE_PATH_PROTOCOL *)NULL;
  UINT16       DevicePath = 0;
  UINTN 	   inScope = 0;
  //
  // Always reset the stack before evaluating an Expression
  //
  ResetExpressionStack ();
  ResetScopeStack();
  
  ExpressionResult->Type = EFI_IFR_TYPE_OTHER;

  opCode = ((EFI_IFR_OP_HEADER*)ControlInfo->ControlConditionalPtr)->OpCode;
  if(opCode != EFI_IFR_SUPPRESS_IF_OP && opCode != EFI_IFR_NO_SUBMIT_IF_OP &&
    opCode != EFI_IFR_INCONSISTENT_IF_OP && opCode != EFI_IFR_GRAY_OUT_IF_OP &&
    opCode != EFI_IFR_DISABLE_IF_OP && opCode != EFI_IFR_VALUE_OP  && opCode !=TSE_EFI_IFR_WARNING_IF_OP &&
    (CONTROL_TYPE_RULE != ControlInfo->ControlType))
  {
    Status = EFI_SUCCESS;
    goto DONE;
  }

  questionValue = (EFI_HII_VALUE*)EfiLibAllocateZeroPool(sizeof(EFI_HII_VALUE));
  if(questionValue == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }

  questionValue2 = (EFI_HII_VALUE*)EfiLibAllocateZeroPool(sizeof(EFI_HII_VALUE));
  if(questionValue2 == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }

  if(((EFI_IFR_OP_HEADER*)ControlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_RULE_OP)
  {
    PushScope(EFI_IFR_RULE_OP);
    inScope++;
  }else
  {
    PushScope(((EFI_IFR_OP_HEADER*)ControlInfo->ControlConditionalPtr)->OpCode);
    inScope++;
    if (
		    ( ( (EFI_IFR_OP_HEADER*)ControlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_INCONSISTENT_IF_OP) || 
		    ( ( (EFI_IFR_OP_HEADER*)ControlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_NO_SUBMIT_IF_OP) ||
		    ( ( (EFI_IFR_OP_HEADER*)ControlInfo->ControlConditionalPtr)->OpCode == TSE_EFI_IFR_WARNING_IF_OP)
	 )
    {
	    gInconsistCondPtr = ControlInfo->ControlConditionalPtr;
    }
  }
  
//for rule ControlConditionalPtr starts with actual data so don't increment
  if (CONTROL_TYPE_RULE != ControlInfo->ControlType)
  {
	  i += ((EFI_IFR_OP_HEADER*)ControlInfo->ControlConditionalPtr)->Length;
  }

  while (IsScopeStackEmpty() == FALSE)
  {
    ifrData = (EFI_IFR_OP_HEADER*)((UINT8 *)ControlInfo->ControlConditionalPtr + i);

    EfiZeroMem (&Data1, sizeof (EFI_HII_VALUE));
    EfiZeroMem (&Data2, sizeof (EFI_HII_VALUE));
    EfiZeroMem (&Data3, sizeof (EFI_HII_VALUE));
    EfiZeroMem (questionValue, sizeof(EFI_HII_VALUE));
    EfiZeroMem (questionValue2, sizeof(EFI_HII_VALUE));

    Value = &Data3;
    Value->Type = EFI_IFR_TYPE_BOOLEAN;
    Status = EFI_SUCCESS;

    questionValue->Type = EFI_IFR_TYPE_NUM_SIZE_64;

    if (ifrData->Scope)
    	inScope++;
    switch (ifrData->OpCode)
    {
      //
      // Built-in functions
      //
    case EFI_IFR_EQ_ID_VAL_OP:
      questionId = ((EFI_IFR_EQ_ID_VAL*)ifrData)->QuestionId;
      Status = _GetValueFromQuestionId(questionId, ControlInfo->ControlPageID, &questionValue);
      if(EFI_ERROR(Status))
      {
        goto DONE;
      }

      questionValue2->Type = EFI_IFR_TYPE_NUM_SIZE_64;
      questionValue2->Value.u64 = ((EFI_IFR_EQ_ID_VAL*)ifrData)->Value;

      Result = CompareHiiValue (questionValue, questionValue2, NULL);
      if (Result == EFI_INVALID_PARAMETER)
      {
        Status = EFI_INVALID_PARAMETER;
	goto DONE;
      }
      Value->Value.b = (Result == 0) ? TRUE : FALSE;
      break;

    case EFI_IFR_EQ_ID_ID_OP:
      questionId = ((EFI_IFR_EQ_ID_ID*)ifrData)->QuestionId1;
      Status = _GetValueFromQuestionId(questionId, ControlInfo->ControlPageID, &questionValue);
      if(EFI_ERROR(Status))
      {
        goto DONE;
      }
      questionId = ((EFI_IFR_EQ_ID_ID*)ifrData)->QuestionId2;
      Status = _GetValueFromQuestionId(questionId, ControlInfo->ControlPageID, &questionValue2);
      if(EFI_ERROR(Status))
      {
        goto DONE;
      }

      Result = CompareHiiValue (questionValue, questionValue2, ControlInfo->ControlHandle);
      if (Result == EFI_INVALID_PARAMETER)
      {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }
      Value->Value.b = (Result == 0) ? TRUE : FALSE;
      break;

    case EFI_IFR_EQ_ID_LIST_OP:
      questionId = ((EFI_IFR_EQ_ID_VAL_LIST*)ifrData)->QuestionId;
      Status = _GetValueFromQuestionId(questionId, ControlInfo->ControlPageID, &questionValue);
      if(EFI_ERROR(Status))
      {
        goto DONE;
      }

      Value->Value.b = FALSE;
      for (Index =0; Index < ((EFI_IFR_EQ_ID_VAL_LIST*)ifrData)->ListLength; Index++)
      {
        if (questionValue->Value.u16 == ((EFI_IFR_EQ_ID_VAL_LIST*)ifrData)->ValueList[Index])
        {
          Value->Value.b = TRUE;
          break;
        }
      }
      break;

    case EFI_IFR_DUP_OP:
      Status = PopExpression (Value);
      if (EFI_ERROR (Status))
      {
        goto DONE;
      }

      Status = PushExpression (Value);
      break;

    case EFI_IFR_QUESTION_REF1_OP:
    case EFI_IFR_THIS_OP:
      if(ifrData->OpCode == EFI_IFR_THIS_OP)
      {
        questionId = ControlInfo->ControlKey;
      }else
      {
        questionId = ((EFI_IFR_QUESTION_REF1*)ifrData)->QuestionId;
      }
      Status = _GetValueFromQuestionId(questionId, ControlInfo->ControlPageID, &questionValue);
      if(EFI_ERROR(Status))
      {
        goto DONE;
      }

      Value = questionValue;
      break;

     
     case EFI_IFR_QUESTION_REF3_OP:

      //
      // EFI_IFR_QUESTION_REF3
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status))
      {
         goto DONE;
      }
      
      //
      // Validate the expression value
      //
      
      if ((Value->Type > EFI_IFR_TYPE_NUM_SIZE_64) || (Value->Value.u64 > 0xffff))
      {
         Status = EFI_NOT_FOUND;
         goto DONE;
      }
      
      if(ifrData->Length > sizeof(EFI_IFR_QUESTION_REF3))
      {
         if(ifrData->Length > sizeof(EFI_IFR_QUESTION_REF3_2))
         {
            MemCopy(&formSetGuid,&((EFI_IFR_QUESTION_REF3_3*)ifrData)->Guid, sizeof (EFI_GUID));
            if(EfiCompareGuid(&formSetGuid, &zeroFormSetGuid) == FALSE)
            {
               UINT16 PageID;
               Status = GetpageIdFromGuid(&formSetGuid,&PageID);
               if(Status == 0)
               {
                  GUIDFound = TRUE;
                  Status = _GetValueFromQuestionId(Value->Value.u16,PageID, &questionValue);
                  if(EFI_ERROR(Status))
                  {
                     goto DONE;
                  }
               }
            }
         }
         DevicePath = ifrData->Length < sizeof(EFI_IFR_QUESTION_REF3_3) ? ((EFI_IFR_QUESTION_REF3_2*)ifrData)->DevicePath : ((EFI_IFR_QUESTION_REF3_3*)ifrData)->DevicePath;
         if(DevicePath != 0 && !GUIDFound)
         {
            PAGE_INFO *tempPageInfo1 = NULL;
            devPathStr = GetToken (DevicePath,ControlInfo->ControlHandle);               
            
            if(devPathStr == NULL){
               goto DONE;
            }
         
            for(count = 0; count < gPages->PageCount; count++)
            {
               PAGE_INFO *tempPageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[count]);
               if(tempPageInfo == NULL)
               {
                  continue;
               }
               Status = GetDevPathFromHandle(tempPageInfo->PageHandle, &tempDevPath);
               if((Status == EFI_SUCCESS) && (tempDevPath != NULL) && tempPageInfo->PageParentID == 0)
               {
                  GetStringFromDevPath(tempDevPath, &tempDevPathStr);
                  if(EfiStrCmp(devPathStr, tempDevPathStr) == 0)
                  {
                     tempcount++;
                     tempPageInfo1=tempPageInfo;
                  }
                     MemFreePointer( (VOID **)&tempDevPathStr );
               }
            }
            MemFreePointer( (VOID **)&devPathStr );
                  
            if(tempcount == 1){
               IsDevicePathFound = TRUE;
               Status = _GetValueFromQuestionId(Value->Value.u16, tempPageInfo1->PageID, &questionValue);
               if(EFI_ERROR(Status))
               {
                  goto DONE;
               }
            }   
            if(tempcount > 1){
               goto DONE;
            }
         }
      }
      if(!IsDevicePathFound && !GUIDFound && !tempcount) 
      {
         Status = _GetValueFromQuestionId(Value->Value.u16, ControlInfo->ControlPageID, &questionValue);
         if(EFI_ERROR(Status))
         {
            goto DONE;
         }
      }
      Value = questionValue;
      break;

    case EFI_IFR_RULE_REF_OP:
    {
      EFI_STATUS 	RuleRefStatus = EFI_SUCCESS;
      EFI_HII_VALUE *StackPtr = NULL;
      UINTN 		Size = 0;
      
      questionId = ((EFI_IFR_RULE_REF*)ifrData)->RuleId;

      Status = _GetRuleControlfromId(questionId, ControlInfo->ControlPageID, &ruleControl);

      /* This Section Needs to be modified
      //
      // Find expression for this rule
      //
      RuleExpression = RuleIdToExpression (Form, OpCode->RuleId);
      if (RuleExpression == NULL) {
      return EFI_NOT_FOUND;
      }*/

      //
      // Evaluate this rule expression
      //
	  //save and restore the stack
      RuleRefStatus = SaveStack (&StackPtr, &Size);
      Status = EvaluateExpression (ruleControl, ExpressionResult, Offset);
      if (!EFI_ERROR (RuleRefStatus))
      {
    	  RuleRefStatus = RestoreStack (StackPtr, Size);
    	  MemFreePointer((void**)&StackPtr);    	  
      }
      if (EFI_ERROR (Status))
      {
		goto DONE;
      }

      Value = ExpressionResult;
      break;
    }
    case EFI_IFR_STRING_REF1_OP:
      Value->Type = EFI_IFR_TYPE_STRING;
      Value->Value.string = ((EFI_IFR_STRING_REF1*)ifrData)->StringId;
      break;

      //
      // Constant
      //
    case EFI_IFR_TRUE_OP:
      Value->Value.b = TRUE;
      break;
    case EFI_IFR_FALSE_OP:
      Value->Value.b = FALSE;
      break;
    case EFI_IFR_ONE_OP:
#define ONE 1
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
      Value->Value.u8 = ONE;
      break;
    case EFI_IFR_ONES_OP:
#define ONES 0xFFFFFFFFFFFFFFFF
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
      Value->Value.u64 = ONES;
      break;
    case EFI_IFR_UINT8_OP:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
      Value->Value.u8 = ((EFI_IFR_UINT8*)ifrData)->Value;
      break;
    case EFI_IFR_UINT16_OP:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_16;
      Value->Value.u16 = ((EFI_IFR_UINT16*)ifrData)->Value;
      break;
    case EFI_IFR_UINT32_OP:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_32;
      Value->Value.u32 = ((EFI_IFR_UINT32*)ifrData)->Value;
      break;
    case EFI_IFR_UINT64_OP:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
      Value->Value.u64 = ((EFI_IFR_UINT64*)ifrData)->Value;
      break;
    case EFI_IFR_UNDEFINED_OP:
      Value->Type = EFI_IFR_TYPE_OTHER;
      break;
    case EFI_IFR_VERSION_OP:
#define VERSION (UINT16)(2<<8)|((3<<4) | 0x1) //Updated the Version to 2.3.1
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_16;
      Value->Value.u16 = VERSION;
      break;
    case EFI_IFR_ZERO_OP:
#define ZERO 0
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
      Value->Value.u8 = ZERO;
      break;

      //
      // unary-op
      //
    case EFI_IFR_LENGTH_OP:
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
		goto DONE;
      }
      if (Value->Type != EFI_IFR_TYPE_STRING) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      StrPtr = GetToken (Value->Value.string, ControlInfo->ControlHandle);
      if (StrPtr == NULL) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
      Value->Value.u64 = EfiStrLen (StrPtr);
      gBS->FreePool (StrPtr);
      break;

    case EFI_IFR_NOT_OP:
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }
      if (Value->Type != EFI_IFR_TYPE_BOOLEAN) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }
      Value->Value.b = !Value->Value.b;
      break;

    case EFI_IFR_QUESTION_REF2_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }

      //
      // Validate the expression value
      //
      if ((Value->Type > EFI_IFR_TYPE_NUM_SIZE_64) || (Value->Value.u64 > 0xffff)) {
        Status = EFI_NOT_FOUND;
        goto DONE;
      }

      Status = _GetValueFromQuestionId(Value->Value.u16, ControlInfo->ControlPageID, &questionValue);
      if(EFI_ERROR(Status))
      {
        goto DONE;
      }

      Value = questionValue;
      break;

    case EFI_IFR_STRING_REF2_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }

      //
      // Validate the expression value
      //
      if ((Value->Type > EFI_IFR_TYPE_NUM_SIZE_64) || (Value->Value.u64 > 0xffff)) {
        Status = EFI_NOT_FOUND;
        goto DONE;
      }

      Value->Type = EFI_IFR_TYPE_STRING;
      StrPtr = GetToken (Value->Value.u16, ControlInfo->ControlHandle);
      if (StrPtr == NULL) {
        //
        // If String not exit, push an empty string
        //
        Value->Value.string = NewString (gEmptyString, ControlInfo->ControlHandle);
      } else {
        Index = (UINT16) Value->Value.u64;
        Value->Value.string = Index;
        gBS->FreePool (StrPtr);
      }
      break;

    case EFI_IFR_TO_BOOLEAN_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }

      //
      // Convert an expression to a Boolean
      //
      if (Value->Type <= EFI_IFR_TYPE_DATE) {
        //
        // When converting from an unsigned integer, zero will be converted to
        // FALSE and any other value will be converted to TRUE.
        //
        Value->Value.b = (Value->Value.u64) ? TRUE : FALSE;

        Value->Type = EFI_IFR_TYPE_BOOLEAN;
      } else if (Value->Type == EFI_IFR_TYPE_STRING) {
        //
        // When converting from a string, if case-insensitive compare
        // with "true" is True, then push True. If a case-insensitive compare
        // with "false" is True, then push False.
        //
        StrPtr = GetToken (Value->Value.string, ControlInfo->ControlHandle);
        if (StrPtr == NULL) {
          Status = EFI_INVALID_PARAMETER;
          goto DONE;
        }

        if ((EfiStrCmp (StrPtr, L"true") == 0))
        {
          Value->Value.b = TRUE;
        } else if(EfiStrCmp (StrPtr, L"false") == 0)
        {
          Value->Value.b = FALSE;
        } else
        {
          Value->Type = EFI_IFR_TYPE_OTHER;
        }
        gBS->FreePool (StrPtr);
        Value->Type = EFI_IFR_TYPE_BOOLEAN;
      }
      break;

    case EFI_IFR_TO_STRING_OP:
      Status = IfrToString (ControlInfo->ControlHandle, ((EFI_IFR_TO_STRING*)ifrData)->Format, Value);
      break;

    case EFI_IFR_TO_UINT_OP:
      Status = IfrToUint (ControlInfo->ControlHandle, Value);
      break;

    case EFI_IFR_TO_LOWER_OP:
    case EFI_IFR_TO_UPPER_OP:
      Status = InitializeUnicodeCollationProtocol ();
      if (EFI_ERROR (Status)) {
        goto DONE;
      }

      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }

      if (Value->Type != EFI_IFR_TYPE_STRING) {
        Status = EFI_UNSUPPORTED;
        goto DONE;
      }

      StrPtr = GetToken (Value->Value.string, ControlInfo->ControlHandle);
      if (StrPtr == NULL) {
        Status = EFI_NOT_FOUND;
        goto DONE;
      }

      if (ifrData->OpCode == EFI_IFR_TO_LOWER_OP) {
        mUnicodeCollation->StrLwr (mUnicodeCollation, StrPtr);
      } else {
        mUnicodeCollation->StrUpr (mUnicodeCollation, StrPtr);
      }
      Value->Value.string = NewString (StrPtr, ControlInfo->ControlHandle);
      gBS->FreePool (StrPtr);
      break;

    case EFI_IFR_BITWISE_NOT_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }
      if (Value->Type > EFI_IFR_TYPE_DATE) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
      Value->Value.u64 = ~Value->Value.u64;
      break;

    //
    // binary-op
    //
    case EFI_IFR_ADD_OP:
    case EFI_IFR_SUBTRACT_OP:
    case EFI_IFR_MULTIPLY_OP:
    case EFI_IFR_DIVIDE_OP:
    case EFI_IFR_MODULO_OP:
    case EFI_IFR_BITWISE_AND_OP:
    case EFI_IFR_BITWISE_OR_OP:
    case EFI_IFR_SHIFT_LEFT_OP:
    case EFI_IFR_SHIFT_RIGHT_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }
      if (Data2.Type > EFI_IFR_TYPE_DATE) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      //
      // Pop another expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }
      if (Data1.Type > EFI_IFR_TYPE_DATE) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;

      switch (ifrData->OpCode) {
        case EFI_IFR_ADD_OP:
          Value->Value.u64 = Data1.Value.u64 + Data2.Value.u64;
          break;

        case EFI_IFR_SUBTRACT_OP:
          Value->Value.u64 = Data1.Value.u64 - Data2.Value.u64;
          break;

        case EFI_IFR_MULTIPLY_OP:
          Value->Value.u64 = MultU64x32 (Data1.Value.u64, (UINT32)Data2.Value.u64);
          break;

        case EFI_IFR_DIVIDE_OP:
          Value->Value.u64 = AmiTseDivU64x32 (Data1.Value.u64, (UINT32)Data2.Value.u64,NULL);
          break;

        case EFI_IFR_MODULO_OP:
          Value->Value.u64 = 0;
          AmiTseDivU64x32(Data1.Value.u64, (UINT32)Data2.Value.u64, (UINTN*)&Value->Value.u64);
          break;

        case EFI_IFR_BITWISE_AND_OP:
          Value->Value.u64 = Data1.Value.u64 & Data2.Value.u64;
          break;

        case EFI_IFR_BITWISE_OR_OP:
          Value->Value.u64 = Data1.Value.u64 | Data2.Value.u64;
          break;

        case EFI_IFR_SHIFT_LEFT_OP:
          Value->Value.u64 = LShiftU64 (Data1.Value.u64, (UINTN) Data2.Value.u64);
          break;

        case EFI_IFR_SHIFT_RIGHT_OP:
          Value->Value.u64 = RShiftU64 (Data1.Value.u64, (UINTN) Data2.Value.u64);
          break;

        default:
          break;
      }
      break;

    case EFI_IFR_AND_OP:
    case EFI_IFR_OR_OP:
      //
      // Two Boolean operator
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }
      if (Data2.Type != EFI_IFR_TYPE_BOOLEAN) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      //
      // Pop another expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }
      if (Data1.Type != EFI_IFR_TYPE_BOOLEAN) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      if (ifrData->OpCode == EFI_IFR_AND_OP) {
        Value->Value.b = Data1.Value.b && Data2.Value.b;
      } else {
        Value->Value.b = Data1.Value.b || Data2.Value.b;
      }
      break;

    case EFI_IFR_EQUAL_OP:
    case EFI_IFR_NOT_EQUAL_OP:
    case EFI_IFR_GREATER_EQUAL_OP:
    case EFI_IFR_GREATER_THAN_OP:
    case EFI_IFR_LESS_EQUAL_OP:
    case EFI_IFR_LESS_THAN_OP:
      //
      // Compare two integer, string, boolean or date/time
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
	goto DONE;
      }
      if (Data2.Type > EFI_IFR_TYPE_BOOLEAN && Data2.Type != EFI_IFR_TYPE_STRING) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      //
      // Pop another expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
	goto DONE;
      }

      Result = CompareHiiValue (&Data1, &Data2, ControlInfo->ControlHandle);
      if (Result == EFI_INVALID_PARAMETER) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      switch (ifrData->OpCode) {
      case EFI_IFR_EQUAL_OP:
        Value->Value.b = (Result == 0) ? TRUE : FALSE;
        break;

      case EFI_IFR_NOT_EQUAL_OP:
        Value->Value.b = (Result != 0) ? TRUE : FALSE;
        break;

      case EFI_IFR_GREATER_EQUAL_OP:
        Value->Value.b = (Result >= 0) ? TRUE : FALSE;
        break;

      case EFI_IFR_GREATER_THAN_OP:
        Value->Value.b = (Result > 0) ? TRUE : FALSE;
        break;

      case EFI_IFR_LESS_EQUAL_OP:
        Value->Value.b = (Result <= 0) ? TRUE : FALSE;
        break;

      case EFI_IFR_LESS_THAN_OP:
        Value->Value.b = (Result < 0) ? TRUE : FALSE;
        break;

      default:
        break;
      }
      break;

    case EFI_IFR_MATCH_OP:
      Status = IfrMatch (ControlInfo->ControlHandle, Value);
      break;
    case EFI_IFR_MATCH2_OP:
    	//copy regular expression SyntexType (GUID) to formsetGUID
		MemCopy(&formSetGuid, &((EFI_IFR_MATCH2 *)ifrData)->SyntaxType, sizeof (EFI_GUID));
		Status = IfrMatch2 (ControlInfo,&formSetGuid, Value);
    	break;

    case EFI_IFR_CATENATE_OP:
      Status = IfrCatenate (ControlInfo->ControlHandle, Value);
      break;

    //
    // ternary-op
    //
    case EFI_IFR_CONDITIONAL_OP:
      //
      // Pop third expression from the expression stack
      //
      Status = PopExpression (&Data3);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }

      //
      // Pop second expression from the expression stack
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }

      //
      // Pop first expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto DONE;
      }
      if (Data1.Type != EFI_IFR_TYPE_BOOLEAN) {
        Status = EFI_INVALID_PARAMETER;
        goto DONE;
      }

      if (Data1.Value.b) {
        Value = &Data3;
      } else {
        Value = &Data2;
      }
      break;

    case EFI_IFR_FIND_OP:
      Status = IfrFind (ControlInfo->ControlHandle, ((EFI_IFR_FIND*)ifrData)->Format, Value);
      break;

    case EFI_IFR_MID_OP:
      Status = IfrMid (ControlInfo->ControlHandle, Value);
      break;

    case EFI_IFR_TOKEN_OP:
      Status = IfrToken (ControlInfo->ControlHandle, Value);
      break;

    case EFI_IFR_SPAN_OP:
      Status = IfrSpan (ControlInfo->ControlHandle, ((EFI_IFR_SPAN*)ifrData)->Flags, Value);
      break;

    default:
      Status = PopScope(&opCode);
      if ((ifrData->OpCode == EFI_IFR_END_OP)&&(inScope)) 
   		  inScope--;
      if (EFI_ERROR (Status))
      {
        goto DONE;
      }
      continue;
    }
    if (EFI_ERROR (Status))
    {
	  goto DONE;
    }

    Status = PushExpression (Value);
    if (EFI_ERROR (Status))
    {
      goto DONE;
    }

    i += ifrData->Length;
  }
  if ((NULL != ifrData) && (inScope))
	{
		if(ifrData->OpCode == EFI_IFR_END_OP)
			i += ifrData->Length;
	}

  //
  // Pop the final result from expression stack
  //
  Value = &Data1;
  Status = PopExpression (Value);
  if (EFI_ERROR (Status)) {
    goto DONE;
  }

  //
  // After evaluating an expression, there should be only one value left on the expression stack
  //
  if (PopExpression (Value) != EFI_ACCESS_DENIED)
  {
    Status = EFI_INVALID_PARAMETER;
    goto DONE;
  }

  EfiCopyMem (ExpressionResult, Value, sizeof (EFI_HII_VALUE));

DONE:
  MemFreePointer((void**)&questionValue);
  MemFreePointer((void**)&questionValue2);

  *Offset = i;
  gInconsistCondPtr = NULL;
  return Status;
}

/**
    Travels till the end of the current Expression and returns the offset

    @param ControlCondPtr Pointer to the Expression beginning.

    @retval UINTN Offsets to skip the current expression.
**/
UINTN _SkipExpression(UINT8 *ControlCondPtr)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )ControlCondPtr;
  UINTN offset = 0;
  INTN inScope = 0;

  // Skip the If condition OpCode
  if(
	 OpHeader->OpCode != EFI_IFR_SUPPRESS_IF_OP &&
	 OpHeader->OpCode != EFI_IFR_NO_SUBMIT_IF_OP &&
	 OpHeader->OpCode != EFI_IFR_INCONSISTENT_IF_OP &&
	 OpHeader->OpCode != EFI_IFR_GRAY_OUT_IF_OP &&
     OpHeader->OpCode != EFI_IFR_DISABLE_IF_OP &&
     OpHeader->OpCode != TSE_EFI_IFR_WARNING_IF_OP
	 )
  {
	// This should not happen. The code should halt here.
	offset = 0;
	goto DONE;
  }

  // Skip the If opcode
  offset = OpHeader->Length;
  OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 *)OpHeader + offset );

  do
  {
	switch( OpHeader->OpCode )
    {
	case EFI_IFR_END_OP:
	  inScope--;
	  break;
	default:
	  if(OpHeader->Scope)
		inScope++;
	  break;
	}
	offset += OpHeader->Length;
	OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 *)OpHeader + OpHeader->Length );
  }while(inScope > 0);

DONE:
  return offset;
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
