//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file LegacySredir.c
    Serial Redirection Compatibility Support Module entry point 
    and interface functions
*/

// Disabling the warnings.
#pragma warning (disable : 4100 )

#pragma warning (disable : 4306 ) 

//---------------------------------------------------------------------------

#include    "Token.h"
#include    "Protocol/AmiLegacySredir.h"
#include    "Protocol/LegacyBios.h"
#include    "Protocol/LegacyBiosExt.h"
#include    "AmiDxeLib.h"
#include    "LegacySredir.h"

//---------------------------------------------------------------------------


extern
EFI_STATUS GetSetupValuesForLegacySredir (
    OUT AMI_COM_PARAMETERS  *AmiComParameters
);

VOID                         *gSreDirImage = 0;
UINTN                        gSreDirImageSize = 0;

UINTN                        gSreDirImageStart=0;
AMI_LEGACYSREDIR_TABLE       *gLegacySreDirTable;
EFI_EVENT                    gSreDir;
UINT32                       gSdlParameters=0;

VOID                         *ProcessOpRomRegistration = NULL;
VOID                         *UpdateComRegistration = NULL;
AMI_LEGACY_SREDIR_PROTOCOL   *LegacySredir=NULL;


EFI_LEGACY_BIOS_PROTOCOL     *gLegacyBios=NULL;
EFI_LEGACY_BIOS_EXT_PROTOCOL *gBiosExtensions = NULL;

AMI_SREDIR_DATA              *gAmiSredirData=NULL;
AMI_COM_PARAMETERS           gAmiComParameters;
AMI_COM_PARAMETERS           *gSredirSetupComParameters;

#if (SREDIR_CODE_DATA_SELECTION == 2)
static UINT32                gSreDirLockUnlockAddr=0; 
static UINT32                gSreDirLockUnlockSize=0; 
static BOOLEAN               gIsBinCopiedToShadow = FALSE;
#else
static BOOLEAN               gIsBinCopiedToBase = FALSE;
#endif

BOOLEAN                      gRedirectionStatus=FALSE;
BOOLEAN                      gIsDataCopied = FALSE;
static BOOLEAN               gIsDataCopiedToBase = FALSE;
BOOLEAN                      gSreDirImageDataInit=FALSE;
extern BOOLEAN               IsPciDevice;
BOOLEAN                      gLegacyOSBooting=FALSE;

UINTN                        gUartInputClock=0;

// AMI_LEGACY_SREDIR_PROTOCOL
AMI_LEGACY_SREDIR_PROTOCOL gLegacySredir = {
    EnableLegacySredir,
    DisableLegacySredir
};

/**
    This function copies the segment and offset address of sredir.bin

    @param  StructStartAddress
    @param  AmiComParameters

    @retval EFI_SUCCESS
*/

EFI_STATUS
DataCopiedToCsm16Function (
    IN  UINTN                   StructStartAddress,
    IN  AMI_COM_PARAMETERS      *AmiComParameters
)
{
    EFI_STATUS                      Status;
    UINTN                           ImageSize = 0x10;
    UINT32                          LockUnlockAddr, LockUnlockSize; 
    UINT16                          *Addr;
    UINT32                          Addr16;
    UINT8                           *RpSBCSig = "$SBC";
    UINT8                           *RpSBFSig = "$SBF";

    // Check Data already copied into CSM16 call back function
    if(gIsDataCopied) {
        return EFI_SUCCESS;
    }

    if(gBiosExtensions==NULL) {
        Status = pBS->LocateProtocol(
                    &gEfiLegacyBiosExtProtocolGuid,\
                    NULL,\
                    &gBiosExtensions);
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    Status = gBiosExtensions->Get16BitFuncAddress(
                                CSM16_OEM_BEFORE_CALL_BOOT_VECTOR, \
                                &Addr16);

    if (EFI_ERROR(Status)) 
        return Status;

    Status = gBiosExtensions->UnlockShadow(
                                (UINT8*)Addr16, \
                                ImageSize, \
                                &LockUnlockAddr, \
                                &LockUnlockSize);

    ASSERT_EFI_ERROR(Status);

    while(Addr16 < 0xf0000)
    {
        if (!MemCmp((UINT8*)Addr16, RpSBCSig, 4))
        {
            Addr16 = Addr16+4;          // Skip the Signature "$SBC"
            Addr = (UINT16*) Addr16;
#if (SREDIR_CODE_DATA_SELECTION == 2) || (SREDIR_CODE_DATA_SELECTION == 1)
            *Addr =(((UINTN )(gAmiSredirData)>>4) & 0xF000);
            Addr++;
            *Addr =(UINT16)(UINTN)(gAmiSredirData);
            Addr++;
            *Addr = (UINT16)(((UINT8 *)&(gAmiSredirData->Flag)) - ((UINT8*)(gAmiSredirData)));
            break;
#else
            *Addr =((StructStartAddress>>4) & 0xF000);
            Addr++;
            *Addr =(UINT16)StructStartAddress;
            Addr++;
            *Addr = (UINT16)(((UINT8 *)&(AmiComParameters->Flag)) - ((UINT8*)(AmiComParameters)));
            break;
#endif
        }
        Addr16++;
    }

    Status = gBiosExtensions->Get16BitFuncAddress(
                                CSM16_OEM_ON_BOOT_FAIL, \
                                &Addr16);

    if (EFI_ERROR(Status)) 
        return Status;
    
    while(Addr16 < 0xf0000)
    {
        if (!MemCmp((UINT8*)Addr16, RpSBFSig, 4))
        {
            Addr16 = Addr16+4;// Skip the Signature "$SBF"
            Addr = (UINT16*) Addr16;
#if (SREDIR_CODE_DATA_SELECTION == 2) || (SREDIR_CODE_DATA_SELECTION == 1)
            *Addr =(((UINTN )(gAmiSredirData)>>4) & 0xF000);
            Addr++;
            *Addr =(UINT16)(UINTN )(gAmiSredirData);
            Addr++;
            *Addr = (UINT16)(((UINT8 *)&(gAmiSredirData->Flag)) - ((UINT8*)(gAmiSredirData))) ;
            break;            
#else
            *Addr =((StructStartAddress>>4) & 0xF000);
            Addr++;
            *Addr =(UINT16)StructStartAddress;
            Addr++;
            *Addr = (UINT16)(((UINT8 *)&(AmiComParameters->Flag)) - ((UINT8*)(AmiComParameters))) ;
            break;            

#endif
        }
        Addr16++;
    }

    gBiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize);
    

    //Data copied into CSM16 call back function
    gIsDataCopied=TRUE;

    return EFI_SUCCESS;
}



