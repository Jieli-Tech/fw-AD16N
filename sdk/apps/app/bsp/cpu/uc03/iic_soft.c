#include "iic_soft.h"
#include "clock.h"

#define LOG_TAG_CONST       IIC
#define LOG_TAG             "[iic_soft]"
#include "log.h"

static u8 soft_iic_state[MAX_SOFT_IIC_NUM] = {0, 0, 0};
static struct iic_master_config soft_iic_cfg_cache[MAX_SOFT_IIC_NUM];

//input pull up
#define IIC_SCL_H(scl)    \
    gpio_set_mode(scl/16, BIT(scl%16), soft_iic_cfg_cache[iic].io_mode)

#define IIC_SCL_L(scl)    \
    gpio_set_mode(scl/16, BIT(scl%16), PORT_OUTPUT_LOW)


#define IIC_SDA_DIR(sda, val) \
    if (val){ \
		gpio_set_mode(IO_PORT_SPILT(sda), soft_iic_cfg_cache[iic].io_mode); \
	} else { \
		gpio_set_mode(IO_PORT_SPILT(sda), PORT_OUTPUT_LOW); \
	}

//input pull up
#define IIC_SDA_H(sda)    \
    gpio_set_mode(sda/16, BIT(sda%16), soft_iic_cfg_cache[iic].io_mode)

#define IIC_SDA_L(sda)    \
    gpio_set_mode(sda/16, BIT(sda%16), PORT_OUTPUT_LOW)

#define IIC_SDA_READ(sda) \
    gpio_read(sda)

#define soft_iic_delay(num) \
	delay_cnt= num; \
    while (delay_cnt--) { \
        asm("nop"); \
    }
static inline u32 iic_get_delay(soft_iic_dev iic)
{
    u32 hsb_clk = clk_get("sys");
    u32 delay_num = 0;
    return delay_num;
}

extern const struct iic_master_config soft_iic_cfg_const[MAX_SOFT_IIC_NUM];
struct iic_master_config *get_soft_iic_config(soft_iic_dev iic)
{
    return (struct iic_master_config *)&soft_iic_cfg_const[iic];
}
enum iic_state_enum soft_iic_init(soft_iic_dev iic, struct iic_master_config *i2c_config)
{
    if (i2c_config == NULL) {
        log_error("soft iic%d param error!\n", iic);
        return IIC_ERROR_PARAM_ERROR;
    }
    if (iic >= MAX_SOFT_IIC_NUM) {
        log_error("soft iic index:%d error!\n", iic);
        return IIC_ERROR_INDEX_ERROR;
    }
    if ((soft_iic_state[iic]&BIT(7)) != 0) {
        log_error("soft iic%d has been occupied!\n", iic);
        return IIC_ERROR_INIT_FAIL;
    }
    soft_iic_state[iic] = BIT(7);//init ok
    memcpy(&soft_iic_cfg_cache[iic], i2c_config, sizeof(struct iic_master_config));

    //freq:

    gpio_set_mode(IO_PORT_SPILT(i2c_config->scl_io), i2c_config->io_mode); //默认10k
    gpio_set_mode(IO_PORT_SPILT(i2c_config->sda_io), i2c_config->io_mode); //默认10k
    gpio_set_drive_strength(IO_PORT_SPILT(i2c_config->scl_io), i2c_config->hdrive);
    gpio_set_drive_strength(IO_PORT_SPILT(i2c_config->sda_io), i2c_config->hdrive);
    return IIC_OK;//ok
}

enum iic_state_enum soft_iic_uninit(soft_iic_dev iic)
{
    if (soft_iic_state[iic] == 0) {
        log_error("soft iic%d has been no init!\n", iic);
        return IIC_ERROR_NO_INIT;
    }
    soft_iic_state[iic] = 0;//no init

    gpio_set_mode(IO_PORT_SPILT(soft_iic_cfg_cache[iic].scl_io), PORT_HIGHZ);
    gpio_set_mode(IO_PORT_SPILT(soft_iic_cfg_cache[iic].sda_io), PORT_HIGHZ);
    gpio_set_drive_strength(IO_PORT_SPILT(soft_iic_cfg_cache[iic].scl_io), PORT_DRIVE_STRENGT_2p4mA);
    gpio_set_drive_strength(IO_PORT_SPILT(soft_iic_cfg_cache[iic].sda_io), PORT_DRIVE_STRENGT_2p4mA);
    return IIC_OK;//ok
}

