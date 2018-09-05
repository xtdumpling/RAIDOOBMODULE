//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jun/07/2016
//
//****************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmiBmc.c
    IPMI Transport Driver.

**/

//---------------------------------------------------------------------------

#include "IpmiBmc.h"
#if (COMMAND_SPECIFIC_COUNTER_UPDATE_SUPPORT == 1)
#include "CommandSpecificRetryCountTable.h"
#endif

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//
// SSIF interface elink to check the smbus alert pin status for reading response data.
//
extern SSIF_ALERT_PIN_CHECK SSIF_ALERT_PIN_CHECK_HOOK_LIST EndOfSsifAlertPinCheckHookListFunctions;
SSIF_ALERT_PIN_CHECK* SsifAlertPinCheckHookList[] = {SSIF_ALERT_PIN_CHECK_HOOK_LIST NULL};

//---------------------------------------------------------------------------

/**
    Updates the SoftErrorCount variable based on the BMC Error input

    @param BmcError - BMC Error
    @param IpmiInstance - Data structure describing BMC
           variables and used for sending commands

    @return EFI_STATUS
    @retval EFI_SUCCESS is returned

**/
EFI_STATUS
UpdateSoftErrorCount (
  IN        UINT8                         BmcError,
  IN OUT    EFI_IPMI_BMC_INSTANCE_DATA    *IpmiInstance )
{
    UINT8   Errors[] = {0xC0, 0xC3, 0xC4, 0xC9, 0xCE, 0xCF, 0xFF, 0x00};
    UINT8   i;

    i = 0;
    while (Errors[i] != 0) {
        if (Errors[i] == BmcError) {
            IpmiInstance->SoftErrorCount++;
            break;
        }
        i++;
    }
    return EFI_SUCCESS;
}

/**
    Check the SMBUS alert pin status function

    @param VOID

    @return BOOLEAN - Status of SMBALERT pin

**/
BOOLEAN
CheckAlertPinHook (
  IN VOID
)
{
    BOOLEAN CheckAlertSignal = FALSE;
    UINTN i;

    for (i = 0; SsifAlertPinCheckHookList[i]; i++) {
        CheckAlertSignal = SsifAlertPinCheckHookList[i]();
    }

    return CheckAlertSignal;
}

/**
    Verify the data integrity using checksum of BMC response data.

    @param ResponseData    Response data from BMC.
    @param ResponseSize    Data size of response data.

    @return EFI_STATUS 
    @retval EFI_SUCCESS when data integrity is valid.

**/
EFI_STATUS
CheckDataValidity (
  IN  UINT8 *ResponseData,
  IN  UINT8 ResponseSize )
{
    UINT8 i;
    UINT8 CheckSum = 0;
    UINT8 DataSum = 0;

    //
    // Header CheckSum
    //
    for(i = 0; i < 2; i++) {
        DataSum += ResponseData[i];
    }

    //
    // Verify header check sum.
    //
    CheckSum = (UINT8) (0x100 - DataSum);
    if(CheckSum != ResponseData[2]) {
        return EFI_INVALID_PARAMETER;
    }

    DataSum = 0;

    //
    // Information CheckSum
    //
    for(i = 3; i < (ResponseSize - 1); i++) {
          DataSum += ResponseData[i];
    }

    //
    // Verify information check sum.
    //
    CheckSum = (UINT8) (0x100 - DataSum);
    if(CheckSum != ResponseData[ResponseSize - 1]) {
        return EFI_INVALID_PARAMETER;
    }
    return EFI_SUCCESS;
}

#if (COMMAND_SPECIFIC_COUNTER_UPDATE_SUPPORT == 1)
/**
    Updates Command specific retry counter based on the availability.
  	If not available, updates default KCS retry counter value.

    @param NetFunction - Net function of the command
    @param Command - Command to be sent to BMC

    @retval - a value of type UINT32
**/
UINT32
UpdateCommandSpecificDelay (
  IN UINT8                  NetFunction,
  IN UINT8                  Command )
{
    UINTN                       TableSize;
    UINTN                       Index;

    SERVER_IPMI_DEBUG((EFI_D_INFO, "%a Entry... \n", __FUNCTION__));
    //
    // Command specific retry count table
    //
    TableSize = (sizeof(CommandSpecificRetryCountTable)/
                   sizeof(COMMAND_SPECIFIC_RETRY_COUNT));

    for (Index = 0; Index < TableSize-1; Index++) {
      if (NetFunction == CommandSpecificRetryCountTable[Index].NetFunction &&\
          Command == CommandSpecificRetryCountTable[Index].Command ){
          SERVER_IPMI_DEBUG((EFI_D_INFO,"Counter value changed\n"));
#if(IPMI_SYSTEM_INTERFACE == KCS_INTERFACE)
          return CommandSpecificRetryCountTable[Index].RetryCounter;
#endif
#if(IPMI_SYSTEM_INTERFACE == BT_INTERFACE)
          return CommandSpecificRetryCountTable[Index].BtDelay;
#endif
      }
    }
    SERVER_IPMI_DEBUG((EFI_D_INFO,"Counter value not changed as there is no match\n"));
#if(IPMI_SYSTEM_INTERFACE == KCS_INTERFACE)
    return IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
#endif
#if(IPMI_SYSTEM_INTERFACE == BT_INTERFACE)
    return BT_DELAY;
#endif
}
#endif

/**
  Lock the IPMI transport

  @param This     Pointer to IPMI Protocol/PPI
  
  @retval VOID    Returns nothing
  
**/

VOID
IpmiTransportAcquireLock (
  EFI_IPMI_TRANSPORT *This )
{
    EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance;

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    IpmiInstance->IpmiTransportLocked = TRUE;
}

/**
  Release/Unlock the IPMI transport.

  @param  This      Pointer to IPMI Protocol/PPI
  
  @retval VOID      Returns nothing
  
**/

VOID
IpmiTransportReleaseLock (
  EFI_IPMI_TRANSPORT *This )
{
    EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance;

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    IpmiInstance->IpmiTransportLocked = FALSE;
}

/**
  This Routine returns the Lock state of IPMI transport

  @param  This      Pointer to IPMI Protocol/PPI
  
  @retval TRUE      IPMI transport is in lock state
  @retval FALSE     IPMI transport is in release state
  
**/

BOOLEAN
IpmiTransportlocked (
  EFI_IPMI_TRANSPORT *This )
{
    EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance;

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    return IpmiInstance->IpmiTransportLocked;
}

