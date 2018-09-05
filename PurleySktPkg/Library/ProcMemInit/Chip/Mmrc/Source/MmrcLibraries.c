/**

Copyright (c) 2005-2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MMRCLibraries.c

  Modular MMRC  libraries used throughout the MMRC, these files are
  independent of the project.

**/

#include "MmrcLibraries.h"
#include "SysFunc.h"

#if defined JTAG
unsigned long isb32Read(unsigned char port, unsigned short offset);
unsigned long isb32Write(unsigned char port, unsigned short offset, unsigned long value);
unsigned long isb32SendCmd (unsigned char cmd, unsigned char port, unsigned long value);
#endif

#if (SIM || JTAG || defined(SIM_BUILD) || defined(HEADLESS_MRC)) && !defined (YAM_ENV)
extern UINT8 ThreadIdToSktId(void);
PSYSHOST MyMrcData[MAX_SOCKET];
#endif
//SKX_TODO: Add for 64 bit build
#if !defined (IA32) || !defined (__GNUC__)
PSYSHOST MyMrcData[MAX_SOCKET];
#endif

UINT8  ResultsString[3][3]= {"P\0", "-\0","*\0"};

/**

  Single entry point to MMRC. MRC calls this function
  and then MMRC acts as a black box, performing the requested
  stage of DDRIO init and returns when complete or when
  an error is encountered.

  @param MrcData:       Host structure for all data related to MMRC
  @param Action:        Phy init, training, post-training, etc
  @param Parameters:    A structure used to pass data between MRC and MMRC.

  @retval Success
  @retval Failure

**/
MMRC_STATUS
MmrcEntry (
  IN  OUT   PSYSHOST             MrcData
)
{
  MMRC_STATUS              Status;
  UINT8                   Channel;
  UINT8                   socket;
  TASK_DESCRIPTOR        *j;
  struct channelNvram     (*channelNvList)[MAX_CH];

  Status = MMRC_SUCCESS;
  socket = MrcData->var.mem.currentSocket;
  channelNvList = GetChannelNvList(MrcData, socket);
  //
  // Execute all steps for the given Stage.
  //

  for (j = InitTasks; j->Function != NULL; j++) {
    if ((MrcData->var.common.mmrcBootMode & j->BootMode) != 0) {
#ifdef  DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
      SetMemPhaseCP (MrcData, j->PostCode, 0);
#endif  // SERIAL_DBG_MSG
#endif  // DEBUG_PERFORMANCE_STATS
      OutputCheckpoint (MrcData, STS_DDRIO_INIT, j->PostCode, socket);
      if ((j->Channel == ChAll) || (j->Channel == ChAllP && !MrcData->var.mem.EnableParallelTraining)) {
        for (Channel = 0; Channel < MAX_CH; Channel++) {
          if ((*channelNvList)[Channel].enabled == 0) continue;
          if (j->CapsuleStartIndex == EXTERNAL) {
#if CAPSULESTRINGS
#ifdef SERIAL_DBG_MSG
            getPrintFControl(MrcData);
            MemDebugPrint((MrcData, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              " Task: %s External\n", CapsuleStrings[j->StringIndex]));
            releasePrintFControl(MrcData);
#endif // SERIAL_DBG_MSG
#endif //CAPSULESTRINGS
          }
          MrcData->var.mem.ExecuteThisRoutineInParallel = FALSE;
          Status = (*(j->Function)) (MrcData, j->CapsuleStartIndex, j->StringIndex, Channel);
          if (Status != MMRC_SUCCESS) {
            return Status;
          }
        }
      } else if (j->Channel == ChAllP) {
        if (j->CapsuleStartIndex == EXTERNAL) {
#if CAPSULESTRINGS
#ifdef SERIAL_DBG_MSG
          getPrintFControl(MrcData);
          MemDebugPrint((MrcData, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            " Task: %s External\n", CapsuleStrings[j->StringIndex]));
          releasePrintFControl(MrcData);
#endif // SERIAL_DBG_MSG
#endif //CAPSULESTRINGS
        }
        MrcData->var.mem.ExecuteThisRoutineInParallel = TRUE;
        Status = (*(j->Function)) (MrcData, j->CapsuleStartIndex, j->StringIndex, 0);
        if (Status != MMRC_SUCCESS) {
          return Status;
        }
      } else {
        if (j->CapsuleStartIndex == EXTERNAL) {
#if CAPSULESTRINGS
#ifdef SERIAL_DBG_MSG
          getPrintFControl(MrcData);
          MemDebugPrint((MrcData, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            " Task: %s External\n", CapsuleStrings[j->StringIndex]));
          releasePrintFControl(MrcData);
#endif // SERIAL_DBG_MSG
#endif //CAPSULESTRINGS
        }
        MrcData->var.mem.ExecuteThisRoutineInParallel = FALSE;
        Status = (*(j->Function)) (MrcData, j->CapsuleStartIndex, j->StringIndex, 0);
        if (Status != MMRC_SUCCESS) {
          return Status;
        }
      }
    }
  }
  return Status;
}

void * MmrcMemset (void *dst, char value, UINT32 cnt) {

  UINT8 *dst8 = (UINT8 *) dst;

  while (cnt--) {
    *dst8 = value;
    dst8++;
  }

  return dst;
}

/**

    get the input string length

    @param String - Pointer to the input string

    @retval i - string length

**/
UINT32
StringLength (
  char *String
)
{
  UINT32 i;
  i = 0;
  while (String[i++] != 0);
  return i;
}

/*
  This routine dumps all registers in the MMRC spreadsheet. On the cover sheet of the SS, the user
  can control the debug level output:

  DUMP_REGISTER_NAMES = 1 means all register names, ports, offsets, values, and default values will be printed.
  DUMP_REGISTER_FIELDS = 1 means all register field names, values, default values, and access types will be printed.
  DUMP_REGISTER_DESCRIPTIONS = 1 will display the description for each register field.

  If DUMP_REGISTER_NAMES is set to 0, the other enables are don't-cares as this is the global enable for this routine.
*/
MMRC_STATUS
  DumpAllRegisters (
)
{
#if DUMP_REGISTER_NAMES
  UINT32 RegisterNumber;
  UINT32 i;
  UINT8  Box;
  UINT8  Channel;
  UINT8  Instance;
  UINT8  Port;
  UINT8  Bus;
  UINT8  Device;
  UINT8  Function;
  UINT32 Junk;
  UINT32 BaseOffset;
  REGISTER_ACCESS Register;
  UINTX  Value;
  UINT8  TotalChannels;
  UINT8  TotalInstances;
#if DUMP_REGISTER_FIELDS
  UINT8  j;
  UINTX  UpperMask;
  UINTX  LowerMask;
  UINT8  MaxBit;

#if USE_64_BIT_VARIABLES
  MaxBit = 64;
#else
  MaxBit = 32;
#endif
#endif

  for (Box = 0; Box < MAX_BOXES; Box++) {
    //
    // If there are no registers for this box, skip the printing.
    //
    if (InstancePortMapGenerated[Box].StringIndex == 0xFFFFFFFF) {
      continue;
    }
    //
    // If this box only has 1 sideband access command, assume it's write-only so we can't
    // read it. This is for things like opcode 0x68 in the DUNIT where you cannot read.
    //
    if (InstancePortMapGenerated[Box].AccessMethod == eSB && InstancePortMapGenerated[Box].ReadOperation == InstancePortMapGenerated[Box].WriteOperation) {
      continue;
    }
    if (InstancePortMapGenerated[Box].Instances == 0) {
      TotalInstances = InstancePortMapGenerated[Box].TotalInstances;
      TotalChannels  = 1;
    } else {
      TotalInstances = InstancePortMapGenerated[Box].Instances;
      TotalChannels  = (InstancePortMapGenerated[Box].TotalInstances / InstancePortMapGenerated[Box].Instances);
    }
    for (Channel = 0; Channel < TotalChannels; Channel++) {
      for (Instance = 0; Instance < TotalInstances; Instance++) {
        if (GetRegisterAccessInfo (Box, Channel, Instance, &Junk, &Port, &BaseOffset, &Bus, &Device, &Function, ModeRead) == NoError) {
        //
        // Print the header for each instance.
        //
#if CAPSULESTRINGS
        MmrcDebugPrint ((SDBG_MAX, "%s%d\n", UnitStringsGenerated[Box], (Channel * InstancePortMapGenerated[Box].Instances) + Instance));
#endif
        MmrcDebugPrint ((SDBG_MAX, "Name"));
        for (i = 0; i < MAX_REGISTER_NAME_LENGTH + 1; i++) {
          MmrcDebugPrint ((SDBG_MAX, " "));
        }
#if DUMP_REGISTER_FIELDS
        MmrcDebugPrint ((SDBG_MAX, "Port     Offset   Bits               Value            Default  Access\n"));
        for (i = 0; i < MAX_REGISTER_NAME_LENGTH + 74; i++) {
          MmrcDebugPrint ((SDBG_MAX, "_"));
        }
#else
        MmrcDebugPrint ((SDBG_MAX, "Port     Offset   Bits               Value            Default\n"));
        for (i = 0; i < MAX_REGISTER_NAME_LENGTH + 66; i++) {
          MmrcDebugPrint ((SDBG_MAX, "_"));
        }
#endif
        MmrcDebugPrint ((SDBG_MAX, "\n"));
        //
        // Grab the first register string for this boxtype.
        //
        RegisterNumber = InstancePortMapGenerated[Box].StringIndex;
        //
        // Loop through all registers for this box....
        //
        while (Registers[RegisterNumber].RegInfo != NULL) {
          //
          // Display the register name, padding with spaces to align everything. Typically you could
          // use printf ("%-15s", string), but this doesn't work in the MiniBIOS, so pad manually here.
          //
          MmrcDebugPrint ((SDBG_MAX, "%s", Registers[RegisterNumber].RegInfo->RegisterName));
          for (i = StringLength (Registers[RegisterNumber].RegInfo->RegisterName); i <= MAX_REGISTER_NAME_LENGTH + 2; i++) {
            MmrcDebugPrint ((SDBG_MAX, " "));
          }
          //
          // Display port, offset, and value.
          //
          Register.Offset = Registers[RegisterNumber].RegInfo->Offset;
          Register.Mask   = Registers[RegisterNumber].RegInfo->Mask;
          Value = MemRegRead (Box, Channel, Instance, Register);
          if (InstancePortMapGenerated[Box].AccessMethod == ePCI) {
            MmrcDebugPrint ((SDBG_MAX, "%02X/%02X/%01X ", Bus, Device, Function));
            } else if (InstancePortMapGenerated[Box].AccessMethod == eBAR) {
              MmrcDebugPrint ((SDBG_MAX, "        "));
          } else {
            MmrcDebugPrint ((SDBG_MAX, "   0x%02X ", Port));
          }
#if USE_64_BIT_VARIABLES
            MmrcDebugPrint ((SDBG_MAX, "0x%08X [xx:xx] 0x%08X%08X 0x%08X%08X\n",
              Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
              (UINT32) (Value >> 32),
              (UINT32) Value,
              (UINT32) (Registers[RegisterNumber].RegInfo->DefaultValue >> 32),
              (UINT32) Registers[RegisterNumber].RegInfo->DefaultValue
              ));
#else
            MmrcDebugPrint ((SDBG_MAX, "0x%08X [xx:xx] 0x%08X%08X 0x%08X%08X\n",
              Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
              0,
              (UINT32) Value,
              0,
              (UINT32) Registers[RegisterNumber].RegInfo->DefaultValue
              ));
#endif
#if DUMP_REGISTER_FIELDS
          //
          // Display all register fields with proper start/end bits and values.
          //
          for (i = 0; i < Registers[RegisterNumber].RegInfo->NumFields; i++) {
            MmrcDebugPrint ((SDBG_MAX, "  %s", Registers[RegisterNumber].RegFields[i].FieldName));
            for (j = (UINT8) StringLength (Registers[RegisterNumber].RegFields[i].FieldName); j <= MAX_REGISTER_NAME_LENGTH; j++) {
              MmrcDebugPrint ((SDBG_MAX, " "));
            }
            //
            // Calculate the mask based on start and end bits.
            //
              if (Registers[RegisterNumber].RegFields[i].EndBit == (MaxBit - 1)) {
#if USE_64_BIT_VARIABLES
                UpperMask = 0xFFFFFFFFFFFFFFFF;
#else
                UpperMask = 0xFFFFFFFF;
#endif
            } else {
              UpperMask = (((UINTX)1 << (Registers[RegisterNumber].RegFields[i].EndBit + 1)) - 1);
            }
            LowerMask = (((UINTX)1 << Registers[RegisterNumber].RegFields[i].StartBit) - 1);
            Register.Mask = UpperMask - LowerMask;
            //
            // Display port, offset, start/end bits, value, and default value.
            //
            Register.Offset = Registers[RegisterNumber].RegInfo->Offset;
            Register.ShiftBit = Registers[RegisterNumber].RegFields[i].StartBit;
            Value = MemFieldRead (Box, Channel, Instance, Register);
            if (InstancePortMapGenerated[Box].AccessMethod == ePCI) {
              MmrcDebugPrint ((SDBG_MAX, "%02X/%02X/%01X ", Bus, Device, Function));
              } else if (InstancePortMapGenerated[Box].AccessMethod == eBAR) {
                MmrcDebugPrint ((SDBG_MAX, "        "));
            } else {
              MmrcDebugPrint ((SDBG_MAX, "   0x%02X ", Port));
            }
#if USE_64_BIT_VARIABLES
              MmrcDebugPrint ((SDBG_MAX, "0x%08X [%02d:%02d] 0x%08X%08X 0x%08X%08X",
                Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
                Registers[RegisterNumber].RegFields[i].EndBit,
                Registers[RegisterNumber].RegFields[i].StartBit,
                (UINT32) (Value >> 32),
                (UINT32) Value,
                (UINT32) (Registers[RegisterNumber].RegFields[i].DefaultFieldValue >> 32),
                (UINT32) Registers[RegisterNumber].RegFields[i].DefaultFieldValue
                ));
#else
              MmrcDebugPrint ((SDBG_MAX, "0x%08X [%02d:%02d] 0x%08X%08X 0x%08X%08X",
                Registers[RegisterNumber].RegInfo->Offset + BaseOffset,
                Registers[RegisterNumber].RegFields[i].EndBit,
                Registers[RegisterNumber].RegFields[i].StartBit,
                0,
                (UINT32) Value,
                0,
                (UINT32) Registers[RegisterNumber].RegFields[i].DefaultFieldValue
                ));
#endif
              //
              // Display the access type (RW, RO, etc)
              //
              MmrcDebugPrint ((SDBG_MAX, "  %s\n", Registers[RegisterNumber].RegFields[i].AccessType));
              //
              // Optionally display the register field description (this takes up a LOT of space for
              // the strings so only use this mode in an environment that isn't space limited such
              // as RTL simulation or JTAG mode when running from a host machine.
              //
#if DUMP_REGISTER_DESCRIPTIONS
              MmrcDebugPrint ((SDBG_MAX, "%s\n", Registers[RegisterNumber].RegFields[i].Description));
#endif
            }
#endif
            RegisterNumber++;
          }

          MmrcDebugPrint ((SDBG_MAX, "\n"));
        }
      }
    }
  }
#endif
  return MMRC_SUCCESS;
}

