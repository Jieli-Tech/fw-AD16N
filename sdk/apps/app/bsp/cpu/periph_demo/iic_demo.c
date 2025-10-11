#include "clock.h"
#include "wdt.h"

#define LOG_TAG_CONST       IIC
#define LOG_TAG             "[iic_demo]"
#include "log.h"

#if 1 //0:软件iic,  1:硬件iic
#define _IIC_USE_HW
#endif
#include "iic_api.h"

/******************************iic master test*****************************/
#if 1
#define IIC_SCL_IO IO_PORTA_08
#define IIC_SDA_IO IO_PORTA_09
#define IIC_MASTER_INDEX HW_IIC_0
void iic_master_polling_test()
{
    struct iic_master_config iic_config_test = {
        .role = IIC_MASTER,
        .scl_io = IIC_SCL_IO,
        .sda_io = IIC_SDA_IO,
        .io_mode = PORT_INPUT_PULLUP_10K,//上拉或浮空
        .hdrive = PORT_DRIVE_STRENGT_2p4mA,   //enum GPIO_HDRIVE 0:2.4MA, 1:8MA, 2:26.4MA, 3:40MA
        .master_frequency = 100000, //软件iic频率不准(hz)
        .io_filter = 1,  //软件无效
    };
    //eeprom

#ifdef _IIC_USE_HW
    log_info("**********************hw iic%d test*************************\n", IIC_MASTER_INDEX);
    u8 eeprom_reg_addr = 66;
#else
    log_info("**********************soft iic test*************************\n");
    u8 eeprom_reg_addr = 64;
#endif
    enum iic_state_enum iic_init_state = iic_init(IIC_MASTER_INDEX, &iic_config_test);
    if (iic_init_state == IIC_OK) {
        log_info("iic%d master init ok", IIC_MASTER_INDEX);
    } else {
        log_error("iic%d master init fail", IIC_MASTER_INDEX);
    }

    u8 eeprom_wbuf[32], eeprom_rbuf[32];
    u8 eeprom_dev_addr = 0xa0;
    u8 eeprom_retry_cnt = 10;
    int eeprom_ret_len = 0;
    for (u8 i = 0; i < sizeof(eeprom_rbuf); i++) {
        eeprom_wbuf[i] = i % 26 + 'a';
        eeprom_rbuf[i] = 0;
    }
    eeprom_ret_len = i2c_master_read_nbytes_from_device_reg(IIC_MASTER_INDEX, eeprom_dev_addr, &eeprom_reg_addr, 1, eeprom_rbuf, 31);
    log_info("%s", eeprom_rbuf);
    memset(eeprom_rbuf, 0, 32);
    mdelay(20);
    eeprom_ret_len = 0;
    log_info(">>>> write in");
    while ((eeprom_ret_len != 8) && (--eeprom_retry_cnt)) {
        eeprom_ret_len = i2c_master_write_nbytes_to_device_reg(IIC_MASTER_INDEX, eeprom_dev_addr, &eeprom_reg_addr, 1, eeprom_wbuf, 8);
    }
    log_info("<<<< write out\n");
    eeprom_retry_cnt = 10;
    mdelay(20);
    eeprom_ret_len = 0;
    log_info(">>>> read in");
    while ((eeprom_ret_len != 31) && (--eeprom_retry_cnt)) {
        eeprom_ret_len = i2c_master_read_nbytes_from_device_reg(IIC_MASTER_INDEX, eeprom_dev_addr, &eeprom_reg_addr, 1, eeprom_rbuf, 31);
    }
    log_info("<<<< read out\n");
    iic_init_state = iic_uninit(IIC_MASTER_INDEX);
    if (iic_init_state == IIC_OK) {
        log_info("iic%d master uninit ok", IIC_MASTER_INDEX);
    } else {
        log_error("iic%d master uninit fail", IIC_MASTER_INDEX);
    }

    log_info_hexdump(eeprom_rbuf, 32);
    log_info("%s", eeprom_rbuf);
}
#endif




