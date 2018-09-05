//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
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
// $Header: /Alaska/SOURCE/Core/Modules/IA32Core/IA32CLib.c 14    11/11/11 3:39p Artems $
//
// $Revision: 14 $
//
// $Date: 11/11/11 3:39p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: IA32CLib.c
//
// Description:	
//  Generic CPU library functions for the IA32 architecture.   See function
// definitions in the x64 library;  most IA32 functions have been removed
// from help builder output to fix a name collision issue.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Pei.h>
#include <AmiLib.h>
#include <Hob.h>

//*************************************************************************
// Math
//*************************************************************************

//*************************************************************************
//
// Name: Shr64
//
// Description:
//  UINT64 Shr64(IN UINT64 Value, IN UINT8 Shift) shifts the 64-bit Value
// right the provided number of bits, Shift.
//
// Input:
//  IN UINT64 Value
// The value to be shifted.
//
//  IN UINT8 Shift
// The number of bits to shift right.
//
// Output:
//  UINT64 Value shifted right Shift number of bits.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//*************************************************************************
UINT64 Shr64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	_asm {
		mov	edx, dword ptr Value[4]
		mov	eax, dword ptr Value
		mov	cl, Shift

		cmp	cl, 64
		jb	less_64
		xor	eax, eax
		xor	edx, edx
		jmp	exit
less_64:
		cmp	cl, 32			//Shift is 32 modulo
		jb less_32

		mov		eax, edx
		xor		edx, edx
less_32:
		shrd	eax, edx, cl
		shr		edx, cl
exit:
	}
}

//*************************************************************************
//
// Name: Shl64
//
// Description:
//  UINT64 Shl64(IN UINT64 Value, IN UINT8 Shift) shifts the 64-bit Value
// left the provided number of bits, Shift.
//
// Input:
//  IN UINT64 Value
// The value to be shifted left.
//
//  IN UINT8 Shift
// The number of bits to shift.
//
// Output:
//  UINT64 Value shifted left Shift number of bits.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//*************************************************************************
UINT64 Shl64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	_asm {
		mov	edx, dword ptr Value[4]
		mov	eax, dword ptr Value
		mov	cl, Shift

		cmp	cl, 64
		jb	less_64
		xor	eax, eax
		xor	edx, edx
		jmp	exit
less_64:
		cmp	cl, 32			//Shift is 32 modulo
		jb less_32

		mov		edx, eax
		xor		eax, eax
less_32:
		shld	edx, eax, cl
		shl		eax, cl
exit:
	}
}

//*************************************************************************
//
// Name: Div64
//
// Description:
//  UINT64 Div64(IN UINT64 Dividend, IN UINTN Divisor, 
// OUT UINTN *Remainder OPTIONAL) divides a 64-bit number, Dividend, by the
// Divisor, which can be up to 31-bits.
//
// Input:
//  IN UINT64 Dividend
// The 64-bit number to be divided.
//
//  IN UINT Divisor
// The number to divide Dividend by; may not exceed 31-bits in size.
//
//  OUT UINTN *Remainder OPTIONAL
// The remainder of the division.  Provide NULL if undesired; otherwise user
// is responsible for handling the necessary memory resources.
//
// Output:
//  UINT64 result of dividing Dividend by Divisor.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//*************************************************************************
UINT64 Div64 (
	IN UINT64	Dividend,
	IN UINTN	Divisor,	//Can only be 31 bits.
  	OUT UINTN	*Remainder OPTIONAL
  	)
{
	UINT64	Result;
  	UINT32	Rem;
 	_asm
	{
		mov		eax, dword ptr Dividend[0]
		mov		edx, dword ptr Dividend[4]
		mov		esi, Divisor
		xor		edi, edi					; Remainder
		mov		ecx, 64						; 64 bits
Div64_loop:		
      	shl     eax, 1						;Shift dividend left. This clears bit 0.
      	rcl     edx, 1    
      	rcl     edi, 1						;Shift remainder left. Bit 0 = previous dividend bit 63.

      	cmp     edi, esi					; If Rem >= Divisor, don't adjust
      	cmc                                 ; else adjust dividend and subtract divisor.
      	sbb     ebx, ebx                    ; if Rem >= Divisor, ebx = 0, else ebx = -1.
		sub		eax, ebx					; if adjust, bit 0 of dividend = 1
      	and     ebx, esi					; if adjust, ebx = Divisor, else ebx = 0. 
      	sub     edi, ebx					; if adjust, subtract divisor from remainder.
		loop	Div64_loop    	

		mov		dword ptr Result[0], eax
		mov		dword ptr Result[4], edx
		mov		Rem, edi
	}

 	if (Remainder) *Remainder = Rem;

  	return Result;
}