UINTX
ByteEnableToUintX (
  IN     UINT8 Be,
  IN     UINTX Value,
  IN     UINT8 *ValueLength
)
/**

    The length of the mask is based on the byte enables.
    Ex. If 2-bits in the BE are set, then the mask will be 2-bytes long.

    @param Be          - Byte Enable
    @param Value       - Current Value
    @param ValueLength - Length of the value

    @retval FinalValue - Mask Value

**/
{
  UINTX  FinalValue;
  UINT8  TestByte;
  UINT8  MaxBit;

#if USE_64_BIT_VARIABLES
  MaxBit = 64;
#else
  MaxBit = 32;
#endif
  *ValueLength = 0;
  FinalValue = 0;
  for (TestByte = 0; TestByte < MaxBit; TestByte += 8) {
    if ((Be & 1) == 1) {
      (*ValueLength)++;
      FinalValue += ((Value & 0xff) << TestByte);
      Value >>= 8;
    }
    Be >>= 1;
  }
  //
  // In case of no byte enables (i.e. wake command or something), set to 1 byte of data.
  //
  if (*ValueLength == 0)
  {
    *ValueLength = 1;
  }
  return FinalValue;
}

/**

  Decodes and executes the Capsule initialization.  The routine starts at a beginning index in the CapsuleInit compressed
  data structure and continues until a ASSIGN_DONE flag is detected.

  @param MrcData:           Host structure for all MRC global data.
  @param CapsuleStartIndex: Starting point within the CapsuleData structure where the Init should begin.
  @param StringIndex:       When provided, this is the index into the string table to show the register names.
  @param Channel:           Current Channel being examined.

  @retval Success
  @retval Failure

**/
MMRC_STATUS
MmrcExecuteTask (
  PSYSHOST          MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
)
{
  UINTX    PfctValue;                                 // Based on the current PFCT, holds the Value for the current assignment.
  UINT8    PfctStatus;                                // Flag to determine if any of the PFCT conditions were met on the current assign.
  UINT8    Action;                                    // Holds the Action of the current assign.
  UINT8    Counter;                                   // Counter for displaying results on the output when enabled.
  UINT8    *CurrentEncLoc;                            // Holds the location of the current assign. being decoded.
  UINT32    Delay;                                    // Holds the Delay for the assignment.  0 if not needed.
  PFCT_VARIATIONS   PFCTVariations[MaxPfct];          // Holds all permutations of PFCTs
  REGISTER_LOCATION_ATTRIBUTES *RegisterAttributes;   // Bit fields for the flags portion of the MEMORYINIT.
  REGISTER_LOCATION_ATTRIBUTES2 *RegisterAttributes2; // Bit fields for the 2nd byte of the flags portion of the MEMORYINIT.
  UINT8    ConditionalExecution;                      // Flag specifying if the assignment should take place.
  UINT8    ChannelToWrite;                            // Specific channel the assignment is directed to, if any.
  UINT8    InstanceToWrite;                           // Specific instance the assignment is directed to, if any.
  UINT16   CurrentPtr;                                // Pointer to the encoded data stream.
  UINT16   RegisterOffset;                            // Register offset provided in the assignment.
  UINT8    BoxType;                                   // Boxtype provided in the assignment.
  UINT8    ByteEnable;                                // Byte enable provided in the assignment.
  UINTX    Mask;                                      // Mask that may be provided in the assignment.
  UINT8    MaskValueLength;                           // Length of the MASK in bytes, when the mask is provided.
  UINT8    Pfct;                                      // PFCT flag provided in the assignment.
  UINT8    PfctDynamic;                               // Dynamic field in the dPFCT provided in the assignment.
  UINT8    NumberConditionalValues;                   // Number of PFCT Condition-Values provided in the assignment.
  UINT8    NumberConditionalBytes;                    // Number of bytes within the condition portion of the PFCT assignment.
  UINT8    ConditionalFlag;                           // When decoding PFCT values, when the condition is met, this value is 1.
  UINT8    TestVal;                                   // Used to determine if the condition is true in the PFCT assignment.
  UINT8    NumberInstances;                           // Number of instances for a specific BoxType.
  UINT32   MCFieldisPerChannel = 0;
  UINT8    Instance;                                  // Instance being specified when looping on total Instances.
  UINTX    readValue;                                 // When doing a poll, this is the value read back.
  UINT8    PfctValueSize;                             // The size in bytes of the value portion of the PFCT CVs.
  UINT32   BaseOffset;                                // Base offset for the instance of the boxtype.
  UINT8    Port;                                      // Port ID for the instance of the boxtype.
  UINT8    Bus;                                       // When doing PCI Access, PCI Bus.
  UINT8    Device;                                    // When doing PCI Access, PCI Device.
  UINT8    Func;                                      // When doing PCI Access, PCI Function.
  UINT8    i;
  REGISTER_ACCESS Register;                           // Field that specifies the location of the register including offset and bits.
  UINTX    NewValue;                                  // Used for temporary storage in DynamicAssignmentGenerated call.
  UINT8    DynamicIndex;                              // Used in DynamicAssignmentGenerated (the switch statement index).
  UINT16   InitialStringIndex;
  UINT8    Socket;                                    // The current CPU socket on which we're operating.
  UINT8    Strobe;                                    // Used to loop on strobes.
  UINT8    Rank;                                      // Used to loop on ranks.
  UINT8    ThisChannel;                               // Used for strobe/rank looping.
  UINT8    ThisInstance;                              // Used for strobe/rank looping.
  UINT8    MaxStrobe;                                 // Used to loop on strobes.
  UINT8    MaxRank;                                   // Used to loop on ranks.
  UINT8    MaxBit;                                    // Are we using 32 bit or 64 bit registers?
  UINT8    NumStrings;                                // Number of strings we need to output to the log.
  UINT8    Zero;                                      // Temporary location for resetting structures to 0.
  UINT8    k;                                         // Temporary counter.
  UINT8    MsgLevel;                                  // Temporary storage for backing up the message level.
  struct   channelNvram     (*channelNvList)[MAX_CH];

#if USE_64_BIT_VARIABLES
  MaxBit = 64;
#else
  MaxBit = 32;
#endif

  InitialStringIndex  = StringIndex;
  ThisInstance = 0;
  ThisChannel = 0;
  MsgLevel = 0;
  Socket = MrcData->var.mem.currentSocket;
  channelNvList = GetChannelNvList(MrcData, Socket);

  for (; Channel < MAX_CH; Channel++) {
    if ((*channelNvList)[Channel].enabled == 0) {
      continue;
    }
    //
    // Initializing the Variables.
    //
    ChannelToWrite                 = 0;
    PfctValue                      = 0;
    Counter                        = 0;
    RegisterOffset                 = 0;
    PfctValueSize                  = 0;
    Delay                          = 0;
    CurrentPtr                     = 0;
    ConditionalExecution           = 1;
    Bus                            = 0;
    Device                         = 0;
    Func                           = 0;
    BaseOffset                     = 0;
    Port                           = 0;
    ThisChannel                    = 0;
    ThisInstance                   = 0;
    StringIndex                    = InitialStringIndex;
#ifdef SERIAL_DBG_MSG
    MsgLevel                       = MrcData->var.mem.serialDebugMsgLvl;
    //
    // If this task is marked as NO_PRINT, turn off all MAX level messages. SDBG_MAX
    // is used by this routine to display all actions taken.
    //
    if (StringIndex == NO_PRINT) {
      MrcData->var.mem.serialDebugMsgLvl &= ~SDBG_MAX;
    }

#endif
#if CAPSULESTRINGS
    MmrcDebugPrint ((SDBG_MAX, " %s\n", CapsuleStrings[StringIndex]));
    MmrcDebugPrint ((SDBG_MAX, "    Box"));
    for (i = 0; i < MAX_BOXNAME_LENGTH; i++) {
      MmrcDebugPrint ((SDBG_MAX, " "));
    }
    MmrcDebugPrint ((SDBG_MAX, "Port Offset Mask               Action    Delay      Value\n"));
#endif
    //
    // Given the current P, F, C, T provide all possible PFCT variations, this makes the comparison quicker
    // since this does not have to be derived for every assignment.  There are a total of 16 different PFCT
    // permutations.
    //
    CreatePFCTSel (MrcData, Channel, PFCTVariations);
    //
    // Begin by assigning the Current encoded location to the start of the assignments.
    //
    CurrentEncLoc = (UINT8 *) &InitData[CapsuleStartIndex];
    //
    // The last assignment of the assignment list has the MACRO "ASSIGNDONE" which is the signature to stop decoding.
    //
    while (1) {
      InstanceToWrite =  0;
      ChannelToWrite  =  Channel;
      ConditionalExecution = 1;
      Zero = 0;
      //
      // Reset to 0.
      //
      RegisterAttributes2 = (REGISTER_LOCATION_ATTRIBUTES2 *) &Zero;
      //
      // The register attributes for the FLAGS starts at the beginning location, and the default length of this field
      // is one byte.
      //
      //MmrcDebugPrint ((SDBG_MAX, "0x%08x 0x%08x 0x%08x\n\r", CurrentEncLoc[0], CurrentEncLoc[1], CurrentEncLoc[2]));
      RegisterAttributes = (REGISTER_LOCATION_ATTRIBUTES *) &CurrentEncLoc[0];
      CurrentPtr = 1;
      if (RegisterAttributes->Done == 1) {
        break;
      }
      if (RegisterAttributes->NeedSecondByte == 1) {
        RegisterAttributes2 = (REGISTER_LOCATION_ATTRIBUTES2 *) &CurrentEncLoc[CurrentPtr];
        CurrentPtr++;
      }
      //
      // Check if the assignment should be executed based on the execution flag.
      //
      if (RegisterAttributes->CondExec == 1) {
        ConditionalExecution = (UINT8) MrcData->nvram.mem.socket[Socket].DynamicVars[Channel][CurrentEncLoc[CurrentPtr]];
        CurrentPtr++;
      }
      //
      // If the ChannelDependent or InstanceDependent flags are either set, then the flags will be one byte larger.
      // Also when not set, Channel/Instance to write are set to 0 but will not be used in the assignment.
      //
      if (RegisterAttributes2->ChannelDependent == 1) {
        ChannelToWrite  = CurrentEncLoc[CurrentPtr++];
      }
      if (RegisterAttributes2->InstanceDependent == 1) {
        InstanceToWrite = CurrentEncLoc[CurrentPtr++];
      }
      //
      // Following the flags is the register Offset.  This will be either a 8-bit or 16-bit value based on the
      // offset16 flag.
      //
      if (RegisterAttributes->Offset16 == 1) {
        RegisterOffset = *((UINT16 *) &CurrentEncLoc[CurrentPtr]);
        CurrentPtr +=2;
      } else {
        RegisterOffset = *((UINT8 *) &CurrentEncLoc[CurrentPtr]);
        CurrentPtr +=1;
      }
      //
      // Read either 2 bytes or 1 byte to get the boxtype and byte enables based on
      // the size of the boxtype number and the PFCT value to be written.
      //
      if (RegisterAttributes->UseBB2 == 1) {
        //
        // The Boxtype and ByteEnables are always provided in the next byte.  They are split into 4-bits each.
        //
        BoxType    = CurrentEncLoc[CurrentPtr];
        ByteEnable = CurrentEncLoc[CurrentPtr + 1];
        CurrentPtr += 2;
      } else {
        //
        // The Boxtype and ByteEnables are always provided in the next byte.  They are split into 4-bits each.
        //
        BoxType    = (CurrentEncLoc[CurrentPtr] >> 4) & 0x0f;
        ByteEnable = (CurrentEncLoc[CurrentPtr] >> 0) & 0x0f;
        CurrentPtr += 1;
      }
      //
      // If the MaskPresent bit in the flags is set, then the mask will be provided.  The length of the mask is based on the
      // byte enables.  Ex. If 2-bits in the BE (_0110) are set, then the mask will be 2-bytes long.
      //
      if (RegisterAttributes->MaskPresent == 1) {
        Mask = ByteEnableToUintX (ByteEnable, *((UINTX *) &CurrentEncLoc[CurrentPtr]), &MaskValueLength);
        CurrentPtr += MaskValueLength;
      } else {
#if USE_64_BIT_VARIABLES
        Mask = ByteEnableToUintX (ByteEnable, 0xffffffffffffffff, &MaskValueLength);
#else
        Mask = ByteEnableToUintX (ByteEnable, 0xffffffff, &MaskValueLength);
#endif
      }
      //
      // The Action/PFCT provides three distinct fields.  The action (get/set/poll), the PFCT, and the dynamic flag.
      // This is a 1-byte field and is split into [3-bits action, 1-bit dyn, 4-bits pfct].
      Action      = (CurrentEncLoc[CurrentPtr] >> 5) & 0x07;
      PfctDynamic = (CurrentEncLoc[CurrentPtr] >> 4) & 0x01;
      Pfct        = (CurrentEncLoc[CurrentPtr] >> 0) & 0x0f;
      CurrentPtr  += 1;
      //
      // If the action is SET_DELAY, then immediately following the ACTION will be a 4byte delay value.
      //
      if (Action == MMRC_SET_DELAY || Action == MMRC_POLL_DELAY || Action == MMRC_RDMW) {
        Delay = *(UINT32 *) &CurrentEncLoc[CurrentPtr];
        CurrentPtr += 4;
      }

      if (Action != MMRC_DELAY) {
        MCFieldisPerChannel = CurrentEncLoc[CurrentPtr];
        CurrentPtr += 4;
      }
      if (MCFieldisPerChannel) {
        BoxType = MCMAINCH;
      }
      //
      // The PFCT status will provide the flag if any of the PFCT conditions where true, the assumption is no
      // condition is true, and will be changed to TAKE_ACTION if one is determined.
      //
      PfctStatus = SKIP_ACTION;
      //
      // If the PFCT flag is set to 0, then the assignment is global/PFCT independent.  The assignment should then
      // provide one value.  If the dynamic bit is set, the value is an 8-bit index value, otherwise the value length
      // will be based on the byte-enables.
      //
      if (Pfct == PFCT_INDEPENDENT) {
        PfctStatus = TAKE_ACTION;
        if (PfctDynamic == 1) {
          PfctValue = CurrentEncLoc[CurrentPtr];
          CurrentPtr += 1;
        } else {
          PfctValue = ByteEnableToUintX (ByteEnable, *((UINTX *) &CurrentEncLoc[CurrentPtr]), &MaskValueLength);
          CurrentPtr += MaskValueLength;
        }
      } else {
        //
        // To reach this point, the PFCT Flag must have been non-zero so non-global.
        // The next byte is the total number of conditional-Value pairs..
        //
        NumberConditionalValues = CurrentEncLoc[CurrentPtr];
        CurrentPtr += 1;
        //
        // Loop through each Condition/Value pair looking for a match based on the current pfct.
        //
        for (Counter = 0; Counter < NumberConditionalValues; Counter++) {
          //
          // Determine the number of bytes are needed for the condition.  The system will
          // compare the PFCT condition byte-by-byte with the TRUE PFCT condition.  If all the
          // bits compare okay, then the condition is true.  ConditionalFlag is the byte-by-byte
          // comparison flag.
          //
          NumberConditionalBytes = PFCTVariations[Pfct].LengthVariation;
          ConditionalFlag = 1;
          while (NumberConditionalBytes > 0 && ConditionalFlag == 1) {
            TestVal = (UINT8) ((PFCTVariations[Pfct].Pfct >> ((NumberConditionalBytes - 1) * 8) ) & 0xff);
            if ((CurrentEncLoc[CurrentPtr+NumberConditionalBytes - 1]  & TestVal) != TestVal) {
               ConditionalFlag = 0;
            }
            NumberConditionalBytes--;
          }
          CurrentPtr += PFCTVariations[Pfct].LengthVariation;
          //
          // Read in the value which is also based on the byte-enables for its length, similar to the mask.
          //
          if (PfctDynamic == 1) {
            PfctValue = CurrentEncLoc[CurrentPtr];
            PfctValueSize = 1;
          } else {
            PfctValue = ByteEnableToUintX (ByteEnable, *((UINTX *) &CurrentEncLoc[CurrentPtr]), &MaskValueLength);
            PfctValueSize = MaskValueLength;
          }
          CurrentPtr += PfctValueSize;
          //
          // If the condition flag is still true, then move the currentPtr to the end of the remaining condition-values.
          // and set the status to take_action.
          if (ConditionalFlag == 1) {
            PfctStatus = TAKE_ACTION;
            CurrentPtr += (PFCTVariations[Pfct].LengthVariation + PfctValueSize) * (NumberConditionalValues - Counter - 1);
            break;
          }
        }  // for (Counter = 0; Counter < NumberConditionalValues; Counter++)
      } // if (Pfct == PFCT_INDEPENDENT) {
      //
      // Save the value which will be used as the index to the switch statement
      // in DynamicAssignmentGenerated.
      //
      DynamicIndex = (UINT8) PfctValue;
      if (RegisterAttributes2->StringPresent == 1) {
        NumStrings = CurrentEncLoc[CurrentPtr++];
      } else {
        NumStrings = 0;
      }
      //
      // Now that a value has been determined true, the value must be programmed based on the loopings to multiple
      // registers.
      //
      if (PfctStatus == TAKE_ACTION  && ConditionalExecution == 1) {
        if (RegisterAttributes->StrobeLoop == 1) {
          MaxStrobe = MAX_STROBES;
        } else {
          MaxStrobe = 1;
        }
        if (RegisterAttributes->RankLoop == 1) {
          MaxRank = MAX_RANKS;
        } else {
          MaxRank = 1;
        }
        //
        // If the register should be written only for a specific channel, and this call is
        // not the correct channel, just skip this assignment and go to the next assignment.
        //
        if (RegisterAttributes2->ChannelDependent == 0 || (RegisterAttributes2->ChannelDependent == 1 && Channel == ChannelToWrite)) {
          //
          // If the InstanceDependent flag is set, then the total number of instances that will be written is 1,
          // otherwise, the total number of instances/channel for the given boxtype will be written. The other case
          // is if we are looping on rank and/or strobe, then we will ignore the instance looping since strobe/rank
          // looping is a higher priority.
          //
          if (RegisterAttributes2->InstanceDependent == 0 && Action != MMRC_DELAY &&
            RegisterAttributes->StrobeLoop == 0 && RegisterAttributes->RankLoop == 0) {
              NumberInstances = InstancePortMapGenerated[BoxType].Instances;
              if (NumberInstances == 0) {
                //
                // If this boxtype is not channel based, then use the total number of instances
                // instead of the instances per channel.
                //
                NumberInstances = InstancePortMapGenerated[BoxType].TotalInstances;
              }
          } else {
            NumberInstances = 1;
          }
          //
          // Loop though all the instances determined earlier.
          //
          for (Instance = 0; Instance < NumberInstances; Instance++) {
            for (Rank = 0; Rank < MaxRank; Rank++) {
              for (Strobe = 0; Strobe < MaxStrobe; Strobe++) {
                //
                // If the register should be written only for a specific channel, and this call is
                // not the correct channel, just skip this assignment and go to the next assignment.
                //
                if (RegisterAttributes2->InstanceDependent == 1) {
                  Instance = InstanceToWrite;
                }
                //
                // Determine the actual offset/mask for the register to be accessed.  The offset currently is based off the
                // offset for the assignment + the rank offset + bytelane offset. The mask and shift bit will also be used
                // for PCI accesses although the data for the PCI access is in PciReg which is filled in below.
                //
                Register.Offset   = RegisterOffset;
                Register.Mask     = Mask;
                Register.ShiftBit = 0;
                //
                // If we're looping on strobe/rank we need to adjust the offset to the correct register location for this strobe/rank.
                //
                if (RegisterAttributes->StrobeLoop == 0 && RegisterAttributes->RankLoop == 0) {
                  ThisChannel = Channel;
                  ThisInstance = Instance;
                } else {
                  if (RegisterAttributes->StrobeLoop == 1) {
                    Register.Offset += StrobeToRegisterGenerated[FloorPlanGenerated[Channel][Strobe].Strobelane][1];
                    ThisChannel = FloorPlanGenerated[Channel][Strobe].Channel;
                    ThisInstance = (UINT8)(Instance + StrobeToRegisterGenerated[FloorPlanGenerated[Channel][Strobe].Strobelane][0]);
                  }
                  if (RegisterAttributes->RankLoop == 1) {
                    Register.Offset += RankToRegisterGenerated[Rank][1];
                    ThisInstance += ((UINT8)(Instance + RankToRegisterGenerated[Rank][0]));
                  }
                }
                //
                // If the action was a GET/SET/SET_DELAY/POLL, then the register offset, mask, delay, and value will be printed.
                //
                if (Action != MMRC_DELAY) {
                  if (GetRegisterAccessInfo (BoxType, ThisChannel, ThisInstance,
                      (UINT32 *)&readValue, &Port, &BaseOffset, &Bus, &Device, &Func, ModeRead) != NoError) {
                    continue;
                  }
                }
                //
                // The printout of the assignment will begin with either a *,/, or +.
                //  *-> the assignment was performed, but it completed the assignment of others as it had broadcast enabled.
                //  /-> the assignment was performed.
                //  +-> the assignment was skipeed as it was assigned with the broadcast.
                //
                if (Instance == 0 && RegisterAttributes2->InstanceDependent == 0 && BROADCAST_SUP) {
                  MmrcDebugPrint ((SDBG_MAX, "   *"));
                } else {
                  MmrcDebugPrint ((SDBG_MAX, "   +"));
                }
                //
                // Print out register information.
                //
                if (Action == MMRC_GET || Action == MMRC_SET || Action == MMRC_SET_DELAY || Action == MMRC_POLL || Action == MMRC_POLL_DELAY || Action == MMRC_RDMW) {
                  //
                  // Convert the PFCTValue to the dynamic value, if the flag is set.
                  //
                  if (PfctDynamic == 1 && Action != MMRC_GET) {
#if USE_64_BIT_VARIABLES
                    Register.Mask     = 0xFFFFFFFFFFFFFFFF;
#else
                    Register.Mask     = 0xFFFFFFFF;
#endif
                    Register.ShiftBit = 0;
                    NewValue = MemRegRead (BoxType, ThisChannel, ThisInstance, Register);
                    DynamicAssignmentGenerated (MrcData->nvram.mem.socket[Socket].DynamicVars, ThisChannel, DynamicIndex, &NewValue);
                    PfctValue = NewValue;
                  }

                  if (InstancePortMapGenerated[BoxType].AccessMethod == eSB) {
                    MmrcDebugPrint((SDBG_MAX, "   0x%02X", Port));
                  } else if (InstancePortMapGenerated[BoxType].AccessMethod == eBAR) {
                    MmrcDebugPrint((SDBG_MAX, "    MEM"));
                  } else if (InstancePortMapGenerated[BoxType].AccessMethod == ePCI) {
                    MmrcDebugPrint((SDBG_MAX, "%02X/%02X/%01X", Bus, Device, Func));
                  }
#if USE_64_BIT_VARIABLES
                  MmrcDebugPrint((SDBG_MAX, " 0x%04X 0x%08X%08X ", Register.Offset + BaseOffset, (UINT32)(Register.Mask >> 32), (UINT32)Register.Mask));
#else
                  MmrcDebugPrint((SDBG_MAX, " 0x%04X 0x%08X%08X ", Register.Offset + BaseOffset, 0, (UINT32)Register.Mask));
#endif
                }
                if (Action == MMRC_GET) {
                  //
                  // Get the right shift bit to capture the variable being read.
                  //
                  for (i = 0; i < MaxBit; i++) {
                    if ((Register.Mask & (UINTX)(BIT0 << i)) != 0)
                    {
                      break;
                    }
                    Register.ShiftBit++;
                  }
                  MrcData->nvram.mem.socket[Socket].DynamicVars[ChannelToWrite][PfctValue] = MemFieldRead (BoxType, ThisChannel, ThisInstance, Register);
#if USE_64_BIT_VARIABLES
                  MmrcDebugPrint ((SDBG_MAX, "GET       0x%08X 0x%08X%08X", 0,
                    (UINT32)(MrcData->nvram.mem.socket[Socket].DynamicVars[ChannelToWrite][PfctValue] >> 32),
                    (UINT32)(MrcData->nvram.mem.socket[Socket].DynamicVars[ChannelToWrite][PfctValue])
                  ));
#else
                  MmrcDebugPrint ((SDBG_MAX, "GET       0x%08X 0x%08X%08X", 0,
                    0,
                    (UINT32)(MrcData->nvram.mem.socket[Socket].DynamicVars[ChannelToWrite][PfctValue])
                    ));
#endif
                } else if (Action == MMRC_SET || Action == MMRC_SET_DELAY || Action == MMRC_RDMW)  {
                  if (Action == MMRC_SET) {
                    MmrcDebugPrint ((SDBG_MAX, "SET       0x%08X ", 0));
                  }
                  if (Action == MMRC_SET_DELAY) {
                    //MmrcDebugPrint ((SDBG_MAX, "SET_DELAY 0x%08X ", Delay));
                  }
                  if (Action == MMRC_RDMW) {
                    //MmrcDebugPrint ((SDBG_MAX, "Read Delay Modify Write 0x%08X ", Delay));
                  }
#if USE_64_BIT_VARIABLES
                  MmrcDebugPrint((SDBG_MAX, "0x%08X%08X", (UINT32)(PfctValue >> 32), (UINT32)(PfctValue)));
#else
                  MmrcDebugPrint((SDBG_MAX, "0x%08X%08X", 0, (UINT32)(PfctValue)));
#endif

                  MemRegWrite (BoxType, ThisChannel, ThisInstance, Register, PfctValue, ByteEnable);
                  //
                  // If the action has a delay, with the set, then delay between every assignment.
                  //
                  if (Action == MMRC_SET_DELAY) {
#if SIM
                      MySimStall (Delay);
#else
                    //
                    // Server MRC has micro-second granularity on their delays so if the delay
                    // is less than 1us, round up to 1us.
                    //
                    //if (Delay < 1000) {
                    //  Delay = 1000;
                    //}
                    //FixedDelay (MrcData, Delay / 1000);
#endif
                  }
                } else if (Action == MMRC_POLL || Action == MMRC_POLL_DELAY) {
                  //
                  // If the action is a POLL, then print out the register/value to be polled for, and begin polling.
                  //
#if USE_64_BIT_VARIABLES
                  MmrcDebugPrint((SDBG_MAX, "POLL      0x00000000 0x%08X%08X", (UINT32)(PfctValue >> 32), (UINT32)(PfctValue)));
#else
                  MmrcDebugPrint((SDBG_MAX, "POLL      0x00000000 0x%08X%08X", 0, (UINT32)(PfctValue)));
#endif
                  PfctValue ^= Mask;
                  do {
                      readValue = MemRegRead (BoxType, ThisChannel, ThisInstance, Register);
#if SIM
                      //
                      // Stall between polling passes in simulation so we don't slow down the
                      // simulator with a bunch of read requests.
                      //
                      MySimStall (50);
#else
                 if (Action == MMRC_POLL_DELAY) {
                 }
#endif
                  readValue &= Mask;
                  } while (readValue == PfctValue);
                } else if(Action == MMRC_DELAY) {
                  //MmrcDebugPrint ((SDBG_REG_ACCESS, "DELAY: %d Q clocks \n", PfctValue));
#if SIM
                  MySimStall ((UINT32)PfctValue);
#else
                  //
                  // Server MRC has micro-second granularity on their delays so if the delay
                  // is less than 1us, round up to 1us.
                  //
                    //if (Delay < 1000) {
                    //  Delay = 1000;
                    //}
                    //FixedDelay (MrcData, Delay / 1000);
#endif
                }
                //
                // Add the length of the current assignment to the current encoded location to the the location of the next assignment.
                //
                if (RegisterAttributes2->StringPresent == 1) {
                  for (k = 0; k < NumStrings; k++) {
#if CAPSULESTRINGS
                    MmrcDebugPrint ((SDBG_MAX, " %s", OutputStrings[CurrentEncLoc[CurrentPtr + k]]));
                    if ((k + 1) < NumStrings) {
                      MmrcDebugPrint ((SDBG_MAX, ","));
                    }
#endif // CAPSULESTRINGS
                  }
                }
                MmrcDebugPrint ((SDBG_MAX, "\n"));
              } // Strobe loop
            } // Rank loop
          } // Instance loop ...
        }
      } // TAKE_ACTION
      CurrentEncLoc += (CurrentPtr + NumStrings);
    } // *CurrentEncLoc != ASSIGNDONE loop ...

    if (!MrcData->var.mem.ExecuteThisRoutineInParallel) {
      //
      // Stop the routine after first execution
      // if not supposed to be executed in parallel
      //
      break;
    }
  } // Channel loop ...
#ifdef SERIAL_DBG_MSG
  MrcData->var.mem.serialDebugMsgLvl = MsgLevel;
#endif
  return MMRC_SUCCESS;
}

