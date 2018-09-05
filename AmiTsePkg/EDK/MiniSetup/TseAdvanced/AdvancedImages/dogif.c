//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/dogif.c $
//
// $Author: Arunsb $
//
// $Revision: 7 $
//
// $Date: 3/28/11 9:35p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file dogif.c
    This file contains code for gif image handling

**/
#include<minisetup.h>

// definitions
#undef DEBUG
#undef TRUE
#undef FALSE

#define DEBUG 0
#define TIANO 1
#if TIANO
#define TRUE    1
#else
#define TRUE    (-1)
#endif
#define FALSE   0

#define MaxColors       256             /* rev 87a */

struct ColorEntry
    {
    unsigned char blue, green, red, dummy;
    };

#if TIANO
#define short int
//#define unsigned
#define huge
#define far
unsigned char * blitbuf = 0;
static struct ColorEntry * pColorMap = 0;
#else
#define _FP_SEG(fp) (*((unsigned __far *)&(fp)+1))
#define _FP_OFF(fp) (*((unsigned __far *)&(fp)))
#define _MK_FP(seg, offset) (void __far *)(((unsigned long)seg << 16) \
    + (unsigned long)(unsigned)offset)
#endif

// variables
#if TIANO
int screen_width = 800;
int screen_height = 600;
#else
int screen_width = 640;
int screen_height = 480;
#endif

unsigned char huge * pbuf = 0;
unsigned long lenbuf = 0;
unsigned long firstframeadd = 0;    
static char GIF87signature[] = "GIF87a";
static char GIF89signature[] = "GIF89a";
static char GIFversion = 0;
#define GIF87 1
#define GIF89 2
static struct ColorEntry GlobalColorMap[MaxColors] = {{0}};
static struct ColorEntry LocalColorMap[MaxColors] = {{0}};
static unsigned int BackdropWidth = 0;
static unsigned int BackdropHeight = 0;
static unsigned int LeftMargin = 0;
static unsigned int TopMargin = 0;
static unsigned int ColorRez = 0;
static unsigned int FillColor = 0;
#if TIANO
static short DefaultNumColors = 0;
#else
static unsigned int DefaultNumColors = 0;
#endif
static unsigned int LeftEdge = 0;
static unsigned int TopEdge = 0;
static unsigned int Width = 0;
static unsigned int Height = 0;
static unsigned int Interlaced = 0;
#if TIANO
static short LocalNumColors = 0;
#else
static unsigned int LocalNumColors = 0;
#endif
static unsigned int RightEdge = 0;
static unsigned int BottomEdge = 0;
static unsigned int InterlacePass = 0;
static unsigned int X = 0;
static unsigned int Y = 0;
static short BaseLine[5] = {0, 4, 2, 1, 0};
static short LineOffset[5] = {8, 8, 4, 2, 0};

unsigned char huge * pcurbuf = 0;
static unsigned long unreadbuf = 0;

static unsigned int CurrentBank = 0;
static unsigned long PixelAdd = 0;

static int TransparentColor = -1;
static int GlobalColorMapActive = FALSE;

static int UnexpectedError = FALSE;

// functions
#if TIANO
unsigned long dofirst( unsigned char *, unsigned long );
unsigned long donext( unsigned char * );
#else
unsigned long far _loadds dofirst(void);
unsigned long far _loadds donext(void);
#endif
//static int _inline readbuf(void);
//static void _inline writepixel(unsigned char);
static int  readbuf(void);
static void writepixel(unsigned char);
static void setbank(unsigned int);
static short ReadScreenDesc(void);
static short ReadImageDesc(void);
static unsigned long ReadGraphicControlExt(void);
#if DEBUG
static short ReadCommentExt(void);
static short ReadPlainTextExt(void);
static short ReadApplicationExt(void);
#endif
static short SkipObject(void);
static void SetPalette( struct ColorEntry [], unsigned int );
static short Expand_Data(void);

