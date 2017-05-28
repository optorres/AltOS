/*
 * Copyright © 2017 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

package org.altusmetrum.altoslib_11;

import java.util.*;

public class AltosFlightSeries extends AltosDataListener {

	public ArrayList<AltosTimeSeries> series = new ArrayList<AltosTimeSeries>();

	public double	speed_filter_width = 4.0;
	public double	accel_filter_width = 4.0;

	public int[] indices() {
		int[] indices = new int[series.size()];
		for (int i = 0; i < indices.length; i++)
			indices[i] = -1;
		step_indices(indices);
		return indices;
	}

	private double time(int id, int index) {
		AltosTimeSeries		s = series.get(id);

		if (index < 0)
			return Double.NEGATIVE_INFINITY;

		if (index < s.values.size())
			return s.values.get(index).time;
		return Double.POSITIVE_INFINITY;
	}

	public boolean step_indices(int[] indices) {
		double	min_next = time(0, indices[0]+1);

		for (int i = 1; i < indices.length; i++) {
			double next = time(i, indices[i]+1);
			if (next < min_next)
				min_next = next;
		}

		if (min_next == Double.POSITIVE_INFINITY)
			return false;

		for (int i = 0; i < indices.length; i++) {
			double	t = time(i, indices[i] + 1);

			if (t <= min_next)
				indices[i]++;
		}
		return true;
	}

	public double time(int[] indices) {
		double max = time(0, indices[0]);

		for (int i = 1; i < indices.length; i++) {
			double t = time(i, indices[i]);
			if (t >= max)
				max = t;
		}
		return max;
	}

	public double value(String name, int[] indices) {
		for (int i = 0; i < indices.length; i++) {
			AltosTimeSeries	s = series.get(i);
			if (s.label.equals(name)) {
				int index = indices[i];
				if (index < 0)
					index = 0;
				if (index >= s.values.size())
					index = s.values.size() - 1;
				return s.values.get(index).value;
			}
		}
		return AltosLib.MISSING;
	}

	public double value(String name, double time) {
		for (AltosTimeSeries s : series) {
			if (s.label.equals(name))
				return s.value(time);
		}
		return AltosLib.MISSING;
	}

	public double value_before(String name, double time) {
		for (AltosTimeSeries s : series) {
			if (s.label.equals(name))
				return s.value_before(time);
		}
		return AltosLib.MISSING;
	}

	public double value_after(String name, double time) {
		for (AltosTimeSeries s : series) {
			if (s.label.equals(name))
				return s.value_after(time);
		}
		return AltosLib.MISSING;
	}

	public AltosTimeSeries make_series(String label, AltosUnits units) {
		return new AltosTimeSeries(label, units);
	}

	public void add_series(AltosTimeSeries s) {
		series.add(s);
	}

	public AltosTimeSeries add_series(String label, AltosUnits units) {
		AltosTimeSeries s = make_series(label, units);
		add_series(s);
		return s;
	}

	public void remove_series(AltosTimeSeries s) {
		series.remove(s);
	}

	public boolean has_series(String label) {
		for (AltosTimeSeries s : series)
			if (s.label.equals(label))
				return true;
		return false;
	}

	public AltosTimeSeries state_series;

	public static final String state_name = "State";

	public void set_state(int state) {

		if (state == AltosLib.ao_flight_pad)
			return;

		if (state_series == null)
			state_series = add_series(state_name, AltosConvert.state_name);
		else if (this.state == state)
			return;
		this.state = state;
		state_series.add(time(), state);
	}

	public AltosTimeSeries	accel_series;

	public static final String accel_name = "Accel";

	public void set_acceleration(double acceleration) {
		if (acceleration == AltosLib.MISSING)
			return;
		if (accel_series == null)
			accel_series = add_series(accel_name, AltosConvert.accel);

		accel_series.add(time(), acceleration);
	}

	private void compute_accel() {
		if (accel_series != null)
			return;

		if (speed_series != null) {
			AltosTimeSeries temp_series = make_series(speed_name, AltosConvert.speed);
			speed_series.filter(temp_series, accel_filter_width);
			accel_series = add_series(accel_name, AltosConvert.accel);
			temp_series.differentiate(accel_series);
		}
	}

	public void set_received_time(long received_time) {
	}

	public AltosTimeSeries rssi_series;

	public static final String rssi_name = "RSSI";

	public AltosTimeSeries status_series;

	public static final String status_name = "Radio Status";

	public void set_rssi(int rssi, int status) {
		if (rssi_series == null) {
			rssi_series = add_series(rssi_name, null);
			status_series = add_series(status_name, null);
		}
		rssi_series.add(time(), rssi);
		status_series.add(time(), status);
	}

	public AltosTimeSeries pressure_series;

	public static final String pressure_name = "Pressure";

	public AltosTimeSeries altitude_series;

	public static final String altitude_name = "Altitude";

	public AltosTimeSeries height_series;

	public static final String height_name = "Height";

	public  void set_pressure(double pa) {
		if (pa == AltosLib.MISSING)
			return;

		if (pressure_series == null)
			pressure_series = add_series(pressure_name, AltosConvert.pressure);
		pressure_series.add(time(), pa);
		if (altitude_series == null)
			altitude_series = add_series(altitude_name, AltosConvert.height);

		if (cal_data.ground_pressure == AltosLib.MISSING)
			cal_data.set_ground_pressure(pa);

		double altitude = AltosConvert.pressure_to_altitude(pa);
		altitude_series.add(time(), altitude);
	}

	private void compute_height() {
		double ground_altitude = cal_data.ground_altitude;
		if (height_series == null && ground_altitude != AltosLib.MISSING && altitude_series != null) {
			height_series = add_series(height_name, AltosConvert.height);
			for (AltosTimeValue alt : altitude_series)
				height_series.add(alt.time, alt.value - ground_altitude);
		}

		if (gps_height == null && cal_data.gps_pad != null && gps_altitude != null) {
			double gps_ground_altitude = cal_data.gps_pad.alt;
			gps_height = add_series(gps_height_name, AltosConvert.height);
			for (AltosTimeValue gps_alt : gps_altitude)
				gps_height.add(gps_alt.time, gps_alt.value - gps_ground_altitude);
		}
	}

	public AltosTimeSeries speed_series;

	public static final String speed_name = "Speed";

	private void compute_speed() {
		if (speed_series != null)
			return;

		AltosTimeSeries	alt_speed_series = null;
		AltosTimeSeries accel_speed_series = null;

		if (altitude_series != null) {
			AltosTimeSeries temp_series = make_series(altitude_name, AltosConvert.height);
			altitude_series.filter(temp_series, speed_filter_width);

			alt_speed_series = make_series(speed_name, AltosConvert.speed);
			temp_series.differentiate(alt_speed_series);
		}
		if (accel_series != null) {
			AltosTimeSeries temp_series = make_series(speed_name, AltosConvert.speed);
			accel_series.integrate(temp_series);

			accel_speed_series = make_series(speed_name, AltosConvert.speed);
			temp_series.filter(accel_speed_series, 0.1);
		}

		if (alt_speed_series != null && accel_speed_series != null) {
			double	apogee_time = AltosLib.MISSING;
			if (state_series != null) {
				for (AltosTimeValue d : state_series) {
					if (d.value >= AltosLib.ao_flight_drogue){
						apogee_time = d.time;
						break;
					}
				}
			}
			if (apogee_time == AltosLib.MISSING) {
				speed_series = alt_speed_series;
			} else {
				speed_series = make_series(speed_name, AltosConvert.speed);
				for (AltosTimeValue d : accel_speed_series) {
					if (d.time <= apogee_time)
						speed_series.add(d);
				}
				for (AltosTimeValue d : alt_speed_series) {
					if (d.time > apogee_time)
						speed_series.add(d);
				}

			}
		} else if (alt_speed_series != null) {
			speed_series = alt_speed_series;
		} else if (accel_speed_series != null) {
			speed_series = accel_speed_series;
		}
		if (speed_series != null)
			add_series(speed_series);
	}

	public AltosTimeSeries	kalman_height_series, kalman_speed_series, kalman_accel_series;

	public static final String kalman_height_name = "Kalman Height";
	public static final String kalman_speed_name = "Kalman Speed";
	public static final String kalman_accel_name = "Kalman Accel";

	public void set_kalman(double height, double speed, double acceleration) {
		if (kalman_height_series == null) {
			kalman_height_series = add_series(kalman_height_name, AltosConvert.height);
			kalman_speed_series = add_series(kalman_speed_name, AltosConvert.speed);
			kalman_accel_series = add_series(kalman_accel_name, AltosConvert.accel);
		}
		kalman_height_series.add(time(), height);
		kalman_speed_series.add(time(), speed);
		kalman_accel_series.add(time(), acceleration);
	}

	public AltosTimeSeries thrust_series;

	public static final String thrust_name = "Thrust";

	public	void set_thrust(double N) {
		if (thrust_series == null)
			thrust_series = add_series(thrust_name, AltosConvert.force);
		thrust_series.add(time(), N);
	}

	public AltosTimeSeries temperature_series;

	public static final String temperature_name = "Temperature";

	public  void set_temperature(double deg_c) {
		if (temperature_series == null)
			temperature_series = add_series(temperature_name, AltosConvert.temperature);
		temperature_series.add(time(), deg_c);
	}

	public AltosTimeSeries battery_voltage_series;

	public static final String battery_voltage_name = "Battery Voltage";

	public void set_battery_voltage(double volts) {
		if (volts == AltosLib.MISSING)
			return;
		if (battery_voltage_series == null)
			battery_voltage_series = add_series(battery_voltage_name, AltosConvert.voltage);
		battery_voltage_series.add(time(), volts);
	}

	public AltosTimeSeries apogee_voltage_series;

	public static final String apogee_voltage_name = "Apogee Voltage";

	public void set_apogee_voltage(double volts) {
		if (volts == AltosLib.MISSING)
			return;
		if (apogee_voltage_series == null)
			apogee_voltage_series = add_series(apogee_voltage_name, AltosConvert.voltage);
		apogee_voltage_series.add(time(), volts);
	}

	public AltosTimeSeries main_voltage_series;

	public static final String main_voltage_name = "Main Voltage";

	public void set_main_voltage(double volts) {
		if (volts == AltosLib.MISSING)
			return;
		if (main_voltage_series == null)
			main_voltage_series = add_series(main_voltage_name, AltosConvert.voltage);
		main_voltage_series.add(time(), volts);
	}

	public ArrayList<AltosGPSTimeValue> gps_series;

	public AltosGPS gps_before(double time) {
		AltosGPS gps = null;
		for (AltosGPSTimeValue gtv : gps_series)
			if (gtv.time <= time)
				gps = gtv.gps;
			else
				break;
		return gps;
	}

	public AltosTimeSeries	sats_in_view;
	public AltosTimeSeries sats_in_soln;
	public AltosTimeSeries gps_altitude;
	public AltosTimeSeries gps_height;
	public AltosTimeSeries gps_ground_speed;
	public AltosTimeSeries gps_ascent_rate;
	public AltosTimeSeries gps_course;
	public AltosTimeSeries gps_speed;
	public AltosTimeSeries gps_pdop, gps_vdop, gps_hdop;

	public static final String sats_in_view_name = "Satellites in view";
	public static final String sats_in_soln_name = "Satellites in solution";
	public static final String gps_altitude_name = "GPS Altitude";
	public static final String gps_height_name = "GPS Height";
	public static final String gps_ground_speed_name = "GPS Ground Speed";
	public static final String gps_ascent_rate_name = "GPS Ascent Rate";
	public static final String gps_course_name = "GPS Course";
	public static final String gps_speed_name = "GPS Speed";
	public static final String gps_pdop_name = "GPS Dilution of Precision";
	public static final String gps_vdop_name = "GPS Vertical Dilution of Precision";
	public static final String gps_hdop_name = "GPS Horizontal Dilution of Precision";

	public void set_gps(AltosGPS gps) {
		if (gps_series == null)
			gps_series = new ArrayList<AltosGPSTimeValue>();
		gps_series.add(new AltosGPSTimeValue(time(), gps));

		if (sats_in_soln == null) {
			sats_in_soln = add_series(sats_in_soln_name, null);
		}
		sats_in_soln.add(time(), gps.nsat);
		if (gps.pdop != AltosLib.MISSING) {
			if (gps_pdop == null)
				gps_pdop = add_series(gps_pdop_name, null);
			gps_pdop.add(time(), gps.pdop);
		}
		if (gps.hdop != AltosLib.MISSING) {
			if (gps_hdop == null)
				gps_hdop = add_series(gps_hdop_name, null);
			gps_hdop.add(time(), gps.hdop);
		}
		if (gps.vdop != AltosLib.MISSING) {
			if (gps_vdop == null)
				gps_vdop = add_series(gps_vdop_name, null);
			gps_vdop.add(time(), gps.vdop);
		}
		if (gps.locked) {
			if (gps.alt != AltosLib.MISSING) {
				if (gps_altitude == null)
					gps_altitude = add_series(gps_altitude_name, AltosConvert.height);
				gps_altitude.add(time(), gps.alt);
			}
			if (gps.ground_speed != AltosLib.MISSING) {
				if (gps_ground_speed == null)
					gps_ground_speed = add_series(gps_ground_speed_name, AltosConvert.speed);
				gps_ground_speed.add(time(), gps.ground_speed);
			}
			if (gps.climb_rate != AltosLib.MISSING) {
				if (gps_ascent_rate == null)
					gps_ascent_rate = add_series(gps_ascent_rate_name, AltosConvert.speed);
				gps_ascent_rate.add(time(), gps.climb_rate);
			}
			if (gps.course != AltosLib.MISSING) {
				if (gps_course == null)
					gps_course = add_series(gps_course_name, null);
				gps_course.add(time(), gps.course);
			}
			if (gps.ground_speed != AltosLib.MISSING && gps.climb_rate != AltosLib.MISSING) {
				if (gps_speed == null)
					gps_speed = add_series(gps_speed_name, null);
				gps_speed.add(time(), Math.sqrt(gps.ground_speed * gps.ground_speed +
								gps.climb_rate * gps.climb_rate));
			}
		}
		if (gps.cc_gps_sat != null) {
			if (sats_in_view == null)
				sats_in_view = add_series(sats_in_view_name, null);
			sats_in_view.add(time(), gps.cc_gps_sat.length);
		}
	}

	public static final String accel_along_name = "Accel Along";
	public static final String accel_across_name = "Accel Across";
	public static final String accel_through_name = "Accel Through";

	public AltosTimeSeries accel_along, accel_across, accel_through;

	public static final String gyro_roll_name = "Roll Rate";
	public static final String gyro_pitch_name = "Pitch Rate";
	public static final String gyro_yaw_name = "Yaw Rate";

	public AltosTimeSeries gyro_roll, gyro_pitch, gyro_yaw;

	public static final String mag_along_name = "Magnetic Field Along";
	public static final String mag_across_name = "Magnetic Field Across";
	public static final String mag_through_name = "Magnetic Field Through";

	public AltosTimeSeries mag_along, mag_across, mag_through;

	public  void set_accel(double along, double across, double through) {
		if (accel_along == null) {
			accel_along = add_series(accel_along_name, AltosConvert.accel);
			accel_across = add_series(accel_across_name, AltosConvert.accel);
			accel_through = add_series(accel_through_name, AltosConvert.accel);
		}
		accel_along.add(time(), along);
		accel_across.add(time(), across);
		accel_through.add(time(), through);
	}

	public  void set_accel_ground(double along, double across, double through) {
	}

	public  void set_gyro(double roll, double pitch, double yaw) {
		if (gyro_roll == null) {
			gyro_roll = add_series(gyro_roll_name, AltosConvert.rotation_rate);
			gyro_pitch = add_series(gyro_pitch_name, AltosConvert.rotation_rate);
			gyro_yaw = add_series(gyro_yaw_name, AltosConvert.rotation_rate);
		}
		gyro_roll.add(time(), roll);
		gyro_pitch.add(time(), pitch);
		gyro_yaw.add(time(), yaw);
	}

	public  void set_mag(double along, double across, double through) {
		if (mag_along == null) {
			mag_along = add_series(mag_along_name, AltosConvert.magnetic_field);
			mag_across = add_series(mag_across_name, AltosConvert.magnetic_field);
			mag_through = add_series(mag_through_name, AltosConvert.magnetic_field);
		}
		mag_along.add(time(), along);
		mag_across.add(time(), across);
		mag_through.add(time(), through);
	}

	public static final String orient_name = "Tilt Angle";

	public AltosTimeSeries orient_series;

	public void set_orient(double orient) {
		if (orient_series == null)
			orient_series = add_series(orient_name, AltosConvert.orient);
		orient_series.add(time(), orient);
	}

	public static final String pyro_voltage_name = "Pyro Voltage";

	public AltosTimeSeries pyro_voltage;

	public  void set_pyro_voltage(double volts) {
		if (pyro_voltage == null)
			pyro_voltage = add_series(pyro_voltage_name, AltosConvert.voltage);
		pyro_voltage.add(time(), volts);
	}

	private static String[] igniter_voltage_names;

	public String igniter_voltage_name(int channel) {
		if (igniter_voltage_names == null || igniter_voltage_names.length <= channel) {
			String[] new_igniter_voltage_names = new String[channel + 1];
			int	i = 0;

			if (igniter_voltage_names != null) {
				for (; i < igniter_voltage_names.length; i++)
					new_igniter_voltage_names[i] = igniter_voltage_names[i];
			}
			for (; i < channel+1; i++)
				new_igniter_voltage_names[i] = AltosLib.igniter_name(i);
			igniter_voltage_names = new_igniter_voltage_names;
		}
		return igniter_voltage_names[channel];
	}

	public AltosTimeSeries[] igniter_voltage;

	public  void set_igniter_voltage(double[] voltage) {
		int channels = voltage.length;
		if (igniter_voltage == null || igniter_voltage.length <= channels) {
			AltosTimeSeries[]	new_igniter_voltage = new AltosTimeSeries[channels + 1];
			int			i = 0;

			if (igniter_voltage != null) {
				for (; i < igniter_voltage.length; i++)
					new_igniter_voltage[i] = igniter_voltage[i];
			}
			for (; i < channels; i++)
				new_igniter_voltage[i] = add_series(igniter_voltage_name(i), AltosConvert.voltage);
			igniter_voltage = new_igniter_voltage;
		}
		for (int channel = 0; channel < voltage.length; channel++)
			igniter_voltage[channel].add(time(), voltage[channel]);
	}

	public static final String pyro_fired_name = "Pyro Channel State";

	public AltosTimeSeries pyro_fired_series;

	int	last_pyro_mask;

	public  void set_pyro_fired(int pyro_mask) {
		if (pyro_fired_series == null)
			pyro_fired_series = add_series(pyro_fired_name, AltosConvert.pyro_name);
		for (int channel = 0; channel < 32; channel++) {
			if ((last_pyro_mask & (1 << channel)) == 0 &&
			    (pyro_mask & (1 << channel)) != 0) {
				pyro_fired_series.add(time(), channel);
			}
		}
		last_pyro_mask = pyro_mask;
	}

	public void set_companion(AltosCompanion companion) {
	}

	public void finish() {
		compute_speed();
		compute_accel();
		compute_height();
	}

	public AltosTimeSeries[] series() {
		finish();
		return series.toArray(new AltosTimeSeries[0]);
	}

	public AltosFlightSeries(AltosCalData cal_data) {
		super(cal_data);
	}
}