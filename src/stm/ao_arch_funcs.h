/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
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

#ifndef _AO_ARCH_FUNCS_H_
#define _AO_ARCH_FUNCS_H_

extern uint8_t	ao_spi_mutex[STM_NUM_SPI];

static inline void ao_spi_get(uint8_t spi_index) { ao_mutex_get(&ao_spi_mutex[spi_index]); }
static inline void ao_spi_put(uint8_t spi_index) { ao_mutex_put(&ao_spi_mutex[spi_index]); }

void
ao_spi_send(void *block, uint16_t len, uint8_t spi_index);

void
ao_spi_recv(void *block, uint16_t len, uint8_t spi_index);

void
ao_spi_init(uint8_t spi_index);

#endif /* _AO_ARCH_FUNCS_H_ */
