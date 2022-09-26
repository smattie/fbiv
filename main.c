// SPDX-License-Identifier: WTFPL
//
// Copyright 2022 Matt
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the COPYING file for more details.

#define  _POSIX_C_SOURCE 199309L
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/termios.h>

#include "main.h"
#include "load.c"
#include "error.c"

#define KEYQUIT      'q'
#define KEYPRINTPATH 'p'
#define KEYRESET     '0'
#define KEYZOOMIN    'i'
#define KEYZOOMOUT   'o'
#define KEYPANUP     'k'
#define KEYPANDOWN   'j'
#define KEYPANLEFT   'h'
#define KEYPANRIGHT  'l'

#define PANSTEP 8

static u32 framebufw;
static u32 framebufh;
static u32*framebuf;

static i32 windowx;
static i32 windowy;
static u32 windoww;
static u32 windowh;

static struct termios termcfg;

static int
getwindowsize (void) {
	struct winsize ws;
	u32    cellw;
	u32    cellh;

	ws.ws_col = 80;
	ws.ws_row = 25;
	ioctl (1, TIOCGWINSZ, &ws);

	cellw = framebufw / ws.ws_col;
	cellh = framebufh / ws.ws_row;

	windowx = 0;
	windowy = 0;
	windoww = ws.ws_col * cellw;
	windowh = ws.ws_row * cellh;

	return 0; }

static int
mapframebuffer (void) {
	struct fb_fix_screeninfo fbfix;
	struct fb_var_screeninfo fbvar;
	int    fd;

	fd = open ("/dev/fb0", O_RDWR);
	if (fd < 0)
		return -1;

	ioctl (fd, FBIOGET_FSCREENINFO, &fbfix);
	ioctl (fd, FBIOGET_VSCREENINFO, &fbvar);

	framebufw = fbvar.xres;
	framebufh = fbvar.yres;
	framebuf  = mmap (0, framebufh * framebufw * 4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (framebuf == (void*)-1)
		return -3;

	close (fd);
	return 0; }

static int
initterm (int restore) {
	struct termios new;
	char  *esc;

	esc = "\x1b[?25h"; // enable cursor
	new = termcfg;
	if (!restore) {
		if (tcgetattr (0, &termcfg) != 0)
			return -1;

		new             = termcfg;
		new.c_lflag    &= ~(ICANON | ECHO);
		new.c_cc[VMIN ] = 1;
		new.c_cc[VTIME] = 0;

		esc = "\x1b[?25l"; } // disable cursor

	if (tcsetattr (1, TCSANOW, &new) != 0)
		return -2;

	write (1, esc, 6);
	ioctl (1, TIOCSTI, "\0"); // fake input - trigger redraw

	return 0; }

static void
swizzle (struct image *img) {
	for (u32 i = 0; i < img->height * img->width; ++i) {
		u32 p = img->data[i];
		u32 b = ((p >> 16) & 255) <<  0;
		u32 g = ((p >>  8) & 255) <<  8;
		u32 r = ((p >>  0) & 255) << 16;
		u32 a = ((p >> 24) & 255) << 24;

		img->data[i] = a | r | g | b; }

	return; }

static void
clear (void) {
	for (u32 i = 0; i < framebufh * framebufw; ++i)
		framebuf[i] = 0;

	return; }

extern int
main (int ac,  char **av, char **ev) {
	struct image img;
	int    err;
	f32    zoomw;
	f32    zoomh;
	f32    scalew;
	f32    scaleh;

	if ((err = mapframebuffer ()) < 0)
		framebuferror (err);

	if ((err = getwindowsize ()) < 0) {
		return 2; }

	if (loadimage ("/home/matt/dev/src/games/tools/sxed/test/heart.sx1", &img) < 0) {
		return 3; }

	if ((err = initterm (0)) < 0)
		termerror (err);

	zoomw  = (f32)img.width  * 0.05;
	zoomh  = (f32)img.height * 0.05;
	scalew = img.width;
	scaleh = img.height;

	for (;;) {
		u32*src;
		u32*dst;
		u32 input;
		f32 incx;
		f32 incy;
		i32 w;
		i32 h;
		int update;

		update = 0;
		input = 0;
		read (0, &input, sizeof (input));
		while (input != 0) {
			switch (input & 0xff) {
				case KEYQUIT : goto end;
				case KEYRESET:
					img.x  = 0;
					img.y  = 0;
					scalew = img.width;
					scaleh = img.height;
					update++;
					break;

				case KEYZOOMIN : scalew += zoomw; scaleh += zoomh; update++; break;
				case KEYZOOMOUT: scalew -= zoomw; scaleh -= zoomh; update++; break; }

			input >>= 8; }

		if (update)
			// otherwise the framebuffer doesn't get redrawn. we do it here
			// rather than at the so we dont draw a blank space over the image
			write (1, " \r", 2);

		if (scalew <= 0.0) scalew = 0.1;
		if (scaleh <= 0.0) scaleh = 0.1;

		w = scalew;
		h = scaleh;
		if (w <= 0) w = 1;
		if (h <= 0) h = 1;
		if (w >  windoww) w = windoww - 1;
		if (h >  windowh) h = windowh - 1;

		incx = (f32)img.width  / scalew;
		incy = (f32)img.height / scaleh;

		clear ();

		src = img.data;
		dst = &framebuf[windowy * framebufw + windowx];
		for (i32 y = 0; y < h; ++y) {
			u32 sampy;
			u32 sampx;

			sampy = (f32)y * incy;
			for (i32 x = 0; x < w; ++x) {
				sampx  = (f32)x * incx;
				dst[x] = src[sampy * img.width + sampx]; }

			dst += framebufw; }}

	end:
	initterm (1);
	return 0; }
