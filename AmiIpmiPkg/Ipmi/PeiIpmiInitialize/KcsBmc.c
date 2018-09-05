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

/** @file KcsBmc.c
    KCS Transport Hook.

**/

//---------------------------------------------------------------------------

#include "IpmiBmc.h"
#include "KcsBmc.h"

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#define KCS_RETRY_COUNTER    IpmiInstance->Interface.KCS.KcsRetryCounter

//---------------------------------------------------------------------------

/**
    KCS error exit function implementation based on the Flow chart given in
    IPMI specification.

    @param IpmiInstance Data structure describing BMC
                        variables and used for sending commands
    @param Context NULL here

    @return EFI_STATUS Return Status

**/

EFI_STATUS
KcsErrorExit (
  IN  EFI_IPMI_BMC_INSTANCE_DATA    *IpmiInstance,
  IN  VOID                          *Context )
{
    UINT8                   KcsData;
    EFI_KCS_STATUS          KcsStatus;
    UINT8                   RetryCount;
    UINT32                  Counter;
    UINTN                   KcsDataPort;
    UINTN                   KcsCommandPort;

    //
    // Update KcsDataPort and KcsCommandPort based on IpmiInstance->AccessType.
    //
    if (IpmiInstance->AccessType == IPMI_IO_ACCESS) {
        KcsDataPort = (UINTN) IpmiInstance->Interface.KCS.DataPort;
        KcsCommandPort = (UINTN) IpmiInstance->Interface.KCS.CommandPort;
    } else {
        KcsDataPort = IpmiInstance->MmioBaseAddress;
        KcsCommandPort = KcsDataPort + IpmiInstance->BaseAddressRange;
    }

    //
    // Flow chart: Aborting KCS Transactions in-progress and/or Retrieving KCS Error Status
    // Error exit Flow chart: Check for RETRY LIMIT.
    //
    RetryCount = 0;
    while (RetryCount < KCS_ABORT_RETRY_COUNT) {
        //
        // Error exit Flow chart: wait for IBF=0
        //
        Counter =  KCS_RETRY_COUNTER;
        while ( TRUE ) {
            KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
            if ( !KcsStatus.Status.Ibf ) {
                break;
            }
            if ( Counter-- == 0 ) {
                return EFI_DEVICE_ERROR;
            }
            MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
        } //while loop: IBF flag

        //
        // Error exit Flow chart: GET_STATUS/ABORT to CMD; phase = error1
        //
        KcsData = KCS_ABORT;
        BmcWrite8 (IpmiInstance->AccessType, KcsCommandPort, KcsData);

        //
        // Error exit Flow chart: wait for IBF=0
        //
        Counter = KCS_RETRY_COUNTER;
        while ( TRUE ) {
            KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
            if ( !KcsStatus.Status.Ibf ) {
                break;
            }
            if ( Counter-- == 0 ) {
                return EFI_DEVICE_ERROR;
            }
            MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
        }//while loop: IBF flag

        //
        // Error exit Flow chart: clear OBF
        //
        KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsDataPort);

        //
        // Error exit Flow chart: 00h to DATA_IN; phase = error2
        //
        KcsData = 0x0;
        BmcWrite8 (IpmiInstance->AccessType, KcsDataPort, KcsData);

        //
        // Error exit Flow chart: wait for IBF=0
        //
        Counter = KCS_RETRY_COUNTER; 
        while ( TRUE ) {
            KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
            if ( !KcsStatus.Status.Ibf ) {
                break;
            }
            if( Counter-- == 0 ) {
                return EFI_DEVICE_ERROR;
            }
            MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
        } //while loop: IBF flag

        //
        // Check for KCS State
        //
        if (KcsStatus.Status.State == KcsReadState) {

            //
            // Error exit Flow chart: KCS is in READ_STEATE so wait for OBF=1
            //
            Counter = KCS_RETRY_COUNTER;
            while ( TRUE ) {
                KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
                if ( KcsStatus.Status.Obf ) {
                    break;
                }
                if ( Counter-- == 0 ) {
                    return EFI_DEVICE_ERROR;
                }
                MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
            }  // while loop: OBF flag

            //
            // Error exit Flow chart: Read error status code byte from DATA_OUT
            //
            BmcRead8 (IpmiInstance->AccessType, KcsDataPort);

            //
            // Error exit Flow chart: Write READ dummy byte to DATA_IN; phase = error3
            //
            KcsData = KCS_READ;
            BmcWrite8 (IpmiInstance->AccessType, KcsDataPort, KcsData);

            //
            // Error exit Flow chart: wait for IBF=0
            //
            Counter = KCS_RETRY_COUNTER;
            while ( TRUE ) {
                KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
                if ( !KcsStatus.Status.Ibf ) {
                    break;
                }
                if ( Counter-- == 0 ) {
                    return EFI_DEVICE_ERROR;
                }
                MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
            } // while loop: IBF flag

            //
            // Check for KCS State
            //
            if (KcsStatus.Status.State == KcsIdleState) {

                //
                // Error exit Flow chart: KCS is in IDLE_STEATE so wait for OBF=1
                //
                Counter = KCS_RETRY_COUNTER;
                while ( TRUE ) {
                    KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
                    if ( KcsStatus.Status.Obf ) {
                        break;
                    }
                    if ( Counter-- == 0 ) {
                        return EFI_DEVICE_ERROR;
                    }
                    MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
                }  //while loop: OBF flag

                //
                // Error exit Flow chart: clear OBF
                //
                KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsDataPort);

                //
                // Error exit Flow chart: phase = idle
                // return EFI_ABORTED
                //
                return EFI_ABORTED;

            } else {  //if (KcsStatus.Status.State == KcsIdleState)

                //
                // Error exit Flow chart: KCS is not in IDLE_STATE so increment retry count
                // and continue
                //
                RetryCount++;
                continue;
            }

        } else {  //if (KcsStatus.Status.State == KcsReadState)

            //
            // Error exit Flow chart: KCS is not in READ_STATE so increment retry count
            // and continue
            //
            RetryCount++;
            continue;
        }  //else loop
    }  //While loop for Retry count

    //
    // Control comes here only when retry limit is exceeded so
    // Error exit Flow chart: return Communication Failure
    //
    return EFI_DEVICE_ERROR;
}

/**
    Checks for the Status BMC KCS interface

    @param IpmiInstance Data structure describing BMC
        variables and used for sending commands
    @param KcsState State of the BMC
    @param Idle Signifies whether BMC is in 
        IDLE State or not
    @param Context NULL here

    @return EFI_STATUS Return Status

**/

EFI_STATUS
KcsCheckStatus (
  IN  EFI_IPMI_BMC_INSTANCE_DATA     *IpmiInstance,
  IN  KCS_STATE                       KcsState,
  IN  BOOLEAN                         *Idle,
  IN  VOID                            *Context )
{
    EFI_KCS_STATUS          KcsStatus;
    UINT32                  Counter;
    UINTN                   KcsDataPort;
    UINTN                   KcsCommandPort;

    *Idle = FALSE;

    //
    // Update KcsDataPort and KcsCommandPort based on IpmiInstance->AccessType.
    //
    if (IpmiInstance->AccessType == IPMI_IO_ACCESS) {
        KcsDataPort = (UINTN) IpmiInstance->Interface.KCS.DataPort;
        KcsCommandPort = (UINTN) IpmiInstance->Interface.KCS.CommandPort;
    } else {
        KcsDataPort = IpmiInstance->MmioBaseAddress;
        KcsCommandPort = KcsDataPort + IpmiInstance->BaseAddressRange;
    }

    //
    // Write/Read Transfer Flow Chart: wait for IBF=0
    //
    Counter = KCS_RETRY_COUNTER;
    while ( TRUE ) {
        KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
        if ( !KcsStatus.Status.Ibf ) {
            break;
        }
        if ( Counter-- == 0 ) {
            return EFI_DEVICE_ERROR;
        }
        MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
    } //while loop: IBF flag

    //
    // Write Transfer Flow Chart:
    //
    if (KcsState == KcsWriteState) {

        //
        // Write Transfer Flow Chart: Check whether KCS is in WRITE_STATE or not.
        //
        if (KcsStatus.Status.State== KcsWriteState) {

            //
            // KCS is in WRITE_STATE so
            // Write Transfer Flow Chart: Clear OBF; phase=wr_data
            // and return EFI_SUCCESS
            //
            BmcRead8 (IpmiInstance->AccessType, KcsDataPort);
            return EFI_SUCCESS;
        } else {

            //
            // KCS is not in WRITE_STATE so
            // Write Transfer Flow Chart: Error Exit
            //

            return KcsErrorExit (IpmiInstance, Context);
        }  //if (KcsStatus.Status.State == KcsWriteState)
    }  //if ( KcsState == KcsWriteState )

    //
    // Read Transfer Flow Chart:
    //
    if (KcsState == KcsReadState) {

        //
        // Read Transfer Flow Chart:
        // If KCS is neither in READ_STATE nor in IDLE_STATE then call error exit path.
        //
        if ( (KcsStatus.Status.State != KcsReadState) && \
            (KcsStatus.Status.State != KcsIdleState) ) {
            return KcsErrorExit (IpmiInstance, Context);
        } else {

            //
            // Read Transfer Flow Chart: wait for OBF=1
            //
            Counter = KCS_RETRY_COUNTER;
            while ( TRUE ) {
                KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
                if ( KcsStatus.Status.Obf ) {
                    break;
                }
                if ( Counter-- == 0 ) {
                    return EFI_DEVICE_ERROR;
                }
                MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
            }  //while loop: OBF flag

            //
            // Read Transfer Flow Chart: Read dummy data byte from DATA_OUT
            // If KCS is in IDLE_STATE after waiting for OBF==1 then Read dummy data
            // byte from DATA_OUT
            //
            if ( KcsStatus.Status.State == KcsIdleState ) {
                BmcRead8 (IpmiInstance->AccessType, KcsDataPort);

                //
                // Read Transfer Flow Chart: phase = idle
                //
                *Idle = TRUE;
            }  //if ( KcsStatus.Status.State == KcsIdleState )

        }  //else loop

    }  //if ( KcsState == KcsReadState )

    return EFI_SUCCESS;
}

/**
    Sends the command to BMC port

    @param IpmiInstance Data structure describing BMC
        variables and used for sending commands
    @param Context NULL here
    @param Data Pointer to command data that will be sent to BMC 
        along with Command
    @param DataSize Size of the command data

    @return EFI_STATUS Status of the Sending data to BMC port

**/

EFI_STATUS
SendDataToBmcPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance,
  IN  VOID                            *Context,
  IN  UINT8                           *Data,
  IN  UINT8                           DataSize )
{
    EFI_KCS_STATUS          KcsStatus;
    UINT8                   KcsData;
    EFI_STATUS              Status;
    UINT8                   i;
    BOOLEAN                 Idle;
    UINT32                  Counter;
    UINTN                   KcsDataPort;
    UINTN                   KcsCommandPort;

    //
    // Update KcsDataPort and KcsCommandPort based on IpmiInstance->AccessType.
    //
    if (IpmiInstance->AccessType == IPMI_IO_ACCESS) {
        KcsDataPort = (UINTN) IpmiInstance->Interface.KCS.DataPort;
        KcsCommandPort = (UINTN) IpmiInstance->Interface.KCS.CommandPort;
    } else {
        KcsDataPort = IpmiInstance->MmioBaseAddress;
        KcsCommandPort = KcsDataPort + IpmiInstance->BaseAddressRange;
    }

    //
    // Write Transfer Flow Chart: Wait for IBF=0
    //
    Counter = KCS_RETRY_COUNTER;
    while ( TRUE ) {
        KcsStatus.RawData = BmcRead8 (IpmiInstance->AccessType, KcsCommandPort);
        if ( !(KcsStatus.Status.Ibf) ) {
            break;
        }
        if ( Counter-- == 0 ) {
            return EFI_DEVICE_ERROR;
        }
        MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
    } // while loop: IBF flag

    //
    // Write Transfer Flow Chart: Clear OBF
    //
    BmcRead8 (IpmiInstance->AccessType, KcsDataPort);

    //
    // Write Transfer Flow Chart: WR_START to CMD phase=wr_start
    //
    KcsData = KCS_WRITE_START;
    BmcWrite8 (IpmiInstance->AccessType, KcsCommandPort, KcsData);

    //
    // Check for KCS status
    //
    if ((Status = KcsCheckStatus (IpmiInstance, KcsWriteState, &Idle, Context)) != EFI_SUCCESS) {
        return Status;
    }

    //
    // Send the data to BMC
    //
    for (i = 0; i < DataSize; i++) {

        if (i < (DataSize - 1)) {

            //
            // Write Transfer Flow Chart: data byte to DATA
            //
            BmcWrite8 (IpmiInstance->AccessType, KcsDataPort, Data[i]);

            //
            // Check for KCS status
            //
            if ((Status = KcsCheckStatus (IpmiInstance, KcsWriteState, &Idle, Context)) != EFI_SUCCESS) {
                return Status;
            }
        } else {

            //
            // Write Transfer Flow Chart: This is Last write byte so WR_END to CMD; phase=wr_end_cmd
            //
            KcsData = KCS_WRITE_END;
            BmcWrite8 (IpmiInstance->AccessType, KcsCommandPort, KcsData);

            //
            // Check for KCS status
            //
            if ((Status = KcsCheckStatus (IpmiInstance, KcsWriteState, &Idle, Context)) != EFI_SUCCESS) {
                return Status;
            }

            //
            // Write Transfer Flow Chart: data byte to DATA
            //
            BmcWrite8 (IpmiInstance->AccessType, KcsDataPort, Data[i]);
        }
    }  //for (i = 0; i < DataSize; i++)
    return EFI_SUCCESS;
}

