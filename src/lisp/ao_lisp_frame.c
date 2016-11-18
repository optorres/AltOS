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

static inline int
frame_num_size(int num)
{
	return sizeof (struct ao_lisp_frame) + num * sizeof (struct ao_lisp_val);
}

static int
frame_size(void *addr)
{
	struct ao_lisp_frame	*frame = addr;
	return frame_num_size(ao_lisp_frame_num(frame));
}

static void
frame_mark(void *addr)
{
	struct ao_lisp_frame	*frame = addr;
	int			f;

	for (;;) {
		MDBG_MOVE("frame mark %d\n", MDBG_OFFSET(frame));
		if (!AO_LISP_IS_POOL(frame))
			break;
		for (f = 0; f < ao_lisp_frame_num(frame); f++) {
			struct ao_lisp_val	*v = &frame->vals[f];

			ao_lisp_poly_mark(v->val, 0);
			MDBG_MOVE("frame mark atom %s %d val %d at %d\n",
				  ao_lisp_poly_atom(v->atom)->name,
				  MDBG_OFFSET(ao_lisp_ref(v->atom)),
				  MDBG_OFFSET(ao_lisp_ref(v->val)), f);
		}
		frame = ao_lisp_poly_frame(frame->prev);
		MDBG_MOVE("frame next %d\n", MDBG_OFFSET(frame));
		if (!frame)
			break;
		if (ao_lisp_mark_memory(&ao_lisp_frame_type, frame))
			break;
	}
}

static void
frame_move(void *addr)
{
	struct ao_lisp_frame	*frame = addr;
	int			f;

	for (;;) {
		struct ao_lisp_frame	*prev;
		int			ret;

		MDBG_MOVE("frame move %d\n", MDBG_OFFSET(frame));
		if (!AO_LISP_IS_POOL(frame))
			break;
		for (f = 0; f < ao_lisp_frame_num(frame); f++) {
			struct ao_lisp_val	*v = &frame->vals[f];

			ao_lisp_poly_move(&v->atom, 0);
			ao_lisp_poly_move(&v->val, 0);
			MDBG_MOVE("frame move atom %s %d val %d at %d\n",
				  ao_lisp_poly_atom(v->atom)->name,
				  MDBG_OFFSET(ao_lisp_ref(v->atom)),
				  MDBG_OFFSET(ao_lisp_ref(v->val)), f);
		}
		prev = ao_lisp_poly_frame(frame->prev);
		if (!prev)
			break;
		ret = ao_lisp_move_memory(&ao_lisp_frame_type, (void **) &prev);
		if (prev != ao_lisp_poly_frame(frame->prev)) {
			MDBG_MOVE("frame prev moved from %d to %d\n",
				  MDBG_OFFSET(ao_lisp_poly_frame(frame->prev)),
				  MDBG_OFFSET(prev));
			frame->prev = ao_lisp_frame_poly(prev);
		}
		if (ret)
			break;
		frame = prev;
	}
}

const struct ao_lisp_type ao_lisp_frame_type = {
	.mark = frame_mark,
	.size = frame_size,
	.move = frame_move,
	.name = "frame",
};

void
ao_lisp_frame_print(ao_poly p)
{
	struct ao_lisp_frame	*frame = ao_lisp_poly_frame(p);
	int			f;

	printf ("{");
	if (frame) {
		for (f = 0; f < ao_lisp_frame_num(frame); f++) {
			if (f != 0)
				printf(", ");
			ao_lisp_poly_print(frame->vals[f].atom);
			printf(" = ");
			ao_lisp_poly_print(frame->vals[f].val);
		}
		if (frame->prev)
			ao_lisp_poly_print(frame->prev);
	}
	printf("}");
}

ao_poly *
ao_lisp_frame_ref(struct ao_lisp_frame *frame, ao_poly atom)
{
	int f;
	for (f = 0; f < ao_lisp_frame_num(frame); f++)
		if (frame->vals[f].atom == atom)
			return &frame->vals[f].val;
	return NULL;
}

