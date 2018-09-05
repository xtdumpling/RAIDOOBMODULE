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

/** @file NvramRead.c
    NVRAM data area access API

**/
//**********************************************************************
#include "NVRAM.h"

//Definitions for Variables that are used
//to store nested defaults images
// {4599D26F-1A11-49b8-B91F-858745CFF824}
const EFI_GUID AmiDefaultsVariableGuid = 
    { 0x4599d26f, 0x1a11, 0x49b8, { 0xb9, 0x1f, 0x85, 0x87, 0x45, 0xcf, 0xf8, 0x24 } };
const CHAR16 MfgDefaults[] = L"MfgDefaults";
const CHAR16 StdDefaults[] = L"StdDefaults";
const EFI_GUID AmiNvramHobGuid = NVRAM_HOB_GUID;

//Low level access routines

/**
    This function returns GUID of a specified variable. 

    @param pVar pointer to Var 
    @param pInfo pointer to NVRAM store structure

    @retval EFI_GUID pointer to the Var GUID

**/
EFI_GUID* NvGetGuid(NVAR* pVar, NVRAM_STORE_INFO *pInfo)
{
    if (pVar->flags&NVRAM_FLAG_GUID) return (EFI_GUID*)(pVar+1);
    else return pInfo->NvramGuidsAddress-*(UINT8*)(pVar+1);
}

/**
    This function returns pointer to the Var name. 

    @param pVar pointer to the Var 

    @retval VOID pointer to the Var name

**/
VOID* NvGetName(NVAR* pVar)
{
    return (INT8*)(pVar+1)+(pVar->flags&NVRAM_FLAG_GUID ? sizeof(EFI_GUID) : 1);
}

/**
    This function returns last record in the NVRAM storage associated with a specified variable. 

    @param pVar pointer to Var 
    @param pInfo pointer to NVRAM store structure

    @retval NVAR pointer to the last Var

**/
NVAR* NvGetDataNvar(NVAR *pVar, NVRAM_STORE_INFO *pInfo)
{
    NVAR *pLastVar=NULL;
    
    for (
        ; NvIsVariable(pVar,pInfo) // Check if pVar record is correct
        ; pVar = (NVAR*)((UINT8*)pVar + NvNext(pVar))
    )
    {
        if (NvNext(pVar)==FLASH_EMPTY_NEXT) return pVar;
        pLastVar=pVar;
    }
    return pLastVar;
}

/**
    This function returns pointer to Ext Size field in Var

    @param NvStart pointer to the Var to checksum

    @retval 
          Pointer to Ext Size field in Var

**/
VAR_SIZE_TYPE* NvGetExtSize (NVAR* pVar)
{
    return (VAR_SIZE_TYPE*)( (UINT8*)pVar + NvSize(pVar) - sizeof(VAR_SIZE_TYPE) );
}

/**
    This function returns pointer to Ext Flags field in Var

    @param NvStart pointer to the Var to checksum

    @retval Pointer to Ext Flags field in Var

**/
UINT8* NvGetExtFlags (NVAR* pVar)
{
	VOID* ExtSizePtr = NvGetExtSize(pVar);
    return (UINT8*)( (UINT8*)ExtSizePtr - ReadUnaligned16(ExtSizePtr) + sizeof(VAR_SIZE_TYPE) );
}

/**
    This function calculates and Checks the checksum of the Var

    @param NvStart pointer to the Var to checksum
    @param Calculate if TRUE - skip checksum field in Var

    @retval 
         Checksum value

**/
UINT8 NvCalculateNvarChecksum(NVAR* pVar)
{
    UINT8      *p, *Start;
	UINT8		Checksum = 0;

	if (   !(pVar->flags & NVRAM_FLAG_EXT_HEDER)
        || !(*NvGetExtFlags(pVar) & NVRAM_EXT_FLAG_CHECKSUM)
    ) return 0;
    
    //calculate checksum of the variable excluding the header
    Start = (UINT8*)(pVar+1);
	for(p=Start; p < Start+NvSize(pVar)-sizeof(NVAR); p++) 
    {
        Checksum += *p;
    }
    //add variable size
    Start = (UINT8*)&pVar->size;
	for(p=Start; p < Start+sizeof(VAR_SIZE_TYPE); p++) 
    {
        Checksum += *p; 
    }
    //add flags except NVRAM_FLAG_VALID
    Checksum += pVar->flags;

	return 0 - Checksum;
}

