//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  NVRAM Services in PEI
**/

#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <AmiPeiLib.h>
#include <Token.h>
#include "NVRAM.h"
#include <Library/AmiRomLayoutLib.h>
#include <Setup.h>
#include <AmiNvramPeiPlatformHooks.h>

BOOLEAN PeiNvramDataCompatiblityChecker(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
);

//Defined in Tokens.c
extern UINTN NvramAddress;
extern UINTN NvramBackupAddress;
extern const UINTN NvramSize;
extern const BOOLEAN NvSimulation;

typedef struct{
    EFI_PEI_READ_ONLY_VARIABLE2_PPI Ppi;
    UINT32 InfoCount;
    UINT32 LastInfoIndex;
    NVRAM_STORE_INFO NvramInfo[3];
} VARIABLE_PPI;

//--- GetVariable and GetNextVarName Hooks ------
EFI_STATUS PeiGetVariableHook(
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
    IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes, IN OUT UINTN *DataSize, OUT VOID *Data
){
    UINTN i;
    EFI_STATUS Result = EFI_UNSUPPORTED;
    for(i=0; PeiGetVariableHookList[i] && (Result == EFI_UNSUPPORTED); i++) 
        Result = PeiGetVariableHookList[i](This,VariableName, VendorGuid,Attributes, DataSize, Data);
    return Result;
}

EFI_STATUS PeiGetNextVarNameHook(
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
){
    UINTN i;
    EFI_STATUS Result = EFI_UNSUPPORTED;
    for(i=0; PeiGetNextVarNameHookList[i] && (Result == EFI_UNSUPPORTED); i++) 
        Result = PeiGetNextVarNameHookList [i](This,VariableNameSize,VariableName, VendorGuid);
    return Result;
}
//---GetVariable and GetNextVarName Hooks END------

/**
  This function determines if the current NVRAM data 
  (mainly Setup-related variables) is compatible with the current firmware.

  @param PeiServices pointer to a pointer to the PEI Services Table. 
  @param ReadVariablePpi - pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. 
                           The pointer can be used to read and enumerate existing NVRAM variables.

  @retval:
    BOOLEAN
      TRUE - the NVRAM data is compatible
      FALSE - the NVRAM data is not compatible
**/
BOOLEAN PeiNvramDataCompatiblityChecker(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
	static EFI_GUID SetupVariableGuid = SETUP_GUID;	
    UINTN Size = 0;
    EFI_STATUS Status;

    Status=ReadVariablePpi->GetVariable(
        ReadVariablePpi, L"Setup",
        &SetupVariableGuid, NULL,
        &Size, NULL
    );
    if (Status!=EFI_BUFFER_TOO_SMALL) return TRUE;
    return Size==sizeof(SETUP_DATA);
}

/**
    This function calls a list of function defined at build time and 
    return whether the functions completed successfully or not

        
    @param PeiServices pointer to the PeiServices Table
    @param ReadVariablePpi pointer to the Read Only Variable 2 PPI

    @retval 
        BOOLEAN 
        - TRUE - all functions returned correctly
        - FALSE - one of the functions failed during execution

    @note  
  Similar to CallOneParamHooks except that these function also require
  a pointer to the Read Only Variable 2 PPI
          
**/
BOOLEAN CallNvramModeDetectionHooks(
    IN NVRAM_MODE_DETECTION_FUNCTION* FunctionList[],
    IN EFI_PEI_SERVICES **PeiServices, 
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
    UINTN i;
    BOOLEAN Result = FALSE;
    for(i=0; FunctionList[i] && !Result; i++) 
        Result = FunctionList[i](PeiServices,ReadVariablePpi);
    return Result;
}