/**

    Given the current P, F, C, T provide all possible PFCT variations

    @param MrcData    - Data
    @param Channel    - Ch
    @param PFCTSelect - PFCT selection type

    @retval SUCCESS

**/
MMRC_STATUS
CreatePFCTSel (
               IN  OUT   PSYSHOST         MrcData,
               IN        UINT8            Channel,
               IN        PFCT_VARIATIONS *PFCTSelect
               )
{
  UINT16  CurrentPlatform;
  UINT16  CurrentFrequency;
  UINT16  CurrentConfiguration;
  UINT16  CurrentType;
  UINT8  socket;
  struct channelNvram     (*channelNvList)[MAX_CH];

  socket = MrcData->var.mem.currentSocket;
  channelNvList = GetChannelNvList(MrcData, socket);

  CurrentPlatform       = (*channelNvList)[Channel].CurrentPlatform;
  CurrentFrequency      = (*channelNvList)[Channel].CurrentFrequency;
  CurrentConfiguration  = (*channelNvList)[Channel].CurrentConfiguration;
  CurrentType           = (*channelNvList)[Channel].CurrentDdrType;

  PFCTSelect[Pfct].Pfct     = 0;
  PFCTSelect[PfctT].Pfct    = (1  << CurrentType);
  PFCTSelect[PfctC].Pfct    = (1  << CurrentConfiguration);
  PFCTSelect[PfctCT].Pfct   = ((1 << CurrentConfiguration) << (NUM_TYPE)) | (1 << CurrentType);
  PFCTSelect[PfctF].Pfct    = (1  << CurrentFrequency);
  PFCTSelect[PfctFT].Pfct   = ((1 << CurrentFrequency) << (NUM_TYPE)) | (1 << CurrentType);
  PFCTSelect[PfctFC].Pfct   = ((1 << CurrentFrequency) << (NUM_CONF)) | (1 << CurrentConfiguration);
  PFCTSelect[PfctFCT].Pfct  = ((1 << CurrentFrequency) << (NUM_CONF + NUM_TYPE)) | ((1 << CurrentConfiguration) << (NUM_TYPE)) | ((1 << CurrentType));
  PFCTSelect[PfctP].Pfct    = (1  << CurrentPlatform);
  PFCTSelect[PfctPT].Pfct   = ((1 << CurrentPlatform) << (NUM_TYPE) ) | (1 << CurrentType);
  PFCTSelect[PfctPC].Pfct   = ((1 << CurrentPlatform) << (NUM_CONF) ) | (1 << CurrentConfiguration);
  PFCTSelect[PfctPCT].Pfct  = ((1 << CurrentPlatform) << (NUM_CONF + NUM_TYPE) ) | ( (1 << CurrentConfiguration) << (NUM_TYPE) ) | (1 << CurrentType);
  PFCTSelect[PfctPF].Pfct   = ((1 << CurrentPlatform) << (NUM_FREQ) ) | (1 << CurrentFrequency);
  PFCTSelect[PfctPFT].Pfct  = ((1 << CurrentPlatform) << (NUM_FREQ + NUM_TYPE) ) | ( (1 << CurrentFrequency) << (NUM_TYPE) ) | (1 << CurrentType);
  PFCTSelect[PfctPFC].Pfct  = ((1 << CurrentPlatform) << (NUM_FREQ + NUM_CONF) ) | ( (1 << CurrentFrequency) << (NUM_CONF) ) | (1 << CurrentConfiguration);
  PFCTSelect[PfctPFCT].Pfct = ((1 << CurrentPlatform) << (NUM_FREQ + NUM_CONF + NUM_TYPE) ) | ((1 << CurrentFrequency) << (NUM_CONF + NUM_TYPE)) | ((1 << CurrentConfiguration) << (NUM_TYPE)) | (1 << CurrentType);

  PFCTSelect[Pfct].LengthVariation     = 0;
  PFCTSelect[PfctT].LengthVariation    = (NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctC].LengthVariation    = (NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctCT].LengthVariation   = (NUM_CONF + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctF].LengthVariation    = (NUM_FREQ - 1) / 8 + 1;
  PFCTSelect[PfctFT].LengthVariation   = (NUM_FREQ + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctFC].LengthVariation   = (NUM_FREQ + NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctFCT].LengthVariation  = (NUM_FREQ + NUM_CONF + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctP].LengthVariation    = (NUM_PLAT - 1) / 8 + 1;
  PFCTSelect[PfctPT].LengthVariation   = (NUM_PLAT + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctPC].LengthVariation   = (NUM_PLAT + NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctPCT].LengthVariation  = (NUM_PLAT + NUM_CONF + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctPF].LengthVariation   = (NUM_PLAT + NUM_FREQ - 1) / 8 + 1;
  PFCTSelect[PfctPFT].LengthVariation  = (NUM_PLAT + NUM_FREQ + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctPFC].LengthVariation  = (NUM_PLAT + NUM_FREQ + NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctPFCT].LengthVariation = (NUM_PLAT + NUM_FREQ + NUM_CONF + NUM_TYPE - 1) / 8 + 1;

  return MMRC_SUCCESS;
}


