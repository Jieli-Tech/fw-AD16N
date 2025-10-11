#ifndef __RC_APP_H__
#define __RC_APP_H__

#include "typedef.h"
#include "key.h"

#define SYS_TIMER_SOFTOFF  1//无操作定时关机功能
#define SYS_TIMER_SOFTOFF_TIME  3000  //n * 10ms
void dongle_app();
void rf_rc_app();
extern u16 rc_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
int rf_receiver_audio_callback(u8 *rf_packet, u16 packet_len);
#endif

