
# make 编译并下载
# make VERBOSE=1 显示编译详细过程
# make clean 清除编译临时文件
#
# 注意： Linux 下编译方式：
#     1. 从 http://pkgman.jieliapp.com/doc/all 处找到下载链接
#     2. 下载后，解压到 /opt/jieli 目录下，保证
#       /opt/jieli/common/bin/clang 存在（注意目录层次）
#     3. 确认 ulimit -n 的结果足够大（建议大于8096），否则链接可能会因为打开文件太多而失败
#       可以通过 ulimit -n 8096 来设置一个较大的值
#

# 工具路径设置
ifeq ($(OS), Windows_NT)
# Windows 下工具链位置
TOOL_DIR := C:/JL/pi32/bin
CC    := clang.exe
CXX   := clang.exe
LD    := pi32v2-lto-wrapper.exe
AR    := pi32v2-lto-ar.exe
MKDIR := mkdir_win -p
RM    := rm -rf

SYS_LIB_DIR := C:/JL/pi32/pi32v2-lib/r1-large
SYS_INC_DIR := C:/JL/pi32/pi32v2-include
EXT_CFLAGS  := # Windows 下不需要 -D__SHELL__
export PATH:=$(TOOL_DIR);$(PATH)

## 后处理脚本
FIXBAT          := tools\utils\fixbat.exe # 用于处理 utf8->gbk 编码问题
POST_SCRIPT     := apps\app\post_build\uc03\download.bat
RUN_POST_SCRIPT := $(POST_SCRIPT)
else
# Linux 下工具链位置
TOOL_DIR := /opt/jieli/pi32v2/bin
CC    := clang
CXX   := clang++
LD    := lto-wrapper
AR    := lto-ar
MKDIR := mkdir -p
RM    := rm -rf
export OBJDUMP := $(TOOL_DIR)/objdump
export OBJCOPY := $(TOOL_DIR)/objcopy
export OBJSIZEDUMP := $(TOOL_DIR)/objsizedump

SYS_LIB_DIR := $(TOOL_DIR)/../lib/r1-large
SYS_INC_DIR := $(TOOL_DIR)/../include
EXT_CFLAGS  := -D__SHELL__ # Linux 下需要这个保证正确处理 download.c
export PATH:=$(TOOL_DIR):$(PATH)

## 后处理脚本
FIXBAT          := touch # Linux下不需要处理 bat 编码问题
POST_SCRIPT     := apps/app/post_build/uc03/download.sh
RUN_POST_SCRIPT := bash $(POST_SCRIPT)
endif

CC  := $(TOOL_DIR)/$(CC)
CXX := $(TOOL_DIR)/$(CXX)
LD  := $(TOOL_DIR)/$(LD)
AR  := $(TOOL_DIR)/$(AR)
# 输出文件设置
OUT_ELF   := apps/app/post_build/uc03/sdk.elf
OBJ_FILE  := $(OUT_ELF).objs.txt
# 编译路径设置
BUILD_DIR := objs

# 编译参数设置
CFLAGS := \
	-target pi32v2 \
	-mcpu=r1 \
	-integrated-as \
	-mllvm -inline-threshold=5 \
	-Oz \
	-flto \
	-Wuninitialized \
	-Wno-invalid-noreturn \
	-fno-common \
	-integrated-as \
	-Oz \
	-g \
	-flto \
	-fallow-pointer-null \
	-fprefer-gnu-section \
	-Wno-shift-negative-value \
	-Wundef \
	-mllvm -pi32v2-large-program=true \
	-Werror=implicit-function-declaration \
	-Werror=return-type \
	-Werror=undef \
	-Wno-format \


# C++额外的编译参数
CXXFLAGS :=


