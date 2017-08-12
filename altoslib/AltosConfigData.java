/*
 * Copyright © 2011 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.altoslib_12;

import java.util.*;
import java.text.*;
import java.util.concurrent.*;

public class AltosConfigData {

	/* Version information */
	public String	manufacturer;
	public String	product;
	public int	serial;
	public int	flight;
	public int	log_format;
	public int	log_space;
	public String	version;
	public int	altitude_32;
	public int	config_major, config_minor;

	/* Config information */
	/* HAS_FLIGHT*/
	public int	main_deploy;
	public int	apogee_delay;
	public int	apogee_lockout;

	/* HAS_RADIO */
	public int	radio_frequency;
	public String	callsign;
	public int	radio_enable;
	public int	radio_calibration;
	public int	telemetry_rate;
	/* Old HAS_RADIO values */
	public int	radio_channel;
	public int	radio_setting;

	/* HAS_ACCEL */
	public int	accel_cal_plus, accel_cal_minus;
	public int	pad_orientation;

	/* HAS_LOG */
	public int	flight_log_max;
	public int 	log_fixed;

	/* HAS_IGNITE */
	public int	ignite_mode;

	/* HAS_AES */
	public String	aes_key;

	/* AO_PYRO_NUM */
	public AltosPyro[]	pyros;
	public int		npyro;
	public int		pyro;
	public double		pyro_firing_time;

	/* HAS_APRS */
	public int		aprs_interval;
	public int		aprs_ssid;
	public int		aprs_format;

	/* HAS_BEEP */
	public int		beep;

	/* Storage info replies */
	public int	storage_size;
	public int	storage_erase_unit;

	/* Log listing replies */
	public int	stored_flight;

	/* HAS_TRACKER */
	public int	tracker_motion;
	public int	tracker_interval;

	/* HAS_GYRO */
	public int	accel_zero_along, accel_zero_across, accel_zero_through;

	/* ms5607 data */
	AltosMs5607	ms5607;

	public AltosMs5607 ms5607() {
		if (ms5607 == null)
			ms5607 = new AltosMs5607();
		return ms5607;
	}

	public static String get_string(String line, String label) throws  ParseException {
		if (line.startsWith(label)) {
			String	quoted = line.substring(label.length()).trim();

			if (quoted.startsWith("\""))
				quoted = quoted.substring(1);
			if (quoted.endsWith("\""))
				quoted = quoted.substring(0,quoted.length()-1);
			return quoted;
		}
		throw new ParseException("mismatch", 0);
	}

	public static int get_int(String line, String label) throws NumberFormatException, ParseException {
		if (line.startsWith(label)) {
			String tail = line.substring(label.length()).trim();
			String[] tokens = tail.split("\\s+");
			if (tokens.length > 0)
				return  Integer.parseInt(tokens[0]);
		}
		throw new ParseException("mismatch", 0);
	}

	public static int[] get_values(String line, String label) throws NumberFormatException, ParseException {
		if (line.startsWith(label)) {
			String tail = line.substring(label.length()).trim();
			String[] tokens = tail.split("\\s+");
			if (tokens.length > 1) {
				int[]	values = new int[2];
				values[0] = Integer.parseInt(tokens[0]);
				values[1] = Integer.parseInt(tokens[1]);
				return values;
			}
		}
		throw new ParseException("mismatch", 0);
	}

	public int log_space() {
		if (log_space != AltosLib.MISSING)
			return log_space;

		if (storage_size != AltosLib.MISSING) {
			int	space = storage_size;

			if (storage_erase_unit != AltosLib.MISSING && use_flash_for_config())
				space -= storage_erase_unit;

			if (space != AltosLib.MISSING)
				return space;
		}
		return 0;
	}

	public int log_available() {
		switch (log_format) {
		case AltosLib.AO_LOG_FORMAT_TINY:
			if (stored_flight == 0)
				return 1;
			return 0;
		case AltosLib.AO_LOG_FORMAT_TELEMETRY:
		case AltosLib.AO_LOG_FORMAT_TELESCIENCE:
			return 1;
		default:
			if (flight_log_max <= 0)
				return 1;
			int	log_max = flight_log_max * 1024;
			int	log_space = log_space();
			int	log_used;

			if (stored_flight <= 0)
				log_used = 0;
			else
				log_used = stored_flight * log_max;
			int	log_avail;

			if (log_used >= log_space)
				log_avail = 0;
			else
				log_avail = (log_space - log_used) / log_max;

			return log_avail;
		}
	}

	public boolean has_monitor_battery() {
		if (product.startsWith("TeleBT"))
			return true;
		return false;
	}

	int[] parse_version(String v) {
		String[] parts = v.split("\\.");
		int r[] = new int[parts.length];

		for (int i = 0; i < parts.length; i++) {
			try {
				r[i] = (int) AltosLib.fromdec(parts[i]);
			} catch (NumberFormatException n) {
				r[i] = 0;
			}
		}

		return r;
	}

	public boolean altitude_32() {
		return altitude_32 == 1;
	}

	public int compare_version(String other) {
		int[]	me = parse_version(version);
		int[]	them = parse_version(other);

		int	l = Math.min(me.length, them.length);

		for (int i = 0; i < l; i++) {
			int	d = me[i] - them[i];
			if (d != 0)
				return d;
		}
		if (me.length > l)
			return 1;
		if (them.length > l)
			return -1;
		return 0;
	}

	public void reset() {
		manufacturer = null;
		product = null;
		serial = AltosLib.MISSING;
		flight = AltosLib.MISSING;
		log_format = AltosLib.AO_LOG_FORMAT_UNKNOWN;
		log_space = AltosLib.MISSING;
		version = "unknown";
		config_major = AltosLib.MISSING;
		config_minor = AltosLib.MISSING;

		main_deploy = AltosLib.MISSING;
		apogee_delay = AltosLib.MISSING;
		apogee_lockout = AltosLib.MISSING;

		radio_frequency = AltosLib.MISSING;
		callsign = null;
		radio_enable = AltosLib.MISSING;
		radio_calibration = AltosLib.MISSING;
		radio_channel = AltosLib.MISSING;
		radio_setting = AltosLib.MISSING;
		telemetry_rate = AltosLib.MISSING;

		accel_cal_plus = AltosLib.MISSING;
		accel_cal_minus = AltosLib.MISSING;
		pad_orientation = AltosLib.MISSING;

		flight_log_max = AltosLib.MISSING;
		log_fixed = AltosLib.MISSING;
		ignite_mode = AltosLib.MISSING;

		aes_key = null;

		pyro = AltosLib.MISSING;
		npyro = AltosLib.MISSING;
		pyros = null;
		pyro_firing_time = AltosLib.MISSING;

		aprs_interval = AltosLib.MISSING;
		aprs_ssid = AltosLib.MISSING;
		aprs_format = AltosLib.MISSING;

		beep = AltosLib.MISSING;

		tracker_motion = AltosLib.MISSING;
		tracker_interval = AltosLib.MISSING;

		storage_size = AltosLib.MISSING;
		storage_erase_unit = AltosLib.MISSING;
		stored_flight = AltosLib.MISSING;

		accel_zero_along = AltosLib.MISSING;
		accel_zero_across = AltosLib.MISSING;
		accel_zero_through = AltosLib.MISSING;
	}

	public void parse_line(String line) {

		/* Version replies */
		try { manufacturer = get_string(line, "manufacturer"); } catch (Exception e) {}
		try { product = get_string(line, "product"); } catch (Exception e) {}
		try { serial = get_int(line, "serial-number"); } catch (Exception e) {}
		try { flight = get_int(line, "current-flight"); } catch (Exception e) {}
		try { log_format = get_int(line, "log-format"); } catch (Exception e) {}
		try { log_space = get_int(line, "log-space"); } catch (Exception e) {}
		try { altitude_32 = get_int(line, "altitude-32"); } catch (Exception e) {}
		try { version = get_string(line, "software-version"); } catch (Exception e) {}

		/* Version also contains MS5607 info, which we ignore here */

		try { ms5607().reserved = get_int(line, "ms5607 reserved:"); } catch (Exception e) {}
		try { ms5607().sens = get_int(line, "ms5607 sens:"); } catch (Exception e) {}
		try { ms5607().off = get_int(line, "ms5607 off:"); } catch (Exception e) {}
		try { ms5607().tcs = get_int(line, "ms5607 tcs:"); } catch (Exception e) {}
		try { ms5607().tco = get_int(line, "ms5607 tco:"); } catch (Exception e) {}
		try { ms5607().tref = get_int(line, "ms5607 tref:"); } catch (Exception e) {}
		try { ms5607().tempsens = get_int(line, "ms5607 tempsens:"); } catch (Exception e) {}
		try { ms5607().crc = get_int(line, "ms5607 crc:"); } catch (Exception e) {}

		/* Config show replies */

		try {
			if (line.startsWith("Config version")) {
				String[] bits = line.split("\\s+");
				if (bits.length >= 3) {
					String[] cfg = bits[2].split("\\.");

					if (cfg.length >= 2) {
						config_major = Integer.parseInt(cfg[0]);
						config_minor = Integer.parseInt(cfg[1]);
					}
				}
			}
		} catch (Exception e) {}

		/* HAS_FLIGHT */
		try { main_deploy = get_int(line, "Main deploy:"); } catch (Exception e) {}
		try { apogee_delay = get_int(line, "Apogee delay:"); } catch (Exception e) {}
		try { apogee_lockout = get_int(line, "Apogee lockout:"); } catch (Exception e) {}

		/* HAS_RADIO */
		try {
			radio_frequency = get_int(line, "Frequency:");
			if (radio_frequency < 0)
				radio_frequency = 434550;
		} catch (Exception e) {}
		try { callsign = get_string(line, "Callsign:"); } catch (Exception e) {}
		try { radio_enable = get_int(line, "Radio enable:"); } catch (Exception e) {}
		try { radio_calibration = get_int(line, "Radio cal:"); } catch (Exception e) {}
		try { telemetry_rate = get_int(line, "Telemetry rate:"); } catch (Exception e) {}

		/* Old HAS_RADIO values */
		try { radio_channel = get_int(line, "Radio channel:"); } catch (Exception e) {}
		try { radio_setting = get_int(line, "Radio setting:"); } catch (Exception e) {}

		/* HAS_ACCEL */
		try {
			if (line.startsWith("Accel cal")) {
				String[] bits = line.split("\\s+");
				if (bits.length >= 6) {
					accel_cal_plus = Integer.parseInt(bits[3]);
					accel_cal_minus = Integer.parseInt(bits[5]);
				}
			}
		} catch (Exception e) {}
		try { pad_orientation = get_int(line, "Pad orientation:"); } catch (Exception e) {}

		/* HAS_LOG */
		try { flight_log_max = get_int(line, "Max flight log:"); } catch (Exception e) {}
		try { log_fixed = get_int(line, "Log fixed:"); } catch (Exception e) {}

		/* HAS_IGNITE */
		try { ignite_mode = get_int(line, "Ignite mode:"); } catch (Exception e) {}

		/* HAS_AES */
		try { aes_key = get_string(line, "AES key:"); } catch (Exception e) {}

		/* AO_PYRO_NUM */
		try {
			npyro = get_int(line, "Pyro-count:");
			pyros = new AltosPyro[npyro];
			pyro = 0;
		} catch (Exception e) {}
		if (npyro != AltosLib.MISSING) {
			try {
				AltosPyro p = new AltosPyro(pyro, line);
				if (pyro < npyro)
					pyros[pyro++] = p;
			} catch (Exception e) {}
		}
		try { pyro_firing_time = get_int(line, "Pyro time:") / 100.0; } catch (Exception e) {}

		/* HAS_APRS */
		try { aprs_interval = get_int(line, "APRS interval:"); } catch (Exception e) {}
		try { aprs_ssid = get_int(line, "APRS SSID:"); } catch (Exception e) {}
		try { aprs_format = get_int(line, "APRS format:"); } catch (Exception e) {}

		/* HAS_BEEP */
		try { beep = get_int(line, "Beeper setting:"); } catch (Exception e) {}

		/* HAS_TRACKER */
		try {
			int[] values = get_values(line, "Tracker setting:");
			tracker_motion = values[0];
			tracker_interval = values[1];
		} catch (Exception e) {}

		/* Storage info replies */
		try { storage_size = get_int(line, "Storage size:"); } catch (Exception e) {}
		try { storage_erase_unit = get_int(line, "Storage erase unit:"); } catch (Exception e) {}

		/* Log listing replies */
		try { get_int(line, "flight"); stored_flight++; }  catch (Exception e) {}

		/* HAS_GYRO */
		try {
			if (line.startsWith("IMU call along")) {
				String[] bits = line.split("\\s+");
				if (bits.length >= 8) {
					accel_zero_along = Integer.parseInt(bits[3]);
					accel_zero_across = Integer.parseInt(bits[5]);
					accel_zero_through = Integer.parseInt(bits[7]);
				}
			}
		} catch (Exception e) {}
	}

	public AltosConfigData() {
		reset();
	}

	private void read_link(AltosLink link, String finished) throws InterruptedException, TimeoutException {
		for (;;) {
			String line = link.get_reply();
			if (line == null)
				throw new TimeoutException();
			if (line.contains("Syntax error"))
				continue;
			this.parse_line(line);

			/* signals the end of the version info */
			if (line.startsWith(finished))
				break;
		}
	}

	public boolean has_frequency() {
		return radio_frequency != AltosLib.MISSING || radio_setting != AltosLib.MISSING || radio_channel != AltosLib.MISSING;
	}

	public boolean has_telemetry_rate() {
		return telemetry_rate != AltosLib.MISSING;
	}

	public void set_frequency(double freq) {
		int	frequency = radio_frequency;
		int	setting = radio_setting;

		if (frequency != AltosLib.MISSING) {
			radio_frequency = (int) Math.floor (freq * 1000 + 0.5);
			radio_channel = AltosLib.MISSING;
		} else if (setting != AltosLib.MISSING) {
			radio_setting =AltosConvert.radio_frequency_to_setting(freq, radio_calibration);
			radio_channel = AltosLib.MISSING;
		} else {
			radio_channel = AltosConvert.radio_frequency_to_channel(freq);
		}
	}

	public double frequency() {
		int	channel = radio_channel;
		int	setting = radio_setting;

		if (radio_frequency == AltosLib.MISSING && channel == AltosLib.MISSING && setting == AltosLib.MISSING)
			return AltosLib.MISSING;

		if (channel == AltosLib.MISSING)
			channel = 0;
		if (setting == AltosLib.MISSING)
			setting = 0;

		return AltosConvert.radio_to_frequency(radio_frequency,
						       setting,
						       radio_calibration,
						       channel);
	}

	boolean use_flash_for_config() {
		if (product.startsWith("TeleMega"))
			return false;
		if (product.startsWith("TeleMetrum-v2"))
			return false;
		if (product.startsWith("EasyMega"))
			return false;
		return true;
	}


	public boolean mma655x_inverted() throws AltosUnknownProduct {
		if (product.startsWith("EasyMega-v1"))
			return false;
		if (product.startsWith("TeleMetrum-v2"))
			return true;
		if (product.startsWith("TeleMega-v2"))
			return false;
		if (product.startsWith("TeleMega-v1"))
			return false;
		throw new AltosUnknownProduct(product);
	}

	public void get_values(AltosConfigValues source) throws AltosConfigDataException {

		/* HAS_FLIGHT */
		if (main_deploy != AltosLib.MISSING)
			main_deploy = source.main_deploy();
		if (apogee_delay != AltosLib.MISSING)
			apogee_delay = source.apogee_delay();
		if (apogee_lockout != AltosLib.MISSING)
			apogee_lockout = source.apogee_lockout();

		/* HAS_RADIO */
		if (has_frequency())
			set_frequency(source.radio_frequency());
		if (radio_enable != AltosLib.MISSING)
			radio_enable = source.radio_enable();
		if (callsign != null)
			callsign = source.callsign();
		if (telemetry_rate != AltosLib.MISSING)
			telemetry_rate = source.telemetry_rate();

		/* HAS_ACCEL */
		if (pad_orientation != AltosLib.MISSING)
			pad_orientation = source.pad_orientation();

		if (accel_cal_plus != AltosLib.MISSING)
			accel_cal_plus = source.accel_cal_plus();

		if (accel_cal_minus != AltosLib.MISSING)
			accel_cal_minus = source.accel_cal_minus();

		/* HAS_LOG */
		if (flight_log_max != AltosLib.MISSING)
			flight_log_max = source.flight_log_max();

		/* HAS_IGNITE */
		if (ignite_mode != AltosLib.MISSING)
			ignite_mode = source.ignite_mode();

		/* AO_PYRO_NUM */
		if (npyro != AltosLib.MISSING)
			pyros = source.pyros();
		if (pyro_firing_time != AltosLib.MISSING)
			pyro_firing_time = source.pyro_firing_time();

		/* HAS_APRS */
		if (aprs_interval != AltosLib.MISSING)
			aprs_interval = source.aprs_interval();
		if (aprs_ssid != AltosLib.MISSING)
			aprs_ssid = source.aprs_ssid();
		if (aprs_format != AltosLib.MISSING)
			aprs_format = source.aprs_format();

		/* HAS_BEEP */
		if (beep != AltosLib.MISSING)
			beep = source.beep();
		/* HAS_TRACKER */
		if (tracker_motion != AltosLib.MISSING)
			tracker_motion = source.tracker_motion();
		if (tracker_interval != AltosLib.MISSING)
			tracker_interval = source.tracker_interval();
	}

	public void set_values(AltosConfigValues dest) {
		dest.set_serial(serial);
		dest.set_product(product);
		dest.set_version(version);
		dest.set_altitude_32(altitude_32);
		dest.set_main_deploy(main_deploy);
		dest.set_apogee_delay(apogee_delay);
		dest.set_apogee_lockout(apogee_lockout);
		dest.set_radio_calibration(radio_calibration);
		dest.set_radio_frequency(frequency());
		dest.set_telemetry_rate(telemetry_rate);
		boolean max_enabled = true;

		if (log_space() == 0)
			max_enabled = false;

		if (log_fixed != AltosLib.MISSING)
			max_enabled = false;

		switch (log_format) {
		case AltosLib.AO_LOG_FORMAT_TINY:
			max_enabled = false;
			break;
		default:
			if (stored_flight != AltosLib.MISSING)
				max_enabled = false;
			break;
		}

		dest.set_flight_log_max_enabled(max_enabled);
		dest.set_radio_enable(radio_enable);
		dest.set_flight_log_max_limit(log_space() / 1024);
		dest.set_flight_log_max(flight_log_max);
		dest.set_ignite_mode(ignite_mode);
		dest.set_pad_orientation(pad_orientation);
		dest.set_accel_cal(accel_cal_plus, accel_cal_minus);
		dest.set_callsign(callsign);
		if (npyro != AltosLib.MISSING)
			dest.set_pyros(pyros);
		else
			dest.set_pyros(null);
		dest.set_pyro_firing_time(pyro_firing_time);
		dest.set_aprs_interval(aprs_interval);
		dest.set_aprs_ssid(aprs_ssid);
		dest.set_aprs_format(aprs_format);
		dest.set_beep(beep);
		dest.set_tracker_motion(tracker_motion);
		dest.set_tracker_interval(tracker_interval);
	}

	public boolean log_has_state() {
		switch (log_format) {
		case AltosLib.AO_LOG_FORMAT_TELEGPS:
			return false;
		}
		return true;
	}

	public void save(AltosLink link, boolean remote) throws InterruptedException, TimeoutException {

		/* HAS_FLIGHT */
		if (main_deploy != AltosLib.MISSING)
			link.printf("c m %d\n", main_deploy);
		if (apogee_delay != AltosLib.MISSING)
			link.printf("c d %d\n", apogee_delay);
		if (apogee_lockout != AltosLib.MISSING)
			link.printf("c L %d\n", apogee_lockout);

		/* HAS_RADIO */
		if (has_frequency()) {
			boolean has_frequency = radio_frequency != AltosLib.MISSING;
			boolean has_setting = radio_setting != AltosLib.MISSING;
			double frequency = frequency();
			link.set_radio_frequency(frequency,
							has_frequency,
							has_setting,
							radio_calibration);
			/* When remote, reset the dongle frequency at the same time */
			if (remote) {
				link.flush_output();
				link.stop_remote();
				link.set_radio_frequency(frequency);
				link.flush_output();
				link.start_remote();
			}
		}

		if (telemetry_rate != AltosLib.MISSING) {
			link.printf("c T %d\n", telemetry_rate);
			if (remote) {
				link.flush_output();
				link.stop_remote();
				link.set_telemetry_rate(telemetry_rate);
				link.flush_output();
				link.start_remote();
			}
		}

		if (callsign != null) {
			link.printf("c c %s\n", callsign);
			if (remote) {
				link.flush_output();
				link.stop_remote();
				link.set_callsign(callsign);
				link.flush_output();
				link.start_remote();
			}
		}

		if (radio_enable != AltosLib.MISSING)
			link.printf("c e %d\n", radio_enable);

		/* HAS_ACCEL */
		if (pad_orientation != AltosLib.MISSING)
			link.printf("c o %d\n", pad_orientation);
		if (accel_cal_plus != AltosLib.MISSING && accel_cal_minus != AltosLib.MISSING)
			link.printf("c a %d %d\n", accel_cal_plus, accel_cal_minus);

		/* HAS_LOG */
		if (flight_log_max != 0)
			link.printf("c l %d\n", flight_log_max);

		/* HAS_IGNITE */
		if (ignite_mode != AltosLib.MISSING)
			link.printf("c i %d\n", ignite_mode);

		/* HAS_AES */
		/* UI doesn't support AES key config */

		/* AO_PYRO_NUM */
		if (npyro != AltosLib.MISSING) {
			for (int p = 0; p < pyros.length; p++) {
				link.printf("c P %s\n",
						   pyros[p].toString());
			}
		}
		if (pyro_firing_time != AltosLib.MISSING)
			link.printf("c I %d\n", (int) (pyro_firing_time * 100.0 + 0.5));

		/* HAS_APRS */
		if (aprs_interval != AltosLib.MISSING)
			link.printf("c A %d\n", aprs_interval);
		if (aprs_ssid != AltosLib.MISSING)
			link.printf("c S %d\n", aprs_ssid);
		if (aprs_format != AltosLib.MISSING)
			link.printf("c C %d\n", aprs_format);

		/* HAS_BEEP */
		if (beep != AltosLib.MISSING)
			link.printf("c b %d\n", beep);

		/* HAS_TRACKER */
		if (tracker_motion != AltosLib.MISSING && tracker_interval != AltosLib.MISSING)
			link.printf("c t %d %d\n", tracker_motion, tracker_interval);

		/* HAS_GYRO */
		/* UI doesn't support accel cal */

		link.printf("c w\n");
		link.flush_output();
	}

	public AltosConfigData(AltosLink link) throws InterruptedException, TimeoutException {
		reset();
		link.printf("c s\nf\nv\n");
		read_link(link, "software-version");
		switch (log_format) {
		case AltosLib.AO_LOG_FORMAT_UNKNOWN:
		case AltosLib.AO_LOG_FORMAT_NONE:
			break;
		default:
			link.printf("l\n");
			read_link(link, "done");
			break;
		}
	}
}
