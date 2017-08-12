/*
 * Copyright © 2017 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.altosuilib_12;

import java.io.*;
import java.util.ArrayList;

import java.awt.*;
import javax.swing.*;
import org.altusmetrum.altoslib_12.*;

import org.jfree.ui.*;
import org.jfree.chart.*;
import org.jfree.chart.plot.*;
import org.jfree.chart.axis.*;
import org.jfree.chart.renderer.*;
import org.jfree.chart.renderer.xy.*;
import org.jfree.chart.labels.*;
import org.jfree.data.xy.*;
import org.jfree.data.*;

class AltosUITime extends AltosUnits {
	public double value(double v, boolean imperial_units) { return v; }

	public double inverse(double v, boolean imperial_unis) { return v; }

	public String show_units(boolean imperial_units) { return "s"; }

	public String say_units(boolean imperial_units) { return "seconds"; }

	public int show_fraction(int width, boolean imperial_units) {
		if (width < 5)
			return 0;
		return width - 5;
	}

	public int say_fraction(boolean imperial_units) { return 0; }
}

class AltosXYSeries extends XYSeries {

	public AltosXYSeries(String label) {
		super(label);
	}
}

public class AltosUITimeSeries extends AltosTimeSeries implements AltosUIGrapher {
	Color		color;
	boolean		enable;
	AltosUIAxis	axis;
	boolean		marker;
	boolean		marker_top;
	XYItemRenderer	renderer;
	XYPlot		plot;
	AltosXYSeries	xy_series;
	ArrayList<ValueMarker>	markers;


	/* AltosUIGrapher interface */
	public boolean need_reset() {
		return false;
	}

	public void clear() {
	}

	public void add(AltosUIDataPoint dataPoint) {
	}

	public void setNotify(boolean notify) {
	}

	public void fireSeriesChanged() {
	}

	void set_data() {
		if (marker) {
			if (markers != null) {
				for (ValueMarker marker : markers)
					plot.removeDomainMarker(marker);
			}
			markers = new ArrayList<ValueMarker>();
			for (AltosTimeValue v : this) {
				String s = units.string_value(v.value);
				ValueMarker marker = new ValueMarker(v.time);
				marker.setLabel(s);
				if (marker_top) {
					marker.setLabelAnchor(RectangleAnchor.TOP_RIGHT);
					marker.setLabelTextAnchor(TextAnchor.TOP_LEFT);
				} else {
					marker.setLabelAnchor(RectangleAnchor.BOTTOM_RIGHT);
					marker.setLabelTextAnchor(TextAnchor.BOTTOM_LEFT);
				}
				marker.setPaint(color);
				if (enable)
					plot.addDomainMarker(marker);
				markers.add(marker);
			}
		} else {
			xy_series.clear();

			xy_series.setNotify(false);
			for (AltosTimeValue v : this) {
				double value = v.value;
				if (units != null)
					value = units.graph_value(value);
				xy_series.add(v.time, value);
			}
			xy_series.setNotify(true);
		}
	}

	public void set_units() {
		axis.set_units();
		StandardXYToolTipGenerator	ttg;

		if (units != null) {
			String	time_example = (new AltosUITime()).graph_format(7);
			String  example = units.graph_format(7);

			ttg = new StandardXYToolTipGenerator(String.format("{1}s: {2}%s ({0})",
									   units.graph_units()),
							     new java.text.DecimalFormat(time_example),
							     new java.text.DecimalFormat(example));
			renderer.setBaseToolTipGenerator(ttg);
		}
		set_data();
	}

	public AltosXYSeries xy_series() {
		return xy_series;
	}

	public void set_enable(boolean enable) {
		if (this.enable != enable) {
			this.enable = enable;
			if (marker) {
				for (ValueMarker marker : markers) {
					if (enable)
						plot.addDomainMarker(marker);
					else
						plot.removeDomainMarker(marker);
				}
			} else {
				renderer.setSeriesVisible(0, enable);
				axis.set_enable(enable);
			}
		}
	}

	public void set_axis(Color color, boolean enable, AltosUIAxis axis) {
		this.color = color;
		this.enable = enable;
		this.axis = axis;
		this.marker = false;

		axis.ref(this.enable);

		renderer = new XYLineAndShapeRenderer(true, false);
		renderer.setSeriesPaint(0, color);
		renderer.setSeriesStroke(0, new BasicStroke(2, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		renderer.setSeriesVisible(0, enable);
		xy_series = new AltosXYSeries(label);
	}

	public void set_marker(Color color, boolean enable, XYPlot plot, boolean marker_top) {
		this.color = color;
		this.enable = enable;
		this.marker = true;
		this.plot = plot;
		this.marker_top = marker_top;
	}

	public AltosUITimeSeries(String label, AltosUnits units) {
		super(label, units);
	}

	public AltosUITimeSeries(String label, AltosUnits units,
				 Color color, boolean enable,
				 AltosUIAxis axis) {
		this(label, units);
		set_axis(color, enable, axis);
	}
}
