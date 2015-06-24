/*
 * Copyright © 2013 Mike Beattie <mike@ethernal.org>
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

package org.altusmetrum.AltosDroid;

import org.altusmetrum.altoslib_7.*;

import android.app.Activity;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.location.Location;

public class TabPad extends AltosDroidTab {
	private TextView battery_voltage_view;
	private GoNoGoLights battery_lights;

	private TextView receiver_voltage_view;
	private TextView receiver_voltage_label;
	private GoNoGoLights receiver_voltage_lights;

	private TextView apogee_voltage_view;
	private TextView apogee_voltage_label;
	private GoNoGoLights apogee_lights;

	private TextView main_voltage_view;
	private TextView main_voltage_label;
	private GoNoGoLights main_lights;

	private TextView data_logging_view;
	private GoNoGoLights data_logging_lights;

	private TextView gps_locked_view;
	private GoNoGoLights gps_locked_lights;

	private TextView gps_ready_view;
	private GoNoGoLights gps_ready_lights;

	private TextView pad_latitude_view;
	private TextView pad_longitude_view;
	private TextView pad_altitude_view;

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View v = inflater.inflate(R.layout.tab_pad, container, false);
		battery_voltage_view = (TextView) v.findViewById(R.id.battery_voltage_value);
		battery_lights = new GoNoGoLights((ImageView) v.findViewById(R.id.battery_redled),
		                                  (ImageView) v.findViewById(R.id.battery_greenled),
		                                  getResources());

		receiver_voltage_view = (TextView) v.findViewById(R.id.receiver_voltage_value);
		receiver_voltage_label = (TextView) v.findViewById(R.id.receiver_voltage_label);
		receiver_voltage_lights = new GoNoGoLights((ImageView) v.findViewById(R.id.receiver_redled),
							   (ImageView) v.findViewById(R.id.receiver_greenled),
							   getResources());

		apogee_voltage_view = (TextView) v.findViewById(R.id.apogee_voltage_value);
		apogee_voltage_label = (TextView) v.findViewById(R.id.apogee_voltage_label);
		apogee_lights = new GoNoGoLights((ImageView) v.findViewById(R.id.apogee_redled),
		                                 (ImageView) v.findViewById(R.id.apogee_greenled),
		                                 getResources());

		main_voltage_view = (TextView) v.findViewById(R.id.main_voltage_value);
		main_voltage_label = (TextView) v.findViewById(R.id.main_voltage_label);
		main_lights = new GoNoGoLights((ImageView) v.findViewById(R.id.main_redled),
		                               (ImageView) v.findViewById(R.id.main_greenled),
		                               getResources());

		data_logging_view = (TextView) v.findViewById(R.id.logging_value);
		data_logging_lights = new GoNoGoLights((ImageView) v.findViewById(R.id.logging_redled),
		                                      (ImageView) v.findViewById(R.id.logging_greenled),
		                                      getResources());

		gps_locked_view = (TextView) v.findViewById(R.id.gps_locked_value);
		gps_locked_lights = new GoNoGoLights((ImageView) v.findViewById(R.id.gps_locked_redled),
		                                    (ImageView) v.findViewById(R.id.gps_locked_greenled),
		                                    getResources());

		gps_ready_view = (TextView) v.findViewById(R.id.gps_ready_value);
		gps_ready_lights = new GoNoGoLights((ImageView) v.findViewById(R.id.gps_ready_redled),
		                                   (ImageView) v.findViewById(R.id.gps_ready_greenled),
		                                   getResources());

		pad_latitude_view = (TextView) v.findViewById(R.id.pad_lat_value);
		pad_longitude_view = (TextView) v.findViewById(R.id.pad_lon_value);
		pad_altitude_view = (TextView) v.findViewById(R.id.pad_alt_value);
        return v;
	}

	public String tab_name() { return AltosDroid.tab_pad_name; }

	public void show(TelemetryState telem_state, AltosState state, AltosGreatCircle from_receiver, Location receiver) {
		if (state != null) {
			battery_voltage_view.setText(AltosDroid.number("%4.2f V", state.battery_voltage));
			battery_lights.set(state.battery_voltage >= AltosLib.ao_battery_good, state.battery_voltage == AltosLib.MISSING);
			if (state.apogee_voltage == AltosLib.MISSING) {
				apogee_voltage_view.setVisibility(View.GONE);
				apogee_voltage_label.setVisibility(View.GONE);
			} else {
				apogee_voltage_view.setText(AltosDroid.number("%4.2f V", state.apogee_voltage));
				apogee_voltage_view.setVisibility(View.VISIBLE);
				apogee_voltage_label.setVisibility(View.VISIBLE);
			}
			apogee_lights.set(state.apogee_voltage >= AltosLib.ao_igniter_good, state.apogee_voltage == AltosLib.MISSING);
			if (state.main_voltage == AltosLib.MISSING) {
				main_voltage_view.setVisibility(View.GONE);
				main_voltage_label.setVisibility(View.GONE);
			} else {
				main_voltage_view.setText(AltosDroid.number("%4.2f V", state.main_voltage));
				main_voltage_view.setVisibility(View.VISIBLE);
				main_voltage_label.setVisibility(View.VISIBLE);
			}
			main_lights.set(state.main_voltage >= AltosLib.ao_igniter_good, state.main_voltage == AltosLib.MISSING);

			if (state.flight != 0) {
				if (state.state <= AltosLib.ao_flight_pad)
					data_logging_view.setText("Ready to record");
				else if (state.state < AltosLib.ao_flight_landed)
					data_logging_view.setText("Recording data");
				else
					data_logging_view.setText("Recorded data");
			} else {
				data_logging_view.setText("Storage full");
			}
			data_logging_lights.set(state.flight != 0, state.flight == AltosLib.MISSING);

			if (state.gps != null) {
				int soln = state.gps.nsat;
				int nsat = state.gps.cc_gps_sat != null ? state.gps.cc_gps_sat.length : 0;
				gps_locked_view.setText(String.format("%4d in soln, %4d in view", soln, nsat));
				gps_locked_lights.set(state.gps.locked && state.gps.nsat >= 4, false);
				if (state.gps_ready)
					gps_ready_view.setText("Ready");
				else
					gps_ready_view.setText(AltosDroid.integer("Waiting %d", state.gps_waiting));
			} else
				gps_locked_lights.set(false, true);
			gps_ready_lights.set(state.gps_ready, state.gps == null);
		}

		if (telem_state != null) {
			if (telem_state.receiver_battery == AltosLib.MISSING) {
				receiver_voltage_view.setVisibility(View.GONE);
				receiver_voltage_label.setVisibility(View.GONE);
			} else {
				receiver_voltage_view.setText(AltosDroid.number("%4.2f V", telem_state.receiver_battery));
				receiver_voltage_view.setVisibility(View.VISIBLE);
				receiver_voltage_label.setVisibility(View.VISIBLE);
			}
			receiver_voltage_lights.set(telem_state.receiver_battery >= AltosLib.ao_battery_good, telem_state.receiver_battery == AltosLib.MISSING);
		}

		if (receiver != null) {
			double altitude = AltosLib.MISSING;
			if (receiver.hasAltitude())
				altitude = receiver.getAltitude();
			pad_latitude_view.setText(AltosDroid.pos(receiver.getLatitude(), "N", "S"));
			pad_longitude_view.setText(AltosDroid.pos(receiver.getLongitude(), "E", "W"));
			set_value(pad_altitude_view, AltosConvert.height, 6, altitude);
		}
	}

}
