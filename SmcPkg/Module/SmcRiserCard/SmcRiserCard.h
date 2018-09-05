//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//  Rev. 1.07
//    Bug Fix:  Add riser card redriver function.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/10/2017
//
//  Rev. 1.06
//    Bug Fix:  Fix some riser card pcie bifurcation change incorrectly.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Sep/30/2016
//
//  Rev. 1.05
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//  Rev. 1.04
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//  Rev. 1.03
//    Bug Fix:  Support 6 NVMe
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/14/2016
//
//  Rev. 1.02
//      Bug Fixed:  Add riser card name to IIO and OPROM control items.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/21/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//    Rev. 1.00
//    Reason:	Initialize revision.
//    Auditro:	Kasber Chen
//    Date:	04/13/2016
//
//****************************************************************************
#ifndef	_H_SmcRiserCard_
#define	_H_SmcRiserCard_

#include "SuperMDxeDriver.h"
#include "SmcSetupModify.h"
#include <UefiHii.h>

#define EFI_SMC_RISERCARD_PROTOCOL_GUID \
  { 0x7bc8eee1, 0x2f92, 0x466d, 0xbc, 0x5a, 0xa5, 0x0d, 0xa1, 0x2d, 0x1e, 0xd3 }

VOID RCIndexShift(
    IN	UINT32	Board_Pcie_Table[],
    IN OUT	UINT8	*Temp_index
);

VOID RiserCardSmBiosUpdate(void);

EFI_STATUS EFIAPI
SmcRiserCardSetupItem(void);

EFI_STATUS EFIAPI
SmcRiserCardSetupString(void);

EFI_STATUS EFIAPI
SmcRiserCardIioSetup(void);

typedef EFI_STATUS (EFIAPI *RISERCARDBIFURCATE)(IN OUT IIO_CONFIG *SetupData,
		IN UINT8 DefaultIOU0[], IN UINT8 DefaultIOU1[], IN UINT8 DefaultIOU[]);

typedef struct _SMCRISERCARD_PROTOCOL {
    RISERCARDBIFURCATE		SmcRiserCardBifuracate;
} SMCRISERCARD_PROTOCOL;

#endif
