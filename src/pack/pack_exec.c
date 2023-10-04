//
// Created by youpaw on 27/08/2021.
//
#include "load_exec.h"
#include "pack_elf64.h"
#include <stdio.h>

int	pack_exec(const t_exec_map *exec_map, const t_data_wrap *key)
{
	static t_data_wrap *(*pack[N_SUPPORTED_FORMATS])
	(const t_exec_map *, const t_data_wrap *) = { &pack_elf64 };
	t_data_wrap *woody;

	woody = pack[exec_map->fmt](exec_map, key);
	if (!woody)
	{
		perror("Failed to pack woody");
		return (-1);
	}
	write_woody(woody);
	del_data_wrap(&woody);
	return (0);
}