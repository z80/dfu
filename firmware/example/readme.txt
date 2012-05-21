******************************************************************************
  Demo for ChaN's FAT-module on STMicroelectronics STM32 microcontroller
  Version Timestamp 20100704
  by Martin Thomas, Kaiserslautern, Germany <mthomas(at)rhrk.uni-kl.de>
******************************************************************************
  Copyright (c) 2009, 2010
  - ChaN (FAT-Code, driver-template, ff_test_term)
  - Martin Thomas (STM32 SD/MMC/SDHC driver and demo-application)
  - ST Microelectronics (Standard Peripherals Firmware Library)
  - ITB CompuPhase (minIni)
  All rights reserved.

  License for this product follows. Licenses of the components may differ
  but should be compatible. Read the license- and readme-files in the 
  library directory.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
******************************************************************************

Changelog:
4. July 2010:
 - STM Library update to version 3.3.0
 - Makefile: modified for STMLib 3.3.0 directory-structure
 - Makefile: parsing for cygdrive modified
 - Makefile: based on a newer "WinARM-template"
 - main.c: replaced SystemFrequency with SystemCoreClock (see CMSIS V1.20 changes)
 - stm32f10x.h: modification to take stdbool.h into account
 - startup_*_mthomas.c, main.c: SystemInit now called in startup (sync with 
   startup_stm32f10x_md.s V3.3.0)
 - system_stm32f10x.c: some clock-settings take HSE_VALUE 4 and 8 MHz
   into account (one of the boards here has a 4MHz XTAL)
 - ff.c: included ChaN's patch for FatFs R0.07e from January 15, 2010
 - main.c: removed UART loop-test (see STMicro's STM32 StdLib examples if needed),
   added ASCII word-art gimmick (generated with http://www.ihr-freelancer.de/asciiart/)
 - comm.h/.c: added comm_puts
 - comm.h, rtc.h, main.h: added/modified multiple include guards
 - sd_spi_stm32.c: separated card-present and write-protect for boards where only
   one of the switches is connected
 - used Sourcery G++ Lite 2010q1-188 for tests
 - renamed target to project for easier reuse of Eclipse debug-configurations
 - µVision Version 4 Workspace (still just for GNU tools), removed µVision3 settings
 - main.c adjusted pre-scale counters
14. Jan. 2010:
 - ChaN FAT-module update to V0.07e
 - STM Library update to 3.1.2 and included my patches made for 3.1.0
 - ITB minIni update to V0.8, very minor modifications to avoid warnings
   (marked with "mt:")
 - eeprom.h/eeprom.c: update to STM's AppNote version 3, included the 
   minor modification to define memory-area for emulated eeprom in linker-script
 - stm32_sd_spi.c: take ffconf.h into account, avoid warnings for read-only 
   configuration, updated comments, replaced USE_SPI_DMA by USE_SD_SPI_DMA
 - stm32_sd_spi.c: fixed handling of card-present and write-protect socket-switches
   (thanks to Kazuaki Hamahara)
 - stm32_sd_spi.c: fixed DMA setup to take SPI_SD into account
   (thanks to "Ringo Pappnase")
 - ff.c: minor modification: avoid warning for read-only configuration,
   corrected some typos
 - ffconf.h: code-page to 1252 Latin 1 (Windows)
 - Makefile: activated compilation for STM EEPROM-emulation code, modification
   for better support in "Windows-only" setup (w/o MinGW/msys or cygwin)
 - Linker-script: memory-region and start-address of emulated eeprom
 - misc_test_term.c: show FATFS and STM32F10X-STDPERIPH version (v)
 - misc_test_term.c: added EEPROM emulation demo (ef, ew, er)
 - misc_test_term.c: demo-function for newlib's stdio (disabled by default),
   demo-function for DCC
 - stm32f10x_conf.h, main.c: show function-name in assert_failed
 - platform_config.c, sd_spi_stm32.c, main.c, Makefile: added support for 
   MINI STM32 board
 - main.c/main(): GPIO_Write to GPIO_SetBits for LEDs to avoid side-effects
 - startup_stm32f10x_md_mthomas.c: added C++ support 
   (commented the init call since not a C++ project)
 - STM32F10x_128k_20k_flash.ld: added C++ support
 - syscalls.c: minimal implementations for newlib syscalls (i.e. for stdio printf)
 - dcc_stdio.h/.c: files from OpenOCD source-tree with minor modifications, 
   used to interface with DCC (i.e. for "printf-debugging" via JTAG connection).
   Both files are GPL2-licensed but they are just optional, so remove them if
   the GPL is not an option for your project
 - used tool-chain Codesourcery G++ lite for ARM Q3-2009 for tests
 2. Sept. 2009:
 - STM Library update to 3.1.0, minor modifications (enabled by define)
 - ChaN FAT-Module library update to 0.07c with patches up to 27. July 2009
 - additional configuration-options in makefile
 - included minIni from ITB CompuPhase and added small demo for it
 - extended SPI to SD/MMC/SDHC driver (sd_spi_stm32.c) with some config-options
 - minor modifications and extensions in the demo-application,
   linker-script, startup-code
 - OpenOCD SVN 2660 is just included for evaluation. There is some kind of
   licensing-issue with the FTDI-drivers in combination with OpenOCD
   (the discussion on this has been too long to follow). *** You must 
   delete the OpenOCD folder now since it might violate a license. ***
 - tested with Codesourcery G++ lite Q1-2009
 2. May  2009: 
 - OpenOCD updated to SVN1586
 - Using target/stm32.cfg in makefile and Eclipse external tool launch
 - Eclipse debug configuration debug attach
 - renamed and modified in Eclipse debug launches
 - minor modifications in this readme.txt to somehow document the mentioned
   changes



This is a small demo-application to show how Chan's Fat-Module
can be used to interface MMC/SD/SDHC-Cards with a STM32 microcontroller
(ARM Ltd. Cortex-M3 core).

This code uses the _SPI-Interface_ - NOT a SDIO-Interface since the
Eval-Board here has the card-slot connected to SPI1 and the F103VB controller
on it does not offer SDIO. When using a STM32 with SDIO and a board which
has a card-slot connected to the SDIO the example code from the STM fwlib
can be used as low-level driver ("diskio").

Beside of the diskio-drivers for STM32 SPI you may find some useful code 
for developments of firmware with the GNU tool-chain in this package (for 
example: vectors-location, startup, linker-script, OpenOCD flash-
programming).

I have done only minor modification in the STM firmware-library to avoid 
warnings The modifications can be enabled by defining MOD_MTHOMAS_STMLIB
(enabled by default). Special versions of startup-code 
(startup_stm32f10x_md_mthomas.c) and linker-script (stm32f103vb_flash.ld) 
have been created for this project. They are based on the examples that 
came with the STM Standard Peripherals Library but have been heavily
modified.

Project-files for Keil/ARM uVision are included. The evaluation-version
will do to build the project using a GNU tool-chain. For debugging
and simulation you may have to use the full version without
size-limitation (not tested here). For stand-alone building i.e
from Command-line, Programmer's Notepad or Eclipse a makefile is 
included.

The package includes Eclipse-settings and launch-configurations.
Sorry, not enough time to fully document this here. Just load the
workspace and give it a try. Very short guide:
- External Tools "OpenOCD": starts OpenOCD in the background:
  This uses the OpenOCD target-library. The current configuration
  is for an Amontec JTAGkey2 interface.
  When using another interface modify the command-line options in 
  Run->External Tools->Ext. Tools conf.->leaf OpenOCD->field arguments. 
  For all following debug configurations OpenOCD has to run and it can
  be kept running. OpenOCD can be terminated (right-click-Terminate) 
  in the Debug view when something hangs and it has to be restarted.
- Debug Configuration (launch) "OpenOCD reset load run":
  Transfers the load-image into the STM32's internal flash using
  gdb's load-command and starts the application. Similar to 
  "make program" but no need to terminate a running OpenOCD and
  restart it with different command-line-options or scripts.
- Debug Configuration "OpenOCD reset debug":
  Start debugging with an application already loaded into the
  controller's flash
- Debug Configuration "OpenOCD reset load debug":
  Transfers the load-image into the flash-memory and starts
  debugging (similar to "OpenOCD load init only" followed by
  "OpenOCD reset debug")
- Debug Configuration "OpenOCD attach debug":
  Attaches to target w/o reset.
- Debug Configuration "OpenOCD resume":
  Just resumes the target in case it is halted.
All this has been used with the CDT GBD hardware debugging plug-in.
Similar configurations may work with the embedded debug plug-in 
from Zylin but I have not tested with Zylin's plug-in. 

All used codes should be free for commercial and
non-commercial use. They are either non-copyrighted
or under a BSD-style license. The only exceptions 
are dcc_stdio.h and dcc_stdio.c which are just optional.
There is no need to give away source-code which uses 
parts of this project as long as the dcc_stdio functions
are not used. 
My modifications are also basically free, just read 
and respect the 3BSD-license at the top of this text.
Of cause there is no warranty neither for the codes 
from others nor for to code I have written.


Tested with:
- Controller: STM32F103VB Rev.Z ("medium density" 128kB)
- Board: Manley EK-STM32F, SD-slot connected to STM32's SPI1
  (this board does not connect card-detect and write-protect switches)
- SD-Cards: extrememory 1GB performace, SanDisk 256MB
- SDHC Card: SanDisk Ultra II SDHC 4GB


Tools:
- Compiler/Toolchain: GNU cross-toolchain for ARM (binutils, gcc)
- libc: newlib (only a few functions used)
  used packages: Codesourcery G++ lite for ARM EABI
- IDEs/Editors: 
  - Programmers Notepad
  - Eclipse with CDT as in package Eclipse IDE for C/C++ Developers
    Additional plug-ins: CDT Hardware Debugging, Target Management Terminal,
    RXTX end-user Runtime.
  - Keil/ARM MDK-ARM/µVision, µVision has only been used as IDE and for 
    a few tests with the simulator (the simulator is a very good tool!). 
    No effort has been made to make the code compatible with the RealView 
    Tools.
- other tool-software: OpenOCD for "batch programming" (make program)
  and debugging with Eclipse/CDT GDB Hardware Debugging/OpenOCD
- JTAG-Interfaces: Amontec JTAGKey1/JTAGkey2 (Eclipse, make program), 
  Keil ULINK"1" (uVision). I have not used the "ST-LINKII" integrated 
  on the Manley Eval.-Board since it did not work reliably with uVision 
  and there is no support for it in OpenOCD.
- This package comes with a OpenOCD directory including a Win32 binary
  for FT2232 and par.-port interfaces. If you are using a different
  version of OpenOCD or another interface you may have to modify the settings 
  (Makefile, target-library).
******************************************************************************


Demo-Session with FAT-LFN and SPI-DMA enabled, using a 8kByte work-buffer
and extrememory 1GB "performance" SD-Card:


Hello from a STM32 Demo-Application, M. Thomas
V1.0.40 8/2009

FatFs module test terminal
>fi 0
rc=0 FR_OK
>fm 0 0
The drive 0 will be formatted. Are you sure? (Y/n)=Y
rc=0 FR_OK
>fs
FAT type = 2 (FAT16)
Bytes/Cluster = 16384
Number of FATs = 1
Root DIR entries = 512
Sectors/FAT = 288
Number of clusters = 62484
FAT start (lba) = 64
DIR start (lba,clustor) = 352
Data start (lba) = 384

0 files, 0 bytes.
0 folders.
999744 KB total disk space.
999744 KB available.
>fl
   0 File(s),          0 bytes total
   0 Dir(s),  1023737856 bytes free
>fo 10 test
rc=0 FR_OK
>fw 5000000 10
5000000 bytes written with 1488 kB/sec.
>fw 5000000 11
5000000 bytes written with 1340 kB/sec.
>fw 5000000 12
5000000 bytes written with 1350 kB/sec.
>fc
rc=0 FR_OK
>fl
----A 2000/01/01 12:07  15000000  test  
   1 File(s),   15000000 bytes total
   0 Dir(s),  1008730112 bytes free
>fx test test2
Opening "test"
Creating "test2"
Copying file...EOF
15000000 bytes copied with 759 kB/sec.
>fo 1 test2
rc=0 FR_OK
>fr 5000000
5000000 bytes read with 1730 kB/sec.
>fr 5000000
5000000 bytes read with 1484 kB/sec.
>fr 5000000
5000000 bytes read with 1485 kB/sec.
>t
2000/1/1 12:08:26
>t 2009 8 29 12 17 20
2009/8/29 12:17:20
>t
2009/8/29 12:17:25
>fo 10 datetest
rc=0 FR_OK
>fw 100 1
100 bytes written with 50 kB/sec.
>fc
rc=0 FR_OK
>fl
----A 2000/01/01 12:07  15000000  test  
----A 2000/01/01 12:07  15000000  test2  
----A 2009/08/29 12:17       100  datetest  
   3 File(s),   30000100 bytes total
   0 Dir(s),   993705984 bytes free
>fo 10 Mülleimer
rc=0 FR_OK
>fw 1 1
1 bytes written with 0 kB/sec.
>fc
rc=0 FR_OK
>fl
----A 2000/01/01 12:07  15000000  test  
----A 2000/01/01 12:07  15000000  test2  
----A 2009/08/29 12:17       100  datetest  
----A 2009/08/29 12:18         1  MüLLEI~1  Mülleimer
   4 File(s),   30000101 bytes total
   0 Dir(s),   993689600 bytes free
>



Hello from a STM32 Demo-Application, M. Thomas
V1.0.40 8/2009

FatFs module test terminal
>fl
rc=12 FR_INVALID_OBJECT
>fi 0
rc=0 FR_OK
>fl
----A 2000/01/01 12:07  15000000  test  
----A 2000/01/01 12:07  15000000  test2  
----A 2009/08/29 12:17       100  datetest  
----A 2009/08/29 12:18         1  MüLLEI~1  Mülleimer
----A 2009/08/29 13:02         0  TEST.INI  
----A 2009/08/29 13:00    133520  sampled.bin  
   6 File(s),   30133621 bytes total
   0 Dir(s),   993542144 bytes free
>fu test.ini
rc=0 FR_OK
>fl
----A 2000/01/01 12:07  15000000  test  
----A 2000/01/01 12:07  15000000  test2  
----A 2009/08/29 12:17       100  datetest  
----A 2009/08/29 12:18         1  MüLLEI~1  Mülleimer
----A 2009/08/29 13:00    133520  sampled.bin  
   5 File(s),   30133621 bytes total
   0 Dir(s),   993542144 bytes free
>x

Misc test terminal (h for help)
>ic
>x

FatFs module test terminal
>fl
----A 2000/01/01 12:07  15000000  test  
----A 2000/01/01 12:07  15000000  test2  
----A 2009/08/29 12:17       100  datetest  
----A 2009/08/29 12:18         1  MüLLEI~1  Mülleimer
----A 2009/08/29 13:03        97  TEST.INI  
----A 2009/08/29 13:00    133520  sampled.bin  
   6 File(s),   30133718 bytes total
   0 Dir(s),   993525760 bytes free
>fo 1 test.ini
rc=0 FR_OK
>fd 1000
00000000  5B 49 4E 46 4F 5D 0D 0A 63 6F 6D 70 61 6E 79 20 [INFO]..company 
00000010  3D 20 4D 61 72 74 69 6E 20 54 68 6F 6D 61 73 20 = Martin Thomas 
00000020  53 6F 66 74 77 61 72 65 20 45 6E 67 69 6E 65 65 Software Enginee
00000030  72 69 6E 67 0D 0A 61 75 74 68 6F 72 20 3D 20 4D ring..author = M
00000040  61 72 74 69 6E 20 54 68 6F 6D 61 73 0D 0A 6E 75 artin Thomas..nu
00000050  6D 62 65 72 20 3D 20 31 32 33 34 35 36 37 38 0D mber = 12345678.
00000060  0A .
>fc
rc=0 FR_OK
>x

Misc test terminal (h for help)
>ir
INFO/company: Martin Thomas Software Engineering
non-existing entry: <default>
INFO/number: 12345678
>

Hello from a STM32 Demo-Application, M. Thomas
V1.0.41 9/2009

FatFs module test terminal
>x

Misc test terminal (h for help)
>v
Implementer ARM, PartNo Cortex-M3, Prod.-Vers.-ID r1p1
Vector table at 0x00000000 in SRAM (TBLBASE)
Flash size 128 kBytes
Unique Device ID 0x43162932:0x31383836:0xFFFF:0xFF37
SD/MMC/SDHC Interface uses SPI DMA
>ef
Format o.k.
>ew 0 123
write EEMUL[0] := 123 o.k.
>ew 1 456
write EEMUL[1] := 456 o.k.
>er 2
read EEMUL[2] failed!
>ew 2 789
write EEMUL[2] := 789 o.k.
>er 0
read EEMUL[0] = 123
>er 1
read EEMUL[1] = 456
>er 2
read EEMUL[2] = 789
>ew 0 999
write EEMUL[0] := 999 o.k.
>er 0
read EEMUL[0] = 999
>
Hello from a STM32 Demo-Application, M. Thomas
V1.0.42 9/2009

FatFs module test terminal
>x

Misc test terminal (h for help)
>er 0
read EEMUL[0] = 999
>er 1
read EEMUL[1] = 456
>er 2
read EEMUL[2] = 789
>


******************************************************************************

Log of a complete build incl. flash-programming

C:\WinARM\examples\stm32_chan_fat\project>make clean all program
COMSPEC detected  C:\WINDOWS\system32\cmd.exe
SHELL is C:/WINDOWS/system32/cmd.exe, REMOVE_CMD is cs-rm
--------  begin, mode: FLASH_RUN  --------
Cleaning project:
cs-rm -f FLASH_RUN/project.map
cs-rm -f FLASH_RUN/project.elf
cs-rm -f FLASH_RUN/project.hex
cs-rm -f FLASH_RUN/project.bin
cs-rm -f FLASH_RUN/project.sym
cs-rm -f FLASH_RUN/project.lss
cs-rm -f FLASH_RUN/main.o FLASH_RUN/rtc.o FLASH_RUN/comm.o FLASH_RUN/term_io.o F
LASH_RUN/ff_test_term.o FLASH_RUN/misc_test_term.o FLASH_RUN/ff.o FLASH_RUN/ccsb
cs.o FLASH_RUN/fattime.o FLASH_RUN/sd_spi_stm32.o FLASH_RUN/startup_stm32f10x_md
_mthomas.o FLASH_RUN/syscalls.o FLASH_RUN/dcc_stdio.o FLASH_RUN/minIni.o FLASH_R
UN/core_cm3.o FLASH_RUN/system_stm32f10x.o FLASH_RUN/stm32f10x_usart.o FLASH_RUN
/stm32f10x_flash.o FLASH_RUN/stm32f10x_gpio.o FLASH_RUN/stm32f10x_rcc.o FLASH_RU
N/stm32f10x_spi.o FLASH_RUN/stm32f10x_rtc.o FLASH_RUN/stm32f10x_bkp.o FLASH_RUN/
stm32f10x_pwr.o FLASH_RUN/stm32f10x_dma.o FLASH_RUN/stm32f10x_tim.o FLASH_RUN/mi
sc.o FLASH_RUN/eeprom.o
cs-rm -f FLASH_RUN/main.lst FLASH_RUN/rtc.lst FLASH_RUN/comm.lst FLASH_RUN/term_
io.lst FLASH_RUN/ff_test_term.lst FLASH_RUN/misc_test_term.lst FLASH_RUN/ff.lst
FLASH_RUN/ccsbcs.lst FLASH_RUN/fattime.lst FLASH_RUN/sd_spi_stm32.lst FLASH_RUN/
startup_stm32f10x_md_mthomas.lst FLASH_RUN/syscalls.lst FLASH_RUN/dcc_stdio.lst
FLASH_RUN/minIni.lst FLASH_RUN/core_cm3.lst FLASH_RUN/system_stm32f10x.lst FLASH
_RUN/stm32f10x_usart.lst FLASH_RUN/stm32f10x_flash.lst FLASH_RUN/stm32f10x_gpio.
lst FLASH_RUN/stm32f10x_rcc.lst FLASH_RUN/stm32f10x_spi.lst FLASH_RUN/stm32f10x_
rtc.lst FLASH_RUN/stm32f10x_bkp.lst FLASH_RUN/stm32f10x_pwr.lst FLASH_RUN/stm32f
10x_dma.lst FLASH_RUN/stm32f10x_tim.lst FLASH_RUN/misc.lst FLASH_RUN/eeprom.lst
cs-rm -f FLASH_RUN/dep/main.o.d FLASH_RUN/dep/rtc.o.d FLASH_RUN/dep/comm.o.d FLA
SH_RUN/dep/term_io.o.d FLASH_RUN/dep/ff_test_term.o.d FLASH_RUN/dep/misc_test_te
rm.o.d FLASH_RUN/dep/ff.o.d FLASH_RUN/dep/ccsbcs.o.d FLASH_RUN/dep/fattime.o.d F
LASH_RUN/dep/sd_spi_stm32.o.d FLASH_RUN/dep/startup_stm32f10x_md_mthomas.o.d FLA
SH_RUN/dep/syscalls.o.d FLASH_RUN/dep/dcc_stdio.o.d FLASH_RUN/dep/minIni.o.d FLA
SH_RUN/dep/core_cm3.o.d FLASH_RUN/dep/system_stm32f10x.o.d FLASH_RUN/dep/stm32f1
0x_usart.o.d FLASH_RUN/dep/stm32f10x_flash.o.d FLASH_RUN/dep/stm32f10x_gpio.o.d
FLASH_RUN/dep/stm32f10x_rcc.o.d FLASH_RUN/dep/stm32f10x_spi.o.d FLASH_RUN/dep/st
m32f10x_rtc.o.d FLASH_RUN/dep/stm32f10x_bkp.o.d FLASH_RUN/dep/stm32f10x_pwr.o.d
FLASH_RUN/dep/stm32f10x_dma.o.d FLASH_RUN/dep/stm32f10x_tim.o.d FLASH_RUN/dep/mi
sc.o.d FLASH_RUN/dep/eeprom.o.d
cs-rm -f main.s rtc.s comm.s term_io.s ff_test_term.s misc_test_term.s ./Librari
es/fat_sd/ff.s ./Libraries/fat_sd/option/ccsbcs.s ./Libraries/fat_sd/fattime.s .
/Libraries/fat_sd/sd_spi_stm32.s startup_stm32f10x_md_mthomas.s syscalls.s dcc_s
tdio.s ./Libraries/minIni/minIni.s ./Libraries/CMSIS/CM3/CoreSupport/core_cm3.s
./Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.s ./Libraries/
STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.s ./Libraries/STM32F10x_StdPeriph
_Driver/src/stm32f10x_flash.s ./Libraries/STM32F10x_StdPeriph_Driver/src/stm32f1
0x_gpio.s ./Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.s ./Libraries
/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.s ./Libraries/STM32F10x_StdPeriph_
Driver/src/stm32f10x_rtc.s ./Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_
bkp.s ./Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.s ./Libraries/STM
32F10x_StdPeriph_Driver/src/stm32f10x_dma.s ./Libraries/STM32F10x_StdPeriph_Driv
er/src/stm32f10x_tim.s ./Libraries/STM32F10x_StdPeriph_Driver/src/misc.s ./Libra
ries/EEPROMEmulation_AN/source/eeprom.s
cs-rm -f
cs-rm -f
cs-rm -f
--------  end  --------
arm-none-eabi-gcc (Sourcery G++ Lite 2010q1-188) 4.4.1
Copyright (C) 2009 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

---- Compiling C: main.c to FLASH_RUN/main.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/main.lst -MM
D -MP -MF FLASH_RUN/dep/main.o.d -Wnested-externs  -std=gnu99 main.c -o FLASH_RU
N/main.o
---- Compiling C: rtc.c to FLASH_RUN/rtc.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/rtc.lst -MMD
 -MP -MF FLASH_RUN/dep/rtc.o.d -Wnested-externs  -std=gnu99 rtc.c -o FLASH_RUN/r
tc.o
---- Compiling C: comm.c to FLASH_RUN/comm.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/comm.lst -MM
D -MP -MF FLASH_RUN/dep/comm.o.d -Wnested-externs  -std=gnu99 comm.c -o FLASH_RU
N/comm.o
---- Compiling C: term_io.c to FLASH_RUN/term_io.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/term_io.lst
-MMD -MP -MF FLASH_RUN/dep/term_io.o.d -Wnested-externs  -std=gnu99 term_io.c -o
 FLASH_RUN/term_io.o
---- Compiling C: ff_test_term.c to FLASH_RUN/ff_test_term.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/ff_test_term
.lst -MMD -MP -MF FLASH_RUN/dep/ff_test_term.o.d -Wnested-externs  -std=gnu99 ff
_test_term.c -o FLASH_RUN/ff_test_term.o
---- Compiling C: misc_test_term.c to FLASH_RUN/misc_test_term.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/misc_test_te
rm.lst -MMD -MP -MF FLASH_RUN/dep/misc_test_term.o.d -Wnested-externs  -std=gnu9
9 misc_test_term.c -o FLASH_RUN/misc_test_term.o
---- Compiling C: Libraries/fat_sd/ff.c to FLASH_RUN/ff.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/ff.lst -MMD
-MP -MF FLASH_RUN/dep/ff.o.d -Wnested-externs  -std=gnu99 Libraries/fat_sd/ff.c
-o FLASH_RUN/ff.o
---- Compiling C: Libraries/fat_sd/option/ccsbcs.c to FLASH_RUN/ccsbcs.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/ccsbcs.lst -
MMD -MP -MF FLASH_RUN/dep/ccsbcs.o.d -Wnested-externs  -std=gnu99 Libraries/fat_
sd/option/ccsbcs.c -o FLASH_RUN/ccsbcs.o
---- Compiling C: Libraries/fat_sd/fattime.c to FLASH_RUN/fattime.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/fattime.lst
-MMD -MP -MF FLASH_RUN/dep/fattime.o.d -Wnested-externs  -std=gnu99 Libraries/fa
t_sd/fattime.c -o FLASH_RUN/fattime.o
---- Compiling C: Libraries/fat_sd/sd_spi_stm32.c to FLASH_RUN/sd_spi_stm32.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/sd_spi_stm32
.lst -MMD -MP -MF FLASH_RUN/dep/sd_spi_stm32.o.d -Wnested-externs  -std=gnu99 Li
braries/fat_sd/sd_spi_stm32.c -o FLASH_RUN/sd_spi_stm32.o
Libraries/fat_sd/sd_spi_stm32.c:45: note: #pragma message: *** Using DMA ***
---- Compiling C: startup_stm32f10x_md_mthomas.c to FLASH_RUN/startup_stm32f10x_
md_mthomas.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/startup_stm3
2f10x_md_mthomas.lst -MMD -MP -MF FLASH_RUN/dep/startup_stm32f10x_md_mthomas.o.d
 -Wnested-externs  -std=gnu99 startup_stm32f10x_md_mthomas.c -o FLASH_RUN/startu
p_stm32f10x_md_mthomas.o
---- Compiling C: syscalls.c to FLASH_RUN/syscalls.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/syscalls.lst
 -MMD -MP -MF FLASH_RUN/dep/syscalls.o.d -Wnested-externs  -std=gnu99 syscalls.c
 -o FLASH_RUN/syscalls.o
---- Compiling C: dcc_stdio.c to FLASH_RUN/dcc_stdio.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/dcc_stdio.ls
t -MMD -MP -MF FLASH_RUN/dep/dcc_stdio.o.d -Wnested-externs  -std=gnu99 dcc_stdi
o.c -o FLASH_RUN/dcc_stdio.o
---- Compiling C: Libraries/minIni/minIni.c to FLASH_RUN/minIni.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/minIni.lst -
MMD -MP -MF FLASH_RUN/dep/minIni.o.d -Wnested-externs  -std=gnu99 Libraries/minI
ni/minIni.c -o FLASH_RUN/minIni.o
---- Compiling C: Libraries/CMSIS/CM3/CoreSupport/core_cm3.c to FLASH_RUN/core_c
m3.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/core_cm3.lst
 -MMD -MP -MF FLASH_RUN/dep/core_cm3.o.d -Wnested-externs  -std=gnu99 Libraries/
CMSIS/CM3/CoreSupport/core_cm3.c -o FLASH_RUN/core_cm3.o
---- Compiling C: Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10
x.c to FLASH_RUN/system_stm32f10x.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/system_stm32
f10x.lst -MMD -MP -MF FLASH_RUN/dep/system_stm32f10x.o.d -Wnested-externs  -std=
gnu99 Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c -o FLASH
_RUN/system_stm32f10x.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c to
FLASH_RUN/stm32f10x_usart.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_us
art.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_usart.o.d -Wnested-externs  -std=gn
u99 Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c -o FLASH_RUN/stm3
2f10x_usart.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c to
FLASH_RUN/stm32f10x_flash.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_fl
ash.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_flash.o.d -Wnested-externs  -std=gn
u99 Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c -o FLASH_RUN/stm3
2f10x_flash.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c to F
LASH_RUN/stm32f10x_gpio.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_gp
io.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_gpio.o.d -Wnested-externs  -std=gnu9
9 Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c -o FLASH_RUN/stm32f1
0x_gpio.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c to FL
ASH_RUN/stm32f10x_rcc.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_rc
c.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_rcc.o.d -Wnested-externs  -std=gnu99
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c -o FLASH_RUN/stm32f10x_
rcc.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c to FL
ASH_RUN/stm32f10x_spi.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_sp
i.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_spi.o.d -Wnested-externs  -std=gnu99
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c -o FLASH_RUN/stm32f10x_
spi.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c to FL
ASH_RUN/stm32f10x_rtc.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_rt
c.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_rtc.o.d -Wnested-externs  -std=gnu99
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c -o FLASH_RUN/stm32f10x_
rtc.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c to FL
ASH_RUN/stm32f10x_bkp.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_bk
p.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_bkp.o.d -Wnested-externs  -std=gnu99
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c -o FLASH_RUN/stm32f10x_
bkp.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c to FL
ASH_RUN/stm32f10x_pwr.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_pw
r.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_pwr.o.d -Wnested-externs  -std=gnu99
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c -o FLASH_RUN/stm32f10x_
pwr.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c to FL
ASH_RUN/stm32f10x_dma.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_dm
a.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_dma.o.d -Wnested-externs  -std=gnu99
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c -o FLASH_RUN/stm32f10x_
dma.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c to FL
ASH_RUN/stm32f10x_tim.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/stm32f10x_ti
m.lst -MMD -MP -MF FLASH_RUN/dep/stm32f10x_tim.o.d -Wnested-externs  -std=gnu99
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c -o FLASH_RUN/stm32f10x_
tim.o
---- Compiling C: Libraries/STM32F10x_StdPeriph_Driver/src/misc.c to FLASH_RUN/m
isc.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/misc.lst -MM
D -MP -MF FLASH_RUN/dep/misc.o.d -Wnested-externs  -std=gnu99 Libraries/STM32F10
x_StdPeriph_Driver/src/misc.c -o FLASH_RUN/misc.o
---- Compiling C: Libraries/EEPROMEmulation_AN/source/eeprom.c to FLASH_RUN/eepr
om.o
arm-none-eabi-gcc -c -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM3
2F10X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIV
ER -DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE
_FULL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./L
ibraries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST
/STM32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Librarie
s/minIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata
-sections -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredu
ndant-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/eeprom.lst -
MMD -MP -MF FLASH_RUN/dep/eeprom.o.d -Wnested-externs  -std=gnu99 Libraries/EEPR
OMEmulation_AN/source/eeprom.c -o FLASH_RUN/eeprom.o
---- Linking: FLASH_RUN/project.elf
arm-none-eabi-gcc -mthumb -ggdb -Os -mcpu=cortex-m3 -mthumb-interwork  -DSTM32F1
0X_MD -DHSE_VALUE=8000000UL -DSYSCLK_FREQ_72MHz=72000000 -DUSE_STDPERIPH_DRIVER
-DUSE_EK_STM32F -DSTM32_SD_USE_DMA -DSTARTUP_DELAY -DMOD_MTHOMAS_STMLIB -DUSE_FU
LL_ASSERT -DVECT_TAB_FLASH -DFLASH_RUN -DSTM32F10x_128k_20k -DEK_STM32F -I./Libr
aries/STM32F10x_StdPeriph_Driver/inc -I./Libraries/CMSIS/CM3/DeviceSupport/ST/ST
M32F10x -I./Libraries/CMSIS/CM3/CoreSupport -I./Libraries/fat_sd -I./Libraries/m
inIni -I./Libraries/EEPROMEmulation_AN/include -I. -ffunction-sections -fdata-se
ctions -Wall -Wextra -pedantic -Wimplicit -Wcast-align -Wpointer-arith -Wredunda
nt-decls -Wshadow -Wcast-qual -Wcast-align -Wa,-adhlns=FLASH_RUN/main.lst -MMD -
MP -MF FLASH_RUN/dep/project.elf.d FLASH_RUN/main.o FLASH_RUN/rtc.o FLASH_RUN/co
mm.o FLASH_RUN/term_io.o FLASH_RUN/ff_test_term.o FLASH_RUN/misc_test_term.o FLA
SH_RUN/ff.o FLASH_RUN/ccsbcs.o FLASH_RUN/fattime.o FLASH_RUN/sd_spi_stm32.o FLAS
H_RUN/startup_stm32f10x_md_mthomas.o FLASH_RUN/syscalls.o FLASH_RUN/dcc_stdio.o
FLASH_RUN/minIni.o FLASH_RUN/core_cm3.o FLASH_RUN/system_stm32f10x.o FLASH_RUN/s
tm32f10x_usart.o FLASH_RUN/stm32f10x_flash.o FLASH_RUN/stm32f10x_gpio.o FLASH_RU
N/stm32f10x_rcc.o FLASH_RUN/stm32f10x_spi.o FLASH_RUN/stm32f10x_rtc.o FLASH_RUN/
stm32f10x_bkp.o FLASH_RUN/stm32f10x_pwr.o FLASH_RUN/stm32f10x_dma.o FLASH_RUN/st
m32f10x_tim.o FLASH_RUN/misc.o FLASH_RUN/eeprom.o --output FLASH_RUN/project.elf
 -nostartfiles -Wl,-Map=FLASH_RUN/project.map,--cref,--gc-sections -lc -lm -lc -
lgcc -lstdc++  -L.   -T./STM32F10x_128k_20k_flash.ld
Creating Extended Listing/Disassembly: FLASH_RUN/project.lss
arm-none-eabi-objdump -h -S -C -r FLASH_RUN/project.elf > FLASH_RUN/project.lss
Creating Symbol Table: FLASH_RUN/project.sym
arm-none-eabi-nm -n FLASH_RUN/project.elf > FLASH_RUN/project.sym
Creating load file: FLASH_RUN/project.hex
arm-none-eabi-objcopy -O ihex FLASH_RUN/project.elf FLASH_RUN/project.hex
Creating load file: FLASH_RUN/project.bin
arm-none-eabi-objcopy -O binary FLASH_RUN/project.elf FLASH_RUN/project.bin
Size after build:
arm-none-eabi-size -A  FLASH_RUN/project.elf
FLASH_RUN/project.elf  :
section             size        addr
.text              32604   134217728
.rodata             6844   134250332
.data                732   536870912
.bss               11276   536871644
._usrstack           256   536882920
.comment            1204           0
.debug_aranges      4192           0
.debug_pubnames     8631           0
.debug_info        64132           0
.debug_abbrev      12612           0
.debug_line        23259           0
.debug_frame       14280           0
.debug_str         16464           0
.debug_loc         52289           0
.debug_ranges       4520           0
.ARM.attributes       49           0
Total             253344


"Programming with OPENOCD"
.\OpenOCD\bin\openocd -d0 -f interface/jtagkey2.cfg -f target/stm32.cfg -c init
-c "fast enable" -c targets -c "reset halt" -c "jtag_khz 1200" -c "poll off" -c
"flash write_image erase FLASH_RUN/project.elf" -c "verify_image FLASH_RUN/proje
ct.elf" -c "reset run" -c shutdown
Open On-Chip Debugger 0.3.0-in-development (2009-09-01-21:04) svn:2660
$URL: http://svn.berlios.de/svnroot/repos/openocd/trunk/src/openocd.c $
For bug reports, read http://svn.berlios.de/svnroot/repos/openocd/trunk/BUGS
debug_level: 0
1000 kHz
jtag_nsrst_delay: 100
jtag_ntrst_delay: 100
    TargetName         Type       Endian TapName            State
--  ------------------ ---------- ------ ------------------ ------------
 0* stm32.cpu          cortex_m3  little stm32.cpu          running
target state: halted
target halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x08004ef8 msp: 0x20005000
1200 kHz
auto erase enabled
wrote 40180 byte from file FLASH_RUN/project.elf in 3.546875s (11.062776 kb/s)
verified 40180 bytes in 1.062500s

C:\WinARM\examples\stm32_chan_fat\project>

******************************************************************************
Scratchpad - please ignore everything below
******************************************************************************

General->Workspace->Save autom. bef. build

