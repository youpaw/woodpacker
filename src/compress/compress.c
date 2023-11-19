#include	<stdio.h>
#include	<string.h>

#include    "compress.h"

#ifndef SIG_TYPE
#	define	SIG_TYPE	void (*)()
#endif

#ifndef	LSTAT
#	define	lstat	stat
#endif

#undef	min
#define	min(a,b)	((a>b) ? b : a)

#ifndef	IBUFSIZ
#	define	IBUFSIZ	BUFSIZ	/* Default input buffer size							*/
#endif
#ifndef	OBUFSIZ
#	define	OBUFSIZ	BUFSIZ	/* Default output buffer size							*/
#endif

/* Defines for third byte of header 					*/
#define	MAGIC_1		(char_type)'\037'/* First byte of compressed file				*/
#define	MAGIC_2		(char_type)'\235'/* Second byte of compressed file				*/
#define BIT_MASK	0x1f			/* Mask for 'number of compression bits'		*/
/* Masks 0x20 and 0x40 are free.  				*/
/* I think 0x20 should mean that there is		*/
/* a fourth header byte (for expansion).    	*/
#define BLOCK_MODE	0x80			/* Block compression if table is full and		*/
/* compression rate is dropping flush tables	*/

/* the next two codes should not be changed lightly, as they must not	*/
/* lie within the contiguous general code space.						*/
#define FIRST	257					/* first free entry 							*/
#define	CLEAR	256					/* table clear output code 						*/

#define INIT_BITS 9			/* initial number of bits/code */

#	define	BITS   12
#	define HSIZE	5003

#ifndef	O_BINARY
#	define	O_BINARY	0	/* System has no binary mode							*/
#endif

#define CHECK_GAP 10000


typedef long int			code_int;

typedef long int	 		count_int;
typedef long int			cmp_code_int;
typedef	unsigned char	    char_type;

#define MAXCODE(n)	(1L << (n))

#define	output(b,o,c,n)	{	char_type	*p = &(b)[(o)>>3];					\
							long		 i = ((long)(c))<<((o)&0x7);		\
							p[0] |= (char_type)(i);							\
							p[1] |= (char_type)(i>>8);						\
							p[2] |= (char_type)(i>>16);						\
							(o) += (n);										\
						}
#define	input(b,o,c,n,m){	char_type 		*p = &(b)[(o)>>3];				\
							(c) = ((((long)(p[0]))|((long)(p[1])<<8)|		\
									 ((long)(p[2])<<16))>>((o)&0x7))&(m);	\
							(o) += (n);										\
						}

#define reset_n_bits_for_compressor(n_bits, stcode, free_ent, extcode, maxbits) {	\
	n_bits = INIT_BITS;								\
	stcode = 1;									\
	free_ent = FIRST;								\
	extcode = MAXCODE(n_bits);							\
	if (n_bits < maxbits)								\
		extcode++;								\
}

#define reset_n_bits_for_decompressor(n_bits, bitmask, maxbits, maxcode, maxmaxcode) {	\
	n_bits = INIT_BITS;								\
	bitmask = (1<<n_bits)-1;							\
	if (n_bits == maxbits)								\
		maxcode = maxmaxcode;							\
	else										\
		maxcode = MAXCODE(n_bits)-1;						\
}

char_type		inbuf[IBUFSIZ+64];	/* Input buffer									*/
char_type		outbuf[OBUFSIZ+2048];/* Output buffer								*/

static int		maxbits = BITS;

long 			bytes_in;			/* Total number of byte from input				*/
long 			bytes_out;			/* Total number of byte to output				*/

count_int		htab[HSIZE];
unsigned short	codetab[HSIZE];

#define	tab_prefixof(i)			codetab[i]
#define	tab_suffixof(i)			((char_type *)(htab))[i]
#define	de_stack				((char_type *)&(htab[HSIZE-1]))
#define	clear_htab()			memset(htab, -1, sizeof(htab))
#define	clear_tab_prefixof()	memset(codetab, 0, 256);

/*
* Assumptions:
*   When filenames are given, replaces with the compressed version
*   (.Z suffix) only if the file decreases in size.
*
* Algorithm:
*   Modified Lempel-Ziv method (LZW).  Basically finds common
*   substrings and replaces them with a variable size code.  This is
*   deterministic, and can be done on the fly.  Thus, the decompression
*   procedure needs no input table, but tracks the way the table was built.
*/

static int     copy_out_data(void *out, int size, size_t *out_indx)
{
    int i = 0;

    while(i < OBUFSIZ && i < size)
    {
        ((char_type*)out)[*out_indx] = outbuf[i];
        (*out_indx)++;
        i++;
    }
    return (i);
}

static int     copy_in_data(const void *in, size_t size, size_t *in_indx)
{
    int i = 0;
    int tmp = *in_indx;

    while(i < IBUFSIZ && *in_indx < size)
    {
        inbuf[i] = ((char_type*)in)[*in_indx];
        (*in_indx)++;
        i++;
    }
    return (i);
}

