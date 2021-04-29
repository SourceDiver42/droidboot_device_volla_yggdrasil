#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"


#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <mach/mt_pm_ldo.h>
#include <mach/mt_gpio.h>
#endif
/*prize-add-for round corner-houjian-20180918-start*/
#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_rgba4444_roundedpattern.h"
#endif
/*prize-add-for round corner-houjian-20180918-end*/


#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_notice("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

#define I2C_I2C_LCD_BIAS_CHANNEL 0
static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)			(lcm_util.set_reset_pin((v)))
#define MDELAY(n)					(lcm_util.mdelay(n))
#define UDELAY(n)		(lcm_util.udelay(n))

/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */
#define dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) \
	lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
	lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
	lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

static const unsigned char LCD_MODULE_ID = 0x01;
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE	0
#define FRAME_WIDTH  										1080
#define FRAME_HEIGHT 										2310

//prize-penggy modify LCD size-20190328-start
#define LCM_PHYSICAL_WIDTH                  				(67070)
#define LCM_PHYSICAL_HEIGHT                  				(145310)
//prize-penggy modify LCD size-20190328-end
#define GPIO_65132_ENP  GPIO_LCD_BIAS_ENP
#define GPIO_65132_ENN  GPIO_LCD_BIAS_ENN
#define GPIO_LCM_RESET  GPIO_LCD_RST

#define REGFLAG_DELAY             							 0xFFFA
#define REGFLAG_UDELAY             							 0xFFFB
#define REGFLAG_PORT_SWAP									 0xFFFC
#define REGFLAG_END_OF_TABLE      							 0xFFFD   // END OF REGISTERS MARKER

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

struct LCM_setting_table {
	unsigned int cmd;
	unsigned char count;
	unsigned char para_list[160];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
	
	{0x28, 0, {} },
	{REGFLAG_DELAY, 20, {} },
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static struct LCM_setting_table init_setting_vdo[] = {
	{0xb0,0x01,{0x00}},
	{0xb6,0x05,{0x30,0x6b,0x00,0x82,0x03}},
	{0xb7,0x04,{0x51,0x00,0x00,0x00}},
	{0xb8,0x07,{0x57,0x3d,0x19,0xbe,0x1e,0x0a,0x0a}},
	{0xb9,0x07,{0x6f,0x3d,0x28,0xbe,0x3c,0x14,0x0a}},
	{0xba,0x07,{0xb5,0x33,0x41,0xbe,0x64,0x23,0x0a}},
	{0xbb,0x0b,{0x44,0x26,0xc3,0x1f,0x19,0x06,0x03,0xc0,0x00,0x00,0x10}},
	{0xbc,0x0b,{0x32,0x4c,0xc3,0x52,0x32,0x1f,0x03,0xf2,0x00,0x00,0x13}},
	{0xbd,0x0b,{0x24,0x68,0xc3,0xaa,0x3f,0x32,0x03,0xff,0x00,0x00,0x25}},
	{0xbe,0x0c,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0xc0,0x0e,{0x00,0xc4,0x01,0x2c,0x07,0x09,0x06,0x00,0x0a,0x00,0x00,0x08,0x00,0x00}},
	{0xc1,0x29,{0x30,0x00,0x00,0x81,0x00,0x00,0x00,0x00,0x22,0x00,0x05,0x20,0x00,0x80,0xfa,0x40,0x00,0x84,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0xC2,0x81,{0x05,0xF0,0x5F,0x01,0x03,0x10,0x04,0x02,0x00,0x01,0x20,0xC4,0x01,0x02,0x09,0x0F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x10,0xCC,0x04,0x04,0x01,0x03,0xC1,0x00,0x40,0x04,0x00,0x00,0x01,0x08,0x00,0x00,0x00,0x00,0x11,0x00,0x40,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0x00,0x00,0x00,0x03}},	//for VN1
	{0xc3,0x6c,{0x01,0x20,0x12,0x01,0x00,0x20,0x03,0x04,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x5a,0xaa,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0xc4,0x61,{0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x06,0x41,0x40,0xc4,0x00,0x00,0x0f,0x11,0x4f,0x00,0x4f,0x5d,0x5d,0x5f,0x5f,0x61,0x61,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x06,0x41,0x40,0xc4,0x00,0x00,0x0e,0x10,0x4f,0x00,0x4f,0x5d,0x5d,0x5f,0x5f,0x61,0x61,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0xf8,0xfd,0xc0,0xf8,0xfd,0x00,0x07,0x02,0x00,0x07,0x02,0x00,0x00,0xfc,0x00,0x00,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x00,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},//modify 0906 for V4 VN1
	{0xc5,0x05,{0x08,0x00,0x00,0x00,0x00}},
	{0xC6,0x3e,{0x00,0x0A,0x0A,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x13,0x01,0xFF,0x0C,0x1C,0x01,0x3B,0x3B,0x3C,0x00,0x00,0x00,0x01,0x05,0x09,0x28,0x28,0x01,0x1C,0x3B,0x3B,0x3C,0x00,0x00,0x00,0x01,0x0B,0x00,0x00,0x00,0x1E,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x00,0x00,0x00,0x00,0x22,0x22,0x22,0x00,0x00,0x00,0x00,0x10,0x00}},
	{0xcb,0x0e,{0xa0,0x80,0x70,0x00,0x20,0x00,0x00,0x2d,0x41,0x00,0x00,0x00,0x00,0xff}},
	{0xce,0x21,{0x5d,0x40,0x49,0x53,0x59,0x5e,0x63,0x68,0x6e,0x74,0x7e,0x8a,0x98,0xa8,0xbb,0xd0,0xe7,0xff,0x04,0x00,0x04,0x04,0x42,0x00,0x69,0x5a,0x40,0x40,0x00,0x00,0x04,0xfa,0x00}},
	{0xcf,0x06,{0x00,0x00,0x80,0xa1,0x6a,0x00}},
	{0xd0,0x12,{0xc7,0x17,0x8a,0x66,0x09,0x90,0x00,0xcc,0x0f,0x05,0xc7,0x14,0x12,0xfe,0x09,0x09,0xcc,0x00}},
	{0xd1,0x1e,{0xdb,0xdb,0x1b,0xe3,0x07,0x07,0x3b,0x11,0xf1,0x11,0xf1,0x05,0x33,0x73,0x07,0x33,0x33,0x70,0xd3,0xd0,0x06,0x96,0x13,0x93,0x22,0x22,0x22,0xb3,0xbb,0x00}},
	{0xd2,0x11,{0x00,0x00,0x00,0x02,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0xd3,0x99,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff,0xff,0xf7,0xff}},
	{0xd4,0x17,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0xd7,0x4a,{0x21,0x10,0x52,0x52,0x00,0xc4,0x00,0x0a,0x00,0xb6,0x04,0xfd,0x01,0x00,0x03,0x00,0x05,0x05,0x05,0x07,0x04,0x05,0x06,0x07,0x08,0x02,0x02,0x08,0x03,0x03,0x08,0x04,0x08,0x08,0x0c,0x0b,0x0a,0x0a,0x0a,0x07,0x08,0x0a,0x06,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x02,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x06,0x06,0x00,0x00,0x00,0x00}},
	{0xd8,0x3e,{0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0xdd,0x04,{0x30,0x06,0x23,0x65}},
	{0xde,0x0a,{0x00,0x00,0x00,0x0f,0xff,0x00,0x00,0x00,0x00,0x10}},
	{0xe8,0x04,{0x00,0x30,0x63,0x00}},
	{0xea,0x1d,{0x01,0x0e,0x01,0xc0,0x0b,0x00,0x00,0x00,0x09,0x00,0x03,0xb6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xc2,0x00,0x11,0x00,0xc4,0x0c,0x40,0x86}},
	{0xeb,0x07,{0x00,0x00,0x00,0x00,0x01,0x00,0x11}},
	{0xec,0x0a,{0x04,0xb0,0x00,0x10,0x3c,0x0b,0x00,0x00,0x02,0x3a}},
	{0xed,0x20,{0x01,0x01,0x02,0x02,0x08,0x08,0x09,0x09,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x12,0x03,0xae,0x00,0x00,0x00,0x50,0x00,0x03,0xae,0x00,0x00,0xa0,0x10,0x00}},
	{0xee,0x60,{0x01,0x3f,0xf0,0x03,0x00,0xf0,0x03,0x00,0x00,0x00,0x00,0xc2,0x0f,0xff,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x00,0x10,0x02,0x10,0x00,0x08,0x00,0x09,0x03,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0xEF,0x66,{0x01,0xE0,0x59,0x09,0x40,0x00,0x00,0x00,0x00,0x2D,0x2D,0x2D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x59,0x09,0x40,0x00,0x00,0x00,0x00,0x2D,0x2D,0x2D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x03,0x10,0x02,0x02,0x10,0x07,0x19,0x0D,0x0A,0x06,0x00,0x00,0x00,0x00,0x00,0x43,0x00,0x03,0x51,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x02}},
	{0xf9,0x05,{0x44,0x3f,0x00,0x8d,0xbf}},
	//default gamma setting, waiting for Truly modules.
	{0xC7,0x4c,{0x00,0x4E,0x00,0xD6,0x01,0x30,0x01,0x55,0x01,0x6F,0x01,0x79,0x01,0x84,0x01,0x84,0x01,0xA0,0x01,0x71,0x01,0xC5,0x01,0x84,0x01,0xC8,0x01,0x80,0x01,0xE7,0x01,0xD7,0x02,0x43,0x02,0x80,0x02,0x89,0x00,0x4E,0x00,0xD6,0x01,0x30,0x01,0x55,0x01,0x6F,0x01,0x79,0x01,0x84,0x01,0x84,0x01,0xA0,0x01,0x71,0x01,0xC5,0x01,0x84,0x01,0xC8,0x01,0x80,0x01,0xE7,0x01,0xD7,0x02,0x43,0x02,0x80,0x02,0x89}},
	{0xB0,0x01,{0x03}},
	{0x51,0x01,{0xFF}},                                 //// Write_Display_Brightness
	{0x53,0x01,{0x2C}},                                 //// Write_CTRL_Display
	{0x55,0x01,{0x00}},                                 //// Write_CABC
	//LCD driver IC initial code end
	{0x35,0x01,{0x00}}, //TE Open
	{ 0x11, 0x01, {0x00} },
	{REGFLAG_DELAY, 120, {} },
	{ 0x29, 0x01, {0x00} },
	{REGFLAG_DELAY, 10, {} },

	{ REGFLAG_END_OF_TABLE, 0x00, {} }
};


static void push_table(void *cmdq, struct LCM_setting_table *table,
	unsigned int count, unsigned char force_update)
{
	unsigned int i;
	unsigned int cmd;

	for (i = 0; i < count; i++) {
		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
			if (table[i].count <= 10)
				MDELAY(table[i].count);
			else
				MDELAY(table[i].count);
			break;

		case REGFLAG_UDELAY:
			UDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V22(cmdq, cmd, table[i].count,
				table[i].para_list, force_update);
		}
	}
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
	params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////
#endif
	
	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
	
	
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
#if (LCM_DSI_CMD_MODE)
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
	params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
#else
	params->dsi.intermediat_buffer_num = 0; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
#endif

	// Video mode setting
	params->dsi.packet_size=256;
	
	params->dsi.vertical_sync_active				=  5;
	params->dsi.vertical_backporch					= 50;//16 25 30 35 12 8
	params->dsi.vertical_frontporch					= 8;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active = 10;
	params->dsi.horizontal_backporch = 10;//32
	params->dsi.horizontal_frontporch = 17;//78
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;
	/* params->dsi.ssc_disable                                                       = 1; */

#if (LCM_DSI_CMD_MODE)
	params->dsi.PLL_CLOCK = 580;
#else
	params->dsi.PLL_CLOCK = 480;//580;//500;
#endif
	params->dsi.ssc_disable = 1;
	params->dsi.ssc_range = 1;
	params->dsi.cont_clock = 0;
	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.edp_panel =1;

	params->physical_width = 69;
	params->physical_height = 147;
	//params->density = 420;
	
#if 0
/*prize penggy add-for LCD ESD-20190219-end*/	
	params->dsi.ssc_disable = 1;
	params->dsi.lcm_ext_te_monitor = FALSE;
	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
		params->dsi.lcm_esd_check_table[0].cmd					= 0x0a;
		params->dsi.lcm_esd_check_table[0].count			= 1;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9d;
		params->dsi.lcm_esd_check_table[1].cmd					= 0x09;
		params->dsi.lcm_esd_check_table[1].count			= 4;
		params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;
		params->dsi.lcm_esd_check_table[1].para_list[1] = 0x73;
		params->dsi.lcm_esd_check_table[1].para_list[2] = 0x04;
		params->dsi.lcm_esd_check_table[1].para_list[3] = 0x00;
#endif
	/*prize-add-for round corner-houjian-20180918-start*/
	#ifdef MTK_ROUND_CORNER_SUPPORT
		params->round_corner_params.w = ROUND_CORNER_W;
		params->round_corner_params.h = ROUND_CORNER_H;
		params->round_corner_params.lt_addr = left_top;
		params->round_corner_params.rt_addr = right_top;
		params->round_corner_params.lb_addr = left_bottom;
		params->round_corner_params.rb_addr = right_bottom;
	#endif
	/*prize-add-for round corner-houjian-20180918-end*/

}

static unsigned int lcm_compare_id(void)
{
    unsigned int id0,id1,id=0;
	unsigned char buffer[4] ={0};
	unsigned int array[16];
	
	display_bias_enable();
	
	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x04, buffer, 1);
	
	dprintf("LK TD4320 id = 0x%08x 0x%08x 0x%08x\n",buffer[0],buffer[1],buffer[2]);

	return (0x83 == buffer[0])?1:0; 
}

static void lcm_init(void)
{
	display_bias_enable();
	display_tpd_rst_enable(0);
	MDELAY(5);
	display_tpd_rst_enable(1);

	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(20);//must add delay
	
	push_table(NULL, init_setting_vdo,
		sizeof(init_setting_vdo) / sizeof(struct LCM_setting_table),1);
}

static void lcm_suspend(void)
{
    push_table(NULL, lcm_suspend_setting,
		sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table),1);
}

static void lcm_resume(void)
{
	lcm_init();
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int data_array[16];
	
	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(30);
	
	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(30);
}
#endif

LCM_DRIVER td4320_fhdp_dsi_vdo_auo64_lcm_drv = 
{
    .name			= "td4320_fhdp_dsi_vdo_auo64",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
};