//*************************************************************************
//
// Name: Mul64
//
// Description:
//  UINT64 Mul64(IN UINT64 Value64, IN UINTN Value32) multiplies a 64-bit
// number by a 32-bit number and returns the 64-bit result.
//
// Input:
//  IN UINTN64 Value64
// The 64-bit number to multiply by.
//
//  IN UINTN Value32
// The 32-bit number to multiply by.
//
// Output:
//  UINT64 result of multiplying Value64 by Value32.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//*************************************************************************
UINT64 Mul64(
    IN UINT64   Value64,
    IN UINTN	Value32
    )
{
    UINT64      Result;

    _asm {
        mov     eax, dword ptr Value64[0]
        mul     Value32
        mov     dword ptr Result[0], eax
        mov     dword ptr Result[4], edx
        mov     eax, dword ptr Value64[4]
        mul     Value32
        add     dword ptr Result[4], eax
    }

    return Result;
}

//*************************************************************************
// Memory Operations
//*************************************************************************
// The memory functions have been moved to MemCpy.asm and MemSet.asm files.
// Compiler wouldn't let us define these intrinsic functions, yet,
// if they are not defined we are getting an unresolved external error.

//*************************************************************************
// Debug routines
//*************************************************************************

//*************************************************************************
//
// Name: checkpoint
//
// Description:
//  VOID checkpoint(IN UINT8 c) writes the value c to port 0x80.
//
// Input:
//  IN UINT8 c
// The value/checkpoint to write to 0x80.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
// 
// Notes:
//  This routine should only be used if the PROGRESS_CODE or
// PEI_PROGRESS_CODE macros are unavailable.
// 
//*************************************************************************
VOID checkpoint(UINT8 c){
	_asm{
		mov al, c
		out 0x80,al
 	}
}

//*************************************************************************
//
// Name: GetCpuTimer
//
// Description:
//  UINT64 GetCpuTimer() returns the value of the CPU timer.
//
// Input:
//  None.
//
// Output:
//  UINT64 value of the CPU timer.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//*************************************************************************
UINT64 GetCpuTimer(){_asm rdtsc}

//*************************************************************************
// I/O Operations
//*************************************************************************

VOID EnableLongMode(VOID *PageTable, VOID *Function, VOID *Parameter1, VOID *Parameter2);

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: InitLongModeExt
//
// Description:
//  VOID InitLongMode(IN CONST EFI_PEI_SERVICES **PeiServices, IN VOID *Function,
// IN VOID *Parameter1, IN VOID *Parameter2) initializes memory page mapping,
// enables long mode and jumps to a provided function.
//
// Input:
//  IN CONST EFI_PEI_SERVICES **PeiServices
// Double pointer to the PEI Services table.
//
//  IN VOID *Function
// Pointer to a function for EnableLongMode to call.
//
//  IN VOID *Parameter1
// First parameter to provide the Function to be called.
//
//  IN VOID *Parameter2
// Second parameter to provide the Function to be called.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//  FindNextHobByType
//  EFI_ERROR
//  GetPageTableNumPages
//  FillPageTable
//  EnableLongMode
//  
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
VOID InitLongModeExt(
	IN CONST EFI_PEI_SERVICES **PeiServices,
	IN VOID *Function,
	IN VOID *Parameter1,
	IN VOID *Parameter2,
    IN UINT8 NumMemBits
)
{
	EFI_PHYSICAL_ADDRESS PageTable;
    UINT32      NumPages;
	EFI_STATUS	Status;
// In AllocatePages below, change EfiACPIMemoryNVS to EfiBootServicesData.
// It is left as EfiACPIMemoryNVS until future projects can be updated to
// a later CPU module. Future CPU modules will allocate EfiACPIMemoryNVS
// for only 32-bits.

    NumPages = GetPageTableNumPages(NumMemBits);

	Status = (*PeiServices)->AllocatePages(
		PeiServices,
		EfiACPIMemoryNVS,
		NumPages,
		&PageTable
	);
	//ASSERT_PEI_ERROR(PeiServices,Status);

    FillPageTable(NumMemBits, (VOID*)PageTable);

	EnableLongMode((VOID*)PageTable, Function, Parameter1, Parameter2);
}
//*************************************************************************
//<AMI_PHDR_START>
//
// Name: InitLongMode
//
// Description:
//  VOID InitLongMode(IN CONST EFI_PEI_SERVICES **PeiServices, IN VOID *Function,
// IN VOID *Parameter1, IN VOID *Parameter2) initializes memory page mapping,
// enables long mode and jumps to a provided function.
//
// Input:
//  IN CONST EFI_PEI_SERVICES **PeiServices
// Double pointer to the PEI Services table.
//
//  IN VOID *Function
// Pointer to a function for EnableLongMode to call.
//
//  IN VOID *Parameter1
// First parameter to provide the Function to be called.
//
//  IN VOID *Parameter2
// Second parameter to provide the Function to be called.
//
// Output:
//  VOID.
//
// Modified:
//
// Referrals:
//  FindNextHobByType
//  EFI_ERROR
//  GetPageTableNumPages
//  FillPageTable
//  EnableLongMode
//  
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
VOID InitLongMode(
	IN CONST EFI_PEI_SERVICES **PeiServices,
	IN VOID *Function,
	IN VOID *Parameter1,
	IN VOID *Parameter2
)
{


    UINT8       NumMemBits = 32;
    EFI_HOB_CPU *CpuHob;
    VOID        *FirstHob;
	EFI_STATUS	Status;

    (*PeiServices)->GetHobList(PeiServices, &FirstHob);
    //if (!FirstHob) ASSERT_PEI_ERROR(PeiServices, EFI_NOT_FOUND);

    CpuHob = (EFI_HOB_CPU*) FirstHob;
    Status = FindNextHobByType(EFI_HOB_TYPE_CPU, &CpuHob);

    //If error during release mode, The memory cache size will be 32-bits.
    //During debug mode, an assert will happen to alert that the CPU HOB is
    //not produced, so that all memory will not be paged.
    //ASSERT_PEI_ERROR(PeiServices, Status);

    //Find APIC ID Hob.
    if (!EFI_ERROR(Status)) {
        NumMemBits = CpuHob->SizeOfMemorySpace;
    }

    InitLongModeExt (PeiServices, Function, Parameter1, Parameter2, NumMemBits);
}

//*************************************************************************
//
// Name: ReadMsr
//
// Description:
//  UINT64 ReadMsr(IN UINT32 Msr) reads the CPU MSR index defined by Msr and
// returns the value.
//
// Input:
//  IN UINT32 Msr
// 32-bit MSR index to be read.
//
// Output:
//  UINT64 MSR value at MSR index, Msr.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//*************************************************************************
UINT64 ReadMsr(IN UINT32 Msr){
    _asm{
        mov	ecx, Msr                 ;MSR register
        rdmsr
    }
}

//*************************************************************************
//
// Name: WriteMsr
//
// Description:
//  VOID WriteMsr(IN UINT32 Msr, IN UINT64 Value) writes the Value to the
// supplied MSR index, Msr.
//
// Input:
//  IN UINT32 Msr
// 32-bit MSR index to be written to.
//
//  IN UINT64 Value
// Value to be written to MSR index.
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
//*************************************************************************
VOID WriteMsr(IN UINT32 Msr, IN UINT64 Value){
    _asm{
        mov ecx, Msr					;MSR register
        mov edx, dword ptr Value[4]     ;Upper 32 bit MSR Value
        mov eax, dword ptr Value[0]     ;Lower 32 bit MSR Value
        wrmsr
    }
}

