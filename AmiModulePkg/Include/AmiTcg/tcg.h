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
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/tcg.h 9     7/24/11 10:00p Fredericko $
//
// $Revision: 9 $
//
// $Date: 7/24/11 10:00p $
//**********************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  Tcg.h
//
// Description: 
//  Tcg Header file
//
//<AMI_FHDR_END>
//*************************************************************************

#define CMOS_BANK1_INDEXREG     0x70
#define CMOS_BANK2_INDEXREG     0x72
#define CMOS_BANK1_SIZE         0x80

// *****************************************************************************************************************************/
//         PHYSICAL PRESENCE INTERFACE
// *****************************************************************************************************************************/
#define TCPA_PPI_USERABORT       0xFFF0
#define TCPA_PPI_BIOSFAIL        0xFFF1
#define TCPA_PPI_RESPONSE_MASK   0xFFFF0000

#define TCPA_PPIOP_ENABLE                       1
#define TCPA_PPIOP_DISABLE                      2
#define TCPA_PPIOP_ACTIVATE                     3
#define TCPA_PPIOP_DEACTIVATE                   4
#define TCPA_PPIOP_CLEAR                        5
#define TCPA_PPIOP_ENABLE_ACTV                  6
#define TCPA_PPIOP_DEACT_DSBL                   7
#define TCPA_PPIOP_OWNER_ON                     8
#define TCPA_PPIOP_OWNER_OFF                    9
#define TCPA_PPIOP_ENACTVOWNER                  10
#define TCPA_PPIOP_DADISBLOWNER                 11
#define TCPA_PPIOP_UNOWNEDFIELDUPGRADE          12
#define TCPA_PPIOP_SETOPAUTH                    13
#define TCPA_PPIOP_CLEAR_ENACT                  14
#define TCPA_PPIOP_SETNOPPIPROVISION_FALSE      15
#define TCPA_PPIOP_SETNOPPIPROVISION_TRUE       16
#define TCPA_PPIOP_SETNOPPICLEAR_FALSE          17
#define TCPA_PPIOP_SETNOPPICLEAR_TRUE           18
#define TCPA_PPIOP_SETNOPPIMAINTENANCE_FALSE    19
#define TCPA_PPIOP_SETNOPPIMAINTENANCE_TRUE     20
#define TCPA_PPIOP_ENABLE_ACTV_CLEAR            21
#define TCPA_PPIOP_ENABLE_ACTV_CLEAR_ENABLE_ACTV   22
#define TCPA_PPIOP_SET_PCR_BANKS                23
#define TCPA_PPIOP_CHANGE_EPS                   24
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_PCR_FALSE  25
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_PCR_TRUE   26
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_ON_FALSE     27
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_ON_TRUE      28
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_OFF_FALSE     29
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_TURN_OFF_TRUE      30
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_EPS_FALSE   31
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_CHANGE_EPS_TRUE    32
#define TCPA_PPIOP_LOG_ALL_DIGESTS              33
#define TCPA_PPIOP_DISABLE_ENDORSEMENT_ENABLE_STORAGE_HIERARCHY     34
#define TCPA_PPIOP_ENABLE_BLOCK_SID_FUNC     96
#define TCPA_PPIOP_DISABLE_BLOCK_SID_FUNC    97
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_TRUE    98
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_FALSE   99
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_TRUE    100
#define TCPA_PPIOP_SET_PPREQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_FALSE   101
#define TCPA_PPIOP_VENDOR   128

#define PPIXOP_ENABLE            0x01
#define PPIXOP_DISABLE           0x02
#define PPIXOP_ACTIVATE          0x04
#define PPIXOP_DEACTIVATE        0x08
#define PPIXOP_OWNER_ON          0x10
#define PPIXOP_OWNER_OFF         0x20
#define PPIXOP_CLEAR             0x40

#define PPIXOP_EN_FLAG           (PPIXOP_ENABLE | PPIXOP_DISABLE)
#define PPIXOP_A_FLAG            (PPIXOP_ACTIVATE | PPIXOP_DEACTIVATE)
#define PPIXOP_O_FLAG            (PPIXOP_OWNER_ON | PPIXOP_OWNER_OFF)

#define PPI_MAX_BASIC_OP         5
#define PPI_FEATURE_ON           1
#define PPI_FEATURE_OFF          2
#define PPI_FEATURE_CHANGE        (PPI_FEATURE_ON | PPI_FEATURE_OFF)


//
// TSS event strings
//
#define TSS_ACTION_CALLING_INT19             "Calling INT 19h"
#define TSS_ACTION_RETURNED_INT19            "Returned INT 19h"
#define TSS_ACTION_RETURNED_INT18            "Return via INT 18h"
#define TSS_ACTION_BOOTING_BCV_DEVICE        "Booting BCV Device "
#define TSS_ACTION_BOOTING_BEV_DEVICE        "Booting BEV Device "
#define TSS_ACTION_ROM_BASED_SETUP           "Entering ROM Based Setup"
#define TSS_ACTION_BOOTING_PARTIES           "Booting to Parties "
#define TSS_ACTION_USER_PASSWORD             "User Password Entered"
#define TSS_ACTION_ADMINISTRATOR_PASSWORD    "Administrator Password Entered"
#define TSS_ACTION_PASSWORD_FAILURE          "Password Failure"
#define TSS_ACTION_WAKE_EVENT                "Wake Event n"
#define TSS_ACTION_BOOT_SEQ_INTERVENTION     "Boot Sequence User Intervention"
#define TSS_ACTION_CHASSIS_INTRUSION         "Chassis Intrusion"
#define TSS_ACTION_NON_FATAL_ERROR           "Non Fatal Error"
#define TSS_ACTION_OPROM_SCAN                "Start Option ROM Scan"
#define TSS_ACTION_UNHIDEING_OPROM           "Unhiding Option ROM Code"

#define TSS_ACTION_IPL_PARTITION             "Booting from "
#define TSS_ACTION_IPL_LEGACY_PARTITION      "Booting from Legacy "
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
