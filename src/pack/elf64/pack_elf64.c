//
// Created by youpaw on 22/08/2021.
//
#include "pack_elf64.h"
#include "memory/ft_mem.h"
#include <stdlib.h>

static t_data_wrap *allocate_woody(const t_exec_map *exec,
								   const t_cave_info *cave)
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

t_pack_data	*pack_elf64(const t_exec_map *exec)
{
	t_pack_data *pack;
	t_cave_info *cave;

	cave = find_cave_elf64(exec->addr);
	if (!cave)
		return (NULL);
	pack = new_pack_data(allocate_woody(exec, cave), NULL);
	assemble_woody_elf64(pack->woody, cave);
	free(cave);
	return (pack);
}
