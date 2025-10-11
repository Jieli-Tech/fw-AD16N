#include "midi_dec.h"

AT_MIDI_SPARSE_CODE
void  midi_play_set_tempo(void *work_buf, MIDI_PLAY_CTRL_TEMPO *tempo_obj)
{
    long long tmp64;
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    u16 decayval0, decayval1;
    mid_dec_obj->midi_tempo_v = tempo_obj->tempo_val;
    MULSI(mid_dec_obj->now_srTicks, tmp64, mid_dec_obj->srTicks, mid_dec_obj->midi_tempo_v, 10);
    for (int i = 0; i < MAX_CHANNEL_NUM; i++) {
        decayval0 = tempo_obj->decay_val[i] & 0x7ff;
        decayval1 = tempo_obj->decay_val[i] >> 11;
        mid_dec_obj->decay_speed[i] = 32600 + ((168 * decayval0) >> 10);
        if (mid_dec_obj->decay_speed[i] > 32768) {
            mid_dec_obj->decay_speed[i] = 32768;
        }
    }
    mid_dec_obj->mute_threshold = tempo_obj->mute_threshold;
}

AT_MIDI_CODE
static void  midi_play_goon(void *work_buf)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    int player_OK = 0;
    mid_dec_obj->okon_goon_flag = 1;
    int player_index;

    if (mid_dec_obj->okon_start_flag) {
        mid_dec_obj->note_on = 0;
        mid_dec_obj->flag_t = 0;
    } else {
        WaveInfo_t *waveinfo = &mid_dec_obj->OKON_info[0];
        if (waveinfo->stereo_index == -1) {
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

                if (player_OK == 0) {
                    return;
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
                    if (mid_dec_obj->player_index_stero == -1) {
                        return;
                    }

                    WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info);
                    waveInfo->player_on = 0;
                    if (mid_dec_obj->melody_stop) {
                        mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                    }
                    if (mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info.stereo_index >= 0) {
                        player_index = mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info.stereo_index;
                        mid_dec_obj->midi_players[player_index].wave_info.player_on = 0;
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
                    if (mid_dec_obj->player_index_stero == -1) {
                        return;
                    }

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

        WaveInfo_t *wavInfo_t;
        wavInfo_t = &mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info;
        memcpy(wavInfo_t, waveinfo, sizeof(WaveInfo_t));

        int maintrackval;
        if (MAINTRACK_USE_CHN) {
            maintrackval = wavInfo_t->chnl;
        } else {
            maintrackval = wavInfo_t->trk_v;
        }

        if (waveinfo->stereo_index != -1) {
            wavInfo_t = &mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info;
            memcpy(wavInfo_t, &mid_dec_obj->OKON_info[1], sizeof(WaveInfo_t));

            mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info.stereo_index = mid_dec_obj->player_index_stero;
            mid_dec_obj->midi_players[mid_dec_obj->player_index_stero].wave_info.stereo_index = mid_dec_obj->player_index;
        }
        mid_dec_obj->note_on = 1;
        mid_dec_obj->flag_t = 0;
        mid_dec_obj->keydown_cnt++;
        if (mid_dec_obj->melody_on && (mid_dec_obj->key_mode == CMD_MIDI_MELODY_KEY_0) && (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1)) {
            mid_dec_obj->melody_trig.melody_trigger(mid_dec_obj->melody_trig.priv, (wavInfo_t->noKey & 0x7f), (wavInfo_t->noVel & 0x7f));
        }
    }
}

AT_MIDI_SPARSE_CODE
static void midi_save_bk_fun(MIDI_DECODE_VAR *mid_dec_obj)
{
    memcpy(&mid_dec_obj->bk_array_ptr[mid_dec_obj->bk_wr_cnt], &mid_dec_obj->fpos_cnt, sizeof(MIDI_SAVE_POS_STRUCT) - 8);
    mid_dec_obj->bk_array_ptr[mid_dec_obj->bk_wr_cnt].rd_pos = mid_dec_obj->smf_data.rd_pos;
    mid_dec_obj->bk_array_ptr[mid_dec_obj->bk_wr_cnt].remain = mid_dec_obj->smf_data.remain;

    mid_dec_obj->bk_check_flag = mid_dec_obj->bk_check_flag << 1;
    mid_dec_obj->bk_check_flag &= MAX_GO_VAL;
    mid_dec_obj->bk_check_flag |= mid_dec_obj->check_buf_flag;

    mid_dec_obj->check_buf_flag = 0;

    mid_dec_obj->bk_wr_cnt++;
    if (mid_dec_obj->bk_wr_cnt >= MAX_GO_BACK) {
        mid_dec_obj->bk_wr_cnt = 0;
    }
}


AT_MIDI_SPARSE_CODE
static u32 midi_switch_control(void *work_buf, u32 switch_val)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;

    if (switch_val & MARK_ENABLE) {
        mid_dec_obj->triggered = 1;
    } else {
        mid_dec_obj->triggered = 0;
    }

    if (switch_val & MELODY_ENABLE) {
        mid_dec_obj->melody_on = 1;
    } else {
        mid_dec_obj->melody_on = 0;
    }

    if (switch_val & TIM_DIV_ENABLE) {
        mid_dec_obj->tmDiv_enable = 1;
    } else {
        mid_dec_obj->tmDiv_enable = 0;
    }

    if (switch_val & MUTE_ENABLE) {
        mid_dec_obj->mute_enable = 1;
    } else {
        mid_dec_obj->mute_enable = 0;
    }

    if (switch_val & SAVE_DIV_ENBALE) {
        if (MIDI_SAVE_DIV_ENBALE) {
            mid_dec_obj->save_ptr_enable = 1;
        } else {
            mid_dec_obj->save_ptr_enable = 0;
        }
    } else {
        mid_dec_obj->save_ptr_enable = 0;
    }

    if (switch_val & EX_VOL_ENABLE) {
        mid_dec_obj->ex_flag = 1;
    } else {
        mid_dec_obj->ex_flag = 0;
    }

    if (switch_val & SET_PROG_ENABLE) {
        mid_dec_obj->ins_set = 1;

    } else {
        mid_dec_obj->ins_set = 0;
    }

    if (switch_val & MELODY_PLAY_ENABLE) {
        mid_dec_obj->melody_enable = 1;
    } else {
        mid_dec_obj->melody_enable = 0;
    }

    if (switch_val & BEAT_TRIG_ENABLE) {
        mid_dec_obj->beat_enable = 1;
    } else {
        mid_dec_obj->beat_enable = 0;
    }
    if (switch_val & MELODY_STOP_ENABLE) {
        mid_dec_obj->melody_stop = 1;
    } else {
        mid_dec_obj->melody_stop = 0;
    }

    if (switch_val & SEMITONE_ENABLE) {
        mid_dec_obj->pitch_enable = 1;
    } else {
        mid_dec_obj->pitch_enable = 0;
    }

    if (switch_val & LIMITER_ENABLE) {
        mid_dec_obj->limiter_enable = 1;
    } else {
        mid_dec_obj->limiter_enable = 0;
    }
    return 0;
}


