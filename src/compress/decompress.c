#include	<stdio.h>
#include	<stdlib.h>
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

static  int				maxbits = BITS;

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

static int     copy_in_data(const void *in, size_t comp, int insize, size_t *in_indx)
{
    int i = 0;

    while(i < IBUFSIZ && *in_indx < comp)
    {
        (inbuf + insize)[i] = ((char_type*)in)[*in_indx];
        (*in_indx)++;
        i++;
    }
    return (i);
}

void    *decompress(const void *in, size_t comp, size_t orig)
{
    char_type *stackp;
    code_int code;
    int finchar;
    code_int oldcode;
    code_int incode;
    int inbits;
    int posbits;
    int outpos;
    int insize;
    int bitmask;
    code_int free_ent;
    code_int maxcode;
    code_int maxmaxcode;
    int n_bits;
    int rsize;
    int block_mode;
    size_t compressed_indx;
    size_t in_indx;
    size_t out_indx;
    char_type *out;

    bytes_in = 0;
    bytes_out = 0;
    insize = 0;
    compressed_indx = 0;
    in_indx = 0;
    out_indx = 0;
    out = (char_type*)malloc(orig * sizeof(char_type*));
    while (insize < 3)
    {
        rsize = copy_in_data(in, comp, insize, &in_indx);
        insize += rsize;
    }

    if (insize < 3 || inbuf[0] != MAGIC_1 || inbuf[1] != MAGIC_2)
    {
        if (rsize < 0 || insize > 0)
            return (out);
    }

    maxbits = inbuf[2] & BIT_MASK;
    block_mode = inbuf[2] & BLOCK_MODE;

    if (maxbits > BITS)
        return (out);

    maxmaxcode = MAXCODE(maxbits);

    bytes_in = insize;
    reset_n_bits_for_decompressor(n_bits, bitmask, maxbits, maxcode, maxmaxcode);
    oldcode = -1;
    finchar = 0;
    outpos = 0;
    posbits = 3<<3;

    free_ent = ((block_mode) ? FIRST : 256);

    clear_tab_prefixof();	/* As above, initialize the first
								   256 entries in the table. */

    for (code = 255 ; code >= 0 ; --code)
        tab_suffixof(code) = (char_type)code;

    do
    {
        resetbuf:	;
        {
            int i;
            int e;
            int o;

            o = posbits >> 3;
            e = o <= insize ? insize - o : 0;

            for (i = 0 ; i < e ; ++i)
                inbuf[i] = inbuf[i+o];

            insize = e;
            posbits = 0;
        }

        if (insize < sizeof(inbuf)-IBUFSIZ)
        {
            rsize = copy_in_data(in, comp, insize, &in_indx);
            insize += rsize;

            if (rsize < 0)
                return (out);

        }

        inbits = ((rsize > 0) ? (insize - insize%n_bits)<<3 :
                  (insize<<3)-(n_bits-1));

        while (inbits > posbits)
        {
            if (free_ent > maxcode)
            {
                posbits = ((posbits-1) + ((n_bits<<3) -
                                          (posbits-1+(n_bits<<3))%(n_bits<<3)));

                ++n_bits;
                if (n_bits == maxbits)
                    maxcode = maxmaxcode;
                else
                    maxcode = MAXCODE(n_bits)-1;

                bitmask = (1<<n_bits)-1;
                goto resetbuf;
            }

            input(inbuf,posbits,code,n_bits,bitmask);

            if (oldcode == -1)
            {
                outbuf[outpos++] = (char_type)(finchar = (int)(oldcode = code));
                continue;
            }

            if (code == CLEAR && block_mode)
            {
                clear_tab_prefixof();
                free_ent = FIRST - 1;
                posbits = ((posbits-1) + ((n_bits<<3) -
                                          (posbits-1+(n_bits<<3))%(n_bits<<3)));
                reset_n_bits_for_decompressor(n_bits, bitmask, maxbits, maxcode, maxmaxcode);
                goto resetbuf;
            }

            incode = code;
            stackp = de_stack;

            if (code >= free_ent)	/* Special case for KwKwK string.	*/
            {
                if (code > free_ent)
                {
                    char_type *p;

                    posbits -= n_bits;
                    p = &inbuf[posbits>>3];
                    if (p == inbuf) p++;
                }

                *--stackp = (char_type)finchar;
                code = oldcode;
            }

            while ((cmp_code_int)code >= (cmp_code_int)256)
            { /* Generate output characters in reverse order */
                *--stackp = tab_suffixof(code);
                code = tab_prefixof(code);
            }

            *--stackp =	(char_type)(finchar = tab_suffixof(code));

            /* And put them out in forward order */

            {
                int i;

                if (outpos+(i = (de_stack-stackp)) >= OBUFSIZ)
                {
                    do
                    {
                        if (i > OBUFSIZ-outpos) i = OBUFSIZ-outpos;

                        if (i > 0)
                        {
                            memcpy(outbuf+outpos, stackp, i);
                            outpos += i;
                        }

                        if (outpos >= OBUFSIZ)
                        {
                            if (copy_out_data(out, outpos, &out_indx) != outpos)
                                return (out);

                            outpos = 0;
                        }
                        stackp+= i;
                    }
                    while ((i = (de_stack-stackp)) > 0);
                }
                else
                {
                    memcpy(outbuf+outpos, stackp, i);
                    outpos += i;
                }
            }

            if ((code = free_ent) < maxmaxcode) /* Generate the new entry. */
            {
                tab_prefixof(code) = (unsigned short)oldcode;
                tab_suffixof(code) = (char_type)finchar;
                free_ent = code+1;
            }

            oldcode = incode;	/* Remember previous code.	*/
        }

        bytes_in += rsize;
    }
    while (rsize > 0);

    if (outpos > 0)
        copy_out_data(out, outpos, &out_indx);
    return (out);
}
