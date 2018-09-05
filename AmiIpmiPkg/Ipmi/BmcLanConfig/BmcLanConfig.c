//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Update BMC LAN config at saving settings of BIOS setup menu.
//    Reason:   System can't get correct IP after modify IPMI LAN configuration in BIOS setup at the first time boot up.
//              Refer to Grantley #2539
//    Auditor:  Van Lee
//    Date:     Aug/11/2017
//  
//  Rev. 1.02   1.Reset setup item "Update IPMI LAN Configuration" to "No" after IPV6 initialized.
//    Bug Fix:
//    Reason:   IPV4 -> IPV6 -> Reset "Update IPMI LAN Configuration" to "No".
//    Auditor:  Jimmy Chiu
//    Date:     Jul/15/2017
//
//  Rev. 1.01
//    Bug Fix:  Review BMC OEM command.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/11/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jun/07/2016
//
//****************************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file BmcLanConfig.c
    The function of the driver is to configure BMC LAN 
    parameters such as Station Ip address, Station MAC
    address, Subnet MAsk,Router IP address and Router MAC
    address. This driver can get these parameters from a
    DHCP server or as user input from BIOS setup.

**/

//---------------------------------------------------------------------

#include <Library/DebugLib.h>
#include "BmcLanConfig.h"
#include "ServerMgmtSetupVariable.h"
#if SMCPKG_SUPPORT
#include <Include/IpmiNetFnAppDefinitions.h>
#endif
//---------------------------------------------------------------------

//
// Global Variables
//
EFI_IPMI_TRANSPORT                  *gIpmiTransport;
SERVER_MGMT_CONFIGURATION_DATA      gServerMgmtConfiguration;


/**
    Set IP address to 0.0.0.0 to memory pointed by ZeroIp

    @param ZeroIp - Pointer to Ip address in BIOS setup variable

    @retval VOID

**/

VOID
SetZeroIp(
  OUT  CHAR16      ZeroIp[] )
{
    
    ZeroIp[0] = 48;
    ZeroIp[1] = 46;
    ZeroIp[2] = 48;
    ZeroIp[3] = 46;
    ZeroIp[4] = 48;
    ZeroIp[5] = 46;
    ZeroIp[6] = 48;
    ZeroIp[7] = 0;
    ZeroIp[8] = 0;
    ZeroIp[9] = 0;
    ZeroIp[10] = 0;
    ZeroIp[11] = 0;
    ZeroIp[12] = 0;
    ZeroIp[13] = 0;
    ZeroIp[14] = 0;

}

/**
    Set MAC address to 00-00-00-00-00-00 to memory pointed by
    ZeroMac

    @param ZeroMac - Pointer to MAC address in BIOS setup variable

    @retval VOID

**/

VOID
SetZeroMac(
  OUT  CHAR16      ZeroMac[] )
{

    ZeroMac[0] = 48;
    ZeroMac[1] = 48;
    ZeroMac[2] = 45;
    ZeroMac[3] = 48;
    ZeroMac[4] = 48;
    ZeroMac[5] = 45;
    ZeroMac[6] = 48;
    ZeroMac[7] = 48;
    ZeroMac[8] = 45;
    ZeroMac[9] = 48;
    ZeroMac[10] = 48;
    ZeroMac[11] = 45;
    ZeroMac[12] = 48;
    ZeroMac[13] = 48;
    ZeroMac[14] = 45;
    ZeroMac[15] = 48;
    ZeroMac[16] = 48;

}

/**
    It will return the String length of the array.

    @param String - It holds the base address of the array.

    @retval Returns length of the char array.

    @note  Function variable description

              String          - It holds the base address of the array.

              Length          - Holds the String length.

**/

UINTN
EfiStrLen (
  IN CHAR16   *String )
{
    INTN Length = 0;

    while(*String++) Length++;

    return Length;
}

