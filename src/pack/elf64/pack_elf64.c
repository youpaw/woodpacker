//
// Created by youpaw on 22/08/2021.
//
#include "pack_elf64.h"
#include "crypt.h"

t_data_wrap *pack_elf64(const t_exec_map *exec, const t_data_wrap *key)
{
	t_data_wrap *woody;
	t_cave_info *cave;

	cave = find_cave_elf64(exec->addr);
	if (!cave)
		return (NULL);
	woody = allocate_woody(exec, cave);
	encrypt(key->data, woody->data + cave->seg_off,
			cave->size_lz4 + cave->enc_pad, 0);
	assemble_woody_elf64(woody, cave, key);
	del_cave_info(&cave);
	return (woody);
}