AT_MIDI_SPARSE_CODE
static u32 midi_tone_init(MIDI_DECODE_VAR *mid_dec_obj, MIDI_CONFIG_PARM *itt_info)
{
    int chn;
    u8 tbank;
    u16 cmporkind;

    cmporkind = *(u16 *)itt_info->spi_pos;
    mid_dec_obj->midi_spi_pos = (u32)itt_info->spi_pos + 2;
    mid_dec_obj->dec_info.nch = itt_info->out_channel;
    mid_dec_obj->bitwidth = itt_info->bitwidth;
    mid_dec_obj->out_bit = itt_info->OutdataBit;
    if (mid_dec_obj->bitwidth == 24) {
        mid_dec_obj->out_bit = 1;
    }

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
        mid_dec_obj->compressEN = 0;
        mid_dec_obj->stereoEN = 0;
    } else {
        return 1;
    }

    mid_dec_obj->sample_rate = itt_info->sample_rate;
    mid_dec_obj->MAX_PLAYER_CNTt = itt_info->player_t;
    if (mid_dec_obj->MAX_PLAYER_CNTt > MAX_DEC_PLAYER_CNT) {
        mid_dec_obj->MAX_PLAYER_CNTt = MAX_DEC_PLAYER_CNT;
    }

    mid_dec_obj->instr_spi = *((unsigned short *)mid_dec_obj->midi_spi_pos);
    mid_dec_obj->instr_map = mid_dec_obj->midi_spi_pos + 258;
    mid_dec_obj->spi_key_start = mid_dec_obj->midi_spi_pos + (258 + mid_dec_obj->instr_spi * 2);

    mid_dec_obj->spi_zone_start = mid_dec_obj->spi_key_start + 128 * mid_dec_obj->instr_spi;
    mid_dec_obj->instr_key_map[0] = mid_dec_obj->spi_key_start;
    mid_dec_obj->instr_start_index[0] = 0;

    for (chn = 1; chn < MAX_CHANNEL_NUM; chn++) {
        mid_dec_obj->instr_key_map[chn] = mid_dec_obj->instr_key_map[0];
        mid_dec_obj->instr_start_index[chn] = mid_dec_obj->instr_start_index[0];
    }

    mid_dec_obj->instr_on[9] = 128;
    GET_UBYTE_VALUE(tbank, mid_dec_obj->midi_spi_pos, (2 + 128));
    GET_USHORT_VALUE(mid_dec_obj->instr_start_index[9], mid_dec_obj->instr_map, tbank);
    mid_dec_obj->instr_key_map[9] = mid_dec_obj->spi_key_start + tbank * 128;

    for (chn = 0; chn < MAX_CHANNEL_NUM; chn++) {
        mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_VOL_CC] = 100;
        mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_EXPR_CC] = 127;
        mid_dec_obj->channel_mixer[chn].cc[MIDI_CTRL_PAN_CC] = 64;
        mid_dec_obj->pitchBend_v[chn] = 256;                         //��ʼpitchBend��ֵ
    }
    mid_dec_obj->dec_info.sr = smpl_rate_tab[mid_dec_obj->sample_rate];
    mid_dec_obj->dec_info.br = 1;
    return 0;
}

