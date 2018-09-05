//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Improve JEDEC NVDIMM restore code.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Jan/11/2017
//
//****************************************************************************
//****************************************************************************

#include "SysFunc.h"
//
// Disable warning for unsued input parameters
//
#pragma warning(disable : 4100)

#ifdef MEM_NVDIMM_EN
#define SPD_MODULE_PN_SIZE  18
#define NV_NAME_LEN         18
#define NV_REPORT           1
#define NV_REPORT_NOT       0

typedef UINT32(*NVDIMM_FUNC)(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 vendorIndex, struct smbDevice dev);


UINT32 JEDECSwitchPage(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  struct smbDevice  smbusdev,
  UINT8            page
  );


typedef struct _nvDev {
  UINT16         nvVid;                              /* SPD Vender ID                                                       */
  UINT8          nvDid[SPD_MODULE_PN_SIZE + 1];        /* SPD Device ID                                                       */
  UINT32         nvDidCnt;                           /* Str Len for nvDid match: Multiple matched devices use this control table  */
  UINT8          nvName[NV_NAME_LEN];                /* sZ Vender name                                                      */
  UINT8          smbusAdr;                           /* SMBus address used for NVDIMM operations                            */
  UINT8         vendorIndex;  // Unique number per vendor to allow handling multiple NVDIMMs from one function
  NVDIMM_FUNC    getStatus;
  NVDIMM_FUNC    startRestore;
  NVDIMM_FUNC    restoreDone;
  NVDIMM_FUNC    arm;
} nvDev;

UINT32
ReadSmbNVDIMM(
  PSYSHOST    host,
  UINT8       socket,
  UINT8       ch,
  UINT8       dimm,
  struct smbDevice  dev,
  UINT8       byteOffset,
  UINT8       *data
  )
{
  UINT32 status;
  status = CHIP_FUNC_CALL(host, ReadSmb(host, socket, dev, byteOffset, data));
  if (status == EFI_SUCCESS)
    rcPrintf((host, "N%d.C%d.D%d: ReadSmb:Offset=0x%x, Value Read=0x%x,status=0x%x\n", socket, ch, dimm, byteOffset, *data, status));
  else
    rcPrintf((host, "N%d.C%d.D%d: ReadSmb:Offset=0x%x, Read failed, ret value=0%x\n", socket, ch, dimm, byteOffset, status));

  return status;
}


UINT32
WriteSmbNVDIMM(
  PSYSHOST    host,                             // Pointer to the system host (root) structure
  UINT8       socket,                           // Processor socket ID
  struct smbDevice  dev,                        // Device address on SMBUS
  UINT8       byteOffset,                       // Offset in Device
  UINT8       *data                             // Pointer to store value
  //   NOTE: For a PCH block read, the array must be 33bytes in length,
  //   and the first data byte pointed to is the number of bytes read.
  //   This implementation only allows a maximum of 32bytes to be read.
  )
{
  UINT32 status;
  status = CHIP_FUNC_CALL(host, WriteSmb(host, socket, dev, byteOffset, data));
  rcPrintf((host, "  WriteSmb:Offset=0x%x, Value Written=0x%x, status=0x%x\n", byteOffset, *data,status));

  if (status != EFI_SUCCESS)
    rcPrintf((host, "  WRITE FAILED\n"));
  
  return status;
}



UINT32
ReadSmbTimeout(
  PSYSHOST         host,
  UINT8            socket,
  UINT8            ch,
  UINT8            dimm,
  struct smbDevice smbusdev,
  UINT8            byteOffset,
  UINT8            *data,
  UINT8            mask,
  UINT8            expectedvalue,
  UINT16           timeout
  )
{
  UINT32        passnum = 0;
  UINT32        status;
  UINT64_STRUCT startTsc;
  UINT64_STRUCT currentTsc;

  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
    "    Starting a Poll Reg=0x%x, Mask=0x%x, Expected Value=0x%x\n", byteOffset, mask, expectedvalue));

  ReadTsc64(&startTsc);
  do {
    status = CHIP_FUNC_CALL(host, ReadSmb(host, socket, smbusdev, byteOffset, data));

    if ((passnum & 0xFF) == 1) {
      if (status == SUCCESS)
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "        Data=0x%x\n", *data));
      else
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "      SMBUS Read FAILED!!!! Status=0x%x\n", status));
    }

    if (passnum != 0)
      FixedDelay(host, 10000);

    passnum++;

    ReadTsc64(&currentTsc);
    if (TimeDiff(host, startTsc, currentTsc, TDIFF_UNIT_S) > timeout) {
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "      ReadSmbTimeoutTimeout after %d seconds, Reg=0x%x, Mask=0x%x, Expected Value=0x%x, last read=0x%x, last status=0x%x\n", timeout, byteOffset, mask, expectedvalue, *data, status));
      return FAILURE;
    }
  } while (((*data & mask) != expectedvalue) || (status != SUCCESS));
  MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "      Final Data=0x%x\n", *data));
  return SUCCESS;
} // ReadSmbTimeout

UINT32
NetlistStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32  status;
  UINT8   data;

  // Wait for NVDIMM to init
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x9, &data, (1 << 3), (1 << 3), 6);
  if (status == FAILURE) return NVDIMM_ERROR;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "  Making sure there aren't any operations in progress\n"));
#endif // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x8, &data, 0x7, 0, 5 * 60);

  FixedDelay(host, 1000000);

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0x20, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data & (1 << 7)) return NVDIMM_ERROR;

  // Read Nonvolatile Status Flag
  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xC, &data);
  if (status == FAILURE)
    return NVDIMM_ERROR;

  switch (data) {
  case 0:
  case 1:
    return NVDIMM_NODATA;
    break;

  case 3:
    return NVDIMM_RESTORE;
    break;

  default:
  case 2:
    return NVDIMM_ERROR;
    break;
  }

  //Should never make it here
  //return NVDIMM_ERROR;
} // NetlistStatus


UINT32
NetlistRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  data = 1;
  status = WriteSmbNVDIMM(host, socket, smbusdev, 0x4, &data);

  return SUCCESS;
} // NetlistRestoreStart

UINT32
NetlistRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT8   i;
  UINT32  status;

  //Wait for State to change to 0x8
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x10, &data, 0xFF, 0x8, 5 * 60);
  if (status == FAILURE) return FAILURE;

  // Paranoia: Also check for RIP bit to go low
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x8, &data, (1 << 1), 0, 60);
  if (status == FAILURE) return FAILURE;

  if (data & (1 << 7)) {
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "Error during restore\n"));
#endif // SERIAL_DBG_MSG

    for (i = 0x20; i < 0x28; i++) {
      status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, i, &data);
#ifdef SERIAL_DBG_MSG
      if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "Error Reg 0x%x=0x%x\n", i, data));
#endif // SERIAL_DBG_MSG
      status = WriteSmbNVDIMM(host, socket, smbusdev, i, &data);
    } // i loop
  }

  return SUCCESS;
} // NetlistRestoreDone

UINT32
NetlistArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;


#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Waiting for NVDIMM to charge\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 8, &data, (1 << 4), (1 << 4), 10 * 60);
  if (status == FAILURE) return FAILURE;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Erasing\n"));
#endif // SERIAL_DBG_MSG

  // Always erase
  // Initiate Erase
  data = (1 << 2) | (1 << 0);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x4, &data);
  // Wait for Erase to Finish
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 8, &data, (1 << 2), 0, 60);
  if (status == FAILURE) return FAILURE;

  // Enable SAVE_N pin
  data = (1 << 6) | (1 << 1) | (1 << 0);
  status = WriteSmbNVDIMM(host, socket, smbusdev, 0x4, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Making sure NVDIMM is armed\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x9, &data, (1 << 1) | (1 << 2), (1 << 1) | (1 << 2), 60);
  if (status == FAILURE) {
#ifdef SERIAL_DBG_MSG
    status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xC, &data);
    if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "NV STATUS Register = 0x%x\n", data));
#endif // SERIAL_DBG_MSG
    return FAILURE;
  }

  return SUCCESS;
} // NetlistArm


