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

package org.altusmetrum.altoslib_8;

public class AltosLatLon {
	public double	lat;
	public double	lon;

	public int hashCode() {
		return new Double(lat).hashCode() ^ new Double(lon).hashCode();
	}

	public boolean equals(Object o) {
		if (o == null)
			return false;
		if (!(o instanceof AltosLatLon))
			return false;

		AltosLatLon other = (AltosLatLon) o;
		return lat == other.lat && lon == other.lon;
	}

	public String toString() {
		return String.format("%f/%f", lat, lon);
	}

	public AltosLatLon(double lat, double lon) {
		this.lat = lat;
		this.lon = lon;
	}
}
