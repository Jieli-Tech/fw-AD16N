/*************************************************************/
/** @file:		usb_device.h
    @brief:		USB 从机驱动重写，加入iAP协议
    @details:
    @author:	Bingquan Cai
    @date: 		2013-10-11,09:34
    @note:
*/
/*************************************************************/

#ifndef _IAP_H_
#define _IAP_H_

#include "config.h"
#include "printf.h"

//iAP1 and iAP2 COMMON
#define IAP2_VERSIONS  						0x02
#define IAP1_VERSIONS  						0x01
#define DEFAULT_SEQUENCE_NUM				0x0
#define EXTRA_SEQUENCE_NUM					0x1

//< hid
#define HID_PREV_FILE			USB_AUDIO_PREFILE
#define HID_NEXT_FILE			USB_AUDIO_NEXTFILE
#define HID_PP					USB_AUDIO_PP
#define HID_PLAY				USB_AUDIO_PLAY
#define HID_PAUSE				USB_AUDIO_PAUSE
#define HID_VOL_DOWN			USB_AUDIO_VOLDOWN
#define HID_VOL_UP				USB_AUDIO_VOLUP

///variable
typedef enum {
    iAP_IDLE = 0,
    iAP_INTERRUPT,
    iAP_BULK_IN,
    iAP_BULE_OUT,
} iAP_STATE;


enum {
    NO_MFI_CHIP = 0,
    MFI_PROCESS_READY,
    MFI_PROCESS_STANDBY,
    MFI_PROCESS_PASS,

    IAP1_LINK_ERR,
    IAP1_LINK_SUCC,
    IAP2_LINK_ERR,
    IAP2_LINK_SUCC,
};

//extern var
extern u8 chip_online_status;
extern u8 mfi_pass_status;

#define apple_mfi_chip_online_lib()		chip_online_status
#define apple_link_disable()			mfi_pass_status = MFI_PROCESS_STANDBY
#define apple_link_init()				mfi_pass_status = MFI_PROCESS_STANDBY
#define apple_mfi_pass_ready_set_api()	mfi_pass_status = MFI_PROCESS_READY
#define apple_check_mfi_not_standby()   (mfi_pass_status != MFI_PROCESS_STANDBY)

///outside call parameter
extern u8  *iAP_var_pRxBuf ;
extern u8  *iAP_var_pTxBuf ;
extern u16  iAP_var_wLength;
extern u16  iAP_var_rLength;

extern u8  iAP_send_pkt_self[0x40] ;
extern u8  iAP_receive_pkt_self[0x40] ;

///inside call
// void my_code_memcpy(u8  *s1, const u8 *s2, u16 len);
#define my_code_memcpy(x,y,z)		memcpy(x,(u8  *)y,z)
// u8 my_memcmp(u8  *s1, const u8 *s2, u8 len);
#define my_code_memcmp(x,y,z)		memcmp(x,(u8  *)y,z)

u8 iAP_support(void);
u8 apple_mfi_link(void *hdl);

#define GET_U16L(x)				(u8)(x)
#define GET_U16H(x)				(u8)((u16)(x)>>8)
#define SW16(x)					(((u16)(x) & 0x00ffU)<<8 | ((u16)(x) & 0xff00U)>>8)

#endif	/*	_IAP_H_	*/