UINT32
AgigaStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Wait for NVDIMM Enabled\n"));
#endif // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 9, &data, 0xff, 0xa5, 6);
  if (status == FAILURE) return FAILURE;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX))   rcPrintf((host, "Making sure there isn't a Restore in Progress\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xA, &data, 0xff, 0, (2 * 60));
  if (status == FAILURE) return FAILURE;

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 20, &data);

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Backup Status Register = 0x%x\n", data));
#endif // SERIAL_DBG_MSG

  if (data & (1 << 1))
    return NVDIMM_RESTORE;
  else
    return NVDIMM_NODATA;
} // AgigaStatus

UINT32
AgigaRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8 data;

  // Start the Restore
  data = 0x96;
  WriteSmbNVDIMM(host, socket, smbusdev, 11, &data);

  return SUCCESS;
} // AgigaRestoreStart

UINT32
AgigaRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Wait until Restore is complete
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 11, &data, 0xff, 0, (5 * 60));
  if (status == FAILURE) return FAILURE;

  ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 21, &data);
  if (!(data & 0x1)) {

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) {
      rcPrintf((host, "Restore Status Register = 0x%x\n", data));
      rcPrintf((host, "Restore FAILED\n", data));
    }
#endif // SERIAL_DBG_MSG
    return FAILURE;
  }

  return SUCCESS;
} // AgigaRestoreDone

UINT32
AgigaArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Make sure Cap is charged

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Waiting for Cap to Charge\n"));
#endif // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 18, &data, 7, 7, (10 * 60));
  if (status == FAILURE) return FAILURE;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Erasing the NVDIMM\n"));
#endif // SERIAL_DBG_MSG
  data = 0x37;
  status = WriteSmbNVDIMM(host, socket, smbusdev, 0xC, &data);

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x12, &data, 0x7F, 0x7F, 60);
  if (status == FAILURE) return FAILURE;
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "GTG1=0x%x,", data));
#endif // SERIAL_DBG_MSG
  ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0x13, &data);

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, " GTG2=0x%x\n", data));
#endif // SERIAL_DBG_MSG
  // Arming the NVDIMM
  data = 0x18;
  status = WriteSmbNVDIMM(host, socket, smbusdev, 8, &data);

  return SUCCESS;
} // AgigaArm

UINT32 VikingStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Disable All Save Triggers
  data = 0xC0;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x1, &data);


  // Make sure a Save isn't in progress

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, vendorIndex, &data, 3, 0, 2 * 60);
  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "FAILED to read Status register1\n"));
#endif // SERIAL_DBG_MSG

    return NVDIMM_ERROR;
  }


  // Issue a Reset Command
  data = 0x8F;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Starting 7s Delay\n"));
#endif
  FixedDelay(host, 1000000);
  FixedDelay(host, 1000000);
  FixedDelay(host, 1000000);
  FixedDelay(host, 1000000);
  FixedDelay(host, 1000000);
  FixedDelay(host, 1000000);
  FixedDelay(host, 1000000);

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, vendorIndex, &data);
  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "FAILED to read Status register1\n"));
#endif // SERIAL_DBG_MSG

    return NVDIMM_ERROR;
  }

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "Status Register = 0x%x\n", data));
#endif // SERIAL_DBG_MSG

  if (data & (1 << 2)) {
    // Check for Host control again
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xC, &data, 1, 1, 120);
    return NVDIMM_RESTORE;
  }
  else {
    // Switch Memory control to FPGA
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX))
      rcPrintf((host, "Switching control to FPGA\n"));
#endif
    data = 0x80;
    WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Waiting for DDR Init Bit to go high\n"));
#endif
    // Wait for DDR Init Bit to go high
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xC, &data, (1 << 1), (1 << 1), 30);
    if (status != SUCCESS) return NVDIMM_ERROR;

#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Switching Control back to host\n"));
#endif
    // Switch Memory control back to CPU
    data = 0x88;
    WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Checking for Host Control\n"));
#endif
    // Check for Host control again
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xC, &data, 1, 1, 120);
    // Figure Out how to Handle this better
    if (status != SUCCESS) return NVDIMM_ERROR;

    return NVDIMM_NODATA;
  }
} // VikingStatus

UINT32
  VikingRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Switch Memory control to FPGA
  data = 0x80;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

  FixedDelay(host, 5000);

  // Wait for DDR Init Bit to go high
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xC, &data, (1 << 1), (1 << 1), 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  // Make sure NVDIMM is idle
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, vendorIndex, &data, 3, 0, 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  // Make sure SSD is Ready
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 6, &data, (1 << 4), (1 << 4), 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  // Start Restore
  data = 0x82;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

  FixedDelay(host, 5000);

  // Make sure Restore has started
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, vendorIndex, &data, 3, 3, 30);
  if (status != SUCCESS) return FAILURE;

  FixedDelay(host, 5000);

  return SUCCESS;
} // VikingRestoreStart


UINT32
VikingRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  restorestatus;
  UINT32  status;
  //volatile int a = 0;

  // Waiting for restore to complete
  restorestatus = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, vendorIndex, &data, (1 << 3), (1 << 3), 5 * 60);

  FixedDelay(host, 5000);

  // Switch Memory control back to CPU
  data = 0x88;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

  // Check for Host control again
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xC, &data, 1, 1, 30);

  // Figure Out how to Handle this better
  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "Couldn't switch back control to host\n"));
#endif
    //while (a == 0);

    return FAILURE;
  }

  return restorestatus;
} // VikingRestoreDone


UINT32
VikingArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "Waiting for Cap to charge\n"));
#endif
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, vendorIndex, &data, (1 << 5), (1 << 5), 10 * 60);

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Clearing SDV\n"));
#endif
  data = 0x40;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x5, &data);

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "Making sure SAVE pin is only save trigger\n"));
#endif
  data = 0;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x1, &data);

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, vendorIndex, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Status Register0 = 0x%x\n", data));
#endif

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xC, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "Status Register1 = 0x%x\n", data));
#endif

  return status;
} // VikingArm

UINT32
SmartUnlock(
PSYSHOST          host,
UINT8             socket,
UINT8             ch,
UINT8             dimm,
struct smbDevice  smbusdev
  )
{
  UINT32  status;
  UINT8   data;
  UINT8   unlockdata[] = { 'S', 'M', 'O', 'D' };

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Unlocking\n"));
#endif // SERIAL_DBG_MSG

  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[0]);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[1]);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[2]);
  WriteSmbNVDIMM(host, socket, smbusdev, 0x3D, &unlockdata[3]);

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x3D, &data, 0x80, 0x80, 10);

  if (status != SUCCESS) {
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "    Smart Unlock:FAILED\n"));
#endif // SERIAL_DBG_MSG
    return FAILURE;
  }

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Unlock:Passed\n"));
#endif // SERIAL_DBG_MSG
  return SUCCESS;
} // SmartUnlock

UINT32 SmartStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  /*
  status = JEDECSwitchPage(host, socket, smbusdev, 4);

  if (status == EFI_SUCCESS) { 
    rcPrintf((host,"Switched from JEDEC to Smart Command set\n"));  
  } else {
    rcPrintf((host,"FAILED to switch to JEDEC to Smart Command set\n"));
    return NVDIMM_ERROR;
  }
  */

  //Make Sure NVDIMM is alive
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xF, &data, 0x80, 0x80, 10);

  status = SmartUnlock(host, socket, ch, dimm, smbusdev);
  if (status == FAILURE) return NVDIMM_ERROR;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Making sure there isn't a Restore in Progress\n"));
