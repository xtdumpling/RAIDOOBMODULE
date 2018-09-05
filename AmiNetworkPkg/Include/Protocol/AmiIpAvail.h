//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file AmiIpAvail.h
    This file defines AMI defined Protocol interface to
    publish availability of IPv4 (Internet Protocol version 4) 
    and IPv6 (Internet Protocol version 6) Protocol interfaces. 

**/
//*************************************************************************

#ifndef _AMI_IP_AVAIL_H_
#define _AMI_IP_AVAIL_H_

#define AMI_IP_AVAIL_PROTOCOL_GUID \
  { \
    0xd4e3c8b9, 0x548b, 0x4dcb, {0x80, 0x42, 0xba, 0x3c, 0xe7, 0x47, 0xf6, 0xf0 } \
  }

#define	IPVX_NA         0x00
#define	IPVX_AVAIL      0x01

#pragma pack(1)
typedef struct {
  ///
  /// IPv4 Protocol flag
  ///
  UINT8                   IPv4_Avail:1;

  ///
  /// IPv6 Protocol flag
  ///
  UINT8                   IPv6_Avail:1;
  UINT8                   Reserved:6;
} AMI_IP_AVAIL_PROTOCOL;
#pragma pack()

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
