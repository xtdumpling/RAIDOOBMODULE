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

/** @file
   The implementation of SMM PCI Root Bridge Io Protocol.

   PCI Root Bridge I/O protocol is used by PCI Bus Driver to perform PCI Memory, PCI I/O, 
   and PCI Configuration cycles on a PCI Root Bridge. It also provides services to perform 
   defferent types of bus mastering DMA
    
**/

//
// Include common header file for this module.
//
#include "SmmPciRbIo.h"
#include <Token.h>
#include <PciBus.h>
#include <Library/PciAccessCspLib.h>
#include <Library/AmiPciBusLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/AcpiTable.h>


///
/// AMI SMM Pci Root Bridge Io Module Variables
///

EFI_SMM_SYSTEM_TABLE2           *gSmst2             = NULL;
EFI_SMM_BASE2_PROTOCOL          *gSmmBase           = NULL ;

///Use array to hold all SMM_RB_IO instances in case of Multiple Root system.
AMI_SMM_PCI_RB_DATA             *gAmiSmmRbArray     = NULL;  

AMI_PCI_SMM_HANDOFF_PROTOCOL    *gPciHandoffProtocol=NULL;

UINTN                           gSmmRbIoCnt=0;

EFI_SMM_CPU_IO2_PROTOCOL        *mCpuIo2=NULL;




