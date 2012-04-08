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

#ifndef _AO_PINS_H_
#define _AO_PINS_H_

#define HAS_SERIAL_1		1
#define USE_SERIAL_1_STDIN	1
#define SERIAL_1_PB6_PB7	1
#define SERIAL_1_PA9_PA10	0

#define HAS_SERIAL_2		0
#define USE_SERIAL_2_STDIN	1
#define SERIAL_2_PA2_PA3	0
#define SERIAL_2_PD5_PD6	1

#define HAS_SERIAL_3		0
#define USE_SERIAL_3_STDIN	1
#define SERIAL_3_PB10_PB11	0
#define SERIAL_3_PC10_PC11	0
#define SERIAL_3_PD8_PD9	1

#define HAS_SPI_1		1
#define SPI_1_PB3_PB4_PB5	1

#define HAS_SPI_2		0

#define HAS_USB			0
#define HAS_BEEP		0
#define PACKET_HAS_SLAVE	0

#define LOW_LEVEL_DEBUG		1

#define LED_PORT_ENABLE		STM_RCC_AHBENR_GPIOBEN
#define LED_PORT		stm_gpiob
#define LED_PIN_GREEN		7
#define LED_PIN_BLUE		6
#define AO_LED_GREEN		(1 << LED_PIN_GREEN)
#define AO_LED_BLUE		(1 << LED_PIN_BLUE)

#define AO_LED_RED		AO_LED_BLUE	/* a patent lie */

#define LEDS_AVAILABLE		(AO_LED_BLUE | AO_LED_GREEN)

#define AO_LCD_STM_SEG_ENABLED_0 (		\
		(1 << 0) | /* PA1 */		\
		(1 << 1) | /* PA2 */		\
		(1 << 2) | /* PA3 */		\
		(0 << 3) | /* PA6 */		\
		(0 << 4) | /* PA7 */		\
		(0 << 5) | /* PB0 */		\
		(0 << 6) | /* PB1 */		\
		(1 << 7) | /* PB3 */		\
		(1 << 8) | /* PB4 */		\
		(1 << 9) | /* PB5 */		\
		(1 << 10) | /* PB10 */		\
		(1 << 11) | /* PB11 */		\
		(1 << 12) | /* PB12 */		\
		(1 << 13) | /* PB13 */		\
		(1 << 14) | /* PB14 */		\
		(1 << 15) | /* PB15 */		\
		(1 << 16) | /* PB8 */		\
		(1 << 17) | /* PA15 */		\
		(1 << 18) | /* PC0 */		\
		(1 << 19) | /* PC1 */		\
		(1 << 20) | /* PC2 */		\
		(1 << 21) | /* PC3 */		\
		(0 << 22) | /* PC4 */		\
		(0 << 23) | /* PC5 */		\
		(1 << 24) | /* PC6 */		\
		(1 << 25) | /* PC7 */		\
		(1 << 26) | /* PC8 */		\
		(1 << 27) | /* PC9 */		\
		(1 << 28) | /* PC10 or PD8 */	\
		(1 << 29) | /* PC11 or PD9 */	\
		(0 << 30) | /* PC12 or PD10 */	\
		(0 << 31))  /* PD2 or PD11 */

#define AO_LCD_STM_SEG_ENABLED_1 (		\
		(0 << 0) | /* PD12 */		\
		(0 << 1) | /* PD13 */		\
		(0 << 2) | /* PD14 */		\
		(0 << 3) | /* PD15 */		\
		(0 << 4) | /* PE0 */		\
		(0 << 5) | /* PE1 */		\
		(0 << 6) | /* PE2 */		\
		(0 << 7))  /* PE3 */

#define AO_LCD_STM_COM_ENABLED (		\
		(1 << 0) | /* PA8 */		\
		(1 << 1) | /* PA9 */		\
		(1 << 2) | /* PA10 */		\
		(1 << 3) | /* PB9 */		\
		(0 << 4) | /* PC10 */		\
		(0 << 5) | /* PC11 */		\
		(0 << 6)) /* PC12 */

#define AO_LCD_28_ON_C	1

#define HAS_ADC			1

#define AO_ADC_RING		32

struct ao_adc {
	uint16_t		tick;
	int16_t			idd;
	int16_t			temp;
};

#define AO_ADC_IDD		4
#define AO_ADC_PIN0_PORT	stm_gpioa
#define AO_ADC_PIN0_PIN		4

#define AO_ADC_RCC_AHBENR	((1 << STM_RCC_AHBENR_GPIOAEN))
#define AO_ADC_TEMP		16

#define HAS_ADC_TEMP		1

#define AO_NUM_ADC		2

#define AO_ADC_SQ1		AO_ADC_IDD
#define AO_ADC_SQ2		AO_ADC_TEMP
	

#endif /* _AO_PINS_H_ */
