#ifndef midi_dec_h__
#define midi_dec_h__

#include "PlatformDefine.h"
#include "MIDIDefs.h"
#include "MIDI_DEC_API.h"
#include "MIDI_CTRL_API.h"
#include "midi_asm.h"

#ifdef WIN32
#define INLINE
#define PI 3.1415926535897f
#else
#define INLINE  __attribute__((always_inline))
#endif

extern const int MAINTRACK_USE_CHN;
extern const int MAX_CTR_PLAYER_CNT;
extern const int MAX_DEC_PLAYER_CNT;
extern const int NOTE_OFF_TRIGGER;
extern const int  MIDI_TONE_MODE;
extern const int MIDI_TONE_CURVE;
extern const int MIDI_SAVE_DIV_ENBALE;

enum {
    FORMAT_OK = 0,
    FORMAT_OK_BUT_NO_SUPPORT,
    FORMAT_ERR
};

#define   PCM_KIND			0    //
#define   ALAW_KIND			1    //1:2
#define   ADPCM_KIND		2    //1:3
#define   ADPCM4_KIND		3    //1:4
#define   PCM_KIND_DOWN     4    //1:2
#define   ALAW_KIND_DOWN    5    //1:4
#define   ADPCM_KIND_DOWN   6    //1:6
#define   ADPCM4_KIND_DOWN  7    //1:8
#define   CMP_KIND_END      8

#define  WAV_BUF			512
#define  WAV_BUF_BYTE		(WAV_BUF << 1)
#define  CURVE_BUF			128

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


#define MAX_WORD   10

typedef struct _MIDI_W2S_INFO_ {
    unsigned int word_cnt;                           //���ٸ���
    unsigned int data_pos[MAX_WORD + 1];               //������ʼλ��
    unsigned int data_len[MAX_WORD + 1];               //���ݳ���
    unsigned short *rec_data;
    char key_diff;
    unsigned int word_cnti;
} MIDI_W2S_INFO;


#define INBUF_SIZE  512
#define  MIDI_BIT_N(N)       ((1<<N))


#define  MAX_CHANNEL_NUM  16
#define  MAX_TRACK_NUM    MAX_CHANNEL_NUM
#define  MIDI_OBUF_BLOCK  32
#define  MIDI_RESAMPLE_SHIFT    13
#define  MIDI_RESAMPLE_VAL      ((1<<MIDI_RESAMPLE_SHIFT)-1)
#define  MIDI_SwitchLevel_CC   64
#define  MAX_GO_BACK    8
#define  MAX_GO_VAL     ((1<<MAX_GO_BACK)-1)
#define  VOL_Norm_Bit                12

#define SEQ_AE_OFF  0 // event is inactive
#define SEQ_AE_ON   1 // indicates we are counting the duration
#define SEQ_AE_REL  2 // indicates we are waiting on IsFinished
#define SEQ_AE_TM   1 // indicates 'count' is valid
#define SEQ_AE_KEEP 2 // keep the active event for possible restart (not currently used)

enum {
    MIDI_CTRL_SOS_ON_CC = 0x00,              //sustain
    MIDI_CTRL_EXPR_CC,
    MIDI_CTRL_VOL_CC,
    MIDI_CTRL_SFT_ON_CC,                 //vol
    MIDI_CTRL_SUS_ON_CC,
    MIDI_CTRL_MOD_CC,
    MIDI_CTRL_PAN_CC,
    MIDI_total_CC

};

enum {
    MAD_ERROR_MIDI = 0x05,
    MAD_ERROR_FILE_END = 0x40,
    MAD_ERROR_FILESYSTEM_ERR = 0x41,
    MAD_ERROR_DISK_ERR = 0x42,
    MAD_ERROR_SYNC_LIMIT = 0x43,
    MAD_ERROR_FF_FR_FILE_END = 0x44,
    MAD_ERROR_FF_FR_END = 0x45,
    MAD_ERROR_FF_FR_FILE_START = 0x46,
    MAD_ERROR_LIMIT = 0x47,
    MAD_ERROR_NODATA = 0x48
};

