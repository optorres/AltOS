/*
 * Copyright © 2024 Keith Packard <keithp@keithp.com>
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

#include "ao.h"
#include "ao_pwm.h"

static uint8_t	pwm_running;

static uint16_t	pwm_value[NUM_PWM];

static void
ao_pwm_up(void)
{
	if (pwm_running++ == 0) {
		struct stm_tim234	*tim = &AO_PWM_TIMER;

		tim->ccr1 = 0;
		tim->ccr2 = 0;
		tim->ccr3 = 0;
		tim->ccr4 = 0;
		tim->arr = PWM_MAX - 1;	/* turn on the timer */
		tim->cr1 = ((STM_TIM234_CR1_CKD_1 << STM_TIM234_CR1_CKD) |
			    (0 << STM_TIM234_CR1_ARPE) |
			    (STM_TIM234_CR1_CMS_EDGE << STM_TIM234_CR1_CMS) |
			    (STM_TIM234_CR1_DIR_UP << STM_TIM234_CR1_DIR) |
			    (0 << STM_TIM234_CR1_OPM) |
			    (0 << STM_TIM234_CR1_URS) |
			    (0 << STM_TIM234_CR1_UDIS) |
			    (1 << STM_TIM234_CR1_CEN));

		/* Set the timer running */
		tim->egr = (1 << STM_TIM234_EGR_UG);
	}
}

static void
ao_pwm_down(void)
{
	if (--pwm_running == 0) {
		struct stm_tim234	*tim = &AO_PWM_TIMER;

		tim->arr = 0;
		tim->cr1 = ((STM_TIM234_CR1_CKD_1 << STM_TIM234_CR1_CKD) |
			    (0 << STM_TIM234_CR1_ARPE) |
			    (STM_TIM234_CR1_CMS_EDGE << STM_TIM234_CR1_CMS) |
			    (STM_TIM234_CR1_DIR_UP << STM_TIM234_CR1_DIR) |
			    (0 << STM_TIM234_CR1_OPM) |
			    (0 << STM_TIM234_CR1_URS) |
			    (0 << STM_TIM234_CR1_UDIS) |
			    (0 << STM_TIM234_CR1_CEN));

		/* Stop the timer */
		tim->egr = (1 << STM_TIM234_EGR_UG);
	}
}

void
ao_pwm_set(uint8_t pwm, uint16_t value)
{
	struct stm_tim234	*tim = &AO_PWM_TIMER;

#if PWM_MAX < UINT16_MAX
	if (value > PWM_MAX)
		value = PWM_MAX;
#endif
	if (value != 0) {
		if (pwm_value[pwm] == 0)
			ao_pwm_up();
	}
	switch (pwm) {
	case 0:
		tim->ccr1 = value;
		break;
	case 1:
		tim->ccr2 = value;
		break;
	case 2:
		tim->ccr3 = value;
		break;
	case 3:
		tim->ccr4 = value;
		break;
	}
	if (value == 0) {
		if (pwm_value[pwm] != 0)
			ao_pwm_down();
	}
	pwm_value[pwm] = value;
}

static void
ao_pwm_cmd(void)
{
	uint8_t	ch;
	uint16_t val;

	ch = (uint8_t) ao_cmd_decimal();
	val = (uint16_t) ao_cmd_decimal();
	if (ao_cmd_status != ao_cmd_success)
		return;

	printf("Set channel %d to %d\n", ch, val);
	ao_pwm_set(ch, val);
}

static const struct ao_cmds ao_pwm_cmds[] = {
	{ ao_pwm_cmd,	"P <ch> <val>\0Set PWM ch to val" },
	{ 0, NULL },
};