//*************************************************************************
//
// Name: CPULib_CpuID
//
// Description:
//  VOID CPULib_CpuID(IN UINT32 CpuIDIndex, IN OUT UINT32 *pRegEAX,
// IN OUT UINT32 *pRegEBX, IN OUT UINT32 *pRegECX, IN OUT UINT32 *pRegEDX)
// issues the CPUID instruction with the index provided and returns the
// register values.
//
// Input:
//  IN UINT32 CpuIDIndex
// 32-bit CPUID index.
//
//  IN OUT UINT32 *pRegEAX
// Pointer to UINT32 for EAX return value.
//
//  IN OUT UINT32 *pRegEBX
// Pointer to UINT32 for EBX return value.
//
//  IN OUT UINT32 *pRegECX
// Pointer to UINT32 for ECX return value.
//
//  IN OUT UINT32 *pRegEDX
// Pointer to UINT32 for EDX return value.
//
// Output:
//  IN OUT UINT32 *pRegEAX
// Value of EAX after CPUID instruction.
//
//  IN OUT UINT32 *pRegEBX
// Value of EBX after CPUID instruction.
//
//  IN OUT UINT32 *pRegECX
// Value of ECX after CPUID instruction.
//
//  IN OUT UINT32 *pRegEDX
// Value of EDX after CPUID instruction.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//*************************************************************************
VOID CPULib_CpuID(
    IN UINT32 CpuIDIndex, 
    IN OUT UINT32 *pRegEAX, 
    IN OUT UINT32 *pRegEBX, 
    IN OUT UINT32 *pRegECX, 
    IN OUT UINT32 *pRegEDX)
{
    _asm{
        push ebx
        push ecx
        push edx
        push esi
        mov esi, pRegECX
        mov	ecx, [esi]
        mov eax, CpuIDIndex
        cpuid
        mov esi, pRegEAX
        or  esi, esi
        jz  skip1
        mov	[esi], eax
skip1:
        mov	esi, pRegEBX
        or  esi, esi
        jz  skip2
        mov	[esi], ebx
skip2:
        mov	esi, pRegECX
        or  esi, esi
        jz  skip3
        mov	[esi], ecx
skip3:
        mov	esi, pRegEDX
        or  esi, esi
        jz  skip4
        mov	[esi], edx
skip4:
        pop	esi
        pop	edx
        pop	ecx
        pop ebx
    }
}

//*************************************************************************
//
// Name: DisableCacheInCR0
//
// Description:
//  VOID DisableCacheInCR0(VOID) disables the CPU cache using the CR0 register.
//
// Input:
//  VOID.
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
//*************************************************************************
VOID DisableCacheInCR0(VOID){
    _asm{
        wbinvd
        mov	eax, cr0
        or	eax, 060000000h		;SET CD, NW
        mov	cr0, eax
        wbinvd					;Invalidate cache
    }
}

//*************************************************************************
//
// Name: EnableCacheInCR0
//
// Description:
//  VOID EnableCacheInCR0(VOID) enables the CPU cache using the CR0 register.
//
// Input:
//  VOID.
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
//*************************************************************************
VOID EnableCacheInCR0(VOID){
    _asm{
    // Enable cache
        mov	eax, cr0
        and	eax, 09fffffffh		;SET CD, NW
        mov	cr0, eax
        wbinvd
    }
}
//*************************************************************************
//
// Name: ReadCr3
//
// Description:
//  UINTN ReadCr3(VOID) reads the register CR3 and returns its value.
//
// Input:
//  VOID.
//
// Output:
//  Returns UINTN value stored in the CR3 register.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//*************************************************************************
UINTN ReadCr3(VOID){
    _asm{
        mov     eax, cr3
    }
}
//*************************************************************************
//
// Name: WriteCr3
//
// Description:
//  VOID WriteCr3(IN UINTN CR3) writes the provided value to the CR3 register.
//
// Input:
//  IN UINTN CR3
// Value to be written to the CR3 register.
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
//*************************************************************************
VOID WriteCr3(IN UINTN CR3){
    _asm{
        mov     eax, CR3 
        mov     cr3, eax
    }
}

//*************************************************************************
//
// Name: CPULib_EnableInterrupt
//
// Description:
//  VOID CPULib_EnableInterrupt(VOID) enables interrupts on the CPU.
//
// Input:
//  VOID.
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
//*************************************************************************
VOID CPULib_EnableInterrupt(VOID){
    _asm{
    // Enable Interrupt
        sti
    }
}

//*************************************************************************
//
// Name: CPULib_DisableInterrupt
//
// Description:
//  VOID CPULib_DisableInterrupt() disables interrupts on the CPU.
//
// Input:
//  VOID.
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
//*************************************************************************
VOID CPULib_DisableInterrupt(){
    _asm{
    // Disable Interrupt
        cli
    }
}

