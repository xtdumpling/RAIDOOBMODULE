//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.14
//    Bug Fix : [Fixes] Fixed the string of "Error DIMM information" on screen 
//              is corrupted when equipping failed DIMM.
//    Reason  : Bug Fixed
//    Auditor : Joe Jhang
//    Date    : Aug/09/2017
//
//  Rev. 1.13
//      Bug Fixed:  Remove the "P1" string when MRC post error occurred for UP server
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Jul/28/2017
//
//  Rev. 1.12
//      Bug Fixed:  Modify IPMI MRC event log data "SensorNumber" to "FE" for Purley platform.
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/06/2017
//
//  Rev. 1.11
//      Bug Fixed:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/05/2017
//
//  Rev. 1.10
//      Bug Fixed:  Fix MRC related warning message doesn't show at correct
//                  position.
//      Reason:     
//      Auditor:    Isaac Hsu
//      Date:       Jun/03/2017
//
//  Rev. 1.09
//      Bug Fixed:  Early Graphic Logo Support.
//      Reason:     
//      Auditor:    Isaac Hsu
//      Date:       Mar/14/2017
//
//  Rev. 1.08
//      Bug Fixed:  Fix coding error.
//      Reason:     
//      Auditor:    Isaac Hsu
//      Date:       Mar/13/2017
//
//  Rev. 1.07
//      Bug Fixed:  Fix that Screen doesn't show No memory DIMM detected... 
//                  when remove DIMM and IPMI unavailable.
//      Reason:     
//      Auditor:    Isaac Hsu
//      Date:       Mar/10/2017
//
//  Rev. 1.06
//      Bug Fixed:  Fix that System log mistake sel (QPI Error) when no memory
//                  found.
//      Reason:     
//      Auditor:    Isaac Hsu
//      Date:       Mar/07/2017
//
//  Rev. 1.05
//      Bug Fixed:  Improve the KTI oem code.
//      Reason:     check NULL pointer.
//      Auditor:    Leon Xu
//      Date:       Jan/18/2017
//
//  Rev. 1.04
//      Bug Fixed:  Corrected channel number for failing DIMM
//      Reason:     Purley projects all CPU start from channel A.
//      Auditor:    Stephen Chen
//      Date:       Jan/17/2017
//
//  Rev. 1.03       
//      Bug Fixed:  Move no memory error beep into function OemPlatformFatalError.
//      Reason:     No memory error beep doesn't working in funciton AmiPlatformFatalErrorWrapper
//      Auditor:    Jimmy Chiu
//      Date:       Aug/17/2016
//
//  Rev. 1.02
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
//  Rev. 1.01
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
//
//  Rev. 1.00
//      Bug Fixed:  Add board level override KTI setting
//      Reason:     
//      Auditor:    Salmon Chen
//      Date:       Mar/14/2016
//
//*****************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiMrcHooksWrapperLib.c
    MRC Hooks wrapper implementation
**/

//---------------------------------------------------------------------------
 #include <AmiMrcHooksWrapperLib.h>
#include <Token.h>			//SMCPKG_SUPPORT++
//---------------------------------------------------------------------------

 PLATFORM_LOG_WARNING_ELINK* PlatformLogWarningElink[] = { OVERRIDE_PLATFORM_LOG_WARNING_HOOK NULL };
 MODIFY_MRC_INPUT_PARAMETERS_ELINK* ModifyMrcInputParametersElink[] = {OVERRIDE_MRC_INPUT_PARAMETERS_HOOK NULL};
 UPDATE_PLATFORM_DATA_ELINK* UpdatePlatformDataElink[] = {OVERRIDE_PLATFORM_DATA_HOOK NULL};
 MODIFY_MRC_SPEED_ELINK* ModifyMrcSpeedElink[] = {OVERRIDE_MRC_SPEED_FREQUENCY_HOOK NULL};
 MODIFY_KTI_PARAMETER_ELINK* ModifyKtiParameterElink[] = {OVERRIDE_KTI_PARAMETER_HOOK NULL};
 PLATFORM_LOG_FATAL_ERROR_ELINK* PlatformFatalErrorElink[] = {OVERRIDE_PLATFORM_LOG_FATAL_ERROR_HOOK NULL};
 KTI_LOG_WARNING_ELINK* KtiLogWarningElink[] = {OVERRIDE_KTI_LOG_WARNING_HOOK NULL};
 KTI_CHECKPOINT_ELINK*  KtiCheckPointElink[] = {OVERRIDE_KTI_CHECKPOINT_HOOK NULL};
 GET_BOARD_TYPE_ELINK*  OemGetBoardTypeBitmaskElink[] = {OVERRIDE_GET_BOARD_TYPE_HOOK NULL};
 KTI_GET_EPARAMS_ELINK* OemKtiGetEParamsElink[] = {OVERRIDE_KTI_GET_EPARAMS_HOOK NULL};
 PUBLISH_HOB_DATA_ELINK* OemPublishHobDataElink[] = {OVERRIDE_PUBLISH_HOB_DATA_HOOK NULL};
 PLATFORM_OUTPUT_ERROR_ELINK* PlatformOutputErrorElink[] = {OVERRIDE_PLATFORM_OUTPUT_ERR_HOOK NULL};
 PLATFORM_OUTPUT_WARNING_ELINK* PlatformOutputWarningElink[] = {OVERRIDE_PLATFORM_OUTPUT_WAR_HOOK NULL};
 GET_RESOURCEMAP_UPDATE_ELINK* OemGetResourceMapUpdateElink[] = {OVERRIDE_GET_RESOURCEMAP_UPDATE_HOOK NULL};
 TURN_AROUND_TIMINGS_ELINK* TurnAroundTimingsElink[] = {OVERRIDE_TURN_AROUND_TIMINGS_HOOK NULL};

