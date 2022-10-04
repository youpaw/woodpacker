//
// Created by youpaw on 7/17/22.
//

#include "woodpacker.h"
#include "memory/ft_mem.h"

t_data_wrap *new_data_wrap(void *data, size_t size)
{
	t_data_wrap *wrap;

	wrap = ft_xmalloc(sizeof(t_data_wrap));
	wrap->data = data;
	wrap->size = size;
	return (wrap);
}