/**
   Polls an address in memory mapped I/O space until an exit condition is met, or 
   a timeout occurs. 

   This function provides a standard way to poll a PCI memory location. A PCI memory read
   operation is performed at the PCI memory address specified by Address for the width specified
   by Width. The result of this PCI memory read operation is stored in Result. This PCI memory
   read operation is repeated until either a timeout of Delay 100 ns units has expired, or (Result &
   Mask) is equal to Value.

   @param[in]   This      A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The base address of the memory operations. The caller is
                          responsible for aligning Address if required.
   @param[in]   Mask      Mask used for the polling criteria. Bytes above Width in Mask
                          are ignored. The bits in the bytes below Width which are zero in
                          Mask are ignored when polling the memory address.
   @param[in]   Value     The comparison value used for the polling exit criteria.
   @param[in]   Delay     The number of 100 ns units to poll. Note that timer available may
                          be of poorer granularity.
   @param[out]  Result    Pointer to the last value read from the memory location.
   
   @retval EFI_SUCCESS            The last data returned from the access matched the poll exit criteria.
   @retval EFI_INVALID_PARAMETER  Width is invalid.
   @retval EFI_INVALID_PARAMETER  Result is NULL.
   @retval EFI_TIMEOUT            Delay expired before a match occurred.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoPollMem ( 
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  )
{
  EFI_STATUS  Status;
  UINT64      NumberOfTicks;
  UINTN       Remainder;
  volatile UINTN    t;
  AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
  if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;
  if (Result == NULL) return EFI_INVALID_PARAMETER;
  if (Width < 0 || Width > EfiPciWidthUint64) return EFI_INVALID_PARAMETER;
  
  // No matter what, always do a single poll.
  Status = This->Mem.Read (This, Width, Address, 1, Result);
  if (EFI_ERROR (Status)) return Status;
  
  if ((*Result & Mask) == Value) return EFI_SUCCESS;
  
  if (Delay == 0) return EFI_SUCCESS;
  else {

    // Assume a 30 uS delay (300, 100ns units) for each poll itteration
    NumberOfTicks = Div64(Delay, 300, &Remainder);
    if (Remainder != 0) {
      NumberOfTicks++;
    }
    NumberOfTicks++; 
  
    while (NumberOfTicks) {

      for (t=0;t<NumberOfTicks;t++); //MicroSecondDelay (30);
    
      Status = This->Mem.Read (This, Width, Address, 1, Result);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    
      if ((*Result & Mask) == Value) {
        return EFI_SUCCESS;
      }

      NumberOfTicks--;
    }

  }

  return EFI_TIMEOUT;
}

/**
   Reads from the I/O space of a PCI Root Bridge. Returns when either the polling exit criteria is
   satisfied or after a defined duration.

   This function provides a standard way to poll a PCI I/O location. A PCI I/O read operation is
   performed at the PCI I/O address specified by Address for the width specified by Width.
   The result of this PCI I/O read operation is stored in Result. This PCI I/O read operation is
   repeated until either a timeout of Delay 100 ns units has expired, or (Result & Mask) is equal
   to Value.

   @param[in] This      A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in] Width     Signifies the width of the I/O operations.
   @param[in] Address   The base address of the I/O operations. The caller is responsible
                        for aligning Address if required.
   @param[in] Mask      Mask used for the polling criteria. Bytes above Width in Mask
                        are ignored. The bits in the bytes below Width which are zero in
                        Mask are ignored when polling the I/O address.
   @param[in] Value     The comparison value used for the polling exit criteria.
   @param[in] Delay     The number of 100 ns units to poll. Note that timer available may
                        be of poorer granularity.
   @param[out] Result   Pointer to the last value read from the memory location.
   
   @retval EFI_SUCCESS            The last data returned from the access matched the poll exit criteria.
   @retval EFI_INVALID_PARAMETER  Width is invalid.
   @retval EFI_INVALID_PARAMETER  Result is NULL.
   @retval EFI_TIMEOUT            Delay expired before a match occurred.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoPollIo ( 
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  )
{
  EFI_STATUS  Status;
  UINT64      NumberOfTicks;
  UINTN       Remainder;
  volatile UINTN    t;
  AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
  if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;
  if (Result == NULL) return EFI_INVALID_PARAMETER;
  if (Width < 0 || Width > EfiPciWidthUint64) return EFI_INVALID_PARAMETER;
  
  
  // No matter what, always do a single poll.
  Status = This->Io.Read (This, Width, Address, 1, Result);
  if (EFI_ERROR (Status)) {
    return Status;
  }    
  if ((*Result & Mask) == Value) {
    return EFI_SUCCESS;
  }

  if (Delay == 0) {
    return EFI_SUCCESS;
  
  } else {

    //
    // Assume a 30 uS delay (300, 100ns units) for each poll itteration
    //
    NumberOfTicks = Div64(Delay, 300, &Remainder);
    if (Remainder != 0) {
      NumberOfTicks++;
    }
    NumberOfTicks++;
  
    while (NumberOfTicks) {

      for (t=0;t<NumberOfTicks;t++); //MicroSecondDelay (30);
    
      Status = This->Io.Read (This, Width, Address, 1, Result);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    
      if ((*Result & Mask) == Value) {
        return EFI_SUCCESS;
      }

      NumberOfTicks--;
    }
  }
  return EFI_TIMEOUT;
}

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

   The Mem.Read(), and Mem.Write() functions enable a driver to access PCI controller
   registers in the PCI root bridge memory space.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI Root Bridge on a platform might require.

   @param[in]   This      A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operation.
   @param[in]   Address   The base address of the memory operation. The caller is
                          responsible for aligning the Address if required.
   @param[in]   Count     The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoMemRead (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
    UINTN   IncrementType       = Width & ~3;
    UINT8   IncrementValue      = 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
    UINT8   IncrementBuffer     = 0;
    UINT8   IncrementAddress    = 0;
    AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
    if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;
    if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
    if (IncrementType > 16 || IncrementValue > 8) return EFI_INVALID_PARAMETER;

    //if FIFO Address falls out of IO Range
    if(sizeof(UINTN)==4){
        if (Address  > 0xffffffff) return EFI_INVALID_PARAMETER;    
    }
    
    switch (IncrementType)
    {
    case 0: //EfiPciWidthUintxx
        IncrementAddress = IncrementBuffer = IncrementValue;
        break;
    case 4: //EfiPciWidthFifoUintxx
        IncrementBuffer = IncrementValue;
        break;
    default: //EfiPciWidthFillUintxx
        IncrementAddress = IncrementValue;
    }

    //Exclude FIFO operation from address increase check
    if(IncrementAddress) {
        if(sizeof(UINTN)==8){
            if (( 0xffffffffffffffff - Count * IncrementValue) <= Address)
                return EFI_INVALID_PARAMETER;                       
        } else {
            if ((Address + Count * IncrementValue) > 0xffffffff)
                return EFI_INVALID_PARAMETER;                       
        }
    }


    
    return mCpuIo2->Mem.Read (
                       mCpuIo2, 
                       (EFI_SMM_IO_WIDTH) IncrementType, 
                       Address, 
                       Count, 
                       Buffer
                       );
}

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

   The Mem.Read(), and Mem.Write() functions enable a driver to access PCI controller
   registers in the PCI root bridge memory space.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI Root Bridge on a platform might require.

   @param[in]   This      A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operation.
   @param[in]   Address   The base address of the memory operation. The caller is
                          responsible for aligning the Address if required.
   @param[in]   Count     The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
**/
EFI_STATUS
EFIAPI SmmRbIoMemWrite (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64       Address,
  IN     UINTN        Count,
  IN OUT VOID         *Buffer)
  {
      UINTN   IncrementType       = Width & ~3;
      UINT8   IncrementValue      = 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
      UINT8   IncrementBuffer     = 0;
      UINT8   IncrementAddress    = 0;
      AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
  //--------------------------------------------
      if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;

      if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
      if (IncrementType > 16 || IncrementValue > 8) return EFI_INVALID_PARAMETER;

      //if FIFO Address falls out of IO Range
      if(sizeof(UINTN)==4){
          if (Address  > 0xffffffff) return EFI_INVALID_PARAMETER;    
      }
      
      switch (IncrementType)
      {
      case 0: //EfiPciWidthUintxx
          IncrementAddress = IncrementBuffer = IncrementValue;
          break;
      case 4: //EfiPciWidthFifoUintxx
          IncrementBuffer = IncrementValue;
          break;
      default: //EfiPciWidthFillUintxx
          IncrementAddress = IncrementValue;
      }

      //Exclude FIFO operation from buffer increase check
      if(IncrementAddress) {
          if(sizeof(UINTN)==8){
              if (( 0xffffffffffffffff - Count * IncrementValue) <= Address)
                  return EFI_INVALID_PARAMETER;                       
          } else {
              if ((Address + Count * IncrementValue) > 0xffffffff)
                  return EFI_INVALID_PARAMETER;                       
          }
      }

      return mCpuIo2->Mem.Write (mCpuIo2, (EFI_SMM_IO_WIDTH) IncrementType, 
                       Address, Count, Buffer);
}

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

   @param[in]   This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width       Signifies the width of the memory operations.
   @param[in]   Address     The base address of the I/O operation. The caller is responsible for
                            aligning the Address if required.
   @param[in]   Count       The number of I/O operations to perform. Bytes moved is Width
                            size * Count, starting at Address.
   @param[out]  Buffer      For read operations, the destination buffer to store the results. For
                            write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS              The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoIoRead (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64       Address,
  IN     UINTN        Count,
  IN OUT VOID         *Buffer)
  {
      UINTN   IncrementType       = Width & ~3;
      UINT8   IncrementValue      = 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
      UINT8   IncrementBuffer     = 0;
      UINT8   IncrementAddress    = 0;
      AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
      if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;

      if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
      if (IncrementType > 16 || IncrementValue > 8) return EFI_INVALID_PARAMETER;

      //if FIFO Address falls out of IO Range
      if (Address  > 0xffff)return EFI_INVALID_PARAMETER; 

      switch (IncrementType)
      {
      case 0: //EfiPciWidthUintxx
          IncrementAddress = IncrementBuffer = IncrementValue;
          break;
      case 4: //EfiPciWidthFifoUintxx
          IncrementBuffer = IncrementValue;
          break;
      default: //EfiPciWidthFillUintxx
          IncrementAddress = IncrementValue;
      }

      //Exclude FIFO operation from buffer increase check
      if(IncrementAddress) {
          if(Io32support){
              if ((Address + Count * IncrementValue) > 0xffffffff)
                  return EFI_INVALID_PARAMETER;   
          } else {
              //IO must be within range of the bridge.
              if ((Address + Count * IncrementValue) > 0xffff)
                  return EFI_INVALID_PARAMETER;   
          }
      }

      return mCpuIo2->Io.Read (mCpuIo2, (EFI_SMM_IO_WIDTH) IncrementType, 
                      Address, Count, Buffer);

}

