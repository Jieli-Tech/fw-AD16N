#ifndef _RESERVED_AREA_H
#define _RESERVED_AREA_H

u32 get_flash_alignsize(void);
#define RESERVED_AREA_SIZE		get_flash_alignsize()

/*****************auth*******************/
typedef struct __auth_header {
    u16 crc16;
    u16 data_len;
    const u8 data_p[];
} __attribute__((packed)) auth_header_t;

void rsv_auth_analysis(void);
/*****************auth end*******************/

#endif

