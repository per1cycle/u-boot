/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) Spacemit
 */

#ifndef _SPACEMIT_K1_H_
#define _SPACEMIT_K1_H_

#define K1_PLAT_GIC_BASE       0xD8000000
#define K1_PLAT_GICC_BASE      0xD8400000
#define K1_PLAT_GICR_BASE      0xD8100000
#define K1_PLAT_CIU_BASE       0xD8440000

#define K1_USB_BASE            0xC0900000
#define USB2_PHY_REG_BASE       0xC0940000
#define K1_AXI_BUS_BASE        0xD4200000
#define K1_SDH2_BASE           0xD4281000
#define K1_SDH0_BASE           0xD4280000
#define K1_SDH1_BASE           0xD4280800
#define K1_CIU_BASE            0xD4282C00  /* cpu config unit */
#define K1_APB_BUS_BASE        0xD4000000
#define K1_SOC_RTC_BASE        0xD4010000
#define K1_SOC_WDT_BASE        0xD4080000
#define K1_MPMU_BASE           0xD4050000
#define K1_APMU_BASE           0xD4282800
#define K1_APB_SPARE_BASE      0xD4090000

/* THERMAL TSEN & AUXADC */
#define K1_APB_SPARE4_BASE     (K1_APB_SPARE_BASE + 0x10C)    /* AUXADC control */
#define K1_AUXADC_BASE         0xD4013300
#define K1_AUXADC_DATA         (K1_AUXADC_BASE + 0x80)
#define K1_AUXADC_ISR          (K1_AUXADC_BASE + 0xF0)

#define K1_ARCH_TIMER_BASE     0xD5001000
#define K1_APBC_BASE           0xD4015000  /* APB Clock Unit */
#define K1_SOC_TIMER_BASE      0xD4014000
#define K1_PDMA_BASE           0xD4000000
#define K1_UART1_BASE          0xD4017000
#define K1_UART2_BASE          0xD4018000
#define K1_UART3_BASE          0xD4017800
#define K1_MFPR_BASE           0xD401e000  /* Multi-Function Pin Registers */
#define K1_MFPR_GPIO26_BASE    (0xD401E000 + 0x20C)
#define K1_GPIO_BASE           0xD4019000
#define K1_SSP0_BASE           0xD401B000
#define K1_SSP2_BASE           0xD401C000
#define K1_PWM0_BASE           0xD401A000
#define K1_PWM1_BASE           0xD401A400
#define K1_TWSI0_BASE          0xD4011000
#define K1_TWSI1_BASE          0xD4010800
#define K1_TWSI2_BASE          0xD4013800
#define K1_TWSI3_BASE          0xD4018800
#define K1_TWSI4_BASE          0xD4020000
#define K1_TWSI5_BASE          0xD4020800
#define K1_TWSI6_BASE          0xD4021000
#define K1_TWSI7_BASE          0xD4021800
#define K1_TWSI8_BASE          0xD4022000
#define K1_RIPC3_BASE          0xD40B0300

#define K1_SWJTAG_BASE         0xD4013100
#define K1_EDGE_WAKEUP_BASE    0xD4019800
#define K1_DDR_BASE            0xC0000000
#define K1_DDR_PHY1_BASE       0xC0100000
#define K1_DDRAXI_MON_BASE     0xC0058500
#define K1_PORT0_BASE          0xD84400F0

#define K1_APMU_DEBUG          (K1_APMU_BASE + 0x88)

/* RTC Backup Registers */
#define REG_RTC_RTC_BR0         (K1_SOC_RTC_BASE + 0x14)

#define K1_DRO_BASE            0xD4013200

#endif /* _SPACEMIT_K1_H_ */
