# /*++
# Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
# --*/

#ifndef MINIBIOS_BUILD
# Externs
.extern  PcdGet32 (PcdFlashFvSecPeiBase)
.extern  PcdGet32 (PcdFlashFvBackSecPeiBase)
.extern  PcdGet8 (PcdCmosDebugPrintLevelReg)
;.extern PcdGet32 (PcdFlashFvBackSecPeiBase)

#Constant equates
.equ       CMOS_INDEX, 0x70
.equ       CMOS_DATA, 0x71
.equ       CMOS_STATUS, 0x0E
.equ       CMOS_RTCSTS, 0x80
.equ       CMOS_CHECKSUMSTS, 0x40
#endif

#----------------------------------------------------------------------------
#  STARTUP_SEG  S E G M E N T  STARTS
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#
# Procedure:  EarlyChpsetInit_OemHook
#
# Description:  This routine provide oem hook after EarlyChipsetInit
#
# Input:  Stack NOT available
#        MMX3 = routine return address
#
# Output:  None
#
# Modified:  All,
#
#----------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(EarlyChpsetInit_OemHook)
ASM_PFX(EarlyChpsetInit_OemHook):
 #platform OEM code

#ifndef MINIBIOS_BUILD
  movb $CMOS_STATUS, %al
  outb %al, $CMOS_INDEX
  inb  $CMOS_DATA, %al

  andb $(CMOS_CHECKSUMSTS | CMOS_RTCSTS), %al
  jz CmosGood
  movb PcdGet8 (PcdSerialDbgLvlAtBadCmos), %bl
  xorl %edx, %edx                       # Serial debug message level data default = 00
  cmpb $01, %bl
  jnz 1f
  movl $0x80000000, %edx                # Minimum level
  jmp UpdateData
1:
  cmpb $02, %bl
  jnz 2f
  movl $0x80000042, %edx                # Normal level
  jmp UpdateData
2:
  cmpb $03, %bl
  jnz UpdateData
  movl $0xFFFFFFFF, %edx                # Maximum level

UpdateData: 
  movl $4, %ecx
  movb PcdGet8 (PcdCmosDebugPrintLevelReg), %al
3:
  outb %al, $CMOS_INDEX
  rorl $16, %eax
  movb %dl, %al
  outb %al, $CMOS_DATA
  shrl $8, %edx
  rorl $16, %eax
  incb %al
  loopl 3b

CmosGood: 
#endif

 #the following code is RET_ESI_MMX3
  movd  %mm3, %esi                      # restore return address from MM3
  jmp   *%esi                           # Absolute jump

#----------------------------------------------------------------------------
#
# Procedure:  LateChipsetInit_OemHook
#
# Description:  This routine provide oem hook after LateChipsetInit
#
# Input:  none
#
# Output:  None
#
# Modified:  All, except EBP and ESP
#
#----------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(LateChipsetInit_OemHook)
ASM_PFX(LateChipsetInit_OemHook):
 #platform OEM code
  ret


#----------------------------------------------------------------------------
#  STARTUP_SEG  S E G M E N T  ENDS
#----------------------------------------------------------------------------