// SuperM overrie KTI tap
#ifndef __SMC_KTI_OVERRIDE__
#define __SMC_KTI_OVERRIDE__
#endif
#include "..\..\..\SmcPkg\Include\SuperMPeiDriver.h"
#if SMCPKG_SUPPORT			//SMCPKG_SUPPORT++
#include "Ppi/IPMITransportPPi.h"
#include "Include/IpmiNetFnSensorDefinitions.h"
#include "UBOX_MISC.h"
#include <PeiSimpleTextOutPpi.h>
//#include "AstVbiosPrintLib.h"
#include "Library/IoLib.h"
#endif //SMCPKG_SUPPORT++		
// SuperM overrie KTI tap


#if SMCPKG_SUPPORT     //SMCPKG_SUPPORT++
EFI_GUID gEfiPeiSimpleTextOutPPIGuid = EFI_PEI_SIMPLETEXTOUT_PPI_GUID;
void ReportErrorDIMMToScreen(
  PSYSHOST host,
  UINT8    majorCode,
  UINT8    minorCode,
  UINT8    logData,
  UINT8	   pipe
  )
{
#if defined EarlyVideo_SUPPORT && EarlyVideo_SUPPORT == 0x01
    EFI_PEI_SIMPLETEXTOUT_PPI  *mSimpleTextOutPpi = NULL;
    EFI_PEI_SERVICES  **PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
    CHAR8 character[2]={0,0};
#if EarlyVideo_Mode
    UINT8 NumOfRow = SMC_MAX_ROW - 8;
#else
    UINT8 NumOfRow = 0x11;
#endif
    UINT8 NumOfCol = 0x00;
    EFI_STATUS Status;
    UINT16 WARN_CODE[] = {    0x0700, 0x0900, 0x0A00, 0x0B00, 0x0C00, 0x0D00, 0x0E00, 0x0F00, 
                              0x1000, 0x1400, 0x1500, 0x1700, 0x1F00, 0x2400, 0x2600, 0x2800,
                              0x2900, 0x2A00, 0x3000, 0x3100, 0x3400, 0x3500, 0x3600, 0x3900,
                              0x3A00, 0x3B00, 0x3C00, 0x8400, 0xE800, 0xEB00, 0xEC00, 0xED00,
                              0xEF00, 0xF000, 0xF400, 0xF500, 0xF600, 0xF700, 0xF800, 0xF900,
                              0xFA00
                         };
    UINT8 Index1;
#if EarlyVideo_Mode
    UINT8 Attr = PcdGet8(PcdSmcColor);
#else
    UINT8 Attr = 0x0F;
#endif

    Status = (**PeiServices).LocatePpi(PeiServices, &gEfiPeiSimpleTextOutPPIGuid, 0, NULL, &mSimpleTextOutPpi);
    if(Status==EFI_SUCCESS){
        //OpenVgaDecode();
#ifdef SERIAL_DBG_MSG
        rcPrintf ((host, "Failing DIMM: pipe = %x\n", pipe));
#endif	
        if (pipe) NumOfRow+=2;
	
        for ( Index1 = 0; Index1 < sizeof(WARN_CODE)/sizeof(WARN_CODE[0]); Index1++ ){
            if ( WARN_CODE[Index1] != (majorCode << 8) ) continue;
#ifdef SERIAL_DBG_MSG      
            rcPrintf ((host, "Failing DIMM: Fatal & Warning Code detected = %x, logData = %x \n", WARN_CODE[Index1], logData));
#endif		
            mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "                                                                   ");
            switch (WARN_CODE[Index1]) {
                case 0x0700:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory DIMM incompatible with memory controllers.");
                    break;
                case 0x0900:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Failed to honor Lockstep mode.");        	 
                    break;
                case 0x0A00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "(runtime)Failing DIMM: DIMM location.");
                    break;
                case 0x0B00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Uncorrectable error found, Memory Rank is disabled.");
                    break; 
                case 0x0C00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Failed to honor Mirror mode");        	 
                    break;
                case 0x0D00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Partial Mirror mode is disabled.");        	 
                    break;
                case 0x0E00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Failed to honor Interleave mode.");
                    break;
                case 0x0F00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "DIMM communication failed.");
                    break;
                case 0x1000:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Sparing is disabled.");
                    break;
                case 0x1400:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory training failure.");
                    break;
                case 0x1500:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory training failure.");
                    break;
                case 0x1700:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Incorrect memory DIMM population.");
                    break;
                case 0x1F00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Early Read ID training warning.");
                    break;
                case 0x2400:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "DQ swizzing failed.");
                    break;
                case 0x2600:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory signal is too marginal.");
                    break;
                case 0x2800:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "FNV opcode invalid.");
                    break;
                case 0x2900:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory training failed.");
                    break;
                case 0x2A00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "No memory error.");
                    break;
                case 0x3000:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Failing DIMM: DIMM location. (Correctable memory component found)");
                    break;
                case 0x3100:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Failing DIMM: DIMM location. (Uncorrectable memory component found)");
                    break;
                case 0x3400:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Difference Completion Delay is exceeded.");
                    break;
                case 0x3500:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Post Package Repair warning.");
                    break;
                case 0x3600:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Check Bounds error.");
                    break;
                case 0x3900:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "NVDIMM boot related warning.");
                    break;
                case 0x3A00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "ADDC(Adaptive Daul Device Data Correction) is disabled.");
                    break;
                case 0x3B00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "SDDC(Single Device Data Correction) is disabled");
                    break;
                case 0x3C00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "NVDIMM controller FW is out of date.");
                    break;
                case 0x8400:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "NVDIMM controller Media statua warning.");
                    break;
                case 0xE800:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "No memory DIMM detected, install memory DIMMs.");
                    break;
                case 0xEB00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory is not useful.");
                    break;
                case 0xEC00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory error. (Vendor:IDT)");
                    break;
                case 0xED00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Incorrect memory DIMM population, check Memory Population Rule.");
                    break;
                case 0xEF00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory initialization panic, apply AC Reset.");
                    break;
                case 0xF000:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Failed to program memory voltage regulator.");
                    break;
                case 0xF400:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory controller error.");
                    break;
                case 0xF500:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Failed to program memory voltage regulator.");
                    break;
                case 0xF600:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Processor SMBUS error.");
                    break;
                case 0xF700:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "PCU error.");
                    break;
                case 0xF800:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "NVDIMM error.");
                    break;
                case 0xF900:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "Memory rank interleave error.");
                    break;
                case 0xFA00:
                    mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "DIMM capacity exceeded SKU limit");
                    break;
                default:

                    break;
            }
            NumOfRow = (NumOfRow + 0x01);

            if ( logData != 0xFF )
            {    
#ifdef SERIAL_DBG_MSG		
                rcPrintf ((host, "Failing DIMM: DIMM logdata = %x\n", logData));		 
#endif		
                // Bit7-5:CPU, 4-2: Channel 1-0: DIMM
#if Single_CPU_Platform == 1                
                mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "   ");
                character[0]=' ';
                mSimpleTextOutPpi->OutputString (3, NumOfRow, Attr, character);
                mSimpleTextOutPpi->OutputString (4, NumOfRow, Attr, " DIMM");
