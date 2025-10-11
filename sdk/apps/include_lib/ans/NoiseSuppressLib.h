#ifndef NoiseSuppressLib_H
#define NoiseSuppressLib_H
#define NOISESP_CONFIG_FREEZE 0
#define NOISESP_CONFIG_NOISEFLOOR 1
#define NOISESP_CONFIG_LOWCUTTHR 2
#ifdef __cplusplus
extern "C"
{
#endif
int NoiseSuppress_GetMiniFrame(int is_wideband);
int NoiseSuppress_QueryProcessDelay(int mode, int is_wideband);
int NoiseSuppress_QueryBufSize(int mode, int is_wideband);
int NoiseSuppress_QueryTempBufSize(int mode, int is_wideband);
void NoiseSuppress_Init(void *NoiseSpRunBuffer,
                        int AggressFactor, // Q16
                        int minSuppress,   // Q16
                        int mode,
                        int is_wideband,
                        int noise_lvl); // Q10
void NoiseSuppress_Process(void *NoiseSpRunBuffer,
                           void *NoiseSpTempBuffer,
                           short *input,
                           short *output,
                           short *inputH,
                           short *outputH,
                           int npoint);
void NoiseSuppress_Config(void *NoiseSpRunBuffer,
                          int ConfigType,
                          int *ConfigPara);
#ifdef __cplusplus
}
#endif
#endif


