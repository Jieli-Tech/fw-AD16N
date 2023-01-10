#ifndef __DEBUG_H__
#define __DEBUG_H__

// #define _MPU_WR_SFR_EN 	        do{JL_MPU->WREN = 0xE7;}while(0)
// #define _MPU_WR_EN			    JL_MPU->WREN
// #define _DEBUG_MSG_CLR			JL_MPU->MSG_CLR

// #define _DCU_CON                JL_DCU->CON
// #define _DCU_EMU_CON            JL_DCU->EMU_CON
// #define _DCU_EMU_MSG            JL_DCU->EMU_MSG
// #define _DCU_EMU_ID             JL_DCU->EMU_ID
// #define _DCU_CMD_CON            JL_DCU->CMD_CON
// #define _DCU_CMD_BEG            JL_DCU->CMD_BEG
// #define _DCU_CMD_END            JL_DCU->CMD_END
// #define _DCU_CNT_RACK           JL_DCU->CNT_RACK
// #define _DCU_CNT_UNACK          JL_DCU->CNT_UNACK

#define _DSP_PC_LIML0			q32DSP(0)->LIM_PC0_L
#define _DSP_PC_LIMH0			q32DSP(0)->LIM_PC0_H
#define _DSP_PC_LIML1			q32DSP(0)->LIM_PC1_L
#define _DSP_PC_LIMH1			q32DSP(0)->LIM_PC1_H
#define _DSP_PC_LIML2			q32DSP(0)->LIM_PC2_L
#define _DSP_PC_LIMH2			q32DSP(0)->LIM_PC2_H
#define _EMU_CON				q32DSP(0)->EMU_CON
#define _EMU_MSG				q32DSP(0)->EMU_MSG
#define _EMU_SSP_H              q32DSP(0)->EMU_SSP_H
#define _EMU_SSP_L              q32DSP(0)->EMU_SSP_L
#define _EMU_USP_H              q32DSP(0)->EMU_USP_H
#define _EMU_USP_L              q32DSP(0)->EMU_USP_L
#define _ETM_CON                q32DSP(0)->ETM_CON
#define _ESU_CON                q32DSP(0)->ESU_CON

#define _ICU_CON                q32DSP_icu(0)->CON
#define _ICU_EMU_CON            q32DSP_icu(0)->EMU_CON
#define _ICU_EMU_MSG            q32DSP_icu(0)->EMU_MSG
#define _ICU_EMU_ID             q32DSP_icu(0)->EMU_ID
#define _ICU_CMD_CON            q32DSP_icu(0)->CMD_CON
#define _ICU_CMD_BEG            q32DSP_icu(0)->CMD_BEG
#define _ICU_CMD_END            q32DSP_icu(0)->CMD_END
#define _ICU_CNT_RACK           q32DSP_icu(0)->CNT_RACK
#define _ICU_CNT_UNACK          q32DSP_icu(0)->CNT_UNACK


#define CDBG_IDx(n, id) ((1<<(n+4)) | (id<<(n*8+8)))
#define CDBG_INV         (1<<7)
#define CDBG_PEN         (1<<3)
#define CDBG_XEN         (1<<2)
#define CDBG_WEN         (1<<1)
#define CDBG_REN         (1<<0)

void debug_init();
void exception_analyze(unsigned int *sp);

/********************************** DUBUG SFR *****************************************/

u32 get_dev_id(char *name);


void emu_stack_limit_set(u8 mode, u32 limit_l, u32 limit_h);


#endif


