//
// Created by youpaw on 11/1/23.
//
#include "string/ft_str.h"
#include <elf.h>
#include <stdio.h>
#include <errno.h>

static int validate_segments(const Elf32_Ehdr *ehdr, size_t size)
{
	Elf32_Phdr	*seg;
	int			idx;

	if ((ehdr->e_phoff + ehdr->e_phentsize * ehdr->e_phnum) > size)
		goto inval_err;
	idx = 0;
	while (idx != ehdr->e_phnum)
	{
		seg = (void *) ehdr + ehdr->e_phoff + ehdr->e_phentsize * idx;
		if ((seg->p_offset + seg->p_filesz) > size)
			goto inval_err;
		idx++;
	}
	return (0);
	inval_err:
	errno = EINVAL;
	perror("Provided file is invalid");
	return (errno);
}

static int validate_sections(const Elf32_Ehdr *ehdr, size_t size)
{
	Elf32_Shdr	*sect;
	size_t		cnt;

	if ((ehdr->e_shoff + ehdr->e_shentsize * ehdr->e_shnum) > size)
		goto inval_err;
	cnt = 0;
	while (cnt != ehdr->e_shnum)
	{
		sect = (void *) ehdr + ehdr->e_shoff + ehdr->e_shentsize * cnt;
		if ((sect->sh_offset + sect->sh_size) > size)
			goto inval_err;
		cnt++;
	}
	return (0);
	inval_err:
	errno = EINVAL;
	perror("Provided file is invalid");
	return (errno);
}

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
	validate_segments(exec_map, size) || validate_sections(exec_map, size);
	return (errno);
}
