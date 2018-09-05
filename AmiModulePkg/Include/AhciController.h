//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AhciController.h
    Header file for the Ahci Controller related definition

**/


#ifndef _AHCI_CONTROLLER_H
#define _AHCI_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif


// Forward reference for pure ANSI compatibility
typedef struct _AHCI_COMMAND_FIS AHCI_COMMAND_FIS;
typedef struct _AHCI_ATAPI_COMMAND AHCI_ATAPI_COMMAND;
typedef struct _AHCI_COMMAND_PRDT AHCI_COMMAND_PRDT;

#pragma pack(1)

// Command List Structure
typedef struct {
    UINT32          Ahci_Cmd_CFL:5;
    UINT32          Ahci_Cmd_A:1;
    UINT32          Ahci_Cmd_W:1;
    UINT32          Ahci_Cmd_P:1;
    UINT32          Ahci_Cmd_R:1;
    UINT32          Ahci_Cmd_B:1;
    UINT32          Ahci_Cmd_C:1;
    UINT32          Ahci_Cmd_Rsvd1:1;
    UINT32          Ahci_Cmd_PMP:4;
    UINT32          Ahci_Cmd_PRDTL:16;  
    UINT32          Ahci_Cmd_PRDBC;
    UINT32          Ahci_Cmd_CTBA;
    UINT32          Ahci_Cmd_CTBAU;  
    UINT32          Ahci_Cmd_Rsvd2[4]; 
} AHCI_COMMAND_LIST;

typedef struct {
    UINT8           Ahci_Dsfis[0x1C];               // DMA Setup Fis
    UINT8           Ahci_Dsfis_Rsvd[0x04];             
    UINT8           Ahci_Psfis[0x14];               // PIO Setup Fis
    UINT8           Ahci_Psfis_Rsvd[0x0C];             
    UINT8           Ahci_Rfis[0x14];                // D2H Register Fis
    UINT8           Ahci_Rfis_Rsvd[0x04];            
    UINT64          Ahci_Sdbfis;                     // Set Device Bits Fis
    UINT8           Ahci_Ufis[0x40];                 // Unknown FIS
    UINT8           Ahci_Ufis_Rsvd[0x60];            
} AHCI_RECEIVED_FIS; 

// Register - Host to Device FIS Layout
struct _AHCI_COMMAND_FIS{
    UINT8           Ahci_CFis_Type;
    UINT8           AHci_CFis_PmPort:4;
    UINT8           Ahci_CFis_Rsvd1:1;
    UINT8           Ahci_CFis_Rsvd2:1;
    UINT8           Ahci_CFis_Rsvd3:1;
    UINT8           Ahci_CFis_C:1;
    UINT8           Ahci_CFis_Cmd;
    UINT8           Ahci_CFis_Features;
    UINT8           Ahci_CFis_SecNum;
    UINT8           Ahci_CFis_ClyLow;
    UINT8           Ahci_CFis_ClyHigh;
    UINT8           Ahci_CFis_DevHead;
    UINT8           Ahci_CFis_SecNumExp;
    UINT8           Ahci_CFis_ClyLowExp;
    UINT8           Ahci_CFis_ClyHighExp;
    UINT8           Ahci_CFis_FeaturesExp;
    UINT8           Ahci_CFis_SecCount;
    UINT8           Ahci_CFis_SecCountExp;
    UINT8           Ahci_CFis_Rsvd4;
    UINT8           Ahci_CFis_Control;
    UINT8           Ahci_CFis_Rsvd5[4];
    UINT8           Ahci_CFis_Rsvd6[44];
} ;

// Physical Region Descriptor Table
struct _AHCI_COMMAND_PRDT{
    UINT32          Ahci_Prdt_DBA;
    UINT32          Ahci_Prdt_DBAU;
    UINT32          Ahci_Prdt_Rsvd;
    UINT32          Ahci_Prdt_DBC:22;  
    UINT32          Ahci_Prdt_Rsvd1:9;
    UINT32          Ahci_Prdt_I:1;  
};