/**
    This function returns last variable in NVRAM storage. 

    @param pVar pointer to Var 
    @param NameLength Length of the Var name
    @param pDataSize pointer to where the site of data returned will be store
    @param pInfo pointer to NVRAM store structure

    @retval VOID pointer to the Data

**/
VOID* NvGetData(NVAR* pVar, UINTN NameLength, UINTN* pDataSize, NVRAM_STORE_INFO *pInfo)
{
    UINTN DataOffset;
    UINTN ExtSize;

    pVar = NvGetDataNvar(pVar,pInfo);
    
    if (pVar==NULL) return NULL;
    
    DataOffset = sizeof(NVAR);
    
    if (!(pVar->flags&NVRAM_FLAG_DATA_ONLY))
    {
        DataOffset +=  NameLength
                       + (   pVar->flags&NVRAM_FLAG_GUID
                             ? sizeof(EFI_GUID) : 1
                         );
    }
    
    if (pDataSize) 
    {   // if Ext. Header present - get its size
        if (pVar->flags & NVRAM_FLAG_EXT_HEDER) ExtSize = NvExtSize(pVar);
        else ExtSize = 0;
        *pDataSize = NvSize(pVar) - DataOffset - ExtSize;
    }
    
    return (INT8*)pVar+DataOffset;
}

//Validation routines
/**
    This function checks if passed Var is valid. 

    @param pVar pointer to the Var 

    @retval BOOLEAN TRUE of FALSE based on result

**/
BOOLEAN NvIsValid(NVAR* pVar)
{
    return  ((pVar->flags^~FLASH_EMPTY_FLAG)&NVRAM_FLAG_VALID) &&
            !(pVar->flags&NVRAM_FLAG_DATA_ONLY);
}

/**
    This function checks if pVar record is correct. 

    @param pVar pointer to Var 
    @param pInfo pointer to NVRAM store structure

    @retval BOOLEAN TRUE of FALSE based on result

**/
BOOLEAN NvIsVariable(NVAR *pVar, NVRAM_STORE_INFO *pInfo)
{
	VAR_SIZE_TYPE Size = NvSize(pVar);
	
    return
           NvSignature(pVar)==NVAR_SIGNATURE
        && pVar->flags!=FLASH_EMPTY_FLAG
        && !(pVar->flags&~ALL_FLAGS)
        && Size!=FLASH_EMPTY_SIZE && Size>sizeof(NVAR)
        && Size<=(pInfo->pEndOfVars-(UINT8*)pVar)
        && (       NvNext(pVar)==FLASH_EMPTY_NEXT
               ||     NvNext(pVar)>=Size
                   && NvNext(pVar)<=(UINT32)(pInfo->pEndOfVars-(UINT8*)pVar)
           )
        && (      !(pVar->flags & NVRAM_FLAG_EXT_HEDER)
               || !(*NvGetExtFlags (pVar) & NVRAM_EXT_FLAG_CHECKSUM)
               || !(pVar->flags & NVRAM_FLAG_VALID)
               || !NvCalculateNvarChecksum(pVar)
           )
        ;
}

//Iteration routines
/**
    This function returns the next valid Var after pVar. 

    @param pVar pointer to Var 
    @param pInfo pointer to NVRAM store structure

    @retval NVAR pointer to next valid Var after pVar, NULL - if none

**/
NVAR* NvGetNextNvar(NVAR* pVar, NVRAM_STORE_INFO *pInfo)
{
	if (!pVar || pVar >= (NVAR*)pInfo->pEndOfVars) return NULL;
	
    if (NvIsVariable(pVar,pInfo))
    {
        pVar = (NVAR*)((UINT8*)pVar+NvSize(pVar));
        if (pVar >= (NVAR*)pInfo->pEndOfVars) return NULL;
    }
    
    do
    {
    	VAR_SIZE_TYPE Size;
    	
        if (pVar >= (NVAR*)pInfo->pEndOfVars) return NULL;
        
        //If we found a variable, we are done
        if (NvIsVariable(pVar,pInfo)) return pVar;
        Size = NvSize(pVar);
        //Otherwise, let's check if record size seems valid
        //If size is valid, skip corrupted NVAR
        //Otherwise give up and return NULL
        if (    Size!=FLASH_EMPTY_SIZE
                && Size>sizeof(NVAR)
                && Size<=(pInfo->pEndOfVars-(UINT8*)pVar)
           ) pVar = (NVAR*)((UINT8*)pVar+Size);
        else return NULL;
    }
    while (TRUE);
}

/**
    This function returns the next valid Var after pVar. 

    @param pVar pointer to Var 
    @param pInfo pointer to NVRAM store structure

    @retval NVAR pointer to next valid Var after pVar, NULL - if none

**/
NVAR* NvGetNextValid(NVAR* pVar, NVRAM_STORE_INFO *pInfo)
{
    do
    {
        pVar = NvGetNextNvar(pVar,pInfo);
    }
    while (pVar && !NvIsValid(pVar));
    
    return pVar;
}

/**
    This function returns the first valid Var in NVRAM. 

    @param pInfo pointer to NVRAM store structure

    @retval NVAR pointer to first valid Var in NVRAM, NULL - if none

**/
NVAR* NvGetFirstValid(NVRAM_STORE_INFO *pInfo)
{
    NVAR* pNvar = (NVAR*)pInfo->pFirstVar;
    
    if (!NvIsVariable(pNvar,pInfo)) return NULL;
    
    if (NvIsValid(pNvar)) return pNvar;
    
    return NvGetNextValid(pNvar,pInfo);
}

