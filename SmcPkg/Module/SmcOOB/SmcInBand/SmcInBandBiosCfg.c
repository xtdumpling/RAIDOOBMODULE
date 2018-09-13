//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.21
//    Bug Fix:  
//    Reason: Change free metho in one way.
//    Auditor:  Durant Lin
//    Date:     Aug/14/2018
//
//  Rev. 1.20
//    Bug Fix:  
//    Reason: Improve UpdateVariable procedure.
//    Auditor:  Durant Lin
//    Date:     Aug/07/2018
//
//  Rev. 1.19
//    Bug Fix:  
//    Reason: Move VarGuid to platformPolicyDxe.
//    Auditor:  Durant Lin
//    Date:     Aug/07/2018
//
//  Rev. 1.18
//    Bug Fix:  Add a function hook before LoadDefault.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jun/12/2018
//
//  Rev. 1.17
//    Bug Fix:  
//    Reason:   Modify Debug message.
//    Auditor:  Durant Lin
//    Date:     Jun/01/2018
//
//  Rev. 1.16
//    Bug Fix:  Add a interface to hook updatevariable before.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jan/17/2018
//
//  Rev. 1.15
//    Bug Fix:  Hide unused forms to fix sum default value errors.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/19/2016
//
//  Rev. 1.14
//    Bug Fix:  Add variable GUID in bin for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/23/2016
//
//  Rev. 1.13
//    Bug Fix:  Add variable - Timeout  for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/20/2016
//
//    Rev.  1.12
//     Bug Fix: Fix some variables can't be saved by sum.
//     Reason:
//     Auditor:	Hartmann Hsieh
//     Date:  	Oct/12/2016
//
//    Rev.  1.11
//     Bug Fix: Add sum load default function.
//     Reason:
//     Auditor:	Hartmann Hsieh
//     Date:  	Oct/11/2016
//
//  Rev. 1.10
//    Bug Fix:  Fix some variables didn't be sent to sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/26/2016
//
//  Rev. 1.09
//    Bug Fix:  Fix sum can't get BIOS Config by InBand.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/23/2016
//
//  Rev. 1.08
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.07
//    Bug Fix:  Fix setup menu root page titles are disappeared.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/17/2016
//
//  Rev. 1.06
//    Bug Fix:  Combine all variables for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/17/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix some errors for ChangeBiosCfg.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/09/2016
//
//  Rev. 1.04
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.03
//    Bug Fix:  Extend FileSize in InBand header.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/22/2016
//
//  Rev. 1.02
//    Bug Fix:  Add BIOS config InBand function for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/18/2016
//
//  Rev. 1.01
//    Bug Fix:  Use AMIBCP to disable boot procedure messages displaying.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/21/2016
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
// Name:        SmcInBandBiosCfg.c
//
// Description: In-Band BIOS configure update feature.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <AmiLib.h>
#include <SspTokens.h>
#include "Token.h"
#include "SmcLib.h"
#include "SmcInBand.h"
#include "SmcRomHole.h"
#include <Protocol/SmcRomHoleProtocol.h>
#include <Protocol/SmcOobLibraryProtocol.h>
#include <Protocol/SmcLsiRaidOOBSetupDriver.h>

extern SMC_OOB_PLATFORM_POLICY      	*mSmcOobPlatformPolicyPtr;
extern SMC_ROM_HOLE_PROTOCOL        	*mSmcRomHoleProtocol;
extern SMC_OOB_LIBRARY_PROTOCOL			*mSmcOobLibraryProtocol;
extern SMC_LSI_RAID_OOB_SETUP_DRIVER	*mSmcLsiRaidOOBSetupDriver;
 
#define DEFAULTS_GUID  \
  {0x4599d26f, 0x1a11, 0x49b8, 0xb9, 0x1f, 0x85, 0x87, 0x45, 0xcf, 0xf8, 0x24}

#define MAX_STDDEFAULTS_SIZE 0x100000

#define FREE_NVSTRUCT(_X)\
		do {\
			gBS->FreePool(_X->SmcRaidVarName);\
			gBS->FreePool(_X->SmcRaidVarGuid);\
			gBS->FreePool(_X->SmcRaidVarSize);\
			gBS->FreePool(_X->SmcRaidVarBuffer);\
		}while(FALSE)

