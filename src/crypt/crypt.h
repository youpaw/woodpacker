//
// Created by youpaw on 28/08/2021.
//

#ifndef CRYPT_H
#define CRYPT_H
#define ENCRYPT_KEY_SIZE 16
#define ENCRYPT_ALIGN ENCRYPT_KEY_SIZE

#include <stddef.h>

void	encrypt(void *key, void *data, size_t size);
void	*generate_key(size_t key_size, const void *seed);


#endif //CRYPT_H
