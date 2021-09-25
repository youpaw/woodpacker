//
// Created by youpaw on 29/08/2021.
//

#include "pack_elf64.h"
#include <stdio.h>
#include <errno.h>

static Elf64_Phdr	*get_data_segment_phdr(Elf64_Ehdr *ehdr)
{
	Elf64_Phdr	*phdr;
	size_t		nphdr;

	nphdr = 0;
	phdr = (Elf64_Phdr *) ((char *) ehdr + ehdr->e_phoff);
	while (nphdr < ehdr->e_phnum)
	{
		if (phdr->p_type == PT_LOAD && phdr->p_flags == (PF_R | PF_W))
			return (phdr);
		phdr = (Elf64_Phdr *) ((char *) phdr + ehdr->e_phentsize);
		nphdr++;
	}
	errno = EINVAL;
	perror("Cannot find data header in woody elf64 template");
	return (NULL);
}

static void	fix_shdr_offset(Elf64_Ehdr *ehdr, size_t file_size)
{
	Elf64_Shdr *str_shdr;

	str_shdr = (Elf64_Shdr *)((char *)ehdr + ehdr->e_shoff +
			ehdr->e_shentsize * ehdr->e_shstrndx);
	str_shdr->sh_offset += file_size;
	ehdr->e_shoff += file_size;
}

static void	init_inject_info(t_inject_info *inject, Elf64_Phdr *data_phdr,
								const t_map_data *exec)
{
	inject->file_off = data_phdr->p_offset + data_phdr->p_filesz;
	inject->addr = data_phdr->p_offset + data_phdr->p_memsz;
	inject->size = exec->size;
}

int	init_tmpl_elf64(const t_map_data *tmpl, const t_map_data *exec,
					   t_inject_info *inject)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*data_phdr;

	ehdr = (Elf64_Ehdr *) tmpl->map;
	data_phdr = get_data_segment_phdr(ehdr);
	if (!data_phdr)
		return (-1);
	init_inject_info(inject, data_phdr, exec);
	if (adjust_tmpl_vars_elf64(tmpl, inject))
		return (-1);
	data_phdr->p_filesz += inject->size;
	data_phdr->p_memsz += inject->size;
	fix_shdr_offset(ehdr, inject->size);
	return (0);
}