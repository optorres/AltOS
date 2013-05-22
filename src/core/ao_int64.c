/*
 * Copyright © 2013 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include <ao_int64.h>

void ao_plus64(ao_int64_t *r, ao_int64_t *a, ao_int64_t *b) {
	uint32_t	t;

	r->low = t = a->low + b->low;
	r->high = a->high + b->high;
	if (t < a->low)
		r->high++;
}

void ao_rshift64(ao_int64_t *r, ao_int64_t *a, uint8_t d) {
	if (d < 32) {
		r->high = (int32_t) a->high >> d;
		r->low = a->low >> d;
		if (d)
			r->low |= a->high << (32 - d);
	} else {
		d &= 0x1f;
		r->high = 0;
		r->low = (int32_t) a->high >> d;
	}
}

void ao_lshift64(ao_int64_t *r, ao_int64_t *a, uint8_t d) {
	if (d < 32) {
		r->high = a->high << d;
		r->low = a->low << d;
		if (d)
			r->high |= a->low >> (32 - d);
	} else {
		d &= 0x1f;
		r->low = 0;
		r->high = a->low << d;
	}
}

void ao_umul64(ao_int64_t *r, uint32_t a, uint32_t b)
{
	uint32_t	r1;
	uint32_t	r2, r3, r4;
	ao_int64_t	s,t,u,v;
	r1 = (uint32_t) (uint16_t) a * (uint16_t) b;
	r2 = (uint32_t) (uint16_t) (a >> 16) * (uint16_t) b;
	r3 = (uint32_t) (uint16_t) a * (uint16_t) (b >> 16);
	r4 = (uint32_t) (uint16_t) (a >> 16) * (uint16_t) (b >> 16);

	s.low = r1;
	s.high = r4;

	t.high = (uint32_t) r2 >> 16;
	t.low = r2 << 16;
	ao_plus64(&u, &s, &t);

	v.high = (int32_t) r3 >> 16;
	v.low = r3 << 16;
	ao_plus64(r, &u, &v);
}

void ao_neg64(ao_int64_t *r, ao_int64_t *a) {
	r->high = ~a->high;
	r->low = ~a->low;
	if (!++r->low)
		r->high++;
}

void ao_mul64(ao_int64_t *r, int32_t a, int32_t b) {
	uint8_t		negative = 0;

	if (a < 0) {
		a = -a;
		negative = 1;
	}
	if (b < 0) {
		b = -b;
		negative = !negative;
	}
	ao_umul64(r, a, b);
	if (negative)
		ao_neg64(r, r);
}

void ao_umul64_16(ao_int64_t *r, ao_int64_t *a, uint16_t b) {
	uint32_t	low = a->low;
	ao_umul64(r, (uint32_t) low >> 1, (uint32_t) b << 1);
	if (low & 1) {
		if ((uint32_t) (r->low += b) < (uint32_t) b)
			r->high++;
	}
	r->high += a->high * b;
}

void ao_mul64_16(ao_int64_t *r, ao_int64_t *a, uint16_t b) {
	if ((int32_t) a->high < 0) {
		ao_int64_t	t;

		ao_neg64(&t, a);
		ao_umul64_16(r, &t, b);
		ao_neg64(r, r);
	} else
		ao_umul64_16(r, a, b);
}