VOID
CombineVariable(
    IN CHAR16 *VariableName,
    IN EFI_GUID* VariableGuid,
    IN UINTN VariableSize,
    IN UINT8 *VariableBuffer,
    IN OUT UINT32 *TotalDataBufferPtr,
    IN OUT UINT8 *TotalDataBuffer
)
{
    UINT32	TempPtr;

	DEBUG((-1,"[SMC_OOB] :: CombineVariable !\n"));
    TempPtr = *TotalDataBufferPtr;
//variable name string length - 1 byte
    TotalDataBuffer[TempPtr] = (UINT8)StrLen(VariableName);
    TempPtr++;
//variable string - n bytes (string length)
    UnicodeStrToAsciiStr(VariableName, &TotalDataBuffer[TempPtr]);
    TempPtr += (UINT8)StrLen(VariableName);
//variable GUID - 16 bytes
    gBS->CopyMem(&TotalDataBuffer[TempPtr], VariableGuid, sizeof (EFI_GUID));
    TempPtr += (UINT32)sizeof (EFI_GUID);
//variable length - 2 bytes
    *(UINT16*)&TotalDataBuffer[TempPtr] = (UINT16)VariableSize;
    TempPtr += 2;
//variable values - m bytes (variable length)
    gBS->CopyMem(&TotalDataBuffer[TempPtr], VariableBuffer, VariableSize);
    TempPtr += (UINT32)VariableSize;

    *TotalDataBufferPtr = TempPtr;
}

EFI_STATUS UpdateStdDefaults ()
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID mStdDefGuid = DEFAULTS_GUID;
    UINT8 *pStdDefaults = NULL;
    UINTN StdDefaultsSize = 0;
    NVAR *pNvar = NULL;
    UINT32 NameStrLen = 0;

	DEBUG((-1,"[SMC_OOB] :: UpdateStdDefaults !\n"));

    StdDefaultsSize = MAX_STDDEFAULTS_SIZE;
    Status = gBS->AllocatePool(EfiBootServicesData, StdDefaultsSize, &pStdDefaults);
	if(EFI_ERROR(Status)) return Status;

    Status = gRT->GetVariable (
                 L"StdDefaults",
                 &mStdDefGuid,
                 NULL,
                 &StdDefaultsSize,
                 pStdDefaults);

    if (EFI_ERROR (Status)) return Status;

    //
    // Loop Defaults.bin data. Find the default data of VariableName.
    //
    pNvar = (NVAR*)pStdDefaults;
    while (1)
    {
        //
        // Check signature is "NVAR"
        //
        if (pNvar->signature != 0x5241564e) break;

        NameStrLen = (UINT32)Strlen(pNvar->name);

        if (Strcmp (pNvar->name, "PchRcConfiguration") == 0) {
        }
        else if (Strcmp (pNvar->name, "IntelSetup") == 0) {
        }
        else if (Strcmp (pNvar->name, "SocketMemoryConfig") == 0) {
        }

        pNvar = (NVAR*)((UINT8*)pNvar + pNvar->size);
    }

    if (pStdDefaults != NULL) gBS->FreePool(pStdDefaults);

    return EFI_SUCCESS;
}

