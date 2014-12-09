#
# gcc Makefile for LPC810
# Kamal Mostafa <kamal@whence.com>
#
# License: Public Domain
#

CROSS = arm-none-eabi-
CPU = -mthumb -mcpu=cortex-m0plus -MD
FPU = # -mfloat-abi=soft

FILENAME = firmware
LINKERSCRIPT = src/firmware.ld

CFLAGS+= -Os --specs=nano.specs -ffunction-sections -fdata-sections -fno-builtin -flto -Wall -Wextra

CC = ${CROSS}gcc
LD = ${CROSS}ld
OBJCOPY = ${CROSS}objcopy
SIZE = ${CROSS}size

# use these on the $(LD) line, as needed:
LIBM = ${shell ${CC} ${CFLAGS} --print-file-name=libm.a}
LIBC = ${shell ${CC} ${CFLAGS} --print-file-name=libc.a}
LIBGCC = ${shell ${CC} ${CFLAGS} --print-file-name=libgcc.a}

CFLAGS += $(CPU) $(FPU)
LDFLAGS += --gc-sections

INC += -Iinc -Ilib/lpcopen/inc -Ilib/1-wire/inc -Ilib/ds18x20/inc -Ilib/pid/inc -Ilib/printf/inc
CFLAGS += $(INC) -DCORE_M0PLUS -D__USE_CMSIS

OBJS = src/main.o \
       src/startup_lpc8xx.o \
       src/system_lpc8xx.o \
       src/retarget.o \
       lib/lpcopen/src/acmp_8xx.o \
       lib/lpcopen/src/chip_8xx.o \
       lib/lpcopen/src/clock_8xx.o \
       lib/lpcopen/src/crc_8xx.o \
       lib/lpcopen/src/gpio_8xx.o \
       lib/lpcopen/src/i2c_8xx.o \
       lib/lpcopen/src/iocon_8xx.o \
       lib/lpcopen/src/pinint_8xx.o \
       lib/lpcopen/src/pmu_8xx.o \
       lib/lpcopen/src/ring_buffer.o \
       lib/lpcopen/src/sct_8xx.o \
       lib/lpcopen/src/spi_8xx.o \
       lib/lpcopen/src/swm_8xx.o \
       lib/lpcopen/src/syscon_8xx.o \
       lib/lpcopen/src/sysinit_8xx.o \
       lib/lpcopen/src/uart_8xx.o \
       lib/lpcopen/src/wkt_8xx.o \
       lib/lpcopen/src/wwdt_8xx.o \
       lib/1-wire/src/1-Wire.o \
       lib/1-wire/src/1-WireCRC.o \
       lib/ds18x20/src/DS18x20.o \
       lib/pid/src/pid.o \
       lib/printf/src/printf.o

all: $(FILENAME).bin

firmware.axf: $(LINKERSCRIPT) $(OBJS)
	@$(LD) -o $@ $(LDFLAGS) -T $(LINKERSCRIPT) $(OBJS) $(LIBGCC) $(LIBC) $(LIBM)
	-@echo ""
	@$(SIZE) $(FILENAME).axf

clean:
	@rm -f src/*.o src/*.d
	@rm -f lib/lpcopen/src/*.o lib/lpcopen/src/*.d
	@rm -f lib/1-wire/src/*.o lib/1-wire/src/*.d
	@rm -f lib/ds18x20/src/*.o lib/ds18x20/src/*.d
	@rm -f lib/pid/src/*.o lib/pid/src/*.d
	@rm -f lib/printf/src/*.o lib/printf/src/*.d
	@rm -f $(FILENAME).axf $(FILENAME).hex $(FILENAME).bin

%.bin: %.axf
	-@echo ""
	-@echo "Generating $(FILENAME).hex (FlashMagic, etc.)"
	@$(OBJCOPY) --strip-unneeded -O ihex $(FILENAME).axf $(FILENAME).hex
	-@echo "Generating $(FILENAME).bin"
	@$(OBJCOPY) --strip-unneeded -O binary $(FILENAME).axf $(FILENAME).bin
