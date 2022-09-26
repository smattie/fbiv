// SPDX-License-Identifier: WTFPL
//
// Copyright 2022 Matt
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the COPYING file for more details.

#pragma once
#include <stdint.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint16_t f16;
typedef float    f32;
typedef int      bool;

#define elements(a) (sizeof(a) / sizeof(a[0]))
#define min(a, b)   ({ typeof(a) _a = (a), _b = (b); (_a < _b) ? _a : _b; })
#define max(a, b)   ({ typeof(a) _a = (a), _b = (b); (_a > _b) ? _a : _b; })
#define kib(n)      ((n)    * 1024)
#define mib(n)      (kib(n) * 1024)

#define print2(s, f) write (f, s, sizeof (s)-1)
#define print(s)     print2 (s, 1)

#define __str(s) #s
#define str(s)   __str (s)

#define ls(s) (sizeof(s)-1), (s)
#define sl(s) (s), (sizeof(s)-1)

#define noinline __attribute__ ((noinline))

struct linux_dirent64 {
	i64 d_ino;
	i64 d_off;
	unsigned short d_reclen;
	unsigned char  d_type;
	unsigned char  d_name[]; };

