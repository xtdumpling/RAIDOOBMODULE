//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Add NVDIMM vender information
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     28-Jul-2017
//
//****************************************************************************
//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//**********************************************************************
/** @file Smbios.c
    Smbios Driver file.

**/
//**********************************************************************

#include <AmiDxeLib.h>
#include <Token.h>
#include <AmiHobs.h>
#include <TimeStamp.h>
#include <Setup.h>
#include <Protocol/AmiSmbios.h>
#include <Protocol/SmbiosDynamicData.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/DebugLib.h>

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/LegacyRegion2.h>
EFI_LEGACY_REGION2_PROTOCOL         *LegacyRegionProtocol = NULL;
EFI_LEGACY_BIOS_PROTOCOL            *LegacyBiosProtocol = NULL;
LEGACY16_TO_EFI_DATA_TABLE_STRUC    *Legacy16Data;
EFI_STATUS Install16bitPnPSmbiosFunctions();
#endif                                      // CSM_SUPPORT

#if (defined(SmbiosDMIEdit_SUPPORT) && (SmbiosDMIEdit_SUPPORT !=0))
#include "SmbiosDmiEdit/SmbiosDmiEdit.h"
#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
#include <Protocol/SmbiosGetFlashDataProtocol.h>
#endif                                      // SMBIOS_DMIEDIT_DATA_LOC
#endif                                      // SmbiosDMIEdit_SUPPORT

#if defined iAMT_SUPPORT && iAMT_SUPPORT == 1
#define AMT_SMBIOS_GROUP \
    { 0xed27920d, 0x4422, 0x4b4d, { 0xa4, 0xa3, 0x4d, 0xc2, 0xb3, 0xe5, 0x46, 0x3b } }
#endif                                      // iAMT_SUPPORT

// Added for TPM
#define SMBIOS_EFI_TABLE_GROUP \
    { 0xb3dae700, 0x2a77, 0x4ea4, 0xaf, 0x79, 0x32, 0x97, 0xb4, 0x84, 0xbe, 0x61 }

EFI_EVENT   SmbiosPointerEvent;
EFI_GUID    EfiSmbiosPointerGuid  = AMI_SMBIOS_POINTER_GUID;

#if (SMBIOS_STATIC_DATA_SUPPORT == 0) && (SMBIOS_STATIC_DATA_DT_SUPPORT == 0)
// Assumptions when (SMBIOS_STATIC_DATA_SUPPORT == 0) && (SMBIOS_STATIC_DATA_DT_SUPPORT == 0)
#define NUMBER_OF_SYSTEM_CHASSIS    1
#define ELEMENT_COUNT_1             0
#define ELEMENT_LEN_1               0
#define NUMBER_OF_BASEBOARDS        1
#define NUMBER_OF_SYSTEM_CHASSIS    1
#define NUMBER_OF_POWER_SUPPLY      1
#endif                                      // SMBIOS_STATIC_DATA_SUPPORT

extern  EFI_BOOT_SERVICES   *pBS;

#if (defined(SB_WAKEUP_TYPE_FN) && (SB_WAKEUP_TYPE_FN == 1))
extern VOID     NbRuntimeShadowRamWrite(IN BOOLEAN Enable);
extern UINT8    getWakeupTypeForSmbios(VOID);
#endif                                      // SB_WAKEUP_TYPE_FN

#if defined iAMT_SUPPORT && iAMT_SUPPORT == 1
VOID AmtNotifyEventFunction(IN EFI_EVENT Event, IN VOID *Context) {}
#endif                                      // iAMT_SUPPORT

// Added for TPM
VOID NotifyEventFunction(IN EFI_EVENT Event, IN VOID *Context) {}

UINT8       MemType = 0;              // 0 = Not supported
                                      // 1 = DDR2
                                      // 2 = DDR3
                                      // 3 = DDR4
BOOLEAN     SmbiosTableAtE000 = FALSE;
BOOLEAN     VersionUpdated = FALSE;
BOOLEAN     ReleaseDateUpdated = FALSE;
BOOLEAN		BeforeEndofDxe = TRUE;

SETUP_DATA  SetupData;
UINT8       SecureData = 0;

SMBIOS_NVRAM_TYPE4      NvramType4;

#ifdef FLASH_PART_STRING_LENGTH
VOID GetFlashPartInfomation(UINT8 *pBlockAddress, UINT8 *Buffer);
#endif                                      // FLASH_PART_STRING_LENGTH

BOOLEAN
FindStructureType(
    IN OUT UINT8    **Buffer,
    IN OUT UINT8    **StructureFoundPtr,
    IN     UINT8    SearchType,
    IN     UINT8    Instance        // 1-based
);

