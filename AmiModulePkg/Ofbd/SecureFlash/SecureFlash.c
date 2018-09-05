//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	SecureFlash.c
//
// Description: This file provides OFBD Secure Flash function.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/SecSmiFlash.h>
#include <Protocol/SmiFlash.h>

#if PI_SPECIFICATION_VERSION >= 0x1000A
#include <Protocol\SmmCpu.h>
#include <Protocol\SmmBase2.h>
#include <Protocol\SmmSwDispatch2.h>
#else
#include <Protocol\SmmBase.h>
#include <Protocol\SmmSwDispatch.h>
#endif

#include <OFBD.h>
#include <SecureFlash.h>

// GUIDs of consumed protocols
static EFI_GUID		gEfiSecSmiFlashProtocolGuid		= EFI_SEC_SMI_FLASH_GUID;

// Consumed protocols
EFI_SEC_SMI_FLASH_PROTOCOL		*SecSmiFlash	= NULL;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LoadFirmwareImage
//
// Description:	Transfer ROM image from AFU's buffer to Secure Flash Module
//				through EFI_SEC_SMI_FLASH_PROTOCOL.
//
// Input:
//		IN VOID		*Data	The data is a memory buffer. Its content is FUNC_BLOCK
//							defined in SmiFlash.h. The member function
//							LOAD_FLASH_IMAGE in EFI_SEC_SMI_FLASH_PROTOCOL only
//							accepts this type of parameter.
//
// Output:
//		EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS LoadFirmwareImage (
	VOID		*Data
)
{
	EFI_STATUS		Status;

	if (NULL == SecSmiFlash) return EFI_UNSUPPORTED;

	Status = SecSmiFlash->LoadFwImage((FUNC_BLOCK *)Data);

	((FUNC_BLOCK *)Data)->ErrorCode = (EFI_ERROR(Status) ? 1 : 0);

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: GetFlashUpdatePolicy
//
// Description:	Get Flash Update Policy from Secure Flash module.
//
// Input:
//		IN VOID		*Data	The data is a memory buffer. Its content is
//							FLASH_POLICY_INFO_BLOCK defined in SecSmiFlash.h.
//
// Output:
//		EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetFlashUpdatePolicy (
	VOID		*Data
)
{
	EFI_STATUS					Status;
	FLASH_POLICY_INFO_BLOCK		*pFlashPolicy = (FLASH_POLICY_INFO_BLOCK *)Data;

	if (NULL == SecSmiFlash) return EFI_UNSUPPORTED;

	Status = SecSmiFlash->GetFlUpdPolicy(pFlashPolicy);

	pFlashPolicy->ErrorCode = (EFI_ERROR(Status) ? 1 : 0);

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SetFlashUpdateMethod
//
// Description:	Set Flash Update Policy to Secure Flash module.
//
// Input:
//		IN VOID		*Data	The data is a memory buffer. Its content is
//							FUNC_FLASH_SESSION_BLOCK defined in SecSmiFlash.h.
//
// Output:
//		EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SetFlashUpdateMethod (
	VOID		*Data
)
{
	EFI_STATUS					Status;
	FUNC_FLASH_SESSION_BLOCK	*pFlashSessionBlock = (FUNC_FLASH_SESSION_BLOCK *)Data;

	if (NULL == SecSmiFlash) return EFI_UNSUPPORTED;

	Status = SecSmiFlash->SetFlUpdMethod(pFlashSessionBlock);

	pFlashSessionBlock->ErrorCode = (EFI_ERROR(Status) ? 1 : 0);

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	SecureFlashProtocolCallback
//
// Description:	Locate SecSmiFlash protocol callback
//
// Input:
//		IN EFI_EVENT		Event
//		IN VOID				*Context
//
// Output:
//		VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if PI_SPECIFICATION_VERSION >= 0x1000A
EFI_STATUS SecureFlashProtocolCallback (
	CONST EFI_GUID		*Protocol,
	VOID				*Interface,
	EFI_HANDLE			Handle
)
{
	EFI_STATUS	Status;
	Status = pSmst->SmmLocateProtocol(&gEfiSecSmiFlashProtocolGuid, NULL, &SecSmiFlash);

	return Status;
}
#else
VOID SecureFlashProtocolCallback (
	EFI_EVENT			Event,
	VOID				*Context
)
{
	pBS->LocateProtocol(&gEfiSecSmiFlashProtocolGuid, NULL, &SecSmiFlash);
	pBS->CloseEvent(Event);
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:   ASFUInSmm
//
// Description: This function installs the ASFU Protocol.
//
// Input:
//		VOID
//
// Output:
//		VOID
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ASFUInSmm (
	VOID
)
{
	EFI_STATUS	Status;

#if PI_SPECIFICATION_VERSION >= 0x1000A
	Status = pSmst->SmmLocateProtocol(&gEfiSecSmiFlashProtocolGuid, NULL, &SecSmiFlash);
	if (EFI_ERROR(Status)) {
		VOID		*Registration;
		Status = pSmst->SmmRegisterProtocolNotify(
							&gEfiSecSmiFlashProtocolGuid,
							SecureFlashProtocolCallback,
							&Registration);
	}
#else
	Status = pBS->LocateProtocol(&gEfiSecSmiFlashProtocolGuid, NULL, &SecSmiFlash);
	if (EFI_ERROR(Status)) {
		EFI_EVENT	SecFlashCallbackEvt;
		VOID		*Reg;
		RegisterProtocolCallback(
			&gEfiSecSmiFlashProtocolGuid,
			SecureFlashProtocolCallback,
			NULL,
			&SecFlashCallbackEvt,
			&Reg);
	}
#endif
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	ASFUEntry
//
// Description:	OFBD Secure Flash Update Entry point
//
// Input:
//		IN VOID				*Buffer
//		IN OUT UINT8		*pOFBDDataHandled
//
// Output:
//		VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ASFUEntry (
	VOID		*Buffer,
	UINT8		*pOFBDDataHandled
)
{
	OFBD_HDR					*pOFBDHdr;
	OFBD_EXT_HDR				*pOFBDExtHdr;
	VOID						*pOFBDTblEnd;
	OFBD_TC_70_SFU_STRUCT		*ASFUStructPtr;
	EFI_STATUS					Status = EFI_SUCCESS;

	if (*pOFBDDataHandled == 0) {
		pOFBDHdr = (OFBD_HDR *)Buffer;
		pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
		ASFUStructPtr = (OFBD_TC_70_SFU_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR));
		pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));

		if (pOFBDHdr->OFBD_FS & OFBD_FS_SFU) {
			//Check Type Code ID
			if (pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_SFU) {
				switch (ASFUStructPtr->Command) {
					case OFBD_TC_SFU_LOAD_FIRMWARE_IMAGE :
						Status = LoadFirmwareImage(pOFBDTblEnd);
						break;
					case OFBD_TC_SFU_GET_FLASH_UPDATE_POLICY :
						Status = GetFlashUpdatePolicy(pOFBDTblEnd);
						break;
					case OFBD_TC_SFU_SET_FLASH_UPDATE_METHOD :
						Status = SetFlashUpdateMethod(pOFBDTblEnd);
						break;
					default :
						Status = EFI_UNSUPPORTED;
						break;
				}
				if (EFI_ERROR(Status)) {
					*pOFBDDataHandled = 0xFE;
					ASFUStructPtr->Status = OFBD_TC_SFU_NOT_SUPPORTED;
				} else {
					*pOFBDDataHandled = 0xFF;
					ASFUStructPtr->Status = OFBD_TC_SFU_OK;
				}
			}
		}
	}
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
