//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  1.Remove setup item "IPV6 Support" load default function sicne its vaule should read back from IPMI 
//                or user decide to update IPMI setting ("Update IPMI LAN Configuration" = "Yes")
//    Reason:   
//    Auditor:  Jimmy Chiu
//    Date:     Jul/07/2017
//
//  Rev. 1.02
//    Bug Fix:  1.Set IPV6 "Configuration Address Source" default to "DHCP".
//    Reason:   
//    Auditor:  Jimmy Chiu
//    Date:     Jun/15/2017
//
//  Rev. 1.01
//    Bug Fix:  Fix IPMI Network Link Status show N/A
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Aug/18/2016
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

/** @file BmcLanParam.c
    Reads the LAN parameters from BMC and updates in Setup
    and Verifies the Static BMC Network configuration with Setup Callback

**/

//----------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include "BmcLanConfig.h"
#include <ServerMgmtSetup.h>
#include <ServerMgmtSetupVariable.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Include/IpmiNetFnTransportDefinitions.h>
#include <Protocol/IPMITransportProtocol.h>
#if SMCPKG_SUPPORT
#include <Include/IpmiNetFnAppDefinitions.h>
#define GET_LAN_COMMAND_DATA_SIZE	0x04
#endif
//----------------------------------------------------------------------

#define DEFAULT_ZERO_START     ((UINTN) ~0)
#define STRING_BUFFER_LENGTH        4000

//
// Global Variables
//
EFI_IPMI_TRANSPORT  *gIpmiTransport;

//
// Function Prototype
//
VOID InitString(EFI_HII_HANDLE HiiHandle, STRING_REF StrRef, CHAR16 *sFormat, ...);

/**
    It will return the String length of the array.

    @param String It holds the base address of the array.

    @return UINTN Returns length of the char array.

    @note  Function variable description

            String - It holds the base address of the array.

            Length - Holds the String length.

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
    @param Source - string IP to be validated

    @retval EFI_STATUS

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
ValidateIpAddress(
  OUT  UINT8      Destination[],
  IN  CHAR16      Source[] )
{
    INTN       Index;
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

    Index = (INTN) EfiStrLen(Source);

    //
    //Assigning index = 15 because it is not returning 15 as strlen if ip is
    //like xxx.xxx.xxx.xxx this.
    //

    if ( Index > 15) {
        Index = 15;
    }

    for (Index = (Index - 1); Index >= 0; Index--) {

        if (Source[Index] <= 57 && Source[Index] >= 48) {

            GotTheFirstDigit = 1;

            if (DigitCount > 2) {

                IsIpValid = 0;
                break;
            }

            if (DigitCount == 2) {

                if (Source[Index] - 48 >2) {

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

                    if(LookingIp < 0) {

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

        Destination[0] = 0;
        Destination[1] = 0;
        Destination[2] = 0;
        Destination[3] = 0;
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
    @param  Source      - string MAC to be validated

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
ValidateMacAddress(
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

            if (TmpValue >= 97 && TmpValue <= 102) {
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
    Set IP address to ::::::: for memory pointed by ZeroIp

    @param ZeroIp - Pointer to Ip address in BIOS setup variable

    @retval VOID

**/
VOID
SetZeroIp6(
  IN OUT  CHAR16      ZeroIp6[] )
{

    ZeroMem((UINT8*)&ZeroIp6[0], 40 * 2);
    ZeroIp6[0] = ':';
    ZeroIp6[1] = ':';
    ZeroIp6[2] = ':';
    ZeroIp6[3] = ':';
    ZeroIp6[4] = ':';
    ZeroIp6[5] = ':';
    ZeroIp6[6] = ':';
    return;
}

