/********************************************************/
/*      file name:fl10802_fwvga_dsi_vdo.c               */
/*      By @KosBeg for S-TELL M477                      */
/********************************************************/

#ifdef BUILD_LK
#include <stdio.h>
#include <string.h>
#else
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util;
#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY             							0xFE
#define REGFLAG_END_OF_TABLE      							0xFFF

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static struct LCM_setting_table {
  unsigned char cmd;
  unsigned char count;
  unsigned char para_list[64];
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update) {
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
};

static struct LCM_setting_table lcm_initialization_setting[] = {
    { 0xAA, 0x55, {0xA5, 0x80}},
    { 0xF2, 0x03, {0x00, 0x84, 0x08}},
    { 0xF0, 0x05, {0x55, 0xAA, 0x52, 0x08, 0x01}},
    { 0xB6, 0x03, {0x34, 0x34, 0x34}},
    { 0xB0, 0x03, {0x09, 0x09, 0x09}},
    { 0xB7, 0x03, {0x34, 0x34, 0x34}},
    { 0xB1, 0x03, {0x09, 0x09, 0x09}},
    { 0xB8, 0x03, {0x34, 0x34, 0x34}},
    { 0xB2, 0x03, {0x00, 0x00, 0x00}},
    { 0xB9, 0x03, {0x24, 0x24, 0x24}},
    { 0xB3, 0x03, {0x05, 0x05, 0x05}},
    { 0xBF, 0x01, {0x01}},
    { 0xBA, 0x03, {0x2F, 0x2F, 0x2F}},
    { 0xB5, 0x03, {0x0B, 0x0B, 0x0B}},
    { 0xBC, 0x03, {0x00, 0xA3, 0x00}},
    { 0xBD, 0x03, {0x00, 0xA3, 0x00}},
    { 0xBE, 0x02, {0x00, 0x5B}},
    { 0xD1, 0x34, {0x00, 0x01, 0x00, 0x37, 0x00, 0x69, 0x00, 0x89, 0x00, 0x9C, 0x00, 0xC3, 0x00, 0xEC, 0x01, 0x1F, 0x01, 0x46, 0x01, 0x84, 0x01, 0xB7, 0x02, 0x04, 0x02, 0x43, 0x02, 0x46, 0x02, 0x7E, 0x02, 0xBD, 0x02, 0xE3, 0x03, 0x17, 0x03, 0x38, 0x03, 0x64, 0x03, 0x7E, 0x03, 0x9A, 0x03, 0xA8, 0x03, 0xB3, 0x03, 0xB7, 0x03, 0xFE}},
    { 0xD2, 0x34, {0x00, 0x01, 0x00, 0x37, 0x00, 0x69, 0x00, 0x89, 0x00, 0x9C, 0x00, 0xC3, 0x00, 0xEC, 0x01, 0x1F, 0x01, 0x46, 0x01, 0x84, 0x01, 0xB7, 0x02, 0x04, 0x02, 0x43, 0x02, 0x46, 0x02, 0x7E, 0x02, 0xBD, 0x02, 0xE3, 0x03, 0x17, 0x03, 0x38, 0x03, 0x64, 0x03, 0x7E, 0x03, 0x9A, 0x03, 0xA8, 0x03, 0xB3, 0x03, 0xB7, 0x03, 0xFE}},
    { 0xD3, 0x34, {0x00, 0x01, 0x00, 0x37, 0x00, 0x69, 0x00, 0x89, 0x00, 0x9C, 0x00, 0xC3, 0x00, 0xEC, 0x01, 0x1F, 0x01, 0x46, 0x01, 0x84, 0x01, 0xB7, 0x02, 0x04, 0x02, 0x43, 0x02, 0x46, 0x02, 0x7E, 0x02, 0xBD, 0x02, 0xE3, 0x03, 0x17, 0x03, 0x38, 0x03, 0x64, 0x03, 0x7E, 0x03, 0x9A, 0x03, 0xA8, 0x03, 0xB3, 0x03, 0xB7, 0x03, 0xFE}},
    { 0xD4, 0x34, {0x00, 0x01, 0x00, 0x37, 0x00, 0x69, 0x00, 0x89, 0x00, 0x9C, 0x00, 0xC3, 0x00, 0xEC, 0x01, 0x1F, 0x01, 0x46, 0x01, 0x84, 0x01, 0xB7, 0x02, 0x04, 0x02, 0x43, 0x02, 0x46, 0x02, 0x7E, 0x02, 0xBD, 0x02, 0xE3, 0x03, 0x17, 0x03, 0x38, 0x03, 0x64, 0x03, 0x7E, 0x03, 0x9A, 0x03, 0xA8, 0x03, 0xB3, 0x03, 0xB7, 0x03, 0xFE}},
    { 0xD5, 0x34, {0x00, 0x01, 0x00, 0x37, 0x00, 0x69, 0x00, 0x89, 0x00, 0x9C, 0x00, 0xC3, 0x00, 0xEC, 0x01, 0x1F, 0x01, 0x46, 0x01, 0x84, 0x01, 0xB7, 0x02, 0x04, 0x02, 0x43, 0x02, 0x46, 0x02, 0x7E, 0x02, 0xBD, 0x02, 0xE3, 0x03, 0x17, 0x03, 0x38, 0x03, 0x64, 0x03, 0x7E, 0x03, 0x9A, 0x03, 0xA8, 0x03, 0xB3, 0x03, 0xB7, 0x03, 0xFE}},
    { 0xD6, 0x34, {0x00, 0x01, 0x00, 0x37, 0x00, 0x69, 0x00, 0x89, 0x00, 0x9C, 0x00, 0xC3, 0x00, 0xEC, 0x01, 0x1F, 0x01, 0x46, 0x01, 0x84, 0x01, 0xB7, 0x02, 0x04, 0x02, 0x43, 0x02, 0x46, 0x02, 0x7E, 0x02, 0xBD, 0x02, 0xE3, 0x03, 0x17, 0x03, 0x38, 0x03, 0x64, 0x03, 0x7E, 0x03, 0x9A, 0x03, 0xA8, 0x03, 0xB3, 0x03, 0xB7, 0x03, 0xFE}},
    { 0xF0, 0x05, {0x55, 0xAA, 0x52, 0x08, 0x00}},
    { 0xB5, 0x01, {0x6B}},
    { 0xB6, 0x01, {0x0A}},
    { 0xB7, 0x02, {0x00, 0x00}},
    { 0xB8, 0x04, {0x01, 0x05, 0x05, 0x05}},
    { 0xBA, 0x01, {0x01}},
    { 0xBC, 0x03, {0x00, 0x00, 0x00}},
    { 0xBD, 0x05, {0x01, 0x84, 0x07, 0x31, 0x00}},
    { 0xBE, 0x05, {0x01, 0x84, 0x07, 0x31, 0x00}},
    { 0xBF, 0x05, {0x01, 0x84, 0x07, 0x31, 0x00}},
    { 0xCC, 0x03, {0x03, 0x00, 0x00}},
    { 0xB1, 0x02, {0xF8, 0x00}},
    { 0x35, 0x01, {0x00}},
    { 0x36, 0x01, {0x00}},
    { 0x3A, 0x01, {0x55}},
    { 0x11, 0x01, {0x00}},
    { 0xFE, 120, {}},
    { 0x29, 0x01, {0x00}}
};

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util) {
  memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
};

