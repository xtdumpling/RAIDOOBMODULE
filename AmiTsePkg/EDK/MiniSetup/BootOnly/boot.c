
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/boot.c $
//
// $Author: Rajashakerg $
//
// $Revision: 41 $
//
// $Date: 5/28/12 6:09a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file boot.c
    This file contains code for Boot management

**/

#include "minisetup.h"
#include EFI_PROTOCOL_DEFINITION (BlockIo)
#include EFI_PROTOCOL_DEFINITION (UsbIo)
#include EFI_PROTOCOL_DEFINITION (DiskIo)
#define BOOT_OPTION_ALLOC_UNIT  10
//////////////////////////////////////
//Extern Variables
//////////////////////////////////////
extern UINTN        	gDelOptionCount;
extern BOOT_DATA    	*gDelBootData;
extern UINT32       	gBootFlow;
extern UINTN 			CurrentScreenresolutionX, CurrentScreenresolutionY; 
extern BOOLEAN gBootNextFlag;
extern UINT8 IsImageCodeTypeSupport (VOID);
extern UINT8 gRecoveryBootingType;
extern BOOT_DATA *RecoveryGetRecoveryData( UINT16 Option, UINT8 RecoveryBootingType);
extern void PrintGopAndConInDetails();
extern EFI_GUID gEfiLoadFileProtocolGuid;

//////////////////////////////////////
//Internal Variable Declarations
//////////////////////////////////////
UINTN		        		gBootOptionCount;
EFI_GUID	        		gBootNowCountGuid 	= 	BOOT_NOW_COUNT_GUID;
UINT16		        *gBBSDisabled 			= 	L"Disabled in BBS Order";
UINTN		        		gLangCount;
BOOT_DATA		    	*gBootData;
LANGUAGE_DATA	    	*gLanguages;
EFI_HANDLE          	gCurrentBootHandle 	= 	NULL;
EFI_EVENT				gShellLaunchEvent 	= NULL;		
EFI_EVENT				gShell20LaunchEvent 	= NULL;	
UINT16 					DISABLED_BOOT_OPTION; 
UINT16 					DISABLED_DRIVER_OPTION; 
UINTN						gDriverOptionCount;
BOOT_DATA				*gDriverData;
BOOLEAN              gBrowserCallbackEnabled = FALSE; 
BOOLEAN              gFileParsed = FALSE; 
BOOT_DATA				*gOSRecoveryData;
BOOT_DATA				*gPlatformRecoveryData;
BOOLEAN              gWatchDogTimerSet = FALSE; 
UINTN						gOSRecoveryOptionCount;
UINTN						gPlatformRecoveryOpCount;					
//////////////////////////////////////
//Internal Functions Declarations
//////////////////////////////////////
EFI_DEVICE_PATH_PROTOCOL *_BootBuildFVDevicePath( UINT32 *index, EFI_GUID *guidPtr );
EFI_DEVICE_PATH_PROTOCOL *_BootBuildFileDevicePath( UINT32 *index, CHAR16 *fileName );
EFI_DEVICE_PATH_PROTOCOL* _DiscoverPartition(EFI_DEVICE_PATH_PROTOCOL *DevicePath);
VOID        _BootSetBootManagerVariables(VOID);
VOID        _BootInstallLoadOptions( EFI_HANDLE handle, VOID *Options, UINTN OptionSize );
EFI_STATUS  _BootLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath, VOID *Options, UINTN OptionSize, BOOLEAN ValidBootOption );
UINT16      _BootSetBootNowCount(VOID);
CHAR16      *TseGetUefiDevPathString(EFI_DEVICE_PATH_PROTOCOL *DevPath);
VOID        InvalidateStatusInBgrtWrapper (VOID);
VOID        UninstallBgrtWrapper(VOID);
VOID        FormHotBootKeys (CHAR16 *VarName);
VOID        RefreshBootKeysDetails (VOID);
VOID        FreeExtraKeyMemories (VOID);
EFI_STATUS	CheckForDeviceNeedRepair (EFI_DEVICE_PATH_PROTOCOL *);
EFI_HANDLE	CheckDevSupShortFormPath (EFI_DEVICE_PATH_PROTOCOL *);
VOID 			LegacyBootFailHook(EFI_STATUS);
VOID			UefiBootFailHook (EFI_STATUS);
VOID			UpdateBootVariables ();
VOID			FixHiddenOptions (BOOLEAN, UINT16 **, UINTN);
VOID 			_SetDriverManagerVariables (VOID);
VOID 			UpdateDriverVariables ();
UINT32  		ShellTextMode (VOID);
UINTN   		PISpecVersion(VOID);
VOID 			 ReInstallBgrtWrapper ();
UINT32 		SkipOrphanBootOrderVar();
BOOLEAN TSESkipBootOrderVar(UINT16 *BootOrder, UINTN BootOrderSize,CHAR16 *VarName);
CHAR16* GetFileNameFromDevPath( EFI_DEVICE_PATH_PROTOCOL *DevPath);
BOOLEAN TSEIsBootOptionValid(BOOT_OPTION* Buffer, UINTN Size);
EFI_DEVICE_PATH_PROTOCOL* EfiHTTPSGetFullDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath);
BOOLEAN IsURIDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath);
BOOLEAN IsMediaFileDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath);
/**
    This function collects all the boot options, both efi
    and legacy boot options, that are present in the
    system and sets various boot manager variables that
    are used to expand boot manager questions.

    @param VOID

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS BootGetBootOptions( VOID )
{
    EFI_STATUS	Status;
    UINTN		AllocatedCount;

#ifndef STANDALONE_APPLICATION
    CHAR16		Pattern[]=L"boot[0-9a-f][0-9a-f][0-9a-f][0-9a-f]";
    CHAR16		KeyPattern[]=L"Key[0-9a-f][0-9a-f][0-9a-f][0-9a-f]";
    UINTN		OldVarNameSize;
    UINTN		VarNameSize;
    CHAR16		*VarName;
    EFI_GUID	VarGuid;
	VOID *		UnicodeInterface;
    UINTN		size=0, Length = 2, BootOrderSize = 0;
    UINT8		*bufPtr;
    BOOT_OPTION *buffer;
    BOOT_DATA   *dataPtr;
	UINT32      *conditionPtr;
	UINT16      *BootOrder = NULL;
    CHAR16      *String=NULL;
//    UINTN       KeyVarNameSize = 0;
#endif //STANDALONE_APPLICATION
	BOOLEAN IsFastBoot=FALSE;
	UINTN		DriverAllocatedCount;
    CHAR16		DriverPattern[] = L"Driver[0-9a-f][0-9a-f][0-9a-f][0-9a-f]";
	BOOLEAN 	BootOption = FALSE;
	BOOLEAN 	DriverOption = FALSE;
	UINT16 		jIndex 		= 	0;
	CHAR16		OSRecoveryPattern[] = L"OsRecovery[0-9a-f][0-9a-f][0-9a-f][0-9a-f]";
	CHAR16		PlatformPattern[] = L"PlatformRecovery[0-9a-f][0-9a-f][0-9a-f][0-9a-f]";
	UINTN 		OSRecoveryAllocCount;
	UINTN 		PlatformOptionAllocCount;
	UINT16 		*PlatformRecoveryOrder = NULL;
	UINT16 		Index = 0;
	
	DISABLED_BOOT_OPTION 	= 	0xFFFF;
	DISABLED_DRIVER_OPTION 	= 	0xFFFF;
	DriverAllocatedCount = gDriverOptionCount = 0;
   AllocatedCount = gBootOptionCount = 0;
   OSRecoveryAllocCount = gOSRecoveryOptionCount = 0;
   PlatformOptionAllocCount = gPlatformRecoveryOpCount = 0;
   
#ifndef STANDALONE_APPLICATION
	Status = InitUnicodeCollectionProtocol(&UnicodeInterface);
    if(EFI_ERROR(Status))
        return Status;

    RefreshBootKeysDetails ();
    //start with a size of 80 bytes
    VarNameSize = 80;
    OldVarNameSize = VarNameSize;
    VarName = EfiLibAllocateZeroPool(VarNameSize);
    conditionPtr = VarGetNvramName( L"BootFlow", &_gBootFlowGuid, NULL, &size );
    if ( (conditionPtr != NULL) && (BOOT_FLOW_CONDITION_FAST_BOOT == *conditionPtr) )
	{
		IsFastBoot = TRUE;
		BootOrder = HelperGetVariable(VARIABLE_ID_BOOT_ORDER, L"BootOrder", &gEfiGlobalVariableGuid, NULL, &BootOrderSize );
		VarNameSize = ((EfiStrLen(L"BOOTXXXX")+1)*2);
		
		if(VarName)
			MemFreePointer( (VOID **)&VarName );
		
		VarName = EfiLibAllocateZeroPool(VarNameSize);
		SPrint (VarName, VarNameSize, L"Boot%04X",BootOrder[0] );
	    MemFreePointer((VOID **) &BootOrder);
	    MemFreePointer((VOID **) &conditionPtr);
	}
   size = 0;
   BootOrderSize = 0;
	BootOrder = VarGetNvramName (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &BootOrderSize);
	if(NULL == BootOrder)
		return EFI_OUT_OF_RESOURCES;
		
    do
    {
		if (!IsFastBoot)
		{
			Status = gRT->GetNextVariableName (&VarNameSize,
											VarName,
											&VarGuid);
			if (Status == EFI_BUFFER_TOO_SMALL)
			{
				//Allocate correct size
				VarName = MemReallocateZeroPool (VarName,
												OldVarNameSize,
												VarNameSize);
				OldVarNameSize = VarNameSize;
				continue;
			}
		}
        //KeyVarNameSize = VarNameSize;
        if ( (VarNameSize == ((EfiStrLen (L"BOOTXXXX")+1)*2)) || (VarNameSize == ((EfiStrLen (L"DRIVERXXXX")+1)*2))
      	  || (VarNameSize == ((EfiStrLen (L"OsRecoveryXXXX")+1)*2)) || (VarNameSize == ((EfiStrLen (L"PlatformRecoveryXXXX")+1)*2)))	        //Find if this variable is one of L"BOOTXXXX" or L"DRIVERXXXX"
        {
			if (MetaiMatch (UnicodeInterface, VarName, Pattern))											//The buffer is definatly a boot#### option
			{
                size = 0;
                buffer = VarGetNvramName (VarName, &gEfiGlobalVariableGuid, NULL, &size);
                if (buffer)																					//Variable exists. Store details in global array.
                {
                	if (TRUE == TSESkipBootOrderVar(BootOrder, BootOrderSize,VarName))
                	{	
                		if(SkipOrphanBootOrderVar())
                		{
                			goto _NextVariable;
                		}
                		if(!TSEIsBootOptionValid(buffer, size))
                		{
                			SETUP_DEBUG_TSE( "\n[TSE] Excluding a corrupted or non-efi orphan boot variable %s\n", VarName );     
                			goto _NextVariable;
                		}				
                		
                	}
                    if (gBootOptionCount >= AllocatedCount)													//Check if enough size has been already allocated
                    {
                        gBootData = MemReallocateZeroPool (gBootData, AllocatedCount * sizeof(BOOT_DATA), (AllocatedCount + BOOT_OPTION_ALLOC_UNIT) * sizeof(BOOT_DATA));
                        if (NULL == gBootData)
						{
                            return EFI_UNSUPPORTED;
						}
                        AllocatedCount += BOOT_OPTION_ALLOC_UNIT;
                    }
					dataPtr = &(gBootData [gBootOptionCount]);
					HexStringToBuf(
						(UINT8 *)(&(dataPtr->Option)),
						&Length,
						&(VarName[4]),
						NULL);
					BootOption = TRUE;
					gBootOptionCount ++;
				}
			}
			else if (MetaiMatch (UnicodeInterface, VarName, DriverPattern))
			{
                size = 0;
                buffer = VarGetNvramName (VarName, &gEfiGlobalVariableGuid, NULL, &size);
                if (buffer)		//Variable exists. Store details in global array.
                {
                    if (gDriverOptionCount >= DriverAllocatedCount)		//Check if enough size has been already allocated
                    {
                        gDriverData = MemReallocateZeroPool (gDriverData, DriverAllocatedCount * sizeof (BOOT_DATA), (DriverAllocatedCount + BOOT_OPTION_ALLOC_UNIT) * sizeof(BOOT_DATA));
                        if (NULL == gDriverData)
						{
                            return EFI_UNSUPPORTED;
						}	
                        DriverAllocatedCount += BOOT_OPTION_ALLOC_UNIT;
                    }
					dataPtr = &(gDriverData [gDriverOptionCount]);
					HexStringToBuf(
						(UINT8 *)(&(dataPtr->Option)),
						&Length,
						&(VarName[6]),
						NULL);
					DriverOption = TRUE;
					gDriverOptionCount ++;
				}
			}
			else if (IsRecoverySupported() && (MetaiMatch (UnicodeInterface, VarName, OSRecoveryPattern)))											//The buffer is definatly a boot#### option
			{
				 size = 0;
				 buffer = VarGetNvramName (VarName, &gEfiGlobalVariableGuid, NULL, &size);
				 if (buffer)																					//Variable exists. Store details in global array.
				 {
					  if (gOSRecoveryOptionCount >= OSRecoveryAllocCount)													//Check if enough size has been already allocated
					  {
							gOSRecoveryData = MemReallocateZeroPool (gOSRecoveryData, OSRecoveryAllocCount * sizeof(BOOT_DATA), (OSRecoveryAllocCount + BOOT_OPTION_ALLOC_UNIT) * sizeof(BOOT_DATA));
							if (NULL == gOSRecoveryData)
							{
								 return EFI_UNSUPPORTED;
							}
							OSRecoveryAllocCount += BOOT_OPTION_ALLOC_UNIT;
					  }
					  dataPtr = &(gOSRecoveryData [gOSRecoveryOptionCount]);
					  HexStringToBuf(
							  (UINT8 *)(&(dataPtr->Option)),
							  &Length,
							  &(VarName[10]),
							  NULL);
					  DriverOption = TRUE;
					  gOSRecoveryOptionCount ++;
				 }
			}
			else if (IsRecoverySupported() && (MetaiMatch (UnicodeInterface, VarName, PlatformPattern)))											//The buffer is definatly a boot#### option
			{
				 size = 0;
				 buffer = VarGetNvramName (VarName, &gEfiGlobalVariableGuid, NULL, &size);
				 if (buffer)																					//Variable exists. Store details in global array.
				 {
					  if (gPlatformRecoveryOpCount >= PlatformOptionAllocCount)													//Check if enough size has been already allocated
					  {
							gPlatformRecoveryData = MemReallocateZeroPool (gPlatformRecoveryData, PlatformOptionAllocCount * sizeof(BOOT_DATA), (PlatformOptionAllocCount + BOOT_OPTION_ALLOC_UNIT) * sizeof(BOOT_DATA));
							if (NULL == gPlatformRecoveryData)
							{
								 return EFI_UNSUPPORTED;
							}
							PlatformOptionAllocCount += BOOT_OPTION_ALLOC_UNIT;
					  }
					  dataPtr = &(gPlatformRecoveryData [gPlatformRecoveryOpCount]);
					  HexStringToBuf(
						(UINT8 *)(&(dataPtr->Option)),
						&Length,
						&(VarName[16]),
						NULL);
					  DriverOption = TRUE;
					  gPlatformRecoveryOpCount++;
				 }
			}
			else
			{
				goto _NextVariable;					//Jumping for next variable iteration
			}
			if (DriverOption || BootOption)			//Common code for both driver and boot options
			{
				dataPtr->Active = buffer->Active;
				dataPtr->Name = StrDup (buffer->Name);
				bufPtr = (UINT8 *)((CHAR16 *)buffer->Name + EfiStrLen( buffer->Name ) + 1);
				dataPtr->DevicePath = EfiLibAllocateZeroPool( buffer->PathLength );
				if (NULL != dataPtr->DevicePath)
				{
					MemCopy (dataPtr->DevicePath, bufPtr, buffer->PathLength);
				}
				bufPtr += buffer->PathLength;
				size -= (UINTN)bufPtr - (UINTN)buffer;
				if (size != 0)
				{
					dataPtr->LoadOptions = EfiLibAllocatePool( size );
					if ( dataPtr->LoadOptions != NULL )
					{
						dataPtr->LoadOptionSize = size;
						MemCopy( dataPtr->LoadOptions, bufPtr, size );
					}
				}
				if (BootOption)
				{
	                //In case of DEVIE PATH support, to reset the name using device path
	                String = TseGetUefiDevPathString(dataPtr->DevicePath);
	                if (NULL != String)
					{
	                    // For UEFI boot options, Prefix the string with "UEFI: "
	                    MemFreePointer( (VOID **)&(dataPtr->Name) );
	                    dataPtr->Name = EfiLibAllocateZeroPool( EfiStrLen( String ) + EfiStrLen(L"UEFI: ")+ 1 );
	                    EfiStrCpy(dataPtr->Name, L"UEFI: ");
	                    EfiStrCat(dataPtr->Name, String);
	                }
	            }
				MemFreePointer( (VOID **)&buffer );
			}
_NextVariable:
			BootOption = DriverOption = FALSE;		//Getting ready for next iteration
        }
        else if (VarNameSize == ((EfiStrLen(L"keyXXXX")+1)*2))		//Boot, Driver and Key are different length so handling in else if cases
        {
            if (MetaiMatch (UnicodeInterface, VarName, KeyPattern))
            {
                FormHotBootKeys (VarName);
            }
        }
		// Only one Boot options is valid in FastBoot case
		if(IsFastBoot)
			break;
        //reset the size of this buffer to what has been allocated for it
        VarNameSize = OldVarNameSize;

    }while(Status != EFI_NOT_FOUND);
	MemFreePointer( (VOID **)&BootOrder );

#endif //STANDALONE_APPLICATION

	//For Boot#### variable
	for ( jIndex = 0; jIndex < gBootOptionCount; jIndex++ )
	{
		if ( gBootData[jIndex].Option == DISABLED_BOOT_OPTION )
		{
			DISABLED_BOOT_OPTION--;//Changing the value based on the available of BOOT#### variable
			jIndex	=	-1;
			continue;
		}
	}
	//For Driver#### variable
	for ( jIndex = 0; jIndex < gDriverOptionCount; jIndex++ )
	{
		if ( gDriverData[jIndex].Option == DISABLED_DRIVER_OPTION )	
		{
			DISABLED_DRIVER_OPTION--;//Changing the value based on the available of DRIVER#### variable
			jIndex	=	-1;
			continue;
		}
	}
    //Free unused memory
    if (gBootOptionCount < AllocatedCount)
        gBootData = MemReallocateZeroPool(gBootData, AllocatedCount * sizeof (BOOT_DATA), gBootOptionCount * sizeof (BOOT_DATA));

    if (gDriverOptionCount < DriverAllocatedCount)
        gDriverData = MemReallocateZeroPool(gDriverData, DriverAllocatedCount * sizeof (BOOT_DATA), gDriverOptionCount * sizeof (BOOT_DATA));

    if (gOSRecoveryOptionCount < OSRecoveryAllocCount)
		  gOSRecoveryData = MemReallocateZeroPool (gOSRecoveryData, OSRecoveryAllocCount * sizeof(BOOT_DATA), gOSRecoveryOptionCount * sizeof(BOOT_DATA));
    
    if (gPlatformRecoveryOpCount < PlatformOptionAllocCount)
		  gPlatformRecoveryData = MemReallocateZeroPool (gPlatformRecoveryData, PlatformOptionAllocCount * sizeof(BOOT_DATA), gPlatformRecoveryOpCount * sizeof(BOOT_DATA));
  
    PlatformRecoveryOrder = EfiLibAllocateZeroPool(gPlatformRecoveryOpCount * sizeof(UINT16) );
    
    if(PlatformRecoveryOrder)
    {
		for (Index = 0; Index < gPlatformRecoveryOpCount; Index++) 
		{
			PlatformRecoveryOrder[Index] = gPlatformRecoveryData[Index].Option;
		}
		
		Status = VarSetNvramName(L"PlatformRecoveryOrder",
					&gPlatformRecoveryOrderGuid,
					EFI_VARIABLE_BOOTSERVICE_ACCESS,
					PlatformRecoveryOrder,
					gPlatformRecoveryOpCount * sizeof(UINT16));
		
		MemFreePointer( (VOID **)&PlatformRecoveryOrder );
    }

    FreeExtraKeyMemories ();
    //Set boot manager variables
	if(!IsFastBoot)
	{
	    _BootSetBootManagerVariables ();
		_SetDriverManagerVariables ();
	}
    Status = EFI_SUCCESS;
    return Status;
}

/**
    This function launches the boot option supplied

    @param u16Option: BootOption to be launched
              pOrder: The order in which to set BBS priorities
              u16OrderCount: No of options in pOrder

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS BootLaunchBootOption(UINT16 u16Option, UINT16 *pOrder, UINTN u16OrderCount)
{
    EFI_STATUS	status = EFI_NOT_FOUND;
    UINT16		u16BootCurrent;

    BOOT_DATA	*pBootData = NULL;
    SETUP_DEBUG_TSE("\n[TSE] BootLaunchBootOption Entering :\n");
    if(gRecoveryBootingType == BOOT_OPTION_BOOTING)
    {
    	pBootData = BootGetBootData(u16Option); // Normal bootoption
    }
    else
    {
    	pBootData = RecoveryGetRecoveryData(u16Option, gRecoveryBootingType); // Recovery Bootoption
    }
    
    if (pBootData)
    {
        //Set BootCurrent
        u16BootCurrent = pBootData->Option;
        VarSetNvramName(L"BootCurrent",
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                &(u16BootCurrent),
                sizeof(u16BootCurrent));
        if( gBootFlow != BOOT_FLOW_CONDITION_FAST_BOOT) 
		{
        	CsmBBSSetBootPriorities(pBootData, pOrder, u16OrderCount);
        }
status = _BootLaunchDevicePath(pBootData->DevicePath, pBootData->LoadOptions, pBootData->LoadOptionSize,TRUE);

        //clear BootCurrent
        VarSetNvramName(L"BootCurrent",
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                &(u16BootCurrent),
                0);
    }
    SETUP_DEBUG_TSE("\n[TSE] BootLaunchBootOption Exiting with status = %r\n",status);
    return status;
}

EFI_STATUS BBSSetBootNowPriorityForBootNext(UINT16 u16Option, UINT16 *pOrder, UINTN u16OrderCount)
{
    EFI_STATUS	status = EFI_NOT_FOUND;
    UINT16		u16BootCurrent;
    BOOT_DATA	*pBootData = NULL;
    
    
    pBootData = BootGetBootData(u16Option);
    if (pBootData)
    {  
    	//Set BootCurrent
        u16BootCurrent = pBootData->Option;
        VarSetNvramName(L"BootCurrent",
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                &(u16BootCurrent),
                sizeof(u16BootCurrent));
        if( gBootFlow != BOOT_FLOW_CONDITION_FAST_BOOT)
        {
      	  	  
      	  	   		gBootNextFlag = TRUE;  	   
					CsmBBSSetBootNowPriority(pBootData,0,gShowAllBbsDev);
					gBootNextFlag = FALSE;
        }
       
        status = _BootLaunchDevicePath(pBootData->DevicePath, pBootData->LoadOptions, pBootData->LoadOptionSize,TRUE);

        //clear BootCurrent
        VarSetNvramName(L"BootCurrent",
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                &(u16BootCurrent),
                0);
    }

    return status;
}



/**
    Finds and returns internal data structure BOOT_DATA
    for a given boot option number.

    @param Option: Option number for which BOOT_DATA is needed

    @retval Returns pointer to BOOT_DATA if found. Returns NULL
        if BOOT_DATA not found.

**/
BOOT_DATA *BootGetBootData( UINT16 Option )
{
    UINTN i;

    for ( i = 0; i < gBootOptionCount; i++ )
    {
        if ( gBootData[i].Option == Option )
            return &gBootData[i];
    }

    return NULL;
}

/**
    Finds and returns internal data structure BOOT_DATA
    for a given Driver option number.

    @param Option: Option number for which BOOT_DATA is needed

    @retval Returns pointer to BOOT_DATA if found. Returns NULL
        if BOOT_DATA not found.

**/
BOOT_DATA *DriverGetDriverData (UINT16 Option)
{
    UINTN i;

    for ( i = 0; i < gDriverOptionCount; i++ )
    {
        if ( gDriverData[i].Option == Option )
            return &gDriverData[i];
    }
    return NULL;
}

/**
    Provides the unicode name for the given boot option
    in the form of internal data structure BOOT_DATA.

    @param bootData: BOOT_DATA struct for which unicode name is
        needed.

    @retval Returns unicode string corresponding to the provided
        BOOT_DATA. If the BOOT_DATA provided is for legacy
        option then the name for the first legacy option in
        that category is returned.

**/
CHAR16 *BootGetOptionName( BOOT_DATA *bootData)
{
    if ( BBSValidDevicePath(bootData->DevicePath) ) {
		return CsmBBSBootOptionName(bootData);
    }
    return bootData->Name;
}

/**
    Provides the unicode name for the given driver option
    in the form of internal data structure BOOT_DATA.

    @param DriverData: BOOT_DATA struct for which unicode name is
        needed.

    @retval Returns unicode string corresponding to the provided BOOT_DATA

**/
CHAR16 *DriverGetOptionName (BOOT_DATA *DriverData)
{
    return DriverData->Name;
}

/**
    Provides the unicode name for the given boot option.

    @param value: boot option for which unicode name is needed

    @retval Returns unicode string corresponding to the provided
        boot option.

**/
CHAR16 *BootGetBootNowName( UINT16 value, BOOLEAN ShowAllBBSDev, BOOLEAN TseBootNowInBootOrde)
{
    UINT16	*BootOrder=NULL;
    UINTN	size = 0;
    UINTN	i,j,k;
    UINTN	count;

    BOOT_DATA *bootData;

	if((!ShowAllBBSDev) && (!TseBootNowInBootOrde))
		if(gBootData == NULL)
			return NULL;
		else
		{
			if(gLoadOptionHidden && (gBootData[value].Active & LOAD_OPTION_HIDDEN))
				return NULL;

			return BootGetOptionName(&(gBootData[value]));
		}
	if(TseBootNowInBootOrde)
	{
		BootOrder = HelperGetVariable(VARIABLE_ID_BOOT_ORDER, L"BootOrder", &gEfiGlobalVariableGuid, NULL, &size );
		if(NULL == BootOrder){
			ASSERT (0);
			return NULL;
		}

	    //Find the first disabled option
	    for ( i = 0; i < gBootOptionCount; i++ )
	    {
	        if ( DISABLED_BOOT_OPTION == BootOrder[i] )
	            break;
	    }

	    if(i<gBootOptionCount)
	    {
	        //There are disabled options replace them with valid options
	        for(j=0;j<gBootOptionCount;j++)
	        {
	            for(k=0;k<gBootOptionCount;k++)
	            {
	                if(BootOrder[k] == gBootData[j].Option)
	                    break;
	            }

	            if(k >= gBootOptionCount)
	            {
	                //gBootData[j].Option is not present in BootOrder; fill it
	                BootOrder[i] = gBootData[j].Option;
	                i++;
	            }
	        }
	    }
	}

	if(ShowAllBBSDev)
	{
	    count = 0;
	    for(i=0;i<gBootOptionCount;i++)
	    {
			if(TseBootNowInBootOrde) {
		        bootData = BootGetBootData(BootOrder[i]);
				if(NULL == bootData){
					TRACE((-1,"\n[TSE] Boot data is NULL\n"));
					ASSERT(0);
				}
			}
			else {
	     		bootData = gBootData + i;
				if(NULL == bootData) 
				{
					TRACE((-1,"\n[TSE] Boot data is NULL\n"));
					ASSERT(0);
				}
			}

	        if ( BBSValidDevicePath(bootData->DevicePath) )
	        {
	            if((value >= count) && (value < (count+bootData->LegacyDevCount)))
	            {
					if(TseBootNowInBootOrde)
		                MemFreePointer((VOID **) &BootOrder);
				if (gLoadOptionHidden && ((bootData->Active & LOAD_OPTION_HIDDEN)!= 0))
 					return NULL;
				
	                return bootData->OrderList[value-count].Name;
	            }

	            count+=bootData->LegacyDevCount;
	        }
	        else {
	            if(value == count)
	            {
					if(TseBootNowInBootOrde)
		                MemFreePointer((VOID **) &BootOrder);
					if (gLoadOptionHidden && ((bootData->Active & LOAD_OPTION_HIDDEN)!= 0))
 					 return NULL;
	                return bootData->Name;
	            }
	            count++;
	        }
	    }
 		return NULL;
	}
	else {
	    bootData = BootGetBootData(BootOrder[value]);
		if(NULL == bootData)	
		{
			TRACE((-1,"\n[TSE] Boot data is NULL\n"));
			ASSERT(0);
		}
	    MemFreePointer((VOID **) &BootOrder);
		if (gLoadOptionHidden && ((bootData->Active & LOAD_OPTION_HIDDEN)!= 0))
				return NULL;

	    return BootGetOptionName(bootData);
	}

}

/**
    Finds the languages that the firmware supports.

    @param VOID

    @retval VOID

**/
VOID GetBootLanguages( VOID )
{
    CHAR16	*langString;
    CHAR8	*langCodes, *langPtr;
    UINTN	i, count, size = 0;

    langCodes = VarGetNvramName( L"LangCodes", &gEfiGlobalVariableGuid, NULL, &size );
    count = size /3;
    
    if(gLanguages)
    {
    	for(i=0;i<gLangCount;i++)
    	{
    		if(gLanguages[i].LangCode) 
    			MemFreePointer( (VOID **)&gLanguages[i].LangCode );
    		if(gLanguages[i].LangString)
    			MemFreePointer( (VOID **)&gLanguages[i].LangString  );
    		if(gLanguages[i].Unicode )
    			MemFreePointer( (VOID **)&gLanguages[i].Unicode );
    	}
    	MemFreePointer( (VOID **)&gLanguages );
    }

    if ( size == 0 )
    {
	    langCodes = VarGetNvramName( L"Lang", &gEfiGlobalVariableGuid, NULL, &size );
		if ( size != 0) {
		    count = 1;
		}
		else {
	        gLanguages = EfiLibAllocateZeroPool( sizeof(LANGUAGE_DATA) );
	        if ( gLanguages == NULL )
	            return;

			gLanguages[0].LangCode = StrDup8("eng");
			gLanguages[0].Unicode = StrDup(L"eng");
	        gLangCount = 1;
	        langString = HiiGetStringLanguage( (VOID*)(UINTN)INVALID_HANDLE, 1, gLanguages[0].Unicode );
	        gLanguages[0].Token = HiiAddString( gHiiHandle, langString );
	        gLanguages[0].LangString = langString;
	        return;
		}
    }

    gLanguages = EfiLibAllocateZeroPool( count * sizeof(LANGUAGE_DATA) );
    if ( gLanguages == NULL )
        return;

    for ( langPtr = langCodes, i = 0; i < count; i++, langPtr += 3 )
    {
		gLanguages[i].LangCode = EfiLibAllocateZeroPool( 4 * sizeof(CHAR8));
        MemCopy( gLanguages[i].LangCode, langPtr, 3 * sizeof(CHAR8) );
		gLanguages[i].Unicode = StrDup8to16(gLanguages[i].LangCode);
    }

    gLangCount = count;

    for ( langPtr = langCodes, i = 0; i < count; i++, langPtr += 3 )
    {
        langString = HiiGetStringLanguage( (VOID*)(UINTN)INVALID_HANDLE, 1, gLanguages[i].Unicode );
        gLanguages[i].Token = HiiAddString( gHiiHandle, langString );
        gLanguages[i].LangString = langString;
    }

    MemFreePointer( (VOID **)&langCodes );
}

/**
    Finds the languages that the firmware supports.

    @param VOID

    @retval VOID

**/
VOID GetPlatformBootLanguages( VOID )
{
    CHAR16	*langString;
    CHAR8	*langCodes, *langPtr;
    UINTN	i = 0, count=0, size = 0, pos = 0;

    langCodes = VarGetNvramName( L"PlatformLangCodes", &gEfiGlobalVariableGuid, NULL, &size );
    if(langCodes != NULL)
		count = GetTokenCount(langCodes);
    
    if(gLanguages)
	{
		 for(i=0;i<gLangCount;i++)
		 {
			if(gLanguages[i].LangCode) 
				MemFreePointer( (VOID **)&gLanguages[i].LangCode );
			if(gLanguages[i].LangString)
				MemFreePointer( (VOID **)&gLanguages[i].LangString  );
			if(gLanguages[i].Unicode )
				MemFreePointer( (VOID **)&gLanguages[i].Unicode );
		 }
		 MemFreePointer( (VOID **)&gLanguages );
	}
    
    if ((0 == size) || (NULL == langCodes))
    {
	    langCodes = VarGetNvramName( L"PlatformLang", &gEfiGlobalVariableGuid, NULL, &size );
	    if ( size  != 0 ) {
			count=1;
		}
		else {
		        gLanguages = (LANGUAGE_DATA *)EfiLibAllocateZeroPool( sizeof(LANGUAGE_DATA) );
	        if ( gLanguages == NULL )
	            return;

	        gLanguages[0].LangCode = StrDup8("en-US");
	        gLanguages[0].Unicode = StrDup(L"en-US");

	        gLangCount = 1;
	        langString = HiiGetStringLanguage( (VOID*)(UINTN)INVALID_HANDLE, 1, gLanguages[0].Unicode );
	        gLanguages[0].Token = HiiAddString( gHiiHandle, langString );
	        gLanguages[0].LangString = langString;
	        return;
	    }
	}

    gLanguages = EfiLibAllocateZeroPool( count * sizeof(LANGUAGE_DATA) );
    if ( gLanguages == NULL )
        return;

    for ( langPtr = langCodes, i = 0; i < count; i++ )
    {
        gLanguages[i].LangCode = GetTokenString(langPtr, &pos);
        gLanguages[i].Unicode  = StrDup8to16(gLanguages[i].LangCode);
        langString = HiiGetStringLanguage( INVALID_HANDLE, 1, gLanguages[i].Unicode );
        gLanguages[i].Token = HiiAddString( gHiiHandle, langString );
        gLanguages[i].LangString = langString;

        langPtr += pos + 1;
    }

    gLangCount = count;
    MemFreePointer( (VOID **)&langCodes );
}

/**
    Launches the boot option provided.

        index: The nth option in the boot now menu.
    @param ShowAllBbsDev SETUP_SHOW_ALL_BBS_DEVICES

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS BootLaunchBootNow( UINT16 index, BOOLEAN ShowAllBbsDev )
{

    EFI_STATUS	Status = EFI_OUT_OF_RESOURCES;
    BOOLEAN		bLegacyBoot = FALSE;
    UINT16		count = 0, BootCurrent;
    UINT16		*BootOrder;
    UINTN		i = 0;

    BOOT_DATA *bootData=NULL;

	ProcessProceedToBootNowHook();

    BootOrder = BootNowinBootOrderInit();

	if(ShowAllBbsDev)
	{
	    for(i=0;i<gBootOptionCount;i++)
	    {
			bootData = BootGetBootNowBootData(gBootData,BootOrder,i);
			if(NULL == bootData ) { 
				continue;
			}
	        if ( BBSValidDevicePath(bootData->DevicePath) )
	        {
	            if((index >= count) && (index < (count+bootData->LegacyDevCount)))
	            {
	                bLegacyBoot = TRUE;
	                break;
	            }
	            else
	                count=count+(UINT16)(bootData->LegacyDevCount);
	        }
	        else
	        {
	            if(index == count)
	                break;
	            count++;
	        }
	    }
	}
	else
	{
		if(0 == gBootOptionCount)
	        return EFI_NOT_FOUND;
		bootData = BootGetBootNowBootData(gBootData,BootOrder,index);
	}

	if(BootOrder!=NULL)
		MemFreePointer((VOID **) &BootOrder);

	if(NULL == bootData){
		return EFI_NOT_FOUND;
	}

	if(ShowAllBbsDev)
	{
	    if(i == gBootOptionCount)
	        return Status;

	    //Boot bootData and device index-count
	    //set bbs priorities
	    if(bLegacyBoot)
		{
			Status = CsmBBSSetBootNowPriority(bootData,index-count,ShowAllBbsDev);
			if(EFI_ERROR( Status )) 
				return EFI_UNSUPPORTED;
		}
	}

    //Set BootCurrent
    BootCurrent = bootData->Option;
    VarSetNvramName(L"BootCurrent",
            &gEfiGlobalVariableGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
            &(BootCurrent),
            sizeof(BootCurrent));
	if(!ShowAllBbsDev)
	{
	    if(BBSValidDevicePath(bootData->DevicePath))
		{
	        Status = CsmBBSSetBootNowPriority(bootData,0,ShowAllBbsDev);
			if(EFI_ERROR( Status )) 
				return EFI_UNSUPPORTED;
		}	
	}
    Status = _BootLaunchDevicePath( bootData->DevicePath, bootData->LoadOptions, bootData->LoadOptionSize,TRUE );
    //clear BootCurrent
    VarSetNvramName(L"BootCurrent",
            &gEfiGlobalVariableGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
            &(BootCurrent),
            0);

    return Status;
}

/**
    Launches the guided file from FV.

    @param guid: Guid of the file to be launched.

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS BootLaunchGuid( EFI_GUID *guid )
{
    EFI_STATUS	Status = EFI_UNSUPPORTED;
    UINT32		index = 0;

    EFI_DEVICE_PATH_PROTOCOL *devicePath;

    do
    {
        devicePath = _BootBuildFVDevicePath( &index, guid );
        if ( index != (UINT32)-1 )
            Status = _BootLaunchDevicePath( devicePath, NULL, 0,FALSE );

        MemFreePointer( (VOID **)&devicePath );
    }
    while ( ( EFI_ERROR( Status ) ) && ( index != (UINT32)-1 ) );

    return Status;
}

/**
    Launches the file mentioned in file path from the
    available FS.

    @param fileName: Path of the file to be launched.

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS BootLaunchFilename( CHAR16 *fileName )
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    UINT32     index = 0;

    EFI_DEVICE_PATH_PROTOCOL *devicePath;

    do
    {
        devicePath = _BootBuildFileDevicePath( &index, fileName );
        if ( index != (UINT32)-1 )
            Status = _BootLaunchDevicePath( devicePath, NULL, 0,FALSE );

        MemFreePointer( (VOID **)&devicePath );
    }
    while ( ( EFI_ERROR( Status ) ) && ( index != (UINT32)-1 ) );

    return Status;
}

/**
    Function to move the hidden option at last of the boot order

    @param UINT16 ** => Boot order to be reformed

    @retval VOID

**/
VOID FixHiddenOptions (BOOLEAN Option, UINT16 **Order, UINTN OptionCount)
{
	UINTN iIndex = 0;
	UINTN jIndex = 0;
	UINT16 TempBootOption = 0;
	BOOT_DATA   *pBootData;

	if (0 == OptionCount)
	{
		return;
	}
	for (iIndex = 0; iIndex < OptionCount-1; iIndex ++)
	{
		if (BOOT_ORDER_OPTION == Option)
		{
			pBootData = BootGetBootData ((*Order) [iIndex]);
		}
		else
		{
			pBootData = DriverGetDriverData ((*Order) [iIndex]);
		}
		if (pBootData)
		{
			if (pBootData->Active & LOAD_OPTION_HIDDEN)
			{
				for (jIndex = iIndex+1; jIndex < OptionCount; jIndex ++)
				{
					if (BOOT_ORDER_OPTION == Option)
					{
						pBootData = BootGetBootData ((*Order) [jIndex]);
					}
					else
					{
						pBootData = DriverGetDriverData ((*Order) [jIndex]);
					}
					if (!(pBootData->Active & LOAD_OPTION_HIDDEN))
					{
						TempBootOption = (*Order) [iIndex];
						(*Order) [iIndex] = (*Order) [jIndex];
						(*Order) [jIndex] = TempBootOption;
						break;
					}
				}
			}
		}
	}
}

