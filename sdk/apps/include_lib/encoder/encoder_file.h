#ifndef __ENCODER_FILE_H__
#define __ENCODER_FILE_H__

#include "typedef.h"
#include "encoder_mge.h"
// #include "audio_enc_api.h"
// #include "sound_mge.h"
// #include "audio_adc.h"

enc_obj *encoder_file(u32(*fun)(void *, void *, void *), void *pfile);


#endif
