//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SerialRecovery.c
    File implements Serial Recovery Functions
**/

//----------------------------------------------------------------------

#include "SerialRecovery.h"

//----------------------------------------------------------------------

EFI_PEI_SERVICES    **mPeiServices;

EFI_PEI_STALL_PPI    *mStallPpi;

EFI_PEI_DEVICE_RECOVERY_MODULE_PPI SerialRecoveryModule = {
    GetNumberRecoveryCapsules, GetRecoveryCapsuleInfo, LoadRecoveryCapsule
};

//
// PPI to be installed
//
EFI_PEI_PPI_DESCRIPTOR  SerialRecoveryPpiList[] = {
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiDeviceRecoveryModulePpiGuid, &SerialRecoveryModule
    }
};

PEI_RECOVERY_SERIAL_MODE_PPI ComSerialDevice = {
    0x3f8
};

//
// PPI to be installed
//
EFI_PEI_PPI_DESCRIPTOR SerialDevicePpiList[] = { 
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gSerialRecoveryDevicePpiGuid, &ComSerialDevice
    }
};

/**
    Serial Recovery PEI entry Function.

    @param FfsHeader - FFSHeader
    @param PeiServices - pointer to PEI services

    @retval EFI_SUCCESS function executed successfully
**/
EFI_STATUS 
EFIAPI 
SerialRecoveryPeimEntry (
    IN EFI_PEI_FILE_HANDLE          FfsHeader,
    IN CONST EFI_PEI_SERVICES       **PeiServices 
)
{
    EFI_STATUS    Status;

    mPeiServices = PeiServices;

    Status = (*PeiServices)->LocatePpi(
                            PeiServices,
                            &gEfiPeiStallPpiGuid,
                            0,
                            NULL,
                            &mStallPpi
                            );
    ASSERT_PEI_ERROR( PeiServices, Status );
    if (EFI_ERROR(Status)) { 
        return Status;
    }

    Status = (*PeiServices)->InstallPpi( PeiServices,SerialRecoveryPpiList );
    ASSERT_PEI_ERROR( PeiServices, Status );
    if (EFI_ERROR(Status)) { 
        return Status;
    }

    //
    // Install the Serial Device for Com port
    //
    ComSerialDevice.SerialDeviceBaseAddress = (UINT16)PcdGet64( PcdSerialRegisterBase );
    Status = (*PeiServices)->InstallPpi ( PeiServices,SerialDevicePpiList );
    ASSERT_PEI_ERROR( PeiServices, Status );

    return Status;
}

/**
    Function to get recovery file information.

    @param PeiServices
    @param pCapsuleName
    @param pCapsuleSize
    @param ExtendedVerification

    @retval EFI_STATUS
**/
EFI_STATUS 
GetRecoveryFileInfo (
    IN EFI_PEI_SERVICES    **PeiServices,
    IN OUT VOID            **pCapsuleName,
    IN OUT UINTN           *pCapsuleSize,
    OUT    BOOLEAN         *ExtendedVerification 
)
{
    if( pCapsuleName != NULL )
        *pCapsuleName = "AMI.ROM";

    if( pCapsuleSize != NULL )
        *pCapsuleSize = (UINTN) PcdGet32( PcdRecoveryImageSize );

    if( ExtendedVerification != NULL )
        *ExtendedVerification = FALSE;

    return EFI_SUCCESS; 
}

