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

#ifndef _AO_SCHEME_H_
#define _AO_SCHEME_H_

#ifndef DBG_MEM
#define DBG_MEM		0
#endif
#ifndef DBG_EVAL
#define DBG_EVAL	0
#endif
#ifndef DBG_READ
#define DBG_READ	0
#endif
#ifndef DBG_FREE_CONS
#define DBG_FREE_CONS	0
#endif
#define NDEBUG		1

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#define AO_SCHEME_BUILTIN_FEATURES
#include "ao_scheme_builtin.h"
#undef AO_SCHEME_BUILTIN_FEATURES
#include <ao_scheme_os.h>
#ifndef __BYTE_ORDER
#include <endian.h>
#endif

typedef uint16_t	ao_poly;
typedef int16_t		ao_signed_poly;

#if AO_SCHEME_SAVE

struct ao_scheme_os_save {
	ao_poly		atoms;
	ao_poly		globals;
	uint16_t	const_checksum;
	uint16_t	const_checksum_inv;
};

#ifndef AO_SCHEME_POOL_TOTAL
#error Must define AO_SCHEME_POOL_TOTAL for AO_SCHEME_SAVE
#endif

#define AO_SCHEME_POOL_EXTRA	(sizeof(struct ao_scheme_os_save))
#define AO_SCHEME_POOL	((int) (AO_SCHEME_POOL_TOTAL - AO_SCHEME_POOL_EXTRA))

int
ao_scheme_os_save(void);

int
ao_scheme_os_restore_save(struct ao_scheme_os_save *save, int offset);

int
ao_scheme_os_restore(void);

#endif

#ifdef AO_SCHEME_MAKE_CONST
#define AO_SCHEME_POOL_CONST	16384
extern uint8_t ao_scheme_const[AO_SCHEME_POOL_CONST] __attribute__((aligned(4)));
#define ao_scheme_pool ao_scheme_const
#define AO_SCHEME_POOL AO_SCHEME_POOL_CONST

#define _atom(n) ao_scheme_atom_poly(ao_scheme_atom_intern((char *) n))
#define _bool(v) ao_scheme_bool_poly(ao_scheme_bool_get(v))

#define _ao_scheme_bool_true	_bool(1)
#define _ao_scheme_bool_false	_bool(0)

#define _ao_scheme_atom_eof	_atom("eof")
#define _ao_scheme_atom_else	_atom("else")

#define AO_SCHEME_BUILTIN_ATOMS
#include "ao_scheme_builtin.h"

#else
#include "ao_scheme_const.h"
#ifndef AO_SCHEME_POOL
#error Must define AO_SCHEME_POOL
#endif
#ifndef AO_SCHEME_POOL_EXTRA
#define AO_SCHEME_POOL_EXTRA 0
#endif
extern uint8_t		ao_scheme_pool[AO_SCHEME_POOL + AO_SCHEME_POOL_EXTRA] __attribute__((aligned(4)));
#endif

/* Primitive types */
#define AO_SCHEME_CONS		0
#define AO_SCHEME_INT		1
#define AO_SCHEME_BIGINT	2
#define AO_SCHEME_OTHER		3

#define AO_SCHEME_TYPE_MASK	0x0003
#define AO_SCHEME_TYPE_SHIFT	2
#define AO_SCHEME_REF_MASK	0x7ffc
#define AO_SCHEME_CONST		0x8000

/* These have a type value at the start of the struct */
#define AO_SCHEME_ATOM		4
#define AO_SCHEME_BUILTIN	5
#define AO_SCHEME_FRAME		6
#define AO_SCHEME_FRAME_VALS	7
#define AO_SCHEME_LAMBDA	8
#define AO_SCHEME_STACK		9
#define AO_SCHEME_BOOL		10
#define AO_SCHEME_STRING	11
#ifdef AO_SCHEME_FEATURE_FLOAT
#define AO_SCHEME_FLOAT		12
#define _AO_SCHEME_FLOAT	AO_SCHEME_FLOAT
#else
#define _AO_SCHEME_FLOAT	12
#endif
#ifdef AO_SCHEME_FEATURE_VECTOR
#define AO_SCHEME_VECTOR	13
#define _AO_SCHEME_VECTOR	AO_SCHEME_VECTOR
#else
#define _AO_SCHEME_VECTOR	_AO_SCHEME_FLOAT
#endif
#define AO_SCHEME_NUM_TYPE	(_AO_SCHEME_VECTOR+1)

