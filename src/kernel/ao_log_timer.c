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
#include <ao_log.h>
#include <ao_data.h>
#include <ao_flight.h>

#if HAS_FLIGHT
static uint8_t	ao_log_data_pos;

/* a hack to make sure that ao_log_timers fill the eeprom block in even units */
typedef uint8_t check_log_size[1-(256 % sizeof(struct ao_log_timer))] ;

#ifndef AO_SENSOR_INTERVAL_ASCENT
#define AO_SENSOR_INTERVAL_ASCENT	1
#define AO_SENSOR_INTERVAL_DESCENT	10
#define AO_OTHER_INTERVAL		32
#endif

void
ao_log(void)
{
	AO_TICK_TYPE		next_sensor;
	uint8_t			i;

	ao_storage_setup();

	ao_log_scan();

	while (!ao_log_running)
		ao_sleep(&ao_log_running);

#if HAS_FLIGHT
	ao_log_data.type = AO_LOG_FLIGHT;
	ao_log_data.tick = (uint16_t) ao_sample_tick;
#if HAS_ACCEL
	ao_log_data.u.flight.ground_accel = ao_ground_accel;
#endif
#if HAS_GYRO
	ao_log_data.u.flight.ground_accel_along = ao_ground_accel_along;
	ao_log_data.u.flight.ground_accel_across = ao_ground_accel_across;
	ao_log_data.u.flight.ground_accel_through = ao_ground_accel_through;
	ao_log_data.u.flight.ground_roll = ao_ground_roll;
	ao_log_data.u.flight.ground_pitch = ao_ground_pitch;
	ao_log_data.u.flight.ground_yaw = ao_ground_yaw;
#endif
	ao_log_data.u.flight.flight = ao_flight_number;
	ao_log_write(&ao_log_data);
#endif

	/* Write the whole contents of the ring to the log
	 * when starting up.
	 */
	ao_log_data_pos = ao_data_ring_next(ao_data_head);
	next_sensor = ao_data_ring[ao_log_data_pos].tick;
	ao_log_state = ao_flight_startup;
	for (;;) {
		/* Write samples to EEPROM */
		while (ao_log_data_pos != ao_data_head) {
			AO_TICK_TYPE tick = ao_data_ring[ao_log_data_pos].tick;
			ao_log_data.tick = (uint16_t) tick;
			volatile struct ao_data *d = &ao_data_ring[ao_log_data_pos];
			if ((AO_TICK_SIGNED) (tick - next_sensor) >= 0) {
				ao_log_data.type = AO_LOG_SENSOR;

#if HAS_IMU
				ao_log_data.u.sensor.accel_along = ao_data_along(d);
				ao_log_data.u.sensor.accel_across = ao_data_across(d);
				ao_log_data.u.sensor.accel_through = ao_data_through(d);
				ao_log_data.u.sensor.gyro_roll = ao_data_roll(d);
				ao_log_data.u.sensor.gyro_pitch = ao_data_pitch(d);
				ao_log_data.u.sensor.gyro_yaw = ao_data_yaw(d);
#endif
#if HAS_MAG
				ao_log_data.u.sensor.mag_along = ao_data_mag_along(d);
				ao_log_data.u.sensor.mag_across = ao_data_mag_across(d);
				ao_log_data.u.sensor.mag_through = ao_data_mag_through(d);
#endif
				ao_log_data.u.sensor.v_batt = d->adc.v_batt;
				for (i = 0; i < AO_ADC_NUM_SENSE; i++)
					ao_log_data.u.sensor.sense[i] = d->adc.sense[i];
				ao_log_data.u.sensor.pyro = ao_pyro_fired;

				ao_log_write(&ao_log_data);
				if (ao_log_state <= ao_flight_coast)
					next_sensor = tick + AO_SENSOR_INTERVAL_ASCENT;
				else
					next_sensor = tick + AO_SENSOR_INTERVAL_DESCENT;
			}
			ao_log_data_pos = ao_data_ring_next(ao_log_data_pos);
		}
#if HAS_FLIGHT
		/* Write state change to EEPROM */
		if (ao_flight_state != ao_log_state) {
			ao_log_state = ao_flight_state;
			ao_log_data.type = AO_LOG_STATE;
			ao_log_data.tick = (uint16_t) ao_time();
			ao_log_data.u.state.state = ao_log_state;
			ao_log_data.u.state.reason = 0;
			ao_log_write(&ao_log_data);

			if (ao_log_state == ao_flight_landed)
				ao_log_stop();
		}
#endif

		ao_log_flush();

		/* Wait for a while */
		ao_delay(AO_MS_TO_TICKS(100));

		/* Stop logging when told to */
		while (!ao_log_running)
			ao_sleep(&ao_log_running);
	}
}
#endif /* HAS_FLIGHT */

