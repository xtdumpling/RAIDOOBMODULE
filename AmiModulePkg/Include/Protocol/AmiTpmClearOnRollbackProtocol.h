//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TpmClearOnRollBack/AmiTpmClearOnRollbackProtocol.h 1     5/23/13 7:50p Fredericko $
//
// $Revision: 1 $
//
// $Date: 5/23/13 7:50p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TCG/TpmClearOnRollBack/AmiTpmClearOnRollbackProtocol.h $
//
// 1     5/23/13 7:50p Fredericko
// [TAG]        EIP121823
// [Category]   Improvement
// [Description]    Clear TPM on BIOS rollback
// [Files]          AmiTpmClearOnRollbackProtocol.cif
// AmiTpmClearOnRollbackProtocol.h
//
//
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:
//
// Description:
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef _AMI_TPM_CLEAR_ON_ROLLBACK_H
#define _AMI_TPM_CLEAR_ON_ROLLBACK_H



#define TPM_CLEAR_SUCCESSFUL 0x0
#define TPM_CLEAR_RESET_REQUIRED 0x01
#define TPM_FATAL_CONTINUE_FLASH 0x02
#define TPM_FATAL_DISCONTINUE_FLASH 0x03

typedef
UINT8
(EFIAPI * CLEAR_TPM_ON_ROLLBACK)(VOID);

typedef struct _CLEAR_TPM_ROLLBACK_PROTOCOL
{
    CLEAR_TPM_ON_ROLLBACK            ClearTpmOnRollBack;
} CLEAR_TPM_ROLLBACK_PROTOCOL;

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
