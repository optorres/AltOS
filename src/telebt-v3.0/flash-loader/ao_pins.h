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

#ifndef _AO_PINS_H_
#define _AO_PINS_H_

/* External crystal at 8MHz */
#define AO_HSE		8000000

#include <ao_flash_stm_pins.h>

/* Blue LED */

#define AO_BOOT_PIN			1
#define AO_BOOT_APPLICATION_GPIO	stm_gpioc
#define AO_BOOT_APPLICATION_PIN		15
#define AO_BOOT_APPLICATION_VALUE	0
#define AO_BOOT_APPLICATION_MODE	AO_EXTI_MODE_PULL_DOWN

#endif /* _AO_PINS_H_ */