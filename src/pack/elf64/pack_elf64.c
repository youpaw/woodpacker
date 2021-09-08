//
// Created by youpaw on 22/08/2021.
//
#include "pack_elf64.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

static off_t	map_woody_tmpl(t_map_data *tmpl)
{
	int		fd;
	off_t	size;

	fd = open(WOODY_TMPL_ELF64, O_RDONLY);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	tmpl->map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	tmpl->size = size;
	if (errno)
	{
		perror("Cannot open woody binary template");
		return (-1);
	}
	return (0);
}

int	pack_elf64(const t_map_data *exec)
{
	int			ret;
	t_map_data	tmpl;
	t_inject_info inject;

	if (map_woody_tmpl(&tmpl) == -1)
		return (-1);
	ret = adjust_tmpl_elf64(&tmpl, exec, &inject);
	if (!ret)
		ret = write_woody_elf64(&tmpl, exec, &inject);
	munmap(tmpl.map, tmpl.size);
	return (ret);
}
