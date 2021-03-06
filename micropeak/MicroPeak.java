/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.micropeak;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;
import java.util.concurrent.*;
import java.util.*;
import org.altusmetrum.altoslib_14.*;
import org.altusmetrum.altosuilib_14.*;

public class MicroPeak extends MicroFrame implements ActionListener, ItemListener, AltosFilterListener {

	File		filename;
	AltosGraph	graph;
	AltosUIEnable	enable;
	AltosFlightStatsTable	statsTable;
	MicroRaw	raw;
	MicroData	data;
	Container	container;
	JTabbedPane	pane;
	static int	number_of_windows;

	MicroPeak SetData(MicroData data) {
		MicroPeak	mp = this;
		if (this.data != null) {
			mp = new MicroPeak();
			return mp.SetData(data);
		}
		this.data = data;
		if (data.flight_series == null)
			System.out.printf("no data in flight\n");
		if (data.flight_stats == null)
			System.out.printf("no stats in flight\n");
		graph.set_data(data.flight_stats, data.flight_series);
		statsTable.set_stats(data.flight_stats);
		raw.setData(data);
		setTitle(data.name);
		return this;
	}

	void SetName(String name) {
		graph.setName(name);
		setTitle(name);
	}

	private static MicroData ReadFile(File filename) throws IOException, FileNotFoundException {
		MicroData	data = null;
		FileInputStream	fis = new FileInputStream(filename);
		try {
			data = new MicroData((InputStream) fis, filename.getName());
			AltosUIPreferences.set_last_logdir(filename);
		} catch (MicroData.NonHexcharException nhe) {
			data = null;
		} catch (MicroData.FileEndedException nhe) {
			data = null;
		} catch (InterruptedException ie) {
			data = null;
		} finally {
			fis.close();
		}
		return data;
	}

	private void OpenFile(File filename) {
		try {
			SetData(ReadFile(filename));
		} catch (FileNotFoundException fne) {
			JOptionPane.showMessageDialog(this,
						      fne.getMessage(),
						      "Cannot open file",
						      JOptionPane.ERROR_MESSAGE);
		} catch (IOException ioe) {
			JOptionPane.showMessageDialog(this,
						      ioe.getMessage(),
						      "File Read Error",
						      JOptionPane.ERROR_MESSAGE);
		}
	}

	private void SelectFile() {
		MicroFileChooser	chooser = new MicroFileChooser(this);
		File			file = chooser.runDialog();

		if (file != null)
			OpenFile(file);
	}

	private void Preferences() {
		new AltosUIConfigure(this);
	}

	private void DownloadData() {
		AltosDevice	device = MicroDeviceDialog.show(this);

		if (device != null)
			new MicroDownload(this, device);
	}

	private void no_data() {
			JOptionPane.showMessageDialog(this,
						      "No data available",
						      "No data",
						      JOptionPane.INFORMATION_MESSAGE);
	}

	private void Save() {
		if (data == null) {
			no_data();
			return;
		}
		MicroSave	save = new MicroSave (this, data);
		if (save.runDialog())
			SetName(data.name);
	}

	private void Export() {
		if (data == null) {
			no_data();
			return;
		}
		MicroExport	export = new MicroExport (this, data);
		export.runDialog();
	}

	private static void CommandGraph(File file) {
		MicroPeak m = new MicroPeak();
		m.OpenFile(file);
	}

	private static void CommandExport(File file) {
		try {
			MicroData d = ReadFile(file);
			if (d != null) {
				File	csv = new File(AltosLib.replace_extension(file.getPath(), ".csv"));
				try {
					System.out.printf ("Export \"%s\" to \"%s\"\n", file.getPath(), csv.getPath());
					MicroExport.export(csv, d);
				} catch (FileNotFoundException fe) {
					System.err.printf("Cannot create file \"%s\" (%s)\n", csv.getName(), fe.getMessage());
				} catch (IOException ie) {
					System.err.printf("Cannot write file \"%s\" (%s)\n", csv.getName(), ie.getMessage());
				}
			}
		} catch (IOException ie) {
			System.err.printf("Cannot read file \"%s\" (%s)\n", file.getName(), ie.getMessage());
		}
	}

	private void Close() {
		setVisible(false);
		dispose();
		--number_of_windows;
		if (number_of_windows == 0)
			System.exit(0);
	}

	public void actionPerformed(ActionEvent ev) {
		if ("Exit".equals(ev.getActionCommand()))
			System.exit(0);
		else if ("Close".equals(ev.getActionCommand()))
			Close();
		else if ("Open".equals(ev.getActionCommand()))
			SelectFile();
		else if ("Download".equals(ev.getActionCommand()))
			DownloadData();
		else if ("Export".equals(ev.getActionCommand()))
			Export();
		else if ("Preferences".equals(ev.getActionCommand()))
			Preferences();
		else if ("Save a Copy".equals(ev.getActionCommand()))
			Save();
	}