/**
    Function to get the Number of Recovery Capsules.

    @param PeiServices - pointer to PEI services
    @param This
    @param NumberRecoveryCapsules - Number of Recovery Capsule

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI 
GetNumberRecoveryCapsules (
    IN EFI_PEI_SERVICES                      **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
    OUT UINTN                                *NumberRecoveryCapsules 
)
{
    *NumberRecoveryCapsules = 1;
    return EFI_SUCCESS;
}

/**
    Function to get the Recovery Capsule Information.

    @param PeiServices - pointer to PEI services
    @param This
    @param CapsuleInstance
    @param Size
    @param CapsuleType

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI 
GetRecoveryCapsuleInfo (
    IN EFI_PEI_SERVICES                      **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
    IN UINTN                                 CapsuleInstance,
    OUT UINTN                                *Size,
    OUT EFI_GUID                             *CapsuleType 
)
{

    if( CapsuleType )
        *CapsuleType = gSerialCapsuleGuid;

    if( Size )
        *Size = (UINTN)PcdGet32( PcdRecoveryImageSize );

    return EFI_SUCCESS;

}

/**
    Function to print the Serial Port base address.

    @param BaseAddress

    @retval VOID
**/
VOID 
PrintBaseAddress (
    IN UINT64    BaseAddress 
)
{
    CHAR8    TempBuffer[5]={0,0,0,0,0};
    UINTN    ValueCharNum=0;
    UINTN    Remainder;
    CHAR8    TempPrintBuffer[5]={0,0,0,0,0};
    UINTN    Index=0;

    do {
        BaseAddress = (UINT64)Div64 ((UINT64)BaseAddress, 0x10, &Remainder);

        if(Remainder < 0xa) {
            TempBuffer[ValueCharNum] = (CHAR8)(Remainder + '0');
        } else {
            TempBuffer[ValueCharNum] = (CHAR8)(Remainder + 'A' - 0xa);
        }
        ValueCharNum++;
    } while (BaseAddress != 0);

    //
    // Reverse temp string into Buffer.
    //
    while (ValueCharNum) {
        TempPrintBuffer[Index] = TempBuffer[ValueCharNum-1];
        ValueCharNum--;
        Index++;
    }
    SendSerialString(&TempPrintBuffer[0]);
    return;
}       

