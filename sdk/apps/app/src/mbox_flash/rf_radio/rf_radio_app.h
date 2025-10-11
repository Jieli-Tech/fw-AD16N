#ifndef __RF_RADIO_APP_H__
#define __RF_RADIO_APP_H__

#include "typedef.h"
#include "app.h"

#include "audio_rf_mge.h"

#if ENCODER_JLA_LW_EN
#define RA_ENC_SR      16000
#define RA_ENC_TYPE    FORMAT_JLA_LW
#define RA_ENC_FUNC    jla_lw_encode_api
#elif ENCODER_UMP3_EN
#define RA_ENC_SR      32000
#define RA_ENC_TYPE    FORMAT_UMP3
#define RA_ENC_FUNC    ump3_encode_api
#elif ENCODER_OPUS_EN
#define RA_ENC_SR      16000
#define RA_ENC_TYPE    FORMAT_OPUS
#define RA_ENC_FUNC    opus_encode_api
#elif ENCODER_SBC_EN
#define RA_ENC_SR      16000
#define RA_ENC_TYPE    FORMAT_SBC
#define RA_ENC_FUNC    sbc_encode_api
#else
#define RA_ENC_SR      0
#define RA_ENC_TYPE    -1
#define RA_ENC_FUNC    NULL
#endif

// #define PERIODIC_ADV_MODE 0 //周期广播模式，仅支持同一时刻单向收发
#define APP_SOFTOFF_CNT_TIME_10MS  TCFG_SHUTDOWN_TIME //5000 * 10ms

#if APP_SOFTOFF_CNT_TIME_10MS
#define app_softoff_time_reset(n) (n = maskrom_get_jiffies() + APP_SOFTOFF_CNT_TIME_10MS)
#else
#define app_softoff_time_reset(n)
#endif

extern u16 rf_radio_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
extern u32 g_rf_radio_softoff_jif;
extern u32 g_rf_radio_standby_jif_cnt;

void rf_radio_app(void);
void rra_timeout_reset(void);


void rra_timeout_reset(void);
void audio_init(void);
void audio_off(void);
#endif

