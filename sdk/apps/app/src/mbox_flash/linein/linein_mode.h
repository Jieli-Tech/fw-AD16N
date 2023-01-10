#ifndef __LINEIN_MODE__
#define __LINEIN_MODE__
#include "typedef.h"
#include "app.h"
#include "key.h"

#define ANALOG_LINEIN_MODE  0
#define DIGITAL_LINEIN_MODE 1

void linein_app(void);
void aux_test_audio(void);
extern u16 linein_key_msg_filter(u8 key_status, u8 key_num, u8 key_type);
#endif
