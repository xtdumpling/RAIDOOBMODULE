#include  "MpService.h"
#include "Protocol/CpuCsrAccess.h"
#include "UBOX_CFG.h"
#include <Protocol/IioUds.h>
#include "Microcode.h"

//ESS Override Start
// 4168379: Odin: PL1 cannot be changed greater than 200W and lock bit set.
extern UINT32                              mTurboOverride;
//ESS Override End



#define LOAD_UCODE_PATCH23_SETUP_OPTION  1

extern MICROCODE_INFO *mMicrocodeInfo;



VOID 
BdeHookInitialize();

UINTN
GetNewSbspProcessNumber (
  UINT32 SocketNumber
  );

VOID WA_InitializeLLC(
  UINTN      ProcessorNumber
  );

VOID WA_Lock();




