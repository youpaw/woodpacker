//
// Created by youpaw on 29/08/2021.
//

#include "pack_elf64.h"
#include "memory/ft_mem.h"
#include <elf.h>

static void inject_payload(void *bin, t_cave_info *cave_info)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*seg;
	Elf64_Addr	jmp;
	void		*cave;

	ehdr = bin;
	seg = bin + cave_info->phoff;
	cave = bin + cave_info->off;
	jmp = ehdr->e_entry;
	ft_memcpy(cave, PAYLOAD_ELF64, PAYLOAD_SIZE_ELF64);
	ft_memcpy( cave + JMP_ADDR_OFF_ELF64, &jmp, sizeof(ehdr->e_entry));
	ehdr->e_entry = seg->p_vaddr + seg->p_filesz;
	seg->p_filesz += PAYLOAD_SIZE_ELF64;
	seg->p_memsz += PAYLOAD_SIZE_ELF64;
}



t_data_wrap *assemble_woody_elf64(t_data_wrap *woody, t_cave_info *cave_info)
{
	inject_payload(woody->data, cave_info);
	return (0);
}