//
// Created by youpaw on 11/1/23.
//
#include "string/ft_str.h"
#include <elf.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int validate_exec_elf32(const void *exec_map, size_t size)
{
	Elf32_Ehdr *ehdr;

	if (size < SELFMAG || ft_strncmp(exec_map, ELFMAG, SELFMAG))
		return (1);
	if (size < sizeof(Elf32_Ehdr))
		return (1);
	ehdr = (Elf32_Ehdr *) exec_map;
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS32)
		return (1);
	if (!(ehdr->e_type == ET_EXEC || ehdr->e_type == ET_DYN))
	{
		errno = EINVAL;
		perror("Unsupported elf type, executables only");
	}
	else if (ehdr->e_machine != EM_386)
	{
		errno = EINVAL;
		perror("Unsupported architecture");
	}
	return (errno);
}
