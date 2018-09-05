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
// $Header: /Alaska/SOURCE/Modules/TCG/TCGMisc.h 31    4/19/12 8:04p Fredericko $
//
// $Revision: 31 $
//
// $Date: 4/19/12 8:04p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TCG/TCGMisc.h $
// 
// 31    4/19/12 8:04p Fredericko
// Add Fastboot GUID definition for Internal use
// 
// 30    3/19/12 6:19p Fredericko
// Changes for TcgPerformance Metrics Improvement. 
// Files Changed: Tcg.sdl, TcgMisc.h, TcgDxe.c, TcgCommon.c, TcgCommon.h, 
// AmiTcgPlatformPeiLib.c, AmiTcgPlatformDxe.c, TcgDxe.dxs
// 
// 29    2/03/12 5:11p Fredericko
// [TAG]  		EIP81665
// [Category]  	Improvement
// [Description]  	Support for MOR feature improvement
// [Files]  		Tcg.sdl, AmiTcgPlatformDxe.c, Tcgdxe.c, Tcglegacy.c
// 
// 28    1/17/12 11:22a Fredericko
// Install Tpm_Initialized Ppi in PEI
// 
// 27    11/01/11 4:15a Fredericko
// Fixed spelling changes request for physicalPresenceLock and
// bGlobalLock.
// 
// 26    8/26/11 12:53p Fredericko
// [TAG]  		EIP65710 
// [Category]  	Bug Fix
// [Severity]  	Minor
// [Symptom]  	Wrong structure for TPM and TCM permanent flags
// [RootCause]  	Lower part of structure was not defined properly since we
// don't use them in BIOS
// [Solution]  	Update structure in TcgMisc.h
// [Files]  		TcgMisc.h
// 
// 25    8/09/11 6:13p Fredericko
// [TAG]  		EIP65605
// [Category]  	Bug Fix
// [Severity]  	Minor
// [Symptom]  	None
// [RootCause]  	Tpm Permanent flags Structure needs to be updated
// [Solution]  	Updated the Tpm Permanent Flags structure
// [Files]  		TcgMisc.h
// 
// 24    7/24/11 10:05p Fredericko
// [TAG]  		EIP65177
// [Category]  	Spec Update
// [Severity]  	Minor
// [Description]  	Included sample  protocoldefinition to read and set
// persistent BIOS flags.
// 2. Also include defines for new smi transactions for Ppi.
// 
// 23    4/06/11 6:46p Fredericko
// PPI confirmation Override changes
// 
// 22    4/01/11 10:50a Fredericko
// Added TCG legacy GUID for initializing the TPM in legacy mode
// 
// 21    3/29/11 12:25p Fredericko
// 
// 20    3/28/11 12:22p Fredericko
// [TAG]        EIP 54642
// [Category] Improvement
// [Description] 1. Checkin Files related to TCG function override
// 2. Include TCM and TPM auto detection
// [Files] Affects all TCG files
// 
// 17    8/04/10 5:21p Fredericko
// Added #defines for TCG SMI transactions
// 
// 16    7/09/10 3:26p Fredericko
// EFI_NULL GUID defined
// 
// 15    5/19/10 5:18p Fredericko
// Included File Header
// Code Beautification
// EIP 37653
//
// 14    3/25/10 7:13p Fredericko
// Function definition for ResetPPI variables added
//
// 13    3/23/10 8:40p Fredericko
//
// 12    3/19/10 4:13p Fredericko
// Generic guid defined for legacy MA Driver, Legacy MP driver and TCG
// driver hobs
// Also modified structures Far32Locals, MAStruct, ESPFuncStruc
//
// 11    1/14/10 11:43a Fredericko
// Added TCG NVRAM structure  and guid definitions.
//
// 10    12/03/09 6:23p Fredericko
// added UEFI guid definition for MOR
//
// 9     6/08/09 4:35p Fredericko
// typedefs for STANY_FLAGS
//
// 8     6/01/09 3:54p Fredericko
// changes for TCM support.
//
// 7     4/30/09 6:13p Fredericko
// Updated Header Date
//
// 6     4/30/09 5:30p Fredericko
// AMI company Header Address changes
//
// 5     3/05/09 2:02p Fredericko
// Changes for CoreSources dependency
//
// 4     2/05/09 4:54p Fredericko
// Generic TCG structures added and GUIDs added in favor of building
// without EDK.
//
// 3     26/08/08 10:44a Anandakrishnanl
// Added Sdl Token for the define Equates and Changed the Corresponding
// Codes
//
// 2     2/27/08 5:50p Fredericko
// TCG structures included
//
// 1     7/23/07 10:24a Fredericko
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TcgMisc.h
//
// Description: 
//  Miscellaneous Tcg Header file
//
//<AMI_FHDR_END>
//*************************************************************************
//------------------------------------------------------------------------------
//                OEM INFO for TCG structures(Port OEM info)
//------------------------------------------------------------------------------
#ifndef  _TCGMISC_H   //To Avoid this header get compiled twice
#define  _TCGMISC_H
#include <Uefi.h>
#include <Token.h>

