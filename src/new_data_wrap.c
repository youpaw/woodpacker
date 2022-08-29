//
// Created by youpaw on 7/17/22.
//

#include "woodpacker.h"
#include "memory/ft_mem.h"

t_data_wrap *new_data_wrap(const void *data, size_t size)
{
	t_data_wrap *wrap;

	wrap = ft_xmalloc(sizeof(t_data_wrap));
	wrap->data = ft_xmalloc(size);
	ft_memcpy(wrap->data, data, size);
	wrap->size = size;
	return (wrap);
}