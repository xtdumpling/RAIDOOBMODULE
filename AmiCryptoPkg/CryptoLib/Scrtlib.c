//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
// Name: Scrtlib.c -  Standard C Run-time Library(CRT) Interfaces
//
// Description:     _strdup; Atoi_ex; Snprintf 
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>
#include "includes.h"

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure:  _strdup
//
// Description:    The strdup() function shall return a pointer to a new string, 
//              which is a duplicate of the string pointed to by s1
//
// Input:  const char *s  - string source 
//
// Output:    pointer to duplicated string  
//
//**********************************************************************
//<AMI_PHDR_END>
CHAR8 * _strdup(const CHAR8 *s)
{
    CHAR8 *src=(CHAR8*)s, *aa=NULL;
    int size=0, maxsize = 0x1000;

    while(*src++ && size++ < maxsize);
    if(size < maxsize) {
        aa = os_malloc(size);
        memcpy((void*)aa, (void*)s, size);
    }
    return aa;
}

/* formatted Atoi func. replaces sscanf*/
//*************************************************************************
//<AMI_PHDR_START>
//
// Name: AtoiEX
//
// Description:
//  INT32 AtoiEX(IN CHAR8 *string) converts an ASCII string
//  that represents an integer into an actual INT integer value and returns the result.
//  The input string size is limited by strlen parameter
//
// Input:
//  IN CHAR8 *string
// Pointer to a CHAR8 string that represents an integer number.
//  IN UINT8 string length
// Limits the string length to parse.
//
// Output:
//  INT32 value that the string represented.
//
// Modified:
//
// Referrals:
// 
// Notes:    
// 
//<AMI_PHDR_END>
//*************************************************************************
int AtoiEX(char *s, UINT8 len, int* value)
{
    char ch;
    char str[9];
    if(len>8) len=8; // max int size
    memcpy(str, s, len);
    ch = *(str+len);
    *(str+len) = 0;
    *value = Atoi(str);
    *(str+len) = ch;

    return *value;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Snprintf
//
// Description:
//  Write formatted data to a string
//
// Input:
//  str 		Storage location for output
//  size		Maximum number of characters to store
//  format		Control string 
//  argument	Optional arguments
//
// Output:
//  number of bytes stored in buffer, not counting the terminating null character
//
//<AMI_PHDR_END>
//*************************************************************************
int Snprintf(char *str, UINTN size, const char *format, ...)
{
	VA_LIST Marker;
	UINTN   NumberOfPrinted;
	CHAR8 *s1 = NULL;

	if (str==NULL) return 0;
	//AMI mod - MDE lib version process '%s' as unicode strings
	s1 = AsciiStrStr(format, "%s");
	if (s1) s1[1] = 'a';

	VA_START (Marker, format);
	NumberOfPrinted = AsciiVSPrint (str, size, format, Marker);
	VA_END (Marker);
	
	return (int)NumberOfPrinted;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
