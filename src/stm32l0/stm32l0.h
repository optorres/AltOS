/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#ifndef _STM32L0_H_
#define _STM32L0_H_

#include <stdint.h>

typedef volatile uint32_t	vuint32_t;
typedef volatile void *		vvoid_t;

struct stm_gpio {
	vuint32_t	moder;
	vuint32_t	otyper;
	vuint32_t	ospeedr;
	vuint32_t	pupdr;

	vuint32_t	idr;
	vuint32_t	odr;
	vuint32_t	bsrr;
	vuint32_t	lckr;

	vuint32_t	afrl;
	vuint32_t	afrh;
	vuint32_t	brr;
};

#define STM_MODER_SHIFT(pin)		((pin) << 1)
#define STM_MODER_MASK			3
#define STM_MODER_INPUT			0
#define STM_MODER_OUTPUT		1
#define STM_MODER_ALTERNATE		2
#define STM_MODER_ANALOG		3

static inline void
stm_moder_set(struct stm_gpio *gpio, int pin, vuint32_t value) {
	gpio->moder = ((gpio->moder &
			~(STM_MODER_MASK << STM_MODER_SHIFT(pin))) |
		       value << STM_MODER_SHIFT(pin));
}

static inline uint32_t
stm_spread_mask(uint16_t mask) {
	uint32_t m = mask;

	/* 0000000000000000mmmmmmmmmmmmmmmm */
	m = (m & 0xff) | ((m & 0xff00) << 8);
	/* 00000000mmmmmmmm00000000mmmmmmmm */
	m = (m & 0x000f000f) | ((m & 0x00f000f0) << 4);
	/* 0000mmmm0000mmmm0000mmmm0000mmmm */
	m = (m & 0x03030303) | ((m & 0x0c0c0c0c) << 2);
	/* 00mm00mm00mm00mm00mm00mm00mm00mm */
	m = (m & 0x11111111) | ((m & 0x22222222) << 2);
	/* 0m0m0m0m0m0m0m0m0m0m0m0m0m0m0m0m */
	return m;
}

static inline void
stm_moder_set_mask(struct stm_gpio *gpio, uint16_t mask, uint32_t value) {
	uint32_t	bits32 = stm_spread_mask(mask);
	uint32_t	mask32 = 3 * bits32;
	uint32_t	value32 = (value & 3) * bits32;

	gpio->moder = ((gpio->moder & ~mask32) | value32);
}

static inline uint32_t
stm_moder_get(struct stm_gpio *gpio, int pin) {
	return (gpio->moder >> STM_MODER_SHIFT(pin)) & STM_MODER_MASK;
}

#define STM_OTYPER_SHIFT(pin)		(pin)
#define STM_OTYPER_MASK			1
#define STM_OTYPER_PUSH_PULL		0
#define STM_OTYPER_OPEN_DRAIN		1

static inline void
stm_otyper_set(struct stm_gpio *gpio, int pin, vuint32_t value) {
	gpio->otyper = ((gpio->otyper &
			 ~(STM_OTYPER_MASK << STM_OTYPER_SHIFT(pin))) |
			value << STM_OTYPER_SHIFT(pin));
}

static inline uint32_t
stm_otyper_get(struct stm_gpio *gpio, int pin) {
	return (gpio->otyper >> STM_OTYPER_SHIFT(pin)) & STM_OTYPER_MASK;
}

#define STM_OSPEEDR_SHIFT(pin)		((pin) << 1)
#define STM_OSPEEDR_MASK		3
#define STM_OSPEEDR_LOW			0
#define STM_OSPEEDR_MEDIUM		1
#define STM_OSPEEDR_HIGH		2
#define STM_OSPEEDR_VERY_HIGH		3

static inline void
stm_ospeedr_set(struct stm_gpio *gpio, int pin, uint32_t value) {
	gpio->ospeedr = ((gpio->ospeedr &
			~(STM_OSPEEDR_MASK << STM_OSPEEDR_SHIFT(pin))) |
		       value << STM_OSPEEDR_SHIFT(pin));
}

static inline void
stm_ospeedr_set_mask(struct stm_gpio *gpio, uint16_t mask, uint32_t value) {
	uint32_t	bits32 = stm_spread_mask(mask);
	uint32_t	mask32 = 3 * bits32;
	uint32_t	value32 = (value & 3) * bits32;

	gpio->ospeedr = ((gpio->ospeedr & ~mask32) | value32);
}

static inline uint32_t
stm_ospeedr_get(struct stm_gpio *gpio, int pin) {
	return (gpio->ospeedr >> STM_OSPEEDR_SHIFT(pin)) & STM_OSPEEDR_MASK;
}

#define STM_PUPDR_SHIFT(pin)		((pin) << 1)
#define STM_PUPDR_MASK			3
#define STM_PUPDR_NONE			0
#define STM_PUPDR_PULL_UP		1
#define STM_PUPDR_PULL_DOWN		2
#define STM_PUPDR_RESERVED		3

static inline void
stm_pupdr_set(struct stm_gpio *gpio, int pin, uint32_t value) {
	gpio->pupdr = ((gpio->pupdr &
			~(STM_PUPDR_MASK << STM_PUPDR_SHIFT(pin))) |
		       value << STM_PUPDR_SHIFT(pin));
}

static inline void
stm_pupdr_set_mask(struct stm_gpio *gpio, uint16_t mask, uint32_t value) {
	uint32_t	bits32 = stm_spread_mask(mask);
	uint32_t	mask32 = 3 * bits32;
	uint32_t	value32 = (value & 3) * bits32;

	gpio->pupdr = (gpio->pupdr & ~mask32) | value32;
}

static inline uint32_t
stm_pupdr_get(struct stm_gpio *gpio, int pin) {
	return (gpio->pupdr >> STM_PUPDR_SHIFT(pin)) & STM_PUPDR_MASK;
}

#define STM_AFR_SHIFT(pin)		((pin) << 2)
#define STM_AFR_MASK			0xf
#define STM_AFR_NONE			0
#define STM_AFR_AF0			0x0
#define STM_AFR_AF1			0x1
#define STM_AFR_AF2			0x2
#define STM_AFR_AF3			0x3
#define STM_AFR_AF4			0x4
#define STM_AFR_AF5			0x5
#define STM_AFR_AF6			0x6
#define STM_AFR_AF7			0x7
#define STM_AFR_AF8			0x8
#define STM_AFR_AF9			0x9
#define STM_AFR_AF10			0xa
#define STM_AFR_AF11			0xb
#define STM_AFR_AF12			0xc
#define STM_AFR_AF13			0xd
#define STM_AFR_AF14			0xe
#define STM_AFR_AF15			0xf

static inline void
stm_afr_set(struct stm_gpio *gpio, int pin, uint32_t value) {
	/*
	 * Set alternate pin mode too
	 */
	stm_moder_set(gpio, pin, STM_MODER_ALTERNATE);
	if (pin < 8)
		gpio->afrl = ((gpio->afrl &
			       ~(STM_AFR_MASK << STM_AFR_SHIFT(pin))) |
			      value << STM_AFR_SHIFT(pin));
	else {
		pin -= 8;
		gpio->afrh = ((gpio->afrh &
			       ~(STM_AFR_MASK << STM_AFR_SHIFT(pin))) |
			      value << STM_AFR_SHIFT(pin));
	}
}
	
static inline uint32_t
stm_afr_get(struct stm_gpio *gpio, int pin) {
	if (pin < 8)
		return (gpio->afrl >> STM_AFR_SHIFT(pin)) & STM_AFR_MASK;
	else {
		pin -= 8;
		return (gpio->afrh >> STM_AFR_SHIFT(pin)) & STM_AFR_MASK;
	}
}

static inline void
stm_gpio_set(struct stm_gpio *gpio, int pin, uint8_t value) {
	/* Use the bit set/reset register to do this atomically */
	gpio->bsrr = value ? (1 << pin) : (1 << (pin + 16));
}

static inline void
stm_gpio_set_mask(struct stm_gpio *gpio, uint16_t bits, uint16_t mask) {
	/* Use the bit set/reset register to do this atomically */
	gpio->bsrr = ((uint32_t) (~bits & mask) << 16) | ((uint32_t) (bits & mask));
}

static inline void
stm_gpio_set_bits(struct stm_gpio *gpio, uint16_t bits) {
	gpio->bsrr = bits;
}

static inline void
stm_gpio_clr_bits(struct stm_gpio *gpio, uint16_t bits) {
	gpio->bsrr = ((uint32_t) bits) << 16;
}

static inline uint8_t
stm_gpio_get(struct stm_gpio *gpio, int pin) {
	return (gpio->idr >> pin) & 1;
}

static inline uint16_t
stm_gpio_get_all(struct stm_gpio *gpio) {
	return gpio->idr;
}

/*
 * We can't define these in registers.ld or our fancy
 * ao_enable_gpio macro will expand into a huge pile of code
 * as the compiler won't do correct constant folding and
 * dead-code elimination
 */

extern struct stm_gpio stm_gpioa;
extern struct stm_gpio stm_gpiob;
extern struct stm_gpio stm_gpioc;
extern struct stm_gpio stm_gpiod;
extern struct stm_gpio stm_gpioe;
extern struct stm_gpio stm_gpioh;

#define stm_gpiob  (*((struct stm_gpio *) 0x50000400))
#define stm_gpioa  (*((struct stm_gpio *) 0x50000000))

struct stm_usart {
	vuint32_t	cr1;	/* control register 1 */
	vuint32_t	cr2;	/* control register 2 */
	vuint32_t	cr3;	/* control register 3 */
	vuint32_t	brr;	/* baud rate register */

	vuint32_t	gtpr;	/* guard time and prescaler */
	vuint32_t	rtor;	/* receiver timeout register */
	vuint32_t	rqr;	/* request register */
	vuint32_t	isr;	/* interrupt and status register */

	vuint32_t	icr;	/* interrupt flag clear register */
	vuint32_t	rdr;	/* receive data register */
	vuint32_t	tdr;	/* transmit data register */
};

#define STM_USART_CR1_M1	28
#define STM_USART_CR1_EOBIE	27
#define STM_USART_CR1_RTOIE	26
#define STM_USART_CR1_DEAT	21
#define STM_USART_CR1_DEDT	16
#define STM_USART_CR1_OVER8	15
#define STM_USART_CR1_CMIE	14
#define STM_USART_CR1_MME	13
#define STM_USART_CR1_M0	12
#define STM_USART_CR1_WAKE	11
#define STM_USART_CR1_PCE	10
#define STM_USART_CR1_PS	9
#define STM_USART_CR1_PEIE	8
#define STM_USART_CR1_TXEIE	7
#define STM_USART_CR1_TCIE	6
#define STM_USART_CR1_RXNEIE	5
#define STM_USART_CR1_IDLEIE	4
#define STM_USART_CR1_TE	3
#define STM_USART_CR1_RE	2
#define STM_USART_CR1_UESM	1
#define STM_USART_CR1_UE	0

#define STM_USART_CR2_ADD	24
#define STM_USART_CR2_RTOEN	23
#define STM_USART_CR2_ABRMOD	21
#define STM_USART_CR2_ABREN	20
#define STM_USART_CR2_MSBFIRST	19
#define STM_USART_CR2_DATAINV	18
#define STM_USART_CR2_TXINV	17
#define STM_USART_CR2_RXINV	16
#define STM_USART_CR2_SWAP	15
#define STM_USART_CR2_LINEN	14
#define STM_USART_CR2_STOP	12
#define STM_USART_CR2_CLKEN	11
#define STM_USART_CR2_CPOL	10
#define STM_USART_CR2_CHPA	9
#define STM_USART_CR2_LBCL	8
#define STM_USART_CR2_LBDIE	6
#define STM_USART_CR2_LBDL	5
#define STM_USART_CR2_ADDM7	4

#define STM_USART_CR3_WUFIE	22
#define STM_USART_CR3_WUS	20
#define STM_USART_CR3_SCARCNT	17
#define STM_USART_CR3_DEP	15
#define STM_USART_CR3_DEM	14
#define STM_USART_CR3_DDRE	13
#define STM_USART_CR3_OVRDIS	12
#define STM_USART_CR3_ONEBIT	11
#define STM_USART_CR3_CTIIE	10
#define STM_USART_CR3_CTSE	9
#define STM_USART_CR3_RTSE	8
#define STM_USART_CR3_DMAT	7
#define STM_USART_CR3_DMAR	6
#define STM_USART_CR3_SCEN	5
#define STM_USART_CR3_NACK	4
#define STM_USART_CR3_HDSEL	3
#define STM_USART_CR3_IRLP	2
#define STM_USART_CR3_IREN	1
#define STM_USART_CR3_EIE	0

#define STM_USART_GTPR_GT	8
#define STM_USART_GTPR_PSC	0

#define STM_USART_RQR_TXFRQ	4
#define STM_USART_RQR_RXFRQ	3
#define STM_USART_RQR_MMRQ	2
#define STM_USART_RQR_SBKRQ	1
#define STM_USART_RQR_ABRRQ	0

#define STM_USART_ISR_REACK	22
#define STM_USART_ISR_TEACK	21
#define STM_USART_ISR_WUF	20
#define STM_USART_ISR_RWU	19
#define STM_USART_ISR_SBKF	18
#define STM_USART_ISR_CMF	17
#define STM_USART_ISR_BUSY	16
#define STM_USART_ISR_ABRF	15
#define STM_USART_ISR_ABRE	14
#define STM_USART_ISR_EOBF	12
#define STM_USART_ISR_RTOF	11
#define STM_USART_ISR_CTS	10
#define STM_USART_ISR_CTSIF	9
#define STM_USART_ISR_LBDF	8
#define STM_USART_ISR_TXE	7
#define STM_USART_ISR_TC	6
#define STM_USART_ISR_RXNE	5
#define STM_USART_ISR_IDLE	4
#define STM_USART_ISR_ORE	3
#define STM_USART_ISR_NF	2
#define STM_USART_ISR_FE	1
#define STM_USART_ISR_PE	0

#define STM_USART_ICR_WUCF	20
#define STM_USART_ICR_CMCF	17
#define STM_USART_ICR_EOBCF	12
#define STM_USART_ICR_RTOCF	11
#define STM_USART_ICR_CTSCF	9
#define STM_USART_ICR_LBDCF	8
#define STM_USART_ICR_TCCF	6
#define STM_USART_ICR_IDLECF	4
#define STM_USART_ICR_ORECF	3
#define STM_USART_ICR_NCF	2
#define STM_USART_ICR_FECF	1
#define STM_USART_ICR_PECF	0

extern struct stm_usart	stm_usart1;
extern struct stm_usart stm_usart2;
#define stm_usart1 (*((struct stm_usart *) 0x40013800))
#define stm_usart2 (*((struct stm_usart *) 0x40004400))

struct stm_tim {
};

extern struct stm_tim stm_tim9;

struct stm_tim1011 {
	vuint32_t	cr1;
	uint32_t	unused_4;
	vuint32_t	smcr;
	vuint32_t	dier;
	vuint32_t	sr;
	vuint32_t	egr;
	vuint32_t	ccmr1;
	uint32_t	unused_1c;
	vuint32_t	ccer;
	vuint32_t	cnt;
	vuint32_t	psc;
	vuint32_t	arr;
	uint32_t	unused_30;
	vuint32_t	ccr1;
	uint32_t	unused_38;
	uint32_t	unused_3c;
	uint32_t	unused_40;
	uint32_t	unused_44;
	uint32_t	unused_48;
	uint32_t	unused_4c;
	vuint32_t	or;
};

extern struct stm_tim1011 stm_tim10;
extern struct stm_tim1011 stm_tim11;

#define STM_TIM1011_CR1_CKD	8
#define  STM_TIM1011_CR1_CKD_1		0
#define  STM_TIM1011_CR1_CKD_2		1
#define  STM_TIM1011_CR1_CKD_4		2
#define  STM_TIM1011_CR1_CKD_MASK	3
#define STM_TIM1011_CR1_ARPE	7
#define STM_TIM1011_CR1_URS	2
#define STM_TIM1011_CR1_UDIS	1
#define STM_TIM1011_CR1_CEN	0

#define STM_TIM1011_SMCR_ETP	15
#define STM_TIM1011_SMCR_ECE	14
#define STM_TIM1011_SMCR_ETPS	12
#define  STM_TIM1011_SMCR_ETPS_OFF	0
#define  STM_TIM1011_SMCR_ETPS_2	1
#define  STM_TIM1011_SMCR_ETPS_4	2
#define  STM_TIM1011_SMCR_ETPS_8	3
#define  STM_TIM1011_SMCR_ETPS_MASK	3
#define STM_TIM1011_SMCR_ETF	8
#define  STM_TIM1011_SMCR_ETF_NONE		0
#define  STM_TIM1011_SMCR_ETF_CK_INT_2		1
#define  STM_TIM1011_SMCR_ETF_CK_INT_4		2
#define  STM_TIM1011_SMCR_ETF_CK_INT_8		3
#define  STM_TIM1011_SMCR_ETF_DTS_2_6		4
#define  STM_TIM1011_SMCR_ETF_DTS_2_8		5
#define  STM_TIM1011_SMCR_ETF_DTS_4_6		6
#define  STM_TIM1011_SMCR_ETF_DTS_4_8		7
#define  STM_TIM1011_SMCR_ETF_DTS_8_6		8
#define  STM_TIM1011_SMCR_ETF_DTS_8_8		9
#define  STM_TIM1011_SMCR_ETF_DTS_16_5		10
#define  STM_TIM1011_SMCR_ETF_DTS_16_6		11
#define  STM_TIM1011_SMCR_ETF_DTS_16_8		12
#define  STM_TIM1011_SMCR_ETF_DTS_32_5		13
#define  STM_TIM1011_SMCR_ETF_DTS_32_6		14
#define  STM_TIM1011_SMCR_ETF_DTS_32_8		15
#define  STM_TIM1011_SMCR_ETF_MASK		15

#define STM_TIM1011_DIER_CC1E	1
#define STM_TIM1011_DIER_UIE	0

#define STM_TIM1011_SR_CC1OF	9
#define STM_TIM1011_SR_CC1IF	1
#define STM_TIM1011_SR_UIF	0

#define STM_TIM1011_EGR_CC1G	1
#define STM_TIM1011_EGR_UG	0

#define STM_TIM1011_CCMR1_OC1CE	7
#define STM_TIM1011_CCMR1_OC1M	4
#define  STM_TIM1011_CCMR1_OC1M_FROZEN			0
#define  STM_TIM1011_CCMR1_OC1M_SET_1_ACTIVE_ON_MATCH	1
#define  STM_TIM1011_CCMR1_OC1M_SET_1_INACTIVE_ON_MATCH	2
#define  STM_TIM1011_CCMR1_OC1M_TOGGLE			3
#define  STM_TIM1011_CCMR1_OC1M_FORCE_INACTIVE		4
#define  STM_TIM1011_CCMR1_OC1M_FORCE_ACTIVE		5
#define  STM_TIM1011_CCMR1_OC1M_PWM_MODE_1		6
#define  STM_TIM1011_CCMR1_OC1M_PWM_MODE_2		7
#define  STM_TIM1011_CCMR1_OC1M_MASK			7
#define STM_TIM1011_CCMR1_OC1PE	3
#define STM_TIM1011_CCMR1_OC1FE	2
#define STM_TIM1011_CCMR1_CC1S	0
#define  STM_TIM1011_CCMR1_CC1S_OUTPUT			0
#define  STM_TIM1011_CCMR1_CC1S_INPUT_TI1		1
#define  STM_TIM1011_CCMR1_CC1S_INPUT_TI2		2
#define  STM_TIM1011_CCMR1_CC1S_INPUT_TRC		3
#define  STM_TIM1011_CCMR1_CC1S_MASK			3

#define  STM_TIM1011_CCMR1_IC1F_NONE		0
#define  STM_TIM1011_CCMR1_IC1F_CK_INT_2	1
#define  STM_TIM1011_CCMR1_IC1F_CK_INT_4	2
#define  STM_TIM1011_CCMR1_IC1F_CK_INT_8	3
#define  STM_TIM1011_CCMR1_IC1F_DTS_2_6		4
#define  STM_TIM1011_CCMR1_IC1F_DTS_2_8		5
#define  STM_TIM1011_CCMR1_IC1F_DTS_4_6		6
#define  STM_TIM1011_CCMR1_IC1F_DTS_4_8		7
#define  STM_TIM1011_CCMR1_IC1F_DTS_8_6		8
#define  STM_TIM1011_CCMR1_IC1F_DTS_8_8		9
#define  STM_TIM1011_CCMR1_IC1F_DTS_16_5	10
#define  STM_TIM1011_CCMR1_IC1F_DTS_16_6	11
#define  STM_TIM1011_CCMR1_IC1F_DTS_16_8	12
#define  STM_TIM1011_CCMR1_IC1F_DTS_32_5	13
#define  STM_TIM1011_CCMR1_IC1F_DTS_32_6	14
#define  STM_TIM1011_CCMR1_IC1F_DTS_32_8	15
#define  STM_TIM1011_CCMR1_IC1F_MASK		15
#define STM_TIM1011_CCMR1_IC1PSC	2
#define  STM_TIM1011_CCMR1_IC1PSC_1		0
#define  STM_TIM1011_CCMR1_IC1PSC_2		1
#define  STM_TIM1011_CCMR1_IC1PSC_4		2
#define  STM_TIM1011_CCMR1_IC1PSC_8		3
#define  STM_TIM1011_CCMR1_IC1PSC_MASK		3
#define STM_TIM1011_CCMR1_CC1S		0

#define STM_TIM1011_CCER_CC1NP		3
#define STM_TIM1011_CCER_CC1P		1
#define STM_TIM1011_CCER_CC1E		0

#define STM_TIM1011_OR_TI1_RMP_RI	3
#define STM_TIM1011_ETR_RMP		2
#define STM_TIM1011_TI1_RMP		0
#define  STM_TIM1011_TI1_RMP_GPIO		0
#define  STM_TIM1011_TI1_RMP_LSI		1
#define  STM_TIM1011_TI1_RMP_LSE		2
#define  STM_TIM1011_TI1_RMP_RTC		3
#define  STM_TIM1011_TI1_RMP_MASK		3

/* Flash interface */

struct stm_flash {
	vuint32_t	acr;
	vuint32_t	pecr;
	vuint32_t	pdkeyr;
	vuint32_t	pekeyr;

	vuint32_t	prgkeyr;
	vuint32_t	optkeyr;
	vuint32_t	sr;
	vuint32_t	obr;

	vuint32_t	wrpr;
};

extern struct stm_flash	stm_flash;

#define STM_FLASH_ACR_RUN_PD	(4)
#define STM_FLASH_ACR_SLEEP_PD	(3)
#define STM_FLASH_ACR_ACC64	(2)
#define STM_FLASH_ACR_PRFEN	(1)
#define STM_FLASH_ACR_LATENCY	(0)

#define STM_FLASH_PECR_OBL_LAUNCH	18
#define STM_FLASH_PECR_ERRIE		17
#define STM_FLASH_PECR_EOPIE		16
#define STM_FLASH_PECR_FPRG		10
#define STM_FLASH_PECR_ERASE		9
#define STM_FLASH_PECR_FTDW		8
#define STM_FLASH_PECR_DATA		4
#define STM_FLASH_PECR_PROG		3
#define STM_FLASH_PECR_OPTLOCK		2
#define STM_FLASH_PECR_PRGLOCK		1
#define STM_FLASH_PECR_PELOCK		0

#define STM_FLASH_SR_OPTVERR		11
#define STM_FLASH_SR_SIZERR		10
#define STM_FLASH_SR_PGAERR		9
#define STM_FLASH_SR_WRPERR		8
#define STM_FLASH_SR_READY		3
#define STM_FLASH_SR_ENDHV		2
#define STM_FLASH_SR_EOP		1
#define STM_FLASH_SR_BSY		0

#define STM_FLASH_PEKEYR_PEKEY1	0x89ABCDEF
#define STM_FLASH_PEKEYR_PEKEY2 0x02030405

#define STM_FLASH_PRGKEYR_PRGKEY1 0x8C9DAEBF
#define STM_FLASH_PRGKEYR_PRGKEY2 0x13141516

struct stm_rcc {
	vuint32_t	cr;
	vuint32_t	icscr;
	uint32_t	unused08;
	vuint32_t	cfgr;

	vuint32_t	cier;
	vuint32_t	cifr;
	vuint32_t	cicr;
	vuint32_t	iopstr;

	vuint32_t	ahbrstr;
	vuint32_t	apb2rstr;
	vuint32_t	apb1rstr;
	vuint32_t	iopenr;

	vuint32_t	ahbenr;
	vuint32_t	apb2enr;
	vuint32_t	apb1enr;
	vuint32_t	iopsmen;

	vuint32_t	ahbsmenr;
	vuint32_t	apb2smenr;
	vuint32_t	apb1smenr;
	vuint32_t	ccipr;

	vuint32_t	csr;
};

extern struct stm_rcc stm_rcc;

/* Nominal high speed internal oscillator frequency is 16MHz */
#define STM_HSI_FREQ		16000000
#define STM_MSI_FREQ		 2097000

#define STM_RCC_CR_RTCPRE	(29)
#define  STM_RCC_CR_RTCPRE_HSE_DIV_2	0
#define  STM_RCC_CR_RTCPRE_HSE_DIV_4	1
#define  STM_RCC_CR_RTCPRE_HSE_DIV_8	2
#define  STM_RCC_CR_RTCPRE_HSE_DIV_16	3
#define  STM_RCC_CR_RTCPRE_HSE_MASK	3

#define STM_RCC_CR_CSSON	(28)
#define STM_RCC_CR_PLLRDY	(25)
#define STM_RCC_CR_PLLON	(24)
#define STM_RCC_CR_HSEBYP	(18)
#define STM_RCC_CR_HSERDY	(17)
#define STM_RCC_CR_HSEON	(16)
#define STM_RCC_CR_MSIRDY	(9)
#define STM_RCC_CR_MSION	(8)
#define STM_RCC_CR_HSIRDY	(1)
#define STM_RCC_CR_HSION	(0)

#define STM_RCC_CFGR_MCOPRE	(28)
#define  STM_RCC_CFGR_MCOPRE_DIV_1	0
#define  STM_RCC_CFGR_MCOPRE_DIV_2	1
#define  STM_RCC_CFGR_MCOPRE_DIV_4	2
#define  STM_RCC_CFGR_MCOPRE_DIV_8	3
#define  STM_RCC_CFGR_MCOPRE_DIV_16	4
#define  STM_RCC_CFGR_MCOPRE_MASK	7

#define STM_RCC_CFGR_MCOSEL	(24)
#define  STM_RCC_CFGR_MCOSEL_DISABLE	0
#define  STM_RCC_CFGR_MCOSEL_SYSCLK	1
#define  STM_RCC_CFGR_MCOSEL_HSI	2
#define  STM_RCC_CFGR_MCOSEL_MSI	3
#define  STM_RCC_CFGR_MCOSEL_HSE	4
#define  STM_RCC_CFGR_MCOSEL_PLL	5
#define  STM_RCC_CFGR_MCOSEL_LSI	6
#define  STM_RCC_CFGR_MCOSEL_LSE	7
#define  STM_RCC_CFGR_MCOSEL_MASK	7

#define STM_RCC_CFGR_PLLDIV	(22)
#define  STM_RCC_CFGR_PLLDIV_2		1
#define  STM_RCC_CFGR_PLLDIV_3		2
#define  STM_RCC_CFGR_PLLDIV_4		3
#define  STM_RCC_CFGR_PLLDIV_MASK	3

#define STM_RCC_CFGR_PLLMUL	(18)
#define  STM_RCC_CFGR_PLLMUL_3		0
#define  STM_RCC_CFGR_PLLMUL_4		1
#define  STM_RCC_CFGR_PLLMUL_6		2
#define  STM_RCC_CFGR_PLLMUL_8		3
#define  STM_RCC_CFGR_PLLMUL_12		4
#define  STM_RCC_CFGR_PLLMUL_16		5
#define  STM_RCC_CFGR_PLLMUL_24		6
#define  STM_RCC_CFGR_PLLMUL_32		7
#define  STM_RCC_CFGR_PLLMUL_48		8
#define  STM_RCC_CFGR_PLLMUL_MASK	0xf

#define STM_RCC_CFGR_PLLSRC	(16)

#define STM_RCC_CFGR_PPRE2	(11)
#define  STM_RCC_CFGR_PPRE2_DIV_1	0
#define  STM_RCC_CFGR_PPRE2_DIV_2	4
#define  STM_RCC_CFGR_PPRE2_DIV_4	5
#define  STM_RCC_CFGR_PPRE2_DIV_8	6
#define  STM_RCC_CFGR_PPRE2_DIV_16	7
#define  STM_RCC_CFGR_PPRE2_MASK	7

#define STM_RCC_CFGR_PPRE1	(8)
#define  STM_RCC_CFGR_PPRE1_DIV_1	0
#define  STM_RCC_CFGR_PPRE1_DIV_2	4
#define  STM_RCC_CFGR_PPRE1_DIV_4	5
#define  STM_RCC_CFGR_PPRE1_DIV_8	6
#define  STM_RCC_CFGR_PPRE1_DIV_16	7
#define  STM_RCC_CFGR_PPRE1_MASK	7

#define STM_RCC_CFGR_HPRE	(4)
#define  STM_RCC_CFGR_HPRE_DIV_1	0
#define  STM_RCC_CFGR_HPRE_DIV_2	8
#define  STM_RCC_CFGR_HPRE_DIV_4	9
#define  STM_RCC_CFGR_HPRE_DIV_8	0xa
#define  STM_RCC_CFGR_HPRE_DIV_16	0xb
#define  STM_RCC_CFGR_HPRE_DIV_64	0xc
#define  STM_RCC_CFGR_HPRE_DIV_128	0xd
#define  STM_RCC_CFGR_HPRE_DIV_256	0xe
#define  STM_RCC_CFGR_HPRE_DIV_512	0xf
#define  STM_RCC_CFGR_HPRE_MASK		0xf

#define STM_RCC_CFGR_SWS	(2)
#define  STM_RCC_CFGR_SWS_MSI		0
#define  STM_RCC_CFGR_SWS_HSI		1
#define  STM_RCC_CFGR_SWS_HSE		2
#define  STM_RCC_CFGR_SWS_PLL		3
#define  STM_RCC_CFGR_SWS_MASK		3

#define STM_RCC_CFGR_SW		(0)
#define  STM_RCC_CFGR_SW_MSI		0
#define  STM_RCC_CFGR_SW_HSI		1
#define  STM_RCC_CFGR_SW_HSE		2
#define  STM_RCC_CFGR_SW_PLL		3
#define  STM_RCC_CFGR_SW_MASK		3

#define STM_RCC_IOPENR_IOPAEN		0
#define STM_RCC_IOPENR_IOPBEN		1
#define STM_RCC_IOPENR_IOPCEN		2
#define STM_RCC_IOPENR_IOPDEN		3
#define STM_RCC_IOPENR_IOPEEN		4
#define STM_RCC_IOPENR_IOPHEN		7

#define STM_RCC_AHBENR_DMA1EN		0
#define STM_RCC_AHBENR_MIFEN		8
#define STM_RCC_AHBENR_CRCEN		12
#define STM_RCC_AHBENR_CRYPEN		24

#define STM_RCC_APB2ENR_DBGEN		(22)
#define STM_RCC_APB2ENR_USART1EN	(14)
#define STM_RCC_APB2ENR_SPI1EN		(12)
#define STM_RCC_APB2ENR_ADC1EN		(9)
#define STM_RCC_APB2ENR_FWEN		(7)
#define STM_RCC_APB2ENR_TIM22EN		(5)
#define STM_RCC_APB2ENR_TIM21EN		(2)
#define STM_RCC_APB2ENR_SYSCFGEN	(0)

#define STM_RCC_APB1ENR_LPTIM1EN	31
#define STM_RCC_APB1ENR_I2C3EN		30
#define STM_RCC_APB1ENR_PWREN		28
#define STM_RCC_APB1ENR_I2C2EN		22
#define STM_RCC_APB1ENR_I2C1EN		21
#define STM_RCC_APB1ENR_USART5EN	20
#define STM_RCC_APB1ENR_USART4EN	19
#define STM_RCC_APB1ENR_LPUART1EN	18
#define STM_RCC_APB1ENR_USART2EN	17
#define STM_RCC_APB1ENR_SPI2EN		14
#define STM_RCC_APB1ENR_WWDGEN		11
#define STM_RCC_APB1ENR_TIM7EN		5
#define STM_RCC_APB1ENR_TIM6EN		4
#define STM_RCC_APB1ENR_TIM3EN		1
#define STM_RCC_APB1ENR_TIM2EN		0

#define STM_RCC_CSR_LPWRRSTF		(31)
#define STM_RCC_CSR_WWDGRSTF		(30)
#define STM_RCC_CSR_IWDGRSTF		(29)
#define STM_RCC_CSR_SFTRSTF		(28)
#define STM_RCC_CSR_PORRSTF		(27)
#define STM_RCC_CSR_PINRSTF		(26)
#define STM_RCC_CSR_OBLRSTF		(25)
#define STM_RCC_CSR_RMVF		(24)
#define STM_RCC_CSR_RTFRST		(23)
#define STM_RCC_CSR_RTCEN		(22)
#define STM_RCC_CSR_RTCSEL		(16)

#define  STM_RCC_CSR_RTCSEL_NONE		0
#define  STM_RCC_CSR_RTCSEL_LSE			1
#define  STM_RCC_CSR_RTCSEL_LSI			2
#define  STM_RCC_CSR_RTCSEL_HSE			3
#define  STM_RCC_CSR_RTCSEL_MASK		3

#define STM_RCC_CSR_LSEBYP		(10)
#define STM_RCC_CSR_LSERDY		(9)
#define STM_RCC_CSR_LSEON		(8)
#define STM_RCC_CSR_LSIRDY		(1)
#define STM_RCC_CSR_LSION		(0)

struct stm_pwr {
	vuint32_t	cr;
	vuint32_t	csr;
};

extern struct stm_pwr stm_pwr;

#define STM_PWR_CR_LPRUN	(14)

#define STM_PWR_CR_VOS		(11)
#define  STM_PWR_CR_VOS_1_8		1
#define  STM_PWR_CR_VOS_1_5		2
#define  STM_PWR_CR_VOS_1_2		3
#define  STM_PWR_CR_VOS_MASK		3

#define STM_PWR_CR_FWU		(10)
#define STM_PWR_CR_ULP		(9)
#define STM_PWR_CR_DBP		(8)

#define STM_PWR_CR_PLS		(5)
#define  STM_PWR_CR_PLS_1_9	0
#define  STM_PWR_CR_PLS_2_1	1
#define  STM_PWR_CR_PLS_2_3	2
#define  STM_PWR_CR_PLS_2_5	3
#define  STM_PWR_CR_PLS_2_7	4
#define  STM_PWR_CR_PLS_2_9	5
#define  STM_PWR_CR_PLS_3_1	6
#define  STM_PWR_CR_PLS_EXT	7
#define  STM_PWR_CR_PLS_MASK	7

#define STM_PWR_CR_PVDE		(4)
#define STM_PWR_CR_CSBF		(3)
#define STM_PWR_CR_CWUF		(2)
#define STM_PWR_CR_PDDS		(1)
#define STM_PWR_CR_LPSDSR	(0)

#define STM_PWR_CSR_EWUP3	(10)
#define STM_PWR_CSR_EWUP2	(9)
#define STM_PWR_CSR_EWUP1	(8)
#define STM_PWR_CSR_REGLPF	(5)
#define STM_PWR_CSR_VOSF	(4)
#define STM_PWR_CSR_VREFINTRDYF	(3)
#define STM_PWR_CSR_PVDO	(2)
#define STM_PWR_CSR_SBF		(1)
#define STM_PWR_CSR_WUF		(0)

struct stm_tim67 {
	vuint32_t	cr1;
	vuint32_t	cr2;
	uint32_t	_unused_08;
	vuint32_t	dier;

	vuint32_t	sr;
	vuint32_t	egr;
	uint32_t	_unused_18;
	uint32_t	_unused_1c;

	uint32_t	_unused_20;
	vuint32_t	cnt;
	vuint32_t	psc;
	vuint32_t	arr;
};

extern struct stm_tim67 stm_tim6;

#define STM_TIM67_CR1_ARPE	(7)
#define STM_TIM67_CR1_OPM	(3)
#define STM_TIM67_CR1_URS	(2)
#define STM_TIM67_CR1_UDIS	(1)
#define STM_TIM67_CR1_CEN	(0)

#define STM_TIM67_CR2_MMS	(4)
#define  STM_TIM67_CR2_MMS_RESET	0
#define  STM_TIM67_CR2_MMS_ENABLE	1
#define  STM_TIM67_CR2_MMS_UPDATE	2
#define  STM_TIM67_CR2_MMS_MASK		7

#define STM_TIM67_DIER_UDE	(8)
#define STM_TIM67_DIER_UIE	(0)

#define STM_TIM67_SR_UIF	(0)

#define STM_TIM67_EGR_UG	(0)

struct stm_lcd {
	vuint32_t	cr;
	vuint32_t	fcr;
	vuint32_t	sr;
	vuint32_t	clr;
	uint32_t	unused_0x10;
	vuint32_t	ram[8*2];
};

extern struct stm_lcd stm_lcd;

#define STM_LCD_CR_MUX_SEG		(7)

#define STM_LCD_CR_BIAS			(5)
#define  STM_LCD_CR_BIAS_1_4		0
#define  STM_LCD_CR_BIAS_1_2		1
#define  STM_LCD_CR_BIAS_1_3		2
#define  STM_LCD_CR_BIAS_MASK		3

#define STM_LCD_CR_DUTY			(2)
#define  STM_LCD_CR_DUTY_STATIC		0
#define  STM_LCD_CR_DUTY_1_2		1
#define  STM_LCD_CR_DUTY_1_3		2
#define  STM_LCD_CR_DUTY_1_4		3
#define  STM_LCD_CR_DUTY_1_8		4
#define  STM_LCD_CR_DUTY_MASK		7

#define STM_LCD_CR_VSEL			(1)
#define STM_LCD_CR_LCDEN		(0)

#define STM_LCD_FCR_PS			(22)
#define  STM_LCD_FCR_PS_1		0x0
#define  STM_LCD_FCR_PS_2		0x1
#define  STM_LCD_FCR_PS_4		0x2
#define  STM_LCD_FCR_PS_8		0x3
#define  STM_LCD_FCR_PS_16		0x4
#define  STM_LCD_FCR_PS_32		0x5
#define  STM_LCD_FCR_PS_64		0x6
#define  STM_LCD_FCR_PS_128		0x7
#define  STM_LCD_FCR_PS_256		0x8
#define  STM_LCD_FCR_PS_512		0x9
#define  STM_LCD_FCR_PS_1024		0xa
#define  STM_LCD_FCR_PS_2048		0xb
#define  STM_LCD_FCR_PS_4096		0xc
#define  STM_LCD_FCR_PS_8192		0xd
#define  STM_LCD_FCR_PS_16384		0xe
#define  STM_LCD_FCR_PS_32768		0xf
#define  STM_LCD_FCR_PS_MASK		0xf

#define STM_LCD_FCR_DIV			(18)
#define STM_LCD_FCR_DIV_16		0x0
#define STM_LCD_FCR_DIV_17		0x1
#define STM_LCD_FCR_DIV_18		0x2
#define STM_LCD_FCR_DIV_19		0x3
#define STM_LCD_FCR_DIV_20		0x4
#define STM_LCD_FCR_DIV_21		0x5
#define STM_LCD_FCR_DIV_22		0x6
#define STM_LCD_FCR_DIV_23		0x7
#define STM_LCD_FCR_DIV_24		0x8
#define STM_LCD_FCR_DIV_25		0x9
#define STM_LCD_FCR_DIV_26		0xa
#define STM_LCD_FCR_DIV_27		0xb
#define STM_LCD_FCR_DIV_28		0xc
#define STM_LCD_FCR_DIV_29		0xd
#define STM_LCD_FCR_DIV_30		0xe
#define STM_LCD_FCR_DIV_31		0xf
#define STM_LCD_FCR_DIV_MASK		0xf

#define STM_LCD_FCR_BLINK		(16)
#define  STM_LCD_FCR_BLINK_DISABLE		0
#define  STM_LCD_FCR_BLINK_SEG0_COM0		1
#define  STM_LCD_FCR_BLINK_SEG0_COMALL		2
#define  STM_LCD_FCR_BLINK_SEGALL_COMALL	3
#define  STM_LCD_FCR_BLINK_MASK			3

#define STM_LCD_FCR_BLINKF		(13)
#define  STM_LCD_FCR_BLINKF_8			0
#define  STM_LCD_FCR_BLINKF_16			1
#define  STM_LCD_FCR_BLINKF_32			2
#define  STM_LCD_FCR_BLINKF_64			3
#define  STM_LCD_FCR_BLINKF_128			4
#define  STM_LCD_FCR_BLINKF_256			5
#define  STM_LCD_FCR_BLINKF_512			6
#define  STM_LCD_FCR_BLINKF_1024		7
#define  STM_LCD_FCR_BLINKF_MASK		7

#define STM_LCD_FCR_CC			(10)
#define  STM_LCD_FCR_CC_MASK			7

#define STM_LCD_FCR_DEAD		(7)
#define  STM_LCD_FCR_DEAD_MASK			7

#define STM_LCD_FCR_PON			(4)
#define  STM_LCD_FCR_PON_MASK			7

#define STM_LCD_FCR_UDDIE		(3)
#define STM_LCD_FCR_SOFIE		(1)
#define STM_LCD_FCR_HD			(0)

#define STM_LCD_SR_FCRSF		(5)
#define STM_LCD_SR_RDY			(4)
#define STM_LCD_SR_UDD			(3)
#define STM_LCD_SR_UDR			(2)
#define STM_LCD_SR_SOF			(1)
#define STM_LCD_SR_ENS			(0)

#define STM_LCD_CLR_UDDC		(3)
#define STM_LCD_CLR_SOFC		(1)

/* The SYSTICK starts at 0xe000e010 */

struct stm_systick {
	vuint32_t	csr;
	vuint32_t	rvr;
	vuint32_t	cvr;
	vuint32_t	calib;
};

extern struct stm_systick stm_systick;

#define STM_SYSTICK_CSR_ENABLE		0
#define STM_SYSTICK_CSR_TICKINT		1
#define STM_SYSTICK_CSR_CLKSOURCE	2
#define  STM_SYSTICK_CSR_CLKSOURCE_HCLK_8		0
#define  STM_SYSTICK_CSR_CLKSOURCE_HCLK			1
#define STM_SYSTICK_CSR_COUNTFLAG	16

/* The NVIC starts at 0xe000e100, so add that to the offsets to find the absolute address */

struct stm_nvic {
	vuint32_t	iser;		/* 0x000 0xe000e100 Set Enable Register */

	uint8_t		_unused020[0x080 - 0x004];

	vuint32_t	icer;		/* 0x080 0xe000e180 Clear Enable Register */

	uint8_t		_unused0a0[0x100 - 0x084];

	vuint32_t	ispr;		/* 0x100 0xe000e200 Set Pending Register */

	uint8_t		_unused120[0x180 - 0x104];

	vuint32_t	icpr;		/* 0x180 0xe000e280 Clear Pending Register */

	uint8_t		_unused1a0[0x300 - 0x184];

	vuint32_t	ipr[8];		/* 0x300 0xe000e400 Priority Register */
};

extern struct stm_nvic stm_nvic;

#define IRQ_MASK(irq)	(1 << (irq))
#define IRQ_BOOL(v,irq)	(((v) >> (irq)) & 1)

static inline void
stm_nvic_set_enable(int irq) {
	stm_nvic.iser = IRQ_MASK(irq);
}

static inline void
stm_nvic_clear_enable(int irq) {
	stm_nvic.icer = IRQ_MASK(irq);
}

static inline int
stm_nvic_enabled(int irq) {
	return IRQ_BOOL(stm_nvic.iser, irq);
}

static inline void
stm_nvic_set_pending(int irq) {
	stm_nvic.ispr = IRQ_MASK(irq);
}

static inline void
stm_nvic_clear_pending(int irq) {
	stm_nvic.icpr = IRQ_MASK(irq);
}

static inline int
stm_nvic_pending(int irq) {
	return IRQ_BOOL(stm_nvic.ispr, irq);
}

#define IRQ_PRIO_REG(irq)	((irq) >> 2)
#define IRQ_PRIO_BIT(irq)	(((irq) & 3) << 3)
#define IRQ_PRIO_MASK(irq)	(0xff << IRQ_PRIO_BIT(irq))

static inline void
stm_nvic_set_priority(int irq, uint8_t prio) {
	int		n = IRQ_PRIO_REG(irq);
	uint32_t	v;

	v = stm_nvic.ipr[n];
	v &= ~IRQ_PRIO_MASK(irq);
	v |= (prio) << IRQ_PRIO_BIT(irq);
	stm_nvic.ipr[n] = v;
}

static inline uint8_t
stm_nvic_get_priority(int irq) {
	return (stm_nvic.ipr[IRQ_PRIO_REG(irq)] >> IRQ_PRIO_BIT(irq)) & IRQ_PRIO_MASK(0);
}

struct stm_scb {
	vuint32_t	cpuid;
	vuint32_t	icsr;
	vuint32_t	vtor;
	vuint32_t	aircr;

	vuint32_t	scr;
	vuint32_t	ccr;
	vuint32_t	shpr1;
	vuint32_t	shpr2;

	vuint32_t	shpr3;
	vuint32_t	shcrs;
	vuint32_t	cfsr;
	vuint32_t	hfsr;

	uint32_t	unused_30;
	vuint32_t	mmfar;
	vuint32_t	bfar;
};

extern struct stm_scb stm_scb;

#define STM_SCB_AIRCR_VECTKEY		16
#define  STM_SCB_AIRCR_VECTKEY_KEY		0x05fa
#define STM_SCB_AIRCR_PRIGROUP		8
#define STM_SCB_AIRCR_SYSRESETREQ	2
#define STM_SCB_AIRCR_VECTCLRACTIVE	1
#define STM_SCB_AIRCR_VECTRESET		0

struct stm_mpu {
	vuint32_t	typer;
	vuint32_t	cr;
	vuint32_t	rnr;
	vuint32_t	rbar;

	vuint32_t	rasr;
	vuint32_t	rbar_a1;
	vuint32_t	rasr_a1;
	vuint32_t	rbar_a2;
	vuint32_t	rasr_a2;
	vuint32_t	rbar_a3;
	vuint32_t	rasr_a3;
};

extern struct stm_mpu stm_mpu;

#define STM_MPU_TYPER_IREGION	16
#define  STM_MPU_TYPER_IREGION_MASK	0xff
#define STM_MPU_TYPER_DREGION	8
#define  STM_MPU_TYPER_DREGION_MASK	0xff
#define STM_MPU_TYPER_SEPARATE	0

#define STM_MPU_CR_PRIVDEFENA	2
#define STM_MPU_CR_HFNMIENA	1
#define STM_MPU_CR_ENABLE	0

#define STM_MPU_RNR_REGION	0
#define STM_MPU_RNR_REGION_MASK		0xff

#define STM_MPU_RBAR_ADDR	5
#define STM_MPU_RBAR_ADDR_MASK		0x7ffffff

#define STM_MPU_RBAR_VALID	4
#define STM_MPU_RBAR_REGION	0
#define STM_MPU_RBAR_REGION_MASK	0xf

