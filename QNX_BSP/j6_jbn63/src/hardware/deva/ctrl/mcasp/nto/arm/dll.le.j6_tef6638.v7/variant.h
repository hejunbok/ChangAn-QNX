/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#define VARIANT_TEF6638

#define MCLK                 11289600L
#define AHCLKX_FREQ          MCLK
#define AHCLKR_FREQ          MCLK
#define PLAYBACK_SAMPLE_RATE 44100
#define CAPTURE_SAMPLE_RATE  PLAYBACK_SAMPLE_RATE
#define SAMPLE_SIZE          2                     /* in bytes */
#define PROTOCOL             PROTOCOL_TDM_I2S

/*
 * AIC3106's MCLK comes from the BCLK and it requires this to be no less than 2MHz
 * Thus we need to use 32 bits slot so as we can generate 2.8224MHz BLK
 */
#define TDM_SLOT_SIZE        32
#define TDM_NSLOTS           2
#define RX_VOICES            2
#define TX_VOICES            2
#define AHCLKX_DIR           INPUT                /* AHCLKX is an input from ATL_CLK2 */
#define AHCLKR_DIR           INPUT                /* AHCLKR is an input */

#define CLOCK_MODE           MASTER
#define ASYNC_CLKS           0
#define FS_ACTIVE_WIDTH      FS_WORD

#define RX_TDM_SLOT_START    0
#define TX_TDM_SLOT_START    0
 
#define RX_SERIALIZER        1
#define TX_SERIALIZER        0

#define CODEC_SLAVE_ADDR1    0x18
#define CODEC_SLAVE_ADDR2    0x19
#define CODEC_SLAVE_ADDR3    0x1a

/* Board Specific Mapping */
#define     EDMA3CC_ADDR     0x43300000
#define     MCASP0_BASEADDR  0x48460000
#define     MCASP1_BASEADDR  0x48464000
#define     MCASP2_BASEADDR  0x48468000
#define     MCASP3_BASEADDR  0x4846C000
#define     MCASP4_BASEADDR  0x48470000
#define     MCASP5_BASEADDR  0x48474000
#define     MCASP6_BASEADDR  0x48478000
#define     MCASP7_BASEADDR  0x4847C000
#define     MCASP_SIZE       0x2C0

#define     MCASP0_dMAX      0x45800000
#define     MCASP1_dMAX      0x45C00000
#define     MCASP2_dMAX      0x46000000
#define     MCASP3_dMAX      0x48436000
#define     MCASP4_dMAX      0x4843A000
#define     MCASP5_dMAX      0x4844C000
#define     MCASP6_dMAX      0x48450000
#define     MCASP7_dMAX      0x48454000

#define MCASP0_DMA_PLAY     8
#define MCASP0_DMA_CAP      9
#define MCASP1_DMA_PLAY     0
#define MCASP1_DMA_CAP      0
#define MCASP2_DMA_PLAY     58
#define MCASP2_DMA_CAP      59
#define MCASP3_DMA_PLAY     0
#define MCASP3_DMA_CAP      0

#define MCASP4_DMA_PLAY     10	//6638
#define MCASP4_DMA_CAP      11	//6638

#define MCASP5_DMA_PLAY     16
#define MCASP5_DMA_CAP      17
#define MCASP6_DMA_PLAY     18
#define MCASP6_DMA_CAP      19


#define MCASP0_TX_IRQ       (109 + 32)
#define MCASP0_RX_IRQ       (108 + 32)
#define MCASP1_TX_IRQ       0
#define MCASP1_RX_IRQ       0
#define MCASP2_TX_IRQ       (157 + 32)
#define MCASP2_RX_IRQ       (158 + 32)
#define MCASP3_TX_IRQ       0
#define MCASP3_RX_IRQ       0

#define MCASP4_TX_IRQ       (156 + 32)	//6638
#define MCASP4_RX_IRQ       (155 + 32)	//6638

#define MCASP5_TX_IRQ       (152 + 32)
#define MCASP5_RX_IRQ       (151 + 32)
#define MCASP6_TX_IRQ       (154 + 32)
#define MCASP6_RX_IRQ       (153 + 32)


#define DMA_RELOAD          64

#define EDMA_BASE_IRQ       0x2000


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/deva/ctrl/mcasp/nto/arm/dll.le.j6_aic3106.v7/variant.h $ $Rev: 740198 $")
#endif
