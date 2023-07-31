CC := arm-none-eabi-gcc
LD := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
SIZE := arm-none-eabi-size
GDB := $(shell which arm-none-eabi-gdb || which gdb-multiarch)

BUILD_DIR := .build

ifneq ($(VERB),1)
    Q := @
endif

ifeq ($(DEBUG), 1)
    CFLAGS += -ggdb3 -O0 -g3
else
    CFLAGS += -O3
endif

ARCH_FLAGS := -mthumb -mthumb-interwork -mcpu=cortex-m3 -msoft-float
ALL_CFLAGS := -std=gnu99 \
              -Wall -Wextra -Wimplicit-function-declaration \
              -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes \
              -Wundef -Wshadow -fstack-usage \
              -fno-common $(ARCH_FLAGS) $(ADD_FLAGS) -MD $(DEFS) \
              -I$(OPENCM3_DIR)/include -I$(INCLUDE_DIR) $(CFLAGS)

ALL_LDFLAGS := -l$(OPENCM3_LIB) --static -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
               -T$(LDSCRIPT) -nostartfiles -Wl,--gc-sections -Wl,--print-memory-usage \
               $(ARCH_FLAGS) -mfix-cortex-m3-ldrd \
               -L$(OPENCM3_DIR)/lib

SOURCES := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
SRC_ASM := $(wildcard $(addsuffix /*.s, $(SRC_DIRS)))
OBJECTS := $(SOURCES:.c=.o)
OBJECTS += $(SRC_ASM:.s=.o)

all: elf hex
rebuild: clean elf hex
elf: $(BINARY).elf
bin: $(BINARY).bin
hex: $(BINARY).hex
srec: $(BINARY).srec
list: $(BINARY).list
size: $(BINARY).size

%.bin: %.elf
	@printf "  [OBJCOPY] $@\n"
	$(Q)$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	@printf "  [OBJCOPY] $@\n"
	$(Q)$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.srec: %.elf
	@printf "  [OBJCOPY] $@\n"
	$(Q)$(OBJCOPY) -Osrec $(*).elf $(*).srec

%.list: %.elf
	@printf "  [OBJDUMP] $@\n"
	$(Q)$(OBJDUMP) -S $(*).elf > $(*).list

%.size: %.elf
	@printf "\n"
	$(Q)$(SIZE) -A -x $(*).elf
	@printf "\n"

%.elf %.map: prj_dirs $(OBJECTS) $(LDSCRIPT)
	@printf "  [LD]      $@\n"
	$(Q)$(LD) $(addprefix $(BUILD_DIR)/, $(OBJECTS)) $(ALL_LDFLAGS) -Wl,-Map=$(*).map -o $(*).elf

prj_dirs:
	$(Q)mkdir -p $(addprefix $(BUILD_DIR)/, $(SRC_DIRS))

%.o: %.c
	@printf "  [CC]      $@\n"
	$(Q)$(CC) $(ALL_CFLAGS) -o $(BUILD_DIR)/$@ -c $<

%.o: %.s
	@printf "  [CC]      $@\n"
	$(Q)$(CC) $(ALL_CFLAGS) -o $(BUILD_DIR)/$@ -c $<

check:
	cppcheck -q --enable=warning,style,performance,portability --template=gcc $(SRC_DIRS)

flash:
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
		-f /usr/share/openocd/scripts/target/stm32f1x.cfg \
		-c "init" -c "reset init" \
		-c "flash write_image erase $(BINARY).hex" \
		-c "reset" \
		-c "shutdown"

debug:
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
		-f /usr/share/openocd/scripts/target/stm32f1x.cfg &
	$(GDB) -ex "file $(BINARY).elf" -ex "target extended-remote :3333" -ex "break main" \
		-ex "source svd-dump.py" -ex "svd_load STMicro STM32F103xx.svd"
	pkill -3 openocd

.PHONY: clean elf bin srec hex list flash debug

clean:
	@printf "  [CLEAN]   $(BUILD_DIR)\n"
	$(Q)find $(BUILD_DIR) -delete
	@printf "  [CLEAN]   $(BINARY).elf $(BINARY).bin $(BINARY).hex $(BINARY).srec $(BINARY).list $(BINARY).map\n"
	$(Q)rm -f $(BINARY).elf $(BINARY).bin $(BINARY).hex $(BINARY).srec $(BINARY).list $(BINARY).map

-include $(addprefix $(BUILD_DIR)/, $(OBJECTS:.o=.d))
