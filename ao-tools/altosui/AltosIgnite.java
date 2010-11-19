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

import java.io.*;

public class AltosIgnite {
	AltosDevice	device;
	AltosSerial	serial;
	boolean		remote;
	final static int	None = 0;
	final static int	Apogee = 1;
	final static int	Main = 2;

	final static int	Unknown = 0;
	final static int	Ready = 1;
	final static int	Active = 2;
	final static int	Open = 3;

	private void start_serial() throws InterruptedException {
		if (remote) {
			serial.set_channel(AltosPreferences.channel(device.getSerial()));
			serial.set_callsign(AltosPreferences.callsign());
			serial.printf("~\np\n");
			serial.flush_input();
		}
	}

	private void stop_serial() throws InterruptedException {
		if (serial == null)
			return;
		if (remote) {
			serial.printf("~");
			serial.flush_output();
		}
	}

	class string_ref {
		String	value;

		public String get() {
			return value;
		}
		public void set(String i) {
			value = i;
		}
		public string_ref() {
			value = null;
		}
	}

	private boolean get_string(String line, String label, string_ref s) {
		if (line.startsWith(label)) {
			String	quoted = line.substring(label.length()).trim();

			if (quoted.startsWith("\""))
				quoted = quoted.substring(1);
			if (quoted.endsWith("\""))
				quoted = quoted.substring(0,quoted.length()-1);
			s.set(quoted);
			return true;
		} else {
			return false;
		}
	}

	private int status(String status_name) {
		if (status_name.equals("unknown"))
			return Unknown;
		if (status_name.equals("ready"))
			return Ready;
		if (status_name.equals("active"))
			return Active;
		if (status_name.equals("open"))
			return Open;
		return Unknown;
	}

	public int status(int igniter) {
		int status = Unknown;
		if (serial == null)
			return status;
		string_ref status_name = new string_ref();
		try {
			start_serial();
			serial.printf("t\n");
			for (;;) {
				String line = serial.get_reply();
				if (get_string(line, "Igniter: drogue Status: ", status_name))
					if (igniter == Apogee)
						status = status(status_name.get());
				if (get_string(line, "Igniter:   main Status: ", status_name)) {
					if (igniter == Main)
						status = status(status_name.get());
					break;
				}
			}
		} catch (InterruptedException ie) {
		} finally {
			try {
				stop_serial();
			} catch (InterruptedException ie) {
			}
		}
		return status;
	}

	public void fire(int igniter) {
		if (serial == null)
			return;
		try {
			start_serial();
			switch (igniter) {
			case Main:
				serial.printf("i DoIt main\n");
				break;
			case Apogee:
				serial.printf("i DoIt drogue\n");
				break;
			}
		} catch (InterruptedException ie) {
		} finally {
			try {
				stop_serial();
			} catch (InterruptedException ie) {
			}
		}
	}

	public AltosIgnite(AltosDevice in_device) throws FileNotFoundException, AltosSerialInUseException {

		device = in_device;
		serial = null;
//		serial = new AltosSerial(device);
		remote = false;

//		if (!device.matchProduct(AltosDevice.product_telemetrum))
//			remote = true;
	}
}