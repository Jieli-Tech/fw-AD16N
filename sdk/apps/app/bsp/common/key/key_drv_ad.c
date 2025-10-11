#pragma bss_seg(".adkey.data.bss")
#pragma data_seg(".adkey.data")
#pragma const_seg(".adkey.text.const")
#pragma code_seg(".adkey.text")
#pragma str_literal_override(".adkey.text.const")

#include "key_drv_ad.h"
#include "app_config.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[key_ad]"
#include "log.h"

#if KEY_AD_EN

#define EXTERN_R_UP     100//220 -> 22k,外挂上拉电阻,0使用内部上拉,内部上拉为10k

#if EXTERN_R_UP
#define R_UP       EXTERN_R_UP
#else
#define R_UP       100    //内部上拉为10K，有20%误差
#endif

#if (ADC_SAMPLE_BITS == 12)
#define ADC10_33   (0xfffL)
#else
#define ADC10_33   (0x3ffL)
#endif

#define ADC10_30   (ADC10_33 * 1000   / (1000 + R_UP))     //100K
#define ADC10_27   (ADC10_33 * 510    / (510  + R_UP))     //51K
#define ADC10_23   (ADC10_33 * 240    / (240  + R_UP))     //24K
#define ADC10_20   (ADC10_33 * 150    / (150  + R_UP))     //15K
#define ADC10_17   (ADC10_33 * 100    / (100  + R_UP))     //10K
#define ADC10_13   (ADC10_33 * 68     / (68   + R_UP))     //6.8K
#define ADC10_10   (ADC10_33 * 47     / (47   + R_UP))     //4.7K
#define ADC10_07   (ADC10_33 * 22     / (22   + R_UP))     //2.2K
#define ADC10_04   (ADC10_33 * 10     / (10   + R_UP))     //1K
#define ADC10_00   (0)

#define AD_NOKEY        ((ADC10_33 + ADC10_30) / 2)
#define ADKEY1_0		((ADC10_30 + ADC10_27) / 2)
#define ADKEY1_1		((ADC10_27 + ADC10_23) / 2)
#define ADKEY1_2		((ADC10_23 + ADC10_20) / 2)
#define ADKEY1_3		((ADC10_20 + ADC10_17) / 2)
#define ADKEY1_4		((ADC10_17 + ADC10_13) / 2)
#define ADKEY1_5		((ADC10_13 + ADC10_10) / 2)
#define ADKEY1_6		((ADC10_10 + ADC10_07) / 2)
#define ADKEY1_7		((ADC10_07 + ADC10_04) / 2)
#define ADKEY1_8		((ADC10_04 + ADC10_00) / 2)

const u16 ad_key_table[] = {
    ADKEY1_8, ADKEY1_7, ADKEY1_6, ADKEY1_5, ADKEY1_4,
    ADKEY1_3, ADKEY1_2, ADKEY1_1, ADKEY1_0
};

u8 ad_key_init_flag;
/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
static u8 key_filter(u8 key)
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
/**@brief   ad按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ad_key0_init(void)
*/
/*----------------------------------------------------------------------------*/
void ad_key_init(void)
{
    key_puts("ad key init\n");

    if ((u32) - 1 == adc_io2ch(AD_KEY_IO)) {
        key_puts("ad key init fail!\n");
        return;
    }

#if EXTERN_R_UP
    gpio_set_mode(IO_PORT_SPILT(AD_KEY_IO), PORT_INPUT_FLOATING);
#else
    gpio_set_mode(IO_PORT_SPILT(AD_KEY_IO), PORT_INPUT_PULLUP_10K);
#endif
    gpio_set_function(IO_PORT_SPILT(AD_KEY_IO), PORT_FUNC_GPADC);

    adc_add_sample_ch(adc_io2ch(AD_KEY_IO));
    ad_key_init_flag = 1;
}

/*----------------------------------------------------------------------------*/
/**@brief   获取ad按键值
   @param   void
   @param   void
   @return  key_number
   @note    tu8 get_adkey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 get_adkey_value(void)
{
    if (ad_key_init_flag != 1) {
        return 0xff;
    }
    u32 key_value = adc_get_value(adc_io2ch(AD_KEY_IO));
    if (key_value > AD_NOKEY) {
        return key_filter(NO_KEY);
    }

    u8 key_number = 0;
    for (; key_number < sizeof(ad_key_table) / sizeof(ad_key_table[0]); key_number++) {
        if (key_value < ad_key_table[key_number]) {
            break;
        }
    }
    /* log_info("key_value:%d, vdd_ad_val:%d, key_num:0x%x\n", key_value, key_value * 320 / 0x3ff, key_number); */
    return key_filter(key_number);
}

const key_interface_t key_ad_info = {
    .key_type = KEY_TYPE_AD,
    .key_init = ad_key_init,
    .key_get_value = get_adkey_value,
};
#endif

