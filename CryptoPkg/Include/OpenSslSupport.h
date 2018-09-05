/** @file
  Root include file to support building OpenSSL Crypto Library.

Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __OPEN_SSL_SUPPORT_H__
#define __OPEN_SSL_SUPPORT_H__

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>

#define MAX_STRING_SIZE  0x1000

//
// OpenSSL relies on explicit configuration for word size in crypto/bn,
// but we want it to be automatically inferred from the target. So we
// bypass what's in <openssl/opensslconf.h> for OPENSSL_SYS_UEFI, and
// define our own here.
//
#ifdef CONFIG_HEADER_BN_H
#error CONFIG_HEADER_BN_H already defined
#endif

#define CONFIG_HEADER_BN_H

#if defined(MDE_CPU_X64) || defined(MDE_CPU_AARCH64) || defined(MDE_CPU_IA64)
//
// With GCC we would normally use SIXTY_FOUR_BIT_LONG, but MSVC needs
// SIXTY_FOUR_BIT, because 'long' is 32-bit and only 'long long' is
// 64-bit. Since using 'long long' works fine on GCC too, just do that.
//
#define SIXTY_FOUR_BIT
#elif defined(MDE_CPU_IA32) || defined(MDE_CPU_ARM) || defined(MDE_CPU_EBC)
#define THIRTY_TWO_BIT
#else
#error Unknown target architecture
#endif

//
// File operations are not required for building Open SSL, 
// so FILE is mapped to VOID * to pass build
//
typedef VOID  *FILE;

//
// Map all va_xxxx elements to VA_xxx defined in MdePkg/Include/Base.h
//
#if !defined(__CC_ARM) // if va_list is not already defined
#define va_list   VA_LIST
#define va_arg    VA_ARG
#define va_start  VA_START
#define va_end    VA_END
#else // __CC_ARM
#define va_start(Marker, Parameter)   __va_start(Marker, Parameter)
#define va_arg(Marker, TYPE)          __va_arg(Marker, TYPE)
#define va_end(Marker)                ((void)0)
#endif

//
// #defines from EFI Application Toolkit required to buiild Open SSL
//

//
// AMI PORTING START - Include Error codes needed in files like crypto\bio\bss_file.c.

/* Supported address families. */
#define AF_UNSPEC	0
#define AF_INET		2	/* Internet IP Protocol 	*/


/* Control Messages */

#define SCM_RIGHTS		1

/* Socket types. */
#define SOCK_STREAM	1		/* stream (connection) socket	*/
#define SOCK_DGRAM	2		/* datagram (conn.less) socket	*/
#define SOCK_RAW	3		/* raw socket			*/
#define SOCK_RDM	4		/* reliably-delivered message	*/
#define SOCK_SEQPACKET	5		/* sequential packet socket	*/
#define SOCK_PACKET	10		/* linux specific way of	*/
					/* getting packets at the dev	*/
					/* level.  For writing rarp and	*/
					/* other similar things on the	*/
					/* user level.			*/

#define SOL_SOCKET  	0xffff
#define SO_ERROR 		0x1007
#define SO_KEEPALIVE 	0x0008
#define SO_TYPE 	 	0x1008
#define SO_REUSEADDR   	0x0004

#define PROTNUM_TCP   	6
#define PROTNUM_UDP   	17

#define IPPROTO_TCP     (PROTNUM_TCP)
#define IPPROTO_UDP     (PROTNUM_UDP)

#ifndef INADDR_ANY
# define INADDR_ANY	((unsigned long int) 0x00000000)
#endif /* INADDR_ANY */

#ifndef INADDR_LOOPBACK
# define INADDR_LOOPBACK	((unsigned long int) 0x7f000001)
#endif /* INADDR_LOOPBACK */

//
// #define ENOMEM       12               /* Cannot allocate memory */
// #define EINVAL       22               /* Invalid argument */
/*
 * Error codes - Taken from reference file bsd/sys/errno.h
 */

#define EPERM       1       /* Operation not permitted */
#define ENOENT      2       /* No such file or directory */
#define ESRCH       3       /* No such process */
#define EINTR       4       /* Interrupted system call */
#define EIO         5       /* Input/output error */
#define ENXIO       6       /* Device not configured */
#define E2BIG       7       /* Argument list too long */
#define ENOEXEC     8       /* Exec format error */
#define EBADF       9       /* Bad file descriptor */
#define ECHILD      10      /* No child processes */
#define EDEADLK     11      /* Resource deadlock avoided */
           /* 11 was EAGAIN */