#endif // SERIAL_DBG_MSG

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x8, &data, (1 << 1), 0, 4 * 60);
  if (status == FAILURE) return NVDIMM_ERROR;

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xA, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Status/Event #1(0xA)=0x%x\n", data));
#endif // SERIAL_DBG_MSG
  // Clear the register
  WriteSmbNVDIMM(host, socket, smbusdev, 0xA, &data);

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xB, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Status/Event #2(0xB)=0x%x\n", data));
#endif // SERIAL_DBG_MSG
  // Clear the register
  WriteSmbNVDIMM(host, socket, smbusdev, 0xB, &data);

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xE, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    State Information #1(0xE)=0x%x\n", data));
#endif

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xF, &data);
#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    State Information #2(0xF)=0x%x\n", data));
#endif
  if (status == FAILURE) {
#ifdef SERIAL_DBG_MSG
    if (checkMsgLevel(host, SDBG_MINMAX)) rcPrintf((host, "    FAILED to read State Information Register"));
#endif // SERIAL_DBG_MSG
    return NVDIMM_ERROR;
  }

  if (data & (1 << 2))
    return NVDIMM_RESTORE;
  else
    return NVDIMM_NODATA;
} // SmartStatus


UINT32
SmartRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8 data;

  // Start the Restore
  data = 1 << 4;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x8, &data);

  return SUCCESS;
} // SmartRestoreStart

UINT32
SmartRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

  // Waiting for restore to complete
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x8, &data, (1 << 4), 0, 2 * 60);
  if (status == FAILURE) return FAILURE;

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xB, &data, 1, 1, 10);
  if (status == FAILURE) return FAILURE;

  // Check for Restore Failure
  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xB, &data);
  if (status == FAILURE) return FAILURE;

  if (data & (1 << 3)) return FAILURE;

  // Clear Trigger Done bit
  WriteSmbNVDIMM(host, socket, smbusdev, 0xB, &data);

  return SUCCESS;
} // SmartRestoreDone


UINT32
SmartArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT8   data;
  UINT32  status;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Waiting for the CAP to charge\n"));
#endif  // SERIAL_DBG_MSG
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xF, &data, (1 << 3), (1 << 3), 3 * 60);

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Erasing Flash\n"));
#endif  // SERIAL_DBG_MSG
  data = 1 << 5;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x8, &data);

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0x8, &data, (1 << 5), 0, 10);
  if (status == FAILURE) return FAILURE;

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xB, &data, 1, 1, 10);
  if (status == FAILURE) return FAILURE;

  // Clear Trigger Done bit
  WriteSmbNVDIMM(host, socket, smbusdev, 0xB, &data);

  // Check for Erase errors
  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, 0xA, &data);
  if (status == FAILURE)
    return FAILURE;
  if (data & (1 << 1))
    return FAILURE;

#ifdef SERIAL_DBG_MSG
  if (checkMsgLevel(host, SDBG_MAX)) rcPrintf((host, "    Arming NVDIMM\n"));
#endif  // SERIAL_DBG_MSG
  data = 1;
  WriteSmbNVDIMM(host, socket, smbusdev, 0x7, &data);

  // Verify NVDIMM Armed
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, 0xE, &data, 0x80, 0x80, 10);
  if (status == FAILURE) return FAILURE;

  return SUCCESS;
} // SmartArm



#define NVDIMM_INIT_TIMEOUT 203

#define OPEN_PAGE         0x0

#define HOST_MAX_OPERATION_RETRY 0x15

#define CSAVE_TIMEOUT0     0x18
#define CSAVE_TIMEOUT1     0x19

#define PAGE_SWITCH_LATENCY0  0x1A
#define PAGE_SWITCH_LATENCY1  0x1B

#define RESTORE_TIMEOUT0   0x1C
#define RESTORE_TIMEOUT1   0x1D

#define ERASE_TIMEOUT0     0x1E
#define ERASE_TIMEOUT1     0x1F

#define ARM_TIMEOUT0       0x20
#define ARM_TIMEOUT1       0x21

#define FIRMWARE_OPS_TIMEOUT0 0x22
#define FIRMWARE_OPS_TIMEOUT1 0x23

#define ABORT_CMD_TIMEOUT0   0x24
#define ABORT_CMD_TIMEOUT1   0x25


#define NVDIMM_MGT_CMD0      0x40
#define RST_CTRL              BIT0;
#define CL_SAVE_STAT          (1<<2)
#define CL_SET_ES_STAT        (1<<7)
#define NVDIMM_MGT_CMD1      0x41
#define NVDIMM_FUNC_CMD0     0x43
#define START_SAVE            (1<<1)
#define START_RESTORE         (1<<2)
#define START_ERASE           (1<<3)
#define ABORT_CURRENT_OP      (1<<4)
#define ARM_CMD             0x45
#define SET_EVENT_NOTIFICATION_CMD 0x47
#define SET_ES_POLICY_CMD   0x49


#define NVDIMM_READY        0x60
#define NVDIMM_CMD_STATUS   0x61
#define SAVE_IN_PROGRESS     (1<<2)
#define RESTORE_IN_PROGRESS  (1<<3)
#define ERASE_IN_PROGRESS    (1<<4)
#define ABORT_IN_PROGRESS    (1<<5)
#define ARM_IN_PROGRESS      (1<<6)

#define ABORT_SUCCESS         (1<<4)
#define ABORT_ERROR           (1<<5)
#define SAVE_STATUS         0x64
#define RESTORE_STATUS0      0x66
#define ERASE_STATUS0        0x68
#define ARM_STATUS0          0x6A
#define FACTORY_DEFAULT_STATUS0         0x6C
#define SET_EVENT_NOTIFICATION_STATUS0  0x6E
#define SET_ES_POLICY_STATUS          0x70
#define FIRMWARE_OPS_STATUS0            0x71

#define CSAVE_INFO          0x80
#define CSAVE_FAIL_INFO0    0x84
#define CSAVE_FAIL_INFO1    0x85

#define MODULE_HEALTH       0xA0
#define MODULE_HEALTH_STATUS0     0xA1
#define MODULE_HEALTH_STATUS1     0xA2
#define MODULE_OPS_CONFIG  0xAA
#define CSAVE_FAIL_INFO0      0x84
#define CSAVE_FAIL_INFO1      0x85

#define ES_CHARGE_TIMEOUT0 0x10
#define ES_CHARGE_TIMEOUT1 0x11

