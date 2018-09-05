//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//  Rev. 1.04
//    Bug Fix: Support AOC NVMe card redriver function.
//    Reason:     
//    Auditor: Kasber Chen
//    Date:    Apr/20/2017
//
//  Rev. 1.03
//    Reason:   Add AOC-SLG3 card port number..
//    Auditor:  Kasber Chen
//    Date:     Jan/16/2017
//
//  Rev. 1.02
//    Reason:   Correct port D device and function number.
//    Auditro:  Kasber Chen
//    Date:     Jan/12/2017
//
//  Rev. 1.01
//    Reason:   Remove unused code.
//    Auditro:  Kasber Chen
//    Date:     11/16/2016
//
//  Rev. 1.00
//    Reason:   Initialize revision.
//    Auditro:  Kasber Chen
//    Date:     10/03/2016
//
//****************************************************************************
#ifndef	_H_SmcAOCCommon_
#define	_H_SmcAOCCommon_

#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioRegs.h>
#include <../CpRcPkg/Library/BaseMemoryCoreLib/Chip/Skx/Include/Iio/IioConfig.h>

#ifndef	x16
#define x32     32
#define x24     24
#define x20     20
#define x16     16
#define x8      8
#define x4      4

typedef enum{
    P0PE1A = 0x00110000, P0PE1B = 0x00110008, P0PE1C = 0x00110010, P0PE1D = 0x00110018,
    P0PE2A = 0x00120000, P0PE2B = 0x00120008, P0PE2C = 0x00120010, P0PE2D = 0x00120018,
    P0PE3A = 0x00130000, P0PE3B = 0x00130008, P0PE3C = 0x00130010, P0PE3D = 0x00130018,
    P1PE0A = 0x00200000,
    P1PE1A = 0x00210000, P1PE1B = 0x00210008, P1PE1C = 0x00210010, P1PE1D = 0x00210018,
    P1PE2A = 0x00220000, P1PE2B = 0x00220008, P1PE2C = 0x00220010, P1PE2D = 0x00220018,
    P1PE3A = 0x00230000, P1PE3B = 0x00230008, P1PE3C = 0x00230010, P1PE3D = 0x00230018
}SMC_IIO_PORT;

typedef struct{
    UINT8       Address;
    UINT8       Channel;
    UINT32      BDF;
    UINT8       Bifurcate;
}SMC_SLOT_INFO;
#endif

//typedef enum{   //Pericom
//    CH0 = 0x01, CH1 = 0x02, CH2 = 0x04, CH3 = 0x08,
//    CH4 = 0x10, CH5 = 0x20, CH6 = 0x40, CH7 = 0x80
//}MUX_CH;


typedef struct{
    UINT8       Reg;
    UINT8       Data;
}AOC_ReD;

typedef struct{
    CHAR8       *AOC_NAME;
    UINT8       PortNum;
    UINT8       ReD_Add[4];
    AOC_ReD     *ReD_Data_Tbl[4];
    UINT8       ReD_Data_Tbl_S[4];
}AOC_INFO;

EFI_STATUS EFIAPI
SmcAOCBifurcation(
    IN  OUT     IIO_CONFIG      *SetupData,
    IN          UINT8           DefaultIOU0[],
    IN          UINT8           DefaultIOU1[],
    IN          UINT8           DefaultIOU2[]
);

EFI_STATUS
SearchAOCTbl(
    IN          UINT8   *AOC_Name,
    IN OUT      UINT8   *index,
    IN OUT      UINT8   *PortNum
);

VOID
GetRedriverData(
    IN  UINT8   AOC_Index,
    IN  UINT8   Port_Index,
    IN OUT      AOC_ReD ReD_Data[]
);

#endif
