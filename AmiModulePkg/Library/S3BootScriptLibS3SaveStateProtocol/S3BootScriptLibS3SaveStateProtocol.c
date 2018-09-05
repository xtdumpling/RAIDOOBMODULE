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
/** @file
  S3BootScriptLib instance built on top of the S3SaveState or S3SmmSaveState protocol.

  It provides  the following advantages over the MdeModulePkg.DxeS3BootScriptLib instance:
    - This instance is a wrapper around S3SaveState/S3SmmSaveState
      protocol. It produces much smaller binaries than MdeModulePkg instance.
    - MdeModulePkg instance can produce cyclic dependency.
      MdeModulePkg S3BootScriptLib instance has dependency from SmbusLib instance, 
      which typically has dependency from SmbusHc protocol. 
      The SmbusHc protocol is published by chipset specific driver which may use 
      S3BootScriptLib library thus producing a cyclic dependency:
      S3BootScriptLib -> SmbusLib -> SmbusHc Protocol -> S3BootScriptLib.

  __NOTE: This instance does not support the following S3BootScriptLib library class functions:
    S3BootScriptCloseTable, S3BootScriptExecute, S3BootScriptMoveLastOpcode
**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/SmbusLib.h>
#include <Library/BaseLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/S3SaveState.h>

EFI_S3_SAVE_STATE_PROTOCOL *S3SaveState = NULL;

/**
  Save I/O write to boot script.

  @param[in] Width     The width of the I/O operations.
  @param[in] Address   The base address of the I/O operations.
  @param[in] Count     The number of I/O operations to perform.
  @param[in] Buffer    The source buffer from which to write data.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.

**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveIoWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN  UINT64                    Address,
  IN  UINTN                     Count,
  IN  VOID                      *Buffer
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_IO_WRITE_OPCODE,
			Width, Address, Count, Buffer
	);
}

/**
  Adds a record for an I/O modify operation into a S3 boot script table.

  @param[in] Width      The width of the I/O operations.
  @param[in] Address    The base address of the I/O operations.
  @param[in] Data       A pointer to the data to be OR-ed.
  @param[in] DataMask   A pointer to the data mask to be AND-ed with the data
                        read from the register.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.

**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveIoReadWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN  UINT64                    Address,
  IN  VOID                      *Data,
  IN  VOID                      *DataMask
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_IO_READ_WRITE_OPCODE,
			Width, Address, Data, DataMask
	);
}

/**
  Adds a record for a memory write operation into a specified boot script table.

  @param[in] Width     The width of the I/O operations.
  @param[in] Address   The base address of the memory operations
  @param[in] Count     The number of memory operations to perform.
  @param[in] Buffer    The source buffer from which to write the data.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveMemWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN  UINT64                    Address,
  IN  UINTN                     Count,
  IN  VOID                      *Buffer
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE,
			Width, Address, Count, Buffer
	);
}

/**
  Adds a record for a memory modify operation into a specified boot script table.

  @param[in] Width      The width of the I/O operations.
  @param[in] Address    The base address of the memory operations. Address needs 
                        alignment, if required
  @param[in] Data       A pointer to the data to be OR-ed.
  @param[in] DataMask   A pointer to the data mask to be AND-ed with the data 
                        read from the register.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveMemReadWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN  UINT64                    Address,
  IN  VOID                      *Data,
  IN  VOID                      *DataMask
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE,
			Width, Address, Data, DataMask
	);
}

/**
  Adds a record for a PCI configuration space write operation into a specified boot script table.

  @param[in] Width     The width of the I/O operations.
  @param[in] Address   The address within the PCI configuration space.
  @param[in] Count     The number of PCI operations to perform.
  @param[in] Buffer    The source buffer from which to write the data.

  @retval RETURN_OUT_OF_RESOURCES  Not enough memory for the table to perform 
                                   the operation.
  @retval RETURN_SUCCESS           The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSavePciCfgWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN  UINT64                    Address,
  IN  UINTN                     Count,
  IN  VOID                      *Buffer
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,
			Width, Address, Count, Buffer
	);
}

/**
  Adds a record for a PCI configuration space modify operation into a specified boot script table.

  @param[in] Width      The width of the I/O operations.
  @param[in] Address    The address within the PCI configuration space.
  @param[in] Data       A pointer to the data to be OR-ed.The size depends on Width.
  @param[in] DataMask   A pointer to the data mask to be AND-ed.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN__SUCCESS           The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSavePciCfgReadWrite (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN  UINT64                    Address,
  IN  VOID                      *Data,
  IN  VOID                      *DataMask
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_PCI_CONFIG_READ_WRITE_OPCODE,
			Width, Address, Data, DataMask
	);
}

/**
  Adds a record for a PCI configuration space modify operation into a specified boot script table.

  @param[in] Width     The width of the I/O operations.
  @param[in] Segment   The PCI segment number for Address.
  @param[in] Address   The address within the PCI configuration space.
  @param[in] Count     The number of PCI operations to perform.
  @param[in] Buffer    The source buffer from which to write the data.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSavePciCfg2Write (
  IN S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN UINT16                    Segment,
  IN UINT64                    Address,
  IN UINTN                     Count,
  IN VOID                      *Buffer
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_PCI_CONFIG2_WRITE_OPCODE,
			Width, Segment, Address, Count, Buffer
	);
}

/**
  Adds a record for a PCI configuration space modify operation into a specified boot script table.

  @param[in] Width      The width of the I/O operations.
  @param[in] Segment    The PCI segment number for Address.
  @param[in] Address    The address within the PCI configuration space.
  @param[in] Data       A pointer to the data to be OR-ed. The size depends on Width.
  @param[in] DataMask   A pointer to the data mask to be AND-ed.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSavePciCfg2ReadWrite (
  IN S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN UINT16                    Segment,
  IN UINT64                    Address,
  IN VOID                      *Data,
  IN VOID                      *DataMask
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_PCI_CONFIG2_READ_WRITE_OPCODE,
			Width, Segment, Address, Data, DataMask
	);
}

/**
  Adds a record for an SMBus command execution into a specified boot script table.

  @param[in] SmBusAddress   Address that encodes the SMBUS Slave Address, SMBUS 
                            Command, SMBUS Data Length, and PEC.
  @param[in] Operation      Indicates which particular SMBus protocol it will use 
                            to execute the SMBus transactions.
  @param[in] Length         A pointer to signify the number of bytes that this 
                            operation will do.
  @param[in] Buffer         Contains the value of data to execute to the SMBUS 
                            slave device.
  
  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveSmbusExecute (
  IN  UINTN                SmBusAddress, 
  IN  EFI_SMBUS_OPERATION  Operation,
  IN  UINTN                *Length,
  IN  VOID                 *Buffer
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_SMBUS_EXECUTE_OPCODE,
			SMBUS_LIB_SLAVE_ADDRESS(SmBusAddress), SMBUS_LIB_COMMAND(SmBusAddress),
			Operation, SMBUS_LIB_PEC(SmBusAddress), Length, Buffer
	);
}

/**
  Adds a record for an execution stall on the processor into a specified boot script table.

  @param[in] Duration   The duration in microseconds of the stall.
  
  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveStall (
  IN  UINTN  Duration
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_STALL_OPCODE, Duration
	);
}

/**
  Adds a record for dispatching specified arbitrary code into a specified boot script table.

  @param[in] EntryPoint   The entry point of the code to be dispatched.
  @param[in] Context      The argument to be passed into the EntryPoint of the code 
                          to be dispatched.
  
  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveDispatch2 (
  IN  VOID  *EntryPoint,
  IN  VOID  *Context
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_DISPATCH_2_OPCODE, EntryPoint, Context
	);
}

/**
  Adds a record for dispatching specified arbitrary code into a specified boot script table.

  @param[in] EntryPoint   The entry point of the code to be dispatched.
  
  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveDispatch (
  IN  VOID *EntryPoint
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_DISPATCH_OPCODE, EntryPoint
	);
}

/**
  Adds a record for memory reads of the memory location and continues when the exit 
  criteria is satisfied, or after a defined duration.

  Please aware, below interface is different with PI specification, Vol 5:
  EFI_S3_SAVE_STATE_PROTOCOL.Write() for EFI_BOOT_SCRIPT_MEM_POLL_OPCODE.
  "Duration" below is microseconds, while "Delay" in PI specification means
  the number of 100ns units to poll.

  @param[in] Width       The width of the memory operations.
  @param[in] Address     The base address of the memory operations.
  @param[in] BitMask     A pointer to the bit mask to be AND-ed with the data read 
                         from the register.
  @param[in] BitValue    A pointer to the data value after to be Masked.
  @param[in] Duration    The duration in microseconds of the stall.
  @param[in] LoopTimes   The times of the register polling.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.

**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveMemPoll (
  IN  S3_BOOT_SCRIPT_LIB_WIDTH  Width,
  IN  UINT64                    Address,
  IN  VOID                      *BitMask,
  IN  VOID                      *BitValue,
  IN  UINTN                     Duration,
  IN  UINTN                     LoopTimes
  ){
	ASSERT(S3SaveState!=NULL);
	// Duration is in microseconds; the protocol expects delay in 100 nanosecond units.
	Duration *= 10; 
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_MEM_POLL_OPCODE,
			Width, Address, BitValue, BitMask, MultU64x32(Duration, (UINT32)LoopTimes)
	);
}

/**
  Store arbitrary information in the boot script table. This opcode is a no-op on 
  dispatch and is only used for debugging script issues.
  
  @param[in] InformationLength   Length of the data in bytes
  @param[in] Information        Information to be logged in the boot scrpit
 
  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.

**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveInformation (
  IN  UINT32  InformationLength, 
  IN  VOID    *Information
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_INFORMATION_OPCODE,
			InformationLength, (EFI_PHYSICAL_ADDRESS)(UINTN)Information
	);
}

/**
  Adds a record for I/O reads the I/O location and continues when the exit criteria
   is satisfied, or after a defined duration.
  
  @param  Width                 The width of the I/O operations. 
  @param  Address               The base address of the I/O operations.
  @param  Data                  The comparison value used for the polling exit criteria.
  @param  DataMask              The mask used for the polling criteria. The bits in 
                                the bytes below Width which are zero in Data are 
                                ignored when polling the memory address.
  @param  Delay                 The number of 100ns units to poll. Note that timer 
                                available may be of insufficient granularity, so the 
                                delay may be longer.

 @retval RETURN_OUT_OF_RESOURCES  Not enough memory for the table to perform the 
                                  operation.
 @retval RETURN_SUCCESS           The opcode was added.
 @note   The FRAMEWORK version implementation does not support this API 
**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveIoPoll (
  IN S3_BOOT_SCRIPT_LIB_WIDTH       Width,
  IN UINT64                     Address,
  IN VOID                      *Data,
  IN VOID                      *DataMask, 
  IN UINT64                     Delay   
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_IO_POLL_OPCODE,
			Width, Address, Data, DataMask, Delay
	);
}

/**
  Adds a record for PCI configuration space reads and continues when the exit 
  criteria is satisfied ,or after a defined duration.

  @param  Width                 The width of the I/O operations. 
  @param  Address               The address within the PCI configuration space.
  @param  Data                  The comparison value used for the polling exit 
                                criteria.
  @param  DataMask              Mask used for the polling criteria. The bits in 
                                the bytes below Width which are zero in Data are
                                ignored when polling the memory address.
  @param  Delay                 The number of 100ns units to poll. Note that timer 
                                available may be of insufficient granularity, so the 
                                delay may be longer.

 @retval RETURN_OUT_OF_RESOURCES  Not enough memory for the table to perform the 
                                 operation.
 @retval RETURN_SUCCESS           The opcode was added.
 @note   The FRAMEWORK version implementation does not support this API 
**/
RETURN_STATUS
EFIAPI
S3BootScriptSavePciPoll (
   IN S3_BOOT_SCRIPT_LIB_WIDTH   Width,
   IN UINT64                     Address,
   IN VOID                      *Data,
   IN VOID                      *DataMask,
   IN UINT64                     Delay
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_PCI_CONFIG_POLL_OPCODE,
			Width, Address, Data, DataMask, Delay
	);
}

/**
  Adds a record for PCI configuration space reads and continues when the exit criteria 
  is satisfied, or after a defined duration.

  @param  Width                 The width of the I/O operations. 
  @param  Segment               The PCI segment number for Address.
  @param  Address               The address within the PCI configuration space.
  @param  Data                  The comparison value used for the polling exit 
                                criteria.
  @param  DataMask              Mask used for the polling criteria. The bits in 
                                the bytes below Width which are zero
                                in Data are ignored when polling the memory address
  @param  Delay                 The number of 100ns units to poll. Note that timer 
                                available may be of insufficient granularity so the delay 
                                may be longer.

  @retval RETURN_OUT_OF_RESOURCES  Not enough memory for the table to perform the 
                                   operation.
  @retval RETURN_SUCCESS           The opcode was added.
  @note  A known Limitations in the implementation: When interpreting the opcode  
         EFI_BOOT_SCRIPT_PCI_CONFIG2_WRITE_OPCODE, EFI_BOOT_SCRIPT_PCI_CONFIG2_READ_WRITE_OPCODE
         and EFI_BOOT_SCRIPT_PCI_CONFIG2_POLL_OPCODE, the 'Segment' parameter is assumed as 
         Zero, or else, assert.
         The FRAMEWORK version implementation does not support this API.

**/
RETURN_STATUS
EFIAPI
S3BootScriptSavePci2Poll (
   IN S3_BOOT_SCRIPT_LIB_WIDTH      Width,
   IN UINT16                        Segment,
   IN UINT64                        Address,
   IN VOID                         *Data,
   IN VOID                         *DataMask,
   IN UINT64                        Delay
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Write(
			S3SaveState, EFI_BOOT_SCRIPT_PCI_CONFIG2_POLL_OPCODE,
			Width, Segment, Address, Data, DataMask, Delay
	);
}