AT_MIDI_SPARSE_CODE
u32 midi_dec_confing(void *work_buf, u32 cmd, void *parm)
{

    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;

    if (cmd == CMD_MIDI_SEEK_BACK_N) {
        if (mid_dec_obj->save_ptr_enable) {
            MIDI_SEEK_BACK_STRUCT *t_obj = (MIDI_SEEK_BACK_STRUCT *)parm;
            MIDI_SAVE_POS_STRUCT *tmp_obj;

            int seek_back_n = t_obj->seek_back_n;
            if (seek_back_n > MAX_GO_BACK) {
                seek_back_n = MAX_GO_BACK;
            }

            int rd_cnt = mid_dec_obj->bk_wr_cnt - seek_back_n;
            if (rd_cnt < 0) {
                rd_cnt = rd_cnt + MAX_GO_BACK;
            }

            int tmpn, go_back_n = 0;

            tmpn = mid_dec_obj->bk_check_flag & ((1 << (-rd_cnt)) - 1);

            while (tmpn) {
                if (tmpn & 1) {
                    go_back_n++;
                }
                tmpn = tmpn >> 1;
            }

            tmp_obj = &mid_dec_obj->bk_array_ptr[rd_cnt];
            memcpy(&mid_dec_obj->fpos_cnt, tmp_obj, sizeof(MIDI_SAVE_POS_STRUCT) - 8);
            mid_dec_obj->fpos_cnt -= tmp_obj->remain;
            mid_dec_obj->smf_data.rd_pos = 0;
            mid_dec_obj->smf_data.remain = 0;

            mid_dec_obj->tmDiv_up_cnt = 0;
            if (mid_dec_obj->out_total == 0) {
                mid_dec_obj->mdiv_cnt = 0;
                if (mid_dec_obj->tmDiv > 30) {
                    mid_dec_obj->mdiv_cnt = 2;
                }
            } else {
                mid_dec_obj->mdiv_cnt = 0;
            }

            mid_dec_obj->err = 0;
        }

    } else if (cmd == CMD_MIDI_SET_CHN_PROG) {
        u8 zone_key_v;
        int tbank, addr;
        MIDI_PROG_CTRL_STRUCT *t_obj = (MIDI_PROG_CTRL_STRUCT *)parm;
        mid_dec_obj->ins_trk = t_obj->prog;
        mid_dec_obj->replace_mode = t_obj->replace_mode;
        mid_dec_obj->set_chvol_in = t_obj->ex_vol;
        tbank = mid_dec_obj->ins_trk;

        GET_UBYTE_VALUE(zone_key_v, mid_dec_obj->midi_spi_pos, (2 + tbank));
        GET_USHORT_VALUE(mid_dec_obj->instr_start_index_set, mid_dec_obj->instr_map, zone_key_v);
        mid_dec_obj->instr_key_map_set = mid_dec_obj->spi_key_start + zone_key_v * 128;

    } else if (cmd == CMD_MIDI_CTRL_TEMPO) {
        midi_play_set_tempo(work_buf, (MIDI_PLAY_CTRL_TEMPO *)parm);
    } else if (cmd == CMD_MIDI_GOON) {
        //one key one note�ز���
        midi_play_goon(work_buf);
    } else if (cmd == CMD_MIDI_CTRL_MODE) {
        MIDI_PLAY_CTRL_MODE *tparm = (MIDI_PLAY_CTRL_MODE *)parm;
        midi_play_ctrl_on(work_buf, tparm);
    } else if (cmd == CMD_MIDI_SET_SWITCH) {
        u32 v_switch = *(u32 *)parm;
        midi_switch_control(work_buf, (u32)v_switch);
    } else if (cmd == CMD_MIDI_SET_EX_VOL) {
        EX_CH_VOL_PARM *tparm = (EX_CH_VOL_PARM *)parm;
        memcpy((u8 *)(&mid_dec_obj->ex_vol), tparm, sizeof(EX_CH_VOL_PARM));
    } else if (cmd == CMD_MIDI_MELODY_TRIGGER) {
        EX_MELODY_STRUCT *tparm = (EX_MELODY_STRUCT *)parm;
        memcpy((u8 *)(&mid_dec_obj->melody_trig), tparm, sizeof(EX_MELODY_STRUCT));
    } else if (cmd == CMD_MIDI_STOP_MELODY_TRIGGER) {
        EX_MELODY_STOP_STRUCT *tparm = (EX_MELODY_STOP_STRUCT *)parm;
        memcpy((u8 *)(&mid_dec_obj->melody_stop_trig), tparm, sizeof(EX_MELODY_STOP_STRUCT));
    } else if (cmd == CMD_INIT_CONFIG) {
        u8 zone_key_v, tbank;
        int addr;
        unsigned int chn;
        MIDI_INIT_STRUCT *itt_info = (MIDI_INIT_STRUCT *)parm;
        int err = midi_tone_init(mid_dec_obj, &itt_info->init_info);
        if (err) {
            return 1;
        }

        mid_dec_obj->key_mode = itt_info->okon_info.Melody_Key_Mode;
        mid_dec_obj->okon_mode = itt_info->okon_info.OKON_Mode;

        memcpy((u8 *)(&mid_dec_obj->ex_vol), (&itt_info->vol_info), sizeof(EX_CH_VOL_PARM));
        memcpy((u8 *)(&mid_dec_obj->semitone_ctrl), (&itt_info->semitone_info), sizeof(MIDI_SEMITONE_CTRL_STRUCT));
        memcpy((u8 *)(&mid_dec_obj->mark_trig), (&itt_info->mark_info), sizeof(EX_INFO_STRUCT));
        memcpy((u8 *)(&mid_dec_obj->melody_trig), (&itt_info->moledy_info), sizeof(EX_MELODY_STRUCT));
        memcpy((u8 *)(&mid_dec_obj->tmDiv_trig), (&itt_info->tmDiv_info), sizeof(EX_TmDIV_STRUCT));
        memcpy((u8 *)(&mid_dec_obj->beat_trig), (&itt_info->beat_info), sizeof(EX_BeatTrig_STRUCT));
        memcpy((u8 *)(&mid_dec_obj->melody_stop_trig), (&itt_info->moledy_stop_info), sizeof(EX_MELODY_STOP_STRUCT));
        memcpy((u8 *)(&mid_dec_obj->wdt_clear), (&itt_info->wdt_clear), sizeof(WDT_CLEAR));
        memcpy((u8 *)(&mid_dec_obj->note_on_trigger), (&itt_info->note_on_trigger), sizeof(MIDI_NOTE_ON));
        memcpy((u8 *)(&mid_dec_obj->note_off_trigger), (&itt_info->note_off_trigger), sizeof(MIDI_NOTE_OFF));
        memcpy((u8 *)(&mid_dec_obj->adsr_trigger), (&itt_info->adsr_trigger), sizeof(MIDI_ADSR));

        midi_play_ctrl_on(work_buf, &itt_info->mode_info);
        midi_play_set_tempo(work_buf, (MIDI_PLAY_CTRL_TEMPO *)&itt_info->tempo_info);

        mid_dec_obj->ins_trk = itt_info->prog_info.prog;
        mid_dec_obj->set_chvol_in = itt_info->prog_info.ex_vol;
        mid_dec_obj->replace_mode = itt_info->prog_info.replace_mode;
        if (itt_info->mainTrack_info.chn < 17) {
            mid_dec_obj->tracknV = itt_info->mainTrack_info.chn;
            mid_dec_obj->track_set = 1;
        }
        tbank = mid_dec_obj->ins_trk;

        GET_UBYTE_VALUE(zone_key_v, mid_dec_obj->midi_spi_pos, (2 + tbank));
        GET_USHORT_VALUE(mid_dec_obj->instr_start_index_set, mid_dec_obj->instr_map, zone_key_v);

        mid_dec_obj->instr_key_map_set = mid_dec_obj->spi_key_start + zone_key_v * 128;
        midi_switch_control(work_buf, itt_info->switch_info);

        if (mid_dec_obj->save_ptr_enable) {
            for (int test_i = 0; test_i < MAX_GO_BACK; test_i++) {
                midi_save_bk_fun(mid_dec_obj);
            }
        }

        memcpy((u8 *)(&mid_dec_obj->w2s_obj), (u8 *)(&itt_info->w2s_info), sizeof(MIDI_W2S_STRUCT));
        mid_dec_obj->midi_spi_posbk = (u32)mid_dec_obj->w2s_obj.rec_data;

    } else if (cmd == CMD_INIT_CONFIGS) {
        MIDI_CONFIG_PARM *itt_info = (MIDI_CONFIG_PARM *)parm;
        int err = midi_tone_init(mid_dec_obj, itt_info);
        if (err) {
            return 1;
        }
    } else if (cmd == CMD_MIDI_OKON_MODE) {
        MIDI_OKON_MODE *okon = (MIDI_OKON_MODE *)parm;
        mid_dec_obj->okon_mode = okon->OKON_Mode;
        mid_dec_obj->key_mode = okon->Melody_Key_Mode;
        if (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) {
            if (mid_dec_obj->key_mode == CMD_MIDI_MELODY_KEY_1) {
                mid_dec_obj->note_on = 1;
                mid_dec_obj->okon_start_flag = 0;
            } else {
                mid_dec_obj->note_on = 2;
                mid_dec_obj->okon_start_flag = 1;
            }
        }
    } else if (cmd == CMD_MIDI_SET_SEMITONE) {
        MIDI_SEMITONE_CTRL_STRUCT *semitone_ctrl = (MIDI_SEMITONE_CTRL_STRUCT *)parm;
        memcpy(&mid_dec_obj->semitone_ctrl, semitone_ctrl, sizeof(MIDI_SEMITONE_CTRL_STRUCT));
    } else if (cmd == CMD_MIDI_LIMITER_TRIGGER) {
        MIDI_Limiter *limiter_info = (MIDI_Limiter *)parm;
        memcpy(&mid_dec_obj->limiter_info, limiter_info, sizeof(MIDI_Limiter));
    } else if (cmd == CMD_MIDI_NOTE_ON_TRIGGER) {
        MIDI_NOTE_ON *note_on_trigger = (MIDI_NOTE_ON *)parm;
        memcpy(&mid_dec_obj->note_on_trigger, note_on_trigger, sizeof(MIDI_NOTE_ON));
    } else if (cmd == CMD_MIDI_NOTE_OFF_TRIGGER) {
        MIDI_NOTE_OFF *note_off_trigger = (MIDI_NOTE_OFF *)parm;
        memcpy(&mid_dec_obj->note_off_trigger, note_off_trigger, sizeof(MIDI_NOTE_OFF));
    } else if (cmd == CMD_MIDI_ADSR_TRIGGER) {
        MIDI_ADSR *adsr_trigger = (MIDI_ADSR *)parm;
        memcpy(&mid_dec_obj->adsr_trigger, adsr_trigger, sizeof(MIDI_ADSR));
    } else if (cmd == CMD_MIDI_SET_PROG) {
        MIDI_PROG_CHNL_CTRL *prog_info = (MIDI_PROG_CHNL_CTRL *)parm;
        u8 zone_key_v;
        int tbank, addr;
        tbank = prog_info->prog;
        int chn = prog_info->chnl;
        mid_dec_obj->instr_key_chnl_enable[chn] = prog_info->enable;
        if (prog_info->enable) {
            GET_UBYTE_VALUE(zone_key_v, mid_dec_obj->midi_spi_pos, (2 + tbank));
            GET_USHORT_VALUE(mid_dec_obj->instr_start_chnl_index[chn], mid_dec_obj->instr_map, zone_key_v);
            mid_dec_obj->instr_key_chnl_map[chn] = mid_dec_obj->spi_key_start + zone_key_v * 128;
        }
    } else if (cmd == CMD_MIDI_CTRL_EVENT) {
        MIDI_CTRL_EVENT *protect_trigger = (MIDI_CTRL_EVENT *)parm;
        memcpy(&mid_dec_obj->protect_trigger, protect_trigger, sizeof(MIDI_CTRL_EVENT));
    } else if (cmd == CMD_MIDI_SET_MARK) {
        MIDI_MARK_PARAM *mark_info = (MIDI_MARK_PARAM *)parm;
        int mark_start = mark_info->mark_start;
        int mark_end = mark_info->mark_end;
        int mark_enable = mark_info->mark_enable;
        if (mark_start <= 0) {
            mark_enable = 0;
        }
        if (mark_start >= mark_end) {
            mark_enable = 0;
        }

        mid_dec_obj->midi_mark_info.mark_start = mark_start;
        mid_dec_obj->midi_mark_info.mark_end = mark_end;
        mid_dec_obj->midi_mark_info.loop_enable = mark_info->loop_enable;
        mid_dec_obj->midi_mark_info.mark_enable = mark_enable;
        mid_dec_obj->midi_mark_info.mark_set = 1;
    }
    return 0;
}

