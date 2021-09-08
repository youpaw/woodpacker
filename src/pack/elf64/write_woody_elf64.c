//
// Created by youpaw on 29/08/2021.
//

#include "pack_elf64.h"
#include <fcntl.h>
#include <stdio.h>

int			write_woody_elf64(const t_map_data *tmpl, const t_map_data *exec,
								 t_inject_info *inject)
{
	int fd;

	fd = open(WOODY_FILE, O_WRONLY | O_CREAT | O_TRUNC,
			  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (fd == -1)
	{
		perror("Cannot create woody binary");
		return (-1);
	}
	write(fd, tmpl->map, inject->file_off);
	write(fd, exec->map, exec->size);
	write(fd, tmpl->map + inject->file_off, tmpl->size - inject->file_off);
	close(fd);
	return (0);
}