//
// Created by youpaw on 11/1/23.
//
#include "pack_exec.h"
#include "memory/ft_mem.h"

t_data_wrap *allocate_woody(const t_exec_map *exec, const t_cave_info *cave)
{
	t_data_wrap *woody;

	woody = ft_xmalloc(sizeof(t_data_wrap));
	woody->size = exec->size + cave->extend;
	woody->data = ft_xmalloc(woody->size);
	if (cave->extend)
	{
		ft_memcpy(woody->data, exec->addr, cave->off);
		ft_bzero(woody->data + cave->off, cave->extend);
		ft_memcpy(woody->data + cave->off + cave->extend,
				  exec->addr + cave->off,
				  exec->size - cave->off);
	}
	else
		ft_memcpy(woody->data, exec->addr, exec->size);
	return (woody);
}
