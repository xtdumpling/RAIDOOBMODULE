//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//  Rev. 1.16
//    Bug fixed:
//    Reason   : Add a Buffer print function.
//    Auditor  : Durant Lin
//    Date     : Aug/07/2018
//
//  Rev. 1.15
//    Bug fixed:
//    Reason   : Use Oob SMM Library Protocol to replace some library functions.
//    Auditor  : Durant Lin
//    Date     : Feb/12/2018
//
//  Rev. 1.14
//    Bug fixed:
//    Reason   : Correct code error that write romhole smbios failed.
//    Auditor  : Durant Lin
//    Date     : Feb/09/2018
//
//  Rev. 1.13
//    Bug fixed:
//    Reason   : Add an interface to call platform specific SmmRomHoleLib
//    Auditor  : Durant Lin
//    Date     : Feb/07/2018
//
////  Rev. 1.12
//    Bug Fix : N/A
//    Reason  : Add a version control string for all OOB EFI files. "_SMCOOBV1.xx.xx_"
//    Auditor : Durant Lin
//    Date    : Jan/04/2018
//
//  Rev. 1.11
//    Bug Fix : N/A.
//    Reason  : Defended SMCI OOB in-band SW SMI 0xE6 with SMM communication 
//              mail box mechanism.
//    Auditor : Joe Jhang
//    Date    : Jun/06/2017
//
//  Rev. 1.10
//    Bug Fix:  Fix sum GetDmiInfo inband function failed at first time.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/04/2016
//
//  Rev. 1.09
//    Bug Fix:  Preventing Inband data from sum exceeds SmcRomHole region size.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/05/2016
//
//  Rev. 1.08
//    Bug Fix:  Fix sum can't get BIOS Config by InBand.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/23/2016
//
//  Rev. 1.07
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.06
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.05
//    Bug Fix:  Correct bin TotalDataSize for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.04
//    Bug Fix:  Add InBand HII data for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/21/2016
//
//  Rev. 1.03
//    Bug Fix:  Add BIOS config InBand function for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/18/2016
//
//  Rev. 1.02
//    Bug Fix:  Add a subcommand for SUM get major and minor version.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     Jul/14/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix system hangs at Sum InBand command.
//    Reason:   Because of using Boot Service in SMM. Separate SmcRomHole into two libraries - DXE and SMM.
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/12/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcInBand.c
//
// Description: SMC In-Band feature include BIOS config and SMBIOS.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "Token.h"
#include "SspTokens.h"
#include "SmcLib.h"
#include "SmcInBand.h"
#include <AmiSmm.h>
#include <Library\AmiBufferValidationLib.h>
#include "SmcInBandHii.h"
#include "SmcInBandFullSM.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include "SmcRomHole.h"
#include <Protocol/SmcRomHoleProtocol.h>
#include <Protocol/SmcOobLibraryProtocol.h>
//#include "a7printf.h"
#include "SmcOobVersion.h"

static UINT8 *gSmcOobSmRam = NULL;

EFI_SMM_BASE2_PROTOCOL  *SmmBase;
EFI_SMM_SYSTEM_TABLE2   *gSmst;
EFI_SMM_CPU_PROTOCOL    *gSmmCpu;

SMC_SMM_ROM_HOLE_PROTOCOL		*mSmcSmmRomHoleProtocol;
SMC_SMM_OOB_LIBRARY_PROTOCOL	*mSmcSmmOobLibraryProtocol;

UINT8		*gPtrMem = NULL;
UINTN		MemSize = 0x40000;
SMC_OOB_PLATFORM_POLICY *mSmcOobPlatformPolicyPtr;
SMC_OOB_PLATFORM_POLICY mSmcOobPlatformPolicy;
UINT32                  mCmosSmcAddrHH;
UINT32                  mCmosSmcAddrH;
UINT32                  mCmosSmcAddrL;
UINT32                  mCmosSmcAddrLL;

