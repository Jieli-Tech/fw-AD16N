

#pragma bss_seg(".msg.data.bss")
#pragma data_seg(".msg.data")
#pragma const_seg(".msg.text.const")
#pragma code_seg(".msg.text")
#pragma str_literal_override(".msg.text.const")

#include "msg.h"
#include "irq.h"
#include "common.h"
/* #include "string.h" */
#include "circular_buf.h"
#include "uart.h"
#include <stdarg.h>
#include "config.h"

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[msg]"
#include "log.h"


#define EVENT_TOTAL     (1+(sizeof(event2msg)/2 -1)/32)

static const u16 event2msg[] = {
    MSG_F1A1_FILE_END, 		/* 0 */
    MSG_F1A1_FILE_ERR, 		/* 1 */
    MSG_F1A1_LOOP,
    MSG_F1A2_FILE_END,
    MSG_F1A2_FILE_ERR,
    MSG_F1A2_LOOP,          /* 5 */
    MSG_MIDI_FILE_END,
    MSG_MIDI_FILE_ERR,
    NO_MSG,
    MSG_A_FILE_END,
    MSG_A_FILE_ERR,    		/* 10 */
    MSG_A_LOOP,
    MSG_MP3_FILE_END,
    MSG_MP3_FILE_ERR,
    MSG_MP3_LOOP,
    MSG_WAV_FILE_END,	/* 15 */
    MSG_WAV_FILE_ERR,
    MSG_WAV_LOOP,
    MSG_APP_SWITCH_ACTIVE,
    MSG_WFILE_FULL,

    MSG_OTG_IN,  /* 20 */
    MSG_OTG_OUT,
    MSG_USB_DISK_IN,
    MSG_USB_DISK_OUT,
    MSG_PC_IN,
    MSG_PC_OUT,  /* 25 */
    MSG_PC_SPK,  /* 26 */
    MSG_PC_MIC,  /* 27 */
    MSG_SDMMCA_IN,
    MSG_SDMMCA_OUT,
    MSG_AUX_IN,
    MSG_AUX_OUT,
    MSG_EXTFLSH_IN,

    NO_MSG,
};

static cbuffer_t msg_cbuf;
static u32 event_buf[EVENT_TOTAL];
static u32 msg_pool[MAX_POOL];
/* static u32 msg_test_num; */

static void event_init(void)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    for (u32 i = 0; i < EVENT_TOTAL; i++) {
        event_buf[i] = 0;
    }
    OS_EXIT_CRITICAL();
}

void clear_one_event(u32 event)
{
    if (event >= ARRAY_SIZE(event2msg)) {
        return;
    }
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    event_buf[event / 32] &= ~BIT(event % 32);
    OS_EXIT_CRITICAL();
}

static u32 get_event(void)
{
    u32 i;

    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    u32 event_cls;
    for (u32 i = 0; i < EVENT_TOTAL; i++) {
        __asm__ volatile("%0 = clz(%1)":"=r"(event_cls):"r"(event_buf[i]));
        if (event_cls != 32) {
            OS_EXIT_CRITICAL();
            /* log_info(" has event 0x%x\n",i*32 + (31 - event_cls)); */
            return i * 32 + (31 - event_cls);
        }
    }
    OS_EXIT_CRITICAL();

    return NO_EVENT;
}

static u32 event2msg_api(u32 event)
{
    return event2msg[event];
}

bool get_event_status(u32 event)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    if (event_buf[event / 32] & BIT(event % 32)) {
        OS_EXIT_CRITICAL();
        return TRUE;
    }
    OS_EXIT_CRITICAL();
    return FALSE;
}

int get_msg(int len, int *msg)
{
    u32 param = 0;
    u16 *t_msg = (u16 *)&param;
    //get_msg
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    u32 event = get_event();

    if (event != NO_EVENT) {
        /* log_info("event 0x%x\n", event); */
        clear_one_event(event);
        msg[0] = event2msg[event];
        //log_info("event_mag %d\n ", msg[0]);
        OS_EXIT_CRITICAL();
        return MSG_NO_ERROR;
    }

    u32 tlen = cbuf_read(&msg_cbuf, (void *)t_msg, MSG_HEADER_BYTE_LEN);

    if (MSG_HEADER_BYTE_LEN != tlen) {
        /* if (MSG_HEADER_BYTE_LEN != cbuf_read(&msg_cbuf, (void *)&param, MSG_HEADER_BYTE_LEN)) { */
        /* memset(msg, NO_MSG, len); */
        OS_EXIT_CRITICAL();

        /* log_info(" gm a 0x%x\n",param); */
        /*get no msg,cpu enter idle.why do this? TODO*/
        /* __builtin_pi32_idle(); */
        msg[0] = NO_MSG;
        return MSG_NO_MSG;
    }
    /* log_info(" gm a 0x%x\n",param); */
    msg[0] = t_msg[0] & (MSG_HEADER_ALL_BIT >> MSG_PARAM_BIT_LEN);
    u32 param_len = param >> MSG_TYPE_BIT_LEN;
    if (param_len > (len - 1)) {
        OS_EXIT_CRITICAL();
        return MSG_BUF_NOT_ENOUGH;
    }
    u32 rlen = cbuf_read(&msg_cbuf, (void *)(msg + 1), param_len * sizeof(int));
    if (param_len != rlen * sizeof(int)) {
        OS_EXIT_CRITICAL();
        return MSG_CBUF_ERROR;
    }

    OS_EXIT_CRITICAL();
    return MSG_NO_ERROR;
}

int post_event(int event)
{
    if (event >= ARRAY_SIZE(event2msg)) {
        return MSG_EVENT_PARAM_ERROR;
    }
    CPU_SR_ALLOC();
    /* log_info(">>> %d evenr post : 0x%x\n", msg_test_num++, event); */
    OS_ENTER_CRITICAL();
    event_buf[event / 32] |= BIT(event % 32);
    OS_EXIT_CRITICAL();
    return MSG_NO_ERROR;
}

int post_msg(int argc, ...)
{
    u32 param;
    u16 *t_msg = (u16 *)&param;
    CPU_SR_ALLOC();
    va_list argptr;
    OS_ENTER_CRITICAL();
    va_start(argptr, argc);
    if (!cbuf_is_write_able(&msg_cbuf, (argc - 1)*sizeof(int) + MSG_HEADER_BYTE_LEN)) {
        OS_EXIT_CRITICAL();
        return MSG_BUF_NOT_ENOUGH;
    }

    t_msg[0] = (MSG_HEADER_ALL_BIT >> MSG_PARAM_BIT_LEN) & va_arg(argptr, int);
    t_msg[0] = ((argc - 1) << (MSG_TYPE_BIT_LEN)) | t_msg[0];

    /* if (t_msg[0] != MSG_500MS) { */
    /*     log_info(">>> %d msg post 0x%x\n", msg_test_num++, t_msg[0]); */
    /* } */
    cbuf_write(&msg_cbuf, (void *)&t_msg[0], MSG_HEADER_BYTE_LEN);

    for (u32 i = 0; i < argc - 1; i++) {
        param = va_arg(argptr, int);
        cbuf_write(&msg_cbuf, (void *)&param, sizeof(int));
    }
    va_end(argptr);
    OS_EXIT_CRITICAL();
    return MSG_NO_ERROR;
}

void clear_all_message(void)
{
    cbuf_clear(&msg_cbuf);
}

void message_init()
{
    event_init();
    cbuf_init(&msg_cbuf, msg_pool, sizeof(msg_pool));
    cbuf_clear(&msg_cbuf);
}

