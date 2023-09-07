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
    CFLAGS += -O0 -ggdb -g3 -gdwarf-2
else
    CFLAGS += -O2
endif

ARCH_FLAGS := -mcpu=cortex-m3 -mthumb -mthumb-interwork -msoft-float
ALL_CFLAGS := $(ARCH_FLAGS) $(CFLAGS) $(ADD_FLAGS) -MD $(DEFS) \
              -I$(OPENCM3_DIR)/include -I$(INCLUDE_DIR) \
              -fstack-usage -fno-common -ffunction-sections -fdata-sections \
              -Wall -Wextra

ALL_LDFLAGS := $(ARCH_FLAGS) -T$(LDSCRIPT) -nostartfiles -nostdlib \
               --specs=nano.specs --specs=nosys.specs \
               -l$(OPENCM3_LIB) -L$(OPENCM3_DIR)/lib \
               -Wl,--gc-sections,--print-memory-usage \

C_SOURCES := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
A_SOURCES := $(wildcard $(addsuffix /*.s, $(SRC_DIRS)))
OBJECTS := $(C_SOURCES:.c=.o)
OBJECTS += $(A_SOURCES:.s=.o)

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
	$(Q)find "$(BUILD_DIR)" -delete
	@printf "  [CLEAN]   $(BINARY).elf $(BINARY).bin $(BINARY).hex $(BINARY).srec $(BINARY).list $(BINARY).map\n"
	$(Q)rm -f $(BINARY).elf $(BINARY).bin $(BINARY).hex $(BINARY).srec $(BINARY).list $(BINARY).map

-include $(addprefix $(BUILD_DIR)/, $(OBJECTS:.o=.d))
