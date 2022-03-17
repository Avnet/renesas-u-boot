/*
 * Copyright 2022 AVNET
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __AVNET_FDT_OVERLAY_H
#define __AVNET_FDT_OVERLAY_H

#define GET_OVERLAY_APPLY(dtbo_name)          \
	"echo Applying DT overlay: " dtbo_name " ;"  \
	"fatload mmc ${mmcdev}:${mmcpart} ${dtbo_addr} ${overlaydir}/" dtbo_name "; " \
	"fdt addr ${dtb_addr}; " \
	"fdt apply ${dtbo_addr}; "

#define   FDT_ENTRY_CHECK_RUN_ENV(string, name)     \
        "if env exists " string " && "              \
        "   test ${" string "} = 1 -o ${" string "} = yes ; then " \
            GET_OVERLAY_APPLY(name)           \
        "fi; "

#define   FDT_ENTRY_CHECK_RUN_DISP(name)              \
        "if env exists " name " ; then "              \
            " if test ${" name "} = hdmi ; then "     \
			GET_OVERLAY_APPLY("rzboard-hdmi.dtbo")  \
            " elif test ${" name "} = mipi ; then "   \
            GET_OVERLAY_APPLY("rzboard-mipi.dtbo")  \
            " elif test ${" name "} = dual ; then "   \
            GET_OVERLAY_APPLY("rzboard-dual-display.dtbo")  \
            " else;"                                  \
            "       echo  no found displayer; "       \
            " fi; "                                   \
        "fi; "

/* 
 * enable_overlay_xxx is set in uEnv.txt, then load the corresponding dtbo file
 */
#define FDT_ENTRY_DEF_SETTINGS          \
				FDT_ENTRY_CHECK_RUN_DISP("enable_overlay_disp")  \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_gpio", "rzboard-ext-gpio.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_i2c", "rzboard-ext-i2c.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_pwm", "rzboard-ext-pwm.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_spi", "rzboard-ext-spi.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_sai2", "rzboard-ext-sai2.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_uart2", "rzboard-ext-uart2.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_camera", "rzboard-ov5640.dtbo") \
				FDT_ENTRY_CHECK_RUN_ENV("enable_overlay_usbgadget", "rzboard-usb0-device.dtbo")


#define MMC_RUN_FDT_OVERLAY       \
	"fdt addr ${dtb_addr}; "      \
	"fdt resize 0x10000; "        \
	FDT_ENTRY_DEF_SETTINGS        \
	"if env exists fdt_extra_overlays && test -n $fdt_extra_overlays ; then " \
		"for dtbo_file in ${fdt_extra_overlays};  do "  \
			GET_OVERLAY_APPLY( "${dtbo_file}" )         \
		"done; "                  \
	"fi;"


#define MMC_FDT_OVERLAY_SETTING                  \
	"dtbo_addr=0x48010000\0"                     \
	"overlaydir=overlays\0"                      \
	"mmcbootdto=echo Booting from mmc ...; "     \
		"run mmcbootargs; "                      \
		"if run loadfdt; then "                  \
			MMC_RUN_FDT_OVERLAY                  \
			"booti ${image_addr} - ${dtb_addr}; "  \
		"else "                                  \
			"echo WARN: Cannot load the DT; "    \
		"fi;\0"


 #define MMC_BOOT_WITH_FDT_OVERLAY          \
	 "mmc dev ${mmcdev};"                   \
	 "if mmc rescan; then "                 \
	 	"if run loadimage; then "           \
			"run mmcbootdto; "              \
		"else  "                            \
			"run netboot; "                 \
		"fi; "                              \
	"fi;"

#define   AVNET_UENV_FDTO_SUPPORT

#endif  /* __AVNET_FDT_OVERLAY_H */
