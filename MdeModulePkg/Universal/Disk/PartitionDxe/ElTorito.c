/** @file
  Decode an El Torito formatted CD-ROM

Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
//*** AMI PORTING BEGIN ***//
// ISO9660 Filesystem support
#include <Guid/AmiDevicePath.h>
//*** AMI PORTING END ***//
#include "Partition.h"


/**
  Install child handles if the Handle supports El Torito format.

  @param[in]  This        Calling context.
  @param[in]  Handle      Parent Handle.
  @param[in]  DiskIo      Parent DiskIo interface.
  @param[in]  DiskIo2     Parent DiskIo2 interface.
  @param[in]  BlockIo     Parent BlockIo interface.
  @param[in]  BlockIo2    Parent BlockIo2 interface.
  @param[in]  DevicePath  Parent Device Path


  @retval EFI_SUCCESS         Child handle(s) was added.
  @retval EFI_MEDIA_CHANGED   Media changed Detected.
  @retval other               no child handle was added.

**/
EFI_STATUS
PartitionInstallElToritoChildHandles (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Handle,
  IN  EFI_DISK_IO_PROTOCOL         *DiskIo,
  IN  EFI_DISK_IO2_PROTOCOL        *DiskIo2,
  IN  EFI_BLOCK_IO_PROTOCOL        *BlockIo,
  IN  EFI_BLOCK_IO2_PROTOCOL       *BlockIo2,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_STATUS              Status;
  UINT64                  VolDescriptorOffset;
  UINT32                  Lba2KB;
  EFI_BLOCK_IO_MEDIA      *Media;
  CDROM_VOLUME_DESCRIPTOR *VolDescriptor;
  ELTORITO_CATALOG        *Catalog;
  UINTN                   Check;
  UINTN                   Index;
  UINTN                   BootEntry;
  UINTN                   MaxIndex;
  UINT16                  *CheckBuffer;
  CDROM_DEVICE_PATH       CdDev;
  UINT32                  SubBlockSize;
  UINT32                  SectorCount;
  EFI_STATUS              Found;
  UINT32                  VolSpaceSize;
//*** AMI PORTING BEGIN ***//
  VOID *SaveVolDescriptor = NULL;
//*** AMI PORTING END ***//

  Found         = EFI_NOT_FOUND;
  Media         = BlockIo->Media;

  VolSpaceSize  = 0;

  //
  // CD_ROM has the fixed block size as 2048 bytes (SIZE_2KB)
  //

  // If the ISO image has been copied onto a different storage media
  // then the block size might be different (eg: USB).
  // Ensure 2048 (SIZE_2KB) is a multiple of block size
  if (((SIZE_2KB % Media->BlockSize) != 0) || (Media->BlockSize > SIZE_2KB)) {
    return EFI_NOT_FOUND;
  }

  VolDescriptor = AllocatePool ((UINTN)SIZE_2KB);

  if (VolDescriptor == NULL) {
    return EFI_NOT_FOUND;
  }

  Catalog = (ELTORITO_CATALOG *) VolDescriptor;

  //
  // Loop: handle one volume descriptor per time
  //       The ISO-9660 volume descriptor starts at 32k on the media
  //
  for (VolDescriptorOffset = SIZE_32KB;
       VolDescriptorOffset <= MultU64x32 (Media->LastBlock, Media->BlockSize);
       VolDescriptorOffset += SIZE_2KB) {
    Status = DiskIo->ReadDisk (
                       DiskIo,
                       Media->MediaId,
                       VolDescriptorOffset,
                       SIZE_2KB,
                       VolDescriptor
                       );
    if (EFI_ERROR (Status)) {
      Found = Status;
      break;
    }
    //
    // Check for valid volume descriptor signature
    //
    if (VolDescriptor->Unknown.Type == CDVOL_TYPE_END ||
        CompareMem (VolDescriptor->Unknown.Id, CDVOL_ID, sizeof (VolDescriptor->Unknown.Id)) != 0
        ) {
      //
      // end of Volume descriptor list
      //
      break;
    }
    //
    // Read the Volume Space Size from Primary Volume Descriptor 81-88 byte,
    // the 32-bit numerical values is stored in Both-byte orders
    //
    if (VolDescriptor->PrimaryVolume.Type == CDVOL_TYPE_CODED) {
      VolSpaceSize = VolDescriptor->PrimaryVolume.VolSpaceSize[0];
      
//*** AMI PORTING BEGIN ***//
      if(PcdGetBool(PcdUdfSupport) || PcdGetBool(PcdIso9660Support)) {
    	  SaveVolDescriptor = AllocatePool ((UINTN) SIZE_2KB);
    	  if(SaveVolDescriptor != NULL)
    		  CopyMem(SaveVolDescriptor, VolDescriptor, (UINTN) SIZE_2KB);
      }
//*** AMI PORTING END ***//
      
    }
    //
    // Is it an El Torito volume descriptor?
    //
    if (CompareMem (VolDescriptor->BootRecordVolume.SystemId, CDVOL_ELTORITO_ID, sizeof (CDVOL_ELTORITO_ID) - 1) != 0) {
      continue;
    }
    //
    // Read in the boot El Torito boot catalog
    // The LBA unit used by El Torito boot catalog is 2KB unit
    //
    Lba2KB = UNPACK_INT32 (VolDescriptor->BootRecordVolume.EltCatalog);
    // Ensure the LBA (in 2KB unit) fits into our media
    if (Lba2KB * (SIZE_2KB / Media->BlockSize) > Media->LastBlock) {
      continue;
    }

    Status = DiskIo->ReadDisk (
                       DiskIo,
                       Media->MediaId,
                       MultU64x32 (Lba2KB, SIZE_2KB),
                       SIZE_2KB,
                       Catalog
                       );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "EltCheckDevice: error reading catalog %r\n", Status));
      continue;
    }
    //
    // We don't care too much about the Catalog header's contents, but we do want
    // to make sure it looks like a Catalog header
    //
    if (Catalog->Catalog.Indicator != ELTORITO_ID_CATALOG || Catalog->Catalog.Id55AA != 0xAA55) {
      DEBUG ((EFI_D_ERROR, "EltCheckBootCatalog: El Torito boot catalog header IDs not correct\n"));
      continue;
    }

    Check       = 0;
    CheckBuffer = (UINT16 *) Catalog;
    for (Index = 0; Index < sizeof (ELTORITO_CATALOG) / sizeof (UINT16); Index += 1) {
      Check += CheckBuffer[Index];
    }

    if ((Check & 0xFFFF) != 0) {
      DEBUG ((EFI_D_ERROR, "EltCheckBootCatalog: El Torito boot catalog header checksum failed\n"));
      continue;
    }

    MaxIndex = Media->BlockSize / sizeof (ELTORITO_CATALOG);
    for (Index = 1, BootEntry = 1; Index < MaxIndex; Index += 1) {
      //
      // Next entry
      //
      Catalog += 1;

      //
      // Check this entry
      //
      if (Catalog->Boot.Indicator != ELTORITO_ID_SECTION_BOOTABLE || Catalog->Boot.Lba == 0) {
        continue;
      }

      SubBlockSize  = 512;
      SectorCount   = Catalog->Boot.SectorCount * (SIZE_2KB / Media->BlockSize);

      switch (Catalog->Boot.MediaType) {

      case ELTORITO_NO_EMULATION:
        SubBlockSize = Media->BlockSize;
        break;

      case ELTORITO_HARD_DISK:
        break;

      case ELTORITO_12_DISKETTE:
        SectorCount = 0x50 * 0x02 * 0x0F;
        break;

      case ELTORITO_14_DISKETTE:
        SectorCount = 0x50 * 0x02 * 0x12;
        break;

      case ELTORITO_28_DISKETTE:
        SectorCount = 0x50 * 0x02 * 0x24;
        break;

      default:
        DEBUG ((EFI_D_INIT, "EltCheckDevice: unsupported El Torito boot media type %x\n", Catalog->Boot.MediaType));
        SectorCount   = 0;
        SubBlockSize  = Media->BlockSize;
        break;
      }
      //
      // Create child device handle
      //
      CdDev.Header.Type     = MEDIA_DEVICE_PATH;
      CdDev.Header.SubType  = MEDIA_CDROM_DP;
      SetDevicePathNodeLength (&CdDev.Header, sizeof (CdDev));

      if (Index == 1) {
        //
        // This is the initial/default entry
        //
        BootEntry = 0;
      }

      CdDev.BootEntry = (UINT32) BootEntry;
      BootEntry++;
      CdDev.PartitionStart = Catalog->Boot.Lba * (SIZE_2KB / Media->BlockSize);
      if (SectorCount < 2) {
        //
        // When the SectorCount < 2, set the Partition as the whole CD.
        //
        if (VolSpaceSize > (Media->LastBlock + 1)) {
          CdDev.PartitionSize = (UINT32)(Media->LastBlock - Catalog->Boot.Lba + 1);
        } else {
          CdDev.PartitionSize = (UINT32)(VolSpaceSize - Catalog->Boot.Lba);
        }
      } else {
        CdDev.PartitionSize = DivU64x32 (
                                MultU64x32 (
                                  SectorCount,
                                  SubBlockSize
                                  ) + Media->BlockSize - 1,
                                Media->BlockSize
                                );
      }

      Status = PartitionInstallChildHandle (
                This,
                Handle,
                DiskIo,
                DiskIo2,
                BlockIo,
                BlockIo2,
                DevicePath,
                (EFI_DEVICE_PATH_PROTOCOL *) &CdDev,
                Catalog->Boot.Lba * (SIZE_2KB / Media->BlockSize),
                MultU64x32 (Catalog->Boot.Lba + CdDev.PartitionSize - 1, SIZE_2KB / Media->BlockSize),
                SubBlockSize,
//*** AMI PORTING BEGIN ***//
// Partition GUID support. See comments before PartitionInstallChildHandle for additional details.
                //FALSE
				NULL
//*** AMI PORTING END ***//
                );
      if (!EFI_ERROR (Status)) {
        Found = EFI_SUCCESS;
      }
    }
  }

  FreePool (VolDescriptor);
  
//*** AMI PORTING BEGIN ***//
  if(SaveVolDescriptor != NULL) {
  // UDF Filesystem support
        Status = EFI_UNSUPPORTED;
        if(PcdGetBool(PcdUdfSupport)){
        	  Status = PartitionInstallUdfHandle(
      			    This, Handle, DiskIo, DiskIo2,
      			    BlockIo, BlockIo2, DevicePath,
      			    0, BlockIo->Media->LastBlock,
      			    SIZE_2KB,
      			    VolSpaceSize, SaveVolDescriptor);
        }
  // ISO9660 Filesystem support
  //APTIOV_SERVER_OVERRIDE_START
        if (EFI_ERROR(Status) && Status != EFI_ALREADY_STARTED && PcdGetBool(PcdIso9660Support)){
  //APTIOV_SERVER_OVERRIDE_END
        			  PartitionInstallIso9660Handle(
        			    This, Handle, DiskIo, DiskIo2,
        			    BlockIo, BlockIo2, DevicePath,
        			    0, BlockIo->Media->LastBlock,
        			    SIZE_2KB,
        			    VolSpaceSize, SaveVolDescriptor);
        }
        FreePool (SaveVolDescriptor);
  }
//*** AMI PORTING END ***//
  return Found;
}

//*** AMI PORTING BEGIN ***//
// ISO9660 Filesystem support
/**
  Create a child handle for a ISO9660 primary volume partition

  @param[in]  This        Calling context.
  @param[in]  Handle      Parent Handle.
  @param[in]  DiskIo      Parent DiskIo interface.
  @param[in]  DiskIo2     Parent DiskIo2 interface.
  @param[in]  BlockIo     Parent BlockIo interface.
  @param[in]  BlockIo2    Parent BlockIo2 interface.
  @param[in]  DevicePath  Parent Device Path
  @param[in]  Start       Start Block.
  @param[in]  End         End Block.
  @param[in]  BlockSize   Child block size.
  @param[in]  VolumeSize  Volume size.
  @param[in]  VolDesc     Pointer to Volume Descriptor

  @retval EFI_SUCCESS         Child handle(s) was added.
  @retval other               no child handle was added.

**/
EFI_STATUS
PartitionInstallIso9660Handle (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Handle,
  IN  EFI_DISK_IO_PROTOCOL         *DiskIo,
  IN  EFI_DISK_IO2_PROTOCOL        *DiskIo2,
  IN  EFI_BLOCK_IO_PROTOCOL        *BlockIo,
  IN  EFI_BLOCK_IO2_PROTOCOL       *BlockIo2,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN  EFI_LBA                      Start,
  IN  EFI_LBA                      End,
  IN  UINT32                       BlockSize,
  IN  UINT32                       VolumeSize,
  IN  VOID                         *VolDesc
  )
{
  AMI_ISO9660_DEVICE_PATH ChildDp;
  VOID *RootRecord;
  VOID *VolumeLabel;

  RootRecord = (UINT8 *)VolDesc + ROOT_DIRECTORY_RECORD_OFFSET;
  VolumeLabel = (UINT8 *)VolDesc + VOLUME_LABEL_OFFSET;

  ChildDp.Dp.Header.Type = MEDIA_DEVICE_PATH;
  ChildDp.Dp.Header.SubType = MEDIA_VENDOR_DP;
  ChildDp.Dp.Guid = gAmiIso9660MediaGuid;
  ChildDp.VolumeSize = VolumeSize;
  CopyMem(ChildDp.Root, RootRecord, ROOT_DIRECTORY_RECORD_SIZE);
  CopyMem(ChildDp.VolumeLabel, VolumeLabel, VOLUME_LABEL_SIZE);
  SetDevicePathNodeLength(&ChildDp.Dp.Header, sizeof(AMI_ISO9660_DEVICE_PATH));

  return PartitionInstallChildHandle (
	This,
	Handle,
	DiskIo,
	DiskIo2,
	BlockIo,
	BlockIo2,
	DevicePath,
	(EFI_DEVICE_PATH_PROTOCOL *) &ChildDp,
	Start,
	End,
	BlockSize,
	NULL
	);
}

