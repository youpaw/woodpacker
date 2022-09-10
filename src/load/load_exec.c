//
// Created by youpaw on 21/08/2021.
//
#include "load_exec.h"

t_exec_map	*load_exec(const char *path)
{
	t_exec_map 	*exec;

	exec = mmap_exec(path);
	if (!exec)
		return (NULL);
	exec->fmt = validate_exec(exec->addr, exec->size);
	if (exec->fmt == f_uns)
	{
		munmap_exec(&exec);
		return (NULL);
	}
	return (exec);
}
