/*
 * Copyright © 2016 Keith Packard <keithp@keithp.com>
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

#ifndef _LIBALTOS_PRIVATE_H_
#define _LIBALTOS_PRIVATE_H_

#include "libaltos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLUETOOTH_PRODUCT_TELEBT	"TeleBT"

#define USB_BUF_SIZE	64

struct altos_file {
	/* Shared data */
	unsigned char			out_data[USB_BUF_SIZE];
	int				out_used;
	unsigned char			in_data[USB_BUF_SIZE];
	int				in_used;
	int				in_read;
};

#ifdef LINUX
#define USE_POLL
#endif

#ifdef DARWIN
#include <unistd.h>

#define strndup(s,n) altos_strndup(s,n)

char *altos_strndup(const char *s, size_t n);

#endif

void
altos_set_last_error(int code, char *string);

extern struct altos_error altos_last_error;

PUBLIC int
altos_flush(struct altos_file *file);

int
altos_fill(struct altos_file *file, int timeout);

#endif /* _LIBALTOS_PRIVATE_H_ */