#define ENOMEM      12      /* Cannot allocate memory */
#define EACCES      13      /* Permission denied */
#define EFAULT      14      /* Bad address */
#ifndef _POSIX_SOURCE
#define ENOTBLK     15      /* Block device required */
#endif
#define EBUSY       16      /* Device busy */
#define EEXIST      17      /* File exists */
#define EXDEV       18      /* Cross-device link */
#define ENODEV      19      /* Operation not supported by device */
#define ENOTDIR     20      /* Not a directory */
#define EISDIR      21      /* Is a directory */
#define EINVAL      22      /* Invalid argument */
#define ENFILE      23      /* Too many open files in system */
#define EMFILE      24      /* Too many open files */
#define ENOTTY      25      /* Inappropriate ioctl for device */
#ifndef _POSIX_SOURCE
#define ETXTBSY     26      /* Text file busy */
#endif
#define EFBIG       27      /* File too large */
#define ENOSPC      28      /* No space left on device */
#define ESPIPE      29      /* Illegal seek */
#define EROFS       30      /* Read-only file system */
#define EMLINK      31      /* Too many links */
#define EPIPE       32      /* Broken pipe */

/* math software */
#define EDOM        33      /* Numerical argument out of domain */
#define ERANGE      34      /* Result too large */

/* non-blocking and interrupt i/o */
#define EAGAIN      35      /* Resource temporarily unavailable */
#ifndef _POSIX_SOURCE
#define EWOULDBLOCK EAGAIN      /* Operation would block */
#define EINPROGRESS 36      /* Operation now in progress */
#define EALREADY    37      /* Operation already in progress */

/* ipc/network software -- argument errors */
#define ENOTSOCK    38      /* Socket operation on non-socket */
#define EDESTADDRREQ    39      /* Destination address required */
#define EMSGSIZE    40      /* Message too long */
#define EPROTOTYPE  41      /* Protocol wrong type for socket */
#define ENOPROTOOPT 42      /* Protocol not available */
#define EPROTONOSUPPORT 43      /* Protocol not supported */
#define ESOCKTNOSUPPORT 44      /* Socket type not supported */
#endif /* ! _POSIX_SOURCE */
#define ENOTSUP 45      /* Operation not supported */
#ifndef _POSIX_SOURCE
#define EOPNOTSUPP   ENOTSUP        /* Operation not supported */
#define EPFNOSUPPORT    46      /* Protocol family not supported */
#define EAFNOSUPPORT    47      /* Address family not supported by protocol family */
#define EADDRINUSE  48      /* Address already in use */
#define EADDRNOTAVAIL   49      /* Can't assign requested address */

/* ipc/network software -- operational errors */
#define ENETDOWN    50      /* Network is down */
#define ENETUNREACH 51      /* Network is unreachable */
#define ENETRESET   52      /* Network dropped connection on reset */
#define ECONNABORTED    53      /* Software caused connection abort */
#define ECONNRESET  54      /* Connection reset by peer */
#define ENOBUFS     55      /* No buffer space available */
#define EISCONN     56      /* Socket is already connected */
#define ENOTCONN    57      /* Socket is not connected */
#define ESHUTDOWN   58      /* Can't send after socket shutdown */
#define ETOOMANYREFS    59      /* Too many references: can't splice */
#define ETIMEDOUT   60      /* Operation timed out */
#define ECONNREFUSED    61      /* Connection refused */

#define ELOOP       62      /* Too many levels of symbolic links */
#endif /* _POSIX_SOURCE */
#define ENAMETOOLONG    63      /* File name too long */

/* should be rearranged */
#ifndef _POSIX_SOURCE
#define EHOSTDOWN   64      /* Host is down */
#define EHOSTUNREACH    65      /* No route to host */
#endif /* _POSIX_SOURCE */
#define ENOTEMPTY   66      /* Directory not empty */