#define STM_MPU_RASR_XN		28
#define STM_MPU_RASR_AP		24
#define  STM_MPU_RASR_AP_NONE_NONE	0
#define  STM_MPU_RASR_AP_RW_NONE	1
#define  STM_MPU_RASR_AP_RW_RO		2
#define  STM_MPU_RASR_AP_RW_RW		3
#define  STM_MPU_RASR_AP_RO_NONE	5
#define  STM_MPU_RASR_AP_RO_RO		6
#define  STM_MPU_RASR_AP_MASK		7
#define STM_MPU_RASR_TEX	19
#define  STM_MPU_RASR_TEX_MASK		7
#define STM_MPU_RASR_S		18
#define STM_MPU_RASR_C		17
#define STM_MPU_RASR_B		16
#define STM_MPU_RASR_SRD	8
#define  STM_MPU_RASR_SRD_MASK		0xff
#define STM_MPU_RASR_SIZE	1
#define  STM_MPU_RASR_SIZE_MASK		0x1f
#define STM_MPU_RASR_ENABLE	0

#define isr_decl(name) void stm_ ## name ## _isr(void)

isr_decl(halt);
isr_decl(ignore);

isr_decl(nmi);
isr_decl(hardfault);
isr_decl(usagefault);
isr_decl(svc);
isr_decl(debugmon);
isr_decl(pendsv);
isr_decl(systick);
isr_decl(wwdg);
isr_decl(pvd);
isr_decl(rtc);
isr_decl(flash);
isr_decl(rcc_crs);
isr_decl(exti1_0);
isr_decl(exti3_2);
isr_decl(exti15_4);
isr_decl(dma1_channel1);
isr_decl(dma1_channel3_2);
isr_decl(dma1_channel7_4);
isr_decl(adc_comp);
isr_decl(lptim1);
isr_decl(usart4_usart5);
isr_decl(tim2);
isr_decl(tim3);
isr_decl(tim4);
isr_decl(tim6);
isr_decl(tim7);
isr_decl(tim21);
isr_decl(i2c3);
isr_decl(tim22);
isr_decl(i2c1);
isr_decl(i2c2);
isr_decl(spi1);
isr_decl(spi2);
isr_decl(usart1);
isr_decl(usart2);
isr_decl(usart3);
isr_decl(lpuart1_aes);

#undef isr_decl

#define STM_ISR_WWDG_POS		0
#define STM_ISR_PVD_POS			1
#define STM_ISR_RTC_POS			2
#define STM_ISR_FLASH_POS		3
#define STM_ISR_RCC_CRS_POS		4
#define STM_ISR_EXTI1_0_POS		5
#define STM_ISR_EXTI3_2_POS		6
#define STM_ISR_EXTI15_4_POS		7
#define STM_ISR_DMA1_CHANNEL1_POS	9
#define STM_ISR_DMA1_CHANNEL3_2_POS	10
#define STM_ISR_DMA1_CHANNEL7_4_POS	11
#define STM_ISR_ADC_COMP_POS		12
#define STM_ISR_LPTIM1_POS		13
#define STM_ISR_USART4_USART5_POS	14
#define STM_ISR_TIM2_POS		15
#define STM_ISR_TIM3_POS		16
#define STM_ISR_TIM6_POS		17
#define STM_ISR_TIM7_POS		18
#define STM_ISR_TIM21_POS		20
#define STM_ISR_I2C3_POS		21
#define STM_ISR_TIM22_POS		22
#define STM_ISR_I2C1_POS		23
#define STM_ISR_I2C2_POS		24
#define STM_ISR_SPI1_POS		25
#define STM_ISR_SPI2_POS		26
#define STM_ISR_USART1_POS		27
#define STM_ISR_USART2_POS		28
#define STM_ISR_LPUART1_AES_POS		29

struct stm_syscfg {
	vuint32_t	memrmp;
	vuint32_t	pmc;
	vuint32_t	exticr[4];
};

extern struct stm_syscfg stm_syscfg;

#define STM_SYSCFG_MEMRMP_MEM_MODE	0
#define  STM_SYSCFG_MEMRMP_MEM_MODE_MAIN_FLASH		0
#define  STM_SYSCFG_MEMRMP_MEM_MODE_SYSTEM_FLASH	1
#define  STM_SYSCFG_MEMRMP_MEM_MODE_SRAM		3
#define  STM_SYSCFG_MEMRMP_MEM_MODE_MASK		3

#define STM_SYSCFG_PMC_USB_PU		0

#define STM_SYSCFG_EXTICR_PA		0
#define STM_SYSCFG_EXTICR_PB		1
#define STM_SYSCFG_EXTICR_PC		2
#define STM_SYSCFG_EXTICR_PD		3
#define STM_SYSCFG_EXTICR_PE		4
#define STM_SYSCFG_EXTICR_PH		5

static inline void
stm_exticr_set(struct stm_gpio *gpio, int pin) {
	uint8_t	reg = pin >> 2;
	uint8_t	shift = (pin & 3) << 2;
	uint8_t	val = 0;

	/* Enable SYSCFG */
	stm_rcc.apb2enr |= (1 << STM_RCC_APB2ENR_SYSCFGEN);

	if (gpio == &stm_gpioa)
		val = STM_SYSCFG_EXTICR_PA;
	else if (gpio == &stm_gpiob)
		val = STM_SYSCFG_EXTICR_PB;
	else if (gpio == &stm_gpioc)
		val = STM_SYSCFG_EXTICR_PC;
	else if (gpio == &stm_gpiod)
		val = STM_SYSCFG_EXTICR_PD;
	else if (gpio == &stm_gpioe)
		val = STM_SYSCFG_EXTICR_PE;

	stm_syscfg.exticr[reg] = (stm_syscfg.exticr[reg] & ~(0xf << shift)) | val << shift;
}

struct stm_dma_channel {
	vuint32_t	ccr;
	vuint32_t	cndtr;
	vvoid_t		cpar;
	vvoid_t		cmar;
	vuint32_t	reserved;
};

#define STM_NUM_DMA	7

struct stm_dma {
	vuint32_t		isr;
	vuint32_t		ifcr;
	struct stm_dma_channel	channel[STM_NUM_DMA];
};

extern struct stm_dma stm_dma;

/* DMA channels go from 1 to 7, instead of 0 to 6 (sigh)
 */

#define STM_DMA_INDEX(channel)		((channel) - 1)

#define STM_DMA_ISR(index)		((index) << 2)
#define STM_DMA_ISR_MASK			0xf
#define STM_DMA_ISR_TEIF			3
#define STM_DMA_ISR_HTIF			2
#define STM_DMA_ISR_TCIF			1
#define STM_DMA_ISR_GIF				0

#define STM_DMA_IFCR(index)		((index) << 2)
#define STM_DMA_IFCR_MASK			0xf
#define STM_DMA_IFCR_CTEIF      		3
#define STM_DMA_IFCR_CHTIF			2
#define STM_DMA_IFCR_CTCIF			1
#define STM_DMA_IFCR_CGIF			0

#define STM_DMA_CCR_MEM2MEM		(14)

#define STM_DMA_CCR_PL			(12)
#define  STM_DMA_CCR_PL_LOW			(0)
#define  STM_DMA_CCR_PL_MEDIUM			(1)
#define  STM_DMA_CCR_PL_HIGH			(2)
#define  STM_DMA_CCR_PL_VERY_HIGH		(3)
#define  STM_DMA_CCR_PL_MASK			(3)

#define STM_DMA_CCR_MSIZE		(10)
#define  STM_DMA_CCR_MSIZE_8			(0)
#define  STM_DMA_CCR_MSIZE_16			(1)
#define  STM_DMA_CCR_MSIZE_32			(2)
#define  STM_DMA_CCR_MSIZE_MASK			(3)

#define STM_DMA_CCR_PSIZE		(8)
#define  STM_DMA_CCR_PSIZE_8			(0)
#define  STM_DMA_CCR_PSIZE_16			(1)
#define  STM_DMA_CCR_PSIZE_32			(2)
#define  STM_DMA_CCR_PSIZE_MASK			(3)

#define STM_DMA_CCR_MINC		(7)
#define STM_DMA_CCR_PINC		(6)
#define STM_DMA_CCR_CIRC		(5)
#define STM_DMA_CCR_DIR			(4)
#define  STM_DMA_CCR_DIR_PER_TO_MEM		0
#define  STM_DMA_CCR_DIR_MEM_TO_PER		1
#define STM_DMA_CCR_TEIE		(3)
#define STM_DMA_CCR_HTIE		(2)
#define STM_DMA_CCR_TCIE		(1)
#define STM_DMA_CCR_EN			(0)

#define STM_DMA_CHANNEL_ADC1		1
#define STM_DMA_CHANNEL_SPI1_RX		2
#define STM_DMA_CHANNEL_SPI1_TX		3
#define STM_DMA_CHANNEL_SPI2_RX		4
#define STM_DMA_CHANNEL_SPI2_TX		5
#define STM_DMA_CHANNEL_USART3_TX	2
#define STM_DMA_CHANNEL_USART3_RX	3
#define STM_DMA_CHANNEL_USART1_TX	4
#define STM_DMA_CHANNEL_USART1_RX	5
#define STM_DMA_CHANNEL_USART2_RX	6
#define STM_DMA_CHANNEL_USART2_TX	7
#define STM_DMA_CHANNEL_I2C2_TX		4
#define STM_DMA_CHANNEL_I2C2_RX		5
#define STM_DMA_CHANNEL_I2C1_TX		6
#define STM_DMA_CHANNEL_I2C1_RX		7
#define STM_DMA_CHANNEL_TIM2_CH3	1
#define STM_DMA_CHANNEL_TIM2_UP		2
#define STM_DMA_CHANNEL_TIM2_CH1	5
#define STM_DMA_CHANNEL_TIM2_CH2	7
#define STM_DMA_CHANNEL_TIM2_CH4	7
#define STM_DMA_CHANNEL_TIM3_CH3	2
#define STM_DMA_CHANNEL_TIM3_CH4	3
#define STM_DMA_CHANNEL_TIM3_UP		3
#define STM_DMA_CHANNEL_TIM3_CH1	6
#define STM_DMA_CHANNEL_TIM3_TRIG	6
#define STM_DMA_CHANNEL_TIM4_CH1	1
#define STM_DMA_CHANNEL_TIM4_CH2	4
#define STM_DMA_CHANNEL_TIM4_CH3	5
#define STM_DMA_CHANNEL_TIM4_UP		7
#define STM_DMA_CHANNEL_TIM6_UP_DA	2
#define STM_DMA_CHANNEL_C_CHANNEL1	2
#define STM_DMA_CHANNEL_TIM7_UP_DA	3
#define STM_DMA_CHANNEL_C_CHANNEL2	3

/*
 * Only spi channel 1 and 2 can use DMA
 */
#define STM_NUM_SPI	2

struct stm_spi {
	vuint32_t	cr1;
	vuint32_t	cr2;
	vuint32_t	sr;
	vuint32_t	dr;
	vuint32_t	crcpr;
	vuint32_t	rxcrcr;
	vuint32_t	txcrcr;
};

extern struct stm_spi stm_spi1, stm_spi2, stm_spi3;

/* SPI channels go from 1 to 3, instead of 0 to 2 (sigh)
 */

#define STM_SPI_INDEX(channel)		((channel) - 1)

#define STM_SPI_CR1_BIDIMODE		15
#define STM_SPI_CR1_BIDIOE		14
#define STM_SPI_CR1_CRCEN		13
#define STM_SPI_CR1_CRCNEXT		12
#define STM_SPI_CR1_DFF			11
#define STM_SPI_CR1_RXONLY		10
#define STM_SPI_CR1_SSM			9
#define STM_SPI_CR1_SSI			8
#define STM_SPI_CR1_LSBFIRST		7
#define STM_SPI_CR1_SPE			6
#define STM_SPI_CR1_BR			3
#define  STM_SPI_CR1_BR_PCLK_2			0
#define  STM_SPI_CR1_BR_PCLK_4			1
#define  STM_SPI_CR1_BR_PCLK_8			2
#define  STM_SPI_CR1_BR_PCLK_16			3
#define  STM_SPI_CR1_BR_PCLK_32			4
#define  STM_SPI_CR1_BR_PCLK_64			5
#define  STM_SPI_CR1_BR_PCLK_128		6
#define  STM_SPI_CR1_BR_PCLK_256		7
#define  STM_SPI_CR1_BR_MASK			7