/**
    This function determines if the system in in manufacturing mode.
    This function is called by NVRAM code. 
    If system is in manufacturing mode, manufacturing values for NVRAM variables 
    are used.

    @param PeiServices - pointer to a pointer to the PEI Services Table. 
    @param ReadVariablePpi - pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. The pointer 
           can be used to read and enumerate existing NVRAM variables.

             
    @retval TRUE Manufacturing mode is detected
    @retval FALSE Manufacturing mode is not detected

    @note  
  This routine is called very early, prior to SBPEI and NBPEI
**/
BOOLEAN IsMfgMode(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
#if MANUFACTURING_MODE_SUPPORT
    //Use IsMfgMode eLink to install customer handlers
    return CallNvramModeDetectionHooks(IsMfgModeList, PeiServices, ReadVariablePpi);
#else
    //Do not change this.
    //This is needed to be able to disable manufacturing mode support using SDL token.
    return FALSE;
#endif
}

/**
    This function determines if the system configuration needs to be reset.
    This function is called by NVRAM code. 

    @param PeiServices - pointer to a pointer to the PEI Services Table. 
    @param ReadVariablePpi - pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. The 
           pointer can be used to read and enumerate existing NVRAM 
           variables.

             
    @retval TRUE Reset Configuration condition is detected
    @retval FALSE Reset Configuration condition is not detected

    @note  This routine is called very early, prior to SBPEI and NBPEI
**/
BOOLEAN IsResetConfigMode(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
    //Use IsResetConfigMode eLink to install customer handlers
    return CallNvramModeDetectionHooks(
        IsResetConfigModeList, PeiServices, ReadVariablePpi
    );
}

/**
    This function determines if the system should boot with the default configuration.
    This function is called by NVRAM code. 
    If boot with default configuration is detected, default values for NVRAM 
    variables are used.

    @param PeiServices - pointer to a pointer to the PEI Services Table. 
    @param ReadVariablePpi - pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. The pointer 
           can be used to read and enumerate existing NVRAM variables.

             
    @retval TRUE Boot with default configuration is detected
    @retval FALSE Boot with default configuration is not detected

    @note  
  This routine is called very early, prior to SBPEI and NBPEI

**/
BOOLEAN IsDefaultConfigMode(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
    // Use IsDefaultConfigMode eLink to install custom handlers
    return CallNvramModeDetectionHooks(
        IsDefaultConfigModeList, PeiServices, ReadVariablePpi
    );
}

/**
    This function searches for a Variable with specific GUID and Name

    @param PeiServices Double pointer to PEI Services instance
    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var will be returned

    @retval EFI_STATUS based on result

**/
EFI_STATUS VarPpiGetVariable(
	IN EFI_PEI_SERVICES **PeiServices,
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data
)
{
    PEI_TRACE((-1, PeiServices,"Deprecated ReadOnlyVariable PPI is used. Use ReadOnlyVariable2 PPI instead\n"));
	return PeiGetVariable(
        PeiServices, VariableName, VendorGuid, Attributes, DataSize, Data
    );
}

/**
    Return the next variable name and GUID.
    
    This function is called multiple times to retrieve the VariableName 
    and VariableGuid of all variables currently available in the system.
	On each call, the previous results are passed into the interface, 
	and, on return, the interface returns the data for the next interface. 
	When the entire variable list has been returned, EFI_NOT_FOUND is returned.    
  
    @param PeiServices double pointer to PEI Services instance
    @param  VariableNameSize  On entry, points to the size of the buffer pointed to by VariableName.
                              On return, the size of the variable name buffer.
    @param  VariableName      On entry, a pointer to a null-terminated string that is the variable's name.
                              On return, points to the next variable's null-terminated name string.

    @param  VariableGuid      On entry, a pointer to an EFI_GUID that is the variable's GUID. 
                              On return, a pointer to the next variable's GUID.

    @retval EFI_SUCCESS           The variable was read successfully.
    @retval EFI_NOT_FOUND         The variable could not be found.
    @retval EFI_BUFFER_TOO_SMALL  The VariableNameSize is too small for the resulting
                                  data. VariableNameSize is updated with the size
                                  required for the specified variable.
    @retval EFI_INVALID_PARAMETER VariableName, VariableGuid or
                                  VariableNameSize is NULL.
    @retval EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.
**/
EFI_STATUS VarPpiGetNextVariableName(
	IN EFI_PEI_SERVICES **PeiServices,
	IN OUT UINTN *VariableNameSize,
	IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
)
{
    PEI_TRACE((-1, PeiServices,"Deprecated ReadOnlyVariable PPI is used. Use ReadOnlyVariable2 PPI instead\n"));
	return PeiGetNextVariableName(
        PeiServices, VariableNameSize, VariableName, VendorGuid
    );
}

