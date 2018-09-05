//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//   Rev. 1.19
//     Bug Fix: Support bridge solution AOC card and two CPLD on one BPN.
//     Reason:     
//     Auditor: Kasber Chen
//     Date:    Aug/21/2017
//
//   Rev. 1.18
//     Bug Fix: Add PLX detection for VMD mode. It's used to help create SMBIOS type 40.
//     Reason:     
//     Auditor: Jason Hsu
//     Date:    Aug/10/2017
//
//   Rev. 1.17
//     Bug Fix: Fix 24NVMe VMD setting malfunction if set separately
//     Reason:  24NVMe has 2 PLX switches which are controlled separately   
//     Auditor: Donald Han
//     Date:    Aug/08/2017
//
//   Rev. 1.16
//     Bug Fix: Add extend flag for slot information.
//     Reason:     
//     Auditor: Kasber Chen
//     Date:    Jul/25/2017
//
//   Rev. 1.15
//     Bug Fix: Update NVMe Bit map for PLX bridge.
//     Reason:  Request from PM John Hui, Follow the SPEC. Code may change if SPEC change in the future.
//     Auditor: Jacker Yeh
//     Date:    Jun/21/2017
//
//   Rev. 1.14
//     Bug Fix: Confirm smbus statue by IPMI before MUX channel change
//     Reason:  Fix code change side effect.
//     Auditor: Kasber Chen
//     Date:    Jun/13/2017
//
//  Rev. 1.13
//    Bug Fix:  Check PcdSmcRiserID extended bit to differentiate AOC-2UR6N4-IN001 from AOC-2U6N4-i4XT
//    Reason:   AOC-2U6N4-i4XT/AOC-2UR6N4-IN001 use same board ID, we have to use sideband sensor info to distinguish
//    Auditor:  Donald Han
//    Date:     Jun/12/2017
//
//   Rev. 1.12
//     Bug Fix: 1. Change m/b MUX acess from SMBus to BMC and add feedback check
//				2. Add 24NVMe VMD support
//     Reason:  1. BMC -> m/b MUX looks more robust than PCH -> m/b MUX
//				2. 24NVMe has 2nd MUX on riser card; 24NVMe has 2 CPLDs on BP, one is 66 and one is 6E
//     Auditor: Donald Han
//     Date:    Jun/08/2017
//
//   Rev. 1.11
//     Bug Fix: Remove unused redriver codes.
//     Reason:  
//     Auditor: Kasber Chen
//     Date:    May/22/2017
//
//   Rev. 1.10
//     Bug Fix: Change "<" to "<=" to fix AOC redriver value is incomplete
//     Reason:  Normally loop index starts from 0, the index starts from 1 in this loop
//     Auditor: Donald Han
//     Date:    May/04/2017
//
//   Rev. 1.09
//     Bug Fix: Support AOC NVMe card redriver function.
//     Reason:     
//     Auditor: Kasber Chen
//     Date:    Apr/20/2017
//
//   Rev. 1.08
//     Bug Fix: Fix OPROM control no function when x8 AOC card in x16 lane reverse slot.
//     Reason:  Can't get bus number
//     Auditor: Kasber Chen
//     Date:    Mar/08/2017
//
//   Rev.  1.07
//     Bug Fix: Fix incorrect pcie bifurcate setting when lane reverse.
//     Reason:	
//     Auditor: Kasber Chen
//     Date:    Mar/02/2017
//
//   Rev.  1.06
//     Bug Fix: Support PCH smbus connect to BPN directly case.
//     Reason:	
//     Auditor: Kasber Chen
//     Date:    Feb/03/2017
//
//   Rev.  1.05
//     Bug Fix: Add AOC-SLG3 port number and support BPN CPLD bitmap filled by PCH.
//     Reason:	
//     Auditor: Kasber Chen
//     Date:    Jan/16/2017
//
//   Rev.  1.04
//     Bug Fix: Correct NVMe VMD and legacy mode changed offset.
//     Reason:  From branch SVN_732
//     Auditor: Kasber Chen
//     Date:    12/29/2016
//
//   Rev.  1.03
//     Bug Fix: Support NVMe change VMD and legacy mode per port.
//     Reason:	
//     Auditor: Kasber Chen
//     Date:    12/19/2016
//
//   Rev.  1.02
//     Bug Fix:
//     Reason:  Fix MUX channel auto change.
//     Auditor: Kasber Chen
//     Date:    11/24/2016
//
//   Rev.  1.01
//     Bug Fix:
//     Reason:  Support multi NVMe cards.
//     Auditor: Kasber Chen
//     Date:    11/16/2016
//
//   Rev.  1.00
//     Bug Fix:
//     Reason:  Initialize revision
//     Auditor: Kasber Chen
//     Date:    10/03/2016
//
//***************************************************************************
//***************************************************************************
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Ppi/Smbus2.h>
#include <Ppi/IPMITransportPpi.h>
#include <Ppi/Stall.h>
#include <Ppi/MemoryDiscovered.h>
#include <IndustryStandard/Pci22.h>
#include <Token.h>
#include "SmcLib.h"
#include "SmcAOC.h"
#include "SmcAOCPei.h"
#include <Base.h>
#include <MeUma.h>

EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;

#define AOC_ReD_Data_Max 30

static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyDesc[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gMemoryInitDoneSentGuid,
    SmcAocPeiPlxCallback
  }
};

EFI_STATUS
SmcAOCRedriver(
    IN  EFI_PEI_SMBUS2_PPI          *Smbus,
    IN  UINT8   *AOC_Name,
    IN  UINT8   AOC_Index
)
{
    EFI_STATUS  Status;
    EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress;
    AOC_ReD     AOC_ReD_Data[AOC_ReD_Data_Max];
    UINT8       i, j;
    UINTN       Length = 1;

    DEBUG((-1, "SmcAOCRedriver Entry.\n"));

    for(i = 0; i < AOC_Max_Port; i++){      //i:port index
        SetMem((UINT8*)AOC_ReD_Data, sizeof(AOC_ReD) * AOC_ReD_Data_Max, 0);
        GetRedriverData(AOC_Index, i, AOC_ReD_Data);
        DEBUG((-1, "AOC add %x, data size %d.\n", AOC_ReD_Data[0].Reg, AOC_ReD_Data[0].Data));
        if(!AOC_ReD_Data[0].Reg || (AOC_ReD_Data[0].Reg == 0xff))
            continue;

        SlaveAddress.SmbusDeviceAddress = AOC_ReD_Data[0].Reg >> 1;  //Smbus address is better to set before the loop
        for(j = 1; j <= AOC_ReD_Data[0].Data; j++){
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            AOC_ReD_Data[j].Reg,
                            EfiSmbusWriteByte,
                            0,
                            &Length,
                            &AOC_ReD_Data[j].Data);
            DEBUG((-1, "Set AOC add %x, Reg %x, Data %x, %r.\n", AOC_ReD_Data[0].Reg,
                    AOC_ReD_Data[j].Reg, AOC_ReD_Data[j].Data, Status));
        }
    }
    DEBUG((-1, "SmcAOCRedriver End.\n"));
    return Status;
}

EFI_STATUS  EFIAPI
SmcAOCCardDetection(
    IN  EFI_PEI_SERVICES  **PeiServices,
    IN  UINT8   Channel,        //IPMI smbus channel for system
    IN  UINTN   MUX_Address,
    IN  UINT8   MUX_Channel,  
    IN  SMC_SLOT_INFO   SLOT_Tbl[],
    IN  UINT8   SLOT_Tbl_Size
)
{
    EFI_STATUS  Status;
    PEI_IPMI_TRANSPORT_PPI      *IpmiTransportPpi;
    EFI_PEI_SMBUS2_PPI          *Smbus;
    EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress;
    EFI_PEI_STALL_PPI           *gStallPpi;
    UINT8       TempBuffer[100], FRU_Add, FRU_Index, AOC_Index;
    UINT8       i, j, k, found, PortNum, Mux_Switch[] = {0, 0, 0, 0, 0};
    UINT8       ResponseDataSize = 10, ResponseData[10], retry, Extend_Flag;
    UINT64      SLOT_AOC = 0, SLOT_AOC_FLAG = 0, temp_AOC;
    UINTN       Length = 1;
    
    DEBUG((-1, "PEI SmcAOCCardDetection entry.\n"));

    Status = (**PeiServices).LocatePpi(
                        PeiServices, 
                        &gEfiPeiStallPpiGuid, 
                        0, 
                        NULL, 
                        (VOID**)&gStallPpi);
    DEBUG((-1, "LocatePpi StallPpi %r\n", Status));
    if(Status)  return Status;

    Status = (**PeiServices).LocatePpi(
                    PeiServices,
                    &gEfiPeiSmbus2PpiGuid,
                    0,
                    NULL,
                    &Smbus);
    DEBUG((-1, "LocatePpi Smbus2Ppi %r\n", Status));
    if(Status)  return Status;

    if(Channel){
        Status = (**PeiServices).LocatePpi(
                        PeiServices,
                        &gEfiPeiIpmiTransportPpiGuid,
                        0,
                        NULL,
                        &IpmiTransportPpi);
        DEBUG((-1, "LocatePpi IpmiTransportPpi %r\n", Status));
        if(Status)  return Status;
    }
//switch first layer MUX, MB MUX channel
    if(MUX_Address && MUX_Channel){
        if(Channel){    // confirm smbus work by IPMI
            Mux_Switch[0] = Channel;
            Mux_Switch[1] = (UINT8)MUX_Address;
            Mux_Switch[2] = 1;          //read
            for(retry = 0; retry < 10; retry++){        // normally 5 is enough, we use 10 for master production situation
                Status = IpmiTransportPpi->SendIpmiCommand(
                                IpmiTransportPpi,
                                0x06,       // APP
                                BMC_LUN,
                                0x52,       // master write-read
                                Mux_Switch,
                                sizeof(Mux_Switch) - 1,
                                ResponseData,
                                &ResponseDataSize);
                if(ResponseDataSize == 1)       break;  // jump out if get mux data
                gStallPpi->Stall(PeiServices, gStallPpi, 100000);
            }
            if(ResponseDataSize == 0)   return EFI_DEVICE_ERROR;
        }
        SlaveAddress.SmbusDeviceAddress = MUX_Address >> 1;
        Status = Smbus->Execute(
                        Smbus,
                        SlaveAddress,
                        0,      // Mux channel register is 0x00
                        EfiSmbusWriteByte,
                        0,
                        &Length,
                        &MUX_Channel);
        DEBUG((-1, "Set address %x, channel %x, %r.\n", MUX_Address, MUX_Channel, Status));
        if(Status)  return Status;
    }
//switch second layer MUX, card MUX channel
    for(i = 0; i < SLOT_Tbl_Size; i++){ //switch MUX channel, i:slot index
        if(!SLOT_Tbl[i].Address && !SLOT_Tbl[i].Channel && !SLOT_Tbl[i].Bifurcate && !SLOT_Tbl[i].BDF)
            continue; //empty slot

        DEBUG((-1, "SLOT_Tbl[%d] = {%x, %x, %x, %x}\n", i, SLOT_Tbl[i].Address,
                        SLOT_Tbl[i].Channel, SLOT_Tbl[i].BDF, SLOT_Tbl[i].Bifurcate));
        found = 0;      //search AOC card from slots
        if(!SLOT_Tbl[i].Address && !SLOT_Tbl[i].Channel){       //for 1U riser card.
            SLOT_Tbl[i].Address = MUX_Address;                  //No MUX in 1U riser
            SLOT_Tbl[i].Channel = MUX_Channel;
        }
        SlaveAddress.SmbusDeviceAddress = SLOT_Tbl[i].Address >> 1;
        Status = Smbus->Execute(
                        Smbus,
                        SlaveAddress,
                        0,      // Mux channel register is 0x00
                        EfiSmbusWriteByte,
                        0,
                        &Length,
                        &(SLOT_Tbl[i].Channel));
        DEBUG((-1, "Set MUX address %x, channel %x, %r.\n", SLOT_Tbl[i].Address,
                        SLOT_Tbl[i].Channel, Status));
        if(Status)  break;

        for(FRU_Add = 0xa0; (FRU_Add < 0xaf) && !found; FRU_Add += 2){
            SetMem(TempBuffer, sizeof(TempBuffer), 0);
            SlaveAddress.SmbusDeviceAddress = FRU_Add >> 1;
            for(j = 0; j < 100; j++){
                Status = Smbus->Execute(
                                Smbus,
                                SlaveAddress,
                                j,
                                EfiSmbusReadByte,
                                0,
                                &Length,
                                &TempBuffer[j]);

                if(Status)      break;
            }
            DEBUG((-1, "Search slot %d, Add %x.\n", i + 1, FRU_Add));
            DEBUG((-1, "EEPROM Data :"));
            for(j = 0; j < 100; j++){
                if(!(j % 16)) DEBUG((-1, "\n"));
                DEBUG((-1, "%x ", TempBuffer[j]));
            }
            DEBUG((-1, "\n"));
            for(FRU_Index = 0; FRU_Index < sizeof(TempBuffer); FRU_Index++){
                if(!CompareMem(&TempBuffer[FRU_Index], "AOC", 3)){      //search card name.
                    PortNum = 0;
                    Status = SearchAOCTbl(&TempBuffer[FRU_Index], &AOC_Index, &PortNum);
                    if(!Status){
                        found = 1;                              //find AOC card.
                        DEBUG((-1, "AOC index = %x, PortNum = %x.\n", AOC_Index, PortNum));
                        if(!SLOT_Tbl[i].Address && !SLOT_Tbl[i].Channel){
                            SLOT_Tbl[i].Address = MUX_Address;
                            SLOT_Tbl[i].Channel = MUX_Channel;
                        }
                        SlaveAddress.SmbusDeviceAddress = SLOT_Tbl[i].Address >> 1;
                        Status = Smbus->Execute(
                                        Smbus,
                                        SlaveAddress,
                                        0,      // Mux channel register is 0x00
                                        EfiSmbusWriteByte,
                                        0,
                                        &Length,
                                        &(SLOT_Tbl[i].Channel));
                        DEBUG((-1, "Set MUX address %x, channel %x, %r.\n", SLOT_Tbl[i].Address,
                                SLOT_Tbl[i].Channel, Status));
                        Status = SmcAOCRedriver(Smbus, &TempBuffer[FRU_Index], AOC_Index);
                        break;
                    }
                }
            }
//SLOT_AOC: CPU0, bit0  ~ bit 7 is IIO0, bit8  ~ bit15 is IIO1, bit16 ~ bit23 is IIO2
//          CPU1, bit32 ~ bit39 is IIO0, bit40 ~ bit47 is IIO1, bit48 ~ bit55 is IIO2
//SLOT_AOC_FLAG: same with SLOT_AOC
            Extend_Flag = 0;      //Extend_Flag = 1 --> lane reverse
            j = 0;
            if((found) && (SLOT_Tbl[i].BDF >> 16)){   //check root port is from CPU or PCH
                if(SLOT_Tbl[i].Bifurcate == x16){
                	DEBUG((-1, "Card in x16 slot.\n"));
                    if((UINT8)SLOT_Tbl[i].BDF == 0x00){  //P0PE1A, 2A, 3A, P1PE1A, 2A, 3A
                        if(PortNum == 1)
                            Extend_Flag = 0x02; // Bridge AOC card in X16 slot
                        else if(PortNum == 2)
                            j = 0x02;   // x8 AOC card in X16 slot
                        else if(PortNum == 4)
                            j = 0x03;   // x16 AOC card in x16 slot
                    } else if((UINT8)SLOT_Tbl[i].BDF == 0x18){  //P0PE1D, 2D, 3D, P1PE1D, 2D, 3D
                    	DEBUG((-1, "Lane reversed.\n"));
                        Extend_Flag = 0x01;
                        if(PortNum == 1)
                            Extend_Flag = 0x02; // Bridge AOC card in X16 slot
                        else if(PortNum == 2)
                            j = 0x30;   // x8 AOC card in X16 slot(slot lane reverse)
                        else if(PortNum == 4)
                            j = 0x03;   // x16 AOC card in x16 slot
                    }
                }
                else if(SLOT_Tbl[i].Bifurcate == x8){
                	DEBUG((-1, "Card in x8 slot.\n"));
                    if(((UINT8)SLOT_Tbl[i].BDF == 0x00) || ((UINT8)SLOT_Tbl[i].BDF == 0x08)){
                        //P0PE1A, 1B, 2A, 2B, 3A, 3B, P1PE1A, 1B, 2A, 2B, 3A, 3B
                        if((UINT8)SLOT_Tbl[i].BDF == 0x08)      //lane reverse
                            Extend_Flag = 0x01;
                        if(PortNum == 1)
                            Extend_Flag = 0x02; // Bridge AOC in X8 slot
                        else
                            j = 0x02;
                    } else if(((UINT8)SLOT_Tbl[i].BDF == 0x10) || ((UINT8)SLOT_Tbl[i].BDF == 0x18)){
                    	DEBUG((-1, "Lane reversed.\n"));
                        //P0PE1C, 1D, 2C, 2D, 3C, 3D, P1PE1C, 1D, 2C, 2D, 3C, 3D
                        if((UINT8)SLOT_Tbl[i].BDF == 0x18)      //lane reverse
                            Extend_Flag = 0x10;
                        if(PortNum == 1)
                            Extend_Flag = 0x20; // Bridge AOC in X8 slot
                        else
                            j = 0x20;
                    }
                }

                k = (((SLOT_Tbl[i].BDF >> 16) & 0x0f) - 1) * 8;    //map to IIO, one IIO 8bits 
                k += (((SLOT_Tbl[i].BDF >> 20) & 0x0f) - 1) * 32;   //map to CPU, one CPU 32bits
                SLOT_AOC |= LShiftU64((UINT64)j, k);
                SLOT_AOC_FLAG |= LShiftU64((UINT64)Extend_Flag, k);
            }
        }
    }
    temp_AOC = PcdGet64(PcdSmcAOCLocation);
    temp_AOC |= SLOT_AOC;
    PcdSet64(PcdSmcAOCLocation, temp_AOC);
    DEBUG((-1, "PcdSmcAOCLocation = %lx.\n", temp_AOC));
    temp_AOC = PcdGet64(PcdSmcAOCExtendFlag);
    temp_AOC |= SLOT_AOC_FLAG;
    PcdSet64(PcdSmcAOCExtendFlag, temp_AOC);
    DEBUG((-1, "PcdSmcAOCExtendFlag = %lx.\n", temp_AOC));

    DEBUG((-1, "PEI SmcAOCCardDetection end.\n"));
    return      EFI_SUCCESS;
}

