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

import java.util.*;

import org.altusmetrum.altoslib_7.*;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.*;
import android.os.Bundle;
import android.support.v4.app.Fragment;
//import android.support.v4.app.FragmentTransaction;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.location.Location;

public class TabMap extends AltosDroidTab {
	private SupportMapFragment mMapFragment;
	private GoogleMap mMap;
	private boolean mapLoaded = false;

	private HashMap<Integer,Marker> rockets = new HashMap<Integer,Marker>();
	private Marker mPadMarker;
	private boolean pad_set;
	private Polyline mPolyline;

	private TextView mDistanceView;
	private TextView mBearingView;
	private TextView mTargetLatitudeView;
	private TextView mTargetLongitudeView;
	private TextView mReceiverLatitudeView;
	private TextView mReceiverLongitudeView;

	private double mapAccuracy = -1;

	private AltosLatLon my_position = null;
	private AltosLatLon target_position = null;

	private Bitmap rocket_bitmap(String text) {

		/* From: http://mapicons.nicolasmollet.com/markers/industry/military/missile-2/
		 */
		Bitmap orig_bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.rocket);
		Bitmap bitmap = orig_bitmap.copy(Bitmap.Config.ARGB_8888, true);

		Canvas canvas = new Canvas(bitmap);
		Paint paint = new Paint();
		paint.setTextSize(40);
		paint.setColor(0xff000000);

		Rect	bounds = new Rect();
		paint.getTextBounds(text, 0, text.length(), bounds);

		int	width = bounds.right - bounds.left;
		int	height = bounds.bottom - bounds.top;

		float x = bitmap.getWidth() / 2.0f - width / 2.0f;
		float y = bitmap.getHeight() / 2.0f - height / 2.0f;

		AltosDebug.debug("map label x %f y %f\n", x, y);

		canvas.drawText(text, 0, text.length(), x, y, paint);
		return bitmap;
	}

	private Marker rocket_marker(int serial, double lat, double lon) {
		Bitmap	bitmap = rocket_bitmap(String.format("%d", serial));

		return mMap.addMarker(new MarkerOptions()
				      .icon(BitmapDescriptorFactory.fromBitmap(bitmap))
				      .position(new LatLng(lat, lon))
				      .visible(true));
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mMapFragment = new SupportMapFragment() {
			@Override
			public void onActivityCreated(Bundle savedInstanceState) {
				super.onActivityCreated(savedInstanceState);
				setupMap();
			}
		};
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View v = inflater.inflate(R.layout.tab_map, container, false);
		mDistanceView  = (TextView)v.findViewById(R.id.distance_value);
		mBearingView   = (TextView)v.findViewById(R.id.bearing_value);
		mTargetLatitudeView  = (TextView)v.findViewById(R.id.target_lat_value);
		mTargetLongitudeView = (TextView)v.findViewById(R.id.target_lon_value);
		mReceiverLatitudeView  = (TextView)v.findViewById(R.id.receiver_lat_value);
		mReceiverLongitudeView = (TextView)v.findViewById(R.id.receiver_lon_value);
		return v;
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
		getChildFragmentManager().beginTransaction().add(R.id.map, mMapFragment).commit();
	}

	private void setupMap() {
		mMap = mMapFragment.getMap();
		if (mMap != null) {
			set_map_type(altos_droid.map_type);
			mMap.setMyLocationEnabled(true);
			mMap.getUiSettings().setTiltGesturesEnabled(false);
			mMap.getUiSettings().setZoomControlsEnabled(false);

			Bitmap label_bitmap = rocket_bitmap("hello");

			mPadMarker = mMap.addMarker(
					new MarkerOptions().icon(BitmapDescriptorFactory.fromResource(R.drawable.pad))
					                   .position(new LatLng(0,0))
					                   .visible(false)
					);

			mPolyline = mMap.addPolyline(
					new PolylineOptions().add(new LatLng(0,0), new LatLng(0,0))
					                     .width(20)
					                     .color(Color.BLUE)
					                     .visible(false)
					);

			mapLoaded = true;
		}
	}

	private void center(double lat, double lon, double accuracy) {
		if (mapAccuracy < 0 || accuracy < mapAccuracy/10) {
			mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(lat, lon),14));
			mapAccuracy = accuracy;
		}
	}

	public String tab_name() { return "map"; }

	private void set_rocket(int serial, AltosState state) {
		Marker	marker;

		if (state.gps == null || state.gps.lat == AltosLib.MISSING)
			return;

		if (rockets.containsKey(serial)) {
			marker = rockets.get(serial);
			marker.setPosition(new LatLng(state.gps.lat, state.gps.lon));
		} else {
			marker = rocket_marker(serial, state.gps.lat, state.gps.lon);
			rockets.put(serial, marker);
			marker.setVisible(true);
		}
	}

	private void remove_rocket(int serial) {
		Marker marker = rockets.get(serial);
		marker.remove();
		rockets.remove(serial);
	}

	public void show(TelemetryState telem_state, AltosState state, AltosGreatCircle from_receiver, Location receiver) {
		if (from_receiver != null) {
			mBearingView.setText(String.format("%3.0f°", from_receiver.bearing));
			set_value(mDistanceView, AltosConvert.distance, 6, from_receiver.distance);
		}

		if (telem_state != null) {
			for (int serial : rockets.keySet()) {
				if (!telem_state.states.containsKey(serial))
					remove_rocket(serial);
			}

			for (int serial : telem_state.states.keySet()) {
				set_rocket(serial, telem_state.states.get(serial));
			}
		}

		if (state != null) {
			if (mapLoaded) {
				if (!pad_set && state.pad_lat != AltosLib.MISSING) {
					pad_set = true;
					mPadMarker.setPosition(new LatLng(state.pad_lat, state.pad_lon));
					mPadMarker.setVisible(true);
				}
			}
			if (state.gps != null) {

				target_position = new AltosLatLon(state.gps.lat, state.gps.lon);

				mTargetLatitudeView.setText(AltosDroid.pos(state.gps.lat, "N", "S"));
				mTargetLongitudeView.setText(AltosDroid.pos(state.gps.lon, "E", "W"));
				if (state.gps.locked && state.gps.nsat >= 4)
					center (state.gps.lat, state.gps.lon, 10);
			}
		}

		if (receiver != null) {
			double accuracy;

			if (receiver.hasAccuracy())
				accuracy = receiver.getAccuracy();
			else
				accuracy = 1000;

			my_position = new AltosLatLon(receiver.getLatitude(), receiver.getLongitude());
			mReceiverLatitudeView.setText(AltosDroid.pos(my_position.lat, "N", "S"));
			mReceiverLongitudeView.setText(AltosDroid.pos(my_position.lon, "E", "W"));
			center (my_position.lat, my_position.lon, accuracy);
		}

		if (my_position != null && target_position != null) {
			mPolyline.setPoints(Arrays.asList(new LatLng(my_position.lat, my_position.lon), new LatLng(target_position.lat, target_position.lon)));
			mPolyline.setVisible(true);
		}

	}

	public void set_map_type(int map_type) {
		if (mMap != null) {
			if (map_type == AltosMap.maptype_hybrid)
				mMap.setMapType(GoogleMap.MAP_TYPE_HYBRID);
			else if (map_type == AltosMap.maptype_satellite)
				mMap.setMapType(GoogleMap.MAP_TYPE_SATELLITE);
			else if (map_type == AltosMap.maptype_terrain)
				mMap.setMapType(GoogleMap.MAP_TYPE_TERRAIN);
			else
				mMap.setMapType(GoogleMap.MAP_TYPE_NORMAL);
		}
	}

	public TabMap() {
	}
}