/**
    function to set the variables for the boot manager

    @param void

    @retval void

**/
VOID _BootSetBootManagerVariables(VOID)
{
    UINT16	*NewBootOrder, *BootOrder, count;
    UINTN	size=0, i=0, j=0, k=0, BootOrderCount;

    EFI_GUID 	BootManGuid = BOOT_MANAGER_GUID;
    BOOT_DATA   *pBootData;

    //Correct BootOrder variable if necessary
    NewBootOrder = NULL;
    if(gBootOptionCount)
        NewBootOrder = (UINT16 *)EfiLibAllocateZeroPool(gBootOptionCount * sizeof(UINT16));

    BootOrder = (UINT16 *)VarGetNvramName( L"BootOrder", &gEfiGlobalVariableGuid, NULL, &size );
    BootOrderCount = size/sizeof(UINT16);

    //Get all the enabled boot options in the boot order
    for(i = 0; i < BootOrderCount; i++)
    {
        pBootData = BootGetBootData(BootOrder[i]);
        if(pBootData)
        {
	  	    if(IsPreservedDisabledBootOptionOrder())
		    {
				/*if(gLoadOptionHidden && (pBootData->Active & LOAD_OPTION_HIDDEN))
				{
					j++;
					continue;
				}*/
				NewBootOrder[j] = BootOrder[i];
	   			j++;
		    }
		    else if(pBootData->Active & LOAD_OPTION_ACTIVE)
	        {
				/*if(gLoadOptionHidden && (pBootData->Active & LOAD_OPTION_HIDDEN))
				{
					j++;
					continue;
				}*/
	            NewBootOrder[j] = BootOrder[i];
	            j++;
	        }
        }
    }

    //Append all options that are enabled but not included in
    //BootOrder. FCFS used.
    for(i=0; i < gBootOptionCount; i++)
    {
        if(gBootData[i].Active & LOAD_OPTION_ACTIVE)
        {
            //Check presence in boot order
            for(k=0;k<BootOrderCount;k++)
            {
                if(BootOrder[k] == gBootData[i].Option)
                    break;
            }
            if(k >= BootOrderCount)
            {
                //Not present in boot order! Add option
                NewBootOrder[j] = gBootData[i].Option;
                j++;
            }
        }
    }

    //Free Boot order
    MemFreePointer((VOID **)&BootOrder);

    if(!IsPreservedDisabledBootOptionOrder())
    {
        //Put disabled options at the end of NewBootOrder
        for(i=0; i < gBootOptionCount; i++)
        {
            if(!(gBootData[i].Active & LOAD_OPTION_ACTIVE) && (!(gBootData[i].Active & LOAD_OPTION_CATEGORY_APP)))
            {
/*				if(gLoadOptionHidden && (gBootData[i].Active & LOAD_OPTION_HIDDEN))
				{
					j++;
					continue;
				}*/
    			NewBootOrder[j] = gBootData[i].Option;
    			j++;
	        }
        }
        ASSERT(j==gBootOptionCount);
    }

    //Set BootOrder Variable with corrected order
	if (gLoadOptionHidden)
	{
		FixHiddenOptions (BOOT_ORDER_OPTION, &NewBootOrder, gBootOptionCount);
	}
    VarSetNvramName(L"BootOrder",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
        NewBootOrder,
        gBootOptionCount * sizeof(UINT16));

    //Free NewBootOrder
    MemFreePointer((VOID **)&NewBootOrder);

    //Get BBS devices
    CsmBBSGetDeviceList();

    // Update the BootOrder,BBSOrder Cache From the NVRAM.
    UpdateBootVariables();

    //Set Boot manager variable
    count = (UINT16)gBootOptionCount;
    VarSetNvramName( L"BootManager", &BootManGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &count, sizeof(count) );

    //Set Boot now count
	if(gShowAllBbsDev)
  		count = _BootSetBootNowCount();

    VarSetNvramName( L"BootNowCount", &gBootNowCountGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &count, sizeof(count) );
	UpdateAddDeleteBootVar ();
}

/**
    Sets DriverManager variable for drivers count and sets DriverOrder

    @param void

    @retval void

**/
VOID _SetDriverManagerVariables (VOID)
{
	UINT16	*DriverOrder = NULL;
	UINT16	*NewDriverOrder = NULL;
	UINT16	DriverOrderCount = 0;
    UINTN	DriverOrderSize = 0;
	UINTN 	iIndex = 0;
	UINTN 	jIndex = 0;
	UINT16 	Count = 0;
    BOOT_DATA   *pDrvData;			//BOOT_DATA using same structure for driver options too
    EFI_GUID 	DrvMgrGuid = DRIVER_MANAGER_GUID;

	if (gDriverOptionCount)
	{	
	    NewDriverOrder = (UINT16 *)EfiLibAllocateZeroPool (gDriverOptionCount * sizeof(UINT16));
		if (NULL == NewDriverOrder)
		{
			return;
		}
	}
	DriverOrder = (UINT16 *)VarGetNvramName (L"DriverOrder", &gEfiGlobalVariableGuid, NULL, &DriverOrderSize);
	DriverOrderCount = (UINT16)(DriverOrderSize/sizeof(UINT16));
	if ((NULL == DriverOrder) || (0 == DriverOrderSize))
	{
		for (iIndex = 0; iIndex < gDriverOptionCount; iIndex++)			//Forming DriverOrder variable freshly
        {
			if (gDriverData [iIndex].Active & LOAD_OPTION_ACTIVE)
			{
				NewDriverOrder [Count] = gDriverData [iIndex].Option;
				Count ++;
			}
		}
		goto _FormDisableDevices;
	}
	else											//Forming DriverOrder variable with first active and non hidden drivers
	{
	    for (iIndex = 0; iIndex < DriverOrderCount; iIndex ++)
	    {
	        pDrvData = DriverGetDriverData (DriverOrder [iIndex]);
	        if (pDrvData)
	        {													
		    	if ( (pDrvData->Active & LOAD_OPTION_ACTIVE) )				
				{													//Checking for active drivers
	                NewDriverOrder [Count] = DriverOrder [iIndex];
	                Count ++;
	            }
	        }
	    }
	}
	//Append all options that are enabled but not included in
    //DriverOrder. Having same logic as of boot order.
    for(iIndex = 0; iIndex < gDriverOptionCount; iIndex ++)
    {
		if ( (gDriverData [iIndex].Active & LOAD_OPTION_ACTIVE) )
        {
            //Check presence in DriverOrder
            for (jIndex = 0; jIndex < DriverOrderCount; jIndex ++)
            {
                if (DriverOrder [jIndex] == gDriverData [iIndex].Option)
                    break;
            }
            if (jIndex >= DriverOrderCount)
            {
                //Not present in Driver order! Add option
                NewDriverOrder [Count] = gBootData[iIndex].Option;
                Count ++;
            }
        }
    }
_FormDisableDevices:			//Forming disabled drivers last in the newly formed DriverOrder variable.
	for (iIndex = 0; iIndex < gDriverOptionCount; iIndex ++)
	{
		if (!(gDriverData [iIndex].Active & LOAD_OPTION_ACTIVE))
		{	 
			NewDriverOrder [Count] = gDriverData [iIndex].Option;
			Count ++;
		}
	}
	if (gLoadOptionHidden)
	{
		FixHiddenOptions (DRIVER_ORDER_OPTION, &NewDriverOrder, gDriverOptionCount);
	}
	VarSetNvramName (
		L"DriverOrder",
		&gEfiGlobalVariableGuid,
		EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
		NewDriverOrder,
		gDriverOptionCount * sizeof(UINT16)
		);
    //Set Driver manager variable
    Count = (UINT16)gDriverOptionCount;
    VarSetNvramName (L"DriverManager", &DrvMgrGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &Count, sizeof(Count));
    UpdateDriverVariables ();
	if (DriverOrder)
	{
		MemFreePointer ((VOID **)&DriverOrder);
	}
	if (DriverOrder)
	{
		MemFreePointer ((VOID **)&NewDriverOrder);
	}
	UpdateAddDeleteDriverVar ();
}

/**
    function to set boot new devices count

    @param void

    @retval void

**/
UINT16 _BootSetBootNowCount(VOID)
{
    UINT16 count = 0;
    UINTN i;

    BOOT_DATA *bootData;

    for( i=0; i<gBootOptionCount; i++)
    {
        bootData = &(gBootData[i]);

		if(BBSValidDevicePath(bootData->DevicePath))
        {
            count = count + bootData->LegacyDevCount;
        }
        else
            count++;
    }

    return count;
}

/**
    Clears the screen for shell boot

    @param EFI_EVENT , VOID *

    @retval VOID

**/
void ClearGrphxScreen (void);
VOID _ShellClearScreen (EFI_EVENT Event, VOID *Context)
{
	UINT32 shelltextmodetype;
	InvalidateStatusInBgrtWrapper();
	ClearGrphxScreen ();
	gST->ConOut->ClearScreen (gST->ConOut);
	gBS->CloseEvent (Event);
	gST->ConOut->EnableCursor (gST->ConOut, TRUE);

	if (Event != gShellLaunchEvent)				//One event will be closed in incoming fnc argument other we have to close it
	{
		gBS->CloseEvent (gShellLaunchEvent);
	}
	
	if (Event != gShell20LaunchEvent)			
	{
		gBS->CloseEvent (gShell20LaunchEvent);
	}
	
	if (IsTseWatchDogTimer() && gWatchDogTimerSet) 
	{
		gBS->SetWatchdogTimer(0, 0, 0, NULL); 		//Disables the watchdog timer
		gWatchDogTimerSet = FALSE;
	}

	gShellLaunchEvent = NULL;					//Make NULL otherwise we will try to close it after startimage
	gShell20LaunchEvent = NULL;
	
	shelltextmodetype = ShellTextMode();//OEM needs Token to assign the display mode of SHELL.
	
	if ( 0xFF != shelltextmodetype )
		gST->ConOut->SetMode( gST->ConOut, shelltextmodetype );	
}

/**
    Registers the shell guid

    @param VOID

    @retval VOID

**/
VOID _RegisterShellGuid (VOID)
{
	EFI_STATUS 	Status = EFI_UNSUPPORTED;
	VOID 		*Registration = NULL;
	EFI_GUID 	EfiShellInterfaceGuid = EFI_SHELL_PROTOCOL_GUID;
	EFI_GUID 	EfiShell20InterfaceGuid = EFI_SHELL2_0_FILE_GUID;

	Status = gBS->CreateEvent (
				EFI_EVENT_NOTIFY_SIGNAL, 
				EFI_TPL_CALLBACK,
				_ShellClearScreen,
				NULL,
				&gShellLaunchEvent);
	if (!EFI_ERROR (Status))
	{
		Status = gBS->RegisterProtocolNotify(
				&EfiShellInterfaceGuid,
				gShellLaunchEvent,
				&Registration
				);
	}
	Status = gBS->CreateEvent (
				EFI_EVENT_NOTIFY_SIGNAL, 
				EFI_TPL_CALLBACK,
				_ShellClearScreen,
				NULL,
				&gShell20LaunchEvent);
	if (!EFI_ERROR (Status))
	{
		Status = gBS->RegisterProtocolNotify(
				&EfiShell20InterfaceGuid,
				gShell20LaunchEvent,
				&Registration
				);
	}
}


