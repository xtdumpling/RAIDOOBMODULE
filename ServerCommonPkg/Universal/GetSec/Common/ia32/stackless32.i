#
# This file contains an 'Intel Peripheral Driver' and is      
# licensed for Intel CPUs and chipsets under the terms of your
# license agreement with Intel or your vendor.  This file may 
# be modified by the user, subject to additional terms of the 
# license agreement                                           
#
#------------------------------------------------------------------------------
#
# Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
#
# Module Name:
#
#   stackless32.inc
#
# Abstract:
#
#------------------------------------------------------------------------------

.macro MOVDIN0
 movd %ebx, %mm0  # db 00fh, 06eh, 0c3h
.endm

.macro MOVDOUT0
 movd %mm0, %ebx  # db 00fh, 07eh, 0c3h
.endm

.macro MOVDIN1
 movd %ebx, %mm1  # db 00fh, 06eh, 0cbh
.endm

.macro MOVDOUT1
 movd %mm1, %ebx  # db 00fh, 07eh, 0cbh
.endm

.macro MOVDIN2
 movd %ebx, %mm2  # db 00fh, 06eh, 0d3h
.endm

.macro MOVDOUT2
 movd %mm2, %ebx  # db 00fh, 07eh, 0d3h
.endm

.macro MOVDIN3
 movd %ebx, %mm3  # db 00fh, 06eh, 0dbh
.endm

.macro MOVDOUT3
 movd %mm3, %ebx  # db 00fh, 07eh, 0dbh
.endm

.macro MOVDIN4
 movd %ebx, %mm4  # db 00fh, 06eh, 0e3h
.endm

.macro MOVDOUT4
 movd %mm4, %ebx  # db 00fh, 07eh, 0e3h
.endm

.macro MOVDIN5
 movd %ebx, %mm5  # db 00fh, 06eh, 0ebh
.endm

.macro MOVDOUT5
 movd %mm5, %ebx  # db 00fh, 07eh, 0ebh
.endm

.macro MOVDIN6
 movd %ebx, %mm6  # db 00fh, 06eh, 0f3h
.endm

.macro MOVDOUT6
 movd %mm6, %ebx  # db 00fh, 07eh, 0f3h
.endm

.macro MOVDIN7
 movd %ebx, %mm7  # db 00fh, 06eh, 0fbh
.endm

.macro MOVDOUT7
 movd %mm7, %ebx  # db 00fh, 07eh, 0fbh
.endm

.macro _EMMS
 emms  # db 0fh, 77h
.endm

#-----  CALL_NS  ------------------------------------------------------------;
#                                                                            ;
#       Purpose:        Stackless "call" to destination procedure            ;
#                                                                            ;
#       Input:          routine to jump to                                   ;
#                                                                            ;
#       Output:         none                                                 ;
#                                                                            ;
#       Setup:          CR4 bit 9 must be set                                ;
#                                                                            ;
#       Uses:           esp, XMM4                                            ;
#                                                                            ;
#       Notes:          This macro allows the nesting of CALL_NS routines to ;
#                       be 3-deep.  DO NOT ATTEMPT TO GO DEEPER THAN THAT,   ;
#                       SINCE THE FIRST ADDRESS WILL BE LOST!!!              ;
#                                                                            ;
#----------------------------------------------------------------------------;
.macro CALL_NS dest

        movl    retaddr, %esp                   # get the return address

        # Shift XMM register left to make room for the WORD.
        .byte   0x66,0xf,0x73,0xfc,0x2          # PSLLDQ xmm4, 02h

        # Insert the word
        .byte   0x66,0xf,0xc4,0xe4,0x0          # PINSRW xmm4, esp, 00h

        # Do the above again, but for the high word of esp
        shrl    $16, %esp
        .byte   0x66,0xf,0x73,0xfc,0x2          # PSLLDQ xmm4, 02h
        .byte   0x66,0xf,0xc4,0xe4,0x0          # PINSRW xmm4, esp, 00h
        jmp     dest
retaddr:
.endm


#-----  RET_NS  -------------------------------------------------------------;
#                                                                            ;
#       Purpose:        Return to a stackless "call"                         ;
#                                                                            ;
#       Input:          none                                                 ;
#                                                                            ;
#       Output:         none                                                 ;
#                                                                            ;
#       Setup:          CR4 bit 9 must be set                                ;
#                                                                            ;
#       Uses:           esp, XMM4                                            ;
#                                                                            ;
#       Notes:          See the description for CALL_NS for details          ;
#                                                                            ;
#----------------------------------------------------------------------------;
.macro RET_NS
        # Extract the most significant word of return address
        .byte   0x66,0xf,0xc5,0xe4,0x0        # PEXTRW xmm4, esp, 00h
        # Shift XMM register right to setup the next WORD.
        .byte   0x66,0xf,0x73,0xdc,0x2        # PSRLDQ xmm4, 02h

        # Do the above again, except for the low word of esp
        shll    $16, %esp
        .byte   0x66,0xf,0xc5,0xe4,0x0        # PEXTRW xmm4, esp, 00h
        .byte   0x66,0xf,0x73,0xdc,0x2        # PSRLDQ xmm4, 02h
        jmp     esp
.endm
