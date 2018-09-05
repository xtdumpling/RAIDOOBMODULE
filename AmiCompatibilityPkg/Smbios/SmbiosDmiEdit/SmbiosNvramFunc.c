//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//**********************************************************************//
// $Header: /Alaska/SOURCE/Modules/SMBIOS/SmbiosDMIEditSupport/SmbiosNvramFunc.c 9     9/04/12 11:05a Davidd $
//
// $Revision: 9 $
//
// $Date: 9/04/12 11:05a $
//**********************************************************************//
//**********************************************************************//

#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/AmiSmbios.h>
#include "SmbiosDmiEdit.h"

#if SMBIOS_DMIEDIT_DATA_LOC == 2
static BOOLEAN      DmiEditVarPresent =  FALSE;
CHAR16              *DmiArrayVar = L"DmiArray";
DMI_VAR             DmiArray[DMI_ARRAY_COUNT] = {0};
UINTN               DmiArraySize = DMI_ARRAY_COUNT * sizeof(DMI_VAR);
CHAR16              *S1 = L"                ";
UINT8               DmiData[0x1000];
UINT8               *DmiDataPtr;
UINT8               Buffer[0x400];
UINTN               BufferSize;
UINTN               DmiDataLength[DMI_ARRAY_COUNT];
UINT8               gRecoveryKeepDMIFlag = FALSE;

/**
    Preserve the DMIEdit data by loading its data into memory
    prior to flashing

    @param None

    @retval None

**/
VOID
PreserveDmiEditData (VOID)
{
    EFI_STATUS  Status;
    UINT8       Count;
    UINT8       Index;

	//
	// Get number of DMI data records in NVRAM
	//
	// Note: DMI data record actually starts with record #1,
	//		 first record #0 holds total number of DMI data records
	//       instead of TABLE_INFO
	//       ===> DmiArray[0].Type = count
	//
    Status = pRS->GetVariable(
                        DmiArrayVar,
                        &gAmiSmbiosNvramGuid,
                        NULL,
                        &DmiArraySize,
                        &DmiArray[0]);

    if (Status == EFI_SUCCESS) {
        DmiDataPtr = DmiData;

        Count = DmiArray[0].Type;     // Note: DmiArray[0] has count #

		// Get DMI data into memory. The data will be saved back into
		// NVRAM later in RestoreDmiEditData
        for (Index = 0; Index < Count; Index++) {
            Swprintf(S1, L"DmiVar%02x%04x%02x%02x",
					   DmiArray[Index + 1].Type,
					   DmiArray[Index + 1].Handle,
					   DmiArray[Index + 1].Offset,
					   DmiArray[Index + 1].Flags);

            BufferSize = sizeof(Buffer);
            Status = pRS->GetVariable(
                                S1,
                                &gAmiSmbiosNvramGuid,
                                NULL,
                                &BufferSize,
                                &Buffer);

			if (Status == EFI_SUCCESS) {
                MemCpy(DmiDataPtr, Buffer, BufferSize);
				DmiDataLength[Index] = BufferSize;
                DmiDataPtr += BufferSize;
			}
			else {
				DmiDataLength[Index] = 0;
			}
        }

        DmiEditVarPresent = TRUE;
    }
}

/**
    Restore the DMIEdit data in NVRAM with data previously loaded
    in memory by PreserveDmiEditData.

    @param None

    @retval None

**/
VOID
RestoreDmiEditData (VOID)
{
    UINT8       Count;
    UINT8       Index;

	// DMI data were read and saved in memory in PreserveDmiEditData.
	// Now save DMI data back into NVRAM if present
    if (DmiEditVarPresent) {
        pRS->SetVariable(
                    DmiArrayVar,
                    &gAmiSmbiosNvramGuid,
                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    DmiArraySize,
                    &DmiArray[0]);

        DmiDataPtr = DmiData;

        Count = DmiArray[0].Type;     // Note: DmiArray[0] has count # instead of Type/Offset

        for (Index = 0; Index < Count; Index++) {
            Swprintf(S1, L"DmiVar%02x%04x%02x%02x",
					   DmiArray[Index + 1].Type,
					   DmiArray[Index + 1].Handle,
					   DmiArray[Index + 1].Offset,
					   DmiArray[Index + 1].Flags);

            pRS->SetVariable(
                        S1,
                        &gAmiSmbiosNvramGuid,
                        EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
						DmiDataLength[Index],
                        DmiDataPtr);

            DmiDataPtr += DmiDataLength[Index];
        }
    }
}

/**
    Preserve the DMIEdit data by loading its data into memory
    prior to flashing (for capsule mode)

    @param None

    @retval None

**/
VOID
RecoveryPreserveDmiEditData (VOID)
{
    EFI_STATUS      Status;
    EFI_GUID        gEfiSmbiosNvramGuid = {0x4b3082a3, 0x80c6, 0x4d7e, { 0x9c, 0xd0, 0x58, 0x39, 0x17, 0x26, 0x5d, 0xf1 }};
    CHAR16          *PreserveSmbiosNvramVar = L"PreserveSmbiosNvramVar";
    UINTN           Size = sizeof (UINT8);
    UINT32          PreserveSmbiosNvram;

    gRecoveryKeepDMIFlag = FALSE;

    Status = pRS->GetVariable (
            PreserveSmbiosNvramVar,
            &gEfiSmbiosNvramGuid,
            NULL,
            &Size,
            &PreserveSmbiosNvram
            );

    if (!EFI_ERROR (Status)) {
        gRecoveryKeepDMIFlag = TRUE;
        PreserveDmiEditData ();
    }

    pRS->SetVariable (
            PreserveSmbiosNvramVar,
            &gEfiSmbiosNvramGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
            0,
            NULL
            );
}

/**
    Restore the DMIEdit data in NVRAM with data previously loaded
    in memory by PreserveDmiEditData (for capsule mode)

    @param None

    @retval None

**/
VOID
RecoveryRestoreDmiEditData (VOID)
{
    if (gRecoveryKeepDMIFlag) {
        RestoreDmiEditData ();
    }

    gRecoveryKeepDMIFlag = FALSE;
}
#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
