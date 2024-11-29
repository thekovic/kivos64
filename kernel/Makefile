# Suppress echo of commands called by make unless V=1
ifneq ($(V),1)
.SILENT:
endif

BUILD_DIR := build
SOURCE_DIR := src
INCLUDE_DIR := include

KERNEL_ROM := kivos64.n64
KERNEL_LDSCRIPT := n64.ld

SRCS := $(shell find $(SOURCE_DIR)/ -type f -name '*.c' | sort)
OBJS := $(SRCS:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS += $(BUILD_DIR)/entrypoint.o

# Find installation of GCC for N64 toolchain from libdragon
N64_GCCPREFIX ?= $(N64_INST)
N64_GCCPREFIX_TRIPLET := $(N64_GCCPREFIX)/bin/mips64-elf-
# Define tools
N64_CC := $(N64_GCCPREFIX_TRIPLET)gcc
N64_AS := $(N64_GCCPREFIX_TRIPLET)as
N64_OBJDUMP := $(N64_GCCPREFIX_TRIPLET)objdump
N64_SIZE := $(N64_GCCPREFIX_TRIPLET)size

N64_TOOL := $(N64_INST)/bin/n64tool
N64_TOOLFLAGS := --title "kivos64" --header "../boot/kivos64_ipl3.n64"

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
# Add include folder
N64_CFLAGS += -I$(INCLUDE_DIR)

N64_ASFLAGS := -march=vr4300 -mtune=vr4300 -Wa,--fatal-warnings -MMD

N64_LDFLAGS = -g -Wl,-T$(KERNEL_LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/kivos64.map -Wl,--gc-sections

all: $(KERNEL_ROM)

# C rule.
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c 
	@mkdir -p $(dir $@)
	@echo "    [CC] $<"
	$(N64_CC) -c $(N64_CFLAGS) -o $@ $<

# Assembly rule.
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "    [AS] $<"
	$(N64_CC) -c $(N64_ASFLAGS) -o $@ $<

$(BUILD_DIR)/kivos64.elf: $(KERNEL_LDSCRIPT) $(OBJS)
	@echo "    [LD] $@"
	$(N64_CC) $(N64_CFLAGS) $(N64_LDFLAGS) -o $@ $(filter %.o,$^)
	$(N64_SIZE) -G $@

%.n64: $(BUILD_DIR)/%.elf
	@echo "    [Z64] $@"
	$(N64_TOOL) $(N64_TOOLFLAGS) --toc --output $@ --align 256 $< --align 8

disasm: $(BUILD_DIR)/kivos64.elf
	$(N64_OBJDUMP) -D $(BUILD_DIR)/kivos64.elf

clean:
	rm -rf $(BUILD_DIR) $(KERNEL_ROM)