extern UINT16 gGifSrcX;
extern UINT16 gGifSrcY;
extern UINT16 gGifDestX;
extern UINT16 gGifDestY;
extern BOOLEAN GifImageFlag;

// Expander
struct code_entry
    {
    short prefix;                       /* prefix code */
    char suffix;                        /* suffix character */
    char stack;
    };
static short code_size = 0;
static short clear_code = 0;
static short eof_code = 0;
static short first_free = 0;
static short bit_offset = 0;
static short byte_offset = 0;
static short bits_left = 0;
static short max_code = 0;
static short free_code = 0;
static short old_code = 0;
static short input_code = 0;
static short code = 0;
static short suffix_char = 0;
static short final_char = 0;
static short bytes_unread = 0;
static unsigned char code_buffer[64] = {0};
static struct code_entry code_table[4096] = {0};
static short mask[12] =
    {0x001, 0x003, 0x007, 0x00F,
     0x01F, 0x03F, 0x07F, 0x0FF,
     0x1FF, 0x3FF, 0x7FF, 0xFFF};
static unsigned int endofdata = 0;

#if TIANO
unsigned long dofirst( unsigned char * filemem, unsigned long filememlen )
#else
unsigned long far _loadds dofirst( )
#endif
{

#if TIANO
	pbuf = filemem;
	lenbuf = filememlen;
#endif
	pcurbuf = pbuf;
	unreadbuf = lenbuf;
	if (!ReadScreenDesc())
	{
	    /* Invalid GIF dataset */
#if DEBUG
		printf("\nnot a GIF file - 1\n");
#endif
		return 0xffffffffL;
	}
#if DEBUG
    printf("\nscreen width = %d, screen height = %d\n", BackdropWidth, BackdropHeight);
    printf("# of colors = %d out of %ld\n", DefaultNumColors, (long)(1L << (3*ColorRez)));
    printf("fill color index = %d\n", FillColor);
#endif
	
	SetPalette( GlobalColorMap, DefaultNumColors );
	GlobalColorMapActive = TRUE;

#if TIANO
	LeftMargin = 0;
	TopMargin = 0;
	screen_width = BackdropWidth;
	screen_height = BackdropHeight;
	firstframeadd	=	(unsigned long)pcurbuf;
    return (BackdropWidth << 16) | BackdropHeight;
#else
	LeftMargin = ( screen_width - BackdropWidth ) / 2;
	TopMargin = ( screen_height - BackdropHeight ) / 2;
#if DEBUG
		    printf("LeftMargin = %d, TopMargin = %d\n", LeftMargin, TopMargin);
#endif
    if (FillColor > 0)
	{
		/* Set the default color palette.  Flood the backdrop region with
		   the FillColor. */
		setbank( 0 );
	    Width = BackdropWidth;
		Height = BackdropHeight;
	    LeftEdge = 0;
	    TopEdge = 0;
		RightEdge = LeftEdge + Width - 1;
		BottomEdge = TopEdge + Height - 1;
	    Interlaced = 0;
#if DEBUG
		    printf("LeftEdge = %d, RightEdge = %d, Width = %d\n", LeftEdge, RightEdge, Width);
		    printf("TopEdge = %d, BottomEdge = %d, Height = %d\n", TopEdge, BottomEdge, Height);
#endif
		X = LeftEdge;
		Y = TopEdge;
		InterlacePass = 0;
		{
			PixelAdd = ( Y + TopMargin ) * (unsigned long)screen_width + ( X + LeftMargin );
			if( CurrentBank != (PixelAdd >> 16) )
				setbank((unsigned int)(PixelAdd >> 16));
		}
		for( ; Y <= BottomEdge ; Y++ )
			for( ; X <= BottomEdge ; X++ )
				writepixel( (unsigned char)FillColor );
	}
	return donext();
#endif
}

