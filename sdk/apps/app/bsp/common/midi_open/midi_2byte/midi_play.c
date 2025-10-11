#include "midi_dec.h"

typedef struct  _EVENT_FIFO_CONTEXT_ {
    MIDI_CTRL_PARM  player_control;
} EVENT_FIFO_CONTEXT;


static u32 needMidiPlayersBuf()
{
    return MAX_CTR_PLAYER_CNT * sizeof(MIDI_PLAYER);
}

static u32 needPlayKeyBuf()
{
    return MAX_CTR_PLAYER_CNT * MAX_CHANNEL_NUM * sizeof(u8);
}

static u32 MIDI_CTRL_OPEN(void *work_buf, void *dec_parm, void *parm)
{
    unsigned int chn;
    long long tmp64;
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    MIDI_CONFIG_PARM *midi_param = (MIDI_CONFIG_PARM *)parm;
    MIDI_CTRL_PARM *midi_dec_param = (MIDI_CTRL_PARM *)dec_parm;

    EVENT_FIFO_CONTEXT *midi_fifo_t = (EVENT_FIFO_CONTEXT *)(&mid_dec_obj->smf_data);

    memset(work_buf, 0, sizeof(MIDI_DECODE_VAR));

    memcpy(midi_fifo_t, midi_dec_param, sizeof(MIDI_CTRL_PARM));

    u16 *cmporkind;
    cmporkind = (u16 *)midi_param->spi_pos;
    if (cmporkind[0] == 0xABCD) {
        mid_dec_obj->midi_spi_pos = (u8 *)&cmporkind[1];
        mid_dec_obj->compressIN = 1;
        mid_dec_obj->instr_spi = cmporkind[1];
        mid_dec_obj->instr_map = (unsigned short *)&cmporkind[130];
    } else {
        mid_dec_obj->midi_spi_pos = (u8 *)midi_param->spi_pos;
        mid_dec_obj->compressIN = 0;
        mid_dec_obj->instr_spi = cmporkind[0];
        mid_dec_obj->instr_map = (unsigned short *)&cmporkind[129];
    }

    mid_dec_obj->sample_rate = midi_param->sample_rate;
    mid_dec_obj->MAX_PLAYER_CNTt = midi_param->player_t;
    if (mid_dec_obj->MAX_PLAYER_CNTt > MAX_CTR_PLAYER_CNT) {
        mid_dec_obj->MAX_PLAYER_CNTt = MAX_CTR_PLAYER_CNT;
    }

    mid_dec_obj->midi_tempo_v = 1024;
    for (int i = 0; i < CTRL_CHANNEL_NUM; i++) {
        mid_dec_obj->decay_speed[i] = 32768;
    }
    mid_dec_obj->mute_threshold = 1L << 29;


    mid_dec_obj->spi_key_start = (unsigned char *)(&mid_dec_obj->midi_spi_pos[258 + mid_dec_obj->instr_spi * 2]);
    mid_dec_obj->spi_zone_start = mid_dec_obj->spi_key_start + 128 * mid_dec_obj->instr_spi;
    mid_dec_obj->instr_key_map[0] = (char *)mid_dec_obj->spi_key_start;
    mid_dec_obj->instr_start_index[0] = 0;


    for (chn = 1; chn < MAX_CHANNEL_NUM; chn++) {
        mid_dec_obj->instr_key_map[chn] = mid_dec_obj->instr_key_map[0];
        mid_dec_obj->instr_start_index[chn] = mid_dec_obj->instr_start_index[0];
    }


    for (chn = 0; chn < MAX_CHANNEL_NUM; chn++) {
        mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_VOL_CC] = 127;
        mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_EXPR_CC] = 127;
        mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_PAN_CC] = 64;
        mid_dec_obj->pitchBend_v[chn] = 256;
    }

    mid_dec_obj->srTicks = (midi_dec_param->tempo / 4) * (smpl_rate_tab[mid_dec_obj->sample_rate] * (1 << (MIDI_RESAMPLE_SHIFT - 6)) / 3094);
    mid_dec_obj->numTrk = 1;

    MULSI(mid_dec_obj->now_srTicks, tmp64, mid_dec_obj->srTicks, mid_dec_obj->midi_tempo_v, 10);

    int *ptr = mid_dec_obj->mempool;
    mid_dec_obj->midi_players = (MIDI_PLAYER *)ptr;
    memset(mid_dec_obj->midi_players, 0, needMidiPlayersBuf());
    ptr = ptr + needMidiPlayersBuf() / sizeof(int);
    mid_dec_obj->play_key = (u8 *)ptr;
    for (int i = 0; i < MAX_CTR_PLAYER_CNT * MAX_CHANNEL_NUM; i++) {
        mid_dec_obj->play_key[i] = 255;
    }
    return 0;
}


static u32 MIDI_CTRL_MAIN(void *ptr)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)ptr;
    unsigned int trk_num = 0;
    int sample_cnt, j;
    EVENT_FIFO_CONTEXT *midi_fifo_t = (EVENT_FIFO_CONTEXT *)(&mid_dec_obj->smf_data);

    if (mid_dec_obj->o_len) {
        unsigned char *obuf = (unsigned char *)mid_dec_obj->out_channel;
        int midi_olen;
        int len_out = mid_dec_obj->o_len;
        midi_olen = midi_fifo_t->player_control.output(midi_fifo_t->player_control.priv, &obuf[mid_dec_obj->o_index], len_out);
        memset(mid_dec_obj->out_val, 0, sizeof(mid_dec_obj->out_val));
        if (midi_olen < len_out) {
            mid_dec_obj->o_index += midi_olen;
            mid_dec_obj->o_len = len_out - midi_olen;
            return 0;
        }
        sample_cnt = mid_dec_obj->sample_cnt_backup;
        mid_dec_obj->o_len = 0;
        mid_dec_obj->o_index = 0;
        goto _MIDI_OUTPUT;
    }


    mid_dec_obj->theTick += mid_dec_obj->now_srTicks;
    sample_cnt = mid_dec_obj->theTick >> MIDI_RESAMPLE_SHIFT;
    mid_dec_obj->theTick &= MIDI_RESAMPLE_VAL;

    int midi_olen;
