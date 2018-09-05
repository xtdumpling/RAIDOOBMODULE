/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file PipeInit.c

  Memory Initialization Module.

**/

#include "SysHost.h"
#include "SysFunc.h"
#include "CoreApi.h"
#include "Pipe.h"
#include "SysFuncChip.h"
#include "MemHostChip.h"
#include "RcRegs.h"
#include "MemFuncChip.h"
#include "ScratchPadReg.h"

#ifdef _MSC_VER
#pragma warning(disable : 4213)
#endif


static UINT32 ImcUncErrorMsr[ ] = {
  0x401, 0x405, 0x409, 0x40D, 0x41D, 0x421, 0x435, 0x439, 0x43d, 0x441, 0x445, 0x449, 0
};

//
// Function Declarations
//
/**

  Removes current processor from non-eviction mode

  @param *PipeDispatchCmd -      MMCFG based reg address of pipe dispatch command register
          ClearMmioRule    -       If TRUE, then NEM-related MMIO_RULE CSR needs to be cleared

  @retval N/A

**/
VOID
TearDownNem (
  UINT32     *PipeDispatchCmd
  )
{
#ifdef __GNUC__
  asm
  (
    // save pipe dispatch command mailbox reg address in EDI
    "mov  %0, %%edi\n\t"        // edi = ptr to pipe dispatch command reg

    // set CR0.CD bit to globally disable cache
    "mov  %%cr0, %%eax\n\t"
    "or   $(1 << 30), %%eax\n\t"
    "mov  %%eax, %%cr0\n\t"

    // invalid/flush cache
    "invd\n\t"

    // clear Global MTRR Enable bit
    "mov $0x02FF, %%ecx\n\t"
    "rdmsr\n\t"
    "and $(~0x0800), %%eax\n\t"
    "wrmsr\n\t"

    // clear No Eviction Mode Setup/Run bits
    "mov $0x02E0, %%ecx\n\t"
    "rdmsr\n\t"
    "btr $1, %%eax\n\t"
    "wrmsr\n\t"
    "btr $0, %%eax\n\t"
    "wrmsr\n\t"

    // Renable fast strings after NEM exit
    // FAST_STRING_ENABLE = 1
    "mov   $0x01A0, %%ecx\n\t" // MSR_IA32_MISC_ENABLES           0x01A0
    "rdmsr\n\t"
    "bts   $0, %%eax\n\t"
    "wrmsr\n\t"

    // Clear pipe dispatch command mailbox
    "movl  $0, (%%edx)\n\t" // PIPE_DISPATCH_NULL            0

    "loop1:\n\t"
    "cli\n\t"
    "hlt\n\t"
    "jmp loop1\n\t"
    :
    : "r" (PipeDispatchCmd)
    :
  );
#else
  _asm
  {
    ; save pipe dispatch command mailbox reg address in EDI
    mov edi, PipeDispatchCmd        ; edi = ptr to pipe dispatch command reg

    ; set CR0.CD bit to globally disable cache
    mov  eax, cr0
    or   eax, BIT30
    mov  cr0, eax

    ; invalid/flush cache
    invd

    ; clear Global MTRR Enable bit
    mov ecx, 02FFh
    rdmsr
    and eax, NOT (0x0800)
    wrmsr

    ; clear No Eviction Mode Setup/Run bits
    mov ecx, 02E0h
    rdmsr
    btr eax, 1
    wrmsr
    btr eax, 0
    wrmsr

    ;
    ; Renable fast strings after NEM exit
    ; FAST_STRING_ENABLE = 1
    ;
    mov   ecx, MSR_IA32_MISC_ENABLES
    rdmsr
    bts   eax, 0
    wrmsr

    ; Clear pipe dispatch command mailbox
    mov DWORD PTR[edi], PIPE_DISPATCH_NULL

    loop1 :
    cli
    hlt
    jmp loop1
  }
#endif
}

