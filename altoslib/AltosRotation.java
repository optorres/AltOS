/*
 * Copyright © 2014 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.altoslib_13;

public class AltosRotation extends AltosQuaternion {
	private AltosQuaternion		rotation;

	/* Compute pitch angle from vertical by taking the pad
	 * orientation vector and rotating it by the current total
	 * rotation value. That will be a unit vector pointing along
	 * the airframe axis. The Z value will be the cosine of the
	 * angle from vertical.
	 *
	 * rot = ao_rotation * vertical * ao_rotation°
	 * rot = ao_rotation * (0,0,0,1) * ao_rotation°
	 *     = ((a.z, a.y, -a.x, a.r) * (a.r, -a.x, -a.y, -a.z)) .z
	 *
	 *     = (-a.z * -a.z) + (a.y * -a.y) - (-a.x * -a.x) + (a.r * a.r)
	 *     = a.z² - a.y² - a.x² + a.r²
	 *
	 * rot = ao_rotation * (0, 0, 0, -1) * ao_rotation°
	 *     = ((-a.z, -a.y, a.x, -a.r) * (a.r, -a.x, -a.y, -a.z)) .z
	 *
	 *     = (a.z * -a.z) + (-a.y * -a.y) - (a.x * -a.x) + (-a.r * a.r)
	 *     = -a.z² + a.y² + a.x² - a.r²
	 *
	 * tilt = acos(rot)  (in radians)
	 */

	public double tilt() {
		double	rotz = rotation.z * rotation.z - rotation.y * rotation.y - rotation.x * rotation.x + rotation.r * rotation.r;

		double tilt = Math.acos(rotz) * 180.0 / Math.PI;
		return tilt;
	}

	/* Given euler rotations in three axes, perform a combined rotation using
	 * quaternions
	 */
	public void rotate(double x, double y, double z) {
		AltosQuaternion	rot = AltosQuaternion.euler(x, y, z);
		rotation = rot.multiply(rotation).normalize();
	}

	/* Clone an existing rotation value */
	public AltosRotation (AltosRotation old) {
		this.rotation = new AltosQuaternion(old.rotation);
	}

	/* Create a new rotation value given an acceleration vector pointing down */
	public AltosRotation(double x,
			     double y,
			     double z,
			     int pad_orientation) {
		AltosQuaternion	orient = AltosQuaternion.vector(x, y, z).normalize();
		double sky = pad_orientation == 0 ? 1 : -1;
		AltosQuaternion	up = new AltosQuaternion(0, 0, 0, sky);
		rotation = up.vectors_to_rotation(orient);
	}

	public AltosRotation() {
		rotation = new AltosQuaternion();
	}
}
