//
// Created by youpaw on 21/08/2021.
//

#include "load_exec.h"
#include "memory/ft_mem.h"
#include <stdlib.h>

t_exec_data *new_exec_data(const void *exec_map, size_t size, t_exec_fmt fmt)
{
	t_exec_data *exec_data;

	exec_data = malloc(sizeof(t_exec_data));
	if (!exec_data)
		return (NULL);
	exec_data->data = malloc(size);
	if (!exec_data->data)
	{
		free(exec_data);
		return (NULL);
	}
	ft_memcpy(exec_data->data, exec_map, size);
	exec_data->size = size;
	exec_data->fmt = fmt;
	return (exec_data);
}
