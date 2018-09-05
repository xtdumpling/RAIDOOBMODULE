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
	This file contains all functionality of the TseDefaultBootOrder eModule.

*/

#include <BootOptions.h>
#include "TseDefaultBootOrder.h"

//----------------------------------------------------------------------------

/**
	This function returns result of comparing boot option tags and BbsIndex value.
	
	This function takes two linked list items and does a comparison to determine which
	has a higher priority.  First it looks at the Tag values and then if that is the 
	same it looks at the BbsIndex values.  If both are the same a 0 is returned.
	
	@param Link1 - pointer to the first tag for compare
	@param Link2 - pointer to the second tag for compare
	
	@return INT32
	@retval -1  - Link1 < Link2
	@retval 0   - Link1 == Link2
	@retval 1   - Link1 > Link2
 */
INT32 CompareTagThenBbsIndex( IN DLINK *Link1, IN DLINK *Link2 )
{
    BOOT_OPTION *Option1 = (BOOT_OPTION*)Link1;
    BOOT_OPTION *Option2 = (BOOT_OPTION*)Link2;
    
    // Compare by Tag values first
    if (Option1->Tag < Option2->Tag) 
        return -1;
    else if (Option1->Tag > Option2->Tag) 
        return 1;

    // If the Tag values are the same, 
    // Compare by BBS index number
    if (Option1->BbsIndex < Option2->BbsIndex) 
        return -1;
    else if (Option1->BbsIndex > Option2->BbsIndex) 
        return 1;
    
    // If we have not found a difference, then return 0
    return 0;
}

/**
    This function sets variables: DefaultLegacyDevOrder and DefaultBootOrder.
    
    This function takes the current boot order and then it creates two NVRAM 
    variables, DefaultLegacyDevOrder and DefaultBootOrder, by sorting the order 
    based on rules defined at build time. The function then calls SetVariable 
    for each of those variables with attributes of BOOT_VARIABLE_ATTRIBUTES.
    Once created, these variables are available for the boot process to use if
    the user chooses to load the default boot order.

	@return VOID
	
	@note In the case where CSM is disabled or not present, the code to set variable
	DefaultLegacyDevOrder is disabled.
 */
VOID SetDefaultTseBootOrder(VOID)
{
	#if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
	UINTN DevOrderSize;
	LEGACY_DEVICE_ORDER *DevOrder;
	EFI_GUID DefaultLegacyDevOrderGuid = DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;
	LEGACY_DEVICE_ORDER *Order;
	UINTN  i;
	#endif  // #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    UINTN BootOrderSize;
    UINT16 *BootOrder;
    UINT16 PreviousBootOptionNumber;
    DLINK *Link;
    UINTN BootOrderIndex;
    BOOT_OPTION *Option;

    EFI_GUID DefaultBootOrderGuid = DEFAULT_BOOT_ORDER_VARIABLE_GUID;


    // PRECONDITION: All Boot Option Numbers are set
    BootOrderSize = BootOptionList->Size * sizeof(UINT16);
    if (BootOrderSize == 0) 
        return;
    
    SortList(BootOptionList, CompareTagThenBbsIndex);

    #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    // Build DefaultLegacyDevOrder variable
    BuildLegacyDevOrderBuffer(&DevOrder, &DevOrderSize);

    // Remove disable mask from buffer DevOrder
    for( Order = DevOrder; 
         (UINT8*)Order < (UINT8*)DevOrder + DevOrderSize; 
         Order = (LEGACY_DEVICE_ORDER*)((UINT8*)Order + Order->Length + sizeof(Order->Type)) )
        for( i = 0; i < (Order->Length / sizeof(UINT16) - 1); i++ )
            Order->Device[i] &= 0x00FF;

    // Store "DefaultLegacyDevOrder" EFI variable
    pRS->SetVariable(
        L"DefaultLegacyDevOrder", 
        &DefaultLegacyDevOrderGuid,
        BOOT_VARIABLE_ATTRIBUTES,
        DevOrderSize,
        DevOrder
    );
    pBS->FreePool(DevOrder);
	#endif  // #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    // Build DefaultBootOrder variable
    BootOrder = Malloc(BootOrderSize);
    PreviousBootOptionNumber = INVALID_BOOT_OPTION_NUMBER;
    BootOrderIndex = 0;

    // Set boot option number for each boot option
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        if ( (PreviousBootOptionNumber == Option->BootOptionNumber) ||
             (Option->Attributes & LOAD_OPTION_HIDDEN) )
            continue;
        BootOrder[BootOrderIndex++] = Option->BootOptionNumber;
        PreviousBootOptionNumber = Option->BootOptionNumber;
    }
    
    // Put hidden boot option with lowest priority.
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        if ( (PreviousBootOptionNumber == Option->BootOptionNumber) ||
             !(Option->Attributes & LOAD_OPTION_HIDDEN) )
            continue;
        BootOrder[BootOrderIndex++] = Option->BootOptionNumber;
        PreviousBootOptionNumber = Option->BootOptionNumber;        
    }
    
    // Store "DefaultBootOrder" EFI variable
    pRS->SetVariable(
        L"DefaultBootOrder", 
        &DefaultBootOrderGuid,
        BOOT_VARIABLE_ATTRIBUTES, 
        BootOrderIndex * sizeof(UINT16), 
        BootOrder
    );
    pBS->FreePool(BootOrder);
}


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