/**
    Function to download the Capsule file from the Serial device and returns
    the buffer.

    @param PeiServices - pointer to PEI services
    @param This
    @param CapsuleInstance
    @param Buffer - Recovery Capsule Data

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI 
LoadRecoveryCapsule (
    IN OUT EFI_PEI_SERVICES                  **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *This,
    IN UINTN                                 CapsuleInstance,
    OUT VOID                                 *Buffer 
)
{
    CHAR8                           *pBuff;
    UINT32                          FileSize;
    UINT8                           Result   = 0xFF;
    CHAR8                           *message;
    UINTN                           Number;
    CHAR8                           Char      = 0;
    CHAR8                           *ReadChar = &Char;
    UINT32                          waitCount;
    BOOLEAN                         Recovery = FALSE;
    PEI_RECOVERY_SERIAL_MODE_PPI    *ComDevice=NULL;
    UINT8                           Index=0;
    EFI_STATUS                      Status;
    UINTN                           RecoveryCapsuleSize;
    BOOLEAN                         ExtendedVerification;

    Status = GetRecoveryFileInfo (
                    PeiServices, 
                    NULL, 
                    &RecoveryCapsuleSize, 
                    &ExtendedVerification
                    );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    mPeiServices = PeiServices;
    Index=0;

    do {
        //
        // Locate the Serial Device PPI
        //
        Status = (**PeiServices).LocatePpi (
                                    PeiServices, 
                                    &gSerialRecoveryDevicePpiGuid, 
                                    Index, 
                                    NULL, 
                                    &ComDevice
                                    );

        //
        // If Found Proceed with Reading recovery file. If not break the 
        // loop and return the status
        //
        if (( EFI_ERROR(Status) )) {
            break;   
        }

        FileSize = (UINT32)RecoveryCapsuleSize;

        GetNumberRecoveryCapsules( PeiServices, This, &Number );
        if(Number == 0) {
            return EFI_UNSUPPORTED;
        }

        PcdSet64 ( PcdSerialRegisterBase, ComDevice->SerialDeviceBaseAddress );

        Status = SerialPortInitialize();
        if (EFI_ERROR(Status)) {
            return Status;
        }

        *ReadChar = '\n';
        SerialPortWrite( ReadChar, 0x01 );
        PrintCopyRightMessage();

        message = "Press space to start recovery from Serial Address 0x";

        //
        // Display the Serial Port Base address
        //
        SendSerialString( message );
        PrintBaseAddress( (UINT64)ComDevice->SerialDeviceBaseAddress );
        message = "\n\r or 'N' to try Next Serial Device or 'Q' to quit\n\r";
        SendSerialString( message );
        do {
            waitCount=0xF;        // EKCheck
            while ( waitCount-- ) {
                if ( GetSerialData ( ReadChar ) == TRUE ) {
                    Recovery = TRUE;
                    break;
                }
            }

            if( *ReadChar ==' ' ) {
                break;
            }

            if ( *ReadChar=='N' || *ReadChar=='n' ) {
                //
                // Try the Next Serial Device
                //
                message = "Trying next serial device if avilable.\n\r";
                SendSerialString(message);
                Recovery = FALSE;
                break; 
            }
            if ( *ReadChar == 'Q' || *ReadChar == 'q' ) {
                //
                // Exit from Serial Recovery
                //
                message = "Serial Recovery is aborted by user.\n\r";
                SendSerialString( message );
                return EFI_ABORTED;
            }
        } while( Recovery==TRUE );

        if(Recovery == FALSE) {
            //
            // Try Next Device
            //
            Index++;
            continue;
        }
        message="Send BIOS image using XMODEM protocol\n\r";
        SendSerialString( message );

        //
        // Wait for 3-5 Seconds before User Sends a file
        //
        mStallPpi->Stall( mPeiServices, mStallPpi, 20000000 );

        pBuff = Buffer;

        if ( !ReadFileFromSerial( pBuff, &FileSize, &Result )) {
            PEI_TRACE((-1, 
                       PeiServices, 
                       "\nRead File Failed Capsule Size= %d, FileRead Result=%d\n", 
                       FileSize, Result));

            if ( Result == 1 ) {
                PEI_TRACE((-1, PeiServices, "\nNo Responce from Terminal\n"));
            }

            if ( Result == 3 ) {
                PEI_TRACE((-1, PeiServices, "\nFile Size over Run\n"));
            }

            //
            // Error getting Recovery file from current serial device. 
            // So try another Serial Device
            //
            Index++;
            continue;
        } else {
            //
            // File got successfully. So Return
            //
            if( ExtendedVerification || FileSize == (UINT32)RecoveryCapsuleSize )
                return EFI_SUCCESS;
        }
    } while( TRUE );

    return EFI_NOT_FOUND;
}

/**
    Function to read the Recovery file through Serial Device.

    @param Buffer - Data buffer
    @param Size - Size
    @param result - Result Status

    @retval BOOLEAN
**/
BOOLEAN 
ReadFileFromSerial (
    CHAR8 *    Buffer,
    UINT32*    Size,
    UINT8 *    result 
)
{
    //
    // Read recovery file through serial port, check checksum,signature etc.,
    // and copy it to the buffer ImageStart.
    //

    UINT32    NumBlocks = *Size / 128;
    UINT32    count     = 10;
    UINT32    BlockID   = 1;
    UINT8     BlockNum  = 1;
    UINT8     Command   = XMODEM_NAK;
    UINT8     Data      = 0;
    BOOLEAN   Status;

    NumBlocks++;
    while ( NumBlocks ) {
        if ( !SendNakAck( &Data, &Command )) {     // send NAK
            *result = 1;
            return FALSE;
        }

        if ( Data == XMODEM_EOT ) { // If end of transfer, exit with success
            Command = XMODEM_ACK;
            SendSerialData( &Command );
            BlockID--;
            *Size   = BlockID * 128;
            *result = 0;
            return TRUE;
        }

        if ( Data == XMODEM_SOH ) {
            //
            // Start header received, get rest of the packet
            //
            Status = GetSerialData( &Data );
            //
            // Read block#
            //
            if ( Status && (Data == BlockNum)) {
                Status = GetSerialData( &Data );
                if ( Status ) {
                    //
                    // If FF-block number doesn't match, retry
                    //
                    if ( ReadSerialBlock(Buffer) ) {     
                        //
                        // Read 128 byte packet
                        //
                        BlockNum++;
                        BlockID++;
                        NumBlocks--;
                        Command = XMODEM_ACK;
                        continue;
                    }
                }
            }
            while ( GetSerialData( &Data )) {
                ;
            }
        }
    }

    //
    // No more block to read, No EOT byte received, 
    // terminate xfer and exit with error
    //
    *result = 3;
    while ( GetSerialData( &Data )) {
        ;    // If abort, read and discard
    }
    Command = XMODEM_CAN;
    while ( count-- ) {
        SendSerialData( &Command );    // Send CANCEL Command
    }
    return FALSE;
}

