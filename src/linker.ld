/* MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
} */
/* _estack = ORIGIN(RAM) + LENGTH(RAM); */

SECTIONS
{
  . = 0x80000;
  .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
  .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
  PROVIDE(_data = .);
  .data : { *(.data .data.* .gnu.linkonce.d*) }
  .bss (NOLOAD): {
    . = ALIGN(16)
    __bss_start = .;
    *(.bss .bss.*)
    *(COMMON)
    __bss_end = .;
  }
  _end = .;
  /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;

