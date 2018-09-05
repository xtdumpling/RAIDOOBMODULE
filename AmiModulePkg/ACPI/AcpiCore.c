//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file AcpiCore.c
    Main ACPI Driver File. It has ACPI Driver entry point,
    ACPISupport Protocol and ACPITable Protocol.

**/
//**********************************************************************

#include <AmiDxeLib.h>
#include <AcpiRes.h>
#include "AcpiCore.h"
#include <AcpiOemElinks.h>


//--------------------------------------
//Some Global vars
extern UINTN 			gDsdtPages;
ACPI_DB                 gAcpiData = {0, 0, NULL, 0};
RSDT_PTR_20             *gRsdtPtrStr = NULL;
UINT8                   gForceAcpi1 = 0, gPublishedOnReadyToBoot = 0;

#define ACPI_TABLES_FILE_GUID \
		{ 0x709e6472, 0x1bcd, 0x43bd, { 0x8b, 0x6b, 0xcd, 0x2d, 0x6d, 0x8, 0xb9, 0x67 } }

typedef EFI_STATUS (ACPI_OEM_FUNCTION)(IN OUT ACPI_HDR *AcpiHdr);
extern ACPI_OEM_FUNCTION OEM_LIST EndOfOemList;
ACPI_OEM_FUNCTION* AcpiOemPartsList[] = {OEM_LIST NULL};

EFI_STATUS MpsTableBuilderInit(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable);

//------------Move to header------------------------
EFI_STATUS BuildMandatoryAcpiTbls ();
EFI_STATUS AcpiReadyToBootEvent();
EFI_STATUS UpdateFacp ();
EFI_STATUS PublishTbl (IN UINT8 RsdtBuild, IN UINT8 XsdtBuild);


//------------Move to header------------------------
/**
    This function provides platform specific OEM_ID and OEM_TABLE_ID to
    overwrite default ACPI Table header.

         
    @param AcpiHdr ACPI TABLE header

        @return EFI_STATUS  
		@retval EFI_SUCCESS if Values overwtitten.
        @retval EFI_INSUPPORTED if no need to change values - use default.

    @note  UINT8 *AcpiOemId[6]; UINT8 *AcpiOemTblId[8] avoid buffer overrun!
**/

EFI_STATUS OemAcpiSetPlatformId(IN OUT ACPI_HDR *AcpiHdr)
{
    EFI_STATUS       Status=EFI_UNSUPPORTED;
    UINTN i;

    for (i=0; AcpiOemPartsList[i]; i++) Status = AcpiOemPartsList[i](AcpiHdr);

    return Status;
}


/**
    Finds ACPI table by Handle and returns its entry number in
    gAcpiData structure

    @param Handle - Handle (pointer to ACPI table header)


    @retval Entry number in gAcpiData structure

**/
UINTN FindAcpiTblByHandle (UINTN *Handle)
{
    UINTN i;
    
    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
        if (*Handle == (UINTN)gAcpiData.AcpiEntries[i]->BtHeaderPtr)   // Handle is the address of ACPI table
        {
            return i;
        }
    }
    
    return ACPI_TABLE_NOT_FOUND;
}// end of FindAcpiTblByHandle

/**
    Founds ACPI Table with defined Signature (Sig) and Version in gAcpiData structure.


    @param TblPtr pointer to the ACPI entrie in gAcpiData,
        modifided by this function
    @param Sig Signature of a table to search for
        IN EFI_ACPI_TABLE_VERSION - Version of a table to be found

    @retval EFI_SUCCESS table with corresponding signature was found
    @retval EFI_NOT_FOUND otherwise

**/
EFI_STATUS GetBtTable (OUT ACPI_TBL_ITEM **TblPtr, IN UINT32 Sig, IN EFI_ACPI_TABLE_VERSION Versiov)
{
    UINTN       i;
    
    for (i = 0; i < gAcpiData.AcpiEntCount; i++)
    {
        if ((gAcpiData.AcpiEntries[i]->BtHeaderPtr->Signature == Sig)
                && (gAcpiData.AcpiEntries[i]->AcpiTblVer & Versiov))
        {
            *TblPtr = gAcpiData.AcpiEntries[i];
            return EFI_SUCCESS;
        }
    }
    
    return EFI_NOT_FOUND;
}// end of GetBtTable