	public void itemStateChanged(ItemEvent e) {
	}

	/* OSXAdapter interfaces */
	public void macosx_file_handler(String path) {
		CommandGraph(new File(path));
	}

	public void macosx_quit_handler() {
		System.exit(0);
	}

	public void macosx_preferences_handler() {
		Preferences();
	}

	public void filter_changed(double speed_filter, double accel_filter) {
		data.flight_series.set_filter(speed_filter, accel_filter);
		graph.filter_changed();
		data.flight_stats = new AltosFlightStats(data.flight_series);
		statsTable.filter_changed(data.flight_stats);
	}

	public double speed_filter() {
		if (data != null && data.flight_series != null)
			return data.flight_series.speed_filter_width;
		return 4.0;
	}

	public double accel_filter() {
		if (data != null && data.flight_series != null)
			return data.flight_series.accel_filter_width;
		return 1.0;
	}

	public MicroPeak() {

		++number_of_windows;

		register_for_macosx_events();

		AltosUIPreferences.set_component(this);

		container = getContentPane();
		pane = new JTabbedPane();

		setTitle("MicroPeak");

		JMenuBar menuBar = new JMenuBar();
		setJMenuBar(menuBar);

		JMenu fileMenu = new JMenu("File");
		menuBar.add(fileMenu);

		JMenuItem openAction = new JMenuItem("Open");
		fileMenu.add(openAction);
		openAction.addActionListener(this);

		JMenuItem downloadAction = new JMenuItem("Download");
		fileMenu.add(downloadAction);
		downloadAction.addActionListener(this);

		JMenuItem saveAction = new JMenuItem("Save a Copy");
		fileMenu.add(saveAction);
		saveAction.addActionListener(this);

		JMenuItem exportAction = new JMenuItem("Export");
		fileMenu.add(exportAction);
		exportAction.addActionListener(this);

		JMenuItem preferencesAction = new JMenuItem("Preferences");
		fileMenu.add(preferencesAction);
		preferencesAction.addActionListener(this);

		JMenuItem closeAction = new JMenuItem("Close");
		fileMenu.add(closeAction);
		closeAction.addActionListener(this);

		JMenuItem exitAction = new JMenuItem("Exit");
		fileMenu.add(exitAction);
		exitAction.addActionListener(this);

		JButton downloadButton = new JButton ("Download");
		downloadButton.addActionListener(this);
		menuBar.add(downloadButton);

		setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				statsTable.tell_closing();
				raw.tell_closing();
				Close();
			}
		});

		enable = new AltosUIEnable(this);

		graph = new AltosGraph(enable);
		statsTable = new AltosFlightStatsTable();
		raw = new MicroRaw();
		pane.add(graph.panel, "Graph");
		pane.add(enable, "Configure Graph");
		pane.add(statsTable, "Statistics");
		JScrollPane scroll = new JScrollPane(raw);
		pane.add(scroll, "Raw Data");
		pane.doLayout();
		pane.validate();
		container.add(pane);
		container.doLayout();
		container.validate();
		doLayout();
		validate();
		Insets i = getInsets();
		Dimension ps = pane.getPreferredSize();
		ps.width += i.left + i.right;
		ps.height += i.top + i.bottom;
//		setPreferredSize(ps);
		setSize(ps);
		setVisible(true);
	}

	public static void help(int code) {
		System.out.printf("Usage: micropeak [OPTION] ... [FILE]...\n");
		System.out.printf("  Options:\n");
		System.out.printf("    --csv\tgenerate comma separated output for spreadsheets, etc\n");
		System.out.printf("    --graph\tgraph a flight\n");
		System.exit(code);
	}

	public static void main(final String[] args) {
		boolean	opened = false;
		boolean graphing = true;

		try {
			UIManager.setLookAndFeel(AltosUIPreferences.look_and_feel());
		} catch (Exception e) {
		}

		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("--help"))
				help(0);
			else if (args[i].equals("--export"))
				graphing = false;
			else if (args[i].equals("--graph"))
				graphing = true;
			else if (args[i].startsWith("--"))
				help(1);
			else {
				File	file = new File(args[i]);
				try {
					if (graphing)
						CommandGraph(file);
					else
						CommandExport(file);
					opened = true;
				} catch (Exception e) {
					System.err.printf("Error processing \"%s\": %s %s\n",
							  file.getName(), e.toString(), e.getMessage());
					e.printStackTrace();
				}
			}
		}
		if (!opened)
			new MicroPeak();
	}
}
