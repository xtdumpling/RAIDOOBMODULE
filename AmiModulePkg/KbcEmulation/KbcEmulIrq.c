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

/** @file KbcEmulIrq.c
    Handles the IRQ1 and IRQ12 generation

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include "KbcEmul.h"
#include "KbcEmulLib.h"
#include "KbcDevEmul.h"

#if OHCI_EMUL_SUPPORT
#include "KbcOhci.h"
#endif

#if SB_EMUL_SUPPORT
#include "KbcSb.h"
#endif

//---------------------------------------------------------------------------

VOID GenerateIRQ12(VIRTKBC*);
VOID GenerateIRQ1(VIRTKBC*);

/**
    Generate IRQ1

    @param vkbc

    @retval VOID

**/
VOID
GenerateIRQ12 (
    VIRTKBC  *vkbc
)
{

#if UHCI_EMUL_SUPPORT || IOTRAP_EMUL_SUPPORT
    ByteReadIO(SW_IRQ_GENERATION_REG);
    ByteWriteIO(SW_IRQ_GENERATION_REG, 0);
    ByteWriteIO(SW_IRQ_GENERATION_REG, 2);
    return;
#endif

#if OHCI_EMUL_SUPPORT
    if (GetHceStatus() & HCE_STS_OUTPUTFULL ){
        SetHceControl((GetHceControl() | HCE_CNTRL_IRQ_ENABLE));
    } else {
        SetHceStatus((GetHceStatus() | HCE_STS_OUTPUTFULL | HCE_STS_AUXOUTPUTFULL));
        SetHceControl((GetHceControl() | HCE_CNTRL_IRQ_ENABLE));
    }
    return;
#endif

#if SB_EMUL_SUPPORT
    GenerateSbIRQ12();
    return;
#endif

}

/**
    Generate IRQ1

    @param vkbc

    @retval VOID

**/
VOID
GenerateIRQ1 (
    VIRTKBC  *vkbc
)
{

#if UHCI_EMUL_SUPPORT || IOTRAP_EMUL_SUPPORT
    ByteReadIO(SW_IRQ_GENERATION_REG);
    ByteWriteIO(SW_IRQ_GENERATION_REG, 0);
    ByteWriteIO(SW_IRQ_GENERATION_REG, 1);
    return;
#endif
#if OHCI_EMUL_SUPPORT
    if (GetHceStatus() & HCE_STS_OUTPUTFULL ){
        SetHceControl((GetHceControl() | HCE_CNTRL_IRQ_ENABLE));
    } else {
        vkbc->st_ |= KBC_STATUS_OBF;
        SetHceStatus((GetHceStatus() | KBC_STATUS_OBF));
        SetHceControl((GetHceControl() | HCE_CNTRL_IRQ_ENABLE));
    }
    return;
#endif

#if SB_EMUL_SUPPORT
    GenerateSbIRQ1();
    return;
#endif
}

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