/**
    Checks if Notify function(s) present dispatches it.


    @param TableHeaderPtr - pointer to the ACPI Table header,
        modifided by this function
    @retval EFI_STATAUS From the function dispatched.

**/
EFI_STATUS DispatchSdtNotify(ACPI_TBL_ITEM* TableItemPtr)
{
    EFI_STATUS  Status=EFI_SUCCESS;
    UINTN   i;
//--------
    for(i=0;i<gAcpiData.NotifyFnCount; i++){
        Status=gAcpiData.AcpiNotifyFn[i]((EFI_ACPI_SDT_HEADER*)TableItemPtr->BtHeaderPtr,TableItemPtr->AcpiTblVer,(UINTN)TableItemPtr);        
        
        ASSERT_EFI_ERROR(Status);        
    }    

    return Status;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ACPI SUPPORT PPROTOCOL function Implementation
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**
    This function returns ACPI table

         
    @param This pointer to EFI_ACPI_SUPPORT_PROTOCOL instance
    @param Index Index of ACPI table to return
    @param Table Pointer where to place found table
    @param Version requested ACPI table version
    @param Handle requested ACPI table handle

          
    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_OUT_OF_RESOURCES not enough memory to allocate table
    @retval EFI_INVALID_PARAMETER invalid EFI_ACPI_SUPPORT_PROTOCOL pointer
    @retval EFI_NOT_FOUND requested ACPI table not found

**/

EFI_STATUS EFIAPI AcpiSupportGetAcpiTable (
    IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
    IN INTN                                 Index,
    OUT VOID                                **Table,
    OUT EFI_ACPI_TABLE_VERSION              *Version,
    OUT UINTN                               *Handle )
{
    ACPI_HDR        *HdrPtr;
    VOID            *Ptr;
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
//    UINTN           i;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    
//----------------------

    if (This!=&gAcpiData.AcpiSupportProtocol) return EFI_INVALID_PARAMETER;
    
    if (Index > ((INTN)gAcpiData.AcpiEntCount - 1)) return EFI_NOT_FOUND;
    
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
//    for (i = Index; i < (UINTN) gAcpiData.AcpiEntCount; i++)
//    {
    HdrPtr = gAcpiData.AcpiEntries[Index]->BtHeaderPtr;
    Ptr = Malloc(HdrPtr->Length);
    ASSERT(Ptr);
            
    if (!Ptr) return EFI_OUT_OF_RESOURCES;
            
    *Version = gAcpiData.AcpiEntries[Index]->AcpiTblVer;
    *Handle = (UINTN) HdrPtr;
    pBS->CopyMem(Ptr, HdrPtr, HdrPtr->Length);
    *Table = Ptr;
    return EFI_SUCCESS;
//    }
    
//    return EFI_NOT_FOUND;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
}//end of AcpiSupportGetAcpiTable

/**
    This function allows to add, remove of modify ACPI table

         
    @param This pointer to EFI_ACPI_SUPPORT_PROTOCOL instance
    @param Table Pointer to update data. If NULL, corresponded table
        should be removed
    @param Checksum if TRUE, function will recalculate checksum before adding table
    @param Version requested ACPI table version
    @param Handle requested ACPI table handle

          
    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_OUT_OF_RESOURCES not enough memory to perform operation
    @retval EFI_INVALID_PARAMETER invalid EFI_ACPI_SUPPORT_PROTOCOL pointer or ACPI table
        content
    @retval EFI_ABORTED provided ACPI table already present

**/

EFI_STATUS EFIAPI AcpiSupportSetAcpiTable(
    IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
    IN CONST VOID                           *Table    OPTIONAL,
    IN BOOLEAN                              Checksum,
    IN EFI_ACPI_TABLE_VERSION               Version,
    IN OUT UINTN                            *Handle  )
{
    ACPI_TBL_ITEM   *AcpiTableAdded = NULL, *TblDummy = NULL;
    ACPI_HDR        *Hdr1 = NULL, *Hdr2 = NULL, *Dsdt = NULL, *XtDsdt = NULL;
    VOID            *Ptr = NULL;
    EFI_ACPI_TABLE_VERSION XtDsdtVer = 0,  DsdtVer = 0, MultyVer = 0;
    EFI_STATUS      Status = 0;
    UINTN           TblNum = ACPI_TABLE_NOT_FOUND, DsdtPages;
    BOOLEAN         CorrectFacp = FALSE, WasChecksummed = FALSE;

    
    //Handle == NULL   Table != NULL add the table
    //Handle != NULL   Table != NULL replace the table
    //Handle != NULL   Table == NULL remove the table
//-----------------------------------------------------------------------------------
    if ((This != &gAcpiData.AcpiSupportProtocol) || (Handle==NULL) || (*Handle == 0 && Table == NULL ))  //---------------------------------------------------------------
    {
        Status = EFI_INVALID_PARAMETER;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    DsdtPages = gDsdtPages;
    TRACE((-1, "ACPI: SetAcpiTable() Table=0x%X; Handle=0x%X; *Handle=0x%X\n", Table, Handle, *Handle));
    if (Table != NULL)
    {
        Hdr1 = (ACPI_HDR*) Table;
        
        //Check is we are getting a "special" table that needs a specific care
        if (Hdr1->Signature == FACS_SIG) 
        {
            return EFI_INVALID_PARAMETER;
        }
        if (Hdr1->Signature == FACP_SIG)
        {
            CorrectFacp = TRUE;
            if (Hdr1->Revision <= ACPI_REV2)
            {
                Version = EFI_ACPI_TABLE_VERSION_1_0B;
                TRACE((-1, "Setting FACP Version to 1_B\n"));
            }
        }

        
        if (Hdr1->Signature == DSDT_SIG)
        {
            CorrectFacp = TRUE;
            if (Hdr1->Revision < ACPI_REV2)
            {
                Version = EFI_ACPI_TABLE_VERSION_1_0B;
                TRACE((-1, "Setting DSDT Version to 1_B\n"));
            }
        }
        
        if (Version == EFI_ACPI_TABLE_VERSION_NONE)
            Status = pBS->AllocatePool(EfiACPIMemoryNVS, Hdr1->Length, &Ptr);
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
        	ASSERT_EFI_ERROR(Status);
            if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        else
        {
        	if ((Hdr1->Signature == DSDT_SIG) && (gDsdtPages != (UINTN)-1))
        	{
        		EFI_PHYSICAL_ADDRESS Memory = 0x00000000ffffffff;      
        		
        		Status = pBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData, 
        									EFI_SIZE_TO_PAGES(Hdr1->Length), &Memory);
        		ASSERT_EFI_ERROR(Status);
        		if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;
        		
        		gDsdtPages = EFI_SIZE_TO_PAGES(Hdr1->Length);  
        		Ptr = (VOID*)((UINTN)Memory);

        	}
        	else Ptr = Malloc(Hdr1->Length);
        }
            
        ASSERT(Ptr);
        
        if (Ptr==NULL) 
        {
            return EFI_OUT_OF_RESOURCES;
        }
        
        pBS->CopyMem(Ptr, Hdr1, Hdr1->Length);
        AcpiTableAdded = MallocZ (sizeof (ACPI_TBL_ITEM));
        ASSERT(AcpiTableAdded);
        
        if (!AcpiTableAdded) 
        {
            return EFI_OUT_OF_RESOURCES;
        }
        
        AcpiTableAdded->AcpiTblVer = Version;
        AcpiTableAdded->BtHeaderPtr = (ACPI_HDR*) Ptr;
        if (!Checksum)
            if (!ChsumTbl((UINT8*)Ptr, ((ACPI_HDR*) Ptr)->Length))
                WasChecksummed = TRUE;
        //If table was checksumed and Checksum parameter of SetTable function was not set 
        //to TRUE in next string OemAcpiSetPlatformId may modify the table - let's
        //remember was it checksumed or not

        if (EFI_ERROR(OemAcpiSetPlatformId ((ACPI_HDR*) Ptr))) WasChecksummed = FALSE;
        //If OemAcpiSetPlatformId did not modifies table - reset WasChecksummed to FALSE

        Status = DispatchSdtNotify(AcpiTableAdded);
        ASSERT_EFI_ERROR(Status);

        Status = AppendItemLst ((T_ITEM_LIST*)&gAcpiData, (VOID*) AcpiTableAdded);
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) 
        {
            return EFI_OUT_OF_RESOURCES;
        }
        

        if (Version != EFI_ACPI_TABLE_VERSION_NONE)
            gAcpiData.AcpiLength += ((ACPI_HDR*) Ptr)->Length;
            
        if (Checksum || WasChecksummed)
        {
            ((ACPI_HDR*) Ptr)->Checksum = 0;
            ((ACPI_HDR*) Ptr)->Checksum = ChsumTbl((UINT8*)Ptr, ((ACPI_HDR*) Ptr)->Length);
        }
    }
    
    if (*Handle)
    {
        Status=EFI_SUCCESS;
        
        TRACE((-1, "ACPI: SetAcpiTable() Hnd!=0, Removing Tbl. Ver=0x%X, ",Version));
        
        Hdr2 = (ACPI_HDR*)(*Handle);
        
        //Check is we are getting a "special" table that needs a specific care
        if (Hdr2->Signature == FACS_SIG) 
        {
            return EFI_INVALID_PARAMETER;
        }
        
        TblNum = FindAcpiTblByHandle (Handle);
        TRACE((-1, "TblNum=0x%X ", TblNum));
        
        if (TblNum == ACPI_TABLE_NOT_FOUND) Status=EFI_INVALID_PARAMETER;
        
        TRACE((-1,"Status = %r\n", Status));
        
        // Table with this Handle does not exist
        if (EFI_ERROR(Status))
        {
            return Status;
        }
        

        if (Version != EFI_ACPI_TABLE_VERSION_NONE)
            gAcpiData.AcpiLength -= gAcpiData.AcpiEntries[TblNum]->BtHeaderPtr->Length;
            
        Status = DeleteItemLst((T_ITEM_LIST*) &gAcpiData, TblNum, TRUE);
        TRACE((-1,"ACPI:  Deleting Table From Storage: Status = %r\n", Status));
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR (Status)) 
        {
            return EFI_OUT_OF_RESOURCES;
        }
        if ((Hdr2->Signature == DSDT_SIG) && (gDsdtPages != (UINTN)-1))
        {
        	TRACE((-1,"ACPI: !!! Deleting DSDT !!! \n"));
        	pBS->FreePages((EFI_PHYSICAL_ADDRESS)(UINTN)Hdr2, DsdtPages);
        	gDsdtPages = 0;
        }
        else
        	pBS->FreePool(Hdr2);
    }
    
    //Update Handle with New Table Instance.
    *Handle=(UINTN)Ptr;
    
    if (CorrectFacp)
    {
        Status = UpdateFacp ();
        
        TRACE((-1,"UpdateFacp Status = %r\n", Status));
    }
    
    TRACE((-1,"ACPI: SetAcpiTable() Exiting... Status = %r\n", Status));

    if (gPublishedOnReadyToBoot)
    {
        if (!gForceAcpi1) Status = PublishTbl (1, 1);
        else Status = PublishTbl (1, 0);
        TRACE((-1,"ACPI: PublishTables in SetAcpiTable() Status = %r\n", Status));
    }

    ASSERT_EFI_ERROR(Status);

    return Status;
