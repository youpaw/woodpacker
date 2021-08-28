//
// Created by youpaw on 22/08/2021.
//
#include "pack_elf64.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

static off_t	map_woody_tmpl(void **map)
{
	int		fd;
	off_t	size;

	fd = open(WOODY_TMPL_ELF64, O_RDONLY);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	*map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (errno)
	{
		perror("Cannot open woody binary template");
		return (-1);
	}
	return (size);
}

int	pack_elf64(const t_map_data *exec)
{
	t_map_data tmpl;

	tmpl.size = map_woody_tmpl(&tmpl.map);
	if (errno)
		return (-1);

	munmap(tmpl.map, tmpl.size);
	return (0);
}