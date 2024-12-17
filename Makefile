# KIVOS64 root Makefile.

# Define the order of subprojects.
SUBPROJECTS := tools boot kernel game

# Find installation of GCC for N64 toolchain.
N64_GCCPREFIX ?= $(N64_INST)
# Make variable containing path to project root.
KIVOS_ROOT := $(CURDIR)

# Default target: build all subprojects in the specified order.
all: $(SUBPROJECTS)

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
	$(MAKE) -C $@

game:
ifeq ($(N64_GCCPREFIX),)
	$(error "GCC toolchain for N64 is not installed. Please install it to continue.")
else
	@echo "GCC toolchain for N64 is installed. Building game..."
endif
	$(MAKE) -C $@ KIVOS_ROOT=$(KIVOS_ROOT)

# Clean all subprojects.
clean:
	@for dir in $(SUBPROJECTS); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all $(SUBPROJECTS) clean