int
ao_lisp_frame_set(struct ao_lisp_frame *frame, ao_poly atom, ao_poly val)
{
	while (frame) {
		if (!AO_LISP_IS_CONST(frame)) {
			ao_poly *ref = ao_lisp_frame_ref(frame, atom);
			if (ref) {
				*ref = val;
				return 1;
			}
		}
		frame = ao_lisp_poly_frame(frame->prev);
	}
	return 0;
}

ao_poly
ao_lisp_frame_get(struct ao_lisp_frame *frame, ao_poly atom)
{
	while (frame) {
		ao_poly *ref = ao_lisp_frame_ref(frame, atom);
		if (ref)
			return *ref;
		frame = ao_lisp_poly_frame(frame->prev);
	}
	return AO_LISP_NIL;
}

struct ao_lisp_frame	*ao_lisp_frame_free_list[AO_LISP_FRAME_FREE];

struct ao_lisp_frame *
ao_lisp_frame_new(int num)
{
	struct ao_lisp_frame	*frame;

	if (num < AO_LISP_FRAME_FREE && (frame = ao_lisp_frame_free_list[num]))
		ao_lisp_frame_free_list[num] = ao_lisp_poly_frame(frame->prev);
	else {
		frame = ao_lisp_alloc(frame_num_size(num));
		if (!frame)
			return NULL;
	}
	frame->type = AO_LISP_FRAME;
	frame->_num = num;
	frame->prev = AO_LISP_NIL;
	memset(frame->vals, '\0', num * sizeof (struct ao_lisp_val));
	return frame;
}

ao_poly
ao_lisp_frame_mark(struct ao_lisp_frame *frame)
{
	if (!frame)
		return AO_LISP_NIL;
	frame->_num |= AO_LISP_FRAME_MARK;
	return ao_lisp_frame_poly(frame);
}

void
ao_lisp_frame_free(struct ao_lisp_frame *frame)
{
	if (!ao_lisp_frame_marked(frame)) {
		int	num = ao_lisp_frame_num(frame);
		if (num < AO_LISP_FRAME_FREE) {
			frame->prev = ao_lisp_frame_poly(ao_lisp_frame_free_list[num]);
			ao_lisp_frame_free_list[num] = frame;
		}
	}
}

static struct ao_lisp_frame *
ao_lisp_frame_realloc(struct ao_lisp_frame **frame_ref, int new_num)
{
	struct ao_lisp_frame	*frame = *frame_ref;
	struct ao_lisp_frame	*new;
	int			copy;

	if (new_num == ao_lisp_frame_num(frame))
		return frame;
	new = ao_lisp_frame_new(new_num);
	if (!new)
		return NULL;
	/*
	 * Re-fetch the frame as it may have moved
	 * during the allocation
	 */
	frame = *frame_ref;
	copy = new_num;
	if (copy > ao_lisp_frame_num(frame))
		copy = ao_lisp_frame_num(frame);
	memcpy(new->vals, frame->vals, copy * sizeof (struct ao_lisp_val));
	new->prev = frame->prev;
	ao_lisp_frame_free(frame);
	return new;
}

int
ao_lisp_frame_add(struct ao_lisp_frame **frame_ref, ao_poly atom, ao_poly val)
{
	struct ao_lisp_frame *frame = *frame_ref;
	ao_poly *ref = frame ? ao_lisp_frame_ref(frame, atom) : NULL;

	if (!ref) {
		int f;
		ao_lisp_poly_stash(0, atom);
		ao_lisp_poly_stash(1, val);
		if (frame) {
			f = ao_lisp_frame_num(frame);
			frame = ao_lisp_frame_realloc(frame_ref, f + 1);
		} else {
			f = 0;
			frame = ao_lisp_frame_new(1);
		}
		atom = ao_lisp_poly_fetch(0);
		val = ao_lisp_poly_fetch(1);
		if (!frame)
			return 0;
		*frame_ref = frame;
		frame->vals[f].atom = atom;
		ref = &frame->vals[f].val;
	}
	*ref = val;
	return 1;
}