/**
  Save ASCII string information specified by Buffer to boot script with opcode 
  EFI_BOOT_SCRIPT_INFORMATION_OPCODE.

  @param[in] String   The Null-terminated ASCII string to store into the S3 boot 
                      script table.

  @retval RETURN_OUT_OF_RESOURCES   Not enough memory for the table to perform 
                                    the operation.
  @retval RETURN_SUCCESS            The opcode was added.

**/
RETURN_STATUS
EFIAPI
S3BootScriptSaveInformationAsciiString (
  IN  CONST CHAR8  *String
  ){
	return S3BootScriptSaveInformation( (UINT32)AsciiStrSize(String), (VOID*)String );
}

/**
  This is an function to close the S3 boot script table. The function could only 
  be called in BOOT time phase. To comply with the Framework spec definition on 
  EFI_BOOT_SCRIPT_SAVE_PROTOCOL.CloseTable(), this function will fulfill following things:
  1. Closes the specified boot script table
  2. It allocates a new memory pool to duplicate all the boot scripts in the specified table. 
     Once this function is called, the table maintained by the library will be destroyed 
     after it is copied into the allocated pool.
  3. Any attempts to add a script record after calling this function will cause a 
     new table to be created by the library.
  4. The base address of the allocated pool will be returned in Address. Note that 
     after using the boot script table, the CALLER is responsible for freeing the 
     pool that is allocated by this function. 

  In Spec PI1.1, this EFI_BOOT_SCRIPT_SAVE_PROTOCOL.CloseTable() is retired. This 
  API is supplied here to meet the requirements of the Framework Spec.
  
  If anyone does call CloseTable() on a real platform, then the caller is responsible
  for figuring out how to get the script to run on an S3 resume because the boot script 
  maintained by the lib will be destroyed.
 
  @return the base address of the new copy of the boot script table.   

**/
UINT8*
EFIAPI
S3BootScriptCloseTable (
  VOID
  ){
	// The function is not support by this library instance.
	ASSERT(FALSE);
	return NULL;
}

/**
  Executes the S3 boot script table.

  @retval RETURN_SUCCESS       The boot script table was executed successfully.
  @retval RETURN_UNSUPPORTED   Invalid script table or opcode.  

**/
RETURN_STATUS
EFIAPI
S3BootScriptExecute (
  VOID
  ){
	// The function is not support by this library instance.
	ASSERT(FALSE);
	return RETURN_UNSUPPORTED;
}

/**
  Move the last boot script entry to the position 

  @param  BeforeOrAfter         Specifies whether the opcode is stored before 
                                (TRUE) or after (FALSE) the positionin the boot 
                                script table specified by Position. If Position 
                                is NULL or points to NULL then the new opcode is 
                                inserted at the beginning of the table (if TRUE) 
                                or end of the table (if FALSE).
  @param  Position              On entry, specifies the position in the boot script 
                                table where the opcode will be inserted, either 
                                before or after, depending on BeforeOrAfter. On 
                                exit, specifies the position of the inserted opcode 
                                in the boot script table.

  @retval RETURN_OUT_OF_RESOURCES  The table is not available.
  @retval RETURN_INVALID_PARAMETER The Position is not a valid position in the 
                                   boot script table.
  @retval RETURN_SUCCESS           The opcode was inserted.
  @note   The FRAMEWORK version implementation does not support this API. 
**/
RETURN_STATUS
EFIAPI
S3BootScriptMoveLastOpcode (
  IN     BOOLEAN                        BeforeOrAfter,
  IN OUT VOID                         **Position OPTIONAL
  ){
	// The function is not support by this library instance.
	ASSERT(FALSE);
	return RETURN_UNSUPPORTED;
}

