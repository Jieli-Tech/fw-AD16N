// *INDENT-OFF*
#include "app_config.h"
#include  "maskrom_stubs.ld"
//config
#define _ADDR_RAM0_END    0x109b00
#define _ADDR_RAM0_START  (0x100000 + 64*4)
MEMORY
{
	app_code(rx)        : ORIGIN = 0x4000100,           LENGTH = 32M-0x100
    ram0(rw)            : ORIGIN = _ADDR_RAM0_START,    LENGTH = _ADDR_RAM0_END - _ADDR_RAM0_START - 0x24
    boot_ram(rw)        : ORIGIN = _ADDR_RAM0_END - 0x24,       LENGTH = 0x24
}
ENTRY(_start)

SECTIONS
{
    . = ORIGIN(app_code);
    .app_code ALIGN(4):
    {
        app_code_text_begin = .;
        KEEP(*(.start))
        *(.start.text)
        *(.*.text.const)
        *(.*.text)
        *(.version)
        *(.debug)
        *(.debug.text.const)
        *(.debug.text)
        *(.debug.string)
        *(.text)
        _CLK_CODE_START = .;
        *(.clock.text.cache.L2)
        . = ALIGN(4);
        _CLK_CODE_END = .;
        _SPI_CODE_START = .;
        *(.spi_code)
        . = ALIGN(4);
        _SPI_CODE_END = .;
        *(.rodata*)
        *(.ins)
        app_code_text_end = . ;

        . = ALIGN(4);
        vfs_ops_begin = .;
        KEEP(*(.vfs_operations))
        vfs_ops_end = .;

        . = ALIGN(4);
        PROVIDE(device_node_begin = .);
        KEEP(*(.device))
        PROVIDE(device_node_end = .);

		. = ALIGN(4);
	    lp_target_begin = .;
	    PROVIDE(lp_target_begin = .);
	    KEEP(*(.lp_target))
	    lp_target_end = .;
	    PROVIDE(lp_target_end = .);

        . = ALIGN(4);
        /* . = LENGTH(app_code) - SIZEOF(.data); */
        text_end = .;
	} >app_code

	. = ORIGIN(boot_ram);
	.boot_data ALIGN(4):
	{
		*(.boot_info)
	} > boot_ram

    /* L1 memory sections */
    . = ORIGIN(ram0);
    .data ALIGN(4):
    {
        PROVIDE(data_buf_start = .);
        *(.data_magic)
        *(.data)
        *(.*.data)
        *(.common)

        cache_Lx_code_text_begin = .;
        *(.audio_isr_text)
        *(.log_ut_text)
        *(.*.text.cache.L1)
        *(.*.text.cache.L2)
        *(.*.text.cache.L3)
        . = (. + 3) / 4 * 4 ;
        cache_Lx_code_text_end = .;
	} > ram0

	.debug_data ALIGN(4):
	{
        PROVIDE(debug_buf_start = .);
        *(.debug.data.bss)
        *(.debug.data)
        . = (. + 3) / 4 * 4 ;
    } > ram0

    .bss (NOLOAD) : SUBALIGN(4)
    {
        PROVIDE(bss_buf_start = .);
        . = ALIGN(32);
        _cpu0_sstack_begin = .;
        PROVIDE(cpu0_sstack_begin = .);
        . = ALIGN(32);
        *(.intr_stack)
        . = ALIGN(32);
		*(.stack_magic);
        . = ALIGN(32);
        *(.stack)
        . = ALIGN(32);
		*(.stack_magic0);
        . = ALIGN(32);
        _cpu0_sstack_end = .;
        PROVIDE(cpu0_sstack_end = .);
        . = ALIGN(32);
		_system_data_begin = .;
        *(.bss)
        *(.*.data.bss)
        *(.non_volatile_ram)
        _system_data_end = .;
    } > ram0

    __overlay_start = .;
    OVERLAY : AT(0xA00000)
    {
        .d_music_play
        {
            PROVIDE(mode_music_overlay_data_start = .);
            *(.mode_music_overlay_data);
            /* *(.usb_h_dma); */
            *(.fat_buf);
            PROVIDE(mode_music_overlay_data_end = .);
        }
        .d_fat_tmp
        {
            . = mode_music_overlay_data_end;
            *(.fat_tmp_buf)
        }
        .d_simple_decode
        {
            PROVIDE(mode_smpl_dec_ovly_start = .);
            *(.mode_smpl_dec_data);
            PROVIDE(mode_smpl_dec_ovly_end = .);
        }
        .d_speed
        {
            . = MAX(mode_music_overlay_data_end, mode_smpl_dec_ovly_end);
            PROVIDE(speed_buf_start = .);
            . = ALIGN(4);
            *(.sp_data)
            PROVIDE(speed_buf_end = .);
        }
        .d_a
        {
            . = speed_buf_end;
            PROVIDE(a_buf_start = .);
            *(.a_data);
            PROVIDE(a_buf_end = .);
        }
        .d_f1a
        {
            . = a_buf_end;
			PROVIDE(f1a_1_buf_start = .);
			*(.f1a_1_buf);
			PROVIDE(f1a_1_buf_end = .);
			PROVIDE(f1a_2_buf_start = .);
			*(.f1a_2_buf);
			PROVIDE(f1a_2_buf_end = .);

            PROVIDE(f1a_buf_start = .);
            *(.f1a_data);
            PROVIDE(f1a_buf_end = .);
        }
        .d_ump3
        {
            . = a_buf_end;
            PROVIDE(ump3_buf_start = .);
            *(.ump3_data);
            PROVIDE(ump3_buf_end = .);
        }
        .d_midi
        {
            . = a_buf_end;
            PROVIDE(midi_buf_start = .);
            *(.midi_buf);
            PROVIDE(midi_buf_end = .);
            PROVIDE(midi_ctrl_buf_start = .);
            *(.midi_ctrl_buf);
            PROVIDE(midi_ctrl_buf_end = .);
        }
        /* 标准MP3与WAV不与A格式同时播放 */
        .d_mp3_st
        {
            . = speed_buf_end;
            PROVIDE(mp3_st_buf_start = .);
            *(.mp3_st_data);
            PROVIDE(mp3_st_buf_end = .);
        }
        .d_wav
        {
            . = speed_buf_end;
            PROVIDE(wav_buf_start = .);
            *(.wav_data);
            PROVIDE(wav_buf_end = .);
        }

		.d_rec
		{
            rec_data_start = .;
			*(.rec_data)
            rec_data_end = .;
		}
		.d_enc_ima
		{
			. = rec_data_end;
			*(.enc_a_data)
            enc_a_data_end = .;
		}
		.d_enc_mp3
		{
			. = rec_data_end;
			*(.enc_mp3_data)
            enc_mp3_data_end = .;
		}
		.d_enc_ump3
		{
			. = rec_data_end;
			*(.enc_ump3_data)
            enc_ump3_data_end = .;
		}

        .d_linein
        {
            PROVIDE(mode_linein_overlay_data_start = .);
            *(.digital_linein_data);
            PROVIDE(mode_linein_overlay_data_end = .);
        }
        .d_usb_slave
        {
            PROVIDE(mode_pc_overlay_data_start = .);
            *(.uac_var);
            *(.uac_rx);
            *(.uac_spk);
            *(.mass_storage);
            *(.hid_config_var);
            *(.usb_msd_dma);
            *(.usb_hid_dma);
            *(.usb_iso_dma);
            PROVIDE(mode_pc_overlay_data_end = .);
        }

    } > ram0
    __overlay_end = .;

    d_dec_max = MAX(mp3_st_buf_end, wav_buf_end);

    .heap_buf ALIGN(4):
    {
        PROVIDE(_free_start = .);
        . = LENGTH(ram0) + ORIGIN(ram0) - 1;
        PROVIDE(_free_end = .);
    } > ram0

    _ram_end = .;

    bss_begin       = ADDR(.bss);
    bss_size        = SIZEOF(.bss);

    /*除堆栈外的bss区*/
    bss_size1       = _system_data_end - _system_data_begin;
    bss_begin1      = _system_data_begin;

	data_addr  = ADDR(.data) ;
	data_begin = text_end ;
	data_size =  SIZEOF(.data) + SIZEOF(.debug_data);

    text_size       = SIZEOF(.app_code);

    heap_size = _free_end - _free_start;
    speed_buf_size = speed_buf_end - speed_buf_start;
    a_buf_size = a_buf_end - a_buf_start;

    __overlay_size = __overlay_end - __overlay_start;
    linein_size = mode_linein_overlay_data_end - mode_linein_overlay_data_start;
    usb_slave_size = mode_pc_overlay_data_end - mode_pc_overlay_data_start;
    music_play_size = mode_music_overlay_data_end - mode_music_overlay_data_start;
    simple_decode_size = mode_smpl_dec_ovly_end - a_buf_start;

    decoder_start_addr = MAX(mode_music_overlay_data_end,mode_smpl_dec_ovly_end);
    f1a_size = f1a_buf_end - f1a_1_buf_start;
    ump3_size = ump3_buf_end - ump3_buf_start;
    mp3_st_size = mp3_st_buf_end - mp3_st_buf_start;
    wave_size = wav_buf_end - wav_buf_start;
    midi_a_size = midi_ctrl_buf_end - a_buf_end;

    a_rec_size = rec_data_end - rec_data_start;
    mp3_rec_size = enc_mp3_data_end - rec_data_start;
    ump3_rec_size = enc_ump3_data_end - rec_data_start;


}