typedef struct _MIDI_INPUT_CONTROL_ {
    unsigned char input_buf[INBUF_SIZE];
    unsigned int  remain;
    unsigned int  rd_pos;
    unsigned char *buf_ptr;

} MIDI_INPUT_CONTROL;


typedef struct _MIDI_TRACK_CONTROL_ {
    unsigned int deltat; // deltaTime

} MIDI_TRACK_CONTROL;


typedef struct _RINFO_STRUCT_ {
    u16 initAtten; //Initial attenuation;
    u16 vol_dec;
    u16 vol_rel;
    u16 vol_hold;
    u16 vol_incr;
    char panv;
    u8 flenhigh;
    unsigned int loopStartR;
    unsigned int flen;
} RINFO_STRUCT;

typedef struct _my_Zones_ {
    int  vol_rel;

    unsigned int  loopStart; ///< the phase where we start looping
    unsigned int  loopLen;   ///< loopEnd - loopStart
    unsigned int  tableEnd;  ///< one past last playable sample
    unsigned int  sampleMap;  // Initial attenuation
    unsigned short initAtten;

    unsigned short  vol_incr;
    unsigned short  vol_dec;          ///hold_val
    unsigned short  vol_hold;         //get hold_val
    unsigned short  vol_hold_flag;    //hold count

    unsigned short   cents;     ///< detune amount (multiplier for frequency) frq *= pow(2, cents / 1200);//���õĵ���
    char   keyNum;     ///< 0-127, the nominal frequency is 440 * pow(2, (keyNum - 69)/12) //
    char   exclNote;
    char   mode;
    char pan;
    unsigned short have_array;
    unsigned char lowpass_level_vel0; //设置0就是不滤波
    unsigned char lowpass_curve_control;

} Zone_t;

typedef struct Voice_s {
    int isend;
    unsigned int indexIncr;
    unsigned int index;

    u32 wave_pos;
    u32  wavebuf;
    u32  start_index;
} Voice_t;

typedef struct _ENV {
    unsigned char env_use;
    int vol_f;
    u32 env_infov;
    u32 start_index;
    u32 env_pos;
    int env_points;
    int stepcnt;
    int stepval;
    u32 envi;
} ENV;

typedef  struct WaveInfo_s {

    unsigned char ison;   ///< SEQ_AE_REL after stop is sent to the instrument
    unsigned char flags;  ///< event options
    unsigned char sostenuto;
    unsigned char pendingStop;

    unsigned char  chnl;
    unsigned char  noKey;
    unsigned char  noVel;
    unsigned char  modulate_vel;  ///< flags indicating default modulator connection
    char   mode;
    unsigned char exclOn;
    unsigned short exclIndex;

    Voice_t  voice;
    unsigned char vol_cnt;
    unsigned char prognum;
    unsigned char player_on;

    unsigned int sampleMap;
    int   tableStart;///< first playable sample (usually 0)
    int   tableEnd;  ///< one past last playable sample
    int   loopStart; ///< the phase where we start looping
    int   loopLen;   ///< loopEnd - loopStart
    int   loopEnd;

    int pan;
    int panLft;
    int panRgt;                     //2^7


    int ctrlAtten;
    int initAtten; ///< Initial attenuation

    int attack_incr;

    u32  vol_hold_new;          //����vol_hold �� vol_atten����µ�vol_hold_new;
    u32  vol_now;               //��¼��ǰ����

    u16  vol_dec;
    u16  vol_hold;           //�������Ϊzone������ֹͣʱΪtab_len
    int  vol_dec_now;        //��ǰ���½�ֵ
    int  vol_rel;            //��Ӧzone��vol_rel

    u16  vol_hold_flag;      //hold count
    u16  vol_array[MIDI_OBUF_BLOCK];
    u16  vol_atten;             //��ǰ����

    short dec_oo_index;
    short dec_old;
    int dec_velpred;

    int dec_index;
    short loop_velpred;
    short loop_index;
    unsigned char trk_v;
    unsigned char vol_dec_now_bit;
    unsigned char active_off_flag;
    char stereo_index;
    char query_play_key;
    char stero_channel;
    ENV env_info;

    u32 note_on_off_dest;
    u32 note_on_off_cnt;

    unsigned char lowpass_level_vel0;
    int b1;
    int b2;
    int a2;
    int mem;
    int ii_index;
    int ii_phase;
} WaveInfo_t;

typedef struct _MidiMarkInfo {
    u16 mark_start;
    u16 mark_end;
    u16 cur_mark_pos;
    u16 cur_mark_store_pos;
    u8 loop_enable;
    u8 mark_enable;
    u8 mark_set;
} MidiMarkInfo;


typedef struct _MIDI_PLAYER_ {
    WaveInfo_t wave_info;
} MIDI_PLAYER;

typedef struct _MIDI_CHANNEL_CONTROL_ {
    unsigned char cc[MIDI_total_CC];
} MIDI_CHANNEL_CONTROL;


typedef  struct  Instr_s {
    unsigned int zoneMap[128];//zone  flash address
} Instr_t;

typedef struct _midi_bp_info_ {
    unsigned int  fpos_cnt;         //�ļ�λ��
    unsigned int  out_total;        //��ǰ����ʱ��
} midi_bp_info;

typedef struct _MIDI_SAVE_POS_STRUCT_ {
    unsigned int  fpos_cnt;
    unsigned int track_len[MAX_TRACK_NUM];            //track�����Ч����
    unsigned char lastMsg[MAX_TRACK_NUM];
    MIDI_TRACK_CONTROL track_info[MAX_TRACK_NUM];
    u8 value_rd;
    u8 saved_rd;
    unsigned int out_total;
    unsigned int channel_over;

    unsigned int  remain;
    unsigned int  rd_pos;

} MIDI_SAVE_POS_STRUCT;


