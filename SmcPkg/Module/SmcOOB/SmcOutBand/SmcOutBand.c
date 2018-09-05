//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.29
//    Bug fixed:
//    Reason   : Add a protocol guid to inform OOB update to BMC done.
//    Auditor  : Durant Lin
//    Date     : Aug/07/2018
//
//  Rev. 1.28
//    Bug fixed:
//    Reason   : Use Oob Library Protocol to replace some library functions.
//    Auditor  : Durant Lin
//    Date     : Feb/12/2018
//
//  Rev. 1.27
//    Bug Fix : N/A
//    Reason  : Add a version control string for all OOB EFI files. "_SMCOOBV1.xx.xx_"
//    Auditor : Durant Lin
//    Date    : Jan/04/2018
//
//  Rev. 1.26
//    Bug Fix : N/A
//    Reason  : Isolate PciLib from OOB to OobLibraryProtocol, since different
//              platform will cause hang up issue.
//    Auditor : Durant Lin
//    Date    : Jun/02/2018
//
//  Rev. 1.25
//    Bug Fix : N/A
//    Reason  : Implement a SmcOobProtocol Interface for InBand and OutBand to
//              access Build time OobLibrary not OBJ OobLobrary. 
//    Auditor : Durant Lin
//    Date    : Dec/27/2017
//
//  Rev. 1.24
//    Bug Fix:  [X10DPG] Fix SUM TC#216.
//    Reason:   BIOS CFG size over 64k.
//    Auditor:  Jason Hsu
//    Date:     Oct/23/2017
//
//  Rev. 1.23
//    Bug Fix:  Skip to send DataToBIOSDone command when there is no available file from BMC.
//    Reason:   
//    Auditor:  Jason Hsu
//    Date:     Sep/28/2017
//
//  Rev. 1.22
//    Bug Fix:  Modify restored regions in DownloadBiosStorageFromBmc().
//    Reason:   
//    Auditor:  Jason Hsu
//    Date:     May/11/2017
//
//  Rev. 1.21
//    Bug Fix:  Fix compiler fail afer enable SMC_OOB_MD5_SUPPORT
//    Reason:   Lost OobFileStatus0 and free wrong pool data
//    Auditor:  Salmon Chen
//    Date:     Mar/20/2017
//
//  Rev. 1.20
//    Bug Fix:  Clear OA key when $SMC can't be searched.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Feb/16/2017
//
//  Rev. 1.19
//    Bug Fix:  Add checksum in the latest byte of Bios Storage.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jan/12/2017
//
//  Rev. 1.18
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jan/06/2017
//
//  Rev. 1.17
//    Bug Fix:  Fix sometimes system hangs by downloading DMI from BMC.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/22/2016
//
//  Rev. 1.16
//    Bug Fix:  Increase delay time for OOB files downloading issue.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/19/2016
//
//  Rev. 1.15
//    Bug Fix:  Increase delay time for OOB files downloading issue.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/14/2016
//
//  Rev. 1.14
//    Bug Fix:  Tune the delay time of uploading OOB data.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/02/2016
//
//  Rev. 1.13
//    Bug Fix:  Fix sum ChangeDmiInfo no function.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/02/2016
//
//  Rev. 1.12
//    Bug Fix:  Fix system hanging after getting bin from BMC.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/02/2016
//
//  Rev. 1.11
//    Bug Fix:  Fix coding errors at DownloadSmbiosFileFromBmc.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/24/2016
//
//  Rev. 1.10
//    Bug Fix:  Fix some errors for upload and download OOB files.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/20/2016
//
//  Rev. 1.09
//    Bug Fix:  Rewrite OOB download file code.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/19/2016
//
//  Rev. 1.08
//    Bug Fix:  Fix OOB data didn't be sent to BMC.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/03/2016
//
//  Rev. 1.07
//    Bug Fix:  Skip SmcOutBand and SmcAssetInfo when disabling JPG1.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/12/2016
//
//  Rev. 1.06
//    Bug Fix:  Send all HII data for sum and fix checksum error.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/09/2016
//
//  Rev. 1.05
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.03
//    Bug Fix:  Fix some errors in SmcOutBand module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/10/2016
//
//  Rev. 1.02
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.01
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.00
//    Bug Fix:  Fix system hangs at Sum InBand command.
//    Reason:   Because of using Boot Service in SMM. Separate SmcRomHole into two libraries - DXE and SMM.
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/12/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcOutBand.c
//
// Description: Supermicro Out-of-Band feature.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Token.h"
#include <AmiDxeLib.h>
#include <Include/ServerMgmtSetupVariable.h>
#include "Library\BaseCryptLib.h"
#include <Setup.h>
#include "Protocol\IPMITransportProtocol.h"
#include "SmcRomHole.h"
#include "SmcLib.h"
#include "SmcInBandBiosCfg.h"
#include "SmcInBandSmbios.h"
#include "SmcInBandHii.h"
#include "SmcFeatureFlag.h"
#include "SmcInBandFullSM.h"
#include "SmcInBand.h"
#include "SmcOutBand.h"
#include "OobLibrary.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include <Protocol/SmcIpmiOemCommandSetProtocol.h>
#include <Protocol/SmcRomHoleProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmcOobLibraryProtocol.h>
#include "SmcOobVersion.h"

#define EFI_TPL_NOTIFY 16

#define MAX_DOWNLOAD_SIZE 256 * 1024

extern EFI_GUID SetupGuid;

extern EFI_GUID gSmcFeatureSupportFlagsGuid;
extern EFI_GUID gSmcOobDataUploadBmcDoneGuid;

BIOS_GET_OOB_FILE_STATUS_RSP OobFileStatusRes;

SMC_IPMI_OEM_COMMAND_SET_PROTOCOL *mSmcIpmiOemCommandSetProtocol;
SMC_ROM_HOLE_PROTOCOL             *mSmcRomHoleProtocol;
SMC_OOB_PLATFORM_POLICY           *mSmcOobPlatformPolicyPtr;
SMC_OOB_LIBRARY_PROTOCOL		  *mSmcOobLibraryProtocol;

EFI_IPMI_TRANSPORT *mIpmiTransportProtocol = NULL;


UINT32	GetCmosMemoryAddressValue(){
				 
	UINT32 Data32 = 0;
	UINT32 Temp32 = 0;

	Data32 = mSmcOobLibraryProtocol->Smc_OobGetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrHH);
    Temp32 |= Data32 << 24;

	Data32 = mSmcOobLibraryProtocol->Smc_OobGetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrH);
    Temp32 |= Data32 << 16;

	Data32 = mSmcOobLibraryProtocol->Smc_OobGetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrL);
    Temp32 |= Data32 << 8;

	Data32 = mSmcOobLibraryProtocol->Smc_OobGetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrLL);
    Temp32 |= Data32;

	return Temp32;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  UploadBiosCfgToBMC
