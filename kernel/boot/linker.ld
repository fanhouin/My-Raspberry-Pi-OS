/* MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
} */
/* _estack = ORIGIN(RAM) + LENGTH(RAM); */

SECTIONS
{
  . = 0x80000;
  _start = .;
  .text : { KEEP(*(.text.boot)) *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : {
    . = ALIGN(16);
    __bss_start = .;
    *(.bss)
    __bss_end = .;
  }
  _end = .;
}
__bss_size = (__bss_end - __bss_start) >> 3;

