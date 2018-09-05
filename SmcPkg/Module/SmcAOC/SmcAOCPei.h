#ifndef	_H_SmcAOCPei_
#define	_H_SmcAOCPei_

#include <PEI.h>
#include "SmcAOCCommon.h"

#define EFI_SMC_AOC_PEI_PROTOCOL_GUID \
  {0x3b3803d1, 0x4709, 0x45e6, 0x80, 0xf6, 0x43, 0x72, 0x28, 0x63, 0xf4, 0x27}

typedef EFI_STATUS (EFIAPI *PEIAOCPCIEBIFURCATION)(IN OUT IIO_CONFIG *SetupData,
        IN UINT8 DefaultIOU0[], IN UINT8 DefaultIOU1[], IN UINT8 DefaultIOU2[]);

typedef EFI_STATUS (EFIAPI *PEIAOCCARDDETECTION)(IN EFI_PEI_SERVICES **PeiServices, IN UINT8 Channel,
        IN UINTN MUX_Address, IN UINT8 MUX_Channel, IN SMC_SLOT_INFO SLOT_Tbl[], IN UINT8 SLOT_Tbl_Size);

typedef EFI_STATUS (EFIAPI *PEIAOCSETMVNEMODE)(IN EFI_PEI_SERVICES **PeiServices, IN UINT8 Channel,
        IN UINTN MUX_Address, IN UINT8 MUX_Channel, IN SMC_SLOT_INFO SLOT_Tbl[], IN UINT8 SLOT_Tbl_Size);

typedef EFI_STATUS (EFIAPI *PEIAOCPLXDETECTION)();

EFI_STATUS
EFIAPI
SmcAocPeiPlxCallback (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN  VOID                      *Ppi
  );

typedef struct{
    PEIAOCCARDDETECTION		SmcAOCPeiCardDetection;
    PEIAOCPCIEBIFURCATION	SmcAOCPeiBifurcation;
    PEIAOCSETMVNEMODE           SmcAOCPeiSetNVMeMode;
    PEIAOCPLXDETECTION          SmcAOCPeiPlxDetection;
}PEI_SMC_AOC_PROTOCOL;

#endif