AT_MIDI_SPARSE_CODE
static u32 needMidiPlayersBuf()
{
    return MAX_DEC_PLAYER_CNT * sizeof(MIDI_PLAYER);
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
static u32 needSaveDivBuf()
{
    u32 bufsize;
    bufsize = MAX_GO_BACK * sizeof(MIDI_SAVE_POS_STRUCT);
    return bufsize;
}

AT_MIDI_SPARSE_CODE
static u32 need_dcbuf_size()
{
    int bufsize = sizeof(MIDI_DECODE_VAR);
    bufsize += needMidiPlayersBuf();

    if (MIDI_SAVE_DIV_ENBALE) {
        bufsize += needSaveDivBuf();
    }
    bufsize += needMidiOutBuf();

    return  bufsize;
}

AT_MIDI_SPARSE_CODE
static u32 midi_open(void *work_buf, const struct if_decoder_io *decoder_io, u8 *bk_point_ptr)
{

    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    memset(mid_dec_obj, 0, need_dcbuf_size());

    memcpy((u8 *)&mid_dec_obj->smf_io, (u8 *)decoder_io, sizeof(struct if_decoder_io));

    mid_dec_obj->step = 2;
    mid_dec_obj->midi_tempo_v = 1024;
    for (int i = 0; i < MAX_CHANNEL_NUM; i++) {
        mid_dec_obj->decay_speed[i] = 32768;
    }
    mid_dec_obj->mute_threshold = 1L << 29;

    mid_dec_obj->srTicks = (1L << 18) + (1L << 17);

    int *ptr = mid_dec_obj->mempool;

    mid_dec_obj->out_val = ptr;
    memset(mid_dec_obj->out_val, 0, MIDI_OBUF_BLOCK * 2 * sizeof(int));
    ptr = ptr + MIDI_OBUF_BLOCK * 2;

    mid_dec_obj->out_channel = (short *)ptr;
    memset(mid_dec_obj->out_channel, 0, MIDI_OBUF_BLOCK * 2 * sizeof(short));
    ptr = ptr + (MIDI_OBUF_BLOCK * 2 * sizeof(short) / sizeof(int));

    mid_dec_obj->midi_players = (MIDI_PLAYER *)ptr;
    memset(mid_dec_obj->midi_players, 0, needMidiPlayersBuf());
    ptr = ptr + needMidiPlayersBuf() / sizeof(int);

    if (MIDI_SAVE_DIV_ENBALE) {
        mid_dec_obj->bk_array_ptr = (MIDI_SAVE_POS_STRUCT *)ptr;
        memset(mid_dec_obj->bk_array_ptr, 0, needSaveDivBuf());
        ptr = ptr + (needSaveDivBuf() / sizeof(int));
    }

    for (int i = 0; i < MAX_DEC_PLAYER_CNT; i++) {
        mid_dec_obj->midi_players[i].wave_info.stereo_index = -1;
    }

    return 0;
}

AT_MIDI_CODE
u32 midi_inputdata(MIDI_DECODE_VAR *mid_dec_obj, u32 length)
{
    u32 tmp, res = 0;
    MIDI_INPUT_CONTROL *mic = (MIDI_INPUT_CONTROL *)(&mid_dec_obj->smf_data);
    if (mic->remain >= length) {
        res = ((unsigned int)mic->buf_ptr[mic->rd_pos + 3] << 24) | ((unsigned int)mic->buf_ptr[mic->rd_pos + 2] << 16) | ((unsigned int)mic->buf_ptr[mic->rd_pos + 1] << 8) | (unsigned int)mic->buf_ptr[mic->rd_pos];
        mic->rd_pos += length;
        mic->remain = mic->remain - length;
    } else {
        u32  rdlen;
        if (mic->remain) {
            res = ((unsigned int)mic->buf_ptr[mic->rd_pos + 3] << 24) | ((unsigned int)mic->buf_ptr[mic->rd_pos + 2] << 16) | ((unsigned int)mic->buf_ptr[mic->rd_pos + 1] << 8) | (unsigned int)mic->buf_ptr[mic->rd_pos];
            res = res & ((1 << (mic->remain * 8)) - 1);

        }
        tmp = length - mic->remain;
        mid_dec_obj->smf_data.buf_ptr = mid_dec_obj->smf_data.input_buf;
        rdlen = mid_dec_obj->smf_io.input(mid_dec_obj->smf_io.priv, mid_dec_obj->fpos_cnt, mid_dec_obj->smf_data.input_buf, 512, 0);
        mic->remain = rdlen;
        if (!rdlen) {
            mid_dec_obj->err = MAD_ERROR_FILE_END;
        }
        mid_dec_obj->fpos_cnt += rdlen;

        mic->rd_pos = 0;
        rdlen = ((unsigned int)mic->buf_ptr[mic->rd_pos + 3] << 24) | ((unsigned int)mic->buf_ptr[mic->rd_pos + 2] << 16) | ((unsigned int)mic->buf_ptr[mic->rd_pos + 1] << 8) | (unsigned int)mic->buf_ptr[mic->rd_pos];
        res = res | (rdlen << (length - tmp) * 8);
        mic->rd_pos += tmp;
        mic->remain = mic->remain - tmp;
        if (mid_dec_obj->save_ptr_enable) {
            mid_dec_obj->check_buf_flag = 1;
        }
    }
    if (length < 4) {
        tmp = (1 << (length * 8)) - 1;
        res = res & tmp;
    }

    return res;
}

AT_MIDI_SPARSE_CODE
void midi_skipdata(MIDI_DECODE_VAR *mid_dec_obj, u32 length)
{
}



AT_MIDI_SPARSE_CODE
u32  DATA_REVERSE(u32 data, int i_max)
{
    u32 i, reversed_data = 0;
    for (i = 0; i < i_max; i++) {
        reversed_data = (reversed_data << 8) + (data & 0xff);
        data = data >> 8;
    }
    return reversed_data;
}


AT_MIDI_SPARSE_CODE
static u32 MIDI_FORMAT_CHECK(void *ptr)                 //save track pos, track rd_pos
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)ptr;
    u32 tmpChunkStr, tmpChunckLen, trkNum = 0;
    int err_cnt = 0;
    u16 test_i;

    tmpChunkStr = SMF_MTHD_CHUNK;
    tmpChunkStr = midi_inputdata(mid_dec_obj, 4);

    if (tmpChunkStr != SMF_MTHD_CHUNK) {
        return FORMAT_ERR;
    }
    tmpChunckLen = midi_inputdata(mid_dec_obj, 4);
    if (tmpChunckLen != 0x06000000) {
        return FORMAT_ERR;
    }
    mid_dec_obj->format = DATA_REVERSE(midi_inputdata(mid_dec_obj, 2), 2);
    mid_dec_obj->numTrk = DATA_REVERSE(midi_inputdata(mid_dec_obj, 2), 2);
    mid_dec_obj->tmDiv = DATA_REVERSE(midi_inputdata(mid_dec_obj, 2), 2);

    if (mid_dec_obj->tmDiv > 30) {
        mid_dec_obj->mdiv_cnt = 2;
    }
    if (mid_dec_obj->tmDiv & 0x8000) {
        return FORMAT_ERR;
    }

    trkNum = 0;

    if (mid_dec_obj->numTrk > MAX_TRACK_NUM) {
        mid_dec_obj->numTrk = MAX_TRACK_NUM;
    }

    while (trkNum < mid_dec_obj->numTrk) {
        tmpChunkStr = midi_inputdata(mid_dec_obj, 4);
        if (tmpChunkStr != SMF_MTRK_CHUNK) {
            err_cnt++;

            if (err_cnt > 40) {
                return FORMAT_ERR;
            }

            continue;
        }
        mid_dec_obj->track_len[trkNum] = DATA_REVERSE(midi_inputdata(mid_dec_obj, 4), 4);

        if (err_cnt > 40) {
            return FORMAT_ERR;
        }

        mid_dec_obj->channel_over |= MIDI_BIT_N(trkNum);
        trkNum++;

    }

    mid_dec_obj->dec_info.sr = smpl_rate_tab[mid_dec_obj->sample_rate];
    mid_dec_obj->dec_info.br = 1;


    mid_dec_obj->div_first_cnt = 1;

    memcpy(&mid_dec_obj->mark_array[0], &mid_dec_obj->fpos_cnt, sizeof(MIDI_SAVE_POS_STRUCT) - 8);
    mid_dec_obj->mark_array[0].rd_pos = mid_dec_obj->smf_data.rd_pos;
    mid_dec_obj->mark_array[0].remain = mid_dec_obj->smf_data.remain;

    return 0;
}

