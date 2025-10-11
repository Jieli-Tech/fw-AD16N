#include "midi_dec.h"

typedef struct  _EVENT_FIFO_CONTEXT_ {
    MIDI_CTRL_PARM  player_control;
} EVENT_FIFO_CONTEXT;

AT_MIDI_SPARSE_CODE
static u32 needMidiPlayersBuf()
{
    return MAX_CTR_PLAYER_CNT * sizeof(MIDI_PLAYER);
}

AT_MIDI_SPARSE_CODE
static u32 needPlayKeyBuf()
{
    return MAX_CTR_PLAYER_CNT * sizeof(u8);
}

AT_MIDI_SPARSE_CODE
static u32 needMidiOutBuf()
{
    u32 bufsize;

    bufsize = MIDI_OBUF_BLOCK * sizeof(int) * 2;
    bufsize += MIDI_OBUF_BLOCK * sizeof(short) * 2;
    return bufsize;
}

AT_MIDI_SPARSE_CODE
static u32 needMidiCurveBuf()
{
    u32 bufsize;
    bufsize = MAX_DEC_PLAYER_CNT * CURVE_BUF;
    return bufsize;
}

AT_MIDI_SPARSE_CODE
static u32 MIDI_CTRL_OPEN(void *work_buf, void *dec_parm, void *parm)
{
    unsigned int chn;
    int addr;
    long long tmp64;
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    MIDI_CONFIG_PARM *midi_param = (MIDI_CONFIG_PARM *)parm;
    MIDI_CTRL_PARM *midi_dec_param = (MIDI_CTRL_PARM *)dec_parm;

    EVENT_FIFO_CONTEXT *midi_fifo_t = (EVENT_FIFO_CONTEXT *)(&mid_dec_obj->smf_data);

    memset(work_buf, 0, sizeof(MIDI_DECODE_VAR));
    memcpy(midi_fifo_t, midi_dec_param, sizeof(MIDI_CTRL_PARM));
    mid_dec_obj->dec_info.nch = midi_param->out_channel;
    mid_dec_obj->bitwidth = midi_param->bitwidth;
    mid_dec_obj->out_bit = midi_param->OutdataBit;
    if (mid_dec_obj->bitwidth == 24) {
        mid_dec_obj->out_bit = 1;
    }

    u16 cmporkind;
    cmporkind = *(u16 *)midi_param->spi_pos;
    mid_dec_obj->midi_spi_pos = (u32)midi_param->spi_pos + 2;

    if (cmporkind == 0xAFCD) {
        mid_dec_obj->compressEN = 1;
        mid_dec_obj->stereoEN = 0;
    } else if (cmporkind == 0xBFCD) {
        mid_dec_obj->compressEN = 1;
        mid_dec_obj->stereoEN = 1;
    } else if (cmporkind == 0xCFCD) {
        mid_dec_obj->compressEN = 0;
        mid_dec_obj->stereoEN = 1;
    } else if (cmporkind == 0xDFCD) {
        mid_dec_obj->stereoEN = 0;
    } else {
        return 1;
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

    GET_USHORT_VALUE(mid_dec_obj->instr_spi, mid_dec_obj->midi_spi_pos, 0);
    mid_dec_obj->instr_map = mid_dec_obj->midi_spi_pos + 258;
    mid_dec_obj->spi_key_start = mid_dec_obj->midi_spi_pos + (258 + mid_dec_obj->instr_spi * 2);

    mid_dec_obj->spi_zone_start = mid_dec_obj->spi_key_start + 128 * mid_dec_obj->instr_spi;
    mid_dec_obj->instr_key_map[0] = mid_dec_obj->spi_key_start;
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
    for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
        mid_dec_obj->midi_players[i].wave_info.stereo_index = -1;
    }

    mid_dec_obj->out_val = ptr;
    memset(mid_dec_obj->out_val, 0, MIDI_OBUF_BLOCK * 2 * sizeof(int));
    ptr = ptr + MIDI_OBUF_BLOCK * 2;

    mid_dec_obj->out_channel = (short *)ptr;
    memset(mid_dec_obj->out_channel, 0, MIDI_OBUF_BLOCK * 2 * sizeof(short));
    ptr = ptr + (MIDI_OBUF_BLOCK * 2 * sizeof(short) / sizeof(int));

    mid_dec_obj->play_key = (u8 *)ptr;
    for (int i = 0; i < MAX_CTR_PLAYER_CNT; i++) {
        mid_dec_obj->play_key[i] = 255;
    }
    ptr = ptr + needPlayKeyBuf() / sizeof(int);
    return 0;
}