// Command table

typedef struct _AHCI_COMMAND_TABLE{
    AHCI_COMMAND_FIS        CFis;
    AHCI_ATAPI_COMMAND      AtapiCmd;
    UINT8                   Rsvd[0x30];
    AHCI_COMMAND_PRDT       PrdtTable;
}   AHCI_COMMAND_TABLE;

typedef struct _AHCI_COMMAND_TABLE_NO_PRDT{
    AHCI_COMMAND_FIS        CFis;
    AHCI_ATAPI_COMMAND      AtapiCmd;
    UINT8                   Rsvd[0x30];
}   AHCI_COMMAND_TABLE_NO_PRDT;

#pragma pack()

#define     ATA_SIGNATURE_32                0x00000101
#define     ATAPI_SIGNATURE_32              0xEB140101
#define     PMPORT_SIGNATURE                0x96690101
#define     PRD_MAX_DATA_COUNT              0x400000

#define     PCI_LBAR_INDEX                  4
#define     PCI_ABAR_INDEX                  5
#define     PCI_LBAR_ADDRESS_MASK           0xFFFFFFE0
#define     INDEX_OFFSET_FROM_LBAR          0x10
#define     DATA_OFFSET_FROM_LBAR           0x14

#define     PCI_LBAR                        0x20
#define     PCI_ABAR                        0x24
#define     RECEIVED_FIS_SIZE               0x100

#define     CONTROL_PORT                    0x0F

//Generic Host Control Registers
#define     HBA_CAP                         0x0000
#define     HBA_CAP_NP_MASK                 0x1F
#define     HBA_CAP_EMS                     BIT06
#define     HBA_CAP_PSC                     BIT13
#define     HBA_CAP_SSC                     BIT14
#define     HBA_CAP_PMD                     BIT15
#define     HBA_CAP_FBSS                    BIT16
#define     HBA_CAP_SPM                     BIT17
#define     HBA_CAP_SAM                     BIT18
#define     HBA_CAP_SNZO                    BIT19
#define     HBA_CAP_ISS_MASK                (BIT20 | BIT21 | BIT22 | BIT23)
#define     HBA_CAP_SCLO                    BIT24
#define     HBA_CAP_SAL                     BIT25
#define     HBA_CAP_SALP                    BIT26
#define     HBA_CAP_SSS                     BIT27
#define     HBA_CAP_SMPS                    BIT28
#define     HBA_CAP_SSNTF                   BIT29
#define     HBA_CAP_SCQA                    BIT30
#define     HBA_CAP_S64A                    BIT31

#define     HBA_GHC                         0x0004
#define     HBA_GHC_RESET                   0x0001
#define     HBA_GHC_IE                      0x0002
#define     HBA_GHC_AE                      0x80000000
#define     HBA_GHC_AE_RESET                0x80000001
#define     HBA_IS                          0x0008
#define     HBA_PI                          0x000C
#define     HBA_VS                          0x0010
#define     HBA_CCC_CTL                     0x0014
#define     HBA_CCC_PORTS                   0x0018
#define     HBA_EM_LOC                      0x001C
#define     HBA_EM_CTL                      0x0020
#define     HBA_CAP2                        0x0024
#define     HBA_CAP2_APST                   0x0004
#define     HBA_CAP2_SDS                    0x0008
#define     HBA_CAP2_SADM                   0x0010
#define     HBA_CAP2_DESO                   0x0020