/**
  Convert one Null-terminated ASCII string to EFI_IPv6_ADDRESS. 

  @param[in]      String         The pointer to the Ascii string.
  @param[out]     Ip6Address     The pointer to the converted IPv6 address.

  @retval EFI_SUCCESS            Convert to IPv6 address successfully.
  @retval EFI_INVALID_PARAMETER  The string is mal-formated or Ip6Address is NULL.

**/
EFI_STATUS
EFIAPI
ConvertAsciiStrToBmcIp6 (
  IN CONST CHAR8                 *String,
  OUT      IPMI_IPV6_ADDRESS      *Ip6Address
  )
{
    UINT8                          Index;
    CHAR8                          *Ip6Str;
    CHAR8                          *TempStr;
    CHAR8                          *TempStr2;
    UINT8                          NodeCnt;
    UINT8                          TailNodeCnt;
    UINT8                          AllowedCnt;
    UINTN                          NodeVal;
    BOOLEAN                        Short;
    BOOLEAN                        Update;
    BOOLEAN                        LeadZero;
    UINT8                          LeadZeroCnt;
    UINT8                          Cnt;

    if ((String == NULL) || (Ip6Address == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    Ip6Str      = (CHAR8 *) String;
    AllowedCnt  = 6;
    LeadZeroCnt = 0;

    //
    // An IPv6 address leading with : looks strange.
    //
    if (*Ip6Str == ':') {
        if (*(Ip6Str + 1) != ':') {
          return EFI_INVALID_PARAMETER;
        } else {
        AllowedCnt = 7;
        }
    }

    ZeroMem (Ip6Address, sizeof (EFI_IPv6_ADDRESS));

    NodeCnt     = 0;
    TailNodeCnt = 0;
    Short       = FALSE;
    Update      = FALSE;
    LeadZero    = FALSE;

    for (Index = 0; Index < 15; Index = (UINT8) (Index + 2)) {
        TempStr = Ip6Str;

    while ((*Ip6Str != '\0') && (*Ip6Str != ':')) {
        Ip6Str++;
    }

    if ((*Ip6Str == '\0') && (Index != 14)) {
      return EFI_INVALID_PARAMETER;
    }

    if (*Ip6Str == ':') {
      if (*(Ip6Str + 1) == ':') {
        if ((NodeCnt > 6) || 
            ((*(Ip6Str + 2) != '\0') && (AsciiStrHexToUintn (Ip6Str + 2) == 0))) {
          //
          // ::0 looks strange. report error to user.
          //
          return EFI_INVALID_PARAMETER;
        }
        if ((NodeCnt == 6) && (*(Ip6Str + 2) != '\0') && 
            (AsciiStrHexToUintn (Ip6Str + 2) != 0)) {
          return EFI_INVALID_PARAMETER;
        }

        //
        // Skip the abbreviation part of IPv6 address.
        //
        TempStr2 = Ip6Str + 2;
        while ((*TempStr2 != '\0')) {
          if (*TempStr2 == ':') {
            if (*(TempStr2 + 1) == ':') {
              //
              // :: can only appear once in IPv6 address.
              //
              return EFI_INVALID_PARAMETER;
            }

            TailNodeCnt++;
            if (TailNodeCnt >= (AllowedCnt - NodeCnt)) {
              //
              // :: indicates one or more groups of 16 bits of zeros.
              //
              return EFI_INVALID_PARAMETER;
            }
          }

          TempStr2++;
        }

        Short  = TRUE;
        Update = TRUE;

        Ip6Str = Ip6Str + 2;
      } else {
        if (*(Ip6Str + 1) == '\0') {
          return EFI_INVALID_PARAMETER;
        }
        Ip6Str++;
        NodeCnt++;
        if ((Short && (NodeCnt > 6)) || (!Short && (NodeCnt > 7))) {
          //
          // There are more than 8 groups of 16 bits of zeros.
          //
          return EFI_INVALID_PARAMETER;
        }
      }
    }

    //
    // Convert the string to IPv6 address. AsciiStrHexToUintn stops at the first
    // character that is not a valid hexadecimal character, ':' or '\0' here.
    //
    NodeVal = AsciiStrHexToUintn (TempStr);
    if ((NodeVal > 0xFFFF) || (Index > 14)) {
      return EFI_INVALID_PARAMETER;
    }
    if (NodeVal != 0) {
      if ((*TempStr  == '0') && 
          ((*(TempStr + 2) == ':') || (*(TempStr + 3) == ':') || 
          (*(TempStr + 2) == '\0') || (*(TempStr + 3) == '\0'))) {
        return EFI_INVALID_PARAMETER;
      }
      if ((*TempStr  == '0') && (*(TempStr + 4) != '\0') && 
          (*(TempStr + 4) != ':')) { 
        return EFI_INVALID_PARAMETER;
      }
    } else {
      if (((*TempStr  == '0') && (*(TempStr + 1) == '0') && 
          ((*(TempStr + 2) == ':') || (*(TempStr + 2) == '\0'))) ||
          ((*TempStr  == '0') && (*(TempStr + 1) == '0') && (*(TempStr + 2) == '0') && 
          ((*(TempStr + 3) == ':') || (*(TempStr + 3) == '\0')))) {
        return EFI_INVALID_PARAMETER;
      }
    }

    Cnt = 0;
    while ((TempStr[Cnt] != ':') && (TempStr[Cnt] != '\0')) {
      Cnt++; 
    }
    if (LeadZeroCnt == 0) {
      if ((Cnt == 4) && (*TempStr  == '0')) {
        LeadZero = TRUE;
        LeadZeroCnt++;
      }
      if ((Cnt != 0) && (Cnt < 4)) {
        LeadZero = FALSE;
        LeadZeroCnt++;
      }
    } else {
      if ((Cnt == 4) && (*TempStr  == '0') && !LeadZero) {
        return EFI_INVALID_PARAMETER;
      }
      if ((Cnt != 0) && (Cnt < 4) && LeadZero) {
        return EFI_INVALID_PARAMETER;
      }
    } 

    Ip6Address->Addr[Index] = (UINT8) (NodeVal >> 8);
    Ip6Address->Addr[Index + 1] = (UINT8) (NodeVal & 0xFF);

    //
    // Skip the groups of zeros by ::
    //
    if (Short && Update) {
      Index  = (UINT8) (16 - (TailNodeCnt + 2) * 2);
      Update = FALSE;
    }
  }

  if ((!Short && Index != 16) || (*Ip6Str != '\0')) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Convert one Null-terminated Unicode string to Ipv6 address of 16 bytes.

  @param[in]      String         The pointer to the Unicode string.
  @param[out]     Ip6Address     The pointer to the converted IPv6 address.

  @retval EFI_SUCCESS            Converted IPv6 address successfully.
  @retval EFI_INVALID_PARAMETER  The string is mal-formated or Ip6Address is NULL.
  @retval EFI_OUT_OF_RESOURCES   Fail to perform the operation due to lack of resource.

**/

EFI_STATUS
EFIAPI
ConvertStrToBmcIp6Address (
  IN CONST CHAR16                *String,
  OUT      IPMI_IPV6_ADDRESS      *Ip6Address
  )
{
  CHAR8                          *Ip6Str;
  EFI_STATUS                     Status;

  if ((String == NULL) || (Ip6Address == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Ip6Str = (CHAR8 *) AllocatePool ((StrLen (String) + 1) * sizeof (CHAR8));
  if (Ip6Str == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  UnicodeStrToAsciiStr (String, Ip6Str);

  Status = ConvertAsciiStrToBmcIp6 (Ip6Str, Ip6Address);

  FreePool (Ip6Str);

  return Status;
}

/**

  Convert one EFI_IPv6_ADDRESS to Null-terminated Unicode string.

  @param[in]       Ip6Address     The pointer to the IPv6 address.
  @param[out]      String         The buffer to return the converted string.
  @param[in]       StringSize     The length in bytes of the input String.
                                  
  @retval EFI_SUCCESS             Convert to string successfully.
  @retval EFI_INVALID_PARAMETER   The input parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL    The BufferSize is too small for the result. BufferSize has been 
                                  updated with the size needed to complete the request.
**/
EFI_STATUS
ConvertBmcIp6ToStr (
  IN   IPMI_IPV6_ADDRESS     *Ip6Address,
  OUT  CHAR16                *String,
  IN   UINTN                 StringSize
  )
{
    UINT16     Ip6Addr[8];
    UINTN      Index;
    UINTN      LongestZerosStart;
    UINTN      LongestZerosLength;
    UINTN      CurrentZerosStart;
    UINTN      CurrentZerosLength;
    CHAR16     Buffer[sizeof"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"];
    CHAR16     *Ptr;

    if (Ip6Address == NULL || String == NULL || StringSize == 0) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Convert the UINT8 array to an UINT16 array for easy handling.
    // 
    ZeroMem (Ip6Addr, sizeof (Ip6Addr));
    for (Index = 0; Index < 16; Index++) {
        Ip6Addr[Index / 2] |= (Ip6Address->Addr[Index] << ((1 - (Index % 2)) << 3));
    }

    //
    // Find the longest zeros and mark it.
    //
    CurrentZerosStart  = DEFAULT_ZERO_START;
    CurrentZerosLength = 0;
    LongestZerosStart  = DEFAULT_ZERO_START;
    LongestZerosLength = 0;
    for (Index = 0; Index < 8; Index++) {
        if (Ip6Addr[Index] == 0) {
            if (CurrentZerosStart == DEFAULT_ZERO_START) {
                CurrentZerosStart = Index;
                CurrentZerosLength = 1;
            } else {
               CurrentZerosLength++;
            }
        } else {
            if (CurrentZerosStart != DEFAULT_ZERO_START) {
                if (CurrentZerosLength > 2 && (LongestZerosStart == (DEFAULT_ZERO_START) || CurrentZerosLength > LongestZerosLength)) {
                    LongestZerosStart = CurrentZerosStart;
                    LongestZerosLength = CurrentZerosLength;
                }
                CurrentZerosStart  = DEFAULT_ZERO_START;
                CurrentZerosLength = 0;
           }
        }
    }

  if (CurrentZerosStart != DEFAULT_ZERO_START && CurrentZerosLength > 2) {
    if (LongestZerosStart == DEFAULT_ZERO_START || LongestZerosLength < CurrentZerosLength) {
      LongestZerosStart  = CurrentZerosStart;
      LongestZerosLength = CurrentZerosLength;
    }
  }

  Ptr = Buffer;
  for (Index = 0; Index < 8; Index++) {
    if (LongestZerosStart != DEFAULT_ZERO_START && Index >= LongestZerosStart && Index < LongestZerosStart + LongestZerosLength) {
      if (Index == LongestZerosStart) {
        *Ptr++ = L':';
      }
      continue;
    }
    if (Index != 0) {
      *Ptr++ = L':';
    }
    Ptr += UnicodeSPrint(Ptr, 10, L"%x", Ip6Addr[Index]);
  }
  
  if (LongestZerosStart != DEFAULT_ZERO_START && LongestZerosStart + LongestZerosLength == 8) {
    *Ptr++ = L':';
  }
  *Ptr = L'\0';

  if ((UINTN)Ptr - (UINTN)Buffer > StringSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  StrCpy (String, Buffer);

  return EFI_SUCCESS;
}

/**
    This function validate the given ip and MAC addresses and
    display error messages if given input is invalid data

    @param HiiHandle A handle that was previously registered in the
                     HII Database.
    @param Class    Formset Class of the Form Callback Protocol passed in
    @param SubClass Formset sub Class of the Form Callback Protocol passed in
    @param Key Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS Return Status

**/
EFI_STATUS
BmcLanConfigCallbackFunction(
  IN  EFI_HII_HANDLE     HiiHandle,
  IN  UINT16             Class,
  IN  UINT16             SubClass,
  IN  UINT16             Key )
{
    SERVER_MGMT_CONFIGURATION_DATA     *ServerMgmtConfiguration = NULL;
    BMC_IP_BIOS_SETTINGS                BmcLanIp1SetupValues;
#if BMC_LAN_COUNT == 2
    BMC_IP_BIOS_SETTINGS                BmcLanIp2SetupValues;
#endif
    CHAR16                              OutputString[STRING_BUFFER_LENGTH];
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CHAR16                              CharPtr[STRING_BUFFER_LENGTH];
    EFI_STATUS                          OutputStrStatus = EFI_SUCCESS;
    UINTN                               BufferSize = STRING_BUFFER_LENGTH;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Entered with Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( (Key != BMC_NETWORK_STATION_IP1_KEY) && \
         (Key != BMC_NETWORK_SUBNET1_KEY) && \
         (Key != BMC_NETWORK_ROUTER_IP1_KEY) && \
         (Key != BMC_NETWORK_ROUTER_MAC1_KEY) && \
         (Key != BMC_NETWORK_STATION_IP2_KEY) && \
         (Key != BMC_NETWORK_SUBNET2_KEY) && \
         (Key != BMC_NETWORK_ROUTER_IP2_KEY) && \
         (Key != BMC_NETWORK_ROUTER_MAC2_KEY) ) {
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nCallback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = pBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }
    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nStatus of HiiLibGetBrowserData() = %r\n",Status));
    ASSERT_EFI_ERROR (Status);
    #if ( TSE_BUILD > 0x1208 )
    {
        //
        // Get the call back parameters and verify the action
        //
        CALLBACK_PARAMETERS *CallbackParameters = GetCallbackParameters();
        if ( CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ) {
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so load the defaults \n"));
            pBS->FreePool(ServerMgmtConfiguration);
            return  EFI_UNSUPPORTED;
        }

        if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
            pBS->FreePool(ServerMgmtConfiguration);
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\n CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
            return  EFI_SUCCESS;
        }
    }
    #endif //#if (TSE_BUILD > 0x1208)
    //
    // Validate Ip/MAC, for error case, display error message
    //
    switch( Key )
    {
        case BMC_NETWORK_STATION_IP1_KEY :
            Status = ValidateIpAddress (
                        BmcLanIp1SetupValues.StationIp,
                        ServerMgmtConfiguration->StationIp1 );
            if ( EFI_ERROR (Status) ) {
                SetZeroIp (ServerMgmtConfiguration->StationIp1);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_STATION_IP), OutputString, &BufferSize, NULL);
            }
            break;

        case BMC_NETWORK_SUBNET1_KEY :
            Status = ValidateIpAddress (
                           BmcLanIp1SetupValues.Subnet,
                           ServerMgmtConfiguration->Subnet1 );
            if ( EFI_ERROR (Status) ) {
                SetZeroIp(ServerMgmtConfiguration->Subnet1);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_SUBNETMASK), OutputString, &BufferSize, NULL);
            }
            break;

        case BMC_NETWORK_ROUTER_IP1_KEY :
            Status = ValidateIpAddress (
                        BmcLanIp1SetupValues.RouterIp,
                        ServerMgmtConfiguration->RouterIp1 );
            if ( EFI_ERROR (Status) ) {
                SetZeroIp(ServerMgmtConfiguration->RouterIp1);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_ROUTER_IP), OutputString, &BufferSize, NULL);
            }
            break;

        case BMC_NETWORK_ROUTER_MAC1_KEY :
            Status = ValidateMacAddress (
                        BmcLanIp1SetupValues.RouterMac,
                        ServerMgmtConfiguration->RouterMac1 );
            if ( EFI_ERROR (Status) ) {
                SetZeroMac(ServerMgmtConfiguration->RouterMac1);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_ROUTER_MAC), OutputString, &BufferSize, NULL);
            }
            break;

