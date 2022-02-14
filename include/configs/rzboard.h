/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 Renesas Electronics Corporation
 */

#ifndef __SMARC_RZV2L_H
#define __SMARC_RZV2L_H

#include <asm/arch/rmobile.h>

#define CONFIG_REMAKE_ELF

#ifdef CONFIG_SPL
#define CONFIG_SPL_TARGET	"spl/u-boot-spl.scif"
#endif

/* boot option */

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/* Generic Interrupt Controller Definitions */
/* RZ/V2L use GIC-v3 */
#define CONFIG_GICV3
#define GICD_BASE	0x11900000
#define GICR_BASE	0x11960000

/* console */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BAUDRATE_TABLE	{ 115200, 38400 }

/* PHY needs a longer autoneg timeout */
#define PHY_ANEG_TIMEOUT		20000

/* MEMORY */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

/* SDHI clock freq */
#define CONFIG_SH_SDHI_FREQ		133000000

#define DRAM_RSV_SIZE			0x08000000
#define CONFIG_SYS_SDRAM_BASE		(0x40000000 + DRAM_RSV_SIZE)
#define CONFIG_SYS_SDRAM_SIZE		(0x80000000u - DRAM_RSV_SIZE) //total 2GB
#define CONFIG_SYS_LOAD_ADDR		0x58000000
#define CONFIG_LOADADDR			CONFIG_SYS_LOAD_ADDR // Default load address for tfpt,bootp...
#define CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED		(0x80000000u - DRAM_RSV_SIZE)

#define CONFIG_SYS_MONITOR_BASE		0x00000000
#define CONFIG_SYS_MONITOR_LEN		(1 * 1024 * 1024)
#define CONFIG_SYS_MALLOC_LEN		(64 * 1024 * 1024)
#define CONFIG_SYS_BOOTM_LEN		(64 << 20)

/* The HF/QSPI layout permits up to 1 MiB large bootloader blob */
#define CONFIG_BOARD_SIZE_LIMIT		1048576

/* support uEnv.txt to pass environment variables to the kernel */
#ifndef CONFIG_CMD_IMPORTENV
#define CONFIG_CMD_IMPORTENV
#endif

#define DEFAULT_MMC_UENV_ARGS \
	"bootenvfile=uEnv.txt\0" \
	"importbootenv=echo Importing environment from mmc${mmcdev} ...; " \
		"env import -t ${image_loadaddr} ${filesize}\0" \
	"loadbootenv=fatload mmc ${mmcdev} ${image_loadaddr} ${bootenvfile}\0" \
	"envboot=mmc dev ${mmcdev}; " \
		"if mmc rescan; then " \
			"echo SD/MMC found on device ${mmcdev};" \
			"if run loadbootenv; then " \
				"echo Loaded env from ${bootenvfile};" \
				"run importbootenv;" \
			"fi;" \
		"fi;\0"

#define MMC_BOOT_UENV  "run envboot;"
#define DEFAULT_MMC_RZV2L_ARGS  DEFAULT_MMC_UENV_ARGS


/* ENV setting */

#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_MMC_RZV2L_ARGS \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"image=Image \0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=" __stringify(CONFIG_SYS_MMC_ENV_PART) "\0" \
	"fdt_loadaddr=0x48000000 \0" \
	"image_loadaddr=0x48080000 \0" \
	"bootm_size=0x10000000 \0" \
	"mmcbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p2 \0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${image_loadaddr} ${image}\0" \
	"loadfdt=echo loading ${fdtfile};fatload mmc ${mmcdev}:${mmcpart} ${fdt_loadaddr} ${fdtfile}\0" \
	"mmcload=mmc dev ${mmcdev};run loadfdt;run loadimage;run mmcbootargs \0" \
	"bootimage=run mmcload; booti 0x48080000 - 0x48000000 \0"

#define CONFIG_BOOTCOMMAND	\
	MMC_BOOT_UENV \
	"run bootimage"

/* For board */
/* Ethernet RAVB */
#define CONFIG_BITBANGMII_MULTI

#endif /* __SMARC_RZV2L_H */