AT_MIDI_CODE
static u32 MIDI_CTRL_MAIN(void *ptr)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)ptr;
    unsigned int trk_num = 0;
    int sample_cnt, j;
    EVENT_FIFO_CONTEXT *midi_fifo_t = (EVENT_FIFO_CONTEXT *)(&mid_dec_obj->smf_data);

    if (mid_dec_obj->o_len) {
        unsigned char *obuf;
        if (mid_dec_obj->out_bit == 0) {
            obuf = (unsigned char *)mid_dec_obj->out_channel;
        } else {
            obuf = (unsigned char *)mid_dec_obj->out_val;
        }
        int midi_olen;
        int len_out = mid_dec_obj->o_len;
        midi_olen = midi_fifo_t->player_control.output(midi_fifo_t->player_control.priv, &obuf[mid_dec_obj->o_index], len_out);

        if (midi_olen < len_out) {
            mid_dec_obj->o_index += midi_olen;
            mid_dec_obj->o_len = len_out - midi_olen;
            return 0;
        }
        sample_cnt = mid_dec_obj->sample_cnt_backup;
        mid_dec_obj->o_len = 0;
        mid_dec_obj->o_index = 0;
        memset(mid_dec_obj->out_val, 0, MIDI_OBUF_BLOCK * 2 * sizeof(int));
        goto _MIDI_OUTPUT;
    }


    mid_dec_obj->theTick += mid_dec_obj->now_srTicks;
    sample_cnt = mid_dec_obj->theTick >> MIDI_RESAMPLE_SHIFT;
    mid_dec_obj->theTick &= MIDI_RESAMPLE_VAL;
    int midi_olen;

