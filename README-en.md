# fw-AD16N
About Firmware for Generic MCU SDK（AD16N series）, Support AD16N

[tag download]:https://github.com/Jieli-Tech/fw-AD16N/tags
[tag_badgen]:https://img.shields.io/github/v/tag/Jieli-Tech/fw-AD16N?style=plastic&labelColor=ffffff&color=informational&label=Tag&

# fw-AD16N_SDK   [![tag][tag_badgen]][tag download]

[中文](./README.md) | EN

firmware for Generic MCU SDK（AD16 series）

This repository contains the Jieli source code, aims at helping the developers for the toy & small music speaker & generic MCU applications.
It must be combined with lib.a and the repositories that use the same
naming convention to build the provided samples and to use the additional
subsystems and libraries.

Getting Started
------------

Welcome to JL open source! See the `Introduction to SDK` for a high-level overview,
and the documentation's `Getting Started Guide` to start developing.

Toolchain
------------

How to get the `JL Toolchain` and setup the build enviroment,see below

* Complie Tool ：install the JL complie tool to setup the build enviroment, [download link](https://pan.baidu.com/s/1f5pK7ZaBNnvbflD-7R22zA) code: `ukgx`
* Compiler invitation code: 4VlZaQCq-lImlZX2u-GBeCs501-ektNxDGu

* USB updater : program flash tool to download the `hex` file to the target board, please accquire the tool form the [link](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22883854875.5.504d246bXKwyeH&id=620295020803) and check the related configuration and [document](.doc/stuff/ISD_CONFIG.INI配置文件说明.pdf)


Documentation
------------

* Chipset brief : [SoC datasheet](./doc)

* SDK Version: [SDK History](doc/AD14N-AD15N_SDK_发布版本信息.pdf)

* SDK introduction : [SDK quick start guide](./doc/AD14N_AD15N_AC104N_SDK手册.pdf)

* SDK architure : [SDK module architure ](./doc/architure)

Build
-------------
Select a project to build. The following folders contains buildable projects:

* APP_TOY、APP_MUSIC: ./sdk/AD16N_flash.cbp, usage: 
 


Comming Soon：
-------------

SDK support Codeblock to build to project,make sure you already setup the enviroment

* Codeblock build : enter the project directory and find the `.cbp`,double click and build.

* Makefile build : `apps/app_cfg` select the target you want to build,double click the `make_prompt` and excute `make`

  `before build the project make sure the USB updater is connect and enter the update mode correctly`


Hardware
-------------

* EV Board ：(https://shop321455197.taobao.com/?spm=a230r.7195193.1997079397.2.2a6d391d3n5udo)

* Production Tool : massive prodution and program the SoC, please accquire the tool from the [link](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22883854875.8.504d246bXKwyeH&id=620941819219) and check the releated [doc](./doc/stuff/烧写器使用说明文档.pdf)

Packing tool & audio file conversion tool
-------------

* [download link](https://pan.baidu.com/s/1ajzBF4BFeiRFpDF558ER9w#list/path=%2F) code: `3jey`
  
SDK function
-------------
* Supports decoding and playback of built-in FLASH
* Support the decoding and playback of external FLASH
* Support the decoding and playback of external SDMMC card
* Support the decoding and playback of external udisk
* Support the USB_DEVICE SPEAKER_CLASS
* Support the USB_DEVICE MIC_CLASS
* Support the USB_DEVICE HID_CLASS
* Support decoding MIO function
* Support the decoding and playback of the formats of .a/.b/.e
* Support the decoding and playback of the formats of .f1a/.f1b/.f1c
* Support the decoding and playback of the formats of UMP3
* Support the decoding and playback of the formats of MP3
* Support MIDI playback
* Support variable speed modulation (but need to adjust the system clock above 100Mhz)
* Supports up to .a/.b/.e + .f1a/.f1b/.f1c two channels of audio decoding and playback at the same time
* AUDIO_DAC supports two-channel output
* AUDIO_DAC supports 8K, 11.025K, 12k, 16K, 22.05k, 24K, 32K, 44.1k, 48k, 64k, 88.2k, 96k sampling rate;
* AUDIO_ADC supports mono single-ended input, supports 8K, 1.025K, 12k, 16K, 22.05k, 24K, 32K, 44.1k, 48k sampling rate
* Support digital LINEIN and analog direct LINEIN;
* Support Support digital tube Led UI display
* Support ADC, IO, infrared, matrix keys, support key tone
* Support VM power-off memory, and the memory data is saved in the built-in FLASH
* Support charging management
* Support hardware resampling
* Support SOFT OFF shutdown and POWER DOWN sleep, shutdown power consumption 1.7uA+, sleep power consumption 38uA+;
  
MCU information
-------------
* RISC / 160MHz /40K+16K
* flash / 2Mbit / 4Mbit
* UART * 2 / IIC / SPI * 2
* 10bit ADC
* 16bit audio DAC
* 16bit audio ADC
* LCD Controler
* LED Controler
* RAND64
* TDM
* RDEC(rotary encoder)
* SD IO
* IR
* MCPWM * 8
* 32 bit timer * 3
* 37 * GPIO

Community
--------------

* [Dingtalk Group](./doc/stuff/dingtalk.jpg)

Disclaimer
------------

AD16N_SDK supports development with AD16 series devices.
AD16 Series devices (which are pre-production) and Toy applications are supported for development in Release for production and deployment in end products.