/*
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */
size_t	compress(const void *in, void *out, size_t size)
{
    long hp;
    int rpos;
    long fc;
    int outbits;
    int rlop;
    int rsize;
    int stcode;
    code_int free_ent;
    int boff;
    int n_bits;
    int ratio;
    long checkpoint;
    code_int extcode;
    size_t in_indx;
    size_t  out_indx;

    in_indx = 0;
    rsize = 0;
    out_indx = 0;
    union
    {
        long			code;
        struct
        {
            char_type		c;
            unsigned short	ent;
        } e;
    } fcode;

    ratio = 0;
    checkpoint = CHECK_GAP;
    reset_n_bits_for_compressor(n_bits, stcode, free_ent, extcode, maxbits);

    memset(outbuf, 0, sizeof(outbuf));
    bytes_out = 0; bytes_in = 0;
    outbuf[0] = MAGIC_1;
    outbuf[1] = MAGIC_2;
    outbuf[2] = (char)(maxbits | BLOCK_MODE);
    boff = outbits = (3<<3);
    fcode.code = 0;

    clear_htab();

    while (rsize < size && ((rsize = copy_in_data(in, size, &in_indx)) > 0))
    {
        if (bytes_in == 0)
        {
            fcode.e.ent = inbuf[0];
            rpos = 1;
        }
        else
            rpos = 0;

        rlop = 0;

        do
        {
            if (free_ent >= extcode && fcode.e.ent < FIRST)
            {
                if (n_bits < maxbits)
                {
                    boff = outbits = (outbits-1)+((n_bits<<3)-
                                                  ((outbits-boff-1+(n_bits<<3))%(n_bits<<3)));
                    if (++n_bits < maxbits)
                        extcode = MAXCODE(n_bits)+1;
                    else
                        extcode = MAXCODE(n_bits);
                }
                else
                {
                    extcode = MAXCODE(16)+OBUFSIZ;
                    stcode = 0;
                }
            }

            if (!stcode && bytes_in >= checkpoint && fcode.e.ent < FIRST)
            {
                long int rat;

                checkpoint = bytes_in + CHECK_GAP;

                if (bytes_in > 0x007fffff)
                {							/* shift will overflow */
                    rat = (bytes_out+(outbits>>3)) >> 8;

                    if (rat == 0)				/* Don't divide by zero */
                        rat = 0x7fffffff;
                    else
                        rat = bytes_in / rat;
                }
                else
                    rat = (bytes_in << 8) / (bytes_out+(outbits>>3));	/* 8 fractional bits */
                if (rat >= ratio)
                    ratio = (int)rat;
                else
                {
                    ratio = 0;
                    clear_htab();
                    output(outbuf,outbits,CLEAR,n_bits);
                    boff = outbits = (outbits-1)+((n_bits<<3)-
                                                  ((outbits-boff-1+(n_bits<<3))%(n_bits<<3)));
                    reset_n_bits_for_compressor(n_bits, stcode, free_ent, extcode, maxbits);
                }
            }

            if (outbits >= (OBUFSIZ<<3))
            {
                if (copy_out_data(out, OBUFSIZ, &out_indx) != OBUFSIZ)
                    return (1);

                outbits -= (OBUFSIZ<<3);
                boff = -(((OBUFSIZ<<3)-boff)%(n_bits<<3));
                bytes_out += OBUFSIZ;

                memcpy(outbuf, outbuf+OBUFSIZ, (outbits>>3)+1);
                memset(outbuf+(outbits>>3)+1, '\0', OBUFSIZ);
            }

            {
                int i;

                i = rsize-rlop;

                if ((code_int)i > extcode-free_ent)	i = (int)(extcode-free_ent);
                if (i > ((sizeof(outbuf) - 32)*8 - outbits)/n_bits)
                    i = ((sizeof(outbuf) - 32)*8 - outbits)/n_bits;

                if (!stcode && (long)i > checkpoint-bytes_in)
                    i = (int)(checkpoint-bytes_in);

                rlop += i;
                bytes_in += i;
            }

            goto next;
            hfound:			fcode.e.ent = codetab[hp];
            next:  			if (rpos >= rlop)
            goto endlop;
            next2: 			fcode.e.c = inbuf[rpos++];
            {
                code_int i;
                fc = fcode.code;
                hp = (((long)(fcode.e.c)) << (BITS-8)) ^ (long)(fcode.e.ent);

                if ((i = htab[hp]) == fc)
                    goto hfound;

                if (i != -1)
                {
                    long disp;

                    disp = (HSIZE - hp)-1;	/* secondary hash (after G. Knott) */

                    do
                    {
                        if ((hp -= disp) < 0)	hp += HSIZE;

                        if ((i = htab[hp]) == fc)
                            goto hfound;
                    }
                    while (i != -1);
                }
            }

            output(outbuf,outbits,fcode.e.ent,n_bits);

            {
                long fc = fcode.code;
                fcode.e.ent = fcode.e.c;

                if (stcode)
                {
                    codetab[hp] = (unsigned short)free_ent++;
                    htab[hp] = fc;
                }
            }

            goto next;

            endlop:			if (fcode.e.ent >= FIRST && rpos < rsize)
            goto next2;

            if (rpos > rlop)
            {
                bytes_in += rpos-rlop;
                rlop = rpos;
            }
        }
        while (rlop < rsize);
    }

    if (rsize < 0)
        return(1);

    if (bytes_in > 0)
    output(outbuf,outbits,fcode.e.ent,n_bits);

    if (copy_out_data(out, (outbits+7)>>3, &out_indx) != (outbits+7)>>3)
        return (1);

    bytes_out += (outbits+7)>>3;

    return (out_indx);
}
