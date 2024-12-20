# Kovic's Instructional Vanity Operating System for N64

KIVOS64 is a simple operating system for Nintendo 64 made for educational purposes. If you're interested in how the N64 works and what the minimal code required to draw something on screen, make it move with the controller, make it beep, use the TLB, or make a userspace task is, then I hope this is a fine codebase to study and learn from.

## Building

The code requires the GCC-MIPS64 toolchain. This is not commonly distributed by package managers but a prepared package is distributed by [libdragon](https://github.com/DragonMinded/libdragon/wiki/Installing-libdragon#option-2-download-a-prebuilt-binary-toolchain-via-zip-file-or-debrpm). KIVOS64's Makefile can automatically download the .deb package for you, if you wish. If you already have libdragon installed, then you have all the prerequisites.

## Running

The code runs on real NTSC Nintendo 64 with or without the Expansion Pak. Support for PAL consoles is not implemented but could be added relatively simply.

Emulators tend to not be able to run custom low-level code but [ares](https://github.com/ares-emulator/ares) is known to work and was used during development. (Known issue: The graphics flicker on ares. This issue is not present on real hardware.)

## Credits
I would like to thank folks in the N64Brew community for answering my questions while I was making this. Namely, I would like to thank
- Rasky
- Polprzewodnikowy
- daevangelion