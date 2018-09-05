//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//**********************************************************************
// $Header: /Alaska/SOURCE/Core/Library/StdLibC.c 19    2/05/11 3:11p Artems $
//
// $Revision: 19 $
//
// $Date: 2/05/11 3:11p $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	StdLibC.c
//
// Description:
//  StdLibC.c contains a number of standard C library functions, including
// but not limited to string and cctype library functions.
//
//<AMI_FHDR_END>
//*************************************************************************
#include <AmiLib.h>

char EfiErrorStatus[][25] = 
{
	"EFI_LOAD_ERROR",
	"EFI_INVALID_PARAMETER",
	"EFI_UNSUPPORTED",
	"EFI_BAD_BUFFER_SIZE",
	"EFI_BUFFER_TOO_SMALL",
	"EFI_NOT_READY",
	"EFI_DEVICE_ERROR",
	"EFI_WRITE_PROTECTED",
	"EFI_OUT_OF_RESOURCES",
	"EFI_VOLUME_CORRUPTED",
	"EFI_VOLUME_FULL",
	"EFI_NO_MEDIA",
	"EFI_MEDIA_CHANGED",
	"EFI_NOT_FOUND",
	"EFI_ACCESS_DENIED",
	"EFI_NO_RESPONSE",
	"EFI_NO_MAPPING",
	"EFI_TIMEOUT",
	"EFI_NOT_STARTED",
	"EFI_ALREADY_STARTED",
	"EFI_ABORTED",
	"EFI_ICMP_ERROR",
	"EFI_TFTP_ERROR",
	"EFI_PROTOCOL_ERROR",
	"EFI_INCOMPATIBLE_VERSION",
	"EFI_SECURITY_VIOLATION",
	"EFI_CRC_ERROR",
    "EFI_END_OF_MEDIA",
    "EFI_UNDEFINED_29",
    "EFI_UNDEFINED_30",
    "EFI_END_OF_FILE",
    "EFI_INVALID_LANGUAGE",
    "EFI_COMPROMISED_DATA"

};

char PiErrorStatus[][25] = 
{
    "EFI_INTERRUPT_PENDING",
    "EFI_REQUEST_UNLOAD_IMAGE",
    "EFI_NOT_AVAILABLE_YET"
};

char AmiErrorStatus[][25] = 
{
	"EFI_DBE_EOF",
	"EFI_DBE_BOF"
};

char EfiWarningStatus[][26] =
{
	"EFI_WARN_UNKNOWN_GLYPH",
	"EFI_WARN_DELETE_FAILURE",
	"EFI_WARN_WRITE_FAILURE",
	"EFI_WARN_BUFFER_TOO_SMALL"
};

char PiWarningStatus[][35] =
{
	"EFI_WARN_INTERRUPT_SOURCE_PENDING",
	"EFI_WARN_INTERRUPT_SOURCE_QUIESCED"
};

#define NUM_OF_EFI_ERRORS	30
#define NUM_OF_EFI_WARNINGS	4
#define NUM_OF_AMI_ERRORS 2
#define NUM_OF_PI_ERRORS 3
#define NUM_OF_PI_WARNINGS 2

char EfiUnknown[] = "Status Code";	//If unknown value.

CHAR16 * wcsupr(CHAR16 *str);