/**
    function to launch the boot operation

    @param DevicePath VOID *Options, UINTN OptionSize, BOOLEAN ValidBootOption

    @retval status

**/
EFI_STATUS _BootLaunchDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevicePath, VOID *Options, UINTN OptionSize, BOOLEAN ValidBootOption )
{
    EFI_STATUS	Status;
    EFI_HANDLE	handle;
    BOOLEAN		FreeDevicePath = FALSE;
    EFI_EVENT	ReadyToBootEvent;
	EFI_GUID	Bootguid = AMITSE_EVENT_BEFORE_BOOT_GUID;
	EFI_GUID	AfterBootGuid = AMITSE_EVENT_AFTER_BOOT_GUID; 
	EFI_STATUS	WatchDogTimerStatus = EFI_UNSUPPORTED;
    EFI_TPL CurrentTpl;
	TSE_POST_STATUS	BackupPostStatus;
	EFI_LOADED_IMAGE_PROTOCOL *Image = NULL;
	UINT16 *BootCurrent = NULL;
	UINTN		size = 0;
	
	
#ifndef STANDALONE_APPLICATION
    EFI_CONSOLE_CONTROL_SCREEN_MODE ScreenMode = EfiConsoleControlScreenText;
#endif //STANDALONE_APPLICATION
    SETUP_DEBUG_TSE("\n[TSE] _BootLaunchDevicePath Entering :\n");
    if(DevicePath != NULL)
    	SETUP_DEBUG_TSE("[TSE] gBootFlow = %d  DevicePath->Type = %d DevicePath->SubType = %d\n",gBootFlow,DevicePath->Type,DevicePath->SubType);
	BackupPostStatus = gPostStatus; // Back it up and of boot fail restore it back
	gPostStatus = TSE_POST_STATUS_PROCEED_TO_BOOT;

#if defined(EFI_EVENT_SIGNAL_READY_TO_BOOT) && EFI_SPECIFICATION_VERSION<0x20000
    Status = gBS->CreateEvent(
                EFI_EVENT_SIGNAL_READY_TO_BOOT | EFI_EVENT_NOTIFY_SIGNAL_ALL,
                EFI_TPL_CALLBACK,
                NULL,
                NULL,
                &ReadyToBootEvent
                );
#else
    Status = TseEfiCreateEventReadyToBoot(
                EFI_TPL_CALLBACK,
                NULL,
                NULL,
                &ReadyToBootEvent
                );
#endif //EFI_EVENT_SIGNAL_READY_TO_BOOT

    if (EFI_ERROR(Status))
        return Status;

    gBS->SignalEvent( ReadyToBootEvent );
    gBS->CloseEvent( ReadyToBootEvent );
	EfiLibReportStatusCode(EFI_PROGRESS_CODE, DXE_READY_TO_BOOT,0,NULL,NULL);

    TSEIDEPasswordFreezeDevices();

    if( gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT)
    {
		// Call the Hook and do the FastBoot
		if ( BBSValidDevicePath( DevicePath ) )
		{
			BeforeLegacyBootLaunchHook();
			UninstallBgrtWrapper();
		}
		else
		{
			BeforeEfiBootLaunchHook();							//Clearing the logo for shell in fast boot also 
			if (NULL != gShellLaunchEvent)				
			{
				gBS->CloseEvent (gShellLaunchEvent);
				gShellLaunchEvent = NULL;
			}
			if (NULL != gShell20LaunchEvent)					
			{
				gBS->CloseEvent (gShell20LaunchEvent);
				gShell20LaunchEvent = NULL;
			}
			// Validate the Gop before usage in all the possible cases and also get instance of Gop through notification 
			if ( ( NULL != gGOP ) && (CurrentScreenresolutionX && CurrentScreenresolutionY) && //If it has valid gGOP and resolution
				 ((CurrentScreenresolutionX != gGOP->Mode->Info->HorizontalResolution) || (CurrentScreenresolutionY != gGOP->Mode->Info->VerticalResolution)) //If current and quietboot resolution is different
				) 
			{
				GOPSetScreenResolution(&CurrentScreenresolutionX, &CurrentScreenresolutionY);
				//gGOP->SetMode (gGOP, CurrentResolutionIndex); // To maintain graphics resolution 
			}
			EfiLibNamedEventSignal (&Bootguid);
			_RegisterShellGuid ();
		}
		// Performance measurement Pause
		PERF_END (0, AMITSE_TEXT("Boot"), NULL, 0);
		Status = FastBootLaunch();
		if ( BBSValidDevicePath( DevicePath ) ) 
		{
			ReInstallBgrtWrapper (); // in fast boot, if its legacy then we will reinstall as we uninstalled bgrt
		}
		EfiLibNamedEventSignal (&AfterBootGuid);
		return Status;
    }
#if APTIO_4_00						//Useful for secure boot violation message box
	gMaxRows = MAX_ROWS;
	gMaxCols = MAX_COLS;
#endif
    if ( BBSValidDevicePath( DevicePath ) )			// Changing the mode only for csm
    {
        SETUP_DEBUG_TSE("\n[TSE]  Legacy Devicepath :\n");
#ifndef STANDALONE_APPLICATION
	// Fast Boot May want to boot without Console Control
		if (gConsoleControl != NULL)
		{
		    gConsoleControl->GetMode (gConsoleControl, &ScreenMode, NULL, NULL);
		    if (ScreenMode != EfiConsoleControlScreenText)
			{
		        gConsoleControl->SetMode (gConsoleControl, EfiConsoleControlScreenText);
				InvalidateStatusInBgrtWrapper ();
			}
		}
#endif //STANDALONE_APPLICATION
        BeforeLegacyBootLaunchHook();
        UninstallBgrtWrapper();
        Status = CsmBBSLaunchDevicePath( DevicePath );
        ReInstallBgrtWrapper();
		if (EFI_ERROR (Status))
			LegacyBootFailHook(Status);
        AfterLegacyBootLaunchHook();
		gPostStatus = BackupPostStatus;

		EfiLibNamedEventSignal (&AfterBootGuid);
		SETUP_DEBUG_TSE("\n[TSE] _BootLaunchDevicePath Exiting :\n");
        return Status;
    }

    CurrentTpl = gBS->RaiseTPL( EFI_TPL_HIGH_LEVEL );
    gBS->RestoreTPL( EFI_TPL_APPLICATION );
    if (DevicePath->Type==MEDIA_DEVICE_PATH && DevicePath->SubType==MEDIA_HARDDRIVE_DP)
    {
        DevicePath = _DiscoverPartition(DevicePath);
        FreeDevicePath = TRUE;
    }
    else if(DevicePath->Type==MEDIA_DEVICE_PATH && DevicePath->SubType==MEDIA_FILEPATH_DP && (!gFileParsed))
    {
     	  CHAR16	*FileName = NULL;
    	   	  
     	  FileName = GetFileNameFromDevPath(DevicePath);
		  if(NULL != FileName)
		  {
          gFileParsed = TRUE;
          Status = BootLaunchFilename(FileName);
          gPostStatus = BackupPostStatus;
          gFileParsed = FALSE;
          MemFreePointer((VOID **) &FileName);
	       
          if ( CurrentTpl > EFI_TPL_APPLICATION )
         	 gBS->RaiseTPL( CurrentTpl );
          return Status;
		  }
     }
    else if(IsURIDevicePath(DevicePath)) 
    {
   	 EFI_DEVICE_PATH_PROTOCOL    *TempDevicePath = NULL;
   	 TempDevicePath = EfiHTTPSGetFullDevicePath(DevicePath);
   	 if(TempDevicePath)
   	 {
   		 DevicePath = TempDevicePath;
   		 FreeDevicePath = TRUE;
   	 }
    }
    
    Status = gBS->LoadImage (TRUE, gImageHandle, DevicePath, NULL, 0, &handle);
    
    SETUP_DEBUG_TSE("\n[TSE] gBS->LoadImage 1 Status = %r\n",Status);
    if (EFI_ERROR (Status))
    {
        EFI_DEVICE_PATH_PROTOCOL    *TempDevicePath = DevicePath;
        EFI_STATUS RepairStatus = FALSE;
		UefiBootFailHook (Status);
        RepairStatus = CheckForDeviceNeedRepair (TempDevicePath);           //support for UEFI specification v 2.3.1, p. 10.10.1 Driver health protocol
        SETUP_DEBUG_TSE("[TSE] RepairStatus = %r\n",Status);
        if (!(EFI_ERROR (RepairStatus)))
        {
            Status = gBS->LoadImage (TRUE, gImageHandle, DevicePath, NULL, 0, &handle);
            SETUP_DEBUG_TSE("\n[TSE] gBS->LoadImage 2 Status = %r\n",Status);
			if (EFI_ERROR (Status))
			{
				UefiBootFailHook (Status);
			}
        }
    }
    if (EFI_ERROR (Status))
    {
        //Try default behaviour
        EFI_DEVICE_PATH_PROTOCOL    *TempDevicePath = NULL;
        EFI_HANDLE                  DevHandle;
        EFI_HANDLE                  UsbDevHandle = NULL;
        EFI_BLOCK_IO_PROTOCOL       *BlkIo = NULL;
        VOID                        *Buffer = NULL;
        EFI_DEVICE_PATH_PROTOCOL    *FilePath = NULL;

        // Find a Simple File System protocol on the device path.
        TempDevicePath = DevicePath;

        UsbDevHandle = CheckDevSupShortFormPath (DevicePath);       ////Support for section 3.1.2. booting from a short-form device path
        if (NULL != UsbDevHandle)
        {
            DevHandle = UsbDevHandle;
        }
        else
        {
            Status = gBS->LocateDevicePath (
                        &gEfiSimpleFileSystemProtocolGuid,
                        &TempDevicePath,
                        &DevHandle
                        );
        }
        if ((!EFI_ERROR (Status) && IsDevicePathEnd (TempDevicePath)) || (NULL != UsbDevHandle))
        {
        	if(IsMediaFileDevicePath(DevicePath))
        	{
        		FilePath = EfiLibAllocateZeroPool(EfiDevicePathSize(DevicePath));
        		if(FilePath)
        			MemCpy(FilePath,DevicePath,EfiDevicePathSize(DevicePath)); 
        	}
        	else
        	{     		  
        		// Files are specified in the device path so try to
        		// load the default removable media file name.
        		FilePath = EfiFileDevicePath (DevHandle, gBootFileName);
        	}
        	
			if (FilePath)
			{
                // Issue a dummy read to the device to check for media
                // change. When the removable media is changed, any Block
                // IO read/write will cause the BlockIo protocol be
                // reinstalled and EFI_MEDIA_CHANGED is returned. After
                // the Block IO protocol is reinstalled, subsequent Block
                // IO read/write will success.
                Status = gBS->HandleProtocol (
                                    DevHandle,
                                    &gEfiBlockIoProtocolGuid,
                                    (VOID **) &BlkIo
                                    );
                if (!EFI_ERROR (Status))
                {
                    Buffer = EfiLibAllocatePool (BlkIo->Media->BlockSize);
                    if (Buffer != NULL)
                    {
                        BlkIo->ReadBlocks (
                            BlkIo,
                            BlkIo->Media->MediaId,
                            0,
                            BlkIo->Media->BlockSize,
                            Buffer
                            );
                        MemFreePointer((VOID **)&Buffer);
                    }
                }

                Status = gBS->LoadImage (
                    TRUE,
                    gImageHandle,
                    FilePath,
                    NULL,
                    0,
                    &handle
                    );
                
				if (EFI_ERROR (Status))
				{
					UefiBootFailHook (Status);
				}
                MemFreePointer((VOID **) &FilePath);
            }
            else
            {
                Status = EFI_NOT_FOUND;
            }
        }
        else
        {
            Status = EFI_NOT_FOUND;
        }
    }


    if(ValidBootOption==TRUE)
        if (EFI_ERROR (Status)) // Report only if it Boot Option launch - Boot of loading Error!!!
    		EfiLibReportStatusCode(EFI_ERROR_CODE| EFI_ERROR_MAJOR, DXE_BOOT_OPTION_LOAD_ERROR,0,NULL,NULL);

    if (FreeDevicePath) gBS->FreePool(DevicePath);

    if ( ! EFI_ERROR( Status ) )
    {
		Status = gBS->HandleProtocol (
		 				  handle,
		                  &gEfiLoadedImageProtocolGuid,
		                  (VOID**)&Image
		                  );
		if (!EFI_ERROR (Status))
		{
			if(IsImageCodeTypeSupport())
			{
				if (Image->ImageCodeType  != EfiLoaderCode){
					TRACE((TRACE_DXE_CORE, "ERROR: Image referred by %x is not a UEFI application. Skipping...\n", Image->ImageCodeType));
					Status = EFI_UNSUPPORTED;
					gPostStatus = BackupPostStatus;
					if ( CurrentTpl > EFI_TPL_APPLICATION )
						gBS->RaiseTPL( CurrentTpl );
					return Status;
			    }
			}
		}
		if (gST->ConOut != NULL)
        {
            gST->ConOut->EnableCursor (gST->ConOut, FALSE);         //Disabling bcoz Cursor appears on the WIN8 boot
        }
        _BootInstallLoadOptions( handle, Options, OptionSize );

        BeforeEfiBootLaunchHook();

		// Performance measurement Pause
		PERF_END (0, AMITSE_TEXT("Boot"), NULL, 0);

		SETUP_DEBUG_TSE("\n[TSE] Before Validating Gop before StartImage()\n",Status);
		PrintGopAndConInDetails();
		
		// Validate the Gop before usage in all the possible cases and also get instance of Gop through notification
		if ( ( NULL != gGOP ) && (CurrentScreenresolutionX && CurrentScreenresolutionY) && //If it has valid gGOP and resolution
			 ((CurrentScreenresolutionX != gGOP->Mode->Info->HorizontalResolution) || (CurrentScreenresolutionY != gGOP->Mode->Info->VerticalResolution)) //If current and quietboot resolution is different
			) 
		{
			GOPSetScreenResolution(&CurrentScreenresolutionX, &CurrentScreenresolutionY);
			//gGOP->SetMode (gGOP, CurrentResolutionIndex); // To maintain graphics resolution 
		}

		SETUP_DEBUG_TSE("\n[TSE] After Validating Gop before StartImage()\n",Status);
		PrintGopAndConInDetails();
		
		// Signal AMITSE_EVENT_BEFORE_BOOT_GUID Event;
		EfiLibNamedEventSignal (&Bootguid);
		_RegisterShellGuid ();						//clearing the screen if its shell boot
								
		if (IsTseWatchDogTimer()) 
		{
			UINT16 i = 0;
			
			BootCurrent = (UINT16 *)VarGetNvramName( L"BootCurrent", &gEfiGlobalVariableGuid, NULL, &size );
			
			if (BootCurrent) {
			    BOOT_DATA	*pBootData = NULL;
			    pBootData = BootGetBootData(*BootCurrent);
			    
			    if (pBootData && !(pBootData->Active & LOAD_OPTION_CATEGORY_APP))
			    {
					//=enabling the watch dog timer on load success==//
					TRACE ((-1,"Load Image() Return Status = %r\n", Status)); 
					WatchDogTimerStatus = gBS->SetWatchdogTimer (5 * 60, 0, 0, NULL);
					gWatchDogTimerSet = TRUE;
					TRACE ((-1,"WatchDog Timer Status = %r\n", WatchDogTimerStatus)); 
			    }
	        	MemFreePointer( (VOID **)&BootCurrent );
			}
		}

		Status = gBS->StartImage( handle, NULL, NULL );
		SETUP_DEBUG_TSE("\n[TSE] gBS->StartImage Status= %r\n",Status);		
		if (IsTseWatchDogTimer()) 
		{
			//====disabling the timer on start image failure==//
			if (!EFI_ERROR (WatchDogTimerStatus) && gWatchDogTimerSet)
			{
				TRACE ((-1,"Start Image() Return Status = %r\n", Status)); 
				//clearing the watch dog timer.
				WatchDogTimerStatus = gBS->SetWatchdogTimer(0, 0, 0, NULL); 
				gWatchDogTimerSet = FALSE;
				TRACE ((-1,"WatchDog Timer Status = %r\n", WatchDogTimerStatus));
			}
		}
		
		if (NULL != gShellLaunchEvent)
		{
			gBS->CloseEvent (gShellLaunchEvent);
			gShellLaunchEvent = NULL;
		}
		if (NULL != gShell20LaunchEvent)				
		{
			gBS->CloseEvent (gShell20LaunchEvent);
			gShell20LaunchEvent = NULL;
		}
		// Performance measurement continue
		PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);

        AfterEfiBootLaunchHook();
		EfiLibNamedEventSignal (&AfterBootGuid);
/*#if APTIO_4_00
		gMaxRows = MAX_ROWS;
		gMaxCols = MAX_COLS;
#endif //APTIO_4_00

		SetDesiredTextMode();			// StyleInit will set the desired text mode*/
		SETUP_DEBUG_TSE("\n[TSE] Clearing Graphics Screen\n");

      ClearGrphxScreen ();
        if(EFI_ERROR( Status ))
        {
      	  InvalidateStatusInBgrtWrapper ();
            // Report only if it is Boot Option launch - Starting the Boot option failed.!!!!
            if(ValidBootOption==TRUE)
    			EfiLibReportStatusCode(EFI_ERROR_CODE| EFI_ERROR_MAJOR, DXE_BOOT_OPTION_FAILED,0,NULL,NULL);

    		//Clear the flush buffer so that flush lines will actually
            //draw the complete screen again
			 if ( NULL != gFlushBuffer)
        		MemSet(gFlushBuffer, sizeof(SCREEN_BUFFER), 0);

			if (gST->ConOut != NULL)
			{
	            //Disable cursor, set desired attributes and clear screen
	            gST->ConOut->EnableCursor( gST->ConOut, FALSE );
	            gST->ConOut->SetAttribute( gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE);
	            gST->ConOut->ClearScreen( gST->ConOut);
			}
			SETUP_DEBUG_TSE("\n[TSE] Redrawing whole screen using FlushLines()\n",Status);
            //Call flush lines to draw the whole screen again
            FlushLines( 0, gMaxRows - 1 );
			//DoRealFlushLines();			
        }
		else
		{
			InvalidateStatusInBgrtWrapper ();			//When win8 launched after successfull boot then BGRT table should be invalidated
		}
    }
	 gPostStatus = BackupPostStatus; /// restore the pre. Post status.

    if ( CurrentTpl > EFI_TPL_APPLICATION )
        gBS->RaiseTPL( CurrentTpl );
	
    SETUP_DEBUG_TSE("\n[TSE] _BootLaunchDevicePath Exiting :\n");
    return Status;
}

/**
    function to launch the boot operation

    @param DevicePath 

    @retval device path

**/
EFI_DEVICE_PATH_PROTOCOL* _DiscoverPartition(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
    EFI_STATUS	Status;
    EFI_HANDLE	*Handle;
    UINTN		Count, i;

    EFI_DEVICE_PATH_PROTOCOL *FullDevicePath=NULL;
    HARDDRIVE_DEVICE_PATH* BootParitionDevicePath  = (HARDDRIVE_DEVICE_PATH*)DevicePath;

    //get list of available Block I/O devices
    Status = gBS->LocateHandleBuffer(ByProtocol,&gEfiBlockIoProtocolGuid,NULL,&Count,&Handle);
    if (EFI_ERROR(Status)) return NULL;

    for( i=0; i<Count; i++ )
    {
        EFI_BLOCK_IO_PROTOCOL		*BlockIo;
        EFI_DEVICE_PATH_PROTOCOL	*PartitionDevicePath, *TmpDevicePath;
        HARDDRIVE_DEVICE_PATH*		PartitionNode;

        Status = gBS->HandleProtocol(Handle[i],&gEfiBlockIoProtocolGuid,&BlockIo);
        if (EFI_ERROR(Status))
			continue;

        // if this is not partition, continue
        if (!BlockIo->Media->LogicalPartition)
			continue;

		Status = gBS->HandleProtocol(Handle[i],&gEfiDevicePathProtocolGuid,&PartitionDevicePath);
        if (EFI_ERROR(Status))
			continue;

        // Get last node of the device path. It should be partition node
        PartitionNode = (HARDDRIVE_DEVICE_PATH*)PartitionDevicePath;

        for( TmpDevicePath = PartitionDevicePath;
             !IsDevicePathEndType(TmpDevicePath);
             TmpDevicePath=NextDevicePathNode(TmpDevicePath) )
		{
			PartitionNode = (HARDDRIVE_DEVICE_PATH*)TmpDevicePath;
		}

        //Check if our partition matches Boot partition
        if (PartitionNode->Header.Type!=MEDIA_DEVICE_PATH || PartitionNode->Header.SubType!=MEDIA_HARDDRIVE_DP)
			continue;

        if ( PartitionNode->PartitionNumber==BootParitionDevicePath->PartitionNumber &&
             PartitionNode->SignatureType==BootParitionDevicePath->SignatureType &&
             !MemCmp(PartitionNode->Signature,BootParitionDevicePath->Signature,16) )
        {
            //Match found
            FullDevicePath = EfiAppendDevicePath(PartitionDevicePath,NextDevicePathNode(DevicePath));
            break;
        }
    }

    gBS->FreePool(Handle);
    return FullDevicePath;
}

/**
    function to install the load options

    @param handle VOID *Options, UINTN OptionSize

    @retval void

**/
VOID _BootInstallLoadOptions( EFI_HANDLE handle, VOID *Options, UINTN OptionSize )
{
    EFI_STATUS Status;
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;

    Status = gBS->HandleProtocol( handle, &gEfiLoadedImageProtocolGuid, &LoadedImage );
    if ( EFI_ERROR( Status ) )
        return;

    LoadedImage->LoadOptions = Options;
    LoadedImage->LoadOptionsSize = (UINT32)OptionSize;

    gCurrentBootHandle = handle;		//Exposing the Handle of the image that's being launched for boot, This will help other elinks to do the processing.

}

#if EFI_SPECIFICATION_VERSION<0x20000
static MEDIA_FW_VOL_FILEPATH_DEVICE_PATH  gFvFile =
{
  { MEDIA_DEVICE_PATH, MEDIA_FV_FILEPATH_DP, { sizeof(MEDIA_FW_VOL_FILEPATH_DEVICE_PATH), 0 } },
  MINI_SETUP_DATA_GUID
};
#endif //EFI_SPECIFICATION_VERSION

static EFI_DEVICE_PATH_PROTOCOL gEndDevicePath =
{
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 }
};

/**
    function to build firmware volume device path protocol.

    @param index EFI_GUID *guidPtr

    @retval device path

**/
EFI_DEVICE_PATH_PROTOCOL *_BootBuildFVDevicePath( UINT32 *index, EFI_GUID *guidPtr )
{
	EFI_STATUS  Status;
	EFI_HANDLE  *HandleBuffer;
	UINTN       Count;
	UINT32      i;
	EFI_GUID tempEfiFirmwareVolumeProtocolGuid;

    EFI_DEVICE_PATH_PROTOCOL *DevicePath, *FilePath = NULL;
#if EFI_SPECIFICATION_VERSION>=0x20000
    MEDIA_FW_VOL_FILEPATH_DEVICE_PATH gFvFile;
#endif //EFI_SPECIFICATION_VERSION

	if ( PISpecVersion() < 0x00010000 )
		tempEfiFirmwareVolumeProtocolGuid = gEfiFirmwareVolumeProtocolGuid;
	else
		tempEfiFirmwareVolumeProtocolGuid = gEfiFirmwareVolume2ProtocolGuid;

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &tempEfiFirmwareVolumeProtocolGuid,
            NULL,
            &Count,
            &HandleBuffer
            );

    if ( EFI_ERROR( Status ) )
    {
        *index = (UINT32)-1;
        return FilePath;
    }

#if EFI_SPECIFICATION_VERSION<0x20000
    MemCopy( (UINT8 *)&gFvFile + sizeof(EFI_DEVICE_PATH_PROTOCOL), guidPtr, sizeof(EFI_GUID) );
#endif //EFI_SPECIFICATION_VERSION

    for ( i = *index; i < (UINT32)Count; i++ )
    {
        Status = gBS->HandleProtocol( HandleBuffer[i], &gEfiDevicePathProtocolGuid, &DevicePath );
        if ( EFI_ERROR( Status ) )
            continue;

#if EFI_SPECIFICATION_VERSION>=0x20000
        TseEfiInitializeFwVolDevicepathNode (&gFvFile, guidPtr);
#endif //EFI_SPECIFICATION_VERSION

        FilePath = EfiAppendDevicePathNode( DevicePath, (EFI_DEVICE_PATH_PROTOCOL *)&gFvFile );
        if ( FilePath != NULL )
        {
            *index = i + 1;
            break;
        }
    }

    if ( i == (UINT32)Count )
        *index = (UINT32)-1;

    MemFreePointer( (VOID **)&HandleBuffer );

    return FilePath;
}

/**
    function to build File device path protocol.

    @param index CHAR16 *fileName

    @retval device path

**/
EFI_DEVICE_PATH_PROTOCOL *_BootBuildFileDevicePath( UINT32 *index, CHAR16 *fileName )
{
    EFI_STATUS  Status;
    EFI_HANDLE  *HandleBuffer;
    UINTN       Count;
    UINT32      i;

    EFI_DEVICE_PATH_PROTOCOL *FilePath = NULL;

    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            &Count,
            &HandleBuffer
            );

    if ( EFI_ERROR( Status ) )
    {
        *index = (UINT32)-1;
        return FilePath;
    }

    for ( i = *index; i < (UINT32)Count; i++ )
    {
        FilePath = EfiFileDevicePath( HandleBuffer[i], fileName );
        if ( FilePath != NULL )
        {
            *index = i + 1;
            break;
        }
    }

    if ( i == (UINT32)Count )
        *index = (UINT32)-1;

    MemFreePointer( (VOID **)&HandleBuffer );
    return FilePath;
}

/**
    Function to check the boot option status

    @param value BOOLEAN ShowAllBBSDev, BOOLEAN TseBootNowInBootOrde, BOOLEAN FromSetup

    @retval 

**/
BOOLEAN IsBootDeviceEnabled( UINT16 value, BOOLEAN ShowAllBBSDev, BOOLEAN TseBootNowInBootOrde, BOOLEAN FromSetup)
{

	UINT16	*BootOrder=NULL;
    UINTN	size = 0;
    UINTN	i,j,k;
    UINT16	count;

    BOOT_DATA *bootData;

	if((!ShowAllBBSDev) && (!TseBootNowInBootOrde))
		if(gBootData == NULL)
			return FALSE;
		else
			{
				bootData = &gBootData[value];
				if(NULL == bootData)	{
					TRACE((-1,"\n[TSE] Boot data is NULL\n"));
					ASSERT(0);
				}
				if(BBSValidDevicePath(bootData->DevicePath) )
					return BootGetBBSOptionStatus(bootData, 0, FromSetup,ShowAllBBSDev);
				else
	      		return BootGetOptionStatus(bootData, FromSetup);//Getting the Boot option status when TSE_BOOT_NOW_IN_BOOT_ORDER and SETUP_SHOW_ALL_BBS_DEVICES is OFF.
			}
	if(TseBootNowInBootOrde)
	{
		BootOrder = HelperGetVariable(VARIABLE_ID_BOOT_ORDER, L"BootOrder", &gEfiGlobalVariableGuid, NULL, &size );
		if (NULL == BootOrder)		//Check for boot order else the system will hang
		{
			return TRUE;
		}
	    //Find the first disabled option
	    for ( i = 0; i < gBootOptionCount; i++ )
	    {
	        if ( DISABLED_BOOT_OPTION == BootOrder[i] )
	            break;
	    }

	    if(i<gBootOptionCount)
	    {
	        //There are disabled options replace them with valid options
	        for(j=0;j<gBootOptionCount;j++)
	        {
	            for(k=0;k<gBootOptionCount;k++)
	            {
	                if(BootOrder[k] == gBootData[j].Option)
	                    break;
	            }

	            if(k >= gBootOptionCount)
	            {
	                //gBootData[j].Option is not present in BootOrder; fill it
	                BootOrder[i] = gBootData[j].Option;
	                i++;
	            }
	        }
	    }
	}

	if(ShowAllBBSDev)
	{
	    count = 0;
	    for(i=0;i<gBootOptionCount;i++)
	    {
			if(TseBootNowInBootOrde) {
				bootData = BootGetBootData(BootOrder[i]);
				if(NULL == bootData)	
				{
					TRACE((-1,"\nBoot data is NULL\n"));
					ASSERT(0);
				}
			}
			else {
	     		bootData = gBootData + i;
				if(NULL == bootData)	
				{
					TRACE((-1,"\n[TSE] Boot data is NULL\n"));
					ASSERT(0);
				}
			}

	        if ( BBSValidDevicePath(bootData->DevicePath) )
	        {
	            if((value >= count) && (value < (count+bootData->LegacyDevCount)))
	            {
					if(TseBootNowInBootOrde)
		                MemFreePointer((VOID **) &BootOrder);
	                return BootGetBBSOptionStatus(bootData, value-count, FromSetup, ShowAllBBSDev);//Getting the Legacy Boot option status when SETUP_SHOW_ALL_BBS_DEVICES is ON.
	            }

	            count+=bootData->LegacyDevCount;
	        }
	        else {
	            if(value == count)
	            {
					if(TseBootNowInBootOrde)
		                MemFreePointer((VOID **) &BootOrder);
	                return BootGetOptionStatus(bootData, FromSetup);//Getting the Boot option status when SETUP_SHOW_ALL_BBS_DEVICES is ON.
	            }
	            count++;
	        }
	    }
 		return FALSE;
	}
	else {
	    bootData = BootGetBootData(BootOrder[value]);
	    
		if(NULL == bootData)	
		{
			TRACE((-1,"\nBoot data is NULL\n"));
			ASSERT(0);
		}
		
	    MemFreePointer((VOID **) &BootOrder);
		if(BBSValidDevicePath(bootData->DevicePath) )
			return BootGetBBSOptionStatus(bootData, 0, FromSetup,ShowAllBBSDev);
		else
	      return BootGetOptionStatus(bootData, FromSetup);
	}
}

/**
    Function to check the boot option status in Boot Order

    @param bootData BOOLEAN FromSetup

    @retval 

**/
BOOLEAN BootGetOptionStatus(BOOT_DATA *bootData, BOOLEAN FromSetup)
{
	UINTN size = 0;
	UINTN i=0;
	UINT16 *buffer = NULL;

			if(0 == FromSetup)
			{
					if(bootData->Active & LOAD_OPTION_ACTIVE)
						return FALSE;
					else
						return TRUE;

			}

			//buffer = EfiLibAllocateZeroPool(  gBootOptionCount * sizeof(UINT16));	//Memory leak fix
			buffer = HelperGetVariable(VARIABLE_ID_BOOT_ORDER, L"BootOrder", &gEfiGlobalVariableGuid, NULL, &size );
         if (NULL == buffer)
         {
            return TRUE;
         }

			for(i=0 ; i<gBootOptionCount ; i++)
			{
				if(buffer[i] == bootData->Option)
				   break;
			}
			if(i == gBootOptionCount )
			{
				MemFreePointer((VOID **) &buffer);
					return TRUE;
			}
			else
			{
				MemFreePointer((VOID **) &buffer);
					return FALSE;
			}

}

/**
    Function to check the Legacy boot option status

    @param bootData BOOLEAN FromSetup, BOOLEAN ShowAllBBSDev

    @retval 

**/
BOOLEAN BootGetBBSOptionStatus(BOOT_DATA *bootData, UINT16 value, BOOLEAN FromSetup, BOOLEAN ShowAllBBSDev)
{
	   UINT32 offset=0;
		UINT32 i;
		UINT8 *pDevOrder;
		BBS_ORDER_TABLE	*pDev;
		UINTN size = 0;
		UINT16 *buf = NULL, *Tempbuf = NULL;

		if(0 == FromSetup)
		{
		   if(!BootGetOptionStatus(bootData, FromSetup))
		   {
	            offset = (UINT16)bootData->LegacyEntryOffset;

			  		pDevOrder = HelperGetVariable(VARIABLE_ID_BBS_ORDER,L"LegacyDevOrder", &gLegacyDevGuid, NULL, &size);

	         	pDev = (BBS_ORDER_TABLE *)(pDevOrder + offset);

					if ( (pDev->Length >= size) || (0 == pDev->Length) ) 
						return TRUE;

              if(DISABLED_BOOT_OPTION == pDev->Data[value])
					{
						 MemFreePointer((VOID **) &pDevOrder);
  					    return  TRUE;
					}
    		  		else
					{
						 MemFreePointer((VOID **) &pDevOrder);
                   return  FALSE;
					}
		   }
		   else
		       return TRUE;
        }
		if(!BootGetOptionStatus(bootData, FromSetup))
		{

			size=0;
		 	offset = (UINT16)bootData->LegacyEntryOffset;
			pDevOrder = HelperGetVariable(VARIABLE_ID_BBS_ORDER,L"LegacyDevOrder", &gLegacyDevGuid, NULL, &size);
            if (NULL == pDevOrder)
            {
              return TRUE;
            }

			pDev = (BBS_ORDER_TABLE *)(pDevOrder + offset);

			if ( (pDev->Length >= size) || (0 == pDev->Length) ) 
				return TRUE;

			if(!ShowAllBBSDev)
			{
					if(DISABLED_BOOT_OPTION == pDev->Data[value])
					{
						MemFreePointer((VOID **) &pDevOrder);
  					   	return  TRUE;
    				}
					else
					{
						MemFreePointer((VOID **) &pDevOrder);
                  	return  FALSE;
					}
			}
			
	      buf = EfiLibAllocateZeroPool(  pDev->Length - sizeof(UINT16));
			
         if (NULL == buf)	
         {
            return FALSE;
         }
			
         Tempbuf = buf;
			MemCopy( buf, &pDev->Data, pDev->Length - sizeof(UINT16) );
			
			for(i=0; i < bootData->LegacyDevCount ;i++)
			{
            if(*buf == bootData->OrderList[value].Index)
               break;
            buf++;
			}
			
         MemFreePointer((VOID **) &pDevOrder);		
         MemFreePointer((VOID **) &Tempbuf);
			
			if(i ==  bootData->LegacyDevCount)
			{
            return  TRUE;
			}
			else
			{
            return  FALSE;
			}
		}
		else
			return TRUE;
}
/**
    Function to check the Boot option status if gLoadOptionhidden token is Enabled

    @param value 

    @retval BOOLEAN

**/
BOOLEAN LoadOptionhidden (UINT16 value, BOOLEAN Option)
{
	if (BOOT_ORDER_OPTION == Option)
	{
    	if (gBootData [value].Active & LOAD_OPTION_HIDDEN)
			return TRUE;
	}
	else if (DRIVER_ORDER_OPTION == Option)				
	{
		if (gDriverData [value].Active & LOAD_OPTION_HIDDEN)
			return TRUE;
	}	
	return FALSE;
}

/**
    Check whether the Boot/Driver option has hidden property

    @param UINT16, BOOLEAN

    @retval BOOLEAN

**/
BOOLEAN CheckHiddenforBootDriverOption (UINT16 Option, BOOLEAN HiddenOption)
{
	UINTN i = 0;
	if (gLoadOptionHidden)
	{	
		if (BOOT_ORDER_OPTION == HiddenOption)
		{
			for (i = 0 ;i < gBootOptionCount ;i++)
			{
				if (gBootData [i].Option == Option)
				{
					if (gBootData [i].Active & LOAD_OPTION_HIDDEN)
					{
						return TRUE;
					}
					break;
				}
			}
		}
		else if (DRIVER_ORDER_OPTION == HiddenOption)
		{
			for (i = 0 ;i < gDriverOptionCount ;i++)
			{
				if (gDriverData [i].Option == Option)
				{
					if (gDriverData [i].Active & LOAD_OPTION_HIDDEN)
					{
						return TRUE;
					}
					break;
				}
			}
		}
	}
	return FALSE;
}


/**
    Checks the input boot option matches with any of the boot option in the system

    @param UINT16 = Boot option to which match to be find

    @retval UINT32 = Returns 0 if no match found
        = 32 bit CRC value of boot option if match found

**/
UINT32 CheckBootOptionMatch (UINT16 BootOption)
{
//    UINTN	iIndex = 0;  Unused Variable
    CHAR16  BootOptionName [9];             //Bootxxxx + 1 NULL char
    UINT32	*LoadOptions = NULL;        //Using 32 bit ptr bcoz to find CRC32
    UINTN	LoadOptionSize = 0;
    UINT32  CRC32 = 0;

    SPrint (BootOptionName, sizeof (BootOptionName), L"Boot%04X", BootOption);
    LoadOptions = VarGetNvramName (BootOptionName, &gEfiGlobalVariableGuid, NULL, &LoadOptionSize);     //Getting boot options
    if ((NULL == LoadOptions) || (0 == LoadOptionSize))
    {
        return CRC32;           //returning 0
    }
   gBS->CalculateCrc32 ((UINT8 *)LoadOptions, LoadOptionSize, &CRC32);
   return CRC32;
}

/**
    Function to set the BootOptionSupport variable

    @param UINT32 = Capabilities for the BootOptionSupport variable

    @retval VOID

**/
VOID SetBootOptionSupportVariable (UINT32 BootManCapabilities)
{
    CHAR16 VariableName [] = L"BootOptionSupport";
    UINT32 Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    UINTN DataSize = 0;
    VOID *Data;
    UINT32 SetData = 0;

    Data = VarGetNvramName (VariableName, &gEfiGlobalVariableGuid, &Attributes, &DataSize);     //If the variable exists use its attribute to set it
    if (NULL != Data)
    {
        SetData = *((UINT32 *)Data);
    }
    SetData |= BootManCapabilities;
    DataSize = sizeof (UINT32);         //Sizeof BootOptionSupport variable is UINT32
    VarSetNvramName (VariableName, &gEfiGlobalVariableGuid, Attributes, (VOID *)&SetData, DataSize);
}

/**
    Matches the device path with USB class device path (Table 60) and returns
    the corresponding USB's file system handle

    @param EFI_DEVICE_PATH_PROTOCOL * -> Device path for the boot option

    @retval EFI_HANDLE -> Handle for the file system

**/
EFI_HANDLE CheckDevSupShortFormPath (EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
	UINTN       NumHandles = 0;
	UINTN       iIndex = 0;
	CHAR16		*USBString = NULL;
	CHAR16		*USBDevPathString = NULL;
	UINT16     	*LangIDTable;
	UINT16		TableSize = 0;
	EFI_STATUS  Status;
	EFI_HANDLE  *UsbIoHandles = NULL;
    EFI_USB_IO_PROTOCOL         *UsbIoProtocolInstance = NULL;
    EFI_USB_DEVICE_DESCRIPTOR   DeviceDescriptor;
    USB_CLASS_DEVICE_PATH       *UsbClassDevPath = NULL;
	USB_WWID_DEVICE_PATH		*UsbWwidDevPath = NULL;
    EFI_DEVICE_PATH_PROTOCOL    *TempDevPath = DevicePath,*HandleDevPath = NULL;
	
    if ((MESSAGING_DEVICE_PATH == TempDevPath->Type) && (MSG_USB_CLASS_DP == TempDevPath->SubType))     //Check for USB Device Path Class. type = 3 and subtype = 0xf
    {
		UsbClassDevPath = (USB_CLASS_DEVICE_PATH *)TempDevPath;
    }
    else if ((MESSAGING_DEVICE_PATH == TempDevPath->Type) && (MSG_USB_WWID_CLASS_DP == TempDevPath->SubType))
    {
		UsbWwidDevPath = (USB_WWID_DEVICE_PATH *)TempDevPath;
		USBDevPathString = (CHAR16 *)((UINT8 *)UsbWwidDevPath + sizeof (USB_WWID_DEVICE_PATH));		//String will be present at the end of the WWID device path
    }
	else
	{
		return NULL;
	}
	Status = gBS->LocateHandleBuffer (                  //To match with USB Device Path Class
				ByProtocol,
				&gEfiUsbIoProtocolGuid,
				NULL,
				&NumHandles,
				&UsbIoHandles
				);
	if (EFI_ERROR (Status))
	{
        return NULL;
	}
	for (iIndex = 0; iIndex < NumHandles; iIndex ++)
	{
		Status = gBS->HandleProtocol (UsbIoHandles [iIndex], &gEfiUsbIoProtocolGuid, (void**) &UsbIoProtocolInstance);
		if (EFI_ERROR (Status))
    	{
            continue;
        }
        Status = UsbIoProtocolInstance->UsbGetDeviceDescriptor (UsbIoProtocolInstance, &DeviceDescriptor);
        if (EFI_ERROR (Status))
    	{
            continue;
        }
		if (UsbWwidDevPath)
		{
			Status = UsbIoProtocolInstance->UsbGetSupportedLanguages (UsbIoProtocolInstance, &LangIDTable, &TableSize);
			if (!EFI_ERROR (Status) && TableSize)
			{
				Status = UsbIoProtocolInstance->UsbGetStringDescriptor (UsbIoProtocolInstance, LangIDTable [0], DeviceDescriptor.StrSerialNumber, &USBString);		//LangIDTable [0], getting default as English
				if (EFI_ERROR (Status))
				{
					USBString = NULL;  		//Explicitly making as NULL
				}
			}
			if ((NULL != USBString) ^ (0 != EfiStrLen (USBDevPathString)))		//If serial number string present in device path and not in descriptor then try for other device and vice versa too
			{																			//If device path and descriptor not has the string then proceed
				continue;
			}
		}
		if (UsbClassDevPath?
		(			//Check for USB Class device path
        ((UsbClassDevPath->VendorId == DeviceDescriptor.IdVendor) || (0xFFFF == UsbClassDevPath->VendorId)) &&      //If values are 0xF's then dont consider that option
        ((UsbClassDevPath->ProductId == DeviceDescriptor.IdProduct) || (0xFFFF == UsbClassDevPath->ProductId)) &&
        ((UsbClassDevPath->DeviceClass == DeviceDescriptor.DeviceClass) || (0xFF == UsbClassDevPath->DeviceClass)) &&
        ((UsbClassDevPath->DeviceSubClass == DeviceDescriptor.DeviceSubClass) || (0xFF == UsbClassDevPath->DeviceSubClass)) &&
        ((UsbClassDevPath->DeviceProtocol == DeviceDescriptor.DeviceProtocol) || (0xFF == UsbClassDevPath->DeviceProtocol))
        ):
		(			//Check for USB WWID device path
		(UsbWwidDevPath->VendorId == DeviceDescriptor.IdVendor) &&
		(UsbWwidDevPath->ProductId == DeviceDescriptor.IdProduct) &&
		(USBString ? (!(EfiStrCmp (USBDevPathString, USBString))):1)			//String number of USB might not be filled in some case in such conditions will take it as TRUE
		)
		)
        {
            UINTN       Count;
        	EFI_GUID    **ppGuid;
            UINTN       jIndex = 0;
            UINTN       kIndex = 0;

            Status = gBS->ProtocolsPerHandle (UsbIoHandles [iIndex], &ppGuid, &Count);
            if (EFI_ERROR (Status))
            {
                continue;
            }
            for (jIndex = 0; jIndex < Count; jIndex ++)
	        {
                if (!guidcmp (ppGuid [jIndex], &gEfiDiskIoProtocolGuid))
                {
                    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *pInfo;
                    UINTN InfoCount = 0;
                    VOID  *FilsSystemInstance = NULL;
                    EFI_HANDLE handle = NULL;
                    EFI_DEVICE_PATH_PROTOCOL    *FilePath = NULL;

                    Status = gBS->OpenProtocolInformation (UsbIoHandles [iIndex], ppGuid [jIndex], &pInfo, &InfoCount);
                    if (EFI_ERROR (Status))
                    {
                        continue;
                    }
                    for (kIndex = 0; kIndex < InfoCount; kIndex ++)
                    {
                        Status = gBS->HandleProtocol (pInfo [kIndex].ControllerHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FilsSystemInstance);
                        if (EFI_ERROR (Status))
                        {
                            continue;
                        }
                        HandleDevPath = EfiDevicePathFromHandle(pInfo [kIndex].ControllerHandle);
                        if(IsMediaFileDevicePath(HandleDevPath))
                        {
                        	FilePath = EfiLibAllocateZeroPool(EfiDevicePathSize(HandleDevPath));
                        	if(FilePath)
                        		MemCpy(FilePath,HandleDevPath,EfiDevicePathSize(HandleDevPath));                        	
                        }
                        else
                        	FilePath = EfiFileDevicePath (pInfo [kIndex].ControllerHandle, gBootFileName);

                        if (FilePath)
                        {
                            Status = gBS->LoadImage (                       //Ensuring the image can load
                                                       TRUE,
                                                        gImageHandle,
                                                        FilePath,
                                                        NULL,
                                                        0,
                                                        &handle
                                                    );
                            MemFreePointer((VOID **) &FilePath);
                            if (!EFI_ERROR (Status))
                            {
								if (USBString)
								{
									MemFreePointer ((VOID **)&USBString);
								}
                                MemFreePointer ((VOID **)&UsbIoHandles);
                                return pInfo [kIndex].ControllerHandle;
                            }

                        }
                    }
                    if (InfoCount)
					{
						MemFreePointer ((VOID **)&pInfo);
                	}
                }
            }
        }
		if (USBString)
		{
			MemFreePointer ((VOID **)&USBString);
		}
	}
	MemFreePointer ((VOID **)&UsbIoHandles);
    return NULL;
}

