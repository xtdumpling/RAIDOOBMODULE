//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2015 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Brickland
//    Auditor:  Salmon Chen
//    Date:     Feb/04/2015
//
//***************************************************************************
//***************************************************************************
#include "token.h"
#include <AmiLib.h>

#if DYNAMIC_PCIEXBASE_SUPPORT
#include <DynamicPciBase.h>
#endif

#if (A7_MRC_DEBUG == 0)
   #define A7Printf_IPMI_SUPPORT 0
#else
   #define A7Printf_IPMI_SUPPORT 1
#endif

#if (A7_MRC_DEBUG == 1)
extern UINT64 ReadMsr (UINT32 Msr);
extern VOID WriteMsr(UINT32 Msr, UINT64 Value);
#endif
//;SameDiskDebugSampleCode Mon 06-14-2010 For support SPI Hyper-Terminal debug tool.>> [a]+
//#if A7_Command_SUPPORT
//#define IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER 1000
#define IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER 1000
//#define IPMI_KCS_COMMAND_PORT 0xCA4
//#define IPMI_KCS_DATA_PORT 0xCA5

#ifndef SB_RCBA
  #define SB_RCBA		0xfed1c000	// Default RCBA
#endif

#ifndef SPI_BAR_OFFSET
  #define SPI_BAR_OFFSET	0x3800					// SPI BAR offset (ICH10, PCH)
#endif

#ifndef SB_SPI_BASE_ADDRESS
  #define SB_SPI_BASE_ADDRESS	0xFE010000//(SB_RCBA + SPI_BAR_OFFSET)	// SPI BASE Address (ICH10, PCH)
#endif

#ifndef DEFAULT_PCI_BUS_NUMBER_PCH
#define DEFAULT_PCI_BUS_NUMBER_PCH          0
#endif

#ifndef PCI_DEVICE_NUMBER_PCH_SPI
#define PCI_DEVICE_NUMBER_PCH_SPI           31  
#endif

#ifndef PCI_FUNCTION_NUMBER_PCH_SPI
#define PCI_FUNCTION_NUMBER_PCH_SPI         5 
#endif

#ifndef R_PCH_SPI_BAR0
#define R_PCH_SPI_BAR0                      0x10
#endif

#ifndef PCI_COMMAND_OFFSET
#define PCI_COMMAND_OFFSET                  0x04
#endif

#ifndef Command_97_SUPPORT
//  #define Command_97_SUPPORT	TRUE				// Default set support 97 command.
  #define Command_97_SUPPORT	FALSE				// Default set do not support 97 command.
#endif

#ifndef U8
typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned long   U32;
#endif

//
// Completion code related definitions
//
#define COMPLETION_CODE_SUCCESS                     0x00
#define COMPLETION_CODE_DEVICE_SPECIFIC_START       0x01
#define COMPLETION_CODE_DEVICE_SPECIFIC_END         0x7E
#define COMPLETION_CODE_COMMAND_SPECIFIC_START      0x80
#define COMPLETION_CODE_COMMAND_SPECIFIC_END        0xBE

#define ERROR_COMPLETION_CODE(a)        !( (a == COMPLETION_CODE_SUCCESS) || \
                            ( (a >= COMPLETION_CODE_DEVICE_SPECIFIC_START) &&\
                                (a <= COMPLETION_CODE_DEVICE_SPECIFIC_END) )||\
                            ( (a >= COMPLETION_CODE_COMMAND_SPECIFIC_START) &&\
                                (a <= COMPLETION_CODE_COMMAND_SPECIFIC_END) ) )

typedef enum
{
	KcsIdleState,
	KcsReadState,
	KcsWriteState,
	KcsErrorState
}KCS_STATE;

UINT8	SoftErrorCount;
UINT8	TempData[100];

#if (A7_MRC_DEBUG == 1)
extern VOID MemCpy(VOID* pDestination, VOID* pSource, UINTN Count);
#endif
typedef char *A7Va_list;
#define A7PUTC(_c,_CharCount)   StrWhole[_CharCount]=_c; _CharCount++; 
#define A7INTSIZEOF(n)   	( (sizeof(n) + sizeof(U32) - 1) & ~(sizeof(U32) - 1) )
#define A7Va_start(ap,v)  	( ap = (A7Va_list)&v + A7INTSIZEOF(v) )
#define A7Isdigit(_c) 		( ((_c) >= '0') && ((_c) <= '9'))
#define A7Va_arg(ap,t)    	( *(t *)((ap += A7INTSIZEOF(t)) - A7INTSIZEOF(t)) )
#define A7GETMEM(addr,dataType) (*((dataType *)(addr))) 
#define A7SETMEM(addr,dtyp,dat) (*((dtyp *)(addr))=dat) 
#define A7SPIBAR(offset)	(SB_SPI_BASE_ADDRESS + offset)
#define A7GETMEM8(addr)		A7GETMEM(addr, U8)	
#define A7GETMEM16(addr)	A7GETMEM(addr, U16)	
#define A7GETMEM32(addr)	A7GETMEM(addr, U32)
#define A7SETMEM8(addr,dat)	A7SETMEM(addr, U8, dat)	
#define A7SETMEM32(addr,dat)	A7SETMEM(addr, U32, dat)	
#define	A7SETSPI8(offs, dat)	A7SETMEM8(A7SPIBAR(offs),dat);
#define	A7SETSPI32(offs, dat)	A7SETMEM32(A7SPIBAR(offs),dat);
#define SMC_PCIEX_BASE_ADDRESS	0xE0000000

U8 A7SpiGet8(U8 offs) {
	return(A7GETMEM8(A7SPIBAR(offs)));
}  

VOID JA7SETSPI8(U8 offs, U8 dat) {
	A7SETMEM8(A7SPIBAR(offs),dat);
}

//FUNCTION: Wait until SPI Cycle NOT In Progress (SCIP).
VOID A7WaitUntiSpiReady() {    
	while( (A7SpiGet8(0x04) & 0x20) == 0x20 );	// Wait until "Cycle in Progress" bit is 0
}

//FUNCTION: Wait until SPI Cycle Done.
VOID A7WaitUntilSpiCycleDone() {
	while( (A7SpiGet8(0x04) & 0x01) != 0x01 );	// Wait until "Flash Cycle Done" bit is 1
	A7SETSPI8(0x04, 0x01);					// Clear 'Cycle Done Status'.
}

//FUNCTION: Send a SPI-UART command
//Command : A7-Command-SO-SO					// Send a UART command and get two byte data (SO-SO)
//INPUT   : Command (SI-SI-SI)
//OUTPUT  : SO-SO
U16 A7Command(U32 cmd32) {
	U16 SoSo;

	A7WaitUntiSpiReady();					// Wait device not busy		
	A7SETSPI8(0xA6, 0x02);			        // OP Type : Read
	A7SETSPI32(0x08, cmd32);					// FADDR (decode SPI FADDR as three byte command)
	A7SETSPI8(0x07, 0x01);			        // Flash Data Byte count = 2
	A7SETMEM8 (A7SPIBAR(0x04), 0x01);		// Clear 'Flash Cycle Done'.
	A7SETSPI8(0x06, 0x0B);			        // 5A command( Read SFDP ) , Go!
	A7WaitUntilSpiCycleDone();					// Wait command completely
	SoSo = A7GETMEM16(A7SPIBAR(0x10));
	return( SoSo );
}

//FUNCTION: Check 'TETO Ready'
//Command : A7-00-00-00-00-x1 : TETO Busy
//          A7-00-00-00-00-x0 : TETO Ready
//RETURN  : 1 = Busy
//          0 = Ready
int A7ChkTetoReady() {
//alan	return( A7Command(0) & 0x0100 );	// 0x0100 : Busy, 0x0000 : Ready
  return 0;//alan
}

//FUNCTION: PUT TETO Data
//Command : A7-00-81-31-00-00 : Put TETO Data
int A7PutTetoData(U8 dat8) {
    	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI32(0x08, 0x008100 | dat8);				// FADDR (decode SPI FADDR as data)
	A7SETSPI8(0x06, 0x0B);			        // 5A command( Read SFDP ) , Go!
    	A7WaitUntilSpiCycleDone();					// Wait command completely
	return 0;
}

//FUNCTION: Sent TETO
//Command : A7-00-80-01-00-00 : Sent TETO
int A7SentTeto() {
    	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI32(0x08, 0x008001);				// FADDR (decode SPI FADDR as data)
	A7SETSPI8(0x06, 0x0B);			        // 5A command( Read SFDP ) , Go!
    	A7WaitUntilSpiCycleDone();					// Wait command completely
	return 0;
}

//FUNCTION: Get device ID 
//Command : A7-00-80-00-ID-ID 	; Command 'Get UART ID16'
//Comment : 25AA = ZC25U64/128 + STM32
//	    2525 = ZC25U64/128 only
// 	    10AA = SPI2UART cable
// 	    0000/FFFF = Real Flash or Not supported version
// 	    AA=ARM
// 	    25=ROMTER
// 	    10=Cable
//RETURN  : Device ID of A7
U16 A7GetDeviceId() {
//alan	return( A7Command(0x008000) );				// send command 00-80-00 to get Device ID of A7.
	return( A7Command(0x008001) );				// send command 00-80-00 to get Device ID of A7.   //alan
}

//FUNCTION: Check Samedisk device existed or not
//RETURN  : TRUE  = Existed.
//          FALSE = Not existed.
BOOLEAN A7DeviceExisted() {
	U8 deviceId8;
//alan	deviceId8 = (U8) (A7GetDeviceId() >> 8);
	deviceId8 = (U8) A7GetDeviceId();//alan
	return( (deviceId8 == 0xAA) || (deviceId8 == 0x25)  ); 		// AA=ARM, 25=ROMTER that supports A7 command
}

//FUNCTION: Check Samedisk Emulator device existed or not
//RETURN  : TRUE  = Existed.
//          FALSE = Not existed.
BOOLEAN EmuDeviceExisted() {
	U8 deviceId8;
//alan	deviceId8 = (U8) (A7GetDeviceId() >> 8);
	deviceId8 = (U8) A7GetDeviceId();//alan	
	return( deviceId8 == 0x25); 		// 25=ROMTER that supports A7 command
}

//FUNCTION: Send a character to show on Hyper-Terminal.
VOID A7PutChar(UINT8 CharData) {
        while( (A7ChkTetoReady()) != 0);					// Wait until TETO Ready
	A7PutTetoData(CharData);					// PUT TETO Data
	A7SentTeto();						// Sent TETO
}

//FUNCTION: Print a string with specified length.
int A7SendStrLen(UINT8   *String, UINT8 len) {
    UINT8	i;
    
    for(i=0; i<len-1; i++) {						// Skip End-Of-String (ASCIIZ) character.            
        if (*String=='\n') A7PutChar('\r');					// replace "\n" with "\r\n"
        A7PutChar(*String);
        String++;
    }   

    return 0;
}

//FUNCTION: Send a string.
int A7SendString(char *String) {
    UINT8       *Buffer;
    UINT8       len=0;

    Buffer = String;	
    while (*Buffer) {
	len++;
        if (*Buffer=='\n') {
		A7SendStrLen(Buffer-len+1, len+1);
		len = 0;
	}
	Buffer++;
    }	
    A7SendStrLen(Buffer-len, len+1);

    return 0;
}

