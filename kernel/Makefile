# Suppress echo of commands called by make unless V=1
ifneq ($(V),1)
.SILENT:
endif

BUILD_DIR := build
SOURCE_DIR := src
INCLUDE_DIR := include

KERNEL_LIB := libkivos64.a

SRCS := $(shell find $(SOURCE_DIR)/ -type f -name '*.c' | sort)
OBJS := $(SRCS:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS += $(BUILD_DIR)/entrypoint.o

# Find installation of GCC for N64 toolchain.
N64_GCCPREFIX ?= $(N64_INST)
N64_GCCPREFIX_TRIPLET := $(N64_GCCPREFIX)/bin/mips64-elf-
# Define tools
N64_CC := $(N64_GCCPREFIX_TRIPLET)gcc
N64_AS := $(N64_GCCPREFIX_TRIPLET)as
N64_AR = $(N64_GCCPREFIX_TRIPLET)ar

# Set architecture to N64's VR4300 CPU
N64_CFLAGS := -march=vr4300 -mtune=vr4300 -Os
# Turn off standard C runtime and standard library
N64_CFLAGS += -ffreestanding -nostdlib
# Turn on sections so that linker can better optimize unused stuff
N64_CFLAGS += -ffunction-sections -fdata-sections -g -G0
# Optimize floating point math
N64_CFLAGS += -ffast-math -ftrapping-math -fno-associative-math
# Reasonable general GCC settings
N64_CFLAGS += -Wall -Werror -fdiagnostics-color=always -MMD -std=gnu17
N64_CFLAGS += -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=unused-function -Wno-error=unused-parameter -Wno-error=unused-but-set-parameter -Wno-error=unused-label -Wno-error=unused-local-typedefs -Wno-error=unused-const-variable
# Add include folder
N64_CFLAGS += -I$(INCLUDE_DIR)

N64_ASFLAGS := -march=vr4300 -mtune=vr4300 -Wa,--fatal-warnings -MMD

all: $(KERNEL_LIB)

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

$(KERNEL_LIB): $(OBJS)
	@echo "    [AR] $@"
	$(N64_AR) -rcs -o $@ $^

clean:
	rm -rf $(BUILD_DIR) $(KERNEL_LIB)

.PHONY: all clean
