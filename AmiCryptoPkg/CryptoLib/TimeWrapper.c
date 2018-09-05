//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: TimeWrapper. - Time Management Routines Wrapper Implementation
//                      for Cryptographic Library (used in DXE & RUNTIME).
//
// Description:     os_get_time, os_mktime 
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>

#include "includes.h"
#include "os.h"
#include "wpa_debug.h"

static EFI_TIME  EfiTime = {0,0,0,0};
static EFI_TIME *crEfiTime = &EfiTime;
/**
 * set_crypt_efitime - updates the local value of crypto EfiTime 
 * @input *EFI_TIME 
 */
void set_crypt_efitime(EFI_TIME *EfiTime)
{
    memcpy((void*)crEfiTime,(void*)EfiTime, sizeof(EFI_TIME));

    wpa_printf(MSG_INFO, "EfiTime Now :"
           "(Year=%d Month=%d Day=%d Hour=%d Min=%d Sec=%d)",
           crEfiTime->Year, crEfiTime->Month, crEfiTime->Day, crEfiTime->Hour, crEfiTime->Minute, crEfiTime->Second);
}

//
// -- EfiTime Management Routines --
//

#define IsLeap(y)   (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define SECSPERHOUR (60 * 60)
#define SECSPERDAY  (24 * SECSPERHOUR)

//
//  The arrays give the cumulative number of days up to the first of the
//  month number used as the index (1 -> 12) for regular and leap years.
//  The value at index 13 is for the whole year.
//
static int CumulativeDays[2][14] = {
  {
    0,
    0,
    31,
    31 + 28,
    31 + 28 + 31,
    31 + 28 + 31 + 30,
    31 + 28 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
  },
  {
    0,
    0,
    31,
    31 + 29,
    31 + 29 + 31,
    31 + 29 + 31 + 30,
    31 + 29 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31 
  }
};
/**
 * os_mktime - Convert broken-down time into seconds since 1970-01-01
 * @year: Four digit year
 * @month: Month (1 .. 12)
 * @day: Day of month (1 .. 31)
 * @hour: Hour (0 .. 23)
 * @min: Minute (0 .. 59)
 * @sec: Second (0 .. 60)
 * @t: Buffer for returning calendar time representation (seconds since
 * 1970-01-01 00:00:00)
 * Returns: 0 on success, -1 on failure
 *
 * Note: The result is in seconds from Epoch, i.e., in UTC, not in local time
 * which is used by POSIX mktime().
 */

int os_mktime(int year, int month, int day, int hour, int min, int sec,
	      os_time_t *t)
{
    int Year;
    int err;
    *t = 0;
    err = -1;

    wpa_printf(MSG_INFO, "Year=%d Month=%d Day=%d Hour=%d Min=%d Sec=%d",
           year, month, day, hour, min, sec);

	if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 ||
	    hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 60
    )
		goto Exit;

    //
    // Years Handling
    // UTime should now be set to 00:00:00 on Jan 1 of the current year.
    //
    for (Year = 1970, *t = 0; Year != year; Year++) {
        *t = *t + (os_time_t)(CumulativeDays[IsLeap(Year)][13] * SECSPERDAY);
    }
    
    //
    // Add in number of seconds for current Month, Day, Hour, Minute, Seconds, and TimeZone adjustment
    //
    *t = *t + 
           (os_time_t)(CumulativeDays[IsLeap(year)][month] * SECSPERDAY) + 
           (os_time_t)(((day > 0) ? day - 1 : 0) * SECSPERDAY) + 
           (os_time_t)(hour * SECSPERHOUR) + 
           (os_time_t)(min * 60) + 
           (os_time_t)sec;

    err = 0;

Exit:

    wpa_printf(MSG_INFO, "%x %a", *t, err==0?"":"Invalid time");

    return err;

}
//*******************************************************
//EFI_TIME
//*******************************************************
// This represents the current time information
/* typedef struct {
    UINT16  Year;       // 1998 - 2097
    UINT8   Month;      // 1 - 12
    UINT8   Day;        // 1 - 31
    UINT8   Hour;       // 0 - 23
    UINT8   Minute;     // 0 - 59
    UINT8   Second;     // 0 - 59
    UINT8   Pad1;
    UINT32  Nanosecond; // 0 - 999,999,999
    INT16   TimeZone;   // -1440 to 1440 or EFI_UNSPECIFIED_TIMEZONE
    UINT8   Daylight;
    UINT8   Pad2;
} EFI_TIME; */
/**
 * os_get_time - Get current time (sec, usec)
 * @t: Pointer to buffer for the time
 * Returns: 0 on success, -1 on failure
 */
int os_get_time(struct os_time *t)
{
    os_time_t Tm;
    int err;

    wpa_printf(MSG_INFO, "Time Now:");
    err = os_mktime(crEfiTime->Year, crEfiTime->Month, crEfiTime->Day, crEfiTime->Hour, crEfiTime->Minute, crEfiTime->Second, &Tm);
    t->sec = (os_time_t)Tm;
    t->usec = (os_time_t)Tm*1000000;

    return err;  
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
