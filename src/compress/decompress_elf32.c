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