//
// Description:
//  Upload BIOS config To BMC.
//
//  In upload data to BMC routine,
//  we implement retry to make sure transfer data successfully.
//  In some special case, BMC is too busy to handle upload commands.
//
//  Steps :
//  1. Get variables and save them to buffer - "TotalDataBuffer".
//  2. Compare current BiosCfg MD5 with old MD5.
//  3. Upload current BiosCfg data to BMC.
//
// Input:
//      None.
//
// Output:
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID UploadBiosCfgToBMC(void)
{
    EFI_STATUS Status;
    UINT8 *TotalDataBuffer = NULL;
    UINTN TotalDataBufferSize;
    UINT8 FileRetry = OOB_IPMI_FILE_RETRY;
    UINTN DigestLen = MD5_DIGEST_SIZE;
    UINT8 Digest[MD5_DIGEST_SIZE], OldDigest[MD5_DIGEST_SIZE];
    InBand_BIOS_Cfg_Header* InBandBIOSCfgHeader = NULL;
    UINT32 OobFileStatus0 = 0;
    UINT32 Temp32 = 0;
    UINT8  ResponseStatus;

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadBiosCfgToBMC entry.\n"));

			 
	Temp32 = GetCmosMemoryAddressValue(); 
    TotalDataBuffer = (UINT8*)(Temp32 + SMC_OOB_SMRAM_BIOS_CFG_OFFSET);

    InBandBIOSCfgHeader = (InBand_BIOS_Cfg_Header*)TotalDataBuffer;

    if(MemCmp(InBandBIOSCfgHeader->Signature, "_BIN", 4)) { // Skip, if it's not bin file.
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : No bin data in BIOS.\n"));
        return;
    }
    else {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : bin data exists.\n"));
    }

    //
    // There is one byte of checksum at the end, so plus 1.
    //
    TotalDataBufferSize = InBandBIOSCfgHeader->FileSize + sizeof (InBand_BIOS_Cfg_Header) + 1;

if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {

	//mSmcOobLibraryProtocol->Smc_CalculateMd5(TotalDataBuffer, TotalDataBufferSize , Digest);
	CalculateMd5(TotalDataBuffer, TotalDataBufferSize , Digest);
    //
    // BIOS Get OOB File Status from BMC.
    //
    OobFileStatus0 = mSmcOobPlatformPolicyPtr->OobConfigurations.OobFileStatus0;
    OOB_DEBUG((-1, "OOB_DEBUG : Get OOB status : 0x%x\n", OobFileStatus0));

    if (OobFileStatus0 & (1 << OOB_FILE_TYPE_BIN)) { // Bin file is available

        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Bin file is available.\n"));

        Status = pRS->GetVariable(
                     L"SMCBIOSBIN",
                     &gSMCOOBMD5GUID,
                     NULL,
                     &DigestLen,
                     &OldDigest);

        if (!EFI_ERROR (Status)) {

            //
            // Compare current BiosCfg MD5 with old MD5.
            //

            if(MemCmp(OldDigest, Digest, MD5_DIGEST_SIZE) == 0) {
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : MD5 Matches since Last time.\n"));
                return;
            }
        }
    }

}

    OOB_DEBUG((TRACE_ALWAYS, "UploadBiosCfgToBMC : 03 TotalDataBufferSize = 0x%x\n", TotalDataBufferSize));

    //
    // Upload current BiosCfg data to BMC.
    //
    while(FileRetry) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadBiosCfgToBMC by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->PrepareDataToBmc (mSmcIpmiOemCommandSetProtocol, TotalDataBuffer, (UINT32)TotalDataBufferSize);
        OOB_DEBUG((TRACE_ALWAYS, "UploadBiosCfgToBMC : 05 Status = %r\n", Status));
        OOB_DEBUG((TRACE_ALWAYS, "UploadBiosCfgToBMC : 05 TotalDataBufferSize = 0x%x\n", TotalDataBufferSize));
        if (!EFI_ERROR (Status)) {
            Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIN, (UINT32)TotalDataBufferSize, &ResponseStatus);
            pBS->Stall(20000);
            OOB_DEBUG((TRACE_ALWAYS, "UploadBiosCfgToBMC : 06 Status = %r\n", Status));
            if (!EFI_ERROR (Status)) {
                Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIN, &ResponseStatus);
                OOB_DEBUG((TRACE_ALWAYS, "UploadBiosCfgToBMC : 07 Status = %r\n", Status));
                if (!EFI_ERROR (Status)) {
if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
                    Status = pRS->SetVariable(
                                 L"SMCBIOSBIN",
                                 &gSMCOOBMD5GUID,
                                 EFI_VARIABLE_NON_VOLATILE |
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 MD5_DIGEST_SIZE,
                                 &Digest);
                    OOB_DEBUG((TRACE_ALWAYS, "UploadBiosCfgToBMC : 08 Status = %r\n", Status));
                }
                    break;
                }
            }
        }

        pBS->Stall(100000); // delay 100000 microseconds
        FileRetry--;
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadBiosCfgToBMC checksum fail retry.\n"));
    }

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadBiosCfgToBMC end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DownloadBiosCfgFromBMC
//
// Description:
//  Download BIOS config From BMC and save them to BIOS ROM.
//  Waiting InBand BiosCfg function(InBandUpdateBiosCfg) update to system.
//
// Input:
//      None.
//
// Output:
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID DownloadBiosCfgFromBMC(void)
{
    EFI_STATUS Status;
    UINT8 RspStatus, WaitRspStatus;
    UINT32 WaitBmcLoop = 0;
    UINT32 FileSize;
    UINT8 *TotalDataBuffer = NULL;
    UINT32 TotalDataBufferPtr;
    UINT8 checksum;
    UINT8 DownloadFailRetry = 0;
    UINT8 DownloadSuccess = 0, DownloadProcess = 0;
    UINT32 OobFileStatus0 = 0;

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : DownloadBiosCfgFromBMC entry.\n"));

    Status = pBS->AllocatePool(EfiBootServicesData, MAX_DOWNLOAD_SIZE, &TotalDataBuffer);
    if (EFI_ERROR (Status)) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Allocating memory is failed.\n"));
        return;
    }

    for (DownloadFailRetry = 0; DownloadFailRetry < 5; DownloadFailRetry++) {

        DownloadProcess = 0;
        DownloadSuccess = 0;

        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : DownloadBiosCfgFromBMC by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIN, &FileSize, &RspStatus);
        if (!EFI_ERROR (Status)) { //return success
            if(RspStatus == 1) { // Available
                DownloadProcess = 1;
                WaitRspStatus = 0;

                //
                // Wait BMC to copy data to VRAM.
                //
                for (WaitBmcLoop = 0; WaitBmcLoop < 200; WaitBmcLoop++) {
                    Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataReady (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIN, &WaitRspStatus);
                    if(!EFI_ERROR (Status)) {
                        if (WaitRspStatus == 0) { // Not Available
                            break;
                        }
                        else if (WaitRspStatus == 1) { // File is preparing
                        }
                        else if (WaitRspStatus == 2) { // File is ready
                            break;
                        }
                        else { // Unknown
                        }
                    }
                    pBS->Stall(30000);
                }
                pBS->Stall(200000);
                if (WaitRspStatus == 2) { // File is ready, copy data from VRAM.
                    Status = mSmcIpmiOemCommandSetProtocol->ReadDataToBios (mSmcIpmiOemCommandSetProtocol, TotalDataBuffer, FileSize);
                    if(!EFI_ERROR (Status)) {
                        //
                        //calculate checksum
                        //
                        checksum = 0;
                        for(TotalDataBufferPtr = 0; TotalDataBufferPtr < (FileSize-1); TotalDataBufferPtr++) {
                            checksum += TotalDataBuffer[TotalDataBufferPtr];
                        }

                        if(checksum == TotalDataBuffer[FileSize-1]) {
                            DownloadSuccess = 1;
                            break;
                        }
                        else {
                            OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : Checksum is error when downloading file from BMC. IPMI CMD : 0x30 0xa0 0x0a\n"));
                        }
                    }
                }
                else {
                    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : Status is error when downloading file from BMC. IPMI CMD : 0x30 0xa0 0x0a\n"));
                }
            }
            else if(RspStatus == 0) { // Not available
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : File is not available on BMC.\n"));
                break;
            }
            else { // Error
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : Status is error when downloading file from BMC.  IPMI CMD : 0x30 0xa0 0x09\n"));
            }
        }
    }

    //
    // Send result to BMC.
    //
    if (DownloadProcess) {
        if (DownloadSuccess) {
            Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIN, 0); // Success
            if (!EFI_ERROR (Status)) {
                //
                // Store data from BMC into BIOS ROM waiting SmcInBand BiosCfg function.
                //
                Status = mSmcRomHoleProtocol->SmcRomHoleWriteRegion (INBAND_REGION, TotalDataBuffer);
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Store BIOS config data from BMC into BIOS ROM. Status = %r\n", Status));
            }
        }
        else { // checksum error
            Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIN, 1); // Failure
            OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Downloading BIOS config data is failed.\n"));
        }
    }

    if (TotalDataBuffer != NULL) pBS->FreePool(TotalDataBuffer);

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : DownloadBiosCfgFromBMC end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  UploadHiiToBMC
//
// Description:
//  Upload HII To BMC.
//  HII file is the most important file for BiosCfg function.
//  It included all information SUM need.
//  It must be uploaded to BMC when system flash new BIOS or new BMC.
//
//  In upload data to BMC routine,
//  we implement retry to make sure transfer data successfully.
//  In some special case, BMC is too busy to handle upload commands.
//
// Input:
//      None.
//
// Output:
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UploadHiiToBMC(void)
{
    EFI_STATUS Status;
    UINT8 FileRetry = OOB_IPMI_FILE_RETRY;
    UINT8* HiiData = NULL;
    UINT32 HiiDataSize = 0;
    INBAND_HII_FILE_HEADER* HiiFileHeader = NULL;
    UINT8 Digest[MD5_DIGEST_SIZE], OldDigest[MD5_DIGEST_SIZE];
    UINTN DigestLen = MD5_DIGEST_SIZE;
    UINT32 Temp32 = 0;
    UINT32 OobFileStatus0 = 0;
    UINT8  ResponseStatus;

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadHiiToBMC entry.\n"));

	Temp32 = GetCmosMemoryAddressValue(); 

    HiiData = (UINT8*)Temp32;
    HiiData += SMC_OOB_SMRAM_HII_OFFSET;

    HiiFileHeader = (INBAND_HII_FILE_HEADER*)HiiData;
    HiiDataSize = HiiFileHeader->FileSize + sizeof (INBAND_HII_FILE_HEADER);
    //
    // Check the signature. it must meets "_HII".
    //
    if(MemCmp(HiiFileHeader->Signature, "_HII", 4)) {	//skip, if no dat file in BIOS
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : No HII data in BIOS.\n"));
        return;
    }
    else {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : HII data exists.\n"));
    }

if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {

//	mSmcOobLibraryProtocol->Smc_CalculateMd5(HiiData, HiiDataSize, Digest);
	CalculateMd5(HiiData, HiiDataSize, Digest);
    //
    // BIOS Get OOB File Status from BMC.
    //
    OobFileStatus0 = mSmcOobPlatformPolicyPtr->OobConfigurations.OobFileStatus0;
    OOB_DEBUG((-1, "OOB_DEBUG : Get OOB status : 0x%x\n", OobFileStatus0));

    if (OobFileStatus0 & (1 << OOB_FILE_TYPE_HII)) { // HII file is available

        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : HII file is available.\n"));

        Status = pRS->GetVariable(
                     L"SMCOOBHII",
                     &gSMCOOBMD5GUID,
                     NULL,
                     &DigestLen,
                     &OldDigest);

        if(!EFI_ERROR (Status)) {

            if (MemCmp(OldDigest, Digest, MD5_DIGEST_SIZE) == 0) {
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : The OOB HII MD5 in BMC and BIOS are same.\n"));
                return;
            }
        }
    }

}

    OOB_DEBUG((TRACE_ALWAYS, "UploadHiiToBMC : 03 HiiDataSize = 0x%x\n", HiiDataSize));

    //
    // Upload HII file to BMC.
    //
    while(FileRetry) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadHiiToBMC by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->PrepareDataToBmc (mSmcIpmiOemCommandSetProtocol, HiiData, HiiDataSize);
        OOB_DEBUG((TRACE_ALWAYS, "UploadHiiToBMC : 05 Status = %r\n", Status));
        OOB_DEBUG((TRACE_ALWAYS, "UploadHiiToBMC : 05 HiiDataSize = 0x%x\n", HiiDataSize));
        if(!EFI_ERROR (Status)) {
            Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_HII, HiiDataSize, &ResponseStatus); // 0x30, 0xA0, 0x0C
            pBS->Stall(120000); // HII is big, so it needs more delay time.
            OOB_DEBUG((TRACE_ALWAYS, "UploadHiiToBMC : 06 Status = %r\n", Status));
            if(!EFI_ERROR (Status)) {
                Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_HII, &ResponseStatus); // 0x30, 0xA0, 0x0E
                OOB_DEBUG((TRACE_ALWAYS, "UploadHiiToBMC : 07 Status = %r\n", Status));
                if(!EFI_ERROR (Status)) {
if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
                    Status = pRS->SetVariable(
                                 L"SMCOOBHII",
                                 &gSMCOOBMD5GUID,
                                 EFI_VARIABLE_NON_VOLATILE |
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 MD5_DIGEST_SIZE,
                                 &Digest);
                    OOB_DEBUG((TRACE_ALWAYS, "UploadHiiToBMC : 08 Status = %r\n", Status));
}
                    break;
                }
            }
        }
        pBS->Stall(100000); // delay 100000 microseconds
        FileRetry--;
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadHiiToBMC checksum fail retry.\n"));
    }

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadHiiToBMC end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  UploadFullSmBiosToBMC
//
// Description:
//  Upload FullSmBios To BMC.
//  HII file is the most important file for BiosCfg function.
//  It included all information SUM need.
//  It must be uploaded to BMC when system flash new BIOS or new BMC.
//
//  In upload data to BMC routine,
//  we implement retry to make sure transfer data successfully.
//  In some special case, BMC is too busy to handle upload commands.
//
// Input:
//      None.
//
// Output:
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UploadFullSmBiosToBMC(VOID)
{
    EFI_STATUS Status;
    UINT8 FileRetry = OOB_IPMI_FILE_RETRY;
    UINT8* FullSmBiosData = NULL;
    UINT32 FullSmBiosSize = 0;
    InBand_FullSmBios_Header* FullSmBiosHeader = NULL; 
    UINT8 Digest[MD5_DIGEST_SIZE], OldDigest[MD5_DIGEST_SIZE];
    UINTN DigestLen = MD5_DIGEST_SIZE; 
    UINT32 Temp32 = 0;
    UINT32 OobFileStatus0 = 0;
    UINT8  ResponseStatus;

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadFullSmBiosToBMC entry.\n"));

	Temp32 = GetCmosMemoryAddressValue(); 
    FullSmBiosData = (UINT8*)(Temp32 + SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET);

    FullSmBiosHeader = (InBand_FullSmBios_Header*)FullSmBiosData;
    FullSmBiosSize = FullSmBiosHeader->FileSize + sizeof (InBand_FullSmBios_Header);

    //
    // Check the signature. it must meets SM_SIGNATURE.
    //
    if(MemCmp(FullSmBiosHeader->Signature, "_SM_", 4)) {	//skip, if no FullSmBios file in BIOS
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : No FullSmBios data in BIOS.\n"));
        return;
    }
    else {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : FullSmBios data exists.\n"));
    }

if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {

	//mSmcOobLibraryProtocol->Smc_CalculateMd5(FullSmBiosData, FullSmBiosSize, Digest);
	CalculateMd5(FullSmBiosData, FullSmBiosSize, Digest);
    //
    // BIOS Get OOB File Status from BMC.
    //
    OobFileStatus0 = mSmcOobPlatformPolicyPtr->OobConfigurations.OobFileStatus0;
    OOB_DEBUG((-1, "OOB_DEBUG : Get OOB status : 0x%x\n", OobFileStatus0));

    if (OobFileStatus0 & (1 << OOB_FILE_TYPE_FULL_SMBIOS)) { // Full SMBIOS file is available

        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Full SMBIOS file is available.\n"));

        Status = pRS->GetVariable(
                     L"SMCFULLSM",
                     &gSMCOOBMD5GUID,
                     NULL,
                     &DigestLen,
                     &OldDigest);

        if(!EFI_ERROR (Status)) {

            if (MemCmp(OldDigest, Digest, MD5_DIGEST_SIZE) == 0) {
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : The OOB FullSmBios MD5 in BMC and BIOS are same.\n"));
                return;
            }
        }
    }

}

    OOB_DEBUG((TRACE_ALWAYS, "UploadFullSmBiosToBMC : 03 FullSmBiosSize = %d\n", FullSmBiosSize));

    //
    // Upload FullSmBios file to BMC.
    //
    while(FileRetry) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadFullSmBiosToBMC by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->PrepareDataToBmc (mSmcIpmiOemCommandSetProtocol, FullSmBiosData, FullSmBiosSize);
        OOB_DEBUG((TRACE_ALWAYS, "UploadFullSmBiosToBMC : 05 Status = %r\n", Status));
        OOB_DEBUG((TRACE_ALWAYS, "UploadFullSmBiosToBMC : 05 FullSmBiosSize = 0x%x\n", FullSmBiosSize));
        if(!EFI_ERROR (Status)) {
            Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_FULL_SMBIOS, FullSmBiosSize, &ResponseStatus);
            pBS->Stall(10000);
            OOB_DEBUG((TRACE_ALWAYS, "UploadFullSmBiosToBMC : 06 Status = %r\n", Status));
            if(!EFI_ERROR (Status)) {
                Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_FULL_SMBIOS, &ResponseStatus);
                OOB_DEBUG((TRACE_ALWAYS, "UploadFullSmBiosToBMC : 07 Status = %r\n", Status));
                if(!EFI_ERROR (Status)) {
if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
                    Status = pRS->SetVariable(
                                 L"SMCFULLSM",
                                 &gSMCOOBMD5GUID,
                                 EFI_VARIABLE_NON_VOLATILE |
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 MD5_DIGEST_SIZE,
                                 &Digest);
                    OOB_DEBUG((TRACE_ALWAYS, "UploadFullSmBiosToBMC : 08 Status = %r\n", Status));
}
                    break;
                }
            }
        }
        pBS->Stall(100000); // delay 100000 microseconds
        FileRetry--;
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadFullSmBiosToBMC checksum fail retry.\n"));
    }

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadFullSmBiosToBMC end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  UploadSmbiosFileToBmc
//
// Description:
//  Upload current Smbios File To Bmc.
//  OutBand SMBIOS upload function is later than InBand SMBIOS, so InBand SMBIOS
//  will store current SMBIOS data into BIOS ROM.
//  OutBand read current SMBIOS from BIOS ROM and compare with backup SMBIOS(previous),
//  If data is different, it mean current SMBIOS is different too,
//  upload current SMBIOS to BMC and update backup.
//
//  In upload data to BMC routine,
//  we implement retry to make sure transfer data successfully.
//  In some special case, BMC is too busy to handle upload commands.
//
// Input:
//      None.
//
// Output:
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UploadSmbiosFileToBmc(void)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	*SmbiosData = NULL;
    UINTN	TotalDataSize;
    UINT8	FileRetry = OOB_IPMI_FILE_RETRY;
    InBand_Smbios_Data_Header *SmcOobSmbiosHeader = NULL;
    UINT32	AreaSize = 0;
    UINT32 OobFileStatus0 = 0;
    UINT8	Digest[MD5_DIGEST_SIZE], OldDigest[MD5_DIGEST_SIZE];
    UINTN	DigestLen = MD5_DIGEST_SIZE;
    UINT8 ResponseStatus;

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadSmbiosFileToBmc entry.\n"));

    //
    // Read current SMBIOS data from BIOS ROM.
    // OutBand SMBIOS upload function is later than InBand SMBIOS,
    // So the current SMBIOS data came from InBand SMBIOS.
    //
    AreaSize = mSmcRomHoleProtocol->SmcRomHoleSize (CURRENT_SMBIOS_REGION);
    Status = pBS->AllocatePool(EfiBootServicesData, AreaSize, &SmbiosData);
    if(EFI_ERROR (Status)) {
        return;
    }

    Status = mSmcRomHoleProtocol->SmcRomHoleReadRegion (CURRENT_SMBIOS_REGION, SmbiosData);
    if(EFI_ERROR (Status)) {
        if (SmbiosData != NULL) pBS->FreePool(SmbiosData);
        return;
    }
    SmcOobSmbiosHeader = (InBand_Smbios_Data_Header*)SmbiosData;

    //
    // Compare SMBIOS data got from BMC, if no "_SMB" in header, the data is fail.
    //
    if(MemCmp(SmcOobSmbiosHeader->Signature, "_SMB", 4) != 0) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadSmbiosFileToBmc, Smbios come from SW or BMC.\n"));
        if (SmbiosData != NULL) pBS->FreePool(SmbiosData);
        return;
    }

    TotalDataSize = (UINTN)(SmcOobSmbiosHeader->FileSize + sizeof(InBand_Smbios_Data_Header));
    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Smbios file totalDataSize = %x.\n", TotalDataSize));