#if BMC_LAN_COUNT == 2

        case BMC_NETWORK_STATION_IP2_KEY :
            Status = ValidateIpAddress (
                        BmcLanIp2SetupValues.StationIp,
                        ServerMgmtConfiguration->StationIp2 );
            if ( EFI_ERROR (Status) ) {
                SetZeroIp(ServerMgmtConfiguration->StationIp2);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_STATION_IP), OutputString, &BufferSize, NULL);
            }
            break;

        case BMC_NETWORK_SUBNET2_KEY :
            Status = ValidateIpAddress (
                        BmcLanIp2SetupValues.Subnet,
                        ServerMgmtConfiguration->Subnet2 );
            if ( EFI_ERROR (Status) ) {
                SetZeroIp(ServerMgmtConfiguration->Subnet2);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_SUBNETMASK), OutputString, &BufferSize, NULL);
            }
            break;

        case BMC_NETWORK_ROUTER_IP2_KEY :
            Status = ValidateIpAddress (
                        BmcLanIp2SetupValues.RouterIp,
                        ServerMgmtConfiguration->RouterIp2 );
            if ( EFI_ERROR (Status) ) {
                SetZeroIp(ServerMgmtConfiguration->RouterIp2);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_ROUTER_IP), OutputString, &BufferSize, NULL);
            }
            break;

        case BMC_NETWORK_ROUTER_MAC2_KEY :
            Status = ValidateMacAddress (
                        BmcLanIp2SetupValues.RouterMac,
                        ServerMgmtConfiguration->RouterMac2 );
            if ( EFI_ERROR (Status) ) {
                SetZeroMac(ServerMgmtConfiguration->RouterMac2);
                OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_ROUTER_MAC), OutputString, &BufferSize, NULL);
            }
            break;
#endif

        default :
            pBS->FreePool(ServerMgmtConfiguration);
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nCallback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
            return EFI_UNSUPPORTED;
    }
    if ( EFI_ERROR (Status) ) {

        AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr = NULL;
        UINT8                       MsgBoxSel;

        //
        // Locate AmiPostMgr protocol to print the Error message
        //
        if ( !EFI_ERROR(pBS->LocateProtocol
             (&gAmiPostManagerProtocolGuid, NULL, (VOID **)&AmiPostMgr)) ) {
            BufferSize = STRING_BUFFER_LENGTH;
            Status = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMCLAN_ERROR_INFO), CharPtr, &BufferSize, NULL);
            if ( (!EFI_ERROR (OutputStrStatus)) && (!EFI_ERROR (Status))) {
                AmiPostMgr->DisplayMsgBox(
                    CharPtr,
                    OutputString,
                    MSGBOX_TYPE_OK,
                    &MsgBoxSel );
            }
        }
        //
        // Set Browser DATA
        //
        Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nStatus of HiiLibSetBrowserData() = %r\n",Status));
    } // EFI_ERROR (Status)

    pBS->FreePool(ServerMgmtConfiguration);

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Exiting... \n", __FUNCTION__));

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

    if (++Retries == 0) {
        return EFI_TIMEOUT;
    }
    return EFI_SUCCESS;
}

/**
    Reads the LAN parameters from BMC and updates in Setup

    @param  HiiHandle - Handle to HII database
    @param  Class - Indicates the setup class

    @retval VOID

**/

