//
// Created by youpaw on 11/1/23.
//
#include "pack_elf32.h"
#include "memory/ft_mem.h"
#include <elf.h>
#include <unistd.h>

static void inject_payload(void *bin, t_cave_info *cave, Elf32_Phdr	*seg,
						   void *key)
{
	Elf32_Ehdr	*ehdr;
	uint32_t	tmp;
	void		*inject;

	ehdr = bin;
	inject = bin + cave->seg_off + cave->size_lz4 + cave->enc_pad;
	ft_memcpy(inject, PAYLOAD_ELF32, PAYLOAD_SIZE_ELF32);
	ft_memcpy(inject + ORIG_SIZE_OFF_ELF32, &seg->p_filesz, sizeof(uint32_t));
	ft_memcpy(inject + COMP_SIZE_OFF_ELF32, &cave->size_lz4, sizeof(uint32_t));
	tmp = cave->size_lz4 + cave->enc_pad;
	ft_memcpy(inject + ENC_SIZE_OFF_ELF32, &tmp, sizeof(uint32_t));
	ft_memcpy(inject + KEY_PART1_OFF_ELF32, key, 4);
	ft_memcpy(inject + KEY_PART2_OFF_ELF32, key + 4, 4);
	ft_memcpy(inject + KEY_PART3_OFF_ELF32, key + 8, 4);
	ft_memcpy(inject + KEY_PART4_OFF_ELF32, key + 12, 4);
	tmp = ~((uint32_t) sysconf(_SC_PAGE_SIZE)) + 1;
	ft_memcpy(inject + ALIGN_OFF_ELF32, &tmp, sizeof(uint32_t));
	ft_memcpy(&tmp, &PAYLOAD_ELF32[LOAD_ADDR_OFF_ELF32], sizeof(uint32_t));
	tmp += cave->ld_pad;
	ft_memcpy(inject + LOAD_ADDR_OFF_ELF32, &tmp, sizeof(uint32_t));
	inject += PAYLOAD_SIZE_ELF32 + cave->ld_pad;
	ft_memcpy(inject, LOAD_ELF32, LOAD_SIZE_ELF32);
	tmp = ehdr->e_entry - ((inject - bin) + JMP_ADDR_OFF_ELF32) - 4;
	ft_memcpy(inject + JMP_ADDR_OFF_ELF32, &tmp, sizeof(uint32_t));
	ehdr->e_entry = seg->p_vaddr + cave->size_lz4 + cave->enc_pad;
	ehdr->e_shoff += cave->extend;
}

static void patch_segments(void *bin, t_cave_info *cave)
{
	Elf32_Ehdr	*head;
	Elf32_Phdr	*seg;
	Elf32_Phdr	*load;
	size_t		cnt;

	load = bin + cave->ph_off;
	load->p_filesz += cave->extra;
	load->p_memsz += cave->extra;
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
	Elf32_Ehdr	*head;
	Elf32_Shdr	*sect;
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
				sect->sh_size += cave->extra;
				if (!cave->extend)
					break;
			}
		}
		else
			sect->sh_offset += cave->extend;
		cnt++;
	}
}

void assemble_woody_elf32(t_data_wrap *woody, t_cave_info *cave_info,
						  const t_data_wrap *key)
{
	Elf32_Phdr	*txt_seg;

	txt_seg = woody->data + cave_info->ph_off;
	inject_payload(woody->data, cave_info, txt_seg, key->data);
	patch_segments(woody->data, cave_info);
	patch_sections(woody->data, cave_info);
}
