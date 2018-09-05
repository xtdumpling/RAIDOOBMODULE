//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix: Remove unused redriver settings.
//    Reason:     
//    Auditor: Kasber Chen
//    Date:    May/22/2017
//
//  Rev. 1.02
//    Bug Fix: Support AOC NVMe card redriver function.
//    Reason:     
//    Auditor: Kasber Chen
//    Date:    Apr/20/2017
//
//  Rev. 1.01
//    Reason:   Add AOC-SLG3 card port number..
//    Auditor:  Kasber Chen
//    Date:     Jan/16/2017
//
//  Rev. 1.00
//    Reason:   Initialize revision.
//    Auditor:  Kasber Chen
//    Date:     10/03/2016
//
//****************************************************************************
#ifndef	_H_SmcAOCDB_
#define	_H_SmcAOCDB_

AOC_ReD SLG3_2E4R_1[] = AOC_SLG3_2E4R;
AOC_ReD SLG3_2E4R_2[] = AOC_SLG3_2E4R;
AOC_ReD SLG3_4E4R_1[] = AOC_SLG3_4E4R;
AOC_ReD SLG3_4E4R_2[] = AOC_SLG3_4E4R;
AOC_ReD SLG3_4E4R_3[] = AOC_SLG3_4E4R;
AOC_ReD SLG3_4E4R_4[] = AOC_SLG3_4E4R;

AOC_INFO AOC_TBL[] = {
{
    "",
    0,
    {0xff, 0xff, 0xff, 0xff},
    {NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0}
},
{
    "AOC-SLG3-2E4R",
    2,
    {0xb0, 0xb2, 0, 0},
    {SLG3_2E4R_1, SLG3_2E4R_2, NULL, NULL},
    {sizeof(SLG3_2E4R_1), sizeof(SLG3_2E4R_2), 0, 0}
},
{
    "AOC-SLG3-4E4R",
    4,
    {0xb0, 0xb2, 0xb4, 0xb6},
    {SLG3_4E4R_1, SLG3_4E4R_2, SLG3_4E4R_3, SLG3_4E4R_4},
    {sizeof(SLG3_4E4R_1), sizeof(SLG3_4E4R_2), sizeof(SLG3_4E4R_3), sizeof(SLG3_4E4R_4)}
},
{
    "AOC-SLG3-2E4T",
    2,
    {0, 0, 0, 0},
    {NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0}
},
{
    "AOC-SLG3-4E4T",
    4,
    {0, 0, 0, 0},
    {NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0}
},
{
    "",
    0,
    {0xff, 0xff, 0xff, 0xff},
    {NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0}
}
};

#endif
