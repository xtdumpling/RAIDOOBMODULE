
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//    Rev. 1.01
//      Bug Fixed: This driver didn't support "Ctrl+Home".
//      Reason   : Pressing "Ctrl+Home" to do BIOS recovery.
//      Auditor  : Yenhsin Cho
//      Date     : Apr/10/2017
//
//     Rev. 1.00
//       Bug Fix: Patch CTRL+HOME can't work via console
//       Reason:  Function improve.
//       Auditor: Jacker Yeh (Refer from Grantley)
//       Date:    Oct/27/2016
//
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file TerminalBoard.h
    Board file for the Terminal Module.

**/

#ifndef _TERMINAL_BOARD_H__
#define _TERMINAL_BOARD_H__

//---------------------------------------------------------------------------
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AmiKeycode.h>
#include "TerminalSetupVar.h"

//---------------------------------------------------------------------------

#define ESC_SEQ_STATE_ARRAY_END_MARKER {0,0,0,0,0}
#define RESET       0xFFFF
#define ASCII_ESC   0x1B

#define GLOBAL_ALL      PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_SCO | PUTTY_ESCN | PUTTY_VT400

typedef struct _STATE_DESCRIPTION STATE_DESCRIPTION;

//
// Esc Sequence defination.
//
struct _STATE_DESCRIPTION{
    CHAR16 InputChar;
    STATE_DESCRIPTION *NextEscSeqState;
    UINT16 EfiInputKeyScanCode;
    CHAR16 EfiInputKeyUnicodeChar;
    UINT8  KeyPad;
};

STATE_DESCRIPTION EscSeqStateArrayEndMarker      = ESC_SEQ_STATE_ARRAY_END_MARKER;
STATE_DESCRIPTION *EscSeqStateArrayEndMarkerPtr  = &EscSeqStateArrayEndMarker;

