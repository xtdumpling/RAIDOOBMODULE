#ifndef FPGA_ERROR_HANDLER
#define FPGA_ERROR_HANDLER

#define FME_ERROR_MASK   0x4008
#define FME_ERROR        0x4010
#define FME_FIRST_ERROR  0x4018
#define KTI_ERROR_EN0    0x0394
#define KTI_FERR         0x03c8
#define KTI_NERR         0x03cc

#define SMB2_CMD_MAILBOX_INDEX      0
#define SMB2_CMD_EXT_MAILBOX_INDEX  1
#define SMB2_DATA_MAILBOX_INDEX     2
#define MAILBOX_BIOS_CMD_SMB_ACCESS 0xAE

#define FPGA_ERR_VARIABLE_NAME L"FpgaErrorLog"

extern EFI_GUID   gFpgaErrorRecordGuid;

typedef struct {
  BOOLEAN  Valid;
  UINT64   FmeErrorStatus;
  UINT32   KtiFerr;
  UINT32   KtiNerr;
  UINT32   KtiErrorStat[6];

} FPGA_ERROR_LOG;
#endif