#else                
                mSimpleTextOutPpi->OutputString (00, NumOfRow, Attr, "  P");
                character[0]='1' + (CHAR8)((logData >> 5) & 0x07);
                mSimpleTextOutPpi->OutputString (3, NumOfRow, Attr, character);
                mSimpleTextOutPpi->OutputString (4, NumOfRow, Attr, "-DIMM");
#endif                
                character[0]='A' + (CHAR8)((logData >> 2) & 0x07);
                mSimpleTextOutPpi->OutputString (9, NumOfRow, Attr, character);
                character[0]='1' + (CHAR8)(logData & 0x03);
                mSimpleTextOutPpi->OutputString (10, NumOfRow, Attr, character);

                NumOfCol = 0x00;
                NumOfRow = (NumOfRow + 0x01);
            }
        }
    }
#endif //EarlyVideo_SUPPORT
}
#endif //SMCPKG_SUPPORT++

 /**

   AmiPlatformFatalErrorWrapper - OEM hook wrapper to provide fatal error output to the
   user interface and to override the halt on error policy.

   @param host - pointer to sysHost structure
   @param majorCode - major error/warning code
   @param minorCode - minor error/warning code
   @param haltOnError - pointer to variable that selects policy

 **/
 VOID
 AmiPlatformFatalErrorWrapper (
   IN PSYSHOST host,
   IN UINT8    majorCode,
   IN UINT8    minorCode,
   IN UINT8    *haltOnError
   ) {
   
    UINT8 ElinkCount;
#if SMCPKG_SUPPORT  //SMCPKG_SUPPORT++
    EFI_STATUS	Status = EFI_SUCCESS;
    EFI_PEI_SERVICES  **PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
    PEI_IPMI_TRANSPORT_PPI *IpmiTransportPpi = (PEI_IPMI_TRANSPORT_PPI *)host->var.common.ipmiTransPpi;
    UINT8     ResponseDataSize, i, k=0xFF, Index1;
    UINT8     ResponseData[2];  
    IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST EventMessage;
    UINT32    j=0;
    BOOLEAN MRC_ERROR_FOUND = FALSE;
    UINT16 WARN_CODE[] = {    0x0700, 0x0900, 0x0A00, 0x0B00, 0x0C00, 0x0D00, 0x0E00, 0x0F00, 
                              0x1000, 0x1400, 0x1500, 0x1700, 0x1F00, 0x2400, 0x2600, 0x2800,
                              0x2900, 0x2A00, 0x3000, 0x3100, 0x3400, 0x3500, 0x3600, 0x3900, 
                              0x3A00, 0x3B00, 0x3C00, 0x8400, 0xE800, 0xEB00, 0xEC00, 0xED00,
                              0xEF00, 0xF000, 0xF400, 0xF500, 0xF600, 0xF700, 0xF800, 0xF900,
                              0xFA00
                         };
    UINT8    l;
    UINT8    SKIP_CODE[] = { ERR_MRC_STRUCT, ERR_READ_MC_FREQ, ERR_SMBUS, ERR_PCU, ERR_NGN, ERR_INTERLEAVE_FAILURE,
                             ERR_SKU_LIMIT, WARN_DIMM_COMPAT, WARN_INTERLEAVE_FAILURE, WARN_DIMM_POP_RUL, WARN_EARLY_RID,
                             WARN_DQ_SWIZZLE_DISC, WARN_MEMORY_TRAINING, WARN_RT_DIFF_EXCEED
                           };

    //if ( majorCode == ERR_NO_MEMORY )
    //	  SmcPostErrorNoMemBeep();
    //
    //Locate ipmi Transport PPI to send commands to BMC.
    //
    if(host->var.common.socketId == 0) { //BSP, report asap
        ResponseDataSize = 1;
        EventMessage.GeneratorId = 0x1A;
        EventMessage.EvMRevision = 0x00;
        EventMessage.SensorNumber = 0xFE;   //FE for purley platform
        EventMessage.EventDirType = 0x6F;	//EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
        EventMessage.OEMEvData1 = majorCode;
        EventMessage.OEMEvData2 = minorCode;

        for ( Index1 = 0; Index1 < sizeof(WARN_CODE)/sizeof(WARN_CODE[0]); Index1++ )
        {
            if ( WARN_CODE[Index1] == (majorCode << 8) ) // if MRC error
            {	
                MRC_ERROR_FOUND = TRUE;
#ifdef SERIAL_DBG_MSG					  
                rcPrintf ((host, "Failing DIMM: FATAL/WARNING code found: %x....\n", WARN_CODE[Index1]));
#endif				  
                break;
            } else continue;	// keep finding
        }
 	  
        if (MRC_ERROR_FOUND)
        {
#ifdef SERIAL_DBG_MSG			  
            rcPrintf ((host, "Failing DIMM: MRC_ERROR_FOUND flag set, start to log SEL.\n"));
#endif			  
            EventMessage.SensorType = 0xCC;
            j = (UINT32) ((majorCode << 8) | minorCode);
            if (j == 0x0A00) return;  // Runtime UECC already log to BMC, don't log here.					
            for (i = 0; i < MAX_LOG; i++)
            {
                if ( j == host->var.common.status.log[i].code)
                {
                    k = (UINT8)((((host->var.common.status.log[i].data)&0x3FFFFFF)>>19)|(((host->var.common.status.log[i].data)&0x3FFFFF)>>14)|(((host->var.common.status.log[i].data)&0x3FF) >> 8));					  
                    // Bit7-5:CPU, 4-2: Channel 1-0: DIMM
                    for(l=0; l<sizeof(SKIP_CODE); l++){
                        if(majorCode == SKIP_CODE[l])
                            k = 0xFF; //Set CPU/CH/DIMM information to 0xFF when the error/warning doesn't contain full information
                    }
                    EventMessage.OEMEvData3 = k; //(socket << 24) | (ch << 16) | (dimm << 8) | rank)
#ifdef SERIAL_DBG_MSG							
                    rcPrintf ((host, "Failing DIMM: data = 0x%x, k=0x%x\n", host->var.common.status.log[i].data, k));
#endif						
                    ReportErrorDIMMToScreen(host, majorCode, minorCode, k, 0);
                }
            }
        }// if (MRC_ERROR_FOUND)
        else {// QPI error
            EventMessage.SensorType = 0xCB;
            EventMessage.OEMEvData3 = 0xFF;
        }

        if(!IpmiTransportPpi)
            Status = (**PeiServices).LocatePpi(PeiServices, &gEfiPeiIpmiTransportPpiGuid, 0, NULL, &IpmiTransportPpi);

        if(IpmiTransportPpi) {
            Status = IpmiTransportPpi-> SendIpmiCommand (
                                          IpmiTransportPpi,
                                          IPMI_NETFN_SENSOR_EVENT,
                                          BMC_LUN,
                                          IPMI_SENSOR_PLATFORM_EVENT_MESSAGE,
                                          (UINT8 *) &EventMessage,
                                          sizeof (EventMessage),
                                          (UINT8 *) &ResponseData,
                                          (UINT8 *) &ResponseDataSize );
        }
    }	//if(host->var.common.socketId == 0)
#endif //SMCPKG_SUPPORT	 
	
    for( ElinkCount = 0; PlatformFatalErrorElink[ElinkCount]; ElinkCount++ ) {
      PlatformFatalErrorElink[ElinkCount](host,majorCode,minorCode,haltOnError);
    }
 }

#if SMCPKG_SUPPORT		//SMCPKG_SUPPORT++
 
void
OemIpmiWarningReport(PSYSHOST host)
{
    PEI_IPMI_TRANSPORT_PPI *IpmiTransportPpi = (PEI_IPMI_TRANSPORT_PPI *)host->var.common.ipmiTransPpi;
    UINT8    ResponseDataSize;
    UINT8    ResponseData[10];  
    IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST EventMessage;
    UINT8    MajorCode;
    UINT8    MinorCode;
    UINT32   LogData;
    UINT8    Node, Channel, Dimm;
    EFI_STATUS Status;
    UINT8    i;
    UINT8    SKIP_CODE[] = { ERR_MRC_STRUCT, ERR_READ_MC_FREQ, ERR_SMBUS, ERR_PCU, ERR_NGN, ERR_INTERLEAVE_FAILURE,
                             ERR_SKU_LIMIT, WARN_DIMM_COMPAT, WARN_INTERLEAVE_FAILURE, WARN_DIMM_POP_RUL, WARN_EARLY_RID,
                             WARN_DQ_SWIZZLE_DISC, WARN_MEMORY_TRAINING, WARN_RT_DIFF_EXCEED
                           };

 #ifdef SERIAL_DBG_MSG	
    rcPrintf ((host, "\n IpmiTransportPpi = 0x%x \n",IpmiTransportPpi ));
 #endif  
    if(IpmiTransportPpi) {
 /*
 Description         Generator ID[8:9]    Sensor Type[11]    Sensor Number[12]    Event Type[13]    Event Data1[14]    Event Data2[15]    Event Data3[16]
 BIOS MRC Error	0x1A              0x00               0xCA                 0XFF              0x6F          MajorCode          MinorCode    Bit0~1:Dimm Bit2~4:Channel Bit5~7:CPU
 */
        for (Node = 1; Node < MAX_SOCKET; Node++) {
            //
            // Check if socket is valid
            //
            if ((host->var.common.socketPresentBitMap & (BIT0 << Node)) == 0) continue;
            MajorCode = host->var.mem.socket[Node].majorCode;
            MinorCode = host->var.mem.socket[Node].minorCode;
            LogData  =  host->var.mem.socket[Node].logData;
 #ifdef SERIAL_DBG_MSG			  
            rcPrintf ((host, "\n host->var.mem.imc[%x] = Major %x Minor %x  Data %x, host->var.common.socketId = %d\n",Node,  MajorCode, MinorCode, LogData, host->var.common.socketId));
 #endif		  
            if (LogData & 0x80000000) {
                // Node =  ( LogData >> 24 ) & 0x7F ;
                Channel = ( LogData >> 16) & 0xFF;
                Dimm =  ( LogData >> 8) & 0xFF;
            } else { 
                Channel =  0xFF;
                Dimm =   0xFF;
            }

            if(MajorCode == 0) continue; // || MinorCode == 0 ) continue;

            EventMessage.GeneratorId = 0x1A;
            EventMessage.EvMRevision = 0x00;
            EventMessage.SensorNumber = 0xFE;   //FE for purley platform
            EventMessage.EventDirType = 0x6F;	//EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
            EventMessage.OEMEvData1 = MajorCode;
            EventMessage.OEMEvData2 = MinorCode;
            EventMessage.SensorType = 0xCC;
            EventMessage.OEMEvData3 = ( Node << 5 ) |    // Bit5~7:CPU
                                      (Channel << 2 ) |  // Bit2-4 CH
                                      Dimm;

            for(i=0; i<sizeof(SKIP_CODE); i++){
                if(MajorCode == SKIP_CODE[i])
                EventMessage.OEMEvData3 = 0xFF; //Set CPU/CH/DIMM information to 0xFF when the error/warning doesn't contain full information
            }

 #ifdef SERIAL_DBG_MSG														 
            rcPrintf ((host, "Failing DIMM: OemIpmiWarningReport: host->var.mem.imc[Node].logData = 0x%x, EventMessage.OEMEvData3=0x%x\n", LogData, EventMessage.OEMEvData3));
 #endif		  
            ReportErrorDIMMToScreen(host, MajorCode, MinorCode, (UINT8)EventMessage.OEMEvData3, 1);
            ResponseDataSize = sizeof(ResponseData);
            Status = IpmiTransportPpi-> SendIpmiCommand (
                     IpmiTransportPpi,
                     IPMI_NETFN_SENSOR_EVENT,
                     BMC_LUN,
                     IPMI_SENSOR_PLATFORM_EVENT_MESSAGE,
                     (UINT8 *) &EventMessage,
                     sizeof (EventMessage),
                     (UINT8 *) &ResponseData,
                     (UINT8 *) &ResponseDataSize );
            //rcPrintf (host, "\n IpmiTransportPpi status = %x \n",Status);
        }
    }
 }