/**

  Saves the MRC host structure to an MMX register for future use.

  @param MrcDataAddress - Address of MRC Host Structure

  @retval None

**/
VOID
SaveMrcHostStructureAddress (
  IN    PSYSHOST   MrcDataAddress
)
{
#if SIM || JTAG || defined SIM_BUILD
  UINT8 socket;
  socket = MrcDataAddress->var.mem.currentSocket;
  MyMrcData[socket] = MrcDataAddress;
#else
#if __GNUC__
  asm (
    "movd %0,%%mm3;"
    :/* no output */
    : "c" (MrcDataAddress)
    :/* no clobber */
    );
#elif defined (IA32)              //SKX_TODO: Add for 64 bit build
  _asm {
    movd mm3, MrcDataAddress;
  }
#else                     //SKX_TODO: Add for 64 bit build
  UINT8 socket;
  socket = MrcDataAddress->var.mem.currentSocket;
  MyMrcData[socket] = MrcDataAddress;
#endif
#endif
}

/**

  Returns the MRC host structure from an MMX register.

  @param None

  @retval Address of host structure.

**/
PSYSHOST
GetMrcHostStructureAddress (
)
{
#if (SIM || JTAG || defined(SIM_BUILD) || defined(HEADLESS_MRC)) && !defined (YAM_ENV)
  return MyMrcData[ThreadIdToSktId()];
#else
  PSYSHOST ThisMrcData;
#if __GNUC__
  asm (
    "movd %%mm3,%0;"
    : "=c" (ThisMrcData)
    :/* no input */
    :/* no clobber */
    );
#elif defined (IA32)              //SKX_TODO: Add for 64 bit build
  _asm {
    movd ThisMrcData, mm3;
  }
#else                     //SKX_TODO: Add for 64 bit build
  return MyMrcData[0];
#endif
  return ThisMrcData;
#endif
}