EFI_STATUS
CombineBiosCfg(
    IN OUT	UINT8	*TotalDataBuffer,
    IN OUT	UINT32	*TotalDataBufferPtr
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 *TempBuffer = NULL;
    UINT8 checksum;
    UINT32	i;
    UINTN	VariableSize;
    EFI_GUID VendorGuidI;
    CHAR16 *VariableNameI = NULL;
    UINTN VariableNameISize;
    UINTN VariableNameISizeNew;

    DEBUG((-1, "[SMC_OOB] :: CombineBiosCfg entry.\n"));

   	Status = UpdateStdDefaults ();
	DEBUG((-1,"[SMC_OOB] ::  UpdateStdDefaults Status = %r\n",Status));

    *TotalDataBufferPtr += sizeof(InBand_BIOS_Cfg_Header);	//for BIOS config header

    VariableNameISize = 256 * sizeof (CHAR16);
    Status = gBS->AllocatePool(EfiBootServicesData, VariableNameISize, &VariableNameI);
    if (EFI_ERROR (Status)) {
		DEBUG((-1,"[SMC_OOB] :: AllocatePool Status = %r\n",Status));
        return Status;
    }
    VariableNameI[0] = L'\0';

    VariableSize = 0x4000;
    Status = gBS->AllocatePool(EfiBootServicesData, VariableSize, &TempBuffer);
    if (EFI_ERROR (Status)) {
        if (VariableNameI != NULL) gBS->FreePool(VariableNameI);
        return Status;
    }

    //
    // Scan all variables in system.
    //
    while (1) {
        //
        // Get the variable name one by one in the variable database.
        //
        VariableNameISizeNew = VariableNameISize;
        Status = gRT->GetNextVariableName (
                     &VariableNameISizeNew,
                     VariableNameI,
                     &VendorGuidI
                 );
        if (Status == EFI_BUFFER_TOO_SMALL) {
            DEBUG((-1, "[SMC_OOB] :: ERROR : Variable name size exceeds buffer size.\n"));
            continue;
        }
        else if (EFI_ERROR (Status)) {
            break;
        }

        VariableSize = 0x4000;
        Status = gRT->GetVariable(
                     VariableNameI,
                     &VendorGuidI,
                     NULL,
                     &VariableSize,
                     TempBuffer);
        if (!EFI_ERROR (Status)) {
#if DEBUG_MODE == 1
            {
                CHAR8 StrBuff2[128] = {0};
                UINT32 TmpSize = (UINT32)VariableSize;
                UnicodeStrToAsciiStr(VariableNameI, StrBuff2);
                DEBUG((-1, "[SMC_OOB] :: VariableName = %s\n", VariableNameI));
                DEBUG((-1, "[SMC_OOB] :: GUID = %g\n", &VendorGuidI));
                DEBUG((-1, "[SMC_OOB] :: VariableSize = 0x%x\n", VariableSize));
                DEBUG((-1, "[SMC_OOB] :: Start Address = 0x%x\n", *TotalDataBufferPtr));
                if (Strcmp (StrBuff2, "AMITSESetup") == 0) {
                    UINT8* TempData = (UINT8*)(&(TempBuffer[0]));
                    UINT32 i = 0;
                    for (i = 0; i < VariableSize; i++) {
                        if (i == 0)
                            DEBUG((-1, "%04x0   ", (i / 16)));
                        else if (i != 0 && i % 16 == 0) {
                            DEBUG((-1, "\n%04x0   ", (i / 16)));
                        }
                        DEBUG((-1, "%02x ", TempData[i]));
                    }
                    DEBUG((-1, "\n"));
                }
            }
#endif // #if DEBUG_MODE == 1
            CombineVariable(VariableNameI, &VendorGuidI, VariableSize, TempBuffer, TotalDataBufferPtr, TotalDataBuffer);
        }
    }
	if(!!mSmcLsiRaidOOBSetupDriver){
		SMC_LSI_RAID_OOB_SETUP_NVSTRUCT*	pRaidNvStruct = NULL;
		Status = gBS->AllocatePool(EfiBootServicesData,sizeof(SMC_LSI_RAID_OOB_SETUP_NVSTRUCT),&pRaidNvStruct);
		if(!EFI_ERROR(Status)) {
			MemSet(pRaidNvStruct,sizeof(SMC_LSI_RAID_OOB_SETUP_NVSTRUCT),0x00);
			while(!EFI_ERROR(mSmcLsiRaidOOBSetupDriver->SmcLsiSetupDriverGetNvData(mSmcLsiRaidOOBSetupDriver,FALSE,pRaidNvStruct))){
				DEBUG((-1,"[SMC_OOB] :: SMC RAID Var Name[%s], Guid[%g], VarSize[%x]\n",
						   pRaidNvStruct->SmcRaidVarName,pRaidNvStruct->SmcRaidVarGuid,*pRaidNvStruct->SmcRaidVarSize));

				CombineVariable(pRaidNvStruct->SmcRaidVarName, 
								pRaidNvStruct->SmcRaidVarGuid, 
								*pRaidNvStruct->SmcRaidVarSize, 
								pRaidNvStruct->SmcRaidVarBuffer, 
								TotalDataBufferPtr, 
								TotalDataBuffer);
				FREE_NVSTRUCT(pRaidNvStruct);
			}
			gBS->FreePool(pRaidNvStruct);
		}
	}

    if (TempBuffer != NULL) gBS->FreePool(TempBuffer);
    if (VariableNameI != NULL) gBS->FreePool(VariableNameI);

    //calculate data checksum exclude header
    checksum = 0;
    for(i = sizeof(InBand_BIOS_Cfg_Header); i < *TotalDataBufferPtr; i++)
        checksum += TotalDataBuffer[i];

    //fill header
    gBS->SetMem(TotalDataBuffer, sizeof(InBand_BIOS_Cfg_Header), 0x00);
    CopyMem(((InBand_BIOS_Cfg_Header*)TotalDataBuffer)->Signature, "_BIN", 4);
    ((InBand_BIOS_Cfg_Header*)TotalDataBuffer)->Version = mSmcOobPlatformPolicyPtr->OobConfigurations.BiosCfgVersion;
    ((InBand_BIOS_Cfg_Header*)TotalDataBuffer)->FileSize = \
            (UINT32)(*TotalDataBufferPtr - sizeof(InBand_BIOS_Cfg_Header));
    ((InBand_BIOS_Cfg_Header*)TotalDataBuffer)->Checksum = checksum;

    //calculate data checksum include header
    checksum = 0;
    for(i = 0; i < *TotalDataBufferPtr; i++)
        checksum += TotalDataBuffer[i];

    TotalDataBuffer[*TotalDataBufferPtr] = checksum;
    (*TotalDataBufferPtr)++;

    DEBUG((-1, "[SMC_OOB] :: CombineBiosCfg end.\n"));
    return Status;
}

