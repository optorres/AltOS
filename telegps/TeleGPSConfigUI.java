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

package org.altusmetrum.telegps;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import org.altusmetrum.altoslib_4.*;
import org.altusmetrum.altosuilib_2.*;

public class TeleGPSConfigUI
	extends AltosUIDialog
	implements ActionListener, ItemListener, DocumentListener, AltosConfigValues, AltosUnitsListener
{

	Container		pane;
	JLabel			product_label;
	JLabel			version_label;
	JLabel			serial_label;
	JLabel			frequency_label;
	JLabel			radio_calibration_label;
	JLabel			radio_frequency_label;
	JLabel			radio_enable_label;
	JLabel			aprs_interval_label;
	JLabel			flight_log_max_label;
	JLabel			callsign_label;
	JLabel			tracker_horiz_label;
	JLabel			tracker_vert_label;

	public boolean		dirty;

	JFrame			owner;
	JLabel			product_value;
	JLabel			version_value;
	JLabel			serial_value;
	AltosFreqList		radio_frequency_value;
	JTextField		radio_calibration_value;
	JRadioButton		radio_enable_value;
	JComboBox<String>	aprs_interval_value;
	JComboBox<String>	flight_log_max_value;
	JTextField		callsign_value;
	JComboBox<String>	tracker_horiz_value;
	JComboBox<String>	tracker_vert_value;

	JButton			save;
	JButton			reset;
	JButton			reboot;
	JButton			close;

	ActionListener		listener;

	static String[] 	flight_log_max_values = {
		"64", "128", "192", "256", "320",
		"384", "448", "512", "576", "640",
		"704", "768", "832", "896", "960",
	};

	static String[] 	aprs_interval_values = {
		"Disabled",
		"2",
		"5",
		"10"
	};

	static String[]		tracker_horiz_values_m = {
		"250",
		"500",
		"1000",
		"2000"
	};

	static String[]		tracker_horiz_values_ft = {
		"500",
		"1000",
		"2500",
		"5000"
	};

	static String[]		tracker_vert_values_m = {
		"25",
		"50",
		"100",
		"200"
	};

	static String[]		tracker_vert_values_ft = {
		"50",
		"100",
		"250",
		"500"
	};

	/* A window listener to catch closing events and tell the config code */
	class ConfigListener extends WindowAdapter {
		TeleGPSConfigUI	ui;

		public ConfigListener(TeleGPSConfigUI this_ui) {
			ui = this_ui;
		}

		public void windowClosing(WindowEvent e) {
			ui.actionPerformed(new ActionEvent(e.getSource(),
							   ActionEvent.ACTION_PERFORMED,
							   "Close"));
		}
	}

	public void set_pyros(AltosPyro[] new_pyros) {
	}

	public AltosPyro[] pyros() {
		return null;
	}

	boolean is_telemetrum() {
		String	product = product_value.getText();
		return product != null && product.startsWith("TeleGPS");
	}

	void set_radio_calibration_tool_tip() {
		if (radio_calibration_value.isEnabled())
			radio_calibration_value.setToolTipText("Tune radio output to match desired frequency");
		else
			radio_calibration_value.setToolTipText("Cannot tune radio while connected over packet mode");
	}

	void set_radio_enable_tool_tip() {
		if (radio_enable_value.isEnabled())
			radio_enable_value.setToolTipText("Enable/Disable telemetry and RDF transmissions");
		else
			radio_enable_value.setToolTipText("Firmware version does not support disabling radio");
	}

	void set_aprs_interval_tool_tip() {
		if (aprs_interval_value.isEnabled())
			aprs_interval_value.setToolTipText("Enable APRS and set the interval between APRS reports");
		else
			aprs_interval_value.setToolTipText("Hardware doesn't support APRS");
	}

	void set_flight_log_max_tool_tip() {
		if (flight_log_max_value.isEnabled())
			flight_log_max_value.setToolTipText("Size reserved for each flight log (in kB)");
		else
			flight_log_max_value.setToolTipText("Cannot set max value with flight logs in memory");
	}

	/* Build the UI using a grid bag */
	public TeleGPSConfigUI(JFrame in_owner) {
		super (in_owner, "Configure Flight Computer", false);

		owner = in_owner;
		GridBagConstraints c;
		int row = 0;

		Insets il = new Insets(4,4,4,4);
		Insets ir = new Insets(4,4,4,4);

		pane = getContentPane();
		pane.setLayout(new GridBagLayout());

		/* Product */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		product_label = new JLabel("Product:");
		pane.add(product_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		product_value = new JLabel("");
		pane.add(product_value, c);
		row++;

		/* Version */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		version_label = new JLabel("Software version:");
		pane.add(version_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		version_value = new JLabel("");
		pane.add(version_value, c);
		row++;

		/* Serial */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		serial_label = new JLabel("Serial:");
		pane.add(serial_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		serial_value = new JLabel("");
		pane.add(serial_value, c);
		row++;

		/* Frequency */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		radio_frequency_label = new JLabel("Frequency:");
		pane.add(radio_frequency_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		radio_frequency_value = new AltosFreqList();
		radio_frequency_value.addItemListener(this);
		pane.add(radio_frequency_value, c);
		radio_frequency_value.setToolTipText("Telemetry, RDF and packet frequency");
		row++;

		/* Radio Calibration */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		radio_calibration_label = new JLabel("RF Calibration:");
		pane.add(radio_calibration_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		radio_calibration_value = new JTextField(String.format("%d", 1186611));
		radio_calibration_value.getDocument().addDocumentListener(this);
		pane.add(radio_calibration_value, c);
		set_radio_calibration_tool_tip();
		row++;

		/* Radio Enable */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		radio_enable_label = new JLabel("Telemetry/RDF/APRS Enable:");
		pane.add(radio_enable_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		radio_enable_value = new JRadioButton("Enabled");
		radio_enable_value.addItemListener(this);
		pane.add(radio_enable_value, c);
		set_radio_enable_tool_tip();
		row++;

		/* APRS interval */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		aprs_interval_label = new JLabel("APRS Interval(s):");
		pane.add(aprs_interval_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		aprs_interval_value = new JComboBox<String>(aprs_interval_values);
		aprs_interval_value.setEditable(true);
		aprs_interval_value.addItemListener(this);
		pane.add(aprs_interval_value, c);
		set_aprs_interval_tool_tip();
		row++;

		/* Callsign */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		callsign_label = new JLabel("Callsign:");
		pane.add(callsign_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		callsign_value = new JTextField(AltosUIPreferences.callsign());
		callsign_value.getDocument().addDocumentListener(this);
		pane.add(callsign_value, c);
		callsign_value.setToolTipText("Callsign reported in telemetry data");
		row++;

		/* Flight log max */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		flight_log_max_label = new JLabel("Maximum Flight Log Size:");
		pane.add(flight_log_max_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		flight_log_max_value = new JComboBox<String>(flight_log_max_values);
		flight_log_max_value.setEditable(true);
		flight_log_max_value.addItemListener(this);
		pane.add(flight_log_max_value, c);
		set_flight_log_max_tool_tip();
		row++;

		/* Tracker triger horiz distances */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		tracker_horiz_label = new JLabel(get_tracker_horiz_label());
		pane.add(tracker_horiz_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		tracker_horiz_value = new JComboBox<String>(tracker_horiz_values());
		tracker_horiz_value.setEditable(true);
		tracker_horiz_value.addItemListener(this);
		pane.add(tracker_horiz_value, c);
		row++;

		/* Tracker triger vert distances */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		c.ipady = 5;
		tracker_vert_label = new JLabel(get_tracker_vert_label());
		pane.add(tracker_vert_label, c);

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 4;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = ir;
		c.ipady = 5;
		tracker_vert_value = new JComboBox<String>(tracker_vert_values());
		tracker_vert_value.setEditable(true);
		tracker_vert_value.addItemListener(this);
		pane.add(tracker_vert_value, c);
		set_tracker_tool_tip();
		row++;

		/* Buttons */
		c = new GridBagConstraints();
		c.gridx = 0; c.gridy = row;
		c.gridwidth = 2;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_START;
		c.insets = il;
		save = new JButton("Save");
		pane.add(save, c);
		save.addActionListener(this);
		save.setActionCommand("Save");

		c = new GridBagConstraints();
		c.gridx = 2; c.gridy = row;
		c.gridwidth = 2;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.CENTER;
		c.insets = il;
		reset = new JButton("Reset");
		pane.add(reset, c);
		reset.addActionListener(this);
		reset.setActionCommand("Reset");

		c = new GridBagConstraints();
		c.gridx = 4; c.gridy = row;
		c.gridwidth = 2;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.CENTER;
		c.insets = il;
		reboot = new JButton("Reboot");
		pane.add(reboot, c);
		reboot.addActionListener(this);
		reboot.setActionCommand("Reboot");

		c = new GridBagConstraints();
		c.gridx = 6; c.gridy = row;
		c.gridwidth = 2;
		c.fill = GridBagConstraints.NONE;
		c.anchor = GridBagConstraints.LINE_END;
		c.insets = il;
		close = new JButton("Close");
		pane.add(close, c);
		close.addActionListener(this);
		close.setActionCommand("Close");

		addWindowListener(new ConfigListener(this));
		AltosPreferences.register_units_listener(this);
	}

	/* Once the initial values are set, the config code will show the dialog */
	public void make_visible() {
		pack();
		setLocationRelativeTo(owner);
		setVisible(true);
	}

	/* If any values have been changed, confirm before closing */
	public boolean check_dirty(String operation) {
		if (dirty) {
			Object[] options = { String.format("%s anyway", operation), "Keep editing" };
			int i;
			i = JOptionPane.showOptionDialog(this,
							 String.format("Configuration modified. %s anyway?", operation),
							 "Configuration Modified",
							 JOptionPane.DEFAULT_OPTION,
							 JOptionPane.WARNING_MESSAGE,
							 null, options, options[1]);
			if (i != 0)
				return false;
		}
		return true;
	}

	void set_dirty() {
		dirty = true;
		save.setEnabled(true);
	}

	public void set_clean() {
		dirty = false;
		save.setEnabled(false);
	}

	public void dispose() {
		AltosPreferences.unregister_units_listener(this);
		super.dispose();
	}

	/* Listen for events from our buttons */
	public void actionPerformed(ActionEvent e) {
		String	cmd = e.getActionCommand();

		if (cmd.equals("Close") || cmd.equals("Reboot"))
			if (!check_dirty(cmd))
				return;
		listener.actionPerformed(e);
		if (cmd.equals("Close") || cmd.equals("Reboot")) {
			setVisible(false);
			dispose();
		}
		set_clean();
	}

	/* ItemListener interface method */
	public void itemStateChanged(ItemEvent e) {
		set_dirty();
	}

	/* DocumentListener interface methods */
	public void changedUpdate(DocumentEvent e) {
		set_dirty();
	}

	public void insertUpdate(DocumentEvent e) {
		set_dirty();
	}

	public void removeUpdate(DocumentEvent e) {
		set_dirty();
	}

	/* Let the config code hook on a listener */
	public void addActionListener(ActionListener l) {
		listener = l;
	}

	public void units_changed(boolean imperial_units) {
		if (tracker_horiz_value.isEnabled() && tracker_vert_value.isEnabled()) {
			String th = tracker_horiz_value.getSelectedItem().toString();
			String tv = tracker_vert_value.getSelectedItem().toString();
			tracker_horiz_label.setText(get_tracker_horiz_label());
			tracker_vert_label.setText(get_tracker_vert_label());
			set_tracker_horiz_values();
			set_tracker_vert_values();
			int[] t = {
				(int) (AltosConvert.height.parse(th, !imperial_units) + 0.5),
				(int) (AltosConvert.height.parse(tv, !imperial_units) + 0.5)
			};
			set_tracker_distances(t);
		}
	}

	/* set and get all of the dialog values */
	public void set_product(String product) {
		radio_frequency_value.set_product(product);
		product_value.setText(product);
		set_flight_log_max_tool_tip();
	}

	public void set_version(String version) {
		version_value.setText(version);
	}

	public void set_serial(int serial) {
		radio_frequency_value.set_serial(serial);
		serial_value.setText(String.format("%d", serial));
	}

	public void set_main_deploy(int new_main_deploy) {
	}

	public int main_deploy() {
		return -1;
	}

	public void set_apogee_delay(int new_apogee_delay) { }

	public int apogee_delay() {
		return -1;
	}

	public void set_apogee_lockout(int new_apogee_lockout) { }

	public int apogee_lockout() { return -1; }

	public void set_radio_frequency(double new_radio_frequency) {
		radio_frequency_value.set_frequency(new_radio_frequency);
	}

	public double radio_frequency() {
		return radio_frequency_value.frequency();
	}

	public void set_radio_calibration(int new_radio_calibration) {
		radio_calibration_value.setVisible(new_radio_calibration >= 0);
		if (new_radio_calibration < 0)
			radio_calibration_value.setText("Disabled");
		else
			radio_calibration_value.setText(String.format("%d", new_radio_calibration));
	}

	public int radio_calibration() {
		return Integer.parseInt(radio_calibration_value.getText());
	}

	public void set_radio_enable(int new_radio_enable) {
		if (new_radio_enable >= 0) {
			radio_enable_value.setSelected(new_radio_enable > 0);
			radio_enable_value.setEnabled(true);
		} else {
			radio_enable_value.setSelected(true);
			radio_enable_value.setVisible(radio_frequency() > 0);
			radio_enable_value.setEnabled(false);
		}
		set_radio_enable_tool_tip();
	}

	public int radio_enable() {
		if (radio_enable_value.isEnabled())
			return radio_enable_value.isSelected() ? 1 : 0;
		else
			return -1;
	}

	public void set_callsign(String new_callsign) {
		callsign_value.setVisible(new_callsign != null);
		callsign_value.setText(new_callsign);
	}

	public String callsign() {
		return callsign_value.getText();
	}

	public void set_flight_log_max(int new_flight_log_max) {
		flight_log_max_value.setSelectedItem(Integer.toString(new_flight_log_max));
		set_flight_log_max_tool_tip();
	}

	public void set_flight_log_max_enabled(boolean enable) {
		flight_log_max_value.setEnabled(enable);
		set_flight_log_max_tool_tip();
	}

	public int flight_log_max() {
		return Integer.parseInt(flight_log_max_value.getSelectedItem().toString());
	}

	public void set_flight_log_max_limit(int flight_log_max_limit) {
		//boolean	any_added = false;
		flight_log_max_value.removeAllItems();
		for (int i = 0; i < flight_log_max_values.length; i++) {
			if (Integer.parseInt(flight_log_max_values[i]) < flight_log_max_limit){
				flight_log_max_value.addItem(flight_log_max_values[i]);
				//any_added = true;
			}
		}
		flight_log_max_value.addItem(String.format("%d", flight_log_max_limit));
	}

	public void set_ignite_mode(int new_ignite_mode) { }
	public int ignite_mode() { return -1; }


	public void set_pad_orientation(int new_pad_orientation) { }
	public int pad_orientation() { return -1; }

	public void set_beep(int new_beep) { }

	public int beep() { return -1; }

	String[] tracker_horiz_values() {
		if (AltosConvert.imperial_units)
			return tracker_horiz_values_ft;
		else
			return tracker_horiz_values_m;
	}

	void set_tracker_horiz_values() {
		String[]	v = tracker_horiz_values();
		while (tracker_horiz_value.getItemCount() > 0)
			tracker_horiz_value.removeItemAt(0);
		for (int i = 0; i < v.length; i++)
			tracker_horiz_value.addItem(v[i]);
		tracker_horiz_value.setMaximumRowCount(v.length);
	}

	String get_tracker_horiz_label() {
		return String.format("Logging Trigger Horizontal (%s):", AltosConvert.height.show_units());
	}

	String[] tracker_vert_values() {
		if (AltosConvert.imperial_units)
			return tracker_vert_values_ft;
		else
			return tracker_vert_values_m;
	}

	void set_tracker_vert_values() {
		String[]	v = tracker_vert_values();
		while (tracker_vert_value.getItemCount() > 0)
			tracker_vert_value.removeItemAt(0);
		for (int i = 0; i < v.length; i++)
			tracker_vert_value.addItem(v[i]);
		tracker_vert_value.setMaximumRowCount(v.length);
	}

	void set_tracker_tool_tip() {
		if (tracker_horiz_value.isEnabled())
			tracker_horiz_value.setToolTipText("How far the device must move before logging is enabled");
		else
			tracker_horiz_value.setToolTipText("This device doesn't disable logging before motion");
		if (tracker_vert_value.isEnabled())
			tracker_vert_value.setToolTipText("How far the device must move before logging is enabled");
		else
			tracker_vert_value.setToolTipText("This device doesn't disable logging before motion");
	}

	String get_tracker_vert_label() {
		return String.format("Logging Trigger Vertical (%s):", AltosConvert.height.show_units());
	}

	public void set_tracker_distances(int[] tracker_distances) {
		if (tracker_distances != null) {
			tracker_horiz_value.setSelectedItem(AltosConvert.height.say(tracker_distances[0]));
			tracker_vert_value.setSelectedItem(AltosConvert.height.say(tracker_distances[1]));
			tracker_horiz_value.setEnabled(true);
			tracker_vert_value.setEnabled(true);
		} else {
			tracker_horiz_value.setEnabled(false);
			tracker_vert_value.setEnabled(false);
		}
	}

	public int[] tracker_distances() {
		if (tracker_horiz_value.isEnabled() && tracker_vert_value.isEnabled()) {
			int[] t = {
				(int) (AltosConvert.height.parse(tracker_horiz_value.getSelectedItem().toString()) + 0.5),
				(int) (AltosConvert.height.parse(tracker_vert_value.getSelectedItem().toString()) + 0.5),
			};
			return t;
		}
		return null;
	}

	public void set_aprs_interval(int new_aprs_interval) {
		String	s;

		if (new_aprs_interval <= 0)
			s = "Disabled";
		else
			s = Integer.toString(new_aprs_interval);
		aprs_interval_value.setSelectedItem(s);
		aprs_interval_value.setVisible(new_aprs_interval >= 0);
		set_aprs_interval_tool_tip();
	}

	public int aprs_interval() {
		String	s = aprs_interval_value.getSelectedItem().toString();

		if (s.equals("Disabled"))
			return 0;
		return Integer.parseInt(s);
	}
}