UINT32 JEDECDumpStatusRegs(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8  data;

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, NVDIMM_READY, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (0xA5 != data){
    rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_READY=0x%x != 0xA5\n", socket, ch, dimm, data));
  }
  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[0] - Controller busy\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[1] - FACTORY_DEFAULT \n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[2] - SAVE in Progress \n", socket, ch, dimm));
    if (data & (1 << 3))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[3] - RESTORE in Progress \n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[4] - ERASE in Progress \n", socket, ch, dimm));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[5] - ABORT in Progress \n", socket, ch, dimm));
    if (data & (1 << 6))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[6] - ARM in Progress \n", socket, ch, dimm));
    if (data & (1 << 7))
      rcPrintf((host,   "N%d.C%d.D%d:NVDIMM_CMD_STATUS[7] - FIRMWARE_OPS in Progress \n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, SAVE_STATUS, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:SAVE_STATUS[0] - SAVE SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:SAVE_STATUS[1] - SAVE ERROR\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:SAVE_STATUS[4] - ABORT SUCESS\n", socket, ch, dimm));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:SAVE_STATUS[5] - ABORT ERROR\n", socket, ch, dimm));
    if (data & ((1 << 2) | (1 << 3) | (1 << 6) | (1 << 7)))
      rcPrintf((host,   "N%d.C%d.D%d:SAVE_STATUS=0x%x[3:2,7:6] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, CSAVE_INFO, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_INFO[0]-NVM_Data_Valid\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_INFO[1]-START_SAVE_CMD\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_INFO[2]-SAVE_N\n", socket, ch, dimm));
    if (data & (0xf8))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_INFO=0x%x[7:3]-reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, CSAVE_FAIL_INFO0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[0]-VOLTAGE_REGULATOR_FAILED\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[1]-VDD_LOST\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[2]-VPP_LOST\n", socket, ch, dimm));
    if (data & (1 << 3))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[3]-VTT_LOST\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[4]-DRAM_NOT_SELF_REFRESH\n", socket, ch, dimm));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[5]-CONTROLLER_HARDWARE_ERROR\n", socket, ch, dimm));
    if (data & (1 << 6))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[6]-NVM_CONTROLLER_ERROR\n", socket, ch, dimm));
    if (data & (1 << 7))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO0[7]-NVM_MEDIA_ERROR\n", socket, ch, dimm));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, CSAVE_FAIL_INFO1, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO1[0]-NOT_ENOUGH_ENERGY_FOR_CSAVE\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_FAIL_INFO1[1]-PARTIAL_DATA_SAVED\n", socket, ch, dimm));
    if (data & (0xfC))//bits 7:2
      rcPrintf((host,   "N%d.C%d.D%d:CSAVE_INFO=0x%x[7:2]-reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, RESTORE_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:RESTORE_STATUS0[0]-RESTORE_SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:RESTORE_STATUS0[1]-RESTORE_ERROR\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:RESTORE_STATUS0=0x%x[4] - ABORT SUCESS\n", socket, ch, dimm, data));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:RESTORE_STATUS0=0x%x[5] - ABORT ERROR\n", socket, ch, dimm, data));
    if (data & ((1 << 2) | (1 << 3) | (1 << 6) | (1 << 7)))
      rcPrintf((host,   "N%d.C%d.D%d:RESTORE_STATUS0=0x%x[3:2,7:6] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, ERASE_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:ERASE_STATUS0[0]-RESTORE_SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:ERASE_STATUS0[1]-RESTORE_ERROR\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:ERASE_STATUS0[4] - ABORT SUCESS\n", socket, ch, dimm, data));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:ERASE_STATUS0[5] - ABORT ERROR\n", socket, ch, dimm, data));
    if (data & ((1 << 2) | (1 << 3) | (1 << 6) | (1 << 7)))
      rcPrintf((host,   "N%d.C%d.D%d:ERASE_STATUS0=0x%x[3:2,7:6] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, ARM_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:ARM_STATUS0[0]-ARM_SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:ARM_STATUS0[1]-ARM_ERROR\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:ARM_STATUS0[2]-SAVE_N_ARMED\n", socket, ch, dimm));
    if (data & (0xF8))
      rcPrintf((host,   "N%d.C%d.D%d:ARM_STATUS0=0x%x[7:3] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, FACTORY_DEFAULT_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:FACTORY_DEFAULT_STATUS0[0]-FACTORY_DEFAULT_SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:FACTORY_DEFAULT_STATUS0[1]-FACTORY_DEFAULT_ERROR\n", socket, ch, dimm));
    if (data & (0xFC))
      rcPrintf((host,   "N%d.C%d.D%d:ARM_STATUS0=0x%x[7:2] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, SET_EVENT_NOTIFICATION_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:SET_EVENT_NOTIFICATION_STATUS0[0]-SET_EVENT_NOTIFICATION_SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:SET_EVENT_NOTIFICATION_STATUS0[1]-SET_EVENT_NOTIFICATION_ERROR\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:SET_EVENT_NOTIFICATION_STATUS0[2]-PERSISTENCY_ENABLED\n", socket, ch, dimm));
    if (data & (1 << 3))
      rcPrintf((host,   "N%d.C%d.D%d:SET_EVENT_NOTIFICATION_STATUS0[3]-WARNING_THRESHOLD_ENABLED\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:SET_EVENT_NOTIFICATION_STATUS0[4]-VOLTAGE_REGULATOR_FAIL_ENABLED\n", socket, ch, dimm));
    if (data & (0xE0))
      rcPrintf((host,   "N%d.C%d.D%d:SET_EVENT_NOTIFICATION_STATUS0=0x%x[7:5] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, SET_ES_POLICY_STATUS, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:SET_ES_POLICY_STATUS[0]-SET_ES_POLICY_SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:SET_ES_POLICY_STATUS[1]-SET_ES_POLICY_ERROR\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:SET_ES_POLICY_STATUS[2]-DEVICE_MANAGED_POLICY_ENABLED\n", socket, ch, dimm));
    if (data & (1 << 3))
      rcPrintf((host,   "N%d.C%d.D%d:SET_ES_POLICY_STATUS[3]-HOST_MANAGED_POLICY_ENABLED\n", socket, ch, dimm));
    if (data & (0xF0))
      rcPrintf((host,   "N%d.C%d.D%d:SET_ES_POLICY_STATUS=0x%x[7:4] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, FIRMWARE_OPS_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:FIRMWARE_OPS_STATUS0[0]-FIRMWARE_OPS_SUCCESS\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:FIRMWARE_OPS_STATUS0[1]-FIRMWARE_OPS_ERROR\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:FIRMWARE_OPS_STATUS0[1]-FIRMWARE_UPDATE_MODE\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:FIRMWARE_OPS_STATUS0[4] - ABORT SUCESS\n", socket, ch, dimm));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:FIRMWARE_OPS_STATUS0[5] - ABORT ERROR\n", socket, ch, dimm));
    if (data & (0xC8))
      rcPrintf((host,   "N%d.C%d.D%d:FIRMWARE_OPS_STATUS0=0x%x[3, 7:6] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, MODULE_HEALTH, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH[0]-LOST_PERSISTENCY_ERROR\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH[1]-WARNING_THRESHOLD_EXCEEDED\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH[2]-PERSISTENCY_RESTORED\n", socket, ch, dimm));
    if (data & (1 << 3))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH[3]-BELOW_WARNING_THRESHOLD\n", socket, ch, dimm));
    if (data & (0xf0))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH=0x%x[7:4] - reserved\n", socket, ch, dimm, data));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, MODULE_HEALTH_STATUS0, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[0]-VOLTAGE_REGULATOR_FAILED\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[1]-VDD_LOST\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[2]-VPP_LOST\n", socket, ch, dimm));
    if (data & (1 << 3))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[3]-VTT_LOST\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[4]-DRAM_NOT_SELF_REFRESH\n", socket, ch, dimm));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[5]-CONTROLLER_HARDWARE_ERROR\n", socket, ch, dimm));
    if (data & (1 << 6))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[6]-NVM_CONTROLLER_ERROR\n", socket, ch, dimm));
    if (data & (1 << 7))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS0[7]-NVM_LIFETIME_ERROR\n", socket, ch, dimm));
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, MODULE_HEALTH_STATUS1, &data);
  if (data){
    if (data & (1 << 0))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[0]-NOT_ENOUGH_ENERGY_FOR_CSAVE\n", socket, ch, dimm));
    if (data & (1 << 1))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[1]-INVALID_FIRMWARE_ERROR\n", socket, ch, dimm));
    if (data & (1 << 2))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[2]-CONFIG_DATA_ERROR\n", socket, ch, dimm));
    if (data & (1 << 3))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[3]-NO_ES_PRESENT\n", socket, ch, dimm));
    if (data & (1 << 4))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[4]-ES_POLICY_NOT_SET\n", socket, ch, dimm));
    if (data & (1 << 5))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[5]-ES_HARDWARE_FAILURE\n", socket, ch, dimm));
    if (data & (1 << 6))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[6]-ES_HEALTH_ASSESSMENT_ERROR\n", socket, ch, dimm));
    if (data & (1 << 7))
      rcPrintf((host,   "N%d.C%d.D%d:MODULE_HEALTH_STATUS1[7]-Reserved\n", socket, ch, dimm));
  }
  return status;
}


UINT32 JEDECSwitchPage(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
struct smbDevice  smbusdev,
  UINT8            page
  )
{
  UINT32 status;
  UINT8 currentpage;

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, OPEN_PAGE, &currentpage);
  rcPrintf((host, "N%d.C%d.D%d:Current Page=0x%x\n", socket, ch, dimm, currentpage));

  // Avoid the Page Switch if we are already on the new Page
  if (currentpage == page)
    return SUCCESS;

  WriteSmbNVDIMM(host, socket, smbusdev, OPEN_PAGE, &page);

  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, OPEN_PAGE, &currentpage, page, page, 5);
  rcPrintf((host, "N%d.C%d.D%d:New Page=0x%x\n", socket, ch, dimm,currentpage));

  if (currentpage == page)
    return SUCCESS;
  else
    return FAILURE;
}




