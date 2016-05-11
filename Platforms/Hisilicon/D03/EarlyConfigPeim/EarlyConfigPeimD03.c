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


#include <Uefi.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/CacheMaintenanceLib.h>

#include <PlatformArch.h>
#include <Library/PlatformSysCtrlLib.h>

#include <Library/OemMiscLib.h>
#include <Library/OemAddressMapLib.h>
#include <Library/ArmLib.h>

#define TIMER_SUBCTRL_BASE   PcdGet64(PcdPeriSubctrlAddress)
#define SC_TM_CLKEN0_REG     (0x2050)

VOID PlatformTimerStart (VOID)
{
    // Timer0 clock enable
    MmioWrite32 (TIMER_SUBCTRL_BASE + SC_TM_CLKEN0_REG, 0x3);
}

void QResetAp(VOID)
{
    MmioWrite64(FixedPcdGet64(PcdMailBoxAddress), 0x0);
    (void)WriteBackInvalidateDataCacheRange((VOID *) FixedPcdGet64(PcdMailBoxAddress), 8);
    asm("DSB SY");
    asm("ISB");

    //CDIE A
    if (!PcdGet64 (PcdTrustedFirmwareEnable))
    {
        StartupAp();
    }
}


EFI_STATUS
EFIAPI
EarlyConfigEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    DEBUG((EFI_D_ERROR,"SMMU CONFIG........."));
    (VOID)SmmuConfigForOS();
    DEBUG((EFI_D_ERROR,"Done\n"));

    //DEBUG((EFI_D_ERROR,"ITS CONFIG........."));
    //(VOID)ITSCONFIG();
    //DEBUG((EFI_D_ERROR,"Done\n"));

    DEBUG((EFI_D_ERROR,"AP CONFIG........."));
    (VOID)QResetAp();
    DEBUG((EFI_D_ERROR,"Done\n"));

    DEBUG((EFI_D_ERROR,"MN CONFIG........."));
    (VOID)MN_CONFIG();
    DEBUG((EFI_D_ERROR,"Done\n"));

    if(OemIsMpBoot())
    {
        DEBUG((EFI_D_ERROR,"Event Broadcast CONFIG........."));
        //EVENT broadcast
        *(volatile UINT32 *)(0x60016220)   = 0x7;
        *(volatile UINT32 *)(0x60016230)   = 0x40016260;
        *(volatile UINT32 *)(0x60016234)   = 0X0;
        *(volatile UINT32 *)(0x60016238)   = 0x60016260;
        *(volatile UINT32 *)(0x6001623C)   = 0x400;
        *(volatile UINT32 *)(0x60016240)   = 0x40016260;
        *(volatile UINT32 *)(0x60016244)   = 0x400;

        *(volatile UINT32 *)(0x40016220)   = 0x7;
        *(volatile UINT32 *)(0x40016230)   = 0x60016260;
        *(volatile UINT32 *)(0x40016234)   = 0X0;
        *(volatile UINT32 *)(0x40016238)   = 0x60016260;
        *(volatile UINT32 *)(0x4001623C)   = 0x400;
        *(volatile UINT32 *)(0x40016240)   = 0x40016260;
        *(volatile UINT32 *)(0x40016244)   = 0x400;

        *(volatile UINT32 *)(0x60016220 + S1_BASE)   = 0x7;
        *(volatile UINT32 *)(0x60016230 + S1_BASE)   = 0x40016260;
        *(volatile UINT32 *)(0x60016234 + S1_BASE)   = 0X0;
        *(volatile UINT32 *)(0x60016238 + S1_BASE)   = 0x60016260;
        *(volatile UINT32 *)(0x6001623C + S1_BASE)   = 0x0;
        *(volatile UINT32 *)(0x60016240 + S1_BASE)   = 0x40016260;
        *(volatile UINT32 *)(0x60016244 + S1_BASE)   = 0x400;

        *(volatile UINT32 *)(0x40016220 + S1_BASE)   = 0x7;
        *(volatile UINT32 *)(0x40016230 + S1_BASE)   = 0x60016260;
        *(volatile UINT32 *)(0x40016234 + S1_BASE)   = 0X0;
        *(volatile UINT32 *)(0x40016238 + S1_BASE)   = 0x60016260;
        *(volatile UINT32 *)(0x4001623C + S1_BASE)   = 0x400;
        *(volatile UINT32 *)(0x40016240 + S1_BASE)   = 0x40016260;
        *(volatile UINT32 *)(0x40016244 + S1_BASE)   = 0x0;
        DEBUG((EFI_D_ERROR,"Done\n"));
    }

    DEBUG((EFI_D_ERROR,"PCIE RAM Address CONFIG........."));

    //MmioWrite32(0xa0001000, 0x27);
    //MmioWrite32(0xa0001018, 0x15003d);  //PCIE2  0xA8000000~0xB0000000

    if(OemIsMpBoot())
    {
        MmioWrite32(0xa0001000, 0x27);
        MmioWrite32(0xa0001018, 0x15003d);  //PCIE2  0xA8000000~0xB0000000
        MmioWrite32(0xb0001000, 0x2F);
        MmioWrite32(0xb0001010, 0x178033);  //PCIE3  0xBC000000~0xC0000000
        MmioWrite32(0xb0001018, 0x170035);  //PCIE2  0xB8000000~0xBC000000

    }

    else
    {
        MmioWrite32(0xa0001200, 0x0);
        MmioWrite32(0xa0001000, 0x77);
        MmioWrite32(0xa0001014, 0x17003c);  //PCIE1  0xB8000000~0xC0000000
        MmioWrite32(0xa0001018, 0x15003d);  //PCIE2  0xA8000000~0xB0000000
        MmioWrite32(0xa000101C, 0x16003e);  //PCIE0  0xB0000000~0xB8000000
    }

    DEBUG((EFI_D_ERROR,"Done\n"));

    MmioWrite32(0xd00021f0, 0x7);
    //MmioWrite32(0xa0000a8c, 0x1f);

    DEBUG((EFI_D_ERROR,"Timer CONFIG........."));
    PlatformTimerStart ();
    DEBUG((EFI_D_ERROR,"Done\n"));

    return EFI_SUCCESS;
}