#if(SREDIR_CODE_DATA_SELECTION==1) ||(SREDIR_CODE_DATA_SELECTION==2)
/***---------------------------------------------------------------------------
    This function finds whether VGA is present or not
    
    @Param NONE
    
    @retval  TRUE - Present
    FALSE - Not Present
*/

BOOLEAN
IsVgaPresent()
{
    return  (*(UINT16*)(UINTN)0x42 == 0xC000) ?TRUE:FALSE;
}

/***
    This function copies the Sredir.bin into Base Memory

    @param This Indicates the EFI_LEGACY_SREDIR_PROTOCOL instance.

    @retval EFI_SUCCESS
*/


EFI_STATUS
CopySredirDataIntoBaseMemory (
    IN AMI_LEGACY_SREDIR_PROTOCOL   *This
)
{
    UINT16              EbdaSeg = (UINT16)(*(UINT16*)0x40E);
    UINT8               *EbdaAddress = (UINT8*)((UINTN)EbdaSeg<<4);
    UINT32              EbdaSize = (*EbdaAddress)<<10;
    UINT32              NewEbdaAdd;
    UINT32              NewEbdaSeg;
    UINT8               SredirDataSize;

    if(gIsDataCopiedToBase)
        return EFI_SUCCESS;

    // Data  area for Sredir.bin is placed in below A0000. 
    // It will not be reported as EBDA memory. 
    // So place will not be changed when there is allocateEbda.
    if(EbdaSeg== 0) { 
        return EFI_NOT_FOUND;
    }

    // If VGA Present, NvRam variable InstallLegacyOSthroughRemote is 0 and 
    // Recorder mode is disabled, Sredir.bin Data area size =2KB, else It is 6KB.
    if(IsVgaPresent()&&(gAmiComParameters.InstallLegacyOSthroughRemote==0)\
                                    &&(gAmiComParameters.RecorderMode==0))
        SredirDataSize=2; 
    else 
        SredirDataSize=6;

    //New EBDA Address will be EbdaAddress - Sredir.bin Data area
    NewEbdaAdd = (EbdaSeg<<4)-(SredirDataSize * 1024);     
    NewEbdaSeg = NewEbdaAdd>>4;

    //We are taking memory for the Sredir.bin Data area from the base memory.
    //So adjust the Base Memory
    *(UINT16*)0x413 = ((*(UINT16*)0x413) - (SredirDataSize));

    //Update the New EBDA address
    *(UINT16*)0x40e = (UINT16)NewEbdaSeg;  

    //Move the OLD EBDA data to new EBDA area. 
    //We have not changed the EBDA Size here. 
    //This makes Sredir.bin data area will not be moved any where.
    //it means Sredir.bin data areais not placed under EBDA area.
    pBS->CopyMem((VOID*)NewEbdaAdd,(VOID*)(EbdaSeg<<4),EbdaSize);

    // Copy the Sredir.bin data area into Base Memory.
    gAmiSredirData = (AMI_SREDIR_DATA *)(NewEbdaAdd+EbdaSize);  

    pBS->SetMem((VOID*)(gAmiSredirData), SredirDataSize * 1024 , 0);

    // Data Area Signature 
    gAmiSredirData->Signature = 0x24535244; 

    // Some of the Basic Data Init
    gAmiSredirData->InsideInt10=0;
    gAmiSredirData->software_fc_flag=1;
    gAmiSredirData->color_save=0xFF;
    gAmiSredirData->cntr=1;
    gAmiSredirData->PrevCursorPos=0xFFFF;
    gAmiSredirData->cur_cksum=0XFFFFFFFF;
    gAmiSredirData->GraphredirFlags=2;

    // Set background and ForeGround Color of the screen
    gAmiSredirData->ESC_Seq.reset_Attribute=0x6d305b1b;
    gAmiSredirData->ESC_Seq.esc_char=0x1b;
    gAmiSredirData->ESC_Seq.sq_bracket=0x5b;
    gAmiSredirData->ESC_Seq.attrib=0;
    gAmiSredirData->ESC_Seq.semicolon1=0x3b;
    gAmiSredirData->ESC_Seq.foreground=0;
    gAmiSredirData->ESC_Seq.semicolon2=0x3b;
    gAmiSredirData->ESC_Seq.background=0;
    gAmiSredirData->ESC_Seq.m_char =0x006d;

    // Char Set Attribute 
    gAmiSredirData->VT100_attrib_[0]=0x1b;
    gAmiSredirData->VT100_attrib_[1]=0x5b;
    gAmiSredirData->VT100_attrib_[2]=0x30;
    gAmiSredirData->VT100_attrib_[3]=0x6d;
    gAmiSredirData->VT100_attrib_[4]=0x1b;
    gAmiSredirData->VT100_attrib_[5]=0x5b;
    gAmiSredirData->VT100_attrib_[6]=0;
    gAmiSredirData->VT100_attrib_[7]='m';


    // Set Cursor Position ESC Sequence
    gAmiSredirData->VT100_CursorPosition[0]=0x1b;
    gAmiSredirData->VT100_CursorPosition[1]=0x5b;
    gAmiSredirData->VT100_CursorPosition[2]=0;
    gAmiSredirData->VT100_CursorPosition[3]=0;
    gAmiSredirData->VT100_CursorPosition[4]=';';
    gAmiSredirData->VT100_CursorPosition[5]=0x30;
    gAmiSredirData->VT100_CursorPosition[6]=0x30;
    gAmiSredirData->VT100_CursorPosition[7]='H';

    // Initilize the Setup default and other SDL token data's.
    if(gAmiComParameters.MMIOBaseAddress != 0) {
        gSdlParameters |= MMIO_DEVICE;
        gAmiComParameters.BaseAddress = 0;
        gAmiComParameters.SwSMIValue = LEGACY_SREDIR_SWSMI;
#if defined(SW_SMI_IO_ADDRESS)
        gAmiComParameters.SwSMIPort = SW_SMI_IO_ADDRESS;
#endif
    } 
#if ACPI_SUPPORT
    #if FACP_FLAG_TMR_VAL_EXT
        gAmiComParameters.AcpiTimerAddress = TIMER_SIGNATURE_32BIT;
    #else
        gAmiComParameters.AcpiTimerAddress = TIMER_SIGNATURE_24BIT;
    #endif
#if ACPI_TIMER_IN_LEGACY_SUPPORT
     gAmiComParameters.AcpiTimerAddress = (UINT16)PM_TMR_BLK_ADDRESS;
#else
     gAmiComParameters.AcpiTimerAddress = (UINT16)0;
#endif
#endif

    gIsDataCopiedToBase = TRUE;

    return EFI_SUCCESS;
}

#endif

#if (SREDIR_CODE_DATA_SELECTION == 0 )

/**
    This function copies sredir.bin into base memory

    @param  IN AMI_LEGACY_SREDIR_PROTOCOL   * This

    @retval EFI_SUCCESS
*/
EFI_STATUS
CopySredirBinIntoBaseMemory (
    IN AMI_LEGACY_SREDIR_PROTOCOL   * This
)
{

    UINT16                      EbdaSeg = (UINT16)(*(UINT16*)0x40E);
    UINT8                       *EbdaAddress = (UINT8*)((UINTN)EbdaSeg<<4);
    UINT32                      EbdaSize = (*EbdaAddress)<<10;
    UINT32                      NewEbdaAdd;
    UINT32                      NewEbdaSeg;

    // Check Sredir.bin already copied into Memory
    if(gIsBinCopiedToBase) {
        return EFI_SUCCESS;
    }

    // Sredir.bin is placed in below A0000. 
    // It will not be reported as EBDA memory. 
    // So sredir place will not be changed when there is allocateEbda.
    if(EbdaSeg== 0) { 
        return EFI_NOT_FOUND;
    }

    //New EBDA Address will be EbdaAddress - Sredir.bin
    //To allocate memory for sredir.bin
    NewEbdaAdd = (EbdaSeg<<4)-SREDIR_BIN_SIZE;       
    NewEbdaSeg = NewEbdaAdd>>4;

    //We are taking memory for the Sredir.bin from the base memory. 
    //So adjust the Base Memory
    *(UINT16*)0x413 = ((*(UINT16*)0x413) - (SREDIR_BIN_SIZE >> 10));

    //Update the New EBDA address
    *(UINT16*)0x40e = (UINT16)NewEbdaSeg;

    //Move the OLD ebda data to new EBDA area. 
    //We have not changed the EBDA Size here. 
    //This makes Sredir.bin area will not be moved any where.
    //it means Sredir.bin is not placed under EBDA area.
    pBS->CopyMem((VOID*)NewEbdaAdd,(VOID*)(EbdaSeg<<4),EbdaSize);
    
    // Copy the Sredir.bin Base Memory.
    pBS->CopyMem((VOID*)(NewEbdaAdd+EbdaSize), gSreDirImage, gSreDirImageSize);
    gSreDirImageStart = (UINTN)(NewEbdaAdd+EbdaSize);
   
    //Serial Redirection starting Address
    gLegacySreDirTable = (AMI_LEGACYSREDIR_TABLE*)gSreDirImageStart;
    if (gLegacySreDirTable == NULL) return EFI_NOT_FOUND;
    
    gSredirSetupComParameters = (AMI_COM_PARAMETERS  *)(gSreDirImageStart + \
                              (gLegacySreDirTable->SreDirEfiToLegacyOffset));

    pBS->CopyMem((VOID*)gSredirSetupComParameters,\
                (VOID*)&gAmiComParameters, \
                sizeof(AMI_COM_PARAMETERS));
    
    //Sredir.bin copied into Base Memory
    gIsBinCopiedToBase=TRUE;
    
    return EFI_SUCCESS;

}
#endif




#if (SREDIR_CODE_DATA_SELECTION == 1)
/**
    This function copies sredircode.bin into base memory

    @param  IN AMI_LEGACY_SREDIR_PROTOCOL   * This

    @retval EFI_SUCCESS
*/
EFI_STATUS
CopySredirBinCodeIntoBaseMemory (
    IN AMI_LEGACY_SREDIR_PROTOCOL   * This
)
{

    UINT16                      EbdaSeg = (UINT16)(*(UINT16*)0x40E);
    UINT8                       *EbdaAddress = (UINT8*)((UINTN)EbdaSeg<<4);
    UINT32                      EbdaSize = (*EbdaAddress)<<10;
    UINT32                      NewEbdaAdd;
    UINT32                      NewEbdaSeg;
    EFI_IA32_REGISTER_SET       RegSet;
 
    // Check Sredircode.bin already copied into Memory
    if(gIsBinCopiedToBase) {
        return EFI_SUCCESS;
    }
    
    // Sredircode.bin is placed in below A0000.
    // It will not be reported as EBDA memory. 
    // So Sredircode.bin's place will not be changed when there is allocateEbda.
    if(EbdaSeg== 0) { 
        return EFI_NOT_FOUND;
    }

    // New EBDA Address will be EbdaAddress - Sredircode.bin
    // To allocate memory for sredir.bin
    NewEbdaAdd = (EbdaSeg<<4)-SREDIR_BIN_SIZE;        
    NewEbdaSeg = NewEbdaAdd>>4;

    // We are taking memory for the Sredircode.bin from the base memory. 
    // So adjust the Base Memory
    *(UINT16*)0x413 = ((*(UINT16*)0x413) - (SREDIR_BIN_SIZE >> 10));

    //Update the New EBDA address
    *(UINT16*)0x40e = (UINT16)NewEbdaSeg;

    // Move the OLD ebda data to new EBDA area. 
    // We have not changed the EBDA Size here. 
    // This makes Sredircode.bin area will not be moved any where.
    // It means Sredircode.bin is not placed under EBDA area.
    pBS->CopyMem((VOID*)NewEbdaAdd,(VOID*)(EbdaSeg<<4),EbdaSize);

    // Copy the Sredircode.bin into Base Memory.
    pBS->CopyMem((VOID*)(NewEbdaAdd+EbdaSize), gSreDirImage, gSreDirImageSize);
    gSreDirImageStart = (UINTN)(NewEbdaAdd+EbdaSize);
   
    // Serial Redirection starting Address
    gLegacySreDirTable = (AMI_LEGACYSREDIR_TABLE*)gSreDirImageStart;
    if (gLegacySreDirTable == NULL) return EFI_NOT_FOUND;
 
    CopySredirDataIntoBaseMemory(This);
    
    gSredirSetupComParameters = (AMI_COM_PARAMETERS  *)(gSreDirImageStart + \
                               (gLegacySreDirTable->SreDirEfiToLegacyOffset));
    pBS->CopyMem((VOID*)gSredirSetupComParameters, \
                 (VOID*)&gAmiComParameters, \
                 sizeof(AMI_COM_PARAMETERS));

    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy_FindDataAreaAndInitSetup; 
    gLegacyBios->FarCall86(gLegacyBios, 
                            (UINT16)(gSreDirImageStart>>4), 
                            gLegacySreDirTable->SreDirOffset, 
                            &RegSet, 
                            NULL, 
                            0);
    //Sredircode.bin copied into Base Memory
    gIsBinCopiedToBase=TRUE;
    
    return EFI_SUCCESS;

}
#endif

#if (SREDIR_CODE_DATA_SELECTION ==2)
/**
    This function copies sredircode.bin into shadow memory

    @param  IN AMI_LEGACY_SREDIR_PROTOCOL   * This

    @retval EFI_SUCCESS
*/