#define STM_SPI_CR1_MSTR		2
#define STM_SPI_CR1_CPOL		1
#define STM_SPI_CR1_CPHA		0

#define STM_SPI_CR2_TXEIE	7
#define STM_SPI_CR2_RXNEIE	6
#define STM_SPI_CR2_ERRIE	5
#define STM_SPI_CR2_SSOE	2
#define STM_SPI_CR2_TXDMAEN	1
#define STM_SPI_CR2_RXDMAEN	0

#define STM_SPI_SR_FRE		8
#define STM_SPI_SR_BSY		7
#define STM_SPI_SR_OVR		6
#define STM_SPI_SR_MODF		5
#define STM_SPI_SR_CRCERR	4
#define STM_SPI_SR_UDR		3
#define STM_SPI_SR_CHSIDE	2
#define STM_SPI_SR_TXE		1
#define STM_SPI_SR_RXNE		0

struct stm_adc {
	vuint32_t	sr;
	vuint32_t	cr1;
	vuint32_t	cr2;
	vuint32_t	smpr1;
	vuint32_t	smpr2;
	vuint32_t	smpr3;
	vuint32_t	jofr1;
	vuint32_t	jofr2;
	vuint32_t	jofr3;
	vuint32_t	jofr4;
	vuint32_t	htr;
	vuint32_t	ltr;
	vuint32_t	sqr1;
	vuint32_t	sqr2;
	vuint32_t	sqr3;
	vuint32_t	sqr4;
	vuint32_t	sqr5;
	vuint32_t	jsqr;
	vuint32_t	jdr1;
	vuint32_t	jdr2;
	vuint32_t	jdr3;
	vuint32_t	jdr4;
	vuint32_t	dr;
	uint8_t		reserved[0x300 - 0x5c];
	vuint32_t	csr;
	vuint32_t	ccr;
};

extern struct stm_adc stm_adc;

#define STM_ADC_SQ_TEMP		16
#define STM_ADC_SQ_V_REF	17

#define STM_ADC_SR_JCNR		9
#define STM_ADC_SR_RCNR		8
#define STM_ADC_SR_ADONS	6
#define STM_ADC_SR_OVR		5
#define STM_ADC_SR_STRT		4
#define STM_ADC_SR_JSTRT	3
#define STM_ADC_SR_JEOC		2
#define STM_ADC_SR_EOC		1
#define STM_ADC_SR_AWD		0

#define STM_ADC_CR1_OVRIE	26
#define STM_ADC_CR1_RES		24
#define  STM_ADC_CR1_RES_12		0
#define  STM_ADC_CR1_RES_10		1
#define  STM_ADC_CR1_RES_8		2
#define  STM_ADC_CR1_RES_6		3
#define  STM_ADC_CR1_RES_MASK		3
#define STM_ADC_CR1_AWDEN       23
#define STM_ADC_CR1_JAWDEN	22
#define STM_ADC_CR1_PDI		17
#define STM_ADC_CR1_PDD		16
#define STM_ADC_CR1_DISCNUM	13
#define  STM_ADC_CR1_DISCNUM_1		0
#define  STM_ADC_CR1_DISCNUM_2		1
#define  STM_ADC_CR1_DISCNUM_3		2
#define  STM_ADC_CR1_DISCNUM_4		3
#define  STM_ADC_CR1_DISCNUM_5		4
#define  STM_ADC_CR1_DISCNUM_6		5
#define  STM_ADC_CR1_DISCNUM_7		6
#define  STM_ADC_CR1_DISCNUM_8		7
#define  STM_ADC_CR1_DISCNUM_MASK	7
#define STM_ADC_CR1_JDISCEN	12
#define STM_ADC_CR1_DISCEN	11
#define STM_ADC_CR1_JAUTO	10
#define STM_ADC_CR1_AWDSGL	9
#define STM_ADC_CR1_SCAN	8
#define STM_ADC_CR1_JEOCIE	7
#define STM_ADC_CR1_AWDIE	6
#define STM_ADC_CR1_EOCIE	5
#define STM_ADC_CR1_AWDCH	0
#define  STM_ADC_CR1_AWDCH_MASK		0x1f

#define STM_ADC_CR2_SWSTART	30
#define STM_ADC_CR2_EXTEN	28
#define  STM_ADC_CR2_EXTEN_DISABLE	0
#define  STM_ADC_CR2_EXTEN_RISING	1
#define  STM_ADC_CR2_EXTEN_FALLING	2
#define  STM_ADC_CR2_EXTEN_BOTH		3
#define  STM_ADC_CR2_EXTEN_MASK		3
#define STM_ADC_CR2_EXTSEL	24
#define  STM_ADC_CR2_EXTSEL_TIM9_CC2	0
#define  STM_ADC_CR2_EXTSEL_TIM9_TRGO	1
#define  STM_ADC_CR2_EXTSEL_TIM2_CC3	2
#define  STM_ADC_CR2_EXTSEL_TIM2_CC2	3
#define  STM_ADC_CR2_EXTSEL_TIM3_TRGO	4
#define  STM_ADC_CR2_EXTSEL_TIM4_CC4	5
#define  STM_ADC_CR2_EXTSEL_TIM2_TRGO	6
#define  STM_ADC_CR2_EXTSEL_TIM3_CC1	7
#define  STM_ADC_CR2_EXTSEL_TIM3_CC3	8
#define  STM_ADC_CR2_EXTSEL_TIM4_TRGO	9
#define  STM_ADC_CR2_EXTSEL_TIM6_TRGO	10
#define  STM_ADC_CR2_EXTSEL_EXTI_11	15
#define  STM_ADC_CR2_EXTSEL_MASK	15
#define STM_ADC_CR2_JWSTART	22
#define STM_ADC_CR2_JEXTEN	20
#define  STM_ADC_CR2_JEXTEN_DISABLE	0
#define  STM_ADC_CR2_JEXTEN_RISING	1
#define  STM_ADC_CR2_JEXTEN_FALLING	2
#define  STM_ADC_CR2_JEXTEN_BOTH	3
#define  STM_ADC_CR2_JEXTEN_MASK	3
#define STM_ADC_CR2_JEXTSEL	16
#define  STM_ADC_CR2_JEXTSEL_TIM9_CC1	0
#define  STM_ADC_CR2_JEXTSEL_TIM9_TRGO	1
#define  STM_ADC_CR2_JEXTSEL_TIM2_TRGO	2
#define  STM_ADC_CR2_JEXTSEL_TIM2_CC1	3
#define  STM_ADC_CR2_JEXTSEL_TIM3_CC4	4
#define  STM_ADC_CR2_JEXTSEL_TIM4_TRGO	5
#define  STM_ADC_CR2_JEXTSEL_TIM4_CC1	6
#define  STM_ADC_CR2_JEXTSEL_TIM4_CC2	7
#define  STM_ADC_CR2_JEXTSEL_TIM4_CC3	8
#define  STM_ADC_CR2_JEXTSEL_TIM10_CC1	9
#define  STM_ADC_CR2_JEXTSEL_TIM7_TRGO	10
#define  STM_ADC_CR2_JEXTSEL_EXTI_15	15
#define  STM_ADC_CR2_JEXTSEL_MASK	15
#define STM_ADC_CR2_ALIGN	11
#define STM_ADC_CR2_EOCS	10
#define STM_ADC_CR2_DDS		9
#define STM_ADC_CR2_DMA		8
#define STM_ADC_CR2_DELS	4
#define  STM_ADC_CR2_DELS_NONE		0
#define  STM_ADC_CR2_DELS_UNTIL_READ	1
#define  STM_ADC_CR2_DELS_7		2
#define  STM_ADC_CR2_DELS_15		3
#define  STM_ADC_CR2_DELS_31		4
#define  STM_ADC_CR2_DELS_63		5
#define  STM_ADC_CR2_DELS_127		6
#define  STM_ADC_CR2_DELS_255		7
#define  STM_ADC_CR2_DELS_MASK		7
#define STM_ADC_CR2_CONT	1
#define STM_ADC_CR2_ADON	0

#define STM_ADC_CCR_TSVREFE	23
#define STM_ADC_CCR_ADCPRE	16
#define  STM_ADC_CCR_ADCPRE_HSI_1	0
#define  STM_ADC_CCR_ADCPRE_HSI_2	1
#define  STM_ADC_CCR_ADCPRE_HSI_4	2
#define  STM_ADC_CCR_ADCPRE_MASK	3

struct stm_temp_cal {
	uint16_t	vref;
	uint16_t	ts_cal_cold;
	uint16_t	reserved;
	uint16_t	ts_cal_hot;
};

extern struct stm_temp_cal	stm_temp_cal;

#define stm_temp_cal_cold	25
#define stm_temp_cal_hot	110

struct stm_dbg_mcu {
	uint32_t	idcode;
};

extern struct stm_dbg_mcu	stm_dbg_mcu;

static inline uint16_t
stm_dev_id(void) {
	return stm_dbg_mcu.idcode & 0xfff;
}

struct stm_flash_size {
	uint16_t	f_size;
};

extern struct stm_flash_size	stm_flash_size_reg;
#define stm_flash_size_reg	(*((struct stm_flash_size *) 0x1ff8007c))

/* Returns flash size in bytes */
extern uint32_t
stm_flash_size(void);

struct stm_unique_id {
	uint32_t	u_id0;
	uint32_t	u_id1;
	uint32_t	u_id2;
};

extern struct stm_unique_id	stm_unique_id;
#define stm_unique_id	(*((struct stm_unique_id) 0x1ff80050))

struct stm_device_id {
	uint32_t	device_id;
};

extern struct stm_device_id	stm_device_id;
#define stm_device_id	(*((struct stm_device_id) 0x40015800))

#define STM_NUM_I2C	2

#define STM_I2C_INDEX(channel)	((channel) - 1)

struct stm_i2c {
	vuint32_t	cr1;
	vuint32_t	cr2;
	vuint32_t	oar1;
	vuint32_t	oar2;
	vuint32_t	dr;
	vuint32_t	sr1;
	vuint32_t	sr2;
	vuint32_t	ccr;
	vuint32_t	trise;
};

extern struct stm_i2c stm_i2c1, stm_i2c2;

#define STM_I2C_CR1_SWRST	15
#define STM_I2C_CR1_ALERT	13
#define STM_I2C_CR1_PEC		12
#define STM_I2C_CR1_POS		11
#define STM_I2C_CR1_ACK		10
#define STM_I2C_CR1_STOP	9
#define STM_I2C_CR1_START	8
#define STM_I2C_CR1_NOSTRETCH	7
#define STM_I2C_CR1_ENGC	6
#define STM_I2C_CR1_ENPEC	5
#define STM_I2C_CR1_ENARP	4
#define STM_I2C_CR1_SMBTYPE	3
#define STM_I2C_CR1_SMBUS	1
#define STM_I2C_CR1_PE		0

#define STM_I2C_CR2_LAST	12
#define STM_I2C_CR2_DMAEN	11
#define STM_I2C_CR2_ITBUFEN	10
#define STM_I2C_CR2_ITEVTEN	9
#define STM_I2C_CR2_ITERREN	8
#define STM_I2C_CR2_FREQ	0
#define  STM_I2C_CR2_FREQ_2_MHZ		2
#define  STM_I2C_CR2_FREQ_4_MHZ		4
#define  STM_I2C_CR2_FREQ_8_MHZ		8
#define  STM_I2C_CR2_FREQ_16_MHZ	16
#define  STM_I2C_CR2_FREQ_24_MHZ	24
#define  STM_I2C_CR2_FREQ_32_MHZ	32
#define  STM_I2C_CR2_FREQ_MASK		0x3f

#define STM_I2C_SR1_SMBALERT	15
#define STM_I2C_SR1_TIMEOUT	14
#define STM_I2C_SR1_PECERR	12
#define STM_I2C_SR1_OVR		11
#define STM_I2C_SR1_AF		10
#define STM_I2C_SR1_ARLO	9
#define STM_I2C_SR1_BERR	8
#define STM_I2C_SR1_TXE		7
#define STM_I2C_SR1_RXNE	6
#define STM_I2C_SR1_STOPF	4
#define STM_I2C_SR1_ADD10	3
#define STM_I2C_SR1_BTF		2
#define STM_I2C_SR1_ADDR	1
#define STM_I2C_SR1_SB		0