//
// String type tables
//
STRING_TABLE    StringType_0[] =   {{0x04, 1, 1},
                                    {0x05, 2, 2},
                                    {0x08, 3, 3},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_1[] =   {{0x04, 1, 1},
                                    {0x05, 2, 2},
                                    {0x06, 3, 3},
                                    {0x07, 4, 4},
                                    {0x19, 5, 5},
                                    {0x1a, 6, 6},
                                    {0xff, 0, 0},
                                   };

#if (TYPE2_STRUCTURE == 1)
STRING_TABLE    StringType_2[] =   {{0x04, 1, 1},
                                    {0x05, 2, 2},
                                    {0x06, 3, 3},
                                    {0x07, 4, 4},
                                    {0x08, 5, 5},
                                    {0x0a, 6, 6},
                                    {0xff, 0, 0},
                                   };
#else
STRING_TABLE    StringType_2[] =   {{0xff, 0, 0}};
#endif                                                  // TYPE2_STRUCTURE

#if (TYPE3_STRUCTURE == 1)
STRING_TABLE    StringType_3[NUMBER_OF_SYSTEM_CHASSIS][6] =
                                  {{{0x04, 1, 1},
                                    {0x06, 2, 2},
                                    {0x07, 3, 3},
                                    {0x08, 4, 4},
                                    {0x15 + (ELEMENT_COUNT_1 * ELEMENT_LEN_1), 5, 5},
                                    {0xff, 0, 0},
                                   },
#if NUMBER_OF_SYSTEM_CHASSIS > 1
                                   {
                                    {0x04, 1, 1},
                                    {0x06, 2, 2},
                                    {0x07, 3, 3},
                                    {0x08, 4, 4},
                                    {0x15 + (ELEMENT_COUNT_2 * ELEMENT_LEN_2), 5, 5},
                                    {0xff, 0, 0},
                                   },
#endif                                                  // NUMBER_OF_SYSTEM_CHASSIS
#if NUMBER_OF_SYSTEM_CHASSIS > 2
                                   {
                                    {0x04, 1, 1},
                                    {0x06, 2, 2},
                                    {0x07, 3, 3},
                                    {0x08, 4, 4},
                                    {0x15 + (ELEMENT_COUNT_3 * ELEMENT_LEN_3), 5, 5},
                                    {0xff, 0, 0},
                                   },
#endif                                                  // NUMBER_OF_SYSTEM_CHASSIS
#if NUMBER_OF_SYSTEM_CHASSIS > 3
                                   {
                                    {0x04, 1, 1},
                                    {0x06, 2, 2},
                                    {0x07, 3, 3},
                                    {0x08, 4, 4},
                                    {0x15 + (ELEMENT_COUNT_4 * ELEMENT_LEN_4), 5, 5},
                                    {0xff, 0, 0},
                                   },
#endif                                                  // NUMBER_OF_SYSTEM_CHASSIS
#if NUMBER_OF_SYSTEM_CHASSIS > 4
                                   {
                                    {0x04, 1, 1},
                                    {0x06, 2, 2},
                                    {0x07, 3, 3},
                                    {0x08, 4, 4},
                                    {0x15 + (ELEMENT_COUNT_5 * ELEMENT_LEN_5), 5, 5},
                                    {0xff, 0, 0},
                                   },
#endif                                                  // NUMBER_OF_SYSTEM_CHASSIS
                                  };                    // StringType_3
#else
STRING_TABLE    StringType_3[] =   {{0xff, 0, 0}};
#endif                                                  // TYPE3_STRUCTURE

STRING_TABLE    StringType_4[] =   {{0x04, 1, 1},
                                    {0x07, 2, 2},
                                    {0x10, 3, 3},
                                    {0x20, 4, 4},
                                    {0x21, 5, 5},
                                    {0x22, 6, 6},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_5[] =   {{0xff, 0, 0}};

STRING_TABLE    StringType_6[] =   {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_7[] =   {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_8[] =   {{0x04, 1, 1},
                                    {0x06, 2, 2},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_9[] =   {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_10[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_11[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_12[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_13[] =  {{0x15, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_14[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_15[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_16[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_17[] =  {{0x10, 1, 1},
                                    {0x11, 2, 2},
                                    {0x17, 3, 3},
                                    {0x18, 4, 4},
                                    {0x19, 5, 5},
                                    {0x1a, 6, 6},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_18[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_19[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_20[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_21[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_22[] =  {{0x04, 1, 1},
                                    {0x05, 2, 2},
                                    {0x06, 3, 3},
                                    {0x07, 4, 4},
                                    {0x08, 5, 5},
                                    {0x0e, 6, 6},
                                    {0x14, 7, 7},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_23[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_24[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_25[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_26[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_27[] =  {{0x0e, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_28[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_29[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_30[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_31[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_32[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_33[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_34[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_35[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_36[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_37[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_38[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_39[] =  {{0x05, 1, 1},
                                    {0x06, 2, 2},
                                    {0x07, 3, 3},
                                    {0x08, 4, 4},
                                    {0x09, 5, 5},
                                    {0x0a, 6, 6},
                                    {0x0b, 7, 7},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_40[] =  {{0xff, 0, 0}};

STRING_TABLE    StringType_41[] =  {{0x04, 1, 1},
                                    {0xff, 0, 0},
                                   };

STRING_TABLE    StringType_42[] =  {{0xff, 0, 0}};

//
// String table
//
VOID*    StringTable[] = {&StringType_0,        // 0
                          &StringType_1,        // 1
                          &StringType_2,        // 2
                          &StringType_3,        // 3
                          &StringType_4,        // 4
                          &StringType_5,        // 5
                          &StringType_6,        // 6
                          &StringType_7,        // 7
                          &StringType_8,        // 8
                          &StringType_9,        // 9
                          &StringType_10,       // 10
                          &StringType_11,       // 11
                          &StringType_12,       // 12
                          &StringType_13,       // 13
                          &StringType_14,       // 14
                          &StringType_15,       // 15
                          &StringType_16,       // 16
                          &StringType_17,       // 17
                          &StringType_18,       // 18
                          &StringType_19,       // 19
                          &StringType_20,       // 20
                          &StringType_21,       // 21
                          &StringType_22,       // 22
                          &StringType_23,       // 23
                          &StringType_24,       // 24
                          &StringType_25,       // 25
                          &StringType_26,       // 26
                          &StringType_27,       // 27
                          &StringType_28,       // 28
                          &StringType_29,       // 29
                          &StringType_30,       // 30
                          &StringType_31,       // 31
                          &StringType_32,       // 32
                          &StringType_33,       // 33
                          &StringType_34,       // 34
                          &StringType_35,       // 35
                          &StringType_36,       // 36
                          &StringType_37,       // 37
                          &StringType_38,       // 38
                          &StringType_39,       // 39
                          &StringType_40,       // 40
                          &StringType_41,       // 41
                          &StringType_42,       // 42
                         };

typedef struct _JEDEC_MF_ID {
    UINT8           VendorId;
    CHAR8           *ManufacturerString;
} JEDEC_MF_ID;

JEDEC_MF_ID Bank0Table [] = {{ 0x01, "AMD" },
                             { 0x04, "Fujitsu" },
                             { 0x07, "Hitachi" },
                             { 0x89, "Intel" },
                             { 0x10, "NEC" },
                             { 0x97, "Texas Instrument" },
                             { 0x98, "Toshiba" },
                             { 0x1c, "Mitsubishi" },
                             { 0x1f, "Atmel" },
                             { 0x20, "STMicroelectronics" },
                             { 0xa4, "IBM" },
                             { 0x2c, "Micron Technology" },
                             { 0xad, "SK Hynix" },
                             { 0xb0, "Sharp" },
                             { 0xb3, "IDT" },
                             { 0x3e, "Oracle" },
                             { 0xbf, "SST" },
                             { 0x40, "ProMos/Mosel" },
                             { 0xc1, "Infineon" },
                             { 0xc2, "Macronix" },
                             { 0x45, "SanDisk" },
                             { 0xce, "Samsung" },
                             { 0xda, "Winbond" },
                             { 0xe0, "LG Semi" },
                             { 0x62, "Sanyo" },
                             { 0x34, "Agiga" },
                             { 0xff, "Undefined" }
};
JEDEC_MF_ID Bank1Table [] = {{ 0x98, "Kingston" },
                             { 0xba, "PNY" },
                             { 0x4f, "Transcend" },
                             { 0x7a, "Apacer" },
                             { 0x40, "Viking" },
                             { 0x94, "Smart" },
                             { 0xff, "Undefined" }
};
JEDEC_MF_ID Bank2Table [] = {{ 0x9e, "Corsair" },
                             { 0xfe, "Elpida" },
                             { 0xff, "Undefined" }
};
JEDEC_MF_ID Bank3Table [] = {{ 0x0b, "Nanya" },
                             { 0x94, "Mushkin" },
                             { 0x25, "Kingmax" },
                             { 0x16, "Netlist" },
                             { 0xff, "Undefined" }
};
JEDEC_MF_ID Bank4Table [] = {{ 0xb0, "OCZ" },
                             { 0xcb, "A-DATA" },
                             { 0xcd, "G Skill" },
                             { 0xef, "Team" },
                             { 0xff, "Undefined" }
};
JEDEC_MF_ID Bank5Table [] = {{ 0x02, "Patriot" },
                             { 0x9b, "Crucial" },
                             { 0x51, "Qimonda" },
                             { 0x57, "AENEON" },
                             { 0xf7, "Avant" },
                             { 0xff, "Undefined" }
};
JEDEC_MF_ID Bank6Table [] = {{ 0x34, "Super Talent" },
                             { 0xd3, "Silicon Power" },
                             { 0xF1, "Innodisk" },
                             { 0xff, "Undefined" }
};
JEDEC_MF_ID Bank7Table [] = {{ 0xff, "Undefined" }
};


JEDEC_MF_ID *ManufacturerJedecIdBankTable [] = {
                                                Bank0Table,
                                                Bank1Table,
                                                Bank2Table,
                                                Bank3Table,
                                                Bank4Table,
                                                Bank5Table,
                                                Bank6Table,
                                                Bank7Table
};

typedef struct  {
    UINT8       NumContCode;
    UINT8       ModuleManufacturer;
    UINT8       *Manufacturer;
} JEDEC_DATA;

JEDEC_DATA  ManufacturerTable[] = MANUFACTURER_ID_CODE;

#if SMBIOS_2X_SUPPORT
SMBIOS_TABLE_ENTRY_POINT    SmbiosEntryPointTable = {
            '_', 'S', 'M', '_',
            0,                                              // EPS Checksum
            0x1f,                                           // Entry Point Length
            SMBIOS_2X_MAJOR_VERSION,                        // SMBIOS Major Version
            SMBIOS_2X_MINOR_VERSION,                        // SMBIOS Minor Version
            0x100,                                          // Maximum Structure Size
            0,                                              // Entry Point Revision
            0, 0, 0, 0, 0,                                  // Formatted Area
            '_', 'D', 'M', 'I', '_',                        // Intermediate Anchor String
            0,                                              // Intermediate Checksum
            0,                                              // Structure Table Length
            0,                                              // Structure Table Address
            0x10,                                           // Number of SMBIOS Stuctures
            (SMBIOS_2X_MAJOR_VERSION << 4)|SMBIOS_2X_MINOR_VERSION  // SMBIOS BCD Revision};
            };

SMBIOS_TABLE_ENTRY_POINT        *pSmbiosTableEntryPoint = &SmbiosEntryPointTable;
SMBIOS_TABLE_ENTRY_POINT        *pSmbiosTableEntryPointF000 = NULL;
#endif

#if SMBIOS_3X_SUPPORT
SMBIOS_3X_TABLE_ENTRY_POINT SmbiosV3EntryPointTable = {
            '_', 'S', 'M', '3', '_',
            0,                                              // EPS Checksum
            sizeof(SMBIOS_3X_TABLE_ENTRY_POINT),            // Entry Point Length
            SMBIOS_3X_MAJOR_VERSION,                        // SMBIOS Major Version
            SMBIOS_3X_MINOR_VERSION,                        // SMBIOS Minor Version
            SMBIOS_DOC_VERSION,                             // SMBIOS Docrev
            0x01,                                           // Entry Point Revision 3.0
            0,                                              // Reserved
            0x00000000,                                     // Structure table maximum size
            0x0000000000000000,                             // Structure table address
            };

SMBIOS_3X_TABLE_ENTRY_POINT     *pSmbiosV3TableEntryPoint = &SmbiosV3EntryPointTable;
SMBIOS_3X_TABLE_ENTRY_POINT     *pSmbiosV3TableEntryPointF000 = NULL;
#endif

UINT8                           *SmbiosDataTable = NULL;
UINT8                           *ScratchBufferPtr = NULL;
UINT16                          MaximumTableSize = 0x4000;
UINT16                          LastHandle;
EFI_SMBIOS_BOARD_PROTOCOL       *gSmbiosBoardProtocol = NULL;
BOOLEAN                         UpdateCpuStructure;

#if AMI_SMBIOS_PROTOCOL_ENABLE
AMI_SMBIOS_PROTOCOL   AmiSmbiosProtocol = {
                                        GetSmbiosTableEntryPoint,
                                        GetScratchBufferPtr,
                                        GetBufferMaxSize,
                                        GetFreeHandle,
                                        AddStructure,
                                        AddStructureByHandle,
                                        DeleteStructureByHandle,
                                        ReadStructureByHandle,
                                        ReadStructureByType,
                                        WriteStructureByHandle,
                                        UpdateSmbiosTableHeader,
                                        AddStructureByIndex
                                       };
#endif                                          // AMI_SMBIOS_PROTOCOL_ENABLE

/**
    Returns the checksum of "length" bytes starting from the
    "*ChecksumSrc"

    @param ChecksumSrc  pointer to buffer to be checksumed
    @param length       size of buffer

    @return Checksum value

**/
UINT8
SmbiosCheckSum(
    IN UINT8    *ChecksumSrc,
    IN UINT8    length
)
{
    UINT8       Checksum = 0;
    UINT8       i;

    for (i = 0; i < length; i++) {
        Checksum += *ChecksumSrc++;
    }
    return (0 - Checksum);
}

/**
    Returns the length of the structure pointed by BufferStart
    in bytes

    @param BufferStart  pointer to buffer

    @return Structure size

**/
UINT16
GetStructureLength(
    IN UINT8    *BufferStart
)
{
    UINT8       *BufferEnd = BufferStart;

    BufferEnd += ((SMBIOS_STRUCTURE_HEADER*)BufferStart)->Length;

    while (*(UINT16*)BufferEnd != 0) {
        BufferEnd++;
    }

    return (UINT16)(BufferEnd + 2 - BufferStart);   // +2 for double zero terminator
}

/**
    Returns the instance of the input structure type and its handle

    @param Type     structure type
    @param Handle   structure handle

    @return Instance number (1-based) if found, or 0 if not found

**/
UINT8
GetInstanceByTypeHandle(
    IN UINT8    Type,
    IN UINT16   Handle
)
{
    UINT8   *Table = SmbiosDataTable;
    UINT8   Instance = 0;       // 1-based

    while(((SMBIOS_STRUCTURE_HEADER*)Table)->Type != 127) {
        if (((SMBIOS_STRUCTURE_HEADER*)Table)->Type == Type) {
            Instance ++;
        }

        if (((SMBIOS_STRUCTURE_HEADER*)Table)->Handle == Handle) {
            return Instance;
        }

        Table = Table + GetStructureLength(Table);
    }

    return 0;                   // Not found
}

/**
    Find structure type starting from memory location pointed by
    Buffer

    @param Buffer
    @param StructureFoundPtr
    @param SearchType
    @param Instance


    @retval TRUE Structure found
    @retval FALSE Structure not found

    @remark
        @li If SearchType is found:\n
            UINT8   **Buffer - Points to the next structure\n
            UINT8   **StructureFoundPtr - Points to the structure that was found\n

        @li If SearchType is not found:\n
            UINT8   **Buffer - No change\n
            UINT8   **StructureFoundPtr = NULL

**/
BOOLEAN
FindStructureType(
    IN OUT UINT8    **Buffer,
    IN OUT UINT8    **StructureFoundPtr,
    IN     UINT8    SearchType,
    IN     UINT8    Instance        // 1-based
)
{
    UINT8       *BufferPtr = *Buffer;
    BOOLEAN     FindStatus = FALSE;

    *StructureFoundPtr = NULL;
    while (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type != 127) {
        if (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type == SearchType) {
            // If this instance, set the find status flag and update the Buffer pointer
            if (--Instance == 0) {
                FindStatus = TRUE;
                *StructureFoundPtr = BufferPtr;
                *Buffer = BufferPtr + GetStructureLength(BufferPtr);
                break;
            }
        }
        BufferPtr += GetStructureLength(BufferPtr);
    }
    if ((FindStatus == FALSE) && (SearchType == 127)) {
        FindStatus = TRUE;
        *StructureFoundPtr = BufferPtr;
        *Buffer = BufferPtr + GetStructureLength(BufferPtr);
    }
    return FindStatus;
}

/**
    Find structure handle starting from memory location pointed
    by Buffer

    @param Buffer
    @param Handle


    @retval TRUE Structure found
    @retval FALSE Structure not found

    @remark
        If SearchType is found:\n
        UINT8   **Buffer - Points to the structure that was found

**/
BOOLEAN
FindStructureHandle(
    IN OUT UINT8    **Buffer,
    IN     UINT16   Handle
)
{
    while (((SMBIOS_STRUCTURE_HEADER*)*Buffer)->Handle != Handle) {
        if (((SMBIOS_STRUCTURE_HEADER*)*Buffer)->Type == 127) {
            return FALSE;
        }
        *Buffer += GetStructureLength(*Buffer);
    }
    return TRUE;
}

/**
    Updates Structure Type 127 handle and sets global variable
    LastHandle to the last structure handle.

    @param Buffer

    @return Update global variable LastHandle

**/
VOID
UpdateType127Handle(
    IN UINT8    *Buffer
)
{
    UINT8       *BufferPtr = Buffer;
    UINT16      Handle;

    while (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type != 127) {
        Handle = ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle;
        BufferPtr += GetStructureLength(BufferPtr);
    }
    ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle = ++Handle;
    LastHandle = Handle;
}

/**
    Fix up links among structures

    @param  *Buffer

    @return None

**/
VOID
FixupHandles(
    IN UINT8    *Buffer
)
{
    UINT8               i;
    UINT8               *BufferPtr;
    UINT8               *SrcPtr;
    UINT8               *DestPtr;
    UINT8               LastSrcType = 0;
    UINT8               LastSrcInstance = 0;
    UINT8               LastDestType = 0;
    UINT8               LastDestInstance = 0;
    BOOLEAN             SrcFound;
    BOOLEAN             DestFound;
    BOOLEAN             FindStatus;
    HANDLE_FIXUP_TABLE  *HandleTblPtr;
    HANDLE_FIXUP_TABLE  HandleFixupTable[] = {
#if TYPE2_STRUCTURE && TYPE3_STRUCTURE
                                              {2,  1, 0x0b, 3,  1},
#endif
#if TYPE4_STRUCTURE
 #define SRC_INSTANCE_1    1
 #if (PROC_1_L1_HANDLE == 0xffff)
  #define SRC_INSTANCE_2 SRC_INSTANCE_1
 #else
                                              {4,  1, 0x1a, 7,  SRC_INSTANCE_1},
  #define SRC_INSTANCE_2 (SRC_INSTANCE_1 + 1)
 #endif                                                             // (PROC_1_L1_HANDLE != 0xffff)
 #if (PROC_1_L2_HANDLE == 0xffff)
  #define SRC_INSTANCE_3 SRC_INSTANCE_2
 #else
                                              {4,  1, 0x1c, 7,  SRC_INSTANCE_2},
  #define SRC_INSTANCE_3 (SRC_INSTANCE_2 + 1)
 #endif                                                             // (PROC_1_L2_HANDLE != 0xffff)
 #if (PROC_1_L3_HANDLE == 0xffff)
  #define SRC_INSTANCE_4 SRC_INSTANCE_3
 #else
                                              {4,  1, 0x1e, 7,  SRC_INSTANCE_3},
  #define SRC_INSTANCE_4 (SRC_INSTANCE_3 + 1)
 #endif                                                             // (PROC_1_L3_HANDLE != 0xffff)
#if (NO_OF_PROCESSOR_SOCKETS > 1)
 #if (PROC_2_L1_HANDLE == 0xffff)
  #define SRC_INSTANCE_5 SRC_INSTANCE_4
 #else
                                              {4,  2, 0x1a, 7,  SRC_INSTANCE_4},
  #define SRC_INSTANCE_5 (SRC_INSTANCE_4 + 1)
 #endif                                                             // (PROC_2_L1_HANDLE != 0xffff)
 #if (PROC_2_L2_HANDLE == 0xffff)
  #define SRC_INSTANCE_6 SRC_INSTANCE_5
 #else
                                              {4,  2, 0x1c, 7,  SRC_INSTANCE_5},
  #define SRC_INSTANCE_6 (SRC_INSTANCE_5 + 1)
 #endif                                                             // (PROC_2_L2_HANDLE != 0xffff)
 #if (PROC_2_L3_HANDLE == 0xffff)
  #define SRC_INSTANCE_7 SRC_INSTANCE_6
 #else
                                              {4,  2, 0x1e, 7,  SRC_INSTANCE_6},
  #define SRC_INSTANCE_7 (SRC_INSTANCE_6 + 1)
 #endif                                                             // (PROC_2_L3_HANDLE != 0xffff)
#endif                                                              // (NO_OF_PROCESSOR_SOCKETS > 1)
#if (NO_OF_PROCESSOR_SOCKETS > 2)
 #if (PROC_3_L1_HANDLE == 0xffff)
  #define SRC_INSTANCE_8 SRC_INSTANCE_7
 #else
                                              {4,  3, 0x1a, 7,  SRC_INSTANCE_7},
  #define SRC_INSTANCE_8 (SRC_INSTANCE_7 + 1)
 #endif                                                             // (PROC_3_L1_HANDLE != 0xffff)
 #if (PROC_3_L2_HANDLE == 0xffff)
  #define SRC_INSTANCE_9 SRC_INSTANCE_8
 #else
                                              {4,  3, 0x1c, 7,  SRC_INSTANCE_8},
  #define SRC_INSTANCE_9 (SRC_INSTANCE_8 + 1)
 #endif                                                             // (PROC_3_L2_HANDLE != 0xffff)
 #if (PROC_3_L3_HANDLE == 0xffff)
  #define SRC_INSTANCE_10 SRC_INSTANCE_9
 #else
                                              {4,  3, 0x1e, 7,  SRC_INSTANCE_9},
  #define SRC_INSTANCE_10 (SRC_INSTANCE_9 + 1)
 #endif                                                             // (PROC_3_L3_HANDLE != 0xffff)
#endif                                                              // (NO_OF_PROCESSOR_SOCKETS > 2)
#if (NO_OF_PROCESSOR_SOCKETS > 3)
 #if (PROC_4_L1_HANDLE == 0xffff)
  #define SRC_INSTANCE_11 SRC_INSTANCE_10
 #else
                                              {4,  4, 0x1a, 7,  SRC_INSTANCE_10},
  #define SRC_INSTANCE_11 (SRC_INSTANCE_10 + 1)
 #endif                                                             // (PROC_4_L1_HANDLE != 0xffff)
 #if (PROC_4_L2_HANDLE == 0xffff)
  #define SRC_INSTANCE_12 SRC_INSTANCE_11
 #else
                                              {4,  4, 0x1c, 7,  SRC_INSTANCE_11},
  #define SRC_INSTANCE_12 (SRC_INSTANCE_11 + 1)
 #endif                                                             // (PROC_4_L2_HANDLE != 0xffff)
 #if (PROC_4_L3_HANDLE == 0xffff)
  #define SRC_INSTANCE_13 SRC_INSTANCE_12
 #else
                                              {4,  4, 0x1e, 7,  SRC_INSTANCE_12},
  #define SRC_INSTANCE_13 (SRC_INSTANCE_12 + 1)
 #endif                                                             // (PROC_4_L3_HANDLE != 0xffff)
#endif                                                              // (NO_OF_PROCESSOR_SOCKETS > 3)
#endif                                                              // TYPE4_STRUCTURE
#if TYPE5_STRUCTURE
                                              {5,  1, 0x0f, 6,  1},
#if (NUMBER_OF_MEM_MODULE > 1)
                                              {5,  1, 0x11, 6,  2},
#endif
#if (NUMBER_OF_MEM_MODULE > 2)
                                              {5,  1, 0x13, 6,  3},
#endif
#if (NUMBER_OF_MEM_MODULE > 3)
                                              {5,  1, 0x15, 6,  4},
#endif
#if (NUMBER_OF_MEM_MODULE > 4)
                                              {5,  1, 0x17, 6,  5},
#endif
#if (NUMBER_OF_MEM_MODULE > 5)
                                              {5,  1, 0x19, 6,  6},
#endif
#if (NUMBER_OF_MEM_MODULE > 6)
                                              {5,  1, 0x1b, 6,  7},
#endif
#if (NUMBER_OF_MEM_MODULE > 7)
                                              {5,  1, 0x1d, 6,  8},
#endif
#endif      // TYPE5_STRUCTURE
#if TYPE16_STRUCTURE
// ===== TYPE17_STRUCTURE =====
                                              {17, 1, 0x04, 16, 1},
#if (A1_MEMORY_SOCKETS > 1)
                                              {17, 2, 0x04, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 2)
                                              {17, 3, 0x04, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 3)
                                              {17, 4, 0x04, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 4)
                                              {17, 5, 0x04, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 5)
                                              {17, 6, 0x04, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 6)
                                              {17, 7, 0x04, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 7)
                                              {17, 8, 0x04, 16, 1},
#endif
// ===== TYPE19_STRUCTURE =====
                                              {19, 1, 0x0c, 16, 1},
// ===== TYPE20_STRUCTURE =====
#if TYPE20_STRUCTURE
                                              {20, 1, 0x0c, 16, 1},
                                              {20, 1, 0x0e, 19, 1},
#if (A1_MEMORY_SOCKETS > 1)
                                              {20, 2, 0x0e, 19, 1},
                                              {20, 2, 0x0c, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 2)
                                              {20, 3, 0x0c, 16, 1},
                                              {20, 3, 0x0e, 19, 1},
#endif
#if (A1_MEMORY_SOCKETS > 3)
                                              {20, 4, 0x0e, 19, 1},
                                              {20, 4, 0x0c, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 4)
                                              {20, 5, 0x0c, 16, 1},
                                              {20, 5, 0x0e, 19, 1},
#endif
#if (A1_MEMORY_SOCKETS > 5)
                                              {20, 6, 0x0e, 19, 1},
                                              {20, 6, 0x0c, 16, 1},
#endif
#if (A1_MEMORY_SOCKETS > 6)
                                              {20, 7, 0x0c, 16, 1},
                                              {20, 7, 0x0e, 19, 1},
#endif
#if (A1_MEMORY_SOCKETS > 7)
                                              {20, 8, 0x0e, 19, 1},
                                              {20, 8, 0x0c, 16, 1},
#endif
#endif      // TYPE20_STRUCTURE
#endif      // TYPE16_STRUCTURE
// ===== TYPE26_STRUCTURE =====
#if TYPE26_STRUCTURE
#if (VOLT_PROBE_MGNT_DEV_1 != 0)
                                              {35, VOLT_PROBE_MGNT_DEV_1, 0x07, 26, 1},
#endif      // VOLT_PROBE_MGNT_DEV_1
#if (VOLT_PROBE_SYS_PWR_SUPPLY_1 != 0)
                                              {39, VOLT_PROBE_SYS_PWR_SUPPLY_1, 0x10, 26, 1},
#endif      // VOLT_PROBE_SYS_PWR_SUPPLY_1
#if (NUMBER_OF_VOLTAGE_PROBE > 1)
#if (VOLT_PROBE_MGNT_DEV_2 != 0)
                                              {35, VOLT_PROBE_MGNT_DEV_2, 0x07, 26, 2},
#endif      // VOLT_PROBE_MGNT_DEV_2
#if (VOLT_PROBE_SYS_PWR_SUPPLY_2 != 0)
                                              {39, VOLT_PROBE_SYS_PWR_SUPPLY_2, 0x10, 26, 2},
#endif      // VOLT_PROBE_SYS_PWR_SUPPLY_2
#endif      // (NUMBER_OF_VOLTAGE_PROBE > 1)
#if (NUMBER_OF_VOLTAGE_PROBE > 2)
#if (VOLT_PROBE_MGNT_DEV_3 != 0)
                                              {35, VOLT_PROBE_MGNT_DEV_3, 0x07, 26, 3},
#endif      // VOLT_PROBE_MGNT_DEV_3
#if (VOLT_PROBE_SYS_PWR_SUPPLY_3 != 0)
                                              {39, VOLT_PROBE_SYS_PWR_SUPPLY_3, 0x10, 26, 3},
#endif      // VOLT_PROBE_SYS_PWR_SUPPLY_3
#endif      // (NUMBER_OF_VOLTAGE_PROBE > 2)
#if (NUMBER_OF_VOLTAGE_PROBE > 3)
#if (VOLT_PROBE_MGNT_DEV_4 != 0)
                                              {35, VOLT_PROBE_MGNT_DEV_4, 0x07, 26, 4},
#endif      // VOLT_PROBE_MGNT_DEV_4
#if (VOLT_PROBE_SYS_PWR_SUPPLY_4 != 0)
                                              {39, VOLT_PROBE_SYS_PWR_SUPPLY_4, 0x10, 26, 4},
#endif      // VOLT_PROBE_SYS_PWR_SUPPLY_4
#endif      // (NUMBER_OF_VOLTAGE_PROBE > 3)
#if (NUMBER_OF_VOLTAGE_PROBE > 4)
#if (VOLT_PROBE_MGNT_DEV_5 != 0)
                                              {35, VOLT_PROBE_MGNT_DEV_5, 0x07, 26, 5},
#endif      // VOLT_PROBE_MGNT_DEV_5
#if (VOLT_PROBE_SYS_PWR_SUPPLY_5 != 0)
                                              {39, VOLT_PROBE_SYS_PWR_SUPPLY_5, 0x10, 26, 5},
#endif      // VOLT_PROBE_SYS_PWR_SUPPLY_4
#endif      // (NUMBER_OF_VOLTAGE_PROBE > 4)
#endif      // TYPE26_STRUCTURE
// ===== TYPE27_STRUCTURE =====
#if TYPE27_STRUCTURE
#if (COOLING_MGNT_DEV_1 != 0)
                                            {35, COOLING_MGNT_DEV_1, 0x07, 27, 1},
#endif      // COOLING_MGNT_DEV_1
#if (COOLING_SYS_PWR_SUPPLY_1 != 0)
                                            {39, COOLING_SYS_PWR_SUPPLY_1, 0x12, 27, 1},
#endif      // COOLING_SYS_PWR_SUPPLY_1
#if (NUMBER_OF_COOLING_DEVICE > 1)
#if (COOLING_MGNT_DEV_2 != 0)
                                            {35, COOLING_MGNT_DEV_2, 0x07, 27, 2},
#endif      // COOLING_MGNT_DEV_2
#if (COOLING_SYS_PWR_SUPPLY_2 != 0)
                                            {39, COOLING_SYS_PWR_SUPPLY_2, 0x12, 27, 2},
#endif      // COOLING_SYS_PWR_SUPPLY_2
#endif      // (NUMBER_OF_COOLING_DEVICE > 1)
#if (NUMBER_OF_COOLING_DEVICE > 2)
#if (COOLING_MGNT_DEV_3 != 0)
                                            {35, COOLING_MGNT_DEV_3, 0x07, 27, 3},
#endif      // COOLING_MGNT_DEV_3
#if (COOLING_SYS_PWR_SUPPLY_3 != 0)
                                            {39, COOLING_SYS_PWR_SUPPLY_3, 0x12, 27, 3},
#endif      // COOLING_SYS_PWR_SUPPLY_3
#endif      // (NUMBER_OF_COOLING_DEVICE > 2)
#if (NUMBER_OF_COOLING_DEVICE > 3)
#if (COOLING_MGNT_DEV_4 != 0)
                                            {35, COOLING_MGNT_DEV_4, 0x07, 27, 4},
#endif      // COOLING_MGNT_DEV_4
#if (COOLING_SYS_PWR_SUPPLY_4 != 0)
                                            {39, COOLING_SYS_PWR_SUPPLY_4, 0x12, 27, 4},
#endif      // COOLING_SYS_PWR_SUPPLY_4
#endif      // (NUMBER_OF_COOLING_DEVICE > 3)
#if (NUMBER_OF_COOLING_DEVICE > 4)
#if (COOLING_MGNT_DEV_5 != 0)
                                            {35, COOLING_MGNT_DEV_5, 0x07, 27, 5},
#endif      // COOLING_MGNT_DEV_5
#if (COOLING_SYS_PWR_SUPPLY_5 != 0)
                                            {39, COOLING_SYS_PWR_SUPPLY_5, 0x12, 27, 5},
#endif      // COOLING_SYS_PWR_SUPPLY_5
#endif      // (NUMBER_OF_COOLING_DEVICE > 4)
#endif      // TYPE27_STRUCTURE
// ===== TYPE28_STRUCTURE =====
#if TYPE28_STRUCTURE
#if (TEMPERATURE_PROBE_MGNT_DEV_1 != 0)
                                            {35, TEMPERATURE_PROBE_MGNT_DEV_1, 0x07, 28, 1},
#endif      // TEMPERATURE_PROBE_MGNT_DEV_1
#if (NUMBER_OF_TEMPERATURE_PROBES > 1)
#if (TEMPERATURE_PROBE_MGNT_DEV_2 != 0)
                                            {35, TEMPERATURE_PROBE_MGNT_DEV_2, 0x07, 28, 2},
#endif      // TEMPERATURE_PROBE_MGNT_DEV_2
#endif      // (NUMBER_OF_TEMPERATURE_PROBES > 1)
#if (NUMBER_OF_TEMPERATURE_PROBES > 2)
#if (TEMPERATURE_PROBE_MGNT_DEV_3 != 0)
                                            {35, TEMPERATURE_PROBE_MGNT_DEV_3, 0x07, 28, 3},
#endif      // TEMPERATURE_PROBE_MGNT_DEV_3
#endif      // (NUMBER_OF_TEMPERATURE_PROBES > 2)
#if (NUMBER_OF_TEMPERATURE_PROBES > 3)
#if (TEMPERATURE_PROBE_MGNT_DEV_4 != 0)
                                            {35, TEMPERATURE_PROBE_MGNT_DEV_4, 0x07, 28, 4},
#endif      // TEMPERATURE_PROBE_MGNT_DEV_4
#endif      // (NUMBER_OF_TEMPERATURE_PROBES > 3)
#if (NUMBER_OF_TEMPERATURE_PROBES > 4)
#if (TEMPERATURE_PROBE_MGNT_DEV_5 != 0)
                                            {35, TEMPERATURE_PROBE_MGNT_DEV_5, 0x07, 28, 5},
#endif      // TEMPERATURE_PROBE_MGNT_DEV_5
#endif      // (NUMBER_OF_TEMPERATURE_PROBES > 4)
#endif      // TYPE28_STRUCTURE
// ===== TYPE29_STRUCTURE =====
#if TYPE29_STRUCTURE
#if (ELECTRICAL_PROBE_MGNT_DEV_1 != 0)
                                            {35, ELECTRICAL_PROBE_MGNT_DEV_1, 0x07, 29, 1},
#endif      // ELECTRICAL_PROBE_MGNT_DEV_1
#if (ELECTRICAL_PROBE_SYS_PWR_SUPPLY_1 != 0)
                                            {39, ELECTRICAL_PROBE_SYS_PWR_SUPPLY_1, 0x14, 29, 1},
#endif      // ELECTRICAL_PROBE_SYS_PWR_SUPPLY_1
#if (NUMBER_OF_ELECTRICAL_PROBES > 1)
#if (ELECTRICAL_PROBE_MGNT_DEV_2 != 0)
                                            {35, ELECTRICAL_PROBE_MGNT_DEV_2, 0x07, 29, 2},
#endif      // ELECTRICAL_PROBE_MGNT_DEV_2
#if (ELECTRICAL_PROBE_SYS_PWR_SUPPLY_2 != 0)
                                            {39, ELECTRICAL_PROBE_SYS_PWR_SUPPLY_2, 0x14, 29, 2},
#endif      // ELECTRICAL_PROBE_SYS_PWR_SUPPLY_2
#endif      // (NUMBER_OF_ELECTRICAL_PROBES > 1)
#if (NUMBER_OF_ELECTRICAL_PROBES > 2)
#if (ELECTRICAL_PROBE_MGNT_DEV_3 != 0)
                                            {35, ELECTRICAL_PROBE_MGNT_DEV_3, 0x07, 29, 3},
#endif      // ELECTRICAL_PROBE_MGNT_DEV_3
#if (ELECTRICAL_PROBE_SYS_PWR_SUPPLY_3 != 0)
                                            {39, ELECTRICAL_PROBE_SYS_PWR_SUPPLY_3, 0x14, 29, 3},
#endif      // ELECTRICAL_PROBE_SYS_PWR_SUPPLY_3
#endif      // (NUMBER_OF_ELECTRICAL_PROBES > 2)
#if (NUMBER_OF_ELECTRICAL_PROBES > 3)
#if (ELECTRICAL_PROBE_MGNT_DEV_4 != 0)
                                            {35, ELECTRICAL_PROBE_MGNT_DEV_4, 0x07, 29, 4},
#endif      // ELECTRICAL_PROBE_MGNT_DEV_4
#if (ELECTRICAL_PROBE_SYS_PWR_SUPPLY_4 != 0)
                                            {39, ELECTRICAL_PROBE_SYS_PWR_SUPPLY_4, 0x14, 29, 4},
#endif      // ELECTRICAL_PROBE_SYS_PWR_SUPPLY_4
#endif      // (NUMBER_OF_ELECTRICAL_PROBES > 3)
#if (NUMBER_OF_ELECTRICAL_PROBES > 4)
#if (ELECTRICAL_PROBE_MGNT_DEV_5 != 0)
                                            {35, ELECTRICAL_PROBE_MGNT_DEV_5, 0x07, 29, 5},
#endif      // ELECTRICAL_PROBE_MGNT_DEV_5
#if (ELECTRICAL_PROBE_SYS_PWR_SUPPLY_5 != 0)
                                            {39, ELECTRICAL_PROBE_SYS_PWR_SUPPLY_5, 0x14, 29, 5},
#endif      // ELECTRICAL_PROBE_SYS_PWR_SUPPLY_5
#endif      // (NUMBER_OF_ELECTRICAL_PROBES > 4)
#endif      // TYPE29_STRUCTURE
// ===== TYPE35_STRUCTURE =====
#if TYPE35_STRUCTURE
#if (MGMT_DEV_COMPONENT_MGNT_DEV_1 != 0)
                                            {35, 1, 0x05, 34, MGMT_DEV_COMPONENT_MGNT_DEV_1},
#endif      // MGMT_DEV_COMPONENT_MGNT_DEV_1
#if (NUMBER_OF_MGNT_DEV_COMPONENTS > 1)
#if (MGMT_DEV_COMPONENT_MGNT_DEV_2 != 0)
                                            {35, 2, 0x05, 34, MGMT_DEV_COMPONENT_MGNT_DEV_2},
#endif      // MGMT_DEV_COMPONENT_MGNT_DEV_2
#endif      // (NUMBER_OF_MGNT_DEV_COMPONENTS > 1)
#if (NUMBER_OF_MGNT_DEV_COMPONENTS > 2)
#if (MGMT_DEV_COMPONENT_MGNT_DEV_3 != 0)
                                            {35, 3, 0x05, 34, MGMT_DEV_COMPONENT_MGNT_DEV_3},
#endif      // MGMT_DEV_COMPONENT_MGNT_DEV_3
#endif      // (NUMBER_OF_MGNT_DEV_COMPONENTS > 2)
#if (NUMBER_OF_MGNT_DEV_COMPONENTS > 3)
#if (MGMT_DEV_COMPONENT_MGNT_DEV_4 != 0)
                                            {35, 4, 0x05, 34, MGMT_DEV_COMPONENT_MGNT_DEV_4},
#endif      // MGMT_DEV_COMPONENT_MGNT_DEV_4
#endif      // (NUMBER_OF_MGNT_DEV_COMPONENTS > 3)
#if (NUMBER_OF_MGNT_DEV_COMPONENTS > 4)
#if (MGMT_DEV_COMPONENT_MGNT_DEV_5 != 0)
                                            {35, 5, 0x05, 34, MGMT_DEV_COMPONENT_MGNT_DEV_5},
#endif      // MGMT_DEV_COMPONENT_MGNT_DEV_5
#endif      // (NUMBER_OF_MGNT_DEV_COMPONENTS > 4)
#endif      // TYPE35_STRUCTURE
};

    DEBUG((DEBUG_INFO, "***  SMBIOS - FixupHandles  ***\n"));

    HandleTblPtr = HandleFixupTable;

    for (i = 0; i < (sizeof(HandleFixupTable)/sizeof(HANDLE_FIXUP_TABLE)); i++, HandleTblPtr++) {
        SrcPtr = NULL;
        SrcFound = FALSE;
        if ((HandleTblPtr->SrcStrucType != LastSrcType) || \
            (HandleTblPtr->SrcInstance != LastSrcInstance)) {
            LastSrcType = HandleTblPtr->SrcStrucType;
            LastSrcInstance = HandleTblPtr->SrcInstance;

            BufferPtr = Buffer;
            FindStatus = FindStructureType(&BufferPtr,
                                           &SrcPtr,
                                           HandleTblPtr->SrcStrucType,
                                           HandleTblPtr->SrcInstance
                                          );
            if (FindStatus) {
                SrcFound = TRUE;
            }
        }

        if (!SrcFound) continue;

        DestPtr = NULL;
        DestFound = FALSE;
        if ((HandleTblPtr->DestStrucType != LastDestType) || \
            (HandleTblPtr->DestInstance != LastDestInstance)) {
            LastDestType = HandleTblPtr->DestStrucType;
            LastDestInstance = HandleTblPtr->DestInstance;

            BufferPtr = Buffer;
            FindStatus = FindStructureType(&BufferPtr,
                                           &DestPtr,
                                           HandleTblPtr->DestStrucType,
                                           HandleTblPtr->DestInstance
                                          );
            if (FindStatus) {
                DestFound = TRUE;
            }
        }

        if (!DestFound) continue;

        if ((SrcPtr != NULL) && (DestPtr != NULL)) {
            *(UINT16*)(DestPtr + HandleTblPtr->DestOffset) = ((SMBIOS_STRUCTURE_HEADER*)SrcPtr)->Handle;
        }
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit FixupHandles  :::\n"));
}

/**
    Returns the number of structures starting from Buffer til
    (and including) type 127 structure.

    @param Buffer

    @return Number of structures

**/
UINT16
GetNumberOfStructures(
    IN UINT8    *Buffer
)
{
    UINT8       *BufferPtr = Buffer;
    UINT16      SmbiosStrucCount = 1;

    while (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type != 127) {
        ++SmbiosStrucCount;
        BufferPtr += GetStructureLength(BufferPtr);
    }
    return SmbiosStrucCount;
}

/**
    Returns the largest structure size

    @param Buffer

    @return Largest structure size

**/
UINT16
GetLargestStructureSize(
    IN UINT8    *Buffer
)
{
    UINT8       *BufferPtr = Buffer;
    UINT16      LargestStructureSize = 0;
    UINT16      CurrentStructureSize;

    while (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type != 127) {
        UINT8       *LastBufferPtr;

        LastBufferPtr = BufferPtr;
        BufferPtr += ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Length;
        while(TRUE) {
            if ((*(UINT16*)BufferPtr) == 0) {
                BufferPtr += 2;
                break;
            }
            BufferPtr++;
        }
        CurrentStructureSize = (UINT16)(BufferPtr - LastBufferPtr);
        if (CurrentStructureSize > LargestStructureSize) {
            LargestStructureSize = CurrentStructureSize;
        }
    }
    return LargestStructureSize;
}

/**
    Return pointer to the input type string table

    @param Structure Type

    @return Pointer to the input type string table\n
            (or NULL if not found)

**/
VOID*
GetTypeTable(
    IN UINT8    StructureType
)
{
    if (StructureType < 42) {
        return StringTable[StructureType];
    }
    else {
        return NULL;
    }
}

/**
    Return the string index, assuming all strings exist and they
    sequentially numbered according to Smbios specification for
    the input type structure

    @param Type     structure type
    @param Offset   offset from begining of the structure
    @param Instance 1- based

    @return String index (0-based)\n
            (or 0xff if not found)

**/
UINT8
GetStrIndex(
    IN  UINT8       Type,
    IN  UINT8       Offset,
    IN  UINT8       Instance        // 1- based
)
{
    UINT8           i = 0;
    STRING_TABLE    *StrTablePtr;

    StrTablePtr = GetTypeTable(Type);
    if (StrTablePtr != NULL) {
        StrTablePtr += 6 * (Instance - 1);
        while (StrTablePtr->Offset != 0xff) {
            if (StrTablePtr->Offset == Offset) {
                i = StrTablePtr->SpecStrNum - 1;    // 0-based
                break;
            }

            StrTablePtr++;
        }

        if (StrTablePtr->Offset != 0xff) {
            return i;
        }
    }

    return 0xff;
}

/**
    Return the string number for a structure "Type" at "Offset"

    @param SmbiosTable  pointer to SmbiosTable or Structure
    @param Type         structure type
    @param Offset       offset from begining of the structure

    @return String number (1-based)\n
            (or 0xff if not found)

**/
UINT8
GetStrNumber(
    IN  UINT8       *SmbiosTable,
    IN  UINT8       Type,
    UINT8           Offset
)
{
    UINT8       *NextStructPtr = SmbiosTable;
    UINT8       *TempPtr;

    if (FindStructureType(&NextStructPtr, &TempPtr, Type, 1)) {
        return *(TempPtr + Offset);
    }
    else {
        return 0xff;
    }
}

/**
    Return the largest string number in a structure

    @param StructPtr
    @param StrTablePtr

    @return String number (1-based)\n
            (or 0 if not found)

**/
UINT8
FindLargestStrNumber (
    IN UINT8            *StructPtr,
    IN STRING_TABLE     *StrTablePtr
)
{
    UINT8       Number;
    UINT8       StrNumber = 0;

    // Find largest string number from structure
    while (StrTablePtr->Offset != 0xff) {
        Number = *(StructPtr + StrTablePtr->Offset);
        if (Number > StrNumber) {
            StrNumber = Number;
        }
        StrTablePtr++;
    }

    return StrNumber;       // 1-based, 0 if no string
}

/**
    Add new string number for a structure "Type" at "Offset".
    Return the string index, assuming all strings exist in the
    structure according to the Smbios specification

    @param SmbiosTable  pointer to SmbiosTable or Structure
    @param Type         structure type
    @param Offset       offset from begining of the structure

    @return String index (0-based)\n
            (0xff if not found)

**/
UINT8
AddStringNumber(
    IN  UINT8       *SmbiosTable,
    IN  UINT8       Type,
    UINT8           Offset
)
{
    STRING_TABLE    *StrTablePtr;
    UINT8           *NextStructPtr = SmbiosTable;
    UINT8           *TempPtr;
    UINT8           Index = 0xff;
    UINT8           StrNumber = 0;
    UINT8           Number;

    if (FindStructureType(&NextStructPtr, &TempPtr, Type, 1)) {
        StrTablePtr = GetTypeTable(Type);
        if (StrTablePtr != NULL) {
            // Find largest string number from structure
            while (StrTablePtr->Offset != 0xff) {
                if (StrTablePtr->Offset == Offset) {
                    // String index in Smbios spec
                    Index = StrTablePtr->SpecStrNum - 1;        // 0-based
                }

                Number = *(TempPtr + StrTablePtr->Offset);
                if (Number > StrNumber) {
                    StrNumber = Number;
                }
                StrTablePtr++;
            }

            // Assign next string number to structure at input Offset
            *(TempPtr + Offset) = ++StrNumber;

            return Index;           // 0-based
        }
    }

    return 0xff;
}

/**
    Zero out the string number in StructPtr

    @param StructurePtr
    @param StrNumber

    @retval None

**/
VOID
DeleteStringNumber (
    IN UINT8    *StructPtr,
    IN UINT8    StrNumber
)
{
    UINT8           Number;
    STRING_TABLE    *StrTablePtr;

    StrTablePtr = GetTypeTable(((SMBIOS_STRUCTURE_HEADER*)StructPtr)->Type);

    while (StrTablePtr->Offset != 0xff) {
        Number = *(StructPtr + StrTablePtr->Offset);
        if (Number > StrNumber) {
            *(StructPtr + StrTablePtr->Offset) = Number - 1;
        }
        if (Number == StrNumber) {
            *(StructPtr + StrTablePtr->Offset) = 0;
        }
        StrTablePtr++;
    }
}

/**
    Returns the string offset for StringNumber from BufferStart

    @param BufferStart
    @param StringNumber

    @return offset from BufferStart

**/
UINT16
GetStringOffset(
    IN UINT8    *BufferStart,
    IN UINT8    StringNumber          // 1-based
)
{
    UINT8       *BufferEnd = BufferStart;

    while (--StringNumber) {
        while(*BufferEnd != 0) {
            BufferEnd++;
        }
        BufferEnd++;
    }
    return (UINT16)(BufferEnd - BufferStart);
}

/**
    Returns pointer to the string number in structure BufferPtr

    @param BufferPtr
    @param StringNumber

    @retval TRUE    *BufferPtr = Pointer to the #StringNumber string

**/
BOOLEAN
FindString(
    IN OUT UINT8    **BufferPtr,
    IN     UINT8    StringNumber          // 1-based
)
{
    UINT8       *StructurePtr;
    UINT8       *StructureEnd;

    StructurePtr = *BufferPtr;
    StructureEnd = StructurePtr + GetStructureLength(StructurePtr);
    StructurePtr += ((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Length;
    StructurePtr += GetStringOffset(StructurePtr, StringNumber);
    if (StructurePtr >= StructureEnd) {
        return FALSE;
    }
    else {
        *BufferPtr = StructurePtr;
        return TRUE;
    }
}

/**
    Delete string at Offset

    @param StructPtr
    @param Offset

    @retval None

**/
VOID
DeleteString (
    IN UINT8    *StructPtr,
    IN UINT8    StrNumber
)
{
    UINT8       *TempPtr;
    UINT8       *StructEndPtr;
    UINTN       RemainingSize;

    // Delete string number
    DeleteStringNumber(StructPtr, StrNumber);

    FindString(&StructPtr, StrNumber);              // StructPtr = StrNumber string
    TempPtr = StructPtr + Strlen((char*)StructPtr) + 1;    // Move pointer to next string

    // Find end of structure
    StructEndPtr = TempPtr;
    while(*(UINT16*)StructEndPtr != 0) {
        StructEndPtr++;
    }

    // Copy remaining strings
    RemainingSize = StructEndPtr + 2 - TempPtr;     // Including double NULL characters
    MemCpy(StructPtr, TempPtr, RemainingSize);
}

/**
    Add NULL terminator to the end of the structure

    @param StructPtr
    @param StrTablePtr

    @retval None

**/
VOID
AddNullTerminator (
    IN UINT8            *StructPtr,
    IN STRING_TABLE     *StrTablePtr
)
{
    UINT8       StrNumber;
    UINT8       i;

    // Find largest string number
    StrNumber = FindLargestStrNumber(StructPtr, StrTablePtr);

    // Skip to string section
    StructPtr += ((SMBIOS_STRUCTURE_HEADER*)StructPtr)->Length;

    // Move pointer to the end of last string
    for (i = 0; i < StrNumber; i++) {
        while (*StructPtr != 0) StructPtr++;
        StructPtr++;
    }

    // Add NULL terminator
    *StructPtr = 0;
}

/**
    Copy strings from NvramData to StructPtr

    @param NvramData
    @param StructPtr
    @param StrTableInstance // 1-based

    @return None\n
            Strings updated in StructPtr

**/
VOID
UpdateStrings(
    IN     SMBIOS_NVRAM_TYPE    *NvramData,
    IN OUT UINT8                *StructPtr,
    IN     UINT8                StrTableInstance    // 1-based
)
{
    UINT8               TestBit;
    UINT8               i;
    UINT8               Type;
    UINT8               StrNumber;

    if (NvramData->Flag != 0) {
        STRING_TABLE        *StrTablePtr;

        // Set StringTablePtr pointer to the appropriate string table
        StrTablePtr = GetTypeTable(((SMBIOS_STRUCTURE_HEADER*)StructPtr)->Type);
        if (((SMBIOS_STRUCTURE_HEADER*)StructPtr)->Type == 3) {
            StrTablePtr += 6 * (StrTableInstance - 1);
        }

        if (StrTablePtr != NULL)  {
            // Update String fields
            for (i = 0; StrTablePtr[i].Offset != 0xff; i++) {
                TestBit = (1 << i);
                if (NvramData->Flag & TestBit) {
                    CHAR8               *String;

                    // Structure string has been changed, update it
                    Type = ((SMBIOS_STRUCTURE_HEADER*)StructPtr)->Type;
                    StrNumber = GetStrNumber(StructPtr, Type, StrTablePtr[i].Offset);
                    String = NvramData->StringSet[i];
                    if (Strlen(String) != 0) {
                        if (StrNumber == 0) {
                            AddStringNumber(StructPtr, Type, StrTablePtr[i].Offset);
                            StrNumber = GetStrNumber(StructPtr, Type, StrTablePtr[i].Offset);
                        }
                        ReplaceString(StructPtr, StrNumber, (UINT8*)String);    // StrNumber 1-based
                    }
                    else {
                        DeleteString(StructPtr, StrNumber);
                    }
                }
            }

            // Add structure terminator Null byte
            AddNullTerminator(StructPtr, StrTablePtr);
        }
    }
}

/**
    Copy Type 0 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType0(
    IN     SMBIOS_NVRAM_TYPE0   *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16      BlockSize;

    // Copy structure data from SrcBuffer
    BlockSize = GetStructureLength(SrcBuffer);
    MemCpy(TempBuffer, SrcBuffer, BlockSize);

    if (NvramData != NULL) {
        UpdateStrings((SMBIOS_NVRAM_TYPE*)NvramData, TempBuffer, 1);
    }
}

/**
    Copy Type 1 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType1(
    IN     SMBIOS_NVRAM_TYPE1   *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16              BlockSize;
    UINT16              i;
    SMBIOS_SYSTEM_INFO  *Ptr;
    UINT8               *NvramUuidPtr;

    // Copy structure data from SrcBuffer
    BlockSize = GetStructureLength(SrcBuffer);
    MemCpy(TempBuffer, SrcBuffer, BlockSize);

    if (NvramData != NULL) {
        // UUID field - Offset 8 (Test Bit16)
        if (NvramData->Flag & 0x00010000) {
            UINT8               *UuidPtr;

            Ptr = (SMBIOS_SYSTEM_INFO*)TempBuffer;
            UuidPtr = (UINT8*)&Ptr->Uuid;
            NvramUuidPtr = (UINT8*)&NvramData->Uuid;
            for (i = 0; i < sizeof(EFI_GUID); i++) {
                UuidPtr[i] = NvramUuidPtr[i];
            }
        }

        UpdateStrings((SMBIOS_NVRAM_TYPE*)NvramData, TempBuffer, 1);
    }
}

#if BASE_BOARD_INFO
/**
    Copy Type 2 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType2(
    IN     SMBIOS_NVRAM_TYPE2   *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16      BlockSize;

    // Copy structure data from SrcBuffer
    BlockSize = GetStructureLength(SrcBuffer);
    MemCpy(TempBuffer, SrcBuffer, BlockSize);

    if (NvramData != NULL) {
        UpdateStrings((SMBIOS_NVRAM_TYPE*)NvramData, TempBuffer, 1);
    }
}
#endif

#if SYS_CHASSIS_INFO
/**
    Copy Type 3 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer
    @param StringTableInstance

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType3(
    IN     SMBIOS_NVRAM_TYPE3   *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer,
    IN     UINT8                StrTableInstance    // 1-based
)
{
    UINT16      BlockSize;

    // Copy structure data from SrcBuffer
    BlockSize = GetStructureLength(SrcBuffer);
    MemCpy(TempBuffer, SrcBuffer, BlockSize);

    if (NvramData != NULL) {
        // Type field - Offset 5 (Test Bit16)
        if (NvramData->Flag & 0x00010000) {
            ((SMBIOS_SYSTEM_ENCLOSURE_INFO*)TempBuffer)->Type = NvramData->Type;
        }

        // OEM-Defined field - Offset 0x0D (Test Bit17)
        if (NvramData->Flag & 0x00020000) {
            ((SMBIOS_SYSTEM_ENCLOSURE_INFO*)TempBuffer)->OemDefined = NvramData->OemDefined;
        }

        // Number of Power Cords field - Offset 0x12 (Test Bit18)
        if (NvramData->Flag & 0x00040000) {
            ((SMBIOS_SYSTEM_ENCLOSURE_INFO*)TempBuffer)->NumberOfPowerCord = NvramData->NumberOfPowerCord;
        }

        UpdateStrings((SMBIOS_NVRAM_TYPE*)NvramData, TempBuffer, StrTableInstance);
    }
}
#endif

#if ( defined(PROCESSOR_DMIEDIT_SUPPORT) && (PROCESSOR_DMIEDIT_SUPPORT == 1) )
/**
    Copy Type 4 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType4(
    IN     SMBIOS_NVRAM_TYPE4   *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16      BlockSize;

    // Copy structure data from SrcBuffer
    BlockSize = GetStructureLength(SrcBuffer);
    MemCpy(TempBuffer, SrcBuffer, BlockSize);

    if (NvramData != NULL) {
        UpdateStrings((SMBIOS_NVRAM_TYPE*)NvramData, TempBuffer, 1);
    }
}
#endif

#if OEM_STRING_INFO
/**
    Copy Type 11 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType11(
    IN     SMBIOS_NVRAM_TYPE11  *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16      StringSize;
    UINT16      NvramStringSize;
    UINT16      BlockSize;
    UINT32      TestBit;
    UINT8       i;
    UINT8       Count;

    if (NvramData->Flag == 0) {
        BlockSize = GetStructureLength(SrcBuffer);
        MemCpy(TempBuffer, SrcBuffer, BlockSize);
    }
    else {
        // Copy structure data (without string data)
        BlockSize = ((SMBIOS_STRUCTURE_HEADER*)SrcBuffer)->Length;
        MemCpy(TempBuffer, SrcBuffer, BlockSize);

        Count = ((SMBIOS_OEM_STRINGS_INFO*)TempBuffer)->Count;

        // Move pointers
        SrcBuffer += BlockSize;
        TempBuffer += BlockSize;

        // "NUMBER_OF_OEM_STRINGS" string fields
        for (i = 0; i < Count; i++) {
            StringSize = (UINT16)(Strlen((char*)SrcBuffer) + 1);       // Size including string NULL terminator
            TestBit = (1 << i);
            if (NvramData->Flag & TestBit) {
                NvramStringSize = (UINT16)(Strlen(NvramData->StringSet[i]) + 1);
                MemCpy(TempBuffer, NvramData->StringSet[i], NvramStringSize);
                TempBuffer += NvramStringSize;
            }
            else {
                MemCpy(TempBuffer, SrcBuffer, StringSize);
                TempBuffer += StringSize;
            }
            SrcBuffer += StringSize;
        }

        // Add NULL byte for end of string-set
        *TempBuffer = 0;
    }
}
#endif

#if SYSTEM_CONFIG_OPTION_INFO
/**
    Copy Type 12 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType12(
    IN     SMBIOS_NVRAM_TYPE12  *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16      StringSize;
    UINT16      NvramStringSize;
    UINT16      BlockSize;
    UINT8       TestBit;
    UINT8       i;
    UINT8       Count;

    if (NvramData->Flag == 0) {
        BlockSize = GetStructureLength(SrcBuffer);
        MemCpy(TempBuffer, SrcBuffer, BlockSize);
    }
    else {
        // Copy structure data (without string data)
        BlockSize = ((SMBIOS_STRUCTURE_HEADER*)SrcBuffer)->Length;
        MemCpy(TempBuffer, SrcBuffer, BlockSize);

        Count = ((SMBIOS_SYSTEM_CONFIG_INFO*)TempBuffer)->Count;

        // Move pointers
        SrcBuffer += BlockSize;
        TempBuffer += BlockSize;

        // "NUMBER_OF_SYSTEM_CONFIG_STRINGS" string fields
        for (i = 0; i < Count; i++) {
            StringSize = (UINT16)(Strlen((char*)SrcBuffer) + 1);       // Size including string NULL terminator
            TestBit = (1 << i);
            if (NvramData->Flag & TestBit) {
                NvramStringSize = (UINT16)(Strlen(NvramData->StringSet[i]) + 1);
                MemCpy(TempBuffer, NvramData->StringSet[i], NvramStringSize);
                TempBuffer += NvramStringSize;
            }
            else {
                MemCpy(TempBuffer, SrcBuffer, StringSize);
                TempBuffer += StringSize;
            }
            SrcBuffer += StringSize;
        }

        // Add NULL byte for end of string-set
        *TempBuffer = 0;
    }
}
#endif

#if PORTABLE_BATTERY_INFO
/**
    Copy Type 22 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType22(
    IN     SMBIOS_NVRAM_TYPE22  *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16      BlockSize;

    // Copy structure data from SrcBuffer
    BlockSize = GetStructureLength(SrcBuffer);
    MemCpy(TempBuffer, SrcBuffer, BlockSize);

    if (NvramData != NULL) {
        // Type field - Offset 9 (Test Bit16)
        if (NvramData->Flag & 0x00010000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->DeviceChemistry = NvramData->DeviceChemistry;
        }

        // OEM-Defined field - Offset 0x0a (Test Bit17)
        if (NvramData->Flag & 0x00020000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->DesignCapacity = NvramData->DesignCapacity;
        }

        // OEM-Defined field - Offset 0x0c (Test Bit18)
        if (NvramData->Flag & 0x00040000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->DesignVoltage = NvramData->DesignVoltage;
        }

        // OEM-Defined field - Offset 0x0f (Test Bit19)
        if (NvramData->Flag & 0x00080000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->MaxErrorInBatteryData = NvramData->MaxErrorInBatteryData;
        }

        // OEM-Defined field - Offset 0x10 (Test Bit20)
        if (NvramData->Flag & 0x00100000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->SBDSSerialNumber = NvramData->SbdsSerialNumber;
        }

        // OEM-Defined field - Offset 0x12 (Test Bit21)
        if (NvramData->Flag & 0x00200000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->SBDSManufacturerDate = NvramData->SbdsManufacturerDate;
        }

        // OEM-Defined field - Offset 0x15 (Test Bit22)
        if (NvramData->Flag & 0x00400000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->DesignCapabilityMult = NvramData->DesignCapacityMultiplier;
        }

        // OEM-Defined field - Offset 0x16 (Test Bit23)
        if (NvramData->Flag & 0x00800000) {
            ((SMBIOS_PORTABLE_BATTERY_INFO*)TempBuffer)->OEMSpecific = NvramData->OemSpecific;
        }

        UpdateStrings((SMBIOS_NVRAM_TYPE*)NvramData, TempBuffer, 1);
    }
}
#endif

#if SYSTEM_POWER_SUPPLY_INFO
/**
    Copy Type 39 strings to TempBuffer. Strings will be copied
    from NVRAM if exist, or else from existing strings in ROM image.
    SrcBuffer and TempBuffer pointers are updated

    @param NvramData
    @param SrcBuffer
    @param TempBuffer

    @return None \n
            Updated SrcBuffer and TempBuffer

**/
VOID
UpdateStructuresWithNvramType39(
    IN     SMBIOS_NVRAM_TYPE39  *NvramData,
    IN OUT UINT8                *SrcBuffer,
    IN OUT UINT8                *TempBuffer
)
{
    UINT16      BlockSize;

    // Copy structure data from SrcBuffer
    BlockSize = GetStructureLength(SrcBuffer);
    MemCpy(TempBuffer, SrcBuffer, BlockSize);

    if (NvramData != NULL) {
        // Type field - Offset 4 (Test Bit16)
        if (NvramData->Flag & 0x00010000) {
            ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)TempBuffer)->PwrUnitGroup = NvramData->PwrUnitGroup;
        }

        // OEM-Defined field - Offset 0x0c (Test Bit17)
        if (NvramData->Flag & 0x00020000) {
            ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)TempBuffer)->MaxPwrCapacity = NvramData->MaxPwrCapacity;
        }

        // OEM-Defined field - Offset 0x0e (Test Bit18)
        if (NvramData->Flag & 0x00040000) {
            ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)TempBuffer)->PwrSupplyChar = NvramData->PwrSupplyChar;
        }

        // OEM-Defined field - Offset 0x10 (Test Bit19)
        if (NvramData->Flag & 0x00080000) {
            ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)TempBuffer)->InputVoltProbeHandle = NvramData->InputVoltageProbeHandle;
        }

        // OEM-Defined field - Offset 0x12 (Test Bit20)
        if (NvramData->Flag & 0x00100000) {
            ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)TempBuffer)->CoolingDevHandle = NvramData->CoolingDevHandle;
        }

        // OEM-Defined field - Offset 0x14 (Test Bit21)
        if (NvramData->Flag & 0x00200000) {
            ((SMBIOS_SYSTEM_PWR_SUPPY_INFO*)TempBuffer)->InputCurrentProbeHandle = NvramData->InputCurrentProbeHandle;
        }

        UpdateStrings((SMBIOS_NVRAM_TYPE*)NvramData, TempBuffer, 1);
    }
}
#endif

/**
    Returns the string index for Type 22 structure from a given
    Offset.

    @param Offset

    @return Type 22 Structure String index

**/
UINT8
GetType22StringIndex(
    IN UINT8       Offset
)
{
    switch (Offset) {
        case 0x0e: return 5;
        case 0x14: return 6;
    }
    return (Offset - 4);
}

/**
    Returns the string index for Type 39 structure from a given
    Offset.

    @param Offset

    @return Type 39 Structure String index

**/
UINT8
GetType39StringIndex(
    IN UINT8       Offset
)
{
    return (Offset - 5);
}

/**
    Locates the input specified Guid file in the Firmware Volumn
    and loads it into the input Buffer

    @param Guid File GUID to read
    @param SectionType
    @param Buffer
    @param BufferSize

    @return EFI_STATUS \n
            **Buffer - Contains content of the Guid file\n
            *BufferSize - Size of the output buffer

**/
EFI_STATUS
LoadRealModeFileSection(
    IN      EFI_GUID                *Guid,
    IN      EFI_SECTION_TYPE        SectionType,
    IN OUT  VOID                    **Buffer,
    IN OUT  UINTN                   *BufferSize
)
{
    EFI_STATUS                    Status;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
    UINT32                        Authentication;
    UINTN                         NumHandles;
    EFI_HANDLE                    *HandleBuffer;
    UINTN                         i;

    DEBUG((DEBUG_INFO, ":::  SMBIOS - LoadRealModeFileSection  :::\n"));

    Status = pBS->LocateHandleBuffer(ByProtocol,&gEfiFirmwareVolume2ProtocolGuid,NULL,&NumHandles,&HandleBuffer);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "***  Failed to locate handle buffer EFI_FIRMWARE_VOLUME2_PROTOCOL  ***\n"));
        return Status;
    }

    for (i = 0; i< NumHandles; ++i) {
        Status = pBS->HandleProtocol(HandleBuffer[i],&guidFV,(void**)&Fv);
        if (EFI_ERROR(Status)) continue;

        Status = Fv->ReadSection(Fv,
                                Guid,
                                SectionType,
                                0,        //Instance
                                Buffer,
                                BufferSize,
                                &Authentication);

        if (Status == EFI_SUCCESS) break;
    }

    pBS->FreePool(HandleBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit LoadRealModeFileSection. Status = %r  :::\n", Status));

    return Status;
}

#if (defined(SmbiosDMIEdit_SUPPORT) && (SmbiosDMIEdit_SUPPORT != 0))
/**
    Updates structures in input Buffer with DMI Data in NVRAM

    @param Buffer
    @param BufferSize

    @return EFI_STATUS

**/
EFI_STATUS
UpdateStructuresWithNvramData(
    IN  UINT8     *Buffer,
    IN  UINTN     BufferSize
)
{
    EFI_STATUS              Status;
    UINT16                  StructureSize;
    UINT8                   *BufferPtr = Buffer;
    UINT8                   *TempBuffer;
    UINT8                   *TempBufferPtr;
    UINTN                   Index;
    UINTN                   i;
    BOOLEAN                 UpdateFlag = FALSE; // Flag indicating if any structure has been updated

#if (TYPE2_STRUCTURE == 1)
    UINT16                  *Type2Handle;
#endif                                          // TYPE2_STRUCTURE
#if (TYPE3_STRUCTURE == 1)
    UINT16                  *Type3Handle;
    UINT8                   StrTableInstance;
#endif                                          // TYPE3_STRUCTURE
    UINT16                  *Type22Handle;
    UINT16                  *Type39Handle;

    UINT8                   *DmiData = NULL;
    UINTN                   DmiDataSize;

    SMBIOS_NVRAM_TYPE0      NvramType0;
    SMBIOS_NVRAM_TYPE1      NvramType1;
    SMBIOS_NVRAM_TYPE2      *NvramType2 = NULL;
    SMBIOS_NVRAM_TYPE3      *NvramType3 = NULL;
    SMBIOS_NVRAM_TYPE11     NvramType11;
    SMBIOS_NVRAM_TYPE12     NvramType12;
    SMBIOS_NVRAM_TYPE22     *NvramType22 = NULL;
    SMBIOS_NVRAM_TYPE39     *NvramType39 = NULL;

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
    EFI_GUID                FlashDataFile = {0xfd44820b, 0xf1ab, 0x41c0, 0xae, 0x4e, 0x0c, 0x55, 0x55, 0x6e, 0xb9, 0xbd};
    UINT8                   *FlashData = NULL;
    UINTN                   FlashDataSize = 0;
    UINT8                   *FlashDataPtr;
    UINT8                   *FlashDataEnd;
#else
    UINT8                   *NvramDataPtrArray[DMI_ARRAY_COUNT];
#endif                                      // SMBIOS_DMIEDIT_DATA_LOC

    DEBUG((DEBUG_INFO, "***  SMBIOS - UpdateStructuresWithNvramData  ***\n"));

    MemSet(&NvramType0, sizeof(SMBIOS_NVRAM_TYPE0), 0);
    MemSet(&NvramType1, sizeof(SMBIOS_NVRAM_TYPE1), 0);
#if (TYPE2_STRUCTURE == 1)
    pBS->AllocatePool(EfiBootServicesData, NUMBER_OF_BASEBOARDS * sizeof(UINT16), (void**)&Type2Handle);
    pBS->AllocatePool(EfiBootServicesData, NUMBER_OF_BASEBOARDS * sizeof(SMBIOS_NVRAM_TYPE2), (void**)&NvramType2);
    for (i = 0; i < NUMBER_OF_BASEBOARDS; i++) {
        Type2Handle[i] = 0;
        NvramType2[i].Flag = 0;
    }
#endif                                      // TYPE2_STRUCTURE
#if (TYPE3_STRUCTURE == 1)
    pBS->AllocatePool(EfiBootServicesData, NUMBER_OF_SYSTEM_CHASSIS * sizeof(UINT16), (void**)&Type3Handle);
    pBS->AllocatePool(EfiBootServicesData, NUMBER_OF_SYSTEM_CHASSIS * sizeof(SMBIOS_NVRAM_TYPE3), (void**)&NvramType3);
    for (i = 0; i < NUMBER_OF_SYSTEM_CHASSIS; i++) {
        Type3Handle[i] = 0;
        NvramType3[i].Flag = 0;
    }
#endif                                      // TYPE3_STRUCTURE
    MemSet(&NvramType4, sizeof(SMBIOS_NVRAM_TYPE4), 0);
    MemSet(&NvramType11, sizeof(SMBIOS_NVRAM_TYPE11), 0);
    MemSet(&NvramType12, sizeof(SMBIOS_NVRAM_TYPE12), 0);
    pBS->AllocatePool(EfiBootServicesData, gSmbiosBoardProtocol->NumberOfBatteries * sizeof(UINT16), (void**)&Type22Handle);
    pBS->AllocatePool(EfiBootServicesData, gSmbiosBoardProtocol->NumberOfBatteries * sizeof(SMBIOS_NVRAM_TYPE22), (void**)&NvramType22);
    for (i = 0; i < gSmbiosBoardProtocol->NumberOfBatteries; i++) {
        Type22Handle[i] = 0;
        NvramType22[i].Flag = 0;
    }
    pBS->AllocatePool(EfiBootServicesData, NUMBER_OF_POWER_SUPPLY * sizeof(UINT16), (void**)&Type39Handle);
    pBS->AllocatePool(EfiBootServicesData, NUMBER_OF_POWER_SUPPLY * sizeof(SMBIOS_NVRAM_TYPE39), (void**)&NvramType39);
    for (i = 0; i < NUMBER_OF_POWER_SUPPLY; i++) {
        Type39Handle[i] = 0;
        NvramType39[i].Flag = 0;
    }

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
    // Get SMBios NVRam Data Structure Image
    Status = LoadRealModeFileSection(&FlashDataFile, EFI_SECTION_RAW, (void**)&FlashData, &FlashDataSize);
    if (EFI_ERROR(Status)) goto Function_Exit;

    FlashDataPtr = FlashData;
    FlashDataEnd = FlashData + FLASHDATA_SIZE;

    // Skip _ASB signature and _FlashDataSize
    FlashDataPtr += 8;

    while ((FlashDataPtr < FlashDataEnd) && (*(UINT32*)FlashDataPtr != 0xffffffff)) {
        if (((TABLE_INFO*)FlashDataPtr)->Flags & DMIEDIT_DELETE_STRUC) {
            DEBUG((DEBUG_INFO, "Delete structure. Handle = %x\n", ((TABLE_INFO*)FlashDataPtr)->Handle));
            DeleteStructureByHandle(((TABLE_INFO*)FlashDataPtr)->Handle);
            UpdateFlag = TRUE;
        }
        else if (((TABLE_INFO*)FlashDataPtr)->Flags & DMIEDIT_ADD_STRUC) {
            DEBUG((DEBUG_INFO, "Add structure by handle. Handle = %x\n", ((TABLE_INFO*)FlashDataPtr)->Handle));
            TempBufferPtr = FlashDataPtr + sizeof (TABLE_INFO);
            AddStructureByHandle(((TABLE_INFO*)FlashDataPtr)->Handle, TempBufferPtr, ((TABLE_INFO*)FlashDataPtr)->Size);
            UpdateFlag = TRUE;
        }
        else {
            DEBUG((DEBUG_INFO, "Change structure. Type = %x, Handle = %x, Offset = %x\n",\
                                          ((TABLE_INFO*)FlashDataPtr)->Type,\
                                          ((TABLE_INFO*)FlashDataPtr)->Handle,\
                                          ((TABLE_INFO*)FlashDataPtr)->Offset));
            DmiDataSize = ((TABLE_INFO*)FlashDataPtr)->Size;
            Status = pBS->AllocatePool(
                                EfiBootServicesData,
                                DmiDataSize,
                                (void**)&DmiData);
            if (Status == EFI_SUCCESS) {
                MemCpy(DmiData,
                       FlashDataPtr + sizeof(TABLE_INFO),
                       DmiDataSize);

                switch (((TABLE_INFO*)FlashDataPtr)->Type) {
                    case 0  :   {
                                    // Get string index (Smbios spec) at Offset
                                    Index = GetStrIndex(0, ((TABLE_INFO*)FlashDataPtr)->Offset, 1);
                                    if (Index != 0xff) {
                                        NvramType0.StringSet[Index] = (char*)DmiData;
                                        NvramType0.Flag |= (1 << Index);
                                        UpdateFlag = TRUE;
                                    }
                                    if (((TABLE_INFO*)FlashDataPtr)->Offset == 5) {
                                        VersionUpdated = TRUE;
                                    }
                                    if (((TABLE_INFO*)FlashDataPtr)->Offset == 8) {
                                        ReleaseDateUpdated = TRUE;
                                    }
                                    break;
                                }
                    case 1  :   {
                                    if (((TABLE_INFO*)FlashDataPtr)->Offset == 8) {
                                        MemCpy(&NvramType1.Uuid, FlashDataPtr + sizeof(TABLE_INFO), ((TABLE_INFO*)FlashDataPtr)->Size);
                                        NvramType1.Flag |= 0x00010000;
                                        UpdateFlag = TRUE;
                                    }
                                    else {
                                        // Get string index (Smbios spec) at Offset
                                        Index = GetStrIndex(1, ((TABLE_INFO*)FlashDataPtr)->Offset, 1);
                                        if (Index != 0xff) {
                                            NvramType1.StringSet[Index] = (char*)DmiData;
                                            NvramType1.Flag |= (1 << Index);
                                            UpdateFlag = TRUE;
                                        }
                                    }
                                    break;
                                }
                    #if TYPE2_STRUCTURE
                    case 2  :   {
                                    if (gSmbiosBoardProtocol->BaseBoardInfoSupport) {
                                        // Save handle number in Type2Handle array for each Type 2 structure
                                        for (i = 0; i < NUMBER_OF_BASEBOARDS; i++) {
                                            if (Type2Handle[i] == 0) {
                                                Type2Handle[i] = ((TABLE_INFO*)FlashDataPtr)->Handle;
                                                break;
                                            }
                                            if (((TABLE_INFO*)FlashDataPtr)->Handle == Type2Handle[i]) break;
                                        }
                                        if (i < NUMBER_OF_BASEBOARDS) {
                                            NvramType2[i].Handle = ((TABLE_INFO*)FlashDataPtr)->Handle;

                                            // Get string index (Smbios spec) at Offset
                                            Index = GetStrIndex(2, ((TABLE_INFO*)FlashDataPtr)->Offset, 1);
                                            if (Index != 0xff) {
                                                NvramType2[i].StringSet[Index] = (char*)DmiData;
                                                NvramType2[i].Flag |= (1 << Index);
                                                UpdateFlag = TRUE;
                                            }
                                        }
                                    }
                                    break;
                                }
                    #endif                  // BASE_BOARD_INFO
                    #if TYPE3_STRUCTURE
                    case 3  :   {
                                    StrTableInstance = GetInstanceByTypeHandle(3, ((TABLE_INFO*)FlashDataPtr)->Handle);

                                    if (gSmbiosBoardProtocol->SysChassisInfoSupport) {
                                        // Save handle number in Type3Handle array for each Type 3 structure
                                        for (i = 0; i < NUMBER_OF_SYSTEM_CHASSIS; i++) {
                                            if (Type3Handle[i] == 0) {
                                                Type3Handle[i] = ((TABLE_INFO*)FlashDataPtr)->Handle;
                                                break;
                                            }
                                            if (((TABLE_INFO*)FlashDataPtr)->Handle == Type3Handle[i]) break;
                                        }
                                        if (i < NUMBER_OF_SYSTEM_CHASSIS) {
                                            NvramType3[i].Handle = ((TABLE_INFO*)FlashDataPtr)->Handle;
                                            if (((TABLE_INFO*)FlashDataPtr)->Offset == 5) {
                                                MemCpy(&NvramType3[i].Type, FlashDataPtr + sizeof(TABLE_INFO), 1);
                                                NvramType3[i].Flag |= 0x00010000;
                                                UpdateFlag = TRUE;
                                            }
                                            else if (((TABLE_INFO*)FlashDataPtr)->Offset == 0x0d) {
                                                MemCpy(&NvramType3[i].OemDefined, FlashDataPtr + sizeof(TABLE_INFO), sizeof(UINT32));
                                                NvramType3[i].Flag |= 0x00020000;
                                                UpdateFlag = TRUE;
                                            }
                                            if (((TABLE_INFO*)FlashDataPtr)->Offset == 0x12) {
                                                MemCpy(&NvramType3[i].NumberOfPowerCord, FlashDataPtr + sizeof(TABLE_INFO), 1);
                                                NvramType3[i].Flag |= 0x00040000;
                                                UpdateFlag = TRUE;
                                            }
                                            else {
                                                // Get string index (Smbios spec) at Offset
                                                Index = GetStrIndex(3, ((TABLE_INFO*)FlashDataPtr)->Offset, StrTableInstance);
                                                if (Index != 0xff) {
                                                    NvramType3[i].StringSet[Index] = (char*)DmiData;
                                                    NvramType3[i].Flag |= (1 << Index);
                                                    UpdateFlag = TRUE;
                                                }
                                            }
                                        }
                                    }
                                    break;
                                }
                    #endif                  // SYS_CHASSIS_INFO
                    #if PROCESSOR_DMIEDIT_SUPPORT
                    case 4  :   {
                                    if (gSmbiosBoardProtocol->ProcessorDmiEditSupport) {
                                        // Get string index (Smbios spec) at Offset
                                        Index = GetStrIndex(4, ((TABLE_INFO*)FlashDataPtr)->Offset, 1);
                                        if (Index != 0xff) {
                                            NvramType4.StringSet[Index] = (char*)DmiData;
                                            NvramType4.Flag |= (1 << Index);
                                            UpdateFlag = TRUE;
                                        }
                                    }
                                    break;
                                }
                    #endif                  // SYS_CHASSIS_INFO
                    #if OEM_STRING_INFO
                    case 11 :   {
                                    if (gSmbiosBoardProtocol->OemStringInfoSupport) {
                                        Index = ((TABLE_INFO*)FlashDataPtr)->Offset - 1;
                                        NvramType11.StringSet[Index] = (char*)DmiData;
                                        NvramType11.Flag |= (1 << Index);
                                        UpdateFlag = TRUE;
                                    }
                                    break;
                                }
                    #endif                  // OEM_STRING_INFO
                    #if SYSTEM_CONFIG_OPTION_INFO
                    case 12 :   {
                                    if (gSmbiosBoardProtocol->SystemConfigOptionInfoSupport) {
                                        Index = ((TABLE_INFO*)FlashDataPtr)->Offset - 1;
                                        NvramType12.StringSet[Index] = (char*)DmiData;
                                        NvramType12.Flag |= (1 << Index);
                                        UpdateFlag = TRUE;
                                    }
                                    break;
                                }
                    #endif                  // SYSTEM_CONFIG_OPTION_INFO
                    #if PORTABLE_BATTERY_INFO
                    case 22 :   {
                                    // Save handle number in Type22Handle array for each Type 22 structure
                                    for (i = 0; i < gSmbiosBoardProtocol->NumberOfBatteries; i++) {
                                        if (Type22Handle[i] == 0) {
                                            Type22Handle[i] = ((TABLE_INFO*)FlashDataPtr)->Handle;
                                            break;
                                        }
                                        if (((TABLE_INFO*)FlashDataPtr)->Handle == Type22Handle[i]) break;
                                    }
                                    if (i < gSmbiosBoardProtocol->NumberOfBatteries) {
                                        NvramType22[i].Handle = ((TABLE_INFO*)FlashDataPtr)->Handle;
                                        switch (((TABLE_INFO*)FlashDataPtr)->Offset) {
                                            case 0x04:
                                            case 0x05:
                                            case 0x06:
                                            case 0x07:
                                            case 0x08:
                                            case 0x0e:
                                            case 0x14:  Index = GetType22StringIndex(((TABLE_INFO*)FlashDataPtr)->Offset);
                                                        NvramType22[i].StringSet[Index] = (char*)DmiData;
                                                        NvramType22[i].Flag |= (1 << Index);
                                                        break;
                                            case 0x09:  MemCpy(&NvramType22[i].DeviceChemistry, FlashDataPtr + sizeof(TABLE_INFO), 1);
                                                        NvramType22[i].Flag |= 0x00010000;
                                                        break;
                                            case 0x0a:  MemCpy(&NvramType22[i].DesignCapacity, FlashDataPtr + sizeof(TABLE_INFO), 2);
                                                        NvramType22[i].Flag |= 0x00020000;
                                                        break;
                                            case 0x0c:  MemCpy(&NvramType22[i].DesignVoltage, FlashDataPtr + sizeof(TABLE_INFO), 2);
                                                        NvramType22[i].Flag |= 0x00040000;
                                                        break;
                                            case 0x0f:  MemCpy(&NvramType22[i].MaxErrorInBatteryData, FlashDataPtr + sizeof(TABLE_INFO), 1);
                                                        NvramType22[i].Flag |= 0x00080000;
                                                        break;
                                            case 0x10:  MemCpy(&NvramType22[i].SbdsSerialNumber, FlashDataPtr + sizeof(TABLE_INFO), 2);
                                                        NvramType22[i].Flag |= 0x00100000;
                                                        break;
                                            case 0x12:  MemCpy(&NvramType22[i].SbdsManufacturerDate, FlashDataPtr + sizeof(TABLE_INFO), 2);
                                                        NvramType22[i].Flag |= 0x00200000;
                                                        break;
                                            case 0x15:  MemCpy(&NvramType22[i].DesignCapacityMultiplier, FlashDataPtr + sizeof(TABLE_INFO), 1);
                                                        NvramType22[i].Flag |= 0x00400000;
                                                        break;
                                            case 0x16:  MemCpy(&NvramType22[i].OemSpecific, FlashDataPtr + sizeof(TABLE_INFO), 4);
                                                        NvramType22[i].Flag |= 0x00800000;
                                                        break;
                                        }
                                    }
                                    UpdateFlag = TRUE;
                                    break;
                                }
                    #endif                  // PORTABLE_BATTERY_INFO
                    #if SYSTEM_POWER_SUPPLY_INFO
                    case 39 :   {
                                    // Save handle number in Type39Handle array for each Type 39 structure
                                    for (i = 0; i < NUMBER_OF_POWER_SUPPLY; i++) {
                                        if (Type39Handle[i] == 0) {
                                            Type39Handle[i] = ((TABLE_INFO*)FlashDataPtr)->Handle;
                                            break;
                                        }
                                        if (((TABLE_INFO*)FlashDataPtr)->Handle == Type39Handle[i]) break;
                                    }
                                    if (i < NUMBER_OF_POWER_SUPPLY) {
                                        NvramType39[i].Handle = ((TABLE_INFO*)FlashDataPtr)->Handle;
                                        switch (((TABLE_INFO*)FlashDataPtr)->Offset) {
                                            case 0x04:  MemCpy(&NvramType39[i].PwrUnitGroup, FlashDataPtr + sizeof(TABLE_INFO), 1);
                                                        NvramType39[i].Flag |= 0x00010000;
                                                        break;
                                            case 0x05:
                                            case 0x06:
                                            case 0x07:
                                            case 0x08:
                                            case 0x09:
                                            case 0x0a:
                                            case 0x0b:  Index = GetType39StringIndex(((TABLE_INFO*)FlashDataPtr)->Offset);
                                                        NvramType39[i].StringSet[Index] = (char*)DmiData;
                                                        NvramType39[i].Flag |= (1 << Index);
                                                        break;
                                            case 0x0c:  NvramType39[i].MaxPwrCapacity = *(UINT16*)(FlashDataPtr + sizeof(TABLE_INFO));
                                                        NvramType39[i].Flag |= 0x00020000;
                                                        break;
                                            case 0x0e:  NvramType39[i].PwrSupplyChar = *(UINT16*)(FlashDataPtr + sizeof(TABLE_INFO));
                                                        NvramType39[i].Flag |= 0x00040000;
                                                        break;
                                            case 0x10:  NvramType39[i].InputVoltageProbeHandle = *(UINT16*)(FlashDataPtr + sizeof(TABLE_INFO));
                                                        NvramType39[i].Flag |= 0x00080000;
                                                        break;
                                            case 0x12:  NvramType39[i].CoolingDevHandle = *(UINT16*)(FlashDataPtr + sizeof(TABLE_INFO));
                                                        NvramType39[i].Flag |= 0x00100000;
                                                        break;
                                            case 0x14:  NvramType39[i].InputCurrentProbeHandle = *(UINT16*)(FlashDataPtr + sizeof(TABLE_INFO));
                                                        NvramType39[i].Flag |= 0x00200000;
                                                        break;
                                        }
                                    }
                                    UpdateFlag = TRUE;
                                }
                    #endif                  // SYSTEM_POWER_SUPPLY_INFO
                }
            }
        }

        if (((TABLE_INFO*)FlashDataPtr)->Flags & DMIEDIT_DELETE_STRUC) {
            FlashDataPtr += sizeof(TABLE_INFO);
        }
        else {
            FlashDataPtr += (sizeof(TABLE_INFO) + ((TABLE_INFO*)FlashDataPtr)->Size);
        }
    }
#else                                       // SMBIOS_DMIEDIT_DATA_LOC
{
    CHAR16                  *DmiArrayVar = L"DmiArray";
    DMI_VAR                 *DmiArray;
    UINTN                   DmiArraySize = DMI_ARRAY_COUNT * sizeof(DMI_VAR);
    UINT8                   Count = 0;
    CHAR16                  *S1 = L"                ";
    UINT8                   Type;
    UINT16                  Handle;
    UINT8                   Offset;
    UINT8                   Flags;

    // Initialize NVRam variables
    for (i = 0; i < DMI_ARRAY_COUNT; i++) {
        NvramDataPtrArray[i] = NULL;
    }

    pBS->AllocatePool(EfiBootServicesData, DmiArraySize, (void**)&DmiArray);
    Status = pRS->GetVariable(
                        DmiArrayVar,
                        &gAmiSmbiosNvramGuid,
                        NULL,
                        &DmiArraySize,
                        DmiArray);

    if (Status == EFI_SUCCESS) {
        Count = DmiArray[0].Type;     // Note: DmiArray[0] has count # instead of Type/Offset

        for (Index = 0; Index < Count; Index++) {
            Type = DmiArray[Index + 1].Type;
            Handle = DmiArray[Index + 1].Handle;
            Offset = DmiArray[Index + 1].Offset;
            Flags = DmiArray[Index + 1].Flags;

            Swprintf(S1, L"DmiVar%02x%04x%02x%02x",
                       Type,
                       Handle,
                       Offset,
                       Flags);

            // Check if variable already exists
            //
            // Size of zero is used to detect if the variable exists.
            // If the variable exists, an error code of EFI_BUFFER_TOO_SMALL
            // would be returned
            DmiDataSize = 0;
            Status = pRS->GetVariable(
                                S1,
                                &gAmiSmbiosNvramGuid,
                                NULL,
                                &DmiDataSize,
                                &NvramDataPtrArray[Index]);

            if (Status == EFI_NOT_FOUND) {
                continue;
            }

            if (Status == EFI_BUFFER_TOO_SMALL) {
                Status = pBS->AllocatePool(
                                    EfiBootServicesData,
                                    DmiDataSize,
                                    (void**)&NvramDataPtrArray[Index]);

                DmiData = NvramDataPtrArray[Index];

                Status = pRS->GetVariable(
                                    S1,
                                    &gAmiSmbiosNvramGuid,
                                    NULL,
                                    &DmiDataSize,
                                    DmiData);
            }

            if (Flags & DMIEDIT_DELETE_STRUC) {
                DEBUG((DEBUG_INFO, "Delete structure. Handle = %x\n", Handle));
                DeleteStructureByHandle(Handle);
                UpdateFlag = TRUE;
            }
            else if (Flags & DMIEDIT_ADD_STRUC) {
                if (Status == EFI_SUCCESS) {
                    DEBUG((DEBUG_INFO, "Add structure by handle. Handle = %x\n", Handle));
                    AddStructureByHandle(Handle, DmiData, (UINT16)DmiDataSize);
                    UpdateFlag = TRUE;
                }
            }
            else {
                if (Status == EFI_SUCCESS) {
                    DEBUG((DEBUG_INFO, "Change structure. Type = %x, Handle = %x, Offset = %x\n",\
                                Type, Handle, Offset));
                    switch (Type) {
                        case 00:
                                if (Offset == 0x04) {               // Vendor
                                    NvramType0.StringSet[0] = (char*)DmiData;
                                    NvramType0.Flag |= 0x00000001;
                                }
                                else if (Offset == 0x05) {          // Version
                                    NvramType0.StringSet[1] = (char*)DmiData;
                                    NvramType0.Flag |= 0x00000002;
                                    VersionUpdated = TRUE;
                                }
                                else if (Offset == 0x08) {          // Release Date
                                    NvramType0.StringSet[2] = (char*)DmiData;
                                    NvramType0.Flag |= 0x00000004;
                                    ReleaseDateUpdated = TRUE;
                                }
                                UpdateFlag = TRUE;
                                break;
                        case 01:
                                if (Offset == 0x04) {               // Manufacturer
                                    NvramType1.StringSet[0] = (char*)DmiData;
                                    NvramType1.Flag |= 0x00000001;
                                }
                                else if (Offset == 0x05) {          // Product Name
                                    NvramType1.StringSet[1] = (char*)DmiData;
                                    NvramType1.Flag |= 0x00000002;
                                }
                                else if (Offset == 0x06) {          // Version
                                    NvramType1.StringSet[2] = (char*)DmiData;
                                    NvramType1.Flag |= 0x00000004;
                                }
                                else if (Offset == 0x07) {          // Serial Number
                                    NvramType1.StringSet[3] = (char*)DmiData;
                                    NvramType1.Flag |= 0x00000008;
                                }
                                else if (Offset == 0x08) {          // UUID
                                    MemCpy(&NvramType1.Uuid,
                                            DmiData,
                                            16);
                                    NvramType1.Flag |= 0x00010000;
                                }
                                else if (Offset == 0x19) {          // SKU Number
                                    NvramType1.StringSet[4] = (char*)DmiData;
                                    NvramType1.Flag |= 0x00000010;
                                }
                                else if (Offset == 0x1a) {          // Family
                                    NvramType1.StringSet[5] = (char*)DmiData;
                                    NvramType1.Flag |= 0x00000020;
                                }
                                UpdateFlag = TRUE;
                                break;
#if BASE_BOARD_INFO
                        case 02:
                                if (gSmbiosBoardProtocol->BaseBoardInfoSupport) {
                                    // Save handle number in Type2Handle array for each Type 2 structure
                                    for (i = 0; i < NUMBER_OF_BASEBOARDS; i++) {
                                        if (Type2Handle[i] == 0) {
                                            Type2Handle[i] = Handle;
                                            break;
                                        }
                                        if (Handle == Type2Handle[i]) break;
                                    }
                                    if (i < NUMBER_OF_BASEBOARDS) {
                                        NvramType2[i].Handle = Handle;
                                        if (Offset == 0x04) {               // Manufacturer
                                            NvramType2[i].StringSet[0] = (char*)DmiData;
                                            NvramType2[i].Flag |= 0x00000001;
                                        }
                                        else if (Offset == 0x05) {          // Product
                                            NvramType2[i].StringSet[1] = (char*)DmiData;
                                            NvramType2[i].Flag |= 0x00000002;
                                        }
                                        else if (Offset == 0x06) {          // Version
                                            NvramType2[i].StringSet[2] = (char*)DmiData;
                                            NvramType2[i].Flag |= 0x00000004;
                                        }
                                        else if (Offset == 0x07) {          // Serial Number
                                            NvramType2[i].StringSet[3] = (char*)DmiData;
                                            NvramType2[i].Flag |= 0x00000008;
                                        }
                                        else if (Offset == 0x08) {          // Asset Tag
                                            NvramType2[i].StringSet[4] = (char*)DmiData;
                                            NvramType2[i].Flag |= 0x00000010;
                                        }
                                        else if (Offset == 0x0a) {          // Location in Chassis
                                            NvramType2[i].StringSet[5] = (char*)DmiData;
                                            NvramType2[i].Flag |= 0x00000020;
                                        }
                                    }
                                }
                                UpdateFlag = TRUE;
                                break;
#endif                                      // BASE_BOARD_INFO
#if SYS_CHASSIS_INFO
                        case 03:
                                if (gSmbiosBoardProtocol->SysChassisInfoSupport) {
                                    // Save handle number in Type3Handle array for each Type 3 structure
                                    for (i = 0; i < NUMBER_OF_SYSTEM_CHASSIS; i++) {
                                        if (Type3Handle[i] == 0) {
                                            Type3Handle[i] = Handle;
                                            break;
                                        }
                                        if (Handle == Type3Handle[i]) break;
                                    }
                                    if (i < NUMBER_OF_SYSTEM_CHASSIS) {
                                        NvramType3[i].Handle = Handle;
                                        TempBuffer = SmbiosDataTable;

                                        if (Offset == 0x04) {               // Manufacturer
                                            NvramType3[i].StringSet[0] = (char*)DmiData;
                                            NvramType3[i].Flag |= 0x00000001;
                                        }
                                        else if (Offset == 0x05) {          // Type
                                            NvramType3[i].Type = *(UINT8*)DmiData;
                                            NvramType3[i].Flag |= 0x00010000;
                                        }
                                        else if (Offset == 0x06) {          // Version
                                            NvramType3[i].StringSet[1] = (char*)DmiData;
                                            NvramType3[i].Flag |= 0x00000002;
                                        }
                                        else if (Offset == 0x07) {          // Serial Number
                                            NvramType3[i].StringSet[2] = (char*)DmiData;
                                            NvramType3[i].Flag |= 0x00000004;
                                        }
                                        else if (Offset == 0x08) {          // Asset Tag
                                            NvramType3[i].StringSet[3] = (char*)DmiData;
                                            NvramType3[i].Flag |= 0x00000008;
                                        }
                                        else if (Offset == 0x0d) {          // OEM-defined
                                            NvramType3[i].OemDefined = *(UINT32*)DmiData;
                                            NvramType3[i].Flag |= 0x00020000;
                                        }
                                        else if (Offset == 0x12) {          // Number of Power Cords
                                            NvramType3[i].NumberOfPowerCord = *(UINT8*)DmiData;
                                            NvramType3[i].Flag |= 0x00040000;
                                        }
                                        else if (FindStructureHandle(&TempBuffer, Handle)) {
                                            if (Offset == 0x15 + (((SMBIOS_SYSTEM_ENCLOSURE_INFO*)TempBuffer)->ElementCount * ((SMBIOS_SYSTEM_ENCLOSURE_INFO*)TempBuffer)->ElementRecordLength)) {  // SKU Number
                                                NvramType3[i].StringSet[4] = (char*)DmiData;
                                                NvramType3[i].Flag |= 0x00000010;
                                            }
                                        }
                                    }
                                }
                                UpdateFlag = TRUE;
                                break;
#endif                                      // SYS_CHASSIS_INFO
#if ( defined(PROCESSOR_DMIEDIT_SUPPORT) && (PROCESSOR_DMIEDIT_SUPPORT == 1) )
                        case 04:
                                if (gSmbiosBoardProtocol->ProcessorDmiEditSupport) {
                                    if (Offset == 0x20) {               // Serial Number
                                        NvramType4.StringSet[3] = (char*)DmiData;
                                        NvramType4.Flag |= BIT3;
                                    }
                                    else if (Offset == 0x21) {          // Asset Tag
                                        NvramType4.StringSet[4] = (char*)DmiData;
                                        NvramType4.Flag |= BIT4;
                                    }
                                    else if (Offset == 0x22) {          // Part Number
                                        NvramType4.StringSet[5] = (char*)DmiData;
                                        NvramType4.Flag |= BIT5;
                                    }
                                }
                                UpdateFlag = TRUE;
                                break;
#endif                                      // PROCESSOR_DMIEDIT_SUPPORT
#if OEM_STRING_INFO
                        case 11:
                                if (gSmbiosBoardProtocol->OemStringInfoSupport) {
                                    NvramType11.StringSet[Offset] = (char*)DmiData;
                                    NvramType11.Flag |= (1 << Offset);
                                }
                                UpdateFlag = TRUE;
                                break;
#endif                                      // OEM_STRING_INFO
#if SYSTEM_CONFIG_OPTION_INFO
                        case 12:
                                if (gSmbiosBoardProtocol->SystemConfigOptionInfoSupport) {
                                    NvramType12.StringSet[Offset] = (char*)DmiData;
                                    NvramType12.Flag |= (1 << Offset);
                                }
                                UpdateFlag = TRUE;
                                break;
#endif                                      // SYSTEM_CONFIG_OPTION_INFO
#if PORTABLE_BATTERY_INFO
                        case 22:
                                // Save handle number in Type22Handle array for each Type 22 structure
                                for (i = 0; i < gSmbiosBoardProtocol->NumberOfBatteries; i++) {
                                    if (Type22Handle[i] == 0) {
                                        Type22Handle[i] = Handle;
                                        break;
                                    }
                                    if (Handle == Type22Handle[i]) break;
                                }
                                if (i < gSmbiosBoardProtocol->NumberOfBatteries) {
                                    NvramType22[i].Handle = Handle;
                                    if (Offset == 0x04) {               // Location
                                        NvramType22[i].StringSet[0] = (char*)DmiData;
                                        NvramType22[i].Flag |= 0x00000001;
                                    }
                                    else if (Offset == 0x05) {          // Manufacturer
                                        NvramType22[i].StringSet[1] = (char*)DmiData;
                                        NvramType22[i].Flag |= 0x00000002;
                                    }
                                    else if (Offset == 0x06) {          // Manufacturer Date
                                        NvramType22[i].StringSet[2] = (char*)DmiData;
                                        NvramType22[i].Flag |= 0x00000004;
                                    }
                                    else if (Offset == 0x07) {          // Serial Number
                                        NvramType22[i].StringSet[3] = (char*)DmiData;
                                        NvramType22[i].Flag |= 0x00000008;
                                    }
                                    else if (Offset == 0x08) {          // Device Name
                                        NvramType22[i].StringSet[4] = (char*)DmiData;
                                        NvramType22[i].Flag |= 0x00000010;
                                    }
                                    else if (Offset == 0x09) {          // Device Chemistry
                                        NvramType22[i].DeviceChemistry = *(UINT8*)DmiData;
                                        NvramType22[i].Flag |= 0x00010000;
                                    }
                                    else if (Offset == 0x0a) {          // Design Capacity
                                        NvramType22[i].DesignCapacity = *(UINT16*)DmiData;
                                        NvramType22[i].Flag |= 0x00020000;
                                    }
                                    else if (Offset == 0x0c) {          // Design Voltage
                                        NvramType22[i].DesignVoltage = *(UINT16*)DmiData;
                                        NvramType22[i].Flag |= 0x00040000;
                                    }
                                    else if (Offset == 0x0e) {          // SBDS Version Number
                                        NvramType22[i].StringSet[5] = (char*)DmiData;
                                        NvramType22[i].Flag |= 0x00000020;
                                    }
                                    else if (Offset == 0x0f) {          // Maximum Error in Battery Data
                                        NvramType22[i].MaxErrorInBatteryData = *(UINT8*)DmiData;
                                        NvramType22[i].Flag |= 0x00080000;
                                    }
                                    else if (Offset == 0x10) {          // SBDS Serial Number
                                        NvramType22[i].SbdsSerialNumber = *(UINT16*)DmiData;
                                        NvramType22[i].Flag |= 0x00100000;
                                    }
                                    else if (Offset == 0x12) {          // SBDS Manufacturer Date
                                        NvramType22[i].SbdsManufacturerDate = *(UINT16*)DmiData;
                                        NvramType22[i].Flag |= 0x00200000;
                                    }
                                    else if (Offset == 0x14) {          // SBDS Device Chemistry
                                        NvramType22[i].StringSet[6] = (char*)DmiData;
                                        NvramType22[i].Flag |= 0x00000040;
                                    }
                                    else if (Offset == 0x15) {          // Design Capacity Multiplier
                                        NvramType22[i].DesignCapacityMultiplier = *(UINT8*)DmiData;
                                        NvramType22[i].Flag |= 0x00400000;
                                    }
                                    else if (Offset == 0x16) {          // OEM-specific
                                        NvramType22[i].OemSpecific = *(UINT32*)DmiData;
                                        NvramType22[i].Flag |= 0x00800000;
                                    }
                                }
                                UpdateFlag = TRUE;
                                break;
#endif                                      // PORTABLE_BATTERY_INFO
#if SYSTEM_POWER_SUPPLY_INFO
                        case 39:
                                // Save handle number in Type39Handle array for each Type 39 structure
                                for (i = 0; i < NUMBER_OF_POWER_SUPPLY; i++) {
                                    if (Type39Handle[i] == 0) {
                                        Type39Handle[i] = Handle;
                                        break;
                                    }
                                    if (Handle == Type39Handle[i]) break;
                                }
                                if (i < NUMBER_OF_POWER_SUPPLY) {
                                    NvramType39[i].Handle = Handle;
                                    if (Offset == 0x04) {               // Power Unit Group
                                        NvramType39[i].PwrUnitGroup = *(UINT8*)DmiData;
                                        NvramType39[i].Flag |= 0x00010000;
                                    }
                                    else if (Offset == 0x05) {          // Location
                                        NvramType39[i].StringSet[0] = (char*)DmiData;
                                        NvramType39[i].Flag |= 0x00000001;
                                    }
                                    else if (Offset == 0x06) {          // Device Name
                                        NvramType39[i].StringSet[1] = (char*)DmiData;
                                        NvramType39[i].Flag |= 0x00000002;
                                    }
                                    else if (Offset == 0x07) {          // Manufacturer
                                        NvramType39[i].StringSet[2] = (char*)DmiData;
                                        NvramType39[i].Flag |= 0x00000004;
                                    }
                                    else if (Offset == 0x08) {          // Serial Number
                                        NvramType39[i].StringSet[3] = (char*)DmiData;
                                        NvramType39[i].Flag |= 0x00000008;
                                    }
                                    else if (Offset == 0x09) {          // Asset Tag Number
                                        NvramType39[i].StringSet[4] = (char*)DmiData;
                                        NvramType39[i].Flag |= 0x00000010;
                                    }
                                    else if (Offset == 0x0a) {          // Model Part Number
                                        NvramType39[i].StringSet[5] = (char*)DmiData;
                                        NvramType39[i].Flag |= 0x00000020;
                                    }
                                    else if (Offset == 0x0b) {          // Revision Level
                                        NvramType39[i].StringSet[6] = (char*)DmiData;
                                        NvramType39[i].Flag |= 0x00000040;
                                    }
                                    else if (Offset == 0x0c) {          // Max Power Capacity
                                        NvramType39[i].MaxPwrCapacity = *(UINT16*)DmiData;
                                        NvramType39[i].Flag |= 0x00020000;
                                    }
                                    else if (Offset == 0x0e) {          // Power Supply Characteristics
                                        NvramType39[i].PwrSupplyChar = *(UINT16*)DmiData;
                                        NvramType39[i].Flag |= 0x00040000;
                                    }
                                    else if (Offset == 0x10) {          // Input Voltage Proble Handle
                                        NvramType39[i].InputVoltageProbeHandle = *(UINT16*)DmiData;
                                        NvramType39[i].Flag |= 0x00080000;
                                    }
                                    else if (Offset == 0x12) {          // Cooling Device Handle
                                        NvramType39[i].CoolingDevHandle = *(UINT16*)DmiData;
                                        NvramType39[i].Flag |= 0x00100000;
                                    }
                                    else if (Offset == 0x14) {          // Input Current Probe Handle
                                        NvramType39[i].InputCurrentProbeHandle = *(UINT16*)DmiData;
                                        NvramType39[i].Flag |= 0x00200000;
                                    }
                                }
                                UpdateFlag = TRUE;
#endif                                      // SYSTEM_POWER_SUPPLY_INFO
                    }
                }
            }
        }
    }
}
#endif                                      // SMBIOS_DMIEDIT_DATA_LOC

    // Update Smbios table if any structure has been changed
    if (UpdateFlag) {
        DEBUG((DEBUG_INFO, "Updating structures\n"));
        pBS->AllocatePool(EfiBootServicesData, BufferSize, (void**)&TempBuffer);
        MemSet(TempBuffer, BufferSize, 0);
        TempBufferPtr = TempBuffer;
        BufferPtr = SmbiosDataTable;
        do {
            switch (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type) {
                case 0  :   {
                                DEBUG((DEBUG_INFO, "Updating Type 0 structure\n"));
                                UpdateStructuresWithNvramType0(&NvramType0, BufferPtr, TempBufferPtr);
                                break;
                            }
                case 1  :   {
                                DEBUG((DEBUG_INFO, "Updating Type 1 structure\n"));
                                UpdateStructuresWithNvramType1(&NvramType1, BufferPtr, TempBufferPtr);
                                break;
                            }
                #if TYPE2_STRUCTURE
                case 2  :   {
                                DEBUG((DEBUG_INFO, "Updating Type 2 structure\n"));
                                if (gSmbiosBoardProtocol->BaseBoardInfoSupport) {
                                    // Find the NvramType2 entry for the structure by matching the handle number
                                    for (i = 0; i < NUMBER_OF_BASEBOARDS; i++) {
                                        if (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle == NvramType2[i].Handle) break;
                                    }
                                    // Update the structure with the data in NvramType2 array if found,
                                    // or just copy the original structure if not found
                                    if (i < NUMBER_OF_BASEBOARDS) {
                                        UpdateStructuresWithNvramType2(&NvramType2[i], BufferPtr, TempBufferPtr);
                                    }
                                    else {
                                        UpdateStructuresWithNvramType2(NULL, BufferPtr, TempBufferPtr);
                                    }
                                }
                                break;
                            }
                #endif                                      // BASE_BOARD_INFO
                #if TYPE3_STRUCTURE
                case 3  :   {
                                DEBUG((DEBUG_INFO, "Updating Type 3 structure\n"));
                                if (gSmbiosBoardProtocol->SysChassisInfoSupport) {
                                    StrTableInstance = GetInstanceByTypeHandle(3, ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle);

                                    // Find the NvramType3 entry for the structure by matching the handle number
                                    for (i = 0; i < NUMBER_OF_SYSTEM_CHASSIS; i++) {
                                        if (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle == NvramType3[i].Handle) break;
                                    }
                                    // Update the structure with the data in NvramType3 array if found,
                                    // or just copy the original structure if not found
                                    if (i < NUMBER_OF_SYSTEM_CHASSIS) {
                                        UpdateStructuresWithNvramType3(&NvramType3[i], BufferPtr, TempBufferPtr, StrTableInstance);
                                    }
                                    else {
                                        UpdateStructuresWithNvramType3(NULL, BufferPtr, TempBufferPtr, StrTableInstance);
                                    }
                                }
                                break;
                            }
                #endif                                      // SYS_CHASSIS_INFO
                #if ( defined(PROCESSOR_DMIEDIT_SUPPORT) && (PROCESSOR_DMIEDIT_SUPPORT == 1) )
                case 4  :   {
                                DEBUG((DEBUG_INFO, "Updating Type 4 structure\n"));
                                if (gSmbiosBoardProtocol->ProcessorDmiEditSupport) {
                                    UpdateStructuresWithNvramType4(&NvramType4, BufferPtr, TempBufferPtr);
                                }
                                break;
                            }
                #endif                                      // PROCESSOR_DMIEDIT_SUPPORT
                #if OEM_STRING_INFO
                case 11 :   {
                                DEBUG((DEBUG_INFO, "Updating Type 11 structure\n"));
                                if (gSmbiosBoardProtocol->OemStringInfoSupport) {
                                    UpdateStructuresWithNvramType11(&NvramType11, BufferPtr, TempBufferPtr);
                                }
                                break;
                            }
                #endif                                      // OEM_STRING_INFO
                #if SYSTEM_CONFIG_OPTION_INFO
                case 12 :   {
                                DEBUG((DEBUG_INFO, "Updating Type 12 structure\n"));
                                if (gSmbiosBoardProtocol->SystemConfigOptionInfoSupport) {
                                    UpdateStructuresWithNvramType12(&NvramType12, BufferPtr, TempBufferPtr);
                                }
                                break;
                            }
                #endif                                      // SYSTEM_CONFIG_OPTION_INFO
                #if PORTABLE_BATTERY_INFO
                case 22:    {
                                DEBUG((DEBUG_INFO, "Updating Type 22 structure\n"));
                                // Find the NvramType22 entry for the structure by matching the handle number
                                for (i = 0; i < gSmbiosBoardProtocol->NumberOfBatteries; i++) {
                                    if (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle == NvramType22[i].Handle) break;
                                }
                                // Update the structure with the data in NvramType22 array if found,
                                // or just copy the original structure if not found
                                if (i < gSmbiosBoardProtocol->NumberOfBatteries) {
                                    UpdateStructuresWithNvramType22(&NvramType22[i], BufferPtr, TempBufferPtr);
                                }
                                else {
                                    UpdateStructuresWithNvramType22(NULL, BufferPtr, TempBufferPtr);
                                }
                                break;
                            }
                #endif                                      // PORTABLE_BATTERY_INFO
                #if SYSTEM_POWER_SUPPLY_INFO
                case 39:    {
                                DEBUG((DEBUG_INFO, "Updating Type 39 structure\n"));
                                // Find the NvramType39 entry for the structure by matching the handle number
                                for (i = 0; i < NUMBER_OF_POWER_SUPPLY; i++) {
                                    if (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle == NvramType39[i].Handle) break;
                                }
                                // Update the structure with the data in NvramType39 array if found,
                                // or just copy the original structure if not found
                                if (i < NUMBER_OF_POWER_SUPPLY) {
                                    UpdateStructuresWithNvramType39(&NvramType39[i], BufferPtr, TempBufferPtr);
                                }
                                else {
                                    UpdateStructuresWithNvramType39(NULL, BufferPtr, TempBufferPtr);
                                }
                                break;
                            }
                #endif                                      // SYSTEM_POWER_SUPPLY_INFO
                default :   {
                                StructureSize = GetStructureLength(BufferPtr);
                                MemCpy(TempBufferPtr, BufferPtr, StructureSize);
                            }
            }
            BufferPtr += GetStructureLength(BufferPtr);
            TempBufferPtr += GetStructureLength(TempBufferPtr);
        } while (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type != 127);

        // Copy Type 127
        StructureSize = GetStructureLength(BufferPtr);
        MemCpy(TempBufferPtr, BufferPtr, StructureSize);

        DEBUG((DEBUG_INFO, "Replace all DMI data with TempBuffer\n"));
        // Replace all DMI data with TempBuffer
        MemCpy(Buffer, TempBuffer, BufferSize);

        pBS->FreePool(TempBuffer);
    }

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
    pBS->FreePool(FlashData);
#endif                                      // SMBIOS_DMIEDIT_DATA_LOC
    Status = EFI_SUCCESS;

#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC != 2
Function_Exit:
#endif                                      // SMBIOS_DMIEDIT_DATA_LOC
#if (TYPE2_STRUCTURE == 1)
    pBS->FreePool(Type2Handle);
    pBS->FreePool(NvramType2);
#endif                                      // TYPE2_STRUCTURE
#if (TYPE3_STRUCTURE == 1)
    pBS->FreePool(Type3Handle);
    pBS->FreePool(NvramType3);
#endif                                      // TYPE3_STRUCTURE
    pBS->FreePool(Type22Handle);
    pBS->FreePool(NvramType22);
    pBS->FreePool(Type39Handle);
    pBS->FreePool(NvramType39);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit UpdateStructuresWithNvramData. Status = %r  :::\n", Status));

    return Status;
}
#endif                                      // SmbiosDMIEdit_SUPPORT

#if UPDATE_BASEBOARD_TYPE2
/**
    Updates base board structure (Type 2) in input Buffer with
    dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateBaseBoardData(
    IN UINT8        *Buffer
)
{
    EFI_STATUS      Status;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINT8           *SrcBuffer;
    UINT8           *StructureFoundPtr;
    UINT8           *SmbiosVariableBuffer;
    UINTN           SmbiosVariableSize;
    UINT16          StructureHandle;
    UINT8           *TempStructure2;
    UINTN           TempStructure2Size;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateBaseBoardData  ***\n"));

    SmbiosVariableSize = sizeof(BASE_BOARD_DATA);

    pBS->AllocatePool(
                EfiBootServicesData,
                SmbiosVariableSize,
                &SmbiosVariableBuffer);

    Status = pRS->GetVariable(
                        SmbiosBaseBoardVar,
                        &gAmiSmbiosDynamicDataGuid,
                        &Attributes,
                        &SmbiosVariableSize,
                        SmbiosVariableBuffer);
    if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) {
        pBS->FreePool(SmbiosVariableBuffer);

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get SmbiosBaseBoardVar\n"));
        return Status;
    }

    if (Status != EFI_NOT_FOUND) {
        SrcBuffer = Buffer;

        //
        // Search for Type 2 (Base Board) structure pointer
        //
        if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 2, 1)) {

            TempStructure2Size = (UINTN)(sizeof(SMBIOS_BASE_BOARD_INFO) + 6*65 + 1);
            pBS->AllocatePool(EfiBootServicesData,
                             TempStructure2Size,
                             &TempStructure2);

            StructureHandle = ((SMBIOS_STRUCTURE_HEADER*)StructureFoundPtr)->Handle;
            MemCpy(TempStructure2, StructureFoundPtr, GetStructureLength(StructureFoundPtr));

#if UPDATE_BOARD_MANUFACTURER
            //
            // Update Board Manufacturer Name
            //
            ReplaceString(TempStructure2, 1, (UINT8*)((BASE_BOARD_DATA*)SmbiosVariableBuffer)->BoardManufacturer);
#endif                                      // UPDATE_BOARD_MANUFACTURER
#if UPDATE_BOARD_NAME
            //
            // Update Board Name String corresponding to Board ID.
            //
            ReplaceString(TempStructure2, 2, (UINT8*)((BASE_BOARD_DATA*)SmbiosVariableBuffer)->BoardName);
#endif                                      // UPDATE_BOARD_NAME
            //
            // Add Structure to the table
            //
            WriteStructureByHandle(StructureHandle, TempStructure2, GetStructureLength(TempStructure2));

            pBS->FreePool(TempStructure2);
        }
    }

    pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateBaseBoardData  :::\n"));

    return EFI_SUCCESS;
}
#endif                                      // UPDATE_BASEBOARD_TYPE2

#if UPDATE_SYSTEM_CHASSIS_TYPE3
/**
    Updates Chassis structure (Type 3) in input Buffer with
    dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateChassisData(
    IN UINT8        *Buffer
)
{
    EFI_STATUS      Status;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINT8           *SrcBuffer;
    UINT8           *StructureFoundPtr;
    UINT8           *SmbiosVariableBuffer;
    UINTN           SmbiosVariableSize;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateChassisData  ***\n"));

    SmbiosVariableSize = sizeof(CHASSIS_DATA);

    pBS->AllocatePool(
                EfiBootServicesData,
                SmbiosVariableSize,
                &SmbiosVariableBuffer);

    Status = pRS->GetVariable(
                        SmbiosChassisVar,
                        &gAmiSmbiosDynamicDataGuid,
                        &Attributes,
                        &SmbiosVariableSize,
                        SmbiosVariableBuffer);
    if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) {
        pBS->FreePool(SmbiosVariableBuffer);

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get SmbiosChassisVar\n"));
        return Status;
    }

    if (Status != EFI_NOT_FOUND) {
        SrcBuffer = Buffer;
        if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 3, 1)) {
            ((SMBIOS_SYSTEM_ENCLOSURE_INFO*)StructureFoundPtr)->Type = ((CHASSIS_DATA*)SmbiosVariableBuffer)->Type;
        }
    }

    pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateChassisData  :::\n"));

    return EFI_SUCCESS;
}
#endif                                      // UPDATE_SYSTEM_CHASSIS_TYPE3

#if UPDATE_CPU_TYPE4
/**
    Updates CPU structures (Type 4 & 7) in input Buffer with
    dynamically detected data.

    @param Buffer

    @return EFI_STATUS

    @remark If SMBIOS_CPU_INFO_PROTOCOL is not found, this function tries to\n
            use the "CPU_DATA" variable (backward compatibility). Otherwise\n
            CPU dynamic information is not available.

**/
EFI_STATUS
DynamicUpdateCpuData(
    IN UINT8        *Buffer
)
{
    EFI_STATUS                  Status;
    UINT8                       UseVariable = FALSE;
    UINT8                       i;
    UINT8                       *SrcBuffer =  Buffer;
    UINT8                       *StructureFoundPtr;
    UINT8                       *SmbiosVariableBuffer;
    UINT32                      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINTN                       Size;
    EFI_GUID                    gSmbiosCpuInfoGuid = AMI_SMBIOS_CPU_INFO_PROTOCOL_GUID;
    SMBIOS_CPU_INFO_PROTOCOL    *SmbiosCpuInfo;
    SMBIOS_PROCESSOR_INFO       *CurCpu;
    SMBIOS_CACHE_INFO           *CurCache;
    SINGLE_CPU_DATA             *DynamicData;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateCpuData  ***\n"));

    Status = pBS->LocateProtocol(&gSmbiosCpuInfoGuid, NULL, (void**)&SmbiosCpuInfo);
    if (EFI_ERROR(Status)) {
        UseVariable = TRUE,
        Size = sizeof(CPU_DYNAMIC_DATA);
        pBS->AllocatePool(
                EfiBootServicesData,
                Size,
                (void**)&SmbiosVariableBuffer);
        Status = pRS->GetVariable(
                            SmbiosCpuVar,
                            &gAmiSmbiosDynamicDataGuid,
                            &Attributes,
                            &Size,
                            SmbiosVariableBuffer);
        if (EFI_ERROR(Status)) {
            pBS->FreePool(SmbiosVariableBuffer);

            DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get SmbiosCpuVar\n"));
            return Status;
        }
    }

    for (i = 0; i < gSmbiosBoardProtocol->NumberOfCPU; i++) {
      if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 4, 1)) {
        Size = GetStructureLength(StructureFoundPtr);
        Status = pBS->AllocatePool(
                                EfiBootServicesData,
                                Size + 1024,            // 1K additional buffer
                                (void**)&CurCpu);

        if (Status == EFI_SUCCESS) {
          UINT16    *L3HandlePtr;

          MemCpy(CurCpu, StructureFoundPtr, Size);
          if (UseVariable)
            DynamicData =
              &((CPU_DYNAMIC_DATA*)SmbiosVariableBuffer)->CpuData[i];
          else
            DynamicData = &SmbiosCpuInfo->CpuData[i];

          CurCpu->Family = DynamicData->CpuData.Family;
          CurCpu->ProcessorID_1 = DynamicData->CpuData.ProcessorID_1;
          CurCpu->ProcessorID_2 = DynamicData->CpuData.ProcessorID_2;
          CurCpu->Voltage = DynamicData->CpuData.Voltage | BIT07;
          CurCpu->ExtClockFreq = DynamicData->CpuData.ExtClockFreq;
          CurCpu->MaxSpeed = DynamicData->CpuData.MaxSpeed;
          CurCpu->CurrentSpeed = DynamicData->CpuData.CurrentSpeed;
          CurCpu->Status = DynamicData->CpuData.Status;
          CurCpu->Upgrade = DynamicData->CpuData.Upgrade;
          if (DynamicData->CpuData.CoreCount > 255) {
              CurCpu->CoreCount = 0xff;
              CurCpu->CoreCount2 = DynamicData->CpuData.CoreCount;
              CurCpu->CoreEnabled = 0xff;
              CurCpu->CoreEnabled2 = DynamicData->CpuData.CoreEnabled;
          }
          else {
              CurCpu->CoreCount = (UINT8)DynamicData->CpuData.CoreCount;
              CurCpu->CoreEnabled = (UINT8)DynamicData->CpuData.CoreEnabled;
          }
          if (DynamicData->CpuData.ThreadCount > 255) {
              CurCpu->ThreadCount = 0xff;
              CurCpu->ThreadCount2 = DynamicData->CpuData.ThreadCount;
          }
          else {
              CurCpu->ThreadCount = (UINT8)DynamicData->CpuData.ThreadCount;
          }
          CurCpu->Family2 = DynamicData->CpuData.Family2;

          L3HandlePtr = &CurCpu->L3CacheHandle;

          ReplaceString((UINT8*)CurCpu, 2, DynamicData->CpuData.Manufacturer);
          ReplaceString((UINT8*)CurCpu, 3, DynamicData->CpuData.Version);

          if (DynamicData->CpuData.Status & 0x040) {
            // L1 Cache
            if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 7, 1)) {
              CurCache = (SMBIOS_CACHE_INFO*)StructureFoundPtr;
              CurCache->CacheConfig = DynamicData->L1Cache.CacheConfig;
              CurCache->MaxCacheSize = DynamicData->L1Cache.MaxCacheSize;
              CurCache->InstalledSize = DynamicData->L1Cache.InstalledSize;
              CurCache->SupportSRAM = DynamicData->L1Cache.SupportSRAM;
              CurCache->CurrentSRAM = DynamicData->L1Cache.CurrentSRAM;
              CurCache->CacheSpeed = DynamicData->L1Cache.CacheSpeed;
              CurCache->ErrorCorrectionType =
                DynamicData->L1Cache.ErrorCorrectionType;
              CurCache->SystemCacheType = DynamicData->L1Cache.SystemCacheType;
              CurCache->Associativity = DynamicData->L1Cache.Associativity;
              CurCache->MaxCacheSize2 = DynamicData->L1Cache.MaxCacheSize2;
              CurCache->InstalledCacheSize2 = DynamicData->L1Cache.InstalledCacheSize2;

              // L2 Cache
              if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 7, 1)) {
                CurCache = (SMBIOS_CACHE_INFO*)StructureFoundPtr;
                CurCache->CacheConfig = DynamicData->L2Cache.CacheConfig;
                CurCache->MaxCacheSize = DynamicData->L2Cache.MaxCacheSize;
                CurCache->InstalledSize = DynamicData->L2Cache.InstalledSize;
                CurCache->SupportSRAM = DynamicData->L2Cache.SupportSRAM;
                CurCache->CurrentSRAM = DynamicData->L2Cache.CurrentSRAM;
                CurCache->CacheSpeed = DynamicData->L2Cache.CacheSpeed;
                CurCache->ErrorCorrectionType =
                  DynamicData->L2Cache.ErrorCorrectionType;
                CurCache->SystemCacheType = DynamicData->L2Cache.SystemCacheType;
                CurCache->Associativity = DynamicData->L2Cache.Associativity;
                CurCache->MaxCacheSize2 = DynamicData->L2Cache.MaxCacheSize2;
                CurCache->InstalledCacheSize2 = DynamicData->L2Cache.InstalledCacheSize2;

                // L3 Cache
                if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 7, 1)) {
                  CurCache = (SMBIOS_CACHE_INFO*)StructureFoundPtr;

                  // Hide structure if L3 cache size is 0
                  if (DynamicData->L3Cache.InstalledSize == 0) {
                    CurCache->StructureType.Type = 126;
                    *L3HandlePtr = 0xffff;
                  }
                  else {
                    CurCache->CacheConfig = DynamicData->L3Cache.CacheConfig;
                    CurCache->MaxCacheSize = DynamicData->L3Cache.MaxCacheSize;
                    CurCache->InstalledSize = DynamicData->L3Cache.InstalledSize;
                    CurCache->SupportSRAM = DynamicData->L3Cache.SupportSRAM;
                    CurCache->CurrentSRAM = DynamicData->L3Cache.CurrentSRAM;
                    CurCache->CacheSpeed = DynamicData->L3Cache.CacheSpeed;
                    CurCache->ErrorCorrectionType =
                      DynamicData->L3Cache.ErrorCorrectionType;
                    CurCache->SystemCacheType =
                      DynamicData->L3Cache.SystemCacheType;
                    CurCache->Associativity = DynamicData->L3Cache.Associativity;
                    CurCache->MaxCacheSize2 = DynamicData->L3Cache.MaxCacheSize2;
                    CurCache->InstalledCacheSize2 = DynamicData->L3Cache.InstalledCacheSize2;
                  }
                }
              }
            }
          }

          // L1 Cache
          else if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 7, 1)) {
            CurCache = (SMBIOS_CACHE_INFO*)StructureFoundPtr;
            CurCache->StructureType.Type = 126;

            // L2 Cache
            if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 7, 1)) {
              CurCache = (SMBIOS_CACHE_INFO*)StructureFoundPtr;
              CurCache->StructureType.Type = 126;

              // L3 Cache
              if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 7, 1)) {
                CurCache = (SMBIOS_CACHE_INFO*)StructureFoundPtr;
                CurCache->StructureType.Type = 126;
              }
            }
          }

          WriteStructureByHandle(((SMBIOS_STRUCTURE_HEADER*)CurCpu)->Handle,
                                 (UINT8*)CurCpu,
                                 GetStructureLength((UINT8*)CurCpu)
                                );

          pBS->FreePool(CurCpu);
        }
      }
    }

    if (UseVariable)
        pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateCpuData  :::\n"));

    return EFI_SUCCESS;
}
#endif                                      // UPDATE_CPU_TYPE4

#if UPDATE_SLOT_TYPE9
/**
    Updates System Slot structure (Type 9) in input Buffer with
    dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateSlotsData(
    IN UINT8        *Buffer
)
{
    EFI_STATUS      Status;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINT8           *SrcBuffer;
    UINT8           *StructureFoundPtr;
    UINT8           *SmbiosVariableBuffer;
    UINTN           SmbiosVariableSize;
    UINT8           i;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateSlotsData  ***\n"));

    SmbiosVariableSize = sizeof(SYSTEM_SLOT_DYNAMIC_DATA);
    pBS->AllocatePool(EfiBootServicesData, SmbiosVariableSize, (void**)&SmbiosVariableBuffer);
    Status = pRS->GetVariable(SmbiosSlotsVar, &gAmiSmbiosDynamicDataGuid,
                             &Attributes, &SmbiosVariableSize, SmbiosVariableBuffer);
    if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) {
        pBS->FreePool(SmbiosVariableBuffer);

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get SmbiosSlotsVar\n"));
        return Status;
    }

    if (Status != EFI_NOT_FOUND) {
        SrcBuffer = Buffer;
        for (i = 0; i < gSmbiosBoardProtocol->NumberOfSystemSlots; i++) {
            if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 9, 1)) {
                ((SMBIOS_SYSTEM_SLOTS_INFO*)StructureFoundPtr)->CurrentUsage = ((SYSTEM_SLOT_DYNAMIC_DATA*)SmbiosVariableBuffer)->CurrentUsage[i];
                ((SMBIOS_SYSTEM_SLOTS_INFO*)StructureFoundPtr)->BusNumber = ((SYSTEM_SLOT_DYNAMIC_DATA*)SmbiosVariableBuffer)->BusNumber[i];
            }
        }
    }
    pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateSlotsData  :::\n"));

    return EFI_SUCCESS;
}
#endif                                      // UPDATE_SLOT_TYPE9

#if UPDATE_ONBOARD_DEV_TYPE10
/**
    Updates On Board Devices Information structure (Type 10) in
    input Buffer with dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateOnboardDevData(
    IN UINT8        *Buffer
)
{
    EFI_STATUS      Status;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINT8           *SrcBuffer;
    UINT8           *StructureFoundPtr;
    UINT8           *SmbiosVariableBuffer;
    UINTN           SmbiosVariableSize;
    UINT8           i;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateOnboardDevData  ***\n"));

    SmbiosVariableSize = sizeof(ONBOARD_DEV_DYNAMIC_DATA);
    pBS->AllocatePool(EfiBootServicesData, SmbiosVariableSize, (void**)&SmbiosVariableBuffer);
    Status = pRS->GetVariable(SmbiosOnBoardVar, &gAmiSmbiosDynamicDataGuid,
                             &Attributes, &SmbiosVariableSize, SmbiosVariableBuffer);
    if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) {
        pBS->FreePool(SmbiosVariableBuffer);

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get SmbiosOnBoardVar\n"));
        return Status;
    }

    if (Status != EFI_NOT_FOUND) {
        SrcBuffer = Buffer;
        if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 10, 1)) {
            StructureFoundPtr += 4;
            for (i = 0; i < gSmbiosBoardProtocol->NumberOfOnBoardDevices; i++) {
                ((SINGLE_DEV_INFO*)StructureFoundPtr)->DeviceType &= 0x7f;
                ((SINGLE_DEV_INFO*)StructureFoundPtr)->DeviceType |= ((ONBOARD_DEV_DYNAMIC_DATA*)SmbiosVariableBuffer)->OnBoardDev[i];

                StructureFoundPtr += 2;
            }
        }
    }

    pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateOnboardDevData  :::\n"));

    return EFI_SUCCESS;
}
#endif                                      // UPDATE_ONBOARD_DEV_TYPE10

/**
    Update Type 17 Serial Number field for the input memory slot
    with data obtained from the SPD.

    @param StructureBuffer
    @param SpdSmBusAddr

    @return EFI_STATUS

**/
EFI_STATUS
UpdateSerialNumber (
    IN UINT8                    *StructBuffer,
    IN EFI_SMBUS_DEVICE_ADDRESS SpdSmBusAddr,
    IN MEM_SPD_DATA             *SpdData
)
{
    EFI_STATUS              Status;
    UINTN                   i;
    UINT8                   Data8;
    UINTN                   SpdOffset = 0;
    UINT8                   Ascii;

    StructBuffer += ((SMBIOS_STRUCTURE_HEADER*)StructBuffer)->Length;
    StructBuffer += GetStringOffset(StructBuffer, 4);

    if (MemType == 0) return EFI_UNSUPPORTED;       // Not supported
    if (MemType == 1) SpdOffset = 0x5f;
    if (MemType == 2) SpdOffset = 122;
    if (MemType == 3) SpdOffset = 325;

    if (SpdOffset == 0) return EFI_UNSUPPORTED;       // Not supported

    for (i = 0; i < 4; i++) {
        if (SpdData) {
            if (SpdOffset == 95) {                  // 0x5f
                Data8 = SpdData->Byte95To98[95 + i - 95];
            }
            else if (SpdOffset == 122){
                Data8 = SpdData->Byte122To125[122 + i - 122];
            }
            else {
                Data8 = SpdData->Byte325To328[325 + i - 325];
            }
        }
        else {
            Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, SpdOffset + i, &Data8);
            if (EFI_ERROR(Status)) return Status;
        }

        if (*StructBuffer == 0) return EFI_BUFFER_TOO_SMALL;    // Truncate if string is too long
        Ascii = (Data8 >> 4);
        if (Ascii > 9) *StructBuffer++ = Ascii - 10 + 0x41;
        else *StructBuffer++ = Ascii + 0x30;
        if (*StructBuffer == 0) return EFI_BUFFER_TOO_SMALL;    // Truncate if string is too long
        Ascii = Data8 & 15;
        if (Ascii > 9) *StructBuffer++ = Ascii - 10 + 0x41;
        else *StructBuffer++ = Ascii + 0x30;
    }

    while (*StructBuffer != 0) *StructBuffer++ = 0x20;

    return EFI_SUCCESS;
}

/**
    If input character is in the range of 0x20 to 0x7e (readable
    character), returns the input character value unchanged.
    Otherwise returns single space character.

    @param _char

    @return Input character or space

**/
CHAR8
FilterCharacter (
    IN CHAR8    _char
)
{
    if( (_char >= 0x20 && _char <= 0x7E) ){
        return _char;
    }

    return ' ';
}

/**
    Update Type 17 Part Number field for the input memory slot
    with data obtained from the SPD.

    @param StructBuffer
    @param SpdSmBusAddr

    @return EFI_STATUS

**/
EFI_STATUS
UpdatePartNumber (
    IN UINT8                    *StructBuffer,
    IN EFI_SMBUS_DEVICE_ADDRESS SpdSmBusAddr,
    IN MEM_SPD_DATA             *SpdData
)
{
    EFI_STATUS              Status;
    UINTN                   i;
    UINT8                   Data8;
    UINTN                   SpdOffset = 0;

    StructBuffer += ((SMBIOS_STRUCTURE_HEADER*)StructBuffer)->Length;
    StructBuffer += GetStringOffset(StructBuffer, 6);

    if (MemType == 0) return EFI_UNSUPPORTED;       // Not supported
    if (MemType == 1) SpdOffset = 0x49;
    if (MemType == 2) SpdOffset = 128;
    if (MemType == 3) SpdOffset = 329;

    if (SpdOffset == 0) return EFI_UNSUPPORTED;     // Not supported

    for (i = 0; i < 18; i++) {
        if (SpdData) {
            if (SpdOffset == 73) {                  // 0x49
                Data8 = SpdData->Byte73To90[73 + i - 73];
            }
            else if (SpdOffset == 128){
                Data8 = SpdData->Byte128To145[128 + i - 128];
            }
            else {
                Data8 = SpdData->Byte329To348[329 + i - 329];
            }
        }
        else {
            Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, SpdOffset + i, &Data8);
            if (EFI_ERROR(Status)) return Status;
        }
        if (*StructBuffer == 0) return EFI_BUFFER_TOO_SMALL;    // Truncate if string is too long
        if (Data8 == 0) Data8 = 0x20;
        *StructBuffer++ = FilterCharacter(Data8);
    }

    while (*StructBuffer != 0) *StructBuffer++ = 0x20;

    return EFI_SUCCESS;
}

/**
    Validate StringNum for input structure type

    @param StructureType
    @param StringNum String number (1 based)

    @return BOOLEAN

**/
BOOLEAN
ValidateStringNumber(
    IN UINT8    *StructurePtr,
    IN UINT8    StructureType,
    IN UINTN    StringNumber
)
{
	BOOLEAN		Status = FALSE;

    if (FindString(&StructurePtr, (UINT8)StringNumber)) {
        UINT8       *NextStrPtr;

        NextStrPtr = StructurePtr;

        while(*NextStrPtr != 0) {
            NextStrPtr++;
        }

        if (NextStrPtr != StructurePtr) {
        	Status = TRUE;
        }
    }

    return Status;
}

/**
    Replace the #StringNum in the input buffer *DestStructPtr
    with StringData

    @param DestStructPtr Pointer to structure to be updated
    @param StringNum String number (1 based)
    @param StringData String with NULL terminated character

    @return EFI_STATUS

**/
EFI_STATUS
ReplaceString(
    IN UINT8    *DestStructPtr,
    IN UINT8    StringNum,
    IN UINT8    *StringData
)
{
    UINT8       StringSize = 0;
    UINT8       *StructEndPtr;
    UINTN       RemainingSize;

    if (FindString(&DestStructPtr, StringNum)) {
        UINT8       *TempPtr;
        UINT8       *NextStrPtr;

        NextStrPtr = DestStructPtr;
        StructEndPtr = DestStructPtr;

        while(*NextStrPtr != 0) {
            NextStrPtr++;
        }

        // NextStrPtr = Pointer to the next string
        NextStrPtr++;

        while(*(UINT16*)StructEndPtr != 0) {
            StructEndPtr++;
        }

        RemainingSize = StructEndPtr + 2 - NextStrPtr;  // Including double NULL characters

        TempPtr = StringData;
        while (*(TempPtr++) != 0) {
            StringSize++;
        }
        StringSize++;                   // Including NULL character

        // Copy remaining strings
        MemCpy(DestStructPtr + StringSize, NextStrPtr, RemainingSize);

        // Copy the string
        MemCpy(DestStructPtr, StringData, StringSize);

        return EFI_SUCCESS;
    }
    else {
        // String not found
        return EFI_NOT_FOUND;
    }
}

/**
    Update Type 17 Manufacturer field for the input memory slot
    with data obtained from the SPD.

    @param structBuffer
    @param SpdSmBusAddr

    @return EFI_STATUS

**/
EFI_STATUS
UpdateManufacturer(
    IN UINT8                    *StructBuffer,
    IN EFI_SMBUS_DEVICE_ADDRESS SpdSmBusAddr,
    IN MEM_SPD_DATA             *SpdData
)
{
    EFI_STATUS      Status;
    UINTN           i;
    UINT8           Data8;
    UINT8           Index;
    JEDEC_MF_ID     *IdTblPtr = NULL;

    switch (MemType) {
        case 1: for (i = 0; i < 8; i++) {           // DDR2
                    if (SpdData) {
                        Data8 = SpdData->Byte64To71[64 + i - 64];
                    }
                    else {
                        Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, 64 + i, &Data8);
                        if (EFI_ERROR(Status)) return Status;
                    }
                    if (Data8 != 0x7f) break;
                }
                break;
        case 2: if (SpdData) {                      // DDR3
                    i = SpdData->Byte117To118[0] & 0x7f;    // Remove parity bit
                    Data8 = SpdData->Byte117To118[118 - 117];
                }
                else {
                    Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, 117, &Data8);
                    if (EFI_ERROR(Status)) return Status;
                    i = Data8 & 0x7f;               // Remove parity bit
                    Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, 118, &Data8);
                    if (EFI_ERROR(Status)) return Status;
                }
                break;
        case 3: if (SpdData) {                      // DDR4
                    i = SpdData->Byte320To321[0] & 0x7f;    // Remove parity bit
                    Data8 = SpdData->Byte320To321[321 - 320];
                }
                else {
                    Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, 320, &Data8);
                    if (EFI_ERROR(Status)) return Status;
                    i = Data8 & 0x7f;               // Remove parity bit
                    Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, 321, &Data8);
                    if (EFI_ERROR(Status)) return Status;
                }
                break;
        default: return EFI_UNSUPPORTED;            // Not supported
    }

    if (i > 7) i = 7;
    IdTblPtr = ManufacturerJedecIdBankTable[i];

    // Search in Manufacturer table
    while ((IdTblPtr->VendorId != Data8) && (IdTblPtr->VendorId != 0xff)) {
        IdTblPtr++;
    }

    if (IdTblPtr->VendorId != 0xff) {
        Status = ReplaceString(StructBuffer, 3, (UINT8*)IdTblPtr->ManufacturerString);
    }
    else {
        Index = 0;
        Status = EFI_NOT_FOUND;
        while (Strcmp((char*)ManufacturerTable[Index].Manufacturer, "Undefined")) {
            if (i == ManufacturerTable[Index].NumContCode && Data8 == ManufacturerTable[Index].ModuleManufacturer) {
                  Status = ReplaceString(StructBuffer, 3, ManufacturerTable[Index].Manufacturer);
                  break;
            }

            Index++;
        }
    }

    return Status;
}

/**
    Detect memory device type from SPD data.
    Only DDR2 and DDR3 are supported

    @param SpdSmBusAddr

    @retval UINT8 Memory Type\n
                  0 -> Memory type not supported\n
                  1 -> DDR2\n
                  2 -> DDR3\n

**/
UINT8
GetMemoryType(
    IN EFI_SMBUS_DEVICE_ADDRESS     SpdSmBusAddr
)
{
    EFI_STATUS      Status;
    UINT8           Data8;

    MemType = 0;                            // Not supported

    Status = gSmbiosBoardProtocol->GetSpdByte(SpdSmBusAddr, 2, &Data8);
    if (!EFI_ERROR(Status)) {
        if (Data8 == 8) MemType = 1;        // DDR2
        if (Data8 == 11) MemType = 2;       // DDR3
        if (Data8 == 12) MemType = 3;       // DDR4
    }

    return MemType;
}

#if UPDATE_MEMORY_TYPE16
/**
    Updates Memory related structures (Type 16-20) in
    input Buffer with dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateMemoryData(
    IN UINT8        *Buffer
)
{
    EFI_STATUS      Status;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINT8           *SrcBuffer;
    UINT8           *StructureFoundPtr;
    UINT8           i;
    UINT8           j;
    UINT8           Data8;
    UINT8           MemErrorCorrection;
    UINTN           SmbiosVariableSize = sizeof(SYSTEM_MEM_ARRAY_DYNAMIC_DATA);
    SYSTEM_MEM_ARRAY_DYNAMIC_DATA     *SmbiosVariableBuffer;
    SMBIOS_PHYSICAL_MEM_ARRAY_INFO    *SmbArray;
    SMBIOS_MEMORY_DEVICE_INFO         *SmbDevice;
#if MEMORY_ERROR_INFO
    SMBIOS_MEMORY_ERROR_INFO          *SmbError;
#endif
    SMBIOS_MEM_ARRAY_MAP_ADDR_INFO    *SmbAmap;
#if MEMORY_DEVICE_INFO
    SMBIOS_MEM_DEV_MAP_ADDR_INFO      *SmbDmap;
#endif
    PHYSICAL_MEM_ARRAY                *DynArray;
    MEMORY_DEVICE_GROUP               *DynDevice;
    UINT8                             DataWidth[] = {8, 16, 32, 64};
    EFI_SMBUS_DEVICE_ADDRESS          DummyAddr;
    EFI_GUID                          HobListGuid = HOB_LIST_GUID;
    EFI_GUID                          SpdInfoHobGuid = AMI_SMBIOS_MEMORY_INFO_HOB_GUID;
    VOID                              *pHobList;
    MEM_SPD_DATA                      *SpdDataPtr;
    UINT8                             *TempBuffer;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateMemoryData  ***\n"));

    pBS->AllocatePool(EfiBootServicesData,
                        SmbiosVariableSize,
                        (void**)&SmbiosVariableBuffer);

    pBS->AllocatePool(EfiBootServicesData,
                        0x200,
                        (void**)&TempBuffer);

    MemSet(TempBuffer, 0x200, 0);

    Status = pRS->GetVariable(SmbiosMemVar,
                        &gAmiSmbiosDynamicDataGuid,
                        &Attributes,
                        &SmbiosVariableSize,
                        SmbiosVariableBuffer);
    if (EFI_ERROR(Status)) {
        pBS->FreePool(SmbiosVariableBuffer);
        if (Status == EFI_NOT_FOUND)
            return EFI_SUCCESS;
        return Status;
    }

    pHobList = GetEfiConfigurationTable(pST, &HobListGuid);
    if (pHobList != NULL) {
        Status = FindNextHobByGuid(&SpdInfoHobGuid, &pHobList);
        if (EFI_ERROR(Status)) {
            pHobList = NULL;                // Smbios Memory Info Hob not found
        }
        else {
            SpdDataPtr = (MEM_SPD_DATA*)((UINTN)pHobList + sizeof(EFI_HOB_GUID_TYPE));
        }
    }

    SrcBuffer = Buffer;
    for (i = 0; i < gSmbiosBoardProtocol->NumberOfPhysicalMemoryArray; i++)
    {
        // Update Type 16
        if (FindStructureType(&SrcBuffer,
                        &StructureFoundPtr,
                        16,
                        1)) {
            SmbArray = ((SMBIOS_PHYSICAL_MEM_ARRAY_INFO*)StructureFoundPtr);
            DynArray = &(SmbiosVariableBuffer->PhysicalMemArray[i]);
            SmbArray->MaxCapacity = DynArray->MaxCapacity;
            SmbArray->ExtMaxCapacity = DynArray->ExtMaxCapacity;
            MemErrorCorrection = 0x06;                      // Assume Multi-bit ECC

            // Update Type 18
            #if MEMORY_ERROR_INFO
            if (gSmbiosBoardProtocol->MemoryErrorInfoSupport) {
                if (DynArray->ArrayMemoryError.ErrorType == 3) {
                    SmbArray->MemErrInfoHandle = 0xffff;    // No error
                }
                if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 18, 1)) {
                    SmbError = (SMBIOS_MEMORY_ERROR_INFO*)StructureFoundPtr;
                    SmbError->ErrorType = DynArray->ArrayMemoryError.ErrorType;
                    SmbError->ErrorGranularity = DynArray->ArrayMemoryError.ErrorGranularity;
                    SmbError->ErrorOperation = DynArray->ArrayMemoryError.ErrorOperation;
                    SmbError->MemArrayErrorAddress = DynArray->ArrayMemoryError.MemArrayErrorAddress;
                    SmbError->DeviceErrorAddress = DynArray->ArrayMemoryError.DeviceErrorAddress;
                    SmbError->ErrorResolution = DynArray->ArrayMemoryError.ErrorResolution;

                    if (DynArray->ArrayMemoryError.ErrorType != 3) {
                        // Update this device's Memory Error Information Handle
                        SmbArray->MemErrInfoHandle = SmbError->StructureType.Handle;
                    }
                }
            }
            #endif

            // Update Type 19
            if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 19, 1)) {
                SmbAmap = (SMBIOS_MEM_ARRAY_MAP_ADDR_INFO*)StructureFoundPtr;
                SmbAmap->StartingAddress = DynArray->MemArrayMapAddr.StartingAddress;
                SmbAmap->EndingAddress = DynArray->MemArrayMapAddr.EndingAddress;
                SmbAmap->PartitionWidth = DynArray->MemArrayMapAddr.PartitionWidth;
                SmbAmap->ExtendedStartAddr = DynArray->MemArrayMapAddr.ExtendedStartAddr;
                SmbAmap->ExtendedEndAddr = DynArray->MemArrayMapAddr.ExtendedEndAddr;
            }

            DEBUG((DEBUG_INFO, "Updating memory slots\n"));
            for (j = 0; j < gSmbiosBoardProtocol->NumberOfMemorySlots[i]; j++)
            {
                // Update Type 17
                DEBUG((DEBUG_INFO, "Searching for slot %x structure\n", j+1));
                if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 17, 1)){
                    DEBUG((DEBUG_INFO, "Found slot %x structure\n", j+1));
                    SmbDevice = (SMBIOS_MEMORY_DEVICE_INFO*)StructureFoundPtr;
                    DynDevice = &(DynArray->MemoryDeviceData[j]);

                    SmbDevice->Size = DynDevice->Type17.Size;
                    SmbDevice->ExtendedSize = DynDevice->Type17.ExtendedSize;
                    SmbDevice->ConfMemClkSpeed = DynDevice->Type17.ConfMemClkSpeed;
                    SmbDevice->TotalWidth = DynDevice->Type17.TotalWidth;
                    SmbDevice->DeviceSet = DynDevice->Type17.DeviceSet;
                    SmbDevice->MinimumVoltage = DynDevice->Type17.MinimumVoltage;
                    SmbDevice->MaximumVoltage = DynDevice->Type17.MaximumVoltage;
                    SmbDevice->ConfiguredVoltage = DynDevice->Type17.ConfiguredVoltage;
                    if (DynDevice->Type17.TypeDetail != 0) {
                        SmbDevice->TypeDetail = DynDevice->Type17.TypeDetail;
                    }
                    if (SmbDevice->Size == 0) {
                        DEBUG((DEBUG_INFO, "Slot %x is empty\n", j+1));
                        SmbDevice->MemoryType = 2;      // Set to Unknown if slot empty
                        SmbDevice->Speed = 0;
                        SmbDevice->Attributes = 0;
                    }
                    else {
                        DEBUG((DEBUG_INFO, "Slot %x populated\n", j+1));
                        SmbDevice->MemoryType = DynDevice->Type17.MemoryType;
                        SmbDevice->Speed = DynDevice->Type17.Speed;

                        if (pHobList) {
                            MemType = 0;

                            switch (SpdDataPtr->Byte2) {
                                case 0x08:              // DDR2
                                        MemType = 1;
                                        SmbDevice->TotalWidth = SpdDataPtr->Byte5To8[6 - 5];
                                        Data8 = SpdDataPtr->Byte11To14[14 - 11];
                                        SmbDevice->DataWidth = SmbDevice->TotalWidth - Data8;
                                        Data8 = SpdDataPtr->Byte5To8[0];
                                        SmbDevice->Attributes = (Data8 & 0x07) + 1;
                                        if (SpdDataPtr->Byte11To14[0] != 0x02) {
                                            MemErrorCorrection = 0x03;      // No ECC
                                        }
                                        break;

                                case 0x0b:              // DDR3
                                        MemType = 2;
                                        Data8 = SpdDataPtr->Byte5To8[8 - 5];
                                        SmbDevice->DataWidth = DataWidth[Data8 & 0x07];
                                        Data8 = (Data8 & 0x18) ? 8: 0;
                                        if (Data8 == 0) {
                                            MemErrorCorrection = 0x03;      // No ECC
                                        }
                                        SmbDevice->TotalWidth = SmbDevice->DataWidth + Data8;
                                        Data8 = SpdDataPtr->Byte5To8[7 - 5];
                                        SmbDevice->Attributes = ((Data8 >> 3) & 0x07) + 1;
                                        break;

                                case 0x0c:              // DDR4
                                        MemType = 3;
                                        Data8 = SpdDataPtr->Byte11To14[13 - 11];
                                        SmbDevice->DataWidth = DataWidth[Data8 & 0x07];
                                        Data8 = (Data8 & 0x18) ? 8: 0;
                                        if (Data8 == 0) {
                                            MemErrorCorrection = 0x03;      // No ECC
                                        }
                                        SmbDevice->TotalWidth = SmbDevice->DataWidth + Data8;
                                        Data8 = SpdDataPtr->Byte11To14[12 - 11];
                                        SmbDevice->Attributes = ((Data8 >> 3) & 0x07) + 1;
                            }

                            DummyAddr.SmbusDeviceAddress = 0;
                            MemCpy(TempBuffer, SmbDevice, GetStructureLength((UINT8*)SmbDevice));
                            UpdateManufacturer(TempBuffer, DummyAddr, SpdDataPtr);
                            UpdateSerialNumber(TempBuffer, DummyAddr, SpdDataPtr);
                            UpdatePartNumber(TempBuffer, DummyAddr, SpdDataPtr);
                            WriteStructureByHandle(SmbDevice->StructureType.Handle, TempBuffer, GetStructureLength(TempBuffer));
                        }
                        else {
                            switch (GetMemoryType(DynArray->SpdSmBusAddr[j])) {
                                case 0: break;

                                case 1: gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 6, &Data8);    // DDR2
                                        SmbDevice->TotalWidth = Data8;
                                        gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 14, &Data8);
                                        SmbDevice->DataWidth = SmbDevice->TotalWidth - Data8;
                                        gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 5, &Data8);
                                        SmbDevice->Attributes = (Data8 & 0x07) + 1;
                                        gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 11, &Data8);
                                        if (Data8 != 0x02) {
                                            MemErrorCorrection = 0x03;      // No ECC
                                        }
                                        break;

                                case 2: gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 8, &Data8);    // DDR3
                                        SmbDevice->DataWidth = DataWidth[Data8 & 0x07];
                                        Data8 = (Data8 & 0x18) ? 8: 0;
                                        if (Data8 == 0) {
                                            MemErrorCorrection = 0x03;      // No ECC
                                        }
                                        SmbDevice->TotalWidth = SmbDevice->DataWidth + Data8;
                                        gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 7, &Data8);
                                        SmbDevice->Attributes = ((Data8 >> 3) & 0x07) + 1;
                                        break;

                                case 3: gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 13, &Data8);    // DDR4
                                        SmbDevice->DataWidth = DataWidth[Data8 & 0x07];
                                        Data8 = (Data8 & 0x18) ? 8: 0;
                                        if (Data8 == 0) {
                                            MemErrorCorrection = 0x03;      // No ECC
                                        }
                                        SmbDevice->TotalWidth = SmbDevice->DataWidth + Data8;
                                        gSmbiosBoardProtocol->GetSpdByte(DynArray->SpdSmBusAddr[j], 12, &Data8);
                                        SmbDevice->Attributes = ((Data8 >> 3) & 0x07) + 1;
                            }

                            if (GetMemoryType(DynArray->SpdSmBusAddr[j]) != 0) {
                                MemCpy(TempBuffer, SmbDevice, GetStructureLength((UINT8*)SmbDevice));
                                UpdateManufacturer(TempBuffer, DynArray->SpdSmBusAddr[j], 0);
                                UpdateSerialNumber(TempBuffer, DynArray->SpdSmBusAddr[j], 0);
                                UpdatePartNumber(TempBuffer, DynArray->SpdSmBusAddr[j], 0);
                                WriteStructureByHandle(SmbDevice->StructureType.Handle, TempBuffer, GetStructureLength(TempBuffer));
                            }
                        }
                    }

                    // Update Type 18
                    #if MEMORY_ERROR_INFO
                    if (gSmbiosBoardProtocol->MemoryErrorInfoSupport) {
                        if (DynDevice->Type18.ErrorType == 3) {
                            SmbDevice->MemErrorInfoHandle = 0xffff;    // No error
                        }
                        if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 18, 1)) {
                            SmbError = (SMBIOS_MEMORY_ERROR_INFO*)StructureFoundPtr;
                            SmbError->ErrorType = DynDevice->Type18.ErrorType;
                            SmbError->ErrorGranularity = DynDevice->Type18.ErrorGranularity;
                            SmbError->ErrorOperation = DynDevice->Type18.ErrorOperation;
                            SmbError->MemArrayErrorAddress = DynDevice->Type18.MemArrayErrorAddress;
                            SmbError->DeviceErrorAddress = DynDevice->Type18.DeviceErrorAddress;
                            SmbError->ErrorResolution = DynDevice->Type18.ErrorResolution;

                            if (DynDevice->Type18.ErrorType != 3) {
                                // Update this device's Memory Error Information Handle
                                SmbDevice->MemErrorInfoHandle = SmbError->StructureType.Handle;
                            }
                        }
                    }
                    #endif

                    // Update Type 20
                    #if MEMORY_DEVICE_INFO
                    if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 20, 1)) {
                        SmbDmap = (SMBIOS_MEM_DEV_MAP_ADDR_INFO*)StructureFoundPtr;
                        if (SmbDevice->Size) {
                            SmbDmap->StartingAddress = DynDevice->Type20.StartingAddress;
                            SmbDmap->EndingAddress = DynDevice->Type20.EndingAddress;
                            SmbDmap->PartitionRowPosition = DynDevice->Type20.PartitionRowPosition;
                            SmbDmap->InterleavePosition = DynDevice->Type20.InterleavePosition;
                            SmbDmap->InterleaveDataDepth = DynDevice->Type20.InterleaveDataDepth;
                            SmbDmap->ExtendedStartAddr = DynDevice->Type20.ExtendedStartAddr;
                            SmbDmap->ExtendedEndAddr = DynDevice->Type20.ExtendedEndAddr;
                        }
                        else {
                            SmbDmap->StructureType.Type = 126;
                        }
                    }
                    #endif
                }

                SpdDataPtr++;
            }

            SmbArray->MemErrorCorrection = MemErrorCorrection;
        }
    }

    pBS->FreePool(TempBuffer);
    pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateMemoryData  :::\n"));

    return EFI_SUCCESS;
}
#endif

#if FLASH_MEMORY_ARRAY_INFO && defined(FLASH_PART_STRING_LENGTH)
/**
    Updates Flash Memory related structures (Type 16-20)
    with dynamically detected data.

    @param None

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateFlashMemoryData(VOID)
{
    UINT32                      Flash4GBMapStart;
    UINT8                       FlashInfo[0x100];
    char                        *Manufacturer;
    char                        *PartNumber;
    UINT8                       *Buffer;
    UINT8                       *FlashMemoryStructurePtr;
    EFI_SMBIOS_HANDLE           SmbiosHandle;
    UINTN                       StringNumber;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateFlashMemoryData  ***\n"));

    Flash4GBMapStart = 0xFFFFFFFF - FLASH_BLOCK_SIZE;
    Flash4GBMapStart ++;
    GetFlashPartInfomation ( (UINT8*)Flash4GBMapStart, FlashInfo );

    Manufacturer = FlashInfo + 8;               // Skip "$FPS" signature and size
    PartNumber = Strstr(Manufacturer, " ");
    // Replace " " with string terminator for Manufacturer
    *PartNumber++ = 0;

    Buffer = SmbiosDataTable;

    // Search for Flash Memory structure
    while (1) {
        FindStructureType(&Buffer, &FlashMemoryStructurePtr, 17, 1);
        if (FlashMemoryStructurePtr == NULL) {
            break;
        }
        else {
            if (((SMBIOS_MEMORY_DEVICE_INFO*)FlashMemoryStructurePtr)->MemoryType == 9) {
                break;
            }
        }
    };

    if (FlashMemoryStructurePtr != NULL) {
        SmbiosHandle = ((SMBIOS_STRUCTURE_HEADER*)FlashMemoryStructurePtr)->Handle;
        StringNumber = 3;
        SmbiosPiUpdateString(NULL, &SmbiosHandle, &StringNumber, Manufacturer);
        StringNumber = 6;
        SmbiosPiUpdateString(NULL, &SmbiosHandle, &StringNumber, PartNumber);

        DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateFlashMemoryData  :::\n"));
        return EFI_SUCCESS;
    }

    DEBUG((DEBUG_ERROR | DEBUG_WARN, "Flash Memory Structure not found\n"));
    return EFI_NOT_FOUND;
}
#endif

#if UPDATE_BATTERY_TYPE22
/**
    Updates Portable Battery structures (Type 22) in
    input Buffer with dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateBatteryData(
    IN UINT8        *Buffer
)
{
    EFI_STATUS      Status;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINT16          StructureHandle;
    UINT8           *SrcBuffer;
    UINT8           *StructureFoundPtr;
    UINT8           i;
    UINT8           *TempStructure22;
    UINTN           TempStructure22Size;
    UINT8           *SmbiosVariableBuffer;
    UINTN           SmbiosVariableSize;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateBatteryData  ***\n"));

    SmbiosVariableSize = sizeof(BATTERY_DYNAMIC_DATA);
    pBS->AllocatePool(EfiBootServicesData,
                    SmbiosVariableSize,
                    &SmbiosVariableBuffer);

    Status = pRS->GetVariable(SmbiosBatteryVar,
                        &gAmiSmbiosDynamicDataGuid,
                        &Attributes,
                        &SmbiosVariableSize,
                        SmbiosVariableBuffer);

    if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) {
        pBS->FreePool(SmbiosVariableBuffer);

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get SmbiosBatteryVar\n"));
        return Status;
    }

    if (Status != EFI_NOT_FOUND) {
        TempStructure22Size = sizeof(SMBIOS_PORTABLE_BATTERY_INFO) + (7 * 65) + 1;
        pBS->AllocatePool(EfiBootServicesData,
                        TempStructure22Size,
                        &TempStructure22);

        for (i = 0; i < gSmbiosBoardProtocol->NumberOfBatteries; i++) {
            SrcBuffer = Buffer;
            if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 22, i + 1)) {
                StructureHandle = ((SMBIOS_STRUCTURE_HEADER*)StructureFoundPtr)->Handle;
                if(((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].Valid){
                    MemCpy(TempStructure22,
                        StructureFoundPtr,
                        GetStructureLength(StructureFoundPtr));

                    ((SMBIOS_PORTABLE_BATTERY_INFO*)TempStructure22)->DesignCapacity =
                        ((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].DesignCapacity;
                    ((SMBIOS_PORTABLE_BATTERY_INFO*)TempStructure22)->DesignVoltage =
                        ((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].DesignVoltage;
                    ((SMBIOS_PORTABLE_BATTERY_INFO*)TempStructure22)->SBDSSerialNumber =
                        ((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].SbdsSerialNumber;
                    ((SMBIOS_PORTABLE_BATTERY_INFO*)TempStructure22)->SBDSManufacturerDate =
                        ((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].SbdsManufacturedDate;
                    ((SMBIOS_PORTABLE_BATTERY_INFO*)TempStructure22)->DesignCapabilityMult =
                        ((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].DesignCapacityMult;

                    ReplaceString(TempStructure22,
                        5,
                        ((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].DeviceName);
                    ReplaceString(TempStructure22,
                        6,
                        ((BATTERY_DYNAMIC_DATA*)SmbiosVariableBuffer)->BatteryDynamicData[i].SbdsVersion);

                    WriteStructureByHandle(StructureHandle,
                                        TempStructure22,
                                        GetStructureLength(TempStructure22));
                }
                else {
                    DeleteStructureByHandle(StructureHandle);
                }
            }
        }
        pBS->FreePool(TempStructure22);
    }

    pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateBatteryData  :::\n"));

    return EFI_SUCCESS;
}
#endif

#if UPDATE_ADDITIONAL_INFO_TYPE40
/**
    Updates Additional Information structures (Type 40) in
    input Buffer - Referenced Handle field.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateAdditionalInfoData(
    IN UINT8        *Buffer
)
{
    UINT8           *SrcBuffer;
    UINT8           *AdditionalInfoPtr;
    UINT8           *StructureFoundPtr;
    UINT8           NumAdditionalInfoEntries;
    UINT8           StructureType;
    UINT8           Instance;
    UINT8           i;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateAdditionalInfoData  ***\n"));

    SrcBuffer = Buffer;
    if (FindStructureType(&SrcBuffer, &AdditionalInfoPtr, 40, 1))
    {
        NumAdditionalInfoEntries = ((SMBIOS_ADDITIONAL_INFO*)AdditionalInfoPtr)->NumAdditionalInfoEntries;
        for (i = 0; i < NumAdditionalInfoEntries; i++)
        {
            SrcBuffer = Buffer;
            StructureType = ((SMBIOS_ADDITIONAL_INFO*)AdditionalInfoPtr)->AdditionalEntries[i].RefHandle / 16;
            Instance = ((SMBIOS_ADDITIONAL_INFO*)AdditionalInfoPtr)->AdditionalEntries[i].RefHandle % 16;
            if (FindStructureType(&SrcBuffer, &StructureFoundPtr, StructureType, Instance))
            {
                ((SMBIOS_ADDITIONAL_INFO*)AdditionalInfoPtr)->AdditionalEntries[i].RefHandle =
                                                        ((SMBIOS_STRUCTURE_HEADER*)StructureFoundPtr)->Handle;
            }
        }
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateAdditionalInfoData  :::\n"));

    return EFI_SUCCESS;
}
#endif

#if UPDATE_DEVICE_EXT_TYPE41
/**
    Updates On Board Devices Extended Information structure (Type 41)
    in input Buffer with dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateOnboardDevExt(
    IN UINT8        *Buffer
)
{
    EFI_STATUS      Status;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINT8           *SrcBuffer;
    UINT8           *StructureFoundPtr;
    UINT8           *SmbiosVariableBuffer;
    UINTN           SmbiosVariableSize;
    UINT8           i;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateOnboardDevExt  ***\n"));

    SmbiosVariableSize = sizeof(ONBOARD_DEV_EXT_DYNAMIC_DATA);
    pBS->AllocatePool(EfiBootServicesData, SmbiosVariableSize, (void**)&SmbiosVariableBuffer);
    Status = pRS->GetVariable(SmbiosOnBoardExtVar, &gAmiSmbiosDynamicDataGuid,
                             &Attributes, &SmbiosVariableSize, SmbiosVariableBuffer);
    if (EFI_ERROR(Status) && Status != EFI_NOT_FOUND) {
        pBS->FreePool(SmbiosVariableBuffer);

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get SmbiosOnBoardExtVar\n"));
        return Status;
    }

    if (Status != EFI_NOT_FOUND) {
        SrcBuffer = Buffer;
        for (i = 0; i < gSmbiosBoardProtocol->NumberOfOnboardDevExt; i++) {
            if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 41, 1)) {
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)StructureFoundPtr)->DeviceType &= 0x7f;
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)StructureFoundPtr)->DeviceType |= ((ONBOARD_DEV_EXT_DYNAMIC_DATA*)SmbiosVariableBuffer)->OnBoardDev[i];
                ((SMBIOS_ONBOARD_DEV_EXT_INFO*)StructureFoundPtr)->BusNumber = ((ONBOARD_DEV_EXT_DYNAMIC_DATA*)SmbiosVariableBuffer)->BusNumber[i];
            }
        }
    }
    pBS->FreePool(SmbiosVariableBuffer);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateOnboardDevExt  :::\n"));
    return EFI_SUCCESS;
}
#endif

#if (SMBIOS_TABLES_BUILD_TYPE == 1)
#if UPDATE_BIOS_RELEASE_DATE
/**
    Update Bios Release Date data field in Type 0 structure
    with today's date

    @param      *Buffer

    @return     EFI_STATUS

**/
EFI_STATUS
UpdateBiosReleaseDate(
    IN UINT8        *Buffer
)
{
    UINT8           *StructureFoundPtr;

    if (FindStructureType(&Buffer, &StructureFoundPtr, 0, 1)) {
        ReplaceString(StructureFoundPtr, 3, (UINT8*)TODAY);
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}
#endif                      // UPDATE_BIOS_RELEASE_DATE

/**
    Search for structure "Type" and replace its "StringNum" string
    with input StringData

    @param Type Structure type to search for
    @param InstanceNum Instance number of structure to search for (1 based)
    @param StringNum String number (1 based)
    @param StringData String with NULL terminated character

    @return EFI_STATUS

**/
EFI_STATUS
ReplaceStructureString(
    IN UINT8    Type,
    IN UINT8    InstanceNum,
    IN UINT8    StringNumber,
    IN UINT8    *StringData
)
{
    EFI_STATUS      Status;
    UINT16          Handle;
    UINT16          Size;
    UINT8           *StructurePtr;
    UINT8           *TempBuffer;

    pBS->AllocatePool(
                EfiBootServicesData,
                0x1000,
                (void**)&TempBuffer);

    if (FindStructureType(&SmbiosDataTable, &StructurePtr, Type, InstanceNum)) {
        Size = GetStructureLength(StructurePtr);
        MemCpy(TempBuffer, StructurePtr, Size);
        ReplaceString(TempBuffer, StringNumber, (UINT8*)StringData);
        Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
        Status = WriteStructureByHandle(Handle, TempBuffer, GetStructureLength(TempBuffer));
    }
    else {
        Status = EFI_NOT_FOUND;
    }

    pBS->FreePool(TempBuffer);
    return Status;
}

/**
    Update Bios Version data field in Type 0 structure
    with CORE_MAJOR_VERSION and CORE_BUILD_NUMBER SDL
    token values

    @param      *Buffer

    @return     EFI_STATUS

**/
EFI_STATUS
UpdateBiosVersion(
    IN UINT8        *Buffer
)
{
    CHAR8           MajorVersionStr[] = "                    ";
    CHAR8           BuildNumberStr[]  = "                    ";
    CHAR8           BiosVersionStr[]  = "                    ";

    Sprintf(BiosVersionStr,
            "%s.%s",
            Itoa(CORE_MAJOR_VERSION, MajorVersionStr, 10),
            Itoa(CORE_BUILD_NUMBER, BuildNumberStr, 10)
           );

    return ReplaceStructureString(0, 1, 2, (UINT8*)BiosVersionStr);
}

/**
    Update Bios ROM size data field in Type 0 structure

    @param      *Buffer

    @return     EFI_STATUS

**/
EFI_STATUS
UpdateBiosRomSize(
    IN UINT8    *Buffer
)
{
    UINT8       *StructPtr;

    if (FindStructureType(&Buffer, &StructPtr, 0, 1)) {
        ((SMBIOS_BIOS_INFO*)StructPtr)->BiosRomSize = (BIOS_SIZE >> 16);
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}
#endif                      // (SMBIOS_TABLES_BUILD_TYPE == 1)

/**
    Updates CPU, System Slot, On Board Devices, Memory structures
    input Buffer with dynamically detected data.

    @param Buffer

    @return EFI_STATUS

**/
EFI_STATUS
DynamicUpdateStructures(
    IN  UINT8                       *Buffer
)
{
    static BOOLEAN FirstRun = TRUE;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DynamicUpdateStructures  ***\n"));

    if (FirstRun && (gSmbiosBoardProtocol != NULL)) {
        #if ((SMBIOS_TABLES_BUILD_TYPE == 1) && UPDATE_BIOS_VERSION)
            if (!VersionUpdated) {
                UpdateBiosVersion(SmbiosDataTable);
            }
            if (!ReleaseDateUpdated) {
                UpdateBiosRomSize(SmbiosDataTable);
            }
        #endif
        #if ((SMBIOS_TABLES_BUILD_TYPE == 1) && UPDATE_BIOS_RELEASE_DATE)
            UpdateBiosReleaseDate(SmbiosDataTable);
        #endif
        #if (TYPE2_STRUCTURE && UPDATE_BASEBOARD_TYPE2)
            if (gSmbiosBoardProtocol->BaseBoardInfoSupport) {
                DynamicUpdateBaseBoardData(SmbiosDataTable);
            }
        #endif
        #if (TYPE3_STRUCTURE && UPDATE_SYSTEM_CHASSIS_TYPE3)
            if (gSmbiosBoardProtocol->SysChassisInfoSupport) {
                DynamicUpdateChassisData(SmbiosDataTable);
            }
        #endif
        #if (TYPE4_STRUCTURE && UPDATE_CPU_TYPE4)
        if (UpdateCpuStructure) {
            DynamicUpdateCpuData(SmbiosDataTable);
        }
        #endif
        #if (TYPE9_STRUCTURE && UPDATE_SLOT_TYPE9)
            DynamicUpdateSlotsData(SmbiosDataTable);
        #endif
        #if (TYPE10_STRUCTURE && UPDATE_ONBOARD_DEV_TYPE10)
            if (gSmbiosBoardProtocol->OnboardDeviceInfoSupport) {
                DynamicUpdateOnboardDevData(SmbiosDataTable);
            }
        #endif
        #if (TYPE16_STRUCTURE && UPDATE_MEMORY_TYPE16)
            gSmbiosBoardProtocol->EnableSmbusController();
            DynamicUpdateMemoryData(SmbiosDataTable);
            gSmbiosBoardProtocol->RestoreSmbusController();
        #endif
        #if FLASH_MEMORY_ARRAY_INFO && defined(FLASH_PART_STRING_LENGTH)
            DynamicUpdateFlashMemoryData();
        #endif
        #if (TYPE22_STRUCTURE && UPDATE_BATTERY_TYPE22)
            if (gSmbiosBoardProtocol->PortableBatteryInfoSupport) {
                DynamicUpdateBatteryData(SmbiosDataTable);
            }
        #endif
        #if (TYPE40_STRUCTURE && UPDATE_ADDITIONAL_INFO_TYPE40)
            if (gSmbiosBoardProtocol->AdditionalInfoSupport) {
                DynamicUpdateAdditionalInfoData(SmbiosDataTable);
            }
        #endif
        #if (TYPE41_STRUCTURE && UPDATE_DEVICE_EXT_TYPE41)
            if (gSmbiosBoardProtocol->OnboardDeviceExtInfoSupport) {
                DynamicUpdateOnboardDevExt(SmbiosDataTable);
            }
        #endif

        FirstRun = FALSE;
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DynamicUpdateStructures  :::\n"));
    return EFI_SUCCESS;
}

/**
    SMBIOS protocol - Returns the pointer to the SmbiosTableEntryPoint

    @param None

    @return Pointer to the SmbiosTableEntryPoint

**/
VOID*
GetSmbiosTableEntryPoint(
)
{
#if SMBIOS_2X_SUPPORT
    return pSmbiosTableEntryPoint;
#elif SMBIOS_3X_SUPPORT
    return pSmbiosV3TableEntryPoint;
#endif
    return 0;
}

/**
    SMBIOS protocol - Returns the pointer to input version of the
    SmbiosTableEntryPoint

    @param      SmbiosVersion

    @return     VOID* - Pointer to the input version of SmbiosTableEntryPoint

**/
VOID*
GetSmbiosVerTableEntryPoint(
    IN UINT8    SmbiosMajorVersion
)
{
    if (SmbiosMajorVersion == 2) {
#if SMBIOS_2X_SUPPORT
        return pSmbiosTableEntryPoint;
#else
        return 0;
#endif                                          // SMBIOS_2X_SUPPORT
    }

    if (SmbiosMajorVersion == 3) {
#if SMBIOS_3X_SUPPORT
        return pSmbiosV3TableEntryPoint;
#else
        return 0;
#endif                                          // SMBIOS_3X_SUPPORT
    }

    return 0;
}

/**
    SMBIOS protocol - Returns the pointer to the Scratch buffer
    equal the SMBIOS data structure in size.

    @param None

    @return Pointer to the Scratch buffer

**/
VOID*
GetScratchBufferPtr(
)
{
    return ScratchBufferPtr;
}

/**
    SMBIOS protocol - Returns the maximum size for the SMBIOS
    data structure table.

    @param None

    @return Maximum SMBIOS data table size

**/
UINT16
GetBufferMaxSize(VOID)
{
    return MaximumTableSize;
}

/**
    Checks for structure validity

    @param Buffer
    @param Size

    @return EFI_STATUS

**/
EFI_STATUS
CheckForValidStructure(
    IN  UINT8   *Buffer,
    IN  UINT16  Size
)
{
    // Check for valid terminator
    if (*(UINT16*)(Buffer + Size - 2) != 0) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Invalid structure terminator\n"));
        return DMI_BAD_PARAMETER;
    }

    // Check for valid size
    if (Size < (((SMBIOS_STRUCTURE_HEADER*)Buffer)->Length + 2)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Invalid structure size\n"));
        return DMI_BAD_PARAMETER;
    }
    return DMI_SUCCESS;
}

/**
    Returns the total structure size

    @param Buffer

    @return Total structure size

**/
UINT16
GetTotalStructureSize(
    IN UINT8  *Buffer
)
{
    UINT16  Length = 0;
    UINT16  BlockSize;

    while (((SMBIOS_STRUCTURE_HEADER*)Buffer)->Type != 127) {
        BlockSize = GetStructureLength(Buffer);
        Length += BlockSize;
        Buffer += BlockSize;
    }
    Length += GetStructureLength(Buffer);
    return Length;
}

/**
    Return the size from the Pointer Buffer to the last
    structure 127.

    @param Buffer

    @return Size of remaining structure

**/
UINT16
GetRemainingStructuresSize(
    IN UINT8  *Buffer
)
{
    UINT16  Length = 0;
    UINT16  BlockSize;

    while (((SMBIOS_STRUCTURE_HEADER*)Buffer)->Type != 127) {
        BlockSize = GetStructureLength(Buffer);
        Length += BlockSize;
        Buffer += BlockSize;
    }
    Length += GetStructureLength(Buffer);
    return Length;
}

#if (defined(SB_WAKEUP_TYPE_FN) && (SB_WAKEUP_TYPE_FN == 1))
/**
    Create "WakeUpType" NVRAM variable containing pointer to
    Type 1 structure "Wake-up Type" field in Smbios Data Table
    so that it can be updated on S3 resume

    @param      None

    @return     None

**/
VOID
SaveWakeupTypeLocation(
)
{
    UINT8           *Buffer;
    UINT8           *FoundPtr = NULL;

    Buffer = SmbiosDataTable;

    if (FindStructureType(&Buffer, &FoundPtr, 1, 1)) {
        FoundPtr += 0x18;

        DEBUG((DEBUG_INFO, "WakeUpType byte location: %x\n", FoundPtr));

        pRS->SetVariable(
                        L"WakeUpType",
                        &gAmiSmbiosNvramGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        4,
                        &FoundPtr);
    }
}
#endif      // SB_WAKEUP_TYPE_FN

#if SMBIOS_2X_SUPPORT
/**
    Updates the SMBIOS 2.x Entry Point Header

    @param pSmbiosTableEntryPoint

    @retval None

**/
VOID
UpdateEPSHeader(
    IN  SMBIOS_TABLE_ENTRY_POINT  *pSmbiosTableEntryPoint
)
{
    DEBUG((DEBUG_INFO, ":::  SMBIOS - UpdateEPSHeader. pSmbiosTableEntryPoint = %08x  :::\n", pSmbiosTableEntryPoint));

    // Updating EPS Header
    // Update SMBIOS Data Structure Table length in EPS Header
    pSmbiosTableEntryPoint->TableLength = GetTotalStructureSize(SmbiosDataTable);
    pSmbiosTableEntryPoint->TableAddress = (UINT32)(UINTN)SmbiosDataTable;
    DEBUG((DEBUG_INFO, "***  SMBIOS 2.x - TableAddress = %08x  ***\n", pSmbiosTableEntryPoint->TableAddress));

    // Find and update number of SMBios Structures in EPS Header
    pSmbiosTableEntryPoint->NumberOfSmbiosStructures = GetNumberOfStructures(SmbiosDataTable);

    // Find and update largest SMBios Structure in EPS Header
    pSmbiosTableEntryPoint->MaxStructureSize = GetLargestStructureSize((UINT8*)(UINTN)pSmbiosTableEntryPoint->TableAddress);

    // Update Checksums in EPS Header
    pSmbiosTableEntryPoint->IntermediateChecksum = 0;
    pSmbiosTableEntryPoint->IntermediateChecksum = SmbiosCheckSum((UINT8*)pSmbiosTableEntryPoint + 0x10, 15);

    pSmbiosTableEntryPoint->EntryPointStructureChecksum = 0;
    pSmbiosTableEntryPoint->EntryPointStructureChecksum = SmbiosCheckSum((UINT8*)pSmbiosTableEntryPoint,
                                                                      pSmbiosTableEntryPoint->EntryPointLength);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit UpdateEPSHeader  :::\n"));
}
#endif                                          // SMBIOS_2X_SUPPORT

#if SMBIOS_3X_SUPPORT
/**
    Updates the SMBIOS 3.x Entry Point Header

    @param pSmbiosV3TableEntryPoint

    @retval None

**/
VOID
UpdateEPSHeader_3(
    IN  SMBIOS_3X_TABLE_ENTRY_POINT *pSmbiosV3TableEntryPoint
)
{
    DEBUG((DEBUG_INFO, ":::  SMBIOS - UpdateEPSHeader_3. pSmbiosV3TableEntryPoint = %016lx  :::\n", pSmbiosV3TableEntryPoint));

    pSmbiosV3TableEntryPoint->TableMaximumSize = GetTotalStructureSize(SmbiosDataTable);
    pSmbiosV3TableEntryPoint->TableAddress = (UINT64)(UINTN)SmbiosDataTable;
    DEBUG((DEBUG_INFO, "***  SMBIOS 3.0 - TableAddress = %016lx  ***\n", pSmbiosV3TableEntryPoint->TableAddress));

    // Update Checksums in EPS Header
    pSmbiosV3TableEntryPoint->EntryPointStructureChecksum = 0;
    pSmbiosV3TableEntryPoint->EntryPointStructureChecksum = SmbiosCheckSum((UINT8*)pSmbiosV3TableEntryPoint,
                                                                      pSmbiosV3TableEntryPoint->EntryPointLength);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit UpdateEPSHeader_3  :::\n"));
}
#endif                                          // SMBIOS_3X_SUPPORT

/**
    SMBIOS protocol - Updates the SMBIOS Table Header

    @param      None

    @return     EFI_STATUS

**/
EFI_STATUS
UpdateSmbiosTableHeader(VOID)
{
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    UINT32      Granularity = 0;
#endif

    DEBUG((DEBUG_INFO, "*** SMBIOS - UpdateSmbiosTableHeader  ***\n"));

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    if (LegacyRegionProtocol != NULL) {
        LegacyRegionProtocol->UnLock(LegacyRegionProtocol,
                                     0xf0000,
                                     0x10000,
                                     &Granularity);
    }
#endif                                          // CSM_SUPPORT

#if SMBIOS_2X_SUPPORT
    UpdateEPSHeader(pSmbiosTableEntryPoint);            // EPS 2.x Header in runtime memory
    if (pSmbiosTableEntryPointF000) {
        // Copy to second EPS 2.x Header in F000
        MemCpy(pSmbiosTableEntryPointF000, pSmbiosTableEntryPoint, sizeof(SMBIOS_TABLE_ENTRY_POINT));
    }
#endif                                          // SMBIOS_2X_SUPPORT

#if SMBIOS_3X_SUPPORT
    UpdateEPSHeader_3(pSmbiosV3TableEntryPoint);        // EPS 3.0 Header in runtime memory
    if (pSmbiosV3TableEntryPointF000) {
        // Copy to second EPS 3.0 Header in F000
        MemCpy(pSmbiosV3TableEntryPointF000, pSmbiosV3TableEntryPoint, sizeof(SMBIOS_3X_TABLE_ENTRY_POINT));
    }
#endif                                          // SMBIOS_3X_SUPPORT

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    if (LegacyRegionProtocol != NULL) {
        LegacyRegionProtocol->Lock(LegacyRegionProtocol,
                                     0xf0000,
                                     0x10000,
                                     &Granularity);
    }
#endif                                          // CSM_SUPPORT

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit UpdateSmbiosTableHeader  :::\n"));
    return  EFI_SUCCESS;
}

#if (CONVERT_TYPE4_V2X_TO_V3X == 1)
/**
    Converts Smbios Type4 structure from Smbios 2.x format
    to Smbios 3.x

    @param      **Buffer

    @return     None

**/
VOID
ConverType4ToV30(
    IN OUT UINT8    **Buffer
)
{
    UINT8       *TempBuffer;
    UINT16      Size;

    pBS->AllocatePages(
                AllocateAnyPages,
                EfiBootServicesData,
                1,
                (EFI_PHYSICAL_ADDRESS*)&TempBuffer
                );

    Size = GetStructureLength(*Buffer);
    MemCpy(TempBuffer, *Buffer, Size);
    ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Length = sizeof(SMBIOS_PROCESSOR_INFO);
    MemSet(TempBuffer + 0x2a, 6, 0);
    MemCpy(TempBuffer + sizeof(SMBIOS_PROCESSOR_INFO), *Buffer + 0x2a, Size - 0x2a);
    *Buffer = TempBuffer;
}
#endif

/**
    Update the SMBIOS Table

    @param Operation
    @param StructurePtr
        - Adding operation: N/A
        - Delete operation: Pointer to
        structure to be deleted
    @param Buffer
        - Adding operation: Pointer to buffer
        to be added
        - Delete operation: N/A
    @param Size
        - Adding operation: Size of buffer
        to be added
        - Delete operation: N/A
    @param Handle
        - Adding operation:
        = 0xFFFE or 0xFFFF to assign next handle number
        = Other value to assign a specific
        handle number
        - Delete operation: N/A

    @retval EFI_ALREADY_STARTED Input Handle already exists for "add" operation
    @retval EFI_PROTOCOL_ERROR Input structure is invalid for "add" operation
    @retval EFI_OUT_OF_RESOURCES Not enough space for "add" operation
    @retval EFI_PROTOCOL_ERROR Error - Unsupported operation
    @retval EFI_SUCCESS Function success

    @remark Dynamic table sizing can only be done when SMBIOS table is at\n
            high memory. At E000, table can be expanded up to the padding size.\n
            This limitation is due to CSM can only allocate memory, but it does\n
            not have memory deallocation feature.
**/
EFI_STATUS
UpdateSmbiosTable(
    IN SMBIOS_OPERATION     Operation,
    IN UINT8                *StructurePtr,
    IN UINT8                *Buffer,
    IN UINT16               Size,
    IN UINT16               Handle
)
{
    EFI_STATUS              Status = -1;
    EFI_STATUS              TableCopyStatus = -1;
    UINT8                   *SmbiosTableCopy;
    UINT8                   *AllocatedMemory = NULL;
    UINT8                   *MemoryPtr;
    UINT8                   *TempPtr;
    UINT8                   *TempPtr2;
    UINT8                   *Type127;
    UINT16                  TableLength;
    UINT16                  Length;
    UINT16                  StructureSize;

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    UINT32  Granularity;

    if ((LegacyRegionProtocol != NULL) && SmbiosTableAtE000) {
        LegacyRegionProtocol->UnLock (LegacyRegionProtocol,
                                      (UINT32)(UINTN)SmbiosDataTable,
                                      MaximumTableSize,
                                      &Granularity);
    }
#endif

    DEBUG((DEBUG_INFO, "***  SMBIOS - UpdateSmbiosTable  ***\n"));
    DEBUG((DEBUG_INFO, "Operation = %x, Size = %x, Handle = %x\n", Operation, Size, Handle));

    switch (Operation) {
        case ADD_STRUCTURE:
        case ADD_STRUCTURE_BY_INDEX:
                DEBUG((DEBUG_INFO, "Adding structure\n"));
                if (Buffer == NULL) break;
                if ((Handle != 0xfffe) && (Handle != 0xffff)) {
                    // Check for existing handle
                    TempPtr = SmbiosDataTable;
                    if (FindStructureHandle(&TempPtr, Handle)) {
                        Status = EFI_ALREADY_STARTED;
                        break;
                    }
                }

                // Check input buffer for valid structure
                Status = CheckForValidStructure(Buffer, Size);
                if (Status != EFI_SUCCESS) {
                    Status = EFI_PROTOCOL_ERROR;
                    break;
                }

                // Work around hanging case when Type 4 structure having header
                // size equal to 0x2a but its actual size is 0x30 (Smbios 3.0 change)
                if ((*Buffer == 4) && (*(Buffer + 1) == 0x2a)) {
                    if (*(Buffer + 4) || *(Buffer + 7) || *(Buffer + 0x10) ||\
                        *(Buffer + 0x20) || *(Buffer + 0x21) || *(Buffer + 0x22)) {
                        if ((*(UINT32*)(Buffer + 0x2a) == 0) && \
                            (*(UINT16*)(Buffer + 0x2e) == 0)) {
                            MemCpy(Buffer + 0x2a, Buffer + 0x30, Size - 0x30);
                            Size -= 6;
                        }
                    }
                }

#if (CONVERT_TYPE4_V2X_TO_V3X == 1)
                if ((((SMBIOS_STRUCTURE_HEADER*)Buffer)->Type == 0x04) && (((SMBIOS_STRUCTURE_HEADER*)Buffer)->Length == 0x2a)) {
                    DEBUG((DEBUG_INFO, "Converting Type 4 structure to Version 3.0\n"));
                    ConverType4ToV30(&Buffer);
                    Size += 6;
                }
#endif

                // Make copy of the original Smbios Table
                TableCopyStatus = pBS->AllocatePool(
                                            EfiBootServicesData,
                                            MaximumTableSize,
                                            (void**)&SmbiosTableCopy
                                            );
                if (EFI_ERROR(TableCopyStatus)) {
                    Status = EFI_OUT_OF_RESOURCES;
                    break;
                }

                // TempPtr = pointer to the original Smbios table
                TempPtr = SmbiosTableCopy;
                MemCpy(TempPtr, SmbiosDataTable, MaximumTableSize);

                // Check for enough space
                TableLength = GetTotalStructureSize(SmbiosDataTable);
                if (Size > (MaximumTableSize - TableLength)) {
                    DEBUG((DEBUG_INFO, "Not enough space\n"));
                    if (SmbiosTableAtE000) {
                        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Not enough in E000\n"));
                        Status =  EFI_OUT_OF_RESOURCES;
                        break;
                    }
                    else {
                    	if (BeforeEndofDxe) {
							DEBUG((DEBUG_INFO, "Allocating memory for new table\n"));
							// Allocate memory to store new Smbios table
							TableLength = MaximumTableSize + Size;
							AllocatedMemory = (UINT8*)0xFFFFFFFF;
							Status = pBS->AllocatePages(
												AllocateMaxAddress,
												EfiRuntimeServicesData,
												EFI_SIZE_TO_PAGES(TableLength),
												(EFI_PHYSICAL_ADDRESS*)&AllocatedMemory
												);
							if (EFI_ERROR(Status)) {
								DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to allocate memory\n"));
								Status = EFI_OUT_OF_RESOURCES;
								break;
							}

							// Copy current Smbios table to the new location
							MemCpy(AllocatedMemory, SmbiosDataTable, MaximumTableSize);

							MemoryPtr = AllocatedMemory;
							DEBUG((DEBUG_INFO, "Memory allocated = %x\n", MemoryPtr));
                    	}
                    	else {
                    		DEBUG((DEBUG_ERROR | DEBUG_WARN, "Can not expand Smbios Table after end of DXE\nTry increase EXTRA_RESERVED_BYTES SDL token value to reserve more space for Smbios Table to grow\n"));
							Status = EFI_OUT_OF_RESOURCES;
							break;
                    	}
                    }
                }
                else {
                    MemoryPtr = SmbiosDataTable;
                }

                if (Operation == ADD_STRUCTURE_BY_INDEX) {
                    UINT16          Index = 0;

                    while (((SMBIOS_STRUCTURE_HEADER*)MemoryPtr)->Type != 127) {
                        Length = GetStructureLength(MemoryPtr);
                        if (*(UINT16*)StructurePtr == Index++) break;
                        // Move pointer in the new Smbios table
                        MemoryPtr += Length;
                        // Move pointer in the original Smbios table
                        TempPtr += Length;
                    }
                }
                else {
                    if (ADD_STRUCTURE_LOCATION == 1) {
                        // Advance the pointers to the first structure whose handle number
                        // is larger than the input handle. In case input handle is FFFF,
                        // move the pointers to the last structure (Type 127).
                        while (((SMBIOS_STRUCTURE_HEADER*)MemoryPtr)->Type != 127) {
                            if (((SMBIOS_STRUCTURE_HEADER*)MemoryPtr)->Handle > Handle) {
                                break;
                            }

                            Length = GetStructureLength(MemoryPtr);

                            // Move pointer in the new Smbios table
                            MemoryPtr += Length;

                            // Move pointer in the original Smbios table
                            TempPtr += Length;
                        }
                    }
                    else {
                        TempPtr2 = TempPtr;
                        FindStructureType(&TempPtr2, &TempPtr, 127, 1);
                        MemoryPtr += TempPtr - SmbiosTableCopy;
                    }
                }

                if ((Handle == 0xfffe) || (Handle == 0xffff)) {
                    // Assign the LastHandle to the newly added structure
                    ((SMBIOS_STRUCTURE_HEADER*)Buffer)->Handle = LastHandle;
                }
                else {
                    ((SMBIOS_STRUCTURE_HEADER*)Buffer)->Handle = Handle;

                    if (Handle > LastHandle) {
                        LastHandle = Handle;
                    }
                }

                // Insert input structure
                MemCpy(MemoryPtr, Buffer, Size);
                MemoryPtr += Size;

                // Copy remaining structures
                Length = GetRemainingStructuresSize(TempPtr);
                MemCpy(MemoryPtr, TempPtr, Length);

                // Update Type 127 handle if needed
                FindStructureType(&MemoryPtr, &Type127, 127, 1);
                if (LastHandle >= ((SMBIOS_STRUCTURE_HEADER*)Type127)->Handle) {
                    ((SMBIOS_STRUCTURE_HEADER*)Type127)->Handle = ++LastHandle;
                }

                DEBUG((DEBUG_INFO, "Done adding structure\n"));
                Status = EFI_SUCCESS;
                break;

        case DELETE_STRUCTURE:
                DEBUG((DEBUG_INFO, "Deleting structure\n"));
                // Clear UpdateCpuStructure flag if CPU (type 4) structure is being deleted
                if (((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Type == 4) {
                    UpdateCpuStructure = FALSE;
                }

                // StructurePtr = Pointer to structure to be deleted
                StructureSize = GetStructureLength(StructurePtr);
                Length = GetTotalStructureSize(StructurePtr) - StructureSize;
                TempPtr = StructurePtr + StructureSize;
                MemCpy(StructurePtr, TempPtr, Length);

                // Clean up
                TempPtr = StructurePtr + Length;
                MemSet(TempPtr, StructureSize, 0xff);

                Status = EFI_SUCCESS;
                DEBUG((DEBUG_INFO, "Done deleting\n"));
                break;

        default:
                TableCopyStatus = EFI_PROTOCOL_ERROR;
                Status = EFI_PROTOCOL_ERROR;
    }

    if (TableCopyStatus == EFI_SUCCESS) {
        pBS->FreePool(SmbiosTableCopy);
    }

    DEBUG((DEBUG_INFO, "Status = %r\n", Status));
    if (Status == EFI_OUT_OF_RESOURCES) {
    	// Assert if there is not enough space to add structure
        ASSERT_EFI_ERROR(Status);
    }
    else if (Status == EFI_SUCCESS) {
        if (!SmbiosTableAtE000) {
            if (AllocatedMemory) {
                DEBUG((DEBUG_INFO, "Freeing previous table memory\n"));
                pBS->FreePages((EFI_PHYSICAL_ADDRESS)SmbiosDataTable, EFI_SIZE_TO_PAGES(MaximumTableSize));

                SmbiosDataTable = AllocatedMemory;
                MaximumTableSize = TableLength;
            }
        }

        UpdateSmbiosTableHeader();
    }

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    if ((LegacyRegionProtocol != NULL) && SmbiosTableAtE000) {
        LegacyRegionProtocol->Lock (LegacyRegionProtocol,
                                    (UINT32)(UINTN)SmbiosDataTable,
                                    MaximumTableSize,
                                    &Granularity);
    }
#endif

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit UpdateSmbiosTable. Status = %r  :::\n", Status));
    return Status;
}

/**
    SMBIOS protocol - Searches for available handle in Smbios Data Table

    @param None

    @return Handle number or -1(if not found)

**/
UINT16
GetFreeHandle(VOID)
{
    EFI_STATUS    FoundStatus;

    UINT16        Handle = 0;
    UINT8         *StructurePtr;

    // Count Handle form 0 to 0xFFFF to find available Handle
    for (Handle = 0; Handle < 0xffff; Handle++) {
        StructurePtr = SmbiosDataTable;
        FoundStatus = FindStructureHandle(&StructurePtr, Handle);
        if (!FoundStatus) {
            return Handle;
        }
    }

    return -1; // No available Handle to use;
}

/**
    Update input Type 4 (CPU structure) buffer with DMIEdit data
    and add to Smbios table

    @param AddOperationType         // ADD_STRUCTURE, ADD_STRUCTURE_BY_INDEX
    @param Index                    // structure index in Smbios table
    @param Handle
    @param Buffer
    @param Size

    @return Updated Buffer and Size

**/
EFI_STATUS
AddType4WithDmiEditData(
    IN UINT8    AddOperationType,
    IN UINT8    *Index,
    IN UINT16   Handle,
    IN UINT8    *Buffer,
    IN UINT16   Size
)
{
    EFI_STATUS  Status;
    UINT8       *PoolMemory;
    UINT16      StructureSize;

    // Allocate 1K memory
    pBS->AllocatePool(
                EfiBootServicesData,
                Size + 0x400,
                (void**)&PoolMemory);
    MemSet(PoolMemory, Size + 0x400, 0);
    MemCpy(PoolMemory, Buffer, Size);

    // Replace Serial Number, Asset Tag, or Part Number strings
    // with DMIEdit strings if exist
    UpdateStrings((SMBIOS_NVRAM_TYPE*)&NvramType4, PoolMemory, 1);
    StructureSize = GetStructureLength(PoolMemory);

    Status = UpdateSmbiosTable(AddOperationType,
                               Index,
                               PoolMemory,
                               StructureSize,
                               Handle
                               );

    pBS->FreePool(PoolMemory);

    return Status;
}

/**
    SMBIOS protocol - Add new structure

    @param Buffer
    @param Size

    @retval EFI_OUT_OF_RESOURCES    Not enough space
    @retval EFI_PROTOCOL_ERROR      Invalid input structure or protocol error
    @retval EFI_SUCCESS             Function successfully executed\n
                                    Buffer->Handle = Assigned handle number

**/
EFI_STATUS
AddStructure(
    IN UINT8        *Buffer,
    IN UINT16       Size
)
{
    EFI_STATUS      Status;

    DEBUG((DEBUG_INFO, "***  SMBIOS - AddStructure  ***\n"));

    if ((((SMBIOS_STRUCTURE_HEADER*)Buffer)->Type == 4) &&
        (NvramType4.Flag & 0x3f)) {
        // Adding Type 4 structure and with DMIEdit data for
        // Serial Number, or Asset Tag, or Part Number fields
        Status = AddType4WithDmiEditData(ADD_STRUCTURE,
                                         NULL,
                                         0xFFFE,
                                         Buffer,
                                         Size
                                         );
    }
    else {
        Status = UpdateSmbiosTable(ADD_STRUCTURE,
                                   NULL,
                                   Buffer,
                                   Size,
                                   0xFFFE       // Assign next handle number
                                   );
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit AddStructure. Status = %r  :::\n", Status));
    return Status;
}

/**
    SMBIOS protocol - Add new structure at a specific index

    @param Handle
    @param Buffer
    @param Size
    @param Index (zero based)

    @retval EFI_ALREADY_STARTED     Input Handle already exists
    @retval EFI_PROTOCOL_ERROR      Invalid input structure or protocol error
    @retval EFI_OUT_OF_RESOURCES    Not enough space
    @retval EFI_SUCCESS             Function successfully executed\n
                                    Buffer->Handle = Assigned handle number

**/
EFI_STATUS
AddStructureByIndex(
    IN UINT16       Handle,
    IN UINT8        *Buffer,
    IN UINT16       Size,
    IN UINT16       Index
)
{
    EFI_STATUS      Status;

    DEBUG((DEBUG_INFO, "***  SMBIOS - AddStructureByIndex  ***\n"));

    if ((((SMBIOS_STRUCTURE_HEADER*)Buffer)->Type == 4) &&
        (NvramType4.Flag & 0x3f)) {
        // Adding Type 4 structure and with DMIEdit data for
        // Serial Number, or Asset Tag, or Part Number fields
        Status = AddType4WithDmiEditData(ADD_STRUCTURE_BY_INDEX,
                                         (UINT8*)&Index,
                                         Handle,
                                         Buffer,
                                         Size
                                         );
    }
    else {
        Status = UpdateSmbiosTable(ADD_STRUCTURE_BY_INDEX,
                                   (UINT8*)&Index,
                                   Buffer,
                                   Size,
                                   Handle
                                   );
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit AddStructureByIndex. Status = %r  :::\n", Status));
    return Status;
}

/**
    SMBIOS protocol - Add new structure with a specific handle -
    structure will not be added if another structure with this
    handle already existed.

    @param Handle
    @param Buffer
    @param Size

    @retval EFI_ALREADY_STARTED     Input Handle already exists
    @retval EFI_PROTOCOL_ERROR      Invalid input structure or protocol error
    @retval EFI_OUT_OF_RESOURCES    Not enough space
    @retval EFI_SUCCESS             Function successfully executed

**/
EFI_STATUS
AddStructureByHandle(
    IN UINT16       Handle,
    IN UINT8        *Buffer,
    IN UINT16       Size
)
{
    EFI_STATUS      Status;

    DEBUG((DEBUG_INFO, "***  SMBIOS - AddStructureByHandle  ***\n"));

    if ((((SMBIOS_STRUCTURE_HEADER*)Buffer)->Type == 4) &&
        (NvramType4.Flag & 0x3f)) {
        // Adding Type 4 structure and with DMIEdit data for
        // Serial Number, or Asset Tag, or Part Number fields
        Status = AddType4WithDmiEditData(ADD_STRUCTURE,
                                         NULL,
                                         Handle,
                                         Buffer,
                                         Size
                                         );
    }
    else {
        Status =  UpdateSmbiosTable(ADD_STRUCTURE,
                                    NULL,
                                    Buffer,
                                    Size,
                                    Handle
                                    );
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit AddStructureByHandle. Status = %r  :::\n", Status));
    return Status;
}

/**
    SMBIOS protocol - Searches and deletes structure by handle

    @param Handle

    @retval EFI_SUCCESS         Function successfully executed
    @retval EFI_PROTOCOL_ERROR  Error

**/
EFI_STATUS
DeleteStructureByHandle(
    IN UINT16       Handle
)
{
    EFI_STATUS      Status;
    UINT8           *StructurePtr = SmbiosDataTable;

    DEBUG((DEBUG_INFO, "***  SMBIOS - DeleteStructureByHandle. Handle = %x  ***\n", Handle));

    if (FindStructureHandle(&StructurePtr, Handle)) {
        DEBUG((DEBUG_INFO, "Deleting structure type %x\n", ((SMBIOS_STRUCTURE_HEADER*)StructurePtr)->Type));
        Status = UpdateSmbiosTable(
                        DELETE_STRUCTURE,
                        StructurePtr,
                        0,
                        0,
                        0
                        );
    }
    else {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Structure not found\n"));
        Status = EFI_NOT_FOUND;
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit DeleteStructureByHandle. Status = %r  :::\n", Status));
    return Status;
}

/**
    SMBIOS protocol - Searches for a structure with input handle,
    and return a copy of this structure in BufferPtr if found.

    @param Handle
    @param BufferPtr
    @param BufferSize

    @retval EFI_SUCCESS             Function successfully executed
    @retval EFI_OUT_OF_RESOURCES    Not enough memory to allocate for output buffer
    @retval EFI_INVALID_PARAMETER   Structure not found

    @remark Memory will be allocated for the returning structure if\n
            structure with input handle is found. Caller is responsible\n
            for freeing this memory when done with it.
**/
EFI_STATUS
ReadStructureByHandle(
    IN      UINT16            Handle,
    IN OUT  UINT8             **BufferPtr,
    IN OUT  UINT16            *BufferSize
)
{
    EFI_STATUS    Status;
    UINT8         *StructurePtr = SmbiosDataTable;

    DEBUG((DEBUG_INFO, "***  SMBIOS - ReadStructureByHandle  ***\n"));

    if (FindStructureHandle(&StructurePtr, Handle)) {
        *BufferSize = GetStructureLength(StructurePtr);
        Status = pBS->AllocatePool(EfiBootServicesData, *BufferSize, (void**)BufferPtr);
        if (Status != EFI_SUCCESS) {
            *BufferPtr = NULL;
            *BufferSize = 0;

            DEBUG((DEBUG_ERROR | DEBUG_WARN, "Memory allocation failed. Exit\n"));
            return  EFI_OUT_OF_RESOURCES;
        }
        MemCpy(*BufferPtr, StructurePtr, *BufferSize);

        DEBUG((DEBUG_INFO, "Exit ReadStructureByHandle\n"));
        return  EFI_SUCCESS;
    }
    else {
        *BufferPtr = NULL;
        *BufferSize = 0;

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Structure not found. Exit\n"));
        return  EFI_INVALID_PARAMETER;
    }
}

/**
    SMBIOS protocol - Searches for the (n)th structure of input
    Type and return a copy of this structure in BufferPtr if found.

    @param Type
    @param Instance
    @param BufferPtr
    @param BufferSize

    @retval EFI_SUCCESS             Function successfully executed
    @retval EFI_OUT_OF_RESOURCES    Not enough memory to allocate for output buffer
    @retval EFI_INVALID_PARAMETER   Structure not found

    @remark Memory will be allocated for the returning structure if\n
            structure with input handle is found. Caller is responsible\n
            for freeing this memory when done with it.
**/
EFI_STATUS
ReadStructureByType(
    IN      UINT8           Type,
    IN      UINT8           Instance,
    IN OUT  UINT8           **BufferPtr,
    IN OUT  UINT16          *BufferSize
)
{
    EFI_STATUS      Status;
    UINT8           *StructurePtr = SmbiosDataTable;
    UINT8           *FoundStructPtr;

    DEBUG((DEBUG_INFO, "***  SMBIOS - ReadStructureByType  ***\n"));

    if (FindStructureType(&StructurePtr, &FoundStructPtr, Type, Instance)) {
        *BufferSize = GetStructureLength(FoundStructPtr);
        Status = pBS->AllocatePool(EfiBootServicesData, *BufferSize, (void**)BufferPtr);
        if (Status != EFI_SUCCESS) {
            *BufferPtr = NULL;
            *BufferSize = 0;

            DEBUG((DEBUG_ERROR | DEBUG_WARN, "Memory allocation failed. Exit\n"));
            return  EFI_OUT_OF_RESOURCES;
        }
        MemCpy(*BufferPtr, FoundStructPtr, *BufferSize);

        DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit ReadStructureByType  :::\n"));
        return  EFI_SUCCESS;
    }
    else {
        *BufferPtr = NULL;
        *BufferSize = 0;

        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Structure type %x not found\n", Type));
        return  EFI_INVALID_PARAMETER;
    }
}

/**
    SMBIOS protocol - Overwrite an existing structure by handle

    @param Handle
    @param BufferPtr
    @param BufferSize

    @retval EFI_PROTOCOL_ERROR      Invalid input structure
    @retval EFI_OUT_OF_RESOURCES    Not enough space
    @retval EFI_SUCCESS             Function successfully executed

**/
EFI_STATUS
WriteStructureByHandle(
    IN UINT16   Handle,
    IN UINT8    *BufferPtr,
    IN UINT16   BufferSize
)
{
    EFI_STATUS    Status;

#if WRITE_STRUCTURE_HANDLE_POLICY == 0

    DEBUG((DEBUG_INFO, "***  SMBIOS - WriteStructureByHandle  ***\n"));

    Status = DeleteStructureByHandle(Handle);
    if (!EFI_ERROR(Status)) {
        if ((((SMBIOS_STRUCTURE_HEADER*)Buffer)->Type == 4) &&
            (NvramType4.Flag & 0x3f)) {
            // Adding Type 4 structure and with DMIEdit data for
            // Serial Number, or Asset Tag, or Part Number fields
            Status = AddType4WithDmiEditData(ADD_STRUCTURE,
                                             NULL,
                                             Handle,
                                             Buffer,
                                             Size
                                             );
        }
        else {
            Status = UpdateSmbiosTable(ADD_STRUCTURE,
                                       NULL,
                                       BufferPtr,
                                       BufferSize,
                                       Handle
                                       );
        }
    }

    DEBUG((DEBUG_INFO, "::: SMBIOS - Exit WriteStructureByHandle. Status = %r  :::\n", Status));
    return Status;

#else

    UINT8       *SmbiosTableCopy;
    UINT8       *TempPtr;
    UINT8       *MemoryPtr;
    UINT16      Size;
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    UINT32      Granularity;
#endif                                          // CSM_SUPPORT

    DEBUG((DEBUG_INFO, "***  SMBIOS - WriteStructureByHandle  ***\n"));

    // Check input buffer for valid structure according to its size
    Status = CheckForValidStructure(BufferPtr, BufferSize);
    if (Status != EFI_SUCCESS) {
        return EFI_PROTOCOL_ERROR;
    }

    MemoryPtr = SmbiosDataTable;
    if (FindStructureHandle(&MemoryPtr, Handle)) {
        // Check for enough space
        Size = GetTotalStructureSize(SmbiosDataTable) - GetStructureLength(MemoryPtr);
        if (BufferSize > (MaximumTableSize - Size)) {
            if (SmbiosTableAtE000) {
                // Table can not grow when in E000, so exit
                DEBUG((DEBUG_ERROR | DEBUG_WARN, "Not enough space in E000. Exit\n"));
                return  EFI_OUT_OF_RESOURCES;
            }
            else {
                Status = DeleteStructureByHandle(Handle);
                if (EFI_ERROR(Status)) {
                    DEBUG((DEBUG_ERROR | DEBUG_WARN, "Cannot delete handle %x. Exit\n", Handle));
                    return EFI_OUT_OF_RESOURCES;
                }
            }
        }
    }

    // Make copy of the original Smbios Table
    Status = pBS->AllocatePool(
                            EfiBootServicesData,
                            MaximumTableSize,
                            (void**)&SmbiosTableCopy
                            );
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Memory allocation failed. Exit\n"));
        return EFI_OUT_OF_RESOURCES;
    }

    // TempPtr = pointer to copy of original Smbios table
    TempPtr = SmbiosTableCopy;
    MemCpy(TempPtr, SmbiosDataTable, MaximumTableSize);

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    if ((LegacyRegionProtocol != NULL) && SmbiosTableAtE000) {
        LegacyRegionProtocol->UnLock (LegacyRegionProtocol,
                                      (UINT32)(UINTN)SmbiosDataTable,
                                      MaximumTableSize,
                                      &Granularity);
    }
#endif                                          // CSM_SUPPORT

    MemoryPtr = SmbiosDataTable;

    if (FindStructureHandle(&MemoryPtr, Handle)) {
        MemCpy(MemoryPtr, BufferPtr, BufferSize);
        TempPtr += MemoryPtr - SmbiosDataTable;
        TempPtr += GetStructureLength(TempPtr);
        MemoryPtr += GetStructureLength(MemoryPtr);

        // Copy remaining structures
        Size = GetRemainingStructuresSize(TempPtr);
        MemCpy(MemoryPtr, TempPtr, Size);
    }
    else {
        AddStructureByHandle(Handle, BufferPtr, BufferSize);
    }

    pBS->FreePool(SmbiosTableCopy);

    UpdateSmbiosTableHeader();

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    if ((LegacyRegionProtocol != NULL) && SmbiosTableAtE000) {
        LegacyRegionProtocol->Lock (LegacyRegionProtocol,
                                    (UINT32)(UINTN)SmbiosDataTable,
                                    MaximumTableSize,
                                    &Granularity);
    }
#endif                                          // CSM_SUPPORT

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit WriteStructureByHandle  :::\n"));
    return EFI_SUCCESS;

#endif                                          // WRITE_STRUCTURE_HANDLE_POLICY
}

/**
    Searches for CSM16 Table in F000 segment

    @param Csm16Table

    @retval EFI_COMPATIBILITY16_TABLE Csm16Table
    @retval EFI_STATUS Status
**/
#if (defined(ITK_SUPPORT) && (ITK_SUPPORT != 0)) || (SMBIOS_TABLE_LOCATION)
#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))
EFI_STATUS FindCsm16Table(OUT EFI_COMPATIBILITY16_TABLE** Csm16Table){

    UINT32* PtrCsmTable = (UINT32*)0xe0000;

    for( ; PtrCsmTable < (UINT32*)0x100000; PtrCsmTable+=4) {
        if(*PtrCsmTable == (UINT32)'$EFI'){
            *Csm16Table = (EFI_COMPATIBILITY16_TABLE*)PtrCsmTable;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}
#endif                                          // CSM_SUPPORT
#endif                                          // ITK_SUPPORT || SMBIOS_TABLE_LOCATION

/**
    Allocate memory for SMBIOS table in low memory

    @param TablePtr
    @param Size

    @return EFI_STATUS
            If success, TablePtr points to allocated memory in E000 segment.

**/
#if (defined(ITK_SUPPORT) && (ITK_SUPPORT != 0)) || (SMBIOS_TABLE_LOCATION)
#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))
EFI_STATUS
SmbiosE000Alloc(
    IN OUT  UINT8       **TablePtr,
    IN      UINT16      Size
)
{
    EFI_STATUS                  Status;
    EFI_IA32_REGISTER_SET       Registers;
    EFI_COMPATIBILITY16_TABLE*  Csm16Table;
    UINT32                      Granularity;

    DEBUG((DEBUG_INFO, ":::  SMBIOS - SmbiosE000Alloc  :::\n"));

    if ((LegacyBiosProtocol != NULL) && (LegacyRegionProtocol != NULL)) {
        MemSet(&Registers, sizeof (EFI_IA32_REGISTER_SET), 0);

        Status = FindCsm16Table(&Csm16Table);
        if (EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR | DEBUG_WARN, "Csm16 Table not found. Exit\n"));
            return Status;
        }

        Status = LegacyRegionProtocol->UnLock (LegacyRegionProtocol,
                                               0xe0000,
                                               0x20000,
                                               &Granularity);

        // In case E000-F000 fails to unlock then return error without allocating memory
        if (EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to unlock shadow. Exit\n"));
            return Status;
        }

        Registers.X.AX = Legacy16GetTableAddress;
        Registers.X.BX = E0000_BIT;         // allocate from 0xE0000 64 KB block
        Registers.X.CX = (UINT16)Size;
        Registers.X.DX = 1;                 // alignment

        Status = LegacyBiosProtocol->FarCall86(LegacyBiosProtocol,
                                               Csm16Table->Compatibility16CallSegment,
                                               Csm16Table->Compatibility16CallOffset,
                                               &Registers,
                                               0,
                                               0);

        if (Registers.X.AX) Status = (Registers.X.AX & 0x7fff) | EFI_ERROR_BIT;

        if (EFI_ERROR(Status)) {			// E000 allocation failed
        	DEBUG((DEBUG_ERROR | DEBUG_WARN, "CSM16 failed to allocate E000 memory\n"));
        }
        else {           					// E000 allocation success
            *TablePtr = (UINT8*)(((UINTN)Registers.X.DS << 4) + Registers.X.BX);
        }

        LegacyRegionProtocol->Lock (LegacyRegionProtocol,
                                    0xe0000,
                                    0x20000,
                                    &Granularity);
    }
    else {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "No LegacyBiosProtocol. Exit\n"));
        Status = EFI_PROTOCOL_ERROR;
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit SmbiosE000Alloc. Status = %r  :::\n", Status));
    return Status;
}
#endif                                          // CSM_SUPPORT
#endif                                          // ITK_SUPPORT || SMBIOS_TABLE_LOCATION

#if BIOS_LANGUAGE_INFO
/**
    Add or update BIOS Language Information structure type 13

    @param None

    @retval None

**/
VOID
AddUpdateType13 (VOID)
{
    EFI_STATUS      Status;
    UINT8           i;
    UINT8           j;
    UINT8           StrNumber = 0;
    UINT8           *SmbiosTablePtr = SmbiosDataTable;
    UINT8           *TempPtr;
    UINT8           *BufferPtr;
    UINT16          BufferSize;
    UINTN           Size;
    EFI_GUID        guidEfiVar = EFI_GLOBAL_VARIABLE;

//----------------------------------------------------------------------------
//                  ***** BIOS LANGUAGE TYPE 13 Tables *****
//            (Table entries should cover the languages specified
//             in Languages.mak)
//----------------------------------------------------------------------------
    #define LANGUAGESIZE 8
    CHAR8   SupportLang[][LANGUAGESIZE] = {
                                "en-US",        // English
                                "fr-FR",        // French
                                "es-ES",        // Spanish
                                "de-DE",        // German
                                "ru-RU",        // Russian
                                "ko-KR",        // Korean
                                "it-IT",        // Italian
                                "da-DK",        // Danish
                                "fi-FI",        // Finnish
                                "nl-NL",        // Dutch
                                "nb-NO",        // Norweigan
                                "nn-NO",        // Norweigan
                                "pt-PT",        // Portuguese
                                "pt-BR",        // Portuguese
                                "sv-SE",        // Swedish
                                "ja-JP",        // Japansese
                                "zh-chs",       // Chinese simplified
                                "zh-cht",       // Chinese traditional
                                "zh-Hans",      // Chinese simplified
                                "zh-Hant",      // Chinese traditional
                             };
#if BIOS_LANGUAGE_FORMAT
    CHAR8   LangShort[][4] = {  "enUS",
                                "frFR",
                                "esES",
                                "deDE",
                                "ruRU",
                                "koKR",
                                "itIT",
                                "daDK",
                                "fiFI",
                                "nlNL",
                                "nbNO",
                                "nnNO",
                                "ptPT",
                                "ptBR",
                                "svSE",
                                "jaJP",
                                "zhCN",
                                "zhTW",
                                "zhCN",
                                "zhTW",
                             };
#else
    CHAR8   LangLong[][16] = {  "en|US|iso8859-1\0",
                                "fr|FR|iso8859-1\0",
                                "es|ES|iso8859-1\0",
                                "de|DE|iso8859-1\0",
                                "ru|RU|iso8859-5\0",
                                "ko|KR|unicode\0",
                                "it|IT|iso8859-1\0",
                                "da|DK|iso8859-1\0",
                                "fi|FI|iso8859-1\0",
                                "nl|NL|iso8859-1\0",
                                "nb|NO|iso8859-1\0",
                                "nn|NO|iso8859-1\0",
                                "pt|PT|iso8859-1\0",
                                "pt|BR|iso8859-1\0",
                                "sv|SE|iso8859-1\0",
                                "ja|JP|unicode\0",
                                "zh|CN|unicode\0",
                                "zh|TW|unicode\0",
                                "zh|CN|unicode\0",
                                "zh|TW|unicode\0",
                             };
#endif                                          // BIOS_LANGUAGE_FORMAT

    CHAR8           *Lang = NULL;
    CHAR8           *LangPtr;
    CHAR8           *LangCodes = NULL;
    CHAR8           *LangCodesPtr;
    UINT8           LangCounts = 1;
//----------------------------------------------------------------------------

    DEBUG((DEBUG_INFO, "***  SMBIOS - AddUpdateType13  ***\n"));

    Status = GetEfiVariable(L"PlatformLangCodes",
                            &guidEfiVar,
                            NULL,
                            &Size,
                            (void**)&LangCodes);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get PlatformLangCodes variable. Exit\n"));
        return;
    }
    LangCodesPtr = LangCodes;

    for (i = 0; i < Size; i++) {
        if (LangCodes[i] == ';') {
            LangCounts++;
        }
    }

    Status = GetEfiVariable(L"PlatformLang",
                            &guidEfiVar,
                            NULL,
                            &Size,
                            (void**)&Lang);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to get PlatformLang variable. Exit\n"));
        pBS->FreePool(LangCodes);
        return;
    }
    LangPtr = Lang;

    BufferSize = 1024;

    Status = pBS->AllocatePool(EfiBootServicesData, BufferSize, (void**)&BufferPtr);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Memory allocation failed. Exit\n"));
        pBS->FreePool(LangCodes);
        pBS->FreePool(Lang);
        return;
    }

    pBS->SetMem(BufferPtr, BufferSize, 0);

    ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type = 13;
    ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Length = sizeof(SMBIOS_BIOS_LANG_INFO);
    ((SMBIOS_BIOS_LANG_INFO*)BufferPtr)->InstallableLang = LangCounts;
    ((SMBIOS_BIOS_LANG_INFO*)BufferPtr)->Flags = BIOS_LANGUAGE_FORMAT;

    TempPtr = BufferPtr + sizeof(SMBIOS_BIOS_LANG_INFO);

    for (i = 0; i < LangCounts; i++) {
        CHAR8   CodeString[20];
        CHAR8   *CodesStringPtr = CodeString;

        pBS->SetMem(CodesStringPtr, 20, 0);

        while (*LangCodesPtr) {
            if (*LangCodesPtr != ';') {
                *CodesStringPtr++ = *LangCodesPtr++;
            }
            else {
                LangCodesPtr++;
                break;
            }
        }

        for (j = 0; j < sizeof(SupportLang)/LANGUAGESIZE; j++) {
            if (!Strcmp(&SupportLang[j][0], CodeString)) {
                StrNumber++;

#if BIOS_LANGUAGE_FORMAT
                Size = 4;
                MemCpy(TempPtr, &LangShort[j][0], Size);
#else
                Size = Strlen(&LangLong[j][0]);
                MemCpy(TempPtr, &LangLong[j][0], Size);
#endif                                          // BIOS_LANGUAGE_FORMAT
                *(TempPtr + Size) = 0;          // string termination

                TempPtr += Size + 1;
            }

            if (!Strcmp(CodeString, Lang)) {
                ((SMBIOS_BIOS_LANG_INFO*)BufferPtr)->CurrentLang = StrNumber;
            }
        }
    }

    *TempPtr = 0;                               // End of structure

    BufferSize = GetStructureLength(BufferPtr);

    if (FindStructureType(&SmbiosTablePtr, &TempPtr, 13, 1)) {
        ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle = \
                        ((SMBIOS_STRUCTURE_HEADER*)TempPtr)->Handle;

        // Type 13 exists, overwrite it
        WriteStructureByHandle(
                        ((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Handle,
                        BufferPtr,
                        BufferSize);
    }
    else {
        // Type 13 not found, add a new one
        AddStructure(BufferPtr, BufferSize);
    }

    pBS->FreePool(LangCodes);
    pBS->FreePool(Lang);
    pBS->FreePool(BufferPtr);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit AddUpdateType13  :::\n"));
}
#endif

#if SORT_SMBIOS_TABLE_BY_TYPE
/**
    Sort the Smbios data table by structure types

    @param None

    @retval None

**/
VOID
SortSmbiosTableByType(VOID)
{
    UINT8       *SourcePtr;
    UINT8       *DestinationPtr;
    UINT8       *FoundPtr;
    UINT8       Instance;
    UINT16      Type;
    UINT16      Size;

    DEBUG((DEBUG_INFO, "***  SMBIOS - SortSmbiosTableByType  ***\n"));

    MemCpy(ScratchBufferPtr, SmbiosDataTable, MaximumTableSize);
    DestinationPtr = SmbiosDataTable;

    for (Type = 0; Type < 256; Type++) {
        if (Type == 127) continue;              // Skip checking for type 127
        for (Instance = 1;; Instance++) {
            SourcePtr = ScratchBufferPtr;
            if (FindStructureType(&SourcePtr, &FoundPtr, (UINT8)Type, Instance)) {
                Size = GetStructureLength(FoundPtr);
                MemCpy(DestinationPtr, FoundPtr, Size);
                DestinationPtr += Size;
            }
            else {
                break;
            }
        }
    }

    // Copy type 127
    SourcePtr = ScratchBufferPtr;
    if (FindStructureType(&SourcePtr, &FoundPtr, 127, 1)) {
        MemCpy(DestinationPtr, FoundPtr, 6);
    }

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit SortSmbiosTableByType  :::\n"));
}
#endif                                          // SORT_SMBIOS_TABLE_BY_TYPE

/**
    Update Processor Information, System Slots, and On-Board
    Devices Information structures.

    @param Event
    @param Context

    @retval None

**/
VOID
SmbiosDynamicUpdate (
    IN EFI_EVENT      Event,
    IN VOID           *Context
)
{
    EFI_STATUS                  Status;

    DEBUG((DEBUG_INFO, "*** SmbiosDynamicUpdate Entry ***\n"));

    pBS->CloseEvent(Event);

#if SMBIOS_DYNAMIC_UPDATE_POLICY == 0
#if (TYPE2_STRUCTURE && UPDATE_BASEBOARD_TYPE2)
    Status = gSmbiosBoardProtocol->SmbiosCreateBaseBoardData();
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateBaseBoardData failed\n"));
    }
#endif                                          // TYPE2_STRUCTURE && UPDATE_BASEBOARD_TYPE2
#if (TYPE3_STRUCTURE && UPDATE_SYSTEM_CHASSIS_TYPE3)
    Status = gSmbiosBoardProtocol->SmbiosCreateChassisData();
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateChassisData failed\n"));
    }
#endif                                          // TYPE3_STRUCTURE && UPDATE_SYSTEM_CHASSIS_TYPE3
#if (TYPE4_STRUCTURE && UPDATE_CPU_TYPE4)
    if (UpdateCpuStructure) {
        Status = gSmbiosBoardProtocol->SmbiosCreateCPUData();
        if (EFI_ERROR (Status)) {
        	DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateCPUData failed\n"));
        }
    }
#endif                                          // TYPE4_STRUCTURE && UPDATE_CPU_TYPE4
#if (TYPE9_STRUCTURE && UPDATE_SLOT_TYPE9)
    Status = gSmbiosBoardProtocol->SmbiosCreateSystemSlotData();
    if (EFI_ERROR (Status)) {
    	DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateSystemSlotData failed\n"));
    }
#endif                                          // TYPE9_STRUCTURE && UPDATE_SLOT_TYPE9
#if (TYPE10_STRUCTURE && UPDATE_ONBOARD_DEV_TYPE10)
    Status = gSmbiosBoardProtocol->SmbiosCreateOnBoardDevData();
    if (EFI_ERROR (Status)) {
    	DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateOnBoardDevData failed\n"));
    }
#endif                                          // TYPE10_STRUCTURE && UPDATE_ONBOARD_DEV_TYPE10
#if (TYPE22_STRUCTURE && UPDATE_BATTERY_TYPE22)
    if (gSmbiosBoardProtocol->PortableBatteryInfoSupport) {
        Status = gSmbiosBoardProtocol->SmbiosCreateBatteryDevData();
        if (EFI_ERROR (Status)) {
        	DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateBatteryDevData failed\n"));
        }
    }
#endif                                          // TYPE22_STRUCTURE && UPDATE_BATTERY_TYPE22
#if (TYPE41_STRUCTURE && UPDATE_DEVICE_EXT_TYPE41)
    Status = gSmbiosBoardProtocol->SmbiosCreateOnBoardDevExtInfo();
    if (EFI_ERROR (Status)) {
    	DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateOnBoardDevExtInfo failed\n"));
    }
#endif                                          // TYPE41_STRUCTURE && UPDATE_DEVICE_EXT_TYPE41
#endif                                          // SMBIOS_DYNAMIC_UPDATE_POLICY == 0

#if BIOS_LANGUAGE_INFO
    AddUpdateType13();
#endif                                          // BIOS_LANGUAGE_INFO

#if SMBIOS_DYNAMIC_UPDATE_POLICY == 0
    DEBUG((DEBUG_INFO, "*** Before DynamicUpdateStructures (SMBIOS_DYNAMIC_UPDATE_POLICY = 0) ***\n"));
    DynamicUpdateStructures(SmbiosDataTable);
    DEBUG((DEBUG_INFO, "*** After DynamicUpdateStructures (SMBIOS_DYNAMIC_UPDATE_POLICY = 0) ***\n"));
#endif                                          // SMBIOS_DYNAMIC_UPDATE_POLICY

    // Call OEM porting hook in SmbiosBoard.c - OEM can make changes
    // to the SMBIOS table in this hook
    gSmbiosBoardProtocol->OemUpdate();

#if (defined(SmbiosDMIEdit_SUPPORT) && (SmbiosDMIEdit_SUPPORT !=0))
    #if SMBIOS_UPDATE_POLICY == 1
        DEBUG((DEBUG_INFO, "*** Before UpdateStructuresWithNvramData (SMBIOS_UPDATE_POLICY = 1) ***\n"));
        UpdateStructuresWithNvramData(SmbiosDataTable, MaximumTableSize);
        DEBUG((DEBUG_INFO, "*** After UpdateStructuresWithNvramData (SMBIOS_UPDATE_POLICY = 1) ***\n"));
    #endif                                      // SMBIOS_UPDATE_POLICY
#endif                                          // SmbiosDMIEdit_SUPPORT

	// Allocate scratch buffer
	DEBUG((DEBUG_INFO, "Allocating memory for Scratch buffer\n"));
	Status = pBS->AllocatePool(EfiRuntimeServicesData, MaximumTableSize, (void**)&ScratchBufferPtr);
	if (EFI_ERROR(Status)) ScratchBufferPtr = NULL;

#if SORT_SMBIOS_TABLE_BY_TYPE
    SortSmbiosTableByType();
#endif                                          // SORT_SMBIOS_TABLE_BY_TYPE

#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))
    {
        UINTN                       NumberOfHandles;
        EFI_HANDLE                  *HandleBuffer;

        if (SecureData != 0 || SetupData.CsmSupport == 0) {             // if CSM is off or Secure boot Enable
            LegacyRegionProtocol = NULL;
            LegacyBiosProtocol = NULL;
        }
        else {
            Status = pBS->LocateHandleBuffer(ByProtocol,
                                             &gEfiLegacyRegion2ProtocolGuid,
                                             NULL,
                                             &NumberOfHandles,
                                             &HandleBuffer);

            if (!EFI_ERROR(Status)) {
                Status = pBS->HandleProtocol(HandleBuffer[0],
                                             &gEfiLegacyRegion2ProtocolGuid,
                                             (void**)&LegacyRegionProtocol);
                pBS->FreePool(HandleBuffer);
                if (EFI_ERROR (Status)) {
                	DEBUG((DEBUG_ERROR | DEBUG_WARN, "LegacyRegionProtocol not found\n"));
                }
            }

            Status = pBS->LocateHandleBuffer(ByProtocol,
                                             &gEfiLegacyBiosProtocolGuid,
                                             NULL,
                                             &NumberOfHandles,
                                             &HandleBuffer);

            if (!EFI_ERROR(Status)) {
                Status = pBS->HandleProtocol(HandleBuffer[0],
                                             &gEfiLegacyBiosProtocolGuid,
                                             (void**)&LegacyBiosProtocol);

                pBS->FreePool(HandleBuffer);
                if (EFI_ERROR (Status)) {
                	DEBUG((DEBUG_ERROR | DEBUG_WARN, "LegacyBiosProtocol not found\n"));
                }
            }
        }
    }

#if ((defined(ITK_SUPPORT) && (ITK_SUPPORT != 0)) || (SMBIOS_TABLE_LOCATION)) && SMBIOS_2X_SUPPORT
    {
        UINT8   *SmbiosE000Ptr;
        UINT32  Granularity;

        if (LegacyRegionProtocol != NULL) {
            Status = SmbiosE000Alloc(&SmbiosE000Ptr, MaximumTableSize);

            // If success, relocate table
            if (!EFI_ERROR(Status)) {
                DEBUG((DEBUG_INFO, "*** SmbiosE000Alloc success ***\n"));
                Status = LegacyRegionProtocol->UnLock(LegacyRegionProtocol,
                                                      (UINT32)SmbiosE000Ptr,
                                                      MaximumTableSize,
                                                      &Granularity);

                if (!EFI_ERROR(Status)) {
                    DEBUG((DEBUG_INFO, "*** E000 unlocked, copying Smbios Table to E000 ***\n"));
                    MemCpy(SmbiosE000Ptr, SmbiosDataTable, MaximumTableSize);
                    pBS->FreePages((EFI_PHYSICAL_ADDRESS)SmbiosDataTable, EFI_SIZE_TO_PAGES(MaximumTableSize));
                    SmbiosDataTable = SmbiosE000Ptr;
                    SmbiosTableAtE000 = TRUE;

                    LegacyRegionProtocol->Lock(LegacyRegionProtocol,
                                               (UINT32)SmbiosE000Ptr,
                                               MaximumTableSize,
                                               &Granularity);
                    DEBUG((DEBUG_INFO, "*** E000 locked ***\n"));
                }
            }
        }
    }
#endif                                          // (ITK_SUPPORT || SMBIOS_TABLE_LOCATION) && SMBIOS_2X_SUPPORT

#if (CSM16_VERSION_MINOR >= 64)
    {
        UINT32                      Granularity;

        if (LegacyRegionProtocol != NULL) {
            LegacyRegionProtocol->UnLock(LegacyRegionProtocol,
                                         0xf0000,
                                         0x10000,
                                         &Granularity);

            DEBUG((DEBUG_INFO, "*** F000 unlocked ***\n"));

            Legacy16Data = (LEGACY16_TO_EFI_DATA_TABLE_STRUC*)(UINTN)(0xf0000 + *(UINT16*)0xfff4c);

            if (*(UINT32*)(UINTN)(0xf0000 + Legacy16Data->Compatibility16TableOfs) == (UINT32)'$EFI') {
#if SMBIOS_2X_SUPPORT
                // Smbios 2.x Entry Point Table
                pSmbiosTableEntryPointF000 = (SMBIOS_TABLE_ENTRY_POINT*)(UINTN)(0xf0000 + Legacy16Data->SmbiosTableOfs);

                DEBUG((DEBUG_INFO, "*** Second Smbios 2.x Table Entry Point = %x ***\n", pSmbiosTableEntryPointF000));

#endif                                          // SMBIOS_2X_SUPPORT

#if (CSM16_VERSION_MINOR >= 78)
#if SMBIOS_3X_SUPPORT
                // Smbios 3.0 Entry Point Table
                // Offset to Smbios 3.0 Entry Point Table is right after Smbios 2.x
                pSmbiosV3TableEntryPointF000 = (SMBIOS_3X_TABLE_ENTRY_POINT*)(UINTN)(0xf0000 + Legacy16Data->SmbiosTableOfs + 0x20);

                DEBUG((DEBUG_INFO, "*** Second Smbios 3.0 Table Entry Point = %x ***\n", pSmbiosV3TableEntryPointF000));

#endif                                          // SMBIOS_3X_SUPPORT
#endif                                          // CSM16_VERSION_MINOR >= 78
            }

            LegacyRegionProtocol->Lock(LegacyRegionProtocol,
                                         0xf0000,
                                         0x10000,
                                         &Granularity);
        }
    }
#endif                                          // CSM16_VERSION_MINOR >= 64

    DEBUG((DEBUG_INFO, "*** Installing 16bit PnP Smbios Functions ***\n"));

    Install16bitPnPSmbiosFunctions();
#endif                                          // CSM_SUPPORT

    // Updating Smbios EPS Header
    UpdateSmbiosTableHeader();

#if (defined(SB_WAKEUP_TYPE_FN) && (SB_WAKEUP_TYPE_FN == 1))
{
    UINT8           *Buffer;
    UINT8           *FoundPtr = NULL;
    EFI_BOOT_MODE   BootMode;

    // Save Smbios Type 1 "Wakeup Type" location in NVRAM for S3 resume
    SaveWakeupTypeLocation();

    //
    // If S4/S5 resume, update System Information structure (Type 1)
    // Wake-up Type field with detected wake-up source type.
    //
    BootMode = GetBootMode();

    if ((BootMode == BOOT_ON_S4_RESUME) || (BootMode == BOOT_ON_S5_RESUME)) {
        Buffer = SmbiosDataTable;

        if (FindStructureType(&Buffer, &FoundPtr, 1, 1)) {
            FoundPtr += 0x18;

            if ((UINT32)FoundPtr < 0xfffff) {
                NbRuntimeShadowRamWrite(TRUE);
            }

            *(UINT8*)FoundPtr = getWakeupTypeForSmbios();

            if ((UINT32)FoundPtr < 0xfffff) {
                NbRuntimeShadowRamWrite(FALSE);
            }
        }
    }
}
#endif                                          // SB_WAKEUP_TYPE_FN

#if (defined(iAMT_SUPPORT) && (iAMT_SUPPORT == 1))
{
    EFI_EVENT                   SigEvent;
    EFI_GUID                    EfiSmbiosAmtDataGuid = AMT_SMBIOS_GROUP;

    Status = pBS->CreateEventEx(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY)AmtNotifyEventFunction,
                    NULL,
                    &EfiSmbiosAmtDataGuid,
                    &SigEvent);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to create event for AMT\n"));
    }

    pBS->SignalEvent(SigEvent);
    pBS->CloseEvent(SigEvent);
}
#endif                                          // iAMT_SUPPORT

// Added for TPM
{
    EFI_EVENT       SmbiosTableEvent;
    EFI_GUID        EfiSmbiosTblPubGuid  = SMBIOS_EFI_TABLE_GROUP;

    Status = pBS->CreateEventEx(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY)NotifyEventFunction,
                    NULL,
                    &EfiSmbiosTblPubGuid,
                    &SmbiosTableEvent);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to create event for TPM\n"));
    }

    pBS->SignalEvent(SmbiosTableEvent);
    pBS->CloseEvent(SmbiosTableEvent);
}

#if SMBIOS_2X_SUPPORT
    pRS->SetVariable(L"SmbiosEntryPointTable",
                    &gAmiSmbiosNvramGuid,
                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof(SMBIOS_TABLE_ENTRY_POINT*),
                    &pSmbiosTableEntryPoint);
#endif                                          // SMBIOS_2X_SUPPORT

#if SMBIOS_3X_SUPPORT
    pRS->SetVariable(L"SmbiosV3EntryPointTable",
                    &gAmiSmbiosNvramGuid,
                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof(SMBIOS_3X_TABLE_ENTRY_POINT*),
                    &pSmbiosV3TableEntryPoint);
#endif                                          // SMBIOS_3X_SUPPORT

    pRS->SetVariable(L"SmbiosScratchBuffer",
                    &gAmiSmbiosNvramGuid,
                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof(UINT8*),
                    &ScratchBufferPtr);

    pRS->SetVariable(L"MaximumTableSize",
                    &gAmiSmbiosNvramGuid,
                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof(UINT16),
                    &MaximumTableSize);

//APTIOV_SERVER_OVERRIDE_START: Security fix
{
    #include <Protocol/VariableLock.h>

    EDKII_VARIABLE_LOCK_PROTOCOL    *pEdkiiVariableLockProtocol = NULL;

    Status = pBS->LocateProtocol(&gEdkiiVariableLockProtocolGuid,
                                 NULL,
                                 (void**)&pEdkiiVariableLockProtocol);

    if (Status == EFI_SUCCESS) {
        pEdkiiVariableLockProtocol->RequestToLock(pEdkiiVariableLockProtocol,
                                                  L"SmbiosEntryPointTable",
                                                  &gAmiSmbiosNvramGuid);

        pEdkiiVariableLockProtocol->RequestToLock(pEdkiiVariableLockProtocol,
                                                  L"SmbiosV3EntryPointTable",
                                                  &gAmiSmbiosNvramGuid);

        pEdkiiVariableLockProtocol->RequestToLock(pEdkiiVariableLockProtocol,
                                                  L"SmbiosScratchBuffer",
                                                  &gAmiSmbiosNvramGuid);

        pEdkiiVariableLockProtocol->RequestToLock(pEdkiiVariableLockProtocol,
                                                  L"MaximumTableSize",
                                                  &gAmiSmbiosNvramGuid);
    }
    else {
        DEBUG((DEBUG_WARN, "EdkiiVariableLockProtocol not found!\n"));
    }
}
//APTIOV_SERVER_OVERRIDE_END: Security fix

    Status = pBS->CreateEventEx(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY)NotifyEventFunction,
                    NULL,
                    &EfiSmbiosPointerGuid,
                    &SmbiosPointerEvent);
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to create SmbiosPointerEvent\n"));
    }

    pBS->SignalEvent(SmbiosPointerEvent);
    pBS->CloseEvent(SmbiosPointerEvent);

    BeforeEndofDxe = FALSE;

    DEBUG((DEBUG_INFO, "::: Exitting SmbiosDynamicUpdate :::\n"));
}

#if SMBIOS_DYNAMIC_UPDATE_POLICY == 1
/**
    Update structures dynamically

    @param None

    @retval None

**/
VOID
UpdateDynamicStructures(
    VOID)
{
#if UPDATE_BASEBOARD_TYPE2 || \
    UPDATE_SYSTEM_CHASSIS_TYPE3 || \
    UPDATE_CPU_TYPE4 || \
    UPDATE_SLOT_TYPE9 || \
    UPDATE_ONBOARD_DEV_TYPE10 || \
    UPDATE_BATTERY_TYPE22 || \
    UPDATE_DEVICE_EXT_TYPE41
    EFI_STATUS  Status;
#endif

    DEBUG((DEBUG_INFO, ":::  SMBIOS - UpdateDynamicStructures  :::\n"));

#if UPDATE_BASEBOARD_TYPE2
    Status = gSmbiosBoardProtocol->SmbiosCreateBaseBoardData();
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateBaseBoardData failed\n"));
    }
#endif                                          // UPDATE_BASEBOARD_TYPE2
#if UPDATE_SYSTEM_CHASSIS_TYPE3
    Status = gSmbiosBoardProtocol->SmbiosCreateChassisData();
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateChassisData failed\n"));
    }
#endif                                          // UPDATE_SYSTEM_CHASSIS_TYPE3
#if UPDATE_CPU_TYPE4
    if (UpdateCpuStructure) {
        Status = gSmbiosBoardProtocol->SmbiosCreateCPUData();
        if (EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateCPUData failed\n"));
        }
    }
#endif                                          // UPDATE_CPU_TYPE4
#if UPDATE_SLOT_TYPE9
    Status = gSmbiosBoardProtocol->SmbiosCreateSystemSlotData();
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateSystemSlotData failed\n"));
    }
#endif                                          // UPDATE_SLOT_TYPE9
#if UPDATE_ONBOARD_DEV_TYPE10
    Status = gSmbiosBoardProtocol->SmbiosCreateOnBoardDevData();
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateOnBoardDevData failed\n"));
    }
#endif                                          // UPDATE_ONBOARD_DEV_TYPE10
#if UPDATE_BATTERY_TYPE22
    if (gSmbiosBoardProtocol->PortableBatteryInfoSupport) {
        Status = gSmbiosBoardProtocol->SmbiosCreateBatteryDevData();
        if (EFI_ERROR (Status)) {
            DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateBatteryDevData failed\n"));
        }
    }
#endif                                          // UPDATE_BATTERY_TYPE22
#if UPDATE_DEVICE_EXT_TYPE41
    Status = gSmbiosBoardProtocol->SmbiosCreateOnBoardDevExtInfo();
    if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "SmbiosCreateOnBoardDevExtInfo failed\n"));
    }
