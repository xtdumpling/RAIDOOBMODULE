//
// Struct
//
typedef struct {
    USHORT usVendorID;
    USHORT usDeviceID;	
} _PCI_DEVICE;

//
// Basic API
//
ULONG FindAstVgaDevice (USHORT, USHORT);
VOID SetPCIReg (ULONG, ULONG, ULONG);
ULONG GetPCIReg (ULONG, ULONG);

