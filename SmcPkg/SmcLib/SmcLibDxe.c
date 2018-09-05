//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Function for get MAC from VPD.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Aug/09/2017
//
//  Rev. 1.02
//    Bug Fix:  Add function GetPurleyCPUType
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Jun/08/2017
//
//  Rev. 1.01
//    Bug Fix:  Review BMC OEM command.
//    Reason:   Add function GetSystemConfig
//    Auditor:  Jimmy Chiu
//    Date:     Jul/28/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************
//****************************************************************************
#include "Setup.h"
#include "Token.h"
#include "EFI.h"
#include <AmiDxeLib.h>
#include <Library/DebugLib.h>
#include <Library\BaseMemoryLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include "SmcLibBmc.h"
#include "SuperMDxeDriver.h"
#include "SmcLibBmcPrivate.h"

#include "UncoreCommonIncludes.h"
#include <Protocol/IioSystem.h> 
#include <Library/HobLib.h>
#include <KtiHost.h>
#include <Library/MmPciBaseLib.h>

#ifndef EFI_SM_INTEL_OEM
#define EFI_SM_INTEL_OEM    0x30
#endif

EFI_GUID	gEfiSuperMDriverProtocolGuid = EFI_SUPERM_DRIVER_PROTOCOL_GUID;
SUPERMDRIVER_PROTOCOL *gSuperMProtocol = NULL;

extern EFI_BOOT_SERVICES *gBS;
extern EFI_RUNTIME_SERVICES *gRT;





//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  SMC_IPMICmd
//
// Description :  Send command to IPMI
//
// Parameters:    IN  UINT8          NetFunction,
//                IN  UINT8          Lun,
//                IN  UINT8          Command,
//                IN  UINT8          *CommandData,
//                IN  UINT8          CommandDataSize,
//                OUT UINT8         *ResponseData,
//                OUT UINT8         *ResponseDataSize
//
// Returns     :  EFI_STATUS Status
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>

