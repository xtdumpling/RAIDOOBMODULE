//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03       Move no memory error beep into function OemPlatformFatalError.
//      Bug Fixed:
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
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
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
	UINT8 NumOfRow = 0x11;
	UINT8 NumOfCol = 0x00;
    EFI_STATUS Status;
	UINT16 WARN_CODE[] = {	0x0702, 0x0705, 0x0706, 0x0708, 0x070A, 0x0902, 0x0A00, 0x0C02, 0x0E01, 0x0E02, 
				0x0E03, 0x0E04, 0x0E05, 0x1401, 0x1501, 0x1600, 0x1701, 0x2600, 0x3000, 0x3013,
				0x3014, 0x3015, 0x3016, 0x301C, 0x3024, 0x3100, 0x3113, 0x3114, 0x3115, 0x3116,
				0x311C, 0x3124, 0x3125, 0xE801, 0xE802, 0xED01, 0xED02, 0xED03, 0xED04, 0xED05,
				0xED06, 0xED07, 0xED08, 0xEF01, 0xEF02, 0xF001};
	UINT8 Index1;
	
	Status = (**PeiServices).LocatePpi(
                PeiServices,
                &gEfiPeiSimpleTextOutPPIGuid,
                0,
                NULL,
                &mSimpleTextOutPpi);
