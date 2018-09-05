//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.04
//    Bug Fix: Fix POST hang up when set MMCFG Base to 3G.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Mar/17/2017
//
//  Rev. 1.03
//    Bug Fix: Fix Early Graphic Logo malfunction for DP platform.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Mar/15/2017
//
//  Rev. 1.02
//    Bug Fix: Early Graphic Logo Support.
//    Reason : 
//    Auditor: Isaac Hsu
//    Date   : Mar/14/2017
//
//  Rev. 1.01
//    Bug Fix: Turn on Early-Video in earlier PEI.
//    Reason : To sync with Grantley
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/05/2014
//
//****************************************************************************
//****************************************************************************
#include <Token.h>

struct _InitValueTable
{
  UINT8     Offset;
  UINT8     Value;
};
typedef struct _InitValueTable IVT_t;

struct _InitPciDevice
{
  UINT8     PciBus;
  UINT8     PciDev;
  UINT8     PciFun;
  UINT16    VendorId;
  UINT16    DeviceId;
  IVT_t    *InitValueTable;
};
typedef struct _InitPciDevice IPD_t;


IVT_t IntelPciEBrigdeInitTable[] =
{
  // I/O Base & Limit
  {0x1C,0xD0},{0x1D,0xD0},
  // Memory Base & Limit, Prefetch Memory Base & Limit
#if EarlyVideo_Mode
  {0x20,0x00},{0x21,(UINT8)(VgaMmio>>24)},{0x22,0x00},{0x23,(UINT8)((VgaMmio+0x1000000)>>24)},
#else
  {0x20,0x00},{0x21,0xF8},{0x22,0x00},{0x23,0xFA},
#endif
  {0x24,0xF1},{0x25,0xFF},{0x26,0x01},{0x27,0x00},
  // Cache Line Size
  {0x0C,0x10},
  // Interrupt Line
  {0x3C,0x05},

  // Bridge Control
  {0x3E,0x18},
  // PCI Command
  {0x04,0x07},
  // Table End.
  {0x00,0x00}
};

IVT_t AspeedPciEBrigdeInitTable[] =
{
  // I/O Base & Limit
  {0x1C,0xD1},{0x1D,0xD1},
  // Memory Base & Limit, Prefetch Memory Base & Limit
#if EarlyVideo_Mode
  {0x20,0x00},{0x21,(UINT8)(VgaMmio>>24)},{0x22,0x00},{0x23,(UINT8)((VgaMmio+0x1000000)>>24)},
#else
  {0x20,0x00},{0x21,0xF8},{0x22,0x00},{0x23,0xFA},
#endif
  {0x24,0xF1},{0x25,0xFF},{0x26,0x01},{0x27,0x00},
  // Cache Line Size
  {0x0C,0x10},
  // Interrupt Line
  {0x3C,0x07},

  // Bridge Control
  {0x3E,0x18},
  // PCI Command
  {0x04,0x07},
  // Table End.
  {0x00,0x00}
};

IVT_t AspeedPciVGAInitTable[] =
{
  // BAR 0
  {0x10,0x00},
  {0x11,0x00},
  {0x12,0x00},
#if EarlyVideo_Mode
  {0x13,(UINT8)(VgaMmio>>24)},
#else
  {0x13,0xF8},
#endif
  // BAR 1
  {0x14,0x00},
  {0x15,0x00},
  {0x16,0x00},
#if EarlyVideo_Mode
  {0x17,(UINT8)((VgaMmio+0x1000000)>>24)},
#else
  {0x17,0xFA},
#endif
  // BAR 2
  {0x18,0x01},
  {0x19,0xD0},
  {0x1A,0x00},
  {0x1B,0x00},
  // Interrupt Line
  {0x3C,0x07},

  // PCI Command
  {0x04,0x07},
  // Table End.
  {0x00,0x00}
};

//#define MaxDeepPciBusNumber     0x12	//R1.02
//#define	Aspeed_P2P_BUS	0x11
//#define	Aspeed_VGA_BUS	0x12

IPD_t AstVbiosPciDeviceInitTable[] =
{
   ///  Bus/Dev/Fun  VendorId/DeviceId    InitValueTable  ///
  {0x00, Root_Port_Dev, Root_Port_Fun, 0x8086, Root_Port_DID, &IntelPciEBrigdeInitTable[0]},
  {Aspeed_P2P_BUS, 0x00, 0x00, 0x1A03, 0x1150, &AspeedPciEBrigdeInitTable[0]},
  {Aspeed_VGA_BUS, 0x00, 0x00, 0x1A03, 0x2000, &AspeedPciVGAInitTable[0]},
  {0,0,0,0,0, NULL}
};

