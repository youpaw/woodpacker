//
// Created by youpaw on 21/08/2021.
//
#include "load_exec.h"
#include <stdlib.h>

void		del_exec_data(t_exec_data **exec_data)
{
	t_exec_data *p_exec_data;
	if (!exec_data || !*exec_data)
		return ;
	p_exec_data = *exec_data;
	free(p_exec_data->data);
	free(p_exec_data);
	*exec_data = NULL;
}