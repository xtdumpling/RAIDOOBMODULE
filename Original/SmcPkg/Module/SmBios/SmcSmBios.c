//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.07
//    Bug Fix:  Don't update SMBIOS when data from BMC is empty.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Mar/14/2017
//
//  Rev. 1.06
//    Bug Fix:  Move SmcEDMI code to SmcSmBios.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Feb/06/2017
//
//  Rev. 1.05
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/06/2017
//
//  Rev. 1.04
//    Bug Fix:  Add extra 0x100 memory size for update SMBIOS data.
//    Reason:   
//    Auditor:  Wayne Chen
//    Date:     Oct/13/2016
//
//  Rev. 1.03
//    Bug Fix:  Add VPD data to type 40.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/26/2016
//
//  Rev. 1.02
//    Bug Fix:  Add type 16 and type 17, define type token.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/20/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix SMBIOS update incorrectly.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/06/2016
//
//  Rev. 1.00
//    Bug Fix:  Renew the SMC ROM Hole.
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/04/2016
//
//****************************************************************************
//****************************************************************************
#include <token.h>
#include <AmiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SMBios.h>
#include <Protocol/AmiSmbios.h>
#include <Protocol/SmbiosDynamicData.h>
#include "SmcSmBios.h"
#include "PlatformSmBios.h"
#if SmcRomHole_SUPPORT
#include "SmcRomHole.h"
#endif

AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;
extern	SMC_SMBIOS_UPDATE_TABLE            SmcSmBiosUpdateTable[];
UINT8	SMB_Func, Instance, Temp_Instance;

VOID
SmcSmBiosInit(
    IN	EFI_EVENT	Event,
    IN	VOID		*Context
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	i, *TempBuffer = NULL, *Buffer = NULL;
    UINT16	Handle, TempBufferSize;

    DEBUG((-1, "SmcSmBios callback function entry\n"));
    
    gBS->CloseEvent(Event);

    Status = gBS->LocateProtocol(
                    &gAmiSmbiosProtocolGuid,
                    NULL,
                    &SmbiosProtocol);

    if(Status)	return;

    for(i = 0; SmcSmBiosUpdateTable[i].SmBiosUpdateType != 0xff; i++){
        Status = EFI_SUCCESS;
        Temp_Instance = 0;
        for(Instance = 1; !EFI_ERROR(Status); Instance++){
            Status = SmbiosProtocol->SmbiosReadStrucByType(
                    SmcSmBiosUpdateTable[i].SmBiosUpdateType,
                    Instance,
                    &TempBuffer,
                    &TempBufferSize);
            DEBUG((-1, "SmbiosReadStrucByType = %r. TempBufferSize = %x\n", Status, TempBufferSize));
            if(!Status){
                SMB_Func = WriteStructure;	//default
                Handle = ((SMBIOS_STRUCTURE_HEADER *)TempBuffer)->Handle;
		
                Status = gBS->AllocatePool (EfiBootServicesData, TempBufferSize + 0x100, &Buffer);
                if (EFI_ERROR (Status)) {
                    gBS->FreePool(TempBuffer);
                    continue;
                }
                MemCpy(Buffer, TempBuffer, TempBufferSize);
                gBS->FreePool(TempBuffer);

                SmcSmBiosUpdateTable[i].SmBiosUpdateFn(
                        Handle,
                        (VOID*)Buffer,
                        &TempBufferSize);

                if(SMB_Func == 1){
                    DEBUG((-1, "Write BufferSize = %x\n", TempBufferSize));
                    Status = SmbiosProtocol->SmbiosWriteStructure(
                            Handle,
                            Buffer,
                            TempBufferSize);
                    DEBUG((-1, "SmbiosWriteStructure = %r, Handle = %x.\n", Status, Handle));
                }
                else if(SMB_Func == 2){
                    Status = SmbiosProtocol->SmbiosDeleteStructure(Handle);
                    DEBUG((-1, "SmbiosDeleteStructure = %r, Handle = %x.\n", Status, Handle));
                }
                gBS->FreePool(Buffer);
            }
        }
    }

    DEBUG((-1, "Exit SmcSmBios callback function\n"));
    return;
}

