/*******************************************************************************************
  File : audio_energy_detect.h
********************************************************************************************/
#ifndef _AUDIO_ENERGY_DETECT_H_
#define _AUDIO_ENERGY_DETECT_H_

#include "typedef.h"

#define AUDIO_E_DET_UNMUTE      (0x00)
#define AUDIO_E_DET_MUTE        (0x01)

typedef struct _audio_energy_detect_param {
    s16 mute_energy;                    // mute 阈值
    s16 unmute_energy;                  // unmute 阈值
    u16 mute_time_ms;                   // 能量低于mute阈值进入mute状态时间
    u16 unmute_time_ms;                 // 能量高于unmute阈值进入unmute状态时间
    u32 sample_rate;                    // 采样率
    u16 count_cycle_ms;                 // 能量计算的时间周期
} audio_energy_detect_param;

/*******************************************************
* Function name	: audio_energy_detect_open
* Description	: 打开能量检测
* Parameter		:
*   @param	     	能量获取配置参数
********************************************************/
extern void audio_energy_detect_open(audio_energy_detect_param *param);

/*******************************************************
* Function name	: audio_energy_detect_run
* Description	: 能量计算运算
* Parameter		:
*   @data	     	数据起始地址
*   @len	     	数据字节长度
********************************************************/
extern void audio_energy_detect_run(s16 *data, u32 len);

/*******************************************************
* Function name	: audio_energy_detect_close
* Description	: 关闭能量检测
* Parameter		:
********************************************************/
extern void audio_energy_detect_close(void);

/*******************************************************
* Function name	: auido_energy_detect_10ms_timer
* Description	: 能量计算定时检查函数，用于检查超时没有输入
				  数据运算
* Parameter		:
* Return        : 无
********************************************************/
extern void auido_energy_detect_10ms_timer(void);

/*******************************************************
* Function name	: audio_energy_detect_sample_rate_update
* Description	: 能量获取更新数据采样率
* Parameter		:
*   @sample_rate    采样率
********************************************************/
extern void audio_energy_detect_sample_rate_update(u32 sample_rate);

/*******************************************************
* Function name	: audio_energy_get_mute_status
* Description	: 获取能量mute状态
* Parameter		:
********************************************************/
extern u8 audio_energy_get_mute_status(void);

#endif

