//
// Created by youpaw on 29/08/2021.
//

#include "pack_elf64.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <elf.h>

static Elf64_Phdr	*get_text_segment_phdr(Elf64_Ehdr *ehdr)
{
	Elf64_Phdr	*phdr;
	size_t		nphdr;

	nphdr = 0;
	phdr = (Elf64_Phdr *) ((char *) ehdr + ehdr->e_phoff);
	while (nphdr < ehdr->e_phnum)
	{
		if (phdr->p_type == PT_LOAD && phdr->p_flags == (PF_R | PF_X))
			return (phdr);
		phdr = (Elf64_Phdr *) ((char *) phdr + ehdr->e_phentsize);
		nphdr++;
	}
	errno = EINVAL;
	perror("Cannot find text segment header in provided elf64 executable");
	return (NULL);
}

t_data_wrap *assemble_woody_elf64(t_exec_map *exec)
{
//	Elf64_Ehdr	*ehdr;
//	Elf64_Phdr	*data_phdr;
//
//	ehdr = (Elf64_Ehdr *) exec->data;
//	data_phdr = get_data_segment_phdr(ehdr);
//	if (!data_phdr)
//		return (-1);
//	init_inject_info(inject, data_phdr, exec);
//	if (adjust_tmpl_vars_elf64(tmpl, inject))
//		return (-1);
//	data_phdr->p_filesz += inject->size;
//	data_phdr->p_memsz += inject->size;
//	fix_shdr_offset(ehdr, inject->size);
	return (0);
}