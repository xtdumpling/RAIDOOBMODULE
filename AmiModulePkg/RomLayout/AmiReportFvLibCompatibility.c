#include <Ppi/AmiReadyToLoadDxeCore.h>
#include <Library/PeiServicesLib.h>

CONST EFI_PEI_PPI_DESCRIPTOR mReadyToLoadDxePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmiReadyToLoadDxeCorePpiGuid,
  NULL
};

/**
 * Function included for compatibility reasons only. The functionality of this library function
 * has already been moved into the RomLayoutPei Peim Entry Point.
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 *
 * @retval EFI_SUCCESS
 */
EFI_STATUS ReportFV2Pei(IN CONST EFI_PEI_SERVICES **PeiServices)
{
    return EFI_SUCCESS;
}


/**
 * Function included for compatibility reasons only. The functionality of this library function
 * has already been moved into the RomLayoutPei Peim Memory discovered Notification function.
 *
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 *
 * @retval EFI_SUCCESS
 */
EFI_STATUS ReportFV2PeiAfterMem(IN CONST EFI_PEI_SERVICES **PeiServices)
{
    return EFI_SUCCESS;
}

/**
 * Function included for compatibility between modules that rely on the DxeIpl from Foundation_04 and previous.
 * After that time, this functionality will be covered by the RomLayoutPei's ReportFv2DxeWrapper. At the point
 * that this module is called, it should translate the functionality of the older ReportFv module into
 * the newer functionality of installing the AmiReadyToLoadDxeCorePpi.
 *
 * @param RevoeryCapsule Optional pointer to the Recovery Image, if the the system is in recovery mode
 * @param PeiServices Pointer to the Pointer to the Pei Services Table
 *
 * @retval EFI_SUCCESS
 */

EFI_STATUS ReportFV2Dxe(IN VOID* RecoveryCapsule OPTIONAL, IN CONST EFI_PEI_SERVICES **PeiServices)
{
    return PeiServicesInstallPpi(&mReadyToLoadDxePpiList);
}