/**
    Receives the Data from BMC port

    @param IpmiInstance Data structure describing BMC
            variables and used for sending commands
    @param Context NULL here
    @param Data Pointer to Response data that is received from BMC
    @param DataSize Size of the Response data

    @return EFI_STATUS Status of the Receiving data from BMC port

**/

EFI_STATUS
ReceiveBmcDataFromPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance,
  IN  VOID                            *Context,
  OUT UINT8                           *Data,
  OUT UINT8                           *DataSize )
{
    UINT8                   KcsData;
    EFI_STATUS              Status;
    BOOLEAN                 Idle;
    UINT8                   Count;
    UINTN                   KcsDataPort;

    Count = 0;

    //
    // Update KcsDataPort based on IpmiInstance->AccessType.
    //
    if (IpmiInstance->AccessType == IPMI_IO_ACCESS) {
        KcsDataPort = (UINTN) IpmiInstance->Interface.KCS.DataPort;
    } else {
        KcsDataPort = IpmiInstance->MmioBaseAddress;
    }

    while (TRUE) {

        //
        // Check for KCS status
        //
        if ((Status = KcsCheckStatus (IpmiInstance, KcsReadState, &Idle, Context)) != EFI_SUCCESS) {
            return Status;
        }

        //
        // If KCS in IDLE_STATE that means all the data byte have been read
        // so update Data size and return EFI_SUCCESS
        //
        if (Idle) {
            *DataSize = Count;
            return EFI_SUCCESS;
        }

        //
        // Read Transfer Flow Chart: Read data byte from DATA_OUT
        //
        Data[Count] = BmcRead8 (IpmiInstance->AccessType, KcsDataPort);
        Count++;

        //
        // Read Transfer Flow Chart: write READ byte to DATA_IN
        //
        KcsData = KCS_READ;
        BmcWrite8 (IpmiInstance->AccessType, KcsDataPort, KcsData);
    }  // while loop
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