/**
    BT interface IPMI send command Implementation

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC
    @param  Context - NULL here

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
IpmiBtSendCommand (
  IN EFI_IPMI_TRANSPORT               *This,
  IN UINT8                            NetFunction,
  IN UINT8                            Lun,
  IN UINT8                            Command,
  IN UINT8                            *CommandData,
  IN UINT8                            CommandDataSize,
  OUT UINT8                           *ResponseData,
  OUT UINT8                           *ResponseDataSize,
  OUT UINT8                           *CompletionCode,
  IN VOID                             *Context )
{
    EFI_IPMI_BMC_INSTANCE_DATA          *IpmiInstance;
    UINT8                               DataSize;
    EFI_STATUS                          Status;
    UINT8                               Seq = 0;

    if (IpmiTransportlocked (This)) {
        return EFI_NOT_READY;
    } else {
        IpmiTransportAcquireLock (This);
    }

    if (!ResponseDataSize || (!ResponseData && *ResponseDataSize)) {
        IpmiTransportReleaseLock (This);
        return EFI_INVALID_PARAMETER;
    }
 
    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    IpmiInstance->TempData[0] = 0x03 + CommandDataSize;//netfn+cmd+seq+data
    IpmiInstance->TempData[1] = (UINT8) ((NetFunction << 2) | (Lun & 0xfc));
    IpmiInstance->TempData[2] = Seq;
    IpmiInstance->TempData[3] = Command;

    if (CommandDataSize > 0) {
        if (CommandData == NULL) {
            IpmiTransportReleaseLock (This);
            return EFI_INVALID_PARAMETER;
        }
        if (CommandDataSize <= (MAX_TEMP_DATA - 4) ) {
            CopyMem (&IpmiInstance->TempData[4], CommandData, CommandDataSize);
        } else {
            IpmiTransportReleaseLock (This);
            return EFI_BAD_BUFFER_SIZE;
        }
    }

#if (COMMAND_SPECIFIC_COUNTER_UPDATE_SUPPORT == 1)
    //
    // Update Command Specific Delay Counter
    //
    IpmiInstance->Interface.BT.BtDelay = UpdateCommandSpecificDelay(NetFunction,Command);
    SERVER_IPMI_DEBUG((EFI_D_INFO,"Counter value is %x\n",IpmiInstance->Interface.BT.BtDelay));
#endif

    if ((Status = SendDataToBtBmcPort (IpmiInstance, Context, IpmiInstance->TempData,
                                   (CommandDataSize + 4))) != EFI_SUCCESS ) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }
    DataSize = MAX_TEMP_DATA;
    if ((Status = ReceiveBmcDataFromBtPort (IpmiInstance, Context,
                IpmiInstance->TempData, &DataSize)) != EFI_SUCCESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }

    if (CompletionCode != NULL) {
        *CompletionCode = IpmiInstance->TempData[4];
    }

    This->CommandCompletionCode = IpmiInstance->TempData[4];
    if (ERROR_COMPLETION_CODE(This->CommandCompletionCode)) {
        UpdateSoftErrorCount (This->CommandCompletionCode, IpmiInstance);
        // Write completion code into return buffer if an IPMI command returns an error
        if (*ResponseDataSize) {
            *ResponseData = This->CommandCompletionCode;
            *ResponseDataSize = 1;
        }
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }

    if (DataSize < 4) {
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }
#if SMCPKG_SUPPORT
    if((UINT8)(DataSize - 4) > *ResponseDataSize){
#else
    if ((DataSize - 4) > *ResponseDataSize) {
#endif
        *ResponseDataSize = (UINT8) (DataSize - 4);
        IpmiTransportReleaseLock (This);
        return EFI_BUFFER_TOO_SMALL;
    }

    //
    // Copying the response data into ResponseData buffer
    //
    CopyMem (ResponseData, &IpmiInstance->TempData[5], (DataSize - 4));
    *ResponseDataSize = (UINT8) (DataSize - 4);

    IpmiTransportReleaseLock (This);
    return EFI_SUCCESS;
}

/**
    IPMI send command API for BT interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
EFIAPI
EfiBtIpmiSendCommand (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize )
{
    return IpmiBtSendCommand (
            This,
            NetFunction,
            Lun,
            Command,
            CommandData,
            CommandDataSize,
            ResponseData,
            ResponseDataSize,
            NULL,
            NULL );
}

/**
    IPMI send command Extension API for BT interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
EFIAPI
EfiBtIpmiSendCommandEx (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize,
  OUT UINT8             *CompletionCode )
{
    return IpmiBtSendCommand (
            This,
            NetFunction,
            Lun,
            Command,
            CommandData,
            CommandDataSize,
            ResponseData,
            ResponseDataSize,
            CompletionCode,
            NULL );
}

/**
    KCS interface IPMI send command Implementation

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC
    @param  Context - NULL here

    @return EFI_STATUS Status returned from BMC while executing the command.

**/
#if SMCPKG_SUPPORT
UINT16	ShortFixedDelay;
#endif

