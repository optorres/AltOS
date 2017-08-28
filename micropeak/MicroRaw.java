/*
 * Copyright © 2013 Keith Packard <keithp@keithp.com>
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
import java.io.*;
import javax.swing.*;
import org.altusmetrum.altoslib_12.*;
import org.altusmetrum.altosuilib_12.*;

public class MicroRaw extends JTextArea implements AltosFontListener {

	public void font_size_changed(int font_size) {
		setFont(AltosUILib.table_value_font);
	}

	public void setData(MicroData data) {
		StringWriter	sw = new StringWriter();
		try {
			data.export(sw);
			setRows(data.length());
			setText(sw.toString());
		} catch (IOException ie) {
			setText(String.format("Error writing data: %s", ie.getMessage()));
		}
		setCaretPosition(0);
	}

	public void tell_closing() {
		AltosUIPreferences.unregister_font_listener(this);
	}

	public MicroRaw() {
		super(1, 30);
		setFont(AltosUILib.table_value_font);
		setEditable(false);
		AltosUIPreferences.register_font_listener(this);
	}
}