EFI_STATUS
SMC_IPMICmd(
  IN  UINT8          NetFunction,
  IN  UINT8          Lun,
  IN  UINT8          Command,
  IN  UINT8          *CommandData,
  IN  UINT8          CommandDataSize,
  OUT UINT8         *ResponseData,
  OUT UINT8         *ResponseDataSize
)	
{
    EFI_STATUS	           Status;
    EFI_IPMI_TRANSPORT	   *IpmiTransport = NULL;
    DEBUG((-1, "SMC_IPMICmd entry.\n"));
 
 	Status = gBS->LocateProtocol(
			&gEfiDxeIpmiTransportProtocolGuid,
			NULL,
			&IpmiTransport);
			   
    if(IpmiTransport == NULL ){
       DEBUG((-1, "Cannot locate IPMI ret.\n"));
       return EFI_NOT_READY;
    }   
    DEBUG((-1, "Netfun0x%x,Lun=0x%x cmd=0x%x\n", NetFunction,Lun,Command));
    
    Status = IpmiTransport->SendIpmiCommand(
		         IpmiTransport,
		         NetFunction,		// EFI_SM_INTEL_OEM, 
		         Lun,				// LUN
		         Command,			
		         CommandData,	    // *CommandData
		         CommandDataSize,	// Size of CommandData
		         ResponseData,		// *ResponseData
		         ResponseDataSize);		// ResponseDataSize

 
    DEBUG((-1, "SMC_IPMICmd end. %r\n", Status));
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetLanMacInfo 
//
// Description :  Get Lan Mac from SuperMboardInfo 
//
// Parameters  :    
//                IN OUT UINT8   *LanMacData,
//
// Returns     :  EFI_STATUS      Status
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
GetLanMacInfo(
  IN OUT UINT8         *LanMacData
){
   EFI_STATUS       Status = EFI_SUCCESS;
   SUPERMBOARDINFO  gSuperMBoardInfo; 
 
   Status = gBS->LocateProtocol(&gEfiSuperMDriverProtocolGuid, NULL, &gSuperMProtocol);
   if ( Status != EFI_SUCCESS){
      DEBUG((-1, "Cannot locate SuperM ret.Status=0x%x\n",Status));
      return EFI_NOT_READY;
   }
   ZeroMem(&gSuperMBoardInfo,sizeof(SUPERMBOARDINFO));
   Status = gSuperMProtocol->GetSuperMBoardInfo(&gSuperMBoardInfo);
   //Support Max Lan number is 12 and a byte of lan num 
   gBS->CopyMem(LanMacData,gSuperMBoardInfo.LanMacInfo,sizeof(gSuperMBoardInfo.LanMacInfo) );
   
   return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetHardwareInfo 
//
// Description :  Get HardwareInfo from SuperMboardInfo 
//
// Parameters  :    
//                IN OUT UINT8   *HardWareData,
//
// Returns     :  EFI_STATUS      Status
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
GetHardwareInfo(
  IN OUT UINT8         *HardWareData
){
   EFI_STATUS       Status = EFI_SUCCESS;
   SUPERMBOARDINFO  gSuperMBoardInfo; 
 
   Status = gBS->LocateProtocol(&gEfiSuperMDriverProtocolGuid, NULL, &gSuperMProtocol);
   if ( Status != EFI_SUCCESS){
      DEBUG((-1, "Cannot locate SuperM ret.Status=0x%x\n",Status));
      return EFI_NOT_READY;
   }
   ZeroMem(&gSuperMBoardInfo,sizeof(SUPERMBOARDINFO));
   Status = gSuperMProtocol->GetSuperMBoardInfo(&gSuperMBoardInfo);
   //Hardware Info in DXE contain MAC address from LAN1.
   gBS->CopyMem(HardWareData,gSuperMBoardInfo.HardwareInfo,sizeof(gSuperMBoardInfo.HardwareInfo) );
   
   return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetSystemConfig 
//
// Description :  Get SystemConfiguration from SuperMboardInfo 
//
// Parameters  :    
//                IN OUT UINT8   *SysConfigData,
//
// Returns     :  EFI_STATUS      Status
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
GetSystemConfig(
  IN OUT UINT8         *SysConfigData
){
   EFI_STATUS       Status = EFI_SUCCESS;
   SUPERMBOARDINFO  gSuperMBoardInfo; 
 
   Status = gBS->LocateProtocol(&gEfiSuperMDriverProtocolGuid, NULL, &gSuperMProtocol);
   if ( Status != EFI_SUCCESS){
      DEBUG((-1, "Cannot locate SuperM ret.Status=0x%x\n",Status));
      return EFI_NOT_READY;
   }
   ZeroMem(&gSuperMBoardInfo,sizeof(SUPERMBOARDINFO));
   Status = gSuperMProtocol->GetSuperMBoardInfo(&gSuperMBoardInfo);
   //SystemConfiguration Info in DXE contain M/B attributes.
   gBS->CopyMem(SysConfigData,gSuperMBoardInfo.SystemConfiguration,sizeof(gSuperMBoardInfo.SystemConfiguration) );
   
   return Status;
}
//****************************************************************************

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  CheckPurleyFPGACPU 
//
// Description :  Check if the CPU is FPGA CPU. 
//
// Parameters  :  None
//
// Returns     :  UINT32
//					Bit[1:0] -> Socket[0]: 0 - normal CPU; 1 -  FPGA CPU; 2 - WolfRiver CPU
//					Bit[3:2] -> Socket[1]: 0 - normal CPU; 1 -  FPGA CPU; 2 - WolfRiver CPU
//					Bit[5:4] -> Socket[2]: 0 - normal CPU; 1 -  FPGA CPU; 2 - WolfRiver CPU
//					Bit[7:6] -> Socket[3]: 0 - normal CPU; 1 -  FPGA CPU; 2 - WolfRiver CPU
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT32
GetPurleyCPUType()
{
	UINT32					returnByte = 0;
	IIO_GLOBALS				*IioGlobalData = NULL;
	EFI_PEI_HOB_POINTERS	HobPtr;

	UINT32 i;

	HobPtr.Guid = GetFirstGuidHob (&gIioPolicyHobGuid);
	IioGlobalData = GET_GUID_HOB_DATA (HobPtr.Guid);

	DEBUG((-1, "IioGlobalData = 0x%x\n", IioGlobalData));

	if ( NULL != IioGlobalData ) {
		for(i=0; i<MAX_SOCKET; i++) {
			switch(IioGlobalData->IioVar.IioVData.SkuPersonality[i]) {
				case TYPE_FPGA:
					returnByte |= 1<<(i<<1);
					break;
				case TYPE_MCP:
					returnByte |= 2<<(i<<1);
					break;
				default:
					break;
			}
		}
	}
	
	DEBUG((-1, "returnByte = 0x%x\n", returnByte));

	return returnByte;
}


static void GetVPDData(UINT8* buffer, UINT32 iSize, UINT16 BDF, UINT8 cappointer)
{
	VOLATILE UINT32* pData;
	VOLATILE UINT16* pIndex;
	
	UINT32 iData;
	UINT16 iIndex;

	UINT32 i;
	
	pData = (UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), cappointer+4);
	pIndex = (UINT16*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), cappointer+2);

#ifdef EFI_DEBUG
	DEBUG((-1, "pData=0x%x, pIndex=0x%x\n", pData, pIndex));
#endif

	for(i=0; i<iSize; i=i+4) {
		*pIndex = i;

		iIndex = *pIndex;

#ifdef EFI_DEBUG
		DEBUG((-1, "iIndex=0x%x\n", iIndex));
#endif

		while(!(iIndex&0x8000)){
#ifdef EFI_DEBUG
			DEBUG((-1, "iIndex=0x%x\n", iIndex));
#endif
			iIndex = *pIndex;
		}

		iData = *pData;

#ifdef EFI_DEBUG
		DEBUG((-1, "iData=0x%x\n", iData));
#endif

		*(UINT32*)(&(buffer[i])) = iData;
	}
}

static UINT8 GetVPDCap(UINT16 PFA, UINT8 capoff)
{
	UINT32 temp = (UINT32)PFA;

	UINT32 pciaddr = 0x80000000 + (((temp>>8)&0xff)<<16) + (((temp>>3)&0x1f)<<11) + ((temp&0x7)<<8);

	pciaddr = pciaddr+(UINT32)capoff;

#ifdef EFI_DEBUG
	DEBUG((-1, "pciaddr=0x%x\n", pciaddr));
#endif

	IoWrite32(0xcf8, pciaddr);
	temp = IoRead32(0xcfc);

#ifdef EFI_DEBUG
	DEBUG((-1, "temp=0x%x\n", temp));
#endif

	if ( capoff == 0x34 ) {

#ifdef EFI_DEBUG
		DEBUG((-1, "here\n"));
#endif

		return GetVPDCap(PFA, (UINT8)(temp&0xff));
	}
	else if ( (temp&0xff) == 0x03 ) {
		return capoff;
	}
	else {
		if ( (temp&0xff00) != 0 ) {
#ifdef EFI_DEBUG
			DEBUG((-1, "here\n"));
#endif
			return GetVPDCap(PFA, (UINT8)((temp>>8)&0xff));
		}
		else { 
			return 0;
		}
	}
}

static void PrintData(UINT8* ptr, UINT32 iSize)
{

	UINT32 i = 0, j = 0, k;
	UINT8 arr[16];
	DEBUG((-1, "\n====================================================================\n"));
	DEBUG((-1, "     00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\n"));
	DEBUG((-1, "--------------------------------------------------------------------"));
	for(i=0; i<iSize; i++) {
	        if ( (i%16) == 0 ) {
	                j = 0;

	                if ( i != 0 ) {
	                        for(k=0; k<16;k++) {
	                                if ( arr[k] == 0 )
	                                        DEBUG((-1, " "));
	                                else
	                                        DEBUG((-1, "%c", arr[k]));
	                        }
	                }
	                DEBUG((-1, "\n%02X0  ", i/16));
	        }
	        arr[j] = ptr[i];
	        j++;
	        DEBUG((-1, "%02X  ", ptr[i]));
	}
	DEBUG((-1, "\n====================================================================\n"));

}

static void ConvertASCII2HEX(UINT8* desbuffer, UINT8* sourcebuffer, UINT32 ioff, UINT32 iLen)
{
	UINT32 i;
	UINT8 var8, var81;

#ifdef EFI_DEBUG
	DEBUG((-1, "ConvertASCII2HEX:"));
#endif
	
	for(i=ioff; i<(iLen+ioff); i=i+2) {
		var8 = sourcebuffer[i];
		var81 = sourcebuffer[i+1];
#ifdef EFI_DEBUG
		DEBUG((-1, "[0x%x, 0x%x]", var8, var81));
#endif
		if((var8>='0')&&(var8<='9'))
			var8 -= '0';
		else if((var8>='A')&&(var8<='F'))
			var8 -= 'A'-10;

		if((var81>='0')&&(var81<='9'))
			var81 -= '0';
		else if((var81>='A')&&(var81<='F'))
			var81 -= 'A'-10;

		*desbuffer = (var8<<4)|var81;
#ifdef EFI_DEBUG
		DEBUG((-1, "(0x%x)", *desbuffer));
#endif
		desbuffer++;
	}

#ifdef EFI_DEBUG
	DEBUG((-1, "\n"));
#endif
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  SmcGetMacFrVpd 
//
// Description :  Get VPD MAC address. 
//
// Parameters  :  BDF - Bus Device fn
//				LanNum - which lan device? 1st 2nd 3rd 4th 
//
// Returns     :  
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID SmcGetMacFrVpd (UINT16 BDF, UINT16 LanNum, UINT8 *LanMacAddressStart)
{
#ifdef EFI_DEBUG
	UINT8* ptr = NULL;
#endif
	UINT8 buffer[256];
	
	UINT8 cappointer = 0x34;

	UINT32 i, va = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0;

	cappointer = GetVPDCap(BDF, cappointer);

#ifdef EFI_DEBUG
	DEBUG((-1, "cappointer=0x%x\n", cappointer));
#endif

	if ( cappointer != 0 ) {
		GetVPDData(buffer, sizeof(buffer), BDF, cappointer);
		
#ifdef EFI_DEBUG
		PrintData(buffer, sizeof(buffer));
#endif

		for(i=0; i<sizeof(buffer); i++) {
			if ( buffer[i] == 'V' && buffer[i+1] == 'A' ) {
				if ( buffer[i+2] == 1 ) {
					va = buffer[i+3] - '0';
					DEBUG((-1, "VA=0x%x V2=0x%x V3=0x%x V4=0x%x V5=0x%x\n", va, v2, v3, v4, v5));
				}
			}

			if ( buffer[i] == 'V' && buffer[i+1] == '2' ) {
				if ( buffer[i+2] == 0x0c ) {
					v2 = i+3;
					DEBUG((-1, "VA=0x%x V2=0x%x V3=0x%x V4=0x%x V5=0x%x\n", va, v2, v3, v4, v5));
				}
			}

			if ( buffer[i] == 'V' && buffer[i+1] == '3' ) {
				if ( buffer[i+2] == 0x0c ) {
					v3 = i+3;
					DEBUG((-1, "VA=0x%x V2=0x%x V3=0x%x V4=0x%x V5=0x%x\n", va, v2, v3, v4, v5));
				}
			}

			if ( buffer[i] == 'V' && buffer[i+1] == '4' ) {
				if ( buffer[i+2] == 0x0c ) {
					v4 = i+3;
					DEBUG((-1, "VA=0x%x V2=0x%x V3=0x%x V4=0x%x V5=0x%x\n", va, v2, v3, v4, v5));
				}
			}

			if ( buffer[i] == 'V' && buffer[i+1] == '5' ) {
				if ( buffer[i+2] == 0x0c ) {
					v5 = i+3;
					DEBUG((-1, "VA=0x%x V2=0x%x V3=0x%x V4=0x%x V5=0x%x\n", va, v2, v3, v4, v5));
				}
			}
		}

		DEBUG((-1, "VA=0x%x V2=0x%x V3=0x%x V4=0x%x V5=0x%x VpdOfs=0x%x\n", va, v2, v3, v4, v5, LanNum));

#ifdef EFI_DEBUG
		PrintData(buffer, sizeof(buffer));

		ptr = LanMacAddressStart;
		DEBUG((-1, "LanMacAddressStart: "));
		for(i=0;i<6; i++) {
			DEBUG((-1, " 0x%x ", *ptr));
			ptr++;
		}
		DEBUG((-1, "\n"));

		DEBUG((-1, "buffer[v2]: "));
		for(i=v2;i<(v2+12); i++) {
			DEBUG((-1, " [0x%x:0x%x] ", i, buffer[i]));
		}
		DEBUG((-1, "\n"));

		DEBUG((-1, "buffer[v3]: "));
		for(i=v3;i<(v3+12); i++) {
			DEBUG((-1, " [0x%x:0x%x] ", i, buffer[i]));
		}
		DEBUG((-1, "\n"));
#endif

		if ( va >= 1 && LanNum == 1 ) {
			ConvertASCII2HEX(LanMacAddressStart, buffer, v2, 0x0c);
		}

		if ( va >= 2 && LanNum == 2 ) {
			ConvertASCII2HEX(LanMacAddressStart, buffer, v3, 0x0c);
		}

		if ( va >= 3 && LanNum == 3) {
			ConvertASCII2HEX(LanMacAddressStart, buffer, v4, 0x0c);
		}

		if ( va >= 4 && LanNum == 4) {
			ConvertASCII2HEX(LanMacAddressStart, buffer, v5, 0x0c);
		}

#ifdef EFI_DEBUG
		ptr = LanMacAddressStart;
		DEBUG((-1, "LanMacAddressStart: "));
		for(i=0;i<6; i++) {
			DEBUG((-1, " 0x%x ", *ptr));
			ptr++;
		}
		DEBUG((-1, "\n"));
#endif

	}
}