#if defined (IA32) || defined (__GNUC__)    //SKX_TODO: Add for 64 bit build
void __cdecl _allshl (void)
/**

  Performs a shift left on a 64 bit number. From the Intel Xeon Manuals:
  In non-64-bit modes and default 64-bit mode only bits 0 through 4 of
  the count are used. This masks the count to a value between 0 and 31.
  If a count is greater than the operand size, the result is undefined.
  For this reason, we must do a SHL for the upper 32 bits and another for
  the lower 32 bits.

  @param CL      - Shift amount (in bits)
  @param EDX:EAX - 64 bit number to shift.


  @retval EDX:EAX - 64 bit result of shift left.

**/
{
#if __GNUC__
  asm (
    "cmp  $32, %%cl;"
    "jae  ShiftLowDword;"
    "shld %%cl, %%edx, %%eax;"
    "shl  %%cl, %%eax;"
    "ret;"
"ShiftLowDword:;"
    "mov  %%eax, %%edx;"
    "xor  %%eax, %%eax;"
    "sub  $32, %%cl;"
    "shl  %%cl, %%edx;"
    "ret;"
    : /* no output */
    : /* no input */
    : "%eax", "%edx"
  );
#else
_asm {
  cmp  cl, 32
  jae  ShiftLowDword    ; If shifting more than 31 bits, ignore the high DWORD since it be shifted out.
  shld edx, eax, cl
  shl  eax, cl
  ret
ShiftLowDword:
  mov  edx, eax
  xor  eax, eax
  sub  cl,  32          ; We already shifted the value by 32 bits by moving EAX into EDX. So just shift what is left over above 32 bits.
  shl  edx, cl
  ret
}
#endif
}

