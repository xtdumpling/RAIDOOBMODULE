/*************************************************************************\
*  Module Name: iolib.h
*
*  Descriprtion:
*
*  Date:
\**************************************************************************/
/* IOType Define */
#define	NormalIO	0
#define	IndexIO		1

/* ProtoType Type */
VOID SetReg (USHORT, UCHAR, UCHAR);
VOID SetReg2 (USHORT, UCHAR);
UCHAR GetReg (USHORT, UCHAR);
UCHAR GetReg2 (USHORT);
VOID SetIndexReg (USHORT, UCHAR, UCHAR, UCHAR);
VOID SetIndexReg2 (USHORT, UCHAR, UCHAR);
UCHAR GetIndexReg (USHORT, UCHAR, UCHAR);
UCHAR GetIndexReg2 (USHORT, UCHAR);
VOID Delay15us (VOID);

