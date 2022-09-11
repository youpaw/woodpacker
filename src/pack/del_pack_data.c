//
// Created by youpaw on 9/10/22.
//

#include "pack_exec.h"
#include <stdlib.h>

void del_pack_data(t_pack_data **pack)
{
	t_pack_data *p_pack;

	if (!pack || !*pack)
		return;
	p_pack = *pack;
	del_data_wrap(&p_pack->woody);
	del_data_wrap(&p_pack->key);
	free(p_pack);
	*pack = NULL;
}