if(Status==EFI_SUCCESS){	
	//OpenVgaDecode();
#ifdef SERIAL_DBG_MSG
	rcPrintf ((host, "Failing DIMM: pipe = %x\n", pipe));
#endif	
	if (pipe)	NumOfRow+=2;
	
	for ( Index1 = 0; Index1 < sizeof(WARN_CODE)/sizeof(WARN_CODE[0]); Index1++ ){
		if ( WARN_CODE[Index1] != ((majorCode << 8) | minorCode))	continue;
#ifdef SERIAL_DBG_MSG      
		rcPrintf ((host, "Failing DIMM: Fatal & Warning Code detected = %x, logData = %x \n", WARN_CODE[Index1], logData));
#endif		
 
        switch (WARN_CODE[Index1]) {
         case 0x0702:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Max number of ranks exceeded on the channel.");
        	 break;    
         case 0x0705:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "The number of ranks on this device is not supported.");
        	 break;    
         case 0x0706:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "This DIMM does not support DDR4-1333 or higher.");
        	 break;    
         case 0x0708:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Channel configuration is not supported.");
        	 break;    
         case 0x070A:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "DDR4 voltage is not supported.");
        	 break;         	 
         case 0x0902:
        	 //OutputString (00, NumOfRow, 0x0F, "Channel DIMM configuration does not support Lockstep mode.");
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Failed to honor Lockstep mode");        	 
        	 break;    
         case 0x0A00:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "(runtime)Failing DIMM: DIMM location");
        	 break;     	 
         case 0x0C02:
        	 //OutputString (00, NumOfRow, 0x0F, "Channel DIMM configuration does not support Mirror mode.");
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Failed to honor Mirror mode");        	 
        	 break;      
         case 0x0E01:
         case 0x0E02:
         case 0x0E03:
         case 0x0E04:
         case 0x0E05:        	 
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Failed to honor Interleave mode");
        	 break;   
         case 0x1401:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Memory training failure.");
        	 break;   
         case 0x1501:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Memory training failure.");
        	 break;   
         case 0x1600:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Memory training failure.");
        	 break;           	 
         case 0x1701:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Incorrect memory DIMM population.");
        	 break;  
         case 0x2600:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Memory signal is too marginal.");
        	 break;  
         case 0x3000:
         case 0x3013:
         case 0x3014:
         case 0x3015:
         case 0x3016:
         case 0x301C:
         case 0x3024:        	 
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Failing DIMM: DIMM location (Correctable memory component found)");
        	 break;       
         case 0x3100:
         case 0x3113:
         case 0x3114:
         case 0x3115:
         case 0x3116:
         case 0x311C:
         case 0x3124:        	 
         case 0x3125:        	 
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Failing DIMM: DIMM location (Uncorrectable memory component found)");
        	 break;          	 
         case 0xE801:
         case 0xE802:        	 
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "No memory DIMM detected, install memory DIMMs.");
        	 break;   
         case 0xED01:
         case 0xED02:
         case 0xED03:        	 
         case 0xED04:
         case 0xED05:
         case 0xED06:
         case 0xED07:
         case 0xED08:        	 
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Incorrect memory DIMM population, check Memory Population Rule.");
        	 break; 
         case 0xEF01:
         case 0xEF02:        	 
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Memory initialization panic, apply AC Reset.");
        	 break; 
         case 0xF001:
        	 mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "Failed to program memory voltage regulator.");
        	 break;      	 
         default: // For 0xA/0x30/0x31

        	 break;
         }
	 NumOfRow = (NumOfRow + 0x01);
	 
	if ( logData != 0xFF )
	{    
#ifdef SERIAL_DBG_MSG		
		rcPrintf ((host, "Failing DIMM: DIMM logdata = %x\n", logData));		 
#endif		
		// Bit7-5:CPU, 4-2: Channel 1-0: DIMM
			mSimpleTextOutPpi->OutputString (00, NumOfRow, 0x0F, "  P");
			character[0]='1' + (CHAR8)((logData >> 5) & 0x07);
//SGI+			
			mSimpleTextOutPpi->OutputString (3, NumOfRow, 0x0F, character);
//			mSimpleTextOutPpi->OutputString (4, NumOfRow, 0x0F, "-DIMM");
            mSimpleTextOutPpi->OutputString (4, NumOfRow, 0x0F, "-D");
			character[0]='0' + (CHAR8)((logData >> 2) & 0x07) + (CHAR8)(((logData >> 5) & 0x07) * 4);
			mSimpleTextOutPpi->OutputString (6, NumOfRow, 0x0F, character);
//			character[0]='1' + (CHAR8)(logData & 0x03);
//			mSimpleTextOutPpi->OutputString (10, NumOfRow, 0x0F, character);
//SGI-    
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
  PEI_IPMI_TRANSPORT_PPI          *IpmiTransportPpi = (PEI_IPMI_TRANSPORT_PPI          *)host->var.common.ipmiTransPpi;
 
  UINT8               ResponseDataSize, i, k=0xFF, Index1;
  UINT8               ResponseData[2];  
  IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST         EventMessage;
  UINT32			  j=0;
  BOOLEAN		MRC_ERROR_FOUND = FALSE;
  UINT16 WARN_CODE[] = {	0x0702, 0x0705, 0x0706, 0x0708, 0x070A, 0x0902, 0x0A00, 0x0C02, 0x0E01, 0x0E02, 
  				0x0E03, 0x0E04, 0x0E05, 0x1401, 0x1501, 0x1600, 0x1701, 0x2600, 0x3000, 0x3013,
  				0x3014, 0x3015, 0x3016, 0x301C, 0x3024, 0x3100, 0x3113, 0x3114, 0x3115, 0x3116,
  				0x311C, 0x3124, 0x3125, 0xE801, 0xE802, 0xED01, 0xED02, 0xED03, 0xED04, 0xED05,
  				0xED06, 0xED07, 0xED08, 0xEF01, 0xEF02, 0xF001};  

  
  //if ( majorCode == ERR_NO_MEMORY )
  //	  SmcPostErrorNoMemBeep();
  //
  //Locate ipmi Transport PPI to send commands to BMC.
  //
 
  if(IpmiTransportPpi && host->var.common.socketId == 0) { //BSP, report asap  
  
	  Status = (**PeiServices).LocatePpi (
              PeiServices,
              &gEfiPeiIpmiTransportPpiGuid,
              0,
              NULL,
              &IpmiTransportPpi );

	  if(Status == EFI_SUCCESS) {
		  ResponseDataSize = 1;
		  EventMessage.GeneratorId = 0x1A;
		  EventMessage.EvMRevision = 0x00;
		  EventMessage.SensorNumber = 0xFF;
		  EventMessage.EventDirType = 0x6F;	//EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;

		  EventMessage.OEMEvData1 = majorCode;
		  EventMessage.OEMEvData2 = minorCode;

		  for ( Index1 = 0; Index1 < sizeof(WARN_CODE)/sizeof(WARN_CODE[0]); Index1++ )
		  {
			  if ( WARN_CODE[Index1] == ((majorCode << 8) | minorCode))	// if MRC error
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
						EventMessage.OEMEvData3 = k; //(socket << 24) | (ch << 16) | (dimm << 8) | rank)
#ifdef SERIAL_DBG_MSG							
						rcPrintf ((host, "Failing DIMM: data = 0x%x, k=0x%x\n", host->var.common.status.log[i].data, k));
#endif						
						ReportErrorDIMMToScreen(host, majorCode, minorCode, k, 0);
		                  } else {// QPI error
		                 	  EventMessage.SensorType = 0xCB;
		                 	  EventMessage.OEMEvData3 = 0xFF;
		                  }
			  }
		     Status = IpmiTransportPpi-> SendIpmiCommand (
  			IpmiTransportPpi,
  			IPMI_NETFN_SENSOR_EVENT,
  			BMC_LUN,
  			IPMI_SENSOR_PLATFORM_EVENT_MESSAGE,
  			(UINT8 *) &EventMessage,
  			sizeof (EventMessage),
  			(UINT8 *) &ResponseData,
  			(UINT8 *) &ResponseDataSize );
		  }// if (MRC_ERROR_FOUND)
  	  }	//if(Status == EFI_SUCCESS) 
  }	//if(IpmiTransportPpi && host->var.common.socketId == 0)

#endif //SMCPKG_SUPPORT	 
	
    for( ElinkCount = 0; PlatformFatalErrorElink[ElinkCount]; ElinkCount++ ) {
      PlatformFatalErrorElink[ElinkCount](host,majorCode,minorCode,haltOnError);
    }
 }

#if SMCPKG_SUPPORT		//SMCPKG_SUPPORT++
 
 void
 OemIpmiWarningReport(PSYSHOST host)
 {
   PEI_IPMI_TRANSPORT_PPI          *IpmiTransportPpi = (PEI_IPMI_TRANSPORT_PPI          *)host->var.common.ipmiTransPpi;
   UINT8               ResponseDataSize;
   UINT8               ResponseData[10];  
   IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST         EventMessage;
   UINT8    MajorCode;
   UINT8    MinorCode;
   UINT32   LogData;
   UINT8			  Node, Channel, Dimm;
   EFI_STATUS Status;
   
 #ifdef SERIAL_DBG_MSG	
   rcPrintf ((host, "\n IpmiTransportPpi = 0x%x \n",IpmiTransportPpi ));
 #endif  
   if(IpmiTransportPpi) {

 /*
 Description	Generator ID[8:9]	Sensor Type[11]	Sensor Number[12]	Event Type[13]	Event Data1[14]	Event Data2[15]	Event Data3[16]
 BIOS MRC Error	0x1A 0x00	        0xCA	             0XFF	                           0x6F	                    MajorCode	MinorCode 	Bit0~1:Dimm Bit2~4:Channel Bit5~7:CPU
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

 	      if(MajorCode == 0) continue;		// || MinorCode == 0 ) continue;

               EventMessage.GeneratorId = 0x1A;
               EventMessage.EvMRevision = 0x00;
         
               EventMessage.SensorNumber = 0xFF;
               EventMessage.EventDirType = 0x6F;	//EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
         
               EventMessage.OEMEvData1 = MajorCode;
               EventMessage.OEMEvData2 = MinorCode;
             
             
               EventMessage.SensorType = 0xCC;
 		EventMessage.OEMEvData3 = ( Node << 5 ) |  // Bit5~7:CPU
                                                   (Channel << 2 ) |  // Bit2-4 CH
                                                      Dimm;
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
 //      	       rcPrintf (host, "\n IpmiTransportPpi status = %x \n",Status);
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
  PEI_IPMI_TRANSPORT_PPI          *IpmiTransportPpi = (PEI_IPMI_TRANSPORT_PPI          *)host->var.common.ipmiTransPpi;
  EFI_PEI_SERVICES  **PeiServices  = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  UINT8               ResponseDataSize, i, k=0;
  UINT8               ResponseData[2];  
  IPMI_PLATFORM_EVENT_MESSAGE_DATA_REQUEST         EventMessage;
  UINT32			  j;

  
  
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
      EventMessage.SensorNumber = 0xFF;
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
			//k = (UINT8)((((host->var.common.status.log[i].data)&0x3FFFF)>>11)|(((host->var.common.status.log[i].data)&0x3FF)>>3)|(((host->var.common.status.log[i].data)&0xFF)));
			k = (UINT8)((((host->var.common.status.log[i].data)&0x3FFFFFF)>>19)|(((host->var.common.status.log[i].data)&0x3FFFFF)>>14)|(((host->var.common.status.log[i].data)&0x3FF) >> 8));					  			
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

    Status = mSuperMPeiDriver->GetSuperMEParam(
                PeiServices,
                mSuperMPeiDriver,
                SizeOfTable,
				Ptr);
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
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