static void lcm_get_params(LCM_PARAMS *params) {
  memset(params, 0, sizeof(LCM_PARAMS));
  params->dsi.packet_size = 256;
  params->dsi.vertical_backporch = 14;
  params->dsi.vertical_frontporch = 16;
  params->dsi.horizontal_sync_active = 4;
  params->dsi.PLL_CLOCK = 200;
  params->type = 2;
  params->dsi.LANE_NUM = 2;
  params->dsi.data_format.format = 2;
  params->dsi.intermediat_buffer_num = 2;
  params->dsi.PS = 2;
  params->dsi.vertical_sync_active = 2;
  params->dsi.noncont_clock_period = 2;
  params->width = 480;
  params->dsi.horizontal_active_pixel = 480;
  params->height = 854;
  params->dsi.vertical_active_line = 854;
  params->dsi.mode = 1;
  params->dsi.lcm_int_te_period = 1;
  params->dsi.noncont_clock = 1;
  params->dsi.data_format.color_order = 0;
  params->dsi.data_format.trans_seq = 0;
  params->dsi.data_format.padding = 0;
  params->dsi.lcm_int_te_monitor = 0;
  params->dsi.lcm_ext_te_monitor = 0;
  params->dsi.horizontal_backporch = 50;
  params->dsi.horizontal_frontporch = 50;
};

static void lcm_init(void) {
  SET_RESET_PIN(1);
  MDELAY(20);
  SET_RESET_PIN(0);
  MDELAY(20);
  SET_RESET_PIN(1);
  MDELAY(120);

  push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
};

static void lcm_suspend(void) {
  SET_RESET_PIN(1);
  MDELAY(10);
  SET_RESET_PIN(0);
  MDELAY(20);
  SET_RESET_PIN(1);
  MDELAY(120);
};

static void lcm_resume(void) {
  SET_RESET_PIN(1);
  MDELAY(10);
  SET_RESET_PIN(0);
  MDELAY(20);
  SET_RESET_PIN(1);
  MDELAY(120);
  
  push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
};

static unsigned int lcm_compare_id(void) {
  return 1;
/*
  unsigned char buffer[2];
  int data_array[16];

  SET_RESET_PIN(1);
  MDELAY(10);
  SET_RESET_PIN(0);
  MDELAY(10);
  SET_RESET_PIN(1);
  MDELAY(10);

  data_array[0] = 0x00063902;
  data_array[1] = 0x52AA55F0;
  data_array[2] = 0x00000108;
  dsi_set_cmdq(data_array, 3, 1);
  MDELAY(10);

  data_array[0] = 0x00023700;
  dsi_set_cmdq(data_array, 1, 1);
  MDELAY(10);

  read_reg_v2(0xC5, buffer, 2);

  return (buffer[1] | (buffer[0] << 8)) - 21776 <= 0;
*/
};

static void lcm_setbacklight(unsigned int level) {
  unsigned int data_array[16];

  if ( level >= 255 )
    level = 255;

  data_array[0] = 0x00023902;
  data_array[1] = (level << 8) | 0x51;
  dsi_set_cmdq(data_array, 2, 1);
};

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER nt35510_fwvga_dsi_vdo_lcm_drv =
{
    .name           = "nt35510_fwvga_dsi_vdo",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .set_backlight  = lcm_setbacklight,
    .compare_id     = lcm_compare_id,
};
