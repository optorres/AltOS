/*
 * Copyright © 2015 Keith Packard <keithp@keithp.com>
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
import java.io.*;
import java.text.*;

import org.altusmetrum.AltosDroid.R;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.*;
import android.widget.AdapterView.*;
import android.location.Location;
import android.location.LocationManager;
import android.location.LocationListener;
import android.location.Criteria;

import org.altusmetrum.altoslib_7.*;

/**
 * This Activity appears as a dialog. It lists any paired devices and
 * devices detected in the area after discovery. When a device is chosen
 * by the user, the MAC address of the device is sent back to the parent
 * Activity in the result Intent.
 */
public class PreloadMapActivity extends Activity implements AltosLaunchSiteListener, AltosMapInterface, AltosMapLoaderListener, LocationListener {

	private ArrayAdapter<AltosLaunchSite> known_sites_adapter;

	private CheckBox	hybrid;
	private CheckBox	satellite;
	private CheckBox	roadmap;
	private CheckBox	terrain;

	private Spinner		known_sites_spinner;
	private Spinner		min_zoom;
	private Spinner		max_zoom;
	private Spinner		tile_radius;

	private EditText	latitude;
	private EditText	longitude;

	private ProgressBar	progress;

	/* AltosMapLoaderListener interfaces */
	public void loader_start(final int max) {
		this.runOnUiThread(new Runnable() {
				public void run() {
					progress.setMax(max);
					progress.setProgress(0);
				}
			});
	}

	public void loader_notify(final int cur, final int max, final String name) {
		this.runOnUiThread(new Runnable() {
				public void run() {
					progress.setProgress(cur);
				}
			});
	}

	public void loader_done(int max) {
		this.runOnUiThread(new Runnable() {
				public void run() {
					progress.setProgress(0);
					finish();
				}
			});
	}

	/* AltosLaunchSiteListener interface */
	public void notify_launch_sites(final List<AltosLaunchSite> sites) {
		this.runOnUiThread(new Runnable() {
				public void run() {
					for (AltosLaunchSite site : sites)
						known_sites_adapter.add(site);
				}
			});
	}

	AltosMap	map;
	AltosMapLoader	loader;

	class PreloadMapImage implements AltosImage {
		public void flush() {
		}

		public PreloadMapImage(File file) {
		}
	}

	public AltosMapPath new_path() {
		return null;
	}

	public AltosMapLine new_line() {
		return null;
	}

	public AltosImage load_image(File file) throws Exception {
		return new PreloadMapImage(file);
	}

	public AltosMapMark new_mark(double lat, double lon, int state) {
		return null;
	}

	class PreloadMapTile extends AltosMapTile {
		public void paint(AltosMapTransform t) {
		}

		public PreloadMapTile(AltosMapTileListener listener, AltosLatLon upper_left, AltosLatLon center, int zoom, int maptype, int px_size) {
			super(listener, upper_left, center, zoom, maptype, px_size, 2);
		}

	}

	public AltosMapTile new_tile(AltosMapTileListener listener, AltosLatLon upper_left, AltosLatLon center, int zoom, int maptype, int px_size) {
		return new PreloadMapTile(listener, upper_left, center, zoom, maptype, px_size);
	}

	public int width() {
		return AltosMap.px_size;
	}

	public int height() {
		return AltosMap.px_size;
	}

	public void repaint() {
	}

	public void repaint(AltosRectangle damage) {
	}

	public void set_zoom_label(String label) {
	}

	public void debug(String format, Object ... arguments) {
		AltosDebug.debug(format, arguments);
	}

	/* LocationProvider interface */

	AltosLaunchSite	current_location_site;

	public void onLocationChanged(Location location) {
		AltosDebug.debug("location changed");
		if (current_location_site == null) {
			AltosLaunchSite selected_item = (AltosLaunchSite) known_sites_spinner.getSelectedItem();

			current_location_site = new AltosLaunchSite("Current Location", location.getLatitude(), location.getLongitude());
			known_sites_adapter.insert(current_location_site, 0);

			if (selected_item != null)
				known_sites_spinner.setSelection(known_sites_adapter.getPosition(selected_item));
			else {
				latitude.setText(new StringBuffer(String.format("%12.6f", current_location_site.latitude)));
				longitude.setText(new StringBuffer(String.format("%12.6f", current_location_site.longitude)));
			}
		} else {
			current_location_site.latitude = location.getLatitude();
			current_location_site.longitude = location.getLongitude();
		}
	}

	public void onStatusChanged(String provider, int status, Bundle extras) {
	}