UINT16 JEDECTimeoutInSeconds(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  struct smbDevice  smbusdev,
  UINT8            offset
  )
{
  UINT32 status;
  UINT16 timeout;
  UINT8 low, high;

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, offset, &low);
  if (status == FAILURE) {
    rcPrintf((host, "N%d.C%d.D%d:JEDECTimeoutInSeconds: Low Read Failed\n", socket, ch, dimm));
    return 0;
  }

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, offset+1, &high);
  if (status == FAILURE) {
    rcPrintf((host, "N%d.C%d.D%d:JEDECTimeoutInSeconds: High Read Failed\n", socket, ch, dimm));
    return 0;
  }

  //rcPrintf((host, "JEDECTimeoutInSeconds: high=0x%x, low=0x%x\n", high, low));
  
  //
  // ES_CHARGE_TIMEOUT always returns time in seconds,
  // so, the bit7 of the ES_CHARGE_TIMEOUT1 may not be set.
  //
  if (offset == ES_CHARGE_TIMEOUT0) 
    timeout = (high << 8) | low;
  else 
    timeout = ((high & 0x7f) << 8) | low;

  // If timescale is milliseconds divide by 1000 to convert to seconds
  if (!(high & (1 << 7)) && (offset != ES_CHARGE_TIMEOUT0)) // ES_CHARGE_TIMEOUT always returns time in seconds...
    timeout = timeout / 1000;

  //  If the time ends up being less than 1 second, return 1 second
  if (timeout == 0)
    timeout = 1;

  return timeout;
}

UINT32 JEDECStatus(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT16 timeout;
  UINT8  data = 0;


  JEDECSwitchPage(host, socket, ch, dimm, smbusdev, 0);

  //ReadSpd(host, socket, ch, dimm, NVDIMM_INIT_TIMEOUT, &data);
  //timeout = (UINT16)data;
  timeout = 10;

  rcPrintf((host, "N%d.C%d.D%d: Waiting for NVDIMM to be ready\n", socket, ch, dimm));
  // Wait for the NVDIMM to be Ready
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_READY, &data, 0xA5, 0xA5, timeout);
  if (status == FAILURE) return NVDIMM_ERROR;

  timeout = JEDECTimeoutInSeconds(host, socket, ch, dimm, smbusdev, CSAVE_TIMEOUT0);
  if (timeout == 0) return NVDIMM_ERROR;
  rcPrintf((host, "N%d.C%d.D%d: Save timeout=%d seconds\n", socket, ch, dimm, timeout));

  // Make sure there isn't a Backup in Progress
  rcPrintf((host, "N%d.C%d.D%d: Making sure there isn't a backup in progress\n", socket, ch, dimm));
  data = 0;
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
  if (status == FAILURE) return NVDIMM_ERROR;

  // Check to see if there is a backup to restore from
  rcPrintf((host, "N%d.C%d.D%d: Checking if there is a backup to restore from\n", socket, ch, dimm));
  data = 0;
  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, SAVE_STATUS, &data);
  if (status == FAILURE) return NVDIMM_ERROR;

  // SAVE_STATUS[0] - SAVE SUCCESS
  // SAVE_STATUS[1] - SAVE ERROR
  // SAVE_STATUS[4] - ABORT SUCESS
  // SAVE_STATUS[5] - ABORT ERROR
  // If there was a SAVE ERROR return NVDIMM_ERROR
  if (data & (1 << 1)) {
    rcPrintf((host, "N%d.C%d.D%d: Save Error. Status=0x%x\n", socket, ch, dimm, data));
    JEDECDumpStatusRegs(host, socket, ch, dimm, smbusdev);
    return NVDIMM_ERROR | ERROR_SAVE;
  }

  //  Check if there was a SAVE operation
  if (!(data & (1 << 0)))
    return NVDIMM_NODATA;

  // Verify there is a valid save image
  data = 0;
  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, CSAVE_INFO, &data);
  if (status == FAILURE) return NVDIMM_ERROR;
  rcPrintf((host, "N%d.C%d.D%d: CSAVE_INFO=0x%x\n", socket, ch, dimm, data));

  // CSAVE_INFO[0]-NVM_Data_Valid
  // CSAVE_INFO[1]-START_SAVE_CMD
  // CSAVE_INFO[2]-SAVE_N
  // CSAVE_INFO = 0x % x[7:3] - reserved
  if (data & (1 << 0))
    return NVDIMM_RESTORE;
  else{
    JEDECDumpStatusRegs(host, socket, ch, dimm, smbusdev);
    data = CL_SAVE_STAT;
    status = WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);
    return NVDIMM_ERROR | ERROR_RESTORE;
  }
}


UINT32 JEDECRestoreStart(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8 data = 0;
  UINT16 timeout;

#if SMCPKG_SUPPORT
  UINT32 retry = 0;
#endif

  timeout = 10;
  //1.	NVDIMM_CMD_STATUS0 : wait for Controller NOT busy.
  // Wait for the NVDIMM to be Ready
  rcPrintf((host, "N%d.C%d.D%d: Making sure NVDIMM is idle\n", socket, ch, dimm));
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
  if (status == FAILURE) return NVDIMM_ERROR;

  //2.	NVDIMM_MGT_CMD0 : set CL_RESTORE_STAT to clear restore status register.
  rcPrintf((host, "N%d.C%d.D%d: Clearing Restore Status\n", socket, ch, dimm));
  data = (1 << 3);
  WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);

#if SMCPKG_SUPPORT
RETRY_STEP_3:
#endif

  // 3.	NVDIMM_FUNC_CMD0 : START_RESTORE
  rcPrintf((host, "N%d.C%d.D%d:Starting Restore\n", socket, ch, dimm));
  data = (1 << 2);
  WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_FUNC_CMD0, &data);

  // 4.	NVDIMM_CMD_STATUS0 : check for RESTORE in progress bit to be set.If not set, check for RESTORE to be done in RESTORE_STATUS0.If restore not done, retry step 3.

#if SMCPKG_SUPPORT
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x09, timeout);  //wait for NVDIMM_CMD_STATUS to equal
																									 //0x09 timeout=10 seconds
		if(status==FAILURE) {
		//read RESTORE_STATUS0 (0ffset 0x66) if the Restore is not done (status == 0) then retry sending the command (step 3)
		status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, RESTORE_STATUS0, &data);
		if (status == SUCCESS)
		{
			if((data == 0) && retry<3)
			{
				retry++;	  
				goto RETRY_STEP_3;
			}
			else
				return FAILURE;
		}
		else
			return FAILURE;
	}
#endif

  return SUCCESS;

}

UINT32 JEDECRestoreDone(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8 data = 0;
  UINT16 timeout;
  UINT8 count;
  UINT8 retries;


  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, HOST_MAX_OPERATION_RETRY, &data);
  if (status == FAILURE) return FAILURE;

  retries = data >> 2;
  retries &= 0x3;
  
  rcPrintf((host, "N%d.C%d.D%d: NVDIMM allows for %d retries\n", socket, ch, dimm,retries));

  for (count = 0; count < retries; count++) {

    if (count != 0)
      JEDECRestoreStart(host, socket, ch, dimm, vendorIndex, smbusdev);

    timeout = JEDECTimeoutInSeconds(host, socket, ch, dimm, smbusdev, RESTORE_TIMEOUT0);
    if (timeout == 0) continue;
    rcPrintf((host, "N%d.C%d.D%d: Restore timeout=%d\n", socket, ch, dimm,timeout));

    rcPrintf((host, "N%d.C%d.D%d:Waiting for Restore to Complete\n", socket, ch, dimm));
    // 5.	NVDIMM_CMD_STATUS0 : wait for RESTORE in progress bit to be clear.If RESTORE in progress bit is not clear after RESTORE_TIMEOUT, abort the RESTORE(see 9.6).
    data = 0;
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
    if (status == FAILURE) continue;

    //  6.	RESTORE_STATUS0 : check for either RESTORE successful or error.If error or RESTORE aborted, Host may retry up to HOST_MAX_OPERATION_RETRY.
    data = 0;
    status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, RESTORE_STATUS0, &data);
    if (status == FAILURE) continue;

    rcPrintf((host, "N%d.C%d.D%d: Restore Status=0x%x\n", socket, ch, dimm, data));
    if (data & (1 << 0)) {
        return SUCCESS;
    }
    else
      continue;
  }

  rcPrintf((host, "N%d.C%d.D%d: Restore Failed after %d attempts\n", socket, ch, dimm,count));
  JEDECDumpStatusRegs(host, socket, ch, dimm, smbusdev);
  return FAILURE;

}


