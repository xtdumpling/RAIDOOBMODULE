#include "MemApiSkx.h"


/**

Initialize the DDRIO interface Wrapper

@param host  - Pointer to sysHost

@retval SUCCESS

**/
UINT32
InitInterface(
  PSYSHOST  host                              // Pointer to sysHost, the system host (root) structure
);

/**

  Initialize the DDRIO interface for SKX 
  This init sequence might also apply for ICX/KNH (only when RUN_MMRC_TASK is turned off)

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
InitDdrioInterface(
    PSYSHOST  host                              // Pointer to sysHost, the system host (root) structure
    );

UINT32 InitDdrioInterfaceLate(PSYSHOST host);                                                     // CALLTAB

/**

  This function updates the function block number

  @param host          - Pointer to sysHost, the system host (root) structure
  @param rank          - Rank number (0-based)
  @param strobe        - Strobe number (0-based)
  @param regOffset     - Register offset

  @retval CSR Data

**/
UINT32
UpdateIoRegister (
  PSYSHOST    host,                             // Pointer to sysHost, the system host (root) structure
  UINT8       rank,                             // Rank number (0-based)
  UINT8       strobe,                           // Strobe number (0-based)
  UINT32      regOffset                         // Register offset
  );

UINT32 UpdateIoRegisterCh (PSYSHOST host, UINT8 strobe, UINT32 regOffset);                                                       // CHIP

void   IODdrtResetPerCh(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);

/**

  Check to see if Rank for specified socket is populated

  @param host    - Pointer to sysHost, the system host (root) structure struct
  @param socket  - Socket Id

  @retval None

**/
void   CheckRankPopLocal(PSYSHOST host, UINT8 socket);                    // CHIP

/**

  Programs turnaround times

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval SUCCESS

**/
UINT32 TurnAroundTimings(PSYSHOST host, UINT8 socket);                                              // CHIP
/**

  Programs timing parameters

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number
  @param ch      - Channel number

  @retval None

**/
void   ProgramTimings(PSYSHOST host, UINT8 socket, UINT8 ch);                                       // CHIP

void   GetWpqRidtoRt (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm);
/**

  Enables Write CRC

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval None

**/
void EnableWrCRC (PSYSHOST host, UINT8 socket);

/**

  Write to CADB

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket to send command to
  @param ch      - Channel to send command to
  @param dimm    - DIMM to send command to
  @param rank    - CS to send the command to
  @param dram    - DRAM to send the command to
  @param data    - MRS command to be sent
  @param bank    - Value to be sent

  @retval None

**/
void   WriteMRSPDA(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 dram, UINT16 data, UINT8 bank);   // CHIP
void   WriteMR6(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT16 data, UINT8 bank);   // CHIP


UINT32 PXCTraining(
  PSYSHOST  host
  );

#ifdef DEBUG_TURNAROUNDS

/**

  Compares turnaround times with HAS table of expected values
  reports via scratchpad if worse than expected

  @param host    - Pointer to sysHost, the system host (root) structure
  @param socket  - Socket number

  @retval SUCCESS

**/
UINT32 CheckTurnAroundTimings(PSYSHOST host, UINT8 socket);                                         // CHIP
#endif  // DEBUG_TURNAROUNDS

/**

  Wrapper code to gather device specific SPDData

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 GatherSPDData(PSYSHOST host);                                                                    // CALLTAB

/**

  Verifies current population does not validate POR restrictions

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 CheckPORCompat(PSYSHOST host);                                                                   // CALLTAB

/**



  @param host             - Pointer to sysHost, the system host (root) structure struct
  @param socket           - Socket Id
  @param cBits            - number of column bits
  @param rBits            - number of row bits
  @param bBits            - number of bank bits
  @param rankEnabled      - logical rank enabled
  @param dimmRank         - dimm containing logicalRank
  @param rankChEnabled    - at least one logical rank on channels enabled
                            (OR of rankEnabled for all channels)
  @param maxEnabledRank   - max logical rank enabled on node
  @param logical2Physical - Stores the rank within the channel

  @retval None

**/
void   MemRankGeometry (PSYSHOST  host, UINT8 socket, UINT8 cBits[MAX_RANK_CH][MAX_CH], UINT8 rBits[MAX_RANK_CH][MAX_CH],
                      UINT8 bBits[MAX_RANK_CH][MAX_CH], UINT8 rankEnabled[MAX_RANK_CH][MAX_CH],
                      UINT8 dimmRank[MAX_RANK_CH][MAX_CH], UINT8 rankChEnabled[MAX_CH],
                      UINT8 *maxEnabledRank, UINT8 logical2Physical[MAX_RANK_CH][MAX_CH]);              // CHIP


