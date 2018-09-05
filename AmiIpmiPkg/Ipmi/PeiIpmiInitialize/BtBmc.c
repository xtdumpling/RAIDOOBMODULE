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

/** @file BtBmc.c
    BT Transport Hook.

**/

//---------------------------------------------------------------------------

#include "IpmiBmc.h"
#include "BtBmc.h"

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#define BT_DELAY_COUNTER           IpmiInstance->Interface.BT.BtDelay

//---------------------------------------------------------------------------

/**
    Sends the command to BT interface BMC port

    @param IpmiInstance Data structure describing BMC
               variables and used for sending commands
    @param Context NULL here
    @param Data Pointer to command data that will be sent
                to BMC along with Command
    @param DataSize Size of the command data

    @return  EFI_STATUS Status of the Sending data to BMC port

**/
EFI_STATUS
SendDataToBtBmcPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance,
  IN  VOID                            *Context,
  IN  UINT8                           *Data,
  IN  UINT8                           DataSize )
{
    UINT8                   BtCntlData;
    UINT8                   i;
    UINT32                  Retry;
    UINTN                   BtCtrlPort;
    UINTN                   BtCommunicationBufferPort;

    //
    // Update BtCtrlPort and BtCommunicationBufferPort based on IpmiInstance->AccessType.
    //
    if (IpmiInstance->AccessType == IPMI_IO_ACCESS) {
        BtCtrlPort = (UINTN)IpmiInstance->Interface.BT.CtrlPort;
        BtCommunicationBufferPort = (UINTN)IpmiInstance->Interface.BT.ComBuffer;
    } else {
        BtCtrlPort = IpmiInstance->MmioBaseAddress;
        BtCommunicationBufferPort = BtCtrlPort + IpmiInstance->BaseAddressRange;
    }

    //
    // wait for B_BUSY bit to clear (BMC ready to accept a request)
    //
    for (Retry= 0; Retry < BT_DELAY_COUNTER; Retry++) {
        BtCntlData = BmcRead8 (IpmiInstance->AccessType, BtCtrlPort);
        MicroSecondDelay (15);  // 15 microsecond delay.
        if (!(BtCntlData & B_BUSY_BIT)) {
            break;
        }
    }
    if (Retry == BT_DELAY_COUNTER ) {
        return EFI_NOT_READY;
    }

    //
    // wait for H2B_ATN bit to clear (Acknowledgement of previous commands)
    //
    for (Retry= 0; Retry < BT_DELAY_COUNTER; Retry++) {
        BtCntlData = BmcRead8 (IpmiInstance->AccessType, BtCtrlPort);
        MicroSecondDelay (15);  // 15 microsecond delay.
        if (!(BtCntlData & H2B_ATN_BIT)) {
            break;
        }
    }
    if (Retry == BT_DELAY_COUNTER ) {
        return EFI_NOT_READY;
    }

    //
    // set CLR_WR_PTR
    //
    BtCntlData = CLR_WR_PTR_BIT;
    BmcWrite8 (IpmiInstance->AccessType, BtCtrlPort, BtCntlData);

    //
    // send each message byte out (write data to HOST2BMC buffer)
    //
    for ( i = 0; i < DataSize; i++ ) {  // send the message
        BmcWrite8 (IpmiInstance->AccessType, BtCommunicationBufferPort, *(Data + i));
    }

    //
    // set H2B_ATN bit to inform BMC that data is available
    //
    BtCntlData = H2B_ATN_BIT;
    BmcWrite8 (IpmiInstance->AccessType, BtCtrlPort, BtCntlData);

    return EFI_SUCCESS;
}

/**
    Receives the Data from BT interface BMC port

    @param IpmiInstance Data structure describing BMC
            variables and used for sending commands
    @param Context NULL here
    @param Data Pointer to Response data that is received from BMC
    @param DataSize Size of the Response data

    @return EFI_STATUS Status of the Receiving data from BMC port

**/
EFI_STATUS
ReceiveBmcDataFromBtPort (
  IN  EFI_IPMI_BMC_INSTANCE_DATA      *IpmiInstance,
  IN  VOID                            *Context,
  OUT UINT8                           *Data,
  OUT UINT8                           *DataSize )
{
    UINT8           BtCntlData;
    UINT8           Length;
    UINT8           i;
    UINT32          Retry;
    UINTN           BtCtrlPort;
    UINTN           BtCommunicationBufferPort;

    //
    // Update BtCtrlPort and BtCommunicationBufferPort based on IpmiInstance->AccessType.
    //
    if (IpmiInstance->AccessType == IPMI_IO_ACCESS) {
        BtCtrlPort = (UINTN)IpmiInstance->Interface.BT.CtrlPort;
        BtCommunicationBufferPort = (UINTN)IpmiInstance->Interface.BT.ComBuffer;
    } else {
        BtCtrlPort = IpmiInstance->MmioBaseAddress;
        BtCommunicationBufferPort = BtCtrlPort + IpmiInstance->BaseAddressRange;
    }

    //
    // wait for B2H_ATN bit to be set,signalling data is available for host.
    //
    for (Retry = 0; Retry < BT_DELAY_COUNTER; Retry++) {
        BtCntlData = BmcRead8 (IpmiInstance->AccessType, BtCtrlPort);
        MicroSecondDelay (15);  // 15 microsecond delay.
        if (BtCntlData & B2H_ATN_BIT) {
            break;
        }
    }
    if (Retry == BT_DELAY_COUNTER ) {
        return EFI_NOT_READY;
    }

    //
    // set H_BUSY bit, indicating host is in process of reading data from interface
    //
    BtCntlData = BmcRead8 (IpmiInstance->AccessType, BtCtrlPort);
    if (!(BtCntlData & H_BUSY )) {
        BtCntlData = H_BUSY;        // most bits are rw1c, so clear them.
        BmcWrite8 (IpmiInstance->AccessType, BtCtrlPort, BtCntlData);
    }

    //
    // clear B2H_ATN bit,to acknowledge receipt of message reponse
    //
    BtCntlData = B2H_ATN_BIT;     // most bits are rw1c, so clear them.
    BmcWrite8 (IpmiInstance->AccessType, BtCtrlPort, BtCntlData);

    //
    // set CLR_RD_PTR bit
    //
    BtCntlData = CLR_RD_PTR_BIT;  // most bits are rw1c, so clear them.
    BmcWrite8 (IpmiInstance->AccessType, BtCtrlPort, BtCntlData);

    //
    // read the data bytes from BMC
    //
    Length = BmcRead8 (IpmiInstance->AccessType, BtCommunicationBufferPort);
    Data[0] = Length;
    *DataSize = Length;
    for (i = 1; i <= Length; i++) {
        *(Data + i) = BmcRead8 (IpmiInstance->AccessType, BtCommunicationBufferPort);
    }

    //
    // clear H_BUSY bit indicating host is done reading data from BMC
    //
    BtCntlData = BmcRead8 (IpmiInstance->AccessType, BtCtrlPort);
    if (BtCntlData & H_BUSY ) {
        BtCntlData = H_BUSY;        // most bits are rw1c, so clear them.
        BmcWrite8 (IpmiInstance->AccessType, BtCtrlPort, BtCntlData);
    }

    if(Length == 0 ) {
        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
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
