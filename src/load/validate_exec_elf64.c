//
// Created by youpaw on 21/08/2021.
//
#include "string/ft_str.h"
#include <elf.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int validate_exec_elf64(const void *exec_map, size_t size)
{
	Elf64_Ehdr *ehdr;

	if (size < SELFMAG || ft_strncmp(exec_map, ELFMAG, SELFMAG))
		return (1);
	if (size < sizeof(Elf64_Ehdr))
		return (1);
	ehdr = (Elf64_Ehdr *) exec_map;
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
		return (1);
	if (ehdr->e_type != ET_EXEC)
	{
		errno = EINVAL;
		perror("Unsupported elf type, executables only");
		return (-1);
	}
	return (0);
}