VOID
BmcLanParamDisplayInSetup(
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class )
{

    EFI_STATUS                    Status;
    UINT8                         CommandDataSize;
    EFI_IPMI_GET_LAN_CONFIG_PRAM  GetLanParamCmd;
    EFI_GET_LAN_IP_ADDRESS_SRC    LanAddressSource;
    EFI_GET_LAN_MAC_ADDRESS       LanMacAddress;
    EFI_GET_LAN_IP_ADDRESS        LanIPAddress;
    EFI_GET_LAN_SUBNET_MASK       LanSubnetMask;
    UINT8                         ResponseDataSize;
    UINT16                        IPSource[5] = { STRING_TOKEN(STR_UNSPECIFIED),
                                                  STRING_TOKEN(STR_IPSOURCE_STATIC_ADDRESS),
                                                  STRING_TOKEN(STR_IPSOURCE_DYNAMIC_ADDRESS_BMC_DHCP),
                                                  STRING_TOKEN(STR_IPSOURCE_DYNAMIC_ADDRESS_BIOS_DHCP),
                                                  STRING_TOKEN(STR_IPSOURCE_DYNAMIC_ADDRESS_BMC_NON_DHCP)
                                                };
    UINT8                         IPSourceIndex;
    CHAR16                        CharPtr[STRING_BUFFER_LENGTH];
    EFI_STATUS                    CharPtrStatus = EFI_SUCCESS;
    UINTN                         BufferLength = STRING_BUFFER_LENGTH;
#if SMCPKG_SUPPORT
    SERVER_MGMT_CONFIGURATION_DATA	ServerConfiguration;
    EFI_GUID	ServerConfigurationGuid = SERVER_MGMT_CONFIGURATION_GUID;
    UINTN	VarSize;
    UINT32	Attributes;
    UINT8	CommandData[GET_LAN_COMMAND_DATA_SIZE] = {0, 0, 0, 0};
    UINT8	ResponseData[10];
#endif

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Entry... Class ID:  %x\n", __FUNCTION__, Class));

    //
    // Continue only for Server Mgmt setup page
    //
    if (Class != SERVER_MGMT_CLASS_ID) {
        return;
    }
    //
    // Locate the IPMI transport protocol
    //
    Status = pBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, " gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }
#if SMCPKG_SUPPORT
    VarSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
    Status = pST->RuntimeServices->GetVariable(
		    L"ServerSetup",
		    &ServerConfigurationGuid,
		    &Attributes,
		    &VarSize,
		    &ServerConfiguration);
#endif
    
    //
    // Common for all LAN 1 Channel
    //
    GetLanParamCmd.LanChannel.ChannelNumber = BMC_LAN1_CHANNEL_NUMBER;
    GetLanParamCmd.LanChannel.Reserved = 0;
    GetLanParamCmd.LanChannel.GetParam = 0; 
    GetLanParamCmd.SetSelector = 0;
    GetLanParamCmd.BlockSelector = 0;
    CommandDataSize = sizeof (GetLanParamCmd);

    //
    //Get IP address Source for Channel-1
    //

    GetLanParamCmd.ParameterSelector = IpmiLanIpAddressSource;
    ResponseDataSize = sizeof (LanAddressSource);

    //
    // Wait until Set In Progress field is cleared          
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanAddressSource,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            if (LanAddressSource.AddressSrc > 4) {
                IPSourceIndex = 0;
            } else {
                IPSourceIndex = LanAddressSource.AddressSrc;
            }

            CharPtrStatus = IpmiHiiGetString (HiiHandle, IPSource[IPSourceIndex], CharPtr, &BufferLength, NULL);
            if (!EFI_ERROR(CharPtrStatus)) {
                InitString (
                    HiiHandle,
                    STRING_TOKEN(STR_CURR_LANCAS1_VAL),
                    CharPtr );
            }
        } 
    }
    if (EFI_ERROR (Status)) {
        CharPtrStatus = IpmiHiiGetString (HiiHandle, IPSource[0], CharPtr, &BufferLength, NULL);
        if (!EFI_ERROR (CharPtrStatus)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_CURR_LANCAS1_VAL),
                CharPtr);
        }
    }

    //
    //Get station IP address of Channel-1
    //
    GetLanParamCmd.ParameterSelector = IpmiLanIpAddress;
    ResponseDataSize = sizeof (LanIPAddress);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanIPAddress,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_IP1_VAL),
                L"%d.%d.%d.%d",
                LanIPAddress.IpAddress[0], LanIPAddress.IpAddress[1], LanIPAddress.IpAddress[2], LanIPAddress.IpAddress[3]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP1_VAL),
            L"%d.%d.%d.%d",
            0,0,0,0);
        }

    //
    //Get Subnet MASK of Channel-1
    //
    GetLanParamCmd.ParameterSelector = IpmiLanSubnetMask;
    ResponseDataSize = sizeof (LanSubnetMask);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
    
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanSubnetMask,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_SUBNET_MASK1_VAL),
                L"%d.%d.%d.%d",
                LanSubnetMask.IpAddress[0], LanSubnetMask.IpAddress[1], LanSubnetMask.IpAddress[2], LanSubnetMask.IpAddress[3]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SUBNET_MASK1_VAL),
            L"%d.%d.%d.%d",
            0,0,0,0);
    }

    //
    //Get MAC address of Channel-1
    //
    GetLanParamCmd.ParameterSelector = IpmiLanMacAddress;
    ResponseDataSize = sizeof (LanMacAddress);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanMacAddress,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_MAC1_VAL),
                L"%02x-%02x-%02x-%02x-%02x-%02x",
                LanMacAddress.MacAddress[0], LanMacAddress.MacAddress[1], LanMacAddress.MacAddress[2], LanMacAddress.MacAddress[3], LanMacAddress.MacAddress[4], LanMacAddress.MacAddress[5]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_MAC1_VAL),
            L"%02x-%02x-%02x-%02x-%02x-%02x",
            0,0,0,0,0,0);
    }

    //
    //Get Router Gateway IP Address of Channel-1
    //

    GetLanParamCmd.ParameterSelector = IpmiLanDefaultGateway;
    ResponseDataSize = sizeof (LanIPAddress);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanIPAddress,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_ROUTER_IP1_VAL),
                L"%d.%d.%d.%d",
                LanIPAddress.IpAddress[0], LanIPAddress.IpAddress[1], LanIPAddress.IpAddress[2], LanIPAddress.IpAddress[3]);
        }
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_ROUTER_IP1_VAL),
            L"%d.%d.%d.%d",
            0,0,0,0);
    }

    //
    //Get Router MAC address of Channel-1
    //
    GetLanParamCmd.ParameterSelector = IpmiLanDefaultGatewayMac;
    ResponseDataSize = sizeof (LanMacAddress);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanMacAddress,
                    &ResponseDataSize );
        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_ROUTER_MAC1_VAL),
                L"%02x-%02x-%02x-%02x-%02x-%02x",
                LanMacAddress.MacAddress[0], LanMacAddress.MacAddress[1], LanMacAddress.MacAddress[2], LanMacAddress.MacAddress[3], LanMacAddress.MacAddress[4], LanMacAddress.MacAddress[5]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_ROUTER_MAC1_VAL),
            L"%02x-%02x-%02x-%02x-%02x-%02x",
            0,0,0,0,0,0);
    }
#if BMC_LAN_COUNT == 2
    //
    // Common for all LAN 2 Channel
    //
    GetLanParamCmd.LanChannel.ChannelNumber = BMC_LAN2_CHANNEL_NUMBER;
    GetLanParamCmd.LanChannel.Reserved = 0;
    GetLanParamCmd.LanChannel.GetParam = 0; 
    GetLanParamCmd.SetSelector = 0;
    GetLanParamCmd.BlockSelector = 0;
    CommandDataSize = sizeof (GetLanParamCmd);

    //
    //Get IP address Source for Channel-2
    //

    GetLanParamCmd.ParameterSelector = IpmiLanIpAddressSource;
    ResponseDataSize = sizeof (LanAddressSource);
    
    //
    // Wait until Set In Progress field is cleared
    //
    BufferLength = STRING_BUFFER_LENGTH;
    Status = IpmiWaitSetInProgressClear (BMC_LAN2_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanAddressSource,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {

            if( LanAddressSource.AddressSrc > 4) {
                IPSourceIndex = 0;
            } else {
                IPSourceIndex = LanAddressSource.AddressSrc;
            }
            CharPtrStatus = IpmiHiiGetString (HiiHandle, IPSource[IPSourceIndex], CharPtr, &BufferLength, NULL);
            if (!EFI_ERROR(CharPtrStatus)) {
                InitString (
                    HiiHandle,
                    STRING_TOKEN(STR_CURR_LANCAS2_VAL),
                    CharPtr);
            }
        } 
    }
    if (EFI_ERROR (Status)) {
        CharPtrStatus = IpmiHiiGetString (HiiHandle, IPSource[0], CharPtr, &BufferLength, NULL);
        if (!EFI_ERROR(CharPtrStatus)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_CURR_LANCAS2_VAL),
                CharPtr);
        }
    }

    //
    //Get station IP address of Channel-2
    //
    GetLanParamCmd.ParameterSelector = IpmiLanIpAddress;
    ResponseDataSize = sizeof (LanIPAddress);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN2_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanIPAddress,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_IP2_VAL),
                L"%d.%d.%d.%d",
                LanIPAddress.IpAddress[0], LanIPAddress.IpAddress[1], LanIPAddress.IpAddress[2], LanIPAddress.IpAddress[3]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP2_VAL),
            L"%d.%d.%d.%d",
            0,0,0,0);
    }

    //
    //Get Subnet MASK of Channel-2
    //
    GetLanParamCmd.ParameterSelector = IpmiLanSubnetMask;
    ResponseDataSize = sizeof (LanSubnetMask);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN2_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanSubnetMask,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_SUBNET_MASK2_VAL),
                L"%d.%d.%d.%d",
                LanSubnetMask.IpAddress[0], LanSubnetMask.IpAddress[1], LanSubnetMask.IpAddress[2], LanSubnetMask.IpAddress[3]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SUBNET_MASK2_VAL),
            L"%d.%d.%d.%d",
            0,0,0,0);
    }

    //
    //Get MAC address of Channel-2
    //
    GetLanParamCmd.ParameterSelector = IpmiLanMacAddress;
    ResponseDataSize = sizeof (LanMacAddress);
    
    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN2_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanMacAddress,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_MAC2_VAL),
                L"%02x-%02x-%02x-%02x-%02x-%02x",
                LanMacAddress.MacAddress[0], LanMacAddress.MacAddress[1], LanMacAddress.MacAddress[2], LanMacAddress.MacAddress[3], LanMacAddress.MacAddress[4], LanMacAddress.MacAddress[5]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_MAC2_VAL),
            L"%02x-%02x-%02x-%02x-%02x-%02x",
            0,0,0,0,0,0);
    }

    //
    //Get Router Gateway IP Address of Channel-2
    //
    GetLanParamCmd.ParameterSelector = IpmiLanDefaultGateway;
    ResponseDataSize = sizeof (LanIPAddress);
    
    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN2_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanIPAddress,
                    &ResponseDataSize );

        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_ROUTER_IP2_VAL),
                L"%d.%d.%d.%d",
                LanIPAddress.IpAddress[0], LanIPAddress.IpAddress[1], LanIPAddress.IpAddress[2], LanIPAddress.IpAddress[3]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_ROUTER_IP2_VAL),
            L"%d.%d.%d.%d",
            0,0,0,0);
    }

    //
    //Get Router MAC address of Channel-2
    //
    GetLanParamCmd.ParameterSelector = IpmiLanDefaultGatewayMac;
    ResponseDataSize = sizeof (LanMacAddress);
    
    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (BMC_LAN2_CHANNEL_NUMBER);
    if (!EFI_ERROR (Status)) {

        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetLanParamCmd,
                    CommandDataSize,
                    (UINT8*)&LanMacAddress,
                    &ResponseDataSize );
        if (!EFI_ERROR (Status)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_ROUTER_MAC2_VAL),
                L"%02x-%02x-%02x-%02x-%02x-%02x",
                LanMacAddress.MacAddress[0], LanMacAddress.MacAddress[1], LanMacAddress.MacAddress[2], LanMacAddress.MacAddress[3], LanMacAddress.MacAddress[4], LanMacAddress.MacAddress[5]);
        } 
    }
    if (EFI_ERROR (Status)) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_ROUTER_MAC2_VAL),
            L"%02x-%02x-%02x-%02x-%02x-%02x",
            0,0,0,0,0,0);
    }