/**

  Initialize early memory throttling

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 InitThrottlingEarly (PSYSHOST host);                                     // CALLTAB

/**

  Initialize memory throttling

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 InitThrottling (PSYSHOST host);                                          // CALLTAB

void EnableTsod (PSYSHOST host, UINT8    socket);                               // CHIP

/**

  Initialize rank structures.  This is based on the requested
  RAS mode and what RAS modes the configuration supports. This
  routine also evalues the requested RAS modes to ensure they
  are supported by the system configuration.

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 CheckRASSupportAfterMemInit (PSYSHOST host);                             // CALLTAB

void   SetVLSModePerChannel(struct sysHost *host, UINT8 socket);                //CHIP

void   SetVLSRegionPerChannel(struct sysHost *host, UINT8 socket);                //CHIP

/**

  Adjust memory address map for mirroring
  Routine figures out the adjustment required in SAD, SAD2TAD, TAD tables.

  @param Host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 AdjustMemAddrMapForMirror (PSYSHOST Host);                               // CHIP
UINT32 AdjustMemorySizeFieldsforMirror(PSYSHOST Host);
UINT32 TADInterleave(PSYSHOST host, UINT16 TOLM);

/**

  Peform crossover calibration

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 CrossoverCalib(PSYSHOST host);                                           // CHIP

/**

  Peform crossover calibration

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 CrossoverCalibFnv(PSYSHOST host);                                        // CHIP

/**

  This function implements Sense Amp Offset training.
  SenseAmp offset cancellation
  Test VOC/senseamp settings 0 to 31 and look for transition

  @param host - Pointer to sysHost, the system host (root) structure struct

  @retval SUCCESS

**/
UINT32 SenseAmpOffset(PSYSHOST host);                                                                     // CHIP

void enforceTXDQDQSLimit(PSYSHOST  host, UINT8 socket, UINT8 ch);

/**

  Gets the CSR address and cache index for given IoGroup and side

  @param host     - Pointer to sysHost, the system host (root) structure struct
  @param ioGroup  - Pi group number
  @param side     - Pi group side
  @param cmdIndex - Cache index for the CSR

  @retval CSR Address - CSR address for given Pi group number and side

**/
UINT32 GetCmdGroupAddress(PSYSHOST host, UINT8 ioGroup, UINT8 side, UINT8 *cmdIndex);

/**

  Set starting/initial values for Clock and Control signals
  Initial values come from Analog Design

  @param host   - Pointer to sysHost, the system host (root) structure struct
  @param socket - Socket

  @retval None

**/
void   SetStartingCCC(PSYSHOST host, UINT8 socket);                                                                               // CHIP

/**

  This function returns the CH corrected offset for each used CPGC register, and corrects for HA0/1 if needed

  @param host                - Pointer to sysHost, the system host (root) structure
  @param socket              - Socket number
  @param ch                  - Channel Number
  @param regOffset           - Base register offset

  @retval New register Offset based on CH number

**/
UINT32
GetCpgcRegOffset (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT32    regOffset
  );

/**

  Write the MR2 register for all the ranks

  @param host            - Pointer to sysHost, the system host (root) structure
  @param socket          - Processor socket
  @param ch              - Channel to issue the ZQ to
  @param zqType          - Type of ZQ Calibration: Long or Short

  @retval SUCCESS

**/
UINT32
CpgcIssueZQ (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     zqType
  );

/**

  Fill in FPGA SAD CRSs for memory map from config info in host structure

  @param host   - Pointer to sysHost, the system host (root) structure struct

  @retval None

**/
void   InitFPGAMemoryMap(PSYSHOST host);                    // CHIP//

void
ReadChipWrADDDelays(
  PSYSHOST host,
  UINT8 socket,
  UINT8 ch,
  INT16* CRAddDelay, 
  PCHIP_WL_CLEANUP_STRUCT WLChipCleanUpStruct
  );