//*************************************************************************
//
// Name: CPULib_GetInterruptState
//
// Description:
//  BOOLEAN CPULib_GetInterruptState(VOID)returns the current CPU interrupt
// state.
//
// Input:
//  VOID.
//
// Output:
//  Returns FALSE if interrupts are disabled; otherwise TRUE.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//*************************************************************************
BOOLEAN CPULib_GetInterruptState(VOID){
    _asm{
        xor	bl, bl
        pushfd			; push flags onto stack.
        pop	eax		; eax = flags.
        bt	eax,9		; IF (bit 9) if set, set carry flag.
                                    ; Interrupts are allowed if IF is set.
        adc	bl, 0		; BL = IF = CF.

        mov	al, bl		; Return value
    }
}

//*************************************************************************
//
// Name: GetCsSegment
//
// Description:
//  UINT16 GetCsSegment(VOID) retreives the value of the CS register.
//
// Input:
//  VOID.
//
// Output:
//  Returns UINT16 value of CS.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//*************************************************************************
UINT16 GetCsSegment(VOID){
    _asm{
        mov ax, cs
    }
}

//*************************************************************************
//
// Name: ReadRtdsc
//
// Description:
//  UINT64 ReadRtdsc(VOID) retrieves the time stamp counter.
//
// Input:
//  VOID.
//
// Output:
//  Returns UINT64 value of the time stamp counter.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//*************************************************************************
UINT64 ReadRtdsc(VOID){
    _asm{
        rdtsc
    }
}

//*************************************************************************
//
// Name: WaitForever
//
// Description:
//  VOID WaitForever(VOID) performs an infinite loop which does nothing.
//
// Input:
//  VOID.
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
//*************************************************************************
VOID WaitForever(VOID){
    _asm{
    bbb:
        jmp	bbb
    }
}

//*************************************************************************
//
// Name: HltCpu
//
// Description:
//  VOID HltCpu(VOID) halts the CPU.
//
// Input:
//  VOID.
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
//*************************************************************************
VOID HltCpu(VOID){
    _asm{
    bbb:
        cli
        hlt
        jmp	bbb
    }
}

//*************************************************************************
//
// Name: CPULib_Pause
//
// Description:
//  VOID CPULib_Pause(VOID) performs the pause assembly instruction.
//
// Input:
//  VOID.
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
//*************************************************************************
VOID CPULib_Pause(VOID){
    _asm{
        pause
    }
}

//*************************************************************************
//
// Name: WaitForSemaphore
//
// Description:
//  VOID WaitForSemaphore(IN volatile UINT8 *Semaphore) waits for the
// semaphore to become available; once available, it claims the semaphore and
// returns.
//
// Input:
//  IN volatile UINT8 *Semaphore
// Pointer to the desired semaphore.
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
//*************************************************************************
VOID WaitForSemaphore(IN volatile UINT8 *Semaphore){
    _asm{
        push ebx
        mov	al, 1
        mov ebx, Semaphore
    bbb:
        xchg al, [ebx]
        or	al, al
        pause
        jnz	bbb
        pop	ebx
    }
}

//*************************************************************************
//
// Name: WaitUntilZero8
//
// Description:
//  VOID WaitUntilZero8(IN volatile UINT8 *Value) waits until the byte stored
// at Value becomes 0, then continues.
//
// Input:
//  IN volatile UINT8 *Value
// Address of the byte value to be monitored.
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
//*************************************************************************
VOID WaitUntilZero8(IN volatile UINT8 *Value){
    _asm{
        push ebx
        mov	ebx, Value
    bbb:
        mov al, [ebx]
        or	al, al
        pause
        jnz bbb
        pop	ebx
    }
}

//*************************************************************************
//
// Name: WaitUntilZero32
//
// Description:
//  VOID WaitUntilZero32(IN volatile UINT32 *Value) waits until the UINT32
// value stored at the Value address becomes 0, then continues.
//
// Input:
//  IN volatile UINT32 *Value
// Address of the UINT32 value to be monitored.
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
//*************************************************************************
VOID WaitUntilZero32(IN volatile UINT32 *Value) {
    _asm{
        push ebx
        mov	ebx, Value
    bbb:
        mov eax, [ebx]
        or	eax, eax
        pause
        jnz bbb
        pop	ebx
    }
}

//*************************************************************************
//
// Name: CPULib_LockByteInc
//
// Description:
//  VOID CPULib_LockByteInc(IN UINT8 *ptr) locks the next byte after the
// address pointed to by ptr.
//
// Input:
//  IN UINT8 *ptr// Address to the byte which preceeds the desired byte to be locked.
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
//*************************************************************************
VOID CPULib_LockByteInc(IN UINT8 *xptr){
    _asm{
        mov	eax, xptr
        lock inc byte ptr [eax]
    }
}

