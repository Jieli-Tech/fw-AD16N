#include "includes.h"
#include "uart_v2.h"
#include "gpio.h"

#define LOG_TAG_CONST   UART
#define LOG_TAG         "[uart_demo]"
#include "log.h"

static void uart_irq_func(int uart_num, enum uart_event event)
{
    if (event & UART_EVENT_TX_DONE) {
        log_info("uart[%d] tx done", uart_num);
    }

    if (event & UART_EVENT_RX_TIMEOUT) {
        log_info("uart[%d] rx timerout data", uart_num);
    }

    if (event & UART_EVENT_RX_FIFO_OVF) {
        log_info("uart[%d] rx fifo ovf", uart_num);
    }
}

u8 uart_rx_ptr[128];
u8 frame_ptr[64];


void uart_sync_demo_start(void)
{
    const struct uart_config config = {
        .baud_rate = 1000000,
        .tx_pin = IO_PORTA_03,
        .rx_pin = IO_PORTA_04,
        .parity = UART_PARITY_DISABLE,
        .tx_wait_mutex = 0,//1:不支持中断调用,互斥,0:支持中断,不互斥
    };

    log_info("uart_rx_ptr:%d", uart_rx_ptr);
    log_info("frame_ptr:%d", frame_ptr);

    const struct uart_dma_config dma = {
        .rx_timeout_thresh = 100,
        .event_mask = UART_EVENT_TX_DONE | UART_EVENT_RX_FIFO_OVF | UART_EVENT_RX_TIMEOUT,
        .irq_priority = 3,
        .irq_callback = uart_irq_func,
        .rx_cbuffer = uart_rx_ptr,
        .rx_cbuffer_size = 128,
        .frame_size = 128,//=rx_cbuffer_size
    };

    uart_dev uart_id = 1;
    int ut = uart_init(uart_id, &config);
    if (ut < 0) {
        log_error("init error %d", ut);
    } else {
        log_info("init ok %d", ut);
    }
    int r = uart_dma_init(uart_id, &dma);
    if (r < 0) {
        log_error("dma init error %d", ut);
    } else {
        log_info("dma init ok %d", ut);
    }
    uart_dump();
    while (1) {
        r = uart_recv_blocking(uart_id, frame_ptr, 64, 10);
        if (r > 0) { //ok
            log_info("r:%d\n", r);
            log_info_hexdump((u8 *)frame_ptr, r);
        }
        r = uart_send_blocking(uart_id, frame_ptr, r, 20);
        wdt_clear();
    }
    uart_deinit(uart_id);
}
