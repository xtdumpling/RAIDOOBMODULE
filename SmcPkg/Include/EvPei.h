
#ifndef __PEI_EV_PPI_H_
#define __PEI_EV_PPI_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <Pei.h>



#ifndef GUID_VARIABLE_DECLARATION
#define GUID_VARIABLE_DECLARATION(Variable, Guid) extern EFI_GUID Variable
#endif

GUID_VARIABLE_DECLARATION(gEfiEvPpiGuid, EFI_EV_PPI_GUID);

typedef struct {
   BOOLEAN EarlyVideoOn;
} EFI_PEI_EV_PPI;



/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

