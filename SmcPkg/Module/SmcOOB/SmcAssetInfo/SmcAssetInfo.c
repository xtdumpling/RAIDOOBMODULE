//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.12
//    Bug Fix : N/A
//    Reason  : Read SATA device configuration by Devices protocol.
//    Auditor : Durant Lin
//    Date    : Jun/01/2018
//
//  Rev. 1.11
//    Bug Fix : N/A
//    Reason  : Use Oob Library Protocol to replace some library functions.
//    Auditor : Durant Lin
//    Date    : Feb/12/2018
//
//  Rev. 1.10
//    Bug Fix : N/A
//    Reason  : Add a version control string for all OOB EFI files. "_SMCOOBV1.xx.xx_"
//    Auditor : Durant Lin
//    Date    : Jan/04/2018
//
//  Rev. 1.09
//    Bug Fix : N/A
//    Reason  : Isolate PciLib from OOB to OobLibraryProtocol, since different
//              platform will cause hang up issue.
//    Auditor : Durant Lin
//    Date    : Jun/02/2018
//
//  Rev. 1.08
//    Bug Fix:  [Assetinfo] Fixed HDD information "Serial number", "Firmware Revision" and "Model Number" missing one ASCII character.
//    Reason:   Add 1 byte for ModelNumber, SerialNumber, FW_Rev 
//    Auditor:  Jimmy Chiu (Refer Greenlow - SVN_2191)
//    Date:     Mar/27/2017
//
//  Rev. 1.07
//    Bug Fix:  Fix GetSataInfo errors of sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/28/2016
//
//  Rev. 1.06
//    Bug Fix:  Fix some errors for upload and download OOB files.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/20/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix system hanging when enabling debug mode.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/13/2016
//
//  Rev. 1.04
//    Bug Fix:  Fix OOB data didn't be sent to BMC.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/03/2016
//
//  Rev. 1.03
//    Bug Fix:  Skip SmcOutBand and SmcAssetInfo when disabling JPG1.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/12/2016
//
//  Rev. 1.02
//    Bug Fix:  Enable SmcAssetInfo module and change IPMI command.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/10/2016
//
//  Rev. 1.01
//    Bug Fix:  Add Full SMBIOS module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:   From Grantley.
//    Auditor:  Kasber Chen
//    Date:     May/25/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcAssetInfo.c
//
// Description: SMC In-Band feature include BIOS config and SMBIOS.
//
//<AMI_FHDR_END>
//**********************************************************************
#include "Token.h"
#include <AmiDxeLib.h>
#include "Setup.h"
#include <Protocol/DevicePath.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskInfo.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#include <Protocol/IPMITransportProtocol.h>
#include <AmiProtocol.h>
//#include <Library/PcdLib.h>
#include "Library\BaseCryptLib.h"
#include "SmcCspLib.h"
#include "SmcLib.h"
#include "SmcAssetInfo.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include <Protocol/SmcIpmiOemCommandSetProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
//#include <Library/PciLib.h>
#include <Protocol/SmcOobLibraryProtocol.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include "SmcOobVersion.h"
#include "OobLibrary.h"

SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *mSmcIpmiOemCommandSetProtocol;
SMC_OOB_PLATFORM_POLICY           *mSmcOobPlatformPolicyPtr;
SMC_OOB_LIBRARY_PROTOCOL		  *mSmcOobLibraryProtocol;
VOID
StringOptimize(
    IN	CHAR8	*Data,
    IN	UINT8	length
)
{
    UINT8  ii, jj = 0;
    CHAR8  cl, tmp[100];

    pBS->SetMem(tmp, length, 0x20);
    for(ii = 0; ii < length; ii++) {
        if(Data[ii] != ' ')	break;
    }

    for( ; ii < length; ii++) {
        cl = Data[ii];
        if(cl) {
            tmp[jj] = cl;
            jj++;
        }
    }
    pBS->SetMem(Data, length, 0x20);
    pBS->CopyMem(Data, tmp, length);
    Data[length - 1] = '\0';
}

