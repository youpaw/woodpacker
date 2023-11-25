//
// Created by youpaw on 22/08/2021.
//
#include "pack_elf64.h"
#include "crypt.h"
#include <stdlib.h>
#include <elf.h>

t_data_wrap *pack_elf64(const t_exec_map *exec, const t_data_wrap *key)
{
	t_data_wrap *woody;
	t_cave_info *cave;
	Elf64_Phdr	*exec_seg;

	cave = find_cave_elf64(exec->addr);
	if (!cave)
		return (NULL);
	woody = allocate_woody(exec, cave);
	exec_seg = woody->data + cave->ph_off;
	encrypt(key->data, woody->data + exec_seg->p_offset,
			exec_seg->p_filesz + cave->seg_pad, 0);
	assemble_woody_elf64(woody, cave, key);
	free(cave);
	return (woody);
}