EFI_STATUS
UpdateVariable(
    IN CHAR16 *VariableName,
    IN EFI_GUID* VariableGuid,
    IN UINTN VariableSize,
    IN UINT8 *VariableBuffer
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
	Variable_GUID*	VarGUID = NULL;
	static UINT8 VarCount = 0; 
    UINT8  i;
	UINT8* TempBuffer = NULL;
    UINTN VarSize = 0;
    UINT32 Attr = 0;

    DEBUG((-1, "[SMC_OOB] :: UpdateVariable entry.\n"));

	VarGUID = mSmcOobPlatformPolicyPtr->PlatformVar;

	if(VarCount == 0)
		for(VarCount = 0 ; StrCmp(VarGUID[VarCount].Var,L"SMC_VAR_END") != 0; VarCount++); 

	DEBUG((-1, "[SMC_OOB] :: VarCount = 0x%x\n",VarCount));
    
	Status = EFI_UNSUPPORTED;
    Status = gBS->AllocatePool(EfiBootServicesData, 0x2000, &TempBuffer);
	if(EFI_ERROR(Status)) return Status;

	Status = EFI_NOT_FOUND;
    for(i = 0; i < VarCount; i++) {
        if(!StrCmp(VarGUID[i].Var, VariableName) && MemCmp (&(VarGUID[i].Guid), VariableGuid, sizeof (EFI_GUID)) == 0) {

            gBS->SetMem(TempBuffer, 0x2000, 0xff);
            VarSize = VariableSize;

            Status = gRT->GetVariable(
                         VarGUID[i].Var,
                         &VarGUID[i].Guid,
                         &Attr,
                         &VarSize,
                         TempBuffer);

            if(EFI_ERROR (Status)) break;

            if((VarSize != VariableSize)) { Status = EFI_BAD_BUFFER_SIZE; break; }

            Status = gRT->SetVariable(
                         VarGUID[i].Var,
                         &VarGUID[i].Guid,
                         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         VarSize,
                         VariableBuffer);

            if(EFI_ERROR (Status)) {
                //
                // If SetVariable failed, change attribute to do again.
                //
                Status = gRT->SetVariable(
                             VarGUID[i].Var,
                             &VarGUID[i].Guid,
                             Attr,
                             VarSize,
                             VariableBuffer);
                if(EFI_ERROR (Status)) break;
            }
        }
    }
    gBS->FreePool(TempBuffer);
    DEBUG((-1, "[SMC_OOB] :: UpdateVariable end. Status = %r\n",Status));
    return Status;
}

