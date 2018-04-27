/*
 * Copyright © 2010 Keith Packard <keithp@keithp.com>
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

import java.io.*;

public class AltosRomconfig {
	public boolean	valid;
	public int	version;
	public int	check;
	public int	serial_number;
	public int	radio_calibration;
	public AltosUsbId	usb_id;
	public String		usb_product;

	static private long find_address(AltosHexfile hexfile, String name, int len) throws AltosNoSymbol {
		AltosHexsym symbol = hexfile.lookup_symbol(name);
		if (symbol == null) {
			System.out.printf("no symbol %s\n", name);
			throw new AltosNoSymbol(name);
		}
		if (hexfile.address <= symbol.address && symbol.address + len <= hexfile.max_address) {
			System.out.printf("%s: %x\n", name, symbol.address);
			return symbol.address;
		}
		System.out.printf("invalid symbol addr %x len %d range is %x - %x\n",
				  symbol.address, len, hexfile.address, hexfile.max_address);
		throw new AltosNoSymbol(name);
	}

	static private int find_offset(AltosHexfile hexfile, String name, int len) throws AltosNoSymbol {
		return (int) (find_address(hexfile, name, len) - hexfile.address);
	}

	static int get_int(AltosHexfile hexfile, String name, int len) throws AltosNoSymbol {
		byte[] bytes = hexfile.data;
		int start = (int) find_offset(hexfile, name, len);

		int	v = 0;
		int	o = 0;
		while (len > 0) {
			v = v | ((((int) bytes[start]) & 0xff) << o);
			start++;
			len--;
			o += 8;
		}
		return v;
	}

	static void put_int(int value, AltosHexfile hexfile, String name, int len) throws AltosNoSymbol, IOException {
		byte[] bytes = hexfile.data;
		int start = find_offset(hexfile, name, len);

		while (len > 0) {
			bytes[start] = (byte) (value & 0xff);
			start++;
			len--;
			value >>= 8;
		}
	}

	static void put_string(String value, AltosHexfile hexfile, String name) throws AltosNoSymbol {
		byte[] bytes = hexfile.data;
		int start = find_offset(hexfile, name, value.length());

		for (int i = 0; i < value.length(); i++)
			bytes[start + i] = (byte) value.charAt(i);
	}

	static final int AO_USB_DESC_STRING	= 3;

	static void put_usb_serial(int value, AltosHexfile hexfile, String name) throws AltosNoSymbol {
		byte[] bytes = hexfile.data;
		int start = find_offset(hexfile, name, 2);

		int string_num = 0;

		while (start < bytes.length && bytes[start] != 0) {
			if (bytes[start + 1] == AO_USB_DESC_STRING) {
				++string_num;
				if (string_num == 4)
					break;
			}
			start += ((int) bytes[start]) & 0xff;
		}
		if (start >= bytes.length || bytes[start] == 0)
			throw new AltosNoSymbol(name);

		int len = ((((int) bytes[start]) & 0xff) - 2) / 2;
		String fmt = String.format("%%0%dd", len);

		String s = String.format(fmt, value);
		if (s.length() != len)
			throw new AltosNoSymbol(String.format("weird usb length issue %s isn't %d\n", s, len));

		for (int i = 0; i < len; i++) {
			bytes[start + 2 + i*2] = (byte) s.charAt(i);
			bytes[start + 2 + i*2+1] = 0;
		}
	}

	final static String ao_romconfig_version = "ao_romconfig_version";
	final static String ao_romconfig_check = "ao_romconfig_check";
	final static String ao_serial_number = "ao_serial_number";
	final static String ao_radio_cal = "ao_radio_cal";
	final static String ao_usb_descriptors = "ao_usb_descriptors";

	public AltosRomconfig(AltosHexfile hexfile) {
		try {
			System.out.printf("Attempting symbols\n");
			version = get_int(hexfile, ao_romconfig_version, 2);
			System.out.printf("version %d\n", version);
			check = get_int(hexfile, ao_romconfig_check, 2);
			System.out.printf("check %d\n", check);
			if (check == (~version & 0xffff)) {
				switch (version) {
				case 2:
				case 1:
					serial_number = get_int(hexfile, ao_serial_number, 2);
					System.out.printf("serial %d\n", serial_number);
					try {
						radio_calibration = get_int(hexfile, ao_radio_cal, 4);
					} catch (AltosNoSymbol missing) {
						radio_calibration = 0;
					}
					valid = true;
					break;
				}
			}
			System.out.printf("attempting usbid\n");
			usb_id = hexfile.find_usb_id();
			if (usb_id == null)
				System.out.printf("No usb id\n");
			else
				System.out.printf("usb id: %04x:%04x\n",
						  usb_id.vid, usb_id.pid);
			usb_product = hexfile.find_usb_product();
			if (usb_product == null)
				System.out.printf("No usb product\n");
			else
				System.out.printf("usb product: %s\n", usb_product);

		} catch (AltosNoSymbol missing) {
			valid = false;
		}
	}

	private final static String[] fetch_names = {
		ao_romconfig_version,
		ao_romconfig_check,
		ao_serial_number,
		ao_radio_cal,
		ao_usb_descriptors,
	};

	private static int fetch_len(String name) {
		if (name.equals(ao_usb_descriptors))
			return 256;
		return 2;
	}

	private final static String[] required_names = {
		ao_romconfig_version,
		ao_romconfig_check,
		ao_serial_number
	};

	private static boolean name_required(String name) {
		for (String required : required_names)
			if (name.equals(required))
				return true;
		return false;
	}

	public static long fetch_base(AltosHexfile hexfile) throws AltosNoSymbol {
		long	base = 0xffffffffL;
		for (String name : fetch_names) {
			try {
				int	len = fetch_len(name);
				long	addr = find_address(hexfile, name, len);

				if (addr < base)
					base = addr;
				System.out.printf("symbol %s at %x base %x\n", name, addr, base);
			} catch (AltosNoSymbol ns) {
				if (name_required(name))
					throw (ns);
			}
		}
		return base;
	}

	public static long fetch_bounds(AltosHexfile hexfile) throws AltosNoSymbol {
		long	bounds = 0;
		for (String name : fetch_names) {
			try {
				int	len = fetch_len(name);
				long	addr = find_address(hexfile, name, len) + len;
				if (addr > bounds)
					bounds = addr;
				System.out.printf("symbol %s at %x bounds %x\n", name, addr, bounds);
			} catch (AltosNoSymbol ns) {
				if (name_required(name))
					throw (ns);
			}
		}

		return bounds;
	}

	public void write (AltosHexfile hexfile) throws IOException {
		if (!valid)
			throw new IOException("rom configuration invalid");

		AltosRomconfig existing = new AltosRomconfig(hexfile);
		if (!existing.valid)
			throw new IOException("image does not contain existing rom config");

		try {
			switch (existing.version) {
			case 2:
				try {
					put_usb_serial(serial_number, hexfile, ao_usb_descriptors);
				} catch (AltosNoSymbol missing) {
				}
				/* fall through ... */
			case 1:
				put_int(serial_number, hexfile, ao_serial_number, 2);
				try {
					put_int(radio_calibration, hexfile, ao_radio_cal, 4);
				} catch (AltosNoSymbol missing) {
				}
				break;
			}
		} catch (AltosNoSymbol missing) {
			throw new IOException(missing.getMessage());
		}

		AltosRomconfig check = new AltosRomconfig(hexfile);
		if (!check.valid)
			throw new IOException("writing new rom config failed\n");
	}

	public AltosRomconfig(int in_serial_number, int in_radio_calibration) {
		valid = true;
		version = 1;
		check = (~version & 0xffff);
		serial_number = in_serial_number;
		radio_calibration = in_radio_calibration;
	}

	public boolean valid() {
		return valid && serial_number != 0;
	}

	public AltosRomconfig() {
		valid = false;
	}
}
