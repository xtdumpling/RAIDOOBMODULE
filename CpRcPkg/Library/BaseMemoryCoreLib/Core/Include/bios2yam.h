#ifndef __BIOS2YAM_H__
#define __BIOS2YAM_H__

#ifndef RTL_BUILD
#include "SysHost.h"
#endif

#ifndef  RcApp
#include "unistd.h"
#else
#include <stdlib.h>
#endif

// Define Command types send over socket from client

typedef enum  { BIOS2YAM_Complete=0, BIOS2YAM_IsReady, BIOS2YAM_DeassertResetIo,
                BIOS2YAM_Exit, BIOS2YAM_BiosFailure} BIOS2YAM_DBG;

// TODO: Remove BIOS2YAM_WriteCr
typedef enum { BIOS2YAM_WriteCr=0,
                    BIOS2YAM_ReadCr,
                    BIOS2YAM_DimmCfg,
                    BIOS2YAM_SpdRead, BIOS2YAM_CteRead,
                    BIOS2YAM_DelayRefClk,  BIOS2YAM_DebugHook,
                    BIOS2YAM_None, BIOS2YAM_Ready,
                    BIOS2YAM_GetInputParam, BIOS2YAM_WriteCr32,
                    BIOS2YAM_WriteCr64, BIOS2YAM_ReadCr64,
                    BIOS2YAM_DelayFastClk,
                    BIOS2YAM_DelayQClk, BIOS2YAM_DelayDClk,
               BIOS2YAM_ProgramCADB, BIOS2YAM_Version} BIOS2YAM_CMD;

#define BIOS2YAM_SPD_MAX_SIZE 100   // 100 unsigned bytes
#define BIOS2YAM_INF_MAX_SIZE 100*8 // 100 unsigned ints
#define SPD_ARRAY_SIZE  512
#ifndef RTL_BUILD
#define INTERFACE_VERSION 1
int cinterfaceVersion;
#endif
//
// Input parameters for BIOS code.  Most are 0 for disabled and
// 1 for enabled.  BIOS will call the following function to
// populate these from CPU simulation and automatically modify
// its code path accordingly.
//

typedef enum _INPUT_PARAM {
  cpuFlows=0, // which CPU flows are enabled
  qpiFlows,   // which QPI flows are enabled
  memFlows,   // which MEM flows are enabled
  haCnt,
  enableFastCADB,
  chipRev,
  mccpubusno,
  bios_checkpoint,
  rtDefaultValue,
  cmdTiming,
  ddrFreqLimit,
  cpubusno,
  hsxXover2to2,
  opioFlows,
  readPreamble,
  writePreamble,
  disableChannel,
  channelEn,
  pagemodeEn
} Input_Param;

#ifndef RTL_BUILD

#ifdef __cplusplus
extern "C" {
#endif
void C_read_cr(unsigned int cr_addr,unsigned int *data);
void C_write_cr(unsigned int cr_addr, unsigned int data, unsigned int byte_en);
void C_write_cr32(unsigned int cr_addr, unsigned int data, unsigned int byte_en);
void C_write_cr64(unsigned int cr_addr, unsigned int hi_data, unsigned int lo_data, unsigned int byte_en);
void C_spd_read(unsigned int ch_num,unsigned int dimm_no, unsigned char spd[SPD_ARRAY_SIZE], int* spd_size);
void C_delay_q_clk(unsigned int delay_time);
void C_delay_d_clk(unsigned int delay_time);
void C_delay_f(unsigned int delay_time);
void C_delay_ref(unsigned int delay_time);
void C_debug_hook(unsigned int icode);
void C_get_input_param(Input_Param param, unsigned int attributes, unsigned int *data);
  // return 0 if project mismatch, otherwise version of interface to use.
  int C_version();
#ifndef VMSE
  void C_programCADB(int channel, int start_entry, int entries, PatCadbProg0 prog0[16], PatCadbProg1 prog1[16]);
  void C_programCADBChip (PatCadbProg0 prog0, PatCadbProg1 prog1, int *pattern0, int *pattern1);
#endif
unsigned int C_IsFnvFeedbackPresent(void);
void C_ReadFnvCfgSmb(unsigned int ch_num, unsigned int dimm_no, unsigned int reg_addr, unsigned int reg_size, unsigned int *data);
void C_WriteFnvCfgSmb (unsigned int ch_num, unsigned int dimm_no, unsigned int reg_addr, unsigned int reg_size, unsigned int data);
void C_ReadRcdCfgSmb(unsigned int ch_num, unsigned int dimm_no, unsigned int reg_addr, unsigned int reg_size, unsigned int *data);
void C_WriteRcdCfgSmb (unsigned int ch_num, unsigned int dimm_no, unsigned int reg_addr, unsigned int reg_size, unsigned int data);

int ClientConnect(char *hostName, int port);
void ClientDisconnect(int theSocket);
void cvl_connect(char *name, char* host, int port);
void cvl_disconnect(void);
void ReportError(int nret, const char *desc);
void GetChipName (char *name);

#ifdef __cplusplus
}
#endif
#endif //RTL_BUILD
#endif //header
