#include "cpu.h"
#include "mio_api.h"
#include "my_malloc.h"
#include "vfs.h"
#include "decoder_api.h"
#include "app_config.h"
#include "audio_dac_api.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "log.h"

sound_mio_obj *g_mio_obj[2];

void mio_module_init(void)
{
    memset((void *)&g_mio_obj[0], 0, sizeof(g_mio_obj));
}
void mio_start(void *mio_obj)
{
    if (NULL == mio_obj) {
        return;
    }
    sound_mio_obj *obj = mio_obj;
    if (obj->status & B_MIO_EN) {
        obj->status |= B_MIO_START;
    }
}
#define MIO_WORKING (B_MIO_EN | B_MIO_START)
AT(.audio_d.text.cache.L2)
void mio_kick(void *mio_obj, u32 dac_packt)
{
    if (NULL == mio_obj) {
        return;
    }
    sound_mio_obj *obj = mio_obj;
    if (MIO_WORKING != (obj->status & (MIO_WORKING | B_MIO_ERR))) {
        return;
    }
    obj->dac_cnt += dac_packt;
    obj->status |= B_MIO_KICK;
    kick_decoder();
}




#define MIO_ACTIVE (B_MIO_EN | B_MIO_KICK | B_MIO_START)

static bool mio_run_one(void *mio_obj)
{
    if (NULL == mio_obj) {
        return false;
    }
    sound_mio_obj *obj = mio_obj;
    if (MIO_ACTIVE != (obj->status & (MIO_ACTIVE | B_MIO_ERR))) {
        return false;
    }
    /* if ((obj->status & B_MIO_ERR)) */
    /* { */
    /* return false; */
    /* } */
#if 1
    u64 tmp_cnt = obj->dac_cnt;
    tmp_cnt = tmp_cnt * (32000 / 100) / (dac_sr_read() / 100);
    u32 tmp_dac_cnt = tmp_cnt;
    tmp_dac_cnt -= obj->dac_used_cnt;
    /* log_info("t_cnt:%d used_cnt:%d", tmp_dac_cnt, obj->dac_used_cnt); */
    if (tmp_dac_cnt < obj->dac_step) {
        obj->status &= ~B_MIO_KICK;
        return false;
    }
    local_irq_disable();
    obj->dac_used_cnt += obj->dac_step;
    local_irq_enable();
#else
    if (obj->dac_cnt < obj->dac_step) {
        obj->status &= ~B_MIO_KICK;
        return false;
    }
    local_irq_disable();
    obj->dac_cnt -= obj->dac_step;
    local_irq_enable();
#endif
    u32 size = obj->read(obj->pfile, obj->r_buf, obj->r_size);
    if (size != obj->r_size) {
        obj->status |= B_MIO_ERR;
        return false;
    }
    u32 i = 0;
    if (0 != obj->info.pwm_total) {
        for (i = 0; i < obj->info.pwm_total; i++) {
            obj->pwm_run(i, obj->r_buf[i]);
        }
    }
    if (0 != obj->info.io_total) {
        obj->io_run(obj->io_mask, obj->r_buf[i] | (obj->r_buf[i + 1] << 8));
    }
    return true;
}
#define MAX_MIO_CHANNEL  (sizeof(g_mio_obj)/sizeof(g_mio_obj[0]))

void mio_run(void)
{
    u32 i;
    for (i = 0; i < MAX_MIO_CHANNEL; i++) {
        while (true == mio_run_one(g_mio_obj[i]));
    }
}

static sound_mio_obj *regist_mio_channel(sound_mio_obj *obj)
{
    u32 i;
    void *pres = NULL;
    for (i = 0; i < MAX_MIO_CHANNEL; i++) {
        if (NULL == g_mio_obj[i]) {
            break;
        }
    }
    if (i >= MAX_MIO_CHANNEL) {
        return NULL;
    }
    if (NULL == obj) {
        obj =  my_malloc(sizeof(sound_mio_obj), MM_MIO);
        local_irq_disable();
        g_mio_obj[i] =  obj;
        memset(obj, 0, sizeof(sound_mio_obj));
        local_irq_enable();
        return g_mio_obj[i];
    } else {
        return obj;
    }
}
static sound_mio_obj *unregist_mio_channel(sound_mio_obj *obj)
{
    u32 i;
    for (i = 0; i < MAX_MIO_CHANNEL; i++) {
        if (obj == g_mio_obj[i]) {
            break;
        }
    }
    if (i >= MAX_MIO_CHANNEL) {
        return obj;
    }

    local_irq_disable();
    g_mio_obj[i] = NULL;
    local_irq_enable();
    return my_free(obj);
}

static bool mio_port_init(sound_mio_obj *obj)
{
    if (NULL == obj) {
        return false;
    }
    u32 i = 0;
    if (0 != obj->info.pwm_total) {
        for (i = 0; i < obj->info.pwm_total; i++) {
            obj->pwm_init(i);
        }
    }
    if (0 != obj->info.io_total) {
        for (u32 i = 0; i < obj->info.io_total; i++) {
            obj->io_mask <<= 1;
            obj->io_mask++;
        }
        obj->io_init(obj->io_mask);
    }
    return true;
}

u32 mio_check(sound_mio_obj *obj)
{
    if (MIO_LOGO != obj->info.logo) {
        return E_MIO_LOGO;
    }
    if (MIO_VER_V1_1 != obj->info.version) {
        return E_MIO_VER;
    }
    if ((obj->info.pwm_total > MIO_MAX_CHL_PWM) || (obj->info.io_total > MIO_MAX_CHL_IO) || (obj->info.pwm_total + obj->info.io_total) == 0) {
        return E_MIO_CHL;
    }

    obj->r_size = obj->info.pwm_total + (obj->info.io_total + 7) / 8;
    if (0 == obj->info.level) {
        log_info("mio level is 0 err!\n");
        return E_MIO_LEVEL;
    }
    obj->dac_step = 32 * obj->info.level;
    log_info("mio_check ok\n");
    return 0;
}

bool mio_open(void **pp_obj, void *pfile, void *func)
{
    if ((NULL == pfile) || (NULL == func)) {
        return false;
    }
    void(*hook)(sound_mio_obj *);
    hook = func;
    *pp_obj = regist_mio_channel(*pp_obj);
    if (NULL != *pp_obj) {
        sound_mio_obj *obj;
        obj = *pp_obj;
        hook(obj);

        obj->pfile = pfile;
        vfs_seek(obj->pfile, 0, SEEK_SET);
        if (sizeof(struct mio_info) != vfs_read(obj->pfile, &obj->info, sizeof(struct mio_info))) {
            /* return E_MIO_READ; */
            return false;
        }

        mio_port_init(obj);

        u32 err = mio_check(obj);
        if (err != 0) {
            mio_close((void **)&obj);
            log_info("mio_check err : 0x%x\n", err);
            return false;
        }
        obj->status = B_MIO_EN;
        return true;
    }
    return false;
}

void mio_close(void **pp_obj)
{
    sound_mio_obj *obj = *pp_obj;
    if (NULL != obj) {
        obj->status = 0;
        mio_port_init(obj);
        vfs_file_close(&obj->pfile);

        *pp_obj = unregist_mio_channel(*pp_obj);
    }
}




