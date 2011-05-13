/*
 * Copyright © 2011 Keith Packard <keithp@keithp.com>
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

#include "ao-bringup.h"

#define AO_STACK_SIZE	128

uint8_t	new_stack[512];
int	stack_count;

#define PUSH8(stack, val)	(*((stack)--) = (val))

#define PUSH16(stack, val)	PUSH8(stack, ((uint16_t) (val))); PUSH8(stack, ((uint16_t) (val)) >> 8)

void
count(int count)
{

}

void
blink(void)
{
	while (1) {
		LEDPORT ^= (1 << LEDOUT);
		_delay_ms(200);
	}
}

void
function(void)
{
	return;
}

void
init_stack(void (*f) (void))
{
	uint8_t		*stack = new_stack + AO_STACK_SIZE - 1;
	uint16_t	a;
	uint8_t		h, l;

	/* Return address */
	a = (uint16_t) f;
	l = a;
	h = a >> 8;
	PUSH8(stack, l);
	PUSH8(stack, h);

	/* Clear register values */
	l = 32;
	while (l--)
		PUSH8(stack, 0);

	/* SREG with interrupts enabled */
	PUSH8(stack, 0x80);
	stack_count = stack - new_stack;
	printf("Function is at %p. Stack[1] is %02x Stack[2] is %02x\n",
	       f, stack[1], stack[2]);
	printf ("stack_count is %d\n", stack_count);
	printf ("stack is %p\n", stack);
}

void
switch_stack(void) __attribute__((naked));

void show_stack(char *s, uint8_t h, uint8_t l)
{
	printf ("SP at %s %02x %02x\n", s, h, l);
}

void
switch_stack(void)
{
	uint8_t	*sp = (new_stack + stack_count);
	uint8_t		l, h;

//	for (l = 0; l < AO_STACK_SIZE - stack_count; l++)
//		printf ("stack[%2d] = %2x\n", l, sp[l]);
	l = new_stack[AO_STACK_SIZE - 1];
	h = new_stack[AO_STACK_SIZE - 2];
//	printf ("Target return address: h %02x l %02x\n", h, l);
#if 0
	asm("push %0" : : "r" (l));
	asm("push %0" : : "r" (h));
	asm("ret");
#endif
#if 0
	asm("push r31; push r30");
	asm("push r29; push r28; push r27; push r26; push r25");
	asm("push r24; push r23; push r22; push r21; push r20");
	asm("push r19; push r18; push r17; push r16; push r15");
	asm("push r14; push r13; push r12; push r11; push r10");
	asm("push r9; push r8; push r7; push r6; push r5");
	asm("push r4; push r3; push r2; push r1; push r0");
	asm("in r0, __SREG__" "\n\t"
	    "push r0");
#endif

	uint8_t	sp_l, sp_h;
	static	uint8_t reg[34];
	sp_l = (uint16_t) sp;
	sp_h = ((uint16_t) sp) >> 8;
	asm volatile ("out __SP_H__,%0" : : "r" (sp_h) );
	asm volatile ("out __SP_L__,%0" : : "r" (sp_l) );

	asm volatile ("in %0,__SP_H__" : "=&r" (sp_h));
	asm volatile ("in %0,__SP_L__" : "=&r" (sp_l));
	asm volatile("pop r0"	"\n\t"
		     "out __SREG__, r0");
	asm volatile("pop r0" "\n\t"
		     "pop r1" "\n\t"
		     "pop r2" "\n\t"
		     "pop r3" "\n\t"
		     "pop r4");
	asm volatile("pop r5" "\n\t"
		     "pop r6" "\n\t"
		     "pop r7" "\n\t"
		     "pop r8" "\n\t"
		     "pop r9");
	asm volatile("pop r10" "\n\t"
		     "pop r11" "\n\t"
		     "pop r12" "\n\t"
		     "pop r13" "\n\t"
		     "pop r14");
	asm volatile("pop r15" "\n\t"
		     "pop r16" "\n\t"
		     "pop r17" "\n\t"
		     "pop r18" "\n\t"
		     "pop r19");
	asm volatile("pop r20" "\n\t"
		     "pop r21" "\n\t"
		     "pop r22" "\n\t"
		     "pop r23" "\n\t"
		     "pop r24");
	asm volatile("pop r25" "\n\t"
		     "pop r26" "\n\t"
		     "pop r27" "\n\t"
		     "pop r28" "\n\t"
		     "pop r29");
	asm volatile("pop r30" "\n\t"
		     "pop r31");
	asm volatile ("in %0,__SP_H__" : "=&r" (sp_h));
	asm volatile ("in %0,__SP_L__" : "=&r" (sp_l));
	asm volatile ("pop %0" : "=&r" (h));
	asm volatile ("pop %0" : "=&r" (l));
	show_stack("before ret", sp_h, sp_l);
	show_stack("returning to", h, l);
	asm volatile("push %0" : : "r" (l));
	asm volatile("push %0" : : "r" (h));
	asm volatile("ret");
}

void back(void)
{
	switch_stack();
	blink();
}

void main(void)
{
	ao_bringup_init();

	printf("starting\n");

	init_stack(blink);
	switch_stack();
}