#define EV_SPECIFICATION_IDENTIFIER        (UINT32)0x000000012
//--------------------AMI_PORTING----------------------------------------------
#define TCG_BIOS_VENDOR           "AMI"
#define TCG_OEM_ID               "ALASKA"
//-----------------------------------------------------------------------------

#define AMI_TCG_MANUFACTURING_MODE_HOB_GUID \
    {0x5d7f83ee, 0x88e5, 0x4ea8, 0xbe, 0xe8, 0x23, 0x79, 0xa6, 0x3c, 0xff, 0x8}


#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)
#define ___INTERNAL_CONVERT_TO_WSTRING___(a) L###a
#define CONVERT_TO_WSTRING(a) ___INTERNAL_CONVERT_TO_WSTRING___(a)

#define NVRAM_MODE_MANUFACTORING 1

#pragma pack (1)

typedef struct {
    EFI_PHYSICAL_ADDRESS NvramAddress;
    EFI_PHYSICAL_ADDRESS BackupAddress;
    UINT32 NvramSize;
    UINT32 HeaderLength;
    UINT32 NvramMode;
} Manufac_HOB;


typedef struct {
    UINT32  Count;
    UINT32  Tcg2SpecVersion;
    UINT32  PcrBanks; // Record the BIOS Activate PCR
    EFI_PHYSICAL_ADDRESS baseAddress;
    UINT32   Size;
}FwVolHob;

typedef struct {
   UINT8 Tpm20DeviceState;
   UINT8 InterfaceType;
   UINT8 Reserved;
   UINT32 Tpm2FWersion1;
   UINT32 Tpm2manufacturer;
   UINT32 Tpm2FWersion2;   
} Tpm20DeviceHob;

typedef struct
{
    UINT32 RQST;                    //Store PPI request
    UINT32 RCNT;                    //Store most recent PPI request
    UINT32 ERROR;                   //Store BIOS ERROR information after a PPI request
    UINT32 Flag;                    //Flag used by PPI SMM interface to determine ACPI PPI function request
    UINT32 AmiMisc;                 //Misc storage not used
    UINT32 OptionalData;            //Store optional data that OS might provide during a PPI request
} AMI_PPI_NV_VAR;

typedef struct
{
    UINT32 RQST;                 //Store PPI request
    UINT32 RCNT;                 //Store most recent PPI request
    UINT32 ERROR;                //Store BIOS ERROR information after a PPI request
    UINT32 Flag;                 //Flag used by PPI SMM interface to determine ACPI PPI function request
    UINT32 AmiMisc;              //Misc storage not used
    UINT32 OptionalRqstData;     //Store optional data that OS might provide during a PPI request
    UINT32 RequestFuncResponse;  //Store Response of ACPI function request. Returned to caller
} AMI_ASL_PPI_NV_VAR;


typedef struct _mem_in
{
    UINTN  datat;       //0=use_32;1=use_64;2=use_both; 3=dont_use
    UINT64 address;     //Start Memory address
    UINTN  size;        //Memory block Size
} mem_in;

