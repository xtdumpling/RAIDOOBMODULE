//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Capsule format guid for server capsule image.

Copyright (c)  2011 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.
**/

#ifndef _SERVER_CAPSULE_GUID_H_
#define _SERVER_CAPSULE_GUID_H_

#define SERVER_CAPSULE_GUID \
  { 0xd400d1e4, 0xa314, 0x442b, { 0x89, 0xed, 0xa9, 0x2e, 0x4c, 0x81, 0x97, 0xcb } }

#define SMI_INPUT_UPDATE_SIGNED_CAP 0x27
#define SMI_INPUT_GET_SIGNED_CAP    0x28

#define SMI_CAP_FUNCTION            0xEF

#pragma pack(1)
typedef struct {
   UINT64	Address;
   UINT32	BufferOffset;	
   UINT32	Size;
   UINT32	Flags;
   UINT32	Reserved;
} CAPSULE_FRAGMENT;

typedef struct {
  UINTN			   CapsuleLocation;	// Top of the capsule that point to structure CAPSULE_FRAGMENT
  UINTN			   CapsuleSize;		// Size of the capsule
  EFI_STATUS	 Status;			// Returned status
} CAPSULE_INFO_PACKET;

typedef struct {
  UINTN           BlocksCompleted;	// # of blocks processed
  UINTN           TotalBlocks;			// Total # of blocks to be processed
  EFI_STATUS	    Status;			      // returned status
} UPDATE_STATUS_PACKET;
#pragma pack()

extern EFI_GUID gEfiServerCapsuleGuid;

#endif