void
ao_pwm_init(void)
{
	struct stm_tim234	*tim = &AO_PWM_TIMER;

	stm_rcc.apb1enr |= (1 << AO_PWM_TIMER_ENABLE);

	tim->cr1 = 0;
	tim->psc = AO_PWM_TIMER_SCALE - 1;
	tim->cnt = 0;
	tim->ccer = ((1 << STM_TIM234_CCER_CC1E) |
		     (0 << STM_TIM234_CCER_CC1P) |
		     (1 << STM_TIM234_CCER_CC2E) |
		     (0 << STM_TIM234_CCER_CC2P) |
		     (1 << STM_TIM234_CCER_CC3E) |
		     (0 << STM_TIM234_CCER_CC3P) |
		     (1 << STM_TIM234_CCER_CC4E) |
		     (0 << STM_TIM234_CCER_CC4P));

	tim->ccmr1 = ((0 << STM_TIM234_CCMR1_OC2CE) |
		      (STM_TIM234_CCMR1_OC2M_PWM_MODE_1 << STM_TIM234_CCMR1_OC2M) |
		      (0 << STM_TIM234_CCMR1_OC2PE) |
		      (0 << STM_TIM234_CCMR1_OC2FE) |
		      (STM_TIM234_CCMR1_CC2S_OUTPUT << STM_TIM234_CCMR1_CC2S) |

		      (0 << STM_TIM234_CCMR1_OC1CE) |
		      (STM_TIM234_CCMR1_OC1M_PWM_MODE_1 << STM_TIM234_CCMR1_OC1M) |
		      (0 << STM_TIM234_CCMR1_OC1PE) |
		      (0 << STM_TIM234_CCMR1_OC1FE) |
		      (STM_TIM234_CCMR1_CC1S_OUTPUT << STM_TIM234_CCMR1_CC1S));


	tim->ccmr2 = ((0 << STM_TIM234_CCMR2_OC4CE) |
		      (STM_TIM234_CCMR2_OC4M_PWM_MODE_1 << STM_TIM234_CCMR2_OC4M) |
		      (0 << STM_TIM234_CCMR2_OC4PE) |
		      (0 << STM_TIM234_CCMR2_OC4FE) |
		      (STM_TIM234_CCMR2_CC4S_OUTPUT << STM_TIM234_CCMR2_CC4S) |

		      (0 << STM_TIM234_CCMR2_OC3CE) |
		      (STM_TIM234_CCMR2_OC3M_PWM_MODE_1 << STM_TIM234_CCMR2_OC3M) |
		      (0 << STM_TIM234_CCMR2_OC3PE) |
		      (0 << STM_TIM234_CCMR2_OC3FE) |
		      (STM_TIM234_CCMR2_CC3S_OUTPUT << STM_TIM234_CCMR2_CC3S));
	tim->egr = 0;

	tim->sr = 0;
	tim->dier = 0;
	tim->smcr = 0;
	tim->cr2 = ((0 << STM_TIM234_CR2_TI1S) |
		    (STM_TIM234_CR2_MMS_RESET<< STM_TIM234_CR2_MMS) |
		    (0 << STM_TIM234_CR2_CCDS));

	stm_set_afio_mapr(AO_AFIO_PWM_REMAP,
			  AO_AFIO_PWM_REMAP_VAL,
			  AO_AFIO_PWM_REMAP_MASK);

	ao_enable_port(AO_PWM_0_GPIO);

	stm_gpio_conf(AO_PWM_0_GPIO, AO_PWM_0_PIN,
		      STM_GPIO_CR_MODE_OUTPUT_2MHZ,
		      STM_GPIO_CR_CNF_OUTPUT_AF_PUSH_PULL);
#if NUM_PWM > 1
	stm_gpio_conf(AO_PWM_1_GPIO, AO_PWM_1_PIN,
		      STM_GPIO_CR_MODE_OUTPUT_2MHZ,
		      STM_GPIO_CR_CNF_OUTPUT_AF_PUSH_PULL);
#endif
#if NUM_PWM > 2
	stm_gpio_conf(AO_PWM_2_GPIO, AO_PWM_2_PIN,
		      STM_GPIO_CR_MODE_OUTPUT_2MHZ,
		      STM_GPIO_CR_CNF_OUTPUT_AF_PUSH_PULL);
#endif
#if NUM_PWM > 3
	stm_gpio_conf(AO_PWM_3_GPIO, AO_PWM_3_PIN,
		      STM_GPIO_CR_MODE_OUTPUT_2MHZ,
		      STM_GPIO_CR_CNF_OUTPUT_AF_PUSH_PULL);
#endif
	ao_cmd_register(&ao_pwm_cmds[0]);
}
