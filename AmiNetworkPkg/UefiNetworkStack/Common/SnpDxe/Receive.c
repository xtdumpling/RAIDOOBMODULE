/** @file
  Implementation of receiving a packet from a network interface.

Copyright (c) 2004 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed
and made available under the terms and conditions of the BSD License which
accompanies this distribution. The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "Snp.h"
#include <Token.h>

// AMI PORTING STARTS
#if (SNP_SPEEDUP == 1)

#include "../ArpDxe/ArpImpl.h"
#include <Protocol/Ip4Config2.h>

/**
  Gets the Ipaddress from SnpHandle and compares with the destination IP

  @param  SnpHandle    SnpHandle value
  @param  DestIP       Destination ip to match with

  @retval EFI_STATUS  returns EFI_SUCCESS if IP matched.

**/
EFI_STATUS MatchIP(EFI_HANDLE SnpHandle,IP4_ADDR   	 *DestIP)
{
	EFI_IP4_CONFIG2_PROTOCOL *Ip4Config2Protocol = NULL;
	EFI_IP4_CONFIG2_INTERFACE_INFO   *IfInfo= NULL;
	extern EFI_GUID		  gEfiIp4ConfigProtocolGuid;
	EFI_STATUS		Status = EFI_SUCCESS;
	EFI_STATUS		OverallStatus = EFI_NOT_FOUND;	
	  
	Status = gBS->HandleProtocol (SnpHandle, &gEfiIp4Config2ProtocolGuid, &Ip4Config2Protocol);

	if (!EFI_ERROR (Status))
	{
		UINTN ConfigDataSize = 0;
		Status = Ip4Config2Protocol->GetData (Ip4Config2Protocol,Ip4Config2DataTypeInterfaceInfo, &ConfigDataSize, IfInfo);

		if (EFI_BUFFER_TOO_SMALL == Status)
		{
			IfInfo = AllocateZeroPool (ConfigDataSize);
			Status = Ip4Config2Protocol->GetData (Ip4Config2Protocol,Ip4Config2DataTypeInterfaceInfo, &ConfigDataSize, IfInfo);
		}
		if (!EFI_ERROR (Status))
		{
			if(NULL != DestIP)
			{
				if ( (!CompareMem (&IfInfo->StationAddress.Addr, DestIP, sizeof (EFI_IPv4_ADDRESS))) )
					OverallStatus = EFI_SUCCESS;
			}
		}
		else
			OverallStatus = EFI_SUCCESS;
	}
	else
		OverallStatus = EFI_SUCCESS;
	
	if(IfInfo)
		FreePool(IfInfo);
	
	return OverallStatus;
}

/**
  Function to check HostNetWork match of snp interface with Destination IP address

  @param  Snp          Pointer to snp driver structure
  @param  DestIP       Destination ip to match with

  @retval EFI_STATUS  returns EFI_SUCCESS if matched.

**/
EFI_STATUS MatchHostNetWork(SNP_DRIVER      *Snp,IP4_ADDR   	 *DestIP)
{
	  extern EFI_GUID 	gEfiSimpleNetworkProtocolGuid;
	  EFI_HANDLE 		*SnpHandles = NULL;  
	  UINTN				HandleCount = 0, iIndex = 0;
	  EFI_SIMPLE_NETWORK_PROTOCOL	*SnpProtocol = NULL;
	  EFI_STATUS		Status = EFI_SUCCESS;
	  EFI_STATUS		OverallStatus = EFI_NOT_FOUND;	
	  static UINTN PrevMatchedSnpHandleIndex = 0XFFFF;
	  
	  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleNetworkProtocolGuid, NULL, &HandleCount, &SnpHandles);
	  if (!EFI_ERROR (Status))
	  {
		  if(0XFFFF != PrevMatchedSnpHandleIndex)
		  {
			  Status = gBS->HandleProtocol (SnpHandles [PrevMatchedSnpHandleIndex], &gEfiSimpleNetworkProtocolGuid, &SnpProtocol);
			  if ((UINTN)SnpProtocol == (UINTN)&Snp->Snp)
			  {
				  OverallStatus = MatchIP(SnpHandles [PrevMatchedSnpHandleIndex],DestIP);
				  return OverallStatus;
			  }
			  else
				  PrevMatchedSnpHandleIndex = 0XFFFF;
		  }
		  for (iIndex = 0; iIndex < HandleCount; iIndex ++)
		  {	  
			  Status = gBS->HandleProtocol (SnpHandles [iIndex], &gEfiSimpleNetworkProtocolGuid, &SnpProtocol);
			  if (!EFI_ERROR (Status))
			  {  
				  if ((UINTN)SnpProtocol == (UINTN)&Snp->Snp)
				  {
					  PrevMatchedSnpHandleIndex = iIndex;
					  OverallStatus = MatchIP(SnpHandles [iIndex],DestIP);
					  break;
				  }
			  }
		  }
	  }
	  FreePool(SnpHandles);
	  return OverallStatus;
}
#endif	// SNP_SPEEDUP
// AMI PORTING ENDS

