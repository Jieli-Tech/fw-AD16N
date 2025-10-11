#include "key_touch.h"
#include "app_config.h"
#include "pl_cnt.h"
#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[key_touch]"
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
        touch_normal_value[i] = 0;
        touch_pre_value[i] = 0;
        touch_calibrate_cnt[i] = 0;
    }
}

//获取加权平均值
u32 touch_key_weighted_averag(u32 old_data, u32 new_data, float factor)
{
    u32 data;
    if (old_data == 0) {
        data = new_data;
    } else if (old_data > new_data) {
        data = old_data - (u32)((old_data - new_data) * factor);
    } else {
        data = old_data + (u32)((new_data - old_data) * factor);
    }
    return data;
}

//只返回两种状态 0：没被按下  1：被按下
u8 touch_key_algo(u8 ch, u32 ch_res)
{
    u8 key_state = 0;
    //简单滤波
    touch_pre_value[ch] = touch_key_weighted_averag(touch_pre_value[ch], ch_res, 0.2f);

    //处理滤波之后的值
    if ((touch_normal_value[ch]) && (touch_pre_value[ch] > (touch_normal_value[ch] + TOUCH_DELDA))) {
        touch_calibrate_cnt[ch] = 0;
        key_state = 1;
    } else {
        touch_calibrate_cnt[ch] ++;
    }
    //定期标定常态下的基准值
    if (touch_calibrate_cnt[ch] > TOUCH_VAL_CALIBRATE_CYCLE) {
        touch_calibrate_tmp_value[ch] = touch_calibrate_tmp_value[ch] / 10;
        touch_normal_value[ch] = touch_key_weighted_averag(touch_normal_value[ch], touch_calibrate_tmp_value[ch], 0.2f);
        touch_calibrate_tmp_value[ch] = 0;
        touch_calibrate_cnt[ch] = 0;
    } else if (touch_calibrate_cnt[ch] >= (TOUCH_VAL_CALIBRATE_CYCLE / 2)) {
        if (touch_calibrate_cnt[ch] < ((TOUCH_VAL_CALIBRATE_CYCLE / 2) + 10)) {
            touch_calibrate_tmp_value[ch] += touch_pre_value[ch];
        }
    } else {
        touch_calibrate_tmp_value[ch] = 0;
    }
    return key_state;
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
        //printf("ch:%d val:%d\n", i, cur_val);
        if (touch_key_algo(i, cur_val)) {
            key_or |=  BIT(1);
        }
    }
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
        /* log_info("after %d ", key_num); */
    }
    return key_filter(key_num);
}

const key_interface_t key_touch_info = {
    .key_type = KEY_TYPE_TOUCH,
    .key_init = touch_key_init,
    .key_get_value = get_touch_key_value,
};

#endif

