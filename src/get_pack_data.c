//
// Created by youpaw on 27/08/2021.
//

#include "woodpacker.h"

void	get_pack_data(t_pack_data *pack_data, const t_exec_data *exec_data)
{
	pack_data->data = exec_data->data;
	pack_data->size = exec_data->size;
}