//FUNCTION: transfer string.
void A7UintnToStr(U32 Value, char *Str, U32 Width, U32 Flags, U32 Base) {
   U32 Negative;
   U32 Int;
   char *Ptr;
   char Prefix;
   char c;
   U32 i;
   const char Hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

   Ptr = Str;
   if ( (Value > 0x7fffffffU) && (Flags & 0x20) ) {
      Int = ~Value + 1;  /* -Value */
      Negative = 1;
   } else {
      Int = Value;
      Negative = 0;
   }

   i = 0;
   do {           /* generate digits in reverse order */
      i++;
      *Ptr++ = Hex[Int % Base];
      if ( Flags & 0x40 ) {
         if ( Base == 16 ) {
            if ( i % 4 == 0 ) {
               *Ptr++ = ',';
            }
         } else if ( Base == 10 ) {
            if ( i % 3 == 0 ) {
               *Ptr++ = ',';
            }
         }
      }
   } while ((Int /= Base) > 0);
   if ( *(Ptr-1) == ',') {
      Ptr--;
   }
   if (Negative) {
      *Ptr++ = '-';
   } else if ( Flags & 0x02 ) {
      *Ptr++ = '+';
   }
   if ( Flags & 0x08 ) {
      Prefix = '0';
   } else if ( !(Flags & 0x01)) {
      Prefix = ' ';
   } else {
      Prefix = 0x00;
   }
   if (Prefix != 0x00) {
      for (i = (int)(Ptr - Str); i < Width; i++) {
	  	
         //*Ptr++ = Prefix;
		 if(Prefix == '0')
		 	*Ptr++ = '0';
		 if(Prefix == ' ')
		 	*Ptr++ = ' ';
		 
      }
   }
   *Ptr = '\0';

   /* reverse string */
   while (Str < --Ptr) {
      c = *Str;
      *Str++ = *Ptr;
      *Ptr = c;
   }
}

//FUNCTION: transfer string to number.
U32 A7StrToNumber(char **String) {
   U32  Sum;
   char *Str;

   Str = *String;
   if (*Str == '0') {
      Str++;
   }
   Sum = 0;
   while (A7Isdigit(*Str)) {
      Sum = Sum*10 + (*Str++ - '0');
   }
   *String = Str;
   return (Sum);
}

//FUNCTION: printf command.
int A7Vprintf(const char *Format, A7Va_list Marker) {
   char *p;
   U32 Width;
   U32 Flags;
   char Str[160], StrWhole[255];
   char *String;
   U32 CharCount;

   for (p = (char *)Format, CharCount = 0; *p; p++) {	
      if ( *p != '%' ) {
         A7PUTC(*p, CharCount);
      } else {
         p++;
         // Check for flags
         Flags = 0;
         if ( *p == '-') {
            Flags |= 0x01;
         } else if ( *p == '+' ) {
            Flags |= 0x02;
         } else if ( *p == ' ' ) {
            Flags |= 0x04;
         }
         if (Flags != 0) {
            p++;
         }

         // Check for width
         if ( A7Isdigit(*p) ) {
            if ( *p == '0' ) {
               Flags |= 0x08;
            }
            Width = A7StrToNumber(&p);
         } else if ( *p == '*' ) {
            Width = A7Va_arg( Marker, int );
            p++;
         } else {
            Width = 0;
         }

         if ( *p == ',' ) {
            Flags |= 0x40;
            p++;
         }

         // get type
         String = Str;
         switch (*p) {
         case 'd':
         case 'i':
                // always print as UINTN will need extra code to print different widths
            	A7UintnToStr ((U32)A7Va_arg( Marker, U32 *),Str , Width, Flags | 0x20, 10);
         break;
         case 'u':
                // always print as UINTN will need extra code to print different widths
            	A7UintnToStr ((U32)A7Va_arg( Marker, U32 *),Str , Width, Flags, 10);
         break;
         case 'x':
         case 'X':
                // always print as UINTN will need extra code to print different widths
	    	A7UintnToStr ((U32)A7Va_arg( Marker, U32 *),Str , Width, Flags, 16);
         break;
         case 'c':
            	A7PUTC (A7Va_arg (Marker, char), CharCount);
            	Str[0] = '\0';
         	break;
         case 's':
            	String = (char *)A7Va_arg (Marker, char *);
         	break;
         case 'p':
            	A7UintnToStr ((U32)A7Va_arg( Marker, U32 *),Str , Width, Flags, 16);
        	 break;
         }
	 
         while (*String != '\0') {
            	A7PUTC (*String++, CharCount);
         }
      }
   }
   StrWhole[CharCount] = 0; 	// End of ASCIIZ
   A7SendString(StrWhole); 		
   return CharCount;
}
  
UINTN
A7_MmPciBase (
  IN UINT32                       Bus,
  IN UINT32                       Device,
  IN UINT32                       Function
  )
{
  return ((UINTN) (SMC_PCIEX_BASE_ADDRESS) + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) (Function << 12));
}


VOID A7_InitSpiBar(void)
{
      U8     data;
      U32    data32;
      UINTN  PchSpiBase; 

      PchSpiBase = A7_MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SPI,
                      PCI_FUNCTION_NUMBER_PCH_SPI
                   );        

      data32 = A7GETMEM32(PchSpiBase + R_PCH_SPI_BAR0);
      if( data32 != SB_SPI_BASE_ADDRESS )
      {	      
	      A7SETMEM32(PchSpiBase + R_PCH_SPI_BAR0, SB_SPI_BASE_ADDRESS);          
	      data = A7GETMEM8(PchSpiBase + PCI_COMMAND_OFFSET);                     
	      data = data | 0x02;     //EFI_PCI_COMMAND_MEMORY_SPACE                    
	      A7SETMEM8(PchSpiBase + PCI_COMMAND_OFFSET, data);  
      }		      
}
 
//FUNCTION: Samedisk A7 printf() - send a string to Hyper-Terminal by A7 device that supports A7 command.
//USAGE   : a7printf( <Format String>, <Argument 1>, [Argument 2, 3, ...] )
//EXAMPLE : Show i value with a string --> a7printf("i = %x\n", i);	
VOID a7printf(const char *Format, ...) {   

  A7_InitSpiBar();   
                     
	A7Command(0x000003);                   
	A7Command(0x000025);               
	A7Command(0x000002);
        
	if(A7DeviceExisted()) {					// if A7 device existed
	   	A7Va_list  Marker;
	
		A7Va_start(Marker, Format);
		A7Vprintf (Format, Marker);
} 
}

//FUNCTION: Print debug message
//INPUT   : IN EFI_PEI_SERVICES *PeiServices 
//	  : IN CHAR8 *String
//OUTPUT  : None	
VOID a7printf_message(IN VOID *PeiServices, IN CHAR8 *String) 
{   
		a7printf("%s", String);
} 


// Pause and Get key {
//============================================================================================ a7getch() / a7pause()
//FUNCTION: Get TERMINAL STAT
//Command : A7-00-00-00-SO-SO : TERMINAL STAT
//RETURN  : Terminal state (SO-SO)
U16 A7GetState() {
	return(A7Command(0));					// SI-SI-SI : 00-00-00
}

//FUNCTION: TEKI Request
//Command : A7-00-00-00-00-1x : TERMINAL STAT (TEKI Request)
//RETURN  : 0     = No data (key) in
//          1000h = Has data (key) in
int A7GetTeki() {
	return(A7GetState() & 0x1000);
}

//FUNCTION: Send TEKI Acknowledge
//Command : A7-00-80-10-00-00 : Send TEKI Acknowledge
VOID A7TekiAck() {
	A7Command(0x008010);					// send command 00-80-10 (TEKI Acknowledge)
}

//FUNCTION: Get TERM Data
//Command : A7-00-01-00-00-Data : Get TERM Data
//RETURN  : Received data byte (-SO)
U8 A7GetTermData8() {
	U16 dat16;
	dat16 = A7Command(0x000100);				// Get Terminal Data
	A7TekiAck();						// send command 00-80-10 (TEKI Acknowledge)
	return( (U8) (dat16 >> 8) );
}

//FUNCTION: C-like getch() - Wait and get a key from SUART.
//INPUT   : None.
//RETURN  : Pressed key code.
U8 a7getch() {
	while(!A7GetTeki());					// wait until any key pressed.
	return( A7GetTermData8() );					// return pressed key code.
}

//FUNCTION: Pause system and wait until user pressed [Space] key on PuTTY Hyper-Terminal.
//INPUT   : None.
//OUTPUT  : Pressed key code.
U8 a7pause() {
	U8 ch, ch2;
	while(1) {
		a7printf("\n* a7pause: Press [Space] key to continue... ");
		ch=a7getch();					// wait and get a key
		ch2=ch;
		// special characters
		if(ch >= 8 && ch <= 13)				// if [TAB] and [Enter]
			ch2 = '.';					// show '.'
		else if(ch == 27) 					// if [ESC] key
			ch2 = '[';					// show '['
		// check and show key information.
		if(ch != 0x20) {					// if not [Space] key
			a7printf("[%c] ", ch2);
			a7printf("Keycode %02Xh, not [Space] *", ch);
		} else {
			a7printf("Continue! *\n");
			break;
		} // if()
	} // while()
	return(ch);
}
// Pause and Get key }

//FUNCTION: Send 86 command for port 80h
//Command : SPI86Command
//INPUT   : Port 80 data
//
int SPI86Command(U8 cmd8) {
    if(A7DeviceExisted()) {						// if A7 device existed
	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI8(0x9E, 0x86);					// OPMENU index 6 Code (86 command)
	A7SETSPI8(0x97, 0xA4);					// OP Type : Set Index 6 & 7 for Read
	A7SETSPI8(0x08, 0x00);
	A7SETSPI8(0x09, cmd8);
	A7SETSPI8(0x0A, 0x00);
    	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
    	A7WaitUntilSpiCycleDone();					// Wait command completely
	A7SETSPI8(0x9E, 0x86);					// OPMENU index 6 Code (86 command)
	A7SETSPI8(0x97, 0xA4);					// OP Type : Set Index 6 & 7 for Read
	A7SETSPI8(0x08, 0x00);
	A7SETSPI8(0x09, cmd8);
	A7SETSPI8(0x0A, 0x01);
    	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
	A7WaitUntilSpiCycleDone();					// Wait command completely
    }
       	return(0 );
}

//FUNCTION: Send 86 command for port 80h
//Command : SPI86Command2
//INPUT   : Port 80 data
int SPI86Command2(U8 cmd8) {
    if(A7DeviceExisted()) {						// if A7 device existed
	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI8(0x9E, 0x86);					// OPMENU index 6 Code (86 command)
	A7SETSPI8(0x97, 0xA4);					// OP Type : Set Index 6 & 7 for Read
	A7SETSPI8(0x08, 0x00);
	A7SETSPI8(0x09, cmd8);
	A7SETSPI8(0x0A, 0x02);
    	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
    	A7WaitUntilSpiCycleDone();					// Wait command completely
    	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
    	A7WaitUntilSpiCycleDone();					// Wait command completely
    	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
    	A7WaitUntilSpiCycleDone();					// Wait command completely
    	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
	A7WaitUntilSpiCycleDone();					// Wait command completely
    }
       	return(0 );
}


//FUNCTION: Send 86 and A7printf command for port 80h
//Command : OUTSPI80
//INPUT   : Port 80 data
VOID OUTSPI80(UINT8 g) {
    if(A7DeviceExisted()) {						// if A7 device existed
    SPI86Command(g);						// Send 86 command with port 80 data.
    a7printf("Postcode %02Xh**\n", g);					// Send A7printf command with port 80 data.
//    a7printf("*\n");
    }
}

