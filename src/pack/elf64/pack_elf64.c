//
// Created by youpaw on 22/08/2021.
//
#include "pack_elf64.h"
#include "crypt.h"
#include "memory/ft_mem.h"
#include <stdlib.h>
#include <elf.h>

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

static void encrypt_woody(void *woody, t_cave_info *cave_info, void *key)
{
	Elf64_Phdr *exec_seg;

	exec_seg = woody + cave_info->ph_off;
	encrypt(key, woody + exec_seg->p_offset,
			exec_seg->p_filesz + cave_info->seg_pad);
}

t_data_wrap *pack_elf64(const t_exec_map *exec, const t_data_wrap *key)
{
	t_data_wrap *woody;
	t_cave_info *cave;

	cave = find_cave_elf64(exec->addr);
	if (!cave)
		return (NULL);
	woody = allocate_woody(exec, cave);
	encrypt_woody(woody->data, cave, key->data);
	assemble_woody_elf64(woody, cave, key);
	free(cave);
	return (woody);
}