EFI_STATUS
CopySredirBinCodeIntoShadowMemory (
    IN AMI_LEGACY_SREDIR_PROTOCOL   * This
)
{

    EFI_STATUS                  Status;
    VOID                        *gLegacyMemoryAddress;
    EFI_IA32_REGISTER_SET       RegSet;
    
    // Check Sredircode.bin already copied into Memory
    if(gIsBinCopiedToShadow) {
        return EFI_SUCCESS;
    }
    if(gBiosExtensions == NULL) {
        Status = pBS->LocateProtocol(
                        &gEfiLegacyBiosExtProtocolGuid,\
                        NULL,\
                        &gBiosExtensions);

        if (EFI_ERROR(Status)) { 
            return Status;
        }
    }

    // Get the Shadow ( Legacy) Region memory for Sredircode.bin
    Status = gLegacyBios->GetLegacyRegion(gLegacyBios, gSreDirImageSize, \
                                          2, 0x10, &gLegacyMemoryAddress);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) { 
        return Status;
    }

    // Write Enable the Shadow Region
    gBiosExtensions->UnlockShadow(\
                        (UINT8*)gLegacyMemoryAddress, gSreDirImageSize,
                        &gSreDirLockUnlockAddr, &gSreDirLockUnlockSize);

    // Copy the Sredircode.bin into legacy region
    pBS->CopyMem((VOID*)gLegacyMemoryAddress, gSreDirImage, gSreDirImageSize);

    gSreDirImageStart=(UINTN)gLegacyMemoryAddress;

    //Serial Redirection starting Address
    gLegacySreDirTable = (AMI_LEGACYSREDIR_TABLE*)gSreDirImageStart;
    if (gLegacySreDirTable == NULL) return EFI_NOT_FOUND;
    
    Status = CopySredirDataIntoBaseMemory(This);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) { 
        return Status;
    }
    gSredirSetupComParameters = (AMI_COM_PARAMETERS  *)(gSreDirImageStart + \
                                 (gLegacySreDirTable->SreDirEfiToLegacyOffset));
    pBS->CopyMem((VOID*)gSredirSetupComParameters, \
                 (VOID*)&gAmiComParameters,\
                  sizeof(AMI_COM_PARAMETERS));

    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = Legacy_FindDataAreaAndInitSetup; 
    gLegacyBios->FarCall86(gLegacyBios, 
                        (UINT16)(gSreDirImageStart>>4), 
                        gLegacySreDirTable->SreDirOffset, 
                        &RegSet, 
                        NULL, 
                        0);

    // Lock the Shadow Region allocated for Sredir.bin
    gBiosExtensions->LockShadow(gSreDirLockUnlockAddr, gSreDirLockUnlockSize);
    //Sredircode.bin copied into shadow Memory
    gIsBinCopiedToShadow=TRUE;
    return EFI_SUCCESS;
}
#endif


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:        InitilizeBuadRate
//
// Description: This Function is used to Initilize the Baud Rate of the COM port 
//
// Input:       AMI_COM_PARAMETERS AmiComParameters
//
//
// Output:      Com port Baud Rate Initilized.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
InitilizeBuadRate(
    OUT AMI_COM_PARAMETERS *AmiComParameters
)
{

    UINT8   Data8;
    UINT16  Divisor;
    UINTN   Remainder;
    UINT8   MmioWidth = AmiComParameters->MmioWidth;

    if ( !gUartInputClock) {
        // Set the default value((24000000/13)MHz input clock).
        gUartInputClock = 1843200;
    }
    // Compute the baud rate divisor.
    Divisor = (UINT32) Div64 (gUartInputClock,
                                ((UINT32)AmiComParameters->Baudrate * 16), 
                                &Remainder);
    if ( Remainder ) {
        Divisor += 1;
    }
    
    if ((Divisor == 0) || (Divisor & 0xffff0000)) {
        return EFI_INVALID_PARAMETER;
    }

    // Check for the MMIO device. If it's MMIO device do MMIO access to 
    // Read and Write to the COM port Registers. Otherwise use IO access
    // to Read and Write to COM port Registers.
    if(AmiComParameters->MMIOBaseAddress != 0) {

        // Program Serial port. 
        // Set Line Control Register (LCR)
        Data8 = DLAB_BIT << 7;

        if (MmioWidth == DWORD_WIDTH) {
            *(UINT32*)(AmiComParameters->MMIOBaseAddress+ \
                                    (LCR_OFFSET*DWORD_WIDTH))=(UINT32)Data8;
        } else if (MmioWidth == WORD_WIDTH ) {
            *(UINT16*)(AmiComParameters->MMIOBaseAddress+ \
                                   (LCR_OFFSET*WORD_WIDTH))=(UINT16)Data8;
        } else {
            *(UINT8*)(AmiComParameters->MMIOBaseAddress+\
                                     (LCR_OFFSET*BYTE_WIDTH))=(UINT8)Data8;
        }

        //Program the Baud Rate
        if (MmioWidth == DWORD_WIDTH) {
        *(UINT32*)(AmiComParameters->MMIOBaseAddress+ \
                              (DIV_LSB*DWORD_WIDTH))=(UINT32)Divisor & 0xFF;
        *(UINT32*)(AmiComParameters->MMIOBaseAddress+ \
                             (DIV_MSB*DWORD_WIDTH))=(UINT32)Divisor >> 8;
        } else if (MmioWidth == WORD_WIDTH ) {
            *(UINT16*)(AmiComParameters->MMIOBaseAddress+ \
                               (DIV_LSB*WORD_WIDTH))=(UINT16)Divisor & 0xFF;
            *(UINT16*)(AmiComParameters->MMIOBaseAddress+ \
                               (DIV_MSB*WORD_WIDTH))=(UINT16)Divisor >> 8;

        } else {
            *(UINT8*)(AmiComParameters->MMIOBaseAddress+ \
                                (DIV_LSB*BYTE_WIDTH))=(UINT8)Divisor & 0xFF;
            *(UINT8*)(AmiComParameters->MMIOBaseAddress+ \
                                (DIV_MSB*BYTE_WIDTH))=(UINT8)(Divisor >> 8);
        }

        // Clear DLAB bit in LCR
        Data8 &= ~((UINT8)DLAB_BIT << 7);

        if (MmioWidth == DWORD_WIDTH) {
        *(UINT32*)(AmiComParameters->MMIOBaseAddress+ \
                                    (LCR_OFFSET*DWORD_WIDTH))=(UINT32)Data8;
        } else if (MmioWidth == WORD_WIDTH ) {
            *(UINT16*)(AmiComParameters->MMIOBaseAddress+ \
                                    (LCR_OFFSET*WORD_WIDTH))=(UINT16)Data8;
        } else {
            *(UINT8*)(AmiComParameters->MMIOBaseAddress+ \
                                     (LCR_OFFSET*BYTE_WIDTH))=(UINT8)Data8;
        }

    } else {

        // Program Serial port. 
        // Set Line Control Register (LCR)
        Data8 = DLAB_BIT << 7;
        IoWrite8(AmiComParameters->BaseAddress + LCR_OFFSET, Data8 );

        //Program the Baud Rate
        IoWrite8(AmiComParameters->BaseAddress + DIV_LSB, Divisor & 0xFF);
        IoWrite8(AmiComParameters->BaseAddress + DIV_MSB, Divisor >> 8);

        // Clear DLAB bit in LCR
        Data8 &= ~((UINT8)DLAB_BIT << 7);
        IoWrite8(AmiComParameters->BaseAddress + LCR_OFFSET, Data8 );
    }

    return EFI_SUCCESS;
}

