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

#include <ao.h>
#include <ao_lco.h>
#include <ao_event.h>
#include <ao_seven_segment.h>
#include <ao_quadrature.h>
#include <ao_radio_cmac.h>
#include <ao_adc_single.h>

#define AO_LCO_PAD_DIGIT	0
#define AO_LCO_BOX_DIGIT_1	1
#define AO_LCO_BOX_DIGIT_10	2

#define AO_LCO_DRAG_RACE_START_TIME	AO_SEC_TO_TICKS(5)
#define AO_LCO_DRAG_RACE_STOP_TIME	AO_SEC_TO_TICKS(2)

/* UI values */
static uint8_t	ao_lco_select_mode;
static uint8_t	ao_lco_event_debug;

#define PRINTE(...) do { if (!ao_lco_debug && !ao_lco_event_debug) break; printf ("\r%5lu %s: ", (unsigned long) ao_tick_count, __func__); printf(__VA_ARGS__); flush(); } while(0)
#define AO_LCO_SELECT_PAD	0
#define AO_LCO_SELECT_BOX	1

static uint8_t	ao_lco_display_mutex;

void
ao_lco_show_pad(int8_t pad)
{
	ao_mutex_get(&ao_lco_display_mutex);
	ao_seven_segment_set(AO_LCO_PAD_DIGIT, (uint8_t) (pad | (ao_lco_drag_race << 4)));
	ao_mutex_put(&ao_lco_display_mutex);
}

#define SEVEN_SEGMENT_d		((0 << 0) |	\
				 (0 << 1) |	\
				 (1 << 2) |	\
				 (1 << 3) |	\
				 (1 << 4) |	\
				 (1 << 5) |	\
				 (1 << 6))


#define SEVEN_SEGMENT_r		((0 << 0) |	\
				 (0 << 1) |	\
				 (0 << 2) |	\
				 (1 << 3) |	\
				 (1 << 4) |	\
				 (0 << 5) |	\
				 (0 << 6))

void
ao_lco_show_box(int16_t box)
{
	ao_mutex_get(&ao_lco_display_mutex);
	ao_seven_segment_set(AO_LCO_BOX_DIGIT_1, (uint8_t) (box % 10 | (ao_lco_drag_race << 4)));
	ao_seven_segment_set(AO_LCO_BOX_DIGIT_10, (uint8_t) (box / 10 | (ao_lco_drag_race << 4)));
	ao_mutex_put(&ao_lco_display_mutex);
}

static void
ao_lco_show_value(uint16_t value, uint8_t point)
{
	uint8_t	hundreds, tens, ones;

	PRINTD("value %d\n", value);
	ones = (uint8_t) (value % 10);
	tens = (uint8_t) ((value / 10) % 10);
	hundreds = (uint8_t) ((value / 100) % 10);
	switch (point) {
	case 2:
		hundreds |= 0x10;
		break;
	case 1:
		tens |= 0x10;
		break;
	case 0:
		ones |= 0x10;
		break;
	default:
		break;
	}
	ao_mutex_get(&ao_lco_display_mutex);
	ao_seven_segment_set(AO_LCO_PAD_DIGIT, ones);
	ao_seven_segment_set(AO_LCO_BOX_DIGIT_1, tens);
	ao_seven_segment_set(AO_LCO_BOX_DIGIT_10, hundreds);
	ao_mutex_put(&ao_lco_display_mutex);
}

static void
ao_lco_show_lco_voltage(void)
{
	struct ao_adc	packet;
	int16_t		decivolt;

	ao_adc_single_get(&packet);
	decivolt = ao_battery_decivolt(packet.v_batt);
	ao_lco_show_value((uint16_t) decivolt, 1);
}

void
ao_lco_show(void)
{
	switch (ao_lco_box) {
	case AO_LCO_LCO_VOLTAGE:
		ao_lco_show_lco_voltage();
		break;
	default:
		switch (ao_lco_pad) {
		case AO_LCO_PAD_VOLTAGE:
			ao_lco_show_value(ao_pad_query.battery, 1);
			break;
		case AO_LCO_PAD_RSSI:
			if (!(ao_lco_valid[ao_lco_box] & AO_LCO_VALID_LAST))
				ao_lco_show_value(888, 0);
			else
				ao_lco_show_value((uint16_t) (-ao_radio_cmac_rssi), 0);
			break;
		default:
			ao_lco_show_pad(ao_lco_pad);
			ao_lco_show_box(ao_lco_box);
			break;
		}
	}
}

static void
ao_lco_set_select(void)
{
	if (ao_lco_armed) {
		ao_led_off(AO_LED_PAD);
		ao_led_off(AO_LED_BOX);
	} else {
		switch (ao_lco_select_mode) {
		case AO_LCO_SELECT_PAD:
			ao_led_off(AO_LED_BOX);
			ao_led_on(AO_LED_PAD);
			break;
		case AO_LCO_SELECT_BOX:
			ao_led_off(AO_LED_PAD);
			ao_led_on(AO_LED_BOX);
			break;
		default:
			break;
		}
	}
}