#endif                                          // UPDATE_DEVICE_EXT_TYPE41

    DynamicUpdateStructures(SmbiosDataTable);

    DEBUG((DEBUG_INFO, ":::  SMBIOS - Exit UpdateDynamicStructures  :::\n"));
}
#endif

//**********************************************************************//
// AMI Implementation - PI Smbios Protocol Section Start
//**********************************************************************//

EFI_PRODUCER_HANDLE  ProducerHandleTable[PRODUCER_HANDLE_ELEMENTS];

EFI_SMBIOS_PROTOCOL   EfiSmbiosProtocol = {
                                        SmbiosPiAddStructure,
                                        SmbiosPiUpdateString,
                                        SmbiosPiRemoveStructure,
                                        SmbiosPiGetNextStructure,
#if (SMBIOS_2X_SUPPORT)
                                        SMBIOS_2X_MAJOR_VERSION,
                                        SMBIOS_2X_MINOR_VERSION,
#else
                                        SMBIOS_3X_MAJOR_VERSION,
                                        SMBIOS_3X_MINOR_VERSION,
#endif
                                       };

/**
    Add/Register the ProducerHandle associated with SmbiosHandle
    to the ProducerHandleTable

    @param SmbiosHandle
    @param ProducerHandle

    @retval None

**/
VOID
AddProducerHandle (
    IN EFI_SMBIOS_HANDLE    SmbiosHandle,
    IN EFI_HANDLE           ProducerHandle
)
{
    EFI_PRODUCER_HANDLE     *ProducerPtr = &ProducerHandleTable[0];
    UINT16                  i = 0;

    while ((ProducerPtr->SmbiosHandle != 0xffff) &&
            (ProducerPtr->SmbiosHandle != SmbiosHandle) &&
            (i < PRODUCER_HANDLE_ELEMENTS)) {
        ProducerPtr++;
        i++;
    }

    if (i < PRODUCER_HANDLE_ELEMENTS) {
        ProducerPtr->SmbiosHandle = SmbiosHandle;
        ProducerPtr->ProducerHandle = ProducerHandle;
    }
}

