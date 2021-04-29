#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
//#include <mach/mt_gpio.h>
//#include <linux/xlog.h>
//#include <mach/mt_pm_ldo.h>
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1440)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0
#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif
/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {


{0xE0,0x01,{0x00}},

{0xE1,0x01,{0x93}},
{0xE2,0x01,{0x66}},
{0xE3,0x01,{0xF9}},
{0x80,0x01,{0x03}}, //4lane 0x03 ;3lane 0x02

{0xE0,0x01,{0x01}},

{0x00,0x01,{0x01}},
{0x01,0x01,{0x34}},
{0x03,0x01,{0x01}},
{0x04,0x01,{0x34}},

{0x0A,0x01,{0x08}},

{0x13,0x01,{0x00}},
{0x14,0x01,{0x99}},

{0x17,0x01,{0x00}},
{0x18,0x01,{0xA4}},
{0x19,0x01,{0x00}},
{0x1A,0x01,{0x00}},
{0x1B,0x01,{0xA4}},
{0x1C,0x01,{0x00}},

{0x1F,0x01,{0x2F}},
{0x20,0x01,{0x2D}},
{0x21,0x01,{0x19}},
{0x22,0x01,{0x0D}},
{0x23,0x01,{0x02}},
{0x24,0x01,{0xFE}},

{0x26,0x01,{0xDF}},
{0x35,0x01,{0x13}},

{0x37,0x01,{0x09}},

{0x38,0x01,{0x04}},
{0x39,0x01,{0x01}},
{0x3A,0x01,{0x03}},
{0x3C,0x01,{0x60}},
{0x3D,0x01,{0x18}},
{0x3E,0x01,{0x80}},
{0x3F,0x01,{0x4E}},
{0x3D,0x01,{0xFF}},
{0x3E,0x01,{0xFF}},
{0x3F,0x01,{0xFF}},
{0x4B,0x01,{0x04}},

{0x40,0x01,{0x04}},
{0x41,0x01,{0xB4}},
{0x42,0x01,{0x70}},
{0x43,0x01,{0x24}},
{0x44,0x01,{0x0C}},
{0x45,0x01,{0x64}},

{0x55,0x01,{0x01}},//2 power
{0x56,0x01,{0x01}},
{0x57,0x01,{0x6D}},
{0x58,0x01,{0x0A}},
{0x59,0x01,{0x8A}},
{0x5A,0x01,{0x28}},
{0x5B,0x01,{0x23}},
{0x5C,0x01,{0x15}},

{0x5D,0x01,{0x75}},
{0x5E,0x01,{0x61}},
{0x5F,0x01,{0x53}},
{0x60,0x01,{0x47}},
{0x61,0x01,{0x43}},
{0x62,0x01,{0x34}},
{0x63,0x01,{0x39}},
{0x64,0x01,{0x22}},
{0x65,0x01,{0x3B}},
{0x66,0x01,{0x3A}},
{0x67,0x01,{0x3B}},
{0x68,0x01,{0x5C}},
{0x69,0x01,{0x4D}},
{0x6A,0x01,{0x57}},
{0x6B,0x01,{0x4B}},
{0x6C,0x01,{0x4B}},
{0x6D,0x01,{0x3E}},
{0x6E,0x01,{0x2E}},
{0x6F,0x01,{0x1A}},

{0x70,0x01,{0x75}},
{0x71,0x01,{0x61}},
{0x72,0x01,{0x53}},
{0x73,0x01,{0x47}},
{0x74,0x01,{0x43}},
{0x75,0x01,{0x34}},
{0x76,0x01,{0x39}},
{0x77,0x01,{0x22}},
{0x78,0x01,{0x3B}},
{0x79,0x01,{0x3A}},
{0x7A,0x01,{0x3B}},
{0x7B,0x01,{0x5C}},
{0x7C,0x01,{0x4D}},
{0x7D,0x01,{0x57}},
{0x7E,0x01,{0x4B}},
{0x7F,0x01,{0x4B}},
{0x80,0x01,{0x3E}},
{0x81,0x01,{0x2E}},
{0x82,0x01,{0x1A}},

{0xE0,0x01,{0x02}},

{0x00,0x01,{0x5D}},
{0x01,0x01,{0x51}},
{0x02,0x01,{0x5D}},
{0x03,0x01,{0x5D}},
{0x04,0x01,{0x5E}},
{0x05,0x01,{0x5F}},
{0x06,0x01,{0x51}},
{0x07,0x01,{0x41}},
{0x08,0x01,{0x45}},
{0x09,0x01,{0x5D}},
{0x0A,0x01,{0x5D}},
{0x0B,0x01,{0x4B}},
{0x0C,0x01,{0x49}},
{0x0D,0x01,{0x5D}},
{0x0E,0x01,{0x5D}},
{0x0F,0x01,{0x47}},
{0x10,0x01,{0x5F}},
{0x11,0x01,{0x5D}},
{0x12,0x01,{0x5D}},
{0x13,0x01,{0x5D}},
{0x14,0x01,{0x5D}},
{0x15,0x01,{0x5D}},

{0x16,0x01,{0x5D}},
{0x17,0x01,{0x50}},
{0x18,0x01,{0x5D}},
{0x19,0x01,{0x5D}},
{0x1A,0x01,{0x5E}},
{0x1B,0x01,{0x5F}},
{0x1C,0x01,{0x50}},
{0x1D,0x01,{0x40}},
{0x1E,0x01,{0x44}},
{0x1F,0x01,{0x5D}},
{0x20,0x01,{0x5D}},
{0x21,0x01,{0x4A}},
{0x22,0x01,{0x48}},
{0x23,0x01,{0x5D}},
{0x24,0x01,{0x5D}},
{0x25,0x01,{0x46}},
{0x26,0x01,{0x5F}},
{0x27,0x01,{0x5D}},
{0x28,0x01,{0x5D}},
{0x29,0x01,{0x5D}},
{0x2A,0x01,{0x5D}},
{0x2B,0x01,{0x5D}},

{0x2C,0x01,{0x1D}},
{0x2D,0x01,{0x10}},
{0x2E,0x01,{0x1D}},
{0x2F,0x01,{0x1D}},
{0x30,0x01,{0x1F}},
{0x31,0x01,{0x1E}},
{0x32,0x01,{0x00}},
{0x33,0x01,{0x10}},
{0x34,0x01,{0x06}},
{0x35,0x01,{0x1D}},
{0x36,0x01,{0x1D}},
{0x37,0x01,{0x08}},
{0x38,0x01,{0x0A}},
{0x39,0x01,{0x1D}},
{0x3A,0x01,{0x1D}},
{0x3B,0x01,{0x04}},
{0x3C,0x01,{0x1F}},
{0x3D,0x01,{0x1D}},
{0x3E,0x01,{0x1D}},
{0x3F,0x01,{0x1D}},
{0x40,0x01,{0x1D}},
{0x41,0x01,{0x1D}},

{0x42,0x01,{0x1D}},
{0x43,0x01,{0x11}},
{0x44,0x01,{0x1D}},
{0x45,0x01,{0x1D}},
{0x46,0x01,{0x1F}},
{0x47,0x01,{0x1E}},
{0x48,0x01,{0x01}},
{0x49,0x01,{0x11}},
{0x4A,0x01,{0x07}},
{0x4B,0x01,{0x1D}},
{0x4C,0x01,{0x1D}},
{0x4D,0x01,{0x09}},
{0x4E,0x01,{0x0B}},
{0x4F,0x01,{0x1D}},
{0x50,0x01,{0x1D}},
{0x51,0x01,{0x05}},
{0x52,0x01,{0x1F}},
{0x53,0x01,{0x1D}},
{0x54,0x01,{0x1D}},
{0x55,0x01,{0x1D}},
{0x56,0x01,{0x1D}},
{0x57,0x01,{0x1D}},

{0x58,0x01,{0x41}},
{0x59,0x01,{0x00}},
{0x5A,0x01,{0x00}},
{0x5B,0x01,{0x10}},
{0x5C,0x01,{0x02}},
{0x5D,0x01,{0x60}},
{0x5E,0x01,{0x01}},
{0x5F,0x01,{0x02}},
{0x60,0x01,{0x40}},
{0x61,0x01,{0x03}},
{0x62,0x01,{0x04}},
{0x63,0x01,{0x14}},
{0x64,0x01,{0x1A}},
{0x65,0x01,{0x55}},
{0x66,0x01,{0xB1}},
{0x67,0x01,{0x73}},
{0x68,0x01,{0x04}},
{0x69,0x01,{0x16}},
{0x6A,0x01,{0x58}},
{0x6B,0x01,{0x0A}},
{0x6C,0x01,{0x00}},
{0x6D,0x01,{0x00}},
{0x6E,0x01,{0x00}},
{0x6F,0x01,{0x88}},
{0x70,0x01,{0x00}},
{0x71,0x01,{0x00}},
{0x72,0x01,{0x06}},
{0x73,0x01,{0x7B}},
{0x74,0x01,{0x00}},
{0x75,0x01,{0xBC}},
{0x76,0x01,{0x00}},
{0x77,0x01,{0x05}},
{0x78,0x01,{0x34}},
{0x79,0x01,{0x00}},
{0x7A,0x01,{0x00}},
{0x7B,0x01,{0x00}},
{0x7C,0x01,{0x00}},
{0x7D,0x01,{0x03}},
{0x7E,0x01,{0x7B}},
{0x80,0x01,{0x06}},

{0xE0,0x01,{0x04}},

{0x00,0x01,{0x02}},
{0x02,0x01,{0x23}},
{0x03,0x01,{0x8F}},

{0x09,0x01,{0x11}},
{0x0E,0x01,{0x2A}},

{0x9A,0x01,{0x01}},
{0x9B,0x01,{0x05}},

{0xA9,0x01,{0x00}},
{0xAA,0x01,{0x68}},

{0xAC,0x01,{0x19}},
{0xAD,0x01,{0x15}},
{0xAE,0x01,{0x15}},

{0xE0,0x01,{0x00}},
{0xE6,0x01,{0x02}},
{0xE7,0x01,{0x06}},

{0xE0,0x01,{0x00}},
{0x35,0x01,{0x00}}, 
		{0x11,1,{0x00}},       
		{REGFLAG_DELAY, 120, {}},
		// Display ON            
		{0x29, 1, {0x00}},       
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

/*

static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}



static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;
	//	params->density = 320;//LCM_DENSITY;

#if LCM_DSI_CMD_MODE
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = BURST_VDO_MODE;
#endif
		params->dsi.vertical_sync_active = 4;
		params->dsi.vertical_backporch = 8;
		params->dsi.vertical_frontporch = 36;
		params->dsi.vertical_active_line = FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 8;	
		params->dsi.horizontal_backporch				= 26;//50
		params->dsi.horizontal_frontporch				= 26;  //30 
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		params->dsi.LANE_NUM = LCM_FOUR_LANE;
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
		params->dsi.ssc_disable				=1;
		
		params->dsi.packet_size=256;
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;


		// params->dsi.HS_TRAIL=20; 
		params->dsi.PLL_CLOCK = 209; //320 20200424 //240=4db  230=1.5db
/***********************    esd  check   ***************************/
//#ifndef BUILD_LK
	//	params->dsi.esd_check_enable = 1;
	//	params->dsi.customization_esd_check_enable = 1;
	//	params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
	//	params->dsi.lcm_esd_check_table[0].count        = 1;
	//	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
//#endif

}



//extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

static unsigned int lcm_compare_id(void)
{
    
        return 1;

}
static void lcm_init(void)
{	
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{

	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(1);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	//display_bias_disable();
	//display_ldo18_enable(0);
	//`nnnndisplay_ldo28_enable(0);
}


static void lcm_resume(void)
{
//	lcm_initialization_setting[141].para_list[0] = lcm_initialization_setting[141].para_list[0] + 1; 
	lcm_init();
	
//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}








LCM_DRIVER jd9366d_hdplus_dsi_vdo_dzx_lcm_drv = {
	.name		= "jd9366d_hdplus_dsi_vdo_dzx",
    	//prize-lixuefeng-20150512-start
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "jd9366d_dzx",
		.vendor	= "dezhixin",
		.id		= "0x9366",
		.more	= "lcm_1440*720",
	},
	#endif
	//prize-lixuefeng-20150512-end	
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id 	= lcm_compare_id,
	
#if (LCM_DSI_CMD_MODE)
	.set_backlight	= lcm_setbacklight,
    .update         = lcm_update,
#endif
};

