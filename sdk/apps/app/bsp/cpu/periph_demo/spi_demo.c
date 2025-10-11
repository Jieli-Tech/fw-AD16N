#include "gpio.h"
#include "clock.h"
#include "wdt.h"
#include "spi.h"

#define LOG_TAG_CONST       SPI
#define LOG_TAG             "[spi_demo]"
#include "log.h"


/**********************spi master test********************/
#if 1
void spi_master_isr_callback_test(hw_spi_dev spi, enum hw_spi_isr_status sta)  //spi isr callback
{
    log_info("%s(),spi:%d,sta:%d", __func__, spi, sta);
}
void spi_master_test(hw_spi_dev spi)
{
    struct spi_platform_data spix_p_data_test = {
        .port = {
            IO_PORTA_08, //clk any io
            IO_PORTA_09, //do any io
            IO_PORTA_10, //di any io
            IO_PORTA_11, //d2 any io
            IO_PORTA_12, //d3 any io
            0xff, //cs any io(主机不操作cs)
        },
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE_BIDIR_1BIT,//SPI_MODE_UNIDIR_2BIT,//SPI_MODE_BIDIR_1BIT,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,//clk level in idle state:0:low,  1:high
        .cpha = 0,//sampling edge:0:first,  1:second
        .ie_en = 0, //ie enbale:0:disable,  1:enable
        .spi_isr_callback = NULL,  //spi isr callback
        .clk  = 1000000L,
    };

//init
    int ret = spi_open(spi, &spix_p_data_test);
    if (ret < 0) {
        log_error("spi master init error(%d)!", ret);
    }

//dma tx(阻塞等pnd)
    u8 spi_tx_buf_test[50];
    log_info("spi(%d) master block dma tx test:", spi);
    memset(spi_tx_buf_test, 0xa5, sizeof(spi_tx_buf_test));
    ret = spi_dma_send(spi, spi_tx_buf_test, 50);
    if (ret < 0) {
        log_error("spi master dma send error(%d)!", ret);
    }
    log_info("spi(%d) master block dma tx ok!\n", spi);
    spi_close(spi);



//init
    spix_p_data_test.ie_en = 1;
    spix_p_data_test.spi_isr_callback = spi_master_isr_callback_test;
    ret = spi_open(spi, &spix_p_data_test);
    if (ret < 0) {
        log_error("spi master init error(%d)!", ret);
    }

//dma tx(中断)
    memset(spi_tx_buf_test, 0x39, sizeof(spi_tx_buf_test));
    log_info("spi(%d) master irq dma tx test:", spi);
    spi_dma_transmit_for_isr(spi, spi_tx_buf_test, sizeof(spi_tx_buf_test), 0); //rw:1-rx; 0-tx
    while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
        wdt_clear();
    }
    log_info("spi(%d) master irq dma tx ok!(len:%d)\n", spi, hw_spix_get_isr_len(spi));


//dma rx(中断)
    log_info("spi(%d) master irq dma rx test:", spi);
    memset(spi_tx_buf_test, 0x39, sizeof(spi_tx_buf_test));
    spi_dma_transmit_for_isr(spi, spi_tx_buf_test, sizeof(spi_tx_buf_test), 1); //rw:1-rx; 0-tx
    while (hw_spix_get_isr_status(spi) != SPI_RX_FINISH) {
        wdt_clear();
    }
    log_info("spi(%d) master irq dma rx ok!len:%d\n", spi, hw_spix_get_isr_len(spi));
    log_info_hexdump(spi_tx_buf_test, sizeof(spi_tx_buf_test));


//byte 全双工(SPI_MODE_BIDIR_1BIT),中断
    log_info("spi(%d) master irq byte tx&rx test:", spi);
    hw_spix_irq_change_callback(spi, NULL);
    u8 spi_rx_buf_test[100];
    memset(spi_rx_buf_test, 0, sizeof(spi_rx_buf_test));
    memset(spi_tx_buf_test, 0xb6, sizeof(spi_tx_buf_test));
    hw_spix_clear_isr_len(spi);//clear len
