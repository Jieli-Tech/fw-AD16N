#ifndef __AUDIO_DAC_API_H__
#define __AUDIO_DAC_API_H__
#include "typedef.h"
#include "audio_dac.h"
//#include "typedef.h"


#define DAC_PACKET_SIZE     (32*8)

// #define DAC_DECODER_KICK_SIZE     (DAC_PACKET_SIZE * 4)

#define DAC_DECODER_BUF_SIZE     (DAC_PACKET_SIZE * 5)



bool dac_mute(bool mute);
void dac_mode_init(u16 vol);
void dac_init_api(u32 sr);
void dac_sr_api(u32 sr);
void dac_off_api(void);
// void dac_sr_api(u32 sr);
u8 dac_vol(char set, u8 vol);
u8 stereo_dac_vol(char set, u8 vol_l, u8 vol_r);
void dac_usb_vol(u8 vol_r, u8 vol_l);
bool regist_dac_channel(void *sound, void *kick);
bool unregist_dac_channel(void *dec_hld);

bool dac_cbuff_active(void *sound_hld);
void dac_kick(void *sound_hld, void *pkick);


#endif


