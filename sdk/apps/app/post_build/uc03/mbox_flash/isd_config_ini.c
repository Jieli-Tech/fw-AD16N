// *INDENT-OFF*
//#####################################################
//#
//#   配置数据按照 长度+配置名字+数据的方式存储
//#
//#####################################################

[EXTRA_CFG_PARAM]
NEW_FLASH_FS=YES;
CHIP_NAME=AD16N;//8
ENTRY=0x4000100;//程序入口地址
PID=UC03;//长度16byte,示例：芯片封装_应用方向_方案名称
CHECK_OTA_BIN=NO;//可不检测ota.bin

RESERVED_OPT=0;//入口地址为0x1E00120需要定义该配置项

//DOWNLOAD_MODEL=SERIAL;
DOWNLOAD_MODEL=usb;
SERIAL_DEVICE_NAME=JlVirtualJtagSerial;
SERIAL_BARD_RATE=100000;
SERIAL_CMD_OPT=10;
SERIAL_CMD_RATE=10; [n*10000]
SERIAL_CMD_RES=0;
SERIAL_INIT_BAUD_RATE=9600;
LOADER_BAUD_RATE=100000;
LOADER_ASK_BAUD_RATE=1000000;
SERIAL_SEND_KEY=YES;
BEFORE_LOADER_WAIT_TIME=150;
INTERNAL_DIR_ALIGN=2;

//#NEED_RESERVED_4K=YES;//关闭4K保留
NEED_RESERVED_4K=NO;
NEED_RESERVED_AREA=NO;

//#是否实时根据FLASH支持的对齐方式组织文件（256字节或4K字节对齐）；
SPECIAL_OPT=0

//#强制4k对齐
FORCE_4K_ALIGN=YES


[CHIP_VERSION]
SUPPORTED_LIST=A,B,C

//#####################################################    UBOOT配置项，请勿随意调整顺序    ##################################################
[SYS_CFG_PARAM]
//#clk [0-255]
//#data_width[0 1 2 3 4] 3的时候uboot自动识别2或者4线
//#mode:
//#     0 RD_OUTPUT,       1 cmd       1 addr
//#     1 RD_I/O,          1 cmd       x addr
//#     2 RD_I/O_CONTINUE] no_send_cmd x add
//#SPI = data_width,clk,mode;
SPI=2_3_0_0;  #data_width,clk,mode;
//#OSC=btosc;
//#OSC_FREQ=24MHz; #[24MHz 12MHz]
//#SYS_CLK=48MHz; #[48MHz 24MHz]
UTTX=PA06;//uboot串口tx
UTBD=1000000;//uboot串口波特率
//#UTRX=DP;           //串口升级[PB00 PB05 PA05], 默认PB05
//#RESET=PB01_08_0;   //port口_长按时间_有效电平（长按时间有00、04、08三个值可选，单位为秒，当长按时间为00时，则关闭长按复位功能。）
//CACHE_WAY=1;// 范围1~4
//#WAIT_TIME=10;// * 100ms
EX_FLASH=PB00_1A_PB08;  #cs,spi1,A,flashpower
EX_FLASH_IO=1_PB01_PB02_PB03_PB04_PB05;  #data_width,clk,do,di,d2,d3
//#############################################################################################################################################

//####################################################
[TOOL_CONFIG]
1TO2_MIN_VER=2.24.0
1TO8_MIN_VER=3.0.34
BTBOX_MIN_VER=1.2.4.f
//####################################################

//####################################################
//烧写器配置项
//OTP_BOOT = 0,DEBUG模式
//OTP_BOOT = 1,OTP模式
[BURNER_PASSTHROUGH_CFG]
LIGHTING2TYPEC=0    //uboot_start
OTP_BOOT=0

//########flash空间使用配置区域###############################################
//#PDCTNAME:    产品名，对应此代码，用于标识产品，升级时可以选择匹配产品名
//#BOOT_FIRST:  1=代码更新后，提示APP是第一次启动；0=代码更新后，不提示
//#UPVR_CTL：   0：不允许高版本升级低版本   1：允许高版本升级低版本
//#XXXX_ADR:    区域起始地址  AUTO：由工具自动分配起始地址
//#XXXX_LEN:    区域长度      CODE_LEN：代码长度
//#XXXX_OPT:    区域操作属性
//#
//#
//#
//#操作符说明  OPT:
//#   0:  下载代码时擦除指定区域
//#   1:  下载代码时不操作指定区域
//#   2:  下载代码时给指定区域加上保护
//############################################################################
[RESERVED_CONFIG]

VM_ADR=0;
VM_LEN=24K;
VM_OPT=1;

EEPROM_ADR=AUTO;
EEPROM_LEN=24K;
EEPROM_OPT=1;

[BURNER_CONFIG]
SIZE=32;

