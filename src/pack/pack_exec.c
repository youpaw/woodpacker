//
// Created by youpaw on 27/08/2021.
//
#include "load_exec.h"
#include "pack_elf64.h"
#include <stdio.h>

int	pack_exec(const t_exec_map *exec_map)
{
	static t_pack_data *(*pack[N_SUPPORTED_FORMATS])(const t_exec_map *) = {
			&pack_elf64 };
	t_pack_data *pack_data;

	pack_data = pack[exec_map->fmt](exec_map);
	if (!pack_data)
	{
		perror("Failed to pack woody");
		return (-1);
	}
	write_woody(pack_data->woody);
	del_pack_data(&pack_data);
	return (0);
}