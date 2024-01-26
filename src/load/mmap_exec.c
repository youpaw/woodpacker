//
// Created by youpaw on 7/18/22.
//

#include "load_exec.h"
#include "memory/ft_mem.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

t_exec_map	*mmap_exec(const char *path)
{
	t_exec_map	*exec_map;
	void		*map;
	off_t 		size;
	int			fd;

	fd = open(path, O_RDONLY);
	if (errno)
	{
		perror("Cannot open specified executable");
		return (NULL);
	}
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (errno)
	{
		perror("Cannot map specified executable");
		return (NULL);
	}
	exec_map = ft_xmalloc(sizeof(t_exec_map));
	exec_map->addr = map;
	exec_map->size = size;
	exec_map->fmt = f_uns;
	return (exec_map);
}