AT_MIDI_CODE
static u32 MIDI_DEC_MAIN(void *ptr, u32 type)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)ptr;
    unsigned int trk_num = 0;
    u32 tmpVar;
    u16 msg;
    int block_len;
    int sample_cnt, j;
    int break_flag = 0;
    int midi_play_on = 1;
    int samplePoint = 0;

    if (mid_dec_obj->o_len) {

        unsigned char *obuf;
        if (mid_dec_obj->out_bit == 0) {
            obuf = (unsigned char *)mid_dec_obj->out_channel;
        } else {
            obuf = (unsigned char *)mid_dec_obj->out_val;
        }

        int midi_olen;
        int len_out = mid_dec_obj->o_len;
        midi_olen = mid_dec_obj->smf_io.output(mid_dec_obj->smf_io.priv, &obuf[mid_dec_obj->o_index], len_out);

        if (midi_olen < len_out) {
            mid_dec_obj->o_index += midi_olen;
            mid_dec_obj->o_len = len_out - midi_olen;
            return 0;
        }
        sample_cnt = mid_dec_obj->sample_cnt_backup;
        mid_dec_obj->o_len = 0;
        mid_dec_obj->o_index = 0;
        memset(mid_dec_obj->out_val, 0, 2 * MIDI_OBUF_BLOCK * sizeof(int));
        goto _MIDI_OUTPUT_OK;
    }


