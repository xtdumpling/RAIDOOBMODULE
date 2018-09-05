//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AhciSmm.c
    AHCISmm function implementation.

**/

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/IdeControllerInit.h>
#include "Protocol/PciIo.h"
#include "Protocol/BlockIo.h"
#include <Protocol/AmiAhciBus.h>
#include <Protocol/AhciSmmProtocol.h>
#include "AhciSmm.h"
#include <Protocol/SmmBase.h>
#include "AhciController.h"

//---------------------------------------------------------------------------


#define SCC_AHCI_CONTROLLER         0x06
#define SCC_RAID_CONTROLLER         0x04
#define PCI_SCC                     0x000A  // Sub Class Code Register

UINT64      gCommandListBaseAddress;
UINT64      gFisBaseAddress;

AMI_AHCI_BUS_SMM_PROTOCOL   AhciSmm;
BOOLEAN     gPortReset = FALSE;             // Avoid Re-entry
UINT8       *SenseData=NULL;
UINT64 PciExpressBaseAddress = 0;
#if INDEX_DATA_PORT_ACCESS
extern UINT16 IndexPort, DataPort;
#endif

/**
    Stalls for the Required Amount of MicroSeconds

    @param 
        UINTN   Usec    // Number of microseconds to delay

    @retval VOID

**/
VOID 
SmmStall (
    UINTN           Usec
)
{
    UINTN     Counter, i;
    UINT32    Data32, PrevData;
    UINT32    Remainder;

    Counter = (UINTN)DivU64x32Remainder ((Usec * 10), 3, &Remainder);

    if (Remainder != 0) {
        Counter++;
    }

    // Call WaitForTick for Counter + 1 ticks to try to guarantee Counter tick
    // periods, thus attempting to ensure Microseconds of stall time.
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
    return;
}

/**
    Clears the buffer

    @param void    *Buffer,
        UINTN   Size

    @retval VOID

**/
void
ZeroMemorySmm (
    void    *Buffer,
    UINTN    Size
 )
{
    UINT8    *Ptr;
    Ptr = Buffer;
    while (Size--) {
        *(Ptr++) = 0;
    }
}

/**
    Wait for memory to be set to the test value.

    @param    SATA_DEVICE_INTERFACE   *SataDevInterface,
    @param    PMPort
    @param    Register
    @param    MaskValue        - The mask value of memory
    @param    TestValue        - The test value of memory
    @param    WaitTimeInMs     - The time out value for wait memory set

    @retval    EFI_SUCCESS HBA reset successfully.
    @retval EFI_DEVICE_ERROR HBA failed to complete hardware reset.

**/ 
EFI_STATUS 
WaitforPMMemSet (
    IN AMI_AHCI_BUS_SMM_PROTOCOL   *SataDevInterface,
    IN UINT8                    PMPort,
    IN UINT8                    Register,
    IN UINT32                   AndMask,
    IN UINT32                   TestValue,
    IN UINT32                   WaitTimeInMs
)
{
    UINT32    Data32;

    while(WaitTimeInMs!=0){ 
        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, FALSE);
        if((Data32 & AndMask) == TestValue) {return EFI_SUCCESS;}
        SmmStall (1000);  //  1Msec
        WaitTimeInMs--;
   }
   return EFI_DEVICE_ERROR;
}

/**
    Wait for memory to be set to the test value.

    @param    MemTestAddr      - The memory address to test
    @param    MaskValue        - The mask value of memory
    @param    TestValue        - The test value of memory
    @param    WaitTimeInMs     - The time out value for wait memory set

    @retval    EFI_SUCCESS HBA reset successfully.
    @retval EFI_DEVICE_ERROR HBA failed to complete hardware reset.

**/ 
EFI_STATUS 
WaitForMemSet (
    IN UINT32    BaseAddr,
    IN UINT8     Port,
    IN UINT8     Register,
    IN UINT32    AndMask,
    IN UINT32    TestValue,
    IN UINT32    WaitTimeInMs
)
{
    UINT8    Delay;
    while(WaitTimeInMs!=0){ 
        for ( Delay = 10; Delay > 0; Delay--) {
            if(((SMM_HBA_PORT_REG32(BaseAddr, Port, Register)) & AndMask) == TestValue) {return EFI_SUCCESS;}
            SmmStall (100);  // 100 usec * 10 = 1Msec
        }
        WaitTimeInMs--;
    }
   return EFI_DEVICE_ERROR;
}

/**
    Wait for memory to be set to the test value.

    @param MemTestAddr      - The memory address to test
        MaskValue        - The mask value of memory
        WaitTimeInMs     - The time out value for wait memory set

    @retval EFI_SUCCESS HBA reset successfully.
    @retval EFI_DEVICE_ERROR HBA failed to complete hardware reset.

**/ 
EFI_STATUS 
WaitForMemClear (
    IN UINT32    BaseAddr,
    IN UINT8     Port,
    IN UINT8     Register,
    IN UINT32    AndMask,
    IN UINT32    WaitTimeInMs
)
{
    UINT8    Delay;
    while(WaitTimeInMs!=0){ 
        for ( Delay = 10; Delay > 0; Delay--) {
            if(!((SMM_HBA_PORT_REG32(BaseAddr, Port, Register)) & AndMask)) {return EFI_SUCCESS;}
            SmmStall (100);  // 100 usec * 10 = 1Msec
        }
        WaitTimeInMs--;
    }
    return EFI_DEVICE_ERROR;
}

/**

    @param AhciBusInterface 
    @param SataDevInterface 
    @param Port 
    @param PMPort 
    @param Register (0 : SStatus 1: SError 2: SControl)

    @retval 
        UINT32
    @note  
  1. Check if the device is connected directly to the port
  2. if yes, read to the AHCI Controller else write to the Port Multiplier register. 

**/ 
UINT32
ReadSCRRegister (
    IN AMI_AHCI_BUS_SMM_PROTOCOL   *SataDevInterface,
    IN UINT8                    Port, 
    IN UINT8                    PMPort, 
    IN UINT8                    Register
)
{

    UINT32    Data32 = 0;
    UINT32    Reg = HBA_PORTS_SSTS;

    if (PMPort != 0xFF) {
        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, FALSE);
    }
    else {
        if (Register == 1) Reg = HBA_PORTS_SCTL;
        if (Register == 2) Reg = HBA_PORTS_SERR;
        Data32 = SMM_HBA_PORT_REG32 (SataDevInterface->AhciBaseAddress, Port, Reg);
    }

    return Data32;
}

