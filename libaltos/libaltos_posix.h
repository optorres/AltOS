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

#ifndef _LIBALTOS_POSIX_H_
#define _LIBALTOS_POSIX_H_

#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

struct altos_file_posix {
	struct altos_file		file;

	int				fd;
#ifdef USE_POLL
	int				pipe[2];
#else
	int				out_fd;
#endif
};

void
altos_set_last_posix_error(void);

#endif /* _LIBALTOS_POSIX_H_ */