EFI_STATUS
UpdateBiosCfgToSystem(
    IN	UINT8	*TotalDataBuffer
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CHAR16	VariableName[50];
    UINT8	VariableNameSize;
    UINT32	TotalSize, i;
    UINTN	VariableSize;
    EFI_GUID VariableGuid;
    UINT32 si = 0;

    DEBUG((-1, "[SMC_OOB] :: UpdateBiosCfgToSystem entry.\n"));

    TotalSize = ((InBand_BIOS_Cfg_Header*)TotalDataBuffer)->FileSize + sizeof(InBand_BIOS_Cfg_Header);
	
	DEBUG((-1,"[SMC_OOB] :: TotalSize = %x\n",TotalSize));

    for(i = sizeof(InBand_BIOS_Cfg_Header); i < TotalSize; ) {
        VariableNameSize = TotalDataBuffer[i];
        i++;			//point to variable name
        for (si = 0; si < VariableNameSize; si++) {
            VariableName[si] = TotalDataBuffer[i + si];
        }
        VariableName[si] = 0;

        DEBUG((-1, "[SMC_OOB] :: VariableName = %s\n", VariableName));
        DEBUG((-1, "[SMC_OOB] :: VariableNameSize = 0x%x\n", VariableNameSize));

        i += VariableNameSize;	//point to Variable size
        gBS->CopyMem(&VariableGuid, (UINT8*)&(TotalDataBuffer[i]), sizeof (EFI_GUID));
        i += sizeof (EFI_GUID);
        VariableSize = *(UINT16*)&TotalDataBuffer[i];
        i += 2;			//point to vabieable
        DEBUG((-1, "[SMC_OOB] :: VariableSize = 0x%x\n", VariableSize));
		Status = mSmcOobLibraryProtocol->Smc_UpdatePlatformVariable(VariableName, &VariableGuid, VariableSize, &TotalDataBuffer[i]);
        Status = UpdateVariable(VariableName, &VariableGuid, VariableSize, &TotalDataBuffer[i]);
        DEBUG((-1, "[SMC_OOB] :: UpdateVariable Return - %r\n", Status));

		if((Status == EFI_NOT_FOUND) && !!mSmcLsiRaidOOBSetupDriver){
			Status = mSmcLsiRaidOOBSetupDriver->SmcLsiSetupDriverCollectData(
												mSmcLsiRaidOOBSetupDriver,
												VariableName,
												&VariableGuid,
												VariableSize,
												&TotalDataBuffer[i]);
			DEBUG((-1,"mSmcLsiRaidOOBSetupDriver SmcLsiSetupDriverCollectData Status[%r]\n",Status));
		}

        i += (UINT16)VariableSize;
    }

	if(!!mSmcLsiRaidOOBSetupDriver){
		Status = mSmcLsiRaidOOBSetupDriver->SmcLsiSetupDriverHandleDataStart(mSmcLsiRaidOOBSetupDriver);
	}
    DEBUG((-1, "[SMC_OOB] :: UpdateBiosCfgToSystem end.\n"));
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  InBandUpdateBiosCfg
//
// Description:
//  Checking BIOS config area in BIOS ROM, if BIOS config area exist BIOS config date,
//  update the BIOS config date to system and erase BIOS config area.
//  Finally reboot or global reboot system.
//
// Steps :
//  1. Read the InBand data from flash part into TotalDataBuffer.
//  2. Checking BIOS config date area exit BIOS config date from user.
//  3. Update BIOS config to variables by paramater - "TotalDataBuffer".
//  4. Erase BIOS config area in BIOS ROM.
//  5. Check some variables changing. If it's true, global reset the system.
//
// Input:
//      None
//
// Output:
//      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
InBandUpdateBiosCfg(void)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 *TotalDataBuffer = NULL;
    UINTN Size;
    UINT8 Value;

    DEBUG((-1, "[SMC_OOB] :: InBandUpdateBiosCfg entry.\n"));

//#if POST_Progress_Message
//    {
//        EFI_STATUS Status=EFI_SUCCESS;
//        EFI_GUID mSetupGuid = SETUP_GUID;
//        SETUP_DATA SetupData;
//        UINTN VariableSize = sizeof(SETUP_DATA);
//        UINT8 DisplayPostMessages = 1;
//
//        Status = pRS->GetVariable(L"Setup", &mSetupGuid, NULL, &VariableSize, &SetupData);
//        if(!EFI_ERROR(Status)) {
//            if (!SetupData.SmcBootProcedureMsg_Support)
//                DisplayPostMessages = 0;
//        }
//        if (DisplayPostMessages)
//            _PrintPostBootMessage (L"  DXE--OOB Data Sync-Up..", TRUE, FALSE);
//    }
//#endif

    //
    // Delete variable - SmcInBandLoadDefault.
    //
    Size = sizeof (UINT8);
    Status = gRT->GetVariable (
                 L"SmcInBandLoadDefault",
                 &gEfiSmcInBandLoadDefaultGuid,
                 NULL,
                 &Size,
                 &Value);
	//Delete Variable
    if (!EFI_ERROR (Status)) {
        Status = gRT->SetVariable (
                     L"SmcInBandLoadDefault",
                     &gEfiSmcInBandLoadDefaultGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                     0,
                     &Value );
    }
    //
    // Read the InBand data from flash part into TotalDataBuffer.
    //
    Status = gBS->AllocatePool(EfiBootServicesData, mSmcRomHoleProtocol->SmcRomHoleSize(INBAND_REGION), &TotalDataBuffer);
    if(EFI_ERROR (Status)) {
        return Status;
    }
    Status = mSmcRomHoleProtocol->SmcRomHoleReadRegion(INBAND_REGION, TotalDataBuffer);
    if(EFI_ERROR (Status)) {
		FREE_BUFFER(TotalDataBuffer)
        return Status;
    }

    //
    // Checking BIOS config area, if no BIOS config date in BIOS config area, exit.
    //
    if(EFI_ERROR (Status) || MemCmp((UINT8*)TotalDataBuffer, "_BIN", 4) != 0) {	//inband region empty.
        DEBUG((-1, "[SMC_OOB] :: InBandUpdateBiosCfg buffer empty.\n"));
        FREE_BUFFER(TotalDataBuffer)
        return Status;
    }

    if(!(((InBand_BIOS_Cfg_Header*)TotalDataBuffer)->Flag & BIT0)) { 
        DEBUG((-1, "[SMC_OOB] :: InBand BIOS config don't need to update.\n"));
		FREE_BUFFER(TotalDataBuffer)
        return Status;
    }

    if(((InBand_BIOS_Cfg_Header*)TotalDataBuffer)->Flag & BIT1)	{ // load default
        Size = sizeof (UINT8);
        Value = 1;
        Status = gRT->SetVariable (
                     L"SmcInBandLoadDefault",
                     &gEfiSmcInBandLoadDefaultGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                     Size,
                     &Value );
		mSmcOobLibraryProtocol->Smc_OobLoadDefaultPreSetting();
    }
    else {
        UpdateBiosCfgToSystem (TotalDataBuffer);
    }

    gBS->SetMem(TotalDataBuffer, mSmcRomHoleProtocol->SmcRomHoleSize(INBAND_REGION), 0xff);
    mSmcRomHoleProtocol->SmcRomHoleWriteRegion(INBAND_REGION, TotalDataBuffer);

    FREE_BUFFER(TotalDataBuffer)
    DEBUG((-1, "[SMC_OOB] :: InBandUpdateBiosCfg end.\n"));

    //
    // Global reset the system.
    //
    IoWrite8(0xcf9, 0x0E);
    return Status;
}
