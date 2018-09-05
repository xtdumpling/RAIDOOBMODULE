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

/** @file UsbIrq.c

**/

// Module specific Includes
#include <Token.h>

// Consumed Protocols
#include "Rt/AmiDef.h"
#include "Rt/UsbDef.h"
#include "Uhcd.h"
#include <Protocol/Cpu.h>
#include <Protocol/Legacy8259.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/PciIo.h>
#include <Protocol/LegacyBios.h>
#include "UsbIrq.h"

#include <AmiCspLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>

EFI_LEGACY_8259_PROTOCOL  *mLegacy8259;
EFI_CPU_ARCH_PROTOCOL     *CpuArch;
EFI_USB_PROTOCOL          *gAmiUsbController;
USB_GLOBAL_DATA           *gUsbData;
UINT16                    IrqEnableStore;
#if USB_RUNTIME_DRIVER_IN_SMM
EFI_SMM_CONTROL2_PROTOCOL *gSmmCtl = NULL;
#endif
URP_STRUC                 *UrpBuffer=NULL;
UINT8                     UefiIsrInstall = FALSE;
UINT8                     LegacyIsrInstall = FALSE;
UINT8                     LegacyBiosInstall = FALSE;
EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios = NULL;

#ifndef SLAVE_8259_IRQ
#define SLAVE_8259_IRQ    0x02
#endif

#define LEGACY_HOST_INFO_STRUCT          0x24245548  //$$UH
#define LEGACY_HOST_INFO_STRUCT_SIZE     0x4
#define URP_COUNT_SIZE                   0x1
#define URP_INFO_SIZE                    0x5
#define MEMORY_OFFSET_1M                 0x100000
#define MEMORY_OFFSET_896K               0xe0000
#define IRQ0_LEGACY_INT_OFFSET	         0x08
#define IRQ8_LEGACY_INT_OFFSET		 0x70


#define LEGACY_ISR_TABLE_STRUCT1         0x24245531  //$$U1
#define LEGACY_ISR_TABLE_STRUCT1_SIZE    0x04
#define LEGACY_ISR_TABLE_STRUCT2         0x24245532  //$$U2
#define LEGACY_ISR_TABLE_STRUCT2_SIZE    0x04

#define GenericIrqHandler(x) VOID UsbInterruptHandler##x (\
    IN EFI_EXCEPTION_TYPE   InterruptType,\
    IN EFI_SYSTEM_CONTEXT   SystemContext\
)\
{\
    UsbInterruptHandler(x);\
}

typedef VOID (INTERRUPT_HANDLER_MAP)(
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext );



/**
  USB IRQ Interrupt Handler

  @param Irq           - USB IRQ number.

*/

VOID
EFIAPI
UsbInterruptHandler(
    EFI_8259_IRQ    Irq
)
{
    EFI_TPL     OldTpl;
    URP_STRUC   Params;
    UINTN       Index;
#if USB_RUNTIME_DRIVER_IN_SMM
    URP_STRUC   *UrpTemp=NULL;
    UINT8       SwSmiValue = USB_SWSMI;
    UINT8       DataSize = 1;
#endif

    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

    // clear the interrupt flag
    mLegacy8259->EndOfInterrupt(mLegacy8259, Irq);
   
    //Find Usb Host & exectue interrupt.
    for (Index = 0;Index < gUsbData->NumOfHc;Index++){
        if (gUsbData->HcTable[Index]->Irq == Irq) {
            Params.bFuncNumber = USB_API_HC_PROC;
            Params.bSubFunc = opHC_ProcessInterrupt;
            Params.bRetValue = USB_ERROR;
            Params.ApiData.HcProc.paramBuffer = &gUsbData->HcTable[Index];
            Params.ApiData.HcProc.paramSize = sizeof(VOID*)*1;
            Params.ApiData.HcProc.bHCType = gUsbData->HcTable[Index]->bHCType;
            Params.ApiData.HcProc.retVal = 0;
            if (gAmiUsbController->UsbInvokeApi) {
                gAmiUsbController->UsbInvokeApi(&Params);
            } else {
                UrpTemp = gUsbData->fpURP;
                gUsbData->fpURP = &Params;
                gSmmCtl->Trigger(gSmmCtl, &SwSmiValue, &DataSize, 0, 0);
                gUsbData->fpURP = UrpTemp;
            }
        }
    }

    pBS->RestoreTPL(OldTpl);
}

//
// Usb Interrutp Handler
//
    GenericIrqHandler(0)
    GenericIrqHandler(1)
    GenericIrqHandler(2)
    GenericIrqHandler(3)
    GenericIrqHandler(4)
    GenericIrqHandler(5)
    GenericIrqHandler(6)
    GenericIrqHandler(7)
    GenericIrqHandler(8)
    GenericIrqHandler(9)
    GenericIrqHandler(10)
    GenericIrqHandler(11)
    GenericIrqHandler(12)
    GenericIrqHandler(13)
    GenericIrqHandler(14)
    GenericIrqHandler(15)

//
// Usb Interrupt Handler list
//
INTERRUPT_HANDLER_MAP* UsbInterruptHandlerList[] = { 
    UsbInterruptHandler0,
    UsbInterruptHandler1,
    UsbInterruptHandler2,
    UsbInterruptHandler3,
    UsbInterruptHandler4,
    UsbInterruptHandler5,
    UsbInterruptHandler6,
    UsbInterruptHandler7,
    UsbInterruptHandler8,	
    UsbInterruptHandler9,
    UsbInterruptHandler10,
    UsbInterruptHandler11,
    UsbInterruptHandler12,
    UsbInterruptHandler13,
    UsbInterruptHandler14,
    UsbInterruptHandler15,
    NULL };

/**
    The Eevent wait for Legacy Bios protocol ready.

    @param Event 
    @param Context 

    @retval None

**/
    
VOID
EFIAPI
CsmReadyEvent (
    IN    EFI_EVENT     Event,
    IN    VOID          *Context )
{
    LegacyBiosInstall = TRUE;
}

/**
    Read To Boot Callback routine for init USB legacy ISR.

    @param Event 
    @param Context 

    @retval None

**/

VOID
EFIAPI
UsbIsrReadyToBoot(
    IN    EFI_EVENT     Event,
    IN    VOID          *Context
)
{
    UINT32      UsbIsrStructOffset;
    UINT32      UrpTempOffset = (UINT32)UrpBuffer;
    UINTN       Index;
    URP_STRUC   *Params;
    UINT32      IntTableOffset, IsrSegment;
    UINT16      IrqEnableTemp1=0,IrqEnableTemp2=0;

    // check LegacyBios protocol ready.
    if (LegacyBiosInstall == FALSE) return;
    // Check Usb Legacy Isr install.
    if(LegacyIsrInstall==TRUE) return;
    
    LegacyIsrInstall = TRUE;
    
    //Open shadow
    NbRuntimeShadowRamWrite(TRUE);
    
    // seek e0000~100000 memory to find '$$UH'
    for (UsbIsrStructOffset = MEMORY_OFFSET_896K; UsbIsrStructOffset < MEMORY_OFFSET_1M; ++UsbIsrStructOffset) {
        if (*(UINT32*)UsbIsrStructOffset == LEGACY_HOST_INFO_STRUCT) break;
    }
    UsbIsrStructOffset+=LEGACY_HOST_INFO_STRUCT_SIZE;
    
    // Set Urp info to shadow ram. 
    *(UINT8*)UsbIsrStructOffset = gUsbData->NumOfHc;
    UsbIsrStructOffset+=URP_COUNT_SIZE;

    for (Index = 0;Index < gUsbData->NumOfHc;Index++){
    	Params = (URP_STRUC*)UrpTempOffset;
    	Params->ApiData.HcProc.paramBuffer = &gUsbData->HcTable[Index];
        *(UINT8*)UsbIsrStructOffset = gUsbData->HcTable[Index]->Irq;
        *(UINT32*)(UsbIsrStructOffset+1) = UrpTempOffset;
        UsbIsrStructOffset += URP_INFO_SIZE;
        UrpTempOffset += sizeof(URP_STRUC);
    }

    // close shodow
    NbRuntimeShadowRamWrite(FALSE);

    // seek e0000~100000 memory to find '$$U1'
    for (UsbIsrStructOffset = MEMORY_OFFSET_896K; UsbIsrStructOffset < MEMORY_OFFSET_1M; ++UsbIsrStructOffset) {
        if (*(UINT32*)UsbIsrStructOffset == LEGACY_ISR_TABLE_STRUCT1) break;
    }
    UsbIsrStructOffset += LEGACY_ISR_TABLE_STRUCT1_SIZE;

    // seek e0000~100000 memory to find '$$U2'
    for (IsrSegment = MEMORY_OFFSET_896K; IsrSegment < MEMORY_OFFSET_1M; ++IsrSegment) {
        if (*(UINT32*)IsrSegment == LEGACY_ISR_TABLE_STRUCT2) break;
    }
    IsrSegment += LEGACY_ISR_TABLE_STRUCT2_SIZE;
    IsrSegment = IsrSegment - (UINT16)*(UINT16*)UsbIsrStructOffset;
    IsrSegment = IsrSegment >> 4;

    for (Index = 0;Index < gUsbData->NumOfHc;Index++){
        
        IrqEnableTemp2 = 1<<(gUsbData->HcTable[Index]->Irq);
        
        if (!(IrqEnableTemp2&IrqEnableTemp1)){
            IrqEnableTemp1 |= 1<<(gUsbData->HcTable[Index]->Irq);  
            
            IntTableOffset = (UINT32)gUsbData->HcTable[Index]->Irq;
            if (gUsbData->HcTable[Index]->Irq<8){
                IntTableOffset = (IntTableOffset + IRQ0_LEGACY_INT_OFFSET)*4;
            } else {
                IntTableOffset = (IntTableOffset - 8 + IRQ8_LEGACY_INT_OFFSET)*4;
            }
            *(UINT16*)IntTableOffset = *(UINT16*)(gUsbData->HcTable[Index]->Irq*2+UsbIsrStructOffset);
            *(UINT16*)(IntTableOffset+2) = (UINT16)IsrSegment;
        }     
    }
}

/**
  This function call to install the USB Legacy Irq handler.

  @param  This           - The AMI_USB_ISR_PROTOCOL instance.

  @retval    EFI_SUCCESS           - The USB handler was installed.
  @retval    EFI_UNSUPPORTED       - The platform does not support Usb irq interrupts.
  @retval    EFI_DEVICE_ERROR      - The Usb irq handler could not be registered.

*/

EFI_STATUS
EFIAPI
InstallUsbLegacyInterrupt(
	IN AMI_USB_ISR_PROTOCOL	*This
)
{
    if (UefiIsrInstall==FALSE) return EFI_SUCCESS;
    UsbIsrReadyToBoot( NULL,NULL );	
    return EFI_SUCCESS;
}

/**
  This function call to install the USB Irq handler.

  @param  This           - The AMI_USB_ISR_PROTOCOL instance.

  @retval    EFI_SUCCESS           - The USB handler was installed.
  @retval    EFI_UNSUPPORTED       - The platform does not support Usb irq interrupts.
  @retval    EFI_DEVICE_ERROR      - The Usb irq handler could not be registered.

*/

EFI_STATUS
EFIAPI
InstallUsbInterrupt(
	IN AMI_USB_ISR_PROTOCOL	*This
)
{
    EFI_STATUS            Status;
    UINT32                UsbVector = 0;
    VOID                  *InterruptHandlerAddr;
    UINT16                IrqEnableTemp = 1<<(gUsbData->HcTable[gUsbData->NumOfHc-1]->Irq);
    UINT32                UrpTempOffset = (UINT32)UrpBuffer;
    URP_STRUC             Params;

    if (UefiIsrInstall==FALSE) UefiIsrInstall = TRUE;
    
    // Init URP_STRUC data. 
    Params.bFuncNumber = USB_API_HC_PROC;
    Params.bSubFunc = opHC_ProcessInterrupt;
    Params.bRetValue = USB_ERROR;
    Params.ApiData.HcProc.paramBuffer = &gUsbData->HcTable[gUsbData->NumOfHc-1];
    Params.ApiData.HcProc.paramSize = sizeof(VOID*)*1;
    Params.ApiData.HcProc.bHCType = gUsbData->HcTable[gUsbData->NumOfHc-1]->bHCType;
    Params.ApiData.HcProc.retVal = 0;
    UrpTempOffset += sizeof(URP_STRUC) * (gUsbData->NumOfHc-1);
    // Check this USB Host IRQ Handle has been installed.
    if (IrqEnableStore&IrqEnableTemp){
        CopyMem((UINT32*)UrpTempOffset, &Params, sizeof(URP_STRUC));
        return EFI_SUCCESS;
    }
#if USB_RUNTIME_DRIVER_IN_SMM  
    if (!gSmmCtl){
        Status = pBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, &gSmmCtl);
        ASSERT_EFI_ERROR(Status);
    }
#endif
    	
    // Get Interrupt vector.
    Status = mLegacy8259->GetVector( mLegacy8259, gUsbData->HcTable[gUsbData->NumOfHc-1]->Irq, (UINT8 *) & UsbVector);
    ASSERT_EFI_ERROR(Status);

    // Get Usb Interrupt handler address.
    InterruptHandlerAddr = (VOID*)UsbInterruptHandlerList[gUsbData->HcTable[gUsbData->NumOfHc-1]->Irq];

    // Register USB interrupt handler
    Status = CpuArch->RegisterInterruptHandler(CpuArch, UsbVector, InterruptHandlerAddr);
    ASSERT_EFI_ERROR(Status);
    
    if (!EFI_ERROR(Status)){
        IrqEnableStore |= 1<<(gUsbData->HcTable[gUsbData->NumOfHc-1]->Irq);

        // Enable USB Irq.
        mLegacy8259->EnableIrq(mLegacy8259, gUsbData->HcTable[gUsbData->NumOfHc-1]->Irq, FALSE);
    
        // Enable slave 8259.   
        if (gUsbData->HcTable[gUsbData->NumOfHc-1]->Irq>7)
            mLegacy8259->EnableIrq(mLegacy8259, SLAVE_8259_IRQ, FALSE);
            
        CopyMem((UINT32*)UrpTempOffset, &Params, sizeof(URP_STRUC));
    }
  
    return Status;
}

//
// USB isr protocol
//
AMI_USB_ISR_PROTOCOL gAmiUsbIsrProtocol =
{
    InstallUsbInterrupt,
    InstallUsbLegacyInterrupt
};

/**
  Initialize the USB IRQ driver

  @param  ImageHandle - ImageHandle of the loaded driver
  @param  SystemTable - Pointer to the System Table

  @retval  EFI_SUCCESS           - USB ISR Protocol created
  @retval  EFI_OUT_OF_RESOURCES  - Not enough resources available to initialize driver.
  @retval  EFI_DEVICE_ERROR      - A device error occured attempting to initialize the driver.

*/

EFI_STATUS
EFIAPI 
UsbIrqInitEntry(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS            Status;
    EFI_GUID              gAmiUsbIsrProtocolGuid = AMI_USB_ISR_PROTOCOL_GUID;
    EFI_EVENT             ReadyToBoot;
    EFI_EVENT             mLegacyBiosEvent;
    VOID                  *mLegacyBiosReg;
    
    InitAmiLib(ImageHandle, SystemTable);

    // Find the Legacy Interrupt(8259) Protocol
    Status = gBS->LocateProtocol(&gEfiLegacy8259ProtocolGuid, 0, &mLegacy8259);
    ASSERT_EFI_ERROR(Status);

    // Find the CPU Arch Protocol
    Status = gBS->LocateProtocol(&gEfiCpuArchProtocolGuid, 0, &CpuArch);
    ASSERT_EFI_ERROR(Status);

    // Find the AMI USB protocol.
    Status = gBS->LocateProtocol(&gEfiUsbProtocolGuid, NULL, &gAmiUsbController);
    ASSERT_EFI_ERROR(Status);

    gUsbData = gAmiUsbController->USBDataPtr;
    
    UrpBuffer = (URP_STRUC*) 0xFFFFFFFF;
    Status = gBS->AllocatePages(
                 AllocateMaxAddress,
		 EfiRuntimeServicesData,
		 EFI_SIZE_TO_PAGES(sizeof(URP_STRUC) * 8),
		 (EFI_PHYSICAL_ADDRESS*)&UrpBuffer);
    ASSERT_EFI_ERROR(Status);
    
    // Find the Legacy Bios Protocol
    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, 0, &LegacyBios);
    if (!EFI_ERROR(Status)){
         LegacyBiosInstall = TRUE;
    } else {
        RegisterProtocolCallback(
                     &gEfiLegacyBiosProtocolGuid,
                     CsmReadyEvent,
                     NULL,
                     &mLegacyBiosEvent,
                     &mLegacyBiosReg);
    }
    // A event for init USB legacy isr.
    Status = EfiCreateEventReadyToBootEx(
                 TPL_CALLBACK, UsbIsrReadyToBoot,
                 NULL, &ReadyToBoot);
  
    Status = gBS->InstallProtocolInterface(
                    &ImageHandle,
                    &gAmiUsbIsrProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gAmiUsbIsrProtocol );
    return Status;
}
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