/**
    Validate the IP address, and also convert the string
    IP to decimal value.If the IP is invalid then 0 Ip
    is entered.

    The Source string is parsed from right to left with
    following rules
    1) no characters other than numeral and dot is allowed
    2) the first right most letter should not be a dot
    3) no consecutive dot allowed
    4) values greater than 255 not allowed
    5) not more than four parts allowed

    @param Destination - contains validated IP address in decimal 
                        system
    @param Source      - string IP to be validated

    @return EFI_STATUS

    @note  Function variable description

              Index           - Counter for for loop and store the length
                                of the source array initially.
              LookingIp       - IP address is taken into four parts, one 
                                part by one will be extracted from the
                                string and saved to Destination variable.
                                LookingIp variable contains which IP part
                                currently we are extracting.
              SumIp           - Contains sum of the digit of an IP address
                                multiplied by its base power. SumIp=
                               (unit digit * 1) + (Tenth digit * 10) +
                               (hundredth digit * 100)
              IpDigit         - base power of the digit we are extracting
              DigitCount      - digit number - 1 we are extracting in a 
                                IP part
              IsIpValid       - flag to set if an invalid IP is entered
                                and break the loop.
              GotTheFirstDigit- flag is set when the first decimal value is
                                found in the string,So if a dot is
                                encountered first, immediately the loop can
                                be terminated.
              TotalSumIp      - sum of all 4 SumIp part of an IP address.
                                this variable is used to avoid
                                000.000.000.000 IP case in the
                                BmcLanConfiguration.intial value is set to
                                zero.if sum of all 4 SumIp part of an IP
                                address is zero then that is invalid
                                input.

**/

EFI_STATUS
ConvertToIpAddress(
  OUT  UINT8      Destination[],
  IN  CHAR16      Source[] )
{
    INT8       Index;
    INT8       LookingIp;
    UINT8      SumIp;
    UINT8      IpDigit;
    UINT8      DigitCount;
    UINT8      IsIpValid;
    UINT8      GotTheFirstDigit;
    UINT16     TotalSumIp;
  
    LookingIp = 3;
    SumIp = 0;
    IpDigit = 1;
    DigitCount = 0;
    IsIpValid = 1;
    GotTheFirstDigit = 0;
    TotalSumIp = 0 ;

    Index = (INT8) EfiStrLen (Source);

    //
    //Assigning index = 15 because it is not returning 15 as strlen if ip is
    //like xxx.xxx.xxx.xxx this.
    //

    if ( Index > 15) {

        Index = 15;

    }

    for (Index = (Index-1); Index >= 0; Index--) {

        if (Source[Index] <= 57 && Source[Index] >= 48) {

            GotTheFirstDigit = 1;

            if (DigitCount > 2) {
                IsIpValid = 0;
                break;
            }

            if (DigitCount == 2) {

                if (Source[Index] - 48 > 2) {

                    IsIpValid = 0;
                    break;

                }

                if (Source[Index] - 48 == 2 && ((Source[Index + 1] - 48 > 5) ||
                (Source[Index + 1] - 48 == 5 && Source[Index + 2] - 48 > 5))) {
                    
                    IsIpValid = 0;
                    break;

                } 

            }
            SumIp = SumIp + ((UINT8)Source[Index] - 48) * IpDigit;
            IpDigit = IpDigit * 10;
            DigitCount = DigitCount + 1;

        } else if (Source[Index] == 46) {

            if (GotTheFirstDigit == 0 || Source[Index + 1] == 46) {

                IsIpValid = 0;
                break;

            }
            Destination[LookingIp] = SumIp;
            TotalSumIp = TotalSumIp + SumIp;
            LookingIp--;

            if(LookingIp<0) {
                IsIpValid = 0;
                break;
            }

            SumIp = 0;
            IpDigit = 1;
            DigitCount = 0;
        } else if (Source[Index] != 0 ||
            (Source[Index] == 0 && GotTheFirstDigit == 1)) {

            IsIpValid = 0;
            break;
        }
    }

    if (LookingIp == 0) {

        Destination[LookingIp] = SumIp;
        TotalSumIp = TotalSumIp + SumIp;

    } 

    if(LookingIp !=0 || IsIpValid == 0 || TotalSumIp == 0) {

        Destination[0] =0;
        Destination[1] =0;
        Destination[2] =0;
        Destination[3] =0;
        return EFI_INVALID_PARAMETER;

    }

    return EFI_SUCCESS;

}

/**
    Validate the MAC address, and also convert the string MAC to
    decimal value.If the MAC is invalid then 0 MAC is entered.

    The Source string is parsed from right to left with following
    rules

    1) no characters other than numeral, alphabets a-f, A-F and
    - is allowed
    2) the first right most letter should not be a -
    3) no consecutive - allowed
    4) not more than six parts allowed

    @param Destination - contains validated MAC address in decimal
                               system
    @param Source     - string MAC to be validated

    @return EFI_STATUS

    @note  Function variable description
  
              Index        - counter for loop.
              LookingMac   - MAC address is taken into six parts, one part
                             by one will be extracted from the string and
                             saved to Destination variable.
                             LookingMac variable contains which MAC part
                             currently we are extracting.
              SumMac       - contains sum of the digit of an MAC address
                             multiplied by its base power. SumMac=
                             (unit digit * 1) + (Tenth digit * 16)
              MacDigit     - base power of the digit we are extracting
              DigitCount   - digit number - 1 we are extracting in a MAC
                             part
              IsMacValid   - flag to set if an invalid MAC is entered and
                             break the loop.
              GotTheFirstDigit - flag is set when the first hex value is
                                 found in the string,So if a - is 
                                 encountered first,immediately the loop
                                 can be terminated.
              TmpValue     - Used to convert all small letters to capital
                             letters
              TotalSumMac  - sum of all 6 SumMac part of an MAC address.
                             this variable is used to avoid
                             00-00-00-00-00-00 MAC case in the
                             BmcLanConfiguration.intial value is set to
                             zero.if sum of all 6 SumMac is zero then
                             that is invalid MAC.

**/

EFI_STATUS
ConvertToMacAddress(
  OUT UINT8       Destination[],
  IN  CHAR16      Source[] )
{
    INT8       Index;
    INT8       LookingMac;
    UINT8      SumMac;
    UINT8      MacDigit;
    UINT8      DigitCount;
    UINT8      IsMacValid;
    UINT8      GotTheFirstDigit;
    UINT8      TmpValue;
    UINT16     TotalSumMac;

    LookingMac = 5;
    SumMac = 0;
    MacDigit = 1;
    DigitCount = 0;
    IsMacValid = 1;
    GotTheFirstDigit = 0;
    TotalSumMac = 0;

    for (Index = 16; Index >= 0; Index--) {

        if ( (Source[Index] <= 57 && Source[Index] >= 48) ||
            (Source[Index] >= 65 && Source[Index] <= 70) ||
            (Source[Index] >= 97 && Source[Index] <= 102) ) {

            GotTheFirstDigit = 1;

            if (DigitCount > 1) {
                IsMacValid = 0;
                break;
            }
            TmpValue = (UINT8)Source[Index];
      
            if(TmpValue >= 97 && TmpValue <= 102) {

                TmpValue = TmpValue - 32;
            }
            TmpValue = TmpValue - 48;
      
            if (TmpValue > 9) {
                TmpValue = TmpValue - 7;
            }

            SumMac = SumMac + (TmpValue * MacDigit);
            MacDigit = MacDigit * 16;
            DigitCount = DigitCount + 1;
        } else if (Source[Index] == 45) {

            if (GotTheFirstDigit == 0 || Source[Index + 1] == 45) {
                IsMacValid = 0;
                break;
            }

            Destination[LookingMac] = SumMac;
            TotalSumMac = TotalSumMac + SumMac;
            LookingMac--;
            if(LookingMac < 0) {
                IsMacValid = 0;
                break;
            }
            SumMac = 0;
            MacDigit = 1;
            DigitCount = 0;
        } else {
            IsMacValid = 0;
            break;
        }
    }

    if (LookingMac == 0) {
        Destination[LookingMac] = SumMac;
        TotalSumMac = TotalSumMac + SumMac;
    } 

    if (LookingMac !=0 || IsMacValid == 0 || TotalSumMac == 0) {
        Destination[0] = 0;
        Destination[1] = 0;
        Destination[2] = 0;
        Destination[3] = 0;
        Destination[4] = 0;
        Destination[5] = 0;
        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}

/**
    Checks for Set-In Progress Bit and Wait to get it Clear

    @param  LanChannelNumber - Channel Number of LAN

    @return EFI_STATUS
    @retval  EFI_SUCCESS - Set-In Progress Bit is Cleared
    @retval  EFI_TIMEOUT - Specified Time out over
    
**/

EFI_STATUS
IpmiWaitSetInProgressClear (
   UINT8 LanChannelNumber ) 
{
    EFI_STATUS                      Status;
    UINT8                           ResponseSize;
    UINT8                           Retries = 10;
    EFI_IPMI_GET_LAN_CONFIG_PRAM    GetLanParamCmd;
    EFI_GET_LAN_CONFIG_PARAM0       GetSetInProgResponse;

    GetLanParamCmd.LanChannel.ChannelNumber = LanChannelNumber;
    GetLanParamCmd.LanChannel.Reserved = 0;
    GetLanParamCmd.LanChannel.GetParam = 0; 
    GetLanParamCmd.ParameterSelector = 0;       
    GetLanParamCmd.SetSelector = 0;
    GetLanParamCmd.BlockSelector = 0;

    //
    // Wait for Set-In progress bit to clear
    //
    do {
        ResponseSize = sizeof (GetSetInProgResponse);
        Status = gIpmiTransport->SendIpmiCommand (
                     gIpmiTransport,
                     IPMI_NETFN_TRANSPORT,
                     BMC_LUN,
                     IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                     (UINT8*) &GetLanParamCmd,
                     sizeof (GetLanParamCmd),
                     (UINT8*) &GetSetInProgResponse,
                     &ResponseSize );

        if (EFI_ERROR (Status)) {
            return Status;
        }

        if (GetSetInProgResponse.Param0.SetInProgress == 0) {
            break;
        }
        gBS->Stall (IPMI_STALL);
    } while (Retries-- > 0);

    if (++Retries == 0) {  // time out after Retires and first attempt.
        return EFI_TIMEOUT;
    }
    return EFI_SUCCESS;
}

/**
    Set the LAN configuration values from BIOS setup to BMC

    @param LanChannelNumber - LAN channel to use

    @param BmcLanIpSetupValues -  Structure containing IP and MAC to be
                                  entered in BMC

    @return EFI_STATUS

**/

EFI_STATUS 
SetStaticBmcNetworkParameters(
  IN  UINT8       LanChannelNumber,
  IN  BMC_IP_BIOS_SETTINGS    BmcLanIpSetupValues )
{

    EFI_STATUS               Status;
    UINT8                    CommandData[8];
    UINT8                    ResponseData[1];
    UINT8                    ResponseDataSize;

    CommandData[0] = LanChannelNumber;

    //
    //Set station IP address
    //
    CommandData[1] = IpmiLanIpAddress;
    CopyMem (&CommandData[2], BmcLanIpSetupValues.StationIp, sizeof (BmcLanIpSetupValues.StationIp));
    ResponseDataSize = 1;

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
                CommandData,
                6,
                ResponseData,
                &ResponseDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS: IpmiLanIpAddress: LanChannelNumber: %x Status: %r\n", LanChannelNumber, Status));

    //
    //Set Subnet Mask
    //
    CommandData[1] = IpmiLanSubnetMask;
    CopyMem (&CommandData[2], BmcLanIpSetupValues.Subnet, sizeof (BmcLanIpSetupValues.Subnet));
    ResponseDataSize = 1;

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
                CommandData,
                6,
                ResponseData,
                &ResponseDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS: IpmiLanSubnetMask: LanChannelNumber: %x Status: %r\n", LanChannelNumber, Status));

    //
    //Set Default Gateway Ip Address
    //
    CommandData[1] = IpmiLanDefaultGateway;
    CopyMem (&CommandData[2], BmcLanIpSetupValues.RouterIp, sizeof(BmcLanIpSetupValues.RouterIp));
    ResponseDataSize = 1;

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
                CommandData,
                6,
                ResponseData,
                &ResponseDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS: IpmiLanDefaultGateway: LanChannelNumber: %x Status: %r\n", LanChannelNumber, Status));
#if SMCPKG_SUPPORT == 0
    //
    //Set Default Gateway MAC Address
    //
    CommandData[1] = IpmiLanDefaultGatewayMac;
    CopyMem (&CommandData[2], BmcLanIpSetupValues.RouterMac, sizeof (BmcLanIpSetupValues.RouterMac));
    ResponseDataSize = 1;

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
                CommandData,
                8,
                ResponseData,
                &ResponseDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS: IpmiLanDefaultGatewayMac: LanChannelNumber: %x Status: %r\n", LanChannelNumber, Status));
#endif
    return EFI_SUCCESS;
}

/**
    Set the LAN IP address source values from BIOS setup to BMC

    @param LanChannelNumber - LAN channel to use

    @param IpAddessSource - Value of address source

    @return EFI_STATUS

**/

EFI_STATUS
SetIpAddressSource (
  IN  UINT8      LanChannelNumber,
  IN  UINT8      IpAddessSource )
{
    EFI_STATUS                    Status;
    UINT8                         CommandData[4];
    UINT8                         ResponseData[1];
    UINT8                         ResponseDataSize;
    //
    //Set IP address source
    //
    CommandData[0] = LanChannelNumber;
    CommandData[1] = IpmiLanIpAddressSource;
    CommandData[2] = IpAddessSource;
    ResponseDataSize = 1;

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
                CommandData,
                3,
                ResponseData,
                &ResponseDataSize );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS: IpAddessSource:%x LanChannelNumber: %x Status: %r\n", IpAddessSource, LanChannelNumber, Status));

    return Status;
}

#if SMCPKG_SUPPORT
EFI_STATUS
IPMILANSelection (
    IN	UINT8	LanChannelNumber,
    IN	UINT8	IPMILANSelectionValue
)
{
    EFI_STATUS	Status;
    UINT8	CommandData[3];
    UINT8	ResponseData, ResponseDataSize = sizeof(ResponseData);

    DEBUG((-1, "SMC IPMILANSelection entry.\n"));
    //
    // Set IPMI LAN Selection.
    // 0x30 0x70 0x0C 0x01 [Setting]
    //
    CommandData[0] = 0x0c;		
    CommandData[1] = 01;
    CommandData[2] = IPMILANSelectionValue;

    Status = IpmiWaitSetInProgressClear(LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand(
		    gIpmiTransport,
		    0x30,
		    BMC_LUN,		//Lun, 0x00
		    0x70,		//OEM Command, 0x70
		    CommandData,
		    3,
		    &ResponseData,
		    &ResponseDataSize);
    DEBUG((-1, "SMC IPMILANSelection end.\n"));
    return Status;
}

EFI_STATUS
SetIPMIVlanID(
    IN	UINT8	LanChannelNumber,
    IN	UINT8	VlanState,
    IN	UINT16	VlanID
)
{
    EFI_STATUS	Status;
    UINT8	CommandData[4];
    UINT8	ResponseData, ResponseDataSize = 1;
    UINT8	CommandRetry = 10;

    DEBUG((-1, "SMC SetIPMIVlanID entry.\n"));
    CommandData[0] = BMC_LAN1_CHANNEL_NUMBER;		
    CommandData[1] = 20;
    CommandData[2] = (UINT8)gServerMgmtConfiguration.SmcBmcVlanID;
    CommandData[3] = (UINT8)(gServerMgmtConfiguration.SmcBmcVlanID >> 8) |
		    (gServerMgmtConfiguration.SmcBmcVlan << 7);

    while(CommandRetry){
	Status = IpmiWaitSetInProgressClear (LanChannelNumber);
	Status = gIpmiTransport->SendIpmiCommand(
			gIpmiTransport,
			IPMI_NETFN_TRANSPORT,
			BMC_LUN,				//Lun, 0x00
			IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
			CommandData,
			4,
			&ResponseData,
			&ResponseDataSize);

	if(!EFI_ERROR(Status))	break;

	CommandRetry--;
    }
    DEBUG((-1, "SMC SetIPMIVlanID end.\n"));
    return Status;
}

UINT8
GetIpAddressSource (
    IN  UINT8      LanChannelNumber
)
{
    EFI_STATUS                    Status;
    UINT8                         CommandData[4];
    UINT8                         ResponseData[2];
    UINT8                         ResponseDataSize;
    UINT8    IpAddessSource;
    //
    //Set IP address source
    //
    CommandData[0] = LanChannelNumber;
    CommandData[1] = IpmiLanIpAddressSource;
    CommandData[2] = 0;
    CommandData[3] = 0;    
    ResponseDataSize = 2;
	Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                CommandData,
                4,
                ResponseData,
                &ResponseDataSize );
    
    if (!EFI_ERROR(Status)) IpAddessSource = ResponseData[1] & 0x0F;
    else return 0;
    
    return IpAddessSource;

}

