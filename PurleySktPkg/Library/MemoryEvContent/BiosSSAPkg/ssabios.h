#ifndef __SSA_BIOS_H__
#define __SSA_BIOS_H__

#undef _DEBUG // to avoid warning C4005: '_DEBUG' : macro redefinition (see .\EdkCompatibilityPkg\Foundation\Library\EdkIIGlueLib\Include\Library\EdkIIGlueDebugLib.h line #238)

#ifdef SSA_FLAG

#ifndef SUPPORT_FOR_RDIMM
#define  SUPPORT_FOR_RDIMM (1)
#endif

#ifndef SUPPORT_FOR_EXTENDED_RANGE
#define  SUPPORT_FOR_EXTENDED_RANGE (1)
#endif

#ifndef SUPPORT_FOR_LRDIMM
#define  SUPPORT_FOR_LRDIMM (1)
#endif

#ifndef SUPPORT_FOR_DDRT
#define  SUPPORT_FOR_DDRT (1)
#endif

// PT comment out below three line for the minibios build. but i need them for the stitch content
//#ifndef SIM_BUILD
//#include "EdkIIGluePeim.h"
//#endif // SIM_BUILD
#ifndef MINIBIOS_BUILD
#include "SysHost.h"
//#include "Uefi.h"   // We need this for the EFI_XXXXX errorcode macros
//#include <Cpu\CpuBaseLib.h>
#endif // MINIBIOS_BUILD
#include "MrcSsaServices.h"
#include "bssalogging.h"
#include "memfunctions.h"

// disable warning of unreferenced formal parameter
#pragma warning( disable : 4100 )

#define BIOS_SSA_VER_MAJOR    1
#define BIOS_SSA_VER_MINOR    0
#define BIOS_SSA_VER_REVISION 0

typedef
EFI_STATUS
(EFIAPI * BIOS_SSA_TEST_ENTRY_POINT) (
  SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  VOID                  *Config
);

#endif  // SSA_FLAG
#endif
