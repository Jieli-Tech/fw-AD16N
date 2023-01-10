// *INDENT-OFF*
ifndef __APP_MODULES_H__
define __APP_MODULES_H__

** 此文件在在服务器编译库时会自动生成，源文件位于apps/app/bsp/common/app_modules_h.c

** 作者: 刘杰
** 日期:2022年11月22日
** 设计目的: 用于在应用代码中控制各种算法模块的使用
** 注意事项：不要在库文件中包含

-
-
** A格式解码
#ifdef HAS_A_DECODER
define DECODER_A_EN  1
#else
define DECODER_A_EN  0
#endif


-
** 标准MP3格式解码
#ifdef HAS_MP3_ST_DECODER
define DECODER_MP3_ST_EN 1
#else
define DECODER_MP3_ST_EN 0
#endif


-
** WAV格式解码
#ifdef HAS_WAV_DECODER
define DECODER_WAV_EN 1
#else
define DECODER_WAV_EN 0
#endif


-
** F1A格式解码
#ifdef HAS_F1A_DECODER
define DECODER_F1A_EN  1
define MAX_F1A_CHANNEL HAS_MAX_F1A_NUMBER
#else
define DECODER_F1A_EN  0
define MAX_F1A_CHANNEL 0
#endif

-
** UMP3格式解码
#ifdef HAS_UMP3_DECODER
define DECODER_UMP3_EN 1
#else
define DECODER_UMP3_EN 0
#endif

-
** MIDI格式解码
#ifdef HAS_MIDI_DECODER
define DECODER_MIDI_EN 1
#else
define DECODER_MIDI_EN 0
#endif

-
** MIDI琴格式解码
#ifdef HAS_MIDI_KEYBOARD_DECODER
define DECODER_MIDI_KEYBOARD_EN 1
#else
define DECODER_MIDI_KEYBOARD_EN 0
#endif

-
** 无缝循环使能
#ifdef HAS_DECODER_LOOP_EN
define DECODER_LOOP_EN 1
#else
define DECODER_LOOP_EN 0
#endif

-
** mp3格式压缩
#ifdef HAS_MP3_ENCODER
define ENCODER_MP3_EN     1
define ENCODER_MP3_STEREO     1 --  库支持压缩立体声的音源，改为0后只能压缩单声道，但是会节省2K+的RAM消耗
#else
define ENCODER_MP3_EN     0
#endif


-
** ump3格式压缩
#ifdef HAS_UMP3_ENCODER
define ENCODER_UMP3_EN  1
#else
define ENCODER_UMP3_EN  0
#endif

-
** a格式压缩
#ifdef HAS_A_ENCODER
define ENCODER_A_EN  1
#else
define ENCODER_A_EN  0
#endif

-
** MIO功能使能
#ifdef HAS_MIO_PLAYER
define HAS_MIO_EN  1
#else
define HAS_MIO_EN  0
#endif

-
** SPEED功能(变速变调)使能
#ifdef HAS_SPEED_EFFECT
define HAS_SPEED_EN  1
#else
define HAS_SPEED_EN  0
#endif

-
** A/F1A格式的解码文件需要同时判断后缀名确认采样率
if ( DECODER_A_EN || DECODER_F1A_EN  )
define DECODE_SR_IS_NEED_JUDIGMENT 1
else
define DECODE_SR_IS_NEED_JUDIGMENT 0
endif

-
** APP应用使能
#ifdef HAS_MUSIC_MODE
define MUSIC_MODE_EN  1  -- mbox音乐应用模式
#else
define MUSIC_MODE_EN  0  -- mbox音乐应用模式
#endif

#ifdef HAS_RECORD_MODE
define RECORD_MODE_EN  1  -- 录音应用模式
#else
define RECORD_MODE_EN  0  -- 录音应用模式
#endif

#ifdef HAS_LINEIN_MODE
define LINEIN_MODE_EN  1 -- Linein应用模式
#else
define LINEIN_MODE_EN  0 -- Linein应用模式
#endif

#ifdef HAS_SIMPLE_DEC_MODE
define SIMPLE_DEC_EN  1  -- 多路解码应用模式
#else
define SIMPLE_DEC_EN  0  -- 多路解码应用模式
#endif

-
#ifdef HAS_NEW_VM_MODE
define NEW_VM_EN  1  -- NVM使能
#else
define NEW_VM_EN  0  -- NVM使能
#endif

-
-
endif
