#include "key_touch.h"
#include "app_config.h"
#include "pl_cnt.h"
#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

#if KEY_TOUCH_EN

#define TOUCH_KEY_NUM       sizeof(touch_key_io)

static u8 touch_key_io[] = TOUCH_KEY_INIT;
static u32 touch_pre_value[TOUCH_KEY_NUM] = {0};
static u32 touch_normal_value[TOUCH_KEY_NUM] = {0};
static u32 touch_calibrate_cnt[TOUCH_KEY_NUM] = {0};
static u32 touch_calibrate_tmp_value[TOUCH_KEY_NUM] = {0};

static const struct pl_cnt_platform_data pl_cnt_data = {
    .port_num       = TOUCH_KEY_NUM,
    .port           = touch_key_io,
    .sum_num        = 5,
    .charge_time    = 25,
};

/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
u8 key_filter(u8 key)
{
    static u8 used_key = NO_KEY;
    static u8 old_key;
    static u8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }
    return used_key;
}

/*----------------------------------------------------------------------------*/
/**@brief   触摸按键初始化
   @param   void
   @note    void touch_key_init(void)
*/
/*----------------------------------------------------------------------------*/
void touch_key_init(void)
{
    pl_cnt_init(&pl_cnt_data);
    for (u8 i = 0; i < TOUCH_KEY_NUM; i ++) {
        touch_normal_value[i] -= (TOUCH_DELDA * 2);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   获取触摸按键值
   @param   void
   @param   void
   @return  key_num:io按键号
   @note    u8 get_touch_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 get_touch_key_value(void)
{
    u16 key_or = 0;
    u32 cur_val = 0;
    for (u8 i = 0; i < TOUCH_KEY_NUM; i ++) {
        cur_val = get_pl_cnt_value(i);
        log_info("before %d ", cur_val);
        //简单滤波
        if (touch_pre_value[i] == 0) {
            touch_pre_value[i] = cur_val;
        } else if (cur_val >= touch_pre_value[i]) {
            touch_pre_value[i] = touch_pre_value[i] + (u32)((cur_val - touch_pre_value[i]) * 0.2f);
        } else {
            touch_pre_value[i] = touch_pre_value[i] - (u32)((touch_pre_value[i] - cur_val) * 0.2f);
        }
        //处理滤波之后的值
        if (touch_pre_value[i] > (touch_normal_value[i] + TOUCH_DELDA)) {
            key_or |= BIT(i);
            touch_calibrate_cnt[i] = 0;
        } else {
            touch_calibrate_cnt[i] ++;
        }
        //定期标定常态下的基准值
        if (touch_calibrate_cnt[i] > TOUCH_VAL_CALIBRATE_CYCLE) {
            touch_normal_value[i] = touch_calibrate_tmp_value[i] / 10;
            touch_calibrate_tmp_value[i] = 0;
            touch_calibrate_cnt[i] = 0;
        } else if (touch_calibrate_cnt[i] >= (TOUCH_VAL_CALIBRATE_CYCLE / 2)) {
            if (touch_calibrate_cnt[i] < ((TOUCH_VAL_CALIBRATE_CYCLE / 2) + 10)) {
                touch_calibrate_tmp_value[i] += touch_pre_value[i];
            }
        } else {
            touch_calibrate_tmp_value[i] = 0;
        }
    }
    /* log_print("\n"); */
    static u8 pre_i = 0;
    u8 key_num = NO_KEY;
    if (key_or) {
        if (key_or & BIT(pre_i)) {
            key_num = pre_i;
        } else {
            for (u8 i = 0; i < TOUCH_KEY_NUM; i ++) {
                if (key_or & BIT(i)) {
                    key_num = i;
                    pre_i = i;
                    break;
                }
            }
        }
        log_info("after %d ", key_num);
    }
    return key_filter(key_num);
}


#endif