/**
    This function is used for enabling the legacy Serial Redirection
    This function contains calls for Initializing Serial ports and 
    hooking the interrupts which are required for purpose of Redirection

    @param  This Indicates the AMI_LEGACY_SREDIR_PROTOCOL instance.

    @retval EFI_SUCCESS
*/

EFI_STATUS
EFIAPI
EnableLegacySredir (
    IN  AMI_LEGACY_SREDIR_PROTOCOL  *This
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_IA32_REGISTER_SET   RegSet;
#if(SREDIR_CODE_DATA_SELECTION==0)
    UINTN                       StructStartAddress;
#endif
    
    
    if(gRedirectionStatus) {
        return EFI_NOT_FOUND;
    }

    if(gLegacyBios==NULL) {
        Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, \
                                     NULL,\
                                     &gLegacyBios);
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }

    if(gSreDirImageDataInit == FALSE) {

        //Getting Setup values from terminal module
        Status = GetSetupValuesForLegacySredir(&gAmiComParameters);

        if(EFI_ERROR(Status)){
            return Status;
        }
    #if DISPLAY_WHOLE_SCREEN
        gSdlParameters |= DISPLAY_SCREEN;
    #endif
    #if TRAP_INT10_WORKAROUND
        gSdlParameters |= INT10_WORKAROUND;
    #endif
    #if SERIAL_READ_WRITE_CALLBACK
        gSdlParameters |= SERIAL_READWRITE_CALLBACK;
    #endif
    #if CTRLI_KEY_MAPPING
        gSdlParameters |= CTRLI_MAPPING;
    #endif
    #if CTRLH_KEY_MAPPING
        gSdlParameters |= CTRLH_MAPPING;
    #endif
    #if SERIAL_WRITE_ERROR_CHECK
        gSdlParameters |= SERIAL_ERROR_CHECK;
    #endif
    #if OEM_ESC_SEQUENCES
        gSdlParameters |= ESC_SEQUENCES;
    #endif


        //Initialize the Non Standard Serial Port Registers.
        Status = InitilizeNonCommonSerialRegsiters(&gAmiComParameters);
        if(EFI_ERROR(Status)){
            return Status;
        }

        if(gAmiComParameters.MMIOBaseAddress != 0) {
            gSdlParameters |= MMIO_DEVICE;
            gAmiComParameters.BaseAddress = 0;
            gAmiComParameters.SwSMIValue = LEGACY_SREDIR_SWSMI;
    #if defined(SW_SMI_IO_ADDRESS)
            gAmiComParameters.SwSMIPort = SW_SMI_IO_ADDRESS;
    #endif
        } 
        

        gAmiComParameters.SdlParameters = gSdlParameters;

        gAmiComParameters.SredirBinSize=SREDIR_BIN_SIZE;
        gAmiComParameters.RefreshScreenKey = REFRESH_SCREEN_KEY;
        gAmiComParameters.UartPollingRedirection = UART_POLLING_REDIRECTION;
        gAmiComParameters.MaximumErrorCount= MAXIMUM_SERIAL_WRITE_ERROR_COUNT;
        gAmiComParameters.FlowControlFailureCount= MAX_FAILURES_ALLOWED;
        gAmiComParameters.CheckForLoopBackRetryCount = CHECKLOOPBACK_RETRY_COUNT;
        // Copy the Sredir.bin into Shadow region or Base Memory based on the 
        // SREDIR_BIN_IN_SHADOW sdl token value. Data area is allocated in the 
        // Base memory and used by the sredir.bin
        
#if (SREDIR_CODE_DATA_SELECTION==0)
        Status =CopySredirBinIntoBaseMemory(&gLegacySredir);
#endif
#if (SREDIR_CODE_DATA_SELECTION==1)
        Status = CopySredirBinCodeIntoBaseMemory(&gLegacySredir);
#endif
#if (SREDIR_CODE_DATA_SELECTION==2)
        Status = CopySredirBinCodeIntoShadowMemory(&gLegacySredir);
#endif
        if(EFI_ERROR(Status)){
            return Status;
        }
        gSreDirImageDataInit = TRUE;
    }


    if(gLegacyOSBooting) {
    //
    // Transfer the Setup Values and COM Parameters from EFI to Legacy.
    //
#if (SREDIR_CODE_DATA_SELECTION == 0 )

    StructStartAddress = (UINTN )gSredirSetupComParameters;
    Status=DataCopiedToCsm16Function(StructStartAddress, &gAmiComParameters);
#else
        Status=DataCopiedToCsm16Function(0,NULL);
#endif
        if(EFI_ERROR(Status)){
            return Status;
        }
    }
    
    Status=InitilizeBuadRate(&gAmiComParameters);

    if(EFI_ERROR(Status)){
        return Status;
    }
    
    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = LEGACY_SreDirInitializeSerialPort; 
