//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
 
*/

#ifndef _ACPI_RES_DEFS
#define _ACPI_RES_DEFS

#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <AmiDxeLib.h>
#include <Acpi.h>
#include <Acpi20.h>
#include <Protocol/AcpiSupport.h>
//--------------------------------------------------------------------------------------------
#pragma pack(push, 1)
//--------------------------------------------------------------------------------------------
//**********************************************************************
//Definitions for ACPI resource descriptors defined in ACPI 2.0 spec 
// Starting from Section 6.4
//**********************************************************************

//-----------------------------------------------------------------------
//OUT of Resources Generic support definitions
//-----------------------------------------------------------------------
#define AMI_PCI_OUT_OF_RESOURCES_GUID   \
        { 0xf26dec72, 0x19cf, 0x4e26, 0xb1, 0xad, 0x0f,0x6a, 0xef, 0x4f, 0x1d, 0x47 }

//**********************************************************************
//resource descriptor terms
//**********************************************************************
//============================================================================================
//SMALL Resources Data Type
//============================================================================================
//All Resource type definitions starts with ASLR_and then ASLMACRO Name. 
//All ResourceField types definitions starts with ASLRF_ and then ASLMACRO Name ... 
//All Constatnt Definitions goes with names as defined in ASL Macro Parameters see
//ACPI spec section 16.2.4
#define ASLV_RES_TYPE_MSK	0x80
#define ASLV_SMALL_RES		0
#define ASLV_LARGE_RES		1

//Small resource common header
typedef union  _ASLRF_S_HDR {
	UINT8		HDR;
	struct{
		UINT8	Length	: 3;
		UINT8	Name	: 4;
		UINT8	Type	: 1;
	};
} ASLRF_S_HDR;

//Small resource Name definitions the valid part only nibble
#define ASLV_RT_IRQ					0x4
#define ASLV_RT_DMA					0x5
#define ASLV_RT_StartDependentFn	0x6 
#define ASLV_RT_EndDependentFn		0x7 
#define ASLV_RT_IO					0x8
#define ASLV_RT_FixedIO				0x9
#define ASLV_RT_VendorShort			0xe	
#define ASLV_SR_EndTag				0xf 
#define ASLV_END_TAG_HDR			0x79
//All other values are reserved		

//--------------------------------------------------------------------------------------------
//IRQ resource descriptor NoFlags
//--------------------------------------------------------------------------------------------
//IRQ Descr MAcros definition
#define ASLV_Nothing	0
#define ASLV_Edge 		1	//_HE
#define ASLV_Level 		0	//_HE
#define ASLV_ActiveHigh 0	//_LL
#define ASLV_ActiveLow	1	//_LL 
#define ASLV_Shared		1	//_SHR
#define ASLV_Exclusive 	0 	//_SHR, Nothing defaults to Exclusive

//type must be casted according to Length field value
//Note: If byte 3 is not included, High true, edge sensitive, non-shareable is assumed.
typedef struct _ASLR_IRQNoFlags {
	ASLRF_S_HDR		Hdr;
	UINT16			_INT;
}ASLR_IRQNoFlags; 

//--------------------------------------------------------------------------------------------
//IRQ descriptor Flags for ASL_RT_IRQ_L3
typedef union _ASLRF_IFLAGS{
	UINT8	IRQ_FLAGS;
	struct{
		UINT8	_HE		: 	1;	//0=Level-Triggered 1=Edge-Triggfered 
		UINT8	Ignored :	2;	//don't care bits 1..2 
		UINT8	_LL		: 	1;	//0=Active-High 1=Active-Low 
		UINT8	_SHR	:	1;	//Interrupt Shareble 1=
		UINT8   Reserved:	3;	//must be 0 bits 5..7
	};
}ASLRF_IFLAGS;

//Note: This descriptor is meant for describing interrupts that are connected to PICcompatible
//interrupt controllers, which can only be programmed for Active-High-
//Edge-Triggered or Active-Low-Level-Triggered interrupts. Any other
//combination is illegal. The Extended Interrupt Descriptor can be used to describe
//other combinations.

typedef struct _ASLR_IRQ {
	ASLRF_S_HDR		Hdr;
	UINT16			_INT;
	ASLRF_IFLAGS	Flags;
}ASLR_IRQ;

//--------------------------------------------------------------------------------------------
// DMA Resource Descriptor

//releated Definitions 
// _TYP, DMA channel speed
#define	ASLV_Compatibility 	0
#define	ASLV_TypeA			1
#define	ASLV_TypeB			2
#define	ASLV_TypeF 			3
// _BM, Nothing defaults to BusMaster
#define	ASLV_BusMaster		1
#define	ASLV_NotBusMaster	2
 // _SIZ, Transfer size
#define	ASLV_Transfer8		0
#define	ASLV_Transfer16		2
#define	ASLV_Transfer8_16	1

typedef union _ASLRF_DFLAGS {
	UINT8	DMA_FLAGS;
	struct {
		UINT8	_SIZ 	:	2;
		UINT8	_BM		:	1;
		UINT8	Ignored	:	2;
		UINT8	_TYP	:	2;
		UINT8	Reserved:	1;
	};
}ASLRF_DFLAGS;

typedef struct _ASLR_DMA{
	ASLRF_S_HDR		Hdr;
	UINT8			_DMA;
	ASLRF_DFLAGS 	Flags;						
}ASLR_DMA;

//--------------------------------------------------------------------------------------------
// Start Dependent Function Resource Descriptor

//releated Definitions 
#define	ASLV_Optimal		0
#define ASLV_Acceptable		1
#define ASLV_Suboptimal		2

typedef union _ASLRF_PFLAGS{
	UINT8	PR_FLAGS;
	struct {
		UINT8	_CMP	: 2;
		UINT8	_PRF	: 2;
		UINT8	Reserved: 4;
	};
}ASLRF_PFLAGS;

typedef struct _ASLR_StartDependentFn{
	ASLRF_S_HDR		Hdr;
	ASLRF_PFLAGS	_PRI;		//Priority	
}ASLR_StartDependentFn;

#define ASLR_StartDependentFnNoPri	ASLRF_S_HDR

//--------------------------------------------------------------------------------------------
// End Dependent Function Resource Descriptor

#define ASLR_EndDependentFn			ASLRF_S_HDR

//--------------------------------------------------------------------------------------------
// I/O Port Resource Descriptor

//releated Definitions 
#define ASLV_Decode16		1	// _DEC
#define ASLV_Decode10 		0 	// _DEC

typedef struct _ASLR_IO {
	ASLRF_S_HDR		Hdr;
	UINT8			_DEC;	
	UINT16			_MIN;
	UINT16			_MAX;
	UINT8			_ALN;
	UINT8			_LEN;
}ASLR_IO;

//this descriptor Assumes 10 bir IO
typedef struct _ASLR_FixedIO {
	ASLRF_S_HDR		Hdr;
	UINT16			_BAS;
	UINT8			_LEN;
}ASLR_FixedIO;

//--------------------------------------------------------------------------------------------
// Vendor Defined Short Resource Descriptor
#define ASLR_VendorShort	ASLRF_S_HDR

//--------------------------------------------------------------------------------------------
// END Resource Descriptor Tag

//Automaticaly generated by ASL compiller 
//at the end of ResourceTemplate statement
typedef struct _ASLR_EndTag {
	ASLRF_S_HDR		Hdr;
	UINT8			Chsum;
} ASLR_EndTag;

//============================================================================================
//LARGE Resources Data Type
//============================================================================================
//All Resource type definitions starts with ASLR_and then ASLMACRO Name. 
//All ResourceField types definitions starts with ASLRF_ and then ASLMACRO Name ... 
//All Constatnt Definitions goes with names as defined in ASL Macro Parameters see
//ACPI spec section 16.2.4

//Large Resource Name definitions the valid part bits 6..0
#define ASLV_RT_Memory24			0x1
#define ASLV_RT_Register			0x2
#define ASLV_RT_VendorLong			0x4 
#define ASLV_RT_Memory32			0x5
#define ASLV_RT_Memory32Fixed		0x6
#define ASLV_RT_DWORD_ASD			0x7
#define ASLV_RT_WORD_ASD			0x8
#define ASLV_RT_Interrupt			0x9 //extended Interrupt Descr
#define ASLV_RT_QWORD_ASD			0xA
//All other values are reserved		

//Large resource common header
typedef struct _ASLRF_L_HDR {
	union {
		UINT8	HDR;
		struct {
			UINT8		Name	: 7;
			UINT8		Type	: 1;
		};
	};
	UINT16			Length;
} ASLRF_L_HDR;

//--------------------------------------------------------------------------------------------
// 24 Bit Memory Range Resource Descriptor
// has not been used recently... but for compatibility sake


//releated Definitions 
#define ASLV_ReadWrite	 	1 // _RW
#define ASLV_ReadOnly		0 // _RW

//In _ASLR_MEM24 the lover 8 bits of the _MIN and _MAX of the addrss Assume to be 0;
//with this object we can specify fixed space making _MIN=_MAX.
typedef struct _ASLR_Memory24 {
	ASLRF_L_HDR		Hdr;
	UINT8			_RW;	//0=NonWrtitable 1=Writable
	UINT16			_MIN;
	UINT16			_MAX;
	UINT16			_ALN;
	UINT16			_LEN;
} ASLR_Memory24;


//--------------------------------------------------------------------------------------------
// Vendor Defined Long Resource Descriptor

#define ASLR_VendorLong	ASLRF_L_HDR;

//--------------------------------------------------------------------------------------------
// 32 Bit Memory Range Resource Descriptor
typedef struct _ASLR_Memory32 {
	ASLRF_L_HDR		Hdr;
	UINT8			_RW;	//0=NonWrtitable 1=Writable
	UINT32			_MIN;
	UINT32			_MAX;
	UINT32			_ALN;
	UINT32			_LEN;
} ASLR_Memory32;


//--------------------------------------------------------------------------------------------
// 32 Bit Memory Fixed Range Resource Descriptor

typedef struct _ASLR_Memory32Fixed {
	ASLRF_L_HDR		Hdr;
	UINT8			_RW;	//0=NonWrtitable 1=Writable
	UINT32			_BAS;
	UINT32			_LEN;
} ASLR_Memory32Fixed;
//Mixing 24 bit and 32 bit resources in the same device are not allowed


//--------------------------------------------------------------------------------------------
// Extended Interrupt Descriptor 

//Extended Interrupts Flags Definitions
typedef struct _ASLRF_XIFAGS{
	UINT8	_RPR	:	1;  //0=Device Consumes 1=Produces and Consume this Resource
	UINT8	_HE		: 	1;	//0=Level-Triggered 1=Edge-Triggfered 
	UINT8	_LL		: 	1;	//0=Active-High 1=Active-Low 
	UINT8	_SHR	:	1;	//Interrupt Shareble 1=
	UINT8   Reserved:	4;	//must be 0 bits 5..7
} ASLRF_XIFAGS;

typedef struct _ASLR_Interrupt {
	ASLRF_L_HDR		Hdr;
	ASLRF_XIFAGS	Flags;
	UINT8			IntTblLen;		
	UINT8			_INT[1];
} ASLR_Interrupt;

//--------------------------------------------------------------------------------------------
// Generic Register Descriptor 

// Definitions For _ASI field in ASLR_Register
#define ASLRV_SystemMem		0
#define ASLRV_SystemIo		1
#define ASLRV_PciConfSpc	2
#define ASLRV_EmbCtrl		3
#define ASLRV_SmBus			4
#define ASLRV_FixedHw		0x7f
//all other values are reserved

typedef struct _ASLR_Register {
	ASLRF_L_HDR		Hdr;
	UINT8			_ASI;
	UINT8			_RBW; //Register Width in Bits
	UINT8			_RBO; //Register Offset in Bits	
	UINT64			_ADR;			
} ASLR_Register;

//============================================================================================
//Address Space Resource Descriptor Internal definitions
//============================================================================================

//Definitions for _RPR Field in ASLRF_GFLAGS
#define ASLV_ResourceConsumer 	1
#define ASLV_ResourceProducer 	0

//Definitions for Type Field in ASLR_XXXXX_ASD
#define ASLRV_SPC_TYPE_MEM	0
#define ASLRV_SPC_TYPE_IO	1
#define ASLRV_SPC_TYPE_BUS	2
//3..255 are reserved

