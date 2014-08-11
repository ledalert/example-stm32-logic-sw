#Project settings
PROJECT_NAME = template_f1
SOURCES = main.c
BUILD_DIR = build/

#Debugging makefile
DEBUG_PRINT ?= 0
DEBUG_INDENT ?=

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)%.o)
TARGET_ELF = $(BUILD_DIR)$(PROJECT_NAME).elf
TARGET_BIN = $(TARGET_ELF:%.elf=%.bin)

#Toolchain settings
TOOLCHAIN = arm-none-eabi

CC = $(TOOLCHAIN)-gcc
OBJCOPY = $(TOOLCHAIN)-objcopy
OBJDUMP = $(TOOLCHAIN)-objdump
SIZE = $(TOOLCHAIN)-size

#Target CPU options
CPU_DEFINES = -mthumb -mcpu=cortex-m3 -msoft-float -DSTM32F1

#Compiler options
CFLAGS += -c -std=gnu99 -g -Os -Wall -fno-common -ffunction-sections
CFLAGS += -fdata-sections -Wl,--gc-sections
CFLAGS += -nostartfiles
CFLAGS += $(CPU_DEFINES)

LINK_SCRIPT = stm32f100x6.ld

LINK_FLAGS = --static -nostartfiles -Llib/libopencm3/lib 
LINK_FLAGS += -Llib/libopencm3/lib/stm32/f1 -Xlinker --gc-sections 
LINK_FLAGS += -T$(LINK_SCRIPT) -lnosys -lopencm3_stm32f1

LIBS = libopencm3_stm32f1.a

#Directories
vpath %.c src
vpath %.o $(BUILD_DIR)
vpath %.ld lib/libopencm3/lib/stm32/f1
vpath %.a lib/libopencm3/lib

default: $(TARGET_BIN)

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET_BIN)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(TARGET_ELF): $(BUILD_DIR) $(LIBS) $(OBJECTS) $(LINK_SCRIPT)
	$(CC) $(LINK_FLAGS) $(OBJECTS) -o $(TARGET_ELF)

$(OBJECTS): $(BUILD_DIR)%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

$(LINK_SCRIPT): libopencm3_stm32f1.a

libopencm3_stm32f1.a: lib/libopencm3/.git
	cd lib/libopencm3; $(MAKE) lib/stm32/f1

lib/libopencm3/.git:
	cd lib/libopencm3; git submodule init
	cd lib/libopencm3; git submodule update

clean:
	rm -f $(OBJECTS) $(TARGET_ELF) $(TARGET_BIN)

deep-clean: clean
	cd lib/libopencm3; $(MAKE) clean

	

.PHONY: default clean deep-clean libopencm3

#makefile_debug:
