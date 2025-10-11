#include "circular_buf.h"
#include "errno-base.h"
#include "NoiseSuppressLib.h"
#include "app_modules.h"
#include "sound_mge.h"
#include "remain_output.h"

#define LOG_TAG_CONST       NORM
/* #define LOG_TAG_CONST       OFF */
#define LOG_TAG             "[ans]"
#include "log.h"

#if defined(ANS_EN) && (ANS_EN)


#define NS_MODE                 0
#define NS_IS_WIDEBAND          1//0:8K 1:16K; 只支持16k 和 8k采样率, 8k采样率暂不建议使用

#define READSIZE                128     //每次run处理样点数
#define ANS_RUN_BUFFSIZE        5400
#define ANS_TMP_BUFSIZE         3604
#define ANS_NEAR_SIZE           (READSIZE * 2)

static int ans_runbuf[ANS_RUN_BUFFSIZE / 4] AT(.ans_data);
static int ans_tmpbuf[ANS_TMP_BUFSIZE / 4] AT(.ans_data);

static short ans_output_buff[READSIZE] AT(.ans_data);

static remain_ops ans_remain_ops AT(.ans_data);
static EFFECT_OBJ ans_effect_obj AT(.ans_data);
/* --------------------------------- */



/*----------------------------------------------------------------------------*/
/**@brief   ans_run函数
   @param   hld: 传递dbuf和运算接口
   @param   inbuf:输入数据
   @param   len:输入数据长度（byte）
   @return  处理了多少数据（byte）
   @author  liuhaokun
   @note    int ans_run(void *hld, short *inbuf, int len)
*/
/*----------------------------------------------------------------------------*/
int ans_run(void *hld, short *inbuf, int len)
{
    u32 rlen = 0;
    remain_ops *p_ans_remain_ops = &ans_remain_ops;

    /* 0. output 上次剩余的数据 */
    remain_output(&ans_effect_obj.sound, p_ans_remain_ops);
    /* kick_encode_api(NULL); */
    if (p_ans_remain_ops->remain_len) {        //上次数据输出仍旧没输出完
        return 0;
    }

    /* 1. 新一轮输入输出 */
    memset(ans_output_buff, 0, sizeof(ans_output_buff));    //清空outdata

    /* 2. input 数据 */
    if (len < READSIZE * sizeof(short)) {
        /* 本次输入数据不够128个点 */
        /* log_info("not enough 128\n"); */
        return 0;
    }

    /* 3. 运算run */
    NoiseSuppress_Process(ans_runbuf, ans_tmpbuf, inbuf, ans_output_buff, NULL, NULL, READSIZE);

    /* 4. 设置需要 output 数据量 */
    set_remain_len(p_ans_remain_ops, READSIZE * sizeof(short));      //设置output需要输出一包的数据量

    /* 5. 输出 */
    remain_output(&ans_effect_obj.sound, p_ans_remain_ops);
    /* kick_encode_api(NULL); */

    return READSIZE * sizeof(short);    //成功返回读取byte长度
}

/*----------------------------------------------------------------------------*/
/**@brief   ans 数据流初始化函数
   @param   obuf:
   @param   ppsound:后级sound
   @return  u8:返回值
   @author  liuhaokun
   @note    void *ans_phy(void *obuf, void **ppsound)
*/
/*----------------------------------------------------------------------------*/
void *ans_phy(void *obuf, void **ppsound)
{
    memset(&ans_effect_obj, 0, sizeof(ans_effect_obj));

    /* 配给前级 */
    ans_effect_obj.run = (void *)ans_run;

    ans_effect_obj.sound.p_obuf = obuf;
    *ppsound = &ans_effect_obj.sound;

    u32 ret = init_remain_obuf(&ans_remain_ops, sound_output, ans_output_buff, sizeof(ans_output_buff));
    if (ret) {
        log_error("init_remain_obuf error 0x%x\n", ret);
        return NULL;
    }
    return &ans_effect_obj;

}

