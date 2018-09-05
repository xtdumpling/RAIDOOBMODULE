//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/SOURCE/Core/Library/EfiDBE.c 7     7/10/09 4:32p Felixp $
//
// $Revision: 7 $
//
// $Date: 7/10/09 4:32p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: EfiDBE.c
//
// Description:	
//  EFI Generic Database Engine serves all data storage, search and
// maintanance needs for Database Engine (DBE) objects.
//
//<AMI_FHDR_END>
//*************************************************************************
#include "AmiDxeLib.h"

//Debug switch to verify database indexes integrity\after each database transaction.
//#define BDE_DBG_VERIFY 1

//*************************************************************************
//<_AMI_GHDR_START>
//
// Name: Database_Functions
//
// Description:
//  A simple database API provided by the AMI library.
//
// Fields: Header Name Description
// ------------------------------------------------------------------
// AmiDxeLib DBE_DATABASE
// AmiDxeLib DBE_KEY_FIELD
// None      DbeBinSearch
// AmiDxeLib DbeInsert
// AmiDxeLib DbeLocateKey
// AmiDxeLib DbeGoToIndex
// AmiDxeLib DbeDelete
// 
// Notes:
//  Header details which header file contains the function prototype for
// the above functions; append .h to the given name.  None indicates that
// no header file contains a function prototype.
//  
//<_AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<_AMI_SHDR_START>
//
// Name: DBE_CMP
//
// Description:
//  DBE_CMP is a function prototype structure for defining a function which
// takes in two records and returns: a positive INTN value if pRecord1 is
// greater than pRecord2, a negative INTN value if pRecord1 is less than
// pRecord2, or 0 if both records are equivalent.
//
// Input:
//
// Output:
//
// Modified:
//
// Referrals:
//  Database_Functions
//
// Notes:
//  Prototype provided in AmiDxeLib.h.  See Database_Functions for example
// usage.
//
//<_AMI_SHDR_END>
//*************************************************************************

//*************************************************************************
//<_AMI_SHDR_START>
//
// Name: DBE_KEY_FIELD
//
// Description:
//
// Fields: Name Type Description
// ------------------------------------------------------------------
// RRcmp    DBE_CMP 
// KRcmp    DBE_CMP 
// pContext VOID*
//
// Referrals:
//  Database_Functions
//
// Notes:
//  Prototype provided in AmiDxeLib.h.  See Database_Functions for example
// usage.
//
//<_AMI_SHDR_END>
//*************************************************************************