UINT32 CurrentIndex = 0;  // Global variable to hold current buffer index

/**
    Function to read the Serial Block.

    @param Buffer - Data buffer

    @retval BOOLEAN
**/
BOOLEAN 
ReadSerialBlock (
    UINT8*    Buffer 
)
{
    UINT8     CheckSum     = 0;
    UINT8     DataByte     = 0;
    UINT8     * DataBuffer = Buffer;
    UINT32    TempIndex    = CurrentIndex;
    UINT8     bytecount;

    //
    // Read 128 byte packet
    //
    for ( bytecount = 1; bytecount <= 128; bytecount++ ) {
        //
        // If error  reading serial port, retry packet
        //
        if ( !(GetSerialData( DataBuffer + CurrentIndex ))) {
            CurrentIndex = TempIndex;
            return FALSE;
        }
        CheckSum += *(DataBuffer + CurrentIndex++);
    }

    //
    // Get Checksum byte
    //
    if ( !(GetSerialData( &DataByte ))) {
        CurrentIndex = TempIndex;
        return FALSE;
    }

    //
    // If byte checksum doesn't match, retry
    //
    if ( DataByte != CheckSum ) {
        CurrentIndex = TempIndex;
        return FALSE;
    }
    return TRUE;
}

/**
    Function to send the Ack.

    @param Data
    @param XCommand

    @retval BOOLEAN
**/
BOOLEAN 
SendNakAck (
    UINT8    * Data,
    UINT8    *XCommand 
)
{
    UINT8    AbortCount = 12, count = 24; // Number of NAKs or ACKs to send
                                          // before abort
    CHAR8    * SerialMsg;

    while ( count-- ) {
        SendSerialData( XCommand );
        if ( GetSerialData( Data )) {
            return TRUE;
        }
    }
    //
    // If timeout, cancel the transfer and
    // return false, with abort message
    //
    while ( GetSerialData( Data )) {      // If abort, read and discard
        ;
    }
    *XCommand = XMODEM_CAN;
    while ( AbortCount-- ) {
        SendSerialData( XCommand );       // Send CANCEL Command
    }
    SerialMsg = "\n\rAborting Serial Recovery";
    SendSerialString( SerialMsg );
    return FALSE;
}

/**
    Function to get the Data from Serial Port

    @param ReadData - Data Got from Serial Device

    @retval TRUE    If data read successfully from receive buffer
    @retval FALSE   If overrun/framing error
**/
BOOLEAN 
GetSerialData (
    UINT8*    ReadData 
)
{
    UINTN    Result;
    UINTN    MaxDelay = 0x1046A;

    while(!SerialPortPoll() && MaxDelay)
        MaxDelay--;

    if ( MaxDelay <= 0 ) {
        return FALSE;
    }
     
    Result = SerialPortRead(ReadData, 0x01);

    if ( Result < 0x01 ) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
    Function to send the Data to serial Device.

    @param d - Data to be send it to Serial Device
    
    @retval VOID
**/
VOID 
SendSerialData (
    UINT8*    d 
)
{

    SerialPortWrite( d, 0x01 );
}

/**
    Function to send the String to serial Device

    @param Str - String to be send it to Serial Device
    
    @retval VOID
**/
VOID 
SendSerialString (
    CHAR8*    Str 
)
{
    UINTN       Count = 0;
    UINT8       *TempBuffer ;

    TempBuffer = (UINT8*) Str;

     //
     // Lets count the size of the string
     //
    while ( *(TempBuffer + Count) ) {
        Count++;
    }

    if ( Count )  
        SerialPortWrite(TempBuffer, Count); 
}

/**
    Function to display the CopyRight Message in Serial Device.
    
    @param VOID
    
    @retval VOID
**/
VOID 
PrintCopyRightMessage( )
{
    static CHAR8
    *Message =  "\n\r*********************************************************\n\r"
               "(C)Copyright 1985-2014, American Megatrends, Inc.\n\r"
               "               All Rights Reserved.\n\r"
               "               AMI  Serial Recovery.\n\r"
               "*********************************************************\n\r";

    SendSerialString( Message );
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