typedef struct tdMiscPCR_EVENT_HDR {
  UINT32                     PCRIndex;
  UINT32                     EventType;
  UINT8                      digest[20];
  UINT32                     EventSize;
} MiscPCR_EVENT_HDR;

typedef struct  { 
	MiscPCR_EVENT_HDR evtHdr;
    UINT32   VendorId;
    EFI_GUID ReferenceManifest;
} TCGNISTSpecPlatformId;

typedef struct tdTCG_EFISpecIdEventAlgorithmSize{
    UINT16 algortihgmId;
    UINT16 digestSize;
}TCG_EFISpecIdEventAlgorithmSize;

#define NumberOfAlgorithms 1;

typedef struct
{
	MiscPCR_EVENT_HDR evtHdr;
    UINT8  Signature[16];
    UINT8  PlatformClass;
    UINT8  SpecVersionMinor;
    UINT8  SpecVersionMajor;
    UINT8  SpecErrata;
    UINT8  uintnSize;
    UINT32 numberOfAlgorithms;
    TCG_EFISpecIdEventAlgorithmSize digestSizes[1];
    UINT8  VendorInfoSize;
} TCG_PCClientSpecIDEventStruct;

typedef struct
{
    UINT8  Signature[16];
    UINT32 PlatformClass;    // Spec format fix, see PC Client EFI protocol Spec, ver 0.04
    UINT8  SpecVersionMinor;
    UINT8  SpecVersionMajor;
    UINT8  SpecErrata;
    UINT8  uintnSize;
    UINT32 numberOfAlgorithms;
    TCG_EFISpecIdEventAlgorithmSize digestSizes[5];  // adjustable size, set be 5 is for Both sha1+sha256+sha384+sha512+SM3_256
    UINT8  VendorInfoSize;
} TCG_PCClientSpecIDEventStructEx;

typedef struct
{
    UINT32 SupportedPcrBitMap;
    UINT32 ActivePcrBitMap;
    UINT32 Reserved;
    UINT32 TpmFwVersion;
    UINT32 TpmManufacturer;
}AMITCGSETUPINFOFLAGS;
#pragma pack()


#pragma pack (1)
typedef struct
{
    UINT16 Signature;             // 0xaa55
    UINT32 CodeP;                 // Pointer to beginning of code ( offset to entry point for driver)
    UINT16 Size;                  //Total size of the driver in bytes including header
    UINT32 BaseAddress;           //base address of the TPM(as set by BIOS)
    UINT32 BaseAddress2;          //optional second base address of the TPM(as set by BIOS)
    UINT8  TcgIrqL;               //IRQ level
    UINT8  DMACH;                 //DMA channel
    UINT8  checksum;              //XOR checksum
    UINT8  Reserved;
    UINT32 PCIPFA;                //IRQ level
    UINT32 Other;                 //Cardbus, USB etc
    UINT32 TpmPort;               //location of TPM configuration port
    UINT16 VID;                   //vendor ID
    UINT16 DID;                   //device ID
} MPDRIVER_LEGHEADER;
#pragma pack ()



#define  EfiBootservicesData   0x04

#pragma pack(1)

typedef struct
{
    UINTN                   NumberOfTables;
    EFI_CONFIGURATION_TABLE TableEntry[1];
} TCG_EFI_HANDOFF_TABLE_POINTERS;

//
// EFI_VARIABLE_DATA
//
// This structure serves as the header for measuring variables. The name of the
// variable (in Unicode format) should immediately follow, then the variable
// data.
//
typedef struct
{
    EFI_GUID VariableName;
    UINT64   UnicodeNameLength;
    UINT64   VariableDataLength;
    CHAR16   UnicodeName[1];
    INT8     VariableData[1]; // Driver or platform-specific data
} TCG_EFI_VARIABLE_DATA;
#pragma pack()

#define INTERNAL_SHA_ARRAY_LENGTH            0x500 //500 bytes per SHA update or extend

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Internal Tcg Capabilities Headers and Struct
//-----------------------------------------------------------------------------
VOID* LocateATcgHob (
    UINTN                   NoTableEntries,
    EFI_CONFIGURATION_TABLE *ConfigTable,
    EFI_GUID                *HOB_guid );

