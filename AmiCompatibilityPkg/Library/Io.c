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
// $Header: /Alaska/SOURCE/Core/Library/IO.c 4     7/10/09 3:49p Felixp $
//
// $Revision: 4 $
//
// $Date: 7/10/09 3:49p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name: IO.c
//
// Description:	
//  Contains generic I/O read/write functions.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiDxeLib.h>

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: IO_Read_Write_Functions
//
// Description:
//  Generic I/O read/write functions.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiLib  IoRead       Read value(s) from an I/O address or an entire region.
// AmiLib  IoRead8      Read 8-bit value from an I/O address.
// AmiLib  IoRead16     Read 16-bit value from an I/O address.
// AmiLib  IoRead32     Read 32-bit value from an I/O address.
// AmiLib  IoReadWrite  Read value from an I/O address then write back a value.
// AmiLib  IoWrite      Write buffer to an I/O address or region.
// AmiLib  IoWrite8     Write 8-bit value to an I/O address.
// AmiLib  IoWrite16    Write 16-bit value to an I/O address.
// AmiLib  IoWrite32    Write 32-bit value to an I/O address.
// AmiLib  MemRead      Read value(s) from a memory mapped I/O address or an entire region.
// AmiLib  MemReadWrite Read value from a memory mapped I/O address then write back a value.
// AmiLib  MemWrite     Write buffer to an I/O address or region.
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
// Name: IoRead
//
// Description:	
//  EFI_STATUS IoRead(IN CPU_IO_WIDTH Width, IN UINT64 Address,
// IN UINTN Count, OUT VOID *Buffer) performs Count I/O reads of the defined
// Width at the Address and stores the result in Buffer.  After every I/O
// operation, Address and Buffer are updated according to the Width. See
// notes for details.
//
// Input:
//  IN CPU_IO_WIDTH Width
// Number of bits to read from I/O.  Supports at most 32-bits.
//
//  IN UINT64 Address
// I/O address to read from.
//
//  IN UINTN Count
// Number of reads to perform.
//
//  OUT VOID *Buffer
// Buffer where the read results will be stored.  User is responsible for 
// properly allocating the necessary memory resources.
//
// Output:
//  EFI_INVALID_PARAMETER, if Count = 0, Width > 32-bits, or the range of
//                         Address > 0xffff.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  IoRead8
//  IoRead16
//  IoRead32
// 
// Notes:
//  Address and Buffer are updated according to the Width as follows:
//   CpuIoWidthUint8 - Both Address and Buffer are incremented by 1.
//   CpuIoWidthUint16 - Both Address and Buffer are incremented by 2.
//   CpuIoWidthUint32 - Both Address and Buffer are incremented by 4.
//   CpuIoWidthFifoUint8 - Only Buffer is incremented by 1.
//   CpuIoWidthFifoUint16 - Only Buffer is incremented by 2.
//   CpuIoWidthFifoUint32 - Only Buffer is incremented by 4.
//   CpuIoWidthFillUint8 - Only Address is incremented by 1.
//   CpuIoWidthFillUint16 - Only Address is incremented by 2.
//   CpuIoWidthFillUint32 - Only Address is incremented by 4.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS IoRead(
	IN CPU_IO_WIDTH	Width,
	IN UINT64		Address,
	IN UINTN		Count,
	IN VOID			*Buffer
	)
{
	UINT8 ValueWidth = 1 << (Width & 3);
	UINT8 AddrInc, BuffInc;
	UINTN AddrCount;	

	AddrInc = BuffInc = ValueWidth;

	switch (Width & ~3)
	{
	case CpuIoWidthFifoUint8: AddrInc = 0; break;
	case CpuIoWidthFillUint8: BuffInc = 0; break;
	}

	AddrCount = (Count-1) * AddrInc + ValueWidth;


	//---Validate Inputs---
	if (!Count) return EFI_INVALID_PARAMETER;
	if (Address + AddrCount > 0xffff) return EFI_INVALID_PARAMETER;
	if ((Width & 3) > CpuIoWidthUint32) return EFI_INVALID_PARAMETER;


	while (Count--)
	{
		switch(Width & 3)
		{
		case CpuIoWidthUint8:
			*(UINT8*)Buffer = IoRead8((UINT16)Address);
			break;
		case CpuIoWidthUint16:
			*(UINT16*)Buffer = IoRead16((UINT16)Address);
			break;
		default:
			*(UINT32*)Buffer = IoRead32((UINT16)Address);
		}
		Address = Address + AddrInc;
		Buffer  = (UINT8*) Buffer + BuffInc;
	}

	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: IoWrite
//
// Description:	
//  EFI_STATUS IoWrite(IN CPU_IO_WIDTH Width, IN UINT64 Address,
// IN UINTN Count, IN VOID *Buffer) performs Count I/O writes of the defined
// Width at the Address from the Buffer.  After every I/O operation, Address
// and Buffer are updated according to the Width.  See notes for details.
//
// Input:
//  IN CPU_IO_WIDTH Width
// Number of bits to read from I/O.  Supports at most 32-bits.
//
//  IN UINT64 Address
// I/O address to write to.
//
//  IN UINTN Count
// Number of writes to perform.
//
//  IN VOID *Buffer
// Buffer where the data to be written is located.
//
// Output:
//  EFI_INVALID_PARAMETER, if Count = 0, Width > 32-bits, or the range of
//                         Address > 0xffff.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  IoWrite8
//  IoWrite16
//  IoWrite32
// 
// Notes:
//  Address and Buffer are updated according to the Width as follows:
//   CpuIoWidthUint8 - Both Address and Buffer are incremented by 1.
//   CpuIoWidthUint16 - Both Address and Buffer are incremented by 2.
//   CpuIoWidthUint32 - Both Address and Buffer are incremented by 4.
//   CpuIoWidthFifoUint8 - Only Buffer is incremented by 1.
//   CpuIoWidthFifoUint16 - Only Buffer is incremented by 2.
//   CpuIoWidthFifoUint32 - Only Buffer is incremented by 4.
//   CpuIoWidthFillUint8 - Only Address is incremented by 1.
//   CpuIoWidthFillUint16 - Only Address is incremented by 2.
//   CpuIoWidthFillUint32 - Only Address is incremented by 4.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS IoWrite(
	IN CPU_IO_WIDTH	Width,
	IN UINT64		Address,
	IN UINTN		Count,
	OUT VOID		*Buffer
	)
{
	UINT8 ValueWidth = 1 << (Width & 3);
	UINT8 AddrInc, BuffInc;
	UINTN AddrCount;	

	AddrInc = BuffInc = ValueWidth;

	switch (Width & ~3)
	{
	case CpuIoWidthFifoUint8: AddrInc = 0; break;
	case CpuIoWidthFillUint8: BuffInc = 0; break;
	}

	AddrCount = (Count-1) * AddrInc + ValueWidth;


	//---Validate Inputs---
	if (!Count) return EFI_INVALID_PARAMETER;
	if (Address + AddrCount > 0xffff) return EFI_INVALID_PARAMETER;
	if ((Width & 3) > CpuIoWidthUint32) return EFI_INVALID_PARAMETER;


	while (Count--)
	{
		switch(Width & 3)
		{
		case CpuIoWidthUint8:
			IoWrite8((UINT16)Address, *(UINT8*) Buffer);
			break;
		case CpuIoWidthUint16:
			IoWrite16((UINT16)Address, *(UINT16*) Buffer);
			break;
		default:
			IoWrite32((UINT16)Address, *(UINT32*) Buffer);
		}
		Address = Address + AddrInc;
		Buffer  = (UINT8*) Buffer + BuffInc;
	}

	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: IoReadWrite
//
// Description:	
//  EFI_STATUS IoReadWrite(IN CPU_IO_WIDTH Width, IN UINT64 Address,
// IN VOID *DataValue, IN VOID *DataMask) reads the specified I/O Address,
// ANDs the result with DataMask, then ORs the modified result with DataValue
// and writes the value back to Address.
//
// Input:
//  IN CPU_IO_WIDTH Width
// Number of bits to read/write from I/O.  Only supports: CpuIoWidthUint8,
// CpuIoWidthUint16, and CpuIoWidthUint32.
//
//  IN UINT64 Address
// I/O address to read and write to.
//
//  IN VOID *DataValue
// UINT32 OR mask value.
//
//  IN VOID *DataMask
// UINT32 AND mask value.
//
// Output:
//  EFI_INVALID_PARAMETER, if Width not supported, or the range of
//                         Address > 0xffff.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  IoWrite
//  IoRead
// 
// Notes:
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS IoReadWrite(
	IN CPU_IO_WIDTH	Width,
	IN UINT64		Address,
	IN VOID			*DataValue,
	IN VOID			*DataMask
	)
{
	EFI_STATUS	Status;
	UINT32		Value32;
	UINT32		Mask;
	UINT32		Data;

#if defined(EFI64) || defined(EFIx64)
	//In case of aligment issues.
	Mask = *((UINT8*)DataMask + 3) + *((UINT8*)DataMask + 2) + *((UINT8*)DataMask + 1) + *(UINT8*)DataMask;
	Data = *((UINT8*)DataValue + 3) + *((UINT8*)DataValue + 2) + *((UINT8*)DataValue + 1) + *(UINT8*)DataValue;
	/////////////////////////////
#else
	Mask = *(UINT32*)DataMask;
	Data = *(UINT32*)DataValue;
#endif

	if ((Width & ~3) != CpuIoWidthUint8) return EFI_INVALID_PARAMETER; //Only CpuIoWidthUintxx Supported


	if ((Status = IoRead(Width, Address, 1, &Value32)) != EFI_SUCCESS) return Status;
	Value32 &= Mask;
	Value32 |= Data;
	return IoWrite(Width,Address,1,&Value32);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MemRead
//
// Description:	
//  EFI_STATUS MemRead(IN CPU_IO_WIDTH Width, IN UINT64 Address,
// IN UINTN Count, IN VOID *Buffer) performs Count MMIO reads of the size
// Width at the Address and stores the result in Buffer. After every MMIO
// operation, Address and Buffer are updated according to Width.
//
// Input:
//  IN CPU_IO_WIDTH Width
// The width of the access.
//
//  IN UINT64 Address
// The physical address of the access.
//
//  IN UINTN Count
// The number of accesses to perform.
//
//  IN VOID *Buffer
// A pointer to the buffer of data.  User is responsible for allocating the
// necessary memory resources.
//
// Output:
//  EFI_INVALID_PARAMETER, if Count = 0, Width not supported, or the range of
//                         Address overflows the bounds of memory.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:
//  Address and Buffer are updated according to Width as follows:
//   CpuIoWidthUint8 - Both Address and Buffer are incremented by 1.
//   CpuIoWidthUint16 - Both Address and Buffer are incremented by 2.
//   CpuIoWidthUint32 - Both Address and Buffer are incremented by 4.
//   CpuIoWidthUint64 - Both Address and Buffer are incremented by 8.
//   CpuIoWidthFifoUint8 - Only Buffer is incremented by 1.
//   CpuIoWidthFifoUint16 - Only Buffer is incremented by 2.
//   CpuIoWidthFifoUint32 - Only Buffer is incremented by 4.
//   CpuIoWidthFifoUint64 - Only Buffer is incremented by 8.
//   CpuIoWidthFillUint8 - Only Address is incremented by 1.
//   CpuIoWidthFillUint16 - Only Address is incremented by 2.
//   CpuIoWidthFillUint32 - Only Address is incremented by 4.
//   CpuIoWidthFillUint64 - Only Address is incremented by 8.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS MemRead(
	IN CPU_IO_WIDTH	Width,
	IN UINT64		Address,
	IN UINTN		Count,
	IN VOID			*Buffer
	)
{
	UINT8 ValueWidth = 1 << (Width & 3);
	UINT8 AddrInc, BuffInc;
	UINTN AddrCount;	

	AddrInc = BuffInc = ValueWidth;

	switch (Width & ~3)
	{
	case CpuIoWidthFifoUint8: AddrInc = 0; break;
	case CpuIoWidthFillUint8: BuffInc = 0; break;
	}

	AddrCount = (Count-1) * AddrInc + ValueWidth;

	//---Validate Inputs---
	if (!Count) return EFI_INVALID_PARAMETER;
	
#if defined(EFI64) || defined(EFIx64)
	if (Address + AddrCount < Address)  return EFI_INVALID_PARAMETER;	//Overflow
#else
	if (Address + AddrCount > 0xffffffff) return EFI_INVALID_PARAMETER;
	if ((Width & 3) > CpuIoWidthUint32) return EFI_INVALID_PARAMETER;
#endif


	while (Count--)
	{
		switch(Width & 3)
		{
		case CpuIoWidthUint8:
			*(UINT8*)Buffer = *(UINT8*)Address;
			break;
		case CpuIoWidthUint16:
			*(UINT16*)Buffer = *(UINT16*)Address;
			break;
		case CpuIoWidthUint32:
			*(UINT32*)Buffer = *(UINT32*)Address;
			break;
		default:
			*(UINT64*)Buffer = *(UINT64*)Address;
		}
		Address = Address + AddrInc;
		Buffer  = (UINT8*) Buffer + BuffInc;
	}

	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MemWrite
//
// Description:	
//  EFI_STATUS MemWrite(IN CPU_IO_WIDTH Width, IN UINT64 Address,
// IN UINTN Count, OUT VOID *Buffer) performs Count MMIO writes of the size
// Width at the Address using the contents of Buffer. After every MMIO
// operation, Address and Buffer are updated according to Width.
//
// Input:
//  IN CPU_IO_WIDTH Width
// The width of the access.
//
//  IN UINT64 Address
// The physical address of the access.
//
//  IN UINTN Count
// The number of accesses to perform.
//
//  OUT VOID *Buffer
// A pointer to the buffer of data. 
//
// Output:
//  EFI_INVALID_PARAMETER, if Count = 0, Width not supported, or the range of
//                         Address overflows the bounds of memory.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
// 
// Notes:
//  Address and Buffer are updated according to Width as follows:
//   CpuIoWidthUint8 - Both Address and Buffer are incremented by 1.
//   CpuIoWidthUint16 - Both Address and Buffer are incremented by 2.
//   CpuIoWidthUint32 - Both Address and Buffer are incremented by 4.
//   CpuIoWidthUint64 - Both Address and Buffer are incremented by 8.
//   CpuIoWidthFifoUint8 - Only Buffer is incremented by 1.
//   CpuIoWidthFifoUint16 - Only Buffer is incremented by 2.
//   CpuIoWidthFifoUint32 - Only Buffer is incremented by 4.
//   CpuIoWidthFifoUint64 - Only Buffer is incremented by 8.
//   CpuIoWidthFillUint8 - Only Address is incremented by 1.
//   CpuIoWidthFillUint16 - Only Address is incremented by 2.
//   CpuIoWidthFillUint32 - Only Address is incremented by 4.
//   CpuIoWidthFillUint64 - Only Address is incremented by 8.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS MemWrite(
	IN CPU_IO_WIDTH	Width,
	IN UINT64		Address,
	IN UINTN		Count,
	OUT VOID		*Buffer
	)
{
	UINT8 ValueWidth = 1 << (Width & 3);
	UINT8 AddrInc, BuffInc;
	UINTN AddrCount;	

	AddrInc = BuffInc = ValueWidth;

	switch (Width & ~3)
	{
	case CpuIoWidthFifoUint8: AddrInc = 0; break;
	case CpuIoWidthFillUint8: BuffInc = 0; break;
	}

	AddrCount = (Count-1) * AddrInc + ValueWidth;

	//---Validate Inputs---
	if (!Count) return EFI_INVALID_PARAMETER;
	
#if defined(EFI64) || defined(EFIx64)
	if (Address + AddrCount < Address)  return EFI_INVALID_PARAMETER;	//Overflow
#else
	if (Address + AddrCount > 0xffffffff) return EFI_INVALID_PARAMETER;
	if ((Width & 3) > CpuIoWidthUint32) return EFI_INVALID_PARAMETER;
#endif


	while (Count--)
	{
		switch(Width & 3)
		{
		case CpuIoWidthUint8:
			*(UINT8*)Address = *(UINT8*)Buffer;
			break;
		case CpuIoWidthUint16:
			*(UINT16*)Address = *(UINT16*)Buffer;
			break;
		case CpuIoWidthUint32:
			*(UINT32*)Address = *(UINT32*) Buffer;
			break;
		default:
			*(UINT64*)Address = *(UINT64*) Buffer;
		}
		Address = Address + AddrInc;
		Buffer  = (UINT8*) Buffer + BuffInc;
	}

	return EFI_SUCCESS;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MemReadWrite
//
// Description:	
//  EFI_STATUS MemReadWrite(IN CPU_IO_WIDTH Width, IN UINT64 Address,
// IN VOID *DataValue, IN VOID *DataMask) reads the specified physical
// Address, ANDs the result with DataMask, then ORs the modified result with 
// DataValue and writes the value back to Address.
//
// Input:
//  IN CPU_IO_WIDTH Width
// The width of the access.  Only supports CpuIoWidthUintxx.
//
//  IN UINT64 Address
// The physical address of the access.
//
//  IN VOID *DataValue
// UINT64 OR mask value.
//
//  IN VOID *DataMask
// UINT64 AND mask value.
//
// Output:
//  EFI_INVALID_PARAMETER, if Width not supported, or the range of
//                         Address overflows the bounds of memory.
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
EFI_STATUS MemReadWrite(
	IN CPU_IO_WIDTH	Width,
	IN UINT64		Address,
	IN VOID			*DataValue,
	IN VOID			*DataMask
	)
{
	EFI_STATUS	Status;
	UINT64		Value64;
	UINT64		Mask;
	UINT64		Data;

#if defined(EFI64) || defined(EFIx64)
	//In case of aligment issues.
	Mask = *((UINT8*)DataMask + 4) + *((UINT8*)DataMask + 3) + *((UINT8*)DataMask + 2) + *((UINT8*)DataMask + 1) + *(UINT8*)DataMask;
	Data = *((UINT8*)DataValue + 4) + *((UINT8*)DataValue + 3) + *((UINT8*)DataValue + 2) + *((UINT8*)DataValue + 1) + *(UINT8*)DataValue;
	/////////////////////////////
#else
	Mask = *(UINT64*)DataMask;
	Data = *(UINT64*)DataValue;
#endif

	if ((Width & ~3) != CpuIoWidthUint8) return EFI_INVALID_PARAMETER; //Only CpuIoWidthUintxx Supported

	if ((Status = MemRead(Width, Address, 1, &Value64)) != EFI_SUCCESS) return Status;
	Value64 &= Mask;
	Value64 |= Data;
	return MemWrite(Width,Address,1,&Value64);
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
