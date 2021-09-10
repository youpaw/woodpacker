//
// Created by youpaw on 21/08/2021.
//

#include "load_exec.h"
#include <stdio.h>
#include <errno.h>

t_exec_fmt validate_exec(const void *exec_map, size_t size)
{
	static int (*handlers[N_SUPPORTED_FORMATS])(const void *, size_t) = {
			&validate_exec_elf64
	};
	t_exec_fmt	fmt;
	int			ret;

	fmt = 0;
	while (fmt < N_SUPPORTED_FORMATS)
	{
		ret = handlers[fmt](exec_map, size);
		if (!ret)
			return (fmt);
		else if(ret == -1)
			break ;
		fmt++;
	}
	if (!errno)
	{
		errno = EINVAL;
		perror("Executable format not supported, elf x86_64 only");
	}
	return (f_uns);
}
