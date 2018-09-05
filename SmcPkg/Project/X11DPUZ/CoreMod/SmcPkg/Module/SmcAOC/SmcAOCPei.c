//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
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
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Ppi/Smbus2.h>
#include <Ppi/IPMITransportPpi.h>
#include <Ppi/Stall.h>
#include <Token.h>
#include "SmcLib.h"
#include "SmcAOC.h"
#include "SmcAOCPei.h"

EFI_GUID gSmcAOCPeiProtocolGuid = EFI_SMC_AOC_PEI_PROTOCOL_GUID;

#define AOC_ReD_Data_Max 30

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
    EFI_PEI_SMBUS2_PPI          *Smbus;
    EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress;
    UINT8       TempBuffer[100], FRU_Add, FRU_Index, AOC_Index;
    UINT8       i, j, k, found, PortNum;
    UINT64      SLOT_AOC = 0, temp_AOC;
    UINTN       Length = 1;
    
    DEBUG((-1, "PEI SmcAOCCardDetection entry.\n"));

    Status = (**PeiServices).LocatePpi(
                    PeiServices,
                    &gEfiPeiSmbus2PpiGuid,
                    0,
                    NULL,
                    &Smbus);

    if(Status)  return Status;
//switch MB MUX channel
    if(MUX_Address && MUX_Channel){
        SlaveAddress.SmbusDeviceAddress = MUX_Address >> 1;
        Status = Smbus->Execute(
                        Smbus,
                        SlaveAddress,
                        0,      // Mux channel register is 0x00
                        EfiSmbusSendByte,	// EfiSmbusWriteByte,
                        0,
                        &Length,
                        &MUX_Channel);
        DEBUG((-1, "Set address %x, channel %x, %r.\n", MUX_Address, MUX_Channel, Status));
        if(Status)  return Status;
    }