//FUNCTION: Send "5EC0" on 86 command or "Power on" string to A7printf command for clear samdebug logscreen.
//Command : SPIPowerONInit
//INPUT   : None.
VOID SPIPowerONInit() {
    if(A7DeviceExisted()) {						// if A7 device existed
	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI8(0x9E, 0x86);					// Set OPMENU index 6 Code (86 command)
	A7SETSPI8(0x97, 0xA4);					// OP Type : Set Index 6 & 7 for Read
	A7SETSPI8(0x08, 0x00);
	A7SETSPI8(0x09, 0x5E);
	A7SETSPI8(0x0A, 0x00);
	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
	A7WaitUntilSpiCycleDone();					// Wait command completely
	A7SETSPI8(0x9E, 0x86);					// Set OPMENU index 6 Code (86 command)
	A7SETSPI8(0x97, 0xA4);					// OP Type : Set Index 6 & 7 for Read
	A7SETSPI8(0x08, 0x00);
	A7SETSPI8(0x09, 0xC0);
	A7SETSPI8(0x0A, 0x01);
	A7SETMEM8 (A7SPIBAR(0x90), 0x04);			// Clear 'Cycle Done Status'.
	A7SETSPI8(0x91, 0x62);					// Go! Selece OPMenu index 6.
	A7WaitUntilSpiCycleDone();					// Wait command completely
	a7printf("*Power on*\n");
    }
}

//============================================================================================ 97 Command
//FUNCTION: Read data from Emulator.
//Command : CM-SI-SI-SI-SO-SO-...
//          97-Offset  -Data
//INPUT   : Offset 0x00-0x7F --> Point to offset 0x300-0x3FF of Emulator DEBUG.BIN.
//OUTPUT  : HSPIBAR(0x11) --- DEBUG.BIN address (0x300 + Offset)
void hugetbios(unsigned char Offset, int len) {
	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI8(0x90,0x8C);					// Clear status.
	A7SETSPI8(0x9F,0x97);					// Set OPMENU index 7 Code
	A7SETSPI8(0x97, 0xA4);					// OP Type : Set Index 6 & 7 for Read
	A7SETSPI32(0x08, Offset);					// FADDR (decode SPI FADDR as three byte command)
	A7SETSPI8(0x92, (0x40+len))				// Byte count (one dummy at HSPIBAR(0x10))
	A7SETSPI8(0x91, 0x72);					// Go! Selece OPMenu index 7.
	A7WaitUntilSpiCycleDone();					// Wait command completely
}

//============================================================================================ A7 Command for get and set 97 buffer.
//FUNCTION: Read data from Emulator.
//Command : A7-03-xx-Addr(8)-xx-DD-...
//          A7-Offset  -len
//INPUT   : Offset 0x00-0x7F --> Point to offset 0x300-0x3FF of Emulator DEBUG.BIN.
//OUTPUT  : HSPIBAR(0x11) --- DEBUG.BIN address (0x300 + Offset)
void A7hugetbios(unsigned char Offset, int len) {
	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI8(0x04, 0x01);              // Clear 'Flash Cycle Done'.
	A7SETSPI8(0xA6, 0x02);              // OP Type : Read   
	A7SETSPI8(0x08, Offset);					// The buffer offset of 0x97 buffer want to get.
	A7SETSPI8(0x09, 0x00);					// dummy data
	A7SETSPI8(0x0A, 0x03);					// 0x03 is command for get data to 0x97 buffer.
	A7SETSPI8(0x07, len);               // Byte count (one dummy at HSPIBAR(0x10)) 
	A7SETSPI8(0x04, 0x01);              // Clear 'Flash Cycle Done'.    
	A7SETSPI8(0x06, 0x0B);              // 5A command( Read SFDP ) , Go!  
	A7WaitUntilSpiCycleDone();					// Wait command completely
}

//FUNCTION: Set data from Emulator.
//Command : A7-02-Addr(8)-Datain(8)-...
//          A7-Offset  -Data
//INPUT   : Offset 0x00-0x7F --> Point to offset 0x300-0x3FF of Emulator DEBUG.BIN.
//OUTPUT  : HSPIBAR(0x11) --- DEBUG.BIN address (0x300 + Offset)
void A7husetbios(unsigned char Offset, int Data) {
	A7WaitUntiSpiReady();					// Wait device not busy
	A7SETSPI8(0x04, 0x01);              // Clear 'Flash Cycle Done'.
	A7SETSPI8(0xA6, 0x02);              // OP Type : Read  
	A7SETSPI8(0x08, Data);					// The data for set to 0x97 buffer.
	A7SETSPI8(0x09, Offset);					// The buffer offset of 0x97 buffer want to set.
	A7SETSPI8(0x0A, 0x02);					// 0x02 is command for set data to 0x97 buffer.
	A7SETSPI8(0x07, 0x01);              // Flash Data Byte count = 2  
	A7SETSPI8(0x04, 0x01);              // Clear 'Flash Cycle Done'.
	A7SETSPI8(0x06, 0x0B);              // 5A command( Read SFDP ) , Go!  
	A7WaitUntilSpiCycleDone();					// Wait command completely
}

//FUNCTION: Read data from Emulator.
//Command : CM-SI-SI-SI-SO
//          97-Offset  -Data
//INPUT   : Offset 0x00-0x7F --> Point to offset 0x300-0x3FF of Emulator DEBUG.BIN.
//OUTPUT  : Data byte of DEBUG.BIN address (0x300 + Offset)
//Read 8bit data from Emulator.
unsigned char hugetbios8(unsigned char Offset) {				// Get 8bit data form offset 0x300 ~ 0x3FF Emulator DEBUG.BIN.
	unsigned char data8;
	A7hugetbios(Offset, 1);					// read 1 byte
//alan	data8=A7GETMEM8(A7SPIBAR(0x11));			// read the first byte after dummy character.
	data8=A7GETMEM8(A7SPIBAR(0x10));			// read the first byte after dummy character. //alan
	return( data8 );						// Return one data byte
}

//FUNCTION: Read 16bit data from Emulator.
U16 hugetbios16(unsigned char Offset) {				// Get 16bit data form offset 0x300 ~ 0x3FF Emulator DEBUG.BIN.
	U16 data16;
	A7hugetbios(Offset, 2);					// read 2 byte
	//alan data16=A7GETMEM16(A7SPIBAR(0x11));			// read the first byte after dummy character.
	data16=A7GETMEM16(A7SPIBAR(0x10));			// read the first byte after dummy character. //alan  
	return( data16 );						// Return one data byte
}

//FUNCTION: Read 32bit data from Emulator.
U32 hugetbios32(unsigned char Offset) {				// Get 32bit data form offset 0x300 ~ 0x3FF Emulator DEBUG.BIN.
	U32 data32;
	U16 data16;
	A7hugetbios(Offset+2, 2);					// read 2 byte
//alan	data16=A7GETMEM16(A7SPIBAR(0x11));			// read the first byte after dummy character.
	data16=A7GETMEM16(A7SPIBAR(0x10));			// read the first byte after dummy character. //alan 
	data32= (U32) (data16<<16);					//
	A7hugetbios(Offset, 2);					// read 2 byte
	//alan data16=A7GETMEM16(A7SPIBAR(0x11));			// read the first byte after dummy character.
	data16=A7GETMEM16(A7SPIBAR(0x10));			// read the first byte after dummy character. //alan  
	data32=data32|data16;					//
	return( data32 );						// Return one data byte
}
//============================================================================================

//----------------------------------------------------------------------------- PCI & PCIe Address>>
U32 SDPcieAddr(U8 bus, U8 device, U8 function, U16 reg) {
#if DYNAMIC_PCIEXBASE_SUPPORT
    return((U32)(GetPciBaseAddr() | (bus << 20) | (device << 15) | (function << 12) | reg));
#else
    return((U32)(SMC_PCIEX_BASE_ADDRESS | (bus << 20) | (device << 15) | (function << 12) | reg));
#endif
}
#if DYNAMIC_PCIEXBASE_SUPPORT
#define SDSetPcieAddr(bus, device, function, reg, Data) (*(volatile UINT8*)(UINTN)(GetPciBaseAddr() | (bus << 20) | (device << 15) | (function << 12) | reg)= Data)
#else
#define SDSetPcieAddr(bus, device, function, reg, Data) (*(volatile UINT8*)(UINTN)(SMC_PCIEX_BASE_ADDRESS | (bus << 20) | (device << 15) | (function << 12) | reg)= Data)
#endif

U32 SDReadPciDword(U8 bus, U8 device, U8 function, U16 reg)
{
//     U32 data;
//
//     regAddr = (U8 *) SDPcieAddr(bus, device, function, reg );
//     data = *(volatile U32 *)regAddr;
//     data = *(volatile U32 *)SDPcieAddr(bus, device, function, reg );
//     (Under construction...)

   return(*(volatile U32 *)SDPcieAddr(bus, device, function, reg ));
}

U16 SDReadPciWord(U8 bus, U8 device, U8 function, U16 reg)
{
   return(*(volatile U16 *)SDPcieAddr(bus, device, function, reg ));
}

U8 SDReadPciByte(U8 bus, U8 device, U8 function, U16 reg)
{
   return(*(volatile U8 *)SDPcieAddr(bus, device, function, reg ));
}

//FUNCTION: Read 256 bytes PCI/PCIe device registers.
VOID SDPcie(U8 Bus, U8 Device, U8 Function) {
	U32 Address;
	U16 i, j;
	a7printf("==================================================\n");
	a7printf("BUS : %04Xh / Device : %04Xh / Function : %04Xh\n", Bus, Device, Function);
	a7printf("VID : %04Xh / DID : %04Xh\n", SDReadPciWord(Bus, Device, Function,0), SDReadPciWord(Bus, Device, Function,2));
	// Read 256 bytes PCI registers
	a7printf("   00 01 02 03 04 05 06 07 08-09 0A 0B 0C 0D 0E 0F\n");
	for(i=0; i<0x10; i++) {
		a7printf("%02X ", i*0x10);
	        for(j=0; j<0x10; j++) {
	           if (j<0x0F) {
	            a7printf("%02X-", SDReadPciByte( (U8) Bus, (U8) Device, (U8) Function, (U16) (i*0x10+j) ));
	           } else {
	            a7printf("%02X", SDReadPciByte( (U8) Bus, (U8) Device, (U8) Function, (U16) (i*0x10+j) ));
	           }
	        } // for()
	        a7printf("\n");
	} //for()
	a7printf("==================================================\n");
#if DYNAMIC_PCIEXBASE_SUPPORT
	Address=(GetPciBaseAddr() | (Bus << 20) | (Device << 15) | (Function << 12));
#else
	Address=(SMC_PCIEX_BASE_ADDRESS | (Bus << 20) | (Device << 15) | (Function << 12));
#endif
	a7printf("%08Xh\n",Address);
} // SDPcie()

//FUNCTION: Set PCI Address data.
VOID SDSetPCI() {
	U8 Bus;
	U8 Dev;
	U8 Fun;
	U8 Reg;
	U8 Data;
	Bus=hugetbios8(0xC1);					// PCI BUS number.
	Dev=hugetbios8(0xC2);					// PCI Device number.
	Fun=hugetbios8(0xC3);					// PCI Function number.
	Reg=hugetbios8(0xC4);					// PCI Register number.
	a7printf("PCI BUS : %02Xh / Device : %02Xh / Function : %02Xh/ Register: %2Xh\n", Bus,Dev,Fun,Reg);
	Data=hugetbios8(0xC5);					// Data
	a7printf("Regsiter data set to %02Xh**\n", Data);
	SDSetPcieAddr(Bus, Dev, Fun, Reg, Data);
	SDPcie(Bus,Dev,Fun);
	A7husetbios(0xC0,0x01);
} //SDSetPCI()

//FUNCTION: Get PCI List.
VOID SDGetPCIList() {
	U8 Bus;
	U8 Dev;
	U8 Fun;
	for(Bus=0; Bus<=0x3F; Bus++) {
	        for(Dev=0; Dev<=0x1F; Dev++) {
		        for(Fun=0; Fun<=0x7; Fun++) {
		           if (((SDReadPciByte(Bus, Dev, Fun, 0x00))!=0xFF)&&((SDReadPciByte(Bus, Dev, Fun, 0x00))!=0x00)) {
			SDPcie(Bus,Dev,Fun);
		           }
		        } // for()
	        } // for()
	} //for()
} //SDGetPCIList()