//Comparison routines
/**
    This function checks if pNvar attributes are the same as the Attributes passed. 

    @param pNVar pointer to Var 
    @param Attributes Attributes of Var
    @param pInfo pointer to NVRAM store structure

    @retval NVAR pointer to next valid Var after pVar, NULL - if none

**/
BOOLEAN NvAttribEq(NVAR* pNvar, UINT32 Attributes, NVRAM_STORE_INFO *pInfo)
{
    //--- By ! we convert integer to boolean
    return        !(pNvar->flags & NVRAM_FLAG_RUNTIME)
              ==  !(Attributes & EFI_VARIABLE_RUNTIME_ACCESS)
            &&    !(pInfo->Flags & NVRAM_STORE_FLAG_NON_VALATILE)
              ==  !(Attributes & EFI_VARIABLE_NON_VOLATILE)
            &&    !(pNvar->flags & NVRAM_FLAG_HARDWARE_ERROR_RECORD)
              ==  !(Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD)
            &&    !(pNvar->flags & NVRAM_FLAG_AUTH_WRITE)
              ==  !(Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES)
           ;
}

/**
    This function verifies if two Var are the same

    @param Nvar1 pointer to the first Var 
    @param Info1 pointer to the first NVRAM_STORE_INFO structure
    @param Nvar2 pointer to the second Var 
    @param Info2 pointer to the second NVRAM_STORE_INFO structure

    @retval VOID

**/
BOOLEAN NvarEqNvar(
    NVAR *Nvar1, NVRAM_STORE_INFO *Info1,
    NVAR *Nvar2, NVRAM_STORE_INFO *Info2
)
{
    EFI_GUID *Guid1, *Guid2;
    UINT8 *Name1, *Name2;
    BOOLEAN AsciiName = Nvar1->flags&NVRAM_FLAG_ASCII_NAME;

    if (AsciiName != (Nvar2->flags&NVRAM_FLAG_ASCII_NAME)) return FALSE;

    if (   (Info1->Flags & NVRAM_STORE_FLAG_NON_VALATILE)
            != (Info2->Flags & NVRAM_STORE_FLAG_NON_VALATILE)
       ) return FALSE;

    Guid1=NvGetGuid(Nvar1,Info1);
    Guid2=NvGetGuid(Nvar2,Info2);

    if (guidcmp(Guid1,Guid2)) return FALSE;

    Name1=(UINT8*)NvGetName(Nvar1);
    Name2=(UINT8*)NvGetName(Nvar2);

    if (AsciiName)
    {
        while (*Name1 && *Name1==*Name2) {Name1++; Name2++;}
        if (*Name1!=*Name2) return FALSE;
    }

    else
    {
    	// Unaligned word access causes exception on ARM.
    	// Since Name1 and Name2 may be not word aligned, we are doing byte comparison.
        while ((Name1[0]||Name1[1]) && Name1[0]==Name2[0] && Name1[1]==Name2[1]) {Name1+=2; Name2+=2;}
        if (Name1[0]!=Name2[0] || Name1[1]!=Name2[1]) return FALSE;
    }

    return TRUE;
}

/**
    This function checks if pNvar GUID and Name are the same as the with passed ones.

    @param pNVar pointer to Var 
    @param sName Name of the Var to compare 
    @param pGuid pointer to GUID to compare with
    @param pNameSize Pointer to memory where Name size will be returned
    @param pInfo pointer to NVRAM store structure

    @retval BOOLEAN TRUE of FALSE based on result

**/
BOOLEAN NvVarEq(NVAR* pNvar, CHAR16* sName, EFI_GUID* pGuid, UINTN* pNameSize, NVRAM_STORE_INFO *pInfo)
{
    UINT8 *pVarGuid, *pNameStart, *pN;
    pVarGuid=(UINT8*)NvGetGuid(pNvar,pInfo);
    pN=(UINT8*)NvGetName(pNvar);
    pNameStart=pN;
    //-- Check name if ASCII
    if (pNvar->flags&NVRAM_FLAG_ASCII_NAME)
    {
        while (*pN && *pN==*sName) {pN++; sName++;}
        
        if (*pN!=*sName) return FALSE;
        
        pN++;
    }
    
    else   //-- Check name if Unicode
    {
    	// Unaligned word access causes exception on ARM.
    	// Since pN may be not word aligned, we are using unaligned access functions.
        CHAR16 *sN=(CHAR16*)pN;
        
        while (ReadUnaligned16(sN) && ReadUnaligned16(sN)==*sName) {sN++; sName++;}
        
        if (ReadUnaligned16(sN)!=*sName) return FALSE;
        
        pN=(UINT8*)++sN;
    }
    
    if (!guidcmp(pVarGuid,pGuid))//---Check GUID
    {
        if (pNameSize) *pNameSize=pN-pNameStart;
        
        return TRUE;
    }
    
    return FALSE;
}

