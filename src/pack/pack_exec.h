//
// Created by youpaw on 27/08/2021.
//

#ifndef PACK_EXEC_H
#define PACK_EXEC_H
#define WOODY_FILE "woody"
#include "woodpacker.h"
#include "load_exec.h"

typedef struct	s_cave_info {
	size_t off;
	size_t size;
	size_t extend;
	size_t phoff;
}				t_cave_info;

t_cave_info	*find_cave_elf64(const void *bin);
int			pack_exec(const t_exec_map *exec_map);
int			write_woody(const t_data_wrap *woody);
#endif //PACK_EXEC_H
