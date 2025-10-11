#pragma bss_seg(".midi_dec.data.bss")
#pragma data_seg(".midi_dec.data")
#pragma const_seg(".midi_dec.text.const")
#pragma code_seg(".midi_dec.text")
#pragma str_literal_override(".midi_dec.text.const")

#include "lib_midi.h"
#include "cpu.h"
#include "config.h"
#include "typedef.h"
#include "decoder_api.h"
#include "vfs.h"
#include "circular_buf.h"
/* #include "dac.h" */
/* #include "resample.h" */
#include "msg.h"
#include "errno-base.h"
#include "midi_api.h"
#include "MIDI_CTRL_API.h"
#include "MIDI_DEC_API.h"
#include "boot.h"
#include "decoder_msg_tab.h"
/* #include "dac_api.h" */

#include "audio_dac_api.h"
#include "audio_dac.h"
#include "app_modules.h"

#if defined(DECODER_MIDI_EN) && (DECODER_MIDI_EN)

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[midi_api]"
#include "log.h"

/* midi乐谱解码最大同时发声的key数,该值影响音符的叠加,值越大需要的解码buffer越大,需要的buffer大小由need_dcbuf_size()获取 */
extern const int MIDI_MAX_MARK_CNT;    //midi解码最大支持的mark数
extern const int MAX_DEC_PLAYER_CNT;//该值在app_config.c中定义

/* param */
static u32 midi_tone_tab                AT(.midi_buf);
/* static MIDI_CONFIG_PARM midi_t_parm     AT(.midi_buf); */
static MIDI_INIT_STRUCT init_info       AT(.midi_buf);
/* decode */
dec_obj dec_midi_hld;
cbuffer_t cbuf_midi                     AT(.midi_buf);
u16 obuf_midi[DAC_DECODER_BUF_SIZE / 2] AT(.midi_buf);
/* u32 midi_decode_buff[(MIDI_DEC_DBUF_SIZE + 3) / 4]    AT(.midi_buf); */
/* #define MIDI_CAL_BUF ((void *)&midi_decode_buff[0]) */

u32 midi_decode_buff_full[(MIDI_DEC_DBUF_SIZE + 3) / 4]       AT(.midi_buf);
u32 midi_decode_buff_nomark[MIDI_DEC_NOMARK_NEED_BUF_SIZE / 4]    AT(.midi_buf);

struct if_decoder_io midi_dec_io0 AT(.midi_buf);
/* const struct if_decoder_io midi_dec_io0 = { */
/*     &dec_midi_hld,      //input跟output函数的第一个参数，解码器不做处理，直接回传，可以为NULL */
/*     mp_input, */
/*     0, */
/*     mp_output, */
/*     decoder_get_flen, */
/*     0 */
/* }; */

MIDI_PLAY_CTRL_MODE *get_midi_mode(void)
{
    return &init_info.mode_info;
}

u32 *get_midi_switch_info(void)
{
    return &init_info.switch_info;
}

static u8 midi_musicsr_to_cfgsr(u32 sr)
{
    const u16 midi_sr_tab[] = {
        48000,
        44100,
        32000,
        24000,
        22050,
        16000,
        12000,
        11025,
        8000,
    };

    for (int i = 0; i < sizeof(midi_sr_tab) / sizeof(midi_sr_tab[0]); i++) {
        if (sr == midi_sr_tab[i]) {
            return i;
        }
    }

    ASSERT(0, "midi error sr !!!:%d \n", sr);
    return 0;
}

__attribute__((weak))
void midi_init_info(MIDI_INIT_STRUCT *init_info, u8 sr_index, u32 spi_pos_addr, int max_cnt)
{

}