EFI_STATUS  EFIAPI
SmcAOCSetNVMeMode(
    IN  EFI_PEI_SERVICES  **PeiServices,
    IN  UINT8   Channel,        //IPMI Smbus used, when Channel = 0, send from PCH Smbus
    IN  UINTN   MUX_Address,    //PCH Smbus used
    IN  UINT8   MUX_Channel,    //PCH Smbus used
    IN  SMC_SLOT_INFO   SLOT_Tbl[],     //PCH Smbus used
    IN  UINT8   SLOT_Tbl_Size           //PCH Smbus used
)
{
    EFI_STATUS  Status;
    PEI_IPMI_TRANSPORT_PPI      *IpmiTransportPpi;
    EFI_PEI_SMBUS2_PPI          *Smbus;
    EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress;
    EFI_PEI_STALL_PPI           *gStallPpi;
    UINT8       IPMI_SMB_CMD[] = {0, 0, 0, 0, 0}, i, j, k;
    UINT8       CPLD_Add_Tbl[] = BPN_CPLD_Address, Bitmap;
    UINT8       ResponseDataSize = 10, ResponseData[10], retry;
    UINT64      NVMe_Mode;
    UINTN       Length = 1;

    DEBUG((-1, "PEI SmcAOCSetNVMeMode entry.\n"));

    if(!PcdGet64(PcdSmcAOCLocation) && !PcdGet64(PcdSmcAOCExtendFlag)){
        DEBUG((-1, "No NVMe port detected.\n"));
        return      EFI_SUCCESS;
    }

    NVMe_Mode = PcdGet64(PcdSmcAOCNVMeMode);
    
    Status = (**PeiServices).LocatePpi(
                    PeiServices, 
                    &gEfiPeiStallPpiGuid, 
                    0, 
                    NULL, 
                    (VOID**)&gStallPpi);
    DEBUG((-1, "LocatePpi Smbus2Ppi %r\n", Status));
    if(Status)  return Status;

    Status = (**PeiServices).LocatePpi(
                    PeiServices,
                    &gEfiPeiSmbus2PpiGuid,
                    0,
                    NULL,
                    &Smbus);

    if(Status)  return Status;

    if(Channel){
        Status = (**PeiServices).LocatePpi(
                        PeiServices,
                        &gEfiPeiIpmiTransportPpiGuid,
                        0,
                        NULL,
                        &IpmiTransportPpi);
        DEBUG((-1, "LocatePpi IpmiTransportPpi %r\n", Status));
        if(Status)  return Status;
    }
//if only IPMI smbus channel, send by IPMI
    if(Channel && !MUX_Address && !MUX_Channel){
        DEBUG((-1, "Send from IPMI Smbus.\n"));
        for(j = 0; j < sizeof(CPLD_Add_Tbl); j++){
            DEBUG((-1, "CPLD address %x.\n", CPLD_Add_Tbl[j]));
            if(!CPLD_Add_Tbl[j])
                continue;
            DEBUG((-1, "Value = %x.\n", (UINT16)(RShiftU64(NVMe_Mode, CPLD_Max_Port * j))));
            //one CPLD support max 32 ports
            for(k = 0; k < 4; k++){
                Bitmap = (UINT8)(RShiftU64(NVMe_Mode, (CPLD_Max_Port * j) + (8 * k)));
                IPMI_SMB_CMD[0] = Channel;                 //IPMI smbus channel
                IPMI_SMB_CMD[1] = CPLD_Add_Tbl[j];         //IPMI send to CPLD address
                IPMI_SMB_CMD[2] = 0;                       //write
                IPMI_SMB_CMD[3] = 0x08 + k;                //register, start from 0x08
                IPMI_SMB_CMD[4] = Bitmap;                  //data
                DEBUG((-1, "CPLD add %x, reg %x, Bitmap %x.\n",\
                        IPMI_SMB_CMD[1], IPMI_SMB_CMD[3], IPMI_SMB_CMD[4]));
                for(retry = 0; (retry < 3) && Bitmap; retry++){
                    Status = IpmiTransportPpi->SendIpmiCommand(
                                    IpmiTransportPpi,
                                    0x06,       // APP
                                    BMC_LUN,
                                    0x52,       // master write-read
                                    IPMI_SMB_CMD,
                                    sizeof(IPMI_SMB_CMD),
                                    ResponseData,
                                    &ResponseDataSize);
                    DEBUG((-1, "Send Bitmap Status = %r.\n", Status));
                    if(!Status) break;
                }
            }
        }
    } else {
        DEBUG((-1, "Send from PCH Smbus.\n"));
//switch MB MUX channel
        if(MUX_Address && MUX_Channel){
            if(Channel){        // confirm smbus channel by IPMI
                IPMI_SMB_CMD[0] = Channel;
                IPMI_SMB_CMD[1] = (UINT8)MUX_Address;
                IPMI_SMB_CMD[2] = 1;          //read
                IPMI_SMB_CMD[3] = 0;
                for(retry = 0; retry < 10; retry++){        // normally 5 is enough, we use 10 for master production situation
                    Status = IpmiTransportPpi->SendIpmiCommand(
                                    IpmiTransportPpi,
                                    0x06,       // APP
                                    BMC_LUN,
                                    0x52,       // master write-read
                                    IPMI_SMB_CMD,
                                    sizeof(IPMI_SMB_CMD) - 1,
                                    ResponseData,
                                    &ResponseDataSize);
                    if(ResponseDataSize == 1)       break;  // jump out if get mux data
                    gStallPpi->Stall(PeiServices, gStallPpi, 100000);
                }
                if(ResponseDataSize == 0)   return EFI_DEVICE_ERROR;
            }
            SlaveAddress.SmbusDeviceAddress = MUX_Address >> 1;
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            0,      // Mux channel register is 0x00
                            EfiSmbusWriteByte,
                            0,
                            &Length,
                            &MUX_Channel);
            DEBUG((-1, "Set address %x, channel %x, %r.\n", MUX_Address, MUX_Channel, Status));
            if(Status)  return Status;
        }
//PCH directly connect to BPN CPLD
        if(!SLOT_Tbl_Size){
            DEBUG((-1, "PCH directly connect to BPN CPLD.\n"));
            for(j = 0; j < sizeof(CPLD_Add_Tbl); j++){
                DEBUG((-1, "CPLD address %x.\n", CPLD_Add_Tbl[j]));
                if(!CPLD_Add_Tbl[j])
                    continue;
                DEBUG((-1, "Vaule = %x.\n", (UINT16)(RShiftU64(NVMe_Mode, CPLD_Max_Port * j))));                
                //one CPLD support max 32 ports
                for(k = 0; k < 4; k++){
                    Bitmap = (UINT8)(RShiftU64(NVMe_Mode, (CPLD_Max_Port * j) + (8 * k)));
                    IPMI_SMB_CMD[0] = Channel;                 //IPMI smbus channel
                    IPMI_SMB_CMD[1] = CPLD_Add_Tbl[j];         //IPMI send to CPLD address
                    IPMI_SMB_CMD[2] = 0;                       //write
                    IPMI_SMB_CMD[3] = 0x08 + k;                //register, start from 0x08
                    IPMI_SMB_CMD[4] = Bitmap;                  //data
                    DEBUG((-1, "CPLD add %x, reg %x, Bitmap %x.\n",\
                            IPMI_SMB_CMD[1], IPMI_SMB_CMD[3], IPMI_SMB_CMD[4]));
                    for(retry = 0; (retry < 3) && Bitmap; retry++){
                        Status = IpmiTransportPpi->SendIpmiCommand(
                                        IpmiTransportPpi,
                                        0x06,       // APP
                                        BMC_LUN,
                                        0x52,       // master write-read
                                        IPMI_SMB_CMD,
                                        sizeof(IPMI_SMB_CMD),
                                        ResponseData,
                                        &ResponseDataSize);
                        DEBUG((-1, "Send Bitmap Status = %r.\n", Status));
                        if(!Status) break;
                    }
                }
            }
            return Status;
        }
//switch card MUX channel
        for(i = 0; i < SLOT_Tbl_Size; i++){ //switch MUX channel, i:slot index
            if(!SLOT_Tbl[i].Address && !SLOT_Tbl[i].Channel && !SLOT_Tbl[i].Bifurcate && !SLOT_Tbl[i].BDF)
                continue;         //empty slot

            DEBUG((-1, "SLOT_Tbl[%d] = {%x, %x, %x, %x}\n", i, SLOT_Tbl[i].Address,
                            SLOT_Tbl[i].Channel, SLOT_Tbl[i].BDF, SLOT_Tbl[i].Bifurcate));

            if(!SLOT_Tbl[i].Address && !SLOT_Tbl[i].Channel){
                SLOT_Tbl[i].Address = MUX_Address;
                SLOT_Tbl[i].Channel = MUX_Channel;
            }
            SlaveAddress.SmbusDeviceAddress = SLOT_Tbl[i].Address >> 1;
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            0,      // Mux channel register is 0x00
                            EfiSmbusWriteByte,
                            0,
                            &Length,
                            &(SLOT_Tbl[i].Channel));
            DEBUG((-1, "Set MUX address %x, channel %x, %r.\n", SLOT_Tbl[i].Address,
                            SLOT_Tbl[i].Channel, Status));
            if(Status)  break;
//send NVMe mode bitmap to CPLD
            for(j = 0; j < sizeof(CPLD_Add_Tbl); j++){
                DEBUG((-1, "CPLD address %x.\n", CPLD_Add_Tbl[j]));
                if(!CPLD_Add_Tbl[j])
                    continue;
                DEBUG((-1, "Value = %x.\n", (UINT16)(RShiftU64(NVMe_Mode, CPLD_Max_Port * j))));
                //one CPLD support max 32 ports
                for(k = 0; k < 4; k++){
                    Bitmap = (UINT8)(RShiftU64(NVMe_Mode, (CPLD_Max_Port * j) + (8 * k)));
                    IPMI_SMB_CMD[0] = Channel;                 //IPMI smbus channel
                    IPMI_SMB_CMD[1] = CPLD_Add_Tbl[j];         //IPMI send to CPLD address
                    IPMI_SMB_CMD[2] = 0;                       //write
                    IPMI_SMB_CMD[3] = 0x08 + k;                //register, start from 0x08
                    IPMI_SMB_CMD[4] = Bitmap;                  //data
                    DEBUG((-1, "CPLD add %x, reg %x, Bitmap %x.\n",\
                            IPMI_SMB_CMD[1], IPMI_SMB_CMD[3], IPMI_SMB_CMD[4]));
                    for(retry = 0; (retry < 3) && Bitmap; retry++){
                        Status = IpmiTransportPpi->SendIpmiCommand(
                                        IpmiTransportPpi,
                                        0x06,       // APP
                                        BMC_LUN,
                                        0x52,       // master write-read
                                        IPMI_SMB_CMD,
                                        sizeof(IPMI_SMB_CMD),
                                        ResponseData,
                                        &ResponseDataSize);
                        DEBUG((-1, "Send Bitmap Status = %r.\n", Status));
                        if(!Status) break;
                    }
                }
            }
        }
    }

    DEBUG((-1, "PEI SmcAOCSetNVMeMode end.\n"));
    return      EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcPLXDetection (
  ) {
  UINT16    Bus, Dev, Fun;
  UINT8     SecBus;
  UINT32    VidDid;
  UINT16    PlxId, ClassCode;
  UINTN     Address;
  BOOLEAN   AssignBus;
  UINT64    AocExtendFlag, TempAoc;
  UINT16    PlxFlag, ShiftCount;
  UINT8     Index;
  UINT16    ScanBDF;
  UINT32    TempBDF[] = {
              0x00110000, 0x00110008, 0x00110010, 0x00110018, // CPU0,IOU0: 1A, 1B, 1C, 1D
              0x00120000, 0x00120008, 0x00120010, 0x00120018, // CPU0,IOU1: 2A, 2B, 2C, 2D
              0x00130000, 0x00130008, 0x00130010, 0x00130018, // CPU0,IOU2: 3A, 3B, 3C, 3D
              0x00210000, 0x00210008, 0x00210010, 0x00210018, // CPU1,IOU0: 1A, 1B, 1C, 1D
              0x00220000, 0x00220008, 0x00220010, 0x00220018, // CPU1,IOU1: 2A, 2B, 2C, 2D
              0x00230000, 0x00230008, 0x00230010, 0x00230018, // CPU1,IOU2: 3A, 3B, 3C, 3D
              };
#if EFI_DEBUG
  UINT8     HeaderType;
#endif

  DEBUG ((DEBUG_INFO, "PEI SmcPLXDetection Entry.\n"));

#if EFI_DEBUG
  //
  // For debug purpose: Please make sure dupmed IIO buses are initialized. The scan point need be adjusted if the buses are 0x11/0x12/0x13.
  //   SmcPLXDetection: BUS = 0x17, DEV = 0x00, FUN = 0x00, VID&DID = 0x20308086
  //   SmcPLXDetection: BUS = 0x3A, DEV = 0x00, FUN = 0x00, VID&DID = 0x20308086
  //   SmcPLXDetection: BUS = 0x5D, DEV = 0x00, FUN = 0x00, VID&DID = 0x20308086
  //   SmcPLXDetection: BUS = 0x85, DEV = 0x00, FUN = 0x00, VID&DID = 0x20308086
  //   SmcPLXDetection: BUS = 0xAE, DEV = 0x00, FUN = 0x00, VID&DID = 0x20308086
  //   SmcPLXDetection: BUS = 0xD7, DEV = 0x00, FUN = 0x00, VID&DID = 0x20308086
  //
  for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; Dev++) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; Fun++) {
        Address = PCI_LIB_ADDRESS (Bus, Dev, Fun, PCI_VENDOR_ID_OFFSET);
        VidDid = PciRead32 (Address);
        
        if (VidDid == 0xFFFFFFFF) {
          continue;
        }
      
        DEBUG ((DEBUG_INFO, "SmcPLXDetection: BUS = 0x%02x, DEV = 0x%02x, FUN = 0x%02x, VID&DID = 0x%08x\n", Bus, Dev, Fun, VidDid));

        if (Fun == 0) {
          Address = PCI_LIB_ADDRESS (Bus, 0, 0, PCI_HEADER_TYPE_OFFSET);
          HeaderType = PciRead8 (Address);

          if (HeaderType & BIT7) {
            break;
          }
        }
      }
    }
  }
