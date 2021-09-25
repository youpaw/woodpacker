//
// Created by youpaw on 12/09/2021.
//

#include "pack_elf64.h"
#include "string/ft_str.h"
#include <stdio.h>
#include <errno.h>

char *get_strtab_ptr_elf64(Elf64_Ehdr *ehdr)
{
	Elf64_Shdr	*strtab_shdr;
	char		*shstrtab;
	Elf64_Shdr	*shdr;
	size_t		cnt;

	strtab_shdr = (Elf64_Shdr *)((char *)ehdr + ehdr->e_shoff +
								 (ehdr->e_shstrndx * ehdr->e_shentsize));
	shstrtab = (char *)ehdr + strtab_shdr->sh_offset;
	shdr = (Elf64_Shdr *)((char *)ehdr + ehdr->e_shoff);
	cnt = 0;
	while (cnt < ehdr->e_shnum)
	{
		if (shdr->sh_type == SHT_STRTAB && \
			!ft_strcmp(shstrtab + shdr->sh_name, STRTAB_NAME))
			return ((char *)ehdr + shdr->sh_offset);
		shdr = (Elf64_Shdr *)((char *)shdr + ehdr->e_shentsize);
	}
	errno = EINVAL;
	perror("Cannot find string table for symbols in woody elf64 template");
	return (NULL);
}