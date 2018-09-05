//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//   Rev. 1.08
//     Bug Fix: Fix code change side effect.
//     Reason:  
//     Auditor: Kasber Chen
//     Date:    Jun/13/2017
//
//   Rev. 1.07
//     Bug Fix: Change m/b MUX access from SMBus to BMC and add feedback check
//     Reason:  BMC -> m/b MUX looks more robust than PCH -> m/b MUX
//     Auditor: Donald Han
//     Date:    Jun/08/2017
//
// Rev. 1.06
//    Bug Fix:  Fix 2nd level MUX is not set correctly before programming behind redriver chip
//    Reason:   We cannot use "&" for 2 conditions AND, use "&&" instead
//    Auditor:  Donald Han
//    Date:     May/04/2017
//
// Rev. 1.05
//   Reason:	Add riser card MUX channel change.
//   Auditro:	Kasber Chen
//   Date:      Feb/21/2017
//
//  Rev. 1.04
//    Bug Fix:  1. Revise redrive programming counter.
//              2. Program riser card mux before programming redrive value
//    Reason:   1. array includes address/value, so the counter should be half
//				2. We need to switch riser card mux to redriver channel first
//    Auditor:  Donald Han
//    Date:     Feb/17/2017
//
//  Rev. 1.03
//    Bug Fix:  Add riser card redriver function.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/10/2017
//
//  Rev. 1.02
//      Bug Fixed:  Support AOC NVMe card in riser card slots.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Nov/16/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//    Rev.  1.00
//     Bug Fix:
//     Reason:	Initialize revision
//     Auditor:	Kasber Chen
//     Date:  	04/18/2016
//
//***************************************************************************
//***************************************************************************
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/Smbus2.h>
#include "SmcRiserCardCommon.h"
#include "SmcRiserCard.h"
#include "SmcRiserCardPei.h"

#define RC_ReD_Data_Max 30

EFI_GUID gSmcRiserCardPeiProtocolGuid = EFI_SMCRISERCARD_PEI_DRIVER_PROTOCOL_GUID;

PEI_SMCRISERCARD_PROTOCOL SmcRiserCardPeiProtocol =
{
    SmcRiserCardBifuracate,
    SmcRiserCardSlotBDFTable,
    SmcRiserCardRedriver
};

