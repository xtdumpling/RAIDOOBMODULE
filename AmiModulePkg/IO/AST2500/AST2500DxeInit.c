//*********************************************************************
//*********************************************************************
//**                                                                 **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.          **
//**                                                                 **
//*********************************************************************
//*********************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Fixed systme hang at generic SIO SMM module when enable debug mode problem.
//      Reason:     
//      Auditor:    Jacker Yeh
//      Date:       May/26/2016
//
//**********************************************************************
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
/** @file AST2500DxeInit.c

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

extern DXE_DEVICE_INIT_DATA   AST2500_DXE_COM_Mode_Init_Table[];
extern SIO_DEVICE_INIT_DATA   DXE_GPIO_Init_Table_Before_Active[];
extern UINT8 DXE_GPIO_Init_Table_Before_Active_Count;
extern SIO_DEVICE_INIT_DATA   DXE_GPIO_Init_Table_After_Active[];
extern UINT8 DXE_GPIO_Init_Table_After_Active_Count;
#if AST2500_SWC_PRESENT
extern SIO_DEVICE_INIT_DATA   DXE_SWC_Init_Table_Before_Active[];
extern UINT8 DXE_SWC_Init_Table_Before_Active_Count;
#endif
#if AST2500_ILPC2AHB_PRESENT
extern SIO_DEVICE_INIT_DATA   DXE_ILPC2AHB_Init_Table_Before_Active[];
extern UINT8 DXE_ILPC2AHB_Init_Table_Before_Active_Count;
#endif
#if AST2500_MAILBOX_PRESENT
extern SIO_DEVICE_INIT_DATA   DXE_MAILBOX_Init_Table_Before_Active[];
extern UINT8 DXE_MAILBOX_Init_Table_Before_Active_Count;
#endif
//----------------------------------------------------------------------
// internal funtion declare; these funtions are only used by this file.
//----------------------------------------------------------------------
/**
  This function will clear SIO resource.

  @param  Dev                   Pointer to SIO device private data structure.
**/
VOID AST2500_ClearDevResource(
    IN  SIO_DEV2    *Dev
)
{
    //Enter Configuration Mode.
    IoWrite8(AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);
    IoWrite8(AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);
    //Select device
    IoWrite8(AST2500_CONFIG_INDEX, AST2500_LDN_SEL_REGISTER);
    IoWrite8(AST2500_CONFIG_DATA, Dev->DeviceInfo->Ldn);
    //Clear Base Address
    IoWrite8(AST2500_CONFIG_INDEX, AST2500_BASE1_HI_REGISTER);
    IoWrite8(AST2500_CONFIG_DATA, 0);
    IoWrite8(AST2500_CONFIG_INDEX, AST2500_BASE1_LO_REGISTER);
    IoWrite8(AST2500_CONFIG_DATA, 0);
    //Clear Interrupt
    IoWrite8(AST2500_CONFIG_INDEX, AST2500_IRQ1_REGISTER);
    IoWrite8(AST2500_CONFIG_DATA, 0);
    //Exit Configuration Mode
    IoWrite8(AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_EXIT_VALUE);

    return;
}

/**
  This function will call back in ready to boot phase to save registers into bootscript table.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context.
**/
static VOID AST2500_GpioCallbackReadyToBoot(
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
    EFI_STATUS         Status;

    EFI_S3_SAVE_STATE_PROTOCOL * BootScriptProtocol = NULL;
    EFI_GUID gSioEfiS3SaveStateProtocolGuid= EFI_S3_SAVE_STATE_PROTOCOL_GUID;

    Status = pBS->LocateProtocol(&gSioEfiS3SaveStateProtocolGuid,NULL,&BootScriptProtocol);
    if (EFI_ERROR(Status)) {
        TRACE((-1,"GenericSIO: SIODXE fail to locate EfiBootScriptSaveProtocol %r",Status));
        return;
    }
    //1,AMI_TODO:enter cfgmode
    SioLib_BootScriptIO(AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE, BootScriptProtocol);
    SioLib_BootScriptIO(AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE, BootScriptProtocol);
    //2,AMI_TODO:select gpio device
    SioLib_BootscriptLdnSel(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA, \
                            AST2500_LDN_SEL_REGISTER, AST2500_LDN_GPIO, BootScriptProtocol);

    //3,save table value
    SioLib_BootScriptSioS3SaveTable(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA, \
                                    DXE_GPIO_Init_Table_After_Active,DXE_GPIO_Init_Table_After_Active_Count, BootScriptProtocol);
    //4,AMI_TODO:exit cfg mode
    SioLib_BootScriptIO(AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_EXIT_VALUE, BootScriptProtocol);
    //
    //Kill the Event
    //
    pBS->CloseEvent(Event);
}