#define true  TRUE
#define false FALSE

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Stdlib_Functions
//
// Description:
//  C standard general utilities library functions, typically found in
// stdlib.h, that are defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiLib    Atoi     Convert 8-bit ASCII string to INT32.
// AmiLib    StrtolEx Convert 8-bit ASCII string to INT64.
// AmiLib    ItoaEx   Convert INT64 to 8-bit ASCII string.
// AmiLib    ItowEx   Convert INT64 to 16-bit Unicode string.
// AmiDxeLib Malloc   Allocate memory from EfiBootServicesData.
// AmiDxeLib MallocZ  Allocate memory from EfiBootServicesData that has been cleared with zeros.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions; append .h to the given name.  None indicates that
// no header file contains a function prototype.
//  
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Ctype_Functions
//
// Description:
//  C character handling functions, typically found in ctype.h, that are
// defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// None    Toupper     Convert lowercase 8-bit ASCII character to uppercase.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions; append .h to the given name.  None indicates that
// no header file contains a function prototype.
//  
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: String_Functions
//
// Description:
//  String related functions that are defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiLib  Strlen      Find length of null-terminated 8-bit ASCII string.
// AmiLib  Wcslen      Find length of null-terminated 16-bit Unicode string.
// AmiLib  Strcpy      Copy an 8-bit ASCII string into another string.
// AmiLib  Wcscpy      Copy a 16-bit Unicode string into another string.
// AmiLib  Strcmp      Compare two 8-bit ASCII strings to each other.
// AmiLib  Wcscmp      Compare two 16-bit Unicode strings to each other.
// AmiLib  Strupr      Convert an 8-bit ASCII string into all uppercase characters.
// AmiLib  Wcsupr      Convert a 16-bit Unicode string to all uppercase characters.
// AmiLib  Strstr      Find an occurrence of an 8-bit ASCII string inside another string.
// AmiLib  Sprintf     Print a formatted 8-bit ASCII string to a buffer.
// AmiLib  Swprintf    Print a formatted 16-bit Unicode string to a buffer.
// AmiLib  Sprintf_s   Print a formatted 8-bit ASCII string to a buffer of predetermined size.
// AmiLib  Swprintf_s  Print a formatted 16-bit Unicode string to a buffer of predetermined size.
// AmiLib  ItoaEx      Convert INT64 to 8-bit ASCII string.
// AmiLib  ItowEx      Convert INT64 to 16-bit Unicode string.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions; append .h to the given name.  None indicates that
// no header file contains a function prototype.
// 
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Cstring_Functions
//
// Description:
//  C string handling functions, typically found in string.h, that are
// defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiLib  Strlen      Find length of null-terminated 8-bit ASCII string.
// AmiLib  Wcslen      Find length of null-terminated 16-bit Unicode string.
// AmiLib  Strcpy      Copy an 8-bit ASCII string into another string.
// AmiLib  Wcscpy      Copy a 16-bit Unicode string into another string.
// AmiLIb  MemCpy      Copy a buffer into another buffer.
// AmiLib  Strcmp      Compare two 8-bit ASCII strings to each other.
// AmiLib  Wcscmp      Compare two 16-bit Unicode strings to each other.
// AmiLib  MemCmp      Compare two buffers for equality.
// AmiLib  Strupr      Convert an 8-bit ASCII string into all uppercase characters.
// AmiLib  Wcsupr      Convert a 16-bit Unicode string to all uppercase characters.
// AmiLib  Strstr      Find an occurrence of an 8-bit ASCII string inside another string.
// AmiLib  MemSet      Fills a buffer with a user provided value. 
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions; append .h to the given name.  None indicates that
// no header file contains a function prototype.
// 
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_GHDR_START>
//
// Name: Stdio_Functions
//
// Description:
//  C standard I/O functions, typically found in stdio.h, that are
// defined in the AMI library.
//
// Fields: Header Function Description
// ------------------------------------------------------------------
// AmiLib  Sprintf     Print a formatted 8-bit ASCII string to a buffer.
// AmiLib  Swprintf    Print a formatted 16-bit Unicode string to a buffer.
// AmiLib  Sprintf_s   Print a formatted 8-bit ASCII string to a buffer of predetermined size.
// AmiLib  Swprintf_s  Print a formatted 16-bit Unicode string to a buffer of predetermined size.
//
// Notes:
//  Header details which header file contains the function prototype for
// the above functions; append .h to the given name.  None indicates that
// no header file contains a function prototype.
// 
//<AMI_GHDR_END>
//*************************************************************************

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Toupper
//
// Description:
//  INT32 Toupper(IN INT32 c) converts a lowercase ASCII character, c, to an
// uppercase ASCII character and returns its uppercase ASCII value.  If c is
// not a valid lowercase ASCII character, returns c.
//
// Input:
//  IN INT32 c
// ASCII value to be converted from lowercase to uppercase.
//
// Output:
//  INT32 value of c, if c is not a valid lowercase ASCII character.
//  Otherwise, INT32 ASCII value of c as an uppercase letter.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
int Toupper(int c) {
	return (c>='a' && c<='z') ? c-'a'+'A' : c;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Strlen
//
// Description:
//  UINTN Strlen(IN CHAR8 *string) takes a null-terminated CHAR8 string and
// returns its UINTN length (not including the null-terminator).
//
// Input:
//  IN CHAR8 *string
// Pointer to a null-terminated CHAR8 string.
//
// Output:
//  UINTN length of the string (not including the null-terminator).
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
UINTN Strlen(char *string) {
	UINTN length=0;
	while(*string++) length++;
	return length;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Wcslen
//
// Description:
//  UINTN Wcslen(IN CHAR16 *string) takes a null-terminated CHAR16 string and
// returns its UINTN length (not including the null-terminator).
//
// Input:
//  IN CHAR16 *string
// Pointer to a null-terminated CHAR16 string.
//
// Output:
//  UINTN length of the string (not including the null-terminator).
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
UINTN Wcslen(CHAR16 *string) {
	UINTN length=0;
	while(*string++) length++;
	return length;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Strcpy
//
// Description:
//  CHAR8* Strcpy(IN OUT CHAR8 *string1, IN CHAR8 *string2) copies the
// contents of a CHAR8 string, string2, into another CHAR8 string, string1.
//
// Input:
//  IN OUT CHAR8 *string1
// Pointer to a CHAR8 string buffer to receive the contents of string2.
// String size must be greater than or equal to string2.  User is
// responsible for allocating the necessary memory resources.
//
//  IN CHAR8 *string2
// Pointer to a null-terminated CHAR8 string to be copied.
//
// Output:
//  CHAR8* string pointer to the last character added to string1.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
char* Strcpy(char *string1,char *string2){
    char *dest = string1;
	while(*string1++=*string2++);
	return dest;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Wcscpy
//
// Description:
//  CHAR16* Wcscpy(IN OUT CHAR16 *string1, IN CHAR16 *string2) copies the
// contents of a CHAR16 string, string2, into another CHAR16 string, string1.
//
// Input:
//  IN OUT CHAR16 *string1
// Pointer to a CHAR16 string buffer to receive the contents of string2.
// String size must be greater than or equal to string2.  User is
// responsible for allocating the necessary memory resources.
//
//  IN CHAR16 *string2
// Pointer to a null-terminated CHAR16 string to be copied.
//
// Output:
//  CHAR16* string pointer to the last character added to string1.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
CHAR16* Wcscpy(CHAR16 *string1, CHAR16* string2){
    CHAR16 *dest = string1;
	while(*string1++=*string2++);
	return dest;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Strcmp
//
// Description:
//  INT32 Strcmp(IN CHAR8 *string1, IN CHAR8 *string2) compares two CHAR8
// strings, string1 and string2, and returns 0 if they are identical,
// non-zero if they are not identical.
//
// Input:
//  IN CHAR8 *string1
// Pointer to a CHAR8 null-terminated string.
//
//  IN CHAR8 *string2
// Pointer to a CHAR8 null-terminated string.
//
// Output:
//  INT32 non-zero value if both string1 and string2 are not identical.
//  Otherwise, returns 0 if both string1 and string 2 are identical.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
int Strcmp( char *string1, char *string2 ) {
	while(*string1 && *string1==*string2) {string1++;string2++;}
	return *string1 - *string2;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Wcscmp
//
// Description:
//  INT32 Wcscmp(IN CHAR16 *string1, IN CHAR16 *string2) compares two CHAR16
// strings, string1 and string2, and returns 0 if they are identical,
// non-zero if they are not identical.
//
// Input:
//  IN CHAR16 *string1
//      Pointer to a CHAR16 null-terminated string.
//
//  IN CHAR16 *string2
//      Pointer to a CHAR16 null-terminated string.
//
// Output:
//  INT32 non-zero value if both string1 and string2 are not identical.
//  Otherwise, returns 0 if both string1 and string 2 are identical.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
int Wcscmp( CHAR16 *string1, CHAR16 *string2 ) {
	while(*string1 && *string1==*string2) {string1++;string2++;}
	return *string1 - *string2;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Atoi
//
// Description:
//  INT32 Atoi(IN CHAR8 *string) converts an ASCII string that represents an
// integer into an actual INT32 integer value and returns the result.
//
// Input:
//  IN CHAR8 *string
// Pointer to a CHAR8 string that represents an integer number.
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
int Atoi(char *string) {
	int value=0;
	while (*string>='0' && *string<='9') {
		value *=10;
		value += *string-'0';
		string++;
	}
	return value;
}

//*************************************************************************
//
// Name: ItoaHelper
//
// Description:
//  CHAR8* ItoaHelper(IN INT64 value, IN OUT CHAR8 *string, IN INT32 radix,
// IN BOOLEAN is_int64) is a helper function for ItoaEx and ItowEx which 
// converts an INT64 value into a CHAR8 ASCII representation in reverse
// order.
//
// Input:
//  IN INT64 value
// Value to be converted.
//
//  IN OUT CHAR8 *string
// Pointer to a CHAR8 string buffer that will hold the result of the reverse
// conversion.  User is responsible for allocating the necessary memory
// resources.
//
//  IN INT32 radix
// Radix of the conversion.
//
//  IN BOOLEAN is_int64
// TRUE if value is a 64-bit integer; FALSE if it's 32-bit.
//
// Output:
//  CHAR8* string pointer to the last character added to the input string, 
// which now contains an ASCII string that is the reverse of what value 
// represents.
//
// Modified:
//
// Referrals:
//  Div64
// 
// Notes:	
// 
//*************************************************************************
char * ItoaHelper(INT64 value, char *string,int radix, BOOLEAN is_int64) {
	UINTN digit;
	UINT64 v = (value>=0) 
			   ? (UINT64) value
			   : (radix==10) 
				 ? (UINT64)-value 
				 : (is_int64) 
				   ? (UINT64)value 
				   : (UINT32)value;
	if (v)
		while (v) {
            #ifdef MDE_CPU_ARM
		        v = DivU64x32Remainder(v,radix,&digit);
            #else
		        v = Div64(v,radix,&digit);
            #endif // MDE_CPU_ARM
			if (digit<0xa) *string=(char)(digit+'0');
			else *string=(char)(digit-0xa+'a');
			string++;
		}
	else *string++='0';
	if (radix==10 && value<0) *string++='-';
	*string--=0;
	return string;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ItoaEx
//
// Description:
//  CHAR8* ItoaEx(IN INT64 value, IN OUT CHAR8 *string, IN INT32 radix, 
// IN BOOLEAN is_int64) converts an INT64 value into a CHAR8 ASCII 
// representation and returns a pointer to the ASCII string.
//
// Input:
//  IN INT64 value
// Value that needs to be converted into an ASCII representation.
//
//  IN OUT CHAR8 *string
// Pointer to a CHAR8 string buffer which will contain the result of the
// conversion.  User is responsible for allocating the necessary memory
// resources.
//
//  IN INT32 radix
// Radix of the conversion.  For example, 10 for decimal, 16 for hexadecimal,
// etc.
//
//  IN BOOLEAN is_int64
// TRUE if value is a 64-bit integer; FALSE if it's 32-bit.
//
// Output:
//  CHAR8* string pointer, string, that contains the ASCII representation of
// value in the user requested radix.
//
// Modified:
//
// Referrals:
//  ItoaHelper
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
char * ItoaEx(INT64 value, char *string,int radix, BOOLEAN is_int64) {
	char *strp=string;
	char *str2p=ItoaHelper(value,strp,radix,is_int64);
	//reverse string
	while(strp<str2p) {
		char temp=*strp;
		*strp=*str2p;
		*str2p=temp;
		strp++;str2p--;
	}
	return string;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: ItowEx
//
// Description:
//  CHAR16* ItowEx(IN INT64 value, IN OUT CHAR16 *string, IN INT32 radix, 
// IN BOOLEAN is_int64) converts an INT64 value into a 16-bit Unicode 
// representation and returns a CHAR16 pointer to the string.
//
// Input:
//  IN INT64 value
// Value that needs to be converted into an ASCII representation.
//
//  IN OUT CHAR16 *string
// Pointer to a CHAR8 string buffer which will contain the result of the
// conversion.
//
//  IN INT32 radix
// Radix of the conversion.  For example, 10 for decimal, 16 for hexadecimal,
// etc.
//
//  IN BOOLEAN is_int64
// TRUE if value is a 64-bit integer; FALSE if it's 32-bit.
//
// Output:
//  CHAR16* string pointer, string, that contains the 16-bit Unicode
// representation of value in the user requested radix.
//
// Modified:
//
// Referrals:
//  ItoaHelper
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
CHAR16 * ItowEx(INT64 value, CHAR16 *string,int radix, BOOLEAN is_int64) {
	char s[0x100];
	CHAR16 *str = string;
	char *send=ItoaHelper(value,(char*)s,radix,is_int64);
	//convert to unicode
	while(send>=s) {
		*str++ = *send--;
	}
	*str=0;
	return string;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: StrtolEx
//
// Description:
//  INT64 StrtolEx(IN CHAR8 *nptr, OUT CHAR8 **endptr, IN INT32 base,
// IN INT32 increment) converts a CHAR8 string representation of a number
// into an INT64 representation.
//
// Input:
//  IN CHAR8 *nptr
// CHAR8 string to be converted into an INT64.
//
//  OUT CHAR8 **endptr
// CHAR8 string pointer to the final character read from nptr.
//
//  IN INT32 base
// The base of the string being passed in.  If 0, string will be parsed by
// defaults; preceding "0x" treated as hexadecimal, preceding "0" treated as
// octal, everything else treated as decimal.
//
//  IN INT32 increment
// The number of characters in between each number digit.  For example,
// "14131" would be 1; "1 4 1 3 1" would be 2.
//
// Output:
//  INT64 representation of the number in the ASCII nptr string using the
// provided base and increment amount.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
long StrtolEx(char *nptr,char **endptr,int base, int increment) {
	unsigned char overflow=false;
	char sign=1;
	char digit;
	long value=0;

	while(*nptr == ' ' || *nptr == '\t') nptr+=increment;
	if (*nptr == '\0') {*endptr=(char*)nptr;return 0;}

	if (*nptr == '-') {sign = -1;nptr+=increment;}
	if (*nptr == '+') nptr+=increment;

	if (!base) {
		base=10;
		if (*nptr == '0') {
			base=8;
			nptr+=increment;
			if ((*nptr&0xdf)=='X') {	//Check for 'x' or 'X'
				base=16;
				nptr+=increment;
			}
		}
	}
	while(true) {
		if (*nptr >= '0' && *nptr <='9') digit=*nptr-'0';
		else if ((*nptr&0xdf)>='A'&& (*nptr&0xdf)<='Z') digit = (*nptr&0xdf) - 'A' + 0xa;
		else break;
		if (digit >= base) break;
		value = value * base + digit;
		if (sign==1) {
			if ((unsigned) value >= (unsigned) 0x80000000) overflow=true;
		} else if ((unsigned) value > (unsigned) 0x80000000) overflow=true;
		nptr+=increment;
	}
	*endptr=(char*)nptr;
	if (overflow) {
		value=0x7fffffff;
		if (sign==-1) value++;
	}
	return value*sign;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Strupr
//
// Description:
//  CHAR8* Strupr(IN CHAR8 *str) converts a CHAR8 string to all uppercase
// characters.
//
// Input:
//  IN CHAR8 *str
// CHAR8 string to be converted to all uppercase.
//
// Output:
//  CHAR8* string pointer to the start of the modified string.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  Original string will be modified!
// 
//<AMI_PHDR_END>
//*************************************************************************
char * Strupr(char *str) {
	char *strptr=str;
	while (*strptr) {	//End of string?
		if (*strptr >=0x61 && *strptr<=0x7a) *strptr-=0x20;	//If char is lower case, convert to upper.
		strptr++;		//Next char
	}
	return str;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Wcsupr
//
// Description:
//  CHAR16* Wcsupr(IN CHAR16 *str) converts a CHAR16 string to all uppercase
// characters.
//
// Input:
//  IN CHAR16 *str
// CHAR16 string to be converted to all uppercase.
//
// Output:
//  CHAR16* string pointer to the start of the modified string.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  Original string will be modified!
// 
//<AMI_PHDR_END>
//*************************************************************************
CHAR16 * Wcsupr(CHAR16 *str) {
	CHAR16 *strptr=str;
	while (*strptr) {	//End of string?
		if (*strptr >=0x61 && *strptr<=0x7a) *strptr-=0x20;	//If char is lower case, convert to upper.
		strptr++;		//Next char
	}
	return str;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Strstr
//
// Description:
//  CHAR8* Strstr(IN CONST CHAR8 *string, IN CONST CHAR8 *strSearch) locates
// strSearch within string, and returns a pointer to the start of the located
// string.
//
// Input:
//  IN CONST CHAR8 *string
// The string to be searched through.
//
//  IN CONST CHAR8 *strSearch 
// The string to locate/find.
//
// Output:
//  CHAR8* string pointer to the start of the located string if strSearch is 
// located.  If not found, returns a pointer to the end of the input string.
// If strSearch is an empty null-terminated string, returns the start of
// string.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
char *Strstr(
   const char *string,
   const char *strSearch 
)
{
  char *StartPos = (char *)string;

  if (!*strSearch) return StartPos;

  while (*StartPos)
  {
    char *p = StartPos;
    char *q = (char *) strSearch;

    while (*q && *p == *q) {++p;++q;}
    if (!*q) return StartPos;
    ++StartPos;
  }
  return StartPos;
}

//*************************************************************************
//
// Name: Sprintf_va_list
//
// Description:
//  UINTN Sprintf_va_list(OUT CHAR8 *buffer, IN CHAR8 *format,
// IN va_list arg) produces a null-terminated ASCII string in the output
// buffer.  The ASCII string is produced by parsing the format string
// specified by format.  Arguments are pulled from the variable argument
// list, specified by arg, based on the contents of the format string.  The
// number of ASCII characters in the produced output buffer is returned, not
// including the null-terminator.
//
// Input:
//  buffer - Pointer to a null-terminated output ASCII string buffer.  
//  User is responsible for allocating the necessary memory resources!
//
//  BufferSize - Size of the buffer in bytes.
//
//  format - Pointer to a null-terminated format ASCII string.
//
//  arg - Marker for the variable argument list.
//
// Output:
//  UINTN - number of ASCII characters in the produced output buffer, not
//  including the null-terminator.
//
// Referrals:
//  va_arg
//  Strtol
//  Sprintf
//  Strupr
//  I64toa
//  Itoa
//  Strlen
// 
// Notes: Refer to Sprintf function for format string syntax.
//
//*************************************************************************
UINTN Sprintf_va_list( char *buffer, char *format, va_list arg )
{
    return Sprintf_s_va_list( buffer, 0, format, arg );
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Sprintf
//
// Description:
//  UINTN Sprintf(OUT CHAR8 *Buffer, IN CHAR8 *Format, IN ...) produces a
// null-terminated ASCII string in the output Buffer.  The ASCII string is
// produced by parsing the format string specified by Format.  Arguments are 
// pulled from the variable argument list based on the contents of the format
// string.  The number of ASCII characters in the produced output buffer is
// returned, not including the null-terminator. See notes for format string 
// information.
//
// Input:
//  OUT CHAR8 *Buffer
// Pointer to a null-terminated output ASCII string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN CHAR8 *Format
// Pointer to a null-terminated format ASCII string.
//
//  IN ...
// Variable argument list which provides the data/variables used within the
// format string.
//
// Output:
//  UINTN number of ASCII characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  va_start
//  Sprintf_va_list
//  va_end
// 
// Notes:
//  Objects inside the format string have the following syntax.
//   %[flags][width]type
//
//  *** [flags] ***
//  
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
// . Flag  .  Description
// .       . 
// . -     .  The field is left justified.  If flag is not specified, then 
// .       . the field is right justified.
// .       . 
// . space .  Prefix a space character to a number.  Only valid for types X, 
// .       . x, and d.
// .       .  
// . +     .  Prefix a plus character to a number.  Only valid for types X,
// .       . x, and d.  If both space and `+' are specified, then space is
// .       . ignored.
// .       .
// . 0     .  Pad with `0' characters to the left of a number.  Only valid
// .       . for types X, x, and d.
// .       .
// . L, l  .  The number being printed is a UINT64.  Only valid for types X,
// .       . x, and d.  If this flag is not specified, then the number being
// .       . printed is an int.
// .       .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
//  
//  NOTE
//   All invalid [flags] are ignored.
//
// *** [width] ***
//
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
// . Width  .  Description
// .        .
// . *      .  The width of the field is specified by a UINTN argument in the
// .        . argument list.
// .        .
// . Number .  The number specified as a decimal value represents the width of
// .        . the field.
// .        .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
//
// NOTE
//  If [width] is not specified, then a field width of 0 is assumed.
//
// *** type ***
// 
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
// . Type  .  Description
// .       .
// . %     .  Print a `%'.
// .       .
// . c     .  The argument is an ASCII character. 
// .       .
// . x     .  The argument is a hexadecimal number.  The characters used are 
// .       . 0..9 and a..f.  If the flag `l' is not specified, then the
// .       . argument is assumed to be an int.
// .       .
// . X     .  The argument is a hexadecimal number.  The characters used are 
// .       . 0..9 and A..F.  If the flag `l' is not specified, then the
// .       . argument is assumed to be an int.
// .       .
// . d     .  The argument is a decimal number.  If the flag `l' is not
// .       . specified, then the argument is assumed to be an int.
// .       .
// . i     .  The same as `d'.
// .       .
// . s     .  The argument is a pointer to null-terminated ASCII string.
// .       .
// . a     .  The same as `s'.
// .       .
// . S     .  The argument is a pointer to a null-terminated Unicode string.
// .       .
// . g     .  The argument is a pointer to a GUID structure.  The GUID is
// .       . printed in the format xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx.
// .       .
// . G     .  The argument is a pointer to a GUID structure.  The GUID is
// .       . printed in the format XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.
// .       .
// . r     .  The argument is an EFI_STATUS value.  This value is converted
// .       . to a string.
// .       .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
//
// NOTE
//  All invalid type characters are copied into the result string.
//
//<AMI_PHDR_END>
//*************************************************************************
UINTN Sprintf( char *Buffer, char *Format, ...)
{
    va_list	ArgList = va_start(ArgList,Format);
    UINTN Ret = Sprintf_va_list( Buffer, Format, ArgList );
    va_end(ArgList);
    return Ret;
}

char* GetStatusCodeString(EFI_STATUS Status){
    static char* EfiSuccess="EFI_SUCCESS";

    if (Status == 0) return EfiSuccess;
    if ((Status & EFI_ERROR_BIT) == 0 ){//warning
        if ((Status & PI_WARNING_BIT) == PI_WARNING_BIT ){//PI warning
            if (Status >= NUM_OF_PI_WARNINGS) return NULL;
            return PiWarningStatus[Status];
        }else{//UEFI warning
            if (Status > NUM_OF_EFI_WARNINGS) return NULL;
            return EfiWarningStatus[Status-1];
        }
    }else{//error
        UINTN Index = Status & ~(EFI_ERROR_BIT | PI_ERROR_BIT | OEM_ERROR_BIT);
        if ((Status & PI_ERROR_BIT) == PI_ERROR_BIT ){//PI error
            if (Index >= NUM_OF_PI_ERRORS) return NULL;
            return PiErrorStatus[Index];
        }else if ((Status & OEM_ERROR_BIT) == OEM_ERROR_BIT ){//OEM error
            if (Index > NUM_OF_AMI_ERRORS) return NULL;
            return AmiErrorStatus[Index-1];
        }else{//UEFI error
            if (Index > NUM_OF_EFI_ERRORS) return NULL;
            return EfiErrorStatus[Index-1];
        }

    }     
}

//*************************************************************************
//
// Name: Sprintf_s_va_list
//
// Description:
//  UINTN Sprintf_s_va_list(OUT CHAR8 *Buffer, IN UINTN BufferSize,
// IN CHAR8 *Format, IN va_list Marker) produces a null-terminated ASCII
// string in the output Buffer of size BufferSize.  The ASCII string is
// produced by parsing the format string specified by Format.  Arguments are
// pulled from the variable argument list, Marker, based on the contents of
// the format string.  The number of ASCII characters in the produced output
// buffer is returned, not including the null-terminator.
//
// Input:
//  OUT CHAR8 *Buffer
// Pointer to a null-terminated output ASCII string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN UINTN BufferSize
// The size, in bytes, of the output Buffer.
//
//  IN CHAR8 *Format
// Pointer to a null-terminated format ASCII string.
//
//  IN va_list Marker
// Marker for the variable argument list.
//
// Output:
//  UINTN number of ASCII characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  Sprintf_va_list
//  MemCpy
// 
// Notes:	
//  This is a helper function for Sprintf_s.  Refer to Sprintf function for
// format string syntax.
// 
//*************************************************************************
UINTN Sprintf_s_va_list(char *Buffer, UINTN BufferSize, char *Format, va_list Marker)
{
	char filler;
	int width;
	char numbuffer[32];
	UINTN strlength;
	char *strp = Format;
	char *buffp = Buffer;
	char *_strp, *str2p;
	int	radix;
    BOOLEAN its_int64;
    UINTN n;

    if (Buffer==NULL || Format==NULL) return -1;
    //If BuuferSize is 0, no size check required
    while(BufferSize!=1 && *strp) {
        if (*strp != '%'){
            *buffp++ = *strp++;
            BufferSize--;
            continue;
        }
        strp++; //skip %
        if (*strp=='%') {
            strp++;
            *buffp++ = '%';
            BufferSize--;
            continue;
        }
        
        filler = ' ';
    
        //Get flags
        if (*strp == '0') { 
            filler = '0'; 
            strp++;
        }

        //Get Width
        if (*strp == '*') { //width is supplied by argument.
            strp++;
            width = va_arg(Marker,int);
        } else {
            width = Strtol(strp,&_strp,10);	//If no width, then 0 returned.
            strp=_strp;
        }
    
        //Get type.
        if (*strp == 's' || *strp == 'a'/*to be compatible with Intel Print library*/) { //string
            char *str = va_arg(Marker,char *);
            while (*str) {
                if ( --BufferSize == 0 ) {
                    *buffp = 0; // null terminator
                    return buffp - Buffer;
                }
                *buffp++ = *str++;
            }
            ++strp;
            continue;
        }
        
        if (*strp == 'S') { // unicode string
            CHAR16 *str = va_arg(Marker,CHAR16 *);
            while (*str) {
                if ( --BufferSize == 0 ) {
                    *buffp = 0; // null terminator
                    return buffp - Buffer;
                }
                *buffp++ = (char)*str++;
            }
            ++strp;
            continue;
        }
        
        if (*strp == 'c') { //character
            *buffp++ = va_arg(Marker,char);
            ++strp;
            continue;
        }
        
        if ((*strp & 0xdf) == 'G') {     //'G' or 'g'
            EFI_GUID *guid = va_arg(Marker,EFI_GUID*);
            CHAR8 *origp = buffp;
            UINT32 Data32Guid;
            UINT16 Data16Guid1;
            UINT16 Data16Guid2;

            Data32Guid = ReadUnaligned32(&(guid->Data1));
            Data16Guid1 = ReadUnaligned16(&(guid->Data2));
            Data16Guid2 = ReadUnaligned16(&(guid->Data3));
            n = Sprintf_s( 
                buffp,
                BufferSize,
                "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                Data32Guid, Data16Guid1, Data16Guid2, guid->Data4[0],
                guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4],
                guid->Data4[5], guid->Data4[6], guid->Data4[7]
            );
            buffp += n;
            if (*strp == 'G') 
                Strupr(origp);
            BufferSize -= n;
            ++strp;
            continue;
        }
        
        if (*strp == 'r') {
            EFI_STATUS Status = va_arg(Marker,EFI_STATUS);
            char *StatusCodeString = GetStatusCodeString(Status);

            if (StatusCodeString==NULL)
                n = Sprintf_s( buffp, BufferSize, "%s(%X)", EfiUnknown, Status );
            else
                n = Sprintf_s( buffp, BufferSize, "%s", StatusCodeString );                

            buffp += n;
            BufferSize -= n;
            ++strp;
            continue;
        }
        
        if (*strp == 'l') { 
            strp++; 
            its_int64 = TRUE;
        } else 
            its_int64 = FALSE;
		//Aptio V: we are adding %p support to be compatible with EDKII code
        if (*strp == 'p' && sizeof(VOID*) == 8) {
       		its_int64 = 1;
        }
        if (*strp == 'd' || *strp == 'i') 
            radix = 10;
        else if ( (*strp & 0xdf) == 'X' || *strp == 'p' )
            radix = 16; //'X' or 'x' or 'p'
        else 
            continue;	//invalid *strp
        
        if (its_int64) 
            I64toa( va_arg(Marker,INT64), numbuffer, radix );
        else 
            Itoa( va_arg(Marker,int), numbuffer, radix );
        if (*strp == 'X' || *strp == 'p') 
            Strupr(numbuffer);
        
        strlength = Strlen(numbuffer);
        while ( strlength++ < (unsigned)width ) {
            if ( --BufferSize == 0 ) {
                *buffp = 0; // null terminator
                return buffp - Buffer;
            }
            *buffp++ = filler;
        }
        
        str2p = numbuffer;
        while (*str2p) {
            if ( --BufferSize == 0 ) {
                *buffp = 0; // null terminator
                return buffp - Buffer;
            }
            *buffp++ = *str2p++;
        }
        
        strp++;
    }
    
    *buffp = 0;
    return buffp - Buffer;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Sprintf_s
//
// Description:
//  UINTN Sprintf_s(OUT CHAR8 *Buffer, IN UINTN BufferSize, IN CHAR8 *Format,
// IN ...) produces a null-terminated ASCII string in the output Buffer of
// size BufferSize.  The ASCII string is produced by parsing the format
// string specified by Format.  Arguments are pulled from the variable
// argument list based on the contents of the format string.  The number of
// ASCII characters in the produced output buffer is returned, not including
// the null-terminator.
//
// Input:
//  OUT CHAR8 *Buffer
// Pointer to a null-terminated output ASCII string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN UINTN BufferSize
// The size, in bytes, of the output Buffer.
//
//  IN CHAR8 *Format
// Pointer to a null-terminated format ASCII string.
//
//  IN ...
// Variable argument list of data/variables used within the format string.
//
// Output:
//  UINTN number of ASCII characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  va_start
//  Sprintf_s_va_list
//  va_end
// 
// Notes:	
//  Refer to Sprintf function for format string syntax.
// 
//<AMI_PHDR_END>
//*************************************************************************
UINTN Sprintf_s( char *Buffer, UINTN BufferSize, char *Format, ... )
{
	va_list	ArgList = va_start(ArgList, Format);
	UINTN Ret = Sprintf_s_va_list( Buffer, BufferSize, Format, ArgList );
	va_end(ArgList);
	return Ret;
}

//*************************************************************************
//
// Name: Swprintf_va_list
//
// Description:
//  UINTN Swprintf_va_list(OUT CHAR16 *buffer, IN CHAR16 *format,
// IN va_list arg) produces a null-terminated Unicode string in the output
// buffer.  The Unicode string is produced by parsing the format string
// specified by format.  Arguments are pulled from the variable argument list
// specified by arg based on the contents of the format string.  The number 
// of Unicode characters in the produced output buffer is returned, not
// including the null-terminator.
//
// Input:
//  OUT CHAR16 *buffer
// Pointer to a null-terminated output Unicode string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN CHAR16 *format
// Pointer to a null-terminated format Unicode string.
//
//  IN va_list arg
// Marker for the variable argument list.
//
// Output:
//  UINTN number of Unicode characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  va_arg
//  Wcstol
//  Swprintf
//  Wcsupr
//  I64tow
//  Itow
//  Wcslen
// 
// Notes:	
//  Refer to Sprintf function for format string syntax.
// 
//*************************************************************************
UINTN Swprintf_va_list(CHAR16 *buffer, CHAR16 *format, va_list arg)
{
    return Swprintf_s_va_list( buffer, 0, format, arg );
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Swprintf
//
// Description:
//  UINTN Swprintf(OUT CHAR16 *Buffer, IN CHAR16 *Format, IN ...) produces a
// null-terminated Unicode string in the output Buffer. The Unicode string is
// produced by parsing the format string specified by Format.  Arguments are
// pulled from the variable argument list based on the contents of the format
// string.  The number of Unicode characters in the produced output buffer is
// returned, not including the null-terminator.  
//
// Input:
//  OUT CHAR16 *Buffer
// Pointer to a null-terminated output Unicode string buffer.  User is
// responsible for allocating the necssary memory resources!
//
//  IN CHAR16 *Format
// Pointer to a null-terminated format Unicode string.
//
//  IN ...
// Variable arguement list of data/variables used within the format string.
//
// Output:
//  UINTN number of Unicode characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  va_start
//  Swprintf_va_list
//  va_end
// 
// Notes:	
//  See Sprintf function for format string details.
//
//<AMI_PHDR_END>
//*************************************************************************
UINTN Swprintf(CHAR16 *Buffer, CHAR16 *Format, ...)
{
	va_list	ArgList = va_start(ArgList,Format);
	UINTN Ret = Swprintf_va_list(Buffer,Format,ArgList);
	va_end(ArgList);
	return Ret;
}

//*************************************************************************
//
// Name: Swprintf_s_va_list
//
// Description:
//  UINTN Swprintf_s_va_list(OUT CHAR16 *Buffer, IN UINTN BufferSize,
// IN CHAR16 *Format, IN va_list Marker) produces a null-terminated Unicode
// string in the output Buffer of size BufferSize.  The Unicode string is
// produced by parsing the format string specified by Format.  Arguments are
// pulled from the variable argument list based on the contents of the format
// string.  The number of Unicode characters in the produced output buffer is 
// returned, not including the null-terminator.
//
// Input:
//  OUT CHAR16 *Buffer
// Pointer to a null-terminated output Unicode string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN UINTN BufferSize
// The size, in bytes, of the output Buffer.
//
//  IN CHAR16 *Format
// Pointer to a null-terminated format Unicode string.
//
//  IN va_list Marker
// Marker for the variable argument list.
//
// Output:
//  UINTN number of ASCII characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  Swprintf_va_list
//  MemCpy
// 
// Notes:	
//  This is a helper function for Swprintf_s.  Refer to Sprintf function for
// format string syntax.
// 
//*************************************************************************
UINTN Swprintf_s_va_list(CHAR16 *Buffer, UINTN BufferSize, CHAR16 *Format, va_list Marker)
{
    CHAR16 filler;
    int width;
    CHAR16 numbuffer[32];
    UINTN strlength;
    CHAR16 *strp = Format;
    CHAR16 *buffp = Buffer;
    CHAR16 *_strp, *str2p;
    int radix;
    BOOLEAN its_int64;
    UINTN n;

    if (Buffer==NULL || Format==NULL) return -1;
    //If BuuferSize is 0, no size check required
    while(BufferSize!=1 && *strp) {
        if (*strp != '%'){
            *buffp++ = *strp++;
            BufferSize--;
            continue;
        }
        strp++; //skip %
        if (*strp=='%') {
            strp++;
            *buffp++ = '%';
            BufferSize--;
            continue;
        }
        
        filler = ' ';
    
        //Get flags
        if (*strp == '0') { 
            filler = '0'; 
            strp++;
        }

        //Get Width
        if (*strp == '*') { //width is supplied by argument.
            strp++;
            width = va_arg(Marker, int);
        } else {
            width = Wcstol( strp, &_strp, 10 );    //If no width, then 0 returned.
            strp=_strp;
        }
    
        //Get type.
        if (*strp == 's') { // unicode string
            CHAR16 *str = va_arg(Marker, CHAR16*);
            while (*str) {
                if ( --BufferSize == 0 ) {
                    *buffp = 0; // null terminator
                    return buffp - Buffer;
                }
                *buffp++ = *str++;
            }
            ++strp;
            continue;
        }

        if ( *strp == 'S' || *strp == 'a' /*to be compatible with Intel Print library*/ ) { //string
            char *str = va_arg(Marker, char *);
            while (*str) {
                if ( --BufferSize == 0 ) {
                    *buffp = 0; // null terminator
                    return buffp - Buffer;
                }
                *buffp++ = *str++;
            }
            ++strp;
            continue;
        }

        if (*strp == 'c') { //character
            *buffp++ = va_arg(Marker, CHAR16);
            ++strp;
            continue;
        }

        if ((*strp & 0xdf) == 'G') {     //'G' or 'g'
            EFI_GUID *guid = va_arg(Marker, EFI_GUID*);
            CHAR16 *origp = buffp;
            n = Swprintf_s(
                buffp, 
                BufferSize,
                L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                guid->Data1, guid->Data2, guid->Data3, guid->Data4[0],
                guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4],
                guid->Data4[5], guid->Data4[6], guid->Data4[7]
            );
            buffp += n;
            if (*strp == 'G') 
                Wcsupr(origp);
            BufferSize -= n;
            ++strp;
            continue;
        }

        if (*strp == 'r') {
            EFI_STATUS Status = va_arg(Marker, EFI_STATUS);
            char *StatusCodeString = GetStatusCodeString(Status);

            if (StatusCodeString==NULL)
                n = Swprintf_s( buffp, BufferSize, L"%S(%X)", EfiUnknown, Status );
            else
                n = Swprintf_s( buffp, BufferSize, L"%S", StatusCodeString );                

            buffp += n;
            BufferSize -= n;
            ++strp;
            continue;
        }

        if (*strp == 'l') { 
            strp++; 
            its_int64 = TRUE;
        } else 
            its_int64 = FALSE;
		//Aptio V: we are adding %p support to be compatible with EDKII code
        if (*strp == 'p' && sizeof(VOID*) == 8) {
       		its_int64 = 1;
        }    
        if (*strp == 'd' || *strp == 'i') 
            radix = 10;
        else if ((*strp & 0xdf) == 'X') 
            radix = 16;  //'X' or 'x'
        else 
            continue;    //invalid *strp

        if (its_int64) 
            I64tow( va_arg(Marker,INT64), numbuffer, radix );
        else 
            Itow( va_arg(Marker,int), numbuffer, radix );
        if (*strp == 'X' || *strp == 'p') 
            Wcsupr(numbuffer);

        strlength = Wcslen(numbuffer);
        while ( strlength++ < (unsigned)width ) {
            if ( --BufferSize == 0 ) {
                *buffp = 0; // null terminator
                return buffp - Buffer;
            }
            *buffp++ = filler;
        }

        str2p = numbuffer;
        while (*str2p) {
            if ( --BufferSize == 0 ) {
                *buffp = 0; // null terminator
                return buffp - Buffer;
            }
            *buffp++ = *str2p++;
        }
            
        strp++;
    }
    
    *buffp = 0;
    return buffp - Buffer;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: Swprintf_s
//
// Description:
//  UINTN Swprintf_s(OUT CHAR16 *Buffer, IN UINTN BufferSize,
// IN CHAR16 *Format, IN ...) produces a null-terminated Unicode string in
// the output Buffer of size BufferSize.  The Unicode string is produced by
// parsing the format string specified by Format.  Arguments are pulled from
// the variable argument list based on the contents of the format string.
// The number of Unicode characters in the produced output buffer is
// returned, not including the null-terminator.
//
// Input:
//  OUT CHAR16 *Buffer
// Pointer to a null-terminated output Unicode string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN UINTN BufferSize
// The size, in bytes, of the output Buffer.
//
//  IN CHAR16 *Format
// Pointer to a null-terminated format Unicode string.
//
//  IN ...
// Variable argument list of data/variables used within the format string.
//
// Output:
//  UINTN number of Unicode characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  va_start
//  Swprintf_s_va_list
//  va_end
// 
// Notes:	
//  Refer to Sprintf function for format string syntax.
// 
//<AMI_PHDR_END>
//*************************************************************************
UINTN Swprintf_s(CHAR16 *Buffer, UINTN BufferSize, CHAR16 *Format, ...)
{
	va_list	ArgList = va_start(ArgList,Format);
	UINTN Ret = Swprintf_s_va_list(Buffer,BufferSize,Format,ArgList);
	va_end(ArgList);
	return Ret;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
