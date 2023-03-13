
#ifndef syn_ps_api_h__
#define syn_ps_api_h__
#include "typedef.h"

enum {
    PS69_SPEED_UP = 1,
    PS69_SPEED_DOWN = 2
};

typedef struct _PS69_CONTEXT_IO_ {
    u8 *outpriv;
    u32(*output)(void *priv, u8 *buf, int len);
} PS69_audio_IO;

typedef struct _PS69_CONTEXT_CONF_ {
    u16 chn;
    u16 sr;
    u16 speedV;
    u16 pitchV;
} PS69_CONTEXT_CONF;

typedef struct _PS69_API_CONTEXT_ {
    u32(*need_size)(void);
    u32(*open)(u8 *ptr, PS69_audio_IO *audio_IO);
    u32(*dconfig)(u8 *ptr, PS69_CONTEXT_CONF *conf_obj);
    u32(*run)(u8 *ptr, s16 *inbuf, u32 len);
    u32(*run_same)(u8 *ptr, s16 *inbuf, u32 len, s16 *outdata);
} PS69_API_CONTEXT;

#define  ABS_LEN_FLAG       0x100

extern PS69_API_CONTEXT   *get_ps_cal_api();

#endif // syn_ps_api_h__


#if 0

{
    PS69_API_CONTEXT *testops = get_ps69_api();
    PS69_CONTEXT_CONF test_config_obj;

    test_config_obj.pitchV = 32768;
    test_config_obj.sr = 44100;
    test_config_obj.chn = 2;
    test_config_obj.speedV = 80;

    bufsize = syn_ops->need_size();
    ptr = malloc(bufsize);
    testops->open(ptr, &testIO);
    testops->dconfig(ptr, &test_config_obj);

    while (1) {
        if (feof(fp1)) {
            break;
        }
        fread(test_buf, 1, TEST_LEN, fp1);
        testops->run(ptr, test_buf, TEST_LEN);
    }

    free(ptr);
}

#endif

