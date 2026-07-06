[tag download]:https://github.com/Jieli-Tech/fw-AD16N/tags
[tag_badgen]:https://img.shields.io/github/v/tag/Jieli-Tech/fw-AD16N?style=plastic&labelColor=ffffff&color=informational&label=Tag&

# fw-AD16N_GP-MCU_SDK  [![tag][tag_badgen]][tag download]

<div align="center">

**Jieli AD16N Series Universal MCU SDK Firmware**

[中文](./README.md) · [Documentation Center](doc/AD16N_开源SDK手册_V1.2.pdf) · [SDK Release History](doc/AD16N_FLASH_SDK_发布版本信息.pdf) · [Report an Issue](https://github.com/Jieli-Tech/fw-AD16N/issues)

</div>

---

## Table of Contents

- [1. Overview](#1-overview)
- [2. Supported Chips and Platforms](#2-supported-chips-and-platforms)
- [3. Environment Setup](#3-environment-setup)
- [4. Quick Start](#4-quick-start)
- [5. Project Structure](#5-project-structure)
- [6. Applications and Examples](#6-applications-and-examples)
- [7. Build Guide](#7-build-guide)
- [8. Flashing and Upgrade](#8-flashing-and-upgrade)
- [9. Configuration](#9-configuration)
- [10. FAQ](#10-faq)
- [11. Community and Support](#11-community-and-support)
- [12. Disclaimer](#12-disclaimer)

---

## 1. Overview

`fw-AD16N_GP-MCU_SDK` is a universal MCU SDK development package provided by Jieli Technology for the AD16N series chips. These chips are primarily targeted at the following application scenarios:

| Application Type | Typical Products |
|---------|---------|
| **Voice Toys** | Storytelling machines, learning devices, MIDI instruments |
| **Mini Speakers** | MP3 music players, voice recorders, loudspeakers |
| **General MCU** | Smart control, sensor acquisition, USB audio devices |

### Core Features

- **Audio Decoding**: Supports playback of .a/.b/.e, .f1a/.f1b/.f1c/.f1x, and other audio formats
- **MP3 Decoding**: Supports standard MP3, WMA, and WAV hardware decoding
- **MIDI Playback**: Supports MIDI synthesis and playback
- **Audio Encoding**: Supports MP2, UMP3, A, and other encoding formats for recording
- **Multi-channel Playback**: Up to .a/.b/.e + .f1a/.f1b/.f1c two simultaneous audio decode streams
- **Variable Speed/Pitch**: Supports variable-speed and pitch-shifted audio playback (requires system clock ≥ 100 MHz)
- **Hardware Resampling**: Built-in hardware resampling
- **Built-in Audio Codec**: 16-bit DAC dual-channel output + 16-bit ADC mono input, supports 8K–96K sample rates
- **Multi-band EQ/DRC**: Supports multi-band EQ configuration and DRC limiter
- **Low Power**: Shutdown current as low as 1.7 µA+
- **Multiple Storage Options**: Supports internal/external FLASH, SD/MMC cards, USB flash drives, FAT file system
- **Charging Management**: Built-in lithium battery charging manager, up to 120 mA
- **LCD Driver**: Some models support segment LCD / LED digit display
- **OTP Memory**: Built-in 32KB OTP (One-Time Programmable), capable of running code directly without requiring external Flash

This repository contains SDK release versions and sample projects. Compilation requires the corresponding library files (`lib.a`) that follow the naming convention.

---

## 2. Supported Chips and Platforms

### 2.1 SoC Families

| Chip Series | Application Domain |
|---------|---------|
| AD160A / AD161A | General MCU / Audio player (supports LCD, QFN52/LQFP48 large packages) |
| AD162A/B/C/D | Mini Speakers / Audio player (SOP16 small package, built-in Flash) |
| AD165A/C/D | Mini Speakers / General MCU (QSOP24) |
| AD166A | General MCU / Audio player (QFN32) |
| AD168A | Mini Speakers / Audio player (SOP8 smallest package) |

> For chip models, datasheets, and schematic resources, see: [doc/ directory](doc/)

### 2.2 MCU Hardware/Software Parameter Differences

![Chip](jl_ad_chip.png)

---

## 3. Environment Setup

### 3.1 Prerequisites

| System | Description |
|------|------|
| **Windows** | Code::Blocks IDE recommended for compilation |
| **Linux** | Command-line compilation via Makefile (requires rewriting `download_sh.c` for Linux compatibility) |
| **macOS** | Cross-compilation toolchain must be configured manually |

### 3.2 Install the Build Toolchain

1. Download and install the **Jieli Build Toolchain**: [Download Link](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/dev_env/index.html)
2. Linux users can download from: [pkgman.jieliapp.com](http://pkgman.jieliapp.com/doc/all)
   - Extract to `/opt/jieli` after downloading
   - Ensure `/opt/jieli/pi32/bin/clang` exists
3. Verify installation:

```bash
# Verify toolchain installation
clang --version
```

### 3.3 Install Flashing Tools

| Tool | Purpose | How to Obtain |
|------|------|---------|
| **USB Upgrade Tool** | Flash firmware to the target board | [Application Link](https://item.taobao.com/item.htm?id=620295020803) · [User Guide](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/forced_upgrade/index.html) |
| **Mass Production Burner** | Mass production / bare-die programming | **From distributor** · [User Guide](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) |

---

### 3.4 Audio Tools

Universal audio tools for packaging, audio file conversion, MIDI, etc.: [Download Link](https://pan.baidu.com/s/1ajzBF4BFeiRFpDF558ER9w#list/path=%2F) Password: `3jey`

---

## 4. Quick Start

### 4.1 Clone the Repository

```bash
git clone https://github.com/Jieli-Tech/fw-AD16N.git
cd fw-AD16N/sdk
```

### 4.2 Project Entry Points

The SDK includes the following application projects, located in the `sdk/` root directory:

| Project File | Chip | Application Type |
|---------|------|---------|
| `AD16N_mbox_flash.cbp` | AD16N All Series | Mini Speakers / Audio player |

### 4.3 Application Code Entry Points

```
sdk/apps/app/src/mbox_flash/    # Mini speaker / Audio player application
```

### 4.4 Build and Flash

**Method 1: Code::Blocks (Recommended for Windows users)**

1. Double-click the `AD16N_mbox_flash.cbp` project file
2. Click **Build → Build** (Ctrl+F9)
3. After a successful build, use the USB Upgrade Tool to flash the generated firmware

**Method 2: Makefile (Command Line)**

```bash
# Windows users
Double-click sdk/make_prompt.bat to open the command-line environment

# Build
make -j4

# Build with verbose output
make VERBOSE=1 -j4
```

> **💡 Tip**: Before building, ensure the USB Upgrade Tool is properly connected and the target board has entered programming mode.


**Method 3: VS Code Build**

The repository comes pre-configured with VS Code tasks. Press `Ctrl+Shift+B` to select a build target.

---

## 5. Project Structure

```
fw-AD16N/
├── sdk/                           # SDK root directory
│   ├── apps/                      # Application layer code
│   │   ├── app/                   #   Application entry source code
│   │   │   ├── src/               #     Application source
│   │   │   │   └── mbox_flash/    #       Mini speaker / Audio player application
│   │   │   ├── bsp/               #     Board Support Package (BSP)
│   │   │   └── post_build/        #     Post-build scripts and tools
│   │   └── include_lib/           #   Headers and precompiled libraries
│   │       ├── cpu/               #     CPU platform headers
│   │       ├── decoder/           #     Decoder API headers
│   │       ├── encoder/           #     Encoder API headers
│   │       ├── audio/             #     Audio API headers
│   │       ├── device/            #     Device driver headers
│   │       ├── common/            #     Common headers
│   │       ├── config/            #     Configuration headers
│   │       ├── sound_effect_list/ #     Audio effects
│   │       ├── pcm_eq/            #     PCM EQ
│   │       ├── msg/               #     Message mechanism
│   │       ├── update/            #     Firmware upgrade
│   │       ├── apple_dock/        #     Apple Dock
│   │       └── liba/              #     Precompiled libraries (.a)
│   ├── tools/                     # Build tools and scripts
│   │   ├── make_prompt.bat        #   Windows build command-line launcher
│   │   └── utils/                 #   Utilities (make, rm, etc.)
│   ├── midi_2byte.bat             # MIDI 2-byte mode switch
│   ├── midi_4byte.bat             # MIDI 4-byte mode switch
│   ├── MIDI_VER_SELECT.bat        # MIDI version selection
│   ├── Makefile                   # Top-level Makefile
│   └── *.cbp                      # Code::Blocks project files
├── doc/                           # Documentation
│   ├── datasheet/                 #   Chip datasheets
│   ├── schematic/                 #   Schematics
│   ├── stuff/                     #   Miscellaneous (DingTalk group, etc.)
│   ├── README.md                  #   Chip selection guide
│   ├── AD16N_FLASH_SDK_发布版本信息.pdf  # SDK release notes
│   ├── AD16N_开源SDK手册_V1.2.pdf    # SDK manual
│   ├── AD16N_芯片手册_V1.2.pdf       # Chip manual
│   ├── AD16N通用音频MCU硬件设计指南V1.3.pdf  # Hardware design guide
│   └── 杰理科技32位AD系列语音MCU选型表.pdf  # Chip selection table
└── README.md                      # This file
```

---

## 6. Applications and Examples

### 6.1 Mini Speaker / Audio Player Application (`apps/app/src/mbox_flash/`)

| Feature | Description |
|-------|------|
| **Music Playback** | Local/external FLASH, SD card, USB flash drive file playback (supports MP3/WMA/WAV/.a/.b/.e/.f1a/.f1b/.f1c, etc.) |
| **MIDI Performance** | MIDI synthesis and playback |
| **Recording** | MP2/UMP3/A format encoding and recording |
| **USB Device** | USB slave device (Speaker / MIC / HID / MSD) |
| **LINEIN** | Digital LINEIN and analog pass-through LINEIN |
| **Loudspeaker** | Loudspeaker / voice amplifier |

Target domains: portable speakers, MP3 players, voice toys, storytelling machines, USB audio devices, etc.

---

## 7. Build Guide

### 7.1 Build Command Quick Reference

Run the following commands from the `sdk/` directory:

| Target | Command |
|------|------|
| **Build** | `make -j4` |
| **Build (verbose)** | `make VERBOSE=1 -j4` |
| **Clean** | `make clean` |

### 7.2 Code::Blocks Build (Recommended for Windows users)

1. Ensure the Jieli build toolchain is installed
2. Double-click the `AD16N_mbox_flash.cbp` project file to open Code::Blocks
3. Click **Build → Build** (Ctrl+F9)
4. After a successful build, the firmware will be generated in the `post_build/` directory

### 7.3 Makefile Build

```bash
# Windows users
Double-click sdk/make_prompt.bat to open the command-line environment
make -j4

# Linux users (requires modifying download_sh.c for Linux compatibility)
cd sdk
make -j`nproc`
```

### 7.4 Common Build Errors

| Error Message | Solution |
|---------|---------|
| `clang: command not found` | Jieli build toolchain is not installed, or environment variables are not configured |
| `cannot find -lxxx` | Missing corresponding `.a` library file; check the `apps/include_lib/liba/` directory |
| `make: command not found` | On Windows, use `tools/make_prompt.bat` to open the build command environment |
| Link errors | Verify that the Makefile target matches the current chip model |

---

## 8. Flashing and Upgrade

### 8.1 First-Time Flashing

1. **Connect Hardware**: Connect the development board to the PC via **USB** or **USB Upgrade Tool**
2. **Enter Programming Mode**:
    - Method 1 (USB): Hold the flash button on the development board, then reset or power-cycle
    - Method 2 (USB/UART): Use the USB Upgrade Tool to enter programming mode
3. **Launch USB Upgrade Tool**: Start the flashing host software
4. **Select Firmware**: Choose the compiled firmware file
5. **Start Flashing**: Click the download button and wait for completion

> **Note**: Before flashing, ensure the USB Upgrade Tool is properly connected and the target board has entered programming mode. For details on ISD_CONFIG.INI, see [ISD Configuration Guide](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/toolchains/ini_cfg.html).

### 8.2 Mass Production Flashing

For mass production scenarios, use Jieli's mass production burner (one-to-two / one-to-eight), which supports bare-die programming. See [One-to-Two Burner User Guide](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) · [One-to-Eight Burner User Guide](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo8/index.html)

### 8.3 OTA Upgrade

Supports dual-bank custom firmware upgrade, including USB flash drive upgrade, SD card upgrade, UART upgrade, and more.

---

## 9. Configuration

- Edit `sdk/apps/app/src/mbox_flash/app_config.h` to configure feature toggles for the target application

---

## 10. FAQ

### 10.1 Development Workflow

**Q: How do I select a different chip model?**
A: Select the corresponding target via the Makefile, or modify the chip configuration in `app_config.h`.

**Q: How do I switch between internal and external FLASH?**
A: Specify the Flash type in the configuration. External Flash is accessed via the SPI Flash controller.

### 10.2 Build Issues

**Q: On Windows, `make` is reported as an invalid command?**
A: Use `sdk/make_prompt.bat` to enter the pre-configured command-line environment, which sets up all environment variables and the `make` path.

**Q: How can I speed up compilation?**
A: Use the `-j` flag for parallel compilation, e.g. `make -j4` (the number specifies the parallel job count).

### 10.3 Debugging Tips

- **UART Logging**: Debug logs can be output via UART
- **GPIO Debug**: Use idle GPIO pins to output debug waveforms for timing measurement

---

## 11. Community and Support

### Technical Discussion

| Platform | Group / Link | Status |
|------|-----------|------|
| **DingTalk Tech Group** | See [Group QR Code](doc/stuff/dingtalk.jpg) | ✅ Joinable |

### Resource Links

| Resource | Link |
|------|------|
| 📖 **Jieli Tools Online Documentation** | [doc.zh-jieli.com/Tools](https://doc.zh-jieli.com/Tools/zh-cn/index.html) |
| 📚 **SDK Release History** | [SDK Release Notes](doc/AD16N_FLASH_SDK_发布版本信息.pdf) |
| 🔧 **SDK Quick Start** | [SDK Manual](doc/AD16N_开源SDK手册_V1.2.pdf) |
| 📄 **Chip Selection Guide** | [Selection Table](doc/杰理科技32位AD系列语音MCU选型表.pdf) · [Selection Guide](doc/README.md) |
| 🎬 **Video Tutorials** | [Bilibili Homepage](https://space.bilibili.com/3493277347088769/dynamic) |
| 🎬 **SDK Training Video** | [AD16N FLASH SDK Training](https://www.bilibili.com/video/BV15T411a7YE/) |
| 🎵 **MIDI Development Manual** | [MIDI Application Development Guide](https://doc.zh-jieli.com/MIDI/zh-cn/master/index.html) |
| 📦 **FAE Support** | [FAE Support Repository](https://gitee.com/jieli-tech_fae/fw-jl) |
| 🛒 **Dev Board Purchase** | [Development Evaluation Board](https://item.taobao.com/item.htm?id=696613076397) |
| 🛒 **Flashing Tool Purchase** | [Jieli Official Store](https://shop321455197.taobao.com/) |
| 🐛 **Issue Tracker** | [Github Issues](https://github.com/Jieli-Tech/fw-AD16N/issues) |

---

## 12. Disclaimer

`fw-AD16N_GP-MCU_SDK` supports development for the AD16N series chips. These chips support common general-purpose MCU applications and may be used for development, evaluation, sampling, and mass production. For the corresponding SDK version, please refer to [SDK Release History](doc/AD16N_FLASH_SDK_发布版本信息.pdf).

---

<div align="center">
  <sub>Copyright &copy; Zhuhai Jieli Technology Co., Ltd. All rights reserved.</sub>
</div>