//Port Registers
#define     HBA_PORTS_START                 0x0100
#define     HBA_PORTS_REG_WIDTH             0x0080
#define     HBA_PORTS_CLB                   0x0000
#define     HBA_PORTS_CLBU                  0x0004
#define     HBA_PORTS_FB                    0x0008
#define     HBA_PORTS_FBU                   0x000C
#define     HBA_PORTS_IS                    0x0010
#define     HBA_PORTS_IS_DHRS               BIT00
#define     HBA_PORTS_IS_PSS                BIT01
#define     HBA_PORTS_IS_SSS                BIT02
#define     HBA_PORTS_IS_SDBS               BIT03
#define     HBA_PORTS_IS_UFS                BIT04
#define     HBA_PORTS_IS_DPS                BIT05
#define     HBA_PORTS_IS_PCS                BIT06
#define     HBA_PORTS_IS_DIS                BIT07
#define     HBA_PORTS_IS_PRCS               BIT22
#define     HBA_PORTS_IS_IPMS               BIT23
#define     HBA_PORTS_IS_OFS                BIT24
#define     HBA_PORTS_IS_INFS               BIT26
#define     HBA_PORTS_IS_IFS                BIT27
#define     HBA_PORTS_IS_HBDS               BIT28
#define     HBA_PORTS_IS_HBFS               BIT29
#define     HBA_PORTS_IS_TFES               BIT30
#define     HBA_PORTS_IS_CPDS               BIT31
#define     HBA_PORTS_IS_CLEAR              0xFFC000FF
#define     HBA_PORTS_IS_FIS_CLEAR          0x0000001F
#define     HBA_PORTS_IS_ERR_CHK            BIT04 + BIT06 + BIT23 + BIT24 + BIT27 + \
                                            BIT28 + BIT29 + BIT30 + BIT31

#define     HBA_PORTS_IE                    0x0014
#define     HBA_PORTS_CMD                   0x0018
#define     HBA_PORTS_CMD_ST_MASK           0xFFFFFFFE
#define     HBA_PORTS_CMD_ST                BIT00
#define     HBA_PORTS_CMD_SUD               BIT01
#define     HBA_PORTS_CMD_POD               BIT02
#define     HBA_PORTS_CMD_CLO               BIT03
#define     HBA_PORTS_CMD_CR                BIT15
#define     HBA_PORTS_CMD_FRE               BIT04
#define     HBA_PORTS_CMD_FR                BIT14
#define     HBA_PORTS_CMD_MASK              ~(HBA_PORTS_CMD_ST | HBA_PORTS_CMD_FRE | HBA_PORTS_CMD_CLO)
#define     HBA_PORTS_CMD_PMA               BIT17
#define     HBA_PORTS_CMD_HPCP              BIT18
#define     HBA_PORTS_CMD_MPSP              BIT19
#define     HBA_PORTS_CMD_CPD               BIT20
#define     HBA_PORTS_CMD_ESP               BIT21
#define     HBA_PORTS_CMD_ATAPI             BIT24
#define     HBA_PORTS_CMD_DLAE              BIT25
#define     HBA_PORTS_CMD_ALPE              BIT26
#define     HBA_PORTS_CMD_ASP               BIT27
#define     HBA_PORTS_CMD_ICC_MASK          (BIT28 | BIT29 | BIT30 | BIT31)
#define     HBA_PORTS_CMD_ACTIVE            (1 << 28 )
#define     HBA_PORTS_TFD                   0x0020
#define     HBA_PORTS_TFD_MASK              (BIT07 | BIT03 | BIT00)
#define     HBA_PORTS_TFD_BSY               BIT07
#define     HBA_PORTS_TFD_DRQ               BIT03
#define     HBA_PORTS_TFD_ERR               BIT00
#define     HBA_PORTS_TFD_ERR_MASK          0x00FF00    // BIT8 - BIT15
#define     HBA_PORTS_SIG                   0x0024
#define     HBA_PORTS_SSTS                  0x0028
#define     HBA_PORTS_SSTS_DET_MASK         0x000F
#define     HBA_PORTS_SSTS_DET              0x0001
#define     HBA_PORTS_SSTS_DET_PCE          0x0003
#define     HBA_PORTS_SSTS_SPD_MASK         0x00F0
#define     HBA_PORTS_SCTL                  0x002C
#define     HBA_PORTS_SCTL_DET_MASK         0x000F
#define     HBA_PORTS_SCTL_MASK             (~HBA_PORTS_SCTL_DET_MASK)
#define     HBA_PORTS_SCTL_DET_INIT         0x0001
#define     HBA_PORTS_SCTL_DET_PHYCOMM      0x0003
#define     HBA_PORTS_SCTL_SPD_MASK         0x00F0
#define     HBA_PORTS_SCTL_SPD_NSNR         0x0
#define     HBA_PORTS_SCTL_SPD_GEN1         0x1
#define     HBA_PORTS_SCTL_SPD_GEN2         0x2
#define     HBA_PORTS_SCTL_SPD_GEN3         0x3
#define     HBA_PORTS_SCTL_IPM_MASK         0x0F00
#define     HBA_PORTS_SCTL_IPM_DIS          0x00
#define     HBA_PORTS_SCTL_IPM_PSD          0x01
#define     HBA_PORTS_SCTL_IPM_SSD          0x02
#define     HBA_PORTS_SCTL_IPM_PSSD         0x03
#define     HBA_PORTS_SCTL_IPM_PSD_SSD      0x0300
#define     HBA_PORTS_SERR                  0x0030
#define     HBA_PORTS_SERR_RDIE             BIT00
#define     HBA_PORTS_SERR_RCE              BIT01
#define     HBA_PORTS_SERR_TDIE             BIT08
#define     HBA_PORTS_SERR_PCDIE            BIT09
#define     HBA_PORTS_SERR_PE               BIT10
#define     HBA_PORTS_SERR_IE               BIT11
#define     HBA_PORTS_SERR_PRC              BIT16
#define     HBA_PORTS_SERR_PIE              BIT17
#define     HBA_PORTS_SERR_CW               BIT18
#define     HBA_PORTS_SERR_BDE              BIT19
#define     HBA_PORTS_SERR_DE               BIT20                // Not used
#define     HBA_PORTS_SERR_CRCE             BIT21
#define     HBA_PORTS_SERR_HE               BIT22
#define     HBA_PORTS_SERR_LSE              BIT23
#define     HBA_PORTS_SERR_TSTE             BIT24
#define     HBA_PORTS_SERR_UFT              BIT25
#define     HBA_PORTS_SERR_EX               BIT26
#define     HBA_PORTS_PxSACT                0x0034
#define     HBA_PORTS_PxCI                  0x0038
#define     HBA_PORTS_PxDEVSLP              0x0044
#define     HBA_PORTS_PxDEVSLP_ADSE         BIT00
#define     HBA_PORTS_PxDEVSLP_DSP          BIT01
#define     HBA_PORTS_PxDEVSLP_DETO_MASK    (0x000003FC)
#define     HBA_PORTS_PxDEVSLP_DMDAT_MASK   (0x00007C00)
#define     HBA_PORTS_PxDEVSLP_DITO_MASK    (0x01FF8000)
#define     HBA_PORTS_PxDEVSLP_DM_MASK      (0x1E000000)