/**
   Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

   @param[in]   This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width       Signifies the width of the memory operations.
   @param[in]   Address     The base address of the I/O operation. The caller is responsible for
                            aligning the Address if required.
   @param[in]   Count       The number of I/O operations to perform. Bytes moved is Width
                            size * Count, starting at Address.
   @param[out]  Buffer      For read operations, the destination buffer to store the results. For
                            write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS              The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER    Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoIoWrite (
  IN       EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL     *This,
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH   Width,
  IN       UINT64                                  Address,
  IN       UINTN                                   Count,
  IN OUT   VOID                                    *Buffer
  )
{
    UINTN   IncrementType       = Width & ~3;
    UINT8   IncrementValue      = 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
    UINT8   IncrementBuffer     = 0;
    UINT8   IncrementAddress    = 0;
    AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
    if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;

    if (Buffer==NULL || ((UINTN)Width>=(UINTN)EfiPciWidthMaximum)) return EFI_INVALID_PARAMETER;
    if (IncrementType > 16 || IncrementValue > 8) return EFI_INVALID_PARAMETER;

    //if FIFO Address falls out of IO Range
    if (Address  > 0xffff)return EFI_INVALID_PARAMETER; 

    switch (IncrementType)
    {
    case 0: //EfiPciWidthUintxx
        IncrementAddress = IncrementBuffer = IncrementValue;
        break;
    case 4: //EfiPciWidthFifoUintxx
        IncrementBuffer = IncrementValue;
        break;
    default: //EfiPciWidthFillUintxx
        IncrementAddress = IncrementValue;
    }

    //Exclude FIFO operation from buffer increase check
    if(IncrementAddress) {
        if(Io32support){
            if ((Address + Count * IncrementValue) > 0xffffffff)
                return EFI_INVALID_PARAMETER;   
        } else {
            //IO must be within range of the bridge.
            if ((Address + Count * IncrementValue) > 0xffff)
                return EFI_INVALID_PARAMETER;   
        }
    }

    return mCpuIo2->Io.Write (mCpuIo2, (EFI_SMM_IO_WIDTH) Width, 
                      Address, Count, Buffer);

}

/**
   Enables a PCI driver to copy one region of PCI root bridge memory space to another region of PCI
   root bridge memory space.

   The CopyMem() function enables a PCI driver to copy one region of PCI root bridge memory
   space to another region of PCI root bridge memory space. This is especially useful for video scroll
   operation on a memory mapped video buffer.
   The memory operations are carried out exactly as requested. The caller is responsible for satisfying
   any alignment and memory width restrictions that a PCI root bridge on a platform might require.

   @param[in] This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
   @param[in] Width       Signifies the width of the memory operations.
   @param[in] DestAddress The destination address of the memory operation. The caller is
                          responsible for aligning the DestAddress if required.
   @param[in] SrcAddress  The source address of the memory operation. The caller is
                          responsible for aligning the SrcAddress if required.
   @param[in] Count       The number of memory operations to perform. Bytes moved is
                          Width size * Count, starting at DestAddress and SrcAddress.
   
   @retval  EFI_SUCCESS             The data was copied from one memory region to another memory region.
   @retval  EFI_INVALID_PARAMETER   Width is invalid for this PCI root bridge.
   @retval  EFI_OUT_OF_RESOURCES    The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoCopyMem (
  IN EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 DestAddress,
  IN UINT64                                 SrcAddress,
  IN UINTN                                  Count
  )
{
  EFI_STATUS  Status;
  BOOLEAN     Direction;
  UINTN       Stride;
  UINTN       Index;
  UINT64      Result;
  AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
  if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;

  if (Width < 0 || Width > EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }    

  if (DestAddress == SrcAddress) {
    return EFI_SUCCESS;
  }

  Stride = (UINT32)(1 << Width);

  //
  // Determine the Direction of copy operation.
  // 
  Direction = TRUE;
  if ((DestAddress > SrcAddress) && (DestAddress < (SrcAddress + Count * Stride))) {
    Direction   = FALSE;
    SrcAddress  = SrcAddress  + (Count-1) * Stride;
    DestAddress = DestAddress + (Count-1) * Stride;
  }

  for (Index = 0;Index < Count;Index++) {
    Status = SmmRbIoMemRead (
               This,
               Width,
               SrcAddress,
               1,
               &Result
               );
    if (EFI_ERROR (Status)) return Status;
    
    Status = SmmRbIoMemWrite (
               This,
               Width,
               DestAddress,
               1,
               &Result
               );
    if (EFI_ERROR (Status)) return Status;
    
    if (Direction) {
      //
      // Forward
      // 
      SrcAddress  += Stride;
      DestAddress += Stride;
    } else {
      //
      // Backward
      // 
      SrcAddress  -= Stride;
      DestAddress -= Stride;
    }
  }
  return EFI_SUCCESS;
}

/**
   Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

   The Pci.Read() and Pci.Write() functions enable a driver to access PCI configuration
   registers for a PCI controller.
   The PCI Configuration operations are carried out exactly as requested. The caller is responsible for
   any alignment and PCI configuration width issues that a PCI Root Bridge on a platform might
   require.

   @param[in]   This      A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count     The number of PCI configuration operations to perform. Bytes
                          moved is Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoPciRead (
  IN       EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN       UINT64                                 Address,
  IN       UINTN                                  Count,
  IN OUT   VOID                                   *Buffer
  )
{
    AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
    if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;
    if (Buffer == NULL) return EFI_INVALID_PARAMETER;
    if (Width < 0 || Width >= EfiPciWidthMaximum) return EFI_INVALID_PARAMETER;
    
    return SmmRbIoPciRW (This, FALSE, Width, Address, Count, Buffer);
}

/**
   Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

   The Pci.Read() and Pci.Write() functions enable a driver to access PCI configuration
   registers for a PCI controller.
   The PCI Configuration operations are carried out exactly as requested. The caller is responsible for
   any alignment and PCI configuration width issues that a PCI Root Bridge on a platform might
   require.

   @param[in]   This      A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Width     Signifies the width of the memory operations.
   @param[in]   Address   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count     The number of PCI configuration operations to perform. Bytes
                          moved is Width size * Count, starting at Address.
   @param[out]  Buffer    For read operations, the destination buffer to store the results. For
                          write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoPciWrite (
  IN       EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN       UINT64                                 Address,
  IN       UINTN                                  Count,
  IN OUT   VOID                                   *Buffer
  )
{
    AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
    if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;
    if (Buffer == NULL) return EFI_INVALID_PARAMETER;
    if (Width < 0 || Width >= EfiPciWidthMaximum) return EFI_INVALID_PARAMETER;

    return SmmRbIoPciRW (This, TRUE, Width, Address, Count, Buffer);
}

/**
   Provides the PCI controller-specific addresses required to access system memory from a
   DMA bus master.

   The Map() function provides the PCI controller specific addresses needed to access system
   memory. This function is used to map system memory for PCI bus master DMA accesses.

   @param[in]       This            A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]       Operation       Indicates if the bus master is going to read or write to system memory.
   @param[in]       HostAddress     The system memory address to map to the PCI controller.
   @param[in][out]  NumberOfBytes   On input the number of bytes to map. On output the number of bytes that were mapped.
   @param[out]      DeviceAddress   The resulting map address for the bus master PCI controller to use
                                    to access the system memory's HostAddress.
   @param[out]      Mapping         The value to pass to Unmap() when the bus master DMA operation is complete.
   
   @retval EFI_SUCCESS            The range was mapped for the returned NumberOfBytes.
   @retval EFI_INVALID_PARAMETER  Operation is invalid.
   @retval EFI_INVALID_PARAMETER  HostAddress is NULL.
   @retval EFI_INVALID_PARAMETER  NumberOfBytes is NULL.
   @retval EFI_INVALID_PARAMETER  DeviceAddress is NULL.
   @retval EFI_INVALID_PARAMETER  Mapping is NULL.
   @retval EFI_UNSUPPORTED        The HostAddress cannot be mapped as a common buffer.
   @retval EFI_DEVICE_ERROR       The system hardware could not map the requested address.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
EFI_STATUS
EFIAPI SmmRbIoMap (
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  )
{
  return EFI_UNSUPPORTED;
}

/**
   Completes the Map() operation and releases any corresponding resources.

   The Unmap() function completes the Map() operation and releases any corresponding resources.
   If the operation was an EfiPciOperationBusMasterWrite or
   EfiPciOperationBusMasterWrite64, the data is committed to the target system memory.
   Any resources used for the mapping are freed.  

   @param[in] This      A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in] Mapping   The mapping value returned from Map().
   
   @retval EFI_SUCCESS            The range was unmapped.
   @retval EFI_INVALID_PARAMETER  Mapping is not a value that was returned by Map().
   @retval EFI_DEVICE_ERROR       The data was not committed to the target system memory.

**/
EFI_STATUS
EFIAPI SmmRbIoUnmap (
  IN EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN VOID                                 *Mapping
  )

