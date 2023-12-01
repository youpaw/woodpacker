/*

LZ4X - An optimized LZ4 compressor

Written and placed in the public domain by Ilya Muravyov

*/

#include <inttypes.h>
#include <memory.h>
#include <stdlib.h>


#define LZ4_MAGIC_NUMBER 0x184C2102
#define BLOCK_SIZE (8<<20) // 8 MB

#define COMPRESS_BOUND (16+BLOCK_SIZE+(BLOCK_SIZE/255))

unsigned char buf[BLOCK_SIZE+COMPRESS_BOUND];

#define GET_BYTE() buf[BLOCK_SIZE+(bp++)]

void	*decompress(const void *in, size_t comp, size_t orig)
{
	__label__ err;
	size_t in_off = 0;
	uint32_t magic = *(uint32_t *)in;
	in_off += sizeof(uint32_t);

	if (magic != LZ4_MAGIC_NUMBER)
		return (NULL);

	void *out = malloc(orig);
	size_t out_off = 0;
	while (in_off < comp)
	{
		int bsize = *(int *)(in + in_off);
		in_off += sizeof(int);

		if (bsize < 0 || bsize > COMPRESS_BOUND)
			goto err;

		memcpy(&buf[BLOCK_SIZE], in + in_off, bsize);
		in_off += bsize;

		int p = 0;
		int bp = 0;
		while (bp < bsize)
		{
			const int tag = GET_BYTE();
			if (tag >= 16)
			{
				int run = tag >> 4;
				if (run == 15)
				{
					for (;;)
					{
						const int c = GET_BYTE();
						run += c;
						if (c != 255)
							break;
					}
				}

				buf[p++] = GET_BYTE();

				while (--run)
					buf[p++] = GET_BYTE();

				if (bp >= bsize)
					break;
			}

			int s = p - GET_BYTE();
			s -= GET_BYTE() << 8;

			int len = tag & 15;
			if (len == 15)
			{
				for (;;)
				{
					const int c = GET_BYTE();
					len += c;
					if (c != 255)
						break;
				}
			}

			buf[p++] = buf[s++];
			buf[p++] = buf[s++];
			buf[p++] = buf[s++];
			buf[p++] = buf[s++];

			while (len--)
				buf[p++] = buf[s++];
		}

		if (bp!=bsize)
			goto err;

		memcpy(&out[out_off], buf, p);
		out_off += p;
	}

	return (out);
	err:
	free(out);
	return (NULL);
}


//
// Created by youpaw on 7/18/22.
//

#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int fd = open("test.cmp", O_RDONLY);
	if (errno)
	{
		perror("Cannot open specified executable");
		return (errno);
	}
	size_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	void *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	size_t orig = 846888;
	void *data = decompress(map, size, orig);
	fd = open("test", O_WRONLY | O_CREAT | O_TRUNC,
			  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (fd == -1)
	{
		perror("Cannot create woody binary");
		return (-1);
	}
	write(fd, data, orig);
	close(fd);
	free(data);
	munmap(map, size);
	return 0;
}