//*************************************************************************
//<_AMI_SHDR_START>
//
// Name: DBE_DATABASE
//
// Description:
//  Structure to hold database information.
//
// Fields: Name Type Description
// ------------------------------------------------------------------
// MemoryType   EFI_MEMORY_TYPE 
// InitialCount UINTN           Initial number of elements to allocate space for in the index array.
// RecordCount  UINTN           Number of records in database.
// KeyCount     UINTN           Number of keys in database.
// KeyField     DBE_KEY_FIELD*  
// IndexArray   VOID*           Address of the index array.
//
// Referrals:
//  Database_Functions
//
// Notes:
//  Prototype provided in AmiDxeLib.h.  See Database_Functions for example
// usage.
//
//<_AMI_SHDR_END>
//*************************************************************************

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: OffsetRRCmp
//
// Description:	
//  OffsetRRCmp(IN VOID *pContext, IN VOID *pRecord1, IN VOID *pRecord2)
// compares two DBE records' keys and returns 0 if they are equal, non-zero
// if not.
//
// Input:
//  IN VOID *pContext
// DBE_OFFSET_KEY_CONTEXT of this Database Engine (DBE).
//
//  IN VOID *pRecord1
// Pointer to the first record whose key is to be compared with pRecord2's
// key.
// 
// 
//  IN VOID *pRecord2
// Pointer to the second record whose key is to be compared with pRecord1's
// key.
//
// Output:
//  INTN value of 0 if both records have the same key value; otherwise,
// returns non-zero value.
//
// Modified:
//
// Referrals:
//  MemCmp
// 
// Notes:
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
INTN OffsetRRCmp(
	IN VOID *pContext, VOID *pRecord1, VOID *pRecord2
)
{
	DBE_OFFSET_KEY_CONTEXT *pOffsetKey = (DBE_OFFSET_KEY_CONTEXT*)pContext;
	INT64 r;
	switch (pOffsetKey->Size)
	{
		case 2: 
			r = (INT16)(*(UINT16*)((INT8*)pRecord1+pOffsetKey->Offset) - *(UINT16*)((INT8*)pRecord2+pOffsetKey->Offset)); 
			break;
		case 4: 
			r = (INT32)(*(UINT32*)((INT8*)pRecord1+pOffsetKey->Offset) - *(UINT32*)((INT8*)pRecord2+pOffsetKey->Offset)); 
			break;
		case 8: 
			r = *(UINT64*)((INT8*)pRecord1+pOffsetKey->Offset) - *(UINT64*)((INT8*)pRecord2+pOffsetKey->Offset); 
			break;
		default:
			return MemCmp((INT8*)pRecord1+pOffsetKey->Offset,
				  		  (INT8*)pRecord2+pOffsetKey->Offset,
				  		  pOffsetKey->Size
				   );
	}
	return (r>0) ? 1 : (r<0) ? -1 : 0;
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: OffsetKRCmp
//
// Description:
//  INTN OffsetKRCmp(IN DBE_OFFSET_KEY_CONTEXT *pContext, IN VOID *pKey,
// IN VOID *pRecord) compares a key with a record's key and returns 0 if
// they are equal, non-zero if not.
//
// Input:
//  IN DBE_OFFSET_KEY_CONTEXT *pContext
// DBE_OFFSET_KEY_CONTEXT of this Database Engine (DBE).
//
//  IN VOID *pKey
// Pointer to the key to be compared with pRecord's key.
//
//  IN VOID *pRecord
// Pointer to the record whose key is to be compared with pKey's key.
//
// Output:
//  INTN value of 0 if both keys have the same value; otherwise, returns
// non-zero value.
//
// Modified:
//
// Referrals:
//  MemCmp
// 
// Notes:
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
INTN OffsetKRCmp(
	IN DBE_OFFSET_KEY_CONTEXT *pContext, VOID *pKey, VOID *pRecord
)
{
	DBE_OFFSET_KEY_CONTEXT *pOffsetKey = (DBE_OFFSET_KEY_CONTEXT*)pContext;
	INT64 r;
	switch (pOffsetKey->Size)
	{
		case 2: 
			r = (INT16)(*(UINT16*)pKey - *(UINT16*)((INT8*)pRecord+pOffsetKey->Offset)); 
			break;
		case 4: 
			r = (INT32)(*(UINT32*)pKey - *(UINT32*)((INT8*)pRecord+pOffsetKey->Offset)); 
			break;
		case 8: 
			r = *(UINT64*)pKey - *(UINT64*)((INT8*)pRecord+pOffsetKey->Offset); 
			break;
		default:
			return MemCmp(pKey,
				  		  (INT8*)pRecord+pOffsetKey->Offset,
				  		  pOffsetKey->Size
				   );
	}
	return (r>0) ? 1 : (r<0) ? -1 : 0;
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: AddrRRCmp
//
// Description:
//  INTN AddrRRCmp(IN VOID* *pContext, IN VOID *pRecord1, IN VOID *pRecord2)
// compares two record pointers and returns 0 if they are equal, non-zero if
// not.
//
// Input:
//  IN VOID **pContext
// Unused.
//
//  IN VOID *pRecord1
// Pointer to the first record to be compared.
//
//  IN VOID *pRecord2
// Pointer to the second record to be compared.
//
// Output:
//  INTN value of 0 if both record pointers point to the same record;
// otherwise, returns non-zero value.
//
// Modified:
//
// Referrals:
// 
// Notes:
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
INTN AddrRRCmp(
	IN VOID* *pContext, VOID *pRecord1, VOID *pRecord2
)
{
	return (UINTN)pRecord1-(UINTN)pRecord2;
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: AddrKRCmp
//
// Description:
//  INTN AddrKRCmp(IN DBE_OFFSET_KEY_CONTEXT *pContext, IN VOID *pKey,
// IN VOID *pRecord) compares the a key and the address of a record and 
// returns 0 if they are equal, non-zero if not.
//
// Input:
//  IN DBE_OFFSET_KEY_CONTEXT *pContext
// Unused.
//
//  IN VOID *pKey
// Pointer to the key to be compared.
// 
//  IN VOID *pRecord
// Pointer to the record to be compared.
//
// Output:
//  INTN value of 0 if both the key's value and the address of the record are
// equal; otherwise, returns the key value minus the address of the record
// (*(UINTN*)pKey - pRecord).
//
// Modified:
//
// Referrals:
// 
// Notes:
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
INTN AddrKRCmp(
	IN DBE_OFFSET_KEY_CONTEXT *pContext, VOID *pKey, VOID *pRecord
)
{
	return (UINTN)*(UINTN*)pKey-(UINTN)pRecord;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DbeBinSearch
//
// Description:	
//  VOID DbeBinSearch(IN DBE_DATABASE *Database, IN UINTN KeyNo,
// IN DBE_CMP Cmp, IN VOID *NewKey, OUT VOID **DataLocation,
// OUT INT8 *Vicinity, OUT INTN *RecordInd OPTIONAL) executes a binary 
// search of the nearest location closest to the *NewKey value. If the 
// DBE_KEY_FIELD structure of the database is NULL, the NewKey field is the
// address of the *Data; otherwise the NewKey field will be calculated as 
// "(UINT8*)Data+KeyField.Offset" of "KeyField.Size" size.
//
// Input:
//  IN DBE_DATABASE *Database
// Address of the database to be searched.
//
//  IN UINTN KeyNo
// Starting key entry number.
//
//  IN DBE_CMP Cmp
// Address of function which will be used to compare entries.
//
//  IN VOID *NewKey
// Pointer to the Key Value we are comparing against.
//
// Output:
//  OUT VOID **DataLocation
// Pointer to location within Index Array of desired data.
//
//  OUT INT8 *Vicinity
// -1 if new entry should go before provided location, 0 if located, or 1
// if new entry should follow the provided location. 
// 
//  OUT INTN *RecordInd OPTIONAL
// If not NULL, provides the index position within Index Array of the
// desired data.
//
// Modified:
//
// Referrals:
// 
// Notes:
//          
//<AMI_PHDR_END>
//*************************************************************************

VOID DbeBinSearch(IN DBE_DATABASE	*Database,
				  IN  UINTN			KeyNo,
				  DBE_CMP			Cmp,
				  IN  VOID			*NewKey, 
				  OUT VOID			**DataLocation, 
				  OUT INT8			*Vicinity,
		          OUT INTN			*RecordInd      OPTIONAL)
{
//--------------------------------------------------
	UINTN			*IndexArray;
	DBE_KEY_FIELD	*KeyField = Database->KeyField+KeyNo;
	INTN			st=0,mid=0,end=Database->RecordCount-1;
	INTN			CmpResult;
//--------------------------------------------------
	IndexArray=(UINTN*)((UINTN)Database->IndexArray+Database->InitialCount*sizeof(VOID*)*KeyNo);
	
	if(!Database->RecordCount)*Vicinity=-1;
	else{ 
		while(st<=end){
			mid=(st+end+1)/2;
			CmpResult=Cmp(KeyField->pContext, NewKey, (VOID*)IndexArray[mid]);
			if(!CmpResult ){ if(mid==end) break; st = mid; }
			//New Key > Old Key we must go UP
			else if(CmpResult>0) st=mid+1;			
			//New Key < Old Key we must go DOWN
			else				  end=mid-1;
		}//while //else
		*Vicinity= (CmpResult > 0) ? 1 : (CmpResult < 0) ? -1 : 0;
	}
	*DataLocation=&IndexArray[mid];
	if(RecordInd)*RecordInd=(UINTN)mid;
	return;
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: Validate
//
// Description:
//  VOID Validate(IN DBE_DATABASE *Database) validates a Database Engine
// (DBE); ASSERTs if debug mode is on and any record within the DBE contains
// a key which is out of order from the other records.
//
// Input:
//  IN DBE_DATABASE *Database
// Database to be validated.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
// 
// Notes:
//  Only available if the BDE_DBG_VERIFY macro is defined as 1 in EfiDBE.c.
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
#if defined(BDE_DBG_VERIFY) && BDE_DBG_VERIFY
VOID Validate(DBE_DATABASE *Database){
	UINTN i,j;
	VOID **IndexArray = (VOID**)Database->IndexArray;
	if (Database->RecordCount<2) return;
	for (i=0;i<Database->KeyCount;i++){
		for(j=0;j<Database->RecordCount-1;j++)
			ASSERT(Database->KeyField[i].RRcmp(Database->KeyField[i].pContext,IndexArray[j],IndexArray[j+1])<=0)
		IndexArray+=Database->InitialCount;		
	}
}
#else
#define Validate(x)
#endif

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: DbeReallocIndexArray
//
// Description:
//  EFI_STATUS DbeReallocIndexArray(IN DBE_DATABASE *Database) doubles
// the database's index array size by reallocating twice as much memory,
// copying the contents of the old memory to the new memory, and updating the
// necessary DBE_DATABASE members.
//
// Input:
//  IN DBE_DATABASE *Database
// Database to have its index array doubled in size.
//
// Output:
//  EFI_OUT_OF_RESOURCES,   if the memory could not be allocated.
//  EFI_INVALID_PARAMETER,  if the previous index array was corrupted.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  MemSet
//  MemCpy
//  Validate
// 
// Notes:
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
EFI_STATUS DbeReallocIndexArray(DBE_DATABASE *Database){
	VOID			*New, *Old;
    UINTN           OldCount, NewCount, i;
	EFI_STATUS		Status;
//------------------------
	Validate(Database);
	Old=Database->IndexArray;
    OldCount=Database->InitialCount;
	NewCount = OldCount * 2;
	Status=pBS->AllocatePool(Database->MemoryType,NewCount*sizeof(VOID*)*Database->KeyCount,&New);
	if(EFI_ERROR(Status)) return Status;
	MemSet(New,NewCount*sizeof(VOID*)*Database->KeyCount,0);
	for(i=0; i<Database->KeyCount; i++)
		MemCpy((VOID**)New + NewCount*i,
			   (VOID**)Old + OldCount*i,
			   OldCount*sizeof(VOID*)
		);
	Database->IndexArray = New;
	Database->InitialCount = Database->InitialCount*2;
	Status=pBS->FreePool(Old);
	Validate(Database);
	return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DbeInsert
//
// Description:	
//  EFI_STATUS DbeInsert(IN DBE_DATABASE *Database, IN VOID *Record) inserts
// a user provided record into a database.
//
// Input:
//  IN DBE_DATABASE *Database
// The database to be inserted into.
//
//  IN VOID *Record
// The record to insert into Database.
//
// Output:
//  EFI_INVALID_PARAMETER,  if Database, Record, or Database->KeyField are
//                         NULL.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  DbeReallocIndexArray
//  DbeBinSearch
//  MemCpy
// 
// Notes:
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS DbeInsert(DBE_DATABASE *Database,VOID* Record){
	UINTN		i;
	EFI_STATUS	Status = EFI_SUCCESS;
//----------------------------
	if((!Database)||(!Record)) return EFI_INVALID_PARAMETER;

    if((Database->InitialCount-1)==Database->RecordCount){
		Status=DbeReallocIndexArray(Database);
		if(EFI_ERROR(Status)) return Status;
	}

	if(!Database->KeyField)return EFI_INVALID_PARAMETER;
	for (i=0;i<Database->KeyCount;i++){ 
		INTN				RecordIndex;	
		DBE_KEY_FIELD		*KeyField;
		INT8				Vicinity;
		VOID				*ptr;

		KeyField=Database->KeyField+i;
		//Search for the key...
		DbeBinSearch(Database,i,KeyField->RRcmp, Record, &ptr,&Vicinity,&RecordIndex);
	
		//if we stoped at the position but the "NewKey" we are about to insert bigger then the 
		//OldKey referenced by the "pos" NewKey has to be inserted after the pos. In case of 
		//lesser or equal it must be inserted at the current position
		if(Vicinity>=0){
			RecordIndex++;
			ptr = (UINTN*)ptr+1;
		}
		MemCpy((VOID*)(((UINTN*)ptr)+1),ptr,sizeof(UINTN)*(Database->RecordCount-RecordIndex));
		*(UINTN*)ptr=(UINTN)Record;
	}
	Database->RecordCount++;
	Validate(Database);
	return Status;	
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DbeLocateKey
//
// Description:
//  EFI_STATUS DbeLocateKey(IN DBE_DATABASE *Database, IN UINTN KeyNo,
// IN VOID *KeyValue, OUT VOID **Record, OUT INT8 *Vicinity,
// OUT UINTN *RecordIndex OPTIONAL) locates a record by its key in a
// database.
//
// Input:
//  IN DBE_DATABASE *Database
// Database to locate the key within.
//
//  IN UINTN KeyNo
// Key entry number to start searching from.  For example, 0 starts from the
// the first key entry; 1 starts from the second.
//
//  IN VOID *KeyValue
// Pointer to the key to search for.
//
//  OUT VOID **Record
// Output location for the pointer to the record which maps to the provided key.
//
//  OUT INT8 *Vicinity
// Relative location of the KeyValue in respect to the returned Record.  For
// instance, 0 means that the Record uses KeyValue, 1 means Record is the
// next highest entry above the desired KeyValue, and -1 means Record is the
// previous entry.  This should be 0 if the key was located.
//
//  OUT UINTN *RecordIndex OPTIONAL
// Index position within Index Array of desired record pointer.
//
// Output:
//  EFI_INVALID_PARAMETER,  if Database, Record, or Database->KeyField are
//                         NULL; also if KeyNo > Database->KeyCount - 1.
//  EFI_NOT_FOUND,          if key not found.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  DbeBinSearch
// 
// Notes:
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS DbeLocateKey(IN DBE_DATABASE *Database, 
						IN	UINTN		KeyNo, 
						IN	VOID		*KeyValue, 
						OUT VOID		**Record, 
						OUT INT8		*Vicinity, 
				        OUT	UINTN		*RecordIndex OPTIONAL)
{
//---------------------------------------------------------
	//Don't do extra stuff 
	if((!Database)||(!Record))return EFI_INVALID_PARAMETER;
    if((!Database->KeyField)||(KeyNo>Database->KeyCount-1)) return EFI_INVALID_PARAMETER;
	
	DbeBinSearch(Database,KeyNo,Database->KeyField[KeyNo].KRcmp,KeyValue,Record,Vicinity,(INTN*)RecordIndex);

	if(*Vicinity==0)return EFI_SUCCESS;
	else return EFI_NOT_FOUND;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DbeGoToIndex
//
// Description:
//  EFI_STATUS DbeGoToIndex(IN DBE_DATABASE *Database, IN UINTN KeyNo,
// IN INTN Index, OUT VOID **Record) returns the record pointer at the user
// provided index inside Database's IndexArray.
//
// Input:
//  IN DBE_DATABASE *Database
// Database to fetch the record pointer from.
//
//  IN UINTN KeyNo
// The key number of the desired IndexArray.
// 
//  IN INTN Index
// The index within the IndexArray to retrieve the record pointer from.
//
//  OUT VOID **Record
// Filled with the address of the record pointer associated with the provided
// Index.
//
// Output:
//  EFI_DBE_EOF,  if Index is negative or exceeds the number of valid
//               records.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS DbeGoToIndex(
                          IN DBE_DATABASE	*Database, 
						  IN UINTN			KeyNo, 
						  IN INTN			Index,
						  OUT VOID			**Record)
{
	UINTN			*IndexArray;
//-----------------------------------------------------
	IndexArray=(UINTN*)((UINTN)Database->IndexArray+Database->InitialCount*sizeof(VOID*)*KeyNo);
	
	if((UINTN)Index>=Database->RecordCount) {
		if (Database->RecordCount)
			*Record=(VOID*)IndexArray[Database->RecordCount-1];
		return EFI_DBE_EOF;
	}
	if(Index<0) {
		*Record=(VOID*)IndexArray[0];
		return EFI_DBE_BOF;
	}
	
	*Record=(VOID*)IndexArray[Index];
	return EFI_SUCCESS;
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: DbeDelIndexKeyEntry
//
// Description:
//  EFI_STATUS DbeDelIndexKeyEntry(IN DBE_DATABASE *Database, IN UINTN KeyNo,
// IN VOID *Data) deletes a record entry in Database associated with KeyNo
// containing Data.
//
// Input:
//  IN DBE_DATABASE *Database
// Database to delete the provided Data from.
//
//  IN UINTN KeyNo
// Key number within the Database to delete the Data from.
//
//  IN VOID *Data
// The data associated with the record to be deleted from Database.
//
// Output:
//  EFI_INVALID_PARAMETER,  if the desired Data to be deleted could not be
//                         located.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  MemCpy
// 
// Notes:
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
EFI_STATUS DbeDelIndexKeyEntry(IN DBE_DATABASE *Database, IN UINTN KeyNo, IN VOID* Data){
	UINTN				RecordIndex;	
	VOID				**IndexArray, **ptr;
//------------------------------------	
	IndexArray=(VOID**)((UINTN)Database->IndexArray+Database->InitialCount*sizeof(VOID*)*KeyNo);
	for(RecordIndex=0; RecordIndex < Database->RecordCount && IndexArray[RecordIndex]!=Data; RecordIndex++);
	if(IndexArray[RecordIndex]!=Data) return EFI_INVALID_PARAMETER;
	ptr = IndexArray+RecordIndex;
	MemCpy(ptr,ptr+1,sizeof(VOID*)*(Database->RecordCount-RecordIndex-1));
	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DbeDelete
//
// Description:
//  EFI_STATUS DbeDelete(IN DBE_DATABASE *Database, IN VOID *Data,
// IN BOOLEAN FreeData) removes all records associated with the user
// provided Data pointer.  Function assumes every keyIf FreeData is TRUE, it will also FreePool the
// Data when it is done.
//
// Input:
//  IN DBE_DATABASE *Database
// Database to remove the associated records from.
//
//  IN VOID *Data
// Data to check 
//  IN BOOLEAN FreeData
//
// Output:
//  EFI_INVALID_PARAMETER,  if the provided Database is empty.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  DbeDelIndexKeyEntry
//  Validate
//
// Notes:
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS DbeDelete(IN DBE_DATABASE *Database, IN VOID* Data, IN BOOLEAN FreeData){
	INTN				KeyNo;
	EFI_STATUS			Status=EFI_SUCCESS;
//------------------------------------	
	if (!Database->RecordCount) return EFI_INVALID_PARAMETER;
	for (KeyNo=0; KeyNo<(INTN)Database->KeyCount; KeyNo++){
		Status=DbeDelIndexKeyEntry(Database,KeyNo,Data);
		if(EFI_ERROR(Status)) return Status;
	}
	Database->RecordCount--;
	if(FreeData) pBS->FreePool(Data);
	Validate(Database);
	return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
