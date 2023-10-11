//
// Created by youpaw on 29/08/2021.
//

#include "pack_elf64.h"
#include "memory/ft_mem.h"
#include <elf.h>
#include <unistd.h>

static void inject_payload(void *bin, t_cave_info *cave, void *key)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Phdr	*seg;
	uint64_t	tmp;
	void		*inject;

	ehdr = bin;
	seg = bin + cave->ph_off;
	inject = bin + cave->off + cave->seg_pad;
	ft_memcpy(inject, PAYLOAD_ELF64, PAYLOAD_SIZE_ELF64);
	tmp = seg->p_filesz + cave->seg_pad;
	ft_memcpy(inject + SEG_LENGTH_OFF_ELF64, &tmp, sizeof(uint64_t));
	ft_memcpy(inject + KEY_PART1_OFF_ELF64, key, sizeof(uint64_t));
	ft_memcpy(inject + KEY_PART2_OFF_ELF64, key + sizeof(uint64_t), sizeof(uint64_t));
	tmp = ehdr->e_entry - (cave->off + cave->seg_pad + JMP_ADDR_OFF_ELF64) - 4;
	ft_memcpy(inject + JMP_ADDR_OFF_ELF64, &tmp, sizeof(uint32_t));
	tmp = ~sysconf(_SC_PAGE_SIZE) + 1;
	ft_memcpy(inject + ALIGN_OFF_ELF64, &tmp, sizeof(uint32_t));
	ehdr->e_entry = seg->p_vaddr + seg->p_filesz + cave->seg_pad;
	ehdr->e_shoff += cave->extend;
}

static void patch_segments(void *bin, t_cave_info *cave)
{
	Elf64_Ehdr	*head;
	Elf64_Phdr	*seg;
	Elf64_Phdr	*load;
	size_t		cnt;

	load = bin + cave->ph_off;
	load->p_filesz += PAYLOAD_SIZE_ELF64 + cave->seg_pad;
	load->p_memsz += PAYLOAD_SIZE_ELF64 + cave->seg_pad;
	if (cave->extend)
	{
		head = bin;
		cnt = cave->ph_idx + 1;
		while (cnt != head->e_phnum)
		{
			seg = bin + head->e_phoff + head->e_phentsize * cnt;
			if (seg->p_offset > cave->off)
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
				sect->sh_size += PAYLOAD_SIZE_ELF64 + cave->seg_pad;
				if (!cave->extend)
					break;
			}
		}
		else
			sect->sh_offset += cave->extend;
		cnt++;
	}
}

void assemble_woody_elf64(t_data_wrap *woody, t_cave_info *cave_info,
						  const t_data_wrap *key)
{
	inject_payload(woody->data, cave_info, key->data);
	patch_segments(woody->data, cave_info);
	patch_sections(woody->data, cave_info);
}