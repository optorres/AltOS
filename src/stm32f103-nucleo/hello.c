/*
 * Copyright © 2023 Keith Packard <keithp@keithp.com>
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
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include <ao.h>

void
ao_delay(AO_TICK_TYPE ticks)
{
	uint32_t then = ao_time();
	while ((int32_t) (ao_time() - then) < (int32_t) ticks)
		ao_arch_nop();
}

int main(void)
{
	ao_clock_init();
	ao_timer_init();
	ao_led_init();
	for (;;) {
		ao_led_for(AO_LED_GREEN, 50);
		ao_delay(50);
	}
}