#if TIANO
unsigned long donext( unsigned char * BlitBuf )
#else
unsigned long far _loadds donext( )
#endif
{
	unsigned int Done;
    short Id;           /* object identifier */
    short Status;       /* return status code */
	unsigned long ret = 0;

#if TIANO
	blitbuf = BlitBuf;
#endif
	Done = FALSE;
    /* Now display one or more GIF objects */
    while(!Done)
	{
		switch (readbuf())
	    {
	    case ';':
			/* End of the GIF dataset */
			Done = TRUE;
			break;

	    case ',':
			/* Start of an image object.
			   Read the image description. */
			if (!ReadImageDesc())
			{
			    /* Invalid GIF dataset */
#if DEBUG
				printf("\nnot a GIF file - 2\n");
#endif
				ret = 0xffffffffL;
				Done = TRUE;
				break;
			}
			/* Setup the color palette for the image */
			if (LocalNumColors > 0)
		    {
			    /* Change the palette table */
				SetPalette( LocalColorMap, LocalNumColors );
				GlobalColorMapActive = FALSE;
		    }
			else if (DefaultNumColors > 0)
		    {
			    /* Reset the palette table back to the default setting */
				if( !GlobalColorMapActive )
			    {
					SetPalette( GlobalColorMap, DefaultNumColors );
					GlobalColorMapActive = TRUE;
			    }
		    }

			X = LeftEdge;
			Y = TopEdge;
			RightEdge = LeftEdge + Width - 1;
			BottomEdge = TopEdge + Height - 1;
			
			///Need to update the image based on the current frame coordinates.
			gGifSrcX = (UINT16)LeftEdge;
			gGifSrcY = (UINT16)TopEdge;
			if(Width) gGifDestX = (UINT16)Width-1;
			if(Height) gGifDestY = (UINT16)Height-1;
			GifImageFlag=1;

#if DEBUG
		    printf("LeftEdge = %d, RightEdge = %d, Width = %d\n", LeftEdge, RightEdge, Width);
		    printf("TopEdge = %d, BottomEdge = %d, Height = %d\n", TopEdge, BottomEdge, Height);
#endif
			InterlacePass = 0;
			{
				PixelAdd = ( Y + TopMargin ) * (unsigned long)screen_width + ( X + LeftMargin );
				if( CurrentBank != (PixelAdd >> 16) )
					setbank((unsigned int)(PixelAdd >> 16));

			}

			Status = Expand_Data();

			if (Status != 0)
		    {
		    /* Error expanding the raster image */
#if DEBUG
				printf("\nnot a GIF file - 3\n");
#endif
				ret = 0xffffffffL;
				Done = TRUE;
				break;
			}
			break;

	    case '!':
			/* Start of an extended object (not in rev 87a) */
			Id = readbuf(); /* Get the object identifier */
			if (Id < 0)
		    {
			    /* Error reading object identifier */
#if DEBUG
				printf("\nnot a GIF file - 4\n");
#endif
				ret = 0xffffffffL;
				Done = TRUE;
				break;
			}
			switch ( Id )
		    {
			case 0xF9:
				ret = ReadGraphicControlExt();
				if( ret != 0 )
			    {
#if DEBUG
					if (ret == 0xffffffffL)
						printf("\nnot a GIF file - 9\n");
#endif
				    Done = TRUE;
				}
				break;
#if DEBUG
			case 0xFE:
				if (!ReadCommentExt())
			    {
					printf("\nnot a GIF file - 6\n");
					ret = 0xffffffffL;
				    Done = TRUE;
				}
				break;
			case 0x01:
				if (!ReadPlainTextExt())
			    {
					printf("\nnot a GIF file - 7\n");
					ret = 0xffffffffL;
				    Done = TRUE;
				}
				break;
			case 0xFF:
				if (!ReadApplicationExt())
			    {
					printf("\nnot a GIF file - 8\n");
					ret = 0xffffffffL;
				    Done = TRUE;
				}
				break;
#endif
			default:
				/* Since there no extented objects defined in rev 87a, we
				   will just skip over them.  In the future, we will handle
				   the extended object here. */
				if (!SkipObject())
			    {
#if DEBUG
					printf("\nnot a GIF file - 5\n");
#endif
					ret = 0xffffffffL;
				    Done = TRUE;
				}
				break;
			}
			break;

	    default:
			/* Error */
#if DEBUG
			printf("\nnot a GIF file - 10\n");
#endif
			ret = 0xffffffffL;
			Done = TRUE;
			break;
	    }
	}
#if DEBUG
	printf("offset = %lx\n", lenbuf-unreadbuf);
	printf("wait = %ld\n", ret);
#endif
	if( (ret > 0) && (ret < 100) )
		ret = 100;
	return ret;
}