/*----------------------------------------------------------------------------*/
/**@brief   ans初始化函数
   @param   obuf:后级的obuf
   @param   ppsound:后级的sound
   @return  void*:返回后级sound
   @author  liuhaokun
   @note    void *ans_api(void *obuf, void **ppsound)
*/
/*----------------------------------------------------------------------------*/
void *ans_api(void *obuf, void **ppsound, u32 sr)
{
    const u32 ans_supprt_sr[2] = {8000, 16000};
    if (sr != ans_supprt_sr[NS_IS_WIDEBAND]) {
        log_error("ans not support curr sr %d\n", sr);
        return NULL;
    }
    int tolbufsize = NoiseSuppress_QueryBufSize(NS_MODE, NS_IS_WIDEBAND);;
    log_info("tolbufsize %d %d\n", tolbufsize, sizeof(ans_runbuf));
    ASSERT(ANS_RUN_BUFFSIZE >= tolbufsize);

    int maxtmpbufsize = NoiseSuppress_QueryTempBufSize(NS_MODE, NS_IS_WIDEBAND);
    log_info("maxtmpbufsize %d %d\n", maxtmpbufsize, sizeof(ans_tmpbuf));
    ASSERT(ANS_TMP_BUFSIZE >= maxtmpbufsize);

    int ANS_AggressFactor = (int)(125 * 65536 / 100);/*范围：1~2,动态调整,越大越强(1.25f)*/
    /*噪声前级动态压制，对应定点范围 65536~131072 */

    int ANS_MinSuppress = (int)(10 * 65536 / 100);	/*范围：0~1,静态定死最小调整,越小越强(0.1f)*/
    /*噪声后级动态压制，对应定点范围 0~65536 */

    int ANS_NoiseLevel = (int)(1429 * 1024);      /*范围：-100dB ~ -40dB (-75dB) (1429 = (10^(-75/20))*2^23)*/
    /*噪声初始等级，影响算法启动性能 */

    NoiseSuppress_Init(ans_runbuf, ANS_AggressFactor, ANS_MinSuppress, NS_MODE, NS_IS_WIDEBAND, ANS_NoiseLevel);

    return ans_phy(obuf, ppsound);

}

/*----------------------------------------------------------------------------*/
/**@brief   录音文件seek函数
   @param   p_sound_out:前级sound
   @param   p_ans_obuf:ans输出buf
   @return  void*:返回后级sound
   @author  liuhaokun
   @note    void *link_ans_sound(void *p_sound_out, void *p_ans_obuf)
*/
/*----------------------------------------------------------------------------*/
void *link_ans_sound(void *p_sound_out, void *p_ans_obuf, u32 sr)
{

    sound_out_obj *p_next_sound = 0;
    sound_out_obj *p_curr_sound = p_sound_out;

    p_curr_sound->effect = ans_api(p_ans_obuf, (void **)&p_next_sound, sr);
    if (NULL != p_curr_sound->effect) {
        p_curr_sound->enable |= B_DEC_EFFECT;
        p_curr_sound = p_next_sound;
        log_info("ans init succ\n");
    } else {
        log_info("ans init fail\n");
    }
    return p_curr_sound;

}


//算法库使用的表
const short STFT_Win_FixHalf_M256_D128[] = {
    0,   402,   804,  1206,  1608,  2009,  2411,  2811,  3212,  3612,  4011,  4410,  4808,  5205,  5602,  5998, //
    6393,  6787,  7180,  7571,  7962,  8351,  8740,  9127,  9512,  9896, 10279, 10660, 11039, 11417, 11793, 12167, //
    12540, 12910, 13279, 13646, 14010, 14373, 14733, 15091, 15447, 15800, 16151, 16500, 16846, 17190, 17531, 17869, //
    18205, 18538, 18868, 19195, 19520, 19841, 20160, 20475, 20788, 21097, 21403, 21706, 22006, 22302, 22595, 22884, //
    23170, 23453, 23732, 24008, 24279, 24548, 24812, 25073, 25330, 25583, 25833, 26078, 26320, 26557, 26791, 27020, //
    27246, 27467, 27684, 27897, 28106, 28311, 28511, 28707, 28899, 29086, 29269, 29448, 29622, 29792, 29957, 30118, //
    30274, 30425, 30572, 30715, 30853, 30986, 31114, 31238, 31357, 31471, 31581, 31686, 31786, 31881, 31972, 32058, //
    32138, 32214, 32286, 32352, 32413, 32470, 32522, 32568, 32610, 32647, 32679, 32706, 32729, 32746, 32758, 32766, //
    32767, 32766, 32758, 32746, 32729, 32706, 32679, 32647, 32610, 32568, 32522, 32470, 32413, 32352, 32286, 32214, //
    32138, 32058, 31972, 31881, 31786, 31686, 31581, 31471, 31357, 31238, 31114, 30986, 30853, 30715, 30572, 30425, //
    30274, 30118, 29957, 29792, 29622, 29448, 29269, 29086, 28899, 28707, 28511, 28311, 28106, 27897, 27684, 27467, //
    27246, 27020, 26791, 26557, 26320, 26078, 25833, 25583, 25330, 25073, 24812, 24548, 24279, 24008, 23732, 23453, //
    23170, 22884, 22595, 22302, 22006, 21706, 21403, 21097, 20788, 20475, 20160, 19841, 19520, 19195, 18868, 18538, //
    18205, 17869, 17531, 17190, 16846, 16500, 16151, 15800, 15447, 15091, 14733, 14373, 14010, 13646, 13279, 12910, //
    12540, 12167, 11793, 11417, 11039, 10660, 10279,  9896,  9512,  9127,  8740,  8351,  7962,  7571,  7180,  6787, //
    6393,  5998,  5602,  5205,  4808,  4410,  4011,  3612,  3212,  2811,  2411,  2009,  1608,  1206,   804,   402, //
};

#endif