# 宏定义
DEFINES := \
	-D__FPGA=0 \
	-DCONFIG_CPU_UC03=1 \
	-DD_MASKROM_CODE=0 \
	-DD_IS_FLASH_SYSTEM \
	-DD_SFC_DEVICE_EN \
	-DFPGA=0 \
	-DHAS_SRC_EN=1 \
	-DHAS_MIO_PLAYER \
	-DHAS_MP3_ST_DECODER \
	-DHAS_WAV_DECODER \
	-DHAS_F1A_DECODER \
	-DHAS_MAX_F1A_NUMBER=2 \
	-DHAS_UMP3_DECODER \
	-DHAS_A_DECODER \
	-DHAS_MIDI_DECODER \
	-DHAS_MIDI_KEYBOARD_DECODER \
	-DHAS_DECODER_LOOP_EN \
	-DHAS_MP3_ENCODER \
	-DHAS_UMP3_ENCODER \
	-DHAS_A_ENCODER \
	-DHAS_SPEED_EFFECT \
	-DHAS_SONG_SPEED_EFFECT \
	-DHAS_HOWLING_EFFECT \
	-DHAS_ECHO_EFFECT \
	-DHAS_VOICE_PITCH_EFFECT \
	-DHAS_USB_EN=1 \
	-DHAS_SDMMC_EN=1 \
	-DHAS_EXTFLASH_EN=1 \
	-DHAS_VFS_EN=1 \
	-DHAS_NORFS_EN=1 \
	-DHAS_SYDFS_EN=1 \
	-DHAS_LINEIN_MODE \
	-DHAS_SIMPLE_DEC_MODE \
	-DHAS_RECORD_MODE \
	-DHAS_MUSIC_MODE \
	-DHAS_USB_SLAVE_MODE \
	-DHAS_LOUDSPEAKER_MODE \
	-DHAS_RTC_MODE \
	-DHAS_NEW_VM_MODE \
	-DNOFLOAT \


DEFINES += $(EXT_CFLAGS) # 额外的一些定义

# 头文件搜索路径
INCLUDES := \
	-Iapps/include_lib/fs/sydf \
	-Iapps/include_lib/fs/nor_fs \
	-Iapps/include_lib/fs/fat \
	-Iapps/app/bsp/common/decoder/mio \
	-Iapps/include_lib/encoder \
	-Iapps/include_lib/encoder/list \
	-Iapps/app/src \
	-Iapps/include_lib \
	-Iapps/app/post_build/uc03 \
	-Iapps/include_lib/common \
	-Iapps/include_lib/audio \
	-Iapps/include_lib/audio_eq \
	-Iapps/include_lib/cpu/uc03 \
	-Iapps/include_lib/cpu \
	-Iapps/maske_include \
	-Iapps/app/bsp/lib \
	-Iapps/app/bsp/start \
	-Iapps/app/bsp/cpu/uc03 \
	-Iapps/app/bsp/cpu/uc03/entry \
	-Iapps/app/bsp/common \
	-Iapps/app/bsp/common/power_manage \
	-Iapps/app/bsp/common/usb/usr \
	-Iapps/app/bsp/device \
	-Iapps/app/src/mbox_flash \
	-Iapps/app/src/mbox_flash/uc03 \
	-Iapps/include_lib/msg \
	-Iapps/include_lib/fs \
	-Iapps/include_lib/dev_mg \
	-Iapps/include_lib/decoder \
	-Iapps/include_lib/decoder/list \
	-Iapps/include_lib/sound_mge \
	-Iapps/include_lib/device \
	-Iapps/include_lib/device/sdmmc \
	-Iapps/include_lib/device/usb \
	-Iapps/include_lib/device/usb/device \
	-Iapps/include_lib/device/usb/host \
	-Iapps/include_lib/device/norflash \
	-Iapps/include_lib/update \
	-Iapps/app/bsp \
	-Iapps/app/bsp/common/key \
	-Iapps/include_lib/sound_effect_list \
	-Iapps/include_lib/liba/ARCH/pi32v2_lto_r1/header \
	-Iapps/app/src/mbox_flash/common \
	-Iapps/app/src/mbox_flash/common/ui \
	-Iapps/app/src/mbox_flash/music \
	-Iapps/app/src/mbox_flash/usb_slave \
	-Iapps/app/src/mbox_flash/linein \
	-Iapps/app/src/mbox_flash/record \
	-Iapps/app/src/mbox_flash/simple_decode \
	-Iapps/app/src/mbox_flash/midi_dec \
	-Iapps/app/src/mbox_flash/midi_keyboard \
	-Iapps/app/src/mbox_flash/loudspeaker \
	-Iapps/app/src/mbox_flash/rtc \
	-Iapps/app/src/mbox_flash/softoff_app \
	-Iapps/app/src/mbox_flash/test_dir \
	-Iapps/app/bsp/common/usb \
	-Iapps/app/bsp/common/usb/device \
	-Iapps/app/bsp/common/usb/host \
	-I$(SYS_INC_DIR) \