#if(SREDIR_CODE_DATA_SELECTION == 0)
    RegSet.X.BX = MAXIMUM_SERIAL_WRITE_ERROR_COUNT;
    RegSet.X.CX = MAX_FAILURES_ALLOWED;
#if ACPI_SUPPORT
    #if FACP_FLAG_TMR_VAL_EXT
        RegSet.E.EDX = TIMER_SIGNATURE_32BIT;
    #else
        RegSet.E.EDX = TIMER_SIGNATURE_24BIT;
    #endif
#if ACPI_TIMER_IN_LEGACY_SUPPORT
     RegSet.X.DX = PM_TMR_BLK_ADDRESS;
#else
    RegSet.X.DX = 0;
#endif
#endif
#endif

    Status = gLegacyBios->FarCall86(gLegacyBios, 
             (UINT16)(gSreDirImageStart>>4), 
             gLegacySreDirTable->SreDirOffset, 
             &RegSet, 
             NULL, 
             0);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = LEGACY_SerialRedirection;  
    Status = gLegacyBios->FarCall86(gLegacyBios, 
             (UINT16)(gSreDirImageStart>>4), 
             gLegacySreDirTable->SreDirOffset, 
             &RegSet, 
             NULL, 
             0);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    gRedirectionStatus=TRUE;
    return Status;
}


/**
    This function is used to disable the Legacy Serial redirection
    This function contains calls for functions which are used to 
    release the interrupts which are used for Serial Redirection

    @param  This Indicates the AMI_LEGACY_SREDIR_PROTOCOL instance.

    @retval EFI_SUCCESS

*/

EFI_STATUS
EFIAPI
DisableLegacySredir (
    IN  AMI_LEGACY_SREDIR_PROTOCOL   *This
)
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_IA32_REGISTER_SET       RegSet;

    if(!gRedirectionStatus) {
        return EFI_NOT_FOUND;
    }

    if(gLegacyBios==NULL) {
        Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid,\
                                     NULL,\
                                     &gLegacyBios);
        if (EFI_ERROR(Status)) { 
            return Status;
        }
    }
 
    //Serial Redirection starting Address
    gLegacySreDirTable = (AMI_LEGACYSREDIR_TABLE*)gSreDirImageStart;

    if (gLegacySreDirTable == NULL) {
        return EFI_NOT_FOUND;
    }
 
    pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
    RegSet.X.AX = LEGACY_ReleaseSerialRedirection;  

    Status = gLegacyBios->FarCall86(gLegacyBios, 
             (UINT16)(gSreDirImageStart>>4), 
             gLegacySreDirTable->SreDirOffset, 
             &RegSet, 
             NULL, 
             0);

    gRedirectionStatus=FALSE;

    return Status;
}


/**
    This function is used for Legacy Serial Redirection

    @param EFI_Hanlde - Image Handle
    @param EFI_System_Table - Pointer to System Table

    @retval EFI_SUCCESS or EFI_NOT_FOUND

*/
EFI_STATUS
EFIAPI
Legacy_OS_SerialRedirection (
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)   
{

    EFI_STATUS  Status;

    if(!LegacySredir) {
        Status=pBS->LocateProtocol(&gEfiLegacySredirProtocolGuid, NULL, &LegacySredir);
        if(EFI_ERROR(Status)) {
            return Status;
        }
    }

    gLegacyOSBooting=TRUE;
    Status = LegacySredir->EnableLegacySredir(LegacySredir);

    return Status;

}

