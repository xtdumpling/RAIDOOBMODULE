//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
StdEc.h

Header file for the StdEC

*/

#ifndef _EFI_STDEC_H_
#define _EFI_STDEC_H_
#ifdef __cplusplus
extern "C" {
#endif

#define RETURN_ERR 1
//===============================
// Standard EC Flash Port define	
//===============================
#define EC_KBC_DATA_PORT		0x60
#define EC_KBC_CMD_PORT			0x64

//===============================
// Standard EC status bits definition
//===============================
#define EC_KBC_OBF			0x01
#define EC_KBC_IBF			0x02

//===========================
// Standard EC Flash Command 
//===========================
#define EC_CMD_ERASE_MEMORY     0x01
#define EC_CMD_INITIAL_PM       0x02
#define EC_CMD_START_TRANS 		0x03
#define EC_CMD_WRITE_DATA		0x04
#define EC_CMD_EXIT_PM  		0x05
#define EC_CMD_ENTRY_FLASHMODE	0x0DF
#define EC_CMD_EXIT_FLASHMODE	0x0FE
#define EC_CMD_RESTART  		0x0FF
#define EC_NO_ERROR_FM_ACK		0x0FA
#define EC_CMD_GET_CHIPID		0x0A3
#define EC_CMD_DISABLE_KBD		0x0AD
#define EC_CMD_ENABLE_KBD		0x0AE

VOID StdECEntry (IN VOID *Buffer, IN OUT UINT8 *pOFBDDataHandled);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
