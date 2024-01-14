//
// Created by youpaw on 11/1/23.
//
#include "pack_exec.h"
#include "memory/ft_mem.h"

t_data_wrap *allocate_woody(const t_exec_map *exec, const t_cave_info *cave)
{
	t_data_wrap *woody;

	woody = ft_xmalloc(sizeof(t_data_wrap));
	woody->size = exec->size + cave->extend;
	woody->data = ft_xmalloc(woody->size);
	ft_memcpy(woody->data, exec->addr, cave->seg_off);
	ft_memcpy(woody->data + cave->seg_off, cave->seg_lz4, cave->size_lz4);
	ft_bzero(woody->data + cave->off - cave->ld_pad, cave->ld_pad);
	if (cave->extend)
	{
		ft_bzero(woody->data + cave->off, cave->extend);
		ft_memcpy(woody->data + cave->off + cave->extend,
				  exec->addr + cave->off,
				  exec->size - cave->off);
	}
	else
		ft_memcpy(woody->data + cave->off, exec->addr + cave->off,
				  exec->size - cave->off);
	ft_bzero(woody->data + cave->seg_off + cave->size_lz4, cave->enc_pad);
	return (woody);
}