/**
    Write to SCONTROL/Serror/SStatus register
                   
    @param    AhciBusInterface 
    @param    SataDevInterface 
    @param    Port 
    @param    PMPort 
    @param    Register (0 : SStatus 1: SError 2: SControl)
    @param    Data32 
  
    @retval   EFI_STATUS

    @note    1. Check if the device is connected directly to the port
             2. if yes, write to the AHCI Controller else write to the Port Multiplier register

**/ 

EFI_STATUS
WriteSCRRegister (
    IN AMI_AHCI_BUS_SMM_PROTOCOL   *SataDevInterface,
    IN UINT8                    Port, 
    IN UINT8                    PMPort, 
    IN UINT8                    Register,
    IN UINT32                   Data32
)
{

    UINT32    Reg = HBA_PORTS_SSTS;

    if (PMPort != 0xFF) {
        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, TRUE);
    }
    else {
        if (Register == 2) Reg = HBA_PORTS_SCTL;
        if (Register == 1) Reg = HBA_PORTS_SERR;
        SMM_HBA_PORT_REG32_OR (SataDevInterface->AhciBaseAddress, Port, Reg, Data32); 
    }

    return EFI_SUCCESS;
}

/**
    Wait till command completes

                   
    @param SataDevInterface 
    @param CommandType 
    @param TimeOut 

    @retval 
        EFI_STATUS

    @note  
  1. Check for SError bits. If set return error.
  2. For PIO IN/Out and Packet IN/OUT command wait till PIO Setup FIS is received
  3. If D2H register FIS is received, exit the loop.
  4. Check for SError and TFD bits.

**/
EFI_STATUS
WaitforCommandComplete  (
    IN AMI_AHCI_BUS_SMM_PROTOCOL          *SataDevInterface,
    IN COMMAND_TYPE             CommandType,
    IN UINTN                    TimeOut    
)
{

    UINT32    AhciBaseAddr = (UINT32)SataDevInterface->AhciBaseAddress;
    UINT8     Port = SataDevInterface->PortNumber;
    UINT32    Data32_SERR, Data32_IS, i;
    BOOLEAN    PxSERR_ERROR = FALSE, PIO_SETUP_FIS = FALSE;
    volatile AHCI_RECEIVED_FIS   *FISReceiveAddress = (AHCI_RECEIVED_FIS   *)SataDevInterface->PortFISBaseAddr;
    UINTN    TimeOutCount = TimeOut;

    for(i = 0; i < TimeOutCount * 2; i++, SmmStall(500)) { // 500usec
        //  Check for Error bits
        Data32_SERR = SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_SERR);
        if (Data32_SERR & HBA_PORTS_ERR_CHK) {
            PxSERR_ERROR = TRUE;
            break;
        }

        //  Check for Error bits
        Data32_IS = SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_IS);
        if (Data32_IS & HBA_PORTS_IS_ERR_CHK) {
            PxSERR_ERROR = TRUE;
            break;
        }
        
        switch (CommandType) {

            case PIO_DATA_IN_CMD:
            case PIO_DATA_OUT_CMD:
            case PACKET_PIO_DATA_IN_CMD:
            case PACKET_PIO_DATA_OUT_CMD:

                // check if PIO setup received
                if(FISReceiveAddress->Ahci_Psfis[0] == FIS_PIO_SETUP) {
                    FISReceiveAddress->Ahci_Psfis[0] = 0;
                    TimeOutCount = TimeOut;    
                    PIO_SETUP_FIS = TRUE;                         
                }
                break;
            default: 
                break;
            
        }
        // check if D2H register FIS is received
        if(FISReceiveAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H) break;

        // For PIO Data in D2H register FIS is not received. So rely on BSY bit
        if ((CommandType == PIO_DATA_IN_CMD) &&  PIO_SETUP_FIS &&
                    !((SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_TFD) & 
                    (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)))){
            break;
        }
    }
    if (PxSERR_ERROR) {
        // clear the status and return error
        SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
        SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);
        return EFI_DEVICE_ERROR;    
    } 
    // check if CI register is zero
    if (SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_CI)){
        return EFI_DEVICE_ERROR;                
    }
    // check for status bits
    if (SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_ERR | HBA_PORTS_TFD_DRQ)){
        return EFI_DEVICE_ERROR;                
    }
    return EFI_SUCCESS;

}

/**

    @param    AhciBusInterface 
    @param    SataDevInterface 
    @param    CIBitMask 

    @retval   EFI_STATUS

    @note    1. Clear Status register
             2. Enable FIS and CR running bit
             3. Enable Start bit
             4. Update CI bit mask

**/ 

EFI_STATUS
StartController (
    IN AMI_AHCI_BUS_SMM_PROTOCOL            *SataDevInterface,
    IN UINT32                               CIBitMask
)
{
    UINT32    AhciBaseAddr = (UINT32)(SataDevInterface->AhciBaseAddress);
    UINT8     Port = SataDevInterface->PortNumber;

    // Clear Status
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    // Enable FIS Receive
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE);

    // Wait till FIS is running
    WaitForMemSet(AhciBaseAddr,
                  Port,
                  HBA_PORTS_CMD,
                  HBA_PORTS_CMD_FR,
                  HBA_PORTS_CMD_FR,
                  HBA_FR_CLEAR_TIMEOUT);

    // Clear FIS Receive area
    ZeroMemorySmm ((VOID *)SataDevInterface->PortFISBaseAddr, RECEIVED_FIS_SIZE);

    // Enable ST
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_ST);

    // Enable Command Issued
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CI, CIBitMask);             

    return EFI_SUCCESS;
}

/**
    Check if COM Reset is successful or not

    @param    AhciBusInterface 
    @param    SataDevInterface 
    @param    Port 
    @param    PMPort 
  
    @retval    EFI_STATUS

    @note    1. Check if Link is active. If not return error.
             2. If Link is present, wait for PhyRdy Change bit to be set.
             3. Clear SError register
             4. Wait for D2H register FIS
             5. Check the Status register for errors.
             6. If COMRESET is success wait for sometime if the device is ATAPI or GEN1

**/ 
EFI_STATUS
HandlePortComReset(
    IN AMI_AHCI_BUS_SMM_PROTOCOL          *SataDevInterface,
    IN UINT8                    Port,
    IN UINT8                    PMPort
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    BOOLEAN    DeviceDetected = FALSE;
    UINT32     Data32, i, Delay, SStatusData;
    UINT32     AhciBaseAddr = (UINT32)(SataDevInterface->AhciBaseAddress);
    volatile AHCI_RECEIVED_FIS  *FISAddress;

    //  Check if detection is complete
    for (i = 0; i < HBA_PRESENCE_DETECT_TIMEOUT; i++){   // Total delay 10msec
        SStatusData = ReadSCRRegister (SataDevInterface, Port, PMPort, 0); // SStatus
        SStatusData &= HBA_PORTS_SSTS_DET_MASK;            
        if ((SStatusData == HBA_PORTS_SSTS_DET_PCE) || (SStatusData == HBA_PORTS_SSTS_DET)) { 
            DeviceDetected = TRUE;
            break; 
        }
        SmmStall (1000);                               // 1msec
    }

    if (DeviceDetected) {

        // Wait till PhyRdy Change bit is set
        if (PMPort == 0xFF) {
            Status = WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_SERR,
                            HBA_PORTS_SERR_EX,
                            HBA_PORTS_SERR_EX,
                            ATAPI_BUSY_CLEAR_TIMEOUT);
        } else {
            Status = WaitforPMMemSet (SataDevInterface, PMPort, PSCR_1_SERROR, 
                        HBA_PORTS_SERR_EX, HBA_PORTS_SERR_EX, ATAPI_BUSY_CLEAR_TIMEOUT);
        }
        // Clear Status register
        FISAddress = (AHCI_RECEIVED_FIS*)((UINT64)(SMM_HBA_PORT_REG64(AhciBaseAddr, Port, HBA_PORTS_FB)));

        for (i = 0; i < ATAPI_BUSY_CLEAR_TIMEOUT; ) {
            WriteSCRRegister (SataDevInterface, Port, PMPort, 1, HBA_PORTS_ERR_CLEAR); //SError

            if(FISAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H){
                break;
            }

            // 1msec Strange. Delay is needed for read to succeed.
            SmmStall (1000);                               // 1msec (1000);

            // For device behind PM Port, there is a delay in 
            // writing to the register. So count can be decreased.
            if (PMPort != 0xFF) {
                i+= 100;
            } else {
                i++;
            }
        } 

        for ( Delay = 0; Delay < (ATAPI_BUSY_CLEAR_TIMEOUT * 10); Delay++) {
            if(!((SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_TFD)) & HBA_PORTS_TFD_BSY)) {break;}
            if((SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_IS)) & (BIT30 + BIT29 + BIT28 + BIT27 + BIT26)) {break;}
                SmmStall (1000);                               // 1msec (100);  // 100 usec * 10 = 1Msec
        }
        // check for errors
        Data32 = SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_TFD); 
        if (Data32 & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_ERR)) Status = EFI_DEVICE_ERROR;

        Data32 = SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_IS); 
        if (Data32 & (BIT30 + BIT29 + BIT28 + BIT27 + BIT26)) Status = EFI_DEVICE_ERROR;

        SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    } else {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}

/**
    Issue a Port Reset

    @param    AhciBusInterface 
    @param    SataDevInterface 
    @param    CurrentPort 
    @param    Speed 
    @param    PowerManagement 

    @retval   EFI_STATUS

    @note    1. Issue port reset by setting DET bit in SControl register
             2. Call HandlePortComReset to check the status of the reset.

**/ 

EFI_STATUS
GeneratePortReset (
    AMI_AHCI_BUS_SMM_PROTOCOL           *SataDevInterface, 
    UINT8                    Port,
    UINT8                    PMPort,
    UINT8                    Speed,
    UINT8                    PowerManagement
)
{

    EFI_STATUS    Status;
    UINT32        AhciBaseAddr = (UINT32) SataDevInterface->AhciBaseAddress;
    volatile AHCI_RECEIVED_FIS  *FISAddress = ((AHCI_RECEIVED_FIS *)((UINT64)(SMM_HBA_PORT_REG64(AhciBaseAddr, Port, HBA_PORTS_FB))));
    UINT32        Data32;

    if (!FISAddress){
        return EFI_DEVICE_ERROR;   // FIS receive address is not programmed.
    }

    if (gPortReset){
        return EFI_SUCCESS;
    }

    gPortReset = TRUE;

    // Disable Start bit
    SMM_HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~HBA_PORTS_CMD_ST);

    // Wait till CR is cleared
    Status = WaitForMemClear(AhciBaseAddr,
                             Port,
                             HBA_PORTS_CMD,
                             HBA_PORTS_CMD_CR,
                             HBA_CR_CLEAR_TIMEOUT);

    // Clear Status register
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    if (PMPort != 0xFF) {
        Data32 = HBA_PORTS_ERR_CLEAR;
        ReadWritePMPort (SataDevInterface, PMPort, PSCR_1_SERROR, &Data32, TRUE);
    }
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 
    
    // Enable FIS Receive Enable
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE);

    // Wait till FIS is running and then clear the data area
    WaitForMemSet(AhciBaseAddr,
                  Port,
                  HBA_PORTS_CMD,
                  HBA_PORTS_CMD_FR,
                  HBA_PORTS_CMD_FR,
                  HBA_FR_CLEAR_TIMEOUT);

    FISAddress->Ahci_Rfis[0] = 0;

    if (PMPort == 0xFF) {
        // Issue Port COMRESET
        SMM_HBA_PORT_REG32_AND_OR (
              AhciBaseAddr,
              Port,
              HBA_PORTS_SCTL,
              0xFFFFF000, 
              HBA_PORTS_SCTL_DET_INIT + (Speed << 4) + (PowerManagement << 8));
        // 1msec
        SmmStall (1000);
        SMM_HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_SCTL, ~HBA_PORTS_SCTL_DET_MASK);
    }
    else {
        Data32 = HBA_PORTS_SCTL_DET_INIT + (Speed << 4) + (PowerManagement << 8);
        ReadWritePMPort (SataDevInterface, PMPort, PSCR_2_SCONTROL, &Data32, TRUE);
        SmmStall (1000);                               // 1msec
        Data32 = (Speed << 4) + (PowerManagement << 8);
        ReadWritePMPort (SataDevInterface, PMPort, PSCR_2_SCONTROL, &Data32, TRUE);
    }
    
    Status = HandlePortComReset(SataDevInterface, Port, PMPort);

    //  Disable FIS Receive Enable
    SMM_HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~HBA_PORTS_CMD_FRE);

    gPortReset = FALSE;

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }
    return EFI_SUCCESS;
}

/**
    Stop FIS and CR

    @param    AhciBusInterface 
    @param    SataDevInterface 

    @retval    EFI_STATUS

    @note    1. clear ST bit and wait till CR bits gets reset
             2. if not generate Port reset
             3. Clear FIS running bit.
             4. Clear status register

**/ 

EFI_STATUS
StopController(
    IN AMI_AHCI_BUS_SMM_PROTOCOL            *SataDevInterface,
    IN BOOLEAN                  StartOrStop
) 
{
    UINT8       Port = SataDevInterface->PortNumber;
    UINT8       PMPort = SataDevInterface->PMPortNumber;
    UINT32      AhciBaseAddr = (UINT32)(SataDevInterface->AhciBaseAddress);
    EFI_STATUS  Status;

    if(StartOrStop) {

        // Saving FIS and Command List Registers
        gCommandListBaseAddress = SMM_HBA_PORT_REG64(AhciBaseAddr,Port,HBA_PORTS_CLB);
        gFisBaseAddress         = SMM_HBA_PORT_REG64(AhciBaseAddr,Port,HBA_PORTS_FB);

        SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_CLB,SataDevInterface->PortCommandListBaseAddr);
        SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_FB,SataDevInterface->PortFISBaseAddr);
    }

    // Clear Start
    SMM_HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));

    // Make sure CR is 0 with in 500msec
    Status = WaitForMemClear(AhciBaseAddr,
                             Port,
                             HBA_PORTS_CMD,
                             HBA_PORTS_CMD_CR,
                             HBA_CR_CLEAR_TIMEOUT);

    if (EFI_ERROR(Status)) { 
        Status = GeneratePortReset(SataDevInterface,
                                   Port,
                                   PMPort,
                                   HBA_PORTS_SCTL_SPD_NSNR,
                                   HBA_PORTS_SCTL_IPM_DIS);
    }

    if (EFI_ERROR(Status)) {
        goto StopController_ErrorExit;
    }

    //  Clear FIS receive enable.
    SMM_HBA_PORT_REG32_AND (AhciBaseAddr,
                        Port, 
                        HBA_PORTS_CMD,
                        ~(HBA_PORTS_CMD_FRE));
    // Make sure FR is 0 with in 500msec
    Status = WaitForMemClear(AhciBaseAddr, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_FR,
                            HBA_FR_CLEAR_TIMEOUT);

    if (EFI_ERROR(Status)) {
        goto StopController_ErrorExit;
    }

    // Clear Status register
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    if(!StartOrStop) {

        // Restoring FIS and Command List Registers
        SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_CLB,gCommandListBaseAddress);
        SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_FB,gFisBaseAddress);
    }

    return  EFI_SUCCESS;

StopController_ErrorExit:

    // Restoring FIS and Command List Registers
    SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_CLB,gCommandListBaseAddress);
    SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_FB,gFisBaseAddress);

    return Status;
}

/**
    Check if the device is ready to accept command.

    @param    SataDevInterface 

    @retval    EFI_STATUS      

    @note    1. Check the device is ready to accept the command. BSY and DRQ 
     should be de-asserted.
             2. If set, generate Port reset
             3. In case Port Multiplier is connected to the port, enable all 
     the ports of the Port Multiplier.

**/ 
EFI_STATUS
ReadytoAcceptCmd (
    IN AMI_AHCI_BUS_SMM_PROTOCOL            *SataDevInterface
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    UINT32        AhciBaseAddr = (UINT32)(SataDevInterface->AhciBaseAddress);
    UINT8         Port = SataDevInterface->PortNumber;
    UINT8         PMPort = SataDevInterface->PMPortNumber;
    UINT32        Data32 = 0;

    // Is the Device ready to accept the command
    if (SMM_HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)){
        Data32 = SMM_HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_SCTL);
        Data32 &= 0xFF0;

        // make sure the status we read is for the right port
        Status = GeneratePortReset(SataDevInterface,
                                   Port,
                                   PMPort,
                                   (UINT8)((Data32 & 0xF0) >> 4),
                                   (UINT8)(Data32 >> 8));
        if (EFI_ERROR(Status)){
            return Status;
        }
    } 
    return Status;
}

/**
    Builds command list

    @param    SataDevInterface 
    @param    CommandList 
    @param    CommandTableBaseAddr 

    @retval   EFI_STATUS

    @note    1. Update CommandList bits
             2. Not all fields like Ahci_Cmd_A are updated.
             3. Port number is set to 0xF (Control port) if PM Port number is 0xFF. 

**/ 

EFI_STATUS
BuildCommandList (
    IN AMI_AHCI_BUS_SMM_PROTOCOL        *SataDevInterface, 
    IN AHCI_COMMAND_LIST                *CommandList,
    IN UINT64                           CommandTableBaseAddr
)
{
    UINT8    PMPort = SataDevInterface->PMPortNumber;

    ZeroMemorySmm (CommandList, sizeof(AHCI_COMMAND_LIST));
    CommandList->Ahci_Cmd_A = SataDevInterface->DeviceType == ATAPI ? 1 : 0;      // set elsewhere 
    CommandList->Ahci_Cmd_P = 0;
    CommandList->Ahci_Cmd_R = 0;
    CommandList->Ahci_Cmd_B = 0;
    CommandList->Ahci_Cmd_Rsvd1 = 0;
    CommandList->Ahci_Cmd_PMP = PMPort == 0xFF ? 0x0 : SataDevInterface->PMPortNumber;
    CommandList->Ahci_Cmd_PRDTL = 0;
    CommandList->Ahci_Cmd_PRDBC = 0;
    CommandList->Ahci_Cmd_CTBA = (UINT32)CommandTableBaseAddr;
    CommandList->Ahci_Cmd_CTBAU = (UINT32)Shr64(CommandTableBaseAddr,32);   // Store Upper 32 bit value of CommandTableAddr

    return EFI_SUCCESS;
}

/**
    Copy Length bytes from Source to Destination.

    @param    Destination 
    @param    Source 
    @param    Length 

    @retval    VOID

**/ 

VOID
CopyMemSmm (
    IN VOID    *Destination,
    IN VOID    *Source,
    IN UINTN   Length
)
{
    CHAR8 *Destination8;
    CHAR8 *Source8;

    if (Source < Destination) {
        Destination8  = (CHAR8 *) Destination + Length - 1;
        Source8       = (CHAR8 *) Source + Length - 1;
        while (Length--) {
            *(Destination8--) = *(Source8--);
        }
    } else {
        Destination8  = (CHAR8 *) Destination;
        Source8       = (CHAR8 *) Source;
        while (Length--) {
            *(Destination8++) = *(Source8++);
        }
    }
}

/**

    @param    SataDevInterface 
    @param    CommandStructure 
    @param    CommandList 
    @param    Commandtable 

    @retval   EFI_STATUS

    @note    1. Copy Packet data to command table
             2. Set Atapi bit in Command List    

**/ 
EFI_STATUS
BuildAtapiCMD(
    IN AMI_AHCI_BUS_SMM_PROTOCOL            *SataDevInterface, 
    IN COMMAND_STRUCTURE        CommandStructure,
    IN AHCI_COMMAND_LIST        *CommandList,
    IN AHCI_COMMAND_TABLE       *Commandtable
)
{
    CopyMemSmm(&(Commandtable->AtapiCmd),&(CommandStructure.AtapiCmd),sizeof(AHCI_ATAPI_COMMAND));

    if (Commandtable->CFis.Ahci_CFis_Cmd == PACKET_COMMAND){ // Is it a packet command?         
        CommandList->Ahci_Cmd_A = 1;
    }

    return EFI_SUCCESS;
}

