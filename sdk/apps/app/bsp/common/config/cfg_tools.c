#include "typedef.h"
#include "irq.h"
#include "common.h"
#include "msg.h"
#include "circular_buf.h"
#include "uart.h"
#include "config.h"
#include "crc16.h"
#include "my_malloc.h"
#include "cfg_tools.h"
#include "app_config.h"
#include "device/cdc.h"
#include "effects_adj.h"

/* #define LOG_TAG_CONST       NORM */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[cfg]"
#include "log.h"

/* #if TCFG_CFG_TOOL_ENABLE */
#if (USB_DEVICE_CLASS_CONFIG & CDC_CLASS) && TCFG_CFG_TOOL_ENABLE

struct cfg_tool_info {
    /* R_QUERY_BASIC_INFO 		r_basic_info; */
    R_QUERY_FILE_SIZE		r_file_size;
    R_QUERY_FILE_CONTENT	r_file_content;
    R_PREPARE_WRITE_FILE	r_prepare_write_file;
    R_READ_ADDR_RANGE		r_read_addr_range;
    R_ERASE_ADDR_RANGE      r_erase_addr_range;
    R_WRITE_ADDR_RANGE      r_write_addr_range;
    /* R_ENTER_UPGRADE_MODE    r_enter_upgrade_mode; */

    /* S_QUERY_BASIC_INFO 		s_basic_info; */
    S_QUERY_FILE_SIZE		s_file_size;
    S_PREPARE_WRITE_FILE    s_prepare_write_file;
    u8 *write_file_ptr;
};

struct cfg_tool_event {
    u32 event;
    u8 *packet;
    u16 size;
};

static struct cfg_tool_info info;
#define __this  (&info)
#define TEMP_BUF_SIZE	256
static u8 local_packet[TEMP_BUF_SIZE];

extern const char *sdk_version_info_get(void);
extern u8 *sdfile_get_burn_code(u8 *len);
extern int norflash_erase(u32 cmd, u32 addr);
extern void doe(u16 k, void *pBuf, u32 lenIn, u32 addr);
extern void go_mask_usb_updata();

const char fa_return[] = "FA";	//失败
const char ok_return[] = "OK";	//成功
const char er_return[] = "ER";	//不能识别的命令
static u32 size_total_write = 0;
static u8 parse_seq = 0;
struct cfg_tool_event cfg_packet;
#define TWS_FUNC_ID_CFGTOOL_SYNC    TWS_FUNC_ID('C', 'F', 'G', 'S')

static u32 encode_data_by_user_key(u16 key, u8 *buff, u16 size, u32 dec_addr, u8 dec_len)
{
    u16 key_addr;
    u16 r_len;

    while (size) {
        r_len = (size > dec_len) ? dec_len : size;
        key_addr = (dec_addr >> 2)^key;
        doe(key_addr, buff, r_len, 0);
        buff += r_len;
        dec_addr += r_len;
        size -= r_len;
    }
    return dec_addr;
}

u32 packet_combined(u8 *packet, u8 num)
{
    u32 _packet = 0;
    _packet = (packet[num] | (packet[num + 1] << 8) | (packet[num + 2] << 16) | (packet[num + 3] << 24));
    return _packet;
}

void all_assemble_package_send_to_pc(u8 id, u8 sq, u8 *buf, u32 len)
{
    u8 *send_buf = NULL;
    u16 crc16_data;
    send_buf = (u8 *)my_malloc(TEMP_BUF_SIZE, MM_CFG);
    if (send_buf == NULL) {
        log_error("send_buf malloc err!");
        return;
    }

    send_buf[0] = 0x5A;
    send_buf[1] = 0xAA;
    send_buf[2] = 0xA5;
    send_buf[5] = 2 + len;/*L*/
    send_buf[6] = id;/*T*/
    send_buf[7] = sq;/*SQ*/
    memcpy(send_buf + 8, buf, len);
    crc16_data = CRC16(&send_buf[5], len + 3);
    send_buf[3] = crc16_data & 0xff;
    send_buf[4] = (crc16_data >> 8) & 0xff;

    cdc_write_data(0, send_buf, len + 8);
    my_free(send_buf);
}