#define STM_I2C_SR2_PEC		8
#define  STM_I2C_SR2_PEC_MASK	0xff00
#define STM_I2C_SR2_DUALF	7
#define STM_I2C_SR2_SMBHOST	6
#define STM_I2C_SR2_SMBDEFAULT	5
#define STM_I2C_SR2_GENCALL	4
#define STM_I2C_SR2_TRA		2
#define STM_I2C_SR2_BUSY       	1
#define STM_I2C_SR2_MSL		0

#define STM_I2C_CCR_FS		15
#define STM_I2C_CCR_DUTY	14
#define STM_I2C_CCR_CCR		0
#define  STM_I2C_CCR_MASK	0x7ff

struct stm_tim234 {
	vuint32_t	cr1;
	vuint32_t	cr2;
	vuint32_t	smcr;
	vuint32_t	dier;

	vuint32_t	sr;
	vuint32_t	egr;
	vuint32_t	ccmr1;
	vuint32_t	ccmr2;

	vuint32_t	ccer;
	vuint32_t	cnt;
	vuint32_t	psc;
	vuint32_t	arr;

	uint32_t	reserved_30;
	vuint32_t	ccr1;
	vuint32_t	ccr2;
	vuint32_t	ccr3;

	vuint32_t	ccr4;
	uint32_t	reserved_44;
	vuint32_t	dcr;
	vuint32_t	dmar;

	uint32_t	reserved_50;
};

extern struct stm_tim234 stm_tim2, stm_tim3, stm_tim4;

#define STM_TIM234_CR1_CKD	8
#define  STM_TIM234_CR1_CKD_1		0
#define  STM_TIM234_CR1_CKD_2		1
#define  STM_TIM234_CR1_CKD_4		2
#define  STM_TIM234_CR1_CKD_MASK	3
#define STM_TIM234_CR1_ARPE	7
#define STM_TIM234_CR1_CMS	5
#define  STM_TIM234_CR1_CMS_EDGE	0
#define  STM_TIM234_CR1_CMS_CENTER_1	1
#define  STM_TIM234_CR1_CMS_CENTER_2	2
#define  STM_TIM234_CR1_CMS_CENTER_3	3
#define  STM_TIM234_CR1_CMS_MASK	3
#define STM_TIM234_CR1_DIR	4
#define  STM_TIM234_CR1_DIR_UP		0
#define  STM_TIM234_CR1_DIR_DOWN	1
#define STM_TIM234_CR1_OPM	3
#define STM_TIM234_CR1_URS	2
#define STM_TIM234_CR1_UDIS	1
#define STM_TIM234_CR1_CEN	0

#define STM_TIM234_CR2_TI1S	7
#define STM_TIM234_CR2_MMS	4
#define  STM_TIM234_CR2_MMS_RESET		0
#define  STM_TIM234_CR2_MMS_ENABLE		1
#define  STM_TIM234_CR2_MMS_UPDATE		2
#define  STM_TIM234_CR2_MMS_COMPARE_PULSE	3
#define  STM_TIM234_CR2_MMS_COMPARE_OC1REF	4
#define  STM_TIM234_CR2_MMS_COMPARE_OC2REF	5
#define  STM_TIM234_CR2_MMS_COMPARE_OC3REF	6
#define  STM_TIM234_CR2_MMS_COMPARE_OC4REF	7
#define  STM_TIM234_CR2_MMS_MASK		7
#define STM_TIM234_CR2_CCDS	3

#define STM_TIM234_SMCR_ETP	15
#define STM_TIM234_SMCR_ECE	14
#define STM_TIM234_SMCR_ETPS	12
#define  STM_TIM234_SMCR_ETPS_OFF		0
#define  STM_TIM234_SMCR_ETPS_DIV_2	       	1
#define  STM_TIM234_SMCR_ETPS_DIV_4		2
#define  STM_TIM234_SMCR_ETPS_DIV_8		3
#define  STM_TIM234_SMCR_ETPS_MASK		3
#define STM_TIM234_SMCR_ETF	8
#define  STM_TIM234_SMCR_ETF_NONE		0
#define  STM_TIM234_SMCR_ETF_INT_N_2		1
#define  STM_TIM234_SMCR_ETF_INT_N_4		2
#define  STM_TIM234_SMCR_ETF_INT_N_8		3
#define  STM_TIM234_SMCR_ETF_DTS_2_N_6		4
#define  STM_TIM234_SMCR_ETF_DTS_2_N_8		5
#define  STM_TIM234_SMCR_ETF_DTS_4_N_6		6
#define  STM_TIM234_SMCR_ETF_DTS_4_N_8		7
#define  STM_TIM234_SMCR_ETF_DTS_8_N_6		8
#define  STM_TIM234_SMCR_ETF_DTS_8_N_8		9
#define  STM_TIM234_SMCR_ETF_DTS_16_N_5		10
#define  STM_TIM234_SMCR_ETF_DTS_16_N_6		11
#define  STM_TIM234_SMCR_ETF_DTS_16_N_8		12
#define  STM_TIM234_SMCR_ETF_DTS_32_N_5		13
#define  STM_TIM234_SMCR_ETF_DTS_32_N_6		14
#define  STM_TIM234_SMCR_ETF_DTS_32_N_8		15
#define  STM_TIM234_SMCR_ETF_MASK		15
#define STM_TIM234_SMCR_MSM	7
#define STM_TIM234_SMCR_TS	4
#define  STM_TIM234_SMCR_TS_ITR0		0
#define  STM_TIM234_SMCR_TS_ITR1		1
#define  STM_TIM234_SMCR_TS_ITR2		2
#define  STM_TIM234_SMCR_TS_ITR3		3
#define  STM_TIM234_SMCR_TS_TI1F_ED		4
#define  STM_TIM234_SMCR_TS_TI1FP1		5
#define  STM_TIM234_SMCR_TS_TI2FP2		6
#define  STM_TIM234_SMCR_TS_ETRF		7
#define  STM_TIM234_SMCR_TS_MASK		7
#define STM_TIM234_SMCR_OCCS	3
#define STM_TIM234_SMCR_SMS	0
#define  STM_TIM234_SMCR_SMS_DISABLE		0
#define  STM_TIM234_SMCR_SMS_ENCODER_MODE_1	1
#define  STM_TIM234_SMCR_SMS_ENCODER_MODE_2	2
#define  STM_TIM234_SMCR_SMS_ENCODER_MODE_3	3
#define  STM_TIM234_SMCR_SMS_RESET_MODE		4
#define  STM_TIM234_SMCR_SMS_GATED_MODE		5
#define  STM_TIM234_SMCR_SMS_TRIGGER_MODE	6
#define  STM_TIM234_SMCR_SMS_EXTERNAL_CLOCK	7
#define  STM_TIM234_SMCR_SMS_MASK		7

#define STM_TIM234_DIER_TDE		14
#define STM_TIM234_DIER_CC4DE		12
#define STM_TIM234_DIER_CC3DE		11
#define STM_TIM234_DIER_CC2DE		10
#define STM_TIM234_DIER_CC1DE		9
#define STM_TIM234_DIER_UDE		8

#define STM_TIM234_DIER_TIE		6
#define STM_TIM234_DIER_CC4IE		4
#define STM_TIM234_DIER_CC3IE		3
#define STM_TIM234_DIER_CC2IE		2
#define STM_TIM234_DIER_CC1IE		1
#define STM_TIM234_DIER_UIE		0

#define STM_TIM234_SR_CC4OF	12
#define STM_TIM234_SR_CC3OF	11
#define STM_TIM234_SR_CC2OF	10
#define STM_TIM234_SR_CC1OF	9
#define STM_TIM234_SR_TIF	6
#define STM_TIM234_SR_CC4IF	4
#define STM_TIM234_SR_CC3IF	3
#define STM_TIM234_SR_CC2IF	2
#define STM_TIM234_SR_CC1IF	1
#define STM_TIM234_SR_UIF	0

#define STM_TIM234_EGR_TG	6
#define STM_TIM234_EGR_CC4G	4
#define STM_TIM234_EGR_CC3G	3
#define STM_TIM234_EGR_CC2G	2
#define STM_TIM234_EGR_CC1G	1
#define STM_TIM234_EGR_UG	0

#define STM_TIM234_CCMR1_OC2CE	15
#define STM_TIM234_CCMR1_OC2M	12
#define  STM_TIM234_CCMR1_OC2M_FROZEN			0
#define  STM_TIM234_CCMR1_OC2M_SET_HIGH_ON_MATCH	1
#define  STM_TIM234_CCMR1_OC2M_SET_LOW_ON_MATCH		2
#define  STM_TIM234_CCMR1_OC2M_TOGGLE			3
#define  STM_TIM234_CCMR1_OC2M_FORCE_LOW		4
#define  STM_TIM234_CCMR1_OC2M_FORCE_HIGH		5
#define  STM_TIM234_CCMR1_OC2M_PWM_MODE_1		6
#define  STM_TIM234_CCMR1_OC2M_PWM_MODE_2		7
#define  STM_TIM234_CCMR1_OC2M_MASK			7
#define STM_TIM234_CCMR1_OC2PE	11
#define STM_TIM234_CCMR1_OC2FE	10
#define STM_TIM234_CCMR1_CC2S	8
#define  STM_TIM234_CCMR1_CC2S_OUTPUT			0
#define  STM_TIM234_CCMR1_CC2S_INPUT_TI2		1
#define  STM_TIM234_CCMR1_CC2S_INPUT_TI1		2
#define  STM_TIM234_CCMR1_CC2S_INPUT_TRC		3
#define  STM_TIM234_CCMR1_CC2S_MASK			3

#define STM_TIM234_CCMR1_OC1CE	7
#define STM_TIM234_CCMR1_OC1M	4
#define  STM_TIM234_CCMR1_OC1M_FROZEN			0
#define  STM_TIM234_CCMR1_OC1M_SET_HIGH_ON_MATCH	1
#define  STM_TIM234_CCMR1_OC1M_SET_LOW_ON_MATCH		2
#define  STM_TIM234_CCMR1_OC1M_TOGGLE			3
#define  STM_TIM234_CCMR1_OC1M_FORCE_LOW		4
#define  STM_TIM234_CCMR1_OC1M_FORCE_HIGH		5
#define  STM_TIM234_CCMR1_OC1M_PWM_MODE_1		6
#define  STM_TIM234_CCMR1_OC1M_PWM_MODE_2		7
#define  STM_TIM234_CCMR1_OC1M_MASK			7
#define STM_TIM234_CCMR1_OC1PE	3
#define STM_TIM234_CCMR1_OC1FE	2
#define STM_TIM234_CCMR1_CC1S	0
#define  STM_TIM234_CCMR1_CC1S_OUTPUT			0
#define  STM_TIM234_CCMR1_CC1S_INPUT_TI1		1
#define  STM_TIM234_CCMR1_CC1S_INPUT_TI2		2
#define  STM_TIM234_CCMR1_CC1S_INPUT_TRC		3
#define  STM_TIM234_CCMR1_CC1S_MASK			3

