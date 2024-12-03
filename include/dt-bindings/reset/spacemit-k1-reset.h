// SPDX-License-Identifier: GPL-2.0-+

#ifndef __DT_BINDINGS_RESET_SAPCEMIT_K1_H__
#define __DT_BINDINGS_RESET_SAPCEMIT_K1_H__
//APBC
#define DEBUG 1
#define RESET_UART1   1
#define RESET_UART2   2
#define RESET_GPIO    3
#define RESET_PWM0    4
#define RESET_PWM1    5
#define RESET_PWM2    6
#define RESET_PWM3    7
#define RESET_PWM4    8
#define RESET_PWM5    9
#define RESET_PWM6    10
#define RESET_PWM7    11
#define RESET_PWM8    12
#define RESET_PWM9    13
#define RESET_PWM10   14
#define RESET_PWM11   15
#define RESET_PWM12   16
#define RESET_PWM13   17
#define RESET_PWM14   18
#define RESET_PWM15   19
#define RESET_PWM16   20
#define RESET_PWM17   21
#define RESET_PWM18   22
#define RESET_PWM19   23
#define RESET_SSP3    24
#define RESET_UART3   25
#define RESET_RTC     26
#define RESET_TWSI0   27

#define RESET_TIMERS1 28
#define RESET_AIB     29
#define RESET_TIMERS2 30
#define RESET_ONEWIRE 31
#define RESET_SSPA0   32
#define RESET_SSPA1   33
#define RESET_DRO     34
#define RESET_IR      35
#define RESET_TWSI1   36

#define RESET_TSEN    37
#define RESET_TWSI2   38
#define RESET_TWSI4   39
#define RESET_TWSI5   40
#define RESET_TWSI6   41
#define RESET_TWSI7   42
#define RESET_TWSI8   43
#define RESET_IPC_AP2AUD  44
#define RESET_UART4   45
#define RESET_UART5   46
#define RESET_UART6   47
#define RESET_UART7   48
#define RESET_UART8   49
#define RESET_UART9   50
#define RESET_CAN0    51

//MPMU
#define RESET_WDT     52

//APMU
#define	RESET_JPG        53
#define	RESET_CSI        54
#define	RESET_CCIC2_PHY  55
#define	RESET_CCIC3_PHY  56
#define	RESET_ISP        57
#define	RESET_ISP_AHB    58
#define	RESET_ISP_CI     59
#define	RESET_ISP_CPP    60
#define	RESET_LCD        61
#define	RESET_DSI_ESC    62
#define	RESET_V2D        63
#define	RESET_MIPI       64
#define	RESET_LCD_SPI    65
#define	RESET_LCD_SPI_BUS  66
#define	RESET_LCD_SPI_HBUS 67
#define	RESET_LCD_MCLK     68
#define	RESET_CCIC_4X      69
#define	RESET_CCIC1_PHY    70
#define	RESET_SDH_AXI      71
#define	RESET_SDH0         72
#define	RESET_SDH1         73
#define	RESET_USB_AXI      74
#define	RESET_USBP1_AXI    75
#define	RESET_USB3_0       76
#define	RESET_QSPI         77
#define	RESET_QSPI_BUS     78
#define RESET_DMA          79
#define	RESET_AES          80
#define	RESET_VPU          81
#define	RESET_GPU          82
#define	RESET_SDH2         83
#define	RESET_MC           84
#define	RESET_EM_AXI       85
#define	RESET_EM           86
#define	RESET_AUDIO_SYS    87
#define	RESET_HDMI         88
#define	RESET_PCIE0        89
#define	RESET_PCIE1        90
#define	RESET_PCIE2        91
#define	RESET_EMAC0        92
#define	RESET_EMAC1        93

//APBC2
#define	RESET_SEC_UART1    94
#define	RESET_SEC_SSP2     95
#define	RESET_SEC_TWSI3    96
#define	RESET_SEC_RTC      97
#define	RESET_SEC_TIMERS0  98
#define	RESET_SEC_KPC      99
#define	RESET_SEC_GPIO     100

#define	RESET_NUMBER       101

/* APBC register offset */
#define APBC_UART1_CLK_RST      0x0
#define APBC_UART2_CLK_RST      0x4
#define APBC_GPIO_CLK_RST       0x8
#define APBC_PWM0_CLK_RST       0xc
#define APBC_PWM1_CLK_RST       0x10
#define APBC_PWM2_CLK_RST       0x14
#define APBC_PWM3_CLK_RST       0x18
#define APBC_TWSI8_CLK_RST      0x20
#define APBC_UART3_CLK_RST      0x24
#define APBC_RTC_CLK_RST        0x28
#define APBC_TWSI0_CLK_RST      0x2c
#define APBC_TWSI1_CLK_RST      0x30
#define APBC_TIMERS1_CLK_RST    0x34
#define APBC_TWSI2_CLK_RST      0x38
#define APBC_AIB_CLK_RST        0x3c
#define APBC_TWSI4_CLK_RST      0x40
#define APBC_TIMERS2_CLK_RST    0x44
#define APBC_ONEWIRE_CLK_RST    0x48
#define APBC_TWSI5_CLK_RST      0x4c
#define APBC_DRO_CLK_RST        0x58
#define APBC_IR_CLK_RST         0x5c
#define APBC_TWSI6_CLK_RST      0x60
#define APBC_TWSI7_CLK_RST      0x68
#define APBC_TSEN_CLK_RST       0x6c

