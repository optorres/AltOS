#
# AltOS build
#
#

include ../stmf0/Makefile.defs

INC = \
	ao.h \
	ao_arch.h \
	ao_arch_funcs.h \
	ao_boot.h \
	ao_pins.h \
	ao_product.h \
	ao_task.h \
	ao_lisp.h \
	ao_lisp_const.h \
	stm32f0.h \
	Makefile

ALTOS_SRC = \
	ao_boot_chain.c \
	ao_interrupt.c \
	ao_product.c \
	ao_romconfig.c \
	ao_cmd.c \
	ao_config.c \
	ao_task.c \
	ao_led_stmf0.c \
	ao_beep_stm.c \
	ao_dma_stm.c \
	ao_stdio.c \
	ao_panic.c \
	ao_timer.c \
	ao_mutex.c \
	ao_usb_stm.c \
	ao_serial_stm.c \
	ao_flash_stm.c \
	ao_lisp_atom.c \
	ao_lisp_builtin.c \
	ao_lisp_cons.c \
	ao_lisp_error.c \
	ao_lisp_eval.c \
	ao_lisp_frame.c \
	ao_lisp_int.c \
	ao_lisp_lambda.c \
	ao_lisp_lex.c \
	ao_lisp_mem.c \
	ao_lisp_poly.c \
	ao_lisp_read.c \
	ao_lisp_rep.c \
	ao_lisp_save.c \
	ao_lisp_stack.c \
	ao_lisp_string.c \
	ao_lisp_os_save.c

PRODUCT=Nucleo-32
PRODUCT_DEF=-DNUCLEO
IDPRODUCT=0x000a

CFLAGS = $(PRODUCT_DEF) $(STMF0_CFLAGS)

LDFLAGS=$(CFLAGS) -L$(TOPDIR)/stmf0 -Wl,-Tload.ld -n

PROGNAME=nucleo-32
PROG=$(PROGNAME)-$(VERSION).elf
HEX=$(PROGNAME)-$(VERSION).ihx

SRC=$(ALTOS_SRC) ao_nucleo.c
OBJ=$(SRC:.c=.o)

all: $(PROG) $(HEX)

$(PROG): Makefile $(OBJ) altos.ld
	$(call quiet,CC) $(LDFLAGS) $(CFLAGS) -o $(PROG) $(OBJ) $(LIBS)

$(OBJ): $(INC)

ao_product.h: ao-make-product.5c ../Version
	$(call quiet,NICKLE,$<) $< -m altusmetrum.org -i $(IDPRODUCT) -p $(PRODUCT) -v $(VERSION) > $@

load: $(PROG)
	stm-load $(PROG)

distclean:	clean

clean:
	rm -f *.o $(PROGNAME)-*.elf $(PROGNAME)-*.ihx
	rm -f ao_product.h

install:

uninstall:
