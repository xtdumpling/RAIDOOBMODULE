//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
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
// $Header: /Alaska/SOURCE/Core/Library/Memory.c 9     4/26/11 10:45p Markw $
//
// $Revision: 9 $
//
// $Date: 4/26/11 10:45p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: Memory.c
//
// Description: 
//  Contains memory related library functions.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Efi.h>
#include <AmiLib.h>

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Memory_Functions
//
// Description:
//  Memory functions defined in the AMI library.  Note some may only be
// available in DXE.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiLib    MemSet   Fills a buffer with a user provided value. 
// AmiLib    MemCmp   Compare two buffers for equality.
// AmiLIb    MemCpy   Copy a buffer into another buffer.
// AmiDxeLib Malloc   Allocate memory from EfiBootServicesData.
// AmiDxeLib MallocZ  Allocate memory from EfiBootServicesData that has been cleared with zeros.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions.  Append .h to the given name.
//  
//<AMI_GHDR_END>
//*************************************************************************

VOID CPULib_CpuID(IN UINT32 CpuIDIndex, OUT UINT32 *RegEAX, OUT UINT32 *RegEBX, 
  IN OUT UINT32 *RegECX, OUT UINT32 *RegEDX);

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MemCmp
//
// Description:	
//  INTN MemCmp(IN VOID* pDestination, IN VOID* pSource, IN UINTN Count)
// compares Length bytes of pDestination to Length bytes of pSource.  If all
// Length bytes of the two buffers are identical, then 0 is returned.
// Otherwise, the value returned is the first mismatched byte in pSource
// subtracted from the first mismatched byte in pDestination.
//
// Input:
//  IN VOID* pDestination
// Pointer to the first buffer to compare.
//
//  IN VOID* pSource
// Pointer to the second buffer to compare.
//
//  IN UINTN Count
// Number of bytes to compare.
//
// Output:
//  INTN value of first mismatched byte in pSource subtracted from the first
// mismatched byte in pDestination (pDestination - pSource). Returns 0 if
// both buffers are the same. 
//
// Modified:
//
// Referrals:
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
INTN MemCmp(VOID* pDestination, VOID* pSource, UINTN Count){
    INT8 *d = (INT8*)pDestination, *s = (INT8*)pSource;
    INT8 *end=d+Count;
    UINTN r;
////////////////////////////////////////////////////////////
// NOTE: in order to increase the comparison speed we will
// compare sizeof(UINTN) bytes at a time if possible.
// Once inequality is found, we will go to the last while loop
// to do a byte by byte comparison to get proper results.
// We have to do it because of the following:
// Let's say we are trying to compare values: 0x1234 and 0x4321:
// If we compare them as double words the second value will be greater.
// If we compare them byte by byte the first value wiill be greater
// (because of little endian architecture of x86)
////////////////////////////////////////////////////////////
    if (Count >= sizeof(UINTN)){
        r = (UINTN)d & (sizeof(UINTN)-1);
        if (r && r== ((UINTN)s & (sizeof(UINTN)-1))){
            r = sizeof(UINTN)-r;
            Count-=r;
            for(;r;r--) 
                if (*d==*s){d++;s++;}
                //the values are not equal 
                //let's break to do a byte by byte comparison 
                //to figure out which one is greater
                else break;
        }
        while(d<=end-sizeof(UINTN)) 
            if (*(UINTN*)d==*(UINTN*)s){
                d+=sizeof(UINTN);s+=sizeof(UINTN);
            }
            //the values are not equal 
            //let's break to do a byte by byte comparison 
            //to figure out which one is greater
            else break; 
    }
    while(d<end)
        if (*d==*s){d++;s++;}
        else return *d-*s;
    return 0;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: GetPageTableNumPages
//
// Description:
//  UINT32 GetPageTableNumPages(IN UINT8 NumberMemoryBits) gets the number
// of pages to allocate for paging.
// Unlimited if 1 GB page tables is supported. Otherwise, limit 512 MB address space.
//
// Input:
//  IN UINT8 NumberMemoryBits
// Number of memory bits to map.
//
// Output:
//  UINT32 number of pages.
//
// Modified:
//
// Referrals:
//  Shl64
//  CPULib_CpuID
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
UINT32 GetPageTableNumPages(
    IN UINT8 NumberMemoryBits
)
{
    BOOLEAN GigPageSupport;
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT32 NumPages;

    CPULib_CpuID(0x80000001, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    GigPageSupport = !!(RegEdx & BIT26);

    //Limit number of memory bits to 39 (512 GB) if no support for GB page-tables.
    if (!GigPageSupport && NumberMemoryBits > 39) NumberMemoryBits = 39;

    if (GigPageSupport) {
    	//1 page for PML4E Table.
        //4 page for for Directory. First 4GB of 2MB pages.
        //1 Page for Page-Table Entries for 1st 2MB of 4k pages.        
        NumPages = 6;
 
        //Allocate Pages for Directory Pointer.
        NumPages += 1 << (NumberMemoryBits > 39 ? NumberMemoryBits - 39 : 0);
    } else {
    	//Allocate at least 7 pages to cover Identity Mapping for 32 bits.
    	//1 page for PML4E Table
    	//1 page for Page-Directory Pointer
        //Pages for Directory allocated later--one for each directory.
        //1 page for Page-Table Entries for 1st 2MB. This must be 4k blocks.
        NumPages = 3;
    
        //Allocate pages for page directories.
        NumPages += 1 << (NumberMemoryBits - 30);
    }
    return NumPages;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: FillPageTable
//
// Description:
//  VOID FillPageTable(IN UINT8 NumberMemoryBits, IN VOID *PageTable) fills a
// provided page table with an identity map.
//
// Input:
//  IN UINT8 NumberMemoryBits
// Number of memory bits to map.
//
//  IN VOID *PageTable
// Page table to fill.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//  Shl64
//  MemSet
//  CPULib_CpuID
// 
// Notes:	
//          
//<AMI_PHDR_END>
//*************************************************************************
VOID FillPageTable(
    IN UINT8  NumberMemoryBits,
    IN VOID   *PageTable
)
{
    BOOLEAN GigPageSupport;
    UINT32 RegEax, RegEbx, RegEcx, RegEdx;
    UINT64 *Pml4eTable = (UINT64*)PageTable;
	UINT64 *PdpTable   = (UINT64*)((UINT8*)PageTable + 0x1000);
	UINT64 *PDir;
    UINT64 *PTableEntry;
    UINT32 NumPml4Entries;
    UINT32 NumPgDirPtrEntries;
    UINT32 NumPgDirEntries;
    UINT32 i;

    CPULib_CpuID(0x80000001, &RegEax, &RegEbx, &RegEcx, &RegEdx);
    GigPageSupport = !!(RegEdx & BIT26);

    //Limit number of memory bits to 39 (512 GB) if no support for GB page-tables.
    if (!GigPageSupport && NumberMemoryBits > 39) NumberMemoryBits = 39;

	// Update Page-Map Level-4 tables.
    NumPml4Entries =  1 << (NumberMemoryBits > 39 ? NumberMemoryBits - 39 : 0);
    for (i = 0; i < NumPml4Entries; ++i) Pml4eTable[i] = BIT0 + BIT1 + (i << 12) + (UINT64)(UINTN)PdpTable;
	MemSet(Pml4eTable + i, 4096 - 8 * i, 0);	//Clear unused entries.

	//Update Page Pointer Directories.
    NumPgDirPtrEntries = 1 << (NumberMemoryBits - 30);
    PDir = PdpTable + (NumPgDirPtrEntries <= 512 ? 512 : NumPgDirPtrEntries);
    if (GigPageSupport) {
        //1st 4 pages must point to a Page directory
        PdpTable[0] = BIT0 + BIT1 + (0 << 12) + (UINT64)(UINTN)PDir;
        PdpTable[1] = BIT0 + BIT1 + (1 << 12) + (UINT64)(UINTN)PDir;
        PdpTable[2] = BIT0 + BIT1 + (2 << 12) + (UINT64)(UINTN)PDir;
        PdpTable[3] = BIT0 + BIT1 + (3 << 12) + (UINT64)(UINTN)PDir;
        for (i = 4; i <  NumPgDirPtrEntries; ++i)
            PdpTable[i] = BIT0 + BIT1 + BIT7 + Shl64(i, 30);
    } else {
        for (i = 0; i <  NumPgDirPtrEntries; ++i)
            PdpTable[i] = BIT0 + BIT1 + (i << 12) + (UINT64)(UINTN)PDir;
    }
    if (i < 512) MemSet(PdpTable + i, 4096 - 8 * i, 0);	//Clear unused entries.

	//Initialize Page Directores.
    if (GigPageSupport) NumPgDirEntries = 2048;  //First 4 GB
    else NumPgDirEntries = 1 << (NumberMemoryBits - 21); //Number of 2MB pages.
    PTableEntry = PDir + NumPgDirEntries;
    PDir[0] = BIT0 + BIT1 + (UINT64)(UINTN)PTableEntry;  //4K Page Table for first 2MB.
	for(i = 1; i < NumPgDirEntries; ++i) PDir[i] = 0x83 + Shl64(i, 21); 

    //Initialize 4k page entries for first 2MB.
    for(i = 0; i < 512; ++i) PTableEntry[i] = BIT0 + BIT1 + (i << 12);
}

VOID MemCpyAsm(VOID* pDestination, VOID* pSource, UINTN Length);
VOID MemSetAsm(VOID* pBuffer, UINTN Size, UINT8 Value);
VOID MemCpy(VOID* pDestination, VOID* pSource, UINTN Length){
	MemCpyAsm(pDestination, pSource, Length);
}
VOID MemSet(VOID* pBuffer, UINTN Size, UINT8 Value){
	MemSetAsm(pBuffer, Size, Value);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