#define     HBA_PORTS_ERR_CHK           (HBA_PORTS_SERR_TDIE + HBA_PORTS_SERR_PCDIE +\
                                        HBA_PORTS_SERR_PE + HBA_PORTS_SERR_IE + \
                                        HBA_PORTS_SERR_PIE + \
                                        HBA_PORTS_SERR_BDE + \
                                        HBA_PORTS_SERR_DE + HBA_PORTS_SERR_CRCE + \
                                        HBA_PORTS_SERR_HE +  HBA_PORTS_SERR_LSE + \
                                        HBA_PORTS_SERR_TSTE + HBA_PORTS_SERR_UFT  + \
                                        HBA_PORTS_SERR_EX)

#define     HBA_PORTS_ERR_CLEAR         (HBA_PORTS_SERR_RDIE + HBA_PORTS_SERR_RCE +\
                                        HBA_PORTS_SERR_TDIE + HBA_PORTS_SERR_PCDIE +\
                                        HBA_PORTS_SERR_PE + HBA_PORTS_SERR_IE + \
                                        HBA_PORTS_SERR_PRC + HBA_PORTS_SERR_PIE + \
                                        HBA_PORTS_SERR_CW + HBA_PORTS_SERR_BDE + \
                                        HBA_PORTS_SERR_DE + HBA_PORTS_SERR_CRCE + \
                                        HBA_PORTS_SERR_HE +  HBA_PORTS_SERR_LSE + \
                                        HBA_PORTS_SERR_TSTE + HBA_PORTS_SERR_UFT  + \
                                        HBA_PORTS_SERR_EX)