/**
    This function collects all the BOOT#### name (e.g BOOT00001, BOOT002)
    check with BOOTOrder Variable. If BOOT#### is not present in BootOrder variable
    then it need to skip to create TSE bootData. If present then continue to create TSE
    TSE BootData

    @param VOID

    @retval Return TRUE/FALSE to skip BOOT####

**/
BOOLEAN TSESkipBootOrderVar(UINT16 *BootOrder, UINTN BootOrderSize,CHAR16 *VarName)
{
	EFI_STATUS Status;
	UINTN   Index = 0 ;
	UINTN	 Length = 2;
	UINT16 Option = 0xffff;
	Status = HexStringToBuf((UINT8 *)(&Option),&Length,&(VarName[4]), NULL) ;
	if (EFI_ERROR(Status)) 
		return TRUE;
	for (Index = 0 ; Index < BootOrderSize/sizeof(UINT16) ; Index++)
	{
		// Check BOOT#### present in BootOrder or not.
		if (BootOrder[Index] == Option)
			break;
	}

	if (Index >= BootOrderSize/sizeof(UINT16)) 
	{   
		//This is not in BOOT#### and skip this variable.
		return TRUE;
	}
	return FALSE;
}

/**
  Converts a File device path structure to its string and obtains the file name.

  @param DevPath         The input device path structure.
                         representation for a device node can be used, where applicable.
  @retval				    file name obtained from the path

**/
CHAR16* GetFileNameFromDevPath( EFI_DEVICE_PATH_PROTOCOL *DevPath)
{
   FILEPATH_DEVICE_PATH  *Fp = NULL;
   UINTN 	i = 0;
   UINTN 	Length = 0; 
   CHAR16	*FilePath = NULL;
   CHAR16	*FileName = NULL;

   if(NULL == DevPath)
      return FileName;
   Fp = (FILEPATH_DEVICE_PATH *)DevPath;
   //Obtain file path
   FilePath = EfiLibAllocateZeroPool((StrLen(Fp->PathName)+1) * sizeof(CHAR16));
   if(NULL == FilePath)
      return FileName;

   StrCpy(FilePath, Fp->PathName);
  
   //Obtain the file name from file path after the last '\'
   for(i=0; FilePath[i]!= L'\0'; i++);
   Length = i;
   for(i--; FilePath[i]!=L'\\'; i--);
   Length = Length-i-1;
   FileName  = EfiLibAllocateZeroPool((Length+1)*sizeof(CHAR16));
   if(NULL == FileName)
   	return FileName;
   StrCpy(FileName,FilePath+i+1); 
   MemFreePointer ((VOID **)&FilePath);
  
   return FileName;
}

/**
    Finds and returns internal data structure BOOT_DATA for a given OS defined boot
	option number for OSRecovery or platform  defined boot option for platform recovery.

    @param Option: Option number for which BOOT_DATA is needed

    @param OsRecovery: To check OSRecovery data or Platform recovery data

    @retval Returns pointer to BOOT_DATA if found. Returns NULL
        if BOOT_DATA not found.

**/
BOOT_DATA *RecoveryGetRecoveryData( UINT16 Option, UINT8 RecoveryBootingType)
{
    UINTN i;
    if(RecoveryBootingType == OS_RECOVERY_OPTION_BOOTING)
    {
		 for ( i = 0; i < gOSRecoveryOptionCount; i++ )
		 {
			  if ( gOSRecoveryData[i].Option == Option )
					return &gOSRecoveryData[i];
		 }
    }
    else if (RecoveryBootingType == PLATFORM_RECOVERY_OPTION_BOOTING)
    {
		 for ( i = 0; i < gPlatformRecoveryOpCount; i++ )
		 {
			  if ( gPlatformRecoveryData[i].Option == Option )
					return &gPlatformRecoveryData[i];
		 }
    }

    return NULL;
}

/**
 * The function checks if boot option is well-formed
 * same as bds\bootoptions.c -> IsBootOptionValid adjusted for the differences
 * between BOOT_OPTION and EFI_LOAD_OPTION structure
 *
 * @param Buffer - Pointer to the BOOT_OPTION structure
 * @param Size of BOOT_OPTION plus the optional data section
 *
 * @retval TRUE boot option is valid, FALSE otherwise
*/
BOOLEAN TSEIsBootOptionValid(BOOT_OPTION* Buffer, UINTN Size)
{
	CHAR16 *Char;
	CHAR16 *EndOfDescription;
	EFI_DEVICE_PATH_PROTOCOL *Dp;
	UINTN DevicePathSize;
	UINTN NvramOptionSize = Size;
	BOOT_OPTION *NvramOption = Buffer;
	
	// The boot option must have at least the header, 
	// an empty (just the NULL-terminator) description,
	// and an empty device path (End-of-Device-Path node).
	if (NvramOption->PathLength<sizeof(EFI_DEVICE_PATH_PROTOCOL)) return FALSE;
	
	if (NvramOptionSize < sizeof(*NvramOption)) return FALSE;
	NvramOptionSize -= sizeof(*NvramOption);
	NvramOptionSize += sizeof(CHAR16);

	if (  NvramOption->PathLength >= NvramOptionSize ) return FALSE;
	NvramOptionSize -= NvramOption->PathLength;

	// The description must include at least the NULL-terminator
	if (NvramOptionSize < sizeof(CHAR16)) return FALSE;

	    // The description must be NULL-terminated
	Char = NvramOption->Name;
	EndOfDescription = (CHAR16*)((CHAR8*)Char+NvramOptionSize);
	while( *Char && Char < EndOfDescription) Char++;
		if (Char==EndOfDescription) return FALSE;

	
	// Validate the device path;
	Dp = (EFI_DEVICE_PATH_PROTOCOL*)(Char+1); // skip the terminating zero.
	if (EFI_ERROR(IsValidDevicePath(Dp))) return FALSE;
	if (BBSValidDevicePath(Dp)) return FALSE; // filter BBS options
	// NvramOption->FilePathListLength can't be zero. 
	    // We checked that at the start of the funciton.
	DevicePathSize = NvramOption->PathLength;
	while (TRUE) {
		UINTN Length = NODE_LENGTH(Dp);
		if (Length>DevicePathSize) return FALSE;
	        // We are not making sure that Length is not equal to zero.
	        // This has already been verified by the IsValidDevicePath above.
		DevicePathSize -= Length;
		if (DevicePathSize < sizeof(EFI_DEVICE_PATH_PROTOCOL)){
	         if (DevicePathSize != 0 ) return FALSE;
	         //The last node must be an End-of-Device-Path node.
	          return isEndNode(Dp) && Dp->SubType == END_ENTIRE_SUBTYPE && Length == sizeof(EFI_DEVICE_PATH_PROTOCOL);
        }
		Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp+Length);
    }
    //Should never reach this point.
    return FALSE;
}

/**
 * The function checks DevicePath having URI Devicepath Or Not 
 * If the device path having URI it will return TRUE else FLASE
 * 
 * @param DevicePath - The input device path structure.
 * 
 * @retval TRUE Device path having URI, FALSE otherwise
*/
BOOLEAN IsURIDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
	EFI_DEVICE_PATH_PROTOCOL *TempDevicePath=NULL;
	
	if(!DevicePath  || EFI_ERROR(IsValidDevicePath(DevicePath)))
		return FALSE;
	
	for(TempDevicePath=DevicePath;!IsDevicePathEndType(TempDevicePath);TempDevicePath = NextDevicePathNode(TempDevicePath))
	{
		if((TempDevicePath->Type == MESSAGING_DEVICE_PATH) && (TempDevicePath->SubType == MSG_URI_DP))
			return TRUE;
	}
	return FALSE;
}

/**
 * The function to return HTTP(S) Full DevicePath, 
 * 
 * @param DevicePath - The input device path structure.
 *
 * @retval DevicePath
*/
EFI_DEVICE_PATH_PROTOCOL* EfiHTTPSGetFullDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
	EFI_STATUS  Status;
	EFI_HANDLE  *Handles=NULL;
	UINTN       HandleCount = 0,Index =0,DPSize=0;
	EFI_DEVICE_PATH_PROTOCOL *FullDevicePath=NULL,*TmpDevicePath=NULL;
	EFI_DEVICE_PATH_PROTOCOL *NewDevicePath=NULL,*OldDevicePath=NULL;

	if(!DevicePath || EFI_ERROR(IsValidDevicePath(DevicePath)) || !IsURIDevicePath(DevicePath))
		return NULL;

	Status = gBS->LocateHandleBuffer(
			ByProtocol,
			&gEfiLoadFileProtocolGuid,
			NULL,
			&HandleCount,
			&Handles
			);
	if(EFI_ERROR(Status))
	{
		return NULL;		
	}
	 
	for(Index = 0;Index < HandleCount;Index++)
	{
		TmpDevicePath = EfiDevicePathFromHandle(Handles[Index]);
		if(!TmpDevicePath)
			continue;
		
		for(NewDevicePath =TmpDevicePath,OldDevicePath = DevicePath;
				!IsDevicePathEndType(NewDevicePath) && !IsDevicePathEndType(OldDevicePath);
				NewDevicePath = NextDevicePathNode(NewDevicePath),OldDevicePath = NextDevicePathNode(OldDevicePath))
		{
			if((NewDevicePath->Type == MESSAGING_DEVICE_PATH)&&(OldDevicePath->Type == MESSAGING_DEVICE_PATH) &&
					(NewDevicePath->SubType == MSG_MAC_ADDR_DP)&&(OldDevicePath->SubType == MSG_MAC_ADDR_DP) &&
					!MemCmp((UINT8 *)&((AMITSE_MAC_ADDR_DEVICE_PATH *)NewDevicePath)->MacAddress,(UINT8 *)&((AMITSE_MAC_ADDR_DEVICE_PATH *)OldDevicePath)->MacAddress,32*sizeof(UINT8))
					)
			{
				for(	;!IsDevicePathEndType(NewDevicePath) && !IsDevicePathEndType(OldDevicePath);
						NewDevicePath = NextDevicePathNode(NewDevicePath),OldDevicePath = NextDevicePathNode(OldDevicePath) )
				{
					if((NewDevicePath->Type != OldDevicePath->Type)||(NewDevicePath->SubType != OldDevicePath->SubType))
					{
						break;
					}
				}
				if((NewDevicePath->Type == OldDevicePath->Type)&&(NewDevicePath->SubType == OldDevicePath->SubType))
				{				
					DPSize = EfiDevicePathSize(TmpDevicePath);
					FullDevicePath = EfiLibAllocateZeroPool(DPSize);
					if(FullDevicePath)
						MemCpy(FullDevicePath,TmpDevicePath,DPSize);
					return FullDevicePath;
				}
			
			}
		}
	}
	return NULL;
}

/**
 * The function to Check whether Devicepath contains MediaFile path, 
 * 
 * @param DevicePath - The input device path structure.
 *
 * @retval BOOLEAN
*/
BOOLEAN IsMediaFileDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
	EFI_DEVICE_PATH_PROTOCOL *TempDevicePath=NULL;
	
	if(!DevicePath  || EFI_ERROR(IsValidDevicePath(DevicePath)))
		return FALSE;
	
	for(TempDevicePath=DevicePath;!IsDevicePathEndType(TempDevicePath);TempDevicePath = NextDevicePathNode(TempDevicePath))
	{
		if((TempDevicePath->Type == MEDIA_DEVICE_PATH) && (TempDevicePath->SubType == MEDIA_FILEPATH_DP))
			return TRUE;
	}
	return FALSE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
