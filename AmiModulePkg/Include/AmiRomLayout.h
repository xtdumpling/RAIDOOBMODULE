//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  @brief ROM Layout Header File.

  The file contains definitions of the ROM Layout structures and constants.
**/

#ifndef __AMI_ROM_LAYOUT__H__
#define __AMI_ROM_LAYOUT__H__
#include <Base.h>

/// Defines type of a ROM Layout Area.
///
typedef enum {
    AmiRomAreaTypeFv, ///< ROM area is a firmware volume.
    AmiRomAreaTypeRaw, ///< ROM area has an unknown structure.
    AmiRomAreaTypeMax
} AMI_ROM_AREA_TYPE;

#pragma pack(push, 1)

/// A ROM Layout Area type represents a portion of the firmware ROM image.
/// An array of these structures describes layout of the entire ROM image.
///
/// __NOTE: Additional fields may be added at the end of the structure in the future,
/// in which case the value of AMI_ROM_LAYOUT_VERSION will be increased
/// (see also a NOTE in the AMI_ROM_LAYOUT_HEADER description).__
typedef struct {
    /// Unique ROM area identifier. For FV areas, the same GUID is stored in the FvName field of the extended header.
    /// If GUID is not specified in the VeB ROM Layout Wizard, FV extended header is not generated and a zero GUID is placed into a ROM area descriptor.
    /// If ROM contains redundant copies of the same area, the same GUID may be used.
    ///
    /// __NOTE: If ROM contains redundant copies, only one copy can have access attributes.
    /// Other copies must have no access attributes (they can have other types of attributes set).
    /// They can be published "on-demand" using RomLayout module's porting infrastructure.__
    GUID Guid;
    /// Physical address the area is mapped at during firmware execution.
    /// If address is not specified in the VeB ROM Layout Wizard, Address is set to '<Rom-Starting-Address>+Offset'.
    UINT64 Address;
    /// Area offset in the ROM image.
    ///
    UINT32 Offset;
    /// Area size.
    ///
    UINT32 Size;
    /// A value of the ::AMI_ROM_AREA_TYPE.
    /// It's declared as UINT32 (and not as ::AMI_ROM_AREA_TYPE) because enum size is compiler dependent.
    UINT32 Type;
    /// A bit mask that describes area properties.
    /// Supported values are defined by the AMI_ROM_AREA_XXX macros.
    /// There are several attribute groups:
    ///  - @ref RomAreaAccessAttributes "Access Attributes"
    ///  - @ref RomAreaContentTypeAttributes "Content Type Attributes"
    ///  - @ref RomAreaCryptographicAttributes "Cryptographic Attributes"
    ///  - @ref RomAreaUpdateAttributes "Update Attributes"
    UINT32 Attributes;
} AMI_ROM_AREA;

/// ROM layout data structure is stored somewhere in the ROM image.
/// The data structure is a sequence of AMI_ROM_AREA elements preceded by this header.
///
/// __NOTE: The ROM layout may be stored in the area of the ROM image that is updated independently from the code
/// that performs ROM layout processing. Never assume ROM layout format matches these definitions.
/// Always check Version and DesriptorSize fields.
/// If Version is larger than AMI_ROM_LAYOUT_VERSION, AMI_ROM_AREA may have additional fields at the end of the structure.
/// If Version is smaller than AMI_ROM_LAYOUT_VERSION, some of the fields at the end of the AMI_ROM_AREA structure may be missing.__
typedef struct {
    /// Set to AMI_ROM_LAYOUT_SIGNATURE
    ///
    UINT32 Signature;
    /// ROM Layout version. Set to AMI_ROM_LAYOUT_VERSION at build time.
    /// Applies to this header as well as the AMI_ROM_AREA descriptors.
    UINT32 Version;
    /// Size of the individual descriptor.
    ///
    /// __NOTE: The size of the descriptor is not necessarily equal to the `sizeof(AMI_ROM_AREA)`.
    /// You might be dealing with an older or newer layout format.__
    UINT32 DescriptorSize;
    /// A number of AMI_ROM_AREA descriptors in the ROM Layout array.
    ///
    UINT32 NumberOfDescriptors;
//  AMI_ROM_AREA Descriptors[...];
} AMI_ROM_LAYOUT_HEADER;

#pragma pack(pop)

/// @name ROM Area Access Attributes
/// @anchor RomAreaAccessAttributes
/// Access attributes define at what point during the boot process the FV is exposed to the Core.
/// The access attributes are illegal for non-FV areas.
///@{