EFI_STATUS EFIAPI
SmcRiserCardRedriver(
    IN  EFI_PEI_SERVICES        **PeiServices,
    IN  UINT8   Channel,
    IN  UINTN   MUX_Address,
    IN  UINT8   MUX_Channel,
    IN  UINT8   SXB_Index
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress;
    EFI_PEI_SMBUS2_PPI          *Smbus;
    RC_ReD      RC_ReD_Data[RC_ReD_Data_Max];
    UINT8       RC_ReD_Add, RC_ReD_Index, RC_ReD_Data_Index, RC_ReD_Data_Size;
    UINT8       RC_MUX_Address, RC_MUX_Channel;
    UINTN       Length = 1;
    
    DEBUG((-1, "SmcRiserCardRedriver start.\n"));
    
    if(!SXB_Index)       return Status;

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
        DEBUG((-1, "Set MB MUX address %x, channel %x, %r.\n", MUX_Address, MUX_Channel, Status));
        if(Status)  return Status;
    }
    
    for(RC_ReD_Index = 0; RC_ReD_Index < 12; RC_ReD_Index++){
        SetMem((UINT8*)RC_ReD_Data, sizeof(RC_ReD) * RC_ReD_Data_Max, 0);
        Status = SmcRiserCardRedriverData(SXB_Index, RC_ReD_Index, &RC_MUX_Address,
                        &RC_MUX_Channel, &RC_ReD_Add, RC_ReD_Data, &RC_ReD_Data_Size);
        if(Status) continue;
//switch riser MUX channel
        if(RC_MUX_Address && RC_MUX_Channel){
            SlaveAddress.SmbusDeviceAddress = RC_MUX_Address >> 1;
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            0,      // Mux channel register is 0x00
                            EfiSmbusSendByte,	// EfiSmbusWriteByte,
                            0,
                            &Length,
                            &RC_MUX_Channel);
            DEBUG((-1, "Set riser MUX address %x, channel %x, %r.\n",
                            RC_MUX_Address, RC_MUX_Channel, Status));
            if(Status)  continue;
        }

        SlaveAddress.SmbusDeviceAddress = RC_ReD_Add >> 1;  //Smbus address is better set before the loop
        for(RC_ReD_Data_Index = 0; RC_ReD_Data_Index < RC_ReD_Data_Size; RC_ReD_Data_Index++){
            Status = Smbus->Execute(
                            Smbus,
                            SlaveAddress,
                            RC_ReD_Data[RC_ReD_Data_Index].Reg,
                            EfiSmbusWriteByte,
                            0,
                            &Length,
                            &RC_ReD_Data[RC_ReD_Data_Index].Data);
            DEBUG((-1, "Set Add %x, Reg %x, Data %x, %r.\n", RC_ReD_Add,
                RC_ReD_Data[RC_ReD_Data_Index].Reg, RC_ReD_Data[RC_ReD_Data_Index].Data, Status));
        }
    }
    DEBUG((-1, "SmcRiserCardRedriver end.\n"));
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcRiserCardPeiInit
//
// Description : Get riser card ID from MB GPIO.
//
// Parameters  : 
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
SmcRiserCardPeiInit(
    IN	EFI_FFS_FILE_HEADER	*FfsHeader,
    IN	EFI_PEI_SERVICES	**PeiServices
)
{
    UINT32	SMC_RC_ID = 0, temp_ID;
    UINT32	SXB1_GPIO[] = SXB1_GPIO_Tbl;
    UINT32	SXB2_GPIO[] = SXB2_GPIO_Tbl;
    UINT32	SXB3_GPIO[] = SXB3_GPIO_Tbl;
    EFI_STATUS	Status;
    EFI_PEI_PPI_DESCRIPTOR	*mPpiList = NULL;

    DEBUG((-1, "SmcRiserCardPeiInit start.\n"));

    //get riser card ID from PCH GPIO
#if WIO_SUPPORT || GIO_SUPPORT
    SmcRiserCardIDUpdate(SXB1_GPIO, sizeof(SXB1_GPIO)/sizeof(UINT32), &temp_ID);
    SMC_RC_ID |= temp_ID;
    SmcRiserCardIDUpdate(SXB2_GPIO, sizeof(SXB2_GPIO)/sizeof(UINT32), &temp_ID);
    SMC_RC_ID |= temp_ID << 4;
#endif
#if ULIO_SUPPORT || GIO_SUPPORT
    SmcRiserCardIDUpdate(SXB3_GPIO, sizeof(SXB3_GPIO)/sizeof(UINT32), &temp_ID);
    SMC_RC_ID |= temp_ID << 8;
#endif
    PcdSet32(PcdSmcRiserID, SMC_RC_ID);
    DEBUG((-1, "Full SMC_RC_ID = %x.\n", SMC_RC_ID));

    //search riser card ID in riser card database
#if WIO_SUPPORT
    PcdSet8(PcdSmcSXB1Index, SmcRiserCardIDCmp(SMC_RC_ID & 0x0000000f));
    PcdSet8(PcdSmcSXB2Index, SmcRiserCardIDCmp(SMC_RC_ID & 0x000000f0));
#endif
#if ULIO_SUPPORT
    PcdSet8(PcdSmcSXB3Index, SmcRiserCardIDCmp(SMC_RC_ID & 0x00001f00));
#endif
#if GIO_SUPPORT
    PcdSet8(PcdSmcSXB1Index, SmcRiserCardIDCmp((SMC_RC_ID & 0x00000003) + 0x0000000C));
    PcdSet8(PcdSmcSXB2Index, SmcRiserCardIDCmp((SMC_RC_ID & 0x00000030) + 0x000000C0));
    PcdSet8(PcdSmcSXB3Index, SmcRiserCardIDCmp((SMC_RC_ID & 0x00000300) + 0x00000C00));
#endif
#if FRIO_SUPPORT
    //search FRIO riser card ID.
    PcdSet8(PcdSmcSXB1Index, SmcRiserCardIDCmp(SMC_RC_ID & 0x00000003));
#endif
#if TIO_SUPPORT
    //search TIO riser card ID.
    PcdSet8(PcdSmcSXB1Index, SmcRiserCardIDCmp(SMC_RC_ID & 0x00000003));
#endif
#if FFIO_SUPPORT
    //search FFIO SXB1 riser card ID
    PcdSet8(PcdSmcSXB1Index, SmcRiserCardIDCmp(SMC_RC_ID & 0x00000007));
    //search FFIO SXB2 riser card ID
    PcdSet8(PcdSmcSXB2Index, SmcRiserCardIDCmp(SMC_RC_ID & 0x00000038));
#endif
    
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_PPI_DESCRIPTOR), &mPpiList);

    mPpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    mPpiList->Guid = &gSmcRiserCardPeiProtocolGuid;
    mPpiList->Ppi = &SmcRiserCardPeiProtocol;

    Status = (**PeiServices).InstallPpi(PeiServices, mPpiList);
    if(EFI_ERROR(Status)){
	DEBUG((DEBUG_INFO, "install SmcRiserCardPeiProtocol fail.\n"));
	return EFI_SUCCESS;
    }

    DEBUG((-1, "SmcRiserCardPeiInit end.\n"));
    return	EFI_SUCCESS;
}
