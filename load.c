// SPDX-License-Identifier: WTFPL
//
// Copyright 2022 Matt
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the COPYING file for more details.

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
//include "stb_image.h"

struct image {
	u32 *data;
	i32  x;
	i32  y;
	u32  width;
	u32  height; };

static u32 *
loadsx1 (char *path, int *width, int *height, int *depth) {
	u32 *data;
	int fd;

	data = 0;

	fd = open (path, O_RDONLY);
	if (fd > 0) {
		u32 hdr[4];

		if (read (fd, &hdr[0], 16) == 16) {
			if (hdr[0] == 0x11235853) {
				u32 size;

				size = hdr[1] * hdr[2] * hdr[3] * 4;
				data = malloc (size);
				if (data) {
					u32 *at = data;
					int  nb;

					do {
						nb = read (fd, at, size);
						if (nb >= 0) {
							size -= nb;
							at   += nb/4; }

						} while (size > 0);

					*width  = hdr[1];
					*height = hdr[2];
					*depth  = hdr[3]; }}}

		close (fd); }

	return data; }

static int
loadimage (char *path, struct image *img) {
	int w, h, d;
	u32 *p;

	p = loadsx1 (path, &w, &h, &d);
	if (p == 0)
		return -1;

	img->data   = p;
	img->x      = 0;
	img->y      = 0;
	img->width  = w;
	img->height = h;

	return 0; }
