// SPDX-License-Identifier: WTFPL
//
// Copyright 2022 Matt
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the COPYING file for more details.

static noinline void
framebuferror (int e) {
	switch (-e) {
		case 1: print2 ("failed to open /dev/fb0\n",        2); break;
		case 2: print2 ("unsupported framebuffer format\n", 2); break;
		case 3: print2 ("failed to map framebuffer\n",      2); break; }

	exit (1);
	return; }

static noinline void
termerror (int e) {
	switch (-e) {
		case 1: print2 ("failed to read term config\n",  2); break;
		case 2: print2 ("failed to write term config\n", 2); break; }

	exit (2);
	return; }
