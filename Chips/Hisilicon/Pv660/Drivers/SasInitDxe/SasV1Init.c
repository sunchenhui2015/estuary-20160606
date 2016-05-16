/** @file
*
*  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2016, Linaro Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/


#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/PlatformSasProtocol.h>

#define SAS_RESET_VALUE             0x7ffff

STATIC
VOID
SasInit_0 (
    IN PLATFORM_SAS_PROTOCOL   *This
)
{
  UINT32 base = 0xc0000000;

  // Apply reset and disable clock
  MmioWrite32(base + 0xa60, SAS_RESET_VALUE);
  MmioWrite32(base + 0x33c, SAS_RESET_VALUE);
  // Wait for reset takes effect
  MicroSecondDelay(1000);
  // De-reset and enable clock
  MmioWrite32(base + 0xa64, SAS_RESET_VALUE);
  MmioWrite32(base + 0x338, SAS_RESET_VALUE);
}

PLATFORM_SAS_PROTOCOL Sas0 = {
  0xc1000000,
  SasInit_0
};

STATIC
VOID
SasInit_1 (
    IN PLATFORM_SAS_PROTOCOL   *This
)
{
  UINT32 base = 0xb0000000;

  // Apply reset and disable clock
  MmioWrite32(base + 0xa18, SAS_RESET_VALUE);
  MmioWrite32(base + 0x31c, SAS_RESET_VALUE);
  // Wait for reset takes effect
  MicroSecondDelay(1000);
  // De-reset and enable clock
  MmioWrite32(base + 0xa1c, SAS_RESET_VALUE);
  MmioWrite32(base + 0x318, SAS_RESET_VALUE);
}

PLATFORM_SAS_PROTOCOL Sas1 = {
  0xb1000000,
  SasInit_1
};

EFI_STATUS
EFIAPI
SasV1InitEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &Handle,
                  &gPlatformSasProtocolGuid, &Sas0,
                  NULL
                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &Handle,
                  &gPlatformSasProtocolGuid, &Sas1,
                  NULL
                 );
  return Status;
}
