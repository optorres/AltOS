/*
 * Copyright © 2010 Keith Packard <keithp@keithp.com>
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

#define HAS_RADIO		1
#define HAS_RADIO_RATE		1
#define HAS_TELEMETRY		0

#define HAS_FLIGHT		0
#define HAS_USB			1
#define HAS_BEEP		0
#define HAS_GPS			0
#define HAS_SERIAL_1		0
#define HAS_ADC			1
#define HAS_DBG			0
#define HAS_EEPROM		1
#define HAS_LOG			0
#define HAS_PAD			1
#define USE_INTERNAL_FLASH	1
#define IGNITE_ON_P0		0
#define PACKET_HAS_MASTER	0
#define PACKET_HAS_SLAVE	0
#define AO_DATA_RING		32
#define HAS_FIXED_PAD_BOX	1

/* 8MHz High speed external crystal */
#define AO_HSE			8000000

/* PLLVCO = 96MHz (so that USB will work) */
#define AO_PLLMUL		12
#define AO_RCC_CFGR_PLLMUL	(STM_RCC_CFGR_PLLMUL_12)

#define AO_CC1200_FOSC		40000000

/* SYSCLK = 32MHz (no need to go faster than CPU) */
#define AO_PLLDIV		3
#define AO_RCC_CFGR_PLLDIV	(STM_RCC_CFGR_PLLDIV_3)

/* HCLK = 32MHz (CPU clock) */
#define AO_AHB_PRESCALER	1
#define AO_RCC_CFGR_HPRE_DIV	STM_RCC_CFGR_HPRE_DIV_1

/* Run APB1 at 16MHz (HCLK/2) */
#define AO_APB1_PRESCALER	2
#define AO_RCC_CFGR_PPRE1_DIV	STM_RCC_CFGR_PPRE2_DIV_2

/* Run APB2 at 16MHz (HCLK/2) */
#define AO_APB2_PRESCALER	2
#define AO_RCC_CFGR_PPRE2_DIV	STM_RCC_CFGR_PPRE2_DIV_2

#define HAS_EEPROM		1
#define USE_INTERNAL_FLASH	1
#define USE_EEPROM_CONFIG	1
#define USE_STORAGE_CONFIG	0
#define HAS_USB			1
#define HAS_BEEP		0
#define HAS_RADIO		1
#define HAS_RADIO_RATE		1
#define HAS_TELEMETRY		0
#define HAS_AES			1

#define HAS_SPI_1		0
#define SPI_1_PA5_PA6_PA7	0
#define SPI_1_PB3_PB4_PB5	0
#define SPI_1_PE13_PE14_PE15	0

#define HAS_SPI_2		1	/* CC1200 */
#define SPI_2_PB13_PB14_PB15	1
#define SPI_2_PD1_PD3_PD4	0
#define SPI_2_GPIO		(&stm_gpiob)
#define SPI_2_SCK		13
#define SPI_2_MISO		14
#define SPI_2_MOSI		15
#define SPI_2_OSPEEDR		STM_OSPEEDR_10MHz

#define HAS_I2C_1		0

#define HAS_I2C_2		0

#define PACKET_HAS_SLAVE	0
#define PACKET_HAS_MASTER	0

#define FAST_TIMER_FREQ		10000	/* .1ms for debouncing */

/*
 * Radio is a cc1200 connected via SPI
 */

#define AO_RADIO_CAL_DEFAULT 	5695733

#define AO_FEC_DEBUG		0
#define AO_CC1200_SPI_CS_PORT	(&stm_gpioa)
#define AO_CC1200_SPI_CS_PIN	7
#define AO_CC1200_SPI_BUS	AO_SPI_2_PB13_PB14_PB15
#define AO_CC1200_SPI		stm_spi2

#define AO_CC1200_INT_PORT		(&stm_gpiob)
#define AO_CC1200_INT_PIN		(11)