/**
    This function searches for Variable with specific name and GUID

    @param This pointer to FI_PEI_READ_ONLY_VARIABLE2_PPI
    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Pointer to memory where Attributes will be returned 
    @param DataSize size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
        will be returned and DataSize will be set to actual size needed
    @param Data Pointer to memory where Var will be returned

    @retval EFI_STATUS based on result

**/
EFI_STATUS VarPpi2GetVariable(
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
    IN CONST CHAR16 *VariableName, IN CONST EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes, IN OUT UINTN *DataSize, OUT VOID *Data
)
{
	EFI_STATUS Status;
    VARIABLE_PPI *VarPpi = (VARIABLE_PPI*)This;
    Status = PeiGetVariableHook (
                This, VariableName, VendorGuid, Attributes, DataSize, Data
             );
    if (Status != EFI_UNSUPPORTED) return Status;
	Status = NvGetVariable2(
				(CHAR16*)VariableName, (EFI_GUID*)VendorGuid, Attributes,
				DataSize, Data, VarPpi->InfoCount, VarPpi->NvramInfo
			 );
	return Status;
}

/**
    Return the next variable name and GUID.
    
    This function is called multiple times to retrieve the VariableName 
    and VariableGuid of all variables currently available in the system.
	On each call, the previous results are passed into the interface, 
	and, on return, the interface returns the data for the next interface. 
	When the entire variable list has been returned, EFI_NOT_FOUND is returned.    

    @param  This              A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.

    @param  VariableNameSize  On entry, points to the size of the buffer pointed to by VariableName.
                              On return, the size of the variable name buffer.
    @param  VariableName      On entry, a pointer to a null-terminated string that is the variable's name.
                              On return, points to the next variable's null-terminated name string.

    @param  VariableGuid      On entry, a pointer to an EFI_GUID that is the variable's GUID. 
                              On return, a pointer to the next variable's GUID.

    @retval EFI_SUCCESS           The variable was read successfully.
    @retval EFI_NOT_FOUND         The variable could not be found.
    @retval EFI_BUFFER_TOO_SMALL  The VariableNameSize is too small for the resulting
                                  data. VariableNameSize is updated with the size
                                  required for the specified variable.
    @retval EFI_INVALID_PARAMETER VariableName, VariableGuid or
                                  VariableNameSize is NULL.
    @retval EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.
**/
EFI_STATUS VarPpi2GetNextVariableName(
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
)
{
	EFI_STATUS Status;
    VARIABLE_PPI *VarPpi = (VARIABLE_PPI*)This;
    Status = PeiGetNextVarNameHook (
                This, VariableNameSize, VariableName, VendorGuid
             );
    if (Status != EFI_UNSUPPORTED) return Status;
    Status = NvGetNextVariableName2(
                 VariableNameSize, VariableName, VendorGuid,
                 VarPpi->InfoCount, VarPpi->NvramInfo,
                 &(VarPpi->LastInfoIndex),FALSE
		   );
	return Status;
}

// PPI interface definition
EFI_PEI_READ_ONLY_VARIABLE_PPI VariablePpi = {VarPpiGetVariable, VarPpiGetNextVariableName};
EFI_PEI_READ_ONLY_VARIABLE2_PPI Variable2Ppi = {VarPpi2GetVariable, VarPpi2GetNextVariableName};

