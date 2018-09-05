#include <PiDxe.h>
#include <Library/ReportStatusCodeLib.h>
#include <AmiStatusCodes.h>

EFI_STATUS AmiResetNotAvailableYet(
	IN EFI_RESET_TYPE ResetType, IN EFI_STATUS ResetStatus,
	IN UINTN DataSize, IN CHAR16 *ResetData OPTIONAL
)
{
    REPORT_STATUS_CODE (
        EFI_ERROR_CODE|EFI_ERROR_MINOR,
        DXE_RESET_NOT_AVAILABLE
    );
    
    return EFI_NOT_AVAILABLE_YET;;
}

EFI_STATUS
EFIAPI
ResetNotAvailableYetLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  SystemTable->RuntimeServices->ResetSystem = (EFI_RESET_SYSTEM)AmiResetNotAvailableYet;
  
  return EFI_SUCCESS;
}