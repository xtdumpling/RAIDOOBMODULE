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
// $Header: /Alaska/SOURCE/Core/Modules/x64Core/x64CLib.c 4     7/10/09 9:30a Felixp $
//
// $Revision: 4 $
//
// $Date: 7/10/09 9:30a $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: x64CLib.c
//
// Description:
//  Generic CPU library functions for the x64 architecture.
//
//<AMI_FHDR_END>
//*************************************************************************

#include <Efi.h>

//*************************************************************************
// Math
//*************************************************************************

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Math_64_Bit_Functions
//
// Description:
//  Math functions involving 64-bit input parameters designed to be usable on
// both 32-bit and 64-bit platforms.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiLib  Shr64  Shift 64-bit value right.
// AmiLib  Shl64  Shift 64-bit value left.
// AmiLib  Div64  Divide 64-bit value by a 31-bit value.
// AmiLib  Mul64  Multiply 64-bit value by a 32-bit value.
// AmiLib  GetPowerOfTwo64 Determine the highest set bit in a 64-bit value.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions.  Append .h to the given name.
//  
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: CPU_Functions
//
// Description:
//  CPU related functions defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// None   CPULib_CpuID             Perform the cpuid assembly instruction.
// None   CPULib_DisableInterrupt  Disable interrupts.
// None   CPULib_EnableInterrupt   Enable interrupts.
// None   CPULib_GetInterruptState Get current interrupt state (enabled or disabled).
// None   CPULIB_GetPageTable      Retrieve address of the page table.
// None   CPULib_LoadGdt           Load GDT.
// None   CPULib_LoadIdt           Load IDT.
// None   CPULib_LockByteDec       Lock preceeding byte.
// None   CPULib_LockByteInc       Lock following byte.
// None   CPULib_Pause             Pause CPU.
// None   CPULib_SaveGdt           Save GDT at provided location.
// None   CPULib_SaveIdt           Save IDT at provided location.
// None   DisableCacheInCR0        Disable CPU cache using CR0.
// None   EnableCacheInCR0         Enable CPU cache using CR0.
// None   EnableMachineCheck       Enable machine check exception bit in CR4.
// AmiLib GetCpuTimer              Retrieve the CPU timer's value.
// None   GetCsSegment             Retrieve the code segment.
// None   HltCpu                   Halt the CPU.
// None   WaitForever              Perform infinite loop.
// None   WaitForSemaphore         Wait for semaphore to become available. then take control.
// None   WaitUntilZero32          Wait until a 32-bit memory region becomes zero.
// None   WaitUntilZero8           Wait until an 8-bit meomry region becomes zero.
// None   WriteMsr                 Write a value to a MSR.
// None   ReadMsr                  Read value from a MSR.
// AmiLib WriteCr3                 Write a value to CR3.
// AmiLib ReadCr3                  Read value from CR3.
// None   ReadRtdsc                Retrieve the time stamp counter. 
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions.  Append .h to the given name.
//  
//<AMI_GHDR_END>
//*************************************************************************


//*************************************************************************
//<AMI_PHDR_START>
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
//<AMI_PHDR_END>
//*************************************************************************
UINT64 Shr64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return Value>>Shift;
}

//*************************************************************************
//<AMI_PHDR_START>
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
//<AMI_PHDR_END>
//*************************************************************************
UINT64 Shl64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return Value<<Shift;
}

//*************************************************************************
//<AMI_PHDR_START>
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
//<AMI_PHDR_END>
//*************************************************************************
UINT64 Div64 (
	IN UINT64	Dividend,
	IN UINTN	Divisor,	//Can only be 31 bits.
  	OUT UINTN	*Remainder OPTIONAL
  	)
{
	UINT64 	Result = Dividend/Divisor;
	if (Remainder) *Remainder=Dividend%Divisor;
  	return Result;
}

//*************************************************************************
//<AMI_PHDR_START>
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
//<AMI_PHDR_END>
//*************************************************************************
UINT64 Mul64(
    IN UINT64   Value64,
    IN UINTN	Value32
    )
{
	return Value64*Value32;
}

// The memset function has been moved to MemSet.asm
// Compiler wouldn't let us define this intrinsic functions, yet,
// if it is not definedwe are getting an unresolved external error.

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