/**
    Initialize the ProducerHandleTable

    @param ProducerHandle

    @retval None

**/
VOID
InitializeProducerHandleTable (
    IN EFI_HANDLE  ProducerHandle
)
{
    EFI_PRODUCER_HANDLE     *ProdPtr = &ProducerHandleTable[0];
    UINT16                  i;
    UINT8                   *Ptr = SmbiosDataTable;

    for (i = 0; i < PRODUCER_HANDLE_ELEMENTS; i++) {
        ProdPtr->SmbiosHandle = 0xffff;
        ProdPtr->ProducerHandle = 0;
        ProdPtr++;
    };

    // Set ProducerHandle for each Smbios record
    while (((EFI_SMBIOS_TABLE_HEADER*)Ptr)->Type != 127) {
        AddProducerHandle(((EFI_SMBIOS_TABLE_HEADER*)Ptr)->Handle, ProducerHandle);
        Ptr += GetStructureLength(Ptr);
    }

    AddProducerHandle(((EFI_SMBIOS_TABLE_HEADER*)Ptr)->Handle, ProducerHandle);
}

/**
    Remove the "SmbiosHandle" entry from the ProducerHandleTable

    @param SmbiosHandle

    @retval None

**/
VOID
RemoveProducerHandle (
    IN EFI_SMBIOS_HANDLE    SmbiosHandle
)
{
    UINT16                  i = 0;
    EFI_PRODUCER_HANDLE     *ProducerPtr = &ProducerHandleTable[0];
    UINT16                  Size;

    while ((ProducerPtr->SmbiosHandle != 0xffff) && (ProducerPtr->SmbiosHandle != SmbiosHandle)) {
        i++;
        ProducerPtr++;
    }

    if (ProducerPtr->SmbiosHandle == SmbiosHandle) {
        EFI_PRODUCER_HANDLE     *TempPtr;

        TempPtr = ProducerPtr + 1;
        Size = (PRODUCER_HANDLE_ELEMENTS - i - 1) * sizeof(EFI_PRODUCER_HANDLE);
        MemCpy((UINT8*)ProducerPtr, (UINT8*)TempPtr, Size);
        ProducerHandleTable[PRODUCER_HANDLE_ELEMENTS - 1].SmbiosHandle = 0xffff;
        ProducerHandleTable[PRODUCER_HANDLE_ELEMENTS - 1].ProducerHandle = 0;
    }
}