void app_cfg_tool_event_handler(struct cfg_tool_event *cfg_tool_dev)
{
    u8 *buf = NULL;
    u32 send_len = 0, read_len, tmp_len;

    buf = (u8 *)my_malloc(TEMP_BUF_SIZE, MM_CFG);
    if (buf == NULL) {
        log_error("buf malloc err!");
        return;
    }

    memset(buf, 0, TEMP_BUF_SIZE);
    memcpy(buf + 1, cfg_tool_dev->packet, cfg_tool_dev->size);
    const struct tool_interface *p;
    list_for_each_tool_interface(p) {
        if (p->id == cfg_tool_dev->packet[1]) {
            p->tool_message_deal(buf + 2, cfg_tool_dev->size - 1);
            my_free(buf);
            return;
        }
    }

#if TCFG_CFG_TOOL_ENABLE

    memset(buf, 0, TEMP_BUF_SIZE);

    switch (cfg_tool_dev->event) {
    case ONLINE_SUB_OP_QUERY_FILE_SIZE://获取文件大小
        __this->r_file_size.file_id = packet_combined(cfg_tool_dev->packet, 7);
        log_info("event_ONLINE_SUB_OP_QUERY_FILE_SIZE! id: %d\n", __this->r_file_size.file_id);

        if (__this->r_file_size.file_id != CFG_EQ_FILEID) {
            goto _exit_;
        }
        __this->s_file_size.file_size = get_eq_file_len();
        send_len = sizeof(__this->s_file_size.file_size);//长度
        memcpy(buf, &(__this->s_file_size.file_size), 4);
        break;

    case ONLINE_SUB_OP_QUERY_FILE_CONTENT://读取文件内容
        __this->r_file_content.file_id = packet_combined(cfg_tool_dev->packet, 7);
        __this->r_file_content.offset = packet_combined(cfg_tool_dev->packet, 11);
        __this->r_file_content.size = packet_combined(cfg_tool_dev->packet, 15);
        log_info("event_ONLINE_SUB_OP_QUERY_FILE_CONTENT! file_id: %d, offset: 0x%x, size: %d\n",
                 __this->r_file_content.file_id,
                 __this->r_file_content.offset,
                 __this->r_file_content.size);

        if (__this->r_file_size.file_id != CFG_EQ_FILEID) {
            goto _exit_;
        }

        tmp_len = effect_get_cfg_file_data(__this->r_file_content.offset, buf, __this->r_file_content.size);
        if (tmp_len != __this->r_file_content.size) {
            goto _exit_;
        }
        send_len = __this->r_file_content.size;
        break;

    case ONLINE_SUB_OP_PREPARE_WRITE_FILE://准备写文件
        __this->r_prepare_write_file.file_id = packet_combined(cfg_tool_dev->packet, 7);
        __this->r_prepare_write_file.size = packet_combined(cfg_tool_dev->packet, 11);
        log_info("event_ONLINE_SUB_OP_PREPARE_WRITE_FILE! id: %d, size: %d\n",
                 __this->r_prepare_write_file.file_id,
                 __this->r_prepare_write_file.size);

        if (__this->r_prepare_write_file.file_id != CFG_EQ_FILEID) {
            goto _exit_;
        }
        __this->s_prepare_write_file.file_size = get_eq_file_len();
        __this->s_prepare_write_file.file_addr = 0;
        __this->s_prepare_write_file.earse_unit = 256;
        send_len = sizeof(__this->s_prepare_write_file);
        memcpy(buf, &(__this->s_prepare_write_file), send_len);
        break;

    case ONLINE_SUB_OP_READ_ADDR_RANGE://读取地址范围内容
        __this->r_read_addr_range.addr = packet_combined(cfg_tool_dev->packet, 7);
        __this->r_read_addr_range.size = packet_combined(cfg_tool_dev->packet, 11);
        log_info("event_ONLINE_SUB_OP_READ_ADDR_RANGE! addr: 0x%x, size: %d\n",
                 __this->r_read_addr_range.addr,
                 __this->r_read_addr_range.size);

        if (__this->r_prepare_write_file.file_id != CFG_EQ_FILEID) {
            goto _exit_;
        }

        if (__this->r_read_addr_range.addr < __this->s_prepare_write_file.file_size) {
            read_len = __this->s_prepare_write_file.file_size - __this->r_read_addr_range.addr;
            if (read_len > __this->r_read_addr_range.size) {
                read_len = __this->r_read_addr_range.size;
            }
            tmp_len = effect_get_cfg_file_data(__this->r_read_addr_range.addr, buf, read_len);
            if (tmp_len != read_len) {
                log_error("read data error!");
                goto _exit_;
            }
        }
        send_len = __this->r_read_addr_range.size;
        break;

    case ONLINE_SUB_OP_ERASE_ADDR_RANGE://擦除地址范围内容
        __this->r_erase_addr_range.addr = packet_combined(cfg_tool_dev->packet, 7);
        __this->r_erase_addr_range.size = packet_combined(cfg_tool_dev->packet, 11);
        log_info("event_ONLINE_SUB_OP_ERASE_ADDR_RANGE! addr: 0x%x, size: %d\n",
                 __this->r_erase_addr_range.addr,
                 __this->r_erase_addr_range.size);

        if (__this->write_file_ptr) {
            __this->write_file_ptr = my_free(__this->write_file_ptr);
        }
        if (__this->r_prepare_write_file.file_id != CFG_EQ_FILEID) {
            goto _exit_;
        }
        __this->write_file_ptr = my_malloc(__this->r_erase_addr_range.size, MM_CFG);
        if (__this->write_file_ptr == NULL) {
            goto _exit_;
        }

        memcpy(buf, ok_return, sizeof(ok_return));
        send_len = sizeof(ok_return);
        break;

    case ONLINE_SUB_OP_WRITE_ADDR_RANGE://写入地址范围内容
        __this->r_write_addr_range.addr = packet_combined(cfg_tool_dev->packet, 7);
        __this->r_write_addr_range.size = packet_combined(cfg_tool_dev->packet, 11);
        log_info("event_ONLINE_SUB_OP_WRITE_ADDR_RANGE! addr: 0x%x, size: %d\n",
                 __this->r_write_addr_range.addr,
                 __this->r_write_addr_range.size);

        if (__this->r_prepare_write_file.file_id != CFG_EQ_FILEID) {
            goto _exit_;
        }

        if (__this->write_file_ptr == NULL) {
            goto _exit_;
        }

        memcpy(__this->write_file_ptr + __this->r_write_addr_range.addr, cfg_tool_dev->packet + 15, __this->r_write_addr_range.size);
        if ((__this->r_write_addr_range.addr + __this->r_write_addr_range.size) >= __this->r_erase_addr_range.size) {
            log_info("write_file_ptr end!\n");
            tmp_len = write_eq_vm_data(__this->write_file_ptr, __this->s_prepare_write_file.file_size);//只写文件长度内容
            __this->write_file_ptr = my_free(__this->write_file_ptr);
            if (tmp_len != __this->s_prepare_write_file.file_size) {
                goto _exit_;
            }
        }

        memcpy(buf, ok_return, sizeof(ok_return));
        send_len = sizeof(fa_return);
        break;

    default:
        log_error("invalid data\n");
        memcpy(buf, er_return, sizeof(er_return));//不认识的命令返回ER
        send_len = sizeof(er_return);
        break;
_exit_:
        memcpy(buf, fa_return, sizeof(fa_return));//文件打开失败返回FA
        send_len = sizeof(fa_return);
        break;
    }

    all_assemble_package_send_to_pc(REPLY_STYLE, cfg_tool_dev->packet[2], buf, send_len);
#endif

    my_free(buf);
}

