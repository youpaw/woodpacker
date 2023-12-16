//
// Created by youpaw on 11/1/23.
//

#include "pack_elf32.h"
#include "crypt.h"
#include <stdlib.h>
#include <elf.h>

t_data_wrap *pack_elf32(const t_exec_map *exec, const t_data_wrap *key)
{
	t_data_wrap *woody;
	t_cave_info *cave;
	Elf32_Phdr *exec_seg;

	cave = find_cave_elf32(exec->addr);
	if (!cave)
		return (NULL);
	woody = allocate_woody(exec, cave);
	exec_seg = woody->data + cave->ph_off;
	encrypt(key->data, woody->data + exec_seg->p_offset,
			exec_seg->p_filesz + cave->enc_pad, 1);
	assemble_woody_elf32(woody, cave, key);
	free(cave);
	return (woody);
}