UINT32 JEDECArm(
  PSYSHOST          host,
  UINT8             socket,
  UINT8             ch,
  UINT8             dimm,
  UINT8             vendorIndex,
  struct smbDevice  smbusdev
  )
{
  UINT32 status;
  UINT8 data = 0;
  UINT16 timeout;
  //int i;


  
  //9.5 ERASE WORKFLOW

    //To initiate an erase, the Host shall do the following :
    //1.	NVDIMM_CMD_STATUS0 : wait for Controller NOT busy.
    // Wait for the NVDIMM to be Ready
  timeout = 10;
  rcPrintf((host, "N%d.C%d.D%d: Making sure NVDIMM is idle\n", socket, ch, dimm));
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
  if (status == FAILURE) return NVDIMM_ERROR;


  rcPrintf((host, "N%d.C%d.D%d: Setting Energy Policy to Device Managed\n", socket, ch, dimm));
  status = JEDECSwitchPage(host, socket, ch, dimm, smbusdev, 0);  
#ifndef HOST_MANAGED_POLICY
  data = BIT0;		// Device Manage Policy
#else
  data = BIT1;		// Host Manage Policy
#endif
  WriteSmbNVDIMM(host, socket, smbusdev, SET_ES_POLICY_CMD, &data);

  data = 0;
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);

  status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, SET_ES_POLICY_STATUS, &data);
#ifndef HOST_MANAGED_POLICY
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, SET_ES_POLICY_STATUS, &data, BIT2, BIT2, timeout);  // TLN Check [2] : DEVICE_MANAGED_POLICY_ENABLED 
#else
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, SET_ES_POLICY_STATUS, &data, BIT3, BIT3, timeout);  // TLN Check [3] : HOST_MANAGED_POLICY_ENABLED 
#endif
  rcPrintf((host, "N%d.C%d.D%d: SET_ES_POLICY_STATUS=0x%x\n", socket, ch, dimm, data));


  status = JEDECSwitchPage(host, socket, ch, dimm, smbusdev, 1);

  timeout = JEDECTimeoutInSeconds(host, socket, ch, dimm, smbusdev, ES_CHARGE_TIMEOUT0);
  rcPrintf((host, "N%d.C%d.D%d: Charge timeout=0x%x\n", socket, ch, dimm,timeout));

  status = JEDECSwitchPage(host, socket,ch, dimm, smbusdev, 0);


  rcPrintf((host, "N%d.C%d.D%d: Waiting for the ES to charge\n", socket, ch, dimm));
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, MODULE_HEALTH_STATUS1, &data, 1, 0, timeout);


  timeout = JEDECTimeoutInSeconds(host, socket, ch, dimm, smbusdev, ERASE_TIMEOUT0);
  if (timeout == 0) return NVDIMM_ERROR;

  rcPrintf((host, "N%d.C%d.D%d:Erase timeout=0x%x\n", socket, ch, dimm,timeout));

    //2.	NVDIMM_MGT_CMD0 : set CL_ERASE_STAT to clear restore status register.
  rcPrintf((host, "N%d.C%d.D%d: Clearing Erase Status\n", socket, ch, dimm));
    data = (1 << 4);
    WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);

    //3.	NVDIMM_FUNC_CMD0 : START_ERASE
    rcPrintf((host, "N%d.C%d.D%d: Starting Erase Command\n", socket, ch, dimm));
    data = (1 << 3);
    WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_FUNC_CMD0, &data);

    //4.	NVDIMM_CMD_STATUS0 : check for ERASE in progress bit to be set.
    rcPrintf((host, "N%d.C%d.D%d: Waiting for Erase to Start\n", socket, ch, dimm));
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, (1<<4), (1<<4), 1);

    //5.	NVDIMM_CMD_STATUS0 : wait for ERASE in progress bit to be clear.If the ERASE in progress bit is not clear after ERASE_TIMEOUT, abort the ERASE(see 9.6).
    rcPrintf((host, "N%d.C%d.D%d:Waiting for Erase to Finish\n", socket, ch, dimm));
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
    if (status == FAILURE) return FAILURE;

    //6.	ERASE_STATUS0 : check for either ERASE successful or error.If error or ERASE aborted, Host may retry up to HOST_MAX_OPERATION_RETRY.

    rcPrintf((host, "N%d.C%d.D%d:Checking Erase status\n",socket, ch, dimm));
    status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, ERASE_STATUS0, &data);
    
    rcPrintf((host, "N%d.C%d.D%d:ERASE_STATUS0=0x%x\n",socket, ch, dimm, data));
    
    //AJB HACK to take out erase status check
    if (!(data & (1 << 0))) {
      return FAILURE;
    }

  //for (i = 0; i < 30; i++) FixedDelay(host, 1000000);

        //9.4 ARM WORKFLOW
    timeout = JEDECTimeoutInSeconds(host, socket, ch, dimm, smbusdev, ARM_TIMEOUT0);
    if (timeout == 0) return NVDIMM_ERROR;
  //timeout = 120;
    rcPrintf((host, "N%d.C%d.D%d:Arm timeout=0x%x\n",socket, ch, dimm, timeout));

  //  To initiate an arm operation, the Host shall do the following :
  //1.	NVDIMM_CMD_STATUS0 : wait for Controller NOT busy.
    
    
    //2.	NVDIMM_MGT_CMD0 : set CL_ARM_STAT to clear arm status register.
    rcPrintf((host, "N%d.C%d.D%d:Clearing Arm status\n",socket, ch, dimm));
    data = (1 << 5);
    WriteSmbNVDIMM(host, socket, smbusdev, NVDIMM_MGT_CMD0, &data);

    //3.	ARM_CMD : Set the appropriate bits to reflect the save triggers to be enabled.
    rcPrintf((host, "N%d.C%d.D%d:Starting Arm operation\n",socket, ch, dimm));
    data = (1 << 0);
    WriteSmbNVDIMM(host, socket, smbusdev, ARM_CMD, &data);

    //4.	NVDIMM_CMD_STATUS0 : check for ARM in progress bit to be set.
    //6.	NVDIMM_CMD_STATUS0 : wait for ARM in progress bit to be clear.If ARM in progress bit is not clear after ARM_TIMEOUT, abort the ARM(see 9.6).
    //5.
    //6.	ARM_STATUS0 : check for either ARM successful or error.If successful, the triggers that are enabled for catastrophic save shall be set.
    
    rcPrintf((host, "N%d.C%d.D%d:Waiting for Arm to complete\n",socket, ch, dimm));
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_CMD_STATUS, &data, 0xFF, 0x0, timeout);
    if (status == FAILURE) return FAILURE;
    

    status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, MODULE_HEALTH, &data);
    rcPrintf((host, "N%d.C%d.D%d:MODULE_HEALTH=0x%x\n",socket, ch, dimm, data));

    status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, MODULE_HEALTH_STATUS0, &data);
    rcPrintf((host, "N%d.C%d.D%d:MODULE_HEALTH_STATUS0=0x%x\n",socket, ch, dimm, data));

    status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, MODULE_HEALTH_STATUS1, &data);
    rcPrintf((host, "N%d.C%d.D%d:MODULE_vHEALTH_STATUS1=0x%x\n",socket, ch, dimm, data));

        
    rcPrintf((host, "N%d.C%d.D%d:Checking Arm status\n",socket, ch, dimm));
    status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, ARM_STATUS0, &data, 0x5, 0x5, 30);
    //status = ReadSmbNVDIMM(host, socket, ch, dimm, smbusdev, ARM_STATUS, &data);
    rcPrintf((host, "N%d.C%d.D%d:ARM_STATUS0=0x%x\n",socket, ch, dimm, data));

    if (data & (1 << 0))
      return SUCCESS;
    else
      return FAILURE;
}