#endif //#if BMC_LAN_COUNT == 2

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Returning...\n", __FUNCTION__));
#if SMCPKG_SUPPORT
    //
    // Get IPMI LAN Selection.
    // 0x30 0x70 0x0C 0x00
    //
    CommandData[0] = 0x0c;
    CommandData[1] = 00;
    ResponseDataSize = 1;
    Status = IpmiWaitSetInProgressClear(BMC_LAN1_CHANNEL_NUMBER);

    if(!EFI_ERROR(Status)){
	Status = gIpmiTransport->SendIpmiCommand(
			gIpmiTransport,
			0x30,
			BMC_LUN,	//Lun, 0x00
			0x70,		//OEM Command, 0x70
			CommandData,
			2,
			ResponseData,
			&ResponseDataSize);

	if(!EFI_ERROR(Status)){
	    ServerConfiguration.IPMILANSelection = ResponseData[0];
	}
    }
    //
    // Get IPMI LAN Selection.
    // 0x30 0x70 0x60 0x00
    //
    CommandData[0] = 0x60;
    CommandData[1] = 00;
    ResponseDataSize = 10;
    Status = IpmiWaitSetInProgressClear(BMC_LAN1_CHANNEL_NUMBER);
    if(!EFI_ERROR(Status)){
	Status = gIpmiTransport->SendIpmiCommand(
			gIpmiTransport,
			0x30,
			BMC_LUN,	//Lun, 0x00
			0x70,		//OEM Command, 0x70
			CommandData,
			2,
			ResponseData,
			&ResponseDataSize);
    }
    if(!EFI_ERROR(Status)){
	switch(ResponseData[1] & 0x01){
	case 0:
	    if(ResponseData[4] & 0x01)
		InitString(HiiHandle, STRING_TOKEN(STR_LAN_STATUS_VALUE), L"%a", "Dedicated LAN");
	    else
		InitString(HiiHandle, STRING_TOKEN(STR_LAN_STATUS_VALUE), L"%a", "No Connect");
	    break;
	case 1:
	    if(ResponseData[7] & 0x01)
		InitString(HiiHandle, STRING_TOKEN(STR_LAN_STATUS_VALUE), L"%a", "Shared LAN");
	    else
		InitString(HiiHandle, STRING_TOKEN(STR_LAN_STATUS_VALUE), L"%a", "No Connect");
	    break;
	default:
	    InitString(HiiHandle, STRING_TOKEN(STR_LAN_STATUS_VALUE), L"%a", "No Connect");
	    break;
	}
    }
    
    //
    // Get IPMI VLAN Status.
    //
    CommandData[0] = BMC_LAN1_CHANNEL_NUMBER;
    CommandData[1] = 20;
    CommandDataSize = GET_LAN_COMMAND_DATA_SIZE;
    ResponseDataSize = sizeof(ResponseData);
    Status = IpmiWaitSetInProgressClear(BMC_LAN1_CHANNEL_NUMBER);
    if(!EFI_ERROR(Status)){
	Status = gIpmiTransport->SendIpmiCommand(
			gIpmiTransport,
			IPMI_NETFN_TRANSPORT,
			BMC_LUN,
			IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
			CommandData,
			CommandDataSize,
			ResponseData,
			&ResponseDataSize);
    }    
    if(!Status){
    	ServerConfiguration.SmcBmcVlan = ResponseData[2] >> 7;
    	ServerConfiguration.SmcBmcVlanID = ((ResponseData[2] & 0x7f) << 8) + ResponseData[1];
    }

    VarSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);

    Status = pST->RuntimeServices->SetVariable(
               L"ServerSetup",
               &ServerConfigurationGuid,
               Attributes,   // Attributes
               VarSize,
               &ServerConfiguration );
#endif
    return;
}

/**
    This function validate the given ip and display error messages
    if given input is invalid data

    @param HiiHandle A handle that was previously registered in the
                     HII Database.
    @param Class    Formset Class of the Form Callback Protocol passed in
    @param SubClass Formset sub Class of the Form Callback Protocol passed in
    @param Key Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS Return Status

**/
EFI_STATUS
BmcLanIpv6ConfigCallbackFunction(
  IN  EFI_HII_HANDLE     HiiHandle,
  IN  UINT16             Class,
  IN  UINT16             SubClass,
  IN  UINT16             Key )
{
    SERVER_MGMT_CONFIGURATION_DATA      *ServerMgmtConfiguration = NULL;
    CHAR16                              OutputString[STRING_BUFFER_LENGTH];
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CHAR16                              CharPtr[STRING_BUFFER_LENGTH];
    EFI_STATUS                          OutputStrStatus = EFI_SUCCESS;
    UINTN                               BufferSize = STRING_BUFFER_LENGTH;
    IPMI_IPV6_ADDRESS                   Ipv6Address;
    CHAR16                              StaticIp6Lan[40];

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Entered with Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( (Key != BMC_NETWORK_STATION_IP6_LAN1_KEY) && \
        (Key != BMC_NETWORK_STATION_IP6_LAN2_KEY)) {
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = pBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Status of HiiLibGetBrowserData() = %r\n",Status));
    ASSERT_EFI_ERROR (Status);
#if ( TSE_BUILD > 0x1208 )
    {
        //
        // Get the call back parameters and verify the action
        //
		CALLBACK_PARAMETERS *CallbackParameters = GetCallbackParameters();
        if ( CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ) {
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so load defaults \n"));
            pBS->FreePool(ServerMgmtConfiguration);
            return  EFI_UNSUPPORTED;
        } else if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
            pBS->FreePool(ServerMgmtConfiguration);
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\n IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
            return  EFI_SUCCESS;
        }
    }
#endif //#if (TSE_BUILD > 0x1208)

    if (Key == BMC_NETWORK_STATION_IP6_LAN1_KEY) {
        //
        // Validate Ipv6 value, for error case, display error message.
        // Local copy is made to prevent ASSERT due to alignment checking in ConvertStrToBmcIp6Address
        //
        CopyMem ((UINT8*)StaticIp6Lan, (UINT8*)ServerMgmtConfiguration->StaticIp6Lan1, 40 * 2);
        Status = ConvertStrToBmcIp6Address (
                    StaticIp6Lan,
                    &Ipv6Address);
        if (EFI_ERROR(Status)) {
            SetZeroIp6 (ServerMgmtConfiguration->StaticIp6Lan1);
            OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_BMC_IPV6_ADDRESS), OutputString, &BufferSize, NULL);
        }
    }

#if BMC_LAN_COUNT == 2
    if (Key == BMC_NETWORK_STATION_IP6_LAN2_KEY) {
        //
        // Validate Ipv6 value, for error case, display error message
        // Local copy is made to prevent ASSERT due to alignment checking in ConvertStrToBmcIp6Address
        //
        CopyMem ((UINT8*)StaticIp6Lan, (UINT8*)ServerMgmtConfiguration->StaticIp6Lan2, 40 * 2);
        Status = ConvertStrToBmcIp6Address (
                    StaticIp6Lan,
                    &Ipv6Address);
        if (EFI_ERROR(Status)) {
            SetZeroIp6 (ServerMgmtConfiguration->StaticIp6Lan2);
            OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_BMC_IPV6_ADDRESS), OutputString, &BufferSize, NULL);
        }
    }
#endif

     if ( EFI_ERROR (Status) ) {

        AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr = NULL;
        UINT8                       MsgBoxSel;

        //
        // Locate AmiPostMgr protocol to print the Error message
        //
        if ( !EFI_ERROR(pBS->LocateProtocol
             (&gAmiPostManagerProtocolGuid, NULL, (VOID **)&AmiPostMgr)) ) {
            BufferSize = STRING_BUFFER_LENGTH;
            Status = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMCLAN_ERROR_INFO), CharPtr, &BufferSize, NULL);
            if ( (!EFI_ERROR (OutputStrStatus)) && (!EFI_ERROR (Status))) {
                AmiPostMgr->DisplayMsgBox(
                    CharPtr,
                    OutputString,
                    MSGBOX_TYPE_OK,
                    &MsgBoxSel );
            }
        }

        //
        // Set Browser DATA
        //
        Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Status of HiiLibSetBrowserData() = %r\n",Status));
    } // EFI_ERROR (Status)

    pBS->FreePool(ServerMgmtConfiguration);

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Exiting... \n", __FUNCTION__));

    return EFI_SUCCESS;
}

/**
    This function validate the given ip and display error messages
    if given input is invalid data

    @param HiiHandle A handle that was previously registered in the
                     HII Database.
    @param Class    Formset Class of the Form Callback Protocol passed in
    @param SubClass Formset sub Class of the Form Callback Protocol passed in
    @param Key Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS Return Status

**/
EFI_STATUS
BmcLanIpv6Router1ConfigCallbackFunction(
  IN  EFI_HII_HANDLE     HiiHandle,
  IN  UINT16             Class,
  IN  UINT16             SubClass,
  IN  UINT16             Key )
{
    SERVER_MGMT_CONFIGURATION_DATA     *ServerMgmtConfiguration = NULL;
    CHAR16                              OutputString[STRING_BUFFER_LENGTH];
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINTN                               SelectionBufferSize;
    CHAR16                              CharPtr[STRING_BUFFER_LENGTH];
    EFI_STATUS                          OutputStrStatus = EFI_SUCCESS;
    UINTN                               BufferSize = STRING_BUFFER_LENGTH;
    IPMI_IPV6_ADDRESS                   Ipv6Address;
    CHAR16                              StaticIp6Lan[40];

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Entered with Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ( (Key != BMC_NETWORK_STATION_IP6_ROUTER1_LAN2_KEY) && \
        (Key != BMC_NETWORK_STATION_IP6_ROUTER1_LAN1_KEY)) {
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = pBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Status of HiiLibGetBrowserData() = %r\n",Status));
    ASSERT_EFI_ERROR (Status);
#if ( TSE_BUILD > 0x1208 )
    {
        //
        // Get the call back parameters and verify the action
        //
        CALLBACK_PARAMETERS *CallbackParameters = GetCallbackParameters();
        if ( CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ) {
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"IPMI: CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD... so load defaults \n"));
            pBS->FreePool(ServerMgmtConfiguration);
            return  EFI_UNSUPPORTED;
        } else if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
            pBS->FreePool(ServerMgmtConfiguration);
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\n IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
            return  EFI_SUCCESS;
        }
    }
#endif //#if (TSE_BUILD > 0x1208)

    if (Key == BMC_NETWORK_STATION_IP6_ROUTER1_LAN1_KEY) {
        //
        // Validate Ipv6 value, for error case, display error message.
        // Local copy is made to prevent ASSERT due to alignment checking in ConvertStrToBmcIp6Address
        //
        CopyMem ((UINT8*)StaticIp6Lan, (UINT8*)ServerMgmtConfiguration->StaticIp6Lan1Router1Ip, 40 * 2);
        Status = ConvertStrToBmcIp6Address (
                    StaticIp6Lan,
                    &Ipv6Address);
        if (EFI_ERROR(Status)) {
            SetZeroIp6 (ServerMgmtConfiguration->StaticIp6Lan1Router1Ip);
            OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_BMC_IPV6_ADDRESS), OutputString, &BufferSize, NULL);
        }
    }

#if BMC_LAN_COUNT == 2
    if (Key == BMC_NETWORK_STATION_IP6_ROUTER1_LAN2_KEY) {
        //
        // Validate Ipv6 value, for error case, display error message
        // Local copy is made to prevent ASSERT due to alignment checking in ConvertStrToBmcIp6Address
        //
        CopyMem ((UINT8*)StaticIp6Lan, (UINT8*)ServerMgmtConfiguration->StaticIp6Lan2Router1Ip, 40 * 2);
        Status = ConvertStrToBmcIp6Address (
                    StaticIp6Lan,
                    &Ipv6Address);
        if (EFI_ERROR(Status)) {
            SetZeroIp6 (ServerMgmtConfiguration->StaticIp6Lan2Router1Ip);
            OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_INVALID_BMC_IPV6_ADDRESS), OutputString, &BufferSize, NULL);
        }
    }
#endif

     if ( EFI_ERROR (Status) ) {

        AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr = NULL;
        UINT8                       MsgBoxSel;

        //
        // Locate AmiPostMgr protocol to print the Error message
        //
        if ( !EFI_ERROR(pBS->LocateProtocol
             (&gAmiPostManagerProtocolGuid, NULL, (VOID **)&AmiPostMgr)) ) {
            BufferSize = STRING_BUFFER_LENGTH;
            Status = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_BMCLAN_ERROR_INFO), CharPtr, &BufferSize, NULL);
            if ( (!EFI_ERROR (OutputStrStatus)) && (!EFI_ERROR (Status))) {
                AmiPostMgr->DisplayMsgBox(
                    CharPtr,
                    OutputString,
                    MSGBOX_TYPE_OK,
                    &MsgBoxSel );
            }
        }

        //
        // Set Browser DATA
        //
        Status = HiiLibSetBrowserData (
                    SelectionBufferSize,
                    ServerMgmtConfiguration,
                    &gEfiServerMgmtSetupVariableGuid,
                    L"ServerSetup" );
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Status of HiiLibSetBrowserData() = %r\n",Status));
    } // EFI_ERROR (Status)

    pBS->FreePool(ServerMgmtConfiguration);

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Exiting... \n", __FUNCTION__));

    return EFI_SUCCESS;
}

/**
    This function changes the Ipv6 Support option.

    @param HiiHandle A handle that was previously registered in the
                     HII Database.
    @param Class    Formset Class of the Form Callback Protocol passed in
    @param SubClass Formset sub Class of the Form Callback Protocol passed in
    @param Key Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS Return Status

**/