EFI_STATUS
GetLanInformation (
    IN UINT8      LanChannelNumber,
    IN UINT8      ParameterSelector,
    OUT CHAR16     IpAddress[]
)
{
    EFI_STATUS                    Status;
    UINT8                         CommandData[4];
    UINT8                         ResponseData[5];
    UINT8                         ResponseDataSize;
    UINT8    Temp;
    
    //
    //Set IP address source
    //
    CommandData[0] = LanChannelNumber;
    CommandData[1] = ParameterSelector;
    CommandData[2] = 0;
    CommandData[3] = 0;    
    ResponseDataSize = 5;
	Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                CommandData,
                4,
                ResponseData,
                &ResponseDataSize );
    
    if (!EFI_ERROR(Status)) {
        IpAddress[0] = ResponseData[1] / 100 + 0x30;
        Temp = ResponseData[1] % 100;
        IpAddress[1] = Temp / 10 + 0x30;
        IpAddress[2] = Temp % 10 + 0x30;
        IpAddress[3] = 46;
        IpAddress[4] = ResponseData[2] / 100 + 0x30;
        Temp = ResponseData[2] % 100;
        IpAddress[5] = Temp / 10 + 0x30;
        IpAddress[6] = Temp % 10 + 0x30;
        IpAddress[7] = 46;
        IpAddress[8] = ResponseData[3] / 100 + 0x30;
        Temp = ResponseData[3] % 100;
        IpAddress[9] = Temp / 10 + 0x30;
        IpAddress[10] = Temp % 10 + 0x30;
        IpAddress[11] = 46;
        IpAddress[12] = ResponseData[4] / 100 + 0x30;
        Temp = ResponseData[4] % 100;
        IpAddress[13] = Temp / 10 + 0x30;
        IpAddress[14] = Temp % 10 + 0x30;        
    }
        
    return Status;

}
#endif

/**
    DriverEntryPoint, 
    In this driver we will configure BMC LAN channel using static
    information available through setup or using a DHCP client as
    per user request.

    @param ImageHandle - Variable of EFI_HANDLE
    @param SystemTable - Pointer to EFI_SYSTEM_TABLE

    @return EFI_STATUS

**/