/* quotas & mush */
#ifndef _POSIX_SOURCE
#define EPROCLIM    67      /* Too many processes */
#define EUSERS      68      /* Too many users */
#define EDQUOT      69      /* Disc quota exceeded */

/* Network File System */
#define ESTALE      70      /* Stale NFS file handle */
#define EREMOTE     71      /* Too many levels of remote in path */
#define EBADRPC     72      /* RPC struct is bad */
#define ERPCMISMATCH    73      /* RPC version wrong */
#define EPROGUNAVAIL    74      /* RPC prog. not avail */
#define EPROGMISMATCH   75      /* Program version wrong */
#define EPROCUNAVAIL    76      /* Bad procedure for program */
#endif /* _POSIX_SOURCE */

#define ENOLCK      77      /* No locks available */
#define ENOSYS      78      /* Function not implemented */

#ifndef _POSIX_SOURCE
#define EFTYPE      79      /* Inappropriate file type or format */
#define EAUTH       80      /* Authentication error */
#define ENEEDAUTH   81      /* Need authenticator */
#define EOVERFLOW   84      /* Value too large to be stored in data type */
#define ELAST       84      /* Must be equal largest errno */
#endif /* _POSIX_SOURCE */

/* Intelligent device errors */
#define EPWROFF     82  /* Device power is off */
#define EDEVERR     83  /* Device error, e.g. paper out */

/* Program loading errors */
#define EBADEXEC    85  /* Bad executable */
#define EBADARCH    86  /* Bad CPU type in executable */
#define ESHLIBVERS  87  /* Shared library version mismatch */
#define EBADMACHO   88  /* Malformed Macho file */
//
// AMI PORTING END - Include Error codes needed in files like crypto\bio\bss_file.c.
//

#define BUFSIZ       1024             /* size of buffer used by setbuf */
#define INT_MAX      2147483647       /* max value for an int */
#define INT_MIN      (-2147483647-1)  /* min value for an int */
#define LONG_MAX     2147483647L      /* max value for a long */
#define LONG_MIN     (-2147483647-1)  /* min value for a long */
#define ULONG_MAX    0xffffffff       /* max value for an unsigned long */
#define LOG_DAEMON   (3<<3)           /* system daemons */
#define LOG_EMERG    0                /* system is unusable */
#define LOG_ALERT    1                /* action must be taken immediately */
#define LOG_CRIT     2                /* critical conditions */
#define LOG_ERR      3                /* error conditions */
#define LOG_WARNING  4                /* warning conditions */
#define LOG_NOTICE   5                /* normal but significant condition */
#define LOG_INFO     6                /* informational */
#define LOG_DEBUG    7                /* debug-level messages */
#define LOG_PID      0x01             /* log the pid with each message */
#define LOG_CONS     0x02             /* log on the console if errors in sending */

//
// Macros from EFI Application Toolkit required to buiild Open SSL
//
/* The offsetof() macro calculates the offset of a structure member
   in its structure.  Unfortunately this cannot be written down
   portably, hence it is provided by a Standard C header file.
   For pre-Standard C compilers, here is a version that usually works
   (but watch out!): */
#define offsetof(type, member) OFFSET_OF (type, member)

//
// Basic types from EFI Application Toolkit required to buiild Open SSL
//
typedef UINTN          size_t;
typedef INTN           ssize_t;
typedef INT64          off_t;
typedef UINT16         mode_t;
typedef long           time_t;
typedef unsigned long  clock_t;
typedef UINT32         uid_t;
typedef UINT32         gid_t;
typedef UINT32         ino_t;
typedef UINT32         dev_t;
typedef UINT16         nlink_t;
typedef int            pid_t;
typedef void           *DIR;
typedef void           __sighandler_t (int);
typedef UINT32 		   in_addr_t; 

typedef INT8 int8_t;
typedef UINT8 uint8_t;
typedef INT16 int16_t;
typedef UINT16 uint16_t;
typedef INT32 int32_t;
typedef UINT32 uint32_t;
typedef INT64 int64_t;
typedef UINT64 uint64_t;

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;