/**
    Read the next byte from buffer

    @param VOID

    @retval 0 .. 255        the byte
        -1              end of file

**/
//static int _inline readbuf( )
static int  readbuf( )
{

	if( unreadbuf == 0 )
		return -1;

    unreadbuf--;
#if TIANO
    return (((int)*pcurbuf++)&0xff);
#else
    return (int) *pcurbuf++;
#endif
}

/**
    Write a pixel at (X,Y) on the screen.

    @param unsigned char Pixel

    @retval VOID

**/
//static void _inline writepixel(unsigned char Pixel)
static void writepixel(unsigned char Pixel)
{
	unsigned char far * fp;
    // Display the pixel on the screen
	if( Pixel != TransparentColor )
	{
#if TIANO
		fp = blitbuf + PixelAdd * 4;
		*fp++ = pColorMap[Pixel].blue;
		*fp++ = pColorMap[Pixel].green;
		*fp++ = pColorMap[Pixel].red;
		*fp++ = 0;
#else
	fp = _MK_FP( 0xa000, PixelAdd );
	*fp = Pixel;
#endif
	}
    // Advance the point
    X++;
	PixelAdd++;
    if (X > RightEdge)
	{
		X = LeftEdge;

		if (Interlaced)
	    {
			Y += LineOffset[InterlacePass];
		    if (Y > BottomEdge)
			{
				InterlacePass++;
				Y = TopEdge + BaseLine[InterlacePass];
			}
	    }
		else
		    Y++;
		PixelAdd = ( Y + TopMargin ) * (unsigned long)screen_width + ( X + LeftMargin );
	}
	if( CurrentBank != (PixelAdd >> 16) )
		setbank((unsigned int)(PixelAdd >> 16));
}

static void setbank(unsigned int bank)
{
	CurrentBank = bank;
#if TIANO == 0
	_asm
	{
		mov     ax,4f05h
		mov bx,0
		mov     dx,CurrentBank
		int 10h
		/*mov retvesa,ax*/
	}
	
	_asm
	{
		mov     ax,4f05h
		mov bx,1
		mov     dx,CurrentBank
		int 10h
		/*mov retvesa,ax*/
	}
	
#endif
}

