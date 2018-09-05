#//**********************************************************************//
#//**********************************************************************//
#//**                                                                  **//
#//**        (C)Copyright 1985-2003, American Megatrends, Inc.         **//
#//**                                                                  **//
#//**                       All Rights Reserved.                       **//
#//**                                                                  **//
#//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **//
#//**                                                                  **//
#//**                       Phone: (770)-246-8600                      **//
#//**                                                                  **//
#//**********************************************************************//
#//**********************************************************************//

#************************************************************************//
# $Header: /VisualeBios/Src/Applications/Veb/Projects/WizTemplate/NCW/Aptio3/eModule/RootBridgeDriver/RootBridgeDriver.mak 1     1/31/06 8:45a Igork $
#
# $Revision: 1 $
#
# $Date: 1/31/06 8:45a $
#************************************************************************//
# Revision History
# ----------------
# $Log: /VisualeBios/Src/Applications/Veb/Projects/WizTemplate/NCW/Aptio3/eModule/RootBridgeDriver/RootBridgeDriver.mak $
## 
## 1     1/31/06 8:45a Igork
#
#************************************************************************//
FILE_GUID = $(FILE_GUID)
COMPONENT_TYPE = BS_DRIVER

INCLUDES = \
	-I $(EFI_SOURCE)\EDK\FOUNDATION \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\$(PROCESSOR) \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\EFI \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\EFI\$(PROCESSOR) \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\EFI\INCLUDE \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\EFI\INCLUDE\$(PROCESSOR) \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\FRAMEWORK \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\FRAMEWORK\$(PROCESSOR) \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\FRAMEWORK\INCLUDE \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\FRAMEWORK\INCLUDE\$(PROCESSOR) \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\INCLUDE \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\INCLUDE\$(PROCESSOR) \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\LIBRARY\DXE\INCLUDE \
	-I $(EFI_SOURCE)\EDK\FOUNDATION\LIBRARY\DXE\INCLUDE\$(PROCESSOR) 

LIBS = \
	$(LIB_DIR)\efidriverlib.lib 

IMAGE_ENTRY_POINT = RootBridgeDriverEntry
#//**********************************************************************//
#//**********************************************************************//
#//**                                                                  **//
#//**        (C)Copyright 1985-2003, American Megatrends, Inc.         **//
#//**                                                                  **//
#//**                       All Rights Reserved.                       **//
#//**                                                                  **//
#//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **//
#//**                                                                  **//
#//**                       Phone: (770)-246-8600                      **//
#//**                                                                  **//
#//**********************************************************************//
#//**********************************************************************//
