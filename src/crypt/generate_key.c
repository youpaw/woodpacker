#include "memory/ft_mem.h"
#include "crypt.h"
#include <stdlib.h>

t_data_wrap	*generate_key(const char *seed)
{
    t_data_wrap *key;
    size_t n_rounds;
    size_t round;
    int key_part;

    key = new_data_wrap(ft_xmalloc(ENCRYPT_KEY_SIZE), ENCRYPT_KEY_SIZE);
    n_rounds = ENCRYPT_KEY_SIZE / sizeof(int);
    round = 0;
    while(round != n_rounds)
    {
        key_part = rand();
        ft_memcpy((char *)key->data + (sizeof(int) * round), &key_part, sizeof(int));
        round++;
    }
    return (key);
}