//High level routines that work with a single NV store
/**
    This function searches for Var with specific GUID and Name. 

    @param sName Name of the Var to search 
    @param pGuid pointer to GUID to search
    @param pNameSize Pointer to memory where Name size will be returned
    @param pInfo pointer to NVRAM store structure

    @retval VOID Pointer to found Var, NULL - if not found

**/
VOID* NvFindVariable(
    CHAR16* sName, EFI_GUID* pGuid,
    UINTN* pNameSize, NVRAM_STORE_INFO *pInfo
)
{
    NVAR* pNvar = NvGetFirstValid(pInfo);
    
    for (; pNvar; pNvar=NvGetNextValid(pNvar,pInfo))
    {
        if (NvVarEq(pNvar,sName,pGuid,pNameSize,pInfo)) return pNvar;
    }
    
    return NULL;
}

/**
    This function searches for Var equal to one passed. 

    @param SourceNvar pointer to Var, which need to be found 
    @param SourceInfo pointer to source NVRAM store structure
    @param DestInfo pointer to NVRAM store structure where to search

    @retval VOID Pointer to found Var, NULL - if not found

**/
VOID* NvFindVariableByNvar(NVAR* SourceNvar, 
        NVRAM_STORE_INFO *SourceInfo, NVRAM_STORE_INFO *DestInfo
)
{
    NVAR* pNvar = NvGetFirstValid(DestInfo);
    
    for (; pNvar; pNvar=NvGetNextValid(pNvar,DestInfo))
    {
        if (NvarEqNvar(SourceNvar, SourceInfo, pNvar, DestInfo)) return pNvar;
    }
    
    return NULL;
}

