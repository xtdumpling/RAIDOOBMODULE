#ifndef	_H_SmcRiserCardPei_
#define	_H_SmcRiserCardPei_

#include <PEI.h>
#include "SmcRiserCardCommon.h"

#define EFI_SMCRISERCARD_PEI_DRIVER_PROTOCOL_GUID \
  {0x799b5f79, 0xf0e1, 0x4354, 0x89, 0x27, 0x27, 0x7f, 0x71, 0xd0, 0xd9, 0x26}

typedef EFI_STATUS (EFIAPI *PEIRISERPCIEBIFURCATION)(IN OUT IIO_CONFIG *SetupData,
        IN UINT8 DefaultIOU0[], IN UINT8 DefaultIOU1[], IN UINT8 DefaultIOU2[]);

typedef EFI_STATUS (EFIAPI *PEIRISERSLOTBDFTABLE)(IN OUT SMC_SLOT_INFO SLOTBDFTbl1[],
        IN OUT SMC_SLOT_INFO SLOTBDFTbl2[], IN OUT SMC_SLOT_INFO SLOTBDFTbl3[]);

typedef EFI_STATUS (EFIAPI *PEIRISERREDRIVER)(IN EFI_PEI_SERVICES **PeiServices,
        IN UINT8 Channel, IN UINTN MUX_Address, IN UINT8 MUX_Channel, IN UINT8 SXB_Index);

typedef struct{
    PEIRISERPCIEBIFURCATION	PeiSmcRiserCardPcieBifurcation;
    PEIRISERSLOTBDFTABLE        PeiSmcRiserCardSlotBDFTable;
    PEIRISERREDRIVER            PeiSmcRiserCardRedriver;
}PEI_SMCRISERCARD_PROTOCOL;

EFI_STATUS EFIAPI
SmcRiserCardRedriver(
    IN  EFI_PEI_SERVICES        **PeiServices,
    IN  UINT8   Channel,
    IN  UINTN   MUX_Address,
    IN  UINT8   MUX_Channel,
    IN  UINT8   SXB_Index
);
#endif