#pragma pack (1)
typedef struct
{
    UINT16 Tag;
    UINT32 ParamSize;
    UINT32 CommandCode;
    UINT32 caparea;
    UINT32 subCapSize;
    UINT32 subCap;
} TPM_GetCapabilities_Input;

typedef struct
{
    UINT16 tag;
    UINT32 paramSize;
    UINT32 ordinal;
    UINT32 capArea;
    UINT32 subCapSize;
    UINT32 subCap;
    UINT32 setValueSize;
    UINT32 setValue;
} TPM_SetCapability_Input;

typedef struct
{
    UINT16 Tag;
    UINT32 ParamSize;
    UINT32 RetCode;
    UINT32 respSize;
    UINT16 tag;
    UINT8  disabled;
    UINT8  ownership;
    UINT8  deactivated;
    UINT8  readPubek;
    UINT8  disableOwnerClear;
    UINT8 allowMaintenance;
    UINT8 physicalPresenceLifetimeLock;
    UINT8 physicalPresenceHWEnable;
    UINT8 physicalPresenceCMDEnable;
    UINT8 CEKPUsed;
    UINT8 TPMpost;
    UINT8 TPMpostLock;
    UINT8 FIPS;
    UINT8 Operator;
    UINT8 enableRevokeEK;
    UINT8 nvLocked;
    UINT8 readSRKPub;
	UINT8 tpmEstablished;
	UINT8 maintenanceDone;
	UINT8 disableFullDALogicInfo;
} TPM_Capabilities_PermanentFlag;

#define NTC_ORD_GET_TPM_STATUS   ((UINT32) 0x20000021)
typedef struct
{
    UINT16 Tag;
    UINT32 ParamSize;
    UINT32 RetCode;
    UINT8  isdisabled;
    UINT8  isdeactivated;
    UINT8  isEkSet;
    UINT8  isOwnerSet;
    UINT8  preConfigSet;
    //ami specific we 
    //don't need rest of structure
} NUVOTON_SPECIFIC_FLAGS;


typedef struct
{
    UINT16 Tag;
    UINT32 ParamSize;
    UINT32 RetCode;
    UINT32 respSize;
    UINT16 tag;
    UINT8  deactivated;
    UINT8  disableForceClear;
    UINT8  physicalPresence;
    UINT8  physicalPresenceLock;
    UINT8  bGlobalLock;
} TPM_VOLATILE_FLAGS;



typedef struct
{
    UINT16 Tag;
    UINT32 ParamSize;
    UINT32 RetCode;
    UINT32 respSize;
    UINT16 tag;
    UINT8  disabled;
    UINT8  ownership;
    UINT8  deactivated;
    UINT8  readPubek;
    UINT8  disableOwnerClear;
    UINT8 physicalPresenceLifetimeLock;
    UINT8 physicalPresenceHWEnable;
    UINT8 physicalPresenceCMDEnable;
    UINT8 CEKPUsed;
    UINT8 TPMpost;
    UINT8 TPMpostLock;
    UINT8 Operator;
    UINT8 enableRevokeEK;
    UINT8 nvLocked;
    UINT8 tpmEstablished;
    UINT8 writeEkCertLock;
} TCM_Capabilities_PermanentFlag;


typedef struct _TPMTransmitEntryStruct
{
    UINT32 pbInBuf;
    UINT32 dwInLen;
    UINT32 pbOutBuf;
    UINT32 dwOutLen;
} TPMTransmitEntryStruct;


#define PCI_EXPANSION_ROM_HEADER_SIGNATURE       0xaa55


/*
 * Old Ppi 1.2 Transactions
#define     READTRANSACTION                          0x01
#define     WRITETRANSACTION                         0x02
#define     GETCONFTRANSACTION                       0x03
#define     RQSTVAR                                  0x10
#define     RCNTVAR                                  0x20
#define     ERRORVAR                                 0x30
#define     ERRORVAR2                                0x50
#define     CONFIRMATION                             0x40
#define     WRITEMOR                                 0x20
#define     WRITERQST                                0x10
#define     WRITEENDRQSTFLAG                         0x01
#define     WRITEENDMORFLAG                          0x02
#define     READENDCONFLAG                           0x03
#define     WRITEENDOPTIONALDATA                     0x04
#define	    TRANSACTION_MASK                         0x0F
#define     TYPE_MASK 	                             0xF0
*/