nvDev nv[] =
{
  { // Dummy Entry, so none of the devices have an index of 0
    0xFFFF,
  },

  { // JEDEC
    // Do Not remove this from the first entry of the table.
    // The nvVid/nvDid are designed to never match anything
    0xFFFE,              // nvVid 
    "NOMATCH",        // nvDid  
    6,                  // nvDidCnt
    "JEDEC",           // nvName
    0x08,                // smbusAdr
    0,
    JEDECStatus,
    JEDECRestoreStart,
    JEDECRestoreDone,
    JEDECArm
  },
  
  { /*----         Viking         ----*/
    0x4001,              /* nvVid    */
    "VRA7",            /* nvDid    */
    4,                   /* nvDidCnt */
    "Viking",            /* nvName   */
    0xB,                /* smbusAdr */
    0,         /* Vendor index */
    VikingStatus,
    VikingRestoreStart,
    VikingRestoreDone,
    VikingArm
  },
  { /*----         Viking         ----*/
    0x4001,              /* nvVid    */
    "VRA9",            /* nvDid    */
    4,                   /* nvDidCnt */
    "Viking",            /* nvName   */
    0x8,                /* smbusAdr */
    2,         /* Vendor index */
    VikingStatus,
    VikingRestoreStart,
    VikingRestoreDone,
    VikingArm
  },

  { /*----          Smart        ----*/
    0x9401,              /* nvVid    */
    "SGN",   /* nvDid    */
    3,                  /* nvDidCnt */
    "Smart",             /* nvName   */
    0x0B,                /* smbusAdr */
    0,
    SmartStatus,
    SmartRestoreStart,
    SmartRestoreDone,
    SmartArm
  },
  { /*----          Smart        ----*/
    0x9401,              /* nvVid    */
    "S",   /* nvDid    */
    1,                  /* nvDidCnt */
    "Smart",             /* nvName   */
    0x08,                /* smbusAdr */
    0,
    SmartStatus,
    SmartRestoreStart,
    SmartRestoreDone,
    SmartArm
  },
  { /**----      Agigatech      -----*/
    0x3400,               /* nvVid   3480 BIT7 is cleard on SPD read??? */
    "AGIGA8",             /* nvDid    */
    5,                   /* nvDidCnt max len 18 ch*/
    "Agigatech",          /* nvName   */
    0x02,                 /* smbusAdr    */
    0,
    AgigaStatus,
    AgigaRestoreStart,
    AgigaRestoreDone,
    AgigaArm
  },
  { /**----      Micron      -----*/
    0x2C00,               /* nvVid   3480 BIT7 is cleard on SPD read??? */
    "",             /* nvDid    */
    0,                   /* nvDidCnt max len 18 ch*/
    "Micron",          /* nvName   */
    0x02,                 /* smbusAdr    */
    0,
    AgigaStatus,
    AgigaRestoreStart,
    AgigaRestoreDone,
    AgigaArm
  },
  { /**----      SK Hynix      -----*/
    0xAD00,               /* nvVid   3480 BIT7 is cleard on SPD read??? */
    "",             /* nvDid    */
    0,                   /* nvDidCnt max len 18 ch*/
    "SK Hynix",          /* nvName   */
    0x02,                 /* smbusAdr    */
    0,
    AgigaStatus,
    AgigaRestoreStart,
    AgigaRestoreDone,
    AgigaArm
  },

  { /*----       Netlist         ----*/
    0x1603,              /* nvVid    */
    "NV3",        /* nvDid    */
    3,                  /* nvDidCnt */
    "Netlist",           /* nvName   */
    0x0B,                /* smbusAdr    */
    0,
    NetlistStatus,
    NetlistRestoreStart,
    NetlistRestoreDone,
    NetlistArm
  },
  {
    0x00000000    /* Table End */
  }
};

#define NV_OFFSET 206
#define MFGID_OFFSET 203
#define PN_OFFSET 201

UINT32
UnarmNVdimm
(
PSYSHOST          host,
UINT8             socket,
UINT8             ch,
UINT8             dimm
)
{
  struct smbDevice  smbusdev;
  UINT32              status;
  UINT8 data = 0;
  UINT16 timeout;


  smbusdev.compId = SPD;
  smbusdev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &smbusdev));
  smbusdev.address.deviceType = nv[1].smbusAdr;

  JEDECSwitchPage(host, socket, ch, dimm, smbusdev, 0);
  timeout = 100;
  //rcPrintf((host, "N%d.C%d.D%d: Waiting for NVDIMM to be ready\n", socket, ch, dimm));
  // Wait for the NVDIMM to be Ready
  status = ReadSmbTimeout(host, socket, ch, dimm, smbusdev, NVDIMM_READY, &data, 0xA5, 0xA5, timeout);
  if (status == FAILURE) {
  //  MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    Can not UNARM NVDIMM\n"));
    return NVDIMM_ERROR;
  }

  //Disable any previous NVDIMM arm command in ARM_CMD register
  data = 0;
  WriteSmbNVDIMM(host, socket, smbusdev, ARM_CMD, &data);
  //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    NVDIMM UNARMED\n"));
  
  return 0;

}


UINT32
CoreDetectNVDIMM(
PSYSHOST          host,
UINT8             socket,
UINT8             ch,
UINT8             dimm,
struct smbDevice  spd
  )
{
  UINT32  nvIdx = 1;
  UINT8   i;
  UINT8   SPDReg;
  UINT16  SPDMMfgId;
  UINT16 offset;
  UINT16 funcdesc;


  // First look a JEDEC NVDIMM.
  ReadSpd(host, socket, ch, dimm, SPD_KEY_BYTE2, &SPDReg);
  if ((SPDReg & 0xF0) == 0x90){
    for (offset = 204; offset < 220; offset++) {
      ReadSpd(host, socket, ch, dimm, offset + 1, &SPDReg);
      funcdesc = (UINT16)(SPDReg << 8);
      ReadSpd(host, socket, ch, dimm, offset, &SPDReg);
      funcdesc |= (UINT16)(SPDReg);

      if ((funcdesc & 0x3F) == 0x21){

      // WCS NVDIMM-N: Save NVDIMM function interface descriptor for NFIT Processing
      funcdesc = (((((funcdesc >> 5) & 0x1F)) << 8) | (funcdesc & 0x1F));
      host->nvram.mem.socket[socket].channelList[ch].dimmList[dimm].funcdesc = funcdesc;
      UnarmNVdimm(host, socket, ch, dimm);
      // WCS Comments: This routine is always returning LUT index 1 for JEDEC NVDIMM APIs
      return 1;
      }
        
    }
  }


  // If it isn't a JEDEC compliant NVDIMM, search for the NV tagging, and then through the legacy Table
  offset = 0;
  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
    offset = NV_OFFSET;
  } else {
    offset = 0;
  }

  ReadSpd(host, socket, ch, dimm, SPD_NVDIMM_ID_N + offset, &SPDReg);
  if (SPDReg != 'N') return NOT_NVDIMM;
  
  ReadSpd(host, socket, ch, dimm, SPD_NVDIMM_ID_V + offset, &SPDReg);
  if (SPDReg != 'V') return NOT_NVDIMM;


  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) offset = MFGID_OFFSET;
  ReadSpd(host, socket, ch, dimm, SPD_MMID_MSB + offset, &SPDReg);
  SPDMMfgId = (UINT16)(SPDReg << 8);
  ReadSpd(host, socket, ch, dimm, SPD_MMID_LSB + offset, &SPDReg);
  SPDMMfgId |= (UINT16)(SPDReg &~BIT7);


  if (host->nvram.mem.dramType == SPD_TYPE_DDR4) offset = PN_OFFSET;
  while (nv[nvIdx].nvVid != 0) {
    if (nv[nvIdx].nvVid == SPDMMfgId) {
      for (i = 0; i < nv[nvIdx].nvDidCnt; i++) {
        ReadSpd(host, socket, ch, dimm, SPD_MODULE_PN + i + offset, &SPDReg);
        if (nv[nvIdx].nvDid[i] != SPDReg) break;
      } // i loop

      if (i == nv[nvIdx].nvDidCnt) {
        return nvIdx;
      }
    }

    nvIdx++;
  } // while loop

  return NOT_NVDIMM;
}


