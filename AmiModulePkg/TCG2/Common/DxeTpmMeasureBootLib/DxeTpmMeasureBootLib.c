/** @file
  The library instance provides security service of TPM measure boot.  

Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License 
which accompanies this distribution.  The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Protocol/TcgService.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/DevicePathToText.h>
#include <AmiProtocol.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PeCoffLib.h>
#include <Library/SecurityManagementLib.h>
#include <Protocol/SmmAccess2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmBase2.h>
#include <Token.h>
#if AMI_MODULE_PKG_VERSION >= 28
#include <Library/AmiImageTypeLib.h>
#endif


//
// Flag to check GPT partition. It only need be measured once.
//
BOOLEAN                        mMeasureGptTableFlag = FALSE;
UINTN                             mMeasureGptCount = 0;
EFI_EVENT       ReadyToBootEvent;
static BOOLEAN          ReadytoBootSignaled = FALSE;
EFI_SMM_BASE2_PROTOCOL    *gSmmBase2 = NULL;
EFI_TCG_PLATFORM_PROTOCOL     *TcgPlatform = NULL;

#ifndef AMI_IMAGE_FROM_FV
#define AMI_IMAGE_FROM_FV   1
#endif


/**
  Reads contents of a PE/COFF image in memory buffer.

  @param  FileHandle      Pointer to the file handle to read the PE/COFF image.
  @param  FileOffset      Offset into the PE/COFF image to begin the read operation.
  @param  ReadSize        On input, the size in bytes of the requested read operation.  
                          On output, the number of bytes actually read.
  @param  Buffer          Output buffer that contains the data read from the PE/COFF image.
  
  @retval EFI_SUCCESS     The specified portion of the PE/COFF image was read and the size 
**/

EFI_STATUS
EFIAPI
ImageRead (
  IN     VOID    *FileHandle,
  IN     UINTN   FileOffset,
  IN OUT UINTN   *ReadSize,
  OUT    VOID    *Buffer
  )
{
  CopyMem (Buffer, (UINT8 *)((UINTN) FileHandle + FileOffset), *ReadSize);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EfiCreateEventReadyToBootEx (
  IN  EFI_TPL           NotifyTpl,
  IN  EFI_EVENT_NOTIFY  NotifyFunction,  OPTIONAL
  IN  VOID              *NotifyContext,  OPTIONAL
  OUT EFI_EVENT         *ReadyToBootEvent
  );


#if AMI_MODULE_PKG_VERSION < 28
UINT32
TcgAmiGetImageType (
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File,
  IN  VOID                             *FileBuffer,
  IN  UINTN                            FileSize,
  IN BOOLEAN                           BootPolicy
  )
{
    return 0; //return unknown
}
#endif



/**
  The security handler is used to abstract platform-specific policy 
  from the DXE core response to an attempt to use a file that returns a 
  given status for the authentication check from the section extraction protocol.  

  The possible responses in a given SAP implementation may include locking 
  flash upon failure to authenticate, attestation logging for all signed drivers, 
  and other exception operations.  The File parameter allows for possible logging 
  within the SAP of the driver.


  If File is NULL device path information will not be placed in
  TCG LOG
  If the file specified by File with an authentication status specified by 
  AuthenticationStatus is safe for the DXE Core to use, then EFI_SUCCESS is returned.

  If the file specified by File with an authentication status specified by 
  AuthenticationStatus is not safe for the DXE Core to use under any circumstances, 
  then EFI_ACCESS_DENIED is returned.

  If the file specified by File with an authentication status specified by 
  AuthenticationStatus is not safe for the DXE Core to use right now, but it 
  might be possible to use it at a future time, then EFI_SECURITY_VIOLATION is 
  returned.

  @param[in, out] AuthenticationStatus  This is the authentication status returned
                                        from the securitymeasurement services for the
                                        input file.
  @param[in]      File       This is a pointer to the device path of the file that is
                             being dispatched. This will optionally be used for logging.
  @param[in]      FileBuffer File buffer matches the input file device path.
  @param[in]      FileSize   Size of File buffer matches the input file device path.

  @retval EFI_SUCCESS            The file specified by File did authenticate, and the
                                 platform policy dictates that the DXE Core may use File.
								 If File is NULL device path information will not be placed in
								 TCG LOG

  @retval EFI_SECURITY_VIOLATION The file specified by File did not authenticate, and
                                 the platform policy dictates that File should be placed
                                 in the untrusted state. A file may be promoted from
                                 the untrusted to the trusted state at a future time
                                 with a call to the Trust() DXE Service.
  @retval EFI_ACCESS_DENIED      The file specified by File did not authenticate, and
                                 the platform policy dictates that File should not be
                                 used for any purpose.

**/
EFI_STATUS
EFIAPI
DxeTpmMeasureBootPEHandler (
  IN  OUT   UINT32                     AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File,
  IN  VOID                             *FileBuffer OPTIONAL,
  IN  UINTN                            FileSize OPTIONAL
  )
{
  EFI_STATUS                     Status;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePathNode=NULL;
  EFI_DEVICE_PATH_PROTOCOL      *OrigDevicePathNode=NULL;
  EFI_HANDLE                     Handle = NULL;
  BOOLEAN                        ApplicationRequired;
  PE_COFF_LOADER_IMAGE_CONTEXT      ImageContext;
  UINT32                            isFlashImage;
  BOOLEAN                        BootPolicy=FALSE;
  
  if(TcgPlatform == NULL){
      Status = gBS->LocateProtocol (&gEfiTcgPlatformProtocolGuid, NULL, (VOID **) &TcgPlatform);
      if (EFI_ERROR (Status)) {
          TcgPlatform = NULL;
          return EFI_SUCCESS;
      }
  }
  
#if AMI_MODULE_PKG_VERSION >= 28
  isFlashImage = AmiGetImageType(File, FileBuffer, FileSize, BootPolicy);
#else
  isFlashImage = TcgAmiGetImageType(File, FileBuffer, FileSize, BootPolicy);
#endif
      
  if(isFlashImage == AMI_IMAGE_FROM_FV)
  {
      goto Finish;
  }
  
  ApplicationRequired = FALSE;
  OrigDevicePathNode = DuplicateDevicePath (File);


  //
  // Check whether this device path support FV2 protocol.
  //
  DevicePathNode = OrigDevicePathNode;
  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &DevicePathNode, &Handle);
  if (!EFI_ERROR (Status)) {
    //
    // Don't check FV image, and directly return EFI_SUCCESS.
    // It can be extended to the specific FV authentication according to the different requirement.
    //
    if (IsDevicePathEnd (DevicePathNode)) {
      return EFI_SUCCESS;
    }
    //
    // The image from Firmware image will not be mearsured.
    // Current policy doesn't measure PeImage from Firmware if it is driver
    // If the got PeImage is application, it will be still be measured.
    //
    //ApplicationRequired = TRUE;
  }
  
  //
  // File is not found.
  //
  if (FileBuffer == NULL) {
    Status = EFI_SECURITY_VIOLATION;
    goto Finish;
  }

  //
  // Measure PE Image
  //
  DevicePathNode = OrigDevicePathNode;
  ZeroMem (&ImageContext, sizeof (ImageContext));
  ImageContext.Handle    = (VOID *) FileBuffer;
  ImageContext.ImageRead = (PE_COFF_LOADER_READ_FILE) ImageRead;

  //
  // Get information about the image being loaded
  //
  Status = PeCoffLoaderGetImageInfo (&ImageContext);
  
  DEBUG ((DEBUG_INFO, "ImageContext.ImageCodeMemoryType %x.\n", ImageContext.ImageCodeMemoryType));
  DEBUG ((DEBUG_INFO, "ImageContext.ImageDataMemoryType %x.\n", ImageContext.ImageDataMemoryType));
  DEBUG ((DEBUG_INFO, "ImageContext.EntryPoint %x.\n", ImageContext.EntryPoint));
  DEBUG ((DEBUG_INFO, "ImageContext.DestinationAddress %x.\n", ImageContext.DestinationAddress));
  DEBUG ((DEBUG_INFO, "ImageContext.ImageCodeMemoryType %x.\n", ImageContext.ImageCodeMemoryType));
  DEBUG ((DEBUG_INFO, "ImageContext.ImageType %x.\n", ImageContext.ImageType));
  DEBUG ((DEBUG_INFO, "ImageContext.Handle %x\n", ImageContext.Handle));
  DEBUG ((DEBUG_INFO, "ImageContext.IsTeImage %x.\n", ImageContext.IsTeImage));

  if (EFI_ERROR (Status)) {
    //
    // The information can't be got from the invalid PeImage
    //
    DEBUG ((DEBUG_ERROR, "PeCoffLoaderGetImageInfo Error  %r.\n", Status));
    goto Finish;
  }
  
  //
  // Measure only application if Application flag is set
  // Measure drivers and applications if Application flag is not set
  //
  if ((!ApplicationRequired) || 
        (ApplicationRequired && ImageContext.ImageType == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION)) {  
    //
    // Print the image path to be measured.
    //    
    DEBUG_CODE_BEGIN ();
      CHAR16                            *ToText;
      EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *DevPathToText;
      Status = gBS->LocateProtocol (
                      &gEfiDevicePathToTextProtocolGuid,
                      NULL,
                      (VOID **) &DevPathToText
                      );
      if (!EFI_ERROR (Status)) {
        ToText = DevPathToText->ConvertDevicePathToText (
                                  DevicePathNode,
                                  FALSE,
                                  TRUE
                                  );
        if (ToText != NULL) {
          DEBUG ((DEBUG_INFO, "The measured image path is %s.\n", ToText));
        }
      }
    DEBUG_CODE_END ();

    //
    // Measure PE image into TPM log.
    //
    DEBUG ((DEBUG_INFO, "ImageContext.ImageAddress %x \n", ImageContext.ImageAddress));
       
    //else hash any loaded image
    
    DEBUG ((DEBUG_INFO, "calling measure PE image \n"));
    
    Status = TcgPlatform->MeasurePeImage (
               FALSE,
               (EFI_PHYSICAL_ADDRESS) (UINTN) FileBuffer, 
               FileSize, 
               (UINTN) ImageContext.ImageAddress, 
               ImageContext.ImageType,
               Handle,
               DevicePathNode);
  }

  //
  // Done, free the allocated resource.
  //
Finish:
  if(OrigDevicePathNode!=NULL){
      FreePool (OrigDevicePathNode);
  }

  return EFI_SUCCESS;
}





/**
  The security handler is used to abstract platform-specific policy 
  from the DXE core response to an attempt to use a file that returns a 
  given status for the authentication check from the section extraction protocol.  

  The possible responses in a given SAP implementation may include locking 
  flash upon failure to authenticate, attestation logging for all signed drivers, 
  and other exception operations.  The File parameter allows for possible logging 
  within the SAP of the driver.


  If File is NULL, then EFI_SUCCESS is returned. File is not measured.
  If the file specified by File with an authentication status specified by 
  AuthenticationStatus is safe for the DXE Core to use, then EFI_SUCCESS is returned.

  If the file specified by File with an authentication status specified by 
  AuthenticationStatus is not safe for the DXE Core to use under any circumstances, 
  then EFI_ACCESS_DENIED is returned.

  If the file specified by File with an authentication status specified by 
  AuthenticationStatus is not safe for the DXE Core to use right now, but it 
  might be possible to use it at a future time, then EFI_SECURITY_VIOLATION is 
  returned.

  @param[in, out] AuthenticationStatus  This is the authentication status returned
                                        from the securitymeasurement services for the
                                        input file.
  @param[in]      File       This is a pointer to the device path of the file that is
                             being dispatched. This will optionally be used for logging.
  @param[in]      FileBuffer File buffer matches the input file device path.
  @param[in]      FileSize   Size of File buffer matches the input file device path.

  @retval EFI_SUCCESS            The file specified by File did authenticate, and the
                                 platform policy dictates that the DXE Core may use File.
								 If File is NULL function returns EFI success and 
								 File is not measured.

  @retval EFI_SECURITY_VIOLATION The file specified by File did not authenticate, and
                                 the platform policy dictates that File should be placed
                                 in the untrusted state. A file may be promoted from
                                 the untrusted to the trusted state at a future time
                                 with a call to the Trust() DXE Service.
  @retval EFI_ACCESS_DENIED      The file specified by File did not authenticate, and
                                 the platform policy dictates that File should not be
                                 used for any purpose.

**/
EFI_STATUS
EFIAPI
DxeTpmMeasureBootHandler (
  IN  OUT   UINT32                     AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File,
  IN  VOID                             *FileBuffer OPTIONAL,
  IN  UINTN                            FileSize OPTIONAL
  )
{
  EFI_TCG_PLATFORM_PROTOCOL     *TcgPlatform = NULL;
  EFI_STATUS                     Status;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePathNode=NULL;
  EFI_DEVICE_PATH_PROTOCOL      *OrigDevicePathNode=NULL;
  EFI_HANDLE                     Handle = NULL;

  if (File == NULL) {
    return EFI_SUCCESS;
  }
  
  ReadytoBootSignaled = TRUE;

  Status = gBS->LocateProtocol (&gEfiTcgPlatformProtocolGuid, NULL, (VOID **) &TcgPlatform);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // Copy File Device Path
  //
  OrigDevicePathNode = DuplicateDevicePath (File);
  ASSERT (OrigDevicePathNode != NULL);
  
  //
  // 1. Check whether this device path support BlockIo protocol.
  // Is so, this device path may be a GPT device path.
  //
  DevicePathNode = OrigDevicePathNode;
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &DevicePathNode, &Handle);
  if (!EFI_ERROR (Status) && !mMeasureGptTableFlag) {
    //
    // Find the gpt partion on the given devicepath
    //
    DevicePathNode = OrigDevicePathNode;
    while (!IsDevicePathEnd (DevicePathNode)) {
      //
      // Find the Gpt partition
      //
      if (DevicePathType (DevicePathNode) == MEDIA_DEVICE_PATH &&
            DevicePathSubType (DevicePathNode) == MEDIA_HARDDRIVE_DP) {
        //
        // Check whether it is a gpt partition or not
        //                           
        if (((HARDDRIVE_DEVICE_PATH *) DevicePathNode)->MBRType == MBR_TYPE_EFI_PARTITION_TABLE_HEADER && 
            ((HARDDRIVE_DEVICE_PATH *) DevicePathNode)->SignatureType == SIGNATURE_TYPE_GUID) {

          //
          // Change the partition device path to its parent device path (disk) and get the handle.
          //
          DevicePathNode->Type    = END_DEVICE_PATH_TYPE;
          DevicePathNode->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
          DevicePathNode          = OrigDevicePathNode;
          Status = gBS->LocateDevicePath (
                         &gEfiDiskIoProtocolGuid,
                         &DevicePathNode,
                         &Handle
                         );
          if (!EFI_ERROR (Status)) {
            //
            // Measure GPT disk.
            //
//            Status = TcgMeasureGptTable (TcgProtocol, Handle);
            Status =  TcgPlatform->MeasureGptTable( OrigDevicePathNode);
            if (!EFI_ERROR (Status)) {
              //
              // GPT disk check done.
              //
              mMeasureGptTableFlag = TRUE;
            }
          }

          if(OrigDevicePathNode!=NULL){
              FreePool (OrigDevicePathNode);
          }
          
          OrigDevicePathNode = DuplicateDevicePath (File);
          ASSERT (OrigDevicePathNode != NULL);
          break;
        }
      }
      DevicePathNode    = NextDevicePathNode (DevicePathNode);
    }
  }

  if(OrigDevicePathNode!=NULL){
      FreePool (OrigDevicePathNode);
  }

  return EFI_SUCCESS;
}



