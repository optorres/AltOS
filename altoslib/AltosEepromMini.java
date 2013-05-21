/*
 * Copyright © 2011 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.altoslib_1;

import java.text.*;

public class AltosEepromMini {
	public int	cmd;
	public int	tick;
	public boolean	valid;
	public String	data;
	public int	config_a, config_b;

	public int	data8[];

	public static final int	record_length = 16;
	static final int	header_length = 4;
	static final int	data_length = record_length - header_length;

	public int data8(int i) {
		return data8[i];
	}

	public int data16(int i) {
		return ((data8[i] | (data8[i+1] << 8)) << 16) >> 16;
	}

	public int data24(int i) {
		return data8[i] | (data8[i+1] << 8) | (data8[i+2] << 16);
	}

	public int data32(int i) {
		return data8[i] | (data8[i+1] << 8) | (data8[i+2] << 16) | (data8[i+3] << 24);
	}

	/* AO_LOG_FLIGHT elements */
	public int flight() { return data16(0); }
	public int ground_pres() { return data32(4); }

	/* AO_LOG_STATE elements */
	public int state() { return data16(0); }
	public int reason() { return data16(2); }

	/* AO_LOG_SENSOR elements */
	public int pres() { return data24(0); }
	public int temp() { return data24(3); }
	public int sense_a() { return data16(6); }
	public int sense_m() { return data16(8); }
	public int v_batt() { return data16(10); }

	public AltosEepromMini (AltosEepromChunk chunk, int start) throws ParseException {
		cmd = chunk.data(start);

		valid = !chunk.erased(start, record_length);
		if (valid) {
			if (AltosConvert.checksum(chunk.data, start, record_length) != 0)
				throw new ParseException(String.format("invalid checksum at 0x%x",
								       chunk.address + start), 0);
		} else {
			cmd = AltosLib.AO_LOG_INVALID;
		}

		tick = chunk.data16(start+2);

		data8 = new int[data_length];
		for (int i = 0; i < data_length; i++)
			data8[i] = chunk.data(start + header_length + i);
	}

	public AltosEepromMini (String line) {
		valid = false;
		tick = 0;

		if (line == null) {
			cmd = AltosLib.AO_LOG_INVALID;
			line = "";
		} else {
			try {
				String[] tokens = line.split("\\s+");

				if (tokens[0].length() == 1) {
					if (tokens.length != 2 + data_length) {
						cmd = AltosLib.AO_LOG_INVALID;
						data = line;
					} else {
						cmd = tokens[0].codePointAt(0);
						tick = Integer.parseInt(tokens[1],16);
						valid = true;
						data8 = new int[data_length];
						for (int i = 0; i < data_length; i++)
							data8[i] = Integer.parseInt(tokens[2 + i],16);
					}
				} else if (tokens[0].equals("Config") && tokens[1].equals("version:")) {
					cmd = AltosLib.AO_LOG_CONFIG_VERSION;
					data = tokens[2];
				} else if (tokens[0].equals("Main") && tokens[1].equals("deploy:")) {
					cmd = AltosLib.AO_LOG_MAIN_DEPLOY;
					config_a = Integer.parseInt(tokens[2]);
				} else if (tokens[0].equals("Apogee") && tokens[1].equals("delay:")) {
					cmd = AltosLib.AO_LOG_APOGEE_DELAY;
					config_a = Integer.parseInt(tokens[2]);
				} else if (tokens[0].equals("Radio") && tokens[1].equals("channel:")) {
					cmd = AltosLib.AO_LOG_RADIO_CHANNEL;
					config_a = Integer.parseInt(tokens[2]);
				} else if (tokens[0].equals("Callsign:")) {
					cmd = AltosLib.AO_LOG_CALLSIGN;
					data = tokens[1].replaceAll("\"","");
				} else if (tokens[0].equals("Accel") && tokens[1].equals("cal")) {
					cmd = AltosLib.AO_LOG_ACCEL_CAL;
					config_a = Integer.parseInt(tokens[3]);
					config_b = Integer.parseInt(tokens[5]);
				} else if (tokens[0].equals("Radio") && tokens[1].equals("cal:")) {
					cmd = AltosLib.AO_LOG_RADIO_CAL;
					config_a = Integer.parseInt(tokens[2]);
				} else if (tokens[0].equals("Max") && tokens[1].equals("flight") && tokens[2].equals("log:")) {
					cmd = AltosLib.AO_LOG_MAX_FLIGHT_LOG;
					config_a = Integer.parseInt(tokens[3]);
				} else if (tokens[0].equals("manufacturer")) {
					cmd = AltosLib.AO_LOG_MANUFACTURER;
					data = tokens[1];
				} else if (tokens[0].equals("product")) {
					cmd = AltosLib.AO_LOG_PRODUCT;
					data = tokens[1];
				} else if (tokens[0].equals("serial-number")) {
					cmd = AltosLib.AO_LOG_SERIAL_NUMBER;
					config_a = Integer.parseInt(tokens[1]);
				} else if (tokens[0].equals("log-format")) {
					cmd = AltosLib.AO_LOG_LOG_FORMAT;
					config_a = Integer.parseInt(tokens[1]);
				} else if (tokens[0].equals("software-version")) {
					cmd = AltosLib.AO_LOG_SOFTWARE_VERSION;
					data = tokens[1];
				} else if (tokens[0].equals("ms5607")) {
					if (tokens[1].equals("reserved:")) {
						cmd = AltosLib.AO_LOG_BARO_RESERVED;
						config_a = Integer.parseInt(tokens[2]);
					} else if (tokens[1].equals("sens:")) {
						cmd = AltosLib.AO_LOG_BARO_SENS;
						config_a = Integer.parseInt(tokens[2]);
					} else if (tokens[1].equals("off:")) {
						cmd = AltosLib.AO_LOG_BARO_OFF;
						config_a = Integer.parseInt(tokens[2]);
					} else if (tokens[1].equals("tcs:")) {
						cmd = AltosLib.AO_LOG_BARO_TCS;
						config_a = Integer.parseInt(tokens[2]);
					} else if (tokens[1].equals("tco:")) {
						cmd = AltosLib.AO_LOG_BARO_TCO;
						config_a = Integer.parseInt(tokens[2]);
					} else if (tokens[1].equals("tref:")) {
						cmd = AltosLib.AO_LOG_BARO_TREF;
						config_a = Integer.parseInt(tokens[2]);
					} else if (tokens[1].equals("tempsens:")) {
						cmd = AltosLib.AO_LOG_BARO_TEMPSENS;
						config_a = Integer.parseInt(tokens[2]);
					} else if (tokens[1].equals("crc:")) {
						cmd = AltosLib.AO_LOG_BARO_CRC;
						config_a = Integer.parseInt(tokens[2]);
					} else {
						cmd = AltosLib.AO_LOG_INVALID;
						data = line;
					}
				} else {
					cmd = AltosLib.AO_LOG_INVALID;
					data = line;
				}
			} catch (NumberFormatException ne) {
				cmd = AltosLib.AO_LOG_INVALID;
				data = line;
			}
		}
	}

	public AltosEepromMini(int in_cmd, int in_tick) {
		cmd = in_cmd;
		tick = in_tick;
		valid = true;
	}
}
