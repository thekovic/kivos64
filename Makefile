# KIVOS64 root Makefile.

# Define the order of subprojects.
SUBPROJECTS := tools boot kernel

# Find installation of GCC for N64 toolchain.
N64_GCCPREFIX ?= $(N64_INST)
# Make variable containing path to project root.
KIVOS_ROOT := $(CURDIR)

# Default target: build all subprojects in the specified order.
all: $(SUBPROJECTS)
	@echo "KIVOS64 built successfully!"

tools:
	$(MAKE) -C $@

boot:
ifeq ($(N64_GCCPREFIX),)
	$(error "GCC toolchain for N64 is not installed. Please install it to continue.")
else
	@echo "GCC toolchain for N64 is installed. Building bootcode..."
endif
	$(MAKE) -C $@

kernel:
ifeq ($(N64_GCCPREFIX),)
	$(error "GCC toolchain for N64 is not installed. Please install it to continue.")
else
	@echo "GCC toolchain for N64 is installed. Building kernel..."
endif
	$(MAKE) -C $@ KIVOS_ROOT=$(KIVOS_ROOT)

# Clean all subprojects.
clean:
	@for dir in $(SUBPROJECTS); do \
		$(MAKE) -C $$dir clean; \
	done

disasm:
	$(MAKE) -C kernel disasm KIVOS_ROOT=$(KIVOS_ROOT)

.PHONY: all $(SUBPROJECTS) clean disasm
