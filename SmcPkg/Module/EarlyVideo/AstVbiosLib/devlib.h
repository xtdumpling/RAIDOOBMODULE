#include "typedef.h"
//#define (__386__)
#include "iolib.h"
#include "memlib.h"
#include "pcilib.h"

//#define ULONG UINT32
//#define USHORT UINT16
//#define UCHAR UINT16

//
// options
//
//#define RelocateIO		1
#define SoftwareTraining			0

//
// define
//
#define SCU_BASE		0x1e6e2000
#define I2C_BASE		0x1e78a000

//
// Struct
//
typedef struct _DP501INFO {
	
	UCHAR I2CAddress;				// DP501 I2C Address
    UCHAR ConnectStatus;			// connect/not connect	
    UCHAR SinkDeviceType;			// DP/DVI
     
} _DP501Info;	

typedef struct _DEVINFO {
	
    ULONG  PCIAddr;	
    USHORT usVendorID;
    USHORT usDeviceID;
    UCHAR  jRevision;

    UCHAR jDRAMType;
    ULONG ulMCLK;
    ULONG ulDRAMBusWidth;
        
    ULONG ulFBSize;
    ULONG ulFBBase;
//#if defined (__386__)     
    UCHAR *pjFBLinear;
//#endif        
    
    ULONG ulMMIOSize;            
    ULONG ulMMIOBase;
//#if defined (__386__)     
    UCHAR *pjMMIOLinear;     
//#endif       

    USHORT usRelocateIOBase;

//#if defined (__386__)     
    UCHAR *pjA0000Linear;
    UCHAR *pjB0000Linear;
    UCHAR *pjC0000Linear;                                
//#endif   
    
    _DP501Info	DP501Info;			// DP501

} _DEVInfo;

//
// Export
//
/* ProtoType */
VOID udelay(ULONG us);

ULONG ReadDD_SOC3(_DEVInfo *DevInfo, ULONG addr);
VOID WriteDD_SOC3(_DEVInfo *DevInfo, ULONG addr, ULONG data);
USHORT ReadDW_SOC3(_DEVInfo *DevInfo, ULONG addr);
VOID WriteDW_SOC3(_DEVInfo *DevInfo, ULONG addr, USHORT data);
UCHAR ReadDB_SOC3(_DEVInfo *DevInfo, ULONG addr);
VOID WriteDB_SOC3(_DEVInfo *DevInfo, ULONG addr, UCHAR data);

VOID SetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex, UCHAR jData);
UCHAR GetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex);

BOOL GetDevInfo(USHORT usVendorID, USHORT usDeviceID, _DEVInfo *DevInfo);