{
  return EFI_UNSUPPORTED;
}

/**
   Allocates pages that are suitable for an EfiPciOperationBusMasterCommonBuffer or
   EfiPciOperationBusMasterCommonBuffer64 mapping.
  
   @param This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Type        This parameter is not used and must be ignored.
   @param MemoryType  The type of memory to allocate, EfiBootServicesData or EfiRuntimeServicesData.
   @param Pages       The number of pages to allocate.
   @param HostAddress A pointer to store the base system memory address of the allocated range.
   @param Attributes  The requested bit mask of attributes for the allocated range. Only
                      the attributes EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE, EFI_PCI_ATTRIBUTE_MEMORY_CACHED, 
                      and EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE may be used with this function.
   
   @retval EFI_SUCCESS            The requested memory pages were allocated.
   @retval EFI_INVALID_PARAMETER  MemoryType is invalid.
   @retval EFI_INVALID_PARAMETER  HostAddress is NULL.
   @retval EFI_UNSUPPORTED        Attributes is unsupported. The only legal attribute bits are
                                  MEMORY_WRITE_COMBINE, MEMORY_CACHED, and DUAL_ADDRESS_CYCLE.
   @retval EFI_OUT_OF_RESOURCES   The memory pages could not be allocated.

**/
EFI_STATUS
EFIAPI SmmRbIoAllocateBuffer (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                    Type,
  IN  EFI_MEMORY_TYPE                      MemoryType,
  IN  UINTN                                Pages,
  OUT VOID                                 **HostAddress,
  IN  UINT64                               Attributes
  )