//----------------------------------------------------------------------------- PCI & PCIe address.<<

//----------------------------------------------------------------------------- Memory Base Address.>>
//FUNCTION: Read 256 bytes Memroy area registers.
VOID SDMem(UINT32 Address) {
	U16 i, j;
	a7printf("==================================================\n");
	a7printf("Get Memory data : %08Xh\n", Address);
	// Read 256 bytes Memory area registers
	a7printf("   00 01 02 03 04 05 06 07 08-09 0A 0B 0C 0D 0E 0F\n");
	for(i=0; i<0x10; i++) {
		a7printf("%02X ", i*0x10);
	        for(j=0; j<0x10; j++) {
	           if (j<0x0F) {
	            a7printf("%02X-", A7GETMEM8((U32)(Address +(i*0x10+j))));
	           } else {
	            a7printf("%02X", A7GETMEM8((U32)(Address +(i*0x10+j))));
	           }
	        } // for()
	        a7printf("\n");
	} //for()
	a7printf("==================================================\n");

} // SDMem()

//FUNCTION: Set Memory Base address data.
VOID SDSetMem() {
	U32 Address;
	U8 Data;
	Address=hugetbios32(0xD1);					// Memory_BaseAddress
	a7printf("Memory Baseaddress %08Xh\n", Address);
	Data=hugetbios8(0xD5);					// Data value for set
	a7printf("Set Data value to %02Xh**\n", Data);
	A7SETMEM8(Address,Data);
	SDMem(Address & 0xFFFFFF00);
	A7husetbios(0xD0,0x01);
} //SDSetMem()

//----------------------------------------------------------------------------- Memory Base Address.<<

//----------------------------------------------------------------------------- IO Base Address.>>
//FUNCTION: Show 256 bytes data of IO Base Address.
VOID SDIO(UINT16 Address) {
	int i, j;

	a7printf("==================================================\n");
	a7printf("Get IO Base address data : %04Xh\n", Address);
     	a7printf("   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
     	for (i=0; i<0x10; i++) {
     	    a7printf("%02X ", i * 0x10);
     	    for (j=0; j<0x10; j++) {
	           if (j<0x0F) {
  		a7printf("%02X-", IoRead8((U16)(Address +(i*0x10+j))) );
	           } else {
 		a7printf("%02X ", IoRead8((U16)(Address +(i*0x10+j))) );
	           }
  	    } // for()
  	    a7printf("\n");
  	} // for()
	a7printf("==================================================\n");

} //SDIO()

//FUNCTION: Set IO Base address data.
VOID SDSetIO() {
	U16 Address;
	U8 Data;
	Address=hugetbios16(0x91);					// IO_BaseAddress
	a7printf("IOBaseaddress %04Xh**\n", Address);
	Data=hugetbios8(0x93);					// Data
	a7printf("Set Data value to %02Xh**\n", Data);
	IoWrite8(Address,Data);
	SDIO(Address & 0xFF00);
	A7husetbios(0x90,0x01);
} //SDSetIO()
//----------------------------------------------------------------------------- IO Base address.<<

//----------------------------------------------------------------------------- Index IO. >>
//FUNCTION: Get [IndexIO] = Index IO address.
unsigned char SDGetIndex(unsigned char IndexAddr, unsigned char IndexData) {
	IoWrite8(IndexAddr, IndexData);				// Index
	return IoRead8(IndexAddr+1);				// Data
}

//FUNCTION: Set [IndexIO] = Data by Index IO address.
int SDSetIndex(unsigned char IndexAddr, unsigned char IndexData, unsigned char DataSet) {
	IoWrite8(IndexAddr, IndexData);				// Index
	IoWrite8(0xEB, 0x55);					// IO delay
	IoWrite8(IndexAddr+1, DataSet);				// Write Data
	return 0;
}

//FUNCTION: Show 256 bytes data of Index IO
VOID SDGetIndexIO(U8 IndexIO) {
	int i, j;

	a7printf("==================================================\n");
	if (IndexIO==0x70 || IndexIO==0x72){
	a7printf("Get CMOS %02Xh/%02Xh Data\n", IndexIO, IndexIO +1);
	} else {
	a7printf("Get Inedx IO %02Xh/%02Xh Data\n", IndexIO, IndexIO +1);
	}
     	a7printf("   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
     	for (i=0; i<0x10; i++) {
     	    a7printf("%02X ", i * 0x10);
     	    for (j=0; j<0x10; j++) {
	           if (j<0x0F) {
  		a7printf("%02X-", SDGetIndex(IndexIO,(0x10 * i + j)) );
	           } else {
  		a7printf("%02X ", SDGetIndex(IndexIO,(0x10 * i + j)) );
	           }
  	    } // for()
  	    a7printf("\n");
  	} // for()
	a7printf("==================================================\n");
} //SDGetIndexIO()

//FUNCTION: Set INDEXIO offset address data.
VOID SDSetINDEXIO() {
	U8 Addr;
	U8 Offset;
	U8 Data;
	Addr=hugetbios8(0xA1);					// INDEXIO_Address
	Offset=hugetbios8(0xA2);					// INDEXIO_Offset
	a7printf("INDEXIO %02Xh Offset %2Xh\n", Addr, Offset);
	Data=hugetbios8(0xA3);					// Data need to set
	a7printf("Set Data value to %02Xh**\n", Data);
	SDSetIndex(Addr,Offset,Data);
	SDGetIndexIO(Addr);
	A7husetbios(0xA0,0x01);
} //SDSetINDEXIO()

//FUNCTION: Set CMOS offset address data.
VOID SDSetCMOS() {
	U8 Addr;
	U8 Data;
	Addr=hugetbios8(0xB1);					// CMOS_Address
	a7printf("CMOS address %02Xh", Addr);
	Data=hugetbios8(0xB2);					// Data need to set
	a7printf("Set Data value to %02Xh**\n", Data);
	SDSetIndex(0x70,Addr,Data);
	SDGetIndexIO(0x70);
	A7husetbios(0xB0,0x01);
} //SDSetCMOS()

//----------------------------------------------------------------------------- Index IO. <<

//----------------------------------------------------------------------------- Super IO >>
unsigned char SDGet2E(unsigned char sioIndex) {
	IoWrite8(0x2E, sioIndex);					// Index
	return IoRead8(0x2F);					// Data
}

//FUNCTION: Set [sioIndex] = sioData by SIO 2E/2F.
int SDSet2E(unsigned char sioIndex, unsigned char sioData) {
	IoWrite8(0x2E, sioIndex);					// Index
	IoWrite8(0xEB, 0x55);					// IO delay
	IoWrite8(0x2F, sioData);					// Write Data
	return 0;
}

//FUNCTION: Show 256 bytes data of Super IO 2E/2F
VOID SDsio2e() {
	int i, j;

	a7printf("==================================================\n");
	a7printf("Get Super IO 2E/2F Data\n");
     	a7printf("   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
     	for (i=0; i<0x10; i++) {
     	    a7printf("%02X ", i * 0x10);
     	    for (j=0; j<0x10; j++) {
	           if (j<0x0F) {
  		a7printf("%02X-", SDGet2E(0x10 * i + j) );
	           } else {
  		a7printf("%02X ", SDGet2E(0x10 * i + j) );
	           }
  	    } // for()
  	    a7printf("\n");
  	} // for()
	a7printf("==================================================\n");

} //SDsio2e()
//----------------------------------------------------------------------------- Super IO <<

//FUNCTION: Issue 97 or A7 command for BIOS show control and check is any request for pause until user pressed any key on PuTTY Hyper-Terminal.
//INPUT   : None.
//OUTPUT  : None.
VOID PauseForKeyin() {
U8 PCIBus;
U8 PCIDev;
U8 PCIFun;
U16 IOBase;
U8 IndexAdd;
U32 MemBase;
volatile  unsigned short   TempSpiStatus;
U8 Keys;

  if(EmuDeviceExisted()) {	// if Emulator device existed
        do {
	if (hugetbios8(0x40)!=0){					// PCI_REQUEST
	PCIBus=hugetbios8(0x41);					// PCI_BUS
	PCIDev=hugetbios8(0x42);					// PCI_DEV
	PCIFun=hugetbios8(0x43);					// PCI_FUN
	SDPcie(PCIBus,PCIDev,PCIFun);
//	A7husetbios(0x40,0x00);
	}
	if (hugetbios8(0x50)!=0){					// IO_REQUEST
	IOBase=hugetbios16(0x51);					// IO_BaseAddress
	SDIO(IOBase);
//	A7husetbios(0x50,0x00);
	}
	if (hugetbios8(0x60)!=0){					// INDEXIO_REQUEST
	IndexAdd=hugetbios8(0x61);					// INDEXIO_INDEXAddress
	SDGetIndexIO(IndexAdd);
//	A7husetbios(0x60,0x00);
	}
	if (hugetbios8(0x70)!=0){					// CMOS_REQUEST
	SDGetIndexIO(0x70);
//	A7husetbios(0x70,0x00);
	}
	if (hugetbios8(0x80)!=0){					// MEMORY_REQUEST
	MemBase=hugetbios32(0x81);				// Memory_Address
	SDMem(MemBase);
//	A7husetbios(0x80,0x00);
	}
	if ((hugetbios8(0x48)!=0)&&(hugetbios8(0x8A)!=0)){		// 0x48==>> PCI_List_REQUEST  && 0x8A==>Read_REQUEST
	SDGetPCIList();
	}
	if ((hugetbios8(0x01)!=0)&&(hugetbios8(0x30)!=0)){		// 0x01==>> BIOS_ANY_REQUEST  && 0x30==>BIOS_PAUSE_REQUEST
		if (hugetbios8(0x90)!=0){				// Set IO_Base_REQUEST
		SDSetIO();
		}
		if (hugetbios8(0xA0)!=0){				// Set INDEXIO_REQUEST
		SDSetINDEXIO();
		}
		if (hugetbios8(0xB0)!=0){				// Set CMOS_REQUEST
		SDSetCMOS();
		}
		if (hugetbios8(0xC0)!=0){				// Set PCI_Device_REQUEST
		SDSetPCI();
		}
		if (hugetbios8(0xD0)!=0){				// Set Memory_REQUEST
		SDSetMem();
		}
	a7printf("******Press Any key to get value again or Space key for Continue...********\n");
	Keys=a7getch();						// wait and get a key
		if(Keys == 0x20) {					// if [Space] key
		A7husetbios(0x01,0x00);				// Clear BIOS any request.
		A7husetbios(0x30,0x00);				// Clear BIOS pause request.
		a7printf("Continue! *\n");
		}
	}

	TempSpiStatus = hugetbios8(0x30);
        } while (TempSpiStatus!= 0); 				// Wait for BIOS_PAUSE_REQUEST to be unchecked.
    }
}

#if A7Printf_IPMI_SUPPORT
VOID SMC_FixedDelay( UINT32 dCount )
{
	UINT8	Reference;
	UINT8	Current;

	Reference = IoRead8(0x61);
	Reference &= 0x10;
	while(dCount)
	{
		Current = IoRead8(0x61);
    	Current &= 0x10;
    	if(Reference != Current)
    	{
      		Reference = Current;
      		dCount--;
    	}
  	}
}

//Initialize 8254 Counter 1 for mode 2 operation.
VOID SMC_InitializeCounter()
{
	IoWrite8(0x43, 0x74);	//Initialize 8254 Counter 1 for mode 2 operation.
	IoWrite8(0x41, 0x00);
	IoWrite8(0x41, 0x10);
}


UINT8 SMC_KcsErrorExit( UINT16 KcsPor )
{
	UINT8		KcsData;
	UINT8		KcsStatus;
	UINT8		BmcStatus;
	UINT8		RetryCount;
	UINT16		Counter;

	RetryCount = 0;
	while(RetryCount < 10)	//KCS_ABORT_RETRY_COUNT
	{

		#ifdef PEI_STATUS_CODE
			//Initialize 8254 Counter 1 for mode 2 operation.
		    SMC_InitializeCounter();
			Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
		#else
			Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
		#endif

    	while( TRUE )
		{
			KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);
			if( KcsStatus == 0xFF)
			{
				RetryCount = 10;		//KCS_ABORT_RETRY_COUNT
				break;
			}

			if( !(KcsStatus & 0x02) )	//IBF
			{
				break;
			}

			if( Counter-- == 0 )
			{
				a7printf("SMC_KcsErrorExit A1 EFI_NOT_READY\n");
	    		return 0x40;			//EFI_NOT_READY
			}

			#ifdef PEI_STATUS_CODE
				SMC_FixedDelay(2);  	//Default delay is 5ms
			#else
				SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
			#endif
		}

		if(RetryCount >= 10)	//KCS_ABORT_RETRY_COUNT
		{
			break;
		}

		KcsData = 0x60;			//KCS_ABORT
		IoWrite8(IPMI_KCS_COMMAND_PORT, KcsData);

		#ifdef PEI_STATUS_CODE
			Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
			//Initialize 8254 Counter 1 for mode 2 operation.
			SMC_InitializeCounter();
		#else
			Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
		#endif

		while( TRUE )
		{
      		KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);
			if( !(KcsStatus & 0x02) )	//Check IBF
			{
				break;
			}

			if ( Counter-- == 0 )
			{
				a7printf("SMC_KcsErrorExit A2 EFI_NOT_READY\n");
				return 0x40; 			//EFI_NOT_READY
			}

			#ifdef PEI_STATUS_CODE
				SMC_FixedDelay(2);  	//Default delay is 5ms
			#else
				SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
			#endif
		}

		KcsData = IoRead8(IPMI_KCS_DATA_PORT);

		KcsData = 0x0;
		IoWrite8(IPMI_KCS_DATA_PORT, KcsData);

		#ifdef PEI_STATUS_CODE
			Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
			//Initialize 8254 Counter 1 for mode 2 operation.
			SMC_InitializeCounter();
		#else
			Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
		#endif

		while ( TRUE )
		{
			KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);

      		if ( !(KcsStatus & 0x02) )	//Check IBF
			{
        		break;
			}

			if( Counter-- == 0 )
			{
				a7printf("SMC_KcsErrorExit A3 EFI_NOT_READY\n");
				return 0x40;			//EFI_NOT_READY
			}

			#ifdef PEI_STATUS_CODE
				SMC_FixedDelay(2);  //Default delay is 5ms
			#else
				SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
			#endif
		}

		if( ( (KcsStatus & 0xC0) >> 6 ) == KcsReadState)	//Check State
		{
			#ifdef PEI_STATUS_CODE
			    Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
				//Initialize 8254 Counter 1 for mode 2 operation.
				SMC_InitializeCounter();
			#else
				Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
			#endif

			while( TRUE )
			{
				KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);

				if( KcsStatus & 0x01 )	//Check OBF
				{
					break;
				}

				if( Counter-- == 0 )
				{
					a7printf("SMC_KcsErrorExit A4 EFI_NOT_READY\n");
					return 0x40;		//EFI_NOT_READY
				}

				#ifdef PEI_STATUS_CODE
					SMC_FixedDelay(2);  //Default delay is 5ms
				#else
					SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
				#endif
			}

			BmcStatus = IoRead8(IPMI_KCS_DATA_PORT);

			KcsData = 0x68;	//KCS_READ
			IoWrite8(IPMI_KCS_DATA_PORT, KcsData);

			#ifdef PEI_STATUS_CODE
			    Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
				//Initialize 8254 Counter 1 for mode 2 operation.
				SMC_InitializeCounter();
			#else
				Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
			#endif

			while( TRUE )
			{
				KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);

				if( !(KcsStatus & 0x02) )	//Check IBF
				{
					break;
				}

				if( Counter-- == 0 )
				{
					a7printf("SMC_KcsErrorExit A5 EFI_NOT_READY\n");
					return 0x40;			//EFI_NOT_READY
				}

				#ifdef PEI_STATUS_CODE
					SMC_FixedDelay(2);  	//Default delay is 5ms
				#else
					SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
				#endif
			}

			if( ( (KcsStatus & 0xC0) >> 6 ) == KcsIdleState)	//Check State
			{
				#ifdef PEI_STATUS_CODE
					Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
					//Initialize 8254 Counter 1 for mode 2 operation.
			    	SMC_InitializeCounter();
				#else
					Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
				#endif

				while( TRUE )
				{
					KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);

					if( KcsStatus & 0x01 )		//Check OBF
					{
						break;
					}

					if ( Counter-- == 0 )
					{
						a7printf("SMC_KcsErrorExit A6 EFI_NOT_READY\n");
						return 0x40;			//EFI_NOT_READY
					}

					#ifdef PEI_STATUS_CODE
						SMC_FixedDelay(2);  		//Default delay is 5ms
					#else
						SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
					#endif
				}

				KcsData = IoRead8(IPMI_KCS_DATA_PORT);
       			break;

			}
			else
			{
				RetryCount++;
				continue;
			}
		}
		else
		{
			RetryCount++;
			continue;
		}
	}

	if(RetryCount >= 10)	//KCS_ABORT_RETRY_COUNT
	{
		a7printf("SMC_KcsErrorExit A7 EFI_DEVICE_ERROR\n");
		return 0x80;		//EFI_DEVICE_ERROR
	}

	a7printf("SMC_KcsErrorExit A8 EFI_ABORTED\n");
	return 0x01;			//EFI_ABORTED
}