/**

    @param SmbiosHandle
    @param ProducerHandle

    @return ProducerHandle

**/
VOID
GetProducerHandle (
    IN EFI_SMBIOS_HANDLE    SmbiosHandle,
    IN OUT EFI_HANDLE       *ProducerHandle
)
{
    EFI_PRODUCER_HANDLE     *ProducerPtr = &ProducerHandleTable[0];

    if (SmbiosHandle != 127) {
        while ((ProducerPtr->SmbiosHandle != 127) && (ProducerPtr->SmbiosHandle != SmbiosHandle)) {
            ProducerPtr++;
        }

        *ProducerHandle = ProducerPtr->ProducerHandle;
    }
}

/**
    Add an Smbios record

    @param This
    @param ProducerHandle OPTIONAL
    @param SmbiosHandle
    @param Record

    @return EFI_STATUS \n
            SmbiosHandle

**/
EFI_STATUS
SmbiosPiAddStructure (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN EFI_HANDLE                   ProducerHandle, OPTIONAL
    IN OUT EFI_SMBIOS_HANDLE        *SmbiosHandle,
    IN EFI_SMBIOS_TABLE_HEADER      *Record
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    UINT16          RecordSize;

    DEBUG((DEBUG_INFO, "In SmbiosPiAddStructure\n"));

    RecordSize = GetStructureLength((UINT8*)Record);

    if (*SmbiosHandle != 0xfffe) {
        if (AddStructureByHandle(*SmbiosHandle, (UINT8*)Record, RecordSize)) {
            Status = EFI_ALREADY_STARTED;
        }
        else {
            AddProducerHandle(*SmbiosHandle, ProducerHandle);
        }
    }
    else {
        if (AddStructure((UINT8*)Record, RecordSize)) {
            Status = EFI_OUT_OF_RESOURCES;
        }
        else {
            *SmbiosHandle = Record->Handle;
            AddProducerHandle(*SmbiosHandle, ProducerHandle);
        }
    }

    if (Status == EFI_SUCCESS) {
        UpdateSmbiosTableHeader();
    }

    DEBUG((DEBUG_INFO, "Exit SmbiosPiAddStructure. Status = %r\n", Status));
    return Status;
}

