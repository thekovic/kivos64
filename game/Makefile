# Suppress echo of commands called by make unless V=1
ifneq ($(V),1)
.SILENT:
endif

BUILD_DIR := build
SOURCE_DIR := .
INCLUDE_DIR := $(KIVOS_ROOT)/kernel/include
KERNEL_DIR := $(KIVOS_ROOT)/kernel
KERNEL_LIB := $(KIVOS_ROOT)/kernel/libkivos64.a

GAME_ROM := kivos64.n64
GAME_LDSCRIPT := n64.ld

OBJS := $(BUILD_DIR)/main.o

# Find installation of GCC for N64 toolchain.
N64_GCCPREFIX ?= $(N64_INST)
N64_GCCPREFIX_TRIPLET := $(N64_GCCPREFIX)/bin/mips64-elf-
# Define tools
N64_CC := $(N64_GCCPREFIX_TRIPLET)gcc
N64_AS := $(N64_GCCPREFIX_TRIPLET)as
N64_OBJDUMP := $(N64_GCCPREFIX_TRIPLET)objdump
N64_SIZE := $(N64_GCCPREFIX_TRIPLET)size

N64_TOOL := $(KIVOS_ROOT)/tools/n64tool
N64_TOOLFLAGS := --title "kivos64" --header "$(KIVOS_ROOT)/boot/kivos64_ipl3.n64"

# Set architecture to N64's VR4300 CPU
N64_CFLAGS := -march=vr4300 -mtune=vr4300
# Turn off standard C runtime and standard library
N64_CFLAGS += -ffreestanding -nostdlib
# Turn on sections so that linker can better optimize unused stuff
N64_CFLAGS += -ffunction-sections -fdata-sections -g
# Optimize floating point math
N64_CFLAGS += -ffast-math -ftrapping-math -fno-associative-math
# Reasonable general GCC settings
N64_CFLAGS += -Wall -Werror -fdiagnostics-color=always -MMD -std=gnu17
N64_CFLAGS += -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=unused-function -Wno-error=unused-parameter -Wno-error=unused-but-set-parameter -Wno-error=unused-label -Wno-error=unused-local-typedefs -Wno-error=unused-const-variable
# Add include folder
N64_CFLAGS += -I$(INCLUDE_DIR)

N64_ASFLAGS := -march=vr4300 -mtune=vr4300 -Wa,--fatal-warnings -MMD

N64_LDFLAGS = -g -L$(KERNEL_DIR) -lkivos64 -Wl,-T$(GAME_LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/kivos64.map -Wl,--gc-sections

all: $(GAME_ROM)

# C rule.
$(BUILD_DIR)/%.o: %.c 
	@mkdir -p $(dir $@)
	@echo "    [CC] $<"
	$(N64_CC) -c $(N64_CFLAGS) -o $@ $<

$(BUILD_DIR)/kivos64.elf: $(GAME_LDSCRIPT) $(OBJS)
	@echo "    [LD] $@"
	$(N64_CC) $(N64_CFLAGS) $(N64_LDFLAGS) -o $@ $(filter %.o,$^)
	$(N64_SIZE) -G $@

$(GAME_ROM): $(KERNEL_LIB) $(BUILD_DIR)/kivos64.elf
	@echo "    [N64] $@"
	$(N64_TOOL) $(N64_TOOLFLAGS) --toc --output $@ --align 256 $(BUILD_DIR)/kivos64.elf --align 8

disasm: $(BUILD_DIR)/kivos64.elf
	$(N64_OBJDUMP) -D $(BUILD_DIR)/kivos64.elf

clean:
	rm -rf $(BUILD_DIR) $(GAME_ROM)

.PHONY: all disasm clean