/**
  Find a label within the boot script table and, if not present, optionally create it.

  @param  BeforeOrAfter         Specifies whether the opcode is stored before (TRUE)
                                or after (FALSE) the position in the boot script table 
                                specified by Position.
  @param  CreateIfNotFound      Specifies whether the label will be created if the  
                                label does not exists (TRUE) or not (FALSE).
  @param  Position              On entry, specifies the position in the boot script 
                                table where the opcode will be inserted, either 
                                before or after, depending on BeforeOrAfter. On exit, 
                                specifies the positionof the inserted opcode in 
                                the boot script table.
  @param  Label                 Points to the label which will be inserted in the 
                                boot script table.
  @retval EFI_SUCCESS           The operation succeeded. A record was added into 
                                the specified script table.
  @retval EFI_INVALID_PARAMETER The parameter is illegal or the given boot script 
                                is not supported. If the opcode is unknow or not 
                                supported because of the PCD Feature Flags.
  @retval EFI_OUT_OF_RESOURCES  There is insufficient memory to store the boot script.
  @note   The FRAMEWORK version implementation does not support this API 

**/
RETURN_STATUS
EFIAPI 
S3BootScriptLabel (
  IN       BOOLEAN                      BeforeOrAfter,
  IN       BOOLEAN                      CreateIfNotFound,
  IN OUT   VOID                       **Position OPTIONAL,
  IN CONST CHAR8                       *Label
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Label(
			S3SaveState, BeforeOrAfter, CreateIfNotFound, Position, Label
	);
}

/**
  Compare two positions in the boot script table and return their relative position.
  @param  Position1             The positions in the boot script table to compare
  @param  Position2             The positions in the boot script table to compare
  @param  RelativePosition      On return, points to the result of the comparison

  @retval EFI_SUCCESS           The operation succeeded. A record was added into the
                                specified script table.
  @retval EFI_INVALID_PARAMETER The parameter is illegal or the given boot script 
                                is not supported. If the opcode is unknow or not s
                                upported because of the PCD Feature Flags.
  @retval EFI_OUT_OF_RESOURCES  There is insufficient memory to store the boot script.
  @note   The FRAMEWORK version implementation does not support this API 
**/
RETURN_STATUS
EFIAPI 
S3BootScriptCompare (
  IN  UINT8                       *Position1,
  IN  UINT8                       *Position2,
  OUT UINTN                       *RelativePosition
  ){
	ASSERT(S3SaveState!=NULL);
	return S3SaveState->Compare(
			S3SaveState, Position1, Position2, RelativePosition
	);
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
