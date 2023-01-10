#ifndef __LIB_SPEED_API_H__
#define __LIB_SPEED_API_H__


typedef struct _RS_IO_CONTEXT_ {
    void *priv;
    int(*output)(void *priv, void *data, int len);
} RS_IO_CONTEXT;


typedef struct _RS_PARA_STRUCT_ {
    unsigned short insample;
    unsigned short outsample;
    unsigned char  quality;
} RS_PARA_STRUCT;

typedef struct _SPEED_PITCH_PARA_STRUCT_ {
    unsigned short insample;
    unsigned short pitchrate;        //128<=>1
    unsigned char  quality;
    unsigned char speedin;
    unsigned char speedout;
    unsigned char pitchflag;
} SPEED_PITCH_PARA_STRUCT;

typedef struct  _RS_STUCT_API_ {
    unsigned int(*need_buf)();
    int (*open)(unsigned int *ptr, RS_PARA_STRUCT *rs_para, RS_IO_CONTEXT *rs_io);
    int (*run)(unsigned int *ptr, short *inbuf, int len);
} RS_STUCT_API;

RS_STUCT_API *get_resample_context();

typedef struct  _SPEEDPITCH_STUCT_API_ {
    unsigned int(*need_buf)(int srv);
    int(*open)(unsigned int *ptr, SPEED_PITCH_PARA_STRUCT *speedpitch_para, RS_IO_CONTEXT *rs_io);
    int(*run)(unsigned int *ptr, short *inbuf, int len);
} SPEEDPITCH_STUCT_API;

SPEEDPITCH_STUCT_API *get_sppitch_context();


#endif
