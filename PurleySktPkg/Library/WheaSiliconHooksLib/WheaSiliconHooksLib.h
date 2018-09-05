/*
 * WheaSiliconHooksLib.h
 *
 *  Created on: Jun 3, 2014
 *      Author: mganesan
 *  Modifications:
 *      04/16/15: jpborja: Moved structs, definitions and function prototypes from PlatPkg for RASCM effort.
 */

#ifndef WHEASILICONHOOKSLIB_H_
#define WHEASILICONHOOKSLIB_H_


#include <Uefi.h>
#include <Protocol/CpuCsrAccess.h>
#include <PiDxe.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MemRasProtocol.h>
#include <Library/DebugLib.h>
#include <CpuRegs.h>
#include <Protocol/IioUds.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/LocalApicLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/MpService.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Library/SynchronizationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Protocol/MemRasProtocol.h>
#include <Register/LocalApic.h> 
#include <MemHost.h>
#include "RcRegs.h"
#include "ErrorRecords.h"
#include "emca.h"

#include "WheaPlatformDefs.h"
#include <Protocol/WheaBootProtocol.h>
#include <Library/PciExpressLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include "LastBootErrorLogHob.h"
#include "mca_msr.h"
#include "CpuDataStruct.h"
#include <Library/mpsyncdatalib.h>


// 
// WHEA error injection 
// 
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8        7
#define PCH_PCIE_RROT_PORT_AER_UES                      0x104
#define PREV_BOOT_ERR_SRC_HOB_SIZE                      1000
#define WHEA_EINJ_TRIGGER_ERROR_ACTION                  0xFF
#define INJECTION_ADDRESS_VALUE                         0xDEADBEEF;

#define MC_PS_INT_DOWNGRADE							0x17f
#define V_PS_INT_DOWNGRADE							0x4  // BIT2

//TODO: TBD is a table to contain both IVT and HSX values. Need to re-route code to use the table instead.
#define IVT_CORE_MCBANK_NUM_START	0
#define IVT_CORE_MCBANK_NUM_END		3
#define IVT_QPI_MCBANK_NUM_START	4
#define IVT_QPI_MCBANK_NUM_END		5
#define IVT_MEM_MCBANK_NUM_START	7
#define IVT_MEM_MCBANK_NUM_END		16
#define IVT_CBO_MCBANK_NUM_START	17
#define IVT_CBO_MCBANK_NUM_END		31

#define MC_ERR_INJ_LCK          0x790
#define MSR_MCG_CONTAIN         0x178


#define PCI_ADDRESS(seg, bus, dev, func, reg) \
    ((UINTN) ((((UINTN) bus) << 20) + (((UINTN) dev) << 15) + (((UINTN) func) << 12) + ((UINTN) reg)))

    #define CORE_IFU_BANK           0
    #define CORE_DCU_BANK           1

	//Core mcbank offsets
	#define CORE_MCBANK_NUM_START	0
	#define CORE_MCBANK_NUM_END		3

	//KTI mcbank offsets
	#define KTI0_MCBANK_NUM_START	4
	#define KTI0_MCBANK_NUM_END		5

	#define KTI1_MCBANK_NUM			12

	#define KTI2_MCBANK_NUM			19

    #define MEM_M2M_0_BANK          7
    #define MEM_M2M_1_BANK          8

	//Memory mcbank offsets
	#define MEM_M2M_MCBANK_NUM_START	MEM_M2M_0_BANK
	#define MEM_M2M_MCBANK_NUM_END		MEM_M2M_1_BANK

	//Cbo MC Bank offsets
	#define CHA_MCBANK_NUM_START	9
	#define CHA_MCBANK_NUM_END		11

    #define MEM_IMC0_CH0_BANK    13
    #define MEM_IMC0_CH1_BANK    14
    #define MEM_IMC1_CH0_BANK    15
    #define MEM_IMC1_CH1_BANK    16
    #define MEM_IMC0_CH2_BANK    17
    #define MEM_IMC1_CH2_BANK   18

	#define MEM_IMC_MCBANK_NUM_START	MEM_IMC0_CH0_BANK
	#define MEM_IMC_MCBANK_NUM_END		MEM_IMC1_CH2_BANK

