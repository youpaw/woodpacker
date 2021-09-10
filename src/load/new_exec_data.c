//
// Created by youpaw on 21/08/2021.
//

#include "load_exec.h"
#include "memory/ft_mem.h"

t_exec_data *new_exec_data(const void *exec_map, size_t size, t_exec_fmt fmt)
{
	t_exec_data *exec_data;

	exec_data = ft_xmalloc(sizeof(t_exec_data));
	exec_data->data = ft_xmalloc(size);
	ft_memcpy(exec_data->data, exec_map, size);
	exec_data->size = size;
	exec_data->fmt = fmt;
	return (exec_data);
}