if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {

	//mSmcOobLibraryProtocol->Smc_CalculateMd5(SmbiosData, TotalDataSize, Digest);
	CalculateMd5(SmbiosData, TotalDataSize, Digest);
    //
    // BIOS Get OOB File Status from BMC.
    //
    OobFileStatus0 = mSmcOobPlatformPolicyPtr->OobConfigurations.OobFileStatus0;
    OOB_DEBUG((-1, "OOB_DEBUG : Get OOB status : 0x%x\n", OobFileStatus0));

    if (OobFileStatus0 & (1 << OOB_FILE_TYPE_DMI)) { // SMBIOS file is available

        OOB_DEBUG((-1, "OOB_DEBUG : SMBIOS file is available.\n"));

        //
        // Compare current SMBIOS data with backup(previous) by MD5.
        //

        Status = pRS->GetVariable(
                     L"SMCSMBIOS",
                     &gSMCOOBMD5GUID,
                     NULL,
                     &DigestLen,
                     &OldDigest);

        if (!EFI_ERROR (Status)) {

            if (MemCmp(OldDigest, Digest, MD5_DIGEST_SIZE) == 0) {
                OOB_DEBUG((-1, "OOB_DEBUG : Smbios is the same.\n"));
                if (SmbiosData != NULL) pBS->FreePool(SmbiosData);
                return;
            }
        }
    }

}

    OOB_DEBUG((TRACE_ALWAYS, "UploadSmbiosFileToBmc : 03 TotalDataSize = %d\n", TotalDataSize));

    //
    // Upload current SMBIOS data to BMC.
    // Upload data to BMC must follow three steps.
    //
    while (FileRetry) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadSmbiosFileToBmc by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->PrepareDataToBmc (mSmcIpmiOemCommandSetProtocol, SmbiosData, (UINT32)TotalDataSize);
        OOB_DEBUG((TRACE_ALWAYS, "UploadSmbiosFileToBmc : 05 Status = %r\n", Status));
        OOB_DEBUG((TRACE_ALWAYS, "UploadSmbiosFileToBmc : 05 TotalDataSize = 0x%x\n", TotalDataSize));
        if (!EFI_ERROR (Status)) {
            Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_DMI, (UINT32)TotalDataSize, &ResponseStatus);
            pBS->Stall(8000);
            OOB_DEBUG((TRACE_ALWAYS, "UploadSmbiosFileToBmc : 06 Status = %r\n", Status));
            if (!EFI_ERROR (Status)) {
                Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_DMI, &ResponseStatus);
                OOB_DEBUG((TRACE_ALWAYS, "UploadSmbiosFileToBmc : 07 Status = %r\n", Status));
                //
                // If upload data to BMC successfully, exit.
                //
                if(!EFI_ERROR(Status)) {
if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
                    Status = pRS->SetVariable(
                                 L"SMCSMBIOS",
                                 &gSMCOOBMD5GUID,
                                 EFI_VARIABLE_NON_VOLATILE |
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 MD5_DIGEST_SIZE,
                                 &Digest);
                    OOB_DEBUG((TRACE_ALWAYS, "UploadSmbiosFileToBmc : 08 Status = %r\n", Status));
}
                    break;
                }
            }
        }
        pBS->Stall(100000); // delay 100000 microseconds
        FileRetry--;
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadSmbiosFileToBmc checksum fail retry.\n"));
    }

    if (SmbiosData != NULL) pBS->FreePool(SmbiosData);
    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadSmbiosFileToBmc end.\n"));
}