enum iic_state_enum soft_iic_suspend(soft_iic_dev iic)
{
    if ((soft_iic_state[iic] & 0xc0) != 0x80) {
        log_error("soft iic%d is no init or suspend!\n", iic);
        return IIC_ERROR_SUSPEND_FAIL;
    }
    if ((soft_iic_state[iic] & 0x3f) != 0) {
        log_error("soft iic%d is busy!\n", iic);
        return IIC_ERROR_BUSY;
    }
    soft_iic_state[iic] |= BIT(6);//suspend ok
    gpio_set_mode(IO_PORT_SPILT(soft_iic_cfg_cache[iic].scl_io), PORT_HIGHZ);
    gpio_set_mode(IO_PORT_SPILT(soft_iic_cfg_cache[iic].sda_io), PORT_HIGHZ);
    gpio_set_drive_strength(IO_PORT_SPILT(soft_iic_cfg_cache[iic].scl_io), PORT_DRIVE_STRENGT_2p4mA);
    gpio_set_drive_strength(IO_PORT_SPILT(soft_iic_cfg_cache[iic].sda_io), PORT_DRIVE_STRENGT_2p4mA);
    return IIC_OK;//ok
}

enum iic_state_enum soft_iic_resume(soft_iic_dev iic)
{
    if ((soft_iic_state[iic] & 0xc0) != 0xc0) {
        log_error("soft iic%d is no init or no suspend!\n", iic);
        return IIC_ERROR_RESUME_FAIL;
    }
    soft_iic_state[iic] &= ~ BIT(6); //resume ok

    gpio_set_mode(IO_PORT_SPILT(soft_iic_cfg_cache[iic].scl_io), soft_iic_cfg_cache[iic].io_mode); //默认10k
    gpio_set_mode(IO_PORT_SPILT(soft_iic_cfg_cache[iic].sda_io), soft_iic_cfg_cache[iic].io_mode); //默认10k
    gpio_set_drive_strength(IO_PORT_SPILT(soft_iic_cfg_cache[iic].scl_io), soft_iic_cfg_cache[iic].hdrive);
    gpio_set_drive_strength(IO_PORT_SPILT(soft_iic_cfg_cache[iic].sda_io), soft_iic_cfg_cache[iic].hdrive);
    return IIC_OK;//ok
}

//return:0:error, 1:ok
enum iic_state_enum soft_iic_check_busy(soft_iic_dev iic)
{
    if (soft_iic_state[iic] & 0x0f) {
        return IIC_ERROR_BUSY;//error
    }
    soft_iic_state[iic]++;//busy
    return IIC_OK;//ok
}
void soft_iic_idle(soft_iic_dev iic)
{
    soft_iic_state[iic] &= 0xf0;//idle
}

void soft_iic_start(soft_iic_dev iic)
{
    u32 delay_cnt;
    u32 dly_t = iic_get_delay(iic);
    /* log_info("soft_iic_init hsb clock:%d, delay cnt:%d\n",clk_get("sys"),dly_t); */

    IIC_SDA_H(soft_iic_cfg_cache[iic].sda_io);
    soft_iic_delay(dly_t);

    IIC_SCL_H(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t * 2);

    IIC_SDA_L(soft_iic_cfg_cache[iic].sda_io);
    soft_iic_delay(dly_t);

    IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t);
}

void soft_iic_stop(soft_iic_dev iic)
{
    u32 delay_cnt;
    u32 dly_t = iic_get_delay(iic);
    IIC_SDA_L(soft_iic_cfg_cache[iic].sda_io);
    soft_iic_delay(dly_t);

    IIC_SCL_H(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t * 2);

    IIC_SDA_H(soft_iic_cfg_cache[iic].sda_io);
    soft_iic_delay(dly_t);
    soft_iic_idle(iic);
}

void soft_iic_reset(soft_iic_dev iic)//同iic_v2
{
    hw_iic_start(iic);
    hw_iic_stop(iic);
}

