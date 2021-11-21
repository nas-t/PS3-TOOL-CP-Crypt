// some functions borrowed from:
// Sven Peter <svenpeter@gmail.com>
// Segher Boessenkool  <segher@kernel.crashing.org>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef TOOLS_H__
#define TOOLS_H__ 1
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

void fail(const char *a, ...);
void *mmap_file(const char *path);
void get_rand(u8 *bfr, u32 size);

#endif