/**
    Update the string associated with an existing SMBIOS record

    @param This
    @param SmbiosHandle
    @param StringNumber
    @param String

    @return EFI_STATUS

**/
EFI_STATUS
SmbiosPiUpdateString (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN EFI_SMBIOS_HANDLE            *SmbiosHandle,
    IN UINTN                        *StringNumber,
    IN CHAR8                        *String
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    UINT8           *StrucPtr = SmbiosDataTable;
    UINT8           *BufferPtr;
    UINT16          BufferSize;

    DEBUG((DEBUG_INFO, "In SmbiosPiUpdateString\n"));

    if (*StringNumber) {
        if (FindStructureHandle(&StrucPtr, *SmbiosHandle)) {
            DEBUG((DEBUG_INFO, "Found structure with handle = %x\n", *SmbiosHandle));
            if (ValidateStringNumber(StrucPtr, ((SMBIOS_STRUCTURE_HEADER*)StrucPtr)->Type, *StringNumber)) {
                BufferSize = GetStructureLength(StrucPtr);

                Status = pBS->AllocatePool(EfiBootServicesData,
                                        BufferSize + (UINT16)Strlen(String) + 1,
                                        (void**)&BufferPtr);
                if (EFI_ERROR(Status)) {
                    DEBUG((DEBUG_ERROR | DEBUG_WARN, "Memory allocation failed. Exit\n"));
                    return EFI_UNSUPPORTED;
                }

                MemCpy(BufferPtr, StrucPtr, BufferSize);

                if (ReplaceString(BufferPtr, *(UINT8*)StringNumber, (UINT8*)String)) {
                    DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to replace string\n"));
                    Status = EFI_UNSUPPORTED;
                }
                else {
                    DEBUG((DEBUG_INFO, "Deleting structure with handle = %x\n", *SmbiosHandle));
                    if (DeleteStructureByHandle(*SmbiosHandle)) {
                        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to delete structure\n"));
                        Status = EFI_INVALID_PARAMETER;
                    }

                    BufferSize = GetStructureLength(BufferPtr);

                    DEBUG((DEBUG_INFO, "Adding structure with handle = %x\n", *SmbiosHandle));
                    if (AddStructureByHandle(*SmbiosHandle, BufferPtr, BufferSize)) {
                        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to add structure\n"));
                        Status = EFI_ALREADY_STARTED;
                    }
                }

                pBS->FreePool(BufferPtr);

                UpdateSmbiosTableHeader();

                Status = EFI_SUCCESS;
            }
            else {
                DEBUG((DEBUG_ERROR | DEBUG_WARN, "StringNumber validation failed!\n"));
                Status = EFI_NOT_FOUND;
            }
        }
        else {
            DEBUG((DEBUG_ERROR | DEBUG_WARN, "Structure not found!\n"));
            Status = EFI_INVALID_PARAMETER;
        }
    }
    else {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "String number missing!\n"));
        Status = EFI_NOT_FOUND;
    }

    DEBUG((DEBUG_INFO, "Exiting SmbiosPiUpdateString. Status = %r\n", Status));
    return Status;
}

