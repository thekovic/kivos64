OUTPUT_FORMAT ("elf32-bigmips", "elf32-bigmips", "elf32-littlemips")
OUTPUT_ARCH (mips)
EXTERN (_start)
ENTRY (_start)

PHDRS
{
    irq PT_LOAD AT ( 0x80000000 );
    kernel PT_LOAD AT ( 0x80000400 );
    user PT_LOAD AT ( 0x00080000 );
}

SECTIONS
{
    /DISCARD/ : { *(.MIPS.abiflags) }

    /* Interrupt vectors are at specific addresses (see page 200). */
    .intvectors 0x80000000 : {
        . = ALIGN(32);
        KEEP(*(.intvectors))
        __intvectors_end = .;
    } : irq

    /* Kernel space code and data section. */
    .kernel 0x80000400 : {
        __kernel_start = .;
        *(.boot)
        . = ALIGN(16);
        *(.irqs)
        . = ALIGN(16);
        *(EXCLUDE_FILE (*build/main.o) .text*)
        . = ALIGN(16);
        __kernel_end  = .;
    } : kernel

    .kernel_data : {
        __kernel_data_start = .;
        *(EXCLUDE_FILE (*build/main.o) .data*)
        . = ALIGN(16);
        *(EXCLUDE_FILE (*build/main.o) .rodata*)
        . = ALIGN(16);
        __kernel_data_end = .;
    }

    /* Here the ROM is finished. The rest is just in RAM */
    . = ALIGN(16);
    __rom_end = .;

    . = ALIGN(16);
    .bss : {
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(16);
         __bss_end = .;
    }

    . = ALIGN(16);

    /* User space code and data section. */
    .user 0x00080000 : {
        __user_text_start = .;
        *(build/main.o .text*)
        . = ALIGN(16);
        __user_text_end = .;
        
    } : user

    .user_data : {
        __user_data_start = .;
        *(build/main.o .data*)
        . = ALIGN(16);
        *(*build/main.o .rodata*)
        . = ALIGN(16);
        __user_data_end = .;
    }

    end = .;
}
