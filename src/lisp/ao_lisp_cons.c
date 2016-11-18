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

#define OFFSET(a)	((int) ((uint8_t *) (a) - ao_lisp_const))

static void cons_mark(void *addr)
{
	struct ao_lisp_cons	*cons = addr;

	for (;;) {
		ao_lisp_poly_mark(cons->car, 1);
		cons = ao_lisp_poly_cons(cons->cdr);
		if (!cons)
			break;
		if (ao_lisp_mark_memory(cons, sizeof (struct ao_lisp_cons)))
			break;
	}
}

static int cons_size(void *addr)
{
	(void) addr;
	return sizeof (struct ao_lisp_cons);
}

static void cons_move(void *addr)
{
	struct ao_lisp_cons	*cons = addr;

	if (!cons)
		return;

	for (;;) {
		struct ao_lisp_cons	*cdr;
		int			ret;

		(void) ao_lisp_poly_move(&cons->car, 1);
		cdr = ao_lisp_poly_cons(cons->cdr);
		if (!cdr)
			break;
		ret = ao_lisp_move_memory((void **) &cdr, sizeof (struct ao_lisp_cons));
		if (cdr != ao_lisp_poly_cons(cons->cdr))
			cons->cdr = ao_lisp_cons_poly(cdr);
		if (ret)
			break;
		cons = cdr;
	}
}

const struct ao_lisp_type ao_lisp_cons_type = {
	.mark = cons_mark,
	.size = cons_size,
	.move = cons_move,
};

static ao_poly	cons_car;
static struct ao_lisp_cons *cons_cdr;
static int been_here;

struct ao_lisp_cons *
ao_lisp_cons_cons(ao_poly car, struct ao_lisp_cons *cdr)
{
	struct ao_lisp_cons	*cons;

	if (!been_here) {
		ao_lisp_root_add(&ao_lisp_cons_type, &cons_cdr);
		ao_lisp_root_poly_add(&cons_car);
		been_here = 1;
	}
	cons_car = car;
	cons_cdr = cdr;
	cons = ao_lisp_alloc(sizeof (struct ao_lisp_cons));
	if (!cons)
		return NULL;
	cons->car = cons_car;
	cons->cdr = ao_lisp_cons_poly(cons_cdr);
	cons_car = AO_LISP_NIL;
	cons_cdr = NULL;
	return cons;
}

void
ao_lisp_cons_print(ao_poly c)
{
	struct ao_lisp_cons *cons = ao_lisp_poly_cons(c);
	int	first = 1;
	printf("(");
	while (cons) {
		if (!first)
			printf(" ");
		ao_lisp_poly_print(cons->car);
		cons = ao_lisp_poly_cons(cons->cdr);
		first = 0;
	}
	printf(")");
}

void
ao_lisp_cons_patom(ao_poly c)
{
	struct ao_lisp_cons *cons = ao_lisp_poly_cons(c);

	while (cons) {
		ao_lisp_poly_patom(cons->car);
		cons = ao_lisp_poly_cons(cons->cdr);
	}
}

int
ao_lisp_cons_length(struct ao_lisp_cons *cons)
{
	int	len = 0;
	while (cons) {
		len++;
		cons = ao_lisp_poly_cons(cons->cdr);
	}
	return len;
}