/**
    Build PRDT table

    @param    SataDevInterface 
    @param    CommandStructure 
    @param    CommandList 
    @param    Commandtable 

    @retval   EFI_STATUS

    @note    1. Build as many PRDT table entries based on ByteCount.
             2. Set the I flag for the last PRDT table.
             3. Update PRDT table length in CommandList

**/
EFI_STATUS
BuildPRDT (
    IN AMI_AHCI_BUS_SMM_PROTOCOL  *SataDevInterface, 
    IN COMMAND_STRUCTURE          CommandStructure,
    IN AHCI_COMMAND_LIST          *CommandList,
    IN AHCI_COMMAND_TABLE         *Commandtable
)
{
    UINT32                  ByteCount = CommandStructure.ByteCount;
    UINT16                  Prdtlength = 0;
    AHCI_COMMAND_PRDT       *PrdtTable = &(Commandtable->PrdtTable);

    //for (;ByteCount; (UINT8 *)PrdtTable += sizeof(AHCI_COMMAND_PRDT)){
    for (;ByteCount; PrdtTable ++){
        PrdtTable->Ahci_Prdt_DBA = (UINT32)CommandStructure.Buffer;
        PrdtTable->Ahci_Prdt_DBC = ByteCount >= PRD_MAX_DATA_COUNT ? (PRD_MAX_DATA_COUNT - 1) : (ByteCount - 1);
        ByteCount -= (PrdtTable->Ahci_Prdt_DBC + 1);
        PrdtTable->Ahci_Prdt_I = 0;
        Prdtlength+= sizeof(AHCI_COMMAND_PRDT);
        CommandStructure.Buffer = ((UINT8 *)CommandStructure.Buffer + PrdtTable->Ahci_Prdt_DBC + 1);
    }

    //  Set I flag only for the last entry.
    //(UINT8 *)PrdtTable -= sizeof(AHCI_COMMAND_PRDT);
    PrdtTable --;
    PrdtTable->Ahci_Prdt_I = 1;
    CommandList->Ahci_Cmd_PRDTL = Prdtlength / sizeof(AHCI_COMMAND_PRDT);

    return EFI_SUCCESS;
}

/**
    Build Command FIS

    @param    SataDevInterface 
    @param    CommandStructure 
    @param    CommandList 
    @param    Commandtable 

    @retval   EFI_STATUS

    @note    1. Update Command FIS data area.
             2. Update the Command FIS length in Command List table  

**/ 
EFI_STATUS
BuildCommandFIS (
    IN AMI_AHCI_BUS_SMM_PROTOCOL       *SataDevInterface, 
    IN COMMAND_STRUCTURE               CommandStructure,
    IN AHCI_COMMAND_LIST               *CommandList,
    IN AHCI_COMMAND_TABLE              *Commandtable
)
{
    ZeroMemorySmm (Commandtable, sizeof(AHCI_COMMAND_TABLE));

    Commandtable->CFis.Ahci_CFis_Type = FIS_REGISTER_H2D;
    Commandtable->CFis.AHci_CFis_PmPort = SataDevInterface->PMPortNumber == 0xFF ? 0x0 : SataDevInterface->PMPortNumber;

    Commandtable->CFis.Ahci_CFis_Cmd = CommandStructure.Command;

    Commandtable->CFis.Ahci_CFis_Features = CommandStructure.Features;
    Commandtable->CFis.Ahci_CFis_FeaturesExp = CommandStructure.FeaturesExp;

    Commandtable->CFis.Ahci_CFis_SecNum = CommandStructure.LBALow;
    Commandtable->CFis.Ahci_CFis_SecNumExp = CommandStructure.LBALowExp;

    Commandtable->CFis.Ahci_CFis_ClyLow = CommandStructure.LBAMid;
    Commandtable->CFis.Ahci_CFis_ClyLowExp = CommandStructure.LBAMidExp;

    Commandtable->CFis.Ahci_CFis_ClyHigh = CommandStructure.LBAHigh;
    Commandtable->CFis.Ahci_CFis_ClyHighExp = CommandStructure.LBAHighExp;

    Commandtable->CFis.Ahci_CFis_SecCount = (UINT8)(CommandStructure.SectorCount);
    Commandtable->CFis.Ahci_CFis_SecCountExp = (UINT8)(CommandStructure.SectorCount >> 8);

    Commandtable->CFis.Ahci_CFis_DevHead = CommandStructure.Device;
    Commandtable->CFis.Ahci_CFis_Control = CommandStructure.Control;    

    CommandList->Ahci_Cmd_CFL = FIS_REGISTER_H2D_LENGTH / 4;

    return EFI_SUCCESS;

}

