/** @file

@copyright
 Copyright (c) 2009 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "PchHdaVerbTables.h"

HDAUDIO_VERB_TABLE HdaVerbTableAlc286S = {
  //
  //  VerbTable: (Realtek ALC286S)
  //  Revision ID = 0xff
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0286
  //
  {
    0x10EC,     // Vendor ID
    0x0286,     // Device ID
    0xFF,       // Revision ID (any)
    0xFF,       // SDI number (any)
    15 * 4 + 2  // Data size in DWORDs
  },
  {
    //===================================================================================================
    //
    //                               Realtek Semiconductor Corp.
    //
    //===================================================================================================

    //Realtek High Definition Audio Configuration - Version : 5.0.2.1
    //Realtek HD Audio Codec : ALC286
    //PCI PnP ID : PCI\VEN_0000&DEV_0000&SUBSYS_00000000
    //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0286&SUBSYS_00000000
    //The number of verb command block : 15

    //    NID 0x12 : 0x90A60130
    //    NID 0x13 : 0x40000000
    //    NID 0x14 : 0x90170110
    //    NID 0x17 : 0x411111F0
    //    NID 0x18 : 0x03A19020
    //    NID 0x19 : 0x411111F0
    //    NID 0x1A : 0x0381302F
    //    NID 0x1D : 0x4066832D
    //    NID 0x1E : 0x411111F0
    //    NID 0x21 : 0x0321101F

    //===== HDA Codec Subsystem ID Verb-table =====
    //HDA Codec Subsystem ID  : 0x00000000
    0x00172000,
    0x00172100,
    0x00172200,
    0x00172300,

    //===== Pin Widget Verb-table =====
    //Widget node 0x01 :
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    //Pin widget 0x12 - DMIC
    0x01271C40,
    0x01271D01,
    0x01271EA6,
    0x01271F90,
    //Pin widget 0x13 - DMIC
    0x01371C00,
    0x01371D00,
    0x01371E00,
    0x01371F40,
    //Pin widget 0x14 - SPEAKER-OUT (Port-D)
    0x01471C10,
    0x01471D01,
    0x01471E17,
    0x01471F90,
    //Pin widget 0x17 - I2S-OUT
    0x01771CF0,
    0x01771D11,
    0x01771E11,
    0x01771F41,
    //Pin widget 0x18 - MIC1 (Port-B)
    0x01871C30,
    0x01871D90,
    0x01871EA1,
    0x01871F03,
    //Pin widget 0x19 - I2S-IN
    0x01971CF0,
    0x01971D11,
    0x01971E11,
    0x01971F41,
    //Pin widget 0x1A - LINE1 (Port-C)
    0x01A71C3F,
    0x01A71D30,
    0x01A71E81,
    0x01A71F03,
    //Pin widget 0x1D - PC-BEEP
    0x01D71C2D,
    0x01D71D23,
    0x01D71E66,
    0x01D71F40,
    //Pin widget 0x1E - S/PDIF-OUT
    0x01E71CF0,
    0x01E71D11,
    0x01E71E11,
    0x01E71F41,
    //Pin widget 0x21 - HP-OUT (Port-A)
    0x02171C20,
    0x02171D10,
    0x02171E21,
    0x02171F03,
    //Widget node 0x20 :
    0x02050071,
    0x02040014,
    0x02050010,
    0x02040C22,
    //Widget node 0x20 - 1 :
    0x0205004F,
    0x02045029,
    0x02050051,
    0x02045428,
    //Widget node 0x20 - 2 :
    0x0205002B,
    0x02040C50,
    0x0205002D,
    0x02041020,
    // New verbs from Zeek (Realtek)
    0x02050063,
    0x02042906
  }
};

HDAUDIO_VERB_TABLE HdaVerbTableAlc298 = {
  //
  //  VerbTable: (Realtek ALC298)
  //  Revision ID = 0xff
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0286
  //
  {
    0x10EC,     // Vendor ID
    0x0298,     // Device ID
    0xFF,       // Revision ID (any)
    0xFF,       // SDI number (any)
    16 * 4      // Data size in DWORDs
  },
  {
    //===================================================================================================
    //
    //                               Realtek Semiconductor Corp.
    //
    //===================================================================================================

    //Realtek High Definition Audio Configuration - Version : 5.0.2.1
    //Realtek HD Audio Codec : ALC298
    //PCI PnP ID : PCI\VEN_0000&DEV_0000&SUBSYS_00000000
    //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0298&SUBSYS_00000000
    //The number of verb command block : 15

    //    NID 0x12 : 0x90A60130
    //    NID 0x13 : 0x411111F0
    //    NID 0x14 : 0x90170110
    //    NID 0x17 : 0x40000000
    //    NID 0x18 : 0x03A11020
    //    NID 0x19 : 0x411111F0
    //    NID 0x1A : 0x411111F0
    //    NID 0x1D : 0x40E6852D
    //    NID 0x1E : 0x411111F0
    //    NID 0x21 : 0x0321101F


    //===== HDA Codec Subsystem ID Verb-table =====
    //HDA Codec Subsystem ID  : 0x00000000
    0x00172000,
    0x00172100,
    0x00172200,
    0x00172300,


    //===== Pin Widget Verb-table =====
    //Widget node 0x01 :
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    //Pin widget 0x12 - DMIC
    0x01271C30,
    0x01271D01,
    0x01271EA6,
    0x01271F90,
    //Pin widget 0x13 - DMIC
    0x01371CF0,
    0x01371D11,
    0x01371E11,
    0x01371F41,
    //Pin widget 0x14 - SPEAKER-OUT (Port-D)
    0x01471C10,
    0x01471D01,
    0x01471E17,
    0x01471F90,
    //Pin widget 0x17 - I2S-OUT
    0x01771C00,
    0x01771D00,
    0x01771E00,
    0x01771F40,
    //Pin widget 0x18 - MIC1 (Port-B)
    0x01871C20,
    0x01871D10,
    0x01871EA1,
    0x01871F03,
    //Pin widget 0x19 - I2S-IN
    0x01971CF0,
    0x01971D11,
    0x01971E11,
    0x01971F41,
    //Pin widget 0x1A - LINE1 (Port-C)
    0x01A71CF0,
    0x01A71D11,
    0x01A71E11,
    0x01A71F41,
    //Pin widget 0x1D - PC-BEEP
    0x01D71C2D,
    0x01D71D85,
    0x01D71EE6,
    0x01D71F40,
    //Pin widget 0x1E - S/PDIF-OUT
    0x01E71CF0,
    0x01E71D11,
    0x01E71E11,
    0x01E71F41,
    //Pin widget 0x21 - HP-OUT (Port-A)
    0x02171C1F,
    0x02171D10,
    0x02171E21,
    0x02171F03,
    //Widget node 0x20 :
    0x02050019,
    0x02040217,
    0x02050001,
    0x0204AD55,
    //Widget node 0x20 - 1 :
    0x02050002,
    0x02048EB5,
    0x02050034,
    0x02045610,
    //Widget node 0x20 - 2 :
    0x02050035,
    0x02041AA4,
    0x0205008F,
    0x02041000,
    //Widget node 0x20 - 3 :
    0x0205004F,
    0x02045009,
    0x0205004F,
    0x02045009
  }
};

HDAUDIO_VERB_TABLE HdaVerbTableAlc888S = {
  //
  //  VerbTable: (Realtek ALC888S)
  //  Revision ID = 0xFF
  //  Codec Verb Table for SKL PCH boards
  //  Codec Address: CAd value (0/1/2)
  //  Codec Vendor: 0x10EC0888
  //
  {
    0x10EC,     // Vendor ID
    0x0888,     // Device ID
    0xFF,       // Revision ID (any)
    0xFF,       // SDI number (any)
    17 * 4      // Data size in DWORDs
  },
  {
    //===================================================================================================
    //
    //                               Realtek Semiconductor Corp.
    //
    //===================================================================================================

    //Realtek High Definition Audio Configuration - Version : 5.0.3.1
    //Realtek HD Audio Codec : ALC888S-VD
    //PCI PnP ID : PCI\VEN_0000&DEV_0000&SUBSYS_00000000
    //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0888&SUBSYS_00000000
    //The number of verb command block : 17

    //    NID 0x11 : 0x40000000
    //    NID 0x12 : 0x411111F0
    //    NID 0x14 : 0x411111F0
    //    NID 0x15 : 0x411111F0
    //    NID 0x16 : 0x411111F0
    //    NID 0x17 : 0x411111F0
    //    NID 0x18 : 0x411111F0
    //    NID 0x19 : 0x02A19020
    //    NID 0x1A : 0x411111F0
    //    NID 0x1B : 0x02214010
    //    NID 0x1C : 0x411111F0
    //    NID 0x1D : 0x4024C019
    //    NID 0x1E : 0x411111F0
    //    NID 0x1F : 0x01C56130

//===== HDA Codec Subsystem ID Verb-table =====
//HDA Codec Subsystem ID  : 0x00000000
    0x0017206D,
    0x00172109,
    0x001722D9,
    0x00172315,


//===== Pin Widget Verb-table =====
//Widget node 0x01 :
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
//Pin widget 0x11 - S/PDIF-OUT2
    0x01171C00,
    0x01171D00,
    0x01171E00,
    0x01171F40,
//Pin widget 0x12 - DMIC
    0x01271CF0,
    0x01271D11,
    0x01271E11,
    0x01271F41,
//Pin widget 0x14 - FRONT (Port-D)
    0x01471CF0,
    0x01471D11,
    0x01471E11,
    0x01471F41,
//Pin widget 0x15 - SURR (Port-A)
    0x01571CF0,
    0x01571D11,
    0x01571E11,
    0x01571F41,
//Pin widget 0x16 - CEN/LFE (Port-G)
    0x01671CF0,
    0x01671D11,
    0x01671E11,
    0x01671F41,
//Pin widget 0x17 - SIDESURR (Port-H)
    0x01771CF0,
    0x01771D11,
    0x01771E11,
    0x01771F41,
//Pin widget 0x18 - MIC1 (Port-B)
    0x01871CF0,
    0x01871D11,
    0x01871E11,
    0x01871F41,
//Pin widget 0x19 - MIC2 (Port-F)
    0x01971C20,
    0x01971D90,
    0x01971EA1,
    0x01971F02,
//Pin widget 0x1A - LINE1 (Port-C)
    0x01A71CF0,
    0x01A71D11,
    0x01A71E11,
    0x01A71F41,
//Pin widget 0x1B - LINE2 (Port-E)
    0x01B71C10,
    0x01B71D40,
    0x01B71E21,
    0x01B71F02,
//Pin widget 0x1C - CD-IN
    0x01C71CF0,
    0x01C71D11,
    0x01C71E11,
    0x01C71F41,
//Pin widget 0x1D - BEEP-IN
    0x01D71C19,
    0x01D71DC0,
    0x01D71E24,
    0x01D71F40,
//Pin widget 0x1E - S/PDIF-OUT1
    0x01E71CF0,
    0x01E71D11,
    0x01E71E11,
    0x01E71F41,
//Pin widget 0x1F - S/PDIF-IN
    0x01F71C30,
    0x01F71D61,
    0x01F71EC5,
    0x01F71F01,
//Widget node 0x20 :
    0x02050007,
    0x020409C8,
    0x02050007,
    0x020409C8
  }
};

HDAUDIO_VERB_TABLE HdaVerbTableAlc892 = {

   //
   //  VerbTable: (RealTek ALC892)
   //  Revision ID = 0xFF
   //  Codec Verb Table For Redfort
   //  Codec Address: CAd value (0/1/2)
   //  Codec Vendor: 0x10EC0892
   //
  {
    0x10EC,         // Vendor ID
    0x0892,         // Device ID
    0xFF,           // Revision ID
    0xFF,           // SDI number (any)
    14 * 4          // Data size in DWORDs
  },
  {
    //===== HDA Codec Subsystem ID Verb-table =====
    //HDA Codec Subsystem ID  : 0x00000000
    0x00172000,
    0x00172100,
    0x00172200,
    0x00172300,

    //===== Pin Widget Verb-table =====
    //Widget node 0x01 :
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,

    //
    // Pin Complex 1      (NID 11h)
    // Pin widget 0x11 - S/PDIF-OUT2
    //
    0x01171CF0,
    0x01171D11,
    0x01171E11,
    0x01171F41,

    //Pin widget 0x12 - DMIC
    0x01271CF0,
    0x01271D10,
    0x01271E11,
    0x01271F41,

    //
    // Pin Complex (NID 0x14 )
    // Pin widget 0x14 - Front (Port-D)
    //
    0x01471C30,
    0x01471D41,
    0x01471E01,
    0x01471F01,

    //Pin widget 0x15 - SURR (Port-A)
    0x01571C31,
    0x01571D11,
    0x01571E01,
    0x01571F01,

    //Pin widget 0x16 - CEN/LFE (Port-G)
    0x01671C32,
    0x01671D61,
    0x01671E01,
    0x01671F01,

    //Pin widget 0x17 - SIDESURR (Port-H)
    0x01771C33,
    0x01771D21,
    0x01771E01,
    0x01771F01,

    //
    // Pin Complex (NID 0x18 )
    // Pin widget 0x18 - Mic1 (Port-B)
    //
    0x01871C50,
    0x01871D98,
    0x01871EA1,
    0x01871F01,

    //Pin widget 0x19 - MIC2 (Port-F)
    0x01971C80,
    0x01971D11,
    0x01971EA0,
    0x01971F70,

    //Pin widget 0x1A - LINE1 (Port-C)
    0x01A71C51,
    0x01A71D30,
    0x01A71E81,
    0x01A71F01,

    //Pin widget 0x1B - LINE2 (Port-E)
    0x01B71C40,
    0x01B71D11,
    0x01B71E20,
    0x01B71F70,

    //
    // Pin Complex 2      (NID 1Eh)
    // Pin widget 0x1E - S/PDIF-OUT
    //
    0x01E71CF0,
    0x01E71D11,
    0x01E71EF0,
    0x01E71F70,

    //
    // Pin Complex 3      (NID 1Fh)
    // Pin widget 0x1F - S/PDIF-IN
    //
    0x01F71CF0,
    0x01F71D11,
    0x01F71EF0,
    0x01F71F70
  }
};