// PPI to be installed
EFI_PEI_PPI_DESCRIPTOR VariablePpiListTemplate[] =
{ 
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI,
	    &gEfiPeiReadOnlyVariable2PpiGuid, &Variable2Ppi
    },
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
	    &gEfiPeiReadOnlyVariablePpiGuid, &VariablePpi
    }
};

EFI_STATUS GetNvramLocation (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8 **MyNvramAddress,
  OUT UINT8 **MyNvramBackupAddress,
  OUT UINTN *MyNvramSize
)
{
    AMI_ROM_AREA *RomArea = NULL;
    
    RomArea = AmiGetFirstRomAreaByGuid (&gAmiNvramMainRomAreaGuid);
    if (RomArea != NULL)
    {
        *MyNvramAddress = (UINT8*)(UINTN)RomArea->Address;
        *MyNvramSize = RomArea->Size;
    }
    else
    { 
        PEI_TRACE((-1, PeiServices, "NVRAM PEI: NVRAM ROM area was not found!!! Using default address.\n"));
        *MyNvramAddress = (UINT8*)NvramAddress;
        *MyNvramSize = NvramSize;
    }
    RomArea = NULL;
    RomArea = AmiGetFirstRomAreaByGuid (&gAmiNvramBackupRomAreaGuid);
    if (RomArea != NULL)
    {
        *MyNvramBackupAddress = (UINT8*)(UINTN)RomArea->Address;
    }
    else 
    {
        PEI_TRACE((-1, PeiServices, "NVRAM PEI: NVRAM BACKUP ROM area was not found!!! Using default address.\n"));
        *MyNvramBackupAddress = (UINT8*)NvramBackupAddress;
    }
    return EFI_SUCCESS;
}

#define FV_BB_DEFAULTS_FFS_FILE_GUID \
    { 0xaf516361, 0xb4c5, 0x436e, { 0xa7, 0xe3, 0xa1, 0x49, 0xa3, 0x1b, 0x14, 0x61 } }

EFI_STATUS GetDefaultsInfoFromFvBb (
    IN CONST EFI_PEI_SERVICES  **PeiServices, IN NVRAM_STORE_INFO *DefInfo
){
    UINTN FvInstance = 0;
    EFI_PEI_FV_HANDLE FvHandle;
    EFI_PEI_FILE_HANDLE  FileHandle;
    static EFI_GUID gFvBbDefaultsFfsFileGuid = FV_BB_DEFAULTS_FFS_FILE_GUID;
    EFI_STATUS Status;

    while(TRUE) {
        VOID *Buffer;
        UINTN Size;
        NVRAM_STORE_INFO WrapperInfo;
        
        FvHandle = NULL;
        Status = (*PeiServices)->FfsFindNextVolume (PeiServices, FvInstance++, &FvHandle);
        if (EFI_ERROR (Status)) return Status;
        FileHandle = NULL;
        // Find file
        Status = (*PeiServices)->FfsFindFileByName (&gFvBbDefaultsFfsFileGuid, FvHandle, &FileHandle);
        if (EFI_ERROR (Status)) continue;
        // Read section data
        Status = (*PeiServices)->FfsFindSectionData(PeiServices, EFI_SECTION_RAW, FileHandle, &Buffer);
        if (EFI_ERROR (Status)) continue;
        Size = SECTION_SIZE((EFI_COMMON_SECTION_HEADER*)Buffer - 1) - sizeof(EFI_COMMON_SECTION_HEADER);
        WrapperInfo.NvramAddress = Buffer;
        WrapperInfo.NvramSize = Size;
        NvInitInfoBuffer(
            &WrapperInfo, 0,
              NVRAM_STORE_FLAG_NON_VALATILE
            | NVRAM_STORE_FLAG_READ_ONLY
            | NVRAM_STORE_FLAG_DO_NOT_ENUMERATE,
            NULL
        );
        if ( NvGetDefaultsInfo(StdDefaults, &WrapperInfo, DefInfo) == NULL ) continue;
        return EFI_SUCCESS;
    }
}