//
// Structures from EFI Application Toolkit required to buiild Open SSL
//
struct tm {
  int   tm_sec;     /* seconds after the minute [0-60] */
  int   tm_min;     /* minutes after the hour [0-59] */
  int   tm_hour;    /* hours since midnight [0-23] */
  int   tm_mday;    /* day of the month [1-31] */
  int   tm_mon;     /* months since January [0-11] */
  int   tm_year;    /* years since 1900 */
  int   tm_wday;    /* days since Sunday [0-6] */
  int   tm_yday;    /* days since January 1 [0-365] */
  int   tm_isdst;   /* Daylight Savings Time flag */
  long  tm_gmtoff;  /* offset from CUT in seconds */
  char  *tm_zone;   /* timezone abbreviation */
};

struct timeval {
  long tv_sec;      /* time value, in seconds */
  long tv_usec;     /* time value, in microseconds */
};

struct dirent {
  UINT32  d_fileno;         /* file number of entry */
  UINT16  d_reclen;         /* length of this record */
  UINT8   d_type;           /* file type, see below */
  UINT8   d_namlen;         /* length of string in d_name */
  char    d_name[255 + 1];  /* name must be no longer than this */
};

struct stat {
  dev_t    st_dev;          /* inode's device */
  ino_t    st_ino;          /* inode's number */
  mode_t   st_mode;         /* inode protection mode */
  nlink_t  st_nlink;        /* number of hard links */
  uid_t    st_uid;          /* user ID of the file's owner */
  gid_t    st_gid;          /* group ID of the file's group */
  dev_t    st_rdev;         /* device type */
  time_t   st_atime;        /* time of last access */
  long     st_atimensec;    /* nsec of last access */
  time_t   st_mtime;        /* time of last data modification */
  long     st_mtimensec;    /* nsec of last data modification */
  time_t   st_ctime;        /* time of last file status change */
  long     st_ctimensec;    /* nsec of last file status change */
  off_t    st_size;         /* file size, in bytes */
  INT64    st_blocks;       /* blocks allocated for file */
  UINT32   st_blksize;      /* optimal blocksize for I/O */
  UINT32   st_flags;        /* user defined flags for file */
  UINT32   st_gen;          /* file generation number */
  INT32    st_lspare;
  INT64    st_qspare[2];
};

//AptioV Porting - for build error 1.0.2j >>>
struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};

struct sockaddr {
   unsigned short sa_family;
   char sa_data[14];
};

struct sockaddr_in{
   short sin_family;
   unsigned short sin_port;
   struct in_addr sin_addr;
   char sin_zero[8];
};

struct  hostent {
  char   *h_name;       /**< official name of host */
  char  **h_aliases;    /**< alias list */
  int     h_addrtype;   /**< host address type */
  int     h_length;     /**< length of address */
  char  **h_addr_list;  /**< list of addresses from name server */
};

struct  servent {
  char   *s_name;     /**< official service name */
  char  **s_aliases;  /**< alias list */
  int     s_port;     /**< port # */
  char   *s_proto;    /**< protocol to use */
};
//AptioV Porting - for build error 1.0.2j <<<

//
// Externs from EFI Application Toolkit required to buiild Open SSL
//
extern int errno;

