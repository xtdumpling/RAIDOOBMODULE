/*************************************************************************\
*  Module Name: memlib.h
*
*  Descriprtion:
*
*  Note: .386 only
*
*  Date:
\**************************************************************************/

/* Macro Define */
#define ReadDB(baseaddr,offset)		*(volatile BYTE *)((ULONG)(baseaddr)+(ULONG)(offset))
#define ReadDD(baseaddr,offset)      	*(volatile ULONG *)((ULONG)(baseaddr)+(ULONG)(offset))
#define ReadDW(baseaddr,offset)      	*(volatile USHORT *)((ULONG)(baseaddr)+(ULONG)(offset))
#define WriteDB(baseaddr,offset,data)	*(BYTE *)((ULONG)(baseaddr)+(ULONG)(offset)) = (BYTE)(data)
#define WriteDD(baseaddr,offset,data)  	*(ULONG *)((ULONG)(baseaddr)+(ULONG)(offset))=(ULONG)(data)
#define WriteDW(baseaddr,offset,data) 	*(USHORT *)((ULONG)(baseaddr)+(ULONG)(offset))=(USHORT)(data)

#define ReadDB2(baseaddr,offset, mask)		*(volatile BYTE *)((ULONG)(baseaddr)+(ULONG)(offset)) & (ULONG)mask
#define ReadDD2(baseaddr,offset, mask)      	*(volatile ULONG *)((ULONG)(baseaddr)+(ULONG)(offset)) & (ULONG)mask
#define ReadDW2(baseaddr,offset, mask)      	*(volatile USHORT *)((ULONG)(baseaddr)+(ULONG)(offset)) & (ULONG)mask
#define WriteDB2(baseaddr,offset,mask, data)	*(BYTE *)((ULONG)(baseaddr)+(ULONG)(offset)) =(*(BYTE *)((ULONG)(baseaddr)+(ULONG)(offset)) & (ULONG)(mask)) | (ULONG)(data)
#define WriteDD2(baseaddr,offset,mask, data)  	*(ULONG *)((ULONG)(baseaddr)+(ULONG)(offset))=(*(ULONG *)((ULONG)(baseaddr)+(ULONG)(offset)) & (ULONG)(mask)) | (ULONG)(data)
#define WriteDW2(baseaddr,offset,mask, data) 	*(USHORT *)((ULONG)(baseaddr)+(ULONG)(offset))=(*(USHORT *)((ULONG)(baseaddr)+(ULONG)(offset)) & (ULONG)(mask)) | (ULONG)(data)

#define m64KBytesAlignment(x)		(((x) + 0x00010000) & 0xFFFF0000)
#define m16KBytesAlignment(x)		(((x) + 0x00003FFF) & 0xFFFFC000)
#define m02KBytesAlignment(x)           (((x) + 0x000007FF) & 0xFFFFF800)
#define m64BytesAlignment(x)            (((x) + 0x0000003F) & 0xFFFFFFC0)
#define m32BytesAlignment(x)            (((x) + 0x0000001F) & 0xFFFFFFE0)
#define m16BytesAlignment(x)            (((x) + 0x0000000F) & 0xFFFFFFF0)
#define m08BytesAlignment(x)            (((x) + 0x00000007) & 0xFFFFFFF8)
#define m04BytesAlignment(x)            (((x) + 0x00000003) & 0xFFFFFFFC)
#define m02BytesAlignment(x)            (((x) + 0x00000001) & 0xFFFFFFFE)
