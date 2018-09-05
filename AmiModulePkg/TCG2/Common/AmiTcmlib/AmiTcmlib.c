//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/AmiTcmlib/AmiTcmlib.c 6     4/01/11 9:32a Fredericko $
//
// $Revision: 6 $
//
// $Date: 4/01/11 9:32a $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiTcmlib.c
//
// Description:
//  AmiTcmlibrary functions
//
//<AMI_FHDR_END>
//**********************************************************************
#include "AmiTcmlib.h"
#include "Token.h"


TCM_ID_STRUC  TCMSupportedArray[NUMBER_OF_SUPPORTED_TCM_DEVICES]=
{
    {SUPPORTED_TCM_DEVICE_1_VID,SUPPORTED_TCM_DEVICE_1_DID},    //ZTEIC
    {SUPPORTED_TCM_DEVICE_2_VID,SUPPORTED_TCM_DEVICE_2_DID},    //ZTEIC2
    {SUPPORTED_TCM_DEVICE_3_VID,SUPPORTED_TCM_DEVICE_3_DID}     //ZTEIC3
};




//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   AutoSupportType
//
// Description: verifies support for a TCM module on a platform
//
// Input:       NONE
//
// Output:      BOOLEAN
//
// Modified:
//
// Referrals:
//
// Notes:
//<AMI_PHDR_END>
//**********************************************************************
extern
BOOLEAN
EFIAPI IsTcmSupportType ()
{
#if TCG_LEGACY == 0
    UINTN i=0;

    for(i=0; i<(sizeof(TCMSupportedArray)/sizeof(TCM_ID_STRUC)); i++)
    {
        if((TCMSupportedArray[i].VID == *(UINT16 *)(UINTN)(PORT_TPM_IOMEMBASE + 0xF00)) &&
                (TCMSupportedArray[i].DID == *(UINT16 *)(UINTN)(PORT_TPM_IOMEMBASE + 0xF02)))
        {
            return TRUE;
        }
    }
#endif
    return FALSE;
}




//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
