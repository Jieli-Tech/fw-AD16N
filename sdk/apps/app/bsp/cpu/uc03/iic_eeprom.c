#include "includes.h"
#include "uart.h"

#define LOG_TAG_CONST       EEPROM
#define LOG_TAG             "[eeprom]"
#include "log.h"

#define EEPROM_EN 0
#if EEPROM_EN

#if 1 //0:软件iic,  1:硬件iic
#define EEPROM_IIC_TYPE_SELECT 1  //1:硬件iic，0:软件iic
#define _IIC_USE_HW
#else
#define EEPROM_IIC_TYPE_SELECT 0  //1:硬件iic，0:软件iic
#endif
#include "iic_api.h"

#define IIC_SCL_IO IO_PORTA_04
#define IIC_SDA_IO IO_PORTA_07
/***************************eeprom 24c02*****************************/
#define EEPROM_RADDR        0xa1
#define EEPROM_WADDR        0xa0
#define DELAY_CNT           0

#define BYTE_READ   1//1:byte_read    or  0:buf_read
#define BYTE_WRITE  1//1:byte_write   or  0:buf_write
#define eeprom_putchar(x) //putchar(x)

#define iic_dev                             0   //软件IIC设备组别选择
extern void delay(unsigned int cnt);

void eeprom_init()
{
#if(EEPROM_IIC_TYPE_SELECT)
    log_info("-------hw_iic-------\n");
#else
    log_info("-------soft_iic-------\n");
#endif
    struct iic_master_config iic_config_test = {
        .role = IIC_MASTER,
        .scl_io = IIC_SCL_IO,
        .sda_io = IIC_SDA_IO,
        .io_mode = PORT_INPUT_PULLUP_10K,//上拉或浮空
        .hdrive = PORT_DRIVE_STRENGT_2p4mA,   //enum GPIO_HDRIVE 0:2.4MA, 1:8MA, 2:26.4MA, 3:40MA
        .master_frequency = 100000, //软件iic频率不准(hz)
        .io_filter = 1,  //软件无效
    };
    iic_init(iic_dev, &iic_config_test);
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
            eeprom_putchar('h');
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
            eeprom_putchar('i');
#if defined CONFIG_CPU_UC03 && (EEPROM_IIC_TYPE_SELECT)
            iic_stop(iic);
#endif
            ret = iic_tx_byte(iic, buf[offset + tx_len - 1]);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            iic_stop(iic);
            eeprom_putchar('j');
            delay(DELAY_CNT);
            break;
        }
        offset += tx_len;
    }
__exit:
    if (!ret) {
        log_error("byte write error! offset:%d", offset);
        iic_stop(iic);
#if defined CONFIG_CPU_UC03 && (EEPROM_IIC_TYPE_SELECT)
        iic_stop(iic);
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
            eeprom_putchar('h');
            ret = iic_write_buf(iic, buf + offset, tx_len);
            if (ret < tx_len - 1) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            eeprom_putchar('i');
            iic_stop(iic);
            delay(DELAY_CNT);
            break;
        }
        offset += tx_len;
        eeprom_putchar('j');
    }
__exit:
    if (offset < len) {
        log_error("buf write error! offset:%d", offset);
        iic_stop(iic);
#if defined CONFIG_CPU_UC03 && (EEPROM_IIC_TYPE_SELECT)
        iic_stop(iic);
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
        eeprom_putchar('k');
        for (i = 0; i < len - 1; i++) {
            buf[i] = iic_rx_byte(iic, 1);
            delay(DELAY_CNT);
        }
        eeprom_putchar('l');
#if defined CONFIG_CPU_UC03 && (EEPROM_IIC_TYPE_SELECT)
        iic_stop(iic);
#endif
        buf[len - 1] = iic_rx_byte(iic, 0);
        iic_stop(iic);
        delay(DELAY_CNT);
        eeprom_putchar('m');
        break;
    }
    if (!ret) {
        log_error("byte read error");
        iic_stop(iic);
#if defined CONFIG_CPU_UC03 && (EEPROM_IIC_TYPE_SELECT)
        iic_stop(iic);
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
        break;
    }
    if (!ret) {
        log_error("buf read error");
        iic_stop(iic);
#if defined CONFIG_CPU_UC03 && (EEPROM_IIC_TYPE_SELECT)
        iic_stop(iic);
        iic_rx_byte(iic, 0);
#endif
    }
#endif
}


#if 0
#define IIC_TRANCE_LEN 64
static u8 eeprom_rbuf[IIC_TRANCE_LEN], eeprom_wbuf[IIC_TRANCE_LEN];
void eeprom_test_main()
{
    int i = 0;
    u8 flag = 0;

    eeprom_init();
    for (i = 0; i < IIC_TRANCE_LEN; i++) {
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
        put_u8hex(eeprom_rbuf[i]);
        if (i % 16 == 15) {
            log_char('\n');
        }

    }
    log_char('\n');
    if (flag == 0) {
        log_info("eeprom read/write test pass\n");
    } else {
        log_error("eeprom read/write test fail\n");
    }
    iic_uninit(iic_dev);
}
#endif

#endif

