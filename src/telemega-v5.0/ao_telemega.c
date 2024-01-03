/*
 * Copyright © 2021 Keith Packard <keithp@keithp.com>
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
#include <ao_mpu6000.h>
#include <ao_mmc5983.h>
#include <ao_adxl375.h>
#include <ao_log.h>
#include <ao_exti.h>
#include <ao_packet.h>
#include <ao_companion.h>
#include <ao_profile.h>
#include <ao_eeprom.h>
#include <ao_i2c_bit.h>
#ifdef HAS_GPS_MOSAIC
#include <ao_gps_mosaic.h>
#endif
#if HAS_SAMPLE_PROFILE
#include <ao_sample_profile.h>
#endif
#include <ao_pyro.h>
#if HAS_STACK_GUARD
#include <ao_mpu.h>
#endif
#include <ao_pwm.h>

int
main(void)
{
	ao_clock_init();

#if HAS_STACK_GUARD
	ao_mpu_init();
#endif

	ao_task_init();
	ao_serial_init();
	ao_led_init();
	ao_led_on(LEDS_AVAILABLE);
	ao_timer_init();

	ao_spi_init();
#ifdef MMC5983_I2C
	ao_i2c_bit_init();
#endif
	ao_dma_init();
	ao_exti_init();

	ao_adc_init();
#if HAS_BEEP
	ao_beep_init();
#endif
	ao_cmd_init();

	ao_ms5607_init();
	ao_mpu6000_init();
	ao_mmc5983_init();
	ao_adxl375_init();

	ao_eeprom_init();
	ao_storage_init();

	ao_flight_init();
	ao_log_init();
	ao_report_init();

	ao_usb_init();
	ao_gps_init();
#ifdef HAS_GPS_MOSAIC
	ao_gps_mosaic_init();
#endif
	ao_gps_report_mega_init();
	ao_telemetry_init();
	ao_radio_init();
	ao_packet_slave_init(false);
	ao_igniter_init();
	ao_companion_init();
	ao_pyro_init();

	ao_config_init();
#if AO_PROFILE
	ao_profile_init();
#endif
#if HAS_SAMPLE_PROFILE
	ao_sample_profile_init();
#endif

	ao_pwm_init();

	ao_led_off(LEDS_AVAILABLE);

	ao_start_scheduler();
	return 0;
}
