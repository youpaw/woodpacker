//
// Created by youpaw on 7/17/22.
//

#include "woodpacker.h"
#include <stdlib.h>

void		del_data_wrap(t_data_wrap **wrap)
{
	t_data_wrap *p_wrap;

	if (!wrap || !*wrap)
		return ;
	p_wrap = *wrap;
	free(p_wrap->data);
	p_wrap->data = NULL;
	free(p_wrap);
	*wrap = NULL;
}