UINT8 SMC_KcsCheckStatus( UINT16 KcsPort, KCS_STATE KcsState, BOOLEAN *Idle )
{
	UINT8		KcsStatus;
	UINT16		Counter;
	UINT8		KcsData;

	*Idle = FALSE;
	#ifdef PEI_STATUS_CODE
		//Initialize 8254 Counter 1 for mode 2 operation.
	    SMC_InitializeCounter();
		Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
	#else
		Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
	#endif

	while( TRUE )
	{
		KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);
    	if( !(KcsStatus & 0x02) ) break;	//check IBF

    	if( Counter-- == 0 )
		{
			a7printf("SMC_KcsCheckStatus A1 EFI_NOT_READY\n");	//display error message
			return 0x40; 					//EFI_NOT_READY
		}

		#ifdef PEI_STATUS_CODE
			SMC_FixedDelay(2);  			//Default delay is 5ms
		#else
			SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
		#endif
  	}

  	if(((KcsStatus & 0xC0)>>6) != KcsState)     //Check State
	{
    	if( ( ( (KcsStatus & 0xC0) >> 6 ) == KcsIdleState) && (KcsState == KcsReadState))
		{
      		*Idle = TRUE;
    	}
		else
		{
			a7printf("KcsStatus : %x\n", KcsStatus);	//dispaly error message
      		return SMC_KcsErrorExit(KcsPort);
    	}
  	}

	if(KcsState == KcsReadState)
	{
		#ifdef PEI_STATUS_CODE
			Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
    		//Initialize 8254 Counter 1 for mode 2 operation.
		    SMC_InitializeCounter();
		#else
			Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
		#endif
		while( TRUE )
		{
      		KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);
      		if( KcsStatus & 0x01 ) break;	//Check OBF

      		if( Counter-- == 0 )
			{
				a7printf("SMC_KcsCheckStatus A2 EFI_NOT_READY\n");	//dispaly error message
				return 0x40;				//EFI_NOT_READY
			}

			#ifdef PEI_STATUS_CODE
				SMC_FixedDelay(2);  		//Default delay is 5ms
			#else
				SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
			#endif
    	}
  	}

	if(KcsState == KcsWriteState || *Idle)
	{
		KcsData = IoRead8(IPMI_KCS_DATA_PORT);
  	}

  	return 0;	//EFI_SUCCESS
}

