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
	ao_product.c \
	ao_romconfig.c \
	ao_cmd.c \
	ao_task.c \
	ao_led_stm.c \
	ao_stdio.c \
	ao_panic.c \
	ao_timer.c \
	ao_lcd_stm.c \
	ao_lcd_font.c \
	ao_mutex.c \
	ao_storage.c \
	ao_m25.c \
	ao_dma_stm.c \
	ao_spi_stm.c \
	ao_adc_stm.c \
	ao_data.c \
	ao_i2c_stm.c \
	ao_usb_stm.c \
	ao_exti_stm.c

PRODUCT=StmDemo-v0.0
PRODUCT_DEF=-DSTM_DEMO
IDPRODUCT=0x000a

CFLAGS = $(PRODUCT_DEF) $(STM_CFLAGS)

PROG=stm-demo-$(VERSION)
ELF=$(PROG).elf
IHX=$(PROG).ihx

SRC=$(ALTOS_SRC) ao_demo.c
OBJ=$(SRC:.c=.o)

all: $(ELF) $(IHX)

LDFLAGS=-L../stm -Wl,-Taltos.ld

$(ELF): Makefile $(OBJ)
	$(call quiet,CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

ao_product.h: ao-make-product.5c ../Version
	$(call quiet,NICKLE,$<) $< -m altusmetrum.org -i $(IDPRODUCT) -p $(PRODUCT) -v $(VERSION) > $@

$(OBJ): $(INC)

distclean:	clean

clean:
	rm -f *.o *.elf *.ihx
	rm -f ao_product.h

install:

uninstall:
