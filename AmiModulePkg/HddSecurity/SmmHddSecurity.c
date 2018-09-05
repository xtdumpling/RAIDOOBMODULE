
//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file SmmHddSecurity.c
    Provide functions to unlock HDD password during S3 resume

**/

//---------------------------------------------------------------------------

#include "SmmHddSecurity.h"

#ifdef  SW_SMI_IO_ADDRESS
UINT8   SwSmiPort = SW_SMI_IO_ADDRESS;
#else
// Default Sw Smi port 
UINT8   SwSmiPort=0xB2;
#endif

//---------------------------------------------------------------------------

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
AMI_AHCI_BUS_SMM_PROTOCOL *mAhciSmm = NULL;
#endif

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
AMI_NVME_PASS_THRU_PROTOCOL     *gSmmNvmePassThru=NULL;
#endif

UINT8    *SecurityUnlockCommandBuffer = NULL;

EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch;
EFI_SMM_SYSTEM_TABLE2           *pSmst2;



COMMAND_BUFFER        *mCmdBuffer;
UINT8                 *mDataBuffer        = NULL;
UINT8                 *mDataSmmBuffer     = NULL;
UINT8                 *SecurityBuffer     = NULL;
BOOLEAN               AhciInit            = FALSE;
UINT64                PciExpressBaseAddress = 0;

#define PCI_CFG_ADDR( bus, dev, func, reg ) \
    ((VOID*)(UINTN) (PciExpressBaseAddress\
                     + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

#define AHCI_BAR    0x24
#define PCI_SCC     0x000A        // Sub Class Code Register


#if defined(EFI64) || defined(EFIx64)         // Check Flag to Select the Function for
                                              // the corresponding Execution Platform
/**
    Dividend input is divided by the Divisor input
    parameter and Stored the Remainder value in Remainder
    argument and Returns Quotient.This Function is selected in
    64 bit environment architecture. 

    @param Dividend 
    @param Divisor //Can only be 31 bits.
    @param Remainder OPTIONAL

    @retval static UINT64

**/

static
UINT64 DivU64x32Local (
    IN  UINT64              Dividend,
    IN  UINTN               Divisor,
    OUT UINTN*Remainder     OPTIONAL
)
{
    UINT64 Result = Dividend / Divisor;

    if ( Remainder ) {
        *Remainder = Dividend % Divisor;
    }
    return Result;
}

#else

/**
    Dividend input is divided by the Divisor input
    parameter and Stored the Remainder value in Remainder
    argument and Returns Quotient. This function is selected in
    32 bit environment architecture.

    @param Dividend 
    @param Divisor //Can only be 31 bits.
    @param Remainder OPTIONAL

    @retval static UINT64

**/

static
UINT64 DivU64x32Local (
    IN  UINT64              Dividend,
    IN  UINTN               Divisor,   //Can only be 31 bits.
    OUT UINTN*Remainder     OPTIONAL
)
{
    UINT64 Result;
    UINT32 Rem;

    _asm
    {
        mov eax, dword ptr Dividend[0]
        mov edx, dword ptr Dividend[4]
        mov esi, Divisor
        xor     edi, edi                    ; Remainder
        mov     ecx, 64                     ; 64 bits
Div64_loop:
        shl     eax, 1                      ;Shift dividend left. This clears bit 0.
        rcl     edx, 1
        rcl     edi, 1                      ;Shift remainder left. Bit 0 = previous dividend bit 63.

        cmp     edi, esi                    ; IF Rem >= Divisor, dont adjust
        cmc                                 ; ELSE adjust dividend and subtract divisor.
        sbb     ebx, ebx                    ; IF Rem >= Divisor, ebx = 0, ELSE ebx = -1.
        sub     eax, ebx                    ; IF adjust, bit 0 of dividend = 1
        and     ebx, esi                    ; IF adjust, ebx = Divisor, ELSE ebx = 0. 
        sub     edi, ebx                    ; IF adjust, subtract divisor from remainder.
        loop    Div64_loop

        mov     dword ptr Result[0], eax
        mov     dword ptr Result[4], edx
        mov     Rem, edi
    }

        if (Remainder) *Remainder = Rem;

        return Result;
}
#endif


/**
    Stalls for the Required Amount of MicroSeconds

    @param UINTN   Usec    // Number of microseconds to delay

    @retval VOID

**/
VOID 
Stall (
    UINTN           Usec
)
{
    UINTN   Counter, i;
    UINT32  Data32, PrevData;
    UINTN   Remainder;

  Counter = (UINTN)DivU64x32Local ((Usec * 10), 3, &Remainder);

  if (Remainder != 0) {
    Counter++;
  }

  //
  // Call WaitForTick for Counter + 1 ticks to try to guarantee Counter tick
  // periods, thus attempting to ensure Microseconds of stall time.
  //
  if (Counter != 0) {

    PrevData = IoRead32(PM_BASE_ADDRESS + 8);
    for (i=0; i < Counter; ) {
       Data32 = IoRead32(PM_BASE_ADDRESS + 8);    
        if (Data32 < PrevData) {        // Reset if there is a overlap
            PrevData=Data32;
            continue;
        }
        i += (Data32 - PrevData);        
        PrevData = Data32;
    }
  }
}

/**
    Clears the buffer

    @param void    *Buffer,
    @param UINTN   Size

    @retval VOID

**/
void
ZeroMemorySmm (
    void            *Buffer,
    UINTN           Size
 )
{
    UINT8   *Ptr;
    Ptr = Buffer;
    while (Size--) {
        *(Ptr++) = 0;
    }
}

/**
    Waits for the given bit to be clear

    @param HDD_PASSWORD            *pHddPassword,
    @param UINT8                   BitClear,
    @param UINT32                  Timeout

    @retval EFI_STATUS

**/
EFI_STATUS
SMMWaitforBitClear (
    HDD_PASSWORD        *pHddPassword,
    UINT8               BitClear,
    UINT32              Timeout
)
{
    UINT8           Delay;
    UINT8           Data8;

    for ( ; Timeout > 0; Timeout--) {
        for ( Delay = 100; Delay > 0; Delay--) {
            Data8 = IoRead8(pHddPassword->DeviceControlReg);
            if (!(Data8 & BitClear)) return EFI_SUCCESS;
            Stall(10);                      // 10 Usec
        }
    }
    return EFI_TIMEOUT;
}

/**
    Checks for a particular Bit to be set for a given amount 
    of time

    @param HDD_PASSWORD            *pHddPassword,
    @param UINT8                   BitSet,
    @param UINT32                  Timeout

    @retval EFI_STATUS

**/

EFI_STATUS
SMMWaitforBitSet (
    HDD_PASSWORD                    *pHddPassword,
    UINT8                           BitSet,
    UINT32                          TimeOut
)
{
    UINT8           Delay;
    UINT8           Data8;

    for ( ; TimeOut > 0; TimeOut--) {
        for ( Delay = 100; Delay > 0; Delay--) {
            Data8 = IoRead8(pHddPassword->DeviceControlReg);
            if (Data8 & BitSet) return EFI_SUCCESS;
            Stall(10);                      // 10 Usec
        }
    }
    return EFI_TIMEOUT;
}

/**
    Waits for BSY bit to get clear

    @param HDD_PASSWORD    *pHddPassword 

    @retval EFI_STATUS

    @note  Wait for BSY bit to get clear. Check for any errors.

**/
EFI_STATUS
SMMWaitForCmdCompletion (
    HDD_PASSWORD                    *pHddPassword
)
{
    UINT8           Data8;
    EFI_STATUS      Status;

//  Read ATL_STATUS and ignore the result. Just a delay
    Data8 = IoRead8(pHddPassword->DeviceControlReg);
    
//  Check for BSY bit to be clear
    Status = SMMWaitforBitClear (   pHddPassword, 
                                    IDE_BSY, 
                                    DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT);

    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

//  Check for errors. 
    Data8 = IoRead8(pHddPassword->BaseAddress + 7);

    if (Data8 & (IDE_ERR | IDE_DF)) return EFI_DEVICE_ERROR;  

    return EFI_SUCCESS;
}

#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )

/**
    This Function unlocks HDD password during S3 resume in 
    Ahci Mode Using Int 13.

    @param HDD_PASSWORD    *pHddPassword 

    @retval EFI_STATUS

**/
EFI_STATUS
SMMAhciSecurityUnlockCommand (
    HDD_PASSWORD                    *pHddPassword
)
{
    UINT8              i;
    UINT32             AhciBar = 0;
    UINT8              SccReg = 0 ;
    BOOLEAN            ControllerinAhciMode = FALSE;
    UINT32             GlobalControl;
    COMMAND_STRUCTURE  CommandStructure;
    UINT32             NoofPorts=0;

    if(mAhciSmm == NULL) {
        return EFI_NOT_FOUND;
    }

    //    1. Check if Controller is in AHCI Mode.
    //    2. Read ABAR Offset and Get HbaAddress.
    //    3. Disable AHCI_ENABLE. 
    //    4. Issue Controller Reset. 
    //    5. Wait for HBA Reset to Complete.
    //    6. Enable AHCI_ENABLE.

    AhciBar = *(UINT32*)PCI_CFG_ADDR(pHddPassword->BusNo,
                               pHddPassword->DeviceNo,
                               pHddPassword->FunctionNo,
                               AHCI_BAR);
    AhciBar &= 0xFFFFFFF0;
    
    mAhciSmm->AhciBaseAddress = AhciBar;

    // Find Number of Ports from BIT0:BIT4 of the HBA_CAP register
    NoofPorts = ((MMIO_READ32(AhciBar + HBA_CAP)) & HBA_CAP_NP_MASK) + 1;

    // Check if AHCI MMIO address space resides in SMRAM region. If yes, don't proceed.
    if( AmiValidateMmioBuffer( (VOID*)mAhciSmm->AhciBaseAddress, HBA_PORTS_START + (NoofPorts * HBA_PORTS_REG_WIDTH) ) ) {
        return EFI_NOT_FOUND;
    }

    if(!AhciInit) {
        SccReg = *(UINT8*)PCI_CFG_ADDR(pHddPassword->BusNo,
                                       pHddPassword->DeviceNo,
                                       pHddPassword->FunctionNo,
                                       PCI_SCC);    // Get Scc Register;

        if((SccReg & 0x06) || (SccReg & 0x04)) { 
            ControllerinAhciMode = TRUE;
        }

        if(ControllerinAhciMode) {
            GlobalControl = MMIO_READ32(AhciBar + 0x04);

            GlobalControl &= 0x7FFFFFFF;
            MMIO_WRITE32(AhciBar + 0x04, GlobalControl);

            GlobalControl = 0x01;
            MMIO_WRITE32(AhciBar + 0x04, GlobalControl);

            Stall(5000);   // 5 milliSec Delay

            GlobalControl = 0x80000000;
            MMIO_WRITE32(AhciBar + 0x04, GlobalControl);

            Stall(1000000);   // 1 Second Delay
        }
        AhciInit=TRUE;
    }

    //  Clear the Buffer
    ZeroMemorySmm (SecurityUnlockCommandBuffer, 512);
    SecurityUnlockCommandBuffer[0] = pHddPassword->Control & 1;;
    if(SecurityUnlockCommandBuffer[0]) {
    //Copy 32 Bytes of Password
        for (i = 0; i < IDE_PASSWORD_LENGTH; i++) {
            ((UINT8 *)SecurityUnlockCommandBuffer)[i + 2] = pHddPassword->MasterPassword[i];
        }
    } else {
    //Copy 32 Bytes of Password
        for (i = 0; i < IDE_PASSWORD_LENGTH; i++) {
                ((UINT8 *)SecurityUnlockCommandBuffer)[i + 2] = pHddPassword->UserPassword[i];
        }
    }
    //
    //Resuming from S3. So bring back the AHCI controller to working state
    //
    mAhciSmm->AhciSmmInitPortOnS3Resume(mAhciSmm,pHddPassword->PortNumber);
    
    //
    //Setup the Unlock command 
    //
    ZeroMemorySmm(&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.SectorCount = 1;
    CommandStructure.LBALow = 0;
    CommandStructure.LBAMid = 0;
    CommandStructure.LBAHigh = 0;
    CommandStructure.Device = 0x40;
    CommandStructure.Command = SECURITY_UNLOCK;
    CommandStructure.Buffer = SecurityUnlockCommandBuffer;
    CommandStructure.ByteCount = 512;
    //
    //Issue the unlock command
    //
    mAhciSmm->AhciSmmExecutePioDataCommand( mAhciSmm,
                                            &CommandStructure,
                                            pHddPassword->PortNumber,
                                            0xFF,
                                            ATA,
                                            TRUE);
 
#if DISABLE_SOFT_SET_PREV 
    ZeroMemorySmm (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Features = DISABLE_SATA2_SOFTPREV;         // Disable Software Preservation
    CommandStructure.SectorCount = 6;
    CommandStructure.Command = SET_FEATURE_COMMAND;

    mAhciSmm->AhciSmmExecuteNonDataCommand( mAhciSmm,
                                            CommandStructure,
                                            pHddPassword->PortNumber,
                                            0xFF,
                                            ATA
                                            );
#endif

    //
    //Issue the Security Freeze lock command
    //
    ZeroMemorySmm(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
    CommandStructure.Command = SECURITY_FREEZE_LOCK;
    mAhciSmm->AhciSmmExecuteNonDataCommand( mAhciSmm,
                                            CommandStructure,
                                            pHddPassword->PortNumber,
                                            0xFF,
                                            ATA
                                            );

    //
    //Issue the Device configure Freeze lock command 
    //
    ZeroMemorySmm (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Command = DEV_CONFIG_FREEZE_LOCK;
    CommandStructure.Features = DEV_CONFIG_FREEZE_LOCK_FEATURES;
    mAhciSmm->AhciSmmExecuteNonDataCommand( mAhciSmm,
                                            CommandStructure,
                                            pHddPassword->PortNumber,
                                            0xFF,
                                            ATA
                                            );

    return EFI_SUCCESS;

}
#endif

/**
    This Function unlocks HDD password during S3 resume.

    @param HDD_PASSWORD    *pHddPassword 

    @retval EFI_STATUS

**/
EFI_STATUS
SMMSecurityUnlockCommand (
    HDD_PASSWORD                    *pHddPassword
)
{
    EFI_STATUS                      Status;
    UINT8                           Data8;
    UINT8                           i;
    UINT16                          Reg;

//  Disable Interrupt
    IoWrite8(pHddPassword->DeviceControlReg, 2);

//  Select the drive
    IoWrite8(pHddPassword->BaseAddress + 6, pHddPassword->Device << 4);

//  Wait for BSY to go low
    Status = SMMWaitforBitClear (pHddPassword, IDE_BSY, S3_BUSY_CLEAR_TIMEOUT);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

//  Clear the Buffer
    ZeroMemorySmm (SecurityBuffer, 512);

    SecurityBuffer[0] = pHddPassword->Control & 1;

    if(SecurityBuffer[0]) {

//      Copy 32 Bytes of Password
   
        for (i = 0; i < IDE_PASSWORD_LENGTH; i++) {
            ((UINT8 *)SecurityBuffer)[i + 2] = pHddPassword->MasterPassword[i];
        }
    } else {

//      Copy 32 Bytes of Password
        for (i = 0; i < IDE_PASSWORD_LENGTH; i++) {
                ((UINT8 *)SecurityBuffer)[i + 2] = pHddPassword->UserPassword[i];
        }
    }

    Status = SMMIdeNonDataCommand (pHddPassword, 
                                    0,
                                    0,
                                    0,
                                    0,
                                    0, 
                                    SECURITY_UNLOCK); 
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

//  Wait for Command completion
    Status = SMMWaitForCmdCompletion (pHddPassword);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

//  Check for DRQ
    Status = SMMWaitforBitSet(pHddPassword, IDE_DRQ, DRQ_TIMEOUT);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    Reg = pHddPassword->BaseAddress;

//  Status = IdeWriteMultipleWord (pHddPassword->BaseAddress, 256, &SecurityBuffer);
    IoWrite(
        CpuIoWidthFifoUint16,
        Reg,
        256,
        SecurityBuffer
    );

//  Check for errors
    Status = SMMWaitForCmdCompletion (pHddPassword);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

//  Check for errors. 
    Data8 = IoRead8 ( pHddPassword->BaseAddress + 7);
    if(Data8 & 0x21) {          // ERR OR DF bit set ?
        return EFI_DEVICE_ERROR;  
    }

#if DISABLE_SOFT_SET_PREV 
    Status = SMMIdeNonDataCommand (pHddPassword, 
                                    DISABLE_SATA2_SOFTPREV,
                                    6,
                                    0,
                                    0,
                                    0, 
                                    SET_FEATURE_COMMAND);

            //  Check for errors
            Status = SMMWaitForCmdCompletion (pHddPassword);
            if (EFI_ERROR(Status)) return Status;
#endif

     Status = SMMIdeNonDataCommand (pHddPassword, 
                              DEV_CONFIG_FREEZE_LOCK_FEATURES,
                              0,
                              0,
                              0,
                              0, 
                              DEV_CONFIG_FREEZE_LOCK);
	 ASSERT_EFI_ERROR (Status);

//  Check for errors
        Status = SMMWaitForCmdCompletion (pHddPassword);
        if (EFI_ERROR(Status)) return Status;

    return EFI_SUCCESS;
}

/**
    Issues Set Feature command (Non Data)

    @param HDD_PASSWORD    *pHddPassword
    @param UINT8           Command

    @retval EFI_STATUS

**/
EFI_STATUS 
SMMIdeNonDataCommand (
    HDD_PASSWORD                        *pHddPassword,
    IN  UINT8                            Features,
    IN  UINT8                            SectorCount,
    IN  UINT8                            LBALow,
    IN  UINT8                            LBAMid,
    IN  UINT8                            LBAHigh,
    IN  UINT8                            Command
) 
{

    EFI_STATUS                      Status;
    UINT8                           Data8;

//  Select the drive
    IoWrite8(pHddPassword->BaseAddress + 6, pHddPassword->Device << 4);

//  Check for Controller presence
    Data8 = IoRead8(pHddPassword->DeviceControlReg);
    if (Data8 == 0xFF) {
        ASSERT(Data8 == 0xFF);
        return EFI_DEVICE_ERROR;
    }

//  Before Writing to Sector Count Register, BSY and DRQ bit should be zero
    Status = SMMWaitforBitClear(pHddPassword, IDE_BSY, S3_BUSY_CLEAR_TIMEOUT);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

//  Check for DRDY
    Status = SMMWaitforBitSet(pHddPassword, IDE_DRDY, DRDY_TIMEOUT);
    if (EFI_ERROR(Status)) return EFI_TIMEOUT;

//  Issue command

    IoWrite8 ( pHddPassword->BaseAddress + 1, Features);
    IoWrite8 ( pHddPassword->BaseAddress + 2, SectorCount);
    IoWrite8 ( pHddPassword->BaseAddress + 3, LBALow);
    IoWrite8 ( pHddPassword->BaseAddress + 4, LBAMid);
    IoWrite8 ( pHddPassword->BaseAddress + 5, LBAHigh);
    IoWrite8 ( pHddPassword->BaseAddress + 7, Command);

    return EFI_SUCCESS;

}
/**
    This function Locates the AhciSmm protocol from the Smm

    @param VOID

    @retval VOID

**/
EFI_STATUS
LocateSMMServices (
    IN  EFI_GUID    *VariableGuid,
    IN  VOID        **VariablePointer
)
{
    UINTN           Index;


    for (Index = 0; Index < pSmst2->NumberOfTableEntries; ++Index) {
        if (guidcmp(&pSmst2->SmmConfigurationTable[Index].VendorGuid,VariableGuid) == 0) {
            break;
        }
    }

    if (Index != pSmst2->NumberOfTableEntries) {
        *VariablePointer = pSmst2->SmmConfigurationTable[Index].VendorTable;
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

/**
    This function Locates the protocols and saves in global pointer

    @param VOID

    @retval VOID

**/
VOID LocateIdeSmmProtocols()
{
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if(mAhciSmm == NULL) {
        LocateSMMServices(&gAmiAhciSmmProtocolGuid,(VOID **)&mAhciSmm);
    }
#endif
 
    return ;  
}

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
/**
    This function Locates the protocols to access Nvme controller and saves in global pointer

    @param VOID

    @retval VOID

**/
EFI_STATUS LocateNvmeSmmProtocols(
    EFI_HANDLE                    NvmeControllerHandle,
    AMI_NVME_CONTROLLER_PROTOCOL  **SmmNvmeController
)
{
    EFI_STATUS  Status=EFI_NOT_FOUND;
    EFI_HANDLE  *HandleBuffer = NULL;
    UINTN       i, HandleBufferSize = 0;

    if(gSmmNvmePassThru == NULL) {
        Status = pSmst2->SmmLocateProtocol(&gAmiSmmNvmePassThruProtocolGuid, NULL, &gSmmNvmePassThru);
        if(EFI_ERROR(Status)) {
            return Status;
        }
    }

    Status = pSmst2->SmmLocateHandle( ByProtocol, 
                                      &gAmiSmmNvmeControllerProtocolGuid,
                                      NULL,
                                      &HandleBufferSize, 
                                      HandleBuffer );
        
    if( EFI_ERROR(Status) ) {
        if( Status == EFI_BUFFER_TOO_SMALL ) {
            Status = pSmst2->SmmAllocatePool( EfiRuntimeServicesCode,
                                              HandleBufferSize,
                                              (VOID**)&HandleBuffer );
            if( EFI_ERROR(Status) ) {
                ASSERT(FALSE);
                return Status;
            }

            Status = pSmst2->SmmLocateHandle( ByProtocol, 
                                              &gAmiSmmNvmeControllerProtocolGuid,
                                              NULL,
                                              &HandleBufferSize, 
                                              HandleBuffer );
        }
    }

    if( !EFI_ERROR(Status) ) {
        for(i=0; i<HandleBufferSize/sizeof(EFI_HANDLE); i++) {
            Status = pSmst2->SmmHandleProtocol( HandleBuffer[i], 
                                                &gAmiSmmNvmeControllerProtocolGuid, 
                                                SmmNvmeController );
                
            if( !EFI_ERROR(Status) && (*SmmNvmeController)->ControllerHandle == NvmeControllerHandle ) {
                Status = EFI_SUCCESS;
                break;
            }
        }

        if( i == HandleBufferSize/sizeof(EFI_HANDLE) ) {
            Status = EFI_NOT_FOUND;
        }
    }

    // Free HandleBuffer memory
    if( HandleBuffer ) {
        pSmst2->SmmFreePool(HandleBuffer);
    }

    return Status;
}
#endif

/**
    This function saves the HDD password and other information 
    necessary to unlock HDD password during S3 Resume.

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS 
EFIAPI
SaveHDDPassword (
    IN  EFI_HANDLE      DispatchHandle,
    IN  CONST   VOID    *DispatchContext OPTIONAL,
    IN  OUT VOID        *CommBuffer      OPTIONAL,
    IN  OUT UINTN       *CommBufferSize  OPTIONAL
)
{

    HDD_PASSWORD    *pHddPassword, *StoredHDDList;
    DLINK           *dlink;
    UINT8           i;
    BOOLEAN         UserOrMaster = FALSE;
    BOOLEAN         CheckFlag;
    EFI_STATUS      Status=EFI_SUCCESS;

    LocateIdeSmmProtocols();

    pHddPassword = (HDD_PASSWORD *)CommBuffer;

    if (!pHddPassword) return Status; 

//  Check if signature is present or not
    if ((UINT32) pHddPassword->Signature != '$HDD') {
        return Status;
    }

//  Check whether info about the HDD is already present
    dlink = gPasswordList.pHead;
    for ( ; dlink; dlink = dlink->pNext) {
        StoredHDDList = OUTTER(dlink, LinkList, HDD_PASSWORD);
        CheckFlag = FALSE;

        // Check if drive is connected to same controller
        if ((StoredHDDList->BusNo      != pHddPassword->BusNo   ) ||
            (StoredHDDList->DeviceNo   != pHddPassword->DeviceNo) ||
            (StoredHDDList->FunctionNo != pHddPassword->FunctionNo)) {
            continue;
        }

        if(StoredHDDList->BusInterfaceType == AmiStorageInterfaceAhci) {
            if ((StoredHDDList->PortNumber   == pHddPassword->PortNumber) && 
                (StoredHDDList->PMPortNumber == pHddPassword->PMPortNumber)) {
                CheckFlag = TRUE;
            }
        } else if(StoredHDDList->BusInterfaceType == AmiStorageInterfaceIde ) {
            if ((StoredHDDList->BaseAddress == pHddPassword->BaseAddress) && 
                (StoredHDDList->Device      == pHddPassword->Device)) {
                CheckFlag = TRUE;
            }
        } else { // i.e. StoredHDDList->BusInterfaceType == AmiStorageInterfaceNvme
            if ( StoredHDDList->DevHandle == pHddPassword->DevHandle ) {
                CheckFlag = TRUE;
            }
        }

        if (CheckFlag) {
            UserOrMaster = (BOOLEAN)((pHddPassword->Control) & 0x01);

            if(UserOrMaster){
//              Match has been found. Just copy the Password buffer
                for (i = 0; i < sizeof (pHddPassword->MasterPassword); i++) StoredHDDList->MasterPassword[i] = pHddPassword->MasterPassword[i];
         
            } else {
//              Match has been found. Just copy the Password buffer
                for (i = 0; i < sizeof (pHddPassword->UserPassword); i++) StoredHDDList->UserPassword[i] = pHddPassword->UserPassword[i];
            }
            return Status;
        }
    }

    //  Allocate memory needed while unlocking the Password. Done only once. 
    //  Same buffer will be reused.
    if (SecurityBuffer == NULL) { 
        pSmst2->SmmAllocatePool(EfiRuntimeServicesData, 512, (VOID**)&SecurityBuffer);
    }

    //  Match has not been found. Allocate memory and copy the buffer.
    if (pSmst2->SmmAllocatePool(EfiRuntimeServicesData, sizeof(HDD_PASSWORD), (VOID**)&StoredHDDList) != EFI_SUCCESS){ 
        return Status;
    }
    
    for (i = 0; i < sizeof (HDD_PASSWORD); i++) ((UINT8 *)StoredHDDList)[i] = ((UINT8 *)pHddPassword)[i];
    DListAdd(&gPasswordList, &StoredHDDList->LinkList); 

    return Status;
}

/**
    This Function unlocks HDD password during S3 resume.

  @param DispatchHandle  Handle to the Dispatcher
  @param Context         A Pointer to EFI_SMM_SW_REGISTER_CONTEXT.
  @param CommBuffer      A pointer to EFI_SMM_SW_CONTEXT.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS 
EFIAPI
UnlockHDDPassword (
    IN  EFI_HANDLE       DispatchHandle,
    IN  CONST VOID       *DispatchContext OPTIONAL,
    IN  OUT VOID         *CommBuffer OPTIONAL,
    IN  OUT UINTN        *CommBufferSize OPTIONAL
)
{
    HDD_PASSWORD    *StoredHDDList;
    DLINK           *dlink;
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    HDD_PASSWORD    *StoredHDDList2;
    DLINK           *dlink2;
#endif    
    EFI_STATUS      Status=EFI_SUCCESS;

    dlink = gPasswordList.pHead;
    for ( ; dlink; dlink = dlink->pNext) {
        StoredHDDList = OUTTER(dlink, LinkList, HDD_PASSWORD);
        if (StoredHDDList->Signature == '$HDD') {
            if(StoredHDDList->BusInterfaceType == AmiStorageInterfaceIde ) {
                SMMSecurityUnlockCommand (StoredHDDList);
            } else if (StoredHDDList->BusInterfaceType == AmiStorageInterfaceAhci ){
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
                // Check if this is the first entry for this controller.
                // If yes, we have to make sure the controller will be fully re-initialized
                AhciInit = FALSE;
                for (dlink2 = dlink->pPrev; dlink2; dlink2 = dlink2->pPrev) {
                    StoredHDDList2 = OUTTER(dlink2, LinkList, HDD_PASSWORD);
                    if ((StoredHDDList2->BusNo      == StoredHDDList->BusNo   ) &&
                        (StoredHDDList2->DeviceNo   == StoredHDDList->DeviceNo) &&
                        (StoredHDDList2->FunctionNo == StoredHDDList->FunctionNo)) {
                        // there was another one previously, so initialization is already done
                        AhciInit = TRUE;
                        break;
                    }
                }
                SMMAhciSecurityUnlockCommand (StoredHDDList);
#endif
            } else { // i.e. StoredHDDList->BusInterfaceType == AmiStorageInterfaceNvme
                SMMUnlockNvmeDevice(StoredHDDList);
            }
        }
    }

    return Status;
}

/**
    This Function removes HDD password from the internal database

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
EFIAPI
RemoveHDDPassword (
    IN  EFI_HANDLE      DispatchHandle,
    IN  CONST VOID      *DispatchContext OPTIONAL,
    IN  OUT VOID        *CommBuffer      OPTIONAL,
    IN  OUT UINTN       *CommBufferSize  OPTIONAL
)
{
    HDD_PASSWORD    *pHddPassword, *StoredHDDList;
    DLINK           *dlink;
    EFI_STATUS      Status=EFI_SUCCESS;

    LocateIdeSmmProtocols();
   
    pHddPassword = (HDD_PASSWORD *)CommBuffer;
    
    if (!pHddPassword) return Status; 

//  Check if signature is present or not
    if ((UINT32) pHddPassword->Signature != '$HDD') {
        return Status;
    }

//  Check whether info about the HDD is already present
    dlink = gPasswordList.pHead;
    for ( ; dlink; dlink = dlink->pNext) {
        StoredHDDList = OUTTER(dlink, LinkList, HDD_PASSWORD);

        // Check if drive is connected to same controller
        if ((StoredHDDList->BusNo      != pHddPassword->BusNo   ) ||
            (StoredHDDList->DeviceNo   != pHddPassword->DeviceNo) ||
            (StoredHDDList->FunctionNo != pHddPassword->FunctionNo)) {
            continue;
        }

        if(StoredHDDList->BusInterfaceType == AmiStorageInterfaceAhci) {
            if ((StoredHDDList->PortNumber   == pHddPassword->PortNumber) && 
                (StoredHDDList->PMPortNumber == pHddPassword->PMPortNumber)) {
                DListDelete(&gPasswordList, &StoredHDDList->LinkList);
            }
        } else if(StoredHDDList->BusInterfaceType == AmiStorageInterfaceIde) {
            if ((StoredHDDList->BaseAddress == pHddPassword->BaseAddress) && 
                (StoredHDDList->Device      == pHddPassword->Device)) {
                DListDelete(&gPasswordList, &StoredHDDList->LinkList);
            }
        } else {
            if ( StoredHDDList->DevHandle == pHddPassword->DevHandle ) {
                DListDelete(&gPasswordList, &StoredHDDList->LinkList);
            }
        }
    }
    return Status;
}

/**
    This Function Freeze locks HDD, Issues Disable/Enable Software
    Settings preservation Feature for Security Supported HDDs.

  @param DispatchHandle  Handle to the Dispatcher
  @param Context         A Pointer to EFI_SMM_SW_REGISTER_CONTEXT.
  @param CommBuffer      A pointer to EFI_SMM_SW_CONTEXT.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS 
EFIAPI
IdeSmmMiscSmiPassword (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL
)
{
    EFI_STATUS      Status;
    HDD_PASSWORD    *StoredHDDList;
    DLINK           *dlink;

    dlink = gPasswordList.pHead;
    for ( ; dlink; dlink = dlink->pNext) {
        StoredHDDList = OUTTER(dlink, LinkList, HDD_PASSWORD);
        if (StoredHDDList->Signature == '$HDD') {

            Status = SMMIdeNonDataCommand (StoredHDDList, 
                                    0,
                                    0,
                                    0,
                                    0,
                                    0, 
                                    SECURITY_FREEZE_LOCK);
				ASSERT_EFI_ERROR (Status);

            //	Check for errors
            Status = SMMWaitForCmdCompletion (StoredHDDList);
            if (EFI_ERROR(Status)) return Status;

#if DISABLE_SOFT_SET_PREV

            Status = SMMIdeNonDataCommand (StoredHDDList, 
                                    DISABLE_SATA2_SOFTPREV,
                                    6,
                                    0,
                                    0,
                                    0, 
                                    SET_FEATURE_COMMAND);

            //	Check for errors
            Status = SMMWaitForCmdCompletion (StoredHDDList);
            if (EFI_ERROR(Status)) return Status;
#endif

        }
    }

    return Status;
}

/**
  This Function Saves SMI value and SW SMI Port Address 
  in Boot Script for S3 resume during ReadytoBoot

  @param DispatchHandle  Handle to the Dispatcher
  @param Context         A Pointer to EFI_SMM_SW_REGISTER_CONTEXT.
  @param CommBuffer      A pointer to EFI_SMM_SW_CONTEXT.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
EFIAPI
BootScriptSaveHddSecSmmEventForS3 (
    IN  EFI_HANDLE       DispatchHandle,
    IN  CONST VOID       *DispatchContext OPTIONAL,
    IN  OUT VOID         *CommBuffer OPTIONAL,
    IN  OUT UINTN        *CommBufferSize OPTIONAL
)
{
    UINT8   Value = SW_SMI_HDD_UNLOCK_PASSWORD;

    S3BootScriptSaveIoWrite (
        S3BootScriptWidthUint8,
        (UINTN)SwSmiPort,
        1,
        &Value
        );

    return EFI_SUCCESS;
}

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
/**
    This function registers Boot script for Nvme devices to initialize Nvme 
    controller along with PCI root bridge BAR,CMD, BUS PCI Configuration 
    values in S3 resume path

  @param DispatchHandle  Handle to the Dispatcher
  @param Context         A Pointer to EFI_SMM_SW_REGISTER_CONTEXT.
  @param CommBuffer      A pointer to EFI_SMM_SW_CONTEXT.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS 
EFIAPI
BootScriptSaveNvmeSecSmmEventForS3 (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL
 )
{
    UINT32 SmmNvmeSaveEntryCount = 0;
    UINT32 NvmeBootScriptSaveBufferSize = 0;
    UINT32 EntryIndex = 0;
    UINTN  PciAddress = 0;
    EFI_BOOT_SCRIPT_WIDTH  Width      = 0;
    UINTN Value      = 0;
    BOOT_SCRIPT_NVME_SECURITY_SAVE_ENTRY *NvmeBootScriptSaveBuffer = NULL;
    
    NvmeBootScriptSaveBufferSize = (UINT32)*CommBufferSize;
	
    // Confirm that communication buffer contains required data
    NvmeBootScriptSaveBuffer = (BOOT_SCRIPT_NVME_SECURITY_SAVE_ENTRY *)*(UINTN*)CommBuffer;
    
    if (NvmeBootScriptSaveBuffer) {
        SmmNvmeSaveEntryCount = *(UINT32*)(((UINT8*)NvmeBootScriptSaveBuffer + NvmeBootScriptSaveBufferSize) - sizeof(UINT32));
    } else {
        return EFI_SUCCESS;
    }

    // Parse the NvmeBootScriptSaveBuffer for the Entry List and save the values to the BootScript.
	// Check for Signature to End of list in case SmmNvmeSaveEntryCount is corrupted
    for( EntryIndex = 0; EntryIndex < SmmNvmeSaveEntryCount; EntryIndex++ ) {

        if((UINT32)PciAddress == NVME_SECURITY_SAVE_SIGNATURE) {
            break;
        }

        PciAddress = NvmeBootScriptSaveBuffer[EntryIndex].PciAddress;
        Width = NvmeBootScriptSaveBuffer[EntryIndex].Width;
        Value = NvmeBootScriptSaveBuffer[EntryIndex].Value;

        TRACE((-1,"BootScriptSaveNvmeSecSmmEventForS3 .... %x %x %x %x ",EntryIndex, PciAddress, Width, Value ));

        S3BootScriptSavePciCfgWrite (
                  Width,
                  PciAddress,
                  1,
                  &Value
                  );
    }

    return EFI_SUCCESS;
}
#endif

/**
    This function Locates the AhciSMM driver and save pointer globally.

  @param DispatchHandle  Handle to the Dispatcher
  @param Context         A Pointer to EFI_SMM_SW_REGISTER_CONTEXT.
  @param CommBuffer      A pointer to EFI_SMM_SW_CONTEXT.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS 
EFIAPI
LocateAhciSMM (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL
 )
{
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
//APTIOV_SERVER_OVERRIDE_START: Security vulnerability fix.
    
    EFI_STATUS Status;
    
    if(SecurityUnlockCommandBuffer == NULL) {
//APTIOV_SERVER_OVERRIDE_END: Security vulnerability fix.
    // Unlock command Buffer
    SecurityUnlockCommandBuffer = (UINT8 *)(*(UINTN *)CommBuffer);
//APTIOV_SERVER_OVERRIDE_START: Security vulnerability fix.
        // Validate the AhciSecurityBuffer not resides in SMRAM region
        Status = AmiValidateMemoryBuffer((VOID*)SecurityUnlockCommandBuffer, 512);
        if(EFI_ERROR(Status)) return Status;
    }   
//APTIOV_SERVER_OVERRIDE_END: Security vulnerability fix.
#endif
    
    LocateIdeSmmProtocols();

    return EFI_SUCCESS;
}


/**
    Called from InstallSmiHandler

    @param 

    @retval VOID

**/
EFI_STATUS
InSmmFunction (
    IN  EFI_HANDLE       ImageHandle,
    IN  EFI_SYSTEM_TABLE *SystemTable
 )
{
    EFI_SMM_SW_REGISTER_CONTEXT Unlock_HDD_Password = {SW_SMI_HDD_UNLOCK_PASSWORD};
    EFI_SMM_SW_REGISTER_CONTEXT IdeSmm_MiscSmi_Password = {SW_SMI_HDD_MISC_SMM_FEATURES};
    EFI_STATUS  Status;
    EFI_HANDLE  Handle;

    Status = pSmst2->SmiHandlerRegister(
                        (VOID *)SaveHDDPassword,
                        &gSaveHddPasswordGuid,
                        &Handle
                        );
	
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;	
    }

    Status = pSmst2->SmiHandlerRegister(
                        (VOID *)RemoveHDDPassword,
                        &gRemoveHddPasswordGuid,
                        &Handle
                        );

    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;	
    }

    Status = SwDispatch->Register(
                        SwDispatch,
                        (VOID *)UnlockHDDPassword,
                        &Unlock_HDD_Password,
                        &Handle
                        );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;	
    }

    Status = SwDispatch->Register(
                    	SwDispatch,
                    	(VOID *)IdeSmmMiscSmiPassword ,
                    	&IdeSmm_MiscSmi_Password ,
                    	&Handle
                    	);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    
    Status = pSmst2->SmiHandlerRegister(
                                (VOID *) BootScriptSaveHddSecSmmEventForS3,
                                &gHddSecurityBootScriptSaveTriggerGuid,
                                &Handle
                                );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    
    Status = pSmst2->SmiHandlerRegister(
                          (VOID *) BootScriptSaveNvmeSecSmmEventForS3,
                          &gNvmeSecurityBootScriptSaveTriggerGuid,
                          &Handle
                          );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
#endif

    Status = pSmst2->SmiHandlerRegister(
                    	(VOID *)LocateAhciSMM,
                    	&gLocateIdeAhciSmmProtocolGuid,
                    	&Handle
                    	);
    
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
    }

    return EFI_SUCCESS;
}


/**
    Initializes HDD Password SMM Drivers.

    @param 

    @retval EFI_STATUS

    @note Here is the control flow of this function:
**/

EFI_STATUS
EFIAPI
HddHDDPasswordSMMInit (
    IN  EFI_HANDLE       ImageHandle,
    IN  EFI_SYSTEM_TABLE *SystemTable
 )
{
    EFI_STATUS              Status;
    EFI_SMM_BASE2_PROTOCOL  *gSmmBase2;

    InitAmiSmmLib(ImageHandle,SystemTable);
    
    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, (VOID**)&gSmmBase2);
    
    if (EFI_ERROR(Status)) { 
        return Status;
    }

    Status = gSmmBase2->GetSmstLocation( gSmmBase2, &pSmst2);
    
    if (EFI_ERROR(Status)) {  
        return EFI_UNSUPPORTED;
    }

    Status  = pSmst2->SmmLocateProtocol( &gEfiSmmSwDispatch2ProtocolGuid, \
                                          NULL, \
                                          (VOID**)&SwDispatch );
    
    if (EFI_ERROR(Status)) {  
        return EFI_UNSUPPORTED;
    }
    

    DListInit(&gPasswordList);
    
    PciExpressBaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
    
    return (InitSmmHandler (ImageHandle, SystemTable, InSmmFunction, NULL));
}

/**
    Routine to Unlock Nvme controller

    @param pHddPassword

    @retval EFI_SUCCESS NvmeController is unlocked successfully

**/
EFI_STATUS
SMMUnlockNvmeDevice (
    HDD_PASSWORD    *pHddPassword
)
{
    EFI_STATUS                    Status=EFI_SUCCESS;
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    UINT16                        SecurityStatus;
    AMI_NVME_CONTROLLER_PROTOCOL  *SmmNvmeController=NULL;

    if( EFI_ERROR( LocateNvmeSmmProtocols( pHddPassword->DevHandle, &SmmNvmeController ) )) {
        return EFI_DEVICE_ERROR;
    }

    Status = SmmNvmeController->NvmeInitController(SmmNvmeController);

    if(EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    Status = GetNvmeDeviceSecurityStatus(
                SmmNvmeController,
                &SecurityStatus );

    // if Nvme device is locked, send Unlock command
    if( !EFI_ERROR(Status) && ( SecurityStatus & NVME_SECURITY_LOCK_MASK ) ) {
        Status = SendNvmeSecurityUnlockCommand (
                 SmmNvmeController,
                 pHddPassword
                 );
        
    }
#endif
    return Status;
}

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
/**
    Routine to get Nvme controller security data

    @param SmmNvmeController
    @param SecurityStatus

    @retval EFI_STATUS

**/
EFI_STATUS
GetNvmeDeviceSecurityStatus(
    AMI_NVME_CONTROLLER_PROTOCOL  *SmmNvmeController,
    UINT16                        *SecurityStatus 
)
{
    EFI_STATUS                    Status;
    NVME_COMMAND_WRAPPER          *NvmeCmdWrapper = SmmNvmeController->NvmeCmdWrapper;
    NVME_SECURITY_RECEIVE_BUFFER  *SecReceiveBuffer=\
            (NVME_SECURITY_RECEIVE_BUFFER*)SecurityUnlockCommandBuffer;
    COMPLETION_QUEUE_ENTRY        *NvmeCompletionData =\
          (COMPLETION_QUEUE_ENTRY *)(SecurityUnlockCommandBuffer+100);  // Use Global buffer for NVMe completion entry

    ZeroMemorySmm (NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER));
    ZeroMemorySmm (SecReceiveBuffer,  sizeof(NVME_SECURITY_RECEIVE_BUFFER));
    ZeroMemorySmm (NvmeCompletionData, sizeof(COMPLETION_QUEUE_ENTRY));
    
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = SECURITY_RECEIVE;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
#if NVME_DRIVER_VERSION > 10
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = SmmNvmeController->NvmeDxeSmmDataArea->CommandIdentifierAdmin;
#else
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = SmmNvmeController->CommandIdentifierAdmin;
#endif
    NvmeCmdWrapper->NvmCmd.NSID = 0;
    NvmeCmdWrapper->NvmCmd.MPTR = 0;
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;  // Queue 0 for Admin cmds,1 for I/O command
    NvmeCmdWrapper->CmdTimeOut = 0x1000;
    
    NvmeCmdWrapper->NvmCmd.CDW10 = (((UINT32)NVME_SECUIRTY_SECP)<<24);
    NvmeCmdWrapper->NvmCmd.CDW11 = 0x10;
    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)SecReceiveBuffer;
    NvmeCmdWrapper->NvmCmd.PRP2 = 0;
    
    Status = gSmmNvmePassThru->ExecuteNvmeCmd (
                  SmmNvmeController, 
                  NvmeCmdWrapper, 
                  NvmeCompletionData );

    // Return Nvme device security status
    *SecurityStatus = SecReceiveBuffer->NvmeSecStatus;
    return Status;
}

/**
    Routine sends SecurityUnlock command to Nvme controller

    @param SmmNvmeController
    @param pHddPassword

    @retval EFI_STATUS

**/
EFI_STATUS
SendNvmeSecurityUnlockCommand (
    AMI_NVME_CONTROLLER_PROTOCOL    *SmmNvmeController,
    HDD_PASSWORD                    *pHddPassword
)
{
    NVME_COMMAND_WRAPPER    *NvmeCmdWrapper = SmmNvmeController->NvmeCmdWrapper;
    UINT8                   *Buffer = SecurityUnlockCommandBuffer;
    COMPLETION_QUEUE_ENTRY   NvmeCompletionData;
    UINTN                    i;
    EFI_STATUS               Status;
    
    //  Clear the Buffer
    ZeroMemorySmm (NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER));
    ZeroMemorySmm (Buffer, 512);

    Buffer[1] = (UINT8)(pHddPassword->Control & 0x1);

    // Buffer[1] will 0/1 i.e. User/Master password
    if( Buffer[1] ){
        // Match has been found. Just copy the Password buffer
        for (i = 0; i < sizeof (pHddPassword->MasterPassword); i++) {
            Buffer[i+2] = pHddPassword->MasterPassword[i];
        }
    } else {
        // Match has been found. Just copy the Password buffer
        for (i = 0; i < sizeof (pHddPassword->UserPassword); i++) {
            Buffer[i+2] = pHddPassword->UserPassword[i];
        }
    }

    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = SECURITY_SEND;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;

#if NVME_DRIVER_VERSION > 10
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = SmmNvmeController->NvmeDxeSmmDataArea->CommandIdentifierAdmin;
#else
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = SmmNvmeController->CommandIdentifierAdmin;
#endif

    NvmeCmdWrapper->NvmCmd.NSID = 0;
    NvmeCmdWrapper->NvmCmd.MPTR = 0;
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;  // Queue 0 for Admin cmds,1 for I/O command
    NvmeCmdWrapper->CmdTimeOut = 0x1000;
    
    NvmeCmdWrapper->NvmCmd.CDW10 = (((UINT32)NVME_SECUIRTY_SECP)<<24) + (((UINT32)NVME_ATA_SECURITY_UNLOCK)<<8);
    NvmeCmdWrapper->NvmCmd.CDW11 = NVME_SECURITY_SEND_BUFFER_SIZE;
    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)Buffer;
    NvmeCmdWrapper->NvmCmd.PRP2 = 0;
    
    Status = gSmmNvmePassThru->ExecuteNvmeCmd (
                  SmmNvmeController, 
                  NvmeCmdWrapper, 
                  &NvmeCompletionData );

    return Status;
}
#endif

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
