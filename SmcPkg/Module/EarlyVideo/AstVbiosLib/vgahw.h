/*
 * Init Mode
 */
#define Init_IO			0x00000001
#define Init_MEM		0x00000002
#define Init_DVO		0x00000004
#define Init_DP501		0x00000008
 
/*
 * I/O Access
 */
#ifdef RelocateIO
#define AR_PORT_WRITE		(DevInfo->usRelocateIOBase + 0x40)
#define MISC_PORT_WRITE		(DevInfo->usRelocateIOBase + 0x42)
#define VGA_ENABLE		(DevInfo->usRelocateIOBase + 0x43)
#define SEQ_PORT 		(DevInfo->usRelocateIOBase + 0x44)
#define DAC_INDEX_WRITE		(DevInfo->usRelocateIOBase + 0x48)
#define DAC_DATA		(DevInfo->usRelocateIOBase + 0x49)
#define GR_PORT			(DevInfo->usRelocateIOBase + 0x4E)
#define CRTC_PORT 		(DevInfo->usRelocateIOBase + 0x54)
#define INPUT_STATUS1_READ	(DevInfo->usRelocateIOBase + 0x5A)
#else   //Std. VGA IO
#define AR_PORT_WRITE		0x3c0	//(dinfo->usRelocateIOBase + 0x40)
#define MISC_PORT_WRITE		0x3c2	//(dinfo->usRelocateIOBase + 0x42)
#define VGA_ENABLE		0x3c3	//(dinfo->usRelocateIOBase + 0x43)
#define SEQ_PORT 		0x3c4	//(dinfo->usRelocateIOBase + 0x44)
#define DAC_INDEX_WRITE		0x3c8	//(dinfo->usRelocateIOBase + 0x48)
#define DAC_DATA		0x3c9	//(dinfo->usRelocateIOBase + 0x49)
#define GR_PORT			0x3ce	//(dinfo->usRelocateIOBase + 0x4E)
#define CRTC_PORT 		0x3d4	//(dinfo->usRelocateIOBase + 0x54)
#define INPUT_STATUS1_READ	0x3da	//(dinfo->usRelocateIOBase + 0x5A)
#endif

/*
 * VRAM
 */
#define VRAM_SIZE_064M		0x04000000
#define VRAM_SIZE_032M		0x02000000
#define VRAM_SIZE_016M		0x01000000
#define VRAM_SIZE_008M		0x00800000

/*
 * Struct
 */
typedef struct ExtRegInfo {
    UCHAR jIndex;
    UCHAR jData;	
} ast_ExtRegInfo;

typedef struct DRAMRegInfo {
    ULONG ulIndex;
    ULONG ulData;	
} ast_DRAMRegInfo;

enum ast_chips {
	AST2000,
	AST2100,
	AST2300
};

enum ast_dram_chips {
	DRAMTYPE_512Mx16,
	DRAMTYPE_1Gx16,
	DRAMTYPE_512Mx32,
	DRAMTYPE_1Gx32,
	DRAMTYPE_2Gx16,
	DRAMTYPE_4Gx16,
};

/*
 * export
 */
void GetVRAMInfo(_DEVInfo *DevInfo);
void GetDRAMInfo(_DEVInfo *DevInfo);

void DisplayOFF(_DEVInfo *DevInfo);
void DisplayON(_DEVInfo *DevInfo);

//void SetDPMSOFF(_DEVInfo *DevInfo);
//void SetDPMSON(_DEVInfo *DevInfo);

//BOOL LoadM68K(_DEVInfo *DevInfo, CHAR *FW);
//BOOL UnloadM68K(_DEVInfo *DevInfo);

void InitVGA_HW(_DEVInfo *DevInfo, ULONG InitOption);



