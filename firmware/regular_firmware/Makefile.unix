###############################################################################
#
#  	    File        : Makefile
#
#       Abstract    : Example Makefile for a C Project
#
#       Environment : Atollic TrueSTUDIO(R)
#
###############################################################################

OPTIMIZATION = 0
#CROSS_PATH   = c:/programs/codesourcery/arm-2011.09-70
#CROSS_PATH   = c:/programs/codesourcery/arm-2011.03-42
CROSS_PATH   = ~/programs/code-sourcery/2011.03-42
#CROSS_PREFIX = arm-none-linux-gnueabi-
CROSS_PREFIX = arm-none-eabi-
LINKER_LD    = cmsis/linker/stm32_flash.ld

# Define output directory
OBJECT_DIR = target
BIN_DIR = $(OBJECT_DIR)

#F_XTAL   = 8000000
#SYSCLOCK_CL = SYSCLK_FREQ_72MHz=72000000

DEFS=
DEFS+=-DHSE_VALUE=8000000UL
DEFS+=-DSYSCLK_FREQ_48MHz=48000000
DEFS+=-DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD
# Parameters for tuning SDC card access.
#BOARD    = EK_STM32F
#BOARD    = MINI_STM32
DEFS += -DUSE_EK_STM32F
#DEFS += -DSTM32_SD_USE_DMA
DEFS += -DSTARTUP_DELAY

#
CROSS = $(CROSS_PATH)/bin/$(CROSS_PREFIX)

# System configuration
CC = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
SIZE = $(CROSS)size
RM=rm -rf

# Assembler, Compiler and Linker flags and linker script settings
LINKER_FLAGS=
LINKER_FLAGS+=-lm -mthumb -mcpu=cortex-m3 
LINKER_FLAGS+=-Wl,--gc-sections -T$(LINKER_LD) 
LINKER_FLAGS+=-static  -Wl,-cref 
LINKER_FLAGS+="-Wl,-Map=$(BIN_DIR)/target.map" 
LINKER_FLAGS+=-Wl,--defsym=malloc_getpagesize_P=0x1000

ASSEMBLER_FLAGS=
ASSEMBLER_FLAGS+=-c -g -O$(OPTIMIZATION) -mcpu=cortex-m3 -mthumb 
ASSEMBLER_FLAGS+=-x assembler-with-cpp 
ASSEMBLER_FLAGS+=-Isrc 
ASSEMBLER_FLAGS+=-Istdperiph
ASSEMBLER_FLAGS+=-Icmsis/src 
ASSEMBLER_FLAGS+=-Iusb

COMPILER_FLAGS=
COMPILER_FLAGS+=-c -g -mcpu=cortex-m3 -O$(OPTIMIZATION) 
COMPILER_FLAGS+=-Wall -ffunction-sections 
COMPILER_FLAGS+=-fdata-sections -mthumb 
COMPILER_FLAGS+=$(DEFS) 
COMPILER_FLAGS+=-Isrc 
COMPILER_FLAGS+=-Istdperiph 
COMPILER_FLAGS+=-Icmsis/src 
COMPILER_FLAGS+=-I3310 
COMPILER_FLAGS+=-Iusb 
COMPILER_FLAGS+=-Ifreertos
COMPILER_FLAGS+=-Ifreertos/cm3
COMPILER_FLAGS+=-Ifatfs
COMPILER_FLAGS+=-Isrc/usb_related
COMPILER_FLAGS+=-Isrc/pawn
COMPILER_FLAGS+=-Isrc/routines

# Define sources and objects
SRC := $(wildcard */*/*/*/*/*/*/*.c) \
	$(wildcard */*/*/*/*/*/*.c) \
	$(wildcard */*/*/*/*/*.c) \
	$(wildcard */*/*/*/*.c) \
	$(wildcard */*/*/*.c) \
	$(wildcard */*/*.c) \
	$(wildcard */*.c)
SRCSASM := $(wildcard */*/*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*.s) \
	$(wildcard */*/*/*/*.s) \
	$(wildcard */*/*/*.s) \
	$(wildcard */*/*.s) \
	$(wildcard */*.s)
OBJS := $(SRC:%.c=$(OBJECT_DIR)/%.o) $(SRCSASM:%.s=$(OBJECT_DIR)/%.o)
OBJS := $(OBJS:%.S=$(OBJECT_DIR)/%.o)  

###############
# Build project
# Major targets
###############
all: buildelf

buildelf: $(OBJS) 
	@$(CC) -o "$(BIN_DIR)/target.elf" $(OBJS) $(LINKER_FLAGS)
	@$(OBJCOPY) -O ihex "$(BIN_DIR)/target.elf" "$(BIN_DIR)/target.hex"
	@$(OBJCOPY) -O binary "$(BIN_DIR)/target.elf" "$(BIN_DIR)/target.bin"
	$(SIZE) "$(BIN_DIR)/target.elf"


clean:
	$(RM) $(OBJS) "$(BIN_DIR)/target.elf" "$(BIN_DIR)/target.map"


##################
# Specific targets
##################
$(OBJECT_DIR)/src/main.o: src/main.c
	@echo make directory "./$(dir $@)"
	@mkdir -p ./$(dir $@)
	$(CC) $(COMPILER_FLAGS) src/main.c -o $(OBJECT_DIR)/src/main.o 


##################
# Implicit targets
##################
$(OBJECT_DIR)/%.o: %.c
	@echo make directory "./$(dir $@)"
	@mkdir -p ./$(dir $@)
	$(CC) $(COMPILER_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o: %.s
	@echo make directory "./$(dir $@)"
	@mkdir -p ./$(dir $@)
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@
	
$(OBJECT_DIR)/%.o: %.S
	@echo make directory "./$(dir $@)"
	@mkdir -p ./$(dir $@)
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@