#if SmcRomHole_SUPPORT
VOID
SmcEDMIInit(
    IN  EFI_EVENT       Event,
    IN  VOID            *Context
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    CHAR16      *DmiArrayVar = L"DmiArray", *S1 = L"                ";
    DMI_VAR     DmiArray[DMI_ARRAY_COUNT] = {0};
    UINTN       DmiArraySize = DMI_ARRAY_COUNT * sizeof(DMI_VAR);
    UINTN       Offset = sizeof(SMC_PRESERVE_TABLE);
    UINT8       Buffer[0x400], *SMB_SMC, *SMB_NV, VarSize = sizeof(DMI_VAR), Index;
    UINT32      TempSize;
    UINT64      BufferSize;

    DEBUG((-1, "SmcEDMIInit Entry\n"));

    gBS->CloseEvent(Event);

    Status = gBS->LocateProtocol(
                    &gAmiSmbiosProtocolGuid,
                    NULL,
                    &SmbiosProtocol);

    if(Status)  return;

    TempSize = SmcRomHoleSize(SMBIOS_FOR_BMC_REGION);
    Status = gBS->AllocatePool(EfiBootServicesData, TempSize, &SMB_SMC);
    if(Status)  return;
    
    Status = gBS->AllocatePool(EfiBootServicesData, TempSize, &SMB_NV);
    if(Status)  return;
    
    Status = SmcRomHoleReadRegion(SMBIOS_FOR_BMC_REGION, SMB_SMC);
    if(Status){
        gBS->FreePool(SMB_SMC);
        gBS->FreePool(SMB_NV);
        return;
    }
//if $SMC in SMCROMHOLE, count not 0 or 0xff, flag is 1, restore DMI from SMCROMHOLE
    if(!MemCmp(SMB_SMC, "$SMC", 4) && (((SMC_PRESERVE_TABLE*)SMB_SMC)->Flags == 0x01)){
        DEBUG((-1, "Restore DMI from SMCROMHOLE.\n"));
        DmiArray[0].Type = (UINT8)((SMC_PRESERVE_TABLE*)SMB_SMC)->Count;
        if((!DmiArray[0].Type) || (DmiArray[0].Type == 0xff)){
            DEBUG((-1, "No any smbios changed.\n"));
            ((SMC_PRESERVE_TABLE*)SMB_SMC)->Flags = 0;
            SmcRomHoleWriteRegion(SMBIOS_FOR_BMC_REGION, SMB_SMC);
            return;
        }

        for(Index = 0; (Index < DmiArray[0].Type) && !Status; Index++){
            MemCpy(&DmiArray[Index + 1], SMB_SMC + Offset, sizeof(DMI_VAR));
            Swprintf(S1, L"DmiVar%02x%04x%02x%02x",
                            DmiArray[Index + 1].Type,
                            DmiArray[Index + 1].Handle,
                            DmiArray[Index + 1].Offset,
                            DmiArray[Index + 1].Flags);

            Offset += sizeof(DMI_VAR);
            BufferSize = SMB_SMC[Offset];
            Offset += sizeof(BufferSize);
            MemCpy(Buffer, &SMB_SMC[Offset], BufferSize);
            Status = gRT->SetVariable(
                            S1, &gAmiSmbiosNvramGuid,
                            EFI_VARIABLE_BOOTSERVICE_ACCESS |
                            EFI_VARIABLE_NON_VOLATILE |
                            EFI_VARIABLE_RUNTIME_ACCESS,
                            BufferSize,
                            Buffer);
            DEBUG((-1, "Greate DmiVar Status = %r.\n", Status));
            Offset += BufferSize;
        }
        Status = gRT->SetVariable(
                        DmiArrayVar,
                        &gAmiSmbiosNvramGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS |
                        EFI_VARIABLE_NON_VOLATILE |
                        EFI_VARIABLE_RUNTIME_ACCESS,
                        DmiArraySize,
                        &DmiArray);
        DEBUG((-1, "Set DmiArrayVar %r.\n", Status));
//clear flags
        ((SMC_PRESERVE_TABLE*)SMB_SMC)->Flags = 0;
        SmcRomHoleWriteRegion(SMBIOS_FOR_BMC_REGION, SMB_SMC);
        IoWrite8(0xcf9, 0x06);
    } else {    // Save system DMI to SMCROMHOLE
        DEBUG((-1, "Save system DMI to SMCROMHOLE.\n"));
        gBS->SetMem(SMB_NV, TempSize, 0xFF);
        MemCpy(SMB_NV, "$SMC", 4);
        Status = gRT->GetVariable(
                        DmiArrayVar,
                        &gAmiSmbiosNvramGuid,
                        NULL,
                        &DmiArraySize,
                        &DmiArray[0]);
//check any one changed DMI by DMI tool.
        if(!Status){     //changed
            DEBUG((-1, "DMI is changed by DMI tool.\n"));
            if((!DmiArray[0].Type) || (DmiArray[0].Type == 0xff)){
                DEBUG((-1, "DMI structure is wrong.\n"));
                gBS->FreePool(SMB_SMC);
                gBS->FreePool(SMB_NV);
                return;
            }
            for(Index = 0; Index < DmiArray[0].Type; Index++){
                DEBUG((-1, "Get DmiArray[%x].\n", Index + 1));
                Swprintf(S1, L"DmiVar%02x%04x%02x%02x",
                                DmiArray[Index + 1].Type,
                                DmiArray[Index + 1].Handle,
                                DmiArray[Index + 1].Offset,
                                DmiArray[Index + 1].Flags);

                BufferSize = sizeof(Buffer);
                Status = gRT->GetVariable(
                                S1,
                                &gAmiSmbiosNvramGuid,
                                NULL,
                                &BufferSize,
                                &Buffer);
                if(Status){
                    DEBUG((-1, "Get DmiArray[%x] fail, Status = %r.\n", Index + 1, Status));
                    (DmiArray[0].Type)--;
                    continue;
                }
                MemCpy(SMB_NV + Offset, &DmiArray[Index + 1], VarSize);
                Offset += VarSize;
                MemCpy(SMB_NV + Offset, &BufferSize, sizeof(BufferSize));
                Offset += sizeof(BufferSize);
                MemCpy(SMB_NV + Offset, Buffer, BufferSize);
                Offset += BufferSize;
            }
            //calculate checksum
            ((SMC_PRESERVE_TABLE*)SMB_NV)->CHK = 0;
            for(Index = sizeof(SMC_PRESERVE_TABLE); Index < (UINT8)Offset; Index++)
                ((SMC_PRESERVE_TABLE*)SMB_NV)->CHK += SMB_NV[Index];

            ((SMC_PRESERVE_TABLE*)SMB_NV)->Count = DmiArray[0].Type;
            ((SMC_PRESERVE_TABLE*)SMB_NV)->Size = (UINT16)Offset;
        }
//if checksum is different with SMCROMHOLE, Save to SMCROMHOLE
        if(((SMC_PRESERVE_TABLE*)SMB_NV)->CHK != ((SMC_PRESERVE_TABLE*)SMB_SMC)->CHK){
            DEBUG((-1, "Checksum is different, Save to SMCROMHOLE.\n"));
            SmcRomHoleWriteRegion(SMBIOS_FOR_BMC_REGION, SMB_NV);
        }
    }

    DEBUG((-1, "SmcEDMIInit End\n"));
    return;
}
#endif

EFI_STATUS
SmcSmBiosEntryPoint(
    IN	EFI_HANDLE		ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_EVENT	SmcSmBiosEvt, SmcEDMIEvt;
    VOID	*SmcSmBiosReg, *SmcEDMIReg;

    DEBUG((-1, "SmcSmBiosEntryPoint entry.\n"));

    Status = gBS->CreateEvent(
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    SmcSmBiosInit,
                    NULL,
                    &SmcSmBiosEvt);

    if(!Status){
  	Status = gBS->RegisterProtocolNotify(
                    &gBdsAllDriversConnectedProtocolGuid,
                    SmcSmBiosEvt,
                    &SmcSmBiosReg);
    }
#if SmcRomHole_SUPPORT
    Status = gBS->CreateEvent(
                        EVT_NOTIFY_SIGNAL,
                        TPL_CALLBACK,
                        SmcEDMIInit,
                        NULL,
                        &SmcEDMIEvt);

    if(!Status){
        Status = gBS->RegisterProtocolNotify(
                        &gBdsAllDriversConnectedProtocolGuid,
                        SmcEDMIEvt,
                        &SmcEDMIReg);
    }
#endif
    DEBUG((-1, "SmcSmBiosEntryPoint end.\n"));
    return Status;
}
