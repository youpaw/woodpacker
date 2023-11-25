//
// Created by youpaw on 28/08/2021.
//

#ifndef LOAD_EXEC_H
#define LOAD_EXEC_H
#define N_SUPPORTED_FORMATS 2
#include "woodpacker.h"
#include <unistd.h>

typedef enum	e_exec_fmt {
	f_elf_64,
	f_elf_32,
	f_uns
}				t_exec_fmt;

typedef struct	s_exec_map{
	void		*addr;
	size_t		size;
	t_exec_fmt	fmt;
}				t_exec_map;

t_exec_map *mmap_exec(const char *path);
void		munmap_exec(t_exec_map **exec);

int			validate_exec_elf64(const void *exec_map, size_t size);
int			validate_exec_elf32(const void *exec_map, size_t size);
t_exec_fmt	validate_exec(const void *exec_map, size_t size);
t_exec_map	*load_exec(const char *path);
#endif //LOAD_EXEC_H