/**
Validate the buffer address is within smm communication buffer.

@param Buffer parameter that specifies Buffer address to be Validated.
@param BufferSize parameter that specifies Buffer size.

@retval EFI_SUCCESS Validate pass.
**/
EFI_STATUS CheckSmmCommunicationBuffer (
    CONST VOID* Buffer, CONST UINTN BufferSize
    );



void PrintBuffer(CHAR16* DumpName, VOID*	pBuffer, UINTN BufferSize){
#if DEBUG_MODE == 1
	UINTN index = 0;
	UINT8* Buffer = pBuffer;
	DEBUG((-1,"\n[SMC_OOB] :: >>> %s CONTENT : START <<< \n",DumpName));
	for(index = 0;index < BufferSize;++index){
		if(index % 16 == 0) DEBUG((-1,"%08x : "));
		DEBUG((-1," %02x",Buffer[index]));
		if((index + 1) % 16 == 0) DEBUG((-1,"\n"));
	}
	DEBUG((-1,"\n[SMC_OOB] :: >>> %s CONTENT : END   <<< \n\n",DumpName));
#endif
	return;
}



VOID
UploadDataToRAM(
  IN OUT InBand_Base_Header	*InBandBaseHeader,
  IN     UINT8              InBand_Area_Type
  )
{
  InBand_Smbios_Data_Header	*InBandSmbiosHeader = NULL;
  InBand_BIOS_Cfg_Header* InBandBiosCfgHeader = NULL;
  INBAND_HII_FILE_HEADER* HiiFileHeader = NULL;
  InBand_FullSmBios_Header* FullSmBiosHeader = NULL;
  UINT32	TotalLength = 0;

  DEBUG((-1, "UploadDataToRAM entry.\n"));

  InBandBaseHeader->ReturnStatus = 0;	//upload success

  if(InBand_Area_Type == 0x00) { // BIOS config
      UINT8* pDataBuff = (UINT8*)(*(UINT64*)InBandBaseHeader->DataBufferAddress);
      InBandBiosCfgHeader = (InBand_BIOS_Cfg_Header*)(gSmcOobSmRam + SMC_OOB_SMRAM_BIOS_CFG_OFFSET);
      TotalLength = InBandBiosCfgHeader->FileSize + sizeof (InBand_BIOS_Cfg_Header) + 1; // There is one byte of checksum at the end, so plus 1.
      MemCpy (pDataBuff, (UINT8*)InBandBiosCfgHeader, TotalLength);
	  
	  PrintBuffer(L"UploadDataToRAM BIOS CONFIG",pDataBuff,TotalLength);
	  
  }
  else if(InBand_Area_Type == 0x01) { // Hii data
      UINT8* pDataBuff = (UINT8*)(*(UINT64*)InBandBaseHeader->DataBufferAddress);
      HiiFileHeader = (INBAND_HII_FILE_HEADER*)(gSmcOobSmRam + SMC_OOB_SMRAM_HII_OFFSET);
      TotalLength = (UINT32)HiiFileHeader->FileSize + sizeof(INBAND_HII_FILE_HEADER);
      MemCpy (pDataBuff, (UINT8*)HiiFileHeader, TotalLength);
	  PrintBuffer(L"UploadDataToRAM Hii Data",pDataBuff,TotalLength);
  }
  else if(InBand_Area_Type == 0x02) { // SMBIOS
      InBandSmbiosHeader = (InBand_Smbios_Data_Header*)(*(UINT64*)InBandBaseHeader->DataBufferAddress);
	  mSmcSmmRomHoleProtocol->SmcRomHoleReadRegion(CURRENT_SMBIOS_REGION,(UINT8*)InBandSmbiosHeader);
      TotalLength = (UINT32)InBandSmbiosHeader->FileSize + sizeof(InBand_Smbios_Data_Header);
	  PrintBuffer(L"UploadDataToRAM SMBIOS",InBandSmbiosHeader,TotalLength);

  }
  else if(InBand_Area_Type == 0x03) { // Full SMBIOS
      UINT8* pDataBuff = (UINT8*)(*(UINT64*)InBandBaseHeader->DataBufferAddress);
      FullSmBiosHeader = (InBand_FullSmBios_Header*)(gSmcOobSmRam + SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET);
      TotalLength = (UINT32)FullSmBiosHeader->FileSize + sizeof(InBand_FullSmBios_Header);
      MemCpy (pDataBuff, (UINT8*)FullSmBiosHeader, TotalLength);
	  PrintBuffer(L"UploadDataToRAM Full SMBIOS",pDataBuff,TotalLength);
  }
  else if(InBand_Area_Type == 0x80) {	//Platform info
	 	mSmcSmmOobLibraryProtocol->Smc_GenerateFeatureSupportFlags((UINT8*)(*(UINT64*)InBandBaseHeader->DataBufferAddress), &TotalLength, &mSmcOobPlatformPolicy);
		PrintBuffer(L"UploadDataToRAM FeatureFlags",(UINT8*)(*(UINT64*)InBandBaseHeader->DataBufferAddress),TotalLength);
  }

  *(UINT32*)InBandBaseHeader->TotalDataSize = TotalLength;
  DEBUG((-1, "TotalDataBufferSize = %x\n", *(UINT32*)InBandBaseHeader->TotalDataSize));
  
  // Check data size, if data size over 256k(SUM limitation), return data size over buffer.
  if(*(UINT32*)InBandBaseHeader->TotalDataSize > SMM_COMMUNICATION_BUFFER_SIZE)
      InBandBaseHeader->ReturnStatus = 0x01; // Data size exceeds SMM_COMMUNICATION_BUFFER_SIZE.
  else if(!TotalLength) // If TotalLength == 0
      InBandBaseHeader->ReturnStatus = 0x03; // Data size is zero
  else
      InBandBaseHeader->ReturnStatus = 0x00; // Upload success
  
  DEBUG((-1, "UploadDataToRAM end.\n"));
}