VOID
SwapEntries(
    IN	CHAR8	*Data,
    IN	UINT16	Size
)
{
    UINT16	Index;
    CHAR8	Temp8;

    for(Index = 0; (Index + 1) < Size; Index += 2) {
        Temp8 = Data[Index];
        Data[Index] = Data[Index + 1];
        Data[Index + 1] = Temp8;
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SMC_Get_SATA_Mode
//
// Description: 
//  Get SATA Interface Mode
//
// Input:
//      None
//
// Output:
//      UINT8 (return) -
//          1 : Raid mode
//          0 : AHCI mode
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8
SMC_Get_SATA_Mode(
  IN	UINT8	Bus,
  IN	UINT8	Device,
  IN	UINT8	Function
){
    UINT32	TempData32;

	TempData32 = mSmcOobLibraryProtocol->Smc_PciRead32(Bus,Device,Function,R_PCH_SATA_SATAGC);
    //TempData32 = PciRead32(PCI_LIB_ADDRESS(0, 23, 0, 0x9C));
    if(TempData32 & BIT16)
	return 1;
    else
	return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SmcAssetInfoCallBack
//
// Description: Run at ready to boot.
//
// Input:       IN EFI_EVENT   Event
//              IN VOID        *Context
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
SmcAssetInfoCallBack(
    IN	EFI_EVENT	Event,
    IN	VOID		*Context
)
{
    EFI_STATUS	Status;
    EFI_IPMI_TRANSPORT	*gIpmiTransport;
    UINT8	*pTemp = NULL, *pCurrent;
    UINT16	i = 0, TotalSize, Retry = 0, HddCount = 0;
    UINT8	CompletionCode, CompletionCodeSize = sizeof(CompletionCode);
    PCI_DEVICE_PATH                 *PciDevicePath;
    UINT8                           Index,CheckSum;
    UINTN                           HandleCount;
    EFI_HANDLE                      *HandleBuffer;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePath, *DevicePathNode;
    EFI_DISK_INFO_PROTOCOL          *DiskInfo;
    UINT32                          IdeDevice, IdeChannel;
    IDENTIFY_DATA                   *IdentifyDriveInfo = NULL;
    UINT32                          BufferSize = 0;
    CHAR8                           ModelNumber[40+1];
    CHAR8                           SerialNumber[20+1];
    CHAR8                           FW_Rev[8+1];
    UINT64                          PciAddress=0;
    UINT8                           IdeControllerLocation[] = {0,0x17,0x0};
    UINT8                           IdeController2Location[] = {0,0x11,0x5};
    UINT8 Digest[MD5_DIGEST_SIZE], OldDigest[MD5_DIGEST_SIZE];
    UINTN DigestLen = MD5_DIGEST_SIZE;
    UINT32 OobFileStatus0 = 0;
    UINT8 FileRetry = OOB_IPMI_FILE_RETRY;
    UINT8   						SATA_Interface_Setting = 0;
    UINT8 ResponseStatus;

    DEBUG((-1, "SmcAssetInfoCallBack Start.\n"));

    Status = pBS->AllocatePool(EfiBootServicesData, 0x1000 * 3, &pTemp);
    if(EFI_ERROR(Status)) {
        pBS->CloseEvent(Event);
        return;
    }

    Status = pBS->AllocatePool(
                 EfiBootServicesData,
                 sizeof(IDENTIFY_DATA) * 3,
                 (void**)&IdentifyDriveInfo);
    if(EFI_ERROR(Status)) {
        pBS->FreePool (pTemp);
        pBS->CloseEvent(Event);
        return;
    }

    pCurrent = pTemp;
    pCurrent = pCurrent + sizeof(Asset_Head_Info);

    pBS->SetMem(pTemp, 0x1000, 0x00);

    PciDevicePath = NULL;

    Status = pBS->LocateHandleBuffer(
                 ByProtocol,
                 &gEfiDiskInfoProtocolGuid,
                 NULL,
                 &HandleCount,
                 &HandleBuffer);

    if(EFI_ERROR(Status))
        HandleCount = 0;

    for(Index = 0; Index < HandleCount; Index++) {
        Status = pBS->HandleProtocol(
                     HandleBuffer[Index],
                     &gEfiDevicePathProtocolGuid,
                     (VOID*)&DevicePath);
        if(EFI_ERROR(Status)) continue;

        DevicePathNode = DevicePath;
        while(!isEndNode(DevicePathNode)) {
            if((DevicePathNode->Type == HARDWARE_DEVICE_PATH) &&
                    (DevicePathNode->SubType == HW_PCI_DP)) {
                PciDevicePath = (PCI_DEVICE_PATH*)DevicePathNode;
                break;
            }

            DevicePathNode = NEXT_NODE(DevicePathNode);
        }

        if(PciDevicePath == NULL)	continue;

        if(((PciDevicePath->Device == IdeControllerLocation[1]) &&
                (PciDevicePath->Function == IdeControllerLocation[2])) ||
                ((PciDevicePath->Device == IdeController2Location[1]) &&
                (PciDevicePath->Function == IdeController2Location[2]))) {
    		
			SATA_Interface_Setting = SMC_Get_SATA_Mode(0x0,PciDevicePath->Device,PciDevicePath->Function);

            Status = pBS->HandleProtocol(
                         HandleBuffer[Index],
                         &gEfiDiskInfoProtocolGuid,
                         (void**)&DiskInfo);
            if(EFI_ERROR(Status)) continue;

            Status = DiskInfo->WhichIde(
                         DiskInfo,
                         &IdeChannel,
                         &IdeDevice);
            if(EFI_ERROR(Status)) continue;

            pBS->SetMem(IdentifyDriveInfo, sizeof(IDENTIFY_DATA), 0);

            BufferSize = sizeof(IDENTIFY_DATA);
            Status = DiskInfo->Identify(
                         DiskInfo,
                         IdentifyDriveInfo,
                         &BufferSize);
            if(EFI_ERROR(Status)) continue;

        } else {
            //
            // Off board
            //
            continue;
        }

        pBS->SetMem(ModelNumber, 40+1, 0x20);
        pBS->SetMem(SerialNumber, 20+1, 0x20);
        pBS->SetMem(FW_Rev, 8+1, 0x20);
        pBS->CopyMem(ModelNumber, IdentifyDriveInfo->Model_Number_27, 40);
        pBS->CopyMem(SerialNumber, IdentifyDriveInfo->Serial_Number_10, 20);
        pBS->CopyMem(FW_Rev, IdentifyDriveInfo->Firmware_Revision_23, 8);
        SwapEntries(ModelNumber, 40);
        SwapEntries(SerialNumber, 20);
        SwapEntries(FW_Rev, 8);
        StringOptimize(ModelNumber, 40+1);
        StringOptimize(SerialNumber, 20+1);
        StringOptimize(FW_Rev, 8+1);
// Reserve 4 bytes for Instance->Length
//	pCurrent = pCurrent + 4;
        ((Instance*)pCurrent)->Length = sizeof(Instance) - 2;
// Fill Hdd_Info_Item information
        ((Instance*)pCurrent)->HDD_Tier1.Tier_ID = 1;
        ((Instance*)pCurrent)->HDD_Tier1.Length = sizeof(Tier1) - 3;
        ((Instance*)pCurrent)->HDD_Tier1.Item1 = 1;
        ((Instance*)pCurrent)->HDD_Tier2.Tier_ID = 2;
        ((Instance*)pCurrent)->HDD_Tier2.Length = sizeof(Tier2) - 3;
        ((Instance*)pCurrent)->HDD_Tier2.Item1 = 1;
        ((Instance*)pCurrent)->HDD_Tier3.Tier_ID = 3;
        ((Instance*)pCurrent)->HDD_Tier3.Length = sizeof(Tier3) - 3;
        ((Instance*)pCurrent)->HDD_Tier3.Item2 = 2;
        ((Instance*)pCurrent)->HDD_Tier3.Item3 = 3;
        ((Instance*)pCurrent)->HDD_Tier3.Item4 = 4;
        ((Instance*)pCurrent)->HDD_Tier3.Item5 = 5;
        ((Instance*)pCurrent)->HDD_Tier3.Item6 = 6;
        ((Instance*)pCurrent)->HDD_Tier3.Item7 = 7;
        if(SATA_Interface_Setting)	//SATA_MODE_RAID
            AsciiStrCpy(((Instance*)pCurrent)->HDD_Tier2.Config_Type , "RAID");
        else				//SATA_MODE_AHCI
            AsciiStrCpy(((Instance*)pCurrent)->HDD_Tier2.Config_Type , "AHCI");

        AsciiStrCpy(((Instance*)pCurrent)->HDD_Tier1.Controller_Name , "PCH SATA");
        ((Instance*)pCurrent)->HDD_Tier3.Slot_ID = IdeChannel;
        ((Instance*)pCurrent)->HDD_Tier3.Slot_Populated = 1;
        pBS->CopyMem(&((Instance*)pCurrent)->HDD_Tier3.Model_Name, ModelNumber, 40+1);
        pBS->CopyMem(&((Instance*)pCurrent)->HDD_Tier3.SN, SerialNumber, 20+1);
        pBS->CopyMem(&((Instance*)pCurrent)->HDD_Tier3.FW_Rev, FW_Rev, 8+1);
        if((IdentifyDriveInfo->Command_Set_Supported_82 & 0x01) == 0x01)
            ((Instance*)pCurrent)->HDD_Tier3.Smart_Support = 1;

        HddCount++;
        pCurrent = pCurrent + sizeof(Instance);
    }

// Fill Asset_Head_Info
    ((Asset_Head_Info*)pTemp)->Major_Version = 1;
    ((Asset_Head_Info*)pTemp)->Minor_Version = mSmcOobPlatformPolicyPtr->OobConfigurations.AssetInfoVersion;
    ((Asset_Head_Info*)pTemp)->Count = HddCount;
    ((Asset_Head_Info*)pTemp)->Length = (UINT32)(pCurrent - (pTemp + sizeof(Asset_Head_Info)));

    CheckSum = 0;
    pCurrent = pTemp + sizeof(Asset_Head_Info);
    for(i = 0; i < ((Asset_Head_Info*)pTemp)->Length; i++)
        CheckSum = CheckSum + *(pCurrent + i);

    ((Asset_Head_Info*)pTemp)->Playload_Checksum = CheckSum;

    CheckSum = 0;
    for(i = 0; i < sizeof (Asset_Head_Info) - 1; i++)
        CheckSum = CheckSum + *(pTemp + i);

    ((Asset_Head_Info*)pTemp)->Header_Checksum = CheckSum;

    TotalSize = (UINT16)(((Asset_Head_Info*)pTemp)->Length + sizeof(Asset_Head_Info));
    DEBUG((-1, "SmcAssetInfoCallBack : 03 TotalSize = %d\n", TotalSize));

if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
// 	mSmcOobLibraryProtocol->Smc_CalculateMd5(pTemp,TotalSize,Digest);	
    CalculateMd5(pTemp, TotalSize, Digest);

    //
    // BIOS Get OOB File Status from BMC.
    //
    OobFileStatus0 = mSmcOobPlatformPolicyPtr->OobConfigurations.OobFileStatus0;
    DEBUG((-1, "Get OOB status : 0x%x\n", OobFileStatus0));

    if (OobFileStatus0 & (1 << OOB_FILE_TYPE_ASSET)) { // Asset Info file is available

        DEBUG((-1, "Asset Info file is available.\n"));

        Status = pRS->GetVariable(
                     L"SMCOOBASSET",
                     &gSMCOOBMD5GUID,
                     NULL,
                     &DigestLen,
                     &OldDigest);

        if(!EFI_ERROR (Status)) {
            if (MemCmp(OldDigest, Digest, MD5_DIGEST_SIZE) == 0) {
                DEBUG((-1, "The OOB Asset Info MD5 in BMC and BIOS are same.\n"));
                if (pTemp != NULL) pBS->FreePool(pTemp);
                if (IdentifyDriveInfo != NULL) pBS->FreePool(IdentifyDriveInfo);
                pBS->CloseEvent(Event);
                return;
            }
        }
    }

}

    Status = pBS->LocateProtocol(
                 &gEfiDxeIpmiTransportProtocolGuid,
                 NULL,
                 &gIpmiTransport);

    if(EFI_ERROR(Status)) {
        if (pTemp != NULL) pBS->FreePool(pTemp);
        if (IdentifyDriveInfo != NULL) pBS->FreePool(IdentifyDriveInfo);
        pBS->CloseEvent(Event);
        return;
    }

    //
    // Upload Asset Info file to BMC.
    //
    while(FileRetry) {
        DEBUG((-1, "Upload Asset Info To BMC By VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->PrepareDataToBmc (mSmcIpmiOemCommandSetProtocol, pTemp, TotalSize);
        DEBUG((-1, "SmcAssetInfoCallBack : 05 Status = %r\n", Status));
        if(!EFI_ERROR (Status)) {
            Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_ASSET, TotalSize, &ResponseStatus);
            DEBUG((-1, "SmcAssetInfoCallBack : 06 Status = %r\n", Status));
            if(!EFI_ERROR (Status)) {
                Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_ASSET, &ResponseStatus);
                DEBUG((-1, "SmcAssetInfoCallBack : 07 Status = %r\n", Status));
                if(!EFI_ERROR (Status)) {
if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
                    Status = pRS->SetVariable(
                                 L"SMCOOBASSET",
                                 &gSMCOOBMD5GUID,
                                 EFI_VARIABLE_NON_VOLATILE |
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 MD5_DIGEST_SIZE,
                                 &Digest);
                    DEBUG((-1, "SmcAssetInfoCallBack : 08 Status = %r\n", Status));
}
                    break;
                }
            }
        }
        pBS->Stall(100000);   // delay 100000 microseconds
        FileRetry--;
        DEBUG((-1, "Upload Asset Info To BMC checksum fail retry.\n"));
    }

    if (pTemp != NULL) pBS->FreePool(pTemp);
    if (IdentifyDriveInfo != NULL) pBS->FreePool(IdentifyDriveInfo);
    pBS->CloseEvent(Event);

    DEBUG((-1, "SmcAssetInfoCallBack End.\n"));
    return;
}

EFI_STATUS
InitSmcAssetInfo(
    IN	EFI_HANDLE		ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    EFI_EVENT	SmEvent;
    VOID        *SmReg;

    DEBUG((-1, "InitSmcAssetInfo Entry.\n"));

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol (&gSmcIpmiOemCommandSetProtocolGuid, NULL, &mSmcIpmiOemCommandSetProtocol);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY; 
    }

    Status = pBS->LocateProtocol (&gSmcOobPlatformPolicyGuid, NULL, &mSmcOobPlatformPolicyPtr);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY; 
    }

	Status = pBS->LocateProtocol (&gSmcOobLibraryProtocolGuid, NULL, &mSmcOobLibraryProtocol);
    if (EFI_ERROR (Status)) {
      OOB_DEBUG((-1, "OOB_DEBUG : SMC_OOB_LIBRARY_PROTOCOL is not ready.\n"));
      return EFI_NOT_READY; 
    }
    Status = pBS->CreateEvent(
                 EVT_NOTIFY_SIGNAL,
                 TPL_CALLBACK,
                 SmcAssetInfoCallBack,
                 NULL,
                 &SmEvent);
    if(!Status) {
        Status = pBS->RegisterProtocolNotify(
                     &gBdsAllDriversConnectedProtocolGuid,
                     SmEvent,
                     &SmReg);
    }

    DEBUG((-1, "InitSmcAssetInfo End.\n"));

    return Status;
}
