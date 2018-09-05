//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


#include <AmiDxeLib.h>
#include <BootOptions.h>
#include "EfiOsNamesFilePathMaps.h"
#include <../EfiOsBootOptionNames.h>
#include <Guid/FileInfo.h>
#include <Library/DevicePathLib.h>

/** @file EobonPriorityMap.c

   Create/Remove boot options by priority map.
*/

#define MAX_PRIORITY_MAP_ITEMS 2

typedef struct {
    NAME_MAP PriorityMap[MAX_PRIORITY_MAP_ITEMS];
}NAME_PRIORITY_MAP;

/**
    Priority maps
*/
NAME_PRIORITY_MAP FileNamePriorityMaps[] = { EfiOsFilePathPriorityMaps {0} };
UINTN FileNamePriorityMapCount = sizeof(FileNamePriorityMaps) / sizeof(NAME_PRIORITY_MAP) - 1;

typedef BOOLEAN (CREATE_TARGET_EFI_OS_BOOT_OPTION) (EFI_HANDLE* Handle, NAME_MAP* NameMap);
extern CREATE_TARGET_EFI_OS_BOOT_OPTION* CreateTargetEfiOsBootOptionFuncPtr;

extern BOOLEAN StrNoCaseCmp(CHAR16* Str1, CHAR16* Str2);
extern EFI_HANDLE GetGptPartitionHandle(EFI_DEVICE_PATH_PROTOCOL *DevicePath);
extern BOOLEAN IsMatchedBootOptionByNameMap(BOOT_OPTION* Option, NAME_MAP* NameMap);

#if CHECK_FILE_TIME_STAMP
/**
    Check whether leap year.

    @param  Year   Year to check

    @retval  TRUE    Is leap year
    @retval  FALSE   Isn't leap year
**/
BOOLEAN IsLeapYear(UINTN Year)
{
    if(Year % 4 == 0)
    {
        if(Year % 100 == 0)
        {
            if(Year % 400 != 0)
                return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

/**
    Add minutes to Time.

    @param  Time   Time to be added
    @param  Min    Minutes to add

    @retval   EFI_INVALID_PARAMETER   One of parameters is invalid
    @retval   EFI_SUCCESS             Add successfully 
**/
EFI_STATUS AddMinute(EFI_TIME* Time, UINT8 Min)
{
    STATIC UINT8 DaysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(Min > 59 || !Time) return EFI_INVALID_PARAMETER;

    if((Time->Minute += Min) > 60)
    {
        Time->Minute -= 60;

        if(++Time->Hour == 24)
        {
            UINTN DayOffset = 0;

            Time->Hour = 0;

            if(Time->Month == 2 && IsLeapYear(Time->Year))
                DayOffset = 1;

            if(++Time->Day > (DaysPerMonth[Time->Month-1] + DayOffset))
            {
                Time->Day = 1;
                if(++Time->Month > 12)
                {
                    Time->Month = 1;
                    Time->Year++;
                }
            }
        }
    }

    return EFI_SUCCESS;
}

/**
    Time diff from year to minute.

    @param  Time1   Time to diff
    @param  Time2   Time to be diff with
    
    @retval  -1   One of parameters is invalid.
    @retval   0   Time1 is later than Time2.
    @retval   1   Time2 is later than Time2.
**/
UINTN EobonTimeDiffToMinute(EFI_TIME* Time1, EFI_TIME* Time2)
{
    UINTN Ret;

    if(!Time1 || !Time2) return -1;

    Ret = 0;
    if(Time1->Year < Time2->Year)
    {
        Ret = 1;
    }
    else if(Time1->Year == Time2->Year)
    {
        if(Time1->Month < Time2->Month)
        {
            Ret = 1;
        }
        else if(Time1->Month == Time2->Month)
        {
            if(Time1->Day < Time2->Day)
            {
                Ret = 1;
            }
            else if(Time1->Day == Time2->Day)
            {
                if(Time1->Hour < Time2->Hour)
                {
                    Ret = 1;
                }
                else if(Time1->Hour == Time2->Hour)
                {
                    if(Time1->Minute < Time2->Minute)
                    {
                        Ret = 1;
                    }
                }
            }
        }
    }

    return Ret;
}

/**
    Get file info.

    @param  File   Protocol to get file info
    @param  Info   File info to output

    @retval  EFI_INVALID_PARAMETER   One of parameters is invalid
    @retval  EFI_OUT_OF_RESOURCES    Fail to MallocZ
    @retval  EFI_SUCCESS             Get file info successfully
    @retval  Others                  Fail to get file info
**/
EFI_STATUS GetFileInfo(EFI_FILE_PROTOCOL* File, EFI_FILE_INFO** Info)
{
    EFI_STATUS     Status;
    EFI_FILE_INFO* FileInfo = NULL;
    UINTN          FileInfoSize = 0;

    if(!File || !Info) return EFI_INVALID_PARAMETER;

    Status = File->GetInfo (
                    File,
                    &gEfiFileInfoGuid,
                    &FileInfoSize,
                    FileInfo);

    if(Status == EFI_BUFFER_TOO_SMALL)
    {
        FileInfo = (EFI_FILE_INFO*) MallocZ(FileInfoSize);

        if(!FileInfo) return EFI_OUT_OF_RESOURCES;

        Status = File->GetInfo (
                        File,
                        &gEfiFileInfoGuid,
                        &FileInfoSize,
                        FileInfo);

        if(!EFI_ERROR(Status))
            *Info = FileInfo;
    }

    return Status;
}

/**
    Get map index of boot file with last time stamp by priority map.

    @param  File   File to open
    @param  Map    Map to check

    @retval  -1      Can't find any files
    @retval  UINTN   Index to the map
**/
UINTN BootFileWithLastTimeStampByPriorityMap(EFI_FILE_PROTOCOL* File, NAME_PRIORITY_MAP* Map)
{
    UINTN MapIdx = -1;

    if(File && Map)
    {
        UINTN               i;
        EFI_STATUS          Status;
        EFI_FILE_PROTOCOL*  NewFile;
        EFI_FILE_INFO*      Info[MAX_PRIORITY_MAP_ITEMS] = {NULL};

        for(i = 0; i < MAX_PRIORITY_MAP_ITEMS; i++)
        {
            Status = File->Open (
                      File,
                      &NewFile,
                      Map->PriorityMap[i].FilePath,
                      EFI_FILE_MODE_READ,
                      0);

            if(!EFI_ERROR(Status))
            {
                Status = GetFileInfo(NewFile, &Info[i]);

                if(!EFI_ERROR(Status));
                { 
                    if(MapIdx != -1)
                    {
                        if(1 == EobonTimeDiffToMinute(&Info[MapIdx]->CreateTime, &Info[i]->CreateTime))
                        {
                            MapIdx = i;
                            AddMinute(&Info[MapIdx]->CreateTime, 3); // tolerance : 3 Min
                        }
                    }
                    else // MapIdx == -1
                    {
                        MapIdx = i;
                        AddMinute(&Info[MapIdx]->CreateTime, 3); // tolerance : 3 Min
                    }
                }

                NewFile->Close(NewFile);
            }
        }

        for (i = 0; i < MAX_PRIORITY_MAP_ITEMS; i++)
            if(Info[i]) pBS->FreePool(Info[i]);
    }

    TRACE((-1, "MapIdx (%X)\n", MapIdx));
    return MapIdx;
}

/**
    Create boot options by priority map with time stamp.

    @param  Handle   Handle to locate file system

    @retval  UINTN   Number of boot option created
**/
UINTN CreateBootOptionsByPriorityMapWithTimeStamp(EFI_HANDLE Handle, EFI_FILE_PROTOCOL* File)
{
    UINTN i;
    UINTN Count = 0;

    if(!Handle || !File) return Count;

    TRACE((-1, "\nCreateBootOptionsByPriorityMapWithTimeStamp\n"));

    for (i = 0; i < FileNamePriorityMapCount; i++) 
    {
        UINTN MapIdx;

        MapIdx = BootFileWithLastTimeStampByPriorityMap(File, &FileNamePriorityMaps[i]);
        if(-1 == MapIdx)
            continue;

        if(CreateTargetEfiOsBootOptionFuncPtr(Handle, &(FileNamePriorityMaps[i].PriorityMap[MapIdx])))
        {
            Count++;
            break;
        }
    }

    return Count;
}
#endif //CHECK_FILE_TIME_STAMP

/**
    Create boot options by priority map.

    @param  Handle   Handle to locate file system

    @retval  UINTN   Number of boot option created
**/
UINTN CreateBootOptionsByPriorityMap(EFI_HANDLE Handle, EFI_FILE_PROTOCOL* File)
{
    EFI_STATUS Status;
    UINTN i, j;
    UINTN Count = 0;

    if(!Handle || !File) return Count;

    TRACE((-1, "\nCreateBootOptionsByPriorityMap\n"));

    for (i = 0; i < FileNamePriorityMapCount; i++) 
    {
        EFI_FILE_PROTOCOL* NewFile;

        for (j = 0; j < MAX_PRIORITY_MAP_ITEMS; j++)
        {
            Status = File->Open (
                         File,
                         &NewFile,
                         FileNamePriorityMaps[i].PriorityMap[j].FilePath,
                         EFI_FILE_MODE_READ,
                         0
                     );

            TRACE((-1, "Open(%S): %r\n", FileNamePriorityMaps[i].PriorityMap[j].FilePath, Status));

            if(EFI_ERROR(Status)) continue;
            NewFile->Close(NewFile);

            if(CreateTargetEfiOsBootOptionFuncPtr(Handle, &(FileNamePriorityMaps[i].PriorityMap[j])))
            {
                Count++;
                break;
            }
        }
    }

    return Count;
}

/**
    Remove boot option by priority priority map 

    @param  Options       Boot options to check
    @param  OptionCount   Number of boot options
**/
VOID RemoveBootOptionByPriorityMap(SIMPLE_BOOT_OPTION *Options, UINTN OptionCount)
{
    if(Options && (OptionCount >= 2))
    {
        SIMPLE_BOOT_OPTION *Option1 = &Options[0];
        UINTN j, k;

        if(!Option1->Fw && !Option1->Invalid)
        {
            EFI_DEVICE_PATH_PROTOCOL* LastDp1 = DPGetLastNode(Option1->Dp);

            if(LastDp1->Type == MEDIA_DEVICE_PATH
                    && LastDp1->SubType == MEDIA_FILEPATH_DP)
            {
                //
                //  Case : If match Secure-Boot boot option then try to kill the non-secure-boot one
                //
                for(j = 0; j < FileNamePriorityMapCount; j++)
                {
                    NAME_MAP*  Map  = &FileNamePriorityMaps[j].PriorityMap[0];

                    //
                    //  File path of Secure-Boot boot option?
                    //
                    if(StrNoCaseCmp((CHAR16*)(LastDp1+1), Map->FilePath))
                    {
                        Map++;
                        for(k = 1; k < OptionCount; k++)
                        {
                            SIMPLE_BOOT_OPTION *Option2 = &Options[k];

#if !CHECK_FILE_TIME_STAMP

                            if(Option2->Fw)
                                continue;
#endif
                            if(!Option2->Invalid)
                            {
                                //
                                //  Same partition?
                                //
                                if(!MemCmp((VOID*)Option1->Dp, (VOID*)Option2->Dp, sizeof(HARDDRIVE_DEVICE_PATH)))
                                {
                                    EFI_DEVICE_PATH_PROTOCOL* LastDp2 = DPGetLastNode(Option2->Dp);

                                    //
                                    //  Boot Option of Non-Secure-Boot?
                                    //
                                    if(StrNoCaseCmp((CHAR16*)(LastDp2+1), Map->FilePath))
                                    {
                                        CHAR16  BootStr[9];
                                        Swprintf(BootStr,L"Boot%04X",Option2->Num);

                                        pRS->SetVariable(
                                            BootStr, &EfiVariableGuid,
                                            BOOT_VARIABLE_ATTRIBUTES, 0, NULL);

                                        Option2->Invalid = TRUE;

                                        TRACE((-1, "BootOptionNumber(%X) Deleted\n", Options[k].Num));
                                        return;
                                    }
                                }
                            }
                        } 
                        return;
                    }
                }

#if !CHECK_FILE_TIME_STAMP
                //
                //  Case : If Non-Secure-Boot boot option then try to modify it if shim.efi found
                //
                for(j = 0; j < FileNamePriorityMapCount; j++)
                {
                    NAME_MAP*  Map  = &FileNamePriorityMaps[j].PriorityMap[0];

                    //
                    //  File path of Non-Secure-Boot boot option?
                    //
                    if(StrNoCaseCmp((CHAR16*)(LastDp1+1), Map[1].FilePath))
                    {
                        EFI_HANDLE Handle = GetGptPartitionHandle(Option1->Dp);
                        if(Handle)
                        {
                            EFI_STATUS Status;
                            EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFileSystem;

                            Status = pBS->HandleProtocol (
                                             Handle,
                                             &gEfiSimpleFileSystemProtocolGuid,
                                             &SimpleFileSystem);

                            if(!EFI_ERROR(Status))
                            {
                                EFI_FILE_PROTOCOL* File;

                                Status = SimpleFileSystem->OpenVolume (
                                            SimpleFileSystem,
                                            &File);

                                if(!EFI_ERROR(Status))
                                {
                                    EFI_FILE_PROTOCOL* NewFile;

                                    Status = File->Open (
                                              File,
                                              &NewFile,
                                              Map->FilePath,
                                              EFI_FILE_MODE_READ,
                                              0);

                                    if(!EFI_ERROR(Status))
                                    {
                                        CHAR16* OldDescription   = (CHAR16*)(Option1->NvramOption+1);
                                        UINTN   OldDescSize      = StrSize(OldDescription); 
                                        UINTN   NewDescSize      = StrSize(Map->BootOptionName);

                                        UINTN NewFilePathSize    = StrSize(Map->FilePath);
                                        UINTN NewFilePathNodeLen = sizeof(EFI_DEVICE_PATH_PROTOCOL) + NewFilePathSize;
                                        UINTN NewFilePathListLen = Option1->NvramOption->FilePathListLength 
                                                                    - NODE_LENGTH(LastDp1) + NewFilePathNodeLen;

                                        UINTN NewOptionSize = (Option1->Size -  NODE_LENGTH(LastDp1) - OldDescSize)
                                                                + NewFilePathNodeLen + NewDescSize;  

                                        EFI_LOAD_OPTION* NewOption = (EFI_LOAD_OPTION*) MallocZ(NewOptionSize);
                                        if(NewOption)
                                        {
                                            EFI_LOAD_OPTION*           OptionWalker = NewOption;
                                            EFI_DEVICE_PATH_PROTOCOL*  NewLastDp;
                                            VOID*                      Remaining;
                                            UINTN                      RemainingSize;
                                            CHAR16                     BootStr[9];

                                            //
                                            //  Copy EFI_LOAD_OPTION
                                            //
                                            OptionWalker->Attributes = Option1->NvramOption->Attributes;
                                            OptionWalker->FilePathListLength = (UINT16)NewFilePathListLen;
                                            OptionWalker = (EFI_LOAD_OPTION*)((UINT8*)OptionWalker + sizeof(EFI_LOAD_OPTION));
                                            //
                                            //  Copy New Description
                                            //
                                            MemCpy((VOID*)OptionWalker, (VOID*)Map->BootOptionName, NewDescSize);
                                            OptionWalker = (EFI_LOAD_OPTION*)((UINT8*)OptionWalker + NewDescSize);
                                            //
                                            //  Copy FilePathList
                                            //
                                            MemCpy((VOID*)OptionWalker, (VOID*)Option1->Dp, DPLength(Option1->Dp));
                                            NewLastDp = DPGetLastNode((EFI_DEVICE_PATH_PROTOCOL*)OptionWalker);
                                            SetDevicePathNodeLength(NewLastDp, NewFilePathNodeLen);
                                            MemCpy((VOID*)(NewLastDp+1), (VOID*)Map->FilePath, NewFilePathSize);
                                            OptionWalker = (EFI_LOAD_OPTION*)((UINT8*)NewLastDp + NewFilePathNodeLen);
                                            //
                                            //  Copy Remaining
                                            //
                                            Remaining = (VOID*)((UINT8*)LastDp1 + NODE_LENGTH(LastDp1));
                                            RemainingSize = Option1->Size - ((UINTN)Remaining - (UINTN)Option1->NvramOption);
                                            MemCpy((VOID*)OptionWalker, Remaining, RemainingSize); 

                                            //
                                            // Change SimpleBootOption
                                            //
                                            pBS->FreePool(Option1->NvramOption);
                                            Option1->NvramOption = NewOption;
                                            Option1->Desc = (CHAR16*)(NewOption+1);
                                            Option1->Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Option1->Desc + NewDescSize);

                                            Swprintf(BootStr,L"Boot%04X",Option1->Num);
                                            pRS->SetVariable(
                                                BootStr, &EfiVariableGuid,
                                                BOOT_VARIABLE_ATTRIBUTES, NewOptionSize, (VOID*)NewOption);
                                        }
                                        NewFile->Close(NewFile);
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }// for
#endif

            }
        }
    }
}

/**
    Check whether uefi os boot option by priority map (FileNamePriorityMaps).

    @param  Option   Boot option to check

    @retval  TRUE   Match the priority map
    @retval  FALSE  Doesn't match the priority map
**/
BOOLEAN CheckUefiOsBootOptionByPriorityMap(BOOT_OPTION* Option)
{
    if(Option)
    {
        UINTN i;
        for(i = 0; i < FileNamePriorityMapCount; i++)
        {
            UINTN j;
            for(j = 0; j < MAX_PRIORITY_MAP_ITEMS; j++)
            {
                if(IsMatchedBootOptionByNameMap(Option, &FileNamePriorityMaps[i].PriorityMap[j]))
                        return TRUE;
            }
        }
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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