_MIDI_OUTPUT:
    while (sample_cnt) {
        int len_out = (sample_cnt > MIDI_OBUF_BLOCK) ? MIDI_OBUF_BLOCK : sample_cnt;
        for (j = 0; j < mid_dec_obj->MAX_PLAYER_CNTt; j++) {
            long long tmp64;
            if (mid_dec_obj->midi_player_on & BIT(j)) {
                WaveInfo_t *waveInfo;
                int tmp_r;
                waveInfo = &(mid_dec_obj->midi_players[j].wave_info);

                if (1) {
                    u8 tmp_i;
                    for (tmp_i = 0; tmp_i < len_out; tmp_i++) {
                        if (waveInfo->vol_cnt == 0) {
                            waveInfo->vol_now += waveInfo->attack_incr;
                            if (waveInfo->vol_now > (1L << 30)) {
                                waveInfo->vol_now = 1L << 30;
                                waveInfo->vol_cnt = 1;
                            }
                            MULSI(tmp_r, tmp64, waveInfo->vol_now, waveInfo->vol_atten, 29);
                            waveInfo->vol_array[tmp_i] = tmp_r;

                        } else {
                            if (waveInfo->vol_hold_flag) {
                                waveInfo->vol_hold_flag--;
                                MULSI(tmp_r, tmp64, waveInfo->vol_now, waveInfo->vol_atten, 29);
                                waveInfo->vol_array[tmp_i] = tmp_r;
                            } else if (waveInfo->env_info.env_use) {
                                ENV *env_info = &waveInfo->env_info;

                                if (env_info->stepcnt > 0) {
                                    int tmp32 = env_info->vol_f + env_info->stepval;
                                    if (tmp32 > 1073741824) {
                                        tmp32 = 1073741824;
                                    } else if (tmp32 < 0) {
                                        tmp32 = 0;
                                    }
                                    env_info->vol_f = tmp32;
                                    env_info->stepcnt--;
                                }

                                if (env_info->stepcnt <= 0) {
                                    int doPoints = smpl_rate_tab[mid_dec_obj->sample_rate] * 5 / 1000;
                                    if (env_info->env_points > 1) {
                                        u32 valget = 0;

                                        u8 readval = env_info->env_infov[env_info->envi++];
                                        valget = (readval & 0x7f);

                                        short tmpshift = 7;
                                        while (!(readval & 0x80)) {
                                            readval = env_info->env_infov[env_info->envi++];
                                            valget = ((readval & 0x7f) << tmpshift) | valget;
                                            tmpshift += 7;
                                        }
                                        int stepcnt = valget;

                                        readval = env_info->env_infov[env_info->envi++];
                                        valget = env_info->env_infov[env_info->envi++];
                                        valget = (valget << 8) | readval;
                                        short tmpste = (short)(valget & 0xffff);

                                        int tmpste1 = tmpste;
                                        env_info->stepval = (tmpste1 << 15) / doPoints;
                                        env_info->stepcnt = stepcnt * doPoints;

                                        env_info->env_points--;
                                        if (env_info->env_points == 1) {
                                            if (env_info->vol_f < 32212255) {
                                                env_info->stepcnt = 0;
                                            }
                                        }
                                    } else {
                                        if (env_info->vol_f < 32212255) {
                                            MULSI(env_info->vol_f, tmp64, env_info->vol_f, waveInfo->vol_dec_now, waveInfo->vol_dec_now_bit);
                                        }
                                    }
                                }
                                MULSI(tmp_r, tmp64, waveInfo->vol_now, env_info->vol_f, 30);
                                MULSI(tmp_r, tmp64, tmp_r, waveInfo->vol_atten, 29);
                                waveInfo->vol_array[tmp_i] = tmp_r;
                            } else {
                                MULSI(waveInfo->vol_now, tmp64, waveInfo->vol_now, waveInfo->vol_dec_now, waveInfo->vol_dec_now_bit);
                                if (waveInfo->vol_now < waveInfo->vol_hold_new) {
                                    waveInfo->vol_now = waveInfo->vol_hold_new;
                                }

                                MULSI(tmp_r, tmp64, waveInfo->vol_now, waveInfo->vol_atten, 29);
                                waveInfo->vol_array[tmp_i] = tmp_r;
                            }
                        }

                    }
                }

                {
                    if (waveInfo->ison == SEQ_AE_ON) {
                        if (midi_ctrl_gen_sample(mid_dec_obj, j, len_out)) {
                            if ((!waveInfo->pendingStop) && (waveInfo->voice.isend)) {
                                waveInfo->exclOn = 0;
                                mid_dec_obj->midi_player_on &= ~BIT(j);

                                if (mid_dec_obj->melody_stop && waveInfo->active_off_flag) {
                                    mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, mid_dec_obj->main_key[j], waveInfo->chnl);
                                }
                            }
                        }
                    } else if (waveInfo->ison == SEQ_AE_REL) {
                        if ((!waveInfo->pendingStop) && (waveInfo->voice.isend)) {
                            waveInfo->exclOn = 0;
                            mid_dec_obj->midi_player_on &= ~BIT(j);

                            if (mid_dec_obj->melody_stop && waveInfo->active_off_flag) {
                                mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, mid_dec_obj->main_key[j], waveInfo->chnl);
                            }
                        } else {
                            {
                                midi_ctrl_gen_sample(mid_dec_obj, j, len_out);
                                if (waveInfo->voice.isend) {
                                    mid_dec_obj->midi_player_on &= ~BIT(j);

                                    if (mid_dec_obj->melody_stop && waveInfo->active_off_flag) {
                                        mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, mid_dec_obj->main_key[j], waveInfo->chnl);
                                    }
                                }
                            }

                        }
                    }

                }

                if (waveInfo->vol_now == 0) {
                    mid_dec_obj->midi_player_on &= ~BIT(j);

                    if (mid_dec_obj->melody_stop && waveInfo->active_off_flag) {
                        mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, mid_dec_obj->main_key[j], waveInfo->chnl);
                    }
                }

            }
        }

        {
            int tmp_i;
            int tmp_res;
            if (mid_dec_obj->limiter_enable) {
                for (tmp_i = 0; tmp_i < len_out; tmp_i++) {
                    tmp_res = (mid_dec_obj->out_val[tmp_i] + 512) >> 10;
                    mid_dec_obj->out_val[tmp_i] = tmp_res;
                }
                mid_dec_obj->limiter_trig.limiter_run(mid_dec_obj->limiter_trig.limiter_buf, mid_dec_obj->out_val, mid_dec_obj->out_channel, len_out);
            } else {
                for (tmp_i = 0; tmp_i < len_out; tmp_i++) {
                    tmp_res = (mid_dec_obj->out_val[tmp_i] + 512) >> 10;
                    if (tmp_res > 32767) {
                        tmp_res = 32767;
                    } else if (tmp_res < -32768) {
                        tmp_res = -32768;
                    }
                    mid_dec_obj->out_channel[tmp_i] = tmp_res;
                }
            }

            midi_olen = midi_fifo_t->player_control.output(midi_fifo_t->player_control.priv, mid_dec_obj->out_channel, len_out * 2);
            memset(mid_dec_obj->out_val, 0, sizeof(mid_dec_obj->out_val));
        }
        sample_cnt = sample_cnt - len_out;
        if (midi_olen < len_out * 2) {
            mid_dec_obj->o_index = midi_olen;
            mid_dec_obj->o_len = len_out * 2 - midi_olen;
            mid_dec_obj->sample_cnt_backup = sample_cnt;
            return 0;
        }

    }

    return 0;
}