VOID UploadFeatureSupportFlagsFileToBmc(void)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8* pBuffer = NULL;
    UINT32 DataSize = 64;
    UINT32 Attr = 0;
    UINT8* VarData = NULL;
    UINTN VarDataSize = sizeof (PlatInfo_H);
    UINT8 FileRetry = OOB_IPMI_FILE_RETRY;
    UINT32 OobFileStatus0 = 0;
    UINT8  ResponseStatus;
    
    Status = pBS->AllocatePool(EfiBootServicesData, DataSize, &pBuffer);
    if (EFI_ERROR (Status)) {
        return;
    }
	mSmcOobLibraryProtocol->Smc_GenerateFeatureSupportFlags(
			pBuffer,
			&DataSize,
			mSmcOobPlatformPolicyPtr
	);

    Status = pBS->AllocatePool(EfiBootServicesData, VarDataSize, &VarData);
    if (EFI_ERROR (Status)) {
        if (pBuffer != NULL) Status = pBS->FreePool(pBuffer);
        return;
    }

    //
    // Check the variable - SMCFSFHEADER in BIOS NVRAM.
    // Because that updating BIOS would clear NVRAM.
    // If user update new BIOS, force to resend the feature flag of new BIOS.
    //

    OobFileStatus0 = mSmcOobPlatformPolicyPtr->OobConfigurations.OobFileStatus0;

    if (OobFileStatus0 & (1 << OOB_FILE_TYPE_BIOS_FEATURE_FLAG)) { // Feature Flag file is available on BMC
        Status = pRS->GetVariable (L"SMCFSFHEADER", &gSmcFeatureSupportFlagsGuid, &Attr, &VarDataSize, VarData);
        if (!EFI_ERROR (Status)) { // BIOS has sent the data.
            if (MemCmp (VarData, pBuffer, VarDataSize) == 0) {
                if (VarData != NULL) Status = pBS->FreePool(VarData);
                if (pBuffer != NULL) Status = pBS->FreePool(pBuffer);
                return;
            }
        }
    }

    while (FileRetry) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadFeatureSupportFlagsFileToBmc by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->PrepareDataToBmc (mSmcIpmiOemCommandSetProtocol, pBuffer, DataSize);
        OOB_DEBUG((TRACE_ALWAYS, "UploadFeatureSupportFlagsFileToBmc : 05 Status = %r\n", Status));
        OOB_DEBUG((TRACE_ALWAYS, "UploadFeatureSupportFlagsFileToBmc : 05 DataSize = 0x%x\n", DataSize));
        if (!EFI_ERROR (Status)) {
            Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_FEATURE_FLAG, DataSize, &ResponseStatus);
            pBS->Stall(3000);
            OOB_DEBUG((TRACE_ALWAYS, "UploadFeatureSupportFlagsFileToBmc : 06 Status = %r\n", Status));
            if (!EFI_ERROR (Status)) {
                Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_FEATURE_FLAG, &ResponseStatus);
                OOB_DEBUG((TRACE_ALWAYS, "UploadFeatureSupportFlagsFileToBmc : 07 Status = %r\n", Status));
                if (!EFI_ERROR (Status)) {
                    VarDataSize = sizeof (PlatInfo_H);
                    Status = pRS->SetVariable(
                                 L"SMCFSFHEADER",
                                 &gSmcFeatureSupportFlagsGuid,
                                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 VarDataSize,
                                 pBuffer);
                    break;
                }
            }
        }
        pBS->Stall(100000); // delay 100000 microseconds
        FileRetry--;
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : UploadFeatureSupportFlagsFileToBmc checksum fail retry.\n"));
    }
    if (VarData != NULL) Status = pBS->FreePool(VarData);
    if (pBuffer != NULL) Status = pBS->FreePool(pBuffer);
}

