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

import org.altusmetrum.altoslib_6.*;

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
	AltosDroid mAltosDroid;

	private TextView mBatteryVoltageView;
	private TextView mBatteryVoltageLabel;
	private GoNoGoLights mBatteryLights;
	private TextView mApogeeVoltageView;
	private TextView mApogeeVoltageLabel;
	private GoNoGoLights mApogeeLights;
	private TextView mMainVoltageView;
	private TextView mMainVoltageLabel;
	private GoNoGoLights mMainLights;
	private TextView mDataLoggingView;
	private GoNoGoLights mDataLoggingLights;
	private TextView mGPSLockedView;
	private GoNoGoLights mGPSLockedLights;
	private TextView mGPSReadyView;
	private GoNoGoLights mGPSReadyLights;
	private TextView mPadLatitudeView;
	private TextView mPadLongitudeView;
	private TextView mPadAltitudeView;

	@Override
	public void onAttach(Activity activity) {
		super.onAttach(activity);
		mAltosDroid = (AltosDroid) activity;
		mAltosDroid.registerTab(this);
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View v = inflater.inflate(R.layout.tab_pad, container, false);
		mBatteryVoltageView = (TextView) v.findViewById(R.id.battery_voltage_value);
		mBatteryVoltageLabel = (TextView) v.findViewById(R.id.battery_voltage_label);
		mBatteryLights = new GoNoGoLights((ImageView) v.findViewById(R.id.battery_redled),
		                                  (ImageView) v.findViewById(R.id.battery_greenled),
		                                  getResources());

		mApogeeVoltageView = (TextView) v.findViewById(R.id.apogee_voltage_value);
		mApogeeVoltageLabel = (TextView) v.findViewById(R.id.apogee_voltage_label);
		mApogeeLights = new GoNoGoLights((ImageView) v.findViewById(R.id.apogee_redled),
		                                 (ImageView) v.findViewById(R.id.apogee_greenled),
		                                 getResources());

		mMainVoltageView = (TextView) v.findViewById(R.id.main_voltage_value);
		mMainVoltageLabel = (TextView) v.findViewById(R.id.main_voltage_label);
		mMainLights = new GoNoGoLights((ImageView) v.findViewById(R.id.main_redled),
		                               (ImageView) v.findViewById(R.id.main_greenled),
		                               getResources());

		mDataLoggingView = (TextView) v.findViewById(R.id.logging_value);
		mDataLoggingLights = new GoNoGoLights((ImageView) v.findViewById(R.id.logging_redled),
		                                      (ImageView) v.findViewById(R.id.logging_greenled),
		                                      getResources());

		mGPSLockedView = (TextView) v.findViewById(R.id.gps_locked_value);
		mGPSLockedLights = new GoNoGoLights((ImageView) v.findViewById(R.id.gps_locked_redled),
		                                    (ImageView) v.findViewById(R.id.gps_locked_greenled),
		                                    getResources());

		mGPSReadyView = (TextView) v.findViewById(R.id.gps_ready_value);
		mGPSReadyLights = new GoNoGoLights((ImageView) v.findViewById(R.id.gps_ready_redled),
		                                   (ImageView) v.findViewById(R.id.gps_ready_greenled),
		                                   getResources());

		mPadLatitudeView = (TextView) v.findViewById(R.id.pad_lat_value);
		mPadLongitudeView = (TextView) v.findViewById(R.id.pad_lon_value);
		mPadAltitudeView = (TextView) v.findViewById(R.id.pad_alt_value);
        return v;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		mAltosDroid.unregisterTab(this);
		mAltosDroid = null;
	}

	public String tab_name() { return "pad"; }

	public void show(AltosState state, AltosGreatCircle from_receiver, Location receiver) {
		if (state != null) {
			mBatteryVoltageView.setText(AltosDroid.number("%4.2f V", state.battery_voltage));
			mBatteryLights.set(state.battery_voltage >= AltosLib.ao_battery_good, state.battery_voltage == AltosLib.MISSING);
			if (state.apogee_voltage == AltosLib.MISSING) {
				mApogeeVoltageView.setVisibility(View.GONE);
				mApogeeVoltageLabel.setVisibility(View.GONE);
			} else {
				mApogeeVoltageView.setText(AltosDroid.number("%4.2f V", state.apogee_voltage));
				mApogeeVoltageView.setVisibility(View.VISIBLE);
				mApogeeVoltageLabel.setVisibility(View.VISIBLE);
			}
			mApogeeLights.set(state.apogee_voltage >= AltosLib.ao_igniter_good, state.apogee_voltage == AltosLib.MISSING);
			if (state.main_voltage == AltosLib.MISSING) {
				mMainVoltageView.setVisibility(View.GONE);
				mMainVoltageLabel.setVisibility(View.GONE);
			} else {
				mMainVoltageView.setText(AltosDroid.number("%4.2f V", state.main_voltage));
				mMainVoltageView.setVisibility(View.VISIBLE);
				mMainVoltageLabel.setVisibility(View.VISIBLE);
			}
			mMainLights.set(state.main_voltage >= AltosLib.ao_igniter_good, state.main_voltage == AltosLib.MISSING);

			if (state.flight != 0) {
				if (state.state <= AltosLib.ao_flight_pad)
					mDataLoggingView.setText("Ready to record");
				else if (state.state < AltosLib.ao_flight_landed)
					mDataLoggingView.setText("Recording data");
				else
					mDataLoggingView.setText("Recorded data");
			} else {
				mDataLoggingView.setText("Storage full");
			}
			mDataLoggingLights.set(state.flight != 0, state.flight == AltosLib.MISSING);

			if (state.gps != null) {
				int soln = state.gps.nsat;
				int nsat = state.gps.cc_gps_sat != null ? state.gps.cc_gps_sat.length : 0;
				mGPSLockedView.setText(String.format("%4d in soln, %4d in view", soln, nsat));
				mGPSLockedLights.set(state.gps.locked && state.gps.nsat >= 4, false);
				if (state.gps_ready)
					mGPSReadyView.setText("Ready");
				else
					mGPSReadyView.setText(AltosDroid.integer("Waiting %d", state.gps_waiting));
			} else
				mGPSLockedLights.set(false, true);
			mGPSReadyLights.set(state.gps_ready, state.gps == null);
		}

		if (receiver != null) {
			double altitude = AltosLib.MISSING;
			if (receiver.hasAltitude())
				altitude = receiver.getAltitude();
			mPadLatitudeView.setText(AltosDroid.pos(receiver.getLatitude(), "N", "S"));
			mPadLongitudeView.setText(AltosDroid.pos(receiver.getLongitude(), "E", "W"));
			set_value(mPadAltitudeView, AltosConvert.height, 6, altitude);
		}
	}

}