UINT32
CoreNVDIMMStatus(
PSYSHOST host
)
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT32              nvVenIdx;
  UINT32              status;
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  struct channelNvram(*channelNvList)[MAX_CH];
  struct smbDevice    smbusdev;


  socket = host->var.mem.currentSocket;

  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CoreNVDIMMStatus\n"));

  channelNvList = &host->nvram.mem.socket[socket].channelList;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      nvVenIdx = (*dimmNvList)[dimm].nvDimmType;
      MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "nvVenIdx = %d\n",nvVenIdx));
      if (nvVenIdx) {

        (*dimmNvList)[dimm].nvDimmStatus = 0;

        smbusdev.compId = SPD;
        smbusdev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
        CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &smbusdev));
        smbusdev.address.deviceType = nv[nvVenIdx].smbusAdr;

        // Check if the NVDIMM has data to restore
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Calling %s getStatus with vendorIndex %d\n", nv[nvVenIdx].nvName, nv[nvVenIdx].vendorIndex));
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "  busSegment=0x%x, strapAddress0x%x, deviceType=0x%x\n", smbusdev.address.busSegment, smbusdev.address.strapAddress, smbusdev.address.deviceType));
        status = nv[nvVenIdx].getStatus(host, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);

        switch (status & 0x0F) {
        case NVDIMM_RESTORE:
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    NVDIMM HAS data to restore\n"));
          (*dimmNvList)[dimm].nvDimmStatus |= STATUS_RESTORE_NEEDED;
          break;

        case NVDIMM_NODATA:
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    NVDIMM does NOT have data to restore\n"));
          break;

        case NVDIMM_ERROR:
        default:
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    NVDIMM in ERROR condition (status = %08x)\n", status));
          (*dimmNvList)[dimm].nvDimmStatus |= ERROR_DETECT | (status & ~(0x0F));
          break;
        }


      }
    } // dimm loop
  } // ch loop
  return 0;
}

UINT32
CoreRestoreNVDIMMs(
PSYSHOST host
)
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT32              status;
  UINT32              nvVenIdx;
  UINT8              nv_ckeMask = 0;
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  struct channelNvram(*channelNvList)[MAX_CH];
  struct ddrRank(*rankList)[MAX_RANK_DIMM];
  struct smbDevice    smbusdev;


  if (!(host->setup.mem.restoreNVDIMMS && (host->setup.mem.ADRDataSaveMode == ADR_NVDIMM))) return 0;

  socket = host->var.mem.currentSocket;
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "CoreRestoreNVDIMMs\n"));

  // First Loop to start the restore on all NVDIMMs
  channelNvList = &host->nvram.mem.socket[socket].channelList;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++)    {
      nv_ckeMask = 0;
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      nvVenIdx = (*dimmNvList)[dimm].nvDimmType;
      if (nvVenIdx) {
        rankList = &(*dimmNvList)[dimm].rankList;
        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          nv_ckeMask |= 1 << (*rankList)[rank].CKEIndex;
        } // rank loop
      }

      if ((nvVenIdx) && ((*dimmNvList)[dimm].nvDimmStatus & STATUS_RESTORE_NEEDED)) {

        smbusdev.compId = SPD;
        smbusdev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
        CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &smbusdev));
        smbusdev.address.deviceType = nv[nvVenIdx].smbusAdr;

        (*dimmNvList)[dimm].nvDimmStatus &= ~(STATUS_RESTORE_NEEDED);

        CHIP_FUNC_CALL(host, ChipDeassertCKE(host, socket, ch,  nv_ckeMask));



        host->var.mem.subBootMode = NvDimmResume;
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Calling %s startRestore with vendorIndex %d\n", nv[nvVenIdx].nvName, nv[nvVenIdx].vendorIndex));
        status = nv[nvVenIdx].startRestore(host, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);

        if (status == FAILURE) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    ERROR starting NVDIMM Recovery\n"));
          (*dimmNvList)[dimm].nvDimmStatus |= ERROR_RESTORE;
        }
        else {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    Started NVDIMM Recovery\n"));
          (*dimmNvList)[dimm].nvDimmStatus |= STATUS_RESTORE_INPROGRESS;
        }
      }
    } // dimm loop
  } // ch loop



  // Second Loop is to verify that all NVDIMMs have finished their Restore
  channelNvList = &host->nvram.mem.socket[socket].channelList;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++)   {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      nvVenIdx = (*dimmNvList)[dimm].nvDimmType;

      if ((nvVenIdx) && ((*dimmNvList)[dimm].nvDimmStatus & STATUS_RESTORE_INPROGRESS)) {

        smbusdev.compId = SPD;
        smbusdev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
        CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &smbusdev));
        smbusdev.address.deviceType = nv[nvVenIdx].smbusAdr;

        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Calling %s restoreDone with vendorIndex %d\n", nv[nvVenIdx].nvName, nv[nvVenIdx].vendorIndex));
        (*dimmNvList)[dimm].nvDimmStatus &= ~(STATUS_RESTORE_INPROGRESS);
        status = nv[nvVenIdx].restoreDone(host, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);
        if (status == FAILURE) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    ERROR during NVDIMM Recovery\n"));
          (*dimmNvList)[dimm].nvDimmStatus |= ERROR_RESTORE;
        }
        else {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    Recovery Successful\n"));
          (*dimmNvList)[dimm].nvDimmStatus |= STATUS_RESTORE_SUCCESSFUL;
        }
      }
    } // dimm loop
  } // ch loop
  return 0;
} // PlatformRestoreNVDIMMs

UINT32
CoreArmNVDIMMs(
PSYSHOST host
)
{
  UINT8               socket;
  UINT8               ch;
  UINT8               dimm;
  UINT32              nvVenIdx; // index into nv[] for vender data
  UINT32              status;
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  struct channelNvram(*channelNvList)[MAX_CH];
  struct smbDevice    smbusdev;

  if (!(host->setup.mem.eraseArmNVDIMMS && (host->setup.mem.ADRDataSaveMode == ADR_NVDIMM))) return 0;
  socket = host->var.mem.currentSocket;
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "PlatformArmNVDIMMs\n"));

  channelNvList = &host->nvram.mem.socket[socket].channelList;
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      nvVenIdx = (*dimmNvList)[dimm].nvDimmType;
      if (nvVenIdx) {
        // We have an NVDIMM, setup the structures and then call routines

        smbusdev.compId = SPD;
        smbusdev.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
        CHIP_FUNC_CALL(host, GetSmbAddress(host, socket, ch, dimm, &smbusdev));
        smbusdev.address.deviceType = nv[nvVenIdx].smbusAdr;

        // Check if the NVDIMM has data to restore
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Calling %s arm with vendorIndex %d device type=0x%x, Strap addr=0x%x\n", nv[nvVenIdx].nvName, nv[nvVenIdx].vendorIndex, smbusdev.address.deviceType, smbusdev.address.strapAddress));
        status = nv[nvVenIdx].arm(host, socket, ch, dimm, nv[nvVenIdx].vendorIndex, smbusdev);
        if (status == FAILURE) {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    ERROR Arming NVDIMM\n"));
          (*dimmNvList)[dimm].nvDimmStatus |= ERROR_ARM;
        }
        else {
          MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "    NVDIMM armed\n"));
          (*dimmNvList)[dimm].nvDimmStatus |= STATUS_ARMED;
        }
      }
    } // dimm loop
  } // ch loop
  return 0;
} // PlatformArmNVDIMMs
#endif  //MEM_NVDIMM_EN