#endif  //SMCPKG_SUPPORT++
 
/**

  AmiPlatformLogWarningWrapper - OEM hook wrapper to provide common warning output to the
  user interface

  @param host      - pointer to sysHost structure
  @param majorCode - major error/warning code
  @param minorCode - minor error/warning code
  @param logData   - data log

  @retval N/A
**/
VOID
AmiPlatformLogWarningWrapper (
  IN PSYSHOST host,
  IN UINT8    majorCode,
  IN UINT8    minorCode,
  IN UINT32   logData
  )
{

    UINT8 ElinkCount;
	
#if SMCPKG_SUPPORT	 //SMCPKG_SUPPORT++
    EFI_STATUS	Status = EFI_SUCCESS;
    PEI_IPMI_TRANSPORT_PPI *IpmiTransportPpi = (PEI_IPMI_TRANSPORT_PPI *)host->var.common.ipmiTransPpi;
    EFI_PEI_SERVICES  **PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
    UINT8    ResponseDataSize, i, k=0;
    UINT8    ResponseData[2];  
    IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST EventMessage;
    UINT32   j;
    UINT8    l;
    UINT8    SKIP_CODE[] = { ERR_MRC_STRUCT, ERR_READ_MC_FREQ, ERR_SMBUS, ERR_PCU, ERR_NGN, ERR_INTERLEAVE_FAILURE,
                             ERR_SKU_LIMIT, WARN_DIMM_COMPAT, WARN_INTERLEAVE_FAILURE, WARN_DIMM_POP_RUL, WARN_EARLY_RID,
                             WARN_DQ_SWIZZLE_DISC, WARN_MEMORY_TRAINING, WARN_RT_DIFF_EXCEED
                           };

    //
    //Locate ipmi Transport PPI to send commands to BMC.
    //
    // SMC R1.02 >>
    if (!IpmiTransportPpi) {
  	Status = (**PeiServices).LocatePpi (
              PeiServices,
              &gEfiPeiIpmiTransportPpiGuid,
              0,
              NULL,
              &IpmiTransportPpi );
        if ( EFI_ERROR(Status) ) {
            return ;
        }      
    }
    if(host->var.common.socketId == 0) { //BSP, report asap
        // SMC R1.02 <<
        ResponseDataSize = 1;
        EventMessage.GeneratorId = 0x1A;
        EventMessage.EvMRevision = 0x00;
        EventMessage.SensorType = 0xCC;
        EventMessage.SensorNumber = 0xFE;   //FE for purley platform
        EventMessage.EventDirType = 0x6F;	//EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
        EventMessage.OEMEvData1 = majorCode;
        EventMessage.OEMEvData2 = minorCode;
        EventMessage.OEMEvData3 = 0xFF;
        j = (UINT32) ((majorCode << 8) | minorCode);

        //Memory Warning only
        for (i = 0; i < MAX_LOG; i++)
        {
            if ( j == host->var.common.status.log[i].code)
            {
                k = (UINT8)((((host->var.common.status.log[i].data)&0x3FFFFFF)>>19)|(((host->var.common.status.log[i].data)&0x3FFFFF)>>14)|(((host->var.common.status.log[i].data)&0x3FF) >> 8));					  			
                for(l=0; l<sizeof(SKIP_CODE); l++){
                    if(majorCode == SKIP_CODE[l])
                        k = 0xFF; //Set CPU/CH/DIMM information to 0xFF when the error/warning doesn't contain full information
                }
                EventMessage.OEMEvData3 = k; //(socket << 24) | (ch << 16) | (dimm << 8) | rank)
            }
        }

        ReportErrorDIMMToScreen(host, majorCode, minorCode, k, 0);
		
        Status = IpmiTransportPpi-> SendIpmiCommand (
  			IpmiTransportPpi,
  			IPMI_NETFN_SENSOR_EVENT,
  			BMC_LUN,
  			IPMI_SENSOR_PLATFORM_EVENT_MESSAGE,
  			(UINT8 *) &EventMessage,
  			sizeof (EventMessage),
  			(UINT8 *) &ResponseData,
  			(UINT8 *) &ResponseDataSize );
  
    }
    else if((host->var.common.socketId == 1)&&majorCode)
    {
        WriteCpuPciCfgEx (host, 1, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG, (((UINT32)majorCode) << 16) + (UINT32) minorCode);
        WriteCpuPciCfgEx (host, 1, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, logData|0x80000000);
    }
#endif  //SMCPKG_SUPPORT
	
    for( ElinkCount = 0; PlatformLogWarningElink[ElinkCount]; ElinkCount++ ) {
         PlatformLogWarningElink[ElinkCount](host, majorCode,minorCode,logData);
    }
}