//--- !!!!!!!!!!!!!!!!!!!!!!!!!! Version none Done ???? !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}// end of AcpiSupportSetAcpiTable


/**
    Causes one or more versions of the ACPI tables to be published in
    the EFI system configuration tables.

         
    @param This pointer to EFI_ACPI_SUPPORT_PROTOCOL instance
    @param Version ACPI table version

          
    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_ABORTED invalid EFI_ACPI_SUPPORT_PROTOCOL pointer or
        an error occurred and the function could not complete successfully.
    @retval EFI_UNSUPPORTED passed ACPI table version invalid

**/

EFI_STATUS EFIAPI AcpiSupportPublishTables(
    IN EFI_ACPI_SUPPORT_PROTOCOL            *This,
    IN EFI_ACPI_TABLE_VERSION               Version )
{
    EFI_STATUS Status;
    
    if ((Version < EFI_ACPI_TABLE_VERSION_1_0B) || (Version > EFI_ACPI_TABLE_VERSION_ALL) || (This!=&gAcpiData.AcpiSupportProtocol))
        return EFI_UNSUPPORTED;
    if (Version == EFI_ACPI_TABLE_VERSION_1_0B)
    {
        if ((gRsdtPtrStr != NULL) && (gRsdtPtrStr->XsdtAddr != 0)) Status = PublishTbl (1, 1);
        else Status = PublishTbl (1, 0);
    }
    
    if ((Version > EFI_ACPI_TABLE_VERSION_1_0B) && (!gForceAcpi1))
    {
        if ((gRsdtPtrStr != NULL) && (gRsdtPtrStr->RsdtAddr != 0)) Status = PublishTbl (1, 1);
        else Status = PublishTbl (0, 1);
    }
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    else return EFI_UNSUPPORTED;
    
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    ASSERT_EFI_ERROR (Status);
    
    if (EFI_ERROR (Status)) return EFI_ABORTED;
    else return EFI_SUCCESS;
}
/**
    Installs an ACPI table into the RSDT/XSDT.

         
    @param This A pointer to a EFI_ACPI_TABLE_PROTOCOL.
    @param AcpiTableBuffer A pointer to a buffer containing the ACPI table to be installed.
    @param AcpiTableBufferSize Specifies the size, in bytes, of the AcpiTableBuffer buffer.
    @param TableKey Returns a key to refer to the ACPI table.

          
    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_INVALID_PARAMETER Either AcpiTableBuffer is NULL, TableKey is NULL, or
        AcpiTableBufferSize and the size field embedded in the ACPI
        table pointed to by AcpiTableBuffer are not in sync
    @retval EFI_OUT_OF_RESOURCES Insufficient resources exist to complete the request.

**/
EFI_STATUS EFIAPI AcpiInstallAcpiTable(
    IN CONST EFI_ACPI_TABLE_PROTOCOL *This,
    IN CONST VOID *AcpiTableBuffer,
    IN UINTN AcpiTableBufferSize,
    OUT UINTN *TableKey)
{
    EFI_STATUS Status;
    UINTN IntTableKey = 0;
    
    
    if ((AcpiTableBuffer == NULL) || (AcpiTableBufferSize != (UINTN)((ACPI_HDR*)AcpiTableBuffer)->Length)
            || (TableKey == NULL)) return EFI_INVALID_PARAMETER;
            
    Status = AcpiSupportSetAcpiTable(&gAcpiData.AcpiSupportProtocol,
                                     (VOID*)AcpiTableBuffer,
                                     TRUE,
                                     // This is the mark, that means, that this table was added by the new protocol
                                     ACPI_TABLE_NOT_REMOVABLE,
                                     &IntTableKey);
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) 
    {
    	if (Status == EFI_ACCESS_DENIED)
    		return Status;
    	else 
    		return EFI_OUT_OF_RESOURCES;
    }
    *TableKey = IntTableKey;
    
    if (!gPublishedOnReadyToBoot)
    {
    	if (!gForceAcpi1) Status = PublishTbl (1, 1);
    	else PublishTbl (1, 0);
    }
    return Status;
}// end of AcpiInstallAcpiTable


