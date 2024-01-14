//
// Created by youpaw on 12/3/23.
//

#include "pack_exec.h"
#include <stdlib.h>

void		del_cave_info(t_cave_info **cave_info)
{
	t_cave_info *p_cave_info;

	if (!cave_info || !*cave_info)
		return;
	p_cave_info = *cave_info;
	free(p_cave_info->seg_lz4);
	free(p_cave_info);
	*cave_info = NULL;
}
