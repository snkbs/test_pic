@echo off

d:\soft\ihex\bin2ihex.exe -a 0x0800A000 -i pic.bin -o pic_offset.hex
"d:\soft\STMicroelectronics\STM32 ST-LINK Utility 4.5\ST-LINK Utility\ST-LINK_CLI.exe" -P pic_offset.hex -Rst

