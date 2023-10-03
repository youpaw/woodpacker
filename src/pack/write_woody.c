//
// Created by youpaw on 7/10/22.
//

#include "pack_exec.h"
#include <fcntl.h>
#include <stdio.h>

int		write_woody(const t_data_wrap *woody)
{
	int fd;

	fd = open(WOODY_FILE, O_WRONLY | O_CREAT | O_TRUNC,
			  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (fd == -1)
	{
		perror("Cannot create woody binary");
		return (-1);
	}
	write(fd, woody->data, woody->size);
	close(fd);
	puts("Woody executable successfully packed!\n");
	return (0);
}