//----------------------------------------------------------------------
// global funtion declare ; these funtions are used for other files.
//----------------------------------------------------------------------

/**
  This function provide COMs initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
EFI_STATUS AST2500_COM_Init(
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
    UINT8                           rv; //ComMode Register
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    switch (InitStep) {
    case isGetSetupData:
        //Clear Device Resource?
        if((!Dev->DeviceInfo->Implemented) || (!Dev->NvData.DevEnable)) {
            AST2500_ClearDevResource(Dev);
        }
        //Change HID value in DXE phase for correct devcie path.
        if(Dev->DeviceInfo->Uid == 0x05) {
            if (Dev->NvData.DevMode > 0) {
                Dev->EisaId.HID = 0x51041D0;
            }
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
        //Program COM Clock Source Registers.
        if(AST2500_DXE_COM_Mode_Init_Table[Dev->DeviceInfo->Uid].AndData8 == 0xFF) {
            rv=AST2500_DXE_COM_Mode_Init_Table[Dev->DeviceInfo->Uid].OrData8;
        } else {
            Status=AmiSio->Access(AmiSio, FALSE, FALSE, AST2500_DXE_COM_Mode_Init_Table[Dev->DeviceInfo->Uid].Reg8,&rv);
            ASSERT_EFI_ERROR(Status);
            rv &= AST2500_DXE_COM_Mode_Init_Table[Dev->DeviceInfo->Uid].AndData8;
            rv |= AST2500_DXE_COM_Mode_Init_Table[Dev->DeviceInfo->Uid].OrData8;
        }
        Status=AmiSio->Access(AmiSio,TRUE,FALSE,AST2500_DXE_COM_Mode_Init_Table[Dev->DeviceInfo->Uid].Reg8,&rv);
        ASSERT_EFI_ERROR(Status);

        break;

    case isGetModeData:
        
        break;

    case isAfterActivate:
        break;

    case isAfterBootScript:
        break;
//SMCPKG_SUPPORT+
#if  AMI_SIO_MINOR_VERSION >= 6     
    case isAfterSmmBootScript:      
        break;
#endif
//SMCPKG_SUPPORT
    default:
        Status=EFI_INVALID_PARAMETER;
    }//switch
    return Status;
}

/**
  This function provide KBC initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
#if AST2500_KEYBOARD_PRESENT
EFI_STATUS AST2500_KBC_Init(
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
    SIO_DEV2                        *Dev=(SIO_DEV2*)AmiSio;

    EFI_STATUS                      Status=EFI_SUCCESS;
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    switch (InitStep) {
    case isGetSetupData:
    case isPrsSelect:
    case isAfterActivate:
    case isAfterBootScript:
    case isGetModeData:
        break;

    case isBeforeActivate:
        //Decode
        AmiSioLibSetLpcDeviceDecoding(PciIo,Dev->VlData.DevBase1, Dev->DeviceInfo->Uid, Dev->DeviceInfo->Type);
        break;

    default:
        Status=EFI_INVALID_PARAMETER;
    } //switch

    return Status;
}
#endif

/**
  This function provide GPIO initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
EFI_STATUS AST2500_GPIO_Init(
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
    EFI_EVENT                       GpioReadytoBoot;
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    switch (InitStep) {
    case isGetSetupData:
        break;

    case isPrsSelect:
    case isGetModeData:
        break;

    case isBeforeActivate:
        //Initial GPIO register if you need.
        //OEM_TODO: You need to fill DXE_GPIO_Init_Table_Before_Active[] first.
        ProgramIsaRegisterTable(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA,\
                                DXE_GPIO_Init_Table_Before_Active,DXE_GPIO_Init_Table_Before_Active_Count);

        break;

    case isAfterActivate:

        //Initial GPIO register if you need.
        //OEM_TODO: You need to fill DXE_GPIO_Init_Table_After_Active[] first.
        ProgramIsaRegisterTable(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA,\
                                DXE_GPIO_Init_Table_After_Active,DXE_GPIO_Init_Table_After_Active_Count);

        //Create event for boot script
        //Because Gpio is not standard device which have no activate reg0x30,so create event to save regs
        Status = CreateReadyToBootEvent(
                     TPL_NOTIFY,
                     AST2500_GpioCallbackReadyToBoot,
                     NULL,
                     &GpioReadytoBoot
                 );
        ASSERT_EFI_ERROR(Status);

        break;

    case isAfterBootScript:
        break;

    default:
        Status=EFI_INVALID_PARAMETER;
    } //switch

    return Status;
}


/**
  This function provide SWC initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
#if AST2500_SWC_PRESENT
EFI_STATUS AST2500_SWC_Init(
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

    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    switch (InitStep) {
    case isGetSetupData:
        break;

    case isPrsSelect:
    case isGetModeData:
        break;

    case isBeforeActivate:
        //Initial SWC register if you need.
        //OEM_TODO: You need to fill DXE_SWC_Init_Table_Before_Active[] first.
        ProgramIsaRegisterTable(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA,\
                DXE_SWC_Init_Table_Before_Active,DXE_SWC_Init_Table_Before_Active_Count);

        break;

    case isAfterActivate:

        break;

    case isAfterBootScript:
        break;

    default:
        Status=EFI_INVALID_PARAMETER;
    } //switch

    return Status;
}
#endif //AST2500_SWC_PRESENT

/**
  This function provide ILPC2AHB initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
#if AST2500_ILPC2AHB_PRESENT
EFI_STATUS AST2500_ILPC2AHB_Init(
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

    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    switch (InitStep) {
    case isGetSetupData:
        break;

    case isPrsSelect:
    case isGetModeData:
        break;

    case isBeforeActivate:
        //Initial ILPC2AHB register if you need.
        //OEM_TODO: You need to fill DXE_ILPC2AHB_Init_Table_Before_Active[] first.
        ProgramIsaRegisterTable(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA,\
                                DXE_ILPC2AHB_Init_Table_Before_Active,DXE_ILPC2AHB_Init_Table_Before_Active_Count);
        break;

    case isAfterActivate:

        break;

    case isAfterBootScript:
        break;

    default:
        Status=EFI_INVALID_PARAMETER;
    } //switch

    return Status;
}
#endif//AST2500_ILPC2AHB_PRESENT

/**
  This function provide MailBox initial routine in GenericSio.c

  @param  This                      AMI board initial protocol.
  @param  Function                  AMI sdl SIO initial routine.
  @param  ParameterBlock            SIO component initial routine parameters block...

  @retval  EFI_SUCCESS              Initial step sucessfully.
  @retval  EFI_INVALID_PARAMETER    Not find the initial step.
**/
#if AST2500_MAILBOX_PRESENT
EFI_STATUS AST2500_MAILBOX_Init(
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

    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_SIO_PARAM_SIG) return EFI_INVALID_PARAMETER;

    switch (InitStep) {
    case isGetSetupData:
        break;

    case isPrsSelect:
    case isGetModeData:
        break;

    case isBeforeActivate:
        //Initial Mailbox register if you need.
        //OEM_TODO: You need to fill DXE_MAILBOX_Init_Table_Before_Active[] first.
        ProgramIsaRegisterTable(AST2500_CONFIG_INDEX, AST2500_CONFIG_DATA,\
                                DXE_MAILBOX_Init_Table_Before_Active,DXE_MAILBOX_Init_Table_Before_Active_Count);

        break;

    case isAfterActivate:
        break;

    case isAfterBootScript:
        break;

    default:
        Status=EFI_INVALID_PARAMETER;
    } //switch

    return Status;
}
#endif //AST2500_MAILBOX_PRESENT

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