#define AO_CC1200_INT_GPIO	2
#define AO_CC1200_INT_GPIO_IOCFG	CC1200_IOCFG2

#define LED_PORT_0		(&stm_gpioa)
#define LED_PORT_1		(&stm_gpiob)

#define LED_PORT_0_ENABLE	STM_RCC_AHBENR_GPIOAEN
#define LED_PORT_1_ENABLE	STM_RCC_AHBENR_GPIOBEN

/* Port A, pins 4-6 */
#define LED_PORT_0_SHIFT	4
#define LED_PORT_0_MASK		0x7
#define LED_PIN_GREEN		0
#define LED_PIN_AMBER		1
#define LED_PIN_RED		2
#define AO_LED_RED		(1 << LED_PIN_RED)
#define AO_LED_AMBER		(1 << LED_PIN_AMBER)
#define AO_LED_GREEN		(1 << LED_PIN_GREEN)

/* Port B, pins 3-5 */
#define LED_PORT_1_SHIFT	0
#define LED_PORT_1_MASK		(0x7 << 3)
#define LED_PIN_CONT_1		3
#define LED_PIN_CONT_0		4
#define LED_PIN_ARMED		5

#define AO_LED_ARMED		(1 << LED_PIN_ARMED)
#define AO_LED_CONTINUITY(c)	(1 << (4 - (c)))
#define AO_LED_CONTINUITY_MASK	(0x3 << 3)

#define LEDS_AVAILABLE		(LED_PORT_0_MASK|LED_PORT_1_MASK)

#define SPI_CS_PORT	P1
#define SPI_CS_SEL	P1SEL
#define SPI_CS_DIR	P1DIR

#define SPI_CONST	0x00

#define AO_PAD_NUM		2
#define	AO_PAD_PORT		(&stm_gpiob)

#define AO_PAD_PIN_0		9
#define AO_PAD_ADC_0		0

#define AO_PAD_PIN_1		8
#define AO_PAD_ADC_1		1

#define AO_PAD_ALL_PINS		((1 << AO_PAD_PIN_0) | (1 << AO_PAD_PIN_1))
#define AO_PAD_ALL_CHANNELS	((1 << 0) | (1 << 1))

/* test these values with real igniters */
#define AO_PAD_RELAY_CLOSED	3524
#define AO_PAD_NO_IGNITER	16904
#define AO_PAD_GOOD_IGNITER	22514

#define AO_PAD_ADC_PYRO		8
#define AO_PAD_ADC_BATT		2

#define AO_ADC_FIRST_PIN	0

#define AO_NUM_ADC		4

#define AO_ADC_SQ1		AO_PAD_ADC_0
#define AO_ADC_SQ2		AO_PAD_ADC_1
#define AO_ADC_SQ3		AO_PAD_ADC_PYRO
#define AO_ADC_SQ4		AO_PAD_ADC_BATT

#define AO_PYRO_R_PYRO_SENSE	200
#define AO_PYRO_R_SENSE_GND	22

#define AO_FIRE_R_POWER_FET	0
#define AO_FIRE_R_FET_SENSE	200
#define AO_FIRE_R_SENSE_GND	22

#define HAS_ADC_TEMP		0

struct ao_adc {
	int16_t		sense[AO_PAD_NUM];
	int16_t		pyro;
	int16_t		batt;
};

#define AO_ADC_DUMP(p)							\
	printf ("tick: %5u 0: %5d 1: %5d pyro: %5d batt %5d\n", \
		(p)->tick,						\
		(p)->adc.sense[0],					\
		(p)->adc.sense[1],					\
		(p)->adc.pyro,						\
		(p)->adc.batt)

#define AO_ADC_PINS	((1 << AO_PAD_ADC_0) | \
			 (1 << AO_PAD_ADC_1) | \
			 (1 << AO_PAD_ADC_PYRO) | \
			 (1 << AO_PAD_ADC_BATT))

#endif /* _AO_PINS_H_ */