EFI_STATUS
BmcLanIpv6SupportCallbackFunction(
  IN  EFI_HII_HANDLE     HiiHandle,
  IN  UINT16             Class,
  IN  UINT16             SubClass,
  IN  UINT16             Key )
{
    SERVER_MGMT_CONFIGURATION_DATA            *ServerMgmtConfiguration = NULL;
    EFI_STATUS                                Status = EFI_SUCCESS;
    UINTN                                     SelectionBufferSize;
    EFI_SET_LAN_CONFIG_IP_4_6_SUPPORT_ENABLE  SetParam;
    UINT8                                     ResponseDataSize = 0;
    UINT8                                     DataSize;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Entered with Key: %x \n", __FUNCTION__, Key));

    //
    // Check for the key and Return if Key value does not match
    //
    if ((Key != BMC_IPV6_SUPPORT_LAN1_KEY) && \
        (Key != BMC_IPV6_SUPPORT_LAN2_KEY)) {
        SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Callback function is called with Wrong Key Value. \
                                              Returning EFI_UNSUPPORTED\n"));
        return EFI_UNSUPPORTED;
    }

    //
    // Allocate memory for SERVER_MGMT_CONFIGURATION_DATA
    //
    SelectionBufferSize = sizeof (SERVER_MGMT_CONFIGURATION_DATA);
    Status = pBS->AllocatePool (
                EfiBootServicesData,
                SelectionBufferSize,
                (VOID **)&ServerMgmtConfiguration );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Browser DATA
    //
    Status = HiiLibGetBrowserData (
                &SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Status of HiiLibGetBrowserData() = %r\n",Status));
    ASSERT_EFI_ERROR (Status);
#if (TSE_BUILD > 0x1208)
    {
        //
        // Get the call back parameters and verify the action
        //
        CALLBACK_PARAMETERS *CallbackParameters = GetCallbackParameters();
        if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED ) {
            pBS->FreePool(ServerMgmtConfiguration);
            SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\n IPMI: CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED... so return EFI_SUCCESS\n"));
            return  EFI_SUCCESS;
        }
    }
#endif //#if (TSE_BUILD > 0x1208)

    if (Key == BMC_IPV6_SUPPORT_LAN1_KEY) {
        DataSize = sizeof (SetParam);
        SetParam.LanChannel.ChannelNumber = BMC_LAN1_CHANNEL_NUMBER;
        SetParam.LanChannel.GetParam = 0;
        SetParam.LanChannel.Reserved = 0;
        SetParam.Param = EfiIpv4OrIpv6AddressEnable;
        if (ServerMgmtConfiguration->Ip6Support1 == 0) {
            SetParam.Data1 = 0;
        } else if (ServerMgmtConfiguration->Ip6Support1 == 1) {
            SetParam.Data1 = 2;
        }
        
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&SetParam,
                    DataSize,
                    NULL,
                    &ResponseDataSize );
    }

#if BMC_LAN_COUNT == 2
    if (Key == BMC_IPV6_SUPPORT_LAN2_KEY) {
        DataSize = sizeof (SetParam);
        SetParam.LanChannel.ChannelNumber = BMC_LAN2_CHANNEL_NUMBER;
        SetParam.LanChannel.GetParam = 0;
        SetParam.LanChannel.Reserved = 0;
        SetParam.Param = EfiIpv4OrIpv6AddressEnable;
        if (ServerMgmtConfiguration->Ip6Support2 == 0) {
            SetParam.Data1 = 0;
        } else if (ServerMgmtConfiguration->Ip6Support2 == 1) {
            SetParam.Data1 = 2;
        }
        
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&SetParam,
                    DataSize,
                    NULL,
                    &ResponseDataSize );
    }
#endif

    //
    // Set Browser DATA
    //
    Status = HiiLibSetBrowserData (
                SelectionBufferSize,
                ServerMgmtConfiguration,
                &gEfiServerMgmtSetupVariableGuid,
                L"ServerSetup" );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"\nIPMI: Status of HiiLibSetBrowserData() = %r\n",Status));

    pBS->FreePool(ServerMgmtConfiguration);

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Exiting... \n", __FUNCTION__));

    return EFI_SUCCESS;
}

/**
    This function update Ipv6 LAN parameters data.

    @param HiiHandle         A handle that was previously registered in the
                             HII Database.
    @param LanChannelNumber  Channel need to be updated.

    @return VOID

**/

VOID
UpdateIp6Parameters(
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT8 LanChannelNumber )
{

    EFI_STATUS                    Status;
    EFI_IPMI_GET_LAN_CONFIG_PRAM  GetParamCommand;
    EFI_IPMI_GET_IPV6_ADDRESS     Ipv6AddressData;
    EFI_IPMI_GET_IPV6_STATIC_ROUTER1_IP_ADDRESS     GetIpv6StaticRouterAddressData;
    EFI_IPMI_GET_IPV6_DYNAMIC_ROUTER1_IP_ADDRESS    GetIpv6DynamicRouterAddressData;
    UINT8                         DataSize;
    UINT8                         IsIpv6Static = 1;
    CHAR16                        CharPtr[STRING_BUFFER_LENGTH];
    CHAR16                        CharPtr1[STRING_BUFFER_LENGTH];
    EFI_STATUS                    CharPtrStatus = EFI_SUCCESS;
    UINTN                         BufferLength = STRING_BUFFER_LENGTH;
    UINT16                        IP6Status [7] = { 
                                                 STRING_TOKEN(STR_BMC_IPV6_ADDRESS_STATUS_ACTIVE),
                                                 STRING_TOKEN(STR_BMC_IPV6_ADDRESS_STATUS_DISABLE),
                                                 STRING_TOKEN(STR_BMC_IPV6_ADDRESS_STATUS_PENDING),
                                                 STRING_TOKEN(STR_BMC_IPV6_ADDRESS_STATUS_FAILED),
                                                 STRING_TOKEN(STR_BMC_IPV6_ADDRESS_STATUS_DEPRECATED),
                                                 STRING_TOKEN(STR_BMC_IPV6_ADDRESS_STATUS_INVALID),
                                                 STRING_TOKEN(STR_BMC_IPV6_ADDRESS_STATUS_RESERVED)
                                                 };
    UINT8                        IP6StatusIndex = 0;


    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Entry...\n", __FUNCTION__));

    //
    //Get IPV6 address
    //
    GetParamCommand.LanChannel.ChannelNumber = LanChannelNumber;
    GetParamCommand.LanChannel.GetParam = 0;
    GetParamCommand.LanChannel.Reserved = 0;
    GetParamCommand.ParameterSelector = EfiIpv6StaticAddress;
    GetParamCommand.BlockSelector = 0;
    GetParamCommand.SetSelector = 0;
    DataSize = sizeof (Ipv6AddressData);

    //
    // Wait until Set In Progress field is cleared
    //
    Status = IpmiWaitSetInProgressClear (LanChannelNumber);
    if (EFI_ERROR(Status)) {
       return;
    }

    Status = gIpmiTransport->SendIpmiCommand (
                gIpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                (UINT8*)&GetParamCommand,
                sizeof (GetParamCommand),
                (UINT8*)&Ipv6AddressData,
                &DataSize );

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS Status: %r\n", Status));

    if (EFI_ERROR(Status)) {
        return;
    }

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : Static Support: %d\n", Ipv6AddressData.Ip6AddressParam.AddressSource.EnableStat));

    //
    // Update the address source.
    //

    if (Ipv6AddressData.Ip6AddressParam.AddressSource.EnableStat == 0) {
        IsIpv6Static = 0;
    }

    if (IsIpv6Static) {
        
        //
        //Get Router IPV6 address
        //
        GetParamCommand.LanChannel.ChannelNumber = LanChannelNumber;
        GetParamCommand.LanChannel.GetParam = 0;
        GetParamCommand.LanChannel.Reserved = 0;
        GetParamCommand.ParameterSelector = EfiIpv6StaticRouter1IpAddr;
        GetParamCommand.BlockSelector = 0;
        GetParamCommand.SetSelector = 0;
        DataSize = sizeof (GetIpv6StaticRouterAddressData);
        
        //
        // Wait until Set In Progress field is cleared
        //
        Status = IpmiWaitSetInProgressClear (LanChannelNumber);
        if (EFI_ERROR(Status)) {
           return;
        }
        
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetParamCommand,
                    sizeof (GetParamCommand),
                    (UINT8*)&GetIpv6StaticRouterAddressData,
                    &DataSize );
        
        SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS Status: %r\n", Status));
        
        if (EFI_ERROR(Status)) {
            return;
        }
        
        ConvertBmcIp6ToStr(&GetIpv6StaticRouterAddressData.Ip6AddressParam.Ipv6Address, CharPtr1, sizeof(CharPtr1));
        SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : Ipv6 string: %s \n", CharPtr1));

        //
        // Get the address source mapping UNI string.
        //
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_IPSOURCE_STATIC_ADDRESS), CharPtr, &BufferLength, NULL);
    } else {
        
        //
        //Get Router1 IPV6 address
        //
        GetParamCommand.LanChannel.ChannelNumber = LanChannelNumber;
        GetParamCommand.LanChannel.GetParam = 0;
        GetParamCommand.LanChannel.Reserved = 0;
        GetParamCommand.ParameterSelector = EfiIpv6DyanamicRouterIpAddr;
        GetParamCommand.BlockSelector = 0;
        GetParamCommand.SetSelector = 0;
        DataSize = sizeof (GetIpv6DynamicRouterAddressData);
        
        //
        // Wait until Set In Progress field is cleared
        //
        Status = IpmiWaitSetInProgressClear (LanChannelNumber);
        if (EFI_ERROR(Status)) {
           return;
        }
        
        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetParamCommand,
                    sizeof (GetParamCommand),
                    (UINT8*)&GetIpv6DynamicRouterAddressData,
                    &DataSize );
        
        SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS Status: %r\n", Status));
        
        if (EFI_ERROR(Status)) {
            return;
        }
        
        ConvertBmcIp6ToStr (&GetIpv6DynamicRouterAddressData.Ip6AddressParam.Ipv6Address, CharPtr1, sizeof (CharPtr1));
        SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : Ipv6 string: %s \n", CharPtr1));

        //
        // Get the address source mapping UNI string.
        //
        CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_IPSOURCE_DYNAMIC_ADDRESS_BMC_DHCP), CharPtr, &BufferLength, NULL);

        //
        // Update the parameter for getting Dynamic address. 
        // Once again command will be sent to get dynamic address.
        //
        GetParamCommand.LanChannel.ChannelNumber = LanChannelNumber;
        GetParamCommand.LanChannel.GetParam = 0;
        GetParamCommand.LanChannel.Reserved = 0;
        GetParamCommand.BlockSelector = 0;
        GetParamCommand.SetSelector = 0;
        GetParamCommand.ParameterSelector = EfiIpv6DhcpAddress;
    }

    if (!EFI_ERROR(CharPtrStatus)) {

        if (LanChannelNumber == BMC_LAN1_CHANNEL_NUMBER) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_IP6_CURR_LANCAS1_VAL),
                CharPtr );
        }