#define     HBA_PORTS_SACT                  0x0034
#define     HBA_PORTS_CI                    0x0038
#define     HBA_PORTS_SNTF                  0x003C

//FIS Types
#define     D2H_FIS_OFFSET                  0x40
#define     DMA_FIS_OFFSET                  0x00
#define     PIO_FIS_OFFSET                  0x20
#define     SDB_FIS_OFFSET                  0x58
#define     FIS_TYPE_MASK                   0xFF
#define     U_FIS_OFFSET                    0x60

#define     FIS_REGISTER_H2D                0x27            // Host To Device
#define     FIS_REGISTER_H2D_LENGTH         20

#define     FIS_REGISTER_D2H                0x34            // Device To Host
#define     FIS_REGISTER_D2H_LENGTH         20

#define     FIS_DMA_ACTIVATE                0x39            // Device To Host
#define     FIS_DMA_ACTIVATE_LENGTH         4

#define     FIS_DMA_SETUP                   0x41            // Bi-directional
#define     FIS_DMA_SETUP_LENGTH            28

#define     FIS_DATA                        0x46            // Bi-directional

#define     FIS_BIST                        0x58            // Bi-directional
#define     FIS_BIST_LENGTH                 12

#define     FIS_PIO_SETUP                   0x5F            // Device To Host
#define     FIS_PIO_SETUP_LENGTH            20

#define     FIS_SET_DEVICE                  0xA1            // Device To Host
#define     FIS_SET_DEVICE_LENGTH           8

#define     READ_PORT_MULTIPLIER            0xE4
#define     WRITE_PORT_MULTIPLIER           0xE8

#define     GSCR_0                          0x00
#define     GSCR_1                          0x01
#define     GSCR_2                          0x02
#define     GSCR_32                         32
#define     GSCR_64                         64
#define     GSCR_96                         96

#define     PSCR_0_SSTATUS                  0x00
#define     PSCR_1_SERROR                   0x01
#define     PSCR_2_SCONTROL                 0x02


#define     HBA_CR_CLEAR_TIMEOUT            500          // AHCI 1.2 spec 10.1.2
#define     HBA_FR_CLEAR_TIMEOUT            500          // AHCI 1.2 spec 10.1.2
#define     HBA_CI_CLEAR_TIMEOUT            200          // 200ms
#define     HBA_PRESENCE_DETECT_TIMEOUT     10           // 10msec Serial ATA 1.0 Sec 5.2

#define     HBA_PORT_REG_BASE(Port) \
            (UINTN) (Port * HBA_PORTS_REG_WIDTH + HBA_PORTS_START)

#define     HBA_REG32( pAhciBusInterface, Register ) \
            (ReadDataDword ((pAhciBusInterface), (Register)))

#define     HBA_WRITE_REG32( pAhciBusInterface, Register, Data ) \
            (WriteDataDword( pAhciBusInterface, Register, Data ))

#define     HBA_REG16( pAhciBusInterface, Register ) \
            (ReadDataWord( pAhciBusInterface, Register ))

#define     HBA_WRITE_REG16( pAhciBusInterface, Register, Data ) \
            (WriteDataWord( pAhciBusInterface, Register, Data ))

#define     HBA_REG8( pAhciBusInterface, Register ) \
            (ReadDataByte ((pAhciBusInterface), (Register)))

#define     HBA_WRITE_REG8( pAhciBusInterface, Register, Data ) \
            (WriteDataByte( pAhciBusInterface, Register, Data ))

#define     HBA_REG8_OR( pAhciBusInterface, Register, OrData) \
            HBA_WRITE_REG8(pAhciBusInterface, Register, ((HBA_REG8 ((pAhciBusInterface), (Register))) | ((UINT8) (OrData))))