// UDF Filesystem support
/**
  Create a child handle for a UDF logical volume partition

  @param[in]  This        Calling context.
  @param[in]  Handle      Parent Handle.
  @param[in]  DiskIo      Parent DiskIo interface.
  @param[in]  DiskIo2     Parent DiskIo2 interface.  
  @param[in]  BlockIo     Parent BlockIo interface.
  @param[in]  BlockIo2    Parent BlockIo2 interface.
  @param[in]  DevicePath  Parent Device Path
  @param[in]  Start       Start Block.
  @param[in]  End         End Block.
  @param[in]  BlockSize   Child block size.
  @param[in]  VolumeSize  Volume size.
  @param[in]  VolDesc     Pointer to Volume Descriptor

  @retval EFI_SUCCESS         Child handle(s) was added.
  @retval other               no child handle was added.

**/
EFI_STATUS
PartitionInstallUdfHandle (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Handle,
  IN  EFI_DISK_IO_PROTOCOL         *DiskIo,
  IN  EFI_DISK_IO2_PROTOCOL        *DiskIo2,
  IN  EFI_BLOCK_IO_PROTOCOL        *BlockIo,
  IN  EFI_BLOCK_IO2_PROTOCOL       *BlockIo2,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN  EFI_LBA                      Start,
  IN  EFI_LBA                      End,
  IN  UINT32                       BlockSize,
  IN  UINT32                       VolumeSize,
  IN  VOID                         *VolDesc
  )
{
  EFI_STATUS Status;
  AMI_UDF_DEVICE_PATH ChildDp;
  VOID *ExtentLocation;
  UINT8 *AnchorDescriptor;
  
  AnchorDescriptor = AllocatePool ((UINTN)BlockSize);

  if (AnchorDescriptor == NULL) {
    return EFI_NOT_FOUND;
  }
  
  Status = DiskIo->ReadDisk (
                     DiskIo,
                     BlockIo->Media->MediaId,
                     MultU64x32 (UDF_ANCHOR_DESCRIPTOR_LBA, BlockSize),
                     BlockSize,
                     AnchorDescriptor
                     );
  if (EFI_ERROR (Status) || *((UINT16 *)AnchorDescriptor) != UDF_ANCHOR_DESCRIPTOR_TAG_ID) {
    FreePool (AnchorDescriptor);
    return EFI_NOT_FOUND;
  }

  ExtentLocation = AnchorDescriptor + UDF_ANCHOR_EXTENT_OFFSET;

  ChildDp.Dp.Header.Type = MEDIA_DEVICE_PATH;
  ChildDp.Dp.Header.SubType = MEDIA_VENDOR_DP;
  ChildDp.Dp.Guid = gAmiUdfMediaGuid;
  CopyMem(ChildDp.Extent, ExtentLocation, UDF_ANCHOR_EXTENT_SIZE);
  SetDevicePathNodeLength(&ChildDp.Dp.Header, sizeof(AMI_UDF_DEVICE_PATH));

  return PartitionInstallChildHandle (
    This,
    Handle,
    DiskIo,
	DiskIo2,
    BlockIo,
    BlockIo2,
    DevicePath,
    (EFI_DEVICE_PATH_PROTOCOL *) &ChildDp,
    Start,
    End,
    BlockSize,
    NULL
    );
}

//*** AMI PORTING END ***//