#if 0 //byte 接口
    for (u16 i = 0; i < sizeof(spi_tx_buf_test); i++) {
        spi_byte_transmit_for_isr(spi, spi_tx_buf_test[i], spi_rx_buf_test + i, 0); //rw:全双工:0    SPI_TX_FINISH
        /* spi_byte_transmit_for_isr(spi, spi_tx_buf_test[i], NULL, 0); //rw:tx:0    SPI_TX_FINISH */
        /* spi_byte_transmit_for_isr(spi, NULL, spi_rx_buf_test+ i, 1); //rw:rx:1    SPI_RX_FINISH */
        while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
            wdt_clear();
        }
    }
    log_info("spi(%d) master byte rx: (len:%d)", spi, hw_spix_get_isr_len(spi));
#else //buf 接口

    spi_buf_transmit_for_isr(spi, spi_tx_buf_test, spi_rx_buf_test, sizeof(spi_tx_buf_test), 0);//rw:1-rx; 0-tx,全双工
    /* spi_buf_transmit_for_isr(spi, spi_tx_buf_test, NULL, sizeof(spi_tx_buf_test), 0);//rw:1-rx; 0-tx,全双工 */
    while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
        wdt_clear();
    }
    log_info("spi(%d) master buf rx: (len:%d)", spi, hw_spix_get_isr_len(spi));
#endif
    log_info("spi(%d) master rx: (len:%d)", spi, hw_spix_get_isr_len(spi));
    log_info_hexdump(spi_rx_buf_test, sizeof(spi_rx_buf_test));

    log_info("spi(%d) master irq byte tx&rx ok\n", spi);
    spi_close(spi);

    while (1) {
        wdt_clear();
    }
}

#endif