static u32 need_dcbuf_size()
{
    int bufsize = sizeof(MIDI_DECODE_VAR);
    bufsize += needMidiPlayersBuf();
    bufsize += needPlayKeyBuf();
    return  bufsize;
}


//设置乐器
static u32 midi_player_prog(void *work_buf, u8 prog, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    u16 zone_key_v = 0;
    int tbank = prog;
    if (chn == 9) {
        tbank = 128 + prog;
    }
    if (mid_dec_obj->instr_on[chn] != tbank) {
        mid_dec_obj->instr_on[chn] = tbank;
        zone_key_v = mid_dec_obj->midi_spi_pos[2 + tbank];
        mid_dec_obj->instr_start_index[chn] = (unsigned short)mid_dec_obj->instr_map[zone_key_v];
        mid_dec_obj->instr_key_map[chn] = (char *)(&mid_dec_obj->spi_key_start[zone_key_v * 128]);
    }

    return 0;
}

//播放一个音符
static u32   midi_play_note_on(void *work_buf, u8 nkey, u8 nvel, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    nkey = nkey & 0x7f;
    nvel = nvel & 0x7f;
    if (nvel == 0) {
        WaveInfo_t *temp_t;
        int i;
        for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
            if (mid_dec_obj->midi_player_on & BIT(i)) {
                temp_t = &(mid_dec_obj->midi_players[i].wave_info);
                if ((temp_t->chnl == chn) && (temp_t->noKey == nkey)) {
                    if (!control_stop(mid_dec_obj, temp_t, chn)) {
                        return 0;
                    }
                }

            }
        }
        return 0;
    }
    {
        int i, player_OK = 0;
        int t_instr_res;

        for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
            if ((mid_dec_obj->midi_player_on & BIT(i))) {
                WaveInfo_t *twaveinfo = &mid_dec_obj->midi_players[i].wave_info;
                if ((twaveinfo->chnl == chn) && (twaveinfo->noKey == nkey)) {
                    control_stop(mid_dec_obj, twaveinfo, chn);
                }
            } else {
                if (!player_OK) {
                    mid_dec_obj->player_index = i;
                    player_OK = 1;
                }
            }
        }

        t_instr_res = mid_dec_obj->instr_key_map[chn][nkey];
        if (t_instr_res == 0) {
            unsigned char *t_instr_k = (unsigned char *)mid_dec_obj->instr_key_map[chn];
            int t_instr_i;

            for (t_instr_i = nkey - 1; t_instr_i >= 0; t_instr_i--) {
                if (t_instr_k[t_instr_i]) {
                    t_instr_res = t_instr_k[t_instr_i];
                    break;
                }
            }
            if (!t_instr_res) {
                for (t_instr_i = nkey; t_instr_i < 128; t_instr_i++) {
                    if (t_instr_res < t_instr_k[t_instr_i]) {
                        t_instr_res = t_instr_k[t_instr_i];
                        break;
                    }
                }
            }

        }
        if (t_instr_res != 0) {
            unsigned char *zone_ptr;

            if (mid_dec_obj->crtl_mode != CMD_MIDI_CTRL_MODE_W2S) {
                zone_ptr = mid_dec_obj->spi_zone_start + (mid_dec_obj->instr_start_index[chn] + t_instr_res - 1) * sizeof(Zone_t);
                memcpy(&(mid_dec_obj->zone), zone_ptr, sizeof(Zone_t));
            }
        }


        if (player_OK == 0) {
            u32 vol_now = 0xffffffff;
            for (i = mid_dec_obj->MAX_PLAYER_CNTt - 1; i >= 0; i--) {
                if (mid_dec_obj->midi_players[i].wave_info.ison == SEQ_AE_REL) {
                    if (mid_dec_obj->midi_players[i].wave_info.vol_now < vol_now) {
                        vol_now = mid_dec_obj->midi_players[i].wave_info.vol_now;
                        player_OK = 1;
                        mid_dec_obj->player_index = i;
                    }
                }
            }
        }

        if (player_OK == 1) {
            player_control_note_on(mid_dec_obj, chn, 0, nkey, nvel);

            if (mid_dec_obj->melody_on) {
                mid_dec_obj->melody_trig.melody_trigger(mid_dec_obj->melody_trig.priv, (nkey & 0x7f), (nvel & 0x7f));
            }
            if (mid_dec_obj->melody_stop) {
                mid_dec_obj->main_key[mid_dec_obj->player_index] = nkey;
            }
        }
    }
    return 0;
}