{
  return EFI_UNSUPPORTED;
}

/**
   Frees memory that was allocated with AllocateBuffer().

   The FreeBuffer() function frees memory that was allocated with AllocateBuffer().

   @param This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Pages       The number of pages to free.
   @param HostAddress The base system memory address of the allocated range.
   
   @retval EFI_SUCCESS            The requested memory pages were freed.
   @retval EFI_INVALID_PARAMETER  The memory range specified by HostAddress and Pages
                                  was not allocated with AllocateBuffer().

**/
EFI_STATUS
EFIAPI SmmRbIoFreeBuffer (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                                Pages,
  OUT VOID                                 *HostAddress
  )
{
  return EFI_UNSUPPORTED;
}

/**
   Flushes all PCI posted write transactions from a PCI host bridge to system memory.

   The Flush() function flushes any PCI posted write transactions from a PCI host bridge to system
   memory. Posted write transactions are generated by PCI bus masters when they perform write
   transactions to target addresses in system memory.
   This function does not flush posted write transactions from any PCI bridges. A PCI controller
   specific action must be taken to guarantee that the posted write transactions have been flushed from
   the PCI controller and from all the PCI bridges into the PCI host bridge. This is typically done with
   a PCI read transaction from the PCI controller prior to calling Flush().

   @param This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   
   @retval EFI_SUCCESS        The PCI posted write transactions were flushed from the PCI host
                              bridge to system memory.
   @retval EFI_DEVICE_ERROR   The PCI posted write transactions were not flushed from the PCI
                              host bridge due to a hardware error.

**/
EFI_STATUS
EFIAPI SmmRbIoFlush (
  IN EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This
  )
{
  return EFI_UNSUPPORTED;
}

/**
   Gets the attributes that a PCI root bridge supports setting with SetAttributes(), and the
   attributes that a PCI root bridge is currently using.  

   The GetAttributes() function returns the mask of attributes that this PCI root bridge supports
   and the mask of attributes that the PCI root bridge is currently using.

   @param This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param Supported   A pointer to the mask of attributes that this PCI root bridge
                      supports setting with SetAttributes().
   @param Attributes  A pointer to the mask of attributes that this PCI root bridge is
                      currently using.
   
   @retval  EFI_SUCCESS           If Supports is not NULL, then the attributes that the PCI root
                                  bridge supports is returned in Supports. If Attributes is
                                  not NULL, then the attributes that the PCI root bridge is currently
                                  using is returned in Attributes.
   @retval  EFI_INVALID_PARAMETER Both Supports and Attributes are NULL.

**/
EFI_STATUS
EFIAPI SmmRbIoGetAttributes (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                               *Supported,
  OUT UINT64                               *Attributes
  )
{
    AMI_SMM_PCI_RB_DATA   *PrivateData=(AMI_SMM_PCI_RB_DATA*)This;
//--------------------------------------------
    if(PrivateData->Signature!= AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;
    
    if (Attributes == NULL && Supported == NULL) return EFI_INVALID_PARAMETER;

    if (Supported) *Supported  = PrivateData-> Supports;
    if (Attributes) *Attributes = PrivateData->Attributes;

    return EFI_SUCCESS;
}

/**
   Sets attributes for a resource range on a PCI root bridge.

   The SetAttributes() function sets the attributes specified in Attributes for the PCI root
   bridge on the resource range specified by ResourceBase and ResourceLength. Since the
   granularity of setting these attributes may vary from resource type to resource type, and from
   platform to platform, the actual resource range and the one passed in by the caller may differ. As a
   result, this function may set the attributes specified by Attributes on a larger resource range
   than the caller requested. The actual range is returned in ResourceBase and
   ResourceLength. The caller is responsible for verifying that the actual range for which the
   attributes were set is acceptable.

   @param[in]       This            A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]       Attributes      The mask of attributes to set. If the attribute bit
                                    MEMORY_WRITE_COMBINE, MEMORY_CACHED, or
                                    MEMORY_DISABLE is set, then the resource range is specified by
                                    ResourceBase and ResourceLength. If
                                    MEMORY_WRITE_COMBINE, MEMORY_CACHED, and
                                    MEMORY_DISABLE are not set, then ResourceBase and
                                    ResourceLength are ignored, and may be NULL.
   @param[in][out]  ResourceBase    A pointer to the base address of the resource range to be modified
                                    by the attributes specified by Attributes.
   @param[in][out]  ResourceLength  A pointer to the length of the resource range to be modified by the
                                    attributes specified by Attributes.
   
   @retval  EFI_SUCCESS     The current configuration of this PCI root bridge was returned in Resources.
   @retval  EFI_UNSUPPORTED The current configuration of this PCI root bridge could not be retrieved.
   @retval  EFI_INVALID_PARAMETER Invalid pointer of EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL

**/
EFI_STATUS
EFIAPI SmmRbIoSetAttributes ( 
  IN     EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                               Attributes,
  IN OUT UINT64                               *ResourceBase,
  IN OUT UINT64                               *ResourceLength   
  )
{
  return EFI_UNSUPPORTED;
}

/**
   Retrieves the current resource settings of this PCI root bridge in the form of a set of ACPI 2.0
   resource descriptors.

   There are only two resource descriptor types from the ACPI Specification that may be used to
   describe the current resources allocated to a PCI root bridge. These are the QWORD Address
   Space Descriptor (ACPI 2.0 Section 6.4.3.5.1), and the End Tag (ACPI 2.0 Section 6.4.2.8). The
   QWORD Address Space Descriptor can describe memory, I/O, and bus number ranges for dynamic
   or fixed resources. The configuration of a PCI root bridge is described with one or more QWORD
   Address Space Descriptors followed by an End Tag.

   @param[in]   This        A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[out]  Resources   A pointer to the ACPI 2.0 resource descriptors that describe the
                            current configuration of this PCI root bridge. The storage for the
                            ACPI 2.0 resource descriptors is allocated by this function. The
                            caller must treat the return buffer as read-only data, and the buffer
                            must not be freed by the caller.
   
   @retval  EFI_SUCCESS     The current configuration of this PCI root bridge was returned in Resources.
   @retval  EFI_UNSUPPORTED The current configuration of this PCI root bridge could not be retrieved.
   @retval  EFI_INVALID_PARAMETER Invalid pointer of EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL

**/
EFI_STATUS
EFIAPI SmmRbIoConfiguration (
  IN  EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT VOID                                 **Resources
  )
{
    AMI_SMM_PCI_RB_DATA *smmrb=(AMI_SMM_PCI_RB_DATA*)This;
//-------------
    if(smmrb->Signature!=AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE) return EFI_INVALID_PARAMETER;
    
    if( ((AMI_SMM_PCI_RB_DATA*)This)->ResAsquired ) {
        
    } else return EFI_NOT_AVAILABLE_YET;
    
    return EFI_SUCCESS;
}

/**
   Internal help function for read and write PCI configuration space.

   @param[in]   This          A pointer to the EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL.
   @param[in]   Write         Switch value for Read or Write.
   @param[in]   Width         Signifies the width of the memory operations.
   @param[in]   UserAddress   The address within the PCI configuration space for the PCI controller.
   @param[in]   Count         The number of PCI configuration operations to perform. Bytes
                              moved is Width size * Count, starting at Address.
   @param[out]  UserBuffer    For read operations, the destination buffer to store the results. For
                              write operations, the source buffer to write data from.
   
   @retval EFI_SUCCESS            The data was read from or written to the PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Width is invalid for this PCI root bridge.
   @retval EFI_INVALID_PARAMETER  Buffer is NULL.
   @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.

**/
static EFI_STATUS
EFIAPI SmmRbIoPciRW (
  IN EFI_SMM_PCI_ROOT_BRIDGE_IO_PROTOCOL    *This,
  IN BOOLEAN                                Write,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 UserAddress,
  IN UINTN                                  Count,
  IN OUT VOID                               *UserBuffer
  )
{
    //Call here PciAccessCspLib Function it will take care about PCIe or cf8/cfc...
    //Generic Function never checkes first parameter set it to NUll.
    return RootBridgeIoPciRW(This, Width, UserAddress, Count, UserBuffer, Write );
}

/*
EFI_STATUS CopyRbResources(PCI_ROOT_BRG_DATA *Rb, AMI_SMM_PCI_RB_DATA * SmmRb ){
    UINTN   i;
    
//----------------
    
    SmmRb->ResCount=Rb->ResCount;
    SmmRb->RbRes=AllocateZeroPool(sizeof(ASLR_QWORD_ASD)*SmmRb->ResCount);
    if(SmmRb->RbRes) return EFI_OUT_OF_RESOURCES;
    
      
    for(i=0;i<Rb->ResCount;i++){
        CopyMem(&SmmRb->RbRes[i],Rb->RbRes[i], sizeof(ASLR_QWORD_ASD));
    }
    
    return EFI_SUCCESS;
}
*/

EFI_STATUS EFIAPI InitSmmRbIoData(){
    
    PCI_HOST_BRG_DATA   *host;
    PCI_ROOT_BRG_DATA   *root;
    AMI_SMM_PCI_RB_DATA *smmrb;
    UINTN               i,j,k;
//    EFI_STATUS          Status;
//--------------------------------
    //Count number of RBs
    //hoost loop;
    for(j=0; j<gPciHandoffProtocol->HostCnt;j++){
        host=&gPciHandoffProtocol->AmiHostData[j];
        for(i=0;i<host->RootBridgeCount; i++){
            root=host->RootBridges[i];
            if(!root->NotPresent)gSmmRbIoCnt++;
        }
    }
    
    //Get some SMM Memory to accomodate private data structures...
    gAmiSmmRbArray=AllocateZeroPool(sizeof(AMI_SMM_PCI_RB_DATA)*gSmmRbIoCnt);
    if(gAmiSmmRbArray==NULL) return EFI_OUT_OF_RESOURCES;
    
    for(j=0,k=0; j<gPciHandoffProtocol->HostCnt;j++){
        host=&gPciHandoffProtocol->AmiHostData[j];
        for(i=0;i<host->RootBridgeCount;i++){
            root=host->RootBridges[i];
            
            //Skip "NotPresent" root bridges; 
            if(root->NotPresent) continue;
                
            smmrb=&gAmiSmmRbArray[k];
            
            //copy needed content from PCI_ROOT_BRG_DATA to PCI_ROOT_BRG_DATA
            //and init the rest of the fields....
            smmrb->Signature=AMI_SMM_PCI_ROOT_BRIDGE_SIGNATURE;
            smmrb->HostIdx=j;
            smmrb->RootIdx=i;
            
            //Init Protocol Interface function pointers...
            smmrb->SmmRbIo.SegmentNumber=root->RbIoProtocol.SegmentNumber;
            smmrb->SmmRbIo.ParentHandle=root->RbIoProtocol.ParentHandle;
            smmrb->SmmRbIo.AllocateBuffer=SmmRbIoAllocateBuffer;
            smmrb->SmmRbIo.Configuration=SmmRbIoConfiguration;
            smmrb->SmmRbIo.CopyMem=SmmRbIoCopyMem;
            smmrb->SmmRbIo.Flush=SmmRbIoFlush;
            smmrb->SmmRbIo.FreeBuffer=SmmRbIoFreeBuffer;
            smmrb->SmmRbIo.GetAttributes=SmmRbIoGetAttributes;
            smmrb->SmmRbIo.SetAttributes=SmmRbIoSetAttributes;
            smmrb->SmmRbIo.Io.Read=SmmRbIoIoRead;
            smmrb->SmmRbIo.Io.Write=SmmRbIoIoWrite;
            smmrb->SmmRbIo.Map=SmmRbIoMap;
            smmrb->SmmRbIo.Unmap=SmmRbIoUnmap;
            smmrb->SmmRbIo.Mem.Read=SmmRbIoMemRead;
            smmrb->SmmRbIo.Mem.Write=SmmRbIoMemWrite;
            smmrb->SmmRbIo.Pci.Read=SmmRbIoPciRead;
            smmrb->SmmRbIo.Pci.Write=SmmRbIoPciWrite;
            smmrb->SmmRbIo.PollIo=SmmRbIoPollIo;
            smmrb->SmmRbIo.PollMem=SmmRbIoPollMem;
            
            //keep filling fields of AMI_SMM_PCI_RB_DATA
            smmrb->Attributes=root->Attributes;
            smmrb->DevicePath=DuplicateDevicePath(root->DevPath);
            smmrb->RbSdlData=root->RbSdlData;
            smmrb->RbSdlIndex=root->RbSdlIndex;
            
            //Now goes RB flags..
            smmrb->BusesSet=root->BusesSet;
            smmrb->NotPresent=root->NotPresent;
            smmrb->ResAsquired=root->ResAsquired;
            smmrb->ResSubmited=root->ResSubmited;
            
            //if DXE RB has resources at that point - copu it here
            /*
            if(smmrb->ResAsquired) {
                Status=CopyRbResources(root, smmrb);
            } else {
                //Set an event to Update resources latter
                
            }
            */
            //----------------
            k++;
        }
    }
    
    
    return EFI_SUCCESS;
}





EFI_STATUS EFIAPI SmmPciRootBridgeInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS              Status;
    BOOLEAN                 InSmm=FALSE;
    UINTN                   i;
    AMI_SMM_PCI_RB_DATA     *smmrb;
//--------------------------

    //Just in case if some other lib uses it.
    InitAmiSmmLib(ImageHandle,SystemTable);
    
    // Retrieve SMM Base2 Protocol and PCI Hand Off Protocol do it only once 
    // Do not use gBS from UefiBootServicesTableLib on purpose
    // to prevent inclusion of gBS, gST, and gImageHandle from SMM Drivers unless the 
    // SMM driver explicity declares that dependency. 
    Status=SystemTable->BootServices->LocateProtocol(&gEfiSmmBase2ProtocolGuid,NULL,(VOID**)&gSmmBase);
    
    if( EFI_ERROR(Status)){
        ASSERT_EFI_ERROR (Status);
        return EFI_UNSUPPORTED;
    }
        
    gSmmBase->InSmm (gSmmBase, &InSmm);
    
    // Check to see if we are already in SMM
    if (!InSmm ) {
        // We are not in SMM, so SMST is not needed
        return EFI_SUCCESS;
    }

    // We are in SMM, retrieve the pointer to SMM System Table
    Status=gSmmBase->GetSmstLocation (gSmmBase, &gSmst2);
    ASSERT (gSmst2 != NULL);
    if( EFI_ERROR(Status) || (gSmst2 == NULL)) return EFI_UNSUPPORTED;


    // Retrieve the SMM CPU I/O 2 Protocol
    mCpuIo2 = (EFI_SMM_CPU_IO2_PROTOCOL*)&gSmst2->SmmIo;

    Status=SystemTable->BootServices->LocateProtocol(&gAmiPciSmmHandoffProtocolGuid,NULL,(VOID**)&gPciHandoffProtocol);
    PCI_TRACE((TRACE_PCI,"SmmRb: Get PCI RB SMM Handoff Protocol - Status=%r.\n",Status));
    //Instance must be there since this driver has dependency on ito protocol
    if(EFI_ERROR(Status)){
        ASSERT_EFI_ERROR (Status);
        return EFI_UNSUPPORTED;
    }
        
    //Now init SMM RB IO driver internal Data...
    Status=InitSmmRbIoData();
    PCI_TRACE((TRACE_PCI,"SmmRb: Initialize SMM RbIo ... RbCnt=%d; Status=%r.\n",gSmmRbIoCnt,Status));
    if(EFI_ERROR(Status)) return Status;
    
    //Instal SmmRbIo protocol instances...
    for(i=0;i<gSmmRbIoCnt; i++){
        smmrb=&gAmiSmmRbArray[i];
        if(smmrb->NotPresent) {
            PCI_TRACE((TRACE_PCI,"SmmRb: RootBridge #%d - Not Present - Skipping...\n",i));
            continue; 
        }
            
        //Install SMM PCI Root Bridge I/O Protocol
        Status=gSmst2->SmmInstallProtocolInterface (
              &smmrb->Handle,
              &gEfiSmmPciRootBridgeIoProtocolGuid,
              EFI_NATIVE_INTERFACE,
              &smmrb->SmmRbIo
              );
        PCI_TRACE((TRACE_PCI,"SmmRb: Installing SMM_PCI_RB_IO_PROTOKOL for Rb #%d; Handle=0x%X; Status=%r.\n",
                    i,smmrb->Handle,Status));

        if(EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
    }
    
    return EFI_SUCCESS;
}