# 需要编译的 .c 文件
c_SRC_FILES := \
	apps/app/bsp/common/audio/audio_adc_api.c \
	apps/app/bsp/common/audio/audio_dac_api.c \
	apps/app/bsp/common/audio/audio_dac_fade.c \
	apps/app/bsp/common/audio/fill_audac.c \
	apps/app/bsp/common/audio_eq/audio_eq_api.c \
	apps/app/bsp/common/decoder/decoder_api.c \
	apps/app/bsp/common/decoder/decoder_msg_tab.c \
	apps/app/bsp/common/decoder/decoder_point.c \
	apps/app/bsp/common/decoder/list/a_api.c \
	apps/app/bsp/common/decoder/list/f1a_api.c \
	apps/app/bsp/common/decoder/list/f1x_parsing.c \
	apps/app/bsp/common/decoder/list/midi_api.c \
	apps/app/bsp/common/decoder/list/midi_ctrl_api.c \
	apps/app/bsp/common/decoder/list/mp3_standard_api.c \
	apps/app/bsp/common/decoder/list/ump3_api.c \
	apps/app/bsp/common/decoder/list/wav_api.c \
	apps/app/bsp/common/decoder/mio/mio_api.c \
	apps/app/bsp/common/decoder/mio/mio_phy.c \
	apps/app/bsp/common/decoder/mp_io.c \
	apps/app/bsp/common/decoder/sine_play.c \
	apps/app/bsp/common/encoder/encoder_api.c \
	apps/app/bsp/common/encoder/list/a_encoder.c \
	apps/app/bsp/common/encoder/list/mp3_encoder.c \
	apps/app/bsp/common/encoder/list/ump3_encoder.c \
	apps/app/bsp/common/fs/fat/fat_resource.c \
	apps/app/bsp/common/fs/nor_fs/nor_fs_resource.c \
	apps/app/bsp/common/fs/sydf/sydf_resource.c \
	apps/app/bsp/common/fs/vfs.c \
	apps/app/bsp/common/fs/vfs_fat.c \
	apps/app/bsp/common/fs/vfs_resource.c \
	apps/app/bsp/common/key/key.c \
	apps/app/bsp/common/key/key_drv_ad.c \
	apps/app/bsp/common/key/key_drv_io.c \
	apps/app/bsp/common/key/key_drv_matrix.c \
	apps/app/bsp/common/key/key_drv_mic.c \
	apps/app/bsp/common/key/key_ir.c \
	apps/app/bsp/common/key/key_touch.c \
	apps/app/bsp/common/msg/msg.c \
	apps/app/bsp/common/my_malloc.c \
	apps/app/bsp/common/norflash/norflash.c \
	apps/app/bsp/common/power_manage/app_power_mg.c \
	apps/app/bsp/common/sound_effect_list/echo_api.c \
	apps/app/bsp/common/sound_effect_list/notch_howling_api.c \
	apps/app/bsp/common/sound_effect_list/pitchshift_howling_api.c \
	apps/app/bsp/common/sound_effect_list/song_speed_api.c \
	apps/app/bsp/common/sound_effect_list/speed_api.c \
	apps/app/bsp/common/sound_effect_list/vo_pitch_api.c \
	apps/app/bsp/common/speaker/speak_api.c \
	apps/app/bsp/common/tab_read.c \
	apps/app/bsp/common/usb/device/descriptor.c \
	apps/app/bsp/common/usb/device/hid.c \
	apps/app/bsp/common/usb/device/msd.c \
	apps/app/bsp/common/usb/device/task_pc.c \
	apps/app/bsp/common/usb/device/uac_stream.c \
	apps/app/bsp/common/usb/device/usb_device.c \
	apps/app/bsp/common/usb/device/usb_device_config.c \
	apps/app/bsp/common/usb/device/user_setup.c \
	apps/app/bsp/common/usb/host/usb_host.c \
	apps/app/bsp/common/usb/usb_test.c \
	apps/app/bsp/common/usb/usr/uac_sync.c \
	apps/app/bsp/common/usb/usr/usb_audio_interface.c \
	apps/app/bsp/common/usb/usr/usb_mic_interface.c \
	apps/app/bsp/common/vm/nvm_api.c \
	apps/app/bsp/common/vm/vm_api.c \
	apps/app/bsp/cpu/uc03/audio_dac_cpu.c \
	apps/app/bsp/cpu/uc03/charge.c \
	apps/app/bsp/cpu/uc03/clock.c \
	apps/app/bsp/cpu/uc03/irflt.c \
	apps/app/bsp/cpu/uc03/lcd_seg4x8_driver.c \
	apps/app/bsp/cpu/uc03/pl_cnt.c \
	apps/app/bsp/cpu/uc03/power_api.c \
	apps/app/bsp/cpu/uc03/power_port.c \
	apps/app/bsp/cpu/uc03/rtc_api.c \
	apps/app/bsp/cpu/uc03/saradc_isr.c \
	apps/app/bsp/cpu/uc03/sd.c \
	apps/app/bsp/cpu/uc03/spi.c \
	apps/app/bsp/cpu/uc03/tick_timer_sys.c \
	apps/app/bsp/cpu/uc03/timer_drv.c \
	apps/app/bsp/cpu/uc03/timer_pwm.c \
	apps/app/bsp/cpu/uc03/uart.c \
	apps/app/bsp/device/device_app.c \
	apps/app/bsp/device/device_list.c \
	apps/app/bsp/start/bsp_loop.c \
	apps/app/bsp/start/flash_init.c \
	apps/app/bsp/start/init.c \
	apps/app/bsp/start/uc03/main.c \
	apps/app/src/mbox_flash/app.c \
	apps/app/src/mbox_flash/app_config.c \
	apps/app/src/mbox_flash/common/hot_msg.c \
	apps/app/src/mbox_flash/common/idle_check.c \
	apps/app/src/mbox_flash/common/pa_mute.c \
	apps/app/src/mbox_flash/common/simple_play_file.c \
	apps/app/src/mbox_flash/common/ui/led5x7_driver.c \
	apps/app/src/mbox_flash/common/ui/ui_api.c \
	apps/app/src/mbox_flash/common/ui/ui_common.c \
	apps/app/src/mbox_flash/init_app.c \
	apps/app/src/mbox_flash/linein/linein_mode.c \
	apps/app/src/mbox_flash/linein/linein_mode_key.c \
	apps/app/src/mbox_flash/loudspeaker/loudspk_mode.c \
	apps/app/src/mbox_flash/loudspeaker/loudspk_mode_key.c \
	apps/app/src/mbox_flash/midi_dec/midi_dec_mode.c \
	apps/app/src/mbox_flash/midi_dec/midi_dec_mode_key.c \
	apps/app/src/mbox_flash/midi_keyboard/midi_keyboard_mode.c \
	apps/app/src/mbox_flash/midi_keyboard/midi_keyboard_mode_key.c \
	apps/app/src/mbox_flash/music/break_point.c \
	apps/app/src/mbox_flash/music/music_device.c \
	apps/app/src/mbox_flash/music/music_key_table.c \
	apps/app/src/mbox_flash/music/music_play.c \
	apps/app/src/mbox_flash/music/play_file.c \
	apps/app/src/mbox_flash/record/enc_in_fatfs.c \
	apps/app/src/mbox_flash/record/enc_in_norfs.c \
	apps/app/src/mbox_flash/record/record_mode.c \
	apps/app/src/mbox_flash/record/record_mode_key.c \
	apps/app/src/mbox_flash/rtc/rtc_mode.c \
	apps/app/src/mbox_flash/rtc/rtc_mode_key.c \
	apps/app/src/mbox_flash/simple_decode/simple_decode.c \
	apps/app/src/mbox_flash/simple_decode/simple_decode_key.c \
	apps/app/src/mbox_flash/softoff_app/softoff_mode.c \
	apps/app/src/mbox_flash/usb_slave/usb_slave_mode.c \
	apps/app/src/mbox_flash/usb_slave/usb_slave_mode_key.c \


