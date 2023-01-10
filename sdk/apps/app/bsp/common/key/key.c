#include "key.h"
#include "key_drv_io.h"
#include "key_drv_ad.h"
#include "key_drv_mic.h"
#include "msg.h"
#include "app_config.h"
#include "sine_play.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[key]"
#include "log.h"

static u16(*key_msg_filter)(u8, u8, u8) = NULL;

#if KEY_IO_EN
#include "key_drv_io.h"
#define D_KEY_TYPE        KEY_TYPE_IO
#define d_key_init()      io_key_init()
#define d_get_key_value() get_iokey_value()
#define d_check_suport_double_click(k)   1
#endif

#if KEY_MATRIX_EN
#include "key_drv_matrix.h"
#define D_KEY_TYPE        KEY_TYPE_MATRIX
#define d_key_init()      matrix_key_init()
#define d_get_key_value() get_matrixkey_value()
#define d_check_suport_double_click(k)   1
#endif

#if KEY_AD_EN
#include "key_drv_ad.h"
#define D_KEY_TYPE        KEY_TYPE_AD
#define d_key_init()      ad_key_init()
#define d_get_key_value() get_adkey_value()
#define d_check_suport_double_click(k)   1
#endif

#if KEY_MIC_EN
#include "key_drv_mic.h"
#define D_KEY_TYPE        KEY_TYPE_MIC
#define d_key_init()      mic_key_init()
#define d_get_key_value() get_mickey_value()
#define d_check_suport_double_click(k)   mic_key_check_support_double_click(k)
#endif

#if KEY_IR_EN
#include "key_ir.h"
#define D_IRKEY_TYPE        KEY_TYPE_IR
#define d_irkey_init()      ir_key_init()
#define d_get_irkey_value() irkey_get_value()
#endif

#if ((!KEY_IO_EN) && (!KEY_MATRIX_EN) && (!KEY_AD_EN) && (!KEY_MIC_EN))
#define D_KEY_TYPE        0
#define d_key_init()      asm("nop")
#define d_get_key_value() 0xff
#define d_check_suport_double_click(k)   1
#endif

void key_table_sel(void *msg_filter)
{
    local_irq_disable();
    key_msg_filter = msg_filter;
    local_irq_enable();
}

void key_init(void)
{
    key_puts("key init\n");
    d_key_init();
#if KEY_IR_EN
    d_irkey_init();
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief   按键-消息转换函数
   @param   key_status：按键状态
   @param   key_num：最后按键值
   @param   key_type：按键值类型
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
static void key2msg_emit(u8 key_status, u8 key_num, u8 key_type)
{
    key_printf("key_status:%d key_num:%d key_type:%d \n", key_status, key_num, key_type);

    if (key_msg_filter) {
        u16 msg = key_msg_filter(key_status, key_num, key_type);
        if (msg != NO_MSG) {
            d_key_voice_kick();
            int ret = post_msg(1, msg);
            if (ret != 0) {
                key_printf("error !!! msg pool full \n");
            }
        }
    }
}


/*----------------------------------------------------------------------------*/
/**@brief   按键-消息转换函数,按键产生顺序：短按抬起/长按-->连按
   @param
   @param
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void key_scan()
{
    static volatile u8 key_type = NO_KEY;        ///<按键类型
    static volatile u8 last_key = NO_KEY;
    static volatile u8 key_press_counter = 0;
    volatile u8 cur_key = 0, key_status = 0, back_last_key = 0;
    /* key_io_t key; */

#if (KEY_DOUBLE_CLICK_EN)
    static u8 double_last_key = 0;
    static u8 key_press_flag = 0;
    static u8 click_cnt = 0;//按下次数
    if (key_press_flag) {
        key_press_flag++;
        if (key_press_flag > KEY_DOUBLE_CLICK_CNT) {
            key_press_flag = 0;
            if (click_cnt == 1) {
                key_puts(" One_Click ");
                key2msg_emit(KEY_SHORT_UP, double_last_key, key_type);
            } else if (click_cnt == 2) {
                key_puts(" Double_Click ");
                key2msg_emit(KEY_DOUBLE, double_last_key, key_type);
            } else if (click_cnt == 3) {
                key_puts(" Triple_Click ");
                key2msg_emit(KEY_TRIPLE, double_last_key, key_type);
            }
            click_cnt = 0;
        }
    }
#endif

    cur_key = NO_KEY;
    back_last_key = last_key;

    cur_key = d_get_key_value();
#if KEY_IR_EN
    if (cur_key == NO_KEY) {
        cur_key = d_get_irkey_value();
        if (cur_key != NO_KEY) {
            key_type = D_IRKEY_TYPE;
        }
    } else
#endif
    {
        key_type = D_KEY_TYPE;
    }

    if (cur_key == last_key) {                          //长时间按键
        if (cur_key == NO_KEY) {
            return;
        }
        key_press_counter++;
        if (key_press_counter == KEY_SHORT_CNT) {
            key_status = KEY_SHORT;
        } else if (key_press_counter == KEY_LONG_CNT) {      //长按
            key_status = KEY_LONG;
        } else if (key_press_counter == (KEY_LONG_CNT + KEY_HOLD_CNT)) {    //连按
            key_status = KEY_HOLD;
            key_press_counter = KEY_LONG_CNT;
        } else {
            return;
        }
    } else { //cur_key = NO_KEY, 抬键
        last_key = cur_key;
        if ((key_press_counter > KEY_SHORT_CNT) && (key_press_counter < KEY_LONG_CNT) && (cur_key == NO_KEY)) {   //短按抬起
            key_press_counter = 0;
            key_status = KEY_SHORT_UP;
        } else if ((cur_key == NO_KEY) && (key_press_counter >= KEY_LONG_CNT)) { //长按抬起
            key_press_counter = 0;
            key_status = KEY_LONG_UP;
            //puts("[UP]");
        } else {
            key_press_counter = 0;
            return;
        }
    }

#if (KEY_DOUBLE_CLICK_EN)
    if ((key_status == KEY_SHORT_UP) && (d_check_suport_double_click(back_last_key) == 1)) {
        if (key_press_flag == 0) {
            key_press_flag = 1;
            double_last_key = back_last_key;
            click_cnt = 1;
        } else if (double_last_key != back_last_key) {
            //单击后,快速单击另一个按键
            key_puts(" -- One_Click ");
            key2msg_emit(KEY_SHORT_UP, double_last_key, key_type);
            key_press_flag = 1;
            double_last_key = back_last_key;
            click_cnt = 1;
        } else {
            click_cnt++;
            key_press_flag = 1;
        }
    } else
#endif
    {
        key2msg_emit(key_status, back_last_key, key_type);
    }
}

