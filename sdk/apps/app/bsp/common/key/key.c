#pragma bss_seg(".key.data.bss")
#pragma data_seg(".key.data")
#pragma const_seg(".key.text.const")
#pragma code_seg(".key.text")
#pragma str_literal_override(".key.text.const")

#include "key.h"
#include "msg.h"
#include "app_config.h"
#include "sine_play.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[key]"
#include "log.h"

static u16(*key_msg_filter)(u8, u8, u8) = NULL;

#if KEY_IO_EN
#include "key_drv_io.h"
#endif

#if KEY_MATRIX_EN
#include "key_drv_matrix.h"
#endif

#if KEY_AD_EN
#include "key_drv_ad.h"
#endif

#if KEY_MIC_EN
#include "key_drv_mic.h"
#define d_check_suport_double_click(k)   mic_key_check_support_double_click(k)
#else
#define d_check_suport_double_click(k)   1
#endif

#if KEY_IR_EN
#include "key_ir.h"
#endif

#if KEY_TOUCH_EN
#include "key_touch.h"
#endif

#if KEY_LPTOUCH_EN
#include "key_lptouch.h"
#endif

static const key_interface_t *key_list[] = {
#if KEY_IO_EN
    &key_io_info,
#endif
#if KEY_AD_EN
    &key_ad_info,
#endif
#if KEY_MATRIX_EN
    &key_matrix_info,
#endif
#if KEY_MIC_EN
    &key_mic_info,
#endif
#if KEY_IR_EN
    &key_ir_info,
#endif
#if KEY_TOUCH_EN
    &key_touch_info,
#endif
#if KEY_LPTOUCH_EN
    &key_lptouch_info,
#endif
};
static volatile u8 is_key_active = 0;
void key_table_sel(void *msg_filter)
{
    local_irq_disable();
    key_msg_filter = msg_filter;
    local_irq_enable();
}

void key_init(void)
{
    key_puts("key init\n");

#if (KEY_IO_EN || KEY_AD_EN || KEY_MATRIX_EN || KEY_MIC_EN || KEY_IR_EN || KEY_TOUCH_EN || KEY_LPTOUCH_EN)
    for (int i = 0; i < (sizeof(key_list) / sizeof(key_list[0])); i++) {
        if (key_list[i]->key_init) {
            key_list[i]->key_init();
        }
    }
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
/**@brief   按键值获取函数
   @param
   @param
   @param
   @return  key_io_t类型按键参数，包含按键类型和键值
   @note
*/
/*----------------------------------------------------------------------------*/
key_io_t get_key_value(void)
{
    u8 i;
    key_io_t key;
    key.key_type = NO_KEY;

#if (KEY_IO_EN || KEY_AD_EN || KEY_MATRIX_EN || KEY_MIC_EN || KEY_IR_EN || KEY_TOUCH_EN || KEY_LPTOUCH_EN)
    for (i = 0; i < (sizeof(key_list) / sizeof(key_list[0])); i++) {
        if (key_list[i]->key_get_value) {
            key.key_num = key_list[i]->key_get_value();

            if (NO_KEY != key.key_num) {
                key.key_type = key_list[i]->key_type;
                /* log_printf("get_key_num = %d\n", key_num); */
                return key;
            }
        }
    }
#endif

    key.key_num = NO_KEY;
    return key;
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
    key_io_t key;

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
    key = get_key_value();
    cur_key = key.key_num;
    if (key.key_type != NO_KEY) {
        key_type = key.key_type;
        is_key_active = 35;      //35*10Ms
    } else if (is_key_active) {
        is_key_active --;
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

#include "power_interface.h"
void key_active_set(P33_IO_WKUP_EDGE edge)
{
    is_key_active = 35;      //35*10Ms
}
u8 key_idle_query(void)
{
    return !is_key_active;
}
REGISTER_LP_TARGET(key_lp_target) = {
    .name = "key",
    .is_idle = key_idle_query,
};


