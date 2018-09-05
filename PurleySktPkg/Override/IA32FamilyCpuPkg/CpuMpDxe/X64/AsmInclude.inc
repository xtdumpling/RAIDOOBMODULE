;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;******************************************************************************
;*
; Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.

;*
;******************************************************************************

VacantFlag                    Equ   00h
NotVacantFlag                 Equ   0ffh
StartupApSignal               Equ   6E750000h
;
; PURLEY_OVERRIDE_BEGIN
;
MonitorFilterSize             Equ   40h
;
; PURLEY_OVERRIDE_END
;
ApInHltLoop                   Equ   1
ApInMwaitLoop                 Equ   2
ApInRunLoop                   Equ   3

LockLocation                  equ        (RendezvousFunnelProcEnd - RendezvousFunnelProcStart)
StackStartAddressLocation     equ        (LockLocation + 08h)
StackSizeLocation             equ        (LockLocation + 10h)
CProcedureLocation            equ        (LockLocation + 18h)
GdtrLocation                  equ        (LockLocation + 20h)
IdtrLocation                  equ        (LockLocation + 2Ah)
BufferStartLocation           equ        (LockLocation + 34h)
Cr3OffsetLocation             equ        (LockLocation + 38h)
InitFlagLocation              equ        (LockLocation + 3Ch)
ApCountLocation               equ        (LockLocation + 40h)
DcuModeSelectFlagLocation     equ        (LockLocation + 44h)
DcuModeLocation               equ        (LockLocation + 45h)
ApLoopModeLocation            equ        (LockLocation + 48h)
EnableExecuteDisable          equ        (LockLocation + 4Ch)
BistBuffer                    equ        (LockLocation + 50h)

;-------------------------------------------------------------------------------
