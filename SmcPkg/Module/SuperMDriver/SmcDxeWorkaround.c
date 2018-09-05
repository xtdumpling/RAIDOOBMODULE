//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.02
//    Bug Fix : Add OnBoard LAN drop check function.
//    Reason  : Remove LAN DID check.
//    Auditor : Kasber Chen
//    Date    : Jul/31/2017
//
//  Rev. 1.01
//    Bug Fix : Stopped TCO timer under EFI shell to avoid the system being 
//              rebooted since some EFI shell applications need more time to 
//              finish specific task.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jul/06/2017
//
//  Rev 1.00
//    Bug Fix : Added B2 workaround and disabled it by default..
//    Reason  :
//    Auditor : Joe Jhang
//    Date    : Jun/01/2017
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include <SspTokens.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AmiSmbios.h>
#include <Setup.h>
#include <Register/PchRegsPmc.h>
#include "SmcLib.h"
#include "SmcCspLib.h"

EFI_EVENT  mFeedEvent;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ReloadTCOTimer
//
// Description: Prevents WDT timeout by restarting it.
//
// Input:       None
//
// Output:      Nothing
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ReloadTCOTimer(
    IN  EFI_EVENT       Event,
    IN  VOID            *Context
)
{
    EFI_STATUS  Status;

    if((IoRead16(TCO_BASE_ADDRESS + R_PCH_TCO1_CNT) & 0x0800) == 0x0800){
        DEBUG((-1, "SMC TCO timer halt\n"));
        Status = gBS->SetTimer(mFeedEvent, TimerCancel, 0);
	Status = gBS->CloseEvent(mFeedEvent);
	return Status;
    }
//1. Set TCO Timer Reload and Current Value to 95 sec.
    IoWrite8(TCO_BASE_ADDRESS, 0x5F);

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   StopTCOTimer
//
// Description: Stops timer and event that kept on feeding watchdog.
//
// Input:       None
//
// Output:      Nothing
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
StopTCOTimer(
    IN  EFI_EVENT       Event,
    IN  VOID            *Context
)
{
    DEBUG((-1, "StopTCOTimer entry.\n"));
    gBS->CloseEvent(Event);
//1. Stop count down.
    gBS->SetTimer(mFeedEvent, TimerCancel, 0);
    IoWrite16(TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, IoRead16(TCO_BASE_ADDRESS + R_PCH_TCO1_CNT) | BIT11);
    DEBUG((-1, "StopTCOTimer end.\n"));
    return EFI_SUCCESS;
}

//<SMC_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcDxeB2Workaround
//
// Description : Supermicro platform workaround in DXE phase
//
// Parameters  : None
//
// Returns     :  None
//
//----------------------------------------------------------------------------
//<SMC_PHDR_START>
VOID
SmcDxeB2Workaround(void)
{
    EFI_STATUS  Status;
    EFI_EVENT   BootEvent;
    VOID        *Registration;

    DEBUG((-1, "SmcDxeB2Workaround entry.\n"));

//1. Set TCO Timer Initial Value to 95 sec.
// Don't change Timer vaule, legacy code check it for legacy IRQ 0 workaround
    IoWrite8(TCO_BASE_ADDRESS + R_PCH_TCO_TMR, 0x5F);
    IoWrite8(TCO_BASE_ADDRESS, 0x5F);
    IoWrite16(TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, (IoRead16(TCO_BASE_ADDRESS + R_PCH_TCO1_CNT) & (~0x800)));

    mFeedEvent = NULL;

//
// Creates Timer event to reload TCO timer.
//
    gBS->CreateEvent(
                    EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    ReloadTCOTimer,
                    NULL,
                    &mFeedEvent);

    gBS->SetTimer(
                    mFeedEvent,
                    TimerPeriodic,
                    (10 * 1000 * 1000));        // one second

//
// Closes Timer event before boot.
//
    Status = gBS->CreateEvent(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    StopTCOTimer,
                    NULL,
                    &BootEvent);

    if(!Status){
        Status = gBS->RegisterProtocolNotify(
                        &gAmiTseEventBeforeBootGuid,
                        BootEvent,
                        &Registration);
    }

    Status = gBS->CreateEvent(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    StopTCOTimer,
                    NULL,
                    &BootEvent);

    if(!Status){
        Status = gBS->RegisterProtocolNotify(
                        &gAmiLegacyBootProtocolGuid,
                        BootEvent,
                        &Registration);
    }

    DEBUG((-1, "SmcDxeB2Workaround end.\n"));
    return;
}

VOID
OnBoardLanCheck(
    IN  EFI_EVENT       Event,
    IN  VOID            *Context
)
{
    EFI_STATUS  Status;
    AMI_SMBIOS_PROTOCOL *SmbiosProtocol;
    UINT8       *Structure, i, j;
    UINT8       CapOffset, CapId = 0, Bus, Dev, Fun;
    UINT16      StructureSize, LinkCap, LinkSta;

    DEBUG((-1, "OnBoardLanCheck entry.\n"));

    gBS->CloseEvent(Event);

    Status = gBS->LocateProtocol(
                    &gAmiSmbiosProtocolGuid,
                    NULL,
                    &SmbiosProtocol);

    for(i = 1; !EFI_ERROR(Status); i++){
        Status = SmbiosProtocol->SmbiosReadStrucByType(
                        41, // Type 41
                        i,  // Instance
                        &Structure,
                        &StructureSize);

        if(Status)      continue;

        Bus = ((SMBIOS_ONBOARD_DEV_EXT_INFO*)Structure)->BusNumber;
        Dev = ((SMBIOS_ONBOARD_DEV_EXT_INFO*)Structure)->DevFuncNumber >> 3;
        Fun = ((SMBIOS_ONBOARD_DEV_EXT_INFO*)Structure)->DevFuncNumber & 0x07;
        DEBUG((-1, "Bus %x, Dev %x, Fun %x.\n", Bus, Dev, Fun));
        if(*(UINT16*)MmPciAddress(0, Bus, Dev, Fun, 0x0A) != 0x0200)
            continue;   // not lan
        // start from 0x34
        CapOffset = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x34) & ~(BIT0 | BIT1);
        // 10 times max, CapId is not 0x00 and 0xff
        for(j = 0; (j < 10) && CapOffset && (CapId != 0xff); j++){
            CapId = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, CapOffset);
            DEBUG((-1, "CapId = %x.\n", CapId));
            if(CapId == 0x10){              // device capability searched
                LinkCap = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, CapOffset + 0x0C);
                LinkSta = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, CapOffset + 0x12);
                DEBUG((-1, "LinkCap = %x, LinkSta = %x.\n", LinkCap, LinkSta));
                if(LinkCap == LinkSta){
                    SetCmosTokensValue(Q_OnBoardLanCheck, 0);   //clear counter
                    break;                  // search next lan device
                } else {
                    if(GetCmosTokensValue(Q_OnBoardLanCheck) < 3){
                        DEBUG((-1, "LAN drop reset.\n"));
                        IoWrite8(0xcf9, 0x06);  // reset system
                    }
                }
            }
            CapOffset = *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, CapOffset + 1) & ~(BIT0 | BIT1);
        }
    }
    DEBUG((-1, "OnBoardLanCheck end.\n"));
}

VOID
SmcDxeOnboardLanWorkaround(void)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_EVENT   LanChkEvt;
    VOID        *LanChkReg;
    
    DEBUG((-1, "SmcDxeOnboardLanWorkaround entry.\n"));

    Status = gBS->CreateEvent(
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    OnBoardLanCheck,
                    NULL,
                    &LanChkEvt);

    if(!Status){
        Status = gBS->RegisterProtocolNotify(
                        &gExitPmAuthProtocolGuid,
                        LanChkEvt,
                        &LanChkReg);
    }

    DEBUG((-1, "SmcDxeOnboardLanWorkaround end.\n"));
    return;
}