#define     HBA_REG16_OR( pAhciBusInterface, Register, OrData) \
            HBA_WRITE_REG16(pAhciBusInterface, Register, ((HBA_REG16 ((pAhciBusInterface), (Register))) | ((UINT16) (OrData))))

#define     HBA_REG32_OR( pAhciBusInterface, Register, OrData) \
            HBA_WRITE_REG32(pAhciBusInterface, Register, ((HBA_REG32 ((pAhciBusInterface), (Register))) | ((UINT32) (OrData))))

#define     HBA_REG8_AND( pAhciBusInterface, Register, AndData) \
            HBA_WRITE_REG8(pAhciBusInterface, Register, ((HBA_REG8 ((pAhciBusInterface), (Register))) & ((UINT8) (AndData))))

#define     HBA_REG16_AND( pAhciBusInterface, Register, AndData) \
            HBA_WRITE_REG16(pAhciBusInterface, Register, ((HBA_REG16 ((pAhciBusInterface), (Register))) & ((UINT16) (AndData))))
 
#define     HBA_REG32_AND( pAhciBusInterface, Register, AndData) \
            HBA_WRITE_REG32(pAhciBusInterface, Register, ((HBA_REG32 ((pAhciBusInterface), (Register))) & ((UINT32) (AndData))))

#define     HBA_REG8_AND_OR( pAhciBusInterface, Register, AndData, OrData) \
            HBA_WRITE_REG8(pAhciBusInterface, Register, ((HBA_REG8 ((pAhciBusInterface), (Register))) & ((UINT8) (AndData)) | ((UINT8) (OrData))))

#define     HBA_REG16_AND_OR( pAhciBusInterface, Register, AndData, OrData) \
            HBA_WRITE_REG16(pAhciBusInterface, Register, ((HBA_REG16 ((pAhciBusInterface), (Register))) & ((UINT16) (AndData)) | ((UINT16) (OrData))))

#define     HBA_REG32_AND_OR( pAhciBusInterface, Register,AndData,  OrData) \
            HBA_WRITE_REG32(pAhciBusInterface, Register, ((HBA_REG32 ((pAhciBusInterface), (Register))) & ((UINT32) (AndData)) | ((UINT32) (OrData))))
  
//Ports
#define     HBA_PORT_REG8(pAhciBusInterface, Port, Register) \
            (HBA_REG8 ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port))))

#define     HBA_PORT_REG16(pAhciBusInterface, Port, Register) \
            (HBA_REG16 ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port))))

#define     HBA_PORT_REG32(pAhciBusInterface, Port, Register) \
            (HBA_REG32 ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port))))

#define     HBA_PORT_REG64(pAhciBusInterface, Port, Register) \
              ( ( Shl64(((UINT64)(HBA_PORT_REG32(pAhciBusInterface, Port, (Register+4)))),32)) | \
                 (HBA_PORT_REG32(pAhciBusInterface, Port, Register)) )
             
#define     HBA_PORT_WRITE_REG8(pAhciBusInterface, Port, Register, Data) \
            (HBA_WRITE_REG8 ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), Data))

#define     HBA_PORT_WRITE_REG16(pAhciBusInterface, Port, Register, Data) \
            (HBA_WRITE_REG16 ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)),Data))

#define     HBA_PORT_WRITE_REG32(pAhciBusInterface, Port, Register,Data) \
            (HBA_WRITE_REG32 ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)),Data))

#define     HBA_PORT_WRITE_REG64(pAhciBusInterface, Port, Register,Data)\
             HBA_PORT_WRITE_REG32(pAhciBusInterface, Port, (Register+4), ((UINT32)(Shr64(Data,32)))); \
             HBA_PORT_WRITE_REG32(pAhciBusInterface, Port, Register, ((UINT32)Data))

#define     HBA_PORT_REG8_OR(pAhciBusInterface, Port, Register, OrData) \
            (HBA_REG8_OR ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (OrData)))

