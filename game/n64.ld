OUTPUT_FORMAT ("elf32-bigmips", "elf32-bigmips", "elf32-littlemips")
OUTPUT_ARCH (mips)
EXTERN (_start)
ENTRY (_start)

PHDRS
{
    irq PT_LOAD AT ( 0x80000000 );
    kernel PT_LOAD AT ( 0x80000400 );
}

SECTIONS
{
    /* Interrupt vectors are at specific addresses (see page 200). */
    .intvectors 0x80000000 : {
        . = ALIGN(32);
        KEEP(*(.intvectors))
        __intvectors_end = .;
    } :irq

    /* The text section carries the app code and its relocation address is
    * the first byte of the cart domain in cached, unmapped memory
    */
    .text 0x80000400 : {
        __text_start = .;
        *(.boot)
        . = ALIGN(16);
        *(.text)
        *(.text.*)
        . = ALIGN(16);
        __text_end  = .;
    } :kernel

    .rodata : {
        *(.rdata)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(8);
    }

    . = ALIGN(8);

    .data : {
        __data_start = .;
        *(.data)
        *(.data.*)
        . = ALIGN(8);
    }
    
    /* Small data START */
    .sdata : {
        _gp = . + 0x8000;
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(8);
    }

    . = ALIGN(8);
    __data_end = .;

    /* Here the ROM is finished. The rest is just in RAM */
    . = ALIGN(8);
    __rom_end = .;

    .sbss : {
         __bss_start = .;
        *(.sbss)
        *(.sbss.*)
        *(.scommon)
        *(.scommon.*)
    }
    /* Small data END */

    . = ALIGN(8);
    .bss : {
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
         __bss_end = .;
    }

    . = ALIGN(8);

    end = .;
}
