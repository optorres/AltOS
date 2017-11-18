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
#include <assert.h>

struct ao_lisp_stack		*ao_lisp_stack;
ao_poly				ao_lisp_v;
uint8_t				ao_lisp_skip_cons_free;

ao_poly
ao_lisp_set_cond(struct ao_lisp_cons *c)
{
	ao_lisp_stack->state = eval_cond;
	ao_lisp_stack->sexprs = ao_lisp_cons_poly(c);
	return AO_LISP_NIL;
}

static int
func_type(ao_poly func)
{
	if (func == AO_LISP_NIL)
		return ao_lisp_error(AO_LISP_INVALID, "func is nil");
	switch (ao_lisp_poly_type(func)) {
	case AO_LISP_BUILTIN:
		return ao_lisp_poly_builtin(func)->args & AO_LISP_FUNC_MASK;
	case AO_LISP_LAMBDA:
		return ao_lisp_poly_lambda(func)->args;
	case AO_LISP_STACK:
		return AO_LISP_FUNC_LAMBDA;
	default:
		ao_lisp_error(AO_LISP_INVALID, "not a func");
		return -1;
	}
}

/*
 * Flattened eval to avoid stack issues
 */

/*
 * Evaluate an s-expression
 *
 * For a list, evaluate all of the elements and
 * then execute the resulting function call.
 *
 * Each element of the list is evaluated in
 * a clean stack context.
 *
 * The current stack state is set to 'formal' so that
 * when the evaluation is complete, the value
 * will get appended to the values list.
 *
 * For other types, compute the value directly.
 */

static int
ao_lisp_eval_sexpr(void)
{
	DBGI("sexpr: "); DBG_POLY(ao_lisp_v); DBG("\n");
	switch (ao_lisp_poly_type(ao_lisp_v)) {
	case AO_LISP_CONS:
		if (ao_lisp_v == AO_LISP_NIL) {
			if (!ao_lisp_stack->values) {
				/*
				 * empty list evaluates to empty list
				 */
				ao_lisp_v = AO_LISP_NIL;
				ao_lisp_stack->state = eval_val;
			} else {
				/*
				 * done with arguments, go execute it
				 */
				ao_lisp_v = ao_lisp_poly_cons(ao_lisp_stack->values)->car;
				ao_lisp_stack->state = eval_exec;
			}
		} else {
			if (!ao_lisp_stack->values)
				ao_lisp_stack->list = ao_lisp_v;
			/*
			 * Evaluate another argument and then switch
			 * to 'formal' to add the value to the values
			 * list
			 */
			ao_lisp_stack->sexprs = ao_lisp_v;
			ao_lisp_stack->state = eval_formal;
			if (!ao_lisp_stack_push())
				return 0;
			/*
			 * push will reset the state to 'sexpr', which
			 * will evaluate the expression
			 */
			ao_lisp_v = ao_lisp_poly_cons(ao_lisp_v)->car;
		}
		break;
	case AO_LISP_ATOM:
		DBGI("..frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
		ao_lisp_v = ao_lisp_atom_get(ao_lisp_v);
		/* fall through */
	case AO_LISP_BOOL:
	case AO_LISP_INT:
	case AO_LISP_STRING:
	case AO_LISP_BUILTIN:
	case AO_LISP_LAMBDA:
		ao_lisp_stack->state = eval_val;
		break;
	}
	DBGI(".. result "); DBG_POLY(ao_lisp_v); DBG("\n");
	return 1;
}

/*
 * A value has been computed.
 *
 * If the value was computed from a macro,
 * then we want to reset the current context
 * to evaluate the macro result again.
 *
 * If not a macro, then pop the stack.
 * If the stack is empty, we're done.
 * Otherwise, the stack will contain
 * the next state.
 */

static int
ao_lisp_eval_val(void)
{
	DBGI("val: "); DBG_POLY(ao_lisp_v); DBG("\n");
	/*
	 * Value computed, pop the stack
	 * to figure out what to do with the value
	 */
	ao_lisp_stack_pop();
	DBGI("..state %d\n", ao_lisp_stack ? ao_lisp_stack->state : -1);
	return 1;
}

/*
 * A formal has been computed.
 *
 * If this is the first formal, then check to see if we've got a
 * lamda/lexpr or macro/nlambda.
 *
 * For lambda/lexpr, go compute another formal.  This will terminate
 * when the sexpr state sees nil.
 *
 * For macro/nlambda, we're done, so move the sexprs into the values
 * and go execute it.
 *
 * Macros have an additional step of saving a stack frame holding the
 * macro value execution context, which then gets the result of the
 * macro to run
 */

static int
ao_lisp_eval_formal(void)
{
	ao_poly			formal;
	struct ao_lisp_stack	*prev;

	DBGI("formal: "); DBG_POLY(ao_lisp_v); DBG("\n");

	/* Check what kind of function we've got */
	if (!ao_lisp_stack->values) {
		switch (func_type(ao_lisp_v)) {
		case AO_LISP_FUNC_LAMBDA:
		case AO_LISP_FUNC_LEXPR:
			DBGI(".. lambda or lexpr\n");
			break;
		case AO_LISP_FUNC_MACRO:
			/* Evaluate the result once more */
			ao_lisp_stack->state = eval_macro;
			if (!ao_lisp_stack_push())
				return 0;

			/* After the function returns, take that
			 * value and re-evaluate it
			 */
			prev = ao_lisp_poly_stack(ao_lisp_stack->prev);
			ao_lisp_stack->sexprs = prev->sexprs;

			DBGI(".. start macro\n");
			DBGI(".. sexprs       "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
			DBGI(".. values       "); DBG_POLY(ao_lisp_stack->values); DBG("\n");
			DBG_FRAMES();

			/* fall through ... */
		case AO_LISP_FUNC_NLAMBDA:
			DBGI(".. nlambda or macro\n");

			/* use the raw sexprs as values */
			ao_lisp_stack->values = ao_lisp_stack->sexprs;
			ao_lisp_stack->values_tail = AO_LISP_NIL;
			ao_lisp_stack->state = eval_exec;

			/* ready to execute now */
			return 1;
		case -1:
			return 0;
		}
	}

	/* Append formal to list of values */
	formal = ao_lisp__cons(ao_lisp_v, AO_LISP_NIL);
	if (!formal)
		return 0;

	if (ao_lisp_stack->values_tail)
		ao_lisp_poly_cons(ao_lisp_stack->values_tail)->cdr = formal;
	else
		ao_lisp_stack->values = formal;
	ao_lisp_stack->values_tail = formal;

	DBGI(".. values "); DBG_POLY(ao_lisp_stack->values); DBG("\n");

	/*
	 * Step to the next argument, if this is last, then
	 * 'sexpr' will end up switching to 'exec'
	 */
	ao_lisp_v = ao_lisp_poly_cons(ao_lisp_stack->sexprs)->cdr;

	ao_lisp_stack->state = eval_sexpr;

	DBGI(".. "); DBG_POLY(ao_lisp_v); DBG("\n");
	return 1;
}

/*
 * Start executing a function call
 *
 * Most builtins are easy, just call the function.
 * 'cond' is magic; it sticks the list of clauses
 * in 'sexprs' and switches to 'cond' state. That
 * bit of magic is done in ao_lisp_set_cond.
 *
 * Lambdas build a new frame to hold the locals and
 * then re-use the current stack context to evaluate
 * the s-expression from the lambda.
 */

static int
ao_lisp_eval_exec(void)
{
	ao_poly v;
	struct ao_lisp_builtin	*builtin;

	DBGI("exec: "); DBG_POLY(ao_lisp_v); DBG(" values "); DBG_POLY(ao_lisp_stack->values); DBG ("\n");
	ao_lisp_stack->sexprs = AO_LISP_NIL;
	switch (ao_lisp_poly_type(ao_lisp_v)) {
	case AO_LISP_BUILTIN:
		ao_lisp_stack->state = eval_val;
		builtin = ao_lisp_poly_builtin(ao_lisp_v);
		v = ao_lisp_func(builtin) (
			ao_lisp_poly_cons(ao_lisp_poly_cons(ao_lisp_stack->values)->cdr));
		DBG_DO(if (!ao_lisp_exception && ao_lisp_poly_builtin(ao_lisp_v)->func == builtin_set) {
				struct ao_lisp_cons *cons = ao_lisp_poly_cons(ao_lisp_stack->values);
				ao_poly atom = ao_lisp_arg(cons, 1);
				ao_poly val = ao_lisp_arg(cons, 2);
				DBGI("set "); DBG_POLY(atom); DBG(" = "); DBG_POLY(val); DBG("\n");
			});
		builtin = ao_lisp_poly_builtin(ao_lisp_v);
		if (builtin && builtin->args & AO_LISP_FUNC_FREE_ARGS && !ao_lisp_stack_marked(ao_lisp_stack) && !ao_lisp_skip_cons_free)
			ao_lisp_cons_free(ao_lisp_poly_cons(ao_lisp_stack->values));

		ao_lisp_v = v;
		ao_lisp_stack->values = AO_LISP_NIL;
		ao_lisp_stack->values_tail = AO_LISP_NIL;
		DBGI(".. result "); DBG_POLY(ao_lisp_v); DBG ("\n");
		DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
		break;
	case AO_LISP_LAMBDA:
		DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
		ao_lisp_stack->state = eval_progn;
		v = ao_lisp_lambda_eval();
		ao_lisp_stack->sexprs = v;
		ao_lisp_stack->values = AO_LISP_NIL;
		ao_lisp_stack->values_tail = AO_LISP_NIL;
		DBGI(".. sexprs "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
		DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
		break;
	case AO_LISP_STACK:
		DBGI(".. stack "); DBG_POLY(ao_lisp_v); DBG("\n");
		ao_lisp_v = ao_lisp_stack_eval();
		DBGI(".. value "); DBG_POLY(ao_lisp_v); DBG("\n");
		DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
		break;
	}
	ao_lisp_skip_cons_free = 0;
	return 1;
}

/*
 * Finish setting up the apply evaluation
 *
 * The value is the list to execute
 */
static int
ao_lisp_eval_apply(void)
{
	struct ao_lisp_cons	*cons = ao_lisp_poly_cons(ao_lisp_v);
	struct ao_lisp_cons	*cdr, *prev;

	/* Glue the arguments into the right shape. That's all but the last
	 * concatenated onto the last
	 */
	cdr = cons;
	for (;;) {
		prev = cdr;
		cdr = ao_lisp_poly_cons(prev->cdr);
		if (cdr->cdr == AO_LISP_NIL)
			break;
	}
	DBGI("before mangling: "); DBG_POLY(ao_lisp_v); DBG("\n");
	prev->cdr = cdr->car;
	ao_lisp_stack->values = ao_lisp_v;
	ao_lisp_v = ao_lisp_poly_cons(ao_lisp_stack->values)->car;
	DBGI("apply: "); DBG_POLY(ao_lisp_stack->values); DBG ("\n");
	ao_lisp_stack->state = eval_exec;
	ao_lisp_skip_cons_free = 1;
	return 1;
}

/*
 * Start evaluating the next cond clause
 *
 * If the list of clauses is empty, then
 * the result of the cond is nil.
 *
 * Otherwise, set the current stack state to 'cond_test' and create a
 * new stack context to evaluate the test s-expression. Once that's
 * complete, we'll land in 'cond_test' to finish the clause.
 */
static int
ao_lisp_eval_cond(void)
{
	DBGI("cond: "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
	DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
	DBGI(".. saved frame "); DBG_POLY(ao_lisp_stack->frame); DBG("\n");
	if (!ao_lisp_stack->sexprs) {
		ao_lisp_v = AO_LISP_NIL;
		ao_lisp_stack->state = eval_val;
	} else {
		ao_lisp_v = ao_lisp_poly_cons(ao_lisp_stack->sexprs)->car;
		if (!ao_lisp_v || ao_lisp_poly_type(ao_lisp_v) != AO_LISP_CONS) {
			ao_lisp_error(AO_LISP_INVALID, "invalid cond clause");
			return 0;
		}
		ao_lisp_v = ao_lisp_poly_cons(ao_lisp_v)->car;
		if (ao_lisp_v == _ao_lisp_atom_else)
			ao_lisp_v = _ao_lisp_bool_true;
		ao_lisp_stack->state = eval_cond_test;
		if (!ao_lisp_stack_push())
			return 0;
	}
	return 1;
}

/*
 * Finish a cond clause.
 *
 * Check the value from the test expression, if
 * non-nil, then set up to evaluate the value expression.
 *
 * Otherwise, step to the next clause and go back to the 'cond'
 * state
 */
static int
ao_lisp_eval_cond_test(void)
{
	DBGI("cond_test: "); DBG_POLY(ao_lisp_v); DBG(" sexprs "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
	DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
	DBGI(".. saved frame "); DBG_POLY(ao_lisp_stack->frame); DBG("\n");
	if (ao_lisp_v != _ao_lisp_bool_false) {
		struct ao_lisp_cons *car = ao_lisp_poly_cons(ao_lisp_poly_cons(ao_lisp_stack->sexprs)->car);
		ao_poly c = car->cdr;

		if (c) {
			ao_lisp_stack->state = eval_progn;
			ao_lisp_stack->sexprs = c;
		} else
			ao_lisp_stack->state = eval_val;
	} else {
		ao_lisp_stack->sexprs = ao_lisp_poly_cons(ao_lisp_stack->sexprs)->cdr;
		DBGI("next cond: "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
		ao_lisp_stack->state = eval_cond;
	}
	return 1;
}

/*
 * Evaluate a list of sexprs, returning the value from the last one.
 *
 * ao_lisp_progn records the list in stack->sexprs, so we just need to
 * walk that list. Set ao_lisp_v to the car of the list and jump to
 * eval_sexpr. When that's done, it will land in eval_val. For all but
 * the last, leave a stack frame with eval_progn set so that we come
 * back here. For the last, don't add a stack frame so that we can
 * just continue on.
 */
static int
ao_lisp_eval_progn(void)
{
	DBGI("progn: "); DBG_POLY(ao_lisp_v); DBG(" sexprs "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
	DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
	DBGI(".. saved frame "); DBG_POLY(ao_lisp_stack->frame); DBG("\n");

	if (!ao_lisp_stack->sexprs) {
		ao_lisp_v = AO_LISP_NIL;
		ao_lisp_stack->state = eval_val;
	} else {
		ao_lisp_v = ao_lisp_poly_cons(ao_lisp_stack->sexprs)->car;
		ao_lisp_stack->sexprs = ao_lisp_poly_cons(ao_lisp_stack->sexprs)->cdr;

		/* If there are more sexprs to do, then come back here, otherwise
		 * return the value of the last one by just landing in eval_sexpr
		 */
		if (ao_lisp_stack->sexprs) {
			ao_lisp_stack->state = eval_progn;
			if (!ao_lisp_stack_push())
				return 0;
		}
		ao_lisp_stack->state = eval_sexpr;
	}
	return 1;
}

/*
 * Conditionally execute a list of sexprs while the first is true
 */
static int
ao_lisp_eval_while(void)
{
	DBGI("while: "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
	DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
	DBGI(".. saved frame "); DBG_POLY(ao_lisp_stack->frame); DBG("\n");

	ao_lisp_stack->values = ao_lisp_v;
	if (!ao_lisp_stack->sexprs) {
		ao_lisp_v = AO_LISP_NIL;
		ao_lisp_stack->state = eval_val;
	} else {
		ao_lisp_v = ao_lisp_poly_cons(ao_lisp_stack->sexprs)->car;
		ao_lisp_stack->state = eval_while_test;
		if (!ao_lisp_stack_push())
			return 0;
	}
	return 1;
}

/*
 * Check the while condition, terminate the loop if nil. Otherwise keep going
 */
static int
ao_lisp_eval_while_test(void)
{
	DBGI("while_test: "); DBG_POLY(ao_lisp_v); DBG(" sexprs "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");
	DBGI(".. frame "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");
	DBGI(".. saved frame "); DBG_POLY(ao_lisp_stack->frame); DBG("\n");

	if (ao_lisp_v != _ao_lisp_bool_false) {
		ao_lisp_stack->values = ao_lisp_v;
		ao_lisp_v = ao_lisp_poly_cons(ao_lisp_stack->sexprs)->cdr;
		ao_lisp_stack->state = eval_while;
		if (!ao_lisp_stack_push())
			return 0;
		ao_lisp_stack->state = eval_progn;
		ao_lisp_stack->sexprs = ao_lisp_v;
	}
	else
	{
		ao_lisp_stack->state = eval_val;
		ao_lisp_v = ao_lisp_stack->values;
	}
	return 1;
}

/*
 * Replace the original sexpr with the macro expansion, then
 * execute that
 */
static int
ao_lisp_eval_macro(void)
{
	DBGI("macro: "); DBG_POLY(ao_lisp_v); DBG(" sexprs "); DBG_POLY(ao_lisp_stack->sexprs); DBG("\n");

	if (ao_lisp_v == AO_LISP_NIL)
		ao_lisp_abort();
	if (ao_lisp_poly_type(ao_lisp_v) == AO_LISP_CONS) {
		*ao_lisp_poly_cons(ao_lisp_stack->sexprs) = *ao_lisp_poly_cons(ao_lisp_v);
		ao_lisp_v = ao_lisp_stack->sexprs;
		DBGI("sexprs rewritten to: "); DBG_POLY(ao_lisp_v); DBG("\n");
	}
	ao_lisp_stack->sexprs = AO_LISP_NIL;
	ao_lisp_stack->state = eval_sexpr;
	return 1;
}

static int (*const evals[])(void) = {
	[eval_sexpr] = ao_lisp_eval_sexpr,
	[eval_val] = ao_lisp_eval_val,
	[eval_formal] = ao_lisp_eval_formal,
	[eval_exec] = ao_lisp_eval_exec,
	[eval_apply] = ao_lisp_eval_apply,
	[eval_cond] = ao_lisp_eval_cond,
	[eval_cond_test] = ao_lisp_eval_cond_test,
	[eval_progn] = ao_lisp_eval_progn,
	[eval_while] = ao_lisp_eval_while,
	[eval_while_test] = ao_lisp_eval_while_test,
	[eval_macro] = ao_lisp_eval_macro,
};

const char *ao_lisp_state_names[] = {
	[eval_sexpr] = "sexpr",
	[eval_val] = "val",
	[eval_formal] = "formal",
	[eval_exec] = "exec",
	[eval_apply] = "apply",
	[eval_cond] = "cond",
	[eval_cond_test] = "cond_test",
	[eval_progn] = "progn",
	[eval_while] = "while",
	[eval_while_test] = "while_test",
	[eval_macro] = "macro",
};

/*
 * Called at restore time to reset all execution state
 */

void
ao_lisp_eval_clear_globals(void)
{
	ao_lisp_stack = NULL;
	ao_lisp_frame_current = NULL;
	ao_lisp_v = AO_LISP_NIL;
}

int
ao_lisp_eval_restart(void)
{
	return ao_lisp_stack_push();
}

ao_poly
ao_lisp_eval(ao_poly _v)
{
	ao_lisp_v = _v;

	if (!ao_lisp_stack_push())
		return AO_LISP_NIL;

	while (ao_lisp_stack) {
		if (!(*evals[ao_lisp_stack->state])() || ao_lisp_exception) {
			ao_lisp_stack_clear();
			return AO_LISP_NIL;
		}
	}
	DBG_DO(if (ao_lisp_frame_current) {DBGI("frame left as "); DBG_POLY(ao_lisp_frame_poly(ao_lisp_frame_current)); DBG("\n");});
	ao_lisp_frame_current = NULL;
	return ao_lisp_v;
}
