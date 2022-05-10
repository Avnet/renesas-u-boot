#include <common.h>
#include <cpu_func.h>
#include <image.h>
#include <init.h>
#include <malloc.h>
#include <netdev.h>
#include <dm.h>
#include <dm/platform_data/serial_sh.h>
#include <asm/processor.h>
#include <asm/mach-types.h>
#include <asm/io.h>
#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#include <asm/arch/rmobile.h>
#include <asm/arch/rcar-mstp.h>
#include <asm/arch/sh_sdhi.h>
#include <i2c.h>
#include <mmc.h>

DECLARE_GLOBAL_DATA_PTR;

#define PFC_BASE	0x11030000

#define ETH_CH0		(PFC_BASE + 0x300c)
#define ETH_CH1		(PFC_BASE + 0x3010)
#define ETH_PVDD_3300	0x00
#define ETH_PVDD_1800	0x01
#define ETH_PVDD_2500	0x02
#define ETH_MII_RGMII	(PFC_BASE + 0x3018)

/* CPG */
#define CPG_BASE					0x11010000
#define CPG_CLKON_BASE				(CPG_BASE + 0x500)
#define CPG_RESET_BASE				(CPG_BASE + 0x800)
#define CPG_RESET_ETH				(CPG_RESET_BASE + 0x7C)
#define CPG_PL2_SDHI_DSEL			(CPG_BASE + 0x218)
#define CPG_CLK_STATUS				(CPG_BASE + 0x280)

void s_init(void)
{
	/* can go in board_eht_init() once enabled */
	*(volatile u32 *)(ETH_CH0) = (*(volatile u32 *)(ETH_CH0) & 0xFFFFFFFC) | ETH_PVDD_1800;
	/* Enable RGMII for both ETH{0,1} */
	*(volatile u32 *)(ETH_MII_RGMII) = (*(volatile u32 *)(ETH_MII_RGMII) & 0xFFFFFFFC);
	/* ETH CLK */
	*(volatile u32 *)(CPG_RESET_ETH) = 0x30003;
	/* SD CLK */
	*(volatile u32 *)(CPG_PL2_SDHI_DSEL) = 0x00110011;
	while (*(volatile u32 *)(CPG_CLK_STATUS) != 0)
		;
}

#define PFC_P18				(PFC_BASE + 0x018)
#define PFC_PM18				(PFC_BASE + 0x130)
#define PFC_PMC18				(PFC_BASE + 0x218)
#define PFC_P21				(PFC_BASE + 0x142)
#define PFC_PM21				(PFC_BASE + 0x128)
#define PFC_PMC21				(PFC_BASE + 0x221)
#define PFC_P23				(PFC_BASE + 0x023)
#define PFC_PM23				(PFC_BASE + 0x146)
#define PFC_PMC23				(PFC_BASE + 0x223)
int board_led_init(void)
{
	/* RED LED: P8_1 = 0; */
	*(volatile u32 *)(PFC_PMC18) &= 0xFFFFFFFD;
	*(volatile u32 *)(PFC_PM18) = (*(volatile u32 *)(PFC_PM18) & 0xFFFFFFF3) | 0x08;
	*(volatile u32 *)(PFC_P18) = (*(volatile u32 *)(PFC_P18) & 0xFFFFFFFD) | 0x0;
	/* GREEN LED: P17_2 = 1; */
	*(volatile u32 *)(PFC_PMC21) &= 0xFFFFFFFB;
	*(volatile u32 *)(PFC_PM21) = (*(volatile u32 *)(PFC_PM21) & 0xFFFFFFCF) | 0x20;
	*(volatile u32 *)(PFC_P21) = (*(volatile u32 *)(PFC_P21) & 0xFFFFFFFB) | 0x4;
	/* BLUE LED: P19_1 = 0; */
	*(volatile u32 *)(PFC_PMC23) &= 0xFFFFFFFD;
	*(volatile u32 *)(PFC_PM23) = (*(volatile u32 *)(PFC_PM23) & 0xFFFFFFF3) | 0x08;
	*(volatile u32 *)(PFC_P23) = (*(volatile u32 *)(PFC_P23) & 0xFFFFFFFD) | 0x0;

    return 0;
}

int board_early_init_f(void)
{
	/* LED's */
	board_led_init();

	return 0;
}

/* PFC */
#define PFC_P37						(PFC_BASE + 0x037)
#define PFC_PM37					(PFC_BASE + 0x16E)
#define PFC_PMC37					(PFC_BASE + 0x237)

#define CONFIG_SYS_SH_SDHI0_BASE	0x11C00000
#define CONFIG_SYS_SH_SDHI1_BASE	0x11C10000
int board_mmc_init(struct bd_info *bis)
{
	int ret = 0;
	
	/* SD1 power control: P39_1 = 0; P39_2 = 1; */
	*(volatile u32 *)(PFC_PMC37) &= 0xFFFFFFF9; /* Port func mode 0b00 */
	*(volatile u32 *)(PFC_PM37) = (*(volatile u32 *)(PFC_PM37) & 0xFFFFFFC3) | 0x28; /* Port output mode 0b1010 */
	*(volatile u32 *)(PFC_P37) = (*(volatile u32 *)(PFC_P37) & 0xFFFFFFF9) | 0x6;	/* Port 39[2:1] output value 0b11*/
		
	ret |= sh_sdhi_init(CONFIG_SYS_SH_SDHI0_BASE,
						0,
						SH_SDHI_QUIRK_64BIT_BUF);

	return ret;
}

#define PFC_P14				(PFC_BASE + 0x014)
#define PFC_PM14				(PFC_BASE + 0x128)
#define PFC_PMC14				(PFC_BASE + 0x214)
int board_eth_init(struct bd_info *bis)
{
	/* Ethernet PHY Reset: P4_1 = 1; */
	*(volatile u32 *)(PFC_PMC14) &= 0xFFFFFFFD;
	*(volatile u32 *)(PFC_PM14) = (*(volatile u32 *)(PFC_PM14) & 0xFFFFFFF3) | 0x08;
	*(volatile u32 *)(PFC_P14) = (*(volatile u32 *)(PFC_P14) & 0xFFFFFFFD) | 0x2;

    return 0;
}

#define PFC_P10				(PFC_BASE + 0x010)
#define PFC_PM10				(PFC_BASE + 0x120)
#define PFC_PMC10				(PFC_BASE + 0x210)
int usbhub_reset(void)
{
	/* USB Hub Reset: P0_0 = 1; */
	*(volatile u32 *)(PFC_PMC10) &= 0xFFFFFFFE;
	*(volatile u32 *)(PFC_PM10) = (*(volatile u32 *)(PFC_PM10) & 0xFFFFFFFC) | 0x02;
	*(volatile u32 *)(PFC_P10) = (*(volatile u32 *)(PFC_P10) & 0xFFFFFFFE) | 0x0;
	mdelay(2);
	*(volatile u32 *)(PFC_P10) = (*(volatile u32 *)(PFC_P10) & 0xFFFFFFFE) | 0x1;

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_TEXT_BASE + 0x50000;

	/* reset usbhub */
	usbhub_reset();

	return 0;
}

void reset_cpu(ulong addr)
{

}
