//
// Created by youpaw on 12/09/2021.
//

#include "pack_elf64.h"
#include <stdio.h>
#include <errno.h>

Elf64_Shdr *get_symtab_shdr_elf64(Elf64_Ehdr *ehdr)
{
	Elf64_Shdr *shdr;
	size_t cnt;

	shdr = (Elf64_Shdr *) ((char *)ehdr + ehdr->e_shoff);
	cnt = 0;
	while (cnt < ehdr->e_shnum)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			return (shdr);
		shdr = (Elf64_Shdr *)((char *)shdr + ehdr->e_shentsize);
		cnt++;
	}
	errno = EINVAL;
	perror("Cannot find symbol table header in woody elf64 template");
	return (NULL);
}