typedef struct _MIDI_DECODE_VAR_ {
    /*input control*/
    MIDI_INPUT_CONTROL smf_data;
    const struct if_decoder_io smf_io;
    unsigned int file_len;
    unsigned char err;
    unsigned int spi_zone_start; //unsigned char *
    unsigned int spi_key_start;	 //unsigned char *
    unsigned int midi_spi_pos;	 //unsigned char *
    unsigned int midi_spi_posbk; //unsigned char *
    unsigned short  instr_spi;
    unsigned int instr_map;		 //unsigned short *
    short MAX_PLAYER_CNTt;
    unsigned short format;
    unsigned short numTrk;
    unsigned short tmDiv;
    unsigned int srTicks;
    short player_index;
    short player_index_stero;
    Zone_t  zone;
    /*out data control*/
    unsigned int sample_rate;                         //output_samplerate;
    //short midi_channel_on;
    unsigned short  max_ch;       //ʵ�ʷ�������Ч�¼�������channel
    unsigned char ins_trk;
    u8 ins_set;
    unsigned short  instr_start_index_set;
    unsigned int  instr_key_map_set;    //unsigned char *
    unsigned short midi_tempo_v;
    unsigned char track_set;
    unsigned char tracknV;

    unsigned int theTick;

    unsigned int  fpos_cnt;
    unsigned int track_len[MAX_TRACK_NUM];            //track�����Ч����
    unsigned char lastMsg[MAX_TRACK_NUM];
    MIDI_TRACK_CONTROL track_info[MAX_TRACK_NUM];
    u8 value_rd;
    u8 saved_rd;
    unsigned int out_total;
    unsigned int channel_over;

    u32 instr_key_map[MAX_CHANNEL_NUM];		//char *
    unsigned short instr_start_index[MAX_CHANNEL_NUM];
    unsigned char instr_on[MAX_CHANNEL_NUM];
    MIDI_CHANNEL_CONTROL channel_mixer[MAX_CHANNEL_NUM];

    dec_inf_t dec_info;
    //	MIDI_PLAYER midi_players[MAX_PLAYER_CNT];
    MIDI_PLAYER *midi_players;

    short *out_channel;    //һ֡��������ݴ�С
    int *out_val;    //һ֡��������ݴ�С

    u32 step;
    EX_CH_VOL_PARM ex_vol;
    u8  ex_flag;
    u8 triggered;
    EX_INFO_STRUCT mark_trig;

    u8 melody_on;
    u8 melody_stop;
    u16 set_chvol_in;
    u16 wav_sample_rate;
    EX_MELODY_STRUCT melody_trig;
    EX_MELODY_STOP_STRUCT melody_stop_trig;
    MIDI_NOTE_ON note_on_trigger;
    MIDI_NOTE_OFF note_off_trigger;
    MIDI_ADSR adsr_trigger;
    MIDI_CTRL_EVENT protect_trigger;

    u16 pitchBend_v[MAX_CHANNEL_NUM];

    unsigned int crtl_mode;
    unsigned char note_flag;
    unsigned char note_on;
    unsigned char midi_index;

    unsigned int now_srTicks;
    unsigned short wyl_index;
    unsigned short max_index_val;

    unsigned int mdiv_cnt;
    unsigned char tmDiv_enable;
    unsigned char melody_enable;
    unsigned char mute_enable;
    unsigned char tmDiv_up_cnt;
    unsigned char tmDiv_up_dest;
    unsigned char save_ptr_enable;
    unsigned char beat_enable;
    unsigned char several_notes;    //��������

    EX_TmDIV_STRUCT tmDiv_trig;
    EX_BeatTrig_STRUCT beat_trig;
    MIDI_SEMITONE_CTRL_STRUCT semitone_ctrl;
    WDT_CLEAR wdt_clear;
    MIDI_Limiter limiter_info;
    WaveInfo_t OKON_info[2];

    u32 instr_key_chnl_map[MAX_CHANNEL_NUM];		//char *
    unsigned short instr_start_chnl_index[MAX_CHANNEL_NUM];
    unsigned char instr_key_chnl_enable[MAX_CHANNEL_NUM];

    unsigned char bk_wr_cnt;
    unsigned char check_buf_flag;
    unsigned short bk_check_flag;

    MIDI_SAVE_POS_STRUCT *bk_array_ptr;
    MIDI_SAVE_POS_STRUCT mark_array[3];
    MidiMarkInfo midi_mark_info;
    MIDI_W2S_INFO w2s_obj;

    u8 key_set;
    u8 note_on_delta;
    u8 flag_t;             //��4����nnote_on�ı�־
    u8 okon_over_flag;      //�����Զ�������

    u32 note_on_cnt;        //

    u8 div_first_cnt;
    u8 replace_mode;
    u16 decay_speed[MAX_CHANNEL_NUM];

    u8 peekOK;
    u8 mute_flag;
    u16 keydown_cnt;
    u32 mute_threshold;

    u8 headSize;
    u8 compressEN;
    u8 stereoEN;
    u8 okon_mode;

    u8 key_mode;
    u8 pitch_enable;
    u8 main_interrupt_enable;
    u8 bitwidth;

    u8 okon_start_flag;
    u8 out_bit;
    u8 limiter_enable;
    u8 *play_key;

    u8 okon_trk_num;
    u8 okon_goon_flag;
    u8 okon_break_flag;

    int vol_threshold_flag;
    int sample_cnt;
    int sample_cnt_backup;
    u16 o_index;
    s16 o_len;
    u32 wdt_count;

    int(*fread)(void *file, void *buf, u32 len);
    int(*fseek)(void *file, u32 offset, int seek_mode);

    int mempool[0];
} MIDI_DECODE_VAR;

extern u32 midi_dec_confing(void *work_buf, u32 cmd, void *parm);

extern u32 midi_inputdata(MIDI_DECODE_VAR *mid_dec_obj, u32 length);
extern void midi_skipdata(MIDI_DECODE_VAR *mid_dec_obj, u32 length);

extern u32  DATA_REVERSE(u32 data, int i_max);


extern int MetaEvent(MIDI_DECODE_VAR *mid_dec_obj, short trkNum, int block_len);
extern int SysCommon(MIDI_DECODE_VAR *mid_dec_obj, unsigned short msg, int block_len);
extern int ChnlMessage(MIDI_DECODE_VAR *mid_dec_obj, short trkNum, unsigned short msg, int block_len);

extern int midi_ctrl_gen_sample(MIDI_DECODE_VAR *mid_dec_obj, int mp_index, int lenOUT);
extern int midi_dec_gen_sample(MIDI_DECODE_VAR *mid_dec_obj, int mp_index, int lenOUT);

