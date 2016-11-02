/*
 * Copyright © 2016 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "ao_lisp.h"
#include <stdlib.h>
#include <ctype.h>

static struct ao_lisp_builtin *
ao_lisp_make_builtin(enum ao_lisp_builtin_id func, int args) {
	struct ao_lisp_builtin *b = ao_lisp_alloc(sizeof (struct ao_lisp_builtin));

	b->type = AO_LISP_BUILTIN;
	b->func = func;
	b->args = args;
	return b;
}

struct builtin_func {
	char	*name;
	int	args;
	int	func;
};

struct builtin_func funcs[] = {
	"car",		AO_LISP_LEXPR,	builtin_car,
	"cdr",		AO_LISP_LEXPR,	builtin_cdr,
	"cons",		AO_LISP_LEXPR,	builtin_cons,
	"quote",	AO_LISP_NLAMBDA,builtin_quote,
	"set",		AO_LISP_LEXPR,	builtin_set,
	"setq",		AO_LISP_MACRO,	builtin_setq,
	"print",	AO_LISP_LEXPR,	builtin_print,
	"+",		AO_LISP_LEXPR,	builtin_plus,
	"-",		AO_LISP_LEXPR,	builtin_minus,
	"*",		AO_LISP_LEXPR,	builtin_times,
	"/",		AO_LISP_LEXPR,	builtin_divide,
	"%",		AO_LISP_LEXPR,	builtin_mod
};

#define N_FUNC (sizeof funcs / sizeof funcs[0])

int
main(int argc, char **argv)
{
	int	f, o;
	ao_poly	atom, val;
	struct ao_lisp_atom	*a;

	for (f = 0; f < N_FUNC; f++) {
		struct ao_lisp_builtin	*b = ao_lisp_make_builtin(funcs[f].func, funcs[f].args);
		struct ao_lisp_atom	*a = ao_lisp_atom_intern(funcs[f].name);
		a->val = ao_lisp_builtin_poly(b);
	}

	for (;;) {
		atom = ao_lisp_read();
		if (!atom)
			break;
		val = ao_lisp_read();
		if (!val)
			break;
		if (ao_lisp_poly_type(atom) != AO_LISP_ATOM) {
			fprintf(stderr, "input must be atom val pairs\n");
			exit(1);
		}
		ao_lisp_poly_atom(atom)->val = val;
	}

	printf("/* constant objects, all referenced from atoms */\n\n");
	printf("#define AO_LISP_POOL_CONST %d\n", ao_lisp_top);
	printf("extern const uint8_t ao_lisp_const[AO_LISP_POOL_CONST] __attribute__((aligned(4)));\n");
	printf("#define ao_builtin_atoms 0x%04x\n", ao_lisp_atom_poly(ao_lisp_atoms));

	for (a = ao_lisp_atoms; a; a = ao_lisp_poly_atom(a->next)) {
		char	*n = a->name, c;
		printf ("#define _ao_lisp_atom_");
		while ((c = *n++)) {
			if (isalnum(c))
				printf("%c", c);
			else
				printf("%02x", c);
		}
		printf("  0x%04x\n", ao_lisp_atom_poly(a));
	}
	printf("#ifdef AO_LISP_CONST_BITS\n");
	printf("const uint8_t ao_lisp_const[] = {");
	for (o = 0; o < ao_lisp_top; o++) {
		uint8_t	c;
		if ((o & 0xf) == 0)
			printf("\n\t");
		else
			printf(" ");
		c = ao_lisp_const[o];
		if (' ' < c && c <= '~' && c != '\'')
			printf (" '%c',", c);
		else
			printf("0x%02x,", c);
	}
	printf("\n};\n");
	printf("#endif /* AO_LISP_CONST_BITS */\n");
}
