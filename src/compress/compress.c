#include <memory.h>

typedef unsigned char byte;
typedef unsigned int uint;

#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))

#define LZ4_MAGIC_NUMBER 0x184C2102
#define BLOCK_SIZE (8<<20) // 8 MB
#define PADDING_LITERALS 8

#define WLOG 16
#define WSIZE (1<<WLOG)
#define WMASK (WSIZE-1)

#define MIN_MATCH 4

#define COMPRESS_BOUND (16+BLOCK_SIZE+(BLOCK_SIZE/255))

byte buf[BLOCK_SIZE+COMPRESS_BOUND];

#define HASH_LOG 18
#define HASH_SIZE (1<<HASH_LOG)
#define NIL (-1)

#if defined(__INTEL_COMPILER) && !defined(FORCE_UNALIGNED)
#define LOAD32(p) (*reinterpret_cast<const uint*>(&buf[p]))
#else
static inline uint LOAD32(int p) {
    uint v;
    memcpy(&v, &(buf[p]), sizeof(uint));
    return v;
}
#endif

#define HASH32(p) ((LOAD32(p)*0x125A517D)>>(32-HASH_LOG))

#define PUT_BYTE(c) (buf[BLOCK_SIZE+(bsize++)]=(c))


size_t compress(const void *in, void *out, size_t size)
{
    size_t out_off = 0;
    static int head[HASH_SIZE];
    static int nodes[WSIZE][2];
    static struct
    {
        int bcount;

        int len;
        int dist;
    } path[BLOCK_SIZE];

#ifdef LZ4_MAGIC_NUMBER
    const uint magic=LZ4_MAGIC_NUMBER;
    memcpy(out, &magic, sizeof(magic));
    out_off += sizeof(magic);
#endif

    int n;
    size_t in_off = 0;

    while (in_off < size)
    {
        // Pass 1: Find all matches
        n = (size - in_off) < BLOCK_SIZE ? (size - in_off) : BLOCK_SIZE;
        memcpy(buf, in + in_off, n);
        in_off += n;
        for (int i=0; i<HASH_SIZE; ++i)
            head[i]=NIL;

        for (int p=0; p < n; ++p)
        {
            int best_len=0;
            int dist;

            const int max_match= (n - PADDING_LITERALS) - p;
            if (max_match>=MIN_MATCH)
            {
                int* left=&nodes[p&WMASK][1];
                int* right=&nodes[p&WMASK][0];

                int left_len=0;
                int right_len=0;

                const int wstart=MAX(p - WSIZE, NIL);

                const uint h=HASH32(p);
                int s=head[h];
                head[h]=p;

                while (s>wstart)
                {
                    int len=MIN(left_len, right_len);

                    if (buf[s+len]==buf[p+len])
                    {
                        while (++len<max_match && buf[s+len]==buf[p+len]);

                        if (len>best_len)
                        {
                            best_len=len;
                            dist=p-s;

                            if (len==max_match)
                                break;
                        }
                    }

                    if (buf[s+len]<buf[p+len])
                    {
                        *right=s;
                        right=&nodes[s&WMASK][1];
                        s=*right;
                        right_len=len;
                    }
                    else
                    {
                        *left=s;
                        left=&nodes[s&WMASK][0];
                        s=*left;
                        left_len=len;
                    }
                }

                *left=NIL;
                *right=NIL;
            }

            path[p].len=best_len;
            path[p].dist=dist;
        }

        // Pass 2: Build the shortest path

        path[n].bcount=0;

        int state=15;

        for (int p = n - 1; p > 0; --p)
        {
            int c0=path[p+1].bcount+1;

            if (!--state)
            {
                state=255;
                ++c0;
            }

            if (path[p].len>=MIN_MATCH)
            {
                path[p].bcount=1<<30;

                for (int i=path[p].len; i>=MIN_MATCH; --i)
                {
                    int c1=path[p+i].bcount+3;

                    int len=i-MIN_MATCH;
                    if (len>=15)
                    {
                        len-=15;
                        for (; len>=255; len-=255)
                            ++c1;
                        ++c1;
                    }

                    if (c1<path[p].bcount)
                    {
                        path[p].bcount=c1;
                        path[p].len=i;
                    }
                }

                if (c0<path[p].bcount)
                {
                    path[p].bcount=c0;
                    path[p].len=0;
                }
                else
                    state=15;
            }
            else
                path[p].bcount=c0;
        }

        // Pass 3: Output the codes

        int bsize=0;
        int pp=0;

        int p=0;
        while (p < n)
        {
            if (path[p].len>=MIN_MATCH)
            {
                int len=path[p].len-MIN_MATCH;
                const int adder=MIN(len, 15);

                if (pp<p)
                {
                    int run=p-pp;
                    if (run>=15)
                    {
                        PUT_BYTE((15<<4)+adder);

                        run-=15;
                        for (; run>=255; run-=255)
                            PUT_BYTE(255);
                        PUT_BYTE(run);
                    }
                    else
                        PUT_BYTE((run<<4)+adder);

                    while (pp<p)
                        PUT_BYTE(buf[pp++]);
                }
                else
                    PUT_BYTE(adder);

                PUT_BYTE(path[p].dist);
                PUT_BYTE(path[p].dist>>8);

                if (len>=15)
                {
                    len-=15;
                    for (; len>=255; len-=255)
                        PUT_BYTE(255);
                    PUT_BYTE(len);
                }

                p+=path[p].len;

                pp=p;
            }
            else
                ++p;
        }

        if (pp<p)
        {
            int run=p-pp;
            if (run>=15)
            {
                PUT_BYTE(15<<4);

                run-=15;
                for (; run>=255; run-=255)
                    PUT_BYTE(255);
                PUT_BYTE(run);
            }
            else
                PUT_BYTE(run<<4);

            while (pp<p)
                PUT_BYTE(buf[pp++]);
        }

        memcpy(out + out_off, &bsize, sizeof(bsize));
        out_off += sizeof(bsize);
        memcpy(out + out_off, &buf[BLOCK_SIZE], bsize);
        out_off += bsize;
    }
    return (out_off);
}