EFI_STATUS RegisterTpmMeasurementCallback(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    EFI_STATUS Status;

    DEBUG ((DEBUG_INFO, "DxeTpmMeasureBootLibConstructor Callback.\n"));

     Status =  RegisterSecurityHandler (
                 DxeTpmMeasureBootHandler,
                 EFI_AUTH_OPERATION_MEASURE_IMAGE | EFI_AUTH_OPERATION_IMAGE_REQUIRED
                      );

     DEBUG ((DEBUG_INFO, "DxeTpmMeasureBootLibConstructor Callback.return Status = %r \n", Status));
     return Status;
}



EFI_STATUS RegisterTpmMeasurePECallback(
    IN EFI_EVENT ev,
    IN VOID      *ctx )
{
    EFI_STATUS Status;

    DEBUG ((DEBUG_INFO, "RegisterTpmMeasurePECallback Callback.\n"));

     Status =  RegisterSecurityHandler (
                 DxeTpmMeasureBootPEHandler,
                 EFI_AUTH_OPERATION_MEASURE_IMAGE | EFI_AUTH_OPERATION_IMAGE_REQUIRED
                      );

     DEBUG ((DEBUG_INFO, "RegisterTpmMeasurePECallback Callback.return Status = %r \n", Status));
     return Status;
}




/**
  Register the security handler to provide TPM measure boot service.

  @param  ImageHandle  ImageHandle of the loaded driver.
  @param  SystemTable  Pointer to the EFI System Table.

  @retval  EFI_SUCCESS            Register successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to register this handler.
**/

EFI_STATUS
EFIAPI
DxeTpmMeasureBootLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS          Status;

    DEBUG ((DEBUG_INFO, "DxeTpmMeasureBootLibConstructor Entry.\n"));

    Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
              RegisterTpmMeasurementCallback,
              NULL,
              &ReadyToBootEvent
              );
    
    RegisterTpmMeasurePECallback(NULL, NULL);
    return Status ;
}


/**
    Destructor that closes the event created by the contructor

    @param  ImageHandle 
    @param  SystemTable 

    @retval Status from CloseEvent 

**/
EFI_STATUS
EFIAPI
DxeTpmMeasureBootLibDestructor (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    //Close the event
    if(ReadyToBootEvent) {
        Status = gBS->CloseEvent (ReadyToBootEvent);
        ASSERT_EFI_ERROR(Status);
    }

    return Status;
}
