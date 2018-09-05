//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file OemPeiIoTable.c
  SIO init table in PEI phase. Any customers have to review below tables
  for themselves platform and make sure each initialization is necessary.

  @note In all tables, only fill with necessary setting. Don't fill with default
**/
//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <Library/AmiSioPeiLib.h>

///---------------------------------------------------------------------
///Decode table for PEI phase.
///---------------------------------------------------------------------
IO_DECODE_DATA Pilot4PeiDecodeTable[]= {
    // -----------------------------
    //|  BaseAdd | UID  | Type |
    // -----------------------------
    {PILOT4_CONFIG_INDEX, 2, 0xFF},
/*	EIP269626<<
    // !!!!Attention!!!!This is necessary
    //OEM_TODO//OEM_TODO//OEM_TODO//OEM_TODO
#if (PILOT4_KCS3_PRESENT)
    {0x62, 0, 0xFF},                                        // KCS3 decode
#endif
    //Below decode is for SIO generic IO decode
#if defined(PILOT4_TOTAL_BASE_ADDRESS) && (PILOT4_TOTAL_BASE_ADDRESS != 0)
    {PILOT4_TOTAL_BASE_ADDRESS, PILOT4_TOTAL_LENGTH, 0xFF}, //PSR,KCS5,BT,WDT,Mail Box Decode
#endif
#if (PILOT4_SWC_PRESENT)
    {PILOT4_SWC_BASE_ADDRESS, PILOT4_SWC_LENGTH, 0xFF},     // SWC decode
#endif
#if (PILOT4_HSPI_PRESENT)
    {PILOT4_HSPI_BASE_ADDRESS, PILOT4_HSPI_LENGTH, 0xFF},   // HSPI decode
#endif
#if (PILOT4_KCS4_PRESENT)
    {PILOT4_KCS4_DATA_BASE_ADDRESS, 0x10, 0xFF},            // KCS4(0xCA2~0xCA3) / SMIC(0xCA8~0xCAC) decode
#endif
EIP269626<< */

};

///---------------------------------------------------------------------
///The PEI decode table count.
///---------------------------------------------------------------------
UINT8 Pilot4PeiDecodeTableCount = sizeof(Pilot4PeiDecodeTable)/sizeof(IO_DECODE_DATA);

///---------------------------------------------------------------------
///Init table for PEI phase.
///---------------------------------------------------------------------
SIO_DEVICE_INIT_DATA Pilot4PeiInitTable[]= {
    // -------------------------------
    //|  Addr | DataMask  | DataValue |
    // -------------------------------
    //------------------------------------------------------------------
    // Enter Configuration Mode.
    //------------------------------------------------------------------
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_CONFIG_MODE_ENTER_VALUE},
    //------------------------------------------------------------------
    // Before init all logical devices, program Global register if needed.
    //------------------------------------------------------------------
    // Device Disabled Setting

    //------------------------------------------------------------------
    // Program and initialize some logical device if needed.
    //------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device 0 (PSR) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_PSR},
#if (PILOT4_PSR_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_PSR_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_PSR_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //------------------------------------------------------------------
    // Program and initialize Logical Device 3 (SWC) Configuration
    //------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_SWC},
#if (PILOT4_SWC_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_SWC_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_SWC_BASE_ADDRESS >> 8)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)((PILOT4_SWC_BASE_ADDRESS + 80) & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)((PILOT4_SWC_BASE_ADDRESS + 80) >> 8)},
    {PILOT4_CONFIG_INDEX, 0x00, 0x65},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)((PILOT4_SWC_BASE_ADDRESS + 84) & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, 0x64},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)((PILOT4_SWC_BASE_ADDRESS + 84) >> 8)},
    {PILOT4_CONFIG_INDEX, 0x00, 0x67},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)((PILOT4_SWC_BASE_ADDRESS + 86) & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, 0x66},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)((PILOT4_SWC_BASE_ADDRESS + 86) >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device 5 (WDT) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_WDT},
#if (PILOT4_BT_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_WDT_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_WDT_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device 8 (KCS3) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_KCS3},
#if (PILOT4_KCS3_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS3_DATA_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS3_DATA_BASE_ADDRESS >> 8)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS3_CMD_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS3_CMD_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Clear Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device 9 (KCS4) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_KCS4},
#if (PILOT4_KCS4_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS4_DATA_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS4_DATA_BASE_ADDRESS >> 8)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS4_CMD_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS4_CMD_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Clear Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE2_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device A (KCS5) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_KCS5},
#if (PILOT4_KCS5_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS5_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_KCS5_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device B (BT) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_BT},
#if (PILOT4_BT_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_BT_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_BT_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Clear Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif

    //--------------------------------------------------------------------------
    // Program and initialize Logical Device C (SMIC) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_SMIC},
#if (PILOT4_SMIC_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_SMIC_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_SMIC_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Clear Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, 0x00},
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device D (MAILBOX) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_MAILBOX},
#if (PILOT4_MAILBOX_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_MAILBOX_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_MAILBOX_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device F (SPI) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_HSPI},
#if (PILOT4_HSPI_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_HSPI_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_HSPI_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
    
    //--------------------------------------------------------------------------
    // Program and initialize Logical Device 10 (TAP Controller) Configuration
    //--------------------------------------------------------------------------
    // Seclect device 
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_LDN_SEL_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_LDN_TAPC},
#if (PILOT4_TAP_CONTROLLER_PRESENT)
    // Program Base Addr
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_LO_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_TAPC_BASE_ADDRESS & 0xFF)},
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_BASE1_HI_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, (UINT8)(PILOT4_TAPC_BASE_ADDRESS >> 8)},
    // Activate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_ACTIVATE_VALUE},
#else
    // Deactivate Device
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_ACTIVATE_REGISTER},
    {PILOT4_CONFIG_DATA,  0x00, PILOT4_DEACTIVATE_VALUE},
#endif
   
    //------------------------------------------------------------------
    // After init all logical devices, program Global register if needed.
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // After init all logical devices,  Exit Configuration Mode.
    //------------------------------------------------------------------
    {PILOT4_CONFIG_INDEX, 0x00, PILOT4_CONFIG_MODE_EXIT_VALUE},

};

///---------------------------------------------------------------------
///The PEI init table count.
///---------------------------------------------------------------------
UINT8 Pilot4PeiInitTableCount = sizeof(Pilot4PeiInitTable)/sizeof(SIO_DEVICE_INIT_DATA);

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

