/*
 * Copyright © 2010 Keith Packard <keithp@keithp.com>
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

package altosui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;
import java.util.concurrent.*;
import java.util.Arrays;
import org.altusmetrum.altoslib_6.*;
import org.altusmetrum.altosuilib_6.*;

public class AltosIdleMonitorUI extends AltosUIFrame implements AltosFlightDisplay, AltosIdleMonitorListener, DocumentListener {
	AltosDevice		device;
	JTabbedPane		pane;
	AltosPad		pad;
	AltosInfoTable		flightInfo;
	AltosFlightStatus	flightStatus;
	AltosIgnitor		ignitor;
	AltosIdleMonitor	thread;
	AltosUIMap      	sitemap;
	int			serial;
	boolean			remote;
	boolean			has_ignitor;
	boolean			has_map;

	void stop_display() {
		if (thread != null) {
			try {
				thread.abort();
			} catch (InterruptedException ie) {
			}
		}
		thread = null;
	}

	void disconnect() {
		stop_display();
	}

	public void reset() {
		pad.reset();
		flightInfo.clear();
	}

	public void font_size_changed(int font_size) {
		pad.font_size_changed(font_size);
		flightInfo.font_size_changed(font_size);
	}

	public void units_changed(boolean imperial_units) {
		pad.units_changed(imperial_units);
		flightInfo.units_changed(imperial_units);
	}

	AltosFlightStatusUpdate	status_update;

	public void show(AltosState state, AltosListenerState listener_state) {
		status_update.saved_state = state;
		if (ignitor.should_show(state)) {
			if (!has_ignitor) {
				pane.add("Ignitor", ignitor);
				has_ignitor = true;
			}
		} else {
			if (has_ignitor) {
				pane.remove(ignitor);
				has_ignitor = false;
			}
		}
		if (state.gps != null && state.gps.connected) {
			if (!has_map) {
				pane.add("Site Map", sitemap);
				has_map = true;
			}
		} else {
			if (has_map) {
				pane.remove(sitemap);
				has_map = false;
			}
		}

//		try {
			pad.show(state, listener_state);
			flightStatus.show(state, listener_state);
			flightInfo.show(state, listener_state);
			if (has_ignitor)
				ignitor.show(state, listener_state);
			if (has_map)
				sitemap.show(state, listener_state);
//		} catch (Exception e) {
//			System.out.print("Show exception " + e);
//		}
	}

	public void update(final AltosState state, final AltosListenerState listener_state) {
		Runnable r = new Runnable() {
				public void run() {
					show(state, listener_state);
				}
			};
		SwingUtilities.invokeLater(r);
	}

	public void failed() {
		Runnable r = new Runnable() {
				public void run() {
					close();
				}
			};
		SwingUtilities.invokeLater(r);
	}

	Container	bag;
	AltosUIFreqList	frequencies;
	JTextField	callsign_value;

	/* DocumentListener interface methods */
	public void changedUpdate(DocumentEvent e) {
		if (callsign_value != null) {
			String	callsign = callsign_value.getText();
			thread.set_callsign(callsign);
			AltosUIPreferences.set_callsign(callsign);
		}
	}

	public void insertUpdate(DocumentEvent e) {
		changedUpdate(e);
	}

	public void removeUpdate(DocumentEvent e) {
		changedUpdate(e);
	}

	int row = 0;

	public GridBagConstraints constraints (int x, int width, int fill) {
		GridBagConstraints c = new GridBagConstraints();
		Insets insets = new Insets(4, 4, 4, 4);

		c.insets = insets;
		c.fill = fill;
		if (width == 3)
			c.anchor = GridBagConstraints.CENTER;
		else if (x == 2)
			c.anchor = GridBagConstraints.EAST;
		else
			c.anchor = GridBagConstraints.WEST;
		c.gridx = x;
		c.gridwidth = width;
		c.gridy = row;
		return c;
	}

	public GridBagConstraints constraints(int x, int width) {
		return constraints(x, width, GridBagConstraints.NONE);
	}

	void idle_exception(JFrame owner, Exception e) {
		if (e instanceof FileNotFoundException) {
			JOptionPane.showMessageDialog(owner,
						      ((FileNotFoundException) e).getMessage(),
						      "Cannot open target device",
						      JOptionPane.ERROR_MESSAGE);
		} else if (e instanceof AltosSerialInUseException) {
			JOptionPane.showMessageDialog(owner,
						      String.format("Device \"%s\" already in use",
								    device.toShortString()),
						      "Device in use",
						      JOptionPane.ERROR_MESSAGE);
		} else if (e instanceof IOException) {
			IOException ee = (IOException) e;
			JOptionPane.showMessageDialog(owner,
						      device.toShortString(),
						      ee.getLocalizedMessage(),
						      JOptionPane.ERROR_MESSAGE);
		} else {
			JOptionPane.showMessageDialog(owner,
						      String.format("Connection to \"%s\" failed",
								    device.toShortString()),
						      "Connection Failed",
						      JOptionPane.ERROR_MESSAGE);
		}
	}

	private void close() {
		try {
			disconnect();
		} catch (Exception ex) {
			System.out.printf("Exception %s\n", ex.toString());
			for (StackTraceElement el : ex.getStackTrace())
				System.out.printf("%s\n", el.toString());
		}
		setVisible(false);
		dispose();
		AltosUIPreferences.unregister_font_listener(AltosIdleMonitorUI.this);
	}

	public AltosIdleMonitorUI(JFrame in_owner)
		throws FileNotFoundException, TimeoutException, InterruptedException {

		device = AltosDeviceUIDialog.show(in_owner, Altos.product_any);
		remote = false;
		if (!device.matchProduct(Altos.product_altimeter))
			remote = true;

		serial = device.getSerial();

		AltosSerial link;
		try {
			link = new AltosSerial(device);
			link.set_frame(this);
		} catch (Exception ex) {
			idle_exception(in_owner, ex);
			return;
		}

		bag = getContentPane();
		bag.setLayout(new GridBagLayout());

		setTitle(String.format("AltOS %s", device.toShortString()));

		/* Stick frequency selector at top of table for telemetry monitoring */
		if (remote && serial >= 0) {
			// Frequency menu
			frequencies = new AltosUIFreqList(AltosUIPreferences.frequency(serial));
			frequencies.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						double frequency = frequencies.frequency();
						thread.set_frequency(frequency);
						AltosUIPreferences.set_frequency(device.getSerial(),
									       frequency);
					}
			});
			bag.add (frequencies, constraints(0, 1));
			bag.add (new JLabel("Callsign:"), constraints(1, 1));
			/* Add callsign configuration */
			callsign_value = new JTextField(AltosUIPreferences.callsign());
			callsign_value.getDocument().addDocumentListener(this);
			callsign_value.setToolTipText("Callsign sent in packet mode");
			bag.add(callsign_value, constraints(2, 1, GridBagConstraints.BOTH));
			row++;
		}


		/* Flight status is always visible */
		flightStatus = new AltosFlightStatus();
		bag.add(flightStatus, constraints(0, 3, GridBagConstraints.HORIZONTAL));
		row++;

		/* The rest of the window uses a tabbed pane to
		 * show one of the alternate data views
		 */
		pane = new JTabbedPane();

		pad = new AltosPad();
		pane.add("Launch Pad", pad);

		flightInfo = new AltosInfoTable();
		pane.add("Table", new JScrollPane(flightInfo));

		ignitor = new AltosIgnitor();

		sitemap = new AltosUIMap();

		/* Make the tabbed pane use the rest of the window space */
		bag.add(pane, constraints(0, 3, GridBagConstraints.BOTH));

		setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

		AltosUIPreferences.register_font_listener(this);

		addWindowListener(new WindowAdapter() {
				@Override
				public void windowClosing(WindowEvent e) {
					close();
				}
			});

		pack();
		setVisible(true);

		thread = new AltosIdleMonitor((AltosIdleMonitorListener) this, link, (boolean) remote);

		status_update = new AltosFlightStatusUpdate(flightStatus);

		new javax.swing.Timer(100, status_update).start();

		thread.start();
	}
}
