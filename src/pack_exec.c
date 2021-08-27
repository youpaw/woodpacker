//
// Created by youpaw on 27/08/2021.
//
#include "woodpacker.h"
#include "pack_elf64.h"

int	pack_exec(const t_exec_data *exec_data)
{
	static int (*pack[N_SUPPORTED_FORMATS])(const t_pack_data *) = {
			&pack_elf64 };
	t_pack_data pack_data;

	get_pack_data(&pack_data, exec_data);
	return (pack[exec_data->fmt](&pack_data));
}