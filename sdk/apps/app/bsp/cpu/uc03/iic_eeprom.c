#include "includes.h"
#include "iic_hw.h"
/* #include "iic_soft.h" */
#include "uart.h"

/* #define LOG_TAG_CONST       EEPROM */
#define LOG_TAG             "[eeprom]"
#include "log.h"

#define EEPROM_EN 0
#if EEPROM_EN

/***************************eeprom 24c02*****************************/
#define EEPROM_RADDR        0xa1
#define EEPROM_WADDR        0xa0
#define DELAY_CNT           0

#define EEPROM_IIC_TYPE_SELECT 1  //1:硬件iic，0:软件iic
#define BYTE_READ   1//1:byte_read    or  0:buf_read
#define BYTE_WRITE  1//1:byte_write   or  0:buf_write


#if(EEPROM_IIC_TYPE_SELECT)
#define iic_dev                             IIC_0 //硬件IIC设备组别选择
#define iic_init(iic)                       hw_iic_init(iic)
#define iic_uninit(iic)                     hw_iic_uninit(iic)
#define iic_start(iic)                      hw_iic_start(iic)
#define iic_stop(iic)                       hw_iic_stop(iic)
#define iic_tx_byte(iic, byte)              hw_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               hw_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         hw_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    hw_iic_suspend(iic)
#define iic_resume(iic)                     hw_iic_resume(iic)
#else
#define iic_dev                             0   //软件IIC设备组别选择
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_uninit(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    soft_iic_suspend(iic)
#define iic_resume(iic)                     soft_iic_resume(iic)
#endif

extern void delay(unsigned int cnt);

void eeprom_init()
{
#if(EEPROM_IIC_TYPE_SELECT)
    log_info("-------hw_iic-------\n");
#else
    log_info("-------soft_iic-------\n");
#endif
    iic_init(iic_dev);
}

void eeprom_write(int iic, u8 *buf, u32 addr, u32 len)
{
    int i;
    u32 retry;
    int ret;
    u32 tx_len;
    u32 offset;
#if (BYTE_WRITE)
    offset = 0;
    while (offset < len) {
        tx_len = len - offset > 8 ? 8 : len - offset;
        retry = 100;
        while (1) {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            delay(DELAY_CNT);
            ret = iic_tx_byte(iic, addr + offset);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            delay(DELAY_CNT);
            /* log_char('h'); */
            log_info("h");
            for (i = 0; i < tx_len - 1; i++) {
                ret = iic_tx_byte(iic, buf[offset + i]);
                if (!ret) {
                    if (--retry) {
                        continue;
                    } else {
                        goto __exit;
                    }
                }
                delay(DELAY_CNT);
            }
            log_info("i");
#if(EEPROM_IIC_TYPE_SELECT)
            iic_stop(iic);
            ret = iic_tx_byte(iic, buf[offset + tx_len - 1]);
#else
            ret = iic_tx_byte(iic, buf[offset + tx_len - 1]);
            iic_stop(iic);
#endif
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            log_info("j");
            /*iic_stop(iic);*/
            delay(DELAY_CNT);
            break;
        }
        offset += tx_len;
    }
__exit:
    if (!ret) {
        log_error("byte write error! offset:%d", offset);
        iic_stop(iic);
#if(EEPROM_IIC_TYPE_SELECT)
        iic_tx_byte(iic, 0);
#endif
    }
#else
    offset = 0;
    while (offset < len) {
        retry = 100;
        tx_len = len - offset > 8 ? 8 : len - offset;
        while (1) {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            iic_tx_byte(iic, addr + offset);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            log_info("h");
            ret = iic_write_buf(iic, buf + offset, tx_len - 1);
            if (ret < tx_len - 1) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            log_info("i");
#if(EEPROM_IIC_TYPE_SELECT)
            iic_stop(iic);
            ret = iic_tx_byte(iic, buf[offset + tx_len - 1]);
#else
            ret = iic_tx_byte(iic, buf[offset + tx_len - 1]);
            iic_stop(iic);
#endif
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            /* iic_stop(iic); */
            delay(DELAY_CNT);
            break;
        }
        offset += tx_len;
        log_info("j");
    }
__exit:
    if (offset < len) {
        log_error("buf write error! offset:%d", offset);
        iic_stop(iic);
#if(EEPROM_IIC_TYPE_SELECT)
        iic_tx_byte(iic, 0);
#endif
    }
#endif
}

void eeprom_read(int iic, u8 *buf, u32 addr, u32 len)
{
    int i = 0;
    u32 retry = 100;
    int ret;
#if (BYTE_READ)
    while (1) {
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_WADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        delay(DELAY_CNT);
        ret = iic_tx_byte(iic, addr);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        delay(DELAY_CNT);
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_RADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        delay(DELAY_CNT);
        log_info("k");
        for (i = 0; i < len - 1; i++) {
            buf[i] = iic_rx_byte(iic, 1);
            delay(DELAY_CNT);
        }
        log_info("l");
#if(EEPROM_IIC_TYPE_SELECT)
        iic_stop(iic);
        buf[len - 1] = iic_rx_byte(iic, 0);
#else
        buf[len - 1] = iic_rx_byte(iic, 0);
        iic_stop(iic);
#endif
        /*iic_stop(iic);*/
        delay(DELAY_CNT);
        log_info("m");
        break;
    }
    if (!ret) {
        log_error("byte read error");
        iic_stop(iic);
#if(EEPROM_IIC_TYPE_SELECT)
        iic_rx_byte(iic, 0);
#endif
    }
#else
    while (1) {
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_WADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_tx_byte(iic, addr);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_RADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_read_buf(iic, buf, len);
        iic_stop(iic);
#if(EEPROM_IIC_TYPE_SELECT)
        iic_rx_byte(iic, 0);
#endif
        break;
    }
    if (!ret) {
        log_error("buf read error");
        iic_stop(iic);
#if(EEPROM_IIC_TYPE_SELECT)
        iic_rx_byte(iic, 0);
#endif
    }
#endif
}


#if 0
#define IIC_TRANCE_LEN 128
static u8 eeprom_rbuf[512], eeprom_wbuf[512];
void eeprom_test_main()
{
    int i = 0;
    u8 flag = 0;

    eeprom_init();
    for (i = 0; i < 512; i++) {
        eeprom_wbuf[i] = i % 26 + 'A';
        eeprom_rbuf[i] = 0;
    }
    log_info(">>>> write start\n");
    eeprom_write(iic_dev, eeprom_wbuf, 0, IIC_TRANCE_LEN);//24c01只有128字节
    log_info("<<<< write end\n");
    delay(200000);
    log_info(">>>> read start\n");
    eeprom_read(iic_dev, eeprom_rbuf, 0, IIC_TRANCE_LEN);
    log_info("<<<< read end\n");

    for (i = 0; i < IIC_TRANCE_LEN; i++) {
        if (eeprom_wbuf[i] != eeprom_rbuf[i]) {
            flag = 1;
            break;
        }
    }
    log_info("read data:");
    /* log_info_hexdump(eeprom_rbuf,IIC_TRANCE_LEN); */
    for (i = 0; i < IIC_TRANCE_LEN; i++) {
        log_info("%x ", eeprom_rbuf[i]);
    }
    log_info("\n");
    if (flag == 0) {
        log_info("eeprom read/write test pass\n");
    } else {
        log_error("eeprom read/write test fail\n");
    }
    iic_uninit(iic_dev);
}
#endif

#endif

