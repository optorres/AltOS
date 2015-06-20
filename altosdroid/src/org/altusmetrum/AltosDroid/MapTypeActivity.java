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

import org.altusmetrum.altoslib_7.*;

public class MapTypeActivity extends Activity {
	private Button hybrid;
	private Button satellite;
	private Button roadmap;
	private Button terrain;
	private int selected_type;

	public static final String EXTRA_MAP_TYPE = "map_type";

	private void done(int type) {

		Intent intent = new Intent();
		intent.putExtra(EXTRA_MAP_TYPE, type);
		setResult(Activity.RESULT_OK, intent);
		finish();
	}

	public void selectType(View view) {
		AltosDebug.debug("selectType %s", view.toString());
		if (view == hybrid)
			done(AltosMap.maptype_hybrid);
		if (view == satellite)
			done(AltosMap.maptype_satellite);
		if (view == roadmap)
			done(AltosMap.maptype_roadmap);
		if (view == terrain)
			done(AltosMap.maptype_terrain);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// Setup the window
		requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
		setContentView(R.layout.map_type);

		hybrid = (Button) findViewById(R.id.map_type_hybrid);
		satellite = (Button) findViewById(R.id.map_type_satellite);
		roadmap = (Button) findViewById(R.id.map_type_roadmap);
		terrain = (Button) findViewById(R.id.map_type_terrain);

		// Set result CANCELED incase the user backs out
		setResult(Activity.RESULT_CANCELED);
	}
}