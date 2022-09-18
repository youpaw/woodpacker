#include "memory/ft_mem.h"
#include "crypt.h"
#include <fcntl.h>
#include "hash/ft_hash.h"
#include "string/ft_str.h"

t_data_wrap	*generate_key(const char *seed)
{
    t_data_wrap	*key;
	int			fd;

    key = new_data_wrap(ft_xmalloc(ENCRYPT_KEY_SIZE), ENCRYPT_KEY_SIZE);
	if (seed) {
		hash_md2(key->data, seed, ft_strlen(seed));
	}
	else {
		fd = open("/dev/urandom", O_RDONLY);
		read(fd, key->data, key->size);
		close(fd);
		hash_md2(key->data, key->data, ENCRYPT_KEY_SIZE);
	}
    return (key);
}
