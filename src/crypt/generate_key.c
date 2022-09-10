#include "memory/ft_mem.h"
#include <stdlib.h>

void	*generate_key(size_t key_size, const void *seed)
{
    void *key;
    size_t n_rounds;
    size_t round;
    int key_part;

    key = ft_xmalloc(key_size);
    n_rounds = key_size / sizeof(int);
    round = 0;

    while(round != n_rounds)
    {
        key_part = rand();
        ft_memcpy((char *)key + (sizeof(int) * round), &key_part, sizeof(int));
        round++;
    }

    return (key);
}