/* Leave two bits for types to use as they please */
#define AO_SCHEME_OTHER_TYPE_MASK	0x3f

#define AO_SCHEME_NIL	0

extern uint16_t		ao_scheme_top;

#define AO_SCHEME_OOM			0x01
#define AO_SCHEME_DIVIDE_BY_ZERO	0x02
#define AO_SCHEME_INVALID		0x04
#define AO_SCHEME_UNDEFINED		0x08
#define AO_SCHEME_REDEFINED		0x10
#define AO_SCHEME_EOF			0x20
#define AO_SCHEME_EXIT			0x40

extern uint8_t		ao_scheme_exception;

static inline int
ao_scheme_is_const(ao_poly poly) {
	return poly & AO_SCHEME_CONST;
}

static inline int
ao_scheme_is_const_addr(const void *addr) {
	const uint8_t *a = addr;
	return (ao_scheme_const <= a) && (a < ao_scheme_const + AO_SCHEME_POOL_CONST);
}

static inline int
ao_scheme_is_pool_addr(const void *addr) {
	const uint8_t *a = addr;
	return (ao_scheme_pool <= a) && (a < ao_scheme_pool + AO_SCHEME_POOL);
}

void *
ao_scheme_ref(ao_poly poly);

ao_poly
ao_scheme_poly(const void *addr, ao_poly type);

struct ao_scheme_type {
	int	(*size)(void *addr);
	void	(*mark)(void *addr);
	void	(*move)(void *addr);
	char	name[];
};

struct ao_scheme_cons {
	ao_poly		car;
	ao_poly		cdr;
};

struct ao_scheme_atom {
	uint8_t		type;
	uint8_t		pad[1];
	ao_poly		next;
	char		name[];
};

struct ao_scheme_string {
	uint8_t		type;
	char		val[];
};

struct ao_scheme_val {
	ao_poly		atom;
	ao_poly		val;
};

struct ao_scheme_frame_vals {
	uint8_t			type;
	uint8_t			size;
	struct ao_scheme_val	vals[];
};

struct ao_scheme_frame {
	uint8_t			type;
	uint8_t			num;
	ao_poly			prev;
	ao_poly			vals;
};

struct ao_scheme_bool {
	uint8_t			type;
	uint8_t			value;
	uint16_t		pad;
};


#ifdef AO_SCHEME_FEATURE_FLOAT
struct ao_scheme_float {
	uint8_t			type;
	uint8_t			pad1;
	uint16_t		pad2;
	float			value;
};
#endif

#ifdef AO_SCHEME_FEATURE_VECTOR
struct ao_scheme_vector {
	uint8_t			type;
	uint8_t			pad1;
	uint16_t		length;
	ao_poly			vals[];
};
#endif

#define AO_SCHEME_MIN_INT	(-(1 << (15 - AO_SCHEME_TYPE_SHIFT)))
#define AO_SCHEME_MAX_INT	((1 << (15 - AO_SCHEME_TYPE_SHIFT)) - 1)

#ifdef AO_SCHEME_FEATURE_BIGINT

struct ao_scheme_bigint {
	uint32_t		value;
};

#define AO_SCHEME_MIN_BIGINT	INT32_MIN
#define AO_SCHEME_MAX_BIGINT	INT32_MAX

#endif	/* AO_SCHEME_FEATURE_BIGINT */

/* Set on type when the frame escapes the lambda */
#define AO_SCHEME_FRAME_MARK	0x80

static inline int ao_scheme_frame_marked(struct ao_scheme_frame *f) {
	return f->type & AO_SCHEME_FRAME_MARK;
}

