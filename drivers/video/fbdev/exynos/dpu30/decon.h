/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Header file for Exynos DECON driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef ___SAMSUNG_DECON_H__
#define ___SAMSUNG_DECON_H__

#include <linux/fb.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/pm_qos.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/platform_device.h>
#include <media/v4l2-device.h>
#include <media/videobuf2-core.h>
#if defined(CONFIG_EXYNOS_BTS)
#include <soc/samsung/bts.h>
#endif
#if defined(CONFIG_EXYNOS_ITMON)
#include <soc/samsung/exynos-itmon.h>
#endif
#if defined(CONFIG_EXYNOS_PD)
#include <soc/samsung/exynos-pd.h>
#endif
#include <linux/exynos_iovmm.h>
#include <linux/sync_file.h>
#include <soc/samsung/exynos-debug.h>

/* TODO: SoC dependency will be removed */
#include "./cal_9830/regs-decon.h"
#include "./cal_9830/decon_cal.h"

#include "./panels/exynos_panel.h"
#include "dsim.h"
#include "displayport.h"
#include "hdr_metadata.h"
#include "format.h"

#ifdef CONFIG_EXYNOS_COMMON_PANEL
#include "../panel/panel_drv.h"
#endif

#ifdef CONFIG_SEC_ABC
#include <linux/sti/abc_common.h>
#endif

/*#define CONFIG_PROFILE_WINCONFIG	1*/

#define SUCCESS_EXYNOS_SMC	0

#define MAX_DECON_CNT		3
#define MAX_DECON_WIN		6
#define MAX_DPP_SUBDEV		7

#define DISP_RESTRICTION_VER	20190612

extern struct decon_device *decon_drvdata[MAX_DECON_CNT];
extern int decon_log_level;
extern int dpu_bts_log_level;
extern int win_update_log_level;
extern int dpu_mres_log_level;
extern int dpu_fence_log_level;
extern int dpu_dma_buf_log_level;
extern int decon_systrace_enable;
extern struct decon_bts_ops decon_bts_control;

#define DECON_MODULE_NAME	"exynos-decon"
#define MAX_NAME_SIZE		32
#define MAX_PLANE_CNT		3
#define MAX_PLANE_ADDR_CNT	4
#define DECON_ENTER_HIBER_CNT	4
#define MIN_BLK_MODE_WIDTH	144
#define MIN_BLK_MODE_HEIGHT	16
#define VSYNC_TIMEOUT_MSEC	200
#define DEFAULT_BPP		32
#define MIN_WIN_BLOCK_WIDTH	8
#define MIN_WIN_BLOCK_HEIGHT	1
#define FD_TRY_CNT		3
#define VALID_FD_VAL		3
#define DECON_TRACE_BUF_SIZE	40

#define DECON_WIN_UPDATE_IDX	MAX_DECON_WIN

#ifndef KHZ
#define KHZ (1000)
#endif
#ifndef MHZ
#define MHZ (1000*1000)
#endif
#ifndef MSEC
#define MSEC (1000)
#endif

#define SHADOW_UPDATE_TIMEOUT	(300 * 1000) /* 300ms */
#define IDLE_WAIT_TIMEOUT	(50 * 1000) /* 50ms */
#define RUN_WAIT_TIMEOUT	IDLE_WAIT_TIMEOUT
#define DSC_INIT_XMIT_DELAY	0x200

#define EINT_PEND(x)		((x == 0) ? 2 : ((x == 1) ? 4 : 1))

#define MAX_DSC_SLICE_CNT	4

/*#define DEBUG_DMA_BUF_LEAK	1*/

#if defined(CONFIG_EXYNOS_COMMON_PANEL)
#define MIN_FRAME_DONE_ERR_CHECK_USEC (8000) /* 8ms */
#define FRAME_DONE_ERR_CHECK_SKIP_TIMEOUT (3000) /* 3s */
#endif

