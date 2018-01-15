/*
 * Copyright © 2010 Anthony Towns
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

package teststand;

import java.io.*;
import java.util.ArrayList;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import org.altusmetrum.altoslib_12.*;
import org.altusmetrum.altosuilib_12.*;

import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.ui.RefineryUtilities;

public class AltosGraphUI extends AltosUIFrame implements AltosFontListener, AltosUnitsListener, AltosFilterListener
{
	JTabbedPane		pane;
	TestStandGraph		graph;
	AltosUIEnable		enable;
	AltosFlightStats	stats;
	AltosFlightStatsTable	statsTable;
	AltosGPS		gps;
	boolean			has_gps;

	public void font_size_changed(int font_size) {
		if (statsTable != null)
			statsTable.font_size_changed(font_size);
	}

	public void units_changed(boolean imperial_units) {
		if (enable != null)
			enable.units_changed(imperial_units);
	}

	AltosUIFlightSeries flight_series;

	public void filter_changed(double speed_filter, double accel_filter) {
		flight_series.set_filter(speed_filter, accel_filter);
		graph.filter_changed();
		stats = new AltosFlightStats(flight_series);
		statsTable.filter_changed(stats);
	}

	public double speed_filter() {
		return flight_series.speed_filter_width;
	}

	public double accel_filter() {
		return flight_series.accel_filter_width;
	}

	AltosGraphUI(AltosRecordSet set, File file) throws InterruptedException, IOException {
		super(file.getName());
		AltosCalData	cal_data = set.cal_data();


		pane = new JTabbedPane();

		flight_series = new AltosUIFlightSeries(cal_data);

		enable = new AltosUIEnable(this);

		set.capture_series(flight_series);

		flight_series.finish();

		stats = new AltosFlightStats(flight_series);

		graph = new TestStandGraph(enable, stats, flight_series);

		statsTable = new AltosFlightStatsTable(stats);

		pane.add("Test Graph", graph.panel);
		pane.add("Configure Graph", enable);
		pane.add("Test Statistics", statsTable);

		has_gps = false;

		setContentPane (pane);

		AltosUIPreferences.register_font_listener(this);
		AltosPreferences.register_units_listener(this);

		addWindowListener(new WindowAdapter() {
				@Override
				public void windowClosing(WindowEvent e) {
					AltosUIPreferences.unregister_font_listener(AltosGraphUI.this);
					AltosPreferences.unregister_units_listener(AltosGraphUI.this);
				}
			});
		pack();

		setVisible(true);
	}
}
