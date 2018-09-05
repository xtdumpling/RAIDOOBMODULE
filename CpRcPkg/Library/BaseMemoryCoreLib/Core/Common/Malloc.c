/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"

#define OCCUPIED  0x00000001
#define HEAP_END  0x80000000

//
// Heap buffer header structure
//
struct heapBufHeader {
  UINT8   *bufBase;
  UINT32  bufLimit;
  UINT32  bufFlags;
};

/**

  Creates a Heap for storage

  @param host      - Pointer to the system host (root) structure

  @retval N/A

**/
void
InitHeap (
         PSYSHOST host
         )
{
  UINT8                 *heapLimitPtr;
  struct heapBufHeader  *heapBase;
  UINT8                 *baseAddr;
  UINT32                heapSize;

  //
  // If heap is provided
  //
  if (host->var.common.heapSize) {
    baseAddr = (UINT8 *)host->var.common.heapBase;
    heapSize = host->var.common.heapSize;

    heapLimitPtr              = baseAddr + heapSize;

    //
    // Initialize the start header
    //
    heapBase            = (struct heapBufHeader *) baseAddr;
    heapBase->bufBase   = baseAddr + sizeof (struct heapBufHeader);
    heapBase->bufLimit  = heapSize - (2 * sizeof (struct heapBufHeader));
    heapBase->bufFlags  = 0;

    //
    // Initialize the end header
    //
    heapBase            = (struct heapBufHeader *) (heapLimitPtr - sizeof (struct heapBufHeader));
    heapBase->bufBase   = heapLimitPtr;
    heapBase->bufLimit  = 0;
    heapBase->bufFlags  = HEAP_END;
  }

}

/**

  Reserve memory in the heap

  @param host  - Pointer to the system host (root) structure
  @param size  - Size of memory to allocate

  @retval Pointer to allocated memory

**/
void *
rcMalloc (
         PSYSHOST host,
         UINT32   size
         )
{
  struct heapBufHeader  *headerPtr;
  struct heapBufHeader  *nextHeaderPtr;

  if (!host->var.common.heapSize) return NULL;

  //
  // Round size up to a QWORD integral
  //
  while (size % 8) {
    size++;

  }
  //
  // Initialize the root header
  //
  headerPtr = (struct heapBufHeader *) host->var.common.heapBase;

  //
  // Walk the heap looking for an available buffer
  //
  while (((headerPtr->bufFlags & OCCUPIED) || (headerPtr->bufLimit < size)) && !(headerPtr->bufFlags & HEAP_END)) {
    headerPtr = (struct heapBufHeader *) (headerPtr->bufBase + headerPtr->bufLimit);

  }
  //
  // Check for the end of heap space
  //
  if (headerPtr->bufFlags & HEAP_END) {
    return NULL;

  }
  //
  // Lock memory for the buffer
  //
  headerPtr->bufFlags |= OCCUPIED;

  //
  // Initialize the current size and next header if required
  //
  if ((headerPtr->bufLimit - size) > sizeof (struct heapBufHeader)) {
    nextHeaderPtr           = (struct heapBufHeader *) (headerPtr->bufBase + size);
    nextHeaderPtr->bufBase  = (UINT8 *) nextHeaderPtr + sizeof (struct heapBufHeader);
    nextHeaderPtr->bufLimit = headerPtr->bufLimit - size - sizeof (struct heapBufHeader);
    nextHeaderPtr->bufFlags = 0;
    headerPtr->bufLimit     = size;
  }
  //
  // Return the current base
  //
  return headerPtr->bufBase;
}

/**

  rcFree previously rcMalloced memory

  @param host      - Pointer to the system host (root) structure
  @param memblock  - Pointer to memory to deallocate

  @retval N/A

**/
void
rcFree (
       PSYSHOST host,
       void     *memblock
       )
{
  struct heapBufHeader  *headerPtr;
  struct heapBufHeader  *tempPtr;

  if (!host->var.common.heapSize) return;

  //
  // Initialize a pointer to the given buffer header
  //
  headerPtr = (struct heapBufHeader *) ((UINT8 *) memblock - sizeof (struct heapBufHeader));

  //
  // Validate the given pointer before proceeding
  //
  if (headerPtr->bufBase != memblock) {
    return ;

  }
  //
  // Free the given buffer
  //
  headerPtr->bufFlags &= ~OCCUPIED;

  //
  // Initialize the root header
  //
  headerPtr = (struct heapBufHeader *) host->var.common.heapBase;

  //
  // Walk the heap looking for holes to merge
  //
  do {
    //
    // Find the next hole
    //
    while (headerPtr->bufFlags & OCCUPIED) {
      headerPtr = (struct heapBufHeader *) (headerPtr->bufBase + headerPtr->bufLimit);

    }
    //
    // Check for the end of heap space
    //
    if (headerPtr->bufFlags & HEAP_END) {
      break;

    }
    //
    // Look for adjacent holes to merge
    //
    tempPtr = (struct heapBufHeader *) (headerPtr->bufBase + headerPtr->bufLimit);
    while (!(tempPtr->bufFlags & (OCCUPIED | HEAP_END))) {
      //
      // Add this buffer to the current limit and move to the next buffer
      //
      headerPtr->bufLimit += tempPtr->bufLimit + sizeof (struct heapBufHeader);
      tempPtr = (struct heapBufHeader *) (tempPtr->bufBase + tempPtr->bufLimit);
    }
    //
    // Move to the next buffer
    //
    headerPtr = (struct heapBufHeader *) (headerPtr->bufBase + headerPtr->bufLimit);

  } while (!(headerPtr->bufFlags & HEAP_END));
}
