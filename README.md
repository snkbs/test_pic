Testing position independent code (PIC) on Cortex-M3 (stm32f103x8).

FW can start standalone from 0x08000000 or any 4bytes aligned offset from bootloader.
Bootloader should write firmware offset to R9 and ~offset to R8 before jump.
