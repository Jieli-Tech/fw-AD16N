/***********************************Jieli tech************************************************
  File : toy_update.c
  By   : mawancheng
  date : 2025-4-11
  brief: 该文件为独立的升级应用，公共消息check完ufw文件后会跳转到该应用，该应用会使用ld文件的d_u         pdate_and_new_stack代码段占用的ram，不可以和其他overlay相关的代码一起运行
********************************************************************************************/
#include "update_app.h"
#include "includes.h"
#include "device_app.h"
#include "app.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[update_app]"
#include "log.h"


void update_app(void)
{
    u32 err;

    log_info("toy_update_mode!\n");


    err = device_update(-1, 0);

    if (err) {
        /* 升级失败，切到下一个模式 */
        log_error("toy_update_err 0x%x\n", err);
        /* post_msg(1, MSG_NEXT_WORKMODE); */
    }
    work_mode++;
    return;
}
