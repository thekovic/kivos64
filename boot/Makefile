# Suppress echo of commands called by make unless V=1
ifneq ($(V),1)
.SILENT:
endif

BUILD_DIR := build
SOURCE_DIR := .

SED ?= sed

# Find installation of GCC for N64 toolchain from libdragon
N64_GCCPREFIX ?= $(N64_INST)
N64_GCCPREFIX_TRIPLET := $(N64_GCCPREFIX)/bin/mips64-elf-
# Define tools
N64_CC := $(N64_GCCPREFIX_TRIPLET)gcc
N64_AS := $(N64_GCCPREFIX_TRIPLET)as
N64_OBJCOPY := $(N64_GCCPREFIX_TRIPLET)objcopy
N64_OBJDUMP := $(N64_GCCPREFIX_TRIPLET)objdump
N64_READELF := $(N64_GCCPREFIX_TRIPLET)readelf

# Set architecture to N64's VR4300 CPU
N64_CFLAGS := -march=vr4300 -mtune=vr4300
# Optimize for reduced size because we need to fit part of boot code into DMEM
N64_CFLAGS += -Os
# Turn off standard C runtime and standard library
N64_CFLAGS += -ffreestanding -nostdlib
# gp is not initialized (don't use it)
N64_CFLAGS += -G0 -ffunction-sections
# Can't compile for 64-bit ABI because DMEM/IMEM don't support 64-bit access
N64_CFLAGS += -mabi=32 -mgp32 -mfp32 -msingle-float
# Reasonable general GCC settings
N64_CFLAGS += -Wall -Werror -fdiagnostics-color=always -MMD

N64_ASFLAGS := -mtune=vr4300 -march=vr4300 -mabi=32 -mgp32 -mfp32 -msingle-float -G0

N64_LDFLAGS = -Wl,-T$(IPL3_LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/ipl3.map -Wl,--gc-sections

OBJS := $(BUILD_DIR)/ipl3.o $(BUILD_DIR)/loader.o $(BUILD_DIR)/boot_trampoline.o 

IPL3_ROM := kivos64_ipl3.n64
IPL3_LDSCRIPT := ipl3.ld

all: $(IPL3_ROM)

$(BUILD_DIR)/%.o: %.c
	@echo "    [CC] $@"
	@mkdir -p $(BUILD_DIR)
# Compile relocatable code. We need this for stage 2 which is relocated to RDRAM,
# but in general it won't hurt for boot code. GCC MIPS doesn't have a way to do
# this simple transformation so we do it ourselves by replacing jal/j with bal/b
# on the assembly output of the compiler.
	$(N64_CC) -S -c $(N64_CFLAGS) -o $@.s $<
	$(SED) -i 's/\bjal\b/bal/g' $@.s
	$(SED) -i 's/\bj\b/b/g' $@.s
	$(N64_AS) $(N64_ASFLAGS) -o $@ $@.s

$(BUILD_DIR)/%.o: %.S
	@echo "    [AS] $@"
	@mkdir -p $(BUILD_DIR)
	$(N64_CC) -c $(N64_ASFLAGS) -o $@ $<

%.n64: $(BUILD_DIR)/ipl3.elf
	@echo "    [N64] $@"
	$(N64_OBJCOPY) -O binary $< $@

$(BUILD_DIR)/ipl3.elf: $(IPL3_LDSCRIPT) $(OBJS)
	@echo "    [LD] $@"
	$(N64_CC) $(N64_CFLAGS) $(N64_LDFLAGS) -o $@ $(filter %.o,$^)
	$(N64_READELF) --wide --sections $@ | grep .text

disasm: $(BUILD_DIR)/ipl3.elf
	$(N64_OBJDUMP) -D $(BUILD_DIR)/ipl3.elf

clean:
	rm -rf $(BUILD_DIR) kivos64_ipl3.n64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all disasm