//
// Function Implementations
//
/**

  Slave side of Pipe Initialization

    @param UINT32    NodeId           NodeId/SocketId of the package BSP as a pipe slave. Only the last 8-bit is meaningful
    @param UINT32    CfgAddrBDF       Bus:Dev:Func portion of Cofig Address of local SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR

    @retval This function does not return to caller, but enters a "cli, hlt" loop at the end.

**/
VOID
PeiPipeSlaveInit (
    UINT32    NodeId,
    UINT32    CfgAddrBDF
  )
{
  PSYSHOST  host;
  UINT32    NumberOfDwords;
  UINT32    CurrentDwordCount;
  UINT32    *Buffer;
  UINT8     socket;
  UINT32    *dispatchAddr;
  UINT32    *dataAddr;
  UINT8     errorStatus;
  UINT32    *ImcUncErrorMsrPtr;
  UINT64_STRUCT msrReg;
  UINT32    StickPad[SCRATCH_PAD_NUM];
  UINT32    i;
  UINT32    BootMode;
  UINT32    currentDword;
  UINT32    Temp;

  /*
    Clear Dispatch register
    Wait for get host structure pointer dispatch command (DispatchReg != 0 && ComamndReg == synchHost)
    Get address for host structure (read dataReg) to "host"
    Clear Dispatch register
    Wait for  (DispatchReg != 0) then DataReg = # dwords to receive
    Set Dwordcounter = 0;
    Clear Dispatch register
     for "# of dewords"
           Wait for (DispatchReg != 0) then DataReg = Dword[DwordCounter]
           Clear Dispatch register

    Go to normal dispatcher (PipeDispatcher(host);)
*/

  socket = (UINT8) NodeId;

  dispatchAddr  = (UINT32 *) ( CfgAddrBDF | (SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_MMIO & 0x00000FFF));
  dataAddr      = (UINT32 *) ( CfgAddrBDF | (SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR_MMIO & 0x00000FFF));

  //
  // Clear Dispatch register
  //
  *(volatile UINT32 *) dispatchAddr = 0x00;

  //
  // Check if there were memory errors on the previous boot
  //
  ImcUncErrorMsrPtr = &ImcUncErrorMsr[ 0 ];
  errorStatus = 0;

  while ( *ImcUncErrorMsrPtr != 0 ) {
    msrReg = ReadMsrLocal (*ImcUncErrorMsrPtr);
    if ((msrReg.hi & BIT31) && (msrReg.hi & BIT29)) {
      errorStatus |= 1;
    }
    ImcUncErrorMsrPtr++;
  }

  //
  // Wait for get host structure pointer dispatch command (DispatchReg != 0)
  //
  while (*(volatile UINT32 *) dispatchAddr == 0x00) {};

  //
  // Send previous error status
  //
  *(volatile UINT32 *) dataAddr = errorStatus;

  //
  // Clear Dispatch register
  //
  *(volatile UINT32 *) dispatchAddr = 0x00;

  //
  // Wait for get host structure pointer dispatch command (DispatchReg != 0)
  //
  while (*(volatile UINT32 *) dispatchAddr == 0x00) {};

  //
  // Get address for host structure (read dataReg) to "host"
  //
  host = (PSYSHOST)(*(volatile UINT32 *) dataAddr);

  MemSetLocal ((VOID *) host, 0x00, sizeof (SYSHOST));

  //
  // Clear Dispatch register
  //
  *(volatile UINT32 *) dispatchAddr = 0x00;

  //
  // Wait for  (DispatchReg != 0) then DataReg = # dwords to receive
  //
  while (*(volatile UINT32 *) dispatchAddr == 0x00) {};
  NumberOfDwords = *(volatile UINT32 *) dataAddr;

  //
  // Clear Dispatch register
  //
  *(volatile UINT32 *) dispatchAddr = 0x00;

  //
  // Wait for next command from master
  //
  while (*(volatile UINT32 *) dispatchAddr == 0x00) {};

  //
  // Get multithreaded/S3 resume flag from master
  //
  BootMode = *(volatile UINT32 *) dataAddr;

  //
  // Save
  //
  if (BootMode == S3Resume) {
    for ( i = 0; i < SCRATCH_PAD_NUM; i++) {
      StickPad[i] = *(volatile UINT32 *)(CfgAddrBDF |  (BiosNonStickyScratchPadCsr[i] & 0x00000FFF));
    }
  }

    //
    // Clear Dispatch register
    //
    *(volatile UINT32 *) dispatchAddr = 0x00;

  Buffer = (UINT32 *) host;

  if (BootMode == S3Resume) {
    for (CurrentDwordCount = 0; CurrentDwordCount < NumberOfDwords; CurrentDwordCount += currentDword) {
    //
    // Wait for (DispatchReg != 0) then DataReg = Dword[DwordCounter]
    //
    while (*(volatile UINT32 *) dispatchAddr == 0x00) {};

      currentDword = ((NumberOfDwords - CurrentDwordCount) <  SCRATCH_PAD_NUM ) ? (NumberOfDwords - CurrentDwordCount) : SCRATCH_PAD_NUM;

      for (i = 0; i < currentDword; i++) {
        Buffer[CurrentDwordCount + i] =  *(volatile UINT32 *)(CfgAddrBDF |  (BiosNonStickyScratchPadCsr[i] & 0x00000FFF));
      }

      //
      // Clear Dispatch register
      //
      if((NumberOfDwords - CurrentDwordCount) >  SCRATCH_PAD_NUM )
      *(volatile UINT32 *) dispatchAddr = 0x00;
    }

    // Restore
    for (i = 0; i < SCRATCH_PAD_NUM; i++) {
      *(volatile UINT32 *)(CfgAddrBDF | (BiosNonStickyScratchPadCsr[i] & 0x00000FFF)) = StickPad[i];
    }

    *(volatile UINT32 *) dispatchAddr = 0x00;
    while (*(volatile UINT32 *) dispatchAddr == 0x00) {};
    *(volatile UINT32 *) dispatchAddr = 0x00;
  } else {
    //
    // for "# of dwords"
    //
    for (CurrentDwordCount = 0; CurrentDwordCount < NumberOfDwords; ) {
      //
      // Wait for (DispatchReg != 0) then DataReg = Dword[DwordCounter]
      //
      while (*(volatile UINT32 *) dispatchAddr == 0x00) {};
      Temp = *(volatile UINT32 *) dispatchAddr;
      CurrentDwordCount = (Temp >> 4) & 0x0FFFFFFF;
      if (CurrentDwordCount < NumberOfDwords) {
        Buffer[CurrentDwordCount] = *(volatile UINT32 *) dataAddr;
      }

      //
      // Clear Dispatch register
      //
      *(volatile UINT32 *) dispatchAddr = 0x00;
    }
  }

  //
  // load local socketId to host->var.common.socketId
  //
  host->var.common.socketId = socket;

  //
  // Init variables that track nested call levels for semaphores
  //
  host->var.common.printfDepth = 0;

  LoadCapId (host, socket);

  //
  // Wait for command from Pipe Master
  //
  PipeDispatcher (host);

  //
  // Ready to exit Pipe
  // This TearDownNem()  function does NOT return!
  //
  TearDownNem (dispatchAddr);

}