STATE_DESCRIPTION PuttyLinuxEscSeq[] = 
{
    {'A', NULL, EFI_SCAN_F1, 0, PUTTY_LINUX},
    {'B', NULL, EFI_SCAN_F2, 0, PUTTY_LINUX},
    {'C', NULL, EFI_SCAN_F3, 0, PUTTY_LINUX},
    {'D', NULL, EFI_SCAN_F4, 0, PUTTY_LINUX},
    {'E', NULL, EFI_SCAN_F5, 0, PUTTY_LINUX},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyLinuxF6[] =
{
    {'~', NULL, EFI_SCAN_F6,  0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyLinuxF7[] =
{
    {'~', NULL, EFI_SCAN_F7,  0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyLinuxF8[] =
{
    {'~', NULL, EFI_SCAN_F8,  0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyLinuxF9[] =
{
    {'~', NULL, EFI_SCAN_F9,  0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyLinuxF10[] =
{
    {'~', NULL, EFI_SCAN_F10,  0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyLinuxF11[] =
{
    {'~', NULL, EFI_SCAN_F11,  0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyLinuxF12[] =
{
    {'~', NULL, EFI_SCAN_F12,  0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyXterm86F5[] =
{
    {'~', NULL, EFI_SCAN_F5,  0, PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyEscNF1[] =
{
    {'~', NULL, EFI_SCAN_F1,  0, PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyEscNF2[] =
{
    {'~', NULL, EFI_SCAN_F2,  0, PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyEscNF3[] =
{
    {'~', NULL, EFI_SCAN_F3,  0, PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyEscNF4[] =
{
    {'~', NULL, EFI_SCAN_F4,  0, PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt100EscSeq1[] =
{
    {'~', NULL, EFI_SCAN_HOME,  0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN },
    {'~', NULL, EFI_SCAN_INS,  0, PUTTY_VT400},
    {'7', PuttyLinuxF6, 0, 0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    {'8', PuttyLinuxF7, 0, 0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    {'9', PuttyLinuxF8, 0, 0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    {'5', PuttyXterm86F5, 0, 0, PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    {'1', PuttyEscNF1, 0, 0, PUTTY_ESCN | PUTTY_VT400},
    {'2', PuttyEscNF2, 0, 0, PUTTY_ESCN | PUTTY_VT400},
    {'3', PuttyEscNF3, 0, 0, PUTTY_ESCN | PUTTY_VT400},
    {'4', PuttyEscNF4, 0, 0, PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt100EscSeq2[] =
{
    {'~', NULL, EFI_SCAN_INS,  0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN},
    {'~', NULL, EFI_SCAN_HOME,  0, PUTTY_VT400},
    {'0', PuttyLinuxF9, 0, 0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    {'1', PuttyLinuxF10, 0, 0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    {'3', PuttyLinuxF11, 0, 0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    {'4', PuttyLinuxF12, 0, 0, PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt100EscSeq3[] =
{
    {'~', NULL, EFI_SCAN_DEL,  0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt100EscSeq4[] =
{
    {'~', NULL, EFI_SCAN_END,  0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt100EscSeq5[] =
{
    {'~', NULL, EFI_SCAN_PGUP,  0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt100EscSeq6[] =
{
    {'~', NULL, EFI_SCAN_PGDN,  0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt400EscSeq3[] =
{
    {'~', NULL, EFI_SCAN_PGUP,  0, PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt400EscSeq4[] =
{
    {'~', NULL, EFI_SCAN_DEL,  0, PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt400EscSeq5[] =
{
    {'~', NULL, EFI_SCAN_END,  0, PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION PuttyVt400EscSeq6[] =
{
    {'~', NULL, EFI_SCAN_PGDN,  0, PUTTY_VT400},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION OemEscSeqStateArray[]=
{
    {'P', NULL, EFI_SCAN_F1, 0, GLOBAL_ALL},
    {'Q', NULL, EFI_SCAN_F2, 0, GLOBAL_ALL},
    {'w', NULL, EFI_SCAN_F3, 0, GLOBAL_ALL},
    {'x', NULL, EFI_SCAN_F4, 0, GLOBAL_ALL},
    {'t', NULL, EFI_SCAN_F5, 0, GLOBAL_ALL},
    {'u', NULL, EFI_SCAN_F6, 0, GLOBAL_ALL},
    {'q', NULL, EFI_SCAN_F7, 0, GLOBAL_ALL},
    {'r', NULL, EFI_SCAN_F8, 0, GLOBAL_ALL},
    {'p', NULL, EFI_SCAN_F9, 0, GLOBAL_ALL},
    {'M', NULL, EFI_SCAN_F10,0, GLOBAL_ALL},
    ESC_SEQ_STATE_ARRAY_END_MARKER

};

STATE_DESCRIPTION CursorPositionEscSeqStateArray[] =
{
    {'C', NULL, EFI_SCAN_RIGHT, 0, GLOBAL_ALL},
    {'D', NULL, EFI_SCAN_LEFT,  0, GLOBAL_ALL},
    {'A', NULL, EFI_SCAN_UP,    0, GLOBAL_ALL},
    {'B', NULL, EFI_SCAN_DN,    0, GLOBAL_ALL},
    {'H', NULL, EFI_SCAN_HOME,  0, GLOBAL_ALL},
    {'K', NULL, EFI_SCAN_END,   0, GLOBAL_ALL},
    {'1', PuttyVt100EscSeq1,    0,   0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400 },
    {'2', PuttyVt100EscSeq2,    0,   0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN | PUTTY_VT400 },
    {'3', PuttyVt100EscSeq3,    0,   0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN },
    {'4', PuttyVt100EscSeq4,    0,   0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN },
    {'5', PuttyVt100EscSeq5,    0,   0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN },
    {'6', PuttyVt100EscSeq6,    0,   0, PUTTY_VT100 | PUTTY_LINUX | PUTTY_XTERMR6 | PUTTY_ESCN },
    {'3', PuttyVt400EscSeq3,    0,   0, PUTTY_VT400},
    {'4', PuttyVt400EscSeq4,    0,   0, PUTTY_VT400},
    {'5', PuttyVt400EscSeq5,    0,   0, PUTTY_VT400},
    {'6', PuttyVt400EscSeq6,    0,   0, PUTTY_VT400},
    {'[', PuttyLinuxEscSeq,     0,   0, PUTTY_LINUX},
    {'M', NULL, EFI_SCAN_F1,    0, PUTTY_SCO},
    {'N', NULL, EFI_SCAN_F2,   0, PUTTY_SCO},
#if OEM_ESC_SEQUENCES	
    {'O', OemEscSeqStateArray,  0,   0, GLOBAL_ALL},
#else
    {'O', NULL, EFI_SCAN_F3,    0, PUTTY_SCO},
#endif
    {'P', NULL, EFI_SCAN_F4,    0, PUTTY_SCO},
    {'Q', NULL, EFI_SCAN_F5,    0, PUTTY_SCO},
    {'R', NULL, EFI_SCAN_F6,    0, PUTTY_SCO},
    {'S', NULL, EFI_SCAN_F7,    0, PUTTY_SCO},
    {'T', NULL, EFI_SCAN_F8,    0, PUTTY_SCO},
    {'U', NULL, EFI_SCAN_F9,    0, PUTTY_SCO},
    {'V', NULL, EFI_SCAN_F10,   0, PUTTY_SCO},
    {'W', NULL, EFI_SCAN_F11,   0, PUTTY_SCO},
    {'X', NULL, EFI_SCAN_F12,   0, PUTTY_SCO},
    {'I', NULL, EFI_SCAN_PGUP,  0, PUTTY_SCO},
    {'G', NULL, EFI_SCAN_PGDN,  0, PUTTY_SCO},
    {'F', NULL, EFI_SCAN_END,   0, PUTTY_SCO},
    {'L', NULL, EFI_SCAN_INS,   0, PUTTY_SCO},
    {0x7F, NULL, EFI_SCAN_DEL,  0, PUTTY_SCO},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION FunctionKeyEscSeqStateArray[] =
{
    {'P', NULL, EFI_SCAN_F1, 0, GLOBAL_ALL},
    {'Q', NULL, EFI_SCAN_F2, 0, GLOBAL_ALL},
    {'R', NULL, EFI_SCAN_F3, 0, GLOBAL_ALL},
    {'S', NULL, EFI_SCAN_F4, 0, GLOBAL_ALL},
    {'T', NULL, EFI_SCAN_F5, 0, PUTTY_VT100},
    {'U', NULL, EFI_SCAN_F6, 0, PUTTY_VT100},
    {'V', NULL, EFI_SCAN_F7, 0, PUTTY_VT100},
    {'W', NULL, EFI_SCAN_F8, 0, PUTTY_VT100},
    {'X', NULL, EFI_SCAN_F9, 0, PUTTY_VT100},
    {'Y', NULL, EFI_SCAN_F10, 0, PUTTY_VT100},
    {'Z', NULL, EFI_SCAN_F11, 0, PUTTY_VT100},
    {'[', NULL, EFI_SCAN_F12, 0, PUTTY_VT100},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

#if SMCPKG_SUPPORT
STATE_DESCRIPTION  CtrlSequenceState2[] =
{
  {'h', NULL, EFI_SCAN_HOME, 0, GLOBAL_ALL},
  ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION  CtrlSequenceState1[] =
{
  {ASCII_ESC, CtrlSequenceState2, 0, 0, GLOBAL_ALL},
  ESC_SEQ_STATE_ARRAY_END_MARKER
};
#endif

STATE_DESCRIPTION ResetSequenceState5[] =
{
    {'R', NULL, RESET, 0, GLOBAL_ALL},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION ResetSequenceState4[] =
{
    {ASCII_ESC, ResetSequenceState5, 0, 0, GLOBAL_ALL},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION ResetSequenceState3[] =
{
    {'r', ResetSequenceState4, 0, 0, GLOBAL_ALL},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION ResetSequenceState2[] =
{
    {ASCII_ESC, ResetSequenceState3, 0, 0, GLOBAL_ALL},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION EscSeqStateArray[] =
{
    {'t', NULL, 0, CHAR_TAB, GLOBAL_ALL},
    {'[', CursorPositionEscSeqStateArray,
                0,              0, GLOBAL_ALL},
    {'O', FunctionKeyEscSeqStateArray,  
                0,              0, GLOBAL_ALL},
    {'R', ResetSequenceState2,
                0,              0, GLOBAL_ALL},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

STATE_DESCRIPTION Vtutf8EscSeqStateArray[] =
{
    {'t', NULL, 0, CHAR_TAB, GLOBAL_ALL},
    {'[', CursorPositionEscSeqStateArray,
                0,              0, GLOBAL_ALL},
    {'O', FunctionKeyEscSeqStateArray,  
                0,              0, GLOBAL_ALL},
    {'h', NULL, EFI_SCAN_HOME,  0, GLOBAL_ALL},
    {'k', NULL, EFI_SCAN_END,   0, GLOBAL_ALL},
    {'+', NULL, EFI_SCAN_INS,   0, GLOBAL_ALL},
    {'-', NULL, EFI_SCAN_DEL,   0, GLOBAL_ALL},
    {'?', NULL, EFI_SCAN_PGUP,  0, GLOBAL_ALL},
    {'/', NULL, EFI_SCAN_PGDN,  0, GLOBAL_ALL},
    {'1', NULL, EFI_SCAN_F1,    0, GLOBAL_ALL},
    {'2', NULL, EFI_SCAN_F2,    0, GLOBAL_ALL},
    {'3', NULL, EFI_SCAN_F3,    0, GLOBAL_ALL},
    {'4', NULL, EFI_SCAN_F4,    0, GLOBAL_ALL},
    {'5', NULL, EFI_SCAN_F5,    0, GLOBAL_ALL},
    {'6', NULL, EFI_SCAN_F6,    0, GLOBAL_ALL},
    {'7', NULL, EFI_SCAN_F7,    0, GLOBAL_ALL},
    {'8', NULL, EFI_SCAN_F8,    0, GLOBAL_ALL},
    {'9', NULL, EFI_SCAN_F9,    0, GLOBAL_ALL},
    {'0', NULL, EFI_SCAN_F10,   0, GLOBAL_ALL},
    {'!', NULL, EFI_SCAN_F11,   0, GLOBAL_ALL},
    {'@', NULL, EFI_SCAN_F12,   0, GLOBAL_ALL},
#if SMCPKG_SUPPORT
    {0x03, CtrlSequenceState1, 0, 0, GLOBAL_ALL},
#endif
    {'R', ResetSequenceState2,
                0,              0, GLOBAL_ALL},
    ESC_SEQ_STATE_ARRAY_END_MARKER
};

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
