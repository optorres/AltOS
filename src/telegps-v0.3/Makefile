#
# AltOS build
#
#

include ../lpc/Makefile.defs

INC = \
	ao.h \
	ao_arch.h \
	ao_arch_funcs.h \
	ao_pins.h \
	ao_product.h \
	ao_tracker.h \
	ao_task.h \
	ao_whiten.h \
	ao_cc115l.h \
	ao_fec.h \
	lpc.h \
	Makefile


ALTOS_SRC = \
	ao_interrupt.c \
	ao_boot_chain.c \
	ao_product.c \
	ao_romconfig.c \
	ao_cmd.c \
	ao_config.c \
	ao_task.c \
	ao_stdio.c \
	ao_panic.c \
	ao_timer_lpc.c \
	ao_mutex.c \
	ao_freq.c \
	ao_spi_lpc.c \
	ao_usb_lpc.c \
	ao_exti_lpc.c \
	ao_serial_lpc.c \
	ao_gps_ublox.c \
	ao_gps_show.c \
	ao_cc115l.c \
	ao_fec_tx.c \
	ao_aprs.c \
	ao_tracker.c \
	ao_telemetry.c \
	ao_storage.c \
	ao_m25.c \
	ao_log.c \
	ao_log_gps.c \
	ao_distance.c \
	ao_sqrt.c \
	$(SAMPLE_PROFILE)

PRODUCT=TeleGPS-v0.3
PRODUCT_DEF=-DTELEGPS
IDPRODUCT=0x0025

CFLAGS = $(PRODUCT_DEF) $(LPC_CFLAGS) $(PROFILE_DEF)

PROGNAME=telegps-v0.3
PROG=$(PROGNAME)-$(VERSION).elf
HEX=$(PROGNAME)-$(VERSION).ihx

SRC=$(ALTOS_SRC) ao_telegps.c
OBJ=$(SRC:.c=.o)

all: $(PROG) $(HEX)

$(PROG): Makefile $(OBJ) altos.ld
	$(call quiet,CC) $(LDFLAGS) -o $(PROG) $(OBJ) $(LIBS)

$(OBJ): $(INC)

distclean:	clean

clean:
	rm -f *.o ao_serial_lpc.h $(PROGNAME)-*.elf $(PROGNAME)-*.ihx $(PROGNAME)-*.map
	rm -f ao_product.h

install:

uninstall:
