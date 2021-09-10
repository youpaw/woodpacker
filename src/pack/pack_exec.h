//
// Created by youpaw on 27/08/2021.
//

#ifndef PACK_EXEC_H
#define PACK_EXEC_H
#define WOODY_FILE "woody"
#include "load_exec.h"

typedef struct s_map_data {
	void	*map;
	size_t	size;
}				t_map_data;

typedef struct s_inject_info {
	size_t	file_off;
}				t_inject_info;

int		pack_exec(const t_exec_data *exec_data);
void	get_map_data(t_map_data *data, void *map, size_t size);

#endif //PACK_EXEC_H
