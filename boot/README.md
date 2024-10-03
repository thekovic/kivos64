## KIVOS64 IPL3

This directory contains the source code of KIVOS64's IPL3, a bootcode required
to be shipped in each ROM to perform the final phases of hardware initializations
and to load the main binary and run it.

### IPL3 features

  * Searches and loads an ELF file appended to it. All loadable
    ELF segments are loaded in RDRAM and then the entrypoint is called.
  * IPL3 reads all program headers of type PT_LOAD, and ignores all sections.
    ELF must be 32-bit or 64-bit.
  * The ELF file must be concatenated to the IPL3 .n64 file respecting a 256-byte
    alignment. IPL3 will linearly scan the ROM searching for the ELF file,
    so the closest the file is to the top of the ROM, the faster it will boot.
    * The ELF file can have multiple PT_LOAD segments, all of them will be loaded.
    * The virtual address of loadable segments must be 8-byte aligned.
    * The file offset of loadable segments must be 2-byte aligned.
    * No segment should try to load itself into the last 64 KiB of RDRAM,
      as those are reserved to IPL3 itself.
  * The RDRAM is cleared before booting. This removes a common source of difference
    between emulators and real hardware when software accesses uninitialized memory.
  * IPL3 passes standard boot flags to the application in DMEM.
    This is the layout of boot flags, stored at the beginning of DMEM:
    - Byte 0..3: Amount of memory, in bytes.
    - Byte 4..7: 32-bit random generation seed.
    - Byte 8: ROM type (0: Cartridge at 0x10000000)
    - Byte 9: TV type (0:PAL, 1:NTSC, 2:MPAL)
    - Byte 10: Reset type (0:cold, 1:warm)
    - Byte 11: Console type (0:n64)
    - Byte 12..15: Address of the ELF header in ROM.
  * Register state at boot is undefined, except for the following:
    - $sp points to the end of the available RDRAM, so it can be used as a stack there.
  * IPL3 is compatible with existing open-source toolchain for N64 homebrew development.
    It can be used as a replacement for their IPL3 which verifies the validity of this
    implementation.

### Building IPL3

The IPL3 is built as part of the full KIVOS64 `Makefile`.
To build the IPL3 alone, simply run in the `boot` folder:

```
    $ make
```

Make sure you have libdragon's N64 toolchain installed first as their distribution
of MIPS GCC is required.
