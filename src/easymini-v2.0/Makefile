#
# AltOS build
#
#

include ../stmf0/Makefile.defs

INC = \
	ao.h \
	ao_arch.h \
	ao_arch_funcs.h \
	ao_pins.h \
	ao_product.h \
	stm32f0.h

#
# Common AltOS sources
#

ALTOS_SRC = \
	ao_interrupt.c \
	ao_boot_chain.c \
	ao_romconfig.c \
	ao_product.c \
	ao_mutex.c \
	ao_panic.c \
	ao_stdio.c \
	ao_storage.c \
	ao_report.c \
	ao_ignite.c \
	ao_flight.c \
	ao_kalman.c \
	ao_sample.c \
	ao_data.c \
	ao_convert_pa.c \
	ao_convert_volt.c \
	ao_task.c \
	ao_log.c \
	ao_log_mini.c \
	ao_cmd.c \
	ao_config.c \
	ao_dma_stm.c \
	ao_timer.c \
	ao_exti_stm.c \
	ao_spi_stm.c \
	ao_adc_stm.c \
	ao_usb_stm.c \
	ao_m25.c \
	ao_ms5607.c \
	ao_beep_stm.c

PRODUCT=EasyMini-v2.0
PRODUCT_DEF=-DEASYMINI_V_2_0
IDPRODUCT=0x0026

CFLAGS = $(PRODUCT_DEF) $(STMF0_CFLAGS)

PROGNAME=easymini-v2.0
PROG=$(PROGNAME)-$(VERSION).elf
HEX=$(PROGNAME)-$(VERSION).ihx
FLASH_PROG=flash-loader/$(PROGNAME)-altos-flash-$(VERSION).elf
BOTH_DFU=$(PROGNAME)-combined-$(VERSION).dfu

MAKEBIN=$(TOPDIR)/../ao-tools/ao-makebin/ao-makebin

SRC=$(ALTOS_SRC) ao_easymini.c
OBJ=$(SRC:.c=.o)

all: $(PROG) $(HEX) $(BOTH_DFU)

$(PROG): Makefile $(OBJ)
	$(call quiet,CC) $(LDFLAGS) -o $(PROG) $(OBJ) $(LIBS)

$(BOTH_DFU): $(PROG) $(FLASH_PROG)
	$(MAKEBIN) --dfu --output=$@ --base=$(FLASH_ADDR) $(FLASH_PROG) $(PROG)

$(FLASH_PROG): FRC
	+cd flash-loader && make

$(OBJ): $(INC)

distclean:	clean

clean:
	rm -f *.o $(PROGNAME)-*.elf $(PROGNAME)-*.ihx $(PROGNAME)-*.map
	rm -f ao_product.h

install:

uninstall:

FRC:
