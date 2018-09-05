//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
//**************************************************************************
//*                                                                        *
//*      Intel Restricted Secret                                           *
//*                                                                        *
//*      CAR Video Initialization Code                                     *
//*                                                                        *
//*      Copyright (c) 2013, Intel Corporation.                            *
//*                                                                        *
//*      This software and associated documentation (if any) is furnished  *
//*      under a license and may only be used or copied in accordance      *
//*      with the terms of the license. Except as permitted by such        *
//*      license, no part of this software or documentation may be         *
//*      reproduced, stored in a retrieval system, or transmitted in any   *
//*      form or by any means without the express written consent of       *
//*      Intel Corporation.                                                *
//*                                                                        *
//*      This program has been developed by Intel Corporation.             *
//*      Licensee has Intel's permission to incorporate this source code   *
//*      into their product, royalty free.  This source code may NOT be    *
//*      redistributed to anyone without Intel's written permission.       *
//*                                                                        *
//*      Intel specifically disclaims all warranties, express or           *
//*      implied, and all liability, including consequential and other     *
//*      indirect damages, for the use of this code, including liability   *
//*      for infringement of any proprietary rights, and including the     *
//*      warranties of merchantability and fitness for a particular        *
//*      purpose.  Intel does not assume any responsibility for any        *
//*      errors which may appear in this code nor any responsibility to    *
//*      update it.                                                        *
//*                                                                        *
//**************************************************************************

//**************************************************************************
//*                                                                        *
//*  PURPOSE:                                                              *
//*                                                                        *
//*      This file defines function prototypes and data structures for     *
//*  interacting with video during reference code.                         *
//*                                                                        *
//**************************************************************************

#ifndef _fastvideo_h
#define _fastvideo_h

//#include <PlatformVideoInit.h>

#define VIDEO_SUCCESS               0x00
#define VIDEO_ERR_BAD_DEVPATH       0x01
#define VIDEO_ERR_BAD_PARM          0x02
#define VIDEO_ERR_OUT_OF_RESOURCES  0x03

typedef struct {
    UINT8   Bus;
    UINT8   Dev;
    UINT8   Func;
} DEV_PATH;

typedef struct {
    UINT8   *FontMap;
    UINT8   StartChar;
    UINT16  CharCount;
    UINT8   CharSize;
    UINT8   TargetTable;
} FONT_MAP_INFO;

typedef struct {
    UINT8       RootBus;            //Bus Number of the root bus containing the VGA controller.
    DEV_PATH    *DevPath;      		//Pointer to a buffer of dev path structure that define the location of the video controller in the PCI topology
    UINT8       DevPathEntries;     //Count of entries in the preceeding buffer.
    UINT32      PciExpressCfgBase;  //The base address of PCI Express Memory Mapped Configuration Space.
    UINT32      MemBase;            //The base address of the region where MMIO BARs are assigned when configuring the VGA controller
    UINT32      MemSizeMax;         //The maximum amount of memory to allow during BAR assignemnt.
    UINT16      IoBase;             //The base address of the region where IO BARs are assigned when configuring the VGA controller
    UINT16      IoSizeMax;          //The maximum amount of IO to allow during BAR assignment.
    FONT_MAP_INFO *FontMap;			//Pointer to a buffer of font map information structures that define the font map(s) to load.
    UINT8       FontMapCount;       //Count of entries in the preceeding buffer.
} VIDEO_PARAMETERS;

#define ALL_TABLES -1

/*
 * VideoInit - Initalizes video controller with VGA standard init. Should be called before any of the other routines are used in POST.
 * 
 * Assumptions:
 *    Basic decode to IOH for MMIO and IO are available. No Bus numbers or resources have been assigned.
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the above error codes if something did not work or bad input was provided.
 */
UINT32 VideoInit(VIDEO_PARAMETERS Parameters); 

/*
 * VideoClear - clears the screen by writting all space characters to the text mode frame buffer.
 * 
 * Input: none.
 *
 * Output:
 *   VIDEO_SUCCESS if successful, one of the above error codes if not.
 *
 */
UINT32 VideoClear();

/*
 * VideoClearRow - clears a single row of text from the screen (writes spaces)
 *
 * Input:
 *  y - the zero-based row number to clear
 *
 * Output:
 *  VIDEO_SUCCESS if everything worked, one of the above error codes if something did not work or bad input was provided.
 */
UINT32 VideoClearRow(UINT8 y);

/*
 * VideoGotoXY - sets the cursor to the row and coloumn specified by x and y.
 *
 * Input:
 *   x - the zero-based column position
 *   y - the zero-based row position
 *
 * Output:
 *  VIDEO_SUCCESS if everything worked, one of the above error codes if something did not work or bad input was provided.
 */
UINT32 VideoGotoXY(UINT8 x, UINT8 y);

/*
 * VideoPrint - prints the specified string. Semantics are the same as the reference code printf function.
 *
 * Input:
 *   Format - the format string.
 *   Variable Arguments - as indicated by the format string.
 *
 * Output:
 *  VIDEO_SUCCESS if everything worked, one of the above error codes if something did not work or bad input was provided.
 */
UINT32 VideoPrint(const char* Format, ...);

/*
 * VideoPrintXY - prints the specified string at the specified position. Semantics are the same as the reference code printf function.
 *
 * Input:
 *   x - the zero-based column position
 *   y - the zero-based row position
 *   Format - the format string.
 *   Variable Arguments - as indicated by the format string.
 *
 * Output:
 *  VIDEO_SUCCESS if everything worked, one of the above error codes if something did not work or bad input was provided.
 */
UINT32 VideoPrintXY(UINT8 x, UINT8 y, const char* Format, ...);

#define VIDEO_COLOR_BLACK       0
#define VIDEO_COLOR_BLUE        1
#define VIDEO_COLOR_GREEN       2
#define VIDEO_COLOR_CYAN        3
#define VIDEO_COLOR_RED         4
#define VIDEO_COLOR_MAGENTA     5
#define VIDEO_COLOR_ORANGE      6
#define VIDEO_COLOR_WHITE       7

#define VIDEO_INTENSITY         BIT3
#define VIDEO_BLINK             BIT7

/*
 * VideoSetColor - Sets the current color to the specified value
 *
 * Input:
 *  c - text color
 *       Foreground is specified by bits 2:0,
 *       Background is specified by bits 6:4
 *       Bit 3 increases the intensity of the color selected.
 *       Bit 7 causes the text to blink.
 *
 *      For example, intense white text on a blue backround would be specified as ((VIDEO_COLOR_BLUE << 4) | VIDEO_COLOR_WHITE | VIDEO_INTENSITY).
 *
 *
 * Output:
 *  VIDEO_SUCCESS if everything worked, one of the above error codes if something did not work or bad input was provided.
 */
UINT32 VideoSetColor(UINT8 c);



#endif