//
// Function prototypes from EFI Application Toolkit required to buiild Open SSL
//
void           *malloc     (size_t);
void           *realloc    (void *, size_t);
void           free        (void *);
int            isdigit     (int);
int            isspace     (int);
int            tolower     (int);
int            isupper     (int);
int            isxdigit    (int);
int            isalnum     (int);
void           *memcpy     (void *, const void *, size_t);
void           *memset 	   (void *, int, size_t);
void           *memchr     (const void *, int, size_t);
int            memcmp      (const void *, const void *, size_t);
void           *memmove    (void *, const void *, size_t);
int            strcmp      (const char *, const char *);
int            strncmp     (const char *, const char *, size_t);
char           *strcpy     (char *, const char *);
char           *strncpy    (char *, const char *, size_t);
size_t         strlen      (const char *);
char           *strcat     (char *, const char *);
char           *strchr     (const char *, int);
int            strcasecmp  (const char *, const char *);
int            strncasecmp (const char *, const char *, size_t);
char           *strncpy    (char *, const char *, size_t);
int            strncmp     (const char *, const char *, size_t);
char           *strrchr    (const char *, int);
unsigned long  strtoul     (const char *, char **, int);
long           strtol      (const char *, char **, int);
int            printf      (const char *, ...);
int            sscanf      (const char *, const char *, ...);
int            open        (const char *, int, ...);
int            chmod       (const char *, mode_t);
int            stat        (const char *, struct stat *);
off_t          lseek       (int, off_t, int);
ssize_t        read        (int, void *, size_t);
ssize_t        write       (int, const void *, size_t);
int            close       (int);
FILE           *fopen      (const char *, const char *);
size_t         fread       (void *, size_t, size_t, FILE *);
size_t         fwrite      (const void *, size_t, size_t, FILE *);
char           *fgets      (char *, int, FILE *);
int            fputs       (const char *, FILE *);
int            fprintf     (FILE *, const char *, ...);
int            vfprintf    (FILE *, const char *, VA_LIST);
int            fflush      (FILE *);
int            fclose      (FILE *);
int       	   ferror  	   (FILE *Stream);
int       	   fseek   	   (FILE *Stream, long Offset, int Whence);
int       	   feof    	   (FILE *Stream);
long      	   ftell   	   (FILE *Stream);
DIR            *opendir    (const char *);
struct dirent  *readdir    (DIR *);
int            closedir    (DIR *);
void           openlog     (const char *, int, int);
void           closelog    (void);
void           syslog      (int, const char *, ...);
time_t         time        (time_t *);
struct tm      *localtime  (const time_t *);
struct tm      *gmtime     (const time_t *);
struct tm      *gmtime_r   (const time_t *, struct tm *);
uid_t          getuid      (void);
uid_t          geteuid     (void);
gid_t          getgid      (void);
gid_t          getegid     (void);
void           qsort       (void *, size_t, size_t, int (*)(const void *, const void *));
char           *getenv     (const char *);
void           exit        (int);
void           abort       (void);
int 		   gettimeofday (struct timeval *tp, void *ignore);
__sighandler_t *signal     (int, __sighandler_t *);

//
// Global variables from EFI Application Toolkit required to buiild Open SSL
//
extern FILE  *stderr;
extern FILE  *stdin;
extern FILE  *stdout;

//
// Macros that directly map functions to BaseLib, BaseMemoryLib, and DebugLib functions
//
#define memcpy(dest,source,count)         CopyMem(dest,source,(UINTN)(count))
#define memset(dest,ch,count)             SetMem(dest,(UINTN)(count),(UINT8)(ch))
#define memchr(buf,ch,count)              ScanMem8(buf,(UINTN)(count),(UINT8)ch)
#define memcmp(buf1,buf2,count)           (int)(CompareMem(buf1,buf2,(UINTN)(count)))
#define memmove(dest,source,count)        CopyMem(dest,source,(UINTN)(count))
#define strcmp                            AsciiStrCmp
#define strncmp(string1,string2,count)    (int)(AsciiStrnCmp(string1,string2,(UINTN)(count)))

/* AptioV - Porting , only updated MdeModulePkg support the below function
#define strcpy(strDest,strSource)         AsciiStrCpyS(strDest,MAX_STRING_SIZE,strSource)
#define strncpy(strDest,strSource,count)  AsciiStrnCpyS(strDest,MAX_STRING_SIZE,strSource,(UINTN)count)
#define strlen(str)                       (size_t)(AsciiStrnLenS(str,MAX_STRING_SIZE))
#define strcat(strDest,strSource)         AsciiStrCatS(strDest,MAX_STRING_SIZE,strSource)
*/
#define strcpy(strDest,strSource)         AsciiStrCpy(strDest,strSource)
#define strncpy(strDest,strSource,count)  AsciiStrnCpy(strDest,strSource,(UINTN)count)
#define strlen(str)                       (size_t)(AsciiStrLen(str))
#define strcat(strDest,strSource)         AsciiStrCat(strDest,strSource)

#define strchr(str,ch)                    ScanMem8((VOID *)(str),AsciiStrSize(str),(UINT8)ch)
#define abort()                           ASSERT (FALSE)
#define assert(expression)
#define localtime(timer)                  NULL
#define gmtime_r(timer,result)            (result = NULL)
#define gettimeofday(tvp,tz)              do { (tvp)->tv_sec = time(NULL); (tvp)->tv_usec = 0; } while (0)
#define atoi(nptr)                        AsciiStrDecimalToUintn(nptr)

#endif
