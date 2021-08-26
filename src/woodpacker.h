//
// Created by Darth Butterwell on 8/7/21.
//

#define N_SUPPORTED_FORMATS 1

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

#ifndef WOODPACKER_H
#define WOODPACKER_H
#endif