/**

  AmiKtiLogWarningWrapper - Wrapper function that provides an interface to handle KTI warnings.

  @param host     - To be filled with the warning log
  @param SocId    - Bit mask, where bit x corresponds to CPU (CPU:0...3). Set to 0xFF if the Warning is system wide.
  @param SocType  - 0: CPU socket. Set to 0xFF if the Warning is system wide.
  @param KtiLink  - Bit mask, where bit x corresponds to CPU KTI port 0/1. Set to 0xFF if the Warning is system wide, 0 if Link Not Applicable.
  @param WarnCode - Warning Code

  @retval None.

**/
VOID
AmiKtiLogWarningWrapper (
  struct sysHost             *host,
  UINT8                      SocId,
  UINT8                      SocType,
  UINT8                      KtiLink,
  UINT8                      WarnCode
  ) {
  
    UINT8 ElinkCount;
	
		
    for( ElinkCount = 0; KtiLogWarningElink[ElinkCount]; ElinkCount++ ) {
         KtiLogWarningElink[ElinkCount](host, SocId,SocType,KtiLink,WarnCode);
    }
}


/**

  AmiPlatformInitializeDataWrapper - OEM hook wrapper to initialize Input structure data if required.

  @param host  - Pointer to sysHost
  @param socket

  @retval N/A

**/
VOID
AmiPlatformInitializeDataWrapper (
  IN OUT PSYSHOST host,
  IN     UINT8 socket
  )
{

    UINT8 ElinkCount;
	
	
    for( ElinkCount = 0; ModifyMrcInputParametersElink[ElinkCount]; ElinkCount++ ) {
        ModifyMrcInputParametersElink[ElinkCount](host, socket);
    }
}