/**
    Read the signature, the screen description, and the optional 
    default color map.

    @param VOID

    @retval TRUE/FALSE

**/
static short ReadScreenDesc( )
{
    unsigned char Buffer[16];

    short
	I, J,
	Status,
	HaveColorMap,
	NumPlanes;

    for (I = 0; I < 13; I++)
	{
	Status = readbuf();
	if (Status < 0) return FALSE;
	Buffer[I] = (unsigned char) Status;
	}

    for (I = 0; I < 6; I++)
	if (Buffer[I] != GIF87signature[I])
		break;
	if (I >= 6)
		GIFversion = GIF87;
	else
	{
    for (I = 0; I < 6; I++)
	if (Buffer[I] != GIF89signature[I])
		break;
	if (I >= 6)
		GIFversion = GIF89;
	else
	    return FALSE;
	}

    BackdropWidth = Buffer[6] | Buffer[7] << 8;
    BackdropHeight = Buffer[8] | Buffer[9] << 8;
    NumPlanes = (Buffer[10] & 0x07) + 1;
    ColorRez = ((Buffer[10] & 0x70) >> 4) + 1;
    HaveColorMap = (Buffer[10] & 0x80) != 0;
    DefaultNumColors = 1 << NumPlanes;
    FillColor = Buffer[11];

    if (HaveColorMap)
	{
	for (I = 0; I < DefaultNumColors; I++)
	    {
	    for (J = 0; J < 3; J++)
		{
		Status = readbuf();
		if (Status < 0) return FALSE;
		Buffer[J] = (unsigned char) Status;
		}

	    if (I < MaxColors)
		{
		GlobalColorMap[I].red = Buffer[0];
		GlobalColorMap[I].green = Buffer[1];
		GlobalColorMap[I].blue = Buffer[2];
		}
	    }
	}
    else
	DefaultNumColors = 0;

    return TRUE;
}

/**
    Read the image description and the optional color map.

    @param VOID

    @retval TRUE/FALSE

**/
static short ReadImageDesc( )
{
    unsigned char Buffer[16];

    short
	I, J,
	NumPlanes,
	HaveColorMap,
	Status;

    for (I = 0; I < 9; I++)
	{
	if ((Status = readbuf()) < 0) return FALSE;
	Buffer[I] = (unsigned char) Status;
	}

    LeftEdge = Buffer[0] | Buffer[1] << 8;
    TopEdge = Buffer[2] | Buffer[3] << 8;
    Width = Buffer[4] | Buffer[5] << 8;
    Height = Buffer[6] | Buffer[7] << 8;
    NumPlanes = (Buffer[8] & 0x07) + 1;
    LocalNumColors = 1 << NumPlanes;
    Interlaced = (Buffer[8] & 0x40) != 0;
    HaveColorMap = (Buffer[8] & 0x80) != 0;

    if (HaveColorMap)
	{
	for (I = 0; I < LocalNumColors; I++)
	    {
	    for (J = 0; J < 3; J++)
		{
		if ((Status = readbuf()) < 0) return FALSE;
		Buffer[J] = (unsigned char) Status;
		}

	    if (I < MaxColors)
		{
		LocalColorMap[I].red = Buffer[0];
		LocalColorMap[I].green = Buffer[1];
		LocalColorMap[I].blue = Buffer[2];
		}
	    }
	}
    else
	LocalNumColors = 0;

    return TRUE;
}

static unsigned long ReadGraphicControlExt()
{
    unsigned char Buffer[6];
    short I;
    short Status;
    short HaveTransparentColor;
	unsigned long timewait;

#if DEBUG
	printf("Graphic Control Extension : ");
#endif
    for (I = 0; I < 6; I++)
	{
	if ((Status = readbuf()) < 0) return 0xffffffffL;
	Buffer[I] = (unsigned char) Status;
	}
	timewait = (Buffer[2] | Buffer[3] << 8)*10;
    HaveTransparentColor = (Buffer[1] & 0x01) != 0;
    if (HaveTransparentColor)
		TransparentColor = Buffer[4];
#if DEBUG
	printf("%ld\n", timewait);
#endif
    return timewait;
}

#if DEBUG
static short ReadCommentExt()
{
    short Count;
	short c;

	printf("Comment Extension : ");
    while ((Count = readbuf()) > 0)
    {
	printf("\n#");
	do
    {
	    if ((c = readbuf()) < 0)
			return FALSE;
		printf("%c", c);
    }while (--Count > 0);
	printf("#");
    }
    if (Count < 0)
		return FALSE;
	printf("\n");
	return TRUE;
}