u32 midi_decode_api(void *strm, void **ppdec, void *p_dp_buf)
{
    dec_data_stream *p_strm = strm;
    dec_obj **p_dec = (dec_obj **)ppdec;
    u32 buff_len, sr;
    decoder_ops_t *ops;
    log_info("midi_decode_api file:0x%x\n", (u32)p_strm->strm_source);
    if (!midi_tone_tab) {
        return E_MIDI_NO_CFG;
    }

    local_irq_disable();
    memset(&dec_midi_hld, 0, sizeof(dec_obj));
    local_irq_enable();

    u32 cal_buf_len;
    void *p_cal_buf;
    if (MIDI_MAX_MARK_CNT) {
        cal_buf_len = sizeof(midi_decode_buff_full);
        p_cal_buf = midi_decode_buff_full;
    } else {
        cal_buf_len = sizeof(midi_decode_buff_nomark);
        p_cal_buf = midi_decode_buff_nomark;
    }
    memset(p_cal_buf, 0, cal_buf_len);

    dec_midi_hld.type = D_TYPE_MIDI;

    ops = get_midi_ops();
    buff_len = ops->need_dcbuf_size();
    if (buff_len > cal_buf_len) {
        log_info("MIDI_DEC Need Buff Len:%d > %d\n", buff_len, cal_buf_len); //buff大小会随MAX_DEC_PLAYER_CNT改变
        return E_MIDI_DBUF;
    }
    /******************************************/
    cbuf_init(&cbuf_midi, &obuf_midi[0], sizeof(obuf_midi));
    /* char name[VFS_FILE_NAME_LEN] = {0}; */
    /* int file_len = vfs_file_name(p_file, (void *) g_file_sname, sizeof(g_file_sname)); */
    /* u8 *name = vfs_file_name(p_file); */
    /* log_info("file name : %s\n", g_file_sname); */

    memcpy(&midi_dec_io0, p_strm->io, sizeof(struct if_decoder_io));
    midi_dec_io0.priv      = &dec_midi_hld;

    sound_stream_obj *psound_strm = p_strm->strm_source;
    dec_midi_hld.p_file = (void *)psound_strm;
    dec_midi_hld.sound.p_obuf = &cbuf_midi;
    dec_midi_hld.sound.info |= MIDI_DEC_TRACK;
    dec_midi_hld.p_dbuf = p_cal_buf;
    dec_midi_hld.dec_ops = ops;
    dec_midi_hld.event_tab = (u8 *)&midi_evt[0];
    /* dec_midi_hld.p_dp_buf = p_dp_buf; */
    //
    /******************************************/
    ops->open(p_cal_buf, &midi_dec_io0, NULL);         //传入io接口，说明如下

    if (!(B_DEC_NO_CHECK & p_strm->strm_ctl)) {
        if (ops->format_check(p_cal_buf)) {                  //格式检查
            return E_MIDIFORMAT;
        }
    }
    /******************************************************************************/
    sr = dac_sr_read();                //获取采样率
    //dac_sr_api(sr);
    log_info(">>>>>>>sr:%d \n", sr);
    /**************相对MP3的调用流程多了这个，其他一致。这个一定要配置***************/
    /* midi_t_parm.player_t = MAX_DEC_PLAYER_CNT;                                //设置需要合成的最多按键个数，8到32可配 */
    /* midi_t_parm.sample_rate = midi_musicsr_to_cfgsr(sr);                            //采样率设为16k */
    /* midi_t_parm.spi_pos = (u8 *)midi_tone_tab;                    //spi_memory为音色文件数据起始地址 */
    /* log_info_hexdump(midi_t_parm.spi_pos, 16); */
    memset((u8 *)&init_info, 0x00, sizeof(init_info));
    /* init_info.init_info = midi_t_parm; */
    midi_init_info(&init_info, midi_musicsr_to_cfgsr(sr), midi_tone_tab, MAX_DEC_PLAYER_CNT);
    ops->dec_confing(p_cal_buf, CMD_INIT_CONFIG, &init_info);

    /**输出dec handle*/
    *p_dec = &dec_midi_hld;

    /* regist_dac_channel(&dec_midi_hld.sound, kick_decoder); //注册到DAC; */

    return 0;
}

u32 midi_dec_confing_api(dec_obj *obj, u32 cmd, void *parm)
{
    audio_decoder_ops *ops = (audio_decoder_ops *)obj->dec_ops;
    if (NULL == obj || NULL == ops) {
        return -1;
    }
    u32 ret = ops->dec_confing(obj->p_dbuf, cmd, parm);
    return ret;
}

int midi_decode_cfg_init(void)
{
    void *pvfs = 0;
    void *pvfile = 0;
    u32 err = 0;

    err = vfs_mount(&pvfs, (void *)NULL, (void *)NULL);
    if (err != 0) {
        return E_MOUNT;
    }

#if defined(MIDI_VER_4BYTE) && (MIDI_VER_SELECT == MIDI_VER_4BYTE)
    /* 4byte的音色库文件后缀是mdb */
    err = vfs_openbypath(pvfs, &pvfile, "/midi_cfg_ster1/00_MIDI.mdb");
#else
    err = vfs_openbypath(pvfs, &pvfile, "/midi_cfg/00_MIDI.mda");
#endif
    if (err != 0) {
        log_info("midi_dec mda open fail, try old midi_cfg.bin!\n");
        err = vfs_openbypath(pvfs, &pvfile, "/midi_cfg/midi_cfg.bin");
        if (err != 0) {
            vfs_fs_close(&pvfs);
            return E_OPENBYPATH;
        }
    }

    ///获取midi音色库的cache地址
    struct vfs_attr attr;
    vfs_get_attrs(pvfile, &attr);
    midi_tone_tab = boot_info.sfc.app_addr + attr.sclust;
    /* log_info("midi_tone_tab : 0x%x\n", midi_tone_tab); */

    vfs_file_close(&pvfile);
    vfs_fs_close(&pvfs);
    return 0;
}

extern const u8 midi_buf_start[];
extern const u8 midi_buf_end[];
u32 midi_buff_api(dec_buf *p_dec_buf)
{
    p_dec_buf->start = (u32)&midi_buf_start[0];
    p_dec_buf->end   = (u32)&midi_buf_end[0];
    return 0;
}

#endif
