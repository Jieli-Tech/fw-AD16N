#ifndef __MIDI_PROG_H__
#define __MIDI_PROG_H__

#define  MIDI_PROG_ACOUSTIC_GRAND_PIANO                     0	//大钢琴
#define  MIDI_PROG_Bright_Acoustic_Piano                    1	//亮音大钢琴
#define  MIDI_PROG_ELECTRIC_GRAND_PIANO                     2	//电钢琴
#define  MIDI_PROG_HONKY_TONK_PIANO                         3	//酒吧钢琴
#define  MIDI_PROG_RHODES_PIANO_PIANO                       4	//练习音钢琴
#define  MIDI_PROG_CHORUSED_PIANO                           5	//合唱加钢琴
#define  MIDI_PROG_HARPSICHORD_PIANO                        6	//拨弦古钢琴
#define  MIDI_PROG_CLAVINET_PIANO                           7	//击弦古钢琴
#define  MIDI_PROG_CELESTA_CHROMATIC_PERCUSSION             8	//钢片琴
#define  MIDI_PROG_GLOCKENSPIEL_CHROMATIC_PERCUSSION        9	//钟琴
#define  MIDI_PROG_MUSIC_BOX_CHROMATIC_PERCUSSION           10	//八音盒
#define  MIDI_PROG_VIBRAPHONE_CHROMATIC_PERCUSSION          11	//电颤琴
#define  MIDI_PROG_MARIMBA_CHROMATIC_PERCUSSION             12	//马林巴
#define  MIDI_PROG_XYLOPHONE_CHROMATIC_PERCUSSION           13	//木琴
#define  MIDI_PROG_TUBULAR_BELLS_CHROMATIC_PERCUSSION       14	//管钟
#define  MIDI_PROG_DULCIMER_CHROMATIC_PERCUSSION            15	//扬琴
#define  MIDI_PROG_HAMMOND_ORGAN                            16	//击杆风琴
#define  MIDI_PROG_PERCUSSIVE_ORGAN                         17	//打击型风琴
#define  MIDI_PROG_ROCK_ORGAN                               18	//摇滚风琴
#define  MIDI_PROG_CHURCH_ORGAN                             19	//管风琴
#define  MIDI_PROG_REED_ORGAN                               20	//簧风琴
#define  MIDI_PROG_ACCORDION_ORGAN                          21	//手风琴
#define  MIDI_PROG_HARMONICA_ORGAN                          22	//口琴
#define  MIDI_PROG_TANGO_ACCORDIAN_ORGAN                    23	//探戈手风琴
#define  MIDI_PROG_NYLON_ACOUSTIC_GUITAR                    24	//尼龙弦吉他
#define  MIDI_PROG_STEEL_ACOUSTIC_GUITA                     25	//钢弦吉他
#define  MIDI_PROG_JAZZ_ELECTRIC_GUITA                      26	//爵士乐电吉他
#define  MIDI_PROG_CLEAN_ELECTRIC_GUITA                     27	//清音电吉他
#define  MIDI_PROG_MUTED_ELECTRIC_GUITA                     28	//弱音电吉他
#define  MIDI_PROG_OVERDRIVEN_GUITAR                        29	//驱动音效吉他
#define  MIDI_PROG_DISTORTION_GUITAR                        30	//失真音效吉他
#define  MIDI_PROG_HARMONICS_GUITAR                         31	//吉他泛音
#define  MIDI_PROG_ACOUSTIC_BASS                            32	//原声贝司
#define  MIDI_PROG_ELECTRIC_FINGER_BASS                     33	//指拨电贝司
#define  MIDI_PROG_ELECTRIC_PICK_BASS                       34	//拨片拨电贝司
#define  MIDI_PROG_FRETLESS_BASS                            35	//无品贝司
#define  MIDI_PROG_SLAP_BASS_1                              36	//击弦贝司1
#define  MIDI_PROG_SLAP_BASS_2                              37	//击弦贝司2
#define  MIDI_PROG_SYNTH_BASS_1                             38	//合成贝司1
#define  MIDI_PROG_SYNTH_BASS_2                             39	//合成贝司2
#define  MIDI_PROG_VIOLIN_SOLO_STRINGS                      40	//小提琴
#define  MIDI_PROG_VIOLA_SOLO_STRINGS                       41	//中提琴
#define  MIDI_PROG_CELLO_SOLO_STRINGS                       42	//大提琴
#define  MIDI_PROG_CONTRABASS_SOLO_STRINGS                  43	//低音提琴
#define  MIDI_PROG_TREMOLO_STRINGS_SOLO_STRINGS             44	//弦乐震音
#define  MIDI_PROG_PIZZICATO_STRINGS_SOLO_STRINGS           45	//弦乐拨奏
#define  MIDI_PROG_ORCHESTRAL_HARP_SOLO_STRINGS             46	//竖琴
#define  MIDI_PROG_TIMPANI_SOLO_STRINGS                     47	//定音鼓
#define  MIDI_PROG_STRING_ENSEMBLE_1                        48	//弦乐合奏1
#define  MIDI_PROG_STRING_ENSEMBLE_2                        49	//弦乐合奏2
#define  MIDI_PROG_SYNTHSTRINGS_ENSEMBLE_1                  50	//合成弦乐1
#define  MIDI_PROG_SYNTHSTRINGS_ENSEMBLE_2                  51	//合成弦乐2
#define  MIDI_PROG_CHOIR_AAHS_ENSEMBLE                      52	//合唱“啊”音
#define  MIDI_PROG_VOICE_OOHS_ENSEMBLE                      53	//人声“嘟”音
#define  MIDI_PROG_SYNTH_VOICE_ENSEMBLE                     54	//合成人声
#define  MIDI_PROG_ORCHESTRA_HIT_ENSEMBLE                   55	//乐队打击乐
#define  MIDI_PROG_TRUMPET_BRASS                            56	//小号
#define  MIDI_PROG_TROMBONE_BRASS                           57	//长号
#define  MIDI_PROG_TUBA_BRASS                               58	//大号
#define  MIDI_PROG_MUTED_TRUMPET_BRASS                      59	//弱音小号
#define  MIDI_PROG_FRENCH_HORN_BRASS                        60	//圆号
#define  MIDI_PROG_BRASS_SECTION_BRASS                      61	//铜管组
#define  MIDI_PROG_SYNTH_BRASS_1                            62	//合成铜管1
#define  MIDI_PROG_SYNTH_BRASS_2                            63	//合成铜管2
#define  MIDI_PROG_SOPRANO_SAX_REED                         64	//高音萨克斯
#define  MIDI_PROG_ALTO_SAX_REED                            65	//中音萨克斯
#define  MIDI_PROG_TENOR_SAX_REED                           66	//次中音萨克斯
#define  MIDI_PROG_BARITONE_SAX_REED                        67	//上低音萨克斯
#define  MIDI_PROG_OBOE_REED                                68	//双簧管
#define  MIDI_PROG_ENGLISH_HORN_REED                        69	//英国管
#define  MIDI_PROG_BASSOON_REED                             70	//大管
#define  MIDI_PROG_CLARINET_REED                            71	//单簧管
#define  MIDI_PROG_PICCOLO_PIPE                             72	//短笛
#define  MIDI_PROG_FLUTE_PIPE                               73	//长笛
#define  MIDI_PROG_RECORDER_PIPE                            74	//竖笛
#define  MIDI_PROG_PAN_FLUTE_PIPE                           75	//排笛
#define  MIDI_PROG_BOTTLE_BLOW_PIPE                         76	//吹瓶口
#define  MIDI_PROG_SKAKUHACHI_PIPE                          77	//尺八
#define  MIDI_PROG_WHISTLE_PIPE                             78	//哨
#define  MIDI_PROG_OCARINA_PIPE                             79	//洋埙
#define  MIDI_PROG_SQUARE_LEAD                              80	//合成主音1（方波）
#define  MIDI_PROG_SAWTOOTH_LEAD                            81	//合成主音2（锯齿波）
#define  MIDI_PROG_CALLIOPE_LEAD                            82	//合成主音3（汽笛风琴）
#define  MIDI_PROG_CHIFF_LEAD                               83	//合成主音4（吹管）
#define  MIDI_PROG_CHARANG_LEAD                             84	//合成主音5（吉他）
#define  MIDI_PROG_VOICE_LEAD                               85	//合成主音6（人声）
#define  MIDI_PROG_FIFTHS_LEAD                              86	//合成主音7（五度）
#define  MIDI_PROG_BASS_LEAD                                87	//合成主音8（低音加主音）
#define  MIDI_PROG_NEW_AGE_SYNTH_PAD                        88	//合成柔音1（新时代）
#define  MIDI_PROG_WARM_SYNTH_PAD                           89	//合成柔音2（暖音）
#define  MIDI_PROG_POLYSYNTH_SYNTH_PAD                      90	//合成柔音3（复合成）
#define  MIDI_PROG_CHOIR_SYNTH_PAD                          91	//合成柔音4（合唱）
#define  MIDI_PROG_BOWED_SYNTH_PAD                          92	//合成柔音5（弓弦）
#define  MIDI_PROG_METALLIC_SYNTH_PAD                       93	//合成柔音6（金属）
#define  MIDI_PROG_HALO_SYNTH_PAD                           94	//合成柔音7（光环）
#define  MIDI_PROG_SWEEP_SYNTH_PAD                          95	//合成柔音8（扫弦）
#define  MIDI_PROG_RAIN_SYNTH_EFFECTS                       96	//合成特效1（雨）
#define  MIDI_PROG_SOUNDTRACK_SYNTH_EFFECTS                 97	//合成特效2（音轨）
#define  MIDI_PROG_CRYSTAL_SYNTH_EFFECTS                    98	//合成特效3（水晶）
#define  MIDI_PROG_ATMOSPHERE_SYNTH_EFFECTS                 99	//合成特效4（大气）
#define  MIDI_PROG_BRIGHTNESS_SYNTH_EFFECTS                 100	//合成特效5（亮音）
#define  MIDI_PROG_GOBLINS_SYNTH_EFFECTS                    101	//合成特效6（小妖）
#define  MIDI_PROG_ECHOES_SYNTH_EFFECTS                     102	//合成特效7（回声）
#define  MIDI_PROG_SCI_FI_SYNTH_EFFECTS                     103	//合成特效8（科幻）
#define  MIDI_PROG_SITAR_ETHNIC                             104	//锡塔尔
#define  MIDI_PROG_BANJO_ETHNIC                             105	//班卓
#define  MIDI_PROG_SHAMISEN_ETHNIC                          106	//三味线
#define  MIDI_PROG_KOTO_ETHNIC                              107	//筝
#define  MIDI_PROG_KALIMBA_ETHNIC                           108	//卡林巴
#define  MIDI_PROG_BAGPIPE_ETHNIC                           109	//风笛
#define  MIDI_PROG_FIDDLE_ETHNIC                            110	//古提琴
#define  MIDI_PROG_SHANAI_ETHNIC                            111	//唢呐
#define  MIDI_PROG_TINKLE_BELL_PERCUSSIVE                   112	//铃铛
#define  MIDI_PROG_AGOGO_PERCUSSIVE                         113	//拉丁打铃
#define  MIDI_PROG_STEEL_DRUMS_PERCUSSIVE                   114	//钢鼓
#define  MIDI_PROG_WOODBLOCK_PERCUSSIVE                     115	//木块
#define  MIDI_PROG_TAIKO_DRUM_PERCUSSIVE                    116	//太鼓
#define  MIDI_PROG_MELODIC_TOM_PERCUSSIVE                   117	//嗵鼓
#define  MIDI_PROG_SYNTH_DRUM_PERCUSSIVE                    118	//合成鼓
#define  MIDI_PROG_REVERSE_CYMBAL_PERCUSSIVE                119	//镲波形反转
#define  MIDI_PROG_GUITAR_FRET_NOISE_SOUND_EFFECTS          120	//磨弦声
#define  MIDI_PROG_BREATH_NOISE_SOUND_EFFECTS               121	//呼吸声
#define  MIDI_PROG_SEASHORE_SOUND_EFFECTS                   122	//海浪声
#define  MIDI_PROG_BIRD_TWEET_SOUND_EFFECTS                 123	//鸟鸣声
#define  MIDI_PROG_TELEPHONE_RING_SOUND_EFFECTS             124	//电话铃声
#define  MIDI_PROG_HELICOPTER_SOUND_EFFECTS                 125	//直升机声
#define  MIDI_PROG_APPLAUSE_SOUND_EFFECTS                   126	//鼓掌声
#define  MIDI_PROG_GUNSHOT_SOUND_EFFECTS                    127	//枪声　　
#define  MIDI_PROG_PERCUSSION_STANDARD                      128
#define  MIDI_PROG_PERCUSSION_01                            129
#define  MIDI_PROG_PERCUSSION_02                            130
#define  MIDI_PROG_PERCUSSION_03                            131
#define  MIDI_PROG_PERCUSSION_04                            132
#define  MIDI_PROG_PERCUSSION_05                            133
#define  MIDI_PROG_PERCUSSION_06                            134
#define  MIDI_PROG_PERCUSSION_07                            135
#define  MIDI_PROG_PERCUSSION_ROOM                          136
#define  MIDI_PROG_PERCUSSION_11                            137
#define  MIDI_PROG_PERCUSSION_12                            138
#define  MIDI_PROG_PERCUSSION_13                            139
#define  MIDI_PROG_PERCUSSION_14                            140
#define  MIDI_PROG_PERCUSSION_15                            141
#define  MIDI_PROG_PERCUSSION_16                            142
#define  MIDI_PROG_PERCUSSION_17                            143
#define  MIDI_PROG_PERCUSSION_POWER                         144
#define  MIDI_PROG_PERCUSSION_21                            145
#define  MIDI_PROG_PERCUSSION_22                            146
#define  MIDI_PROG_PERCUSSION_23                            147
#define  MIDI_PROG_PERCUSSION_24                            148
#define  MIDI_PROG_PERCUSSION_25                            149
#define  MIDI_PROG_PERCUSSION_26                            150
#define  MIDI_PROG_PERCUSSION_27                            151
#define  MIDI_PROG_PERCUSSION_ELECTRONIC                    152
#define  MIDI_PROG_PERCUSSION_31                            153
#define  MIDI_PROG_PERCUSSION_32                            154
#define  MIDI_PROG_PERCUSSION_33                            155
#define  MIDI_PROG_PERCUSSION_34                            156
#define  MIDI_PROG_PERCUSSION_35                            157
#define  MIDI_PROG_PERCUSSION_36                            158
#define  MIDI_PROG_PERCUSSION_37                            159
#define  MIDI_PROG_PERCUSSION_Jazz                          160
#define  MIDI_PROG_PERCUSSION_41                            161
#define  MIDI_PROG_PERCUSSION_42                            162
#define  MIDI_PROG_PERCUSSION_43                            163
#define  MIDI_PROG_PERCUSSION_44                            164
#define  MIDI_PROG_PERCUSSION_45                            165
#define  MIDI_PROG_PERCUSSION_46                            166
#define  MIDI_PROG_PERCUSSION_47                            167
#define  MIDI_PROG_PERCUSSION_BRUSH                         168
#define  MIDI_PROG_PERCUSSION_51                            169
#define  MIDI_PROG_PERCUSSION_52                            170
#define  MIDI_PROG_PERCUSSION_53                            171
#define  MIDI_PROG_PERCUSSION_54                            172
#define  MIDI_PROG_PERCUSSION_55                            173
#define  MIDI_PROG_PERCUSSION_56                            174
#define  MIDI_PROG_PERCUSSION_57                            175
#define  MIDI_PROG_PERCUSSION_ORCHESTRA                     176
#define  MIDI_PROG_PERCUSSION_61                            177
#define  MIDI_PROG_PERCUSSION_62                            178
#define  MIDI_PROG_PERCUSSION_63                            179
#define  MIDI_PROG_PERCUSSION_64                            180
#define  MIDI_PROG_PERCUSSION_65                            181
#define  MIDI_PROG_PERCUSSION_66                            182
#define  MIDI_PROG_PERCUSSION_67                            183
#define  MIDI_PROG_PERCUSSION_SFX                           184
#define  MIDI_PROG_PERCUSSION_71                            185
#define  MIDI_PROG_PERCUSSION_72                            186
#define  MIDI_PROG_PERCUSSION_73                            187
#define  MIDI_PROG_PERCUSSION_74                            188
#define  MIDI_PROG_PERCUSSION_75                            189
#define  MIDI_PROG_PERCUSSION_76                            190
#define  MIDI_PROG_PERCUSSION_77                            191
#define  MIDI_PROG_PERCUSSION_80                            192
#define  MIDI_PROG_PERCUSSION_81                            193
#define  MIDI_PROG_PERCUSSION_82                            194
#define  MIDI_PROG_PERCUSSION_83                            195
#define  MIDI_PROG_PERCUSSION_84                            196
#define  MIDI_PROG_PERCUSSION_85                            197
#define  MIDI_PROG_PERCUSSION_86                            198
#define  MIDI_PROG_PERCUSSION_87                            199
#define  MIDI_PROG_PERCUSSION_90                            200
#define  MIDI_PROG_PERCUSSION_91                            201
#define  MIDI_PROG_PERCUSSION_92                            202
#define  MIDI_PROG_PERCUSSION_93                            203
#define  MIDI_PROG_PERCUSSION_94                            204
#define  MIDI_PROG_PERCUSSION_95                            205
#define  MIDI_PROG_PERCUSSION_96                            206
#define  MIDI_PROG_PERCUSSION_97                            207
#define  MIDI_PROG_PERCUSSION_100                           208
#define  MIDI_PROG_PERCUSSION_101                           209
#define  MIDI_PROG_PERCUSSION_102                           210
#define  MIDI_PROG_PERCUSSION_103                           211
#define  MIDI_PROG_PERCUSSION_104                           212
#define  MIDI_PROG_PERCUSSION_105                           213
#define  MIDI_PROG_PERCUSSION_106                           214
#define  MIDI_PROG_PERCUSSION_107                           215
#define  MIDI_PROG_PERCUSSION_110                           216
#define  MIDI_PROG_PERCUSSION_111                           217
#define  MIDI_PROG_PERCUSSION_112                           218
#define  MIDI_PROG_PERCUSSION_113                           219
#define  MIDI_PROG_PERCUSSION_114                           220
#define  MIDI_PROG_PERCUSSION_115                           221
#define  MIDI_PROG_PERCUSSION_116                           222
#define  MIDI_PROG_PERCUSSION_117                           223
#define  MIDI_PROG_PERCUSSION_120                           224
#define  MIDI_PROG_PERCUSSION_121                           225
#define  MIDI_PROG_PERCUSSION_122                           226
#define  MIDI_PROG_PERCUSSION_123                           227
#define  MIDI_PROG_PERCUSSION_124                           228
#define  MIDI_PROG_PERCUSSION_125                           229
#define  MIDI_PROG_PERCUSSION_126                           230
#define  MIDI_PROG_PERCUSSION_127                           231
#define  MIDI_PROG_PERCUSSION_130                           232
#define  MIDI_PROG_PERCUSSION_131                           233
#define  MIDI_PROG_PERCUSSION_132                           234
#define  MIDI_PROG_PERCUSSION_133                           235
#define  MIDI_PROG_PERCUSSION_134                           236
#define  MIDI_PROG_PERCUSSION_135                           237
#define  MIDI_PROG_PERCUSSION_136                           238
#define  MIDI_PROG_PERCUSSION_137                           239
#define  MIDI_PROG_PERCUSSION_140                           240
#define  MIDI_PROG_PERCUSSION_141                           241
#define  MIDI_PROG_PERCUSSION_142                           242
#define  MIDI_PROG_PERCUSSION_143                           243
#define  MIDI_PROG_PERCUSSION_144                           244
#define  MIDI_PROG_PERCUSSION_145                           245
#define  MIDI_PROG_PERCUSSION_146                           246
#define  MIDI_PROG_PERCUSSION_147                           247
#define  MIDI_PROG_PERCUSSION_150                           248
#define  MIDI_PROG_PERCUSSION_151                           249
#define  MIDI_PROG_PERCUSSION_152                           250
#define  MIDI_PROG_PERCUSSION_153                           251
#define  MIDI_PROG_PERCUSSION_154                           252
#define  MIDI_PROG_PERCUSSION_155                           253
#define  MIDI_PROG_PERCUSSION_156                           254
#define  MIDI_PROG_PERCUSSION_157                           255

#endif
