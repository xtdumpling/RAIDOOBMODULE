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
/** @file Pilot4DxeInit.c

  1. Port SIO DXE initial table and routine for GenericSio.c
  2. Define SIO bootscriptable table
**/
//----------------------------------------------------------------------
//Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiGenericSio.h>
#include <Library/AmiSioDxeLib.h>
#include <Library/PcdLib.h>

//
// 16550 UART register offsets and bitfields
//
#define R_UART_RXBUF          0
#define R_UART_TXBUF          0
#define R_UART_BAUD_LOW       0
#define R_UART_BAUD_HIGH      1
#define R_UART_FCR            2
#define   B_UART_FCR_FIFOE    BIT0
#define   B_UART_FCR_FIFO64   BIT5
#define R_UART_LCR            3
#define   B_UART_LCR_DLAB     BIT7
#define R_UART_MCR            4
#define   B_UART_MCR_RTS      BIT1
#define R_UART_LSR            5
#define   B_UART_LSR_RXRDY    BIT0
#define   B_UART_LSR_TXRDY    BIT5
#define   B_UART_LSR_TEMT     BIT6
#define R_UART_MSR            6
#define   B_UART_MSR_CTS      BIT4
#define   B_UART_MSR_DSR      BIT5
//----------------------------------------------------------------------
// internal funtion declare; these funtions are only used by this file.
//----------------------------------------------------------------------
/**
  This function will initialize Serial port communication registers.

  @param  Dev                   Pointer to SIO device private data structure.
**/
VOID Pilot4_ProgramSerialPortRegisters(
    IN  SIO_DEV2    *Dev
)
{
    UINT32         Divisor;
    //
    // Program Base Addr
    //
    IoWrite8(Dev->Owner->SpioSdlInfo->SioIndex,Dev->Owner->SpioSdlInfo->Base1HiReg);
    IoWrite8(Dev->Owner->SpioSdlInfo->SioData,(UINT8)(Dev->VlData.DevBase1 >> 8));
    IoWrite8(Dev->Owner->SpioSdlInfo->SioIndex,Dev->Owner->SpioSdlInfo->Base1LoReg);
    IoWrite8(Dev->Owner->SpioSdlInfo->SioData,(UINT8)(Dev->VlData.DevBase1 & 0x00FF));
    //
    // Enable the device
    //
    IoWrite8(Dev->Owner->SpioSdlInfo->SioIndex,Dev->Owner->SpioSdlInfo->ActivateReg);
    IoWrite8(Dev->Owner->SpioSdlInfo->SioData,Dev->Owner->SpioSdlInfo->ActivateVal);
    //
    // Calculate divisor for baud generator
    //    Ref_Clk_Rate / Baud_Rate / 16
    //
    Divisor = PcdGet32 (PcdSerialClockRate) / (PcdGet32 (PcdSerialBaudRate) * 16);
    if ((PcdGet32 (PcdSerialClockRate) % (PcdGet32 (PcdSerialBaudRate) * 16)) >= PcdGet32 (PcdSerialBaudRate) * 8) {
      Divisor++;
    }
    //
    // Configure baud rate
    //
    IoWrite8(Dev->VlData.DevBase1 + R_UART_LCR, (UINT8)(IoRead8 (Dev->VlData.DevBase1 + R_UART_LCR) | B_UART_LCR_DLAB));
    IoWrite8(Dev->VlData.DevBase1 + R_UART_BAUD_HIGH, (UINT8) (Divisor >> 8));
    IoWrite8(Dev->VlData.DevBase1 + R_UART_BAUD_LOW, (UINT8) (Divisor & 0xFF));
    //
    // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
    // Strip reserved bits from PcdSerialLineControl
    //
    IoWrite8(Dev->VlData.DevBase1 + R_UART_LCR, (UINT8)(PcdGet8 (PcdSerialLineControl) & 0x3F));
    //
    // Enable and reset FIFOs
    // Strip reserved bits from PcdSerialFifoControl
    //
    IoWrite8(Dev->VlData.DevBase1 + R_UART_FCR, 0x00);
    IoWrite8(Dev->VlData.DevBase1 + R_UART_FCR, (UINT8)(PcdGet8 (PcdSerialFifoControl) & (B_UART_FCR_FIFOE | B_UART_FCR_FIFO64)));
    //
    // Put Modem Control Register(MCR) into its reset state of 0x00.
    //
    IoWrite8(Dev->VlData.DevBase1 + R_UART_MCR, 0x00);

    return;
}

/**
  This function will clear SIO resource.

  @param  Dev                   Pointer to SIO device private data structure.
**/
VOID Pilot4_ClearDevResource(
    IN  SIO_DEV2    *Dev
)
{
    //Enter Configuration Mode.
    IoWrite8(PILOT4_CONFIG_INDEX, PILOT4_CONFIG_MODE_ENTER_VALUE);
    //Select device
    IoWrite8(PILOT4_CONFIG_INDEX, PILOT4_LDN_SEL_REGISTER);
    IoWrite8(PILOT4_CONFIG_DATA, Dev->DeviceInfo->Ldn);
    //Clear Base Address
    IoWrite8(PILOT4_CONFIG_INDEX, PILOT4_BASE1_HI_REGISTER);
    IoWrite8(PILOT4_CONFIG_DATA, 0);
    IoWrite8(PILOT4_CONFIG_INDEX, PILOT4_BASE1_LO_REGISTER);
    IoWrite8(PILOT4_CONFIG_DATA, 0);
    //Clear Interrupt
    IoWrite8(PILOT4_CONFIG_INDEX, PILOT4_IRQ1_REGISTER);
    IoWrite8(PILOT4_CONFIG_DATA, 0);
    //Exit Configuration Mode
    IoWrite8(PILOT4_CONFIG_INDEX, PILOT4_CONFIG_MODE_EXIT_VALUE);

    return;
}

/**
  This function provide COMs initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
EFI_STATUS Pilot4_COM_Init(
    IN AMI_BOARD_INIT_PROTOCOL      *This,
    IN UINTN                        *Function,
    IN OUT VOID                     *ParameterBlock
)
{
    //Update Standard parameter block
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    SIO_INIT_STEP                   InitStep=(SIO_INIT_STEP)Args->InitStep;
    AMI_SIO_PROTOCOL                *AmiSio=(AMI_SIO_PROTOCOL*)Args->Param1;
    EFI_PCI_IO_PROTOCOL             *PciIo=(EFI_PCI_IO_PROTOCOL*)Args->Param2;

    EFI_STATUS                      Status=EFI_SUCCESS;
    SIO_DEV2                        *Dev=(SIO_DEV2*)AmiSio;
    
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    switch (InitStep) {
    case isGetSetupData:
        //Clear Device Resource?
        if((!Dev->DeviceInfo->Implemented) || (!Dev->NvData.DevEnable)) {
            Pilot4_ClearDevResource(Dev);
        }
        break;

    case isPrsSelect:
        break;

    case isBeforeActivate:
        //Only decode UART1/UART2. More others UART port is decode in PEI
        //Attention! Remove the more com ports to PEI decode.
        if(Dev->DeviceInfo->Uid < 0x02) {
            //Decode?
            if(Dev->DeviceInfo->Implemented && Dev->NvData.DevEnable) {
                AmiSioLibSetLpcDeviceDecoding(PciIo,Dev->VlData.DevBase1, Dev->DeviceInfo->Uid, Dev->DeviceInfo->Type);
            } else {
                AmiSioLibSetLpcDeviceDecoding(PciIo,0, Dev->DeviceInfo->Uid, Dev->DeviceInfo->Type);
            }
        }
        //This is specific to Pilot COM port. It loses communication parameters once it is disabled.
        //Need to reprogram all communication parameters after enabled.
        Pilot4_ProgramSerialPortRegisters (Dev);
        break;

    case isGetModeData:
        break;

    case isAfterActivate:
        break;

    case isAfterBootScript:
        break;

#if  AMI_SIO_MINOR_VERSION >= 6     
    case isAfterSmmBootScript:      
        break;
#endif
        
    default:
        Status=EFI_INVALID_PARAMETER;
    }//switch
    return Status;
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

