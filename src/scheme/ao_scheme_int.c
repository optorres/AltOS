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

#include "ao_scheme.h"

void
ao_scheme_int_write(ao_poly p, bool write)
{
	int i = ao_scheme_poly_int(p);
	(void) write;
	printf("%d", i);
}

#ifdef AO_SCHEME_FEATURE_BIGINT

int32_t
ao_scheme_poly_integer(ao_poly p, bool *fail)
{
	if (fail)
		*fail = false;
	switch (ao_scheme_poly_base_type(p)) {
	case AO_SCHEME_INT:
		return ao_scheme_poly_int(p);
	case AO_SCHEME_BIGINT:
		return ao_scheme_poly_bigint(p)->value;
	}
	if (fail)
		*fail = true;
	return 0;
}

ao_poly
ao_scheme_integer_poly(int32_t p)
{
	struct ao_scheme_bigint	*bi;

	if (AO_SCHEME_MIN_INT <= p && p <= AO_SCHEME_MAX_INT)
		return ao_scheme_int_poly(p);
	bi = ao_scheme_alloc(sizeof (struct ao_scheme_bigint));
	bi->value = p;
	return ao_scheme_bigint_poly(bi);
}

static void bigint_mark(void *addr)
{
	(void) addr;
}

static int bigint_size(void *addr)
{
	if (!addr)
		return 0;
	return sizeof (struct ao_scheme_bigint);
}

static void bigint_move(void *addr)
{
	(void) addr;
}

const struct ao_scheme_type ao_scheme_bigint_type = {
	.mark = bigint_mark,
	.size = bigint_size,
	.move = bigint_move,
	.name = "bigint",
};

void
ao_scheme_bigint_write(ao_poly p, bool write)
{
	struct ao_scheme_bigint	*bi = ao_scheme_poly_bigint(p);

	(void) write;
	printf("%d", bi->value);
}
#endif /* AO_SCHEME_FEATURE_BIGINT */