/**
    Function allows a caller to remove an ACPI table.

         
    @param This A pointer to a EFI_ACPI_TABLE_PROTOCOL.
    @param TableKey Specifies the table to uninstall. The key was returned from
        InstallAcpiTable().

          
    @retval EFI_SUCCESS Function executed successfully
    @retval EFI_NOT_FOUND TableKey does not refer to a valid key for a table entry.
    @retval EFI_OUT_OF_RESOURCES Insufficient resources exist to complete the request.

**/

EFI_STATUS EFIAPI AcpiUninstallAcpiTable(
    IN CONST EFI_ACPI_TABLE_PROTOCOL *This,
    IN UINTN TableKey)
{
    EFI_STATUS Status;
    
    Status = AcpiSupportSetAcpiTable(&gAcpiData.AcpiSupportProtocol,
                                     NULL,
                                     FALSE,
                                     // This is the mark, that means, that this table was added by the new protocol
                                     ACPI_TABLE_NOT_REMOVABLE,
                                     &TableKey);
    if (EFI_ERROR(Status))
        return (EFI_NOT_FOUND);
    else
        return Status;
}//end of AcpiUninstallAcpiTable


//
// ACPI SDT Protocol functions implementation. 
//

/**
    Returns a requested ACPI table.
    The GetAcpiTable() function returns a pointer to a buffer containing the ACPI table associated
    with the Index that was input. The following structures are not considered elements in the list of
    ACPI tables:
    - Root System Description Pointer (RSD_PTR)
    - Root System Description Table (RSDT)
    - Extended System Description Table (XSDT)
    Version is updated with a bit map containing all the versions of ACPI of which the table is a
    member.
  
    
    @param Index The zero-based index of the table to retrieve.
    @param Table Pointer for returning the table buffer.
    @paramVersion     On return, updated with the ACPI versions to which this table belongs. Type
        EFI_ACPI_TABLE_VERSION is defined in "Related Definitions" in the
        EFI_ACPI_SDT_PROTOCOL.    
    @param TableKey    On return, points to the table key for the specified ACPI system definition table. This
        is identical to the table key used in the EFI_ACPI_TABLE_PROTOCOL.  
     
    @retval EFI_SUCCESS       The function completed successfully.
    @retval EFI_NOT_FOUND     The requested index is too large and a table was not found.                                  
**/
EFI_STATUS EFIAPI GetAcpiTable2 (
  IN  UINTN                               Index,
  OUT EFI_ACPI_SDT_HEADER                 **Table,
  OUT EFI_ACPI_TABLE_VERSION              *Version,
  OUT UINTN                               *TableKey)
{

    return AcpiSupportGetAcpiTable (&gAcpiData.AcpiSupportProtocol, Index,
                                (VOID **)Table, Version,TableKey);

}

EFI_STATUS EFIAPI RegisterNotify(
  IN BOOLEAN                    Register,
  IN EFI_ACPI_NOTIFICATION_FN   Notification)
{
    if(Notification == NULL) return EFI_INVALID_PARAMETER;

    if(Register){
        return AppendItemLst((T_ITEM_LIST*)&gAcpiData.NotifyInitCount, Notification);
    } else {
        UINTN   i;
    //-----------------
        for(i=0; i<gAcpiData.NotifyFnCount; i++){
            if(gAcpiData.AcpiNotifyFn[i]==Notification){
                return DeleteItemLst((T_ITEM_LIST*)&gAcpiData.NotifyInitCount,i,FALSE);
            }
        }  
        //can't found matching notify function.
        return EFI_INVALID_PARAMETER; 
    }
}

/**
  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param  Instance      Return pointer to the first instance of the protocol

  @return EFI_SUCCESS           The function completed successfully.
  @return EFI_NOT_FOUND         The protocol could not be located.
  @return EFI_OUT_OF_RESOURCES  There are not enough resources to find the protocol.

**/
EFI_STATUS FoundFvWithAcpiTables (OUT EFI_FIRMWARE_VOLUME2_PROTOCOL **Instance)
{
	EFI_STATUS                    Status;
	EFI_HANDLE                    *HandleBuffer;
	UINTN                         NumberOfHandles;
	EFI_FV_FILETYPE               FileType;
	UINT32                        FvStatus = 0;
	EFI_FV_FILE_ATTRIBUTES        Attributes;
	UINTN                         Size;
	UINTN                         Index;
	EFI_FIRMWARE_VOLUME2_PROTOCOL *FvInstance;
	EFI_GUID AcpiTblsFileGuid = ACPI_TABLES_FILE_GUID; // 709E6472-1BCD-43BD-8B6B-CD2D6D08B967

	// Locate protocol.

	Status = pBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiFirmwareVolume2ProtocolGuid,
                   NULL,
                   &NumberOfHandles,
                   &HandleBuffer
                   );
	if (EFI_ERROR (Status)) return Status;


  // Find FV with ACPI tables


	for (Index = 0; Index < NumberOfHandles; Index++) 
	{

		// Get the protocol on each handle

		Status = pBS->HandleProtocol (
                     HandleBuffer[Index],
                     &gEfiFirmwareVolume2ProtocolGuid,
                     (VOID**) &FvInstance
                     );
		ASSERT_EFI_ERROR (Status);
		// See if it has the ACPI storage file
		Status = FvInstance->ReadFile (
                           FvInstance,
                           &AcpiTblsFileGuid,
                           NULL,
                           &Size,
                           &FileType,
                           &Attributes,
                           &FvStatus
                           );

		if (Status == EFI_SUCCESS) 
		{
			*Instance = FvInstance;
			break;
		}
	}

	pBS->FreePool (HandleBuffer);

	return Status;
}

