/** @file 
  All Pcd Ppi services are implemented here.
  
Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include <Library/PeiServicesLib.h>

#include <Ppi/SiliconRegAccess.h>
#include <Library/UsraAccessApi.h>
#include <Library/DebugLib.h>

///
/// Instance of PCD_PPI protocol is EDKII native implementation.
/// This protocol instance support dynamic and dynamicEx type PCDs.
///
USRA_PPI mUsraPpiInstance = {
  PeiRegRead,
  PeiRegWrite,
  PeiRegModify,
  PeiGetRegAddr
};


EFI_PEI_PPI_DESCRIPTOR  mPpiList = {
    EFI_PEI_PPI_DESCRIPTOR_PPI  | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gUsraPpiGuid,
    &mUsraPpiInstance
};

/**
  Main entry for USRA PEIM driver.
  This routine install USRA access ppi.
  
  @param[in] FileHandle           A pointer of the address of the USRA Address Structure to be read out
  @param[in] PeiServices          Describes the list of possible PEI Services

  @retval Status                  Status of install USRA ppi
**/
EFI_STATUS
EFIAPI
UsarPeimInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
  
  //
  // Install Unified Register Access Ppi
  //
  Status = PeiServicesInstallPpi (&mPpiList);
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register read operations. 
  It transfers data from a register into a naturally aligned data buffer.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be read out
  @param[in] Buffer               A pointer of buffer for the value read from the register

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
INTN
EFIAPI
PeiRegRead (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *Buffer
  )
{
  return RegisterRead(Address, Buffer);
}

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit silicon register write operations. 
  It transfers data from a naturally aligned data buffer into a silicon register.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be written
  @param[in] Buffer               A pointer of buffer for the value write to the register

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
INTN
EFIAPI
PeiRegWrite (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *Buffer
  )
{
  return RegisterWrite(Address, Buffer);
}

/**
  This API Perform 8-bit, 16-bit, 32-bit or 64-bit Pcie silicon register AND then OR operations. It read data from a
  register, And it with the AndBuffer, then Or it with the OrBuffer, and write the result back to the register
  
  @param[in] Address              A pointer of the address of the silicon register to be modified
  @param[in] AndBuffer            A pointer of buffer for the value used for AND operation
                                  A NULL pointer means no AND operation. RegisterModify() equivalents to RegisterOr()
  @param[in] OrBuffer             A pointer of buffer for the value used for OR operation
                                  A NULL pointer means no OR operation. RegisterModify() equivalents to RegisterAnd()

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
INTN
EFIAPI
PeiRegModify (
  IN USRA_ADDRESS             *Address,
  IN VOID                     *AndBuffer,
  IN VOID                     *OrBuffer
  )
{
  return RegisterModify(Address, AndBuffer, OrBuffer);
}

/**
  This API get the flat address from the given USRA Address.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure to be read out

  @retval                         The flat address
**/
INTN
EFIAPI
PeiGetRegAddr (
  IN USRA_ADDRESS               *Address
  )
{
  return GetRegisterAddress(Address);
};

