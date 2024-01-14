/*

LZ4X - An optimized LZ4 compressor

Written and placed in the public domain by Ilya Muravyov

*/

#include <sys/mman.h>
#include <stdint.h>
#define LZ4_MAGIC_NUMBER 0x184C2102
#define BLOCK_SIZE (8<<20) // 8 MB

#define COMPRESS_BOUND (16+BLOCK_SIZE+(BLOCK_SIZE/255))

#define GET_BYTE() buf[BLOCK_SIZE+(bp++)]

static void	*ft_memcpy(void *dst, const void *src, uint32_t n)
{
	char *d = dst;
	const char *s = src;

	while (n--)
		*d++ = *s++;
	return dst;
}

uint32_t decompress(const void *in, uint32_t comp, void *out, unsigned char *buf)
{
	__label__ err;
	uint32_t in_off = 0;
	
	uint32_t magic = *(uint32_t *)in;
	in_off += sizeof(uint32_t);

	if (magic != LZ4_MAGIC_NUMBER)
		goto err;

	uint32_t out_off = 0;
	while (in_off < comp)
	{
		int bsize = *(int *)(in + in_off);
		in_off += sizeof(int);

		if (bsize < 0 || bsize > COMPRESS_BOUND)
			goto err;;

		ft_memcpy(&buf[BLOCK_SIZE], in + in_off, bsize);
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

		ft_memcpy(out + out_off, buf, p);
		out_off += p;
	}

	return (out_off);
	err:
	return (0);
}

/*
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int fd = open("test.cmp", O_RDONLY);
	if (errno)
	{
		perror("Cannot open specified executable");
		return (errno);
	}
	uint32_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	void *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	uint32_t orig = 846888;
	void *data = malloc(orig);
	unsigned char *buf = mmap(0, BLOCK_SIZE+COMPRESS_BOUND, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	decompress(map, size, data, buf);
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
	munmap(buf, BLOCK_SIZE+COMPRESS_BOUND);
	return 0;
}
*/