//*************************************************************************
//
// Name: CPULib_LockByteDec
//
// Description:
//  VOID CPULib_LockByteDec(IN UINT8 *ptr) locks the preceeding byte before
// the address pointed to by ptr.
//
// Input:
//  IN UINT8 *ptr
// Address to the byte which follows the desired byte to be locked.
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
//*************************************************************************
VOID CPULib_LockByteDec(IN UINT8 *xptr) {
    _asm{
        mov	eax, xptr
        lock dec byte ptr [eax]
    }
}

//*************************************************************************
//
// Name: CPULib_LoadGdt
//
// Description:
//  VOID CPULib_LoadGdt(IN VOID *ptr) loads the GDT at the location pointed to
// by ptr.
//
// Input:
//  IN VOID *ptr
// Address of the GDT to be loaded.
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
//*************************************************************************
VOID CPULib_LoadGdt(IN VOID *xptr) {
    _asm{
        mov	eax, xptr
        lgdt fword ptr [eax]
    }
}

//*************************************************************************
//
// Name: CPULib_SaveGdt
//
// Description:
//  VOID CPULib_SaveGdt(IN VOID *ptr) stores the loaded GDT at the location
// provided by ptr.
//
// Input:
//  IN VOID *ptr
// Address to save the GDT.  User is responsible for allocating the necessary
// memory resources.
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
//*************************************************************************
VOID CPULib_SaveGdt(IN VOID *xptr) {
    _asm{
        mov	eax, xptr
        sgdt fword ptr [eax]
    }
}

//*************************************************************************
//
// Name: CPULib_LoadIdt
//
// Description:
//  VOID CPULib_LoadIdt(IN VOID *ptr) loads the IDT at the location provided
// by ptr.
//
// Input:
//  IN VOID *ptr
// Address of the IDT to be loaded.
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
//*************************************************************************
VOID CPULib_LoadIdt(IN VOID *xptr) {
    _asm{
        mov	eax, xptr
        lidt fword ptr [eax]
    }
}

//*************************************************************************
//
// Name: CPULib_SaveIdt
//
// Description:
//  VOID CPULib_SaveIdt(IN VOID *ptr) stores the loaded IDT at the location
// provided by ptr.
//
// Input:
//  IN VOID *ptr
// Address to save the IDT.  User is responsible for allocating the necessary
// memory resources.
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
//*************************************************************************
VOID CPULib_SaveIdt(IN VOID *xptr) {
    _asm{
        mov	eax, xptr
        sidt fword ptr [eax]
    }
}

//*************************************************************************
//
// Name: MemRead32
//
// Description:
//  UINT32 MemRead32(IN UINT32 *Address) reads and returns the 32-bit value
// stored at the user provided address.
//
// Input:
//  IN UINT32 *Address
// Address to read 32-bits from.
//
// Output:
//  UINT32 value stored at Address.
//
// Modified:
//
// Referrals:
//
// Notes:
//
//*************************************************************************
UINT32 MemRead32(IN UINT32 *Address) {
    _asm{
        push esi
        mov esi, Address		;esi = address
        mov	eax, [esi]
        pop	esi
    }
}

//*************************************************************************
//
// Name: MemReadWrite32
//
// Description:
//  VOID MemReadWrite32(IN UINT32 *Address, IN UINT32 Value, IN UINT32 Mask)
// reads the 32-bit value stored at Address, ANDs it with Mask, ORs the result
// with Value, then writes the result back to Address.
//
// Input:
//  IN UINT32 *Address
// Address which shall be read from and subsequently written to.
//
//  IN UINT32 Value
// Value to be ORed with the value stored at Address after it has been ANDed
// with the provided Mask.
//
//  IN UINT32 Mask
// Mask to be ANDed with the original value stored at Address.
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
//*************************************************************************
VOID MemReadWrite32(
    IN UINT32 *Address, 
    IN UINT32 Value, 
    IN UINT32 Mask)
{
    _asm{
        push esi
        mov esi, Address		;esi = address
        mov	eax, [esi]
        and eax, Mask		;Mask
        or	eax, Value		;Value
        mov [esi], eax
        pop	esi
    }
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
