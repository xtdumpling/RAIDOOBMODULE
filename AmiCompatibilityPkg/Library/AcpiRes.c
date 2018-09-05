//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
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
// $Header: /Alaska/SOURCE/Core/Library/AcpiRes.c 31    11/15/11 11:59a Felixp $
//
// $Revision: 31 $
//
// $Date: 11/15/11 11:59a $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: AcpiRes.c
//
// Description:
//  Implementation of ASL MACROs for ResourceTemplate.  See ACPI 2.0
// specification section 16.2.4.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiDxeLib.h>
#include <AcpiRes.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiSupport.h>

// ===========================
// In Aptio4, these are externals defined in the AmiProtocolLib.
// In AptioV, there is no AmiProtocolLib.
// We could've listed these in AmiDxeLib.inf, but may break the
// build of some of the Aptio4 modules due to multiple defined symbols.
static EFI_GUID gLocalEfiAcpiSdtProtocolGuid = EFI_ACPI_SDT_PROTOCOL_GUID;

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_IRQ
//
// Description:
//  VOID* ASLM_IRQ(IN UINT8 dsc_cnt, IN UINT8 ll, IN UINT8 he,
// IN UINT8 shr, IN ...) generates a short IRQ descriptor and returns its
// address.  User is responsible for freeing the returned memory.
//
// Input:
//  IN UINT8 dsc_cnt
// Number of IRQ numbers in the list.
//
//  IN UINT8 ll
// IRQ flag.  0 = active-high, 1 = active-low.
//
//  IN UINT8 he
// IRQ flag.  0 = level-triggered, 1 = edge-triggered.
//
//  IN UINT8 shr
// IRQ flag.  0 = not interrupt shareable, 1 = interrupt shareable.
//
//  IN ...
// List of IRQ numbers.  Valid values: 0-15.
//
// Output:
//  VOID* address of the created IRQ descriptor.  User is responsible for
// freeing the returned memory.
//
// Modified:
//
// Referrals:
//  Malloc
//  VA_START
//  VA_ARG
//  VA_END
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_IRQ(UINT8 dsc_cnt, UINT8 ll, UINT8 he, UINT8 shr, ... )
{
    UINT8           i;
    UINT16          irq=0;
    VA_LIST         marker;
    UINT32          sz=sizeof(ASLR_IRQ);
    ASLR_IRQ        *rb=(ASLR_IRQ*)Malloc(sz);
//------------------------------

    if (!rb)return NULL;
    
    pBS->SetMem(rb,sz,0);
    
    //Update descriptor Header
    rb->Hdr.Length=3;//sizeof(ASLR_IRQ)-sizeof(ASLRF_S_HDR);
    rb->Hdr.Name=ASLV_RT_IRQ;
    rb->Hdr.Type=ASLV_SMALL_RES;
    
    //Update descriptor Body
    rb->Flags._LL=ll;
    rb->Flags._HE=he;
    rb->Flags._SHR=shr;
    
    VA_START(marker, shr);
    
    for (i=0; i<dsc_cnt; i++)
    {
        irq|=(1<<VA_ARG(marker, UINT8));
    }
    
    VA_END(marker);
    rb->_INT=irq;
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_IRQNoFlags
//
// Description:
//  VOID* ASLM_IRQNoFlags(IN UINT8 dsc_cnt, IN ...) generates a short IRQ
// descriptor without an optional IRQ information byte and returns its
// address.  User is responsible for freeing the returned memory.
//
// Input:
//  IN UINT8 dsc_cnt
// Number of IRQ numbers in the list.
//
//  IN ...
// List of IRQ numbers.  Valid values: 0-15.
//
// Output:
//  VOID* address of the created IRQ descriptor.  User is responsible for
// freeing the returned memory.
//
// Modified:
//
// Referrals:
//  Malloc
//  VA_START
//  VA_ARG
//  VA_END
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_IRQNoFlags(UINT8 dsc_cnt, ... )
{
    UINT8           i;
    VA_LIST         marker;
    UINT16          irq=0;
    UINT32          sz=sizeof(ASLR_IRQNoFlags);
    ASLR_IRQNoFlags *rb=(ASLR_IRQNoFlags*)Malloc(sz);
//------------------------------

    if (!rb)return NULL;
    
    pBS->SetMem(rb,sz,0);
    
    //Update descriptor Header
    rb->Hdr.Length=2;//sizeof(ASLR_IRQNoFlags)-sizeof(ASLRF_S_HDR);
    rb->Hdr.Name=ASLV_RT_IRQ;
    rb->Hdr.Type=ASLV_SMALL_RES;
    
    VA_START(marker, dsc_cnt);
    
    for (i=0; i<dsc_cnt; i++)
    {
        irq|=(1<<VA_ARG(marker, UINT8));
    }
    
    VA_END(marker);
    rb->_INT=irq;
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_DMA
//
// Description:
//  VOID* ASLM_DMA(IN UINT8 dsc_cnt, IN UINT8 typ, IN UINT8 bm, IN UINT8 siz,
// IN ...) generates a short DMA descriptor and returns its address.  User is
// responsible for freeing the returned memory.
//
// Input:
//  IN UINT8 dsc_cnt
// Number of channel numbers in the list.
//
//  IN UINT8 typ
// DMA channel speed.  Valid values are:
//  ASLV_Compatibility, ASLV_TypeA, ASLV_TypeB, ASLV_TypeF
//
//  IN UINT8 bm
// BusMaster or NotBusMaster.  0 defaults to BusMaster.
//
//  IN UINT8 siz
// Transfer size.  Valid values are:
//  Transfer8, Transfer16, Transfer8_16
//
//  IN ...
// List of channel numbers.  Valid values are 0-7.
//
// Output:
//  VOID* address of the created DMA descriptor.  User is responsible for
// freeing the returned memory.
//
// Modified:
//
// Referrals:
//  Malloc
//  VA_START
//  VA_ARG
//  VA_END
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_DMA(UINT8 dsc_cnt, UINT8 typ, UINT8 bm, UINT8 siz, ... )
{
    UINT8           i, dma=0;
    VA_LIST         marker;
    UINT32          sz=sizeof(ASLR_DMA);
    ASLR_DMA        *rb=(ASLR_DMA*)Malloc(sz);
//------------------------------

    if (!rb)return NULL;
    
    pBS->SetMem(rb,sz,0);
    
    //Update descriptor Header
    rb->Hdr.Length=sizeof(ASLR_DMA)-sizeof(ASLRF_S_HDR);
    rb->Hdr.Name=ASLV_RT_DMA;
    rb->Hdr.Type=ASLV_SMALL_RES;
    
    //Update descriptor Body
    rb->Flags._TYP=typ;
    rb->Flags._BM=bm;
    rb->Flags._SIZ=siz;
    
    VA_START(marker, siz);
    
    for (i=0; i<dsc_cnt; i++)
    {
        dma|=(1<<VA_ARG(marker, UINT8));
    }
    
    VA_END(marker);
    rb->_DMA=dma;
    
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_StartDependentFn
//
// Description:
//  VOID* ASLM_StartDependentFn(IN UINT8 dsc_cnt, IN UINT8 pComp,
// IN UINT8 pPerf, IN ...) generates a Start-Dependent Function descriptor
// with the optional priority byte and returns its address.  User is
// responsible for freeing the returned memory.
//
// Input:
//  IN UINT8 dsc_cnt
// Number of items in the descriptor list.
//
//  IN UINT8 pComp
// Compatibility priority.  Valid values are 0-2.
//
//  IN UINT8 pPerf
// Performance/robustness priority.  Valid values are 0-2.
//
//  IN ...
// List of descriptors for this dependent function.
//
// Output:
//  VOID* address of the created Start-Dependent Function descriptor.  User
// is responsible for freeing the returned memory.
//
// Modified:
//
// Referrals:
//  MallocZ
//  Malloc
//  VA_START
//  VA_ARG
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_StartDependentFn(UINT8 dsc_cnt,UINT8 pComp,UINT8 pPerf, ...)
{
    UINT8           i;
    VA_LIST         marker;
    UINT32          sz=sizeof(EFI_ASL_DepFn)+sizeof(ASLR_StartDependentFn);
    EFI_ASL_DepFn   *rb=(EFI_ASL_DepFn*)MallocZ(sz);
    
//--------------------------------------------
    if (!rb)return NULL;
    
    rb->DepFn=(rb+1);
    ((ASLR_StartDependentFn*)rb->DepFn)->Hdr.Length=sizeof(ASLR_StartDependentFn)-sizeof(ASLRF_S_HDR);
    ((ASLR_StartDependentFn*)rb->DepFn)->Hdr.Name=ASLV_RT_StartDependentFn;
    ((ASLR_StartDependentFn*)rb->DepFn)->Hdr.Type=ASLV_SMALL_RES;
    ((ASLR_StartDependentFn*)rb->DepFn)->_PRI._CMP=pComp;
    ((ASLR_StartDependentFn*)rb->DepFn)->_PRI._PRF=pPerf;
    
    rb->DepRes.Items=Malloc(sizeof(ASLR_HDR*)*dsc_cnt);
    
    if (!rb->DepRes.Items) return NULL;
    
    rb->DepRes.InitialCount=dsc_cnt;
    rb->DepRes.ItemCount=dsc_cnt;
    VA_START(marker, pPerf);
    
    for (i=0; i<dsc_cnt; i++) rb->DepRes.Items[i]=VA_ARG(marker, VOID*);
    
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_StartDependentFnNoPri
//
// Description:
//  VOID* ASLM_StartDependentFnNoPri(IN UINT8 dsc_cnt, IN ...) generates a
// Start-Dependent Function descriptor without the optional priority byte and
// returns its address.  User is responsible for freeing the returned memory.
//
// Input:
//  IN UINT8 dsc_cnt
// Number of items in the descriptor list.
//
//  IN ...
// List of descriptors for this dependent function.
//
// Output:
//  VOID* address of the created Start-Dependent Function descriptor.  User
// is responsible for freeing the returned memory.
//
// Modified:
//
// Referrals:
//  MallocZ
//  Malloc
//  VA_START
//  VA_ARG
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_StartDependentFnNoPri(UINT8 dsc_cnt, ...)
{
    UINT8               i;
    VA_LIST             marker;
    UINT32              sz=sizeof(EFI_ASL_DepFn)+sizeof(ASLR_StartDependentFnNoPri);
    EFI_ASL_DepFn       *rb=(EFI_ASL_DepFn*)MallocZ(sz);
    
//--------------------------------------------
    if (!rb)return NULL;
    
    rb->DepFn=(rb+1);
    ((ASLR_StartDependentFnNoPri*)rb->DepFn)->Length=sizeof(ASLR_StartDependentFnNoPri)-sizeof(ASLRF_S_HDR);
    ((ASLR_StartDependentFnNoPri*)rb->DepFn)->Name=ASLV_RT_StartDependentFn;
    ((ASLR_StartDependentFnNoPri*)rb->DepFn)->Type=ASLV_SMALL_RES;
    
    rb->DepRes.Items=Malloc(sizeof(ASLR_HDR*)*dsc_cnt);
    
    if (!rb->DepRes.Items) return NULL;
    
    rb->DepRes.InitialCount=dsc_cnt;
    rb->DepRes.ItemCount=dsc_cnt;
    
    VA_START(marker, dsc_cnt);
    
    for (i=0; i<dsc_cnt; i++) rb->DepRes.Items[i]=VA_ARG(marker, VOID*);
    
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_IO
//
// Description:
//  VOID* ASLM_IO(IN UINT8 dec, IN UINT16 min, IN UINT16 max, IN UINT8 aln,
// IN UINT8 len) generates a short I/O descriptor and returns its address.
// User is responsible for freeing the returned memory.
//
// Input:
//  IN UINT8 dec
// Decode value.  Valid values are: ASLV_Decode16, ASLV_Decode10.
//
//  IN UINT16 min
// Address minimum.
//
//  IN UINT16 max
// Address maximum.
//
//  IN UINT8 aln
// Base alignment.
//
//  IN UINT8 len
// Range length.
//
// Output:
//  VOID* address of the created I/O descriptor.  User is responsible for
// freeing the returned memory.
//
// Modified:
//
// Referrals:
//  MallocZ
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_IO(UINT8 dec,UINT16 min,UINT16 max,UINT8 aln,UINT8 len)
{
    ASLR_IO         *rb=(ASLR_IO*)MallocZ(sizeof(ASLR_IO));
    
//------------------------------------------
    if (!rb)return NULL;
    
    rb->Hdr.Length=sizeof(ASLR_IO)-sizeof(ASLRF_S_HDR);
    rb->Hdr.Name=ASLV_RT_IO;
    rb->Hdr.Type=ASLV_SMALL_RES;
    
    rb->_DEC=dec;
    rb->_MIN=min;
    rb->_MAX=max;
    rb->_ALN=aln;
    rb->_LEN=len;
    
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_FixedIO
//
// Description:
//  VOID* ASLM_FixedIO(IN UINT16 bas, IN UINT8 len) generates a short fixed
// I/O descriptor and returns its address.  User is responible for freeing
// the returned memory.
//
// Input:
//  IN UINT16 bas
// Address base.
//
//  IN UINT8 len
// Range length.
//
// Output:
//  VOID* address of the created fixed I/O descriptor.  User is responsible
// for freeing the returned memory.
//
// Modified:
//
// Referrals:
//  MallocZ
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_FixedIO(UINT16 bas, UINT8 len)
{
    ASLR_FixedIO    *rb=(ASLR_FixedIO*)MallocZ(sizeof(ASLR_FixedIO));
    
//------------------------------------------
    if (!rb)return NULL;
    
    rb->Hdr.Length=sizeof(ASLR_FixedIO)-sizeof(ASLRF_S_HDR);
    rb->Hdr.Name=ASLV_RT_FixedIO;
    rb->Hdr.Type=ASLV_SMALL_RES;
    
    rb->_BAS=bas;
    rb->_LEN=len;
    
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_VendorShort
//
// Description:
//  VOID* ASLM_VendorShort(IN UINT8 byte_cnt, IN ...) generates a short
// Vendor-Defined descriptor and returns its address.  User is responsible
// for freeing the returned memory.
//
// Input:
//  IN UINT8 byte_cnt
// Number of bytes in the list.  Must be less than 8.
//
//  IN ...
// List of byte expressions for the descriptor.  Must be less than 8
// elements.
//
// Output:
//  VOID* address of the created Vendor-Defined descriptor.  User is
// responsible for freeing the returned memory.
//
// Modified:
//
// Referrals:
//  MallocZ
//  VA_START
//  VA_ARG
//  VA_END
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_VendorShort(UINT8 byte_cnt, ...)
{
    UINT8               i;
    VA_LIST             marker;
    UINT32              sz= sizeof(ASLR_VendorShort)+byte_cnt;
    ASLR_VendorShort    *rb=(ASLR_VendorShort*)MallocZ(sz);
    UINT8               *bb=(UINT8*)rb+1;
//------------------------------

    if (!rb)return NULL;
    
    rb->Length=byte_cnt;
    rb->Name=ASLV_RT_VendorShort;
    rb->Type=ASLV_SMALL_RES;
    
    VA_START(marker, byte_cnt);
    
    //Update descriptor Body
    for (i=0; i<byte_cnt; i++)bb[i]=VA_ARG(marker, UINT8);
    
    VA_END(marker);
    
    return rb;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ASLM_Memory24
//
// Description:
//  VOID* ASLM_Memory24(IN UINT8 rw, IN UINT8 min, IN UINT8 max,
// IN UINT8 aln, IN UINT8 len) generates a long 24-bit memory descriptor and
// returns its address.  User is responsible for freeing the returned memory.
//
// Input:
//  IN UINT8 rw
// Write protection flag.  Valid values: ASLV_ReadOnly, ASLV_ReadWrite.
//
//  IN UINT8 min
// Minimum base memory address [23:8].
//
//  IN UINT8 max
// Maximum base memory address [23:8].
//
//  IN UINT8 aln
// Base alignment.
//
//  IN UINT8 len
// Range length.
//
// Output:
//  VOID* address of the created long 24-bit memory descriptor.  User is
// responsible for freeing the returned memory.
//
// Modified:
//
// Referrals:
//  MallocZ
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID *ASLM_Memory24(UINT8 rw,UINT8 min, UINT8 max, UINT8 aln, UINT8 len)
{
    ASLR_Memory24   *rb=(ASLR_Memory24*)MallocZ(sizeof(ASLR_Memory24));
    
//------------------------------------------
    if (!rb)return NULL;
    
    rb->Hdr.Length=sizeof(ASLR_Memory24)-sizeof(ASLRF_S_HDR);
    rb->Hdr.Name=ASLV_RT_Memory24;
    rb->Hdr.Type=ASLV_SMALL_RES;
    
    rb->_RW=rw;
    rb->_MIN=min;
    rb->_MAX=max;
    rb->_ALN=aln;
    rb->_LEN=len;
    
    return rb;
    
}

//----------------------------------------------------------------------
// Methods Implementation for T_ITEM_LIST object
//----------------------------------------------------------------------

#define LST_INIT_COUNT  10 //initial value

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: reallocItemLst
//
// Description:
//  EFI_STATUS reallocItemLst(IN T_ITEM_LIST *Lst) is an internal worker
// function which reallocates space for a T_ITEM_LIST.Items[] array.
//
// Input:
//  IN T_ITEM_LIST *Lst
// List which needs its Items array reallocated.
//
// Output:
//  EFI_OUT_OF_RESOURCES,  if allocation failed.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  MallocZ
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS reallocItemLst(T_ITEM_LIST *Lst)
{
    UINTN       sz=0;
    VOID        *nrl=NULL; //new resource list pointer;
//---------------------------------------------

    // if Initial count has not been initialized;
    if (Lst->InitialCount==0) sz=LST_INIT_COUNT*sizeof(VOID*);
    else
    {
        //or Initial Count was initialized but memory fot Items[] array was not allocated
        if (Lst->InitialCount && !Lst->Items) sz=Lst->InitialCount*sizeof(VOID*);
        else
        {
            //or if we at the end or near the end of the Items[] array list
            if ((INTN)Lst->ItemCount > (INTN)(Lst->InitialCount-1))
                sz=(Lst->InitialCount+LST_INIT_COUNT)*sizeof(VOID*);
        }
    }
    
    if (sz)
    {
        nrl=MallocZ(sz);
        
        if (!nrl) return EFI_OUT_OF_RESOURCES;
        
        pBS->CopyMem((VOID*)nrl,(VOID*)Lst->Items,sizeof(VOID*)*Lst->ItemCount);
        //Free Buffer of previouse ChildList...
        pBS->FreePool((VOID*)Lst->Items);
        
        //Make it point to a new ChildList
        Lst->Items=nrl;
        Lst->InitialCount=sz/sizeof(VOID*);
    }
    
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: InsertItemLst
//
// Description:
//  EFI_STATUS InsertItemLst(IN T_ITEM_LIST *Lst, IN VOID *pRes,
// IN UINTN ItemIndex) inserts the provided item pointed to by pRes into the
// provided Lst at index ItemIndex.
//
// Input:
//  IN T_ITEM_LIST *Lst
// List to be inserted into.
//
//  IN VOID *pRes
// Pointer to object to place in list.
//
//  IN UINTN ItemIndex
// Index in list to place the object pointed to by pRes.
//
// Output:
//  EFI_INVALID_PARAMETER,  if the ItemIndex value is invalid.
//  EFI_OUT_OF_RESOURCES,   if there is not enough memory for the new object.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  reallocItemLst
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS InsertItemLst(T_ITEM_LIST *Lst, VOID* pRes, UINTN ItemIndex)
{
    INTN        i;
    
//----------------
    if (!Lst->ItemCount && Lst->ItemCount<ItemIndex) return EFI_INVALID_PARAMETER;
    
    //Check if Items[] array can accomodate a new child...
    if (EFI_ERROR(reallocItemLst(Lst)))return EFI_OUT_OF_RESOURCES;
    
    //Shift items after Index forward
    for (i=(INTN)Lst->ItemCount-1; i>=(INTN)ItemIndex; i--)  Lst->Items[i+1]=Lst->Items[i];
    
    //fill Item address we are inserting
    Lst->Items[ItemIndex]=pRes;
    //Adjust Item Count
    Lst->ItemCount++;
    return EFI_SUCCESS;
}


//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DeleteItemLst
//
// Description:
//  EFI_STATUS DeleteItemLst(IN T_ITEM_LIST *Lst, IN UINTN ItemIndex,
// IN BOOLEAN FreeData) deletes the object stored at the provided ItemIndex
// in Lst.  Frees the memory associated with the object pointer if FreeData
// is TRUE.
//
// Input:
//  IN T_ITEM_LIST *Lst
// List to be deleted from.
//
//  IN UINTN ItemIndex
// Index inside Lst->Items to be deleted.
//
//  IN BOOLEAN FreeData
// TRUE will free the memory associated with the deleted object.
//
// Output:
//  EFI_INVALID_PARAMETER,  if the provided ItemIndex is invalid.
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
EFI_STATUS DeleteItemLst(T_ITEM_LIST *Lst, UINTN ItemIndex, BOOLEAN FreeData)
{
    UINTN       i;
    
//----------------
    if (!Lst->ItemCount || Lst->ItemCount<=ItemIndex) return EFI_INVALID_PARAMETER;
    
    //Free Data associated with Iterm if requested;
    if (FreeData)pBS->FreePool(Lst->Items[ItemIndex]);
    
    //Shift items after ItemIndex backwards
    for (i=ItemIndex; i<Lst->ItemCount-1; i++) Lst->Items[i]=Lst->Items[i+1];
    
    //Adjust Item Count
    Lst->ItemCount--;
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AppendItemLst
//
// Description:
//  EFI_STATUS AppendItemLst(IN T_ITEM_LIST *Lst, IN VOID *pRes) appends the
// provided object to Lst.
//
// Input:
//  IN T_ITEM_LIST *Lst
// List to be appended to.
//
//  IN VOID *pRes
// Object to be appended.
//
// Output:
//  EFI_OUT_OF_RESOURCES,  if memory allocation fails.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  reallocItemLst
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS AppendItemLst(T_ITEM_LIST *Lst, VOID* pRes )
{
    //Check if Items[] array can accomodate a new child...
    if (EFI_ERROR(reallocItemLst(Lst)))return EFI_OUT_OF_RESOURCES;
    
    Lst->Items[Lst->ItemCount]=pRes;
    Lst->ItemCount++;
    
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ClearItemLst
//
// Description:
//  VOID ClearItemLst(IN T_ITEM_LIST *Lst, IN BOOLEAN FreeData) clears all
// the items of Lst.  If FreeData is TRUE, frees all the memory associated
// with the stored objects.
//
// Input:
//  IN T_ITEM_LIST *Lst
// List to be cleared.
//
//  IN BOOLEAN FreeData
// If TRUE, all objects are freed.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
VOID ClearItemLst(T_ITEM_LIST *Lst, BOOLEAN FreeData)
{
    UINTN       i;
    
//-----------------------------------------
    if (FreeData)for (i=0; i<Lst->ItemCount; i++)
        {
            if (Lst->Items[i])pBS->FreePool(Lst->Items[i]);
        }
        
    if (Lst->Items) pBS->FreePool(Lst->Items);
    
    Lst->Items=NULL;
    Lst->ItemCount=0;
    Lst->InitialCount=0;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CopyItemLst
//
// Description:
//  EFI_STATUS CopyItemLst(IN T_ITEM_LIST *Lst, OUT T_ITEM_LIST **NewLstPtr)
// copies Lst into a new list and provides the address of the copy in
// NewLstPtr.
//
// Input:
//  IN T_ITEM_LIST *Lst
// List to be copied.
//
//  OUT T_ITEM_LIST **NewLstPtr
// Location to store the address of the new copy.
//
// Output:
//  EFI_OUT_OF_RESOURCES,  if memory allocation fails.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  MallocZ
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS CopyItemLst(T_ITEM_LIST *Lst, T_ITEM_LIST **NewLstPtr)
{
    UINTN i = 0;
    T_ITEM_LIST *NewLst = NULL;
    
    *NewLstPtr = NULL;
    *NewLstPtr = MallocZ(sizeof(T_ITEM_LIST));
    
    if (!*NewLstPtr)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    
    NewLst = *NewLstPtr;
    NewLst->InitialCount = Lst->InitialCount;
    NewLst->ItemCount = Lst->ItemCount;
    
    if (Lst->ItemCount == 0)
    {
        return EFI_SUCCESS;
    }
    
    NewLst->Items = MallocZ( Lst->ItemCount * sizeof(VOID*) );
    
    if (!NewLst->Items)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    
    for (i = 0; i < Lst->ItemCount; i++)
    {
        NewLst->Items[i] = NULL;
        
        if (Lst->Items[i])
        {
            NewLst->Items[i] = MallocZ( sizeof(Lst->Items[i]) );
            
            if (!NewLst->Items[i])
            {
                return EFI_OUT_OF_RESOURCES;
            }
            
            pBS->CopyMem(NewLst->Items[i], Lst->Items[i], sizeof(Lst->Items[i]));
        }
    }
    
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ValidateDescriptor
//
// Description:
//  BOOLEAN ValidateDescriptor(IN ASLR_QWORD_ASD *Descriptor,
// IN ASLR_TYPE_ENUM ResType, IN BOOLEAN Fixed) validates a ASLV_LARGE_RES
// descriptor.
//
// Input:
//  IN ASLR_QWORD_ASD *Descriptor
// Descriptor to be validated.
//
//  IN ASLR_TYPE_ENUM ResType
// The resource type of the descriptor.  Valid values are:
//  tResAll, tResIoMem, tResBus
//
//  IN BOOLEAN Fixed
// If TRUE, checks the general flags for consistancy.
//
// Output:
//  TRUE if a valid ASLR_QWORD_ASD descriptor.  Otherwise, FALSE.
//
// Modified:
//
// Referrals:
//
// Notes:
//  Helper function for ValidateDescriptorBlock.
//
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN ValidateDescriptor(ASLR_QWORD_ASD *Descriptor,ASLR_TYPE_ENUM ResType,BOOLEAN Fixed)
{
    //Check ASL Resource Descriptor Header consistancy
    if (
        (Descriptor->Hdr.Name!=ASLV_RT_QWORD_ASD)   ||
        (Descriptor->Hdr.Type!=ASLV_LARGE_RES)      ||
        (Descriptor->Hdr.Length!=sizeof(ASLR_QWORD_ASD)-sizeof(ASLRF_L_HDR) )
    )return FALSE;
    
    //Check ASL Resource Descriptor General Flags Consistancy
    if (Fixed)
    {
        if (
            (!Descriptor->GFlags._MIF) ||
            (!Descriptor->GFlags._MAF)
        )return FALSE;
    }
    
    if (Descriptor->Type>2) return FALSE;
    
    //Check ASL Resource Descriptor Resource Type
    switch (ResType)
    {
        case tResBus:
        
            if (Descriptor->Type!=ASLRV_SPC_TYPE_BUS) return FALSE;
            
            if ((Descriptor->_MIN+Descriptor->_LEN)>0x100)return FALSE;
            
            if ((Descriptor->_MAX - Descriptor->_MIN +1)!= Descriptor->_LEN ) return FALSE;

            break;
        case tResIoMem:
        
            if (Descriptor->Type>=ASLRV_SPC_TYPE_BUS) return FALSE;
            
            break;
        case tResAll:
        
            if (Descriptor->Type>ASLRV_SPC_TYPE_BUS) return FALSE;
            
            break;
        default : return FALSE;
    }
    
    return TRUE;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ValidateDescriptorBlock
//
// Description:
//  UINTN ValidateDescriptorBlock(IN ASLR_QWORD_ASD *DscStart,
// IN ASLR_TYPE_ENUM ResType, IN BOOLEAN Fixed) validates the consistancy of
// the provided ACPI QWORD resource descriptors block which contains some
// number of ASLR_QWORD_ASDs in memory terminated by an ASLR_EndTag.  Returns
// the total size of the resource descriptors plus the size of the
// ASLR_EndTag.
//
// Input:
//  IN ASLR_QWORD_ASD *DscStart
// Pointer to the first descriptor in the block to be validated.
//
//  IN ASLR_TYPE_ENUM ResType
// The resource type of the descriptors.  Valid values are:
//  tResAll, tResIoMem, tResBus
//
//  IN BOOLEAN Fixed
// If TRUE, checks the general flags for consistancy.
//
// Output:
//  UINTN size of the resource descriptor block plus the size of the
// ASLR_EndTag.
//
// Modified:
//
// Referrals:
//  ValidateDescriptor
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
UINTN ValidateDescriptorBlock(ASLR_QWORD_ASD* DscStart, ASLR_TYPE_ENUM ResType, BOOLEAN Fixed)
{
    ASLR_QWORD_ASD  *qwd=DscStart;
    UINTN           len=0;
    ASLR_EndTag     *endtag=(ASLR_EndTag*)DscStart;
    
//-----------------------------------
    //resource descriptor is empty...
    if (endtag->Hdr.HDR==ASLV_END_TAG_HDR) return sizeof(ASLR_EndTag);
    
    while (endtag->Hdr.HDR!=ASLV_END_TAG_HDR)
    {
        if (!ValidateDescriptor(qwd, ResType, Fixed)) return 0;
        else len+=sizeof(ASLR_QWORD_ASD);
        
        qwd++;
        endtag=(ASLR_EndTag*)qwd;
    }
    
    len+=sizeof(ASLR_EndTag);
    return len;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CheckDsdt
//
// Description:
//  BOOLEAN CheckDsdt(IN ACPI_HDR *Dsdt) checks the signature of the provided
// ACPI header for the DSDT signature.
//
// Input:
//  IN ACPI_HDR *Dsdt
// Pointer to the ACPI header to be checked for the DSDT signature.
//
// Output:
//  TRUE if the header signature is that of the DSDT.  Otherwise, FALSE.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN CheckDsdt(ACPI_HDR *Dsdt)
{
    if (Dsdt->Signature!=DSDT_SIG) return FALSE;
    
    return TRUE;
}


// <AMI_PHDR_START>
//----------------------------------------------------------------------------
// 
// Name: GetRbAmlPackagePointer
//
// Description:
//      This function returns a pointer to a named AML Package object in the
//      specified root bridge scope along with information necessary to 
//      dynamically read and/or modify its data.
//
//      This function also serves as an example of how a named Package object
//      pointer can be obtained, if the scope path is known.
//
// Input:  
//      IN ACPI_HDR **Dsdt
//                  - This pointer is either NULL or the pointer to the DSDT.
//                    (If it is NULL, LibGetDsdt is called to obtain the
//                    pointer.)
//      IN CHAR8 *RbName
//                  - This string specifies the name of the root bridge
//                    device for which the Package object is to be found.
//      IN CHAR8 *PackageName
//                  - This string specifies the name of the Package object
//                    to be found.
//      OUT ASL_OBJ_INFO *Package
//                  - This parameter is updated to contain the address of
//                    the ASL_OBJ_INFO structure associated with the 
//                    located Package object.
//      OUT PACKAGE_ELEMENT *Elements
//                  - This parameter is updated to contain the address
//                    of the first element of the located Package object.
//      OUT UINT8 *ElementCount 
//                  - This parameter is updated to contain the number
//                    of elements in the located Package object.
//
// Output:
//      EFI_STATUS (Return Value)
//                  = EFI_SUCCESS if successful
//                  = or valid EFI error code
//
// Modified:    
//      None
//
// Referrals:     
//      LibGetDsdt()
//      GetAslObj()
//
// Notes:
//      Package objects are aggregate arrays of elements that can be of 
//      different size and type.  This function assumes the caller 
//      understands the structure of the package object associated with the
//      PackageName paramenter and can iterate through the elements. 
//
//      Prototypes for several element types are provided in AcpiRes.h 
//      (see PACKAGE_ELEMENT).  Additional types can be supported by 
//      typecasting PACKAGE_ELEMENT.Ptr.
//
//      Example: Supported ASL Package Declarations (must be initialized)
//
//                  Scope(\_SB.PCI0) {
//                    Name (PAK1, Package ()
//                      {
//                          0x0123,
//                          0x0123,
//                          0x0123,
//                          0x0123,
//                      })
//                  }
//
//                  Scope(\_SB.PCI1) {
//                    Name (PAK1, Package ()
//                      {
//                          0x0123,
//                          0x0123,
//                          0x0123,
//                          0x0123,
//                      })
//                  }
//
//      Example: Usage
//
//                  VOID Function(){
//                      ASL_OBJ_INFO      Package;
//                      PACKAGE_ELEMENT   Element;
//                      UINT8             ElementCount;
//
//                      Status = GetRbAmlPackagePointer( "PCI1", "PAK1",
//                                 &Package, &Element, &ElementCount);
//                      ...
//                  }
//
//
//-------------------------------------------------------------------------- 
// <AMI_PHDR_END>

EFI_STATUS GetRbAmlPackagePointer(
    IN  ACPI_HDR            **Dsdt,
    IN  CHAR8               *RbName,
    IN  CHAR8               *PackageName,
    OUT ASL_OBJ_INFO        *Package,
    OUT PACKAGE_ELEMENT     *Elements,
    OUT UINT8               *ElementCount )
{
    EFI_STATUS              Status;
    EFI_PHYSICAL_ADDRESS    Address;
    ASL_OBJ_INFO            RbScope;
    AML_PACKAGE_HEADER      *Pkg;

    if (*Dsdt == NULL){
        Status = LibGetDsdt(&Address, EFI_ACPI_TABLE_VERSION_ALL);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        else {
            *Dsdt = (ACPI_HDR*)Address;
        }
    }
    
    // Get a pointer to this root bridge's device scope

    Status = GetAslObj( (UINT8*)(*Dsdt + 1), 
                        (*Dsdt)->Length - sizeof(ACPI_HDR) - 1, 
                        (UINT8 *)RbName, 
                        otDevice, 
                        &RbScope );

    // Get a pointer to the AML Package object

    if (!EFI_ERROR(Status)){
        Status = GetAslObj( (UINT8*)RbScope.DataStart,
                            RbScope.Length,
                            (UINT8 *)PackageName,
                            otName,
                            Package );
    }
    
    // Parse the Package object to get a pointer to the first element and
    // number of elements.

    if (!EFI_ERROR(Status)){
        Pkg = Package->DataStart;

        // Check ByteCount to determine the encoding and obtain a pointer
        // to the first element and the element count.

        switch (Pkg->Encoding.Small.ByteCount) {
        case 0:
            (*Elements).Ptr = &Pkg->Encoding.Small.Element;
            *ElementCount = Pkg->Encoding.Small.ElementCount;
            break;
        case 1:
            (*Elements).Ptr = &Pkg->Encoding.Large1.Element;
            *ElementCount = Pkg->Encoding.Large1.ElementCount;
            break;
        case 2:
            (*Elements).Ptr = &Pkg->Encoding.Large2.Element;
            *ElementCount = Pkg->Encoding.Large2.ElementCount;
            break;
        case 3:
            (*Elements).Ptr = &Pkg->Encoding.Large3.Element;
            *ElementCount = Pkg->Encoding.Large3.ElementCount;
        }
    }

    return Status;
}


//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetPackageLen
//
// Description:
//  UINTN GetPackageLen(IN AML_PACKAGE_LBYTE *Pck, OUT UINT8 *BytesCnt)
// determines the length in bytes of an AML package.
//
// Input:
//  IN AML_PACKAGE_LBYTE *Pck
// AML package to be measured.
//
//  OUT UINT8 *BytesCnt
// The ByteCount value of the package.
//
// Output:
//  UINTN size of the AML package in bytes.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
//will determine length in bytes of a package
UINTN GetPackageLen(AML_PACKAGE_LBYTE *Pck, UINT8 *BytesCnt)
{
    UINTN   len=0;
    UINT8   *p=(UINT8*)Pck;
    
//------------------
    if (BytesCnt)*BytesCnt=0;
    
    if (!Pck->ByteCount) len=Pck->Length;
    else
    {
        len|=(Pck->Length&0x0F);
        
        if (BytesCnt)*BytesCnt=Pck->ByteCount;
        
        switch (Pck->ByteCount)
        {
            case 1:
                {
                    UINT8   b=*(p+1);
                    len|=(b<<4);
                } break;
            case 2:
                {
                    UINT16 w=(UINT16)(*((UINT16*)(p+1)));
                    len|=(w<<4);
                } break;
            case 3:
                {
                    UINT32 d=(UINT32)(*((UINT32*)(p+1)));
                    d&=0xffffff;
                    len|=(d<<4);
                } break;
        } //switch
    }
    
    return len;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ConvertAslName
//
// Description:
//  UINT32 ConvertAslName(IN UINT8 *AslName) converts an ASL name into a
// UINT32 value and returns it.
//
// Input:
//  IN UINT8 *AslName
// Pointer to the ASL name string.
//
// Output:
//  UINT32 translation of the ASL name.
//
// Modified:
//
// Referrals:
//  Strlen
//  MemCpy
//
// Notes:
//  Helper function for FindAslObjectName.
//
//<AMI_PHDR_END>
//*************************************************************************
UINT32 ConvertAslName(UINT8 *AslName)
{
    AML_NAME_SEG    n;
    UINTN           l=Strlen((char *)AslName);
//-------------------------
    n.NAME=0x5F5F5F5F;
    MemCpy(&n.Name[0],AslName,l);
    return n.NAME;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FindAslObjectName
//
// Description:
//  VOID* FindAslObjectName(IN UINT8 *Start, IN UINT8 *ObjName, IN UINTN Len)
// finds an ASL object by its name string and returns its location.
//
// Input:
//  IN UINT8 *Start
// Location to begin scanning for the ASL object.
//
//  IN UINT8 *ObjName
// Name of the ASL object to be located.
//
//  IN UINTN Len
// Maximum length of bytes to scan for the ASL object.
//
// Output:
//  VOID* pointer to the start of the ASL name string in memory.
//
// Modified:
//
// Referrals:
//  ConvertAslName
//
// Notes:
//  Helper function for GetAslObj.
//
//<AMI_PHDR_END>
//*************************************************************************
VOID* FindAslObjectName(UINT8 *Start, UINT8 *ObjName, UINTN Len)
{
    unsigned int    i;
    UINT8           *p;
    AML_NAME_SEG    *obj,nm;
//------------------------------
    nm.NAME=ConvertAslName(ObjName);
    
    p=Start;
    
    for (i=0; i<Len; i++)
    {
        obj=(AML_NAME_SEG*)(&p[i]);
        
        if (obj->NAME!=nm.NAME)continue;
        
        return &p[i];
    }
    
    return NULL;
}
//*************************************************************************
//<AMI_PHDR_START>
//
// Name: HideAslMethodFromOs
//
// Description:
// Destroys Method passed as MethodName parameter if it is present in ASL object
// described by AslObj parameter
//
// Input:
//  IN ASL_OBJ_INFO *AslObj
//  Pointer to ASL_OBJ_INFO structure, which discribes ASL object to modify
//
//  UINT8 *MethodName
//  Name of the ASL Method to be destroyed.
//
//
// Referrals:
//  ConvertAslName
//
//<AMI_PHDR_END>
//*************************************************************************
VOID HideAslMethodFromOs (ASL_OBJ_INFO *AslObj, UINT8 *MethodName)
{
    UINT32  AslMethodName = ConvertAslName(MethodName), i, Position; //*(UINT32*)MethodName
    UINT8   *Start = AslObj->DataStart;
    
//   TRACE ((DEBUG_INFO,"AslMethodNameHex = %X, Str=%s; Start= %lX\n", AslMethodName, MethodName, (UINT64) Start));

    for (i = 0; i < AslObj->Length; i++)
    {
        Position =*(UINT32*)(Start+i);
        
        if (Position == AslMethodName)
            // If method is found
        {
//            TRACE ((DEBUG_INFO,"METHOD FOUND = %x,\n", Position));
            Start[i] = 'X'; //Replace leading _ with X
            return;
        }
    }
    
}


//*************************************************************************
//<AMI_PHDR_START>
//
// Name: CheckOpCode
//
// Description:
//  BOOLEAN CheckOpCode(IN UINT8 *NameStart, IN UINT8 OpCode,
// IN UINT8 ExtOpCode, OUT ASL_OBJ_INFO *ObjInfo) checks an ASL object for
// the user provided op code and extended op code and returns the op code
// location in a user allocated ASL_OBJ_INFO structure.
//
// Input:
//  IN UINT8 *NameStart
// Pointer to the start of the ASL object's name.
//
//  IN UINT8 OpCode
// Op code to be checked.
//
//  IN UINT8 ExtOpCode
// Extended op code to be checked.  If set as 0, only checks for OpCode.
//
//  OUT ASL_OBJ_INFO *ObjInfo
// Pointer to a user allocated ASL_OBJ_INFO structure which will be updated
// with the associated information if the op codes are located.  If an
// extended op code is specified, ObjInfo->Object is filled with the location
// of the extended op code; otherwise ObjInfo->Object refers to the location
// of the op code.
//
// Output:
//  TRUE if the op code and extended op code (if specified) are located;
// otherwise, FALSE.
//
// Modified:
//
// Referrals:
//  GetPackageLen
//
// Notes:
//  Helper function for GetAslObj.
//
//<AMI_PHDR_END>
//*************************************************************************
BOOLEAN CheckOpCode(UINT8 *NameStart, UINT8 OpCode, UINT8 ExtOpCode, ASL_OBJ_INFO *ObjInfo)
{
    UINT8   *p, *p1;
    INTN    i;
    
//---------------------
    //Maximum number of bytes in PackageLength is 4
    if ( (*(NameStart-1))==AML_PR_ROOT || (*(NameStart-1))==AML_PR_PARENT) NameStart--;
    
    //if we Looking For ASL Name Object its a special case.
    if (OpCode==AML_OP_NAME && !ExtOpCode)
    {
        p=NameStart-1;
        
        if (*p==OpCode)
        {
            ObjInfo->Object=p;
            ObjInfo->ObjName=NameStart;
            ObjInfo->Length=0;
            return TRUE;
        }
        
        return FALSE;
    }
    
    //if we Looking For ASL Name Object its a special case.
    if (OpCode==AML_OP_OPREG && ExtOpCode)
    {
        p=NameStart-2;
        
        if (*p==ExtOpCode && *(p+1)==OpCode)
        {
            ObjInfo->Object=p;
            ObjInfo->ObjName=NameStart;
            ObjInfo->Length=0;
            return TRUE;
        }
        
        return FALSE;
    }
    
    for (i=2; i<6; i++)
    {
        p=NameStart-i;
        
        if (ExtOpCode) p1=p-1;
        else p1=NULL;
        
        if (p1)if (*p1!=ExtOpCode) continue;
        
        if (*p!=OpCode) continue;
        
        if (p1)ObjInfo->Object=p1;
        else ObjInfo->Object=p;
        
        ObjInfo->Length=GetPackageLen((AML_PACKAGE_LBYTE*)(p+1),NULL);
        ObjInfo->ObjName=NameStart;
        return TRUE;
    }
    
    return FALSE;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetAslObj
//
// Description:
//  EFI_STATUS GetAslObj(IN UINT8 *Start, IN UINTN Length, IN UINT8 *Name,
// IN ASL_OBJ_TYPE ObjType, OUT ASL_OBJ_INFO *ObjInfo)
//
// Input:
//  IN UINT8 *Start
// Starting location in memory to begin scanning for the desired ASL object.
//
//  IN UINTN Length
// Maximum number of bytes in memory to be scanned starting from Start for
// the ASL object.
//
//  IN UINT8 *Name
// Pointer to the name of the ASL object to be located.
//
//  IN ASL_OBJ_TYPE ObjType
// Type of the ASL object to be located.  Supports: otScope, otName, otProc,
// otTermal, otDevice, otMethod, otPwrRes, otOpReg
//
//  OUT ASL_OBJ_INFO *ObjInfo
// Returns completed ASL_OBJ_INFO structure, if the ASL object is found.
//
// Output:
//  EFI_NOT_FOUND,  if object not found or invalid type.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  FindAslObjectName
//  CheckOpCode
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
//Since the search is done by locating the OBJECT name.
//Caller must know what ASL Object Type he is looking for.
EFI_STATUS GetAslObj(UINT8 *Start, UINTN Length, UINT8 *Name, ASL_OBJ_TYPE ObjType, ASL_OBJ_INFO *ObjInfo)
{
    UINT8           *pn, *s, eop,op;
    UINTN           l, i;
    BOOLEAN         objfound=FALSE;
//  ASL_OBJ_INFO    objinf;
//---------------------------
    //if(!CheckDsdt(PDsdt)) return FALSE;
    
    //s=((UINT8*)PDsdt)+sizeof(ACPI_HDR);
    s=Start;
    //l=PDsdt->Length-sizeof(ACPI_HDR)-1;
    l=Length;
    
    while (!objfound)
    {
    
        pn=FindAslObjectName(s,Name,l);
        
        if (!pn) return EFI_NOT_FOUND;
        
        //If we found the name let's check if it is the Object we are looking for
        //it could be just object referance, or field definition inside the object,
        //or double name, or multiple name definition we must filter such situations
        // and make sure we have got the actual object but not its reference instance
        switch (ObjType)
        {
            case    otScope     :
                i=0;
                eop=0;
                op=AML_OP_SCOPE;
                break;
            case    otName      :
                i=0;
                eop=0;
                op=AML_OP_NAME;
                break;
            case    otProc      :
                i=6;
                eop=AML_PR_EXTOP;
                op=AML_OP_PROC;
                break;
            case    otTermal    :
                i=0;
                eop=AML_PR_EXTOP;
                op=AML_OP_THERMAL;
                break;
            case    otDevice    :
                i=0;
                eop=AML_PR_EXTOP;
                op=AML_OP_DEVICE;
                break;
            case    otMethod        :
                i=1;
                eop=0;
                op=AML_OP_METHOD;
                break;
            case    otPwrRes    :
                i=3;
                eop=AML_PR_EXTOP;
                op=AML_OP_PWRRES;
                break;
            case    otOpReg :
                i=0;
                eop=AML_PR_EXTOP;
                op=AML_OP_OPREG;
                break;
                
            default: return EFI_NOT_FOUND;
        } //switch
        
        objfound=CheckOpCode(pn, op, eop, ObjInfo);
        
        if (!objfound)
        {
            l-=(UINTN)(pn-s)-4;
            s=pn+4;
            continue;
        }
        
        ObjInfo->ObjType=ObjType;
        ObjInfo->DataStart=pn+i+4;
        
        return EFI_SUCCESS;
    } //while ()
    
    return EFI_NOT_FOUND;
}


//*************************************************************************
//<AMI_PHDR_START>
//
// Name: UpdateAslNameBuffer
//
// Description:
//  EFI_STATUS UpdateAslNameBuffer(UINT8 *BufferStart, UINTN BufferLen,
//  IN UINT8 *ObjName, IN UINT64 Value) locates an ASL object by its name,
//  ObjName, in Memory Buffer specified by BufferStart, and BufferLen
//  and updates its data start with the user provided Value.
//
// Input:
//  IN UINT8 *BufferStart
// Pointer to the DSDT.
//
//  UINTN BufferLen
// Length of the Buffer
//
//  IN UINT8 *ObjName
// Name of the ASL object to be updated.
//
//  IN UINT64 Value
// New value for the ASL object's data entry.
//
// Output:
//  EFI_INVALID_PARAMETER,  if the DSDT pointer, ObjName or Value are invalid.
//  EFI_NOT_FOUND,          if object specified by ObjName not present.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  GetAslObj
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS  UpdateAslNameBuffer(UINT8 *BufferStart, UINTN BufferLen, UINT8 *ObjName, UINT64 Value)
{
    EFI_STATUS          Status;
    ASL_OBJ_INFO        obj;
//---------------
    Status=GetAslObj(BufferStart,BufferLen, ObjName, otName, &obj);
    
    if (EFI_ERROR(Status)) return Status;
    
    switch (*((UINT8*)obj.DataStart))
    {
        case AML_PR_BYTE:
            {
                UINT8   *p = (UINT8*)((UINT8*)obj.DataStart+1);
                //----------------------------------
                *p=(UINT8)Value;
            } break;
            
        case AML_PR_WORD:
            {
                UINT16  *p = (UINT16*)((UINT8*)obj.DataStart+1);
                //----------------------------------
                *p=(UINT16)Value;
            } break;
            
        case AML_PR_DWORD:
            {
                UINT32  *p = (UINT32*)((UINT8*)obj.DataStart+1);
                //----------------------------------
                *p=(UINT32)Value;
            } break;
            
        case AML_PR_QWORD:
            {
                UINT64  *p = (UINT64*)((UINT8*)obj.DataStart+1);
                //----------------------------------
                *p=(UINT64)Value;
            } break;
            
        case AML_ZERO_OP: case AML_ONE_OP:
            {
                if ((Value == 0) || (Value == 1))
                {
                    UINT8 *p = (UINT8*)((UINT8*)obj.DataStart);
                    *p = (UINT8)Value;
                }
                
                else
                {
                    return EFI_INVALID_PARAMETER;
                }
            } break;
        default:
            {
                return EFI_INVALID_PARAMETER;
            }
    }
    
    return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: UpdateAslNameOfDevice
//
// Description:
//  EFI_STATUS UpdateAslNameOfDevice(IN ACPI_HDR *PDsdt, IN CHAR8 *DeviceName,
//  CHAR8 *ObjName, IN UINT64 Value) locates an ASL Device Object by its name,
//  DeviceName, and then In the scope of this Device Object, trys to locate a
//  Name Object, ObjName, in the DSDT. If found it updates its data place holder
//  with user provided Value.
//
// Input:
//  IN ACPI_HDR *PDsdt
// Pointer to the DSDT.
//
//  IN CHAR8 *DeviceName
// Name of the ASL Device object having in it's scope Name Object to be updated.
//
//  IN CHAR8 *ObjName
// Name of the ASL Name Object to be updated.
//
//  IN UINT64 Value
// New value for the ASL object's data entry.
//
// Output:
//  EFI_INVALID_PARAMETER,  if the DSDT pointer, ObjName or Value are invalid.
//  EFI_NOT_FOUND,          if object specified by Device Name Or ObjName not present.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  CheckDsdt
//  GetAslObj
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS UpdateAslNameOfDevice(ACPI_HDR *PDsdt, CHAR8 *DeviceName, CHAR8 *ObjName, UINT64 Value)
{
    EFI_STATUS          Status;
    ASL_OBJ_INFO        obj;
    
//---------------
    if (!CheckDsdt(PDsdt)) return EFI_INVALID_PARAMETER;
    
    Status=GetAslObj((UINT8*)(PDsdt+1),PDsdt->Length-sizeof(ACPI_HDR)-1, (UINT8 *)DeviceName, otDevice, &obj);
    
    if (EFI_ERROR(Status)) return Status;
    
    Status=UpdateAslNameBuffer((UINT8*)obj.DataStart, obj.Length, (UINT8 *)ObjName, Value);
    return Status;
}



//*************************************************************************
//<AMI_PHDR_START>
//
// Name: UpdateAslNameObject
//
// Description:
//  EFI_STATUS UpdateAslNameObject(IN ACPI_HDR *PDsdt, IN UINT8 *ObjName,
// IN UINT64 Value) locates an ASL object by its name, ObjName, in the DSDT
// and updates its data start with the user provided Value.
//
// Input:
//  IN ACPI_HDR *PDsdt
// Pointer to the DSDT.
//
//  IN UINT8 *ObjName
// Name of the ASL object to be updated.
//
//  IN UINT64 Value
// New value for the ASL object's data entry.
//
// Output:
//  EFI_INVALID_PARAMETER,  if the DSDT pointer, ObjName or Value are invalid.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  CheckDsdt
//  GetAslObj
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS  UpdateAslNameObject(ACPI_HDR *PDsdt, UINT8 *ObjName, UINT64 Value)
{
//------------------------------------
    if (!CheckDsdt(PDsdt)) return EFI_INVALID_PARAMETER;
    
    return UpdateAslNameBuffer((UINT8*)(PDsdt+1),PDsdt->Length-sizeof(ACPI_HDR)-1, ObjName, Value);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ChsumTbl
//
// Description:
//  UINT8 ChsumTbl(IN UINT8 *TblStart, IN UINT32 BytesCount) calculates the
// checksum of a table starting at TblStart of length BytesCount and returns
// the checksum value.
//
// Input:
//  IN UINT8 *TblStart
// Starting address of the memory area to checksum.
//
//  IN UINT32 BytesCount
// Length in bytes of the memory area to checksum.
//
// Output:
//  UINT8 checksum value starting from TblStart and ending at
// TblStart + BytesCount.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//<AMI_PHDR_END>
//*************************************************************************
UINT8 ChsumTbl(UINT8* TblStart, UINT32 BytesCount)
{
    UINTN       i;
    UINT8       res=*TblStart;
    
    for (i=1; i<BytesCount; i++) res+=TblStart[i];
    
    res=0-res;
    return(res);
} //checksum_table

static EFI_GUID gAmiIsaDmaIrqMaskVarGuid = AMI_IRQ_DMA_MASK_VARIABLE_GUID;

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AmiIsaIrqMask
//
// Description:
//  EFI_STATUS AmiIsaIrqMask(IN OUT UINT16 *IsaIrqMask, IN BOOLEAN Get) sets
// or retrieves the ISA IRQ mask from NVRAM depending upon the Get input.
//
// Input:
//  IN OUT UINT16 *IsaIrqMask
// If Get is TRUE, returns an address to the ISA IRQ mask.  If Get is FALSE,
// used to update the ISA IRQ mask NVRAM variable.  User is responsible for
// allocating and managing this memory.
//
//  IN BOOLEAN Get
// If TRUE, causes function to return an address to the ISA IRQ mask.
// If FALSE, causes function to use the user provided ISA IRQ mask to update
// the NVRAM variable with its value.
//
// Output:
//  EFI_NOT_FOUND,           if the ISA IRQ mask NVRAM variable is not found.
//  EFI_DEVICE_ERROR,        if the ISA IRQ mask NVRAM variable could not be
//                          accessed due to a hardware error.
//  EFI_SECURITY_VIOLATION,  if the ISA IRQ mask NVRAM variable could be not
//                          be accessed due to a authentication failure.
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
EFI_STATUS AmiIsaIrqMask(UINT16 *IsaIrqMask, BOOLEAN Get)
{
    UINTN       sz=sizeof(UINT16);
    EFI_STATUS  Status;
//-----------------------

    if (Get) Status = pRS->GetVariable(L"IsaIrqMask", &gAmiIsaDmaIrqMaskVarGuid, NULL, &sz, IsaIrqMask);
    else Status=pRS->SetVariable(L"IsaIrqMask",&gAmiIsaDmaIrqMaskVarGuid,EFI_VARIABLE_BOOTSERVICE_ACCESS, sz, IsaIrqMask);
    
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AmiIsaDmaMask
//
// Description:
//  EFI_STATUS AmiIsaDmaMask(IN OUT UINT8 *IsaDmaMask, IN BOOLEAN Get) sets
// or retrieves the ISA DMA mask from NVRAM depending upon the Get input.
//
// Input:
//  IN OUT UINT8 *IsaDmaMask
// If Get is TRUE, returns an address to the ISA DMA mask.  If Get is FALSE,
// used to update the ISA DMA mask NVRAM variable.  User is responsible for
// allocating and managing this memory.
//
//  IN BOOLEAN Get
// If TRUE, causes function to return an address to the ISA DMA mask.
// If FALSE, causes function to use the user provided ISA DMA mask to update
// the NVRAM variable with its value.
//
// Output:
//  EFI_NOT_FOUND,           if the ISA DMA mask NVRAM variable is not found.
//  EFI_DEVICE_ERROR,        if the ISA DMA mask NVRAM variable could not be
//                          accessed due to a hardware error.
//  EFI_SECURITY_VIOLATION,  if the ISA DMA mask NVRAM variable could be not
//                          be accessed due to a authentication failure.
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
EFI_STATUS AmiIsaDmaMask(UINT8 *IsaDmaMask, BOOLEAN Get)
{
    UINTN       sz=sizeof(UINT8);
    EFI_STATUS  Status;
//-----------------------

    if (Get) Status = pRS->GetVariable(L"IsaDmaMask", &gAmiIsaDmaIrqMaskVarGuid, NULL, &sz, IsaDmaMask);
    else Status=pRS->SetVariable(L"IsaDmaMask",&gAmiIsaDmaIrqMaskVarGuid,EFI_VARIABLE_BOOTSERVICE_ACCESS, sz, IsaDmaMask);
    
    return Status;
}

static EFI_GUID gAmiPciOutOfResVarGuid = AMI_PCI_OUT_OF_RESOURCES_GUID;

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AmiPciOutOfRes
//
// Description:
//  EFI_STATUS AmiPciOutOfRes(AMI_OUT_OF_RES_VAR *AmiOutOfResData, IN BOOLEAN Get) sets
// or retrieves the AMI_OUT_OF_RES_DATA from NVRAM depending upon the Get input.
//
// Input:
//  AMI_OUT_OF_RES_VAR *AmiOutOfResData
// If Get is TRUE, returns an address to the AMI_OUT_OF_RES_VAR.  If Get is FALSE,
// used to update the AMI_OUT_OF_RES_VAR data NVRAM variable.  User is responsible for
// allocating and managing this memory.
//
//  IN BOOLEAN Get
// If TRUE, causes function to return an address to the ISA DMA mask.
// If FALSE, causes function to use the user provided ISA DMA mask to update
// the NVRAM variable with its value.
//
// Output:
//  EFI_NOT_FOUND,           if the NVRAM variable is not found.
//  EFI_DEVICE_ERROR,        if the NVRAM variable could not be
//                          accessed due to a hardware error.
//  EFI_SECURITY_VIOLATION,  if the NVRAM variable could be not
//                          be accessed due to a authentication failure.
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
EFI_STATUS AmiPciOutOfRes(AMI_OUT_OF_RES_VAR *AmiOutOfResData, BOOLEAN Get)
{
    AMI_OUT_OF_RES_VAR  orv;
    UINTN               sz=sizeof(AMI_OUT_OF_RES_VAR);
    EFI_STATUS          Status;
//-----------------------
    if(!Get && AmiOutOfResData==NULL) return EFI_INVALID_PARAMETER;

    if (Get) Status = pRS->GetVariable(L"AmiOutOfRes", &gAmiPciOutOfResVarGuid, NULL, &sz, &orv);
    else Status=pRS->SetVariable(L"AmiOutOfRes",&gAmiPciOutOfResVarGuid,EFI_VARIABLE_BOOTSERVICE_ACCESS, sz, AmiOutOfResData);

    if(Get && AmiOutOfResData!=NULL) *AmiOutOfResData=orv;
    
    return Status;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: LibGetDsdt
//
// Description:
//  EFI_STATUS LibGetDsdt(OUT EFI_PHYSICAL_ADDRESS *DsdtAddr,
// IN EFI_ACPI_TABLE_VERSION Version) retrieves a pointer to the DSDT ACPI
// table for the user requested Version of ACPI.
//
// Input:
//  OUT EFI_PHYSICAL_ADDRESS *DsdtAddr
// Address of the DSDT associated with the user provided Version.
//
//  IN EFI_ACPI_TABLE_VERSION Version
// Which version of ACPI's DSDT to return.  Valid values include:
//  EFI_ACPI_TABLE_VERSION_1_0B, EFI_ACPI_TABLE_VERSION_X,
//  EFI_ACPI_TABLE_VERSION_NONE
//
// Output:
//  EFI_NOT_AVAILABLE_YET,  if ACPI support protocol not installed yet.
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
EFI_STATUS LibGetDsdt(EFI_PHYSICAL_ADDRESS *DsdtAddr, EFI_ACPI_TABLE_VERSION Version)
{
    EFI_PHYSICAL_ADDRESS Dsdt1 = 0;
    EFI_PHYSICAL_ADDRESS Dsdt2 = 0;
    EFI_ACPI_SDT_HEADER         *tbl = NULL;
    EFI_ACPI_SDT_PROTOCOL       *as;

    EFI_STATUS                  Status = EFI_NOT_FOUND;
    UINTN                       i;
    EFI_ACPI_TABLE_VERSION      ver;
    UINTN                       AcpiHandle;
    
//---------------------------

    Status=pBS->LocateProtocol(&gLocalEfiAcpiSdtProtocolGuid, NULL, (VOID **)&as);
        
    if (EFI_ERROR(Status)) {
        TRACE((DEBUG_ERROR,"AcpiResLib: LibGetDsdt(): LocateProtocol(ACPISupport) returned %r \n", Status));
        return EFI_NOT_AVAILABLE_YET;
    } else {
        for (i=0; ; i++) {
            Status = as->GetAcpiTable(i, &tbl, &ver, &AcpiHandle);
            
            if (EFI_ERROR(Status)) {
                TRACE((DEBUG_ERROR,"Can't find DSDT table -> %r search %d Tables\n", Status, i));
                break;
            }
            
            if (tbl->Signature == FACP_SIG) {
                if (ver == EFI_ACPI_TABLE_VERSION_1_0B) {
                    Dsdt1 = (EFI_PHYSICAL_ADDRESS)(((FACP_20 *)tbl)->DSDT);
                    TRACE((DEBUG_INFO,"AcpiResLib: LibGetDsdt(): Found v1.0b   RSDT->DSDT @ 0x%lX; -> %r \n", Dsdt1, Status));
                }
                
                if ((ver & EFI_ACPI_TABLE_VERSION_X)!= 0) {
                    Dsdt2 = (EFI_PHYSICAL_ADDRESS)(((FACP_20 *)tbl)->X_DSDT);
                    TRACE((DEBUG_INFO,"AcpiResLib: LibGetDsdt(): Found v2.0&UP XSDT->DSDT @ 0x%lX; -> %r \n", Dsdt2, Status));
                }
            }
            
            pBS->FreePool((VOID *)tbl);
            
            if (Dsdt1 && Dsdt2) 
                break;
        }
    }
    
    if (Dsdt2 && ((Version & EFI_ACPI_TABLE_VERSION_X) || (Version & EFI_ACPI_TABLE_VERSION_NONE))) {
        *DsdtAddr = Dsdt2;
        Status = EFI_SUCCESS;
    } else {
        if (Dsdt1 && ((Version & EFI_ACPI_TABLE_VERSION_1_0B) || (Version & EFI_ACPI_TABLE_VERSION_NONE))) {
            *DsdtAddr = Dsdt1;
            Status = EFI_SUCCESS;
        }
    }
    return Status;
}



//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