/**
    Remove an SMBIOS record

    @param This
    @param SmbiosHandle

    @return EFI_STATUS

**/
EFI_STATUS
SmbiosPiRemoveStructure (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN EFI_SMBIOS_HANDLE   SmbiosHandle
)
{
    DEBUG((DEBUG_INFO, "In SmbiosPiRemoveStructure\n"));

    if (DeleteStructureByHandle((UINT16)SmbiosHandle)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to delete structure. Exit\n"));
        return EFI_INVALID_PARAMETER;
    }

    RemoveProducerHandle((EFI_SMBIOS_HANDLE)SmbiosHandle);

    UpdateSmbiosTableHeader();

    DEBUG((DEBUG_INFO, "Exit SmbiosPiRemoveStructure\n"));
    return EFI_SUCCESS;
}

/**
    Allow the caller to discover all or some of the SMBIOS records

    @param This
    @param SmbiosHandle
    @param Type OPTIONAL
    @param Record
    @param ProducerHandle OPTIONAL

    @return EFI_STATUS \n
            Pointer to structure found

    @par  Implementation:
          @li SmbiosHandle = 0xfffe,  Type = NULL ==> Record = 1st record in table,\n
                                                  SmbiosHandle = handle of record being returned\n
          @li SmbiosHandle != 0xfffe, Type = NULL ==> If record of SmbiosHandle is not found, return EFI_NOT_FOUND, SmbiosHandle = 0xfffe\n
                                                  else Record = next record after record of SmbiosHandle,\n
                                                  SmbiosHandle = handle of record being returned\n
          @li SmbiosHandle = 0xfffe,  Type <> 0   ==> If record of input "Type" is not found, return EFI_NOT_FOUND, SmbiosHandle = 0xfffe\n
                                                  else Record = 1st record of type "*Type",\n
                                                   SmbiosHandle = handle of record being returned\n
          @li SmbiosHandle != 0xfffe, Type <> 0   ==> If record of SmbiosHandle is not found, or found but no record of type "*Type" after\n
                                                  record of SmbiosHandle can be found, return EFI_NOT_FOUND, SmbiosHandle = 0xfffe\n
                                                  else Record = next record after record of SmbiosHandle,\n
                                                  SmbiosHandle = handle of record being returned
**/
EFI_STATUS
SmbiosPiGetNextStructure (
    IN CONST EFI_SMBIOS_PROTOCOL    *This,
    IN OUT EFI_SMBIOS_HANDLE        *SmbiosHandle,
    IN EFI_SMBIOS_TYPE              *Type OPTIONAL,
    OUT EFI_SMBIOS_TABLE_HEADER     **Record,
    OUT EFI_HANDLE                  *ProducerHandle OPTIONAL
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    UINT8           *StrucPtr;
    UINT8           *StructureFoundPtr;
    UINT16          StrucSize;

    DEBUG((DEBUG_INFO, "In SmbiosPiGetNextStructure\n"));

    StrucPtr = SmbiosDataTable;

    if (Type == NULL) {
        if (*SmbiosHandle == 0xfffe) {
            // Type = NULL, *SmbiosHandle = 0xfffe ==> return 1st record in Smbios table
            Status = EFI_SUCCESS;
            goto GetNext_Exit;
        }
        else {
            // Type = NULL, *SmbiosHandle != 0xfffe
            if (FindStructureHandle(&StrucPtr, *SmbiosHandle)) {
                if (((EFI_SMBIOS_TABLE_HEADER*)StrucPtr)->Type != 127) {
                    // Record of *SmbiosHandle is found, return next record
                    StrucSize = GetStructureLength(StrucPtr);
                    StrucPtr += StrucSize;
                    Status = EFI_SUCCESS;
                }
                else {
                    // End of Smbios table, return error
                    Status = EFI_NOT_FOUND;
                }
            }
            else {
                // Record of *SmbiosHandle is not found, return error
                Status = EFI_NOT_FOUND;
            }

            goto GetNext_Exit;
        }
    }
    else {                                      // Type != NULL
        if (*SmbiosHandle == 0xfffe) {
            // Search for 1st record of *Type
            if (FindStructureType(&StrucPtr, &StructureFoundPtr, *Type, 1)) {
                // Type != NULL, *SmbiosHandle = 0xfffe
                // Record of *Type is found ===> return record found
                StrucPtr = StructureFoundPtr;
                Status = EFI_SUCCESS;
            }
            else {
                // Type != NULL, *SmbiosHandle = 0xfffe
                // Record of *Type is not found, return error
                Status = EFI_NOT_FOUND;
            }

            goto GetNext_Exit;
        }
        else {                                  // Type != NULL, *SmbiosHandle != 0xfffe
            if (FindStructureHandle(&StrucPtr, *SmbiosHandle)) {
                if (((EFI_SMBIOS_TABLE_HEADER*)StrucPtr)->Type != 127) {
                    // Record of SmbiosHandle is found, skip to next record
                    StrucSize = GetStructureLength(StrucPtr);
                    StrucPtr += StrucSize;
                    if (FindStructureType(&StrucPtr, &StructureFoundPtr, *Type, 1)) {
                        // Type != NULL, *SmbiosHandle != 0xfffe
                        // Record of *Type is found ===> return record found
                        StrucPtr = StructureFoundPtr;
                        Status = EFI_SUCCESS;
                    }
                    else {
                        // Type != NULL, *SmbiosHandle != 0xfffe,
                        // Record of SmbiosHandle is found but record of *Type is not found, return error
                        Status = EFI_NOT_FOUND;
                    }
                }
                else {
                    // End of Smbios table, return error
                    Status = EFI_NOT_FOUND;
                }
            }
            else {
                // Type != NULL, *SmbiosHandle != 0xfffe,
                // Record of SmbiosHandle is not found, return error
                Status = EFI_NOT_FOUND;
            }
        }
    }

GetNext_Exit:
    if (Status == EFI_SUCCESS) {
        *Record = (EFI_SMBIOS_TABLE_HEADER*)StrucPtr;

        if (ProducerHandle != NULL) {
            GetProducerHandle(*SmbiosHandle, ProducerHandle);
        }

        if (((EFI_SMBIOS_TABLE_HEADER*)StrucPtr)->Type != 127) {
            *SmbiosHandle = ((EFI_SMBIOS_TABLE_HEADER*)StrucPtr)->Handle;
        }
        else {
            *SmbiosHandle = 0xfffe;
            Status = EFI_NOT_FOUND;
        }
    }
    else {
        *SmbiosHandle = 0xfffe;
    }

    DEBUG((DEBUG_INFO, "Exit SmbiosPiGetNextStructure. Status = %r\n", Status));
    return Status;
}