# 需要编译的 .S 文件
S_SRC_FILES :=


# 需要编译的 .s 文件
s_SRC_FILES :=


# 需要编译的 .cpp 文件
cpp_SRC_FILES :=


# 需要编译的 .cc 文件
cc_SRC_FILES :=


# 需要编译的 .cxx 文件
cxx_SRC_FILES :=


# 链接参数
LFLAGS := \
	--plugin-opt=-pi32v2-always-use-itblock=false \
	--plugin-opt=-enable-ipra=true \
	--plugin-opt=-pi32v2-merge-max-offset=4096 \
	--plugin-opt=mcpu=r1 \
	--plugin-opt=-global-merge-on-const \
	--plugin-opt=-inline-threshold=5 \
	--plugin-opt=-inline-max-allocated-size=32 \
	--plugin-opt=-inline-normal-into-special-section=true \
	--plugin-opt=-dont-used-symbol-list=malloc,free,sprintf,printf,puts,putchar \
	--plugin-opt=save-temps \
	--plugin-opt=-pi32v2-enable-rep-memop \
	--sort-common \
	--gc-sections \
	--plugin-opt=-pi32v2-large-program=true \
	--start-group \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/mp3_patch_lib.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_f1a_decode.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_a_decode.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/ump3_dec_lib.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_midi_decode.a \
	apps/include_lib/liba/uc03/mbox_flash/encoder_mge_lib.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_mp2_code.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/ump2_enc_lib.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_a_code.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_speed_pitch.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/ps_song_lib.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/howling.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_howling_fs.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_echo_cal.a \
	apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_vopitch_cal.a \
	--start-group  \
	apps/include_lib/liba/uc03/mbox_flash/decoder_mge_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/cpu_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/audio_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/audio_eq_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/sound_mge_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/malloc_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/fs_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/src_lib.a \
	apps/include_lib/liba/uc03/mbox_flash/update_lib.a \
	--end-group \
	apps/include_lib/liba/uc03/mbox_flash/new_vm_lib.a \
	--end-group  \
	-Tapps/app/post_build/uc03/app.ld \
	-M=apps/app/post_build/uc03/app.map \
	--plugin-opt=-inline-threshold=5 \
	--plugin-opt=mcpu=r1 \