/**
  Submits Acpi tables from dedicated FFS file.


  @return EFI_SUCCESS
  @return EFI_LOAD_ERROR
  @return EFI_OUT_OF_RESOURCES

**/
VOID SubmitAcpiTablesFromFile ()
{
	EFI_STATUS                     Status;
	EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol;
	UINTN                          Instance;
	EFI_ACPI_COMMON_HEADER         *CurrentTable;
	UINTN                          TableHandle;
	UINT32                         FvStatus;
	UINTN                          TableSize;
	UINTN                          Size;
	EFI_GUID AcpiTblsFileGuid = ACPI_TABLES_FILE_GUID; // 709E6472-1BCD-43BD-8B6B-CD2D6D08B967

	Instance     = 0;
	CurrentTable = NULL;
	TableHandle  = 0;

	// Locate the firmware volume protocol

	Status = FoundFvWithAcpiTables (&FwVol);
	if (EFI_ERROR (Status)) 
	{	
		TRACE((-1,"ACPI: FoundFvWithAcpiTables Fails with a Status = %r\n", Status));
		return;
	}

	// Read tables from the storage file.

	while (Status == EFI_SUCCESS) 
	{

		Status = FwVol->ReadSection (
                      FwVol,
                      &AcpiTblsFileGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID**) &CurrentTable,
                      &Size,
                      &FvStatus
                      );
		if (!EFI_ERROR(Status)) 
		{

			// Check the table

			TableHandle = 0;

			TableSize = ((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Length;
			ASSERT (Size >= TableSize);	// Section size could be bigger than actual table size due to alignment, 
							//but if it is smaller - table is defective


			// Submit ACPI table

			Status = AcpiSupportSetAcpiTable (
							&gAcpiData.AcpiSupportProtocol,
                            CurrentTable,
                            TRUE,
                            ACPI_TABLE_NOT_REMOVABLE,
                            &TableHandle
                            );


			// Free memory allocated by ReadSection

			pBS->FreePool (CurrentTable);

			if (EFI_ERROR(Status)) return;

			Instance++;
			TRACE((-1, "SubmitAcpiTablesFromFile Acpi Tbl submitted Instance = 0x%X\n ", Instance));
			CurrentTable = NULL;
		}
	}

	return;
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Driver entry point
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**
    This function is ACPI driver entry point

         
    @param ImageHandle Image handle
    @param SystemTable pointer to system table

          
    @retval EFI_SUCCESS Function executed successfully, ACPI_SUPPORT_PROTOCOL installed
    @retval EFI_ABORTED Dsdt table not found or table publishing failed
    @retval EFI_ALREADY_STARTED driver already started
    @retval EFI_OUT_OF_RESOURCES not enough memory to perform operation

    @note  
  This function also creates ReadyToBoot event to update AML objects before booting

**/

EFI_STATUS EFIAPI AcpiNewCoreEntry (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    static EFI_GUID Acpisupguid = EFI_ACPI_SUPPORT_GUID;
    VOID            *DummyPtr;


//------------------------
    InitAmiLib(ImageHandle,SystemTable);
    PROGRESS_CODE(DXE_ACPI_INIT);
    //it must be ony one instance of this protocol
    Status = pBS->LocateProtocol(&Acpisupguid,NULL,&DummyPtr);
    TRACE((-1,"IN ACPI Start: %x\n", Status));
    
    if (!EFI_ERROR(Status)) 
    	return EFI_ALREADY_STARTED;

    Status = BuildMandatoryAcpiTbls (); 
    //Call Architecture Specific Library function to build a set of mandatory tables
    if (EFI_ERROR(Status)) 
    	return Status;
    
#if MPS_TABLE_SUPPORT == 1
    Status = MpsTableBuilderInit(ImageHandle, SystemTable);
    ASSERT_EFI_ERROR(Status);
#endif
    
    Status = AcpiReadyToBootEvent();
    //Call Architecture Specific Library function to craeate Ready to Boot event, if necessary 
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) 
    	return Status;
    
    
    gAcpiData.AcpiSupportProtocol.GetAcpiTable = AcpiSupportGetAcpiTable;
    gAcpiData.AcpiSupportProtocol.SetAcpiTable = AcpiSupportSetAcpiTable;
    gAcpiData.AcpiSupportProtocol.PublishTables = AcpiSupportPublishTables;
    
    gAcpiData.AcpiTableProtocol.InstallAcpiTable = AcpiInstallAcpiTable;
    gAcpiData.AcpiTableProtocol.UninstallAcpiTable = AcpiUninstallAcpiTable;

    gAcpiData.AcpiSdtProtocol.GetAcpiTable=GetAcpiTable2;
    gAcpiData.AcpiSdtProtocol.RegisterNotify=RegisterNotify;
    gAcpiData.AcpiSdtProtocol.Open=Open;
    gAcpiData.AcpiSdtProtocol.OpenSdt=OpenSdt;
    gAcpiData.AcpiSdtProtocol.Close=Close;
    gAcpiData.AcpiSdtProtocol.GetChild=GetChild;
    gAcpiData.AcpiSdtProtocol.GetOption=GetOption;
    gAcpiData.AcpiSdtProtocol.SetOption=SetOption;
    gAcpiData.AcpiSdtProtocol.FindPath=FindPath;
    gAcpiData.AcpiSdtProtocol.AcpiVersion= (EFI_ACPI_TABLE_VERSION_ALL | (1 << 5) | (1 << 6));

    gAcpiData.AcpiSupportHandle = NULL;
    SubmitAcpiTablesFromFile ();
    //Instasll ProtocolInterface;
    Status=pBS->InstallMultipleProtocolInterfaces(
               &gAcpiData.AcpiSupportHandle,
               &Acpisupguid,
               &gAcpiData.AcpiSupportProtocol,
               &gEfiAcpiTableProtocolGuid,
               &gAcpiData.AcpiTableProtocol,
               &gEfiAcpiSdtProtocolGuid,
               &gAcpiData.AcpiSdtProtocol, 
               NULL);
    ASSERT_EFI_ERROR(Status);
    return Status;
    
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