//Address Space Descriptor General Flags
typedef union _ASLRF_GFLAGS {
	UINT8		GFLAGS;
	struct {
		UINT8	_RPR		: 1;	//Bit 0=1 This Device only consume this resource
		UINT8	_DEC		: 1;	//if 1 Substractive Decoding 0 - Positive
		UINT8	_MIF		: 1;	//MIN address is Fixed
		UINT8	_MAF		: 1;	//MAX address is Fixed 
		UINT8	Reserved	: 4;
	};
} ASLRF_GFLAGS;

//type specific flag meaning for _MTP
#define ASLRV_MTP_MEM			0
#define ASLRV_MTP_RESERV		1
#define ASLRV_MTP_ACPI			2
#define ASLRV_MTP_NVS			3

//type specific flag meaning for _MEM
#define ASLRV_MEM_UC			0 //Non Cacheable
#define ASLRV_MEM_CE			1 //Cacheable
#define ASLRV_MEM_CEWC			2 //Cacheable and Supports Wrtite Combining
#define ASLRV_MEM_CEPF			3 //Cacheable and Prefatchable

//type specific flag meaning for _RNG
#define ASLRV_RNG_ISA_IO		2
#define ASLRV_RNG_NON_ISA		1

typedef union _ASLRF_TFLAGS_{
	UINT8		TFLAGS;
	struct {//for memory space descriptor type
		UINT8	_RW		:	1; //1=ReadWrite 0=ReadOnly
		UINT8	_MEM	:	2; //Memory Attributes see above
		UINT8	_MTP	: 	2; //Memory Type see above
		UINT8	_TTP 	: 	1; //Translation Type: 1=Translation 0=Static  
		UINT8	Reserved:	2;
	}MEM_FLAGS;
	struct {//For IO Space Descriptor
		UINT8	_RNG	: 	2; //Range see above
		UINT8	Reserved1:	2;
		UINT8	_TTP	:	1; //Translation Type: 1=Translation 0=Static  
		UINT8	_TRA	:	1; //Space Translation 1=Sparse; 0=Dense
		UINT8	Reserved2:	2;
	}IO_FLAGS;
} ASLRF_TFLAGS;

//This date structure mught be attached to any Address Space descriptior
//if ASD.Hdr.Length>sizeof(ASLR_XXXXX_ASD)
typedef struct _ASLRF_RES_SRC {
	UINT8		ResSrcIndex;   //Rersource index in following Device Oblect Resource tamplete
	UINT8		ResSrcName[1]; //Name of the Device Object	
} ASLRF_RES_SRC;

//--------------------------------------------------------------------------------------------
//QWORD Address Space Resource Descriptor Internal definitions
typedef struct _ASLR_QWORD_ASD {
	ASLRF_L_HDR		Hdr;
	UINT8			Type;
	ASLRF_GFLAGS	GFlags; //General Flags
	ASLRF_TFLAGS	TFlags;	//Type Specific Flags
	UINT64			_GRA;
	UINT64			_MIN;
	UINT64			_MAX;
	UINT64			_TRA;
	UINT64			_LEN;
} ASLR_QWORD_ASD;

//--------------------------------------------------------------------------------------------
//DWORD Address Space Resource Descriptor Internal definitions
typedef struct _ASLR_DWORD_ASD {
	ASLRF_L_HDR		Hdr;
	UINT8			Type;
	ASLRF_GFLAGS	GFlags; //General Flags
	ASLRF_TFLAGS	TFlags;	//Type Specific Flags
	UINT64			_GRA;
	UINT64			_MIN;
	UINT64			_MAX;
	UINT64			_TRA;
	UINT64			_LEN;
} ASLR_DWORD_ASD;

//--------------------------------------------------------------------------------------------
//WORD Address Space Resource Descriptor Internal definitions
typedef struct _ASLR_WORD_ASD {
	ASLRF_L_HDR		Hdr;
	UINT8			Type;
	ASLRF_GFLAGS	GFlags; //General Flags
	ASLRF_TFLAGS	TFlags;	//Type Specific Flags
	UINT64			_GRA;
	UINT64			_MIN;
	UINT64			_MAX;
	UINT64			_TRA;
	UINT64			_LEN;
} ASLR_WORD_ASD;