LIBPATHS := \
	-L$(SYS_LIB_DIR) \


LIBS := \
	$(SYS_LIB_DIR)/libm.a \
	$(SYS_LIB_DIR)/libc.a \
	$(SYS_LIB_DIR)/libm.a \
	$(SYS_LIB_DIR)/libcompiler-rt.a \



c_OBJS    := $(c_SRC_FILES:%.c=%.c.o)
S_OBJS    := $(S_SRC_FILES:%.S=%.S.o)
s_OBJS    := $(s_SRC_FILES:%.s=%.s.o)
cpp_OBJS  := $(cpp_SRC_FILES:%.cpp=%.cpp.o)
cxx_OBJS  := $(cxx_SRC_FILES:%.cxx=%.cxx.o)
cc_OBJS   := $(cc_SRC_FILES:%.cc=%.cc.o)

OBJS      := $(c_OBJS) $(S_OBJS) $(s_OBJS) $(cpp_OBJS) $(cxx_OBJS) $(cc_OBJS)
DEP_FILES := $(OBJS:%.o=%.d)


OBJS      := $(addprefix $(BUILD_DIR)/, $(OBJS))
DEP_FILES := $(addprefix $(BUILD_DIR)/, $(DEP_FILES))


VERBOSE ?= 0
ifeq ($(VERBOSE), 1)
QUITE :=
else
QUITE := @
endif

# 一些旧的 make 不支持 file 函数，需要 make 的时候指定 LINK_AT=0 make
LINK_AT ?= 1

# 表示下面的不是一个文件的名字，无论是否存在 all, clean, pre_build 这样的文件
# 还是要执行命令
# see: https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean pre_build

# 不要使用 make 预设置的规则
# see: https://www.gnu.org/software/make/manual/html_node/Suffix-Rules.html
.SUFFIXES:

all: pre_build $(OUT_ELF)
	$(info +POST-BUILD)
	$(QUITE) $(RUN_POST_SCRIPT) sdk

pre_build:
	$(info +PRE-BUILD)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P apps/app/bsp/common/app_modules_h.c -o tmp_app2022.txt
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P apps/app/post_build/uc03/mbox_flash/app_ld.c -o apps/app/post_build/uc03/app.ld
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P apps/app/post_build/uc03/mbox_flash/download_bat.c -o $(POST_SCRIPT)
	$(QUITE) $(FIXBAT) $(POST_SCRIPT)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P apps/app/post_build/uc03/mbox_flash/download_sh.c -o apps/app/post_build/uc03/download.sh
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P apps/app/post_build/uc03/mbox_flash/isd_config_ini.c -o apps/app/post_build/uc03/isd_config.ini

clean:
	$(QUITE) $(RM) $(OUT_ELF)
	$(QUITE) $(RM) $(BUILD_DIR)



ifeq ($(LINK_AT), 1)
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(file >$(OBJ_FILE), $(OBJS))
	$(QUITE) $(LD) -o $(OUT_ELF) @$(OBJ_FILE) $(LFLAGS) $(LIBPATHS) $(LIBS)
else
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(QUITE) $(LD) -o $(OUT_ELF) $(OBJS) $(LFLAGS) $(LIBPATHS) $(LIBS)
endif


$(BUILD_DIR)/%.c.o : %.c
	$(info +CC $<)
	@$(MKDIR) $(@D)
	@$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.S.o : %.S
	$(info +AS $<)
	@$(MKDIR) $(@D)
	@$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.s.o : %.s
	$(info +AS $<)
	@$(MKDIR) $(@D)
	@$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cpp.o : %.cpp
	$(info +CXX $<)
	@$(MKDIR) $(@D)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cxx.o : %.cxx
	$(info +CXX $<)
	@$(MKDIR) $(@D)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cc.o : %.cc
	$(info +CXX $<)
	@$(MKDIR) $(@D)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

-include $(DEP_FILES)