#define ACPI_FUNCTION_GET_PHYSICAL_PRESENCE_INTERFACE_VERSION      1
#define ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS                       2
#define ACPI_FUNCTION_GET_PENDING_REQUEST_BY_OS                    3
#define ACPI_FUNCTION_GET_PLATFORM_ACTION_TO_TRANSITION_TO_BIOS    4
#define ACPI_FUNCTION_RETURN_REQUEST_RESPONSE_TO_OS                5
#define ACPI_FUNCTION_SUBMIT_PREFERRED_USER_LANGUAGE               6
#define ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS_2                     7
#define ACPI_FUNCTION_GET_USER_CONFIRMATION_STATUS_FOR_REQUEST     8
#define ACPI_FUNCTION_SET_MOR_VALUE_FLAG						   9



/*
#define SCAN_F1         EFI_SCAN_F1
#define SCAN_F2         EFI_SCAN_F2
#define SCAN_F3         EFI_SCAN_F3
#define SCAN_F4         EFI_SCAN_F4
#define SCAN_F5         EFI_SCAN_F5
#define SCAN_F6         EFI_SCAN_F6
#define SCAN_F7         EFI_SCAN_F7
#define SCAN_F8         EFI_SCAN_F8
#define SCAN_F9         EFI_SCAN_F9
#define SCAN_F10        EFI_SCAN_F10
#define SCAN_F11        EFI_SCAN_F11
#define SCAN_F12        EFI_SCAN_F12
#define SCAN_ESC        EFI_SCAN_ESC
*/

typedef struct _TCG_LOG_HOB_
{
    UINT32 TableMaxSize;
    UINT32 TableSize;
    UINT32 EventNum;
//    UINT32 Reserved;
    UINT32 Tcg2SpecVersion;
    UINT64 LastEntry;
    UINT64 LastEventPtr;
    UINT32 ActPcrBanks; // Activate PCR Banks BIOS Setting
    UINT32 Reserved;    // 64 Align
} TCG_LOG_HOB;

#pragma pack()
//--------------------------------------------------------------------------
// END_OF Internal Tcg Capabilities Headers and Struct
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//USED PE/COFF image headers
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------
// Description: TCG ACPI table. Requires PORTING: OEM data
// Input: None
// Output: None
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//PORTING STARTS
//-----------------------------------------------------------------------------
#define TCG_TBL_REV          0x02
#define TCG_OEMID            "ALASKA"            //; 6 charachters that identify the OEM
#define TCG_TBL_OEM_ID       0x4653414150414E    //"NAPAASF" //; 8 charachters as OEM manufacturer model ID
#define TCG_TBL_OEM_REV      TOKEN_TCG_TBL_OEM_REV       //; DWORD - Revision Number of this table,
#define TCG_CREATOR_ID       0x5446534D                  // "MSFT"
#define TCG_CREATOR_REVISION 0x01000013                   // TBD

#define FIRMWARE_MANUFACTURING_MODE_EVENT_STRING      "Manufacturing Mode"
#define FIRMWARE_MANUFACTURING_MODE_EVENT_STRING_LEN  (sizeof(FIRMWARE_MANUFACTURING_MODE_EVENT_STRING) - 1)

#define FIRMWARE_DEBUG_MODE_EVENT_STRING      "Debug Mode"
#define FIRMWARE_DEBUG_MODE_EVENT_STRING_LEN  (sizeof(FIRMWARE_DEBUG_MODE_EVENT_STRING) - 1)

#define FIRMWARE_RECOVERY_MODE_EVENT_STRING      "Recovery Mode"
#define FIRMWARE_RECOVERY_MODE_EVENT_STRING_LEN  (sizeof(FIRMWARE_RECOVERY_MODE_EVENT_STRING) - 1)

//-----------------------------------------------------------------------------
//Assuming we are in 32bit mode in PEI and the selectors are set up in real mode
//-----------------------------------------------------------------------------
#define SEL_flatDS      0x8
#define SEL_flatCS      0x10