//Generic ASL resource header
typedef union _ASLR_HDR{
	ASLRF_S_HDR		SHdr;
	ASLRF_L_HDR		LHdr;	
} ASLR_HDR;

//Structure to store dependent functionresource descriptor pointers 
typedef struct _EFI_ASL_DepFn {
	VOID 			*DepFn;
	T_ITEM_LIST		DepRes;
}EFI_ASL_DepFn;

//typedef struct _EFI_ASL_DepFnNoPri {
//	ASLR_StartDependentFnNoPri 	*DepFn;
//	T_ITEM_LIST					DepRes;
//} EFI_ASL_DepFnNoPri;

//typedef union _EFI_ASL_DF{
//	EFI_ASL_DepFn		DFwP;	
//	EFI_ASL_DepFnNoPri	DFnP;
//}EFI_ASL_DF;


//AML Definition of NameSpace "Name" Object 
typedef struct _AML_NAME_OBJ {
	UINT8		OpCode;	
	UINT32   	ObjName;
    UINT8      	Preffix;
}AML_NAME_OBJ;

//AML Name Object Size Preffix 
#define AML_PR_BYTE		0x0A
#define AML_PR_WORD     0x0B
#define AML_PR_DWORD    0x0C
#define AML_PR_QWORD	0x0E
#define AML_ZERO_OP		0x00
#define AML_ONE_OP		0x01

//Opcode Definition for Name Object
#define AML_OP_ALIAS     0x06
#define AML_OP_NAME      0x08
#define AML_OP_SCOPE     0x10

typedef enum {
	tResBus,
	tResIoMem,
	tResAll,
	tResMax
}ASLR_TYPE_ENUM;

//--------------------------------------------------------------------------------------------
#pragma pack(pop)
//--------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
//Function Prototypes
//--------------------------------------------------------------------------------------------
VOID *ASLM_IRQ(UINT8 dsc_cnt, UINT8 ll, UINT8 he, UINT8 shr, ... );

VOID *ASLM_IRQNoFlags(UINT8 dsc_cnt, ... );

VOID *ASLM_DMA(UINT8 dsc_cnt, UINT8 typ, UINT8 bm, UINT8 siz, ... );

VOID *ASLM_StartDependentFn(UINT8 dsc_cnt,UINT8 pComp,UINT8 pPerf, ...);

VOID *ASLM_StartDependentFnNoPri(UINT8 dsc_cnt, ...);

VOID *ASLM_IO(UINT8 dec,UINT16 min,UINT16 max,UINT8 aln,UINT8 len);

VOID *ASLM_FixedIO(UINT16 bas, UINT8 len);

VOID *ASLM_VendorShort(UINT8 byte_cnt, ...);

VOID *ASLM_Memory24(UINT8 rw,UINT8 min, UINT8 max, UINT8 aln, UINT8 len);

BOOLEAN ValidateDescriptor(ASLR_QWORD_ASD *Descriptor, ASLR_TYPE_ENUM ResType, BOOLEAN Fixed);

UINTN ValidateDescriptorBlock(ASLR_QWORD_ASD* DscStart, ASLR_TYPE_ENUM ResType, BOOLEAN Fixed);
//--------------------------------------------------------------------------------------------

//AMP OP_CODE TYPE Definition
#define AML_OP_CODE		UINT8	//Normal OP_CODE
#define AML_EOP_CODE	UINT16	//Extended OP_CODE


//AML Name Object Size Preffix 
#define AML_PR_BYTE		0x0A
#define AML_PR_WORD     0x0B
#define AML_PR_DWORD    0x0C
#define AML_PR_STRING	0x0D
#define AML_PR_QWORD	0x0E

#define AML_PR_DUALNAME	0x2E
#define AML_PR_MULTNAME	0x2F


#define AML_PR_EXTOP	0x5B	//'['
#define AML_PR_ROOT		0x5C	//'\'
#define AML_PR_PARENT	0x5E	//'^'


//Opcode Definition for Name Object
#define AML_OP_ZERO			0x00
#define AML_OP_ONE			0x01
#define AML_OP_ONES			0xFF


