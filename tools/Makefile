# Suppress echo of commands called by make unless V=1
ifneq ($(V),1)
.SILENT:
endif

BUILD_DIR := build
SOURCE_DIR := .

N64_INST ?= uninstalled

TOOLCHAIN_FILE := gcc-toolchain-mips64-x86_64.deb
TOOLCHAIN_URL := https://github.com/DragonMinded/libdragon/releases/download/toolchain-continuous-prerelease/gcc-toolchain-mips64-x86_64.deb

all: toolchain n64tool

toolchain:
ifeq ($(N64_INST), uninstalled)
	@echo "N64_INST is not defined. GCC toolchain for N64 is not installed!"
	@echo "Please, install it before continuing with compiling KIVOS64."
	if [ ! -e "$(TOOLCHAIN_FILE)" ]; then \
		echo ".deb package for the toolchain will be downloaded." ; \
		if command -v wget ; then \
			wget -O "$(TOOLCHAIN_FILE)" "$(TOOLCHAIN_URL)"  ; \
		elif command -v curl ; then \
			curl -Lo "$(TOOLCHAIN_FILE)" "$(TOOLCHAIN_URL)" ; \
		else \
			echo "ERROR: Cannot download toolchain because wget or curl are not installed! Please download manually." ; \
			touch "$(TOOLCHAIN_FILE)" ; \
		fi ; \
	fi
else
	@echo "GCC toolchain for N64 is already installed in '$(N64_INST)'"
endif

n64tool: n64tool.c
	@echo "    [CC] $<"
	gcc -o $@ $<

clean:
	rm -rf $(TOOLCHAIN_FILE) n64tool

.PHONY: all disasm clean