/******************************hw iic slave test*****************************/
#if 1
/* #define IIC_SCL_IO IO_PORTA_02 */
/* #define IIC_SDA_IO IO_PORTA_03 */
u8 slave_tx_buf_test[250];
u8 slave_rx_buf_test[255];
#define IIC_SLAVE_INDEX HW_IIC_0
//rx协议:start,addr write,data0,data1,,,,,,stop
//tx协议:start,addr read,data0,data1,,,,,nack,stop
AT(.iic.text.cache.L1)
void hw_iic_isr_callback()//slave test
{
    static int rx_state = 0, tx_cnt = 0, rx_cnt = 1;
    u8 iic = IIC_SLAVE_INDEX;
    if (rx_state == 0) {
        rx_state = hw_iic_slave_rx_byte(iic, &slave_rx_buf_test[0]);//addr
        if (rx_state == IIC_SLAVE_RX_ADDR_RX) { //rx
            hw_iic_slave_rx_prepare(iic, 1, 0);
        } else if (rx_state == IIC_SLAVE_RX_ADDR_TX) { //tx
            hw_iic_slave_tx_byte(iic, slave_tx_buf_test[tx_cnt++]);
        } else { //error
            log_error("iic slave rx addr error!\n");
            rx_state = 0;
        }
    }

    if (hw_iic_get_pnd(iic, I2C_PND_STOP)) {
        /* putchar('e'); */
        hw_iic_clr_all_pnd(iic);
        hw_iic_slave_rx_prepare(iic, 0, 0);
        if (rx_state >= IIC_SLAVE_RX_ADDR_RX) { //rx
            log_info_hexdump(slave_rx_buf_test, rx_cnt);
        }
        rx_state = 0;
        tx_cnt = 0;
        rx_cnt = 1;
    }

    if (hw_iic_get_pnd(iic, I2C_PND_TASK_DONE)) {
        if (rx_state == IIC_SLAVE_RX_ADDR_TX) { //tx
            if (hw_iic_slave_tx_check_ack(iic) == 0) { //no ack
                return;
            }
            hw_iic_slave_tx_byte(iic, slave_tx_buf_test[tx_cnt++]);
        } else if (rx_state >= IIC_SLAVE_RX_ADDR_RX) { //rx
            hw_iic_slave_rx_byte(iic, &slave_rx_buf_test[rx_cnt++]);
            hw_iic_slave_rx_prepare(iic, 1, 0);
        }
    }
}
void hw_iic_slave_polling_test()
{
    struct hw_iic_slave_config hw_iic_config_test = {
        .config.role = IIC_SLAVE,
        .config.scl_io = IIC_SCL_IO + IIC_SLAVE_INDEX,
        .config.sda_io = IIC_SDA_IO + IIC_SLAVE_INDEX,
        .config.io_mode = PORT_INPUT_PULLUP_10K,//上拉或浮空
        .config.hdrive = PORT_DRIVE_STRENGT_2p4mA,   //enum GPIO_HDRIVE 0:2.4MA, 1:8MA, 2:26.4MA, 3:40MA
        .config.ie_en = 0,
        .config.io_filter = 1,
        .slave_addr = 0x68,
        .iic_slave_irq_callback = NULL,
    };
    for (u16 i = 0; i < sizeof(slave_rx_buf_test); i++) {
        slave_rx_buf_test[i] = 0;
    }
    for (u8 i = 0; i < sizeof(slave_tx_buf_test); i++) {
        slave_tx_buf_test[i] = i;
    }

    //ie
#if 1//ie test
    hw_iic_config_test.config.ie_en = 1;
    hw_iic_config_test.iic_slave_irq_callback = hw_iic_isr_callback;
    enum iic_state_enum iic_init_state1 = hw_iic_slave_init(IIC_SLAVE_INDEX, &hw_iic_config_test);
    if (iic_init_state1 == IIC_OK) {
        log_info("iic%d_slave init ok", IIC_SLAVE_INDEX);
    }
    while (1) {
        wdt_clear();
        mdelay(1000);
    }
#endif

    //polling
    enum iic_state_enum iic_init_state = hw_iic_slave_init(IIC_SLAVE_INDEX, &hw_iic_config_test);
    if (iic_init_state == IIC_OK) {
        log_info("iic%d_slave init ok", IIC_SLAVE_INDEX);
    }
    /* JL_PORTB->DIR &= ~BIT(3);//test io */
    /* JL_PORTB->OUT &= ~BIT(3); */
#if 1//rx test

    while (1) {
        hw_iic_slave_polling_rx(IIC_SLAVE_INDEX, slave_rx_buf_test);
        wdt_clear();
    }
#else//tx test

    while (1) {
        log_info("------------iic%d slave polling tx test------------\n", IIC_SLAVE_INDEX);
        if (hw_iic_slave_polling_tx(IIC_SLAVE_INDEX, slave_tx_buf_test)) {
            log_info("iic%d slave tx ok\n", IIC_SLAVE_INDEX);
        } else {
            log_info("iic%d slave tx fail\n", IIC_SLAVE_INDEX);
        }
        wdt_clear();
    }
#endif
}
#endif