_MIDI_OUTPUT:
    while (sample_cnt) {
        long long tmp64;
        int len_out = (sample_cnt > MIDI_OBUF_BLOCK) ? MIDI_OBUF_BLOCK : sample_cnt;
        for (j = 0; j < mid_dec_obj->MAX_PLAYER_CNTt; j++) {
            WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[j].wave_info);
            if (waveInfo->player_on) {
                int tmp_r;
                if (1) {
                    //这里要改成vol是上升到16384
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
                                    int tmp32 = env_info->vol_f + env_info->stepval;   //限制下0到1，防止精度问题
                                    if (tmp32 > 1073741824) {
                                        tmp32 = 1073741824;
                                    }
                                    if (tmp32 < 0) {
                                        tmp32 = 0;
                                    }
                                    env_info->vol_f = tmp32;
                                    env_info->stepcnt--;
                                }

                                if (env_info->stepcnt <= 0) {
                                    u8 *env_infov = (u8 *)env_info->env_infov;
                                    int doPoints = smpl_rate_tab[mid_dec_obj->sample_rate] * 5 / 1000;
                                    if (env_info->env_points > 1) {  //这里用>1是因为，这个points存的跟工具生成的点数一样，但是实际音色库里面只生成delta，所以会比工具生成的点数少一个。
                                        u32 valget = 0;

                                        u8 readval = env_infov[env_info->envi++];
                                        valget = (readval & 0x7f);

                                        short tmpshift = 7;
                                        while (!(readval & 0x80)) {
                                            readval = env_infov[env_info->envi++];
                                            valget = ((readval & 0x7f) << tmpshift) | valget;
                                            tmpshift += 7;
                                        }
                                        int stepcnt = valget;                       //获取下一个cnt

                                        readval = env_infov[env_info->envi++];
                                        valget = env_infov[env_info->envi++];
                                        valget = (valget << 8) | readval;
                                        short tmpste = (short)(valget & 0xffff);   //获取下一个step

                                        env_info->stepval = tmpste * (1 << 15) / doPoints;
                                        env_info->stepcnt = stepcnt * doPoints;          //上面是5ms，所以根据采样率做换算哈

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
                                waveInfo->player_on = 0;

                                if (waveInfo->stereo_index >= 0) {
                                    mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                                }

                                if (mid_dec_obj->melody_stop && waveInfo->active_off_flag && (waveInfo->stero_channel == 0)) {
                                    mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                }
                                waveInfo->stereo_index = -1;
                            }
                        }
                    } else if (waveInfo->ison == SEQ_AE_REL) {
                        if ((!waveInfo->pendingStop) && (waveInfo->voice.isend)) {
                            waveInfo->exclOn = 0;
                            waveInfo->player_on = 0;

                            if (waveInfo->stereo_index >= 0) {
                                mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                            }

                            if (mid_dec_obj->melody_stop && waveInfo->active_off_flag && (waveInfo->stero_channel == 0)) {
                                mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                            }
                            waveInfo->stereo_index = -1;
                        } else {
                            {
                                midi_ctrl_gen_sample(mid_dec_obj, j, len_out);
                                if (waveInfo->voice.isend) {
                                    waveInfo->player_on = 0;

                                    if (waveInfo->stereo_index >= 0) {
                                        mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                                    }

                                    if (mid_dec_obj->melody_stop && waveInfo->active_off_flag && (waveInfo->stereo_index == 0)) {
                                        mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                    }
                                    waveInfo->stereo_index = -1;
                                }
                            }

                        }
                    }

                }

                if (waveInfo->vol_now < 32768) {
                    waveInfo->player_on = 0;
                    if (waveInfo->stereo_index >= 0) {
                        mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                    }


                    if (mid_dec_obj->melody_stop && waveInfo->active_off_flag && (waveInfo->stero_channel == 0)) {
                        mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                    }
                    waveInfo->stereo_index = -1;
                }

            }
        }

        if (mid_dec_obj->out_bit == 0) {
            int out_chnl = mid_dec_obj->dec_info.nch;
            int out_byte = len_out * out_chnl * sizeof(short);
            {
                int tmp_i, step;
                int tmp_res = 0;
                if (out_chnl == 1) {
                    step = 2;
                } else {
                    step = 1;
                }
                int *out_val = mid_dec_obj->out_val;
                int *outdata = mid_dec_obj->out_val;
                if (mid_dec_obj->limiter_enable) {
                    for (tmp_i = 0; tmp_i < (len_out * out_chnl); tmp_i++) {
                        tmp_res = *out_val;
                        tmp_res = (tmp_res + 512) >> 10;
                        *outdata = tmp_res;
                        out_val = out_val + step;
                        outdata = outdata + 1;
                    }
                    mid_dec_obj->limiter_info.limiter_run(mid_dec_obj->limiter_info.limiter_buf, mid_dec_obj->out_val, mid_dec_obj->out_channel, len_out);
                } else {
                    for (tmp_i = 0; tmp_i < (len_out * out_chnl); tmp_i++) {
                        tmp_res = *out_val;
                        tmp_res = (tmp_res + 512) >> 10;
                        if (tmp_res > 32767) {
                            tmp_res = 32767;
                        }
                        if (tmp_res < -32768) {
                            tmp_res = -32768;
                        }
                        mid_dec_obj->out_channel[tmp_i] = tmp_res;
                        out_val = out_val + step;
                    }
                }

                midi_olen = midi_fifo_t->player_control.output(midi_fifo_t->player_control.priv, mid_dec_obj->out_channel, out_byte);
                memset(mid_dec_obj->out_val, 0, MIDI_OBUF_BLOCK * 2 * sizeof(int));
            }
            sample_cnt = sample_cnt - len_out;
            if (midi_olen < out_byte) {
                mid_dec_obj->o_index = midi_olen;
                mid_dec_obj->o_len = out_byte - midi_olen;
                mid_dec_obj->sample_cnt_backup = sample_cnt;
                return 0;
            }
        } else {
            int out_chnl = mid_dec_obj->dec_info.nch;
            int out_byte = len_out * out_chnl * sizeof(int);
            int out_bit, outbit_div2;
            if (mid_dec_obj->bitwidth == 16) {
                out_bit = 10;
                outbit_div2 = 512;
            } else {
                out_bit = 2;
                outbit_div2 = 1;
            }

            {
                int tmp_i, step;
                int tmp_res = 0;
                if (out_chnl == 1) {
                    step = 2;
                } else {
                    step = 1;
                }
                for (tmp_i = 0; tmp_i < (len_out * out_chnl); tmp_i++) {
                    tmp_res = (mid_dec_obj->out_val[tmp_i * step] + outbit_div2) >> out_bit;
                    mid_dec_obj->out_val[tmp_i] = tmp_res;
                }

                midi_olen = midi_fifo_t->player_control.output(midi_fifo_t->player_control.priv, mid_dec_obj->out_val, out_byte);
            }
            sample_cnt = sample_cnt - len_out;
            if (midi_olen < out_byte) {
                mid_dec_obj->o_index = midi_olen;
                mid_dec_obj->o_len = out_byte - midi_olen;
                mid_dec_obj->sample_cnt_backup = sample_cnt;
                return 0;
            }
            memset(mid_dec_obj->out_val, 0, MIDI_OBUF_BLOCK * 2 * sizeof(int));
        }

    }

    return 0;
}