typedef struct {
  UINT16          WheaErrorRecordFormat;
  BOOLEAN         SetupMcBankStsClear;
  BOOLEAN         PciexErrFound;
} PREV_BOOT_ERR_GLOBAL_DATA;

typedef struct {
  UINT8 Enabled;
	UINT8	Bus;
  UINT8 Device;
  UINT8 Function;
} PCIE_PORT_INFO;


typedef struct {
  UINT8 BusNum;
  UINT8 SocketId;
  UINT8 Core20BusNum;
  UINT8 Valid;
  PCIE_PORT_INFO Port[NUMBER_PORTS_PER_SOCKET];
} IOH_INFO;


#pragma pack (1)
 
 typedef struct {
  UINT32                HeaderSize;
  UINT32                Revision;
  UINT32                TableSize;
  UINT32                EntryCount;
} WHEA_ERROR_TRIGGER_ACTION_HEADER;

typedef struct {
  WHEA_ERROR_TRIGGER_ACTION_HEADER              Header;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger0;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger1;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger2;
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            Trigger3;
} WHEA_EINJ_TRIGGER_ACTION_TABLE;


typedef struct {
  UINT64                Busy:1;
  UINT64                CmdSts:8;
  UINT64                Resv1:55;
} WHEA_EINJ_OP_STATUS;

typedef struct {
  UINT32		ErrType;
  UINT32		VendorErrTypeExtOffset;
  UINT32		Flags;
  UINT32		ApicId;
  UINT64		MemAddress;
  UINT64		MemAddressRange;
  UINT32		PcieSBDF;
} WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRESS;

typedef struct {
  UINT32                Length;		
  UINT32                SBDF;		// This provides a PCIe Segment, Bus, Device and Function number which can be used to read the Vendor ID, Device ID and Rev ID, 
					// so that software can identify the system for error injection purposes            
  UINT16                VendorId;
  UINT16                DeviceId;
  UINT8                 RevId;
  UINT8                 Resvd[3];
  UINT64		            OemDefType1;
  UINT64		            OemDefType2;
  UINT64                OemDefType3;
} WHEA_EINJ_VENDOR_EXT;

typedef struct {
  UINT64                OpState;            // opeartion state, begin/end/etc
  UINT64                ErrorToInject[5];
  UINT64                TriggerActionTable;
  UINT64                ErrorInjectCap;    // Qowrd0 -  Eroor injection cap.
  UINT64                OpStatus;
  UINT64                CmdStatus;
  UINT64                EinjAddr;
  UINT64                AddrMask;
  WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRESS  SetAddressTable;
  WHEA_EINJ_VENDOR_EXT  VendorExt;  
  BOOLEAN               PcieErrInjActionTable;  
  UINT8                 Skt;
} WHEA_EINJ_PARAM_BUFFER;

#pragma pack ()


extern IOH_INFO IohInfo[8];



VOID CheckAndUpdatePciExError(VOID);
EFI_STATUS
ChecAndUpdatePrevBootErrors(
  VOID
  );
void
LockInjLogic(
  UINT8 *node
);
void
UnLockInjLogic(
  UINT8 *node
);
void
DisablePoison(
  UINT8 *node
);
void
EnablePoison(
  UINT8 *node
);
VOID
SetupLlcInjRegister(
  WHEA_EINJ_PARAM_BUFFER *EinjParam);
void
IntDownGrade(
  UINT8 *node
);
  
EFI_STATUS
InjectPciExError (
 IN   UINT32      PcieSBDF,
 IN   UINT32      ErrorToInject,
 IN   WHEA_EINJ_TRIGGER_ACTION_TABLE  *EinjAction,
 IN   WHEA_EINJ_PARAM_BUFFER          *EinjParam
 );
 
VOID
TurnOnQuiesce (
  VOID
  );

VOID
TurnOffQuiesce (
  VOID
  );

VOID
AsmMfence (
  VOID
  );



VOID SetHAParityCheckEnable(
  BOOLEAN enable
  );

#endif /* WHEASILICONHOOKSLIB_H_ */