NVRAM_STORE_INFO* PeiGetStdDefaultsInfo(IN NVRAM_STORE_INFO *InInfo, OUT NVRAM_STORE_INFO *OutInfo){
    CONST EFI_PEI_SERVICES **PeiServices;
    
    if (NvGetDefaultsInfo( StdDefaults, InInfo, OutInfo)!= NULL) return OutInfo;
    PeiServices = GetPeiServicesTablePointer();
    PEI_TRACE((-1, PeiServices, "NVRAM PEI: Built-in NVRAM Defaults are not found\n"));
    if (!EFI_ERROR(GetDefaultsInfoFromFvBb(PeiServices, OutInfo))) {
        PEI_TRACE((-1, PeiServices, "NVRAM PEI: Using FV_BB NVRAM Defaults\n"));
        return OutInfo;
    }
    return NULL;
}
   
/**
    NVRAM PEIM Entry Point

    @param FfsHeader pointer to FfsHeader
    @param PeiServices double pointer to the PEI Services structure

    @retval EFI_STATUS based on result

**/
EFI_STATUS EFIAPI PeiInitNvram (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS Status;
    EFI_PEI_PPI_DESCRIPTOR *VariablePpiList;
    VARIABLE_PPI *VarPpi;
    UINT32 NvramMode=0;
    BOOLEAN ResetConfigMode;
    NVRAM_STORE_INFO MainNvram;
    VARIABLE_PPI TmpVarPpi;
    UINT8 *BackupNvramAddress;
    BOOLEAN NvramIsCorrupted = FALSE;
    UINT32 HeaderLength;
    NVRAM_HOB *pHob;
    BOOLEAN BackupStoreValid;

    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(VariablePpiListTemplate)+sizeof(VARIABLE_PPI), &VariablePpiList);
    if (EFI_ERROR(Status)) return Status;
    VariablePpiList[0]=VariablePpiListTemplate[0];
    VariablePpiList[1]=VariablePpiListTemplate[1];
    VarPpi = (VARIABLE_PPI*)(VariablePpiList + 2);
    VariablePpiList[0].Ppi = VarPpi;
    VarPpi->Ppi = Variable2Ppi;
    VarPpi->InfoCount = 0;
    VarPpi->LastInfoIndex = 0;
#if NV_SIMULATION_NO_FLASH_READS
     NvramMode=NVRAM_MODE_RESET_CONFIGURATION;
     PEI_TRACE((-1, PeiServices, "NVRAM PEI: Working in simulation mode.\n"));
     return (*PeiServices)->InstallPpi(PeiServices,VariablePpiList);