#define     HBA_PORT_REG16_OR(pAhciBusInterface, Port, Register, OrData) \
            (HBA_REG16_OR ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (OrData)))

#define     HBA_PORT_REG32_OR(pAhciBusInterface, Port, Register, OrData) \
            (HBA_REG32_OR ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (OrData)))

#define     HBA_PORT_REG8_AND(pAhciBusInterface, Port, Register, AndData) \
            (HBA_REG8_AND ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData)))

#define     HBA_PORT_REG16_AND(pAhciBusInterface, Port, Register, AndData) \
            (HBA_REG16_AND ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData)))

#define     HBA_PORT_REG32_AND(pAhciBusInterface, Port, Register, AndData) \
            (HBA_REG32_AND ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData)))

#define     HBA_PORT_REG8_AND_OR(pAhciBusInterface, Port, Register, AndData, OrData) \
            (HBA_REG8_AND_OR ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData), (OrData)))

#define     HBA_PORT_REG16_AND_OR(pAhciBusInterface, Port, Register, AndData, OrData) \
            (HBA_REG16_AND_OR ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData), (OrData)))

#define     HBA_PORT_REG32_AND_OR(pAhciBusInterface, Port, Register, AndData, OrData) \
            (HBA_REG32_AND_OR ((pAhciBusInterface), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData), (OrData)))

#define     SMM_HBA_REG32( BaseAddr, Register ) \
            (SmmReadDataDword ((BaseAddr), (Register)))

#define     SMM_HBA_WRITE_REG32( BaseAddr, Register, Data ) \
            (SmmWriteDataDword( BaseAddr, Register, Data ))

#define     SMM_HBA_REG16( BaseAddr, Register ) \
            (SmmReadDataWord( BaseAddr, Register ))

#define     SMM_HBA_WRITE_REG16( BaseAddr, Register, Data ) \
            (SmmWriteDataWord( BaseAddr, Register, Data ))

#define     SMM_HBA_REG8( BaseAddr, Register ) \
            (SmmReadDataByte ((BaseAddr), (Register)))

#define     SMM_HBA_WRITE_REG8( BaseAddr, Register, Data ) \
            (SmmWriteDataByte( BaseAddr, Register, Data ))

#define     SMM_HBA_REG8_OR( BaseAddr, Register, OrData) \
            SMM_HBA_WRITE_REG8(BaseAddr, Register, ((SMM_HBA_REG8 ((BaseAddr), (Register))) | ((UINT8) (OrData))))

#define     SMM_HBA_REG16_OR( BaseAddr, Register, OrData) \
            SMM_HBA_WRITE_REG16(BaseAddr, Register, ((SMM_HBA_REG16 ((BaseAddr), (Register))) | ((UINT16) (OrData))))

#define     SMM_HBA_REG32_OR( BaseAddr, Register, OrData) \
            SMM_HBA_WRITE_REG32(BaseAddr, Register, ((SMM_HBA_REG32 ((BaseAddr), (Register))) | ((UINT32) (OrData))))

#define     SMM_HBA_REG8_AND( BaseAddr, Register, AndData) \
            SMM_HBA_WRITE_REG8(BaseAddr, Register, ((SMM_HBA_REG8 ((BaseAddr), (Register))) & ((UINT8) (AndData))))

#define     SMM_HBA_REG16_AND( BaseAddr, Register, AndData) \
            SMM_HBA_WRITE_REG16(BaseAddr, Register, ((SMM_HBA_REG16 ((BaseAddr), (Register))) & ((UINT16) (AndData))))
 
#define     SMM_HBA_REG32_AND( BaseAddr, Register, AndData) \
            SMM_HBA_WRITE_REG32(BaseAddr, Register, ((SMM_HBA_REG32 ((BaseAddr), (Register))) & ((UINT32) (AndData))))

#define     SMM_HBA_REG8_AND_OR( BaseAddr, Register, AndData, OrData) \
            SMM_HBA_WRITE_REG8(BaseAddr, Register, ((SMM_HBA_REG8 ((BaseAddr), (Register))) & ((UINT8) (AndData)) | ((UINT8) (OrData))))