#if 1
/***********************slave test 无cs**************************/
void spi_slave_isr_callback_test1(hw_spi_dev spi, enum hw_spi_isr_status sta)  //spi isr callback
{
    log_info("%s(),spi:%d,sta:%d", __func__, spi, sta);
}
//固定256长
void spi_slave_test1(hw_spi_dev spi)
{
    struct spi_platform_data spix_p_data_test = {
        .port = {
            IO_PORTA_08, //clk any io
            IO_PORTA_09, //do any io
            IO_PORTA_10, //di any io
            IO_PORTA_11, //d2 any io
            IO_PORTA_12, //d3 any io
            IO_PORTA_13, //cs any io
        },
        .role = SPI_ROLE_SLAVE,
        .mode = SPI_MODE_UNIDIR_2BIT,//SPI_MODE_BIDIR_1BIT,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,//clk level in idle state:0:low,  1:high
        .cpha = 0,//sampling edge:0:first,  1:second
        .ie_en = 1, //ie enbale:0:disable,  1:enable
        .spi_isr_callback = NULL,  //spi isr callback
        .clk  = 1000000L,
    };

//init
    int ret = spi_open(spi, &spix_p_data_test);
    if (ret < 0) {
        log_error("spi slave init error(%d)!", ret);
    }


    u16 i = 0;
    u8 spi_tx_buf_test[256];
    u8 spi_rx_buf_test[256];
    memset(spi_rx_buf_test, 0, sizeof(spi_rx_buf_test));
#if 0
//dma rx
    hw_spix_irq_change_callback(spi, spi_slave_isr_callback_test1);
    log_info("spi(%d) slave dma rx test:", spi);
    while (1) { //rx
        spi_dma_transmit_for_isr(spi, spi_rx_buf_test, sizeof(spi_rx_buf_test), 1); //rw:1-rx; 0-tx
        while (hw_spix_get_isr_status(spi) != SPI_RX_FINISH) {
            wdt_clear();
        }
        log_info("spi(%d) slave dma rx:(len:%d)", spi, hw_spix_get_isr_len(spi));
        log_info_hexdump(spi_rx_buf_test, sizeof(spi_rx_buf_test));
        /* mdelay(500); */
        wdt_clear();
    }
#elif 1
//byte rx
#if 0 //byte 接口

    log_info("spi(%d) slave byte rx test:", spi);
    while (1) { //rx
        hw_spix_clear_isr_len(spi);//clear len
        for (i = 0; i < sizeof(spi_rx_buf_test); i++) {
            spi_byte_transmit_for_isr(spi, 0, spi_rx_buf_test + i, 1); //rw:1-rx; 0-tx
            while (hw_spix_get_isr_status(spi) != SPI_RX_FINISH) {
                wdt_clear();
            }
        }
        log_info("spi(%d) slave byte rx:(len:%d)", spi, hw_spix_get_isr_len(spi));
        log_info_hexdump(spi_rx_buf_test, sizeof(spi_rx_buf_test));
    }
#else //buf 接口

    log_info("spi(%d) slave buf rx test:", spi);
    for (i = 0; i < sizeof(spi_tx_buf_test); i++) {
        spi_tx_buf_test[i] = i;
    }
    while (1) { //tx
        hw_spix_clear_isr_len(spi);//clear len
        spi_buf_transmit_for_isr(spi, NULL, spi_rx_buf_test, sizeof(spi_rx_buf_test), 1);//rw:1-rx; 0-tx,全双工
        while (hw_spix_get_isr_status(spi) != SPI_RX_FINISH) {
            wdt_clear();
        }
        log_info("spi(%d) slave buf rx:(len:%d)", spi, hw_spix_get_isr_len(spi));
        log_info_hexdump(spi_rx_buf_test, sizeof(spi_rx_buf_test));
    }
#endif
#endif
#if 0
//dma tx
    hw_spix_irq_change_callback(spi, spi_slave_isr_callback_test1);
    log_info("spi(%d) slave dma tx test:", spi);
    for (i = 0; i < sizeof(spi_tx_buf_test); i++) {
        spi_tx_buf_test[i] = i;
    }
    while (1) { //tx
        spi_dma_transmit_for_isr(spi, spi_tx_buf_test, sizeof(spi_tx_buf_test), 0); //rw:1-rx; 0-tx
        while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
            wdt_clear();
        }
        log_info("spi(%d) slave dma tx ok!(len:%d)", spi, hw_spix_get_isr_len(spi));
    }
#else

#if 0 //byte 接口
    //byte tx
    log_info("spi(%d) slave byte tx test:", spi);
    for (i = 0; i < sizeof(spi_tx_buf_test); i++) {
        spi_tx_buf_test[i] = i;
    }
    while (1) { //tx
        hw_spix_clear_isr_len(spi);//clear len
        for (i = 0; i < sizeof(spi_tx_buf_test); i++) {
            spi_byte_transmit_for_isr(spi, spi_tx_buf_test[i], NULL, 0);//rw:1-rx; 0-tx
            while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
                wdt_clear();
            }
        }
        log_info("spi(%d) slave byte tx ok!(len:%d)", spi, hw_spix_get_isr_len(spi));
    }
#else //buf 接口

    log_info("spi(%d) slave buf tx test:", spi);
    memset(spi_rx_buf_test, 0, sizeof(spi_rx_buf_test));
    for (i = 0; i < sizeof(spi_tx_buf_test); i++) {
        spi_tx_buf_test[i] = i;
    }
    while (1) { //tx
        hw_spix_clear_isr_len(spi);//clear len
        /* spi_buf_transmit_for_isr(spi, spi_tx_buf_test, spi_rx_buf_test, sizeof(spi_tx_buf_test), 0);//rw:1-rx; 0-tx,全双工 */
        spi_buf_transmit_for_isr(spi, spi_tx_buf_test, NULL, sizeof(spi_tx_buf_test), 0);//rw:1-rx; 0-tx,全双工
        while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
            wdt_clear();
        }
        log_info("spi(%d) slave buf tx ok!(len:%d)", spi, hw_spix_get_isr_len(spi));
    }
#endif
#endif

}