#endif

  PlxFlag = 0x02;
  AocExtendFlag = 0;
  for (Index = 0; Index < sizeof (TempBDF) / sizeof (UINT32); Index++) {
    //
    // Convert 1A, 1B, 1C, 1D... to correct BUS/DEV/FUN:
    //   SmcPLXDetection: TempBDF[12] = 0x00210000, ScanBDF = 0x00008500
    //   SmcPLXDetection: TempBDF[13] = 0x00210008, ScanBDF = 0x00008508
    //   SmcPLXDetection: TempBDF[14] = 0x00210010, ScanBDF = 0x00008510
    //   SmcPLXDetection: TempBDF[15] = 0x00210018, ScanBDF = 0x00008518
    //
    ScanBDF = SmcBDFTransfer (TempBDF[Index]);
    DEBUG ((DEBUG_INFO, "SmcPLXDetection: TempBDF[%d] = 0x%08x, ScanBDF = 0x%08x\n", Index, TempBDF[Index], ScanBDF));

    Bus = (ScanBDF & 0xFF00) >> 8;
    Dev = (ScanBDF & 0x00F8) >> 3;
    Fun = ScanBDF & 0x0007;
    Address = PCI_LIB_ADDRESS (Bus, Dev, Fun, PCI_VENDOR_ID_OFFSET);
    VidDid = PciRead32 (Address);
    DEBUG ((DEBUG_INFO, "SmcPLXDetection: RootBUS = 0x%02x, DEV = 0x%02x, FUN = 0x%02x, VidDid = 0x%08x\n", Bus, Dev, Fun, VidDid));

    if (VidDid == 0xFFFFFFFF) {
      continue;
    }

    Address = PCI_LIB_ADDRESS (Bus, Dev, Fun, PCI_CLASSCODE_OFFSET + 1);
    ClassCode = PciRead16 (Address);

    if (ClassCode != 0x0604) {
      continue;
    }

    Address = PCI_LIB_ADDRESS (Bus, Dev, Fun, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    SecBus = PciRead8 (Address);

    AssignBus = FALSE;
    if (SecBus == 0) {
      //
      // Assign a temp bus number for child devices.
      //
      SecBus = Bus + 2;
      PciWrite8 (Address, SecBus);
      AssignBus = TRUE;
    }

    Address = PCI_LIB_ADDRESS (SecBus, 0, 0, PCI_VENDOR_ID_OFFSET);
    VidDid = PciRead32 (Address);
    DEBUG ((DEBUG_INFO, "SmcPLXDetection:   SecBUS = 0x%02x, DEV = 0x%02x, FUN = 0x%02x, VID&DID = 0x%08x, AssignBus = %d\n", SecBus, 0, 0, VidDid, AssignBus));

    Address = PCI_LIB_ADDRESS (SecBus, 0, 0, PCI_CLASSCODE_OFFSET + 1);
    ClassCode = PciRead16 (Address);
    Address = PCI_LIB_ADDRESS (SecBus, 0, 0, 0xA8);
    PlxId = PciRead16 (Address);

    if ((ClassCode == 0x0604) && (PlxId == 0x15D9)) {
      //
      // Save PLX location to PcdSmcAOCExtendFlag. (PLX value = 0x2, assume a PLX bridge is found in bus 0x87)
      //   SmcPLXDetection: RootBUS = 0x85, DEV = 0x00, FUN = 0x00, VidDid = 0x20308086
      //   SmcPLXDetection:   SecBUS = 0x87, DEV = 0x00, FUN = 0x00, VID&DID = 0x871810B5, AssignBus = 1
      //   SmcPLXDetection:     PLX bridge is found
      //   SmcPLXDetection:     AocExtendFlag = 0x200000000
      //   
      //   PcdSmcAOCExtendFlag:
      //                    CPU1                CPU0 (CPU  = 32 bits)
      //     xxxx IOU2 IOU1 IOU0 xxxx IOU2 IOU1 IOU0 (IOU  = 8 bits)
      //          DCBA DCBA DCBA      DCBA DCBA DCBA (Port = 2 bits)
      //   0x 0 0  0 0  0 0  0 2  0 0  0 0  0 0  0 0
      // 
      DEBUG ((DEBUG_INFO, "SmcPLXDetection:     PLX bridge is found\n"));
      ShiftCount = (((TempBDF[Index] >> 20) & 0x0F) - 1) * 32;
      ShiftCount += (((TempBDF[Index] >> 16) & 0x0F) - 1) * 8;
      ShiftCount += ((TempBDF[Index] >> 3) & 0x07) * 2;
      AocExtendFlag |= LShiftU64(PlxFlag, ShiftCount);
      DEBUG ((DEBUG_INFO, "SmcPLXDetection:     AocExtendFlag = 0x%lx\n", AocExtendFlag));
    }

    if (AssignBus) {
      //
      // Clear temp bus number.
      //
      Address = PCI_LIB_ADDRESS (Bus, Dev, Fun, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
      PciWrite8 (Address, 0);
    }
  }

  TempAoc = PcdGet64 (PcdSmcAOCExtendFlag);
  TempAoc |= AocExtendFlag;
  PcdSet64 (PcdSmcAOCExtendFlag, TempAoc);
  DEBUG ((DEBUG_INFO, "SmcPLXDetection: PcdSmcAOCExtendFlag = 0x%lx\n", TempAoc));

  DEBUG ((DEBUG_INFO, "PEI SmcPLXDetection End.\n"));

  return EFI_SUCCESS;
}

PEI_SMC_AOC_PROTOCOL SmcAOCPeiProtocol =
{
    SmcAOCCardDetection,
    SmcAOCBifurcation,
    SmcAOCSetNVMeMode,
    SmcPLXDetection
};

EFI_STATUS
SmcAOCPeiInit(
    IN	EFI_FFS_FILE_HEADER	*FfsHeader,
    IN	CONST EFI_PEI_SERVICES	**PeiServices
)
{
    EFI_STATUS	Status;
    EFI_PEI_PPI_DESCRIPTOR	*mPpiList = NULL;

    DEBUG((-1, "SmcAOCPeiInit start.\n"));

    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_PPI_DESCRIPTOR), &mPpiList);

    mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    mPpiList->Guid = &gSmcAOCPeiProtocolGuid;
    mPpiList->Ppi = &SmcAOCPeiProtocol;

    Status = (**PeiServices).InstallPpi(PeiServices, mPpiList);
    if(EFI_ERROR(Status)){
    	DEBUG((-1, "install SmcAOCPeiProtocol fail.\n"));
    	return EFI_SUCCESS;
    }

    //
    // Install a callback here to reduce RC modification.
    // SmcAOCProtocol->SmcAOCPeiPlxDetection still can be inserted into RC if there is no proper event for notification.
    //
    Status = PeiServicesNotifyPpi (mNotifyDesc);
    ASSERT_EFI_ERROR (Status);

    DEBUG((-1, "SmcAOCPeiInit end.\n"));
    return	EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcAocPeiPlxCallback (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN  VOID                      *Ppi
  )
{
  EFI_STATUS Status;

  Status = SmcPLXDetection ();
  ASSERT_EFI_ERROR (Status);
  
  return EFI_SUCCESS;
}