static short ReadPlainTextExt()
{
    unsigned char Buffer[13];
    short I;
    short Status;
    short Count;
	short c;

	printf("Plain Text Extension : ");
    for (I = 0; I < 13; I++)
	{
	if ((Status = readbuf()) < 0) return FALSE;
	Buffer[I] = (unsigned char) Status;
	}
    while ((Count = readbuf()) > 0)
    {
	printf("\n#");
	do
    {
	    if ((c = readbuf()) < 0)
			return FALSE;
		printf("%c", c);
    }while (--Count > 0);
	printf("#");
    }
    if (Count < 0)
		return FALSE;
	printf("\n");
	return TRUE;
}

static short ReadApplicationExt()
{
    unsigned char Buffer[13];
    short I;
    short Status;
    short Count;
	short c;

	printf("Application Extension : ");
    for (I = 0; I < 12; I++)
	{
	if ((Status = readbuf()) < 0) return FALSE;
	Buffer[I] = (unsigned char) Status;
	}
    for (I = 1; I < 9; I++)
		printf("%c", Buffer[I]);
	printf(" ");
    for (I = 9; I < 12; I++)
		printf("%c", Buffer[I]);
    while ((Count = readbuf()) > 0)
    {
	printf("\n%2.2X", Count);
	do
    {
	    if ((c = readbuf()) < 0)
			return FALSE;
		printf("%2.2X", c);
    }while (--Count > 0);
   }
    if (Count < 0)
		return FALSE;
	printf("\n");
	return TRUE;
}
#endif

/**
    Skip over an extended GIF object.

    @param VOID

    @retval TRUE/FALSE

**/
static short SkipObject()
{
    short Count;

    while ((Count = readbuf()) > 0)
	do
    {
	    if (readbuf() < 0)
			return FALSE;
    }while (--Count > 0);
    if (Count < 0)
		return FALSE;
	return TRUE;
}

void SetPalette( struct ColorEntry ColorMap[], unsigned int num_colors )
{
#if TIANO
	pColorMap = ColorMap;
//	num_colors; no effect statement
	if (num_colors) {};
	return;
#else
	int i;
	int bitofcolor;
	int retvesa;
	unsigned int segval, offval;
	unsigned char far * fp;

	_asm
	{
		mov     ax,4f08h
		mov bx,0001h
		int 10h
		mov retvesa,ax
		mov     bitofcolor,bx
	}
	if(retvesa != 0x004f)
		bitofcolor = 8;
	else
		bitofcolor >>= 8;
	if( 8 != bitofcolor )
	{
		if( 8 > bitofcolor )
		    for (i = 0; i < num_colors; i++)
			{
				ColorMap[i].red >>= (8 - bitofcolor);
				ColorMap[i].green >>= (8 - bitofcolor);
				ColorMap[i].blue >>= (8 - bitofcolor);
			}
		else
		    for (i = 0; i < num_colors; i++)
			{
				ColorMap[i].red <<= (bitofcolor - 8);
				ColorMap[i].green <<= (bitofcolor - 8);
				ColorMap[i].blue <<= (bitofcolor - 8);
			}
	}
	fp = (unsigned char far *)ColorMap;
	segval = _FP_SEG( fp );
	offval = _FP_OFF( fp );
	_asm
	{
		push di
		mov     ax,4f09h
		mov bl,0
		mov cx,num_colors
		mov     dx,0
		mov di,segval
		mov     es,di
		mov     di,offval
		int 10h
		mov retvesa,ax
		pop di
	}
	
#endif
}

// Expander
#if TIANO
static int  init_table(short min_code_size)
#else
static int  init_table(min_code_size)
    short min_code_size;
#endif
    {
    code_size = min_code_size + 1;
    clear_code = 1 << min_code_size;
    eof_code = clear_code + 1;
    first_free = clear_code + 2;
    free_code = first_free;
    max_code = 1 << code_size;
    return 0; //WARN FIX
    }