/**
  Call UNDI to receive a packet and fills in the data in the input pointers.

  @param  Snp          Pointer to snp driver structure
  @param  Buffer       Pointer to the memory for the received data
  @param  BufferSize   Pointer to the length of the buffer on entry and contains
                       the length of the received data on return
  @param  HeaderSize   Pointer to the header portion of the data received.
  @param  SrcAddr      Pointer to contain the source ethernet address on return
  @param  DestAddr     Pointer to contain the destination ethernet address on
                       return
  @param  Protocol     Pointer to contain the protocol type from the ethernet
                       header on return


  @retval EFI_SUCCESS           The received data was stored in Buffer, and
                                BufferSize has been updated to the number of
                                bytes received.
  @retval EFI_DEVICE_ERROR      Fail to execute UNDI command.
  @retval EFI_NOT_READY         No packets have been received on the network
                                interface.
  @retval EFI_BUFFER_TOO_SMALL  BufferSize is too small for the received
                                packets. BufferSize has been updated to the
                                required size.

**/
EFI_STATUS
PxeReceive (
  SNP_DRIVER      *Snp,
  VOID            *Buffer,
  UINTN           *BufferSize,
  UINTN           *HeaderSize,
  EFI_MAC_ADDRESS *SrcAddr,
  EFI_MAC_ADDRESS *DestAddr,
  UINT16          *Protocol
  )
{
  PXE_CPB_RECEIVE *Cpb;
  PXE_DB_RECEIVE  *Db;
  UINTN           BuffSize;
// AMI PORTING STARTS
#if (SNP_SPEEDUP == 1)
  EFI_STATUS   	  OverallStatus = EFI_NOT_FOUND;

RECEIVEAGAIN:
  OverallStatus = EFI_NOT_FOUND;
#endif //SNP_SPEEDUP
// AMI PORTING ENDS
  
  Cpb       = Snp->Cpb;
  Db        = Snp->Db;
  BuffSize  = *BufferSize;

  Cpb->BufferAddr = (UINT64)(UINTN) Buffer;
  Cpb->BufferLen  = (UINT32) *BufferSize;

  Cpb->reserved       = 0;

  Snp->Cdb.OpCode     = PXE_OPCODE_RECEIVE;
  Snp->Cdb.OpFlags    = PXE_OPFLAGS_NOT_USED;

  Snp->Cdb.CPBsize    = (UINT16) sizeof (PXE_CPB_RECEIVE);
  Snp->Cdb.CPBaddr    = (UINT64)(UINTN) Cpb;

  Snp->Cdb.DBsize     = (UINT16) sizeof (PXE_DB_RECEIVE);
  Snp->Cdb.DBaddr     = (UINT64)(UINTN) Db;

  Snp->Cdb.StatCode   = PXE_STATCODE_INITIALIZE;
  Snp->Cdb.StatFlags  = PXE_STATFLAGS_INITIALIZE;
  Snp->Cdb.IFnum      = Snp->IfNum;
  Snp->Cdb.Control    = PXE_CONTROL_LAST_CDB_IN_LIST;

  //
  // Issue UNDI command and check result.
  //
  DEBUG ((EFI_D_NET, "\nsnp->undi.receive ()  "));

  (*Snp->IssueUndi32Command) ((UINT64)(UINTN) &Snp->Cdb);

  switch (Snp->Cdb.StatCode) {
  case PXE_STATCODE_SUCCESS:
    break;

  case PXE_STATCODE_NO_DATA:
    DEBUG (
      (EFI_D_NET,
      "\nsnp->undi.receive ()  %xh:%xh\n",
      Snp->Cdb.StatFlags,
      Snp->Cdb.StatCode)
      );

    return EFI_NOT_READY;

  default:
    DEBUG (
      (EFI_D_ERROR,
      "\nsnp->undi.receive()  %xh:%xh\n",
      Snp->Cdb.StatFlags,
      Snp->Cdb.StatCode)
      );

    return EFI_DEVICE_ERROR;
  }

  *BufferSize = Db->FrameLen;

  if (HeaderSize != NULL) {
    *HeaderSize = Db->MediaHeaderLen;
  }

  if (SrcAddr != NULL) {
    CopyMem (SrcAddr, &Db->SrcAddr, Snp->Mode.HwAddressSize);
  }

  if (DestAddr != NULL) {
    CopyMem (DestAddr, &Db->DestAddr, Snp->Mode.HwAddressSize);
  }

  if (Protocol != NULL) {
    //
    // We need to do the byte swapping
    //
    *Protocol = (UINT16) PXE_SWAP_UINT16 (Db->Protocol);
  }

  //
  // We have received a packet from network interface, which implies that the
  // network cable should be present. While, some UNDI driver may not report
  // correct media status during Snp->Initialize(). So, we need ensure
  // MediaPresent in SNP mode data is set to correct value.
  //
  if (Snp->Mode.MediaPresentSupported && !Snp->Mode.MediaPresent) {
    Snp->Mode.MediaPresent = TRUE;
  }
// AMI PORTING STARTS
#if (SNP_SPEEDUP == 1)
{
    ETHER_HEAD 		*EtherHeader = (ETHER_HEAD *) (UINTN) Buffer;
    EFI_STATUS 		Status = EFI_SUCCESS;
    IP4_HEAD          *Ip4 = NULL;
    EFI_IP6_HEADER	*Ip6 = NULL;
    EFI_UDP_HEADER	*Udp4 = NULL;
    
    if (EtherHeader)
    {
      if (EtherHeader->EtherType != HTONS (ARP_ETHER_PROTO_TYPE))	//ARP_ETHER_PROTO_TYPE  0x0608
      {
        Ip4 = (IP4_HEAD *)((UINT8 *)Buffer + (sizeof (ETHER_HEAD)));
        if (!CompareMem (Db->DestAddr, &Snp->Mode.CurrentAddress, Snp->Mode.HwAddressSize))
        {
            OverallStatus = EFI_SUCCESS;
        }
        else if(EtherHeader->EtherType == HTONS (ETHER_TYPE_VLAN))	// ETHER_TYPE_VLAN 0x0081	
        {
            OverallStatus = EFI_SUCCESS;
        }
        else if	(EtherHeader->EtherType == HTONS (IPV4_ETHER_PROTO_TYPE))	//0x0008; For PXE offer destination MAC will be FFFFs so check the ports; IPV4
        {
        
            Ip4 = (IP4_HEAD *)((UINT8 *)Buffer + (sizeof (ETHER_HEAD)));
            if (EFI_IP_PROTO_UDP == Ip4->Protocol)
            {
                Udp4 = (EFI_UDP_HEADER *)((UINT8 *)Buffer + (sizeof (ETHER_HEAD) + sizeof (IP4_HEAD)));
                //_DEBUG ((EFI_D_NET, "Udp4->SrcPort %d Udp4->DstPort %d\n", HTONS(Udp4->SrcPort), HTONS(Udp4->DstPort)));
                
                if ( (Udp4->SrcPort == HTONS (67)) || (Udp4->DstPort == HTONS (68)) ||
                    (Udp4->SrcPort == HTONS (137)) || (Udp4->DstPort == HTONS (137)) )
                {
                    OverallStatus = EFI_SUCCESS;
                }
            }
            else if(EFI_IP_PROTO_TCP == Ip4->Protocol)
            {
                if(EFI_SUCCESS == MatchHostNetWork(Snp,&Ip4->Dst))
                {
                    OverallStatus = EFI_SUCCESS;
                }
            }
        }
        else if	(EtherHeader->EtherType == HTONS (IPV6_ETHER_PROTO_TYPE))	//For PXE offer destination MAC will be FFFFs so check the ports; IPV6
        {
            Ip6 = (EFI_IP6_HEADER *)((UINT8 *)Buffer + (sizeof (ETHER_HEAD)));
            if (EFI_IP_PROTO_UDP == Ip6->NextHeader)
            {
                Udp4 = (EFI_UDP_HEADER *)((UINT8 *)Buffer + (sizeof (ETHER_HEAD) + sizeof (EFI_IP6_HEADER)));
                //_DEBUG ((EFI_D_NET, "Udp4->SrcPort %d Udp4->DstPort %d\n", HTONS(Udp4->SrcPort), HTONS(Udp4->DstPort)));
                
                if ( (Udp4->SrcPort == HTONS (546)) || (Udp4->DstPort == HTONS (547)) ||
                    (Udp4->SrcPort == HTONS (137)) || (Udp4->DstPort == HTONS (137)) )
                {
                    OverallStatus = EFI_SUCCESS;
                }
            }
            else if (IP6_ICMP == Ip6->NextHeader)		//Allow all packets of this type
            {
                OverallStatus = EFI_SUCCESS;
            }
        }
      }
      else
      {
          ARP_HEAD        	*ArpHead = NULL;
          ARP_ADDRESS     	ArpAddress;
          extern EFI_GUID 	gEfiSimpleNetworkProtocolGuid;
          EFI_HANDLE 		*SnpHandles = NULL;  
          UINTN				HandleCount = 0, iIndex = 0;
          EFI_SIMPLE_NETWORK_PROTOCOL	*SnpProtocol = NULL;
          
          ArpHead = (ARP_HEAD *)(EtherHeader + 1);
          
          Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleNetworkProtocolGuid, NULL, &HandleCount, &SnpHandles);
          if (!EFI_ERROR (Status))
          {
              for (iIndex = 0; iIndex < HandleCount; iIndex ++)
              {
                  Status = gBS->HandleProtocol (SnpHandles [iIndex], &gEfiSimpleNetworkProtocolGuid, &SnpProtocol);
                  if (!EFI_ERROR (Status))
                  {
                      if ((UINTN)SnpProtocol == (UINTN)&Snp->Snp)
                      {
                          EFI_IP4_CONFIG2_PROTOCOL *Ip4Config2Protocol = NULL;
                          EFI_IP4_CONFIG2_INTERFACE_INFO   *IfInfo= NULL;
                          extern EFI_GUID		  gEfiIp4ConfigProtocolGuid;
                          EFI_IPv4_ADDRESS ZeroIP = {0};
                          
                          Status = gBS->HandleProtocol (SnpHandles [iIndex], &gEfiIp4Config2ProtocolGuid, &Ip4Config2Protocol);
                          if (!EFI_ERROR (Status))
                          {
                              UINTN ConfigDataSize = 0;
                              
                              Status = Ip4Config2Protocol->GetData (Ip4Config2Protocol, Ip4Config2DataTypeInterfaceInfo,&ConfigDataSize, IfInfo);
                              //_DEBUG ((EFI_D_NET, "GetData Status is %r\n", Status));
                              if (EFI_BUFFER_TOO_SMALL == Status)
                              {
                                  IfInfo = AllocateZeroPool (ConfigDataSize);
                                  Status = Ip4Config2Protocol->GetData (Ip4Config2Protocol, Ip4Config2DataTypeInterfaceInfo,&ConfigDataSize, IfInfo);
                              }
                              
                              if ( (EFI_SUCCESS == Status) && 
                                      //check for Ipaddress is not NULL
                                      CompareMem (&IfInfo->StationAddress.Addr, &ZeroIP, sizeof (EFI_IPv4_ADDRESS)) )
                                {
                                  UINT8 iIndex = 0;
                                  ArpAddress.SenderHwAddr    = (UINT8 *)(ArpHead + 1);
                                  ArpAddress.SenderProtoAddr = ArpAddress.SenderHwAddr + ArpHead->HwAddrLen;
                                  ArpAddress.TargetHwAddr    = ArpAddress.SenderProtoAddr + ArpHead->ProtoAddrLen;
                                  ArpAddress.TargetProtoAddr = ArpAddress.TargetHwAddr + ArpHead->HwAddrLen;
                                  
                                  //_DEBUG ((EFI_D_NET, "\nStationAddress is:"));
                                  //for (iIndex = 0; iIndex < sizeof (EFI_IPv4_ADDRESS); iIndex ++)
                                  //{
                                  //    _DEBUG ((EFI_D_NET, " %x", IfInfo->StationAddress.Addr[iIndex]));
                                  //}
                                  if ( (!CompareMem (&IfInfo->StationAddress.Addr, ArpAddress.TargetProtoAddr, sizeof (EFI_IPv4_ADDRESS))) && ((Snp->Cdb.StatCode == 0)))
                                  {
                                      OverallStatus = EFI_SUCCESS;
                                      break;
                                  }
                              }
                              else
                              {
                                  OverallStatus = EFI_SUCCESS;
                                  break;
                              }
                          }
                      }
                  }
              }
          }
      }
    }
}
  
  //_DEBUG ((EFI_D_WARN, "OverallStatus is %x\n", OverallStatus));
  
  if (EFI_ERROR (OverallStatus) && (Snp->Cdb.StatCode == 0))
  {
	  goto RECEIVEAGAIN;
  }
