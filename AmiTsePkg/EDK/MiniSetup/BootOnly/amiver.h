//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/amiver.h $
//
// $Author: Madhans $
//
// $Revision: 3 $
//
// $Date: 2/19/10 1:01p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file amiver.h
    This file contains version macros.

**/

#ifndef _AMIVER_H_
#define	_AMIVER_H_

#define AMI_VERSION_SIGNATURE	{'$','A','P','V'}

#pragma pack(1)
typedef struct
{
	CHAR8	signature[4];
	UINT8	major;
	UINT8	minor;
	UINT16	build;
	CHAR8	name[4];
	UINT8	reserved[4];
}
AMI_VERSION;
#pragma pack()

#ifdef __cplusplus
extern "C"
{
#define	AMI_SET_VERSION(major,minor,build,sign)	\
AMI_VERSION gVersion_##sign = { \
	AMI_VERSION_SIGNATURE, \
	major, \
	minor, \
	build, \
    ##sign, \
	{ 0, 0, 0, 0 } \
}

}
#else

#define	AMI_SET_VERSION(major,minor,build,sign)	\
AMI_VERSION gVersion_##sign = { \
	AMI_VERSION_SIGNATURE, \
	major, \
	minor, \
	build, \
    #sign, \
	{ 0, 0, 0, 0 } \
}


#endif //__cplusplus

#define	AMI_GET_VERSION_MAJOR(ver)	((ver).major)
#define	AMI_GET_VERSION_MINOR(ver)	((ver).minor)
#define	AMI_GET_VERSION_BUILD(ver)	((ver).build)
#define	AMI_GET_VERSION_NAME(ver)	((ver).name)

#define	AMI_VERSION_STRUCT(sign)	(gVersion_##sign)

#define	AMI_CHECK_VERSION(ver) \
	( ((ver).major != 0) || ((ver).minor != 0) )

#endif /* _AMIVER_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