UINT8 SMC_SendDataToBmcPort( UINT8 *Data, UINT8 DataSize )
{
	UINT8		KcsStatus;
	UINT8		KcsData;
	UINT8		Status;
	UINT8		i;
	BOOLEAN		Idle;
	UINT16		Counter;

	#ifdef PEI_STATUS_CODE
		//Initialize 8254 Counter 1 for mode 2 operation.
	    SMC_InitializeCounter();
		Counter = IPMI_PEI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
	#else
		Counter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
	#endif

	while( TRUE )
	{
		KcsStatus = IoRead8(IPMI_KCS_COMMAND_PORT);

		if( ( (KcsStatus & 0xC0) >> 6 ) != KcsIdleState)	//Check State
		{
			if((Status = SMC_KcsErrorExit(IPMI_KCS_BASE_ADDRESS)) != 0)	//EFI_SUCCESS
			{
				a7printf("SMC_SendDataToBmcPort SMC_KcsErrorExit Status : %x\n", Status);
        		return Status;
			}
		}

		if( !((KcsStatus & 0x02) || ((KcsStatus & 0xC0) != KcsIdleState)) )
		{
      		break;
		}

		if( Counter-- == 0 )
		{
     		return 0x40;	//EFI_NOT_READY
    	}

		#ifdef PEI_STATUS_CODE
			SMC_FixedDelay(2);  //Default delay is 5ms
		#else
			SMC_FixedDelay(IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
		#endif
	}

	KcsData = 0x61;	//KCS_WRITE_START
	IoWrite8(IPMI_KCS_COMMAND_PORT, KcsData);
	a7printf("KCS_WRITE_START\n");//display error message

	if((Status = SMC_KcsCheckStatus(IPMI_KCS_BASE_ADDRESS, KcsWriteState, &Idle)) != 0)		// if not EFI_Success
	{
		a7printf("SMC_SendDataToBmcPort SMC_KcsCheckStatus Status A1 : %x\n", Status);//display error message
    	return Status;
	}

	for(i = 0; i < DataSize; i++)
	{
		if (i == (DataSize - 1))
		{
			if ((Status = SMC_KcsCheckStatus(IPMI_KCS_BASE_ADDRESS, KcsWriteState, &Idle)) != 0)	// if not EFI_Success
			{
				a7printf("SMC_SendDataToBmcPort SMC_KcsCheckStatus Status A2 : %x\n", Status);//display error message
				return Status;
			}

			KcsData = 0x62;	//KCS_WRITE_END
			IoWrite8(IPMI_KCS_COMMAND_PORT, KcsData);
			a7printf("KCS_WRITE_END\n");//display error message
		}

		SMC_KcsCheckStatus(IPMI_KCS_BASE_ADDRESS, KcsWriteState, &Idle);
		IoWrite8(IPMI_KCS_DATA_PORT, Data[i]);
		a7printf("Command Data[%d] : %x\n", i, Data[i]);//display error message
	}

	return 0;	//EFI_SUCCESS
}

UINT8 SMC_ReceiveBmcDataFromPort( UINT8 *Data, UINT8 *DataSize )
{
	UINT8		KcsData;
	BOOLEAN		Idle;
	UINT8		Count;
	UINT8		Status;

	Count     = 0;

	while(TRUE)
	{
	    if((Status = SMC_KcsCheckStatus(IPMI_KCS_BASE_ADDRESS, KcsReadState, &Idle)) != 0)	//EFI_SUCCESS
		{
			a7printf("SMC_ReceiveBmcDataFromPort SMC_KcsCheckStatus Status : %x\n", Status);//display error message
      		return Status;
    	}

    	if(Idle)
		{
      		*DataSize = Count;
      		break;
    	}

		Data[Count] = IoRead8(IPMI_KCS_DATA_PORT);
   		Count++;

    	KcsData = 0x68;	//KCS_READ
		IoWrite8(IPMI_KCS_DATA_PORT, KcsData);
	}

	return 0;		//EFI_SUCCESS
}

UINT8 SMC_UpdateErrorStatus( UINT8 BmcError )
{
	UINT8   Errors[] = {0xC0, 0xC3, 0xC4, 0xC9, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xFF, 0x00};
	UINT8   i;

	i = 0;
	while(Errors[i] != 0)
	{
		if(Errors[i] == BmcError)
		{
			SoftErrorCount++;
			break;
		}
		i++;
	}
	return 0;	//EFI_SUCCESS
}

UINT8 SMC_IpmiSendCommand
(
	UINT8	NetFunction,
	UINT8	Lun,
	UINT8	Command,
	UINT8	*CommandData,
	UINT8	CommandDataSize,
	UINT8	*ResponseData,
	UINT8	*ResponseDataSize
)
{
	UINT8	DataSize;
	UINT8	Status;
	UINT8	CompletionCode;
	UINT8	Counter;

	if(SoftErrorCount >= MAX_BMC_CMD_FAIL_COUNT)
    return 0x40;	//EFI_NOT_READY

	TempData[0] = (UINT8) ((NetFunction << 2) | (Lun & 0x03));
	TempData[1] = Command;

	if(CommandDataSize > 0)
	{
		if(CommandData == NULL)
		{
			a7printf("EFI_INVALID_PARAMETER\n");		//display error message
			return 0x04;	//EFI_INVALID_PARAMETER
    	}
		MemCpy(&TempData[2], CommandData, CommandDataSize);
	}

	if((Status = SMC_SendDataToBmcPort( TempData, (UINT8) (CommandDataSize + 2))) != 0)	//EFI_SUCCESS
	{
		a7printf("SMC_SendDataToBmcPort Status : %x\n", Status);
		SoftErrorCount++;
		return Status;
	}

	DataSize = 100;//MAX_TEMP_DATA
	if((Status = SMC_ReceiveBmcDataFromPort( TempData, &DataSize)) != 0)	//EFI_SUCCESS
	{
		a7printf("SMC_ReceiveBmcDataFromPort Status : %x\n", Status);
		SoftErrorCount++;
		return Status;
	}

	CompletionCode = TempData[2];

	if( ERROR_COMPLETION_CODE(CompletionCode) )
	{
    	SMC_UpdateErrorStatus(CompletionCode);
		// Write completion code into return buffer if an IPMI command returns an error
		*ResponseData = CompletionCode;
		*ResponseDataSize = 1;

		a7printf("SMC_IpmiSendCommand EFI_DEVICE_ERROR\n");		//display error message

		return 0x80;	//EFI_DEVICE_ERROR
	}

	if((DataSize - 3) > *ResponseDataSize)
	{

		a7printf("SMC_IpmiSendCommand EFI_BUFFER_TOO_SMALL\n");	//display error message

		return 0x20;	//EFI_BUFFER_TOO_SMALL
	}

	MemCpy(ResponseData, &TempData[3], (DataSize - 3));
	*ResponseDataSize = (UINT8) (DataSize - 3);

	if( *ResponseDataSize!=0 ) a7printf("###### Start to receive data from BMC.\n");	//display message
	for(Counter=0;Counter<*ResponseDataSize;Counter++)
	{
		a7printf("ResponseData[%d] : %x\n", Counter, ResponseData[Counter]);
	}

	return 0;	//EFI_SUCCESS
}
#endif

VOID SMC_Write_MSR()
{
#if (A7_MRC_DEBUG == 1)
	UINT32  Msr;
	UINT64  MsrData;
	UINT32  MsrData_High_32bits;
	UINT32  MsrData_Low_32bits;

	a7printf("***************************************************************************\n");
	//WriteMsr
	MsrData				= 0;
	Msr 				= hugetbios32(0x34);
	MsrData_High_32bits	= hugetbios32(0x38);
	MsrData_Low_32bits	= hugetbios32(0x3C);

	a7printf("WriteMsr\n");
	a7printf("Msr : %x\n", Msr);
	a7printf("MsrData_High_32bits : %x\n", MsrData_High_32bits);
	a7printf("MsrData_Low_32bits  : %x\n", MsrData_Low_32bits);

	MsrData = (Shl64( (MsrData | MsrData_High_32bits), 32)) | MsrData_Low_32bits;
	WriteMsr(Msr, MsrData);

	//ReadMsr
	MsrData = ReadMsr(Msr);
	MsrData_High_32bits = (UINT32)( Shr64(MsrData, 32 ));
	MsrData_Low_32bits = (UINT32)(MsrData & 0x00000000FFFFFFFF);

	a7printf("ReadMsr\n");
	a7printf("Msr : %x\n", Msr);
	a7printf("MsrData_High_32bits : %x\n", MsrData_High_32bits);
	a7printf("MsrData_Low_32bits  : %x\n", MsrData_Low_32bits);

	A7husetbios(0x33, 0x00);
	a7printf("***************************************************************************\n");
#endif
}

//FUNCTION: Get [IndexIO] = 2 bytes Index IO address.
U8 SMC_SDGetIndex16(U16 IndexAddr, U8 IndexData)
{
	IoWrite8((UINT16)IndexAddr, IndexData);				// Index
	return IoRead8((UINT16)IndexAddr+1);					// Data
}

//FUNCTION: Show 256 bytes data of 2 bytes Index IO address.
VOID SMC_SDGetIndexIO16(U16 IndexIO)
{
	int i, j;

	a7printf("==================================================\n");
	if (IndexIO==0x70 || IndexIO==0x72)
	{
		a7printf("Get CMOS %04Xh/%04Xh Data\n", IndexIO, IndexIO +1);
	}
	else
	{
		a7printf("Get Inedx IO %04Xh/%04Xh Data\n", IndexIO, IndexIO +1);
	}

	a7printf("   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	for(i=0; i<0x10; i++)
	{
		a7printf("%02X ", i * 0x10);
		for(j=0; j<0x10; j++)
		{
			if(j<0x0F)
			{
 				a7printf("%02X-", SMC_SDGetIndex16(IndexIO,(0x10 * i + j)) );
			}
			else
			{
				a7printf("%02X ", SMC_SDGetIndex16(IndexIO,(0x10 * i + j)) );
			}
		} // for()
		a7printf("\n");
	} // for()
	a7printf("==================================================\n");
} //SDGetIndexIO16()

//FUNCTION: Set [IndexIO] = Data by 2 bytes Index IO address.
int SMC_SDSetIndex16(U16 IndexAddr, unsigned char IndexData, unsigned char DataSet)
{
	IoWrite8((UINT16)IndexAddr, IndexData);			// Index
	IoWrite8(0xEB, 0x55);						// IO delay
	IoWrite8((UINT16)IndexAddr+1, DataSet);			// Write Data
	return 0;
}

//FUNCTION: Set INDEXIO offset address data for 2 bytes address.
VOID SMC_SDSetINDEXIO16()
{
	U16		Addr;
	U8		Offset;
	U8		Data;

	a7printf("***************************************************************************\n");

	Addr = hugetbios16(0xA7);						// INDEXIO_Address
	Offset = hugetbios8(0xA9);						// INDEXIO_Offset
	a7printf("INDEXIO %04Xh Offset %2Xh\n", Addr, Offset);
	Data = hugetbios8(0xAB);						// Data need to set
	a7printf("Set Data value to %02Xh\n\n", Data);
	SMC_SDSetIndex16(Addr, Offset, Data);

	SMC_SDGetIndexIO16(Addr);
	a7printf("***************************************************************************\n");
	A7husetbios(0xA6, 0x00);
} //SMC_SDSetINDEXIO16()

//FUNCTION: Set IO Base address data.
VOID SMC_SDSetIO()
{
	U16 Address;
	U8 Data;

	a7printf("***************************************************************************\n");

	Address=hugetbios16(0x91);					// IO_BaseAddress
	a7printf("IOBaseaddress %04Xh\n", Address);
	Data=hugetbios8(0x93);						// Data
	a7printf("Set Data value to %02Xh\n\n", Data);
	IoWrite8(Address,Data);
	a7printf("IOBaseaddress %04Xh = %02X \n", Address, IoRead8((U16)(Address)) );

	a7printf("***************************************************************************\n");

	A7husetbios(0x90,0x01);
} //SMC_SDSetIO()

//FUNCTION: Set INDEXIO offset address data.
VOID SMC_SDSetINDEXIO()
{
	U8 Addr;
	U8 Offset;
	U8 Data;

	a7printf("***************************************************************************\n");

	Addr=hugetbios8(0xA1);						// INDEXIO_Address
	Offset=hugetbios8(0xA2);					// INDEXIO_Offset
	a7printf("INDEXIO %02Xh Offset %2Xh\n", Addr, Offset);
	Data=hugetbios8(0xA3);						// Data need to set
	a7printf("Set Data value to %02Xh\n\n", Data);
	SDSetIndex(Addr, Offset, Data);

	if (Addr==0x70 || Addr==0x72)
	{
		a7printf("Get CMOS %02Xh/%02Xh Data\n", Addr, Addr+1);
	}
	else
	{
		a7printf("Get Inedx IO %02Xh/%02Xh Data\n", Addr, Addr+1);
	}

	a7printf("INDEXIO %02Xh Offset %2Xh = %02X \n", Addr, Offset, SDGetIndex(Addr, Offset) );

	a7printf("***************************************************************************\n");

	A7husetbios(0xA0,0x01);
} //SMC_SDSetINDEXIO()

//FUNCTION: Set CMOS offset address data.
VOID SMC_SDSetCMOS()
{
	U8 Addr;
	U8 Data;

	a7printf("***************************************************************************\n");

	Addr=hugetbios8(0xB1);					// CMOS_Address
	a7printf("CMOS address %02Xh", Addr);
	Data=hugetbios8(0xB2);					// Data need to set
	a7printf("Set Data value to %02Xh\n\n", Data);
	SDSetIndex(0x70, Addr, Data);

	a7printf("CMOS address %02Xh = %02Xh\n", Addr, SDGetIndex(0x70, Addr));

	a7printf("***************************************************************************\n");

	A7husetbios(0xB0,0x01);
} //SMC_SDSetCMOS()


//FUNCTION: Set PCI Address data.
VOID SMC_SDSetPCI()
{
	U8 Bus;
	U8 Dev;
	U8 Fun;
	U16 Reg;
	U8 Data;

	a7printf("***************************************************************************\n");

	Bus=hugetbios8(0xC1);					// PCI BUS number.
	Dev=hugetbios8(0xC2);					// PCI Device number.
	Fun=hugetbios8(0xC3);					// PCI Function number.
	Reg=hugetbios16(0xC4);					// PCI Register number.
	a7printf("PCI BUS : %02Xh / Device : %02Xh / Function : %02Xh/ Register : %4Xh\n", Bus, Dev, Fun, Reg);
	Data=hugetbios8(0xC6);					// Data
	a7printf("Regsiter data set to %02Xh**\n\n", Data);
	SDSetPcieAddr(Bus, Dev, Fun, Reg, Data);

	a7printf("BUS : %04Xh / Device : %04Xh / Function : %04Xh\n", Bus, Dev, Fun);
	a7printf("Register %2Xh = %02X\n", Reg, SDReadPciByte( (U8) Bus, (U8) Dev, (U8) Fun, (U16) Reg ));

	a7printf("***************************************************************************\n");

	A7husetbios(0xC0,0x01);
} //SMC_SDSetPCI()

//FUNCTION: Set Memory Base address data.
VOID SMC_SDSetMem()
{
	U32 Address;
	U8 Data;

	a7printf("***************************************************************************\n");

	Address=hugetbios32(0xD1);					// Memory_BaseAddress
	a7printf("Memory Baseaddress %08Xh\n", Address);
	Data=hugetbios8(0xD5);						// Data value for set
	a7printf("Set Data value to %02Xh**\n\n", Data);
	A7SETMEM8(Address,Data);

	a7printf("Memory %08Xh = %02X\n", Address, A7GETMEM8((U32)Address));

	a7printf("***************************************************************************\n");

	A7husetbios(0xD0,0x01);
} //SMC_SDSetMem()

//FUNCTION: Read 256 bytes PCI/PCIe device registers.
VOID SMC_SDPcie(U8 Bus, U8 Device, U8 Function, U16 Register)
{
	U32 Address;
	U16 i, j;
	U16 Register_HighByte;

	Register_HighByte = Register & 0xFF00;

	a7printf("==================================================\n");
	a7printf("BUS : %04Xh / Device : %04Xh / Function : %04Xh\n", Bus, Device, Function);
	a7printf("VID : %04Xh / DID : %04Xh\n", SDReadPciWord(Bus, Device, Function,0), SDReadPciWord(Bus, Device, Function,2));

	// Read 256 bytes PCI registers
	a7printf("     00 01 02 03 04 05 06 07 08-09 0A 0B 0C 0D 0E 0F\n", Register_HighByte);

	for(i=0; i<0x10; i++)
	{
		a7printf("%04X ", i*0x10+Register_HighByte);
		for(j=0; j<0x10; j++)
		{
	    	if(j<0x0F)
			{
				a7printf("%02X-", SDReadPciByte( (U8) Bus, (U8) Device, (U8) Function, (U16) (i*0x10+j+Register_HighByte) ));
			}
			else
			{
				a7printf("%02X", SDReadPciByte( (U8) Bus, (U8) Device, (U8) Function, (U16) (i*0x10+j+Register_HighByte) ));
			}
		} // for()
		a7printf("\n");
	} //for()

	a7printf("==================================================\n");
#if DYNAMIC_PCIEXBASE_SUPPORT
	Address=(GetPciBaseAddr() | (Bus << 20) | (Device << 15) | (Function << 12));
#else
	Address=(SMC_PCIEX_BASE_ADDRESS | (Bus << 20) | (Device << 15) | (Function << 12));
#endif
	a7printf("%08Xh\n",Address);
} // SMC_SDPcie()

//FUNCTION: Issue 97 or A7 command for BIOS show control and check is any request for pause.
//          SMC_PauseForKeyin2 only pause on check point to debug.
//INPUT	  : IN UINT8              Checkpoint
//OUTPUT  : None
VOID SMC_PauseForKeyin2(void)
{
	U8 PCIBus;
	U8 PCIDev;
	U8 PCIFun;
	U16 PCIReg;
	U16 IOBase;
	U8 IndexAdd;
	U16 IndexAdd16;
	U32 MemBase;
	volatile  unsigned short   TempSpiStatus;
	//U8 Keys;

	unsigned char HCKTPType_Pause;
	// HCKTPType_Pause : Pause type
	// HCKTPType_Pause = 0x15 => Single Pause
	// HCKTPType_Pause = 0x25 => Range Pause
	// HCKTPType_Pause = 0x35 => Phase Pause
	// HCKTPType_Pause = 0x45 => All Pause

	unsigned char HCKTPType_Read;
	// HCKTPType_Read : Read type
	// HCKTPType_Read = 0x15 => Single Read
	// HCKTPType_Read = 0x25 => Range Read
	// HCKTPType_Read = 0x35 => Phase Read
	// HCKTPType_Read = 0x45 => All Read

	unsigned char Show_Data_Flag;
	//Show_Data_Flag = 1 => Show Data
	//Show_Data_Flag = 0 => Don't Show Data

#if (A7_MRC_DEBUG == 1)
	UINT32  Msr;
	UINT64  MsrData;
	UINT32  MsrData_High_32bits;
	UINT32  MsrData_Low_32bits;
#endif
	#if A7Printf_IPMI_SUPPORT
	UINT8	NetFunction;
	UINT8	Lun;
	UINT8	Command;
	UINT8	CommandData[20]={0};
	UINT8	CommandDataSize;
	UINT8	ResponseData[100]={0};
	UINT8	ResponseDataSize;
	UINT8	Counter;
	#endif

	if(EmuDeviceExisted())		// if Emulator device existed
	{

		TempSpiStatus = hugetbios8(0x30);

		do
		{

////////////////////////////// Read Data ///////////////////////////////////////////////////
//
// Single Read : Read data when checkpoint equals assigned checkpoint.
// Range Read  : Read data when checkpoint is in the range of assigned two checkpoint.
// Phase Read  : Read data when BIOS code is running in assigned phase.
// All Read    : Read data on every checkpoint.
//
/////////////////////////////////////////////////////////////////////////////////////////////

			HCKTPType_Read = hugetbios8(0x20);

			switch(HCKTPType_Read)
			{
				case 0x15:		//Single Read
					if( (IoRead8(0x80)) == (hugetbios8(0x21)) ) Show_Data_Flag = 1;
					else Show_Data_Flag = 0;
					break;

				case 0x25:		//Range Read
					if( ((IoRead8(0x80)) >= (hugetbios8(0x23))) && ((IoRead8(0x80)) <= (hugetbios8(0x25))) ) Show_Data_Flag = 1;
					else Show_Data_Flag = 0;
					break;

				case 0x35:		//Phase Read
					if(hugetbios8(0x28)==0x25)
					{
						#ifdef PEI_STATUS_CODE
							Show_Data_Flag = 1;
						#else
							Show_Data_Flag = 0;
						#endif
					}
					else if(hugetbios8(0x29)==0x25)
					{
						#ifdef DXE_STATUS_CODE
							Show_Data_Flag = 1;
						#else
							Show_Data_Flag = 0;
						#endif
					}
					break;

				case 0x45:		//All Read
					Show_Data_Flag = 1;
					break;

				default:
					//Show_Data_Flag = 0;
					break;
			}

			if( Show_Data_Flag == 1 )
			{

				if (hugetbios8(0x40)!=0)					// PCI_REQUEST
				{
					a7printf("***************************************************************************\n");
					PCIBus=hugetbios8(0x41);				// PCI_BUS
					PCIDev=hugetbios8(0x42);				// PCI_DEV
					PCIFun=hugetbios8(0x43);				// PCI_FUN
					PCIReg=hugetbios16(0x44);				// PCI_Reg
					SMC_SDPcie(PCIBus, PCIDev, PCIFun, PCIReg);
					if(TempSpiStatus!=0) A7husetbios(0x40, 0x00);
					a7printf("***************************************************************************\n");
				}

				if (hugetbios8(0x50)!=0)					// IO_REQUEST
				{
					a7printf("***************************************************************************\n");
					IOBase=hugetbios16(0x51);				// IO_BaseAddress
					SDIO(IOBase);
					if(TempSpiStatus!=0) A7husetbios(0x50, 0x00);
					a7printf("***************************************************************************\n");
				}

				if (hugetbios8(0x60)!=0)					// INDEXIO_REQUEST
				{
					a7printf("***************************************************************************\n");
					IndexAdd=hugetbios8(0x61);				// INDEXIO_INDEXAddress
					SDGetIndexIO(IndexAdd);
					if(TempSpiStatus!=0) A7husetbios(0x60, 0x00);
					a7printf("***************************************************************************\n");
				}

				if (hugetbios8(0x70)!=0)					// CMOS_REQUEST
				{
					a7printf("***************************************************************************\n");
					SDGetIndexIO(0x70);
					if(TempSpiStatus!=0) A7husetbios(0x70, 0x00);
					a7printf("***************************************************************************\n");
				}

				if (hugetbios8(0x80)!=0)					// MEMORY_REQUEST
				{
					a7printf("***************************************************************************\n");
					MemBase=hugetbios32(0x81);				// Memory_Address
					SDMem(MemBase);
					if(TempSpiStatus!=0) A7husetbios(0x80, 0x00);
					a7printf("***************************************************************************\n");
				}

				if (hugetbios8(0x48)!=0)		// 0x48==>> PCI_List_REQUEST
				{
					a7printf("***************************************************************************\n");
					SDGetPCIList();
					if(TempSpiStatus!=0) A7husetbios(0x48, 0x00);
					a7printf("***************************************************************************\n");
				}

				if (hugetbios8(0x67)!=0)					// INDEXIO2_REQUEST
				{
					a7printf("***************************************************************************\n");
					IndexAdd16 = hugetbios16(0x68);				// INDEXIO2_INDEXAddress
					SMC_SDGetIndexIO16(IndexAdd16);
					if(TempSpiStatus!=0) A7husetbios(0x67, 0x00);
					a7printf("***************************************************************************\n");
				}

				if (hugetbios8(0xC8)!=0)					// READMSR_REQUEST
				{
#if (A7_MRC_DEBUG == 1)
					a7printf("***************************************************************************\n");
					Msr = hugetbios32(0xC9);
					MsrData = ReadMsr(Msr);

					MsrData_High_32bits = (UINT32)( Shr64(MsrData, 32 ));
					MsrData_Low_32bits = (UINT32)(MsrData & 0x00000000FFFFFFFF);

					a7printf("Msr : %x\n", Msr);
					a7printf("MsrData_High_32bits : %x\n", MsrData_High_32bits);
					a7printf("MsrData_Low_32bits  : %x\n", MsrData_Low_32bits);
					if(TempSpiStatus!=0) A7husetbios(0xC8, 0x00);
					a7printf("***************************************************************************\n");
#endif
				}

				#if A7Printf_IPMI_SUPPORT
				if (hugetbios8(0xB8)!=0)					// IPMISENDCOMMAND_REQUEST
				{

					a7printf("***************************************************************************\n");

					NetFunction			=	hugetbios8(0xD9);
					Lun					=	hugetbios8(0xDA);
					Command				=	hugetbios8(0xDB);
					CommandDataSize		=	hugetbios8(0xDC);
					ResponseDataSize	=	hugetbios8(0xDD);

					if(CommandDataSize<21)
					{
						for( Counter=0 ; Counter<CommandDataSize ; Counter++ )
						{
							if(Counter<10) CommandData[Counter] = hugetbios8( 0xE6 + Counter );
							else if(Counter>10) CommandData[Counter] = hugetbios8( 0xF6 + Counter - 10 );
						}
						SMC_IpmiSendCommand(NetFunction, Lun, Command, CommandData, CommandDataSize, ResponseData, &ResponseDataSize);
					}
					else a7printf("Command Size is too large.\n");

					if(TempSpiStatus!=0) A7husetbios(0xB8, 0x00);
					a7printf("***************************************************************************\n");
				}
				#endif
			}
			Show_Data_Flag = 0;

////////////////////////////// Pause ////////////////////////////////////////////////////////
//
// Single Pause : Pause when checkpoint equals assigned checkpoint.
// Range Pause  : Pause when checkpoint is in the range of assigned two checkpoint.
// Phase Pause  : Pause when BIOS code is running in assigned phase.
// All Pause    : Pause on every checkpoint.
//
/////////////////////////////////////////////////////////////////////////////////////////////

			TempSpiStatus = hugetbios8(0x30);
			if(TempSpiStatus!=0)
			{

				HCKTPType_Pause = hugetbios8(0x10);

				switch(HCKTPType_Pause)
				{
					case 0x15:		//Single Pause
						if( (IoRead8(0x80)) == (hugetbios8(0x11)) ) TempSpiStatus = 1;
						else TempSpiStatus = 0;
						break;

					case 0x25:		//Range Pause
						if( ((IoRead8(0x80)) >= (hugetbios8(0x13))) && ((IoRead8(0x80)) <= (hugetbios8(0x15))) ) TempSpiStatus = 1;
						else TempSpiStatus = 0;
						break;

					case 0x35:		//Phase Pause
						if(hugetbios8(0x18)==0x25)
						{
							#ifdef PEI_STATUS_CODE
							//	a7printf("PEI\n");
								TempSpiStatus = 1;
							#else
							//	a7printf("Not PEI\n");
								TempSpiStatus = 0;
							#endif
						}
						else if(hugetbios8(0x19)==0x25)
						{
							#ifdef DXE_STATUS_CODE
							//	a7printf("DXE\n");
								TempSpiStatus = 1;
							#else
							//	a7printf("Not DXE\n");
								TempSpiStatus = 0;
							#endif
						}
						if( (hugetbios8(0x18)==0x25) && (hugetbios8(0x19)==0x25) )
						{

							TempSpiStatus = 0;

							#ifdef PEI_STATUS_CODE
							//	a7printf("PEI\n");
								TempSpiStatus = 1;
							#endif

							#ifdef DXE_STATUS_CODE
							//	a7printf("DXE\n");
								TempSpiStatus = 1;
							#endif
						}

						break;

					case 0x45:		//All Pause
						break;

					default:
						break;
				}
			}

////////////////////////////// Set Data /////////////////////////////////////////////////////
//
//	Set data must in Pause state !!
//
/////////////////////////////////////////////////////////////////////////////////////////////

//			if ((hugetbios8(0x01)!=0)&&(hugetbios8(0x30)!=0)){		// 0x01==>> BIOS_ANY_REQUEST  && 0x30==>BIOS_PAUSE_REQUEST
			if ((hugetbios8(0x01)!=0)&&(TempSpiStatus!=0))		// 0x01==>> BIOS_ANY_REQUEST  && 0x30==>BIOS_PAUSE_REQUEST
			{

				if (hugetbios8(0x90)!=0)				// Set IO_Base_REQUEST
				{
					SMC_SDSetIO();
				}

				if (hugetbios8(0xA0)!=0)				// Set INDEXIO_REQUEST
				{
					SMC_SDSetINDEXIO();
				}

				if (hugetbios8(0xB0)!=0)				// Set CMOS_REQUEST
				{
					SMC_SDSetCMOS();
				}

				if (hugetbios8(0xC0)!=0)				// Set PCI_Device_REQUEST
				{
					SMC_SDSetPCI();
				}

				if (hugetbios8(0xD0)!=0)				// Set Memory_REQUEST
				{
					SMC_SDSetMem();
				}

				if (hugetbios8(0xA6)!=0)				// Set INDEXIO2_REQUEST
				{
					SMC_SDSetINDEXIO16();
				}

				if (hugetbios8(0x33)!=0)				// Set WRIEMSR_REQUEST
				{
					SMC_Write_MSR();
				}

				if(hugetbios8(0x30)==0) 				// check the state of pause
				{
					A7husetbios(0x01,0x00);				// Clear BIOS any request.
					a7printf("Continue! *\n");
				}//if(hugetbios8(0x30)==0)
			}//if ((hugetbios8(0x01)!=0)&&(TempSpiStatus!=0))

		} while (TempSpiStatus!= 0); 				// Wait for BIOS_PAUSE_REQUEST to be unchecked.

	}//if(EmuDeviceExisted())
}

//FUNCTION: Output check point and use samedebug tool to debug.
//INPUT   : IN EFI_PEI_SERVICES *PeiServices
//INPUT	  : IN UINT8              Checkpoint
//OUTPUT  : None
VOID Checkpoint_And_PauseForKeyin(IN UINT8 Checkpoint)
{
		a7printf("Checkpoint = %02X\n", Checkpoint);

		SMC_PauseForKeyin2();
}

//SMCDebug2()
//FUNCTION: Pause anywhere to read data and set data without pressing any key.
//          SMCDebug2 can Pause anywhere to debug.
//INPUT   : None.
//OUTPUT  : None.
VOID SMCDebug2()
{

	U8 PCIBus;
	U8 PCIDev;
	U8 PCIFun;
	U16 PCIReg;
	U16 IOBase;
	U8 IndexAdd;
	U16 IndexAdd16;
	U32 MemBase;
 
//	U8 Keys;
	volatile  unsigned short   TempSpiStatus;

	unsigned char Exit_Flag;
	//Exit_Flag = 0 => Don't exit debug loop
	//Exit_Flag = 1 => Exit debug loop
#if (A7_MRC_DEBUG == 1)
	UINT32  Msr;
	UINT64  MsrData;
	UINT32  MsrData_High_32bits;
	UINT32  MsrData_Low_32bits;
#endif

	#if A7Printf_IPMI_SUPPORT
	#if IPMI_SUPPORT
	UINT8	NetFunction;
	UINT8	Lun;
	UINT8	Command;
	UINT8	CommandData[20]={0};
	UINT8	CommandDataSize;
	UINT8	ResponseData[100]={0};
	UINT8	ResponseDataSize;
	UINT8	Counter;
	#endif
#endif	

	A7Command(0x000003);                   
	A7Command(0x000025);               
	A7Command(0x000002);
    
  if(EmuDeviceExisted()) 	// if Emulator device existed
	{    

	Exit_Flag = 0;
	A7husetbios(0x30, 0x25);

	TempSpiStatus = hugetbios8(0x30);

        do{

			TempSpiStatus = hugetbios8(0x30);
			
////////////////////////////// Read Data /////////////////////////////////////////////////////

			if (hugetbios8(0x40)!=0)					// PCI_REQUEST
			{
				a7printf("***************************************************************************\n");
				PCIBus=hugetbios8(0x41);					// PCI_BUS
				PCIDev=hugetbios8(0x42);					// PCI_DEV
				PCIFun=hugetbios8(0x43);					// PCI_FUN
				PCIReg=hugetbios16(0x44);					// PCI_Reg
				SMC_SDPcie(PCIBus, PCIDev, PCIFun, PCIReg);
				if(TempSpiStatus!=0) A7husetbios(0x40, 0x00);
				a7printf("***************************************************************************\n");
			}

			if (hugetbios8(0x50)!=0)					// IO_REQUEST
			{
				a7printf("***************************************************************************\n");
				IOBase=hugetbios16(0x51);					// IO_BaseAddress
				SDIO(IOBase);
				if(TempSpiStatus!=0) A7husetbios(0x50,0x00);
				a7printf("***************************************************************************\n");
			}

			if (hugetbios8(0x60)!=0)					// INDEXIO_REQUEST
			{
				a7printf("***************************************************************************\n");
				IndexAdd=hugetbios8(0x61);					// INDEXIO_INDEXAddress
				SDGetIndexIO(IndexAdd);
				if(TempSpiStatus!=0) A7husetbios(0x60,0x00);
				a7printf("***************************************************************************\n");
			}

			if (hugetbios8(0x70)!=0)					// CMOS_REQUEST
			{
				a7printf("***************************************************************************\n");
				SDGetIndexIO(0x70);
				if(TempSpiStatus!=0) A7husetbios(0x70,0x00);
				a7printf("***************************************************************************\n");
			}

			if (hugetbios8(0x80)!=0)					// MEMORY_REQUEST
			{
				a7printf("***************************************************************************\n");
				MemBase=hugetbios32(0x81);				// Memory_Address
				SDMem(MemBase);
				if(TempSpiStatus!=0) A7husetbios(0x80,0x00);
				a7printf("***************************************************************************\n");
			}

			if (hugetbios8(0x48)!=0)		// 0x48==>> PCI_List_REQUEST
			{
				a7printf("***************************************************************************\n");
				SDGetPCIList();
				if(TempSpiStatus!=0) A7husetbios(0x48, 0x00);
				a7printf("***************************************************************************\n");
			}

			if (hugetbios8(0x67)!=0)					// INDEXIO2_REQUEST
			{
				a7printf("***************************************************************************\n");
				IndexAdd16 = hugetbios16(0x68);				// INDEXIO2_INDEXAddress
				SMC_SDGetIndexIO16(IndexAdd16);
				if(TempSpiStatus!=0) A7husetbios(0x67, 0x00);
				a7printf("***************************************************************************\n");
			}

			if (hugetbios8(0xC8)!=0)					// READMSR_REQUEST
			{
      #if (A7_MRC_DEBUG == 1)
				a7printf("***************************************************************************\n");
				Msr = hugetbios32(0xC9);
				MsrData = ReadMsr(Msr);

				MsrData_High_32bits = (UINT32)( Shr64(MsrData, 32 ));
				MsrData_Low_32bits = (UINT32)(MsrData & 0x00000000FFFFFFFF);

				a7printf("Msr : %x\n", Msr);
				a7printf("MsrData_High_32bits : %x\n", MsrData_High_32bits);
				a7printf("MsrData_Low_32bits  : %x\n", MsrData_Low_32bits);
				if(TempSpiStatus!=0) A7husetbios(0xC8, 0x00);
				a7printf("***************************************************************************\n");
#endif
			}

			#if A7Printf_IPMI_SUPPORT
			#if IPMI_SUPPORT
			if (hugetbios8(0xB8)!=0)					// IPMISENDCOMMAND_REQUEST
			{

				a7printf("***************************************************************************\n");

				NetFunction			=	hugetbios8(0xD9);
				Lun					=	hugetbios8(0xDA);
				Command				=	hugetbios8(0xDB);
				CommandDataSize		=	hugetbios8(0xDC);
				ResponseDataSize	=	hugetbios8(0xDD);

				if(CommandDataSize<21)
				{
					for( Counter=0 ; Counter<CommandDataSize ; Counter++ )
					{
						if(Counter<10) CommandData[Counter] = hugetbios8( 0xE6 + Counter );
						else if(Counter>10) CommandData[Counter] = hugetbios8( 0xF6 + Counter - 10 );
					}
					SMC_IpmiSendCommand(NetFunction, Lun, Command, CommandData, CommandDataSize, ResponseData, &ResponseDataSize);
				}
				else a7printf("Command Size is too large.\n");

				if(TempSpiStatus!=0) A7husetbios(0xB8, 0x00);
				a7printf("***************************************************************************\n");
			}
			#endif
      #endif		
////////////////////////////// Set Data /////////////////////////////////////////////////////

			if (hugetbios8(0x90)==0x25)				// Set IO_Base_REQUEST     //alan
			{
				SMC_SDSetIO();
			}

			if (hugetbios8(0xA0)==0x25)				// Set INDEXIO_REQUEST     //alan
			{
				SMC_SDSetINDEXIO();
			}

			if (hugetbios8(0xB0)==0x25)				// Set CMOS_REQUEST        //alan
			{
				SMC_SDSetCMOS();
			}

			if (hugetbios8(0xC0)==0x25)				// Set PCI_Device_REQUEST  //alan
			{
				SMC_SDSetPCI();
			}

			if (hugetbios8(0xD0)==0x25)				// Set Memory_REQUEST      //alan
			{
				SMC_SDSetMem();
			}

			if (hugetbios8(0xA6)==0x25)				// Set INDEXIO2_REQUEST    //alan
			{
				SMC_SDSetINDEXIO16();
			}

			if (hugetbios8(0x33)==0x025)				// Set WRIEMSR_REQUEST   //alan
			{
				SMC_Write_MSR();
			}

			if(hugetbios8(0x30)==0) 				// check the state of pause
			{
				Exit_Flag = 1;
				a7printf("Continue! *\n");
			}

        } while (Exit_Flag == 0); 				// Wait for BIOS_PAUSE_REQUEST to be unchecked.
    }//if(EmuDeviceExisted())
}
//#endif	//A7_Command_SUPPORT

VOID A7StatusReport(void *PeiServices, char *String)
{
    a7printf ("%s", String);
	if ((String[0]=='s') && (String[1]=='m') && (String[2]=='c') && (String[3]=='d') && (String[4]=='b') && (String[5]=='g'))
	{
		SMCDebug2();
	}
}

EFI_STATUS A7CheckPointStatus(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value
)
{
	SMC_PauseForKeyin2();
    
    return EFI_SUCCESS;
}
//;SameDiskDebugSampleCode Mon 06-14-2010 For support SPI Hyper-Terminal debug tool.<< [a]-

