#ifndef reverb_api_h__
#define reverb_api_h__

#if 1

typedef struct _EF_ECHO__PARM_ {
    unsigned int delay;                      //回声的延时时间 0-max_ms
    unsigned int decayval;                   // 0-70%
    unsigned int direct_sound_enable;        //直达声使能  0/1
    unsigned int energy_vad_threshold;       //绝对值能量阈值
} ECHO_PARM_SET;



typedef struct  _EF_REVERB_FIX_PARM {
    unsigned int wetgain;           //湿声增益
    unsigned int drygain;           //干声增益
    unsigned int sr;
    unsigned int max_ms;
} EF_REVERB_FIX_PARM;


typedef struct _ECHO_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} ECHO_IO_CONTEXT;


typedef struct __ECHO_FUNC_API_ {
    unsigned int (*need_buf)(unsigned int *ptr, EF_REVERB_FIX_PARM *echo_fix_parm);
    int (*open)(unsigned int *ptr, ECHO_PARM_SET *echo_parm, EF_REVERB_FIX_PARM *echo_fix_parm, ECHO_IO_CONTEXT *echooutput_io);
    int (*init)(unsigned int *ptr, ECHO_PARM_SET *echo_parm);
    int (*run)(unsigned int *ptr, short *inbuf, int len);
    void (*reset_wetdry)(unsigned int *ptr, int wetgain, int drygain);
} ECHO_FUNC_API;

typedef struct _EHCO_API_STRUCT_ {
    ECHO_PARM_SET echo_parm_obj;  //参数
    EF_REVERB_FIX_PARM echo_fix_parm;
    unsigned int *ptr;                   //运算buf指针
    ECHO_FUNC_API *func_api;            //函数指针
} ECHO_API_STRUCT;


extern ECHO_FUNC_API *get_echo_func_api();

#endif

#endif // reverb_api_h__