/**
    This function returns the attributes from a given Nvar

    @param pNVar pointer to Var 
    @param Attributes Pointer to memory where Attributes will be returned 
    @param pInfo pointer to NVRAM store structure

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetAttributesFromNvar(
    IN NVAR *pNvar, IN NVRAM_STORE_INFO *pInfo,
    OUT UINT32 *Attributes
)
{
    if (!pNvar || !pInfo || !Attributes || (pNvar->flags & NVRAM_FLAG_DATA_ONLY)) return EFI_INVALID_PARAMETER;
    
    *Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    if ( pNvar->flags & NVRAM_FLAG_RUNTIME )
        *Attributes|=EFI_VARIABLE_RUNTIME_ACCESS;
    if ( pInfo->Flags & NVRAM_STORE_FLAG_NON_VALATILE )
        *Attributes|=EFI_VARIABLE_NON_VOLATILE;
    if ( pNvar->flags & NVRAM_FLAG_HARDWARE_ERROR_RECORD )
        *Attributes|=EFI_VARIABLE_HARDWARE_ERROR_RECORD;
    if ( pNvar->flags & NVRAM_FLAG_AUTH_WRITE )
        *Attributes|= (UINT8)(*NvGetExtFlags (pNvar) & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES);
    
    return EFI_SUCCESS;
}

/**
    This function searches for Var with specific GUID and Name. 

    @param pNVar pointer to Var 
    @param NameSize size of the Var name
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var will be returned
    @param pInfo pointer to NVRAM store structure
    @param Flags Pointer to memory where Var Flags will be returned

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetVariableFromNvar(
    NVAR *pNvar, UINTN NameSize, OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data,
    IN NVRAM_STORE_INFO *pInfo, OUT UINT8 *Flags
)
{
    UINT8* pN;
    UINTN Size;
    
    if (!pNvar || !NvIsVariable(pNvar,pInfo)) return EFI_NOT_FOUND;
    
    if (Attributes)
        NvGetAttributesFromNvar(pNvar, pInfo, Attributes);
    
    // Data
    pN = (UINT8*)NvGetData(pNvar,NameSize,&Size,pInfo);
    
    if (*DataSize<Size) { *DataSize=Size; return EFI_BUFFER_TOO_SMALL;}
    
    *DataSize=Size;
    MemCpy(Data,pN,Size);
    
    if (Flags) *Flags=pNvar->flags;
    
    return EFI_SUCCESS;
}

/**
    This function searches for a Var after Var  with specific GUID and Name. 

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var will be returned
    @param pInfo pointer to NVRAM store structure
    @param Var pointer to found Var variable  

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data,
    IN NVRAM_STORE_INFO *pInfo, OUT NVAR **Var
)
{
    UINTN NameSize;
    NVAR* pNvar;
    
    if (!VariableName || !VendorGuid || !DataSize || !Data && *DataSize)
        return EFI_INVALID_PARAMETER;
        
    pNvar = (NVAR*)NvFindVariable(VariableName,VendorGuid,&NameSize,pInfo);
    if ((Var != NULL) && (pNvar != NULL))
       *Var = pNvar;
    return NvGetVariableFromNvar(pNvar,NameSize,Attributes,DataSize,Data,pInfo, NULL);
}

/**
    This function searches for a Var after Var with specific GUID and returns it's Name. 

    @param VariableNameSize size of Varname - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param VariableName pointer where Var Name in Unicode will be stored
    @param VendorGuid pointer to memory where Var GUID will be stored
    @param pInfo pointer to NVRAM store structure
    @param Runtime search for Var with NVRAM_FLAG_RUNTIME set

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetNameFromNvar(
    IN NVAR *pNvar, IN OUT CHAR16 *VariableName,
    IN OUT UINTN *VariableNameSize,
    IN OUT EFI_GUID *VendorGuid, NVRAM_STORE_INFO *pInfo
)
{
    UINTN NameSize;
    UINT8 *pN, *pNameStart;
    EFI_GUID* pVarGuid;
    
    if (!pNvar || !VariableNameSize || !VariableName || !VendorGuid) return EFI_INVALID_PARAMETER;
    
    
    pN = pNameStart = (UINT8*)NvGetName(pNvar);
    
    if (pNvar->flags&NVRAM_FLAG_ASCII_NAME)
    {
        while (*pN++);
        
        NameSize = (pN - pNameStart)*2;
    }
    
    else
    {
        while (pN[0]||pN[1]) pN+=2;
        pN+=2; // skip terminating zero
        NameSize = pN - pNameStart;
    }
    
    if (NameSize>*VariableNameSize)
    {
        *VariableNameSize=NameSize;
        return EFI_BUFFER_TOO_SMALL;
    }
    
    *VariableNameSize=NameSize;
    pVarGuid=NvGetGuid(pNvar,pInfo);
    
    if (pNvar->flags&NVRAM_FLAG_ASCII_NAME) while (*VariableName++=*pNameStart++);
    else
    {
        CHAR16* sN = (CHAR16*)pNameStart;
        
        while (*VariableName++=ReadUnaligned16(sN++));
    }
    
    MemCpy(VendorGuid,pVarGuid,sizeof(EFI_GUID));
    pInfo->pLastReturned = pNvar;
    return EFI_SUCCESS;
}

/**
    This function searches for a Var after Var with specific GUID and returns 
    pointer to is using NextNvar parameter

               
    @param VariableName pointer to a Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param pInfo pointer to NVRAM store structure
    @param Runtime search for Var with NVRAM_FLAG_RUNTIME set
    @param NextNvar pointer to the next varaible's NVAR

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetNextVariableNvar(
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid,
    NVRAM_STORE_INFO *pInfo, BOOLEAN Runtime, NVAR **NextNvar
)
{
    NVAR* pNvar;

    if (!VariableName || !VendorGuid) return EFI_INVALID_PARAMETER;
    
    if (VariableName[0]==0)
    {
        pNvar = NvGetFirstValid(pInfo);
    }
    
    else
    {
        if (   pInfo->pLastReturned!=NULL
                && NvIsVariable(pInfo->pLastReturned, pInfo)
                && NvVarEq(pInfo->pLastReturned,VariableName,VendorGuid,NULL, pInfo)
           )
        {
            pNvar=pInfo->pLastReturned;
        }
        
        else
        {
            UINTN NameSize;
            pNvar=(NVAR*)NvFindVariable(VariableName,VendorGuid,&NameSize,pInfo);
            
            if (!pNvar) return EFI_INVALID_PARAMETER;
        }
        if (Runtime && !(pNvar->flags & NVRAM_FLAG_RUNTIME) ) return EFI_INVALID_PARAMETER;
        pNvar=NvGetNextValid(pNvar,pInfo);
    }
    
    if (!pNvar)
    {
        pInfo->pLastReturned = NULL;
        return EFI_NOT_FOUND;
    }
    
    if (Runtime)
    {
        while (pNvar && !(pNvar->flags & NVRAM_FLAG_RUNTIME))
            pNvar=NvGetNextValid(pNvar,pInfo);
            
        if (!pNvar)
        {
            pInfo->pLastReturned = NULL;
            return EFI_NOT_FOUND;
        }
    }
    if (NextNvar) *NextNvar = pNvar;
    return EFI_SUCCESS;
}

/**
    This function searches for a Var after Var with specific GUID and returns it's Name. 

    @param VariableNameSize size of Varname - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param VariableName pointer where Var Name in Unicode will be stored
    @param VendorGuid pointer to memory where Var GUID will be stored
    @param pInfo pointer to NVRAM store structure
    @param Runtime search for Var with NVRAM_FLAG_RUNTIME set

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetNextVariableName(
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid,
    NVRAM_STORE_INFO *pInfo, BOOLEAN Runtime
)
{
    NVAR* pNvar;
    EFI_STATUS Status;
    
    Status=NvGetNextVariableNvar(VariableName,VendorGuid,pInfo,Runtime,&pNvar);
    if (EFI_ERROR(Status)) return Status;
    return NvGetNameFromNvar(
        pNvar, VariableName, VariableNameSize, VendorGuid, pInfo
    );
}

VOID NvResetInfoBuffer(IN NVRAM_STORE_INFO *pInfo)
{
    NVAR *pVar;
    pInfo->NvramGuidsAddress = (EFI_GUID*)(pInfo->NvramAddress+pInfo->NvramSize-sizeof(EFI_GUID));
    pInfo->pFirstVar = pInfo->NvramAddress+pInfo->HeaderLength;
    pInfo->NextGuid = 0;
    pInfo->LastVarSize = 0;
    pInfo->pLastReturned = NULL;
    
    pInfo->pEndOfVars = pInfo->NvramAddress+pInfo->NvramSize;
    pVar = NvGetFirstValid(pInfo);
    
    if ((NVAR*)pInfo->pFirstVar!=pVar)
    {
        if (pVar!=NULL)
        {
            pInfo->pFirstVar=(UINT8*)pVar;
        }
        
        else //if pVar==NULL
        {
            //there are no variables
            //set pEndOfVars equal to pFirstVar to indicate that variable store is empty
        	if (!NvIsVariable((NVAR*)pInfo->pFirstVar,pInfo))
        		pInfo->pEndOfVars = pInfo->pFirstVar;
        }
    }
}

/**
    This function initializes NVRAM_STORE_INFO structure. 

    @param pInfo pointer to NVRAM store structure
    @param HeaderSize Size of the header 
    @param Flags default Flags

    @retval VOID

**/
VOID NvInitInfoBuffer(
    IN NVRAM_STORE_INFO *pInfo, UINT32 HeaderSize, UINT8 Flags,
    NVRAM_STORE_INTERFACE *Interface
)
{
    pInfo->Flags = Flags;
    pInfo->HeaderLength = HeaderSize;
    pInfo->Interface = Interface;
    NvResetInfoBuffer(pInfo);
}

