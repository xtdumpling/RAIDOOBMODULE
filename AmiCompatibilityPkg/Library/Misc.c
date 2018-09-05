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
// $Header: /Alaska/SOURCE/Core/Library/Misc.c 7     7/10/09 3:49p Felixp $
//
// $Revision: 7 $
//
// $Date: 7/10/09 3:49p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: Misc.c
//
// Description:	
//  Miscellaneous generic library functions.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Hob.h>
#include <AmiLib.h>

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FindNextHobByType
//
// Description:
//  EFI_STATUS FindNextHobByType(IN UINT16 Type, IN OUT VOID **Hob) finds a 
// HOB with a specified Type starting from the HOB that comes after the HOB
// pointed by *Hob.  *Hob pointer is updated with the address of the found
// HOB.
//
// Input:
//  IN UINT16 Type
// Type of HOB to return.
//
//  IN OUT VOID **Hob
// Address of the HOB if found.  Otherwise unchanged.
//
// Output:
//  EFI_INVALID_PARAMETER, if Hob = NULL.
//  EFI_NOT_FOUND,         if HOB of specified type not found.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  NextHob
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS FindNextHobByType(IN UINT16   Type, IN OUT VOID **Hob)
{
    EFI_HOB_GENERIC_HEADER *ThisHob;
    if (Hob == NULL) return EFI_INVALID_PARAMETER;
	ThisHob = *Hob;
    while(ThisHob->HobType != EFI_HOB_TYPE_END_OF_HOB_LIST) {
	  	ThisHob=NextHob(ThisHob,EFI_HOB_GENERIC_HEADER);
		if (ThisHob->HobType==Type) {
            *Hob=ThisHob;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FindNextHobByGuid
//
// Description:
//  EFI_STATUS FindNextHobByGuid(IN EFI_GUID *Guid, IN OUT VOID **Hob) finds
// HOB of type Guid Extention with a specified GUID starting from the HOB
// that comes after the HOB pointed by *Hob.  *Hob pointer is updated with
// the address of the found HOB.
//
// Input:
//  IN EFI_GUID *Guid 
// GUID for specific Guid Extention HOB.
//
//  IN OUT VOID **Hob 
// Address of the HOB if found.  Otherwise unchanged.
//
// Output:
//  EFI_INVALID_PARAMETER, if Hob = NULL.
//  EFI_NOT_FOUND,         if HOB of specified type not found.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  FindNextHobByType
//  guidcmp
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS FindNextHobByGuid(IN EFI_GUID *Guid, IN OUT VOID **Hob)
{
    EFI_HOB_GUID_TYPE *ThisHob;
    EFI_STATUS Status;

    if (!Guid||!Hob) return EFI_INVALID_PARAMETER;

    ThisHob=*Hob;

    for(;;){
        Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION,(VOID **)&ThisHob);
        if (EFI_ERROR(Status)) return Status;
        if (!guidcmp(&ThisHob->Name,Guid)) {
            *Hob = ThisHob;
            return EFI_SUCCESS;
        }
    }
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ExecScript
//
// Description:
//  EFI_STATUS ExecScript(IN IO_DATA *ScriptData, IN UINTN EntryCount)
// executes a sequence of CPU I/O operations.
//
// Input:
//  IN IO_DATA *ScriptData 
// Pointer to the first element of  an IO_DATA table.
//
//  IN UINTN EntryCount
// Number of entries in the IO_DATA table.
//
// Output:
//  EFI_INVALID_PARAMETER, if a width or address within the table is invalid.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  IoWrite
// 
// Notes:
// 
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS ExecScript(IO_DATA *ScriptData, UINTN EntryCount){
	UINTN		i;	
	EFI_STATUS	Status = EFI_SUCCESS;
//----------------------
	for(i=0; i<EntryCount;i++){
		Status=IoWrite(ScriptData[i].Width, (UINT64)ScriptData[i].Addr,1,(VOID*)&ScriptData[i].Data);
		if(EFI_ERROR(Status)) break;
	}
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
