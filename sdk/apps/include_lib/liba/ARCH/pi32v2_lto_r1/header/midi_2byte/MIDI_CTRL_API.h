#ifndef MIDI_CTRL_API_h__
#define MIDI_CTRL_API_h__

#include "MIDI_DEC_API.h"
#include "typedef.h"

#define MIDI_CTRLC_NOTEOFF  0x80                        //�����ɿ�
#define MIDI_CTRLC_NOTEON   0x90                        //��������
#define MIDI_CTRLC_CTLCHG   0xB0
#define MIDI_CTRLC_PRGCHG   0xC0                        //�ı�����
#define MIDI_CTRLC_PWCHG    0xE0
#define MIDI_CTRLC_VELVIB   0xD0                       //�𶯷���

#define MIDI_CTRLC_VOL     0x07
#define MIDI_CTRLC_EXPR    0x0B
#define MIDI_CTRLC_SFT_ON  0x43


typedef struct _EVENT_CONTEXT_ {
    u8 msg;
    u8 val1;
    u8 val2;
    u8 trk_num;
    u16 delta;
    u16 val3;
} EVENT_CONTEXT;

typedef struct _MIDI_CTRL_PARM_ {
    char track_num;                //0-16
    unsigned int tempo;             //tempo
    void *priv;
    int (*output)(void *priv, void *data, int len);
} MIDI_CTRL_PARM;

typedef struct _MIDI_CTRL_CONTEXT_ {
    u32(*need_workbuf_size)();		                                   ///<��ȡ��Ҫ��buffer
    u32(*open)(void *work_buf, void *dec_parm, void *parm);                  ///������һ��������
    u32(*run)(void *work_buf);                                            ///����
    u32(*set_prog)(void *work_buf, u8 prog, u8 trk_num);                    ///��������
    u32(*note_on)(void *work_buf, u8 nkey, u8 nvel, u8 chn);               ///ָ�����ŵ�������,nkey��nvel����Чֵ��0-127
    u32(*note_off)(void *work_buf, u8 nkey, u8 chn, u16 time);             ///ָ���رյ�������,nkey��nvel����Чֵ��0-127,timeΪ˥��ʱ��ms����Ϊ0��ʹ����ɫ�е�˥��
    u32(*pitch_bend)(void *work_buf, u16 pitch_val, u8 chn);
    u32(*ctl_confing)(void *work_buf, u32 cmd, void *parm);
    u32(*vel_vibrate)(void *work_buf, u8 nkey, u8 vel_step, u8 vel_rate, u8 chn);  //vel_step:֧��0��7����������,  vel_rate֧��0��15����������
    u8 *(*query_play_key)(void *work_buf, u8 chn); //��ѯָ��ͨ����key���ţ���MAX_CTR_PLAYER_CNT��ֵ����Ϊ255��Ϊ��Чֵ
    u32(*glissando)(void *work_buf, u16 pitch_val, u8 key, u8 chn);
} MIDI_CTRL_CONTEXT;


extern MIDI_CTRL_CONTEXT *get_midi_ctrl_ops();
extern int get_midi_tone_compressor(void *work_buf);

#endif // MIDI_CTRL_API_h__