/**
    This function returns Default Variables info. 

    @param DefaultsVar name of the defaults Vars
    @param pInfo pointer to NVRAM store structure
    @param pOutInfo pointer to memory where defaults will be stored
        NULL - if not found

    @retval NVRAM_STORE_INFO*

**/
NVRAM_STORE_INFO* NvGetDefaultsInfo(IN const CHAR16* DefaultsVar, IN NVRAM_STORE_INFO *pInInfo, OUT NVRAM_STORE_INFO *pOutInfo)
{
    UINTN NameSize;
    NVAR* pNvar;
    pNvar = (NVAR*)NvFindVariable(
                (CHAR16*)DefaultsVar, (EFI_GUID*)&AmiDefaultsVariableGuid,
                &NameSize,pInInfo
            );
            
    if (!pNvar) return NULL;
    
    pOutInfo->NvramAddress = (UINT8*)NvGetData(
                                 pNvar,NameSize,
                                 &pOutInfo->NvramSize,pInInfo
                             );
                             
    if (pOutInfo->NvramAddress==NULL) return NULL;
    
    NvInitInfoBuffer(
        pOutInfo, 0,
        NVRAM_STORE_FLAG_NON_VALATILE
        | NVRAM_STORE_FLAG_READ_ONLY
        | NVRAM_STORE_FLAG_DO_NOT_ENUMERATE,
        NULL
    );

    return pOutInfo;
}