VOID
UploadBiosStorageToBMC(void)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       *DataBuffer, FileRetry = OOB_IPMI_FILE_RETRY, checksum;
    UINT32      i;
    UINT32      OobFileStatus0 = 0;
    UINTN       DigestLen = MD5_DIGEST_SIZE;
    UINT8       Digest[MD5_DIGEST_SIZE], OldDigest[MD5_DIGEST_SIZE];
    UINT8       ResponseStatus;

    DEBUG((-1, "UploadBiosStorageToBMC Start.\n"));

    Status = pBS->AllocatePool(EfiBootServicesData, SMC_ROM_HOLE_SIZE, &DataBuffer);
    if(Status)  return;

    Status = mSmcRomHoleProtocol->SmcRomHoleReadRegion(FULL_REGION, DataBuffer);
    if(Status) {
        pBS->FreePool(DataBuffer);
        return;
    }
//The latest byte of BiosStorage is checksum.
    checksum = 0;
    for(i = 0; i < (SMC_ROM_HOLE_SIZE - 1); i++)
        checksum += DataBuffer[i];
//Fill checksum to the latest byte.
    DataBuffer[SMC_ROM_HOLE_SIZE - 1] = checksum;
    DEBUG((-1, "checksum = %x.\n", checksum));

if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
	//mSmcOobLibraryProtocol->Smc_CalculateMd5(DataBuffer, SMC_ROM_HOLE_SIZE, Digest);
	CalculateMd5(DataBuffer, SMC_ROM_HOLE_SIZE, Digest);
    //
    // BIOS Get OOB File Status from BMC.
    //
    OobFileStatus0 = mSmcOobPlatformPolicyPtr->OobConfigurations.OobFileStatus0;
    DEBUG((-1, "OOB_DEBUG : Get OOB status : 0x%x\n", OobFileStatus0));

    if(OobFileStatus0 & (1 << OOB_FILE_TYPE_BIOS_STORAGE)) { // Bios Storage file is available
        DEBUG((-1, "OOB_DEBUG : BIOS Storage file is available.\n"));
        //
        // Compare current SMBIOS data with backup(previous) by MD5.
        //
        Status = pRS->GetVariable(
                     L"SMCBIOSSTORAGE",
                     &gSMCOOBMD5GUID,
                     NULL,
                     &DigestLen,
                     &OldDigest);

        if(!Status) {
            if(MemCmp(OldDigest, Digest, MD5_DIGEST_SIZE) == 0) {
                DEBUG((-1, "OOB_DEBUG : BIOS Storage is the same.\n"));
//                if (SmbiosData != NULL) gBS->FreePool(SmbiosData);
                if (DataBuffer != NULL) pBS->FreePool(DataBuffer);
                return;
            }
        }
    }
}
    //
    // Upload current BiosCfg data to BMC.
    //
    while(FileRetry) {
        DEBUG((-1, "OOB_DEBUG : UploadBiosStorageToBMC by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->PrepareDataToBmc(mSmcIpmiOemCommandSetProtocol, DataBuffer, SMC_ROM_HOLE_SIZE);
        DEBUG((-1, "UploadBiosStorageToBMC : 05 Status = %r\n", Status));
        DEBUG((-1, "UploadBiosStorageToBMC : 05 DataBufferSize = 0x%x\n", SMC_ROM_HOLE_SIZE));
        if(!Status) {
            Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_STORAGE, SMC_ROM_HOLE_SIZE, &ResponseStatus);
            pBS->Stall(20000);
            DEBUG((-1, "UploadBiosStorageToBMC : 06 Status = %r\n", Status));
            if(!Status) {
                Status = mSmcIpmiOemCommandSetProtocol->UploadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_STORAGE, &ResponseStatus);
                DEBUG((-1, "UploadBiosStorageToBMC : 07 Status = %r\n", Status));
                if(!Status) {
if (mSmcOobPlatformPolicyPtr->OobConfigurations.Md5Support) {
                    Status = pRS->SetVariable(
                                 L"SMCBIOSSTORAGE",
                                 &gSMCOOBMD5GUID,
                                 EFI_VARIABLE_NON_VOLATILE |
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                 MD5_DIGEST_SIZE,
                                 &Digest);
                    DEBUG((-1, "UploadBiosStorageToBMC : 08 Status = %r\n", Status));
}
                    break;
                }
            }
        }

        pBS->Stall(100000); // delay 100000 microseconds
        FileRetry--;
        DEBUG((-1, "OOB_DEBUG : UploadBiosStorageToBMC checksum fail retry.\n"));
    }
    DEBUG((-1, "UploadBiosStorageToBMC End.\n"));
}