#define     SMM_HBA_REG16_AND_OR( BaseAddr, Register, AndData, OrData) \
            SMM_HBA_WRITE_REG16(BaseAddr, Register, ((SMM_HBA_REG16 ((BaseAddr), (Register))) & ((UINT16) (AndData)) | ((UINT16) (OrData))))

#define     SMM_HBA_REG32_AND_OR( BaseAddr, Register,AndData,  OrData) \
            SMM_HBA_WRITE_REG32(BaseAddr, Register, ((SMM_HBA_REG32 ((BaseAddr), (Register))) & ((UINT32) (AndData)) | ((UINT32) (OrData))))
  
//Ports
#define     SMM_HBA_PORT_REG8(BaseAddr, Port, Register) \
            (SMM_HBA_REG8 ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port))))

#define     SMM_HBA_PORT_REG16(BaseAddr, Port, Register) \
            (SMM_HBA_REG16 ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port))))

#define     SMM_HBA_PORT_REG32(BaseAddr, Port, Register) \
            (SMM_HBA_REG32 ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port))))

#define     SMM_HBA_PORT_REG64(BaseAddr, Port, Register) \
            ( (Shl64((UINT64)(SMM_HBA_PORT_REG32(BaseAddr, Port, (Register+4))),32)) | \
                ((UINT64)(SMM_HBA_PORT_REG32(BaseAddr, Port, Register))) )

#define     SMM_HBA_PORT_WRITE_REG8(BaseAddr, Port, Register, Data) \
            (SMM_HBA_WRITE_REG8 ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), Data))

#define     SMM_HBA_PORT_WRITE_REG16(BaseAddr, Port, Register, Data) \
            (SMM_HBA_WRITE_REG16 ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)),Data))

#define     SMM_HBA_PORT_WRITE_REG32(BaseAddr, Port, Register,Data) \
            (SMM_HBA_WRITE_REG32 ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)),Data))

#define     SMM_HBA_PORT_WRITE_REG64(BaseAddr, Port, Register,Data) \
             (SMM_HBA_PORT_WRITE_REG32(BaseAddr, Port, (Register+4), ((UINT32)(Shr64(Data,32)) )) ); \
             SMM_HBA_PORT_WRITE_REG32(BaseAddr, Port, Register, ((UINT32)Data))

#define     SMM_HBA_PORT_REG8_OR(BaseAddr, Port, Register, OrData) \
            (SMM_HBA_REG8_OR ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (OrData)))

#define     SMM_HBA_PORT_REG16_OR(BaseAddr, Port, Register, OrData) \
            (SMM_HBA_REG16_OR ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (OrData)))

#define     SMM_HBA_PORT_REG32_OR(BaseAddr, Port, Register, OrData) \
            (SMM_HBA_REG32_OR ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (OrData)))

#define     SMM_HBA_PORT_REG8_AND(BaseAddr, Port, Register, AndData) \
            (SMM_HBA_REG8_AND ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData)))

#define     SMM_HBA_PORT_REG16_AND(BaseAddr, Port, Register, AndData) \
            (SMM_HBA_REG16_AND ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData)))

#define     SMM_HBA_PORT_REG32_AND(BaseAddr, Port, Register, AndData) \
            (SMM_HBA_REG32_AND ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData)))

#define     SMM_HBA_PORT_REG8_AND_OR(BaseAddr, Port, Register, AndData, OrData) \
            (SMM_HBA_REG8_AND_OR ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData), (OrData)))

#define     SMM_HBA_PORT_REG16_AND_OR(BaseAddr, Port, Register, AndData, OrData) \
            (SMM_HBA_REG16_AND_OR ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData), (OrData)))

#define     SMM_HBA_PORT_REG32_AND_OR(BaseAddr, Port, Register, AndData, OrData) \
            (SMM_HBA_REG32_AND_OR ((BaseAddr), ((Register) + HBA_PORT_REG_BASE (Port)), (AndData), (OrData)))

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
