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

/*eq IIR type*/
typedef enum {
    EQ_IIR_TYPE_HIGH_PASS = 0x00,
    EQ_IIR_TYPE_LOW_PASS,
    EQ_IIR_TYPE_BAND_PASS,
    EQ_IIR_TYPE_HIGH_SHELF,
    EQ_IIR_TYPE_LOW_SHELF,
} EQ_IIR_TYPE;

struct eq_seg_info {
    unsigned short index;	//eq段序号
    unsigned short iir_type; //滤波器类型EQ_IIR_TYPE
    int freq;   	//中心截止频率
    float gain;             //增益（-12 ~12 db）
    float q;                //q值（0.3~30）
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
//系数计算子函数
/*----------------------------------------------------------------------------*/
/**@brief    低通滤波器
   @param    fc:中心截止频率
   @param    fs:采样率
   @param    quality_factor:q值
   @param    coeff:计算后，系数输出地址
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
extern void design_lp(int fc, int fs, float quality_factor, float *coeff);
/*----------------------------------------------------------------------------*/
/**@brief    高通滤波器
   @param    fc:中心截止频率
   @param    fs:采样率
   @param    quality_factor:q值
   @param    coeff:计算后，系数输出地址
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
extern void design_hp(int fc, int fs, float quality_factor, float *coeff);
/*----------------------------------------------------------------------------*/
/**@brief    带通滤波器
   @param    fc:中心截止频率
   @param    fs:采样率
   @param    gain:增益
   @param    quality_factor:q值
   @param    coeff:计算后，系数输出地址
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
extern void design_pe(int fc, int fs, float gain, float quality_factor, float *coeff);
/*----------------------------------------------------------------------------*/
/**@brief    低频搁架式滤波器
   @param    fc:中心截止频率
   @param    fs:采样率
   @param    gain:增益
   @param    quality_factor:q值
   @param    coeff:计算后，系数输出地址
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
extern void design_ls(int fc, int fs, float gain, float quality_factor, float *coeff);
/*----------------------------------------------------------------------------*/
/**@brief    高频搁架式滤波器
   @param    fc:中心截止频率
   @param    fs:采样率
   @param    gain:增益
   @param    quality_factor:q值
   @param    coeff:计算后，系数输出地址
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
extern void design_hs(int fc, int fs, float gain, float quality_factor, float *coeff);
/*----------------------------------------------------------------------------*/
/**@brief    滤波器系数检查
   @param    coeff:滤波器系数
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
extern int eq_stable_check(float *coeff);
float eq_db2mag(float x);
#endif
