#
# AltOS build for TeleLCO
#
#

include ../stm/Makefile.defs

INC = \
	ao.h \
	ao_arch.h \
	ao_arch_funcs.h \
	ao_boot.h \
	ao_companion.h \
	ao_data.h \
	ao_sample.h \
	ao_pins.h \
	ao_product.h \
	ao_seven_segment.h \
	ao_lco.h \
	ao_lco_cmd.h \
	ao_lco_func.h \
	ao_radio_spi.h \
	ao_radio_cmac.h \
	ao_cc1120_CC1120.h \
	stm32l.h

#
# Common AltOS sources
#

#PROFILE=ao_profile.c
#PROFILE_DEF=-DAO_PROFILE=1

ALTOS_SRC = \
	ao_boot_chain.c \
	ao_interrupt.c \
	ao_product.c \
	ao_romconfig.c \
	ao_cmd.c \
	ao_config.c \
	ao_task.c \
	ao_led_stm.c \
	ao_stdio.c \
	ao_panic.c \
	ao_timer.c \
	ao_mutex.c \
	ao_freq.c \
	ao_dma_stm.c \
	ao_spi_stm.c \
	ao_beep_stm.c \
	ao_eeprom_stm.c \
	ao_fast_timer.c \
	ao_lcd_stm.c \
	ao_usb_stm.c \
	ao_exti_stm.c \
	ao_cc1120.c \
	ao_radio_cmac.c \
	ao_aes.c \
	ao_aes_tables.c \
	ao_fec_tx.c \
	ao_fec_rx.c \
	ao_seven_segment.c \
	ao_quadrature.c \
	ao_button.c \
	ao_event.c \
	ao_lco.c \
	ao_lco_bits.c \
	ao_lco_cmd.c \
	ao_lco_func.c \
	ao_radio_cmac_cmd.c

PRODUCT=TeleLCO-v0.2
PRODUCT_DEF=-DTELELCO
IDPRODUCT=0x0023

CFLAGS = $(PRODUCT_DEF) $(STM_CFLAGS) $(PROFILE_DEF)

PROGNAME=telelco-v0.2
PROG=$(PROGNAME)-$(VERSION).elf
HEX=$(PROGNAME)-$(VERSION).ihx

SRC=$(ALTOS_SRC) ao_telelco.c
OBJ=$(SRC:.c=.o)

all: $(PROG) $(HEX)

$(PROG): Makefile $(OBJ) altos.ld
	$(call quiet,CC) $(LDFLAGS) -o $(PROG) $(OBJ) $(LIBS)

$(OBJ): $(INC)

distclean:	clean

clean:
	rm -f *.o $(PROGNAME)-*.elf $(PROGNAME)-*.ihx $(PROGNAME)-*.map
	rm -f ao_product.h

install:

uninstall:
