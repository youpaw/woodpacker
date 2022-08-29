//
// Created by youpaw on 8/29/22.
//

#include "pack_elf64.h"
#include "memory/ft_mem.h"
#include <elf.h>

static int	find_text_segment(Elf64_Phdr **txt_seg, const Elf64_Ehdr *ehdr)
{
	Elf64_Phdr *seg;
	int idx;

	idx = 0;
	while (idx != ehdr->e_phnum)
	{
		seg = (void *) ehdr + ehdr->e_phoff + ehdr->e_phentsize * idx;
		if (seg->p_type == PT_LOAD
			&& (seg->p_flags & PF_X) && (seg->p_flags & PF_R))
		{
			*txt_seg = seg;
			return (idx);
		}
		idx++;
	}
	*txt_seg = NULL;
	return (-1);
}

t_cave_info	*find_cave_elf64(const void *bin)
{
	const Elf64_Ehdr	*ehdr;
	t_cave_info			*cave;
	Elf64_Phdr			*txt_seg;
	size_t				txt_seg_idx;

	cave = ft_xmalloc(sizeof(t_cave_info));
	cave->size = sizeof(t_payload_elf64);
	ehdr = bin;
	txt_seg_idx = find_text_segment(&txt_seg, ehdr);
	Elf64_Phdr *next = (txt_seg + 1);
	size_t pad = (txt_seg + 1)->p_offset - txt_seg->p_offset - txt_seg->p_filesz;
	if (txt_seg_idx < ehdr->e_phnum
	&& cave->size < ((txt_seg + 1)->p_offset - txt_seg->p_offset - txt_seg->p_filesz))
	{
		cave->extend = 0;
		cave->off = txt_seg->p_offset - txt_seg->p_filesz;
	}
	return (cave);
}