static struct ao_task	ao_lco_drag_task;

static void
ao_lco_drag_monitor(void)
{
	AO_TICK_TYPE	delay = ~0UL;
	AO_TICK_TYPE	now;

	ao_beep_for(AO_BEEP_MID, AO_MS_TO_TICKS(200));
	for (;;) {
		PRINTD("Drag monitor count %d delay %lu\n", ao_lco_drag_beep_count, (unsigned long) delay);
		if (delay == (AO_TICK_TYPE) ~0)
			ao_sleep(&ao_lco_drag_beep_count);
		else
			ao_sleep_for(&ao_lco_drag_beep_count, delay);

		delay = ~0UL;
		now = ao_time();
		delay = ao_lco_drag_warn_check(now, delay);
		delay = ao_lco_drag_beep_check(now, delay);
	}
}

static void
ao_lco_input(void)
{
	static struct ao_event	event;

	for (;;) {
		ao_event_get(&event);
		PRINTE("event type %d unit %d value %ld\n",
		       event.type, event.unit, (long) event.value);
		switch (event.type) {
		case AO_EVENT_QUADRATURE:
			switch (event.unit) {
			case AO_QUADRATURE_SELECT:
				if (!ao_lco_armed) {
					switch (ao_lco_select_mode) {
					case AO_LCO_SELECT_PAD:
						ao_lco_step_pad((int8_t) event.value);
						break;
					case AO_LCO_SELECT_BOX:
						ao_lco_step_box((int8_t) event.value);
						break;
					default:
						break;
					}
				}
				break;
			}
			break;
		case AO_EVENT_BUTTON:
			switch (event.unit) {
			case AO_BUTTON_ARM:
				ao_lco_set_armed((uint8_t) event.value);
				ao_lco_set_select();
				break;
			case AO_BUTTON_FIRE:
				if (ao_lco_armed)
					ao_lco_set_firing((uint8_t) event.value);
				break;
			case AO_BUTTON_DRAG_SELECT:
				if (event.value)
					ao_lco_toggle_drag();
				break;
			case AO_BUTTON_DRAG_MODE:
				if (event.value)
					ao_lco_drag_enable();
				else
					ao_lco_drag_disable();
				break;
			case AO_BUTTON_ENCODER_SELECT:
				if (event.value) {
					if (!ao_lco_armed) {
						ao_lco_select_mode = 1 - ao_lco_select_mode;
						ao_lco_set_select();
					}
				}
				break;
			}
			break;
		}
	}
}

/*
 * Light up everything for a second at power on to let the user
 * visually inspect the system for correct operation
 */
static void
ao_lco_display_test(void)
{
	ao_mutex_get(&ao_lco_display_mutex);
	ao_seven_segment_set(AO_LCO_PAD_DIGIT, 8 | 0x10);
	ao_seven_segment_set(AO_LCO_BOX_DIGIT_1, 8 | 0x10);
	ao_seven_segment_set(AO_LCO_BOX_DIGIT_10, 8 | 0x10);
	ao_mutex_put(&ao_lco_display_mutex);
	ao_led_on(AO_LEDS_AVAILABLE);
	ao_delay(AO_MS_TO_TICKS(1000));
	ao_led_off(AO_LEDS_AVAILABLE);
}

static void
ao_lco_batt_voltage(void)
{
	ao_lco_show_lco_voltage();
	ao_delay(AO_MS_TO_TICKS(1000));
}

static struct ao_task ao_lco_input_task;
static struct ao_task ao_lco_monitor_task;
static struct ao_task ao_lco_arm_warn_task;
static struct ao_task ao_lco_igniter_status_task;

static void
ao_lco_main(void)
{
	ao_lco_display_test();
	ao_lco_batt_voltage();
	ao_lco_search();
	ao_add_task(&ao_lco_input_task, ao_lco_input, "lco input");
	ao_add_task(&ao_lco_arm_warn_task, ao_lco_arm_warn, "lco arm warn");
	ao_add_task(&ao_lco_igniter_status_task, ao_lco_igniter_status, "lco igniter status");
	ao_add_task(&ao_lco_drag_task, ao_lco_drag_monitor, "drag race");
	ao_lco_monitor();
}

#if DEBUG
static void
ao_lco_set_debug(void)
{
	uint32_t r = ao_cmd_decimal();
	if (ao_cmd_status == ao_cmd_success){
		ao_lco_debug = r & 1;
		ao_lco_event_debug = (r & 2) >> 1;
	}
}

const struct ao_cmds ao_lco_cmds[] = {
	{ ao_lco_set_debug,	"D <0 off, 1 on>\0Debug" },
	{ ao_lco_search,	"s\0Search for pad boxes" },
	{ ao_lco_pretend,	"p\0Pretend there are lots of pad boxes" },
	{ 0, NULL }
};
#endif

void
ao_lco_init(void)
{
	ao_add_task(&ao_lco_monitor_task, ao_lco_main, "lco monitor");
#if DEBUG
	ao_cmd_register(&ao_lco_cmds[0]);
#endif
}
