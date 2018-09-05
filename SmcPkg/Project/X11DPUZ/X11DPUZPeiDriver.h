//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jacker Yeh
//    Date:     Feb/04/2016
//
//****************************************************************************
//****************************************************************************

//----------------------------------------------------------------------------
// Include File(s)
//----------------------------------------------------------------------------
#include "token.h"
#include "SuperMPeiDriver.h"


/*
Hi all,

        Since Purley CPU PCIE root port assigned rule is different with old.

        CPU root port bus number is different when CPU types or CPU number is different. ex: Omni-patch is different with non Omni-path.

        I redefine BDF rule for the it.

        I add a "word" for segment to all BDF define in SDL, now BDF is a 32bits (Fun: bit 0~2, Dev: bit3~7, Bus: bit8~15, Seg: bit16~31).

        If your pcie is from CPU, Bus is dynamic value so fill Bus = 0, Dev and Fun are same with old.

Segment rule is bit 0~3 is for IOU number, DMI=0, IOU0=1, IOU1=2, IOU3=2. Bit 4~7 is for CPU number, CPU1=1, CPU2=2.

If your pcie is from PCH, Bus is a fixed value so Bus, Dev and Fun are same with old.

When you want to get BDF in code, please call "SmcBDFTransfer()", it will return a 16bits BDF.

The same define implement to smbios type 9 and type 41. You can refer X11SPI and X11DPU.

#P0PE1A = 0x00110000, P0PE1B = 0x00110008, P0PE1C = 0x00110010, P0PE1D = 0x00110020
#P0PE2A = 0x00120000, P0PE2B = 0x00120008, P0PE2C = 0x00120010, P0PE2D = 0x00120020
#P0PE3A = 0x00130000, P0PE3B = 0x00130008, P0PE3C = 0x00130010, P0PE3D = 0x00130020
#P1PE0A = 0x00200000,
#P1PE1A = 0x00210000, P1PE1B = 0x00210008, P1PE1C = 0x00210010, P1PE1D = 0x00210020
#P1PE2A = 0x00220000, P1PE2B = 0x00220008, P1PE2C = 0x00220010, P1PE2D = 0x00220020
#P1PE3A = 0x00230000, P1PE3B = 0x00230008, P1PE3C = 0x00230010, P1PE3D = 0x00230020

Thanks
Kasber

*/

//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2012 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************