u8 online_cfg_tool_data_deal(void *buf, u32 len)
{
    u8 *data_buf = (u8 *)buf;
    u16 crc16_data;

    if ((data_buf[0] != 0x5a) || (data_buf[1] != 0xaa) || (data_buf[2] != 0xa5)) {
        log_error("Header check error, receive an invalid message!\n");
        return 1;
    }
    crc16_data = (data_buf[4] << 8) | data_buf[3];
    if (crc16_data != CRC16(data_buf + 5, len - 5)) {
        log_error("CRC16 check error, receive an invalid message!\n");
        return 1;
    }

    u32 cmd = packet_combined(data_buf, 8);
    switch (cmd) {
    case ONLINE_SUB_OP_QUERY_BASIC_INFO:
    case ONLINE_SUB_OP_ONLINE_INSPECTION:
    case ONLINE_SUB_OP_CPU_RESET:
    case ONLINE_SUB_OP_QUERY_FILE_SIZE:
    case ONLINE_SUB_OP_QUERY_FILE_CONTENT:
    case ONLINE_SUB_OP_PREPARE_WRITE_FILE:
    case ONLINE_SUB_OP_READ_ADDR_RANGE:
    case ONLINE_SUB_OP_ERASE_ADDR_RANGE:
    case ONLINE_SUB_OP_WRITE_ADDR_RANGE:
    case ONLINE_SUB_OP_ENTER_UPGRADE_MODE:
        break;
    default:
        cmd = DEFAULT_ACTION;
        break;
    }
    cfg_packet.event = cmd;
    cfg_packet.size = data_buf[5] + 1;
    if (cfg_packet.size > sizeof(local_packet)) {
        return 0;
    }
    cfg_packet.packet = local_packet;
    memcpy(cfg_packet.packet, &data_buf[5], cfg_packet.size);
    post_msg(1, MSG_CFG_RX_DATA);
    return 0;
}

int cfg_tool_rx_data(void)
{
    app_cfg_tool_event_handler(&cfg_packet);
    return 0;
}


#endif