EFI_STATUS
IpmiSendCommand (
  IN EFI_IPMI_TRANSPORT               *This,
  IN UINT8                            NetFunction,
  IN UINT8                            Lun,
  IN UINT8                            Command,
  IN UINT8                            *CommandData,
  IN UINT8                            CommandDataSize,
  OUT UINT8                           *ResponseData,
  OUT UINT8                           *ResponseDataSize,
  OUT UINT8                           *CompletionCode,
  IN VOID                             *Context )
{
    EFI_IPMI_BMC_INSTANCE_DATA          *IpmiInstance;
    UINT8                               DataSize;
    EFI_STATUS                          Status;

    if (IpmiTransportlocked (This)) {
        return EFI_NOT_READY;
    } else {
        IpmiTransportAcquireLock (This);
    }

#if SMCPKG_SUPPORT
    ShortFixedDelay = 0;
    if((NetFunction == 0x30) && (Command == 0xA0)){
    	if((CommandData[0] >= 0x09) && (CommandData[0] <= 0x28))
    	    ShortFixedDelay = IPMI_KCS_SHORT_DELAY_PER_RETRY;
    }
#endif

    if (!ResponseDataSize || (!ResponseData && *ResponseDataSize)) {
        IpmiTransportReleaseLock (This);
        return EFI_INVALID_PARAMETER;
    }

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    if (IpmiInstance->SoftErrorCount >= MAX_BMC_CMD_FAIL_COUNT) {
        IpmiTransportReleaseLock (This);
        return EFI_NOT_READY;
    }

    IpmiInstance->TempData[0] = (UINT8) ((NetFunction << 2) | (Lun & 0x03));
    IpmiInstance->TempData[1] = Command;
    if (CommandDataSize > 0) {
        if (CommandData == NULL) {
            IpmiTransportReleaseLock (This);
            return EFI_INVALID_PARAMETER;
        }
        if (CommandDataSize <= (MAX_TEMP_DATA - 2)) {
            CopyMem (&IpmiInstance->TempData[2], CommandData, CommandDataSize);
        } else {
            IpmiTransportReleaseLock (This);
            return EFI_BAD_BUFFER_SIZE;
        }
    }

#if (COMMAND_SPECIFIC_COUNTER_UPDATE_SUPPORT == 1)
    //
    // Update Command Specific Retry Counter
    //
    IpmiInstance->Interface.KCS.KcsRetryCounter = UpdateCommandSpecificDelay (NetFunction, Command);
    SERVER_IPMI_DEBUG((EFI_D_INFO,"Counter value is %x\n",IpmiInstance->Interface.KCS.KcsRetryCounter));
#endif

    if ((Status = SendDataToBmcPort (IpmiInstance, Context, IpmiInstance->TempData,
                                (CommandDataSize + 2))) != EFI_SUCCESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }
    DataSize = MAX_TEMP_DATA;
    if ((Status = ReceiveBmcDataFromPort (IpmiInstance, Context,
                IpmiInstance->TempData, &DataSize)) != EFI_SUCCESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }

    if (CompletionCode != NULL) {
        *CompletionCode = IpmiInstance->TempData[2];
    }
	
	This->CommandCompletionCode = IpmiInstance->TempData[2];
    if (ERROR_COMPLETION_CODE(This->CommandCompletionCode)) {
        UpdateSoftErrorCount (This->CommandCompletionCode, IpmiInstance);
        // Write completion code into return buffer if an IPMI command returns an error
        if (*ResponseDataSize) {
            *ResponseData = This->CommandCompletionCode;
            *ResponseDataSize = 1;
        }
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }

    if (DataSize < 3) {
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }
#if SMCPKG_SUPPORT
    if((UINT8)(DataSize - 3) > *ResponseDataSize){
#else
    if ((DataSize - 3) > *ResponseDataSize) {
#endif
        *ResponseDataSize = (UINT8) (DataSize - 3);
        IpmiTransportReleaseLock (This);
        return EFI_BUFFER_TOO_SMALL;
    }

    //
    // Copying the response data into ResponseData buffer
    //
    CopyMem (ResponseData, &IpmiInstance->TempData[3], (DataSize - 3));
    *ResponseDataSize = (UINT8) (DataSize - 3);

    IpmiTransportReleaseLock (This);
    return EFI_SUCCESS;
}

/**
    IPMI send command API for KCS interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
EFIAPI
EfiIpmiSendCommand (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize )
{
    return IpmiSendCommand (
            This,
            NetFunction,
            Lun,
            Command,
            CommandData,
            CommandDataSize,
            ResponseData,
            ResponseDataSize,
            NULL,
            NULL );
}

/**
    IPMI send command Extension API for KCS interface with Completion Code

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
EFIAPI
EfiIpmiSendCommandEx (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize,
  OUT UINT8             *CompletionCode )
{
  return IpmiSendCommand (
          This,
          NetFunction,
          Lun,
          Command,
          CommandData,
          CommandDataSize,
          ResponseData,
          ResponseDataSize,
          CompletionCode,
          NULL );
}

/**
    IPMB interface IPMI send command Implementation

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC
    @param  Context - NULL here

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
IpmiIpmbSendCommand (
   IN  EFI_IPMI_TRANSPORT    *This,
   IN  UINT8                 NetFunction,
   IN  UINT8                 Lun,
   IN  UINT8                 Command,
   IN  UINT8                 *CommandData,
   IN  UINT8                 CommandDataSize,
   OUT UINT8                 *ResponseData,
   OUT UINT8                 *ResponseDataSize,
   OUT UINT8                 *CompletionCode,
   IN  VOID                  *Context )
{
    EFI_IPMI_BMC_INSTANCE_DATA       *IpmiInstance;
    UINT8                            DataSize;
    EFI_STATUS                       Status;
    UINT8                            DataSum = 0;
    UINT8                            CheckSum = 0;
    UINT8                            i;

    if (IpmiTransportlocked (This)) {
        return EFI_NOT_READY;
    } else {
        IpmiTransportAcquireLock (This);
    }

    if (!ResponseDataSize || (!ResponseData && *ResponseDataSize)) {
        IpmiTransportReleaseLock (This);
        return EFI_INVALID_PARAMETER;
    }

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    if (IpmiInstance->SoftErrorCount >= MAX_BMC_CMD_FAIL_COUNT) {
        IpmiTransportReleaseLock (This);
        return EFI_NOT_READY;
    }

    //
    // Request Packet format.
    // | Slave Address | Netfun/Lun | CheckSum (Check Sum of previous data)
    // | Slave Address | Seq No | Command | Data 1..N | CheckSum (Check Sum of previous data)
    //
    IpmiInstance->TempData[0] = IPMI_BMC_SLAVE_ADDRESS;
    IpmiInstance->TempData[1] = (UINT8) ((NetFunction << 2) | (Lun & 0x03));

    DataSum += IpmiInstance->TempData[1] + IPMI_BMC_SLAVE_ADDRESS;
    CheckSum = (UINT8) (0x100 - DataSum);
    DataSum = 0;

    IpmiInstance->TempData[2] = CheckSum;

    IpmiInstance->TempData[3] = IPMI_BMC_SLAVE_ADDRESS;
    DataSum += IPMI_BMC_SLAVE_ADDRESS;

    IpmiInstance->TempData[4] = SEQ_NO;
    DataSum += SEQ_NO;

    IpmiInstance->TempData[5] = Command;
    DataSum += Command;

    if (CommandDataSize > 0) {
        if (CommandData == NULL) {
            IpmiTransportReleaseLock (This);
            return EFI_INVALID_PARAMETER;
        }
        if (CommandDataSize <= (MAX_TEMP_DATA - 6)) {
            CopyMem (&IpmiInstance->TempData[6], CommandData, CommandDataSize);
        } else {
           IpmiTransportReleaseLock (This);
           return EFI_BAD_BUFFER_SIZE;
        }

        for(i = 0; i < CommandDataSize; i++) {
            DataSum += IpmiInstance->TempData[6 + i];
        }
        CheckSum = (UINT8) (0x100 - DataSum);  // Find the checksum for the packing data.
        IpmiInstance->TempData[6 + CommandDataSize] = CheckSum;  // update the check sum.
    } else {
        CheckSum = (UINT8) (0x100 - DataSum);  // Find the checksum for the packing data.
        IpmiInstance->TempData[6] = CheckSum;  // update the check sum.
    }

    if ((Status = SendDataToIpmbBmcPort (IpmiInstance, Context, IpmiInstance->TempData,
                (UINT8) (CommandDataSize + 7))) != EFI_SUCCESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }

    DataSize = MAX_TEMP_DATA;
    if ((Status = ReceiveBmcDataFromIpmbPort (IpmiInstance, Context,
                IpmiInstance->TempData, &DataSize)) != EFI_SUCCESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }

    //
    // Response Packet format.
    // | Slave Address | Netfun/Lun | CheckSum (Check Sum of previous data)
    // | Slave Address | Seq No | Command | Completion code| Data 1..N | CheckSum (Check Sum of previous data)
    //

    //
    // Calculate the check sum.
    //
    Status = CheckDataValidity (IpmiInstance->TempData, DataSize);
    if (EFI_ERROR(Status)) { 
        IpmiTransportReleaseLock (This);
        return Status;
    }

    if (CompletionCode != NULL) {
        *CompletionCode = IpmiInstance->TempData[6];
    }

    This->CommandCompletionCode = IpmiInstance->TempData[6];
    if (ERROR_COMPLETION_CODE(This->CommandCompletionCode)) {
        UpdateSoftErrorCount (This->CommandCompletionCode, IpmiInstance);
        // Write completion code into return buffer if an IPMI command returns an error
        if (*ResponseDataSize) {
            *ResponseData = This->CommandCompletionCode;
            *ResponseDataSize = 1;
        }
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }

    if ( DataSize < 8) {
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }
    if ((DataSize - 8) > *ResponseDataSize) {
        *ResponseDataSize = (UINT8) (DataSize - 8);
        IpmiTransportReleaseLock (This);
        return EFI_BUFFER_TOO_SMALL;
    }


    //
    // Copying the response data into ResponseData buffer
    //
    CopyMem (ResponseData, &IpmiInstance->TempData[7], (DataSize - 8));
    *ResponseDataSize = (UINT8) (DataSize - 8);

    IpmiTransportReleaseLock (This);
    return EFI_SUCCESS;
}

/**
    IPMI send command API for IPMB interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data

    @return EFI_STATUS Status returned from BMC while executing the command.

**/
EFI_STATUS
EFIAPI
EfiIpmbIpmiSendCommand (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize )
{
  return IpmiIpmbSendCommand (
          This,
          NetFunction,
          Lun,
          Command,
          CommandData,
          CommandDataSize,
          ResponseData,
          ResponseDataSize,
          NULL,
          NULL
          );
}

/**
    IPMI send command Extension API for IPMB interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC

    @return EFI_STATUS Status returned from BMC while executing the command.

**/
EFI_STATUS
EFIAPI
EfiIpmbIpmiSendCommandEx (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize,
  OUT UINT8             *CompletionCode )
{
  return IpmiIpmbSendCommand (
          This,
          NetFunction,
          Lun,
          Command,
          CommandData,
          CommandDataSize,
          ResponseData,
          ResponseDataSize,
          CompletionCode,
          NULL
          );
}

/**
    SSIF interface IPMI send command Implementation

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC
    @param  Context - NULL here

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
IpmiSsifSendCommand (
  IN EFI_IPMI_TRANSPORT               *This,
  IN UINT8                            NetFunction,
  IN UINT8                            Lun,
  IN UINT8                            Command,
  IN UINT8                            *CommandData,
  IN UINT8                            CommandDataSize,
  OUT UINT8                           *ResponseData,
  OUT UINT8                           *ResponseDataSize,
  OUT UINT8                           *CompletionCode,
  IN VOID                             *Context )
{
    EFI_IPMI_BMC_INSTANCE_DATA          *IpmiInstance;
    UINT8                               DataSize = 0;
    EFI_STATUS                          Status;
    
    if (IpmiTransportlocked (This)) {
        return EFI_NOT_READY;
    } else {
        IpmiTransportAcquireLock (This);
    }

    if (!ResponseDataSize || (!ResponseData && *ResponseDataSize)) {
        IpmiTransportReleaseLock (This);
        return EFI_INVALID_PARAMETER;
    }
    
    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    //
    // Check the SSIF interface multi-part reads/writes supported.
    // Block length include Command data, NetFn, and Command parameter
	//
    if ( ( (IpmiInstance->Interface.SSIF.RwSupport == SinglePartRw) && \
           ((CommandDataSize + 2) > SMBUS_BLOCK_LENGTH) ) || \
           ( (IpmiInstance->Interface.SSIF.RwSupport == MultiPartRw) && \
           ((CommandDataSize + 2) > (2*SMBUS_BLOCK_LENGTH)) ) || \
           ( (IpmiInstance->Interface.SSIF.RwSupport == MultiPartRwWithMiddle) && \
           ((CommandDataSize + 2) > MAX_TEMP_DATA) ) ) {
        IpmiTransportReleaseLock (This);
        return EFI_INVALID_PARAMETER;
    }

    if (IpmiInstance->SoftErrorCount >= MAX_BMC_CMD_FAIL_COUNT) {
        IpmiTransportReleaseLock (This);
        return EFI_NOT_READY;
    }

    IpmiInstance->TempData[0] = (UINT8) ((NetFunction << 2) | (Lun & 0x03));
    IpmiInstance->TempData[1] = Command;
    if (CommandDataSize > 0) {
        if (CommandData == NULL) {
            IpmiTransportReleaseLock (This);
            return EFI_INVALID_PARAMETER;
        }
        if (CommandDataSize <= (MAX_TEMP_DATA - 2) ) {
            CopyMem (&IpmiInstance->TempData[2], CommandData, CommandDataSize);
        } else {
            IpmiTransportReleaseLock (This);
            return EFI_BAD_BUFFER_SIZE;
        }
    }

    if ((Status = SendDataToSsifBmcPort (IpmiInstance, Context, IpmiInstance->TempData,
                                         (CommandDataSize + 2))) != EFI_SUCCESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }

    //
    // Hook to check smbus alert pin.
    //
    if (IpmiInstance->Interface.SSIF.SmbAlertSupport) {
        if (!CheckAlertPinHook()) {
            IpmiTransportReleaseLock (This);
            return EFI_DEVICE_ERROR;
        }
    } else {
        MicroSecondDelay(IPMI_SSIF_WAIT_RESPONSE_DATA_DELAY);
    }

    DataSize = MAX_SSIF_TEMP_DATA;
    if ((Status = ReceiveBmcDataFromSsifPort (IpmiInstance, Context,
                  IpmiInstance->TempData, &DataSize)) != EFI_SUCCESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        IpmiTransportReleaseLock (This);
        return Status;
    }

    if (CompletionCode != NULL) {
        *CompletionCode = IpmiInstance->TempData[2];
    }
    
    This->CommandCompletionCode = IpmiInstance->TempData[2];
    
    if (ERROR_COMPLETION_CODE(This->CommandCompletionCode)) {
        UpdateSoftErrorCount (This->CommandCompletionCode, IpmiInstance);
        // Write completion code into return buffer if an IPMI command returns an error
        if (*ResponseDataSize) {
            *ResponseData = This->CommandCompletionCode;
            *ResponseDataSize = 1;
        }
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }

    if (DataSize < 3) {
        IpmiTransportReleaseLock (This);
        return EFI_DEVICE_ERROR;
    }

    if ((DataSize - 3) > *ResponseDataSize) {
        *ResponseDataSize = (UINT8) (DataSize - 3);
        IpmiTransportReleaseLock (This);
        return EFI_BUFFER_TOO_SMALL;
    }

    //
    // Copying the response data into ResponseData buffer
    //
    CopyMem (ResponseData, &IpmiInstance->TempData[3], (DataSize - 3));
    *ResponseDataSize = (UINT8) (DataSize - 3);

    IpmiTransportReleaseLock (This);
    return EFI_SUCCESS;
}

/**
    IPMI send command API for SSIF interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data

    @return EFI_STATUS Status returned from BMC while executing the command.

**/
EFI_STATUS
EFIAPI
EfiSsifIpmiSendCommand (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize )
{
  return  IpmiSsifSendCommand (
          This,
          NetFunction,
          Lun,
          Command,
          CommandData,
          CommandDataSize,
          ResponseData,
          ResponseDataSize,
          NULL,
          NULL
          );
}

/**
    IPMI send command Extension API for SSIF interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to BMC
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from BMC
    @param  ResponseDataSize - Size of the Response data
    @param  CompletionCode - Completion Code from BMC

    @return EFI_STATUS Status returned from BMC while executing the command.

**/

EFI_STATUS
EFIAPI
EfiSsifIpmiSendCommandEx (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize,
  OUT UINT8             *CompletionCode )
{
    return IpmiSsifSendCommand (
            This,
            NetFunction,
            Lun,
            Command,
            CommandData,
            CommandDataSize,
            ResponseData,
            ResponseDataSize,
            CompletionCode,
            NULL );
}

/**
    IPMI BMC status Implementation

    @param  This - Pointer to IPMI Protocol/PPI
    @param  BmcStatus - State of the BMC
    @param  ComAddress - BMC Communication address
    @param  Context - NULL here

    @retval EFI_STATUS EFI_SUCCESS Status returned

**/

EFI_STATUS
IpmiBmcStatus (
  IN EFI_IPMI_TRANSPORT               *This,
  OUT EFI_BMC_STATUS                  *BmcStatus,
  OUT EFI_SM_COM_ADDRESS              *ComAddress,
  IN  VOID                            *Context )
{
    EFI_STATUS                            Status;
    UINT8                                 DataSize;
    IPMI_GET_DEVICE_ID_RESPONSE           GetDeviceIdResponse;
    EFI_IPMI_BMC_INSTANCE_DATA            *IpmiInstance;

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

    DataSize = sizeof (GetDeviceIdResponse);

    //
    // Get the device ID information for the BMC.
    //
    Status = This->SendIpmiCommand (
                &IpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_DEVICE_ID,
                NULL,
                0,
                (UINT8*)&GetDeviceIdResponse,
                &DataSize );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Device ID Command: Status %r\n", Status));
    //
    // If there is no error then proceed to check the data returned by the BMC
    //
    if (!EFI_ERROR(Status)) {
        //
        // If the controller is in Update Mode then set the BMC Status to indicate
        // the BMC is in force update mode.
        //
        if (GetDeviceIdResponse.UpdateMode != 0) {
            IpmiInstance->BmcStatus = EFI_BMC_UPDATE_IN_PROGRESS;
        } else {
            IpmiInstance->BmcStatus = EFI_BMC_OK;
        }
    } else {
        if (IpmiInstance->SoftErrorCount >= MAX_SOFT_COUNT) {
            IpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
        } else {
            IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        }
    }
    *BmcStatus = IpmiInstance->BmcStatus;
    ComAddress->ChannelType = EfiSmBmc;
    ComAddress->Address.BmcAddress.SlaveAddress = IpmiInstance->SlaveAddress;
    ComAddress->Address.BmcAddress.ChannelAddress = 0x0;

    return EFI_SUCCESS;
}

/**
    IPMI BMC status API

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  BmcStatus - State of the BMC
    @param  ComAddress - BMC Communication address

    @return EFI_STATUS
    @retval EFI_SUCCESS Status returned

**/

EFI_STATUS
EFIAPI
EfiIpmiBmcStatus (
  IN EFI_IPMI_TRANSPORT               *This,
  OUT EFI_BMC_STATUS                  *BmcStatus,
  OUT EFI_SM_COM_ADDRESS              *ComAddress )
{
  return IpmiBmcStatus (This, BmcStatus, ComAddress, NULL);
}

/**
    Read 8 bit data from BMC port based on access type.

    @param AccessType  - Specifies MMIO or IO access.
    @param Address     - Specifies Address to read.

    @return Data read.
*/

UINT8
BmcRead8 (
  IN UINT8  AccessType,
  IN UINTN  Address
)
{
    if (AccessType == IPMI_IO_ACCESS) {
        return IoRead8 (Address);
    } else {
        return MmioRead8 (Address);
    }
}

/**
    Write 8 bit data to BMC port based on access type.

    @param AccessType  - Specifies MMIO or IO access.
    @param Address     - Specifies Address to write.
    @param Data        - Specifies data to be written.

    @return Data written.
*/

UINT8
BmcWrite8 (
  IN UINT8  AccessType,
  IN UINTN  Address,
  IN UINT8  Data
)
{
    if (AccessType == IPMI_IO_ACCESS) {
        return IoWrite8 (Address, Data);
    } else {
        return MmioWrite8 (Address, Data);
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
