//
// Created by youpaw on 27/08/2021.
//
#include "load_exec.h"
#include "pack_elf64.h"

int	pack_exec(const t_exec_data *exec_data)
{
	static int (*pack[N_SUPPORTED_FORMATS])(t_map_data *) = {
			&pack_elf64 };
	t_map_data exec;

	get_map_data(&exec, exec_data->data, exec_data->size);
	return (pack[exec_data->fmt](&exec));
}