AT_MIDI_SPARSE_CODE
static u32 need_dcbuf_size()
{
    int bufsize = sizeof(MIDI_DECODE_VAR);
    bufsize += needMidiPlayersBuf();
    bufsize += needPlayKeyBuf();
    bufsize += needMidiOutBuf();
    return  bufsize;
}


//设置乐器
AT_MIDI_SPARSE_CODE
static u32 midi_player_prog(void *work_buf, u8 prog, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    u8 zone_key_v = 0;
    int tbank = prog;
    int addr;
    if (chn == 9) {
        tbank = 128 + prog;
    }
    if (mid_dec_obj->instr_on[chn] != tbank) {
        mid_dec_obj->instr_on[chn] = tbank;
        GET_UBYTE_VALUE(zone_key_v, mid_dec_obj->midi_spi_pos, (2 + tbank));
        GET_USHORT_VALUE(mid_dec_obj->instr_start_index[chn], mid_dec_obj->instr_map, zone_key_v);
        mid_dec_obj->instr_key_map[chn] = mid_dec_obj->spi_key_start + zone_key_v * 128;
    }

    return 0;
}

//播放一个音符
AT_MIDI_CODE
static u32   midi_play_note_on(void *work_buf, u8 nkey, u8 nvel, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    int addr;
    nkey = nkey & 0x7f;
    nvel = nvel & 0x7f;
    if (nvel == 0) {
        WaveInfo_t *temp_t;
        for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
            temp_t = &(mid_dec_obj->midi_players[i].wave_info);
            if (temp_t->player_on) {
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
        int player_OK = 0;
        u8 t_instr_res;

        for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
            WaveInfo_t *twaveinfo = &mid_dec_obj->midi_players[i].wave_info;
            if (twaveinfo->player_on) {
                if ((twaveinfo->chnl == chn) && (twaveinfo->noKey == nkey)) {
                    control_stop(mid_dec_obj, twaveinfo, chn);
                }
            }
        }

        GET_UBYTE_VALUE(t_instr_res, mid_dec_obj->instr_key_map[chn], nkey);
        if (t_instr_res == 0) {
            u8 instr_res_tmp;
            int t_instr_i;
            for (t_instr_i = nkey - 1; t_instr_i >= 0; t_instr_i--) {
                GET_UBYTE_VALUE(instr_res_tmp, mid_dec_obj->instr_key_map[chn], t_instr_i);
                if (instr_res_tmp) {
                    t_instr_res = instr_res_tmp;
                    break;
                }
            }
            if (!t_instr_res) {
                for (t_instr_i = nkey; t_instr_i < 128; t_instr_i++) {
                    GET_UBYTE_VALUE(instr_res_tmp, mid_dec_obj->instr_key_map[chn], t_instr_i);
                    if (t_instr_res < instr_res_tmp) {
                        t_instr_res = instr_res_tmp;
                        break;
                    }
                }
            }

        }
        if (t_instr_res != 0) {

            unsigned char *zone_ptr = (u8 *)(mid_dec_obj->spi_zone_start + (mid_dec_obj->instr_start_index[chn] + t_instr_res - 1) * sizeof(Zone_t));
            memcpy(&(mid_dec_obj->zone), zone_ptr, sizeof(Zone_t));
        }


        int player_index;
        int stereoflag = 0;
        if (mid_dec_obj->stereoEN) {
            if (mid_dec_obj->zone.pan > 63 || mid_dec_obj->zone.pan < -64) {
                stereoflag = 1;
            }
        }

        if (stereoflag == 0) {
            for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                if (!mid_dec_obj->midi_players[i].wave_info.player_on) {
                    mid_dec_obj->player_index = i;
                    player_OK = 1;
                    break;
                }
            }

            if (player_OK == 0) {
                u32 vol_now = 0xffffffff;
                for (int i = mid_dec_obj->MAX_PLAYER_CNTt - 1; i >= 0; i--) {
                    if (mid_dec_obj->midi_players[i].wave_info.ison == SEQ_AE_REL) {
                        if (mid_dec_obj->midi_players[i].wave_info.vol_now < vol_now) {
                            vol_now = mid_dec_obj->midi_players[i].wave_info.vol_now;
                            player_OK = 1;
                            mid_dec_obj->player_index = i;
                        }
                    }
                }

                WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info);
                waveInfo->player_on = 0;
                if (mid_dec_obj->melody_stop) {
                    mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                }

                if (mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info.stereo_index >= 0) {
                    player_index = mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info.stereo_index;
                    mid_dec_obj->midi_players[player_index].wave_info.player_on = 0;
                }
            }
        } else {
            mid_dec_obj->player_index = -1;
            mid_dec_obj->player_index_stero = -1;
            for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                if (!mid_dec_obj->midi_players[i].wave_info.player_on) {
                    if (mid_dec_obj->player_index < 0) {
                        mid_dec_obj->player_index = i;
                    } else {
                        mid_dec_obj->player_index_stero = i;
                    }

                    if (mid_dec_obj->player_index_stero > 0) {
                        player_OK = 1;
                        break;
                    }
                }
            }

            if (player_OK == 0) {
                if (mid_dec_obj->player_index > 0) {
                    u32 vol_now = 0xffffffff;
                    for (int i = mid_dec_obj->MAX_PLAYER_CNTt - 1; i >= 0; i--) {
                        if (mid_dec_obj->midi_players[i].wave_info.ison == SEQ_AE_REL) {
                            if (mid_dec_obj->midi_players[i].wave_info.vol_now < vol_now) {
                                vol_now = mid_dec_obj->midi_players[i].wave_info.vol_now;
                                player_OK = 1;
                                mid_dec_obj->player_index_stero = i;
                            }
                        }
                    }

                    if (player_OK) {
                        WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info);
                        waveInfo->player_on = 0;
                        if (mid_dec_obj->melody_stop) {
                            mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                        }
                        if (mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info.stereo_index >= 0) {
                            player_index = mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info.stereo_index;
                            mid_dec_obj->midi_players[player_index].wave_info.player_on = 0;
                        }
                    }
                } else {
                    u32 vol_now = 0xffffffff;
                    short index;
                    for (int i = mid_dec_obj->MAX_PLAYER_CNTt - 1; i >= 0; i--) {
                        if (mid_dec_obj->midi_players[i].wave_info.ison == SEQ_AE_REL) {
                            if (mid_dec_obj->midi_players[i].wave_info.vol_now < vol_now) {
                                vol_now = mid_dec_obj->midi_players[i].wave_info.vol_now;
                                index = mid_dec_obj->player_index;
                                mid_dec_obj->player_index = i;
                                if (mid_dec_obj->midi_players[i].wave_info.stereo_index >= 0) {
                                    mid_dec_obj->player_index_stero = mid_dec_obj->midi_players[i].wave_info.stereo_index;
                                } else {
                                    mid_dec_obj->player_index_stero = index;
                                }
                            }
                        }
                    }

                    if (mid_dec_obj->player_index_stero >= 0) {
                        player_OK = 1;
                        WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info);
                        WaveInfo_t *waveInfo1 = &(mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info);
                        waveInfo->player_on = 0;
                        waveInfo1->player_on = 0;
                        if (mid_dec_obj->melody_stop) {
                            mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                            mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo1->noKey, waveInfo1->chnl);
                        }

                        if (mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info.stereo_index >= 0) {
                            player_index = mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info.stereo_index;
                            mid_dec_obj->midi_players[player_index].wave_info.player_on = 0;
                        }
                    }
                }
            }
        }

        if (player_OK == 1) {
            player_control_note_on(mid_dec_obj, chn, 0, nkey, nvel, 0, stereoflag, mid_dec_obj->player_index);

            if (mid_dec_obj->melody_on) {
                mid_dec_obj->melody_trig.melody_trigger(mid_dec_obj->melody_trig.priv, (nkey & 0x7f), (nvel & 0x7f));
            }
        }
    }
    return 0;
}