#if CLEAR_LEGACYSREDIR_KB_BUFFER_AT_READYTOBOOT
/**
    Function to clear the Keyboard character buffer
    (in the 16-bit Serial Redirection module)

    @param Event 
    @param Context 

    @retval VOID

*/
VOID
EFIAPI
ClearKbCharBuffer (
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
    EFI_IA32_REGISTER_SET       RegSet;

    // Serial Redirection starting Address
    gLegacySreDirTable = (AMI_LEGACYSREDIR_TABLE*)gSreDirImageStart;

    if (gLegacySreDirTable != NULL) {
        pBS->SetMem(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
        RegSet.X.AX = LEGACY_ClearKbCharBuffer;  

        gLegacyBios->FarCall86(gLegacyBios, 
                         gLegacySreDirTable->SreDirSegment, 
                         gLegacySreDirTable->SreDirOffset, 
                         &RegSet, 
                         NULL, 
                         0);
    }

    pBS->CloseEvent(Event);
}
#endif

/**
    This call back will be called before and after installing legacy
    OpROM. Before installing will enable Legacy redirection and
    after installing Legacy Redirection will be disabled.

    @param Event Call back event
    @param Context pointer to calling context

    @retval VOID

*/
VOID
EFIAPI
ProcessOpRomCallback (
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  Handle;
    UINTN                       Size = sizeof(EFI_HANDLE);
    CSM_PLATFORM_POLICY_DATA    *OpRomStartEndProtocol;

    // Locate gOpromStartEndProtocolGuid. If interface is NULL,
    // it means OpRom has beenalready Launched and exited.
    //  else Option Rom is about to Launch.
    Status = pBS->LocateHandle(ByRegisterNotify, \
                               NULL,\
                               ProcessOpRomRegistration, \
                               &Size,\
                               &Handle);
    if (EFI_ERROR(Status)) {
        return;
    }

    Status = pBS->HandleProtocol(Handle, \
                                 &gOpromStartEndProtocolGuid,\
                                 &OpRomStartEndProtocol);
    if (EFI_ERROR(Status)) {
        return;
    }
    
    if((OpRomStartEndProtocol != NULL) && \
       (OpRomStartEndProtocol->ExecuteThisRom == FALSE))
        return;
    
    if(!LegacySredir) {
        Status=pBS->LocateProtocol(&gEfiLegacySredirProtocolGuid, \
                                   NULL, &LegacySredir);
        if(EFI_ERROR(Status)) {
            return;
        }
    }

    if(OpRomStartEndProtocol != NULL) {

        // Option Rom about to launch, Enable Legacy Redirection.
        LegacySredir->EnableLegacySredir(LegacySredir);
        
    } else {

        // Option Rom has been launched and exited, Disable Legacy Redirection.
        LegacySredir->DisableLegacySredir(LegacySredir);
    }

    return;
}

/**
    This call back will update the gAmiComParameters after all drivers are
    connected in BDS, so that it will be used at legacy boot event call.

    @param Event Call back event
    @param Context pointer to calling context

    @retval VOID

*/
VOID
EFIAPI
UpdateAmiComParameters (
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
	
    AMI_COM_PARAMETERS      AmiComParameters;

    // Give a Dummy call to fill gAmiComParameters so that 
    // the data is available for Legacy_OS_SerialRedirection().
    // We save this because the NVRAM variables are cleared by Terminal driver
    // by DisconnectSerialIo() at legacy Boot in csm.
    GetSetupValuesForLegacySredir(&AmiComParameters);
    
    pBS->CloseEvent(Event);

}

/**
    Legacy Serial Redirection driver entry point

    @param Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT
        EFI System Table - Pointer to System Table

    @retval EFI_STATUS OR EFI_NOT_FOUND

*/

EFI_STATUS
EFIAPI
LegacySredirEntryPoint (
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{

    EFI_STATUS                      Status;
    EFI_HANDLE                      NewHandle;
    EFI_EVENT                        Event;
    
    #if CLEAR_LEGACYSREDIR_KB_BUFFER_AT_READYTOBOOT
        EFI_EVENT                    ReadyToBootEvent;
    #endif

    EFI_EVENT                        UpdateComParameterEvent;

    InitAmiLib(ImageHandle, SystemTable);

    // Locate LegacyBios Protocol.
    Status = pBS->LocateProtocol(
               &gEfiLegacyBiosExtProtocolGuid, NULL, &gBiosExtensions);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    // Get Sredir.bin/Sredircode.bin Image
    Status = gBiosExtensions->GetEmbeddedRom(\
                                             SREDIR_MODULEID,\
                                             SREDIR_VENDORID,\
                                             SREDIR_DEVICEID,\
                                             &gSreDirImage,\
                                             &gSreDirImageSize);
    if (EFI_ERROR(Status)) {
         return Status;
    }

    //Install AMI_LEGACY_SREDIR_PROTOCOL on a new handle
    NewHandle = NULL;
    Status = pBS->InstallProtocolInterface (\
                      &NewHandle,\
                      &gEfiLegacySredirProtocolGuid,\
                      EFI_NATIVE_INTERFACE,\
                      &gLegacySredir\
                      );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
         return Status;
    }
    Status = RegisterProtocolCallback(&gOpromStartEndProtocolGuid,\
                     ProcessOpRomCallback,\
                     NULL,\
                     &Event,\
                     &ProcessOpRomRegistration);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    Status = RegisterProtocolCallback(&gBdsAllDriversConnectedProtocolGuid,\
                         UpdateAmiComParameters,\
                         NULL,\
                         &UpdateComParameterEvent,\
                         &UpdateComRegistration);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    #if CLEAR_LEGACYSREDIR_KB_BUFFER_AT_READYTOBOOT
        // Register the ReadyToBoot event function to clear the Keyboard
        // character buffer (in the 16-bit Serial Redirection module)
        CreateReadyToBootEvent(TPL_CALLBACK,\
                               ClearKbCharBuffer,\
                               NULL,\
                               &ReadyToBootEvent\
                               );
    #endif
    
    // Callback to Enable Legacy Serial Redirection for Legacy OS.
    Status = CreateLegacyBootEvent(\
                  TPL_CALLBACK,\
                  Legacy_OS_SerialRedirection,\
                  NULL,\
                  &gSreDir\
        );
    
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
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