/**
    Read/Write routine to PM ports

    @param    SataDevInterface 
    @param    Port 
    @param    RegNum 
    @param    Data 
    @param    READWRITE       // TRUE for Write

    @retval   EFI_STATUS

    @note  
          1. Update Command Structure for READ/Write Port Multiplier command
          2. Issue command
          3. Check for errors.
          4. Read the out data in case of READ.

**/
EFI_STATUS
ReadWritePMPort (
    IN AMI_AHCI_BUS_SMM_PROTOCOL    *SataDevInterface,
    IN UINT8                        Port,
    IN UINT8                        RegNum,
    IN OUT UINT32                   *Data,
    IN BOOLEAN                      READWRITE
)
{
    EFI_STATUS            Status;
    COMMAND_STRUCTURE     CommandStructure;
    AHCI_RECEIVED_FIS     *PortFISBaseAddr = (AHCI_RECEIVED_FIS *)(SataDevInterface->PortFISBaseAddr);
    AHCI_COMMAND_LIST     *CommandList = (AHCI_COMMAND_LIST *) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE    *Commandtable = (AHCI_COMMAND_TABLE *)SataDevInterface->PortCommandTableBaseAddr;

    ZeroMemorySmm (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    CommandStructure.Command = READ_PORT_MULTIPLIER;

    if (READWRITE) {
        CommandStructure.SectorCount = (UINT16) (*Data & 0xFF);
        CommandStructure.LBALow = (UINT8) (*Data >> 8);
        CommandStructure.LBAMid = (UINT8)(*Data >> 16);
        CommandStructure.LBAHigh = (UINT8)(*Data >> 24);
        CommandStructure.Command = WRITE_PORT_MULTIPLIER;
    }

    CommandStructure.Device = Port;
    CommandStructure.Features = RegNum;

    Status = StopController(SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(SataDevInterface,FALSE);
        return Status;
    }

    BuildCommandList(SataDevInterface, CommandList, SataDevInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, CommandStructure, CommandList, Commandtable);

    // Data-in
    CommandList->Ahci_Cmd_W = 0; 

    // Update of Command Register
    Commandtable->CFis.Ahci_CFis_C = 1;

    // Update the Port Address
    CommandList->Ahci_Cmd_PMP = CONTROL_PORT;
    Commandtable->CFis.AHci_CFis_PmPort = CONTROL_PORT; 

    StartController(SataDevInterface, BIT00);

    Status = WaitforCommandComplete(SataDevInterface, NON_DATA_CMD, TIMEOUT_1SEC);

    //  Stop Controller
    StopController(SataDevInterface,FALSE);

    if (!READWRITE) {
        *Data = 0;
        if (!EFI_ERROR(Status)) {
            *Data = PortFISBaseAddr->Ahci_Rfis[12] |
                    (PortFISBaseAddr->Ahci_Rfis[4] << 8) |
                    (PortFISBaseAddr->Ahci_Rfis[5] << 16) |
                    (PortFISBaseAddr->Ahci_Rfis[6] << 24);
        }
    }
    return Status;
}

/**
    Execute the DMA data command

    @param    SataDevInterface 
    @param    CommandStructure 
    @param    UINT8                                   PortNumber,
    @param    UINT8                                   PMPortNumber, 
    @param    DEVICE_TYPE                             DeviceType,
    @param    BOOLEAN                                 READWRITE 

    @retval    EFI_STATUS

**/ 
EFI_STATUS
EFIAPI 
AhciSmmExecuteDmaDataCommand (
        AMI_AHCI_BUS_SMM_PROTOCOL           *SataDevInterface, 
        COMMAND_STRUCTURE                   *CommandStructure,
        UINT8                               PortNumber,
        UINT8                               PMPortNumber, 
        DEVICE_TYPE                         DeviceType,
        BOOLEAN                             READWRITE
)
{

    EFI_STATUS              Status;
    AHCI_COMMAND_LIST       *CommandList = (AHCI_COMMAND_LIST *) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE      *Commandtable = (AHCI_COMMAND_TABLE *)SataDevInterface->PortCommandTableBaseAddr;

    SataDevInterface->PortNumber=PortNumber;
    SataDevInterface->PMPortNumber=PMPortNumber;
    SataDevInterface->DeviceType=DeviceType;

    Status = StopController(SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(SataDevInterface,FALSE);
        return Status;
    }
    BuildCommandList(SataDevInterface, CommandList, SataDevInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(SataDevInterface, *CommandStructure, CommandList, Commandtable);

    // Data-in
    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;
    }
    else {
        CommandList->Ahci_Cmd_W = 0;
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController(SataDevInterface, BIT00);

    Status = WaitforCommandComplete(SataDevInterface, DMA_DATA_IN_CMD, 
                    SataDevInterface->DeviceType == ATA? DMA_ATA_COMMAND_COMPLETE_TIMEOUT \
                    : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT );
    
    if (!EFI_ERROR(Status)){
        //Check if the required BYTES have been received
        if (CommandList->Ahci_Cmd_PRDBC != CommandStructure->ByteCount){
            Status = EFI_DEVICE_ERROR;
        }
    }
    //  Stop Controller
    StopController(SataDevInterface,FALSE);

    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;

    return Status;    

}

/**
    Execute the PIO data command

    @param    SataDevInterface 
    @param    CommandStructure 
    @param    UINT8                                   PortNumber,
    @param    UINT8                                   PMPortNumber, 
    @param    DEVICE_TYPE                             DeviceType,
    @param    BOOLEAN                                 READWRITE 

    @retval    EFI_STATUS

**/ 
EFI_STATUS
EFIAPI 
AhciSmmExecutePioDataCommand (
    AMI_AHCI_BUS_SMM_PROTOCOL           *SataDevInterface, 
    COMMAND_STRUCTURE                   *CommandStructure,
    UINT8                               PortNumber,
    UINT8                               PMPortNumber, 
    DEVICE_TYPE                         DeviceType,
    BOOLEAN                             READWRITE 
    )
{
    EFI_STATUS            Status;
    AHCI_COMMAND_LIST     *CommandList = (AHCI_COMMAND_LIST *) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE    *Commandtable = (AHCI_COMMAND_TABLE *)SataDevInterface->PortCommandTableBaseAddr;

    SataDevInterface->PortNumber=PortNumber;
    SataDevInterface->PMPortNumber=PMPortNumber;
    SataDevInterface->DeviceType=DeviceType;

    Status = StopController(SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(SataDevInterface,FALSE);
        return Status;
    }

    BuildCommandList(SataDevInterface, CommandList, SataDevInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(SataDevInterface, *CommandStructure, CommandList, Commandtable);

    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;
    } else {
        CommandList->Ahci_Cmd_W = 0;
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController(SataDevInterface, BIT00);

    Status = WaitforCommandComplete(SataDevInterface, PIO_DATA_IN_CMD, 
                    SataDevInterface->DeviceType == ATA? DMA_ATA_COMMAND_COMPLETE_TIMEOUT \
                    : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT );

    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;

    //
    //  Stop Controller
    //
    StopController(SataDevInterface,FALSE);

    return Status; 
}

/**
    Execute the Non Data command 

                   
    @param    SataDevInterface 
    @param    CommandStructure 
    @param    UINT8                                   PortNumber,
    @param    UINT8                                   PMPortNumber, 
    @param    DEVICE_TYPE                             DeviceType,

    @retval   EFI_STATUS

**/ 

EFI_STATUS
EFIAPI 
AhciSmmExecuteNonDataCommand (
    AMI_AHCI_BUS_SMM_PROTOCOL           *SataDevInterface, 
    COMMAND_STRUCTURE                   CommandStructure,
    UINT8                               PortNumber,
    UINT8                               PMPortNumber, 
    DEVICE_TYPE                         DeviceType
)
{

    EFI_STATUS            Status;
    AHCI_COMMAND_LIST     *CommandList = (AHCI_COMMAND_LIST *) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE    *Commandtable = (AHCI_COMMAND_TABLE *)SataDevInterface->PortCommandTableBaseAddr;

    SataDevInterface->PortNumber=PortNumber;
    SataDevInterface->PMPortNumber=PMPortNumber;
    SataDevInterface->DeviceType=DeviceType;

    Status = StopController(SataDevInterface,TRUE);
    if (EFI_ERROR(Status)){
        return Status;
    }

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(SataDevInterface,FALSE);
        return Status;
    }

    BuildCommandList(SataDevInterface, CommandList, SataDevInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, CommandStructure, CommandList, Commandtable);

    // Data-in
    CommandList->Ahci_Cmd_W = 0; 

    // Update of Command Register
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController(SataDevInterface, BIT00);

    Status = WaitforCommandComplete(SataDevInterface, NON_DATA_CMD, ATAPI_BUSY_CLEAR_TIMEOUT );

    // Stop Controller
    StopController(SataDevInterface,FALSE);

    return Status;    
}

/**
    Check for ATAPI Errors

    @param    SataDevInterface 

    @retval    EFI_STATUS  

    @note  
      1. Execute ATAPI Request Sense command.
      2. Check for Device getting ready, Media Change, No Media and other errors. Update AtapiDevice->Atapi_Status

**/

EFI_STATUS
HandleAtapiError (
    IN AMI_AHCI_BUS_SMM_PROTOCOL               *SataDevInterface
 )
{

    EFI_STATUS           Status;
    UINT8                Data8 = 0;
    COMMAND_STRUCTURE    CommandStructure;
    UINT32               AhciBaseAddr = (UINT32)(SataDevInterface->AhciBaseAddress);
    UINT8                Port = SataDevInterface->PortNumber;
    ATAPI_DEVICE         *AtapiDevice = &SataDevInterface->AtapiDevice;

    AtapiDevice->Atapi_Status = DEVICE_ERROR;

    ZeroMemorySmm (SenseData, 256);
    ZeroMemorySmm (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_REQUEST_SENSE;
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[4] = 0xff;

    CommandStructure.ByteCount =  256;
    CommandStructure.Buffer = SenseData;

    Status = AhciSmmExecutePacketCommand(SataDevInterface, &CommandStructure, 0,
                                        Port,SataDevInterface->PMPortNumber,ATAPI);

    if (EFI_ERROR(Status)) {
        Data8 = SMM_HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD);
    }

    // Check for DF and CHK
    if (Data8 & (IDE_DF | IDE_CHK)) { 
        goto exit_HandleAtapiError_with_Reset;
    }

    if (!EFI_ERROR(Status)){

        AtapiDevice->Atapi_Status = DEVICE_ERROR;
        Status = EFI_DEVICE_ERROR;              // Default Value    

        if (((SenseData[2] & 0xf) == 2) && (SenseData[12] == 0x3a)) {
            Status = EFI_NO_MEDIA;
            AtapiDevice->Atapi_Status = MEDIUM_NOT_PRESENT;
        }
        if (((SenseData[2] & 0xf) == 2) && (SenseData[12] == 0x04) && (SenseData[13] == 0x01)) {
            Status = EFI_MEDIA_CHANGED;
            AtapiDevice->Atapi_Status = BECOMING_READY;
        }
    
        if (((SenseData[2] & 0xf) == 6) && (SenseData[12] == 0x28)){
                Status = EFI_MEDIA_CHANGED;
                AtapiDevice->Atapi_Status = MEDIA_CHANGED;
        }

        if (((SenseData[2] & 0xf) == 7) && (SenseData[12] == 0x27)){
                Status = EFI_WRITE_PROTECTED;
                AtapiDevice->Atapi_Status = WRITE_PROTECTED_MEDIA;
        }

        if (((SenseData[2] & 0xf) == 6) && (SenseData[12] == 0x29)){
                AtapiDevice->Atapi_Status = POWER_ON_OR_DEVICE_RESET;
        }

        if (((SenseData[2] & 0xf) == 5) && (SenseData[0] == 0x70)){
                AtapiDevice->Atapi_Status = ILLEGAL_REQUEST;
        }
    }    

exit_HandleAtapiError_with_Reset:
        return Status;
}

/**
    Execute a Atapi Packet command

    @param    SataDevInterface 
    @param    CommandStructure 
    @param    IN BOOLEAN                              READWRITE
    @param    UINT8                                   PortNumber,
    @param    UINT8                                   PMPortNumber, 
    @param    DEVICE_TYPE                             DeviceType

    @retval   EFI_STATUS

    @note  
          1. Stop the Controller
          2. Check if the device is ready to accept a Command. 
          3. Build Command list
          4. Start the Controller.
          5. Wait till command completes. Check for errors.

**/ 
EFI_STATUS 
EFIAPI 
AhciSmmExecutePacketCommand (
    AMI_AHCI_BUS_SMM_PROTOCOL               *SataDevInterface, 
    IN COMMAND_STRUCTURE     *CommandStructure,
    IN BOOLEAN               READWRITE,
    UINT8                    PortNumber,
    UINT8                    PMPortNumber, 
    DEVICE_TYPE              DeviceType
)
{

    EFI_STATUS            Status;
    AHCI_COMMAND_LIST     *CommandList = (AHCI_COMMAND_LIST *) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE    *Commandtable = (AHCI_COMMAND_TABLE *)SataDevInterface->PortCommandTableBaseAddr;
    UINT32                AhciBaseAddr = (UINT32)(SataDevInterface->AhciBaseAddress);
    UINT8                 Port = SataDevInterface->PortNumber;
    UINT8                 Data8;
    ATAPI_DEVICE          *AtapiDevice = &SataDevInterface->AtapiDevice;


    SataDevInterface->PortNumber=PortNumber;
    SataDevInterface->PMPortNumber=PMPortNumber;
    SataDevInterface->DeviceType=DeviceType;

    CommandStructure->LBAMid = (UINT8)(CommandStructure->ByteCount);
    CommandStructure->LBAHigh = (UINT8)(CommandStructure->ByteCount >> 8);
    CommandStructure->Command = PACKET_COMMAND;

    Status = StopController( SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController( SataDevInterface,FALSE);
        return Status;
    }

    BuildCommandList(SataDevInterface, CommandList, SataDevInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(SataDevInterface, *CommandStructure, CommandList, Commandtable);


    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;        
    }
    else {
        CommandList->Ahci_Cmd_W = 0;        
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController( SataDevInterface, BIT00);

    Status = WaitforCommandComplete(SataDevInterface, PIO_DATA_IN_CMD, 
                    SataDevInterface->DeviceType == ATA? DMA_ATA_COMMAND_COMPLETE_TIMEOUT \
                    : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT );


    // Handle ATAPI device error
    if (EFI_ERROR(Status) && SataDevInterface->DeviceType == ATAPI) {
        Data8 = SMM_HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD);
        if (Data8 & IDE_CHK ){
            return HandleAtapiError(SataDevInterface);
        }        
    }

    AtapiDevice->Atapi_Status = EFI_SUCCESS;

    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;

    //  Stop Controller
    StopController(SataDevInterface,FALSE);

    return Status;
}

/**
    Initialize the Sata port on S3 resume

                   
    @param    SataDevInterface 
    @param    UINT8                                   Port,

    @retval    EFI_STATUS

**/ 

EFI_STATUS
EFIAPI 
AhciSmmInitPortOnS3Resume(
    AMI_AHCI_BUS_SMM_PROTOCOL           *SataDevInterface, 
    UINT8                               Port
)
{
    UINT32      AhciBaseAddr=(UINT32)SataDevInterface->AhciBaseAddress;

    // If the AhciBaseAddress is 0, Initialize the base address
    if(!AhciBaseAddr) {
#if INDEX_DATA_PORT_ACCESS
        UINT32 lbar;

        lbar = *(UINT32*)PCI_CFG_ADDR(ICH_SATA_BUS_NUMBER,
                                            ICH_SATA_DEVICE_NUMBER,
                                            ICH_SATA_FUNCTION_NUMBER,
                                            PCI_LBAR);  // Get AHCI lbase address;

        lbar &= PCI_LBAR_ADDRESS_MASK;  // Legacy Bus Master Base Address

        IndexPort = (UINT16)lbar + INDEX_OFFSET_FROM_LBAR;
        DataPort = (UINT16)lbar + DATA_OFFSET_FROM_LBAR;
#endif
        //Initialize the AHCI base address
        AhciBaseAddr = *(UINT32*)PCI_CFG_ADDR(ICH_SATA_BUS_NUMBER,
                                            ICH_SATA_DEVICE_NUMBER,
                                            ICH_SATA_FUNCTION_NUMBER,
                                            PCI_ABAR);  // Get AHCI base address;
        //Initialize the Ahci Base address 
        SataDevInterface->AhciBaseAddress=AhciBaseAddr;
    }

    //Set the Spin up device on the port
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_SUD);

    WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_SSTS ,
                                    HBA_PORTS_SSTS_DET_MASK ,
                                    HBA_PORTS_SSTS_DET_PCE ,
                                    HBA_PRESENCE_DETECT_TIMEOUT);
    //Clear Error Register
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port,
                                HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); // Clear Status register

    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    //Set the FIS base address
    gFisBaseAddress = SMM_HBA_PORT_REG64(AhciBaseAddr,Port,HBA_PORTS_FB);
    SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_FB,SataDevInterface->PortFISBaseAddr);

    //  Enable FIS Receive Enable
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE); 

    // Wait till FIS is running
    WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);

    // Enable ST
    SMM_HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_ST);     

    // Wait till CR list is running
    WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_CR,
                                    HBA_PORTS_CMD_CR,
                                    HBA_FR_CLEAR_TIMEOUT);

    //Restore the FIS base address
    SMM_HBA_PORT_WRITE_REG64(AhciBaseAddr,Port,HBA_PORTS_FB,gFisBaseAddress);

    return EFI_SUCCESS;
}

/**
    This function is called from SMM during SMM registration.
        
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/

EFI_STATUS 
InSmmFunction (
    EFI_HANDLE        ImageHandle, 
    EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS               Status;
    UINT64                   FisBaseAddress=0;
    UINT64                   PortCommandListBaseAddr=0;
    EFI_SMM_SYSTEM_TABLE2    *mSmst;

    //Initialize the Ahci Base address to 0
    AhciSmm.AhciBaseAddress=0;

    //  Allocate memory for FIS. Should be aligned on 256 Bytes. Each Port will have it own FIS data area.
    Status = pBS->AllocatePool ( EfiReservedMemoryType,
                                 1 * RECEIVED_FIS_SIZE + 0x100,
                                 (VOID**)&FisBaseAddress );
    ASSERT_EFI_ERROR(Status);
    ZeroMemorySmm ((VOID *)FisBaseAddress,1 * RECEIVED_FIS_SIZE + 0x100);
    AhciSmm.PortFISBaseAddr =((FisBaseAddress & (~0xFF))+ 0x100);

    //  Allocate memory for Command List (1KB aligned) and Command Table (128KB aligned).
    //  All the ports in the controller will share Command List and Command table data Area.
    Status = pBS->AllocatePool ( EfiReservedMemoryType,
                                 COMMAND_LIST_SIZE_PORT * 2,
                                 (VOID**)&PortCommandListBaseAddr );
    ASSERT_EFI_ERROR(Status);

    ZeroMemorySmm ((VOID *)PortCommandListBaseAddr, COMMAND_LIST_SIZE_PORT * 2);

    //  Allocate memory for Sense Data 
    Status = pBS->AllocatePool (EfiReservedMemoryType,
                                256,
                                (VOID**)&SenseData);
    ASSERT_EFI_ERROR(Status);

    AhciSmm.PortCommandListBaseAddr = (PortCommandListBaseAddr & (~0x3FF)) + 0x400;
    AhciSmm.PortCommandTableBaseAddr = AhciSmm.PortCommandListBaseAddr + 0x80;

    AhciSmm.AhciSmmInitPortOnS3Resume=AhciSmmInitPortOnS3Resume;
    AhciSmm.AhciSmmExecutePioDataCommand=AhciSmmExecutePioDataCommand;
    AhciSmm.AhciSmmExecuteDmaDataCommand=AhciSmmExecuteDmaDataCommand;
    AhciSmm.AhciSmmExecuteNonDataCommand=AhciSmmExecuteNonDataCommand;
    AhciSmm.AhciSmmExecutePacketCommand=AhciSmmExecutePacketCommand;

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL,  (VOID **)&pSmmBase);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    pSmmBase->GetSmstLocation (pSmmBase, &mSmst);

    Status = mSmst->SmmInstallConfigurationTable(
                                        mSmst,
                                        &gAmiAhciSmmProtocolGuid,
                                        (VOID *)&AhciSmm,
                                        sizeof(AMI_AHCI_BUS_SMM_PROTOCOL)
                                        );
    ASSERT_EFI_ERROR(Status);

    return Status;
}

/**
    AHCI Smm driver Entry Point

    @param    EFI_HANDLE           ImageHandle,
    @param    EFI_SYSTEM_TABLE     *SystemTable

    @retval    EFI_STATUS

**/

EFI_STATUS 
EFIAPI AhciSmmDriverEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);
    // Get the PCI Express Base Address from the Pcd
    PciExpressBaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);   
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