/**

  Master side of pipe initialization

  @param host - pointer to sysHost structure on stack

  @retval N/A

**/
VOID
InitializePipe (
  struct sysHost             *host
  )
{
  UINT8   socket;
  UINT32  NumberOfDwords;
  UINT32  CurrentDwordCount;
  UINT32  HostDataDword;
  UINT32  *Buffer;
  UINT32  *ImcUncErrorMsrPtr;
  UINT64_STRUCT msrReg;
  UINT32  bootMode;
  UINT8   rcWriteRegDumpSave;

  rcWriteRegDumpSave = host->var.common.rcWriteRegDump;
  host->var.common.rcWriteRegDump = 0;

  //
  // We'll send bootMode = S3 resume only if MRC is multithreaded
  //
  if ( (host->setup.mem.options & MULTI_THREAD_MRC_EN) &&
    ((host->var.common.resetRequired == 0)) &&
    (host->var.common.bootMode == S3Resume) ) {
    bootMode = S3Resume;
  } else {
    bootMode = NormalBoot;
  }


  NumberOfDwords = (sizeof (struct sysHost) / (sizeof (UINT32)) + 1);
  //
  // Control will come here only on SBSP
  // (non-SBSP's are looping on their local SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR waiting for a jmp address)
  //
  Buffer = (UINT32 *) host;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    host->var.common.rcWriteRegDump = 0;
    //
    // Continue if this socket is not present
    //
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) {
      continue;
    }

    if (socket == host->nvram.common.sbspSocketId) {
    //
    // Check if there were memory errors on the previous boot
    //
        ImcUncErrorMsrPtr = &ImcUncErrorMsr[0];

        while (*ImcUncErrorMsrPtr != 0) {
            msrReg = ReadMsrLocal(*ImcUncErrorMsrPtr);
            if ((msrReg.hi & BIT31) && (msrReg.hi & BIT29)) {
                host->var.mem.previousBootError |= 1;
            }
            ImcUncErrorMsrPtr++;
        }
        continue;
    }

    //
    // Starting Pipe here, so rcPrintf Synchronization needs to be enabled
    //
    host->var.common.printfSync = 0x01;

    /*
    At this point non-SBSPs (a.k.a. NBSPs) are held in ParkNbsp PROC in ProcessorStartupUncore.asm, looping on its local scracth pad CSR
    For each non-SBSP present in the system, send command/data via its local scratch pad CSRs in the following order :

            A new stack pointer (that protects future host location)
            The entry address of PeiPipeSlaveInit() function

#ifdef DECOMPRES_RC
               Address of decompression destination buffer
               Address of decompression source buffer
               Address of decompression function
#endif

            "Previous error" flag (forces cold boot path)
            "Multi-Threaded MRC" flag (enable/disable)
            if "Multi-Threaded MRC" == "Enabled"
            {
               Address of host structure
               Size of host structure in DWords
               host structure data, one dword at a time
            }
*/

    //
    // Send new stack pointer (that protects future host location)
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, (UINT32) host);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR, PIPE_DISPATCH_SYNCH_PSYSHOST);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);

    rcPrintf ((host, "Pass PIPE_DISPATCH_SYNCH_PSYSHOST\n"));

    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);
    //
    // Send &PeiPipeSlaveInit
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, (UINT32) &PeiPipeSlaveInit);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    rcPrintf ((host, "Pass PeiPipeSlaveInit\n"));
    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);


    //
    // Just go
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    //
    // Read previous error status flag from slave
    //
    if (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR)) {
      host->var.mem.previousBootError |= 1;
      rcPrintf ((host, "\nSocket %d indicates an uncorrectable error\n", socket));
    }

    //
    // Send host structure address (host)
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, (UINT32) host);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    rcPrintf ((host, "Pass pointer to host\n"));
    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    //
    // Send host structure size in DWORDs (rounded up) ((sizeof sysHost)/(sizeof DWORD) + 1) [# of DWORDS]
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, NumberOfDwords);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);
    rcPrintf ((host, "Copy host structure\n"));
    while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);

    rcPrintf ((host, "\nSynch of host structure to Socket %x starting...\n", socket));

    //
    // Send bootMode
    //
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, bootMode);
    WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, 0x1);

    //
    // If S3 resume and multithreaded MRC
    //
    if (bootMode == S3Resume) {

      MultiThreadS3ResumeScratchPadRestore(host, socket);
    //
    // Not S3 resume or not multithreaded MRC
    //
    } else {
        for (CurrentDwordCount = 0; CurrentDwordCount < NumberOfDwords; CurrentDwordCount++) {
          HostDataDword = Buffer[CurrentDwordCount];
          if (HostDataDword) {
            //
            // Send DWORD[n] value
            //
            while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);
            WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, HostDataDword);
            WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, ((CurrentDwordCount << 4) | 0x1));
          }
        }
        // To terminate the AP
        while (ReadCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != 0x00);
        WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR, 0);
        WriteCpuPciCfgEx (host, socket, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, ((NumberOfDwords << 4) | 0x1));
      }
  } // socket loop

  host->var.common.rcWriteRegDump = rcWriteRegDumpSave;
}

