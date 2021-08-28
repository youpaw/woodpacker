//
// Created by youpaw on 28/08/2021.
//

#include "pack_exec.h"

void	get_map_data(t_map_data *data, void *map, size_t size)
{
	data->map = map;
	data->size = size;
}