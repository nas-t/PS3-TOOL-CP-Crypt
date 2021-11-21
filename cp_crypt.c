// Copyright 2013       nas
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include "tools.h"
#include "aes.h"
#include "types.h"
#include "md5.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>

u8 cp_update_key[0x20] =
{
	0xE8, 0xED, 0x2B, 0x81, 0x72, 0x07, 0xB7, 0x0C, 0x5D, 0xF9, 0x09, 0x05, 0x07, 0xAF, 0x2A, 0x89, 0x82, 0x96, 0x76, 0x20, 0xD6, 0x92, 0xB9, 0x2A, 0x59, 0x23, 0x16, 0x38, 0x40, 0x2D, 0xF1, 0x3F

};

static u64 get_filesize (const char *path)
{
	struct stat st ;
	stat (path, &st) ;
	return st.st_size ;
}

void decrypt(char * in_path, char * out_path)
{
	u8 iv[0x10];
	u8 md5_sum[0x10];
	u8 buf[0x10];
	AES_KEY my_key;
	u32 size = 0;
	MD5_CTX ctx;

	printf("decrypting...\n");

	u64 filesize = get_filesize(in_path);

	if(filesize < 0x20)
	{
		printf("update file error\n");
		exit(0);
	}

	filesize -= 0x20;	

	FILE * in = fopen(in_path, "rb");
	FILE * out = fopen(out_path, "wb");

	if(!fread(iv, 0x10, 1, in))
		fail("invalid CP update\n");
	if(!fread(md5_sum, 0x10, 1, in))
		fail("invalid CP update\n");

	AES_set_decrypt_key(cp_update_key, 0x100, &my_key);
	MD5Init (&ctx);

	while(size = fread(buf, 1, 0x10, in))
	{
		AES_cbc_encrypt(buf, buf, 0x10, &my_key, iv, 0);
		
		if(filesize == 0x10)
			size -= buf[0xF];

		if(size)
		{
			MD5Update (&ctx, buf, size);
			fwrite(buf, size, 1, out);
		}
		
		filesize -= 0x10;
	}

	MD5Final (&ctx);


	if(memcmp(md5_sum,ctx.digest, 0x10))
	{
		printf("MD5 FAIL!\n");
		printf("md5 expected  : ");
		print_hash(md5_sum, 0x10);
		printf("\n");

		printf("md5 calculated: ");
		print_hash(ctx.digest, 0x10);
		printf("\n");
	}

	fclose(out);
	fclose(in);	
}


void encrypt(char * in_path, char * out_path)
{
	u8 iv[0x10];
	u8 md5_sum[0x10];
	u8 buf[0x10];
	AES_KEY my_key;
	u32 size = 0;
	u32 rest = 0;
	MD5_CTX ctx;

	printf("encrypting...\n");

	u64 filesize = get_filesize(in_path);

	if(filesize < 0x20)
	{
		printf("update file error\n");
		exit(0);
	}
	
	rest = filesize % 0x10;

	FILE * in = fopen(in_path, "rb");
	FILE * out = fopen(out_path, "wb");


	get_rand(iv, 0x10);
	fwrite(iv, 0x10, 1, out);
	fwrite(buf, 0x10, 1, out);

	AES_set_encrypt_key(cp_update_key, 0x100, &my_key);
	MD5Init (&ctx);

	while(size = fread(buf, 1, 0x10, in))
	{
		
		if(filesize < 0x10 && size < 0x10)
			buf[0xF] = 0x10 - size;

		MD5Update (&ctx, buf, size);

		AES_cbc_encrypt(buf, buf, 0x10, &my_key, iv, 1);

		fwrite(buf, 0x10, 1, out);		
		filesize -= 0x10;

		if(filesize == 0 && rest == 0)
		{
			memset(buf, 0, 0x10);
			buf[0xF] = 0x10;
			AES_cbc_encrypt(buf, buf, 0x10, &my_key, iv, 1);
			fwrite(buf, 0x10, 1, out);
		}
	}

	MD5Final (&ctx);


	fseek(out, 0x10, SEEK_SET);
	fwrite(ctx.digest, 0x10, 1, out);

	fclose(out);
	fclose(in);	
}

int main(int argc, char *argv[])
{

	printf("CP Update Crypt\nby nas 2013\n\n");

	if(argc == 4 && (strcmp(argv[1], "-d") == 0))
		decrypt(argv[2], argv[3]);
	else if(argc == 4 && (strcmp(argv[1], "-e") == 0))
		encrypt(argv[2], argv[3]);
	else
		printf("usage: %s option infile outfile\noptions: -d\tdecrypt\n\t -e\tencrypt\n", argv[0]);	

}
