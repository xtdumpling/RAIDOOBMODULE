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
 * Copyright 2006 - 2016, Intel Corporation All Rights Reserved.
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

//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#pragma warning(disable : 4702)
#endif

/**

  Initialize the power management timer

  @param host  - Pointer to the system host (root) structure

  @retval VOID

**/
void
InitPmTimer (
            PSYSHOST host
            )
{
  UINT32  base;
  UINT32  enable;

  //
  // Initialize chipset configuration
  // Read PM base address and enable status
  //
  base    = ReadPciCfg (host, 0, REG_ADDR (PMC_BUS, PMC_DEVICE, PMC_FUNC, REG_ACPI_BASE_ADDRESS, sizeof (UINT16)));
  enable  = ReadPciCfg (host, 0, REG_ADDR (PMC_BUS, PMC_DEVICE, PMC_FUNC, PM_ENABLE, sizeof (UINT8)));

  //
  // Use current PM base address
  //
  if (enable & BIT7) {
    host->var.common.pmBase = (UINT16) base & 0xFF80;
  }
  //
  // Initialize PM base if needed
  //
  else {
    //host->var.common.pmBase gets updated in PlatPkg OemInitializePlatformData
    WritePciCfg (
                host,
                0,
                REG_ADDR (PMC_BUS, PMC_DEVICE, PMC_FUNC, REG_ACPI_BASE_ADDRESS, sizeof (UINT16)),
                host->var.common.pmBase | BIT0
                );
    WritePciCfg (host, 0, REG_ADDR (PMC_BUS, PMC_DEVICE, PMC_FUNC, PM_ENABLE, sizeof (UINT8)), (UINT8) enable | BIT7);

  }
}

/**

  Delay time in 1 us units

  @param host    - Pointer to the system host (root) structure
  @param usDelay - number of us units to delay

  @retval VOID

**/
void
FixedDelay (
           PSYSHOST host,
           UINT32   usDelay
           )
{
  UINT64_STRUCT startTsc  = {0, 0};
  UINT64_STRUCT endTsc    = {0, 0};

  if (usDelay == 0) {
    return;
  }

  if (host->var.common.emulation & SIMICS_FLAG) {
    return ;
  }


  ReadTsc64 (&startTsc);
  //startCount = GetCount (host);
  //while (GetDelay (host, startCount) < usDelay);
  ReadTsc64 (&endTsc);
  while(TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_US) < usDelay) {
    ReadTsc64 (&endTsc);
  };

#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  //ReadTsc64 (&endTsc);
  countTrackingData(host, FIXED_DELAY, TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_US));
#endif  // SERIAL_DBG_MSG
#endif  //  DEBUG_PERFORMANCE_STATS
}

/**

  Delay time in 1 us units

  @param host    - Pointer to the system host (root) structure
  @param usDelay - number of us units to delay

  @retval VOID

**/
void
FixedDelayMem (
           PSYSHOST host,
           UINT32   usDelay
           )
{
  UINT64_STRUCT startTsc  = {0, 0};
  UINT64_STRUCT endTsc    = {0, 0};

  if (usDelay == 0) {
    return;
  }

  if (host->var.common.emulation & SIMICS_FLAG) {
    return ;
  }


  ReadTsc64 (&startTsc);
  //startCount = GetCount (host);
  //while (GetDelay (host, startCount) < usDelay);
  ReadTsc64 (&endTsc);
  while(TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_US) < usDelay) {
    ReadTsc64 (&endTsc);
  };

#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  //ReadTsc64 (&endTsc);
  countTrackingData(host, FIXED_DELAY, TimeDiff(host, startTsc, endTsc, TDIFF_UNIT_US));
#endif  // SERIAL_DBG_MSG
#endif  //  DEBUG_PERFORMANCE_STATS
}

/**

  Gets the ACPI timer value

  @param host  - Pointer to the system host (root) structure

  @retval 24-bit counter value

**/
UINT32
GetCount (
         PSYSHOST host
         )
{
  return InPort32 (host, (UINT16) (host->var.common.pmBase + IO_REG_ACPI_TIMER)) & 0xFFFFFF;
}

/**

  determines delay since the GetCount was called

  @param host        - Pointer to the system host (root) structure
  @param startCount  - 24-bit counter value from GetCount()

  @retval delay timein 1 us units

**/
UINT32
GetDelay (
         PSYSHOST host,
         UINT32   startCount
         )
{
  UINT32  stopCount;

  stopCount = GetCount (host);
  if (startCount > stopCount) {
    startCount |= 0xFF000000;
  }

  return((stopCount - startCount) * 1000U) / 3579U;
  //
  // 3.579 MHz clock
  //
}