extern void  midi_play_set_tempo(void *work_buf, MIDI_PLAY_CTRL_TEMPO *val);

//extern int midi_player_Message(MIDI_DECODE_VAR* mid_dec_obj, short trkNum, unsigned short msg, u8 val1, u8 val2, u16 time);
extern u8 control_note_on(MIDI_DECODE_VAR *mid_dec_obj, short chn, short trackn, int val1, int val2, int stero_channel, int stereoflag, int player_index);
extern u8 player_control_note_on(MIDI_DECODE_VAR *mid_dec_obj, short chn, short trackn, int val1, int val2, int stero_channel, int stereoflag, int player_index);
extern int control_stop(MIDI_DECODE_VAR *mid_dec_obj, WaveInfo_t *temp_t, short chn);
extern void SetKeyDecay(WaveInfo_t *wavInfo, int npoint);
void  midi_play_ctrl_on(void *work_buf, MIDI_PLAY_CTRL_MODE *mode_obj);
void Midi_IIR_Init(WaveInfo_t *wavInfo, Zone_t *zone, int SampleRate);

extern const unsigned char pan_tab[128];
extern const u16 pow2VALTab_12[12];
extern const u8 mkey_vol_tab[128];


extern const int smpl_rate_tab[9];

// #define MUL(macc,Rm,Rn)				asm volatile("%0 = %1 * %2(s)	\n\t" : "=&r"(macc) : "r"(Rm),"r"(Rn),"0"(macc) : )
// #define MAC(macc,Rm,Rn)				asm volatile("%0 += %1 * %2(s)	\n\t" : "=&r"(macc) : "r"(Rm),"r"(Rn),"0"(macc) : )
// #define MAS(macc,Rm,Rn)				asm volatile("%0 -= %1 * %2(s)	\n\t" : "=&r"(macc) : "r"(Rm),"r"(Rn),"0"(macc) : )
// #define MACDIV(r,macc,d)			asm volatile("%0 = %0 / %2(s) \n\t" "%1 = %0.l \n\t" :"=&r"(macc),"=&r"(r):"r"(d),"0"(macc),"1"(d))
// #define MRSI(Rm,macc,Rn)			asm volatile("%0 = %1 >> %2(s)	\n\t" : "=&r"(Rm) : "r"(macc),"r"(Rn),"0"(Rm) : )
// #define MRSR(Rm,macc,Rn)			asm volatile("%0 = %1 >> %2(s)	\n\t" : "=&r"(Rm) : "r"(macc),"r"(Rn),"0"(Rm) : )
// #define MULSI(Ro,macc,Rm,Rn,Rp)		{MUL(macc,Rm, Rn); MRSI(Ro, macc,Rp);}
// #define MULRSR(r,macc,a,b,shift)	{MUL(macc,a,b);MRSR(r,macc,shift);}
// #define MULRS(r,macc,a,b,shift)		{MUL(macc,a,b);MRSI(r,macc,shift);}
// #define MACSR(macc,Rn)				asm volatile ("%0 >>= %1 \n\t":"=&r"(macc):"r"(Rn),"0"(macc):)
// #define MACSL(macc,Rn)				asm volatile ("%0 <<= %1(sat) \n\t":"=&r"(macc):"r"(Rn),"0"(macc):)
// #define MACGET(h,l,macc)			asm volatile ("%0 = %2.h \n\t" "%1 = %2.l \n\t": "=&r" (h),"=&r" (l) : "r"(macc),"0"(h),"1"(l) :)
// #define VSHL(r,a,b)					r = b > 0 ? (a << b) : a >> (-b)

// #define GET_UBYTE_VALUE(value,addr,shift)  asm volatile("%0 = b[%1 + %2](u) \n\t":"=&r"(value):"r"(addr),"r"(shift),"0"(value):)
// #define GET_USHORT_VALUE(value,addr,shift) asm volatile("%0 = h[%1 + %2 << 1](u) \n\t":"=&r"(value):"r"(addr),"r"(shift),"0"(value):)

#endif // midi_dec_h__