EFI_STATUS
InitializeBmcLanConfig (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{

    EFI_STATUS          Status;
    UINTN               Size;
    UINT32              Attributes;
    BOOLEAN             HaveInvalidParameter = FALSE;
    BMC_IP_BIOS_SETTINGS        BmcLanIp1SetupValues;
#if BMC_LAN_COUNT == 2
    BMC_IP_BIOS_SETTINGS        BmcLanIp2SetupValues;
#endif
#if SMCPKG_SUPPORT
    UINT8               SMCSetLan, IPMILANSelectionValue;
    UINT8               IpAddressSource;
#endif

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... \n", __FUNCTION__));

    Status = gBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Size = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = gRT->GetVariable (
                L"ServerSetup",
                &gEfiServerMgmtSetupVariableGuid,
                &Attributes,
                &Size,
                &gServerMgmtConfiguration );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gRT->GetVariable status %r \n", Status));
    if (EFI_ERROR(Status)) {
        BmcLanIp1SetupValues.BmcLan = IpmiUnspecified;
#if BMC_LAN_COUNT == 2
        BmcLanIp2SetupValues.BmcLan = IpmiUnspecified;
#endif
    } else {
        BmcLanIp1SetupValues.BmcLan = gServerMgmtConfiguration.BmcLan1;
#if BMC_LAN_COUNT == 2
        BmcLanIp2SetupValues.BmcLan = gServerMgmtConfiguration.BmcLan2;
#endif
    }

    //
    // Static IP has already been validated in Setup. Convert IP and MAC strings into Valid IP and MAC format
    // and copy them to module structure BmcLanIpSetupValues
    // Convert IP and MAC of BMC LAN Channel-1
    //
    if ( ConvertToIpAddress (BmcLanIp1SetupValues.StationIp, gServerMgmtConfiguration.StationIp1) != EFI_SUCCESS ) {
        HaveInvalidParameter = TRUE;
        SetZeroIp (gServerMgmtConfiguration.StationIp1);
    }

    if (ConvertToIpAddress (BmcLanIp1SetupValues.Subnet, gServerMgmtConfiguration.Subnet1) != EFI_SUCCESS) {
        HaveInvalidParameter = TRUE;
        SetZeroIp (gServerMgmtConfiguration.Subnet1);
    }

    if (ConvertToIpAddress (BmcLanIp1SetupValues.RouterIp, gServerMgmtConfiguration.RouterIp1) != EFI_SUCCESS) {
        HaveInvalidParameter = TRUE;
        SetZeroIp (gServerMgmtConfiguration.RouterIp1);
    }

    if (ConvertToMacAddress (BmcLanIp1SetupValues.RouterMac, gServerMgmtConfiguration.RouterMac1) != EFI_SUCCESS) {
        HaveInvalidParameter = TRUE;
        SetZeroMac (gServerMgmtConfiguration.RouterMac1);
    }

#if BMC_LAN_COUNT == 2
    //
    // Convert IP and MAC of BMC LAN Channel-2
    //
    if ( ConvertToIpAddress (BmcLanIp2SetupValues.StationIp, gServerMgmtConfiguration.StationIp2) != EFI_SUCCESS ) {
        HaveInvalidParameter = TRUE;
        SetZeroIp (gServerMgmtConfiguration.StationIp2);
    }

    if (ConvertToIpAddress (BmcLanIp2SetupValues.Subnet, gServerMgmtConfiguration.Subnet2) != EFI_SUCCESS) {
        HaveInvalidParameter = TRUE;
        SetZeroIp (gServerMgmtConfiguration.Subnet2);
    }


    if (ConvertToIpAddress (BmcLanIp2SetupValues.RouterIp, gServerMgmtConfiguration.RouterIp2) != EFI_SUCCESS) {
        HaveInvalidParameter = TRUE;
        SetZeroIp (gServerMgmtConfiguration.RouterIp2);
    }

    if (ConvertToMacAddress (BmcLanIp2SetupValues.RouterMac, gServerMgmtConfiguration.RouterMac2) != EFI_SUCCESS) {
        HaveInvalidParameter = TRUE;
        SetZeroMac (gServerMgmtConfiguration.RouterMac2);
    }
#endif

#if SMCPKG_SUPPORT
    SMCSetLan = gServerMgmtConfiguration.SMCSetLan;
    //gServerMgmtConfiguration.SMCSetLan = 0; //Reset after InitializeIp6BmcLanConfig, IPV4 -> IPV6 -> reset
    HaveInvalidParameter = TRUE;
    IPMILANSelectionValue = gServerMgmtConfiguration.IPMILANSelection;
	
    if(!SMCSetLan){
	IpAddressSource = GetIpAddressSource (BMC_LAN1_CHANNEL_NUMBER);
	  
	switch(IpAddressSource){
	case IpmiStaticAddress:
	    gServerMgmtConfiguration.BmcLan1 = IpmiStaticAddress;
	    break;
	case IpmiDynamicAddressBmcDhcp:
	    gServerMgmtConfiguration.BmcLan1 = IpmiDynamicAddressBmcDhcp;	        
	    break;
	default:
	    gServerMgmtConfiguration.BmcLan1 = IpmiUnspecified;
	    break;
	}
    }

    if(IpAddressSource == IpmiStaticAddress){
	GetLanInformation (BMC_LAN1_CHANNEL_NUMBER, IpmiLanIpAddress ,gServerMgmtConfiguration.StationIp1);
	GetLanInformation (BMC_LAN1_CHANNEL_NUMBER, IpmiLanSubnetMask ,gServerMgmtConfiguration.Subnet1);	    
	GetLanInformation (BMC_LAN1_CHANNEL_NUMBER, IpmiLanDefaultGateway ,gServerMgmtConfiguration.RouterIp1);        
    }
#else
    gServerMgmtConfiguration.BmcLan1 = IpmiUnspecified;
#if BMC_LAN_COUNT == 2
    gServerMgmtConfiguration.BmcLan2 = IpmiUnspecified;
#endif
#endif
    Status = gRT->SetVariable (
                    L"ServerSetup",
                    &gEfiServerMgmtSetupVariableGuid,
                    Attributes,   // Attributes
                    sizeof(SERVER_MGMT_CONFIGURATION_DATA),
                    &gServerMgmtConfiguration);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "HaveInvalidParameter: %X gRT->SetVariable status %r \n", HaveInvalidParameter, Status));

    DEBUG ((EFI_D_INFO, "#########################\n"));
    DEBUG ((EFI_D_INFO, "###Static BMC LAN configuration values from BIOS setup\n"));
    DEBUG ((EFI_D_INFO, "#########################\n"));
    DEBUG ((EFI_D_INFO, "###  Lan1: %d\n", BmcLanIp1SetupValues.BmcLan));
    DEBUG ((EFI_D_INFO, "###  StationIp:  %d %d %d %d\n", BmcLanIp1SetupValues.StationIp[0], BmcLanIp1SetupValues.StationIp[1], BmcLanIp1SetupValues.StationIp[2], BmcLanIp1SetupValues.StationIp[3]));
    DEBUG ((EFI_D_INFO, "###  SubnetMask: %d %d %d %d\n",BmcLanIp1SetupValues.Subnet[0], BmcLanIp1SetupValues.Subnet[1], BmcLanIp1SetupValues.Subnet[2], BmcLanIp1SetupValues.Subnet[3]));
    DEBUG ((EFI_D_INFO, "###  RouterIp: %d %d %d %d\n", BmcLanIp1SetupValues.RouterIp[0], BmcLanIp1SetupValues.RouterIp[1], BmcLanIp1SetupValues.RouterIp[2], BmcLanIp1SetupValues.RouterIp[3]));
    DEBUG ((EFI_D_INFO, "###  Router MAC: %x %x %x %x %x %x\n", BmcLanIp1SetupValues.RouterMac[0], BmcLanIp1SetupValues.RouterMac[1], BmcLanIp1SetupValues.RouterMac[2], BmcLanIp1SetupValues.RouterMac[3], BmcLanIp1SetupValues.RouterMac[4], BmcLanIp1SetupValues.RouterMac[5]));
    DEBUG ((EFI_D_INFO, "###----------------------\n"));