/// The FV will be published early in PEI
///
#define AMI_ROM_AREA_FV_PEI               0x00000002
/// The FV will be published in PEI after the memory detection
///
#define AMI_ROM_AREA_FV_PEI_MEM           0x00000004
/// The FV will be published in PEI after the memory detection
/// (just like ROM_AREA_FV_PEI_MEM).
/// The FV will be copied to memory prior to the publishing.
#define AMI_ROM_AREA_FV_PEI_SHADOW        0x00000020
/// The FV will be published in DXE IPL Entry prior to transition to DXE.
/// The FV published in DXE IPL are not processed by the PEI dispatcher.
/// The FV is visible to DXE IPL, which means it can contain the DXE Core.
#define AMI_ROM_AREA_FV_DXE               0x00000008
/// The FV will be published early in DXE.
/// The FV is not visible to DXE IPL, which means it cannot contain the DXE Core.
#define AMI_ROM_AREA_FV_DXE2              0x00000800
/// The FV will be published in BDS Entry.
/// The FV is processed by the DXE dispatcher.
#define AMI_ROM_AREA_FV_BDS               0x00000010

/// The FV is processed by the PEI dispatcher if either
///  ROM_AREA_FV_PEI, or ROM_AREA_FV_PEI_MEM, or ROM_AREA_FV_PEI_SHADOW attribute is defined.
#define AMI_ROM_AREA_FV_ACCESS_PEI (AMI_ROM_AREA_FV_PEI | AMI_ROM_AREA_FV_PEI_MEM | AMI_ROM_AREA_FV_PEI_SHADOW)

/// The FV is processed by the DXE dispatcher if either
/// ROM_AREA_FV_DXE, or ROM_AREA_FV_DXE2, or ROM_AREA_FV_BDS attribute is defined.
#define AMI_ROM_AREA_FV_ACCESS_DXE (AMI_ROM_AREA_FV_DXE | AMI_ROM_AREA_FV_DXE2 | AMI_ROM_AREA_FV_BDS)

/// Combination of all FV access attributes
///
#define AMI_ROM_AREA_FV_ACCESS_ATTRIBUTES (AMI_ROM_AREA_FV_ACCESS_PEI | AMI_ROM_AREA_FV_ACCESS_DXE)
///@}

/// @name Content Type Attributes
/// @anchor RomAreaContentTypeAttributes
/// The content type attributes provide information regarding type of content stored in the ROM area.
/// They can be used for FV and non-FV areas.
///@{

/// The system should not switch to a recovery mode if area with this attribute is corrupted.
///
#define AMI_ROM_AREA_NON_CRITICAL         0x00000080
/// This area is required to perform the firmware recovery.
/// The firmware recovery will not work if this area is corrupted.
#define AMI_ROM_AREA_VITAL                0x00000400
///@}

/// @name Cryptographic Attributes
/// @anchor RomAreaCryptographicAttributes
/// Cryptographic attributes define if the area should be included during cryptographic processing of the ROM image.
///@{

/// If the attribute is defined, the area is hashed during digital signature creation and validation.
/// If the attribute is not defined, the area is skipped during digital signature creation and validation.
#define AMI_ROM_AREA_SIGNED               0x00000200
/// An area containing the BIOS boot code and/or BIOS boot configuration data whose integrity measurement
/// (via a cryptographic hash) must be recorded in TPM PCRs and reported in TCG log as EV_EFI_PLATFORM_FIRMWARE_BLOB event.
/// The Firmware Volume containing a platform firmware that is either contains or measures the EFI Boot service and EFI Runtime services
/// code must have this attribute set.
#define AMI_ROM_AREA_TCG_MEASURED         0x00008000
///@}

/// @name Update Attributes
/// @anchor RomAreaUpdateAttributes
/// Reserved for future use.
///@{

#define AMI_ROM_AREA_READ_ONLY            0x00001000
#define AMI_ROM_AREA_BOOT_TIME_UPDATABLE  0x00002000
#define AMI_ROM_AREA_RUN_TIME_UPDATABLE   0x00004000
///@}

#define AMI_ROM_LAYOUT_VERSION 1
#define AMI_ROM_LAYOUT_SIGNATURE		SIGNATURE_32('R','O','M','L')

/// When ROM area address is set to this value, the ROM area is not memory mapped.
#define AMI_ROM_AREA_NOT_MEMORY_MAPPED  0xFFFFFFFFFFFFFFFFULL

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