VOID
DownloadDataFromRAM(
  IN OUT InBand_Base_Header	*InBandBaseHeader,
  IN     UINT8	            InBand_Area_Type
)
{
  InBand_BIOS_Cfg_Header* InBandBiosCfgHeader = NULL;
  InBand_Smbios_Data_Header *InBandSmbiosHeader = NULL;
  UINT32	TotalLength = 0;
#if DEBUG_MODE == 1
  UINT8*		TempBuffer = NULL;
  UINT32		RomHoleSize = 0;
  EFI_STATUS 	Status = EFI_SUCCESS;
#endif
  DEBUG((-1, "DownloadDataFromRAM Start.\n"));

  InBandBaseHeader->ReturnStatus = 0x00;

//empty tempbuffer and save settings to rom, update setting during next POST
  if(InBand_Area_Type == 0) {
      InBandBiosCfgHeader = (InBand_BIOS_Cfg_Header*)(*(UINT64*)InBandBaseHeader->DataBufferAddress);
      TotalLength = InBandBiosCfgHeader->FileSize + sizeof (InBand_BIOS_Cfg_Header) + 1; // There is one byte of checksum at the end, so plus 1.

	  DEBUG((-1,"[SMC_OOB] :: RomHole Size - INBAND_REGION %x, Data to be wrote Size %x\n",mSmcSmmRomHoleProtocol->SmcRomHoleSize(INBAND_REGION),TotalLength));
	  PrintBuffer(L"DownloadDataFromRAM BIOS CFG from OutSite Memory",InBandBiosCfgHeader,TotalLength);

	  if(mSmcSmmRomHoleProtocol->SmcRomHoleSize(INBAND_REGION) >= TotalLength){
	  	  mSmcSmmRomHoleProtocol->SmcRomHoleWriteRegion(INBAND_REGION,(UINT8*)InBandBiosCfgHeader);

#if DEBUG_MODE == 1
		  	RomHoleSize = mSmcSmmRomHoleProtocol->SmcRomHoleSize(INBAND_REGION);
  			Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData,RomHoleSize,&TempBuffer);
			if(!EFI_ERROR(Status)){
				MemSet(TempBuffer,RomHoleSize,0x00);
				mSmcSmmRomHoleProtocol->SmcRomHoleReadRegion(INBAND_REGION,TempBuffer);
				PrintBuffer(L"DownloadDataFromRAM BIOS CFG into ROMHOLE",TempBuffer,RomHoleSize);
				gSmst->SmmFreePool(TempBuffer);
			}
#endif

	  }
      else {
          InBandBaseHeader->ReturnStatus = 0x01; // Data size exceeds SmcRomHole region size.
      }
  }
  else if(InBand_Area_Type == 2) {
      InBandSmbiosHeader = (InBand_Smbios_Data_Header*)(*(UINT64*)InBandBaseHeader->DataBufferAddress);
      TotalLength = (UINT32)InBandSmbiosHeader->FileSize + sizeof(InBand_Smbios_Data_Header);

	  DEBUG((-1,"[SMC_OOB] :: RomHole Size - CURRENT_SMBIOS_REGION %x, Data to be wrote Size %x\n",mSmcSmmRomHoleProtocol->SmcRomHoleSize(CURRENT_SMBIOS_REGION),TotalLength));
	  PrintBuffer(L"DownloadDataFromRAM SMBIOS REGION from OutSite Memory",InBandBiosCfgHeader,TotalLength);

	  if(mSmcSmmRomHoleProtocol->SmcRomHoleSize(CURRENT_SMBIOS_REGION) >= TotalLength){
	  	  mSmcSmmRomHoleProtocol->SmcRomHoleWriteRegion(CURRENT_SMBIOS_REGION,(UINT8*)InBandSmbiosHeader);

#if DEBUG_MODE == 1
		  	RomHoleSize = mSmcSmmRomHoleProtocol->SmcRomHoleSize(CURRENT_SMBIOS_REGION);
  			Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData,RomHoleSize,&TempBuffer);
			if(!EFI_ERROR(Status)){
				MemSet(TempBuffer,RomHoleSize,0x00);
				mSmcSmmRomHoleProtocol->SmcRomHoleReadRegion(CURRENT_SMBIOS_REGION,TempBuffer);
				PrintBuffer(L"DownloadDataFromRAM SMBIOS REGION into ROMHOLE",TempBuffer,RomHoleSize);
				gSmst->SmmFreePool(TempBuffer);
			}
#endif

	  }
      else {
          InBandBaseHeader->ReturnStatus = 0x01; // Data size exceeds SmcRomHole region size.
      }
  }
  
  DEBUG((-1, "[SMC_OOB] :: DownloadDataFromRAM End.\n"));
}

