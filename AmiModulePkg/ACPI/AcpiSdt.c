

/** @file
  ACPI Sdt Protocol Driver

  Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved. <BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file AcpiSdt.c
    ACPI SDT protocol Implementation.

**/


//
// Includes
//
#include "AcpiCore.h"

extern ACPI_DB  gAcpiData;


/**
  This function finds the table specified by the buffer.

  @param[in]  Buffer      Table buffer to find.

  @return ACPI table list.
**/

EFI_ACPI_SDT_HEADER *
FindTableByBuffer (
  IN VOID  *Buffer
  )
{
  EFI_ACPI_SDT_HEADER       *Table;
  UINTN i;
  //
  // Get the instance of the ACPI Table
  //
  for (i = 0; i < gAcpiData.AcpiEntCount; i++)
  {
	Table = (EFI_ACPI_SDT_HEADER*)gAcpiData.AcpiEntries[i]->BtHeaderPtr;
    if (((UINTN)Table <= (UINTN)Buffer) &&
            (((UINTN)Table + Table->Length) > (UINTN)Buffer))
    {
      return Table;
    }
  }

  return NULL;
}

/**
  This function finds MAX AML buffer size.
  It will search the ACPI table installed by ACPI_TABLE protocol.

  @param[in]  Buffer        A piece of AML code buffer pointer.
  @param[out] MaxSize       On return it holds the MAX size of buffer.

  @retval EFI_SUCCESS       The table holds the AML buffer is found, and MAX size if returned.
  @retval EFI_NOT_FOUND     The table holds the AML buffer is not found.
**/

EFI_STATUS
SdtGetMaxAmlBufferSize (
  IN  VOID  *Buffer,
  OUT UINTN *MaxSize
  )
{
  EFI_ACPI_SDT_HEADER             *CurrentTable;

  CurrentTable = FindTableByBuffer (Buffer);
  if (CurrentTable == NULL) {
    return EFI_NOT_FOUND;
  }

  *MaxSize = (UINTN)CurrentTable + CurrentTable->Length - (UINTN)Buffer;
  return EFI_SUCCESS;
}

/**
  Create a handle for the first ACPI opcode in an ACPI system description table.
  
  @param[in]    TableKey    The table key for the ACPI table, as returned by GetTable().
  @param[out]   Handle      On return, points to the newly created ACPI handle.

  @retval EFI_SUCCESS       Handle created successfully.
  @retval EFI_NOT_FOUND     TableKey does not refer to a valid ACPI table.  
**/
EFI_STATUS
SdtOpenSdtTable (IN    UINTN           TableKey, OUT   EFI_ACPI_HANDLE *Handle)
{
    ACPI_TBL_ITEM             *TableItem;
    EFI_ACPI_SDT_HEADER       *Table;
    EFI_AML_HANDLE            *AmlHandle;
    UINTN                     i; 
//-----------------------

    // Get the instance of the ACPI Table
    i=FindAcpiTblByHandle(&TableKey);

    // Find the table
    if(i==ACPI_TABLE_NOT_FOUND) return EFI_NOT_FOUND;

    TableItem=gAcpiData.AcpiEntries[i];
    Table=(EFI_ACPI_SDT_HEADER*)TableItem->BtHeaderPtr;
    
    if(Table->Signature!=DSDT_SIG && Table->Signature!=SSDT_SIG)
    	return EFI_NOT_FOUND;

    AmlHandle = MallocZ(sizeof(*AmlHandle));
    ASSERT (AmlHandle != NULL);
    if(AmlHandle == NULL) return EFI_OUT_OF_RESOURCES;


    AmlHandle->Signature       = EFI_AML_ROOT_HANDLE_SIGNATURE;
    AmlHandle->Buffer          = (VOID *)((UINTN)Table + sizeof(EFI_ACPI_SDT_HEADER));
    AmlHandle->Size            = Table->Length - sizeof(EFI_ACPI_SDT_HEADER);
    AmlHandle->AmlByteEncoding = NULL;
    AmlHandle->Modified        = FALSE;
    AmlHandle->Table           = Table;


    //
    // return the ACPI handle
    //
    *Handle = (EFI_ACPI_HANDLE)AmlHandle;

    return EFI_SUCCESS;
}

