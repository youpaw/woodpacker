//
// Created by Darth Butterwell on 8/7/21.
//

#define N_SUPPORTED_FORMATS 1

#include "pack/pack_share.h"
#include <unistd.h>

typedef enum e_exec_fmt {
	f_elf_64,
	f_uns
}				t_exec_fmt;

typedef struct s_exec_data {
	void		*data;
	size_t		size;
	t_exec_fmt	fmt;
}				t_exec_data;

t_exec_data *new_exec_data(const void *exec_map, size_t size, t_exec_fmt fmt);
void		del_exec_data(t_exec_data **exec_data);

t_exec_fmt	validate_exec(const void *exec_map, size_t size);
t_exec_data	*load_exec(const char *path);
int 		pack_exec(const t_exec_data *exec_data);

void		get_pack_data(t_pack_data *pack_data, const t_exec_data *exec_data);

#ifndef WOODPACKER_H
#define WOODPACKER_H
#endif