#if BMC_LAN_COUNT == 2
    DEBUG ((EFI_D_INFO, "###  Lan2: %d\n", BmcLanIp2SetupValues.BmcLan));
    DEBUG ((EFI_D_INFO, "###  StationIp:  %d %d %d %d\n", BmcLanIp2SetupValues.StationIp[0], BmcLanIp2SetupValues.StationIp[1], BmcLanIp2SetupValues.StationIp[2], BmcLanIp2SetupValues.StationIp[3]));
    DEBUG ((EFI_D_INFO, "###  SubnetMask: %d %d %d %d\n", BmcLanIp2SetupValues.Subnet[0], BmcLanIp2SetupValues.Subnet[1], BmcLanIp2SetupValues.Subnet[2], BmcLanIp2SetupValues.Subnet[3]));
    DEBUG ((EFI_D_INFO, "###  RouterIp: %d %d %d %d\n", BmcLanIp2SetupValues.RouterIp[0], BmcLanIp2SetupValues.RouterIp[1], BmcLanIp2SetupValues.RouterIp[2], BmcLanIp2SetupValues.RouterIp[3]));
    DEBUG ((EFI_D_INFO, "###  Router MAC: %x %x %x %x %x %x\n", BmcLanIp2SetupValues.RouterMac[0], BmcLanIp2SetupValues.RouterMac[1], BmcLanIp2SetupValues.RouterMac[2], BmcLanIp2SetupValues.RouterMac[3], BmcLanIp2SetupValues.RouterMac[4], BmcLanIp2SetupValues.RouterMac[5]));
    DEBUG ((EFI_D_INFO, "#########################\n"));
#endif

#if SMCPKG_SUPPORT
    //
    // BMC LAN config function moves to "SmcPkg\Module\SmcPostMsgHotKey\BmcLanConfig.c".
    // It will do the function at Saving BIOS settings in setup menu.
    //check if nothing has to be done for both lan channels
    //
    if(TRUE)	return Status;

    Status = IPMILANSelection(BMC_LAN1_CHANNEL_NUMBER, IPMILANSelectionValue);

    SetIPMIVlanID(BMC_LAN1_CHANNEL_NUMBER, gServerMgmtConfiguration.SmcBmcVlan, gServerMgmtConfiguration.SmcBmcVlanID);
#endif

    //
    // Setting LAN1 address source to BMC.
    //
    SetIpAddressSource (BMC_LAN1_CHANNEL_NUMBER, BmcLanIp1SetupValues.BmcLan);

    //
    // Configure Static address source parameters to BMC
    //
    if (BmcLanIp1SetupValues.BmcLan == IpmiStaticAddress) {
        SetStaticBmcNetworkParameters (BMC_LAN1_CHANNEL_NUMBER, BmcLanIp1SetupValues);
    }
#if BMC_LAN_COUNT == 2
    //
    // Setting LAN2 address source to BMC.
    //
    SetIpAddressSource (BMC_LAN2_CHANNEL_NUMBER, BmcLanIp2SetupValues.BmcLan);

    //
    // Configure Static address source parameters to BMC
    //
    if (BmcLanIp2SetupValues.BmcLan == IpmiStaticAddress) {
        SetStaticBmcNetworkParameters (BMC_LAN2_CHANNEL_NUMBER, BmcLanIp2SetupValues );
    }
#endif

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting... \n", __FUNCTION__));
    return EFI_SUCCESS;
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
