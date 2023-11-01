//
// Created by youpaw on 11/1/23.
//

#ifndef COMPRESS_H
#define COMPRESS_H
#include <unistd.h>

size_t	compress(const void *in, void *out, size_t size);
void	*decompress(const void *in, size_t comp, size_t orig);

#endif //COMPRESS_H
