//
// Created by Darth Butterwell on 8/7/21.
//

#ifndef WOODPACKER_H
#define WOODPACKER_H

#include <unistd.h>

typedef struct s_data_wrap {
	void		*data;
	size_t		size;
}				t_data_wrap;

t_data_wrap *new_data_wrap(const void *data, size_t size);
void		del_data_wrap(t_data_wrap **wrap);

size_t		align(size_t insert, size_t current, size_t align);
void		print_hex(void *data, size_t size);
#endif
