#ifndef __APP_MODULES_H__
#define __APP_MODULES_H__
//  此文件在在服务器编译库时会自动生成，源文件位于apps/app/bsp/common/app_modules_h.c
//  作者: 刘杰
//  日期:2022年11月22日
//  设计目的: 用于在应用代码中控制各种算法模块的使用
//  注意事项：不要在库文件中包含


//  A格式解码
#define DECODER_A_EN 1

//  标准MP3格式解码
#define DECODER_MP3_ST_EN 1

//  WAV格式解码
#define DECODER_WAV_EN 1

//  F1A格式解码
#define DECODER_F1A_EN 1
#define MAX_F1A_CHANNEL 2

//  UMP3格式解码
#define DECODER_UMP3_EN 1

//  MIDI格式解码
#define DECODER_MIDI_EN 1

//  MIDI琴格式解码
#define DECODER_MIDI_KEYBOARD_EN 1

//  无缝循环使能
#define DECODER_LOOP_EN 1

//  mp3格式压缩
#define ENCODER_MP3_EN 1
#define ENCODER_MP3_STEREO 1 //  库支持压缩立体声的音源，改为0后只能压缩单声道，但是会节省2K+的RAM消耗

//  ump3格式压缩
#define ENCODER_UMP3_EN 1

//  a格式压缩
#define ENCODER_A_EN 1

//  MIO功能使能
#define HAS_MIO_EN 0

//  实时SPEED功能(用于扩音)使能
#define HAS_SPEED_EN 1

//  歌曲SPEED功能(用于解码)使能
#define HAS_SONG_SPEED_EN 1

//  陷波/移频啸叫抑制使能(二选一)
#define NOTCH_HOWLING_EN 1
#define PITCHSHIFT_HOWLING_EN 2
#define HOWLING_SEL PITCHSHIFT_HOWLING_EN
//  变声功能使能
#define VO_PITCH_EN 1

//  ECHO混响功能使能
#define ECHO_EN 1

//  A/F1A格式的解码文件需要同时判断后缀名确认采样率
#if ( DECODER_A_EN || DECODER_F1A_EN )
#define DECODE_SR_IS_NEED_JUDIGMENT 1
#else
#define DECODE_SR_IS_NEED_JUDIGMENT 0
#endif

//  APP应用使能
#define MUSIC_MODE_EN 1 //  mbox音乐应用模式
#define RECORD_MODE_EN 1 //  录音应用模式
#define LINEIN_MODE_EN 1 //  Linein应用模式
#define SIMPLE_DEC_EN 1 //  多路解码应用模式
#define LOUDSPEAKER_EN 1 //  扩音应用模式
#define RTC_EN 0 //  RTC模式

#define NEW_VM_EN 1 //  NVM使能


#endif