#define AML_OP_ALIAS		0x06
#define AML_OP_NAME			0x08
#define AML_OP_SCOPE		0x10
#define AML_OP_BUFFER		0x11
#define AML_OP_PACKAGE		0x12
#define AML_OP_VARPACK		0x13
#define AML_OP_METHOD		0x14

#define AML_OP_RETURN		0xA4

#define AML_OP_OPREG		0x80
#define AML_OP_DEVICE		0x82
#define AML_OP_PROC			0x83 
#define AML_OP_PWRRES		0x84
#define AML_OP_THERMAL		0x85


#define AML_NULL_NAME		0x00


//==============================================================
//AML Definition for AML Objects NameString
//==============================================================
#pragma pack(push, 1)

//--------------------------------------------------
//LeadNameChar := 'A'-'Z' | '_' := 0x5f
//DigitChar := '0'-'9'
//NameChar := DigitChar | LeadNameChar
//RootChar := '\'=0x5c
//ParentPrefixChar := '^'=0x5e
//NullName := 0x00
//--------------------------------------------------

//--------------------------------------------------
//NameSeg := <LeadNameChar NameChar NameChar NameChar>
// Notice that NameSegs shorter than 4 characters are
// filled with trailing '_'s.
typedef union _AML_NAME_SEG{
	UINT32		NAME;
	UINT8		Name[4];
}AML_NAME_SEG;

//--------------------------------------------------------------
//DualNamePath := DualNamePrefix NameSeg NameSeg
//DualNamePrefix := 0x2e
typedef struct _AML_DUAL_NAME_PATH{
	AML_OP_CODE		OpCode;
	AML_NAME_SEG	Name1;
	AML_NAME_SEG	Name2;
}AML_DUAL_NAME_PATH;

//--------------------------------------------------------------
//MultiNamePath := MultiNamePrefix SegCount NameSeg(SegCount)
//MultiNamePrefix := 0x2f
//SegCount := ByteData
typedef struct _AML_MULTI_NAME_PATH{
	AML_OP_CODE		OpCode;
	UINT8			SegCount;
	AML_NAME_SEG	Name[1];
}AML_MULTI_NAME_PATH;


////////////////////////////////////////////////////////////////
//Some NAMED objects definitions
////////////////////////////////////////////////////////////////

//Type of object
typedef enum {
	otNone=0,
	otScope,
	otBuffer,
	otName,
	otProc,
	otTermal,
	otDevice,
	otMethod,
	otPwrRes,
	otOpReg
} ASL_OBJ_TYPE;

//Structure to store ASL Object Information
typedef struct _ASL_OBJ_INFO {
	VOID			*Object;
	VOID			*ObjName; 
	VOID			*DataStart;
	ASL_OBJ_TYPE	ObjType;
	UINTN			Length;
} ASL_OBJ_INFO;


//AML Pckage Definition 
typedef struct _AML_PACKAGE_LBYTE {
	UINT8		Length	  :	6;
	UINT8		ByteCount : 2;	
	//Package Length Following Bytes 0..3
} AML_PACKAGE_LBYTE;


typedef struct _AML_PACKAGE{
	AML_OP_CODE			OpCode;	
	AML_PACKAGE_LBYTE	Length;
//	UINT8				NumElements;
//  PckageElements:=ObjectList
} AML_PACKAGE;


typedef struct _AMI_OUT_OF_RES_VAR {
    ASLR_QWORD_ASD  Resource;
    UINTN           Count;    
} AMI_OUT_OF_RES_VAR;


// PackageElements

#define ELEMENT_UINT8_PREFIX    0xA
#define ELEMENT_UINT16_PREFIX   0xB
#define ELEMENT_UINT32_PREFIX   0xC
#define ELEMENT_STRING_PREFIX   0xD
#define ELEMENT_UINT64_PREFIX   0xE

typedef struct {
	UINT8	Prefix;	 
	UINT8	Data;
} PACKAGE_ELEMENT_UINT8;

typedef struct {
	UINT8	Prefix;
	UINT16	Data;
} PACKAGE_ELEMENT_UINT16;

