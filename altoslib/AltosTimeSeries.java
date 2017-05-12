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

public class AltosTimeSeries implements Iterable<AltosTimeValue> {
	public String		label;
	public AltosUnits	units;
	List<AltosTimeValue>	values;

	public void add(double x, double y) {
		values.add(new AltosTimeValue(x, y));
	}

	public Iterator<AltosTimeValue> iterator() {
		return values.iterator();
	}

	public void integrate(AltosTimeSeries integral) {
		double	y = 0.0;
		double 	x = 0.0;
		boolean start = true;

		for (AltosTimeValue v : values) {
			if (start) {
				y = 0.0;
				x = v.x;
				start = false;
			} else {
				y += v.y * (v.x - x);
				x = v.x;
			}
			integral.add(x, y);
		}
	}

	public void differentiate(AltosTimeSeries diff) {
		double y = 0.0;
		double x = 0.0;
		boolean start = true;

		for (AltosTimeValue v: values) {
			if (start) {
				y = 0.0;
				x = v.x;
				start = false;
			} else {
				double	dx = v.x - x;
				double	dy = v.y - y;

				x = v.x;
				y = v.y;
				if (dx != 0)
					diff.add(x, dy);
			}
		}
	}

	private int find_left(int i, double dt) {
		int j;
		double t = values.get(i).x - dt;
		for (j = i; j > 0; j--)	{
			if (values.get(j).x < t)
				break;
		}
		return j;

	}

	public void filter(AltosTimeSeries out, double width) {
		for (int i = 0; i < values.size(); i++) {
		}
	}

	public AltosTimeSeries(String label, AltosUnits units) {
		this.label = label;
		this.units = units;
		this.values = new ArrayList<AltosTimeValue>();
	}
}
