//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/PostCodeLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <PiPei.h>

INTN MemCmp(VOID* pDestination, VOID* pSource, UINTN Count){
	return CompareMem(pDestination, pSource, Count);
}

VOID MemCpy(VOID* pDestination, VOID* pSource, UINTN Length){
	CopyMem(pDestination, pSource, Length);
}

VOID MemSet(VOID* pBuffer, UINTN Size, UINT8 Value){
	SetMem ( pBuffer, Size, Value);
}

UINT64 Shr64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return RShiftU64 (Value, Shift);
}

UINT64 Shl64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return LShiftU64 (Value, Shift);
}

UINT64 Div64 (
	IN UINT64	Dividend,
	IN UINTN	Divisor,	//Can only be 31 bits.
  	OUT UINTN	*Remainder OPTIONAL
  	)
{
	UINT64 Remainder64;
	UINT64 Result = DivU64x64Remainder (Dividend, Divisor, &Remainder64);
	if (Remainder != NULL) *Remainder = (UINTN)Remainder64;
	return Result;
}

UINT64 Mul64(
    IN UINT64   Value64,
    IN UINTN	Value32
    )
{
	return MultU64x64(Value64, Value32);
}

VOID checkpoint(UINT8 c){
	PostCode(c);
}

VOID CPULib_EnableInterrupt(VOID){
	EnableInterrupts();
}

VOID CPULib_DisableInterrupt(){
	DisableInterrupts();
}

BOOLEAN CPULib_GetInterruptState(VOID){
	return GetInterruptState();
}

UINT32 MemRead32(IN UINT32 *Address) {
	return MmioRead32 ((UINTN)Address);
}

VOID MemReadWrite32(
    IN UINT32 *Address, 
    IN UINT32 Value, 
    IN UINT32 Mask)
{
	MmioAndThenOr32( (UINTN)Address, Mask, Value);
}

VOID MemCpy32(VOID* pDestination, VOID* pSource, UINTN Length){
	MmioReadBuffer32 ((UINTN)pSource, Length, pDestination);
}

#if defined (MDE_CPU_IA32) || defined (MDE_CPU_X64)
UINT64 GetCpuTimer(){
	return AsmReadTsc();
}

//Unsupported
VOID InitLongModeExt(
	IN CONST EFI_PEI_SERVICES **PeiServices,
	IN VOID *Function,
	IN VOID *Parameter1,
	IN VOID *Parameter2,
    IN UINT8 NumMemBits
);
//Unsupported
VOID InitLongMode(
	IN CONST EFI_PEI_SERVICES **PeiServices,
	IN VOID *Function,
	IN VOID *Parameter1,
	IN VOID *Parameter2
);

UINT64 ReadMsr(IN UINT32 Msr){
	return AsmReadMsr64(Msr);
}

VOID WriteMsr(IN UINT32 Msr, IN UINT64 Value){
	AsmWriteMsr64(Msr,Value);
}

VOID CPULib_CpuID(
    IN UINT32 CpuIDIndex, 
    IN OUT UINT32 *pRegEAX, 
    IN OUT UINT32 *pRegEBX, 
    IN OUT UINT32 *pRegECX, 
    IN OUT UINT32 *pRegEDX)
{
	AsmCpuidEx(CpuIDIndex, *pRegECX, pRegEAX, pRegEBX, pRegECX, pRegEDX);
}

VOID DisableCacheInCR0(VOID){
	AsmDisableCache();
}

VOID EnableCacheInCR0(VOID){
	AsmEnableCache();
}

UINTN ReadCr3(VOID){
	return AsmReadCr3();
}

VOID WriteCr3(IN UINTN CR3){
	AsmWriteCr3(CR3);
}

UINT16 GetCsSegment(VOID){
	return AsmReadCs();
}

UINT64 ReadRtdsc(VOID){
	return AsmReadTsc();
}

//Unsupported
VOID WaitForever(VOID);

//Unsupported
VOID HltCpu(VOID);

//Unsupported
VOID CPULib_Pause(VOID);

//Unsupported
VOID WaitForSemaphore(IN volatile UINT8 *Semaphore);

//Unsupported
VOID WaitUntilZero8(IN volatile UINT8 *Value);

//Unsupported
VOID WaitUntilZero32(IN volatile UINT32 *Value);

//Unsupported
VOID CPULib_LockByteInc(IN UINT8 *xptr);

//Unsupported
VOID CPULib_LockByteDec(IN UINT8 *xptr);

VOID CPULib_LoadGdt(IN VOID *xptr) {
	AsmWriteGdtr(xptr);
}

VOID CPULib_SaveGdt(IN VOID *xptr) {
	AsmReadGdtr(xptr);
}

VOID CPULib_LoadIdt(IN VOID *xptr) {
	AsmWriteIdtr(xptr);
}

VOID CPULib_SaveIdt(IN VOID *xptr) {
	AsmReadIdtr(xptr);
}

//Unsupported
VOID EnableLongMode(VOID *PageTable, VOID *Function, VOID *Parameter1, VOID *Parameter2);
//Unsupported
VOID EnableMachineCheck(VOID);

VOID* CPULIB_GetPageTable(){
	return (VOID*)AsmReadCr3();
}
		
//Unsupported
UINT32 GetPageTableNumPages(
    IN UINT8 NumberMemoryBits
);
//Unsupported
VOID FillPageTable(
    IN UINT8  NumberMemoryBits,
    IN VOID   *PageTable
);

#endif
#if defined (MDE_CPU_ARM) || defined (MDE_CPU_AARCH64)
UINT64 GetCpuTimer(){
	return GetPerformanceCounter();
}
#endif
#if defined (MDE_CPU_AARCH64)
// CompilerIntrinsicsLib defines memset for ARM, but not for AATCH64. Let's define the function here.
VOID memset(VOID* pBuffer, UINTN Value, UINTN Count){
  SetMem(pBuffer, Count, Value);
}
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
