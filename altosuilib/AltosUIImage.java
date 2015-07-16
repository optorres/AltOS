/*
 * Copyright © 2015 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.altoslib_8;

import javax.swing.*;
import javax.imageio.ImageIO;
import java.awt.image.*;
import java.awt.*;
import java.io.*;
import java.net.*;

public class AltosUIImage implements AltosImage {
	public Image	image;

	/* Discard storage for image */
	public void flush() {
		image.flush();
	}

	public AltosUIImage(Image image) {
		this.image = image;
	}
}
