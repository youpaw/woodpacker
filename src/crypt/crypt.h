//
// Created by youpaw on 28/08/2021.
//

#ifndef CRYPT_H
#define CRYPT_H
#include <unistd.h>

typedef enum	e_aes_key
{
	k_128 = 128,
	k_192 = 192,
	k_256 = 256
}				t_aes_key;

typedef struct	s_aes_data {
	void *data;
	size_t size;
}				t_aes_data;

t_aes_data *encrypt_aes(const void *data, size_t size, const char *key, t_aes_key key_size);

#endif //CRYPT_H