static int read_code()
    {
    short bytes_to_move, i, ch;
    long temp;

    byte_offset = bit_offset >> 3;
    bits_left = bit_offset & 7;

    if (byte_offset >= 61)
	{
	bytes_to_move = 64 - byte_offset;

	for (i = 0; i < bytes_to_move; i++)
	    code_buffer[i] = code_buffer[byte_offset + i];

	while (i < 64)
	    {
	    if (bytes_unread == 0)
		{
		/* Get the length of the next record. A zero-length record
		 * denotes "end of data".
		 */
		if( endofdata )
			break;

		if( unreadbuf > 0 )
		{
			bytes_unread = *pcurbuf++;
		    unreadbuf--;
		}
		else
		{
			UnexpectedError = TRUE;
			return -1;
		}

	    if (bytes_unread == 0)      /* end of data */
		{
			endofdata = TRUE;
			break;
		}
		}

		if( unreadbuf > 0 )
		{
			ch = *pcurbuf++;
		    unreadbuf--;
		}
		else
		{
			UnexpectedError = TRUE;
			return -1;
		}

#if TIANO
		code_buffer[i++] = (char)ch;
#else
		code_buffer[i++] = ch;
#endif
	    bytes_unread--;
	    }

	bit_offset = bits_left;
	byte_offset = 0;
	}

    bit_offset += code_size;
    temp = (long) code_buffer[byte_offset]
	| (long) code_buffer[byte_offset + 1] << 8
	| (long) code_buffer[byte_offset + 2] << 16;

    if (bits_left > 0)
	temp >>= bits_left;

    return temp & mask[code_size - 1];
    }

/**
    Decompress a LZW compressed data stream.

    @param VOID

    @retval 0       OK
        -1      expected end-of-file
        -2      cannot allocate memory
        -3      bad "min_code_size"
        < -3    error status from the get_byte or put_byte routine

**/
static short Expand_Data( )
{
#if TIANO == 0
    short i;
#endif
    short sp;                           /* stack ptr */
#if TIANO == 0
    short status;
#endif
    short min_code_size;

    /* Get the minimum code size (2 to 9) */

	if( unreadbuf > 0 )
	{
		min_code_size = *pcurbuf++;
	    unreadbuf--;
	}
	else
		return -4;

	if (min_code_size < 2 || min_code_size > 9)
		return -3;

    init_table(min_code_size);
    sp = 0;
    bit_offset = 64*8;                  /* force "read_code" to start a new */
    bytes_unread = 0;                   /* record */
	endofdata = FALSE;

    while ((code = read_code()) != eof_code)
	{
	if( UnexpectedError )
		return -4;
	if (code == clear_code)
	    {
	    init_table(min_code_size);
	    code = read_code();
		if( UnexpectedError )
			return -4;
	    old_code = code;
	    suffix_char = code;
	    final_char = code;
	    //(*put_byte)(suffix_char);
		writepixel((unsigned char)suffix_char);
	    }
	else
	    {
	    input_code = code;

	    if (code >= free_code)
		{
		code = old_code;
#if TIANO
		code_table[sp++].stack = (char)final_char;
#else
		code_table[sp++].stack = final_char;
#endif
		}

	    while (code >= first_free)
		{
		code_table[sp++].stack = code_table[code].suffix;
		code = code_table[code].prefix;
		}

	    final_char = code;
	    suffix_char = code;
#if TIANO
	    code_table[sp++].stack = (char)final_char;
#else
	    code_table[sp++].stack = final_char;
#endif

	    while (sp > 0)
		//(*put_byte)(code_table[--sp].stack);
		writepixel((unsigned char)code_table[--sp].stack);

#if TIANO
		code_table[free_code].suffix = (char)suffix_char;
#else
		code_table[free_code].suffix = suffix_char;
#endif
		code_table[free_code].prefix = old_code;
	    free_code++;
	    old_code = input_code;

	    if (free_code >= max_code)
		if (code_size < 12)
		    {
		    code_size++;
		    max_code <<= 1;
		    }
	    }
	}

    return 0;
    }

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