/**
  Create a handle for the first ACPI opcode in an ACPI system description table.
  
  @param[in]    TableKey    The table key for the ACPI table, as returned by GetTable().
  @param[out]   Handle      On return, points to the newly created ACPI handle.

  @retval EFI_SUCCESS       Handle created successfully.
  @retval EFI_NOT_FOUND     TableKey does not refer to a valid ACPI table.  
**/
EFI_STATUS
EFIAPI
OpenSdt (
  IN    UINTN           TableKey,
  OUT   EFI_ACPI_HANDLE *Handle
  )
{
  if (Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return SdtOpenSdtTable (TableKey, Handle);
}


/**
  Create a handle from an ACPI opcode
  
  @param[in]  Buffer                 Points to the ACPI opcode.
  @param[in]  BufferSize             Max buffer size.
  @param[out] Handle                 Upon return, holds the handle.
  
  @retval   EFI_SUCCESS             Success
  @retval   EFI_INVALID_PARAMETER   Buffer is NULL or Handle is NULL or Buffer points to an
                                    invalid opcode.
  
**/
EFI_STATUS
SdtOpenEx (
  IN    VOID            *Buffer,
  IN    UINTN           BufferSize,
  OUT   EFI_ACPI_HANDLE *Handle 
  )
{
  AML_BYTE_ENCODING   *AmlByteEncoding;
  EFI_AML_HANDLE      *AmlHandle;

  AmlByteEncoding = AmlSearchByOpByte (Buffer);
  if (AmlByteEncoding == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Do not open NameString as handle
  //
  if ((AmlByteEncoding->Attribute & AML_IS_NAME_CHAR) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Good, find it
  //
  AmlHandle = Malloc (sizeof(*AmlHandle));
  ASSERT (AmlHandle != NULL);
  
  AmlHandle->Signature       = EFI_AML_HANDLE_SIGNATURE;
  AmlHandle->Buffer          = Buffer;
  AmlHandle->AmlByteEncoding = AmlByteEncoding;
  AmlHandle->Modified        = FALSE;

  AmlHandle->Size = AmlGetObjectSize (AmlByteEncoding, Buffer, BufferSize);
  if (AmlHandle->Size == 0) {
    pBS->FreePool (AmlHandle);
    return EFI_INVALID_PARAMETER;
  }

  *Handle = (EFI_ACPI_HANDLE)AmlHandle;

  return EFI_SUCCESS;
}

/**
  Create a handle from an ACPI opcode
  
  @param[in]  Buffer                 Points to the ACPI opcode.
  @param[out] Handle                 Upon return, holds the handle.
  
  @retval   EFI_SUCCESS             Success
  @retval   EFI_INVALID_PARAMETER   Buffer is NULL or Handle is NULL or Buffer points to an
                                    invalid opcode.
  
**/
EFI_STATUS
EFIAPI
Open (
  IN    VOID            *Buffer,
  OUT   EFI_ACPI_HANDLE *Handle 
  )
{

  EFI_STATUS          Status;
  UINTN               MaxSize;

  MaxSize = 0;

  //
  // Check for invalid input parameters
  //
  if (Buffer == NULL || Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SdtGetMaxAmlBufferSize (Buffer, &MaxSize);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  return SdtOpenEx (Buffer, MaxSize, Handle);

}

/**
  Close an ACPI handle.
  
  @param[in] Handle Returns the handle.
  
  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER Handle is NULL or does not refer to a valid ACPI object.  
**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_ACPI_HANDLE Handle
  )
{
  EFI_AML_HANDLE      *AmlHandle;

  //
  // Check for invalid input parameters
  //
  if (Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AmlHandle = (EFI_AML_HANDLE *)Handle;
  if ((AmlHandle->Signature != EFI_AML_ROOT_HANDLE_SIGNATURE) &&
      (AmlHandle->Signature != EFI_AML_HANDLE_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Update Checksum only if modified
  //
  if (AmlHandle->Modified) {
	  EFI_ACPI_SDT_HEADER             *CurrentTable;  
	  CurrentTable = FindTableByBuffer (AmlHandle->Buffer);
	  if (CurrentTable == NULL) {
	        return EFI_INVALID_PARAMETER;
	    }
	  CurrentTable->Checksum=0;
	  CurrentTable->Checksum=ChsumTbl((UINT8*)CurrentTable, CurrentTable->Length);
  }

  pBS->FreePool(AmlHandle);

  return EFI_SUCCESS;
}

/**
  Retrieve information about an ACPI object.
  
  @param[in]    Handle      ACPI object handle.
  @param[in]    Index       Index of the data to retrieve from the object. In general, indexes read from left-to-right
                            in the ACPI encoding, with index 0 always being the ACPI opcode.
  @param[out]   DataType    Points to the returned data type or EFI_ACPI_DATA_TYPE_NONE if no data exists
                            for the specified index.
  @param[out]   Data        Upon return, points to the pointer to the data.
  @param[out]   DataSize    Upon return, points to the size of Data.
  
  @retval       EFI_SUCCESS           Success.
  @retval       EFI_INVALID_PARAMETER Handle is NULL or does not refer to a valid ACPI object.
**/
EFI_STATUS
EFIAPI
GetOption (
  IN        EFI_ACPI_HANDLE     Handle,
  IN        UINTN               Index,
  OUT       EFI_ACPI_DATA_TYPE  *DataType,
  OUT CONST VOID                **Data,
  OUT       UINTN               *DataSize
  )
{
  EFI_AML_HANDLE      *AmlHandle;
  AML_BYTE_ENCODING   *AmlByteEncoding;
  EFI_STATUS          Status;

  ASSERT (DataType != NULL);
  ASSERT (Data != NULL);
  ASSERT (DataSize != NULL);

  //
  // Check for invalid input parameters
  //
  if (Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AmlHandle = (EFI_AML_HANDLE *)Handle;
  //
  // Do not check EFI_AML_ROOT_HANDLE_SIGNATURE because there is no option for Root handle
  //
  if (AmlHandle->Signature != EFI_AML_HANDLE_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (FindTableByBuffer (AmlHandle->Buffer) == NULL)
    return EFI_INVALID_PARAMETER;
    	
  AmlByteEncoding = AmlHandle->AmlByteEncoding;
  if (Index > AmlByteEncoding->MaxIndex) {
    *DataType = EFI_ACPI_DATA_TYPE_NONE;
    return EFI_SUCCESS;
  }

  //
  // Parse option
  //
  Status = AmlParseOptionHandleCommon (AmlHandle, (AML_OP_PARSE_INDEX)Index, DataType, (VOID **)Data, DataSize);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Change information about an ACPI object.
  
  @param[in]  Handle    ACPI object handle.
  @param[in]  Index     Index of the data to retrieve from the object. In general, indexes read from left-to-right
                        in the ACPI encoding, with index 0 always being the ACPI opcode.
  @param[in]  Data      Points to the data.
  @param[in]  DataSize  The size of the Data.

  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER Handle is NULL or does not refer to a valid ACPI object.
  @retval EFI_BAD_BUFFER_SIZE   Data cannot be accommodated in the space occupied by
                                the option.

**/
EFI_STATUS
EFIAPI
SetOption (
  IN        EFI_ACPI_HANDLE Handle,
  IN        UINTN           Index,
  IN CONST  VOID            *Data,
  IN        UINTN           DataSize
  )
{
  EFI_AML_HANDLE      *AmlHandle;
  AML_BYTE_ENCODING   *AmlByteEncoding;
  EFI_STATUS          Status;
  EFI_ACPI_DATA_TYPE  DataType;
  VOID                *OrgData;
  UINTN               OrgDataSize;

  ASSERT (Data != NULL);

  //
  // Check for invalid input parameters
  //
  if (Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AmlHandle = (EFI_AML_HANDLE *)Handle;
  //
  // Do not check EFI_AML_ROOT_HANDLE_SIGNATURE because there is no option for Root handle
  //
  if (AmlHandle->Signature != EFI_AML_HANDLE_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (FindTableByBuffer (AmlHandle->Buffer) == NULL)
   	return EFI_INVALID_PARAMETER;
   	
  AmlByteEncoding = AmlHandle->AmlByteEncoding;

  if (Index > AmlByteEncoding->MaxIndex) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Parse option
  //
  Status = AmlParseOptionHandleCommon (AmlHandle, (AML_OP_PARSE_INDEX)Index, &DataType, &OrgData, &OrgDataSize);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }
  if (DataType == EFI_ACPI_DATA_TYPE_NONE) {
    return EFI_INVALID_PARAMETER;
  }

  if (DataSize > OrgDataSize) {
    return EFI_BAD_BUFFER_SIZE;
  }

  //
  // Update
  //

  pBS->CopyMem(OrgData, (VOID*)Data, DataSize);
  AmlHandle->Modified = TRUE;

  return EFI_SUCCESS;
}

/**
  Return the child ACPI objects.
  
  @param[in]        ParentHandle    Parent handle.
  @param[in, out]   Handle          On entry, points to the previously returned handle or NULL to start with the first
                                    handle. On return, points to the next returned ACPI handle or NULL if there are no
                                    child objects.

  @retval EFI_SUCCESS               Success
  @retval EFI_INVALID_PARAMETER     ParentHandle is NULL or does not refer to a valid ACPI object.                                
**/

EFI_STATUS
EFIAPI
GetChild (
  IN EFI_ACPI_HANDLE        ParentHandle,
  IN OUT EFI_ACPI_HANDLE    *Handle
  )
{
  EFI_AML_HANDLE      *AmlParentHandle;
  EFI_AML_HANDLE      *AmlHandle;
  VOID                *Buffer;
  EFI_STATUS          Status;

  ASSERT (Handle != NULL);

  //
  // Check for invalid input parameters
  //
  if (ParentHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AmlHandle       = *Handle;
  if ((AmlHandle != NULL) && (AmlHandle->Signature != EFI_AML_HANDLE_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }

  AmlParentHandle = (EFI_AML_HANDLE *)ParentHandle;

  if (FindTableByBuffer (AmlParentHandle->Buffer) == NULL)
  	return EFI_INVALID_PARAMETER;
  	
  if (AmlParentHandle->Signature == EFI_AML_ROOT_HANDLE_SIGNATURE) {
    //
    // Root handle
    //
    Status = AmlGetChildFromRoot (AmlParentHandle, AmlHandle, &Buffer);
  } else if (AmlParentHandle->Signature == EFI_AML_HANDLE_SIGNATURE) {
    //
    // Non-root handle
    //
    Status = AmlGetChildFromNonRoot (AmlParentHandle, AmlHandle, &Buffer);
  } else {
    //
    // Invalid
    //
    return EFI_INVALID_PARAMETER;
  }

  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Buffer == NULL) {
    *Handle = NULL;
    return EFI_SUCCESS;
  }
  return SdtOpenEx (Buffer, (UINTN)AmlParentHandle->Buffer + AmlParentHandle->Size - (UINTN)Buffer, Handle);
}


/**
  Returns the handle of the ACPI object representing the specified ACPI path
  
  @param[in]    HandleIn    Points to the handle of the object representing the starting point for the path search.
  @param[in]    AmlPath     Points to the AML path.
  @param[out]   HandleOut   On return, points to the ACPI object which represents AcpiPath, relative to
                            HandleIn.
                            
  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER HandleIn is NULL or does not refer to a valid ACPI object.                            
**/

EFI_STATUS
SdtFindPathFromNonRoot (
  IN    EFI_ACPI_HANDLE HandleIn,
  IN    UINT8           *AmlPath,
  OUT   EFI_ACPI_HANDLE *HandleOut
  )
{
  EFI_AML_HANDLE      *AmlHandle;
  VOID                *Buffer;
  EFI_STATUS          Status;

  Buffer = NULL;
  AmlHandle = (EFI_AML_HANDLE *)HandleIn;

  //
  // For non-root handle, we need search from THIS node instead of ROOT.
  //
  Status = AmlFindPath (AmlHandle, AmlPath, &Buffer, FALSE);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Buffer == NULL) {
    *HandleOut = NULL;
    return EFI_SUCCESS;
  }
  return SdtOpenEx (Buffer, (UINTN)AmlHandle->Buffer + AmlHandle->Size - (UINTN)Buffer, HandleOut);
}


/**
  Duplicate AML handle.
  
  @param[in]    AmlHandle   Handle to be duplicated.
                            
  @return Duplicated AML handle.
**/

EFI_AML_HANDLE *
SdtDuplicateHandle (
  IN EFI_AML_HANDLE      *AmlHandle
  )
{
  EFI_AML_HANDLE  *DstAmlHandle;

  DstAmlHandle = Malloc(sizeof(*DstAmlHandle));
  ASSERT (DstAmlHandle != NULL);
  pBS->CopyMem(DstAmlHandle, (VOID *)AmlHandle, sizeof(*DstAmlHandle));

  return DstAmlHandle;
}

/**
  Returns the handle of the ACPI object representing the specified ACPI path
  
  @param[in]    HandleIn    Points to the handle of the object representing the starting point for the path search.
  @param[in]    AmlPath     Points to the AML path.
  @param[out]   HandleOut   On return, points to the ACPI object which represents AcpiPath, relative to
                            HandleIn.
                            
  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER HandleIn is NULL or does not refer to a valid ACPI object.                            
**/

EFI_STATUS
SdtFindPathFromRoot (
  IN    EFI_ACPI_HANDLE HandleIn,
  IN    UINT8           *AmlPath,
  OUT   EFI_ACPI_HANDLE *HandleOut
  )
{
  EFI_ACPI_HANDLE     ChildHandle;
  EFI_AML_HANDLE      *AmlHandle;
  EFI_STATUS          Status;
  VOID                *Buffer;

  Buffer = NULL;
  AmlHandle = (EFI_AML_HANDLE *)HandleIn;

  //
  // Handle case that AcpiPath is Root
  //
  if (AmlIsRootPath (AmlPath)) {
    //
    // Duplicate RootHandle
    //
    *HandleOut = (EFI_ACPI_HANDLE)SdtDuplicateHandle (AmlHandle);
    return EFI_SUCCESS;
  }

  //
  // Let children find it.
  //
  ChildHandle = NULL;
  while (TRUE) {
    Status = GetChild (HandleIn, &ChildHandle);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    if (ChildHandle == NULL) {
      //
      // Not found
      //
      *HandleOut = NULL;
      return EFI_SUCCESS;
    }

    //
    // More child
    //
    AmlHandle = (EFI_AML_HANDLE *)ChildHandle;
    Status = AmlFindPath (AmlHandle, AmlPath, &Buffer, TRUE);
    if (EFI_ERROR (Status)) {
      return EFI_INVALID_PARAMETER;
    }

    if (Buffer != NULL) {
      //
      // Great! Find it, open
      //
      Status = SdtOpenEx (Buffer, (UINTN)AmlHandle->Buffer + AmlHandle->Size - (UINTN)Buffer, HandleOut);
      if (!EFI_ERROR (Status))  {
        return EFI_SUCCESS;
      }
      //
      // Not success, try next one
      //
    }
  }

  //
  // Should not run here
  //
}


/**
  Returns the handle of the ACPI object representing the specified ACPI path
  
  @param[in]    HandleIn    Points to the handle of the object representing the starting point for the path search.
  @param[in]    AcpiPath    Points to the ACPI path, which conforms to the ACPI encoded path format.
  @param[out]   HandleOut   On return, points to the ACPI object which represents AcpiPath, relative to
                            HandleIn.
                            
  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER HandleIn is NULL or does not refer to a valid ACPI object.                            
**/

EFI_STATUS
EFIAPI
FindPath (
  IN    EFI_ACPI_HANDLE HandleIn,
  IN    VOID            *AcpiPath,
  OUT   EFI_ACPI_HANDLE *HandleOut
  )
{
  EFI_AML_HANDLE      *AmlHandle;
  EFI_STATUS          Status;
  UINT8               *AmlPath;

  //
  // Check for invalid input parameters
  //
  if (HandleIn == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AmlHandle = (EFI_AML_HANDLE *)HandleIn;
  
  if (FindTableByBuffer (AmlHandle->Buffer) == NULL)
  	return EFI_INVALID_PARAMETER;
  //
  // Convert ASL path to AML path
  //
  AmlPath = AmlNameFromAslName (AcpiPath);
  if (AmlPath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

//  DEBUG_CODE_BEGIN ();
  TRACE ((-1, "AcpiSdt: FindPath - "));
  AmlPrintNameString (AmlPath);
  TRACE ((-1, "\n"));
//  DEBUG_CODE_END ();

  if (AmlHandle->Signature == EFI_AML_ROOT_HANDLE_SIGNATURE) {
    //
    // Root Handle
    //
    Status = SdtFindPathFromRoot (HandleIn, AmlPath, HandleOut);
  } else if (AmlHandle->Signature == EFI_AML_HANDLE_SIGNATURE) {
    //
    // Non-Root handle
    //
    Status = SdtFindPathFromNonRoot (HandleIn, AmlPath, HandleOut);
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  pBS->FreePool (AmlPath);

  return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