#ifndef NT32_BUILD
#ifdef MINIBIOS_BUILD
typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
  UINT8   Pad1;
  UINT32  Nanosecond;
  INT16   TimeZone;
  UINT8   Daylight;
  UINT8   Pad2;
} EFI_TIME;
#endif
#endif // #ifndef NT32_BUILD

#define PCAT_RTC_ADDRESS_REGISTER                   0x70
#define PCAT_RTC_DATA_REGISTER                      0x71

#define RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
#define RTC_ADDRESS_REGISTER_A        10  // R/W[0..6]  R0[7]
#define RTC_ADDRESS_REGISTER_B        11  // R/W
#define RTC_ADDRESS_REGISTER_C        12  // RO
#define RTC_ADDRESS_REGISTER_D        13  // RO
#define RTC_ADDRESS_CENTURY           50  // R/W  Range 19..20 Bit 8 is R/W
//
// Register A
//
typedef struct {
  UINT8 RS : 4;   // Rate Selection Bits
  UINT8 DV : 3;   // Divisor
  UINT8 UIP : 1;  // Update in progress
} RTC_REGISTER_A_BITS;

typedef union {
  RTC_REGISTER_A_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_A;

//
// Register B
//
typedef struct {
  UINT8 DSE : 1;  // 0 - Daylight saving disabled  1 - Daylight savings enabled
  UINT8 MIL : 1;  // 0 - 12 hour mode              1 - 24 hour mode
  UINT8 DM : 1;   // 0 - BCD Format                1 - Binary Format
  UINT8 SQWE : 1; // 0 - Disable SQWE output       1 - Enable SQWE output
  UINT8 UIE : 1;  // 0 - Update INT disabled       1 - Update INT enabled
  UINT8 AIE : 1;  // 0 - Alarm INT disabled        1 - Alarm INT Enabled
  UINT8 PIE : 1;  // 0 - Periodic INT disabled     1 - Periodic INT Enabled
  UINT8 SET1 : 1;  // 0 - Normal operation.         1 - Updates inhibited
} RTC_REGISTER_B_BITS;

typedef union {
  RTC_REGISTER_B_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_B;

//
// Register D
//
typedef struct {
  UINT8 Reserved : 7; // Read as zero.  Can not be written.
  UINT8 VRT : 1;      // Valid RAM and Time
} RTC_REGISTER_D_BITS;

typedef union {
  RTC_REGISTER_D_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_D;

UINTN DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

BOOLEAN
IsLeapYear (
  UINT16   Year
  )
/*++

Routine Description:

  This function verifies the leap year

Arguments:

  Year    year in YYYY format.

Returns:

  TRUE if the year is a leap year

--*/
{
  if (Year % 4 == 0) {
    if (Year % 100 == 0) {
      if (Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

UINTN
CountNumOfLeapDays (
  EFI_TIME  *Time
  )
/*++

Routine Description:

  This function calculates the total number leap days from 1970 to the current year

Arguments:

  Time    - Current Time

Returns:

  Returns the number of leap days since the base year, 1970.

--*/
{
  UINT16  NumOfYear;
  UINT16  BaseYear;
  UINT16  Index;
  UINTN   Count;

  Count     = 0;
  BaseYear  = 1970;
  NumOfYear = Time->Year - 1970;

  for (Index = 0; Index <= NumOfYear; Index++) {
    if (IsLeapYear (BaseYear + Index)) {
      Count++;
    }
  }

  //
  // If the current year is a leap year but the month is January or February,
  // then the leap day has not occurred and should not be counted. If it is
  // February 29, the leap day is accounted for in CalculateNumOfDayPassedThisYear( )
  //
  if ( IsLeapYear( Time->Year ) ) {
    if ( (Count > 0) && (Time->Month < 3) ) {
      Count--;
    }
  }

  return Count;
}

UINTN
CalculateNumOfDayPassedThisYear (
  EFI_TIME    Time
  )
/*++

Routine Description:

  This function calculates the total number of days passed till the day in a year.
  If the year is a leap year, an extra day is not added since the number of leap
  days is calculated in CountNumOfLeapDays.

Arguments:

  Time    This structure contains detailed information about date and time..

Returns:

  Returns the number of days passed until the input day.

--*/
{
  UINTN Index;
  UINTN NumOfDays;

  NumOfDays = 0;
  for (Index = 1; Index < Time.Month; Index++) {
    NumOfDays += DaysOfMonth[Index - 1];
  }

  NumOfDays += Time.Day;
  return NumOfDays;
}

UINT8
BcdToDecimal (
  UINT8 BcdValue
  )
/*++

Routine Description:
  Function converts a BCD to a decimal value.

Arguments:
  BcdValue         - An 8 bit BCD value

Returns:

  UINT8            - Returns the decimal value of the BcdValue

--*/
{
  UINTN High;
  UINTN Low;

  High  = BcdValue >> 4;
  Low   = BcdValue - (High << 4);

  return (UINT8) (Low + (High * 10));
}

//
// RTC read functions were copied here since we need to get the time
// in both DXE and runtime code.  The PcRtc driver is not currently a
// dual mode driver, this is more efficient since making PcRtc dual mode
// would unnecessarily bloat the SMM code space.
//

UINT8
RtcRead (
  UINT8 Address
  )
/*++

Routine Description:

  Read data register and return contents.

Arguments:

  Address - Register address to read    

Returns:

  Value of data register contents

--*/
{
  OutPort8 (NULL, PCAT_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (InPort8 (NULL, PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  return InPort8 (NULL, PCAT_RTC_DATA_REGISTER);
}

VOID
RtcWrite (
  UINT8   Address,
  UINT8   Data
  )
/*++

Routine Description:

  Write data to register address.

Arguments:

  Address - Register address to write
  Data    - Data to write to register

Returns:

  None

--*/
{
  OutPort8 (NULL, PCAT_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (InPort8 (NULL, PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  OutPort8 (NULL, PCAT_RTC_DATA_REGISTER, Data);
}

VOID
ConvertRtcTimeToEfiTime (
  EFI_TIME       *Time,
  RTC_REGISTER_B RegisterB
  )
/*++

Routine Description:

Arguments:

  Time
  RegisterB

Returns:

  None

--*/
{
  BOOLEAN PM;

  if ((Time->Hour) & 0x80) {
    PM = TRUE;
  } else {
    PM = FALSE;
  }

  Time->Hour = (UINT8) (Time->Hour & 0x7f);

  if (RegisterB.Bits.DM == 0) {
    Time->Year    = BcdToDecimal ((UINT8) Time->Year);
    Time->Month   = BcdToDecimal (Time->Month);
    Time->Day     = BcdToDecimal (Time->Day);
    Time->Hour    = BcdToDecimal (Time->Hour);
    Time->Minute  = BcdToDecimal (Time->Minute);
    Time->Second  = BcdToDecimal (Time->Second);
  }
  //
  // If time is in 12 hour format, convert it to 24 hour format
  //
  if (RegisterB.Bits.MIL == 0) {
    if (PM && Time->Hour < 12) {
      Time->Hour = (UINT8) (Time->Hour + 12);
    }

    if (!PM && Time->Hour == 12) {
      Time->Hour = 0;
    }
  }

  Time->Nanosecond  = 0;
  Time->TimeZone    = 0x7FF;  //UNSPECIFIED Timezone
  Time->Daylight    = 0;
}

UINT32
RtcTestCenturyRegister (
  VOID
  )
/*++

Routine Description:

  Test Century Register.

Arguments:

  None

Returns:

  SUCCESS
  FAILURE

--*/
{
  UINT8 Century;
  UINT8 Temp;

  Century = RtcRead (RTC_ADDRESS_CENTURY);

  //
  // Always sync-up the Bit7 "semaphore"...this maintains
  // consistency across the different chips/implementations of
  // the RTC...
  //
  RtcWrite (RTC_ADDRESS_CENTURY, 0x00);
  Temp = (UINT8) (RtcRead (RTC_ADDRESS_CENTURY) & 0x7f);
  RtcWrite (RTC_ADDRESS_CENTURY, Century);
  if (Temp == 0x19 || Temp == 0x20) {
    return SUCCESS;
  }

  return FAILURE;
}

UINT32
RtcWaitToUpdate (
  UINTN Timeout
  )
/*++

Routine Description:

  Waits until RTC register A and D show data is valid

Arguments:

  Timeout - Maximum time to wait

Returns:

  SUCCESS
  FAILURE
  
--*/
{
  RTC_REGISTER_A  RegisterA;
  RTC_REGISTER_D  RegisterD;

  //
  // See if the RTC is functioning correctly
  //
  RegisterD.Data = RtcRead (RTC_ADDRESS_REGISTER_D);

  if (RegisterD.Bits.VRT == 0) {
    return FAILURE;
  }
  //
  // Wait for up to 0.1 seconds for the RTC to be ready.
  //
  Timeout         = (Timeout / 10) + 1;
  RegisterA.Data  = RtcRead (RTC_ADDRESS_REGISTER_A);
  while (RegisterA.Bits.UIP == 1 && Timeout > 0) {
    RegisterA.Data = RtcRead (RTC_ADDRESS_REGISTER_A);
    Timeout--;
  }

  RegisterD.Data = RtcRead (RTC_ADDRESS_REGISTER_D);
  if (Timeout == 0 || RegisterD.Bits.VRT == 0) {
    return FAILURE;
  }

  return SUCCESS;
}

UINT32
RtcGetTime (
  EFI_TIME              *Time
  )
/*++

Routine Description:

  Get time from RTC

Arguments:

  Time - pointer to time structure

Returns:

  SUCCESS
  FAILURE

--*/
{
  RTC_REGISTER_B  RegisterB;
  UINT8           Century;
  UINT32          Status;

  //
  // Check parameters for null pointer
  //
  if (Time == NULL) {
    return FAILURE;
  }

  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = RtcWaitToUpdate (100000);
  if (Status == FAILURE) {
    return Status;
  }

  //
  // Read Register B
  //
  RegisterB.Data = RtcRead (RTC_ADDRESS_REGISTER_B);

  //
  // Get the Time/Date/Daylight Savings values.
  //
  Time->Second  = RtcRead (RTC_ADDRESS_SECONDS);
  Time->Minute  = RtcRead (RTC_ADDRESS_MINUTES);
  Time->Hour    = RtcRead (RTC_ADDRESS_HOURS);
  Time->Day     = RtcRead (RTC_ADDRESS_DAY_OF_THE_MONTH);
  Time->Month   = RtcRead (RTC_ADDRESS_MONTH);
  Time->Year    = RtcRead (RTC_ADDRESS_YEAR);

  ConvertRtcTimeToEfiTime (Time, RegisterB);

  if (RtcTestCenturyRegister () == SUCCESS) {
    Century = BcdToDecimal ((UINT8) (RtcRead (RTC_ADDRESS_CENTURY) & 0x7f));
  } else {
    Century = BcdToDecimal (RtcRead (RTC_ADDRESS_CENTURY));
  }

  Time->Year = (UINT16) (Century * 100 + Time->Year);

  return SUCCESS;
}

UINT32
GetTimeStamp (
  UINT32 *NumOfSeconds
  )
/*+++

Routine Description:

  Return Date and Time from RTC in Unix format which fits in 32 bit format

Arguments:

  NumOfSeconds - pointer to return calculated time

Returns:

  Status

--*/
{
  UINT16                NumOfYears;
  UINTN                 NumOfLeapDays;
  UINTN                 NumOfDays;
  EFI_TIME              Time;
  UINT32                Status;

  Status = RtcGetTime (&Time);

  if (Status == FAILURE) {
    return Status;
  }

  NumOfYears      = Time.Year - 1970;
  NumOfLeapDays   = CountNumOfLeapDays (&Time);
  NumOfDays       = CalculateNumOfDayPassedThisYear (Time);

  //
  // Add 365 days for all years. Add additional days for Leap Years. Subtract off current day.
  //
  NumOfDays += (NumOfLeapDays + (365 * NumOfYears) - 1);

  *NumOfSeconds = (UINT32) (3600 * 24 * NumOfDays + (Time.Hour * 3600) + (60 * Time.Minute) + Time.Second);

  return SUCCESS;
}

UINT32
GetTimeInSecondsFrom1970 (
  VOID
  )
/*++

Routine Description:

  OEM hook for getting the system time from RTC to update System time for Memory devices (Format is number of seconds from current date to 1/1/1970 12:00:00

Arguments:

  None


Returns:
  NumOfSeconds    - Return Number of seconds from Current date to 1/1/1970 12:00:00

--*/
{
  UINT32 NumOfSeconds = 0;
  UINT32 Status = SUCCESS;

  Status = GetTimeStamp (&NumOfSeconds);
  if (Status == FAILURE) {
    NumOfSeconds = 0;
  }
  return NumOfSeconds;
}