//switch card MUX channel
    for(i = 0; i < SLOT_Tbl_Size; i++){ //switch MUX channel, i:slot index
        if((!SLOT_Tbl[i].Bifurcate) || (!SLOT_Tbl[i].BDF))    continue;         //empty slot

        DEBUG((-1, "SLOT_Tbl[%d] = {%x, %x, %x, %x}\n", i, SLOT_Tbl[i].Address,
                        SLOT_Tbl[i].Channel, SLOT_Tbl[i].BDF, SLOT_Tbl[i].Bifurcate));
        found = 0;
        if(!SLOT_Tbl[i].Address && !SLOT_Tbl[i].Channel){
            SLOT_Tbl[i].Address = MUX_Address;
            SLOT_Tbl[i].Channel = MUX_Channel;
        }
        SlaveAddress.SmbusDeviceAddress = SLOT_Tbl[i].Address >> 1;
        Status = Smbus->Execute(
                        Smbus,
                        SlaveAddress,
                        0,      // Mux channel register is 0x00
                        EfiSmbusSendByte,	// EfiSmbusWriteByte,
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
            for(FRU_Index = 0; FRU_Index < sizeof(TempBuffer); FRU_Index++){
                if(!CompareMem(&TempBuffer[FRU_Index], "AOC", 3)){      //search card name.
                    PortNum = 0;
                    Status = SearchAOCTbl(&TempBuffer[FRU_Index], &AOC_Index, &PortNum);
                    if(!Status){
                        found = 1;                              //find AOC card.
                        DEBUG((-1, "AOC database index = %x.\n", AOC_Index));
                        if(!SLOT_Tbl[i].Address && !SLOT_Tbl[i].Channel){
                            SLOT_Tbl[i].Address = MUX_Address;
                            SLOT_Tbl[i].Channel = MUX_Channel;
                        }
                        SlaveAddress.SmbusDeviceAddress = SLOT_Tbl[i].Address >> 1;
                        Status = Smbus->Execute(
                                        Smbus,
                                        SlaveAddress,
                                        0,      // Mux channel register is 0x00
                                        EfiSmbusSendByte,	// EfiSmbusWriteByte,
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
            if((found) && (SLOT_Tbl[i].BDF >> 16)){   //check root port is from CPU or PCH
                if(SLOT_Tbl[i].Bifurcate == x16){
                    if((UINT8)SLOT_Tbl[i].BDF == 0x00){  //P0PE1A, 2A, 3A, P1PE1A, 2A, 3A
                        if(PortNum == 2)
                            j = 0x02;   // x8 AOC card in X16 slot
                        else if(PortNum == 4)
                            j = 0x03;   // x16 AMC card in x16 slot
                    } else if((UINT8)SLOT_Tbl[i].BDF == 0x18){  //P0PE1D, 2D, 3D, P1PE1D, 2D, 3D
                        if(PortNum == 2)
                            j = 0x30;   // x8 AOC card in X16 slot(slot lane reverse)
                        else if(PortNum == 4)
                            j = 0x03;   // x16 AMC card in x16 slot
                    }
                }
                else if(SLOT_Tbl[i].Bifurcate == x8){
                    //P0PE1A, 1B, 2A, 2B, 3A, 3B, P1PE1A, 1B, 2A, 2B, 3A, 3B
                    if(((UINT8)SLOT_Tbl[i].BDF == 0x00) || ((UINT8)SLOT_Tbl[i].BDF == 0x08))
                        j = 0x02;
                    //P0PE1C, 1D, 2C, 2D, 3C, 3D, P1PE1C, 1D, 2C, 2D, 3C, 3D
                    else if(((UINT8)SLOT_Tbl[i].BDF == 0x10) || ((UINT8)SLOT_Tbl[i].BDF == 0x18))
                        j = 0x20;
                }

                k = (((SLOT_Tbl[i].BDF >> 16) & 0x0f) - 1) * 8;    //map to IIO, one IIO 8bits 
                k += (((SLOT_Tbl[i].BDF >> 20) & 0x0f) - 1) * 32;   //map to CPU, one CPU 32bits
                SLOT_AOC |= LShiftU64((UINT64)j, k); 
            }
        }
    }
    temp_AOC = PcdGet64(PcdSmcAOCLocation);
    temp_AOC |= SLOT_AOC;
    PcdSet64(PcdSmcAOCLocation, temp_AOC);
    DEBUG((-1, "temp_AOC = %lx.\n", temp_AOC));

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
    UINT8       Nvme_7_0[] = {0, BPN_CPLD_Address, 0x00, 0x08, 0};
    UINT8       Nvme_15_8[] = {0, BPN_CPLD_Address, 0x00, 0x09, 0};
    UINT8       ResponseDataSize = 10, ResponseData[10], retry;
    UINT8       Bitmap_7_0, Bitmap_15_8, i;
    UINT16      NVMe_Mode;
    UINTN       Length = 1;

    DEBUG((-1, "PEI SmcAOCSetNVMeMode entry.\n"));

    if(!PcdGet64(PcdSmcAOCLocation)){
        DEBUG((-1, "No NVMe port detected.\n"));
        return      EFI_SUCCESS;
    }
    
    NVMe_Mode = PcdGet16(PcdSmcAOCNVMeMode);
    DEBUG((-1, "NVMe_Mode = %x.\n", NVMe_Mode));
    Bitmap_7_0 = (UINT8)(NVMe_Mode & 0x00FF);
    Bitmap_15_8 = (UINT8)((NVMe_Mode & 0xFF00) >> 8);
    
    Status = (**PeiServices).LocatePpi(
                    PeiServices, 
                    &gEfiPeiStallPpiGuid, 
                    0, 
                    NULL, 
                    (VOID**)&gStallPpi);

    if(!Channel){   //Send from PCH Smbus
        DEBUG((-1, "Send from PCH Smbus.\n"));
        Status = (**PeiServices).LocatePpi(
                        PeiServices,
                        &gEfiPeiSmbus2PpiGuid,
                        0,
                        NULL,
                        &Smbus);
    
        if(Status)  return Status;
//switch MB MUX channel
        if(MUX_Address && MUX_Channel){
            SlaveAddress.SmbusDeviceAddress = MUX_Address >> 1;
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            0,      // Mux channel register is 0x00
                            EfiSmbusSendByte,	//EfiSmbusWriteByte,
                            0,
                            &Length,
                            &MUX_Channel);
            DEBUG((-1, "Set address %x, channel %x, %r.\n", MUX_Address, MUX_Channel, Status));
            if(Status)  return Status;
        }
//PCH directly connect to BPN CPLD
        if(!SLOT_Tbl_Size){
            DEBUG((-1, "PCH directly connect to BPN CPLD.\n"));
            for(retry = 0; retry < 3; retry++){
                SlaveAddress.SmbusDeviceAddress = BPN_CPLD_Address >> 1;
                Status = Smbus->Execute(
                                Smbus,
                                SlaveAddress,
                                0x08,      // bitmap 7~0
                                EfiSmbusWriteByte,
                                0,
                                &Length,
                                &Bitmap_7_0);
                DEBUG((-1, "Send Bitmap_7_0 Status = %r.\n", Status));      
                if(!Status)     break;
                gStallPpi->Stall(PeiServices, gStallPpi, 100000);
            }

            for(retry = 0; retry < 3; retry++){
                SlaveAddress.SmbusDeviceAddress = BPN_CPLD_Address >> 1;
                Status = Smbus->Execute(
                                Smbus,
                                SlaveAddress,
                                0x09,      // bitmap 15~8
                                EfiSmbusWriteByte,
                                0,
                                &Length,
                                &Bitmap_15_8);
                DEBUG((-1, "Send Bitmap_15_8 Status = %r.\n", Status));
                if(!Status)     break;
                gStallPpi->Stall(PeiServices, gStallPpi, 100000);
            }
            return Status;
        }
//switch card MUX channel
        for(i = 0; i < SLOT_Tbl_Size; i++){ //switch MUX channel, i:slot index
            if((!SLOT_Tbl[i].Bifurcate) || (!SLOT_Tbl[i].BDF))    continue;         //empty slot

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
                            EfiSmbusSendByte,	// EfiSmbusWriteByte,
                            0,
                            &Length,
                            &(SLOT_Tbl[i].Channel));
            DEBUG((-1, "Set MUX address %x, channel %x, %r.\n", SLOT_Tbl[i].Address,
                            SLOT_Tbl[i].Channel, Status));
            if(Status)  break;
//send NVMe mode bitmap to CPLD
            SlaveAddress.SmbusDeviceAddress = BPN_CPLD_Address >> 1;
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            0x08,      // bitmap 7~0
                            EfiSmbusWriteByte,
                            0,
                            &Length,
                            &Bitmap_7_0);
            DEBUG((-1, "Send Bitmap_7_0 Status = %r.\n", Status));
            if(Status)  continue;
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            0x09,      // bitmap 15~8
                            EfiSmbusWriteByte,
                            0,
                            &Length,
                            &Bitmap_15_8);
            DEBUG((-1, "Send Bitmap_15_8 Status = %r.\n", Status));
            if(!Status) break;
        }
    } else {     //Send from IPMI Smbus
        DEBUG((-1, "Send from IPMI Smbus.\n"));
        Nvme_7_0[0] = Channel;
        Nvme_15_8[0] = Channel;
        Nvme_7_0[4] = (UINT8)(NVMe_Mode & 0x00FF);
        Nvme_15_8[4] = (UINT8)((NVMe_Mode & 0xFF00) >> 8);

        Status = (**PeiServices).LocatePpi(
                        PeiServices,
                        &gEfiPeiIpmiTransportPpiGuid,
                        0,
                        NULL,
                        &IpmiTransportPpi);
    
        if(Status)  return Status;

        for(retry = 0; retry < 3; retry++){
            Status = IpmiTransportPpi->SendIpmiCommand(
                            IpmiTransportPpi,
                            0x06,       // APP
                            BMC_LUN,
                            0x52,       // master write-read
                            Nvme_7_0,
                            sizeof(Nvme_7_0),
                            ResponseData,
                            &ResponseDataSize);
            DEBUG((-1, "Send Nvme_7_0 Status = %r.\n", Status));
            if(!Status) break;
        }
    
        for(retry = 0; retry < 3; retry++){
            Status = IpmiTransportPpi->SendIpmiCommand(
                            IpmiTransportPpi,
                            0x06,       // APP
                            BMC_LUN,
                            0x52,       // master write-read
                            Nvme_15_8,
                            sizeof(Nvme_15_8),
                            ResponseData,
                            &ResponseDataSize);
            DEBUG((-1, "Send Nvme_15_8 Status = %r.\n", Status));
            if(!Status) break;
        }
    }

    DEBUG((-1, "PEI SmcAOCSetNVMeMode end.\n"));
    return      EFI_SUCCESS;
}

PEI_SMC_AOC_PROTOCOL SmcAOCPeiProtocol =
{
    SmcAOCCardDetection,
    SmcAOCBifurcation,
    SmcAOCSetNVMeMode
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

    DEBUG((-1, "SmcAOCPeiInit end.\n"));
    return	EFI_SUCCESS;
}