okon_mode_loop:

    if (!mid_dec_obj->channel_over) {
        mid_dec_obj->err = MAD_ERROR_FILE_END;

        if (mid_dec_obj->tmDiv_up_cnt) {
            mid_dec_obj->tmDiv_up_cnt = 0;
            if (mid_dec_obj->tmDiv_enable) {
                mid_dec_obj->tmDiv_trig.timeDiv_trigger(mid_dec_obj->tmDiv_trig.priv);
            }
        }
        return mid_dec_obj->err;
    }

    if (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) {
        if (mid_dec_obj->note_on == 2) {
            goto  ctrl_mode_goon;
        } else {
            if (!mid_dec_obj->flag_t) {
                u32 min_delta = 0xffffffff;
                for (trk_num = 0; trk_num < mid_dec_obj->numTrk; trk_num++) {
                    if (mid_dec_obj->track_len[trk_num]) {
                        min_delta = (min_delta <= mid_dec_obj->track_info[trk_num].deltat) ? min_delta : mid_dec_obj->track_info[trk_num].deltat;
                    }
                }
                if (min_delta) {
                    for (trk_num = 0; trk_num < mid_dec_obj->numTrk; trk_num++) {
                        if (mid_dec_obj->track_len[trk_num]) {
                            mid_dec_obj->track_info[trk_num].deltat = mid_dec_obj->track_info[trk_num].deltat - min_delta + 1;
                        }
                    }
                }

            }

        }
    }

    if (mid_dec_obj->midi_mark_info.mark_enable) {
        if (mid_dec_obj->midi_mark_info.mark_set && mid_dec_obj->midi_mark_info.cur_mark_store_pos != mid_dec_obj->midi_mark_info.mark_start) {
            mid_dec_obj->midi_mark_info.mark_set = 0;
            if (mid_dec_obj->midi_mark_info.cur_mark_store_pos > mid_dec_obj->midi_mark_info.mark_start) {
                memcpy(&mid_dec_obj->fpos_cnt, &mid_dec_obj->mark_array[0], sizeof(MIDI_SAVE_POS_STRUCT) - 8);
                mid_dec_obj->fpos_cnt -= mid_dec_obj->mark_array[0].remain;
                mid_dec_obj->smf_data.rd_pos = 0;
                mid_dec_obj->smf_data.remain = 0;
                mid_dec_obj->midi_mark_info.cur_mark_store_pos = 0;
                mid_dec_obj->midi_mark_info.cur_mark_pos = 0;

                WaveInfo_t *temp_t;
                for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                    temp_t = &(mid_dec_obj->midi_players[i].wave_info);
                    if (temp_t->player_on) {
                        {
                            control_stop(mid_dec_obj, temp_t, temp_t->chnl);
                        }
                    }
                }
            } else if (mid_dec_obj->midi_mark_info.cur_mark_pos > mid_dec_obj->midi_mark_info.mark_start) {
                memcpy(&mid_dec_obj->fpos_cnt, &mid_dec_obj->mark_array[1], sizeof(MIDI_SAVE_POS_STRUCT) - 8);
                mid_dec_obj->fpos_cnt -= mid_dec_obj->mark_array[1].remain;
                mid_dec_obj->smf_data.rd_pos = 0;
                mid_dec_obj->smf_data.remain = 0;

                mid_dec_obj->midi_mark_info.cur_mark_pos = mid_dec_obj->midi_mark_info.cur_mark_store_pos - 1;
                WaveInfo_t *temp_t;
                for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                    temp_t = &(mid_dec_obj->midi_players[i].wave_info);
                    if (temp_t->player_on) {
                        {
                            control_stop(mid_dec_obj, temp_t, temp_t->chnl);
                        }
                    }
                }
            }
        }

        if (mid_dec_obj->midi_mark_info.loop_enable  && mid_dec_obj->midi_mark_info.cur_mark_pos >= mid_dec_obj->midi_mark_info.mark_end) {
            memcpy(&mid_dec_obj->fpos_cnt, &mid_dec_obj->mark_array[1], sizeof(MIDI_SAVE_POS_STRUCT) - 8);
            mid_dec_obj->fpos_cnt -= mid_dec_obj->mark_array[1].remain;
            mid_dec_obj->smf_data.rd_pos = 0;
            mid_dec_obj->smf_data.remain = 0;

            mid_dec_obj->midi_mark_info.cur_mark_pos = mid_dec_obj->midi_mark_info.cur_mark_store_pos - 1;

            WaveInfo_t *temp_t;
            for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
                temp_t = &(mid_dec_obj->midi_players[i].wave_info);
                if (temp_t->player_on) {
                    {
                        control_stop(mid_dec_obj, temp_t, temp_t->chnl);
                    }
                }
            }
        }
    }
    memcpy(&mid_dec_obj->mark_array[2], &mid_dec_obj->fpos_cnt, sizeof(MIDI_SAVE_POS_STRUCT) - 8);
    mid_dec_obj->mark_array[2].rd_pos = mid_dec_obj->smf_data.rd_pos;
    mid_dec_obj->mark_array[2].remain = mid_dec_obj->smf_data.remain;

    if (mid_dec_obj->okon_goon_flag) {
        trk_num = mid_dec_obj->okon_trk_num;
        mid_dec_obj->okon_goon_flag = 0;
    } else {
        trk_num = 0;
    }

    for (; trk_num < mid_dec_obj->numTrk; trk_num++) {
        block_len = mid_dec_obj->track_len[trk_num];
        break_flag = 0;

        while (1) {
            if (mid_dec_obj->channel_over & MIDI_BIT_N(trk_num)) {
                u32 deltaT = 0;

                if (break_flag) {
                    break;
                }

                if (mid_dec_obj->track_info[trk_num].deltat == 0) {
                    if (block_len <= 0) {
                        mid_dec_obj->channel_over &= (~MIDI_BIT_N(trk_num));
                        break;
                    }

                    do {
                        if (mid_dec_obj->saved_rd) {
                            tmpVar = mid_dec_obj->value_rd;
                            mid_dec_obj->saved_rd = 0;
                            deltaT = (deltaT << 7) + (tmpVar & 0x7f);
                        } else {
                            tmpVar = midi_inputdata(mid_dec_obj, 1);
                            deltaT = (deltaT << 7) + (tmpVar & 0x7f);
                            block_len = block_len - 1;
                        }

                    } while (tmpVar & 0x80);

                    mid_dec_obj->track_info[trk_num].deltat = deltaT;

                    if (mid_dec_obj->peekOK) {
                        WaveInfo_t *tmpwavinfo = &mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info;
                        mid_dec_obj->peekOK = 0;

                        if ((tmpwavinfo->vol_hold_new != 0) || (tmpwavinfo->vol_dec_now != tmpwavinfo->vol_rel)) {
                            MIDI_INPUT_CONTROL *mic = (MIDI_INPUT_CONTROL *)(&mid_dec_obj->smf_data);
                            u8 *rdptr = &mic->buf_ptr[mic->rd_pos];
                            u8 cycle_cnt = 0;
                            u8 lastMsg = mid_dec_obj->lastMsg[trk_num];
                            u8 foundflag = 0;

_MIDI_OKON_SEEKING:
                            msg = rdptr[0];
                            if (msg & MIDI_MSGBIT) {
                                lastMsg = msg;
                                rdptr++;
                            } else {
                                msg = lastMsg;
                            }

                            msg = msg & MIDI_EVTMSK;

                            switch (msg) {
                            case MIDI_NOTEON:
                                if ((rdptr[0] == tmpwavinfo->noKey) && (rdptr[1] == 0)) {
                                    foundflag = 1;
                                }
                                rdptr += 2;
                                break;
                            case MIDI_NOTEOFF: {
                                if (rdptr[0] == tmpwavinfo->noKey) {
                                    foundflag = 1;
                                }
                            }
                            case MIDI_KEYAT:
                            case MIDI_CTLCHG:
                            case MIDI_PWCHG:
                                rdptr += 2;
                                break;
                            case MIDI_PRGCHG:
                            case MIDI_CHNAT:
                                rdptr++;
                                break;
                            default:
                                break;
                            }

                            if (foundflag == 0) {
                                u32 deltaNew = 0;
                                do {
                                    tmpVar = *rdptr++;
                                    cycle_cnt++;
                                    if (cycle_cnt > 16) {
                                        tmpVar = 0;
                                        foundflag = 1;
                                    }
                                    deltaNew = (deltaNew << 7) + (tmpVar & 0x7f);
                                } while (tmpVar & 0x80);

                                deltaT += deltaNew;
                                goto _MIDI_OKON_SEEKING;

                            }

                            mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info.note_on_off_dest = deltaT;
                            if (mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info.note_on_off_dest > 2000) {
                                mid_dec_obj->midi_players[mid_dec_obj->player_index].wave_info.note_on_off_dest = 2000;
                            }

                        }
                    }
                }

                if (mid_dec_obj->track_info[trk_num].deltat) {
                    mid_dec_obj->track_info[trk_num].deltat--;
                    break_flag = 1;
                }

                if (mid_dec_obj->track_info[trk_num].deltat == 0) {
                    msg = midi_inputdata(mid_dec_obj, 1);
                    block_len = block_len - 1;

                    if ((msg & MIDI_EVTMSK) == MIDI_SYSEX) {
                        if (msg == MIDI_META) {
                            block_len = MetaEvent(mid_dec_obj, trk_num, block_len);
                        } else {
                            block_len = SysCommon(mid_dec_obj, msg, block_len);
                        }

                    } else {
                        if (msg & MIDI_MSGBIT) {
                            mid_dec_obj->lastMsg[trk_num] = msg;
                        } else {
                            mid_dec_obj->value_rd = msg;
                            mid_dec_obj->saved_rd = 1;
                            msg = mid_dec_obj->lastMsg[trk_num];
                        }

                        block_len = ChnlMessage(mid_dec_obj, trk_num, msg, block_len);
                        if ((mid_dec_obj->note_on == 2) && (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1)) {
                            if (break_flag) {
                                mid_dec_obj->okon_trk_num = trk_num + 1;
                            } else {
                                mid_dec_obj->okon_trk_num = trk_num;
                            }
                            mid_dec_obj->track_len[trk_num] = block_len;
                            goto ctrl_mode_goon;
                        }
                    }

                }

            } else {
                break;
            }
        }

        mid_dec_obj->track_len[trk_num] = block_len;
    }

