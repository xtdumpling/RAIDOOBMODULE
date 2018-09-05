//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//  Rev. 1.19
//    Bug Fix:  Add RSC-U2M2R-8-NI22 and rename AOC-UR6-i4XTF to AOC-UR-i4XTF.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/17/2017
//
//  Rev. 1.18
//    Bug Fix:  Add AOC-URN4-i2TS into data base.
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     July/14/2017
//
//  Rev. 1.17
//    Bug Fix:  Fix AOC-2UR68-m2TS i2c channel incorrect.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     June/22/2017
//
//  Rev. 1.16
//    Bug Fix:  Add RSC_UN4_88 redriver & i2c mux information.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     June/14/2017
//
//  Rev. 1.15
//    Bug Fix:  [X11SPG]Fixed could not show CPU SLOT3 setup item when directly plugging a PCIe card in this slot, not from riser card. 
//    Reason:   
//    Auditor:  Chen Lin
//    Date:     June/12/2017
//
//
//  Rev. 1.14
//    Bug Fix:  Remove AOC-URN6-i2XT MUX settings.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/22/2017
//
//  Rev. 1.13
//    Bug Fix:  Fix RSC-U2N4-6 redriver is not set correctly, new 24NVMe config
//    Reason:   The driver chips use I2C ch 2, instead of ch 3
//    Auditor:  Donald Han
//    Date:     May/05/2017
//
//  Rev. 1.12
//    Bug Fix:  Fix AOC-2UR6N4-i4XT redriver is not set correctly
//    Reason:   The driver chips use I2C ch 0, instead of ch 2
//    Auditor:  Donald Han
//    Date:     May/04/2017
//
//  Rev. 1.11
//    Bug Fix:  Support default string for SPG.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Apr/18/2017
//
//  Rev. 1.10
//    Bug Fix:  Fixed could not show correct riser card for RSC-R1U-E16R.
//    Reason:   
//    Auditor:  Chen Lin
//    Date:     Apr/12/2017
//
//
//  Rev. 1.09
//    Bug Fix:  Remove MUX settings in AOC-URN4-m2TS
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Mar/31/2017
//
//  Rev. 1.08
//    Bug Fix:  Fill lost LAN string in AOC-UR-i4XT(R1.01)
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Mar/23/2017
//
//  Rev. 1.07
//    Bug Fix:  Revise 24NVMe riser settings
//    Reason:   
//    Auditor:  Donald Han
//    Date:     Feb/17/2017
//
//  Rev. 1.06
//    Bug Fix:  Add riser card AOC-URN4-m2TS support.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/25/2017
//
//  Rev. 1.05
//    Reason:	Support OEM type 130 for SXB slots.
//    Auditro:	Kasber Chen
//    Date:      Jan/25/2017
//
//  Rev. 1.04
//    Bug Fix:  Add riser card redriver function.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/10/2017
//
//  Rev. 1.03
//    Reason:	Correct AOC-2UR6N4-i4XT settings.
//    Auditor:	Kasber Chen
//    Date:     Jan/03/2017
//
//  Rev. 1.02
//    Reason:	Add AOC-2UR68-m2TS.
//    Auditor:	Kasber Chen
//    Date:     Dec/15/2016
//
//  Rev. 1.01
//    Reason:	Add AOC-UR6-i4XTF and AOC-2UR66-I4XTF.
//    Auditor:	Kasber Chen
//    Date:     Nov/29/2016
//
//  Rev. 1.00
//    Reason:	Initialize revision.
//    Auditor:	Kasber Chen
//    Date:     4/13/2016
//
//****************************************************************************
#ifndef	_H_SmcRiserCardDB_
#define	_H_SmcRiserCardDB_