//note off 一个音符
AT_MIDI_CODE
static u32   midi_play_note_off(void *work_buf, u8 nkey, u8 chn, u16 time)
{

    nkey = nkey & 0x7f;
    time = time & 0xffff;

    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    WaveInfo_t *temp_t;
    for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
        temp_t = &(mid_dec_obj->midi_players[i].wave_info);
        if (temp_t->player_on) {
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

AT_MIDI_SPARSE_CODE
static u32 midi_pitchBend(void *work_buf, u16 pitch_val, u8 chn)
{
    WaveInfo_t *waveInfo;
    u16 shift_v;
    u8 tmp_i;

    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    shift_v = pitch_val;
    mid_dec_obj->pitchBend_v[chn] = shift_v;
    for (tmp_i = 0; tmp_i < mid_dec_obj->MAX_PLAYER_CNTt; tmp_i++) {
        waveInfo = &mid_dec_obj->midi_players[tmp_i].wave_info;
        if ((waveInfo->player_on) && (waveInfo->chnl == chn)) {
            waveInfo->voice.indexIncr = (waveInfo->initAtten * shift_v) >> 8;
        }
    }
    return 0;
}

AT_MIDI_SPARSE_CODE
static u32 midi_vellfo(void *work_buf, u8 nkey, u8 vel_step, u8 vel_rate, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    unsigned short *modulatep = (unsigned short *)mid_dec_obj->track_len;
    WaveInfo_t *temp_t;
    int i;
    for (i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
        temp_t = &(mid_dec_obj->midi_players[i].wave_info);
        if (temp_t->player_on) {
            if ((temp_t->chnl == chn) && (temp_t->noKey == nkey)) {
                temp_t->modulate_vel = ((vel_rate & 0x0f) << 3) | (vel_step & 0x7);
                modulatep[i] = 8192;
            }
        }
    }
    return 0;
}

AT_MIDI_SPARSE_CODE
static u8 *query_play_key(void *work_buf, u8 chn)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    u8 k = 0;
    for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
        WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[i].wave_info);
        if ((waveInfo->player_on) && (waveInfo->chnl == chn)) {
            mid_dec_obj->play_key[k] = mid_dec_obj->midi_players[i].wave_info.noKey;
            k++;
        }
    }
    while (k < mid_dec_obj->MAX_PLAYER_CNTt) {
        mid_dec_obj->play_key[k] = 255;
        k++;
    }
    return mid_dec_obj->play_key;
}

AT_MIDI_SPARSE_CODE
static u32 midi_glissando(void *work_buf, u16 pitch_val, u8 key, u8 chn)
{
    WaveInfo_t *waveInfo;
    u16 shift_v;
    u8 tmp_i;

    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    shift_v = pitch_val;
    mid_dec_obj->pitchBend_v[chn] = shift_v;
    for (tmp_i = 0; tmp_i < mid_dec_obj->MAX_PLAYER_CNTt; tmp_i++) {
        waveInfo = &mid_dec_obj->midi_players[tmp_i].wave_info;
        if ((waveInfo->player_on) && (waveInfo->chnl == chn) && (waveInfo->noKey == key)) {
            waveInfo->voice.indexIncr = (waveInfo->initAtten * shift_v) >> 8;
        }
    }
    return 0;
}

AT_MIDI_SPARSE_CONST
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

AT_MIDI_SPARSE_CODE
MIDI_CTRL_CONTEXT *get_midi_ctrl_ops()
{
    return (MIDI_CTRL_CONTEXT *)(&midi_ctrl_obj);
}


