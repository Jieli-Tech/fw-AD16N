#ifndef __HW_EQ_H__
#define __HW_EQ_H__
#include "typedef.h"
// #include "math_fast_function.h"

struct data_q_struct {
    long data;
    char q;
};

enum {				//运行模式
    NORMAL = 0,		//正常模式
    MONO,			//单声道模式
    STEREO			//立体声模式
};
enum {			 	//输出数据类型
    DATO_SHORT = 0, //short
    DATO_INT,		//int
    DATO_FLOAT		//float
};
enum {				//输入数据类型
    DATI_SHORT = 0, //short
    DATI_INT,		// int
    DATI_FLOAT		//float
};
enum {					//输入数据存放模式
    BLOCK_DAT_IN = 0, 	//块模式，例如输入数据是2通道，先存放完第1通道的所有数据，再存放第2通道的所有数据
    SEQUENCE_DAT_IN,	//序列模式，例如输入数据是2通道，先存放第通道的第一个数据，再存放第2个通道的第一个数据，以此类推。
};
enum {				 //输出数据存放模式
    BLOCK_DAT_OUT = 0,//块模式，例如输出数据是2通道，先存放完第1通道的所有数据，再存放第2通道的所有数据
    SEQUENCE_DAT_OUT, //序列模式，例如输入数据是2通道，先存放第通道的第一个数据，再存放第2个通道的第一个数据，以此类推。
};

typedef long long   int64;
#define div64(n, d, q) (((int64)(n) << (q)) / (d))
#define mul64(x, y, q) (((int64)(x) * (y)) >> (q))

#define EQ_OUTQ			22
#define EQ_COEFF0		coeff[3]
#define EQ_COEFF1		coeff[1]
#define EQ_COEFF2		coeff[0]
#define EQ_COEFF3		coeff[4]
#define EQ_COEFF4		coeff[2]

void design_pe_for_int(int fc, int fs, int gain, int quality_factor, float *coeff);
#endif