VOID
 AmiOemInitializePlatformData (
	struct sysHost *host
    )
{
    UINT8 ElinkCount;
	
    for( ElinkCount = 0; UpdatePlatformDataElink[ElinkCount]; ElinkCount++ ) {
    	UpdatePlatformDataElink[ElinkCount](host);
    }
}

/**

  AmiOemLookupFreqTableWrapper - OEM hook wrapper for overriding the memory POR frequency table.

  @param host            - Pointer to PSYSHOST
  @param freqTablePtr    - MRC frequency table
  @param freqTableLength - Frequency table length

  @retval N/A

**/
VOID
AmiOemLookupFreqTableWrapper (
  IN     PSYSHOST host,
  IN OUT struct DimmPOREntryStruct **freqTablePtr,
  IN OUT UINT16 *freqTableLength
  )
{

    UINT8 ElinkCount;
	
		
    for( ElinkCount = 0; ModifyMrcSpeedElink[ElinkCount]; ElinkCount++ ) {
        ModifyMrcSpeedElink[ElinkCount](host, freqTablePtr,freqTableLength);
    }
}

/**

  AmiOemKtiChangeParameterWrapper - OEM hook wrapper for changing KTI related parameters.

  @param host   - Pointer to sysHost

  @retval N/A

**/
VOID
AmiOemKtiChangeParameterWrapper (
  IN OUT struct sysHost *host,
  IN KTI_SOCKET_DATA *SocketData,
  IN KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  ) {
  
    UINT8 ElinkCount;
	
		
    for( ElinkCount = 0; ModifyKtiParameterElink[ElinkCount]; ElinkCount++ ) {
        ModifyKtiParameterElink[ElinkCount](host);
    }
}
/**

  AmiOemPublishHobDataWrapper - OEM hook wrapper to put the host structure into HOB variable

  @param host - pointer to sysHost structure on stack

  @retval VOID

**/
VOID
AmiOemPublishHobDataWrapper(
   struct sysHost             *host
   ) {
   
    UINT8 ElinkCount;
		
		
    for( ElinkCount = 0; OemPublishHobDataElink[ElinkCount]; ElinkCount++ ) {
        OemPublishHobDataElink[ElinkCount](host);
    }
}

/**

  OemKtiGetEParamsOverride - OEM hook wrapper to override the EParams

  @param host - pointer to sysHost structure on stack
  @param Ptr - pointer to ALL_LANES_EPARAM_LINK_INFO structure
  @param SizeOfTable - Size of EParam Table
  @retval VOID

**/
VOID
OemKtiGetEParamsOverride(
   struct sysHost             *host,
   ALL_LANES_EPARAM_LINK_INFO **Ptr,
   UINT32                     *SizeOfTable
   ) {
   
    UINT8 ElinkCount;
#if SMCPKG_SUPPORT
   	EFI_STATUS                              Status = EFI_SUCCESS;
    UINT32 Result = TRUE;
    SUPERM_PEI_DRIVER_PROTOCOL				*mSuperMPeiDriver = NULL;
    EFI_PEI_SERVICES  **PeiServices;
    PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

    Status = (**PeiServices).LocatePpi(
                PeiServices,
                &gSuperMPeiPrococolGuid,
                0,
                NULL,
                &mSuperMPeiDriver );

    ASSERT_EFI_ERROR(Status);
    if ( NULL != mSuperMPeiDriver && NULL != mSuperMPeiDriver->GetSuperMEParam ) {
        Result = mSuperMPeiDriver->GetSuperMEParam(
            PeiServices,
            mSuperMPeiDriver,
            SizeOfTable,
            Ptr);
    }
#endif   	
	
    for( ElinkCount = 0; OemKtiGetEParamsElink[ElinkCount]; ElinkCount++ ) {
        OemKtiGetEParamsElink[ElinkCount](host, Ptr, SizeOfTable);
    }
}

/**

  OemGetBoardTypeBitmaskOverride - OEM hook wrapper to override the BoardTypeBitmask

  @param host - pointer to sysHost structure on stack
  @param BoardTypeBitmask
  @retval VOID

**/
VOID
OemGetBoardTypeBitmaskOverride(
   struct sysHost             *host,
   UINT32                     *BoardTypeBitmask
   ) {
   
    UINT8 ElinkCount;
	  
	  
    for( ElinkCount = 0; OemGetBoardTypeBitmaskElink[ElinkCount]; ElinkCount++ ) {
        OemGetBoardTypeBitmaskElink[ElinkCount](host, BoardTypeBitmask);
    }
}


/**

  AmiKtiCheckPointWrapper - OEM hook wrapper for KtiCheckPoint

  @param SocId - Socket Number
  @param SocType - Socket Type
  @param KtiLink - Kti Link Number
  @param MajorCode - Major Code
  @param MinorCode - Minor Code
  @param host - pointer to sysHost structure
  @retval VOID

**/
VOID
AmiKtiCheckPointWrapper (
  UINT8                      SocId,
  UINT8                      SocType,
  UINT8                      KtiLink,
  UINT8                      MajorCode,
  UINT8                      MinorCode,
  struct sysHost             *host
  )
{

     UINT8 ElinkCount;
	 
	 
     for( ElinkCount = 0; KtiCheckPointElink[ElinkCount]; ElinkCount++ ) {
        KtiCheckPointElink[ElinkCount](SocId, SocType, KtiLink,MajorCode,MinorCode,host);
     }
}

/**

  AmiPlatformOutputErrorWrapper - OEM hook wrapper for PlatformOutputError


  @retval VOID

**/
VOID
AmiPlatformOutputErrorWrapper (
  PSYSHOST host,
  UINT8    ErrorCode,
  UINT8    minorErrorCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  ) {
  
    UINT8 ElinkCount;
	
	
    for( ElinkCount = 0; PlatformOutputErrorElink[ElinkCount]; ElinkCount++ ) {
    	PlatformOutputErrorElink[ElinkCount](host, ErrorCode, minorErrorCode,socket,ch,dimm,rank);
    }
}

/**

  AmiPlatformOutputWarningWrapper - OEM hook wrapper for PlatformOutputWarning


  @retval VOID

**/
VOID
AmiPlatformOutputWarningWrapper (
  PSYSHOST host,
  UINT8    warningCode,
  UINT8    minorWarningCode,
  UINT8    socket,
  UINT8    ch,
  UINT8    dimm,
  UINT8    rank
  ) {
  
    UINT8 ElinkCount;
	
	
    for( ElinkCount = 0; PlatformOutputWarningElink[ElinkCount]; ElinkCount++ ) {
	    PlatformOutputWarningElink[ElinkCount](host,warningCode,minorWarningCode,socket,ch,dimm,rank);
    }
}

/**

  OemGetResourceMapUpdateOverride - OEM hook wrapper to override the CPU Resource

  @param Status - pointer to gEfiPeiReadOnlyVariable2PpiGuid status
  @param PeiVariable - pointer to PeiVariable
  @param CpuRes - pointer to CPU_RESOURCE structure
  @param CpuRes - pointer to CPU_RESOURCE structure
  @retval VOID

**/
VOID
OemGetResourceMapUpdateOverride(
   EFI_STATUS                    Status,
   IN struct sysHost             *host,
   EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable,
   IN OUT KTI_CPU_RESOURCE       *CpuRes,
   IN KTI_OEMCALLOUT             SystemParams
   ) {
   
    UINT8 ElinkCount;
	
    for( ElinkCount = 0; OemGetResourceMapUpdateElink[ElinkCount]; ElinkCount++ ) {
        OemGetResourceMapUpdateElink[ElinkCount](Status, host, PeiVariable, CpuRes, SystemParams);
    }
}

/**

  AmiTurnAroundTimingsWrapper - OEM hook wrapper to override TurnAroundTimings

  @param host   - Pointer to sysHost
  @param socket - Socket number
  
  @retval VOID

**/

VOID
AmiTurnAroundTimingsWrapper(
    PSYSHOST host,
    UINT8    socket		
) {
	UINT8 ElinkCount;
		
	for( ElinkCount = 0; TurnAroundTimingsElink[ElinkCount]; ElinkCount++ ) {
		TurnAroundTimingsElink[ElinkCount]( host, socket);
	}
}

VOID
X11DPTBGetResourceMapUpdateOverride(
   EFI_STATUS                    Status,
   IN struct sysHost             *host,
   EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable,
   IN OUT KTI_CPU_RESOURCE   *CpuRes,
   IN KTI_OEMCALLOUT         SystemParams
   )
{
  CpuRes[0].BusBase = 0;
  CpuRes[0].BusLimit = 0x7F;

  CpuRes[0].StackRes[0].BusBase = 0x00;
  CpuRes[0].StackRes[0].BusLimit = 0x0F;
  CpuRes[0].StackRes[1].BusBase = 0x10;
  CpuRes[0].StackRes[1].BusLimit = 0x16;
  CpuRes[0].StackRes[2].BusBase = 0x17;
  CpuRes[0].StackRes[2].BusLimit = 0x76;
  CpuRes[0].StackRes[3].BusBase = 0x77;
  CpuRes[0].StackRes[3].BusLimit = 0x7F;
  CpuRes[0].StackRes[4].BusBase = 0xFF;
  CpuRes[0].StackRes[4].BusLimit = 0x00;
  CpuRes[0].StackRes[5].BusBase = 0xFF;
  CpuRes[0].StackRes[5].BusLimit = 0x00;

  CpuRes[1].BusBase = 0x80;
  CpuRes[1].BusLimit = 0xFF;

  CpuRes[1].StackRes[0].BusBase = 0x80;
  CpuRes[1].StackRes[0].BusLimit = 0x84;
  CpuRes[1].StackRes[1].BusBase = 0x85;
  CpuRes[1].StackRes[1].BusLimit = 0x8F;
  CpuRes[1].StackRes[2].BusBase = 0x90;
  CpuRes[1].StackRes[2].BusLimit = 0xEF;
  CpuRes[1].StackRes[3].BusBase = 0xF0;
  CpuRes[1].StackRes[3].BusLimit = 0xFF;
  CpuRes[1].StackRes[4].BusBase = 0xFF;
  CpuRes[1].StackRes[4].BusLimit = 0x00;
  CpuRes[1].StackRes[5].BusBase = 0xFF;
  CpuRes[1].StackRes[5].BusLimit = 0x00;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
