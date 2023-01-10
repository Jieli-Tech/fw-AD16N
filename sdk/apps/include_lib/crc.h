#ifndef  __CRC_H__
#define  __CRC_H__

#include "typedef.h"


u16 chip_crc16(void *ptr, u32  len); //CRC校验
void CrcDecode(void  *buf, u16 len);

#endif  /*CRC_H*/
