/*
 * Copyright © 2014 Keith Packard <keithp@keithp.com>
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

import android.content.Context;
import org.altusmetrum.altoslib_8.*;

public class AltosDroidPreferences extends AltosPreferences {

	/* Active device preference name */
	final static String activeDeviceAddressPreference = "ACTIVE-DEVICE-ADDRESS";
	final static String activeDeviceNamePreference = "ACTIVE-DEVICE-NAME";

	static DeviceAddress	active_device_address;

	/* Map source preference name */
	final static String mapSourcePreference = "MAP-SOURCE";

	static final int	MAP_SOURCE_OFFLINE = 0;
	static final int	MAP_SOURCE_ONLINE = 1;

	static int	map_source;

	public static void init(Context context) {
		if (backend != null)
			return;

		AltosPreferences.init(new AltosDroidPreferencesBackend(context));

		String address = backend.getString(activeDeviceAddressPreference, null);
		String name = backend.getString(activeDeviceNamePreference, null);

		if (address != null && name != null)
			active_device_address = new DeviceAddress (address, name);

		map_source = backend.getInt(mapSourcePreference, MAP_SOURCE_ONLINE);
	}

	public static void set_active_device(DeviceAddress address) {
		synchronized(backend) {
			active_device_address = address;
			backend.putString(activeDeviceAddressPreference, active_device_address.address);
			backend.putString(activeDeviceNamePreference, active_device_address.name);
			flush_preferences();
		}
	}

	public static DeviceAddress active_device() {
		synchronized(backend) {
			return active_device_address;
		}
	}

	public static void set_map_source(int map_source) {
		synchronized(backend) {
			AltosDroidPreferences.map_source = map_source;
			backend.putInt(mapSourcePreference, map_source);
			flush_preferences();
		}
	}

	public static int map_source() {
		synchronized(backend) {
			return map_source;
		}
	}
}