//播放一个音符
static u32   midi_play_note_off(void *work_buf, u8 nkey, u8 chn, u16 time)
{

    nkey = nkey & 0x7f;
    time = time & 0xffff;

    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    WaveInfo_t *temp_t;
    int i;
    for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
        if (mid_dec_obj->midi_player_on & BIT(i)) {
            temp_t = &(mid_dec_obj->midi_players[i].wave_info);
            if ((temp_t->chnl == chn) && (temp_t->noKey == nkey)) {
                if (!control_stop(mid_dec_obj, temp_t, chn)) {
                    return 0;
                }
                if (time) {
                    SetKeyDecay(temp_t, time * smpl_rate_tab[mid_dec_obj->sample_rate] / 1000);
                } else {
                    if (NOTE_OFF_TRIGGER) {
                        temp_t->active_off_flag = 0;
                    }
                }
            }

        }
    }
    return 0;
}

static u32 midi_pitchBend(void *work_buf, u16 pitch_val, u8 chn)
{
    WaveInfo_t *waveInfo;
    u16 shift_v;
    u8 tmp_i;

    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    shift_v = pitch_val;
    mid_dec_obj->pitchBend_v[chn] = shift_v;
    for (tmp_i = 0; tmp_i < mid_dec_obj->MAX_PLAYER_CNTt; tmp_i++) {
        if (mid_dec_obj->midi_player_on & BIT(tmp_i)) {
            if (mid_dec_obj->midi_players[tmp_i].wave_info.chnl == chn) {
                waveInfo = &mid_dec_obj->midi_players[tmp_i].wave_info;
                waveInfo->voice.indexIncr = (waveInfo->initAtten * shift_v) >> 8;
            }
        }
    }
    return 0;
}


static u32 midi_vellfo(void *work_buf, u8 nkey, u8 vel_step, u8 vel_rate, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    unsigned short *modulatep = (unsigned short *)mid_dec_obj->track_len;
    WaveInfo_t *temp_t;
    int i;
    for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
        if (mid_dec_obj->midi_player_on & BIT(i)) {
            temp_t = &(mid_dec_obj->midi_players[i].wave_info);
            if ((temp_t->chnl == chn) && (temp_t->noKey == nkey)) {
                temp_t->modulate_vel = ((vel_rate & 0x0f) << 3) | (vel_step & 0x7);
                modulatep[i] = 8192;
            }
        }
    }
    return 0;
}

static u8 *query_play_key(void *work_buf, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    int play_count = 0;
    u8 *play_key = mid_dec_obj->play_key + chn * MAX_CTR_PLAYER_CNT;
    for (int j = 0; j < mid_dec_obj->MAX_PLAYER_CNTt; j++) {
        if (mid_dec_obj->midi_player_on & BIT(j)) {
            WaveInfo_t *waveInfo;
            waveInfo = &(mid_dec_obj->midi_players[j].wave_info);
            play_key[play_count] = waveInfo->noKey;
            play_count++;
        }
    }

    for (int j = play_count; j < mid_dec_obj->MAX_PLAYER_CNTt; j++) {
        play_key[j] = 255;
    }
    return play_key;
}

static u32 midi_glissando(void *work_buf, u16 pitch_val, u8 key, u8 chn)
{
    WaveInfo_t *waveInfo;
    u16 shift_v;
    u8 tmp_i;
    //����pitch_bend������
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    shift_v = pitch_val;
    mid_dec_obj->pitchBend_v[chn] = shift_v;
    for (tmp_i = 0; tmp_i < mid_dec_obj->MAX_PLAYER_CNTt; tmp_i++) {
        if (mid_dec_obj->midi_player_on & BIT(tmp_i)) {
            waveInfo = &mid_dec_obj->midi_players[tmp_i].wave_info;
            if (waveInfo->chnl == chn && waveInfo->noKey == key) {
                waveInfo->voice.indexIncr = (waveInfo->initAtten * shift_v) >> 8;
            }
        }
    }
    return 0;
}

const MIDI_CTRL_CONTEXT  midi_ctrl_obj = {
    need_dcbuf_size,       //need_workbuf_size
    MIDI_CTRL_OPEN,
    MIDI_CTRL_MAIN,
    midi_player_prog,
    midi_play_note_on,
    midi_play_note_off,
    midi_pitchBend,
    midi_dec_confing,
    midi_vellfo,
    query_play_key,
    midi_glissando
};

MIDI_CTRL_CONTEXT *get_midi_ctrl_ops()
{
    return (MIDI_CTRL_CONTEXT *)(&midi_ctrl_obj);
}




