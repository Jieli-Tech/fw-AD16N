#ifndef __AUDIO_DAC_API_H__
#define __AUDIO_DAC_API_H__
#include "typedef.h"
#include "audio_dac.h"
//#include "typedef.h"


#define DAC_PACKET_SIZE     (32*8)

//#define DAC_DECODER_KICK_SIZE     (DAC_PACKET_SIZE * 4)

#ifdef CUT_RAM_BUF_ENABLE
#define DAC_DECODER_BUF_SIZE     (DAC_PACKET_SIZE * 4)
#else
#define DAC_DECODER_BUF_SIZE     (DAC_PACKET_SIZE * 5)
#endif

void audac_clr_buf(void);
// void dac_trim_api(void);
// void dac_phy_init(u32 sr_sel);
// u32 audac_sr_lookup(u32 sr);
// void audac_analog_open_api(u32 delay_flag);
// u32 dac_sr_set(u32 sr);
// void dac_phy_off(void);
// void dac_phy_vol(u16 dac_l, u16 dac_r);

bool dac_mute(bool mute);
void dac_mode_init(u16 vol);
void dac_init(u32 sr, u32 delay_flag);
void dac_init_api(u32 sr);
void dac_sr_api(u32 sr);
void dac_off_api(void);
// void dac_sr_api(u32 sr);
u8 dac_vol(char set, u8 vol);
u8 stereo_dac_vol(char set, u8 vol_l, u8 vol_r);
void dac_usb_vol(u8 vol_r, u8 vol_l);
bool regist_dac_channel(void *psound_pre, void *psound_later, void *kick);
bool unregist_dac_channel(void *dec_hld);

bool dac_cbuff_active(void *sound_hld);
u32 get_dac_max_phy_vol(void);


void uac_audio_dac_percent(u8 ch);
void audio_dac_sync_once(void);

#endif


