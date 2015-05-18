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
	ao_task.h \
	ao_adc_fast.h \
	stm32f0.h

#
# Common AltOS sources
#
ALTOS_SRC = \
	ao_interrupt.c \
	ao_timer.c \
	ao_panic.c \
	ao_mutex.c \
	ao_dma_stm.c \
	ao_adc_fast.c \
	ao_crc_stm.c \
	ao_stdio.c \
	ao_led.c \
	ao_romconfig.c \
	ao_boot_chain.c \
	ao_usb_stm.c \
	ao_trng_send.c \
	ao_task.c \
	ao_product.c

PRODUCT=ChaosKey-v0.1
PRODUCT_DEF=-DCHAOSKEY_V_0_1
IDVENDOR=0x1d50
IDPRODUCT=0x60c6

CFLAGS = $(PRODUCT_DEF) $(STMF0_CFLAGS) -g -Os

PROGNAME=chaoskey-v0.1
PROG=$(PROGNAME)-$(VERSION).elf
HEX=$(PROGNAME)-$(VERSION).ihx

SRC=$(ALTOS_SRC) ao_chaoskey.c
OBJ=$(SRC:.c=.o)

all: $(PROG) $(HEX)

$(PROG): Makefile $(OBJ) altos.ld
	$(call quiet,CC) $(LDFLAGS) $(CFLAGS) -o $(PROG) $(OBJ) $(LIBS)

ao_product.h: ao-make-product.5c ../Version
	$(call quiet,NICKLE,$<) $< -m altusmetrum.org -V $(IDVENDOR) -i $(IDPRODUCT) -p $(PRODUCT) -v $(VERSION) -o $@

$(OBJ): $(INC)

distclean:	clean

clean:
	rm -f *.o $(PROGNAME)-*.elf $(PROGNAME)-*.ihx
	rm -f ao_product.h

install:

uninstall: