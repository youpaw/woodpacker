//
// Created by youpaw on 21/08/2021.
//
#include "woodpacker.h"
#include "ft_mem.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static off_t	map_exec(const char *path, void **exec_map)
{
	off_t 		size;
	int			fd;

	fd = open(path, O_RDONLY);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	*exec_map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (errno)
	{
		perror("Cannot open specified executable");
		return (-1);
	}
	return (size);
}

t_exec_data		*load_exec(const char *path)
{
	t_exec_data	*exec_data;
	t_exec_fmt	fmt;
	void		*exec_map;
	off_t 		size;

	size = map_exec(path, &exec_map);
	if (size == -1)
		return (NULL);
	fmt = validate_exec(exec_map, size);
	if (fmt == f_uns)
		return (NULL);
	exec_data = new_exec_data(exec_map, size, fmt);
	munmap(exec_map, size);
	return (exec_data);
}