RC_ReD	ReD_2URN4_i4XT_1[] = RC_2URN4_i4XT_1;
RC_ReD	ReD_2URN4_i4XT_2[] = RC_2URN4_i4XT_2;
RC_ReD	ReD_2URN4_i4XT_3[] = RC_2URN4_i4XT_3;
RC_ReD	ReD_2URN4_i4XT_4[] = RC_2URN4_i4XT_4;
RC_ReD  ReD_2UR8N4_i2XT_1[] = RC_2UR8N4_i2XT_1;
RC_ReD  ReD_2UR8N4_i2XT_2[] = RC_2UR8N4_i2XT_2;
RC_ReD  ReD_2UR8N4_i2XT_3[] = RC_2UR8N4_i2XT_3;
RC_ReD  ReD_2UR8N4_i2XT_4[] = RC_2UR8N4_i2XT_4;
RC_ReD  ReD_2UR6N4_i4XT_1[] = RC_2UR6N4_i4XT_1;
RC_ReD  ReD_2UR6N4_i4XT_2[] = RC_2UR6N4_i4XT_2;
RC_ReD  ReD_2UR6N4_i4XT_3[] = RC_2UR6N4_i4XT_3;
RC_ReD  ReD_2UR6N4_i4XT_4[] = RC_2UR6N4_i4XT_4;
RC_ReD  ReD_U2N4_1[] = RC_U2N4_1;
RC_ReD  ReD_U2N4_2[] = RC_U2N4_2;
RC_ReD  ReD_U2N4_3[] = RC_U2N4_3;
RC_ReD  ReD_U2N4_4[] = RC_U2N4_4;
RC_ReD  ReD_URN6_i2XT_1[] = RC_URN6_i2XT_1;
RC_ReD  ReD_URN6_i2XT_2[] = RC_URN6_i2XT_2;
RC_ReD  ReD_URN6_i2XT_3[] = RC_URN6_i2XT_3;
RC_ReD  ReD_URN6_i2XT_4[] = RC_URN6_i2XT_4;
RC_ReD  ReD_URN6_i2XT_5[] = RC_URN6_i2XT_5;
RC_ReD  ReD_URN6_i2XT_6[] = RC_URN6_i2XT_6;

RC_ReD  ReD_UN4_88_1[] = RSC_UN4_88_1;
RC_ReD  ReD_UN4_88_2[] = RSC_UN4_88_2;
RC_ReD  ReD_UN4_88_3[] = RSC_UN4_88_3;
RC_ReD  ReD_UN4_88_4[] = RSC_UN4_88_4;
	

SMC_IIO_PORT_INDEX      IIO_Port_Index[] = {
		{P0PE1A, 1}, {P0PE1B, 2}, {P0PE1C, 3}, {P0PE1D, 4},
		{P0PE2A, 5}, {P0PE2B, 6}, {P0PE2C, 7}, {P0PE2D, 8},
		{P0PE3A, 9}, {P0PE3B, 10}, {P0PE3C, 11}, {P0PE3D, 12},
		{P1PE0A, 21},
		{P1PE1A, 22}, {P1PE1B, 23}, {P1PE1C, 24}, {P1PE1D, 25},
		{P1PE2A, 26}, {P1PE2B, 27}, {P1PE2C, 28}, {P1PE2D, 29},
		{P1PE3A, 30}, {P1PE3B, 31}, {P1PE3C, 32}, {P1PE3D, 33}};

RC_DATA_Table RC_Table[] = {
{
    {0xffffffff, L"", ""},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#if WIO_SUPPORT
//RSC-R2UW-4E8
{
    {0x00000001, L"RSC-R2UW-4E8", "RSC-R2UW-4E8"},
    {  x8,    0,   x8,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {0x02,    0, 0x01,    0, 0x03,    0, 0x04,    0, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,   x8,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X8", "SLOT3 PCI-E X8", "SLOT4 PCI-E X8"},
    {"", "", "", ""},
    {"", ""},
    {0x05,    0, 0x04,    0, 0x06,    0, 0x07,    0,    0,    0,    0,    0},
    {0xE0,    0, 0xE0,    0, 0xE0,    0, 0xE0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-UN4-88
{
    {0x00000004, L"RSC-UN4-88", "RSC-UN4-88"},
    {  x4,   x4,   x4,   x4,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {0x34, 0x33, 0x32, 0x31, 0x02,    0, 0x01,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X8", "", ""},
    {"", "", "", ""},
    {"", ""},
    {0x06, 0x06, 0x06, 0x06, 0x05,    0, 0x04,    0,    0,    0,    0,    0},
    {0xE0, 0xE0, 0xE0, 0xE0, 0xE0,    0, 0xE0,    0,    0,    0,    0,    0},
    {0xB6, 0xB4, 0xB2, 0xB0,    0,    0,    0,    0,    0,    0,    0,    0},
    {ReD_UN4_88_4, ReD_UN4_88_3, ReD_UN4_88_2, ReD_UN4_88_1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {sizeof(ReD_UN4_88_4), sizeof(ReD_UN4_88_3), sizeof(ReD_UN4_88_2), sizeof(ReD_UN4_88_1), 0, 0, 0, 0, 0, 0, 0, 0}
},
//RSC-R1UW-2E8E16+
{
    {0x00000005, L"RSC-R2UW-2E8E16+", "RSC-R2UW-2E8E16+"},
    { x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0x02,    0, 0x03,    0, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,  x16,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "SLOT2 PCI-E X8(IN X16)", "SLOT3 PCI-E X8", ""},
    {"", "", "", ""},
    {"", ""},
    {0x04,    0,    0,    0, 0x05,    0, 0x06,    0,    0,    0,    0,    0},
    {0xE0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-W2-66
{
    {0x00000006, L"RSC-W2-66", "RSC-W2-66"},
    { x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0x02,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "SLOT2 PCI-E X16", "", ""},
    {"", "", "", ""},
    {"", ""},
    {0x04,    0,    0,    0, 0x05,    0,    0,    0,    0,    0,    0,    0},
    {0xE0,    0,    0,    0, 0xE0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UW-2E16
{
    {0x00000007, L"RSC-R1UW-2E16", "RSC-R1UW-2E16"},
    { x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0x02,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "SLOT2 PCI-E X16", "", ""},
    {"", "", "", ""},
    {"", ""},
    {0x04,    0,    0,    0, 0x05,    0,    0,    0,    0,    0,    0,    0},
    {0xE0,    0,    0,    0, 0xE0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-U2N4-6
{
    {0x00000008, L"RSC-U2N4-6", "RSC-U2N4-6"},
//KKK    {  x4,   x4,   x4,   x4,  x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
//KKK    {0x34, 0x33, 0x32, 0x31, 0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {0x34,    0,    0,    0, 0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {0x06, 0x06, 0x06, 0x06, 0x04,    0,    0,    0,    0,    0,    0,    0},
    {0xE0, 0xE0, 0xE0, 0xE0, 0xE0,    0,    0,    0,    0,    0,    0,    0},
    {0xB6, 0xB4, 0xB2, 0xB0,    0,    0,    0,    0,    0,    0,    0,    0},
    {ReD_U2N4_4, ReD_U2N4_3, ReD_U2N4_2, ReD_U2N4_1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {sizeof(ReD_U2N4_4), sizeof(ReD_U2N4_3), sizeof(ReD_U2N4_2), sizeof(ReD_U2N4_1), 0, 0, 0, 0, 0, 0, 0, 0}
},
//RSC-W-88
{
    {0x0000000B, L"RSC-W-88", "RSC-W-88"},
    { x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0, 0x02,    0, 0x01,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X8", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0, 0x06,    0, 0x05,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC_R2UW_U2E8
{
    {0x0000000C, L"RSC-R2UW-U2E8", "RSC-R2UW-U2E8"},
    {  x8,    0,   x8,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {0x02,    0, 0x01,    0, 0x03,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,   x8,    0,   x8,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X8", "SLOT3 PCI-E X8", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R2UW-2E8E16
{
    {0x0000000D, L"RSC-R2UW-2E8E16", "RSC-R2UW-2E8E16"},
    { x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0x03,    0, 0x02,    0, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "SLOT2 PCI-E X8", "SLOT3 PCI-E X8", ""},
    {"", "", "", ""},
    {"", ""},
    {0x04,    0,    0,    0, 0x05,    0, 0x06,    0,    0,    0,    0,    0},
    {0xE0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UW-U
{
    {0x0000000E, L"RSC-R1UW-U", "RSC-R1UW-U"},
    { x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0, 0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,    0,    0, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-U2M2R-8-NI22
{
    {0x00000010, L"RSC-U2M2R-8-NI22", "RSC-U2M2R-8-NI22"},
    {   x8,   0,   x4,   x4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { 0x01,   0, 0x51, 0x52, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {   x8,   0,   x4,   x4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"M.2_P", "M.2_C", "", ""},
    {"", ""},
    {    0,   0, 0x06, 0x07,    0,    0,    0,    0,    0,    0,    0,    0},
    {    0,   0, 0xE0, 0xE0,    0,    0,    0,    0,    0,    0,    0,    0},
    {    0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-UMR-8
{
    {0x00000030, L"RSC-UMR-8", "RSC-UMR-8"},
    {   x8,   0,   x4,   x4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { 0x01,   0, 0x51,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {   x8,   0,   x4,   x4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"M.2_PCIe", "", "", ""},
    {"", ""},
    {    0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {    0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {    0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UW-E8R(R1.10)
{
    {0x00000040, L"RSC-R1UW-E8R", "RSC-R1UW-E8R"},
    {   x8,   0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { 0x01,   0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {   x8,   0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {    0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {    0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {    0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R2UW-2E4R   
{
    {0x00000050, L"RSC-R2UW-2E4R", "RSC-R2UW-2E4R"},
    {  x4,   x4,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01, 0x02,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {  x8,   x8,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X4(IN X8)", "SLOT2 PCI-E X4(IN X8)", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#if Single_CPU_Platform
//RSC-R2UW-E8R-UP
{
    {0x00000060, L"RSC-R2UW-E8R-UP", "RSC-R2UW-E8R-UP"},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#else
//RSC-R2UW-2E8R
{
    {0x00000060, L"RSC-R2UW-2E8R", "RSC-R2UW-2E8R"},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0, 0x02,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X8", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#endif //Single_CPU_Platform
#endif
#if ULIO_SUPPORT
//AOC-UR-i2XT(R1.01)
{
    {0x00000000, L"AOC-UR-i2XT", "AOC-UR-i2XT"},
    {  x8,    0,  x24,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0,    0,    0,    0,    0,    0,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,  x16,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8(IN X16)", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-UR-i4XT(R1.01)
{
    {0x00000000, L"AOC-UR-i4XT", "AOC-UR-i4XT"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0,    0,    0,    0,    0, 0x12,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", "Intel Ethernet X540"},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR68-i2XT(R1.01)
{
    {0x00000100, L"AOC-2UR68-i2XT", "AOC-2UR68-i2XT"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x02,    0,    0,    0, 0x01,    0, 0x03,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,  x16,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8(IN X16)", "SLOT2 PCI-E 3.0 X16", "SLOT3 PCI-E 3.0 X8", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", ""},
    {   0,    0, 0x06,    0,    0,    0, 0x05,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR6-i4XT(R1.01)	
{
    {0x00000100, L"AOC-2UR6-i4XT", "AOC-2UR6-i4XT"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x02,    0,    0,    0, 0x12,    0, 0x03,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {"", "SLOT2 PCI-E 3.0 X16", "SLOT3 PCI-E 3.0 X8", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", "Intel Ethernet X540"},
    {   0,    0, 0x06,    0,    0,    0,    0,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0,    0,    0,    0,    0,    0, 0xE0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-UR-i4G(R1.02)
{
    {0x00000200, L"AOC-UR-i4G", "AOC-UR-i4G"},
    {  x4,  x20,    0,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x14,    0,    0,    0,    0,    0, 0x01,    0,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,   x8,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet I350", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR68-i4G
{
    {0x00000300, L"AOC-2UR68-i4G", "AOC-2UR68-i4G"},
    {  x4,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x14,    0, 0x02,    0,    0,    0, 0x03,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,   x8,    0,  x16,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8(IN X16)", "SLOT2 PCI-E 3.0 X16", "SLOT3 PCI-E 3.0 X8", ""},
    {"", "", "", ""},
    {"Intel Ethernet I350", ""},
    {   0,    0, 0x06,    0,    0,    0, 0x07,    0, 0x04,    0,    0,    0},
    {   0,    0, 0xE0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-URN2-i2XT(R1.00)
{
    {0x00000400, L"AOC-URN2-i2XT", "AOC-URN2-i2XT"},
    {  x8,    0,   x8,    0,   x8,    0,   x4,   x4,   x8,    0, 0xff, 0xff},
    {0x12,    0,    0,    0, 0x01,    0, 0x31, 0x32,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-URN2-i4XT(R1.00)	
{
    {0x00000400, L"AOC-URN2-i4XT", "AOC-URN2-i4XT"},
    {  x8,    0,   x8,    0,   x8,    0,   x4,   x4,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x12,    0, 0x01,    0, 0x31, 0x32,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", "Intel Ethernet X540"},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR8N4-i2XT(R1.00)
{
    {0x00000500, L"AOC-2UR8N4-i2XT", "AOC-2UR8N4-i2XT"},
    {  x8,    0,   x4,   x4,   x4,   x4,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x31, 0x32, 0x33, 0x34, 0x01,    0, 0x02,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,  x16,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8(IN X16)", "SLOT2 PCI-E 3.0 X8", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", ""},
    {   0,    0, 0x04, 0x04, 0x04, 0x04, 0x05,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,    0, 0xE0,    0,    0,    0},
    {   0,    0, 0xB0, 0xB2, 0xB4, 0xB6,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, ReD_2UR8N4_i2XT_1, ReD_2UR8N4_i2XT_2, ReD_2UR8N4_i2XT_3, ReD_2UR8N4_i2XT_4, NULL, NULL, NULL, NULL, NULL, NULL},
    {0, 0, sizeof(ReD_2UR8N4_i2XT_1), sizeof(ReD_2UR8N4_i2XT_2), sizeof(ReD_2UR8N4_i2XT_3), sizeof(ReD_2UR8N4_i2XT_4), 0, 0, 0, 0, 0, 0}
},
//AOC-2URN4-i4XT(R1.00)	
{
    {0x00000500, L"AOC-2URN4-i4XT", "AOC-2URN4-i4XT"},
    {  x8,    0,   x4,   x4,   x4,   x4,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x31, 0x32, 0x33, 0x34, 0x12,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", "Intel Ethernet X540"},
    {   0,    0, 0x04, 0x04, 0x04, 0x04,    0,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0, 0xE0, 0xE0, 0xE0,    0,    0, 0xE0,    0,    0,    0},
    {   0,    0, 0xB0, 0xB2, 0xB4, 0xB6,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, ReD_2URN4_i4XT_1, ReD_2URN4_i4XT_2, ReD_2URN4_i4XT_3, ReD_2URN4_i4XT_4, NULL, NULL, NULL, NULL, NULL, NULL},
    {0, 0, sizeof(ReD_2URN4_i4XT_1), sizeof(ReD_2URN4_i4XT_2), sizeof(ReD_2URN4_i4XT_3), sizeof(ReD_2URN4_i4XT_4), 0, 0, 0, 0, 0, 0}
},
//AOC-URN2-i2XS
{
    {0x00000600, L"AOC-URN2-i2XS", "AOC-URN2-i2XS"},
    {  x8,    0,   x8,    0,   x8,    0,   x4,   x4,   x8,    0, 0xff, 0xff},
    {0x12,    0,    0,    0, 0x01,    0, 0x31, 0x32,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet 82599", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR68-i2XS
{
    {0x00000700, L"AOC-2UR68-i2XS", "AOC-2UR68-i2XS"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x02,    0,    0,    0, 0x01,    0, 0x03,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "SLOT2 PCI-E 3.0 X16", "SLOT3 PCI-E 3.0 X8", ""},
    {"", "", "", ""},
    {"Intel Ethernet 82599", ""},
    {   0,    0, 0x06,    0,    0,    0, 0x05,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR66-i4G(R1.00)
{
    {0x00000800, L"AOC-2UR66-i4G", "AOC-2UR66-i4G"},
    {  x8,    0,  x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff},
    {0x14,    0, 0x02,    0,    0,    0, 0x01,    0,    0,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X16", "SLOT2 PCI-E 3.0 X16", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet I350", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-URN6-i2XT
{
    {0x00000900, L"AOC-URN6-i2XT", "AOC-URN6-i2XT"},
    {  x8,    0,   x4,   x4,   x4,   x4,   x4,   x4,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x33, 0x34, 0x35, 0x36, 0x31, 0x32,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0, 0xB4, 0xB6, 0xB8, 0xBA, 0xB0, 0xB2,    0,    0,    0,    0},
    {NULL, NULL, ReD_URN6_i2XT_1, ReD_URN6_i2XT_2, ReD_URN6_i2XT_3, ReD_URN6_i2XT_4, ReD_URN6_i2XT_5, ReD_URN6_i2XT_6, NULL, NULL, NULL, NULL},
    {0, 0, sizeof(ReD_URN6_i2XT_1), sizeof(ReD_URN6_i2XT_2), sizeof(ReD_URN6_i2XT_3), sizeof(ReD_URN6_i2XT_4), sizeof(ReD_URN6_i2XT_5), sizeof(ReD_URN6_i2XT_6), 0, 0, 0, 0}
},
//AOC-URN2-i4GXS
{
    {0x00000a00, L"AOC-URN2-i4GXS", "AOC-URN2-i4GXS"},
    {  x4,   x4,  x16,    0,    0,    0,   x8,    0,   x4,   x4, 0xff, 0xff},
    {0x12, 0x12,    0,    0,    0,    0, 0x01,    0, 0x31, 0x32, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,   x8,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X710", "Intel Ethernet I350"},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0} 
},
//AOC-UR-i4GXS
{
    {0x00000a00, L"AOC-UR-i4GXS", "AOC-UR-i4GXS"},
    {  x4,   x4,  x16,    0,    0,    0,   x8,    0,   x4,   x4, 0xff, 0xff},
    {0x12, 0x12,    0,    0,    0,    0, 0x01,    0, 0x31, 0x32, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,   x8,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X710", "Intel Ethernet I350"},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0} 
},
//AOC-2UR6N4-i4XT(R1.00)
{
    {0x00000B00, L"AOC-2UR6N4-i4XT", "AOC-2UR6N4-i4XT"},
//KKK    {  x4,   x4,   x4,   x4,   x4,   x4,  x16,    0,    0,    0, 0xff, 0xff},
//KKK    {0x12, 0x12, 0x31, 0x32, 0x33, 0x34, 0x01,    0,    0,    0, 0xff, 0xff},
    {  x4,   x4,  x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff},
    {0x12, 0x12, 0x31,    0,    0,    0, 0x01,    0,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X16", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X550", "Intel Ethernet X550"},
    {   0,    0, 0x04, 0x04, 0x04, 0x04, 0x05,    0,    0,    0,    0,    0},
    {   0,    0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,    0,    0,    0,    0,    0},
    {   0,    0, 0xB0, 0xB2, 0xB4, 0xB6,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, ReD_2UR6N4_i4XT_1, ReD_2UR6N4_i4XT_2, ReD_2UR6N4_i4XT_3, ReD_2UR6N4_i4XT_4, NULL, NULL, NULL, NULL, NULL, NULL},
    {0, 0, sizeof(ReD_2UR6N4_i4XT_1), sizeof(ReD_2UR6N4_i4XT_2), sizeof(ReD_2UR6N4_i4XT_3), sizeof(ReD_2UR6N4_i4XT_4), 0, 0, 0, 0, 0, 0}
},
//AOC-2UR6N4-IN001
{
    {0x00000B00, L"AOC-2UR6N4-IN001", "AOC-2UR6N4-IN001"},
//KKK    {  x8,    0,   x4,   x4,   x4,   x4,  x16,    0,    0,    0, 0xff, 0xff},
//KKK    {   0,    0, 0x31, 0x32, 0x33, 0x34, 0x01,    0,    0,    0, 0xff, 0xff},
    {  x4,   x4,  x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff},
    {   0,    0, 0x31,    0,    0,    0, 0x01,    0,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0, 0x04, 0x04, 0x04, 0x04, 0x05,    0,    0,    0,    0,    0},
    {   0,    0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,    0,    0,    0,    0,    0},
    {   0,    0, 0xB0, 0xB2, 0xB4, 0xB6,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, ReD_2UR6N4_i4XT_1, ReD_2UR6N4_i4XT_2, ReD_2UR6N4_i4XT_3, ReD_2UR6N4_i4XT_4, NULL, NULL, NULL, NULL, NULL, NULL},
    {0, 0, sizeof(ReD_2UR6N4_i4XT_1), sizeof(ReD_2UR6N4_i4XT_2), sizeof(ReD_2UR6N4_i4XT_3), sizeof(ReD_2UR6N4_i4XT_4), 0, 0, 0, 0, 0, 0}
},
//AOC-URN2-i2XT(R1.01)
{
    {0x00000c00, L"AOC-URN2-i2XT", "AOC-URN2-i2XT"},
    {  x8,    0,   x8,    0,   x8,    0,   x4,   x4,   x8,    0, 0xff, 0xff},
    {0x12,    0,    0,    0, 0x01,    0, 0x31, 0x32,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-URN2-i4XT(R1.01)	
{
    {0x00000c00, L"AOC-URN2-i4XT", "AOC-URN2-i4XT"},
    {  x8,    0,   x8,    0,   x8,    0,   x4,   x4,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x12,    0, 0x01,    0, 0x31, 0x32,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,   x8,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", "Intel Ethernet X540"},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR8N4-i2XT(R1.00) for 401A
{
    {0x00000D00, L"AOC-2UR8N4-i2XT", "AOC-2UR8N4-i2XT"},
    {  x8,    0,   x4,   x4,   x4,   x4,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x31, 0x32, 0x33, 0x34, 0x01,    0, 0x02,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,  x16,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8(IN X16)", "SLOT2 PCI-E 3.0 X8", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", ""},
    {   0,    0, 0x04, 0x04, 0x04, 0x04, 0x05,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,    0, 0xE0,    0,    0,    0},
    {   0,    0, 0xB0, 0xB2, 0xB4, 0xB6,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, ReD_2UR8N4_i2XT_1, ReD_2UR8N4_i2XT_2, ReD_2UR8N4_i2XT_3, ReD_2UR8N4_i2XT_4, NULL, NULL, NULL, NULL, NULL, NULL},
    {0, 0, sizeof(ReD_2UR8N4_i2XT_1), sizeof(ReD_2UR8N4_i2XT_2), sizeof(ReD_2UR8N4_i2XT_3), sizeof(ReD_2UR8N4_i2XT_4), 0, 0, 0, 0, 0, 0}
},
//AOC-2URN4-i4XT(R1.00) for 401A
{
    {0x00000D00, L"AOC-2URN4-i4XT", "AOC-2URN4-i4XT"},
    {  x8,    0,   x4,   x4,   x4,   x4,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x31, 0x32, 0x33, 0x34, 0x12,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", "Intel Ethernet X540"},
    {   0,    0, 0x04, 0x04, 0x04, 0x04,    0,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0, 0xE0, 0xE0, 0xE0,    0,    0, 0xE0,    0,    0,    0},
    {   0,    0, 0xB0, 0xB2, 0xB4, 0xB6,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, ReD_2URN4_i4XT_1, ReD_2URN4_i4XT_2, ReD_2URN4_i4XT_3, ReD_2URN4_i4XT_4, NULL, NULL, NULL, NULL, NULL, NULL},
    {0, 0, sizeof(ReD_2URN4_i4XT_1), sizeof(ReD_2URN4_i4XT_2), sizeof(ReD_2URN4_i4XT_3), sizeof(ReD_2URN4_i4XT_4), 0, 0, 0, 0, 0, 0}
},
//AOC-2URN4-i4XT(R2.00) for IDT retimer
{
    {0x00000E00, L"AOC-2URN4-i4XT", "AOC-2URN4-i4XT"},
    {  x8,    0,   x4,   x4,   x4,   x4,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x31, 0x32, 0x33, 0x34, 0x12,    0,    0,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X540", "Intel Ethernet X540"},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
},
//AOC-UR-i2XT+, X550
{
    {0x00001000, L"AOC-UR-i2XT+", "AOC-UR-i2XT+"},
    {  x8,    0,  x24,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0,    0,    0,    0,    0,    0,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,  x16,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8(IN X16)", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X550", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-UR-i4XT+, X550
{
    {0x00001000, L"AOC-UR-i4XT+", "AOC-UR-i4XT+"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0,    0,    0,    0,    0, 0x12,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,    0,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet X550", "Intel Ethernet X550"},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR68-i2XT+, X550
{
    {0x0001100, L"AOC-2UR68-i2XT+", "AOC-2UR68-i2XT+"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x02,    0,    0,    0, 0x01,    0, 0x03,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,  x16,    0,   x8,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8(IN X16)", "SLOT2 PCI-E 3.0 X16", "SLOT3 PCI-E 3.0 X8", ""},
    {"", "", "", ""},
    {"Intel Ethernet X550", ""},
    {   0,    0, 0x06,    0,    0,    0, 0x05,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR6-i4XT+, X550	
{
    {0x0001100, L"AOC-2UR6-i4XT+", "AOC-2UR6-i4XT+"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x02,    0,    0,    0, 0x12,    0, 0x03,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,    0,    0,   x8,    0, 0xff, 0xff},
    {"", "SLOT2 PCI-E 3.0 X16", "SLOT3 PCI-E 3.0 X16", ""},
    {"", "", "", ""},
    {"Intel Ethernet X550", "Intel Ethernet X550"},
    {   0,    0, 0x06,    0,    0,    0,    0,    0, 0x07,    0,    0,    0},
    {   0,    0, 0xE0,    0,    0,    0,    0,    0, 0xE0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-UR-i4XTF
{
    {0x0001200, L"AOC-UR-i4XTF", "AOC-UR-i4XTF"},
    {  x8,    0,   x8,    0,  x24,    0,    0,    0,    0,    0, 0xff, 0xff},
    {0x14,    0, 0x01,    0,    0,    0,    0,    0,    0,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet XL710", ""},
    {0x04,    0, 0x05,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {0xE0,    0, 0xE0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR66-I4XTF
{
    {0x0001300, L"AOC-2UR66-I4XTF", "AOC-2UR66-I4XTF"},
    {  x8,    0,  x16,    0,    0,    0,  x16,    0,    0,    0, 0xff, 0xff},
    {0x14,    0, 0x01,    0,    0,    0, 0x02,    0,    0,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X16", "SLOT2 PCI-E 3.0 X16", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet XL710", ""},
    {0x04,    0, 0x05,    0,    0,    0, 0x06,    0,    0,    0,    0,    0},
    {0xE0,    0, 0xE0,    0,    0,    0, 0xE0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-2UR68-m2TS
{
    {0x0001500, L"AOC-2UR68-m2TS", "AOC-2UR68-m2TS"},
    {  x8,    0,  x16,    0,    0,    0,   x8,    0,   x8,    0, 0xff, 0xff},
    {0x12,    0, 0x02,    0,    0,    0, 0x03,    0, 0x01,    0, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,   x8,    0,  x16,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "SLOT2 PCI-E 3.0 X16", "SLOT3 PCI-E 3.0 X8", ""},
    {"", "", "", ""},
    {"Mellanox Ethernet CX4LX", ""},
    {   0,    0, 0x06,    0,    0,    0, 0x07,    0, 0x05,    0,    0,    0},
    {   0,    0, 0xE0,    0,    0,    0, 0xE0,    0, 0xE0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-URN4-m2TS
{
    {0x0001600, L"AOC-URN4-m2TS", "AOC-URN4-m2TS"},
    {  x8,    0,  x16,    0,    0,    0,   x4,   x4,   x4,   x4, 0xff, 0xff},
    {0x12,    0, 0x01,    0,    0,    0, 0x31, 0x32, 0x33, 0x34, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X16", "", "", ""},
    {"", "", "", ""},
    {"Mellanox Ethernet CX4LX", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//AOC-URN4-i2TS
{
    {0x0001800, L"AOC-URN4-i2TS", "AOC-URN4-i2TS"},
    {  x8,    0,  x16,    0,    0,    0,   x4,   x4,   x4,   x4, 0xff, 0xff},
    {0x12,    0, 0x01,    0,    0,    0, 0x31, 0x32, 0x33, 0x34, 0xff, 0xff},
    {   0,    0,  x16,    0,    0,    0,    0,    0,    0,    0, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X16", "", "", ""},
    {"", "", "", ""},
    {"Intel Ethernet XXV710", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#endif
#if GIO_SUPPORT
//RSC-R1U-E16R
{
    {0x00000000, L"RSC-R1U-E16R", "RSC-R1U-E16R"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"PCI-E 3.0 X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UG-2E8G-UP -> ID 0x00
{//Add 0x000C to make sure ID is independent.
    {0x0000000C, L"RSC-R1UG-2E8G-UP", "RSC-R1UG-2E8G-UP"},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0, 0x02,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "SLOT2 PCI-E 3.0 X8", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UG-E16-UP -> ID 0x02
{//Add 0x000C to make sure ID is independent.
    {0x0000000E, L"RSC-R1UG-E16-UP", "RSC-R1UG-E16-UP"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"PCI-E X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UG-2E8GR-UP -> ID 0x00
{//Add 0x00C0 to make sure ID is independent.
    {0x000000C0, L"RSC-R1UG-2E8GR-UP", "RSC-R1UG-2E8GR-UP"},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0, 0x02,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E 3.0 X8", "SLOT2 PCI-E 3.0 X8", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UG-E16R-UP -> ID 0x02
{//Add 0x00C0 to make sure ID is independent.
    {0x000000E0, L"RSC-R1UG-E16R-UP", "RSC-R1UG-E16R-UP"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"PCI-E X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UG-UR
{//Add 0x0C00 to make sure ID is independent.
    {0x00000C00,  L"RSC-R1U-E16R", "RSC-R1U-E16R"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"PCI-E 3.0 X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//default
{//Add 0x0E00 to make sure ID is independent.
    {0x00000E00,  L"DEFAULT_STR", "DEFAULT_STR"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x03,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"", "", "CPU SLOT3 PCI-E 3.0 X16", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//default
{//Add 0x0C00 to make sure ID is independent.
    {0x00000F00,  L"DEFAULT_STR", "DEFAULT_STR"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x03,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"", "", "CPU SLOT3 PCI-E 3.0 X16", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#endif
#if FRIO_SUPPORT
//RSC-R2UF-2E8GR
{
    {0x00000000, L"RSC-R2UF-2E8GR", "RSC-R2UF-2E8GR"},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0, 0x02,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X8", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R1UF-E16
{
    {0x00000001, L"RSC-R1UF-E16R", "RSC-R1UF-E16R"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R2UF-E16
{
    {0x00000002, L"RSC-R2UF-E16R", "RSC-R2UF-E16R"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//no any riser card, but it is a standard slot so must support default slot string.
//if you want to show default string in GIO platform, please fill "RSC" in table.
{
    {0x00000003, L"RSC", "RSC"},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {Smc_Default_String_1, Smc_Default_String_2, "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#endif
#if TIO_SUPPORT
//RSC-R1UTP-E16R
{
    {0x00000000, L"RSC-R1UTP-E16R", "RSC-R1UTP-E16R"},
    { x16,    0,    0,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,    0,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R2UT-3E8R
{
    {0x00000001, L"RSC-R2UT-3E8R", "RSC-R2UT-3E8R"},
    {  x8,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0, 0x02,    0, 0x03,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {  x8,    0,   x8,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X8", "SLOT3 PCI-E X8", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//RSC-R2UT-E8E16R
{
    {0x00000002, L"RSC-R2UT-E8E16R", "RSC-R2UT-E8E16R"},
    { x16,    0,    0,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x02,    0,    0,    0, 0x01,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X8", "SLOT2 PCI-E X16", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
//default string.
{
    {0x00000003, L"RSC", "RSC"},
    { x16,    0,    0,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x01,    0,    0,    0, 0x02,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    { x16,    0,    0,    0,   x8,    0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"SLOT1 PCI-E X16", "SLOT2 PCI-E X8", "", ""},
    {"", "", "", ""},
    {"", ""},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
},
#endif
{
    {0xffffffff, L"", ""},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {"", "", "", ""},
    {"", "", "", ""},
    {"", ""},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}
}};

#endif