static inline struct ao_scheme_frame *
ao_scheme_poly_frame(ao_poly poly) {
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_frame_poly(struct ao_scheme_frame *frame) {
	return ao_scheme_poly(frame, AO_SCHEME_OTHER);
}

static inline struct ao_scheme_frame_vals *
ao_scheme_poly_frame_vals(ao_poly poly) {
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_frame_vals_poly(struct ao_scheme_frame_vals *vals) {
	return ao_scheme_poly(vals, AO_SCHEME_OTHER);
}

enum eval_state {
	eval_sexpr,		/* Evaluate an sexpr */
	eval_val,		/* Value computed */
	eval_formal,		/* Formal computed */
	eval_exec,		/* Start a lambda evaluation */
	eval_apply,		/* Execute apply */
	eval_cond,		/* Start next cond clause */
	eval_cond_test,		/* Check cond condition */
	eval_begin,		/* Start next begin entry */
	eval_while,		/* Start while condition */
	eval_while_test,	/* Check while condition */
	eval_macro,		/* Finished with macro generation */
};

struct ao_scheme_stack {
	uint8_t			type;		/* AO_SCHEME_STACK */
	uint8_t			state;		/* enum eval_state */
	ao_poly			prev;		/* previous stack frame */
	ao_poly			sexprs;		/* expressions to evaluate */
	ao_poly			values;		/* values computed */
	ao_poly			values_tail;	/* end of the values list for easy appending */
	ao_poly			frame;		/* current lookup frame */
	ao_poly			list;		/* most recent function call */
};

#define AO_SCHEME_STACK_MARK	0x80	/* set on type when a reference has been taken */

static inline int ao_scheme_stack_marked(struct ao_scheme_stack *s) {
	return s->type & AO_SCHEME_STACK_MARK;
}

static inline void ao_scheme_stack_mark(struct ao_scheme_stack *s) {
	s->type |= AO_SCHEME_STACK_MARK;
}

static inline struct ao_scheme_stack *
ao_scheme_poly_stack(ao_poly p)
{
	return ao_scheme_ref(p);
}

static inline ao_poly
ao_scheme_stack_poly(struct ao_scheme_stack *stack)
{
	return ao_scheme_poly(stack, AO_SCHEME_OTHER);
}

extern ao_poly			ao_scheme_v;

#define AO_SCHEME_FUNC_LAMBDA		0
#define AO_SCHEME_FUNC_NLAMBDA		1
#define AO_SCHEME_FUNC_MACRO		2

#define AO_SCHEME_FUNC_FREE_ARGS	0x80
#define AO_SCHEME_FUNC_MASK		0x7f

#define AO_SCHEME_FUNC_F_LAMBDA		(AO_SCHEME_FUNC_FREE_ARGS | AO_SCHEME_FUNC_LAMBDA)
#define AO_SCHEME_FUNC_F_NLAMBDA	(AO_SCHEME_FUNC_FREE_ARGS | AO_SCHEME_FUNC_NLAMBDA)
#define AO_SCHEME_FUNC_F_MACRO		(AO_SCHEME_FUNC_FREE_ARGS | AO_SCHEME_FUNC_MACRO)

struct ao_scheme_builtin {
	uint8_t		type;
	uint8_t		args;
	uint16_t	func;
};

#define AO_SCHEME_BUILTIN_ID
#include "ao_scheme_builtin.h"

typedef ao_poly (*ao_scheme_func_t)(struct ao_scheme_cons *cons);

extern const ao_scheme_func_t	ao_scheme_builtins[];

static inline ao_scheme_func_t
ao_scheme_func(struct ao_scheme_builtin *b)
{
	return ao_scheme_builtins[b->func];
}

struct ao_scheme_lambda {
	uint8_t		type;
	uint8_t		args;
	ao_poly		code;
	ao_poly		frame;
};

static inline struct ao_scheme_lambda *
ao_scheme_poly_lambda(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_lambda_poly(struct ao_scheme_lambda *lambda)
{
	return ao_scheme_poly(lambda, AO_SCHEME_OTHER);
}

static inline void *
ao_scheme_poly_other(ao_poly poly) {
	return ao_scheme_ref(poly);
}

static inline uint8_t
ao_scheme_other_type(void *other) {
#if DBG_MEM
	if ((*((uint8_t *) other) & AO_SCHEME_OTHER_TYPE_MASK) >= AO_SCHEME_NUM_TYPE)
		ao_scheme_abort();
#endif
	return *((uint8_t *) other) & AO_SCHEME_OTHER_TYPE_MASK;
}

static inline ao_poly
ao_scheme_other_poly(const void *other)
{
	return ao_scheme_poly(other, AO_SCHEME_OTHER);
}

static inline int
ao_scheme_size_round(int size)
{
	return (size + 3) & ~3;
}

static inline int
ao_scheme_size(const struct ao_scheme_type *type, void *addr)
{
	return ao_scheme_size_round(type->size(addr));
}

#define AO_SCHEME_OTHER_POLY(other) ((ao_poly)(other) + AO_SCHEME_OTHER)

static inline int ao_scheme_poly_base_type(ao_poly poly) {
	return poly & AO_SCHEME_TYPE_MASK;
}

static inline int ao_scheme_poly_type(ao_poly poly) {
	int	type = poly & AO_SCHEME_TYPE_MASK;
	if (type == AO_SCHEME_OTHER)
		return ao_scheme_other_type(ao_scheme_poly_other(poly));
	return type;
}

static inline int
ao_scheme_is_cons(ao_poly poly) {
	return (ao_scheme_poly_base_type(poly) == AO_SCHEME_CONS);
}

static inline int
ao_scheme_is_pair(ao_poly poly) {
	return poly != AO_SCHEME_NIL && (ao_scheme_poly_base_type(poly) == AO_SCHEME_CONS);
}

static inline struct ao_scheme_cons *
ao_scheme_poly_cons(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_cons_poly(struct ao_scheme_cons *cons)
{
	return ao_scheme_poly(cons, AO_SCHEME_CONS);
}

static inline int32_t
ao_scheme_poly_int(ao_poly poly)
{
	return (int32_t) ((ao_signed_poly) poly >> AO_SCHEME_TYPE_SHIFT);
}

static inline ao_poly
ao_scheme_int_poly(int32_t i)
{
	return ((ao_poly) i << 2) | AO_SCHEME_INT;
}

#ifdef AO_SCHEME_FEATURE_BIGINT
static inline struct ao_scheme_bigint *
ao_scheme_poly_bigint(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_bigint_poly(struct ao_scheme_bigint *bi)
{
	return ao_scheme_poly(bi, AO_SCHEME_BIGINT);
}
#endif /* AO_SCHEME_FEATURE_BIGINT */

static inline struct ao_scheme_string *
ao_scheme_poly_string(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_string_poly(struct ao_scheme_string *s)
{
	return ao_scheme_poly(s, AO_SCHEME_OTHER);
}

static inline struct ao_scheme_atom *
ao_scheme_poly_atom(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_atom_poly(struct ao_scheme_atom *a)
{
	return ao_scheme_poly(a, AO_SCHEME_OTHER);
}

static inline struct ao_scheme_builtin *
ao_scheme_poly_builtin(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

static inline ao_poly
ao_scheme_builtin_poly(struct ao_scheme_builtin *b)
{
	return ao_scheme_poly(b, AO_SCHEME_OTHER);
}

static inline ao_poly
ao_scheme_bool_poly(struct ao_scheme_bool *b)
{
	return ao_scheme_poly(b, AO_SCHEME_OTHER);
}

static inline struct ao_scheme_bool *
ao_scheme_poly_bool(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

#ifdef AO_SCHEME_FEATURE_FLOAT
static inline ao_poly
ao_scheme_float_poly(struct ao_scheme_float *f)
{
	return ao_scheme_poly(f, AO_SCHEME_OTHER);
}

static inline struct ao_scheme_float *
ao_scheme_poly_float(ao_poly poly)
{
	return ao_scheme_ref(poly);
}

float
ao_scheme_poly_number(ao_poly p);
#endif

#ifdef AO_SCHEME_FEATURE_VECTOR
static inline ao_poly
ao_scheme_vector_poly(struct ao_scheme_vector *v)
{
	return ao_scheme_poly(v, AO_SCHEME_OTHER);
}

static inline struct ao_scheme_vector *
ao_scheme_poly_vector(ao_poly poly)
{
	return ao_scheme_ref(poly);
}
#endif

/* memory functions */

extern uint64_t ao_scheme_collects[2];
extern uint64_t ao_scheme_freed[2];
extern uint64_t ao_scheme_loops[2];

/* returns 1 if the object was already marked */
int
ao_scheme_mark_memory(const struct ao_scheme_type *type, void *addr);

/* returns 1 if the object was already moved */
int
ao_scheme_move_memory(const struct ao_scheme_type *type, void **ref);

void *
ao_scheme_alloc(int size);

/* Marks an object as being printed, returns 1 if it was already marked */
int
ao_scheme_print_mark_addr(void *addr);

void
ao_scheme_print_clear_addr(void *addr);

/* Notes that printing has started */
void
ao_scheme_print_start(void);

/* Notes that printing has ended, returns 1 if printing is still happening */
int
ao_scheme_print_stop(void);

#define AO_SCHEME_COLLECT_FULL		1
#define AO_SCHEME_COLLECT_INCREMENTAL	0

int
ao_scheme_collect(uint8_t style);

#if DBG_FREE_CONS
void
ao_scheme_cons_check(struct ao_scheme_cons *cons);
#endif

void
ao_scheme_poly_stash(ao_poly poly);

ao_poly
ao_scheme_poly_fetch(void);

static inline void
ao_scheme_cons_stash(struct ao_scheme_cons *cons) {
	ao_scheme_poly_stash(ao_scheme_cons_poly(cons));
}

static inline struct ao_scheme_cons *
ao_scheme_cons_fetch(void) {
	return ao_scheme_poly_cons(ao_scheme_poly_fetch());
}

static inline void
ao_scheme_atom_stash(struct ao_scheme_atom *atom) {
	ao_scheme_poly_stash(ao_scheme_atom_poly(atom));
}

static inline struct ao_scheme_atom *
ao_scheme_atom_fetch(void) {
	return ao_scheme_poly_atom(ao_scheme_poly_fetch());
}

static inline void
ao_scheme_string_stash(struct ao_scheme_string *string) {
	ao_scheme_poly_stash(ao_scheme_string_poly(string));
}

static inline struct ao_scheme_string *
ao_scheme_string_fetch(void) {
	return ao_scheme_poly_string(ao_scheme_poly_fetch());
}

#ifdef AO_SCHEME_FEATURE_VECTOR
static inline void
ao_scheme_vector_stash(struct ao_scheme_vector *vector) {
	ao_scheme_poly_stash(ao_scheme_vector_poly(vector));
}

static inline struct ao_scheme_vector *
ao_scheme_vector_fetch(void) {
	return ao_scheme_poly_vector(ao_scheme_poly_fetch());
}
#endif

static inline void
ao_scheme_stack_stash(struct ao_scheme_stack *stack) {
	ao_scheme_poly_stash(ao_scheme_stack_poly(stack));
}

static inline struct ao_scheme_stack *
ao_scheme_stack_fetch(void) {
	return ao_scheme_poly_stack(ao_scheme_poly_fetch());
}

static inline void
ao_scheme_frame_stash(struct ao_scheme_frame *frame) {
	ao_scheme_poly_stash(ao_scheme_frame_poly(frame));
}

static inline struct ao_scheme_frame *
ao_scheme_frame_fetch(void) {
	return ao_scheme_poly_frame(ao_scheme_poly_fetch());
}

/* bool */

extern const struct ao_scheme_type ao_scheme_bool_type;

void
ao_scheme_bool_write(ao_poly v, bool write);

#ifdef AO_SCHEME_MAKE_CONST
extern struct ao_scheme_bool	*ao_scheme_true, *ao_scheme_false;

struct ao_scheme_bool *
ao_scheme_bool_get(uint8_t value);
#endif

/* cons */
extern const struct ao_scheme_type ao_scheme_cons_type;

struct ao_scheme_cons *
ao_scheme_cons_cons(ao_poly car, ao_poly cdr);

/* Return a cons or NULL for a proper list, else error */
struct ao_scheme_cons *
ao_scheme_cons_cdr(struct ao_scheme_cons *cons);

ao_poly
ao_scheme_cons(ao_poly car, ao_poly cdr);

extern struct ao_scheme_cons *ao_scheme_cons_free_list;

void
ao_scheme_cons_free(struct ao_scheme_cons *cons);

void
ao_scheme_cons_write(ao_poly, bool write);

int
ao_scheme_cons_length(struct ao_scheme_cons *cons);

struct ao_scheme_cons *
ao_scheme_cons_copy(struct ao_scheme_cons *cons);

/* string */
extern const struct ao_scheme_type ao_scheme_string_type;

struct ao_scheme_string *
ao_scheme_string_copy(struct ao_scheme_string *a);

struct ao_scheme_string *
ao_scheme_string_make(char *a);

struct ao_scheme_string *
ao_scheme_atom_to_string(struct ao_scheme_atom *a);

struct ao_scheme_string *
ao_scheme_string_cat(struct ao_scheme_string *a, struct ao_scheme_string *b);

ao_poly
ao_scheme_string_pack(struct ao_scheme_cons *cons);

ao_poly
ao_scheme_string_unpack(struct ao_scheme_string *a);

void
ao_scheme_string_write(ao_poly s, bool write);

/* atom */
extern const struct ao_scheme_type ao_scheme_atom_type;

extern struct ao_scheme_atom	*ao_scheme_atoms;
extern struct ao_scheme_frame	*ao_scheme_frame_global;
extern struct ao_scheme_frame	*ao_scheme_frame_current;

void
ao_scheme_atom_write(ao_poly a, bool write);

struct ao_scheme_atom *
ao_scheme_string_to_atom(struct ao_scheme_string *string);

struct ao_scheme_atom *
ao_scheme_atom_intern(char *name);

ao_poly *
ao_scheme_atom_ref(ao_poly atom, struct ao_scheme_frame **frame_ref);

ao_poly
ao_scheme_atom_get(ao_poly atom);

ao_poly
ao_scheme_atom_set(ao_poly atom, ao_poly val);

ao_poly
ao_scheme_atom_def(ao_poly atom, ao_poly val);

/* int */
void
ao_scheme_int_write(ao_poly i, bool write);

#ifdef AO_SCHEME_FEATURE_BIGINT
int32_t
ao_scheme_poly_integer(ao_poly p, bool *fail);

ao_poly
ao_scheme_integer_poly(int32_t i);

static inline int
ao_scheme_integer_typep(uint8_t t)
{
	return (t == AO_SCHEME_INT) || (t == AO_SCHEME_BIGINT);
}

void
ao_scheme_bigint_write(ao_poly i, bool write);

extern const struct ao_scheme_type	ao_scheme_bigint_type;

#else

#define ao_scheme_poly_integer(a,b) ao_scheme_poly_int(a)
#define ao_scheme_integer_poly ao_scheme_int_poly

static inline int
ao_scheme_integer_typep(uint8_t t)
{
	return (t == AO_SCHEME_INT);
}

#endif /* AO_SCHEME_FEATURE_BIGINT */

/* vector */

void
ao_scheme_vector_write(ao_poly v, bool write);

struct ao_scheme_vector *
ao_scheme_vector_alloc(uint16_t length, ao_poly fill);

ao_poly
ao_scheme_vector_get(ao_poly v, ao_poly i);

ao_poly
ao_scheme_vector_set(ao_poly v, ao_poly i, ao_poly p);

struct ao_scheme_vector *
ao_scheme_list_to_vector(struct ao_scheme_cons *cons);

struct ao_scheme_cons *
ao_scheme_vector_to_list(struct ao_scheme_vector *vector);

extern const struct ao_scheme_type	ao_scheme_vector_type;

/* prim */
void (*ao_scheme_poly_write_func(ao_poly p))(ao_poly p, bool write);

static inline void
ao_scheme_poly_write(ao_poly p, bool write) { (*ao_scheme_poly_write_func(p))(p, write); }

int
ao_scheme_poly_mark(ao_poly p, uint8_t note_cons);

/* returns 1 if the object has already been moved */
int
ao_scheme_poly_move(ao_poly *p, uint8_t note_cons);

/* eval */

void
ao_scheme_eval_clear_globals(void);

int
ao_scheme_eval_restart(void);

ao_poly
ao_scheme_eval(ao_poly p);

ao_poly
ao_scheme_set_cond(struct ao_scheme_cons *cons);

/* float */
#ifdef AO_SCHEME_FEATURE_FLOAT
extern const struct ao_scheme_type ao_scheme_float_type;

void
ao_scheme_float_write(ao_poly p, bool write);

ao_poly
ao_scheme_float_get(float value);
#endif

#ifdef AO_SCHEME_FEATURE_FLOAT
static inline uint8_t
ao_scheme_number_typep(uint8_t t)
{
	return ao_scheme_integer_typep(t) || (t == AO_SCHEME_FLOAT);
}
#else
#define ao_scheme_number_typep ao_scheme_integer_typep
#endif

/* builtin */
void
ao_scheme_builtin_write(ao_poly b, bool write);

extern const struct ao_scheme_type ao_scheme_builtin_type;

/* Check argument count */
ao_poly
ao_scheme_check_argc(ao_poly name, struct ao_scheme_cons *cons, int min, int max);

/* Check argument type */
ao_poly
ao_scheme_check_argt(ao_poly name, struct ao_scheme_cons *cons, int argc, int type, int nil_ok);

/* Fetch an arg (nil if off the end) */
ao_poly
ao_scheme_arg(struct ao_scheme_cons *cons, int argc);

char *
ao_scheme_args_name(uint8_t args);

/* read */
extern int			ao_scheme_read_list;
extern struct ao_scheme_cons	*ao_scheme_read_cons;
extern struct ao_scheme_cons	*ao_scheme_read_cons_tail;
extern struct ao_scheme_cons	*ao_scheme_read_stack;

ao_poly
ao_scheme_read(void);

/* rep */
ao_poly
ao_scheme_read_eval_print(void);

/* frame */
extern const struct ao_scheme_type ao_scheme_frame_type;
extern const struct ao_scheme_type ao_scheme_frame_vals_type;

#define AO_SCHEME_FRAME_FREE	6

extern struct ao_scheme_frame	*ao_scheme_frame_free_list[AO_SCHEME_FRAME_FREE];

ao_poly
ao_scheme_frame_mark(struct ao_scheme_frame *frame);

ao_poly *
ao_scheme_frame_ref(struct ao_scheme_frame *frame, ao_poly atom);

struct ao_scheme_frame *
ao_scheme_frame_new(int num);

void
ao_scheme_frame_free(struct ao_scheme_frame *frame);

void
ao_scheme_frame_bind(struct ao_scheme_frame *frame, int num, ao_poly atom, ao_poly val);

ao_poly
ao_scheme_frame_add(struct ao_scheme_frame *frame, ao_poly atom, ao_poly val);

void
ao_scheme_frame_write(ao_poly p, bool write);

void
ao_scheme_frame_init(void);

/* lambda */
extern const struct ao_scheme_type ao_scheme_lambda_type;

extern const char * const ao_scheme_state_names[];

struct ao_scheme_lambda *
ao_scheme_lambda_new(ao_poly cons);

void
ao_scheme_lambda_write(ao_poly lambda, bool write);

ao_poly
ao_scheme_lambda_eval(void);

/* stack */

extern const struct ao_scheme_type ao_scheme_stack_type;
extern struct ao_scheme_stack	*ao_scheme_stack;
extern struct ao_scheme_stack	*ao_scheme_stack_free_list;

extern int			ao_scheme_frame_print_indent;

void
ao_scheme_stack_reset(struct ao_scheme_stack *stack);

int
ao_scheme_stack_push(void);

void
ao_scheme_stack_pop(void);

void
ao_scheme_stack_clear(void);

void
ao_scheme_stack_write(ao_poly stack, bool write);

ao_poly
ao_scheme_stack_eval(void);

/* error */

void
ao_scheme_vprintf(const char *format, va_list args);

void
ao_scheme_printf(const char *format, ...);

ao_poly
ao_scheme_error(int error, const char *format, ...);

/* builtins */

#define AO_SCHEME_BUILTIN_DECLS
#include "ao_scheme_builtin.h"

/* debugging macros */

#if DBG_EVAL || DBG_READ
int ao_scheme_stack_depth;
#endif

#if DBG_EVAL
#define DBG_DO(a)	a
#define DBG_INDENT()	do { int _s; for(_s = 0; _s < ao_scheme_stack_depth; _s++) printf("  "); } while(0)
#define DBG_IN()	(++ao_scheme_stack_depth)
#define DBG_OUT()	(--ao_scheme_stack_depth)
#define DBG_RESET()	(ao_scheme_stack_depth = 0)
#define DBG(...) 	ao_scheme_printf(__VA_ARGS__)
#define DBGI(...)	do { printf("%4d: ", __LINE__); DBG_INDENT(); DBG(__VA_ARGS__); } while (0)
#define DBG_CONS(a)	ao_scheme_cons_write(ao_scheme_cons_poly(a), true)
#define DBG_POLY(a)	ao_scheme_poly_write(a, true)
#define OFFSET(a)	((a) ? (int) ((uint8_t *) a - ao_scheme_pool) : -1)
#define DBG_STACK()	ao_scheme_stack_write(ao_scheme_stack_poly(ao_scheme_stack), true)
static inline void
ao_scheme_frames_dump(void)
{
	struct ao_scheme_stack *s;
	DBGI(".. current frame: "); DBG_POLY(ao_scheme_frame_poly(ao_scheme_frame_current)); DBG("\n");
	for (s = ao_scheme_stack; s; s = ao_scheme_poly_stack(s->prev)) {
		DBGI(".. stack frame: "); DBG_POLY(s->frame); DBG("\n");
	}
}
#define DBG_FRAMES()	ao_scheme_frames_dump()
#else
#define DBG_DO(a)
#define DBG_INDENT()
#define DBG_IN()
#define DBG_OUT()
#define DBG(...)
#define DBGI(...)
#define DBG_CONS(a)
#define DBG_POLY(a)
#define DBG_RESET()
#define DBG_STACK()
#define DBG_FRAMES()
#endif

#if DBG_READ
#define RDBGI(...)	do { printf("%4d: ", __LINE__); DBG_INDENT(); ao_scheme_printf(__VA_ARGS__); } while (0)
#define RDBG_IN()	(++ao_scheme_stack_depth)
#define RDBG_OUT()	(--ao_scheme_stack_depth)
#else
#define RDBGI(...)
#define RDBG_IN()
#define RDBG_OUT()
#endif

static inline int
ao_scheme_mdbg_offset(void *a)
{
	uint8_t		*u = a;

	if (u == 0)
		return -1;

	if (ao_scheme_pool <= u && u < ao_scheme_pool + AO_SCHEME_POOL)
		return u - ao_scheme_pool;

#ifndef AO_SCHEME_MAKE_CONST
	if (ao_scheme_const <= u && u < ao_scheme_const + AO_SCHEME_POOL_CONST)
		return - (int) (u - ao_scheme_const);
#endif
	return -2;
}

#define MDBG_OFFSET(a)	ao_scheme_mdbg_offset(a)

#if DBG_MEM

#define DBG_MEM_START	1

#include <assert.h>
extern int dbg_move_depth;
#define MDBG_DUMP 1

extern int dbg_mem;

#define MDBG_DO(a)	a
#define MDBG_MOVE(...) do { if (dbg_mem) { int d; for (d = 0; d < dbg_move_depth; d++) printf ("  "); printf(__VA_ARGS__); } } while (0)
#define MDBG_MORE(...) do { if (dbg_mem) printf(__VA_ARGS__); } while (0)
#define MDBG_MOVE_IN()	(dbg_move_depth++)
#define MDBG_MOVE_OUT()	(assert(--dbg_move_depth >= 0))

#else

#define MDBG_DO(a)
#define MDBG_MOVE(...)
#define MDBG_MORE(...)
#define MDBG_MOVE_IN()
#define MDBG_MOVE_OUT()

#endif

#endif /* _AO_SCHEME_H_ */
