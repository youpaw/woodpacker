//
// Created by youpaw on 29/08/2021.
//

#include "pack_elf64.h"
#include "memory/ft_mem.h"
#include <elf.h>

static void inject_payload(void *bin, t_cave_info *cave)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*seg;
	Elf64_Addr	jmp;
	void		*inject;

	ehdr = bin;
	seg = bin + cave->ph_off;
	inject = bin + cave->off;
	jmp = ehdr->e_entry - (cave->off + JMP_ADDR_OFF_ELF64) - 4;
	ft_memcpy(inject, PAYLOAD_ELF64, PAYLOAD_SIZE_ELF64);
	ft_memcpy(inject + JMP_ADDR_OFF_ELF64, &jmp, sizeof(ehdr->e_entry));
	ehdr->e_entry = seg->p_vaddr + seg->p_filesz;
	ehdr->e_shoff += cave->extend;
}

static void patch_segments(void *bin, t_cave_info *cave)
{
	Elf64_Ehdr	*head;
	Elf64_Phdr	*seg;
	Elf64_Phdr	*load;
	size_t		cnt;

	load = bin + cave->ph_off;
	load->p_filesz += PAYLOAD_SIZE_ELF64;
	load->p_memsz += PAYLOAD_SIZE_ELF64;
	if (cave->extend)
	{
		head = bin;
		cnt = cave->ph_idx + 1;
		while (cnt != head->e_phnum)
		{
			seg = bin + head->e_phoff + head->e_phentsize * cnt;
			seg->p_offset += cave->extend;
			cnt++;
		}
	}
}

static void patch_sections(void *bin, t_cave_info *cave)
{
	Elf64_Ehdr	*head;
	Elf64_Shdr	*sect;
	size_t		cnt;

	head = bin;
	cnt = 0;
	while (cnt != head->e_shnum)
	{
		sect = bin + head->e_shoff + head->e_shentsize * cnt;
		if (sect->sh_offset < cave->off)
		{
			if (sect->sh_offset + sect->sh_size == cave->off)
			{
				sect->sh_size += PAYLOAD_SIZE_ELF64;
				if (!cave->extend)
					break;
			}
		}
		else
			sect->sh_offset += cave->extend;
		cnt++;
	}
}

t_data_wrap *assemble_woody_elf64(t_data_wrap *woody, t_cave_info *cave_info)
{
	inject_payload(woody->data, cave_info);
	patch_segments(woody->data, cave_info);
	patch_sections(woody->data, cave_info);
	return (0);
}