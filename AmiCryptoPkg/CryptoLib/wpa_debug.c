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

//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: wpa_debug.c -  Standard C Run-time Library Interface Wrapper
//
// Description:     
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>

#include "includes.h"
#include "common.h"

UINT8 crypto_trace_level = 0; // default - no traces

/**
 * wpa_set_trace_level - sets the debug message print mode
 * @level: print level: 0 - no messages, 1 - minimal, 2 - full
 */
void wpa_set_trace_level(UINT8 level)
{
    crypto_trace_level = level;
}
#ifndef CONFIG_NO_STDOUT_DEBUG

void wpa_printf(int level, const char *fmt, ...)
{
    VA_LIST   ArgList;
    CHAR8 Buffer[256];
    UINTN Length;
    CHAR8 *s1 = NULL;

    if (crypto_trace_level == 0) return;
    if (fmt==NULL) return;
    //AMI mod - MDE lib version process '%s' as unicode strings    
    s1 = AsciiStrStr(fmt, "%s");
    if (s1) s1[1] = 'a';

    VA_START(ArgList,fmt);
    Length = AsciiVSPrint (Buffer, sizeof (Buffer), fmt, ArgList);
    DEBUG ((AmiCryptoPkg_DEBUG_LEVEL, (UINT8 *)Buffer));
    VA_END(ArgList);

    if (level != MSG_MISC) DEBUG ((AmiCryptoPkg_DEBUG_LEVEL, "\n"));
}

void wpa_hexdump(int level, const char *title, const UINT8 *buf,
             size_t len)
{
    size_t i;
    size_t max_print_len;
    
    switch(crypto_trace_level) {
        case 0: return;
        case 1: max_print_len = 16; break;
        case 2: max_print_len = len; break;
        default: max_print_len = len;
    }

    wpa_printf(MSG_MISC, "%a - hexdump(len=%x):", title, (unsigned long) len);
    if (buf == NULL) {
        wpa_printf(MSG_MSGDUMP," [NULL]");
    } else {
        for (i = 0; i < len && i < max_print_len; i++) {
            wpa_printf(MSG_MISC, " %02x", buf[i]);
        }
        if(len>max_print_len)
            wpa_printf(MSG_MISC,"...>>>");
    }
    wpa_printf(MSG_MISC,"\n");}

/**
 * wpa_debug_printf_timestamp - Print timestamp for debug output
 *
 * This function prints a timestamp in seconds_from_1970.microseconds
 * format if debug output has been configured to include timestamps in debug
 * messages.
 */
void wpa_debug_print_timestamp(void)
{
    struct os_time tv;

    if(crypto_trace_level==0) return;
    os_get_time(&tv);
    wpa_printf(MSG_INFO, "Current Time: %x sec\n", (long) tv.sec);
}

/*
 * For the standard ASCII character set, control characters are those between 
 * ASCII codes 0x00 (NUL) and 0x1f (US), plus 0x7f (DEL). 
 * Therefore, printable characters are all but these, although specific compiler 
 * implementations for certain platforms may define additional control characters 
 * in the extended character set (above 0x7f).
*/
int isprint(UINT8 ch)
{
  return (ch > 0x1f) && (ch < 0x7f)?1:0;
}
/**
 * wpa_hexdump_ascii - conditional hex dump
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump with both
 * the hex numbers and ASCII characters (for printable range) are shown. 16
 * bytes per line will be shown.
 */
void wpa_hexdump_ascii(int level, const char *title, const UINT8 *buf,
                   size_t len)
{
    size_t i, llen;
    const UINT8 *pos = buf;
    const size_t line_len = 32;

    size_t max_print_len;
    
    switch(crypto_trace_level) {
        case 0: return;
        case 1: max_print_len = 32; break;
        case 2: max_print_len = len; break;
        default: max_print_len = len;
    }

//    wpa_debug_print_timestamp();
    if (buf == NULL) {
        wpa_printf(MSG_MISC,"%a - hexdump_ascii(len=%x): [NULL]\n", title, (unsigned long) len);
        return;
    }
    wpa_printf(MSG_MISC, "%a - hexdump_ascii(len=%x):\n", title, (unsigned long) len);
    while (len) {
        llen = len > line_len ? line_len : len;
        wpa_printf(MSG_MISC, "\tHEX  :");
        for (i = 0; i < llen; i++)
            wpa_printf(MSG_MISC," %02x", pos[i]);
//        for (i = llen; i < line_len; i++)
//            wpa_printf(MSG_MISC,"===");
        wpa_printf(MSG_MISC, "\n\tASCII: ");
        for (i = 0; i < llen; i++) {
            if (isprint(pos[i]))
                wpa_printf(MSG_MISC, "%c", pos[i]);
            else
                wpa_printf(MSG_MISC,"_");
        }
//        for (i = llen; i < line_len; i++)
//            wpa_printf(MSG_MISC,"=");
        wpa_printf(MSG_MISC,"\n");
        pos += llen;
        len -= llen;
    }
}
#endif //#ifndef CONFIG_NO_STDOUT_DEBUG

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