typedef struct
{
    UINT32 Offset;
    UINT16 Selector;
    UINT32 Codep;
    UINT32 Size;
} FAR32LOCALS;

typedef struct
{
    UINT32 Offset;
    UINT16 Selector;
    UINT32 Codep;
} MASTRUCT;

typedef struct
{
    UINT16  Tag;
    UINT32  ParamSize;
    UINT32  RetCode;
    UINT32  respSize;
    UINT16  tag;
    BOOLEAN postInitialise;
    UINT32  localityModifier;
    BOOLEAN transportExclusive;
    BOOLEAN TOSPresent;
} IntTPM_STANY_FLAGS;


typedef struct
{
    UINT32 ReturnAddress;
    UINT32 Header;
    UINT8  FuncNum;
} ESPFUNCSTRUCT;

typedef struct
{
   UINT16 PpRequiredForTurnON   :1;
   UINT16 PpRequiredForTurnOff  :1;
   UINT16 PpRequiredForClear    :1;
   UINT16 PpRequiredForChangeEPS :1;
   UINT16 PpRequiredForChangePCRS :1;
   UINT16 SetPPRequiredForEnable_BlockSIDFunc_True :1;
   UINT16 SetPPRequiredForEnable_BlockSIDFunc_False :1;
   UINT16 SetPPRequiredForDisable_BlockSIDFunc_True :1;
   UINT16 SetPPRequiredForDisable_BlockSIDFunc_False :1;
   UINT16 Reserved :7;
}PPI1_3_FLAGS;


typedef struct
{
    UINT8  NoPpiProvision;
    UINT8  NoPpiClear;
    UINT8  NoPpiMaintenance;
    PPI1_3_FLAGS Ppi1_3_Flags;
} PERSISTENT_BIOS_TPM_FLAGS;


typedef
UINT8
(EFIAPI * CONFIRM_USER)(
);

typedef struct _AMI_CONFIRMATION_OVERRIDE_PROTOCOL
{
    CONFIRM_USER                  ConfirmUser;
} AMI_CONFIRMATION_OVERRIDE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI * AMI_TCG2_DXE_FUNCTION_OVERRIDE)(
);

typedef struct _AMI_TCG2_DXE_FUNCTION_OVERRIDE_PROTOCOL
{
    AMI_TCG2_DXE_FUNCTION_OVERRIDE   Function;
} AMI_TCG2_DXE_FUNCTION_OVERRIDE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI * READ_PERSISTENT_BIOS_TPM_FLAGS)(
   PERSISTENT_BIOS_TPM_FLAGS *
);

typedef
EFI_STATUS
(EFIAPI * SET_PERSISTENT_BIOS_TPM_FLAGS)(
   PERSISTENT_BIOS_TPM_FLAGS *
);


typedef struct _TPM_PERM_FLAGS
{
    TPM_Capabilities_PermanentFlag  Capabilities;                       
} TPM_PERM_FLAGS;

typedef struct _PERSISTENT_BIOS_TPM_MANAGEMENT_FLAGS_PROTOCOL
{
    READ_PERSISTENT_BIOS_TPM_FLAGS                 ReadBiosTpmflags;
    SET_PERSISTENT_BIOS_TPM_FLAGS                  SetBiosTpmflags;
} PERSISTENT_BIOS_TPM_MANAGEMENT_FLAGS_PROTOCOL;

EFI_STATUS
EFIAPI
CrbSubmitCmd(
  IN      UINT8     *InputBuffer,
  IN      UINT32     InputBufferSize,
  OUT     UINT8     *OutputBuffer,
  OUT     UINT32    *OutputBufferSize);

BOOLEAN isTpm20CrbPresent();


#define MA_FUNCTION_INIT   0x01
#define MP_FUNCTION_CLOSE  0x02
#define MP_FUNCTION_GET_STATUS  0x03
#define MP_FUNCTION_TRANSMIT    0x04
//-----------------------------------------------------------------------------
//PORTING ENDS
//-----------------------------------------------------------------------------

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