typedef struct {
	UINT8	Prefix;
	UINT32	Data;
} PACKAGE_ELEMENT_UINT32;

typedef struct {
	UINT8	Prefix;
	UINT64	Data;
} PACKAGE_ELEMENT_UINT64;

typedef union {
    VOID                    *Ptr;
    PACKAGE_ELEMENT_UINT8   *Type8;
    PACKAGE_ELEMENT_UINT16  *Type16;
    PACKAGE_ELEMENT_UINT32  *Type32;
    PACKAGE_ELEMENT_UINT64  *Type64;
} PACKAGE_ELEMENT;

typedef struct {
	UINT8  OpCode;                  // 0x11 is opcode for Package Objects
    union  
    {
        struct {
            UINT8  Length      :6;  // [5:0] Length if ByteCount == 0
            UINT8  ByteCount   :2;  // [7:6] # Bytes that follow
            UINT8  ElementCount;
            VOID   *Element;
        } Small;
        struct {
            UINT8  LowNibble   :4;  // [3:0] low nibble if ByteCount > 0
            UINT8  Reserved    :2;  // [5:4] reserved if ByteCount > 0
            UINT8  ByteCount   :2;  // [7:6] # Bytes that follow
            UINT8  Byte1;           // [15:8]
            UINT8  ElementCount;    // [23:16]
            VOID   *Element;
        } Large1;
        struct {
            UINT8  LowNibble   :4;  // [3:0] low nibble if ByteCount > 0
            UINT8  Reserved    :2;  // [5:4] reserved if ByteCount > 0
            UINT8  ByteCount   :2;  // [7:6] # Bytes that follow
            UINT8  Byte1;           // [15:8]
            UINT8  Byte2;           // [23:16]
            UINT8  ElementCount;    // [31:24]
            VOID   *Element;
        } Large2;
        struct {
            UINT8  LowNibble   :4;  // [3:0] low nibble if ByteCount > 0
            UINT8  Reserved    :2;  // [5:4] reserved if ByteCount > 0
            UINT8  ByteCount   :2;  // [7:6] # Bytes that follow
            UINT8  Byte1;           // [15:8]
            UINT8  Byte2;           // [23:16]
            UINT8  Byte3;           // [31:24]
            UINT8  ElementCount;    // [39:32]
            VOID   *Element;
        } Large3;
    } Encoding;
} AML_PACKAGE_HEADER;
#pragma pack(pop)



//BOOLEAN CheckOpCode(UINT8 *NameStart, UINT8 OpCode, UINT8 ExtOpCode, ASL_OBJ_INFO *ObjInfo);
VOID* FindAslObjectName(UINT8 *Start, UINT8 *ObjName, UINTN Len);

EFI_STATUS GetAslObj(UINT8 *Start, UINTN Length, UINT8 *Name, ASL_OBJ_TYPE ObjType, ASL_OBJ_INFO *ObjInfo);
UINT8 ChsumTbl(UINT8* TblStart, UINT32 BytesCount);
EFI_STATUS	UpdateAslNameObject(ACPI_HDR *PDsdt, UINT8 *ObjName, UINT64 Value);
UINTN GetPackageLen(AML_PACKAGE_LBYTE *Pck, UINT8 *BytesCnt);
EFI_STATUS UpdateAslNameOfDevice(ACPI_HDR *PDsdt, CHAR8 *DeviceName, CHAR8 *ObjName, UINT64 Value);
VOID HideAslMethodFromOs (ASL_OBJ_INFO *AslObj, UINT8 *MethodName);

EFI_STATUS LibGetDsdt(EFI_PHYSICAL_ADDRESS *DsdtAddr, EFI_ACPI_TABLE_VERSION Version);

EFI_STATUS AmiPciOutOfRes(AMI_OUT_OF_RES_VAR *AmiOutOfResData, BOOLEAN Get);

EFI_STATUS GetRbAmlPackagePointer(
    IN  ACPI_HDR            **Dsdt,
    IN  CHAR8               *RbName,
    IN  CHAR8               *PackageName,
    OUT ASL_OBJ_INFO        *Package,
    OUT PACKAGE_ELEMENT     *Elements,
    OUT UINT8               *ElementCount );

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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