ctrl_mode_goon:

    for (int i = 0; i < mid_dec_obj->MAX_PLAYER_CNTt; i++) {
        if (mid_dec_obj->midi_players[i].wave_info.player_on) {
            midi_play_on = 0;
            break;
        }
    }

    if ((mid_dec_obj->midi_mark_info.mark_enable) || ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) && (mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_2))) {
        if ((midi_play_on) && (mid_dec_obj->wdt_count)) {
            if (mid_dec_obj->wdt_count >= mid_dec_obj->wdt_clear.count) {
                samplePoint = mid_dec_obj->wdt_clear.wdt_clear_trigger(mid_dec_obj->wdt_clear.priv);
                mid_dec_obj->wdt_count = 1;
                if (samplePoint == 0) {
                    goto okon_mode_loop;
                }

            } else {
                goto okon_mode_loop;
            }
        } else {
            mid_dec_obj->wdt_count = 0;
        }
    }

    if (((mid_dec_obj->midi_mark_info.mark_enable) || (mid_dec_obj->okon_mode == CMD_MIDI_OKON_MODE_2)) && (midi_play_on)) {
        if (mid_dec_obj->wdt_count) {
            sample_cnt = samplePoint;
            mid_dec_obj->out_total += sample_cnt;
        } else {
            mid_dec_obj->theTick += mid_dec_obj->now_srTicks;
            sample_cnt = mid_dec_obj->theTick >> MIDI_RESAMPLE_SHIFT;
            mid_dec_obj->theTick &= MIDI_RESAMPLE_VAL;
            mid_dec_obj->out_total += sample_cnt;
        }
    } else {
        mid_dec_obj->theTick += mid_dec_obj->now_srTicks;
        sample_cnt = mid_dec_obj->theTick >> MIDI_RESAMPLE_SHIFT;
        mid_dec_obj->theTick &= MIDI_RESAMPLE_VAL;
        mid_dec_obj->out_total += sample_cnt;
    }



    if (mid_dec_obj->note_on == 2) {
        if (mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) {
            int test_i;
            long long tmp64;
            for (test_i = 0; test_i < mid_dec_obj->MAX_PLAYER_CNTt; test_i++) {
                WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[test_i].wave_info);
                if (waveInfo->player_on) {
                    waveInfo->note_on_off_cnt++;
                    if (waveInfo->note_on_off_cnt == waveInfo->note_on_off_dest) {
                        waveInfo->vol_hold_new = 0;
                        waveInfo->vol_dec_now = waveInfo->vol_rel;
                        if (waveInfo->env_info.env_use) {
                            waveInfo->env_info.env_use = 0;
                            MULSI(waveInfo->vol_now, tmp64, waveInfo->vol_now, waveInfo->env_info.vol_f, 30);
                        }
                    }

                }
            }
        }
    }


    {
        int vol_threshold_flag = 0;
        int midi_olen;
        char main_player_on;

_MIDI_OUTPUT_OK:

        while (sample_cnt) {

            int len_out = (sample_cnt > MIDI_OBUF_BLOCK) ? MIDI_OBUF_BLOCK : sample_cnt;
            long long tmp64;
            for (j = 0; j < mid_dec_obj->MAX_PLAYER_CNTt; j++) {
                WaveInfo_t *waveInfo = &(mid_dec_obj->midi_players[j].wave_info);
                if (waveInfo->player_on) {
                    int tmp_r;
                    int maintrackval = waveInfo->trk_v;
                    if (MAINTRACK_USE_CHN) {
                        maintrackval = waveInfo->chnl;
                    }

                    {
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
                                vol_threshold_flag = 1;

                            } else {
                                if (waveInfo->vol_hold_flag) {
                                    waveInfo->vol_hold_flag--;
                                    MULSI(tmp_r, tmp64, waveInfo->vol_now, waveInfo->vol_atten, 29);
                                    waveInfo->vol_array[tmp_i] = tmp_r;
                                } else if (waveInfo->env_info.env_use) {
                                    ENV *env_info = &waveInfo->env_info;
                                    u8 *env_infov = (u8 *)env_info->env_infov;
                                    if (env_info->stepcnt > 0) {
                                        int tmp32 = env_info->vol_f + env_info->stepval;   //������0��1����ֹ��������
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
                                        int doPoints = smpl_rate_tab[mid_dec_obj->sample_rate] * 5 / 1000;
                                        if (env_info->env_points > 1) {  //������>1����Ϊ�����points��ĸ��������ɵĵ���һ��������ʵ����ɫ������ֻ����delta�����Ի�ȹ������ɵĵ�����һ����
                                            u32 valget = 0;
                                            u8 readval = env_infov[env_info->envi++];
                                            valget = (readval & 0x7f);

                                            short tmpshift = 7;
                                            while (!(readval & 0x80)) {
                                                readval = env_infov[env_info->envi++];
                                                valget = ((readval & 0x7f) << tmpshift) | valget;
                                                tmpshift += 7;
                                            }
                                            int stepcnt = valget;                       //��ȡ��һ��cnt

                                            readval = env_infov[env_info->envi++];
                                            valget = env_infov[env_info->envi++];
                                            valget = (valget << 8) | readval;
                                            short tmpste = (short)(valget & 0xffff);   //��ȡ��һ��step

                                            env_info->stepval = tmpste  * (1 << 15) / doPoints;
                                            env_info->stepcnt = stepcnt * doPoints;          //������5ms�����Ը��ݲ������������

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
                            if (midi_dec_gen_sample(mid_dec_obj, j, len_out)) {
                                if ((!waveInfo->pendingStop) && (waveInfo->voice.isend)) {
                                    waveInfo->exclOn = 0;
                                    if (waveInfo->stereo_index >= 0) {
                                        mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                                    }

                                    waveInfo->player_on = 0;

                                    if (mid_dec_obj->melody_stop) {
                                        if (mid_dec_obj->melody_stop_trig.main_chn_enable) {
                                            if ((maintrackval == mid_dec_obj->tracknV) && (waveInfo->stero_channel == 0)) {
                                                mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                            }
                                        } else {
                                            if (waveInfo->stero_channel == 0) {
                                                mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                            }
                                        }
                                    }

                                    waveInfo->stereo_index = -1;
                                }
                            }
                        } else if (waveInfo->ison == SEQ_AE_REL) {
                            if ((!waveInfo->pendingStop) && (waveInfo->voice.isend)) {
                                waveInfo->exclOn = 0;
                                if (waveInfo->stereo_index >= 0) {
                                    mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                                }

                                waveInfo->player_on = 0;

                                if (mid_dec_obj->melody_stop) {
                                    if (mid_dec_obj->melody_stop_trig.main_chn_enable) {
                                        if ((maintrackval == mid_dec_obj->tracknV) && (waveInfo->stero_channel == 0)) {
                                            mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                        }
                                    } else {
                                        if (waveInfo->stero_channel == 0) {
                                            mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                        }
                                    }
                                }
                                waveInfo->stereo_index = -1;
                            } else {
                                {
                                    midi_dec_gen_sample(mid_dec_obj, j, len_out);
                                    if (waveInfo->voice.isend) {
                                        if (waveInfo->stereo_index >= 0) {
                                            mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                                        }

                                        waveInfo->player_on = 0;
                                        if (mid_dec_obj->melody_stop) {
                                            if (mid_dec_obj->melody_stop_trig.main_chn_enable) {
                                                if ((maintrackval == mid_dec_obj->tracknV) && (waveInfo->stero_channel == 0)) {
                                                    mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                                }
                                            } else {
                                                if (waveInfo->stero_channel == 0) {
                                                    mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                                }
                                            }
                                        }
                                        waveInfo->stereo_index = -1;
                                    }
                                }

                            }
                        }

                    }

                    if (waveInfo->vol_now < 32768) {
                        if (waveInfo->stereo_index >= 0) {
                            mid_dec_obj->midi_players[waveInfo->stereo_index].wave_info.stereo_index = -1;
                        }

                        waveInfo->player_on = 0;
                        if (mid_dec_obj->melody_stop) {
                            if (mid_dec_obj->melody_stop_trig.main_chn_enable) {
                                if ((maintrackval == mid_dec_obj->tracknV) && (waveInfo->stero_channel == 0)) {
                                    mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                }
                            } else {
                                if (waveInfo->stero_channel == 0) {
                                    mid_dec_obj->melody_stop_trig.melody_stop_trigger(mid_dec_obj->melody_stop_trig.priv, waveInfo->noKey, waveInfo->chnl);
                                }
                            }
                        }
                        waveInfo->stereo_index = -1;
                    } else if (waveInfo->vol_now > mid_dec_obj->mute_threshold) {
                        vol_threshold_flag = 1;
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
                    midi_olen = mid_dec_obj->smf_io.output(mid_dec_obj->smf_io.priv, mid_dec_obj->out_channel, out_byte);
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
                    midi_olen = mid_dec_obj->smf_io.output(mid_dec_obj->smf_io.priv, mid_dec_obj->out_val, out_byte);
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

        if (vol_threshold_flag == 0) {
            mid_dec_obj->keydown_cnt = 0;
        }

        if (mid_dec_obj->o_len) {
            return 0;
        }
    }

MIDI_MAIN_DEC_OUT:
    mid_dec_obj->mdiv_cnt++;

    if (mid_dec_obj->mdiv_cnt >= mid_dec_obj->tmDiv) {
        mid_dec_obj->mdiv_cnt = 0;

        mid_dec_obj->tmDiv_up_cnt++;
        if (mid_dec_obj->tmDiv_up_cnt == mid_dec_obj->tmDiv_up_dest) {

            if (mid_dec_obj->save_ptr_enable) {
                midi_save_bk_fun(mid_dec_obj);
            }

            mid_dec_obj->tmDiv_up_cnt = 0;
            if (mid_dec_obj->tmDiv_enable) {
                mid_dec_obj->tmDiv_trig.timeDiv_trigger(mid_dec_obj->tmDiv_trig.priv);
            }

        }

        if (mid_dec_obj->beat_enable) {
            mid_dec_obj->beat_trig.beat_trigger(mid_dec_obj->beat_trig.priv, mid_dec_obj->tmDiv_up_dest, mid_dec_obj->several_notes);
        }
    }

    if (mid_dec_obj->melody_on) {
        if (mid_dec_obj->keydown_cnt == 0) {
            if (mid_dec_obj->mute_flag == 0) {
                mid_dec_obj->mute_flag = 1;
                mid_dec_obj->melody_trig.melody_trigger(mid_dec_obj->melody_trig.priv, 255, 0);
            }
        } else {
            mid_dec_obj->mute_flag = 0;
        }

    }
    return 0;

}

AT_MIDI_SPARSE_CODE
void  midi_play_ctrl_on(void *work_buf, MIDI_PLAY_CTRL_MODE *mode_obj)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;

    mid_dec_obj->crtl_mode = mode_obj->mode;
    mid_dec_obj->okon_goon_flag = 0;
    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) && (mid_dec_obj->key_mode == CMD_MIDI_MELODY_KEY_0)) {
        mid_dec_obj->note_on = 2;
        mid_dec_obj->okon_start_flag = 1;
    }

    if ((mid_dec_obj->crtl_mode == CMD_MIDI_CTRL_MODE_1) && (mid_dec_obj->key_mode == CMD_MIDI_MELODY_KEY_1)) {
        mid_dec_obj->note_on = 1;
        mid_dec_obj->okon_start_flag = 0;
    }
}

AT_MIDI_SPARSE_CODE
static void MIDI_set_step(void *work_buf, u32 step)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    mid_dec_obj->step = step;
}

AT_MIDI_SPARSE_CODE
static void MIDI_set_err_info(void *work_buf, u32 cmd, u8 *ptr, u32 size)
{
}

AT_MIDI_SPARSE_CODE
static u32 midi_get_time(void *work_buf)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    return mid_dec_obj->out_total / smpl_rate_tab[mid_dec_obj->sample_rate];
}

AT_MIDI_SPARSE_CODE
static u32 get_bp_inf(void *work_buf)
{
    return 0;
}

AT_MIDI_SPARSE_CODE
static dec_inf_t *get_dec_inf(void *work_buf)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    return  &(mid_dec_obj->dec_info);
}

AT_MIDI_SPARSE_CODE
static u32 need_bpbuf_size()
{
    return 0;
}

AT_MIDI_SPARSE_CODE
static u32 need_rdbuf_size()
{
    return INBUF_SIZE;
}

AT_MIDI_SPARSE_CONST
const audio_decoder_ops midi_decoder_ops = {
    /*.name              = */ "MID",
    /*.open              = */ midi_open,
    /*.format_check      = */ MIDI_FORMAT_CHECK,
    /*.run               = */ MIDI_DEC_MAIN,
    /*.get_dec_inf       = */ get_dec_inf,
    /*.get_playtime      = */ midi_get_time,
    /*.get_bp_inf        = */ get_bp_inf,
    /*.need_dcbuf_size   = */ need_dcbuf_size,
    /*.need_rdbuf_size   = */ need_rdbuf_size,
    /*.need_bpbuf_size   = */ need_bpbuf_size,
    /*.set step          = */ MIDI_set_step,
    /*.set error info    = */ MIDI_set_err_info,
    /*.dec_confing       = */midi_dec_confing
};

AT_MIDI_SPARSE_CODE
audio_decoder_ops *get_midi_ops()
{
    return (audio_decoder_ops *)(&midi_decoder_ops);
}

AT_MIDI_SPARSE_CODE
int get_midi_tone_compressor(void *work_buf)
{
    MIDI_DECODE_VAR *mid_dec_obj = (MIDI_DECODE_VAR *)work_buf;
    return mid_dec_obj->headSize;
}








