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

//----------------------------------------------------------------------
#include <PiPei.h>
#include <Library/AmiPlatformHooksLib.h>
#include <Library/CpuConfigLib.h>
//----------------------------------------------------------------------

EFI_STATUS
OemUpdatePlatformInfoHob (
   IN CONST EFI_PEI_SERVICES     **PeiServices,
   OUT VOID         *PlatformInfoHob
) {
  // Add OEM Specific Porting here

  return EFI_SUCCESS;
}

/**

  OemPchPolicyOverride - OEM hook NULL wrapper to override the PchPolicy

  @param PchPolicy - pointer to PchPolicy
  @param SetupVariables - pointer to SetupVariables
  @param PchRcVariables - pointer to PchRcVariables
  @retval VOID

**/
VOID
OemPchPolicyOverride(
   IN OUT VOID        *PchPolicy,
   IN VOID      *SetupVariables,
   IN VOID      *PchRcVariables
) {
    return;
}

/**

  OemPchSoftStrapFixupOverride - OEM hook NULL wrapper to override the PchSoftStrap

  @param StrapRegion - pointer to StrapRegion
  @param SystemBoard - pointer to SystemBoardPpi
  @retval VOID

**/
VOID
OemPchSoftStrapFixupOverride(
   IN OUT UINT8                 *StrapRegion,
   IN VOID          *SystemBoard
) {
    return;
}

/**

  OemInstallXhciAcpiTableOverride - OEM hook NULL wrapper to override the XhciAcpiTable

  @param PlatformInfo - pointer to PlatformInfo
  @param XhciAcpiTable - pointer to XhciAcpiTable
  @retval VOID

**/
VOID
OemInstallXhciAcpiTableOverride(
   IN VOID         *PlatformInfo,
   IN OUT UINT64                *XhciAcpiTable
) {
    return;
}

/**

  OemOverrideCpuConfigContextBuffer - OEM hook NULL wrapper to override the CpuConfigContextBuffer

  @param CpuConfigContextBuffer - pointer to CpuConfigContextBuffer
  @retval VOID

**/
VOID
OemOverrideCpuConfigContextBuffer (
   IN OUT CPU_CONFIG_CONTEXT_BUFFER *CpuConfigContextBuffer
) {
    return;
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