/**
    This function searches for Var with specific Name and GUID.

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var will be returned
    @param InfoCount Max NVRAM entry number 
    @param pInfo pointer to NVRAM store structure

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetVariable2(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data,
    UINT32 InfoCount, NVRAM_STORE_INFO *Info
)
{
    UINT32 i;

    if (!VariableName || !VendorGuid || !DataSize || !Data && *DataSize)
        return EFI_INVALID_PARAMETER;
    
    for (i=0; i<InfoCount; i++)
    {
        EFI_STATUS Status = NvGetVariable(
                                VariableName,VendorGuid,Attributes,DataSize,Data,&Info[i],NULL
                            );
                            
        if (Status!=EFI_NOT_FOUND) return Status;
    }
    
    return EFI_NOT_FOUND;
}

/**
    This function searches for Var with specific GUID and Entry number 
    not between LastInfoIndex - InfoCount parameter and returns it's Name. 

    @param VariableNameSize size of Varname - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param VariableName pointer where Var Name in Unicode will be stored
    @param VendorGuid pointer to memory where Var GUID will be stored
    @param InfoCount Max NVRAM entry number 
    @param pInfo pointer to NVRAM store structure
    @param LastInfoIndex NVRAM entry number to start from
    @param Runtime search for Var with NVRAM_FLAG_RUNTIME set

    @retval EFI_STATUS based on result

**/
EFI_STATUS NvGetNextVariableName2(
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid,
    UINT32 InfoCount, NVRAM_STORE_INFO *pInfo, UINT32 *LastInfoIndex,
    BOOLEAN Runtime
)
{
    UINT32 i;
    EFI_STATUS Status;
    NVAR* Nvar;
    CHAR16 OrgName; 

    if ( !VariableNameSize || !VariableName || !VendorGuid)
        return EFI_INVALID_PARAMETER;

    // When EFI_BUFFER_TOO_SMALL is returned, we may end up with the corrupted VariableName[0] character.
    // We are setting it to zero when search algorithm switches to a next varstore.
    // Preserve first VariableName character to restore it before returning.    
    OrgName = VariableName[0];

    if (LastInfoIndex){
      if (VariableName[0]==0) *LastInfoIndex=0;
      i=*LastInfoIndex;
    }else{
      i=0;
    }
    for ( ; i<InfoCount; i++)
    {
        Status=NvGetNextVariableNvar(VariableName,VendorGuid,&pInfo[i],Runtime,&Nvar);
        if (!EFI_ERROR(Status) && pInfo[i].Flags & NVRAM_STORE_FLAG_DO_NOT_ENUMERATE){
            while ( Nvar ){
                UINT32 j;
                for (j = 0; j < i; j++)
                    if (NvFindVariableByNvar(Nvar, &pInfo[i], &pInfo[j] )) 
                        break;
                if (j==i) break;
                Nvar=NvGetNextValid(Nvar,&pInfo[i]);
                if ( Runtime ){
                    while (Nvar && !(Nvar->flags & NVRAM_FLAG_RUNTIME))
                        Nvar=NvGetNextValid(Nvar,&pInfo[i]);
                }
            }
            if (!Nvar)
            {
                pInfo[i].pLastReturned = NULL;
                Status=EFI_NOT_FOUND;
            }
        }

        if (!EFI_ERROR(Status))
        {
            if (LastInfoIndex) *LastInfoIndex=i;
            Status = NvGetNameFromNvar(
                Nvar, VariableName, VariableNameSize, VendorGuid, &pInfo[i]
            );
            if (EFI_ERROR(Status)) {
                VariableName[0] = OrgName; //Restore first letter in case it was set to 0 before going to new Varstore
            }
            return Status;
        }
        
        //When variable with VariableName/VendorGuid
        //is not is the store EFI_INVALID_PARAMETER is returned
        //and we have to keep searching.
        //EFI_NOT_FOUND is returned when variable with VariableName/VendorGuid
        //is found but it's the last variable in the store.
        //In this case we have to return first variable from the next store
        //In order to do that we need to reset the name
        if (Status==EFI_NOT_FOUND)
        {
            VariableName[0]=0;
        }
        
        else
        {
            //If we are here the Status should be EFI_INVALID_PARAMETER,
            //which means the variable with VariableName/VendorGuid has not been found.
            //If we were using LastInfoIndex, the fact that variable has not been found
            //indicates that LastInfoIndex is invalid and we have to restart the search.
            //The condition below checks if LastInfoIndex has been used during this iteration.
            //Seting i to -1 in conjunction with i++ at the end of the iteration restarts
            //the search from 0.
            if (LastInfoIndex && i!=0 && i==*LastInfoIndex){
                i=(UINT32)-1;
                *LastInfoIndex = 0;
            }
        }
    }
    
    if (LastInfoIndex) *LastInfoIndex=0;
    
    return Status;
}

UINT32 GetFvHeaderSize(VOID *Address){
    EFI_FIRMWARE_VOLUME_HEADER *FwVolHeader;
    EFI_FIRMWARE_VOLUME_EXT_HEADER *FwVolExtHeader;
    
    FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *)Address;
    if (   FwVolHeader->Signature!=FV_SIGNATURE
        || FwVolHeader->HeaderLength != (UINT16)(sizeof(EFI_FIRMWARE_VOLUME_HEADER)+sizeof(EFI_FV_BLOCK_MAP_ENTRY))
    ) return 0;
    if (FwVolHeader->ExtHeaderOffset == 0){
        return FwVolHeader->HeaderLength;
    }else {
        UINT32 HeaderSize;
        UINT32 MaxOffset =  MAX_NVRAM_STORE_HEADER_SIZE - sizeof(EFI_FFS_FILE_HEADER) - sizeof(EFI_FIRMWARE_VOLUME_EXT_HEADER);
        UINT32 MaxSize;

        if (   FwVolHeader->ExtHeaderOffset < FwVolHeader->HeaderLength 
            || FwVolHeader->ExtHeaderOffset > MaxOffset
        ) return 0;//0x48 0x78
        MaxSize =   MAX_NVRAM_STORE_HEADER_SIZE 
                  - sizeof(EFI_FFS_FILE_HEADER) 
                  - sizeof(EFI_FIRMWARE_VOLUME_HEADER) - sizeof(EFI_FV_BLOCK_MAP_ENTRY);
        FwVolExtHeader = (EFI_FIRMWARE_VOLUME_EXT_HEADER *) ((UINT8*)FwVolHeader + FwVolHeader->ExtHeaderOffset);
        if (   FwVolExtHeader->ExtHeaderSize < sizeof(EFI_FIRMWARE_VOLUME_EXT_HEADER)
            || FwVolExtHeader->ExtHeaderSize > MaxSize
        ) return 0;
        HeaderSize = FwVolHeader->ExtHeaderOffset + FwVolExtHeader->ExtHeaderSize;
        return ALIGN_VALUE(HeaderSize,8);
    }
}

UINT32 GetNvStoreHeaderSize(VOID *Address){
    UINT32 FvHeaderSize = GetFvHeaderSize(Address);
    if (FvHeaderSize!=0) FvHeaderSize+=sizeof(EFI_FFS_FILE_HEADER);
    return FvHeaderSize;
}