static u8 soft_iic_check_ack(soft_iic_dev iic)
{
    u8 ack;
    u32 delay_cnt;
    u32 dly_t = iic_get_delay(iic);

    IIC_SDA_DIR(soft_iic_cfg_cache[iic].sda_io, 1);
    IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t);

    IIC_SCL_H(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t);

    if (IIC_SDA_READ(soft_iic_cfg_cache[iic].sda_io) == 0) {
        ack = 1;
    } else {
        ack = 0;
    }
    soft_iic_delay(dly_t);

    IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t);
    IIC_SDA_DIR(soft_iic_cfg_cache[iic].sda_io, 0);
    IIC_SDA_L(soft_iic_cfg_cache[iic].sda_io);

    return ack;//1:有应答, 0:无
}

static void soft_iic_rx_ack(soft_iic_dev iic)
{
    u32 delay_cnt;
    u32 dly_t = iic_get_delay(iic);
    IIC_SDA_L(soft_iic_cfg_cache[iic].sda_io);
    soft_iic_delay(dly_t);

    IIC_SCL_H(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t * 2);

    IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t);
}

static void soft_iic_rx_nack(soft_iic_dev iic)
{
    u32 delay_cnt;
    u32 dly_t = iic_get_delay(iic);
    IIC_SDA_H(soft_iic_cfg_cache[iic].sda_io);
    soft_iic_delay(dly_t);

    IIC_SCL_H(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t * 2);

    IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
    soft_iic_delay(dly_t);
}

u8 soft_iic_tx_byte(soft_iic_dev iic, u8 byte)
{
    u32 delay_cnt;
    u32 dly_t = iic_get_delay(iic);
    u8 i;
    IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
    for (i = 0; i < 8; i++) {  //MSB FIRST
        if ((byte << i) & 0x80) {
            IIC_SDA_H(soft_iic_cfg_cache[iic].sda_io);
        } else {
            IIC_SDA_L(soft_iic_cfg_cache[iic].sda_io);
        }
        soft_iic_delay(dly_t);

        IIC_SCL_H(soft_iic_cfg_cache[iic].scl_io);
        soft_iic_delay(dly_t * 2);

        IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
        soft_iic_delay(dly_t);
    }
    return soft_iic_check_ack(iic);//1:有应答, 0:无
}

u8 soft_iic_rx_byte(soft_iic_dev iic, u8 ack)
{
    u32 delay_cnt;
    u32 dly_t = iic_get_delay(iic);
    u8 byte = 0, i;

    IIC_SDA_DIR(soft_iic_cfg_cache[iic].sda_io, 1);

    for (i = 0; i < 8; i++) {
        soft_iic_delay(dly_t);

        IIC_SCL_H(soft_iic_cfg_cache[iic].scl_io);
        soft_iic_delay(dly_t);

        byte = byte << 1;
        if (IIC_SDA_READ(soft_iic_cfg_cache[iic].sda_io)) {
            byte |= 1;
        }
        soft_iic_delay(dly_t);

        IIC_SCL_L(soft_iic_cfg_cache[iic].scl_io);
        soft_iic_delay(dly_t);
    }

    IIC_SDA_DIR(soft_iic_cfg_cache[iic].sda_io, 0);
    if (ack) {
        soft_iic_rx_ack(iic);
    } else {
        soft_iic_rx_nack(iic);
    }

    return byte;
}

//return: =len:ok
int soft_iic_read_buf(soft_iic_dev iic, void *buf, int len)
{
    int i = 0;

    if (!buf || !len) {
        return IIC_ERROR_PARAM_ERROR;
    }
    for (i = 0; i < len - 1; i++) {
        ((u8 *)buf)[i] = soft_iic_rx_byte(iic, 1);
    }
    ((u8 *)buf)[len - 1] = soft_iic_rx_byte(iic, 0);
    return len;
}

//return: =len:ok
int soft_iic_write_buf(soft_iic_dev iic, const void *buf, int len)
{
    int i;
    u8 ack;

    if (!buf || !len) {
        return IIC_ERROR_PARAM_ERROR;
    }
    for (i = 0; i < len; i++) {
        ack = soft_iic_tx_byte(iic, ((u8 *)buf)[i]);
        if (ack == 0) {
            break;
        }
    }
    return i;
}