void __cdecl _aullshr (void)
/**

  Performs a shift right on a 64 bit number.
  In non-64-bit modes and default 64-bit mode only bits 0 through 4 of
  the count are used. This masks the count to a value between 0 and 31.
  If a count is greater than the operand size, the result is undefined.
  For this reason, we must do a SHR for the upper 32 bits and another for
  the lower 32 bits.

  @param CL      - Shift amount (in bits)
  @param EDX:EAX - 64 bit number to shift.


  @retval EDX:EAX - 64 bit result of shift right.

**/
{
#if __GNUC__
  asm (
    "cmp  $32, %%cl;"
    "jae  ShiftHighDword;"
    "shrd %%cl, %%eax, %%edx;"
    "shr  %%cl, %%edx;"
    "ret;"
    "ShiftHighDword:;"
    "mov  %%edx, %%eax;"
    "xor  %%edx, %%edx;"
    "sub  $32, %%cl;"
    "shr  %%cl, %%eax;"
    "ret;"
    : /* no output */
  : /* no input */
  : "%eax", "%edx"
    );
#else
  _asm {
    cmp  cl, 32
    jae  ShiftHighDword   ; If shifting more than 31 bits, ignore the low DWORD since it be shifted out.
    shrd eax, edx, cl
    shr  edx, cl
    ret
ShiftHighDword:
    mov  eax, edx
    xor  edx, edx
    sub  cl,  32          ; We already shifted the value by 32 bits by moving EDX into EAX. So just shift what is left over above 32 bits.
    shr  eax, cl
    ret
  }
#endif
}
#endif
