SECTIONS
{
  . = 0x60000;
  PROVIDE(_bootstart = .);
  . = 0x80000;
  PROVIDE(_start = .);
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
__kernel_image_size = _end - _start;