#if BMC_LAN_COUNT == 2
        if (LanChannelNumber == BMC_LAN2_CHANNEL_NUMBER) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_IP6_CURR_LANCAS2_VAL),
                CharPtr );
        }
#endif
    }

    //
    // Update address parameter if Dynamic is selected.
    //
    if (!IsIpv6Static) {
        DataSize = sizeof (Ipv6AddressData);

        Status = gIpmiTransport->SendIpmiCommand (
                    gIpmiTransport,
                    IPMI_NETFN_TRANSPORT,
                    BMC_LUN,
                    IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                    (UINT8*)&GetParamCommand,
                    sizeof (GetParamCommand),
                    (UINT8*)&Ipv6AddressData,
                    &DataSize );

        SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS Status: %r\n", Status));

        if (EFI_ERROR(Status)) {
            return;
        }
    }

    //
    // Update Ipv6 address.
    //
    ConvertBmcIp6ToStr(&Ipv6AddressData.Ip6AddressParam.Ipv6Address, CharPtr, sizeof (CharPtr));
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : Ipv6 string: %S\n", CharPtr));

    if (LanChannelNumber == BMC_LAN1_CHANNEL_NUMBER) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP6_LAN1_VAL),
            CharPtr );
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP6_LAN1_PREFIX_LENGTH_VAL),
            L"%d",
            Ipv6AddressData.Ip6AddressParam.AddressPrefixLen);
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP6_ROUTER_ADDRESS1_VAL),
            CharPtr1 );
    }
#if BMC_LAN_COUNT == 2
    if (LanChannelNumber == BMC_LAN2_CHANNEL_NUMBER) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP6_LAN2_VAL),
            CharPtr );
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP6_LAN2_PREFIX_LENGTH_VAL),
            L"%d",
            Ipv6AddressData.Ip6AddressParam.AddressPrefixLen );
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_STATION_IP6_ROUTER_ADDRESS2_VAL),
            CharPtr1 );
    }
#endif


    //
    // Update the algorithm used.
    //
    if (!IsIpv6Static) {
        if (Ipv6AddressData.Ip6AddressParam.AddressSource.Type == EFI_DHCP_SLAAC) {
           CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_STATION_IP6_LAN_DHCP_SLAAC), CharPtr, &BufferLength, NULL);
        }
        else if (Ipv6AddressData.Ip6AddressParam.AddressSource.Type == EFI_DHCP_DHCP6) {
           CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_STATION_IP6_LAN_DHCP_DHCP6), CharPtr, &BufferLength, NULL);
        } else {
           CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_STATION_IP6_LAN_DHCP_DHCP6), CharPtr, &BufferLength, NULL);
        }
    } else {
           //
           // For Static source algorithm is not defined.
           //
           CharPtrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_UNKNOWN_STRING), CharPtr, &BufferLength, NULL);
    }

    if (LanChannelNumber == BMC_LAN1_CHANNEL_NUMBER) {
        if (!EFI_ERROR(CharPtrStatus)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_IP6_LAN1_DHCP_ALGO),
                CharPtr );
        }
    }
#if BMC_LAN_COUNT == 2
    if (LanChannelNumber == BMC_LAN2_CHANNEL_NUMBER) {
        if (!EFI_ERROR(CharPtrStatus)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_IP6_LAN2_DHCP_ALGO),
                CharPtr );
        }
    }
#endif

    //
    // Update the address status.
    //
    if (Ipv6AddressData.Ip6AddressParam.AddressStatus > 5) {
        IP6StatusIndex = 5;
    } else {
       IP6StatusIndex = Ipv6AddressData.Ip6AddressParam.AddressStatus;
    }

    CharPtrStatus = IpmiHiiGetString (HiiHandle, IP6Status[IP6StatusIndex], CharPtr, &BufferLength, NULL);
    if (LanChannelNumber == BMC_LAN1_CHANNEL_NUMBER) {
        if(!EFI_ERROR(CharPtrStatus)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_IP6_LAN1_ADD_STATUS),
                CharPtr );
        }
    }
#if BMC_LAN_COUNT == 2
    if (LanChannelNumber == BMC_LAN2_CHANNEL_NUMBER) {
        if(!EFI_ERROR(CharPtrStatus)) {
            InitString (
                HiiHandle,
                STRING_TOKEN(STR_STATION_IP6_LAN2_ADD_STATUS),
                CharPtr );
        }
    }
#endif
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Exiting...\n", __FUNCTION__));
    return;
}

/**
    Reads Ipv6 parameter from BMC and display in setup.

    @param HiiHandle A handle that was previously registered in the
                     HII Database.
    @param Class    Formset Class of the Form Callback Protocol passed in
    @param SubClass Formset sub Class of the Form Callback Protocol passed in
    @param Key Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS Return Status

**/

EFI_STATUS
Ip6BmcLanParamSetupDisplayCallback(
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class,
  IN  UINT16        SubClass,
  IN  UINT16        Key )
{

    EFI_STATUS                         Status;
    SERVER_MGMT_CONFIGURATION_DATA     ServerMgmtConfiguration;
    UINTN                              Size;
    UINT32                             Attributes;

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, " Entry...\n"));

    //
    // Locate the IPMI transport protocol
    //
    Status = pBS->LocateProtocol (
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&gIpmiTransport );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS, " IPMI : gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
    if ( EFI_ERROR (Status) ) {
        return EFI_UNSUPPORTED;
    }

    //
    // Get ServerSetup Variable to find if Ipv6 supported or not. 
    //
    Size = sizeof (ServerMgmtConfiguration);
    Status = gRT->GetVariable (
                L"ServerSetup",
                &gEfiServerMgmtSetupVariableGuid,
                &Attributes,
                &Size,
                &ServerMgmtConfiguration );
    SERVER_IPMI_TRACE ((TRACE_ALWAYS,"IPMI : Status of ServerSetup GetVariable() = %r\n",Status));

    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    if (ServerMgmtConfiguration.Ip6Support1 != 0) {
        UpdateIp6Parameters(HiiHandle, BMC_LAN1_CHANNEL_NUMBER);
    }
#if BMC_LAN_COUNT == 2
    if (ServerMgmtConfiguration.Ip6Support2 != 0) {
       UpdateIp6Parameters(HiiHandle, BMC_LAN2_CHANNEL_NUMBER);
    }
#endif

    SERVER_IPMI_TRACE ((TRACE_ALWAYS, "%s Exiting...\n", __FUNCTION__));
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