//**********************************************************************//
// AMI Implementation - PI Smbios Protocol Section End
//**********************************************************************//

/**
    SMBIOS driver entry point

    @param ImageHandle
    @param SystemTable

    @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
SmbiosDriverEntryPoint(
    IN EFI_HANDLE               ImageHandle,
    IN EFI_SYSTEM_TABLE         *SystemTable
)
{
    EFI_STATUS                  Status;
    UINT8                       *Buffer = NULL;
    UINT8                       *BufferPtr;
    UINTN                       BufferSize;
    UINT8                       *TempPtr = NULL;
    UINT8                       Type127Structure[] = {0x7f, 0x04, 0x00, 0x00, 0x00, 0x00};
    UINT16                      Size;
    EFI_EVENT                   EndOfDxeEvent;
    BOOLEAN                     StaticTableFound = FALSE;

    InitAmiLib(ImageHandle, SystemTable);

    DEBUG((DEBUG_INFO, "In SmbiosDriverEntryPoint\n"));

#if SMBIOS_2X_SUPPORT
{
    EFI_PHYSICAL_ADDRESS    Address;

    // Allocate runtime buffers for Smbios 2.x Table Entry Point
    DEBUG((DEBUG_INFO, "Allocating memory for Smbios v2.x SmbiosEntryPointTable\n"));

    Address = 0xFFFFFFFF;
    Status = pBS->AllocatePages(
                            AllocateAnyPages,
                            EfiRuntimeServicesData,
                            1,
                            &Address
                            );
    ASSERT_EFI_ERROR(Status);
    pSmbiosTableEntryPoint = (SMBIOS_TABLE_ENTRY_POINT*)Address;
    DEBUG((DEBUG_INFO, "pSmbiosTableEntryPoint = %x\n", pSmbiosTableEntryPoint));

    MemCpy(pSmbiosTableEntryPoint, &SmbiosEntryPointTable, sizeof(SMBIOS_TABLE_ENTRY_POINT));
}
#endif                                          // SMBIOS_2X_SUPPORT

#if SMBIOS_3X_SUPPORT
    // Allocate runtime buffers for Smbios 3.x Table Entry Point
    DEBUG((DEBUG_INFO, "Allocating memory for Smbios v3.x Smbios3EntryPointTable\n"));
#if (SMBIOS_TABLE_ADDRESS_LIMIT == 1)
    pBS->AllocatePool(EfiRuntimeServicesData, sizeof(SMBIOS_3X_TABLE_ENTRY_POINT), (void**)&pSmbiosV3TableEntryPoint);

#else
{
    EFI_PHYSICAL_ADDRESS    Address;

    Address = 0xFFFFFFFF;
    Status = pBS->AllocatePages(
                            AllocateMaxAddress,
                            EfiRuntimeServicesData,
                            1,
                            &Address
                            );
    ASSERT_EFI_ERROR(Status);
    pSmbiosV3TableEntryPoint = (SMBIOS_3X_TABLE_ENTRY_POINT*)Address;
    DEBUG((DEBUG_INFO, "pSmbiosV3TableEntryPoint = %x\n", pSmbiosV3TableEntryPoint));
}
#endif                                          // SMBIOS_TABLE_ADDRESS_LIMIT

    MemCpy(pSmbiosV3TableEntryPoint, &SmbiosV3EntryPointTable, sizeof(SMBIOS_3X_TABLE_ENTRY_POINT));
#endif                                          // SMBIOS_3X_SUPPORT

    Status = pBS->LocateProtocol(&gAmiSmbiosBoardProtocolGuid,
                                 NULL,
                                 (void**)&gSmbiosBoardProtocol);
    ASSERT_EFI_ERROR(Status);

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
{
    EFI_GUID    EfiSetupGuid = SETUP_GUID;
    UINTN       VarSize;

    BufferSize = sizeof(SETUP_DATA);
    pRS->GetVariable(L"Setup", &EfiSetupGuid, NULL, &BufferSize, &SetupData);

    VarSize = sizeof(UINT8);
    pRS->GetVariable (L"SecureBoot", &gEfiGlobalVariableGuid, NULL, &VarSize, &SecureData);

}
#endif                                          // CSM_SUPPORT

    DEBUG((DEBUG_INFO, "Get static table\n"));
    // Get SMBios Data Structure Image
    Status = LoadRealModeFileSection(&gAmiSmbiosStaticDataGuid, \
                                    EFI_SECTION_FREEFORM_SUBTYPE_GUID, (void**)&Buffer, &BufferSize);

    // If Smbios static table is found, then use it
    // otherwise create an empty table with size determined by EXTRA_RESERVED_BYTES
    // SDL token and set the beginning structure to type 127
    if (Status == EFI_SUCCESS) {
        DEBUG((DEBUG_INFO, "=== Static table found! ===\n"));

        StaticTableFound = TRUE;

        // Skip over Section GUID
        BufferPtr = Buffer;
        BufferPtr += sizeof (EFI_GUID);
        BufferSize -= sizeof (EFI_GUID);
        UpdateType127Handle(BufferPtr);

#if (SMBIOS_TABLES_BUILD_TYPE == 1)                     // Use DT file
        DEBUG((DEBUG_INFO, "Static table built from DT file\n"));

        FixupHandles(BufferPtr);

        // Allocate new buffer with size equals to (static table size + EXTRA_RESERVED_BYTES)
        // to allow the table to grow
        MaximumTableSize = (UINT16)BufferSize + EXTRA_RESERVED_BYTES;
#else
        DEBUG((DEBUG_INFO, "Static table built from ASM file\n"));
        MaximumTableSize = (UINT16)BufferSize;
#endif                                                  // SMBIOS_TABLES_BUILD_TYPE
    }
    else {
        DEBUG((DEBUG_INFO, "=== Static table NOT found! ===\n"));
        MaximumTableSize = EXTRA_RESERVED_BYTES;
    }

#if SMBIOS_2X_SUPPORT
    // Allocate Runtime buffer for Smbios 2.x table
    DEBUG((DEBUG_INFO, "Allocating memory for Smbios 2.x and 3.x table\n"));
    SmbiosDataTable = (UINT8*)0xFFFFFFFF;
    Status = pBS->AllocatePages(
                            AllocateMaxAddress,
                            EfiRuntimeServicesData,
                            EFI_SIZE_TO_PAGES(MaximumTableSize),
                            (EFI_PHYSICAL_ADDRESS*)&SmbiosDataTable
                            );
#else                                                   // SMBIOS_2X_SUPPORT
    // Allocate Runtime buffer for Smbios 3.x table
    DEBUG((DEBUG_INFO, "Allocating memory for Smbios 3.x table\n"));

#if (SMBIOS_TABLE_ADDRESS_LIMIT == 0)
    SmbiosDataTable = (UINT8*)0xFFFFFFFF;
    Status = pBS->AllocatePages(
                            AllocateMaxAddress,
                            EfiRuntimeServicesData,
                            EFI_SIZE_TO_PAGES(MaximumTableSize),
                            (EFI_PHYSICAL_ADDRESS*)&SmbiosDataTable
                            );
#else                                                   // SMBIOS_TABLE_ADDRESS_LIMIT

    Status = pBS->AllocatePages(
                            AllocateAnyPages,
                            EfiRuntimeServicesData,
                            EFI_SIZE_TO_PAGES(MaximumTableSize),
                            (EFI_PHYSICAL_ADDRESS*)&SmbiosDataTable
                            );
#endif                                                  // SMBIOS_TABLE_ADDRESS_LIMIT
#endif                                                  // SMBIOS_2X_SUPPORT
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    // Clear SmbiosDataTable to 0xff and copy static table
    MemSet(SmbiosDataTable, MaximumTableSize, 0xff);

    if (StaticTableFound) {
        MemCpy(SmbiosDataTable, BufferPtr, BufferSize);

        DEBUG((DEBUG_INFO, "Freeing Buffer\n"));
        pBS->FreePool(Buffer);
    }
    else {
        MemCpy(SmbiosDataTable, Type127Structure, sizeof(Type127Structure));
        LastHandle = 0;
    }

    // Set UpdateCpuStructure flag to FALSE if CPU type 4 structure does not exist
    Status = ReadStructureByType(4, 1, &TempPtr, &Size);
    if (EFI_ERROR(Status)) {
        // CPU type 4 structure not found
        UpdateCpuStructure = FALSE;
    }
    else {
        UpdateCpuStructure = TRUE;
        pBS->FreePool(TempPtr);                 // TempPtr from ReadStructureByType
    }

    // Update structures with NVRAM data
#if (defined(SmbiosDMIEdit_SUPPORT) && (SmbiosDMIEdit_SUPPORT !=0))
    #if SMBIOS_UPDATE_POLICY == 0
        DEBUG((DEBUG_INFO, "Before UpdateStructuresWithNvramData\n"));
        UpdateStructuresWithNvramData(SmbiosDataTable, MaximumTableSize);
        DEBUG((DEBUG_INFO, "After UpdateStructuresWithNvramData\n"));
    #endif
#endif

#if SMBIOS_DYNAMIC_UPDATE_POLICY == 1
    DEBUG((DEBUG_INFO, "Before UpdateDynamicStructures\n"));
    // Update dynamic structures
    UpdateDynamicStructures();
    DEBUG((DEBUG_INFO, "After UpdateDynamicStructures\n"));
#endif

    DEBUG((DEBUG_INFO, "Registering SmbiosDynamicUpdate callback on EndOfDxeEvent\n"));
    //
    // Register the event handling function to dynamically update
    // structures.
    //
    Status = pBS->CreateEventEx (
								EVT_NOTIFY_SIGNAL,
								SMBIOS_DYNAMIC_UPDATE_TPL,
								(EFI_EVENT_NOTIFY)SmbiosDynamicUpdate,
								NULL,
								&gEfiEndOfDxeEventGroupGuid,
								&EndOfDxeEvent
								);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to create event for SmbiosDynamicUpdate\n"));
    }

    DEBUG((DEBUG_INFO, "Initializing Producer Handle Table\n"));
    // Initialize Producer Handle table and set ProducerHandle
    // for each Smbios record to Smbios Driver Image Handle
    InitializeProducerHandleTable(ImageHandle);

    UpdateSmbiosTableHeader();

#if SMBIOS_2X_SUPPORT
{
    EFI_GUID        Smbios2Guid = SMBIOS_TABLE_GUID;

    DEBUG((DEBUG_INFO, "*** Publishing Smbios 2x to System Configuration Table ***\n"));

    // Update EFI Configuration Table
    Status = pBS->InstallConfigurationTable(&Smbios2Guid, pSmbiosTableEntryPoint);
    ASSERT_EFI_ERROR(Status);
}
#endif                                          // SMBIOS_2X_SUPPORT

#if SMBIOS_3X_SUPPORT
{
    EFI_GUID        Smbios3Guid = SMBIOS_3_TABLE_GUID;

    DEBUG((DEBUG_INFO, "*** Publishing Smbios 3x to System Configuration Table ***\n"));

    Status = pBS->InstallConfigurationTable(&Smbios3Guid, pSmbiosV3TableEntryPoint);
    ASSERT_EFI_ERROR(Status);
}
#endif                                          // SMBIOS_3X_SUPPORT

    DEBUG((DEBUG_INFO, "Installing PI Smbios protocol\n"));
    // Install PI Smbios protocol
    Status = pBS->InstallProtocolInterface(&ImageHandle,
                                          &gEfiSmbiosProtocolGuid,
                                          EFI_NATIVE_INTERFACE,
                                          &EfiSmbiosProtocol);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to install EfiSmbiosProtocol\n"));
    }

#if AMI_SMBIOS_PROTOCOL_ENABLE
    DEBUG((DEBUG_INFO, "Installing AMI Smbios protocol\n"));
    // Install AMI Smbios protocol
    Status = pBS->InstallProtocolInterface(&ImageHandle,
                                          &gAmiSmbiosProtocolGuid,
                                          EFI_NATIVE_INTERFACE,
                                          &AmiSmbiosProtocol);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to install AmiSmbiosProtocol\n"));
    }
#endif                                          // AMI_SMBIOS_PROTOCOL_ENABLE

    DEBUG((DEBUG_INFO, "Exiting SmbiosDriverEntryPoint. Status = %r\n", Status));
    return EFI_SUCCESS;
}

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
/**
    SMBIOS 16-bit PnP runtime functions installation.

    @param None

    @return EFI_STATUS

**/
EFI_STATUS
Install16bitPnPSmbiosFunctions()
{
    EFI_STATUS  Status;
    EFI_LEGACY_BIOS_EXT_PROTOCOL    *BiosExtensions = NULL;
    UINTN       Dest;
    UINT32      LockUnlockAddr;
    UINT32      LockUnlockSize;
    VOID        *Buffer = NULL;
    UINTN       BufferSize;
    UINT16      *NewPnPFunctionPtr;

    DEBUG((DEBUG_INFO, "In Install16bitPnPSmbiosFunctions\n"));

    //
    // Locate 16-bit binary and copy it to F000 segment.
    //
    Status = pBS->LocateProtocol(
        &gEfiLegacyBiosExtProtocolGuid, NULL, (void**)&BiosExtensions);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "LegacyBiosExtProtocol not found. Exit\n"));
        return Status;
    }

    Status = BiosExtensions->GetEmbeddedRom(1, 0, 3, &Buffer, &BufferSize);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Embedded Rom not found. Exit\n"));
        return Status;
    }

    Dest = BiosExtensions->CopyLegacyTable(Buffer, (UINT16)BufferSize, 0, 1);

    Legacy16Data = (LEGACY16_TO_EFI_DATA_TABLE_STRUC*)(UINTN)(0xf0000 + *(UINT16*)0xfff4c);
    NewPnPFunctionPtr = (UINT16 *)(UINTN) (0xf0000 + Legacy16Data->AddedPnpFunctionsOfs);

    if ((VOID*)Dest != NULL) {
        BiosExtensions->UnlockShadow((UINT8*)(UINTN)0xf0000, 0x10000, &LockUnlockAddr, &LockUnlockSize);
        *NewPnPFunctionPtr = (UINT16)Dest;
        BiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize);

        DEBUG((DEBUG_INFO, "Exit Install16bitPnPSmbiosFunctions\n"));
        return EFI_SUCCESS;
    }
    else {
        DEBUG((DEBUG_ERROR | DEBUG_WARN, "Failed to copy embedded Rom. Exit\n"));
        return EFI_OUT_OF_RESOURCES;
    }
}
#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
