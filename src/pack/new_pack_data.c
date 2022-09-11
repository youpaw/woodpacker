//
// Created by youpaw on 9/10/22.
//
#include "pack_exec.h"
#include "ft_mem.h"

t_pack_data	*new_pack_data(t_data_wrap *woody, t_data_wrap *key)
{
	t_pack_data *pack;

	pack = ft_xmalloc(sizeof(t_pack_data));
	pack->woody = woody;
	pack->key = key;
	return (pack);
}