// some functions borrowed from:
// Sven Peter <svenpeter@gmail.com>
// Segher Boessenkool  <segher@kernel.crashing.org>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
#include "mingw_mmap.h"
#include <windows.h>
#include <wincrypt.h>
#else
#include <sys/mman.h>
#endif

#include "tools.h"





void *mmap_file(const char *path)
{
	int fd;
	struct stat st;
	void *ptr;

	fd = open(path, O_RDONLY);
	if(fd == -1)
		fail("open %s", path);
	if(fstat(fd, &st) != 0)
		fail("fstat %s", path);

	ptr = mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if(ptr==NULL)
		fail("mmap");
	close(fd);

	return ptr;
}


void fail(const char *a, ...)
{
	char msg[1024];
	va_list va;

	va_start(va, a);
	vsnprintf(msg, sizeof msg, a, va);
	fprintf(stderr, "%s\n", msg);
	perror("perror");

	exit(1);
}


#ifdef WIN32
void get_rand(u8 *bfr, u32 size)
{
	HCRYPTPROV hProv;

	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		fail("unable to open random");

	if (!CryptGenRandom(hProv, size, bfr))
		fail("unable to read random numbers");

	CryptReleaseContext(hProv, 0);
}
#else
void get_rand(u8 *bfr, u32 size)
{
	FILE *fp;

	fp = fopen("/dev/urandom", "r");
	if (fp == NULL)
		fail("unable to open random");

	if (fread(bfr, size, 1, fp) != 1)
		printf("unable to read /dev/urandom");

	fclose(fp);
}
#endif

void print_hash(u8 *ptr, u32 len)
{
	while(len--)
		printf(" %02x", *ptr++);
}


