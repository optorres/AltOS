#
# AltOS build
#
#

include ../stm/Makefile.defs

INC = \
	ao.h \
	ao_arch.h \
	ao_arch_funcs.h \
	ao_boot.h \
	ao_pins.h \
	ao_product.h

#
# Common AltOS sources
#
ALTOS_SRC = \
	ao_interrupt.c \
	ao_boot_chain.c \
	ao_boot_pin.c \
	ao_product.c \
	ao_notask.c \
	ao_timer.c \
	ao_usb_stm.c \
	ao_flash_stm.c

PRODUCT=AltosFlash-$(VERSION)
PRODUCT_DEF=-DALTOS_FLASH
IDPRODUCT=0x000a

CFLAGS = $(PRODUCT_DEF) $(STM_CFLAGS)

PROG=altos-flash-$(VERSION).elf

SRC=$(ALTOS_SRC) ao_stm_flash.c
OBJ=$(SRC:.c=.o)

all: $(PROG)

LDFLAGS=-L../stm -Wl,-Taltos-loader.ld

$(PROG): Makefile $(OBJ)
	$(call quiet,CC) $(LDFLAGS) $(CFLAGS) -o $(PROG) $(OBJ) $(LIBS)

ao_product.h: ao-make-product.5c ../Version
	$(call quiet,NICKLE,$<) $< -m altusmetrum.org -i $(IDPRODUCT) -p $(PRODUCT) -v $(VERSION) > $@

$(OBJ): $(INC)

distclean:	clean

clean:
	rm -f *.o $(PROG)
	rm -f ao_product.h

install:

uninstall:
