//
// Created by youpaw on 7/18/22.
//

#include "load_exec.h"
#include <sys/mman.h>
#include <stdlib.h>

void munmap_exec(t_exec_map **exec_map)
{
	t_exec_map *p_exec_map;

	p_exec_map = *exec_map;
	munmap(p_exec_map->addr, p_exec_map->size);
	free(p_exec_map);
	*exec_map = NULL;
}