/**
    This function searches for firmware volume header and returns pointer to its State field

    @param Info pointer to NVRAM store structure

    @retval  pointer to FFS header State field

**/
EFI_FFS_FILE_STATE* GetNvramFfsFileStatePtr(VOID *Address)
{
    EFI_FFS_FILE_HEADER *FfsFileHeader;
    UINT32 HeaderSize;
    
    HeaderSize = GetFvHeaderSize(Address);
    if (HeaderSize == 0) return NULL;
    FfsFileHeader = (EFI_FFS_FILE_HEADER *)((UINT8*)Address + HeaderSize);
    return &FfsFileHeader->State;
}

/**
    This function searches for the header of the NVRAM image FFS file and returns the value of its State field

    @param Info pointer to NVRAM store structure

    @retval  FFS header State 

**/
EFI_FFS_FILE_STATE GetNvramFfsFileState(VOID *Address)
{
    EFI_FFS_FILE_STATE State;
    EFI_FFS_FILE_STATE *StatePtr = GetNvramFfsFileStatePtr(Address);
    if (StatePtr==NULL) return EFI_FILE_HEADER_INVALID;
    State = (FlashEmpty==0) ? *StatePtr : ~*StatePtr;
    if (State==0) State = EFI_FILE_HEADER_INVALID;
    return State;
}

/**
    This function verifies if main NVRAM storage is valid

               
    @param MainInfo pointer to Main NVRAM storage descriptor
    @param BackUpAddress address of the back up storage

        BOOLEAN
    @retval TRUE the main NVRAM storage is valid
    @retval FALSE the main NVRAM storage is invalid. Back up storage should be used.

**/
BOOLEAN IsMainNvramStoreValid(
    VOID *MainAddress, VOID *BackupAddress, 
    BOOLEAN *BackupStoreValidPtr
){
    EFI_FFS_FILE_STATE MainState;
    EFI_FFS_FILE_STATE BackupState;
    BOOLEAN MainStoreValid;
    BOOLEAN BackupStoreValid;

    MainState = GetNvramFfsFileState(MainAddress);
    BackupState = (BackupAddress!=NULL) ? GetNvramFfsFileState(BackupAddress) : EFI_FILE_HEADER_INVALID;
// State Transitions:
//    Main Store State                          Backup Store State                        Valid Store
// = Initial State After Firmware Flashing
// 0. DATA_VALID                                HEADER_INVALID                            Main
// = Initial State After the first boot (InitHybridInterface sets MARKED_FOR_UPDATE bit)
// 1. DATA_VALID|MARKED_FOR_UPDATE              HEADER_INVALID                            Main
// = Update  Cycle 1
// 2. DATA_VALID|MARKED_FOR_UPDATE              HEADER_VALID                              Main
// 3. DATA_VALID|MARKED_FOR_UPDATE|DELETED      HEADER_VALID                              Main
// 4. DATA_VALID|MARKED_FOR_UPDATE|DELETED      DATA_VALID|MARKED_FOR_UPDATE              Backup
// = Update  Cycle 1 is Over
// = Update  Cycle 2
// 5. HEADER_VALID                              DATA_VALID|MARKED_FOR_UPDATE              Backup
// 6. HEADER_VALID                              DATA_VALID|MARKED_FOR_UPDATE|DELETED      Backup
// 7. DATA_VALID|MARKED_FOR_UPDATE              DATA_VALID|MARKED_FOR_UPDATE|DELETED      Main
// = Update  Cycle 2 is Over
// = Update  Cycle 3
// 8. DATA_VALID|MARKED_FOR_UPDATE              HEADER_VALID                              Main
// State 8 == State 2
    MainStoreValid = (MainState & EFI_FILE_DATA_VALID)!=0 && (MainState & ~VALID_STATE_FLAGS)==0;
    BackupStoreValid = (BackupState & EFI_FILE_DATA_VALID)!=0 && ((BackupState & ~VALID_STATE_FLAGS)==0);
    if (BackupStoreValidPtr) *BackupStoreValidPtr = BackupStoreValid;
    if (MainStoreValid && BackupStoreValid){
    	if ((BackupState & EFI_FILE_DELETED)!=0) return TRUE;
    	if ((MainState & EFI_FILE_DELETED)!=0) return FALSE;
    	if ((MainState & EFI_FILE_MARKED_FOR_UPDATE)!=0){
    		// If we are here we are dealing either with the partially updated NVRAM (only one area has been updated)
    		// or with the NVRAM image based on old format.
    		// The latter can happen if firmware was updated without updating NVRAM.
    		// Older NVRAM images didn't use EFI_FILE_DELETED bit. 
    		// EFI_FILE_MARKED_FOR_UPDATE bit was used instead to indicate beginning of the update operation.
    		if ((BackupState & EFI_FILE_MARKED_FOR_UPDATE)==0) return FALSE;
    		else return TRUE;
    	}
    }
    return MainStoreValid;
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