VOID
DownloadBiosStorageFromBmc(void)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       *DataBuffer, RspStatus, WaitRspStatus, checksum;
    UINT8       DownloadFailRetry, DownloadSuccess, FlashFlag, DownloadProcess;
    UINT32      WaitBmcLoop = 0, FileSize = 0, i;
    UINT8       ResponseStatus;

    DEBUG((-1, "DownloadBiosStorageFromBmc Start.\n"));

    Status = pBS->AllocatePool(EfiBootServicesData, SMC_ROM_HOLE_SIZE, &DataBuffer);
    if(Status) {
        OOB_DEBUG((-1, "OOB_DEBUG : Allocating memory is failed.\n"));
        return;
    }
//check $SMC in BIOS, if $SMC exist, BIOS ROM doesn't flash by programmer or OOB IPMI flash
    Status = mSmcRomHoleProtocol->SmcRomHoleReadRegion(SMBIOS_FOR_BMC_REGION, DataBuffer);
    if(!Status) {
        if(!MemCmp(DataBuffer, "$SMC", 4)) {
            DEBUG((-1, "$SMC signature exist. Don't need to restore from IPMI.\n"));
            pBS->FreePool(DataBuffer);
            return;
        }
    }
    pBS->SetMem(DataBuffer, SMC_ROM_HOLE_SIZE, 0xFF);

    for(DownloadFailRetry = 0; DownloadFailRetry < 5; DownloadFailRetry++){
        DownloadProcess = 0;
        DownloadSuccess = 0;
        DEBUG((-1, "OOB_DEBUG : DownloadBiosStorageFromBmc by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_STORAGE, &FileSize, &RspStatus);
        if((!Status) && (FileSize == SMC_ROM_HOLE_SIZE)){
            if(RspStatus == 1) { // Available
                DownloadProcess = 1;
                WaitRspStatus = 0;
                //
                // Wait BMC to copy data to VRAM.
                //
                for(WaitBmcLoop = 0; WaitBmcLoop < 200; WaitBmcLoop++){
                    Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataReady (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_STORAGE, &WaitRspStatus);
                    if(!Status) {
                        if((WaitRspStatus == 0) || (WaitRspStatus == 2)) // Not Available or File is ready
                            break;
                    }
                    pBS->Stall(30000);
                }
                DEBUG((-1, "OOB_DEBUG : Return from DataToBIOSWait %r, %d\n", Status, WaitRspStatus));
                pBS->Stall(200000);
                if(WaitRspStatus == 2){ // File is ready, copy data from VRAM.
                    DEBUG((-1, "OOB_DEBUG : FileSize = 0x%x\n", FileSize));
                    Status = mSmcIpmiOemCommandSetProtocol->ReadDataToBios (mSmcIpmiOemCommandSetProtocol, DataBuffer, FileSize);
                    if(!Status) {
                        //calculate BiosStorage checksum in the latest byte.
                        checksum = 0;
                        for(i = 0; i < (FileSize - 1); i++)
                            checksum += DataBuffer[i];
                        DEBUG((-1, "checksum %x. latest byte %x.\n", checksum, DataBuffer[FileSize - 1]));

                        if(checksum == DataBuffer[FileSize - 1]) {
                            DEBUG((-1, "BiosStorage read and checksum pass.\n"));
                            DownloadSuccess = 1;
                            FlashFlag = 0;
                            Status = mSmcIpmiOemCommandSetProtocol->SmibiosPreservation (mSmcIpmiOemCommandSetProtocol, 0, FlashFlag, &ResponseStatus);
                            FlashFlag = ResponseStatus;
                            if(!Status){
                                if(!(FlashFlag & Preserve_SMBIOS)){  //users unclick preserve SMBIOS
                                    DEBUG((-1, "User unclicks preserve SMBIOS. Clear SMBIOS region in SmcRomHole.\n"));
                                    pBS->SetMem(DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(SMBIOS_FOR_BMC_REGION),
                                                mSmcRomHoleProtocol->SmcRomHoleSize(SMBIOS_FOR_BMC_REGION), 0xFF);
                                    MemCpy(DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(SMBIOS_FOR_BMC_REGION), "$SMC", 4);
                                } else {
                                    DEBUG((-1, "User clicks preserve SMBIOS. Restore SMBIOS region from BMC.\n"));
                                    //Flags == 0x01
                                    *(UINT8*)(DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(SMBIOS_FOR_BMC_REGION) + 10) = 0x01;
                                }
                                mSmcRomHoleProtocol->SmcRomHoleWriteRegion(SMBIOS_FOR_BMC_REGION, DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(SMBIOS_FOR_BMC_REGION));
                                if(!(FlashFlag & Preserve_OA)){  //users unclick preserve OA key
                                    DEBUG((-1, "User unclicks preserve OA. Clear OA region in SmcRomHole.\n"));
                                    pBS->SetMem(DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(OA3_REGION),
                                                    mSmcRomHoleProtocol->SmcRomHoleSize(OA3_REGION), 0xFF);
                                    pBS->SetMem(DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(OA2_REGION),
                                                    mSmcRomHoleProtocol->SmcRomHoleSize(OA2_REGION), 0xFF);
                                } else {
                                    DEBUG((-1, "User clicks preserve OA. Restore OA region from BMC.\n"));
                                }
                                mSmcRomHoleProtocol->SmcRomHoleWriteRegion(OA3_REGION, DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(OA3_REGION));
                                mSmcRomHoleProtocol->SmcRomHoleWriteRegion(OA2_REGION, DataBuffer + mSmcRomHoleProtocol->SmcRomHoleOffset(OA2_REGION));
                            }
                            mSmcIpmiOemCommandSetProtocol->SmibiosPreservation (mSmcIpmiOemCommandSetProtocol, 1, FlashFlag, &ResponseStatus); //clear flash flag
                            FlashFlag = ResponseStatus;
                            break;
                        }
                    }
                }
            }
            else if(RspStatus == 0) { // Not available
                DEBUG((-1, "OOB_DEBUG : BIOS Storage is not available on BMC.\n"));
                break;
            }
        }
    }
    //
    // Send result to BMC.
    //
    if (DownloadProcess){
        if(DownloadSuccess){
           if(mSmcOobPlatformPolicyPtr->PlatformId == OobBakerville) { // Patch for Bakerville update BIOS issue.
           mSmcRomHoleProtocol->SmcRomHoleWriteRegion(FULL_REGION, DataBuffer);
        }
           Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_STORAGE, 0); // success
           DEBUG((-1, "OOB_DEBUG : Store BIOS Storage from BMC. Status = %r\n", Status));
        } else {
           Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_BIOS_STORAGE, 1); // failure
           DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Downloading BIOS Storage is failed.\n"));
        }
    }
    pBS->FreePool(DataBuffer);
    DEBUG((-1, "DownloadBiosStorageFromBmc End.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DownloadSmbiosFileFromBmc
//
// Description:
//  Download Smbios File From Bmc and Store data to BIOS ROM.
//  Waiting InBand SMBIOS function(InBandUpdateSmbios) update to system.
//
// Input:
//      None.
//
// Output:
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID DownloadSmbiosFileFromBmc(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 RspStatus, WaitRspStatus;
    UINT32 WaitBmcLoop = 0;
    UINT32 FileSize = 0, FileOffset;
    UINT8 checksum;
    UINT8 *TotalDataBuffer = NULL;
    InBand_Smbios_Data_Header *SmcOobSmbiosHeader;
    UINT8 DownloadFailRetry = 0;
    BOOLEAN DownloadSuccess = 0;
    UINT32 OobFileStatus0 = 0;
    UINT8  DownloadProcess;

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : DownloadSmbiosFileFromBmc entry.\n"));

    Status = pBS->AllocatePool(EfiBootServicesData, MAX_DOWNLOAD_SIZE, &TotalDataBuffer);
    if (EFI_ERROR (Status)) {
        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Allocating memory is failed.\n"));
        return;
    }

    for (DownloadFailRetry = 0; DownloadFailRetry < 5; DownloadFailRetry++) {
        DownloadProcess = 0;
        DownloadSuccess = 0;

        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : DownloadSmbiosFileFromBmc by VRAM.\n"));
        Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataStart (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_DMI, &FileSize, &RspStatus);

        if(!EFI_ERROR (Status)) {
            if(RspStatus == 1) { // Available
                DownloadProcess = 1;
                WaitRspStatus = 0;

                //
                // Wait BMC to copy data to VRAM.
                //
                for (WaitBmcLoop = 0; WaitBmcLoop < 200; WaitBmcLoop++) {
                    Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataReady (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_DMI, &WaitRspStatus);
                    if(!EFI_ERROR (Status)) {
                        if (WaitRspStatus == 0) { // Not Available
                            break;
                        }
                        else if (WaitRspStatus == 1) { // File is preparing
                        }
                        else if (WaitRspStatus == 2) { // File is ready
                            break;
                        }
                        else { // Unknown
                        }
                    }
                    pBS->Stall(30000);
                }
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Return from DataToBIOSWait %r, %d\n", Status, WaitRspStatus));
                pBS->Stall(200000);
                if (WaitRspStatus == 2) { // File is ready, copy data from VRAM.
                    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : FileSize = 0x%x\n", FileSize));
                    Status = mSmcIpmiOemCommandSetProtocol->ReadDataToBios (mSmcIpmiOemCommandSetProtocol, TotalDataBuffer, FileSize);
                    if(!EFI_ERROR (Status)) {
                        //
                        // Calculate checksum
                        //
                        SmcOobSmbiosHeader = (InBand_Smbios_Data_Header*)TotalDataBuffer;
                        OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : SmcOobSmbiosHeader->FileSize = 0x%x\n", SmcOobSmbiosHeader->FileSize));
                        if (SmcOobSmbiosHeader->FileSize < MAX_DOWNLOAD_SIZE) { // Prevent deadloop
                            checksum = 0;
                            for(FileOffset = 0; FileOffset < SmcOobSmbiosHeader->FileSize; FileOffset++) {
                                checksum += TotalDataBuffer[FileOffset + sizeof(InBand_Smbios_Data_Header)];
                            }

                            OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Checksum in BIOS = 0x%x.\n", checksum));
                            OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Checksum in BMC = 0x%x.\n", SmcOobSmbiosHeader->Checksum));
                            if((checksum != SmcOobSmbiosHeader->Checksum)) {
                                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : DownloadSmbiosFileFromBmc Checksum fail.\n"));
                            }
                            else {
                                DownloadSuccess = 1;
                                break;
                            }
                        }
                        else {
                            OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : File size of header is too big. Maybe downloading file broken.\n"));
                        }
                    }
                }
                else {
                    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : Status is error when downloading file from BMC. IPMI CMD : 0x30 0xa0 0x0a\n"));
                }
            }
            else if(RspStatus == 0) { // Not available
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : File is not available on BMC.\n"));
                break;
            }
            else { // Error
                OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : ERROR : Status is error when downloading file from BMC.  IPMI CMD : 0x30 0xa0 0x09\n"));
            }
        }
    }

    //
    // Send result to BMC.
    //
    if (DownloadProcess) {
        if (DownloadSuccess) {
            Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_DMI, 0); // success
            if(!EFI_ERROR (Status)) {
                //
                // Store data from BMC into BIOS ROM waiting SmcInBand SMBIOS function.
                //
               Status = mSmcRomHoleProtocol->SmcRomHoleWriteRegion (CURRENT_SMBIOS_REGION, TotalDataBuffer);
               OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Store DMI data from BMC into BIOS ROM. Status = %r\n", Status));
        }
       }
        else {
           Status = mSmcIpmiOemCommandSetProtocol->DownloadOobDataDone (mSmcIpmiOemCommandSetProtocol, OOB_FILE_TYPE_DMI, 1); // failure
           OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : Downloading DMI data is failed.\n"));
        }
    }

    if (TotalDataBuffer != NULL) pBS->FreePool(TotalDataBuffer);

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : DownloadSmbiosFileFromBmc end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SignalOOBUploadDone
//
// Description:
//  Signal Protocol that OOB upload BMC data done.
//
// Input:
//      None.
//
// Output:
//      None.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SignalOOBUploadDone(){

    EFI_HANDLE Handle = NULL;

    pBS->InstallProtocolInterface (
        &Handle,
        &gSmcOobDataUploadBmcDoneGuid,
        EFI_NATIVE_INTERFACE,
        NULL
    );	
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  UploadOobDataToBmc
//
// Description:
//  Upload current Bios configuration and SMBIOS to BMC
//
// Input:
//      None.
//
// Output:
//      None.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UploadOobDataToBmc(
    IN EFI_EVENT	Event,
    IN VOID		*Context
)
{

    pBS->CloseEvent (Event);

    UploadBiosCfgToBMC();

    UploadSmbiosFileToBmc();

    UploadHiiToBMC();

    UploadFullSmBiosToBMC();

    UploadFeatureSupportFlagsFileToBmc();

    UploadBiosStorageToBMC();

	SignalOOBUploadDone();
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  DownloadFileFromBMC
//
// Description:
//  Download file callback for BDS_CONNECT_DRIVERS_PROTOCOL_GUID.
//  The routine must be run before SmcInBandCallback.
//
// Input:
//      IN EFI_EVENT Event - Event.
//      IN VOID *Context - Context.
//
// Output:
//      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
DownloadFileFromBMC (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    DEBUG((-1, "DownloadFileFromBMC entry.\n"));

    pBS->CloseEvent (Event);

    //
    // Download BIOS config from BMC.
    //
    DownloadBiosCfgFromBMC();

    //
    // Download SMBIOS from BMC.
    //
    DownloadSmbiosFileFromBmc();

    DownloadBiosStorageFromBmc();

    DEBUG((-1, "DownloadFileFromBMC end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SmcOutBandEntry
//
// Description:
//  Smc OutBand Entry.
//  SmcOutBand module only provide a interface between BMC and BIOS.
//  BIOS transfer data with BMC and store data into BIOS ROM.
//  Waiting SmcInBand module to get data from BIOS ROM then update data to system.
//  After SmcInBand module update date to BMC, SmcOutBand module upload the new
//  SMBIOS and Bios config to BMC.
//
//  HII file is the most important file for BiosCfg function.
//  It included all information SUM need.
//  It must be uploaded to BMC when system flash new BIOS or new BMC.
//
//  BiosCfg and Smbios files uploaded to BMC and save MD5 in variable.
//  When system POST, BIOS will compare MD5 of current BiosCfg and Smbios with the backup one.
//  If current MD5 of BiosCfg and Smbios is different with backup, it mean current BiosCfg
//  and Smbios is different with BMC, BIOS must upload current BiosCfg and Smbios to
//  BMC again.
//
// Input:
//      IN EFI_HANDLE ImageHandle
//                  - Image handle
//      IN EFI_SYSTEM_TABLE *SystemTable
//                  - System table pointer
//
// Output:
//      EFI_STATUS (Return Value)
//                  = EFI_SUCCESS if successful
//                  = or other valid EFI error code
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
SmcOutBandEntry(
    IN EFI_HANDLE	ImageHandle,
    IN EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
//    UINTN	Size = sizeof(SETUP_DATA);
    EFI_IPMI_TRANSPORT *IpmiTransport;
//#if defined IpmiFunction_SUPPORT && IpmiFunction_SUPPORT == 0x01
//    SERVER_MGMT_CONFIGURATION_DATA   gServerMgmtConfiguration;
//#endif  // #if defined IpmiFunction_SUPPORT && IpmiFunction_SUPPORT == 0x01    
    VOID *Registration;
    EFI_EVENT OobDataReadyEvent;
    EFI_EVENT SmEvent;
    VOID *SmReg;

    InitAmiLib(ImageHandle, SystemTable);

    OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : SmcOutBandEntry entry.\n"));

    Status = pBS->LocateProtocol (&gSmcIpmiOemCommandSetProtocolGuid, NULL, &mSmcIpmiOemCommandSetProtocol);
    if (EFI_ERROR (Status)) {
      OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : SMC_IPMI_OEM_COMMAND_SET_PROTOCOL is not ready.\n"));
      return EFI_NOT_READY; 
    }

    Status = pBS->LocateProtocol (&gSmcRomHoleProtocolGuid, NULL, &mSmcRomHoleProtocol);
    if (EFI_ERROR (Status)) {
      OOB_DEBUG((TRACE_ALWAYS, "OOB_DEBUG : SMC_ROM_HOLE_PROTOCOL is not ready.\n"));
      return EFI_NOT_READY; 
    }

    Status = pBS->LocateProtocol (&gSmcOobPlatformPolicyGuid, NULL, &mSmcOobPlatformPolicyPtr);
    if (EFI_ERROR (Status)) {
      OOB_DEBUG((-1, "OOB_DEBUG : SMC_OOB_PLATFORM_POLICY is not ready.\n"));
      return EFI_NOT_READY; 
    }
	Status = pBS->LocateProtocol (&gSmcOobLibraryProtocolGuid, NULL, &mSmcOobLibraryProtocol);
    if (EFI_ERROR (Status)) {
      OOB_DEBUG((-1, "OOB_DEBUG : SMC_OOB_LIBRARY_PROTOCOL is not ready.\n"));
      return EFI_NOT_READY; 
    }
//#if defined IpmiFunction_SUPPORT && IpmiFunction_SUPPORT == 0x01
//    Size = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
//    Status = gRT->GetVariable (
//                 L"ServerSetup",
//                 &gEfiServerMgmtSetupVariableGuid,
//                 NULL,
//                 &Size,
//                 &gServerMgmtConfiguration );
//    if (!EFI_ERROR(Status) && gServerMgmtConfiguration.IpmiFunctionSupport == 0x00) return EFI_NOT_READY;
//#endif  // #if defined IpmiFunction_SUPPORT && IpmiFunction_SUPPORT == 0x01     

    Status = pBS->LocateProtocol(
                 &gEfiDxeIpmiTransportProtocolGuid,
                 NULL,
                 &IpmiTransport);

    if(EFI_ERROR(Status)) return EFI_NOT_READY;

    /*gRT->GetVariable(L"Setup",
                     &SetupGuid,
                     NULL,
                     &Size,
                     &SetupData);*/

    //
    // Send BIOS SSID and build date to BMC.
    //
    Status = mSmcIpmiOemCommandSetProtocol->GetSetOobIdentify (
               mSmcIpmiOemCommandSetProtocol,
               mSmcOobPlatformPolicyPtr->OobConfigurations.BoardInfoSvid,
               mSmcOobPlatformPolicyPtr->OobConfigurations.BoardInfoDate,
               mSmcOobPlatformPolicyPtr->OobConfigurations.BoardInfoTime
               );
    if(Status) return EFI_SUCCESS;

    Status = pBS->CreateEvent(
                 EVT_NOTIFY_SIGNAL,
                 EFI_TPL_NOTIFY,
                 DownloadFileFromBMC,
                 NULL,
                 &SmEvent);
    if(!EFI_ERROR (Status)) {
        Status = pBS->RegisterProtocolNotify(
                     &gBdsAllDriversConnectedProtocolGuid,
                     SmEvent,
                     &SmReg);
    }

    //
    // Register a callback event to upload new BIOS config and SMBIOS to BMC.
    // the event must later than InBand function "SmcInBandCallback" and earier than SPI protected.
    //
    Status = RegisterProtocolCallback (
                 &gSmcOobDataReadyProtocolGuid,
                 UploadOobDataToBmc,
                 NULL,
                 &OobDataReadyEvent,
                 &Registration);

    ASSERT_EFI_ERROR(Status);

    return EFI_SUCCESS;
}