#endif
    Status = GetNvramLocation(PeiServices,&MainNvram.NvramAddress,&BackupNvramAddress,&MainNvram.NvramSize);
    if (EFI_ERROR(Status)) return Status;
    HeaderLength = GetNvStoreHeaderSize((VOID*)MainNvram.NvramAddress);
    PEI_TRACE((-1, PeiServices, 
        "NVRAM PEI: NVRAM Address: %p; NVRAM Backup Address: %p; NVRAM Size: %X; Header Size: %X\n",
        MainNvram.NvramAddress, BackupNvramAddress, MainNvram.NvramSize, HeaderLength
    ));
    if (!IsMainNvramStoreValid(MainNvram.NvramAddress, BackupNvramAddress,&BackupStoreValid)){
        if (BackupStoreValid){
            UINT8 *Tmp = BackupNvramAddress;
            BackupNvramAddress = MainNvram.NvramAddress;
            MainNvram.NvramAddress = Tmp;
        }else{
            NvramIsCorrupted = TRUE;
        }
    }
    NvInitInfoBuffer(
        &MainNvram,
        HeaderLength,
        NVRAM_STORE_FLAG_NON_VALATILE,
        NULL
    );
    if (NvramIsCorrupted){
        NvramMode=NVRAM_MODE_RESET_CONFIGURATION;
        PEI_TRACE((-1, PeiServices, "NVRAM PEI: NVRAM header corruption is detected\n"));
        if (!EFI_ERROR(GetDefaultsInfoFromFvBb(PeiServices, &VarPpi->NvramInfo[VarPpi->InfoCount]))){
            VarPpi->InfoCount++;
            PEI_TRACE((-1, PeiServices, "NVRAM PEI: Using FV_BB NVRAM Defaults\n"));
        }
    }else{
        TmpVarPpi = *VarPpi;
        TmpVarPpi.NvramInfo[0]=MainNvram;
        TmpVarPpi.InfoCount=1;
    
        if (   IsMfgMode(PeiServices,&TmpVarPpi.Ppi)
            && NvGetDefaultsInfo(
                   MfgDefaults,&MainNvram,&VarPpi->NvramInfo[VarPpi->InfoCount]
               ) != NULL
        ){
            VarPpi->InfoCount++;
            NvramMode|=NVRAM_MODE_MANUFACTORING;
        }
        ResetConfigMode = IsResetConfigMode(PeiServices,&TmpVarPpi.Ppi);
        if (!ResetConfigMode && !IsDefaultConfigMode(PeiServices,&TmpVarPpi.Ppi)){
            VarPpi->NvramInfo[VarPpi->InfoCount]=MainNvram;
            VarPpi->InfoCount++;
            if (PeiGetStdDefaultsInfo(
                    &MainNvram,&VarPpi->NvramInfo[VarPpi->InfoCount]
                ) != NULL
            ){
                VarPpi->InfoCount++;
            }
        }else{
            if (PeiGetStdDefaultsInfo(
                    &MainNvram,&VarPpi->NvramInfo[VarPpi->InfoCount]
                ) != NULL
            ){
                VarPpi->InfoCount++;
            }
            if (ResetConfigMode){
                NvramMode|=NVRAM_MODE_RESET_CONFIGURATION;
            }else{
                VarPpi->NvramInfo[VarPpi->InfoCount]=MainNvram;
                VarPpi->InfoCount++;
                NvramMode|=NVRAM_MODE_DEFAULT_CONFIGURATION;
            }
        }
        if (!CompatibilityChecker(PeiServices, &VarPpi->Ppi)){
            PEI_TRACE((-1, PeiServices, "NVRAM PEI: Incompatible NVRAM detected\n"));
            VarPpi->InfoCount=0;
            if (!EFI_ERROR(GetDefaultsInfoFromFvBb(PeiServices, &VarPpi->NvramInfo[VarPpi->InfoCount]))){
                VarPpi->InfoCount++;
                PEI_TRACE((-1, PeiServices, "NVRAM PEI: Incompatible NVRAM Using FV_BB NVRAM Defaults\n"));
            }
        }
    }
    Status=(*PeiServices)->CreateHob(PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof(NVRAM_HOB),&pHob);
    ASSERT_PEI_ERROR(PeiServices,Status)
	if (!EFI_ERROR(Status)){
	    pHob->Header.Name=AmiNvramHobGuid;
	    pHob->NvramAddress=(EFI_PHYSICAL_ADDRESS)(UINTN)MainNvram.NvramAddress;
	    pHob->BackupAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)BackupNvramAddress;
	    pHob->NvramSize = MainNvram.NvramSize;
	    if (NvSimulation) NvramMode|=NVRAM_MODE_SIMULATION;
	    pHob->NvramMode = NvramMode;
	    pHob->HeaderLength = HeaderLength;
	}
	return (*PeiServices)->InstallPpi(PeiServices,VariablePpiList);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