void dpu_debug_printk(const char *function_name, const char *format, ...);
#define decon_err(fmt, ...)							\
	do {									\
		if (decon_log_level >= 3) {					\
			pr_err(pr_fmt(fmt), ##__VA_ARGS__);			\
		}								\
	} while (0)

#define decon_warn(fmt, ...)							\
	do {									\
		if (decon_log_level >= 4) {					\
			pr_warn(pr_fmt(fmt), ##__VA_ARGS__);			\
		}								\
	} while (0)

#define decon_info(fmt, ...)							\
	do {									\
		if (decon_log_level >= 6)					\
			pr_info(pr_fmt(fmt), ##__VA_ARGS__);			\
	} while (0)

#define decon_dbg(fmt, ...)							\
	do {									\
		if (decon_log_level >= 7)					\
			pr_info(pr_fmt(fmt), ##__VA_ARGS__);			\
	} while (0)

#define DPU_DEBUG_WIN(fmt, args...)						\
	do {									\
		if (win_update_log_level >= 7)					\
			dpu_debug_printk("WIN_UPDATE", fmt,  ##args);		\
	} while (0)

#define DPU_DEBUG_BTS(fmt, args...)						\
	do {									\
		if (dpu_bts_log_level >= 7)					\
			dpu_debug_printk("BTS", fmt,  ##args);			\
	} while (0)

#define DPU_INFO_BTS(fmt, args...)						\
	do {									\
		if (dpu_bts_log_level >= 6)					\
			dpu_debug_printk("BTS", fmt,  ##args);			\
	} while (0)

#define DPU_ERR_BTS(fmt, args...)						\
	do {									\
		if (dpu_bts_log_level >= 3)					\
			dpu_debug_printk("BTS", fmt, ##args);			\
	} while (0)

#define DPU_DEBUG_MRES(fmt, args...)						\
	do {									\
		if (dpu_mres_log_level >= 7)					\
			dpu_debug_printk("MRES", fmt,  ##args);			\
	} while (0)

#define DPU_INFO_MRES(fmt, args...)						\
	do {									\
		if (dpu_mres_log_level >= 6)					\
			dpu_debug_printk("MRES", fmt,  ##args);			\
	} while (0)

#define DPU_ERR_MRES(fmt, args...)						\
	do {									\
		if (dpu_mres_log_level >= 3)					\
			dpu_debug_printk("MRES", fmt, ##args);			\
	} while (0)

#define DPU_DEBUG_FENCE(fmt, args...)						\
	do {									\
		if (dpu_fence_log_level >= 7)					\
			dpu_debug_printk("FENCE", fmt,  ##args);			\
	} while (0)

#define DPU_INFO_FENCE(fmt, args...)						\
	do {									\
		if (dpu_fence_log_level >= 6)					\
			dpu_debug_printk("FENCE", fmt,  ##args);			\
	} while (0)

#define DPU_ERR_FENCE(fmt, args...)						\
	do {									\
		if (dpu_fence_log_level >= 3)					\
			dpu_debug_printk("FENCE", fmt, ##args);			\
	} while (0)

#define DPU_DEBUG_DMA_BUF(fmt, args...)						\
	do {									\
		if (dpu_dma_buf_log_level >= 7)					\
			dpu_debug_printk("DMA_BUF", fmt,  ##args);			\
	} while (0)
#ifdef CONFIG_EXYNOS_MCD_HDR
#define IS_HDR_FMT(hdr_std) ((hdr_std == DPP_HDR_ST2084) || (hdr_std == DPP_HDR_HLG))
#endif
/* DECON systrace related */
void decon_tracing_mark_write(struct decon_device *decon, char id, char *str1, int value);
#define decon_systrace(decon, id, str1, value)					\
	do {									\
		if (decon_systrace_enable)					\
			decon_tracing_mark_write(decon, id, str1, value);		\
	} while (0)

enum decon_hold_scheme {
	/*  should be set to this value in case of DSIM video mode */
	DECON_VCLK_HOLD_ONLY		= 0x00,
	/*  should be set to this value in case of DSIM command mode */
	DECON_VCLK_RUNNING_VDEN_DISABLE = 0x01,
	DECON_VCLK_HOLD_VDEN_DISABLE	= 0x02,
	/*  should be set to this value in case of HDMI, eDP */
	DECON_VCLK_NOT_AFFECTED		= 0x03,
};

enum decon_win_alpha_coef {
	BND_COEF_ZERO			= 0x0,
	BND_COEF_ONE			= 0x1,
	BND_COEF_AF			= 0x2,
	BND_COEF_1_M_AF		= 0x3,
	BND_COEF_AB			= 0x4,
	BND_COEF_1_M_AB		= 0x5,
	BND_COEF_PLNAE_ALPHA0		= 0x6,
	BND_COEF_1_M_PLNAE_ALPHA0	= 0x7,
	BND_COEF_PLNAE_ALPHA1		= 0x8,
	BND_COEF_1_M_PLNAE_ALPHA1	= 0x9,
	BND_COEF_ALPHA_MULT		= 0xA,
	BND_COEF_1_M_ALPHA_MULT	= 0xB,
};

enum decon_win_alpha_sel {
	ALPHA_MULT_SRC_SEL_ALPHA0 = 0,
	ALPHA_MULT_SRC_SEL_ALPHA1 = 1,
	ALPHA_MULT_SRC_SEL_AF = 2,
	ALPHA_MULT_SRC_SEL_AB = 3,
};

enum decon_merger_mode {
	DECON_LRM_NO		= 0x0,
	DECON_LRM_NOSWAP_RF	= 0x4,
	DECON_LRM_NOSWAP_LF	= 0x5,
	DECON_LRM_SWAP_RF	= 0x6,
	DECON_LRM_SWAP_LF	= 0x7,
};

enum decon_te_src {
	DECON_TE_FROM_DDI0 = 0,
	DECON_TE_FROM_DDI1,
	DECON_TE_FROM_DDI2,
	DECON_TE_FROM_USB,
};

/*
 * DECON_STATE_ON : disp power on, decon/dsim clock on & lcd on
 * DECON_HIBER : disp power off, decon/dsim clock off & lcd on
 * DECON_STATE_OFF : disp power off, decon/dsim clock off & lcd off
 */
enum decon_state {
	DECON_STATE_INIT = 0,
	DECON_STATE_ON,
	DECON_STATE_DOZE,
	DECON_STATE_HIBER,
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
	DECON_STATE_DOZE_WAKE,
#endif
	DECON_STATE_DOZE_SUSPEND,
	DECON_STATE_OFF,
	DECON_STATE_TUI,
	MAX_DECON_STATE,
};

/* To find a proper CLOCK ratio */
enum decon_clk_id {
	CLK_ID_VCLK = 0,
	CLK_ID_ECLK,
	CLK_ID_ACLK,
	CLK_ID_PCLK,
	CLK_ID_DPLL, /* DPU_PLL */
	CLK_ID_RESOLUTION,
	CLK_ID_MIC_RATIO,
	CLK_ID_DSC_RATIO,
	CLK_ID_MAX,
};

enum decon_dsc_id {
	DECON_DSC_ENC0 = 0x0,
	DECON_DSC_ENC1 = 0x1,
	DECON_DSC_ENC2 = 0x2,
};

enum decon_share_path {
	SHAREPATH_DQE_USE		= 0x0,
	SHAREPATH_VG0_USE		= 0x1,
	SHAREPATH_VG1_USE		= 0x2,
	SHAREPATH_VGF1_USE		= 0x3,
	SHAREPATH_VGF0_USE		= 0x4,
};

enum dpp_rotate {
	DPP_ROT_NORMAL = 0x0,
	DPP_ROT_XFLIP,
	DPP_ROT_YFLIP,
	DPP_ROT_180,
	DPP_ROT_90,
	DPP_ROT_90_XFLIP,
	DPP_ROT_90_YFLIP,
	DPP_ROT_270,
};

#define CSC_STANDARD_MASK	0x3F	/* 6 bits */
#define CSC_RANGE_MASK		0x7	/* 3 bits */
#define CSC_TRANSFER_MASK	0x1F	/* 5 bits */
enum dpp_csc_eq {
	/* eq_mode : 6bits [5:0] */
	CSC_STANDARD_SHIFT = 0,
	CSC_BT_601 = 0,
	CSC_BT_709 = 1,
	CSC_BT_2020 = 2,
	CSC_DCI_P3 = 3,
	CSC_BT_601_625 = 4,
	CSC_BT_601_625_UNADJUSTED = 5,
	CSC_BT_601_525 = 6,
	CSC_BT_601_525_UNADJUSTED = 7,
	CSC_BT_2020_CONSTANT_LUMINANCE = 8,
	CSC_BT_470M = 9,
	CSC_FILM = 10,
	CSC_ADOBE_RGB = 11,
	CSC_STANDARD_UNSPECIFIED = 63,
	/* eq_mode : 3bits [8:6] */
	CSC_RANGE_SHIFT = 6,
	CSC_RANGE_LIMITED = 0x0,
	CSC_RANGE_FULL = 0x1,
	CSC_RANGE_EXTENDED = 0x2,
	CSC_RANGE_UNSPECIFIED = 7,
	/* transfer : 5bits [13:9] */
	CSC_TRANSFER_SHIFT = 9,
};

enum dpp_comp_src {
	DPP_COMP_SRC_NONE = 0,
	DPP_COMP_SRC_G2D,
	DPP_COMP_SRC_GPU
};

enum dpp_hdr_standard {
	DPP_HDR_OFF = 0,
#ifdef CONFIG_EXYNOS_MCD_HDR
	DPP_TRANSFER_UNSPECIFIED = 0,
#endif
	DPP_HDR_ST2084 = 1,
	DPP_HDR_HLG = 2,
	DPP_TRANSFER_LINEAR = 3,
	DPP_TRANSFER_SRGB = 4,
	DPP_TRANSFER_SMPTE_170M = 5,
	DPP_TRANSFER_GAMMA2_2 = 6,
	DPP_TRANSFER_GAMMA2_6 = 7,
	DPP_TRANSFER_GAMMA2_8 = 8,
};

#if defined(CONFIG_EXYNOS_DECON_DQE)
/* HAL intent info */
enum HAL_intent_info {
	HAL_RENDER_INTENT_COLORIMETRIC = 0,
	HAL_RENDER_INTENT_ENHANCE = 1,
	HAL_RENDER_INTENT_TONE_MAP_COLORIMETRIC = 2,
	HAL_RENDER_INTENT_TONE_MAP_ENHANCE = 3,
};

/* HAL color transform*/
enum HAL_color_transform {
	HAL_COLOR_TRANSFORM_IDENTITY = 0,
	HAL_COLOR_TRANSFORM_ARBITRARY_MATRIX = 1,
	HAL_COLOR_TRANSFORM_VALUE_INVERSE = 2,
	HAL_COLOR_TRANSFORM_GRAYSCALE = 3,
	HAL_COLOR_TRANSFORM_CORRECT_PROTANOPIA = 4,
	HAL_COLOR_TRANSFORM_CORRECT_DEUTERANOPIA = 5,
	HAL_COLOR_TRANSFORM_CORRECT_TRITANOPIA = 6,
};

/* intents num and information in each color mode*/
struct decon_render_intents_num_info {
	u32 color_mode;
	u32 render_intent_num;
};

struct decon_render_intent_info {
	u32 color_mode;
	u32 index;
	u32 render_intent;
};

/* decon supported intent info */
enum decon_supported_intent_info {
	DECON_INTENT_COLORIMETRIC = 0,
	DECON_INTENT_ENHANCE,
	DECON_INTENT_NUM_MAX,
};

#define DECON_MATRIX_ELEMENT_NUM 16
struct decon_color_transform_info {
	u32 hint;
	int matrix[DECON_MATRIX_ELEMENT_NUM];
};

struct decon_color_mode_with_render_intent_info {
	u32 color_mode;
	u32 render_intent;
};
#endif

enum decon_color_mode {
	HAL_COLOR_MODE_NATIVE = 0,
#ifdef CONFIG_EXYNOS_MCD_HDR
	HAL_COLOR_MODE_STANDARD_BT601_625			 = 1,
	HAL_COLOR_MODE_STANDARD_BT601_625_UNADJUSTED = 2,
	HAL_COLOR_MODE_STANDARD_BT601_525			 = 3,
	HAL_COLOR_MODE_STANDARD_BT601_525_UNADJUSTED = 4,
	HAL_COLOR_MODE_STANDARD_BT709				 = 5,
	HAL_COLOR_MODE_DCI_P3						 = 6,
	HAL_COLOR_MODE_SRGB 						 = 7,
	HAL_COLOR_MODE_ADOBE_RGB					 = 8,
	HAL_COLOR_MODE_DISPLAY_P3					 = 9,
 #endif
	HAL_COLOR_MODE_NUM_MAX,
};

/* decon supported color mode */
enum decon_supported_color_mode {
	DECON_COLOR_MODE_NATIVE = 0,
	DECON_COLOR_MODE_SRGB,
	DECON_COLOR_MODE_DCI_P3,
	DECON_COLOR_MODE_NUM_MAX,
};


#define VRR_UPDATE				0x80000000
#define WIN_VRR_HS_MODE			1
#define WIN_VRR_NORMAL_MODE		0

struct decon_color_mode_info {
	int index;
	u32 color_mode;
};

struct decon_clocks {
	unsigned long decon[CLK_ID_DPLL + 1];
};

struct decon_dma_buf_data {
	struct dma_buf			*dma_buf;
	struct dma_buf_attachment	*attachment;
	struct sg_table			*sg_table;
	dma_addr_t			dma_addr;
	struct dma_fence		*fence;
	int				dpp_ch;
};

struct decon_win_rect {
	int x;
	int y;
	u32 w;
	u32 h;
};

struct decon_rect {
	u32 left;
	u32 top;
	u32 right;
	u32 bottom;
};

struct dpp_params {
	dma_addr_t addr[MAX_PLANE_CNT];
	enum dpp_rotate rot;
	enum dpp_csc_eq eq_mode;
	enum dpp_comp_src comp_src;
	enum dpp_hdr_standard hdr_std;
	u32 min_luminance;
	u32 max_luminance;
};

struct decon_frame {
	int x;
	int y;
	u32 w;
	u32 h;
	u32 f_w;
	u32 f_h;
};

struct decon_readback {
	bool enabled;
	bool request;
	bool drm;
	u8 num_buffers;
	struct decon_dma_buf_data dma_buf_data[MAX_PLANE_CNT];
	struct dma_fence *fence;
	struct work_struct work;
	struct workqueue_struct *wq;
	u32 map_cnt;
	u32 unmap_cnt;
};

struct vrr_config_data {
	u32 fps;
	u32 mode;
};

struct decon_win_config {
	enum {
		DECON_WIN_STATE_DISABLED = 0,
		DECON_WIN_STATE_COLOR,
		DECON_WIN_STATE_BUFFER,
		DECON_WIN_STATE_UPDATE,
		DECON_WIN_STATE_CURSOR,
		DECON_WIN_STATE_MRESOL = 0x10000,
		DECON_WIN_STATE_VRR_NORMALMODE= 0x20000,
		DECON_WIN_STATE_VRR_HSMODE = 0x20001,
#ifdef CONFIG_SUPPORT_MASK_LAYER
		DECON_WIN_STATE_FINGERPRINT = 0x30000,
#endif
	} state;

	/* Reusability:This struct is used for IDMA and ODMA */
	union {
		__u32 color;
		struct {
			int				fd_idma[3];
			int				acq_fence;
			int				rel_fence;
			enum decon_blending		blending;
			/* channel id of DECON CHMAP */
			u32				channel;
			enum decon_pixel_format		format;
			struct dpp_params		dpp_parm;
			/* no read area of IDMA */
			struct decon_win_rect		block_area;
			struct decon_win_rect		transparent_area;
			struct decon_win_rect		opaque_area;
			/* source framebuffer coordinates */
			struct decon_frame		src;
		};
	};

	/* destination OSD coordinates */
	struct decon_frame dst;
	bool protection;
	bool compression;
	int plane_alpha;
};

struct decon_reg_data {
	u32 num_of_window;
	int plane_cnt[MAX_DECON_WIN + 2];
	struct list_head list;
	struct decon_rect blender_bg;
	struct decon_win_config dpp_config[MAX_DECON_WIN + 2];
	struct decon_win_rect block_rect[MAX_DECON_WIN];
	struct decon_window_regs win_regs[MAX_DECON_WIN + 2];
	struct decon_dma_buf_data dma_buf_data[MAX_DECON_WIN + 2][MAX_PLANE_CNT];
	struct dma_fence *retire_fence;
	struct decon_readback readback;

	/*
	 * If window update size is changed, that size has to be applied to
	 * DECON, DSIM and panel in case of below
	 * - full size -> partial size
	 * - partial size -> different partial size
	 * - partial size -> full size
	 *
	 * need_update flag indicates whether changes are applied to hw or not
	 */
	bool need_update;
	/* current update region */
	struct decon_rect up_region;
	/* protected contents playback */
	bool protection[MAX_DECON_WIN + 2];
	/* cursor async */
	bool is_cursor_win[MAX_DECON_WIN];
	int cursor_win;

	bool mode_update;
	u32 lcd_width;
	u32 lcd_height;

	u32 fps_update;
	struct vrr_config_data vrr_config;

	int mode_idx;
#ifdef CONFIG_DYNAMIC_FREQ
	int df_update;
#endif
#ifdef CONFIG_SUPPORT_MASK_LAYER
	bool mask_layer;
#endif

#ifdef CONFIG_PROFILE_WINCONFIG
	ktime_t create_time;
	int remained_frames;
#endif

#ifdef DEBUG_DMA_BUF_LEAK
	int import_cnt;
	ktime_t import_time;
#endif

#ifdef CONFIG_EXYNOS_SET_ACTIVE_WITH_EMPTY_WINDOW
	u32 fps;
#endif
	unsigned long idx;
};

struct decon_win_config_extra {
	int remained_frames;
	u32 reserved[7];
};

struct decon_win_config_data_old {
	int	retire_fence;
	int	fd_odma;
	u32	fps;
	struct decon_win_config config[MAX_DECON_WIN + 2];
};

struct decon_win_config_data {
	int	retire_fence;
	int	fd_odma;
	u32	fps;
	struct decon_win_config config[MAX_DECON_WIN + 2];
	struct decon_win_config_extra extra;
};

enum lcd_status {
	LCD_OFF = 0,
	LCD_ON  = 1,
};

enum hwc_ver {
	HWC_INIT = 0,
	HWC_1_0 = 1,
	HWC_2_0 = 2,
	HWC_2_3 = 3,
};

struct decon_disp_info {
	enum hwc_ver ver;
	enum decon_psr_mode psr_mode;
	struct lcd_mres_info mres_info;
	u32 chip_ver;
	unsigned char reverved[128];
};

struct dpu_size_info {
	u32 w_in;
	u32 h_in;
	u32 w_out;
	u32 h_out;
};

struct decon_readback_attribute {
	u32 format;
	u32 dataspace;
};

/**
 * Display Subsystem event management status.
 *
 * These status labels are used internally by the DECON to indicate the
 * current status of a device with operations.
 */
typedef enum dpu_event_type {
	/* Related with FB interface */
	DPU_EVT_BLANK = 0,
	DPU_EVT_UNBLANK,
	DPU_EVT_ACT_VSYNC,
	DPU_EVT_DEACT_VSYNC,
	DPU_EVT_WIN_CONFIG,
	DPU_EVT_DISP_INFO,

	/* Related with interrupt */
	DPU_EVT_TE_INTERRUPT,
	DPU_EVT_UNDERRUN,
	DPU_EVT_DECON_FRAMEDONE,
	DPU_EVT_DSIM_FRAMEDONE,
	DPU_EVT_RSC_CONFLICT,

	/* Related with async event */
	DPU_EVT_UPDATE_HANDLER,
	DPU_EVT_DSIM_COMMAND,
	DPU_EVT_TRIG_MASK,
	DPU_EVT_TRIG_UNMASK,
	DPU_EVT_FENCE_RELEASE,
	DPU_EVT_DECON_FRAMEDONE_WAIT,
	DPU_EVT_DECON_SHUTDOWN,
	DPU_EVT_DSIM_SHUTDOWN,
	DPU_EVT_DECON_FRAMESTART,

	/* Related with DPP */
	DPU_EVT_DPP_WINCON,
	DPU_EVT_DPP_FRAMEDONE,
	DPU_EVT_DPP_STOP,
	DPU_EVT_DPP_UPDATE_DONE,
	DPU_EVT_DPP_SHADOW_UPDATE,
	DPU_EVT_DPP_SUSPEND,
	DPU_EVT_DPP_RESUME,

	/* Related with PM */
	DPU_EVT_DECON_SUSPEND,
	DPU_EVT_DECON_RESUME,
	DPU_EVT_ENTER_HIBER,
	DPU_EVT_EXIT_HIBER,
	DPU_EVT_DSIM_SUSPEND,
	DPU_EVT_DSIM_RESUME,
	DPU_EVT_ENTER_ULPS,
	DPU_EVT_EXIT_ULPS,

	DPU_EVT_LINECNT_ZERO,

	/* write-back events */
	DPU_EVT_WB_SET_BUFFER,
	DPU_EVT_WB_SW_TRIGGER,

	DPU_EVT_DMA_FRAMEDONE,
	DPU_EVT_DMA_RECOVERY,

	DPU_EVT_DECON_SET_BUFFER,
	/* cursor async */
	DPU_EVT_CURSOR_POS,

	/* window update */
	DPU_EVT_WINUP_UPDATE_REGION,
	DPU_EVT_WINUP_FLAGS,
	DPU_EVT_WINUP_APPLY_REGION,

	DPU_EVT_DOZE,
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
	DPU_EVT_DOZE_WAKE,
#endif
	DPU_EVT_DOZE_SUSPEND,

	DPU_EVT_MEM_MAP,
	DPU_EVT_MEM_UNMAP,

	DPU_EVT_ACQUIRE_RSC,
	DPU_EVT_RELEASE_RSC,
	DPU_EVT_STORE_RSC,

	DPU_EVT_MAX, /* End of EVENT */
} dpu_event_t;

/* Location of calling DPU_EVT_WIN_CONFIG & DPU_EVT_UPDATE_HANDLER */
enum dpu_wc_id {
	WC_ID_FAIL = BIT(0),
	WC_ID_SKIP = BIT(1),
	WC_ID_NORMAL = BIT(2),
	WC_ID_FPS = BIT(3),
};

enum dpu_uh_id {
	UH_ID_IFENCE_ERR = 0,
	UH_ID_OFENCE_ERR,
	UH_ID_DPP_ERR,
	UH_ID_NORMAL,
	UH_ID_LAST_REGS,
};

/* Related with Cursor */
struct disp_log_cursor {
	u32 xpos;
	u32 ypos;
	ktime_t elapsed;	/* End time - Start time */
};

/* Related with Fence */
struct disp_log_fence {
	u32 timeline_value;
	int timeline_max;
};

/* Related with PM */
struct disp_log_pm {
	u32 pm_status;		/* ACTIVE(1) or SUSPENDED(0) */
	ktime_t elapsed;	/* End time - Start time */
};

struct decon_win_rawdata {
	u32 id;
	unsigned long idx;
	u32 fps;
	u32 state;
	struct decon_frame winup;
};

/* Related with S3CFB_WIN_CONFIG */
struct decon_update_reg_data {
	struct decon_window_regs 	win_regs[MAX_DECON_WIN];
	struct decon_win_config 	win_config[MAX_DECON_WIN + 2];
	struct decon_win_rect 		win;
	struct decon_win_rawdata 	win_raw;
#if defined(CONFIG_EXYNOS_COMMON_PANEL)
	struct decon_rect		up_region;
#endif
};

/* Related with MIPI COMMAND read/write */
#define DPU_CALLSTACK_MAX 10
struct dsim_log_cmd_buf {
	u32 id;
	u8 buf;
	u32 size;
	u32 pl_cnt;
	u32 line_cnt;
	void *caller[DPU_CALLSTACK_MAX];
};

/* Related with DPP */
struct disp_log_dpp {
	u32 id;
	u32 start_cnt;
	u32 done_cnt;
	u32 comp;
	u32 rot;
	u32 hdr_std;
	struct decon_frame src;
	struct decon_frame dst;
};

/* Related with window update information */
struct disp_log_winup {
	struct decon_frame req_region;
	struct decon_frame adj_region;
	struct decon_frame apl_region;
	bool need_update;
	bool reconfigure;
};

struct disp_log_memmap {
	dma_addr_t dma_addr;
	u32 shd_addr[MAX_PLANE_ADDR_CNT];
	int dpp_ch;
	struct sg_table *sg_table;
};

struct disp_log_rsc {
	unsigned long prev_used_dpp;
	unsigned long cur_using_dpp;
	unsigned long prev_req_win;
	unsigned long cur_req_win;
	unsigned int hw_ch_info;  /* DPP channels actually occupied by HW */
	unsigned int hw_win_info; /* windows actually occupied by HW */
};

/**
 * struct dpu_log - Display Subsystem Log
 * This struct includes DECON/DSIM/DPP
 */
struct dpu_log {
	ktime_t time;
	dpu_event_t type;
	union {
		struct disp_log_dpp dpp;
		struct decon_update_reg_data reg;
		struct dsim_log_cmd_buf cmd_buf;
		struct disp_log_pm pm;
		struct disp_log_fence fence;
		struct disp_log_cursor cursor;
		struct disp_log_winup winup;
		struct disp_log_memmap memmap;
		struct disp_log_rsc rsc;
		struct decon_win_rawdata win_raw;
	} data;
};

struct dpu_size_err_info {
	ktime_t time;
	struct dpu_size_info info;
};

/* Definitions below are used in the DECON */
#define	DPU_EVENT_LOG_MAX	SZ_2K	/* 2048 */
#define	DPU_EVENT_PRINT_MAX	(DPU_EVENT_LOG_MAX >> 1)
#define	DPU_EVENT_LOG_RETRY	3
#define DPU_EVENT_KEEP_CNT	3
typedef enum dpu_event_log_level_type {
	DPU_EVENT_LEVEL_LOW = 0,
	DPU_EVENT_LEVEL_HIGH,
} dpu_log_level_t;

/* APIs below are used in the DECON/DSIM/DPP driver */
#define DPU_EVENT_START() ktime_t start = ktime_get()
void DPU_EVENT_LOG(dpu_event_t type, struct v4l2_subdev *sd, ktime_t time);
void DPU_EVENT_LOG_WINCON(struct v4l2_subdev *sd, struct decon_reg_data *regs,
		enum dpu_uh_id id);
void DPU_EVENT_LOG_CMD(struct v4l2_subdev *sd, u32 cmd_id, unsigned long data, u32 size);
void DPU_EVENT_LOG_CURSOR(struct v4l2_subdev *sd, struct decon_reg_data *regs); /* cursor async */
void DPU_EVENT_LOG_UPDATE_REGION(struct v4l2_subdev *sd,
		struct decon_frame *req_region, struct decon_frame *adj_region);
void DPU_EVENT_LOG_WINUP_FLAGS(struct v4l2_subdev *sd, bool need_update,
		bool reconfigure);
void DPU_EVENT_LOG_APPLY_REGION(struct v4l2_subdev *sd,
		struct decon_rect *apl_rect);
void DPU_EVENT_LOG_MEMMAP(dpu_event_t type, struct v4l2_subdev *sd,
		dma_addr_t dma_addr, int dpp_ch, struct sg_table *sg_table);
void DPU_EVENT_SHOW(struct seq_file *s, struct decon_device *decon);
int decon_create_debugfs(struct decon_device *decon);
void decon_destroy_debugfs(struct decon_device *decon);

/* DPU fence event logger */
typedef enum dpu_f_evt_type {
	/* create retire fence and fd by driver */
	DPU_F_EVT_CREATE_RETIRE_FENCE = 0,
	/* create release fence fds and install fds to retire fence */
	DPU_F_EVT_CREATE_RELEASE_FENCE_FDS,

	/* wait for acquire fence signal */
	DPU_F_EVT_WAIT_ACQUIRE_FENCE,

	/* signal retire fence */
	DPU_F_EVT_SIGNAL_RETIRE_FENCE,

	DPU_F_EVT_MAX,
} dpu_f_evt_t;

#define DPU_FENCE_EVENT_LOG_MAX		SZ_1K
#define DPU_FENCE_EVENT_LOG_RETRY	2
#define MAX_DPU_FENCE_NAME		32

struct dpu_fence_info {
	int fd;
	char name[MAX_DPU_FENCE_NAME];
	u64 context;
	unsigned int seqno;
	unsigned long flags;
};

struct dpu_fence_log {
	ktime_t time;
	dpu_f_evt_t type;

	struct dpu_fence_info fence_info;
};

void DPU_F_EVT_LOG(dpu_f_evt_t type, struct v4l2_subdev *sd,
		struct dpu_fence_info *fence_info);

/* HDR information of panel */
enum decon_hdr_type {
	HDR_NONE = 0,
	HDR_DOLBY_VISION = 1,
	HDR_HDR10 = 2,
	HDR_HLG = 3,
};

struct decon_hdr_capabilities {
	unsigned int out_types[HDR_CAPA_NUM];
};

struct decon_hdr_capabilities_info {
	int out_num;
	int max_luminance;
	int max_average_luminance;
	int min_luminance;
};

struct decon_resources {
	int irq;
	void __iomem *regs;
	void __iomem *ss_regs;
	struct clk *aclk;
	struct clk *dpll;
	struct clk *pclk;
	struct clk *eclk;
	struct clk *eclk_leaf;
	struct clk *vclk;
	struct clk *vclk_leaf;
	struct clk *busp;
	struct clk *busd;
	struct clk *busc;
	struct clk *core;
	struct pinctrl *pinctrl;
	struct pinctrl_state *hw_te_on;
	struct pinctrl_state *hw_te_off;
};

struct decon_dt_info {
	enum decon_psr_mode psr_mode;
	enum decon_trig_mode trig_mode;
	enum decon_dsi_mode dsi_mode;
	enum decon_out_type out_type;
	int out_idx[MAX_DSIM_CNT];
	int max_win;
	int dft_win;
	int dft_ch;
	int wb_win;
	const char *pd_name;
	int dpp_cnt;
	int dsim_cnt;
	int decon_cnt;
	int chip_ver;
};

struct decon_win {
	struct decon_device *decon;
	struct fb_info *fbinfo;

	struct fb_videomode videomode;
	struct decon_dma_buf_data dma_buf_data[MAX_PLANE_CNT];
#if defined(CONFIG_FB_TEST)
	struct decon_dma_buf_data fb_buf_data;
#endif
	int plane_cnt;

	int idx;
	int dpp_id;
	u32 pseudo_palette[16];
};
/* cursor async */
struct decon_user_window {
	int x;
	int y;
};

struct dpu_afbc_info {
	dma_addr_t dma_addr[MAX_DECON_WIN];
	void *dma_v_addr[MAX_DECON_WIN];
	struct dma_buf *dma_buf[MAX_DECON_WIN];
	bool is_afbc[MAX_DECON_WIN];
	struct sg_table	*sg_table[MAX_DECON_WIN];
};

struct dpu_dma_info {
	u32 count;
	ktime_t timestamp;
	/* map */
	u32 buf_attach_cnt;
	u32 map_attach_cnt;
	u32 iovmm_map_cnt;
	/* unmap */
	u32 buf_detach_cnt;
	u32 unmap_attach_cnt;
	u32 iovmm_unmap_cnt;
};

struct decon_debug {
	void __iomem *eint_pend;
	struct dentry *debug_root;
	struct dentry *debug_event;
	struct dentry *debug_dump;
	struct dentry *debug_bts;
	struct dentry *debug_win;
	struct dentry *debug_systrace;
#if defined(CONFIG_DSIM_CMD_TEST)
	struct dentry *debug_cmd;
#endif
	struct dentry *debug_recovery_cnt;
	struct dentry *debug_cmd_lp_ref;
	struct dentry *debug_mres;
	struct dentry *debug_freq_hop;
	struct dentry *debug_fence;
	struct dentry *debug_dma_buf;

	struct dpu_log *event_log;
	u32 event_log_cnt;
	atomic_t event_log_idx;
	dpu_log_level_t event_log_level;
	struct dentry *debug_low_persistence;
	struct dpu_afbc_info prev_afbc_info;
	struct dpu_afbc_info cur_afbc_info;
	struct dma_buf *dmabuf[MAX_DECON_WIN][MAX_PLANE_CNT];
	int prev_afbc_win_id[MAX_DECON_WIN];

	struct dpu_fence_log *f_evt_log;
	u32 f_evt_log_cnt;
	atomic_t f_evt_log_idx;
	struct dpu_dma_info buf_cnt;
	struct dpu_dma_info buf_cnt_bak;
};

struct decon_update_regs {
	struct mutex lock;
	struct list_head list;
	struct list_head saved_list;
	struct task_struct *thread;
	struct kthread_worker worker;
	struct kthread_work work;
	atomic_t remaining_frame;
};

struct decon_vsync {
	wait_queue_head_t wait;
	ktime_t timestamp;
	bool active;
	int irq_refcount;
	struct mutex lock;
	struct task_struct *thread;
	u64 count;
#if defined(CONFIG_EXYNOS_COMMON_PANEL)
	u64 period;
#endif
#if defined(CONFIG_DECON_VRR_MODULATION)
	u64 active_count;
	u64 div_count;
#endif
};

#if defined(CONFIG_EXYNOS_COMMON_PANEL)
struct decon_fsync {
	wait_queue_head_t wait;
	ktime_t timestamp;
	bool active;
	int irq_refcount;
	struct mutex lock;
	struct task_struct *thread;
};
#endif

#if defined(CONFIG_EXYNOS_READ_ESD_SOLUTION)
struct decon_esd {
#define ESD_SLEEP_TIME		4
	struct mutex lock;
	struct task_struct *thread;
};
#endif

#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
struct decon_doze_hiber {
	wait_queue_head_t doze_suspend_wait, doze_wake_wait;
	ktime_t timestamp, doze_suspend_timestamp, doze_wake_timestamp;
	struct mutex lock;
	struct task_struct *doze_suspend_thread, *doze_wake_thread;
	struct kthread_worker worker;
	struct kthread_work doze_suspend_work, doze_wake_work;
	atomic_t trig_cnt;
	atomic_t block_cnt;
	bool enabled;
};
#endif

struct decon_hiber {
	struct mutex lock;
#if defined(CONFIG_EXYNOS_HIBERNATION_THREAD)
	wait_queue_head_t wait;
#endif
	ktime_t timestamp;
	struct task_struct *thread;
	struct kthread_worker worker;
	struct kthread_work work;
	struct task_struct *exit_thread;
	struct kthread_worker exit_worker;
	struct kthread_work exit_work;
	struct dentry *profile;
	atomic_t trig_cnt;
	atomic_t block_cnt;
	u32 enter_cnt;
	u32 exit_cnt;
	bool enabled;
	atomic_t remaining_hiber;

	/* entry time to hibernation */
	ktime_t hiber_entry_time;
	/* total time in hibernation */
	s64 hiber_time;

	/* start time of profiling */
	ktime_t profile_start_time;
	/* total profiling time */
	s64 profile_time;
	/* hibernation entry count during profiling */
	u32 profile_enter_cnt;
	/* hibernation exit count during profiling */
	u32 profile_exit_cnt;

	/* if true, profiling of hibernation entry ratio will be started */
	bool profile_started;

	int hiber_enter_cnt;
#if defined(CONFIG_EXYNOS_CHANGE_HIBER_CNT)
	struct dentry *hiber_cnt;
#endif
	struct dentry *hiber_exit;
	/* hibernation exit count only increasing through sysfs */
	u32 early_wakeup_cnt;
	bool early_wakeup_enable;

	int frame_cnt;
	int fps;
};

struct decon_win_update {
	bool enabled;
	u32 rect_w;
	u32 rect_h;
	u32 hori_cnt;
	u32 verti_cnt;
	/* previous update region */
	struct decon_rect prev_up_region;
	bool force_full;
};

struct decon_bts_ops {
	void (*bts_init)(struct decon_device *decon);
	void (*bts_calc_bw)(struct decon_device *decon,
			struct decon_reg_data *regs);
	void (*bts_update_bw)(struct decon_device *decon,
			struct decon_reg_data *regs, u32 is_after);
	void (*bts_acquire_bw)(struct decon_device *decon);
	void (*bts_release_bw)(struct decon_device *decon);
	void (*bts_hiber_release_bw)(struct decon_device *decon);
	void (*bts_deinit)(struct decon_device *decon);
	void (*bts_pan_display)(struct decon_device *decon, struct decon_win_config *config);
};

struct bts_layer_position {
	u32 x1;
	u32 x2; /* x2 = x1 + width */
	u32 y1;
	u32 y2; /* y2 = y1 + height */
};

struct bts_dpp_info {
	bool used;
	u32 bpp;
	u32 src_h;
	u32 src_w;
	struct bts_layer_position dst;
	u32 bw;
	bool rotation;
};

struct bts_decon_info {
	struct bts_dpp_info dpp[BTS_DPP_MAX];
	u32 vclk; /* Khz */
	u32 lcd_w;
	u32 lcd_h;
};

struct decon_bts {
	bool enabled;
	u32 resol_clk;
	u32 peak;
	u32 total_bw;
	u32 prev_total_bw;
	u32 max_disp_freq;
	u32 prev_max_disp_freq;
	u32 fps;
#if defined(CONFIG_DECON_BTS_VRR_ASYNC)
	u32 next_fps;
	u64 next_fps_vsync_count;
#endif
	u64 ppc;
	u32 line_mem_cnt;
	u32 cycle_per_line;
#if defined(CONFIG_EXYNOS_BTS)
	struct decon_bts_bw bw[BTS_DPP_MAX];

	/* each decon must know other decon's BW to get overall BW */
	u32 ch_bw[3][BTS_DPU_MAX];
	int bw_idx;
	u32 scen_idx[DPU_BS_MAX];
	struct bts_decon_info bts_info;
#endif
	struct decon_bts_ops *ops;
	struct pm_qos_request mif_qos;
	struct pm_qos_request int_qos;
	struct pm_qos_request disp_qos;
	u32 scen_updated;
};

/* cursor async */
struct decon_cursor {
	struct decon_reg_data regs;
	struct mutex unmask_lock;
	spinlock_t pos_lock;
	u32 xpos;
	u32 ypos;
	bool unmask;	/* if true, cursor unmask period */
	bool enabled;
	u32 regset_margin;
};

/* systrace */
struct decon_systrace_data {
	pid_t pid;
};

struct decon_fence {
	char name[8];
	u64 context;
	atomic_t timeline;
	spinlock_t lock;
};

struct decon_freq_hop {
	bool enabled;
	u32 target_m;	/* will be applied to DPHY */
	u32 target_k;	/* will be applied to DPHY */
	u32 request_m;	/* user requested m value */
	u32 request_k;	/* user requested k value */
};

#ifdef CONFIG_SUPPORT_DISPLAY_PROFILER
struct profile_data {
	unsigned int win_cnt;
};
#endif

struct decon_edid_data {
	int size;
	u8 edid_data[EDID_BLOCK_SIZE * MAX_EDID_BLOCK];
};

#ifdef DEBUG_DMA_BUF_LEAK
#define LEAK_INFO_ARRAY_CNT		20

struct dma_leak_info {
	u32 type;
	int import_cnt;
	int free_cnt;
	ktime_t import_time;
	ktime_t free_time;
};
#endif

struct decon_device {
	int id;
	enum decon_state state;
	struct mutex pwr_state_lock;

	unsigned long prev_used_dpp;
	unsigned long cur_using_dpp;
	unsigned long dpp_err_stat;

	unsigned long prev_req_win;
	unsigned long cur_req_win;

	struct mutex lock;
	struct mutex pm_lock;
	spinlock_t slock;
#if defined(CONFIG_EXYNOS_READ_ESD_SOLUTION)
	struct decon_esd esd;
#endif
#if defined(CONFIG_EXYNOS_COMMON_PANEL) || \
	defined(CONFIG_EXYNOS_READ_ESD_SOLUTION)
	atomic_t bypass;
	struct decon_reg_data last_regs;
#endif

	struct v4l2_subdev *out_sd[MAX_DSIM_CNT];
	struct v4l2_subdev *dsim_sd[MAX_DSIM_CNT];
	/* TODO: MAX_DPP_SUBDEV wil be changed to MAX_DPP_CNT */
	struct v4l2_subdev *dpp_sd[MAX_DPP_SUBDEV];
	struct v4l2_subdev *displayport_sd;
	struct v4l2_device v4l2_dev;
	struct v4l2_subdev sd;

	struct device *dev;
	struct decon_dt_info dt;
	struct decon_win *win[MAX_DECON_WIN];
	struct decon_resources res;
	struct decon_debug d;
	struct decon_update_regs up;
	struct decon_vsync vsync;
	struct exynos_panel_info *lcd_info;
	struct decon_win_update win_up;
	struct decon_hiber hiber;
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
	struct decon_doze_hiber doze_hiber;
#endif
	struct decon_bts bts;
	struct decon_cursor cursor;
	struct decon_fence fence;
	struct decon_freq_hop freq_hop;
	struct decon_readback readback;
	struct decon_win_rawdata win_raw;

	unsigned long wc_idx;
	int frame_cnt;
	ktime_t frame_time;
	int frame_cnt_target;
	wait_queue_head_t wait_vstatus;
	int eint_status;
#if defined(CONFIG_LOGGING_BIGDATA_BUG)
	int eint_pend;
#endif

	u32 prev_protection_bitmask;
	unsigned long prev_aclk_khz;

	atomic_t is_shutdown;
	bool up_list_saved;

#if defined(CONFIG_EXYNOS_ITMON)
	struct notifier_block itmon_nb;
	bool notified;
#endif
#if defined(CONFIG_EXYNOS_PD)
	struct exynos_pm_domain *pm_domain;
#endif
	unsigned long prev_hdr_bits;
	struct exynos_hdr_static_info prev_hdr_info;
	enum hwc_ver ver;
	/* systrace */
	struct decon_systrace_data systrace;

	bool mres_enabled;
	bool low_persistence;

#if IS_ENABLED(CONFIG_EXYNOS_FPS_CHANGE_NOTIFY)
	/* display LCD fps change notifier */
	struct atomic_notifier_head fps_change_notifier_list;
#endif
	int color_mode;

#if defined(CONFIG_EXYNOS_COMMON_PANEL)
	struct v4l2_subdev *panel_sd;
	struct panel_state *panel_state;
	struct decon_fsync fsync;
#endif

#if defined(CONFIG_DISPLAY_USE_INFO)
	struct notifier_block dpui_notif;
#endif

#ifdef CONFIG_EXYNOS_MCD_HDR
	struct lcd_hdr_info hdr_info;
#endif

#ifdef CONFIG_DYNAMIC_FREQ
	ktime_t last_update_time;
	struct df_status_info *df_status;
#endif

#ifdef CONFIG_SUPPORT_MASK_LAYER
	bool cur_mask_layer;
	u32 wait_mask_layer_trigger;
	wait_queue_head_t wait_mask_layer_trigger_queue;
#endif

#ifdef CONFIG_PROFILE_WINCONFIG
	ktime_t exit_hiber_time;
#endif
#ifdef CONFIG_SUPPORT_DISPLAY_PROFILER
	struct v4l2_subdev *profile_sd;
#endif

#ifdef DEBUG_DMA_BUF_LEAK
	int import_cnt;
	int free_cnt;
	ktime_t import_time;
	struct dma_leak_info leak_info[LEAK_INFO_ARRAY_CNT];
	int leak_cnt;
#endif

};
#ifdef CONFIG_EXYNOS_MCD_HDR

#define MAX_HDR10P_LUT 42

struct dpp_hdr10_info {
	enum exynos_video_info_type type;
	unsigned int lut[MAX_HDR10P_LUT];
	unsigned int dst_max_luminance;
};
#endif

static inline struct decon_device *get_decon_drvdata(u32 id)
{
	return decon_drvdata[id];
}

/* register access subroutines */
static inline u32 decon_read(u32 id, u32 reg_id)
{
	struct decon_device *decon = get_decon_drvdata(id);
	return readl(decon->res.regs + reg_id);
}

static inline u32 decon_read_mask(u32 id, u32 reg_id, u32 mask)
{
	u32 val = decon_read(id, reg_id);
	val &= (mask);
	return val;
}

static inline void decon_write(u32 id, u32 reg_id, u32 val)
{
	struct decon_device *decon = get_decon_drvdata(id);
	writel(val, decon->res.regs + reg_id);
}

static inline void decon_write_mask(u32 id, u32 reg_id, u32 val, u32 mask)
{
	u32 old = decon_read(id, reg_id);

	val = (val & mask) | (old & ~mask);
	decon_write(id, reg_id, val);
}

static inline u32 dsc_read(u32 dsc_id, u32 reg_id)
{
	struct decon_device *decon = get_decon_drvdata(0);
	u32 dsc_offset = dsc_id ? DSC1_OFFSET : DSC0_OFFSET;

	return readl(decon->res.regs + dsc_offset + reg_id);
}

static inline void dsc_write(u32 dsc_id, u32 reg_id, u32 val)
{
	struct decon_device *decon = get_decon_drvdata(0);
	u32 dsc_offset = dsc_id ? DSC1_OFFSET : DSC0_OFFSET;

	writel(val, decon->res.regs + dsc_offset + reg_id);
}

static inline void dsc_write_mask(u32 dsc_id, u32 reg_id, u32 val, u32 mask)
{
	u32 old = dsc_read(dsc_id, reg_id);

	val = (val & mask) | (old & ~mask);
	dsc_write(dsc_id, reg_id, val);
}

static inline u32 sysreg_read(u32 id, u32 reg_id)
{
	struct decon_device *decon = get_decon_drvdata(id);
	return readl(decon->res.ss_regs + reg_id);
}

static inline u32 sysreg_read_mask(u32 id, u32 reg_id, u32 mask)
{
	u32 val = sysreg_read(id, reg_id);
	val &= (mask);
	return val;
}

static inline void sysreg_write(u32 id, u32 reg_id, u32 val)
{
	struct decon_device *decon = get_decon_drvdata(id);
	writel(val, decon->res.ss_regs + reg_id);
}

static inline void sysreg_write_mask(u32 id, u32 reg_id, u32 val, u32 mask)
{
	u32 old = sysreg_read(id, reg_id);

	val = (val & mask) | (old & ~mask);
	sysreg_write(id, reg_id, val);
}

/* common function API */
bool decon_validate_x_alignment(struct decon_device *decon, int x, u32 w,
		u32 bits_per_pixel);
int decon_wait_for_vsync(struct decon_device *decon, u32 timeout);
#if defined(CONFIG_DECON_VRR_MODULATION)
int decon_wait_for_active_region(struct decon_device *decon, u32 timeout);
#endif
int decon_check_limitation(struct decon_device *decon, int idx,
		struct decon_win_config *config);
void decon_readback_wq(struct work_struct *work);

/* DECON to DSI interface functions */
int decon_register_irq(struct decon_device *decon);
int decon_get_clocks(struct decon_device *decon);
void decon_set_clocks(struct decon_device *decon);
int decon_get_out_sd(struct decon_device *decon);
int decon_get_pinctrl(struct decon_device *decon);
int decon_register_ext_irq(struct decon_device *decon);
int decon_create_vsync_thread(struct decon_device *decon);
void decon_destroy_vsync_thread(struct decon_device *decon);
#if defined(CONFIG_EXYNOS_COMMON_PANEL)
int decon_create_fsync_thread(struct decon_device *decon);
void decon_destroy_fsync_thread(struct decon_device *decon);
int decon_create_last_info(struct decon_device *decon);
void decon_destroy_last_info(struct decon_device *decon);
#endif
int decon_create_esd_thread(struct decon_device *decon);
void decon_destroy_esd_thread(struct decon_device *decon);
int decon_create_psr_info(struct decon_device *decon);
void decon_destroy_psr_info(struct decon_device *decon);
void decon_get_edid(struct decon_device *decon, struct decon_edid_data *edid_data);

/* DECON to writeback interface functions */
int decon_wb_register_irq(struct decon_device *decon);
void decon_wb_free_irq(struct decon_device *decon);
int decon_wb_get_clocks(struct decon_device *decon);
void decon_wb_set_clocks(struct decon_device *decon);
int decon_wb_get_out_sd(struct decon_device *decon);

#if defined(CONFIG_EXYNOS_DISPLAYPORT)
/* DECON to DISPLAYPORT interface functions */
void decon_displayport_set_cur_sst_id(u32 decon_id);
int decon_displayport_register_irq(struct decon_device *decon);
void decon_displayport_free_irq(struct decon_device *decon);
int decon_displayport_create_vsync_thread(struct decon_device *decon);
int decon_displayport_get_clocks(struct decon_device *decon);
int decon_displayport_get_out_sd(struct decon_device *decon);
int decon_displayport_get_hdr_capa(struct decon_device *decon,
		struct decon_hdr_capabilities *hdr_capa);
int decon_displayport_get_hdr_capa_info(struct decon_device *decon,
		struct decon_hdr_capabilities_info *hdr_capa_info);
int decon_displayport_get_config(struct decon_device *dex,
		struct exynos_displayport_data *displayport_data);
int decon_displayport_set_config(struct decon_device *dex,
		struct exynos_displayport_data *displayport_data);
int decon_displayport_get_edid(struct decon_device *decon,
		struct decon_edid_data *edid);
void decon_displayport_under_flow_int_mask(u32 decon_id);
#endif

/* window update related function */
#define DPU_FULL_RECT(r, lcd)			\
	do {					\
		(r)->left = 0;			\
		(r)->top = 0;			\
		(r)->right = (lcd)->xres - 1;	\
		(r)->bottom = (lcd)->yres - 1;	\
	} while (0)
void dpu_init_win_update(struct decon_device *decon);
void dpu_prepare_win_update_config(struct decon_device *decon,
		struct decon_win_config_data *win_data,
		struct decon_reg_data *regs);
void dpu_set_win_update_config(struct decon_device *decon,
		struct decon_reg_data *regs);
void dpu_set_win_update_partial_size(struct decon_device *decon,
		struct decon_rect *up_region);

/* low persistence mode */
void decon_init_low_persistence_mode(struct decon_device *decon);

/* multi-resolution related function */
void dpu_update_mres_lcd_info(struct decon_device *decon,
		struct decon_reg_data *regs);
void dpu_set_mres_config(struct decon_device *decon, struct decon_reg_data *regs);
/* variable refresh rate related functions */
void dpu_update_vrr_lcd_info(struct decon_device *decon,
		struct vrr_config_data *vrr_config);
void dpu_set_vrr_config(struct decon_device *decon,
		struct vrr_config_data *vrr_config);

/* DPHY PLL frequency hopping feature related functions */
void dpu_init_freq_hop(struct decon_device *decon);
void dpu_update_freq_hop(struct decon_device *decon);
void dpu_set_freq_hop(struct decon_device *decon, struct decon_reg_data *regs, bool en);

/* internal only function API */
int decon_check_var(struct fb_var_screeninfo *var, struct fb_info *info);
int decon_set_par(struct fb_info *info);
int decon_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);
int decon_setcolreg(unsigned regno,
			    unsigned red, unsigned green, unsigned blue,
			    unsigned transp, struct fb_info *info);
int decon_mmap(struct fb_info *info, struct vm_area_struct *vma);

u32 wincon(int idx);

static inline u32 win_start_pos(int x, int y)
{
	return (WIN_STRPTR_Y_F(y) | WIN_STRPTR_X_F(x));
}

static inline u32 win_end_pos(int x, int y,  u32 xres, u32 yres)
{
	return (WIN_ENDPTR_Y_F(y + yres - 1) | WIN_ENDPTR_X_F(x + xres - 1));
}

static inline char *get_decon_state_name(int state)
{
	static char *decon_state_names[] = {
		"INIT",
		"ON",
		"DOZE",
		"HIBER",
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
		"DOZE_WAKE",
#endif
		"DOZE_SUSPEND",
		"OFF",
		"TUI",
	};

	if (state < 0 || state >= ARRAY_SIZE(decon_state_names))
		return NULL;

	return decon_state_names[state];
}

/* HIBER releated */
int decon_exit_hiber(struct decon_device *decon);
int decon_enter_hiber(struct decon_device *decon);
int decon_lcd_off(struct decon_device *decon);
int decon_register_hiber_work(struct decon_device *decon);
int decon_hiber_block_exit(struct decon_device *decon);

static inline void decon_hiber_block(struct decon_device *decon)
{
	if (decon)
		atomic_inc(&decon->hiber.block_cnt);
}

static inline bool decon_is_hiber_blocked(struct decon_device *decon)
{
	return (atomic_read(&decon->hiber.block_cnt) > 0);
}

static inline int decon_get_hiber_block_cnt(struct decon_device *decon)
{
	return (atomic_read(&decon->hiber.block_cnt));
}

static inline void decon_hiber_unblock(struct decon_device *decon)
{
	if (decon) {
		if (decon_is_hiber_blocked(decon))
			atomic_dec(&decon->hiber.block_cnt);
	}
}

static inline void decon_hiber_block_reset(struct decon_device *decon)
{
	if (decon)
		atomic_set(&decon->hiber.block_cnt, 0);
}

static inline void decon_hiber_trig_reset(struct decon_device *decon)
{
	if (decon)
		atomic_set(&decon->hiber.trig_cnt, 0);
}

static inline bool decon_min_lock_cond(struct decon_device *decon)
{
	return (atomic_read(&decon->hiber.block_cnt) <= 0);
}

static inline bool decon_hiber_enter_cond(struct decon_device *decon)
{
	return ((atomic_read(&decon->hiber.block_cnt) <= 0)
#if defined(CONFIG_EXYNOS_DISPLAYPORT)
		&& is_displayport_not_running()
#endif
		&& (!decon->low_persistence)
		&& (atomic_inc_return(&decon->hiber.trig_cnt) >=
			decon->hiber.hiber_enter_cnt));
}

#define WAKE_TIMEOUT_MSEC	(100)
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
int decon_register_doze_hiber_work(struct decon_device *decon);
int decon_doze_wake(struct decon_device *decon);
int decon_doze_suspend(struct decon_device *decon);

static inline bool decon_doze_suspend_enter_cond(struct decon_device *decon)
{
	return (decon->state == DECON_STATE_DOZE_WAKE) &&
		(atomic_read(&decon->doze_hiber.block_cnt) <= 0);
}

static inline bool decon_is_wake(struct decon_device *decon)
{
	if (!decon)
		return false;

	return decon->state == DECON_STATE_INIT ||
		decon->state == DECON_STATE_ON ||
		decon->state == DECON_STATE_DOZE ||
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
		decon->state == DECON_STATE_DOZE_WAKE ||
#endif
		decon->state == DECON_STATE_TUI;
}

static inline int decon_doze_wake_lock(struct decon_device *decon,
		unsigned long timeout)
{
	s64 elapsed_usec;
	struct timespec cur_ts, last_ts, delta_ts;
	unsigned long timeout_jiffies;
	bool wakeup = false;

	if (!decon)
		return -EINVAL;

	timeout_jiffies = jiffies + msecs_to_jiffies(timeout);
	ktime_get_ts(&last_ts);
	decon_dbg("%s (%s) cnt:%d +\n",
		__func__, get_decon_state_name(decon->state),
			atomic_read(&decon->doze_hiber.block_cnt));

	atomic_inc(&decon->doze_hiber.block_cnt);
	mutex_lock(&decon->doze_hiber.lock);
	wakeup = (decon->state == DECON_STATE_DOZE_SUSPEND) ? true : false;
	mutex_unlock(&decon->doze_hiber.lock);

	if (wakeup) {
		decon->doze_hiber.doze_wake_timestamp = ktime_get();
		wake_up_interruptible_all(&decon->doze_hiber.doze_wake_wait);
		while (time_is_after_jiffies(timeout_jiffies)
				&& decon->state == DECON_STATE_DOZE_SUSPEND)
			usleep_range(1000, 1100);

		if (time_is_before_jiffies(timeout_jiffies)) {
			decon_err("%s timeout(elapsed %lu msec)\n",
					__func__, timeout);
		}
	}

	decon_dbg("%s (%s) cnt:%d -\n",
			__func__, get_decon_state_name(decon->state),
			atomic_read(&decon->doze_hiber.block_cnt));

	ktime_get_ts(&cur_ts);
	delta_ts = timespec_sub(cur_ts, last_ts);
	elapsed_usec = timespec_to_ns(&delta_ts) / 1000;
	if (elapsed_usec > 20000) {
		decon_warn("%s (elapsed %2lld.%03lld msec)\n",
				__func__, elapsed_usec / 1000, elapsed_usec % 1000);
	}

	return 0;
}

static inline void decon_doze_wake_unlock(struct decon_device *decon)
{
	if (!decon)
		return;

	decon_dbg("%s (%s) cnt:%d +\n",
			__func__, get_decon_state_name(decon->state),
			atomic_read(&decon->doze_hiber.block_cnt));

	if (!atomic_dec_return(&decon->doze_hiber.block_cnt)) {
		decon->doze_hiber.doze_suspend_timestamp = ktime_get();
		wake_up_interruptible_all(&decon->doze_hiber.doze_suspend_wait);
	}

	decon_dbg("%s (%s) cnt:%d -\n",
			__func__, get_decon_state_name(decon->state),
			atomic_read(&decon->doze_hiber.block_cnt));
}

static inline int decon_wake_lock(struct decon_device *decon,
		unsigned long timeout)
{
	int ret;

	decon_hiber_block_exit(decon);
	ret = decon_doze_wake_lock(decon, timeout);

	return ret;
}

static inline void decon_wake_unlock(struct decon_device *decon)
{
	decon_doze_wake_unlock(decon);
	decon_hiber_unblock(decon);
}

static inline int decon_wake_lock_global(u32 id, unsigned long timeout)
{
	struct decon_device *decon = get_decon_drvdata(id);

	return decon_wake_lock(decon, timeout);
}

static inline void decon_wake_unlock_global(u32 id)
{
	struct decon_device *decon = get_decon_drvdata(id);

	decon_wake_unlock(decon);
}
#else
static inline int decon_wake_lock_global(u32 id, unsigned long timeout)
{
	return 0;
}
static inline void decon_wake_unlock_global(u32 id)
{
	return;
}
#endif

static inline void decon_enter_shutdown(struct decon_device *decon)
{
	atomic_inc(&decon->is_shutdown);
}

static inline bool decon_is_enter_shutdown(struct decon_device *decon)
{
	return atomic_read(&decon->is_shutdown);
}

static inline void decon_enter_shutdown_reset(struct decon_device *decon)
{
	atomic_set(&decon->is_shutdown, 0);
}

#if defined(CONFIG_EXYNOS_COMMON_PANEL) || \
	defined(CONFIG_EXYNOS_READ_ESD_SOLUTION)
static inline void decon_set_bypass(struct decon_device *decon, bool on)
{
	atomic_set(&decon->bypass, !!on);
}

static inline void decon_bypass_on(struct decon_device *decon)
{
	atomic_inc(&decon->bypass);
}

static inline void decon_bypass_off(struct decon_device *decon)
{
	atomic_dec(&decon->bypass);
}

static inline bool decon_is_bypass(struct decon_device *decon)
{
	return atomic_read(&decon->bypass);
}
#endif

#if defined(CONFIG_EXYNOS_COMMON_PANEL)
/* TODO : remove decon global function */
static inline void decon_set_bypass_global(u32 id, bool on)
{
	struct decon_device *decon = get_decon_drvdata(id);

	if (!decon)
		return;

	decon_set_bypass(decon, on);
}

static inline void decon_bypass_on_global(u32 id)
{
	struct decon_device *decon = get_decon_drvdata(id);

	if (!decon)
		return;

	decon_bypass_on(decon);
}

static inline void decon_bypass_off_global(u32 id)
{
	struct decon_device *decon = get_decon_drvdata(id);

	if (!decon)
		return;

	decon_bypass_off(decon);
}

static inline int decon_get_bypass_cnt_global(u32 id)
{
	struct decon_device *decon = get_decon_drvdata(id);

	if (!decon)
		return -EINVAL;

	return atomic_read(&decon->bypass);
}

int decon_reset_panel(struct decon_device *decon);

static inline int decon_reset_panel_global(u32 id)
{
	struct decon_device *decon = get_decon_drvdata(id);

	return decon_reset_panel(decon);
}
#endif

int decon_update_pwr_state(struct decon_device *decon, u32 mode);

#ifdef CONFIG_LOGGING_BIGDATA_BUG
void log_decon_bigdata(struct decon_device *decon);
#endif

enum disp_pwr_mode {
	DISP_PWR_OFF = 0,
	DISP_PWR_DOZE,
	DISP_PWR_NORMAL,
	DISP_PWR_DOZE_SUSPEND,
	DISP_PWR_MAX,
};

int decon_update_pwr_state(struct decon_device *decon, enum disp_pwr_mode mode);

typedef int (*set_pwr_state_t)(struct decon_device *);

struct disp_pwr_state {
	enum decon_state state;
	set_pwr_state_t set_pwr_state;
};

static inline bool IS_DECON_ON_STATE(struct decon_device *decon)
{
	return decon->state == DECON_STATE_INIT ||
		decon->state == DECON_STATE_ON ||
		decon->state == DECON_STATE_DOZE ||
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
		decon->state == DECON_STATE_DOZE_WAKE ||
#endif
		decon->state == DECON_STATE_TUI;
}

static inline bool IS_DECON_OFF_STATE(struct decon_device *decon)
{
	return decon->state == DECON_STATE_HIBER ||
		decon->state == DECON_STATE_DOZE_SUSPEND ||
		decon->state == DECON_STATE_OFF;
}

#ifdef CONFIG_EXYNOS_DOZE
static inline bool IS_DECON_DOZE_STATE(struct decon_device *decon)
{
	return decon->state == DECON_STATE_DOZE ||
#if defined(CONFIG_EXYNOS_DOZE_HIBERNATION)
		decon->state == DECON_STATE_DOZE_WAKE ||
#endif
		decon->state == DECON_STATE_DOZE_SUSPEND;
}
#else
static inline bool IS_DECON_DOZE_STATE(struct decon_device *decon)
{
	return false;
}
#endif

static inline bool IS_DECON_HIBER_STATE(struct decon_device *decon)
{
	return decon->state == DECON_STATE_HIBER;
}

/* tui feature support external to security driver(gud) */
int decon_tui_protection(bool tui_en);

/* helper functions */
int dpu_get_sd_by_drvname(struct decon_device *decon, char *drvname);
void dpu_unify_rect(struct decon_rect *r1, struct decon_rect *r2,
		struct decon_rect *dst);
void dpu_save_fence_info(int fd, struct dma_fence *fence,
		struct dpu_fence_info *fence_info);
void dpu_show_readback_buf_info(struct decon_device *decon, u32 diff_cnt);
void dpu_show_dma_attach_info(char *fn, struct decon_device *decon, u32 sel);

void decon_dump(struct decon_device *decon, bool panel_dump);
void decon_to_psr_info(struct decon_device *decon, struct decon_mode_info *psr);
void decon_to_init_param(struct decon_device *decon, struct decon_param *p);
void decon_create_timeline(struct decon_device *decon, char *name);
void decon_create_release_fences(struct decon_device *decon,
		struct decon_win_config_data *win_data,
		struct sync_file *sync_file);
int decon_create_fence(struct decon_device *decon, struct sync_file **sync_file);
int decon_wait_fence(struct decon_device *decon, struct dma_fence *fence, int fd);
void decon_signal_fence(struct decon_device *decon, struct dma_fence *fence);

bool decon_intersect(struct decon_rect *r1, struct decon_rect *r2);
int decon_intersection(struct decon_rect *r1,
		struct decon_rect *r2, struct decon_rect *r3);

bool is_decon_rect_differ(struct decon_rect *r1, struct decon_rect *r2);
bool is_scaling(struct decon_win_config *config);
bool is_full(struct decon_rect *r, struct exynos_panel_info *lcd);
void __iomem *dpu_get_sysreg_addr(void);
void dpu_dump_afbc_info(void);
#if defined(CONFIG_EXYNOS_CONTENT_PATH_PROTECTION)
void decon_set_protected_content(struct decon_device *decon,
		struct decon_reg_data *regs);
#endif
bool is_decon_rect_empty(struct decon_rect *r);

int decon_runtime_suspend(struct device *dev);
int decon_runtime_resume(struct device *dev);
void decon_dpp_stop(struct decon_device *decon, bool do_reset);

/* cursor async mode functions */
void decon_set_cursor_reset(struct decon_device *decon,
		struct decon_reg_data *regs);
void decon_set_cursor_unmask(struct decon_device *decon, bool unmask);
void dpu_cursor_win_update_config(struct decon_device *decon,
		struct decon_reg_data *regs);
int decon_set_cursor_win_config(struct decon_device *decon, int x, int y);
void dpu_init_cursor_mode(struct decon_device *decon);

int dpu_sysmmu_fault_handler(struct iommu_domain *domain,
	struct device *dev, unsigned long iova, int flags, void *token);
#if defined(CONFIG_EXYNOS_PD)
int dpu_pm_domain_check_status(struct exynos_pm_domain *pm_domain);
#endif
int decon_set_out_sd_state(struct decon_device *decon, enum decon_state state);
int decon_update_last_regs(struct decon_device *decon,
		struct decon_reg_data *regs);

/* display LCD on/off notifier */
static ATOMIC_NOTIFIER_HEAD(lcd_status_notifier_list);

int register_lcd_status_notifier(struct notifier_block *nb);
int unregister_lcd_status_notifier(struct notifier_block *nb);
void lcd_status_notifier(u32 lcd_status);

#if IS_ENABLED(CONFIG_EXYNOS_FPS_CHANGE_NOTIFY)
int register_fps_change_notifier(struct notifier_block *nb);
int unregister_fps_change_notifier(struct notifier_block *nb);
void notify_fps_change(u32 fps);
#endif

void decon_hiber_start(struct decon_device *decon);
void decon_hiber_finish(struct decon_device *decon);
int dpu_hw_recovery_process(struct decon_device *decon);

void dpu_pll_sleep_mask(struct decon_device *decon);
void dpu_pll_sleep_unmask(struct decon_device *decon);

u32 decon_processed_linecnt(struct decon_device *decon);
int _decon_disable(struct decon_device *decon, enum decon_state state);
int _decon_enable(struct decon_device *decon, enum decon_state state);

/* IOCTL commands */
#define S3CFB_SET_VSYNC_INT		_IOW('F', 206, __u32)
#define S3CFB_DECON_SELF_REFRESH	_IOW('F', 207, __u32)
#define S3CFB_WIN_CONFIG_OLD		_IOW('F', 209, \
						struct decon_win_config_data_old)
#define S3CFB_WIN_CONFIG		_IOW('F', 209, \
						struct decon_win_config_data)

/* cursor async */
#define DECON_WIN_CURSOR_POS		_IOW('F', 222, struct decon_user_window)
#define S3CFB_POWER_MODE		_IOW('F', 223, __u32)
#define EXYNOS_DISP_INFO		_IOW('F', 260, \
						struct decon_disp_info)
#define EXYNOS_DISP_RESTRICTIONS	_IOW('F', 261, \
						struct dpp_restrictions_info)
#define EXYNOS_GET_DPP_CNT		_IOW('F', 262, __u32)
#define EXYNOS_GET_DPP_RESTRICTION	_IOW('F', 263, \
						struct dpp_ch_restriction)

#define S3CFB_START_CRC			_IOW('F', 270, u32)
#define S3CFB_SEL_CRC_BITS		_IOW('F', 271, u32)
#define S3CFB_GET_CRC_DATA		_IOR('F', 272, u32)

#define EXYNOS_GET_DISPLAYPORT_CONFIG		_IOW('F', 300, \
						struct exynos_displayport_data)
#define EXYNOS_SET_DISPLAYPORT_CONFIG		_IOW('F', 301, \
						struct exynos_displayport_data)

#define EXYNOS_DPU_DUMP		_IOW('F', 302, \
						struct decon_win_config_data)

/* HDR support */
#define S3CFB_GET_HDR_CAPABILITIES _IOW('F', 400, struct decon_hdr_capabilities)
#define S3CFB_GET_HDR_CAPABILITIES_NUM _IOW('F', 401, struct decon_hdr_capabilities_info)

/* DPU aclk */
#define EXYNOS_DPU_GET_ACLK		_IOR('F', 500, u32)

/* COLOR Mode */
#define EXYNOS_GET_COLOR_MODE_NUM	_IOW('F', 600, __u32)
#define EXYNOS_GET_COLOR_MODE		_IOW('F', 601, struct decon_color_mode_info)
#define EXYNOS_SET_COLOR_MODE		_IOW('F', 602, __u32)

#if defined(CONFIG_EXYNOS_DECON_DQE)
#define EXYNOS_GET_RENDER_INTENTS_NUM	_IOW('F', 610, struct decon_render_intents_num_info)
#define EXYNOS_GET_RENDER_INTENT	_IOW('F', 611, struct decon_render_intent_info)

#define EXYNOS_SET_COLOR_TRANSFORM	_IOW('F', 612, struct decon_color_transform_info)
#define EXYNOS_SET_COLOR_MODE_WITH_RENDER_INTENT	_IOW('F', 613, struct decon_color_mode_with_render_intent_info)
#endif

/* Readback */
#define EXYNOS_GET_READBACK_ATTRIBUTE	_IOW('F', 614, struct decon_readback_attribute)

#ifdef CONFIG_EXYNOS_SET_ACTIVE
/* Display Mode Support */
#define EXYNOS_GET_DISPLAY_MODE_NUM	_IOW('F', 700, u32)
#define EXYNOS_GET_DISPLAY_MODE_OLD		_IOW('F', 701, struct exynos_display_mode_old)
#define EXYNOS_GET_DISPLAY_MODE		_IOW('F', 701, struct exynos_display_mode)
#define EXYNOS_SET_DISPLAY_MODE		_IOW('F', 702, struct exynos_display_mode)
#define EXYNOS_GET_DISPLAY_CURRENT_MODE _IOW('F', 705, u32)
#endif
/* EDID data */
#define EXYNOS_GET_EDID		_IOW('F', 800, struct decon_edid_data)
#if defined(CONFIG_EXYNOS_COMMON_PANEL)
#define V4L2_EVENT_DECON                (V4L2_EVENT_PRIVATE_START + 1000)
#define V4L2_EVENT_DECON_FRAME_START    (V4L2_EVENT_DECON + 1)
#define V4L2_EVENT_DECON_FRAME_DONE     (V4L2_EVENT_DECON + 2)
#define V4L2_EVENT_DECON_VSYNC          (V4L2_EVENT_DECON + 3)
#endif
#endif /* ___SAMSUNG_DECON_H__ */