#define STM_TIM234_CCMR1_IC2F	12
#define  STM_TIM234_CCMR1_IC2F_NONE			0
#define  STM_TIM234_CCMR1_IC2F_CK_INT_N_2		1
#define  STM_TIM234_CCMR1_IC2F_CK_INT_N_4		2
#define  STM_TIM234_CCMR1_IC2F_CK_INT_N_8		3
#define  STM_TIM234_CCMR1_IC2F_DTS_2_N_6		4
#define  STM_TIM234_CCMR1_IC2F_DTS_2_N_8		5
#define  STM_TIM234_CCMR1_IC2F_DTS_4_N_6		6
#define  STM_TIM234_CCMR1_IC2F_DTS_4_N_8		7
#define  STM_TIM234_CCMR1_IC2F_DTS_8_N_6		8
#define  STM_TIM234_CCMR1_IC2F_DTS_8_N_8		9
#define  STM_TIM234_CCMR1_IC2F_DTS_16_N_5		10
#define  STM_TIM234_CCMR1_IC2F_DTS_16_N_6		11
#define  STM_TIM234_CCMR1_IC2F_DTS_16_N_8		12
#define  STM_TIM234_CCMR1_IC2F_DTS_32_N_5		13
#define  STM_TIM234_CCMR1_IC2F_DTS_32_N_6		14
#define  STM_TIM234_CCMR1_IC2F_DTS_32_N_8		15
#define STM_TIM234_CCMR1_IC2PSC	10
#define  STM_TIM234_CCMR1_IC2PSC_NONE			0
#define  STM_TIM234_CCMR1_IC2PSC_2			1
#define  STM_TIM234_CCMR1_IC2PSC_4			2
#define  STM_TIM234_CCMR1_IC2PSC_8			3
#define STM_TIM234_CCMR1_IC1F	4
#define  STM_TIM234_CCMR1_IC1F_NONE			0
#define  STM_TIM234_CCMR1_IC1F_CK_INT_N_2		1
#define  STM_TIM234_CCMR1_IC1F_CK_INT_N_4		2
#define  STM_TIM234_CCMR1_IC1F_CK_INT_N_8		3
#define  STM_TIM234_CCMR1_IC1F_DTS_2_N_6		4
#define  STM_TIM234_CCMR1_IC1F_DTS_2_N_8		5
#define  STM_TIM234_CCMR1_IC1F_DTS_4_N_6		6
#define  STM_TIM234_CCMR1_IC1F_DTS_4_N_8		7
#define  STM_TIM234_CCMR1_IC1F_DTS_8_N_6		8
#define  STM_TIM234_CCMR1_IC1F_DTS_8_N_8		9
#define  STM_TIM234_CCMR1_IC1F_DTS_16_N_5		10
#define  STM_TIM234_CCMR1_IC1F_DTS_16_N_6		11
#define  STM_TIM234_CCMR1_IC1F_DTS_16_N_8		12
#define  STM_TIM234_CCMR1_IC1F_DTS_32_N_5		13
#define  STM_TIM234_CCMR1_IC1F_DTS_32_N_6		14
#define  STM_TIM234_CCMR1_IC1F_DTS_32_N_8		15
#define STM_TIM234_CCMR1_IC1PSC	2
#define  STM_TIM234_CCMR1_IC1PSC_NONE			0
#define  STM_TIM234_CCMR1_IC1PSC_2			1
#define  STM_TIM234_CCMR1_IC1PSC_4			2
#define  STM_TIM234_CCMR1_IC1PSC_8			3

#define STM_TIM234_CCMR2_OC4CE	15
#define STM_TIM234_CCMR2_OC4M	12
#define  STM_TIM234_CCMR2_OC4M_FROZEN			0
#define  STM_TIM234_CCMR2_OC4M_SET_HIGH_ON_MATCH	1
#define  STM_TIM234_CCMR2_OC4M_SET_LOW_ON_MATCH		2
#define  STM_TIM234_CCMR2_OC4M_TOGGLE			3
#define  STM_TIM234_CCMR2_OC4M_FORCE_LOW		4
#define  STM_TIM234_CCMR2_OC4M_FORCE_HIGH		5
#define  STM_TIM234_CCMR2_OC4M_PWM_MODE_1		6
#define  STM_TIM234_CCMR2_OC4M_PWM_MODE_2		7
#define  STM_TIM234_CCMR2_OC4M_MASK			7
#define STM_TIM234_CCMR2_OC4PE	11
#define STM_TIM234_CCMR2_OC4FE	10
#define STM_TIM234_CCMR2_CC4S	8
#define  STM_TIM234_CCMR2_CC4S_OUTPUT			0
#define  STM_TIM234_CCMR2_CC4S_INPUT_TI4		1
#define  STM_TIM234_CCMR2_CC4S_INPUT_TI3		2
#define  STM_TIM234_CCMR2_CC4S_INPUT_TRC		3
#define  STM_TIM234_CCMR2_CC4S_MASK			3

#define STM_TIM234_CCMR2_OC3CE	7
#define STM_TIM234_CCMR2_OC3M	4
#define  STM_TIM234_CCMR2_OC3M_FROZEN			0
#define  STM_TIM234_CCMR2_OC3M_SET_HIGH_ON_MATCH	1
#define  STM_TIM234_CCMR2_OC3M_SET_LOW_ON_MATCH		2
#define  STM_TIM234_CCMR2_OC3M_TOGGLE			3
#define  STM_TIM234_CCMR2_OC3M_FORCE_LOW		4
#define  STM_TIM234_CCMR2_OC3M_FORCE_HIGH		5
#define  STM_TIM234_CCMR2_OC3M_PWM_MODE_1		6
#define  STM_TIM234_CCMR2_OC3M_PWM_MODE_2		7
#define  STM_TIM234_CCMR2_OC3M_MASK			7
#define STM_TIM234_CCMR2_OC3PE	3
#define STM_TIM234_CCMR2_OC3FE	2
#define STM_TIM234_CCMR2_CC3S	0
#define  STM_TIM234_CCMR2_CC3S_OUTPUT			0
#define  STM_TIM234_CCMR2_CC3S_INPUT_TI3		1
#define  STM_TIM234_CCMR2_CC3S_INPUT_TI4		2
#define  STM_TIM234_CCMR2_CC3S_INPUT_TRC		3
#define  STM_TIM234_CCMR2_CC3S_MASK			3

#define STM_TIM234_CCER_CC4NP	15
#define STM_TIM234_CCER_CC4P	13
#define  STM_TIM234_CCER_CC4P_ACTIVE_HIGH	0
#define  STM_TIM234_CCER_CC4P_ACTIVE_LOW	1
#define STM_TIM234_CCER_CC4E	12
#define STM_TIM234_CCER_CC3NP	11
#define STM_TIM234_CCER_CC3P	9
#define  STM_TIM234_CCER_CC3P_ACTIVE_HIGH	0
#define  STM_TIM234_CCER_CC3P_ACTIVE_LOW	1
#define STM_TIM234_CCER_CC3E	8
#define STM_TIM234_CCER_CC2NP	7
#define STM_TIM234_CCER_CC2P	5
#define  STM_TIM234_CCER_CC2P_ACTIVE_HIGH	0
#define  STM_TIM234_CCER_CC2P_ACTIVE_LOW	1
#define STM_TIM234_CCER_CC2E	4
#define STM_TIM234_CCER_CC1NP	3
#define STM_TIM234_CCER_CC1P	1
#define  STM_TIM234_CCER_CC1P_ACTIVE_HIGH	0
#define  STM_TIM234_CCER_CC1P_ACTIVE_LOW	1
#define STM_TIM234_CCER_CC1E	0

struct stm_usb {
	vuint32_t	epr[8];
	uint8_t		reserved_20[0x40 - 0x20];
	vuint32_t	cntr;
	vuint32_t	istr;
	vuint32_t	fnr;
	vuint32_t	daddr;
	vuint32_t	btable;
};

#define STM_USB_EPR_CTR_RX	15
#define  STM_USB_EPR_CTR_RX_WRITE_INVARIANT		1
#define STM_USB_EPR_DTOG_RX	14
#define STM_USB_EPR_DTOG_RX_WRITE_INVARIANT		0
#define STM_USB_EPR_STAT_RX	12
#define  STM_USB_EPR_STAT_RX_DISABLED			0
#define  STM_USB_EPR_STAT_RX_STALL			1
#define  STM_USB_EPR_STAT_RX_NAK			2
#define  STM_USB_EPR_STAT_RX_VALID			3
#define  STM_USB_EPR_STAT_RX_MASK			3
#define  STM_USB_EPR_STAT_RX_WRITE_INVARIANT		0
#define STM_USB_EPR_SETUP	11
#define STM_USB_EPR_EP_TYPE	9
#define  STM_USB_EPR_EP_TYPE_BULK			0
#define  STM_USB_EPR_EP_TYPE_CONTROL			1
#define  STM_USB_EPR_EP_TYPE_ISO			2
#define  STM_USB_EPR_EP_TYPE_INTERRUPT			3
#define  STM_USB_EPR_EP_TYPE_MASK			3
#define STM_USB_EPR_EP_KIND	8
#define  STM_USB_EPR_EP_KIND_DBL_BUF			1	/* Bulk */
#define  STM_USB_EPR_EP_KIND_STATUS_OUT			1	/* Control */
#define STM_USB_EPR_CTR_TX	7
#define  STM_USB_CTR_TX_WRITE_INVARIANT			1
#define STM_USB_EPR_DTOG_TX	6
#define  STM_USB_EPR_DTOG_TX_WRITE_INVARIANT		0
#define STM_USB_EPR_STAT_TX	4
#define  STM_USB_EPR_STAT_TX_DISABLED			0
#define  STM_USB_EPR_STAT_TX_STALL			1
#define  STM_USB_EPR_STAT_TX_NAK			2
#define  STM_USB_EPR_STAT_TX_VALID			3
#define  STM_USB_EPR_STAT_TX_WRITE_INVARIANT		0
#define  STM_USB_EPR_STAT_TX_MASK			3
#define STM_USB_EPR_EA		0
#define  STM_USB_EPR_EA_MASK				0xf

#define STM_USB_CNTR_CTRM	15
#define STM_USB_CNTR_PMAOVRM	14
#define STM_USB_CNTR_ERRM	13
#define STM_USB_CNTR_WKUPM	12
#define STM_USB_CNTR_SUSPM	11
#define STM_USB_CNTR_RESETM	10
#define STM_USB_CNTR_SOFM	9
#define STM_USB_CNTR_ESOFM	8
#define STM_USB_CNTR_RESUME	4
#define STM_USB_CNTR_FSUSP	3
#define STM_USB_CNTR_LP_MODE	2
#define STM_USB_CNTR_PDWN	1
#define STM_USB_CNTR_FRES	0

#define STM_USB_ISTR_CTR	15
#define STM_USB_ISTR_PMAOVR	14
#define STM_USB_ISTR_ERR	13
#define STM_USB_ISTR_WKUP	12
#define STM_USB_ISTR_SUSP	11
#define STM_USB_ISTR_RESET	10
#define STM_USB_ISTR_SOF	9
#define STM_USB_ISTR_ESOF	8
#define STM_USB_ISTR_DIR	4
#define STM_USB_ISTR_EP_ID	0
#define  STM_USB_ISTR_EP_ID_MASK		0xf

#define STM_USB_FNR_RXDP	15
#define STM_USB_FNR_RXDM	14
#define STM_USB_FNR_LCK		13
#define STM_USB_FNR_LSOF	11
#define  STM_USB_FNR_LSOF_MASK			0x3
#define STM_USB_FNR_FN		0
#define  STM_USB_FNR_FN_MASK			0x7ff

#define STM_USB_DADDR_EF	7
#define STM_USB_DADDR_ADD	0
#define  STM_USB_DADDR_ADD_MASK			0x7f

extern struct stm_usb stm_usb;

union stm_usb_bdt {
	struct {
		vuint32_t	addr_tx;
		vuint32_t	count_tx;
		vuint32_t	addr_rx;
		vuint32_t	count_rx;
	} single;
	struct {
		vuint32_t	addr;
		vuint32_t	count;
	} double_tx[2];
	struct {
		vuint32_t	addr;
		vuint32_t	count;
	} double_rx[2];
};

#define STM_USB_BDT_COUNT_RX_BL_SIZE	15
#define STM_USB_BDT_COUNT_RX_NUM_BLOCK	10
#define  STM_USB_BDT_COUNT_RX_NUM_BLOCK_MASK	0x1f
#define STM_USB_BDT_COUNT_RX_COUNT_RX	0
#define  STM_USB_BDT_COUNT_RX_COUNT_RX_MASK	0x1ff

#define STM_USB_BDT_SIZE	8

extern uint8_t stm_usb_sram[] __attribute__ ((aligned(4)));

struct stm_exti {
	vuint32_t	imr;
	vuint32_t	emr;
	vuint32_t	rtsr;
	vuint32_t	ftsr;

	vuint32_t	swier;
	vuint32_t	pr;
};

extern struct stm_exti stm_exti;

#endif /* _STM32L0_H_ */