#endif //SNP_SPEEDUP
// AMI PORTING ENDS
  return (*BufferSize <= BuffSize) ? EFI_SUCCESS : EFI_BUFFER_TOO_SMALL;
}

/**
  Receives a packet from a network interface.

  This function retrieves one packet from the receive queue of a network interface.
  If there are no packets on the receive queue, then EFI_NOT_READY will be
  returned. If there is a packet on the receive queue, and the size of the packet
  is smaller than BufferSize, then the contents of the packet will be placed in
  Buffer, and BufferSize will be updated with the actual size of the packet.
  In addition, if SrcAddr, DestAddr, and Protocol are not NULL, then these values
  will be extracted from the media header and returned. EFI_SUCCESS will be
  returned if a packet was successfully received.
  If BufferSize is smaller than the received packet, then the size of the receive
  packet will be placed in BufferSize and EFI_BUFFER_TOO_SMALL will be returned.
  If the driver has not been initialized, EFI_DEVICE_ERROR will be returned.

  @param This       A pointer to the EFI_SIMPLE_NETWORK_PROTOCOL instance.
  @param HeaderSize The size, in bytes, of the media header received on the network
                    interface. If this parameter is NULL, then the media header size
                    will not be returned.
  @param BufferSize On entry, the size, in bytes, of Buffer. On exit, the size, in
                    bytes, of the packet that was received on the network interface.
  @param Buffer     A pointer to the data buffer to receive both the media
                    header and the data.
  @param SrcAddr    The source HW MAC address. If this parameter is NULL, the HW
                    MAC source address will not be extracted from the media header.
  @param DestAddr   The destination HW MAC address. If this parameter is NULL,
                    the HW MAC destination address will not be extracted from
                    the media header.
  @param Protocol   The media header type. If this parameter is NULL, then the
                    protocol will not be extracted from the media header. See
                    RFC 1700 section "Ether Types" for examples.

  @retval EFI_SUCCESS           The received data was stored in Buffer, and
                                BufferSize has been updated to the number of
                                bytes received.
  @retval EFI_NOT_STARTED       The network interface has not been started.
  @retval EFI_NOT_READY         No packets have been received on the network interface.
  @retval EFI_BUFFER_TOO_SMALL  BufferSize is too small for the received packets.
                                BufferSize has been updated to the required size.
  @retval EFI_INVALID_PARAMETER One or more of the following conditions is TRUE:
                                * The This parameter is NULL
                                * The This parameter does not point to a valid
                                  EFI_SIMPLE_NETWORK_PROTOCOL structure.
                                * The BufferSize parameter is NULL
                                * The Buffer parameter is NULL
  @retval EFI_DEVICE_ERROR      The command could not be sent to the network interface.

**/
EFI_STATUS
EFIAPI
SnpUndi32Receive (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This,
  OUT UINTN                      *HeaderSize OPTIONAL,
  IN OUT UINTN                   *BufferSize,
  OUT VOID                       *Buffer,
  OUT EFI_MAC_ADDRESS            *SrcAddr OPTIONAL,
  OUT EFI_MAC_ADDRESS            *DestAddr OPTIONAL,
  OUT UINT16                     *Protocol OPTIONAL
  )
{
  SNP_DRIVER  *Snp;
  EFI_TPL     OldTpl;
  EFI_STATUS  Status;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Snp = EFI_SIMPLE_NETWORK_DEV_FROM_THIS (This);

  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  switch (Snp->Mode.State) {
  case EfiSimpleNetworkInitialized:
    break;

  case EfiSimpleNetworkStopped:
    Status = EFI_NOT_STARTED;
    goto ON_EXIT;

  default:
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  if ((BufferSize == NULL) || (Buffer == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  if (Snp->Mode.ReceiveFilterSetting == 0) {
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  Status = PxeReceive (
             Snp,
             Buffer,
             BufferSize,
             HeaderSize,
             SrcAddr,
             DestAddr,
             Protocol
             );

ON_EXIT:
  gBS->RestoreTPL (OldTpl);

  return Status;
}