	public void onProviderEnabled(String provider) {
	}

	public void onProviderDisabled(String provider) {
	}

	private double text(EditText view) throws ParseException {
		return AltosParse.parse_double_locale(view.getEditableText().toString());
	}

	private double latitude() throws ParseException {
		return text(latitude);
	}

	private double longitude() throws ParseException {
		return text(longitude);
	}

	private int value(Spinner spinner) {
		return (Integer) spinner.getSelectedItem();
	}

	private int min_z() {
		return value(min_zoom);
	}

	private int max_z() {
		return value(max_zoom);
	}

	private int radius() {
		return value(tile_radius);
	}

	private int bit(CheckBox box, int value) {
		if (box.isChecked())
			return 1 << value;
		return 0;
	}

	private int types() {
		return (bit(hybrid, AltosMap.maptype_hybrid) |
			bit(satellite, AltosMap.maptype_satellite) |
			bit(roadmap, AltosMap.maptype_roadmap) |
			bit(terrain, AltosMap.maptype_terrain));
	}

	private void load() {
		try {
			double	lat = latitude();
			double	lon = longitude();
			int	min = min_z();
			int	max = max_z();
			int	r = radius();
			int	t = types();

			loader.load(lat, lon, min, max, r, t);
		} catch (ParseException e) {
		}
	}

	private void add_numbers(Spinner spinner, int min, int max, int def) {

		ArrayAdapter<Integer> adapter = new ArrayAdapter<Integer>(this, android.R.layout.simple_spinner_item);

		int	spinner_def = 0;
		int	pos = 0;

		for (int i = min; i <= max; i++) {
			adapter.add(new Integer(i));
			if (i == def)
				spinner_def = pos;
			pos++;
		}

		spinner.setAdapter(adapter);
		spinner.setSelection(spinner_def);
	}

	class SiteListListener implements OnItemSelectedListener {
		public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
			AltosLaunchSite	site = (AltosLaunchSite) parent.getItemAtPosition(pos);
			latitude.setText(new StringBuffer(String.format("%12.6f", site.latitude)));
			longitude.setText(new StringBuffer(String.format("%12.6f", site.longitude)));
		}
		public void onNothingSelected(AdapterView<?> parent) {
		}

		public SiteListListener() {
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// Setup the window
		requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
		setContentView(R.layout.map_preload);

		// Set result CANCELED incase the user backs out
		setResult(Activity.RESULT_CANCELED);

		// Initialize the button to perform device discovery
		Button loadButton = (Button) findViewById(R.id.preload_load);
		loadButton.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				load();
			}
		});

		latitude = (EditText) findViewById(R.id.preload_latitude);
		longitude = (EditText) findViewById(R.id.preload_longitude);

		hybrid = (CheckBox) findViewById(R.id.preload_hybrid);
		satellite = (CheckBox) findViewById(R.id.preload_satellite);
		roadmap = (CheckBox) findViewById(R.id.preload_roadmap);
		terrain = (CheckBox) findViewById(R.id.preload_terrain);

		hybrid.setChecked(true);

		min_zoom = (Spinner) findViewById(R.id.preload_min_zoom);
		add_numbers(min_zoom,
			    AltosMap.min_zoom - AltosMap.default_zoom,
			    AltosMap.max_zoom - AltosMap.default_zoom, -2);
		max_zoom = (Spinner) findViewById(R.id.preload_max_zoom);
		add_numbers(max_zoom,
			    AltosMap.min_zoom - AltosMap.default_zoom,
			    AltosMap.max_zoom - AltosMap.default_zoom, 2);
		tile_radius = (Spinner) findViewById(R.id.preload_tile_radius);
		add_numbers(tile_radius, 1, 5, 5);

		progress = (ProgressBar) findViewById(R.id.preload_progress);

		// Initialize array adapters. One for already paired devices and
		// one for newly discovered devices
		known_sites_spinner = (Spinner) findViewById(R.id.preload_site_list);

		known_sites_adapter = new ArrayAdapter<AltosLaunchSite>(this, android.R.layout.simple_spinner_item);

		known_sites_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

		known_sites_spinner.setAdapter(known_sites_adapter);
		known_sites_spinner.setOnItemSelectedListener(new SiteListListener());

		map = new AltosMap(this);

		loader = new AltosMapLoader(map, this);

		// Listen for GPS and Network position updates
		LocationManager locationManager = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);

		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 1, this);

		new AltosLaunchSites(this);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		// Stop listening for location updates
		((LocationManager) getSystemService(Context.LOCATION_SERVICE)).removeUpdates(this);
	}
}