/***********************slave test cs**************************/
//cs需双边沿,spi驱动不支持cs模式
void spi_slave_isr_callback_test2(hw_spi_dev spi, enum hw_spi_isr_status sta)  //spi isr callback
{
    log_info("%s(),spi:%d,sta:%d", __func__, spi, sta);
}
static volatile u8 spi_finish_flag = 0;
static int spi_s_dma_data_len_test = 0;
void gpio_irq_callback_p_test(enum gpio_port port, u32 pin, enum gpio_irq_edge edge)
{
    if (edge == PORT_IRQ_EDGE_FALL) {
        spi_finish_flag = 0;
        spi_s_dma_data_len_test = 0;
    } else if (edge == PORT_IRQ_EDGE_RISE) {
        spi_finish_flag = 1;
        spi_s_dma_data_len_test = -hw_spix_slave_get_dma_len(1);
    }
    /* log_info("port%d.%d:%d-cb1", port, pin, edge); */
    /* log_info("flag:%d,len:%d", spi_finish_flag, spi_s_dma_data_len_test); */
}
void spi_slave_cs_test(hw_spi_dev spi)
{
    struct spi_platform_data spix_p_data_test = {
        .port = {
            IO_PORTA_08, //clk any io
            IO_PORTA_09, //do any io
            IO_PORTA_10, //di any io
            IO_PORTA_11, //d2 any io
            IO_PORTA_12, //d3 any io
            IO_PORTA_13, //cs any io
        },
        .role = SPI_ROLE_SLAVE,
        .mode = SPI_MODE_UNIDIR_2BIT,//SPI_MODE_BIDIR_1BIT,
        .bit_mode = SPI_FIRST_BIT_MSB,
        .cpol = 0,//clk level in idle state:0:low,  1:high
        .cpha = 0,//sampling edge:0:first,  1:second
        .ie_en = 0, //ie enbale:0:disable,  1:enable
        .spi_isr_callback = NULL,  //spi isr callback
        .clk  = 1000000L,
    };

//init cs(cs 中断)
    struct gpio_irq_config_st gpio_irq_config_test = {
        .pin = PORT_PIN_6,
        .irq_edge = PORT_IRQ_ANYEDGE,//双边沿
        .callback = gpio_irq_callback_p_test,
    };
    gpio_irq_config(PORTA, &gpio_irq_config_test);//通信结束

//init spi
    int ret = spi_open(spi, &spix_p_data_test);
    if (ret < 0) {
        log_error("spi slave init error(%d)!", ret);
    }
    u16 i = 0;
    u8 spi_tx_buf_test[256 * 2];
    u8 spi_rx_buf_test[256 * 2];
    memset(spi_rx_buf_test, 0, sizeof(spi_rx_buf_test));

//dma 只需io双边沿中断
#if 0
//dma rx, len必大于主机通信长
    log_info("spi(%d) slave dma rx test:", spi);
    while (1) { //rx
        spi_dma_transmit_for_isr(spi, spi_rx_buf_test, 1000000, 1); //rw:1-rx; 0-tx
        log_info("spi_cnt:%d", spi_r_reg_dma_cnt(spi_regs[1]));
        while (spi_finish_flag == 0) {
            wdt_clear();
        }
        spi_finish_flag = 0;
        log_info("spi(%d) slave dma rx:(len:%d)", spi, spi_s_dma_data_len_test);
        log_info_hexdump(spi_rx_buf_test, spi_s_dma_data_len_test);
        /* mdelay(500); */
        wdt_clear();
    }
#elif 0
//dma tx, len必大于主机通信长
    log_info("spi(%d) slave dma tx test:", spi);
    for (i = 0; i < sizeof(spi_tx_buf_test); i++) {
        spi_tx_buf_test[i] = i;
    }
    while (1) { //tx
        spi_dma_transmit_for_isr(spi, spi_tx_buf_test, 1000000, 0); //rw:1-rx; 0-tx
        log_info("spi_cnt:%d", spi_r_reg_dma_cnt(spi_regs[1]));
        while (spi_finish_flag == 0) {
            wdt_clear();
        }
        spi_finish_flag = 0;
        log_info("spi(%d) slave dma tx ok!(len:%d)", spi, spi_s_dma_data_len_test);
    }
#endif
    spi_close(spi);



//init
    spix_p_data_test.ie_en = 1;
    spix_p_data_test.mode = SPI_MODE_BIDIR_1BIT;
    spix_p_data_test.spi_isr_callback = NULL;
    ret = spi_open(spi, &spix_p_data_test);
    if (ret < 0) {
        log_error("spi slave init error(%d)!", ret);
    }
    spi_finish_flag = 0;

//byte 需io双边沿中断及spi中断
#if 0
//byte rx
    u8 j = 0;
    memset(spi_rx_buf_test, 0, sizeof(spi_rx_buf_test));
    while (1) { //rx
        hw_spix_clear_isr_len(spi);//clear len
#if 0 //byte 接口(每次配置spi)

        log_info("spi(%d) slave byte rx test:", spi);
        j = 0;
        while (1) {
            spi_byte_transmit_for_isr(spi, 0, spi_rx_buf_test + j, 1); //rw:1-rx; 0-tx
            while (hw_spix_get_isr_status(spi) != SPI_RX_FINISH) {
                wdt_clear();
                if (spi_finish_flag) {
                    break;
                }
            }
            j++;
            if (spi_finish_flag) {
                break;
            }
        }
        log_info("spi(%d) slave byte rx:(len:%d)", spi, hw_spix_get_isr_len(spi));
#else //buf 接口(配置一次spi, len必大于主机通信长)

        log_info("spi(%d) slave buf rx test:", spi);
        spi_buf_transmit_for_isr(spi, NULL, spi_rx_buf_test, 1000000, 1);//rw:1-rx; 0-tx,全双工
        while (hw_spix_get_isr_status(spi) != SPI_RX_FINISH) {
            wdt_clear();
            if (spi_finish_flag) {
                break;
            }
        }
        log_info("spi(%d) slave buf rx:(len:%d)", spi, hw_spix_get_isr_len(spi));
#endif
        spi_finish_flag = 0;
        log_info_hexdump(spi_rx_buf_test, hw_spix_get_isr_len(spi));
    }
#else
//byte 全双工 tx&rx
    for (i = 0; i < sizeof(spi_tx_buf_test); i++) {
        spi_tx_buf_test[i] = i;
    }
    u8 j = 0;
    memset(spi_rx_buf_test, 0, sizeof(spi_rx_buf_test));
    while (1) { //tx
        hw_spix_clear_isr_len(spi);//clear len
#if 0 //byte 接口(每次配置spi)

        j = 0;
        log_info("spi(%d) slave byte tx&rx test:", spi);
        while (1) {
            spi_byte_transmit_for_isr(spi, spi_tx_buf_test[j], spi_rx_buf_test + j, 0); //rw:1-rx; 0-tx
            while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
                wdt_clear();
                if (spi_finish_flag) {
                    break;
                }
            }
            j++;
            if (spi_finish_flag) {
                break;
            }
        }
        log_info("spi(%d) slave byte tx&rx ok!(len:%d)", spi, hw_spix_get_isr_len(spi));
#else //buf 接口(配置一次spi, len必大于主机通信长)

        log_info("spi(%d) slave buf tx&rx test:", spi);
        spi_buf_transmit_for_isr(spi, spi_tx_buf_test, spi_rx_buf_test, 1000000, 0);//rw:1-rx; 0-tx,全双工
        while (hw_spix_get_isr_status(spi) != SPI_TX_FINISH) {
            wdt_clear();
            if (spi_finish_flag) {
                break;
            }
        }
        log_info("spi(%d) slave buf tx&rx ok!(len:%d)", spi, hw_spix_get_isr_len(spi));
#endif
        spi_finish_flag = 0;
        log_info_hexdump(spi_rx_buf_test, hw_spix_get_isr_len(spi));
    }
#endif
}

#endif

