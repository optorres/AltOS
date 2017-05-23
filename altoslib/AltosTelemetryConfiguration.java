/*
 * Copyright © 2011 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.altoslib_11;


public class AltosTelemetryConfiguration extends AltosTelemetryStandard {
	int	device_type() { return uint8(5); }
	int	flight() { return uint16(6); }
	int	config_major() { return uint8(8); }
	int	config_minor() { return uint8(9); }
	int	apogee_delay() { return uint16(10); }
	int	main_deploy() { return uint16(12); }
	int	v_batt() { return uint16(10); }
	int	flight_log_max() { return uint16(14); }
	String	callsign() { return string(16, 8); }
	String	version() { return string(24, 8); }

	public AltosTelemetryConfiguration(int[] bytes) throws AltosCRCException {
		super(bytes);
	}

	public void update_state(AltosState state) {
		super.update_state(state);
		state.set_device_type(device_type());
		state.set_flight(flight());
		state.set_config(config_major(), config_minor(), flight_log_max());
		if (device_type() == AltosLib.product_telegps)
			state.set_battery_voltage(AltosConvert.tele_gps_voltage(v_batt()));
		else
			state.set_flight_params(apogee_delay(), main_deploy());

		state.set_callsign(callsign());
		state.set_firmware_version(version());
	}
}