EFI_STATUS
SmcInBandSwSmiE8Handler(
  IN     EFI_HANDLE DispatchHandle,
  IN     CONST VOID *Context OPTIONAL,
  IN OUT VOID       *CommBuffer OPTIONAL,
  IN OUT UINTN      *CommBufferSize OPTIONAL
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 DataPort = 0xff;
  UINT8* pBuffer = NULL;
  UINT32 Data32 = 0;
  UINT32 Temp32 = 0;
  INBAND_HII_FILE_HEADER* InbandHiiFileHeader = NULL;
  InBand_FullSmBios_Header* FullSmBiosHeader = NULL;
  InBand_BIOS_Cfg_Header* BiosCfgHeader = NULL;

  DEBUG((-1, "[SMC_OOB] :: SmcInBandSwSmiE8Handler entry.\n"));
	
  	Data32 = mSmcSmmOobLibraryProtocol->Smc_OobGetCmosTokensValue(mCmosSmcAddrHH);
  Temp32 |= Data32 << 24;
  	Data32 = mSmcSmmOobLibraryProtocol->Smc_OobGetCmosTokensValue(mCmosSmcAddrH);
  Temp32 |= Data32 << 16;
  	Data32 = mSmcSmmOobLibraryProtocol->Smc_OobGetCmosTokensValue(mCmosSmcAddrL);
  Temp32 |= Data32 << 8;
  	Data32 = mSmcSmmOobLibraryProtocol->Smc_OobGetCmosTokensValue(mCmosSmcAddrLL);
  Temp32 |= Data32;
  pBuffer = (UINT8*)Temp32;

  DataPort = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->DataPort;

	DEBUG((-1,"[SMC_OOB] :: TRANSFER ADDRESS : %16x, DataPort Cmd : %x\n",Temp32,DataPort)); 

  switch(DataPort) {
  case SWSMI_E8_SUBCMD_COMBINE_HII:
      InbandHiiFileHeader = (INBAND_HII_FILE_HEADER*)(pBuffer + SMC_OOB_SMRAM_HII_OFFSET);

      //
      // Copies HII data to SMRAM.
      //
      MemCpy (
          gSmcOobSmRam + SMC_OOB_SMRAM_HII_OFFSET,
          pBuffer + SMC_OOB_SMRAM_HII_OFFSET,
          InbandHiiFileHeader->FileSize + sizeof (INBAND_HII_FILE_HEADER)
      );

      break;
  case SWSMI_E8_SUBCMD_COMBINE_FULL_SMBIOS:
      FullSmBiosHeader = (InBand_FullSmBios_Header*)(pBuffer + SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET);

      //
      // Copies Full SmBios data to SMRAM.
      //
      MemCpy (
          gSmcOobSmRam + SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET,
          pBuffer + SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET,
          FullSmBiosHeader->FileSize + sizeof (InBand_FullSmBios_Header)
      );

      break;
  case SWSMI_E8_SUBCMD_COMBINE_BIOS_CFG:
      BiosCfgHeader = (InBand_BIOS_Cfg_Header*)(pBuffer + SMC_OOB_SMRAM_BIOS_CFG_OFFSET);

      //
      // Copies Full SmBios data to SMRAM.
      //
      MemCpy (
          gSmcOobSmRam + SMC_OOB_SMRAM_BIOS_CFG_OFFSET,
          pBuffer + SMC_OOB_SMRAM_BIOS_CFG_OFFSET,
          BiosCfgHeader->FileSize + sizeof (InBand_BIOS_Cfg_Header) + 1 // There is a byte of checksum at the end.
      );

      break;
  }

  DEBUG((-1, "[SMC_OOB] :: SmcInBandSwSmiE8Handler end.\n"));

  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SmcInBandSwSmiE6Handler
//
// Description:
//  Software SMI handler for SmcInBand.
//
//  Steps :
//
//  1. Search for the CPU that caused the software Smi.
//     If it's invalid CPU, return.
//  2. Get the data of "InBandBaseHeader" from application memory.
//  3. Check the sub command - "InBandBaseHeader->SubCommand" and execute different routines.
//     The sub commands are as below.
//     0x01 - DownloadDataFromRAM, Write InBand data to flash part by parameter - "InBandBaseHeader".
//     0x03 - (FileType=0) UploadBiosCfgToRAM, Get all variables and put them to memory buffer.
//     0x03 - (FileType=1) UploadDatFileToRAM, Get DAT file data from flash part and put them to memory buffer.
//     0x03 - (FileType=2) UploadSmbiosFileToRAM, Get SMBIOS from flash part and put them to memory buffer.
//
// Input:
//      IN EFI_HANDLE DispatchHandle - Dispatch Handle.
//      IN EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext - Dispatch Context.
//
// Output:
//      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SmcInBandSwSmiE6Handler(
  IN     EFI_HANDLE DispatchHandle,
  IN	 CONST VOID	*Context OPTIONAL,
  IN OUT VOID	    *CommBuffer OPTIONAL,
  IN OUT UINTN	    *CommBufferSize OPTIONAL
  )
{
  EFI_STATUS	Status = EFI_SUCCESS;
  InBand_Base_Header		*InBandBaseHeader;
  UINT32      HighBufferAddress = 0, LowBufferAddress = 0;
  UINT64      pCommBuff;
  UINTN       Cpu = (UINTN)-1;

  DEBUG((-1, "SmcInBandSwSmiE6Handler entry.\n"));

  Cpu = ((EFI_SMM_SW_CONTEXT*)CommBuffer)->SwSmiCpuIndex;
  //
  // Found Invalid CPU number, return
  //
  if(Cpu == (UINTN)-1)	return Status;

  Status = gSmmCpu->ReadSaveState(
                      gSmmCpu,
                      4,
                      EFI_SMM_SAVE_STATE_REGISTER_RBX,
                      Cpu,
                      &LowBufferAddress
                      );
  
  Status = gSmmCpu->ReadSaveState(
                      gSmmCpu,
                      4,
                      EFI_SMM_SAVE_STATE_REGISTER_RCX,
                      Cpu,
                      &HighBufferAddress);
  
  pCommBuff           = HighBufferAddress;
  pCommBuff           = Shl64(pCommBuff, 32);
  pCommBuff           += LowBufferAddress;

    if(EFI_ERROR(AmiValidateMemoryBuffer ((UINT8*)pCommBuff, SMM_COMMUNICATION_BUFFER_SIZE))) return EFI_ACCESS_DENIED;

  DEBUG((-1, "pCommBuff address = %lx.\n", pCommBuff));

  InBandBaseHeader = (InBand_Base_Header*)pCommBuff;
  DEBUG((-1, "HeaderLength = %x.\n", *(UINT16*)(InBandBaseHeader->HeaderLength)));
  DEBUG((-1, "SubCommand = %x.\n", InBandBaseHeader->SubCommand));
  DEBUG((-1, "FileType = %x.\n", InBandBaseHeader->FileType));
  DEBUG((-1, "DataBufferAddress = %lx.\n", *(UINT64*)(InBandBaseHeader->DataBufferAddress)));

  switch(InBandBaseHeader->SubCommand) {
  case 0x01:
      DownloadDataFromRAM(InBandBaseHeader, InBandBaseHeader->FileType);	//0 = BIOS settings, 2 = SMBIOS
      break;

  case 0x02:
      break;

  case 0x03:
      UploadDataToRAM(InBandBaseHeader, InBandBaseHeader->FileType);
      break;
  }

  DEBUG((-1, "TotalDataSize = %x.\n", *(UINT32*)(InBandBaseHeader->TotalDataSize)));
  DEBUG((-1, "ReturnStatus = %x.\n", InBandBaseHeader->ReturnStatus));
  DEBUG((-1, "SmcInBandSwSmiE6Handler end.\n"));
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  InitSmcInBand
//
// Description:
//  Init SmcInBand.
//  Define a SMC software SMI to handle SMC SW (SUM) for date transfer butween
//  BIOS and SUM in Linux.
//
// Input:
//      IN EFI_HANDLE ImageHandle
//                  - Image handle
//      IN EFI_SYSTEM_TABLE *SystemTable
//                  - System table pointer
//
// Output:
//      EFI_STATUS
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
InitSmcInBandSmm(
  IN     EFI_HANDLE          ImageHandle,
  IN     EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;
  EFI_SMM_SW_DISPATCH2_PROTOCOL	*pSwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT   SwE6Context;
  EFI_SMM_SW_REGISTER_CONTEXT   SwE8Context;
    UINT8* pBuffer = NULL;
    UINT32 Data32 = 0;
    UINT32 Temp32 = 0;

  	DEBUG((-1, "[SMC_OOB] :: InitSmcInBandSmm entry.\n"));

    Status = gBS->LocateProtocol (&gSmcOobPlatformPolicyGuid, NULL, &mSmcOobPlatformPolicyPtr);
    if (EFI_ERROR (Status)) {
    	DEBUG((-1,"[SMC_OOB] :: Locate SmcOobPlatformPolicy Protocol failed! Status = %r\n",Status));
		return EFI_NOT_READY; 
    }

    MemCpy (&mSmcOobPlatformPolicy, mSmcOobPlatformPolicyPtr, sizeof (SMC_OOB_PLATFORM_POLICY));

    SwE6Context.SwSmiInputValue = mSmcOobPlatformPolicy.OobConfigurations.InBandSwSmiE6;
    SwE8Context.SwSmiInputValue = mSmcOobPlatformPolicy.OobConfigurations.InBandSwSmiE8;

    mCmosSmcAddrHH = mSmcOobPlatformPolicy.OobConfigurations.CmosSmcAddrHH;
    mCmosSmcAddrH  = mSmcOobPlatformPolicy.OobConfigurations.CmosSmcAddrH;
    mCmosSmcAddrL  = mSmcOobPlatformPolicy.OobConfigurations.CmosSmcAddrL;
    mCmosSmcAddrLL = mSmcOobPlatformPolicy.OobConfigurations.CmosSmcAddrLL;


  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  &SmmBase
                  );
  	if (EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: Locate SmmBase2 Protocol failed! Status = %r\n",Status));
		return EFI_NOT_READY;
	}

  	Status = SmmBase->GetSmstLocation (SmmBase, &gSmst);

	if(EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: GetSmstLocation failed! Status = %r\n",Status));
		return EFI_NOT_READY;
	}
  	Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    &gSmmCpu
                    );
  	if(EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: SmmLocateProtocol gEfiSmmCpuProtocol failed! Status = %r\n",Status));
		return EFI_NOT_READY;
	}

  Status = gSmst->SmmLocateProtocol(
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    &pSwDispatch2
                    );
  	if(EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: SmmLocateProtocol SmmSwDispatch2Protocol failed! Status = %r\n",Status));
		return EFI_NOT_READY;
	}
  	Status = gSmst->SmmLocateProtocol(
		  		&gSmcSmmRomHoleProtocolGuid,
		  		NULL,
				&mSmcSmmRomHoleProtocol
		  	);

 	if(EFI_ERROR(Status)){ 
  		DEBUG((-1,"[SMC_OOB] :: SmmLocateProtocol gSmcSmmRomHoleProtocolGuid failed! Status = %r\n",Status));
		return EFI_NOT_READY;
	}

  	Status = gSmst->SmmLocateProtocol(
		  		&gSmcSmmOobLibraryProtocolGuid,
		  		NULL,
				&mSmcSmmOobLibraryProtocol
		  	);
   	if(EFI_ERROR(Status)){ 
		DEBUG((-1,"[SMC_OOB] :: SmmLocateProtocol gSmcSmmOobLibraryProtocolGuid failed! Status = %r\n",Status));
		return EFI_NOT_READY; 
  	}
		//
  // Allocates a SMRAM for OOB.
  //
  	Status = gSmst->SmmAllocatePool (
           		EfiRuntimeServicesData,
           		SMC_OOB_SMRAM_TOTAL_SIZE,
           		&gSmcOobSmRam
           	  );
	if(EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: SmmAllocatePool failed! Size = %x, Status = %r\n",SMC_OOB_SMRAM_TOTAL_SIZE,Status));
		return EFI_NOT_READY; 	
	}

  //
  // Registers SW SMI.
  //
  	Status = pSwDispatch2->Register(
                           	pSwDispatch2,
                           	SmcInBandSwSmiE6Handler,
                          	&SwE6Context,
                           	&Handle
                           );
	if(EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: pSwDispatch2 Register SmcInBandSwSmiE6Handler failed! Status = %r\n",Status));
		return EFI_NOT_READY; 	
	}

  	Status = pSwDispatch2->Register(
                           	pSwDispatch2,
                           	SmcInBandSwSmiE8Handler,
                           	&SwE8Context,
                           	&Handle
                           );
	if(EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: pSwDispatch2 Register SmcInBandSwSmiE8Handler failed! Status = %r\n",Status));
		return EFI_NOT_READY;
	}
  DEBUG((-1, "[SMC_OOB] :: InitSmcInBandSmm End.\n"));
  
  return EFI_SUCCESS;
}