/**

  Master side of exiting Pipe on slave processors

  @param host  - Pointer to the system host (root) structure

  @retval N/A

**/
VOID
ExitPipe (
  struct sysHost             *host
  )
{
  UINT8 index;

  //
  //  Do nothing but return if a reset request is pending
  //  Note: Since a reset will be triggered soon anyway, we keep pipe slaves looping on pipe command
  //        to avoid potentail issues with INVD instuction in TearDownNem() on a cold boot path
  //        when there are 4 CPU packages present and KTI RC has not yet fully completed the
  //        routings among non-SBSP sockets.
  //
  if (host->var.common.resetRequired)  return;

  //
  // Dispatch the slaves to exit Pipe one at a time
  //
  for (index = 0; index < MAX_SOCKET; index++) {
    if (index == host->nvram.common.sbspSocketId) {
      continue;
    }
    //
    // Continue if this socket is not present
    //
    if ((host->var.common.socketPresentBitMap & (BIT0 << index)) == 0) {
      continue;
    }

    //
    // Send "Exit Pipe" command to the slave
    //
    WriteCpuPciCfgEx (host, index, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR, PIPE_DISPATCH_EXIT);
    //
    // wait for the slave to complete so we can make sure all non-BSP sockets are done
    //
    while (ReadCpuPciCfgEx (host, index, 0, SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR) != (UINT32) PIPE_DISPATCH_NULL);
  }

  //
  //  Ensure all APs are in the Wait for SIPI state (e.g. They are no longer needed!
  //  This includes all other logical processors in the same physical processor
  //  as the BSP and all logical processors in other physical processors.
  //  If any APs are awake, the BIOS must put them back into the Wait for
  //  SIPI state by issuing a broadcast INIT IPI to all excluding self.
  //
  if (host->var.common.numCpus > 1) {
#ifdef __GNUC__
    asm
    (
      "mov $0x0FEE00300, %%edi\n\t"
      "mov $0x0000C4500, %%eax\n\t"
      "mov %%eax, (%%edi)\n\t"
      "loopstart:\n\t"
      "mov (%%edi), %%eax\n\t"
      "bt $12, %%eax\n\t"    /*  Check if send is in progress */
      "jc loopstart\n\t"     /*  Loop until idle */
      :
      :
      : "eax", "edi"
    );
#else
    _asm
    {
      mov edi, 0FEE00300h
      mov eax, 0000C4500h
      mov[edi], eax
      loopstart :
      mov eax, [edi]
      bt eax, 12    /*  Check if send is in progress */
      jc loopstart  /*  Loop until idle */
    }
#endif
  }
}