#define APBC_UART4_CLK_RST      0x70
#define APBC_UART5_CLK_RST      0x74
#define APBC_UART6_CLK_RST      0x78
#define APBC_SSP3_CLK_RST       0x7c

#define APBC_SSPA0_CLK_RST      0x80
#define APBC_SSPA1_CLK_RST      0x84

#define APBC_IPC_AP2AUD_CLK_RST 0x90
#define APBC_UART7_CLK_RST      0x94
#define APBC_UART8_CLK_RST      0x98
#define APBC_UART9_CLK_RST      0x9c

#define APBC_CAN0_CLK_RST       0xa0
#define APBC_PWM4_CLK_RST       0xa8
#define APBC_PWM5_CLK_RST       0xac
#define APBC_PWM6_CLK_RST       0xb0
#define APBC_PWM7_CLK_RST       0xb4
#define APBC_PWM8_CLK_RST       0xb8
#define APBC_PWM9_CLK_RST       0xbc
#define APBC_PWM10_CLK_RST      0xc0
#define APBC_PWM11_CLK_RST      0xc4
#define APBC_PWM12_CLK_RST      0xc8
#define APBC_PWM13_CLK_RST      0xcc
#define APBC_PWM14_CLK_RST      0xd0
#define APBC_PWM15_CLK_RST      0xd4
#define APBC_PWM16_CLK_RST      0xd8
#define APBC_PWM17_CLK_RST      0xdc
#define APBC_PWM18_CLK_RST      0xe0
#define APBC_PWM19_CLK_RST      0xe4
/* end of APBC register offset */

/* MPMU register offset */
#define MPMU_WDTPCR     0x200
/* end of MPMU register offset */

/* APMU register offset */
#define APMU_JPG_CLK_RES_CTRL       0x20
#define APMU_CSI_CCIC2_CLK_RES_CTRL 0x24
#define APMU_ISP_CLK_RES_CTRL       0x38
#define APMU_LCD_CLK_RES_CTRL1      0x44
#define APMU_LCD_SPI_CLK_RES_CTRL   0x48
#define APMU_LCD_CLK_RES_CTRL2      0x4c
#define APMU_CCIC_CLK_RES_CTRL      0x50
#define APMU_SDH0_CLK_RES_CTRL      0x54
#define APMU_SDH1_CLK_RES_CTRL      0x58
#define APMU_USB_CLK_RES_CTRL       0x5c
#define APMU_QSPI_CLK_RES_CTRL      0x60
#define APMU_USB_CLK_RES_CTRL       0x5c
#define APMU_DMA_CLK_RES_CTRL       0x64
#define APMU_AES_CLK_RES_CTRL       0x68
#define APMU_VPU_CLK_RES_CTRL       0xa4
#define APMU_GPU_CLK_RES_CTRL       0xcc
#define APMU_SDH2_CLK_RES_CTRL      0xe0
#define APMU_PMUA_MC_CTRL           0xe8
#define APMU_PMU_CC2_AP             0x100
#define APMU_PMUA_EM_CLK_RES_CTRL   0x104

#define APMU_AUDIO_CLK_RES_CTRL     0x14c
#define APMU_HDMI_CLK_RES_CTRL      0x1B8

#define APMU_PCIE_CLK_RES_CTRL_0    0x3cc
#define APMU_PCIE_CLK_RES_CTRL_1    0x3d4
#define APMU_PCIE_CLK_RES_CTRL_2    0x3dc

#define APMU_EMAC0_CLK_RES_CTRL     0x3e4
#define APMU_EMAC1_CLK_RES_CTRL     0x3ec
/* end of APMU register offset */

/* APBC2 register offset */
#define APBC2_UART1_CLK_RST		0x00
#define APBC2_SSP2_CLK_RST		0x04
#define APBC2_TWSI3_CLK_RST		0x08
#define APBC2_RTC_CLK_RST		0x0c
#define APBC2_TIMERS0_CLK_RST		0x10
#define APBC2_KPC_CLK_RST		0x14
#define APBC2_GPIO_CLK_RST		0x1c
/* end of APBC2 register offset */

#endif /* __DT_BINDINGS_RESET_SAPCEMIT_K1_H__ */
